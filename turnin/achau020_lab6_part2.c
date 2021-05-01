/*	Author: lab6
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

bool paused = false;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
enum states { start, init, change, pause } state;
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
	switch(state) {
		case start:
			state = init;
		break;
		case init:
			state = change;
		break;
		case pause:
			if((~PINA & 0x01)){
				state = change;
			} else {
				state = pause;
			}
		break;
		case change:
			if((~PINA & 0x01) == 0x01){
				state = pause;
				paused = true;
			} else {
				state = change;
			}
		break;
		default: 
			state = start;
		break;
	}

	switch(state){
		case start:
		break;
		case init:
			PORTB = 0x01;
		break;
		case change:
			if(paused == false) {
				if(PORTB == 0x04){
					PORTB = 0x00;
				} else if(PORTB == 0x00){
					PORTB = 0x01;	
				} else {
					PORTB = PORTB << 1;
				}
			} else {
				PORTB = 0x00;
				paused = false;
			}
		break;
		case pause:
			//PORTB = 0x07;
		break;
		default:
		break;
	}

}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRB = 0x00; PORTA = 0xFF;
	TimerSet(300);
	TimerOn();
	state = start;
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
