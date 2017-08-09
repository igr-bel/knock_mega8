/*
 * knock_mega8.c
 *
 * Created: 14.06.2017 13:34:16
 *  Author: igr
 */ 

//-------------------------------------------------------------------------------------------------------------------------------------------------
#include "prj.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------
//#define DBG
//#define DBG_MEM
#define w_voice
//-------------------------------------------------------------------------------------------------------------------------------------------------
#define knock_in	(!(PINB & (1<<2)))
#define btn_pressed	(!(PIND & (1<<6)))
#define magnet_on	(PORTC |= (1<<1))
#define magnet_off	(PORTC &= ~(1<<1))
#define DF_busy		(!(PIND & (1<<5)))

#define small_pause	1
#define big_pause	11
#define sequence_length	15

#define reset_pause	2000

//-------------------------------------------------------------------------------------------------------------------------------------------------
//---prototypes
inline void prog(void);

//---variables
unsigned long start_time;

unsigned char cntr = 0;
unsigned char prog_array[sequence_length];

//---DF player commands
unsigned char data_buff_0[8] = {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xEF};	//SD card
unsigned char data_buff_1[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x01, 0xEF};	//play 1
unsigned char data_buff_2[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x02, 0xEF};	//play 2
unsigned char data_buff_3[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x03, 0xEF};	//play 3
unsigned char data_buff_4[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x04, 0xEF};	//play 4
unsigned char data_buff_5[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x05, 0xEF};	//play 5	
//----
unsigned char data_buff_6[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x08, 0xEF};	//play 10 - 24 --> "length of passwd"
//----	
unsigned char data_buff_7[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x07, 0xEF};	//play 7      
unsigned char data_buff_8[8] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x08, 0xEF};	//play 8
//---flags
unsigned char error_flag;
unsigned char game_state_flag;

//=================================================================================================================================================
int main(void)
{
	DDRC |= (1<<1);		//magnet pin PC1
	DDRB &= ~(1<<2);	//PB2 - sensor in
	
	DDRD &= ~(1<<6);	//PD6 - button pin
	PORTD |= (1<<6);	///pull_up
//---------------------------------		
	DDRD &= ~(1<<5);	//DF busy
//---------------------------------	
	DDRB &= ~( (1<<0) | (1<<1) );	//unused pins
//---------------------------------
	//UART init
	uart_init(MYUBRR);
//---------------------------------			
	//DF init --> read from SD-card
	_delay_ms(3000);
	for (int i=0; i<8; i++)
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = data_buff_0[i];
	}
	_delay_ms(1000);
//---------------------------------	
	millis(on);
//---------------------------------		
	if(btn_pressed) prog();
//---------------------------------
	magnet_on;
//---------------------------------		
	//EEPROM read
		while(EECR & (1 << EEWE));
		EEAR = 0;
		EECR |= (1<<EERE);
		cntr = EEDR;
				
#ifdef DBG_MEM
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = cntr;
	_delay_ms(200);
#endif
		
		for (int i=0; i<cntr; i++)
		{
			while(EECR & (1 << EEWE));
			EEAR = i+1;
			EECR |= (1<<EERE);
			prog_array[i] = EEDR;
			
#ifdef DBG_MEM
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = prog_array[i];
	_delay_ms(100);
#endif
		}//for
_delay_ms(1000);
//---------------------------------
	unsigned char game_cntr = 0;	
	unsigned long game_pause_array[sequence_length];
	unsigned long game_min_pause;
	
	error_flag = 0;
	game_state_flag = 0;
	game_min_pause = 0;
	
	//voice "system ready"
	for (int i=0; i<8; i++)
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = data_buff_1[i];
	}
	_delay_ms(200);
	while(DF_busy);
//---------------------------------
	while (1)
	{
		while(!knock_in);
		start_time = millis(get);
		
		while((2*2) != 5)
		{
			if (knock_in)
			{
				if ( (millis(get) - start_time) >= 50 )
				{
					#ifdef DBG
						while ( !( UCSRA & (1<<UDRE)) );
						UDR = game_cntr;
					#endif
				
					game_pause_array[game_cntr] = (millis(get) - start_time);
					#ifdef DBG
						while ( !( UCSRA & (1<<UDRE)) );
						UDR = game_pause_array[game_cntr];
					#endif
				
					game_cntr++;
					while(knock_in);
					start_time = millis(get);
				}//if
				while ( knock_in && (0 == game_cntr) );
			}//if(knock_in)
			if( (game_cntr > sequence_length) || ((millis(get) - start_time) > reset_pause) ) break;
		}//while(2*2 != 5) 
	//---------------------------------		
		if ( (millis(get) - start_time) > reset_pause )
		{
			if (game_cntr == cntr)
			{
				//---------------------------------
				game_min_pause = game_pause_array[0];
				for (int i=0; i<game_cntr; i++)
				{
					game_min_pause = (game_min_pause <= game_pause_array[i]) ? game_min_pause : game_pause_array[i] ;
				}//for
				//---------------------------------
				for (int i=0; i<game_cntr; i++)
				{
					game_pause_array[i] = ( ((game_pause_array[i] / game_min_pause) > 2) || ((game_pause_array[i] % game_min_pause) > (game_min_pause/2)) ) ? big_pause : small_pause ;
				}//for
				//---------------------------------
				for (int i=0; i<game_cntr; i++)
				{
					if (game_pause_array[i] != prog_array[i]) error_flag = 1;
				}//for
				//---------------------------------
				if (!error_flag)
				{
					if (!game_state_flag)
					{
						magnet_off;
						game_state_flag = 1;
						error_flag = 0;
						#ifdef w_voice
							//voice "door opening"
							for (int i=0; i<8; i++)
							{
								while ( !( UCSRA & (1<<UDRE)) );
								UDR = data_buff_7[i];
							}
							_delay_ms(200);
							while(DF_busy);
						#endif
					}
					else
					{
						magnet_on;
						game_state_flag = 0;
						error_flag = 0;
						
						#ifdef w_voice
							//voice "closing the door"
							for (int i=0; i<8; i++)
							{
								while ( !( UCSRA & (1<<UDRE)) );
								UDR = data_buff_8[i];
							}
							_delay_ms(200);
							while(DF_busy);					
						#endif
					}
				}//if(!error)
				//---------------------------------
			}//if( cntr == )
						
		}//if(check result and reset)
	//---------------------------------
		for (int i=0; i<game_cntr; i++)
		{
			game_pause_array[i] = 0;
		}
		error_flag = 0;
		game_cntr = 0;
	}//while(1)
}//main()

//=================================================================================================================================================
//---functions
//-------------------------------------------------------------------------------------------------------------------------------------------------
inline void prog(void)
{
	unsigned long pause_array[sequence_length];
	unsigned long min_pause = 0;
	
	//voice "input new password"
	for (int i=0; i<8; i++)
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = data_buff_2[i];
	}
	_delay_ms(200);
	while(DF_busy);
	
	while(btn_pressed);
	
//---------------------------------
	millis(on);
//---------------------------------
	start_time = millis(get);	
	while(!knock_in)
	{
		if ( (millis(get) - start_time) > 10000 )
		{
			//voice "wrong passwd"
			for (int i=0; i<8; i++)
			{
				while ( !( UCSRA & (1<<UDRE)) );
				UDR = data_buff_5[i];
			}
			_delay_ms(200);
			while(DF_busy);
			return;
		}//if
	}//while(!knock)
	start_time = millis(get);	

	for(;;)
	{
		if (knock_in)
		{
			if ( (millis(get) - start_time) >= 50 )
			{
				pause_array[cntr] = (millis(get) - start_time);
				cntr++;
				while (knock_in);
				start_time = millis(get);
			}//if
			while ( knock_in && (0 == cntr) );	//1st knock		
		}//if(knock_in)
		
		if( (cntr > sequence_length) || ((millis(get) - start_time) > 10000) ) break;
		
	}//for(;;)
//---------------------------------	
	min_pause = pause_array[0];
	for (int i=0; i<cntr; i++)
	{
		min_pause = (min_pause <= pause_array[i]) ? min_pause : pause_array[i];
	}//for
//---------------------------------	
	for (int i=0; i<cntr; i++)
	{
		prog_array[i] = ( ((pause_array[i] / min_pause) > 2) || ((pause_array[i] % min_pause) > (min_pause/2)) ) ? big_pause : small_pause ;
				while ( !( UCSRA & (1<<UDRE)) );
				UDR = prog_array[i];
	}//for
//---------------------------------
	if ( (min_pause < 100) || (cntr < 1) || (cntr > sequence_length) )
	{
		//voice "wrong passwd"
		for (int i=0; i<8; i++)
		{
			while ( !( UCSRA & (1<<UDRE)) );
			UDR = data_buff_5[i];
		}
		_delay_ms(200);
		while(DF_busy);
		return;
	}//if	
//---------------------------------
	//voice "length of passwd"
	data_buff_6[6] += (cntr + 0x01); 
	for (int i=0; i<8; i++)
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = data_buff_6[i];
	}
	_delay_ms(200);
	while(DF_busy);
//---------------------------------
	//voice "new passwd is set"
	for (int i=0; i<8; i++)
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = data_buff_3[i];
	}
	_delay_ms(200);
	while(DF_busy);
//---------------------------------
	//save cntr
	while(EECR & (1 << EEWE));
	EEAR = 0;
	EEDR = cntr;
	EECR |= (1<<EEMWE);
	EECR |=(1<<EEWE);
	
	//save prog_array[]
	for (int i=0; i<cntr; i++)
	{
		while(EECR & (1 << EEWE));
		EEAR = i+1;
		EEDR = prog_array[i];
		EECR |= (1<<EEMWE);
		EECR |=(1<<EEWE);		
	}//for
	
	return;
}//prog()

//-------------------------------------------------------------------------------------------------------------------------------------------------

//=================================================================================================================================================