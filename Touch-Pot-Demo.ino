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
}

void loop() {
  delay(50);  // Read ~20 times/second

  // Demonstrate direct access to Touch Potentiometer value
  curValue = ReadTpValue(); // faster I2C access than register read
  if (curValue != prevValue) {
    analogWrite(13, curValue);
    Serial.println(curValue);
    prevValue = curValue;
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
