//------------------------------------------------------------------------------------
// Base_TP3_IRC
//------------------------------------------------------------------------------------
//
// AUTH: FJ
// DATE: 09-03-2022
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
// Global CONSTANTS
//------------------------------------------------------------------------------------

#define SYSCLK 22118400 //approximate SYSCLK frequency in Hz
#define LED_On 1
#define LED_Off 0


// Eléments présents sur la carte 8051F020
sbit  LED = P1^6;         // LED verte: '1' = ON; '0' = OFF
sbit BP = P3^7;           // Bouton Poussoir '1' relaché, '0' pressé

// Ports de Test
sbit Tst_P34 = P3^4;
sbit Tst_P35 = P3^5;
sbit Tst_P36 = P3^6;


//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void) {

	Init_Device();
	Tst_P34 = 0;
	LED = LED_Off;
	

   while (1)
   {  
   LED = ~LED;
   Tst_P34 = ~Tst_P34;
   tempo_sup('M',60);			 
   }
}

