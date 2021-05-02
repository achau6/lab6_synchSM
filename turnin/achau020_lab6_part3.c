/*	Author: lab6
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 * 	Demo Link: https://youtu.be/1W2Y5VGUhG4
 */
#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int count = 7;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
enum states { Start, Init, Inc, Dec, Reset } state;
void TimerISR(){ TimerFlag = 1; }
void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff(){
	TCCR1B = 0x00;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void Tick(){
	unsigned char button = ~PINA & 0x03;
	switch(state) {
		case Start:
			state = Init;
		break;

		case Init:
			if(button == 0x01){
				state = Inc;
			} else if(button == 0x02){
				state = Dec;
			} else if(button == 0x03){
				state = Reset;
			} else {
				state = Init;
			}	
		break;

		case Inc:	
			if(button == 0x01){
				if(count < 9) {
					state = Inc;
				}
			} else {
				state = Init;
			}
		break;
	
		case Dec:
			if(button == 0x02){
				if(count != 0) {
                                	state = Dec;
				}
                        } else {
				state = Init;
                        }
		break;

		case Reset:
			//if((button == 0x03)){
			//	state = Reset;
			//} else {
				state = Init;	
			//}
		break;
		
		default:
		break;
	}

	switch(state) {
		case Inc:
			if((button == 0x01) && (count < 9)){
				count ++;
			} 
		break;
	
		case Dec:
			if((button == 0x02) && (count != 0)){
                                count --;
                        }
		break;
		case Init:
		break;
		case Reset:
			count = 0;
		break;
		default:
		break;

	}
	PORTB = count;

}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRB = 0x00; PORTA = 0xFF;
	TimerSet(100);
	TimerOn();
	state = Start;
    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag);
	TimerFlag = 0;
	/*if((~PINA & 0x01) == 0x01){
		PORTB = 0x07;
	} else {
		PORTB = 0x00;
	}*/
	
    }
    return 1;
}
