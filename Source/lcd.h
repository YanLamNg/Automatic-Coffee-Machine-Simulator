/* COMP 4450
	Authors:
		Li Borui 7779844
		YanLam Ng 7775665
	Group: 5  
*/

#ifndef __LCD_H
#define __LCD_H

#include<stdio.h> 

#define LCD_PORT GPIOD    //LCD DATA PORT
/*

PD0 ==> D4
PD1 ==> D5
PD2 ==> D6
PD3 ==> D7
PD7 ==> LCD_RS           
Pd6 ==> LCD_E            

*/
#define LCD_RS   7        //PD7  LCD Command/Data Control
#define LCD_E    6        //PD6  LCD Enable Line

#define CMD 0
#define TXT 1
#define        LINE1    0x80        // Start address of first line
#define        LINE2    0xC0        // Start address of second line

#define BitClr(var) (LCD_PORT->BSRRH |= (1<<var))
#define BitSet(var) (LCD_PORT->BSRRL |= (1<<var))

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

void WaitLCDBusy(void);
void LCD_Init(void);
void LCD_DATA(unsigned char data,unsigned char type);
void LCD_NYB(unsigned char nyb,unsigned char type);
void LCD_LINE(char line);
void ShiftDisplay(unsigned char DC,unsigned char RL);
void DelayMS(unsigned int ms);
void LCD_STR(unsigned char *text, char clear);
void LCD_CLEAR(void);    //void LCD_CLEAR(unsigned char line);
void LCD_print(unsigned char* line_1,unsigned char* line_2);
#endif



/*void LCD_clear();
void LCD_clear_row(int line);    //void LCD_CLEAR(unsigned char line);

void LCD_print_row(int row, unsigned char* line);*/
