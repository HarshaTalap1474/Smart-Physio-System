// Flex Sensor Test
// Pin 34 is an Input Only analog pin on ESP32, perfect for sensors.

const int flexPin = 34; 

void setup() {
  Serial.begin(115200);
  pinMode(flexPin, INPUT);
  Serial.println("Flex Sensor Test Started...");
}

void loop() {
  // Read the raw analog value (0 to 4095 on ESP32)
  int flexValue = analogRead(flexPin);
  
  Serial.print("Raw Sensor Value: ");
  Serial.println(flexValue);
  
  delay(200);
}