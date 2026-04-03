/******************************************************************************
; 	FILE NAME  	: Lib_Base.c
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Fonctions diverses mises à disposition
;
; 	REVISION 1.0
*/
//******************************************************************************

#include "C8051F020.h"
#include "intrins.h"
#include "c8051F020_SFR16.h"

#define SYSCLK 22118400L
#define BAUDRATE 115200L
#define Freq_timer0 (BAUDRATE*16)

#define Preload_Timer1 (SYSCLK/Freq_timer0)
#if Preload_Timer1 > 255 
#error "Valeur Preload Timer0 HORS SPECIFICATIONS"
#endif 

//-----------------------------------------------------------------------------
// Dévalidation du watchdog
//-----------------------------------------------------------------------------
void Reset_Sources_Init()
{
    WDTCN     = 0xDE;
    WDTCN     = 0xAD;
}

//-----------------------------------------------------------------------------
// Configuration des Ports d'entrée-sorties
//-----------------------------------------------------------------------------

void Port_IO_Init()
{
    // P0.0  -  TX0 (UART0), Push-Pull, Digital
    // P0.1  -  RX0 (UART0), Open-Drain, Digital
    // P0.2  -  TX1 (UART1), Push-Pull, Digital
    // P0.3  -  RX1 (UART1), Open-Drain, Digital
    // P0.4  -  T0 (Timer0), Open-Drain, Digital
    // P0.5  -  INT0 (Tmr0), Open-Drain, Digital
    // P0.6  -  T1 (Timer1), Open-Drain, Digital
    // P0.7  -  INT1 (Tmr1), Open-Drain, Digital

    // P1.0  -  T2 (Timer2), Open-Drain, Digital
    // P1.1  -  T2EX (Tmr2), Open-Drain, Digital
    // P1.2  -  T4 (Timer4), Open-Drain, Digital
    // P1.3  -  T4EX (Tmr4), Open-Drain, Digital
    // P1.4  -  SYSCLK,      Push-Pull, Digital  Visu Horloge Système
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  General Purpose I/O, Push-Pull, Digital   Port LED
    // P1.7  -  General Purpose I/O, Open-Drain, Digital

    P0MDOUT   |= (1<<0)|(1<<2); 
    P1MDOUT   |= (1<<4)|(1<<6);
	
    P3MDOUT   |= (1<<4)|(1<<5);
   
    P74OUT    = 0x00;
    P1MDIN    = 0xFF;

    XBR0      = 0x04;
    XBR1      = 0xFE;
    XBR2      = 0x5C;
}

//-----------------------------------------------------------------------------
// Configuration de l'oscillateur - SYSCLK = Clock externe 22118400Hz 
//-----------------------------------------------------------------------------
void Oscillator_Init()
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x0C;
}


//-----------------------------------------------------------------------------
// Initialisation globale du Microcontrôleur - 
//-----------------------------------------------------------------------------
void Init_Device(void)
{
    Reset_Sources_Init();
    Port_IO_Init();
    Oscillator_Init();
}

//*****************************************************************************	 
//*****************************************************************************	 
// Software_Delay_10ms -- Temporisation 10ms
//  Pour SYSCLK = 2 MHz - Niveau optimisation 0 du compilateur
//  Vérifiée en  simulation
void Software_Delay_10ms(void)
   { 
	 unsigned int i;
	 for(i=0;i<998;i++){}
	 }
	 
//*****************************************************************************	 
//*****************************************************************************	 
// Software_Delay -- Temporisation paramétrable
//      Pour SYSCLK = 2 MHz - Niveau optimisation 0 du compilateur
//      Vérifiée en  simulation
//      L'argument passé en exprimé en centièmes de seconde (dans l'hypothèse 
//      où SYSCLK = 2MHz)
	 
 void Software_Delay(unsigned int hundredth_second)
   { 
	 unsigned int i;
	 
	 for(i=0;i<hundredth_second;i++)
      { 
	    Software_Delay_10ms();
			}
	 }

//*****************************************************************************	 
//*****************************************************************************	 
// Software_Delay_10micro -- Temporisation 10 microsecondes
//  Pour SYSCLK = 2 MHz - Niveau optimisation 0 du compilateur
//  Vérifiée en  simulation
void Software_Delay_10micro(void)
   { 
		// L'instruction assembleur NOP est introduite directement dans le code.
    // 1 NOP "consomme" 1 cycle CPU		 
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
   _nop_();
   _nop_();		
   _nop_();			 
	 }

	 //*****************************************************************************	 
//*****************************************************************************	 
// Delay_CLK22M_10micro -- Temporisation 10 microsecondes
//  Pour SYSCLK = 22,1184 MHz - Niveau optimisation 0 du compilateur
//  Vérifiée en  simulation
void Delay_CLK22M_10micro(void)
   { 
unsigned char i;
     for (i=0;i<10;i++);		 
	 }
//*****************************************************************************	 
//******************************************************************************
void Config_Timer2_TimeBase(void)
{
	CKCON &= ~(1<<5);         // T2M: Timer 2 Clock Select
                            // CLK = sysclk/12   
	TR2 = 0;  //Stop Timer
	TF2 = 0;  // RAZ TF2
	EXF2 = 0;  // RAZ EXF2
  RCLK0 = 0;         
  TCLK0 = 0;
  CPRL2 = 0;  // Mode AutoReload	
	EXEN2 = 0;   // Timer2 external Enable Disabled 
  CT2 = 0;    // Mode Timer

	RCAP2 = -((SYSCLK/12)/100); // Interruption à 100Hz
  T2 = RCAP2;
  TR2 = 1;                 // Timer2 démarré
  PT2 = 1;							  // Priorité Timer2 Haute

   ET2 = 1;							  // INT Timer2 autorisée
}

//*****************************************************************************	 
//CFG_clock_UART
//	Utilisation du Timer 1
//
// 
//*****************************************************************************	 
void CFG_Clock_UART0(void)
{
    CKCON |= (1<<4);             // T1M: Timer 1 Clock Select
	                               // Timer CLK = SYSCLK
	
    TMOD |= (1<<5);             
	  TMOD &= ~((1<<7)|(1<<6)|(1<<4));			  // Timer1 configuré en Timer 8 bit avec auto-reload	
	  TF1 = 0;				  // Flag Timer effacé
    TH1 = -(Preload_Timer1);
	  ET1 = 0;				   // Interruption Timer 1 dévalidée
	  TR1 = 1;				   // Timer1 démarré
}
 
//*****************************************************************************	 
//CFG_uart0_mode1
//
//
// 
//*****************************************************************************	 
void CFG_UART0(void)
{
		RCLK0 = 0;   // Source clock Timer 1
		TCLK0 = 0;
		PCON  |= (1<<7); //SMOD0: UART0 Baud Rate Doubler Disabled.
		PCON &= ~(1<<6); // SSTAT0=0
		SCON0 = 0x70;   // Mode 1 - Check Stop bit - Reception validée
		               
 //   TI0 = 0;     // Drapeaux TI et RI à zéro
    RI0 = 0;	
	  TI0 = 1;  //à cause du codage de putchar
	
	  ES0 = 0;  // Interruption UART autorisée
}
//*****************************************************************************	 
char putchar(char c)
{
	while(TI0==0);
	TI0 = 0;
	SBUF0 = c;
	return c;
}

//*****************************************************************************	 
char _getkey(void)
{
  char c;
	while(RI0==0);
	RI0 = 0;
	c = SBUF0;
	return c;
	
}

//*****************************************************************************	 
char getkey_one_time(void)
	
{
  
	if (RI0==0) return 0;
	else
  {
	   RI0 = 0;
	   return SBUF0;
	}
}
