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
// MODIF ACTIVITE 2 : Fast et Slow recalcules pour 25ms/tick (Timer2 a 40Hz)
// Fast : toggle toutes les 4 x 25ms = 100ms -> periode LED = 200ms (5 Hz)
// Slow : toggle toutes les 40 x 25ms = 1000ms -> periode LED = 2s (0.5 Hz)
#define  Slow  40
#define  Fast  4

// Elements presents sur la carte 8051F020
sbit  LED = P1^6;         // LED verte: '1' = ON; '0' = OFF
sbit BP = P3^7;           // Bouton Poussoir '1' relache, '0' presse

unsigned int Value_tempo = Fast;
//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void Config_INT7(void);
void Config_Timer3(void);
void Config_Timer2(void);

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void) {

	Init_Device();
	Config_Timer2();   // MODIF ACTIVITE 2 : Timer2 remplace Timer3
	Config_INT7();

	LED = LED_Off;

	// Config P4.0 a P4.3 en sortie
	P74OUT |= (1<<0); // P4.0 a P4.3 configurees en Sortie Push Pull
	// Config P4.4 a P4.7 en entree
	P74OUT &= ~(1<<1); // P4.4 a P4.7 en OD
	P4 |= (1<<4)|(1<<5)|(1<<6)|(1<<7);

	// Configuration de BP (P3.7) en entree
    BP = 1;
    P3MDOUT &= ~0x80;  // P3.7 place en Open Drain

	// AJOUT ACTIVITE 2 : P3.3 en sortie push-pull pour REF_1Hz
	P3MDOUT |= (1<<3);

	EA = 1;

    while (1);
}

//*****************************************************************************
void Config_INT7(void)
{
	// Registres: P3IF
	P3IF &= ~0x08; // INT7 active front descendant
	P3IF &= ~0x80; // RAZ Flag INT7

	EIE2 |= 0x20;  // INT7 autorisee
	EIP2 &= ~0x20; // Priorite basse
}

//***************************************************************************************
void ISR_INT7(void) interrupt 19
{
	P4 |= (1<<0);      // Drapeau materiel INT7 sur P4.0 (B23) mis a 1
	P3IF &= ~(1<<7);   // RAZ Flag INT7
	// Gestion vitesse de clignotement de la LED
	if (Value_tempo == Slow) Value_tempo = Fast;
	else Value_tempo = Slow;
	P4 &= ~(1<<0);     // Drapeau materiel INT7 sur P4.0 mis a 0
}

//*****************************************************************************
// ACTIVITE 2 - Config Timer2 comme base de temps a 40 Hz
//*****************************************************************************
void Config_Timer2(void)
{
	RCAP2  = -((SYSCLK/12)/40); // reload pour 40Hz : -(1843200/40) = -46080 = 0x4C00
	T2CON  = 0x04;              // TR2=1 (lance), CT2=0 (interne), CPRL2=0 (auto-reload)
	ET2    = 1;                 // autorisation interruption Timer 2 (registre IE)
	PT2    = 1;                 // priorite haute (registre IP)
}

//*****************************************************************************
// ACTIVITE 2 - ISR Timer2 : base de temps 25ms, gestion LED + REF_1Hz
//*****************************************************************************
void ISR_Timer2(void) interrupt 5
{
	static unsigned int Tick_counter = 0;  // compteur pour la LED
	static unsigned int REF_counter  = 0;  // compteur pour REF_1Hz

	P4 |= (1<<3);   // drapeau materiel P4.3 a 1 (debut ISR)
	TF2 = 0;        // RAZ flag overflow Timer 2 (obligatoire, non automatique)

	// --- Gestion clignotement LED ---
	Tick_counter++;
	if (Tick_counter >= Value_tempo)
	{
		Tick_counter = 0;
		LED = ~LED;
	}

	// --- Gestion signal REF_1Hz (0.5 Hz sur P3.3) ---
	// Toggle toutes les 40 interruptions x 25ms = 1s -> periode = 2s -> 0.5 Hz
	REF_counter++;
	if (REF_counter >= 40)
	{
		REF_counter = 0;
		P3 ^= (1<<3);   // toggle P3.3
	}

	P4 &= ~(1<<3);  // drapeau materiel P4.3 a 0 (fin ISR)
}

//*****************************************************************************
// ISR Timer3 conservee comme reference (non utilisee en activite 2)
//*****************************************************************************
void Config_Timer3(void)
{
   	TMR3CN    = 0x04;
    TMR3RL   = 0xB800;
	EIE2      |= (1<<0); // autorisation INT Timer3
	EIP2       |= (1<<0); // Priorite haute
}

void ISR_Timer3(void) interrupt 14  // Interruption toutes les 10 ms
{
	static unsigned int Tick_counter = 0;

	P4 |= (1<<2);          // flag mis a 1 sur P4.2
	TMR3CN &= ~(1<<7);     // RAZ flag overflow Timer 3
	Tick_counter++;
	if (Tick_counter >= Value_tempo)
	{
		Tick_counter = 0;
		LED = ~LED;
	}
	P4 &= ~(1<<2);         // flag mis a 0 sur P4.2
}

 // ACTIVITE 1 :
/*
 Quel timer est utilise?
 Le timer utilise est le timer 3 , on peut le voir dans la fonction config_Timer3(void)
 Quel est le mode de fonctionnement de ce timer ?
 Le timer par de la periode de reload TMR3RL   = 0xB800 une fois qui depasse 0XFFF (valeur max sur 16 bits) il est alors en overflow et il recommence a la valeur de reload
 Quelle est la frequence de l'horloge du timer ?
Avec T3M = 0 :
F_timer = SYSCLK / 12 = 22 118 400 / 12 = 1 843 200 Hz
T_tick  = 1 / 1 843 200 = 0,000 000 5425 s = 0,5425 us
Donc le compteur s'accremente toute les ~ 0,5 us
 Quelle est la recurrence des interruptions, Comment est-elle obtenue ?
Le timer TMR3RL fait une boucle de 0XB800 a 0XFFF donc pour calculer la reccurence des interruptions il suffit de soustraire la valeur de :
0XFFF - 0XB800 + 1 (car l'overflow ce declenche quand on depasse 0XFFF) = 65 535 - 47 104 + 1 = 18 432
Et la multiplier par la frequence de l'horloge du timer :
18 432 * 0,5 us ~ 10 ms
On a donc une frequence d'interuption de 10 ms
 Si vous deviez diviser par 2 cette recurrence, que feriez-vous ?
Divisier par 2 cette recurrence signifie que l'interruption est deux fois moins frequence .
Cela reviens a doubler la periode car la f = 1 / T => on a 10 ms * 2 = 20 ms *
 Serait-il possible de multiplier cette recurrence par 4 ?
EN mutlipliant la frequence par 4 on divise par 4 notre periode on passe a 2,5 ms .
*/
