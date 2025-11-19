#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char *ssid = "Lakshya";
const char *password = "88888888";

WebServer server(80);
Servo myServo;

int servoPin = 13;
int currentAngle = 90;

bool isForward = false;
bool isBackward = false;
bool isStopped = true;

int speedLevel = 3; // 1 to 10

void sendDashboard()
{
  String page = R"(
        <html>
        <head><title>Servo Control</title></head>
        <body style='text-align:center; font-family:Arial;'>

        <h2>MG995 Servo Controller</h2>

        <button onclick="location.href='/forward'"
                style='width:200px; height:50px; font-size:20px;'>Forward</button><br><br>

        <button onclick="location.href='/backward'"
                style='width:200px; height:50px; font-size:20px;'>Backward</button><br><br>

        <button onclick="location.href='/stop'"
                style='width:200px; height:50px; font-size:20px;'>Stop</button><br><br>

        <h3>Speed Control</h3>
        <input type='range' min='1' max='10' value='3' id='spd'
               oninput='sendSpeed(this.value)' style='width:300px;'>
        <p id='val'>Speed: 3</p>

        <script>
        function sendSpeed(v) {
            document.getElementById('val').innerHTML = "Speed: " + v;
            fetch('/setspeed?val=' + v);
        }
        </script>

        </body>
        </html>
    )";
  server.send(200, "text/html", page);
}

void startForward()
{
  isForward = true;
  isBackward = false;
  isStopped = false;
  server.send(200, "text/plain", "Forward");
}

void startBackward()
{
  isForward = false;
  isBackward = true;
  isStopped = false;
  server.send(200, "text/plain", "Backward");
}

void stopServo()
{
  isForward = false;
  isBackward = false;
  isStopped = true;
  server.send(200, "text/plain", "Stopped");
}

void setSpeed()
{
  if (server.hasArg("val"))
  {
    speedLevel = server.arg("val").toInt();
    speedLevel = constrain(speedLevel, 1, 10);
  }
  server.send(200, "text/plain", "Speed updated");
}

void setup()
{
  Serial.begin(115200);

  myServo.attach(servoPin, 500, 2500);
  myServo.write(currentAngle);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("Dashboard: http://");
  Serial.println(WiFi.localIP());

  server.on("/", sendDashboard);
  server.on("/forward", startForward);
  server.on("/backward", startBackward);
  server.on("/stop", stopServo);
  server.on("/setspeed", setSpeed);

  server.begin();
}

void loop()
{
  server.handleClient();

  if (!isStopped)
  {
    int step = speedLevel; // 1-10

    if (isForward && currentAngle < 180)
    {
      currentAngle += step;
    }
    if (isBackward && currentAngle > 0)
    {
      currentAngle -= step;
    }

    currentAngle = constrain(currentAngle, 0, 180);
    myServo.write(currentAngle);

    delay(20);
  }
}
