# ESP32 6-Channel Smart LED Controller

Firmware for ESP32-S3 to control 6 independent LED channels with 10-bit PWM dimming, automated relay power cutoffs, 433 MHz RF switch support, and MQTT over WireGuard VPN.

## Features
* 6-Channel 10-bit PWM dimming (2 kHz).
* Auto-cutoff relays (10s delay at 0% brightness to kill idle PSU draw).
* Non-blocking 433 MHz EV1527 RF switch decoding.
* WireGuard VPN tunnel for secure MQTT communication.
* Web-based OTA firmware updates.

## Build
```bash
pio run -t upload




.

https://github.com/user-attachments/assets/7ffa85c0-9c1a-498b-a39f-cdd80eb669a4

