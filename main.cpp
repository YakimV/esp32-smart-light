#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Update.h>
#include <WireGuard-ESP32.h>

// ================= HARDWARE CONFIGURATION =================
#define NUM_CHANNELS 6

// 6-Channel LED PWM pins
const int pwmPins[NUM_CHANNELS]   = { 5,  6,  7, 15, 17, 18 };
// 6-Channel MOSFET / Relay control pins
const int relayPins[NUM_CHANNELS] = { 4,  8,  9, 10, 11, 12 };

const int frequency  = 2000;   // 2 kHz (avoids coil whine and visual flicker)
const int resolution = 10;     // 10-bit resolution (0-1023)

float currentPwmPct[NUM_CHANNELS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
bool updateRequired[NUM_CHANNELS] = { true, true, true, true, true, true };
uint8_t stateIndex[NUM_CHANNELS]  = { 0, 0, 0, 0, 0, 0 };

// Relay power cutoff delay (anti-chatter)
const uint32_t RELAY_OFF_DELAY_MS = 10000; // 10s cutoff delay
uint32_t relayOffTimestamp[NUM_CHANNELS] = { 0, 0, 0, 0, 0, 0 };
bool relayPendingOff[NUM_CHANNELS]       = { false, false, false, false, false, false };

// Brightness stepping presets (%)
float pwmPresets[4] = { 25.0f, 50.0f, 75.0f, 100.0f };

// ================= RF 433 MHz (EV1527) CONFIGURATION =================
#define RF_RX_PIN 16

const uint32_t RF_SWITCHES[NUM_CHANNELS] = {
  0xA20551, // Switch 1 -> Channel 1
  0xA03761, // Switch 2 -> Channel 2
  0xA085A1, // Switch 3 -> Channel 3
  0xA08381, // Switch 4 -> Channel 4
  0xA203C1, // Switch 5 -> Channel 5
  0xA18271  // Switch 6 -> Channel 6
};

// RF ISR decoding variables
volatile uint32_t shiftRegister = 0;
volatile uint8_t bitCount = 0;
volatile uint32_t lastChangeTime = 0;
volatile uint32_t capturedCode = 0;
volatile bool newCodeReady = false;
volatile bool isSynced = false;

// 2-Match noise filter variables
uint32_t candidateCode = 0;
uint32_t candidateTime = 0;
uint32_t lastConfirmedCode = 0;
uint32_t lastConfirmedTime = 0;

// ================= NETWORK & MQTT CONFIGURATION =================
const char* ssid     = "YOUR_WIFI_SSID";
const char* pass     = "YOUR_WIFI_PASSWORD";

const char* mqtt_srv  = "10.0.0.1";
const int   mqtt_port = 1883;
const char* mqtt_user = "LedControl";
const char* mqtt_pass = "LedPass";

String topicStatus[NUM_CHANNELS];
String topicCommands[NUM_CHANNELS];

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ================= WIREGUARD VPN CONFIGURATION =================
const char* wg_local_ip    = "10.0.0.38";
const char* wg_private_key = "CLIENT_PRIVATE_KEY_BASE64=";
const char* wg_public_key  = "SERVER_PUBLIC_KEY_BASE64=";
const char* wg_endpoint    = "vpn.yourserver.com";
const int   wg_port        = 51820;

WireGuard wg;
bool vpnConnected = false;

// ================= WEB SERVER FOR OTA UPDATES =================
WebServer server(80);
bool isUpdating = false;

const char* serverIndex = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>ESP32 Firmware Update</title>
    <style>
        body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
        .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
        h2 { margin: 0 0 20px 0; color: #111; font-size: 20px; }
        input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 14px; }
        input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 14px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; }
        input[type=submit]:active { background: #218838; }
    </style>
</head>
<body>
    <div class='card'>
        <h2>6-Ch LED Controller OTA</h2>
        <form method='POST' action='/update' enctype='multipart/form-data'>
            <input type='file' name='update' required accept='.bin'>
            <input type='submit' value='Upload Firmware'>
        </form>
    </div>
</body>
</html>
)html";

uint32_t periodStatusIdleMs = 10000;
uint32_t lastStatus = 0;
uint32_t lastLog = 0;

void forceImmediateStatusPublish() { lastStatus = 0; }

// ================= RF 433 MHz ISR DECODER =================
void IRAM_ATTR rfISR() {
  uint32_t now = micros();
  uint32_t duration = now - lastChangeTime;
  lastChangeTime = now;

  if (duration < 100) return; // Glitch filter

  bool currentState = digitalRead(RF_RX_PIN);

  if (currentState == HIGH) {
    if (duration >= 2500 && duration <= 12000) {
      isSynced = true;
      bitCount = 0;
      shiftRegister = 0;
    } else if (duration > 12000) {
      isSynced = false;
    }
  } else {
    if (isSynced) {
      if (duration >= 150 && duration <= 1000) {
        shiftRegister <<= 1;
        if (duration > 480) {
          shiftRegister |= 1;
        }
        bitCount++;

        if (bitCount == 24) {
          capturedCode = shiftRegister;
          newCodeReady = true;
          isSynced = false;
        }
      } else {
        isSynced = false;
        bitCount = 0;
      }
    }
  }
}

// ================= STATUS PUBLISHING =================
void publishStatus(int ch) {
  if (isUpdating || ch < 0 || ch >= NUM_CHANNELS) return;

  JsonDocument d;
  d["pwm_pct"] = round(currentPwmPct[ch] * 10.0f) / 10.0f;
  d["relay"]   = (digitalRead(relayPins[ch]) == HIGH);

  String out;
  serializeJson(d, out);

  if (mqttClient.publish(topicStatus[ch].c_str(), out.c_str(), false)) {
    Serial.printf("[MQTT OUT] Led%d -> %s\n", ch + 1, out.c_str());
  } else {
    Serial.printf("[MQTT OUT] Failed to send status for Led%d\n", ch + 1);
  }
}

void publishAllStatuses() {
  for (int i = 0; i < NUM_CHANNELS; i++) {
    publishStatus(i);
  }
  lastStatus = millis();
}

// ================= RF SWITCH HANDLER =================
void processRfSwitchClick(uint32_t code) {
  for (int i = 0; i < NUM_CHANNELS; i++) {
    if (code == RF_SWITCHES[i]) {
      stateIndex[i] = (stateIndex[i] + 1) % 5; // Cycles: 0 (Off), 1..4 (Presets)
      currentPwmPct[i] = (stateIndex[i] == 0) ? 0.0f : pwmPresets[stateIndex[i] - 1];
      updateRequired[i] = true;

      Serial.printf("[RF BUTTON] Switch %d pressed -> State: %d | Duty: %.1f%%\n",
                    i + 1, stateIndex[i], currentPwmPct[i]);

      publishStatus(i);
      return;
    }
  }
}

// ================= MQTT INBOUND CALLBACK =================
void callback(char* topic, byte* payload, unsigned int len) {
  String msg = "";
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];

  Serial.printf("\n[MQTT IN] Topic: %s | Payload: %s\n", topic, msg.c_str());

  if (isUpdating) return;

  String topicStr = String(topic);

  for (int i = 0; i < NUM_CHANNELS; i++) {
    if (topicStr == topicCommands[i]) {
      JsonDocument d;
      DeserializationError err = deserializeJson(d, msg);

      if (!err) {
        bool presetsUpdated = false;
        if (!d["set_pwm_1"].isNull()) { pwmPresets[0] = d["set_pwm_1"].as<float>(); presetsUpdated = true; }
        if (!d["set_pwm_2"].isNull()) { pwmPresets[1] = d["set_pwm_2"].as<float>(); presetsUpdated = true; }
        if (!d["set_pwm_3"].isNull()) { pwmPresets[2] = d["set_pwm_3"].as<float>(); presetsUpdated = true; }
        if (!d["set_pwm_4"].isNull()) { pwmPresets[3] = d["set_pwm_4"].as<float>(); presetsUpdated = true; }

        if (presetsUpdated) {
          Serial.printf("[PRESETS] Updated: [1]: %.1f%% | [2]: %.1f%% | [3]: %.1f%% | [4]: %.1f%%\n",
                        pwmPresets[0], pwmPresets[1], pwmPresets[2], pwmPresets[3]);
        }

        if (!d["pwm_pct"].isNull()) {
          float pct = d["pwm_pct"].as<float>();
          if (pct >= 0.0f && pct <= 100.0f) {
            currentPwmPct[i] = pct;
            updateRequired[i] = true;
            publishStatus(i);
          }
        }
      } else {
        float pct = msg.toFloat();
        if (pct >= 0.0f && pct <= 100.0f) {
          currentPwmPct[i] = pct;
          updateRequired[i] = true;
          publishStatus(i);
        }
      }
      break;
    }
  }
}

// ================= NETWORK CONNECTIONS =================
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.printf("Connecting to Wi-Fi: %s ", ssid);
  WiFi.begin(ssid, pass);

  int timeoutCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeoutCounter++;
    if (timeoutCounter > 40) {
      Serial.println("\n[Wi-Fi] Connection timeout!");
      return;
    }
  }
  Serial.printf("\n[Wi-Fi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
}

void connectVPN() {
  if (vpnConnected) return;

  connectWiFi();

  Serial.println("[WireGuard] Syncing time via NTP...");
  configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  int attempts = 0;
  while (time(nullptr) < 1000000000l && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (time(nullptr) >= 1000000000l) {
    Serial.printf("\n[NTP] Synchronized. Epoch: %ld\n", time(nullptr));
  }

  Serial.println("[WireGuard] Initializing tunnel...");
  if (wg.begin(
        IPAddress(10, 0, 0, 38),
        wg_private_key,
        wg_endpoint,
        wg_public_key,
        wg_port
      )) {
    Serial.printf("[WireGuard] Tunnel established. Virtual IP: %s\n", wg_local_ip);
    vpnConnected = true;
  } else {
    Serial.println("[WireGuard] Initialization failed.");
    vpnConnected = false;
  }
}

void connectMQTT() {
  if (isUpdating) return;

  while (!mqttClient.connected()) {
    connectVPN();

    String clientId = "ESP32-6CH-LED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.printf("[MQTT] Connecting to broker (%s)...\n", mqtt_srv);

    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus[0].c_str(), 0, false, "{\"state\":\"offline\"}")) {
      for (int i = 0; i < NUM_CHANNELS; i++) {
        mqttClient.subscribe(topicCommands[i].c_str());
      }
      forceImmediateStatusPublish();
      Serial.println("[MQTT] Connected and subscribed.");
    } else {
      Serial.printf("[MQTT] Connection failed, rc=%d. Retrying in 2s...\n", mqttClient.state());
      delay(2000);
    }
  }
}

void setupWebOTA() {
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    String responseHtml;
    if (Update.hasError()) {
      responseHtml = "<html><body style='font-family:sans-serif;text-align:center;padding:50px;'><h2 style='color:red;'>Update Failed!</h2></body></html>";
    } else {
      responseHtml = "<html><body style='font-family:sans-serif;text-align:center;padding:50px;'><h2 style='color:green;'>Success!</h2><p>Rebooting...</p></body></html>";
    }
    server.send(200, "text/html", responseHtml);
    delay(1200);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      isUpdating = true;
      Serial.println("[OTA] Update started...");
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.println("[OTA] Update completed successfully.");
      } else {
        Update.printError(Serial);
      }
      isUpdating = false;
    }
  });

  server.begin();
  Serial.println("[OTA] Web Server started on port 80.");
}

// ================= MAIN SETUP & LOOP =================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== 6-Channel Hybrid LED Controller Initializing ===");

  for (int i = 0; i < NUM_CHANNELS; i++) {
    topicStatus[i]   = "smartHome/user1/Led" + String(i + 1) + "/status";
    topicCommands[i] = "smartHome/user1/Led" + String(i + 1) + "/commands";

    ledcSetup(i, frequency, resolution);
    ledcAttachPin(pwmPins[i], i);

    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }

  pinMode(RF_RX_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RF_RX_PIN), rfISR, CHANGE);

  mqttClient.setServer(mqtt_srv, mqtt_port);
  mqttClient.setCallback(callback);
  mqttClient.setBufferSize(1024);

  connectWiFi();
  setupWebOTA();
  connectMQTT();
}

void loop() {
  server.handleClient();
  if (isUpdating) return;

  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  uint32_t now = millis();

  // RF 433 MHz frame validation (2-Match filter)
  if (newCodeReady) {
    uint32_t code = capturedCode;
    newCodeReady = false;

    if (code == candidateCode && (now - candidateTime < 350)) {
      if (code != lastConfirmedCode || (now - lastConfirmedTime > 400)) {
        processRfSwitchClick(code);
        lastConfirmedCode = code;
        lastConfirmedTime = now;
      }
    } else {
      candidateCode = code;
      candidateTime = now;
    }
  }

  // Channel processing: PWM & Delayed Relay Cutoff
  for (int i = 0; i < NUM_CHANNELS; i++) {
    if (updateRequired[i]) {
      updateRequired[i] = false;

      int targetDuty = (int)((currentPwmPct[i] / 100.0f) * 1023.0f);
      targetDuty = constrain(targetDuty, 0, 1023);
      ledcWrite(i, targetDuty);

      if (currentPwmPct[i] > 0.0f) {
        digitalWrite(relayPins[i], HIGH);
        relayPendingOff[i] = false;
      } else {
        if (digitalRead(relayPins[i]) == HIGH && !relayPendingOff[i]) {
          relayPendingOff[i] = true;
          relayOffTimestamp[i] = now;
        }
      }
    }

    if (relayPendingOff[i] && (now - relayOffTimestamp[i] >= RELAY_OFF_DELAY_MS)) {
      relayPendingOff[i] = false;
      digitalWrite(relayPins[i], LOW);
      publishStatus(i);
    }
  }

  if (now - lastStatus >= periodStatusIdleMs) {
    publishAllStatuses();
  }

  if (now - lastLog > 5000) {
    lastLog = now;
    Serial.printf("[HEARTBEAT] WiFi: %s | VPN: %s | MQTT: %s\n",
                  (WiFi.status() == WL_CONNECTED) ? "UP" : "DOWN",
                  vpnConnected ? "UP" : "DOWN",
                  mqttClient.connected() ? "CONNECTED" : "OFFLINE");
  }

  delay(10);
}
