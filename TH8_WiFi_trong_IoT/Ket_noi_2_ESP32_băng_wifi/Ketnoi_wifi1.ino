#include <WiFi.h>

const char* ssid     = "ESP32S3_AP";
const char* password = "12345678";

#define LED_PIN 48          // LED RGB trên board (nếu không sáng, đổi thành 38)

WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.print("IP cua Server: ");
  Serial.println(WiFi.softAPIP());          // 192.168.4.1

  server.begin();
  neopixelWrite(LED_PIN, 50, 0, 0);         // đỏ: chưa có Client
}

void loop() {
  // 1. LED báo trạng thái WiFi
  int soClient = WiFi.softAPgetStationNum();
  if (soClient > 0) neopixelWrite(LED_PIN, 0, 50, 0);   // xanh lá: có Client
  else              neopixelWrite(LED_PIN, 50, 0, 0);   // đỏ: chưa có Client

  // 2. In trạng thái mỗi 2 giây
  static unsigned long t = 0;
  if (millis() - t > 2000) {
    t = millis();
    Serial.print("So thiet bi dang ket noi WiFi: ");
    Serial.println(soClient);
  }

  // 3. Nhận kết nối TCP mới
  if (!client || !client.connected()) {
    WiFiClient newClient = server.available();
    if (newClient) {
      client = newClient;
      Serial.println("Client da ket noi TCP");
    }
  }

  // 4. Nhận dữ liệu và phản hồi
  if (client && client.connected() && client.available()) {
    String msg = client.readStringUntil('\n');
    msg.trim();
    Serial.println("Nhan: " + msg);
    client.println("Server da nhan: " + msg);
  }
}