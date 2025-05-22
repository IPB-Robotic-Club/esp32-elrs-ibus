#include <IBusBM.h>
IBusBM ibus;

void setup() {
  Serial.begin(115200);             // Debugging
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  // RX = GPIO16, TX = GPIO17 (TX not used)
  ibus.begin(Serial2);
}

void loop() {
  ibus.read();

  for (int i = 0; i < 6; i++) {  // Change 6 to however many channels you use
    int ch = ibus.readChannel(i);
    Serial.print("CH");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(ch);
    Serial.print("  ");
  }
  Serial.println();
  delay(100);
}
