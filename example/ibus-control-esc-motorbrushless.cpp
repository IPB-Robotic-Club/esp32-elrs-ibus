#include <Arduino.h>
#include <IBusBM.h>
#include <ESP32Servo.h>

IBusBM IBus;       // iBUS receiver
Servo esc1;        // Right ESC on pin 14
Servo esc2;        // Left ESC on pin 27

void setup() {
  Serial.begin(115200);
  IBus.begin(Serial2, 1);     // iBUS on Serial2 RX, using timer 1

  // Initialize ESCs
  esc1.setPeriodHertz(50);    // Set standard ESC frequency
  esc1.attach(14, 1000, 2000); // ESC control on pin 14
  esc1.writeMicroseconds(1000); // Arm ESC

  esc2.setPeriodHertz(50);    // Set standard ESC frequency
  esc2.attach(27, 1000, 2000); // ESC control on pin 27
  esc2.writeMicroseconds(1000); // Arm ESC

  delay(2000);                // Wait to arm ESCs
  Serial.println("Start iBUS throttle and steering control");
}

void loop() {
  // Read throttle (CH3) and steering (CH4)
  int ch3 = IBus.readChannel(2);  // CH3 is index 2 (Throttle)
  int ch4 = IBus.readChannel(3);  // CH4 is index 3 (Steering)

  // Map CH3 value (1000-2000) to throttle percentage (0-100)
  int throttlePercent = map(ch3, 1000, 2000, 0, 100);
  throttlePercent = constrain(throttlePercent, 0, 100);

  // Map CH4 value (1000-2000) to steering percentage (-100 to 100)
  int steeringPercent = map(ch4, 1000, 2000, -100, 100);
  steeringPercent = constrain(steeringPercent, -100, 100);

  // Calculate PWM for each ESC
  int rightPwm = map(throttlePercent, 0, 100, 1000, 2000);
  int leftPwm = map(throttlePercent, 0, 100, 1000, 2000);

  // Adjust PWM for turning
  if (steeringPercent > 2) {
    // Turn Right: Reduce speed of the right ESC
    rightPwm -= map(steeringPercent, 0, 100, 0, 500);  // Reduce up to 500 µs
    rightPwm = constrain(rightPwm, 1000, 2000);
  } else if (steeringPercent < -1) {
    // Turn Left: Reduce speed of the left ESC
    leftPwm -= map(abs(steeringPercent), 0, 100, 0, 500);  // Reduce up to 500 µs
    leftPwm = constrain(leftPwm, 1000, 2000);
  }

  // Write PWM signals to ESCs
  esc1.writeMicroseconds(rightPwm);  // Right ESC (pin 14)
  esc2.writeMicroseconds(leftPwm);   // Left ESC (pin 27)

  // Debug output
  Serial.print("CH3 (Throttle): ");
  Serial.print(ch3);
  Serial.print("  Throttle: ");
  Serial.print(throttlePercent);
  Serial.print("%  CH4 (Steering): ");
  Serial.print(ch4);
  Serial.print("  Steering: ");
  Serial.print(steeringPercent);
  Serial.print("%  Right PWM: ");
  Serial.print(rightPwm);
  Serial.print("  Left PWM: ");
  Serial.println(leftPwm);

  delay(100);
}
