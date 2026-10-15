#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ===== Cấu hình =====
#define LED_PIN 48                 // LED RGB trên board (nếu không sáng, đổi thành 38)
#define BLE_NAME "Vinh_LED"        // Tên hiện trong nRF Connect

// UUID kiểu Nordic UART (app Serial Bluetooth Terminal chế độ BLE nhận sẵn)
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_UUID      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"   // điện thoại GHI lệnh vào đây
#define TX_UUID      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"   // ESP32 BÁO trạng thái ra đây

BLEServer*         pServer = nullptr;
BLECharacteristic* pTx     = nullptr;

bool deviceConnected = false;
bool oldConnected    = false;

// Màu hiện tại của LED (0-255)
uint8_t curR = 50, curG = 50, curB = 50;

void setLed(uint8_t r, uint8_t g, uint8_t b) {
  curR = r; curG = g; curB = b;
  neopixelWrite(LED_PIN, r, g, b);
}

void reply(const String &msg) {
  Serial.println(msg);
  if (deviceConnected && pTx) {
    pTx->setValue(msg.c_str());
    pTx->notify();
  }
}

// Xử lý lệnh nhận được: ON, OFF, RED, GREEN, BLUE, WHITE, hoặc "R,G,B" (vd: 255,0,128)
void xuLyLenh(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd.length() == 0) return;

  if      (cmd == "ON" || cmd == "WHITE") { setLed(50, 50, 50); reply("LED: BAT"); }
  else if (cmd == "OFF")                  { setLed(0, 0, 0);    reply("LED: TAT"); }
  else if (cmd == "RED")                  { setLed(50, 0, 0);   reply("LED: DO"); }
  else if (cmd == "GREEN")                { setLed(0, 50, 0);   reply("LED: XANH LA"); }
  else if (cmd == "BLUE")                 { setLed(0, 0, 50);   reply("LED: XANH DUONG"); }
  else {
    // Thử đọc dạng R,G,B
    int c1 = cmd.indexOf(',');
    int c2 = cmd.indexOf(',', c1 + 1);
    if (c1 > 0 && c2 > c1) {
      int r = constrain(cmd.substring(0, c1).toInt(), 0, 255);
      int g = constrain(cmd.substring(c1 + 1, c2).toInt(), 0, 255);
      int b = constrain(cmd.substring(c2 + 1).toInt(), 0, 255);
      setLed(r, g, b);
      reply("LED RGB: " + String(r) + "," + String(g) + "," + String(b));
    } else {
      reply("Lenh sai: " + cmd);
    }
  }
}

// Khi điện thoại nối / ngắt
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* s)    { deviceConnected = true; }
  void onDisconnect(BLEServer* s) { deviceConnected = false; }
};

// Khi điện thoại ghi dữ liệu vào RX
class RxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) {
    auto value = c->getValue();               // tương thích cả core 2.x và 3.x
    String cmd = String(value.c_str());
    Serial.println("Nhan: " + cmd);
    xuLyLenh(cmd);
  }
};

void setup() {
  Serial.begin(115200);
  setLed(0, 0, 50);                           // xanh dương: đang chờ kết nối BLE

  BLEDevice::init(BLE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  // TX: ESP32 gửi thông báo cho điện thoại
  pTx = pService->createCharacteristic(TX_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  pTx->addDescriptor(new BLE2902());

  // RX: điện thoại ghi lệnh xuống ESP32
  BLECharacteristic* pRx = pService->createCharacteristic(
      RX_UUID,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pRx->setCallbacks(new RxCallbacks());

  pService->start();

  BLEAdvertising* pAdv = BLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("Dang phat BLE voi ten: " BLE_NAME);
}

void loop() {
  // Vừa có điện thoại nối vào
  if (deviceConnected && !oldConnected) {
    oldConnected = true;
    setLed(0, 50, 0);                         // xanh lá: đã kết nối
    Serial.println("Da ket noi BLE");
  }

  // Vừa mất kết nối -> phát quảng bá lại để nối lại được
  if (!deviceConnected && oldConnected) {
    oldConnected = false;
    delay(300);
    BLEDevice::startAdvertising();
    setLed(0, 0, 50);                         // xanh dương: chờ kết nối lại
    Serial.println("Mat ket noi, dang phat lai...");
  }

  delay(20);
}
