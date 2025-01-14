#include <Arduino.h>
#include <WiFi.h>
#include "modbus.h"
#include "display.h"
#include "influx.h"
#include "config.h"

bool wifiConnected = false;
unsigned long previousMillis = 0;
unsigned long interval = 5000;

void aggregateData() {
  digitalWrite(HEARTBEAT_PIN, HIGH);

  // Handle button press for scrolling
  handleButtonPress();

  if (wifiConnected && influxStatus) {

    for (int i = 0; i < NUM_SLAVES; i++) {
      readModbusData(i);
      while (read_done != true) {
        delay(50);
      }
      sendDataToInfluxDB(voltage, current, power, apparentPower, reactivePower,
                      powerFactor, frequency, importActiveEnergy, exportActiveEnergy,
                      importReactiveEnergy, exportReactiveEnergy, totalActiveEnergy,
                      totalReactiveEnergy, slaveIDs[i]);
      // Update display with the latest data
      updateDisplay(voltage, current, power, apparentPower, reactivePower, powerFactor, frequency, importActiveEnergy, exportActiveEnergy, importReactiveEnergy, exportReactiveEnergy, totalActiveEnergy, totalReactiveEnergy, MBError, lastDBError, i);
      digitalWrite(HEARTBEAT_PIN, LOW);
    }
  }
}

void setup() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  pinMode(HEARTBEAT_PIN, OUTPUT);
  pinMode(WIFI_CONNECTED_PIN, OUTPUT);
  setupModbus();
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  setupDisplay();
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    digitalWrite(WIFI_CONNECTED_PIN, HIGH);
    delay(100);
    digitalWrite(WIFI_CONNECTED_PIN, LOW);
    delay(100);
    digitalWrite(WIFI_CONNECTED_PIN, HIGH);
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
    wifiConnected = false;
    digitalWrite(WIFI_CONNECTED_PIN, LOW);
  } else if (WiFi.status() == WL_CONNECTED) && (currentMillis - previousMillis >=interval) {
    checkInfluxDBConnection();
    wifiConnected = true;
    digitalWrite(WIFI_CONNECTED_PIN, HIGH);
  }

  // Aggregate data and send to InfluxDB
  aggregateData();
}