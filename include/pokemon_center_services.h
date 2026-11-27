#ifndef GUARD_POKEMON_CENTER_SERVICES_H
#define GUARD_POKEMON_CENTER_SERVICES_H

// Pokémon Emerald Champions: specials for the Pokémon Center services attendant.
u16 PcServices_GetIV(void);
void PcServices_MaxIV(void);
u16 PcServices_GetNature(void);
void PcServices_BufferNatureName(void);
void PcServices_SetNature(void);
u16 PcServices_CheckStatus(void);
void PcServices_GiveStatus(void);

#endif // GUARD_POKEMON_CENTER_SERVICES_H
