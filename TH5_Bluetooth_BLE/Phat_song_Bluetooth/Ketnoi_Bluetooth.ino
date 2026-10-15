#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

void setup() {
  Serial.begin(115200);

  BLEDevice::init("Vinh");                 // Tên hiển thị khi quét
  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);
  service->start();

  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setScanResponse(true);
  BLEDevice::startAdvertising();           // Bắt đầu phát sóng

  Serial.println("Dang phat BLE: Vinh");
}

void loop() {
  delay(1000);
}