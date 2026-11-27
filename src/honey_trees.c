// Pokémon Emerald Champions: honey trees.
// Each honey tree is a one-time wild encounter that counts as its own area. The tree's script
// (data/scripts/honey_trees.inc and the tree's map) asks, sets the tree's flag, puts the tree's
// table in VAR_0x8008 and calls StartHoneyTreeBattle. The Pokémon is picked from that table by
// weight, and its level is the current level cap minus HONEY_TREE_LEVELS_BELOW_CAP, so a tree
// shaken later in the game gives a stronger Pokémon.
#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "event_data.h"
#include "honey_trees.h"
#include "random.h"
#include "script_pokemon_util.h"
#include "constants/honey_trees.h"
#include "constants/items.h"
#include "constants/species.h"

struct HoneyTreeSlot
{
    u16 species;
    u8 weight;
};

struct HoneyTreeTable
{
    const struct HoneyTreeSlot *slots;
    u8 count;
};

// The shared table every tree uses unless its area gets its own copy.
// The weights add up to 100, so each one is the chance in percent.
static const struct HoneyTreeSlot sHoneyTreeSlots_Shared[] =
{
    { SPECIES_COMBEE,    20 },
    { SPECIES_BURMY,     20 },
    { SPECIES_CHERUBI,   15 },
    { SPECIES_AIPOM,     15 },
    { SPECIES_WURMPLE,   15 },
    { SPECIES_HERACROSS,  8 },
    { SPECIES_MUNCHLAX,   7 },
};

static const struct HoneyTreeTable sHoneyTreeTables[HONEY_TREE_TABLE_COUNT] =
{
    [HONEY_TREE_TABLE_SHARED] = { sHoneyTreeSlots_Shared, ARRAY_COUNT(sHoneyTreeSlots_Shared) },
};

static u16 ChooseHoneyTreeSpecies(const struct HoneyTreeTable *table)
{
    u32 i, roll, total = 0;

    for (i = 0; i < table->count; i++)
        total += table->slots[i].weight;

    roll = Random() % total;
    for (i = 0; i < table->count - 1; i++)
    {
        if (roll < table->slots[i].weight)
            break;
        roll -= table->slots[i].weight;
    }
    return table->slots[i].species;
}

static u8 GetHoneyTreeLevel(void)
{
    u32 levelCap = GetCurrentLevelCap();

    if (levelCap > HONEY_TREE_LEVELS_BELOW_CAP + 1)
        return levelCap - HONEY_TREE_LEVELS_BELOW_CAP;
    return 2;
}

void StartHoneyTreeBattle(void)
{
    u32 tableId = gSpecialVar_0x8008;

    if (tableId >= HONEY_TREE_TABLE_COUNT)
        tableId = HONEY_TREE_TABLE_SHARED;

    CreateScriptedWildMon(ChooseHoneyTreeSpecies(&sHoneyTreeTables[tableId]), GetHoneyTreeLevel(), ITEM_NONE);
    BattleSetup_StartScriptedWildBattle();
}
