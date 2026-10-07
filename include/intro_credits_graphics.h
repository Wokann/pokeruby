#ifndef GUARD_INTRO_CREDITS_GRAPHICS_H
#define GUARD_INTRO_CREDITS_GRAPHICS_H

extern const struct CompressedSpriteSheet gSpriteSheet_IntroBrendan[];
extern const struct CompressedSpriteSheet gSpriteSheet_IntroMay[];
extern const struct CompressedSpriteSheet gSpriteSheet_IntroBicycle[];
extern const struct CompressedSpriteSheet gIntro2LatiosSpriteSheet[];
extern const struct CompressedSpriteSheet gIntro2LatiasSpriteSheet[];
extern const struct SpritePalette gIntro2SpritePalettes[];
extern const struct CompressedSpriteSheet gUnknown_08416E24[];
extern const struct CompressedSpriteSheet gUnknown_08416E34[];

void LoadIntroPart2Graphics(u8 scenery);
void SetIntroPart2BgCnt(u8 scenery);
void sub_8148CB0(u8);
void sub_8148E90(u8);
u8 CreateBicycleBgAnimationTask(u8 mode, u16 bg1Speed, u16 bg2Speed, u16 bg3Speed);
void sub_8148F3C(u8);
void CycleSceneryPalette(u8);
u8 CreateIntroBrendanSprite(s16 x, s16 y);
u8 CreateIntroMaySprite(s16 x, s16 y);
u8 intro_create_latios_sprite(s16 a, s16 b);
u8 intro_create_latias_sprite(s16 a, s16 b);

#endif // GUARD_INTRO_CREDITS_GRAPHICS_H
