#include "Step_motor.h"
//*********************************************************
Space 					Work_Parameters;
//Space			EEMEM	Store_Parameters={21,20,5000,15000,0x03};
Space			EEMEM	Store_Parameters={20.516,100,2000,100,'0'};
Polinome				Work_Polinome;
//Polinome		EEMEM	Store_Polinome={0.00075,-7284.33,12.41833,0.0073615};
//Polinome		EEMEM	Store_Polinome={0.00075,-61881.3,124.048,0.0064,0.000035395};
//Polinome		EEMEM	Store_Polinome={0.00075,-60892,118.09067,0.017968,0.000027986267};
Polinome		EEMEM	Store_Polinome={0.001,0,-500,0,0};

unsigned long	EEMEM	Store_Coordinate=-249999;
float			EEMEM	Store_Nm=500;

Gran					Work_Gran;
Gran			EEMEM	Store_Gran={380.0,710.0,20.0};
//*********************************************************
volatile unsigned char  Status;
volatile unsigned char	Direction;

volatile unsigned char	T_acselerate;

volatile unsigned int	T_Speed;
volatile unsigned int	T_Speed_min,T_Speed_max;
volatile long			Work_counter,Up_down_speed;
volatile long			Calibr_counter;

volatile long			Speed;
volatile long			Coordinate;
volatile long			Next_coordinate;
volatile float			t_Nm;
//*********************************************************
extern unsigned long	Store_Coordinate;//={1,2};
//******************************************************************************************
unsigned char Motor_scan(long Step_counter)
{
	if(Status=='B'){printf ("Busy Move\r\n");	return'B';}
	Status='Y';
	if(Step_counter==0)	return'Y';

	if((Step_counter>0)&&((PIND&Rigth_SW_test)!=0)) {Direction=1;PORTD=PORTD|0x20;Work_counter=Step_counter;Up_down_speed=Step_counter/2;}
	else if ((Step_counter<0)&&((PIND&Left_SW_test)!=0))	{Direction=0;PORTD=PORTD&0xDF;Work_counter=-Step_counter;Up_down_speed=-Step_counter/2;}
	else {
		//		SetBit(PORTB,ENABLE);
		if((PIND&Rigth_SW_test)==0) {Status='R';return'R';}
		if((PIND&Left_SW_test)==0) {Status='L';return'L';}
	}
				
	Speed=Work_Parameters.Speed_min;
	
	T_Speed_min=(0xffff-TIMERCLK_3/Work_Parameters.Speed_min);
	T_Speed_max=(0xffff-TIMERCLK_3/Work_Parameters.Speed_max);
	T_Speed=T_Speed_min;	
	T_acselerate=0xff-TIMERCLK_2/Work_Parameters.Acselerate;
	
	ClrBit(PORTB,ENABLE);//motor on
	delay(10);	
	
	TCNT2 = T_acselerate;
	TCNT1 =T_Speed;
	
	TCCR2B = 0x04;//0x02; //start timer0, frequency = 0Hz
	TCCR1B = 0x02;//start timer 2
	
	TIMSK2 |= 0x01;		
	TIMSK1 |= 0x01;
	Status='B';
	return	'B';
}
//*******************************************************************
void Motor_set(unsigned char microstep)
	 {unsigned char u1=0x00;
		u1 = (microstep & 0x7) << 6;	 
		PORTB &= 0x3F;
		PORTB = PORTB + u1;
 	 }
//*******************************************************************
void Write_coordinate(unsigned long *value_coord)
	 {
	 eeprom_write_dword(&Store_Coordinate,*value_coord);
 	 }
//*******************************************************************	  
unsigned long Read_coordinate(void)//N,*DXX
	 {unsigned long DXX;
	 DXX=(unsigned long)eeprom_read_dword(&Store_Coordinate);//
	 return DXX;
	 }
//*******************************************************************
void Write_nm(float *nm)
{
	eeprom_write_float(&Store_Nm,*nm);
}
//*******************************************************************
float Read_nm(void)
{	float Nm;
	Nm=eeprom_read_float(&Store_Nm);//
	return Nm;
}

	 
	 
