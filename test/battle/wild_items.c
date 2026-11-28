// Pokémon Emerald Champions: a wild Pokémon's held item only leaves the battle with a catch
// (B_WILD_ITEMS_NEED_CATCH), so wild items can't be farmed without catching.
// Trainer battles are not affected: test/battle/heist.c covers those.
#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "item.h"
#include "malloc.h"

ASSUMPTIONS
{
    ASSUME(B_WILD_ITEMS_NEED_CATCH == TRUE);
}

WILD_BATTLE_TEST("Wild items: Trick and Switcheroo fail against a wild Pokémon that holds an item")
{
    enum Move move;
    enum Item ownItem;
    PARAMETRIZE { move = MOVE_TRICK; ownItem = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; ownItem = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_TRICK; ownItem = ITEM_IRON_BALL; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; ownItem = ITEM_ORAN_BERRY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ownItem); }
        OPPONENT(SPECIES_ZIGZAGOON) { Item(ITEM_RARE_CANDY); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, move, player);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->item, ownItem);
        EXPECT_EQ(opponent->item, ITEM_RARE_CANDY);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ownItem);
    }
}

WILD_BATTLE_TEST("Wild items: Trick still hands your own item to a wild Pokémon that holds nothing, and it comes back after the battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_IRON_BALL); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_IRON_BALL);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_IRON_BALL);
    }
}

WILD_BATTLE_TEST("Wild items: Magician does not take a wild Pokémon's item, even on the hit that knocks it out")
{
    u32 hp;
    PARAMETRIZE { hp = 0; } // full HP
    PARAMETRIZE { hp = 1; }
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Ability(ABILITY_MAGICIAN); }
        OPPONENT(SPECIES_ZIGZAGOON) { if (hp != 0) HP(hp); Item(ITEM_RARE_CANDY); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_MAGICIAN);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_RARE_CANDY);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

WILD_BATTLE_TEST("Wild items: Pickpocket does not take a wild Pokémon's item")
{
    GIVEN {
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_SNEASEL) { Ability(ABILITY_PICKPOCKET); }
        OPPONENT(SPECIES_ZIGZAGOON) { Item(ITEM_RARE_CANDY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_PICKPOCKET);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_RARE_CANDY);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

WILD_BATTLE_TEST("Wild items: a wild Pokémon's Sticky Barb does not move to the Pokémon that touches it")
{
    GIVEN {
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        ASSUME(gItemsInfo[ITEM_STICKY_BARB].holdEffect == HOLD_EFFECT_STICKY_BARB);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_STICKY_BARB); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_STICKY_BARB);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

WILD_BATTLE_TEST("Wild items: Pickup does not take a berry that a wild Pokémon used up")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(51); Item(ITEM_SITRUS_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PICKUP);
            MESSAGE("Zigzagoon found one Sitrus Berry!");
        }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE); // the wild Pokémon did eat its berry
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

WILD_BATTLE_TEST("Wild items: a wild Pokémon's Bestow fails, so it can't hand its item over")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_BESTOW) == EFFECT_BESTOW);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ZIGZAGOON) { Item(ITEM_RARE_CANDY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BESTOW); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_BESTOW, opponent);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_RARE_CANDY);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

WILD_BATTLE_TEST("Wild items: a caught Pokémon keeps the item it was holding")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ZIGZAGOON) { Item(ITEM_RARE_CANDY); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL, WITH_RNG(RNG_BALLTHROW_SHAKE, 0)); }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM), ITEM_RARE_CANDY);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

// The safety net that runs when a wild battle ends. Nothing in the tests above can get a wild
// Pokémon's item onto the player's Pokémon any more, so the state is set up by hand here.
TEST("Wild items: when a wild battle ends, an item the wild Pokémon brought is removed from the player's Pokémon")
{
    enum Item item;
    struct BattleStruct *savedBattleStruct = gBattleStruct;

    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    ASSUME(gBattleStruct != NULL);

    // The wild Pokémon came in holding a Rare Candy.
    GetLostItemState(B_TRAINER_OPPONENT_A, 0)->originalItem = ITEM_RARE_CANDY;
    GetLostItemState(B_TRAINER_OPPONENT_A, 0)->wasInParty = TRUE;

    // Slot 0 started empty-handed and ended up with the wild Pokémon's Rare Candy: removed.
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 10, 0, OTID_STRUCT_PLAYER_ID);
    item = ITEM_RARE_CANDY;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    GetLostItemState(B_TRAINER_PLAYER, 0)->originalItem = ITEM_NONE;
    GetLostItemState(B_TRAINER_PLAYER, 0)->wasInParty = TRUE;

    // Slot 1 brought its own Rare Candy into the battle: kept.
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_WOBBUFFET, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM, &item);
    GetLostItemState(B_TRAINER_PLAYER, 1)->originalItem = ITEM_RARE_CANDY;
    GetLostItemState(B_TRAINER_PLAYER, 1)->wasInParty = TRUE;

    // Slot 2 fetched a Poké Ball the player threw (Ball Fetch): kept, it never belonged to the wild Pokémon.
    CreateMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_YAMPER, 10, 0, OTID_STRUCT_PLAYER_ID);
    item = ITEM_POKE_BALL;
    SetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HELD_ITEM, &item);
    GetLostItemState(B_TRAINER_PLAYER, 2)->originalItem = ITEM_NONE;
    GetLostItemState(B_TRAINER_PLAYER, 2)->wasInParty = TRUE;

    // Slot 3 was empty when the battle started: it is the wild Pokémon, caught with its item. Kept.
    CreateMon(&gParties[B_TRAINER_PLAYER][3], SPECIES_ZIGZAGOON, 3, 0, OTID_STRUCT_PLAYER_ID);
    item = ITEM_RARE_CANDY;
    SetMonData(&gParties[B_TRAINER_PLAYER][3], MON_DATA_HELD_ITEM, &item);
    GetLostItemState(B_TRAINER_PLAYER, 3)->originalItem = ITEM_NONE;
    GetLostItemState(B_TRAINER_PLAYER, 3)->wasInParty = FALSE;

    RemoveWildItemsFromPlayerParty();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM), ITEM_RARE_CANDY);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HELD_ITEM), ITEM_POKE_BALL);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][3], MON_DATA_HELD_ITEM), ITEM_RARE_CANDY);

    Free(gBattleStruct);
    gBattleStruct = savedBattleStruct;
}
