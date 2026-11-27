// Pokémon Emerald Champions: HMs work from the bag (OW_HM_FIELD_MOVES_FROM_BAG), and HM moves
// can be forgotten (P_CAN_FORGET_HIDDEN_MOVE).
#include "global.h"
#include "event_data.h"
#include "field_move.h"
#include "field_player_avatar.h"
#include "item.h"
#include "pokemon.h"
#include "script_pokemon_util.h"
#include "constants/field_move.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "test/overworld_script.h"
#include "test/test.h"

// Scripts in tests can't use enums, so checkfieldmove takes FIELD_MOVE_CUT as a number.
STATIC_ASSERT(FIELD_MOVE_CUT == 0, FieldMoveCutIsZero);

// Magikarp can't learn any HM, Squirtle can learn Surf but not Cut, Charmander can learn Cut.
static void SetUpParty(void)
{
    ZeroPlayerPartyMons();
    ScriptGiveMon(SPECIES_MAGIKARP, 20, ITEM_NONE);
    ScriptGiveMon(SPECIES_SQUIRTLE, 20, ITEM_NONE);
    ScriptGiveMon(SPECIES_CHARMANDER, 20, ITEM_NONE);
}

TEST("HMs from the bag: without the HM, no Pokémon can use the move")
{
    SetUpParty();
    EXPECT_EQ(GetHMFieldMoveUser(FIELD_MOVE_CUT), PARTY_SIZE);
    EXPECT(!PartyHasMonWithSurf());
}

TEST("HMs from the bag: the first Pokémon that could learn the move uses it")
{
    SetUpParty();
    AddBagItem(ITEM_HM_CUT, 1);
    AddBagItem(ITEM_HM_SURF, 1);
    AddBagItem(ITEM_HM_FLY, 1);
    EXPECT_EQ(GetHMFieldMoveUser(FIELD_MOVE_CUT), 2);
    EXPECT_EQ(GetHMFieldMoveUser(FIELD_MOVE_SURF), 1);
    EXPECT_EQ(GetHMFieldMoveUser(FIELD_MOVE_FLY), 0); // Nobody could learn it, so the first Pokémon.
    EXPECT(PartyHasMonWithSurf());
}

TEST("HMs from the bag: the move still needs its badge")
{
    SetUpParty();
    AddBagItem(ITEM_HM_CUT, 1);
    RUN_OVERWORLD_SCRIPT(
        checkfieldmove 0, TRUE;
    );
    EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);

    FlagSet(FLAG_BADGE01_GET);
    RUN_OVERWORLD_SCRIPT(
        checkfieldmove 0, TRUE;
    );
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(gSpecialVar_0x8004, SPECIES_CHARMANDER);
}

TEST("HMs from the bag: a Pokémon that knows the move is still picked first")
{
    SetUpParty();
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][1], MOVE_CUT, 0);
    AddBagItem(ITEM_HM_CUT, 1);
    FlagSet(FLAG_BADGE01_GET);
    RUN_OVERWORLD_SCRIPT(
        checkfieldmove 0, TRUE;
    );
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(gSpecialVar_0x8004, SPECIES_SQUIRTLE);
}

TEST("HM moves can be forgotten")
{
    EXPECT(!CannotForgetMove(MOVE_SURF));
    EXPECT(!CannotForgetMove(MOVE_CUT));
}
