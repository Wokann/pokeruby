#ifndef GUARD_INTRO_CREDITS_GRAPHICS_H
#define GUARD_INTRO_CREDITS_GRAPHICS_H

extern const struct CompressedSpriteSheet gIntro2BrendanSpriteSheet[];
extern const struct CompressedSpriteSheet gIntro2MaySpriteSheet[];
extern const struct CompressedSpriteSheet gIntro2BicycleSpriteSheet[];
extern const struct CompressedSpriteSheet gIntro2LatiosSpriteSheet[];
extern const struct CompressedSpriteSheet gIntro2LatiasSpriteSheet[];
extern const struct SpritePalette gIntro2SpritePalettes[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalBrendan[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalMay[];

void LoadIntroPart2Graphics(u8 a);
void SetIntroPart2BgCnt(u8 a);
void LoadCreditsSceneGraphics(u8);
void SetCreditsSceneBgCnt(u8);
u8 CreateBicycleBgAnimationTask(u8 a, u16 b, u16 c, u16 d);
void Task_BicycleBgAnimation(u8);
void CycleSceneryPalette(u8);
u8 CreateIntroBrendanSprite(s16 a, s16 b);
u8 CreateIntroMaySprite(s16 a, s16 b);
u8 CreateIntroLatiosSprite(s16 a, s16 b);
u8 CreateIntroLatiasSprite(s16 a, s16 b);

#endif // GUARD_INTRO_CREDITS_GRAPHICS_H
