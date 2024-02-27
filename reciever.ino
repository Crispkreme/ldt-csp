#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

constexpr unsigned long SIGNAL_TIMEOUT = 500; // Signal timeout in milliseconds

const uint64_t pipeIn = 0xF9E8F0F0E1LL;
RF24 radio(8, 9);
unsigned long lastRecvTime = 0;

Servo servo1;
Servo servo2;

int initial_position = 90;
int initial_position1 = 90;
const int motorPin = 10;

struct PacketData {
  byte xAxisValue;
  byte yAxisValue;
  byte switchStatus;
} receiverData;

void setup() {
  Serial.begin(9600);

  servo1.attach(3); // Servo 1 pin
  servo2.attach(5); // Servo 2 pin
  servo1.write(initial_position);
  servo2.write(initial_position1);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening(); // Start the radio receiver

  pinMode(motorPin, OUTPUT); // Set motor pin as output
}

void loop() {
  if (radio.available()) {
    radio.read(&receiverData, sizeof(PacketData));

    if (receiverData.switchStatus)
      moveCar();
    else
      logicToTurnMotors();

    lastRecvTime = millis();
  } else {
    unsigned long now = millis();
    if (now - lastRecvTime > SIGNAL_TIMEOUT)
      rotateMotor(0); // Signal lost, stop the motors
  }
}

void moveCar() {
  int throttle = map(receiverData.yAxisValue, 0, 255, -255, 255);
  int mappedXValue = map(receiverData.xAxisValue, 0, 255, -255, 255);

  int motorSpeed1 = throttle - mappedXValue;
  int motorSpeed2 = throttle + mappedXValue;

  motorSpeed1 = constrain(motorSpeed1, -255, 255);
  motorSpeed2 = constrain(motorSpeed2, -255, 255);

  rotateMotor(motorSpeed1, motorSpeed2);
}

void rotateMotor(int speed1) {
  // Set the direction of the motor rotation based on the sign of speed
  if (speed1 >= 0) {
    digitalWrite(motorPin, HIGH); // Set one direction
  } else {
    digitalWrite(motorPin, LOW); // Set opposite direction
    speed1 = -speed1; // Make speed positive for PWM
  }
  
  // Set the speed of the motor using PWM
  int pwm = map(speed1, 0, 255, 0, 255); // Map speed to PWM range
  analogWrite(motorPin, pwm); // Apply PWM to control speed
}

void logicToTurnMotors() {
  int throttle = map(receiverData.yAxisValue, 0, 255, -255, 255);
  int mappedXValue = map(receiverData.xAxisValue, 0, 255, -255, 255);

  if (throttle < 0)
    mappedXValue = -mappedXValue;

  int servo1Value = throttle - mappedXValue;
  int servo2Value = throttle + mappedXValue;

  servo1Value = constrain(servo1Value, -255, 255);
  servo2Value = constrain(servo2Value, -255, 255);

  servo1.writeMicroseconds(map(servo1Value, -255, 255, 1000, 2000));
  servo2.writeMicroseconds(map(servo2Value, -255, 255, 1000, 2000));
}
