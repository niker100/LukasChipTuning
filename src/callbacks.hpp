
#include <main.hpp>

class Callbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic) {
        Serial.println("onWrite");
        if (pCharacteristic == pTimeCharacteristic)
        {
            std::string value = pCharacteristic->getValue();

            if (value.length() == 0)
            {
                return;
            }

            // Assuming the value is a string with the format "YYYY-MM-DD HH:MM:SS"
            struct tm tm;
            strptime(value.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
            time_t receivedTime = mktime(&tm);

            /*
            // Set the time
            struct timeval now = { .tv_sec = t };
            settimeofday(&now, NULL);
            timeSet = true;
            */

            struct timeval now = {.tv_sec = receivedTime};
            settimeofday(&now, NULL);
            timeSet = true;

            lastSetTimestamp = receivedTime;
        }

        std::string data = "Sent data: ";
        data += pCharacteristic->getValue().c_str();
        Serial.println(data.c_str());
        pStatusCharacteristic->setValue(data.c_str());
        pStatusCharacteristic->notify();

    };
};

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
    };

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");
        pServer->startAdvertising();
    };
};
