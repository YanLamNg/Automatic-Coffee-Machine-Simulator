/* COMP 4450
	Authors:
		Li Borui 7779844
		YanLam Ng 7775665
	Group: 5  
*/

#include "stm32f4xx.h"                  // Device header
#include "coffee_machine_FSM.h"
#include "Sound.h"
fir_8 filt;

unsigned int timer_counter;
unsigned int count_down_timer;
int enableSound = 0;
int timeInterrupt = 0;
int currentIndex = 0;
int selectedAction = 0;
int selectedTiming = 0;
int id = 0;

int timingSetting [4] = { 0, 5, 5, 3};

//{name, espresso time, milk time, chocolate milk}  in second


int sleepTime = 10;
int selectedType = 0;
double sawWave = 0.0;
int countDown = 0;
float frequency = 0;
float filteredSaw = 0.0;

volatile uint32_t sampleCounter = 0;
volatile int16_t sample = 0;

const char * state_str[] = {"Start", "Select type", "Select time", "active mode", "pre_brew", "brew", "reset", "sound" };
const char * type_str[] = {"Unselected","Espresso", "MILK", "CHOCO" };


int (*state[])(void) =  {startAction, noProgrammingAction,     selectTypeAction, selectTimingAction, brewAction,  resetAction,
                         nextAction, preBrewAction, postBrewAction, processSound, resetAction2, resetTypeAction,  activeAction, noActiveAction,
                         preSelectTimingAction, playConfirmAction
                        };

stateElement stateMatrix[5][4] =
{
  //no click                    single click            double click                long click
  //programming mode
  { {start, starting},          {active, activing},     {start, starting},          {type, next}            },    //start
  { {type, dummyProgramming},   {type, selectType},     {timing, preTiming},        {type, dummyProgramming}},    //size
  { {timing, dummyProgramming}, {timing, selectTiming}, {timing, dummyProgramming}, {start, comfrim}         },   //timing
  //active mode
  { {active, dummyActive},      {active, activing},     {pre_brew, preBrew},    {type, resetType}       },    //active
  { {start, reseting},           {start, reseting},       {start, reseting},           {start, reseting}        },    //pre-brew
//  { {brew, brewing},            {brew, brewing},        {brew, brewing},            {brew, reseting}       },    //brew
//  { {sound, reseting},          {sound, reseting},      {sound, reseting},          {sound, reseting}       },    //reset
//  { {sound, sounding},          {sound, sounding},      {sound, sounding},          {sound, sounding}       }    //sound
};

MachineStates currentState = INITIAL_STATE;





// LCD print functions
void LCD_program_print(unsigned char* line, int countDownNum){
	char buffer [17];

	sprintf(buffer,"%-16s",line);
	
	for (int i = countDownNum; i>0; i--){
		buffer[15-i] = 0xFF;
	}
	taskENTER_CRITICAL();
	LCD_print( (unsigned char*) buffer,NULL );
	taskEXIT_CRITICAL();
}



//state functions
int noAction() {
	return 0;
}

//wait for a certain delay
void wait(uint32_t cnt) {
	int i;
	for ( i = 0; i < cnt; i++){
	}
		
}

//functon when entering next sate
int nextAction() {
	setLight(9);
	
	return 0;
}
// the initial state of the machine
int startAction() {
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_SetBits(GPIOD, GPIO_Pin_12);
	taskENTER_CRITICAL();
	LCD_CLEAR();
	LCD_print((unsigned char*)"-----Ready-----", NULL);
	taskEXIT_CRITICAL();	
	isSuspend = 1;
	vTaskSuspend(NULL);
	return 0;
}

//for programming mode, called no user input to update the time out 
int noProgrammingAction() {
	int temp = timer_counter / 10 % 10;

	if (temp < 5)
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
	else
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	if (timer_counter >= 500) {
		selectedType = 0;
		resetAction();
		currentState = start;
		taskENTER_CRITICAL();
		LCD_print((unsigned char * )"Program Abroad!",NULL);
		taskEXIT_CRITICAL();
	}
	if (currentState == type){
		LCD_program_print((unsigned char*)type_str[selectedType], 5 - timer_counter / 100);
	}
	else if (currentState == timing){
		char buffer [16];
		sprintf(buffer, "time: %d", selectedTiming);
		
		LCD_program_print((unsigned char*)buffer, 5 - timer_counter / 100);
	}
	return 0;
}

//select coffee type
int selectTypeAction() {
	selectedType++;
	if (selectedType > 3)
		selectedType = 1;
	setLight(selectedType);
	LCD_program_print((unsigned char*)type_str[selectedType], 5 - timer_counter / 100);

	return 0;
}

//parpare for select time action, and handle the error if coffee size is not selected 
int preSelectTimingAction() {
	if (selectedType == 0)
	{
		LCD_print((unsigned char*) "ERR: NO TYPE",NULL);

		for ( int i = 0 ; i <  10000000 * 2; i++) {
			if ( i % 1000000 == 0 )
				GPIO_ToggleBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
		}
		selectedType = 0;
		currentState = start;
		resetAction2();
	}
	else
	{
		selectedTiming = 0;
		nextAction();
	}
	return 0;
}

// programming the coffee brew time
int selectTimingAction() {
	char buffer [16];
	selectedTiming++;
	setLight(selectedTiming % 2 + 1);
	sprintf(buffer, "time: %d", selectedTiming);
	LCD_program_print((unsigned char*)buffer, 5 - timer_counter / 100);
	return 0;
}

//display confirm meeege when done programming
int playConfirmAction() {
	if (selectedTiming!=0){
	timingSetting [selectedType] = selectedTiming;
	char buffer[16];
	sprintf(buffer, "%.4s set to %d",type_str[selectedType], timingSetting [selectedType]);
	LCD_print((unsigned char*)buffer,NULL);
	count_down_timer = 0;
	while( count_down_timer <= 300){
		if (((count_down_timer/50)+1)%2)
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
		else{
			GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		
		}
	}
}
	else{ //handle time not select or time == 0 error
		LCD_print((unsigned char*) "ERR: NO TIM",NULL);
		int i;
		for ( i = 0 ; i <  10000000 * 2; i++) {
			if ( i % 1000000 == 0 )
				GPIO_ToggleBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	}
}
	// clean up the rest 
	selectedType = 0;
	selectedTiming = 0;
	wait(50);
	resetAction2();
	return 0;
}

// state function for brew coffee
int preBrewAction() {
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	countDown = timingSetting[selectedType] * 100;
	id = (id+1)%100;
	
	coffeeTask_t *currentCoffeeTask  = (coffeeTask_t *)malloc( sizeof(coffeeTask_t) );
	
	//coffee stack counter must avoid race condition
	taskENTER_CRITICAL();
	coffeeCount = coffeeCount >= MAX_COOFFEE_STACK-1? MAX_COOFFEE_STACK-1:coffeeCount+1;
	taskEXIT_CRITICAL();
	
	char buffer [17];
	if (coffeeCount<10){
	generateNewCoffee(currentCoffeeTask, id,selectedType-1, timingSetting);
	makeCoffeeTask(currentCoffeeTask);
		//may be need multipule count_down
	count_down_timer = 0;
	selectedType = 0;
	sprintf(buffer, "%s E:%d M:%d C:%d", currentCoffeeTask->aka,currentCoffeeTask->espressoTime, currentCoffeeTask->milkTime, currentCoffeeTask->chocolateMilkTime);

	}
	else {
		sprintf(buffer, "Reached Max Num");
	}
		//LCD print is not thread save have to enter critical section
	taskENTER_CRITICAL();
	LCD_print((unsigned char*) buffer,NULL);
	taskEXIT_CRITICAL();
	
	for (int i =0; i<20000000; i++){};
	

	

	return 0;
}

//brew the coffee
int brewAction() {
    
	int temp = count_down_timer / 10 % 10;

	if (temp < 5)
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
	else
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	if (count_down_timer >= countDown){	//when done, play the sound 
		currentState = reset;
		frequency = nokia[0];
	}
	int remain_sec = countDown - count_down_timer;
	char buffer [17];
	sprintf(buffer, "In: %02d.%-9d",remain_sec/100,remain_sec/10%10);
	int remaining = count_down_timer*10/countDown/2;
	int i;
	for ( i = remaining; i>=0; i--){
		buffer[10+i] = 0xFF;
	}
	LCD_print(NULL,(unsigned char *) buffer);
	return 0;
}

int postBrewAction() {
	return 0;
}

int resetAction() {

	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    //GPIO_SetBits(GPIOD, GPIO_Pin_14);
	sampleCounter = 0;
	selectedType = 0;
	selectedAction = 0;
	sleepTime = 10;
	sawWave = 0.0;
	filteredSaw = 0.0;
	timeInterrupt = 0;
	button_status = NO_CLICK;
	//uncomment after fix sound and servo conflict
	//codec_ctrl_init();
	return 0;
}

int resetAction2() {

	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	return 0;
}

int activeAction() {
	char buffer [16];
	selectTypeAction();
	sprintf(buffer, "%s", coffees[selectedType-1].name);
	LCD_print((unsigned char *)buffer,NULL);

	return 0;

}
int noActiveAction() {
	return 0;
}

int resetTypeAction() {
	selectedType = 0;
	nextAction();
	return 0;
}

// the core function for FSM
int processAction ( ButtonAction buttomState, MachineStates machineStates ) {
	stateElement currentStateStruct = stateMatrix[machineStates][button_status];
	button_status = NO_CLICK;
	currentState = currentStateStruct.nextStatus;
	return state[currentStateStruct.nextAction]();
}

//define the action of the light
void setLight(int count) {
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	if (count == 0)
	{}
else if (count == 1)
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
else if (count == 2)
	GPIO_SetBits(GPIOD, GPIO_Pin_14 );
else if (count == 3)
	GPIO_SetBits(GPIOD, GPIO_Pin_15);
else if (count == 4)
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
else if (count == 5)
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13);
else if (count == 6)
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_14);
else if (count == 7)
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_15);
else if (count == 8)
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
else
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}




// functions adaped from course note
void InitLEDs()
{
	GPIO_InitTypeDef GPIO_Initstructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_Initstructure);
}


// a very crude FIR lowpass filter
float updateFilter(fir_8* filt, float val)
{
	uint16_t valIndex;
	uint16_t paramIndex;
	float outval = 0.0;

	valIndex = filt->currIndex;
	filt->tabs[valIndex] = val;

	for (paramIndex = 0; paramIndex < 8; paramIndex++)
	{
		outval += (filt->params[paramIndex]) * (filt->tabs[(valIndex + paramIndex) & 0x07]);
	}

	valIndex++;
	valIndex &= 0x07;

	filt->currIndex = valIndex;

	return outval;
}

void initFilter(fir_8* theFilter)
{
	uint8_t i;

	theFilter->currIndex = 0;

	for (i = 0; i < 8; i++)
		theFilter->tabs[i] = 0.0;
	
	theFilter->params[0] = 0.32;
	theFilter->params[1] = 0.32;
	theFilter->params[2] = 0.32;
	theFilter->params[3] = 0.32;
	theFilter->params[4] = 0.32;
	theFilter->params[5] = 0.32;
	theFilter->params[6] = 0.32;
	theFilter->params[7] = 0.32;
}


//TODO:fix the sound and servo conflict

//play music
int processSound() {
	
//	LCD_print(NULL, (unsigned char *)"Enjoy!" );
	int newSampleCounter =0;
	while (newSampleCounter <300000){
	
	if (SPI_I2S_GetFlagStatus(CODEC_I2S, SPI_I2S_FLAG_TXE))
	{
		SPI_I2S_SendData(CODEC_I2S, sample);
				if (newSampleCounter % nextCount == 0){
					currentIndex = (currentIndex+1)%length ;
					frequency = nokia[ currentIndex ];
				}
        //only update on every second sample to insure that L & R ch. have the same sample value
		if (newSampleCounter & 0x00000001)
		{
			sawWave += frequency;
			if (sawWave > 1.0)
				sawWave -= 2.0;

			filteredSaw = updateFilter(&filt, sawWave);
			sample = (int16_t)(NOTEAMPLITUDE * filteredSaw);
		}
		newSampleCounter++;
	}

	if ( newSampleCounter % 10000 == 0 )
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
}
	if ( newSampleCounter == 300000) {

		currentState = start;
		GPIO_ResetBits(GPIOD, GPIO_Pin_4);
		enableSound = 0;
	}

	
	//
	currentState = start;
	return 0;
}
/*
int main () {
	currentState = INITIAL_STATE;

	//initialize all required modules
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

  //iniitalize sound
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	codec_init();
	codec_ctrl_init();
	I2S_Cmd(CODEC_I2S, ENABLE);
	initFilter(&filt);

	int prev_State = NULL;
	while (1) {

		processAction(button_status, currentState);
		if (currentState != prev_State) {
			LCD_print((unsigned char*)state_str[currentState], NULL);
			prev_State = currentState;
		}
	}
	return 0;
}
*/