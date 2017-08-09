/*
 * uart.c
 *
 * Created: 26.04.2017 13:37:39
 *  Author: igr
 */ 

#include "prj.h"

volatile int *data_length_pointer;
volatile unsigned char *data_start_pointer;

ISR(USART_UDRE_vect)
{	
	if( (*data_length_pointer) > 0 )
	{
		UDR = *data_start_pointer;
		data_start_pointer = data_start_pointer + 1;
		*data_length_pointer = (*data_length_pointer) - 1;
	}

	
	if ( 0 == (*data_length_pointer) )
	{
		UCSRB &= ~(1<<UDRIE);	//disable UDRE interrupt

	}//if transmit completed

}//ISR()

//------------------------------------------------------------------------------------------------------------------------
void uart_init( unsigned int ubrr)
{
	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	
	/* Enable receiver and transmitter */
	UCSRB = ( (1<<RXEN)|(1<<TXEN)|(0<<RXCIE)|(0<<TXCIE) );
	
	/* Set frame format: 8 data, 1 stop bit */
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	
	sei();
		
	return;
}//uart_init

//------------------------------------------------------------------------------------------------------------------------
void uart_tx_byte( unsigned char data )
{
	/* wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	
	/* put data into buffer --> sends the data */
	UDR = data;
	
	return;
}//uart_tx()

//------------------------------------------------------------------------------------------------------------------------
unsigned char uart_rx_byte( void )
{
	/* wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	
	/* return received data from buffer */
	return UDR;

}//uart_rx_byte()

//------------------------------------------------------------------------------------------------------------------------
void uart_tx_buffer( unsigned char *data_pointer, int *length_in_bytes )
{
	/*init address of 1st data byte*/
	data_start_pointer = data_pointer;
	
	/*init data length counter*/
	data_length_pointer = length_in_bytes;
	
	/*interrupt init*/
	UCSRB |= (1<<UDRIE);
	
	return;
	
}//uart_tx_buffer()

//------------------------------------------------------------------------------------------------------------------------