
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

#include <DHT.h>
#include <Wire.h>
//#include <Adafruit_INA219.h>
#include "ACS712.h"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "b5PuOm-8WV1LuPmNqmjLwFhbK0jD18Zv";

#define W5100_CS  10
#define SDCARD_CS 4
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};


#define DHTPIN 3          // What digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

ACS712 sensor(ACS712_30A, A0);

void setup()
{
  // Debug console
  Serial.begin(9600);

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card

  Blynk.config(auth, IPAddress(192, 168, 0, 107), 8080);
  Blynk.connect();
  Blynk.begin(auth, IPAddress(192,168,0,107), 8080);


  dht.begin();
  //ina219.begin();
  sensor.calibrate();
  
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  timer.run(); // Initiates SimpleTimer
}


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

  //shuntvoltage = ina219.getShuntVoltage_mV()*1000;
  //busvoltage = ina219.getBusVoltage_V();
  //current = ina219.getCurrent_mA()*1000;
  //power = ina219.getPower_mW()*1000;

  //if (isnan(current) || isnan(shuntvoltage)) {
  //  Serial.println("Failed to read from Ina219 sensor!");
  //  return;
  //}
  shuntvoltage = 14;
  current = CurrentMeasure();
  power = shuntvoltage * current;
  
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

float CurrentMeasure()
{
  // We use 230V because it is the common standard in European countries
  // Change to your local, if necessary
  float U = 5;

  // To measure current we need to know the frequency of current
  // By default 50Hz is used, but you can specify desired frequency
  // as first argument to getCurrentAC() method, if necessary
  float I = sensor.getCurrentAC();

  // To calculate the power we need voltage multiplied by current
  float P = U * I;

  Serial.println(String("I = ") + I + " A");
  Serial.println(String("P = ") + P + " Watts");

  return I;
 }
