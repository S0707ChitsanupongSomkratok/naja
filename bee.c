#include <Servo.h>

Servo servoDry; 
Servo servoWet; 

const int trigPin = 12;
const int echoPin = 11;
const int soilPin = A0;

int threshold = 20; // ค่าความชื้นแบ่งเปียก/แห้ง

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  servoDry.attach(8);
  servoWet.attach(9);
  
  servoDry.write(0); 
  servoWet.write(0);
  
  Serial.println("--- Smart Bin System: Ready ---");
}

void loop() {
  // 1. วัดระยะทางด้วย Ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  // 2. ถ้าเจอขยะในระยะน้อยกว่า 15 ซม.
  if (distance > 0 && distance < 15) {
    Serial.print("Object Detected at: "); Serial.print(distance); Serial.println(" cm");
    
    delay(1000); // ให้เวลาคนวางขยะให้นิ่งบนเซนเซอร์ความชื้น

    // 3. อ่านค่าความชื้น
    int rawValue = analogRead(soilPin);
    int percent = map(rawValue, 1023, 300, 0, 100);
    percent = constrain(percent, 0, 100);

    Serial.print("Moisture Check: "); Serial.print(percent); Serial.println("%");

    // 4. เงื่อนไขแยกขยะ
    if (percent > threshold) {
      Serial.println(">> OPEN WET BIN");
      servoWet.write(90); 
      delay(5000);        
      servoWet.write(0);  
    } 
    else {
      Serial.println(">> OPEN DRY BIN");
      servoDry.write(90); 
      delay(5000);        
      servoDry.write(0);  
    }
    
    Serial.println("Action Finished. Resetting...");
    delay(2000); // รอให้คนดึงมือออกก่อนเริ่มใหม่
  }

  delay(200); 
}