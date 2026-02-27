//******************************************************************************
//TP_8051_Lib.asm
//   Ce fichier contient des définitions et des routines utiles au TP
//  Microcontrôleur
//  Aucune modification n'est nécessaire.
//  Il suffit d'ajouter ce fichier à votre Projet
//  TARGET MCU  :  C8051F020 

//******************************************************************************
#include "C8051F020.h"
#include <c8051F020_SFR16.h>
// Peripheral specific initialization functions,
// Called from the Init_Device() function

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
	   // P0.0  -  TX0 (UART0), 
    // P0.1  -  RX0 (UART0), 
    // P0.2  -  CEX0 (PCA), 
    // P0.3  -  CEX1 (PCA),  
    // P0.4  -  CEX2 (PCA), 
    // P0.5  -  CEX3 (PCA),  
    // P0.6  -  ECI,        
    // P0.7  -  T0 (Timer0), 

    // P1.0  -  INT0 (Tmr0), 
    // P1.1  -  T1 (Timer1), 
    // P1.2  -  INT1 (Tmr1),
    // P1.3  -  T2 (Timer2), 
    // P1.4  -  T2EX (Tmr2), 
    // P1.5  -  SYSCLK,     
    // P1.6  to P3.7 -  Unassigned,  

	  XBR0      = 0x64;
    XBR1      = 0xFE;
    XBR2      = 0x40;
		
// **** Configuration des broches IN/OUT PP/OD ***	
// ATTENTION: la configuration ci-dessous est un objectif pédagogique
// pour ce TP. Elle ne correspond pas à une configuration classique

    P0MDOUT   = 0x85;
    P1MDOUT   = 0x40;
    P2MDOUT   = 0x00;
    P3MDOUT   = 0x00;
    P74OUT    = 0x01;
    P1MDIN    = 0xFF;
    P2        = 0;
    P1        = 0;
	  P3        = 0;
		P4        = 0;
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



	