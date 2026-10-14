#ifndef GUARD_BATTLE_INTERFACE_H
#define GUARD_BATTLE_INTERFACE_H

#include "battle_controllers.h"  // for struct HpAndStatus

struct BattleInterfaceStruct1
{
    s32 maxValue;
    s32 oldValue;
    s32 receivedValue;
    u32 unkC_0:5;
    u32 unk10;
};

enum
{
    HP_CURRENT,
    HP_MAX
};

enum
{
    HEALTH_BAR,
    EXP_BAR
};

enum
{
    HEALTHBOX_ALL,
    HEALTHBOX_CURRENT_HP,
    HEALTHBOX_MAX_HP,
    HEALTHBOX_LEVEL,
    HEALTHBOX_NICK,
    HEALTHBOX_HEALTH_BAR,
    HEALTHBOX_EXP_BAR,
    HEALTHBOX_UNUSED_7,
    HEALTHBOX_UNUSED_8,
    HEALTHBOX_STATUS_ICON,
    HEALTHBOX_SAFARI_ALL_TEXT,
    HEALTHBOX_SAFARI_BALLS_TEXT
};

u8 CreateBattlerHealthboxSprites(u8 battler);
u8 CreateSafariPlayerHealthboxSprites(void);
void SetBattleBarStruct(u8 battler, u8 healthboxSpriteId, u32 maxVal, u32 oldVal, u32 receivedValue);
void SetHealthboxSpriteInvisible(u8 healthboxSpriteId);
void SetHealthboxSpriteVisible(u8 healthboxSpriteId);
void DestoryHealthboxSprite(u8 healthboxSpriteId);
void DummyBattleInterfaceFunc(u8 healthboxSpriteId, bool8 isDoubleBattleBattlerOnly);
void UpdateOamPriorityInAllHealthboxes(u8 priority);
void SwapHpBarsWithHpText(void);
u8 CreatePartyStatusSummarySprites(u8 battler, const struct HpAndStatus *partyInfo, bool8 skipPlayer, bool8 isBattleStart);
void Task_HidePartyStatusSummary(u8 taskId);
void UpdateHealthboxAttribute(u8 healthboxSpriteId, struct Pokemon *mon, u8 elementId);
s32 MoveBattleBar(u8 battler, u8 healthboxSpriteId, u8 whichBar, u8 unused);
s16 sub_80460C8(struct BattleInterfaceStruct1 *barInfo, int *currValue, u16 *dest, int unused);
u8 GetScaledHPFraction(s16 hp, s16 maxhp, u8 scale);
u8 GetHPBarLevel(s16 hp, s16 maxhp);
void UpdateHpTextInHealthbox(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent);

void InitBattlerHealthboxCoords(u8 battler);

#endif  // GUARD_BATTLE_INTERFACE_H
