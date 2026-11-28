@ Pokémon Emerald Champions: playtest setup, a game state for moving around freely.
@ Used by the debug menu (hold R and press START, then Utilities > Playtest setup) and by
@ test/playtest_save.c, which writes the ready-made playtest save (tools/make_playtest_save.py).
@ Everything here works without the overworld (no messages, movements or objects), so the
@ test can run it too. Object flags only show on the next map load.
@
@ What it gives: all 8 badges and all 8 HMs, every trainer marked as beaten, every town on the
@ FLY map, both bikes, the Super Rod, a level 100 team, and the story counted as done up to Wattson
@ in this hack's order (the intro, the woods theft, the museum, the LETTER for STEVEN, Petalburg's
@ first visit, the Route 110 rival, Wally in Mauville). Later story scenes are vanilla and untouched.
Playtest_EventScript_Setup::
	@ The intro, Littleroot and Birch's lab (as the debug menu's Cheat start does it).
	setflag FLAG_SYS_POKEMON_GET
	setflag FLAG_RESCUED_BIRCH
	setflag FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE
	setflag FLAG_ADVENTURE_STARTED
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH
	setflag FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG
	clearflag FLAG_HIDE_ROUTE_101_BOY
	setvar VAR_BIRCH_LAB_STATE, 5
	setvar VAR_ROUTE101_STATE, 3
	setflag FLAG_SYS_POKEDEX_GET
	special SetUnlockedPokedexFlags
	setflag FLAG_RECEIVED_POKEDEX_FROM_BIRCH
	setvar VAR_CABLE_CLUB_TUTORIAL_STATE, 2
	setflag FLAG_SYS_NATIONAL_DEX
	special EnableNationalPokedex
	setflag FLAG_RECEIVED_RUNNING_SHOES
	setflag FLAG_SYS_B_DASH
	setflag FLAG_SET_WALL_CLOCK
	setvar VAR_LITTLEROOT_TOWN_STATE, 4
	setvar VAR_LITTLEROOT_INTRO_STATE, 7
	setvar VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 2
	setvar VAR_LITTLEROOT_HOUSES_STATE_MAY, 2
	setvar VAR_LITTLEROOT_RIVAL_STATE, 4
	setflag FLAG_RIVAL_LEFT_FOR_ROUTE103
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL

	@ The Route 103 rival battle and the rival in Oldale.
	setflag FLAG_DEFEATED_RIVAL_ROUTE103
	setflag FLAG_HIDE_ROUTE_103_RIVAL
	setvar VAR_OLDALE_TOWN_STATE, 1
	setvar VAR_OLDALE_RIVAL_STATE, 2
	setflag FLAG_HIDE_OLDALE_TOWN_RIVAL

	@ Petalburg: Norman, Wally's catching lesson and Scott, as after Norman's badge and
	@ Wally's father handing over HM SURF.
	setvar VAR_PETALBURG_CITY_STATE, 5
	setvar VAR_PETALBURG_GYM_STATE, 7
	setflag FLAG_HIDE_PETALBURG_CITY_WALLY
	setflag FLAG_HIDE_PETALBURG_CITY_WALLYS_MOM
	setflag FLAG_HIDE_PETALBURG_GYM_WALLY
	setvar VAR_SCOTT_PETALBURG_ENCOUNTER, 1
	setflag FLAG_HIDE_PETALBURG_CITY_SCOTT

	@ Petalburg Woods: TEAM AQUA took the DEVON GOODS and the DEVON RESEARCHER has left.
	setvar VAR_PETALBURG_WOODS_STATE, 1
	setflag FLAG_HIDE_PETALBURG_WOODS_AQUA_GRUNT
	setflag FLAG_HIDE_PETALBURG_WOODS_DEVON_EMPLOYEE

	@ Mr. Briney is home with Peeko (as in the debug menu's Cheat start).
	setvar VAR_BRINEY_HOUSE_STATE, 1
	clearflag FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY
	clearflag FLAG_HIDE_BRINEYS_HOUSE_PEEKO
	setvar VAR_ROUTE116_STATE, 2
	setflag FLAG_HIDE_ROUTE_116_MR_BRINEY

	@ The Slateport museum boss: CAPT. STERN has the parts back and gave the LETTER.
	setvar VAR_SLATEPORT_MUSEUM_1F_STATE, 1
	setflag FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_AQUA_GRUNTS
	setflag FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_CAPTAIN_STERN
	setflag FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_ARCHIE
	setflag FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_AQUA_GRUNT_1
	setflag FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_AQUA_GRUNT_2
	setflag FLAG_HIDE_ROUTE_110_TEAM_AQUA
	setflag FLAG_DELIVERED_DEVON_GOODS
	clearflag FLAG_HIDE_ROUTE_116_DEVON_EMPLOYEE
	setflag FLAG_HIDE_RUSTBORO_CITY_DEVON_CORP_3F_EMPLOYEE
	setvar VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE, 1

	@ STEVEN has the LETTER, so the BLACK BELT no longer blocks BRAWLY's GYM.
	setflag FLAG_DELIVERED_STEVEN_LETTER
	setflag FLAG_HIDE_GRANITE_CAVE_STEVEN

	@ The Route 110 rival battle.
	setvar VAR_ROUTE110_STATE, 1
	setflag FLAG_HIDE_ROUTE_110_RIVAL
	setflag FLAG_HIDE_ROUTE_110_RIVAL_ON_BIKE

	@ Wally's battle in front of Mauville's gym; he moved on to Verdanturf.
	setflag FLAG_DEFEATED_WALLY_MAUVILLE
	setflag FLAG_HIDE_MAUVILLE_CITY_WALLY
	setflag FLAG_HIDE_MAUVILLE_CITY_WALLYS_UNCLE
	clearflag FLAG_HIDE_VERDANTURF_TOWN_WANDAS_HOUSE_WALLY
	clearflag FLAG_HIDE_VERDANTURF_TOWN_WANDAS_HOUSE_WALLYS_UNCLE

	@ All 8 badges, and every gym counts as beaten.
	setflag FLAG_BADGE01_GET
	setflag FLAG_BADGE02_GET
	setflag FLAG_BADGE03_GET
	setflag FLAG_BADGE04_GET
	setflag FLAG_BADGE05_GET
	setflag FLAG_BADGE06_GET
	setflag FLAG_BADGE07_GET
	setflag FLAG_BADGE08_GET
	setflag FLAG_DEFEATED_RUSTBORO_GYM
	setflag FLAG_DEFEATED_DEWFORD_GYM
	setflag FLAG_DEFEATED_MAUVILLE_GYM
	setflag FLAG_DEFEATED_LAVARIDGE_GYM
	setflag FLAG_DEFEATED_PETALBURG_GYM
	setflag FLAG_DEFEATED_FORTREE_GYM
	setflag FLAG_DEFEATED_MOSSDEEP_GYM
	setflag FLAG_DEFEATED_SOOTOPOLIS_GYM

	@ Every trainer counts as beaten.
	special Playtest_DefeatAllTrainers

	@ Every town and city can be flown to.
	setflag FLAG_VISITED_LITTLEROOT_TOWN
	setflag FLAG_VISITED_OLDALE_TOWN
	setflag FLAG_VISITED_DEWFORD_TOWN
	setflag FLAG_VISITED_LAVARIDGE_TOWN
	setflag FLAG_VISITED_FALLARBOR_TOWN
	setflag FLAG_VISITED_VERDANTURF_TOWN
	setflag FLAG_VISITED_PACIFIDLOG_TOWN
	setflag FLAG_VISITED_PETALBURG_CITY
	setflag FLAG_VISITED_SLATEPORT_CITY
	setflag FLAG_VISITED_MAUVILLE_CITY
	setflag FLAG_VISITED_RUSTBORO_CITY
	setflag FLAG_VISITED_FORTREE_CITY
	setflag FLAG_VISITED_LILYCOVE_CITY
	setflag FLAG_VISITED_MOSSDEEP_CITY
	setflag FLAG_VISITED_SOOTOPOLIS_CITY
	setflag FLAG_VISITED_EVER_GRANDE_CITY
	setflag FLAG_LANDMARK_POKEMON_LEAGUE
	setflag FLAG_LANDMARK_BATTLE_FRONTIER

	@ All 8 HMs. They work from the bag (OW_HM_FIELD_MOVES_FROM_BAG).
	additem ITEM_HM_CUT
	additem ITEM_HM_FLY
	additem ITEM_HM_SURF
	additem ITEM_HM_STRENGTH
	additem ITEM_HM_FLASH
	additem ITEM_HM_ROCK_SMASH
	additem ITEM_HM_WATERFALL
	additem ITEM_HM_DIVE
	setflag FLAG_RECEIVED_HM_CUT
	setflag FLAG_RECEIVED_HM_FLY
	setflag FLAG_RECEIVED_HM_SURF
	setflag FLAG_RECEIVED_HM_STRENGTH
	setflag FLAG_RECEIVED_HM_FLASH
	setflag FLAG_RECEIVED_HM_ROCK_SMASH
	setflag FLAG_RECEIVED_HM_WATERFALL
	setflag FLAG_RECEIVED_HM_DIVE

	@ Key items for getting around. The Infinite Repel starts switched on.
	additem ITEM_MACH_BIKE
	additem ITEM_ACRO_BIKE
	setflag FLAG_RECEIVED_BIKE
	additem ITEM_SUPER_ROD
	setflag FLAG_RECEIVED_SUPER_ROD
	additem ITEM_GO_GOGGLES
	setflag FLAG_RECEIVED_GO_GOGGLES
	additem ITEM_DEVON_SCOPE
	additem ITEM_DOWSING_MACHINE
	additem ITEM_INFINITE_REPEL
	setflag FLAG_INFINITE_REPEL_ON
	additem ITEM_ENDLESS_CANDY

	@ Other supplies: Heart Scales for the Pokémon Center services, some balls and healing.
	additem ITEM_HEART_SCALE, 99
	additem ITEM_POKE_BALL, 50
	additem ITEM_ULTRA_BALL, 50
	additem ITEM_FULL_RESTORE, 20
	additem ITEM_MAX_REVIVE, 20
	additem ITEM_ESCAPE_ROPE, 10
	special Playtest_SetMaxMoney

	@ A level 100 team (the starters and the Johto starters, fully evolved).
	givemon SPECIES_VENUSAUR, 100
	givemon SPECIES_CHARIZARD, 100
	givemon SPECIES_BLASTOISE, 100
	givemon SPECIES_MEGANIUM, 100
	givemon SPECIES_TYPHLOSION, 100
	givemon SPECIES_FERALIGATR, 100
	return

@ Debug menu: Utilities > Playtest setup.
Debug_EventScript_PlaytestSetup::
	call Playtest_EventScript_Setup
	msgbox Debug_Text_PlaytestSetupDone, MSGBOX_DEFAULT
	releaseall
	end

Debug_Text_PlaytestSetupDone:
	.string "Playtest setup done: all badges and\n"
	.string "HMs, every trainer beaten, and a\l"
	.string "level 100 team.\p"
	.string "People and objects update after\n"
	.string "you leave this area.$"
