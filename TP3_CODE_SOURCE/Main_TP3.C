//------------------------------------------------------------------------------------
// Main_TP3_IRC
//------------------------------------------------------------------------------------
//
// AUTH: FJ
// DATE: 06-02-2025
// Target: C8051F02x
//
// Tool chain: KEIL Eval 'c'
//
//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <c8051f020.h>                    // SFR declarations
#include <c8051F020_SFR16.h>
#include <TP1_Lib_ASM.h>
#include <Lib_Base.h>

//------------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------------

#define SYSCLK 22118400 //approximate SYSCLK frequency in Hz
#define LED_On 1
#define LED_Off 0
#define  Slow  1000
#define  Fast  100

//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void Config_INT7(void);

// El�ments pr�sents sur la carte 8051F020
sbit  LED = P1^6;         // LED verte: '1' = ON; '0' = OFF
sbit BP = P3^7;           // Bouton Poussoir '1' relach�, '0' press�
// Variables globales
unsigned int Value_tempo = Fast;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void) {

 unsigned int value = Fast;
	
	Init_Device();
	
	Config_Timer3();
	Config_INT7();
	
	LED = LED_Off;
		
  BP = 1;
  P3MDOUT &= ~0x80;  
	
	
	P74OUT |= (1<<0);

	
	EA = 1;

   while (1)
   {  
   LED = ~LED;
	 EA = 0;	 
	 value = 	Value_tempo;
	 EA = 1;	 
   tempo_sup('M',value);	 
   }
}

  //***************************************************************************************
void Config_INT7(void)
{
	P3IF &= ~0x08; 
	P3IF &= ~0x80; 
	EIE2 |= 0x20;  
	EIP2 &= ~0x20; 
}
//***************************************************************************************
void ISR_INT7(void) interrupt 19
{
	 P4 |= (1<<0);
	 P3IF &= ~0x80; 
	 if (Value_tempo == Slow) Value_tempo = Fast;
	 else Value_tempo = Slow;
	  P4 &= ~(1<<0);
}
//*****************************************************************************
void Config_Timer3(void)
{
   	TMR3CN    = 0x04;
    TMR3RL   = 0xB800;
}
//*****************************************************************************
 void ISR_Timer3(void) interrupt 14  
 {
	 TMR3CN    &= ~(1<<7);
 }	

//------------------------------------------------------------------------------------

//ACTIVITÉ 1 :
//Les fonctions d'interruption sont ISR_INT7 et ISR_TIMER3.
//La source d'interuption pour ISR_INT7 est le port 3.7 donc le bouton poussoir.
//La source d'interuption pour ISR_TIMER3 est le timer 3.
//ISR_INT7 est une interruption externe car elle est declenché par le bouton poussoir.
//ISR_TIMER3 est une interruption interne car elle est declenché par le timer 3 qui est un composant interne.
//Les lignes permettant de configurer les interruptions externe sont celle de la fonction CONFIG_INIT7
//EA = 0 coupe les interruptions pour lire les variables , EA = 1 réactive les interruptions apres avoir lu les variables 

//ACTIVITÉ 2 :
