#ifndef GUARD_POKEMON_ITEM_EFFECT_H
#define GUARD_POKEMON_ITEM_EFFECT_H

bool8 ExecuteTableBasedItemEffect(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex);
bool8 PokemonUseItemEffects(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex, bool8 usedByAI);

#endif // GUARD_POKEMON_ITEM_EFFECT_H
