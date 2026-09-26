#ifndef GUARD_PC_SCREEN_EFFECT_H
#define GUARD_PC_SCREEN_EFFECT_H

struct PCScreenEffectStruct
{
    /*0x00*/ u16 tileTag;
    /*0x02*/ u16 paletteTag;
    /*0x04*/ u16 spriteSpeed;
    /*0x06*/ u16 revealSpeed;
    /*0x08*/ u16 state;
    /*0x0A*/ u16 spritesFinished;
    /*0x0C*/ s16 revealRadius;
    /*0x10*/ u32 selectedPalettes;
};

void StartPCScreenOpenEffect(struct PCScreenEffectStruct *effect);
bool8 UpdatePCScreenOpenEffect(void);
void StartPCScreenCloseEffect(struct PCScreenEffectStruct *effect);
bool8 UpdatePCScreenCloseEffect(void);

#endif //GUARD_PC_SCREEN_EFFECT_H
