/*---------------------------------------------------------------------------
;
; 	FILE NAME  	: TP1_Lib_C.H
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Configuration pour le TP Archi
;
; 	REVISION 1.0
;
;---------------------------------------------------------------------------*/

#ifndef __Lib_Base__
#define __Lib_Base__


void Init_Device (void);
void Reset_Sources_Init();
void Port_IO_Init();
void Oscillator_Init(void);

#endif