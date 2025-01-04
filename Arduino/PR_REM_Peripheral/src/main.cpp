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
union floatToBytes
{

  char charBuffer[4]; // 8 * 4 = 32 bytes
  float floatingBuffer;

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

char msgBuffer[256];

void receiveEvent(int howMany)
{

  // first byte is message type
  byte messageType = Wire.read();
  switch (messageType)
  {
  // Msg
  case 0x01:
    Wire.readString().toCharArray(msgBuffer, 256);
    break;
  case 0x02:
    // first byte is light lvl
    currLight = Wire.read();

    // note: [] refactor this later to allow for error to be made.

    // first set of 4 is heat index
    for (byte i = 0; i < 3; i++)
    {
      converter.charBuffer[i] = Wire.read();
    }
    converter.floatingBuffer = heatInd;

    // 2nd set of 4 is dew point
    for (byte i = 0; i < 3; i++)
    {
      converter.charBuffer[i] = Wire.read();
    }
    converter.floatingBuffer = dewPnt;

    // 3rd set of 4 is temp (C)
    for (byte i = 0; i < 3; i++)
    {
      converter.charBuffer[i] = Wire.read();
    }
    converter.floatingBuffer = temp;

    // 4th set of 4 is humidity (C)
    for (byte i = 0; i < 3; i++)
    {
      converter.charBuffer[i] = Wire.read();
    }
    converter.floatingBuffer = humidity;
    break;
  default:
    Serial.println("Unknown message type!");
    break;
  }
}

void updateScreen()
{
  lcd_1.clear();
  if (temp == -1)
  {
    lcd_1.print("No data sent!");
    lcd_1.setCursor(0, 1);
    lcd_1.print("(Check I2C)");
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
    lcd_1.print(String(humidity) + "°C");
  default:
    lcd_1.print("Out of state!");
    lcd_1.setCursor(0, 1);
    lcd_1.print("Whoops.");
    break;
  }
}

/**
 * ISR function to reduce state #
 */
void reduceState()
{

  if (state == 0)
  {
    state = 3;
  }
  else
  {
    state--;
  }
  Serial.print("Value of state: ");
  Serial.println(state);
  updateScreen();
  // delay(5000);
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
  delay(3000);

  // Parameters: interupt object (handler?), functionToCall, pinState
  // allow btn to change state regardless of where the board is in code.
  // (also btw, interruptPinDwn is INT.0, hence interrupt 0.)
  attachInterrupt(0, reduceState, FALLING);
}

void loop()
{
  delay(3000);
  if (!justOverridenFlow)
  {
    updateScreen();
    if (state == 4)
    {
      state = 0;
    }
    else
    {
      state++;
    }
  }
  else
  {
    justOverridenFlow = false;
  }
}
