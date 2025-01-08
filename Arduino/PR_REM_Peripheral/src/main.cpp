/*
  MK2 ~ PR_REM
  Central Board Code

  Uses Wire Library
  Uses a photoresistor on port A0.

  Uses code from http://www.arduino.cc/en/Tutorial/Calibration


 */
#include <Arduino.h>

#include <Wire.h>

#include <LiquidCrystal.h>

// float datatype takes same address as float
union FloatToBytes
{
  float floatValue;
  uint8_t bytes[sizeof(float)];
} converter;

uint8_t state = 0;

// highly optimized :>
// Pin that reduces page number
const byte interruptPinDwn = 2;

LiquidCrystal lcd_1(12, 11, 10, 7, 9, 8);

byte currLight;

// note [] reorg as struct?
// heat index
float heatInd;
// dew point
float dewPnt;
// Temperature (Celsius)
// set this to -1 to test if I2C transmission exists.
float temp = -1.0;
// humidity
float humidity;

// allow for delay to exist after changing screens
bool justOverridenFlow = false;

char msgBuffer[32];

void receiveEvent(int howMany)
{
  // first byte is message type
  byte messageType = Wire.read();

  // fixes jump case label issue

  Serial.print("Message Type: ");
  Serial.println(messageType);

  byte _msgIndex = 0;

  switch (messageType)
  {
  // Msg
  case 0x01:
    Serial.println("Received message packet!");
    // Wire.readString(); // Big no no
    while (Wire.available())
    {
      msgBuffer[_msgIndex] = Wire.read();
      ++_msgIndex;
    }
    Serial.print("Message: ");
    Serial.println(msgBuffer);
    break;
  case 0x02:
    Serial.println("Received data packet!");
    // first byte is light lvl
    currLight = Wire.read();

    Serial.print("Current light level: ");
    Serial.println(currLight);

    // first set of 4 is heat index
    for (byte i = 0; i < sizeof(float); i++)
    {
      if (Wire.available())
      {
        converter.bytes[i] = Wire.read();
      }
      else
      {
        Serial.println("Not enough data sent!");
        return;
      }
    }
    Serial.print("Heat Index: ");
    Serial.println(converter.floatValue);
    heatInd = converter.floatValue;

    // 2nd set of 4 is dew point
    for (byte i = 0; i < sizeof(float); i++)
    {
      if (Wire.available())
      {
        converter.bytes[i] = Wire.read();
      }
      else
      {
        Serial.println("Not enough data sent!");
        return;
      }
    }
    Serial.print("Dew Point: ");
    Serial.println(converter.floatValue);
    dewPnt = converter.floatValue;

    // 3rd set of 4 is temp (C)
    for (byte i = 0; i < sizeof(float); i++)
    {
      if (Wire.available())
      {
        converter.bytes[i] = Wire.read();
      }
      else
      {
        Serial.println("Not enough data sent!");
        return;
      }
    }
    Serial.print("Temperature: ");
    Serial.println(converter.floatValue);
    temp = converter.floatValue;

    // 4th set of 4 is humidity (C)
    for (byte i = 0; i < sizeof(float); i++)
    {
      if (Wire.available())
      {
        converter.bytes[i] = Wire.read();
      }
      else
      {
        Serial.println("Not enough data sent!");
        return;
      }
    }
    Serial.print("Humidity: ");
    Serial.println(converter.floatValue);
    humidity = converter.floatValue;
    break;
  default:
    Serial.println("Unknown message type!");
    break;
  }
}

// ::: Screen States :::

#define MAXSTATE 5
// allow for message to scroll
bool msgScroll = false;

// :: focus timer stuff ::
// unsigned long timeInit = 0;

// updates screen
void updateScreen()
{
  lcd_1.clear();
  if (temp == -1)
  {
    lcd_1.print("No data!");
    lcd_1.setCursor(0, 1);
    lcd_1.print("Is I2C working?");
    return;
  }
  switch (state)
  {
  case 0:
    // Light lvl
    lcd_1.print("Current Light:");
    lcd_1.setCursor(0, 1);
    lcd_1.print((int)map(currLight, 0, 255, 0, 100));
    lcd_1.print("%");
    break;
  case 1:
    // Temp
    lcd_1.print("Temperature:");
    lcd_1.setCursor(0, 1);
    lcd_1.print(String(temp) + "°C");
    break;
  case 2:
    // Humdity
    lcd_1.print("Room Humidity:");
    lcd_1.setCursor(0, 1);
    lcd_1.print(String(humidity) + "%");
    break;
  case 3:
    // Dew Point
    lcd_1.print("Dew Point:");
    lcd_1.setCursor(0, 1);
    lcd_1.print(String(humidity) + "°C");
    break;
  case 4:
    // Dew Point
    lcd_1.print("Heat Index:");
    lcd_1.setCursor(0, 1);
    lcd_1.print(String(heatInd) + "°C");
    break;
  case 5:
    // to be refactored []
    // msg
    // mind, we have 16x2 screen, so we can only display 16 chars at a time.
    if (msgBuffer[0] == '\0')
    {
      lcd_1.print("No message!");
      break;
    }
    else
    {
      lcd_1.print("Incoming");
      lcd_1.setCursor(0, 1);
      lcd_1.print("Message!");
      delay(2000);
      lcd_1.clear();

      // bring the family of if statements...
      // is there a better way to do this?
      for (byte i = 0; i < 16; i++)
      {
        if (msgBuffer[i] == '\0')
        {
          break;
        }
        lcd_1.print(msgBuffer[i]);
      }
      if (msgBuffer[16] == '\0')
      {
        lcd_1.setCursor(0, 1);
        for (byte i = 16; i < 32; i++)
        {
          if (msgBuffer[i] == '\0')
          {
            break;
          }
          lcd_1.print(msgBuffer[i]);
        }
      }
      // let the message linger for a bit
      delay(3000);
    }
    break;
  case 6:
    // focus timer

    break;
  default:
    lcd_1.print("Out of state!");
    lcd_1.setCursor(0, 1);
    lcd_1.print("Whoops.");
    break;
  }
}

// timer
unsigned long lastTime = 0;

/**
 * ISR function to reduce state #
 */
void reduceState()
{
  if (state == 0)
  {
    state = MAXSTATE;
  }
  else
  {
    state--;
  }
  // reset the timer to move state
  lastTime = millis();
}

void setup()
{
  Serial.begin(9600);
  lcd_1.begin(16, 2); // Set up the number of columns and rows on the LCD.

  // Print a message to the LCD.
  lcd_1.print("Initializing");

  // init I2C Wire library
  Wire.begin(4);
  Wire.onReceive(receiveEvent); // register event

  // Allows for btn press to be registered >:(
  pinMode(interruptPinDwn, INPUT_PULLUP);

  // allow for Initialization screen to be up without interuptions
  // (also allow for an I2C packet to come in.)
  delay(3000);

  // Parameters: interupt object num, functionToCall, pinState
  // allow btn to change state regardless of where the board is in code.
  // (also btw, interruptPinDwn is INT.0, hence interrupt 0.)
  attachInterrupt(0, reduceState, FALLING);
}

void loop()
{
  // if 5 seconds have passed, change state
  if ((millis() - lastTime > 5000) || msgScroll)
  {
    lastTime = millis();
    if (state == 0)
    {
      state = MAXSTATE;
    }
    else
    {
      state--;
    }
  }
  // and update screen regardless of state
  updateScreen();
  delay(625);
}
