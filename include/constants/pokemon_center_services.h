#ifndef GUARD_CONSTANTS_POKEMON_CENTER_SERVICES_H
#define GUARD_CONSTANTS_POKEMON_CENTER_SERVICES_H

// Pokémon Emerald Champions: the services attendant in every Pokémon Center (as in Run & Bun).
// Script: data/scripts/pokemon_center_services.inc. Specials: src/pokemon_center_services.c.

// Heart Scale prices. The free services (forget a move, change nickname, give a status) have none.
#define PC_SERVICES_PRICE_RELEARN  1 // help a Pokémon remember one level-up move
#define PC_SERVICES_PRICE_IV       1 // raise one IV to 31
#define PC_SERVICES_PRICE_NATURE   3 // change the nature

// Rows of the IV menu, in the summary screen's order.
#define PC_SERVICES_STAT_HP     0
#define PC_SERVICES_STAT_ATK    1
#define PC_SERVICES_STAT_DEF    2
#define PC_SERVICES_STAT_SPATK  3
#define PC_SERVICES_STAT_SPDEF  4
#define PC_SERVICES_STAT_SPEED  5
#define PC_SERVICES_STAT_COUNT  6

// Rows of the status menu.
#define PC_SERVICES_STATUS_BURN       0
#define PC_SERVICES_STATUS_FREEZE     1
#define PC_SERVICES_STATUS_PARALYSIS  2
#define PC_SERVICES_STATUS_POISON     3
#define PC_SERVICES_STATUS_SLEEP      4
#define PC_SERVICES_STATUS_COUNT      5

// What PcServices_CheckStatus returns.
#define PC_SERVICES_STATUS_GIVEN    0 // it can be given
#define PC_SERVICES_STATUS_IMMUNE   1 // its type or ability prevents that status
#define PC_SERVICES_STATUS_FAINTED  2

// The sleep counter a Pokémon gets here: the highest it can hold, as in Run & Bun. It counts down
// each turn the Pokémon tries to move, so it sleeps through 6 turns and wakes on the 7th.
#define PC_SERVICES_SLEEP_COUNTER  7

#endif // GUARD_CONSTANTS_POKEMON_CENTER_SERVICES_H
