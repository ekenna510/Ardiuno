#include <Arduino.h>

#include "motor.h"

volatile int32_t LeftDistance, RightDistance;
volatile uint8_t LastPortB;
volatile uint16_t MotorLeft, MotorRight; // current PWMvalues
volatile char Direction;

void initmotor(void)
{
  // Moved this from ClearMotorPins
//20210705 Motor port changed to port E4 (Digit 2 right white wire) and E5 (digital 3 left balck wire) 
DDRE |= _BV(PORTE4) | _BV(PORTE5) ; /* enable PWM outputs */
LeftEnable = 0;
RightEnable = 0;
// must be 0 require for timer3 to be on Page 56
//PRR1 |= _BV(PRTIM3);

TCCR3A = _BV(WGM31) | _BV(WGM30); /* set timer/counter 3 to PWM mode 3 */
TCCR3A |= _BV(COM3B1) | _BV(COM3C1); /* Clear OCnA/OCnB/OCnC on compare match when up-counting. Set OCnA/OCnB/OCnC on compare match when downcounting.*/
TCCR3B |= _BV(CS30) ; /* prescale: 1 @ 16 MHz, 10 bit PWM overflows @ 15625 Hz giving 7812 HZ */ 




  
// Need to change this different board
//
// ENCODER Section
// old pin outs 
// left wheel
// yellow E4
// green E5
// right wheel
// black E6
// red E7
// NEW
// Yellow D10 PCINT4 PB4
// Green  D11 PCINT5 PB5
// Black  D12 PCINT6 PB6
// Red    D13 PCINT7 PB7
// left wheel pin setup
  pinMode(10, INPUT);
  pinMode(11, INPUT);


// right wheel pin setup
  pinMode(12, INPUT);
  pinMode(51, INPUT);
  LeftDistance=0; 
  RightDistance=0;
  LastPortB=PINB;  
// enable interrupts on pin change
  PCICR |= _BV(PCIE0);
  PCMSK0 |= _BV(PCINT2)| _BV(PCINT4)| _BV(PCINT5)| _BV(PCINT6);
// PWM
// Right white wire to E4 Digitial 2 
// Left black wire to E5 Digitial 3
// see ClearMotorPins

// Direction
// Right white old b2 New to A3 Digitial25
// left black old b1  new to A2 Digitial24
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  PORTA |= 0x0C; // pin 0 and 1  // Forward pins
  Direction  ="F";
  Serial.println("motor init");
}

/*
 * place a function in the '.init1' section to enable external RAM so
 * that the C runtime startup can initialize it during C startup.  
 */
void ClearMotorPins (void)  
  __attribute__ ((naked)) __attribute__ ((section (".init1")));

void ClearMotorPins(void)
{
//20210705 Motor port changed to port E4 (Digit 2 right white wire) and E5 (digital 3 left balck wire)  
// set these as output (pins 0 through 2 on port B)
// PORTB0 is led 
// PORTB1 PORTB2 Direction ports



pinMode(3, OUTPUT);
pinMode(2, OUTPUT);
PORTE = 0;
LeftEnable = 0;
RightEnable = 0;
//DDRE |= _BV(PORTE4) | _BV(PORTE5) ; /* enable PWM outputs */
//LeftEnable = 0;
//RightEnable = 0;
//// must be 0 require for timer3 to be on Page 56
////PRR1 |= _BV(PRTIM3);
//
//TCCR3A = _BV(WGM31) | _BV(WGM30); /* set timer/counter 3 to PWM mode 3 */
//TCCR3A |= _BV(COM3B1) | _BV(COM3C1); /* Clear OCnA/OCnB/OCnC on compare match when up-counting. Set OCnA/OCnB/OCnC on compare match when downcounting.*/
//TCCR3B |= _BV(CS30) ; /* prescale: 1 @ 16 MHz, 10 bit PWM overflows @ 15625 Hz giving 7812 HZ */ 

}


void SetPWMLeft(int16_t PWM )
{
if(LeftEnable != PWM)
  {
  noInterrupts();
  LeftEnable = PWM;
  MotorLeft = PWM;  
  interrupts();

  }
}

void SetPWMRight(int16_t PWM)
{
if(RightEnable != PWM)
  {
  noInterrupts();
  RightEnable=PWM;
  MotorRight=PWM;
  interrupts();
  }
}

void SetDirection(char direction)
{
  // turn off lower pins
  PORTA &= 0xF3;

  if (direction== 'F')
  {
  // then set correct pins to 1
  PORTA |= 0x0C; // pin 0 and 1

  }else if (direction== 'B')
  {
  // then set correct pins to 0
  PORTA |= 0x0; // pin 0 and 1

  }else if (direction== 'L')
  {
  // then set correct pins
  PORTA |= 0x8; // pin 0 and 1
  }else if (direction== 'R')
  {
  // then set correct pins 0 to 0 1 to 1 
  PORTA |= 0x04; // pin 0 and 1
  }else
  {
    /* code */
  }

}
ISR (PCINT0_vect) 
{
// For PCINT of pins B0 a B7  
uint8_t CurrentPortB = PINB;
// last pins need to be in position 1,0 current postition need to be in 3 and 2
uint8_t lefts = ((LastPortB &= B00110000)>>4) |((CurrentPortB &= B00110000)>>2);

uint8_t rights = ((LastPortB &= B01000000)>>6) | ((LastPortB &= B00000100) >> 1) | ((CurrentPortB &= B01000000)>>4) | ((CurrentPortB &= B00000100)<<1);

uint8_t oldright,newright;
oldright= LastPortB &= B01000100;
newright= CurrentPortB &= B01000100;

if (newright != oldright)
  {
     RightDistance++;
  }

//uint8_t rights = ((LastPortB &= B11000000)>>4) |((CurrentPortB &= B11000000)>>2);

  
//  switch (lefts) {
//      case 0: case 5: case 10: case 15:
//        break;
//      case 1: case 7: case 8: case 14:
//        LeftDistance++;  //CW increase
//        break;
//      case 2: case 4: case 11: case 13:
//        LeftDistance--; 
//        break;
//      case 3: case 12:
//        LeftDistance += 2; 
//        break;
//      default:
//        LeftDistance -= 2; 
//        break;
//    }
//  switch (rights) {
//      case 0: case 5: case 10: case 15:
//        break;
//      case 1: case 7: case 8: case 14:
//        RightDistance++;  //CW increase
//        break;
//      case 2: case 4: case 11: case 13:
//        RightDistance--; 
//        break;
//      case 3: case 12:
//        RightDistance += 2; 
//        break;
//      default:
//        RightDistance -= 2; 
//        break;
//    }    
LastPortB = CurrentPortB;

 //Serial.print("i");    
} 


//                           _______         _______       
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2

    //  new new old old
    //  pin2  pin1  pin2  pin1  Result
    //  ----  ----  ----  ----  ------
    //  0 0 0 0 no movement
    //  0 0 0 1 +1
    //  0 0 1 0 -1
    //  0 0 1 1 +2  (assume pin1 edges only)
    //  0 1 0 0 -1
    //  0 1 0 1 no movement
    //  0 1 1 0 -2  (assume pin1 edges only)
    //  0 1 1 1 +1
    //  1 0 0 0 +1
    //  1 0 0 1 -2  (assume pin1 edges only)
    //  1 0 1 0 no movement
    //  1 0 1 1 -1
    //  1 1 0 0 +2  (assume pin1 edges only)
    //  1 1 0 1 -1
    //  1 1 1 0 +1
    //  1 1 1 1 no movement
/*
  // Simple, easy-to-read "documentation" version :-)
  //
  void update(void) {
    uint8_t s = state & 3;
    if (digitalRead(pin1)) s |= 4;
    if (digitalRead(pin2)) s |= 8;
    switch (s) {
      case 0: case 5: case 10: case 15:
        break;
      case 1: case 7: case 8: case 14:
        position++; break;
      case 2: case 4: case 11: case 13:
        position--; break;
      case 3: case 12:
        position += 2; break;
      default:
        position -= 2; break;
    }
    state = (s >> 2);
  }
  */
