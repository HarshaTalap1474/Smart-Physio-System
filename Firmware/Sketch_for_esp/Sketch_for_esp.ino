#include <Wire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// *** SAFETY FIXES FOR WEAK BATTERY ***
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

/* --- 1. CONFIGURATION --- */
#define WIFI_SSID       "HospitalWiFi"
#define WIFI_PASSWORD   "12345678"
#define API_KEY         "AIzaSyBkuDFl_I3gG9byZ_OgF292f-v-bIcGhPI"
#define DATABASE_URL    "flexa-22635-default-rtdb.firebaseio.com"

#define MPU_ADDR        0x68
#define FLEX_PIN        34
#define BUZZER_PIN      19
#define WIFI_LED_PIN    2
#define TENS_PIN        4     // Pin to trigger the TENS unit

// SENSITIVITY SETTINGS
#define FLEX_SENSITIVITY 15   
#define HYSTERESIS       10   
#define TILT_LIMIT       4000
#define MAX_FLEX_TIME    5000 

/* --- 2. GLOBAL VARIABLES --- */
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int16_t AcX, AcY, AcZ;
int baselineFlex = 0;
bool isFlexed = false;
bool signupOK = false;

unsigned long lastUpload = 0;
unsigned long lastSerialPrint = 0;
unsigned long previousBuzzerMillis = 0;
unsigned long flexStartTime = 0; 
bool buzzerState = false;

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
  // *** EMERGENCY FIX 1: DISABLE BROWNOUT DETECTOR ***
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  
  // *** EMERGENCY FIX 2: SLOW CPU TO 80MHz (Saves ~30% power) ***
  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(TENS_PIN, OUTPUT); // Set TENS pin as Output
  digitalWrite(TENS_PIN, LOW); // Ensure shock is OFF at startup
  digitalWrite(WIFI_LED_PIN, LOW);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Calibration
  Serial.println("System Starting... Relax Muscle (3s)");
  long sum = 0;
  for (uint8_t i = 0; i < 100; i++) {
    sum += analogRead(FLEX_PIN);
    delay(10); 
  }
  baselineFlex = sum / 100;
  Serial.print("Base: "); Serial.println(baselineFlex);

  // Quick Beep
  digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW);

  // WiFi Setup with POWER REDUCTION
  WiFi.mode(WIFI_STA); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // *** EMERGENCY FIX 3: LOWER WIFI TRANSMIT POWER ***
  // This reduces the huge current spike that kills your battery.
  WiFi.setTxPower(WIFI_POWER_8_5dBm); 

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFI_LED_PIN, !digitalRead(WIFI_LED_PIN)); 
    Serial.print(".");
    delay(300);
  }
  digitalWrite(WIFI_LED_PIN, HIGH); 
  Serial.println("\nConnected!");

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

  if (currentFlex > 4000) currentFlex = baselineFlex; 

  if (currentFlex < (baselineFlex - FLEX_SENSITIVITY)) {
    if (!isFlexed) {
      isFlexed = true;
      flexStartTime = now;
      
      // *** TENS UNIT TRIGGER (Single Pulse) ***
      digitalWrite(TENS_PIN, HIGH);
      delayMicroseconds(5000); // 5ms Pulse
      digitalWrite(TENS_PIN, LOW);
    }
    
    if (now - flexStartTime > MAX_FLEX_TIME) {
      baselineFlex = currentFlex + 10; 
      isFlexed = false;
    }
  } 
  else if (currentFlex > (baselineFlex - FLEX_SENSITIVITY + HYSTERESIS)) {
    isFlexed = false;
  }

  // Rolling Alarm
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

  // Firebase Upload (SLOWED DOWN to save battery)
  if (Firebase.ready() && signupOK && (now - lastUpload >= 5000)) {
    lastUpload = now;

    FirebaseJson json;
    json.set("MuscleRaw", currentFlex);
    json.set("MuscleStatus", isFlexed ? "Active" : "Relaxed");
    json.set("Posture", (abs(AcX) > TILT_LIMIT) ? "Rolling" : "Safe");
    json.set("TiltVal", AcX);

    if (Firebase.RTDB.updateNode(&fbdo, "/Patient", &json)) {
       Serial.println(">> Cloud Updated");
    }
  }
}