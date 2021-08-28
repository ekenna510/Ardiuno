#ifndef fsm_h
#define fsm_h


#define MAXVALUE 0xFFFF

enum mainStates {WAITSTART = 1,
  CALIBRATE = 2,
  COMMAND= 3};

void InitState(void);
void DoState(void);

#endif
