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

void Config_INT7(void)
{
	P3IF &= ~0x08; // Efface le flag d'interruption sur P3.3 (remise à 0 pour éviter un déclenchement parasite au démarrage)
	P3IF &= ~0x80; // Efface le flag d'interruption sur P3.7 (bit du bouton poussoir, remise à 0 avant activation)
	EIE2 |= 0x20;  // Active l'interruption externe INT7 dans le registre d'activation étendu EIE2 (bit 5 = masque INT7)
	EIP2 &= ~0x20; // Positionne la priorité de INT7 en basse priorité dans EIP2 (bit 5 à 0 = priorité faible)
}

void ISR_INT7(void) interrupt 19
{
	 P4 |= (1<<0);          // Met à '1' le bit 0 du port P4 (signal de début de traitement, utile pour debug oscilloscope)
	 P3IF &= ~0x80;         // Efface le flag d'interruption P3.7 pour autoriser la prochaine interruption sur le bouton
	 if (Value_tempo == Slow) Value_tempo = Fast; // Si la LED clignote lentement, on passe en mode rapide
	 else Value_tempo = Slow;                     // Sinon (mode rapide), on repasse en mode lent
	  P4 &= ~(1<<0);        // Remet à '0' le bit 0 du port P4 (signal de fin de traitement ISR)
}

void Config_Timer3(void)
{
   	TMR3CN    = 0x04;
    TMR3RL   = 0xB800;
}

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

//ACTIVITÉ 3 :
//La ligne de code qui permet de changer la frequence de clignotement de la LED est :
//if (Value_tempo == Slow) Value_tempo = Fast; L86
//else Value_tempo = Slow; L87
//La fonction ISR_INT7 est la fonction qui permet de lier l'evenement materiel et le changement de vitesse
//Les variable P3IF , P3MDOUT et P7OUT sont utilisées pour configurer le bouton poussoir .

//ACTIVITÉ 4 :