#include <main.hpp>

void Callbacks::onWrite(BLECharacteristic *pCharacteristic)
{
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

        time_t internalTime;
        time(&internalTime);
        int delta = difftime(receivedTime, internalTime);

        // Set the time
        struct timeval now = {.tv_sec = receivedTime};
        settimeofday(&now, NULL);
        timeSet = true;

        // Ignore too big or small deltas (i.e. more than 1 hour and less than 5 seconds)
        if (abs(delta) > 3600 || abs(delta) < 5)
        {
            return;
        }

        // Store delta
        if (numDeltas <= MAX_DELTAS)
        {
            deltas[numDeltas++] = {receivedTime - lastSetTimestamp, delta};
            numDeltas++;
        }
        else
        {
            // If reached maximum, overwrite oldest delta
            for (int i = 1; i < MAX_DELTAS; i++)
            {
                deltas[i - 1] = deltas[i];
            }
            deltas[MAX_DELTAS - 1] = {receivedTime - lastSetTimestamp, delta};
        }

        lastSetTimestamp = receivedTime;
    }

    std::string data = "Sent data: ";
    data += pCharacteristic->getValue().c_str();
    Serial.println(data.c_str());
    pStatusCharacteristic->setValue(data.c_str());
    pStatusCharacteristic->notify();
}

void ServerCallbacks::onConnect(BLEServer *pServer)
{
    deviceConnected = true;
};

void ServerCallbacks::onDisconnect(BLEServer *pServer)
{
    deviceConnected = false;
    pServer->startAdvertising();
}
