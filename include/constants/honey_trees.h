#ifndef GUARD_CONSTANTS_HONEY_TREES_H
#define GUARD_CONSTANTS_HONEY_TREES_H

// Pokémon Emerald Champions: honey tree encounter tables.
// A tree's script puts one of these in VAR_0x8008 before it goes to EventScript_ShakeHoneyTree
// (data/scripts/honey_trees.inc). Every tree uses the shared table for now. To give an area
// its own table, add an ID here, copy the shared table in src/honey_trees.c, and use the new
// ID in that area's tree script.
#define HONEY_TREE_TABLE_SHARED 0
#define HONEY_TREE_TABLE_COUNT  1

// The Pokémon from a honey tree is this many levels below the current level cap.
#define HONEY_TREE_LEVELS_BELOW_CAP 2

#endif // GUARD_CONSTANTS_HONEY_TREES_H
