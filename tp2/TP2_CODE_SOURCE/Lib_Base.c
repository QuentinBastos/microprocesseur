//******************************************************************************
//Lib_Base
//   Ce fichier contient des définitions et des routines utiles au TP
//  Microcontrôleur
//  Aucune modification n'est nécessaire.
//  Il suffit d'ajouter ce fichier à votre Projet
//  TARGET MCU  :  C8051F020 

//******************************************************************************
#include "C8051F020.h"

#define SYSCLK 22118400 //approximate SYSCLK frequency in Hz
#define LED_On 1
#define LED_Off 0
#define Oui 1
#define Non 0
#define BP_ON 0
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
	  XBR0      = 0x54;
    XBR1      = 0xFE;
    XBR2      = 0x40;
		
// **** Configuration des broches IN/OUT PP/OD ***	
// ATTENTION: la configuration ci-dessous est un objectif pédagogique
// pour ce TP. Elle ne correspond pas à une configuration classique

    P0MDOUT   = 0x85;
    P1MDOUT   = 0x00;
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

//------------------------------------------------------------------------------------
// Gestion_chenillard
// Appelée à intervalle de temps régulier elle se charge de gérer le chenillard.
// Paramètres passés:
//   - Cfg_Chenillard : contient sur 8 bits la config du chenillard
//                      - Bit 0 : sens de défilement
//                      - Bit 1 : Marche (1)  / Arrêt (0)
//                      - Bit 2-3-4 : Vitesse 1 à 7
//                      - Bit 5-6-7 : Défilement de 1 à 7 bits
//    - Nvelle valeur: Si (1) impose un changement de configuration
//  Valeur retournée: valeur à envoyer au chenillard
//------------------------------------------------------------------------------------
unsigned char Gestion_chenillard(unsigned char Cfg_Chenillard,bit Nvelle_valeur)
{
static unsigned char  CP_Chenillard=0;
static unsigned char Valeur_Chenillard = 1;	
static bit sens = 0;
static bit marche = 1;	
static unsigned char vitesse = 1;
static unsigned char value = 1;
unsigned char i,sum;	
	
    if (Nvelle_valeur == Oui) // Si lecture d'une nvelle confif
			                        // alors décodage de la nvelle config
		{
			 // Extraction de l'info "sens" sur le bit 0
			 if ((Cfg_Chenillard & (1<<0))==0) sens =0;
       else sens = 1;
			  // Extraction de l'info "marche" sur le bit 1
       if ((Cfg_Chenillard & (1<<1))==0) marche = Non;
       else marche = Oui;
			 // Extraction de l'info "vitesse" sur les bits 4-3-2
       vitesse = (Cfg_Chenillard & 0x1C)>>2;
       // Extraction de l'info "valeur"sur les bits 5-6-7
			value =  (Cfg_Chenillard & 0xE0)>>5;			
			 for (i=0,sum=0;i<value;i++)
       { sum += (1<<i); }	
			 Valeur_Chenillard = sum;
		}	
		// on ne change la sortie chenillard que 1 fois sur la valeur "vitesse"
    if (CP_Chenillard >= vitesse)
  	{
			CP_Chenillard = 0;
			if (sens == 0)
			{
     			if ((Valeur_Chenillard & (1<<7))==0) Valeur_Chenillard <<= 1;
          else  Valeur_Chenillard = (Valeur_Chenillard <<1) + 1; 				
			}
      else
      { 
				  if ((Valeur_Chenillard & (1<<0))==0) Valeur_Chenillard >>= 1;
          else  Valeur_Chenillard = (Valeur_Chenillard >>1) + 128; 				
      }	
      	
		}
    else CP_Chenillard++;	
		
		// Test: chenillard fonctionne ou pas
		if (marche == Oui)   return ~Valeur_Chenillard;	
		else  return 0xFF;
}	
