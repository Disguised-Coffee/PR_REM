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
union FloatToBytes
{
  uint8_t bytes[sizeof(float)]; // 8 * 4 = 32 bytes
  float floatValue;
};

FloatToBytes floatConverter;

// this data will be sent to the peripheral board over I2C
char incomingData[32];

// ::: Send Env Information :::
char buffer[256];
StaticJsonDocument<256> doc;

/**
 * Issue:
 * 
 *  I2C transmission gets stuck when Particle function is activated.
 * 
 * Solution: don't send msg to peripheral board in Particle function, but rather set
 *           msgReady flag to true, and send the message in the main loop.  
 */
bool msgReady = false;


/**
 * Send data to the peripheral board via I2c
 *
 * byte startingByte ~
 *    0x04 --> MSG
 *    0x02 --> sensor data
 */
void sendDataOverI2C(byte startingByte)
{
  Wire.beginTransmission(4); // transmit to device #4
  Wire.reset();              // reset the I2C bus
  Wire.write(startingByte);
  switch (startingByte)
  {
  case 0x01:
    Log.info("Sending message: %s", incomingData);
    Wire.write(incomingData);
    // Log.info("Not sending a msg");
    break;

  case 0x02:
    Log.info("Sending sensor data...");
    Wire.write(photoResistor.getPhotoresistorValue());

    // transfer heat index
    floatConverter.floatValue = hi;
    Wire.write(floatConverter.bytes, sizeof(float));

    // dew point
    floatConverter.floatValue = dp;
    Wire.write(floatConverter.bytes, sizeof(float));

    // temp (celsius)
    floatConverter.floatValue = t;
    Wire.write(floatConverter.bytes, sizeof(float));

    // humidity
    floatConverter.floatValue = h;
    Wire.write(floatConverter.bytes, sizeof(float));
    Log.info("Sensor data sent!");
    break;

  default:
    Log.error("Invalid starting byte");
    break;
  }
  byte err = Wire.endTransmission(); // stop transmitting
  Wire.flush();

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
 * Handle incoming messages from the Particle Cloud
 */
void msgHandler(const char *event, const char *data)
{
  Serial.println("We got a message!");
  Serial.println(data);
  DeserializationError err = deserializeJson(doc, data);
  if (err)
  {
    Log.error("Failed to parse incoming data");
    return;
  }

  // array should have 1 element of "msg"
  if (doc.size() != 1)
  {
    Log.error("Invalid message sent: array size not 1");
    return;
  }

  // msg should be null by Particle standards if it can be found by key 
  // (try-catches are disabled)
  const char *msg = doc["msg"];
  if (msg == NULL)
  {
    Log.error("Invalid message sent: No msg key");
    return;
  }
  else if (strlen(msg) > 32)
  {
    Log.error("Invalid message sent: Message too long");
    return;
  }
  else
  {
    // copy the message from integration
    strcpy(incomingData, msg);
    Log.info("We just got a letter!!! :D");
    Log.info("Message: %s", incomingData);
    // sendDataOverI2C(0x01);  // don't do this here
    msgReady = true;
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
 *
 *
 * Also, NO RETURN STATEMENTS IN ISRs!!!!
 */
void updateData()
{
  // Log.info("SensorError: %d", sensorError);

  if (!sensorError)
  {
    sendDataToIntegration();
    sendDataOverI2C(0x02);
  }
  if(msgReady){
    sendDataOverI2C(0x01);
    msgReady = false;
  }
}

// allows for data to be sent every 5 seconds
// [] timer operations are too large???
// Timer timer(5000, updateData);

// Function to be run in the I2C thread
void i2cThreadFunction()
{
  while (true)
  {
    WITH_LOCK(Wire)
    {
      updateData();
    }
    delay(3000); // delay btwn each transmission
  }
}

// I2C Thread
Thread i2cThread("i2cThread", i2cThreadFunction);

void setup()
{
  // inits.
  Wire.begin();
  dht.begin();

  // [] add interrupt to send data via integration
  Serial.begin(9600); // for debugging [] remove later
  Particle.subscribe("handle-msg", msgHandler); // bind function to webhook`
  // timer.start();
}

void loop()
{
  // update sensors
  sensorError = updateSensors();
  if (sensorError)
  {
    Log.warn("Failed to read from DHT sensor");
  }
  delay(3000);
}


