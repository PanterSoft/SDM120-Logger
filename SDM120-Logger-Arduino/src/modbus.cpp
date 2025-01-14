// @PanterSoft
#include "modbus.h"
#include "config.h"
#include <Arduino.h>
#include <SDM.h>

// Define arrays for each variable
float voltage, current, power, apparentPower, reactivePower, powerFactor, frequency, importActiveEnergy, exportActiveEnergy, importReactiveEnergy, exportReactiveEnergy, totalActiveEnergy, totalReactiveEnergy;

String MBError = "";
bool modbusStatus = false;
bool read_done = false;

// Use HardwareSerial instead of SoftwareSerial
HardwareSerial& sdmSerial = Serial;
SDM sdm(sdmSerial, SDM_UART_BAUD, DERE_PIN, SDM_UART_CONFIG, SDM_RX_PIN, SDM_TX_PIN);

uint8_t current_slaveID = slaveIDs[0];

void setupModbus() {
  //Serial.begin(SDM_UART_BAUD, SDM_UART_CONFIG, SDM_RX_PIN, SDM_TX_PIN);
  sdm.begin();
}

void get_result(uint16_t reg, float val) {
  switch (reg) {
    case SDM_PHASE_1_VOLTAGE: voltage = val; break;           // SDM_PHASE_1_VOLTAGE 0x0000
    case SDM_PHASE_1_CURRENT: current = val; break;           // SDM_PHASE_1_CURRENT 0x0006
    case SDM_PHASE_1_POWER: power = val; break;             // SDM_PHASE_1_POWER 0x000C
    case SDM_PHASE_1_APPARENT_POWER: apparentPower = val; break;     // SDM_PHASE_1_APPARENT_POWER 0x0012
    case SDM_PHASE_1_REACTIVE_POWER: reactivePower = val; break;     // SDM_PHASE_1_REACTIVE_POWER 0x0018
    case SDM_PHASE_1_POWER_FACTOR: powerFactor = val; break;       // SDM_PHASE_1_POWER_FACTOR 0x001E
    case SDM_FREQUENCY: frequency = val; break;         // SDM_FREQUENCY 0x0046
    case SDM_IMPORT_ACTIVE_ENERGY: importActiveEnergy = val; break;  // SDM_IMPORT_ACTIVE_ENERGY 0x0048
    case SDM_EXPORT_ACTIVE_ENERGY: exportActiveEnergy = val; break;  // SDM_EXPORT_ACTIVE_ENERGY 0x0162
    case SDM_IMPORT_REACTIVE_ENERGY: importReactiveEnergy = val; break;  // SDM_IMPORT_REACTIVE_ENERGY 0x0180
    case SDM_EXPORT_REACTIVE_ENERGY: exportReactiveEnergy = val; break;  // SDM_EXPORT_REACTIVE_ENERGY 0x004E
    case SDM_TOTAL_ACTIVE_ENERGY: totalActiveEnergy = val; break;  // SDM_TOTAL_ACTIVE_ENERGY 0x0156
    case SDM_TOTAL_REACTIVE_ENERGY: totalReactiveEnergy = val; break;  // SDM_TOTAL_REACTIVE_ENERGY 0x0158
    default: MBError = "Unknown register: " + String(reg); break;
  }
}

void alternativeRead(uint8_t slaveid) {
  voltage = sdm.readVal(SDM_PHASE_1_VOLTAGE, slaveid);
  current = sdm.readVal(SDM_PHASE_1_CURRENT, slaveid);
  power = sdm.readVal(SDM_PHASE_1_POWER, slaveid);
  apparentPower[slaveindex] = sdm.readVal(SDM_PHASE_1_APPARENT_POWER, slaveid);
  reactivePower[slaveindex] = sdm.readVal(SDM_PHASE_1_REACTIVE_POWER, slaveid);
  powerFactor = sdm.readVal(SDM_PHASE_1_POWER_FACTOR, slaveid);
  frequency[slaveindex] = sdm.readVal(SDM_FREQUENCY, slaveid);
  importActiveEnergy[slaveindex] = sdm.readVal(SDM_IMPORT_ACTIVE_ENERGY, slaveid);
  exportActiveEnergy[slaveindex] = sdm.readVal(SDM_EXPORT_ACTIVE_ENERGY, slaveid);
  importReactiveEnergy[slaveindex] = sdm.readVal(SDM_IMPORT_REACTIVE_ENERGY, slaveid);
  exportReactiveEnergy[slaveindex] = sdm.readVal(SDM_EXPORT_REACTIVE_ENERGY, slaveid);
  totalActiveEnergy[slaveindex] = sdm.readVal(SDM_TOTAL_ACTIVE_ENERGY, slaveid);
  totalReactiveEnergy[slaveindex] = sdm.readVal(SDM_TOTAL_REACTIVE_ENERGY, slaveid);
}

String translateError(uint8_t error) {
  switch (error) {
    case 0:  // SDM_ERR_NO_ERROR
      return "No error";
    case 1:  // SDM_ERR_ILLEGAL_FUNCTION
      return "Illegal function";
    case 2:  // SDM_ERR_ILLEGAL_DATA_ADDRESS
      return "Illegal data address";
    case 3:  // SDM_ERR_ILLEGAL_DATA_VALUE
      return "Illegal data value";
    case 5:  // SDM_ERR_SLAVE_DEVICE_FAILURE
      return "Slave device failure";
    case 11:  // SDM_ERR_CRC_ERROR
      return "CRC error";
    case 12:  // SDM_ERR_WRONG_BYTES
      return "Wrong bytes";
    case 13:  // SDM_ERR_NOT_ENOUGHT_BYTES
      return "Not enough bytes";
    case 14:  // SDM_ERR_TIMEOUT
      return "Timeout";
    case 15:  // SDM_ERR_EXCEPTION
      return "Exception";
    case 16:  // SDM_ERR_STILL_WAITING
      return "Still waiting";
    default:
      return "Unknown error";
  }
}

void readModbusData(uint8_t device_id) {
  read_done = false;
    current_slaveID = slaveIDs[device_id];

  uint8_t error = sdm.readValues(SDM_PHASE_1_VOLTAGE, SDM_TOTAL_REACTIVE_ENERGY, current_slaveID, get_result);

  if (error) {
    MBError = translateError(error);
    modbusStatus = false;
  } else {
    MBError = "";
    modbusStatus = true;
  }
  read_done = true;
}
