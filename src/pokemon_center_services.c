// Pokémon Emerald Champions: specials for the services attendant in every Pokémon Center, modelled
// on Run & Bun's. The script is data/scripts/pokemon_center_services.inc; prices and menu rows are
// in include/constants/pokemon_center_services.h.
//
// VAR_0x8004 holds the chosen Pokémon: a party slot, or PC_MON_CHOSEN for one picked from a box
// (GetSelectedBoxMonFromPcOrParty finds it either way).
#include "global.h"
#include "config_changes.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_center_services.h"
#include "string_util.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/party_menu.h"
#include "constants/pokemon_center_services.h"

static const u8 sIVFields[PC_SERVICES_STAT_COUNT] =
{
    [PC_SERVICES_STAT_HP]    = MON_DATA_HP_IV,
    [PC_SERVICES_STAT_ATK]   = MON_DATA_ATK_IV,
    [PC_SERVICES_STAT_DEF]   = MON_DATA_DEF_IV,
    [PC_SERVICES_STAT_SPATK] = MON_DATA_SPATK_IV,
    [PC_SERVICES_STAT_SPDEF] = MON_DATA_SPDEF_IV,
    [PC_SERVICES_STAT_SPEED] = MON_DATA_SPEED_IV,
};

// A party Pokémon keeps its stats, so they're worked out again; a box Pokémon's stats are
// worked out when it leaves the box.
static void RecalcStatsIfInParty(void)
{
    if (gSpecialVar_0x8004 < PARTY_SIZE)
        CalculateMonStats(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004]);
}

// VAR_0x8005: a PC_SERVICES_STAT_* row. Returns the chosen Pokémon's IV for that stat.
u16 PcServices_GetIV(void)
{
    return GetBoxMonData(GetSelectedBoxMonFromPcOrParty(), sIVFields[gSpecialVar_0x8005], NULL);
}

// VAR_0x8005: a PC_SERVICES_STAT_* row. Raises that IV to 31.
void PcServices_MaxIV(void)
{
    u8 iv = MAX_PER_STAT_IVS;

    SetBoxMonData(GetSelectedBoxMonFromPcOrParty(), sIVFields[gSpecialVar_0x8005], &iv);
    RecalcStatsIfInParty();
}

// Returns the chosen Pokémon's nature and buffers its name into STR_VAR_2.
u16 PcServices_GetNature(void)
{
    u32 nature = GetBoxMonData(GetSelectedBoxMonFromPcOrParty(), MON_DATA_HIDDEN_NATURE, NULL);

    StringCopy(gStringVar2, gNaturesInfo[nature].name);
    return nature;
}

// VAR_0x8005: a nature. Buffers its name into STR_VAR_3.
void PcServices_BufferNatureName(void)
{
    StringCopy(gStringVar3, gNaturesInfo[gSpecialVar_0x8005].name);
}

// VAR_0x8005: the new nature. Works like a Mint: the personality stays the same, and with it the
// gender, ability and shininess.
void PcServices_SetNature(void)
{
    u8 nature = gSpecialVar_0x8005;

    SetBoxMonData(GetSelectedBoxMonFromPcOrParty(), MON_DATA_HIDDEN_NATURE, &nature);
    RecalcStatsIfInParty();
}

static bool32 SpeciesHasType(enum Species species, enum Type type)
{
    return GetSpeciesType(species, 0) == type || GetSpeciesType(species, 1) == type;
}

// The type and ability immunities Run & Bun's attendant checks, plus newer abilities that prevent
// these statuses.
static bool32 IsImmuneToStatus(struct Pokemon *mon, u32 status)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Ability ability = GetMonAbility(mon);

    if (ability == ABILITY_COMATOSE || ability == ABILITY_PURIFYING_SALT || ability == ABILITY_SHIELDS_DOWN)
        return TRUE;

    switch (status)
    {
    case PC_SERVICES_STATUS_BURN:
        return SpeciesHasType(species, TYPE_FIRE)
            || ability == ABILITY_WATER_VEIL || ability == ABILITY_WATER_BUBBLE || ability == ABILITY_THERMAL_EXCHANGE;
    case PC_SERVICES_STATUS_FREEZE:
        return SpeciesHasType(species, TYPE_ICE) || ability == ABILITY_MAGMA_ARMOR;
    case PC_SERVICES_STATUS_PARALYSIS:
        return (GetConfig(B_PARALYZE_ELECTRIC) >= GEN_6 && SpeciesHasType(species, TYPE_ELECTRIC))
            || ability == ABILITY_LIMBER;
    case PC_SERVICES_STATUS_POISON:
        return SpeciesHasType(species, TYPE_POISON) || SpeciesHasType(species, TYPE_STEEL)
            || ability == ABILITY_IMMUNITY || ability == ABILITY_PASTEL_VEIL;
    case PC_SERVICES_STATUS_SLEEP:
        return ability == ABILITY_INSOMNIA || ability == ABILITY_VITAL_SPIRIT || ability == ABILITY_SWEET_VEIL;
    }
    return FALSE;
}

// VAR_0x8004: a party slot; VAR_0x8005: a PC_SERVICES_STATUS_* row. Returns
// PC_SERVICES_STATUS_GIVEN if the Pokémon can be given that status, or _IMMUNE or _FAINTED.
u16 PcServices_CheckStatus(void)
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004];

    if (GetMonData(mon, MON_DATA_HP) == 0)
        return PC_SERVICES_STATUS_FAINTED;
    if (IsImmuneToStatus(mon, gSpecialVar_0x8005))
        return PC_SERVICES_STATUS_IMMUNE;
    return PC_SERVICES_STATUS_GIVEN;
}

// VAR_0x8004: a party slot; VAR_0x8005: a PC_SERVICES_STATUS_* row (checked with
// PcServices_CheckStatus first). Gives the Pokémon that status in place of any status it had.
void PcServices_GiveStatus(void)
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004];
    u32 status;

    switch (gSpecialVar_0x8005)
    {
    case PC_SERVICES_STATUS_BURN:
        status = STATUS1_BURN;
        break;
    case PC_SERVICES_STATUS_FREEZE:
        status = B_USE_FROSTBITE ? STATUS1_FROSTBITE : STATUS1_FREEZE;
        break;
    case PC_SERVICES_STATUS_PARALYSIS:
        status = STATUS1_PARALYSIS;
        break;
    case PC_SERVICES_STATUS_POISON:
        status = STATUS1_POISON;
        break;
    default:
        status = STATUS1_SLEEP_TURN(PC_SERVICES_SLEEP_COUNTER);
        break;
    }
    SetMonData(mon, MON_DATA_STATUS, &status);
}
