// @PanterSoft
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "modbus.h"
#include "influx.h"
#include "config.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int currentLine = 0;
const int totalLines = 15;  // Number of lines to Display + 1
const int linesPerPage = 4;  // Number of lines to display per page

// Variable to track the current slave index
int currSlaveIndex = 0;

void setupDisplay() {
  // Initialize the display
  Wire.begin(16, 17);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    //Serial.println(F("SSD1306 allocation failed"));
  }
  display.display();
  delay(2000);  // Pause for 2 seconds
  display.clearDisplay();
}

void drawStatusBar() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Draw WiFi status letter
  display.setCursor(0, 0);
  if (wifiConnected) {
    display.print("W");
  } else {
    display.print(" ");
  }

  // Draw Modbus status letter
  display.setCursor(16, 0);
  if (modbusStatus) {
    display.print("M");
  } else {
    display.print(" ");
  }

  // Draw Database status letter
  display.setCursor(32, 0);
  if (influxStatus) {
    display.print("D");
  } else {
    display.print(" ");
  }

  display.setCursor(48, 0);
  display.print("S:" + String(slaveIDs[currSlaveIndex]));

  // Draw horizontal line to separate status bar from the rest of the display
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
}

void updateDisplay(float voltage, float current, float power, float apparentPower, float reactivePower, float powerFactor, float frequency, float importActiveEnergy, float exportActiveEnergy, float importReactiveEnergy, float exportReactiveEnergy, float totalActiveEnergy, float totalReactiveEnergy, String MBError, String lastDBError, uint8_t slaveIndex) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  drawStatusBar();

  bool debug = true;

  if (debug == true) {
    if (slaveIDs[slaveIndex] == slaveIDs[currSlaveIndex]) {
      for (int i = 0; i < linesPerPage; i++) {
        int lineIndex = (currentLine + i) % totalLines;
        display.setCursor(0, i * 10 + 12);  // Adjust the cursor position for each line

        switch (lineIndex) {
          case 3:
            display.print("Voltage: "); display.print(voltage); display.println(" V");
            break;
          case 13:
            display.print("Current: "); display.print(current); display.println(" A");
            break;
          case 2:
            display.print("Power: "); display.print(power); display.println(" W");
            break;
          case 14:
            display.print("App. Power: "); display.print(apparentPower); display.println(" VA");
            break;
          case 4:
            display.print("Rea. Power: "); display.print(reactivePower); display.println(" VAr");
            break;
          case 5:
            display.print("Pwr. Factor: "); display.println(powerFactor);
            break;
          case 6:
            display.print("Frequency: "); display.print(frequency); display.println(" Hz");
            break;
          case 7:
            display.print("Imp.Act.En.: "); display.print(importActiveEnergy); display.println(" kWh");
            break;
          case 8:
            display.print("Exp.Act.En.: "); display.print(exportActiveEnergy); display.println(" kWh");
            break;
          case 9:
            display.print("Imp.Rea.En.:"); display.print(importReactiveEnergy); display.println("kVArh");
            break;
          case 10:
            display.print("Exp.Rea.En.:"); display.print(exportReactiveEnergy); display.println("kVArh");
            break;
          case 11:
            display.print("Tot.Act.En.: "); display.print(totalActiveEnergy); display.println(" kWh");
            break;
          case 12:
            display.print("Tot.Rea.En.:"); display.print(totalReactiveEnergy); display.println("kVArh");
            break;
          case 0:
            display.print("MB Error: "); display.println(MBError);
            break;
          case 1:
            display.print("DB Error: "); display.println(lastDBError);
            break;
        }
      }
    } else {
      display.setCursor(0, 12);  // Adjust the cursor position for each line
      display.print("MB Error: "); display.println(MBError);
      display.print("DB Error: "); display.println(lastDBError);
    }
    display.display();
  }
}

void handleButtonPress() {
  float voltage = readButtonVoltage();

  if (voltage > BUTTON_UP_VOLTAGE - BUTTON_THRESHOLD && voltage < BUTTON_UP_VOLTAGE + BUTTON_THRESHOLD) {
    currentLine = (currentLine - 1 + totalLines) % totalLines;  // Scroll up
  } else if (voltage > BUTTON_DOWN_VOLTAGE - BUTTON_THRESHOLD && voltage < BUTTON_DOWN_VOLTAGE + BUTTON_THRESHOLD) {
    currentLine = (currentLine + 1) % totalLines;  // Scroll down
  } else if (voltage > BUTTON_CONFIRM_VOLTAGE - BUTTON_THRESHOLD && voltage < BUTTON_CONFIRM_VOLTAGE + BUTTON_THRESHOLD) {
    currSlaveIndex = (currSlaveIndex + 1) % NUM_SLAVES;  // Switch to the next slave
  }
}

float readButtonVoltage() {
  int buttonValue = analogRead(BUTTON_PIN);
  return buttonValue * (3.3 / 4095.0);  // Convert analog value to voltage
}