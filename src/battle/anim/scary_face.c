#include "global.h"
#include "battle_anim.h"
#include "battle.h"
#include "graphics.h"
#include "trig.h"
#include "sound.h"
#include "decompress.h"
#include "palette.h"

static void AnimTask_ScaryFace_Step(u8 taskId);

// scary_face (darkens the screen and shows a scary face.)
// Used in Glare and Scary Face.

void AnimTask_ScaryFace(u8 taskId)
{
    struct BattleAnimBgData animBg;

    REG_BLDCNT = 0x3F42;
    REG_BLDALPHA = 0x1000;
    REG_BG1CNT_BITFIELD.priority = 1;
    REG_BG1CNT_BITFIELD.screenSize = 0;
    if (!IsContest())
        REG_BG1CNT_BITFIELD.charBaseBlock = 1;

    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    GetBattleAnimBg1Data(&animBg);
    DmaFill32Defvars(3, 0, animBg.bgTilemap, 0x1000);
    if (IsContest())
        LZDecompressVram(&gBattleAnimBgTilemap_ScaryFaceContest, animBg.bgTilemap);
    else if (GetBattlerSide(gBattleAnimTarget) == 1)
        LZDecompressVram(&gBattleAnimBgTilemap_ScaryFacePlayer, animBg.bgTilemap);
    else
        LZDecompressVram(&gBattleAnimBgTilemap_ScaryFaceOpponent, animBg.bgTilemap);

    LZDecompressVram(&gBattleAnimBgImage_ScaryFace, animBg.bgTiles);
    LoadCompressedPalette(&gBattleAnimBgPalette_ScaryFace, animBg.paletteId << 4, 32);
    if (IsContest())
        RelocateBattleBgPal(animBg.paletteId, (u16 *)animBg.bgTilemap, 0, 0);

    gTasks[taskId].func = AnimTask_ScaryFace_Step;
}

static void AnimTask_ScaryFace_Step(u8 taskId)
{
    struct BattleAnimBgData animBg;

    switch (gTasks[taskId].data[12])
    {
    case 0:
        if (++gTasks[taskId].data[10] == 2)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[11]++;
            REG_BLDALPHA = gTasks[taskId].data[11] | ((16 - gTasks[taskId].data[11]) << 8);
            if (gTasks[taskId].data[11] == 14)
            {
                gTasks[taskId].data[12]++;
                gTasks[taskId].data[11] = 0;
            }
        }
        break;
    case 1:
        if (++gTasks[taskId].data[11] == 0x15)
        {
            gTasks[taskId].data[11] = 14;
            gTasks[taskId].data[12]++;
        }
        break;
    case 2:
        if (++gTasks[taskId].data[10] == 2)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[11]--;
            REG_BLDALPHA = gTasks[taskId].data[11] | ((16 - gTasks[taskId].data[11]) << 8);
            if (gTasks[taskId].data[11] == 0)
            {
                gTasks[taskId].data[12]++;
                gTasks[taskId].data[11] = 0;
            }
        }
        break;
    case 3:
        GetBattleAnimBg1Data(&animBg);
        DmaFill32Large(3, 0, animBg.bgTiles, 0x2000, 0x1000);
        DmaClear32(3, animBg.bgTilemap, 0x800);
        if (!IsContest())
            REG_BG1CNT_BITFIELD.charBaseBlock = 0;

        gTasks[taskId].data[12]++;
        // fall through
    case 4:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BG1CNT_BITFIELD.priority = 1;
        DestroyAnimVisualTask(taskId);
        break;
    }
}
