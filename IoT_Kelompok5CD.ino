// Ganti aja Blynk sapa yang mau dipake asal di dasboardnya ada V2 pake switch
#define BLYNK_TEMPLATE_ID "TMPL6B8HLZMJz"
#define BLYNK_TEMPLATE_NAME "monitoring sistem"
#define BLYNK_AUTH_TOKEN "DpJmNglVpxreiRnATNxsy74nXM7fOIIo"
#define BLYNK_PRINT Serial

// Jangan pernah ubah urutan include ini ntar errornya panjang
#include <ArduinoJson.h>
#include <CTBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Sesuain ssid wifi sama password yang mau dipake
char ssid[] = "Tester";
char pass[] = "TestMAC11";

// buat id, sesuain id telegram sapa yang mau dipake (caranya cari IDBot trs tulis /getid)
// buat tokennya pake botku cari aja WarningNotif atau buka link t.me/WarnNotifBot
char token[] = "5934412070:AAHu1ogCkIWpSKy85IdY_qSdAhRo0IHsgZk";
const int id = 1417082898;

BlynkTimer timer;
CTBot myBot;

int pinBuzzer = D0;
int pinLedRed = D1;
int pinLedYellow = D2;
int pinLedGreen = D3;
bool buzzerSwitch = false;

DHT dht(D5, DHT11);

void setup() {
  Serial.begin(9600);
  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedYellow, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(100L, sendSensor);

  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  if (myBot.testConnection()) {
    Serial.println("Koneksi Berhasil");
  } else {
    Serial.println("Koneksi Gagal");
  }
}

void loop() {
  Blynk.run();
  sendSensor();
}

void sendSensor() {
  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C"));
  delay(1000);

  if (temperature > 30) {
    digitalWrite(pinLedRed, HIGH);
    digitalWrite(pinLedYellow, LOW);
    digitalWrite(pinLedGreen, LOW);
    myBot.sendMessage(id, "PERINGATAN, SUHU " + String(temperature) + " °C, SUHU BISA SANGAT PANAS!!");

    if (buzzerSwitch) {
      digitalWrite(pinBuzzer, HIGH);
      delay(1000);
    } else {
      digitalWrite(pinBuzzer, LOW);
    }
  } else if (temperature <= 30 && temperature >= 29) {
    digitalWrite(pinLedRed, LOW);
    digitalWrite(pinLedYellow, HIGH);
    digitalWrite(pinLedGreen, LOW);
  } else if (temperature < 29) {
    digitalWrite(pinLedRed, LOW);
    digitalWrite(pinLedYellow, LOW);
    digitalWrite(pinLedGreen, HIGH);
    myBot.sendMessage(id, "PERINGATAN, SUHU " + String(temperature) + " °C, SUHU BISA SANGAT DINGIN!!");

    if (buzzerSwitch) {
      digitalWrite(pinBuzzer, HIGH);
      delay(500);
      digitalWrite(pinBuzzer, LOW);
      delay(1000);
    } else {
      digitalWrite(pinBuzzer, LOW);
    }
  } else {
    if (buzzerSwitch) {
      digitalWrite(pinLedRed, HIGH);
      digitalWrite(pinLedYellow, HIGH);
      digitalWrite(pinLedGreen, HIGH);
      digitalWrite(pinBuzzer, HIGH);
      delay(1000);
      digitalWrite(pinLedRed, LOW);
      digitalWrite(pinLedYellow, LOW);
      digitalWrite(pinLedGreen, LOW);
      digitalWrite(pinBuzzer, LOW);
      delay(1000);
      myBot.sendMessage(id, "PERINGATAN, TERJADI KESALAHAN PADA SENSOR SUHU, MOHON SEGERA DIPERBAIKI!!");
      delay(1000);
    }
  }
}

BLYNK_WRITE(V2) {
  int pinData = param.asInt();
  buzzerSwitch = (pinData == 1);
}