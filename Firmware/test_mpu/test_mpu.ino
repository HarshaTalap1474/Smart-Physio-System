#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial Monitor

  Serial.println("Testing MPU6050...");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip. Check wiring!");
    Serial.println("SDA should be on GPIO 21");
    Serial.println("SCL should be on GPIO 22");
    while (1) { delay(10); }
  }

  Serial.println("MPU6050 Found!");
  
  // Set to basic default range
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Accel X: "); Serial.print(a.acceleration.x);
  Serial.print(", Y: "); Serial.print(a.acceleration.y);
  Serial.print(", Z: "); Serial.print(a.acceleration.z);
  
  // Calculate simple tilt angle just to see if it makes sense
  float angle = atan2(a.acceleration.x, a.acceleration.z) * 180 / PI; 
  Serial.print(" | TILT ANGLE: "); Serial.println(angle);

  delay(500);
}