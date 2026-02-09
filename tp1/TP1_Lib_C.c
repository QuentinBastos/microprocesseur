/******************************************************************************
; 	FILE NAME  	: TP4_Lib_Config_Globale_8051F020.C
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Configurations GLOBALES pour le 8051F020
;
; 	REVISION 1.0
;   Ce fichier contient les codes de configuration globale du 8051F020
;     - Configuration de l'horloge système SYSCLK
;     - Configuration des broches du 80851F020
;     - Configuration Reset et watchdog
*/
//******************************************************************************

#include "C8051F020.h"
#include "c8051F020_SFR16.h"
#include "TP1_lib_C.h"

// Peripheral specific initialization functions,
// Called from the Init_Device() function

//-----------------------------------------------------------------------------
// Dévalidation du watchdog
//-----------------------------------------------------------------------------
void Reset_Sources_Init()
{
	// La configuration des registres WDTCN  sera étudiée plus tard
	 WDTCN = 0xDE;
	 WDTCN = 0XAD;
	 RSTSRC = 0;
}
//-----------------------------------------------------------------------------
// configuration de l'horloge système
//  Quartz externe à 22,1184 MHz
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
// Configuration des Ports d'entrée-sorties
//-----------------------------------------------------------------------------

void Port_IO_Init()
{
    // P0.0  -  Congiguration PIO - TX0 (UART0)
    // P0.1  -  Congiguration PIO - RX0 (UART0)
    // P0.2  -  Congiguration PIO - INT0 (Tmr0)
    // P0.3  -  Congiguration PIO - INT1 (Tmr1)
    // P0.4  -  Congiguration PIO - SYSCLK
    // P0.5 à P7.7 	- Mode GPIO par défaut 
 
// La configuration des registres XBR
    XBR0      = 0x04; // UART0 routée sur P0.0 et P0.1
    XBR1      = 0x94; // INT0 routée sur P0.2, INT1 sur P0.3 et Sysclock routée sur P0.4
    XBR2      = 0x40; //  Crossbar validé

// Port configuration (1 = Push Pull Output)
    P0MDOUT =0x11;    // Output configuration for P0 
    P1MDOUT = 0x40;   // Output configuration for P1  -- Drain ouvert sauf P1.6
    P2MDOUT = 0;   // Output configuration for P2  -- Drain ouvert
    P3MDOUT = 0;   // Output configuration for P3  -- Drain ouvert
    P74OUT = 0;    //  P4, P5, P6 et P7 configurés en drain ouvert
    P1MDIN = 0xFF;  // Pas d'entrée analogique sur P1
    P4 = 0xDF;       // ALE=0, /RD=1, /WR=1 en dehors des cycles mémoires
		P0 = (1<<2)|(1<<3); // Broche P0.2 - INT0 Configurée en entrée
         		            // Broche P0.3 - INT1 Configurée en entrée
		P1 &= ~(1<<6);   // P1.6 mis à zéro
	  P1MDOUT |= (1<<6);  // P1.6  configuré en Push Pull

	
// La configuration qui suit n'a qu'un intêret pédagogique...	
	   P3 = 0;
	   P3MDOUT = 0xFF;
// Fin de la config à intêret pédagogique
}
//-----------------------------------------------------------------------------
// Configuration des 2 DAC
//-----------------------------------------------------------------------------
void DAC_Init()
{
	  REF0CN = 0x03; //; VREF ON
    DAC0CN    = 0x84; //; DAC0 validé - 12 bits à gauche - Update: write to DAC
	  DAC1CN    = 0x84; //; DAC1 validé - 12 bits à gauche - Update: write to DAC
}
//-----------------------------------------------------------------------------
// Envoi d'une valeur 12 bits dans le DAC0
//-----------------------------------------------------------------------------
void send_to_DAC0(unsigned int rampe)
{
	  rampe = (rampe<<4);
    DAC0L = rampe % 256;
    DAC0H = rampe / 256;	 
}
//-----------------------------------------------------------------------------
// Initialisation globale du Microcontrôleur - 
//Description: Initialisations de périphériques
//              Fonctionnalités Microcontroleur
//              Configuration accès mémoire sur P4,5,6 et 7
//             Acces en mode multiplexé.
//             Sorties en Push-Pull sauf pour P4 en Drain ouvert
//             Config SYSCLK:  Quartz externe
//-----------------------------------------------------------------------------
void Init_Device(void)
{
    Reset_Sources_Init();
	  Oscillator_Init();
    Port_IO_Init();
}