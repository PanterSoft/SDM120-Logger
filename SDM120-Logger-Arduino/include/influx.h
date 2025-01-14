#ifndef INFLUX_H
#define INFLUX_H

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <queue>
#include "config.h"

// Declare the InfluxDB client
extern InfluxDBClient client;

extern String lastDBError;
extern bool influxStatus;

void checkInfluxDBConnection();
void sendDataToInfluxDB(float voltage, float current, float power, float apparentPower, float reactivePower, float powerFactor, float frequency, float importActiveEnergy, float exportActiveEnergy, float importReactiveEnergy, float exportReactiveEnergy, float totalActiveEnergy, float totalReactiveEnergy, uint8_t slaveID);

#endif // INFLUX_H