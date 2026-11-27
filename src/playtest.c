// Pokémon Emerald Champions: specials for the playtest setup (data/scripts/playtest.inc),
// which the debug menu and the playtest save (test/playtest_save.c) use.
#include "global.h"
#include "battle_setup.h"
#include "money.h"
#include "playtest.h"
#include "constants/opponents.h"

// Marks every trainer as beaten, so none of them battle when they see the player.
void Playtest_DefeatAllTrainers(void)
{
    u32 trainerId;

    for (trainerId = TRAINER_NONE + 1; trainerId < TRAINERS_COUNT; trainerId++)
        SetTrainerFlag(trainerId);
}

void Playtest_SetMaxMoney(void)
{
    SetMoney(&gSaveBlock1Ptr->money, MAX_MONEY);
}
