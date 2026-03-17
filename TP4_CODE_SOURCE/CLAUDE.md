# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Toolchain

This project targets the **Silicon Laboratories C8051F020** microcontroller and is built with **Keil uVision** (MCS-51 toolchain). There is no command-line build — all compilation, linking, and flashing is done through the Keil uVision IDE using `Projet_TP4.uvproj`.

Two build targets are defined in the project:
- **Simulation** — runs in the Keil software simulator (S8051.DLL / DCYG.DLL with `-pCYGF020`)
- **Exécution 8051FX20-TB** — flashes to the physical board via `BIN\SiC8051F.dll`

## Architecture

The firmware is structured as a layered library used across TP (lab) sessions:

### Initialization layer (`Lib_Base.c` / `Lib_Base.h`)
Called once at startup via `Init_Device()`, which sequentially:
1. `Reset_Sources_Init()` — disables the watchdog (writes `0xDE` then `0xAD` to `WDTCN`)
2. `Port_IO_Init()` — configures the crossbar (XBR0/XBR1/XBR2) and I/O direction registers (PxMDOUT)
3. `Oscillator_Init()` — switches to the 22.1184 MHz external crystal (OSCXCN/OSCICN), with a busy-wait for stabilization

### Assembly timing library (`TP1_Lib_ASM.ASM` / `TP1_Lib_ASM.h`)
Provides software delay routines calibrated for SYSCLK = 22.1184 MHz:
- `tempo(unsigned int)` — delay in microseconds (min 2 µs), passed via R6 (MSB) / R7 (LSB)
- `tempo_20US()` / `tempo_100US()` — fixed-duration convenience wrappers
- `tempo_sup(unit, value)` — parameterized delay with unit `'U'` (µs), `'M'` (ms), `'S'` (s); returns 0 on success or an error code
- `Config_Timer3_BT()` — configures Timer 3 in auto-reload mode; reload value passed in R6/R7

### Application (`Main_TP4_Bastos_Guillemain.C`)
TP4 subject: **interrupt-driven LED blinking with speed control via external interrupt INT7**.

Key hardware mappings:
- `LED` → P1.6 (green LED, active high)
- `BP` → P3.7 (push button, active low, open-drain)
- P4.0–P4.3: outputs (push-pull via P74OUT bit 0); P4.4–P4.7: inputs (open-drain)
- P4.0 (B23) and P4.2 (C22) used as hardware debug flags toggled inside ISRs

Interrupt configuration:
- **Timer 3 ISR** (vector 14, high priority) — fires every 10 ms (TMR3RL = 0xB800, TMR3CN = 0x04); increments a tick counter and toggles the LED when the counter reaches `Value_tempo`
- **INT7 ISR** (vector 19, low priority) — triggered on falling edge of P3.7 (BP press); toggles `Value_tempo` between `Slow` (100 ticks = 1 s) and `Fast` (10 ticks = 100 ms); flag cleared by writing `P3IF &= ~(1<<7)`

Global interrupt enable (`EA = 1`) is set after all peripheral configuration in `main()`.

## Activite 1 - Analyse du code fourni (Timer 3)

---

### Q1 - Quel timer est utilise ?

**Le Timer 3** (`TMR3`).

Comment le voir dans le code :
- La fonction `Config_Timer3()` ecrit dans les registres `TMR3CN` et `TMR3RL` — ces noms sont propres au Timer 3.
- La fonction d'interruption est declaree `interrupt 14` — chaque peripherique du 8051F020 a un numero de vecteur fixe. Le vecteur 14 correspond au Timer 3.

---

### Q2 - Quel est le mode de fonctionnement ?

**Mode auto-reload 16 bits.**

#### Principe du mode auto-reload

Un timer est un compteur qui s'incremente a chaque coup d'horloge. En mode auto-reload :
1. Le timer part d'une valeur de depart (la valeur de "reload" stockee dans `TMR3RL`)
2. Il compte vers le haut : +1 a chaque tick
3. Quand il depasse `0xFFFF` (valeur max sur 16 bits), il **deborde** (overflow)
4. A ce moment : il se **recharge automatiquement** avec la valeur de reload, ET il **declenche une interruption**
5. Le cycle recommence indefiniment

```
TMR3RL = 0xB800
           |
           v
[0xB800] --> [0xB801] --> ... --> [0xFFFF] --> OVERFLOW!
    ^                                              |
    |_____________ rechargement auto ______________|
                                         + interruption declenchee
```

Pourquoi "auto" ? Parce que sans auto-reload, le timer reprendrait de 0x0000 apres le debordement, ce qui donnerait une periode differente et non maitrisee. L'auto-reload permet de choisir precisement la periode.

#### Comment lire TMR3CN = 0x04

`0x04` en binaire = `0000 0100`. Chaque bit a un role :

```
Bit :   7     6     5     4     3     2     1     0
Nom :  TF3    -     -     -     -    TR3   T3M  T3XCLK
Val :   0     0     0     0     0     1     0     0
```

- **Bit 7 (TF3)** = 0 : flag d'overflow (mis a 1 par le hardware quand le timer deborde, doit etre remis a 0 par le programme dans l'ISR)
- **Bit 2 (TR3)** = 1 : Timer Run bit — met le timer en marche. Si TR3=0 le timer est arrete.
- **Bit 1 (T3M)** = 0 : selectionne l'horloge source. 0 = SYSCLK/12, 1 = SYSCLK/1
- **Bit 0 (T3XCLK)** = 0 : source externe desactivee (on utilise l'horloge interne)

---

### Q3 - Quelle est la frequence de l'horloge du timer ?

#### Qu'est-ce qu'un tick ?

Un **tick** = une incrementation du compteur du timer. Le timer est un registre 16 bits qui avance de +1 a chaque coup d'horloge qu'il recoit.
`T_tick` = la duree entre deux incrementations successives = l'unite de temps elementaire du timer.

Analogie : c'est comme les secondes d'une montre. Si la montre fait "tic" toutes les secondes, apres 60 tics il s'est ecoule 60 s. Ici, apres N ticks il s'est ecoule N x T_tick.

```
Horloge du timer :
  __|--|__|--|__|--|__|--|__
     tick tick tick tick
Compteur :
  [0xB800][0xB801][0xB802][0xB803] ...
```

#### D'ou vient le /12 ?

C'est un **heritage de l'architecture 8051 originale d'Intel (1980)**. A cette epoque, le processeur avait besoin de **12 coups d'horloge** pour executer une instruction complete (fetch -> decodage -> execution). Ce groupe de 12 cycles s'appelait un "cycle machine".

Les timers internes etaient synchronises sur ce cycle machine : le timer s'incrementait **une fois par cycle machine**, donc une fois toutes les 12 periodes d'horloge.

Le C8051F020 est un derive 8051 qui garde cette convention par defaut (`T3M = 0`).
Silicon Labs a ajoute une option plus rapide : quand `T3M = 1`, le timer s'incremente a chaque coup d'horloge (SYSCLK/1), soit 12x plus vite.

#### Calcul

Avec `T3M = 0` (notre cas) :
```
F_timer = SYSCLK / 12 = 22 118 400 / 12 = 1 843 200 Hz
T_tick  = 1 / F_timer = 1 / 1 843 200 = 0,5425 µs
```

Le compteur s'incremente de 1 toutes les **0,5425 microsecondes**.

---

### Q4 - Quelle est la recurrence des interruptions ? Comment est-elle obtenue ?

#### Calcul du nombre de ticks entre deux interruptions

Le timer compte de `TMR3RL` jusqu'a `0xFFFF` inclus, puis deborde.
```
TMR3RL = 0xB800 = 47 104 (en decimal)
0xFFFF = 65 535 (en decimal)

Nombre de ticks = 65 535 - 47 104 + 1 = 18 432
```
Le "+1" est important : le tick a la valeur `0xFFFF` est compte, et le debordement a lieu apres, donc on compte bien 18 432 pas.

#### Calcul de la periode d'interruption

```
Periode_ISR = Nombre_de_ticks x T_tick
            = 18 432 x 0,5425 µs
            = 10 000 µs
            = 10 ms
```
L'ISR est donc declenchee **100 fois par seconde** (toutes les 10 ms).

#### Comment la LED clignote

L'ISR ne toggle pas la LED directement a chaque appel. Elle utilise un compteur logiciel `Tick_counter` :

```c
Tick_counter++;
if (Tick_counter >= Value_tempo) {
    Tick_counter = 0;
    LED = ~LED;   // toggle
}
```

`Value_tempo` est exprime en nombre d'interruptions (= nombre de tranches de 10 ms) :
```
Value_tempo = Fast = 10  --> toggle toutes les 10 x 10ms = 100ms
                             periode LED = 2 x 100ms = 200ms (5 Hz)

Value_tempo = Slow = 100 --> toggle toutes les 100 x 10ms = 1s
                             periode LED = 2 x 1s = 2s (0,5 Hz)
```
La periode de la LED est le double du temps entre deux toggles car il faut un toggle pour passer de 0 a 1, et un autre pour revenir de 1 a 0.

---

### Q5 - Diviser par 2 la recurrence (periode ISR : 10ms -> 20ms)

"Diviser la recurrence par 2" signifie que l'interruption se declenche 2 fois moins souvent, donc la **periode double** : 10 ms x 2 = **20 ms**.

#### Demarche de calcul

**Etape 1** : Calculer le nombre de ticks necessaires pour 20 ms
```
Nb_ticks = Periode_souhaitee / T_tick
         = 20 ms / 0,5425 µs
         = 0,020 / 0,0000005425
         = 36 864
```
Ou equivalent :
```
Nb_ticks = Periode_souhaitee x F_timer = 0,020 x 1 843 200 = 36 864
```

**Etape 2** : Calculer la nouvelle valeur de reload
```
TMR3RL = 0x10000 - Nb_ticks
       = 65 536 - 36 864
       = 28 672
       = 0x7000
```
Pourquoi `0x10000` (= 65 536) et pas `0xFFFF` (= 65 535) ?
Parce que le timer compte 65 536 valeurs distinctes : de 0x0000 a 0xFFFF inclus. Partir de `TMR3RL` et arriver a 0xFFFF fait exactement `0x10000 - TMR3RL` ticks.

**Modification dans le code** : remplacer `TMR3RL = 0xB800` par `TMR3RL = 0x7000`.

---

### Q6 - Multiplier la recurrence par 4 (periode ISR : 10ms -> 2,5ms)

"Multiplier la recurrence par 4" = l'interruption se declenche 4 fois plus souvent = periode divisee par 4 : 10 ms / 4 = **2,5 ms**.

#### Demarche de calcul

**Etape 1** : Nombre de ticks pour 2,5 ms
```
Nb_ticks = 0,0025 x 1 843 200 = 4 608 = 0x1200
```

**Etape 2** : Valeur de reload
```
TMR3RL = 0x10000 - 4 608 = 60 928 = 0xEE00
```

**Est-ce possible ?** Oui. La contrainte est que `TMR3RL` doit etre entre 0x0000 et 0xFFFF.
`0xEE00` = 60 928 est bien dans cet intervalle. De plus, 4 608 ticks est bien superieur a 1, donc le timer a le temps de compter.

La periode minimale theorique avec SYSCLK/12 est 1 tick = 0,5425 µs, et la maximale est 65 536 ticks = 65 536 x 0,5425 µs ≈ 35,6 ms. Toute periode dans cet intervalle est realisable.

---

### Formule generale a retenir

Pour calculer `TMR3RL` a partir d'une periode souhaitee :

```
TMR3RL = 0x10000 - (Periode_en_secondes x SYSCLK / 12)
```

Pour verifier une valeur de `TMR3RL` existante :
```
Periode = (0x10000 - TMR3RL) / (SYSCLK / 12)
        = (0x10000 - TMR3RL) x 12 / SYSCLK
```

---

## Key SFR notes

- `P74OUT`: non-standard 8051 SFR controlling push-pull/open-drain for ports 4–7; bit 0 = P4.0–P4.3 direction, bit 1 = P4.4–P4.7 direction
- `EIE2` / `EIP2`: extended interrupt enable/priority registers (Silicon Labs extension); bit 0 = Timer3, bit 5 = INT7
- `TMR3CN` bit 7: Timer 3 overflow flag — must be cleared manually in the ISR (`TMR3CN &= ~(1<<7)`)
- `P3IF` bit 7: INT7 pending flag; bit 3: INT7 edge select (0 = falling edge)
