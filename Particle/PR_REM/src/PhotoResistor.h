#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include "arduino.h"

#define ALPHA 0.75 // percentage of new value to keep

// :: low pass filter imp ::
class PhotoResistor
{
public:
    // These constants won't change:
    PhotoResistor(byte pin);

    byte getPhotoresistorValue();

private:
    const byte lightSenPin;
    byte prevOut = 0;

    // CALEBRATE THESE!! (for proper values)
    const uint16_t maxVal = 975; // the sensor value
    const uint16_t minVal = 54;  // the sensor value
};

#endif