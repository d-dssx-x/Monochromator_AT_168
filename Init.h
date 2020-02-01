#ifndef _Init
#define _Init


#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <MATH.H>
#include <interrupt.h>
#include "Step_motor.h"
#include <wdt.h>

//*********************************************************

#define	Milli_sec 1000
#define	COE_ADC 2.56/1024
#define EN_Tim_3 ETIMSK = 0x04;
#define EN_Tim_1_Comp_1_2	TIMSK |=0x18;
#define EN_Tim_1_Comp_3	ETIMSK |=0x01;
#define Start_Tim_1 TCCR1B = 0x02;TCNT1 = 0x0000;TIMSK |=0x18;
#define Stop_Tim_1  TCCR1B = 0x00;TCNT1 = 0x0000;TIMSK &=0xE7;
#define Start_Ar_ctr TCCR3B = 0x04;
#define Stop_Ar_ctr  TCCR3B = 0x00;
//*********************************************************
#define buf_size 128
//*********************************************************
#define SetBit(x,y) (x|=(1<<y))
#define ClrBit(x,y) (x&=~(1<<y))
#define ToggleBit(x,y) (x^=(1<<y))
#define FlipBit(x,y) (x^=(1<<y)) // Same as ToggleBit.
#define TestBit(x,y) (x&_BV(y))
#define Ext_Mem(adr) (*(volatile char*)(adr))

//*********************************************************
//declare memory mapped variables
//*********************************************************
//typedef union  { unsigned int ival; unsigned char db[2];} Point;
//*******************************************************************************
extern volatile unsigned char 	 Tx_ready;
extern volatile unsigned char 	 Rx_ready;
extern   char 			Data_buff_RS[buf_size];//extern 
extern   char 			Command_buff[buf_size];
extern volatile unsigned char 	ComNumber;//extern volatile unsigned
extern volatile unsigned char 	eol;//extern volatile unsigned 
extern volatile unsigned char 	buff_byte;//extern 
extern volatile unsigned char 	Running;//extern 
extern volatile unsigned char 	Ext_Running;//extern 
//*******************************************************************************
extern volatile unsigned char	Flag_time;
extern volatile unsigned int 	Timer;

//*********************************************************


#define  SW_LEFT1 	PIND3
#define  SW_RIGTH1	PIND4
#define  DIR		PIND5
#define  HOME		PIND6
#define  STEP		PIND7

#define  ENABLE		PINB0
#define  SLEEP		PINB1
#define  RESET_SM   PINB2
#define  MOSI    	PINB3
#define  MISO    	PINB4
#define  SCK    	PINB5
#define  MS1		PINB6
#define  MS2		PINB7

void port_init(void);
void delay(unsigned int time_delay);
void init_devices(void);
void TestReset(char test);

#endif
