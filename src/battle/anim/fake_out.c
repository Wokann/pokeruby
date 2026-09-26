#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "palette.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern u16 gBattle_WIN0V;
extern u16 gBattle_WIN0H;

static void AnimTask_FakeOut_Step1(u8 taskId);
static void AnimTask_FakeOut_Step2(u8 taskId);

// Fake Out background curtain and flash.

void AnimTask_FakeOut(u8 taskId)
{
    int win0v;
    bool8 isContest = IsContest();
    u16 win0h = DISPLAY_WIDTH;
    if (isContest)
    {
        win0h = 152;
        win0v = 0;
    }
    else
    {
        win0v = 0;
    }

    gBattle_WIN0H = win0h;
    gBattle_WIN0V = DISPLAY_HEIGHT;
    REG_WIN0H = win0h;
    REG_WIN0V = DISPLAY_HEIGHT;
    REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
    REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;
    REG_BLDCNT = BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_DARKEN;
    REG_BLDY = 16;
    gTasks[taskId].data[0] = win0v;
    gTasks[taskId].data[1] = win0h;
    gTasks[taskId].func = AnimTask_FakeOut_Step1;
}

static void AnimTask_FakeOut_Step1(u8 taskId)
{
    gTasks[taskId].data[0] += 13;
    gTasks[taskId].data[1] -= 13;
    if (gTasks[taskId].data[0] >= gTasks[taskId].data[1])
    {
        gBattle_WIN0H = 0;
        gTasks[taskId].func = AnimTask_FakeOut_Step2;
    }
    else
    {
        gBattle_WIN0H = gTasks[taskId].data[1] | (gTasks[taskId].data[0] << 8);
    }
}

static void AnimTask_FakeOut_Step2(u8 taskId)
{
    if (++gTasks[taskId].data[10] == 5)
    {
        gTasks[taskId].data[11] = BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_LIGHTEN;
        RequestSpriteCopy((u8 *)(&gTasks[taskId].data[11]), (u8 *)(&REG_BLDCNT), 2);
        BlendPalettes(GetBattlePalettesMask(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE), 16, RGB_WHITE);
    }
    else if (gTasks[taskId].data[10] > 4)
    {
        gBattle_WIN0H = 0;
        gBattle_WIN0V = 0;
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
        REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;
        REG_BLDCNT = 0;
        REG_BLDY = 0;
        DestroyAnimVisualTask(taskId);
    }
}
