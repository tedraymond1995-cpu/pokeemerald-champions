# Pokémon Emerald Champions: notes for Claude

A hardcore Emerald ROM hack in the style of Pokémon Run & Bun, built on RHH's pokeemerald-expansion (master after 1.17.2). The repo owner leads design and playtests in mGBA. Claude does most of the implementation. The owner is new to ROM hacking, so explain changes in plain words and get approval before big design changes.

## Design rules (approved)

- Difficulty matches Run & Bun. The player gets a bit more power (setup moves, customization), and fights are tuned around it.
- Keep the Emerald world and story feel. Routing forces mandatory trainers; optional trainers are tricky but easier. Use back-to-back battles and gauntlets.
- Gym order is Run & Bun's: Brawly, Roxanne, Wattson, Norman, Flannery, Winona, Tate & Liza, Juan. Splits follow Run & Bun's too (Brawly, Roxanne, Wattson, Norman, Fallarbor, Flannery, Winona, Mt. Pyre, Hideouts, Tate & Liza, Space Center/Seafloor, Juan, Victory Road), plus a first split that ends at the Route 104 boss.
- Story before Brawly: the boss in front of Mr. Briney's cottage (`TRAINER_ROUTE104_BOSS`) blocks the door until beaten. In Petalburg Woods the scene path is one tile wide: Team Aqua steals the Devon Goods there, and the Devon researcher then blocks the way to Rustboro until Capt. Stern has them back. Mr. Briney only sails after that scene. His first trip passes Dewford and goes straight to Slateport (Route 109), and he won't sail from there until the museum is done. The museum is a back-to-back boss (two Aqua grunts, no healing between); the player gets the parts back, returns them to Stern, and Stern hands over the Letter for Steven. Then Briney sails to Dewford, but won't sail back to Petalburg until Brawly's badge. Steven skips the PokéNav registration if the player has no PokéNav (he is registered silently).
- Still vanilla and in need of rework for this order: the Rustboro theft, Rusturf Tunnel and Mr. Stone's errands (the Devon Goods and the Letter are already done by then), and Birch's PokéNav registration on Route 110 (skipped when the player has no PokéNav). The Route 110 rival battle still triggers on the first pass.
- Level caps are hard (no EXP at the cap), listed in play order in `src/caps.c`. Decided: 12 at the first boss (the Route 104 boss for now), 16 at the Slateport museum, 20 at gym 1, 24 at gym 2, 98 for the Elite Four. The other values are a draft.
- 12 Rare Candies in the whole game, scarce or earned. They can go over the cap (`B_RARE_CANDY_CAP FALSE`): 12 = 6 Pokémon × 2 levels, so saving all of them gives a level 100 team for the E4.
- No grinding: the Endless Candy (`ITEM_ENDLESS_CANDY`, key item, given at new game) raises a level by one per use, stops at the cap, and is never used up.
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
- No wild battles while the party is empty (`ShouldDisableRandomEncounters` in `src/field_control_avatar.c`), since Littleroot has grass before the starter.
- Early encounters: Littleroot has a grass patch, Professor Birch gives 10 Poké Balls, 10 Repels and the Super Rod with the starter, and the Oldale footprints man gives the Johto starter of the third type at level 5 (Totodile, Chikorita or Cyndaquil). Early water areas have placeholder Super Rod tables; later ones still have vanilla late-game levels.
- Starters are Bulbasaur, Charmander and Squirtle (`src/starter_choose.c`). The rival uses the Gen 5 starters and takes the one that beats the player's: Tepig against Bulbasaur, Oshawott against Charmander, Snivy against Squirtle. The rival trainer IDs keep their vanilla names, which describe the player's pick: `_TREECKO` means the player chose Bulbasaur, `_TORCHIC` means Charmander, `_MUDKIP` means Squirtle.

## Where things live

- Design source for trainers and encounters: the owner's Google Sheet "Emerald Champions Master Sheet" (Trainers and Encounters tabs, one row per Pokémon). Its ID column names the `trainers.party` entry each row belongs to; one row can name several IDs that share a team (May and Brendan rival variants). The sheet is the source of truth for designed trainers: sync it into `trainers.party` rather than hand-editing those entries.
- Trainer teams: `src/data/trainers.party` (Showdown export format, documented at the top of the file)
- New trainers get IDs at the end of `include/constants/opponents.h` (bump `TRAINERS_COUNT_EMERALD`). Only 8 free trainer IDs remain before trainer flags run out (`MAX_TRAINERS_COUNT_EMERALD` 864), so bigger batches need unused vanilla trainers reused or the flag space expanded.
- Wild encounters: `src/data/wild_encounters.json`
- Level caps: `src/caps.c`; cap and EV settings: `include/config/caps.h`
- Battle settings: `include/config/battle.h`; AI flags: `include/constants/battle_ai.h`
- Items: `include/constants/items.h`, `src/data/items.h`; Rare and Endless Candy use: `ItemUseCB_RareCandy` in `src/party_menu.c`
- New game setup: `src/new_game.c`
- Maps and event scripts: `data/maps/<Map>/` (`map.json`, `scripts.inc`). The owner edits map layouts in Porymap.
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
