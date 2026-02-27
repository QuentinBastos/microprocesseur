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
unsigned char Gestion_chenillard(unsigned char Cfg_Chenillard,bit Nvelle_valeur);
void Config_Timer3(void);
void	Config_ADC(void);
void	Config_INT0(void);
void	Config_Timer4(void);
#endif