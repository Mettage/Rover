// Load libraries
// Load DHT22 Digital Temperature and Humidity Sensor library
#include "DHT.h"
// Load SD Card libraries
#include <SPI.h>
#include <SD.h>
// Load BMP085 Pressure sensor library
# include <Wire.h>
# include <Adafruit_Sensor.h>
# include <Adafruit_BMP085_U.h>

// DHT22 definition
#define DHTPIN 3              // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22         // Used type of DHT sensor (AM2302)
DHT dht(DHTPIN, DHTTYPE); 

// PGM definition
int photocellPin = 3;         // the cell and 10K pulldown are connected to a3
int photocellReading;         // the analog reading from the analog resistor divider

// BMP definition
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// TLE definition
int pin = 0;
const byte interruptPin = 2;  // Digital pin connected to the TLE sensor
volatile byte state = LOW;

// Define step
int steps = 1;

// Define output file 
File sensors;

void setup() 
{
  // Define serial rate
  Serial.begin(9600); 
  Serial.println("Sensors");  

  // Define SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("Initialization failed!");
    while (1);
  }
  Serial.println("Initialization done.");
    
  // Start dht communication
  dht.begin();
  bmp.begin();

  // Define TLE49x5L
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin),blink, CHANGE);   
}

void loop() 
{
  // Magnetic field
  Serial.print("Magnetic field: ");         
  Serial.println(state);    // Status of magnetic field threshold

  // Temperature and pressure
  sensors_event_t event;
  bmp.getEvent(&event);
  // Display atmospheric pressue in hPa 
  Serial.print("Pressure: ");
  Serial.print(event.pressure);
  Serial.println(" hPa");
  // First we get the current temperature from the BMP085 
  float temperature;
  bmp.getTemperature(&temperature);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  // Then convert the atmospheric pressure, and SLP to altitude         
  float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
  Serial.print("Altitude: "); 
  Serial.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure)); 
  Serial.println(" m");
  
  // Humidity
  float h = dht.readHumidity();
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));

  // Photoresistor
  photocellReading = analogRead(photocellPin);
  // Invert the reading value from 0-1023 to 1023-0
  photocellReading = 1023 - photocellReading;
  Serial.print("Brightness = ");
  Serial.println(photocellReading);
  // evaluation of brightness
  Serial.print("Brightness = ");  
  if (photocellReading < 10) {
  Serial.println("Dark");
  } else if (photocellReading < 200) {
  Serial.println("Dim");
  } else if (photocellReading < 500) {
  Serial.println("Light");
  } else if (photocellReading < 800) {
  Serial.println("Bright");
  } else {
  Serial.println(" - Very bright");
  }

  // Define and open output file
  sensors = SD.open("sensors.txt", FILE_WRITE);
  
  // Write on SD card
  if (sensors) {
    sensors.print(steps);
    sensors.print("  ");        
    sensors.print(state);
    sensors.print("  ");
    sensors.print(event.pressure);
    sensors.print("  ");
    sensors.print(temperature);
    sensors.print("  ");
    sensors.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure));
    sensors.print("  ");
    sensors.print(h);
    sensors.print("  ");
    sensors.print(photocellReading);
    sensors.println("  ");
    // close the file
    sensors.close();
  } else {
    // if the file didn't open, print an error
    Serial.println("Error opening sensors.txt");  
  }  
  
  Serial.println("");
  steps = steps+1;  
  delay(2000);               // Speed of writing values [ms]
}

void blink()
{
  state=!state;  
}
