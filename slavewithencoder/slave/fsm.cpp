#include "fsm.h"
#include "sonar.h"
#include <stdio.h>


const char s_output[]   = "$03%6lx%6lx%6lx%3x%3x%c"; 
const char s_error[]    = "$02%6lx%2x%3i\n"; //time, state, result
const char s_DisplayConfig[] = "$07C %2x S %2x %2x %2x %2x %2x B %2xZ\n"; 
const char s_HexFour[]  = "%4x"; 
const char s_Endoutput[] = "Z\n";
const char s_GainAck[]   = "$09Z\n"; 

extern byte srf08_ping(byte device, byte cmd);
extern byte srf08_range(byte device, byte echo, short * range);
extern byte srf08_Version(byte device, byte  * Version);
extern byte srf08_SetMaxrange(byte device, byte range, byte  gain);
extern void srf08_ChangeAddress(byte oldAddress, byte newAddress);


extern void SetPWMLeft(int16_t PWM);
extern void SetPWMRight(int16_t PWM);
extern volatile int32_t  LeftDistance, RightDistance;
extern volatile uint16_t MotorLeft, MotorRight; // current PWMvalues
extern volatile char Direction;
extern bool DebugDoCmd;

extern int16_t Watchdog;
extern int32_t PowerWatch;

extern char outputstring[200]; // buffer to print formated strings to

volatile uint16_t heartbeatCycle,heartbeatCycleSize,heartbeatOnTime,Heartbeat2On,Heartbeat2Off;

volatile uint8_t MotorState;
volatile uint8_t MotorDelay;
volatile uint8_t State;
uint8_t SonarAddress[5];
int16_t Sonar[5];
uint8_t SonarCount=2;
uint8_t Range;
uint8_t Gain;


void displayConfig(void)
{
  // hardcode these values so motorcontrol does not have to change
  snprintf(outputstring,79,s_DisplayConfig,0,114,112,0,0,0,115);
  Serial.println(outputstring);
}

void InitState(void)
{
State =WAITSTART;
MotorState= 0; 
MotorDelay=0;
// this should be set in future by eeprom
SonarAddress[0]=114;
SonarAddress[1]=112;
  Serial.println("state init");
}
void runSensors(void)
{
// 20150111 change led to flash twice 
if (heartbeatOnTime != 100)
  {
  heartbeatOnTime  = 100;
  }
if (Heartbeat2On != 200)
  {
  Heartbeat2On  = 200;
  Heartbeat2Off = 300;
  }
// 
uint8_t debug;
int16_t tempvalue;
int8_t nResult;
uint16_t SonarRange;
uint32_t TempLeft,TempRight,timestamp;
uint8_t Sregs;
uint16_t TempMotorLeft, TempMotorRight;
char TempBuffer[80];
char TempNumber[6];
int CommandID;

//debug = SonarDebug;

  if (MotorState == 0 )
    {
    MotorState = 1; 
    }
  if (MotorState == 1 )
    {
    if (SonarCount > 0)
      {
      //debug = 0; // debug flag for sonar stuff
      nResult = srf08_ping(ALLDEVICES, RANGE_US);      /* initiate a ping, distance in cm */
      if (nResult < 0)
        {        
        timestamp= millis();
        snprintf(outputstring,79,s_error,timestamp,2,nResult);
        Serial.println(outputstring);
        } 
//      if (DebugDoCmd)
//        {
//        Serial.print("ping MotorState");  
//        Serial.print(State);  
//        Serial.print("SonarCount");  
//        Serial.print(SonarCount);  
//        
//        }        
      }
    MotorDelay = 70;
    MotorState = 2;
    } // end MotorState == 1
  if (MotorState == 2 )
    {
    
    if (MotorDelay==0)
      {
//      if (DebugDoCmd)
//        {
//        Serial.print("range MotorState");  
//        Serial.println(State);  
//
//        
//        }          
      // get the tick counter in local variable for sending to laptop
      Sregs =SREG; //save & clear ints
      cli();
      TempLeft = LeftDistance;
      TempRight = RightDistance;
      TempMotorLeft = MotorLeft;
      TempMotorRight = MotorRight;
      
      SREG =Sregs; //restore ints 
      timestamp= millis();      
      // time,leftdistance,rightdistance,leftmotor,rightmotor,Direction
      snprintf(TempBuffer,79,s_output,timestamp, TempLeft,TempRight,TempMotorLeft,TempMotorRight,Direction);

//      debug = SonarDebug; // debug flag for sonar stuff

      if (SonarCount>0)
        {
        for(int i=0;i< SonarCount;i++)
          {
          nResult = srf08_range(SonarAddress[i], 0,&SonarRange); 
          if (nResult == 0 )
            {
            Sonar[i]=SonarRange;
            if (Sonar[i] == 0) 
              {
              Sonar[i] = MAXVALUE;
              }  
            snprintf(TempNumber,79,s_HexFour,Sonar[i]);
            strlcat(  TempBuffer,  TempNumber,(size_t) 79 );          
  
            }
          else
            {
            timestamp= millis();
            snprintf(outputstring,79,s_error,timestamp,3,nResult);
            Serial.println(outputstring);
                          
            }
          } // end for
        } // end sonarcount

      // write out Z at end of line
      snprintf(TempNumber,79,s_Endoutput);
      strlcat(  TempBuffer,  TempNumber,(size_t) 79 );

      //compass,front,left,right,

      // Now send all the data to the laptop
      Serial.println(TempBuffer);
      
      
      MotorState = 1;
      } // MotorDelay==0
    MotorDelay--;
    } // end MotorState == 2

} // end runsensor
void handleWait(void)
{
  SetPWMLeft(0);
  SetPWMRight(0);

}
int16_t  SetMax(void)
{
int16_t  returnvalue = 0;
int8_t nResult;
uint32_t timestamp;
      if (DebugDoCmd)
        {
        Serial.print("set maxrange SonarCount ");  
        Serial.println(SonarCount);  
        
        }          
      if (SonarCount > 0)
        {

        for(int i=0;i< SonarCount;i++)
          {
          nResult = srf08_SetMaxrange(SonarAddress[i], Range,Gain); 
          if (nResult != 0 )
            {
            timestamp= millis();
            snprintf(outputstring,79,s_error,timestamp,5,nResult);
            Serial.println(outputstring); 
            returnvalue =    nResult;      
            break;              
            }
          else
            {
            if (i == SonarCount)
              {
              snprintf(outputstring,79,s_GainAck);
              Serial.println(outputstring);                
              }
            }
          } // end for
      } // end sonarcount
      return returnvalue;
}
void handleCalibrateCompass(void )
{
Serial.println("do nothing");
}
void DoState(void)
{
//printf_P(s_DoState,State);

  switch (State) 
    {
    case COMMAND:
      runSensors();
      break;
    case CALIBRATE:
      handleCalibrateCompass();
      break;
    case WAITSTART:
      break;
if (DebugDoCmd)
  {
  Serial.print("doState");  
  Serial.println(State);  
  
  }

  }
}
