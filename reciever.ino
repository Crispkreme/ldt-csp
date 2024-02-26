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

int servo1_pin = 8;
int servo2_pin = 9;
int initial_position = 90;
int initial_position1 = 90;

struct PacketData {
  byte xAxisValue;
  byte yAxisValue;
  byte switchStatus;
} receiverData;

void setup() {
  Serial.begin(9600);
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
  servo1.write(initial_position);
  servo2.write(initial_position1);

  rotateMotor(0, 0);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening(); // Start the radio receiver
}

void loop() {
  x_pos = analogRead(x_key);
  y_pos = analogRead(y_key);

  int servo1 = 0;
  int servo2 = 0;

  // Servo control for x-axis
  if (x_pos < 300) {
    if (initial_position > 10) {
      initial_position -= 20;
      servo1.write(initial_position);
      delay(100);
    }
  }
  else if (x_pos > 700) {
    if (initial_position < 170) {
      initial_position += 20;
      servo1.write(initial_position);
      delay(100);
    }
  }

  // Servo control for y-axis
  if (y_pos < 300) {
    if (initial_position1 > 10) {
      initial_position1 -= 20;
      servo2.write(initial_position1);
      delay(100);
    }
  }
  else if (y_pos > 700) {
    if (initial_position1 < 170) {
      initial_position1 += 20;
      servo2.write(initial_position1);
      delay(100);
    }
  }
}




void setup() {
  
}

void loop() {
  
  
  if (radio.isChipConnected() && radio.available()) {
    radio.read(&receiverData, sizeof(PacketData));
    
    if (receiverData.switchStatus)
      moveCar();
    else
      logicToTurnMotors();
      
    lastRecvTime = millis();
  } else {
    unsigned long now = millis();
    if (now - lastRecvTime > SIGNAL_TIMEOUT)
      rotateMotor(0, 0); // Signal lost, stop the motors
  }
}

void moveCar() {
  int throttle = map(receiverData.yAxisValue, 0);
  int mappedXValue = map(receiverData.xAxisValue, 0);

  int motorDirection = (throttle < 0) ? -1 : 1;

  int servo1 = abs(throttle) - mappedXValue;
  int servo2 = abs(throttle) + mappedXValue;

  servo1 = constrain(servo1, 0, 255);
  servo2 = constrain(servo2, 0, 255);

  rotateMotor(servo1 * motorDirection, servo2 * motorDirection);
}

void rotateMotor(int speed1, int speed2) {
  digitalWrite(servo1_pin, speed1 >= 0 ? HIGH : LOW);
  digitalWrite(servo2_pin, speed1 < 0 ? HIGH : LOW);

  analogWrite(EnableMotor1, abs(speed1));
  analogWrite(EnableMotor2, abs(speed2));
}

void logicToTurnMotors() {
  int throttle = map(receiverData.yAxisValue, 0);
  int mappedXValue = map(receiverData.xAxisValue, 0);

  if (throttle < 0)
    mappedXValue = -mappedXValue;

  int servo1 = throttle - mappedXValue;
  int servo2 = throttle + mappedXValue;

  servo1 = constrain(servo1, -255, 255);
  servo2 = constrain(servo2, -255, 255);

  rotateMotor(motorSpeed1, motorSpeed2);
}