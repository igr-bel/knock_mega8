/*
 * atmega_millis.c
 *
 * Created: 17.04.2017 15:14:13
 *  Author: igr
 */ 
#ifndef ATMEGA_MILLIS_C_
#define ATMEGA_MILLIS_C_


	#include "atmega_millis.h"
	#include <avr/io.h>
	#include <avr/interrupt.h>

	#define F_CPU 16000000
	
	unsigned long counter_ms = 0;
	unsigned long counter_tics = 0;
	unsigned long storage_ms = 0;		//if counter stop
	
	char stop_flag = 0;
	
	ISR( TIMER0_OVF_vect )
	{
		counter_tics++;				
	}//ISR
	
	unsigned long millis(int state)
	{
		switch (state)
		{
			case get:
				if(0 == stop_flag) return (counter_tics / 4);
				else return storage_ms;
			break;
			
			case start:
				TCNT0 = 0x00;
				counter_tics = 0;
				stop_flag = 0;
				return 0;
			break;
			
			case stop:
				storage_ms = counter_tics / 4;
				stop_flag = 1;
				return 0;
			break;
			
			case on:
				SREG |= (1<<7);			//global interrupt enable
				TIMSK |= (1<<TOIE0);		//interrup TC0 enable
				TCCR0 |= (1<<CS01);		//prescaler 8
				stop_flag = 0;
				counter_tics = 0;
				return 0;
			break;

			case off:
				TIMSK &= ~(1<<TOIE0);
				stop_flag = 1;
				counter_tics = 0;
				return 0;
			break;			

			default :
				return 1;
			break;
			
		}//switch()
	}//milis()

#endif /* ATMEGA_MILLIS_C_ */
