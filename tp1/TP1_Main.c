//-----------------------------------------------------------------------------
// TP1_archi_3IRC.c
//-----------------------------------------------------------------------------
// AUTH: FJO
// DATE: 25/1/25
//
// Target: C8051F02x
// Tool chain: KEIL Microvision5
//
//-----------------------------------------------------------------------------
#include <c8051F020.h>
#include <c8051F020_SFR16.h>
#include <TP1_Lib_C.h>
#include <TP1_Lib_ASM.h>

// D�clarations Registres et Bits de l'espace SFR
#define LED_ON 1
#define LED_OFF 0

// Début activité 4
sbit P3_4 = P3^4;
sbit P3_5 = P3^5;
sbit P3_6 = P3^6;
// Fin activité 4

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void) {
	
unsigned int	rampe = 0;
unsigned int  escalier;	

unsigned int xdata tab[16];
unsigned int cp = 0;	
unsigned char i;	
	
	      Init_Device();
	      DAC_Init();
	
        while(1)
        {  
						 P1 |= (1<<6);
					  
					   while (rampe < 4096)
						 { 
							   send_to_DAC0(rampe);
							   rampe++;
							   tempo(10); 
					   }
						 
						 P1 &= ~(1<<6);
						 while (rampe != 0)
						 { 
							   rampe--;
							   escalier = rampe & 0x0F00;
							   send_to_DAC0(escalier);
							   tempo(10); 
					   }
						 for(i = 0;i<16;i++)
						 {  
                tab[i] = cp;
                cp++;							 
						 }  							 
									
        }						               	
			}
//*****************************************************************************	 
