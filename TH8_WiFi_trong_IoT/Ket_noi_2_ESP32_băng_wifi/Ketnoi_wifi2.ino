#include <WiFi.h>

const char* ssid     = "ESP32S3_AP";
const char* password = "12345678";
const char* host     = "192.168.4.1";   // IP của Server
const int   port     = 80;

#define LED_PIN 48          // LED RGB trên board (nếu không sáng, đổi thành 38)

WiFiClient client;
int dem = 0;
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  neopixelWrite(LED_PIN, 50, 0, 0);         // đỏ: chưa nối WiFi

  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi WiFi");
  Serial.print("IP cua Client: ");
  Serial.println(WiFi.localIP());           // 192.168.4.x
  Serial.print("Do manh song (RSSI): ");
  Serial.println(WiFi.RSSI());              // càng gần 0 càng mạnh
  neopixelWrite(LED_PIN, 0, 0, 50);         // xanh dương: đã nối WiFi
}

void loop() {
  // 1. Mất WiFi thì báo đỏ và nối lại
  if (WiFi.status() != WL_CONNECTED) {
    neopixelWrite(LED_PIN, 50, 0, 0);
    Serial.println("Mat WiFi, dang noi lai...");
    WiFi.reconnect();
    delay(2000);
    return;
  }

  // 2. Nối TCP tới Server
  if (!client.connected()) {
    neopixelWrite(LED_PIN, 0, 0, 50);       // xanh dương: có WiFi, chưa nối Server
    Serial.println("Dang ket noi toi Server...");
    if (!client.connect(host, port)) {
      delay(2000);
      return;
    }
    Serial.println("Da ket noi toi Server");
  }
  neopixelWrite(LED_PIN, 0, 50, 0);         // xanh lá: đã nối Server

  // 3. Gửi dữ liệu mỗi 2 giây
  if (millis() - lastSend > 2000) {
    lastSend = millis();
    client.println("Xin chao lan " + String(dem++));
  }

  // 4. Nhận phản hồi
  if (client.available()) {
    Serial.println("Nhan: " + client.readStringUntil('\n'));
  }
}