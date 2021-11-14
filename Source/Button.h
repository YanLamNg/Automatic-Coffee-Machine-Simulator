/* COMP 4450
	Authors:
		Li Borui 7779844
		YanLam Ng 7775665
	Group: 5  
*/
#ifndef __BUTTON_H
#define __BUTTON_H

#define BUTTON_PIN GPIO_Pin_0
#define BUTTON_EXTI_LINE EXTI_Line0

#define DEBUG_LED 0
#include <stdio.h>

typedef enum  {NO_CLICK,SINGLE_CLICK,DOUBLE_CLICK, LONG_CLICK, HOLD_DOWN} ButtonAction;
extern const char * button_action_str[];
extern ButtonAction button_status;
extern unsigned int timer_counter;
extern unsigned int count_down_timer;
extern int isSuspend;
extern unsigned int count_down_timer_ptr;

void InitButton(void);
void EnableButtonEXTI(void);
void DisableButtonEXTI(void);
void EnableTimerInterrupt(void);
void InitButtonTimer(void);
void EnableTimerInterrupt(void);
void TIM5_IRQHandler(void);
#endif 
