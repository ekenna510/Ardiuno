// I2C SRF10 or SRF08 Devantech Ultrasonic Ranger Finder
// by Nicholas Zambetti <http://www.zambetti.com>
// and James Tichenor <http://www.jamestichenor.net>
// Demonstrates use of the Wire library reading data from the
// Devantech Utrasonic Rangers SFR08 and SFR10
// Created 29 April 2006

// Modified EFK 04 July 2021


//#include <Wire.h>
#include "sonar.h"
byte SonarAddress1=114;
byte SonarAddress2=112;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() {
  byte result;
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(115200);         
  pinMode(LED_BUILTIN, OUTPUT);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
 
  result=srf08_SetMaxrange(SonarAddress1, 80, 10);
  Serial.println(result);   
  result=srf08_SetMaxrange(SonarAddress2, 80, 10);
    Serial.println(result);   
}

//extern const struct param __ATTR_PROGMEM__ param_list[];



short reading = 0;
bool runpings=false;

void loop() {
  if (stringComplete) {
    Serial.println(inputString);
    if (inputString=="Y\n")
      {
        runpings=true;
      }
    else
    {
      runpings=false;
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }  
if (runpings)
  {
  byte result=0;
  result=srf08_ping( ALLDEVICES,RANGE_US);
  if (result<0 )
    {
    Serial.print("ping fail to end transmission");
    Serial.println(result);   // print the reading 
    }
  
    // step 2: wait for readings to happen
    delay(70);                   // datasheet suggests at least 65 milliseconds
  
  result=srf08_range(SonarAddress1, 0, &reading); 
  if (result<0 )
    {
    Serial.print("range fail to end transmission");
    Serial.println(result);
    }
  else
    {
          Serial.print("sonar1 ");
          Serial.println(reading);   // print the reading
    }
  result=srf08_range(SonarAddress2, 0, &reading); 
  if (result<0 )
    {
    Serial.println("range fail to end transmission");
    Serial.println(result);
    }
  else
    {
          Serial.print("sonar2 ");
          Serial.println(reading);   // print the reading    
    }
  
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    
    delay(30);  // wait a bit since people have to read the output :)
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
/*


*/
