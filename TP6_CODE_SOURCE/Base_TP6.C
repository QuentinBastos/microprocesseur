//-----------------------------------------------------------------------------
// Base_TP6.c
// AUTH: FJ
// DATE: 5/12/21
//
// Target: C8051F02x
// Tool chain: KEIL Microvision5
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Cette application exécute les tâches suivantes:
// SYSCLK = Quartz externe = 22.1184 MHz (Visu sur P1.4)
// Clignotement de la LED P1.6

// Mise en oeuvre de la base de temps Timer2 interruption Timer2
// Mise en oeuvre de l'UART0 à 115200 Bd, pas de parité, 8 bits, 1 stop bit 
 
// Visu Flag INT Timer2 sur P3.5

//*****************************************************************************
//-----------------------------------------------------------------------------
// Fichiers d'entête
#include "intrins.h"
#include<stdio.h>
#include<stdlib.h>
#include<c8051F020.h>
#include<c8051F020_SFR16.h>

//-----------------------------------------------------------------------------
// Déclaration des MACROS

#define LED_ON 1
#define LED_OFF 0
#define LED_BLINK 0

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void Init_Device (void);
void Reset_Sources_Init();
void Port_IO_Init();
void Oscillator_Init_Osc_Quartz();
void Config_Timer2_TimeBase(void);
void Config_Timer4_Event_Counter(unsigned int);
void CFG_Clock_UART0(void);
void CFG_UART0(void);
char putchar(char c);
char _getkey(void);
char getkey_one_time(void);
void  Voltage_Reference_Init(void);
void  ADC0_Init(void);
void  DAC0_Init(void);
void  Timer3_Init_Fech(void);

//-----------------------------------------------------------------------------
// Déclarations Registres et Bits de l'espace SFR
sbit LED = P1^6;  // LED
sbit BP =P3^7;
sbit VISU_INT_TIMER2 = P3^5;
sbit VISU_INT_ADC0 = P3^4;

//-----------------------------------------------------------------------------
// Variables globales
bit Flag_Seconde = 0;
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void) {
	
unsigned int CP_Mesure=0;
unsigned int Value_mV =0;	
	
 	   // Configurations globales
	      Init_Device();
	   // Configurations  spécifiques  
	      Config_Timer2_TimeBase();
	      CFG_Clock_UART0();
	      CFG_UART0();
	      Voltage_Reference_Init();
	      ADC0_Init();
	      DAC0_Init();
// Fin des configurations	
	      printf("\r\r**** TP6 ARCHI 3IRC - System Ready ****\n\r");
	      EA = 1;  // Validation globale des interruptions
	
// Boucle infinie	
         while(1)
         {
					  if (Flag_Seconde != 0)
					   {
						   Flag_Seconde = 0;
						   CP_Mesure++;
						   printf (" Mesure %5u -ADC0H:%#2.2bx - ADC0L:%#2.2bx - Vin en mV: %#2.2u \n\r",CP_Mesure,ADC0H,ADC0L,Value_mV);
				    }
         }
			 }				 
//******************************************************************************
// Voltage_Reference_Init
//******************************************************************************
void  Voltage_Reference_Init(void)
{
		 REF0CN    = 0x03; // ADC0 voltage reference from VREF0 pin
	                     // ADC1 voltage reference from VREF1 pin
	                     // Internal Temperature sensor off
	                     // Internal bias generator ON
	                     // Internal Referece Buffer On
}

//******************************************************************************
// ADC0_Init
//******************************************************************************
void  ADC0_Init(void)
{
	  AMX0CF = 0;  // Toutes les voies sont en unipolaire
	  AMX0SL = 0x07; // 
	  ADC0CF    = 0x50; //// AD0SC = 10 --> CLKsar = 2,01MHz
	                    // Gain =1 
	                     
	  // Config ADC0CN
	  AD0TM = 1; // Le contisseur a besoin de 19 coups de CLK SAR pour faire une conversion
	             // Temps de conversion: 1/2,01Mhz * 19 = 9,5 Microsecondes  
	  AD0INT = 0;  // 
	  AD0WINT = 0; // 
	
	  AD0CM0 = 0;
	  AD0CM1 = 0; 
	
    AD0LJST = 0; 
	               
	  AD0EN = 1;   // ADC0 démarré
}

//******************************************************************************
// ADC0_Init_Activ4
//******************************************************************************
void  ADC0_Init_Activ4(void)
{
	 // Code de configuration ADC0 pour activité 4
}


//******************************************************************************
// DAC0_Init
//******************************************************************************
void  DAC0_Init(void)
{
   // Code de configuration de l'ADC0
}

//******************************************************************************
// Timer3_Init
//******************************************************************************
void  Timer3_Init_Fech(void)
{
	// Code de configuration du Timer3
}


//******************************************************************************
//  INTERRUPTION Base de temps
// Gestion du clignotement de la LED
//*******************************************************************************
void ISR_Timer2 (void) interrupt 5
{
	static char CP_Temps = 0;

	VISU_INT_TIMER2 = 1;
	
	if (TF2 == 1)
	{
		TF2 = 0;
	  CP_Temps++;
	  if (CP_Temps >= 100) 
	  {
		  Flag_Seconde = 1; // mis à 1 chaque seconde
			CP_Temps = 0;
		  LED = ~LED;
	  }					 
	}
	// Sécurité: si EXF2 est à 1 - RAZ de EXF2 	
	if (EXF2 == 1)
	{
		EXF2 = 0;
	}
		VISU_INT_TIMER2 = 0;
}
#if 0
*****************************************************************************
    REPONSE AUX QUESTIONS
*****************************************************************************

ACTIVITE 2

a.	Mesurez la valeur de cette tension de référence obtenue sur les broches VREFD, VREF0 et VREF1.

b.  Calcul de valeurs théoriques obtenues en sortie d'ADC0
                                             Hexadécimal   --  Décimal                
                VIN = 0V   --> Valeur ADC =       ____       --   ______
								VIN = 0,5V --> Valeur ADC =       ____       --   ______
								VIN = 1V   --> Valeur ADC =       ____       --   ______
							  VIN = 1,5V --> Valeur ADC =       ____       --   ______
								VIN = 2V   --> Valeur ADC =       ____       --   ______
							  VIN = 2,5V --> Valeur ADC =       ____       --   ______
								VIN = 3V   --> Valeur ADC =       ____       --   ______

c.	Sur quelle voie d’entrée (AIN0 à AIN7) la conversion va-elle-être réalisée ?

d.	Quelle est le gain en tension de l’amplificateur placé en amont du convertisseur ADC0 ?

e.	Quel est le temps de conversion de l’ADC0 ?

f.	Comment la conversion est-elle déclenchée ?

g.	Comment le résultat de conversion est-il stocké dans les registres ADC0H et ADC0L ?

2.	Relever les valeurs obtenues réellement en sortie d'ADC0 pour les tensions d’entrées
    suivantes : 0V, 0,5V, 1V, 1,5V, 2V, 2,5V, 3V. 
           
					      VIN = 0V   --> Valeur ADC =     
								VIN = 0,5V --> Valeur ADC =      
								VIN = 1V   --> Valeur ADC =     
							  VIN = 1,5V --> Valeur ADC =      
								VIN = 2V   --> Valeur ADC =  
							  VIN = 2,5V --> Valeur ADC =      
								VIN = 3V   --> Valeur ADC =   
								
ACTIVITE 4

6.	Pour un signal d’entrée à 1Khz sur Ain0, combien a-t-on de points de conversion
    sur une période du signal d’entrée (observation du signal DAC0) ?
		
7. Pour un signal de 9KHz en entrée, qu’observe-t-on en sortie du DAC0 ? 
   Quelle est sa fréquence ? Conclure.	

								
#endif