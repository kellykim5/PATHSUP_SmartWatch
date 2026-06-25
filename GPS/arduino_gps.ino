#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Choose pins for Software Serial (RX, TX)
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// Initialize the LCD (0x27 is standard for PCF8574T, use 0x3F if it doesn't work)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// Variables to handle screen switching interval
unsigned long lastDisplayToggle = 0;
bool displayPage = 0;

void setup() {
 Serial.begin(115200);
 ss.begin(GPSBaud);
  // Initialize the I2C LCD
 lcd.init();
 lcd.backlight();
 lcd.print("GT-U7 GPS Init...");
  Serial.println(F("GT-U7 GPS Initialized"));
 delay(2000);
 lcd.clear();
}

void loop() {
 // Feed data from GPS to TinyGPS++ object
 while (ss.available() > 0) {
   if (gps.encode(ss.read())) {
     // Keep updating Serial Monitor whenever data comes in
     displayInfoSerial();
   }
 }

 // Update the LCD display periodically
 updateLCD();

 // If no data is received for 5 seconds, check wiring
 if (millis() > 5000 && gps.charsProcessed() < 10) {
   Serial.println(F("No GPS detected: check wiring."));
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("No GPS detected!");
   lcd.setCursor(0, 1);
   lcd.print("Check Wiring.");
   while(true);
 }
}

void updateLCD() {
 // Switch between info pages every 3 seconds (3000ms)
 if (millis() - lastDisplayToggle > 3000) {
   displayPage = !displayPage;
   lastDisplayToggle = millis();
   lcd.clear();
 }

 lcd.setCursor(0, 0);

 // Check if GPS has a satellite lock
 if (gps.location.isValid()) {
   if (displayPage == 0) {
     // PAGE 1: Coordinates
     lcd.print("Lat: ");
     lcd.print(gps.location.lat(), 5); // 5 decimals is a good balance for 16x2
     lcd.setCursor(0, 1);
     lcd.print("Lon: ");
     lcd.print(gps.location.lng(), 5);
   } else {
     // PAGE 2: Speed and Satellites
     lcd.print("Speed: ");
     lcd.print(gps.speed.kmph(), 1);
     lcd.print(" km/h");
    
     lcd.setCursor(0, 1);
     lcd.print("Sats: ");
     lcd.print(gps.satellites.value());
   }
 } else {
   // If the GT-U7 doesn't have a satellite fix yet
   lcd.print("Waiting for Fix");
   lcd.setCursor(0, 1);
   lcd.print("Satellites: ");
   lcd.print(gps.satellites.value());
 }
}

// Kept your original serial printing function untouched for debugging
void displayInfoSerial() {
 Serial.print(F("Location: "));
 if (gps.location.isValid()) {
   Serial.print(gps.location.lat(), 6);
   Serial.print(F(","));
   Serial.print(gps.location.lng(), 6);
 } else {
   Serial.print(F("INVALID"));
 }

 Serial.print(F("  Speed: "));
 if (gps.speed.isValid()) {
   Serial.print(gps.speed.kmph());
   Serial.println(F(" km/h"));
 } else {
   Serial.println(F("INVALID"));
 }
}

