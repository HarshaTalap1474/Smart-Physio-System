#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

const int buzzerPin = 18; 
unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Explicitly define SDA, SCL pins
  
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Initializing MPU6050...");
  
  byte status = mpu.begin();
  Serial.print("MPU6050 Status: "); Serial.println(status);
  
  // Status 0 means success
  if(status != 0){
    Serial.println("Could not connect to MPU6050! Check wiring.");
    while(1); // Stop if failed
  }
  
  Serial.println("Calculating offsets, DO NOT MOVE MPU6050...");
  delay(1000);
  mpu.calcOffsets(); // Auto-calibrate
  Serial.println("Done! Setup complete.");
}

void loop() {
  mpu.update(); // This does the math for you

  // Get stable angle (Pitch is usually the forward/backward tilt)
  float tiltAngle = mpu.getAngleX(); 

  // Print every 200ms
  if((millis() - timer) > 200){ 
    Serial.print("Tilt Angle: ");x
    Serial.println(tiltAngle);
    
    // Simple Alert Logic Test
    if (abs(tiltAngle) > 25) { // If tilted more than 25 degrees
      digitalWrite(buzzerPin, HIGH);
      Serial.println(" -> BAD POSTURE!");
    } else {
      digitalWrite(buzzerPin, LOW);
    }
    
    timer = millis();
  }
}