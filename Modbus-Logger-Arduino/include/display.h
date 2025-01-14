#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

void setupDisplay();
void updateDisplay(float voltage, float current, float power, float apparentPower, float reactivePower, float powerFactor, float frequency, float importActiveEnergy, float exportActiveEnergy, float importReactiveEnergy, float exportReactiveEnergy, float totalActiveEnergy, float totalReactiveEnergy, String MBError, String lastDBError, uint8_t slaveIndex);
void handleButtonPress();
float readButtonVoltage();

#endif // DISPLAY_H