#include "PhotoResistor.h"

/**
 * Apparently you don't have to redefine the entire class
 * 
 * wow.
 */

PhotoResistor::PhotoResistor(byte pin) : lightSenPin(pin) {};

byte PhotoResistor::getPhotoresistorValue()
{
    const byte toR = ALPHA * map(analogRead(lightSenPin), minVal, maxVal, 0, 254) + (1 - ALPHA) * prevOut;
    prevOut = toR;
    return toR;
}