//******************************************************************************
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "discoveryf4utils.h"
//******************************************************************************

//******************************************************************************
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "croutine.h"
#include "semphr.h"
//******************************************************************************
#include "lcd.h"
#include "coffee_machine_FSM.h"
#include "servo.h"


void coffeMachineMaster(void *pvParameters);
extern SemaphoreHandle_t xMutex;
//void Delay(uint32_t val);

#define STACK_SIZE_MIN	128	/* usStackDepth	- the stack size DEFINED IN WORDS.*/

//******************************************************************************
int main(void)
{

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	count_down_timer_ptr = NULL;
	//initialize all required modules
		currentState = INITIAL_STATE;

	SystemInit();
	InitLEDs();

	//initialize LCD
	LCD_Init();
	DelayMS(10);
	LCD_CLEAR();
	DelayMS(10);
	LCD_print((unsigned char*)"Initialize...", NULL);



	

	//initialize button
	EnableTimerInterrupt();
	InitButton();
	EnableButtonEXTI();

//TODO: soundconflict with servo
//			tried switch the servo PWD timer 4 to 3 but it did not work

  //iniitalize sound
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	codec_init();
	codec_ctrl_init();
	I2S_Cmd(CODEC_I2S, ENABLE);
	initFilter(&filt);

		//init servo 
	setSysTick();

  InitServos();
	
  InitPWMTimer4();
	
  SetupPWM();
	xMutex = xSemaphoreCreateMutex();

	xTaskCreate( coffeMachineMaster, (const char*)"coffeMachineMaster", 
		STACK_SIZE_MIN, NULL, tskIDLE_PRIORITY, &xMasterThreadHandler );

	vTaskStartScheduler();
}

void coffeMachineMaster(void *pvParameters){
	int prev_State = NULL;
	
	//example for making coffee task
	/*coffeeTask_t* coffeeTask = (coffeeTask_t*)malloc(sizeof(coffeeTask_t));
	coffeeTask->espressoTime = 3;
	coffeeTask->milkTime = 2;
	coffeeTask->chocolateMilkTime = 1;
	makeCoffeeTask(coffeeTask);
	*/
	
	while (1) {	
		processAction(button_status, currentState);
		if (currentState != prev_State) {
			//LCD_print((unsigned char*)state_str[currentState], NULL);
			prev_State = currentState;
		}
		vTaskDelay( 100 / portTICK_RATE_MS );
	}
}

//******************************************************************************
