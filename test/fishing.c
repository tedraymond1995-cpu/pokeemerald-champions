// Pokémon Emerald Champions: the Super Rod is the only rod, and every area has one
// fishing table that uses all the fishing slots.
#include "global.h"
#include "test/test.h"
#include "battle.h"
#include "pokemon.h"
#include "wild_encounter.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/rtc.h"
#include "constants/species.h"
#include "constants/wild_encounter.h"

static const u8 sExpectedSlotOdds[NUM_FISHING_MONS_ENCOUNTER_SLOTS] = {20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1};

static const struct WildPokemonInfo *GetFishingTable(u32 mapGroup, u32 mapNum)
{
    u32 i;

    for (i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++)
    {
        if (gWildMonHeaders[i].mapGroup == mapGroup && gWildMonHeaders[i].mapNum == mapNum)
            return gWildMonHeaders[i].encounterTypes[TIME_OF_DAY_DEFAULT].fishingMonsInfo;
    }
    return NULL;
}

static u32 GetFishingOdds(const struct WildPokemonInfo *info, enum Species species)
{
    u32 i, odds = 0;

    for (i = 0; i < NUM_FISHING_MONS_ENCOUNTER_SLOTS; i++)
    {
        if (info->wildPokemon[i].species == species)
            odds += sExpectedSlotOdds[i];
    }
    return odds;
}

TEST("Fishing: the slots have the same odds as land encounters (20/20/10/10/10/10/5/5/4/4/1/1)")
{
    u32 roll, i;
    u8 counts[NUM_FISHING_MONS_ENCOUNTER_SLOTS] = {0};

    for (roll = 0; roll < 100; roll++)
    {
        u32 slot = GetFishingSlotFromRoll(roll);
        ASSUME(slot < NUM_FISHING_MONS_ENCOUNTER_SLOTS);
        counts[slot]++;
    }
    for (i = 0; i < NUM_FISHING_MONS_ENCOUNTER_SLOTS; i++)
        EXPECT_EQ(counts[i], sExpectedSlotOdds[i]);
}

TEST("Fishing: Littleroot's pond has its locked table")
{
    u32 i;
    const struct WildPokemonInfo *info = GetFishingTable(MAP_GROUP(MAP_LITTLEROOT_TOWN), MAP_NUM(MAP_LITTLEROOT_TOWN));

    ASSUME(info != NULL);
    EXPECT_EQ(GetFishingOdds(info, SPECIES_LOTAD), 20);
    EXPECT_EQ(GetFishingOdds(info, SPECIES_FINNEON), 20);
    EXPECT_EQ(GetFishingOdds(info, SPECIES_TYMPOLE), 20);
    EXPECT_EQ(GetFishingOdds(info, SPECIES_PSYDUCK), 20);
    EXPECT_EQ(GetFishingOdds(info, SPECIES_LUVDISC), 10);
    EXPECT_EQ(GetFishingOdds(info, SPECIES_CHINCHOU), 10);
    for (i = 0; i < NUM_FISHING_MONS_ENCOUNTER_SLOTS; i++)
    {
        EXPECT_EQ(info->wildPokemon[i].minLevel, 4);
        EXPECT_EQ(info->wildPokemon[i].maxLevel, 4);
    }
}

// The old Super Rod slots were 40/40/15/4/1. Route 119's Super Rod table is Carvanha only in
// Emerald, and Route 118's is Sharpedo 40, Carvanha 60 (the last four slots); both must survive
// the change of slot layout.
TEST("Fishing: tables that are not redesigned yet keep their Super Rod odds")
{
    const struct WildPokemonInfo *route118 = GetFishingTable(MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118));
    const struct WildPokemonInfo *route119 = GetFishingTable(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119));

    ASSUME(route118 != NULL);
    ASSUME(route119 != NULL);
    EXPECT_EQ(GetFishingOdds(route119, SPECIES_CARVANHA), 100);
    EXPECT_EQ(GetFishingOdds(route118, SPECIES_SHARPEDO), 40);
    EXPECT_EQ(GetFishingOdds(route118, SPECIES_CARVANHA), 60);
}

TEST("Fishing: every rod fishes the same table at Littleroot's pond")
{
    static const u16 sPond[] = {SPECIES_LOTAD, SPECIES_FINNEON, SPECIES_TYMPOLE, SPECIES_PSYDUCK, SPECIES_LUVDISC, SPECIES_CHINCHOU};
    u32 i, j, rod = 0;
    u16 seen[ARRAY_COUNT(sPond)] = {0};
    const struct WildPokemonInfo *info = GetFishingTable(MAP_GROUP(MAP_LITTLEROOT_TOWN), MAP_NUM(MAP_LITTLEROOT_TOWN));

    PARAMETRIZE { rod = SUPER_ROD; }
    PARAMETRIZE { rod = GOOD_ROD; }
    PARAMETRIZE { rod = OLD_ROD; }

    ASSUME(info != NULL);
    for (i = 0; i < 400; i++)
    {
        u32 species = GenerateFishingWildMon(info, rod);

        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), species);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 4);
        for (j = 0; j < ARRAY_COUNT(sPond); j++)
        {
            if (species == sPond[j])
            {
                seen[j]++;
                break;
            }
        }
        EXPECT_LT(j, ARRAY_COUNT(sPond));
    }
    // 400 casts miss a 10% slot about once in 10^18 tries.
    for (j = 0; j < ARRAY_COUNT(sPond); j++)
        EXPECT_GT(seen[j], 0);
}
