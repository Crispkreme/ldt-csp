#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xF9E8F0F0E1LL;

RF24 radio(8, 9);

int x_key = A1; // Analog input pin for x-axis
int y_key = A0; // Analog input pin for y-axis
int x_pos;
int y_pos;

struct PacketData {
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

  // Initialize switch status
  data.switchStatus = HIGH;
}

void loop() {
  // Read analog values from joystick
  x_pos = analogRead(x_key);
  y_pos = analogRead(y_key);

  // Map joystick values to byte range
  data.xAxisValue = map(x_pos, 0, 1023, 0, 255);
  data.yAxisValue = map(y_pos, 0, 1023, 0, 255);

  // Send data over radio
  radio.write(&data, sizeof(PacketData));

  // Debugging information
  Serial.print("X-axis value: ");
  Serial.println(data.xAxisValue);
  Serial.print("Y-axis value: ");
  Serial.println(data.yAxisValue);

  // Delay to avoid flooding the radio
  delay(100);
}
