#include "global.h"
#include "battle_tower.h"
#include "menu.h"

extern u8 gBattleCommunication[];

extern u8 (*gMenuCallback)(void);

static u8 SogabeDebugMenu_SetRound(void);
static u8 SogabeDebugMenu_HandleInput(void);

static const u8 sText_1stRound[] = _("1st round");
static const u8 sText_2ndRound[] = _("2nd round");
static const u8 sText_3rdRound[] = _("3rd round");
static const u8 sText_4thRound[] = _("4th round");
static const u8 sText_5thRound[] = _("5th round");
static const u8 sText_6thRound[] = _("6th round");
static const u8 sText_7thRound[] = _("7th round");
static const u8 sText_8thRound[] = _("8th round");


static const struct MenuAction sSogabeBattleTowerRoundMenuActions[] = {
    {sText_1stRound, SogabeDebugMenu_SetRound},
    {sText_2ndRound, SogabeDebugMenu_SetRound},
    {sText_3rdRound, SogabeDebugMenu_SetRound},
    {sText_4thRound, SogabeDebugMenu_SetRound},
    {sText_5thRound, SogabeDebugMenu_SetRound},
    {sText_6thRound, SogabeDebugMenu_SetRound},
    {sText_7thRound, SogabeDebugMenu_SetRound},
    {sText_8thRound, SogabeDebugMenu_SetRound}
};

int InitSogabeDebugMenu(void)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 16, 18);
    Menu_PrintItems(2, 1, 8, sSogabeBattleTowerRoundMenuActions);
    InitMenu(0, 1, 1, 8, 0, 15);
    gMenuCallback = SogabeDebugMenu_HandleInput;
    return 0;
}

static u8 SogabeDebugMenu_HandleInput(void)
{
    s8 result = Menu_ProcessInput();
    if (result == -2)
    {
        return 0;
    }
    else if (result == -1)
    {
        CloseMenu();
        return 1;
    }
    else
    {
        gBattleCommunication[0] = result;
        gMenuCallback = sSogabeBattleTowerRoundMenuActions[result].func;
        return 0;
    }
}

static u8 SogabeDebugMenu_SetRound(void)
{
    gSaveBlock2.battleTower.var_4AE[0] = 3;
    gSaveBlock2.battleTower.var_4AE[1] = 3;
    gSaveBlock2.battleTower.curStreakChallengesNum[0] = gBattleCommunication[0] + 1;
    gSaveBlock2.battleTower.curStreakChallengesNum[1] = gBattleCommunication[0] + 1;
    gSaveBlock2.battleTower.curChallengeBattleNum[0] = 1;
    gSaveBlock2.battleTower.curChallengeBattleNum[1] = 1;
    CloseMenu();
    return 1;
}
