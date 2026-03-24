# TP5 - Mise en œuvre d'un système à microprocesseur : UART

**CPE Lyon - 3IRC - Séance 5**

## Contexte

TP sur la mise en œuvre de l'UART sur microcontrôleur **8051 (C8051F020)**, développé sous **Microvision (Keil)**.

## Objectifs de la séance

- Configurer et utiliser une UART en transmission (TX) sur liaison série asynchrone
- Configurer et utiliser une UART en réception (RX) sur liaison série asynchrone

## Structure des fichiers

| Fichier | Rôle | Modifiable ? |
|---|---|---|
| `Main_TP5.C` | Programme principal — **c'est ici qu'on travaille** | Oui |
| `Lib_Base.c` / `Lib_Base.h` | Fonctions de configuration du microcontrôleur | **NON** |
| `TP1_Lib_ASM.ASM` / `TP1_Lib_ASM.h` | Fonctions assembleur appelables depuis C | **NON** |
| `Projet_TP5.uvproj` | Fichier de configuration Microvision | Non |

> `Lib_Base.c` et `TP1_Lib_ASM.asm` **ne doivent pas être modifiés**.

## Rendu

- Fichier à rendre sur e-campus : `Main_TP5.C`
- Nommage obligatoire : **`Main_TP5_Nom1_Nom2.C`**

## Activités

### Activité 1 — Décodage et encodage RS232
- Identifier si un signal est RS232 ou CMOS/TTL selon les niveaux de tension
  - CMOS/TTL : niveaux entre GND et VCC
  - RS232 : niveaux entre -5V/-15V et +5V/+15V
- Analyser un oscillogramme pour déterminer la valeur transmise et la vitesse en Baud
- Dessiner le signal du caractère ASCII `'A'` en RS232 8 bits à 115200 Baud

### Activité 2 — Analyse de la datasheet STM32L010
- Consulter `Datasheet__stm32l010r8.pdf` pour déterminer le nombre d'UARTs disponibles sur le circuit

### Activité 3 — Configuration basique de l'UART0
- Le code de base fait clignoter la LED verte via une boucle infinie (Timer chien de garde + SYSCLK + matrice d'interconnexion)
- Utiliser `Config_Timer2` (déjà fournie) pour faire fonctionner le Timer 2 en **Baud rate generator**
- Coder `void Config_UART0_mode1(void)` : mode 1 = transmission asynchrone 8 bits, 1 start bit, 1 stop bit
- Vitesse : **115200 Baud** (gérée par Timer 2)
- Tester l'envoi de caractères en boucle sur **Putty** et vérifier le signal TX à l'oscilloscope

### Activité 4 — Changer la source d'horloge de l'UART0
- Utiliser le **Timer 1** comme source d'horloge à la place du Timer 2
- Nouvelle vitesse : **19200 Baud**
- Créer `CFG_clock_UART_VerT1`
- Modifier `Config_UART0_mode1` en conséquence
- Vérifier que la transmission fonctionne toujours

### Activité 5 — Fonctions `putchar` et `_getkey`

**5.1 — Echo de caractère**
- Coder `putchar` (envoi d'un caractère via UART) et `_getkey` (réception d'un caractère)
- Modifier `main` pour réaliser un **écho** : chaque caractère reçu est renvoyé

**5.2 — `printf` et `gets` avec bouton poussoir**
- Utiliser `printf` et `gets` (basés sur `putchar` / `_getkey`)
- Appui sur bouton **P3.7** (INT7) → envoyer `"Bouton appuyé.\n"` vers le PC
- Le PC envoie une valeur entre 1 et 10000 → modifier la période du signal **SIG_OUT** (broche **P3.5**)
- La période est gérée via l'interruption périodique du **Timer 3**

## Points clés techniques

- UART0 du 8051 fonctionne en **mode 1** : 8 bits de données, 1 start, 1 stop, pas de parité
- Le **Timer 2** ou **Timer 1** sert de générateur de baud rate
- `printf` et `gets` de la bibliothèque standard C8051 utilisent `putchar` et `_getkey` comme primitives
- Le bouton P3.7 déclenche l'interruption **INT7**
- Le signal SIG_OUT est sur **P3.5**, sa période est ajustée par l'interruption Timer 3
