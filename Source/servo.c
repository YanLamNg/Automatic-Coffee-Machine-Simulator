#include "servo.h"		


volatile uint32_t msTicks=0;      //counts 1ms timeTicks
/*void SysTick_Handler(void) {
	msTicks++;
}*/
//Delays number of Systicks (happens every 1 ms)
void InitServos (void){ 
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    //Initalize PB8 (TIM4 Ch1) and PB9 (TIM4 Ch2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    // GPIO_High_Speed
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // Assign Alternate Functions to pins
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
  
}

//Delays number of Systicks (happens every 1 ms)
static void Delay(__IO uint32_t dlyTicks){                                              
  uint32_t curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}
void setSysTick(void){
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1){};
	}
}
void InitPWMTimer4(void) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  //TIM3 Clock Enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  // Time Base Configuration for 50Hz
  TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 84 -1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM3, ENABLE);
}

void SetupPWM(void) {
	
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //Set output capture as PWM mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //Enable output compare 
  TIM_OCInitStructure.TIM_Pulse = 0; // Initial duty cycle at 0%
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // HIGH output compare active
  // Set the output capture channel 1 and 2 (upto 4)
  TIM_OC3Init(TIM3, &TIM_OCInitStructure); // Channel 1
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM3, ENABLE);	
}

void setServo(int ms){
	
		TIM3->CCR3 = ms;	//PB8
	
}

/*
//servo example
int main(void){
  setSysTick();
  InitServos();
  InitPWMTimer4();
  SetupPWM();

  while (1){
    //only control one servo at timer 4, channel 1 
    TIM4->CCR1 = 600;	// 0.6 ms
    Delay(1000);     
    TIM4->CCR1 = 1500; // 1.5 ms
    Delay(1000);
    TIM4->CCR1 = 2100; // 2.1 ms
    Delay(1000);
  }
}
*/