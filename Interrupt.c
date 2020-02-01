#include "interrupt.h"
#include "Step_motor.h"
#include <avr/io.h>
#include "Init.h"
//*********************************************************
ISR (INT0_vect,)//external interrupt on INT0
{
	printf ("Ext_Int_0 %x\r\n",PCIFR);
}
//*********************************************************
ISR (INT1_vect,)//external interrupt on INT1
{
	printf ("Ext_Int_1 %x\r\n",PCIFR);
}
//*********************************************************
ISR (PCINT0_vect,)//external interrupt on INT0
{
	printf ("Ext_Int_00 %x\r\n",PCIFR);
}
//*********************************************************
ISR (PCINT1_vect,)//external interrupt on INT1
{char test=0;
	test=PINC&0x03;
	//printf ("=deb %c,%c,0x%02x\r\n",Running,Status,PIND);
	Ext_Running='N';
	if (test==0x02 && (Status=='Y'||Status=='R')){Ext_Running='R';}//printf ("Start Left\r\n");
	if (test==0x01 && (Status=='Y'||Status=='L')){Ext_Running='L';};//printf ("Start Right\r\n");
	if (test==0x03 && Status=='B'){Running='A';Ext_Running='N';}//printf ("Stop moving %c,%c\r\n",Running,Status);
}
//*********************************************************
ISR (PCINT2_vect,)//external interrupt on INT1
{
	printf ("Ext_Int_22\r\n");
}
//***********************************************************************************
ISR (TIMER0_OVF_vect,)
{
	--Timer;
	if(Timer==0){Flag_time=0;TCCR0B = 0x00;}//
	TCNT0 = 0xE0;
	return;
}
//***********************************************************************************
ISR (TIMER1_OVF_vect,)
{

	TCNT1 =T_Speed;
	
	if(((PIND&Left_SW_test)==0) && Direction==0){Status='L';TCCR1B=0x0000;TCCR2B = 0x00;return;}//SetBit(PORTB,ENABLE);
	if(((PIND&Rigth_SW_test)==0) && Direction==1){Status='R';TCCR1B=0x0000;TCCR2B = 0x00;return;}//SetBit(PORTB,ENABLE);
	
	if(Work_counter<=0){Status='Y';TCCR1B=0x00;TCCR2B = 0x00;SetBit(PORTB,ENABLE);return;}//
	else if(Status=='E'&& (Speed<=Work_Parameters.Speed_min)){Status='Y';TCCR1B=0x00;TCCR2B = 0x00;SetBit(PORTB,ENABLE);return;}//	
	if(Direction==0)Coordinate--;
	else Coordinate++;
	Work_counter--;
	Up_down_speed--;

	ClrBit(PORTD,STEP);
	SetBit(PORTD,STEP);
	Calibr_counter++;
//	FlipBit(PORTD,STEP);
	
	return;
}
//***********************************************************************************
ISR (TIMER2_OVF_vect,)
{

	TCNT2 = T_acselerate; //0xc8 Reload timer
	if (Up_down_speed>0)Speed++;
	else if (Up_down_speed<=0)Speed--;

	if(Speed<Work_Parameters.Speed_min)T_Speed=T_Speed_min;//
	else if(Speed>=Work_Parameters.Speed_max)T_Speed=T_Speed_max;//
	else T_Speed=(0xffff-TIMERCLK_3/Speed);

	if(Running=='A' )
	{
		Speed = TIMERCLK_3/(0xffff-T_Speed);			
		Up_down_speed=0;
		if(Speed>=Work_Parameters.Speed_max)Speed=Work_Parameters.Speed_max;
		Running='I';Status='E';
	}
	return; 
}
//***********************************************************************************
ISR (TIMER1_COMPA_vect,)// Frequency control
{
	return;
}
ISR (TIMER1_COMPB_vect,)//Arc control
{	
	return;
}
//*****************************************************************
/*
ISR (TIMER2_OVF_vect,)
{
	return;
}
*/
//*****************************************************************
ISR (USART_RX_vect,)
{
unsigned  char rs_in=0x00 ;
		 rs_in = UDR0;Rx_ready='1';
		 rs_in =toupper(rs_in);

		if (rs_in==0x1B) {WDTCSR|=0x18;}//{WDTCSR&=0xE7;}//ESCWDTCSR|=0x08;wdt_enable(1);
		if (rs_in==0x0d) eol|=0x01;
		if (rs_in==0x0a) eol|=0x02;
		if (rs_in==0x08) buff_byte--;

		if (eol==0x03)
		{
			
			if (buff_byte>0)
		   {
			if (Data_buff_RS[0]=='S'&& Data_buff_RS[1]=='T'&& Data_buff_RS[2]=='O'&& Data_buff_RS[3]=='P')
				{Running='A';}
			memset(Command_buff,0,sizeof(Command_buff));
			memcpy(Command_buff,Data_buff_RS,buff_byte);
			memset(Data_buff_RS,0,sizeof(Data_buff_RS));
			Rx_ready='0';
			buff_byte=0;
			eol=0x00;
			
			}
			else {eol=0x00;return;}
		}
		else if(isgraph(rs_in))
		{
		  	if (buff_byte < sizeof (Data_buff_RS))
				{
	 			   Data_buff_RS[buff_byte++]= rs_in;
				}
		  	else	buff_byte = 0;
		}
//		else;
		

		
		return;
}
//*****************************************************************
ISR (USART_UDRE_vect,)
{
 Tx_ready=0;
//  FlipBit(PORTD,LED1);
}
//*****************************************************************
ISR (USART_TX_vect,)
{
 	 Tx_ready=0;
//	 FlipBit(PORTD,LED4);
}
//*********************************************************
ISR (ADC_vect,)
{
//	printf ("ADC=%d\r\n",ADC);
//	SetBit(ACSR,ACIE);
}
//*********************************************************
ISR (ANALOG_COMP_vect,)
{
//	ClrBit(ACSR,ACIE);//Interrupt enable
//		SetBit(ADCSRA,ADIE);
//		SetBit(ADCSRA,ADSC);

//	printf ("Comparator work\r\n");
//	ClrBit(PORTD,Buzzer);
//	SetBit(ACSR,ACIE);//Interrupt enable
}
