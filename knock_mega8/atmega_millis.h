/*
 * atmega_millis.h
 *
 * Created: 17.04.2017 15:15:11
 *  Author: igr
 *
 *
 *	unsigned long millis(int state) - return time iin ms from millis(start).
 *
 *							 state: 
 *									on		- turn on TC0
 *									off		- turn off TC0
 *									start	- start write time
 *									stop	- stop erite time, after stop return last time value 
 *									get		- return time from start
 */ 


#ifndef ATMEGA_MILLIS_H_
#define ATMEGA_MILLIS_H_
	
	#define get		100
		
	#define start	11
	#define stop	10
	
	#define on		1
	#define off		0
	
	unsigned long millis(int state);

#endif /* ATMEGA_MILLIS_H_ */