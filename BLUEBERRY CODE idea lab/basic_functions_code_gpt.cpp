/*
  ESP32 Basic Rover - Movement + Phone Web GUI
  - Controls 2 continuous-rotation servos (differential drive)
  - Web GUI served from ESP32 (open IP on phone browser)
  - Minimal, clean, ready to paste

  Wiring:
    LEFT_SERVO_PIN  -> GPIO 18
    RIGHT_SERVO_PIN -> GPIO 19
    Servos powered from external 5V regulator (UBEC) if needed
    Common ground between battery, servos, and ESP32
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ---------- CONFIG ----------
const char* WIFI_SSID = "YOUR_SSID";    // set your WiFi (or leave empty for AP mode)
const char* WIFI_PASS = "YOUR_PASS";

const int LEFT_SERVO_PIN  = 18; // left wheel servo signal pin
const int RIGHT_SERVO_PIN = 19; // right wheel servo signal pin

// Typical servo pulses (microseconds). Calibrate SERVO_STOP for your servos.
int SERVO_STOP     = 1500;
int SERVO_FORWARD  = 1700;
int SERVO_BACKWARD = 1300;

// ---------- Objects ----------
Servo leftServo, rightServo;
WebServer server(80);

// ---------- Utility ----------
int mapSpeedToPulse(int speed, int dir) {
  // speed: 0..255, dir: +1 forward, -1 backward
  speed = constrain(speed, 0, 255);
  if (dir >= 0) return map(speed, 0, 255, SERVO_STOP, SERVO_FORWARD);
  else           return map(speed, 0, 255, SERVO_STOP, SERVO_BACKWARD);
}

// ---------- Movement functions ----------
void stopMotors() {
  leftServo.writeMicroseconds(SERVO_STOP);
  rightServo.writeMicroseconds(SERVO_STOP);
}

void forwardMotor(int speed = 200) {
  int pL = mapSpeedToPulse(speed, +1);
  int pR = mapSpeedToPulse(speed, +1);
  leftServo.writeMicroseconds(pL);
  rightServo.writeMicroseconds(pR);
}

void backwardMotor(int speed = 200) {
  int pL = mapSpeedToPulse(speed, -1);
  int pR = mapSpeedToPulse(speed, -1);
  leftServo.writeMicroseconds(pL);
  rightServo.writeMicroseconds(pR);
}

void turnLeft(int speed = 200) {
  leftServo.writeMicroseconds(mapSpeedToPulse(speed, -1));
  rightServo.writeMicroseconds(mapSpeedToPulse(speed, +1));
}

void turnRight(int speed = 200) {
  leftServo.writeMicroseconds(mapSpeedToPulse(speed, +1));
  rightServo.writeMicroseconds(mapSpeedToPulse(speed, -1));
}