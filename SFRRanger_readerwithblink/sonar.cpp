#include <Arduino.h>

#include "sonar.h"

// I2C SRF10 or SRF08 Devantech Ultrasonic Ranger Finder
// by Nicholas Zambetti <http://www.zambetti.com>
// and James Tichenor <http://www.jamestichenor.net>
// Demonstrates use of the Wire library reading data from the
// Devantech Utrasonic Rangers SFR08 and SFR10
// Created 29 April 2006

// Modified EFK 04 July 2021


/*
 * srf08_ping - initiate a ping to the SRF08 module.  Returns
 * immidiately, the caller must then wait the appropriate amount of
 * time before reading the echo results.  Use 'srf08_range()' to read
 * the echo results.  'device' is the 7 bit I2C address of the SRF08
 * module.  'cmd' is the command to execute.
 *
 * Returns 0 on success, or -1 if an I2C error occured.  */
byte srf08_ping(byte device, byte cmd)
{
byte returnvalue;
returnvalue=0;

 // step 1: instruct sensor to read echoes
  Wire.beginTransmission(device); // transmit to device #112 (0x70)
  // the address specified in the datasheet is 224 (0xE0)
  // but i2c adressing uses the high 7 bits so it's 112
  returnvalue=Wire.write((byte)0x00);      // sets register pointer to the command register (0x00)
  if (returnvalue==0)
    {
    Serial.println("ping fail to write register");
    returnvalue=-1;
    }  
  if (returnvalue > -1)
    {
      
    // command sensor to measure in "inches" (0x50)
    // use 0x51 for centimeters
    // use 0x52 for ping microseconds
    returnvalue =Wire.write(cmd);  
    if (returnvalue==0)
    {
    Serial.println("ping fail to write command");
    returnvalue=-2;
    }     
  }
  returnvalue=Wire.endTransmission(true);      // stop transmitting
  if (returnvalue > 0)
    {
          Serial.print("ping fail to end transmission ");
          Serial.println(returnvalue);
    }
  return returnvalue;
}



/*
 * srf08_range - Read echo results from the SRF08 module.
 *
 * 'device' should be the 7 bit I2C address of the unit.  'echo'
 * should contain which echo to read, 0=first echo, 1=second echo,
 * etc.  'range' should point to a 2 byte location to hold the 16 bit
 * echo result.
 *
 * Returns 0 on success, or -1 if an I2C error occured.
 */
byte srf08_range(byte device, byte echo, short * range)
{
  byte vh, vl;
  byte addr;
byte returnvalue;
returnvalue=0;

  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(device); // transmit to device #112
  addr = echo*2 + 2;
  returnvalue=Wire.write(addr);      // sets register pointer to echo #1 register (0x02)
  if (returnvalue==0)
    {
    Serial.println("range fail to write register");
    returnvalue=-1;
    }  
    
  returnvalue=Wire.endTransmission(false);      // stop transmitting
    if (returnvalue>0)
    {
    Serial.println("fail to end transmission ");
    Serial.println(returnvalue);
    returnvalue=-2;
    }  
  // step 4: request reading from sensor
  returnvalue=Wire.requestFrom(device, (byte)2);    // request 2 bytes from slave device #112
  if (returnvalue==0)
    {
    Serial.println("range fail to read from");
    returnvalue=-3;
    }  
  // step 5: receive reading from sensor
  if (2 <= Wire.available()) { // if two bytes were received
    vh = Wire.read();  // receive high byte (overwrites previous reading)
    vl = Wire.read(); // receive low byte as lower 8 bits

    *range = (vh << 8) | vl; // shift high byte to be high 8 bits
  }
  returnvalue=Wire.endTransmission(true);      // stop transmitting
  if (returnvalue > 0)
    {
          Serial.println("range fail to end transmission %d");
          Serial.println(returnvalue);
    }
  return returnvalue;  


}

byte srf08_Version(byte device, byte  * Version)
{
byte returnvalue;
returnvalue=0;  
  byte vh;
  byte addr;
  Wire.beginTransmission(device); // transmit to device #112


  addr = 0;
  returnvalue=Wire.write(addr);      // sets register pointer to echo #1 register (0x02)
  if (returnvalue==0)
    {
    Serial.println("version fail to write register");
    returnvalue=-1;
    }  

   returnvalue=Wire.endTransmission(false);      // stop transmitting
    if (returnvalue>0)
    {
    Serial.println("version fail to end transmission %d");
    Serial.println(returnvalue);
    returnvalue=-2;
    }  

 // step 4: request reading from sensor
  returnvalue=Wire.requestFrom(device, (byte)1);    // request 1 bytes from slave device 
  if (returnvalue==0)
    {
    Serial.println("version fail to read from");
    returnvalue=-3;
    }  

 if (1 <= Wire.available()) { // if two bytes were received
    vh = Wire.read();  // receive high byte (overwrites previous reading)
  
    *Version = vh; 
  }
  returnvalue=Wire.endTransmission(true);      // stop transmitting
  if (returnvalue > 0)
    {
          Serial.println("ping fail to end transmission %d");
          Serial.println(returnvalue);
    }
  *Version = vh;

  return returnvalue;
}

byte srf08_SetMaxrange(byte device, byte range, byte  gain)
{
byte returnvalue;
returnvalue=0; 
Wire.beginTransmission(device); // transmit to device #112


byte addr;
// range register address is 2 
addr = 2; 
returnvalue=Wire.write(addr);      // sets register pointer to echo #1 register (0x02)
if (returnvalue==0)
  {
  Serial.println("Set Range fail to write register");
  returnvalue=-1;
  }  

returnvalue =Wire.write(range);  
if (returnvalue==0)
  {
  Serial.println("Set Range fail to write command");
  returnvalue=-2;
  }     
  
returnvalue=Wire.endTransmission(false);      // stop transmitting
if (returnvalue>0)
  {
  Serial.println("Set Range fail to end transmission %d");
  Serial.println(returnvalue);
  returnvalue=-3;
  }  

// gain register address is 1 
addr = 1;

if (gain > 31)
  {
  gain = 31;
  }
Wire.beginTransmission(device); // transmit to device #112

returnvalue=Wire.write(addr);      // sets register pointer to echo #1 register (0x02)
if (returnvalue==0)
  {
  Serial.println("Set Gain fail to write register");
  returnvalue=-4;
  }  

returnvalue =Wire.write(gain);  
if (returnvalue==0)
  {
  Serial.println("Set Gain fail to write command");
  returnvalue=-5;
  }     
  
returnvalue=Wire.endTransmission(true);      // stop transmitting
if (returnvalue>0)
  {
  Serial.println("Set Gain fail to end transmission %d");
  Serial.println(returnvalue);
  returnvalue=-6;
  }  


return returnvalue;


}


// The following code changes the address of a Devantech Ultrasonic Range Finder (SRF10 or SRF08)
// usage: changeAddress(0x70, 0xE6);

void srf08_ChangeAddress(byte oldAddress, byte newAddress)
{
  Wire.beginTransmission(oldAddress);
  Wire.write(byte(0x00));
  Wire.write(byte(0xA0));
  Wire.endTransmission();

  Wire.beginTransmission(oldAddress);
  Wire.write(byte(0x00));
  Wire.write(byte(0xAA));
  Wire.endTransmission();

  Wire.beginTransmission(oldAddress);
  Wire.write(byte(0x00));
  Wire.write(byte(0xA5));
  Wire.endTransmission();

  Wire.beginTransmission(oldAddress);
  Wire.write(byte(0x00));
  Wire.write(newAddress);
  Wire.endTransmission();
}
