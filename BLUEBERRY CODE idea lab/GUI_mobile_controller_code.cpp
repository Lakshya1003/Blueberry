/ ---------- Web GUI HTML ----------
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Rover Control</title>
<style>
  body{font-family:Arial;background:#f7f9fc;color:#222;text-align:center;padding:14px}
  h2{margin-bottom:8px}
  .row{display:flex;justify-content:center;margin:8px}
  button{width:120px;height:70px;font-size:18px;margin:6px;border-radius:10px;border:1px solid #888}
  #status{margin-top:12px;font-size:14px;color:#333}
</style>
</head>
<body>
  <h2>Rover Control</h2>
  <div class="row"><button onclick="sendCmd('f')">Forward</button></div>
  <div class="row">
    <button onclick="sendCmd('l')">Left</button>
    <button onclick="sendCmd('s')">Stop</button>
    <button onclick="sendCmd('r')">Right</button>
  </div>
  <div class="row"><button onclick="sendCmd('b')">Backward</button></div>
  <div id="status">Status: Ready</div>

<script>
function sendCmd(c) {
  fetch('/cmd?c=' + c).then(r=>r.text()).then(t=>{
    document.getElementById('status').innerText = 'Sent: ' + c;
  }).catch(e=>{
    document.getElementById('status').innerText = 'Error sending';
  });
}
</script>
</body>
</html>
)rawliteral";

// ---------- Web handlers ----------
void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.send_P(200, "text/html", index_html);
}

void handleCmd() {
  if (!server.hasArg("c")) { server.send(400, "text/plain", "Missing c"); return; }
  String c = server.arg("c");
  if (c.length() == 0) { server.send(400, "text/plain", "Empty"); return; }
  char ch = c.charAt(0);
  // trigger movement
  switch (ch) {
    case 'f': forwardMotor(); break;
    case 'b': backwardMotor(); break;
    case 'l': turnLeft(); break;
    case 'r': turnRight(); break;
    case 's': stopMotors(); break;
    default: stopMotors(); break;
  }
  server.send(200, "text/plain", String("OK: ") + ch);
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== ESP32 Rover - Basic Control ===");

  // Attach servos
  leftServo.setPeriodHertz(50);
  rightServo.setPeriodHertz(50);
  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);
  stopMotors();

  // Connect to WiFi or start AP
  if (strlen(WIFI_SSID) > 0) {
    Serial.print("Connecting to WiFi: "); Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
      delay(250); Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
    } else {
      Serial.println("\nWiFi failed. Starting AP mode.");
      WiFi.softAP("RoverAP");
      Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
    }
  } else {
    // no credentials -> AP mode
    WiFi.softAP("RoverAP");
    Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
  }

  // Start web server
  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.begin();
  Serial.println("Web server started. Open root page on phone to control.");
}

// ---------- Loop ----------
void loop() {
  server.handleClient();
  // optional serial commands
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n'); s.trim();
    if (s.length() > 0) {
      char ch = s.charAt(0);
      // allow serial testing too
      switch (ch) {
        case 'f': forwardMotor(); break;
        case 'b': backwardMotor(); break;
        case 'l': turnLeft(); break;
        case 'r': turnRight(); break;
        case 's': stopMotors(); break;
      }
    }
  }
}
How to test (quick)
Edit WIFI_SSID and WIFI_PASS in the sketch (or leave blank to use AP mode).

Upload to ESP32. Open Serial Monitor at 115200. Note the printed IP:

If connected to your Wi-Fi: IP: 192.168.x.x

If AP mode: AP IP: 192.168.4.1

On your phone, connect to same Wi-Fi (or to the ESP AP if AP mode). Open browser and go to http://<ip>/ (e.g., http://192.168.4.1/).

Tap Forward/Backward/Left/Right/Stop and check rover movement.

Calibration & quick tips
If the rover creeps on Stop: adjust SERVO_STOP slightly ±10–30 until motors are stationary. Use leftServo.writeMicroseconds(x) in small tests to find perfect stop.

If directions are reversed for a wheel, swap SERVO_FORWARD and SERVO_BACKWARD values for that wheel or invert its wiring. Example: if left wheel goes backward when you press Forward, invert that wheel mapping by changing the code (but easier: reverse servo connector on that wheel if possible).

Keep servos powered from proper regulator, not ESP32 5V pin.

If you want next:

I can give a hold-to-run web GUI (so button held = continuous movement; release = stop), or

a small wiring diagram image showing where to connect servos and power.

Tell me which and I’ll drop it right away.
