;******************************************************************************
;Lib_TP_SM.asm

;   Ce fichier contient des d�finitions et des routines utiles au TP
;   Aucune modification n'est n�cessaire.
;   Il suffit d'ajouter ce fichier � votre Projet
;   TARGET MCU  :  C8051F020 
;******************************************************************************
$INCLUDE(C8051F020.INC)	; Register definition file.
;******************************************************************************
;Declaration des variables et fonctions publiques
;******************************************************************************
PUBLIC   Config_Timer3_BT
PUBLIC   _tempo
PUBLIC   _tempo_20US
PUBLIC   _tempo_100US
PUBLIC   _tempo_sup
;******************************************************************************
;D�clarations de p�riph�riques

GREEN_LED      EQU       P1.6
BP             EQU       P3.7

;******************************************************************************
;Consignes de segmentation
;******************************************************************************
zone_data     segment  DATA
              rseg     zone_data
tampon:       ds       6

Lib_TP_SM     segment  CODE
			  rseg     Lib_TP_SM     ; Switch to this code segment.
              using    0             ; Specify register bank for the following
                                     ; program code.

;******************************************************************************
;Initialisations de la m�moire code - Stockage de constante
;******************************************************************************


;******************************************************************************
; Gestion du Timer 3
; Routines permettant de mettre en oeuvre le timer 3
;
;******************************************************************************

;******************************************************************************                
; Config_Timer3_BT
;
; Description: Sous-programme configurant le Timer3 pour fonctionner en
;              Re-chargement automatique. Le timer va g�n�rer un overflow 
;              toutes les ((65535-N) * 0.542) micro-secondes. 
;
; Param�tres d'entr�e:  N dans R6(MSB) et R7(LSB)
; Param�tres de sortie: aucun
; Registres modifi�s: TMR3RLL,TMR3RLH,TMR3H,TMR3L,TMR3CN
; Pile: 0 octet (sauf pour l'appel de la sous -routine)
;******************************************************************************
; ATTENTION!!!   Il est imp�ratif de remettre � z�ro, par programme, le 
;*************   Timer3 Overflow Flag.  
;******************************************************************************

Config_Timer3_BT:


         mov TMR3RLL, R7   ; Timer 3 Reload Register Low Byte
         mov TMR3RLH, R6   ; Timer 3 Reload Register High Byte
         mov TMR3H, R6     ; Ti mer 3 High Byte
         mov TMR3L, R7     ; Timer 3 Low Byte
         mov TMR3CN, #006h ; Timer 3 Control Register
         ret

;******************************************************************************
;******************************************************************************
; _tempo
;
; Description: Sous-programme produisant une temporisation logicielle
;              param�trable par la variable csg_tempo.
;              La temporisation g�n�r�e est �gale � csg_tempo micro-secondes.
;              ATTENTION: csg_tempo ne doit pas �tre inf�rieure � 2
;              La temporisation est calcul�e pour une horloge SYSCLK de 22,1184 MHz
;              
; Param�tres d'entr�e:  csg_tempo dans R6(MSB) et R7(LSB)
; Param�tres de sortie: aucun
; Registres modifi�s: R6 et R7
; Pile: 2 octets 
;******************************************************************************
_tempo:
        PUSH  ACC
        MOV   A,R5
        PUSH  ACC
        MOV  A,R7
        DEC  R7
        JNZ   ?C0006
        DEC   R6
?C0006:
?C0001:
         MOV  A,R7
         ORL  A,R6
         JZ   ?C0005

         MOV  A,R7
         DEC  R7
         JNZ  ?C0007
         DEC  R6
?C0007:
         MOV  R5,#1  ; chaque incr�ment augmente la dur�e du SP de 0,316 US
		             ;pour SYSCLK = 22,1184 MHz
?C0003:
         MOV  A,R5
         JZ   ?C0001
         DEC  R5
        SJMP ?C0003
?C0005:
         POP ACC
         MOV R5,ACC
         POP ACC  
         RET  	
		 
;******************************************************************************
;******************************************************************************
; __tempo_20US
;
; Description: Sous-programme produisant une temporisation logicielle de 20US (environ)
;             
;
; Param�tres d'entr�e:  aucun
; Param�tres de sortie: aucun
; Registres modifi�s: aucun
; Pile: 2 octets 
;******************************************************************************
_tempo_20US:
        PUSH    AR6
		PUSH    AR7
		MOV    	R7,#20 ; Passage de param�tres avant l'appel de __tempo
 		MOV    	R6,#00
		CALL   	_tempo
        POP     AR7
        POP     AR6		
        RET  	
;******************************************************************************
;******************************************************************************
; __tempo_100US
;
; Description: Sous-programme produisant une temporisation logicielle de 100US (environ)
;             
;
; Param�tres d'entr�e:  aucun
; Param�tres de sortie: aucun
; Registres modifi�s: aucun
; Pile: 2 octets 
;******************************************************************************
_tempo_100US:
        PUSH    AR6
		PUSH    AR7
		MOV    	R7,#100 ; Passage de param�tres avant l'appel de __tempo
 		MOV    	R6,#00
		CALL   	_tempo
        POP     AR7
        POP     AR6		
        RET  	
;******************************************************************************
;  _tempo_sup:
; Description: Sous-programme produisant une temporisation logicielle
;              param�trable par 2 variables multiplicateur et valeur.
;            
;              La temporisation est calcul�e pour une horloge SYSCLK de 22,1184 MHz
;              
; Param�tres d'entr�e:  valeur dans R4(MSB) et R5(LSB)
;                        multiplicateur dans R7 (code ASCII)
;                       si multiplicateur = 'U' tempo = valeur micro-secondes
;                       si multiplicateur = 'M' tempo = valeur * 1000 micro-secondes
;                       si multiplicateur = 'S' tempo = valeur * 1000 000 micro-secondes
;                       si multiplicateur = autre valeur - sortie avec code erreur
; Param�tres de sortie: R7 (R7 = 0 si OK, sinon retourne un code erreur 
; Registres modifi�s: R6 et R7
; Pile: 2 octets 
;
;
;******************************************************************************
#define Unite_incorrecte 1
#define Valeur_nulle 2
_tempo_sup:
       PUSH PSW
	   PUSH ACC
	   PUSH DPL
	   PUSH DPH
	  
	   PUSH AR6
	   PUSH AR0
	   PUSH AR1
     
       MOV A,R4
       JNZ suit
       MOV A,R5
       JNZ suit
	   MOV R7,#Valeur_nulle
	   JMP fin
suit:	   
       CJNE R7,#'S',TRT_non_S
TRT_S:	   
	   mov  DPTR,#0
	  
TRT_S1:mov  R6,AR5 ; consigne valeur multipli�e par 256
	   mov  R7,#0
	   call _tempo
	   inc  DPTR
	   mov A,#0FH  ; valeur test�e 3906 car 3906*256 = 1 000 000 (environ)
	   CJNE A,DPH,TRT_S1
	   mov A,#042H
	   CJNE A,DPL,TRT_S1  
       mov R7,#0	   
	   JMP fin
TRT_non_S:
       CJNE R7,#'M',TRT_non_M
TRT_M:	   
       mov DPTR,#0
TRT_M1:	   mov  R6,AR4
	   mov  R7,AR5
	   call _tempo
	   inc  DPTR
	   mov A,#03H   ;valeur test�e 1000 
	   CJNE A,DPH,TRT_M1
	   mov A,#0E8H
	   CJNE A,DPL,TRT_M1
	   mov R7,#0
	   JMP fin
TRT_non_M:
       CJNE R7,#'U',TRT_non_U
TRT_U:	
       mov  R6,AR4
	   mov  R7,AR5
	   call _tempo
	   mov R7,#0
	   JMP  fin
TRT_non_U:
       MOV R7,#Unite_incorrecte
	  
fin:	
       POP AR1
	   POP AR0
	   POP AR6
	   POP DPH
	  
	   POP DPL
	   POP ACC
	   POP PSW
       
       RET


end