#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// ============ WiFi ============
const char* ssid = "iPhone";
const char* password = "0863415546";

// ============ Telegram ============
const char* botToken = "8310794477:AAE3HQN-9M0frbWDxW9KMmlY8r57vYqtvNg";
const char* chatID  = "8412651038";

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// ============ IR Sensor ============
const int sensorPin = D5;   // OUT ของ IR Sensor
bool objectDetectedBefore = false;

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  client.setInsecure();

  bot.sendMessage(chatID, "✅ ระบบตรวจจับวัตถุเริ่มทำงานแล้ว", "");
}

void loop() {
  int sensorState = digitalRead(sensorPin);
  // IR Sensor ส่วนใหญ่: LOW = พบวัตถุ

  if (sensorState == LOW && !objectDetectedBefore) {
    objectDetectedBefore = true;

    // ส่ง 2 ข้อความ
    bot.sendMessage(chatID, "🚨 พบวัตถุแล้ว! (1)", "");
    delay(500);
    bot.sendMessage(chatID, "🚨 พบวัตถุแล้ว! (2)", "");

    Serial.println("Object detected → Sent 2 messages");
  }

  // เมื่อวัตถุหายไป → พร้อมตรวจจับใหม่
  if (sensorState == HIGH) {
    objectDetectedBefore = false;
  }

  delay(100);
}