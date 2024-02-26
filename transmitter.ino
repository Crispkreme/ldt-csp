#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int x_key = A1; // Analog input pin for x-axis
int y_key = A0; // Analog input pin for y-axis
int x_pos;
int y_pos;

const uint64_t pipeOut = 0xF9E8F0F0E1LL;

RF24 radio(8, 9);

struct PacketData 
{
  byte xAxisValue;
  byte yAxisValue;
  byte switchStatus;
} data;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  radio.stopListening(); 
  
  pinMode(x_key, INPUT);
  pinMode(y_key, INPUT);

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  data.switchStatus = HIGH;
}

void left_direction(int x_pos) // quadrant
{
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
}

void right_direction(int y_pos) // quadrant
{
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

void loop() {
  x_pos = analogRead(x_key);
  y_pos = analogRead(y_key);

  xAxisValue = left_direction(x_pos);
  yAxisValue = right_direction(y_pos);

  Serial.println("The raw value is ");
  Serial.println(xAxisValue);
  Serial.println(yAxisValue);   
  Serial.println("The mapped value is ");
  Serial.println(data.xAxisValue);
  Serial.println(data.yAxisValue);
  Serial.println(radio.isChipConnected());
  delay(500);
  
  radio.write(&data, sizeof(PacketData));
}