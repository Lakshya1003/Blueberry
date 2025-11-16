/*
  ESP32 Rover (refined) - 2x continuous servos + VL53L0X
  - Basic commands: forward/back/left/right/stop
  - Simple scanning mapping: rotate in place, collect distance readings, print angle,dist(mm)
  - Uses ESP32Servo and Adafruit_VL53L0X libraries

  Reference: adapted from user's project file (blueberry text file.txt). :contentReference[oaicite:1]{index=1}
*/

#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_VL53L0X.h>

// ---------- PIN CONFIG -------------
const int LEFT_SERVO_PIN  = 18; // signal pin for left servo
const int RIGHT_SERVO_PIN = 19; // signal pin for right servo
// I2C default for ESP32 (used by VL53L0X)
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// ---------- SERVO PULSE (microseconds) ----------
// Typical starting values for many continuous servos.
// You MUST calibrate SERVO_STOP for your particular servos (see calibrateServos()).
int SERVO_STOP     = 1500; // neutral pulse (stop)
int SERVO_FORWARD  = 1700; // forward pulse
int SERVO_BACKWARD = 1300; // backward pulse

// ---------- Objects ----------
Servo leftServo;
Servo rightServo;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// ---------- Serial / timing ----------
const long SERIAL_BAUD = 115200;
unsigned long lastStatusTime = 0;
const unsigned long STATUS_INTERVAL_MS = 300;

// ---------- Utility ----------
int mapSpeedToPulse(int speed, int dir) {
  // speed: 0..255 ; dir: +1 forward, -1 backward
  speed = constrain(speed, 0, 255);
  if (dir >= 0) {
    // map 0..255 -> SERVO_STOP .. SERVO_FORWARD
    return map(speed, 0, 255, SERVO_STOP, SERVO_FORWARD);
  } else {
    // map 0..255 -> SERVO_STOP .. SERVO_BACKWARD
    return map(speed, 0, 255, SERVO_STOP, SERVO_BACKWARD);
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("\n=== ESP32 Rover (refined) booting ===");

  // Attach servos and set servo PWM freq
  leftServo.setPeriodHertz(50);  // standard 50Hz
  rightServo.setPeriodHertz(50);
  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);
  stopMotors();

  // init I2C for VL53L0X
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!lox.begin()) {
    Serial.println("VL53L0X not found! Check wiring and power.");
    while (1) { delay(500); } // halt if sensor missing
  }
  Serial.println("VL53L0X initialized.");

  // OPTIONAL: Uncomment to run servo calibration on first boot
  // calibrateServos();
  Serial.println("Ready. Type commands in Serial Monitor.");
  printHelp();
}

// ---------- Loop ----------
void loop() {
  // Periodic status printing
  if (millis() - lastStatusTime > STATUS_INTERVAL_MS) {
    lastStatusTime = millis();
    uint16_t d = readFrontDistance();
    Serial.print("DIST(mm): ");
    Serial.println(d);
  }

  // Handle serial commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();
    handleSerialCommand(cmd);
  }
  delay(10);
}

// ---------- Commands & movement ----------
void forward(int speed = 200) {
  int pL = mapSpeedToPulse(speed, +1);
  int pR = mapSpeedToPulse(speed, +1);
  // If your servos are mounted mirrored, you may need to invert one side:
  // swap pR mapping or invert direction.
  leftServo.writeMicroseconds(pL);
  rightServo.writeMicroseconds(pR);
}

void backward(int speed = 200) {
  int pL = mapSpeedToPulse(speed, -1);
  int pR = mapSpeedToPulse(speed, -1);
  leftServo.writeMicroseconds(pL);
  rightServo.writeMicroseconds(pR);
}

void turnLeft(int speed = 200) {
  // left backward, right forward -> rotate left
  leftServo.writeMicroseconds(mapSpeedToPulse(speed, -1));
  rightServo.writeMicroseconds(mapSpeedToPulse(speed, +1));
}

void turnRight(int speed = 200) {
  // left forward, right backward -> rotate right
  leftServo.writeMicroseconds(mapSpeedToPulse(speed, +1));
  rightServo.writeMicroseconds(mapSpeedToPulse(speed, -1));
}

void stopMotors() {
  leftServo.writeMicroseconds(SERVO_STOP);
  rightServo.writeMicroseconds(SERVO_STOP);
}

// ---------- VL53L0X helpers ----------
uint16_t readFrontDistance() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) {
    return measure.RangeMilliMeter;
  } else {
    return 8190; // out of range sentinel
  }
}

// ---------- Simple time-based scanning map ----------
/*
  simpleScanMap(steps, msPerStep, rotateDir)
  - steps: number of samples around ~360 degrees
  - msPerStep: how long to rotate between samples (tune for your speed)
  - rotateDir: +1 rotate right, -1 rotate left
  Output: prints lines "angle,dist" where angle is (i * 360 / steps)
*/
void simpleScanMap(int steps = 36, int msPerStep = 200, int rotateDir = +1) {
  if (steps <= 0) steps = 36;
  if (msPerStep < 50) msPerStep = 50;
  Serial.printf("SCAN start: steps=%d, msStep=%d\n", steps, msPerStep);
  // small safety: ensure stopped
  stopMotors();
  delay(100);

  int readings[400];
  int validCount = min(steps, 400);

  for (int i = 0; i < validCount; i++) {
    // rotate a small amount
    if (rotateDir > 0)
      turnRight(120); // moderate turning speed; tune as needed
    else
      turnLeft(120);
    delay(msPerStep);
    stopMotors();
    delay(30); // settle

    uint16_t d = readFrontDistance();
    readings[i] = d;

    float angle = (360.0 * i) / (float)steps;
    // Print CSV: angle (deg), distance (mm)
    Serial.printf("%.1f,%u\n", angle, d);
    delay(30);
  }
  stopMotors();
  Serial.println("SCAN complete.");
}

// ---------- Serial command parser ----------
void handleSerialCommand(const String &cmd) {
  if (cmd.length() == 0) return;
  // single-letter basic commands
  if (cmd == "f") { forward(); return; }
  if (cmd == "b") { backward(); return; }
  if (cmd == "l") { turnLeft(); return; }
  if (cmd == "r") { turnRight(); return; }
  if (cmd == "s") { stopMotors(); return; }

  // scan command: "scan 36 200" -> steps msPerStep
  if (cmd.startsWith("scan")) {
    int steps = 36;
    int msStep = 200;
    // parse
    int firstSpace = cmd.indexOf(' ');
    if (firstSpace > 0) {
      String rest = cmd.substring(firstSpace + 1);
      rest.trim();
      int sp = rest.indexOf(' ');
      if (sp > 0) {
        steps = rest.substring(0, sp).toInt();
        msStep = rest.substring(sp + 1).toInt();
      } else {
        steps = rest.toInt();
      }
    }
    simpleScanMap(steps, msStep);
    return;
  }

  // help
  if (cmd == "help" || cmd == "?") {
    printHelp();
    return;
  }

  // speed control: "speed 180"
  if (cmd.startsWith("speed")) {
    // not implemented dynamic in this sketch; it's a placeholder
    Serial.println("Use single-letter commands: f,b,l,r,s  or scan [steps] [msPerStep]");
    return;
  }

  Serial.println("Unknown command. Type 'help' for options.");
}

void printHelp() {
  Serial.println("Commands:");
  Serial.println("  f        -> forward");
  Serial.println("  b        -> backward");
  Serial.println("  l        -> turn left");
  Serial.println("  r        -> turn right");
  Serial.println("  s        -> stop");
  Serial.println("  scan N M -> scan N steps, M ms per step (prints angle,dist)");
  Serial.println("  help     -> show this");
}

// ---------- Servo calibration helper ----------
/*
  Run calibrateServos() once to find correct SERVO_STOP value for your servos.
  Steps:
   - Uncomment call to calibrateServos() in setup() temporarily.
   - Open Serial Monitor and type: stop / f / b / exit to test pulses.
   - When you find a value that fully stops the servo, edit SERVO_STOP and re-upload.
*/
void calibrateServos() {
  Serial.println("Servo calibration mode. Commands: stop, f, b, exit");
  while (true) {
    if (Serial.available()) {
      String c = Serial.readStringUntil('\n');
      c.trim();
      if (c == "stop") {
        leftServo.writeMicroseconds(SERVO_STOP);
        rightServo.writeMicroseconds(SERVO_STOP);
        Serial.printf("Sent stop pulse %d\n", SERVO_STOP);
      } else if (c == "f") {
        leftServo.writeMicroseconds(SERVO_FORWARD);
        rightServo.writeMicroseconds(SERVO_FORWARD);
        Serial.println("Forward pulse.");
      } else if (c == "b") {
        leftServo.writeMicroseconds(SERVO_BACKWARD);
        rightServo.writeMicroseconds(SERVO_BACKWARD);
        Serial.println("Backward pulse.");
      } else if (c == "exit") {
        stopMotors();
        break;
      } else {
        Serial.println("stop / f / b / exit");
      }
    }
    delay(10);
  }
}
