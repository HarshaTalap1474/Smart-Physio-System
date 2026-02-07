#include <Wire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

/* --- 1. CONFIGURATION --- */
#define WIFI_SSID       "HospitalWiFi"
#define WIFI_PASSWORD   "12345678"

#define API_KEY         "AIzaSyBkuDFl_I3gG9byZ_OgF292f-v-bIcGhPI"
#define DATABASE_URL    "flexa-22635-default-rtdb.firebaseio.com"

#define MPU_ADDR        0x68
#define FLEX_PIN        34
#define BUZZER_PIN      19
#define WIFI_LED_PIN    2

// SENSITIVITY SETTINGS
#define FLEX_SENSITIVITY 40   // Drop required to trigger Active
#define HYSTERESIS       20   // Buffer to prevent flickering
#define TILT_LIMIT       4000
#define MAX_FLEX_TIME    5000 // 5 Seconds Timeout for "Stuck" sensor

/* --- 2. GLOBAL VARIABLES --- */
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int16_t AcX, AcY, AcZ;
int baselineFlex = 0;
bool isFlexed = false;
bool signupOK = false;

// Timers
unsigned long lastUpload = 0;
unsigned long lastSerialPrint = 0;
unsigned long previousBuzzerMillis = 0;
unsigned long flexStartTime = 0; // To track how long we are stuck
bool buzzerState = false;

/* --- 3. HELPER FUNCTIONS --- */
void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  
  if (Wire.requestFrom(MPU_ADDR, 6, true) == 6) {
    AcX = (Wire.read() << 8) | Wire.read();
    AcY = (Wire.read() << 8) | Wire.read();
    AcZ = (Wire.read() << 8) | Wire.read();
  }
}

int readFlex() {
  long sum = 0;
  for (uint8_t i = 0; i < 20; i++) {
    sum += analogRead(FLEX_PIN);
    delay(1);
  }
  return sum / 20;
}

/* --- 4. SETUP --- */
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(WIFI_LED_PIN, OUTPUT);
  digitalWrite(WIFI_LED_PIN, LOW);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Calibrate Flex Sensor
  Serial.println("System Starting... Relax Muscle for Calibration (3 sec)");
  long sum = 0;
  for (uint8_t i = 0; i < 100; i++) {
    sum += analogRead(FLEX_PIN);
    delay(10); 
  }
  baselineFlex = sum / 100;
  Serial.print("Calibration Done! Baseline: ");
  Serial.println(baselineFlex);

  // Ready Beep
  digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFI_LED_PIN, !digitalRead(WIFI_LED_PIN)); 
    Serial.print(".");
    delay(300);
  }
  digitalWrite(WIFI_LED_PIN, HIGH); 
  Serial.println("\nWiFi Connected!");

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.signer.test_mode = true; 

  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  unsigned long start = millis();
  while (!Firebase.ready() && millis() - start < 5000) { delay(100); }
  if(Firebase.ready()) signupOK = true;
}

/* --- 5. MAIN LOOP --- */
void loop() {
  unsigned long now = millis();

  readMPU();
  int currentFlex = readFlex();

  // *** SAFETY 1: IGNORE SPIKES ***
  // If value is > 4000, it's a loose wire spike. Ignore it.
  if (currentFlex > 4000) {
    currentFlex = baselineFlex; 
  }

  // 2. Logic: Muscle Detection
  if (currentFlex < (baselineFlex - FLEX_SENSITIVITY)) {
    if (!isFlexed) {
      isFlexed = true;
      flexStartTime = now; // Start timer
    }
    
    // *** SAFETY 2: TIMEOUT RESET ***
    // If we have been "Active" for > 5 seconds, assume baseline drifted and RESET IT.
    if (now - flexStartTime > MAX_FLEX_TIME) {
      Serial.println(">> Auto-Resetting Baseline (Stuck Active Fix)");
      baselineFlex = currentFlex + 10; // Reset baseline to current value
      isFlexed = false;
    }

  } 
  else if (currentFlex > (baselineFlex - FLEX_SENSITIVITY + HYSTERESIS)) {
    isFlexed = false;
  }

  // 3. Logic: Rolling Alarm
  if (abs(AcX) > TILT_LIMIT) {
    if (now - previousBuzzerMillis >= 100) {
      previousBuzzerMillis = now;
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  } else {
    buzzerState = false;
    digitalWrite(BUZZER_PIN, LOW);
  }

  // 4. Serial Print (2Hz)
  if (now - lastSerialPrint >= 500) {
    lastSerialPrint = now;
    if (isFlexed) {
      Serial.print("💪 MUSCLE ACTIVE! (Val: ");
      Serial.print(currentFlex);
      Serial.print(" | Base: ");
      Serial.print(baselineFlex);
      Serial.println(")");
    } else if (abs(AcX) > TILT_LIMIT) {
      Serial.print("⚠️ ROLLING! Tilt: ");
      Serial.println(AcX);
    }
  }

  // 5. Firebase Upload (2 Seconds)
  if (Firebase.ready() && signupOK && (now - lastUpload >= 2000)) {
    lastUpload = now;

    FirebaseJson json;
    json.set("MuscleRaw", currentFlex);
    json.set("MuscleStatus", isFlexed ? "Active" : "Relaxed");
    json.set("Posture", (abs(AcX) > TILT_LIMIT) ? "Rolling (Unsafe)" : "Safe");
    json.set("TiltVal", AcX);

    if (Firebase.RTDB.updateNode(&fbdo, "/Patient", &json)) {
       Serial.println(">> Cloud Updated");
    } else {
       Serial.print(">> Upload Failed: ");
       Serial.println(fbdo.errorReason());
    }
  }
}