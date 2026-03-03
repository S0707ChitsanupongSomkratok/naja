#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// -------- WiFi --------
const char* ssid = "See";
const char* password = "123456789";

// -------- Telegram --------
const char* botToken = "8536370930:AAHKviTesioFyEieyIKv3GL06CsTBitRtDI";
const char* chatID  = "8352602258";

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// -------- Pin --------
const int tiltPin   = D6;   // SW-520D
const int buzzerPin = D0;   // Buzzer
const int trigPin   = D5;   // HC-SR04 Trig
const int echoPin   = D7;   // HC-SR04 Echo

// -------- Variable --------
unsigned long lastSendTime = 0;

// -------- Function วัดระยะ --------
long readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  long distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  Serial.begin(115200);

  pinMode(tiltPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(buzzerPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  client.setInsecure();
  bot.sendMessage(chatID, "ESP8266 เริ่มทำงานแล้ว 🚀", "");
}

void loop() {
  int tiltState = digitalRead(tiltPin);
  long distance = readDistanceCM();

  bool tiltFall = (tiltState == HIGH);
  bool objectDetected = (distance > 0 && distance <= 50);

  // ===== กรณีเอียง / ล้ม → แจ้ง Telegram =====
  if (tiltFall) {
    if (millis() - lastSendTime >= 1000) {
      lastSendTime = millis();

      digitalWrite(buzzerPin, HIGH);

      String msg = "🚨 ตรวจพบการล้ม กรุณาตรวจสอบด่วน!";
      bot.sendMessage(chatID, msg, "");
      Serial.println(msg);

      delay(200);
      digitalWrite(buzzerPin, LOW);
    }
  }

  // ===== กรณี HC-SR04 เจอวัตถุ → แค่ส่งเสียง =====
  else if (objectDetected) {
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
  }

  // ===== ปกติ =====
  else {
    lastSendTime = 0;
    digitalWrite(buzzerPin, LOW);
  }
}