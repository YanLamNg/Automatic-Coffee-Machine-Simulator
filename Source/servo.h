#ifndef _SERVO_H
#define _SERVO_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

extern volatile uint32_t msTicks;
void InitServos(void);
//void SysTick_Handler(void);
static void Delay(__IO uint32_t dlyTicks);
void setSysTick(void);
void InitPWMTimer4(void);
void SetupPWM(void);

void setServo(int ms);
#endif