
#include "motor.h"
#include "fsm.h"
#include <stdio.h>

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
char outputstring[200]; // buffer to print formated strings to

extern volatile uint16_t heartbeatCycle,heartbeatCycleSize,heartbeatOnTime,Heartbeat2On,Heartbeat2Off;
extern volatile char Direction;
extern uint8_t Range;
extern uint8_t Gain;
extern volatile uint8_t State;
extern volatile uint8_t MotorDelay;
extern void DoState(void);
extern volatile uint8_t MotorState;
extern void handleWait(void);
extern int16_t  SetMax(void);
extern void InitSonar(void);

int16_t Watchdog=2000;
int32_t PowerWatch =400000;
bool EchoInput=true;
const char s_GainAck[]            = "$09Z\n"; 
const char s_EchoAck[]            = "$09Z\n"; 
const char s_annc[]               = "$01EFK%6lx Build %s\n";
const char s_BuildTime[]          = __DATE__ __TIME__;
bool DebugDoCmd=false;


void setup() {


  // put your setup code here, to run once:
  heartbeatCycle = 0;
  heartbeatCycleSize = 500;
  heartbeatOnTime = 100;
  Heartbeat2On = 0;
  Heartbeat2Off = 0;
  
  Direction="F";
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);   
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)  
  InitSonar();
  delay(2000); 
  initmotor();
  delay(2000); 

  InitState();
  delay(2000); 
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off (Low is the voltage level)
      
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}



void do_cmd(String inputString )
{
  if (DebugDoCmd)
  {
  Serial.print("in doccmd ");
  Serial.println(inputString );    
  }

uint8_t Sregs;
int16_t CommandID;
int16_t nbr,nbr2;
char cCmd = inputString.charAt(0);



String s;
  // put code to enter watchdog here
  if ( cCmd == 'w' || cCmd == 'W' )
    {
    Sregs =SREG; //save & clear ints
    cli();
    Watchdog = 2000;
    PowerWatch = 400000;
    SREG=Sregs; //save & clear ints
    Serial.print(s_EchoAck);
//    snprintf_P(UartPrepBuffer,5,s_EchoAck);
//    srl_write(0, UartPrepBuffer, strlen(UartPrepBuffer));   
    }
  else
    {
    if ( cCmd == 'm' || cCmd == 'M' )
      {
      s=  inputString.substring(2);
      nbr = s.substring(0,s.indexOf(" ")).toInt();
      CommandID = nbr;
      s = s.substring(s.indexOf(" ")+1);
      nbr = s.substring(0,s.indexOf(" ")).toInt();
      if (DebugDoCmd)
      {
      Serial.print("LeftPWM ");
      Serial.print(nbr);
      }      
      SetPWMLeft(nbr);
      s = s.substring(s.indexOf(" ")+1);
      nbr = s.toInt();
      if (DebugDoCmd)
          {
          Serial.print("RightPWM ");
          Serial.println(nbr);
          }     
      SetPWMRight(nbr);
      
      //  let any motor control reset watch dog
      Sregs =SREG; //save & clear ints
      cli();
      Watchdog = 2000;
      PowerWatch = 400000;
      SREG=Sregs; //save & clear ints
 //     snprintf_P(UartPrepBuffer,5,s_EchoAck);
 //     srl_write(0, UartPrepBuffer, strlen(UartPrepBuffer));     
      
      }
    else
      {

    
      // turn on or off echo command after prcessing them
      if ( cCmd == 'e' || cCmd == 'E' )
        {
        s=  inputString.substring(2);
        nbr = s.substring(0,s.indexOf(" ")).toInt();
        CommandID = nbr;
        s = s.substring(s.indexOf(" "));
        nbr = s.toInt();
        if (DebugDoCmd)
            {
            Serial.print("Echo ");
            Serial.print(nbr);
            }                   

        if (nbr == 1)
          {
          EchoInput = true;
          }
        else if (nbr == 2)
          {
          DebugDoCmd= true;
          }
        else if (nbr == 3)
          {
          DebugDoCmd= false;
          }
          else 
          {
          EchoInput = false;
          }
        Serial.print(s_EchoAck);          
//        snprintf_P(UartPrepBuffer,79,s_AckMotor,TimeCounter,-200);
//        snprintf_P(UartPrepBuffer,5,s_EchoAck);
//        srl_write(0, UartPrepBuffer, strlen(UartPrepBuffer));
        }
      // set range aND gain
      if ( cCmd == 'g' || cCmd == 'G' )
        {
        s=  inputString.substring(2);
        nbr = s.substring(0,s.indexOf(" ")).toInt();
        CommandID = nbr;
        s = s.substring(s.indexOf(" ")+1);
        nbr = s.substring(0,s.indexOf(" ")).toInt();
        if (DebugDoCmd)
            {
            Serial.print("range ");
            Serial.print(nbr);
            }         
        Range = (uint8_t) nbr;
        s = s.substring(s.indexOf(" ")+1);
        nbr = s.toInt();
        if (DebugDoCmd)
            {
            Serial.print(" Gain ");
            Serial.println(nbr);
            }          
        Gain = (uint8_t) nbr;
        nbr = SetMax();
        if (nbr == 0)
          { 
          Serial.print(s_EchoAck);
          } 
           
        }
      if ( cCmd == 'd' || cCmd == 'D' )
        {
        s=  inputString.substring(2);
        nbr = s.substring(0,s.indexOf(" ")-1).toInt();
        CommandID = nbr;     
        s = s.substring(s.indexOf(" ")+1);        
        cCmd=s.charAt(0);
        SetDirection(cCmd);
        Direction = cCmd;
        if (DebugDoCmd)
            {
            Serial.print("Direction ");
            Serial.println(Direction);
            }           
        //  let any motor control reset watch dog
        Sregs =SREG; //save & clear ints
        cli();
        Watchdog = 2000;
        PowerWatch = 400000;
        SREG=Sregs; //save & clear ints
    
        //snprintf_P(UartPrepBuffer,5,s_AckMotor,TimeCounter,CommandID);
        //srl_write(0, UartPrepBuffer, strlen(UartPrepBuffer));
        }
    
      if ( cCmd == 'a' || cCmd == 'A')
        {
//        // allow Sonar and compass to be added dynamically
//        sIndex = readcmd( sIndex,&cCmd);
//        sIndex = readInt16(sIndex,&nbr );
//        sIndex = readInt16(sIndex,&nbr2);
//        
//         writeConfig(cCmd, (uint8_t) nbr,(uint8_t) nbr2);
//         readConfig();
        }
      if ( cCmd == 'r' || cCmd == 'R')
        {
//        clearConfig();
        }
      if ( cCmd == 'p' || cCmd == 'P')
        {
//        displayConfig();
        }   
      if (cCmd == 'q' || cCmd == 'Q')
        {
//        sIndex = readInt16(sIndex,&nbr );
//        SonarDebug = (uint8_t)  nbr;        
        
        }
      if ( cCmd == 'b' || cCmd == 'B')
        {
        s=  inputString.substring(2);    
        nbr = s.toInt();    
        if (DebugDoCmd)
            {
            Serial.print("nbr ");
            Serial.println(nbr);
            }                        
        // only allow certain state changes
        if (nbr == COMMAND)
          {
          State =  COMMAND;
          }
        else
          {
          if (nbr == WAITSTART)
            {
            handleWait();
            State =  WAITSTART;
            }
          }
        if (DebugDoCmd)
            {
            Serial.print("State ");
            Serial.println(State);
            }            
        } 
      if (cCmd == 'f' || cCmd == 'F')
        {
//        // Print contents of port
//        sIndex = readInt16(sIndex,&nbr );
//        if (nbr == 1)
//          {
//          Gain = PORTA;
//          }
//        if (nbr == 2)
//          {
//          Gain = PORTB;
//          }
//        if (nbr == 3)
//          {
//          nbr = PORTC;
//          }
//        if (nbr == 4)
//          {
//          Gain = PORTD;
//          }
//        if (nbr == 5)
//          {
//          Gain = PORTE;
//          }
//        if (nbr == 6)
//          {
//          Gain = PORTF;
//          }
//        if (nbr == 7)
//          {
//          Gain = PORTG;
//          }
//        snprintf_P(UartPrepBuffer,79,s_DisplayPort,Gain);
//        srl_write(0, UartPrepBuffer, strlen(UartPrepBuffer));
        }
      if (cCmd == 'h' || cCmd == 'H')
      {
      LeftDistance = 0;
      RightDistance = 0;
      if (DebugDoCmd)
          {
          Serial.println("H done");
          }    
      }
      // print annoucment 
      if (cCmd == 'c' || cCmd == 'C')
      {
      Serial.println("announce");
      uint32_t timestamp;
      timestamp= millis();
      snprintf(outputstring,79,s_annc,timestamp, s_BuildTime);
      Serial.println(outputstring);      
      }
      
    } //else M


  } // else W
  
}

void LigthLEDs(void)
{

// regular heartbeat
heartbeatCycle += 1;
if (heartbeatCycle > heartbeatCycleSize)
  {
  heartbeatCycle = 0;
  }

if (heartbeatCycle == 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  }
if (heartbeatCycle == heartbeatOnTime)
  {
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }
// allow for different flash patterns 
// set  Heartbeat2On = 0 for only 1 flash 
// set  Heartbeat2On >  heartbeatOnTime
//   and Heartbeat2Off > Heartbeat2On but < heartbeatCycleSize
//  for 2 flashes
if (Heartbeat2On > 0 && heartbeatCycle == Heartbeat2On)
    { 
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
if (Heartbeat2On > 0 && heartbeatCycle == Heartbeat2Off)
  {
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }


}
void loop() {
  int32_t localLeftDistance; 
  if (stringComplete) {
    Serial.println(inputString);    
    do_cmd(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }    
  DoState();
  LigthLEDs();  
  delay(1); 
  Watchdog--;
  PowerWatch--; 
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
