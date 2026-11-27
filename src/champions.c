#include "global.h"
#include "champions.h"
#include "constants/abilities.h"

// Pokémon Emerald Champions: helpers for the hack's own rules.

bool32 IsWeatherOrTerrainSetterAbility(enum Ability ability)
{
    switch (ability)
    {
    case ABILITY_DRIZZLE:
    case ABILITY_DROUGHT:
    case ABILITY_SAND_STREAM:
    case ABILITY_SNOW_WARNING:
    case ABILITY_SAND_SPIT:
    case ABILITY_PRIMORDIAL_SEA:
    case ABILITY_DESOLATE_LAND:
    case ABILITY_DELTA_STREAM:
    case ABILITY_ELECTRIC_SURGE:
    case ABILITY_GRASSY_SURGE:
    case ABILITY_MISTY_SURGE:
    case ABILITY_PSYCHIC_SURGE:
    case ABILITY_SEED_SOWER:
    case ABILITY_ORICHALCUM_PULSE:
    case ABILITY_HADRON_ENGINE:
        return TRUE;
    default:
        return FALSE;
    }
}
