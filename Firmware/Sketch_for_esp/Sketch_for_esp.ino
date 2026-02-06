#include <Wire.h>

const int MPU_ADDR = 0x68; 

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL
  
  // 1. Wake up the MPU (It sleeps by default)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Wake up
  Wire.endTransmission(true);
  
  Serial.println("RAW SENSOR DATA (HW CHECK)");
  Serial.println("If these values are STABLE (not jumping wildly), your sensor is GOOD.");
  Serial.println("AcX \t AcY \t AcZ");
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // Start reading at Accelerometer High Byte
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true); // Request 6 bytes (Accel X, Y, Z)
  
  // Read raw data (These are large numbers, roughly -17000 to +17000)
  int16_t AcX = Wire.read()<<8 | Wire.read();      
  int16_t AcY = Wire.read()<<8 | Wire.read();  
  int16_t AcZ = Wire.read()<<8 | Wire.read();  
  
  Serial.print(AcX);
  Serial.print(" \t ");
  Serial.print(AcY);
  Serial.print(" \t ");
  Serial.println(AcZ);
  
  delay(3000);
}