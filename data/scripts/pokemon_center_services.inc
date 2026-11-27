@ Pokémon Emerald Champions: the services attendant who stands at (12,2) in every Pokémon Center,
@ recreating Run & Bun's. She can help a Pokémon relearn a move, forget a move, max out an IV,
@ change its nature, change its nickname or give it a status. Heart Scale prices and menu rows:
@ include/constants/pokemon_center_services.h. Specials: src/pokemon_center_services.c.
@ After each service she offers the menu again.
@ VAR_0x8004 = the chosen Pokémon (party slot, or PC_MON_CHOSEN for a box Pokémon),
@ VAR_0x8005 = the chosen stat, nature or status, VAR_0x8006 = scratch, VAR_0x8007 = a price.
PokemonCenter_EventScript_Services::
	lock
	faceplayer
	message PcServices_Text_Welcome
	waitmessage
PcServices_EventScript_Menu:
	dynmultipush PcServices_Text_MenuRelearn, 0
	dynmultipush PcServices_Text_MenuForget, 1
	dynmultipush PcServices_Text_MenuIV, 2
	dynmultipush PcServices_Text_MenuNature, 3
	dynmultipush PcServices_Text_MenuNickname, 4
	dynmultipush PcServices_Text_MenuStatus, 5
	dynmultipush PcServices_Text_MenuGoodbye, 6
	dynmultistack 0, 0, FALSE, 6, FALSE, 0, DYN_MULTICHOICE_CB_NONE
	switch VAR_RESULT
	case 0, PcServices_EventScript_Relearn
	case 1, PcServices_EventScript_Forget
	case 2, PcServices_EventScript_IV
	case 3, PcServices_EventScript_Nature
	case 4, PcServices_EventScript_Nickname
	case 5, PcServices_EventScript_Status
	msgbox PcServices_Text_Goodbye, MSGBOX_DEFAULT
	release
	end

PcServices_EventScript_AnythingElse:
	message PcServices_Text_AnythingElse
	waitmessage
	goto PcServices_EventScript_Menu

@ VAR_0x8007 = a price. Buffers the number into STR_VAR_1 and "Heart Scale(s)" into STR_VAR_2.
PcServices_EventScript_BufferPrice:
	buffernumberstring STR_VAR_1, VAR_0x8007
	bufferitemnameplural STR_VAR_2, ITEM_HEART_SCALE, VAR_0x8007
	return

PcServices_EventScript_TooFewScales:
	msgbox PcServices_Text_TooFewScales, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ VAR_0x8007 = the price of the service just done.
PcServices_EventScript_Pay:
	call PcServices_EventScript_BufferPrice
	removeitem ITEM_HEART_SCALE, VAR_0x8007
	msgbox PcServices_Text_HandedOverScales, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

PcServices_EventScript_Egg:
	msgbox PcServices_Text_Egg, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ Relearn a level-up move, for PC_SERVICES_PRICE_RELEARN Heart Scales.
PcServices_EventScript_Relearn:
	setvar VAR_0x8007, PC_SERVICES_PRICE_RELEARN
	call PcServices_EventScript_BufferPrice
	checkitem ITEM_HEART_SCALE, PC_SERVICES_PRICE_RELEARN
	goto_if_eq VAR_RESULT, FALSE, PcServices_EventScript_TooFewScales
	msgbox PcServices_Text_RelearnOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PcServices_EventScript_AnythingElse
PcServices_EventScript_RelearnChooseMon:
	msgbox PcServices_Text_RelearnWhichMon, MSGBOX_DEFAULT
	setmoverelearnerstate MOVE_RELEARNER_LEVEL_UP_MOVES
	chooseboxmon SELECT_PC_MON_MOVE_RELEARNER
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, PcServices_EventScript_AnythingElse
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_Egg
	special Special_HasMoveToRelearn
	goto_if_eq VAR_RESULT, FALSE, PcServices_EventScript_RelearnNothing
	msgbox PcServices_Text_RelearnWhichMove, MSGBOX_DEFAULT
	special TeachMoveRelearnerMove
	goto_if_eq VAR_RESULT, FALSE, PcServices_EventScript_AnythingElse
	setvar VAR_0x8007, PC_SERVICES_PRICE_RELEARN
	goto PcServices_EventScript_Pay

PcServices_EventScript_RelearnNothing:
	msgbox PcServices_Text_RelearnNothing, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ Forget a move, free (as the Lilycove Move Deleter does it).
PcServices_EventScript_Forget:
	msgbox PcServices_Text_ForgetWhichMon, MSGBOX_DEFAULT
	chooseboxmon SELECT_PC_MON_MOVE_DELETER
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, PcServices_EventScript_AnythingElse
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_Egg
	special GetNumMovesSelectedMonHas
	goto_if_eq VAR_RESULT, 1, PcServices_EventScript_ForgetOnlyMove
	msgbox PcServices_Text_ForgetWhichMove, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	special MoveDeleterChooseMoveToForget
	fadescreen FADE_FROM_BLACK
	goto_if_eq VAR_0x8005, MAX_MON_MOVES, PcServices_EventScript_AnythingElse
	special BufferMoveDeleterNicknameAndMove
	msgbox PcServices_Text_ForgetConfirm, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PcServices_EventScript_AnythingElse
	special IsLastMonThatKnowsSurf
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_ForgetLastSurf
	special MoveDeleterForgetMove
	playfanfare MUS_MOVE_DELETED
	waitfanfare
	msgbox PcServices_Text_ForgetDone, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

PcServices_EventScript_ForgetOnlyMove:
	special BufferMoveDeleterNicknameAndMove
	msgbox PcServices_Text_ForgetOnlyMove, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

PcServices_EventScript_ForgetLastSurf:
	msgbox PcServices_Text_ForgetLastSurf, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ Max out one IV, for PC_SERVICES_PRICE_IV Heart Scales. The menu shows the current IVs.
PcServices_EventScript_IV:
	setvar VAR_0x8007, PC_SERVICES_PRICE_IV
	call PcServices_EventScript_BufferPrice
	checkitem ITEM_HEART_SCALE, PC_SERVICES_PRICE_IV
	goto_if_eq VAR_RESULT, FALSE, PcServices_EventScript_TooFewScales
	msgbox PcServices_Text_IVOffer, MSGBOX_DEFAULT
	chooseboxmon
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, PcServices_EventScript_AnythingElse
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_Egg
	special BufferMonNickname
	message PcServices_Text_IVWhichStat
	waitmessage
	setvar VAR_0x8005, PC_SERVICES_STAT_HP
	specialvar VAR_RESULT, PcServices_GetIV
	buffernumberstring STR_VAR_2, VAR_RESULT
	dynmultipush PcServices_Text_IVRowHp, PC_SERVICES_STAT_HP
	setvar VAR_0x8005, PC_SERVICES_STAT_ATK
	specialvar VAR_RESULT, PcServices_GetIV
	buffernumberstring STR_VAR_2, VAR_RESULT
	dynmultipush PcServices_Text_IVRowAtk, PC_SERVICES_STAT_ATK
	setvar VAR_0x8005, PC_SERVICES_STAT_DEF
	specialvar VAR_RESULT, PcServices_GetIV
	buffernumberstring STR_VAR_2, VAR_RESULT
	dynmultipush PcServices_Text_IVRowDef, PC_SERVICES_STAT_DEF
	setvar VAR_0x8005, PC_SERVICES_STAT_SPATK
	specialvar VAR_RESULT, PcServices_GetIV
	buffernumberstring STR_VAR_2, VAR_RESULT
	dynmultipush PcServices_Text_IVRowSpatk, PC_SERVICES_STAT_SPATK
	setvar VAR_0x8005, PC_SERVICES_STAT_SPDEF
	specialvar VAR_RESULT, PcServices_GetIV
	buffernumberstring STR_VAR_2, VAR_RESULT
	dynmultipush PcServices_Text_IVRowSpdef, PC_SERVICES_STAT_SPDEF
	setvar VAR_0x8005, PC_SERVICES_STAT_SPEED
	specialvar VAR_RESULT, PcServices_GetIV
	buffernumberstring STR_VAR_2, VAR_RESULT
	dynmultipush PcServices_Text_IVRowSpeed, PC_SERVICES_STAT_SPEED
	dynmultipush PcServices_Text_Cancel, PC_SERVICES_STAT_COUNT
	dynmultistack 0, 0, FALSE, 6, FALSE, 0, DYN_MULTICHOICE_CB_NONE
	goto_if_ge VAR_RESULT, PC_SERVICES_STAT_COUNT, PcServices_EventScript_AnythingElse
	copyvar VAR_0x8005, VAR_RESULT
	call PcServices_EventScript_BufferStatName
	special BufferMonNickname
	specialvar VAR_RESULT, PcServices_GetIV
	goto_if_eq VAR_RESULT, MAX_PER_STAT_IVS, PcServices_EventScript_IVAlreadyMax
	msgbox PcServices_Text_IVConfirm, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PcServices_EventScript_AnythingElse
	special PcServices_MaxIV
	playfanfare MUS_LEVEL_UP
	msgbox PcServices_Text_IVDone, MSGBOX_DEFAULT
	waitfanfare
	setvar VAR_0x8007, PC_SERVICES_PRICE_IV
	goto PcServices_EventScript_Pay

PcServices_EventScript_IVAlreadyMax:
	msgbox PcServices_Text_IVAlreadyMax, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ VAR_0x8005 = a PC_SERVICES_STAT_* row. Buffers the stat's summary-screen name into STR_VAR_3.
PcServices_EventScript_BufferStatName:
	switch VAR_0x8005
	case PC_SERVICES_STAT_ATK, PcServices_EventScript_StatNameAtk
	case PC_SERVICES_STAT_DEF, PcServices_EventScript_StatNameDef
	case PC_SERVICES_STAT_SPATK, PcServices_EventScript_StatNameSpatk
	case PC_SERVICES_STAT_SPDEF, PcServices_EventScript_StatNameSpdef
	case PC_SERVICES_STAT_SPEED, PcServices_EventScript_StatNameSpeed
	bufferstring STR_VAR_3, gText_HP4
	return
PcServices_EventScript_StatNameAtk:
	bufferstring STR_VAR_3, gText_Attack3
	return
PcServices_EventScript_StatNameDef:
	bufferstring STR_VAR_3, gText_Defense3
	return
PcServices_EventScript_StatNameSpatk:
	bufferstring STR_VAR_3, gText_SpAtk4
	return
PcServices_EventScript_StatNameSpdef:
	bufferstring STR_VAR_3, gText_SpDef4
	return
PcServices_EventScript_StatNameSpeed:
	bufferstring STR_VAR_3, gText_Speed2
	return

@ Change the nature (like a Mint, so gender, ability and shininess stay), for
@ PC_SERVICES_PRICE_NATURE Heart Scales. The menu opens on the current nature.
PcServices_EventScript_Nature:
	setvar VAR_0x8007, PC_SERVICES_PRICE_NATURE
	call PcServices_EventScript_BufferPrice
	checkitem ITEM_HEART_SCALE, PC_SERVICES_PRICE_NATURE
	goto_if_eq VAR_RESULT, FALSE, PcServices_EventScript_TooFewScales
	msgbox PcServices_Text_NatureOffer, MSGBOX_DEFAULT
	chooseboxmon
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, PcServices_EventScript_AnythingElse
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_Egg
	special BufferMonNickname
	specialvar VAR_0x8006, PcServices_GetNature
	message PcServices_Text_NatureWhich
	waitmessage
	dynmultipush PcServices_Text_NatureHardy, NATURE_HARDY
	dynmultipush PcServices_Text_NatureLonely, NATURE_LONELY
	dynmultipush PcServices_Text_NatureBrave, NATURE_BRAVE
	dynmultipush PcServices_Text_NatureAdamant, NATURE_ADAMANT
	dynmultipush PcServices_Text_NatureNaughty, NATURE_NAUGHTY
	dynmultipush PcServices_Text_NatureBold, NATURE_BOLD
	dynmultipush PcServices_Text_NatureDocile, NATURE_DOCILE
	dynmultipush PcServices_Text_NatureRelaxed, NATURE_RELAXED
	dynmultipush PcServices_Text_NatureImpish, NATURE_IMPISH
	dynmultipush PcServices_Text_NatureLax, NATURE_LAX
	dynmultipush PcServices_Text_NatureTimid, NATURE_TIMID
	dynmultipush PcServices_Text_NatureHasty, NATURE_HASTY
	dynmultipush PcServices_Text_NatureSerious, NATURE_SERIOUS
	dynmultipush PcServices_Text_NatureJolly, NATURE_JOLLY
	dynmultipush PcServices_Text_NatureNaive, NATURE_NAIVE
	dynmultipush PcServices_Text_NatureModest, NATURE_MODEST
	dynmultipush PcServices_Text_NatureMild, NATURE_MILD
	dynmultipush PcServices_Text_NatureQuiet, NATURE_QUIET
	dynmultipush PcServices_Text_NatureBashful, NATURE_BASHFUL
	dynmultipush PcServices_Text_NatureRash, NATURE_RASH
	dynmultipush PcServices_Text_NatureCalm, NATURE_CALM
	dynmultipush PcServices_Text_NatureGentle, NATURE_GENTLE
	dynmultipush PcServices_Text_NatureSassy, NATURE_SASSY
	dynmultipush PcServices_Text_NatureCareful, NATURE_CAREFUL
	dynmultipush PcServices_Text_NatureQuirky, NATURE_QUIRKY
	dynmultipush PcServices_Text_Cancel, NUM_NATURES
	dynmultistack 0, 0, FALSE, 6, FALSE, VAR_0x8006, DYN_MULTICHOICE_CB_NONE
	goto_if_ge VAR_RESULT, NUM_NATURES, PcServices_EventScript_AnythingElse
	goto_if_eq VAR_RESULT, VAR_0x8006, PcServices_EventScript_NatureSame
	copyvar VAR_0x8005, VAR_RESULT
	special PcServices_BufferNatureName
	msgbox PcServices_Text_NatureConfirm, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PcServices_EventScript_AnythingElse
	special PcServices_SetNature
	playfanfare MUS_LEVEL_UP
	msgbox PcServices_Text_NatureDone, MSGBOX_DEFAULT
	waitfanfare
	setvar VAR_0x8007, PC_SERVICES_PRICE_NATURE
	goto PcServices_EventScript_Pay

PcServices_EventScript_NatureSame:
	msgbox PcServices_Text_NatureSame, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ Change a nickname, free (the Name Rater's rules: only Pokémon the player caught or was given).
PcServices_EventScript_Nickname:
	msgbox PcServices_Text_NicknameWhichMon, MSGBOX_DEFAULT
	chooseboxmon
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, PcServices_EventScript_AnythingElse
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_Egg
	special IsMonOTIDNotPlayers
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_NicknameNotYours
	specialvar VAR_RESULT, MonOTNameNotPlayer
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_NicknameNotYours
	special BufferMonNickname
	msgbox PcServices_Text_NicknameWhatName, MSGBOX_DEFAULT
	call Common_EventScript_NameReceivedPartyMon
	special BufferMonNickname
	msgbox PcServices_Text_NicknameDone, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

PcServices_EventScript_NicknameNotYours:
	special BufferMonNickname
	msgbox PcServices_Text_NicknameNotYours, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ Give a party Pokémon a status, free. A status can't be held in a box, so it's party only.
PcServices_EventScript_Status:
	message PcServices_Text_StatusWhich
	waitmessage
	dynmultipush PcServices_Text_StatusBurn, PC_SERVICES_STATUS_BURN
	dynmultipush PcServices_Text_StatusFreeze, PC_SERVICES_STATUS_FREEZE
	dynmultipush PcServices_Text_StatusParalysis, PC_SERVICES_STATUS_PARALYSIS
	dynmultipush PcServices_Text_StatusPoison, PC_SERVICES_STATUS_POISON
	dynmultipush PcServices_Text_StatusSleep, PC_SERVICES_STATUS_SLEEP
	dynmultipush PcServices_Text_Cancel, PC_SERVICES_STATUS_COUNT
	dynmultistack 0, 0, FALSE, 6, FALSE, 0, DYN_MULTICHOICE_CB_NONE
	goto_if_ge VAR_RESULT, PC_SERVICES_STATUS_COUNT, PcServices_EventScript_AnythingElse
	copyvar VAR_0x8006, VAR_RESULT
	call_if_eq VAR_0x8006, PC_SERVICES_STATUS_SLEEP, PcServices_EventScript_SleepNote
	msgbox PcServices_Text_StatusWhichMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, PcServices_EventScript_AnythingElse
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, PcServices_EventScript_Egg
	copyvar VAR_0x8005, VAR_0x8006
	call PcServices_EventScript_BufferStatusWord
	special BufferMonNickname
	specialvar VAR_RESULT, PcServices_CheckStatus
	goto_if_eq VAR_RESULT, PC_SERVICES_STATUS_IMMUNE, PcServices_EventScript_StatusImmune
	goto_if_eq VAR_RESULT, PC_SERVICES_STATUS_FAINTED, PcServices_EventScript_StatusFainted
	msgbox PcServices_Text_StatusConfirm, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PcServices_EventScript_AnythingElse
	special PcServices_GiveStatus
	msgbox PcServices_Text_StatusDone, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

PcServices_EventScript_SleepNote:
	msgbox PcServices_Text_SleepNote, MSGBOX_DEFAULT
	return

PcServices_EventScript_StatusImmune:
	msgbox PcServices_Text_StatusImmune, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

PcServices_EventScript_StatusFainted:
	msgbox PcServices_Text_StatusFainted, MSGBOX_DEFAULT
	goto PcServices_EventScript_AnythingElse

@ VAR_0x8005 = a PC_SERVICES_STATUS_* row. Buffers "burned", "frozen"... into STR_VAR_3.
PcServices_EventScript_BufferStatusWord:
	switch VAR_0x8005
	case PC_SERVICES_STATUS_FREEZE, PcServices_EventScript_StatusWordFreeze
	case PC_SERVICES_STATUS_PARALYSIS, PcServices_EventScript_StatusWordParalysis
	case PC_SERVICES_STATUS_POISON, PcServices_EventScript_StatusWordPoison
	case PC_SERVICES_STATUS_SLEEP, PcServices_EventScript_StatusWordSleep
	bufferstring STR_VAR_3, PcServices_Text_WordBurn
	return
PcServices_EventScript_StatusWordFreeze:
	bufferstring STR_VAR_3, PcServices_Text_WordFreeze
	return
PcServices_EventScript_StatusWordParalysis:
	bufferstring STR_VAR_3, PcServices_Text_WordParalysis
	return
PcServices_EventScript_StatusWordPoison:
	bufferstring STR_VAR_3, PcServices_Text_WordPoison
	return
PcServices_EventScript_StatusWordSleep:
	bufferstring STR_VAR_3, PcServices_Text_WordSleep
	return

PcServices_Text_Welcome:
	.string "Hello! How can I help your\n"
	.string "POKéMON today?$"

PcServices_Text_AnythingElse:
	.string "Can I do anything else for you?$"

PcServices_Text_Goodbye:
	.string "Come back anytime!$"

PcServices_Text_MenuRelearn:
	.string "Relearn a move$"

PcServices_Text_MenuForget:
	.string "Forget a move$"

PcServices_Text_MenuIV:
	.string "Max out an IV$"

PcServices_Text_MenuNature:
	.string "Change nature$"

PcServices_Text_MenuNickname:
	.string "Change nickname$"

PcServices_Text_MenuStatus:
	.string "Give a status$"

PcServices_Text_MenuGoodbye:
	.string "Goodbye$"

PcServices_Text_Cancel:
	.string "Cancel$"

PcServices_Text_TooFewScales:
	.string "That costs {STR_VAR_1} {STR_VAR_2}, and\n"
	.string "you don't have enough.$"

PcServices_Text_HandedOverScales:
	.string "{PLAYER} handed over\n"
	.string "{STR_VAR_1} {STR_VAR_2}.$"

PcServices_Text_Egg:
	.string "That's an EGG! There's nothing I\n"
	.string "can do for it until it hatches.$"

PcServices_Text_RelearnOffer:
	.string "For {STR_VAR_1} {STR_VAR_2}, I can help a\n"
	.string "POKéMON remember a move it learned\l"
	.string "by leveling up. Shall I?$"

PcServices_Text_RelearnWhichMon:
	.string "Which POKéMON should remember\n"
	.string "a move?$"

PcServices_Text_RelearnNothing:
	.string "That POKéMON has no moves to\n"
	.string "remember yet.$"

PcServices_Text_RelearnWhichMove:
	.string "Which move should it remember?$"

PcServices_Text_ForgetWhichMon:
	.string "Forgetting a move is free.\n"
	.string "Which POKéMON should forget one?$"

PcServices_Text_ForgetWhichMove:
	.string "Which move should it forget?$"

PcServices_Text_ForgetConfirm:
	.string "Should {STR_VAR_1} forget\n"
	.string "{STR_VAR_2}?$"

PcServices_Text_ForgetDone:
	.string "{STR_VAR_1} forgot {STR_VAR_2}.$"

PcServices_Text_ForgetOnlyMove:
	.string "{STR_VAR_1} only knows\n"
	.string "{STR_VAR_2}!\p"
	.string "A POKéMON can't forget its\n"
	.string "last move.$"

PcServices_Text_ForgetLastSurf:
	.string "None of your other POKéMON know\n"
	.string "SURF. I can't let it forget that!$"

PcServices_Text_IVOffer:
	.string "For {STR_VAR_1} {STR_VAR_2}, I can raise one\n"
	.string "IV of a POKéMON to 31.\p"
	.string "Which POKéMON should I train?$"

PcServices_Text_IVWhichStat:
	.string "Which of {STR_VAR_1}'s IVs\n"
	.string "should I raise?$"

PcServices_Text_IVRowHp:
	.string "HP: {STR_VAR_2}$"

PcServices_Text_IVRowAtk:
	.string "ATTACK: {STR_VAR_2}$"

PcServices_Text_IVRowDef:
	.string "DEFENSE: {STR_VAR_2}$"

PcServices_Text_IVRowSpatk:
	.string "SP. ATK: {STR_VAR_2}$"

PcServices_Text_IVRowSpdef:
	.string "SP. DEF: {STR_VAR_2}$"

PcServices_Text_IVRowSpeed:
	.string "SPEED: {STR_VAR_2}$"

PcServices_Text_IVAlreadyMax:
	.string "{STR_VAR_1}'s {STR_VAR_3} IV is already\n"
	.string "31. It can't go any higher!$"

PcServices_Text_IVConfirm:
	.string "Raise {STR_VAR_1}'s {STR_VAR_3}\n"
	.string "IV to 31?$"

PcServices_Text_IVDone:
	.string "{STR_VAR_1}'s {STR_VAR_3} IV\n"
	.string "is now 31!$"

PcServices_Text_NatureOffer:
	.string "For {STR_VAR_1} {STR_VAR_2}, I can change\n"
	.string "a POKéMON's nature.\p"
	.string "Its gender and ability stay the\n"
	.string "same. Which POKéMON?$"

PcServices_Text_NatureWhich:
	.string "{STR_VAR_1}'s nature is {STR_VAR_2}.\n"
	.string "Which nature should it have?$"

PcServices_Text_NatureSame:
	.string "{STR_VAR_1} already has that\n"
	.string "nature!$"

PcServices_Text_NatureConfirm:
	.string "Change {STR_VAR_1}'s nature\n"
	.string "to {STR_VAR_3}?$"

PcServices_Text_NatureDone:
	.string "{STR_VAR_1}'s nature is\n"
	.string "now {STR_VAR_3}!$"

PcServices_Text_NicknameWhichMon:
	.string "Changing a nickname is free.\n"
	.string "Which POKéMON gets a new name?$"

PcServices_Text_NicknameNotYours:
	.string "{STR_VAR_1} was named by another\n"
	.string "TRAINER. I can't change its name.$"

PcServices_Text_NicknameWhatName:
	.string "What should its new nickname be?$"

PcServices_Text_NicknameDone:
	.string "From now on, it's {STR_VAR_1}!$"

PcServices_Text_StatusWhich:
	.string "Giving a status is free.\n"
	.string "Which status should I give?$"

PcServices_Text_StatusBurn:
	.string "Burn$"

PcServices_Text_StatusFreeze:
	.string "Freeze$"

PcServices_Text_StatusParalysis:
	.string "Paralysis$"

PcServices_Text_StatusPoison:
	.string "Poison$"

PcServices_Text_StatusSleep:
	.string "Sleep$"

PcServices_Text_WordBurn:
	.string "burned$"

PcServices_Text_WordFreeze:
	.string "frozen$"

PcServices_Text_WordParalysis:
	.string "paralyzed$"

PcServices_Text_WordPoison:
	.string "poisoned$"

PcServices_Text_WordSleep:
	.string "asleep$"

PcServices_Text_SleepNote:
	.string "A POKéMON I put to sleep stays\n"
	.string "asleep for 6 turns in battle.$"

PcServices_Text_StatusWhichMon:
	.string "Which POKéMON in your party?$"

PcServices_Text_StatusConfirm:
	.string "{STR_VAR_1} will be {STR_VAR_3}.\n"
	.string "Is that OK?$"

PcServices_Text_StatusImmune:
	.string "{STR_VAR_1} can't be {STR_VAR_3}!$"

PcServices_Text_StatusFainted:
	.string "{STR_VAR_1} has fainted! Have it\n"
	.string "healed first.$"

PcServices_Text_StatusDone:
	.string "{STR_VAR_1} is now {STR_VAR_3}.$"

PcServices_Text_NatureHardy:
	.string "Hardy (no change)$"

PcServices_Text_NatureLonely:
	.string "Lonely +ATTACK -DEFENSE$"

PcServices_Text_NatureBrave:
	.string "Brave +ATTACK -SPEED$"

PcServices_Text_NatureAdamant:
	.string "Adamant +ATTACK -SP. ATK$"

PcServices_Text_NatureNaughty:
	.string "Naughty +ATTACK -SP. DEF$"

PcServices_Text_NatureBold:
	.string "Bold +DEFENSE -ATTACK$"

PcServices_Text_NatureDocile:
	.string "Docile (no change)$"

PcServices_Text_NatureRelaxed:
	.string "Relaxed +DEFENSE -SPEED$"

PcServices_Text_NatureImpish:
	.string "Impish +DEFENSE -SP. ATK$"

PcServices_Text_NatureLax:
	.string "Lax +DEFENSE -SP. DEF$"

PcServices_Text_NatureTimid:
	.string "Timid +SPEED -ATTACK$"

PcServices_Text_NatureHasty:
	.string "Hasty +SPEED -DEFENSE$"

PcServices_Text_NatureSerious:
	.string "Serious (no change)$"

PcServices_Text_NatureJolly:
	.string "Jolly +SPEED -SP. ATK$"

PcServices_Text_NatureNaive:
	.string "Naive +SPEED -SP. DEF$"

PcServices_Text_NatureModest:
	.string "Modest +SP. ATK -ATTACK$"

PcServices_Text_NatureMild:
	.string "Mild +SP. ATK -DEFENSE$"

PcServices_Text_NatureQuiet:
	.string "Quiet +SP. ATK -SPEED$"

PcServices_Text_NatureBashful:
	.string "Bashful (no change)$"

PcServices_Text_NatureRash:
	.string "Rash +SP. ATK -SP. DEF$"

PcServices_Text_NatureCalm:
	.string "Calm +SP. DEF -ATTACK$"

PcServices_Text_NatureGentle:
	.string "Gentle +SP. DEF -DEFENSE$"

PcServices_Text_NatureSassy:
	.string "Sassy +SP. DEF -SPEED$"

PcServices_Text_NatureCareful:
	.string "Careful +SP. DEF -SP. ATK$"

PcServices_Text_NatureQuirky:
	.string "Quirky (no change)$"
