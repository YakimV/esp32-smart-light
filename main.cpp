// #include<Arduino.h>


// // Визначаємо піні відповідно до вашої схеми
// const int potPin = 7;  // GPIO 7 — аналоговий вхід для потенціометра
// const int pwmPin = 5;  // GPIO 5 — вихід для ШИМ

// // Налаштування ШИМ для старої версії ядра
// const int pwmChannel = 0;    // Вибираємо канал ШИМ (можна від 0 до 15)
// const int frequency = 20000; // Частота 20 кГц (висока частота)
// const int resolution = 10;   // 10-бітна роздільна здатність (заповнення 0-1023)

// void setup() {
//   // 1. Налаштовуємо параметри ШИМ-каналу (номер каналу, частота, роздільна здатність)
//   ledcSetup(pwmChannel, frequency, resolution);
  
//   // 2. Прив'язуємо наш 5-й пін до налаштованого каналу
//   ledcAttachPin(pwmPin, pwmChannel);

//   pinMode(potPin, INPUT);
// }

// void loop() {
//   // Читаємо потенціометр (12 біт: від 0 до 4095)
//   int potValue = analogRead(potPin);

//   // Масштабуємо у 10 біт (від 0 до 1023)
//   int dutyCycle = map(potValue, 0, 4095, 0, 1023);

//   // Змінюємо заповнення. 
//   // УВАГА: тут ми передаємо НОМЕР КАНАЛУ (pwmChannel), а не номер піна!
//   ledcWrite(pwmChannel, dutyCycle);

//   delay(15);
// }


// #include<Arduino.h>


// // Визначаємо піні відповідно до вашої схеми
// const int potPin = 7;  // GPIO 7 — аналоговий вхід для потенціометра
// const int pwmPin = 5;  // GPIO 5 — вихід для ШИМ

// // Нові налаштування під ваші вимоги
// const int pwmChannel = 0;     
// const int frequency = 100000; // Частота 100 кГц
// const int resolution = 10;     // 8-бітна роздільна здатність (заповнення 0-255)

// void setup() {
//   // Налаштовуємо канал на 100 кГц та 8 біт
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);

//   pinMode(potPin, INPUT);
// }

// void loop() {
//   // Читаємо потенціометр (12 біт: від 0 до 4095)
//   int potValue = analogRead(potPin);

//   // Класний трюк замість функції map():
//   // Оскільки нам треба перевести 12 біт у 8 біт, ми просто зсуваємо значення 
//   // на 4 біти вправо (ділимо на 16). Це працює миттєво і чистить дрібний шум.
//   int dutyCycle = potValue >> 4; 

//   // Записуємо 8-бітне заповнення (0-255) у наш ШИМ-канал
//   ledcWrite(pwmChannel, dutyCycle);

//   delay(10); // Трохи зменшили затримку для кращого відгуку
// }
















// #include <Arduino.h>

// const int potPin = 7;  
// const int pwmPin = 5;  

// const int pwmChannel = 0;     
// const int frequency = 20000;  
// const int resolution = 10;    // 10 біт (0-1023)

// // Змінна для зберігання попереднього стабільного значення
// int lastDutyCycle = -1;

// // Поріг чутливості (мертва зона). 
// // Зміна має бути більшою за це значення, щоб код зреагував.
// const int noiseThreshold = 5; 

// void setup() {
//   Serial.begin(115200);
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);
//   pinMode(potPin, INPUT);
// }

// void loop() {
//   int potValue = analogRead(potPin);

//   // 1. Грубе перетворення: зсуваємо на 2 біти для 10-бітної роздільної здатності
//   int rawDuty = potValue >> 2; 

//   // 2. Фільтр мертвої зони (гістерезис):
//   // Перевіряємо, чи поточне значення суттєво відрізняється від попереднього.
//   // Також додаємо перевірку на крайні точки (0 та 1023), щоб чітко доходити до мінімуму/максимуму.
//   if (abs(rawDuty - lastDutyCycle) > noiseThreshold || rawDuty == 0 || rawDuty == 1023) {
    
//     // Якщо зміна реальна — оновлюємо значення
//     lastDutyCycle = rawDuty;

//     // Записуємо стабільне заповнення в ШИМ
//     ledcWrite(pwmChannel, lastDutyCycle);

//     // Виводимо в Serial тільки коли значення дійсно змінилося
//     float percent = (lastDutyCycle / 1023.0) * 100.0;
//     Serial.print("Duty Cycle: ");
//     Serial.print(lastDutyCycle);
//     Serial.print(" / 1023 (");
//     Serial.print(percent, 1);
//     Serial.println("%)");
//   }

//   delay(50); // Можна повернути швидший відгук, оскільки спаму в Serial більше немає
// }










// #include <Arduino.h>

// const int potPin = 7;  
// const int pwmPin = 5;  

// const int pwmChannel = 0;     
// const int frequency = 20000;  
// const int resolution = 8;     // Перейшли на 8 біт (0-255)

// // Налаштування фільтрації
// const int numReadings = 32;    // Кількість вимірів для усереднення (чим більше, тим плавніший відгук)
// int readings[numReadings];     // Масив для зберігання останніх значень
// int readIndex = 0;             // Індекс поточного виміру
// long total = 0;                // Сума вимірів

// int lastDutyCycle = -1;
// const int noiseThreshold = 3;  // Поріг для 8 біт (3 одиниці з 255 — це досить грубо і надійно)

// void setup() {
//   Serial.begin(115200);
  
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);
//   pinMode(potPin, INPUT);

//   // Заповнюємо масив початковим значенням, щоб старт був плавним
//   int startValue = analogRead(potPin);
//   for (int i = 0; i < numReadings; i++) {
//     readings[i] = startValue;
//   }
//   total = (long)startValue * numReadings;
// }

// void loop() {
//   // 1. Алгоритм ковзного середнього (Moving Average)
//   total = total - readings[readIndex];       // Віднімаємо старе значення
//   readings[readIndex] = analogRead(potPin);  // Читаємо нове значення з АЦП
//   total = total + readings[readIndex];       // Додаємо нове значення до суми
//   readIndex = readIndex + 1;                 // Переходимо до наступного елемента

//   if (readIndex >= numReadings) {
//     readIndex = 0;                           // Повертаємося на початок масиву
//   }

//   // Обчислюємо середнє значення (все ще 12-бітне)
//   int averagePot = total / numReadings;

//   // 2. Переводимо 12 біт (0-4095) у 8 біт (0-255) за допомогою зсуву на 4 біти вправо
//   int rawDuty = averagePot >> 4; 

//   // 3. Фільтр мертвої зони
//   if (abs(rawDuty - lastDutyCycle) > noiseThreshold || rawDuty == 0 || rawDuty == 255) {
    
//     lastDutyCycle = rawDuty;
//     ledcWrite(pwmChannel, lastDutyCycle);

//     // Вивід у відсотках для 8-бітної шкали (255 — це 100%)
//     float percent = (lastDutyCycle / 255.0) * 100.0;
//     Serial.print("Duty Cycle: ");
//     Serial.print(lastDutyCycle);
//     Serial.print(" / 255 (");
//     Serial.print(percent, 1);
//     Serial.println("%)");
//   }

//   delay(5); // Маленька затримка, оскільки усереднення 32 точок займає час
// }








// #include <Arduino.h>

// const int potPin = 7;  
// const int pwmPin = 5;  

// const int pwmChannel = 0;     
// const int frequency = 8000;   // Компромісна частота 8 кГц під PC817
// const int resolution = 12;    // Перейшли на 12 біт (заповнення від 0 до 4095)

// // Фільтр ковзного середнього (залишаємо для захисту від наводок)
// const int numReadings = 20;    
// int readings[numReadings];     
// int readIndex = 0;             
// long total = 0;                

// int lastDutyCycle = -1;
// // Оскільки шкала стала в 4 рази ширшою, поріг шуму для 12 біт ставимо трохи вищим
// const int noiseThreshold = 6; 

// // Перераховуємо відсотки в значення для 12-бітного ШИМ (0-4095)
// const int pwm_50_percent = 2048;   // 50% від 4095
// const int pwm_64_percent = 2621;   // 64% від 4095

// void setup() {
//   Serial.begin(115200);
  
//   // Ініціалізація ШИМ на 8 кГц та 12 біт
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);
  
//   pinMode(potPin, INPUT);

//   int startValue = analogRead(potPin);
//   for (int i = 0; i < numReadings; i++) readings[i] = startValue;
//   total = (long)startValue * numReadings;
// }

// void loop() {
//   // 1. Усереднення значень АЦП
//   total = total - readings[readIndex];       
//   readings[readIndex] = analogRead(potPin);  
//   total = total + readings[readIndex];       
//   readIndex = (readIndex + 1) % numReadings;

//   int averagePot = total / numReadings; // Чисте 12-бітне значення (0-4095)

//   int targetDuty = 0;

//   // 2. Ювелірна математична лупа під 12-бітний ШИМ
//   if (averagePot < 1024) {
//     // Перша чверть повороту ручки: 0% - 50% ШИМ (0 - 2048)
//     targetDuty = map(averagePot, 0, 1023, 0, pwm_50_percent);
//   } 
//   else if (averagePot >= 1024 && averagePot <= 3072) {
//     // Середні 50% ходу ручки: надточний діапазон 50% - 64% ШИМ (2048 - 2621)
//     targetDuty = map(averagePot, 1024, 3072, pwm_50_percent, pwm_64_percent);
//   } 
//   else {
//     // Остання чверть повороту ручки: 64% - 100% ШИМ (2621 - 4095)
//     targetDuty = map(averagePot, 3073, 4095, pwm_64_percent, 4095);
//   }

//   // 3. Фільтр мертвої зони (гістерезис)
//   if (abs(targetDuty - lastDutyCycle) >= noiseThreshold || targetDuty == 0 || targetDuty == 4095) {
//     lastDutyCycle = targetDuty;
//     ledcWrite(pwmChannel, lastDutyCycle);

//     // Рахуємо відсотки від 12-бітної шкали (4095 — це 100%)
//     float percent = (lastDutyCycle / 4095.0) * 100.0;
    
//     Serial.print("Duty Cycle: ");
//     Serial.print(lastDutyCycle);
//     Serial.print(" / 4095 (");
//     Serial.print(percent, 1);
//     Serial.print("%)");
    
//     if (percent >= 50.0 && percent <= 64.0) {
//       Serial.println(" <-- [ ТОЧНА ЗОНА 12-БІТ ]");
//     } else {
//       Serial.println();
//     }
//   }

//   delay(10);
// }



// #include <Arduino.h>

// const int potPin = 7;  
// const int pwmPin = 5;  

// const int pwmChannel = 0;     
// const int frequency = 8000;   // Твоя робоча частота 8 кГц
// const int resolution = 12;    // 12 біт (заповнення від 0 до 4095)

// // Фільтр ковзного середнього (для захисту від наводок)
// const int numReadings = 20;    
// int readings[numReadings];     
// int readIndex = 0;             
// long total = 0;                

// int lastDutyCycle = -1;
// // Поріг шуму для 12 біт
// const int noiseThreshold = 5; 

// // Перераховуємо нові відсотки в значення для 12-бітного ШИМ
// const int pwm_65_percent = 2662;   // 65% від 4095 (4095 * 0.65)
// const int pwm_90_percent = 3686;   // 90% від 4095 (4095 * 0.90)

// void setup() {
//   Serial.begin(115200);
  
//   // Ініціалізація ШИМ на 8 кГц та 12 біт
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);
  
//   pinMode(potPin, INPUT);

//   int startValue = analogRead(potPin);
//   for (int i = 0; i < numReadings; i++) readings[i] = startValue;
//   total = (long)startValue * numReadings;
// }

// void loop() {
//   // 1. Усереднення значень АЦП
//   total = total - readings[readIndex];       
//   readings[readIndex] = analogRead(potPin);  
//   total = total + readings[readIndex];       
//   readIndex = (readIndex + 1) % numReadings;

//   int averagePot = total / numReadings; // Чисте 12-бітне значення (0-4095)

//   // 2. Лінійне розтягування: весь хід ручки (0-4095) перетворюється в ШИМ (65% - 90%)
//   int targetDuty = map(averagePot, 0, 4095, pwm_65_percent, pwm_90_percent);

//   // 3. Фільтр мертвої зони (гістерезис) + фіксація крайніх положень ручки
//   if (abs(targetDuty - lastDutyCycle) >= noiseThreshold || averagePot == 0 || averagePot == 4095) {
//     lastDutyCycle = targetDuty;
//     ledcWrite(pwmChannel, lastDutyCycle);

//     // Рахуємо реальний відсоток заповнення ШИМ
//     float percent = (lastDutyCycle / 4095.0) * 100.0;
    
//     Serial.print("Pot: ");
//     Serial.print(averagePot);
//     Serial.print(" -> PWM Duty: ");
//     Serial.print(lastDutyCycle);
//     Serial.print(" / 4095 (");
//     Serial.print(percent, 1);
//     Serial.println("%)");
//   }

//   delay(10);
// }



// #include <Arduino.h>

// const int potPin = 7;  
// const int pwmPin = 5;  

// const int pwmChannel = 0;     
// const int frequency = 15000;  // Стабільні 15 кГц ШИМ
// const int resolution = 10;    // 10 біт (заповнення від 0 до 1023)

// // Фільтр ковзного середнього
// const int numReadings = 20;    
// int readings[numReadings];     
// int readIndex = 0;             
// long total = 0;                

// int lastDutyCycle = -1;
// const int noiseThreshold = 1; // Діапазон вузький, зменшуємо поріг для максимальної чутливості

// // Розраховуємо жорсткі межі для 10-бітного ШИМ (0-1023)
// const int pwm_55_percent = 562;   // 55% від 1023 (1023 * 0.55)
// const int pwm_72_percent = 736;   // 72% від 1023 (1023 * 0.72)

// void setup() {
//   Serial.begin(115200);
  
//   // Ініціалізація ШИМ на 15 кГц та 10 біт
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);
  
//   pinMode(potPin, INPUT);

//   int startValue = analogRead(potPin);
//   for (int i = 0; i < numReadings; i++) readings[i] = startValue;
//   total = (long)startValue * numReadings;
// }

// void loop() {
//   // 1. Усереднення значень АЦП (0-4095)
//   total = total - readings[readIndex];       
//   readings[readIndex] = analogRead(potPin);  
//   total = total + readings[readIndex];       
//   readIndex = (readIndex + 1) % numReadings;

//   int averagePot = total / numReadings; 

//   // 2. Мапування: весь хід ручки (0-4095) тепер дає від 55% до 72% ШИМ
//   int targetDuty = map(averagePot, 0, 4095, pwm_55_percent, pwm_72_percent);

//   // 3. Гістерезис + чітка фіксація крайніх точок потенціометра
//   if (abs(targetDuty - lastDutyCycle) >= noiseThreshold || averagePot == 0 || averagePot == 4095) {
//     lastDutyCycle = targetDuty;
//     ledcWrite(pwmChannel, lastDutyCycle);

//     // Рахуємо реальний відсоток для виводу в монітор порту
//     float percent = (lastDutyCycle / 1023.0) * 100.0;
    
//     Serial.print("Pot: ");
//     Serial.print(averagePot);
//     Serial.print(" -> PWM Duty: ");
//     Serial.print(lastDutyCycle);
//     Serial.print(" / 1023 (");
//     Serial.print(percent, 1);
//     Serial.println("%)");
//   }

//   delay(10);
// }














// #include <Arduino.h>

// const int potPin = 7;  
// const int pwmPin = 5;  

// const int pwmChannel = 0;     
// const int frequency = 15000;  
// const int resolution = 10;    // 10 біт (0-1023)

// // Фільтр ковзного середнього
// const int numReadings = 20;    
// int readings[numReadings];     
// int readIndex = 0;             
// long total = 0;                

// int lastDutyCycle = -1;
// const int noiseThreshold = 1; 

// // ТАБЛИЦЯ ЛІНЕАРИЗАЦІЇ (Look-Up Table)
// // Перераховані твої відсотки в 10-бітні значення ШИМ (1023 * % / 100)
// // Індекси масиву відповідають напругам: 
// // [0]=24V, [1]=23V, [2]=22V, [3]=21V, [4]=20V, [5]=19V, [6]=18V, [7]=17V, [8]=16V
// const int pwmTable[] = {
//   563, // 55.0% -> 24V
//   573, // 56.0% -> 23V
//   578, // 56.5% -> 22V
//   583, // 57.0% -> 21V
//   588, // 57.5% -> 20V
//   594, // 58.1% -> 19V
//   597, // 58.4% -> 18V
//   626, // 61.2% -> 17V
//   767  // 75.0% -> 16V
// };

// // шпаргалка:
// // при 55% 24 вольт
// // при 56% 23вольт
// // при 56.5 22 вольт
// // при 57 21 вольт
// // при 57.5 20 вольт
// // при 58.1 19 вольт
// // при 58.4 18 вольт
// // при 61.2 17 вльт
// // при 75 16 вольт

// const int numPoints = 9; // Кількість точок у таблиці

// // Функція шматочно-лінійної інтерполяції
// int calculateLinearizedPWM(int potValue) {
//   // Переводимо хід потенціометра (0-4095) у віртуальний індекс від 0.0 до 8.0
//   float virtualIndex = ((float)potValue / 4095.0) * (numPoints - 1);
  
//   int index = (int)virtualIndex;          // Ціла частина (ліва опорна точка)
//   float fraction = virtualIndex - index;  // Дробова частина (відстань між точками)
  
//   if (index >= numPoints - 1) {
//     return pwmTable[numPoints - 1];
//   }
  
//   // Рахуємо проміжне значення між двома сусідніми точками ШИМ
//   int lowerPWM = pwmTable[index];
//   int upperPWM = pwmTable[index + 1];
  
//   int targetPWM = lowerPWM + (int)(fraction * (upperPWM - lowerPWM));
//   return targetPWM;
// }

// void setup() {
//   Serial.begin(115200);
  
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);
  
//   pinMode(potPin, INPUT);

//   int startValue = analogRead(potPin);
//   for (int i = 0; i < numReadings; i++) readings[i] = startValue;
//   total = (long)startValue * numReadings;
// }

// void loop() {
//   total = total - readings[readIndex];       
//   readings[readIndex] = analogRead(potPin);  
//   total = total + readings[readIndex];       
//   readIndex = (readIndex + 1) % numReadings;

//   int averagePot = total / numReadings; 

//   // Обчислюємо ШИМ через таблицю інтерполяції
//   int targetDuty = calculateLinearizedPWM(averagePot);

//   if (abs(targetDuty - lastDutyCycle) >= noiseThreshold || averagePot == 0 || averagePot == 4095) {
//     lastDutyCycle = targetDuty;
//     ledcWrite(pwmChannel, lastDutyCycle);

//     // Розрахунок приблизної напруги для монітора порту (просто для інфо)
//     float estimatedVolts = 24.0 - (((float)averagePot / 4095.0) * 8.0);
    
//     Serial.print("Pot: ");
//     Serial.print(averagePot);
//     Serial.print(" -> Target Volts: ~");
//     Serial.print(estimatedVolts, 1);
//     Serial.print("V -> Calculated PWM: ");
//     Serial.println(lastDutyCycle);
//   }

//   delay(10);
// }



































































































// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <WebServer.h> 
// #include <Update.h>    
// #include <WireGuard-ESP32.h> 

// // ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ (LED) =================
// const int pwmPin = 5;  
// const int pwmChannel = 0;     
// const int frequency = 15000;  
// const int resolution = 10;    // 10 біт (0-1023)

// const int PWM_MIN_BRIGHTNESS = 767; // 0%  яскравість (відповідає 16V)
// const int PWM_MAX_BRIGHTNESS = 563; // 100% яскравість (відповідає 24V)

// float currentPwmPct = 0.0f;   
// bool updateRequired = true;   

// // ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
// const char* ssid     = "GW_1_2.4G";
// const char* pass     = "GreenWave2015";

// // Повертаємо реальний IP брокера. Завдяки AllowedIPs 0.0.0.0/0 трафік все одно піде через VPN
// const char* mqtt_srv  = "164.92.129.221";  
// const int   mqtt_port = 1883;
// const char* mqtt_user = "LedControl";  
// const char* mqtt_pass = "LedPass";   
  
// const char* topicStatus   = "smartHome/Led1/status";
// const char* topicCommands = "smartHome/Led1/commands";

// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// // ================= НАЛАШТУВАННЯ WIREGUARD VPN (#Vitalik_VPN_2) =================
// const char* wg_local_ip   = "10.0.0.38"; 
// const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
// const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; // Публічний ключ СЕРВЕРА
// const char* wg_endpoint    = "161.35.78.1"; 
// const int   wg_port        = 51820;        

// WireGuard wg;
// bool vpnConnected = false; 

// // ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
// WebServer server(80);
// bool isUpdating = false;

// const char* serverIndex = R"html(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset='UTF-8'>
//     <meta name='viewport' content='width=device-width, initial-scale=1.0'>
//     <style>
//         body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
//         .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
//         h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
//         input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
//         input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
//         input[type=submit]:active { background: #218838; }
//     </style>
// </head>
// <body>
//     <div class='card'>
//         <h2>Оновлення прошивки LED модуля через VPN</h2>
//         <form method='POST' action='/update' enctype='multipart/form-data'>
//             <input type='file' name='update' required accept='.bin'>
//             <input type='submit' value='Оновити Led1'>
//         </form>
//     </div>
// </body>
// </html>
// )html";

// uint32_t periodStatusIdleMs = 10000; 
// uint32_t lastStatus = 0;
// uint32_t lastLog = 0;

// void forceImmediateStatusPublish() { lastStatus = 0; }

// void publishStatus() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   bool success = mqttClient.publish(topicStatus, out.c_str(), false);
//   if (success) {
//     Serial.printf("📤 [MQTT OUT] Надіслано статус у топік [%s] -> %s\n", topicStatus, out.c_str());
//   } else {
//     Serial.printf("❌ [MQTT OUT] Помилка відправки статусу в топік [%s]!\n", topicStatus);
//   }
  
//   lastStatus = millis();
// }

// void callback(char* topic, byte* payload, unsigned int len) {
//   String msg = "";
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
//   Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

//   if (isUpdating) return; 

//   JsonDocument d;
//   DeserializationError err = deserializeJson(d, msg);
//   if (err) {
//     Serial.printf("❌ [JSON] Помилка парсингу: %s\n", err.c_str());
//     return;
//   }

//   if (String(topic) == topicCommands) {
//     if (!d["pwm_pct"].isNull()) {
//       float pct = d["pwm_pct"].as<float>();
//       if (pct >= 0.0f && pct <= 100.0f) {
//         currentPwmPct = pct;
//         updateRequired = true;
//         Serial.printf("🎯 [CONTROL] Валідне значення прийнято: %.1f%%\n", currentPwmPct);
//         forceImmediateStatusPublish();
//       }
//     }
//   }
// }

// void connectWiFi() {
//   if (WiFi.status() == WL_CONNECTED) return;
  
//   Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
//   WiFi.begin(ssid, pass);
  
//   int timeoutCounter = 0;
//   while (WiFi.status() != WL_CONNECTED) { 
//     delay(500); 
//     Serial.print(".");
//     timeoutCounter++;
//     if (timeoutCounter > 40) { 
//       Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
//       return;
//     }
//   }
//   Serial.println("\n✅ Wi-Fi успішно підключено!");
//   Serial.print("🏠 Локальна IP-адреса: ");
//   Serial.println(WiFi.localIP());
// }

// void connectVPN() {
//   if (vpnConnected) return; 
  
//   connectWiFi(); 

//   Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
//   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

//   Serial.print("🕒 Очікування відповіді від NTP серверів...");
//   int attempts = 0;
//   while (time(nullptr) < 1000000000l && attempts < 40) { 
//     delay(500); 
//     Serial.print(".");
//     attempts++;
//   }

//   if (time(nullptr) < 1000000000l) {
//     Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час! Хендшейк може впасти.");
//   } else {
//     time_t nowTime = time(nullptr);
//     Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
//   }

//   Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
//   if (wg.begin(
//         IPAddress(10,0,0,38),          
//         wg_private_key,                
//         wg_endpoint,                    
//         wg_public_key,                 
//         wg_port                        
//       )) {
//       Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
//       vpnConnected = true;
//   } else {
//       Serial.println("❌ Помилка старту крипто-графічного ядра WireGuard.");
//       vpnConnected = false;
//   }
// }

// void connectMQTT() {
//   if (isUpdating) return; 
  
//   while (!mqttClient.connected()) {
//     connectVPN(); 
    
//     String clientId = "ESP32-LED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
//     Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
//     if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus, 0, false, "{\"state\":\"offline\"}")) {
      
//       bool subSuccess = mqttClient.subscribe(topicCommands); 
//       if(subSuccess) {
//         Serial.printf("📥 Успішна підписка на топік команд: %s\n", topicCommands);
//       } else {
//         Serial.printf("❌ Помилка підписки на топік команд: %s\n", topicCommands);
//       }
      
//       forceImmediateStatusPublish();
//       Serial.println("🚀 Зв'язок з брокером встановлено. Контролер онлайн.");
//     } else {
//       Serial.printf("❌ Помилка MQTT з'єднання. Код стану rc = %d. Повтор через 2 сек...\n", mqttClient.state());
//       delay(2000);
//     }
//   }
// }

// void setupWebOTA() {
//   server.on("/", HTTP_GET, []() {
//       server.sendHeader("Connection", "close");
//       server.send(200, "text/html", serverIndex);
//   });

//   server.on("/update", HTTP_POST, []() {
//       server.sendHeader("Connection", "close");
//       String responseHtml;
//       if (Update.hasError()) {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
//       } else {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
//       }
//       server.send(200, "text/html", responseHtml);
//       delay(1200);
//       ESP.restart();
//   }, []() {
//       HTTPUpload& upload = server.upload();
//       if (upload.status == UPLOAD_FILE_START) {
//           isUpdating = true;
//           Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
//           if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//           if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_END) {
//           if (!Update.end(true)) { 
//             Update.printError(Serial); 
//           } else {
//             Serial.println("✅ [OTA] Файл успішно записано в Flash!");
//           }
//           isUpdating = false;
//       }
//   });
//   server.begin();
//   Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
// }

// void setup() {
//   Serial.begin(115200);
//   delay(500); 
//   Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: LED NODE STARTED ===");
  
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);

//   mqttClient.setServer(mqtt_srv, mqtt_port);
//   mqttClient.setCallback(callback);
//   mqttClient.setBufferSize(512); 

//   connectWiFi();
//   setupWebOTA(); 
//   connectMQTT();
// }

// void loop() {
//   server.handleClient();
//   if (isUpdating) return; 

//   if (!mqttClient.connected()) {
//     Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
//     connectMQTT();
//   }
//   mqttClient.loop();

//   uint32_t now = millis();

//   if (updateRequired) {
//     updateRequired = false;
//     int targetDuty = PWM_MIN_BRIGHTNESS + (int)((currentPwmPct / 100.0f) * (PWM_MAX_BRIGHTNESS - PWM_MIN_BRIGHTNESS));
//     ledcWrite(pwmChannel, targetDuty);
//     Serial.printf("⚙️ [HARDWARE] Оновлено ШИМ регістр таймера. Задано: %.1f%% | Реальне 10-біт значення: %d\n", currentPwmPct, targetDuty);
//   }

//   if (now - lastStatus >= periodStatusIdleMs) {
//     publishStatus();
//   }

//   if (now - lastLog > 5000) {
//     lastLog = now;
//     Serial.printf("[HEARTBEAT] Pwm: %.1f%% | Live Duty: %d | Net Status -> WiFi: %s | VPN: %s | MQTT: %s\n",
//                   currentPwmPct, 
//                   PWM_MIN_BRIGHTNESS + (int)((currentPwmPct / 100.0f) * (PWM_MAX_BRIGHTNESS - PWM_MIN_BRIGHTNESS)), 
//                   (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
//                   vpnConnected ? "UP" : "DOWN",
//                   mqttClient.connected() ? "CONNECTED" : "OFFLINE");
//   }

//   delay(10);
// }



















// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <WebServer.h> 
// #include <Update.h>    
// #include <WireGuard-ESP32.h> 

// // ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ (LED) =================
// const int pwmPin = 5;  
// const int pwmChannel = 0;     
// const int frequency = 15000;  
// const int resolution = 10;    // 10 біт (0-1023)

// float currentPwmPct = 0.0f;   
// bool updateRequired = true;   

// // ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
// const char* ssid     = "GW_1_2.4G";
// const char* pass     = "GreenWave2015";

// const char* mqtt_srv  = "164.92.129.221";  
// const int   mqtt_port = 1883;
// const char* mqtt_user = "LedControl";  
// const char* mqtt_pass = "LedPass";   
  
// const char* topicStatus   = "smartHome/Led1/status";
// const char* topicCommands = "smartHome/Led1/commands";

// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// // ================= НАЛАШТУВАННЯ WIREGUARD VPN (#Vitalik_VPN_2) =================
// const char* wg_local_ip   = "10.0.0.38"; 
// const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
// const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; // Публічний ключ СЕРВЕРА
// const char* wg_endpoint    = "161.35.78.1"; 
// const int   wg_port        = 51820;        

// WireGuard wg;
// bool vpnConnected = false; 

// // ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
// WebServer server(80);
// bool isUpdating = false;

// const char* serverIndex = R"html(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset='UTF-8'>
//     <meta name='viewport' content='width=device-width, initial-scale=1.0'>
//     <style>
//         body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
//         .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
//         h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
//         input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
//         input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
//         input[type=submit]:active { background: #218838; }
//     </style>
// </head>
// <body>
//     <div class='card'>
//         <h2>Оновлення прошивки LED модуля через VPN</h2>
//         <form method='POST' action='/update' enctype='multipart/form-data'>
//             <input type='file' name='update' required accept='.bin'>
//             <input type='submit' value='Оновити Led1'>
//         </form>
//     </div>
// </body>
// </html>
// )html";

// uint32_t periodStatusIdleMs = 10000; 
// uint32_t lastStatus = 0;
// uint32_t lastLog = 0;

// void forceImmediateStatusPublish() { lastStatus = 0; }

// void publishStatus() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   bool success = mqttClient.publish(topicStatus, out.c_str(), false);
//   if (success) {
//     Serial.printf("📤 [MQTT OUT] Надіслано статус у топік [%s] -> %s\n", topicStatus, out.c_str());
//   } else {
//     Serial.printf("❌ [MQTT OUT] Помилка відправки статусу в топік [%s]!\n", topicStatus);
//   }
  
//   lastStatus = millis();
// }

// void callback(char* topic, byte* payload, unsigned int len) {
//   String msg = "";
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
//   Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

//   if (isUpdating) return; 

//   JsonDocument d;
//   DeserializationError err = deserializeJson(d, msg);
//   if (err) {
//     Serial.printf("❌ [JSON] Помилка парсингу: %s\n", err.c_str());
//     return;
//   }

//   if (String(topic) == topicCommands) {
//     if (!d["pwm_pct"].isNull()) {
//       float pct = d["pwm_pct"].as<float>();
//       if (pct >= 0.0f && pct <= 100.0f) {
//         currentPwmPct = pct;
//         updateRequired = true;
//         Serial.printf("🎯 [CONTROL] Валідне значення прийнято: %.1f%%\n", currentPwmPct);
//         forceImmediateStatusPublish();
//       }
//     }
//   }
// }

// void connectWiFi() {
//   if (WiFi.status() == WL_CONNECTED) return;
  
//   Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
//   WiFi.begin(ssid, pass);
  
//   int timeoutCounter = 0;
//   while (WiFi.status() != WL_CONNECTED) { 
//     delay(500); 
//     Serial.print(".");
//     timeoutCounter++;
//     if (timeoutCounter > 40) { 
//       Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
//       return;
//     }
//   }
//   Serial.println("\n✅ Wi-Fi успішно підключено!");
//   Serial.print("🏠 Локальна IP-адреса: ");
//   Serial.println(WiFi.localIP());
// }

// void connectVPN() {
//   if (vpnConnected) return; 
  
//   connectWiFi(); 

//   Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
//   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

//   Serial.print("🕒 Очікування відповіді від NTP серверів...");
//   int attempts = 0;
//   while (time(nullptr) < 1000000000l && attempts < 40) { 
//     delay(500); 
//     Serial.print(".");
//     attempts++;
//   }

//   if (time(nullptr) < 1000000000l) {
//     Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час! Хендшейк може впасти.");
//   } else {
//     time_t nowTime = time(nullptr);
//     Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
//   }

//   Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
//   if (wg.begin(
//         IPAddress(10,0,0,38),          
//         wg_private_key,                
//         wg_endpoint,                    
//         wg_public_key,                 
//         wg_port                        
//       )) {
//       Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
//       vpnConnected = true;
//   } else {
//       Serial.println("❌ Помилка старту крипто-графічного ядра WireGuard.");
//       vpnConnected = false;
//   }
// }

// void connectMQTT() {
//   if (isUpdating) return; 
  
//   while (!mqttClient.connected()) {
//     connectVPN(); 
    
//     String clientId = "ESP32-LED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
//     Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
//     if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus, 0, false, "{\"state\":\"offline\"}")) {
      
//       bool subSuccess = mqttClient.subscribe(topicCommands); 
//       if(subSuccess) {
//         Serial.printf("📥 Успішна підписка на топік команд: %s\n", topicCommands);
//       } else {
//         Serial.printf("❌ Помилка підписки на топік команд: %s\n", topicCommands);
//       }
      
//       forceImmediateStatusPublish();
//       Serial.println("🚀 Зв'язок з брокером встановлено. Контролер онлайн.");
//     } else {
//       Serial.printf("❌ Помилка MQTT з'єднання. Код стану rc = %d. Повтор через 2 сек...\n", mqttClient.state());
//       delay(2000);
//     }
//   }
// }

// void setupWebOTA() {
//   server.on("/", HTTP_GET, []() {
//       server.sendHeader("Connection", "close");
//       server.send(200, "text/html", serverIndex);
//   });

//   server.on("/update", HTTP_POST, []() {
//       server.sendHeader("Connection", "close");
//       String responseHtml;
//       if (Update.hasError()) {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
//       } else {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
//       }
//       server.send(200, "text/html", responseHtml);
//       delay(1200);
//       ESP.restart();
//   }, []() {
//       HTTPUpload& upload = server.upload();
//       if (upload.status == UPLOAD_FILE_START) {
//           isUpdating = true;
//           Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
//           if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//           if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_END) {
//           if (!Update.end(true)) { 
//             Update.printError(Serial); 
//           } else {
//             Serial.println("✅ [OTA] Файл успішно записано в Flash!");
//           }
//           isUpdating = false;
//       }
//   });
//   server.begin();
//   Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
// }

// void setup() {
//   Serial.begin(115200);
//   delay(500); 
//   Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: LED NODE STARTED ===");
  
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);

//   mqttClient.setServer(mqtt_srv, mqtt_port);
//   mqttClient.setCallback(callback);
//   mqttClient.setBufferSize(512); 

//   connectWiFi();
//   setupWebOTA(); 
//   connectMQTT();
// }

// void loop() {
//   server.handleClient();
//   if (isUpdating) return; 

//   if (!mqttClient.connected()) {
//     Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
//     connectMQTT();
//   }
//   mqttClient.loop();

//   uint32_t now = millis();

//   if (updateRequired) {
//     updateRequired = false;
    
//     // Прямий розрахунок: 0.0% -> 0 | 100.0% -> 1023
//     int targetDuty = (int)((currentPwmPct / 100.0f) * 1023.0f);
//     targetDuty = constrain(targetDuty, 0, 1023);
    
//     ledcWrite(pwmChannel, targetDuty);
//     Serial.printf("⚙️ [HARDWARE] Оновлено ШИМ. Задано: %.1f%% | Прямий Duty Cycle (0-1023): %d\n", currentPwmPct, targetDuty);
//   }

//   if (now - lastStatus >= periodStatusIdleMs) {
//     publishStatus();
//   }

//   if (now - lastLog > 5000) {
//     lastLog = now;
//     int currentDuty = (int)((currentPwmPct / 100.0f) * 1023.0f);
//     Serial.printf("[HEARTBEAT] Pwm: %.1f%% | Live Duty: %d | Net Status -> WiFi: %s | VPN: %s | MQTT: %s\n",
//                   currentPwmPct, 
//                   constrain(currentDuty, 0, 1023), 
//                   (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
//                   vpnConnected ? "UP" : "DOWN",
//                   mqttClient.connected() ? "CONNECTED" : "OFFLINE");
//   }

//   delay(10);
// }




























// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <WebServer.h> 
// #include <Update.h>    
// #include <WireGuard-ESP32.h> 

// // ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ (LED) =================
// const int pwmPin = 5;  
// const int pwmChannel = 0;     
// const int frequency = 20000;   // Знижено до 1 кГц для коректного перемикання PC817
// const int resolution = 10;    // 10 біт (0-1023)

// float currentPwmPct = 0.0f;   
// bool updateRequired = true;   

// // ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
// const char* ssid     = "GW_1_2.4G";
// const char* pass     = "GreenWave2015";

// const char* mqtt_srv  = "164.92.129.221";  
// const int   mqtt_port = 1883;
// const char* mqtt_user = "LedControl";  
// const char* mqtt_pass = "LedPass";   
  
// const char* topicStatus   = "smartHome/Led1/status";
// const char* topicCommands = "smartHome/Led1/commands";

// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// // ================= НАЛАШТУВАННЯ WIREGUARD VPN (#Vitalik_VPN_2) =================
// const char* wg_local_ip   = "10.0.0.38"; 
// const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
// const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; // Публічний ключ СЕРВЕРА
// const char* wg_endpoint    = "161.35.78.1"; 
// const int   wg_port        = 51820;        

// WireGuard wg;
// bool vpnConnected = false; 

// // ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
// WebServer server(80);
// bool isUpdating = false;

// const char* serverIndex = R"html(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset='UTF-8'>
//     <meta name='viewport' content='width=device-width, initial-scale=1.0'>
//     <style>
//         body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
//         .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
//         h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
//         input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
//         input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
//         input[type=submit]:active { background: #218838; }
//     </style>
// </head>
// <body>
//     <div class='card'>
//         <h2>Оновлення прошивки LED модуля через VPN</h2>
//         <form method='POST' action='/update' enctype='multipart/form-data'>
//             <input type='file' name='update' required accept='.bin'>
//             <input type='submit' value='Оновити Led1'>
//         </form>
//     </div>
// </body>
// </html>
// )html";

// uint32_t periodStatusIdleMs = 10000; 
// uint32_t lastStatus = 0;
// uint32_t lastLog = 0;

// void forceImmediateStatusPublish() { lastStatus = 0; }

// void publishStatus() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   bool success = mqttClient.publish(topicStatus, out.c_str(), false);
//   if (success) {
//     Serial.printf("📤 [MQTT OUT] Надіслано статус у топік [%s] -> %s\n", topicStatus, out.c_str());
//   } else {
//     Serial.printf("❌ [MQTT OUT] Помилка відправки статусу в топік [%s]!\n", topicStatus);
//   }
  
//   lastStatus = millis();
// }

// // Універсальна обробка вхідних команд (JSON та RAW число)
// void callback(char* topic, byte* payload, unsigned int len) {
//   String msg = "";
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
//   Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

//   if (isUpdating) return; 

//   if (String(topic) == topicCommands) {
//     float pct = -1.0f;

//     // 1. Спроба розпарсити як JSON {"pwm_pct": X}
//     JsonDocument d;
//     DeserializationError err = deserializeJson(d, msg);
//     if (!err && !d["pwm_pct"].isNull()) {
//       pct = d["pwm_pct"].as<float>();
//     } 
//     // 2. Якщо не JSON, парсимо як звичайне число (наприклад, "50")
//     else {
//       pct = msg.toFloat();
//     }

//     // Валідація отриманого значення
//     if (pct >= 0.0f && pct <= 100.0f) {
//       currentPwmPct = pct;
//       updateRequired = true;
//       Serial.printf("🎯 [CONTROL] Валідне значення прийнято: %.1f%%\n", currentPwmPct);
//       forceImmediateStatusPublish();
//     } else {
//       Serial.println("❌ [CONTROL] Помилка: отримане значення поза діапазоном 0-100%!");
//     }
//   }
// }

// void connectWiFi() {
//   if (WiFi.status() == WL_CONNECTED) return;
  
//   Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
//   WiFi.begin(ssid, pass);
  
//   int timeoutCounter = 0;
//   while (WiFi.status() != WL_CONNECTED) { 
//     delay(500); 
//     Serial.print(".");
//     timeoutCounter++;
//     if (timeoutCounter > 40) { 
//       Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
//       return;
//     }
//   }
//   Serial.println("\n✅ Wi-Fi успішно підключено!");
//   Serial.print("🏠 Локальна IP-адреса: ");
//   Serial.println(WiFi.localIP());
// }

// void connectVPN() {
//   if (vpnConnected) return; 
  
//   connectWiFi(); 

//   Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
//   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

//   Serial.print("🕒 Очікування відповіді від NTP серверів...");
//   int attempts = 0;
//   while (time(nullptr) < 1000000000l && attempts < 40) { 
//     delay(500); 
//     Serial.print(".");
//     attempts++;
//   }

//   if (time(nullptr) < 1000000000l) {
//     Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час! Хендшейк може впасти.");
//   } else {
//     time_t nowTime = time(nullptr);
//     Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
//   }

//   Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
//   if (wg.begin(
//         IPAddress(10,0,0,38),          
//         wg_private_key,                
//         wg_endpoint,                    
//         wg_public_key,                 
//         wg_port                        
//       )) {
//       Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
//       vpnConnected = true;
//   } else {
//       Serial.println("❌ Помилка старту крипто-графічного ядра WireGuard.");
//       vpnConnected = false;
//   }
// }

// void connectMQTT() {
//   if (isUpdating) return; 
  
//   while (!mqttClient.connected()) {
//     connectVPN(); 
    
//     String clientId = "ESP32-LED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
//     Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
//     if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus, 0, false, "{\"state\":\"offline\"}")) {
      
//       bool subSuccess = mqttClient.subscribe(topicCommands); 
//       if(subSuccess) {
//         Serial.printf("📥 Успішна підписка на топік команд: %s\n", topicCommands);
//       } else {
//         Serial.printf("❌ Помилка підписки на топік команд: %s\n", topicCommands);
//       }
      
//       forceImmediateStatusPublish();
//       Serial.println("🚀 Зв'язок з брокером встановлено. Контролер онлайн.");
//     } else {
//       Serial.printf("❌ Помилка MQTT з'єднання. Код стану rc = %d. Повтор через 2 сек...\n", mqttClient.state());
//       delay(2000);
//     }
//   }
// }

// void setupWebOTA() {
//   server.on("/", HTTP_GET, []() {
//       server.sendHeader("Connection", "close");
//       server.send(200, "text/html", serverIndex);
//   });

//   server.on("/update", HTTP_POST, []() {
//       server.sendHeader("Connection", "close");
//       String responseHtml;
//       if (Update.hasError()) {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
//       } else {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
//       }
//       server.send(200, "text/html", responseHtml);
//       delay(1200);
//       ESP.restart();
//   }, []() {
//       HTTPUpload& upload = server.upload();
//       if (upload.status == UPLOAD_FILE_START) {
//           isUpdating = true;
//           Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
//           if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//           if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_END) {
//           if (!Update.end(true)) { 
//             Update.printError(Serial); 
//           } else {
//             Serial.println("✅ [OTA] Файл успішно записано в Flash!");
//           }
//           isUpdating = false;
//       }
//   });
//   server.begin();
//   Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
// }

// void setup() {
//   Serial.begin(115200);
//   delay(500); 
//   Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: LED NODE STARTED ===");
  
//   ledcSetup(pwmChannel, frequency, resolution);
//   ledcAttachPin(pwmPin, pwmChannel);

//   mqttClient.setServer(mqtt_srv, mqtt_port);
//   mqttClient.setCallback(callback);
//   mqttClient.setBufferSize(512); 

//   connectWiFi();
//   setupWebOTA(); 
//   connectMQTT();
// }

// void loop() {
//   server.handleClient();
//   if (isUpdating) return; 

//   if (!mqttClient.connected()) {
//     Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
//     connectMQTT();
//   }
//   mqttClient.loop();

//   uint32_t now = millis();

//   if (updateRequired) {
//     updateRequired = false;
    
//     // Розрахунок заповнення ШИМ: 0.0% -> 0 | 100.0% -> 1023
//     int targetDuty = (int)((currentPwmPct / 100.0f) * 1023.0f);
//     targetDuty = constrain(targetDuty, 0, 1023);
    
//     ledcWrite(pwmChannel, targetDuty);
//     Serial.printf("⚙️ [HARDWARE] Оновлено ШИМ. Задано: %.1f%% | Прямий Duty Cycle (0-1023): %d\n", currentPwmPct, targetDuty);
//   }

//   if (now - lastStatus >= periodStatusIdleMs) {
//     publishStatus();
//   }

//   if (now - lastLog > 5000) {
//     lastLog = now;
//     int currentDuty = (int)((currentPwmPct / 100.0f) * 1023.0f);
//     Serial.printf("[HEARTBEAT] Pwm: %.1f%% | Live Duty: %d | Net Status -> WiFi: %s | VPN: %s | MQTT: %s\n",
//                   currentPwmPct, 
//                   constrain(currentDuty, 0, 1023), 
//                   (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
//                   vpnConnected ? "UP" : "DOWN",
//                   mqttClient.connected() ? "CONNECTED" : "OFFLINE");
//   }

//   delay(10);
// }














// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <WebServer.h> 
// #include <Update.h>    
// #include <WireGuard-ESP32.h> 

// // ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ (LED) =================
// // Канал 1 (Led1)
// const int pwmPin1     = 5;  
// const int pwmChannel1 = 0;     

// // Канал 2 (Led2)
// const int pwmPin2     = 6;  
// const int pwmChannel2 = 1;     

// const int frequency  = 15000;  // Частота ШИМ (20 кГц)
// const int resolution = 10;     // 10 біт (0-1023)

// float currentPwmPct1 = 0.0f;   
// float currentPwmPct2 = 0.0f;   

// bool updateRequired1 = true;   
// bool updateRequired2 = true;   

// // ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
// const char* ssid     = "GW_1_2.4G";
// const char* pass     = "GreenWave2015";

// const char* mqtt_srv  = "164.92.129.221";  
// const int   mqtt_port = 1883;
// const char* mqtt_user = "LedControl";  
// const char* mqtt_pass = "LedPass";   
  
// // Топіки для Led1
// const char* topicStatus1   = "smartHome/Led1/status";
// const char* topicCommands1 = "smartHome/Led1/commands";

// // Топіки для Led2
// const char* topicStatus2   = "smartHome/Led2/status";
// const char* topicCommands2 = "smartHome/Led2/commands";

// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// // ================= НАЛАШТУВАННЯ WIREGUARD VPN (#Vitalik_VPN_2) =================
// const char* wg_local_ip    = "10.0.0.38"; 
// const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
// const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; // Публічний ключ СЕРВЕРА
// const char* wg_endpoint    = "161.35.78.1"; 
// const int   wg_port        = 51820;        

// WireGuard wg;
// bool vpnConnected = false; 

// // ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
// WebServer server(80);
// bool isUpdating = false;

// const char* serverIndex = R"html(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset='UTF-8'>
//     <meta name='viewport' content='width=device-width, initial-scale=1.0'>
//     <style>
//         body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
//         .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
//         h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
//         input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
//         input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
//         input[type=submit]:active { background: #218838; }
//     </style>
// </head>
// <body>
//     <div class='card'>
//         <h2>Оновлення прошивки Dual LED модуля через VPN</h2>
//         <form method='POST' action='/update' enctype='multipart/form-data'>
//             <input type='file' name='update' required accept='.bin'>
//             <input type='submit' value='Оновити контролер'>
//         </form>
//     </div>
// </body>
// </html>
// )html";

// uint32_t periodStatusIdleMs = 10000; 
// uint32_t lastStatus = 0;
// uint32_t lastLog = 0;

// void forceImmediateStatusPublish() { lastStatus = 0; }

// void publishStatus1() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct1 * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   if (mqttClient.publish(topicStatus1, out.c_str(), false)) {
//     Serial.printf("📤 [MQTT OUT] Статус Led1 -> %s\n", out.c_str());
//   } else {
//     Serial.println("❌ [MQTT OUT] Помилка відправки статусу Led1!");
//   }
// }

// void publishStatus2() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct2 * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   if (mqttClient.publish(topicStatus2, out.c_str(), false)) {
//     Serial.printf("📤 [MQTT OUT] Статус Led2 -> %s\n", out.c_str());
//   } else {
//     Serial.println("❌ [MQTT OUT] Помилка відправки статусу Led2!");
//   }
// }

// void publishAllStatuses() {
//   publishStatus1();
//   publishStatus2();
//   lastStatus = millis();
// }

// // Універсальна обробка вхідних команд для Led1 та Led2
// void callback(char* topic, byte* payload, unsigned int len) {
//   String msg = "";
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
//   Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

//   if (isUpdating) return; 

//   String topicStr = String(topic);

//   if (topicStr == topicCommands1 || topicStr == topicCommands2) {
//     float pct = -1.0f;

//     // 1. Спроба розпарсити як JSON {"pwm_pct": X}
//     JsonDocument d;
//     DeserializationError err = deserializeJson(d, msg);
//     if (!err && !d["pwm_pct"].isNull()) {
//       pct = d["pwm_pct"].as<float>();
//     } 
//     // 2. Якщо не JSON, парсимо як звичайне число
//     else {
//       pct = msg.toFloat();
//     }

//     // Валідація отриманого значення
//     if (pct >= 0.0f && pct <= 100.0f) {
//       if (topicStr == topicCommands1) {
//         currentPwmPct1 = pct;
//         updateRequired1 = true;
//         Serial.printf("🎯 [CONTROL Led1] Прийнято нове значення: %.1f%%\n", currentPwmPct1);
//         publishStatus1();
//       } else if (topicStr == topicCommands2) {
//         currentPwmPct2 = pct;
//         updateRequired2 = true;
//         Serial.printf("🎯 [CONTROL Led2] Прийнято нове значення: %.1f%%\n", currentPwmPct2);
//         publishStatus2();
//       }
//     } else {
//       Serial.println("❌ [CONTROL] Помилка: отримане значення поза діапазоном 0-100%!");
//     }
//   }
// }

// void connectWiFi() {
//   if (WiFi.status() == WL_CONNECTED) return;
  
//   Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
//   WiFi.begin(ssid, pass);
  
//   int timeoutCounter = 0;
//   while (WiFi.status() != WL_CONNECTED) { 
//     delay(500); 
//     Serial.print(".");
//     timeoutCounter++;
//     if (timeoutCounter > 40) { 
//       Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
//       return;
//     }
//   }
//   Serial.println("\n✅ Wi-Fi успішно підключено!");
//   Serial.print("🏠 Локальна IP-адреса: ");
//   Serial.println(WiFi.localIP());
// }

// void connectVPN() {
//   if (vpnConnected) return; 
  
//   connectWiFi(); 

//   Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
//   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

//   Serial.print("🕒 Очікування відповіді від NTP серверів...");
//   int attempts = 0;
//   while (time(nullptr) < 1000000000l && attempts < 40) { 
//     delay(500); 
//     Serial.print(".");
//     attempts++;
//   }

//   if (time(nullptr) < 1000000000l) {
//     Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час! Хендшейк може впасти.");
//   } else {
//     time_t nowTime = time(nullptr);
//     Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
//   }

//   Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
//   if (wg.begin(
//         IPAddress(10,0,0,38),          
//         wg_private_key,                
//         wg_endpoint,                    
//         wg_public_key,                 
//         wg_port                        
//       )) {
//       Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
//       vpnConnected = true;
//   } else {
//       Serial.println("❌ Помилка старту крипто-графічного ядра WireGuard.");
//       vpnConnected = false;
//   }
// }

// void connectMQTT() {
//   if (isUpdating) return; 
  
//   while (!mqttClient.connected()) {
//     connectVPN(); 
    
//     String clientId = "ESP32-DualLED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
//     Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
//     if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus1, 0, false, "{\"state\":\"offline\"}")) {
      
//       bool sub1 = mqttClient.subscribe(topicCommands1); 
//       bool sub2 = mqttClient.subscribe(topicCommands2); 

//       if(sub1 && sub2) {
//         Serial.printf("📥 Успішна підписка на топіки:\n - %s\n - %s\n", topicCommands1, topicCommands2);
//       } else {
//         Serial.println("❌ Помилка підписки на один із топіків команд!");
//       }
      
//       forceImmediateStatusPublish();
//       Serial.println("🚀 Зв'язок з брокером встановлено. Двоканальний контролер онлайн.");
//     } else {
//       Serial.printf("❌ Помилка MQTT з'єднання. Код стану rc = %d. Повтор через 2 сек...\n", mqttClient.state());
//       delay(2000);
//     }
//   }
// }

// void setupWebOTA() {
//   server.on("/", HTTP_GET, []() {
//       server.sendHeader("Connection", "close");
//       server.send(200, "text/html", serverIndex);
//   });

//   server.on("/update", HTTP_POST, []() {
//       server.sendHeader("Connection", "close");
//       String responseHtml;
//       if (Update.hasError()) {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
//       } else {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
//       }
//       server.send(200, "text/html", responseHtml);
//       delay(1200);
//       ESP.restart();
//   }, []() {
//       HTTPUpload& upload = server.upload();
//       if (upload.status == UPLOAD_FILE_START) {
//           isUpdating = true;
//           Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
//           if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//           if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_END) {
//           if (!Update.end(true)) { 
//             Update.printError(Serial); 
//           } else {
//             Serial.println("✅ [OTA] Файл успішно записано в Flash!");
//           }
//           isUpdating = false;
//       }
//   });
//   server.begin();
//   Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
// }

// void setup() {
//   Serial.begin(115200);
//   delay(500); 
//   Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: DUAL LED NODE STARTED ===");
  
//   // Налаштування Каналу 1 (GPIO 5)
//   ledcSetup(pwmChannel1, frequency, resolution);
//   ledcAttachPin(pwmPin1, pwmChannel1);

//   // Налаштування Каналу 2 (GPIO 6)
//   ledcSetup(pwmChannel2, frequency, resolution);
//   ledcAttachPin(pwmPin2, pwmChannel2);

//   mqttClient.setServer(mqtt_srv, mqtt_port);
//   mqttClient.setCallback(callback);
//   mqttClient.setBufferSize(512); 

//   connectWiFi();
//   setupWebOTA(); 
//   connectMQTT();
// }

// void loop() {
//   server.handleClient();
//   if (isUpdating) return; 

//   if (!mqttClient.connected()) {
//     Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
//     connectMQTT();
//   }
//   mqttClient.loop();

//   uint32_t now = millis();

//   // Оновлення ШИМ для Led1
//   if (updateRequired1) {
//     updateRequired1 = false;
//     int targetDuty1 = (int)((currentPwmPct1 / 100.0f) * 1023.0f);
//     targetDuty1 = constrain(targetDuty1, 0, 1023);
    
//     ledcWrite(pwmChannel1, targetDuty1);
//     Serial.printf("⚙️ [HARDWARE] Led1 ШИМ: %.1f%% | Duty: %d\n", currentPwmPct1, targetDuty1);
//   }

//   // Оновлення ШИМ для Led2
//   if (updateRequired2) {
//     updateRequired2 = false;
//     int targetDuty2 = (int)((currentPwmPct2 / 100.0f) * 1023.0f);
//     targetDuty2 = constrain(targetDuty2, 0, 1023);
    
//     ledcWrite(pwmChannel2, targetDuty2);
//     Serial.printf("⚙️ [HARDWARE] Led2 ШИМ: %.1f%% | Duty: %d\n", currentPwmPct2, targetDuty2);
//   }

//   // Періодичне відправлення статусу обох каналів
//   if (now - lastStatus >= periodStatusIdleMs) {
//     publishAllStatuses();
//   }

//   // Heartbeat логування
//   if (now - lastLog > 5000) {
//     lastLog = now;
//     int duty1 = constrain((int)((currentPwmPct1 / 100.0f) * 1023.0f), 0, 1023);
//     int duty2 = constrain((int)((currentPwmPct2 / 100.0f) * 1023.0f), 0, 1023);

//     Serial.printf("[HEARTBEAT] Led1: %.1f%% (Duty: %d) | Led2: %.1f%% (Duty: %d) | WiFi: %s | VPN: %s | MQTT: %s\n",
//                   currentPwmPct1, duty1, 
//                   currentPwmPct2, duty2,
//                   (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
//                   vpnConnected ? "UP" : "DOWN",
//                   mqttClient.connected() ? "CONNECTED" : "OFFLINE");
//   }

//   delay(10);
// }



























































































































// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <WebServer.h> 
// #include <Update.h>    
// #include <WireGuard-ESP32.h> 

// // ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ (LED) =================
// // Канал 1 (Led1)
// const int pwmPin1     = 5;  
// const int pwmChannel1 = 0;     

// // Канал 2 (Led2)
// const int pwmPin2     = 6;  
// const int pwmChannel2 = 1;     

// const int frequency  = 15000;  // Частота ШИМ
// const int resolution = 10;     // 10 біт (0-1023)

// float currentPwmPct1 = 0.0f;   
// float currentPwmPct2 = 0.0f;   

// bool updateRequired1 = true;   
// bool updateRequired2 = true;   

// // ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
// const char* ssid     = "GW_1_2.4G";
// const char* pass     = "GreenWave2015";

// const char* mqtt_srv  = "164.92.129.221";  
// const int   mqtt_port = 1883;
// const char* mqtt_user = "LedControl";  
// const char* mqtt_pass = "LedPass";   
  
// // Оновлені топіки з врахуванням ID користувача (user1)
// // Топіки для Led1
// const char* topicStatus1   = "smartHome/user1/Led1/status";
// const char* topicCommands1 = "smartHome/user1/Led1/commands";

// // Топіки для Led2
// const char* topicStatus2   = "smartHome/user1/Led2/status";
// const char* topicCommands2 = "smartHome/user1/Led2/commands";

// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// // ================= НАЛАШТУВАННЯ WIREGUARD VPN (#Vitalik_VPN_2) =================
// const char* wg_local_ip    = "10.0.0.38"; 
// const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
// const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; // Публічний ключ СЕРВЕРА
// const char* wg_endpoint    = "161.35.78.1"; 
// const int   wg_port        = 51820;        

// WireGuard wg;
// bool vpnConnected = false; 

// // ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
// WebServer server(80);
// bool isUpdating = false;

// const char* serverIndex = R"html(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset='UTF-8'>
//     <meta name='viewport' content='width=device-width, initial-scale=1.0'>
//     <style>
//         body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
//         .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
//         h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
//         input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
//         input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
//         input[type=submit]:active { background: #218838; }
//     </style>
// </head>
// <body>
//     <div class='card'>
//         <h2>Оновлення прошивки Dual LED модуля через VPN</h2>
//         <form method='POST' action='/update' enctype='multipart/form-data'>
//             <input type='file' name='update' required accept='.bin'>
//             <input type='submit' value='Оновити контролер'>
//         </form>
//     </div>
// </body>
// </html>
// )html";

// uint32_t periodStatusIdleMs = 10000; 
// uint32_t lastStatus = 0;
// uint32_t lastLog = 0;

// void forceImmediateStatusPublish() { lastStatus = 0; }

// void publishStatus1() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct1 * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   if (mqttClient.publish(topicStatus1, out.c_str(), false)) {
//     Serial.printf("📤 [MQTT OUT] Статус Led1 -> %s\n", out.c_str());
//   } else {
//     Serial.println("❌ [MQTT OUT] Помилка відправки статусу Led1!");
//   }
// }

// void publishStatus2() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct2 * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   if (mqttClient.publish(topicStatus2, out.c_str(), false)) {
//     Serial.printf("📤 [MQTT OUT] Статус Led2 -> %s\n", out.c_str());
//   } else {
//     Serial.println("❌ [MQTT OUT] Помилка відправки статусу Led2!");
//   }
// }

// void publishAllStatuses() {
//   publishStatus1();
//   publishStatus2();
//   lastStatus = millis();
// }

// // Універсальна обробка вхідних команд для Led1 та Led2
// void callback(char* topic, byte* payload, unsigned int len) {
//   String msg = "";
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
//   Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

//   if (isUpdating) return; 

//   String topicStr = String(topic);

//   if (topicStr == topicCommands1 || topicStr == topicCommands2) {
//     float pct = -1.0f;

//     // 1. Спроба розпарсити як JSON {"pwm_pct": X}
//     JsonDocument d;
//     DeserializationError err = deserializeJson(d, msg);
//     if (!err && !d["pwm_pct"].isNull()) {
//       pct = d["pwm_pct"].as<float>();
//     } 
//     // 2. Якщо не JSON, парсимо як звичайне число
//     else {
//       pct = msg.toFloat();
//     }

//     // Валідація отриманого значення
//     if (pct >= 0.0f && pct <= 100.0f) {
//       if (topicStr == topicCommands1) {
//         currentPwmPct1 = pct;
//         updateRequired1 = true;
//         Serial.printf("🎯 [CONTROL Led1] Прийнято нове значення: %.1f%%\n", currentPwmPct1);
//         publishStatus1();
//       } else if (topicStr == topicCommands2) {
//         currentPwmPct2 = pct;
//         updateRequired2 = true;
//         Serial.printf("🎯 [CONTROL Led2] Прийнято нове значення: %.1f%%\n", currentPwmPct2);
//         publishStatus2();
//       }
//     } else {
//       Serial.println("❌ [CONTROL] Помилка: отримане значення поза діапазоном 0-100%!");
//     }
//   }
// }

// void connectWiFi() {
//   if (WiFi.status() == WL_CONNECTED) return;
  
//   Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
//   WiFi.begin(ssid, pass);
  
//   int timeoutCounter = 0;
//   while (WiFi.status() != WL_CONNECTED) { 
//     delay(500); 
//     Serial.print(".");
//     timeoutCounter++;
//     if (timeoutCounter > 40) { 
//       Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
//       return;
//     }
//   }
//   Serial.println("\n✅ Wi-Fi успішно підключено!");
//   Serial.print("🏠 Локальна IP-адреса: ");
//   Serial.println(WiFi.localIP());
// }

// void connectVPN() {
//   if (vpnConnected) return; 
  
//   connectWiFi(); 

//   Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
//   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

//   Serial.print("🕒 Очікування відповіді від NTP серверів...");
//   int attempts = 0;
//   while (time(nullptr) < 1000000000l && attempts < 40) { 
//     delay(500); 
//     Serial.print(".");
//     attempts++;
//   }

//   if (time(nullptr) < 1000000000l) {
//     Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час! Хендшейк може впасти.");
//   } else {
//     time_t nowTime = time(nullptr);
//     Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
//   }

//   Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
//   if (wg.begin(
//         IPAddress(10,0,0,38),          
//         wg_private_key,                
//         wg_endpoint,                    
//         wg_public_key,                 
//         wg_port                        
//       )) {
//       Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
//       vpnConnected = true;
//   } else {
//       Serial.println("❌ Помилка старту крипто-графічного ядра WireGuard.");
//       vpnConnected = false;
//   }
// }

// void connectMQTT() {
//   if (isUpdating) return; 
  
//   while (!mqttClient.connected()) {
//     connectVPN(); 
    
//     String clientId = "ESP32-DualLED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
//     Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
//     if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus1, 0, false, "{\"state\":\"offline\"}")) {
      
//       bool sub1 = mqttClient.subscribe(topicCommands1); 
//       bool sub2 = mqttClient.subscribe(topicCommands2); 

//       if(sub1 && sub2) {
//         Serial.printf("📥 Успішна підписка на топіки:\n - %s\n - %s\n", topicCommands1, topicCommands2);
//       } else {
//         Serial.println("❌ Помилка підписки на один із топіків команд!");
//       }
      
//       forceImmediateStatusPublish();
//       Serial.println("🚀 Зв'язок з брокером встановлено. Двоканальний контролер онлайн.");
//     } else {
//       Serial.printf("❌ Помилка MQTT з'єднання. Код стану rc = %d. Повтор через 2 сек...\n", mqttClient.state());
//       delay(2000);
//     }
//   }
// }

// void setupWebOTA() {
//   server.on("/", HTTP_GET, []() {
//       server.sendHeader("Connection", "close");
//       server.send(200, "text/html", serverIndex);
//   });

//   server.on("/update", HTTP_POST, []() {
//       server.sendHeader("Connection", "close");
//       String responseHtml;
//       if (Update.hasError()) {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
//       } else {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
//       }
//       server.send(200, "text/html", responseHtml);
//       delay(1200);
//       ESP.restart();
//   }, []() {
//       HTTPUpload& upload = server.upload();
//       if (upload.status == UPLOAD_FILE_START) {
//           isUpdating = true;
//           Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
//           if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//           if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_END) {
//           if (!Update.end(true)) { 
//             Update.printError(Serial); 
//           } else {
//             Serial.println("✅ [OTA] Файл успішно записано в Flash!");
//           }
//           isUpdating = false;
//       }
//   });
//   server.begin();
//   Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
// }

// void setup() {
//   Serial.begin(115200);
//   delay(500); 
//   Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: DUAL LED NODE STARTED ===");
  
//   // Налаштування Каналу 1 (GPIO 5)
//   ledcSetup(pwmChannel1, frequency, resolution);
//   ledcAttachPin(pwmPin1, pwmChannel1);

//   // Налаштування Каналу 2 (GPIO 6)
//   ledcSetup(pwmChannel2, frequency, resolution);
//   ledcAttachPin(pwmPin2, pwmChannel2);

//   mqttClient.setServer(mqtt_srv, mqtt_port);
//   mqttClient.setCallback(callback);
//   mqttClient.setBufferSize(512); 

//   connectWiFi();
//   setupWebOTA(); 
//   connectMQTT();
// }

// void loop() {
//   server.handleClient();
//   if (isUpdating) return; 

//   if (!mqttClient.connected()) {
//     Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
//     connectMQTT();
//   }
//   mqttClient.loop();

//   uint32_t now = millis();

//   // Оновлення ШИМ для Led1
//   if (updateRequired1) {
//     updateRequired1 = false;
//     int targetDuty1 = (int)((currentPwmPct1 / 100.0f) * 1023.0f);
//     targetDuty1 = constrain(targetDuty1, 0, 1023);
    
//     ledcWrite(pwmChannel1, targetDuty1);
//     Serial.printf("⚙️ [HARDWARE] Led1 ШИМ: %.1f%% | Duty: %d\n", currentPwmPct1, targetDuty1);
//   }

//   // Оновлення ШИМ для Led2
//   if (updateRequired2) {
//     updateRequired2 = false;
//     int targetDuty2 = (int)((currentPwmPct2 / 100.0f) * 1023.0f);
//     targetDuty2 = constrain(targetDuty2, 0, 1023);
    
//     ledcWrite(pwmChannel2, targetDuty2);
//     Serial.printf("⚙️ [HARDWARE] Led2 ШИМ: %.1f%% | Duty: %d\n", currentPwmPct2, targetDuty2);
//   }

//   // Періодичне відправлення статусу обох каналів
//   if (now - lastStatus >= periodStatusIdleMs) {
//     publishAllStatuses();
//   }

//   // Heartbeat логування
//   if (now - lastLog > 5000) {
//     lastLog = now;
//     int duty1 = constrain((int)((currentPwmPct1 / 100.0f) * 1023.0f), 0, 1023);
//     int duty2 = constrain((int)((currentPwmPct2 / 100.0f) * 1023.0f), 0, 1023);

//     Serial.printf("[HEARTBEAT] Led1: %.1f%% (Duty: %d) | Led2: %.1f%% (Duty: %d) | WiFi: %s | VPN: %s | MQTT: %s\n",
//                   currentPwmPct1, duty1, 
//                   currentPwmPct2, duty2,
//                   (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
//                   vpnConnected ? "UP" : "DOWN",
//                   mqttClient.connected() ? "CONNECTED" : "OFFLINE");
//   }

//   delay(10);
// }




























// #include <Arduino.h>

// #define RF_RX_PIN 16

// volatile uint32_t shiftRegister = 0;
// volatile uint8_t bitCount = 0;
// volatile uint32_t lastChangeTime = 0;
// volatile uint32_t capturedCode = 0;
// volatile bool newCodeReady = false;
// volatile bool isSynced = false;

// void IRAM_ATTR rfISR() {
//   uint32_t now = micros();
//   uint32_t duration = now - lastChangeTime;
//   lastChangeTime = now;

//   if (duration < 100) return; // Фільтр завад

//   bool currentState = digitalRead(RF_RX_PIN);

//   if (currentState == HIGH) {
//     // Перехід LOW -> HIGH: шукаємо Sync Gap (від 2.5 мс до 12 мс)
//     if (duration >= 2500 && duration <= 12000) {
//       isSynced = true;
//       bitCount = 0;
//       shiftRegister = 0;
//     } else if (duration > 12000) {
//       isSynced = false;
//     }
//   } else {
//     // Перехід HIGH -> LOW: вимірюємо тривалість HIGH-імпульсу
//     if (isSynced) {
//       if (duration >= 150 && duration <= 1000) {
//         shiftRegister <<= 1;
//         if (duration > 480) { // ~700 мкс -> '1', ~300 мкс -> '0'
//           shiftRegister |= 1;
//         }
//         bitCount++;

//         // Стандартний 24-бітний EV1527 пакет
//         if (bitCount == 24) {
//           capturedCode = shiftRegister;
//           newCodeReady = true;
//           isSynced = false;
//         }
//       } else {
//         isSynced = false;
//         bitCount = 0;
//       }
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   pinMode(RF_RX_PIN, INPUT);
//   attachInterrupt(digitalPinToInterrupt(RF_RX_PIN), rfISR, CHANGE);

//   Serial.println("\n=== Wall Switch RF Decoder (24-Bit EV1527) Ready ===");
//   Serial.println("Клацайте настінні вимикачі...");
// }

// uint32_t candidateCode = 0;
// uint32_t candidateTime = 0;
// uint32_t lastConfirmedCode = 0;
// uint32_t lastConfirmedTime = 0;

// void loop() {
//   if (newCodeReady) {
//     uint32_t code = capturedCode;
//     newCodeReady = false;

//     uint32_t now = millis();

//     // Фільтр 2-Match
//     if (code == candidateCode && (now - candidateTime < 350)) {
//       if (code != lastConfirmedCode || (now - lastConfirmedTime > 400)) {
        
//         uint32_t switchID = (code >> 4) & 0xFFFFF; // 20 біт — унікальний ID вимикача
//         uint8_t btnCmd    = code & 0x0F;            // 4 біти — кнопка / команда

//         Serial.printf("💡 ВИМИКАЧ: HEX = 0x%06X | ID: 0x%05X | Кнопка: 0x%X\n", 
//                       code, switchID, btnCmd);
        
//         lastConfirmedCode = code;
//         lastConfirmedTime = now;
//       }
//     } else {
//       candidateCode = code;
//       candidateTime = now;
//     }
//   }
// }

































































// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <WebServer.h> 
// #include <Update.h>    
// #include <WireGuard-ESP32.h> 

// // ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ (LED) =================
// const int pwmPin1     = 5;  
// const int pwmChannel1 = 0;     

// const int pwmPin2     = 6;  
// const int pwmChannel2 = 1;     

// const int frequency  = 2000;   // Частота 2кГц (оптимально проти миготіння та биття)
// const int resolution = 10;     // 10 біт (0-1023)

// float currentPwmPct1 = 0.0f;   
// float currentPwmPct2 = 0.0f;   

// bool updateRequired1 = true;   
// bool updateRequired2 = true;   

// // ================= НАЛАШТУВАННЯ RF 433 МГц =================
// #define RF_RX_PIN 16

// // Хекс-коди ваших 6 вимикачів EV1527
// const uint32_t RF_SWITCH_1 = 0xA20551; // Канал 1
// const uint32_t RF_SWITCH_2 = 0xA03761; // Канал 2
// const uint32_t RF_SWITCH_3 = 0xA085A1; // Канал 3 (на майбутнє)
// const uint32_t RF_SWITCH_4 = 0xA08381; // Канал 4 (на майбутнє)
// const uint32_t RF_SWITCH_5 = 0xA203C1; // Канал 5 (на майбутнє)
// const uint32_t RF_SWITCH_6 = 0xA18271; // Канал 6 (на майбутнє)

// // 4 Рівні яскравості (пресети) для перемикання кнопкою (в %)
// float pwmPresets[4] = { 25.0f, 50.0f, 75.0f, 100.0f };

// // Поточний стан перемикача (0 = Вимк, 1..4 = Пресети 1..4)
// uint8_t stateIndex1 = 0;
// uint8_t stateIndex2 = 0;

// // Змінні переривання RF
// volatile uint32_t shiftRegister = 0;
// volatile uint8_t bitCount = 0;
// volatile uint32_t lastChangeTime = 0;
// volatile uint32_t capturedCode = 0;
// volatile bool newCodeReady = false;
// volatile bool isSynced = false;

// // Змінні фільтрації 2-Match
// uint32_t candidateCode = 0;
// uint32_t candidateTime = 0;
// uint32_t lastConfirmedCode = 0;
// uint32_t lastConfirmedTime = 0;

// // ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
// const char* ssid     = "GW_1_2.4G";
// const char* pass     = "GreenWave2015";

// const char* mqtt_srv  = "164.92.129.221";  
// const int   mqtt_port = 1883;
// const char* mqtt_user = "LedControl";  
// const char* mqtt_pass = "LedPass";   
  
// const char* topicStatus1   = "smartHome/user1/Led1/status";
// const char* topicCommands1 = "smartHome/user1/Led1/commands";

// const char* topicStatus2   = "smartHome/user1/Led2/status";
// const char* topicCommands2 = "smartHome/user1/Led2/commands";

// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// // ================= НАЛАШТУВАННЯ WIREGUARD VPN (#Vitalik_VPN_2) =================
// const char* wg_local_ip    = "10.0.0.38"; 
// const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
// const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; 
// const char* wg_endpoint    = "161.35.78.1"; 
// const int   wg_port        = 51820;        

// WireGuard wg;
// bool vpnConnected = false; 

// // ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
// WebServer server(80);
// bool isUpdating = false;

// const char* serverIndex = R"html(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset='UTF-8'>
//     <meta name='viewport' content='width=device-width, initial-scale=1.0'>
//     <style>
//         body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
//         .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
//         h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
//         input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
//         input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
//         input[type=submit]:active { background: #218838; }
//     </style>
// </head>
// <body>
//     <div class='card'>
//         <h2>Оновлення прошивки Dual LED модуля через VPN</h2>
//         <form method='POST' action='/update' enctype='multipart/form-data'>
//             <input type='file' name='update' required accept='.bin'>
//             <input type='submit' value='Оновити контролер'>
//         </form>
//     </div>
// </body>
// </html>
// )html";

// uint32_t periodStatusIdleMs = 10000; 
// uint32_t lastStatus = 0;
// uint32_t lastLog = 0;

// void forceImmediateStatusPublish() { lastStatus = 0; }

// // ================= ISR ДЕКОДЕР RF 433 МГц =================
// void IRAM_ATTR rfISR() {
//   uint32_t now = micros();
//   uint32_t duration = now - lastChangeTime;
//   lastChangeTime = now;

//   if (duration < 100) return; 

//   bool currentState = digitalRead(RF_RX_PIN);

//   if (currentState == HIGH) {
//     if (duration >= 2500 && duration <= 12000) {
//       isSynced = true;
//       bitCount = 0;
//       shiftRegister = 0;
//     } else if (duration > 12000) {
//       isSynced = false;
//     }
//   } else {
//     if (isSynced) {
//       if (duration >= 150 && duration <= 1000) {
//         shiftRegister <<= 1;
//         if (duration > 480) {
//           shiftRegister |= 1;
//         }
//         bitCount++;

//         if (bitCount == 24) {
//           capturedCode = shiftRegister;
//           newCodeReady = true;
//           isSynced = false;
//         }
//       } else {
//         isSynced = false;
//         bitCount = 0;
//       }
//     }
//   }
// }

// // ================= ФУНКЦІЇ ПУБЛІКАЦІЇ СТАТУСУ MQTT =================
// void publishStatus1() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct1 * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   if (mqttClient.publish(topicStatus1, out.c_str(), false)) {
//     Serial.printf("📤 [MQTT OUT] Статус Led1 -> %s\n", out.c_str());
//   } else {
//     Serial.println("❌ [MQTT OUT] Помилка відправки статусу Led1!");
//   }
// }

// void publishStatus2() {
//   if (isUpdating) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct2 * 10.0f) / 10.0f;

//   String out;
//   serializeJson(d, out);
  
//   if (mqttClient.publish(topicStatus2, out.c_str(), false)) {
//     Serial.printf("📤 [MQTT OUT] Статус Led2 -> %s\n", out.c_str());
//   } else {
//     Serial.println("❌ [MQTT OUT] Помилка відправки статусу Led2!");
//   }
// }

// void publishAllStatuses() {
//   publishStatus1();
//   publishStatus2();
//   lastStatus = millis();
// }

// // ================= ОБРОБКА КЛИКІВ ВИМИКАЧІВ =================
// void processRfSwitchClick(uint32_t code) {
//   if (code == RF_SWITCH_1) {
//     stateIndex1 = (stateIndex1 + 1) % 5; // Стани 0, 1, 2, 3, 4
//     currentPwmPct1 = (stateIndex1 == 0) ? 0.0f : pwmPresets[stateIndex1 - 1];
//     updateRequired1 = true;
//     Serial.printf("🔘 [RF BUTTON 1] Натиснуто! Канал 1 -> Стан: %d | Яскравість: %.1f%%\n", stateIndex1, currentPwmPct1);
//     publishStatus1();
//   } 
//   else if (code == RF_SWITCH_2) {
//     stateIndex2 = (stateIndex2 + 1) % 5;
//     currentPwmPct2 = (stateIndex2 == 0) ? 0.0f : pwmPresets[stateIndex2 - 1];
//     updateRequired2 = true;
//     Serial.printf("🔘 [RF BUTTON 2] Натиснуто! Канал 2 -> Стан: %d | Яскравість: %.1f%%\n", stateIndex2, currentPwmPct2);
//     publishStatus2();
//   }
//   else if (code == RF_SWITCH_3) {
//     Serial.println("🔘 [RF BUTTON 3] Натиснуто! (Зарезервовано під Канал 3)");
//   }
//   else if (code == RF_SWITCH_4) {
//     Serial.println("🔘 [RF BUTTON 4] Натиснуто! (Зарезервовано під Канал 4)");
//   }
//   else if (code == RF_SWITCH_5) {
//     Serial.println("🔘 [RF BUTTON 5] Натиснуто! (Зарезервовано під Канал 5)");
//   }
//   else if (code == RF_SWITCH_6) {
//     Serial.println("🔘 [RF BUTTON 6] Натиснуто! (Зарезервовано під Канал 6)");
//   }
// }

// // ================= ОБРОБКА MQTT КОМАНД =================
// void callback(char* topic, byte* payload, unsigned int len) {
//   String msg = "";
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
//   Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

//   if (isUpdating) return; 

//   String topicStr = String(topic);

//   if (topicStr == topicCommands1 || topicStr == topicCommands2) {
//     JsonDocument d;
//     DeserializationError err = deserializeJson(d, msg);

//     if (!err) {
//       // 1. Перевірка оновлення пресетів set_pwm_1 .. set_pwm_4
//       bool presetsUpdated = false;
//       if (!d["set_pwm_1"].isNull()) { pwmPresets[0] = d["set_pwm_1"].as<float>(); presetsUpdated = true; }
//       if (!d["set_pwm_2"].isNull()) { pwmPresets[1] = d["set_pwm_2"].as<float>(); presetsUpdated = true; }
//       if (!d["set_pwm_3"].isNull()) { pwmPresets[2] = d["set_pwm_3"].as<float>(); presetsUpdated = true; }
//       if (!d["set_pwm_4"].isNull()) { pwmPresets[3] = d["set_pwm_4"].as<float>(); presetsUpdated = true; }

//       if (presetsUpdated) {
//         Serial.printf("⚙️ [PRESETS UPDATED] Нові рівні: [1]: %.1f%% | [2]: %.1f%% | [3]: %.1f%% | [4]: %.1f%%\n",
//                       pwmPresets[0], pwmPresets[1], pwmPresets[2], pwmPresets[3]);
//       }

//       // 2. Пряме керування ярскістю pwm_pct
//       if (!d["pwm_pct"].isNull()) {
//         float pct = d["pwm_pct"].as<float>();
//         if (pct >= 0.0f && pct <= 100.0f) {
//           if (topicStr == topicCommands1) {
//             currentPwmPct1 = pct;
//             updateRequired1 = true;
//             publishStatus1();
//           } else if (topicStr == topicCommands2) {
//             currentPwmPct2 = pct;
//             updateRequired2 = true;
//             publishStatus2();
//           }
//         }
//       }
//     } 
//     // Якщо прийшло звичайне число (наприклад "50.5")
//     else {
//       float pct = msg.toFloat();
//       if (pct >= 0.0f && pct <= 100.0f) {
//         if (topicStr == topicCommands1) {
//           currentPwmPct1 = pct;
//           updateRequired1 = true;
//           publishStatus1();
//         } else if (topicStr == topicCommands2) {
//           currentPwmPct2 = pct;
//           updateRequired2 = true;
//           publishStatus2();
//         }
//       }
//     }
//   }
// }

// // ================= МЕРЕЖЕВІ ФУНКЦІЇ =================
// void connectWiFi() {
//   if (WiFi.status() == WL_CONNECTED) return;
  
//   Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
//   WiFi.begin(ssid, pass);
  
//   int timeoutCounter = 0;
//   while (WiFi.status() != WL_CONNECTED) { 
//     delay(500); 
//     Serial.print(".");
//     timeoutCounter++;
//     if (timeoutCounter > 40) { 
//       Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
//       return;
//     }
//   }
//   Serial.println("\n✅ Wi-Fi успішно підключено!");
//   Serial.print("🏠 Локальна IP-адреса: ");
//   Serial.println(WiFi.localIP());
// }

// void connectVPN() {
//   if (vpnConnected) return; 
  
//   connectWiFi(); 

//   Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
//   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

//   Serial.print("🕒 Очікування відповіді від NTP серверів...");
//   int attempts = 0;
//   while (time(nullptr) < 1000000000l && attempts < 40) { 
//     delay(500); 
//     Serial.print(".");
//     attempts++;
//   }

//   if (time(nullptr) < 1000000000l) {
//     Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час! Хендшейк може впасти.");
//   } else {
//     time_t nowTime = time(nullptr);
//     Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
//   }

//   Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
//   if (wg.begin(
//         IPAddress(10,0,0,38),          
//         wg_private_key,                
//         wg_endpoint,                    
//         wg_public_key,                 
//         wg_port                        
//       )) {
//       Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
//       vpnConnected = true;
//   } else {
//       Serial.println("❌ Помилка старту крипто-графічного ядра WireGuard.");
//       vpnConnected = false;
//   }
// }

// void connectMQTT() {
//   if (isUpdating) return; 
  
//   while (!mqttClient.connected()) {
//     connectVPN(); 
    
//     String clientId = "ESP32-DualLED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
//     Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
//     if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus1, 0, false, "{\"state\":\"offline\"}")) {
      
//       bool sub1 = mqttClient.subscribe(topicCommands1); 
//       bool sub2 = mqttClient.subscribe(topicCommands2); 

//       if(sub1 && sub2) {
//         Serial.printf("📥 Успішна підписка на топіки:\n - %s\n - %s\n", topicCommands1, topicCommands2);
//       } else {
//         Serial.println("❌ Помилка підписки на один із топіків команд!");
//       }
      
//       forceImmediateStatusPublish();
//       Serial.println("🚀 Зв'язок з брокером встановлено. Двоканальний контролер онлайн.");
//     } else {
//       Serial.printf("❌ Помилка MQTT з'єднання. Код стану rc = %d. Повтор через 2 сек...\n", mqttClient.state());
//       delay(2000);
//     }
//   }
// }

// void setupWebOTA() {
//   server.on("/", HTTP_GET, []() {
//       server.sendHeader("Connection", "close");
//       server.send(200, "text/html", serverIndex);
//   });

//   server.on("/update", HTTP_POST, []() {
//       server.sendHeader("Connection", "close");
//       String responseHtml;
//       if (Update.hasError()) {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
//       } else {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
//       }
//       server.send(200, "text/html", responseHtml);
//       delay(1200);
//       ESP.restart();
//   }, []() {
//       HTTPUpload& upload = server.upload();
//       if (upload.status == UPLOAD_FILE_START) {
//           isUpdating = true;
//           Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
//           if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//           if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_END) {
//           if (!Update.end(true)) { 
//             Update.printError(Serial); 
//           } else {
//             Serial.println("✅ [OTA] Файл успішно записано в Flash!");
//           }
//           isUpdating = false;
//       }
//   });
//   server.begin();
//   Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
// }

// // ================= SETUP =================
// void setup() {
//   Serial.begin(115200);
//   delay(500); 
//   Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: DUAL LED + RF433 NODE STARTED ===");
  
//   // ШИМ Канал 1
//   ledcSetup(pwmChannel1, frequency, resolution);
//   ledcAttachPin(pwmPin1, pwmChannel1);

//   // ШИМ Канал 2
//   ledcSetup(pwmChannel2, frequency, resolution);
//   ledcAttachPin(pwmPin2, pwmChannel2);

//   // Налаштування приймача RF 433
//   pinMode(RF_RX_PIN, INPUT);
//   attachInterrupt(digitalPinToInterrupt(RF_RX_PIN), rfISR, CHANGE);

//   mqttClient.setServer(mqtt_srv, mqtt_port);
//   mqttClient.setCallback(callback);
//   mqttClient.setBufferSize(512); 

//   connectWiFi();
//   setupWebOTA(); 
//   connectMQTT();
// }

// // ================= LOOP =================
// void loop() {
//   server.handleClient();
//   if (isUpdating) return; 

//   if (!mqttClient.connected()) {
//     Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
//     connectMQTT();
//   }
//   mqttClient.loop();

//   uint32_t now = millis();

//   // === ОБРОБКА СИГНАЛІВ RF 433 МГц ===
//   if (newCodeReady) {
//     uint32_t code = capturedCode;
//     newCodeReady = false;

//     // Фільтрація 2-Match
//     if (code == candidateCode && (now - candidateTime < 350)) {
//       if (code != lastConfirmedCode || (now - lastConfirmedTime > 400)) {
        
//         processRfSwitchClick(code);
        
//         lastConfirmedCode = code;
//         lastConfirmedTime = now;
//       }
//     } else {
//       candidateCode = code;
//       candidateTime = now;
//     }
//   }

//   // Оновлення ШИМ Каналу 1
//   if (updateRequired1) {
//     updateRequired1 = false;
//     int targetDuty1 = (int)((currentPwmPct1 / 100.0f) * 1023.0f);
//     targetDuty1 = constrain(targetDuty1, 0, 1023);
    
//     ledcWrite(pwmChannel1, targetDuty1);
//     Serial.printf("⚙️ [HARDWARE] Led1 ШИМ: %.1f%% | Duty: %d\n", currentPwmPct1, targetDuty1);
//   }

//   // Оновлення ШИМ Каналу 2
//   if (updateRequired2) {
//     updateRequired2 = false;
//     int targetDuty2 = (int)((currentPwmPct2 / 100.0f) * 1023.0f);
//     targetDuty2 = constrain(targetDuty2, 0, 1023);
    
//     ledcWrite(pwmChannel2, targetDuty2);
//     Serial.printf("⚙️ [HARDWARE] Led2 ШИМ: %.1f%% | Duty: %d\n", currentPwmPct2, targetDuty2);
//   }

//   // Періодична публікація статусу
//   if (now - lastStatus >= periodStatusIdleMs) {
//     publishAllStatuses();
//   }

//   // Heartbeat логування
//   if (now - lastLog > 5000) {
//     lastLog = now;
//     int duty1 = constrain((int)((currentPwmPct1 / 100.0f) * 1023.0f), 0, 1023);
//     int duty2 = constrain((int)((currentPwmPct2 / 100.0f) * 1023.0f), 0, 1023);

//     Serial.printf("[HEARTBEAT] Led1: %.1f%% (State: %d) | Led2: %.1f%% (State: %d) | WiFi: %s | VPN: %s | MQTT: %s\n",
//                   currentPwmPct1, stateIndex1, 
//                   currentPwmPct2, stateIndex2,
//                   (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
//                   vpnConnected ? "UP" : "DOWN",
//                   mqttClient.connected() ? "CONNECTED" : "OFFLINE");
//   }

//   delay(10);
// }








// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <WebServer.h> 
// #include <Update.h>    
// #include <WireGuard-ESP32.h> 

// // ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ =================
// #define NUM_CHANNELS 6

// // ШИМ піни для 6 каналів світлодіодів
// const int pwmPins[NUM_CHANNELS]   = { 5,  6,  7, 15, 17, 18 }; 
// // Цифрові піни для керування 6 Мосфетами/Реле
// const int relayPins[NUM_CHANNELS] = { 4,  8,  9, 10, 11, 12 }; 

// const int frequency  = 2000;   // 2 кГц (без миготіння та писку)
// const int resolution = 10;     // 10 біт (0-1023)

// float currentPwmPct[NUM_CHANNELS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
// bool updateRequired[NUM_CHANNELS] = { true, true, true, true, true, true };
// uint8_t stateIndex[NUM_CHANNELS]  = { 0, 0, 0, 0, 0, 0 };

// // 4 Рівні яскравості (пресети) для перемикання кнопкою (в %)
// float pwmPresets[4] = { 25.0f, 50.0f, 75.0f, 100.0f };

// // ================= НАЛАШТУВАННЯ RF 433 МГц =================
// #define RF_RX_PIN 16

// // Хекс-коди ваших 6 вимикачів EV1527
// const uint32_t RF_SWITCHES[NUM_CHANNELS] = {
//   0xA20551, // Вимикач 1 -> Канал 1
//   0xA03761, // Вимикач 2 -> Канал 2
//   0xA085A1, // Вимикач 3 -> Канал 3
//   0xA08381, // Вимикач 4 -> Канал 4
//   0xA203C1, // Вимикач 5 -> Канал 5
//   0xA18271  // Вимикач 6 -> Канал 6
// };

// // Змінні переривання RF
// volatile uint32_t shiftRegister = 0;
// volatile uint8_t bitCount = 0;
// volatile uint32_t lastChangeTime = 0;
// volatile uint32_t capturedCode = 0;
// volatile bool newCodeReady = false;
// volatile bool isSynced = false;

// // Змінні фільтрації 2-Match
// uint32_t candidateCode = 0;
// uint32_t candidateTime = 0;
// uint32_t lastConfirmedCode = 0;
// uint32_t lastConfirmedTime = 0;

// // ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
// const char* ssid     = "GW_1_2.4G";
// const char* pass     = "GreenWave2015";

// const char* mqtt_srv  = "164.92.129.221";  
// const int   mqtt_port = 1883;
// const char* mqtt_user = "LedControl";  
// const char* mqtt_pass = "LedPass";   
  
// // Динамічно генеровані топіки для 6 каналів
// String topicStatus[NUM_CHANNELS];
// String topicCommands[NUM_CHANNELS];

// WiFiClient espClient;
// PubSubClient mqttClient(espClient);

// // ================= НАЛАШТУВАННЯ WIREGUARD VPN =================
// const char* wg_local_ip    = "10.0.0.38"; 
// const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
// const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; 
// const char* wg_endpoint    = "161.35.78.1"; 
// const int   wg_port        = 51820;        

// WireGuard wg;
// bool vpnConnected = false; 

// // ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
// WebServer server(80);
// bool isUpdating = false;

// const char* serverIndex = R"html(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset='UTF-8'>
//     <meta name='viewport' content='width=device-width, initial-scale=1.0'>
//     <style>
//         body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
//         .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
//         h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
//         input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
//         input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
//         input[type=submit]:active { background: #218838; }
//     </style>
// </head>
// <body>
//     <div class='card'>
//         <h2>Оновлення прошивки 6-Ch LED модуля через VPN</h2>
//         <form method='POST' action='/update' enctype='multipart/form-data'>
//             <input type='file' name='update' required accept='.bin'>
//             <input type='submit' value='Оновити контролер'>
//         </form>
//     </div>
// </body>
// </html>
// )html";

// uint32_t periodStatusIdleMs = 10000; 
// uint32_t lastStatus = 0;
// uint32_t lastLog = 0;

// void forceImmediateStatusPublish() { lastStatus = 0; }

// // ================= ISR ДЕКОДЕР RF 433 МГц =================
// void IRAM_ATTR rfISR() {
//   uint32_t now = micros();
//   uint32_t duration = now - lastChangeTime;
//   lastChangeTime = now;

//   if (duration < 100) return; 

//   bool currentState = digitalRead(RF_RX_PIN);

//   if (currentState == HIGH) {
//     if (duration >= 2500 && duration <= 12000) {
//       isSynced = true;
//       bitCount = 0;
//       shiftRegister = 0;
//     } else if (duration > 12000) {
//       isSynced = false;
//     }
//   } else {
//     if (isSynced) {
//       if (duration >= 150 && duration <= 1000) {
//         shiftRegister <<= 1;
//         if (duration > 480) {
//           shiftRegister |= 1;
//         }
//         bitCount++;

//         if (bitCount == 24) {
//           capturedCode = shiftRegister;
//           newCodeReady = true;
//           isSynced = false;
//         }
//       } else {
//         isSynced = false;
//         bitCount = 0;
//       }
//     }
//   }
// }

// // ================= ПУБЛІКАЦІЯ СТАТУСІВ В МЕРЕЖУ =================
// void publishStatus(int ch) {
//   if (isUpdating || ch < 0 || ch >= NUM_CHANNELS) return; 
//   JsonDocument d; 
//   d["pwm_pct"] = round(currentPwmPct[ch] * 10.0f) / 10.0f;
//   d["relay"]   = (currentPwmPct[ch] > 0.0f) ? true : false;

//   String out;
//   serializeJson(d, out);
  
//   if (mqttClient.publish(topicStatus[ch].c_str(), out.c_str(), false)) {
//     Serial.printf("📤 [MQTT OUT] Статус Led%d -> %s\n", ch + 1, out.c_str());
//   } else {
//     Serial.printf("❌ [MQTT OUT] Помилка відправки статусу Led%d!\n", ch + 1);
//   }
// }

// void publishAllStatuses() {
//   for (int i = 0; i < NUM_CHANNELS; i++) {
//     publishStatus(i);
//   }
//   lastStatus = millis();
// }

// // ================= ОБРОБКА КЛИКІВ RF ВИМИКАЧІВ =================
// void processRfSwitchClick(uint32_t code) {
//   for (int i = 0; i < NUM_CHANNELS; i++) {
//     if (code == RF_SWITCHES[i]) {
//       stateIndex[i] = (stateIndex[i] + 1) % 5; // Стани: 0 (Off), 1, 2, 3, 4
//       currentPwmPct[i] = (stateIndex[i] == 0) ? 0.0f : pwmPresets[stateIndex[i] - 1];
//       updateRequired[i] = true;
      
//       Serial.printf("🔘 [RF BUTTON %d] Натиснуто! Канал %d -> Стан: %d | Яскравість: %.1f%%\n", 
//                     i + 1, i + 1, stateIndex[i], currentPwmPct[i]);
      
//       publishStatus(i);
//       return;
//     }
//   }
// }

// // ================= ОБРОБКА ВХІДНИХ MQTT КОМАНД (ДЛЯ ВСІХ 6 КАНАЛІВ) =================
// void callback(char* topic, byte* payload, unsigned int len) {
//   String msg = "";
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
//   Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

//   if (isUpdating) return; 

//   String topicStr = String(topic);

//   for (int i = 0; i < NUM_CHANNELS; i++) {
//     if (topicStr == topicCommands[i]) {
//       JsonDocument d;
//       DeserializationError err = deserializeJson(d, msg);

//       if (!err) {
//         // Оновлення конфігурації пресетів при надсиланні set_pwm_X
//         bool presetsUpdated = false;
//         if (!d["set_pwm_1"].isNull()) { pwmPresets[0] = d["set_pwm_1"].as<float>(); presetsUpdated = true; }
//         if (!d["set_pwm_2"].isNull()) { pwmPresets[1] = d["set_pwm_2"].as<float>(); presetsUpdated = true; }
//         if (!d["set_pwm_3"].isNull()) { pwmPresets[2] = d["set_pwm_3"].as<float>(); presetsUpdated = true; }
//         if (!d["set_pwm_4"].isNull()) { pwmPresets[3] = d["set_pwm_4"].as<float>(); presetsUpdated = true; }

//         if (presetsUpdated) {
//           Serial.printf("⚙️ [PRESETS UPDATED] Нові рівні: [1]: %.1f%% | [2]: %.1f%% | [3]: %.1f%% | [4]: %.1f%%\n",
//                         pwmPresets[0], pwmPresets[1], pwmPresets[2], pwmPresets[3]);
//         }

//         // Встановлення прямого значення яскравості
//         if (!d["pwm_pct"].isNull()) {
//           float pct = d["pwm_pct"].as<float>();
//           if (pct >= 0.0f && pct <= 100.0f) {
//             currentPwmPct[i] = pct;
//             updateRequired[i] = true;
//             publishStatus(i);
//           }
//         }
//       } 
//       // Парсинг числового значення (наприклад "75")
//       else {
//         float pct = msg.toFloat();
//         if (pct >= 0.0f && pct <= 100.0f) {
//           currentPwmPct[i] = pct;
//           updateRequired[i] = true;
//           publishStatus(i);
//         }
//       }
//       break;
//     }
//   }
// }

// // ================= МЕРЕЖЕВІ ФУНКЦІЇ =================
// void connectWiFi() {
//   if (WiFi.status() == WL_CONNECTED) return;
  
//   Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
//   WiFi.begin(ssid, pass);
  
//   int timeoutCounter = 0;
//   while (WiFi.status() != WL_CONNECTED) { 
//     delay(500); 
//     Serial.print(".");
//     timeoutCounter++;
//     if (timeoutCounter > 40) { 
//       Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
//       return;
//     }
//   }
//   Serial.println("\n✅ Wi-Fi успішно підключено!");
//   Serial.print("🏠 Локальна IP-адреса: ");
//   Serial.println(WiFi.localIP());
// }

// void connectVPN() {
//   if (vpnConnected) return; 
  
//   connectWiFi(); 

//   Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
//   configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

//   Serial.print("🕒 Очікування відповіді від NTP серверів...");
//   int attempts = 0;
//   while (time(nullptr) < 1000000000l && attempts < 40) { 
//     delay(500); 
//     Serial.print(".");
//     attempts++;
//   }

//   if (time(nullptr) < 1000000000l) {
//     Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час!");
//   } else {
//     time_t nowTime = time(nullptr);
//     Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
//   }

//   Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
//   if (wg.begin(
//         IPAddress(10,0,0,38),          
//         wg_private_key,                
//         wg_endpoint,                    
//         wg_public_key,                 
//         wg_port                        
//       )) {
//       Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
//       vpnConnected = true;
//   } else {
//       Serial.println("❌ Помилка старту WireGuard.");
//       vpnConnected = false;
//   }
// }

// void connectMQTT() {
//   if (isUpdating) return; 
  
//   while (!mqttClient.connected()) {
//     connectVPN(); 
    
//     String clientId = "ESP32-6CH-LED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
//     Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
//     if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus[0].c_str(), 0, false, "{\"state\":\"offline\"}")) {
      
//       bool allSubscribed = true;
//       Serial.println("📥 Підписка на 6 каналів команд:");
//       for (int i = 0; i < NUM_CHANNELS; i++) {
//         if (mqttClient.subscribe(topicCommands[i].c_str())) {
//           Serial.printf(" - %s [OK]\n", topicCommands[i].c_str());
//         } else {
//           Serial.printf(" - %s [ПОМИЛКА]\n", topicCommands[i].c_str());
//           allSubscribed = false;
//         }
//       }
      
//       forceImmediateStatusPublish();
//       Serial.println("🚀 Зв'язок з брокером встановлено. 6-канальний контролер онлайн.");
//     } else {
//       Serial.printf("❌ Помилка MQTT з'єднання. Код = %d. Повтор через 2 сек...\n", mqttClient.state());
//       delay(2000);
//     }
//   }
// }

// void setupWebOTA() {
//   server.on("/", HTTP_GET, []() {
//       server.sendHeader("Connection", "close");
//       server.send(200, "text/html", serverIndex);
//   });

//   server.on("/update", HTTP_POST, []() {
//       server.sendHeader("Connection", "close");
//       String responseHtml;
//       if (Update.hasError()) {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
//       } else {
//           responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
//       }
//       server.send(200, "text/html", responseHtml);
//       delay(1200);
//       ESP.restart();
//   }, []() {
//       HTTPUpload& upload = server.upload();
//       if (upload.status == UPLOAD_FILE_START) {
//           isUpdating = true;
//           Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
//           if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//           if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
//       } else if (upload.status == UPLOAD_FILE_END) {
//           if (!Update.end(true)) { 
//             Update.printError(Serial); 
//           } else {
//             Serial.println("✅ [OTA] Файл успішно записано в Flash!");
//           }
//           isUpdating = false;
//       }
//   });
//   server.begin();
//   Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
// }

// // ================= SETUP =================
// void setup() {
//   Serial.begin(115200);
//   delay(500); 
//   Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: 6-CHANNEL LED + RELAY + RF433 STARTED ===");
  
//   // Ініціалізація назв топіків та апаратної частини 6 каналів
//   for (int i = 0; i < NUM_CHANNELS; i++) {
//     topicStatus[i]   = "smartHome/user1/Led" + String(i + 1) + "/status";
//     topicCommands[i] = "smartHome/user1/Led" + String(i + 1) + "/commands";

//     // Налаштування PWM
//     ledcSetup(i, frequency, resolution);
//     ledcAttachPin(pwmPins[i], i);

//     // Налаштування пінів Мосфетів / Реле
//     pinMode(relayPins[i], OUTPUT);
//     digitalWrite(relayPins[i], LOW);
//   }

//   // Налаштування приймача RF 433
//   pinMode(RF_RX_PIN, INPUT);
//   attachInterrupt(digitalPinToInterrupt(RF_RX_PIN), rfISR, CHANGE);

//   mqttClient.setServer(mqtt_srv, mqtt_port);
//   mqttClient.setCallback(callback);
//   mqttClient.setBufferSize(1024); 

//   connectWiFi();
//   setupWebOTA(); 
//   connectMQTT();
// }

// // ================= LOOP =================
// void loop() {
//   server.handleClient();
//   if (isUpdating) return; 

//   if (!mqttClient.connected()) {
//     Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
//     connectMQTT();
//   }
//   mqttClient.loop();

//   uint32_t now = millis();

//   // === ОБРОБКА СИГНАЛІВ RF 433 МГц ===
//   if (newCodeReady) {
//     uint32_t code = capturedCode;
//     newCodeReady = false;

//     // Фільтрація 2-Match
//     if (code == candidateCode && (now - candidateTime < 350)) {
//       if (code != lastConfirmedCode || (now - lastConfirmedTime > 400)) {
        
//         processRfSwitchClick(code);
        
//         lastConfirmedCode = code;
//         lastConfirmedTime = now;
//       }
//     } else {
//       candidateCode = code;
//       candidateTime = now;
//     }
//   }

//   // === ОНОВЛЕННЯ 6 КАНАЛІВ ШИМ ТА РЕЛЕ ===
//   for (int i = 0; i < NUM_CHANNELS; i++) {
//     if (updateRequired[i]) {
//       updateRequired[i] = false;
      
//       int targetDuty = (int)((currentPwmPct[i] / 100.0f) * 1023.0f);
//       targetDuty = constrain(targetDuty, 0, 1023);
      
//       // 1. Встановлюємо ШИМ
//       ledcWrite(i, targetDuty);

//       // 2. Авто-керування Реле/Мосфетом (Вмикаємо при >0%, Вимикаємо при 0%)
//       if (currentPwmPct[i] > 0.0f) {
//         digitalWrite(relayPins[i], HIGH);
//       } else {
//         digitalWrite(relayPins[i], LOW);
//       }

//       Serial.printf("⚙️ [HW CH%d] ШИМ Pin %d: %.1f%% (Duty %d) | Relay Pin %d: %s\n", 
//                     i + 1, pwmPins[i], currentPwmPct[i], targetDuty, relayPins[i], 
//                     (currentPwmPct[i] > 0.0f) ? "HIGH" : "LOW");
//     }
//   }

//   // Періодична публікація статусу всіх каналів
//   if (now - lastStatus >= periodStatusIdleMs) {
//     publishAllStatuses();
//   }

//   // Heartbeat логування
//   if (now - lastLog > 5000) {
//     lastLog = now;
//     Serial.printf("[HEARTBEAT] Channels Active | WiFi: %s | VPN: %s | MQTT: %s\n",
//                   (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
//                   vpnConnected ? "UP" : "DOWN",
//                   mqttClient.connected() ? "CONNECTED" : "OFFLINE");
//   }

//   delay(10);
// }

























#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WebServer.h> 
#include <Update.h>    
#include <WireGuard-ESP32.h> 

// ================= НАЛАШТУВАННЯ АПАРАТНОЇ ЧАСТИНИ =================
#define NUM_CHANNELS 6

// ШИМ піни для 6 каналів світлодіодів
const int pwmPins[NUM_CHANNELS]   = { 5,  6,  7, 15, 17, 18 }; 
// Цифрові піни для керування 6 Мосфетами/Реле
const int relayPins[NUM_CHANNELS] = { 4,  8,  9, 10, 11, 12 }; 

const int frequency  = 2000;   // 2 кГц (без миготіння та писку)
const int resolution = 10;     // 10 біт (0-1023)

float currentPwmPct[NUM_CHANNELS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
bool updateRequired[NUM_CHANNELS] = { true, true, true, true, true, true };
uint8_t stateIndex[NUM_CHANNELS]  = { 0, 0, 0, 0, 0, 0 };

// --- ТАЙМЕРИ ЗАГАСАННЯ РЕЛЕ ---
const uint32_t RELAY_OFF_DELAY_MS = 10000; // 10 секунд затримки перед розмиканням реле
uint32_t relayOffTimestamp[NUM_CHANNELS] = { 0, 0, 0, 0, 0, 0 };
bool relayPendingOff[NUM_CHANNELS]       = { false, false, false, false, false, false };

// 4 Рівні яскравості (пресети) для перемикання кнопкою (в %)
float pwmPresets[4] = { 25.0f, 50.0f, 75.0f, 100.0f };

// ================= НАЛАШТУВАННЯ RF 433 МГц =================
#define RF_RX_PIN 16

// Хекс-коди ваших 6 вимикачів EV1527
const uint32_t RF_SWITCHES[NUM_CHANNELS] = {
  0xA20551, // Вимикач 1 -> Канал 1
  0xA03761, // Вимикач 2 -> Канал 2
  0xA085A1, // Вимикач 3 -> Канал 3
  0xA08381, // Вимикач 4 -> Канал 4
  0xA203C1, // Вимикач 5 -> Канал 5
  0xA18271  // Вимикач 6 -> Канал 6
};

// Змінні переривання RF
volatile uint32_t shiftRegister = 0;
volatile uint8_t bitCount = 0;
volatile uint32_t lastChangeTime = 0;
volatile uint32_t capturedCode = 0;
volatile bool newCodeReady = false;
volatile bool isSynced = false;

// Змінні фільтрації 2-Match
uint32_t candidateCode = 0;
uint32_t candidateTime = 0;
uint32_t lastConfirmedCode = 0;
uint32_t lastConfirmedTime = 0;

// ================= НАЛАШТУВАННЯ МЕРЕЖІ ТА ТОПІКІВ =================
const char* ssid     = "97";
const char* pass     = "56086492";

const char* mqtt_srv  = "164.92.129.221";  
const int   mqtt_port = 1883;
const char* mqtt_user = "LedControl";  
const char* mqtt_pass = "LedPass";   
  
// Динамічно генеровані топіки для 6 каналів
String topicStatus[NUM_CHANNELS];
String topicCommands[NUM_CHANNELS];

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ================= НАЛАШТУВАННЯ WIREGUARD VPN =================
const char* wg_local_ip    = "10.0.0.38"; 
const char* wg_private_key = "UNuuGjSEcTexAawLaU76/iFhs+T/dMiqTBmeO/bseEQ="; 
const char* wg_public_key  = "eBaUeGWgcOQfdeyXUGLw9w2t3KmREtbMvJPv6+yPuTA="; 
const char* wg_endpoint    = "161.35.78.1"; 
const int   wg_port        = 51820;        

WireGuard wg;
bool vpnConnected = false; 

// ================= ВЕБ-СЕРВЕР ДЛЯ OTA ОНОВЛЕНЬ =================
WebServer server(80);
bool isUpdating = false;

const char* serverIndex = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <style>
        body { font-family: sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; padding: 15px; box-sizing: border-box; }
        .card { background: white; padding: 30px 20px; border-radius: 16px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); width: 100%; max-width: 380px; text-align: center; box-sizing: border-box; }
        h2 { margin: 0 0 20px 0; color: #111; font-size: 22px; }
        input[type=file] { width: 100%; padding: 12px; background: #f8f9fa; border: 1px solid #ccc; border-radius: 8px; box-sizing: border-box; margin-bottom: 20px; font-size: 15px; }
        input[type=submit] { width: 100%; background: #28a745; color: white; border: none; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; -webkit-appearance: none; }
        input[type=submit]:active { background: #218838; }
    </style>
</head>
<body>
    <div class='card'>
        <h2>Оновлення прошивки 6-Ch LED модуля через VPN</h2>
        <form method='POST' action='/update' enctype='multipart/form-data'>
            <input type='file' name='update' required accept='.bin'>
            <input type='submit' value='Оновити контролер'>
        </form>
    </div>
</body>
</html>
)html";

uint32_t periodStatusIdleMs = 10000; 
uint32_t lastStatus = 0;
uint32_t lastLog = 0;

void forceImmediateStatusPublish() { lastStatus = 0; }

// ================= ISR ДЕКОДЕР RF 433 МГц =================
void IRAM_ATTR rfISR() {
  uint32_t now = micros();
  uint32_t duration = now - lastChangeTime;
  lastChangeTime = now;

  if (duration < 100) return; 

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

// ================= ПУБЛІКАЦІЯ СТАТУСІВ В МЕРЕЖУ =================
void publishStatus(int ch) {
  if (isUpdating || ch < 0 || ch >= NUM_CHANNELS) return; 
  JsonDocument d; 
  d["pwm_pct"] = round(currentPwmPct[ch] * 10.0f) / 10.0f;
  // Показуємо реальний фізичний стан реле (HIGH = true)
  d["relay"]   = (digitalRead(relayPins[ch]) == HIGH);

  String out;
  serializeJson(d, out);
  
  if (mqttClient.publish(topicStatus[ch].c_str(), out.c_str(), false)) {
    Serial.printf("📤 [MQTT OUT] Статус Led%d -> %s\n", ch + 1, out.c_str());
  } else {
    Serial.printf("❌ [MQTT OUT] Помилка відправки статусу Led%d!\n", ch + 1);
  }
}

void publishAllStatuses() {
  for (int i = 0; i < NUM_CHANNELS; i++) {
    publishStatus(i);
  }
  lastStatus = millis();
}

// ================= ОБРОБКА КЛИКІВ RF ВИМИКАЧІВ =================
void processRfSwitchClick(uint32_t code) {
  for (int i = 0; i < NUM_CHANNELS; i++) {
    if (code == RF_SWITCHES[i]) {
      stateIndex[i] = (stateIndex[i] + 1) % 5; // Стани: 0 (Off), 1, 2, 3, 4
      currentPwmPct[i] = (stateIndex[i] == 0) ? 0.0f : pwmPresets[stateIndex[i] - 1];
      updateRequired[i] = true;
      
      Serial.printf("🔘 [RF BUTTON %d] Натиснуто! Канал %d -> Стан: %d | Яскравість: %.1f%%\n", 
                    i + 1, i + 1, stateIndex[i], currentPwmPct[i]);
      
      publishStatus(i);
      return;
    }
  }
}

// ================= ОБРОБКА ВХІДНИХ MQTT КОМАНД =================
void callback(char* topic, byte* payload, unsigned int len) {
  String msg = "";
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  
  Serial.printf("\n📥 [MQTT IN] Отримано пакет! Топік: %s | Пейлоад: %s\n", topic, msg.c_str());

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
          Serial.printf("⚙️ [PRESETS UPDATED] Нові рівні: [1]: %.1f%% | [2]: %.1f%% | [3]: %.1f%% | [4]: %.1f%%\n",
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
      } 
      else {
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

// ================= МЕРЕЖЕВІ ФУНКЦІЇ =================
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  
  Serial.printf("📶 Спроба підключення до Wi-Fi SSID: %s ", ssid);
  WiFi.begin(ssid, pass);
  
  int timeoutCounter = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print(".");
    timeoutCounter++;
    if (timeoutCounter > 40) { 
      Serial.println("\n❌ Таймаут підключення до Wi-Fi!");
      return;
    }
  }
  Serial.println("\n✅ Wi-Fi успішно підключено!");
  Serial.print("🏠 Локальна IP-адреса: ");
  Serial.println(WiFi.localIP());
}

void connectVPN() {
  if (vpnConnected) return; 
  
  connectWiFi(); 

  Serial.println("🔒 Запуск процедури синхронізації часу для WireGuard...");
  configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

  Serial.print("🕒 Очікування відповіді від NTP серверів...");
  int attempts = 0;
  while (time(nullptr) < 1000000000l && attempts < 40) { 
    delay(500); 
    Serial.print(".");
    attempts++;
  }

  if (time(nullptr) < 1000000000l) {
    Serial.println("\n⚠️ [NTP] Не вдалося синхронізувати час!");
  } else {
    time_t nowTime = time(nullptr);
    Serial.printf("\n✅ [NTP] Час синхронізовано! Unix Epoch: %ld\n", nowTime);
  }

  Serial.println("🔒 Ініціалізація крипто-ядра WireGuard...");
  if (wg.begin(
        IPAddress(10,0,0,38),          
        wg_private_key,                
        wg_endpoint,                    
        wg_public_key,                 
        wg_port                        
      )) {
      Serial.printf("✅ VPN тунель успішно піднято! Віртуальний IP пристрою: %s\n", wg_local_ip);
      vpnConnected = true;
  } else {
      Serial.println("❌ Помилка старту WireGuard.");
      vpnConnected = false;
  }
}

void connectMQTT() {
  if (isUpdating) return; 
  
  while (!mqttClient.connected()) {
    connectVPN(); 
    
    String clientId = "ESP32-6CH-LED-Controller-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.printf("🔄 Авторизація на MQTT брокері (%s) через тунель VPN...\n", mqtt_srv);
    
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass, topicStatus[0].c_str(), 0, false, "{\"state\":\"offline\"}")) {
      
      bool allSubscribed = true;
      Serial.println("📥 Підписка на 6 каналів команд:");
      for (int i = 0; i < NUM_CHANNELS; i++) {
        if (mqttClient.subscribe(topicCommands[i].c_str())) {
          Serial.printf(" - %s [OK]\n", topicCommands[i].c_str());
        } else {
          Serial.printf(" - %s [ПОМИЛКА]\n", topicCommands[i].c_str());
          allSubscribed = false;
        }
      }
      
      forceImmediateStatusPublish();
      Serial.println("🚀 Зв'язок з брокером встановлено. 6-канальний контролер онлайн.");
    } else {
      Serial.printf("❌ Помилка MQTT з'єднання. Код = %d. Повтор через 2 сек...\n", mqttClient.state());
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
          responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#dc3545;">Помилка оновлення!</h2></div></body></html>)html";
      } else {
          responseHtml = R"html(<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style="font-family:sans-serif;background:#f0f2f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;"><div style="background:white;padding:40px 20px;border-radius:16px;text-align:center;max-width:340px;width:100%;"><h2 style="color:#28a745;">Успішно!</h2><p>Перезавантаження...</p></div></body></html>)html";
      }
      server.send(200, "text/html", responseHtml);
      delay(1200);
      ESP.restart();
  }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
          isUpdating = true;
          Serial.println("\n📥 [OTA] Початок приймання файлу прошивки...");
          if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Update.printError(Serial); }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Update.printError(Serial); }
      } else if (upload.status == UPLOAD_FILE_END) {
          if (!Update.end(true)) { 
            Update.printError(Serial); 
          } else {
            Serial.println("✅ [OTA] Файл успішно записано в Flash!");
          }
          isUpdating = false;
      }
  });
  server.begin();
  Serial.println("🌐 Локальний Веб-сервер OTA оновлень запущено на порту 80.");
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(500); 
  Serial.println("\n=== ПЕРЕЗАВАНТАЖЕННЯ СИСТЕМИ: 6-CHANNEL LED + RELAY + RF433 STARTED ===");
  
  for (int i = 0; i < NUM_CHANNELS; i++) {
    topicStatus[i]   = "smartHome/user1/Led" + String(i + 1) + "/status";
    topicCommands[i] = "smartHome/user1/Led" + String(i + 1) + "/commands";

    // Налаштування PWM
    ledcSetup(i, frequency, resolution);
    ledcAttachPin(pwmPins[i], i);

    // Налаштування пінів Мосфетів / Реле
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }

  // Налаштування приймача RF 433
  pinMode(RF_RX_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RF_RX_PIN), rfISR, CHANGE);

  mqttClient.setServer(mqtt_srv, mqtt_port);
  mqttClient.setCallback(callback);
  mqttClient.setBufferSize(1024); 

  connectWiFi();
  setupWebOTA(); 
  connectMQTT();
}

// ================= LOOP =================
void loop() {
  server.handleClient();
  if (isUpdating) return; 

  if (!mqttClient.connected()) {
    Serial.println("⚠️ Виявлено розрив зв'язку MQTT! Перепідключення...");
    connectMQTT();
  }
  mqttClient.loop();

  uint32_t now = millis();

  // === ОБРОБКА СИГНАЛІВ RF 433 МГц ===
  if (newCodeReady) {
    uint32_t code = capturedCode;
    newCodeReady = false;

    // Фільтрація 2-Match
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

  // === ОНОВЛЕННЯ 6 КАНАЛІВ ШИМ ТА ЛОГІКИ РЕЛЕ ===
  for (int i = 0; i < NUM_CHANNELS; i++) {
    
    // 1. Якщо прийшла нова команда (з RF або MQTT)
    if (updateRequired[i]) {
      updateRequired[i] = false;
      
      int targetDuty = (int)((currentPwmPct[i] / 100.0f) * 1023.0f);
      targetDuty = constrain(targetDuty, 0, 1023);
      
      // Апаратно виставляємо ШИМ (світло реагує миттєво)
      ledcWrite(i, targetDuty);

      if (currentPwmPct[i] > 0.0f) {
        // Яскравість > 0% -> Негайно вмикаємо реле і скасовуємо таймер розмикання
        digitalWrite(relayPins[i], HIGH);
        relayPendingOff[i] = false;
        
        Serial.printf("⚙️ [HW CH%d] ШИМ Pin %d: %.1f%% (Duty %d) | Relay Pin %d: HIGH\n", 
                      i + 1, pwmPins[i], currentPwmPct[i], targetDuty, relayPins[i]);
      } 
      else {
        // Яскравість = 0% -> ШИМ вимкнено в 0%, засікаємо 10 секунд перед вимкненням реле
        if (digitalRead(relayPins[i]) == HIGH && !relayPendingOff[i]) {
          relayPendingOff[i] = true;
          relayOffTimestamp[i] = now;
          Serial.printf("⏳ [HW CH%d] Світло 0%%. Запуск затримки 10 сек перед вимкненням реле Pin %d...\n", 
                        i + 1, relayPins[i]);
        }
      }
    }

    // 2. Перевірка таймерів затримки вимкнення реле (10 секунд)
    if (relayPendingOff[i] && (now - relayOffTimestamp[i] >= RELAY_OFF_DELAY_MS)) {
      relayPendingOff[i] = false;
      digitalWrite(relayPins[i], LOW); // Розмикаємо реле
      
      Serial.printf("🔌 [RELAY CH%d] 10 сек минуло. Реле Pin %d знеструмлено!\n", i + 1, relayPins[i]);
      publishStatus(i); // Надсилаємо оновлений статус в MQTT (relay: false)
    }
  }

  // Періодична публікація статусу всіх каналів
  if (now - lastStatus >= periodStatusIdleMs) {
    publishAllStatuses();
  }

  // Heartbeat логування
  if (now - lastLog > 5000) {
    lastLog = now;
    Serial.printf("[HEARTBEAT] Channels Active | WiFi: %s | VPN: %s | MQTT: %s\n",
                  (WiFi.status() == WL_CONNECTED) ? "OK" : "DISCONNECTED", 
                  vpnConnected ? "UP" : "DOWN",
                  mqttClient.connected() ? "CONNECTED" : "OFFLINE");
  }

  delay(10);
}