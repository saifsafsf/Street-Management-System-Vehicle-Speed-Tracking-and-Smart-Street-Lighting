#include <WiFi.h>
#include "ThingSpeak.h"
#include <LiquidCrystal_I2C.h>

// Define WiFi credentials
char ssid[] = "Wokwi-GUEST";      // Network SSID (name)
char pass[] = "";                 // Network password
WiFiClient client;

// ThingSpeak channel details
unsigned long channelNum = {{ CHANNEL_NUM }};               // Channel number taken from the cloud
const char *API_Key = "{{ API_KEY }}";        // WriteAPI key of the cloud

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
const int ledPin3 = 4;
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

// Speed Detection
LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 20, 4);

int timer1;
int timer2;

float elapsedTime;

int flagStart = 0;
int flagEnd = 0;

float distance = 5.0;
float vehicleSpeed;

int sensorPin1 = 14;
int sensorPin2 = 27;

int buzzerPin = 19;

void setup() {
  Serial.begin(115200);
  Serial.print("\nSmart Streets System\n");

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

  // Set pin modes for speed detector
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(buzzerPin, OUTPUT);
  ledcSetup(0, 1000, 8);  // Channel 0, 1 kHz, 8-bit resolution
  ledcAttachPin(buzzerPin, 0);  // Attach buzzer to channel 0

  LCD.init();
  LCD.backlight();
  LCD.setCursor(3, 0);
  LCD.print("SMART STREETS");
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
  if (lux > 300) {
    LCD.setCursor(0, 0);
    LCD.print("DAYTIME");
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
   else if (lux <= 300) {
    LCD.setCursor(0, 0);
    LCD.print("NIGHT   ");
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
  ThingSpeak.setField(2, digitalRead(ledPin1));
  ThingSpeak.setField(3, digitalRead(ledPin2));
  ThingSpeak.setField(4, digitalRead(ledPin3));
  ThingSpeak.setField(5, digitalRead(ledPin4));
  ThingSpeak.setField(6, digitalRead(ledPin5));
  ThingSpeak.setField(7, digitalRead(ledPin6));
  ThingSpeak.setField(8, digitalRead(ledPin7));

  if (digitalRead(sensorPin1) == LOW && flagStart == 0) {
    timer1 = millis();
    flagStart = 1;
  }

  if (digitalRead(sensorPin2) == LOW && flagEnd == 0) {
    timer2 = millis();
    flagEnd = 1;
  }

  if (flagStart == 1 && flagEnd == 1) {
    if (timer1 > timer2) {
      elapsedTime = timer1 - timer2;
    } 
    else if (timer2 > timer1) {
      elapsedTime = timer2 - timer1;
    }
    elapsedTime = elapsedTime / 1000; // Convert milliseconds to seconds
    vehicleSpeed = (distance / elapsedTime); // v = d / t
    vehicleSpeed = vehicleSpeed * 3600; // Multiply by seconds per hour
    vehicleSpeed = vehicleSpeed / 1000; // Divide by meters per kilometer
  }

  if(vehicleSpeed==0) {
    LCD.setCursor(0, 3);
    if (flagStart == 0 && flagEnd == 0) {
      LCD.print("No car detected");
    } 
    else {
      LCD.print("Searching...    ");
    }
  } 
  else {
    LCD.setCursor(0, 2);
    LCD.print("Speed:");
    LCD.print(vehicleSpeed, 1);
    LCD.print("Km/Hr  ");
    LCD.setCursor(0, 3);
    Serial.print("\n\nSpeed: ");
    Serial.print(vehicleSpeed);
    Serial.print(" Km/Hr");

    if (vehicleSpeed > 50) {
      LCD.print("Over Speeding!");
      Serial.println("\nOver Speeding");
      tone(buzzerPin, 1000);
      delay(1000);
      noTone(buzzerPin);
    } 
    else {
      LCD.print("Normal Speed");
      Serial.println("\nNormal Speed");
    }
    
    // Set ThingSpeak fields with sensor values
    ThingSpeak.setField(1, vehicleSpeed);

    delay(3000);
    LCD.clear();
    digitalWrite(buzzerPin, LOW);
    vehicleSpeed=0;
    flagStart = 0;
    flagEnd = 0;
  }

  // Write to the ThingSpeak channel
  int status = ThingSpeak.writeFields(channelNum, API_Key);
  if (status == 200) {
    Serial.println("\nChannel update successful.");
  }
  //delay(100);
}
