#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "decompress.h"
#include "palette.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern const u8 gBattleAnimBgTilemap_Attract[];
extern const u8 gBattleAnimBgImage_Attract[];
extern const u8 gBattleAnimBgPalette_Attract[];

extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG1_X;

static void AnimTask_HeartsBackground_Step(u8 taskId);

// Hearts background used in Attract.

void AnimTask_HeartsBackground(u8 taskId)
{
    struct BattleAnimBgData animBg;

    REG_BLDCNT = BLDCNT_TGT2_ALL | BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND;
    REG_BLDALPHA = BLDALPHA_BLEND(0, 16);
    REG_BG1CNT_BITFIELD.priority = 3;
    REG_BG1CNT_BITFIELD.screenSize = 0;
    if (!IsContest())
        REG_BG1CNT_BITFIELD.charBaseBlock = 1;

    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    GetBattleAnimBg1Data(&animBg);
    DmaFill32Defvars(3, 0, animBg.bgTilemap, 0x1000);
    LZDecompressVram(&gBattleAnimBgTilemap_Attract, animBg.bgTilemap);
    LZDecompressVram(&gBattleAnimBgImage_Attract, animBg.bgTiles);
    LoadCompressedPalette(&gBattleAnimBgPalette_Attract, BG_PLTT_ID(animBg.paletteId), PLTT_SIZE_4BPP);
    if (IsContest())
        sub_80763FC(animBg.paletteId, (u16 *)animBg.bgTilemap, 0, 0);

    gTasks[taskId].func = AnimTask_HeartsBackground_Step;
}

static void AnimTask_HeartsBackground_Step(u8 taskId)
{
    struct BattleAnimBgData animBg;

    switch (gTasks[taskId].data[12])
    {
    case 0:
        if (++gTasks[taskId].data[10] == 4)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[11]++;
            REG_BLDALPHA = gTasks[taskId].data[11] | ((16 - gTasks[taskId].data[11]) << 8);
            if (gTasks[taskId].data[11] == 16)
            {
                gTasks[taskId].data[12]++;
                gTasks[taskId].data[11] = 0;
            }
        }
        break;
    case 1:
        if (++gTasks[taskId].data[11] == 0x8D)
        {
            gTasks[taskId].data[11] = 16;
            gTasks[taskId].data[12]++;
        }
        break;
    case 2:
        if (++gTasks[taskId].data[10] == 4)
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
