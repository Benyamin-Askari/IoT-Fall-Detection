# IoT-Fall-Detection
This is one of the assignments for the IoT principles module

# Fall Detection System using Arduino, IMU, WiFi, and ThingSpeak

## Overview

This Arduino sketch implements a fall detection system using an LSM6DS3 IMU sensor and reports detected falls to the ThingSpeak IoT platform via WiFi. The system monitors acceleration and gyroscope data, applying thresholds and rate of change analysis to identify potential fall events. Upon detection, it sends a notification to a designated ThingSpeak channel and activates an onboard LED as a local alert. The code is designed to be used with an Arduino board that has WiFi capabilities (like the Uno WiFi Rev2) and requires a separate header file (`_passwords.h`) to store sensitive WiFi and ThingSpeak credentials.

## Table of Contents

1.  **Hardware Requirements**
2.  **Software Requirements**
3.  **Configuration**
    * 3.1  Credentials (`_passwords.h`)
    * 3.2  Thresholds
    * 3.3  Timing
    * 3.4  LED Pin
    * 3.5  ThingSpeak Channel
4.  **Installation and Setup**
    * 4.1  Arduino IDE and Libraries
    * 4.2  `_passwords.h` File
    * 4.3  ThingSpeak Setup
5.  **Functionality**
    * 5.1  Initialization
    * 5.2  Data Acquisition
    * 5.3  Fall Detection Logic
    * 5.4  Alerting
    * 5.5  ThingSpeak Reporting
    * 5.6  Cooldown Mechanism
6.  **Usage**
7.  **Calibration**
8.  **Potential Improvements**

## Hardware Requirements

* Arduino board with WiFi capabilities (e.g., Arduino Uno WiFi Rev2)
* LSM6DS3 IMU sensor (integrated on some Arduino boards or as a separate module)
* (Optional) External power supply for the Arduino

## Software Requirements

* Arduino IDE
* Arduino_LSM6DS3 library
* WiFiNINA library (for Uno WiFi Rev2 or similar)
* ThingSpeak library

## Configuration

### 3.1 Credentials (`_passwords.h`)

You need to create a separate header file named `_passwords.h` in the same directory as your main sketch to store your sensitive information:

```cpp
// _passwords.h

#ifndef _PASSWORDS_H
#define _PASSWORDS_H

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const unsigned long thingSpeakChannelId = YOUR_THINGSPEAK_CHANNEL_ID;
const char* thingSpeakApiKey = "YOUR_THINGSPEAK_WRITE_API_KEY"; // For sending data

#endif

#Summary
3.2 Thresholds
The following constants define the thresholds for fall detection. These values may need adjustment based on calibration for your specific application and sensor orientation:

ACC_THRESHOLD: Minimum acceleration magnitude (in m/s²) to consider a potential fall event.
GYRO_THRESHOLD: Minimum gyroscope magnitude (in rad/s) to consider a potential fall event.
ACCEL_RATE_THRESHOLD: Minimum rate of change of acceleration magnitude (in m/s² per loop iteration) to indicate a rapid change associated with a fall.
GYRO_RATE_THRESHOLD: Minimum rate of change of gyroscope magnitude (in rad/s per loop iteration) to indicate a rapid rotation during a fall.
HIGH_ACC_DURATION: Minimum duration (in milliseconds) that the acceleration must stay above ACC_THRESHOLD to trigger the more detailed fall detection logic.


3.3 Timing
FALL_COOLDOWN: The minimum time (in milliseconds) that must pass after a fall detection before another fall can be detected. This prevents repeated triggers for a single event.

3.4 LED Pin
LED_PIN: Defines the Arduino pin connected to an indicator LED. The code is currently set to 25, which is the built-in LED pin on the Arduino Uno WiFi Rev2. Adjust this if you are using a different board or LED connection.

3.5 ThingSpeak Channel
thingSpeakChannelId: Your specific ThingSpeak channel ID where fall notifications will be sent.
thingSpeakApiKey: Your ThingSpeak Write API Key for the specified channel.
Installation and Setup

4.1 Arduino IDE and Libraries
Ensure you have the Arduino IDE installed.
Install the necessary libraries through the Arduino Library Manager:
Arduino_LSM6DS3 (for the IMU sensor)
WiFiNINA (for WiFi connectivity on boards like Uno WiFi Rev2)
ThingSpeak (for interacting with the ThingSpeak platform)

4.2 _passwords.h File
Create a new tab in your Arduino sketch in the Arduino IDE Cloud.
Name the tab exactly _passwords.h (note the leading underscore).
Copy and paste the content from the Configuration - Credentials section into this tab, replacing the placeholder values with your actual credentials.

4.3 ThingSpeak Setup
Create an account on ThingSpeak.com.
Create a new channel.
Note down your Channel ID.
Go to the "API Keys" tab of your channel and note down your Write API Key.
Functionality

5.1 Initialization
Initializes serial communication for debugging.
Configures the LED_PIN as an output and turns the LED off.
Initializes the LSM6DS3 IMU sensor.
Connects to the configured WiFi network using the credentials from _passwords.h.
Initializes the ThingSpeak client.

5.2 Data Acquisition
In the loop() function, the script continuously reads acceleration and gyroscope data from the IMU.
It calculates the magnitude of the acceleration and gyroscope vectors.
It also calculates the rate of change of these magnitudes.

5.3 Fall Detection Logic
The fall detection logic involves the following steps:

High Acceleration Detection: If the acceleration magnitude exceeds ACC_THRESHOLD, a potential fall event is being monitored.
Sustained High Acceleration: If the high acceleration persists for longer than HIGH_ACC_DURATION, the script proceeds to check other fall indicators.
Threshold Checks: Within the sustained high acceleration period, the script checks if the gyroscope magnitude, acceleration rate of change, and gyroscope rate of change all exceed their respective thresholds (GYRO_THRESHOLD, ACCEL_RATE_THRESHOLD, GYRO_RATE_THRESHOLD).
Fall Confirmation: If all these conditions are met, a fall is detected.

5.4 Alerting
Upon fall detection, the onboard LED connected to LED_PIN is turned on for 5 seconds as a local alert.
A ">>> FALL DETECTED! &lt;&lt;&lt;" message is printed to the serial monitor.

5.5 ThingSpeak Reporting
When a fall is detected, the script attempts to send a notification to your ThingSpeak channel, writing the value "Fall Detected" to Field 1.
It prints a success or failure message to the serial monitor based on the ThingSpeak update response.

5.6 Cooldown Mechanism
After a fall is detected and reported, a FALL_COOLDOWN period is enforced. During this time, new fall detections are ignored to prevent false positives from the immediate aftermath of a fall.
