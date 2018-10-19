/*
 * Simple Touch Potentiometer Example with Arduino
 *
 * Reads the pot value and controls the brightness of the Arduino LED on
 * Digital Pin 13.  Also logs new values to the serial port.  Utilizes
 * both the direct and indirect command interface forms.
 *
 * Assumes Touch Pot is at I2C Address 8
 *
 * Released into the public domain by Dan Julio.  This software is supplied on an as-is 
 * basis and no warranty as to its suitability for any particular purpose is either made
 * or implied.  danjuliodesigns, LLC. will not accept any claim for damages howsoever
 * arising as a result of use or failure of this software.
 * 
 * Connections:
 * Arduino pin A4 is the I2C SDA pin on the touchpot.
 * Arduino pin A5 is the I2C SCL pin on the touchpot.
 * GND on the touchpot goes to GND on the arduino.
 * VIN on the touchpot goes to +5v on the Arduino
 */
#include "Wire.h"

int i2cAddr = 8; // Direct access at i2cAddr, indirect registers at i2cAddr+1

uint8_t prevValue;
uint8_t curValue;

//Large 7-Segment stuff...
//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClock = 6;
byte segmentLatch = 5;
byte segmentData = 7;
byte addButton = 10;
byte subtractButton = 11;

int counter = 0;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(13, OUTPUT);

  // Demonstrate access to Touch Potentiometer registers
  WriteTpReg(1, 128); // set to 50% by writing to register 1
  curValue = ReadTpReg(1); // read back value just set

  // Set Arduino LED PWM to match
  analogWrite(13, curValue);
  prevValue = curValue;

  //Large 7-Segment Setup...
  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);
  pinMode(addButton,INPUT_PULLUP);
  pinMode(subtractButton,INPUT_PULLUP);

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, LOW);
  digitalWrite(segmentLatch, LOW);

}

void loop() {
  delay(50);  // Read ~20 times/second

  // Demonstrate direct access to Touch Potentiometer value
  curValue = ReadTpValue(); // faster I2C access than register read
  if (curValue != prevValue) {
    analogWrite(13, curValue);
    Serial.print("curValue = ");
    Serial.print(curValue);
    prevValue = curValue;
    counter = map(curValue, 0, 255, 0, 9);
    Serial.print("  counter = ");
    Serial.println(counter);
    postNumber(counter, false); //Show decimal
    digitalWrite(segmentLatch, LOW);
    digitalWrite(segmentLatch, HIGH);
  }
  
  
  
}

// Write a Touch Potentiometer register
void WriteTpReg(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(i2cAddr+1);
  Wire.write('W');
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
}

// Get the Touch Potentiometer value
uint8_t ReadTpValue() {
  Wire.requestFrom(i2cAddr, 1);
  if (Wire.available()) {
    return Wire.read();
  } else {
    return 0;
  }
}

// Read a Touch Potentiometer register
uint8_t ReadTpReg(uint8_t addr) {
  Wire.beginTransmission(i2cAddr+1);
  Wire.write('R');
  Wire.write(addr);
  Wire.endTransmission();

  Wire.requestFrom(i2cAddr+1, 1);
  if (Wire.available()) {
    return Wire.read();
  } else {
    return 0;
  }
}


//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(float value)
{
  int number = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = number % 10;

    postNumber(remainder, false);

    number /= 10;
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}

