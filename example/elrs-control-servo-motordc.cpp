#include <HardwareSerial.h>
#include <ESP32Servo.h>

#define CRSF_SERIAL_PORT Serial2  // Using Serial2 on ESP32
#define CRSF_BAUD_RATE 420000     // CRSF baud rate
#define RX_PIN 13                 // Receiver TX connected to ESP32 RX

// Servo
Servo servo1;
Servo servo2;

int servoPin1 = 18;  // First servo on GPIO 18
int servoPin2 = 19;  // Second servo on GPIO 19

// Motor A pins
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 

// Motor B pins (for turning left/right)
int motor2Pin1 = 25;
int motor2Pin2 = 33;
int enable2Pin = 32;

#define CRSF_MAX_PACKET_SIZE 64
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16

uint16_t rcChannels[16];  // Channel data
int speed = 0;            // Speed value mapped from CH8

void setupMotors() {
  pinMode(enable1Pin, OUTPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  pinMode(enable2Pin, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
}

void moveForward() {
  Serial.println("Moving Forward");
  analogWrite(enable1Pin, speed);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  analogWrite(enable2Pin, speed);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void moveBackward() {
  Serial.println("Moving Backward");
  analogWrite(enable1Pin, speed);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  analogWrite(enable2Pin, speed);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void turnLeft() {
  Serial.println("Turning Left");
  analogWrite(enable1Pin, speed);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  analogWrite(enable2Pin, speed);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void turnRight() {
  Serial.println("Turning Right");
  analogWrite(enable1Pin, speed);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  analogWrite(enable2Pin, speed);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void stopMotors() {
  Serial.println("Stopping");
  analogWrite(enable1Pin, 0);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);

  analogWrite(enable2Pin, 0);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

void processCRSFPacket(uint8_t *packet, uint8_t length) {
  uint8_t packetType = packet[2];

  if (packetType == CRSF_FRAMETYPE_RC_CHANNELS_PACKED) {
    rcChannels[0] = ((packet[3] | packet[4] << 8) & 0x07FF);  // CH1
    rcChannels[1] = ((packet[4] >> 3 | packet[5] << 5) & 0x07FF); // CH2
    rcChannels[2] = ((packet[5] >> 6 | packet[6] << 2 | packet[7] << 10) & 0x07FF); // CH3
    rcChannels[3] = ((packet[7] >> 1 | packet[8] << 7) & 0x07FF); // CH4
    rcChannels[4] = ((packet[8] >> 4 | packet[9] << 4) & 0x07FF); // CH5
    rcChannels[5] = ((packet[9] >> 7 | packet[10] << 1 | packet[11] << 9) & 0x07FF); // CH6
    rcChannels[6] = ((packet[11] >> 2 | packet[12] << 6) & 0x07FF); // CH7
    rcChannels[7] = ((packet[12] >> 5 | packet[13] << 3) & 0x07FF); // CH8

    // Process control
    int pitch = rcChannels[1];  // CH2: Move forward/back
    int roll  = rcChannels[0];  // CH1: Turn
    int rawSpeed = rcChannels[7];  // CH8: Speed control
    rawSpeed = constrain(rawSpeed, 300, 1600);
    speed = map(rawSpeed, 300, 1600, 0, 255);

    if (pitch > 1010) {
      moveForward();
    } else if (pitch < 900) {
      moveBackward();
    } else {
      if (roll > 1000) {
        turnRight();
      } else if (roll < 900) {
        turnLeft();
      } else {
        stopMotors();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  CRSF_SERIAL_PORT.begin(CRSF_BAUD_RATE, SERIAL_8N1, RX_PIN);
  setupMotors();
  Serial.println("CRSF Receiver Initialized.");

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
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
      packetIndex = 0;
    }

    if (packetIndex >= CRSF_MAX_PACKET_SIZE) {
      packetIndex = 0;
    }
  }
}
