# ESP32 6-Channel Smart LED Controller

Firmware for ESP32-S3 designed to control 6 LED channels by directly regulating modified power supply units (adjusting PSU voltage output via feedback circuit rather than using traditional downstream dimmers). Features 10-bit PWM modulation, automated AC mains cutoffs, 433 MHz RF switch support, and MQTT over WireGuard VPN.

https://github.com/user-attachments/assets/de26b639-2acc-4bf8-9b2e-c137fb51c95f

## Features
* **Direct PSU Output Modulation:** Adjusts output voltage directly through modified PSU feedback circuits instead of standard PWM dimmers on the LED strips.
* **6-Channel 10-bit PWM:** Hardware-driven 2 kHz PWM for smooth voltage curve control without coil whine.
* **Zero-Standby Mains Cutoff:** Relays physically disconnect AC mains power to idle PSUs (10s delay at 0% brightness) to prevent phantom power draw.
* **433 MHz RF Decoding:** Non-blocking EV1527 wireless wall switch decoder with brightness presets (25%, 50%, 75%, 100%, OFF).
* **Encrypted MQTT:** Full telemetry and remote control routed through a WireGuard VPN tunnel.
* **Web OTA:** In-browser firmware updates over the local/VPN network interface.


https://github.com/user-attachments/assets/de26b639-2acc-4bf8-9b2e-c137fb51c95f

