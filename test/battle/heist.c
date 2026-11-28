// Pokémon Emerald Champions: stealing a trainer's item.
// Thief and Covet no longer steal (as in Run & Bun), but a "heist" stays possible: an item
// taken from a trainer's Pokémon with Magician, Trick or Switcheroo is kept after the battle.
#include "global.h"
#include "test/battle.h"
#include "item.h"

SINGLE_BATTLE_TEST("Heist: Thief and Covet no longer steal the target's held item")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_LEFTOVERS);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Heist: a trainer's Thief and Covet no longer steal the player's held item")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

TEST("Heist: Covet is a Fairy-type move and Thief is still Dark")
{
    EXPECT_EQ(GetMoveType(MOVE_COVET), TYPE_FAIRY);
    EXPECT_EQ(GetMoveType(MOVE_THIEF), TYPE_DARK);
    EXPECT_EQ(GetMovePower(MOVE_COVET), 60);
    EXPECT_EQ(GetMovePower(MOVE_THIEF), 60);
}

SINGLE_BATTLE_TEST("Heist: Magician takes a trainer's item and it is kept after the battle")
{
    GIVEN {
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_KLEFKI) { Ability(ABILITY_MAGICIAN); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_MAGICIAN);
    } THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    }
}

// The Tate & Liza heist from Run & Bun: copy Magician with Role Play, then hit the holder.
SINGLE_BATTLE_TEST("Heist: Role Play copies Magician, and the next hit takes the item for good")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Ability(ABILITY_COLOR_CHANGE); }
        OPPONENT(SPECIES_KLEFKI) { Ability(ABILITY_MAGICIAN); Item(ITEM_LEFTOVERS); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROLE_PLAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROLE_PLAY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_MAGICIAN);
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_MAGICIAN);
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    }
}

SINGLE_BATTLE_TEST("Heist: Magician takes the item even when the hit knocks the holder out")
{
    GIVEN {
        PLAYER(SPECIES_KLEFKI) { Ability(ABILITY_MAGICIAN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Item(ITEM_LEFTOVERS); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_MAGICIAN);
    } THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    }
}

SINGLE_BATTLE_TEST("Heist: Trick and Switcheroo from empty hands take a trainer's item for good")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TRICK; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_FLAME_ORB); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_FLAME_ORB);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_FLAME_ORB);
    }
}

// A Pokémon that started the battle with a non-berry item gets it back afterwards, so a swap
// is undone and the trainer's item is lost. Run & Bun keeps the swap instead; the owner has
// been asked which rule he wants. Change this test with the rule.
SINGLE_BATTLE_TEST("Heist: a swap with your own item is undone after the battle (for now)")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_IRON_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(opponent->item, ITEM_IRON_BALL);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_IRON_BALL);
    }
}
