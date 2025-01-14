// @PanterSoft

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID getenv("WIFI_SSID")
#define WIFI_PASSWORD getenv("WIFI_PASSWORD")

// Wifi Connection Status
extern bool wifiConnected;

// InfluxDB Configuration
#define INFLUXDB_URL getenv("INFLUXDB_URL")
#define INFLUXDB_TOKEN getenv("INFLUXDB_TOKEN")
#define INFLUXDB_ORG getenv("INFLUXDB_ORG")
#define INFLUXDB_BUCKET getenv("INFLUXDB_BUCKET")
// Time zone info
#define TZ_INFO "UTC1"

// Modbus Configuration
#define SDM_RX_PIN 3
#define SDM_TX_PIN 1
#define DERE_PIN 4
#define SDM_UART_CONFIG SERIAL_8N1

// Define slave IDs (Does not work: 0x001, 0x008-10)
// , 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00A
#define NUM_SLAVES 10
const uint8_t slaveIDs[] = {0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00A};

// Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Button Configuration
#define BUTTON_PIN 32  // GPIO32
#define BUTTON_UP_VOLTAGE 1.25
#define BUTTON_DOWN_VOLTAGE 1.88
#define BUTTON_CONFIRM_VOLTAGE 2.72
#define BUTTON_THRESHOLD 0.2

// Task priorities
#define PRIORITY_DATA_AGGREGATION 2
#define PRIORITY_WIFI 1
#define PRIORITY_QUEUE_PROCESS 1

// Core assignments
#define CORE_DATA_AGGREGATION 0
#define CORE_OTHER_TASKS 1

// Other Configuration
#define HEARTBEAT_PIN 12
#define WIFI_CONNECTED_PIN 14

#endif // CONFIG_H