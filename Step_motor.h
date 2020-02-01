#ifndef Step_motor
#define Step_motor

#include <avr/eeprom.h>
#include <avr/io.h>
#include "Init.h"
#include <stdint.h>

//#define TIMER1CLK 8000000/8// SYSCLK frequency in Hz
//#define TIMER2CLK 8000000/1024// SYSCLK frequency in Hz
#define Acselerate_limit 62// SYSCLK frequency in Hz
#define TIMERCLK_0 8000000/1024// SYSCLK frequency in Hz
#define TIMERCLK_2 8000000/64// SYSCLK frequency in Hz
#define TIMERCLK_3 8000000/8// SYSCLK frequency in Hz
#define Rigth_SW_test 0x10//
#define Rigth_SW_set 0x10 
#define Left_SW_test 0x08//
#define Left_SW_set 0x08//
//*********************************************************
//typedef struct {unsigned int Unit;unsigned int Speed_min;unsigned int Speed_max;unsigned int Acselerate;unsigned char Microstep;}Space;
typedef struct {float Unit;unsigned int Speed_min;unsigned int Speed_max;unsigned int Acselerate;unsigned char Microstep;}Space;
//typedef struct {float Unit;float Linear;float Quadro;}Polinome;
typedef struct {float Step;float Unit;float Linear;float Quadro;float Three;}Polinome;
typedef struct {float Left;float Right;float Delt;}Gran;
//*********************************************************
extern Space 					Work_Parameters;
extern Space			EEMEM	Store_Parameters;
extern Polinome					Work_Polinome;
extern Polinome			EEMEM	Store_Polinome;
extern unsigned long	EEMEM	Store_Coordinate;
extern float			EEMEM	Store_Nm;
extern Gran						Work_Gran;
extern Gran				EEMEM	Store_Gran;
//*********************************************************

extern volatile unsigned char	Status;
extern volatile unsigned char	Direction;

extern volatile unsigned char	T_acselerate;
extern volatile unsigned int	T_Speed;
extern volatile unsigned int	T_Speed_min,T_Speed_max;

extern volatile long			Speed;
extern volatile long			Work_counter,Up_down_speed;
extern volatile long			Calibr_counter;
extern volatile long			Coordinate;
extern volatile float			t_Nm;
extern volatile long			Next_coordinate;
//*********************************************************
extern unsigned long	Store_Coordinate;
//*********************************************************
unsigned char Motor_scan(long Step);
void Motor_set(unsigned char microstep);
void Write_coordinate(unsigned long *value_coord);//
unsigned long Read_coordinate(void);// 
void Write_nm(float *nm);//
float Read_nm(void);

#endif


