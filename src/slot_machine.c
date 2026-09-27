#include "global.h"
#include "constants/songs.h"
#include "strings2.h"
#include "overworld.h"
#include "menu_cursor.h"
#include "field_effect.h"
#include "random.h"
#include "sound.h"
#include "main.h"
#include "slot_machine.h"
#include "string_util.h"
#include "decompress.h"
#include "trig.h"
#include "graphics.h"
#include "palette.h"
#include "util.h"
#include "text.h"
#include "menu.h"
#include "ewram.h"

enum
{
    SLOT_MACHINE_TAG_7_RED,
    SLOT_MACHINE_TAG_7_BLUE,
    SLOT_MACHINE_TAG_AZURILL,
    SLOT_MACHINE_TAG_LOTAD,
    SLOT_MACHINE_TAG_CHERRY,
    SLOT_MACHINE_TAG_POWER,
    SLOT_MACHINE_TAG_REPLAY
};

enum
{
    SLOT_MACHINE_MATCHED_1CHERRY,
    SLOT_MACHINE_MATCHED_2CHERRY,
    SLOT_MACHINE_MATCHED_REPLAY,
    SLOT_MACHINE_MATCHED_LOTAD,
    SLOT_MACHINE_MATCHED_AZURILL,
    SLOT_MACHINE_MATCHED_POWER,
    SLOT_MACHINE_MATCHED_777_MIXED,
    SLOT_MACHINE_MATCHED_777_RED,
    SLOT_MACHINE_MATCHED_777_BLUE,
    SLOT_MACHINE_MATCHED_NONE
};

struct SlotMachineEwramStruct
{
    /*0x00*/ u8 state;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 pikaPower;
    /*0x03*/ u8 unk03;
    /*0x04*/ u8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u16 matchedSymbols;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ s16 coins;
    /*0x0E*/ s16 payout;
    /*0x10*/ s16 unk10;
    /*0x12*/ s16 bet;
    /*0x14*/ s16 unk14;
    /*0x16*/ s16 unk16;
    /*0x18*/ s16 unk18;
    /*0x1A*/ s16 unk1A;
    /*0x1C*/ s16 unk1C[3];
    /*0x22*/ u16 unk22[3];
    /*0x28*/ s16 reelPositions[3];
    /*0x2E*/ s16 unk2E[3];
    /*0x34*/ s16 unk34[3];
    /*0x3A*/ u8 reelTasks[3];
    /*0x3D*/ u8 unk3D;
    /*0x3E*/ u8 unk3E;
    /*0x3F*/ u8 unk3F;
    /*0x40*/ u8 unk40;
    /*0x41*/ u8 unk41;
    /*0x42*/ u8 unk42;
    /*0x43*/ u8 unk43;
    /*0x44*/ u8 unk44[5];
    /*0x49*/ u8 unk49[2];
    /*0x49*/ u8 unk4B[3];
    /*0x4E*/ u8 unk4E[2];
    /*0x50*/ u8 unk50[2];
    /*0x52*/ u8 unk52[2];
    /*0x54*/ u8 unk54[4];
    /*0x58*/ u16 win0h;
    /*0x5a*/ u16 win0v;
    /*0x5c*/ u16 winIn;
    /*0x5e*/ u16 winOut;
    /*0x60*/ u16 backupMapMusic;
    /*0x64*/ MainCallback prevMainCb;
#if DEBUG
             u32 unk68;
             u32 unk6C;
             u32 unk70;
             u32 unk74;
             u32 unk78;
             u32 unk7C;
             u32 unk80;
             u32 unk84;
             u32 unk88;
             u32 unk8C;
             s32 unk90;
#endif
};

struct DigitalDisplaySprite
{
    /*0x00*/ u8 spriteTemplateId;
    /*0x01*/ u8 dispInfoId;
    /*0x02*/ s16 spriteId;
};

#if ENGLISH
#define SLOTMACHINE_GFX_TILES 233
#elif GERMAN
#define SLOTMACHINE_GFX_TILES 236
#endif

static void CB2_SlotMachineSetup(void);
static void CB2_SlotMachineLoop(void);
static void PlaySlotMachine_Internal(u8 arg0, MainCallback cb);
static void SlotMachineDummyTask(u8 taskId);
static void SlotMachineSetup_0_0(void);
static void SlotMachineSetup_6_2(void);
static void SlotMachineSetup_1_0(void);
static void SlotMachineSetup_2_0(void);
static void SlotMachineSetup_2_1(void);
static void SlotMachineSetup_0_1(void);
static void SlotMachineSetup_3_0(void);
static void SlotMachineSetup_4_0(void);
static void SlotMachineSetup_5_0(void);
static void SlotMachineSetup_6_0(void);
static void SlotMachineSetup_6_1(void);
static void CreateSlotMachineTasks(void);
static void Task_SlotMachine(u8 taskId);
static bool8 SlotTask_UnfadeScreen(struct Task *task);
static bool8 SlotTask_WaitUnfade(struct Task *task);
static bool8 SlotTask_ReadyNewSpin(struct Task *task);
static bool8 SlotTask_ReadyNewReelTimeSpin(struct Task *task);
static bool8 SlotTask_AskInsertBet(struct Task *task);
static bool8 SlotTask_HandleBetInput(struct Task *task);
static bool8 SlotTask_PrintMsg_Need3Coins(struct Task *task);
static bool8 SlotTask_WaitMsg_Need3Coins(struct Task *task);
static bool8 SlotTask_WaitInfoBox(struct Task *task);
static bool8 SlotTask_StartSpin(struct Task *task);
static bool8 SlotTask_StartReelTimeSpin(struct Task *task);
static bool8 SlotTask_ResetBiasFailure(struct Task *task);
static bool8 SlotTask_WaitReelStop(struct Task *task);
static bool8 SlotTask_WaitAllReelsStop(struct Task *task);
bool8 SlotTask_CheckMatches(struct Task *task);
static bool8 SlotTask_WaitPayout(struct Task *task);
static bool8 SlotTask_EndPayout(struct Task *task);
static bool8 SlotTask_MatchedPower(struct Task *task);
static bool8 SlotTask_WaitReelTimeAnim(struct Task *task);
static bool8 SlotTask_ResetBetTiles(struct Task *task);
static bool8 SlotTask_NoMatches(struct Task *task);
static bool8 SlotTask_AskQuit(struct Task *task);
static bool8 SlotTask_HandleQuitInput(struct Task *task);
static bool8 SlotTask_PrintMsg_MaxCoins(struct Task *task);
static bool8 SlotTask_WaitMsg_MaxCoins(struct Task *task);
static bool8 SlotTask_PrintMsg_NoMoreCoins(struct Task *task);
static bool8 SlotTask_WaitMsg_NoMoreCoins(struct Task *task);
static bool8 SlotTask_EndGame(struct Task *task);
static bool8 SlotTask_FreeDataStructures(struct Task *task);
#if DEBUG
static bool8 debug_sub_8116E74(struct Task *);
#endif
static void DrawMachineBias(void);
static void ResetBiasFailure(void);
static bool8 ShouldTrySpecialBias(void);
static u8 TrySelectBias_Special(void);
static u16 ReelTimeSpeed(void);
static u8 TrySelectBias_Regular(void);
static void CheckMatch(void);
static void CheckMatch_CenterRow(void);
static void CheckMatch_TopAndBottom(void);
static void CheckMatch_Diagonals(void);
static u8 GetMatchFromSymbolsInRow(u8 c1, u8 c2, u8 c3);
static void AwardPayout(void);
static void Task_Payout(u8 taskId);
static bool8 IsFinalTask_Task_Payout(void);
static bool8 PayoutTask_Init(struct Task *task);
static bool8 PayoutTask_GivePayout(struct Task *task);
static bool8 PayoutTask_Free(struct Task *task);
static u8 GetSymbolAtRest(u8 x, s16 y);
static void CreateReelTasks(void);
static void SpinSlotReel(u8 a0);
static void StopSlotReel(u8 a0);
static bool8 IsSlotReelMoving(u8 a0);
static void Task_Reel(u8 taskId);
static bool8 ReelTask_StayStill(struct Task *task);
static bool8 ReelTask_Spin(struct Task *task);
static bool8 ReelTask_DecideStop(struct Task *task);
static bool8 ReelTask_MoveToStop(struct Task *task);
static bool8 ReelTask_ShakingStop(struct Task *task);
static bool8 DecideStop_Bias_Reel1(void);
static bool8 DecideStop_Bias_Reel1_Bet1(u8 a0, u8 a1);
static bool8 DecideStop_Bias_Reel1_Bet2or3(u8 a0, u8 a1);
static bool8 DecideStop_Bias_Reel2(void);
static bool8 DecideStop_Bias_Reel2_Bet1or2(void);
static bool8 DecideStop_Bias_Reel2_Bet3(void);
static bool8 DecideStop_Bias_Reel3(void);
static bool8 DecideStop_Bias_Reel3_Bet1or2(u8 a0);
static bool8 DecideStop_Bias_Reel3_Bet3(u8 a0);
static void DecideStop_NoBias_Reel1(void);
static void DecideStop_NoBias_Reel2(void);
static void DecideStop_NoBias_Reel2_Bet1(void);
static void DecideStop_NoBias_Reel2_Bet2(void);
static void DecideStop_NoBias_Reel2_Bet3(void);
static void DecideStop_NoBias_Reel3(void);
static void DecideStop_NoBias_Reel3_Bet1(void);
static void DecideStop_NoBias_Reel3_Bet2(void);
static void DecideStop_NoBias_Reel3_Bet3(void);
static void PressStopReelButton(u8 a0);
static void Task_PressStopReelButton(u8 taskId);
static void LightenBetTiles(u8 a0);
static void StopReelButton_Press(struct Task *task, u8 taskId);
static void StopReelButton_Wait(struct Task *task, u8 taskId);
static void StopReelButton_Unpress(struct Task *task, u8 taskId);
static void DarkenBetTiles(u8 a0);
static void CreateInvisibleFlashMatchLineSprites(void);
static void FlashMatchLine(u8 a0);
static bool8 IsMatchLineDoneFlashingBeforePayout(void);
static bool8 TryStopMatchLinesFlashing(void);
static bool8 TryStopMatchLineFlashing(u8 spriteId);
static void SpriteCB_FlashMatchingLines(struct Sprite *sprite);
static void FlashSlotMachineLights(void);
static bool8 TryStopSlotMachineLights(void);
static void Task_FlashSlotMachineLights(u8 taskId);
static void CreatePikaPowerBoltTask(void);
static void AddPikaPowerBolt(u8 pikaPower);
static bool8 IsPikaPowerBoltAnimating(void);
static void Task_CreatePikaPowerBolt(u8 taskId);
static void PikaPowerBolt_Idle(struct Task *task);
static void PikaPowerBolt_AddBolt(struct Task *task);
static void PikaPowerBolt_WaitAnim(struct Task *task);
static void PikaPowerBolt_ClearAll(struct Task *task);
static void ResetPikaPowerBoltTask(struct Task *task);
static void LoadPikaPowerMeter(u8 pikaPower);
static void BeginReelTime(void);
static bool8 IsReelTimeTaskDone(void);
static void Task_ReelTime(u8 taskId);
static void ReelTime_Init(struct Task *task);
static void ReelTime_WindowEnter(struct Task *task);
static void ReelTime_WaitStartPikachu(struct Task *task);
static void ReelTime_PikachuSpeedUp1(struct Task *task);
static void ReelTime_PikachuSpeedUp2(struct Task *task);
static void ReelTime_WaitReel(struct Task *task);
static void ReelTime_CheckExplode(struct Task *task);
static void ReelTime_LandOnOutcome(struct Task *task);
static void ReelTime_PikachuReact(struct Task *task);
static void ReelTime_WaitClearPikaPower(struct Task *task);
static void ReelTime_CloseWindow(struct Task *task);
static void ReelTime_DestroySprites(struct Task *task);
static void ReelTime_SetReelSpeed(struct Task *task);
static void ReelTime_EndSuccess(struct Task *task);
static void ReelTime_ExplodeMachine(struct Task *task);
static void ReelTime_WaitExplode(struct Task *task);
static void ReelTime_WaitSmoke(struct Task *task);
static void ReelTime_EndFailure(struct Task *task);
static void LoadReelTimeWindowTilemap(s16 a0, s16 a1);
static void ClearReelTimeWindowTilemap(s16 a0);
static void OpenInfoBox(u8 a0);
static bool8 IsInfoBoxClosed(void);
static void Task_InfoBox(u8 taskId);
static void InfoBox_FadeIn(struct Task *task);
static void InfoBox_WaitFade(struct Task *task);
static void InfoBox_DrawWindowAndText(struct Task *task);
static void InfoBox_WaitInput(struct Task *task);
static void InfoBox_RestoreSlotMachineDisplay(struct Task *task);
static void InfoBox_FreeTask(struct Task *task);
static void CreateDigitalDisplayTask(void);
static void CreateDigitalDisplayScene(u8 arg0);
static bool8 IsDigitalDisplayAnimFinished(void);
static void DigitalDisplay_Idle(struct Task *task);
static void Task_DigitalDisplay(u8 taskId);
static void CreateReelSymbolSprites(void);
static void CreateCreditPayoutNumberSprites(void);
static void CreateCoinNumberSprite(s16 x, s16 y, u8 a2, s16 a3);
static void CreateReelBackgroundSprite(void);
static void CreateReelTimePikachuSprite(void);
static void DestroyReelTimePikachuSprite(void);
static void CreateReelTimeMachineSprites(void);
static void CreateBrokenReelTimeMachineSprite(void);
static void CreateReelTimeNumberSprites(void);
static void CreateReelTimeShadowSprites(void);
static void CreateReelTimeNumberGapSprite(void);
static void DestroyReelTimeMachineSprites(void);
static void DestroyReelTimeShadowSprites(void);
static void DestroyBrokenReelTimeMachineSprite(void);
static void CreateReelTimeBoltSprites(void);
static void SetReelTimeBoltDelay(s16 a0);
static void DestroyReelTimeBoltSprites(void);
static void CreateReelTimePikachuAuraSprites(void);
static void SetReelTimePikachuAuraFlashDelay(s16 a0);
static void DestroyReelTimePikachuAuraSprites(void);
static void CreateReelTimeExplosionSprite(void);
static void DestroyReelTimeExplosionSprite(void);
static void CreateReelTimeDuckSprites(void);
static void DestroyReelTimeDuckSprites(void);
static void CreateReelTimeSmokeSprite(void);
static bool8 IsReelTimeSmokeAnimFinished(void);
static void DestroyReelTimeSmokeSprite(void);
static u8 CreatePikaPowerBoltSprite(s16 x, s16 y);
static void DestroyPikaPowerBoltSprite(u8 spriteId);
static u8 CreateDigitalDisplaySprite(u8 templateIdx, SpriteCallback callback, s16 x, s16 y, s16 a4);
static void LoadSlotMachineGfx(void);
static void LoadReelBackground(void);
static void LoadMenuGfx(void);
static void LoadMenuAndReelOverlayTilemaps(void);
static void SetReelButtonTilemap(s16 arg0, u16 arg1, u16 arg2, u16 arg3, u16 arg4);
static void LoadInfoBoxTilemap(void);

#if DEBUG
static void debug_sub_811B5D0(void);
static void debug_sub_811B620(void);
static void debug_sub_811B5B4(s32 *, s32);
static void debug_sub_811B894(void);
static u8 debug_sub_811B634(void);
static void debug_sub_811B654(u8 taskId);
#endif

#if DEBUG
static u8 unk_debug_bss_1_0;
static u8 unk_debug_bss_1_1;
static u8 unk_debug_bss_1_2;
static u8 unk_debug_bss_1_3;
static u8 unk_debug_bss_1_4;
static u32 unk_debug_bss_1_8;
#endif

static struct SlotMachineEwramStruct *const sSlotMachine = eSlotMachine;

static const struct DigitalDisplaySprite *const sDigitalDisplayScenes[];
static const u16 gPalette_83EDE24[];
static const u8 sSpecialDrawOdds[][3];
static const u8 sBiasSymbols[];
static const u16 sBiasesSpecial[];
static const u16 sBiasesRegular[];

void PlaySlotMachine(u8 arg0, MainCallback cb)
{
#if DEBUG
    unk_debug_bss_1_1 = 0;
#endif
    PlaySlotMachine_Internal(arg0, cb);
    SetMainCallback2(CB2_SlotMachineSetup);
}

#if DEBUG
void debug_sub_811609C(u8 a, void (*func)(void))
{
    unk_debug_bss_1_1 = 1;
    PlaySlotMachine_Internal(a, func);
    SetMainCallback2(CB2_SlotMachineSetup);
}
#endif

static void CB2_SlotMachineSetup(void)
{
    switch (gMain.state)
    {
        case 0:
            SlotMachineSetup_0_0();
            SlotMachineSetup_0_1();
            gMain.state++;
            break;
        case 1:
            SlotMachineSetup_1_0();
            gMain.state++;
            break;
        case 2:
            SlotMachineSetup_2_0();
            SlotMachineSetup_2_1();
            gMain.state++;
            break;
        case 3:
            SlotMachineSetup_3_0();
            gMain.state++;
            break;
        case 4:
            SlotMachineSetup_4_0();
            gMain.state++;
            break;
        case 5:
            SlotMachineSetup_5_0();
            gMain.state++;
            break;
        case 6:
            SlotMachineSetup_6_0();
            SlotMachineSetup_6_1();
            SlotMachineSetup_6_2();
            SetMainCallback2(CB2_SlotMachineLoop);
            break;
    }
}

static void CB2_SlotMachineLoop(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void SlotMachine_VBlankCallback(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    REG_WIN0H = sSlotMachine->win0h;
    REG_WIN0V = sSlotMachine->win0v;
    REG_WININ = sSlotMachine->winIn;
    REG_WINOUT = sSlotMachine->winOut;
}

static void PlaySlotMachine_Internal(u8 arg0, MainCallback cb)
{
    struct Task *task = gTasks + CreateTask(SlotMachineDummyTask, 0xFF);
    task->data[0] = arg0;
    StoreWordInTwoHalfwords(task->data + 1, (intptr_t)cb);
}

static void sub_81019EC(void)
{
    struct Task *task = gTasks + FindTaskIdByFunc(SlotMachineDummyTask);
    sSlotMachine->unk01 = task->data[0];
    LoadWordFromTwoHalfwords((u16 *)(task->data + 1), (u32 *)&sSlotMachine->prevMainCb);
}

static void SlotMachineDummyTask(u8 taskId)
{
}

static void SlotMachineSetup_0_0(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    REG_DISPCNT = 0;
}

static void SlotMachineSetup_6_2(void)
{
    u16 imeBak;
    SetVBlankCallback(SlotMachine_VBlankCallback);
    imeBak = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = imeBak;
    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
    REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON;
}

static void SlotMachineSetup_1_0(void)
{
    DmaClearLarge16(3, (u16 *)(BG_VRAM), BG_VRAM_SIZE, 0x1000);
}

static void SlotMachineSetup_2_0(void)
{
    DmaClear16(3, (u16 *)OAM, OAM_SIZE);
}

static void SlotMachineSetup_2_1(void)
{
    REG_BG0CNT = 0;
    REG_BG1CNT = 0;
    REG_BG2CNT = 0;
    REG_BG3CNT = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_SCREENBASE(31) | BGCNT_CHARBASE(2);
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(28);
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(29);
    REG_BG3CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(30);
    REG_WININ = 0x3f;
    REG_WINOUT = 0x3f;
    REG_BLDCNT = BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_OBJ;
    REG_BLDALPHA = 0x809;
}

static const s16 sInitialReelPositions[][2];

static void SlotMachineSetup_0_1(void)
{
    u8 i;

    sub_81019EC();
    sSlotMachine->state = 0;
    sSlotMachine->pikaPower = 0;
    sSlotMachine->unk03 = Random() & 1;
    sSlotMachine->unk04 = 0;
    sSlotMachine->matchedSymbols = 0;
    sSlotMachine->unk0A = 0;
    sSlotMachine->unk0B = 0;
    sSlotMachine->coins = gSaveBlock1.coins;
    sSlotMachine->payout = 0;
    sSlotMachine->unk10 = 0;
    sSlotMachine->bet = 0;
    sSlotMachine->unk18 = 0;
    sSlotMachine->unk1A = 8;
    sSlotMachine->win0h = 0xf0;
    sSlotMachine->win0v = 0xa0;
    sSlotMachine->winIn = 0x3f;
    sSlotMachine->winOut = 0x3f;
    sSlotMachine->backupMapMusic = GetCurrentMapMusic();
    for (i = 0; i < 3; i++)
    {
        sSlotMachine->unk22[i] = 0;
        sSlotMachine->reelPositions[i] = sInitialReelPositions[i][sSlotMachine->unk03] % 21;
        sSlotMachine->unk1C[i] = 0x1f8 - sSlotMachine->reelPositions[i] * 24;
        sSlotMachine->unk1C[i] %= 0x1f8;
    }
#if DEBUG
    debug_sub_811B5D0();
    if (unk_debug_bss_1_1 != 0)
        sSlotMachine->coins = 1000;
#endif
}

static void SlotMachineSetup_3_0(void)
{
    Text_LoadWindowTemplate(&gWindowTemplate_81E7128);
    InitMenuWindow(&gWindowTemplate_81E7128);
}

static void SlotMachineSetup_4_0(void)
{
    ResetPaletteFade();
    ResetSpriteData();
    gOamLimit = 128;
    FreeAllSpritePalettes();
    ResetTasks();
}

static void SlotMachineSetup_5_0(void)
{
    LoadMenuGfx();
    LoadMenuAndReelOverlayTilemaps();
    LoadSlotMachineGfx();
}

static void SlotMachineSetup_6_0(void)
{
    CreateReelSymbolSprites();
    CreateCreditPayoutNumberSprites();
    CreateInvisibleFlashMatchLineSprites();
    CreateReelBackgroundSprite();
}

static void SlotMachineSetup_6_1(void)
{
    CreatePikaPowerBoltTask();
    CreateReelTasks();
    CreateDigitalDisplayTask();
    CreateSlotMachineTasks();
}

static void CreateSlotMachineTasks(void)
{
    Task_SlotMachine(CreateTask(Task_SlotMachine, 0));
}

static bool8 (*const gUnknown_083ECAAC[])(struct Task *task) =
{
    SlotTask_UnfadeScreen,
    SlotTask_WaitUnfade,
    SlotTask_ReadyNewSpin,
    SlotTask_ReadyNewReelTimeSpin,
    SlotTask_AskInsertBet,
    SlotTask_HandleBetInput,
    SlotTask_PrintMsg_Need3Coins,
    SlotTask_WaitMsg_Need3Coins,
    SlotTask_WaitInfoBox,
    SlotTask_StartSpin,
    SlotTask_StartReelTimeSpin,
    SlotTask_ResetBiasFailure,
    SlotTask_WaitReelStop,
    SlotTask_WaitAllReelsStop,
    SlotTask_CheckMatches,
    SlotTask_WaitPayout,
    SlotTask_EndPayout,
    SlotTask_MatchedPower,
    SlotTask_WaitReelTimeAnim,
    SlotTask_ResetBetTiles,
    SlotTask_NoMatches,
    SlotTask_AskQuit,
    SlotTask_HandleQuitInput,
    SlotTask_PrintMsg_MaxCoins,
    SlotTask_WaitMsg_MaxCoins,
    SlotTask_PrintMsg_NoMoreCoins,
    SlotTask_WaitMsg_NoMoreCoins,
    SlotTask_EndGame,
    SlotTask_FreeDataStructures,
#if DEBUG
    debug_sub_8116E74,
#endif
};

static void Task_SlotMachine(u8 taskId)
{
    while (gUnknown_083ECAAC[sSlotMachine->state](gTasks + taskId))
        ;
}

static bool8 SlotTask_UnfadeScreen(struct Task *task)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    LoadPikaPowerMeter(sSlotMachine->pikaPower);
    sSlotMachine->state++;
    return FALSE;
}

static bool8 SlotTask_WaitUnfade(struct Task *task)
{
    if (!gPaletteFade.active)
        sSlotMachine->state++;
    return FALSE;
}

static bool8 SlotTask_ReadyNewSpin(struct Task *task)
{
    sSlotMachine->payout = 0;
    sSlotMachine->bet = 0;
    sSlotMachine->unk18 = 0;
    sSlotMachine->unk04 &= 0xc0;
    sSlotMachine->state = 4;
    if (sSlotMachine->coins <= 0)
    {
        sSlotMachine->state = 25;
    }
    else if (sSlotMachine->unk0A)
    {
        sSlotMachine->state = 3;
        CreateDigitalDisplayScene(4);
    }
    return TRUE;
}

static bool8 SlotTask_ReadyNewReelTimeSpin(struct Task *task)
{
    if (IsDigitalDisplayAnimFinished())
        sSlotMachine->state = 4;
    return FALSE;
}

static bool8 SlotTask_AskInsertBet(struct Task *task)
{
    CreateDigitalDisplayScene(0);
    sSlotMachine->state = 5;
    if (
#if DEBUG
      (unk_debug_bss_1_1 == 0 || unk_debug_bss_1_4 == 0) &&
#endif
      sSlotMachine->coins >= 9999)
        sSlotMachine->state = 23;
    return TRUE;
}

static bool8 SlotTask_HandleBetInput(struct Task *task)
{
    s16 i;

#if DEBUG
    if (unk_debug_bss_1_1 != 0 && unk_debug_bss_1_4 != 0)
    {
        if (sSlotMachine->coins <= 3 || JOY_HELD(B_BUTTON))
        {
            unk_debug_bss_1_4 = 0;
        }
        else
        {
            LightenBetTiles(0);
            LightenBetTiles(1);
            LightenBetTiles(2);
            sSlotMachine->coins -= 3;
            sSlotMachine->bet = 3;
            sSlotMachine->state = 9;
            return 0;
        }
    }
    if (unk_debug_bss_1_1 != 0 && JOY_NEW(START_BUTTON))
    {
        debug_sub_811B620();
        sSlotMachine->state = 29;
        return 0;
    }
#endif

    if (JOY_NEW(SELECT_BUTTON))
    {
        OpenInfoBox(0);
        sSlotMachine->state = 8;
    }
    else if (JOY_NEW(R_BUTTON))
    {
        if (sSlotMachine->coins - (3 - sSlotMachine->bet) >= 0)
        {
            for (i = sSlotMachine->bet; i < 3; i++)
                LightenBetTiles(i);
            sSlotMachine->coins -= (3 - sSlotMachine->bet);
            sSlotMachine->bet = 3;
            sSlotMachine->state = 9;
            PlaySE(SE_SHOP);
        }
        else
        {
            sSlotMachine->state = 6;
        }
    }
    else
    {
        if (JOY_NEW(DPAD_DOWN) && sSlotMachine->coins != 0)
        {
            PlaySE(SE_SHOP);
            LightenBetTiles(sSlotMachine->bet);
            sSlotMachine->coins--;
            sSlotMachine->bet++;
        }
        if (sSlotMachine->bet >= 3 || (sSlotMachine->bet != 0 && JOY_NEW(A_BUTTON)))
            sSlotMachine->state = 9;
        if (JOY_NEW(B_BUTTON))
            sSlotMachine->state = 21;
    }
    return FALSE;
}

static void sub_8101F2C(const u8 *str)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(str, 2, 15);
}

static bool8 SlotTask_PrintMsg_Need3Coins(struct Task *task)
{
    sub_8101F2C(gOtherText_DontHaveThreeCoins);
    sSlotMachine->state = 7;
    return FALSE;
}

static bool8 SlotTask_WaitMsg_Need3Coins(struct Task *task)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Menu_EraseScreen();
        sSlotMachine->state = 5;
    }
    return FALSE;
}

static bool8 SlotTask_WaitInfoBox(struct Task *task)
{
    if (IsInfoBoxClosed())
        sSlotMachine->state = 5;
    return FALSE;
}

static bool8 SlotTask_StartSpin(struct Task *task)
{
    DrawMachineBias();
    DestroyDigitalDisplayScene();
    SpinSlotReel(0);
    SpinSlotReel(1);
    SpinSlotReel(2);
    task->data[0] = 0;
    if (sSlotMachine->unk04 & 0x20)
    {
        BeginReelTime();
        sSlotMachine->state = 10;
    }
    else
    {
        CreateDigitalDisplayScene(1);
        sSlotMachine->state = 11;
    }
    sSlotMachine->unk1A = 8;
    if (sSlotMachine->unk0A)
        sSlotMachine->unk1A = ReelTimeSpeed();
#if DEBUG
    if (unk_debug_bss_1_1 != 0)
        debug_sub_811B5B4(&sSlotMachine->unk68, 1);
#endif
    return FALSE;
}

static bool8 SlotTask_StartReelTimeSpin(struct Task *task)
{
    if (IsReelTimeTaskDone())
    {
        CreateDigitalDisplayScene(1);
        sSlotMachine->unk04 &= 0xDF;
        sSlotMachine->state = 11;
    }
    return FALSE;
}

static bool8 SlotTask_ResetBiasFailure(struct Task *task)
{
    if (++task->data[0] >= 30)
    {
#if DEBUG
        if (unk_debug_bss_1_1 != 0 && unk_debug_bss_1_4 != 0)
            unk_debug_bss_1_8 = (Random() & 0x1F) + 1;
#endif
        ResetBiasFailure();
        sSlotMachine->state = 12;
    }
    return FALSE;
}

static bool8 SlotTask_WaitReelStop(struct Task *task)
{
#if DEBUG
    if (unk_debug_bss_1_1 != 0 && unk_debug_bss_1_4 != 0)
    {
        unk_debug_bss_1_8--;
        if (unk_debug_bss_1_8 == 0)
        {
            PlaySE(SE_CONTEST_PLACE);
            StopSlotReel(sSlotMachine->unk18);
            PressStopReelButton(sSlotMachine->unk18);
            unk_debug_bss_1_8 = (Random() & 0x1F) + 1;
            sSlotMachine->state = 13;
        }
        return FALSE;
    }
#endif

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_CONTEST_PLACE);
        StopSlotReel(sSlotMachine->unk18);
        PressStopReelButton(sSlotMachine->unk18);
        sSlotMachine->state = 13;
    }
    return FALSE;
}

static bool8 SlotTask_WaitAllReelsStop(struct Task *task)
{
    if (!IsSlotReelMoving(sSlotMachine->unk18))
    {
        sSlotMachine->unk18++;
        sSlotMachine->state = 12;
        if (sSlotMachine->unk18 > 2)
        {
            sSlotMachine->state = 14;
#if DEBUG
            switch (unk_debug_bss_1_0)
            {
            case 2:
                sSlotMachine->reelPositions[0] = 20;
                sSlotMachine->reelPositions[1] = 20;
                sSlotMachine->reelPositions[2] = 18;
                break;
            case 1:
                sSlotMachine->reelPositions[0] = 20;
                sSlotMachine->reelPositions[1] = 20;
                sSlotMachine->reelPositions[2] = 18;
                break;
            case 4:
                sSlotMachine->reelPositions[0] = 3;
                sSlotMachine->reelPositions[1] = 1;
                sSlotMachine->reelPositions[2] = 2;
                break;
            case 8:
                sSlotMachine->reelPositions[0] = 0;
                sSlotMachine->reelPositions[1] = 2;
                sSlotMachine->reelPositions[2] = 3;
                break;
            case 0x10:
                sSlotMachine->reelPositions[0] = 2;
                sSlotMachine->reelPositions[1] = 5;
                sSlotMachine->reelPositions[2] = 20;
                break;
            case 0x40:
                sSlotMachine->reelPositions[0] = 19;
                sSlotMachine->reelPositions[1] = 19;
                sSlotMachine->reelPositions[2] = 0;
                break;
            case 0x80:
                sSlotMachine->reelPositions[0] = 19;
                sSlotMachine->reelPositions[1] = 19;
                sSlotMachine->reelPositions[2] = 19;
                break;
            }
#endif
        }
        return TRUE;
    }
    return FALSE;
}

bool8 SlotTask_CheckMatches(struct Task *task)
{
    sSlotMachine->unk04 &= 0xc0;
    CheckMatch();
    if (sSlotMachine->unk0A)
    {
        sSlotMachine->unk0A--;
        sSlotMachine->unk0B++;
    }
#if DEBUG
    else
    {
        debug_sub_811B894();
    }
#endif

    if (sSlotMachine->matchedSymbols)
    {
#if DEBUG
        debug_sub_811B5B4(&sSlotMachine->unk6C, sSlotMachine->payout);
#endif
        sSlotMachine->state = 15;
        AwardPayout();
        FlashSlotMachineLights();
        if ((sSlotMachine->unk10 -= sSlotMachine->payout) < 0)
        {
            sSlotMachine->unk10 = 0;
        }
        if (sSlotMachine->matchedSymbols & ((1 << SLOT_MACHINE_MATCHED_777_BLUE) | (1 << SLOT_MACHINE_MATCHED_777_RED)))
        {
            PlayFanfare(MUS_SLOTS_JACKPOT);
            CreateDigitalDisplayScene(6);
        }
        else if (sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_777_MIXED))
        {
            PlayFanfare(MUS_SLOTS_JACKPOT);
            CreateDigitalDisplayScene(5);
        }
        else
        {
            PlayFanfare(MUS_SLOTS_WIN);
            CreateDigitalDisplayScene(2);
        }
        if (sSlotMachine->matchedSymbols & ((1 << SLOT_MACHINE_MATCHED_777_MIXED) | (1 << SLOT_MACHINE_MATCHED_777_BLUE) | (1 << SLOT_MACHINE_MATCHED_777_RED)))
        {
            sSlotMachine->unk04 &= 0x3f;
            if (sSlotMachine->matchedSymbols & ((1 << SLOT_MACHINE_MATCHED_777_BLUE) | (1 << SLOT_MACHINE_MATCHED_777_RED)))
            {
                sSlotMachine->unk0A = 0;
                sSlotMachine->unk0B = 0;
                sSlotMachine->unk03 = 0;
                if (sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_777_BLUE))
                    sSlotMachine->unk03 = 1;
            }
        }
        if (sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_POWER) && sSlotMachine->pikaPower < 16)
        {
            sSlotMachine->pikaPower++;
            AddPikaPowerBolt(sSlotMachine->pikaPower);
        }
    }
    else
    {
        CreateDigitalDisplayScene(3);
        sSlotMachine->state = 20;
        if ((sSlotMachine->unk10 += sSlotMachine->bet) > 9999)
            sSlotMachine->unk10 = 9999;
    }
    return FALSE;
}

static bool8 SlotTask_WaitPayout(struct Task *task)
{
    if (IsFinalTask_Task_Payout())
        sSlotMachine->state = 16;
    return FALSE;
}

static bool8 SlotTask_EndPayout(struct Task *task)
{
    if (TryStopSlotMachineLights())
    {
        sSlotMachine->state = 19;
        if (sSlotMachine->matchedSymbols & ((1 << SLOT_MACHINE_MATCHED_777_RED) | (1 << SLOT_MACHINE_MATCHED_777_BLUE)))
            IncrementGameStat(GAME_STAT_SLOT_JACKPOTS);
        if (sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_REPLAY))
        {
            sSlotMachine->unk18 = 0;
            sSlotMachine->state = 9;
        }
        if (sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_POWER))
            sSlotMachine->state = 17;
        if (sSlotMachine->unk0A && sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_REPLAY))
        {
            CreateDigitalDisplayScene(4);
            sSlotMachine->state = 18;
        }
    }
    return FALSE;
}

static bool8 SlotTask_MatchedPower(struct Task *task)
{
    if (!IsPikaPowerBoltAnimating())
    {
        sSlotMachine->state = 19;
        if (sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_REPLAY))
        {
            sSlotMachine->state = 9;
            if (sSlotMachine->unk0A)
            {
                CreateDigitalDisplayScene(4);
                sSlotMachine->state = 18;
            }
        }
    }
    return FALSE;
}

static bool8 SlotTask_WaitReelTimeAnim(struct Task *task)
{
    if (IsDigitalDisplayAnimFinished())
    {
        sSlotMachine->state = 19;
        if (sSlotMachine->matchedSymbols & (1 << SLOT_MACHINE_MATCHED_REPLAY))
        {
            sSlotMachine->state = 9;
        }
    }
    return FALSE;
}

static bool8 SlotTask_ResetBetTiles(struct Task *task)
{
    DarkenBetTiles(0);
    DarkenBetTiles(1);
    DarkenBetTiles(2);
    sSlotMachine->state = 2;
    return FALSE;
}

static bool8 SlotTask_NoMatches(struct Task *task)
{
    if (++task->data[1] > 64)
    {
        task->data[1] = 0;
        sSlotMachine->state = 19;
    }
    return FALSE;
}

static bool8 SlotTask_AskQuit(struct Task *task)
{
    sub_8101F2C(gOtherText_QuitGamePrompt);
    DisplayYesNoMenu(21, 7, 1);
    sub_814AB84();
    sSlotMachine->state = 22;
    return FALSE;
}

static bool8 SlotTask_HandleQuitInput(struct Task *task)
{
    s8 input = Menu_ProcessInputNoWrap_();
    if (input == 0)
    {
        Menu_EraseScreen();
        DarkenBetTiles(0);
        DarkenBetTiles(1);
        DarkenBetTiles(2);
        sSlotMachine->coins += sSlotMachine->bet;
        sSlotMachine->state = 27;
    }
    else if (input == 1 || input == -1)
    {
        Menu_EraseScreen();
        sSlotMachine->state = 5;
    }
    return FALSE;
}

static bool8 SlotTask_PrintMsg_MaxCoins(struct Task *task)
{
    sub_8101F2C(gOtherText_MaxCoins);
    sSlotMachine->state = 24;
    return FALSE;
}

static bool8 SlotTask_WaitMsg_MaxCoins(struct Task *task)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Menu_EraseScreen();
        sSlotMachine->state = 5;
    }
    return FALSE;
}

static bool8 SlotTask_PrintMsg_NoMoreCoins(struct Task *task)
{
    sub_8101F2C(gOtherText_OutOfCoins);
    sSlotMachine->state = 26;
    return FALSE;
}

static bool8 SlotTask_WaitMsg_NoMoreCoins(struct Task *task)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Menu_EraseScreen();
        sSlotMachine->state = 27;
    }
    return FALSE;
}

static bool8 SlotTask_EndGame(struct Task *task)
{
#if DEBUG
    if (unk_debug_bss_1_1 == 0)
        gSaveBlock1.coins = sSlotMachine->coins;
#else
    gSaveBlock1.coins = sSlotMachine->coins;
#endif
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    sSlotMachine->state++;
    return FALSE;
}

static bool8 SlotTask_FreeDataStructures(struct Task *task)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sSlotMachine->prevMainCb);
    }
    return FALSE;
}

#if DEBUG

static bool8 debug_sub_8116E74(struct Task *task)
{
    if (debug_sub_811B634() != 0)
        sSlotMachine->state = 5;
    return FALSE;
}

#endif

static void DrawMachineBias(void)
{
    u8 r3;

    if (sSlotMachine->unk0A == 0)
    {
#if DEBUG
        if (unk_debug_bss_1_1 != 0 && unk_debug_bss_1_2 != 0)
        {
            sSlotMachine->unk04 = unk_debug_bss_1_3;
            unk_debug_bss_1_2 = 0;
            unk_debug_bss_1_3 = 0;
            if (sSlotMachine->unk04 & 0x80)
                debug_sub_811B5B4(&sSlotMachine->unk88, 1);
            if (sSlotMachine->unk04 & 0x40)
                debug_sub_811B5B4(&sSlotMachine->unk84, 1);
            if (sSlotMachine->unk04 & 0x20)
                debug_sub_811B5B4(&sSlotMachine->unk8C, 1);
            if (sSlotMachine->unk04 & 0x10)
                debug_sub_811B5B4(&sSlotMachine->unk80, 1);
            if (sSlotMachine->unk04 & 8)
                debug_sub_811B5B4(&sSlotMachine->unk7C, 1);
            if (sSlotMachine->unk04 & 4)
                debug_sub_811B5B4(&sSlotMachine->unk78, 1);
            if (sSlotMachine->unk04 & 1)
                debug_sub_811B5B4(&sSlotMachine->unk74, 1);
            if (sSlotMachine->unk04 & 2)
                debug_sub_811B5B4(&sSlotMachine->unk70, 1);
            return;
        }
#endif
        if (!(sSlotMachine->unk04 & 0xc0))
        {
            if (ShouldTrySpecialBias())
            {
                r3 = TrySelectBias_Special();
                if (r3 != 3)
                {
                    sSlotMachine->unk04 |= sBiasesSpecial[r3];
#if DEBUG
                    if (sSlotMachine->unk04 & 0x80)
                        debug_sub_811B5B4(&sSlotMachine->unk88, 1);
                    if (sSlotMachine->unk04 & 0x40)
                        debug_sub_811B5B4(&sSlotMachine->unk84, 1);
                    if (sSlotMachine->unk04 & 0x20)
                        debug_sub_811B5B4(&sSlotMachine->unk8C, 1);
#endif
                    if (r3 != 1)
                    {
                        return;
                    }
                }
            }
            r3 = TrySelectBias_Regular();
            if (r3 != 5)
            {
                sSlotMachine->unk04 |= sBiasesRegular[r3];
#if DEBUG
                if (sSlotMachine->unk04 & 0x10)
                    debug_sub_811B5B4(&sSlotMachine->unk80, 1);
                if (sSlotMachine->unk04 & 8)
                    debug_sub_811B5B4(&sSlotMachine->unk7C, 1);
                if (sSlotMachine->unk04 & 4)
                    debug_sub_811B5B4(&sSlotMachine->unk78, 1);
                if (sSlotMachine->unk04 & 1)
                    debug_sub_811B5B4(&sSlotMachine->unk74, 1);
                if (sSlotMachine->unk04 & 2)
                    debug_sub_811B5B4(&sSlotMachine->unk70, 1);
#endif
            }
        }
    }
}

/*
static void DrawMachineBias(void)
{
    u8 r3;

    if (sSlotMachine->unk0A == 0 && !(sSlotMachine->unk04 & 0xc0))
    {
        if (ShouldTrySpecialBias())
        {
            r3 = TrySelectBias_Special();
            if (r3 != 3)
            {
                sSlotMachine->unk04 |= sBiasesSpecial[r3];
                if (r3 != 1)
                {
                    return;
                }
            }
        }
        r3 = TrySelectBias_Regular();
        if (r3 != 5)
        {
            sSlotMachine->unk04 |= sBiasesRegular[r3];
        }
    }
}
*/

static void ResetBiasFailure(void)
{
    sSlotMachine->unk06 = 0;
    if (sSlotMachine->unk04)
        sSlotMachine->unk06 = 1;
}

static u8 GetBiasSymbol(u8 a0)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        if (a0 & 1)
            return sBiasSymbols[i];
        a0 >>= 1;
    }
    return 0;
}

static bool8 ShouldTrySpecialBias(void)
{
    u8 rval = Random();
    if (sSpecialDrawOdds[sSlotMachine->unk01][sSlotMachine->bet - 1] > rval)
        return TRUE;
    return FALSE;
}

static const u8 sBiasProbabilities_Special[][6];

static u8 TrySelectBias_Special(void)
{
    s16 i;

    for (i = 0; i < 3; i++)
    {
        s16 rval = Random() & 0xff;
        s16 value = sBiasProbabilities_Special[i][sSlotMachine->unk01];
        if (value > rval)
            break;
    }
    return i;
}

static const u8 sBiasProbabilities_Regular[][6];

static u8 TrySelectBias_Regular(void)
{
    s16 i;

    for (i = 0; i < 5; i++)
    {
        s16 rval = Random() & 0xff;
        s16 r3 = sBiasProbabilities_Regular[i][sSlotMachine->unk01];
        if (i == 0 && sSlotMachine->unk03 == 1)
        {
            r3 += 10;
            if (r3 > 0x100)
                r3 = 0x100;
        }
        else if (i == 4 && sSlotMachine->unk03 == 1)
        {
            r3 -= 10;
            if (r3 < 0)
                r3 = 0;
        }
        if (r3 > rval)
            break;
    }
    return i;
}

static const u8 sReelTimeProbabilities_NormalGame[][17];
static const u8 sReelTimeProbabilities_LuckyGame[][17];

static u8 GetReelTimeSpinProbability(u8 a0)
{
    if (sSlotMachine->unk03 == 0)
        return sReelTimeProbabilities_NormalGame[a0][sSlotMachine->pikaPower];
    else
        return sReelTimeProbabilities_LuckyGame[a0][sSlotMachine->pikaPower];
}

static void GetReelTimeDraw(void)
{
    u8 rval;
    s16 i;

    sSlotMachine->unk05 = 0;
    rval = Random();
    if (rval < GetReelTimeSpinProbability(0))
        return;
    for (i = 5; i > 0; i--)
    {
        rval = Random();
        if (rval < GetReelTimeSpinProbability(i))
            break;
    }
    sSlotMachine->unk05 = i;
}

static const u16 sReelTimeExplodeProbability[];

static bool8 ShouldReelTimeMachineExplode(u16 a0)
{
    u16 rval = Random() & 0xff;
    if (rval < sReelTimeExplodeProbability[a0])
        return TRUE;
    else
        return FALSE;
}

static const u16 sReelTimeSpeed_Probabilities[][2];
static const u16 sQuarterSpeed_ProbabilityBoost[];

static u16 ReelTimeSpeed(void)
{
    u8 r4 = 0;
    u8 rval;
    u8 value;
    if (sSlotMachine->unk10 >= 300)
        r4 = 4;
    else if (sSlotMachine->unk10 >= 250)
        r4 = 3;
    else if (sSlotMachine->unk10 >= 200)
        r4 = 2;
    else if (sSlotMachine->unk10 >= 150)
        r4 = 1;
    rval = Random() % 100;
    value = sReelTimeSpeed_Probabilities[r4][0];
    if (rval < value)
        return 4;
    rval = Random() % 100;
    value = sReelTimeSpeed_Probabilities[r4][1] + sQuarterSpeed_ProbabilityBoost[sSlotMachine->unk0B];
    if (rval < value)
        return 2;
    return 8;
}

static void CheckMatch(void)
{
    sSlotMachine->matchedSymbols = 0;
    CheckMatch_CenterRow();
    if (sSlotMachine->bet > 1)
        CheckMatch_TopAndBottom();
    if (sSlotMachine->bet > 2)
        CheckMatch_Diagonals();
}

static const u16 sSlotMatchFlags[];
static const u16 sSlotPayouts[];

static void CheckMatch_CenterRow(void)
{
    u8 c1, c2, c3, match;

    c1 = GetSymbolAtRest(0, 2);
    c2 = GetSymbolAtRest(1, 2);
    c3 = GetSymbolAtRest(2, 2);
    match = GetMatchFromSymbolsInRow(c1, c2, c3);
    if (match != SLOT_MACHINE_MATCHED_NONE)
    {
        sSlotMachine->payout += sSlotPayouts[match];
        sSlotMachine->matchedSymbols |= sSlotMatchFlags[match];
        FlashMatchLine(0);
    }
}

static void CheckMatch_TopAndBottom(void)
{
    u8 c1, c2, c3, match;

    c1 = GetSymbolAtRest(0, 1);
    c2 = GetSymbolAtRest(1, 1);
    c3 = GetSymbolAtRest(2, 1);
    match = GetMatchFromSymbolsInRow(c1, c2, c3);
    if (match != SLOT_MACHINE_MATCHED_NONE)
    {
        if (match == SLOT_MACHINE_MATCHED_1CHERRY)
            match = SLOT_MACHINE_MATCHED_2CHERRY;
        sSlotMachine->payout += sSlotPayouts[match];
        sSlotMachine->matchedSymbols |= sSlotMatchFlags[match];
        FlashMatchLine(1);
    }
    c1 = GetSymbolAtRest(0, 3);
    c2 = GetSymbolAtRest(1, 3);
    c3 = GetSymbolAtRest(2, 3);
    match = GetMatchFromSymbolsInRow(c1, c2, c3);
    if (match != SLOT_MACHINE_MATCHED_NONE)
    {
        if (match == SLOT_MACHINE_MATCHED_1CHERRY)
            match = SLOT_MACHINE_MATCHED_2CHERRY;
        sSlotMachine->payout += sSlotPayouts[match];
        sSlotMachine->matchedSymbols |= sSlotMatchFlags[match];
        FlashMatchLine(2);
    }
}

static void CheckMatch_Diagonals(void)
{
    u8 c1, c2, c3, match;

    c1 = GetSymbolAtRest(0, 1);
    c2 = GetSymbolAtRest(1, 2);
    c3 = GetSymbolAtRest(2, 3);
    match = GetMatchFromSymbolsInRow(c1, c2, c3);
    if (match != SLOT_MACHINE_MATCHED_NONE)
    {
        if (match != SLOT_MACHINE_MATCHED_1CHERRY)
        {
            sSlotMachine->payout += sSlotPayouts[match];
            sSlotMachine->matchedSymbols |= sSlotMatchFlags[match];
        }
        FlashMatchLine(3);
    }
    c1 = GetSymbolAtRest(0, 3);
    c2 = GetSymbolAtRest(1, 2);
    c3 = GetSymbolAtRest(2, 1);
    match = GetMatchFromSymbolsInRow(c1, c2, c3);
    if (match != SLOT_MACHINE_MATCHED_NONE)
    {
        if (match != SLOT_MACHINE_MATCHED_1CHERRY)
        {
            sSlotMachine->payout += sSlotPayouts[match];
            sSlotMachine->matchedSymbols |= sSlotMatchFlags[match];
        }
        FlashMatchLine(4);
    }
}

static const u8 sSymbolToMatch[];

static u8 GetMatchFromSymbolsInRow(u8 c1, u8 c2, u8 c3)
{
    if (c1 == c2 && c1 == c3)
        return sSymbolToMatch[c1];
    if (c1 == SLOT_MACHINE_TAG_7_RED && c2 == SLOT_MACHINE_TAG_7_RED && c3 == SLOT_MACHINE_TAG_7_BLUE)
        return SLOT_MACHINE_MATCHED_777_MIXED;
    if (c1 == SLOT_MACHINE_TAG_7_BLUE && c2 == SLOT_MACHINE_TAG_7_BLUE && c3 == SLOT_MACHINE_TAG_7_RED)
        return SLOT_MACHINE_MATCHED_777_MIXED;
    if (c1 == SLOT_MACHINE_TAG_CHERRY)
        return SLOT_MACHINE_MATCHED_1CHERRY;
    return SLOT_MACHINE_MATCHED_NONE;
}

static void AwardPayout(void)
{
    Task_Payout(CreateTask(Task_Payout, 4));
}

static bool8 IsFinalTask_Task_Payout(void)
{
    if (FindTaskIdByFunc(Task_Payout) == 0xff)
        return TRUE;
    else
        return FALSE;
}

static bool8 (*const sPayoutTasks[])(struct Task *task) =
{
    PayoutTask_Init,
    PayoutTask_GivePayout,
    PayoutTask_Free
};

static void Task_Payout(u8 taskId)
{
    while (sPayoutTasks[gTasks[taskId].data[0]](gTasks + taskId))
        ;
}

static bool8 PayoutTask_Init(struct Task *task)
{
    if (IsMatchLineDoneFlashingBeforePayout())
    {
        task->data[0]++;
        if (sSlotMachine->payout == 0)
        {
            task->data[0] = 2;
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 PayoutTask_GivePayout(struct Task *task)
{
    if (!task->data[1]--)
    {
        if (IsFanfareTaskInactive())
            PlaySE(SE_PIN);
        sSlotMachine->payout--;
        if (sSlotMachine->coins < 9999)
            sSlotMachine->coins++;
        task->data[1] = 8;
        if (JOY_HELD(A_BUTTON))
            task->data[1] = 4;
    }
    if (IsFanfareTaskInactive() && JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_PIN);
        sSlotMachine->coins += sSlotMachine->payout;
        if (sSlotMachine->coins > 9999)
            sSlotMachine->coins = 9999;
        sSlotMachine->payout = 0;
    }
    if (sSlotMachine->payout == 0)
        task->data[0]++;
    return FALSE;
}

static bool8 PayoutTask_Free(struct Task *task)
{
    if (TryStopMatchLinesFlashing())
        DestroyTask(FindTaskIdByFunc(Task_Payout));
    return FALSE;
}

static const u8 sReelSymbols[][21];

static u8 GetSymbolAtRest(u8 x, s16 y)
{
    s16 offset = (sSlotMachine->reelPositions[x] + y) % 21;
    if (offset < 0)
        offset += 21;
    return sReelSymbols[x][offset];
}

static u8 GetSymbol(u8 x, s16 y)
{
    s16 r6 = 0;
    if ((sSlotMachine->unk1C[x]) % 24)
        r6 = -1;
    return GetSymbolAtRest(x, y + r6);
}

static const u8 sReelTimeSymbols[];

static u8 GetReelTimeSymbol(s16 a0)
{
    s16 r1 = (sSlotMachine->unk16 + a0) % 6;
    if (r1 < 0)
        r1 += 6;
    return sReelTimeSymbols[r1];
}

static void AdvanceSlotReel(u8 a0, s16 a1)
{
    sSlotMachine->unk1C[a0] += a1;
    sSlotMachine->unk1C[a0] %= 504;
    sSlotMachine->reelPositions[a0] = 21 - sSlotMachine->unk1C[a0] / 24;
}

static s16 AdvanceSlotReelToNextSymbol(u8 a0, s16 a1)
{
    s16 r1 = sSlotMachine->unk1C[a0] % 24;
    if (r1 != 0)
    {
        if (r1 < a1)
            a1 = r1;
        AdvanceSlotReel(a0, a1);
        r1 = sSlotMachine->unk1C[a0] % 24;
    }
    return r1;
}

static void AdvanceReeltimeReel(s16 a0)
{
    sSlotMachine->unk14 += a0;
    sSlotMachine->unk14 %= 120;
    sSlotMachine->unk16 = 6 - sSlotMachine->unk14 / 20;
}

static s16 AdvanceReeltimeReelToNextSymbol(s16 a0)
{
    s16 r1 = sSlotMachine->unk14 % 20;
    if (r1 != 0)
    {
        if (r1 < a0)
            a0 = r1;
        AdvanceReeltimeReel(a0);
        r1 = sSlotMachine->unk14 % 20;
    }
    return r1;
}

static void CreateReelTasks(void)
{
    u8 i;
    for (i = 0; i < 3; i++)
    {
        u8 taskId = CreateTask(Task_Reel, 2);
        gTasks[taskId].data[15] = i;
        sSlotMachine->reelTasks[i] = taskId;
        Task_Reel(taskId);
    }
}

static void SpinSlotReel(u8 a0)
{
    gTasks[sSlotMachine->reelTasks[a0]].data[0] = 1;
    gTasks[sSlotMachine->reelTasks[a0]].data[14] = 1;
}

static void StopSlotReel(u8 a0)
{
    gTasks[sSlotMachine->reelTasks[a0]].data[0] = 2;
}

static bool8 IsSlotReelMoving(u8 a0)
{
    return gTasks[sSlotMachine->reelTasks[a0]].data[14];
}

static bool8 (*const sReelTasks[])(struct Task *task) =
{
    ReelTask_StayStill,
    ReelTask_Spin,
    ReelTask_DecideStop,
    ReelTask_MoveToStop,
    ReelTask_ShakingStop
};

static void Task_Reel(u8 taskId)
{
    while (sReelTasks[gTasks[taskId].data[0]](gTasks + taskId))
        ;
}

static bool8 ReelTask_StayStill(struct Task *task)
{
    return FALSE;
}

static bool8 ReelTask_Spin(struct Task *task)
{
    AdvanceSlotReel(task->data[15], sSlotMachine->unk1A);
    return FALSE;
}

static bool8 (*const sDecideStop_Bias[])(void) =
{
    DecideStop_Bias_Reel1,
    DecideStop_Bias_Reel2,
    DecideStop_Bias_Reel3
};

static void (*const sDecideStop_NoBias[])(void) =
{
    DecideStop_NoBias_Reel1,
    DecideStop_NoBias_Reel2,
    DecideStop_NoBias_Reel3
};

static bool8 ReelTask_DecideStop(struct Task *task)
{
    task->data[0]++;
    sSlotMachine->unk34[task->data[15]] = 0;
    sSlotMachine->unk2E[task->data[15]] = 0;
    if (sSlotMachine->unk0A == 0 && (sSlotMachine->unk04 == 0 || sSlotMachine->unk06 == 0 || !sDecideStop_Bias[task->data[15]]()))
    {
        sSlotMachine->unk06 = 0;
        sDecideStop_NoBias[task->data[15]]();
    }
    task->data[1] = sSlotMachine->unk2E[task->data[15]];
    return TRUE;
}

static bool8 ReelTask_MoveToStop(struct Task *task)
{
    u16 sp[] = {2, 4, 4, 4, 8};
    s16 r2 = sSlotMachine->unk1C[task->data[15]] % 24;
    if (r2 != 0)
        r2 = AdvanceSlotReelToNextSymbol(task->data[15], sSlotMachine->unk1A);
    else if (sSlotMachine->unk2E[task->data[15]])
    {
        sSlotMachine->unk2E[task->data[15]]--;
        AdvanceSlotReel(task->data[15], sSlotMachine->unk1A);
        r2 = sSlotMachine->unk1C[task->data[15]] % 24;
    }
    if (r2 == 0 && sSlotMachine->unk2E[task->data[15]] == 0)
    {
        task->data[0]++;
        task->data[1] = sp[task->data[1]];
        task->data[2] = 0;
    }
    return FALSE;
}

static bool8 ReelTask_ShakingStop(struct Task *task)
{
    sSlotMachine->unk22[task->data[15]] = task->data[1];
    task->data[1] = -task->data[1];
    task->data[2]++;
    if ((task->data[2] & 0x3) == 0)
        task->data[1] >>= 1;
    if (task->data[1] == 0)
    {
        task->data[0] = 0;
        task->data[14] = 0;
        sSlotMachine->unk22[task->data[15]] = 0;
    }
    return FALSE;
}

static bool8 (*const sDecideStop_Bias_Reel1_Bets[])(u8 a0, u8 a1) =
{
    DecideStop_Bias_Reel1_Bet1,
    DecideStop_Bias_Reel1_Bet2or3,
    DecideStop_Bias_Reel1_Bet2or3
};

static bool8 DecideStop_Bias_Reel1(void)
{
    u8 r3 = GetBiasSymbol(sSlotMachine->unk04);
    u8 r5 = r3;
    if (sSlotMachine->unk04 & 0xc0)
    {
        r5 = 0;
        r3 = 1;
    }
    return sDecideStop_Bias_Reel1_Bets[sSlotMachine->bet - 1](r5, r3);
}

static bool8 EitherSymbolAtPos_Reel1(s16 y, u8 tag1, u8 tag2)
{
    u8 tag = GetSymbol(0, y);
    if (tag == tag1 || tag == tag2)
    {
        sSlotMachine->unk07 = tag;
        return TRUE;
    }
    return FALSE;
}

static bool8 AreCherriesOnScreen_Reel1(s16 y)
{
    if (GetSymbol(0, 1 - y) == 4 || GetSymbol(0, 2 - y) == 4 || GetSymbol(0, 3 - y) == 4)
        return TRUE;
    else
        return FALSE;
}

static bool8 BiasedTowardCherryOr7s(void)
{
    if (sSlotMachine->unk04 & 0xc2)
        return TRUE;
    else
        return FALSE;
}

static bool8 DecideStop_Bias_Reel1_Bet1(u8 a0, u8 a1)
{
    s16 i;

    for (i = 0; i < 5; i++)
    {
        if (EitherSymbolAtPos_Reel1(2 - i, a0, a1))
        {
            sSlotMachine->unk34[0] = 2;
            sSlotMachine->unk2E[0] = i;
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 DecideStop_Bias_Reel1_Bet2or3(u8 tag1, u8 tag2)
{
    s16 i;
    bool8 r6 = BiasedTowardCherryOr7s();
    if (r6 || !AreCherriesOnScreen_Reel1(0))
    {
        for (i = 1; i < 4; i++)
        {
            if (EitherSymbolAtPos_Reel1(i, tag1, tag2))
            {
                sSlotMachine->unk34[0] = i;
                sSlotMachine->unk2E[0] = 0;
                return TRUE;
            }
        }
    }
    for (i = 1; i < 5; i++)
    {
        bool8 r7 = r6;
        if (r7 || !AreCherriesOnScreen_Reel1(i))
        {
            if (EitherSymbolAtPos_Reel1(1 - i, tag1, tag2))
            {
                if (i == 1 && (r7 || !AreCherriesOnScreen_Reel1(3)))
                {
                    sSlotMachine->unk34[0] = 3;
                    sSlotMachine->unk2E[0] = 3;
                    return TRUE;
                }
                if (i < 4 && (r7 || !AreCherriesOnScreen_Reel1(i + 1)))
                {
                    sSlotMachine->unk34[0] = 2;
                    sSlotMachine->unk2E[0] = i + 1;
                    return TRUE;
                }
                sSlotMachine->unk34[0] = 1;
                sSlotMachine->unk2E[0] = i;
                return TRUE;
            }
        }
    }
    return FALSE;
}

static bool8 (*const sDecideStop_Bias_Reel2_Bets[])(void) =
{
    DecideStop_Bias_Reel2_Bet1or2,
    DecideStop_Bias_Reel2_Bet1or2,
    DecideStop_Bias_Reel2_Bet3
};

static bool8 DecideStop_Bias_Reel2(void)
{
    return sDecideStop_Bias_Reel2_Bets[sSlotMachine->bet - 1]();
}

static bool8 DecideStop_Bias_Reel2_Bet1or2(void)
{
    s16 i;
    s16 unk34_0 = sSlotMachine->unk34[0];

    for (i = 0; i < 5; i++)
    {
        if (GetSymbol(1, unk34_0 - i) == sSlotMachine->unk07)
        {
            sSlotMachine->unk34[1] = unk34_0;
            sSlotMachine->unk2E[1] = i;
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 DecideStop_Bias_Reel2_Bet3(void)
{
    s16 i;
    if (DecideStop_Bias_Reel2_Bet1or2())
    {
        if (sSlotMachine->unk34[0] != 2 && sSlotMachine->unk2E[1] > 1 && sSlotMachine->unk2E[1] != 4)
        {
            for (i = 0; i < 5; i++)
            {
                if (GetSymbol(1, 2 - i) == sSlotMachine->unk07)
                {
                    sSlotMachine->unk34[1] = 2;
                    sSlotMachine->unk2E[1] = i;
                    break;
                }
            }
        }
        return TRUE;
    }
    if (sSlotMachine->unk34[0] != 2)
    {
        for (i = 0; i < 5; i++)
        {
            if (GetSymbol(1, 2 - i) == sSlotMachine->unk07)
            {
                sSlotMachine->unk34[1] = 2;
                sSlotMachine->unk2E[1] = i;
                return TRUE;
            }
        }
    }
    return FALSE;
}

static bool8 (*const sDecideStop_Bias_Reel3_Bets[])(u8 a0) =
{
    DecideStop_Bias_Reel3_Bet1or2,
    DecideStop_Bias_Reel3_Bet1or2,
    DecideStop_Bias_Reel3_Bet3
};

static bool8 DecideStop_Bias_Reel3(void)
{
    u8 r3 = sSlotMachine->unk07;
    if (sSlotMachine->unk04 & 0x40)
    {
        r3 = 0;
        if (sSlotMachine->unk07 == 0)
        {
            r3 = 1;
        }
    }
    return sDecideStop_Bias_Reel3_Bets[sSlotMachine->bet - 1](r3);
}

static bool8 DecideStop_Bias_Reel3_Bet1or2(u8 a0)
{
    s16 i;
    s16 unk34_1 = sSlotMachine->unk34[1];

    for (i = 0; i < 5; i++)
    {
        if (GetSymbol(2, unk34_1 - i) == a0)
        {
            sSlotMachine->unk34[2] = unk34_1;
            sSlotMachine->unk2E[2] = i;
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 DecideStop_Bias_Reel3_Bet3(u8 a0)
{
    s16 i;
    s16 r8;
    if (sSlotMachine->unk34[0] == sSlotMachine->unk34[1])
        return DecideStop_Bias_Reel3_Bet1or2(a0);
    if (sSlotMachine->unk34[0] == 1)
        r8 = 3;
    else
        r8 = 1;
    for (i = 0; i < 5; i++)
    {
        if (GetSymbol(2, r8 - i) == a0)
        {
            sSlotMachine->unk2E[2] = i;
            sSlotMachine->unk34[2] = r8;
            return TRUE;
        }
    }
    return FALSE;
}

static void DecideStop_NoBias_Reel1(void)
{
    s16 i = 0;

    while (AreCherriesOnScreen_Reel1(i) != 0)
        i++;
    sSlotMachine->unk2E[0] = i;
}

static bool8 IfSymbol7_SwitchColor(u8 *a0)
{
    if (*a0 == 0)
    {
        *a0 = 1;
        return TRUE;
    }
    if (*a0 == 1)
    {
        *a0 = 0;
        return TRUE;
    }
    return FALSE;
}

static void (*const sDecideStop_NoBias_Reel2_Bets[])(void) =
{
    DecideStop_NoBias_Reel2_Bet1,
    DecideStop_NoBias_Reel2_Bet2,
    DecideStop_NoBias_Reel2_Bet3
};

static void DecideStop_NoBias_Reel2(void)
{
    sDecideStop_NoBias_Reel2_Bets[sSlotMachine->bet - 1]();
}

static void DecideStop_NoBias_Reel2_Bet1(void)
{
    if (sSlotMachine->unk34[0] != 0 && sSlotMachine->unk04 & 0x80)
    {
        u8 sp0 = GetSymbol(0, 2 - sSlotMachine->unk2E[0]);
        if (IfSymbol7_SwitchColor(&sp0))
        {
            s16 i;
            for (i = 0; i < 5; i++)
            {
                if (sp0 == GetSymbol(1, 2 - i))
                {
                    sSlotMachine->unk34[1] = 2;
                    sSlotMachine->unk2E[1] = i;
                    break;
                }
            }
        }
    }
}

static void DecideStop_NoBias_Reel2_Bet2(void)
{
    if (sSlotMachine->unk34[0] != 0 && sSlotMachine->unk04 & 0x80)
    {
        u8 sp0 = GetSymbol(0, sSlotMachine->unk34[0] - sSlotMachine->unk2E[0]);
        if (IfSymbol7_SwitchColor(&sp0))
        {
            s16 i;
            for (i = 0; i < 5; i++)
            {
                if (sp0 == GetSymbol(1, sSlotMachine->unk34[0] - i))
                {
                    sSlotMachine->unk34[1] = sSlotMachine->unk34[0];
                    sSlotMachine->unk2E[1] = i;
                    break;
                }
            }
        }
    }
}

static void DecideStop_NoBias_Reel2_Bet3(void)
{
    s16 i;
    s16 j;
    if (sSlotMachine->unk34[0] != 0 && sSlotMachine->unk04 & 0x80)
    {
        if (sSlotMachine->unk34[0] == 2)
        {
            DecideStop_NoBias_Reel2_Bet2();
        }
        else
        {
            u8 sp0 = GetSymbol(0, sSlotMachine->unk34[0] - sSlotMachine->unk2E[0]);
            if (IfSymbol7_SwitchColor(&sp0))
            {
                j = 2;
                if (sSlotMachine->unk34[0] == 3)
                    j = 3;
                for (i = 0; i < 2; i++, j--)
                {
                    if (sp0 == GetSymbol(1, j))
                    {
                        sSlotMachine->unk34[1] = j;
                        sSlotMachine->unk2E[1] = 0;
                        return;
                    }
                }
                for (j = 1; j < 5; j++)
                {
                    if (sp0 == GetSymbol(1, sSlotMachine->unk34[0] - j))
                    {
                        if (sSlotMachine->unk34[0] == 1)
                        {
                            if (j < 3)
                            {
                                sSlotMachine->unk34[1] = 2;
                                sSlotMachine->unk2E[1] = j + 1;
                            }
                            else
                            {
                                sSlotMachine->unk34[1] = 1;
                                sSlotMachine->unk2E[1] = j;
                            }
                        }
                        else
                        {
                            if (j < 3)
                            {
                                sSlotMachine->unk34[1] = 3;
                                sSlotMachine->unk2E[1] = j;
                            }
                            else
                            {
                                sSlotMachine->unk34[1] = 2;
                                sSlotMachine->unk2E[1] = j - 1;
                            }
                        }
                        return;
                    }
                }
            }
        }
    }
}

static bool8 MismatchedSyms_77(u8 a0, u8 a1)
{
    if ((a0 == 0 && a1 == 1) || (a0 == 1 && a1 == 0))
        return TRUE;
    else
        return FALSE;
}

static bool8 MismatchedSyms_777(u8 a0, u8 a1, u8 a2)
{
    if ((a0 == 0 && a1 == 1 && a2 == 0) || (a0 == 1 && a1 == 0 && a2 == 1))
        return TRUE;
    else
        return FALSE;
}

static bool8 NeitherMatchNor7Mismatch(u8 a0, u8 a1, u8 a2)
{
    if ((a0 == 0 && a1 == 1 && a2 == 0) ||
        (a0 == 1 && a1 == 0 && a2 == 1) ||
        (a0 == 0 && a1 == 0 && a2 == 1) ||
        (a0 == 1 && a1 == 1 && a2 == 0) ||
        (a0 == a1 && a0 == a2))
    {
        return FALSE;
    }
    return TRUE;
}

static void (*const sDecideStop_NoBias_Reel3_Bets[])(void) =
{
    DecideStop_NoBias_Reel3_Bet1,
    DecideStop_NoBias_Reel3_Bet2,
    DecideStop_NoBias_Reel3_Bet3
};

static void DecideStop_NoBias_Reel3(void)
{
    sDecideStop_NoBias_Reel3_Bets[sSlotMachine->bet - 1]();
}

static void DecideStop_NoBias_Reel3_Bet1(void)
{
    s16 i = 0;
    u8 r5 = GetSymbol(0, 2 - sSlotMachine->unk2E[0]);
    u8 r1 = GetSymbol(1, 2 - sSlotMachine->unk2E[1]);
    if (r5 == r1)
    {
        while (1)
        {
            u8 r0;
            if (!(r5 == (r0 = GetSymbol(2, 2 - i)) || (r5 == 0 && r0 == 1) || (r5 == 1 && r0 == 0)))
                break;
            i++;
        }
    }
    else if (MismatchedSyms_77(r5, r1))
    {
        if (sSlotMachine->unk04 & 0x80)
        {
            for (i = 0; i < 5; i++)
            {
                if (r5 == GetSymbol(2, 2 - i))
                {
                    sSlotMachine->unk2E[2] = i;
                    return;
                }
            }
        }
        i = 0;
        while (1)
        {
            if (r5 != GetSymbol(2, 2 - i))
                break;
            i++;
        }
    }
    sSlotMachine->unk2E[2] = i;
}

static void DecideStop_NoBias_Reel3_Bet2(void)
{
    s16 sp0 = 0;
    s16 i;
    u8 r7;
    u8 r6;
    u8 r4;

    if (sSlotMachine->unk34[1] != 0 && sSlotMachine->unk34[0] == sSlotMachine->unk34[1] && sSlotMachine->unk04 & 0x80)
    {
        r7 = GetSymbol(0, sSlotMachine->unk34[0] - sSlotMachine->unk2E[0]);
        r6 = GetSymbol(1, sSlotMachine->unk34[1] - sSlotMachine->unk2E[1]);
        if (MismatchedSyms_77(r7, r6))
        {
            for (i = 0; i < 5; i++)
            {
                r4 = GetSymbol(2, sSlotMachine->unk34[1] - i);
                if (r7 == r4)
                {
                    sp0 = i;
                    break;
                }
            }
        }
    }
    while (1)
    {
        s16 r8;
        for (i = 1, r8 = 0; i < 4; i++)
        {
            r7 = GetSymbol(0, i - sSlotMachine->unk2E[0]);
            r6 = GetSymbol(1, i - sSlotMachine->unk2E[1]);
            r4 = GetSymbol(2, i - sp0);
            if (!NeitherMatchNor7Mismatch(r7, r6, r4) && (!MismatchedSyms_777(r7, r6, r4) || !(sSlotMachine->unk04 & 0x80)))
            {
                r8++;
                break;
            }
        }
        if (r8 == 0)
            break;
        sp0++;
    }
    sSlotMachine->unk2E[2] = sp0;
}

static void DecideStop_NoBias_Reel3_Bet3(void)
{
    u8 r6;
    u8 r5;
    u8 r4;
    s16 r8;
    s16 i;

    DecideStop_NoBias_Reel3_Bet2();
    if (sSlotMachine->unk34[1] != 0 && sSlotMachine->unk34[0] != sSlotMachine->unk34[1] && sSlotMachine->unk04 & 0x80)
    {
        r6 = GetSymbol(0, sSlotMachine->unk34[0] - sSlotMachine->unk2E[0]);
        r5 = GetSymbol(1, sSlotMachine->unk34[1] - sSlotMachine->unk2E[1]);
        if (MismatchedSyms_77(r6, r5))
        {
            r8 = 1;
            if (sSlotMachine->unk34[0] == 1)
                r8 = 3;
            for (i = 0; i < 5; i++)
            {
                r4 = GetSymbol(2, r8 - (sSlotMachine->unk2E[2] + i));
                if (r6 == r4)
                {
                    sSlotMachine->unk2E[2] += i;
                    break;
                }
            }
        }
    }
    while (1)
    {
        r6 = GetSymbol(0, 1 - sSlotMachine->unk2E[0]);
        r5 = GetSymbol(1, 2 - sSlotMachine->unk2E[1]);
        r4 = GetSymbol(2, 3 - sSlotMachine->unk2E[2]);
        if (NeitherMatchNor7Mismatch(r6, r5, r4) || (MismatchedSyms_777(r6, r5, r4) && sSlotMachine->unk04 & 0x80))
            break;
        sSlotMachine->unk2E[2]++;
    }
    while (1)
    {
        r6 = GetSymbol(0, 3 - sSlotMachine->unk2E[0]);
        r5 = GetSymbol(1, 2 - sSlotMachine->unk2E[1]);
        r4 = GetSymbol(2, 1 - sSlotMachine->unk2E[2]);
        if (NeitherMatchNor7Mismatch(r6, r5, r4) || (MismatchedSyms_777(r6, r5, r4) && sSlotMachine->unk04 & 0x80))
            break;
        sSlotMachine->unk2E[2]++;
    }
}

static void PressStopReelButton(u8 a0)
{
    u8 taskId = CreateTask(Task_PressStopReelButton, 5);
    gTasks[taskId].data[15] = a0;
    Task_PressStopReelButton(taskId);
}

static void (*const sReelStopButtonTasks[])(struct Task *task, u8 taskId) =
{
    StopReelButton_Press,
    StopReelButton_Wait,
    StopReelButton_Unpress
};

static void Task_PressStopReelButton(u8 taskId)
{
    sReelStopButtonTasks[gTasks[taskId].data[0]](gTasks + taskId, taskId);
}

static const s16 sReelButtonOffsets[] = {5, 10, 15};

static void StopReelButton_Press(struct Task *task, u8 taskId)
{
    SetReelButtonTilemap(sReelButtonOffsets[task->data[15]], 0x62, 0x63, 0x72, 0x73);
    task->data[0]++;
}

static void StopReelButton_Wait(struct Task *task, u8 taskId)
{
    if (++task->data[1] > 11)
        task->data[0]++;
}

static void StopReelButton_Unpress(struct Task *task, u8 taskId)
{
    SetReelButtonTilemap(sReelButtonOffsets[task->data[15]], 0x42, 0x43, 0x52, 0x53);
    DestroyTask(taskId);
}

static const u16 *const sLitMatchLinePalTable[];
static const u16 *const sDarkMatchLinePalTable[];
static const u8 sMatchLinePalOffsets[];

static void LightenMatchLine(u8 a0)
{
    LoadPalette(sLitMatchLinePalTable[a0], sMatchLinePalOffsets[a0], 2);
}

static void DarkenMatchLine(u8 a0)
{
    LoadPalette(sDarkMatchLinePalTable[a0], sMatchLinePalOffsets[a0], 2);
}

static const u8 sBetToMatchLineIds[][2];
static const u8 sMatchLinesPerBet[];

static void LightenBetTiles(u8 a0)
{
    u8 i;
    for (i = 0; i < sMatchLinesPerBet[a0]; i++)
        LightenMatchLine(sBetToMatchLineIds[a0][i]);
}

static void DarkenBetTiles(u8 a0)
{
    u8 i;
    for (i = 0; i < sMatchLinesPerBet[a0]; i++)
        DarkenMatchLine(sBetToMatchLineIds[a0][i]);
}

static void CreateInvisibleFlashMatchLineSprites(void)
{
    u8 i;
    for (i = 0; i < 5; i++)
    {
        u8 spriteId = CreateInvisibleSprite(SpriteCB_FlashMatchingLines);
        gSprites[spriteId].data[0] = i;
        sSlotMachine->unk44[i] = spriteId;
    }
}

static void FlashMatchLine(u8 a0)
{
    struct Sprite *sprite = gSprites + sSlotMachine->unk44[a0];
    sprite->data[1] = 1;
    sprite->data[2] = 4;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    sprite->data[5] = 2;
    sprite->data[7] = 0;
}

static bool8 IsMatchLineDoneFlashingBeforePayout(void)
{
    u8 i;
    for (i = 0; i < 5; i++)
    {
        struct Sprite *sprite = &gSprites[sSlotMachine->unk44[i]];
        if (sprite->data[1] && sprite->data[2])
            return FALSE;
    }
    return TRUE;
}

static bool8 TryStopMatchLinesFlashing(void)
{
    u8 i;
    for (i = 0; i < 5; i++)
    {
        if (!TryStopMatchLineFlashing(sSlotMachine->unk44[i]))
            return FALSE;
    }
    return TRUE;
}

static bool8 TryStopMatchLineFlashing(u8 spriteId)
{
    struct Sprite *sprite = gSprites + spriteId;
    if (!sprite->data[1])
        return TRUE;
    if (sprite->data[7])
        sprite->data[1] = 0;
    return sprite->data[7];
}

static void SpriteCB_FlashMatchingLines(struct Sprite *sprite)
{
    s16 r4;
    if (sprite->data[1])
    {
        if (!sprite->data[3]--)
        {
            sprite->data[7] = 0;
            sprite->data[3] = 1;
            sprite->data[4] += sprite->data[5];
            r4 = 4;
            if (sprite->data[2])
                r4 = 8;
            if (sprite->data[4] <= 0)
            {
                sprite->data[7] = 1;
                sprite->data[5] = -sprite->data[5];
                if (sprite->data[2])
                    sprite->data[2]--;
            }
            else if (sprite->data[4] >= r4)
                sprite->data[5] = -sprite->data[5];
            if (sprite->data[2])
                sprite->data[3] <<= 1;
        }
        MultiplyPaletteRGBComponents(sMatchLinePalOffsets[sprite->data[0]], sprite->data[4], sprite->data[4], sprite->data[4]);
    }
}

static void FlashSlotMachineLights(void)
{
    u8 taskId = CreateTask(Task_FlashSlotMachineLights, 6);
    gTasks[taskId].data[3] = 1;
    Task_FlashSlotMachineLights(taskId);
}

static const u16 *const sFlashingLightsPalTable[];
static const u16 *const sSlotMachineMenu_Pal;

static bool8 TryStopSlotMachineLights(void)
{
    u8 taskId = FindTaskIdByFunc(Task_FlashSlotMachineLights);
    if (!gTasks[taskId].data[2])
    {
        DestroyTask(taskId);
        LoadPalette(sSlotMachineMenu_Pal, 0x10, 0x20);
        return TRUE;
    }
    return FALSE;
}

static void Task_FlashSlotMachineLights(u8 taskId)
{
    struct Task *task = gTasks + taskId;
    if (!task->data[1]--)
    {
        task->data[1] = 4;
        task->data[2] += task->data[3];
        if (task->data[2] == 0 || task->data[2] == 2)
            task->data[3] = -task->data[3];
    }
    LoadPalette(sFlashingLightsPalTable[task->data[2]], 0x10, 0x20);
}

static void CreatePikaPowerBoltTask(void)
{
    sSlotMachine->unk3E = CreateTask(Task_CreatePikaPowerBolt, 8);
}

static void AddPikaPowerBolt(u8 pikaPower)
{
    struct Task *task = gTasks + sSlotMachine->unk3E;
    ResetPikaPowerBoltTask(task);
    task->data[0] = 1;
    task->data[1]++;
    task->data[15] = 1;
}

static void ResetPikaPowerBolts(void)
{
    struct Task *task = gTasks + sSlotMachine->unk3E;
    ResetPikaPowerBoltTask(task);
    task->data[0] = 3;
    task->data[15] = 1;
}

static bool8 IsPikaPowerBoltAnimating(void)
{
    return gTasks[sSlotMachine->unk3E].data[15];
}

static void (*const sPikaPowerBoltTasks[])(struct Task *task) =
{
    PikaPowerBolt_Idle,
    PikaPowerBolt_AddBolt,
    PikaPowerBolt_WaitAnim,
    PikaPowerBolt_ClearAll
};

static void Task_CreatePikaPowerBolt(u8 taskId)
{
    sPikaPowerBoltTasks[gTasks[taskId].data[0]](gTasks + taskId);
}

static void PikaPowerBolt_Idle(struct Task *task)
{
}

static void PikaPowerBolt_AddBolt(struct Task *task)
{
    task->data[2] = CreatePikaPowerBoltSprite((task->data[1] << 3) + 20, 20);
    task->data[0]++;
}

static const u16 sPikaPowerTileTable[][2] =
{
    {0x9e, 0x6e},
    {0x9f, 0x6f},
    {0xaf, 0x7f},
};

static void PikaPowerBolt_WaitAnim(struct Task *task)
{
    u16 *vaddr = (u16 *)BG_SCREEN_ADDR(29);
    if (gSprites[task->data[2]].data[7])
    {
        s16 r2 = task->data[1] + 2;
        u8 r0 = 0;
        if (task->data[1] == 1)
            r0 = 1;
        else if (task->data[1] == 16)
            r0 = 2;
        vaddr[r2 + 0x40] = sPikaPowerTileTable[r0][0];
        DestroyPikaPowerBoltSprite(task->data[2]);
        task->data[0] = 0;
        task->data[15] = 0;
    }
}

static void PikaPowerBolt_ClearAll(struct Task *task)
{
    u16 *vaddr = (u16 *)BG_SCREEN_ADDR(29);
    s16 r4 = task->data[1] + 2;
    u8 r2 = 0;
    if (task->data[1] == 1)
        r2 = 1;
    else if (task->data[1] == 16)
        r2 = 2;
    if (task->data[2] == 0)
    {
        vaddr[r4 + 0x40] = sPikaPowerTileTable[r2][1];
        task->data[1]--;
    }
    if (++task->data[2] >= 20)
        task->data[2] = 0;
    if (task->data[1] == 0)
    {
        task->data[0] = 0;
        task->data[15] = 0;
    }
}

static void ResetPikaPowerBoltTask(struct Task *task)
{
    u8 i;

    for (i = 2; i < 16; i++)
        task->data[i] = 0;
}

static void LoadPikaPowerMeter(u8 pikaPower)
{
    s16 i;
    u8 r3;
    s16 r2 = 3;
    u16 *vaddr = (u16 *)BG_SCREEN_ADDR(29);
    for (i = 0; i < pikaPower; i++, r2++)
    {
        r3 = 0;
        if (i == 0)
            r3 = 1;
        else if (i == 15)
            r3 = 2;
        vaddr[r2 + 0x40] = sPikaPowerTileTable[r3][0];
    }
    for (; i < 16; i++, r2++)
    {
        r3 = 0;
        if (i == 0)
            r3 = 1;
        else if (i == 15)
            r3 = 2;
        vaddr[r2 + 0x40] = sPikaPowerTileTable[r3][1];
    }
    gTasks[sSlotMachine->unk3E].data[1] = pikaPower;
}

static void BeginReelTime(void)
{
    u8 taskId = CreateTask(Task_ReelTime, 7);
    Task_ReelTime(taskId);
}

static bool8 IsReelTimeTaskDone(void)
{
    if (FindTaskIdByFunc(Task_ReelTime) == 0xFF)
        return TRUE;
    return FALSE;
}

static void (*const sReelTimeTasks[])(struct Task *task) =
{
    ReelTime_Init,
    ReelTime_WindowEnter,
    ReelTime_WaitStartPikachu,
    ReelTime_PikachuSpeedUp1,
    ReelTime_PikachuSpeedUp2,
    ReelTime_WaitReel,
    ReelTime_CheckExplode,
    ReelTime_LandOnOutcome,
    ReelTime_PikachuReact,
    ReelTime_WaitClearPikaPower,
    ReelTime_CloseWindow,
    ReelTime_DestroySprites,
    ReelTime_SetReelSpeed,
    ReelTime_EndSuccess,
    ReelTime_ExplodeMachine,
    ReelTime_WaitExplode,
    ReelTime_WaitSmoke,
    ReelTime_CloseWindow,
    ReelTime_EndFailure
};

static void Task_ReelTime(u8 taskId)
{
    sReelTimeTasks[gTasks[taskId].data[0]](gTasks + taskId);
}

static void ReelTime_Init(struct Task *task)
{
    sSlotMachine->unk0A = 0;
    sSlotMachine->unk14 = 0;
    sSlotMachine->unk16 = 0;
    task->data[0]++;
    task->data[1] = 0;
    task->data[2] = 30;
    task->data[4] = 1280;
    gSpriteCoordOffsetX = 0;
    gSpriteCoordOffsetY = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    LoadReelTimeWindowTilemap(30, 0);
    CreateReelTimeMachineSprites();
    CreateReelTimePikachuSprite();
    CreateReelTimeNumberSprites();
    CreateReelTimeShadowSprites();
    CreateReelTimeNumberGapSprite();
    GetReelTimeDraw();
    StopMapMusic();
    PlayNewMapMusic(MUS_ROULETTE);
}

static void ReelTime_WindowEnter(struct Task *task)
{
    s16 r3;
    gSpriteCoordOffsetX -= 8;
    task->data[1] += 8;
    r3 = ((task->data[1] + 240) & 0xff) >> 3;
    REG_BG1HOFS = task->data[1] & 0x1ff;
    if (r3 != task->data[2] && task->data[3] <= 18)
    {
        task->data[2] = r3;
        task->data[3] = task->data[1] >> 3;
        LoadReelTimeWindowTilemap(r3, task->data[3]);
    }
    if (task->data[1] >= 200)
    {
        task->data[0]++;
        task->data[3] = 0;
    }
    AdvanceReeltimeReel(task->data[4] >> 8);
}

static void ReelTime_WaitStartPikachu(struct Task *task)
{
    AdvanceReeltimeReel(task->data[4] >> 8);
    if (++task->data[5] >= 60)
    {
        task->data[0]++;
        CreateReelTimeBoltSprites();
        CreateReelTimePikachuAuraSprites();
    }
}

static void ReelTime_PikachuSpeedUp1(struct Task *task)
{
    int r5;
    u8 sp0[] = {1, 1, 2, 2};
    s16 sp4[] = {0x40, 0x30, 0x18, 0x08};
    s16 spC[] = {10, 8, 6, 4};

    AdvanceReeltimeReel(task->data[4] >> 8);
    task->data[4] -= 4;
    r5 = 4 - (task->data[4] >> 8);
    SetReelTimeBoltDelay(sp4[r5]);
    SetReelTimePikachuAuraFlashDelay(spC[r5]);
    StartSpriteAnimIfDifferent(gSprites + sSlotMachine->unk3F, sp0[r5]);
    if (task->data[4] <= 0x100)
    {
        task->data[0]++;
        task->data[4] = 0x100;
        task->data[5] = 0;
    }
}

static void ReelTime_PikachuSpeedUp2(struct Task *task)
{
    AdvanceReeltimeReel(task->data[4] >> 8);
    if (++task->data[5] >= 80)
    {
        task->data[0]++;
        task->data[5] = 0;
        SetReelTimePikachuAuraFlashDelay(2);
        StartSpriteAnimIfDifferent(gSprites + sSlotMachine->unk3F, 3);
    }
}

static void ReelTime_WaitReel(struct Task *task)
{
    AdvanceReeltimeReel(task->data[4] >> 8);
    task->data[4] = (u8)task->data[4] + 0x80;
    if (++task->data[5] >= 80)
    {
        task->data[0]++;
        task->data[5] = 0;
    }
}

static void ReelTime_CheckExplode(struct Task *task)
{
    AdvanceReeltimeReel(task->data[4] >> 8);
    task->data[4] = (u8)task->data[4] + 0x40;
    if (++task->data[5] >= 40)
    {
        task->data[5] = 0;
        if (sSlotMachine->unk05)
        {
            if (sSlotMachine->unk0A <= task->data[6])
                task->data[0]++;
        }
        else if (task->data[6] > 3)
        {
            task->data[0]++;
        }
        else if (ShouldReelTimeMachineExplode(task->data[6]))
        {
            task->data[0] = 14;
        }
        task->data[6]++;
    }
}

static void ReelTime_LandOnOutcome(struct Task *task)
{
    s16 r5 = sSlotMachine->unk14 % 20;
    if (r5)
    {
        r5 = AdvanceReeltimeReelToNextSymbol(task->data[4] >> 8);
        task->data[4] = (u8)task->data[4] + 0x40;
    }
    else if (GetReelTimeSymbol(1) != sSlotMachine->unk05)
    {
        AdvanceReeltimeReel(task->data[4] >> 8);
        r5 = sSlotMachine->unk14 % 20;
        task->data[4] = (u8)task->data[4] + 0x40;
    }
    if (r5 == 0 && GetReelTimeSymbol(1) == sSlotMachine->unk05)
    {
        task->data[4] = 0;
        task->data[0]++;
    }
}

static void ReelTime_PikachuReact(struct Task *task)
{
    if (++task->data[4] >= 60)
    {
        StopMapMusic();
        DestroyReelTimeBoltSprites();
        DestroyReelTimePikachuAuraSprites();
        task->data[0]++;
        if(sSlotMachine->unk05 == 0)
        {
            task->data[4] = 0xa0;
            StartSpriteAnimIfDifferent(gSprites + sSlotMachine->unk3F, 5);
            PlayFanfare(MUS_TOO_BAD);
        }
        else
        {
            task->data[4] = 0xc0;
            StartSpriteAnimIfDifferent(gSprites + sSlotMachine->unk3F, 4);
            gSprites[sSlotMachine->unk3F].animCmdIndex = 0;
            if (sSlotMachine->pikaPower)
            {
                ResetPikaPowerBolts();
                sSlotMachine->pikaPower = 0;
            }
            PlayFanfare(MUS_SLOTS_WIN);
        }
    }
}

static void ReelTime_WaitClearPikaPower(struct Task *task)
{
    if ((task->data[4] == 0 || --task->data[4] == 0) && !IsPikaPowerBoltAnimating())
        task->data[0]++;
}

static void ReelTime_CloseWindow(struct Task *task)
{
    s16 r4;
    gSpriteCoordOffsetX -= 8;
    task->data[1] += 8;
    task->data[3] += 8;
    r4 = ((task->data[1] - 8) & 0xff) >> 3;
    REG_BG1HOFS = task->data[1] & 0x1ff;
    if (task->data[3] >> 3 <= 25)
        ClearReelTimeWindowTilemap(r4);
    else
        task->data[0]++;
}

static void ReelTime_DestroySprites(struct Task *task)
{
    sSlotMachine->unk0B = 0;
    sSlotMachine->unk0A = sSlotMachine->unk05;
    gSpriteCoordOffsetX = 0;
    REG_BG1HOFS = 0;
    sSlotMachine->unk1A = 8;
    DestroyReelTimePikachuSprite();
    DestroyReelTimeMachineSprites();
    DestroyReelTimeShadowSprites();
    PlayNewMapMusic(sSlotMachine->backupMapMusic);
    if (sSlotMachine->unk0A == 0)
    {
        DestroyTask(FindTaskIdByFunc(Task_ReelTime));
    }
    else
    {
        CreateDigitalDisplayScene(4);
        task->data[1] = ReelTimeSpeed();
        task->data[2] = 0;
        task->data[3] = 0;
        task->data[0]++;
    }
}

static void ReelTime_SetReelSpeed(struct Task *task)
{
    if (sSlotMachine->unk1A == task->data[1])
        task->data[0]++;
    else if (sSlotMachine->unk1C[0] % 24 == 0 && (++task->data[2]& 0x07) == 0)
        sSlotMachine->unk1A >>= 1;
}

static void ReelTime_EndSuccess(struct Task *task)
{
    if (IsDigitalDisplayAnimFinished())
        DestroyTask(FindTaskIdByFunc(Task_ReelTime));
}

static void ReelTime_ExplodeMachine(struct Task *task)
{
    DestroyReelTimeMachineSprites();
    DestroyReelTimeBoltSprites();
    DestroyReelTimePikachuAuraSprites();
    CreateReelTimeExplosionSprite();
    gSprites[sSlotMachine->unk4E[0]].invisible = TRUE;
    StartSpriteAnimIfDifferent(gSprites + sSlotMachine->unk3F, 5);
    task->data[0]++;
    task->data[4] = 4;
    task->data[5] = 0;
    StopMapMusic();
    PlayFanfare(MUS_TOO_BAD);
    PlaySE(SE_M_EXPLOSION);
}

static void ReelTime_WaitExplode(struct Task *task)
{
    gSpriteCoordOffsetY = task->data[4];
    REG_BG1VOFS = task->data[4];
    if (task->data[5] & 0x01)
        task->data[4] = -task->data[4];
    if ((++task->data[5] & 0x1f) == 0)
        task->data[4] >>= 1;
    if (task->data[4] == 0)
    {
        DestroyReelTimeExplosionSprite();
        CreateReelTimeDuckSprites();
        CreateBrokenReelTimeMachineSprite();
        CreateReelTimeSmokeSprite();
        gSprites[sSlotMachine->unk4E[0]].invisible = FALSE;
        task->data[0]++;
        task->data[5] = 0;
    }
}

static void ReelTime_WaitSmoke(struct Task *task)
{
    gSpriteCoordOffsetY = 0;
    REG_BG1VOFS = 0;
    if (IsReelTimeSmokeAnimFinished())
    {
        task->data[0]++;
        DestroyReelTimeSmokeSprite();
    }
}

static void ReelTime_EndFailure(struct Task *task)
{
    gSpriteCoordOffsetX = 0;
    REG_BG1HOFS = 0;
    PlayNewMapMusic(sSlotMachine->backupMapMusic);
    DestroyReelTimePikachuSprite();
    DestroyBrokenReelTimeMachineSprite();
    DestroyReelTimeShadowSprites();
    DestroyReelTimeDuckSprites();
    DestroyTask(FindTaskIdByFunc(Task_ReelTime));
}

static const u16 sReelTimeWindowTilemap[];

static void LoadReelTimeWindowTilemap(s16 a0, s16 a1)
{
    s16 i;

    for (i = 4; i < 15; i++)
    {
        u16 tile = sReelTimeWindowTilemap[a1 + (i - 4) * 20];
        ((u16 *)BG_SCREEN_ADDR(28))[32 * i + a0] = tile;
    }
}

static void ClearReelTimeWindowTilemap(s16 a0)
{
    s16 i;

    for (i = 4; i < 15; i++)
    {
        ((u16 *)BG_SCREEN_ADDR(28))[32 * i + a0] = 0;
    }
}

static void OpenInfoBox(u8 a0)
{
    u8 taskId = CreateTask(Task_InfoBox, 1);
    gTasks[taskId].data[1] = a0;
    Task_InfoBox(taskId);
}

static bool8 IsInfoBoxClosed(void)
{
    if (FindTaskIdByFunc(Task_InfoBox) == 0xFF)
        return TRUE;
    else
        return FALSE;
}

static void (*const sInfoBoxTasks[])(struct Task *task) =
{
    InfoBox_FadeIn,
    InfoBox_WaitFade,
    InfoBox_DrawWindowAndText,
    InfoBox_WaitFade,
    InfoBox_WaitInput,
    InfoBox_WaitFade,
    InfoBox_RestoreSlotMachineDisplay,
    InfoBox_WaitFade,
    InfoBox_FreeTask
};

static void Task_InfoBox(u8 taskId)
{
    sInfoBoxTasks[gTasks[taskId].data[0]](gTasks + taskId);
}

static void InfoBox_FadeIn(struct Task *task)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    task->data[0]++;
}

static void InfoBox_WaitFade(struct Task *task)
{
    if (!gPaletteFade.active)
        task->data[0]++;
}

static void InfoBox_DrawWindowAndText(struct Task *task)
{
    DestroyDigitalDisplayScene();
    LoadInfoBoxTilemap();
    BasicInitMenuWindow(&gWindowTemplate_81E7144);
    Menu_PrintTextPixelCoords(gOtherText_ReelTime, 10, 32, 1);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    task->data[0]++;
}

static void InfoBox_WaitInput(struct Task *task)
{
    if (JOY_NEW(B_BUTTON | SELECT_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        task->data[0]++;
    }
}

static void InfoBox_RestoreSlotMachineDisplay(struct Task *task)
{
    Menu_EraseScreen();
    BasicInitMenuWindow(&gWindowTemplate_81E7128);
    LoadMenuAndReelOverlayTilemaps();
    CreateDigitalDisplayScene(task->data[1]);
    LoadPikaPowerMeter(sSlotMachine->pikaPower);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    task->data[0]++;
}

static void InfoBox_FreeTask(struct Task *task)
{
    DestroyTask(FindTaskIdByFunc(Task_InfoBox));
}

static void CreateDigitalDisplayTask(void)
{
    u8 i;
    struct Task *task;
    i = CreateTask(Task_DigitalDisplay, 3);
    sSlotMachine->unk3D = i;
    task = gTasks + i;
    task->data[1] = -1;
    for (i = 4; i < 16; i++)
        task->data[i] = MAX_SPRITES;
}

static void LoadSlotMachineReelOverlay(void);

static void CreateDigitalDisplayScene(u8 arg0)
{
    u8 i;
    struct Task *task;

    DestroyDigitalDisplayScene();

    task = gTasks + sSlotMachine->unk3D;
    task->data[1] = arg0;

    for (i = 0; sDigitalDisplayScenes[arg0][i].spriteTemplateId != 0xFF; i++)
    {
        u8 spriteId;
        spriteId = CreateStdDigitalDisplaySprite(
                sDigitalDisplayScenes[arg0][i].spriteTemplateId,
                sDigitalDisplayScenes[arg0][i].dispInfoId,
                sDigitalDisplayScenes[arg0][i].spriteId
        );
        task->data[4 + i] = spriteId;

#ifdef GERMAN
        if (arg0 == 5 && i <= 2)
            gSprites[spriteId].invisible = TRUE;
#endif
    }
}

static void AddDigitalDisplaySprite(u8 a0, SpriteCallback a1, s16 a2, s16 a3, s16 a4)
{
    u8 i;
    struct Task *task = gTasks + sSlotMachine->unk3D;
    for (i = 4; i < 16; i++)
    {
        if (task->data[i] == MAX_SPRITES)
        {
            task->data[i] = CreateDigitalDisplaySprite(a0, a1, a2, a3, a4);
            break;
        }
    }
}

static void (*const sDigitalDisplaySceneExitCallbacks[])(void);

void DestroyDigitalDisplayScene(void)
{
    u8 i;
    struct Task *task = gTasks + sSlotMachine->unk3D;
    if ((u16)task->data[1] != 0xFFFF)
        sDigitalDisplaySceneExitCallbacks[task->data[1]]();
    for (i = 4; i < 16; i++)
    {
        if (task->data[i] != MAX_SPRITES)
        {
            DestroySprite(gSprites + task->data[i]);
            task->data[i] = MAX_SPRITES;
        }
    }
}

static bool8 IsDigitalDisplayAnimFinished(void)
{
    u8 i;
    struct Task *task = gTasks + sSlotMachine->unk3D;
    for (i = 4; i < 16; i++)
    {
        if (task->data[i] != MAX_SPRITES)
        {
            if (gSprites[task->data[i]].data[7])
                return FALSE;
        }
    }
    return TRUE;
}

static void (*const sDigitalDisplayTasks[])(struct Task *task) =
{
    DigitalDisplay_Idle,
};

static void Task_DigitalDisplay(u8 taskId)
{
    sDigitalDisplayTasks[gTasks[taskId].data[0]](gTasks + taskId);
}

static void DigitalDisplay_Idle(struct Task *task)
{
}

static const struct SpriteTemplate sSpriteTemplate_ReelSymbol;

static void CreateReelSymbolSprites(void)
{
    s16 i;
    s16 j;
    s16 x;
    for (i = 0, x = 0x30; i < 3; i++, x += 0x28)
    {
        for (j = 0; j < 120; j += 24)
        {
            struct Sprite *sprite = gSprites + CreateSprite(&sSpriteTemplate_ReelSymbol, x, 0, 14);
            sprite->oam.priority = 3;
            sprite->data[0] = i;
            sprite->data[1] = j;
            sprite->data[3] = -1;
        }
    }
}

static void SpriteCB_ReelSymbol(struct Sprite *sprite)
{
    sprite->data[2] = sSlotMachine->unk1C[sprite->data[0]] + sprite->data[1];
    sprite->data[2] %= 120;
    sprite->y = sSlotMachine->unk22[sprite->data[0]] + 28 + sprite->data[2];
    sprite->sheetTileStart = GetSpriteTileStartByTag(GetSymbolAtRest(sprite->data[0], sprite->data[2] / 24));
    SetSpriteSheetFrameTileNum(sprite);
}

static void CreateCreditPayoutNumberSprites(void)
{
    s16 i;
    s16 x;

    for (x = 203, i = 1; i < 10000; i *= 10, x -= 7)
        CreateCoinNumberSprite(x, 23, 0, i);
    for (x = 235, i = 1; i < 10000; i *= 10, x -= 7)
        CreateCoinNumberSprite(x, 23, 1, i);
}

static const struct SpriteTemplate sSpriteTemplate_CoinNumber;

static void CreateCoinNumberSprite(s16 x, s16 y, u8 a2, s16 a3)
{
    struct Sprite *sprite = gSprites + CreateSprite(&sSpriteTemplate_CoinNumber, x, y, 13);
    sprite->oam.priority = 2;
    sprite->data[0] = a2;
    sprite->data[1] = a3;
    sprite->data[2] = a3 * 10;
    sprite->data[3] = -1;
}

static void SpriteCB_CoinNumber(struct Sprite *sprite)
{
    u16 tag = sSlotMachine->coins;
    if (sprite->data[0])
        tag = sSlotMachine->payout;
    if (sprite->data[3] != tag)
    {
        sprite->data[3] = tag;
        tag %= (u16)sprite->data[2];
        tag /= (u16)sprite->data[1];
        tag += 7;
        sprite->sheetTileStart = GetSpriteTileStartByTag(tag);
        SetSpriteSheetFrameTileNum(sprite);
    }
}

static const struct SpriteTemplate sSpriteTemplate_ReelBackground;
static const struct SubspriteTable sSubspriteTable_ReelBackground[];

static void CreateReelBackgroundSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelBackground, 0x58, 0x48, 15);
    gSprites[spriteId].oam.priority = 3;
    SetSubspriteTables(gSprites + spriteId, sSubspriteTable_ReelBackground);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimePikachu;

static void CreateReelTimePikachuSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimePikachu, 0x118, 0x50, 1);
    gSprites[spriteId].oam.priority = 1;
    gSprites[spriteId].coordOffsetEnabled = TRUE;
    sSlotMachine->unk3F = spriteId;
}

static void DestroyReelTimePikachuSprite(void)
{
    DestroySprite(gSprites + sSlotMachine->unk3F);
}

static void SpriteCB_ReelTimePikachu(struct Sprite *sprite)
{
    sprite->y2 = sprite->x2 = 0;
    if (sprite->animNum == 4)
    {
        sprite->y2 = sprite->x2 = 8;
        if ((sprite->animCmdIndex != 0 && sprite->animDelayCounter != 0) || (sprite->animCmdIndex == 0 && sprite->animDelayCounter == 0))
            sprite->y2 = -8;
    }
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeMachineAntennae;
static const struct SpriteTemplate sSpriteTemplate_ReelTimeMachine;
static const struct SubspriteTable sSubspriteTable_ReelTimeMachineAntennae[];
static const struct SubspriteTable sSubspriteTable_ReelTimeMachine[];

static void CreateReelTimeMachineSprites(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeMachineAntennae, 0x170, 0x34, 7);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sprite->coordOffsetEnabled = TRUE;
    SetSubspriteTables(sprite, sSubspriteTable_ReelTimeMachineAntennae);
    sSlotMachine->unk49[0] = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimeMachine, 0x170, 0x54, 7);
    sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sprite->coordOffsetEnabled = TRUE;
    SetSubspriteTables(sprite, sSubspriteTable_ReelTimeMachine);
    sSlotMachine->unk49[1] = spriteId;
}

static const struct SpriteTemplate sSpriteTemplate_BrokenReelTimeMachine;
static const struct SubspriteTable sSubspriteTable_BrokenReelTimeMachine[];

static void CreateBrokenReelTimeMachineSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_BrokenReelTimeMachine, 0xa8 - gSpriteCoordOffsetX, 0x50, 7);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sprite->coordOffsetEnabled = TRUE;
    SetSubspriteTables(sprite, sSubspriteTable_BrokenReelTimeMachine);
    sSlotMachine->unk42 = spriteId;
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeNumbers;

static void CreateReelTimeNumberSprites(void)
{
    u8 i;
    s16 r5;
    for (i = 0, r5 = 0; i < 3; i++, r5 += 20)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeNumbers, 0x170, 0x00, 10);
        struct Sprite *sprite = &gSprites[spriteId];
        sprite->oam.priority = 1;
        sprite->coordOffsetEnabled = TRUE;
        sprite->data[7] = r5;
        sSlotMachine->unk4B[i] = spriteId;
    }
}

static void SpriteCB_ReelTimeNumbers(struct Sprite *sprite)
{
    s16 r0 = (u16)(sSlotMachine->unk14 + sprite->data[7]);
    r0 %= 40;
    sprite->y = r0 + 59;
    StartSpriteAnimIfDifferent(sprite, GetReelTimeSymbol(r0 / 20));
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeShadow;
static const struct SubspriteTable sSubspriteTable_ReelTimeShadow[];

static void CreateReelTimeShadowSprites(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeShadow, 0x170, 0x64, 9);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->coordOffsetEnabled = TRUE;
    sprite->oam.priority = 1;
    SetSubspriteTables(sprite, sSubspriteTable_ReelTimeShadow);
    sSlotMachine->unk4E[0] = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimeShadow, 0x120, 0x68, 4);
    sprite = &gSprites[spriteId];
    sprite->coordOffsetEnabled = TRUE;
    sprite->oam.priority = 1;
    SetSubspriteTables(sprite, sSubspriteTable_ReelTimeShadow);
    sSlotMachine->unk4E[1] = spriteId;
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeNumberGap;
static const struct SubspriteTable sSubspriteTable_ReelTimeNumberGap[];

static void CreateReelTimeNumberGapSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeNumberGap, 0x170, 0x4c, 11);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->coordOffsetEnabled = TRUE;
    sprite->oam.priority = 1;
    SetSubspriteTables(sprite, sSubspriteTable_ReelTimeNumberGap);
    sSlotMachine->unk40 = spriteId;
}

static void DestroyReelTimeMachineSprites(void)
{
    u8 i;

    DestroySprite(&gSprites[sSlotMachine->unk40]);
    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->unk49[i]]);
    for (i = 0; i < 3; i++)
        DestroySprite(&gSprites[sSlotMachine->unk4B[i]]);
}

static void DestroyReelTimeShadowSprites(void)
{
    u8 i;

    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->unk4E[i]]);
}

static void DestroyBrokenReelTimeMachineSprite(void)
{
    DestroySprite(&gSprites[sSlotMachine->unk42]);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeBolt;

static void CreateReelTimeBoltSprites(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeBolt, 0x98, 0x20, 5);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sprite->hFlip = TRUE;
    sSlotMachine->unk50[0] = spriteId;
    sprite->data[0] = 8;
    sprite->data[1] = -1;
    sprite->data[2] = -1;
    sprite->data[7] = 0x20;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimeBolt, 0xb8, 0x20, 5);
    sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sSlotMachine->unk50[1] = spriteId;
    sprite->data[1] = 1;
    sprite->data[2] = -1;
    sprite->data[7] = 0x20;
}

static void SpriteCB_ReelTimeBolt(struct Sprite *sprite)
{
    if (sprite->data[0] != 0)
    {
        sprite->data[0]--;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->invisible = TRUE;
    }
    else
    {
        sprite->invisible = FALSE;
        sprite->x2 += sprite->data[1];
        sprite->y2 += sprite->data[2];
        if (++sprite->data[3] >= 8)
        {
            sprite->data[0] = sprite->data[7];
            sprite->data[3] = 0;
        }
    }
}

static void SetReelTimeBoltDelay(s16 a0)
{
    gSprites[sSlotMachine->unk50[0]].data[7] = a0;
    gSprites[sSlotMachine->unk50[1]].data[7] = a0;
}

static void DestroyReelTimeBoltSprites(void)
{
    u8 i;

    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->unk50[i]]);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimePikachuAura;

static void CreateReelTimePikachuAuraSprites(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimePikachuAura, 0x48, 0x50, 3);
    gSprites[spriteId].oam.priority = 1;
    gSprites[spriteId].data[0] = 1;
    gSprites[spriteId].data[5] = 0;
    gSprites[spriteId].data[6] = 16;
    gSprites[spriteId].data[7] = 8;
    sSlotMachine->unk52[0] = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimePikachuAura, 0x68, 0x50, 3);
    gSprites[spriteId].oam.priority = 1;
    gSprites[spriteId].hFlip = TRUE;
    sSlotMachine->unk52[1] = spriteId;
}

static const u8 gUnknown_083ECC58[2]; // don't remove this until decompiled through sInitialReelPositions

static void SpriteCB_ReelTimePikachuAura(struct Sprite *sprite)
{
    u8 sp[] = {16, 0};
    if (sprite->data[0] && --sprite->data[6] <= 0)
    {
        MultiplyInvertedPaletteRGBComponents((IndexOfSpritePaletteTag(7) << 4) + 0x103, sp[sprite->data[5]], sp[sprite->data[5]], sp[sprite->data[5]]);
        ++sprite->data[5];
        sprite->data[5] &= 1;
        sprite->data[6] = sprite->data[7];
    }
}

static void SetReelTimePikachuAuraFlashDelay(s16 a0)
{
    gSprites[sSlotMachine->unk52[0]].data[7] = a0;
}

static void DestroyReelTimePikachuAuraSprites(void)
{
    u8 i;
    MultiplyInvertedPaletteRGBComponents((IndexOfSpritePaletteTag(7) << 4) + 0x103, 0, 0, 0);
    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->unk52[i]]);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeExplosion;

static void CreateReelTimeExplosionSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeExplosion, 0xa8, 0x50, 6);
    gSprites[spriteId].oam.priority = 1;
    sSlotMachine->unk41 = spriteId;
}

static void SpriteCB_ReelTimeExplosion(struct Sprite *sprite)
{
    sprite->y2 = gSpriteCoordOffsetY;
}

static void DestroyReelTimeExplosionSprite(void)
{
    DestroySprite(&gSprites[sSlotMachine->unk41]);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeDuck;

static void CreateReelTimeDuckSprites(void)
{
    u8 i;
    u16 sp[] = {0x0, 0x40, 0x80, 0xC0};
    for (i = 0; i < 4; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeDuck, 0x50 - gSpriteCoordOffsetX, 0x44, 0);
        struct Sprite *sprite = &gSprites[spriteId];
        sprite->oam.priority = 1;
        sprite->coordOffsetEnabled = TRUE;
        sprite->data[0] = sp[i];
        sSlotMachine->unk54[i] = spriteId;
    }
}

static void SpriteCB_ReelTimeDuck(struct Sprite *sprite)
{
    sprite->data[0] -= 2;
    sprite->data[0] &= 0xff;
    sprite->x2 = Cos(sprite->data[0], 20);
    sprite->y2 = Sin(sprite->data[0], 6);
    sprite->subpriority = 0;
    if (sprite->data[0] >= 0x80)
    {
        sprite->subpriority = 2;
    }
    if (++sprite->data[1] >= 16)
    {
        sprite->hFlip ^= 1;
        sprite->data[1] = 0;
    }
}

static void DestroyReelTimeDuckSprites(void)
{
    u8 i;
    for (i = 0; i < 4; i++)
    {
        DestroySprite(&gSprites[sSlotMachine->unk54[i]]);
    }
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeSmoke;

static void CreateReelTimeSmokeSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeSmoke, 0xa8, 0x3c, 8);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sprite->oam.affineMode = ST_OAM_AFFINE_DOUBLE;
    InitSpriteAffineAnim(sprite);
    sSlotMachine->unk43 = spriteId;
}

static void SpriteCB_ReelTimeSmoke(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        if (sprite->affineAnimEnded)
            sprite->data[0]++;
    }
    else if (sprite->data[0] == 1)
    {
        sprite->invisible ^= 1;
        if (++sprite->data[2] >= 24)
        {
            sprite->data[0]++;
            sprite->data[2] = 0;
        }
    }
    else
    {
        sprite->invisible = TRUE;
        if (++sprite->data[2] >= 16)
            sprite->data[7] = 1;
    }
    sprite->data[1] &= 0xff;
    sprite->data[1] += 16;
    sprite->y2 -= (sprite->data[1] >> 8);
}

u8 IsReelTimeSmokeAnimFinished(void)
{
    return gSprites[sSlotMachine->unk43].data[7];
}

static void DestroyReelTimeSmokeSprite(void)
{
    struct Sprite *sprite = &gSprites[sSlotMachine->unk43];
    FreeOamMatrix(sprite->oam.matrixNum);
    DestroySprite(sprite);
}

static const struct SpriteTemplate sSpriteTemplate_PikaPowerBolt;

static u8 CreatePikaPowerBoltSprite(s16 x, s16 y)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_PikaPowerBolt, x, y, 12);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 2;
    sprite->oam.affineMode = ST_OAM_AFFINE_DOUBLE;
    InitSpriteAffineAnim(sprite);
    return spriteId;
}

static void SpriteCB_PikaPowerBolt(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
        sprite->data[7] = 1;
}

static void DestroyPikaPowerBoltSprite(u8 spriteId)
{
    struct Sprite *sprite = &gSprites[spriteId];
    FreeOamMatrix(sprite->oam.matrixNum);
    DestroySprite(sprite);
}

static const s16 sDigitalDisplay_SpriteCoords[][2];
static const SpriteCallback sDigitalDisplay_SpriteCallbacks[];

u8 CreateStdDigitalDisplaySprite(u8 templateIdx, u8 cbAndCoordsIdx, s16 a2)
{
    return CreateDigitalDisplaySprite(templateIdx, sDigitalDisplay_SpriteCallbacks[cbAndCoordsIdx], sDigitalDisplay_SpriteCoords[cbAndCoordsIdx][0], sDigitalDisplay_SpriteCoords[cbAndCoordsIdx][1], a2);
}

static const struct SpriteTemplate *const sSpriteTemplates_DigitalDisplay[];
static const struct SubspriteTable *const sSubspriteTables_DigitalDisplay[];

static u8 CreateDigitalDisplaySprite(u8 templateIdx, SpriteCallback callback, s16 x, s16 y, s16 a4)
{
    u8 spriteId = CreateSprite(sSpriteTemplates_DigitalDisplay[templateIdx], x, y, 16);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 3;
    sprite->callback = callback;
    sprite->data[6] = a4;
    sprite->data[7] = 1;
    if (sSubspriteTables_DigitalDisplay[templateIdx])
        SetSubspriteTables(sprite, sSubspriteTables_DigitalDisplay[templateIdx]);
    return spriteId;
}

static void SpriteCB_DigitalDisplay_Static(struct Sprite *sprite)
{
    sprite->data[7] = 0;
}

static void SpriteCB_DigitalDisplay_Smoke(struct Sprite *sprite)
{
    s16 sp0[] = {4, -4, 4, -4};
    s16 sp8[] = {4, 4, -4, -4};

    if (sprite->data[1]++ >= 16)
    {
        sprite->subspriteTableNum ^= 1;
        sprite->data[1] = 0;
    }
    sprite->x2 = 0;
    sprite->y2 = 0;
    if (sprite->subspriteTableNum != 0)
    {
        sprite->x2 = sp0[sprite->data[6]];
        sprite->y2 = sp8[sprite->data[6]];
    }
}

static void SpriteCB_DigitalDisplay_SmokeNE(struct Sprite *sprite)
{
    sprite->hFlip = TRUE;
    SpriteCB_DigitalDisplay_Smoke(sprite);
}

static void SpriteCB_DigitalDisplay_SmokeSW(struct Sprite *sprite)
{
    sprite->vFlip = TRUE;
    SpriteCB_DigitalDisplay_Smoke(sprite);
}

static void SpriteCB_DigitalDisplay_SmokeSE(struct Sprite *sprite)
{
    sprite->hFlip = TRUE;
    sprite->vFlip = TRUE;
    SpriteCB_DigitalDisplay_Smoke(sprite);
}

static void SpriteCB_DigitalDisplay_Reel(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->x += 4;
        if (sprite->x >= 0xd0)
        {
            sprite->x = 0xd0;
            sprite->data[0]++;
        }
        break;
    case 1:
        if (++sprite->data[1] > 90)
            sprite->data[0]++;
        break;
    case 2:
        sprite->x += 4;
        if (sprite->x >= 0x110)
            sprite->data[0]++;
        break;
    case 3:
        sprite->data[7] = 0;
        break;
    }
}

static void SpriteCB_DigitalDisplay_Time(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->x -= 4;
        if (sprite->x <= 0xd0)
        {
            sprite->x = 0xd0;
            sprite->data[0]++;
        }
        break;
    case 1:
        if (++sprite->data[1] > 90)
            sprite->data[0]++;
        break;
    case 2:
        sprite->x -= 4;
        if (sprite->x <= 0x90)
            sprite->data[0]++;
        break;
    case 3:
        sprite->data[7] = 0;
        break;
    }
}

static void SpriteCB_DigitalDisplay_ReelTimeNumber(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        StartSpriteAnim(sprite, sSlotMachine->unk0A - 1);
        sprite->data[0]++;
        // fallthrough
    case 1:
        if (++sprite->data[1] >= 4)
        {
            sprite->data[0]++;
            sprite->data[1] = 0;
        }
        break;
    case 2:
        sprite->x += 4;
        if (sprite->x >= 0xd0)
        {
            sprite->x = 0xd0;
            sprite->data[0]++;
        }
        break;
    case 3:
        if (++sprite->data[1] > 90)
            sprite->data[0]++;
        break;
    case 4:
        sprite->x += 4;
        if (sprite->x >= 0xf8)
            sprite->data[0]++;
        break;
    case 5:
        sprite->data[7] = 0;
        break;
    }
}

static void SpriteCB_DigitalDisplay_PokeballRocking(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->animPaused = TRUE;
        sprite->data[0]++;
        // fallthrough
    case 1:
        sprite->y += 8;
        if (sprite->y >= 0x70)
        {
            sprite->y = 0x70;
            sprite->data[1] = 16;
            sprite->data[0]++;
        }
        break;
    case 2:
        if (sprite->data[2] == 0)
        {
            sprite->y -= sprite->data[1];
            sprite->data[1] = -sprite->data[1];
            if (++sprite->data[3] >= 2)
            {
                sprite->data[1] >>= 2;
                sprite->data[3] = 0;
                if (sprite->data[1] == 0)
                {
                    sprite->data[0]++;
                    sprite->data[7] = 0;
                    sprite->animPaused = FALSE;
                }
            }
        }
        sprite->data[2]++;
        sprite->data[2] &= 0x07;
        break;
    }
}

static void SpriteCB_DigitalDisplay_Stop(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        if (++sprite->data[1] > 8)
            sprite->data[0]++;
        break;
    case 1:
        sprite->y += 2;
        if (sprite->y >= 0x30)
        {
            sprite->y = 0x30;
            sprite->data[0]++;
            sprite->data[7] = 0;
        }
        break;
    }
}

static void SpriteCB_DigitalDisplay_AButtonStop(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->invisible = TRUE;
        if (++sprite->data[1] > 0x20)
        {
            sprite->data[0]++;
            sprite->data[1] = 5;
            sprite->oam.mosaic = TRUE;
            sprite->invisible = FALSE;
            StartSpriteAnim(sprite, 1);
            REG_MOSAIC = ((sprite->data[1] << 4) | sprite->data[1]) << 8;
        }
        break;
    case 1:
        sprite->data[1] -= (sprite->data[2] >> 8);
        if (sprite->data[1] < 0)
            sprite->data[1] = 0;
        REG_MOSAIC = ((sprite->data[1] << 4) | sprite->data[1]) << 8;
        sprite->data[2] &= 0xff;
        sprite->data[2] += 0x80;
        if (sprite->data[1] == 0)
        {
            sprite->data[0]++;
            sprite->data[7] = 0;
            sprite->oam.mosaic = FALSE;
            StartSpriteAnim(sprite, 0);
        }
        break;
    }
}

static const u16 *const sPokeballShiningPalTable[];

static void SpriteCB_DigitalDisplay_PokeballShining(struct Sprite *sprite)
{
    if (sprite->data[1] < 3)
    {
        LoadPalette(sPokeballShiningPalTable[sprite->data[1]], (IndexOfSpritePaletteTag(6) << 4) + 0x100, 0x20);
        if (++sprite->data[2] >= 4)
        {
            sprite->data[1]++;
            sprite->data[2] = 0;
        }
    }
    else
    {
        LoadPalette(sPokeballShiningPalTable[sprite->data[1]], (IndexOfSpritePaletteTag(6) << 4) + 0x100, 0x20);
        if (++sprite->data[2] >= 25)
        {
            sprite->data[1] = 0;
            sprite->data[2] = 0;
        }
    }
    StartSpriteAnimIfDifferent(sprite, 1);
    sprite->data[7] = 0;
}

static void SpriteCB_DigitalDisplay_RegBonus(struct Sprite *sprite)
{
    s16 sp00[] = {0, -40, 0, 0, 48, 0, 24, 0};
    s16 sp10[] = {-32, 0, -32, -48, 0, -48, 0, -48};
    s16 sp20[] = {16, 12, 16, 0, 0, 4, 8, 8};

    switch (sprite->data[0])
    {
    case 0:
        sprite->x2 = sp00[sprite->data[6]];
        sprite->y2 = sp10[sprite->data[6]];
        sprite->data[1] = sp20[sprite->data[6]];
        sprite->data[0]++;
        // fallthrough
    case 1:
        if (sprite->data[1]-- == 0)
            sprite->data[0]++;
        break;
    case 2:
        if (sprite->x2 > 0)
            sprite->x2 -= 4;
        else if (sprite->x2 < 0)
            sprite->x2 += 4;

        if (sprite->y2 > 0)
            sprite->y2 -= 4;
        else if (sprite->y2 < 0)
            sprite->y2 += 4;

        if (sprite->x2 == 0 && sprite->y2 == 0)
            sprite->data[0]++;
        break;
    }
}

static void SpriteCB_DigitalDisplay_BigBonus(struct Sprite *sprite)
{
    s16 sp0[] = {160, 192, 224, 104, 80, 64, 48, 24};

    if (sprite->data[0] == 0)
    {
        sprite->data[0]++;
        sprite->data[1] = 12;
    }
    sprite->x2 = Cos(sp0[sprite->data[6]], sprite->data[1]);
    sprite->y2 = Sin(sp0[sprite->data[6]], sprite->data[1]);
    if (sprite->data[1] != 0)
        sprite->data[1]--;
}

static void SpriteCB_DigitalDisplay_AButtonStart(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
        case 0:
            sSlotMachine->winIn = 0x2f;
            sSlotMachine->winOut = 0x3f;
            sSlotMachine->win0v = 0x2088;
            sprite->invisible = TRUE;
            sprite->data[0]++;
            // fallthrough
        case 1:
            sprite->data[1] += 2;
            sprite->data[2] = sprite->data[1] + 0xb0;
            sprite->data[3] = 0xf0 - sprite->data[1];
            if (sprite->data[2] > 0xd0)
                sprite->data[2] = 0xd0;
            if (sprite->data[3] < 0xd0)
                sprite->data[3] = 0xd0;
            sSlotMachine->win0h = (sprite->data[2] << 8) | sprite->data[3];
            if (sprite->data[1] > 0x33)
            {
                sprite->data[0]++;
                sSlotMachine->winIn = 0x3f;
            }
            break;
        case 2:
            if (sSlotMachine->bet == 0)
                break;
            AddDigitalDisplaySprite(5, SpriteCallbackDummy, 0xd0, 0x74, 0);
            sSlotMachine->win0h = 0xc0e0;
            sSlotMachine->win0v = 0x6880;
            sSlotMachine->winIn = 0x2f;
            sprite->data[0]++;
            sprite->data[1] = 0;
            // fallthrough
        case 3:
            sprite->data[1] += 2;
            sprite->data[2] = sprite->data[1] + 0xc0;
            sprite->data[3] = 0xe0 - sprite->data[1];
            if (sprite->data[2] > 0xd0)
                sprite->data[2] = 0xd0;
            if (sprite->data[3] < 0xd0)
                sprite->data[3] = 0xd0;
            sSlotMachine->win0h = (sprite->data[2] << 8) | sprite->data[3];
            if (sprite->data[1] > 0x0f)
            {
                sprite->data[0]++;
                sSlotMachine->winIn = 0x3f;
            }
            break;
    }
}

static void EndDigitalDisplayScene_Dummy(void)
{
}

static void EndDigitalDisplayScene_StopReel(void)
{
    REG_MOSAIC = 0;
}

static const u16 *const gUnknown_083EDE20;

static void EndDigitalDisplayScene_Win(void)
{
    LoadPalette(gUnknown_083EDE20, (IndexOfSpritePaletteTag(6) << 4) + 0x100, 0x20);
}

static void EndDigitalDisplayScene_InsertBet(void)
{
    sSlotMachine->win0h = 0xf0;
    sSlotMachine->win0v = 0xa0;
    sSlotMachine->winIn = 0x3f;
    sSlotMachine->winOut = 0x3f;
}

static const u8 sReelTimeGfx[];
static const struct SpriteSheet sSlotMachineSpriteSheets[];
static const struct SpritePalette gSlotMachineSpritePalettes[];

static void LoadSlotMachineGfx(void)
{
    LoadReelBackground();
    LZDecompressWram(gSlotMachineReelTimeLights_Gfx, eSlotMachineGfxBuffer);
    LZDecompressWram(sReelTimeGfx, eSlotMachineReelTimeGfxBuffer);
    LoadSpriteSheets(sSlotMachineSpriteSheets);
    LoadSpritePalettes(gSlotMachineSpritePalettes);
}

static const u8 *const sReelBackground_Tilemap;
static const struct SpriteSheet sReelBackgroundSpriteSheet;

static void LoadReelBackground(void)
{
    u8 *dest = eSlotMachineGfxBuffer;
    u8 i = 0;
    const struct SpriteSheet *sheet = &sReelBackgroundSpriteSheet;
    const u8 *src = sReelBackground_Tilemap;
    for (i = 0; i < 0x40; i++)
    {
        u8 j;
        for (j = 0; j < 0x20; j++, dest++)
            *dest = src[j];
    }
    LoadSpriteSheet(sheet);
}

static void LoadMenuGfx(void)
{
    LZDecompressWram(gSlotMachine_Gfx, eSlotMachineGfxBuffer);

    DmaCopyLarge16(3, eSlotMachineGfxBuffer, BG_VRAM, SLOTMACHINE_GFX_TILES * 32, 0x1000);

    LoadPalette(gUnknown_08E95A18, 0, 160);
    LoadPalette(gPalette_83EDE24, 208, 32);
}

static void LoadMenuAndReelOverlayTilemaps(void)
{
    CpuCopy16(gUnknown_08E95AB8, BG_SCREEN_ADDR(29), 20 * 32 * 2);
    LoadSlotMachineReelOverlay();
}

static void LoadSlotMachineReelOverlay(void)
{
    s16 x, y, dx;
    u16 *screen;

    screen = BG_SCREEN_ADDR(30);

    for (x = 4; x < 18; x += 5)
    {
        for (dx = 0; dx < 4; dx++)
        {
            screen[5 * 32 + dx + x] = 0x2051;
            screen[13 * 32 + dx + x] = 0x2851;
            screen[6 * 32 + dx + x] = 0x2061;
            screen[12 * 32 + dx + x] = 0x2861;
        }

        screen[6 * 32 + x] = 0x20BE;
        screen[12 * 32 + x] = 0x28BE;

        for (y = 7; y <= 11; y++)
            screen[y * 32 + x] = 0x20BF;
    }
}

static void SetReelButtonTilemap(s16 arg0, u16 arg1, u16 arg2, u16 arg3, u16 arg4)
{
    u16 *vram = BG_SCREEN_ADDR(29);

    vram[15 * 32 + arg0] = arg1;
    vram[15 * 32 + 1 + arg0] = arg2;
    vram[16 * 32 + arg0] = arg3;
    vram[16 * 32 + 1 + arg0] = arg4;
}

static void LoadInfoBoxTilemap(void)
{
    s16 y, x;
    u16 *screen;

    CpuCopy16(gUnknown_08E95FB8, BG_SCREEN_ADDR(29), 20 * 32 * 2);

    screen = BG_SCREEN_ADDR(30);
    for (y = 0; y < 20; y++)
    {
        for (x = 0; x < 30; x++)
            screen[x + y * 32] = 0;
    }
}

static const u8 sReelSymbols[][21] =
{
    {
        SLOT_MACHINE_TAG_7_RED,
        SLOT_MACHINE_TAG_CHERRY,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_7_BLUE,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_CHERRY,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_7_RED,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_7_BLUE,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_REPLAY
    },
    {
        SLOT_MACHINE_TAG_7_RED,
        SLOT_MACHINE_TAG_CHERRY,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_CHERRY,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_7_BLUE,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_CHERRY,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_CHERRY,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_CHERRY
    },
    {
        SLOT_MACHINE_TAG_7_RED,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_7_BLUE,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_AZURILL,
        SLOT_MACHINE_TAG_POWER,
        SLOT_MACHINE_TAG_REPLAY,
        SLOT_MACHINE_TAG_LOTAD,
        SLOT_MACHINE_TAG_CHERRY
    },
};

static const u8 sReelTimeSymbols[] = {
    1, 0, 5, 4, 3, 2
};

static const s16 sInitialReelPositions[][2] = {
    {0,  6},
    {0, 10},
    {0,  2}
};

static const u8 sSpecialDrawOdds[][3] = {
    {1, 1, 12},
    {1, 1, 14},
    {2, 2, 14},
    {2, 2, 14},
    {2, 3, 16},
    {3, 3, 16}
};

static const u8 sBiasProbabilities_Special[][6] = {
    {25, 25, 30, 40, 40, 50},
    {25, 25, 30, 30, 35, 35},
    {25, 25, 30, 25, 25, 30}
};

static const u8 sBiasProbabilities_Regular[][6] = {
    {20, 25, 25, 20, 25, 25},
    {12, 15, 15, 18, 19, 22},
    {25, 25, 25, 30, 30, 40},
    {25, 25, 20, 20, 15, 15},
    {40, 40, 35, 35, 40, 40}
};

static const u8 sReelTimeProbabilities_NormalGame[][17] = {
    {243, 243, 243,  80,  80,  80,  80,  40,  40,  40,  40,  40,  40,   5,   5,   5,   5},
    {  5,   5,   5, 150, 150, 150, 150, 130, 130, 130, 130, 130, 130, 100, 100, 100,   5},
    {  4,   4,   4,  20,  20,  20,  20,  80,  80,  80,  80,  80,  80, 100, 100, 100,  40},
    {  2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,  45,  45,  45, 100},
    {  1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   5,   5,   5, 100},
    {  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   6}
};

static const u8 sReelTimeProbabilities_LuckyGame[][17] = {
    { 243, 243, 243, 200, 200, 200, 200, 160, 160, 160, 160, 160, 160,  70,  70,  70,   5},
    {   5,   5,   5,  25,  25,  25,  25,   5,   5,   5,   5,   5,   5,   2,   2,   2,   6},
    {   4,   4,   4,  25,  25,  25,  25,  30,  30,  30,  30,  30,  30,  40,  40,  40,  35},
    {   2,   2,   2,   3,   3,   3,   3,  30,  30,  30,  30,  30,  30, 100, 100, 100,  50},
    {   1,   1,   1,   2,   2,   2,   2,  30,  30,  30,  30,  30,  30,  40,  40,  40, 100},
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   4,   4,   4,  60}
};

static const u16 sReelTimeExplodeProbability[] = {
    0x80, 0xaf, 0xc8, 0xe1, 0x100
};

static const u16 sReelTimeSpeed_Probabilities[][2] = {
    {10,  5},
    {10, 10},
    {10, 15},
    {10, 25},
    {10, 35}
};

static const u16 sQuarterSpeed_ProbabilityBoost[] = {
    0, 5, 10, 15, 20
};


static const u8 sBiasSymbols[] = {
    6, 4, 3, 2, 5, 0, 0, 0
};

static const u16 sBiasesSpecial[] = {
    0x80, 0x20, 0x40
};

static const u16 sBiasesRegular[] = {
    0x10, 0x08, 0x04, 0x02, 0x01
};

static const u8 sSymbolToMatch[] = {
    SLOT_MACHINE_MATCHED_777_RED,
    SLOT_MACHINE_MATCHED_777_BLUE,
    SLOT_MACHINE_MATCHED_AZURILL,
    SLOT_MACHINE_MATCHED_LOTAD,
    SLOT_MACHINE_MATCHED_1CHERRY,
    SLOT_MACHINE_MATCHED_POWER,
    SLOT_MACHINE_MATCHED_REPLAY
};

static const u16 sSlotMatchFlags[] = {
    1 << SLOT_MACHINE_MATCHED_1CHERRY,
    1 << SLOT_MACHINE_MATCHED_2CHERRY,
    1 << SLOT_MACHINE_MATCHED_REPLAY,
    1 << SLOT_MACHINE_MATCHED_LOTAD,
    1 << SLOT_MACHINE_MATCHED_AZURILL,
    1 << SLOT_MACHINE_MATCHED_POWER,
    1 << SLOT_MACHINE_MATCHED_777_MIXED,
    1 << SLOT_MACHINE_MATCHED_777_RED,
    1 << SLOT_MACHINE_MATCHED_777_BLUE
};

static const u16 sSlotPayouts[] = {
    2, 4, 0, 6, 12, 3, 90, 300, 300
};

static const s16 sDigitalDisplay_SpriteCoords[][2] = {
    { 0xd0, 0x38},
    { 0xb8, 0x00},
    { 0xc8, 0x08},
    { 0xd8, 0x10},
    { 0xe8, 0x18},
    { 0xd0, 0x48},
    { 0xd0, 0x08},
    { 0xd0, 0x40},
    { 0xd0, 0x38},
    { 0xc0, 0x58},
    { 0xe0, 0x58},
    { 0xc0, 0x78},
    { 0xe0, 0x78},
    { 0x90, 0x38},
    {0x110, 0x58},
    { 0xa8, 0x70},
    { 0xd0, 0x54},
    { 0xd0, 0x70},
    { 0xbc, 0x34},
    { 0xd0, 0x34},
    { 0xe4, 0x34},
    { 0xb8, 0x48},
    { 0xc4, 0x48},
    { 0xd0, 0x48},
    { 0xdc, 0x48},
    { 0xe8, 0x48},
    { 0xbc, 0x34},
    { 0xd0, 0x34},
    { 0xe4, 0x34},
    { 0xb8, 0x48},
    { 0xc4, 0x48},
    { 0xd0, 0x48},
    { 0xdc, 0x48},
    { 0xe8, 0x48},
    { 0x00, 0x00}
};

static const SpriteCallback sDigitalDisplay_SpriteCallbacks[] = {
    SpriteCB_DigitalDisplay_Static,
    SpriteCB_DigitalDisplay_Stop,
    SpriteCB_DigitalDisplay_Stop,
    SpriteCB_DigitalDisplay_Stop,
    SpriteCB_DigitalDisplay_Stop,
    SpriteCB_DigitalDisplay_AButtonStop,
    SpriteCB_DigitalDisplay_PokeballRocking,
    SpriteCB_DigitalDisplay_Static,
    SpriteCB_DigitalDisplay_Static,
    SpriteCB_DigitalDisplay_Smoke,
    SpriteCB_DigitalDisplay_SmokeNE,
    SpriteCB_DigitalDisplay_SmokeSW,
    SpriteCB_DigitalDisplay_SmokeSE,
    SpriteCB_DigitalDisplay_Reel,
    SpriteCB_DigitalDisplay_Time,
    SpriteCB_DigitalDisplay_ReelTimeNumber,
    SpriteCB_DigitalDisplay_Static,
    SpriteCB_DigitalDisplay_PokeballShining,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_RegBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_BigBonus,
    SpriteCB_DigitalDisplay_AButtonStart
};

static const struct DigitalDisplaySprite sDigitalDisplay_InsertBet[] = {
    {25, 34, 0},
    {2, 0, 0},
    {9, 16, 0},
    {255, 0, 0}
};

static const struct DigitalDisplaySprite sDigitalDisplay_StopReel[] = {
    {10, 1, 0},
    {11, 2, 0},
    {12, 3, 0},
    {13, 4, 0},
    {5, 5, 0},
    {8, 6, 0},
    {255, 0, 0}
};

static const struct DigitalDisplaySprite sDigitalDisplay_Win[] = {
    {3, 7, 0},
    {8, 17, 0},
    {255, 0, 0}
};

static const struct DigitalDisplaySprite sDigitalDisplay_Lose[] = {
    {4, 8, 0},
    {6, 9, 0},
    {6, 10, 1},
    {6, 11, 2},
    {6, 12, 3},
    {255, 0, 0}
};

static const struct DigitalDisplaySprite sDigitalDisplay_ReelTime[] = {
    {0, 13, 0},
    {1, 14, 0},
    {7, 15, 0},
    {255, 0, 0}
};

static const struct DigitalDisplaySprite sDigitalDisplay_BonusBig[] = {
    {19, 26, 0},
    {20, 27, 1},
    {21, 28, 2},
    {14, 29, 3},
    {15, 30, 4},
    {16, 31, 5},
    {17, 32, 6},
    {18, 33, 7},
    {8, 17, 0},
    {255, 0, 0}
};

static const struct DigitalDisplaySprite sDigitalDisplay_BonusRegular[] = {
    {22, 18, 0},
    {23, 19, 1},
    {24, 20, 2},
    {14, 21, 3},
    {15, 22, 4},
    {16, 23, 5},
    {17, 24, 6},
    {18, 25, 7},
    {8, 17, 0},
    {255, 0, 0}
};

static const struct DigitalDisplaySprite *const sDigitalDisplayScenes[] = {
    sDigitalDisplay_InsertBet,
    sDigitalDisplay_StopReel,
    sDigitalDisplay_Win,
    sDigitalDisplay_Lose,
    sDigitalDisplay_ReelTime,
    sDigitalDisplay_BonusRegular,
    sDigitalDisplay_BonusBig
};

static void (*const sDigitalDisplaySceneExitCallbacks[])(void) = {
    EndDigitalDisplayScene_InsertBet,
    EndDigitalDisplayScene_StopReel,
    EndDigitalDisplayScene_Win,
    EndDigitalDisplayScene_Dummy,
    EndDigitalDisplayScene_Dummy,
    EndDigitalDisplayScene_Win,
    EndDigitalDisplayScene_Win
};


static const struct OamData sOam_8x8 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOam_8x16 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_V_RECTANGLE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOam_16x16 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOam_16x32 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_V_RECTANGLE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOam_32x32 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOam_32x64 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_V_RECTANGLE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOam_64x32 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_H_RECTANGLE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct OamData sOam_64x64 = {
    .y = 0x0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .x = 0x0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0x0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct SpriteFrameImage sImageTable_ReelTimePikachu[] = {
    {eSlotMachineReelTimeGfxBuffer + 0x0000, 0x800},
    {eSlotMachineReelTimeGfxBuffer + 0x0800, 0x800},
    {eSlotMachineReelTimeGfxBuffer + 0x1000, 0x800},
    {eSlotMachineReelTimeGfxBuffer + 0x1800, 0x800},
    {eSlotMachineReelTimeGfxBuffer + 0x2000, 0x800}
};

static const struct SpriteFrameImage sImageTable_ReelTimeMachineAntennae[] = {
    {eSlotMachineReelTimeGfxBuffer + 0x2800, 0x300}
};

static const struct SpriteFrameImage sImageTable_ReelTimeMachine[] = {
    {eSlotMachineReelTimeGfxBuffer + 0x2B00, 0x500}
};

static const struct SpriteFrameImage sImageTable_BrokenReelTimeMachine[] = {
    {eSlotMachineReelTimeGfxBuffer + 0x3000, 0x600}
};

static const struct SpriteFrameImage sImageTable_ReelTimeNumbers[] = {
    {gSpriteImage_8E988E8, 0x80},
    {gSpriteImage_8E98968, 0x80},
    {gSpriteImage_8E989E8, 0x80},
    {gSpriteImage_8E98A68, 0x80},
    {gSpriteImage_8E98AE8, 0x80},
    {gSpriteImage_8E98B68, 0x80}
};

static const struct SpriteFrameImage sImageTable_ReelTimeShadow[] = {
    {gSpriteImage_8E991E8, 0x200}
};

static const struct SpriteFrameImage sImageTable_ReelTimeNumberGap[] = {
    {gSpriteImage_8E99808, 0x40}
};

static const struct SpriteFrameImage sImageTable_ReelTimeBolt[] = {
    {gSpriteImage_8E98BE8, 0x100},
    {gSpriteImage_8E98CE8, 0x100}
};

static const struct SpriteFrameImage sImageTable_ReelTimePikachuAura[] = {
    {gSpriteImage_8E993E8, 0x400}
};

static const struct SpriteFrameImage sImageTable_ReelTimeExplosion[] = {
    {gSpriteImage_8E98DE8, 0x200},
    {gSpriteImage_8E98FE8, 0x200}
};

static const struct SpriteFrameImage sImageTable_ReelTimeDuck[] = {
    {gSpriteImage_8E98848, 0x20}
};

static const struct SpriteFrameImage sImageTable_ReelTimeSmoke[] = {
    {gSpriteImage_8E98868, 0x80}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Reel[] = {
    {eSlotMachineGfxBuffer + 0x0000, 0x600}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Time[] = {
    {eSlotMachineGfxBuffer + 0x0600, 0x200}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Insert[] = {
    {eSlotMachineGfxBuffer + 0x0800, 0x200}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Stop[] = {
    {eSlotMachineGfxBuffer + 0x0A00, 0x200}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Win[] = {
    {eSlotMachineGfxBuffer + 0x0C00, 0x300}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Lose[] = {
    {eSlotMachineGfxBuffer + 0x1000, 0x400}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Bonus[] = {
    {eSlotMachineGfxBuffer + 0x1400, 0x200}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Big[] = {
    {eSlotMachineGfxBuffer + 0x1600, 0x300}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Reg[] = {
    {eSlotMachineGfxBuffer + 0x1900, 0x300}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_AButton[] = {
    {eSlotMachineGfxBuffer + 0x1C00, 0x200},
    {eSlotMachineGfxBuffer + 0x1E00, 0x200},
    {eSlotMachineGfxBuffer + 0x1E00, 0x200} // is this a typo?
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Smoke[] = {
    {eSlotMachineGfxBuffer + 0x2000, 0x280}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Number[] = {
    {eSlotMachineGfxBuffer + 0x2280, 0x80},
    {eSlotMachineGfxBuffer + 0x2300, 0x80},
    {eSlotMachineGfxBuffer + 0x2380, 0x80},
    {eSlotMachineGfxBuffer + 0x2400, 0x80},
    {eSlotMachineGfxBuffer + 0x2480, 0x80}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_Pokeball[] = {
    {eSlotMachineGfxBuffer + 0x2600, 0x480},
    {eSlotMachineGfxBuffer + 0x2A80, 0x480}
};

static const struct SpriteFrameImage sImageTable_DigitalDisplay_DPad[] = {
    {eSlotMachineGfxBuffer + 0x2F00, 0x180},
    {eSlotMachineGfxBuffer + 0x3080, 0x180}
};

static const struct SpriteFrameImage sImageTable_PikaPowerBolt[] = {
    {gSpriteImage_8E98828, 0x20}
};

static const union AnimCmd gSpriteAnim_83ED230[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED238[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED240[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED248[] = {
    ANIMCMD_FRAME(1, 16),
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED254[] = {
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED260[] = {
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED26C[] = {
    ANIMCMD_FRAME(2, 32),
    ANIMCMD_FRAME(3, 32),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED278[] = {
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED280[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED288[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED290[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED298[] = {
    ANIMCMD_FRAME(3, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED2A0[] = {
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED2A8[] = {
    ANIMCMD_FRAME(5, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED2B0[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED2BC[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_FRAME(1, 16),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED2C8[] = {
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED2D4[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED2DC[] = {
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED2E8[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_FRAME(1, 16),
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_FRAME(1, 16, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_83ED2FC[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED304[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED30C[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED314[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED31C[] = {
    ANIMCMD_FRAME(3, 1),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_83ED324[] = {
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END
};

static const union AnimCmd *const gSpriteAnimTable_83ED32C[] = {
    gSpriteAnim_83ED230
};

static const union AnimCmd *const gSpriteAnimTable_83ED330[] = {
    gSpriteAnim_83ED238
};

static const union AnimCmd *const gSpriteAnimTable_83ED334[] = {
    gSpriteAnim_83ED240,
    gSpriteAnim_83ED248,
    gSpriteAnim_83ED254,
    gSpriteAnim_83ED260,
    gSpriteAnim_83ED26C,
    gSpriteAnim_83ED278
};

static const union AnimCmd *const gSpriteAnimTable_83ED34C[] = {
    gSpriteAnim_83ED280,
    gSpriteAnim_83ED288,
    gSpriteAnim_83ED290,
    gSpriteAnim_83ED298,
    gSpriteAnim_83ED2A0,
    gSpriteAnim_83ED2A8
};

static const union AnimCmd *const gSpriteAnimTable_83ED364[] = {
    gSpriteAnim_83ED2B0
};

static const union AnimCmd *const gSpriteAnimTable_83ED368[] = {
    gSpriteAnim_83ED2BC
};

static const union AnimCmd *const gSpriteAnimTable_83ED36C[] = {
    gSpriteAnim_83ED2C8,
    gSpriteAnim_83ED2D4
};

static const union AnimCmd *const gSpriteAnimTable_83ED374[] = {
    gSpriteAnim_83ED2DC
};

static const union AnimCmd *const gSpriteAnimTable_83ED378[] = {
    gSpriteAnim_83ED2E8,
    gSpriteAnim_83ED2FC
};

static const union AnimCmd *const gSpriteAnimTable_83ED380[] = {
    gSpriteAnim_83ED304,
    gSpriteAnim_83ED30C,
    gSpriteAnim_83ED314,
    gSpriteAnim_83ED31C,
    gSpriteAnim_83ED324
};

static const union AffineAnimCmd gSpriteAffineAnim_83ED394[] = {
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0x1, 0x1, 0, 1),
    AFFINEANIMCMD_LOOP(255),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const gSpriteAffineAnimTable_83ED3BC[] = {
    gSpriteAffineAnim_83ED394
};

static const union AffineAnimCmd gSpriteAffineAnim_83ED3C0[] = {
    AFFINEANIMCMD_FRAME(0x0, 0x0, 8, 32),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 6, 32),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 4, 16),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 12, 2),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -12, 4),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 12, 2),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 12, 2),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -12, 4),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 12, 2),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const gSpriteAffineAnimTable_83ED410[] = {
    gSpriteAffineAnim_83ED3C0
};

static const struct SpriteTemplate sSpriteTemplate_ReelSymbol = {
    0, 0, &sOam_32x32, gSpriteAnimTable_83ED32C, NULL, gDummySpriteAffineAnimTable, SpriteCB_ReelSymbol
};

static const struct SpriteTemplate sSpriteTemplate_CoinNumber = {
    7, 4, &sOam_8x16, gSpriteAnimTable_83ED32C, NULL, gDummySpriteAffineAnimTable, SpriteCB_CoinNumber
};

static const struct SpriteTemplate sSpriteTemplate_ReelBackground = {
    17, 0, &sOam_64x64, gSpriteAnimTable_83ED32C, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimePikachu = {
    0xFFFF, 1, &sOam_64x64, gSpriteAnimTable_83ED334, sImageTable_ReelTimePikachu, gDummySpriteAffineAnimTable, SpriteCB_ReelTimePikachu
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeMachineAntennae = {
    0xFFFF, 2, &sOam_8x16, gSpriteAnimTable_83ED32C, sImageTable_ReelTimeMachineAntennae, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeMachine = {
    0xFFFF, 3, &sOam_8x16, gSpriteAnimTable_83ED32C, sImageTable_ReelTimeMachine, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_BrokenReelTimeMachine = {
    0xFFFF, 3, &sOam_8x16, gSpriteAnimTable_83ED32C, sImageTable_BrokenReelTimeMachine, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeNumbers = {
    0xFFFF, 4, &sOam_16x16, gSpriteAnimTable_83ED34C, sImageTable_ReelTimeNumbers, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeNumbers
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeShadow = {
    0xFFFF, 4, &sOam_16x16, gSpriteAnimTable_83ED32C, sImageTable_ReelTimeShadow, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeNumberGap = {
    0xFFFF, 4, &sOam_16x16, gSpriteAnimTable_83ED32C, sImageTable_ReelTimeNumberGap, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeBolt = {
    0xFFFF, 4, &sOam_16x32, gSpriteAnimTable_83ED364, sImageTable_ReelTimeBolt, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeBolt
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimePikachuAura = {
    0xFFFF, 7, &sOam_32x64, gSpriteAnimTable_83ED32C, sImageTable_ReelTimePikachuAura, gDummySpriteAffineAnimTable, SpriteCB_ReelTimePikachuAura
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeExplosion = {
    0xFFFF, 5, &sOam_32x32, gSpriteAnimTable_83ED368, sImageTable_ReelTimeExplosion, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeExplosion
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeDuck = {
    0xFFFF, 4, &sOam_8x8, gSpriteAnimTable_83ED330, sImageTable_ReelTimeDuck, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeDuck
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeSmoke = {
    0xFFFF, 4, &sOam_16x16, gSpriteAnimTable_83ED32C, sImageTable_ReelTimeSmoke, gSpriteAffineAnimTable_83ED3BC, SpriteCB_ReelTimeSmoke
};

static const struct SpriteTemplate gSpriteTemplate_83ED57C = {
    0xFFFF, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Reel, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED594 = {
    0xFFFF, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Time, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED5AC = {
    0xFFFF, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Insert, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED5C4 = {
    18, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Stop, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED5DC = {
    0xFFFF, 6, &sOam_64x32, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Win, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED5F4 = {
    0xFFFF, 6, &sOam_64x32, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Lose, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED60C = {
    19, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Bonus, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED624 = {
    20, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Big, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED63C = {
    21, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Reg, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED654 = {
    0xFFFF, 6, &sOam_32x32, gSpriteAnimTable_83ED36C, sImageTable_DigitalDisplay_AButton, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED66C = {
    0xFFFF, 6, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_DigitalDisplay_Smoke, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED684 = {
    0xFFFF, 6, &sOam_16x16, gSpriteAnimTable_83ED380, sImageTable_DigitalDisplay_Number, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED69C = {
    0xFFFF, 6, &sOam_8x8, gSpriteAnimTable_83ED378, sImageTable_DigitalDisplay_Pokeball, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate gSpriteTemplate_83ED6B4 = {
    0xFFFF, 6, &sOam_8x8, gSpriteAnimTable_83ED374, sImageTable_DigitalDisplay_DPad, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_PikaPowerBolt = {
    0xFFFF, 4, &sOam_8x8, gSpriteAnimTable_83ED32C, sImageTable_PikaPowerBolt, gSpriteAffineAnimTable_83ED410, SpriteCB_PikaPowerBolt
};

static const struct Subsprite gSubspriteTable_83ED6E4[] = {
    {-64, -64, ST_OAM_SQUARE, 3, 0x0, 3},
    {0, -64, ST_OAM_SQUARE, 3, 0x0, 3},
    {-64, 0, ST_OAM_SQUARE, 3, 0x0, 3},
    {0, 0, ST_OAM_SQUARE, 3, 0x0, 3}
};

static const struct SubspriteTable sSubspriteTable_ReelBackground[] = {
    {4, gSubspriteTable_83ED6E4}
};

static const struct Subsprite gSubspriteTable_83ED70C[] = {
    {-32, -12, ST_OAM_H_RECTANGLE, 1, 0x0, 1},
    {0, -12, ST_OAM_H_RECTANGLE, 1, 0x4, 1},
    {-32, -4, ST_OAM_H_RECTANGLE, 1, 0x8, 1},
    {0, -4, ST_OAM_H_RECTANGLE, 1, 0xc, 1},
    {-32, 4, ST_OAM_H_RECTANGLE, 1, 0x10, 1},
    {0, 4, ST_OAM_H_RECTANGLE, 1, 0x14, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeMachineAntennae[] = {
    {6, gSubspriteTable_83ED70C}
};

static const struct Subsprite gSubspriteTable_83ED744[] = {
    {-32, -20, ST_OAM_H_RECTANGLE, 3, 0x0, 1},
    {-32, 12, ST_OAM_H_RECTANGLE, 1, 0x20, 1},
    {0, 12, ST_OAM_H_RECTANGLE, 1, 0x24, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeMachine[] = {
    {3, gSubspriteTable_83ED744}
};

static const struct Subsprite gSubspriteTable_83ED764[] = {
    {-32, -24, ST_OAM_H_RECTANGLE, 3, 0x0, 1},
    {-32, 8, ST_OAM_H_RECTANGLE, 1, 0x20, 1},
    {0, 8, ST_OAM_H_RECTANGLE, 1, 0x24, 1},
    {-32, 16, ST_OAM_H_RECTANGLE, 1, 0x28, 1},
    {0, 16, ST_OAM_H_RECTANGLE, 1, 0x2c, 1}
};

static const struct SubspriteTable sSubspriteTable_BrokenReelTimeMachine[] = {
    {5, gSubspriteTable_83ED764}
};

static const struct Subsprite gSubspriteTable_83ED794[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 1},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 1},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 1},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeShadow[] = {
    {4, gSubspriteTable_83ED794}
};

static const struct Subsprite gSubspriteTable_83ED7BC[] = {
    {-8, -12, ST_OAM_H_RECTANGLE, 0, 0x0, 1},
    {-8, -4, ST_OAM_H_RECTANGLE, 0, 0x0, 1},
    {-8, 4, ST_OAM_H_RECTANGLE, 0, 0x0, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeNumberGap[] = {
    {3, gSubspriteTable_83ED7BC}
};

static const struct Subsprite gSubspriteTable_83ED7DC[] = {
    {-32, -24, ST_OAM_H_RECTANGLE, 3, 0x0, 3},
    {-32, 8, ST_OAM_H_RECTANGLE, 1, 0x20, 3},
    {0, 8, ST_OAM_H_RECTANGLE, 1, 0x24, 3},
    {-32, 16, ST_OAM_H_RECTANGLE, 1, 0x28, 3},
    {0, 16, ST_OAM_H_RECTANGLE, 1, 0x2c, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED804[] = {
    {5, gSubspriteTable_83ED7DC}
};

static const struct Subsprite gSubspriteTable_83ED80C[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED82C[] = {
    {4, gSubspriteTable_83ED80C}
};

static const struct Subsprite gSubspriteTable_83ED834[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED854[] = {
    {4, gSubspriteTable_83ED834}
};

static const struct Subsprite gSubspriteTable_83ED85C[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED87C[] = {
    {4, gSubspriteTable_83ED85C}
};

static const struct Subsprite gSubspriteTable_83ED884[] = {
    {-32, -12, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -12, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, -4, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, -4, ST_OAM_H_RECTANGLE, 1, 0xc, 3},
    {-32, 4, ST_OAM_H_RECTANGLE, 1, 0x10, 3},
    {0, 4, ST_OAM_H_RECTANGLE, 1, 0x14, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED8B4[] = {
    {6, gSubspriteTable_83ED884}
};

static const struct Subsprite gSubspriteTable_83ED8BC[] = {
    {-16, -16, ST_OAM_SQUARE, 2, 0x0, 3}
};

static const struct Subsprite gSubspriteTable_83ED8C4[] = {
    {-8, -8, ST_OAM_SQUARE, 1, 0x10, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED8CC[] = {
    {1, gSubspriteTable_83ED8BC},
    {1, gSubspriteTable_83ED8C4}
};

static const struct Subsprite gSubspriteTable_83ED8DC[] = {
    {-24, -24, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {8, -24, ST_OAM_H_RECTANGLE, 0, 0x4, 3},
    {-24, -16, ST_OAM_H_RECTANGLE, 1, 0x6, 3},
    {8, -16, ST_OAM_H_RECTANGLE, 0, 0xa, 3},
    {-24, -8, ST_OAM_H_RECTANGLE, 1, 0xc, 3},
    {8, -8, ST_OAM_H_RECTANGLE, 0, 0x10, 3},
    {-24, 0, ST_OAM_H_RECTANGLE, 1, 0x12, 3},
    {8, 0, ST_OAM_H_RECTANGLE, 0, 0x16, 3},
    {-24, 8, ST_OAM_H_RECTANGLE, 1, 0x18, 3},
    {8, 8, ST_OAM_H_RECTANGLE, 0, 0x1c, 3},
    {-24, 16, ST_OAM_H_RECTANGLE, 1, 0x1e, 3},
    {8, 16, ST_OAM_H_RECTANGLE, 0, 0x22, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED93C[] = {
    12, gSubspriteTable_83ED8DC
};

static const struct Subsprite gSubspriteTable_83ED944[] = {
    {-16, -12, ST_OAM_H_RECTANGLE, 2, 0x0, 3},
    {-16, 4, ST_OAM_H_RECTANGLE, 0, 0x8, 3},
    {0, 4, ST_OAM_H_RECTANGLE, 0, 0xa, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED95C[] = {
    {3, gSubspriteTable_83ED944}
};

static const struct Subsprite gSubspriteTable_83ED964[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0x8, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED974[] = {
    {2, gSubspriteTable_83ED964}
};

static const struct Subsprite gSubspriteTable_83ED97C[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x2, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xa, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED98C[] = {
    {2, gSubspriteTable_83ED97C}
};

static const struct Subsprite gSubspriteTable_83ED994[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x4, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xc, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED9A4[] = {
    {2, gSubspriteTable_83ED994}
};

static const struct Subsprite gSubspriteTable_83ED9AC[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x6, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xe, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED9BC[] = {
    {2, gSubspriteTable_83ED9AC}
};

static const struct Subsprite gSubspriteTable_83ED9C4[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0x8, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED9D4[] = {
    {2, gSubspriteTable_83ED9C4}
};

static const struct Subsprite gSubspriteTable_83ED9DC[] = {
    {-4, -8, ST_OAM_SQUARE, 0, 0x2, 3},
    {-4, 0, ST_OAM_SQUARE, 0, 0xa, 3}
};

static const struct SubspriteTable gSubspriteTables_83ED9EC[] = {
    {2, gSubspriteTable_83ED9DC}
};

static const struct Subsprite gSubspriteTable_83ED9F4[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x3, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xb, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDA04[] = {
    {2, gSubspriteTable_83ED9F4}
};

static const struct Subsprite gSubspriteTable_83EDA0C[] = {
    {-4, -8, ST_OAM_SQUARE, 0, 0x5, 3},
    {-4, 0, ST_OAM_SQUARE, 0, 0xd, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDA1C[] = {
    {2, gSubspriteTable_83EDA0C}
};

static const struct Subsprite gSubspriteTable_83EDA24[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x6, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xe, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDA34[] = {
    {2, gSubspriteTable_83EDA24}
};

static const struct Subsprite gSubspriteTable_83EDA3C[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x2, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0x8, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xa, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x10, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x12, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDA6C[] = {
    {6, gSubspriteTable_83EDA3C}
};

static const struct Subsprite gSubspriteTable_83EDA74[] = {
    {-8, -12, ST_OAM_H_RECTANGLE, 0, 0x3, 3},
    {-8, -4, ST_OAM_H_RECTANGLE, 0, 0xb, 3},
    {-8, 4, ST_OAM_H_RECTANGLE, 0, 0x13, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDA8C[] = {
    {3, gSubspriteTable_83EDA74}
};

static const struct Subsprite gSubspriteTable_83EDA94[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x5, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x7, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0xd, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xf, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x15, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x17, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDAC4[] = {
    {6, gSubspriteTable_83EDA94}
};

static const struct Subsprite gSubspriteTable_83EDACC[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x2, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0x8, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xa, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x10, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x12, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDAFC[] = {
    {6, gSubspriteTable_83EDACC}
};

static const struct Subsprite gSubspriteTable_83EDB04[] = {
    {-8, -12, ST_OAM_H_RECTANGLE, 0, 0x3, 3},
    {-8, -4, ST_OAM_H_RECTANGLE, 0, 0xb, 3},
    {-8, 4, ST_OAM_H_RECTANGLE, 0, 0x13, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDB1C[] = {
    {3, gSubspriteTable_83EDB04}
};

static const struct Subsprite gSubspriteTable_83EDB24[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x5, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x7, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0xd, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xf, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x15, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x17, 3}
};

static const struct SubspriteTable gSubspriteTables_83EDB54[] = {
    {6, gSubspriteTable_83EDB24}
};

static const struct SpriteTemplate *const sSpriteTemplates_DigitalDisplay[] = {
    &gSpriteTemplate_83ED57C,
    &gSpriteTemplate_83ED594,
    &gSpriteTemplate_83ED5AC,
    &gSpriteTemplate_83ED5DC,
    &gSpriteTemplate_83ED5F4,
    &gSpriteTemplate_83ED654,
    &gSpriteTemplate_83ED66C,
    &gSpriteTemplate_83ED684,
    &gSpriteTemplate_83ED69C,
    &gSpriteTemplate_83ED6B4,
    &gSpriteTemplate_83ED5C4,
    &gSpriteTemplate_83ED5C4,
    &gSpriteTemplate_83ED5C4,
    &gSpriteTemplate_83ED5C4,
    &gSpriteTemplate_83ED60C,
    &gSpriteTemplate_83ED60C,
    &gSpriteTemplate_83ED60C,
    &gSpriteTemplate_83ED60C,
    &gSpriteTemplate_83ED60C,
    &gSpriteTemplate_83ED624,
    &gSpriteTemplate_83ED624,
    &gSpriteTemplate_83ED624,
    &gSpriteTemplate_83ED63C,
    &gSpriteTemplate_83ED63C,
    &gSpriteTemplate_83ED63C,
    &gDummySpriteTemplate
};

static const struct SubspriteTable *const sSubspriteTables_DigitalDisplay[] = {
    gSubspriteTables_83ED804,
    gSubspriteTables_83ED82C,
    gSubspriteTables_83ED854,
    gSubspriteTables_83ED8B4,
    NULL,
    NULL,
    gSubspriteTables_83ED8CC,
    NULL,
    gSubspriteTables_83ED93C,
    gSubspriteTables_83ED95C,
    gSubspriteTables_83ED974,
    gSubspriteTables_83ED98C,
    gSubspriteTables_83ED9A4,
    gSubspriteTables_83ED9BC,
    gSubspriteTables_83ED9D4,
    gSubspriteTables_83ED9EC,
    gSubspriteTables_83EDA04,
    gSubspriteTables_83EDA1C,
    gSubspriteTables_83EDA34,
    gSubspriteTables_83EDA6C,
    gSubspriteTables_83EDA8C,
    gSubspriteTables_83EDAC4,
    gSubspriteTables_83EDAFC,
    gSubspriteTables_83EDB1C,
    gSubspriteTables_83EDB54,
    NULL
};

static const struct SpriteSheet sSlotMachineSpriteSheets[] = {
    {gSlotMachineReelSymbol1Tiles, 0x200, 0},
    {gSlotMachineReelSymbol2Tiles, 0x200, 1},
    {gSlotMachineReelSymbol3Tiles, 0x200, 2},
    {gSlotMachineReelSymbol4Tiles, 0x200, 3},
    {gSlotMachineReelSymbol5Tiles, 0x200, 4},
    {gSlotMachineReelSymbol6Tiles, 0x200, 5},
    {gSlotMachineReelSymbol7Tiles, 0x200, 6},
    {gSlotMachineNumber0Tiles, 0x40, 7},
    {gSlotMachineNumber1Tiles, 0x40, 8},
    {gSlotMachineNumber2Tiles, 0x40, 9},
    {gSlotMachineNumber3Tiles, 0x40, 10},
    {gSlotMachineNumber4Tiles, 0x40, 11},
    {gSlotMachineNumber5Tiles, 0x40, 12},
    {gSlotMachineNumber6Tiles, 0x40, 13},
    {gSlotMachineNumber7Tiles, 0x40, 14},
    {gSlotMachineNumber8Tiles, 0x40, 15},
    {gSlotMachineNumber9Tiles, 0x40, 16},
    {eSlotMachineGfxBuffer + 0x0A00, 0x200, 18},
    {eSlotMachineGfxBuffer + 0x1400, 0x200, 19},
    {eSlotMachineGfxBuffer + 0x1600, 0x300, 20},
    {eSlotMachineGfxBuffer + 0x1900, 0x300, 21},
    {}
};

static const struct SpriteSheet sReelBackgroundSpriteSheet = {
    eSlotMachineGfxBuffer + 0x0000, 0x800, 17
};

static const u8 *const sReelBackground_Tilemap = gUnknownPalette_08E997E8;

#ifdef SAPPHIRE
static const u16 UnknownPalette_83EDCE8[] = INCBIN_U16("graphics/unknown/sapphire_83EDD40.gbapal");
#elif defined(RUBY)
static const u16 UnknownPalette_83EDCE8[] = INCBIN_U16("graphics/unknown/ruby_83EDCE8.gbapal");
#endif // RS

static const u16 *const sLitMatchLinePalTable[] = {
    UnknownPalette_83EDCE8 + 10,
    UnknownPalette_83EDCE8 + 11,
    UnknownPalette_83EDCE8 + 12,
    UnknownPalette_83EDCE8 + 13,
    UnknownPalette_83EDCE8 + 14
};

static const u16 *const sDarkMatchLinePalTable[] = {
    gUnknown_08E95A18 + 74,
    gUnknown_08E95A18 + 75,
    gUnknown_08E95A18 + 76,
    gUnknown_08E95A18 + 77,
    gUnknown_08E95A18 + 78
};

static const u8 sMatchLinePalOffsets[] = {
    0x4a, 0x4b, 0x4c, 0x4e, 0x4d
};

static const u8 sBetToMatchLineIds[][2] = {
    {0, 0},
    {1, 2},
    {3, 4}
};
static const u8 sMatchLinesPerBet[] = {1, 2, 2};

#ifdef SAPPHIRE
static const u16 Unknown_83EDD3E[] = INCBIN_U16("graphics/unknown/sapphire_83EDD96.gbapal");
static const u16 Unknown_83EDD5E[] = INCBIN_U16("graphics/unknown/sapphire_83EDDB6.gbapal");
static const u16 Unknown_83EDD7E[] = INCBIN_U16("graphics/unknown/sapphire_83EDDD6.gbapal");
#elif defined (RUBY)
static const u16 Unknown_83EDD3E[] = INCBIN_U16("graphics/unknown/ruby_83EDD3E.gbapal");
static const u16 Unknown_83EDD5E[] = INCBIN_U16("graphics/unknown/ruby_83EDD5E.gbapal");
static const u16 Unknown_83EDD7E[] = INCBIN_U16("graphics/unknown/ruby_83EDD7E.gbapal");
#endif // RS

static const u16 *const sFlashingLightsPalTable[] = {
    Unknown_83EDD3E,
    Unknown_83EDD5E,
    Unknown_83EDD7E
};

static const u16 *const sSlotMachineMenu_Pal = gUnknown_08E95A18 + 16;

static const u16 Palette_83EDDB0[] = INCBIN_U16("graphics/slot_machine/83EDDB0.gbapal");
static const u16 Palette_83EDDD0[] = INCBIN_U16("graphics/slot_machine/83EDDD0.gbapal");
static const u16 Palette_83EDDF0[] = INCBIN_U16("graphics/slot_machine/83EDDF0.gbapal");

static const u16 *const sPokeballShiningPalTable[] = {
    Palette_83EDDB0,
    Palette_83EDDD0,
    Palette_83EDDF0,
    gSlotMachineSpritePalette6
};

static const u16 *const gUnknown_083EDE20 = gSlotMachineSpritePalette6;

static const u16 gPalette_83EDE24[] = INCBIN_U16("graphics/slot_machine/83EDE24_pal.bin");

static const struct SpritePalette gSlotMachineSpritePalettes[] = {
    {gSlotMachineSpritePalette0, 0},
    {gSlotMachineSpritePalette1, 1},
    {gSlotMachineSpritePalette2, 2},
    {gSlotMachineSpritePalette3, 3},
    {gSlotMachineSpritePalette4, 4},
    {gSlotMachineSpritePalette5, 5},
    {gSlotMachineSpritePalette6, 6},
    {gSlotMachineSpritePalette4, 7},
    {}
};

static const u8 sReelTimeGfx[] = INCBIN_U8("graphics/slot_machine/reel_time.4bpp.lz");

static const u16 sReelTimeWindowTilemap[] = INCBIN_U16("graphics/slot_machine/reel_time_window_map.bin");

#if DEBUG

static void debug_sub_811B1C4(void)
{
    unk_debug_bss_1_3 |= 2;
    unk_debug_bss_1_0 = (unk_debug_bss_1_0 == 2) ? 0 : 2;
}

static void debug_sub_811B1EC(void)
{
    unk_debug_bss_1_3 |= 1;
    unk_debug_bss_1_0 = (unk_debug_bss_1_0 == 1) ? 0 : 1;
}

static void debug_sub_811B210(void)
{
    unk_debug_bss_1_3 |= 4;
    unk_debug_bss_1_0 = (unk_debug_bss_1_0 == 4) ? 0 : 4;
}

static void debug_sub_811B238(void)
{
    unk_debug_bss_1_3 |= 8;
    unk_debug_bss_1_0 = (unk_debug_bss_1_0 == 8) ? 0 : 8;
}

static void debug_sub_811B260(void)
{
    unk_debug_bss_1_3 |= 0x10;
    unk_debug_bss_1_0 = (unk_debug_bss_1_0 == 0x10) ? 0 : 0x10;
}

static void debug_sub_811B288(void)
{
    unk_debug_bss_1_3 |= 0x40;
    unk_debug_bss_1_0 = (unk_debug_bss_1_0 == 0x40) ? 0 : 0x40;
}

static void debug_sub_811B2B0(void)
{
    unk_debug_bss_1_3 |= 0x80;
    unk_debug_bss_1_0 = (unk_debug_bss_1_0 == 0x80) ? 0 : 0x80;
}

static void debug_sub_811B2D8(void)
{
    unk_debug_bss_1_3 |= 0x20;
}

static void debug_sub_811B2E8(void)
{
    u8 text[2];

    ConvertIntToDecimalStringN(text, sSlotMachine->unk01 + 1, 2, 1);
    Menu_PrintText(text, 6, 1);
}

static const u8 Str_841B1C4[] = DTR("SETTEI", "SET");
static const u8 Str_841B1CB[] = DTR("MAWASITA", "TURNED");
static const u8 Str_841B1D4[] = DTR("MODOSI", "RETURN");
static const u8 Str_841B1DB[] = DTR("NOMARE", "CONSUMED");
static const u8 Str_841B1E2[] = DTR("MAE　7", "BEFORE 7");
static const u8 Str_841B1E8[] = DTR("LR  HENKOU", "LR: CHANGE");
static const u8 Str_841B1F3[] = DTR("START  JIDOUSU", "START: AUTO");
static const u8 Str_841B202[] = DTR("SELECT  SETTEI", "SELECT: FORCE");
// Irregular Romaji: 抽選 (ちゅうせん/chuusen)
static const u8 Str_841B211[] = DTR("TYUHSEN", "DRAWINGS");
static const u8 Str_841B219[] = _("CHERRY");
static const u8 Str_841B220[] = _("REPLAY");
static const u8 Str_841B227[] = DTR("HASUBO", "LOTAD");
static const u8 Str_841B22E[] = DTR("RURIRI", "AZURILL");
static const u8 Str_841B235[] = DTR("INAZU", "LIGHTNING");
static const u8 Str_841B23B[] = _("REG");
static const u8 Str_841B23F[] = _("BIG");
static const u8 Str_841B243[] = DTR("BD", "REEL TIME");
static const u8 Str_841B246[] = _("R7");
static const u8 Str_841B249[] = _("B7");
static const u8 Str_841B24C[] = DTR("A  COIN", "A: COIN");
static const u8 Str_841B254[] = DTR("TYUHSEN", "DRAWINGS");
static const u8 Str_841B25C[] = _("UD  100");
static const u8 Str_841B264[] = _("LR  1000");
static const u8 Str_841B26D[] = _("×");

void debug_sub_811B310(void)
{
    u8 text[5];

    Menu_PrintText(Str_841B1C4, 1, 1);
    Menu_PrintText(Str_841B1CB, 1, 3);
    Menu_PrintText(Str_841B1D4, 1, 5);
    Menu_PrintText(Str_841B1DB, 1, 7);
    Menu_PrintText(Str_841B1E2, 1, 9);
    Menu_PrintText(Str_841B1E8, 1, 11);
    Menu_PrintText(Str_841B1F3, 1, 13);
    Menu_PrintText(Str_841B202, 1, 15);
    Menu_PrintText(Str_841B24C, 1, 17);
    Menu_PrintText(Str_841B211, 15, 1);
    Menu_PrintText(Str_841B219, 15, 3);
    Menu_PrintText(Str_841B220, 15, 5);
    Menu_PrintText(Str_841B227, 15, 7);
    Menu_PrintText(Str_841B22E, 15, 9);
    Menu_PrintText(Str_841B235, 15, 11);
    Menu_PrintText(Str_841B23B, 15, 13);
    Menu_PrintText(Str_841B23F, 15, 15);
    Menu_PrintText(Str_841B243, 15, 17);
    if (sSlotMachine->unk03 == 0)
        Menu_PrintText(Str_841B246, 10, 9);
    else
        Menu_PrintText(Str_841B249, 10, 9);

#define PRINT_NUMBER(n, x, y)                  \
    ConvertIntToDecimalStringN(text, n, 2, 4); \
    Menu_PrintText(text, x, y);

    PRINT_NUMBER(sSlotMachine->unk68, 10, 3);
    PRINT_NUMBER(sSlotMachine->unk6C, 10, 5);
    PRINT_NUMBER(sSlotMachine->unk10, 10, 7);

#if DEBUG_FIX
#define OFFSET 24 // wider window
#else
#define OFFSET 20
#endif
    PRINT_NUMBER(sSlotMachine->unk70, OFFSET, 3);
    PRINT_NUMBER(sSlotMachine->unk74, OFFSET, 5);
    PRINT_NUMBER(sSlotMachine->unk78, OFFSET, 7);
    PRINT_NUMBER(sSlotMachine->unk7C, OFFSET, 9);
    PRINT_NUMBER(sSlotMachine->unk80, OFFSET, 11);
    PRINT_NUMBER(sSlotMachine->unk84, OFFSET, 13);
    PRINT_NUMBER(sSlotMachine->unk88, OFFSET, 15);
    PRINT_NUMBER(sSlotMachine->unk8C, OFFSET, 17);
#undef OFFSET
#undef PRINT_NUMBER

    if (unk_debug_bss_1_0 != 0)
    {
        u8 y = 0;

        switch (unk_debug_bss_1_0)
        {
        case 2:
            y = 3;
            break;
        case 1:
            y = 5;
            break;
        case 4:
            y = 7;
            break;
        case 8:
            y = 9;
            break;
        case 16:
            y = 11;
            break;
        case 64:
            y = 13;
            break;
        case 128:
            y = 15;
            break;
        }
        Menu_PrintText(Str_841B26D, 23, y);
    }
    debug_sub_811B2E8();
}

static void debug_sub_811B5B4(s32 *a, s32 b)
{
    *a += b;
    if (*a > 9999)
        *a = 9999;
}

static void debug_sub_811B5D0(void)
{
    unk_debug_bss_1_0 = 0;
    unk_debug_bss_1_2 = 0;
    unk_debug_bss_1_3 = 0;
    unk_debug_bss_1_4 = 0;
    sSlotMachine->unk68 = 0;
    sSlotMachine->unk6C = 0;
    sSlotMachine->unk70 = 0;
    sSlotMachine->unk74 = 0;
    sSlotMachine->unk78 = 0;
    sSlotMachine->unk7C = 0;
    sSlotMachine->unk80 = 0;
    sSlotMachine->unk84 = 0;
    sSlotMachine->unk88 = 0;
    sSlotMachine->unk8C = 0;
    sSlotMachine->unk90 = 0;
}

static void debug_sub_811B620(void)
{
    CreateTask(debug_sub_811B654, 0);
}

static u8 debug_sub_811B634(void)
{
    if (FindTaskIdByFunc(debug_sub_811B654) == 0xFF)
        return 1;
    else
        return 0;
}

static const struct {const u8 *text; void (*func)();} _841B270[] =
{
    {Str_841B219, debug_sub_811B1C4},
    {Str_841B220, debug_sub_811B1EC},
    {Str_841B227, debug_sub_811B210},
    {Str_841B22E, debug_sub_811B238},
    {Str_841B235, debug_sub_811B260},
    {Str_841B23B, debug_sub_811B288},
    {Str_841B23F, debug_sub_811B2B0},
    {Str_841B243, debug_sub_811B2D8},
};

static void debug_sub_811B654(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s8 selection;

    switch (task->data[0])
    {
    case 0:
#if DEBUG_FIX
        Menu_DrawStdWindowFrame(0, 0, 28, 19); // wider window
#else
        Menu_DrawStdWindowFrame(0, 0, 24, 19);
#endif

        debug_sub_811B310();
        task->data[0]++;
        break;
    case 1:
        if (JOY_NEW(B_BUTTON))
        {
            Menu_EraseScreen();
            DestroyTask(taskId);
            break;
        }
        if (JOY_NEW(DPAD_LEFT))
        {
            sSlotMachine->unk01--;
            if ((s8)sSlotMachine->unk01 < 0)  // Why? It's unsigned
                sSlotMachine->unk01 = 5;
            debug_sub_811B2E8();
            break;
        }
        if (JOY_NEW(DPAD_RIGHT))
        {
            sSlotMachine->unk01++;
            if (sSlotMachine->unk01 > 5)
                sSlotMachine->unk01 = 0;
            debug_sub_811B2E8();
            break;
        }
        if (JOY_NEW(A_BUTTON))
        {
            task->data[0] = 3;
            Menu_EraseScreen();
            Menu_DrawStdWindowFrame(0, 0, 9, 5);
            Menu_PrintText(Str_841B25C, 1, 1);
            Menu_PrintText(Str_841B264, 1, 3);
            break;
        }
        if (JOY_NEW(SELECT_BUTTON))
        {
            unk_debug_bss_1_2 = 0;
            unk_debug_bss_1_3 = 0;
            Menu_EraseScreen();
            Menu_DrawStdWindowFrame(0, 0, 10, 19);
            Menu_PrintText(Str_841B254, 1, 1);
            Menu_PrintItems(2, 3, 8, (void *)_841B270);
            InitMenu(0, 1, 3, 8, 0, 9);
            task->data[0]++;
        }
        if (JOY_NEW(START_BUTTON))
        {
            unk_debug_bss_1_4 = 1;
            Menu_EraseScreen();
            DestroyTask(taskId);
        }
        break;
    case 2:
        selection = Menu_ProcessInput();
        if (selection == -2)
            break;
        if (selection != -1)
        {
            unk_debug_bss_1_2 = 1;
            _841B270[selection].func();
        }
        Menu_EraseScreen();
        DestroyTask(taskId);
        break;
    case 3:
        if (JOY_REPT(0x80))
        {
            sSlotMachine->coins += 100;
            if (sSlotMachine->coins > 9999)
                sSlotMachine->coins = 9999;
            break;
        }
        if (JOY_REPT(0x40))
        {
            sSlotMachine->coins -= 100;
            if (sSlotMachine->coins <= 0)
                sSlotMachine->coins = 9999;
            break;
        }
        if (JOY_REPT(0x20))
        {
            sSlotMachine->coins -= 1000;
            if (sSlotMachine->coins <= 0)
                sSlotMachine->coins = 9999;
            break;
        }
        if (JOY_REPT(0x10))
        {
            sSlotMachine->coins += 1000;
            if (sSlotMachine->coins > 9999)
                sSlotMachine->coins = 9999;
            break;
        }
        if (JOY_NEW(B_BUTTON))
        {
            Menu_EraseScreen();
            DestroyTask(taskId);
        }
        break;
    }
}

static const u8 Str_841B2B0[] = DTR("·カウントエラーがおきました", "Count error occured.");
static const u8 Str_841B2BF[] = DTR("·リールそうさで　エラーが　おきました", "Reel processing error occurred.");
static const u8 Str_841B2D3[] = DTR("·フラグオフエラーが　おきました", "FLAG OFF error occurred.");
static const u8 Str_841B2E4[] = DTR("·ボーナスこやくの　エラーが　おきました", "BONUS use error occurred."); // TRN

static void debug_sub_811B894(void)
{
    if (sSlotMachine->matchedSymbols & 0x180)
    {
        sSlotMachine->unk90++;
        if (sSlotMachine->unk90 > 9999)
            sSlotMachine->unk90 = 9999;
        if (sSlotMachine->unk90 != sSlotMachine->unk88)
        {
            Menu_PrintText(Str_841B2B0, 4, 15);
            unk_debug_bss_1_4 = 0;
        }
        if (!(sSlotMachine->unk04 & 0x80))
        {
            Menu_PrintText(Str_841B2D3, 4, 17);
            unk_debug_bss_1_4 = 0;
        }
    }
    else if (sSlotMachine->matchedSymbols != 0)
    {
        if ((sSlotMachine->unk04 & 0x80) && !(sSlotMachine->matchedSymbols & 3))
        {
            Menu_PrintText(Str_841B2E4, 4, 2);
            unk_debug_bss_1_4 = 0;
        }
    }
    if (sSlotMachine->matchedSymbols == 0 && sSlotMachine->bet == 3 && !(sSlotMachine->unk04 & 0x80))
    {
        u8 sym_0_1 = GetSymbolAtRest(0, 1);
        u8 sym_0_2 = GetSymbolAtRest(0, 2);
        u8 sym_0_3 = GetSymbolAtRest(0, 3);

        u8 sym_1_1 = GetSymbolAtRest(1, 1);
        u8 sym_1_2 = GetSymbolAtRest(1, 2);
        u8 sym_1_3 = GetSymbolAtRest(1, 3);

        u8 sym_2_1 = GetSymbolAtRest(2, 1);
        u8 sym_2_2 = GetSymbolAtRest(2, 2);
        u8 sym_2_3 = GetSymbolAtRest(2, 3);

        if ((sym_0_1 == 0 && sym_1_1 == 1 && sym_2_1 == 0)
         || (sym_0_2 == 0 && sym_1_2 == 1 && sym_2_2 == 0)
         || (sym_0_3 == 0 && sym_1_3 == 1 && sym_2_3 == 0)
         || (sym_0_1 == 0 && sym_1_2 == 1 && sym_2_3 == 0)
         || (sym_0_3 == 0 && sym_1_2 == 1 && sym_2_1 == 0)
         || (sym_0_1 == 1 && sym_1_1 == 0 && sym_2_1 == 1)
         || (sym_0_2 == 1 && sym_1_2 == 0 && sym_2_2 == 1)
         || (sym_0_3 == 1 && sym_1_3 == 0 && sym_2_3 == 1)
         || (sym_0_1 == 1 && sym_1_2 == 0 && sym_2_3 == 1)
         || (sym_0_3 == 1 && sym_1_2 == 0 && sym_2_1 == 1))
        {
            Menu_PrintText(Str_841B2BF, 4, 0);
            unk_debug_bss_1_4 = 0;
        }
    }
}

#endif
