// Pokémon Emerald Champions: items held by wild Pokémon.
#include "global.h"
#include "test/test.h"
#include "battle.h"
#include "pokemon.h"
#include "constants/items.h"
#include "constants/species.h"

TEST("Wild held items: a wild Luvdisc always holds a Heart Scale")
{
    u32 i;

    gBattleTypeFlags = 0;
    for (i = 0; i < 200; i++)
    {
        CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_LUVDISC, 4, Random32(), OTID_STRUCT_PLAYER_ID);
        SetWildMonHeldItem();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_HEART_SCALE);
    }
}
