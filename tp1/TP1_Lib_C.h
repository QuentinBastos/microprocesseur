/*---------------------------------------------------------------------------
;
; 	FILE NAME  	: TP1_Lib_C.H
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Configuration pour le TP Archi
;
; 	REVISION 1.0
;
;---------------------------------------------------------------------------*/

#ifndef __TP1_Lib_C__
#define __TP1_Lib_C__


void Init_Device (void);
void Reset_Sources_Init();
void Port_IO_Init();
void Oscillator_Init(void);
void DAC_Init(void);
void EMI_Init(void);
void DAC_Init(void);
void send_to_DAC0(unsigned int rampe);
#endif