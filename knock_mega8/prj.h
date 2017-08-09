/*
 * prj.h
 *
 * Created: 19.04.2017 11:30:49
 *  Author: igr
 */ 


#ifndef PRJ_H_
#define PRJ_H_

	#define F_CPU 8000000	//int osc

	#include <avr/io.h>
	#include <util/delay.h>
	#include <avr/interrupt.h>
	#include <avr/wdt.h>
	#include <avr/eeprom.h>
	
	#include "uart.h"
//	#include "atmega_adc.h"
	#include "atmega_millis.h"

	#include "knock_mega8.h"
	
	#define on  1
	#define off 0

#endif /* PRJ_H_ */