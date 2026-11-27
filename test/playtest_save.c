// Pokémon Emerald Champions: checks the playtest setup (data/scripts/playtest.inc) on a new game.
// tools/make_playtest_save.py also runs this test to write the ready-made playtest save: it
// patches gPlaytestSaveDump to TRUE, and the test then saves the game and prints the flash
// contents, which the tool turns into a .sav file.
#include "global.h"
#include "battle_setup.h"
#include "event_data.h"
#include "item.h"
#include "load_save.h"
#include "money.h"
#include "new_game.h"
#include "overworld.h"
#include "playtest.h"
#include "pokemon.h"
#include "save.h"
#include "string_util.h"
#include "gba/flash_internal.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/opponents.h"
#include "test/overworld_script.h"
#include "test/test.h"

// Patched to TRUE by tools/make_playtest_save.py.
const volatile bool8 gPlaytestSaveDump = FALSE;

#define PLAYTEST_DUMP_BYTES_PER_LINE 64

// Prints every flash line that isn't blank (0xFF) as "PLAYTEST_SAVE <sector> <offset> <hex>".
static void DumpFlashForPlaytestSave(void)
{
    static const char sHexDigits[] = "0123456789abcdef";
    u8 bytes[PLAYTEST_DUMP_BYTES_PER_LINE];
    char hex[PLAYTEST_DUMP_BYTES_PER_LINE * 2 + 1];
    u32 sector, offset, i;

    for (sector = 0; sector < SECTORS_COUNT; sector++)
    {
        for (offset = 0; offset < SECTOR_SIZE; offset += PLAYTEST_DUMP_BYTES_PER_LINE)
        {
            ReadFlash(sector, offset, bytes, PLAYTEST_DUMP_BYTES_PER_LINE);
            for (i = 0; i < PLAYTEST_DUMP_BYTES_PER_LINE; i++)
            {
                if (bytes[i] != 0xFF)
                    break;
            }
            if (i == PLAYTEST_DUMP_BYTES_PER_LINE)
                continue;

            for (i = 0; i < PLAYTEST_DUMP_BYTES_PER_LINE; i++)
            {
                hex[i * 2] = sHexDigits[bytes[i] >> 4];
                hex[i * 2 + 1] = sHexDigits[bytes[i] & 0xF];
            }
            hex[PLAYTEST_DUMP_BYTES_PER_LINE * 2] = '\0';
            Test_MgbaPrintf("PLAYTEST_SAVE %d %d %s", sector, offset, hex);
        }
    }
    Test_MgbaPrintf("PLAYTEST_SAVE_END");
}

// A new game, as the main menu's NEW GAME starts one, with the playtest setup applied.
// The game continues in front of PETALBURG CITY's Pokémon Center.
static void SetUpPlaytestGame(void)
{
    const struct MapHeader *mapHeader;

    Sav2_ClearSetDefault();
    StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("TED"));
    gSaveBlock2Ptr->playerGender = MALE;
    NewGameInitData();
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
    FlagSet(FLAG_SYS_CLOCK_SET);

    RUN_OVERWORLD_SCRIPT(
        call Playtest_EventScript_Setup;
    );

    SetLastHealLocationWarp(HEAL_LOCATION_PETALBURG_CITY);
    SetContinueGameWarpToHealLocation(HEAL_LOCATION_PETALBURG_CITY);
    SetContinueGameWarpStatus();
    gSaveBlock1Ptr->location = gSaveBlock1Ptr->continueGameWarp;
    gSaveBlock1Ptr->pos.x = gSaveBlock1Ptr->continueGameWarp.x;
    gSaveBlock1Ptr->pos.y = gSaveBlock1Ptr->continueGameWarp.y;
    mapHeader = Overworld_GetMapHeaderByGroupAndId(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
    gSaveBlock1Ptr->mapLayoutId = mapHeader->mapLayoutId;
    CalculatePlayerPartyCount();
}

TEST("Playtest save: the playtest setup gives badges, HMs, trainers and a team")
{
    u32 i;

    SetUpPlaytestGame();

    // Badges, HMs and getting around.
    EXPECT(FlagGet(FLAG_BADGE01_GET));
    EXPECT(FlagGet(FLAG_BADGE08_GET));
    EXPECT(CheckBagHasItem(ITEM_HM_CUT, 1));
    EXPECT(CheckBagHasItem(ITEM_HM_FLY, 1));
    EXPECT(CheckBagHasItem(ITEM_HM_SURF, 1));
    EXPECT(CheckBagHasItem(ITEM_HM_DIVE, 1));
    EXPECT(CheckBagHasItem(ITEM_MACH_BIKE, 1));
    EXPECT(CheckBagHasItem(ITEM_SUPER_ROD, 1));
    EXPECT(CheckBagHasItem(ITEM_INFINITE_REPEL, 1));
    EXPECT(FlagGet(FLAG_INFINITE_REPEL_ON));
    EXPECT(CheckBagHasItem(ITEM_HEART_SCALE, 99));
    EXPECT(FlagGet(FLAG_VISITED_EVER_GRANDE_CITY));
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), MAX_MONEY);

    // Every trainer counts as beaten.
    EXPECT(HasTrainerBeenFought(TRAINER_GRUNT_PETALBURG_WOODS));
    EXPECT(HasTrainerBeenFought(TRAINER_BRAWLY_1));
    EXPECT(HasTrainerBeenFought(TRAINER_SILAS));
    EXPECT(HasTrainerBeenFought(TRAINERS_COUNT - 1));
    EXPECT(!HasTrainerBeenFought(TRAINER_NONE));

    // The early story is done.
    EXPECT(FlagGet(FLAG_DELIVERED_DEVON_GOODS));
    EXPECT(FlagGet(FLAG_DELIVERED_STEVEN_LETTER));
    EXPECT_EQ(VarGet(VAR_PETALBURG_WOODS_STATE), 1);
    EXPECT_EQ(VarGet(VAR_ROUTE110_STATE), 1);

    // A level 100 team.
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], PARTY_SIZE);
    for (i = 0; i < PARTY_SIZE; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_VENUSAUR);

    // Where the game continues.
    EXPECT(UseContinueGameWarp());
    EXPECT_EQ(gSaveBlock1Ptr->location.mapGroup, MAP_GROUP(MAP_PETALBURG_CITY));
    EXPECT_EQ(gSaveBlock1Ptr->location.mapNum, MAP_NUM(MAP_PETALBURG_CITY));

    // The save loads back the way the game loads it when it boots.
    EXPECT(gFlashMemoryPresent);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    ClearSav1();
    ClearSav2();
    ClearSav3();
    ZeroPlayerPartyMons();
    Save_ResetSaveCounters();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(StringCompare(gSaveBlock2Ptr->playerName, COMPOUND_STRING("TED")), 0);
    EXPECT(FlagGet(FLAG_BADGE08_GET));
    EXPECT(HasTrainerBeenFought(TRAINER_BRAWLY_1));
    EXPECT(CheckBagHasItem(ITEM_HM_WATERFALL, 1));
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], PARTY_SIZE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1], MON_DATA_SPECIES), SPECIES_FERALIGATR);
    EXPECT(UseContinueGameWarp());
    EXPECT_EQ(gSaveBlock1Ptr->location.mapNum, MAP_NUM(MAP_PETALBURG_CITY));

    if (gPlaytestSaveDump)
        DumpFlashForPlaytestSave();
}
