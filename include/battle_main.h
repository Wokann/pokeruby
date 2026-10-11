#ifndef GUARD_BATTLE_MAIN_H
#define GUARD_BATTLE_MAIN_H

struct TrainerMoney
{
    u8 trainerClass;
    u8 baseMoney;
};

struct Sprite;

// defines for the u8 array gTypeEffectiveness
#define TYPE_EFFECT_ATK_TYPE(i) ((gTypeEffectiveness[i + 0]))
#define TYPE_EFFECT_DEF_TYPE(i) ((gTypeEffectiveness[i + 1]))
#define TYPE_EFFECT_MULTIPLIER(i) ((gTypeEffectiveness[i + 2]))

// defines for the gTypeEffectiveness multipliers
#define TYPE_MUL_NO_EFFECT          0
#define TYPE_MUL_NOT_EFFECTIVE      5
#define TYPE_MUL_NORMAL             10
#define TYPE_MUL_SUPER_EFFECTIVE    20

// special type table Ids
#define TYPE_FORESIGHT  0xFE
#define TYPE_ENDTABLE   0xFF

void SpriteCB_VsLetterDummy(struct Sprite *sprite);
void SpriteCB_VsLetterInit(struct Sprite *sprite);
void SpriteCallbackDummy_2(struct Sprite *sprite);
void SpriteCB_ShowAsMoveTarget(struct Sprite *sprite);
void SpriteCB_BattleSpriteStartSlideLeft(struct Sprite *sprite);
void SetIdleSpriteCallback(struct Sprite *sprite);
void BeginBattleIntroDummy(void);
void BeginBattleIntro(void);

void HBlankCB_Battle(void);
void VBlankCB_Battle(void);

extern const struct OamData gOamData_BattleSpritePlayerSide;
extern const struct OamData gOamData_BattleSpriteOpponentSide;
extern const struct TrainerMoney gTrainerMoneyTable[];
extern const u8 *const gAbilityDescriptionPointers[];

#endif // GUARD_BATTLE_MAIN_H
