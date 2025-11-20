#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_VL53L0X.h>
#include <ESP32Servo.h>

// WiFi / UDP
const char *ssid = "tester";
const char *pass = "12345678";
const char *host = "192.168.4.2";
const int port = 12345;

// Servo pin
const int s_pin = 19;

// --- SERVO SPEED CALIBRATION ---
const int s_stop = 90;
const int cw_speed = 100;
const int ccw_speed = 80;

// Active brake settings
const int brake_speed_offset = 6;
const unsigned long brake_ms = 60;

// Sweep timing
const unsigned long sweep_360_ms = 5000; // 360° = 2×180° (tweak if needed)
const unsigned long settle_after_stop_ms = 300;

WiFiUDP udp;
Adafruit_VL53L0X lox;
Servo myServo;

void sendUDPAngDist(float a, int d)
{
  char buf[48];
  int n = snprintf(buf, sizeof(buf), "%.1f,%d\n", a, d);
  udp.beginPacket(host, port);
  udp.write((uint8_t *)buf, n);
  udp.endPacket();
}

int getDist()
{
  VL53L0X_RangingMeasurementData_t m;
  lox.rangingTest(&m, false);
  if (m.RangeStatus != 4)
    return m.RangeMilliMeter;
  return -1;
}

void activeBrake(bool wasCW)
{
  if (wasCW)
    myServo.write(s_stop - brake_speed_offset);
  else
    myServo.write(s_stop + brake_speed_offset);

  delay(brake_ms);
  myServo.write(s_stop);
  delay(20);
}

// --- NEW FUNCTION: 360° SWEEP ---
void runSweep360(bool cw)
{
  unsigned long start = millis();
  unsigned long duration = sweep_360_ms;
  unsigned long endTime = start + duration;

  myServo.write(cw ? cw_speed : ccw_speed);

  while (millis() < endTime)
  {
    float t = float(millis() - start) / float(duration);
    if (t < 0)
      t = 0;
    if (t > 1)
      t = 1;

    float ang = cw ? (t * 360.0f) : (360.0f - t * 360.0f);

    int d = getDist();
    sendUDPAngDist(ang, d);

    delay(15);
  }

  myServo.write(s_stop);
  delay(20);
  activeBrake(cw);
  delay(settle_after_stop_ms);
}

void setup()
{
  Serial.begin(115200);

  myServo.setPeriodHertz(50);
  myServo.attach(s_pin);
  myServo.write(s_stop);

  Wire.begin(21, 22);
  if (!lox.begin())
    Serial.println("VL53 FAIL");
  else
    Serial.println("VL53 OK");

  WiFi.softAP(ssid, pass);
  udp.begin(WiFi.localIP(), 0);

  delay(500);
}

void loop()
{
  Serial.println("360° CW Sweep");
  runSweep360(true);

  delay(1000);

  Serial.println("360° CCW Sweep");
  runSweep360(false);

  delay(1000);
}
