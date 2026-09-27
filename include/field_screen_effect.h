#ifndef GUARD_FIELD_SCREEN_EFFECT_H
#define GUARD_FIELD_SCREEN_EFFECT_H

void AnimateFlash(u8);
extern const u16 gOrbEffectBackgroundLayerFlags[];
void DoOrbEffect(void);
void FadeOutOrbEffect(void);
void WriteFlashScanlineEffectBuffer(u8 val);

#endif // GUARD_FIELD_SCREEN_EFFECT_H
