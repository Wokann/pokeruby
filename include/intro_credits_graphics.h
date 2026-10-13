#ifndef GUARD_INTRO_CREDITS_GRAPHICS_H
#define GUARD_INTRO_CREDITS_GRAPHICS_H

extern u16 gIntroCredits_MovingSceneryVBase;
extern s16 gIntroCredits_MovingSceneryVOffset;
extern s16 gIntroCredits_MovingSceneryState;

extern const struct CompressedSpriteSheet gSpriteSheet_CreditsBrendan[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsMay[];
extern const struct CompressedSpriteSheet gSpriteSheet_IntroBicycle[];
extern const struct CompressedSpriteSheet gIntro2LatiosSpriteSheet[];
extern const struct CompressedSpriteSheet gIntro2LatiasSpriteSheet[];
extern const struct SpritePalette gSpritePalettes_Credits[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalBrendan[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalMay[];

void LoadIntroPart2Graphics(u8 scenery);
void SetIntroPart2BgCnt(u8 scenery);
void LoadCreditsSceneGraphics(u8 scene);
void SetCreditsSceneBgCnt(u8 scene);
u8 CreateBicycleBgAnimationTask(u8 mode, u16 bg1Speed, u16 bg2Speed, u16 bg3Speed);
void CycleSceneryPalette(u8 mode);
u8 CreateIntroBrendanSprite(s16 x, s16 y);
u8 CreateIntroMaySprite(s16 x, s16 y);
u8 intro_create_latios_sprite(s16 x, s16 y);
u8 intro_create_latias_sprite(s16 x, s16 y);

#endif // GUARD_INTRO_CREDITS_GRAPHICS_H
