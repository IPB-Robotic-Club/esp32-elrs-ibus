#include <HardwareSerial.h>

#define CRSF_SERIAL_PORT Serial2  // Using Serial2 on the ESP32
#define CRSF_BAUD_RATE 420000     // CRSF baud rate (400,000 bps)
#define RX_PIN 13                 // Receiver TX connected to ESP32 RX (GPIO17)

#define CRSF_MAX_PACKET_SIZE 64
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16

uint16_t rcChannels[16];  // Channel data

void processCRSFPacket(uint8_t *packet, uint8_t length) {
  uint8_t packetType = packet[2];

  if (packetType == CRSF_FRAMETYPE_RC_CHANNELS_PACKED) {
    // Extract 11-bit channel data
    rcChannels[0] = ((packet[3] | packet[4] << 8) & 0x07FF);
    rcChannels[1] = ((packet[4] >> 3 | packet[5] << 5) & 0x07FF);
    rcChannels[2] = ((packet[5] >> 6 | packet[6] << 2 | packet[7] << 10) & 0x07FF);
    rcChannels[3] = ((packet[7] >> 1 | packet[8] << 7) & 0x07FF);
    rcChannels[4] = ((packet[8] >> 4 | packet[9] << 4) & 0x07FF);
    rcChannels[5] = ((packet[9] >> 7 | packet[10] << 1 | packet[11] << 9) & 0x07FF);
    rcChannels[6] = ((packet[11] >> 2 | packet[12] << 6) & 0x07FF);
    rcChannels[7] = ((packet[12] >> 5 | packet[13] << 3) & 0x07FF);

    // Print channel values for CH1 to CH4
    Serial.print("CH1: ");
    Serial.print(rcChannels[0]);
    Serial.print(" CH2: ");
    Serial.print(rcChannels[1]);
    Serial.print(" CH3: ");
    Serial.print(rcChannels[2]);
    Serial.print(" CH4: ");
    Serial.print(rcChannels[3]);
    Serial.print(" CH6: ");
    Serial.print(rcChannels[5]);
    Serial.print(" CH5: ");
    Serial.println(rcChannels[4]);
  }
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  CRSF_SERIAL_PORT.begin(CRSF_BAUD_RATE, SERIAL_8N1, RX_PIN);

  Serial.println("CRSF Receiver Initialized.");
}

void loop() {
  static uint8_t packet[CRSF_MAX_PACKET_SIZE];
  static uint8_t packetIndex = 0;

  while (CRSF_SERIAL_PORT.available()) {
    uint8_t incomingByte = CRSF_SERIAL_PORT.read();

    if (packetIndex == 0 && incomingByte != 0xEE && incomingByte != 0xC8) {
      continue;  // Skip invalid bytes
    }

    packet[packetIndex++] = incomingByte;

    if (packetIndex >= 2 && packetIndex == packet[1] + 2) {
      processCRSFPacket(packet, packetIndex);
      packetIndex = 0;  // Reset for next packet
    }

    if (packetIndex >= CRSF_MAX_PACKET_SIZE) {
      packetIndex = 0;  // Prevent buffer overflow
    }
  }
}
