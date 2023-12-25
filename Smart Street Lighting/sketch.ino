#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "ThingSpeak.h"

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 20, 4);

// Define WiFi credentials
char ssid[] = "Wokwi-GUEST";      // Network SSID (name)
char pass[] = "";                 // Network password
WiFiClient client;

// ThingSpeak channel details
unsigned long channelNum = "Channel number";               // Channel number taken from the cloud
const char *API_Key = "Write API Key";        // WriteAPI key of the cloud

// Smart Street lighting
// Pins for PIR sensors
const int pirSensorPin1 = 35;
const int pirSensorPin2 = 32;
const int pirSensorPin3 = 33;
const int pirSensorPin4 = 25;
const int pirSensorPin5 = 26;

// Pin for LDR sensor
const int ldrSensorPin = 34;
const float GAMMA = 0.7;
const float RL10 = 50;

// Pins for LEDs
const int ledPin1 = 2;
const int ledPin2 = 0;
const int ledPin3 = 12;
const int ledPin4 = 16;
const int ledPin5 = 17;
const int ledPin6 = 5;
const int ledPin7 = 18;

// Variables to track motion detection and LED status
bool motion1 = false;
bool motion2 = false;
bool motion3 = false;
bool motion4 = false;
bool motion5 = false;

bool ledStatus1 = false;
bool ledStatus2 = false;
bool ledStatus3 = false;
bool ledStatus4 = false;
bool ledStatus5 = false;
bool ledStatus6 = false;
bool ledStatus7 = false;

void setup() {
  Serial.begin(115200);
  Serial.print("\nSmart Street Lighting System\n");

  // Set pin modes for street lighting
  pinMode(pirSensorPin1, INPUT);
  pinMode(pirSensorPin2, INPUT);
  pinMode(pirSensorPin3, INPUT);
  pinMode(pirSensorPin4, INPUT);
  pinMode(pirSensorPin5, INPUT);
  pinMode(ldrSensorPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  pinMode(ledPin6, OUTPUT);
  pinMode(ledPin7, OUTPUT);

  LCD.init();
  LCD.backlight();
  LCD.setCursor(3, 0);
  LCD.print("SMART LIGHTING");
  LCD.setCursor(7, 1);
  LCD.print("SYSTEM");
  delay(2000);
  LCD.clear();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("\nAttempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(3000);
    }
  }
  Serial.println("\nConnected!");
}

void loop() {
  // Read LDR sensor values
  int ldrStatus = analogRead(ldrSensorPin);
  ldrStatus = map(ldrStatus, 4095, 0, 1024, 0);
  float voltage = ldrStatus / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

  // Read PIR sensor values
  int pirStatus1 = digitalRead(pirSensorPin1);
  int pirStatus2 = digitalRead(pirSensorPin2);
  int pirStatus3 = digitalRead(pirSensorPin3);
  int pirStatus4 = digitalRead(pirSensorPin4);
  int pirStatus5 = digitalRead(pirSensorPin5);

  // Print sensor values
  Serial.print("\nLDR: ");
  Serial.print(lux);
  Serial.print("\nPIR 1: ");
  Serial.print(pirStatus1);
  Serial.print(" | PIR 2: ");
  Serial.print(pirStatus2);
  Serial.print(" | PIR 3: ");
  Serial.print(pirStatus3);
  Serial.print(" | PIR 4: ");
  Serial.print(pirStatus4);
  Serial.print(" | PIR 5: ");
  Serial.print(pirStatus5);
  Serial.print(" ");

  // Update motion detection variables
  if (pirStatus1 == HIGH) {
    motion1 = true;
  } else if (pirStatus2 == HIGH) {
    motion2 = true;
  } else if (pirStatus3 == HIGH) {
    motion3 = true;
  } else if (pirStatus4 == HIGH) {
    motion4 = true;
  } else if (pirStatus5 == HIGH) {
    motion5 = true;
  } else {
    motion1 = false;
    motion2 = false;
    motion3 = false;
    motion4 = false;
    motion5 = false;
  }

  // Control LEDs based on sensor values
  if (lux > 100) {
    LCD.setCursor(6, 0);
    LCD.print("DAYTIME");
    LCD.setCursor(3, 2);
    LCD.print("[ Saving Mode ]");
    LCD.setCursor(0, 3);
    LCD.print("                  ");
    if (!motion1 && !motion2 && !motion3 && !motion4 && !motion5) {
      // Turn off all LEDs if no motion is detected
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(ledPin4, LOW);
      digitalWrite(ledPin5, LOW);
      digitalWrite(ledPin6, LOW);
      digitalWrite(ledPin7, LOW);
      ledStatus1 = false;
      ledStatus2 = false;
      ledStatus3 = false;
      ledStatus4 = false;
      ledStatus5 = false;
      ledStatus6 = false;
      ledStatus7 = false;
    } else if (ledStatus1 || ledStatus2 || ledStatus3 || ledStatus4 || ledStatus5) {
      // Turn off all LEDs if they are on
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(ledPin4, LOW);
      digitalWrite(ledPin5, LOW);
      digitalWrite(ledPin6, LOW);
      digitalWrite(ledPin7, LOW);
      ledStatus1 = false;
      ledStatus2 = false;
      ledStatus3 = false;
      ledStatus4 = false;
      ledStatus5 = false;
      ledStatus6 = false;
      ledStatus7 = false;
    }
  }
   else if (lux <= 100) {
    LCD.setCursor(6, 0);
    LCD.print("  NIGHT      ");
    LCD.setCursor(0, 3);
    LCD.print("Motion: ");
    LCD.print(motion1 ? " 1" : " 0");
    LCD.print(motion2 ? " 2" : " 0");
    LCD.print(motion3 ? " 3" : " 0");
    LCD.print(motion4 ? " 4" : " 0");
    LCD.print(motion5 ? " 5" : " 0");
    LCD.setCursor(0, 2);
    LCD.print("                  ");

    // Control LEDs based on motion detection
    if (motion1) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, HIGH);
      ledStatus1 = true;
      ledStatus2 = true;
      ledStatus3 = true;
    }

    if (motion2) {
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, HIGH);
      digitalWrite(ledPin4, HIGH);
      ledStatus2 = true;
      ledStatus3 = true;
      ledStatus4 = true;
    }

    if (motion3) {
      digitalWrite(ledPin3, HIGH);
      digitalWrite(ledPin4, HIGH);
      digitalWrite(ledPin5, HIGH);
      ledStatus3 = true;
      ledStatus4 = true;
      ledStatus5 = true;
    }

    if (motion4) {
      digitalWrite(ledPin4, HIGH);
      digitalWrite(ledPin5, HIGH);
      digitalWrite(ledPin6, HIGH);
      ledStatus4 = true;
      ledStatus5 = true;
      ledStatus6 = true;
    }

    if (motion5) {
      digitalWrite(ledPin5, HIGH);
      digitalWrite(ledPin6, HIGH);
      digitalWrite(ledPin7, HIGH);
      ledStatus5 = true;
      ledStatus6 = true;
      ledStatus7 = true;
    }

    // Turn off all LEDs if no motion is detected
    if (!motion1 && !motion2 && !motion3 && !motion4 && !motion5) {
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(ledPin4, LOW);
      digitalWrite(ledPin5, LOW);
      digitalWrite(ledPin6, LOW);
      digitalWrite(ledPin7, LOW);
      ledStatus1 = false;
      ledStatus2 = false;
      ledStatus3 = false;
      ledStatus4 = false;
      ledStatus5 = false;
      ledStatus6 = false;
      ledStatus7 = false;
    }
  }

  // Set ThingSpeak fields with sensor values
  ThingSpeak.setField(1, digitalRead(ledPin1));
  ThingSpeak.setField(2, digitalRead(ledPin2));
  ThingSpeak.setField(3, digitalRead(ledPin3));
  ThingSpeak.setField(4, digitalRead(ledPin4));
  ThingSpeak.setField(5, digitalRead(ledPin5));
  ThingSpeak.setField(6, digitalRead(ledPin6));
  ThingSpeak.setField(7, digitalRead(ledPin7));
  ThingSpeak.setField(8, lux);

  // Write to the ThingSpeak channel
  int status = ThingSpeak.writeFields(channelNum, API_Key);
  if (status == 200) {
    Serial.println("\nChannel update successful.");
  }
}
