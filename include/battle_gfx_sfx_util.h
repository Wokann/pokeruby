#ifndef GUARD_BATTLE_GFX_SFX_UTIL_H
#define GUARD_BATTLE_GFX_SFX_UTIL_H

struct Pokemon;
struct Sprite;

void SpriteCB_WaitForBattlerBallReleaseAnim(struct Sprite *sprite);
void SpriteCB_TrainerSlideIn(struct Sprite *sprite);
void InitAndLaunchChosenStatusAnimation(bool8 isStatus2, u32 status);
bool8 TryHandleLaunchBattleTableAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 tableId, u16 argument);
void InitAndLaunchSpecialAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 tableId);
bool8 IsMoveWithoutAnimation(int move, int animationTurn);
bool8 IsBattleSEPlaying(u8 battler);
void CopyAllBattleSpritesInvisibilities(void);
void CopyBattleSpriteInvisibility(u8 battler);
void TrySetBehindSubstituteSpriteBit(u8 battler, u16 move);
void ClearBehindSubstituteBit(u8 battler);
void HandleLowHpMusicChange(struct Pokemon *mon, u8 battler);
void BattleStopLowHpSound(void);
u8 GetMonHPBarLevel(struct Pokemon *mon);
void SetBattlerSpriteAffineMode(u8 affineMode);
void SpriteCB_SetInvisible(struct Sprite *sprite);
void SetBattlerShadowSpriteCallback(u8 battler, u16 species);
void HideBattlerShadowSprite(u8 battler);
void FillAroundBattleWindows(void);
void ClearTemporarySpeciesSpriteData(u8 battler, bool8 dontClearSubstitute);

void BattleLoadOpponentMonSpriteGfx(struct Pokemon *mon, u8 battler);
void BattleLoadPlayerMonSpriteGfx(struct Pokemon *mon, u8 battler);
void BattleGfxSfxDummy2(u16 species);
void DecompressTrainerFrontPic(u16 frontPicId, u8 battler);
void DecompressTrainerBackPic(u16 backPicId, u8 battler);
void BattleGfxSfxDummy3(int gender);
void BattleLoadAllHealthBoxesGfxAtOnce(void);
bool8 BattleLoadAllHealthBoxesGfx(u8 state);
void LoadBattleBarGfx(u8 unused);
bool8 BattleInitAllSprites(u8 *state1, u8 *battler);
void ClearSpritesHealthboxAnimData(void);
void HandleSpeciesGfxDataChange(u8 battlerAtk, u8 battlerDef, bool8 castform);
void BattleLoadSubstituteOrMonSpriteGfx(u8 battler, bool8 loadMonSprite);
void LoadBattleMonGfxAndAnimate(u8 battler, bool8 loadMonSprite, u8 spriteId);
void HandleBattleLowHpMusicChange(void);
void LoadAndCreateEnemyShadowSprites(void);

#endif // GUARD_BATTLE_GFX_SFX_UTIL_H
