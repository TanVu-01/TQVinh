const int ledPin = 9;

unsigned long lastTime = 0;
bool ledState = false;

int interval = 1000;       // Bắt đầu: 1 giây
const int minInterval = 50; // Nhanh nhất: 50 ms

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long now = millis();

  if (now - lastTime >= interval) {
    lastTime = now;

    // Đảo trạng thái LED
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    // Mỗi lần đổi trạng thái thì nhanh hơn
    if (interval > minInterval) {
      interval -= 20;
    }
  }
}