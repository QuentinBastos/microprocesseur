//------------------------------------------------------------------------------------
// Main_TP4_IRC
//------------------------------------------------------------------------------------
//
// AUTH: FJ
// DATE: 20-02-2025
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
#define  Slow  100
#define  Fast  10

// El�ments pr�sents sur la carte 8051F020
sbit  LED = P1^6;         // LED verte: '1' = ON; '0' = OFF
sbit BP = P3^7;           // Bouton Poussoir '1' relach�, '0' press�

 unsigned int Value_tempo = Fast;
//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void Config_INT7(void);
void Config_Timer3(void);

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void) {

	
	Init_Device();
	Config_Timer3();
	Config_INT7();
	
	LED = LED_Off;  // LED �teinte
	
	// Config P4.0 � P4.3 en sortie
	P74OUT |= (1<<0); // P4.0 � P4.3 configur�es en Sortie Push Pull
	// Config P4.4 � P4.7 en entr�e
	P74OUT &= ~(1<<1); // P4.4 � P4.7 en OD
	P4 |= (1<<4)|(1<<5)|(1<<6)|(1<<7);
	
	// Configuration de BP (P3.7) en entr�e	
  BP = 1;
  P3MDOUT &= ~0x80;  // P3.7 plac� en Open Drain 	

	EA = 1;

  while (1);
}

 //*****************************************************************************
 //***************************************************************************************
void Config_INT7(void)
{
	// Registres: P3IF
	P3IF &= ~0x08; // INT7 active front descendant
	P3IF &= ~0x80; // RAZ Flag INT7
	
	EIE2 |= 0x20;  // INT7 autoris�e
	EIP2 &= ~0x20; //Priorit� basse
}

//***************************************************************************************
void ISR_INT7(void) interrupt 19
{
	  P4 |= (1<<0);   // Drapeau mat�riel INT7 sur P4.0 (B23) mis � 1
	  P3IF &= ~(1<<7); // RAZ Flag INT7
	  // Gestion vitesse de clignotement de la LED
	  if (Value_tempo == Slow) Value_tempo = Fast;
	  else Value_tempo = Slow;
		P4 &= ~(1<<0);	 // Drapeau mat�riel INT7 sur P4.0 mis � 0
}
//*****************************************************************************
void Config_Timer3(void)
{
   	TMR3CN    = 0x04;
    TMR3RL   = 0xB800;
	  EIE2      |= (1<<0); // autorisation INT Timer3
	  EIP2       |= (1<<0); // Priorit� haute
}
//*****************************************************************************
// Interruption p�riodique d�clench�e par le timer 3
// ****************************************************************************
 void ISR_Timer3(void) interrupt 14  // Interruption toutes les 10 ms
 {
	 static unsigned int Tick_counter = 0;
		 
	 P4 |= (1<<2); // flag mis � 1 sur P4.2 (C22)
	 TMR3CN    &= ~(1<<7);
	 Tick_counter++;
	 if (Tick_counter >= Value_tempo)
	 { 	 
		  Tick_counter = 0;
		  LED = ~LED;
	 }	
	 P4 &= ~(1<<2);	 // flag mis � 0 sur P4.2 (C22)
 }	
