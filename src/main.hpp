#include <Adafruit_SSD1322.h>
#include <SPI.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define TIME_BETWEEN_ANI_FRAMES 40

BLEServer *pServer = NULL;
BLECharacteristic *pTimeCharacteristic = NULL;
BLECharacteristic *pStatusCharacteristic = NULL;
bool deviceConnected = false;
bool timeSet = false;
int autoDisconnectTicks = 0;

#define MAX_DELTAS 10
struct Delta
{
    long duration;
    int delta_seconds;
};
Delta deltas[MAX_DELTAS];
int numDeltas = 0;
time_t lastSetTimestamp = 0;

// Used for software SPI
#define OLED_CLK 18
#define OLED_MOSI 23

// Used for software or hardware SPI
#define OLED_CS 5
#define OLED_DC 19

// Used for I2C or SPI
#define OLED_RESET 21
#define OLED_ROTATION 2 // CHANGEME to 0 if your panel is mounted differently

#define SLEEP_PIN 4
#define SLEEP_OVERRIDE_PIN 2

// software SPI
Adafruit_SSD1322 display(256, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
// hardware SPI
// Adafruit_SSD1322 display(256, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS, 1000000);

time_t now;
char strftime_buf[64];
struct tm timeinfo;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TIME_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define STATUS_UUID "6841b1b3-3f7b-4b6f-9e0f-2f1b3d1b3b3b"

class Callbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic) {};
};

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer) {};

    void onDisconnect(BLEServer *pServer) {};
};
