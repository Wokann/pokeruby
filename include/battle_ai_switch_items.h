#ifndef GUARD_BATTLE_AI_SWITCH_ITEMS_H
#define GUARD_BATTLE_AI_SWITCH_ITEMS_H

#define AI_ITEM_FULL_RESTORE     1
#define AI_ITEM_HEAL_HP          2
#define AI_ITEM_CURE_CONDITION   3
#define AI_ITEM_X_STAT           4
#define AI_ITEM_GUARD_SPEC       5
#define AI_ITEM_NOT_RECOGNIZABLE 6

void AI_TrySwitchOrUseItem(void);
u8 GetMostSuitableMonToSwitchInto(void);

#endif // GUARD_BATTLE_AI_SWITCH_ITEMS_H
