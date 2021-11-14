/* COMP 4450
	Authors:
		Li Borui 7779844
		YanLam Ng 7775665
	Group: 5  
*/

#include "Button.h"
#include "stm32f4xx.h"     
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "croutine.h"
#include "coffee_task.h"

ButtonAction button_status;
ButtonAction last_button_status;
int isDoubleClick = 0;
int isSuspend = 0;
const char * button_action_str[] = {"No Click", "Single Click", "Double Click", "Ling Click", "Hold Down" };
int unsigned count_down_timer_ptr = 0;
//init button GPIO, button interrupt the timer interrupt
void InitButton()
{
	GPIO_InitTypeDef GPIO_Initstructure_b;									//init button GPIO
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	
	GPIO_Initstructure_b.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Initstructure_b.GPIO_OType = GPIO_OType_PP;
	GPIO_Initstructure_b.GPIO_Pin = BUTTON_PIN;
	GPIO_Initstructure_b.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Initstructure_b.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure_b);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	// Selects the GPIOA pin 0 used as external interrupt source
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	EXTI_InitTypeDef EXTI_InitStructure_b;
	EXTI_InitStructure_b.EXTI_Line = BUTTON_EXTI_LINE;
	EXTI_InitStructure_b.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure_b.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure_b.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure_b);
	
	//init timer
	InitButtonTimer();
}


void EnableButtonEXTI(){
	NVIC_InitTypeDef NVIC_InitStructure_b;
	NVIC_InitStructure_b.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure_b.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure_b.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure_b.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_b);
}

void DisableButtonEXTI(){
	NVIC_InitTypeDef NVIC_InitStructure_b;
	NVIC_InitStructure_b.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure_b.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure_b.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure_b.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure_b);
}


//botton interrupt callback method
void EXTI0_IRQHandler()
{
	 
	 if (EXTI_GetITStatus(EXTI_Line0) != RESET){ 
		 //on click weak the master thread up

			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){
					if(timer_counter>5 && timer_counter<50 && last_button_status == SINGLE_CLICK){
						 if(DEBUG_LED)GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
						 button_status = DOUBLE_CLICK;
						 last_button_status = NO_CLICK;
						 isDoubleClick = 1;
						 //TIM_ITConfig(TIM5, TIM_IT_CC2, DISABLE);
					}
					else if(last_button_status != SINGLE_CLICK){
						last_button_status = HOLD_DOWN;
						//TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);
					}
			 }
			 else{
					if(isDoubleClick == 1){
						last_button_status = NO_CLICK;
						isDoubleClick = 0;
					}
				  else if(timer_counter > 5 && timer_counter<150 ){
					 last_button_status = SINGLE_CLICK;
					 //TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);
				 }
					else if(last_button_status == HOLD_DOWN && timer_counter <= 5){
						last_button_status = NO_CLICK;
						//TIM_ITConfig(TIM5, TIM_IT_CC2, DISABLE);
					}
					
			 }
		 }
		EXTI_ClearITPendingBit(EXTI_Line0);
	  timer_counter = 0;

   
		if (isSuspend == 1){
			
		BaseType_t xYieldRequired;

     // Resume the suspended task.
     xYieldRequired = xTaskResumeFromISR( xMasterThreadHandler );

     if( xYieldRequired == pdTRUE )
     {
         // We should switch context so the ISR returns to a different task.
         // NOTE:  How this is done depends on the port you are using.  Check
         // the documentation and examples for your port.
         //portYIELD_FROM_ISR();
     }
			
		 }		 
				
				
			
}


//Timer interrupt function
void EnableTimerInterrupt()
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM5_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 0;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void DisableTimerInterrupt()
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM5_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 5;
    nvicStructure.NVIC_IRQChannelSubPriority = 0;
    nvicStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&nvicStructure);
}

//timer interrupt callback method
void TIM5_IRQHandler()
{
	count_down_timer_ptr++;

	timer_counter++;
	count_down_timer++;
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);
	//
	if(timer_counter > 50 && last_button_status == SINGLE_CLICK){		//single click if only one click in 0.5 second
		if(DEBUG_LED)GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		button_status = SINGLE_CLICK;
		last_button_status = NO_CLICK;
	}
	else if(timer_counter  > 150 && last_button_status == HOLD_DOWN){ 				//long click for holding down for 1.5 second
		if(DEBUG_LED)GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		last_button_status = NO_CLICK;
		button_status = LONG_CLICK;
		
		//TIM_ITConfig(TIM5, TIM_IT_CC2, DISABLE);
	}
}


//init 100hz timer
void InitButtonTimer()
{
		TIM_TimeBaseInitTypeDef timer_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    timer_InitStructure.TIM_Prescaler = 839;
    timer_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timer_InitStructure.TIM_Period = 999;
    timer_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timer_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM5, &timer_InitStructure);
    TIM_Cmd(TIM5, ENABLE);
		TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);
}

