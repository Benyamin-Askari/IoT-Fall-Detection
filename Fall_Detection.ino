#include <Arduino_LSM6DS3.h>
#include <WiFiNINA.h>
#include "ThingSpeak.h"

// =======================
// WiFi and ThingSpeak Configuration
// =======================
#include "_passwords.h"
WiFiClient client;

// =======================
// Configuration Parameters
// =======================

// Thresholds (adjust these based on calibration)
const float ACC_THRESHOLD = 1.2; // m/s²
const float GYRO_THRESHOLD = 200.0;  // rad/s

// Rate of change thresholds for detecting falls (calibration needed)
const float ACCEL_RATE_THRESHOLD = 0.3; // m/s² per loop iteration
const float GYRO_RATE_THRESHOLD = 1.0;  // rad/s per loop iteration

// Duration thresholds for detecting sustained high acceleration
const unsigned long HIGH_ACC_DURATION = 100; // ms

// Timing
unsigned long lastFallTime = 0;
const unsigned long FALL_COOLDOWN = 5000; // 5 seconds cooldown

// LED Pin (Built-in LED on pin 25)
const int LED_PIN = 25; // Explicitly define the built-in LED pin for Uno WiFi Rev2

// =======================
// Setup Function
// =======================
void setup() {
  // Initialize Serial Communication
  Serial.begin(9600);
  while (!Serial)
    delay(1000); // Wait for Serial port to connect (only necessary for some boards)
  Serial.println("This is a test.");
  
  // Initialize the built-in LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Turn off LED initially

  // Initialize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1); // Halt execution
  }
  Serial.println("IMU initialized successfully.");

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Connected to WiFi!");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

// =======================
// Loop Function
// =======================
void loop() {
  static float lastAccMag = 0.0, lastGyroMag = 0.0;
  static unsigned long highAccStartTime = 0;
  static bool highAccDetected = false;

  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;

  // Read acceleration data if available
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(xAcc, yAcc, zAcc);
  } else {
    xAcc = yAcc = zAcc = 0.0;
  }

  // Read gyroscope data if available
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(xGyro, yGyro, zGyro);
  } else {
    xGyro = yGyro = zGyro = 0.0;
  }

  // Calculate magnitudes of acceleration and gyroscope vectors
  float accMag = sqrt(xAcc * xAcc + yAcc * yAcc + zAcc * zAcc);
  float gyroMag = sqrt(xGyro * xGyro + yGyro * yGyro + zGyro * zGyro);

  // Calculate rate of change in acceleration and gyroscope values
  float accRate = abs(accMag - lastAccMag);
  float gyroRate = abs(gyroMag - lastGyroMag);

  // Debug: Print values to Serial Monitor
  Serial.print("Acc Magnitude: ");
  Serial.print(accMag, 2);
  Serial.print(" m/s², Gyro Magnitude: ");
  Serial.print(gyroMag, 2);
  Serial.println(" rad/s");

  Serial.print("Acc Rate: ");
  Serial.print(accRate, 2);
  Serial.print(" m/s²/s, Gyro Rate: ");
  Serial.print(gyroRate, 2);
  Serial.println(" rad/s");

  // Get the current time in milliseconds
  unsigned long currentTime = millis();

  // Check if the cooldown period has passed to allow new fall detections
  if (currentTime - lastFallTime > FALL_COOLDOWN) {
    // Detect if high acceleration is sustained for a period
    if (accMag > ACC_THRESHOLD) {
      if (!highAccDetected) {
        highAccStartTime = currentTime;
        highAccDetected = true;
      }

      // If acceleration stays high for more than the defined duration, it's likely a fall
      if (currentTime - highAccStartTime > HIGH_ACC_DURATION) {
        // Detect fall based on thresholds
        if (gyroMag > GYRO_THRESHOLD && accRate > ACCEL_RATE_THRESHOLD && gyroRate > GYRO_RATE_THRESHOLD) {
          Serial.println(">>> FALL DETECTED! <<<");
          digitalWrite(LED_PIN, HIGH); // Turn on LED as an alert

          // Send fall notification to ThingSpeak
          int httpCode = ThingSpeak.writeField(thingSpeakChannelId, 1, "Fall Detected", thingSpeakApiKey);
          if (httpCode == 200) {
            Serial.println("Fall notification sent to ThingSpeak successfully.");
          } else {
            Serial.println("Failed to send notification to ThingSpeak.");
          }

          // Update the last fall time to enforce cooldown
          lastFallTime = currentTime;

          // Keep the LED on for a short duration to indicate detection
          delay(5000); // LED stays on for 5 seconds
          digitalWrite(LED_PIN, LOW); // Turn off LED
        }
      }
    } else {
      highAccDetected = false; // Reset detection if acceleration falls below threshold
    }
  }

  // Update the previous magnitudes for the next loop
  lastAccMag = accMag;
  lastGyroMag = gyroMag;

  // Delay between readings to control sampling rate
  delay(5);
}
