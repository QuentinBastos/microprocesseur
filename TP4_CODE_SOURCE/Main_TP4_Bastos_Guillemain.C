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
void Config_Timer2(void)
{
	RCAP2 = - ((SYSCLK/12)/40)
	T2CON =0X04;
	ET2 = 1;
	PT2 = 1; 
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

 void ISR_Timer2(void) interrupt 5 
 {
	 P4 |= (1<<3); // flag mis � 1 sur P4.3 
	 TF2 = 0;
	 Tick_counter++;
	 if (Tick_counter >= Value_tempo)
	 { 	 
		  Tick_counter = 0;
		  LED = ~LED;
	 }	
	 P4 &= ~(1<<3);	 // flag mis � 0 sur P4.3 
 }

 // ACTIVITÉ 1 :
/* 
 Quel timer est utilisé?
 Le timer utilisé est le timer 3 , on peut le voir dans la fonction config_Timer3(void)
 Quel est le mode de fonctionnement de ce timer ?
 Le timer par de la periode de reload TMR3RL   = 0xB800 une fois qui depasse 0XFFF (valeur max sur 16 bits) il est alors en overflow et il recommence a la valeur de reload
 Quelle est la fréquence de l’horloge du timer ?
Avec T3M = 0 :
F_timer = SYSCLK / 12 = 22 118 400 / 12 = 1 843 200 Hz
T_tick  = 1 / 1 843 200 = 0,000 000 5425 s = 0,5425 µs
Donc le compteur s'accremente toute les ~ 0,5 µs
 Quelle est la récurrence des interruptions, Comment est-elle obtenue ?
Le timer TMR3RL fait une boucle de 0XB800 à 0XFFF donc pour calculer la réccurence des interruptions il suffit de soustraire la valeur de :
0XFFF - 0XB800 + 1 (car l'overflow ce declenche quand on depasse 0XFFF) = 65 535 - 47 104 + 1 = 18 432 
Et la multiplier par la frequence de l'horloge du timer :
18 432 * 0,5 µs ~ 10 ms
On a donc une frequence d'interuption de 10 ms 
 Si vous deviez diviser par 2 cette récurrence, que feriez-vous ?
Divisier par 2 cette recurrence signifie que l'interruption est deux fois moins frequence .
Cela reviens a doubler la periode car la f = 1 / T => on a 10 ms * 2 = 20 ms *
 Serait-il possible de multiplier cette récurrence par 4 ? 
EN mutlipliant la frequence par 4 on divise par 4 notre periode on passe a 2,5 ms .
*/
