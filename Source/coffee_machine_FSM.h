/* COMP 4450
	Authors:
		Li Borui 7779844
		YanLam Ng 7775665
	Group: 5  
*/
#ifndef _COFFEE_MACHINE_FSM_H
#define _COFFEE_MACHINE_FSM_H

#include <stdio.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
/***********************************/
#include "codec.h"
#include "Button.h"
#include "lcd.h"
#include "coffee_task.h"


//variables and default value
extern int timeInterrupt;
extern int currentIndex;
extern int selectedAction;
extern int selectedTiming;
extern int timingSetting[];
extern int sleepTime;
extern double sawWave ;
extern int countDown ;
extern float frequency; ;
extern float filteredSaw ;


extern volatile uint32_t sampleCounter ;
extern volatile int16_t sample;
//state functions

int startAction(void);

int selectTypeAction(void);
int preSelectTimingAction(void);
int selectTimingAction(void);


int noAction(void);
int noProgrammingAction(void);
int noActiveAction(void);

int resetAction2(void);
int resetTypeAction(void);
int nextAction(void);

int activeAction(void);
int preBrewAction(void);
int brewAction(void);
int postBrewAction(void);
int resetAction(void);

int playConfirmAction(void);

int processSound(void);


void InitLEDs(void);
void TIM2_IRQHandler(void);

void setLight(int count);



//define the state machine and transmittion between them

typedef enum  {start, type, timing, active, pre_brew, brew, reset, sound} MachineStates;
extern const char * state_str[];
extern const char * type_str[] ;


typedef enum            {starting,    dummyProgramming,  selectType,       selectTiming,       brewing,     reseting,
                         next,       preBrew,      postBrew,       sounding,     reset2,       resetType,        activing,     dummyActive,
                         preTiming,                          comfrim
                        } MachineActions;



//state
extern int (*state[])(void);

typedef struct
{
  MachineStates nextStatus;
  MachineActions nextAction;
} stateElement;

typedef struct {
  float tabs[8];
  float params[8];
  uint8_t currIndex;
} fir_8;

extern fir_8 filt;
extern stateElement stateMatrix[5][4];

// adatped functions and defines from course note

#define INITIAL_STATE start

#define LED_BLUE_ON   GPIOD->BSRRL = GPIO_Pin_15;
#define LED_BLUE_OFF  GPIOD->BSRRH = GPIO_Pin_15;
#define NOTEFREQUENCY 0.015   //frequency of saw wave: f0 = 0.5 * NOTEFREQUENCY * 48000 (=sample rate)
#define NOTEAMPLITUDE 500.0   //amplitude of the saw wave

extern MachineStates currentState;

int processAction ( ButtonAction buttomState, MachineStates machineStates );
float updateFilter(fir_8* theFilter, float newValue);
void initFilter(fir_8* theFilter);
//int processAction ( ButtonAction action, MachineStates state );
#endif
