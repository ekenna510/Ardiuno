#ifndef sonar_h
#define sonar_h

#include <Arduino.h>
#include <Wire.h>


#define ALLDEVICES uint8_t(0)
#define RANGE_IN uint8_t(0x50)
#define RANGE_CM uint8_t(0x51)
#define RANGE_US uint8_t(0x52)

uint8_t srf08_ping(uint8_t device, uint8_t cmd);
uint8_t srf08_range(uint8_t device, uint8_t echo, short * range);
uint8_t srf08_Version(uint8_t device, uint8_t  * Version);
uint8_t srf08_SetMaxrange(uint8_t device, uint8_t range, uint8_t  gain);
void srf08_ChangeAddress(uint8_t oldAddress, uint8_t newAddress);

#endif
