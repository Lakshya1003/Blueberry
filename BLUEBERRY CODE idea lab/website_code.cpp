// Heyy letss boom the blueberry ;)



// Before we start compiling the code, we need to install libraries. Most of the libraries can be found in the Arduino IDE directly



#include <Wire.h>                    // I2C bus library (core)
#include <WiFi.h>                    // Wi-Fi support for ESP32 (board package)
#include <WiFiUdp.h>                 // UDP networking over Wi-Fi
#include <ESPmDNS.h>                 // mDNS responder for network name resolution
#include <ArduinoOTA.h>              // Over-the-air programming support
#include <VL53L0X_mod.h>             // Driver wrapper for VL53L0X ToF sensor (third-party)
#include "MPU6050_6Axis_MotionApps612.h" // MPU6050 motion/ DMP helper (third-party)
#include "Cdrv8833.h"                // Driver wrapper for DRV8833 motor driver (third-party)
#include "WheelOdometry.h"           // Custom libray for wheel odometry
#include "DataParser.h"              // Custom libray for reading commands sent over Wi-Fi



/*

Once all the libraries are installed, compile the code to check the installation. If all is ok, be happy;  

*/ 

/* Before uploading the code, we will have to make some changes to the code so that it can connect to your wifi hotspot and PC

Replace the Wi-Fi SSID and Password
Replace remoteIP with your PC’s IP address
Motor rotation direction
Replace with your Wifi SSID and Password; otherwise, the robot will not be able to receive messages from the PC or mobile app

*/

#define WIFI_SSID "ground"         //Replace with your own Wi-Fi name
#define WIFI_PASSWORD "12345678"   //Repalce with your password
#define UDP_PORT 12345


const char* remoteIP = "192.168.1.11"; // Add your PC IP address


// Correct wheel spinning direction

  // Change true or false to change direction of rotation of motors
  RMotor.init(IN1_PIN, IN2_PIN, LCHANNEL, true);
  LMotor.init(IN3_PIN, IN4_PIN, RCHANNEL, false);



//   The magic happens in the void loop() This is where all the little functions come together to make the robot work

void loop() {
  ArduinoOTA.handle();  //Handels OTA updates
  process_imu();        //Reads IMU
  odom();               //Calcualtes Wheel rotation
  //heading_control();
  measure_distance();  //Tof sensor measurements
  data_to_send();      //Sends data to external devide over wifi
  udpReceiveTask();    //Receives data from external device like pc or phone
}

void measure_distance()
{
   uint16_t NewMeasurement;
  //Serial.print(".");
  if(sensor.readRangeNoBlocking(NewMeasurement)){
      //Serial.println(".");
        Serial.println(NewMeasurement);
        globalDistance = NewMeasurement;
}

void odom()
{

   // Update odometry for both wheels
    leftWheel.update();
    rightWheel.update();

    // Get and print odometry data
    float leftOdometry = leftWheel.getOdometry();
    float rightOdometry = rightWheel.getOdometry();

    //Serial.print("Wheel 1 Odometry (radians): ");
    //Serial.print(leftOdometry);
    //Serial.print(",");
    //Serial.println(rightOdometry);

    //String Data = String(leftOdometry)+","+String(rightOdometry);
    globalOdom = String(leftOdometry)+","+String(rightOdometry);
    //transmit_data(Data);

    delay(20); // Small delay to reduce noise
  
}

void transmit_data(String Data)
{       //String Data = "Hello";
        udp.beginPacket(remoteIP, UDP_PORT);
        udp.write((const uint8_t*)Data.c_str(), Data.length());
        udp.endPacket();
}


void process_imu()
{
  if (!dmpReady) return;

  // Read a packet from FIFO
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
   mpu.dmpGetQuaternion(&q, fifoBuffer);

   // Send quaternion data over UDP
   char dataBuffer[100];
   //snprintf(dataBuffer, sizeof(dataBuffer), "quat,%f,%f,%f,%f", q.w, q.x, q.y, q.z);
 
    udp.beginPacket(remoteIP, UDP_PORT);

    float yaw = getYaw(q);
    yaw = adjustYaw(yaw);
    
    globalYaw = yaw;//q.z;
    //Serial.println(q.z);
  }
}


