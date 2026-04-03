//------------------------------------------------------------------------------------
// Main_TP4_IRC
//------------------------------------------------------------------------------------
//
// AUTH: FJ
// DATE: 20-02-2025
// Target: C8051F02x
//
// Tool chain: KEIL Eval 'c'
//
//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <c8051f020.h>                    // SFR declarations
#include <c8051F020_SFR16.h>
#include <TP1_Lib_ASM.h>
#include <Lib_Base.h>

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------

#define SYSCLK 22118400 //approximate SYSCLK frequency in Hz
#define LED_On 1
#define LED_Off 0
#define  Slow  100
#define  Fast  10
#define  Baudrate 115200L
#define CLK_UART (Baudrate*16)
// Elements presents sur la carte 8051F020
sbit  LED = P1^6;         // LED verte: '1' = ON; '0' = OFF
sbit BP = P3^7;           // Bouton Poussoir '1' relache, '0' presse

 unsigned int Value_tempo = Fast;
//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void Config_INT7(void);
void Config_Timer3(void);
void Config_Timer2(void);
void CFG_clock_UART_VerT1(void); // Activite 4 : Timer 1 comme source horloge UART0
void Config_UART0_mode1(void);
void UART0_Send_Char(char c);

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void) {

    Init_Device();
    Config_Timer3();
    Config_INT7();
    // Config_Timer2 // activité 3 : Timer 2 comme generateur de baud rate
    CFG_clock_UART_VerT1(); // Activite 4 : Timer 1 comme baud rate generator (19200 Baud)
    Config_UART0_mode1();   // Configuration de l'UART0 en mode 1

    LED = LED_Off;  // LED eteinte

    // Config P4.0 a P4.3 en sortie
    P74OUT |= (1<<0); // P4.0 a P4.3 configurees en Sortie Push Pull
    // Config P4.4 a P4.7 en entree
    P74OUT &= ~(1<<1); // P4.4 a P4.7 en OD
    P4 |= (1<<4)|(1<<5)|(1<<6)|(1<<7);

    // Configuration de BP (P3.7) en entree
  BP = 1;
  P3MDOUT &= ~0x80;  // P3.7 place en Open Drain

    EA = 1;  // Activation globale des interruptions

  while (1)
  {
    // AJOUT : Envoi du caractere 'A' en boucle via l'UART0
    // On peut verifier la reception sur PC avec un terminal serie (ex: PuTTY, 115200 baud)
    UART0_Send_Char('A');

    // Pause de 10 ms entre chaque envoi pour ne pas saturer la liaison serie.
    // tempo() est la fonction assembleur de TP1_Lib_ASM.asm, son argument est en microsecondes.
    tempo(10000);  // 10 000 us = 10 ms
  }
}

 //*****************************************************************************
 //***************************************************************************************
void Config_INT7(void)
{
    // Registres: P3IF
    P3IF &= ~0x08; // INT7 active front descendant
    P3IF &= ~0x80; // RAZ Flag INT7

    EIE2 |= 0x20;  // INT7 autorisee
    EIP2 &= ~0x20; // Priorite basse
}

//***************************************************************************************
void ISR_INT7(void) interrupt 19
{
      P4 |= (1<<0);   // Drapeau materiel INT7 sur P4.0 (B23) mis a 1
      P3IF &= ~(1<<7); // RAZ Flag INT7
      // Gestion vitesse de clignotement de la LED
      if (Value_tempo == Slow) Value_tempo = Fast;
      else Value_tempo = Slow;
        P4 &= ~(1<<0);   // Drapeau materiel INT7 sur P4.0 mis a 0
}
//*****************************************************************************
void Config_Timer3(void)
{
    TMR3CN    = 0x04;
    TMR3RL   = 0xB800;
      EIE2      |= (1<<0); // autorisation INT Timer3
      EIP2       |= (1<<0); // Priorite haute
}
//*****************************************************************************
// Interruption periodique declenchee par le timer 3 (toutes les 10 ms)
// ****************************************************************************
 void ISR_Timer3(void) interrupt 14
 {
     static unsigned int Tick_counter = 0;

     P4 |= (1<<2); // flag mis a 1 sur P4.2 (C22)
     TMR3CN    &= ~(1<<7);  // RAZ du flag d'interruption Timer3
     Tick_counter++;
     if (Tick_counter >= Value_tempo)
     {
          Tick_counter = 0;
          LED = ~LED;  // Inversion etat LED = clignotement
     }
     P4 &= ~(1<<2);  // flag mis a 0 sur P4.2 (C22)
 }
//*****************************************************************************
//******************************************************************************
void Config_Timer2(void)
{
    // Mode Timer2: Baud rate Generator - CLK Timer = SYSCLK/2

    TR2 = 0;  // Precaution: Stop Timer avant de le configurer

    TF2 = 0;   // RAZ TF2
    EXF2 = 0;  // RAZ EXF2
  RCLK0 = 1; // Timer 2 = source d'horloge Reception UART0
  TCLK0 = 1; // Timer 2 = source d'horloge Transmission UART0
  CPRL2 = 0; // Mode AutoReload (inutile en mode Baud Rate, mais bonne pratique)
    EXEN2 = 0; // Timer2 external Enable Disabled
  CT2 = 0;   // Mode Timer (et non compteur d'evenements)

    // Calcul de la valeur de rechargement :
    // On veut une periode de debordement = 1 / (Baudrate * 16)
    // RCAP2 = 65536 - (SYSCLK/2) / (Baudrate*16)
    RCAP2 = 65536L - ((SYSCLK/2)/CLK_UART);
  T2 = RCAP2;  // Precharge du timer avec la valeur de reload

    TR2 = 1;  // Timer2 demarre
}

//*****************************************************************************
// Config_UART0_mode1 : Configure l'UART0 en mode 1
//
// Mode 1 = transmission asynchrone 8 bits :
//   - 1 start bit (niveau bas)
//   - 8 bits de donnees
//   - 1 stop bit (niveau haut)
//   Le baud rate est fourni par le Timer2 (deja configure a 115200 baud).
//*****************************************************************************
void Config_UART0_mode1(void)
{
    // Configuration du registre SCON0 (Serial CONtrol register 0)
    // Le Timer2 doit avoir ete configure avant cet appel (fait dans main)
    //
    //  Bit 7 : SM00 = 0  \
    //                     +--> Mode 1 : UART 8 bits a baud rate variable
    //  Bit 6 : SM10 = 1  /      (si SM00=0 et SM10=1 -> mode 1)
    //  Bit 5 : SM20 = 0  --> Pas de communication multiprocesseur
    //  Bit 4 : REN0 = 1  --> Reception autorisee (bonne pratique, meme si on ne recoit pas)
    //  Bit 3 : TB80 = 0  --> Inutilise en mode 1
    //  Bit 2 : RB80 = 0  --> Inutilise en mode 1
    //  Bit 1 : TI0  = 1  --> FLAG DE FIN DE TRANSMISSION mis a 1 MANUELLEMENT
    //                         Cela est indispensable pour debloquer le 1er appel a UART0_Send_Char()
    //                         car cette fonction attend que TI0 soit a 1 avant d'envoyer.
    //  Bit 0 : RI0  = 0  --> Flag de fin de reception, on le laisse a 0
    //
    //  En binaire : 0101 0010 = 0x52
    SCON0 = 0x52;
}

//*****************************************************************************
// UART0_Send_Char : Envoie un caractere (1 octet) sur l'UART0
//   Parametre : c = le caractere a envoyer
//
// Principe :
//   1. On ecrit le caractere dans SBUF0 -> la transmission demarre automatiquement
//   2. On attend que le hardware finisse (il met TI0 a 1 quand c'est fini)
//   3. On remet TI0 a 0 pour que la prochaine attente fonctionne correctement
//*****************************************************************************
void UART0_Send_Char(char c)
{
    // Ecrire dans SBUF0 declenche immediatement la transmission serie du caractere.
    // Le hardware ajoute seul le start bit, les 8 bits de data et le stop bit.
    SBUF0 = c;

    // On attend la fin de la transmission.
    // TI0 (Transmit Interrupt flag 0) est mis a 1 PAR LE HARDWARE quand le dernier bit
    // (stop bit) vient d'etre envoye.
    while (!TI0);

    // On remet TI0 a 0 manuellement : le microcontroleur ne le fait pas automatiquement.
    // Si on oublie ce RAZ, le prochain while(!TI0) ne bloquera pas et on enverra
    // un caractere avant que la ligne soit libre.
    TI0 = 0;
}

void CFG_clock_UART_VerT1(void)
{
    TR1 = 0;   // Stop Timer 1

    // Desactiver Timer 2 comme source horloge UART0
    RCLK0 = 0;
    TCLK0 = 0;

    // Timer 1 : mode 2 (8 bits auto-reload)
    // TMOD : bits 7-4 concernent Timer 1 -> 0010 xxxx = 0x20
    TMOD = (TMOD & 0x0F) | 0x20;

    TH1 = 0xFA;  // Valeur de reload : 256 - 6 = 250
    TL1 = 0xFA;  // Precharge

    // Activer Timer 1 comme source horloge UART0
    // Sur C8051F020, le Timer 1 overflow alimente l'UART0 quand RCLK0=TCLK0=0 (source par defaut)
    TR1 = 1;   // Demarrage Timer 1
}

// Activité 2 :
// La réponse est directement visible sur la page 1 de la datasheet, dans la section Features :
//  Le STM32L010R8 dispose donc de 2 interfaces série de type UART : USART et LPUART


// Activité 3 :
// On configure le Timer 2 comme generateur de baud rate a 115200 Baud (RCAP2 = 65536 - 6 = 65530).
// L'UART0 est ensuite configure en mode 1 : 8 bits de donnees, 1 start bit, 1 stop bit (SCON0 = 0x52).
// Dans la boucle principale, on envoie le caractere 'A' toutes les 10 ms via UART0_Send_Char().
// Information : Sur le PC de la salle, on ne pouvait pas utiliser Putty (port COM non détécté, on a donc fait en mdoe simmulation avec UART #1)

// Activité 4 :
// On remplace le Timer 2 par le Timer 1 comme source horloge UART0 a 19200 Baud.
// Timer 1 mode 2 (8 bits auto-reload), cadence SYSCLK/12 : TH1 = 256 - 6 = 0xFA
