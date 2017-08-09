/*
 * uart.h
 *
 * Created: 26.04.2017 13:36:33
 *  Author: igr
 *
 *	need to define BAUD in your code
 *	default BAUD = 9600
 *
 */ 


#ifndef UART_H_
#define UART_H_

	#ifndef BAUD
		#define BAUD 9600
	#endif
	
	#define MYUBRR F_CPU/16/BAUD-1
	
	void uart_init( unsigned int ubrr );
	
	void uart_tx_byte( unsigned char data );
	
	unsigned char uart_rx_byte( void );
	
	void uart_tx_buffer( unsigned char *data_pointer, int *length_in_bytes );



#endif /* UART_H_ */