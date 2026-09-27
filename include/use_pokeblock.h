//
// Created by Scott Norton on 5/31/17.
//

#ifndef POKERUBY_USE_POKEBLOCK_H
#define POKERUBY_USE_POKEBLOCK_H

extern void *gUnknown_02030400;
extern s16 gUnknown_02039312;

void ChooseMonToGivePokeblock(struct Pokeblock *, MainCallback);
u8 GetPartyIdFromPokeblockSelection(u8);

#endif //POKERUBY_USE_POKEBLOCK_H
