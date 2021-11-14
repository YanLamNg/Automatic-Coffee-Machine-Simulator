#  Automatic-Coffee-Machine-Simulator




### Introduction

The goal of this Project is to implement FreeRTOS on the STM32F407-Disco1. We implement an automatic coffee machine that capable of executing multiple tasks based on FreeRTOS. All four User LEDs, button, audio output, and PWM (servo motor) of the STM32F407-Disco1 will be utilised. 


The automatic coffee machine is easy to use real-time system (RTS) to help the barista to programme the brewer to brew the coffee at the correct amount of time. The automatic brewing allows the coffee machine to brew three different types of coffee simultaneously (assume all type of coffee have the same priority) using the default preemptive scheduler. E.g. If latte (completion time = 10) is the initial selection, t=0 but the user select mocha (completion time = 10) at t=5; both types of coffee will be scheduled, round-robin.

As you start the program, it will automatically into start state, from there you would be able to entering the programming mode by long press, or single press to enter active mode and select the size for brew. 

	
The programming mode are defined as three sub states: the select size, select time and confirm, as described in class, You could exam if you enter a state successful via examine if there are 3 LEDs are light up, and Once you enter confirm state, the blue light would start blink.
		
After the confirm state you would be bring back to start state. 

On any error or time out you would be bring back to start state

Active mode are defined as three sub states: select size, brew and play sound

As you single click in start state you are entering the active mode, but don't worry, you could Always entering the programing mode via long press in this state.

Then enter the brew state by DOUBLE CLICK, the system would take care from there and carry you to the end and play song, the brew state is interrupt able via long click, while sound state does not.

After sound you would be back to start state  




### LED additional actions:

In start mode:
The Green LED is light up to indicate the program initialized correctly and ready to go.

In programing mode: 
Every time when you enter a new state, you would observed that 3 LEDs are light up.

You will observed Green LED is blinking every second, to remind you the remaining time.
In the select Timing mode, you would observe that the Red and Orange LEDs are alternative lights up as you make single clicks

In brew mode:
The green light would blink every seconds so you could know how long does pass
When playing the music, the Orange LED would blink.




### Button input:
Single click: only one click in 0.5 second
Double click: double click in 0.5 second
Long click: hold down for 1.5 second




### LCD 1602a:
For using LCD, wires connect between stm32f407 to LCD 1602a

stm32f407 GPIO	--> LCD PIN

GPIOD 0        	--> d4

GPIOD 1        	--> d5

GPIOD 2        	--> d6

GPIOD 3        	--> d7

GPIOD 6        	--> E

GPIOD 7        	--> RS

Gound          	--> VSS, RW, K

5v             	--> VCC, A

10k variable resistor   --> v0
	
	
