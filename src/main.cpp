#include <ani.hpp>

void setup()
{
  Serial.begin(115200);

  // pinMode(SLEEP_PIN, INPUT_PULLDOWN);
  // pinMode(SLEEP_OVERRIDE_PIN, OUTPUT);
  // digitalWrite(SLEEP_OVERRIDE_PIN, HIGH);

  // while (! Serial) delay(100);
  BLEDevice::init("Geile Socke");
  BLEDevice::setMTU(512);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTimeCharacteristic = pService->createCharacteristic(
      TIME_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  pStatusCharacteristic = pService->createCharacteristic(
      STATUS_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY);

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  pStatusCharacteristic->setValue("Disconnected");
  pTimeCharacteristic->setCallbacks(new Callbacks());
  pStatusCharacteristic->setCallbacks(new Callbacks());

  if (!display.begin(0x3D))
  {
    Serial.println("Unable to initialize OLED");
    while (1)
      yield();
  }

  Serial.println("SSD1322 initialized");
  // Note: change me if your panel orientation is different than mine
  display.setRotation(OLED_ROTATION);

  Serial.println("SSD1322 clear");
  display.clearDisplay();
  display.display();

  for (auto img : ani_array)
  {
    display.drawBitmap(0, 0, img, 256, 64, SSD1322_WHITE);
    display.display();
    delay(TIME_BETWEEN_ANI_FRAMES);
    display.clearDisplay();
  }

  delay(TIME_BETWEEN_ANI_FRAMES * 3);

  display.drawBitmap(96, 0, epd_bitmap_prneracng, 64, 64, SSD1322_WHITE);
  display.display();
  delay(5000);

  time(&now);
  setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
  tzset();

  // delay(2000);

  // draw a bitmap icon and 'animate' movement
  // testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}

void loop()
{
  /*
  if(digitalRead(SLEEP_PIN) == !HIGH) {
    Serial.println("Going to sleep");
    display.clearDisplay();
    display.display();
    //esp_sleep_enable_ext0_wakeup((gpio_num_t)SLEEP_PIN, HIGH);
    //esp_deep_sleep_start();
  }
  */

  if (deviceConnected && !timeSet)
  {
    /* connection check now done in react-native App */
    /*
    Serial.println("Device connected");
    pStatusCharacteristic->setValue("ESP connected");
    pStatusCharacteristic->notify();
    */
    autoDisconnectTicks = 0;
  }

  if (!deviceConnected && timeSet)
  {
    Serial.println("Device disconnected");
    timeSet = false;
  }

  if (deviceConnected && timeSet && (autoDisconnectTicks > 500))
  {
    pStatusCharacteristic->setValue("ESP auto-disconnect");
    pStatusCharacteristic->notify();
    pServer->disconnect(pServer->getConnId());
    autoDisconnectTicks = 0;
  }

  time(&now);

  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1322_WHITE);
  display.setCursor(0, 0);
  display.println(strftime_buf);
  display.display();

  autoDisconnectTicks++;
  delay(10);
}