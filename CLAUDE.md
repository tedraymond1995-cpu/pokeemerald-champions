# Pokémon Emerald Champions: notes for Claude

A hardcore Emerald ROM hack in the style of Pokémon Run & Bun, built on RHH's pokeemerald-expansion (master after 1.17.2). The repo owner leads design and playtests in mGBA. Claude does most of the implementation. The owner is new to ROM hacking, so explain changes in plain words and get approval before big design changes.

## Design rules (approved)

- Difficulty matches Run & Bun. The player gets a bit more power (setup moves, customization), and fights are tuned around it.
- Keep the Emerald world and story feel. Routing forces mandatory trainers; optional trainers are tricky but easier. Use back-to-back battles and gauntlets.
- Gym order is Run & Bun's: Brawly, Roxanne, Wattson, Norman, Flannery, Winona, Tate & Liza, Juan. Splits follow Run & Bun's too (Brawly, Roxanne, Wattson, Norman, Fallarbor, Flannery, Winona, Mt. Pyre, Hideouts, Tate & Liza, Space Center/Seafloor, Juan, Victory Road).
- Level caps are hard (no EXP at the cap), listed in play order in `src/caps.c`. Decided: 15 at gym 1, 22 at gym 2, 98 for the Elite Four. The other values are a draft.
- 12 Rare Candies in the whole game, scarce or earned. They can go over the cap (`B_RARE_CANDY_CAP FALSE`): 12 = 6 Pokémon × 2 levels, so saving all of them gives a level 100 team for the E4.
- No grinding: the Endless Candy (`ITEM_ENDLESS_CANDY`, key item, given at new game) raises a level by one per use, stops at the cap, and is never used up.
- Heart Scale NPC at Pokémon Centers (move relearning, IV maxing, nature changes), with more Heart Scales than Run & Bun. They are finite rewards, never farmable.
- Gimmicks: Megas only. Never give the player or any trainer Z-Moves, Dynamax or Tera.
- No EVs anywhere (`EV_CAP_NO_GAIN` plus `B_EV_ITEMS_CAP TRUE`). Trainer parties use no EVs.
- No weather or terrain for the player: no setter abilities on catchable Pokémon, and setter moves are removed from what the player's Pokémon can learn. Enemies may use them, and some fights start with a permanent field effect (`Starting Status` in `trainers.party`).
- Affection effects are off (`B_AFFECTION_MECHANICS FALSE`): random survival and crits break planning.
- Trainer AI follows Run & Bun-style rules that players can read and plan around. Document every rule.
- A deathless run must stay possible but very hard: every boss keeps at least one deathless line.
- Keep Run & Bun's ratio of doubles to singles.
- New content goes in two zones: gyms 1–3 (especially between gyms 1 and 2) and a new section between gym 8 and Victory Road. Later-generation characters appear as bosses. Use Emerald's own tilesets so new areas look native.
- Full player documentation ("open book"): every trainer, encounter and item.

## Where things live

- Design source for trainers and encounters: the owner's Google Sheet "Emerald Champions Master Sheet" (Trainers and Encounters tabs, one row per Pokémon). Its ID column names the `trainers.party` entry each row belongs to; one row can name several IDs that share a team (May and Brendan rival variants). The sheet is the source of truth for designed trainers: sync it into `trainers.party` rather than hand-editing those entries.
- Trainer teams: `src/data/trainers.party` (Showdown export format, documented at the top of the file)
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

- Work on a branch, push it, and open a pull request for the owner to merge.
- After a build, give the owner the ROM to playtest: attach it in chat, and copy it into the build folder on their PC when the desktop app has that folder connected.
- Never commit ROMs or saves (`*.gba` and `*.sav` are gitignored). Releases ship as `.bps` patches.
- Keep the RHH credit line in `README.md`.
- Mark every hack-specific change in engine code with a short `Pokémon Emerald Champions` comment, so upstream merges stay easy.
