#include <avr/io.h>
#include <stdio.h>
#include "Init.h"
#include <inttypes.h>
#include "Step_motor.h"
#include <float.h>
#include <avr/eeprom.h>
//#include "TWI_Master.h"

#define loop_until_bit_is_set(sfr,bit)  do { } while (bit_is_clear(sfr, bit))
	
//**********************TWI******************************	
#define TWI_GEN_CALL         0x00  // The General Call address is 0

// Sample TWI transmission commands
#define TWI_CMD_MASTER_WRITE 0x10
#define TWI_CMD_MASTER_READ  0x20

// Sample TWI transmission states, used in the main application.
#define SEND_DATA             0x01
#define REQUEST_DATA          0x02
#define READ_DATA_FROM_BUFFER 0x03	

//********************************************************
char ERRATA;
static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);
static int uart_putchar(char c, FILE *stream)//
{
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0 = c;
//	ClrBit(PORTB,SLEEP);
	return 0;
}
//********************************************************
unsigned int ADC_internal(void)
{
	SetBit(ADCSRA,ADSC);
//	SetBit(ADCSRA,ADIE);
	while ((ADCSRA&0x40) ==0x40);
	return ADC;
}
void ADC_int_start(void)
{
	SetBit(ADCSRA,ADIE);
	SetBit(ADCSRA,ADSC);
	return;
}

//*********************************************************
void External_control(char SET_EX)
{
	EICRA=0x00;
}
/*
void Restore_ADCs(void)
{ int i,j;
	for ( j = 0; j < 6; j++ )
	{for ( i = 0; i < 4; i++ )
		{
		eeprom_read_block ((void*)&ADC_Work,(void*)&Store_ADC[j][i],sizeof(ADC_Struct));
		Write_ADC(j,i,ADC_Work.AD_Gain,ADC_Work.AD_Shift);	
		}	
	}

}
*/


/*unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
{
	// A failure has occurred, use TWIerrorMsg to determine the nature of the failure
	// and take appropriate actions.
	// Se header file for a list of possible failures messages.
	
	// Here is a simple sample, where if received a NACK on the slave address,
	// then a retransmission will be initiated.
	
	if ( (TWIerrorMsg == TWI_MTX_ADR_NACK) | (TWIerrorMsg == TWI_MRX_ADR_NACK) )
	TWI_Start_Transceiver();
	
	return TWIerrorMsg;
}*/

float Polin( float F_DX )
{
	return ((Work_Polinome.Three*F_DX+Work_Polinome.Quadro)*F_DX+Work_Polinome.Linear)*F_DX+Work_Polinome.Unit;
}

/*void Run_Gran( float F_DX )
{
	
}
*/
int Run_Gran( float F_DX )
{	float F_DX_n;
/*	bool f;
	if(F_DX>0)
		f=Gran.Delt>0;
	else
		f=Gran.Delt<0;
*/		
	/*char f=(F_DX>0)?Work_Gran.Delt>0:Work_Gran.Delt<0; //����� �� �������
	if(F_DX>0)
	{
		F_DX+=t_Nm;
		if(F_DX>Work_Gran.Right)
		{
//			F_DX=Work_Gran.Right;
//			f=0; 
			return 1;
		} else {
			if(f) {
				F_DX_n=F_DX;
				F_DX+=Work_Gran.Delt;
				if(F_DX>Work_Gran.Right)
					F_DX=Work_Gran.Right;
			}
		}
	}
	else
	{
		F_DX+=t_Nm;
		if(F_DX<Work_Gran.Left)
		{
//			F_DX=Work_Gran.Left;
//			f=0; 
			return 1;
		} else {
			if(f) {
				F_DX_n=F_DX;
				F_DX+=Work_Gran.Delt;
				if(F_DX<Work_Gran.Left)
					F_DX=Work_Gran.Left;
			}
		}
	}*/
	Next_coordinate=(long)Polin(F_DX);
	//	Motor_scan (Next_coordinate);
	Motor_scan (Next_coordinate-Coordinate);
	while (Status=='B');//printf ("ST** %lu\r\n",Speed);
	if(f) {
		Next_coordinate=(long)Polin(F_DX_n);
		Motor_scan (Next_coordinate-Coordinate);
		while (Status=='B');//printf ("ST** %lu\r\n",Speed);
		t_Nm=F_DX_n;
	} else
		t_Nm=F_DX;
	Write_coordinate((void*)&Coordinate);
	Write_nm((void*)&t_Nm);
	return 0;
}
/**/

//******************************************************************************
int main(void)//int
{
//unsigned char messageBuf[4];
//unsigned char TWI_targetSlaveAddress= 0x20,TWI_operation=0;


	stdout = &mystdout;
	stdin  = NULL;
	stderr = NULL;

	

	init_devices();	
//	TWI_Master_Initialise();	
	memset(Command_buff,0,sizeof(Command_buff));
	memset(Data_buff_RS,0,sizeof(Data_buff_RS));
	
		
	buff_byte=0;
	eol=0x00;
	Running='I';
	Ext_Running='N';
	Rx_ready='1';
	Status='Y';
	ComNumber='0';	
	eeprom_read_block ((void*)&Work_Parameters,(void*)&Store_Parameters,sizeof(Space));
	eeprom_read_block ((void*)&Work_Polinome,(void*)&Store_Polinome,sizeof(Polinome));
	eeprom_read_block ((void*)&Work_Gran,(void*)&Store_Gran,sizeof(Gran));
	
	Motor_set(Work_Parameters.Microstep);
	Coordinate=Read_coordinate();
	t_Nm=Read_nm();
	 
//	while (1) {FlipBit(PORTD,STEP);}
//	delay(6000);
// while (1)	{UDR0 = 0x01;delay(100);}//uart_putchar('C');

//	while (1) {delay(60);UDR0 = 0x55;}
//while (1) {printf("RDII Calibrator V1.0,9600\r\n");delay(100);}

//Motor_scan (1000);

 for (;;)
 {
//*********************WAIT COMMAND**************************************
	while(Rx_ready=='1')
		{
		if (Ext_Running=='L'){Motor_scan (100000);Ext_Running='N';}
		if (Ext_Running=='R'){Motor_scan (-100000);Ext_Running='N';}
		}
	
	Rx_ready='1';
	
//**************************************************************************************************
   	if (!strncmp(Command_buff,"IIRD",4))
	{	memset(Command_buff,0,sizeof(Command_buff));
		printf("RDII Monochromator MSmL 847\r\n");//RDII Calibrator V1.0,9600\r\n
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"STOP",4))
	{
		Rx_ready='1';memset(Command_buff,0,sizeof(Command_buff));
		Running='I';
//		SetBit(PORTB,ENABLE);//motor off
		printf ("STOP\r\n");
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"TDRD",4))
	{	unsigned int TD_timer;// float U_termo;
		sscanf (Command_buff+4,"%u",&TD_timer);
		if (TD_timer==0) TD_timer=100;
		while(Running=='I')
			{
//			U_termo=COE_ADC*ADC;
//			printf ("RDTD %1.2f\r\n",U_termo);//ADC
			printf ("RDTD %u\r\n",ADC);//ADC
			delay(TD_timer);
			}
		Running='I';
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"PLRD",4))
	{//	printf ("RDPL %6.2f,%6.2f,%6.2f\r\n",Work_Polinome.Unit,Work_Polinome.Linear,Work_Polinome.Quadro);
	  //printf ("RDPL %3.6g,%3.6g,",Work_Polinome.Step,Work_Polinome.Unit*Work_Polinome.Step);
		printf ("RDPL %3.6g,",Work_Polinome.Step);
		printf ("%3.6g,",Work_Polinome.Unit*Work_Polinome.Step);
//		printf ("%3.6g,%3.6g,",Work_Polinome.Linear*Work_Polinome.Step,Work_Polinome.Quadro*Work_Polinome.Step);
		printf ("%3.6g,",Work_Polinome.Linear*Work_Polinome.Step);
		printf ("%3.6g,",Work_Polinome.Quadro*Work_Polinome.Step);
		printf ("%3.6g\r\n",Work_Polinome.Three*Work_Polinome.Step);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"PLWR",4))
	{
//		sscanf (Command_buff+4,"%f,%f,%f",&Work_Polinome.Unit,&Work_Polinome.Linear,&Work_Polinome.Quadro);
		sscanf (Command_buff+4,"%f,%f,%f,%f,%f",&Work_Polinome.Step,&Work_Polinome.Unit,&Work_Polinome.Linear,&Work_Polinome.Quadro,&Work_Polinome.Three);
		Work_Polinome.Unit /= Work_Polinome.Step;
		Work_Polinome.Linear /= Work_Polinome.Step;
		Work_Polinome.Quadro /= Work_Polinome.Step;
		Work_Polinome.Three /= Work_Polinome.Step;
		eeprom_write_block ((void*)&Work_Polinome,(void*)&Store_Polinome,sizeof(Polinome));
//		printf ("WRPL %6.2f,%6.2f,%6.2f\r\n",Work_Polinome.Unit,Work_Polinome.Linear,Work_Polinome.Quadro);
//		printf ("WRPL %3.6g,%3.6g,",Work_Polinome.Step,Work_Polinome.Unit*Work_Polinome.Step);
		printf ("%3.6g,",Work_Polinome.Step);
		printf ("%3.6g,",Work_Polinome.Unit*Work_Polinome.Step);
//		printf ("%3.6g,%3.6g,",Work_Polinome.Linear*Work_Polinome.Step,Work_Polinome.Quadro*Work_Polinome.Step);
		printf ("%3.6g,",Work_Polinome.Linear*Work_Polinome.Step);
		printf ("%3.6g,",Work_Polinome.Quadro*Work_Polinome.Step);
		printf ("%3.6g\r\n",Work_Polinome.Three*Work_Polinome.Step);

		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"STRD",4))
	{	printf ("RDST %li,%3.5g\r\n",Coordinate,Coordinate*Work_Polinome.Step);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"GRWR",4))
	{	sscanf (Command_buff+4,"%f,%f,%f",&Work_Gran.Left,&Work_Gran.Right,&Work_Gran.Delt);
		eeprom_write_block ((void*)&Work_Gran,(void*)&Store_Gran,sizeof(Gran));//
//		printf ("WRGR %5.2g,%5.2g,%5.2g\r\n",Work_Gran.Left,Work_Gran.Right,Work_Gran.Delt);
		printf ("WRGR %5.2g,",Work_Gran.Left);
		printf ("%5.2g,",Work_Gran.Right);
		printf ("%5.2g\r\n",Work_Gran.Delt);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"GRRD",4))
	{
//		printf ("RDGR %5.2g,%5.2g,%5.2g\r\n",Work_Gran.Left,Work_Gran.Right,Work_Gran.Delt);
		printf ("RDGR %5.2g,",Work_Gran.Left);
		printf ("%5.2g,",Work_Gran.Right);
		printf ("%5.2g\r\n",Work_Gran.Delt);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"AMCF",4))
	{	sscanf (Command_buff+4,"%f,%u,%u,%u,%c",&Work_Parameters.Unit,&Work_Parameters.Speed_min,&Work_Parameters.Speed_max,&Work_Parameters.Acselerate,&Work_Parameters.Microstep);
		Motor_set(Work_Parameters.Microstep);
		if(Work_Parameters.Acselerate<31)Work_Parameters.Acselerate=31;
		eeprom_write_block ((void*)&Work_Parameters,(void*)&Store_Parameters,sizeof(Space));//
		printf ("CFAM %c\r\n",Status);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"AMST",4))
	{  	printf ("STAM %3.5g,%u,%u,%u,%c\r\n",Work_Parameters.Unit,Work_Parameters.Speed_min,Work_Parameters.Speed_max,Work_Parameters.Acselerate,Work_Parameters.Microstep);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"AMWR",4))
	{	float F_DX; //=34.7;
		sscanf (Command_buff+4,"%f",&F_DX);
//		Coordinate=(long)(F_DX*Work_Parameters.Unit);
		Coordinate=(long)Polin(F_DX);
		Write_coordinate((void*)&Coordinate);
		t_Nm=F_DX;
		Write_nm((void*)&t_Nm);
//		printf ("WRAM %c,%6.2f\r\n",Status,(float)Coordinate/Work_Parameters.Unit);
		printf ("WRAM %c,%3.3f\r\n",Status,t_Nm);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"AMRD",4))
	{//	printf ("RDAM %c,%6.2f\r\n",Status,(float)Coordinate/Work_Parameters.Unit);
		printf ("RDAM %c,%3.3f\r\n",Status,t_Nm);
		continue;
	}
//**************************************************************************************************
	if (!strncmp(Command_buff,"AMTS",4))
	{	
		long abc=1;
		sscanf (Command_buff+4,"%ld",&abc);
		Motor_scan (abc);		
		while(Status=='B')printf ("TSAM %c,%lu,%lu\r\n",Status,Work_counter,Speed);
		printf ("TSAM %c\r\n",Status);
		continue;
	}
//**************************************************************************************************
if (!strncmp(Command_buff,"AMGO",4))
{	float F_DX;
	sscanf (Command_buff+4,"%f",&F_DX);
//	Next_coordinate=(long)Work_Parameters.Unit*F_DX;
	F_DX-=t_Nm;
	if(Status=='B' || Status=='E'){printf ("ABGO %c\r\n",Status);continue;}
	if( Run_Gran(F_DX) == 0 )
		printf ("GOAM %c\r\n",Status);
	else
		printf ("ABGO nor %c\r\n",Status);
	continue;
}
//**************************************************************************************************
if (!strncmp(Command_buff,"AMRU",4))
{	float F_DX;
	sscanf (Command_buff+4,"%f",&F_DX);
//	Next_coordinate=(long)Work_Parameters.Unit*F_DX;
	if(Status=='B' || Status=='E'){printf ("ABRU %c\r\n",Status);continue;}
	if( Run_Gran(F_DX) == 0 )
		printf ("RUAM %c\r\n",Status);
	else
		printf ("ABRU nor %c\r\n",Status);
	continue;
}
//**************************************************************************************************
if (!strncmp(Command_buff,"AMCL",4))
{	Calibr_counter=0;
	Motor_scan (1000000);
	while (Status!='R' && Running!='A');
	Calibr_counter=0;
	Motor_scan (-1000000);
	while (Status!='L' && Running!='A');
	printf ("CLAM %ld\r\n",Calibr_counter);
	Motor_scan (Calibr_counter/2);
	continue;
}
//**************************************************************************************************

/*	if (!strncmp(Command_buff,"TWIT",4))
	{
		
		messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
		messageBuf[1] = 0x00;
		printf ("message %x,%x\r\n",messageBuf[0],messageBuf[1]);
		TWI_Start_Transceiver_With_Data( messageBuf, 2 );
		TWI_operation = REQUEST_DATA; // Set the next operation
		Running='I';
		while (Running=='I')
		{
			// Check if the TWI Transceiver has completed an operation.
			if ( ! TWI_Transceiver_Busy() )
			{
				// Check if the last operation was successful		
				if ( TWI_statusReg.lastTransOK )
				{
					// Determine what action to take now
					if (TWI_operation == SEND_DATA)
					{ // Send data to slave
						messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
						TWI_Start_Transceiver_With_Data( messageBuf, 2 );
						TWI_operation = REQUEST_DATA; // Set next operation
					}
					else if (TWI_operation == REQUEST_DATA)
					{ // Request data from slave
						messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
						TWI_Start_Transceiver_With_Data( messageBuf, 2 );
						TWI_operation = READ_DATA_FROM_BUFFER; // Set next operation
					}
					else if (TWI_operation == READ_DATA_FROM_BUFFER)
					{ // Get the received data from the transceiver buffer
						TWI_Get_Data_From_Transceiver( messageBuf, 2 );
						printf ("Buffer= %c\r\n",messageBuf[1]);        // Store data on PORTB.
						TWI_operation = SEND_DATA;    // Set next operation
					}
				}
				else // Got an error during the last transmission
				{
					// Use TWI status information to determine cause of failure and take appropriate actions.
					ERRATA=TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );
//					printf ("ERROR %X,%X\r\n",ERRATA,TWI_statusReg.lastTransOK);
				}
			}
			// Do something else while waiting for the TWI Transceiver to complete the current operation
			asm("nop"); // Put own code here.
		}
		printf ("stop TWI""\r\n");
		Running='I';
		continue;
	} */

	else
	{
		printf ("BAD COMMAND""\r\n");
		continue;
	}
}
}



