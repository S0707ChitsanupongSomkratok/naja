#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// ================= WiFi =================
const char* ssid = "OPM_WiFi_2.4GHz";
const char* password = "op154899";

// ================= Telegram =================
const char* botToken = "8373241900:AAHyDioSyMeO5IVPTSKx9JZWaAQLGuHfBHU";
const char* chatID  = "8068973517";

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// ================= ตั้งค่าเวลา =================
unsigned long lastSendTime = 0;
const unsigned long interval = 1000; // ทุก 1 วิ
int sendCount = 0;
const int maxSend = 5;

// ================= Gas Sensor =================
const int gasPin = D5;
bool alertActive = false;
bool alertFinished = false;  // กันไม่ให้เริ่มรอบซ้ำ

// ================= Buzzer =================
const int buzzerPin = D0;

void setup() {
  Serial.begin(115200);

  pinMode(gasPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setInsecure();
  bot.sendMessage(chatID, "✅ ระบบตรวจจับแก๊สเริ่มทำงานแล้ว", "");
}

void loop() {

  unsigned long currentMillis = millis();
  int gasState = digitalRead(gasPin);

  // ปกติ LOW = พบแก๊ส
  bool alertNow = (gasState == LOW);

  // ===== เริ่มรอบเมื่อพบแก๊ส และยังไม่เคยแจ้งรอบนี้ =====
  if (alertNow && !alertActive && !alertFinished) {
    alertActive = true;
    sendCount = 0;
    lastSendTime = currentMillis - interval; // ส่งทันที
  }

  // ===== ส่งครบ 5 ครั้ง =====
  if (alertActive && sendCount < maxSend) {

    if (currentMillis - lastSendTime >= interval) {

      lastSendTime = currentMillis;
      sendCount++;

      bot.sendMessage(chatID,
        "🔥 ตรวจพบแก๊ส! ครั้งที่ " + String(sendCount),
        ""
      );

      // 🔊 เสียง
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(buzzerPin, LOW);

      if (sendCount >= maxSend) {
        alertActive = false;
        alertFinished = true;  // ล็อครอบนี้ไว้
      }
    }
  }

  // ===== รีเซ็ตเมื่อแก๊สหาย =====
  if (!alertNow) {
    alertFinished = false;
  }
}