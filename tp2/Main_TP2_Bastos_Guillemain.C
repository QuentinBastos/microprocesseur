//------------------------------------------------------------------------------------
// Base_TP3_IRC
//------------------------------------------------------------------------------------
//
// AUTH: FJ
// DATE: 09-03-2022
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


sbit LED_VERTE = P1^6; // LED verte de la carte
sbit BP = P3^7;        // Bouton Poussoir (1=relâché, 0=pressé)

void main (void) {

    // --- INITIALISATION GLOBALE ---
    Init_Device();

    // Problème : L'ajout de l'UART1 décalait SYSCLK de P1.3 vers P1.5.
    XBR2 |= 0x04; // Activation de l'UART1

    /* ACTIVITE 2 :
     * On peut observer que la LED ne clignote pas sur la carte en premier temps.
     * Ceci est du à la faible impudence qui fait que le signal n'est pas assez fort pour allumer la LED (de plus les ports du 8051 sont en open drain de base
     * dans ce mode, la broche peut imposer un niveau bas donc 0V, elle ne peut pas pousser de courant à haut niveau donc haute impudence)..
     * Cepandant, nous pouvons observer des signaux periodiques à l'aide de loscilataure, nous remarquons que la fréquence des
     * signaux n'est pas compatible avec une fréquence observable.
     * Pour allumer la LED, il faut modifier le mode de la broche en Push-Pull sur P1.6, grace à ca on a une tension sup a 2.2V en ignorant la résistance.
     */

    P1MDOUT |= 0x40; // P1.6 en Push-Pull

    /* Configuration spécifique :
       P6 : Sorties Push-Pull (pour les LEDs externes)
       P7 : Entrées (Open-Drain + écriture de '1')
       P4.7 : Sortie Push-Pull (Témoin d'activité)
       P4.0 : Entrée (Lecture configuration)
    */
    P6MDOUT |= 0xFF;  // Port 6 en Push-Pull
    P7MDOUT &= ~0xFF; // Port 7 en Open-Drain (Entrée)
    P7 = 0xFF;        // Ecriture de '1' pour permettre la lecture externe

    P4MDOUT |= 0x80;  // P4.7 en Push-Pull (1000 0000)
    P4MDOUT &= ~0x01; // P4.0 en Open-Drain (Entrée)
    P4 |= 0x01;       // Ecriture de '1' sur P4.0 pour lecture

    while (1) {

        // Si P4.0 est à 1, P6 recopie P7, sinon il recopie l'inverse.
        if ((P4 & 0x01) == 0x01) {
            P6 = P7;
        } else {
            P6 = ~P7;
        }

        // Inversion de P4.7 à chaque cycle (Témoin)
        P4 ^= 0x80;

        // Allume la LED de la carte si on appuie sur le bouton BP (P3.7)
        // Note : On utilise BP (défini en sbit) car BOUTON n'était pas déclaré
        if (BP == 0) {
            LED_VERTE = 1;
        } else {
            LED_VERTE = 0;
        }

        // Temporisation pour éviter un clignotement trop rapide sur P4.7
        {
            unsigned long i;
            for (i = 0; i < 10000; i++);
        }
    }
}