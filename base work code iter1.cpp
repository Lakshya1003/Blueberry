
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_VL53L0X.h>
#include <ESP32Servo.h>

// ===== USER SETTINGS =====
const bool USE_STA = false; // false = AP mode
const char *AP_SSID = "Ryzen";
const char *AP_PASS = "00000000";

// Pins
const int servoL_pin = 18;
const int servoR_pin = 19;
const int vl_sda = 21;
const int vl_scl = 22;

// Servo calibration
int SERVO_STOP = 90;
int SERVO_FORWARD = 180;
int SERVO_BACKWARD = 0;

// Update interval
const unsigned long DIST_UPDATE_MS = 200;

// ===== GLOBALS =====
WebServer server(80);
Servo servoL, servoR;
Adafruit_VL53L0X lox;

volatile int lastDistance = -1;
bool vl_ok = true;

// ===== SERVO CONTROL =====

void stopServos()
{
  servoL.write(SERVO_STOP);
  servoR.write(SERVO_STOP);
}

void forwardMove()
{
  servoL.write(SERVO_FORWARD);
  servoR.write(SERVO_BACKWARD);
}

void backwardMove()
{
  servoL.write(SERVO_BACKWARD);
  servoR.write(SERVO_FORWARD);
}

void rightSpin()
{
  servoL.write(SERVO_FORWARD);
  servoR.write(SERVO_FORWARD);
}

void leftSpin()
{
  servoL.write(SERVO_BACKWARD);
  servoR.write(SERVO_BACKWARD);
}

// ===== HTML PAGE =====

String htmlPage()
{
  return String(R"====(
<!DOCTYPE html>
<html>
<head>
<title>NYX Control</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { background:#0b0f14; color:#e6eef6; text-align:center; font-family:Arial; }
button { padding:16px; margin:8px; width:180px; border:none; border-radius:8px; font-size:20px; }
.btnF { background:#28a745; color:white; }
.btnB { background:#dc3545; color:white; }
.btnR { background:#17a2b8; color:white; }
.btnL { background:#ffc107; color:black; }
.btnS { background:#6c757d; color:white; }
#dist { font-size:28px; color:#00e5a8; margin-top:12px; }
</style>
<script>
function sendCmd(c){ fetch('/cmd?move='+c); }

function updateDist(){
  fetch('/distance').then(r=>r.text()).then(t=>{
    document.getElementById('dist').innerText = t + " mm";
  });
}

setInterval(updateDist, )====") +
         DIST_UPDATE_MS + String(R"====();
</script>
</head>
<body>

<h1>NYX Robot Dashboard</h1>
<div id="dist">---</div>

<button class="btnF" onclick="sendCmd('forward')">FORWARD</button><br>
<button class="btnB" onclick="sendCmd('backward')">BACKWARD</button><br>
<button class="btnR" onclick="sendCmd('right')">RIGHT SPIN</button><br>
<button class="btnL" onclick="sendCmd('left')">LEFT SPIN</button><br>
<button class="btnS" onclick="sendCmd('stop')">STOP</button>

</body>
</html>
)====");
}

// ===== HTTP HANDLERS =====

void handleRoot()
{
  server.send(200, "text/html", htmlPage());
}

void handleCmd()
{
  if (!server.hasArg("move"))
  {
    server.send(400, "text/plain", "Missing move");
    return;
  }

  String m = server.arg("move");

  if (m == "forward")
    forwardMove();
  else if (m == "backward")
    backwardMove();
  else if (m == "right")
    rightSpin();
  else if (m == "left")
    leftSpin();
  else
    stopServos();

  server.send(200, "text/plain", "OK");
}

void handleDistance()
{
  server.send(200, "text/plain", String(lastDistance));
}

// ===== SETUP =====

void setup()
{
  Serial.begin(115200);
  delay(200);

  // Servo setup
  servoL.setPeriodHertz(50);
  servoR.setPeriodHertz(50);
  servoL.attach(servoL_pin);
  servoR.attach(servoR_pin);
  stopServos();

  // I2C for VL53
  Wire.begin(vl_sda, vl_scl);

  if (!lox.begin())
  {
    Serial.println("VL53 init FAILED!");
    vl_ok = false;
    lastDistance = -1;
  }
  else
  {
    Serial.println("VL53 OK");
    vl_ok = true;
  }

  // WiFi
  if (USE_STA)
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(STA_SSID, STA_PASS);
  }
  else
  {
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.println("AP Mode Started");
    Serial.println(WiFi.softAPIP());
  }

  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.on("/distance", handleDistance);
  server.begin();
}

unsigned long prevTime = 0;

// ===== LOOP =====

void loop()
{
  server.handleClient();

  unsigned long now = millis();
  if (now - prevTime >= DIST_UPDATE_MS)
  {
    prevTime = now;

    if (vl_ok)
    {
      VL53L0X_RangingMeasurementData_t measure;
      lox.rangingTest(&measure, false);
      if (measure.RangeStatus != 4)
        lastDistance = measure.RangeMilliMeter;
      else
        lastDistance = -1;
    }
  }
}
