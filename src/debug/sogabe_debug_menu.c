#include "global.h"
#include "battle_tower.h"
#include "menu.h"
#include "constants/battle_tower.h"

extern u8 gBattleCommunication[];

extern u8 (*gMenuCallback)(void);

static u8 SogabeDebugMenu_SetRound(void);
static u8 SogabeDebugMenu_HandleInput(void);

static const u8 sText_BattleTowerRound1[] = _("1st round");
static const u8 sText_BattleTowerRound2[] = _("2nd round");
static const u8 sText_BattleTowerRound3[] = _("3rd round");
static const u8 sText_BattleTowerRound4[] = _("4th round");
static const u8 sText_BattleTowerRound5[] = _("5th round");
static const u8 sText_BattleTowerRound6[] = _("6th round");
static const u8 sText_BattleTowerRound7[] = _("7th round");
static const u8 sText_BattleTowerRound8[] = _("8th round");


static const struct MenuAction sSogabeBattleTowerRoundMenuActions[] = {
    {sText_BattleTowerRound1, SogabeDebugMenu_SetRound},
    {sText_BattleTowerRound2, SogabeDebugMenu_SetRound},
    {sText_BattleTowerRound3, SogabeDebugMenu_SetRound},
    {sText_BattleTowerRound4, SogabeDebugMenu_SetRound},
    {sText_BattleTowerRound5, SogabeDebugMenu_SetRound},
    {sText_BattleTowerRound6, SogabeDebugMenu_SetRound},
    {sText_BattleTowerRound7, SogabeDebugMenu_SetRound},
    {sText_BattleTowerRound8, SogabeDebugMenu_SetRound}
};

u8 InitSogabeDebugMenu(void)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 16, 18);
    Menu_PrintItems(2, 1, 8, sSogabeBattleTowerRoundMenuActions);
    InitMenu(0, 1, 1, 8, 0, 15);
    gMenuCallback = SogabeDebugMenu_HandleInput;
    return FALSE;
}

static u8 SogabeDebugMenu_HandleInput(void)
{
    s8 result = Menu_ProcessInput();
    if (result == MENU_NOTHING_CHOSEN)
    {
        return FALSE;
    }
    else if (result == MENU_B_PRESSED)
    {
        CloseMenu();
        return TRUE;
    }
    else
    {
        gBattleCommunication[0] = result;
        gMenuCallback = sSogabeBattleTowerRoundMenuActions[result].func;
        return FALSE;
    }
}

static u8 SogabeDebugMenu_SetRound(void)
{
    gSaveBlock2.battleTower.challengeStatus[BATTLE_TOWER_LEVEL_50] = BATTLE_TOWER_CHALLENGE_STATUS_PRIZE_COLLECTED;
    gSaveBlock2.battleTower.challengeStatus[BATTLE_TOWER_LEVEL_100] = BATTLE_TOWER_CHALLENGE_STATUS_PRIZE_COLLECTED;
    gSaveBlock2.battleTower.curStreakChallengesNum[BATTLE_TOWER_LEVEL_50] = gBattleCommunication[0] + BATTLE_TOWER_FIRST_CHALLENGE_NUM;
    gSaveBlock2.battleTower.curStreakChallengesNum[BATTLE_TOWER_LEVEL_100] = gBattleCommunication[0] + BATTLE_TOWER_FIRST_CHALLENGE_NUM;
    gSaveBlock2.battleTower.curChallengeBattleNum[BATTLE_TOWER_LEVEL_50] = BATTLE_TOWER_FIRST_CHALLENGE_NUM;
    gSaveBlock2.battleTower.curChallengeBattleNum[BATTLE_TOWER_LEVEL_100] = BATTLE_TOWER_FIRST_CHALLENGE_NUM;
    CloseMenu();
    return TRUE;
}
