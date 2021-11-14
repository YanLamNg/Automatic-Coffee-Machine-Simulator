/* COMP 4450
	Authors:
		Li Borui 7779844
		YanLam Ng 7775665
	Group: 5  
*/


#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include "lcd.h"

unsigned char BF;
unsigned char blank[20] = {"                   "};


void LCD_Init(void){
		GPIO_InitTypeDef GPIO_Initstructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
    GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_Initstructure);
    //Setup LCD Pins
  

    BitClr(LCD_E);                //clear enable
    BitClr(LCD_RS);               //going to write command

    DelayMS(30);                //delay for LCD to initialise.
    LCD_NYB(0x30,0);              //Required for initialisation
    DelayMS(5);                 //required delay
    LCD_NYB(0x30,0);              //Required for initialisation
    DelayMS(1);                 //required delay
    LCD_DATA(0x02,0);           //set to 4 bit interface, 1 line and 5*7 font
    LCD_DATA(0x28,0);           //set to 4 bit interface, 2 line and 5*10 font
    LCD_DATA(0x0c,0);           //set to 4 bit interface, 2 line and 5*7 font
    LCD_DATA(0x01,0);           //clear display
    LCD_DATA(0x06,0);           //move cursor right after write

}

void LCD_Enable(){
}
void LCD_Disable(){
}
//--------------------------------------------------------------------------------//
void LCD_DATA(unsigned char data,unsigned char type){
    
    WaitLCDBusy();                  //TEST LCD FOR BUSY 

    if(type == CMD){
        BitClr(LCD_RS);             //COMMAND MODE
    } else {
        BitSet(LCD_RS);             //CHARACTER/DATA MODE
    }

    LCD_NYB(data>>4,type);               //WRITE THE UPPER NIBBLE
    LCD_NYB(data,type);                  //WRITE THE LOWER NIBBLE
}
//--------------------------------------------------------------------------------//
void WaitLCDBusy(void){
    DelayMS(2);              //DELAY 1 MilliSeconds
}
//--------------------------------------------------------------------------------//
void LCD_NYB(unsigned char nyb,unsigned char type){
    //SEND DATA LINE THE INFO 
    LCD_PORT->BSRRH |= 0x0F;
    LCD_PORT->BSRRL |= (nyb & 0x0F);

    if(type == CMD){
        BitClr(LCD_RS);             //COMMAND MODE
    } else {
        BitSet(LCD_RS);             //CHARACTER/DATA MODE
    }

    BitSet(LCD_E);         //ENABLE LCD DATA LINE
    DelayMS(1);                 //SMALL DELAY
    BitClr(LCD_E);         //DISABLE LCD DATA LINE
}
//--------------------------------------------------------------------------------//
void LCD_STR(unsigned char *text, char clear){
unsigned char x=0;
    while(*text)
	{
        LCD_DATA(*text++,1);
		x++;
    }
	if(clear == 1)
	{
		while(x<16)
		{
			LCD_DATA(0x20,1);
			x++;
		}
	}
}
//--------------------------------------------------------------------------------//
void LCD_LINE(char line){
    switch(line){
        case 0:
        case 1:
            LCD_DATA(LINE1,0);
            break;
        case 2:
            LCD_DATA(LINE2,0);
            break;
    }
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
void DelayMS(unsigned int ms){
    unsigned int x,i;
    for(x=0;x<ms;x++)
    {
        for(i=0;i<16400;i++);
    }
}
//--------------------------------------------------------------------------------//
void LCD_POS(unsigned char line){
unsigned char myDat;
unsigned char pos = 18;



    LCD_LINE(line);
    switch(line){
        case 0:
        case 1:
            myDat = LINE1 + pos;
            break;
        case 2:
            myDat = LINE2 + pos;
            break;

    }

    myDat |= 0x80;
    LCD_DATA(myDat,0);
}

void LCD_CLEAR(){ 
	LCD_NYB(LCD_CLEARDISPLAY,0);
	DelayMS(1); 
	LCD_NYB(LCD_RETURNHOME,0);
}


void LCD_print(unsigned char* line_1,unsigned char* line_2){
	LCD_CLEAR();
	if(line_1!=NULL){
		LCD_LINE(1);
		LCD_STR(line_1,1);
	}
	if(line_2!=NULL){
		LCD_LINE(2);
		LCD_STR(line_2,1);
	}
}

/*
void LCD_clear();
void LCD_clear_row(int line);    //void LCD_CLEAR(unsigned char line);

void LCD_print_row(int row, unsigned char* line);*/
