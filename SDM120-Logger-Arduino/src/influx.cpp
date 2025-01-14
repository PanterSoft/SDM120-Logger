// @PanterSoft
#include "influx.h"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

// Define the last database error string
String lastDBError = "";

// Define the influxStatus variable
bool influxStatus = false;

void checkInfluxDBConnection() {
  if (client.validateConnection()) {
    influxStatus = true;
  } else {
    influxStatus = false;
    lastDBError = client.getLastErrorMessage();
  }
}

void sendDataToInfluxDB(float voltage, float current, float power, float apparentPower, float reactivePower, float powerFactor, float frequency, float importActiveEnergy, float exportActiveEnergy, float importReactiveEnergy, float exportReactiveEnergy, float totalActiveEnergy, float totalReactiveEnergy, uint8_t slaveID) {
  // Create a point and set measurement name
  Point sensor("Meter_data");

  // Add tags
  sensor.addTag("device", "SDM120M");
  sensor.addTag("slaveID", String(slaveID));

  // Add fields
  sensor.addField("voltage", voltage);
  sensor.addField("current", current);
  sensor.addField("power", power);
  sensor.addField("apparentPower", apparentPower);
  sensor.addField("reactivePower", reactivePower);
  sensor.addField("powerFactor", powerFactor);
  sensor.addField("frequency", frequency);
  sensor.addField("importActiveEnergy", importActiveEnergy);
  sensor.addField("exportActiveEnergy", exportActiveEnergy);
  sensor.addField("importReactiveEnergy", importReactiveEnergy);
  sensor.addField("exportReactiveEnergy", exportReactiveEnergy);
  sensor.addField("totalActiveEnergy", totalActiveEnergy);
  sensor.addField("totalReactiveEnergy", totalReactiveEnergy);

  try {
    if (!client.writePoint(sensor)) {
      lastDBError = client.getLastErrorMessage();
    } else {
      lastDBError = "";
    }
  } catch (const std::exception& e) {
    lastDBError = e.what();
  }
}