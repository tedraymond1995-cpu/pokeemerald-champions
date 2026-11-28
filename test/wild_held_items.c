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

TEST("Wild held items: a wild Zigzagoon always holds a Rare Candy")
{
    u32 i;

    gBattleTypeFlags = 0;
    for (i = 0; i < 200; i++)
    {
        CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_ZIGZAGOON, 3, Random32(), OTID_STRUCT_PLAYER_ID);
        SetWildMonHeldItem();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_RARE_CANDY);
    }
}

TEST("Wild held items: only the Hoenn Zigzagoon holds a Rare Candy, not its Galarian form or Linoone")
{
    u32 i, j;
    static const u16 species[] = { SPECIES_ZIGZAGOON_GALAR, SPECIES_LINOONE, SPECIES_LINOONE_GALAR, SPECIES_OBSTAGOON };

    gBattleTypeFlags = 0;
    for (j = 0; j < ARRAY_COUNT(species); j++)
    {
        for (i = 0; i < 100; i++)
        {
            CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], species[j], 20, Random32(), OTID_STRUCT_PLAYER_ID);
            SetWildMonHeldItem();
            EXPECT_NE(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_RARE_CANDY);
        }
    }
}
