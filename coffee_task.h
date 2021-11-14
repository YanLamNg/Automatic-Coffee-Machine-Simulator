#ifndef _COFFEE_TASK_H
#define _COFFEE_TASK_H

#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "discoveryf4utils.h"
//******************************************************************************
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "croutine.h"
#include "semphr.h"
//******************************************************************************
#include "servo.h"
#include "lcd.h"
#include "Button.h"

#define MAX_COOFFEE_STACK 10
typedef struct{
  char name [10];
	char aka [2];
	int id;
  int espressoTime;         //in second
	int chocolateMilkTime;
	int milkTime;
} coffeeTask_t;

typedef struct {
	char name[10];
	char aka[2];
	int cookingType; // 1: espresso, 2: milk 4: chocolate milk; 
} coffeeType;

typedef enum{ ESPRESSO, MOCHA, LATTE } CoffeeName;
extern TaskHandle_t xMasterThreadHandler;
extern coffeeType coffees [3] ;
extern int coffeeCount;
extern unsigned int count_down_timer_ptr;
extern SemaphoreHandle_t xMutex;
void generateNewCoffee( coffeeTask_t *currentCoffeeTask, int id, CoffeeName coffeeName, int* golbalSetting );

void addEspresso(void);
void addFreshMike(void);
void addChocolateMike(void);
	
void makeCoffeeTask(coffeeTask_t *coffeeTask);


#endif