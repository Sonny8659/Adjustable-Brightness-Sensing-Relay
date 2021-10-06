#include <Arduino.h>
#include <SSD1306.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

SSD1306   display(0x3c, 4, 5, GEOMETRY_128_32);
int lux;
int isrelayon = 0;

const int     RELAY = 15;
const int     pulseA = 12;
const int     pulseB = 13;
const int     pushSW = 2;
volatile int  lastEncoded = 0;
volatile long encoderValue = 0;


IRAM_ATTR void handleRotary() {
  //Never put nay long instruction
  int MSB = digitalRead(pulseA);
  int LSB = digitalRead(pulseB);

  int encoded = (MSB << 1)|LSB;
  int sum = (lastEncoded << 2) | encoded;
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
  lastEncoded = encoded;
  if(encoderValue > 255)      encoderValue = 255;
  else if (encoderValue < 0)  encoderValue = 0;
}

IRAM_ATTR void buttonClicked() {
  Serial.println("Pushed");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pushSW, INPUT_PULLUP);
  pinMode(pulseA, INPUT_PULLUP);
  pinMode(pulseB, INPUT_PULLUP);
  attachInterrupt(pushSW, buttonClicked, FALLING);
  attachInterrupt(pulseA, handleRotary,CHANGE);
  attachInterrupt(pulseB, handleRotary, CHANGE);

  pinMode(RELAY, OUTPUT);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 10, "Hello World");
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(encoderValue);
  lux = analogRead(0);
  display.init();
  display.flipScreenVertically();
  display.drawString(10, 10, (String)encoderValue);
  display.display();

  if(lux < encoderValue) isrelayon = 1;
  else isrelayon = 0;

  digitalWrite(RELAY, isrelayon);
  delay(1000);
}