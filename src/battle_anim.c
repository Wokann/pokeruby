#include "global.h"
#include "battle_anim.h"
#include "battle.h"
#include "battle_interface.h"
#include "contest.h"
#include "decompress.h"
#include "m4a.h"
#include "main.h"
#include "palette.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "ewram.h"
#include "graphics.h"
#include "constants/battle_anim.h"
#include "constants/songs.h"

#include "data/battle_anim.h"

// sBattleAnimScriptPtr is a pointer to the next set of battle script commands.
static EWRAM_DATA const u8 *sBattleAnimScriptPtr = NULL;
static EWRAM_DATA const u8 *sBattleAnimScriptRetAddr = NULL;
EWRAM_DATA void (*gAnimScriptCallback)(void) = NULL;
static EWRAM_DATA s8 sAnimFramesToWait = 0;
EWRAM_DATA bool8 gAnimScriptActive = FALSE;
EWRAM_DATA u8 gAnimVisualTaskCount = 0;
EWRAM_DATA u8 gAnimSoundTaskCount = 0;
EWRAM_DATA struct DisableStruct *gAnimDisableStructPtr = NULL;
EWRAM_DATA s32 gAnimMoveDmg = 0;
EWRAM_DATA u16 gAnimMovePower = 0;
EWRAM_DATA u8 gAnimFriendship = 0;
EWRAM_DATA u16 gWeatherMoveAnim = 0;
static EWRAM_DATA u8 sMonAnimTaskIdArray[2] = {0};
EWRAM_DATA u8 gAnimMoveTurn = 0;
static EWRAM_DATA u8 sAnimBackgroundFadeState = 0;
EWRAM_DATA static u16 sAnimMoveIndex = 0; // set but unused.
EWRAM_DATA u8 gBattleAnimAttacker = 0;
EWRAM_DATA u8 gBattleAnimTarget = 0;
EWRAM_DATA u16 gAnimBattlerSpecies[4] = {0};
EWRAM_DATA u8 gAnimCustomPanning = 0; // some global pan variable

u16 gSoundAnimFramesToWait;
s16 gBattleAnimArgs[ANIM_ARGS_COUNT];
u16 gAnimSpriteIndexArray[ANIM_SPRITE_INDEX_COUNT];


extern const u16 gMovesWithQuietBGM[];
extern const u8 *const gBattleAnims_Moves[];

static void RunAnimScriptCommand(void);
static void Cmd_loadspritegfx(void);
static void Cmd_unloadspritegfx(void);
static void Cmd_createsprite(void);
static void Cmd_createvisualtask(void);
static void Cmd_delay(void);
static void Cmd_waitforvisualfinish(void);
static void Cmd_nop(void);
static void Cmd_nop2(void);
static void Cmd_end(void);
static void Cmd_playse(void);
static void Cmd_monbg(void);
static void FlipBattlerBgTiles(void);
static void Task_UpdateMonBg(u8 taskId);
static void Cmd_clearmonbg(void);
static void Task_ClearMonBg(u8 taskId);
static void Cmd_monbg_static(void);
static void Cmd_clearmonbg_static(void);
static void Task_ClearMonBgStatic(u8 taskId);
static void Cmd_setalpha(void);
static void Cmd_setbldcnt(void);
static void Cmd_blendoff(void);
static void Cmd_call(void);
static void Cmd_return(void);
static void Cmd_setarg(void);
static void Cmd_choosetwoturnanim(void);
static void Cmd_jumpifmoveturn(void);
static void Cmd_goto(void);
static void Cmd_fadetobg(void);
static void Cmd_fadetobgfromset(void);
static void Task_FadeToBg(u8 taskId);
static void LoadMoveBg(u16 bgId);
static void LoadDefaultBg(void);
static void Cmd_restorebg(void);
static void Cmd_waitbgfadeout(void);
static void Cmd_waitbgfadein(void);
static void Cmd_changebg(void);
static void Cmd_playsewithpan(void);
static void Cmd_setpan(void);
static void Cmd_panse(void);
static void Task_PanFromInitialToTarget(u8 taskId);
static void Cmd_panse_adjustnone(void);
static void Cmd_panse_adjustall(void);
static void Cmd_loopsewithpan(void);
static void Task_LoopAndPlaySE(u8 taskId);
static void Cmd_waitplaysewithpan(void);
static void Task_WaitAndPlaySE(u8 taskId);
static void Cmd_createsoundtask(void);
static void Cmd_waitsound(void);
static void Cmd_jumpargeq(void);
static void Cmd_jumpifcontest(void);
static void Cmd_splitbgprio(void);
static void Cmd_splitbgprio_all(void);
static void Cmd_splitbgprio_foes(void);
static void Cmd_invisible(void);
static void Cmd_visible(void);
static void Cmd_teamattack_moveback(void);
static void Cmd_teamattack_movefwd(void);
static void Cmd_stopsound(void);

static void (*const sScriptCmdTable[])(void) = {
    Cmd_loadspritegfx,
    Cmd_unloadspritegfx,
    Cmd_createsprite,
    Cmd_createvisualtask,
    Cmd_delay,
    Cmd_waitforvisualfinish,
    Cmd_nop,
    Cmd_nop2,
    Cmd_end,
    Cmd_playse,
    Cmd_monbg,
    Cmd_clearmonbg,
    Cmd_setalpha,
    Cmd_blendoff,
    Cmd_call,
    Cmd_return,
    Cmd_setarg,
    Cmd_choosetwoturnanim,
    Cmd_jumpifmoveturn,
    Cmd_goto,
    Cmd_fadetobg,
    Cmd_restorebg,
    Cmd_waitbgfadeout,
    Cmd_waitbgfadein,
    Cmd_changebg,
    Cmd_playsewithpan,
    Cmd_setpan,
    Cmd_panse,
    Cmd_loopsewithpan,
    Cmd_waitplaysewithpan,
    Cmd_setbldcnt,
    Cmd_createsoundtask,
    Cmd_waitsound,
    Cmd_jumpargeq,
    Cmd_monbg_static,
    Cmd_clearmonbg_static,
    Cmd_jumpifcontest,
    Cmd_fadetobgfromset,
    Cmd_panse_adjustnone,
    Cmd_panse_adjustall,
    Cmd_splitbgprio,
    Cmd_splitbgprio_all,
    Cmd_splitbgprio_foes,
    Cmd_invisible,
    Cmd_visible,
    Cmd_teamattack_moveback,
    Cmd_teamattack_movefwd,
    Cmd_stopsound,
};

void ClearBattleAnimationVars(void)
{
    s32 i;

    sAnimFramesToWait = 0;
    gAnimScriptActive = FALSE;
    gAnimVisualTaskCount = 0;
    gAnimSoundTaskCount = 0;
    gAnimDisableStructPtr = NULL;
    gAnimMoveDmg = 0;
    gAnimMovePower = 0;
    gAnimFriendship = 0;

    // clear index array.
    for (i = 0; i < ANIM_SPRITE_INDEX_COUNT; i++)
        gAnimSpriteIndexArray[i] |= 0xFFFF;

    // clear anim args.
    for (i = 0; i < ANIM_ARGS_COUNT; i++)
        gBattleAnimArgs[i] = 0;

    sMonAnimTaskIdArray[0] = 0xFF;
    sMonAnimTaskIdArray[1] = 0xFF;
    gAnimMoveTurn = 0;
    sAnimBackgroundFadeState = 0;
    sAnimMoveIndex = 0;
    gBattleAnimAttacker = 0;
    gBattleAnimTarget = 0;
    gAnimCustomPanning = 0;
}

void DoMoveAnim(u16 move)
{
    gBattleAnimAttacker = gBattlerAttacker;
    gBattleAnimTarget = gBattlerTarget;
    LaunchBattleAnimation(gBattleAnims_Moves, move, TRUE);
}

void LaunchBattleAnimation(const u8 *const animsTable[], u16 tableId, bool8 isMoveAnim)
{
    s32 i;

    if (!IsContest())
    {
        InitPrioritiesForVisibleBattlers();
        UpdateOamPriorityInAllHealthboxes(0);
        for (i = 0; i < 4; i++)
        {
            if (GetBattlerSide(i) != B_SIDE_PLAYER)
                gAnimBattlerSpecies[i] = GetMonData(&gEnemyParty[gBattlerPartyIndexes[i]], MON_DATA_SPECIES);
            else
                gAnimBattlerSpecies[i] = GetMonData(&gPlayerParty[gBattlerPartyIndexes[i]], MON_DATA_SPECIES);
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
            gAnimBattlerSpecies[i] = gContestResources__moveAnim.species;
    }

    if (isMoveAnim == 0)
        sAnimMoveIndex = 0;
    else
        sAnimMoveIndex = tableId;

    for (i = 0; i < ANIM_ARGS_COUNT; i++)
        gBattleAnimArgs[i] = 0;

    sMonAnimTaskIdArray[0] = 0xFF;
    sMonAnimTaskIdArray[1] = 0xFF;
    sBattleAnimScriptPtr = animsTable[tableId];
    gAnimScriptActive = TRUE;
    sAnimFramesToWait = 0;
    gAnimScriptCallback = RunAnimScriptCommand;

    for (i = 0; i < ANIM_SPRITE_INDEX_COUNT; i++)
        gAnimSpriteIndexArray[i] |= 0xFFFF;

    if (isMoveAnim)
    {
        for (i = 0; gMovesWithQuietBGM[i] != 0xFFFF; i++)
        {
            if (tableId == gMovesWithQuietBGM[i])
            {
                // Lower the volume for the short song that gets played.
                m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 128);
                break;
            }
        }
    }

    gBattle_WIN0H = 0;
    gBattle_WIN0V = 0;
    gBattle_WIN1H = 0;
    gBattle_WIN1V = 0;
}

void DestroyAnimSprite(struct Sprite *sprite)
{
    FreeSpriteOamMatrix(sprite);
    DestroySprite(sprite);
    gAnimVisualTaskCount--;
}

void DestroyAnimVisualTask(u8 taskId)
{
    DestroyTask(taskId);
    gAnimVisualTaskCount--;
}

void DestroyAnimSoundTask(u8 taskId)
{
    DestroyTask(taskId);
    gAnimSoundTaskCount--;
}

static void AddSpriteIndex(u16 index)
{
    s32 i;

    for (i = 0; i < ANIM_SPRITE_INDEX_COUNT; i++)
    {
        if (gAnimSpriteIndexArray[i] == 0xFFFF)
        {
            gAnimSpriteIndexArray[i] = index;
            return;
        }
    }
}

static void ClearSpriteIndex(u16 index)
{
    s32 i;

    for (i = 0; i < ANIM_SPRITE_INDEX_COUNT; i++)
    {
        if (gAnimSpriteIndexArray[i] == index)
        {
            gAnimSpriteIndexArray[i] |= 0xFFFF;
            return;
        }
    }
}

static void WaitAnimFrameCount(void)
{
    if (sAnimFramesToWait <= 0)
    {
        gAnimScriptCallback = RunAnimScriptCommand;
        sAnimFramesToWait = 0;
    }
    else
    {
        sAnimFramesToWait--;
    }
}

static void RunAnimScriptCommand(void)
{
    do
    {
        sScriptCmdTable[T1_READ_8(sBattleAnimScriptPtr)]();
    } while (sAnimFramesToWait == 0 && gAnimScriptActive);
}

// Loads sprite graphics used in a move into memory.
// arg 0: gfx ANIM_TAG
static void Cmd_loadspritegfx(void)
{
    u16 tag;

    sBattleAnimScriptPtr++;
    tag = T1_READ_16(sBattleAnimScriptPtr);
    LoadCompressedSpriteSheet(&gBattleAnimPicTable[GET_TRUE_SPRITE_INDEX(tag)]);
    LoadCompressedSpritePalette(&gBattleAnimPaletteTable[GET_TRUE_SPRITE_INDEX(tag)]);
    sBattleAnimScriptPtr += 2;
    AddSpriteIndex(GET_TRUE_SPRITE_INDEX(tag));
    sAnimFramesToWait = 1;
    gAnimScriptCallback = WaitAnimFrameCount;
}

// Frees sprite graphics from memory when move animation no longer needs them.
// arg0: gfx ANIM_TAG
static void Cmd_unloadspritegfx(void)
{
    u16 tag;

    sBattleAnimScriptPtr++;
    tag = T1_READ_16(sBattleAnimScriptPtr);
    FreeSpriteTilesByTag(gBattleAnimPicTable[GET_TRUE_SPRITE_INDEX(tag)].tag);
    FreeSpritePaletteByTag(gBattleAnimPicTable[GET_TRUE_SPRITE_INDEX(tag)].tag);
    sBattleAnimScriptPtr += 2;
    ClearSpriteIndex(GET_TRUE_SPRITE_INDEX(tag));
}

// Creates a sprite from the given sprite template.
// arg0: SpriteTemplate
// arg1: s16[] gBattleAnimArgs
static void Cmd_createsprite(void)
{
    s32 i;
    const struct SpriteTemplate *template;
    u8 argVar;
    u8 argsCount;
    s16 subpriority;

    sBattleAnimScriptPtr++;
    template = (const struct SpriteTemplate *)(T2_READ_32(sBattleAnimScriptPtr));
    sBattleAnimScriptPtr += 4;

    argVar = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;

    argsCount = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;
    for (i = 0; i < argsCount; i++)
    {
        gBattleAnimArgs[i] = T1_READ_16(sBattleAnimScriptPtr);
        sBattleAnimScriptPtr += 2;
    }

    if (argVar & 0x80)
    {
        argVar ^= 0x80;
        if (argVar >= 0x40)
            argVar -= 0x40;
        else
            argVar *= -1;

        subpriority = GetBattlerSpriteSubpriority(gBattleAnimTarget) + (s8)(argVar);
    }
    else
    {
        if (argVar >= 0x40)
            argVar -= 0x40;
        else
            argVar *= -1;

        subpriority = GetBattlerSpriteSubpriority(gBattleAnimAttacker) + (s8)(argVar);
    }

    if (subpriority < 3)
        subpriority = 3;

    CreateSpriteAndAnimate(template, GetBattlerSpriteCoord(gBattleAnimTarget, 2), GetBattlerSpriteCoord(gBattleAnimTarget, 3), subpriority);
    gAnimVisualTaskCount++;
}

// Initializes an animation task.
// arg0: AnimTask function
// arg1: s16[] arguments
static void Cmd_createvisualtask(void)
{
    TaskFunc taskFunc;
    u8 taskPriority;
    u8 taskId;
    u8 numArgs;
    s32 i;

    sBattleAnimScriptPtr++;

    taskFunc = (TaskFunc)T2_READ_32(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr += 4;

    taskPriority = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;

    numArgs = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;

    for (i = 0; i < numArgs; i++)
    {
        gBattleAnimArgs[i] = T1_READ_16(sBattleAnimScriptPtr);
        sBattleAnimScriptPtr += 2;
    }

    taskId = CreateTask(taskFunc, taskPriority);
    taskFunc(taskId);
    gAnimVisualTaskCount++;
}

// Creates a visual delay.
// arg0: number of frames to wait.
static void Cmd_delay(void)
{
    sBattleAnimScriptPtr++;
    sAnimFramesToWait = T1_READ_8(sBattleAnimScriptPtr);
    if (sAnimFramesToWait == 0)
        sAnimFramesToWait = -1;
    sBattleAnimScriptPtr++;
    gAnimScriptCallback = WaitAnimFrameCount;
}

// Wait for visual tasks to finish.
static void Cmd_waitforvisualfinish(void)
{
    if (gAnimVisualTaskCount == 0)
    {
        sBattleAnimScriptPtr++;
        sAnimFramesToWait = 0;
    }
    else
    {
        sAnimFramesToWait = 1;
    }
}

static void Cmd_nop(void)
{
}

static void Cmd_nop2(void)
{
}

// Marks the end of an animation. Finishes the anims, tasks, and sound effects.
// started during an animaiton.
static void Cmd_end(void)
{
    s32 i;
    bool32 continuousAnim = FALSE;

    // keep waiting as long as there is animations to be done.
    if (gAnimVisualTaskCount != 0 || gAnimSoundTaskCount != 0
     || sMonAnimTaskIdArray[0] != 0xFF || sMonAnimTaskIdArray[1] != 0xFF)
    {
        gSoundAnimFramesToWait = 0;
        sAnimFramesToWait = 1;
        return;
    }

    // finish the sound effects.
    if (IsSEPlaying())
    {
        if (++gSoundAnimFramesToWait <= 90) // wait 90 frames, then halt the sound effect.
        {
            sAnimFramesToWait = 1;
            return;
        }
        else
        {
            m4aMPlayStop(&gMPlayInfo_SE1);
            m4aMPlayStop(&gMPlayInfo_SE2);
        }
    }

    // the SE has halted, so set the SE Frame Counter to 0 and continue.
    gSoundAnimFramesToWait = 0;

    for (i = 0; i < ANIM_SPRITE_INDEX_COUNT; i++)
    {
        if (gAnimSpriteIndexArray[i] != 0xFFFF)
        {
            FreeSpriteTilesByTag(gBattleAnimPicTable[gAnimSpriteIndexArray[i]].tag);
            FreeSpritePaletteByTag(gBattleAnimPicTable[gAnimSpriteIndexArray[i]].tag);
            gAnimSpriteIndexArray[i] |= 0xFFFF; // set terminator.
        }
    }

    if (!continuousAnim) // may have been used for debug?
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 256);
        if (!IsContest())
        {
            InitPrioritiesForVisibleBattlers();
            UpdateOamPriorityInAllHealthboxes(1);
        }
        gAnimScriptActive = FALSE;
    }
}

// Plays a sound effect.
// arg0: sound effect ID
static void Cmd_playse(void)
{
    sBattleAnimScriptPtr++;
    PlaySE(T1_READ_16(sBattleAnimScriptPtr));
    sBattleAnimScriptPtr += 2;
}

// 
// arg0: battler
static void Cmd_monbg(void)
{
    u8 animBattler;
    u8 battler;
    u8 position;
    bool8 toBG_2;
    u16 spriteId;
    u8 taskId;

    sBattleAnimScriptPtr++;
    animBattler = T1_READ_8(sBattleAnimScriptPtr);
    if (animBattler == ANIM_ATTACKER)
        animBattler = ANIM_ATK_PARTNER;
    else if (animBattler == ANIM_TARGET)
        animBattler = ANIM_DEF_PARTNER;

    if (animBattler == ANIM_ATTACKER || animBattler == ANIM_ATK_PARTNER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    if (IsBattlerSpriteVisible(battler))
    {
        position = GetBattlerPosition(battler);
        position += 0xFF;
        if (position <= 1 || IsContest())
            toBG_2 = 0;
        else
            toBG_2 = 1;

        MoveBattlerSpriteToBG(battler, toBG_2);
        spriteId = gBattlerSpriteIds[battler];
        taskId = CreateTask(Task_UpdateMonBg, 10);
        gTasks[taskId].data[0] = spriteId;
        gTasks[taskId].data[1] = gSprites[spriteId].x + gSprites[spriteId].x2;
        gTasks[taskId].data[2] = gSprites[spriteId].y + gSprites[spriteId].y2;
        if (toBG_2 == 0)
        {
            gTasks[taskId].data[3] = gBattle_BG1_X;
            gTasks[taskId].data[4] = gBattle_BG1_Y;
        }
        else
        {
            gTasks[taskId].data[3] = gBattle_BG2_X;
            gTasks[taskId].data[4] = gBattle_BG2_Y;
        }
        gTasks[taskId].data[5] = toBG_2;
        gTasks[taskId].data[6] = battler;
        sMonAnimTaskIdArray[0] = taskId;

    }

    battler ^= 2;
    if (animBattler >= ANIM_ATK_PARTNER && IsBattlerSpriteVisible(battler))
    {
        position = GetBattlerPosition(battler);
        position += 0xFF;
        if (position <= 1 || IsContest())
            toBG_2 = 0;
        else
            toBG_2 = 1;
        MoveBattlerSpriteToBG(battler, toBG_2);
        spriteId = gBattlerSpriteIds[battler];
        taskId = CreateTask(Task_UpdateMonBg, 10);
        gTasks[taskId].data[0] = spriteId;
        gTasks[taskId].data[1] = gSprites[spriteId].x + gSprites[spriteId].x2;
        gTasks[taskId].data[2] = gSprites[spriteId].y + gSprites[spriteId].y2;
        if (toBG_2 == 0)
        {
            gTasks[taskId].data[3] = gBattle_BG1_X;
            gTasks[taskId].data[4] = gBattle_BG1_Y;
        }
        else
        {
            gTasks[taskId].data[3] = gBattle_BG2_X;
            gTasks[taskId].data[4] = gBattle_BG2_Y;
        }

        gTasks[taskId].data[5] = toBG_2;
        gTasks[taskId].data[6] = battler;
        sMonAnimTaskIdArray[1] = taskId;
    }

    sBattleAnimScriptPtr++;
}

bool8 IsBattlerSpriteVisible(u8 battler)
{
    if (IsContest())
    {
        if (battler == gBattleAnimAttacker)
            return TRUE;
        else
            return FALSE;
    }
    if (!IsBattlerSpritePresent(battler))
        return FALSE;
    if (IsContest())
        return TRUE; // this line wont ever be reached.
    if (!gBattleSpriteInfo[battler].invisible || !gSprites[gBattlerSpriteIds[battler]].invisible)
        return TRUE;

    return FALSE;
}

void MoveBattlerSpriteToBG(u8 battler, bool8 toBG_2)
{
    u8 battlerSpriteId;

    if (toBG_2 == 0)
    {
        volatile u8 pointlessZero;
        struct BattleAnimBgData animBg;
        u8 battlerPosition;

        GetBattleAnimBg1Data(&animBg);
        DmaFill32Large(3, 0, animBg.bgTiles, 0x2000, 0x1000);
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        DmaFill16Defvars(3, 0xFF, (void *)animBg.bgTilemap, 0x1000);

        REG_BG1CNT_BITFIELD.priority = 2;
        REG_BG1CNT_BITFIELD.screenSize = 1;
        REG_BG1CNT_BITFIELD.areaOverflowMode = 0;

        battlerSpriteId = gBattlerSpriteIds[battler];
        gBattle_BG1_X = -(gSprites[battlerSpriteId].x + gSprites[battlerSpriteId].x2) + 32;
        if (IsContest() && IsSpeciesNotUnown(gContestResources__moveAnim.species) != 0)
            gBattle_BG1_X--;
        gBattle_BG1_Y = -(gSprites[battlerSpriteId].y + gSprites[battlerSpriteId].y2) + 32;
        gSprites[gBattlerSpriteIds[battler]].invisible = TRUE;

        REG_BG1HOFS = gBattle_BG1_X;
        REG_BG1VOFS = gBattle_BG1_Y;

        LoadPalette(gPlttBufferUnfaded + 0x100 + battler * 16, animBg.paletteId * 16, 32);
        DmaCopy32Defvars(3, gPlttBufferUnfaded + 0x100 + battler * 16, (u16 *)PLTT + animBg.paletteId * 16, 32);

        if (IsContest())
            battlerPosition = 0;
        else
            battlerPosition = GetBattlerPosition(battler);
        sub_80E4EF8(0, 0, battlerPosition, animBg.paletteId, (u32)animBg.bgTiles, (((s32)animBg.bgTilemap - VRAM) / 2048), REG_BG1CNT_BITFIELD.charBaseBlock);
        if (IsContest())
            FlipBattlerBgTiles();
    }
    else
    {
        volatile u8 pointlessZero;

        DmaFill32Large(3, 0, (void *)(VRAM + 0x6000), 0x2000, 0x1000);
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        DmaFill32Defvars(3, 0, (void *)(VRAM + 0xF000), 0x800);

        REG_BG2CNT_BITFIELD.priority = 2;
        REG_BG2CNT_BITFIELD.screenSize = 1;
        REG_BG2CNT_BITFIELD.areaOverflowMode = 0;

        battlerSpriteId = gBattlerSpriteIds[battler];
        gBattle_BG2_X = -(gSprites[battlerSpriteId].x + gSprites[battlerSpriteId].x2) + 32;
        gBattle_BG2_Y = -(gSprites[battlerSpriteId].y + gSprites[battlerSpriteId].y2) + 32;
        gSprites[gBattlerSpriteIds[battler]].invisible = TRUE;

        REG_BG2HOFS = gBattle_BG2_X;
        REG_BG2VOFS = gBattle_BG2_Y;

        LoadPalette(gPlttBufferUnfaded + 0x100 + battler * 16, 0x90, 32);
        DmaCopy32Defvars(3, gPlttBufferUnfaded + 0x100 + battler * 16, (void *)(PLTT + 0x120), 32);

        sub_80E4EF8(0, 0, GetBattlerPosition(battler), 9, 0x6000, 0x1E, REG_BG2CNT_BITFIELD.charBaseBlock);
    }
}

static void FlipBattlerBgTiles(void)
{
    s32 i;
    s32 j;
    struct BattleAnimBgData animBg;
    u16 *ptr;

    if (IsSpeciesNotUnown(gContestResources__moveAnim.species))
    {
        GetBattleAnimBg1Data(&animBg);
        ptr = (u16 *)animBg.bgTilemap;
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 4; j++)
            {
                u16 temp = ptr[j + i * 32];

                ptr[j + i * 32] = ptr[7 - j + i * 32];
                ptr[7 - j + i * 32] = temp;
            }
        }
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
                ptr[j + i * 32] ^= 0x400;
        }
    }
}

void RelocateBattleBgPal(u16 paletteNum, u16 *dest, u32 offset, bool8 largeScreen)
{
    u8 i;
    u8 j;
    u32 size;

    if (largeScreen == 0)
        size = 32;
    else
        size = 64;
    paletteNum <<= 12;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < 32; j++)
            dest[j + i * 32] = ((dest[j + i * 32] & 0xFFF) | paletteNum) + offset;
    }
}

void ResetBattleAnimBg(bool8 toBG2)
{
    volatile u8 pointlessZero;
    struct BattleAnimBgData animBg;

    GetBattleAnimBg1Data(&animBg);
    if (toBG2 == 0 || IsContest())
    {
        DmaFill32Large(3, 0, animBg.bgTiles, 0x2000, 0x1000);
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        DmaFill32Defvars(3, 0, animBg.bgTilemap, 0x800);
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 0;
    }
    else
    {
        DmaFill32Large(3, 0, (void *)(VRAM + 0x6000), 0x2000, 0x1000);
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        pointlessZero = 0; // is there a stubbed out Dma macro here that left the 0 load in?
        DmaFill32Defvars(3, 0, (void *)(VRAM + 0xF000), 0x800);
        gBattle_BG2_X = 0;
        gBattle_BG2_Y = 0;
    }
}

static void Task_UpdateMonBg(u8 taskId)
{
    u8 spriteId, battler;
    s16 x, y;
    struct BattleAnimBgData animBg;

    spriteId = gTasks[taskId].data[0];
    battler = gTasks[taskId].data[6];
    GetBattleAnimBg1Data(&animBg);
    x = gTasks[taskId].data[1] - (gSprites[spriteId].x + gSprites[spriteId].x2);
    y = gTasks[taskId].data[2] - (gSprites[spriteId].y + gSprites[spriteId].y2);

    if (gTasks[taskId].data[5] == 0)
    {
        gBattle_BG1_X = x + gTasks[taskId].data[3];
        gBattle_BG1_Y = y + gTasks[taskId].data[4];
        DmaCopy32Defvars(3, gPlttBufferFaded + 0x100 + battler * 16, gPlttBufferFaded + 0x100 + animBg.paletteId * 16 - 256, 32);
    }
    else
    {
        gBattle_BG2_X = x + gTasks[taskId].data[3];
        gBattle_BG2_Y = y + gTasks[taskId].data[4];
        DmaCopy32Defvars(3, gPlttBufferFaded + 0x100 + battler * 16, gPlttBufferFaded + 0x100 - 112, 32);
    }
}

static void Cmd_clearmonbg(void)
{
    u8 animBattlerId;
    u8 battler;
    u8 taskId;

    sBattleAnimScriptPtr++;
    animBattlerId = T1_READ_8(sBattleAnimScriptPtr);

    if (animBattlerId == ANIM_ATTACKER)
        animBattlerId = ANIM_ATK_PARTNER;
    else if (animBattlerId == ANIM_TARGET)
        animBattlerId = ANIM_DEF_PARTNER;

    if (animBattlerId == ANIM_ATTACKER || animBattlerId == ANIM_ATK_PARTNER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    if (sMonAnimTaskIdArray[0] != 0xFF)
        gSprites[gBattlerSpriteIds[battler]].invisible = FALSE;
    if (animBattlerId > 1 && sMonAnimTaskIdArray[1] != 0xFF)
        gSprites[gBattlerSpriteIds[battler ^ 2]].invisible = FALSE;
    else
        animBattlerId = 0;

    taskId = CreateTask(Task_ClearMonBg, 5);
    gTasks[taskId].data[0] = animBattlerId;
    gTasks[taskId].data[2] = battler;
    sBattleAnimScriptPtr++;
}

static void Task_ClearMonBg(u8 taskId)
{
    u8 position;
    u8 to_BG2;

    gTasks[taskId].data[1]++;
    if (gTasks[taskId].data[1] != 1)
    {
        position = GetBattlerPosition(gTasks[taskId].data[2]);
        position += 0xFF;
        if (position <= 1 || IsContest())
            to_BG2 = 0;
        else
            to_BG2 = 1;
        if (sMonAnimTaskIdArray[0] != 0xFF)
        {
            ResetBattleAnimBg(to_BG2);
            DestroyTask(sMonAnimTaskIdArray[0]);
            sMonAnimTaskIdArray[0] = 0xFF;
        }
        if (gTasks[taskId].data[0] > 1)
        {
            ResetBattleAnimBg(to_BG2 ^ 1);
            DestroyTask(sMonAnimTaskIdArray[1]);
            sMonAnimTaskIdArray[1] = 0xFF;
        }
        DestroyTask(taskId);
    }
}

static void Cmd_monbg_static(void)
{
    u8 animBattlerId;
    u8 battler;
    u8 position;
    bool8 toBG_2;

    sBattleAnimScriptPtr++;
    animBattlerId = T1_READ_8(sBattleAnimScriptPtr);

    if (animBattlerId == ANIM_ATTACKER)
        animBattlerId = ANIM_ATK_PARTNER;
    else if (animBattlerId == ANIM_TARGET)
        animBattlerId = ANIM_DEF_PARTNER;

    if (animBattlerId == ANIM_ATTACKER || animBattlerId == ANIM_ATK_PARTNER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    if (IsBattlerSpriteVisible(battler))
    {
        position = GetBattlerPosition(battler);
        position += 0xFF;
        if (position <= 1 || IsContest())
            toBG_2 = 0;
        else
            toBG_2 = 1;
        MoveBattlerSpriteToBG(battler, toBG_2);
        gSprites[gBattlerSpriteIds[battler]].invisible = FALSE;
    }

    battler ^= 2;
    if (animBattlerId > ANIM_TARGET && IsBattlerSpriteVisible(battler))
    {
        position = GetBattlerPosition(battler);
        position += 0xFF;
        if (position <= 1 || IsContest())
            toBG_2 = 0;
        else
            toBG_2 = 1;
        MoveBattlerSpriteToBG(battler, toBG_2);
        gSprites[gBattlerSpriteIds[battler]].invisible = FALSE;
    }
    sBattleAnimScriptPtr++;
}

static void Cmd_clearmonbg_static(void)
{
    u8 animBattlerId;
    u8 battler;
    u8 taskId;

    sBattleAnimScriptPtr++;
    animBattlerId = T1_READ_8(sBattleAnimScriptPtr);

    if (animBattlerId == ANIM_ATTACKER)
        animBattlerId = ANIM_ATK_PARTNER;
    else if (animBattlerId == ANIM_TARGET)
        animBattlerId = ANIM_DEF_PARTNER;

    if (animBattlerId == ANIM_ATTACKER || animBattlerId == ANIM_ATK_PARTNER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    if (IsBattlerSpriteVisible(battler))
        gSprites[gBattlerSpriteIds[battler]].invisible = FALSE;
    if (animBattlerId > 1 && IsBattlerSpriteVisible(battler ^ 2))
        gSprites[gBattlerSpriteIds[battler ^ 2]].invisible = FALSE;
    else
        animBattlerId = 0;

    taskId = CreateTask(Task_ClearMonBgStatic, 5);
    gTasks[taskId].data[0] = animBattlerId;
    gTasks[taskId].data[2] = battler;

    sBattleAnimScriptPtr++;
}

static void Task_ClearMonBgStatic(u8 taskId)
{
    u8 position;
    u8 battler;
    bool8 toBG_2;

    gTasks[taskId].data[1]++;
    if (gTasks[taskId].data[1] != 1)
    {
        battler = gTasks[taskId].data[2];
        position = GetBattlerPosition(battler);
        position += 0xFF;
        if (position <= 1 || IsContest())
            toBG_2 = 0;
        else
            toBG_2 = 1;
        if (IsBattlerSpriteVisible(battler))
            ResetBattleAnimBg(toBG_2);
        if (gTasks[taskId].data[0] > 1 && IsBattlerSpriteVisible(battler ^ 2))
            ResetBattleAnimBg(toBG_2 ^ 1);
        DestroyTask(taskId);
    }
}

// Sets transparency of sprite.
// arg0: sprite alpha value
// arg1: background alpha value
static void Cmd_setalpha(void)
{
    u16 spriteAlpha, bgAlpha;

    sBattleAnimScriptPtr++;
    spriteAlpha = *(sBattleAnimScriptPtr++);
    bgAlpha = *(sBattleAnimScriptPtr++) << 8;
    REG_BLDCNT = BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_EFFECT_BLEND;
    REG_BLDALPHA = spriteAlpha | bgAlpha;
}

static void Cmd_setbldcnt(void)
{
    u16 half1, half2;

    sBattleAnimScriptPtr++;
    half1 = *(sBattleAnimScriptPtr++);
    half2 = *(sBattleAnimScriptPtr++) << 8;
    REG_BLDCNT = half1 | half2;
}

// Turns off alpha blending / semi transparency.
static void Cmd_blendoff(void)
{
    sBattleAnimScriptPtr++;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
}

// Calls another animation by resetting sBattleAnimScriptPtr.
// arg0: Function
static void Cmd_call(void)
{
    sBattleAnimScriptPtr++;
    sBattleAnimScriptRetAddr = sBattleAnimScriptPtr + 4;
    sBattleAnimScriptPtr = T2_READ_PTR(sBattleAnimScriptPtr);
}

// Returns to the function that called this.
static void Cmd_return(void)
{
    sBattleAnimScriptPtr = sBattleAnimScriptRetAddr;
}

// Sets a value into gBattleAnimArgs[8]
// arg0: index / arg number
// arg1: value to set
static void Cmd_setarg(void)
{
    const u8 *addr = sBattleAnimScriptPtr;
    u16 value;
    u8 argId;

    sBattleAnimScriptPtr++;
    argId = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;
    value = T1_READ_16(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr = addr + 4;
    gBattleAnimArgs[argId] = value;
}

// Flips between the first and second step of a move with two turns.
// arg0: first turn animation
// arg1: second turn animation
static void Cmd_choosetwoturnanim(void)
{
    sBattleAnimScriptPtr++;
    if (gAnimMoveTurn & 1)
        sBattleAnimScriptPtr += 4;
    sBattleAnimScriptPtr = T2_READ_PTR(sBattleAnimScriptPtr);
}

// Jump to specified step of multi turn moves.
// arg0: move turn
// arg1: turn animation
static void Cmd_jumpifmoveturn(void)
{
    u8 toCheck;

    sBattleAnimScriptPtr++;
    toCheck = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;

    if (toCheck == gAnimMoveTurn)
    {
        sBattleAnimScriptPtr = T2_READ_PTR(sBattleAnimScriptPtr);
    }
    else
    {
        sBattleAnimScriptPtr += 4;
    }
}

// Jump to another animation.
// arg0: new animation
static void Cmd_goto(void)
{
    sBattleAnimScriptPtr++;
    sBattleAnimScriptPtr = T2_READ_PTR(sBattleAnimScriptPtr);
}

bool8 IsContest(void)
{
    if (!gMain.inBattle)
        return TRUE;
    else
        return FALSE;
}

#define tBackgroundId   data[0]
#define tState          data[10]

// Fades the screen and sets new background image.
// arg0: background ID
static void Cmd_fadetobg(void)
{
    u8 backgroundId;
    u8 taskId;

    sBattleAnimScriptPtr++;
    backgroundId = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;
    taskId = CreateTask(Task_FadeToBg, 5);
    gTasks[taskId].tBackgroundId = backgroundId;
    sAnimBackgroundFadeState = 1;
}

// Fades to background image based on context of move (contest, battle)
// arg0: opponent background image ID
// arg1: player background image ID
// arg2: contest background image ID
static void Cmd_fadetobgfromset(void)
{
    u8 bg1, bg2, bg3;
    u8 taskId;

    sBattleAnimScriptPtr++;
    bg1 = sBattleAnimScriptPtr[0];
    bg2 = sBattleAnimScriptPtr[1];
    bg3 = sBattleAnimScriptPtr[2];
    sBattleAnimScriptPtr += 3;
    taskId = CreateTask(Task_FadeToBg, 5);

    if (IsContest())
        gTasks[taskId].tBackgroundId = bg3;
    else if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
        gTasks[taskId].tBackgroundId = bg2;
    else
        gTasks[taskId].tBackgroundId = bg1;

    sAnimBackgroundFadeState = 1;
}

static void Task_FadeToBg(u8 taskId)
{
    if (gTasks[taskId].tState == 0)
    {
        BeginHardwarePaletteFade(0xE8, 0, 0, 16, 0);
        gTasks[taskId].tState++;
        return;
    }
    if (gPaletteFade.active)
        return;
    if (gTasks[taskId].tState == 1)
    {
        gTasks[taskId].tState++;
        sAnimBackgroundFadeState = 2;
    }
    else if (gTasks[taskId].tState == 2)
    {
        s16 bgId = (u16)gTasks[taskId].tBackgroundId;

        if (bgId == -1)
            LoadDefaultBg();
        else
            LoadMoveBg(bgId);

        BeginHardwarePaletteFade(0xE8, 0, 16, 0, 1);
        gTasks[taskId].tState++;
        return;
    }
    if (gPaletteFade.active)
        return;
    if (gTasks[taskId].tState == 3)
    {
        DestroyTask(taskId);
        sAnimBackgroundFadeState = 0;
    }
}

static void LoadMoveBg(u16 bgId)
{
    if (IsContest())
    {
        void *tilemap = gBattleAnimBackgroundTable[bgId].tilemap;

        LZDecompressWram(tilemap, IsContest() ? eBattleAnimPalBackup_Contest : eBattleAnimPalBackup_Battle);
        RelocateBattleBgPal(GetBattleBgPaletteNum(), IsContest() ? eBattleAnimPalBackup_Contest : eBattleAnimPalBackup_Battle, 0x100, 0);
        DmaCopy32Defvars(3, IsContest() ? eBattleAnimPalBackup_Contest : eBattleAnimPalBackup_Battle, (void *)(VRAM + 0xD000), 0x800);
        LZDecompressVram(gBattleAnimBackgroundTable[bgId].image, (void *)(VRAM + 0x2000));
        LoadCompressedPalette(gBattleAnimBackgroundTable[bgId].palette, GetBattleBgPaletteNum() * 16, 32);
    }
    else
    {
        LZDecompressVram(gBattleAnimBackgroundTable[bgId].tilemap, (void *)(VRAM + 0xD000));
        LZDecompressVram(gBattleAnimBackgroundTable[bgId].image, (void *)(VRAM + 0x8000));
        LoadCompressedPalette(gBattleAnimBackgroundTable[bgId].palette, 32, 32);
    }
}

static void LoadDefaultBg(void)
{
    if (IsContest())
        LoadContestBgAfterMoveAnim();
    else
        DrawMainBattleBackground();
}

// Restores default background image.
static void Cmd_restorebg(void)
{
    u8 taskId;

    sBattleAnimScriptPtr++;
    taskId = CreateTask(Task_FadeToBg, 5);
    gTasks[taskId].tBackgroundId = 0xFFFF;
    sAnimBackgroundFadeState = 1;
}

#undef tBackgroundId
#undef tState

// Wait for background image fade out to compete.
static void Cmd_waitbgfadeout(void)
{
    if (sAnimBackgroundFadeState == 2)
    {
        sBattleAnimScriptPtr++;
        sAnimFramesToWait = 0;
    }
    else
    {
        sAnimFramesToWait = 1;
    }
}

// Wait for background image fade in to compete.
static void Cmd_waitbgfadein(void)
{
    if (sAnimBackgroundFadeState == 0)
    {
        sBattleAnimScriptPtr++;
        sAnimFramesToWait = 0;
    }
    else
    {
        sAnimFramesToWait = 1;
    }
}

// Change background.
// arg0: background image ID
static void Cmd_changebg(void)
{
    sBattleAnimScriptPtr++;
    LoadMoveBg(T1_READ_8(sBattleAnimScriptPtr));
    sBattleAnimScriptPtr++;
}

s8 BattleAnimAdjustPanning(s8 pan)
{
    if (!IsContest() && gBattleHealthBoxInfo[gBattleAnimAttacker].statusAnimActive)
    {
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
            pan = SOUND_PAN_TARGET;
        else
            pan = SOUND_PAN_ATTACKER;
    }
    else if (IsContest())
    {
        if (gBattleAnimAttacker != gBattleAnimTarget || gBattleAnimAttacker != 2 || pan != SOUND_PAN_TARGET)
            pan *= -1;
    }
    else if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
    {
        if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
        {
            if (pan == SOUND_PAN_TARGET)
                pan = SOUND_PAN_ATTACKER;
            else if (pan != SOUND_PAN_ATTACKER)
                pan *= -1;
        }
    }
    else if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_OPPONENT)
    {
        if (pan == SOUND_PAN_ATTACKER)
            pan = SOUND_PAN_TARGET;
    }
    else
    {
        pan *= -1;
    }

    if (pan > SOUND_PAN_TARGET)
        pan = SOUND_PAN_TARGET;
    else if (pan < SOUND_PAN_ATTACKER)
        pan = SOUND_PAN_ATTACKER;

    return pan;
}

s8 BattleAnimAdjustPanning2(s8 pan)
{
    if (!IsContest() && gBattleHealthBoxInfo[gBattleAnimAttacker].statusAnimActive)
    {
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
            pan = SOUND_PAN_TARGET;
        else
            pan = SOUND_PAN_ATTACKER;
    }
    else
    {
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER || IsContest())
            pan = -pan;
    }
    return pan;
}

s16 KeepPanInRange(s16 panArg, int oldPan)
{
    s16 pan = panArg;

    if (pan > SOUND_PAN_TARGET)
        pan = SOUND_PAN_TARGET;
    else if (pan < SOUND_PAN_ATTACKER)
        pan = SOUND_PAN_ATTACKER;
    return pan;
}

s16 CalculatePanIncrement(s16 sourcePan, s16 targetPan, s16 incrementPan)
{
    s16 ret;

    if (sourcePan < targetPan)
        ret = ((incrementPan < 0) ? -incrementPan : incrementPan);
    else if (sourcePan > targetPan)
        ret = -((incrementPan < 0) ? -incrementPan : incrementPan);
    else
        ret = 0;

    return ret;
}

static void Cmd_playsewithpan(void)
{
    u16 songId;
    s8 pan;

    sBattleAnimScriptPtr++;
    songId = T1_READ_16(sBattleAnimScriptPtr);
    pan = T1_READ_8(sBattleAnimScriptPtr + 2);
    PlaySE12WithPanning(songId, BattleAnimAdjustPanning(pan));
    sBattleAnimScriptPtr += 3;
}

static void Cmd_setpan(void)
{
    s8 pan;

    sBattleAnimScriptPtr++;
    pan = T1_READ_8(sBattleAnimScriptPtr);
    SE12PanpotControl(BattleAnimAdjustPanning(pan));
    sBattleAnimScriptPtr++;
}

#define tInitialPan     data[0]
#define tTargetPan      data[1]
#define tIncrementPan   data[2]
#define tFramesToWait   data[3]
#define tCurrentPan     data[4]
#define tFrameCounter   data[8]

static void Cmd_panse(void)
{
    u16 songNum;
    s8 currentPanArg, incrementPan, incrementPanArg, currentPan, targetPan;
    u8 framesToWait;
    u8 taskId;

    sBattleAnimScriptPtr++;
    songNum = T1_READ_16(sBattleAnimScriptPtr);
    currentPanArg = T1_READ_8(sBattleAnimScriptPtr + 2);
    incrementPan = T1_READ_8(sBattleAnimScriptPtr + 3);
    incrementPanArg = T1_READ_8(sBattleAnimScriptPtr + 4);
    framesToWait = T1_READ_8(sBattleAnimScriptPtr + 5);

    currentPan = BattleAnimAdjustPanning(currentPanArg);
    targetPan = BattleAnimAdjustPanning(incrementPan);
    incrementPan = CalculatePanIncrement(currentPan, targetPan, incrementPanArg);
    taskId = CreateTask(Task_PanFromInitialToTarget, 1);
    gTasks[taskId].tInitialPan = currentPan;
    gTasks[taskId].tTargetPan = targetPan;
    gTasks[taskId].tIncrementPan = incrementPan;
    gTasks[taskId].tFramesToWait = framesToWait;
    gTasks[taskId].tCurrentPan = currentPan;

    PlaySE12WithPanning(songNum, currentPan);

    gAnimSoundTaskCount++;
    sBattleAnimScriptPtr += 6;
}

void Task_PanFromInitialToTarget(u8 taskId)
{
    bool32 destroyTask = FALSE;
    if (gTasks[taskId].tFrameCounter++ >= gTasks[taskId].tFramesToWait)
    {
        s16 pan;
        s16 initialPanning, targetPanning, currentPan, incrementPan;

        gTasks[taskId].tFrameCounter = 0;
        initialPanning = gTasks[taskId].tInitialPan;
        targetPanning = gTasks[taskId].tTargetPan;
        currentPan = gTasks[taskId].tCurrentPan;
        incrementPan = gTasks[taskId].tIncrementPan;
        pan = currentPan + incrementPan;
        gTasks[taskId].tCurrentPan = pan;

        if (incrementPan == 0) // If we're not incrementing, just cancel the task immediately
        {
            destroyTask = TRUE;
        }
        else if (initialPanning < targetPanning) // Panning increasing
        {
            if (pan >= targetPanning) // Target reached
                destroyTask = TRUE;
        }
        else // Panning decreasing
        {
            if (pan <= targetPanning) // Target reached
                destroyTask = TRUE;
        }

        if (destroyTask)
        {
            pan = targetPanning;
            DestroyTask(taskId);
            gAnimSoundTaskCount--;
        }

        SE12PanpotControl(pan);
    }
}

static void Cmd_panse_adjustnone(void)
{
    u16 songId;
    s8 currentPan, targetPan, incrementPan;
    u8 framesToWait;
    u8 taskId;

    sBattleAnimScriptPtr++;
    songId = T1_READ_16(sBattleAnimScriptPtr);
    currentPan = T1_READ_8(sBattleAnimScriptPtr + 2);
    targetPan = T1_READ_8(sBattleAnimScriptPtr + 3);
    incrementPan = T1_READ_8(sBattleAnimScriptPtr + 4);
    framesToWait = T1_READ_8(sBattleAnimScriptPtr + 5);

    taskId = CreateTask(Task_PanFromInitialToTarget, 1);
    gTasks[taskId].tInitialPan = currentPan;
    gTasks[taskId].tTargetPan = targetPan;
    gTasks[taskId].tIncrementPan = incrementPan;
    gTasks[taskId].tFramesToWait = framesToWait;
    gTasks[taskId].tCurrentPan = currentPan;

    PlaySE12WithPanning(songId, currentPan);

    gAnimSoundTaskCount++;
    sBattleAnimScriptPtr += 6;
}

static void Cmd_panse_adjustall(void)
{
    u16 songId;
    u8 targetPanArg, incrementPanArg, framesToWait, currentPanArg;
    s8 currentPan, targetPan, incrementPan;
    u8 taskId;

    sBattleAnimScriptPtr++;
    songId = T1_READ_16(sBattleAnimScriptPtr);
    currentPanArg = T1_READ_8(sBattleAnimScriptPtr + 2);
    targetPanArg = T1_READ_8(sBattleAnimScriptPtr + 3);
    incrementPanArg = T1_READ_8(sBattleAnimScriptPtr + 4);
    framesToWait = T1_READ_8(sBattleAnimScriptPtr + 5);

    currentPan = BattleAnimAdjustPanning2(currentPanArg);
    targetPan = BattleAnimAdjustPanning2(targetPanArg);
    incrementPan = BattleAnimAdjustPanning2(incrementPanArg);

    taskId = CreateTask(Task_PanFromInitialToTarget, 1);
    gTasks[taskId].data[0] = currentPan;
    gTasks[taskId].data[1] = targetPan;
    gTasks[taskId].data[2] = incrementPan;
    gTasks[taskId].data[3] = framesToWait;
    gTasks[taskId].data[4] = currentPan;

    PlaySE12WithPanning(songId, currentPan);

    gAnimSoundTaskCount++;
    sBattleAnimScriptPtr += 6;
}

#undef tInitialPan
#undef tTargetPan
#undef tIncrementPan
#undef tFramesToWait
#undef tCurrentPan
#undef tFrameCounter

#define tSongId         data[0]
#define tPanning        data[1]
#define tFramesToWait   data[2]
#define tNumberOfPlays  data[3]
#define tFrameCounter   data[8]

static void Cmd_loopsewithpan(void)
{
    u16 songId;
    s8 panningArg, panning;
    u8 framesToWait, numberOfPlays;
    u8 taskId;

    sBattleAnimScriptPtr++;
    songId = T1_READ_16(sBattleAnimScriptPtr);
    panningArg = T1_READ_8(sBattleAnimScriptPtr + 2);
    framesToWait = T1_READ_8(sBattleAnimScriptPtr + 3);
    numberOfPlays = T1_READ_8(sBattleAnimScriptPtr + 4);
    panning = BattleAnimAdjustPanning(panningArg);

    taskId = CreateTask(Task_LoopAndPlaySE, 1);
    gTasks[taskId].tSongId = songId;
    gTasks[taskId].tPanning = panning;
    gTasks[taskId].tFramesToWait = framesToWait;
    gTasks[taskId].tNumberOfPlays = numberOfPlays;
    gTasks[taskId].tFrameCounter = framesToWait;
    gTasks[taskId].func(taskId);

    gAnimSoundTaskCount++;
    sBattleAnimScriptPtr += 5;
}

static void Task_LoopAndPlaySE(u8 taskId)
{
    if (gTasks[taskId].tFrameCounter++ >= gTasks[taskId].tFramesToWait)
    {
        u16 songId;
        s8 panning;
        u8 numberOfPlays;

        gTasks[taskId].tFrameCounter = 0;
        songId = gTasks[taskId].tSongId;
        panning = gTasks[taskId].tPanning;
        numberOfPlays = --gTasks[taskId].tNumberOfPlays;
        PlaySE12WithPanning(songId, panning);
        if (numberOfPlays == 0)
        {
            DestroyTask(taskId);
            gAnimSoundTaskCount--;
        }
    }
}

#undef tSongId
#undef tPanning
#undef tFramesToWait
#undef tNumberOfPlays
#undef tFrameCounter

#define tSongId         data[0]
#define tPanning        data[1]
#define tFramesToWait   data[2]

static void Cmd_waitplaysewithpan(void)
{
    u16 songId;
    s8 panningArg, panning;
    u8 framesToWait;
    u8 taskId;

    sBattleAnimScriptPtr++;
    songId = T1_READ_16(sBattleAnimScriptPtr);
    panningArg = T1_READ_8(sBattleAnimScriptPtr + 2);
    framesToWait = T1_READ_8(sBattleAnimScriptPtr + 3);
    panning = BattleAnimAdjustPanning(panningArg);

    taskId = CreateTask(Task_WaitAndPlaySE, 1);
    gTasks[taskId].tSongId = songId;
    gTasks[taskId].tPanning = panning;
    gTasks[taskId].tFramesToWait = framesToWait;

    gAnimSoundTaskCount++;
    sBattleAnimScriptPtr += 4;
}

static void Task_WaitAndPlaySE(u8 taskId)
{
    if (gTasks[taskId].tFramesToWait-- <= 0)
    {
        PlaySE12WithPanning(gTasks[taskId].tSongId, gTasks[taskId].tPanning);
        DestroyTask(taskId);
        gAnimSoundTaskCount--;
    }
}

#undef tSongId
#undef tPanning
#undef tFramesToWait

// Creates a sound task.
// arg0: sound task function
// arg1: s16[] gBattleAnimArgs
static void Cmd_createsoundtask(void)
{
    TaskFunc func;
    u8 numArgs, taskId;
    s32 i;

    sBattleAnimScriptPtr++;
    func = (TaskFunc)T2_READ_32(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr += 4;
    numArgs = T1_READ_8(sBattleAnimScriptPtr);
    sBattleAnimScriptPtr++;
    for (i = 0; i < numArgs; i++)
    {
        gBattleAnimArgs[i] = T1_READ_16(sBattleAnimScriptPtr);
        sBattleAnimScriptPtr += 2;
    }
    taskId = CreateTask(func, 1);
    func(taskId);
    gAnimSoundTaskCount++;
}

// Wait for sound effect to end.
static void Cmd_waitsound(void)
{
    if (gAnimSoundTaskCount != 0)
    {
        gSoundAnimFramesToWait = 0;
        sAnimFramesToWait = 1;
    }
    else if (IsSEPlaying())
    {
        if (++gSoundAnimFramesToWait > 90)
        {
            m4aMPlayStop(&gMPlayInfo_SE1);
            m4aMPlayStop(&gMPlayInfo_SE2);
            gSoundAnimFramesToWait = 0;
        }
        else
        {
            sAnimFramesToWait = 1;
        }
    }
    else
    {
        gSoundAnimFramesToWait = 0;
        sBattleAnimScriptPtr++;
        sAnimFramesToWait = 0;
    }
}

// Jump to animation based on gBattleAnimArgs[index] value.
// arg0: gBattleAnimArgs[] argument index
// arg1: value
// arg2: animation script
static void Cmd_jumpargeq(void)
{
    u8 argId;
    s16 valueToCheck;

    sBattleAnimScriptPtr++;
    argId = T1_READ_8(sBattleAnimScriptPtr);
    valueToCheck = T1_READ_16(sBattleAnimScriptPtr + 1);

    if (valueToCheck == gBattleAnimArgs[argId])
        sBattleAnimScriptPtr = T2_READ_PTR(sBattleAnimScriptPtr + 3);
    else
        sBattleAnimScriptPtr += 7;
}

// If using move in contest, go to specific animation script.
// arg0: animation script
static void Cmd_jumpifcontest(void)
{
    sBattleAnimScriptPtr++;
    if (IsContest())
        sBattleAnimScriptPtr = T2_READ_PTR(sBattleAnimScriptPtr);
    else
        sBattleAnimScriptPtr += 4;
}

static void Cmd_splitbgprio(void)
{
    u8 wantedBattler;
    u8 battler;
    u8 battlerPosition;

    wantedBattler = T1_READ_8(sBattleAnimScriptPtr + 1);
    sBattleAnimScriptPtr += 2;

    if (wantedBattler != 0)
        battler = gBattleAnimTarget;
    else
        battler = gBattleAnimAttacker;

    battlerPosition = GetBattlerPosition(battler);
    if (!IsContest() && (battlerPosition == 0 || battlerPosition == 3))
    {
        REG_BG1CNT_BITFIELD.priority = 1;
        REG_BG2CNT_BITFIELD.priority = 2;
    }
}

static void Cmd_splitbgprio_all(void)
{
    sBattleAnimScriptPtr++;
    if (!IsContest())
    {
        REG_BG1CNT_BITFIELD.priority = 1;
        REG_BG2CNT_BITFIELD.priority = 2;
    }
}

static void Cmd_splitbgprio_foes(void)
{
    u8 wantedBattler;
    u8 battlerPosition;
    u8 battler;

    wantedBattler = T1_READ_8(sBattleAnimScriptPtr + 1);
    sBattleAnimScriptPtr += 2;
    if (GetBattlerSide(gBattleAnimAttacker) != GetBattlerSide(gBattleAnimTarget))
    {
        if (wantedBattler != 0)
            battler = gBattleAnimTarget;
        else
            battler = gBattleAnimAttacker;
        battlerPosition = GetBattlerPosition(battler);
        if (!IsContest() && (battlerPosition == 0 || battlerPosition == 3))
        {
            REG_BG1CNT_BITFIELD.priority = 1;
            REG_BG2CNT_BITFIELD.priority = 2;
        }
    }
}

// Sets sprite to be invisible.
// arg0: battler sprite ID
static void Cmd_invisible(void)
{
    u8 spriteId;

    spriteId = GetAnimBattlerSpriteId(T1_READ_8(sBattleAnimScriptPtr + 1));
    if (spriteId != 0xFF)
        gSprites[spriteId].invisible = TRUE;

    sBattleAnimScriptPtr += 2;
}

// Sets aprite to be visible.
// arg0: battler sprite ID
static void Cmd_visible(void)
{
    u8 spriteId;

    spriteId = GetAnimBattlerSpriteId(T1_READ_8(sBattleAnimScriptPtr + 1));
    if (spriteId != 0xFF)
        gSprites[spriteId].invisible = FALSE;

    sBattleAnimScriptPtr += 2;
}

static void Cmd_teamattack_moveback(void)
{
    u8 wantedBattler;
    u8 priorityRank;
    u8 spriteId;

    wantedBattler = T1_READ_8(sBattleAnimScriptPtr + 1);
    sBattleAnimScriptPtr += 2;
    if (!IsContest() && IsDoubleBattle()
     && GetBattlerSide(gBattleAnimAttacker) == GetBattlerSide(gBattleAnimTarget))
    {
        if (wantedBattler == 0)
        {
            priorityRank = GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker);
            spriteId = GetAnimBattlerSpriteId(0);
        }
        else
        {
            priorityRank = GetBattlerSpriteBGPriorityRank(gBattleAnimTarget);
            spriteId = GetAnimBattlerSpriteId(1);
        }
        if (spriteId != 0xFF)
        {
            gSprites[spriteId].invisible = FALSE;
            if (priorityRank == 2)
                gSprites[spriteId].oam.priority = 3;
            if (priorityRank == 1)
                ResetBattleAnimBg(0);
            else
                ResetBattleAnimBg(1);
        }
    }
}

static void Cmd_teamattack_movefwd(void)
{
    u8 wantedBattler;
    u8 priorityRank;
    u8 spriteId;

    wantedBattler = T1_READ_8(sBattleAnimScriptPtr  + 1);
    sBattleAnimScriptPtr += 2;
    if (!IsContest() && IsDoubleBattle()
     && GetBattlerSide(gBattleAnimAttacker) == GetBattlerSide(gBattleAnimTarget))
    {
        if (wantedBattler == 0)
        {
            priorityRank = GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker);
            spriteId = GetAnimBattlerSpriteId(0);
        }
        else
        {
            priorityRank = GetBattlerSpriteBGPriorityRank(gBattleAnimTarget);
            spriteId = GetAnimBattlerSpriteId(1);
        }
        if (spriteId != 0xFF && priorityRank == 2)
        {
            gSprites[spriteId].oam.priority = 2;
        }
    }
}

// Cease playing sounds.
static void Cmd_stopsound(void)
{
    m4aMPlayStop(&gMPlayInfo_SE1);
    m4aMPlayStop(&gMPlayInfo_SE2);
    sBattleAnimScriptPtr++;
}
