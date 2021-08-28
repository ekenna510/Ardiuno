#ifndef motor_h
#define motor_h



#define LeftEnable OCR3C
#define RightEnable OCR3B
#define FOWARDPINS 0
#define BACKWARDPINS 0x03
#define LEFTPINS 0x1
#define RIGHTPINS 0x2

extern volatile uint8_t LastPortB;
extern volatile int32_t  LeftDistance, RightDistance;

void SetDirection(char direction);

void SetPWMLeft(int16_t PWM);
void SetPWMRight(int16_t PWM);
void initmotor(void);
ISR (PCINT0_vect); 

#endif
