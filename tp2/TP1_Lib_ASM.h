/*---------------------------------------------------------------------------
; 	FILE NAME  	: TP1_Lib_ASM.h
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: 
;
; 	REVISION 1.0
;
;---------------------------------------------------------------------------*/

#ifndef __TP1_Lib_ASM__
#define __TP1_Lib_ASM__



void Config_Timer3_BT(void);
void   tempo(unsigned int tempo);
void   tempo_20US(void);
void   tempo_100US(void);
unsigned char tempo_sup(unsigned char unite, unsigned int valeur);

#endif