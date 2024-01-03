#include <WiFi.h>
#include "ThingSpeak.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 20, 4);

// Define WiFi credentials
char ssid[] = "Wokwi-GUEST";      // Network SSID (name)
char pass[] = "";                 // Network password
WiFiClient client;

// ThingSpeak channel details
unsigned long channelNum = {{ CHANNEL_NUM }};               // Channel number taken from cloud
const char *API_Key = "{{ API_KEY }}";        // WriteAPI key of cloud

int timer1;
int timer2;

float elapsedTime;

int flagStart = 0;
int flagEnd = 0;

float distance = 5.0;
float vehicleSpeed;
float nonZeroSpeed;

int carCount=0;

int sensorPin1 = 34;
int sensorPin2 = 35;

int buzzerPin = 2;

void setup() {
  Serial.begin(115200);
  Serial.print("\nVehicle Speed Detection System\n");
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(buzzerPin, OUTPUT);

  LCD.init();
  LCD.backlight();
  LCD.setCursor(0, 0);
  LCD.print("   Speed Detector");
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
  LCD.setCursor(2, 1);
  LCD.print("Wifi Connected!");
  delay(1300);
  LCD.clear();
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
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

  if (vehicleSpeed == 0) {
    LCD.setCursor(0, 3);
    if (flagStart == 0 && flagEnd == 0) {
      LCD.print("No car detected");
    } 
    else {
      LCD.print("Searching...    ");
    }
  } 
  else {
    carCount=carCount+1;
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Car Count: ");
    LCD.print(carCount);
    nonZeroSpeed=vehicleSpeed;
    LCD.setCursor(0, 2);
    LCD.print("Speed:");
    LCD.print(vehicleSpeed, 1);
    LCD.print("Km/Hr  ");
    LCD.setCursor(0, 3);
    Serial.print("\nSpeed: ");
    Serial.print(vehicleSpeed);
    Serial.print(" Km/Hr");

    if (vehicleSpeed > 50) {
      LCD.print("Over Speeding!  ");
      Serial.println("\nOver Speeding");
      tone(buzzerPin, 1000);
      delay(1000);
      noTone(buzzerPin);
    } 
    else {
      LCD.print("Normal Speed   ");
      Serial.println("\nNormal Speed");
    }
    
    // Set ThingSpeak fields with sensor values
    ThingSpeak.setField(1, nonZeroSpeed);

    // Write to the ThingSpeak channel
    int status = ThingSpeak.writeFields(channelNum, API_Key);
    if (status == 200) {
      Serial.println("\nChannel update successful.");
    }

    delay(1500);
    digitalWrite(buzzerPin, LOW);
    vehicleSpeed = 0;
    flagStart = 0;
    flagEnd = 0;
  }
}
