// Pokémon Emerald Champions: tests for the Pokémon Center services specials (src/pokemon_center_services.c).
#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_center_services.h"
#include "test/test.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/pokemon_center_services.h"

// Personality 0 gives a Hardy nature and the first ability slot. CreateMon leaves the stats
// (and HP) at 0, so they're worked out here, as for any Pokémon in the party.
static struct Pokemon *SetUpPartyMon(enum Species species)
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];

    CreateMon(mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(mon);
    gSpecialVar_0x8004 = 0;
    return mon;
}

TEST("PC services: each IV menu row reads its own stat")
{
    static const u8 fields[PC_SERVICES_STAT_COUNT] =
    {
        MON_DATA_HP_IV, MON_DATA_ATK_IV, MON_DATA_DEF_IV, MON_DATA_SPATK_IV, MON_DATA_SPDEF_IV, MON_DATA_SPEED_IV,
    };
    struct Pokemon *mon = SetUpPartyMon(SPECIES_WOBBUFFET);
    u32 i;

    for (i = 0; i < PC_SERVICES_STAT_COUNT; i++)
    {
        u8 iv = 10 + i;
        SetMonData(mon, fields[i], &iv);
    }
    for (i = 0; i < PC_SERVICES_STAT_COUNT; i++)
    {
        gSpecialVar_0x8005 = i;
        EXPECT_EQ(PcServices_GetIV(), 10 + i);
    }
}

TEST("PC services: maxing an IV sets it to 31 and raises the stat")
{
    struct Pokemon *mon = SetUpPartyMon(SPECIES_WOBBUFFET);
    u8 zero = 0;
    u32 before;

    SetMonData(mon, MON_DATA_SPEED_IV, &zero);
    CalculateMonStats(mon);
    before = GetMonData(mon, MON_DATA_SPEED);
    gSpecialVar_0x8005 = PC_SERVICES_STAT_SPEED;
    PcServices_MaxIV();
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPEED_IV), MAX_PER_STAT_IVS);
    EXPECT_GT(GetMonData(mon, MON_DATA_SPEED), before);
}

TEST("PC services: changing the nature works like a Mint")
{
    struct Pokemon *mon = SetUpPartyMon(SPECIES_WOBBUFFET);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    u32 hardyAttack = GetMonData(mon, MON_DATA_ATK);

    gSpecialVar_0x8005 = NATURE_ADAMANT;
    PcServices_SetNature();
    EXPECT_EQ(GetMonData(mon, MON_DATA_PERSONALITY), personality);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HIDDEN_NATURE), NATURE_ADAMANT);
    EXPECT_GT(GetMonData(mon, MON_DATA_ATK), hardyAttack);
    EXPECT_EQ(PcServices_GetNature(), NATURE_ADAMANT);
}

TEST("PC services: types and abilities block statuses")
{
    u32 species = 0, status = 0, expected = 0;

    PARAMETRIZE { species = SPECIES_CHARMANDER; status = PC_SERVICES_STATUS_BURN;      expected = PC_SERVICES_STATUS_IMMUNE; }
    PARAMETRIZE { species = SPECIES_SNORUNT;    status = PC_SERVICES_STATUS_FREEZE;    expected = PC_SERVICES_STATUS_IMMUNE; }
    PARAMETRIZE { species = SPECIES_PIKACHU;    status = PC_SERVICES_STATUS_PARALYSIS; expected = PC_SERVICES_STATUS_IMMUNE; }
    PARAMETRIZE { species = SPECIES_GRIMER;     status = PC_SERVICES_STATUS_POISON;    expected = PC_SERVICES_STATUS_IMMUNE; }
    PARAMETRIZE { species = SPECIES_MAGNEMITE;  status = PC_SERVICES_STATUS_POISON;    expected = PC_SERVICES_STATUS_IMMUNE; }
    PARAMETRIZE { species = SPECIES_DROWZEE;    status = PC_SERVICES_STATUS_SLEEP;     expected = PC_SERVICES_STATUS_IMMUNE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET;  status = PC_SERVICES_STATUS_BURN;      expected = PC_SERVICES_STATUS_GIVEN; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET;  status = PC_SERVICES_STATUS_FREEZE;    expected = PC_SERVICES_STATUS_GIVEN; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET;  status = PC_SERVICES_STATUS_PARALYSIS; expected = PC_SERVICES_STATUS_GIVEN; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET;  status = PC_SERVICES_STATUS_POISON;    expected = PC_SERVICES_STATUS_GIVEN; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET;  status = PC_SERVICES_STATUS_SLEEP;     expected = PC_SERVICES_STATUS_GIVEN; }
    ASSUME(GetSpeciesAbility(SPECIES_DROWZEE, 0) == ABILITY_INSOMNIA);
    SetUpPartyMon(species);
    gSpecialVar_0x8005 = status;
    EXPECT_EQ(PcServices_CheckStatus(), expected);
}

TEST("PC services: a fainted Pokémon can't be given a status")
{
    struct Pokemon *mon = SetUpPartyMon(SPECIES_WOBBUFFET);
    u16 hp = 0;

    SetMonData(mon, MON_DATA_HP, &hp);
    gSpecialVar_0x8005 = PC_SERVICES_STATUS_BURN;
    EXPECT_EQ(PcServices_CheckStatus(), PC_SERVICES_STATUS_FAINTED);
}

TEST("PC services: a given status replaces the old one, and sleep gets the longest counter")
{
    u32 status = 0, expected = 0;
    struct Pokemon *mon;
    u32 oldStatus = STATUS1_PARALYSIS;

    PARAMETRIZE { status = PC_SERVICES_STATUS_BURN;      expected = STATUS1_BURN; }
    PARAMETRIZE { status = PC_SERVICES_STATUS_FREEZE;    expected = B_USE_FROSTBITE ? STATUS1_FROSTBITE : STATUS1_FREEZE; }
    PARAMETRIZE { status = PC_SERVICES_STATUS_POISON;    expected = STATUS1_POISON; }
    PARAMETRIZE { status = PC_SERVICES_STATUS_SLEEP;     expected = STATUS1_SLEEP_TURN(PC_SERVICES_SLEEP_COUNTER); }
    mon = SetUpPartyMon(SPECIES_WOBBUFFET);
    SetMonData(mon, MON_DATA_STATUS, &oldStatus);
    gSpecialVar_0x8005 = status;
    PcServices_GiveStatus();
    EXPECT_EQ(GetMonData(mon, MON_DATA_STATUS), expected);
}
