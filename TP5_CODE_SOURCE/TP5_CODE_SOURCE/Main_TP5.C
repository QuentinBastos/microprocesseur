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
#define  Baudrate 115200L
#define CLK_UART (Baudrate*16)
// Eléments présents sur la carte 8051F020
sbit  LED = P1^6;         // LED verte: '1' = ON; '0' = OFF
sbit BP = P3^7;           // Bouton Poussoir '1' relaché, '0' pressé

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
	Config_Timer3();
	Config_INT7();
	
	
	LED = LED_Off;  // LED éteinte
	
	// Config P4.0 à P4.3 en sortie
	P74OUT |= (1<<0); // P4.0 à P4.3 configurées en Sortie Push Pull
	// Config P4.4 à P4.7 en entrée
	P74OUT &= ~(1<<1); // P4.4 à P4.7 en OD
	P4 |= (1<<4)|(1<<5)|(1<<6)|(1<<7);
	
	// Configuration de BP (P3.7) en entrée	
  BP = 1;
  P3MDOUT &= ~0x80;  // P3.7 placé en Open Drain 	

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
	
	EIE2 |= 0x20;  // INT7 autorisée
	EIP2 &= ~0x20; //Priorité basse
}

//***************************************************************************************
void ISR_INT7(void) interrupt 19
{
	  P4 |= (1<<0);   // Drapeau matériel INT7 sur P4.0 (B23) mis à 1
	  P3IF &= ~(1<<7); // RAZ Flag INT7
	  // Gestion vitesse de clignotement de la LED
	  if (Value_tempo == Slow) Value_tempo = Fast;
	  else Value_tempo = Slow;
		P4 &= ~(1<<0);	 // Drapeau matériel INT7 sur P4.0 mis à 0
}
//*****************************************************************************
void Config_Timer3(void)
{
   	TMR3CN    = 0x04;
    TMR3RL   = 0xB800;
	  EIE2      |= (1<<0); // autorisation INT Timer3
	  EIP2       |= (1<<0); // Priorité haute
}
//*****************************************************************************
// Interruption périodique déclenchée par le timer 3
// ****************************************************************************
 void ISR_Timer3(void) interrupt 14  // Interruption toutes les 10 ms
 {
	 static unsigned int Tick_counter = 0;
		 
	 P4 |= (1<<2); // flag mis à 1 sur P4.2 (C22)
	 TMR3CN    &= ~(1<<7);
	 Tick_counter++;
	 if (Tick_counter >= Value_tempo)
	 { 	 
		  Tick_counter = 0;
		  LED = ~LED;
	 }	
	 P4 &= ~(1<<2);	 // flag mis à 0 sur P4.2 (C22)
 }	
//*****************************************************************************	 
//******************************************************************************
void Config_Timer2(void)
{
	// Mode Timer2: Baud rate Generator - CLK Timer = SYSCLK/2

	TR2 = 0;  //Précaution: Stop Timer avant de le configurer
	
	TF2 = 0;  // RAZ TF2
	EXF2 = 0;  // RAZ EXF2
  RCLK0 = 1;  // Timer 2 source d'horloge Réception UART0       
  TCLK0 = 1;  // Timer 2 source d'horloge Réception UART0     
  CPRL2 = 0;  // Mode AutoReload	(cette init est inutile en mode Baud Rate)
	EXEN2 = 0;   // Timer2 external Enable Disabled 
  CT2 = 0;    // Mode Timer

	RCAP2 = 65536L - ((SYSCLK/2)/CLK_UART);
  T2 = RCAP2;
	
	TR2 = 1;  // Timer2 démarré
}

