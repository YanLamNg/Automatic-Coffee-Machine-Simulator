#include "coffee_task.h"
#include "coffee_machine_FSM.h"
#define STACK_MIN	128	/* usStackDepth	- the stack size DEFINED IN WORDS.*/

coffeeType coffees [3] = {{"espresso", "ES", 1}, {"mocha", "MC", 5}, {"latte", "LT",3}};
TaskHandle_t xMasterThreadHandler = NULL;
int coffeeCount  = 0;
SemaphoreHandle_t xMutex;

/*
int (*task[])(void) =  {
                        };
*/
								
void addEspresso(void){
	setServo(600);	// 0.6 ms
}
void addFreshMilk(void){
	setServo(1500); // 1.5 ms
}
void addChocolateMilk(void){
	setServo(2100); // 2.1 ms
}

void printTaskInfo(coffeeTask_t* coffee){
		char buffer [16];
		sprintf(buffer, "%d E:%d M:%d C:%d", coffee->id,coffee->espressoTime, coffee->milkTime, coffee->chocolateMilkTime);
		taskENTER_CRITICAL();
		LCD_print(NULL,(unsigned char*) buffer);
		taskEXIT_CRITICAL();
}

void printRemainTaskInfo(coffeeTask_t* coffee, int espressRemain, int milkTimeRemain, int chocolateMilkRemain){
		char buffer [16];
		sprintf(buffer, "%d E:%d M:%d C:%d", coffee->id,coffee->espressoTime - espressRemain, coffee->milkTime - milkTimeRemain, coffee->chocolateMilkTime- chocolateMilkRemain);
		taskENTER_CRITICAL();
		LCD_print(NULL,(unsigned char*) buffer);
		taskEXIT_CRITICAL();
}



void coffeeHandler(void* coffeePtr){

	coffeeTask_t* coffee = (coffeeTask_t*)coffeePtr;

	
	
	
	for(int i = 0; i < coffee->espressoTime; i++) {
		printRemainTaskInfo(coffee, i, 0,0);
		xSemaphoreTake( xMutex, portMAX_DELAY );
		count_down_timer_ptr  = 0 ;
		addEspresso();
		while( count_down_timer_ptr <= 100){
		//	sprintf(buffer, "%d", count_down_timer_ptr);
		//	LCD_print((unsigned char*)buffer,NULL);
		}
		xSemaphoreGive( xMutex );
		vTaskDelay( 5 / portTICK_RATE_MS );
	}
	
	for(int i = 0; i < coffee->milkTime; i++) {
		printRemainTaskInfo(coffee, coffee->espressoTime, i, 0);
		xSemaphoreTake( xMutex, portMAX_DELAY );
		count_down_timer_ptr  = 0 ;
		addFreshMilk();

		while( count_down_timer_ptr <= 100){
		}
		xSemaphoreGive( xMutex );
		vTaskDelay( 5 / portTICK_RATE_MS );

	}
	for(int i = 0; i < coffee->chocolateMilkTime; i++) {

		printRemainTaskInfo(coffee, coffee->espressoTime, coffee-> milkTime, i);
		xSemaphoreTake( xMutex, portMAX_DELAY );
		count_down_timer_ptr  = 0 ;

		addChocolateMilk();
		while( count_down_timer_ptr <= 100){
		}
		xSemaphoreGive( xMutex );
		vTaskDelay( 5 / portTICK_RATE_MS );
	}

//TODO brew
	
//clean before exit
		printRemainTaskInfo(coffee, coffee->espressoTime, coffee-> milkTime, coffee->chocolateMilkTime);
	
	free(coffee);
	taskENTER_CRITICAL();
	coffeeCount--;
	taskEXIT_CRITICAL();
	processSound();
	vTaskDelete( NULL );
}

void makeCoffeeTask(coffeeTask_t *coffeeTask){
	xTaskCreate( coffeeHandler, (const char*)coffeeTask->name,STACK_MIN, coffeeTask, tskIDLE_PRIORITY, NULL );
}

void generateNewCoffee(coffeeTask_t *currentCoffeeTask, int id, CoffeeName coffeeName, int* golbalSetting ){
	
	coffeeType currentCoffeeType = coffees[coffeeName];
	sprintf (currentCoffeeTask->name, "%s", currentCoffeeType.name);
	sprintf (currentCoffeeTask->aka, "%s", currentCoffeeType.aka);
	currentCoffeeTask->id = id;
	
	currentCoffeeTask->espressoTime = currentCoffeeType.cookingType & 1 ? golbalSetting[1]:0;

	currentCoffeeTask->milkTime = currentCoffeeType.cookingType & 2 ? golbalSetting[2]:0;
	
	currentCoffeeTask->chocolateMilkTime = currentCoffeeType.cookingType & 4 ? golbalSetting[3]:0;
	
	
}
