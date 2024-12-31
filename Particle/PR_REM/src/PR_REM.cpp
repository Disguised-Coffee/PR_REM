/*
 * PR_REM ~ Central Borad
 * Author: Disguised_Coffee
 * Date: 12-30-24
 *
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// leaving stuff above for later usage c;

// :: code adapted from Mk2 build ::
#include "Adafruit_DHT_Particle.h"

#define DHTPIN D8     // what pin we're connected to
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// These constants won't change:
const byte lightSenPin = A0; // pin that the sensor is attached to

// variables:
uint8_t oldLightLvl = 0;  // the sensor value
uint8_t currLightLvl = 0; // the sensor value

// :: changing vars ::
// heat index
float hi;
// dew point
float dp;
// Temperature (Celsius)
float t;
// humidity
float h;

// CALEBRATE THESE!! (for proper values)
uint16_t maxVal = 975; // the sensor value
uint16_t minVal = 54;  // the sensor value

// float datatype takes same address as float
union floatToBytes {
  
    char charBuffer[4]; // 8 * 4 = 32 bytes
    float floatingBuffer;
  
} converter;

void sendData()
{
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(currLightLvl);

  // transfer heat index
  converter.floatingBuffer = hi;
  Wire.write(converter.charBuffer);

  // dew point
  converter.floatingBuffer = dp;
  Wire.write(converter.charBuffer);
  
  // temp (celsius)
  converter.floatingBuffer = t;
  Wire.write(converter.charBuffer);

  // dew point
  converter.floatingBuffer = h;
  Wire.write(converter.charBuffer);

  byte err = Wire.endTransmission(); // stop transmitting

  if (err)
  {
    // No Serial! Use log instead for debugging messages in the cloud!
    Log.info("Error when sending values");
    if (err == 5)
      Log.warn("It was a timeout");
  }
}

void setup()
{
  // inits.
  Wire.begin();
  dht.begin();
}

void loop()
{
  // read the sensor:
  // Note [] This needs a filter.
  currLightLvl = map(analogRead(lightSenPin), minVal, maxVal, 0, 254);
  hi = dht.getHeatIndex();
  t = dht.getTempCelcius();

  if (isnan(hi) || isnan(t))
  {
    Log.warn("Failed to read from DHT sensor!");
    return;
  }
  else
  {
    dp = dht.getDewPoint();
    h = dht.getHumidity();
    
  }
  // send data regardless for now.
  sendData();
  delay(2000);
}