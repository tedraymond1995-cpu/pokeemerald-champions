@ Pokémon Emerald Champions: honey trees.
@ Each honey tree is a one-time wild encounter that counts as its own area. A tree is a
@ sign bg_event on the tree's tiles that runs the tree's own script (in its map's
@ scripts.inc). That script checks and sets the tree's flag, puts the tree's encounter
@ table (constants/honey_trees.h) in VAR_0x8008 and goes to EventScript_ShakeHoneyTree.
@ StartHoneyTreeBattle (src/honey_trees.c) picks the Pokémon from the table by weight;
@ its level is the current level cap minus 2.
@ A HONEY TREE sign stands next to each tree and uses EventScript_HoneyTreeSign.
@ To add a tree: give it a flag, copy a tree script, and add the bg_events and the sign.

EventScript_HoneyTreeSign::
	msgbox Text_HoneyTreeSign, MSGBOX_SIGN
	end

EventScript_AskShakeHoneyTree::
	msgbox Text_HoneyTreeAskShake, MSGBOX_YESNO
	return

EventScript_HoneyTreeLeaveAlone::
	releaseall
	end

EventScript_HoneyTreeUsed::
	msgbox Text_HoneyTreeUsed, MSGBOX_DEFAULT
	releaseall
	end

EventScript_ShakeHoneyTree::
	closemessage
	setvar VAR_0x8004, 0  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 3  @ shake delay
	special ShakeCamera
	waitstate
	msgbox Text_HoneyTreePokemonFell, MSGBOX_DEFAULT
	closemessage
	special StartHoneyTreeBattle
	releaseall
	end

Text_HoneyTreeSign:
	.string "HONEY TREE\p"
	.string "POKéMON come to this tree for its\n"
	.string "sweet honey. Shake it to meet one.\p"
	.string "A tree can only be shaken once.\n"
	.string "The further your journey, the\l"
	.string "stronger the POKéMON that come.$"

Text_HoneyTreeAskShake:
	.string "A sweet smell drifts from this tree.\n"
	.string "Something is rustling in the leaves…\p"
	.string "Shake the tree?$"

Text_HoneyTreeUsed:
	.string "The sweet smell is gone.\n"
	.string "No more POKéMON will come here.$"

Text_HoneyTreePokemonFell:
	.string "A POKéMON tumbled out of the tree!$"
