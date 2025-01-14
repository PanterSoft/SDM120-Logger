#ifndef MODBUS_H
#define MODBUS_H

#include <ModbusRTU.h>
#include <SDM.h>
#include "config.h"

extern ModbusRTU mb;

extern uint8_t current_slaveID;
extern String MBError;
extern bool modbusStatus;
extern bool read_done;

extern float voltage, current, power, apparentPower, reactivePower, powerFactor, frequency, importActiveEnergy, exportActiveEnergy, importReactiveEnergy, exportReactiveEnergy, totalActiveEnergy, totalReactiveEnergy;

void setupModbus();
void readModbusData(uint8_t device_id);
void alternativeRead(uint8_t slaveid, uint8_t slaveindex);

#endif // MODBUS_H