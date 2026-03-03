#include <ESP8266WiFi.h> // ไลบารี่ wifi
#include <ESP8266WebServer.h> // ไลบารี่ server

/* ================= WIFI AP ================= */
const char* ssid = "โปรเจคจบ 6/1&6/2"; // กำหนดชื่อ wifi ของบอร์ด
const char* password = "12345678"; // กำหนดรหัสผ่าน wifi ของบอร์ด

ESP8266WebServer server(80); // ประกาศ port server

/* ============ MOTOR DRIVER PIN ============ */
int forw = D2;
int back = D5;
int lef = D6;
int rig = D7;

// ใบตัดหญ้า (Relay)
#define CUT_MOTOR D1 // ขา relay ของมอเตอร์

/* ================= STATE ================= */
bool cutState = true; // สถานะของ relay กำหนดตอนเเรกให้เป็นเท็จก่อน

/* ================= WEB PAGE ================= */
// หน้าเว็บสำหรับสั่งการ
String webpage = R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Grass Cutter v1.1</title>

<style>
:root{
  --bg:#0e0e0e;
  --card:#1c1c1c;
  --accent:#00c6ff;
  --danger:#ff5252;
  --success:#4caf50;
  --text:#ffffff;
  --muted:#aaaaaa;
}

body{
  margin:0;
  min-height:100vh;
  background:var(--bg);
  color:var(--text);
  font-family: Tahoma, "Noto Sans Thai", Arial, sans-serif;
  display:flex;
  justify-content:center;
  align-items:center;
}

.container{
  background:var(--card);
  width:320px;
  padding:20px;
  border-radius:20px;
  box-shadow:0 20px 40px rgba(0,0,0,.5);
  text-align:center;
}

h2{ margin:0; }
p{
  margin:6px 0 16px;
  color:var(--muted);
  font-size:14px;
}

/* D-PAD */
.grid{
  display:grid;
  grid-template-columns:repeat(3,1fr);
  gap:10px;
  margin-bottom:14px;
}

button{
  height:60px;
  font-size:18px;
  border:none;
  border-radius:14px;
  background:#2a2a2a;
  color:white;
  cursor:pointer;
  transition:.2s;
}

button.active{
  background:var(--accent);
  color:black;
  font-weight:bold;
}

button.stop{
  background:var(--danger);
  font-size:20px;
}

button.cut{
  width:100%;
  background:#2a2a2a;
}

button.cut.active{
  background:var(--success);
  color:white;
}

.footer{
  margin-top:10px;
  font-size:12px;
  color:var(--muted);
}
</style>
</head>

<body>
<div class="container">
  <h2>🚜 รถตัดหญ้า</h2>
  <p>Grass Cutter v1.1</p>

  <div class="grid">
    <div></div>
    <button id="forward" onclick="toggleMove('forward')">▲</button>
    <div></div>

    <button id="left" onclick="toggleMove('left')">◀</button>
    <button id="stop" class="stop" onclick="stopMove()">■</button>
    <button id="right" onclick="toggleMove('right')">▶</button>

    <div></div>
    <button id="backward" onclick="toggleMove('backward')">▼</button>
    <div></div>
  </div>

  <button id="cut" class="cut" onclick="toggleCut()">เปิดใบตัดหญ้า</button>

  <div class="footer">แตะปุ่มเดิมซ้ำเพื่อหยุด</div>
</div>

<script>
let currentMove = "";
let cutOn = false;   // ⭐ state ใบตัด (สำคัญ)

function toggleMove(action){
  if(currentMove === action){
    fetch('/stop');
    setActiveMove("");
    currentMove = "";
  }else{
    fetch('/' + action);
    setActiveMove(action);
    currentMove = action;
  }
}

function stopMove(){
  fetch('/stop');
  setActiveMove("");
  currentMove = "";
}

/* --- แก้ไขเฉพาะฟังก์ชัน toggleCut ใน JavaScript --- */
function toggleCut(){
  cutOn = !cutOn; // สลับสถานะ (true = เปิด, false = ปิด)
  fetch('/cut');

  const btn = document.getElementById("cut");

  if(cutOn){
    btn.innerText = "ปิดใบตัดหญ้า"; // ข้อความที่จะให้กดเพื่อปิด
    btn.classList.remove("active");  // ใส่สีเขียว
  }else{
    btn.innerText = "เปิดใบตัดหญ้า"; // ข้อความที่จะให้กดเพื่อเปิด
    btn.classList.add("active"); // เอาสีออก
  }
}


function setActiveMove(action){
  ['forward','backward','left','right'].forEach(id=>{
    document.getElementById(id).classList.remove('active');
  });
  if(action){
    document.getElementById(action).classList.add('active');
  }
}
</script>


</body>
</html>
)rawliteral";

/* ================= MOTOR FUNCTION ================= */

// เดินหน้า
void forward(){
  digitalWrite(forw, HIGH);
  digitalWrite(back, HIGH);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, HIGH);

  delay(1000);

  digitalWrite(forw, LOW);
  digitalWrite(back, HIGH);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, HIGH);

}

// ถอยหลัง
void backward(){
 digitalWrite(forw, HIGH);
  digitalWrite(back, HIGH);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, HIGH);

  delay(1000);

  digitalWrite(forw, HIGH);
  digitalWrite(back, LOW);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, HIGH);
}

// เลี้ยวซ้าย
void left(){
  digitalWrite(forw, HIGH);
  digitalWrite(back, HIGH);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, HIGH);

  delay(1000);

  digitalWrite(forw, HIGH);
  digitalWrite(back, HIGH);
  digitalWrite(lef, LOW);
  digitalWrite(rig, HIGH);
}

// เลี้ยวขวา
void right(){
  digitalWrite(forw, HIGH);
  digitalWrite(back, HIGH);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, HIGH);

  delay(1000);

  digitalWrite(forw, HIGH);
  digitalWrite(back, HIGH);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, LOW);
}

// หยุดรถ
void stopCar(){
  digitalWrite(forw, HIGH);
  digitalWrite(back, HIGH);
  digitalWrite(lef, HIGH);
  digitalWrite(rig, HIGH);
}

/* ================= SETUP ================= */
void setup(){
  Serial.begin(9600); // กำหนดหน้าจอเเสดง serial monitor

  // กำหนดขา pin ของทุกตัวให้เป็น output
  pinMode(forw, OUTPUT);
  pinMode(back, OUTPUT);
  pinMode(lef, OUTPUT);
  pinMode(rig, OUTPUT);

  pinMode(CUT_MOTOR, OUTPUT);

  stopCar(); // เริ่มต้นสั่งให้รถหยุดก่อน
  
  digitalWrite(CUT_MOTOR, HIGH); // เริ่มต้นสั่งให้ relay ที่สั่งการใบตัดหญ้าหยุดก่อน

  WiFi.softAP(ssid, password); // สั่งให้บอร์ดเชื่อมต่อกับ wifi
  Serial.println("Grass Cutter v1.1");
  Serial.print("IP : ");
  Serial.println(WiFi.softAPIP()); // 192.168.4.1 เลขไอพีของเครื่อง      

  server.on("/", [](){
    server.send(200, "text/html", webpage);
  });

  server.on("/forward", [](){
    forward();
    server.send(200, "text/plain", "FORWARD");
  });

  server.on("/backward", [](){
    backward();
    server.send(200, "text/plain", "BACKWARD");
  });

  server.on("/left", [](){
    left();
    server.send(200, "text/plain", "LEFT");
  });

  server.on("/right", [](){
    right();
    server.send(200, "text/plain", "RIGHT");
  });

  server.on("/stop", [](){
    stopCar();
    server.send(200, "text/plain", "STOP");
  });

  server.on("/cut", [](){
    cutState = !cutState;
    digitalWrite(CUT_MOTOR, cutState ? HIGH : LOW);
    server.send(200, "text/plain", cutState ? "CUT ON" : "CUT OFF");
  });

  server.begin();
}

/* ================= LOOP ================= */
void loop(){
  server.handleClient(); // ทำงานไปเรื่อยตามคำสั่งหน้าเว็บ
}