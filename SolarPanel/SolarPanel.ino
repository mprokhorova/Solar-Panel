/**************************************************************
.
 * WARNING :
 * For this example you'll need SimpleTimer library:
 *   https://github.com/jfturcot/SimpleTimer
 * and Adafruit DHT sensor library:
 *   https://github.com/adafruit/DHT-sensor-library
 *
 * App project setup:
 *   Value Display widget attached to V5
 *   Value Display widget attached to V6
 *
 **************************************************************/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <SimpleTimer.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "0Omxoqk0MWo09_5BUIOT4q_ufd29sZYg";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MaryPro_mts";
char pass[] = "hanna@ha?9605";

#define DHTPIN D3          // What digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float current = 0;
  float shuntvoltage = 0;
  float power = 0;

  shuntvoltage = ina219.getShuntVoltage_mV()*1000;
  //busvoltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA()*1000;
  power = ina219.getPower_mW()*1000;

  if (isnan(current) || isnan(shuntvoltage)) {
    Serial.println("Failed to read from Ina219 sensor!");
    return;
  }
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V7, shuntvoltage);
  Blynk.virtualWrite(V8, current);
  Blynk.virtualWrite(V9, power);


  //Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" V");
  //Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current); Serial.println(" A");
  Serial.print("Power:         "); Serial.print(power); Serial.println(" W");
  Serial.println("");

}

void setup()
{
  Serial.begin(9600); // See the connection status in Serial Monitor
  Blynk.config(auth, IPAddress(192, 168, 0, 107), 8080);
  Blynk.connect();
  //Blynk.begin(auth, ssid, pass);
  
  dht.begin();
  ina219.begin();
  
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}
