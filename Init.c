//#include <inttypes.h>
//#include <avr/io.h>
#include "Init.h"

//*******************************************************************************
volatile unsigned char 	 Tx_ready;
volatile unsigned char 	 Rx_ready;
char 			Data_buff_RS[buf_size];//extern 
char 			Command_buff[buf_size];
volatile unsigned char 	ComNumber;//extern volatile unsigned
volatile unsigned char 	eol;//extern volatile unsigned 
volatile unsigned char 	buff_byte;//extern 
volatile unsigned char 	Running;//extern 
volatile unsigned char 	Ext_Running;
//*******************************************************************************
volatile unsigned char	Flag_time;
volatile unsigned int 	Timer;

//*********************************************************
void port_init(void)
{
	DDRB  = 0xEF;
	PORTB = 0x3F;
 
	DDRC  = 0x00;
	PORTC = 0xFF;
 
	DDRD  = 0xA2;//************************
	PORTD = 0xFF;//************************
}
//*********************************************************
void WDT_init(void)
{
	//WDTCSR=0x01;
	MCUSR &=0x08;	
	WDTCSR|=0x10;
	WDTCSR=0x00;
}
//*********************************************************
void timer0_init(void)
{
	TCCR0A = 0x00; //stop//0x04
	TCNT0 = 0xE0; //set count
	TCCR0A = 0x00;//0x02; //start timer, frequency = 0Hz
}
//*********************************************************
void delay(unsigned int time_delay)
{
	Timer=time_delay;
	TCNT0 = 0xE0;
	TCCR0B = 0x04; //start//0x04
	TIMSK0 |=0x01; //start timer0 //start Timer, frequency = 1000Hz
	Flag_time=1;
	while (Flag_time==1){};
	TIMSK0 &= 0xFE;
	TCCR0B = 0x00; //stop Timer, frequency = 0Hz
}
//*********************************************************
void timer1_init(void)
{
	 TCCR1B = 0x00; //stop
	 TCNT1 = 0x7F0C; //setup
//	 OCR1A = 0xFFCC;
//	 OCR1B = 0x01CC;
	 TCCR1A = 0x00;
	 TCCR1B = 0x00; //start 0x05 - 16000000, 0x04 - 8000000
}
//*********************************************************
void timer2_init(void)
{
	TCCR2A = 0x00; //stop
	TCNT2 = 0x01; //setup
	TCCR2B = 0x00;//0x05; //start timer 2
}
//*********************************************************
//UART0 initialize
// desired baud rate: 115200
// actual: baud rate:117647 (2,1%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
	UBRR0L = 0x08;//67;//set baud rate lo
	UBRR0H = 0x00;//set baud rate hi	
	UCSR0A = 0x02; 
	UCSR0B = 0x00;//disable while setting baud rate
	UCSR0C = 0x06;
// UCSR0B = 0xF8;//d8
	UCSR0B = 0xd8;
}
//**********************************************************************
void SPI_init(void)
{
	SPCR = 0x50;//disable while setting baud rate 0x53
	SPSR = 0x00;
}
//*****************************************************************
void adc_init(void)
{
	ADMUX = 0xCE; //select adc input 0
	ADCSRA = 0xF7; //disable adc
	ADCSRB &= 0x00; //disable adc
}
//*****************************************************************
void comparator_init(void)
{
	ClrBit(ADCSRB,ACME);
}
//*****************************************************************
void handle_xram(void)
{
//	MCUCR |= _BV(SRE);
//    XMCRA |= _BV(XMBK);
}

//*********************************************************
void TestReset(char test)
{	//printf ("RESET= %X\r\n",test);
	if((test & 0x01)==0x01)printf ("Power ON\r\n");
	if((test & 0x02)==0x02)printf ("Ext RESET\r\n");
	if((test & 0x04)==0x04)printf ("BORN RESET\r\n");
	if((test & 0x08)==0x08)printf ("WDT RESET %X\r\n",WDTCSR);	
}
//*********************************************************
void External_Init(void)
{	//printf ("RESET= %X\r\n",test);
	EICRA = 0x00; //external interrupts control register A   B	
	EIMSK = 0x00;
	PCICR = 0x02;
	PCMSK2 = 0x00;	
	PCMSK1 = 0x03;
	PCMSK0 = 0x00;
	
	
}
//*********************************************************
//call this routine to initialise all peripherals
void init_devices(void)
{   char reset=0x00;
	reset=MCUSR;
	cli(); //disable all interrupt
	MCUCR = 0x00; //C0
	MCUSR=0x00;
	port_init();
	uart0_init();
	timer0_init();
	timer1_init();
	timer2_init();
	adc_init();
	WDT_init();
	comparator_init();
	External_Init();
	sei();
//	OSCCAL = 0x5a;
	CLKPR =0x00;

	TIMSK1 = 0x00; //timer 3 interrupt sources
	TIMSK2 = 0x00; //timer 3 interrupt sources
	sei(); //re-enable interrupts
	TestReset(reset);
 //all peripherals are now initialised
 }



