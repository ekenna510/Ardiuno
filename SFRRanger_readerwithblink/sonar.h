#ifndef sonar_h
#define sonar_h


#include <Wire.h>

//SonarAddress[5];
//Sonar[5];
//SonarCount=2;

#define ALLDEVICES byte(0)
#define RANGE_IN byte(0x50)
#define RANGE_CM byte(0x51)
#define RANGE_US byte(0x52)

byte srf08_ping(byte device, byte cmd);
byte srf08_range(byte device, byte echo, short * range);
byte srf08_Version(byte device, byte  * Version);
byte srf08_SetMaxrange(byte device, byte range, byte  gain);
void srf08_ChangeAddress(byte oldAddress, byte newAddress);

#endif
