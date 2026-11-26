"""Pokémon Emerald Champions: make weather and terrain setters trainer-only.

Usage: python3 -I strip_weather.py REPO_DIR

1. Removes weather and terrain moves from every level-up learnset and egg-move list.
2. Moves weather/terrain-setting abilities out of the normal ability slots (0 and 1)
   into the hidden slot (2), swapping in the old hidden ability. Wild Pokémon never
   get slot 2, and the Ability Patch refuses setter abilities, so only trainers
   (who name the ability in trainers.party) can use them.
   Mega, Primal and Gigantamax forms are left alone; their items stay trainer-only.
   Species with no non-setter ability to swap in are reported and left alone.
"""
import pathlib
import re
import sys

WEATHER_TERRAIN_MOVES = [
    "MOVE_RAIN_DANCE", "MOVE_SUNNY_DAY", "MOVE_SANDSTORM", "MOVE_HAIL",
    "MOVE_SNOWSCAPE", "MOVE_CHILLY_RECEPTION",
    "MOVE_ELECTRIC_TERRAIN", "MOVE_GRASSY_TERRAIN",
    "MOVE_MISTY_TERRAIN", "MOVE_PSYCHIC_TERRAIN",
]
SETTER_ABILITIES = {
    "ABILITY_DRIZZLE", "ABILITY_DROUGHT", "ABILITY_SAND_STREAM", "ABILITY_SNOW_WARNING",
    "ABILITY_SAND_SPIT", "ABILITY_PRIMORDIAL_SEA", "ABILITY_DESOLATE_LAND",
    "ABILITY_DELTA_STREAM", "ABILITY_ELECTRIC_SURGE", "ABILITY_GRASSY_SURGE",
    "ABILITY_MISTY_SURGE", "ABILITY_PSYCHIC_SURGE", "ABILITY_SEED_SOWER",
    "ABILITY_ORICHALCUM_PULSE", "ABILITY_HADRON_ENGINE",
}
SKIP_FORM_MARKERS = ("_MEGA", "_PRIMAL", "_GMAX")

moves_alt = "|".join(WEATHER_TERRAIN_MOVES)
LEVEL_UP_LINE = re.compile(r"^\s*LEVEL_UP_MOVE\(\s*\d+\s*,\s*(" + moves_alt + r")\s*\),\s*$")
EGG_LINE = re.compile(r"^\s*(" + moves_alt + r")\s*,\s*$")
ABILITIES_LINE = re.compile(r"^(?P<indent>\s*)\.abilities = \{\s*(?P<body>[^}]*)\},(?P<rest>.*)$")
SPECIES_LINE = re.compile(r"\[(SPECIES_\w+)\]\s*=")


def strip_lines(path: pathlib.Path, pattern: re.Pattern) -> int:
    lines = path.read_text().splitlines(keepends=True)
    kept = [line for line in lines if not pattern.match(line)]
    removed = len(lines) - len(kept)
    if removed:
        path.write_text("".join(kept))
    return removed


def fix_abilities(path: pathlib.Path, changed: list, untouched: list) -> None:
    lines = path.read_text().splitlines(keepends=True)
    species = None
    for i, line in enumerate(lines):
        m = SPECIES_LINE.search(line)
        if m:
            species = m.group(1)
        m = ABILITIES_LINE.match(line.rstrip("\n"))
        if not m:
            continue
        slots = [s.strip() for s in m.group("body").split(",") if s.strip()]
        if not any(s in SETTER_ABILITIES for s in slots):
            continue
        name = species or "?"
        if any(marker in name for marker in SKIP_FORM_MARKERS):
            untouched.append((name, slots, "battle form; keep its item trainer-only"))
            continue
        while len(slots) < 3:
            slots.append("ABILITY_NONE")
        normal = [idx for idx in (0, 1) if slots[idx] in SETTER_ABILITIES]
        if not normal:
            continue  # setter already only in the hidden slot
        hidden = slots[2]
        if hidden == "ABILITY_NONE" or hidden in SETTER_ABILITIES or len(normal) == 2:
            untouched.append((name, slots, "no non-setter ability to swap in"))
            continue
        new = list(slots)
        setter_idx = normal[0]
        new[setter_idx], new[2] = hidden, slots[setter_idx]
        lines[i] = f"{m.group('indent')}.abilities = {{ {', '.join(new)} }},{m.group('rest')}\n"
        changed.append((name, slots, new))
    path.write_text("".join(lines))


def main() -> None:
    repo = pathlib.Path(sys.argv[1])
    pokemon = repo / "src" / "data" / "pokemon"

    for path in sorted((pokemon / "level_up_learnsets").glob("gen_*.h")):
        print(f"{path.name}: removed {strip_lines(path, LEVEL_UP_LINE)} level-up entries")
    print(f"egg_moves.h: removed {strip_lines(pokemon / 'egg_moves.h', EGG_LINE)} egg moves")

    changed, untouched = [], []
    for path in sorted((pokemon / "species_info").glob("gen_*_families.h")):
        fix_abilities(path, changed, untouched)
    print(f"\nAbility slots changed ({len(changed)}):")
    for name, old, new in changed:
        print(f"  {name}: {old} -> {new}")
    print(f"\nLeft as is ({len(untouched)}):")
    for name, slots, why in untouched:
        print(f"  {name}: {slots} ({why})")


if __name__ == "__main__":
    main()
