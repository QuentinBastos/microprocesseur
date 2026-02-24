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


// El�ments pr�sents sur la carte 8051F020
sbit  LED = P1^6;         // LED verte: '1' = ON; '0' = OFF
sbit BP = P3^7;           // Bouton Poussoir '1' relach�, '0' press�

// Ports de Test
sbit Tst_P34 = P3^4;
sbit Tst_P35 = P3^5;
sbit Tst_P36 = P3^6;

// Définition de la LED sur P1.6 (selon le schéma de la carte)
sbit LED_VERTE = P1^6;

void main (void) {

	// --- INITIALISATION ---
	Init_Device(); // Appelle la config de base (UART0, Timers, SYSCLK sur P1.3)

	// --- ACTIVITÉ 1 : Modification Crossbar ---
	XBR2 |= 0x04;

	P1MDOUT |= 0x40; // Met le bit 6 de P1MDOUT à 1 pour passer P1.6 en Push-Pull

	// --- ACTIVITÉ 3 : Entrée Numérique (Bouton Poussoir) ---
	/*
	   PROBLÈME : Le bouton poussoir est une ENTRÉE. Pour lire une entrée sans
	   ambiguïté, la broche doit être laissée en Open-Drain (par défaut) et on
	   doit forcer la valeur à '1' dans le registre de sortie pour "ouvrir"
	   le circuit de lecture.
	*/
	// Si le bouton est sur P3.7 par exemple :
	P3 |= 0x80; // On écrit 1 sur le bit 7 pour pouvoir lire l'état réel externe

	while (1) {


		if (BOUTON == 0) {
			LED_VERTE = 1;
		} else {
			LED_VERTE = 0;
		}

		{
			unsigned long i;
			for (i = 0; i < 50000; i++);
		}
	}
}

/* ACTIVITE 2 :
 * On peut observer que la LED ne clignote pas sur la carte en premier temps.
 * Ceci est du à la faible impudence qui fait que le signal n'est pas assez fort pour allumer la LED (de plus les ports du 8051 sont en open drain de base
 * dans ce mode, la broche peut imposer un niveau bas donc 0V, elle ne peut pas pousser de courant à haut niveau donc haute impudence)..
 * Cepandant, nous pouvons observer des signaux periodiques à l'aide de loscilataure, nous remarquons que la fréquence des
 * signaux n'est pas compatible avec une fréquence observable.
 * Pour allumer la LED, il faut modifier le mode de la broche en Push-Pull sur P1.6, grace à ca on a une tension sup a 2.2V en ignorant la résistance.
 *
