/*
 * PR_REM ~ Central Borad
 * Author: Disguised_Coffee
 * Date: 12-30-24
 *
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Arduino.h"
#include "PhotoResistor.h"

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


// :: ArduinoJson Config ::
/**
 * Fixes error:
 * 
 * ../wiring/inc/avr/pgmspace.h:148:29: error: 'const void*' 
 * is not a pointer-to-object type 
 *  148 | #define pgm_read_ptr(addr) (*(const void *)(addr))
 * 
 * This is supposed to be fixed in the latest version of ArduinoJson,
 * but some reason the earlier fix is not working.
 */
#define ARDUINOJSON_ENABLE_PROGMEM 0

// Does JSON parsing and making easier
#include "ArduinoJson.h"

#define DHTPIN D8     // what pin we're connected to
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// :: changing vars ::
// heat index
float hi;
// dew point
float dp;
// Temperature (Celsius)
float t;
// humidity
float h;

PhotoResistor photoResistor(A0);

// float datatype takes same address as float
// (purpose of union is to share same memory location)
union floatToBytes
{
  char charBuffer[4]; // 8 * 4 = 32 bytes
  float floatingBuffer;
} converter;

char *convertToByte(float value)
{
  converter.floatingBuffer = value;
  return converter.charBuffer;
}

// this data will be sent to the peripheral board over I2C
char incomingData[256];

// ::: Send Env Information :::
char buffer[256];
StaticJsonDocument<256> doc;

/**
 * Handle incoming messages from the Particle Cloud
 */
void msgHandler(const char *event, const char *data)
{
  DeserializationError err = deserializeJson(doc, data);
  if (err)
  {
    Log.error("Failed to parse incoming data");
    return;
  }

  // Get the JsonArray from the JsonDocument
  JsonArray array = doc.as<JsonArray>();

  // array should have 1 element of "msg"
  if (array.size() != 1)
  {
    Log.error("Invalid message sent: array size too long");
    return;
  }

  /**
   * There's an issue here. []
   * 
   * Doing this does not work:
   * array[0]["msg"].is<char *>())
   * 
   * >:C
   */
  const char *msg = array[0]["msg"];
  if (msg == NULL)
  {
    Log.error("Invalid message sent: No msg key");
    return;
  }
  else if(strlen(msg) > 255)
  {
    Log.error("Invalid message sent: Message too long");
    return;
  }
  else
  {
    // copy the message from integration
    strcpy(incomingData, msg);
  }
}

/**
 * Send data to the peripheral board via I2c
 *
 * byte startingByte ~
 *    0x01 --> MSG
 *    0x02 --> sensor data
 */
void sendDataOverI2C(byte startingByte)
{
  Wire.beginTransmission(4); // transmit to device #4

  Wire.write(startingByte);
  switch (startingByte)
  {
  case 0x01:
    Wire.write(incomingData);
    break;

  case 0x02:
    Wire.write(photoResistor.getPhotoresistorValue());

    // transfer heat index
    Wire.write(convertToByte(hi));

    // dew point
    Wire.write(convertToByte(dp));

    // temp (celsius)
    Wire.write(convertToByte(t));

    // humdity
    Wire.write(convertToByte(h));
    break;

  default:
    Log.error("Invalid starting byte");
    break;
  }
  byte err = Wire.endTransmission(); // stop transmitting

  // check for errors in I2C transmission
  // btw, Log class is like Serial, but Cloud based.
  if (err)
  {
    Log.info("Error when sending values");
    if (err == 5)
      Log.warn("It was a timeout");
  }
}

/**
 * Grab variables, and send them.
 */
void sendDataToIntegration()
{
  doc.clear();

  Log.info("Sending data...");
  doc["lightLvl"] = String(photoResistor.getPhotoresistorValue());
  doc["temp"] = String(t);
  doc["humid"] = String(h);
  doc["dew"] = String(dp);
  doc["heatIn"] = String(hi);

  serializeJson(doc, buffer);
  Particle.publish("envInfo", buffer);
}

byte updateSensors()
{
  hi = dht.getHeatIndex();
  t = dht.getTempCelcius();

  if (isnan(hi) || isnan(t))
  {
    return 0x01;
  }
  else
  {
    dp = dht.getDewPoint();
    h = dht.getHumidity();
    return 0x00;
  }
}

// Prevents data from being sent if sensor error occurs
byte sensorError;

/**
 * Sends data to the peripheral board
 * and Particle Cloud
 */
void updateData()
{
  if (sensorError)
  {
    Log.warn("Failed to read from DHT sensor");
    return;
  }
  else
  {
    sendDataToIntegration();
    sendDataOverI2C(0x02);
  }
}

// allows for data to be sent every 5 seconds
Timer timer(5000, updateData);

void setup()
{
  // inits.
  Wire.begin();
  dht.begin();

  // [] add interrupt to send data via integration
  Particle.subscribe("getMsg", msgHandler);
  timer.start();
}

void loop()
{
  // update sensors
  sensorError = updateSensors();
  if (sensorError)
  {
    Log.warn("Failed to read from DHT sensor");
    return;
  }
  delay(2000);
}
