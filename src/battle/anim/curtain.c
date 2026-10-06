#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"
#include "palette.h"

extern u16 gBattle_WIN0V;
extern u16 gBattle_WIN0H;

static void AnimTask_FakeOut_Step1(u8 taskId);
static void AnimTask_FakeOut_Step2(u8 taskId);

// curtain (a sweeping blackening curtain in the BG)
// Used in Fake Out.

void AnimTask_FakeOut(u8 taskId)
{
    int win0v;
    bool8 result = IsContest();
    u16 win0h = 0xF0;
    if (result)
    {
        win0h = 0x98;
        win0v = 0;
    }
    else
    {
        win0v = 0;
    }

    gBattle_WIN0H = win0h;
    gBattle_WIN0V = 0xA0;
    REG_WIN0H = win0h;
    REG_WIN0V = 0xA0;
    REG_WININ = 0x3F1F;
    REG_WINOUT = 0x3F3F;
    REG_BLDCNT = 0xC8;
    REG_BLDY = 0x10;
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
        gTasks[taskId].data[11] = 0x88;
        RequestSpriteCopy((u8 *)(&gTasks[taskId].data[11]), (u8 *)(&REG_BLDCNT), 2);
        BlendPalettes(GetBattlePalettesMask(1, 0, 0, 0, 0, 0, 0), 16, RGB(31, 31, 31));
    }
    else if (gTasks[taskId].data[10] > 4)
    {
        gBattle_WIN0H = 0;
        gBattle_WIN0V = 0;
        REG_WININ = 0x3F3F;
        REG_WINOUT = 0x3F3F;
        REG_BLDCNT = 0;
        REG_BLDY = 0;
        DestroyAnimVisualTask(taskId);
    }
}
