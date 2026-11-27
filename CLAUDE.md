# Pokémon Emerald Champions: notes for Claude

A hardcore Emerald ROM hack in the style of Pokémon Run & Bun, built on RHH's pokeemerald-expansion (master after 1.17.2). The repo owner leads design and playtests in mGBA. Claude does most of the implementation. The owner is new to ROM hacking, so explain changes in plain words and get approval before big design changes.

## Design rules (approved)

- Difficulty matches Run & Bun. The player gets a bit more power (setup moves, customization), and fights are tuned around it.
- Keep the Emerald world and story feel. Routing forces mandatory trainers; optional trainers are tricky but easier. Use back-to-back battles and gauntlets.
- Gym order is Run & Bun's: Brawly, Roxanne, Wattson, Norman, Flannery, Winona, Tate & Liza, Juan. Splits follow Run & Bun's too (Brawly, Roxanne, Wattson, Norman, Fallarbor, Flannery, Winona, Mt. Pyre, Hideouts, Tate & Liza, Space Center/Seafloor, Juan, Victory Road), with two splits before Brawly: Aqua Grunt (ends at the first boss, the Aqua grunt in Petalburg Woods) and Museum (ends at the Slateport museum).
- Story before Brawly: the first boss is the Aqua grunt in Petalburg Woods (`TRAINER_GRUNT_PETALBURG_WOODS`, 3 Pokémon). The scene path there is one tile wide: Team Aqua steals the Devon Goods there, and the Devon researcher then blocks the way to Rustboro until Capt. Stern has them back. Mr. Briney only sails after that scene. His first trip passes Dewford and goes straight to Slateport (Route 109), and he won't sail from there until the museum is done. The museum is a back-to-back boss (two Aqua grunts, no healing between); the player gets the parts back, returns them to Stern, and Stern hands over the Letter for Steven. Then Briney sails to Dewford, but won't sail back to Petalburg until Brawly's badge. Brawly's gym is closed (a Black Belt stands in front of the door) until the Letter is delivered to Steven. Steven skips the PokéNav registration if the player has no PokéNav (he is registered silently).
- Still vanilla and in need of rework for this order: the Rustboro theft, Rusturf Tunnel and Mr. Stone's errands (the Devon Goods and the Letter are already done by then), and Birch's PokéNav registration on Route 110 (skipped when the player has no PokéNav). The Route 110 rival battle still triggers on the first pass.
- Level caps are hard (no EXP at the cap), listed in play order in `src/caps.c`. Decided: 12 at the first boss (the woods Aqua grunt), 16 at the Slateport museum, 20 at gym 1, 24 at gym 2, 98 for the Elite Four. The other values are a draft.
- 12 Rare Candies in the whole game, scarce or earned. They can go over the cap (`B_RARE_CANDY_CAP FALSE`): 12 = 6 Pokémon × 2 levels, so saving all of them gives a level 100 team for the E4.
- No grinding: the Endless Candy (`ITEM_ENDLESS_CANDY`, key item, Birch gives it with the starter) raises a level by one per use, stops at the cap, and is never used up.
- Heart Scale NPC at Pokémon Centers (move relearning, IV maxing, nature changes), with more Heart Scales than Run & Bun. They are finite rewards, never farmable.
- Gimmicks: Megas only. Never give the player or any trainer Z-Moves, Dynamax or Tera.
- No EVs anywhere (`EV_CAP_NO_GAIN` plus `B_EV_ITEMS_CAP TRUE`). Trainer parties use no EVs.
- No weather or terrain for the player: no setter abilities on catchable Pokémon, and setter moves are removed from what the player's Pokémon can learn. Enemies may use them, and some fights start with a permanent field effect (`Starting Status` in `trainers.party`).
- Affection effects are off (`B_AFFECTION_MECHANICS FALSE`): random survival and crits break planning.
- Summary screen (`include/config/summary_screen.h`, `contest.h`): IVs shown as numbers (press L on the stats page), no contest moves page, and no free move relearner there, since relearning costs a Heart Scale.
- Trainer AI follows Run & Bun-style rules that players can read and plan around. Document every rule.
- A deathless run must stay possible but very hard: every boss keeps at least one deathless line.
- Keep Run & Bun's ratio of doubles to singles.
- New content goes in two zones: gyms 1–3 (especially between gyms 1 and 2) and a new section between gym 8 and Victory Road. Later-generation characters appear as bosses. Use Emerald's own tilesets so new areas look native.
- Full player documentation ("open book"): every trainer, encounter and item.
- Battle style is locked to Set, and the bag can't be used in trainer battles (Run & Bun rules; `IsAllowedToUseBag` in `src/battle_util.c`).
- No wild battles while the party is empty (`ShouldDisableRandomEncounters` in `src/field_control_avatar.c`), as a safety net before the starter.
- Early encounters: Littleroot is water only (a small pond). The grass is in the Little Grotto south of town (`LittlerootTown_Grotto`, its own area), which twins block until the player has Poké Balls (`FLAG_ADVENTURE_STARTED`). Slateport's Battle Tent lobby is its own area too (`MAPSEC_SLATEPORT_BATTLE_TENT`): a Pokéfan there gives Eevee (level 10, 3 perfect IVs) with a random Fire, Water or Thunder Stone. Professor Birch gives the Infinite Repel (`ITEM_INFINITE_REPEL`, key item that toggles `FLAG_INFINITE_REPEL_ON`; it works like a Repel that never wears off) and the Endless Candy with the starter. The Super Rod comes from a fisherman on the rock at Route 103's west shore (`Route103_EventScript_SuperRodFisherman`, `FLAG_RECEIVED_SUPER_ROD`). The first Poké Balls (10) come from the rival after the Route 103 battle. The Oldale footprints man gives the Johto starter of the third type at level 5 (Totodile, Chikorita or Cyndaquil). The starter and the Oldale gift get 3 random perfect IVs (the `SetGiftMonPerfectIVs` special does it for gifts, right after `givemon`). Early water areas have placeholder Super Rod tables; later ones still have vanilla late-game levels.
- Route 103 uses Run & Bun's layout (credited in `README.md`): a rock with stairs on each shore and Acro Bike rails across the water between them, so the Acro Bike gives a shortcut from Oldale to the east side and Route 110. The east-side trainers stand where Run & Bun puts them, Fisherman Andrew guards the east rock, and the two swimmers only battle when talked to.
- Mandatory trainers follow the route script; `/home/claude/scripts/mandatory_report.py` checks that a trainer can't be walked around. Route 102: Calvin (south path) and Allen (northwest gap); Rick and Tiana are optional. Route 104 south uses Run & Bun's trees that close the meadow, so the way to the woods runs Billy (foot of the beach stairs), Darian (beach), Cindy (path to the woods). Route 109's beach uses Run & Bun's rows of beach chairs: Chandler and Lola (the 2 solos), Huey and Edmond side by side (the double), Ricky and Hailey (sight lines cross at (22,7), so a careless step gives a double battle). Route 106: Elliot on the beach and Ned beside Run & Bun's boulders guard the way to Granite Cave. Granite Cave 1F: the new Hiker Silas (`TRAINER_SILAS`, ID 855) guards the passage to the ladder. Dewford Gym uses Run & Bun's maze, and its trainers see in every direction (`TRAINER_TYPE_SEE_ALL_DIRECTIONS`, as in Run & Bun's gyms): Brenden and Lilith, then Cristian and Takao, are the two doubles (their sight lines meet), Laura and Jocelyn the solos. Roxanne split: James guards the west passage of the woods past the old block; on Route 104 north Winston, Gina & Mia (the true double, on the bridge) and Haley are mandatory, Ivan is optional. The Trainers tab's Role column says Mandatory or Optional.
- Honey trees: each one is a one-time wild encounter and counts as its own area. A wooden HONEY TREE sign stands next to each tree: Route 102 (the lone tree south of the path), Petalburg Woods (the tree above the sign near the south entrance), Route 110 (a lone tree by the Slateport sign, reachable before the museum) and Route 106 (the tall tree on the beach). The player shakes the tree (YES/NO); the Pokémon comes from a weighted table at the current level cap minus 2, so a tree shaken later gives a stronger Pokémon. All trees share one table (Combee 20, Burmy 20, Cherubi 15, Aipom 15, Wurmple 15, Heracross 8, Munchlax 7, in percent); an area can get its own copy later. No Honey item is needed.
- Starters are Bulbasaur, Charmander and Squirtle (`src/starter_choose.c`). The rival uses the Gen 5 starters and takes the one that beats the player's: Tepig against Bulbasaur, Oshawott against Charmander, Snivy against Squirtle. The Route 103 rival has a second Pokémon, the Gen 3 starter that beats the Oldale gift (Treecko, Torchic or Mudkip). The rival trainer IDs keep their vanilla names, which describe the player's pick: `_TREECKO` means the player chose Bulbasaur, `_TORCHIC` means Charmander, `_MUDKIP` means Squirtle.

## Where things live

- Design source for trainers and encounters: the owner's Google Sheet "Emerald Champions Master Sheet" (Trainers and Encounters tabs, one row per Pokémon). Its ID column names the `trainers.party` entry each row belongs to; one row can name several IDs that share a team (May and Brendan rival variants). The sheet is the source of truth for designed trainers: sync it into `trainers.party` rather than hand-editing those entries.
- The owner's route script (draft): the "Route script" tab of the plan doc (ROM Hack Plan). It sets the order, encounters, trainer counts and caps for each split.
- Trainer teams: `src/data/trainers.party` (Showdown export format, documented at the top of the file)
- New trainers get IDs at the end of `include/constants/opponents.h` (bump `TRAINERS_COUNT_EMERALD`). Only 8 free trainer IDs remain before trainer flags run out (`MAX_TRAINERS_COUNT_EMERALD` 864), so bigger batches need unused vanilla trainers reused or the flag space expanded.
- Wild encounters: `src/data/wild_encounters.json`; honey trees: tables in `src/honey_trees.c`, table IDs in `include/constants/honey_trees.h`, shared script and how to add a tree in `data/scripts/honey_trees.inc`, flags `FLAG_HONEY_TREE_*`
- Level caps: `src/caps.c`; cap and EV settings: `include/config/caps.h`
- Battle settings: `include/config/battle.h`; AI flags: `include/constants/battle_ai.h`
- Items: `include/constants/items.h`, `src/data/items.h`; Rare and Endless Candy use: `ItemUseCB_RareCandy` in `src/party_menu.c`
- New game setup: `src/new_game.c`
- Maps and event scripts: `data/maps/<Map>/` (`map.json`, `scripts.inc`). The owner edits map layouts in Porymap. A new map also needs its `scripts.inc` included in `data/event_scripts.s`, its name appended to a group in `data/maps/map_groups.json` (append, so existing map numbers don't shift), its layout in `data/layouts/layouts.json`, and, for a new area name, an entry appended to `src/data/region_map/region_map_sections.json` (16 characters at most).
- Battle and AI tests: `test/battle/` (for example `test/battle/ai/`)

## Build

On Ubuntu (the cloud workspace):

```
sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libpng-dev
make -j$(nproc)
```

A clean build takes about 8 minutes on 2 cores and produces `pokeemerald.gba`. `make check` runs the test suite (about 6,500 tests, slow on 2 cores).

## Workflow

- Work on a branch, push it, and open a pull request. The owner has approved Claude merging its own pull requests once the change is in a test build, so merge as you go and say so.
- After a build, give the owner the ROM to playtest: attach it in chat, and copy it into the build folder on their PC when the desktop app has that folder connected.
- Never commit ROMs or saves (`*.gba` and `*.sav` are gitignored). Releases ship as `.bps` patches.
- Keep the RHH credit line in `README.md`.
- Mark every hack-specific change in engine code with a short `Pokémon Emerald Champions` comment, so upstream merges stay easy.
