#ifndef GUARD_CHAMPIONS_H
#define GUARD_CHAMPIONS_H

// Pokémon Emerald Champions: helpers for the hack's own rules.

// Weather and terrain setters are trainer-only in this hack. Wild Pokémon never
// roll them (they sit in the hidden ability slot), and the Ability Patch refuses them.
bool32 IsWeatherOrTerrainSetterAbility(enum Ability ability);

#endif // GUARD_CHAMPIONS_H
