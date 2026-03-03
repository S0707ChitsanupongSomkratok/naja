#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// --- ตั้งค่าอุปกรณ์ ---
Servo myServo;
const int servoPin = 2;   // D4 (GPIO 2)
const int trigPin = 5;    // D1 (GPIO 5)
const int echoPin = 4;    // D2 (GPIO 4)
const int relayPin = 14;  // D5 (GPIO 14)
const int statusLed = 16; // D0 (GPIO 16) ใช้เป็นไฟสถานะ (ถ้ามี)

// --- ตั้งค่า WiFi ---
const char *ssid = "ปลาเน่า";
const char *password = "087087087";

ESP8266WebServer server(80);

// --- ฟังก์ชันส่งหน้าเว็บ ---
void handleRoot() {
  String html = R"=====(
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Fish Feeder Control</title>
        <style>
            body { font-family: sans-serif; text-align: center; margin-top: 50px; background-color: #f4f4f4; }
            button { 
                padding: 20px 40px; font-size: 20px; cursor: pointer; 
                background-color: #2ecc71; color: white; border: none; border-radius: 10px;
                box-shadow: 0 4px #27ae60;
            }
            button:active { box-shadow: 0 0 #27ae60; transform: translateY(4px); }
            .status { margin-top: 20px; color: #7f8c8d; }
        </style>
    </head>
    <body>
        <h1>ระบบให้อาหารปลาอัตโนมัติ</h1>
        <button onclick="feedFish()">🐟 สั่งให้อาหารปลา</button>
        <div class="status" id="msg">พร้อมทำงาน</div>

        <script>
            function feedFish() {
                document.getElementById('msg').innerHTML = "กำลังให้อาหาร...";
                fetch("/onFood")
                    .then(response => {
                        setTimeout(() => {
                            document.getElementById('msg').innerHTML = "ให้อาหารเรียบร้อย!";
                        }, 2000);
                    });
            }
        </script>
    </body>
    </html>
  )=====";
  server.send(200, "text/html", html);
}

// --- ฟังก์ชันทำงานของ Servo ---
void handleFood() {
  server.send(200, "text/plain", "Feeding..."); // ตอบกลับทันทีหน้าเว็บจะได้ไม่หมุนค้าง
  Serial.println("Feeding initiated...");
  
  myServo.write(150);
  delay(1000);
  myServo.write(0);
  delay(500);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(statusLed, OUTPUT);
  
  // สถานะเริ่มต้น
  digitalWrite(relayPin, HIGH); // Relay ปิด (Active Low)
  myServo.attach(servoPin);
  myServo.write(0);

  // ตั้งค่า WiFi Access Point
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // เส้นทางคำสั่ง Web Server
  server.on("/", handleRoot);
  server.on("/onFood", handleFood);
  server.begin();
  
  Serial.println("HTTP server started");
}

unsigned long lastUpdate = 0;
const long interval = 500; // ตรวจสอบระยะน้ำทุกๆ 0.5 วินาที

void loop() {
  server.handleClient();

  // ตรวจสอบระยะทางแบบไม่ใช้ Delay (Non-blocking)
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= interval) {
    lastUpdate = currentMillis;

    // วัดระยะทาง Ultrasonic
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distance = (duration * 0.034) / 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Logic เติมน้ำ (Relay)
    // ถ้าระยะห่างมาก (น้ำลดลง) > 12cm ให้เปิดปั๊ม
    if (distance > 12.0) {
      digitalWrite(relayPin, LOW); 
      Serial.println("Pump: ON");
    } 
    // ถ้าระยะห่างน้อย (น้ำเต็ม) < 8cm ให้ปิดปั๊ม
    else if (distance < 8.0) {
      digitalWrite(relayPin, HIGH);
      Serial.println("Pump: OFF");
    }
  }
}