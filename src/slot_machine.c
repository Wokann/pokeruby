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

#define MAX_BET 3

#define SYMBOLS_PER_REEL   21
#define REEL_SYMBOL_HEIGHT 24
#define REEL_HEIGHT        (SYMBOLS_PER_REEL * REEL_SYMBOL_HEIGHT)

#define REELTIME_SYMBOLS       6
#define REELTIME_SYMBOL_HEIGHT 20
#define REELTIME_REEL_HEIGHT   (REELTIME_SYMBOLS * REELTIME_SYMBOL_HEIGHT)

#define REEL_NORMAL_SPEED  8
#define REEL_HALF_SPEED    4
#define REEL_QUARTER_SPEED 2

#define NUM_SPECIAL_BIASES 3
#define NUM_REGULAR_BIASES 5

#define BIAS_REPLAY     (1 << 0)
#define BIAS_CHERRY     (1 << 1)
#define BIAS_LOTAD      (1 << 2)
#define BIAS_AZURILL    (1 << 3)
#define BIAS_POWER      (1 << 4)
#define BIAS_REELTIME   (1 << 5)
#define BIAS_MIXED_7    (1 << 6)
#define BIAS_STRAIGHT_7 (1 << 7)

#define BIAS_7       (BIAS_STRAIGHT_7 | BIAS_MIXED_7)
#define BIAS_SPECIAL (BIAS_7 | BIAS_REELTIME)
#define BIAS_REGULAR (BIAS_REPLAY | BIAS_CHERRY | BIAS_LOTAD | BIAS_AZURILL | BIAS_POWER)

#define MATCH_FLAG(match) (1 << (match))

#define DIG_SPRITE_DUMMY {255, 0, 0}

enum
{
    SYMBOL_7_RED,
    SYMBOL_7_BLUE,
    SYMBOL_AZURILL,
    SYMBOL_LOTAD,
    SYMBOL_CHERRY,
    SYMBOL_POWER,
    SYMBOL_REPLAY
};

enum
{
    GFXTAG_7_RED,
    GFXTAG_7_BLUE,
    GFXTAG_AZURILL,
    GFXTAG_LOTAD,
    GFXTAG_CHERRY,
    GFXTAG_POWER,
    GFXTAG_REPLAY,
    GFXTAG_NUM_0,
    GFXTAG_NUM_1,
    GFXTAG_NUM_2,
    GFXTAG_NUM_3,
    GFXTAG_NUM_4,
    GFXTAG_NUM_5,
    GFXTAG_NUM_6,
    GFXTAG_NUM_7,
    GFXTAG_NUM_8,
    GFXTAG_NUM_9,
    GFXTAG_REEL_BG,
    GFXTAG_STOP,
    GFXTAG_BONUS,
    GFXTAG_BIG,
    GFXTAG_REG,
};

#define GFXTAG_SYMBOLS_START (GFXTAG_7_RED)
#define GFXTAG_NUMBERS_START (GFXTAG_NUM_0)

enum
{
    PALTAG_REEL,
    PALTAG_REEL_TIME_PIKACHU,
    PALTAG_REEL_TIME_MISC,
    PALTAG_REEL_TIME_MACHINE,
    PALTAG_MISC,
    PALTAG_EXPLOSION,
    PALTAG_DIG_DISPLAY,
    PALTAG_PIKA_AURA,
};

enum
{
    MATCH_CHERRY,
    MATCH_TOPBOT_CHERRY,
    MATCH_REPLAY,
    MATCH_LOTAD,
    MATCH_AZURILL,
    MATCH_POWER,
    MATCH_MIXED_7,
    MATCH_RED_7,
    MATCH_BLUE_7,
    MATCH_NONE
};

enum
{
    MATCH_MIDDLE_ROW,
    MATCH_TOP_ROW,
    MATCH_BOTTOM_ROW,
    MATCH_NWSE_DIAG,
    MATCH_NESW_DIAG,
    NUM_MATCH_LINES,
};

enum
{
    LEFT_REEL,
    MIDDLE_REEL,
    RIGHT_REEL,
    NUM_REELS,
};

enum
{
    SLOTTASK_UNFADE,
    SLOTTASK_WAIT_FADE,
    SLOTTASK_READY_NEW_SPIN,
    SLOTTASK_READY_NEW_RT_SPIN,
    SLOTTASK_ASK_INSERT_BET,
    SLOTTASK_BET_INPUT,
    SLOTTASK_MSG_NEED_3_COINS,
    SLOTTASK_WAIT_MSG_NEED_3_COINS,
    SLOTTASK_WAIT_INFO_BOX,
    SLOTTASK_START_SPIN,
    SLOTTASK_START_RT_SPIN,
    SLOTTASK_RESET_BIAS_FAILURE,
    SLOTTASK_WAIT_REEL_STOP,
    SLOTTASK_WAIT_ALL_REELS_STOP,
    SLOTTASK_CHECK_MATCHES,
    SLOTTASK_WAIT_PAYOUT,
    SLOTTASK_END_PAYOUT,
    SLOTTASK_MATCHED_POWER,
    SLOTTASK_WAIT_RT_ANIM,
    SLOTTASK_RESET_BET_TILES,
    SLOTTASK_NO_MATCHES,
    SLOTTASK_ASK_QUIT,
    SLOTTASK_HANDLE_QUIT_INPUT,
    SLOTTASK_MSG_MAX_COINS,
    SLOTTASK_WAIT_MSG_MAX_COINS,
    SLOTTASK_MSG_NO_MORE_COINS,
    SLOTTASK_WAIT_MSG_NO_MORE_COINS,
    SLOTTASK_END,
    SLOTTASK_FREE,
#if DEBUG
    SLOTTASK_WAIT_DEBUG_MENU,
#endif
};

enum
{
    PAYOUT_TASK_INIT,
    PAYOUT_TASK_GIVE_PAYOUT,
    PAYOUT_TASK_FREE,
};

enum
{
    REEL_TASK_STILL,
    REEL_TASK_SPIN,
    REEL_TASK_DECIDE_STOP,
    REEL_TASK_STOP_MOVE,
    REEL_TASK_STOP_SHAKE,
};

enum
{
    PIKABOLT_TASK_IDLE,
    PIKABOLT_TASK_ADD_BOLT,
    PIKABOLT_TASK_WAIT_ANIM,
    PIKABOLT_TASK_CLEAR_ALL,
};

enum
{
    RT_TASK_INIT,
    RT_TASK_WINDOW_ENTER,
    RT_TASK_WAIT_START_PIKA,
    RT_TASK_PIKA_SPEEDUP1,
    RT_TASK_PIKA_SPEEDUP2,
    RT_TASK_WAIT_REEL,
    RT_TASK_CHECK_EXPLODE,
    RT_TASK_LAND,
    RT_TASK_PIKA_REACT,
    RT_TASK_WAIT_CLEAR_POWER,
    RT_TASK_CLOSE_WINDOW_SUCCESS,
    RT_TASK_DESTROY_SPRITES,
    RT_TASK_SET_REEL_SPEED,
    RT_TASK_END_SUCCESS,
    RT_TASK_EXPLODE,
    RT_TASK_WAIT_EXPLODE,
    RT_TASK_WAIT_SMOKE,
    RT_TASK_CLOSE_WINDOW_FAILURE,
    RT_TASK_END_FAILURE,
};

// Sprite template IDs for the digital display in the right panel
enum
{
    DIG_SPRITE_REEL,
    DIG_SPRITE_TIME,
    DIG_SPRITE_INSERT,
    DIG_SPRITE_WIN,
    DIG_SPRITE_LOSE,
    DIG_SPRITE_A_BUTTON,
    DIG_SPRITE_SMOKE,
    DIG_SPRITE_NUMBER,
    DIG_SPRITE_POKE_BALL,
    DIG_SPRITE_D_PAD,
    DIG_SPRITE_STOP_S,
    DIG_SPRITE_STOP_T,
    DIG_SPRITE_STOP_O,
    DIG_SPRITE_STOP_P,
    DIG_SPRITE_BONUS_B,
    DIG_SPRITE_BONUS_O,
    DIG_SPRITE_BONUS_N,
    DIG_SPRITE_BONUS_U,
    DIG_SPRITE_BONUS_S,
    DIG_SPRITE_BIG_B,
    DIG_SPRITE_BIG_I,
    DIG_SPRITE_BIG_G,
    DIG_SPRITE_REG_R,
    DIG_SPRITE_REG_E,
    DIG_SPRITE_REG_G,
    DIG_SPRITE_EMPTY,
    NUM_DIG_DISPLAY_SPRITES,
};

// IDs used by the digital display to set coords and callbacks for its sprites
enum
{
    DIG_DISPINFO_INSERT,
    DIG_DISPINFO_STOP_S,
    DIG_DISPINFO_STOP_T,
    DIG_DISPINFO_STOP_O,
    DIG_DISPINFO_STOP_P,
    DIG_DISPINFO_A_BUTTON_STOP,
    DIG_DISPINFO_POKE_BALL_ROCKING,
    DIG_DISPINFO_WIN,
    DIG_DISPINFO_LOSE,
    DIG_DISPINFO_SMOKE_NW,
    DIG_DISPINFO_SMOKE_NE,
    DIG_DISPINFO_SMOKE_SW,
    DIG_DISPINFO_SMOKE_SE,
    DIG_DISPINFO_REEL,
    DIG_DISPINFO_TIME,
    DIG_DISPINFO_NUMBER,
    DIG_DISPINFO_DPAD,
    DIG_DISPINFO_POKE_BALL_SHINING,
    DIG_DISPINFO_REG_R,
    DIG_DISPINFO_REG_E,
    DIG_DISPINFO_REG_G,
    DIG_DISPINFO_REG_BONUS_B,
    DIG_DISPINFO_REG_BONUS_O,
    DIG_DISPINFO_REG_BONUS_N,
    DIG_DISPINFO_REG_BONUS_U,
    DIG_DISPINFO_REG_BONUS_S,
    DIG_DISPINFO_BIG_B,
    DIG_DISPINFO_BIG_I,
    DIG_DISPINFO_BIG_G,
    DIG_DISPINFO_BIG_BONUS_B,
    DIG_DISPINFO_BIG_BONUS_O,
    DIG_DISPINFO_BIG_BONUS_N,
    DIG_DISPINFO_BIG_BONUS_U,
    DIG_DISPINFO_BIG_BONUS_S,
    DIG_DISPINFO_A_BUTTON_START,
};

// IDs for digital display scenes
enum
{
    DIG_DISPLAY_INSERT_BET,
    DIG_DISPLAY_STOP_REEL,
    DIG_DISPLAY_WIN,
    DIG_DISPLAY_LOSE,
    DIG_DISPLAY_REEL_TIME,
    DIG_DISPLAY_BONUS_REG,
    DIG_DISPLAY_BONUS_BIG,
};

struct SlotMachine
{
    /*0x00*/ u8 state;
    /*0x01*/ u8 machineId;
    /*0x02*/ u8 pikaPowerBolts;
    /*0x03*/ u8 luckyGame;
    /*0x04*/ u8 machineBias;
    /*0x05*/ u8 reelTimeDraw;
    /*0x06*/ u8 didNotFailBias;
    /*0x07*/ u8 biasSymbol;
    /*0x08*/ u16 matches;
    /*0x0A*/ u8 reelTimeSpinsLeft;
    /*0x0B*/ u8 reelTimeSpinsUsed;
    /*0x0C*/ s16 coins;
    /*0x0E*/ s16 payout;
    /*0x10*/ s16 netCoinLoss;
    /*0x12*/ s16 bet;
    /*0x14*/ s16 reeltimePixelOffset;
    /*0x16*/ s16 reeltimePosition;
    /*0x18*/ s16 currentReel;
    /*0x1A*/ s16 reelSpeed;
    /*0x1C*/ s16 reelPixelOffsets[NUM_REELS];
    /*0x22*/ u16 reelShockOffsets[NUM_REELS];
    /*0x28*/ s16 reelPositions[NUM_REELS];
    /*0x2E*/ s16 reelExtraTurns[NUM_REELS];
    /*0x34*/ s16 winnerRows[NUM_REELS];
    /*0x3A*/ u8 slotReelTasks[NUM_REELS];
    /*0x3D*/ u8 digDisplayTaskId;
    /*0x3E*/ u8 pikaPowerBoltTaskId;
    /*0x3F*/ u8 reelTimePikachuSpriteId;
    /*0x40*/ u8 reelTimeNumberGapSpriteId;
    /*0x41*/ u8 reelTimeExplosionSpriteId;
    /*0x42*/ u8 reelTimeBrokenMachineSpriteId;
    /*0x43*/ u8 reelTimeSmokeSpriteId;
    /*0x44*/ u8 flashMatchLineSpriteIds[NUM_MATCH_LINES];
    /*0x49*/ u8 reelTimeMachineSpriteIds[2];
    /*0x49*/ u8 reelTimeNumberSpriteIds[3];
    /*0x4E*/ u8 reelTimeShadowSpriteIds[2];
    /*0x50*/ u8 reelTimeBoltSpriteIds[2];
    /*0x52*/ u8 reelTimePikachuAuraSpriteIds[2];
    /*0x54*/ u8 reelTimeDuckSpriteIds[4];
    /*0x58*/ u16 win0h;
    /*0x5a*/ u16 win0v;
    /*0x5c*/ u16 winIn;
    /*0x5e*/ u16 winOut;
    /*0x60*/ u16 backupMapMusic;
    /*0x64*/ MainCallback prevMainCb;
#if DEBUG
             u32 debugSpinCount;
             u32 debugPayoutTotal;
             u32 debugBiasDrawsCherry;
             u32 debugBiasDrawsReplay;
             u32 debugBiasDrawsLotad;
             u32 debugBiasDrawsAzurill;
             u32 debugBiasDrawsPower;
             u32 debugBiasDrawsMixed7;
             u32 debugBiasDrawsStraight7;
             u32 debugBiasDrawsReelTime;
             s32 debugMatchedStraight7Count;
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
static void CB2_SlotMachine(void);
static void PlaySlotMachine_Internal(u8 machineId, MainCallback exitCallback);
static void SlotMachineDummyTask(u8 taskId);
static void SlotMachineSetup_ResetCallbacksAndDisplay(void);
static void SlotMachineSetup_InitVBlank(void);
static void SlotMachineSetup_InitVRAM(void);
static void SlotMachineSetup_InitOAM(void);
static void SlotMachineSetup_InitGpuRegs(void);
static void InitSlotMachine(void);
static void SlotMachineSetup_InitMenuWindow(void);
static void SlotMachineSetup_InitPalsSpritesTasks(void);
static void SlotMachineSetup_LoadGfxAndTilemaps(void);
static void CreateSlotMachineSprites(void);
static void CreateGameplayTasks(void);
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
static bool8 SlotTask_WaitDebugMenuClosed(struct Task *);
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
static u8 GetMatchFromSymbols(u8 symbol1, u8 symbol2, u8 symbol3);
static void AwardPayout(void);
static void Task_Payout(u8 taskId);
static bool8 IsFinalTask_Task_Payout(void);
static bool8 PayoutTask_Init(struct Task *task);
static bool8 PayoutTask_GivePayout(struct Task *task);
static bool8 PayoutTask_Free(struct Task *task);
static u8 GetSymbolAtRest(u8 x, s16 y);
static void CreateReelTasks(void);
static void SpinSlotReel(u8 reelIndex);
static void StopSlotReel(u8 reelIndex);
static bool8 IsSlotReelMoving(u8 reelIndex);
static void Task_Reel(u8 taskId);
static bool8 ReelTask_StayStill(struct Task *task);
static bool8 ReelTask_Spin(struct Task *task);
static bool8 ReelTask_DecideStop(struct Task *task);
static bool8 ReelTask_MoveToStop(struct Task *task);
static bool8 ReelTask_ShakingStop(struct Task *task);
static bool8 DecideStop_Bias_Reel1(void);
static bool8 DecideStop_Bias_Reel1_Bet1(u8 symbol1, u8 symbol2);
static bool8 DecideStop_Bias_Reel1_Bet2or3(u8 symbol1, u8 symbol2);
static bool8 DecideStop_Bias_Reel2(void);
static bool8 DecideStop_Bias_Reel2_Bet1or2(void);
static bool8 DecideStop_Bias_Reel2_Bet3(void);
static bool8 DecideStop_Bias_Reel3(void);
static bool8 DecideStop_Bias_Reel3_Bet1or2(u8 biasSymbol);
static bool8 DecideStop_Bias_Reel3_Bet3(u8 biasSymbol);
static void DecideStop_NoBias_Reel1(void);
static void DecideStop_NoBias_Reel2(void);
static void DecideStop_NoBias_Reel2_Bet1(void);
static void DecideStop_NoBias_Reel2_Bet2(void);
static void DecideStop_NoBias_Reel2_Bet3(void);
static void DecideStop_NoBias_Reel3(void);
static void DecideStop_NoBias_Reel3_Bet1(void);
static void DecideStop_NoBias_Reel3_Bet2(void);
static void DecideStop_NoBias_Reel3_Bet3(void);
static void PressStopReelButton(u8 reelIndex);
static void Task_PressStopReelButton(u8 taskId);
static void LightenBetTiles(u8 bet);
static void StopReelButton_Press(struct Task *task, u8 taskId);
static void StopReelButton_Wait(struct Task *task, u8 taskId);
static void StopReelButton_Unpress(struct Task *task, u8 taskId);
static void DarkenBetTiles(u8 bet);
static void CreateInvisibleFlashMatchLineSprites(void);
static void FlashMatchLine(u8 matchLineId);
static bool8 IsMatchLineDoneFlashingBeforePayout(void);
static bool8 TryStopMatchLinesFlashing(void);
static bool8 TryStopMatchLineFlashing(u8 spriteId);
static void SpriteCB_FlashMatchingLines(struct Sprite *sprite);
static void FlashSlotMachineLights(void);
static bool8 TryStopSlotMachineLights(void);
static void Task_FlashSlotMachineLights(u8 taskId);
static void CreatePikaPowerBoltTask(void);
static void AddPikaPowerBolt(u8 pikaPowerBolts);
static bool8 IsPikaPowerBoltAnimating(void);
static void Task_CreatePikaPowerBolt(u8 taskId);
static void PikaPowerBolt_Idle(struct Task *task);
static void PikaPowerBolt_AddBolt(struct Task *task);
static void PikaPowerBolt_WaitAnim(struct Task *task);
static void PikaPowerBolt_ClearAll(struct Task *task);
static void ResetPikaPowerBoltTask(struct Task *task);
static void LoadPikaPowerMeter(u8 pikaPowerBolts);
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
static void LoadReelTimeWindowTilemap(s16 x, s16 tilemapOffset);
static void ClearReelTimeWindowTilemap(s16 x);
static void OpenInfoBox(u8 digDisplayId);
static bool8 IsInfoBoxClosed(void);
static void Task_InfoBox(u8 taskId);
static void InfoBox_FadeIn(struct Task *task);
static void InfoBox_WaitFade(struct Task *task);
static void InfoBox_DrawWindowAndText(struct Task *task);
static void InfoBox_WaitInput(struct Task *task);
static void InfoBox_RestoreSlotMachineDisplay(struct Task *task);
static void InfoBox_FreeTask(struct Task *task);
static void CreateDigitalDisplayTask(void);
static void CreateDigitalDisplayScene(u8 sceneId);
static bool8 IsDigitalDisplayAnimFinished(void);
static void DigitalDisplay_Idle(struct Task *task);
static void Task_DigitalDisplay(u8 taskId);
static void CreateReelSymbolSprites(void);
static void CreateCreditPayoutNumberSprites(void);
static void CreateCoinNumberSprite(s16 x, s16 y, u8 isPayout, s16 digitMultiplier);
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
static void SetReelTimeBoltDelay(s16 delay);
static void DestroyReelTimeBoltSprites(void);
static void CreateReelTimePikachuAuraSprites(void);
static void SetReelTimePikachuAuraFlashDelay(s16 delay);
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
static u8 CreateDigitalDisplaySprite(u8 templateIdx, SpriteCallback callback, s16 x, s16 y, s16 internalSpriteId);
static void LoadSlotMachineGfx(void);
static void LoadReelBackground(void);
static void LoadMenuGfx(void);
static void LoadMenuAndReelOverlayTilemaps(void);
static void SetReelButtonTilemap(s16 x, u16 topLeft, u16 topRight, u16 bottomLeft, u16 bottomRight);
static void LoadInfoBoxTilemap(void);

#if DEBUG
static void DebugSlot_ResetCounters(void);
static void DebugSlot_OpenMenu(void);
static void DebugSlot_AddCounterClamped(s32 *, s32);
static void DebugSlot_CheckErrors(void);
static u8 DebugSlot_IsMenuClosed(void);
static void DebugSlot_Task_Menu(u8 taskId);
#endif

#if DEBUG
static u8 sDebugSlotSelectedBias;
static u8 sDebugSlotEnabled;
static u8 sDebugSlotForceBiasPending;
static u8 sDebugSlotForcedBias;
static u8 sDebugSlotAutoSpinEnabled;
static u32 sDebugSlotAutoSpinDelay;
#endif

static struct SlotMachine *const sSlotMachine = eSlotMachine;

static const struct DigitalDisplaySprite *const sDigitalDisplayScenes[];
static const u16 sUnkPalette[];
static const u8 sSpecialDrawOdds[][MAX_BET];
static const u8 sBiasSymbols[];
static const u16 sBiasesSpecial[NUM_SPECIAL_BIASES];
static const u16 sBiasesRegular[NUM_REGULAR_BIASES];

void PlaySlotMachine(u8 machineId, MainCallback exitCallback)
{
#if DEBUG
    sDebugSlotEnabled = 0;
#endif
    PlaySlotMachine_Internal(machineId, exitCallback);
    SetMainCallback2(CB2_SlotMachineSetup);
}

#if DEBUG
void Debug_PlaySlotMachine(u8 machineId, MainCallback exitCallback)
{
    sDebugSlotEnabled = 1;
    PlaySlotMachine_Internal(machineId, exitCallback);
    SetMainCallback2(CB2_SlotMachineSetup);
}
#endif

static void CB2_SlotMachineSetup(void)
{
    switch (gMain.state)
    {
        case 0:
            SlotMachineSetup_ResetCallbacksAndDisplay();
            InitSlotMachine();
            gMain.state++;
            break;
        case 1:
            SlotMachineSetup_InitVRAM();
            gMain.state++;
            break;
        case 2:
            SlotMachineSetup_InitOAM();
            SlotMachineSetup_InitGpuRegs();
            gMain.state++;
            break;
        case 3:
            SlotMachineSetup_InitMenuWindow();
            gMain.state++;
            break;
        case 4:
            SlotMachineSetup_InitPalsSpritesTasks();
            gMain.state++;
            break;
        case 5:
            SlotMachineSetup_LoadGfxAndTilemaps();
            gMain.state++;
            break;
        case 6:
            CreateSlotMachineSprites();
            CreateGameplayTasks();
            SlotMachineSetup_InitVBlank();
            SetMainCallback2(CB2_SlotMachine);
            break;
    }
}

static void CB2_SlotMachine(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void SlotMachine_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    REG_WIN0H = sSlotMachine->win0h;
    REG_WIN0V = sSlotMachine->win0v;
    REG_WININ = sSlotMachine->winIn;
    REG_WINOUT = sSlotMachine->winOut;
}

#define tMachineId data[0]
#define tExitCallback data[1]

static void PlaySlotMachine_Internal(u8 machineId, MainCallback exitCallback)
{
    struct Task *task = gTasks + CreateTask(SlotMachineDummyTask, 0xFF);
    task->tMachineId = machineId;
    StoreWordInTwoHalfwords(&task->tExitCallback, (intptr_t)exitCallback);
}

static void SlotMachine_InitFromTask(void)
{
    struct Task *task = gTasks + FindTaskIdByFunc(SlotMachineDummyTask);
    sSlotMachine->machineId = task->tMachineId;
    LoadWordFromTwoHalfwords((u16 *)&task->tExitCallback, (u32 *)&sSlotMachine->prevMainCb);
}

static void SlotMachineDummyTask(u8 taskId)
{
}

#undef tMachineId
#undef tExitCallback

static void SlotMachineSetup_ResetCallbacksAndDisplay(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    REG_DISPCNT = 0;
}

static void SlotMachineSetup_InitVBlank(void)
{
    u16 imeBak;
    SetVBlankCallback(SlotMachine_VBlankCB);
    imeBak = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = imeBak;
    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
    REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON;
}

static void SlotMachineSetup_InitVRAM(void)
{
    DmaClearLarge16(3, (u16 *)(BG_VRAM), BG_VRAM_SIZE, 0x1000);
}

static void SlotMachineSetup_InitOAM(void)
{
    DmaClear16(3, (u16 *)OAM, OAM_SIZE);
}

static void SlotMachineSetup_InitGpuRegs(void)
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

static const s16 sInitialReelPositions[NUM_REELS][2];

static void InitSlotMachine(void)
{
    u8 i;

    SlotMachine_InitFromTask();
    sSlotMachine->state = SLOTTASK_UNFADE;
    sSlotMachine->pikaPowerBolts = 0;
    sSlotMachine->luckyGame = Random() & 1;
    sSlotMachine->machineBias = 0;
    sSlotMachine->matches = 0;
    sSlotMachine->reelTimeSpinsLeft = 0;
    sSlotMachine->reelTimeSpinsUsed = 0;
    sSlotMachine->coins = gSaveBlock1.coins;
    sSlotMachine->payout = 0;
    sSlotMachine->netCoinLoss = 0;
    sSlotMachine->bet = 0;
    sSlotMachine->currentReel = 0;
    sSlotMachine->reelSpeed = 8;
    sSlotMachine->win0h = 0xf0;
    sSlotMachine->win0v = 0xa0;
    sSlotMachine->winIn = 0x3f;
    sSlotMachine->winOut = 0x3f;
    sSlotMachine->backupMapMusic = GetCurrentMapMusic();
    for (i = 0; i < NUM_REELS; i++)
    {
        sSlotMachine->reelShockOffsets[i] = 0;
        sSlotMachine->reelPositions[i] = sInitialReelPositions[i][sSlotMachine->luckyGame] % SYMBOLS_PER_REEL;
        sSlotMachine->reelPixelOffsets[i] = REEL_HEIGHT - sSlotMachine->reelPositions[i] * REEL_SYMBOL_HEIGHT;
        sSlotMachine->reelPixelOffsets[i] %= 0x1f8;
    }
#if DEBUG
    DebugSlot_ResetCounters();
    if (sDebugSlotEnabled != 0)
        sSlotMachine->coins = 1000;
#endif
}

static void SlotMachineSetup_InitMenuWindow(void)
{
    Text_LoadWindowTemplate(&gSlotMachineMainWindowTemplate);
    InitMenuWindow(&gSlotMachineMainWindowTemplate);
}

static void SlotMachineSetup_InitPalsSpritesTasks(void)
{
    ResetPaletteFade();
    ResetSpriteData();
    gOamLimit = 128;
    FreeAllSpritePalettes();
    ResetTasks();
}

static void SlotMachineSetup_LoadGfxAndTilemaps(void)
{
    LoadMenuGfx();
    LoadMenuAndReelOverlayTilemaps();
    LoadSlotMachineGfx();
}

static void CreateSlotMachineSprites(void)
{
    CreateReelSymbolSprites();
    CreateCreditPayoutNumberSprites();
    CreateInvisibleFlashMatchLineSprites();
    CreateReelBackgroundSprite();
}

static void CreateGameplayTasks(void)
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

static bool8 (*const sSlotTasks[])(struct Task *task) =
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
    SlotTask_WaitDebugMenuClosed,
#endif
};

static void Task_SlotMachine(u8 taskId)
{
    while (sSlotTasks[sSlotMachine->state](gTasks + taskId))
        ;
}

static bool8 SlotTask_UnfadeScreen(struct Task *task)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    LoadPikaPowerMeter(sSlotMachine->pikaPowerBolts);
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
    sSlotMachine->currentReel = 0;
    sSlotMachine->machineBias &= BIAS_7;
    sSlotMachine->state = SLOTTASK_ASK_INSERT_BET;
    if (sSlotMachine->coins <= 0)
    {
        sSlotMachine->state = SLOTTASK_MSG_NO_MORE_COINS;
    }
    else if (sSlotMachine->reelTimeSpinsLeft)
    {
        sSlotMachine->state = SLOTTASK_READY_NEW_RT_SPIN;
        CreateDigitalDisplayScene(DIG_DISPLAY_REEL_TIME);
    }
    return TRUE;
}

static bool8 SlotTask_ReadyNewReelTimeSpin(struct Task *task)
{
    if (IsDigitalDisplayAnimFinished())
        sSlotMachine->state = SLOTTASK_ASK_INSERT_BET;
    return FALSE;
}

static bool8 SlotTask_AskInsertBet(struct Task *task)
{
    CreateDigitalDisplayScene(DIG_DISPLAY_INSERT_BET);
    sSlotMachine->state = SLOTTASK_BET_INPUT;
    if (
#if DEBUG
      (sDebugSlotEnabled == 0 || sDebugSlotAutoSpinEnabled == 0) &&
#endif
      sSlotMachine->coins >= 9999)
        sSlotMachine->state = SLOTTASK_MSG_MAX_COINS;
    return TRUE;
}

static bool8 SlotTask_HandleBetInput(struct Task *task)
{
    s16 i;

#if DEBUG
    if (sDebugSlotEnabled != 0 && sDebugSlotAutoSpinEnabled != 0)
    {
        if (sSlotMachine->coins <= 3 || JOY_HELD(B_BUTTON))
        {
            sDebugSlotAutoSpinEnabled = 0;
        }
        else
        {
            LightenBetTiles(0);
            LightenBetTiles(1);
            LightenBetTiles(2);
            sSlotMachine->coins -= 3;
            sSlotMachine->bet = 3;
            sSlotMachine->state = SLOTTASK_START_SPIN;
            return 0;
        }
    }
    if (sDebugSlotEnabled != 0 && JOY_NEW(START_BUTTON))
    {
        DebugSlot_OpenMenu();
        sSlotMachine->state = SLOTTASK_WAIT_DEBUG_MENU;
        return 0;
    }
#endif

    if (JOY_NEW(SELECT_BUTTON))
    {
        OpenInfoBox(0);
        sSlotMachine->state = SLOTTASK_WAIT_INFO_BOX;
    }
    else if (JOY_NEW(R_BUTTON))
    {
        if (sSlotMachine->coins - (3 - sSlotMachine->bet) >= 0)
        {
            for (i = sSlotMachine->bet; i < MAX_BET; i++)
                LightenBetTiles(i);
            sSlotMachine->coins -= (MAX_BET - sSlotMachine->bet);
            sSlotMachine->bet = MAX_BET;
            sSlotMachine->state = SLOTTASK_START_SPIN;
            PlaySE(SE_SHOP);
        }
        else
        {
            sSlotMachine->state = SLOTTASK_MSG_NEED_3_COINS;
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
        if (sSlotMachine->bet >= MAX_BET || (sSlotMachine->bet != 0 && JOY_NEW(A_BUTTON)))
            sSlotMachine->state = SLOTTASK_START_SPIN;
        if (JOY_NEW(B_BUTTON))
            sSlotMachine->state = SLOTTASK_ASK_QUIT;
    }
    return FALSE;
}

static void SlotMachine_PrintMessage(const u8 *str)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(str, 2, 15);
}

static bool8 SlotTask_PrintMsg_Need3Coins(struct Task *task)
{
    SlotMachine_PrintMessage(gOtherText_DontHaveThreeCoins);
    sSlotMachine->state = SLOTTASK_WAIT_MSG_NEED_3_COINS;
    return FALSE;
}

static bool8 SlotTask_WaitMsg_Need3Coins(struct Task *task)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Menu_EraseScreen();
        sSlotMachine->state = SLOTTASK_BET_INPUT;
    }
    return FALSE;
}

static bool8 SlotTask_WaitInfoBox(struct Task *task)
{
    if (IsInfoBoxClosed())
        sSlotMachine->state = SLOTTASK_BET_INPUT;
    return FALSE;
}

static bool8 SlotTask_StartSpin(struct Task *task)
{
    DrawMachineBias();
    DestroyDigitalDisplayScene();
    SpinSlotReel(LEFT_REEL);
    SpinSlotReel(MIDDLE_REEL);
    SpinSlotReel(RIGHT_REEL);
    task->data[0] = 0;
    if (sSlotMachine->machineBias & BIAS_REELTIME)
    {
        BeginReelTime();
        sSlotMachine->state = SLOTTASK_START_RT_SPIN;
    }
    else
    {
        CreateDigitalDisplayScene(DIG_DISPLAY_STOP_REEL);
        sSlotMachine->state = SLOTTASK_RESET_BIAS_FAILURE;
    }
    sSlotMachine->reelSpeed = REEL_NORMAL_SPEED;
    if (sSlotMachine->reelTimeSpinsLeft)
        sSlotMachine->reelSpeed = ReelTimeSpeed();
#if DEBUG
    if (sDebugSlotEnabled != 0)
        DebugSlot_AddCounterClamped(&sSlotMachine->debugSpinCount, 1);
#endif
    return FALSE;
}

static bool8 SlotTask_StartReelTimeSpin(struct Task *task)
{
    if (IsReelTimeTaskDone())
    {
        CreateDigitalDisplayScene(DIG_DISPLAY_STOP_REEL);
        sSlotMachine->machineBias &= ~BIAS_REELTIME;
        sSlotMachine->state = SLOTTASK_RESET_BIAS_FAILURE;
    }
    return FALSE;
}

static bool8 SlotTask_ResetBiasFailure(struct Task *task)
{
    if (++task->data[0] >= 30)
    {
#if DEBUG
        if (sDebugSlotEnabled != 0 && sDebugSlotAutoSpinEnabled != 0)
            sDebugSlotAutoSpinDelay = (Random() & 0x1F) + 1;
#endif
        ResetBiasFailure();
        sSlotMachine->state = SLOTTASK_WAIT_REEL_STOP;
    }
    return FALSE;
}

static bool8 SlotTask_WaitReelStop(struct Task *task)
{
#if DEBUG
    if (sDebugSlotEnabled != 0 && sDebugSlotAutoSpinEnabled != 0)
    {
        sDebugSlotAutoSpinDelay--;
        if (sDebugSlotAutoSpinDelay == 0)
        {
            PlaySE(SE_CONTEST_PLACE);
            StopSlotReel(sSlotMachine->currentReel);
            PressStopReelButton(sSlotMachine->currentReel);
            sDebugSlotAutoSpinDelay = (Random() & 0x1F) + 1;
            sSlotMachine->state = SLOTTASK_WAIT_ALL_REELS_STOP;
        }
        return FALSE;
    }
#endif

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_CONTEST_PLACE);
        StopSlotReel(sSlotMachine->currentReel);
        PressStopReelButton(sSlotMachine->currentReel);
        sSlotMachine->state = SLOTTASK_WAIT_ALL_REELS_STOP;
    }
    return FALSE;
}

static bool8 SlotTask_WaitAllReelsStop(struct Task *task)
{
    if (!IsSlotReelMoving(sSlotMachine->currentReel))
    {
        sSlotMachine->currentReel++;
        sSlotMachine->state = SLOTTASK_WAIT_REEL_STOP;
        if (sSlotMachine->currentReel > RIGHT_REEL)
        {
            sSlotMachine->state = SLOTTASK_CHECK_MATCHES;
#if DEBUG
            switch (sDebugSlotSelectedBias)
            {
            case BIAS_CHERRY:
                sSlotMachine->reelPositions[0] = 20;
                sSlotMachine->reelPositions[1] = 20;
                sSlotMachine->reelPositions[2] = 18;
                break;
            case BIAS_REPLAY:
                sSlotMachine->reelPositions[0] = 20;
                sSlotMachine->reelPositions[1] = 20;
                sSlotMachine->reelPositions[2] = 18;
                break;
            case BIAS_LOTAD:
                sSlotMachine->reelPositions[0] = 3;
                sSlotMachine->reelPositions[1] = 1;
                sSlotMachine->reelPositions[2] = 2;
                break;
            case BIAS_AZURILL:
                sSlotMachine->reelPositions[0] = 0;
                sSlotMachine->reelPositions[1] = 2;
                sSlotMachine->reelPositions[2] = 3;
                break;
            case BIAS_POWER:
                sSlotMachine->reelPositions[0] = 2;
                sSlotMachine->reelPositions[1] = 5;
                sSlotMachine->reelPositions[2] = 20;
                break;
            case BIAS_MIXED_7:
                sSlotMachine->reelPositions[0] = 19;
                sSlotMachine->reelPositions[1] = 19;
                sSlotMachine->reelPositions[2] = 0;
                break;
            case BIAS_STRAIGHT_7:
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
    sSlotMachine->machineBias &= BIAS_7;
    CheckMatch();
    if (sSlotMachine->reelTimeSpinsLeft)
    {
        sSlotMachine->reelTimeSpinsLeft--;
        sSlotMachine->reelTimeSpinsUsed++;
    }
#if DEBUG
    else
    {
        DebugSlot_CheckErrors();
    }
#endif

    if (sSlotMachine->matches)
    {
#if DEBUG
        DebugSlot_AddCounterClamped(&sSlotMachine->debugPayoutTotal, sSlotMachine->payout);
#endif
        sSlotMachine->state = SLOTTASK_WAIT_PAYOUT;
        AwardPayout();
        FlashSlotMachineLights();
        if ((sSlotMachine->netCoinLoss -= sSlotMachine->payout) < 0)
        {
            sSlotMachine->netCoinLoss = 0;
        }
        if (sSlotMachine->matches & (MATCH_FLAG(MATCH_BLUE_7) | MATCH_FLAG(MATCH_RED_7)))
        {
            PlayFanfare(MUS_SLOTS_JACKPOT);
            CreateDigitalDisplayScene(DIG_DISPLAY_BONUS_BIG);
        }
        else if (sSlotMachine->matches & MATCH_FLAG(MATCH_MIXED_7))
        {
            PlayFanfare(MUS_SLOTS_JACKPOT);
            CreateDigitalDisplayScene(DIG_DISPLAY_BONUS_REG);
        }
        else
        {
            PlayFanfare(MUS_SLOTS_WIN);
            CreateDigitalDisplayScene(DIG_DISPLAY_WIN);
        }
        if (sSlotMachine->matches & (MATCH_FLAG(MATCH_MIXED_7) | MATCH_FLAG(MATCH_BLUE_7) | MATCH_FLAG(MATCH_RED_7)))
        {
            sSlotMachine->machineBias &= ~BIAS_7;
            if (sSlotMachine->matches & (MATCH_FLAG(MATCH_BLUE_7) | MATCH_FLAG(MATCH_RED_7)))
            {
                sSlotMachine->reelTimeSpinsLeft = 0;
                sSlotMachine->reelTimeSpinsUsed = 0;
                sSlotMachine->luckyGame = 0;
                if (sSlotMachine->matches & MATCH_FLAG(MATCH_BLUE_7))
                    sSlotMachine->luckyGame = 1;
            }
        }
        if (sSlotMachine->matches & MATCH_FLAG(MATCH_POWER) && sSlotMachine->pikaPowerBolts < 16)
        {
            sSlotMachine->pikaPowerBolts++;
            AddPikaPowerBolt(sSlotMachine->pikaPowerBolts);
        }
    }
    else
    {
        CreateDigitalDisplayScene(DIG_DISPLAY_LOSE);
        sSlotMachine->state = SLOTTASK_NO_MATCHES;
        if ((sSlotMachine->netCoinLoss += sSlotMachine->bet) > 9999)
            sSlotMachine->netCoinLoss = 9999;
    }
    return FALSE;
}

static bool8 SlotTask_WaitPayout(struct Task *task)
{
    if (IsFinalTask_Task_Payout())
        sSlotMachine->state = SLOTTASK_END_PAYOUT;
    return FALSE;
}

static bool8 SlotTask_EndPayout(struct Task *task)
{
    if (TryStopSlotMachineLights())
    {
        sSlotMachine->state = SLOTTASK_RESET_BET_TILES;
        if (sSlotMachine->matches & (MATCH_FLAG(MATCH_RED_7) | MATCH_FLAG(MATCH_BLUE_7)))
            IncrementGameStat(GAME_STAT_SLOT_JACKPOTS);
        if (sSlotMachine->matches & MATCH_FLAG(MATCH_REPLAY))
        {
            sSlotMachine->currentReel = 0;
            sSlotMachine->state = SLOTTASK_START_SPIN;
        }
        if (sSlotMachine->matches & MATCH_FLAG(MATCH_POWER))
            sSlotMachine->state = SLOTTASK_MATCHED_POWER;
        if (sSlotMachine->reelTimeSpinsLeft && sSlotMachine->matches & MATCH_FLAG(MATCH_REPLAY))
        {
            CreateDigitalDisplayScene(DIG_DISPLAY_REEL_TIME);
            sSlotMachine->state = SLOTTASK_WAIT_RT_ANIM;
        }
    }
    return FALSE;
}

static bool8 SlotTask_MatchedPower(struct Task *task)
{
    if (!IsPikaPowerBoltAnimating())
    {
        sSlotMachine->state = SLOTTASK_RESET_BET_TILES;
        if (sSlotMachine->matches & MATCH_FLAG(MATCH_REPLAY))
        {
            sSlotMachine->state = SLOTTASK_START_SPIN;
            if (sSlotMachine->reelTimeSpinsLeft)
            {
                CreateDigitalDisplayScene(DIG_DISPLAY_REEL_TIME);
                sSlotMachine->state = SLOTTASK_WAIT_RT_ANIM;
            }
        }
    }
    return FALSE;
}

static bool8 SlotTask_WaitReelTimeAnim(struct Task *task)
{
    if (IsDigitalDisplayAnimFinished())
    {
        sSlotMachine->state = SLOTTASK_RESET_BET_TILES;
        if (sSlotMachine->matches & MATCH_FLAG(MATCH_REPLAY))
        {
            sSlotMachine->state = SLOTTASK_START_SPIN;
        }
    }
    return FALSE;
}

static bool8 SlotTask_ResetBetTiles(struct Task *task)
{
    DarkenBetTiles(0);
    DarkenBetTiles(1);
    DarkenBetTiles(2);
    sSlotMachine->state = SLOTTASK_READY_NEW_SPIN;
    return FALSE;
}

static bool8 SlotTask_NoMatches(struct Task *task)
{
    if (++task->data[1] > 64)
    {
        task->data[1] = 0;
        sSlotMachine->state = SLOTTASK_RESET_BET_TILES;
    }
    return FALSE;
}

static bool8 SlotTask_AskQuit(struct Task *task)
{
    SlotMachine_PrintMessage(gOtherText_QuitGamePrompt);
    DisplayYesNoMenu(21, 7, 1);
    DestroyOutlineCursorWindowSprite();
    sSlotMachine->state = SLOTTASK_HANDLE_QUIT_INPUT;
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
        sSlotMachine->state = SLOTTASK_END;
    }
    else if (input == 1 || input == -1)
    {
        Menu_EraseScreen();
        sSlotMachine->state = SLOTTASK_BET_INPUT;
    }
    return FALSE;
}

static bool8 SlotTask_PrintMsg_MaxCoins(struct Task *task)
{
    SlotMachine_PrintMessage(gOtherText_MaxCoins);
    sSlotMachine->state = SLOTTASK_WAIT_MSG_MAX_COINS;
    return FALSE;
}

static bool8 SlotTask_WaitMsg_MaxCoins(struct Task *task)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Menu_EraseScreen();
        sSlotMachine->state = SLOTTASK_BET_INPUT;
    }
    return FALSE;
}

static bool8 SlotTask_PrintMsg_NoMoreCoins(struct Task *task)
{
    SlotMachine_PrintMessage(gOtherText_OutOfCoins);
    sSlotMachine->state = SLOTTASK_WAIT_MSG_NO_MORE_COINS;
    return FALSE;
}

static bool8 SlotTask_WaitMsg_NoMoreCoins(struct Task *task)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Menu_EraseScreen();
        sSlotMachine->state = SLOTTASK_END;
    }
    return FALSE;
}

static bool8 SlotTask_EndGame(struct Task *task)
{
#if DEBUG
    if (sDebugSlotEnabled == 0)
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

static bool8 SlotTask_WaitDebugMenuClosed(struct Task *task)
{
    if (DebugSlot_IsMenuClosed() != 0)
        sSlotMachine->state = SLOTTASK_BET_INPUT;
    return FALSE;
}

#endif

static void DrawMachineBias(void)
{
    u8 whichBias;

    if (sSlotMachine->reelTimeSpinsLeft == 0)
    {
#if DEBUG
        if (sDebugSlotEnabled != 0 && sDebugSlotForceBiasPending != 0)
        {
            sSlotMachine->machineBias = sDebugSlotForcedBias;
            sDebugSlotForceBiasPending = 0;
            sDebugSlotForcedBias = 0;
            if (sSlotMachine->machineBias & BIAS_STRAIGHT_7)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsStraight7, 1);
            if (sSlotMachine->machineBias & BIAS_MIXED_7)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsMixed7, 1);
            if (sSlotMachine->machineBias & BIAS_REELTIME)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsReelTime, 1);
            if (sSlotMachine->machineBias & BIAS_POWER)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsPower, 1);
            if (sSlotMachine->machineBias & BIAS_AZURILL)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsAzurill, 1);
            if (sSlotMachine->machineBias & BIAS_LOTAD)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsLotad, 1);
            if (sSlotMachine->machineBias & BIAS_REPLAY)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsReplay, 1);
            if (sSlotMachine->machineBias & BIAS_CHERRY)
                DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsCherry, 1);
            return;
        }
#endif
        if (!(sSlotMachine->machineBias & BIAS_7))
        {
            if (ShouldTrySpecialBias())
            {
                whichBias = TrySelectBias_Special();
                if (whichBias != NUM_SPECIAL_BIASES)
                {
                    sSlotMachine->machineBias |= sBiasesSpecial[whichBias];
#if DEBUG
                    if (sSlotMachine->machineBias & BIAS_STRAIGHT_7)
                        DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsStraight7, 1);
                    if (sSlotMachine->machineBias & BIAS_MIXED_7)
                        DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsMixed7, 1);
                    if (sSlotMachine->machineBias & BIAS_REELTIME)
                        DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsReelTime, 1);
#endif
                    if (whichBias != 1)
                    {
                        return;
                    }
                }
            }
            whichBias = TrySelectBias_Regular();
            if (whichBias != NUM_REGULAR_BIASES)
            {
                sSlotMachine->machineBias |= sBiasesRegular[whichBias];
#if DEBUG
                if (sSlotMachine->machineBias & BIAS_POWER)
                    DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsPower, 1);
                if (sSlotMachine->machineBias & BIAS_AZURILL)
                    DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsAzurill, 1);
                if (sSlotMachine->machineBias & BIAS_LOTAD)
                    DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsLotad, 1);
                if (sSlotMachine->machineBias & BIAS_REPLAY)
                    DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsReplay, 1);
                if (sSlotMachine->machineBias & BIAS_CHERRY)
                    DebugSlot_AddCounterClamped(&sSlotMachine->debugBiasDrawsCherry, 1);
#endif
            }
        }
    }
}

static void ResetBiasFailure(void)
{
    sSlotMachine->didNotFailBias = 0;
    if (sSlotMachine->machineBias)
        sSlotMachine->didNotFailBias = 1;
}

static u8 GetBiasSymbol(u8 machineBias)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        if (machineBias & 1)
            return sBiasSymbols[i];
        machineBias >>= 1;
    }
    return 0;
}

static bool8 ShouldTrySpecialBias(void)
{
    u8 rval = Random();
    if (sSpecialDrawOdds[sSlotMachine->machineId][sSlotMachine->bet - 1] > rval)
        return TRUE;
    return FALSE;
}

static const u8 sBiasProbabilities_Special[][6];

static u8 TrySelectBias_Special(void)
{
    s16 whichBias;

    for (whichBias = 0; whichBias < NUM_SPECIAL_BIASES; whichBias++)
    {
        s16 rval = Random() & 0xff;
        s16 value = sBiasProbabilities_Special[whichBias][sSlotMachine->machineId];
        if (value > rval)
            break;
    }
    return whichBias;
}

static const u8 sBiasProbabilities_Regular[][6];

static u8 TrySelectBias_Regular(void)
{
    s16 whichBias;

    for (whichBias = 0; whichBias < NUM_REGULAR_BIASES; whichBias++)
    {
        s16 rval = Random() & 0xff;
        s16 value = sBiasProbabilities_Regular[whichBias][sSlotMachine->machineId];
        if (whichBias == 0 && sSlotMachine->luckyGame == 1)
        {
            value += 10;
            if (value > 0x100)
                value = 0x100;
        }
        else if (whichBias == 4 && sSlotMachine->luckyGame == 1)
        {
            value -= 10;
            if (value < 0)
                value = 0;
        }
        if (value > rval)
            break;
    }
    return whichBias;
}

static const u8 sReelTimeProbabilities_NormalGame[][17];
static const u8 sReelTimeProbabilities_LuckyGame[][17];

static u8 GetReelTimeSpinProbability(u8 spins)
{
    if (sSlotMachine->luckyGame == 0)
        return sReelTimeProbabilities_NormalGame[spins][sSlotMachine->pikaPowerBolts];
    else
        return sReelTimeProbabilities_LuckyGame[spins][sSlotMachine->pikaPowerBolts];
}

static void GetReelTimeDraw(void)
{
    u8 rval;
    s16 spins;

    sSlotMachine->reelTimeDraw = 0;
    rval = Random();
    if (rval < GetReelTimeSpinProbability(0))
        return;
    for (spins = 5; spins > 0; spins--)
    {
        rval = Random();
        if (rval < GetReelTimeSpinProbability(spins))
            break;
    }
    sSlotMachine->reelTimeDraw = spins;
}

static const u16 sReelTimeExplodeProbability[];

static bool8 ShouldReelTimeMachineExplode(u16 check)
{
    u16 rval = Random() & 0xff;
    if (rval < sReelTimeExplodeProbability[check])
        return TRUE;
    else
        return FALSE;
}

static const u16 sReelTimeSpeed_Probabilities[][2];
static const u16 sQuarterSpeed_ProbabilityBoost[];

static u16 ReelTimeSpeed(void)
{
    u8 i = 0;
    u8 rval;
    u8 value;
    if (sSlotMachine->netCoinLoss >= 300)
        i = 4;
    else if (sSlotMachine->netCoinLoss >= 250)
        i = 3;
    else if (sSlotMachine->netCoinLoss >= 200)
        i = 2;
    else if (sSlotMachine->netCoinLoss >= 150)
        i = 1;
    rval = Random() % 100;
    value = sReelTimeSpeed_Probabilities[i][0];
    if (rval < value)
        return REEL_HALF_SPEED;
    rval = Random() % 100;
    value = sReelTimeSpeed_Probabilities[i][1] + sQuarterSpeed_ProbabilityBoost[sSlotMachine->reelTimeSpinsUsed];
    if (rval < value)
        return REEL_QUARTER_SPEED;
    return REEL_NORMAL_SPEED;
}

static void CheckMatch(void)
{
    sSlotMachine->matches = 0;
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
    u8 sym1, sym2, sym3, match;

    sym1 = GetSymbolAtRest(LEFT_REEL, 2);
    sym2 = GetSymbolAtRest(MIDDLE_REEL, 2);
    sym3 = GetSymbolAtRest(RIGHT_REEL, 2);
    match = GetMatchFromSymbols(sym1, sym2, sym3);
    if (match != MATCH_NONE)
    {
        sSlotMachine->payout += sSlotPayouts[match];
        sSlotMachine->matches |= sSlotMatchFlags[match];
        FlashMatchLine(MATCH_MIDDLE_ROW);
    }
}

static void CheckMatch_TopAndBottom(void)
{
    u8 sym1, sym2, sym3, match;

    sym1 = GetSymbolAtRest(LEFT_REEL, 1);
    sym2 = GetSymbolAtRest(MIDDLE_REEL, 1);
    sym3 = GetSymbolAtRest(RIGHT_REEL, 1);
    match = GetMatchFromSymbols(sym1, sym2, sym3);
    if (match != MATCH_NONE)
    {
        if (match == MATCH_CHERRY)
            match = MATCH_TOPBOT_CHERRY;
        sSlotMachine->payout += sSlotPayouts[match];
        sSlotMachine->matches |= sSlotMatchFlags[match];
        FlashMatchLine(MATCH_TOP_ROW);
    }
    sym1 = GetSymbolAtRest(LEFT_REEL, 3);
    sym2 = GetSymbolAtRest(MIDDLE_REEL, 3);
    sym3 = GetSymbolAtRest(RIGHT_REEL, 3);
    match = GetMatchFromSymbols(sym1, sym2, sym3);
    if (match != MATCH_NONE)
    {
        if (match == MATCH_CHERRY)
            match = MATCH_TOPBOT_CHERRY;
        sSlotMachine->payout += sSlotPayouts[match];
        sSlotMachine->matches |= sSlotMatchFlags[match];
        FlashMatchLine(MATCH_BOTTOM_ROW);
    }
}

static void CheckMatch_Diagonals(void)
{
    u8 sym1, sym2, sym3, match;

    sym1 = GetSymbolAtRest(LEFT_REEL, 1);
    sym2 = GetSymbolAtRest(MIDDLE_REEL, 2);
    sym3 = GetSymbolAtRest(RIGHT_REEL, 3);
    match = GetMatchFromSymbols(sym1, sym2, sym3);
    if (match != MATCH_NONE)
    {
        if (match != MATCH_CHERRY)
        {
            sSlotMachine->payout += sSlotPayouts[match];
            sSlotMachine->matches |= sSlotMatchFlags[match];
        }
        FlashMatchLine(MATCH_NWSE_DIAG);
    }
    sym1 = GetSymbolAtRest(LEFT_REEL, 3);
    sym2 = GetSymbolAtRest(MIDDLE_REEL, 2);
    sym3 = GetSymbolAtRest(RIGHT_REEL, 1);
    match = GetMatchFromSymbols(sym1, sym2, sym3);
    if (match != MATCH_NONE)
    {
        if (match != MATCH_CHERRY)
        {
            sSlotMachine->payout += sSlotPayouts[match];
            sSlotMachine->matches |= sSlotMatchFlags[match];
        }
        FlashMatchLine(MATCH_NESW_DIAG);
    }
}

static const u8 sSymbolToMatch[];

static u8 GetMatchFromSymbols(u8 sym1, u8 sym2, u8 sym3)
{
    if (sym1 == sym2 && sym1 == sym3)
        return sSymbolToMatch[sym1];
    if (sym1 == SYMBOL_7_RED && sym2 == SYMBOL_7_RED && sym3 == SYMBOL_7_BLUE)
        return MATCH_MIXED_7;
    if (sym1 == SYMBOL_7_BLUE && sym2 == SYMBOL_7_BLUE && sym3 == SYMBOL_7_RED)
        return MATCH_MIXED_7;
    if (sym1 == SYMBOL_CHERRY)
        return MATCH_CHERRY;
    return MATCH_NONE;
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
    [PAYOUT_TASK_INIT] = PayoutTask_Init,
    [PAYOUT_TASK_GIVE_PAYOUT] = PayoutTask_GivePayout,
    [PAYOUT_TASK_FREE] = PayoutTask_Free,
};

#define tPayoutState data[0]
#define tPayoutDelay data[1]

static void Task_Payout(u8 taskId)
{
    while (sPayoutTasks[gTasks[taskId].tPayoutState](gTasks + taskId))
        ;
}

static bool8 PayoutTask_Init(struct Task *task)
{
    if (IsMatchLineDoneFlashingBeforePayout())
    {
        task->tPayoutState++;
        if (sSlotMachine->payout == 0)
        {
            task->tPayoutState = PAYOUT_TASK_FREE;
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 PayoutTask_GivePayout(struct Task *task)
{
    if (!task->tPayoutDelay--)
    {
        if (IsFanfareTaskInactive())
            PlaySE(SE_PIN);
        sSlotMachine->payout--;
        if (sSlotMachine->coins < 9999)
            sSlotMachine->coins++;
        task->tPayoutDelay = 8;
        if (JOY_HELD(A_BUTTON))
            task->tPayoutDelay = 4;
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
        task->tPayoutState++;
    return FALSE;
}

static bool8 PayoutTask_Free(struct Task *task)
{
    if (TryStopMatchLinesFlashing())
        DestroyTask(FindTaskIdByFunc(Task_Payout));
    return FALSE;
}

#undef tPayoutState
#undef tPayoutDelay

static const u8 sReelSymbols[NUM_REELS][SYMBOLS_PER_REEL];

static u8 GetSymbolAtRest(u8 reelIndex, s16 row)
{
    s16 offset = (sSlotMachine->reelPositions[reelIndex] + row) % SYMBOLS_PER_REEL;
    if (offset < 0)
        offset += SYMBOLS_PER_REEL;
    return sReelSymbols[reelIndex][offset];
}

static u8 GetSymbol(u8 reelIndex, s16 row)
{
    s16 symbolOffset = 0;
    if ((sSlotMachine->reelPixelOffsets[reelIndex]) % REEL_SYMBOL_HEIGHT)
        symbolOffset = -1;
    return GetSymbolAtRest(reelIndex, row + symbolOffset);
}

static const u8 sReelTimeSymbols[];

static u8 GetReelTimeSymbol(s16 offset)
{
    s16 symbolIndex = (sSlotMachine->reeltimePosition + offset) % REELTIME_SYMBOLS;
    if (symbolIndex < 0)
        symbolIndex += REELTIME_SYMBOLS;
    return sReelTimeSymbols[symbolIndex];
}

static void AdvanceSlotReel(u8 reelIndex, s16 value)
{
    sSlotMachine->reelPixelOffsets[reelIndex] += value;
    sSlotMachine->reelPixelOffsets[reelIndex] %= REEL_HEIGHT;
    sSlotMachine->reelPositions[reelIndex] = SYMBOLS_PER_REEL - sSlotMachine->reelPixelOffsets[reelIndex] / REEL_SYMBOL_HEIGHT;
}

static s16 AdvanceSlotReelToNextSymbol(u8 reelIndex, s16 value)
{
    s16 offset = sSlotMachine->reelPixelOffsets[reelIndex] % REEL_SYMBOL_HEIGHT;
    if (offset != 0)
    {
        if (offset < value)
            value = offset;
        AdvanceSlotReel(reelIndex, value);
        offset = sSlotMachine->reelPixelOffsets[reelIndex] % REEL_SYMBOL_HEIGHT;
    }
    return offset;
}

static void AdvanceReeltimeReel(s16 value)
{
    sSlotMachine->reeltimePixelOffset += value;
    sSlotMachine->reeltimePixelOffset %= REELTIME_REEL_HEIGHT;
    sSlotMachine->reeltimePosition = REELTIME_SYMBOLS - sSlotMachine->reeltimePixelOffset / REELTIME_SYMBOL_HEIGHT;
}

static s16 AdvanceReeltimeReelToNextSymbol(s16 value)
{
    s16 offset = sSlotMachine->reeltimePixelOffset % REELTIME_SYMBOL_HEIGHT;
    if (offset != 0)
    {
        if (offset < value)
            value = offset;
        AdvanceReeltimeReel(value);
        offset = sSlotMachine->reeltimePixelOffset % REELTIME_SYMBOL_HEIGHT;
    }
    return offset;
}

#define tState          data[0]
#define tExtraTurns     data[1]
#define tShockMagnitude data[1]
#define tTimer          data[2]
#define tMoving         data[14]
#define tReelId         data[15]

static void CreateReelTasks(void)
{
    u8 i;
    for (i = 0; i < NUM_REELS; i++)
    {
        u8 taskId = CreateTask(Task_Reel, 2);
        gTasks[taskId].tReelId = i;
        sSlotMachine->slotReelTasks[i] = taskId;
        Task_Reel(taskId);
    }
}

static void SpinSlotReel(u8 reelIndex)
{
    gTasks[sSlotMachine->slotReelTasks[reelIndex]].tState = REEL_TASK_SPIN;
    gTasks[sSlotMachine->slotReelTasks[reelIndex]].tMoving = TRUE;
}

static void StopSlotReel(u8 reelIndex)
{
    gTasks[sSlotMachine->slotReelTasks[reelIndex]].tState = REEL_TASK_DECIDE_STOP;
}

static bool8 IsSlotReelMoving(u8 reelIndex)
{
    return gTasks[sSlotMachine->slotReelTasks[reelIndex]].tMoving;
}

static bool8 (*const sReelTasks[])(struct Task *task) =
{
    [REEL_TASK_STILL] = ReelTask_StayStill,
    [REEL_TASK_SPIN] = ReelTask_Spin,
    [REEL_TASK_DECIDE_STOP] = ReelTask_DecideStop,
    [REEL_TASK_STOP_MOVE] = ReelTask_MoveToStop,
    [REEL_TASK_STOP_SHAKE] = ReelTask_ShakingStop,
};

static void Task_Reel(u8 taskId)
{
    while (sReelTasks[gTasks[taskId].tState](gTasks + taskId))
        ;
}

static bool8 ReelTask_StayStill(struct Task *task)
{
    return FALSE;
}

static bool8 ReelTask_Spin(struct Task *task)
{
    AdvanceSlotReel(task->tReelId, sSlotMachine->reelSpeed);
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
    task->tState++;
    sSlotMachine->winnerRows[task->tReelId] = 0;
    sSlotMachine->reelExtraTurns[task->tReelId] = 0;
    if (sSlotMachine->reelTimeSpinsLeft == 0 && (sSlotMachine->machineBias == 0 || sSlotMachine->didNotFailBias == 0 || !sDecideStop_Bias[task->tReelId]()))
    {
        sSlotMachine->didNotFailBias = 0;
        sDecideStop_NoBias[task->tReelId]();
    }
    task->tExtraTurns = sSlotMachine->reelExtraTurns[task->tReelId];
    return TRUE;
}

static bool8 ReelTask_MoveToStop(struct Task *task)
{
    u16 reelStopShocks[] = {2, 4, 4, 4, 8};
    s16 reelPixelPos = sSlotMachine->reelPixelOffsets[task->tReelId] % REEL_SYMBOL_HEIGHT;
    if (reelPixelPos != 0)
        reelPixelPos = AdvanceSlotReelToNextSymbol(task->tReelId, sSlotMachine->reelSpeed);
    else if (sSlotMachine->reelExtraTurns[task->tReelId])
    {
        sSlotMachine->reelExtraTurns[task->tReelId]--;
        AdvanceSlotReel(task->tReelId, sSlotMachine->reelSpeed);
        reelPixelPos = sSlotMachine->reelPixelOffsets[task->tReelId] % REEL_SYMBOL_HEIGHT;
    }
    if (reelPixelPos == 0 && sSlotMachine->reelExtraTurns[task->tReelId] == 0)
    {
        task->tState++;
        task->tShockMagnitude = reelStopShocks[task->tExtraTurns];
        task->tTimer = 0;
    }
    return FALSE;
}

static bool8 ReelTask_ShakingStop(struct Task *task)
{
    sSlotMachine->reelShockOffsets[task->tReelId] = task->tShockMagnitude;
    task->tShockMagnitude = -task->tShockMagnitude;
    task->tTimer++;
    if ((task->tTimer & 0x3) == 0)
        task->tShockMagnitude >>= 1;
    if (task->tShockMagnitude == 0)
    {
        task->tState = REEL_TASK_STILL;
        task->tMoving = FALSE;
        sSlotMachine->reelShockOffsets[task->tReelId] = 0;
    }
    return FALSE;
}

#undef tState
#undef tExtraTurns
#undef tShockMagnitude
#undef tTimer
#undef tMoving
#undef tReelId

static bool8 (*const sDecideStop_Bias_Reel1_Bets[])(u8 symbol1, u8 symbol2) =
{
    DecideStop_Bias_Reel1_Bet1,
    DecideStop_Bias_Reel1_Bet2or3,
    DecideStop_Bias_Reel1_Bet2or3
};

static bool8 DecideStop_Bias_Reel1(void)
{
    u8 symbol2 = GetBiasSymbol(sSlotMachine->machineBias);
    u8 symbol1 = symbol2;
    if (sSlotMachine->machineBias & BIAS_7)
    {
        symbol1 = SYMBOL_7_RED;
        symbol2 = SYMBOL_7_BLUE;
    }
    return sDecideStop_Bias_Reel1_Bets[sSlotMachine->bet - 1](symbol1, symbol2);
}

static bool8 EitherSymbolAtPos_Reel1(s16 pos, u8 symbol1, u8 symbol2)
{
    u8 symbol = GetSymbol(LEFT_REEL, pos);
    if (symbol == symbol1 || symbol == symbol2)
    {
        sSlotMachine->biasSymbol = symbol;
        return TRUE;
    }
    return FALSE;
}

static bool8 AreCherriesOnScreen_Reel1(s16 turns)
{
    if (GetSymbol(LEFT_REEL, 1 - turns) == SYMBOL_CHERRY
        || GetSymbol(LEFT_REEL, 2 - turns) == SYMBOL_CHERRY
        || GetSymbol(LEFT_REEL, 3 - turns) == SYMBOL_CHERRY)
        return TRUE;
    else
        return FALSE;
}

static bool8 BiasedTowardCherryOr7s(void)
{
    if (sSlotMachine->machineBias & (BIAS_STRAIGHT_7 | BIAS_MIXED_7 | BIAS_CHERRY))
        return TRUE;
    else
        return FALSE;
}

static bool8 DecideStop_Bias_Reel1_Bet1(u8 symbol1, u8 symbol2)
{
    s16 i;

    for (i = 0; i < 5; i++)
    {
        if (EitherSymbolAtPos_Reel1(2 - i, symbol1, symbol2))
        {
            sSlotMachine->winnerRows[LEFT_REEL] = 2;
            sSlotMachine->reelExtraTurns[LEFT_REEL] = i;
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 DecideStop_Bias_Reel1_Bet2or3(u8 symbol1, u8 symbol2)
{
    s16 i;
    bool8 cherry7Bias = BiasedTowardCherryOr7s();
    if (cherry7Bias || !AreCherriesOnScreen_Reel1(0))
    {
        for (i = 1; i < 4; i++)
        {
            if (EitherSymbolAtPos_Reel1(i, symbol1, symbol2))
            {
                sSlotMachine->winnerRows[LEFT_REEL] = i;
                sSlotMachine->reelExtraTurns[LEFT_REEL] = 0;
                return TRUE;
            }
        }
    }
    for (i = 1; i < 5; i++)
    {
        bool8 cherry7BiasCopy = cherry7Bias;
        if (cherry7BiasCopy || !AreCherriesOnScreen_Reel1(i))
        {
            if (EitherSymbolAtPos_Reel1(1 - i, symbol1, symbol2))
            {
                if (i == 1 && (cherry7BiasCopy || !AreCherriesOnScreen_Reel1(3)))
                {
                    sSlotMachine->winnerRows[LEFT_REEL] = 3;
                    sSlotMachine->reelExtraTurns[LEFT_REEL] = 3;
                    return TRUE;
                }
                if (i < 4 && (cherry7BiasCopy || !AreCherriesOnScreen_Reel1(i + 1)))
                {
                    sSlotMachine->winnerRows[LEFT_REEL] = 2;
                    sSlotMachine->reelExtraTurns[LEFT_REEL] = i + 1;
                    return TRUE;
                }
                sSlotMachine->winnerRows[LEFT_REEL] = 1;
                sSlotMachine->reelExtraTurns[LEFT_REEL] = i;
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
    s16 reel1BiasRow = sSlotMachine->winnerRows[LEFT_REEL];

    for (i = 0; i < 5; i++)
    {
        if (GetSymbol(MIDDLE_REEL, reel1BiasRow - i) == sSlotMachine->biasSymbol)
        {
            sSlotMachine->winnerRows[MIDDLE_REEL] = reel1BiasRow;
            sSlotMachine->reelExtraTurns[MIDDLE_REEL] = i;
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
        if (sSlotMachine->winnerRows[LEFT_REEL] != 2 && sSlotMachine->reelExtraTurns[MIDDLE_REEL] > 1 && sSlotMachine->reelExtraTurns[MIDDLE_REEL] != 4)
        {
            for (i = 0; i < 5; i++)
            {
                if (GetSymbol(MIDDLE_REEL, 2 - i) == sSlotMachine->biasSymbol)
                {
                    sSlotMachine->winnerRows[MIDDLE_REEL] = 2;
                    sSlotMachine->reelExtraTurns[MIDDLE_REEL] = i;
                    break;
                }
            }
        }
        return TRUE;
    }
    if (sSlotMachine->winnerRows[LEFT_REEL] != 2)
    {
        for (i = 0; i < 5; i++)
        {
            if (GetSymbol(MIDDLE_REEL, 2 - i) == sSlotMachine->biasSymbol)
            {
                sSlotMachine->winnerRows[MIDDLE_REEL] = 2;
                sSlotMachine->reelExtraTurns[MIDDLE_REEL] = i;
                return TRUE;
            }
        }
    }
    return FALSE;
}

static bool8 (*const sDecideStop_Bias_Reel3_Bets[])(u8 biasSymbol) =
{
    DecideStop_Bias_Reel3_Bet1or2,
    DecideStop_Bias_Reel3_Bet1or2,
    DecideStop_Bias_Reel3_Bet3
};

static bool8 DecideStop_Bias_Reel3(void)
{
    u8 biasSymbol = sSlotMachine->biasSymbol;
    if (sSlotMachine->machineBias & BIAS_MIXED_7)
    {
        biasSymbol = SYMBOL_7_RED;
        if (sSlotMachine->biasSymbol == SYMBOL_7_RED)
        {
            biasSymbol = SYMBOL_7_BLUE;
        }
    }
    return sDecideStop_Bias_Reel3_Bets[sSlotMachine->bet - 1](biasSymbol);
}

static bool8 DecideStop_Bias_Reel3_Bet1or2(u8 biasSymbol)
{
    s16 i;
    s16 reel2BiasRow = sSlotMachine->winnerRows[MIDDLE_REEL];

    for (i = 0; i < 5; i++)
    {
        if (GetSymbol(RIGHT_REEL, reel2BiasRow - i) == biasSymbol)
        {
            sSlotMachine->winnerRows[RIGHT_REEL] = reel2BiasRow;
            sSlotMachine->reelExtraTurns[RIGHT_REEL] = i;
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 DecideStop_Bias_Reel3_Bet3(u8 biasSymbol)
{
    s16 i;
    s16 biasRow;
    if (sSlotMachine->winnerRows[LEFT_REEL] == sSlotMachine->winnerRows[MIDDLE_REEL])
        return DecideStop_Bias_Reel3_Bet1or2(biasSymbol);
    if (sSlotMachine->winnerRows[LEFT_REEL] == 1)
        biasRow = 3;
    else
        biasRow = 1;
    for (i = 0; i < 5; i++)
    {
        if (GetSymbol(RIGHT_REEL, biasRow - i) == biasSymbol)
        {
            sSlotMachine->reelExtraTurns[RIGHT_REEL] = i;
            sSlotMachine->winnerRows[RIGHT_REEL] = biasRow;
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
    sSlotMachine->reelExtraTurns[LEFT_REEL] = i;
}

static bool8 IfSymbol7_SwitchColor(u8 *symbol)
{
    if (*symbol == SYMBOL_7_RED)
    {
        *symbol = SYMBOL_7_BLUE;
        return TRUE;
    }
    if (*symbol == SYMBOL_7_BLUE)
    {
        *symbol = SYMBOL_7_RED;
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
    if (sSlotMachine->winnerRows[LEFT_REEL] != 0 && sSlotMachine->machineBias & BIAS_STRAIGHT_7)
    {
        u8 reel1MiddleSym = GetSymbol(LEFT_REEL, 2 - sSlotMachine->reelExtraTurns[LEFT_REEL]);
        if (IfSymbol7_SwitchColor(&reel1MiddleSym))
        {
            s16 i;
            for (i = 0; i < 5; i++)
            {
                if (reel1MiddleSym == GetSymbol(MIDDLE_REEL, 2 - i))
                {
                    sSlotMachine->winnerRows[MIDDLE_REEL] = 2;
                    sSlotMachine->reelExtraTurns[MIDDLE_REEL] = i;
                    break;
                }
            }
        }
    }
}

static void DecideStop_NoBias_Reel2_Bet2(void)
{
    if (sSlotMachine->winnerRows[LEFT_REEL] != 0 && sSlotMachine->machineBias & BIAS_STRAIGHT_7)
    {
        u8 reel1BiasSym = GetSymbol(LEFT_REEL, sSlotMachine->winnerRows[LEFT_REEL] - sSlotMachine->reelExtraTurns[LEFT_REEL]);
        if (IfSymbol7_SwitchColor(&reel1BiasSym))
        {
            s16 i;
            for (i = 0; i < 5; i++)
            {
                if (reel1BiasSym == GetSymbol(MIDDLE_REEL, sSlotMachine->winnerRows[LEFT_REEL] - i))
                {
                    sSlotMachine->winnerRows[MIDDLE_REEL] = sSlotMachine->winnerRows[LEFT_REEL];
                    sSlotMachine->reelExtraTurns[MIDDLE_REEL] = i;
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
    u8 reel1BiasSym;

    if (sSlotMachine->winnerRows[LEFT_REEL] != 0 && sSlotMachine->machineBias & BIAS_STRAIGHT_7)
    {
        if (sSlotMachine->winnerRows[LEFT_REEL] == 2)
        {
            DecideStop_NoBias_Reel2_Bet2();
        }
        else
        {
            reel1BiasSym = GetSymbol(LEFT_REEL, sSlotMachine->winnerRows[LEFT_REEL] - sSlotMachine->reelExtraTurns[LEFT_REEL]);
            if (IfSymbol7_SwitchColor(&reel1BiasSym))
            {
                j = 2;
                if (sSlotMachine->winnerRows[LEFT_REEL] == 3)
                    j = 3;
                for (i = 0; i < 2; i++, j--)
                {
                    if (reel1BiasSym == GetSymbol(MIDDLE_REEL, j))
                    {
                        sSlotMachine->winnerRows[MIDDLE_REEL] = j;
                        sSlotMachine->reelExtraTurns[MIDDLE_REEL] = 0;
                        return;
                    }
                }
                for (j = 1; j < 5; j++)
                {
                    if (reel1BiasSym == GetSymbol(MIDDLE_REEL, sSlotMachine->winnerRows[LEFT_REEL] - j))
                    {
                        if (sSlotMachine->winnerRows[LEFT_REEL] == 1)
                        {
                            if (j < 3)
                            {
                                sSlotMachine->winnerRows[MIDDLE_REEL] = 2;
                                sSlotMachine->reelExtraTurns[MIDDLE_REEL] = j + 1;
                            }
                            else
                            {
                                sSlotMachine->winnerRows[MIDDLE_REEL] = 1;
                                sSlotMachine->reelExtraTurns[MIDDLE_REEL] = j;
                            }
                        }
                        else if (j < 3)
                        {
                            sSlotMachine->winnerRows[MIDDLE_REEL] = 3;
                            sSlotMachine->reelExtraTurns[MIDDLE_REEL] = j;
                        }
                        else
                        {
                            sSlotMachine->winnerRows[MIDDLE_REEL] = 2;
                            sSlotMachine->reelExtraTurns[MIDDLE_REEL] = j - 1;
                        }
                        return;
                    }
                }
            }
        }
    }
}

static bool8 MismatchedSyms_77(u8 symbol1, u8 symbol2)
{
    if ((symbol1 == SYMBOL_7_RED && symbol2 == SYMBOL_7_BLUE) || (symbol1 == SYMBOL_7_BLUE && symbol2 == SYMBOL_7_RED))
        return TRUE;
    else
        return FALSE;
}

static bool8 MismatchedSyms_777(u8 symbol1, u8 symbol2, u8 symbol3)
{
    if ((symbol1 == SYMBOL_7_RED && symbol2 == SYMBOL_7_BLUE && symbol3 == SYMBOL_7_RED) || (symbol1 == SYMBOL_7_BLUE && symbol2 == SYMBOL_7_RED && symbol3 == SYMBOL_7_BLUE))
        return TRUE;
    else
        return FALSE;
}

static bool8 NeitherMatchNor7Mismatch(u8 symbol1, u8 symbol2, u8 symbol3)
{
    if ((symbol1 == SYMBOL_7_RED && symbol2 == SYMBOL_7_BLUE && symbol3 == SYMBOL_7_RED) ||
        (symbol1 == SYMBOL_7_BLUE && symbol2 == SYMBOL_7_RED && symbol3 == SYMBOL_7_BLUE) ||
        (symbol1 == SYMBOL_7_RED && symbol2 == SYMBOL_7_RED && symbol3 == SYMBOL_7_BLUE) ||
        (symbol1 == SYMBOL_7_BLUE && symbol2 == SYMBOL_7_BLUE && symbol3 == SYMBOL_7_RED) ||
        (symbol1 == symbol2 && symbol1 == symbol3))
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
    u8 symbol1 = GetSymbol(LEFT_REEL, 2 - sSlotMachine->reelExtraTurns[LEFT_REEL]);
    u8 symbol2 = GetSymbol(MIDDLE_REEL, 2 - sSlotMachine->reelExtraTurns[MIDDLE_REEL]);
    if (symbol1 == symbol2)
    {
        while (1)
        {
            u8 symbol3;
            if (!(symbol1 == (symbol3 = GetSymbol(RIGHT_REEL, 2 - i)) || (symbol1 == SYMBOL_7_RED && symbol3 == SYMBOL_7_BLUE) || (symbol1 == SYMBOL_7_BLUE && symbol3 == SYMBOL_7_RED)))
                break;
            i++;
        }
    }
    else if (MismatchedSyms_77(symbol1, symbol2))
    {
        if (sSlotMachine->machineBias & BIAS_STRAIGHT_7)
        {
            for (i = 0; i < 5; i++)
            {
                if (symbol1 == GetSymbol(RIGHT_REEL, 2 - i))
                {
                    sSlotMachine->reelExtraTurns[RIGHT_REEL] = i;
                    return;
                }
            }
        }
        i = 0;
        while (1)
        {
            if (symbol1 != GetSymbol(RIGHT_REEL, 2 - i))
                break;
            i++;
        }
    }
    sSlotMachine->reelExtraTurns[RIGHT_REEL] = i;
}

static void DecideStop_NoBias_Reel3_Bet2(void)
{
    s16 extraTurns = 0;
    s16 i;
    u8 symbol1;
    u8 symbol2;
    u8 symbol3;

    if (sSlotMachine->winnerRows[MIDDLE_REEL] != 0 && sSlotMachine->winnerRows[LEFT_REEL] == sSlotMachine->winnerRows[MIDDLE_REEL] && sSlotMachine->machineBias & BIAS_STRAIGHT_7)
    {
        symbol1 = GetSymbol(LEFT_REEL, sSlotMachine->winnerRows[LEFT_REEL] - sSlotMachine->reelExtraTurns[LEFT_REEL]);
        symbol2 = GetSymbol(MIDDLE_REEL, sSlotMachine->winnerRows[MIDDLE_REEL] - sSlotMachine->reelExtraTurns[MIDDLE_REEL]);
        if (MismatchedSyms_77(symbol1, symbol2))
        {
            for (i = 0; i < 5; i++)
            {
                symbol3 = GetSymbol(RIGHT_REEL, sSlotMachine->winnerRows[MIDDLE_REEL] - i);
                if (symbol1 == symbol3)
                {
                    extraTurns = i;
                    break;
                }
            }
        }
    }
    while (1)
    {
        s16 numMatches;
        for (i = 1, numMatches = 0; i < 4; i++)
        {
            symbol1 = GetSymbol(LEFT_REEL, i - sSlotMachine->reelExtraTurns[LEFT_REEL]);
            symbol2 = GetSymbol(MIDDLE_REEL, i - sSlotMachine->reelExtraTurns[MIDDLE_REEL]);
            symbol3 = GetSymbol(RIGHT_REEL, i - extraTurns);
            if (!NeitherMatchNor7Mismatch(symbol1, symbol2, symbol3) && (!MismatchedSyms_777(symbol1, symbol2, symbol3) || !(sSlotMachine->machineBias & BIAS_STRAIGHT_7)))
            {
                numMatches++;
                break;
            }
        }
        if (numMatches == 0)
            break;
        extraTurns++;
    }
    sSlotMachine->reelExtraTurns[RIGHT_REEL] = extraTurns;
}

static void DecideStop_NoBias_Reel3_Bet3(void)
{
    u8 symbol1;
    u8 symbol2;
    u8 symbol3;
    s16 row;
    s16 i;

    DecideStop_NoBias_Reel3_Bet2();
    if (sSlotMachine->winnerRows[MIDDLE_REEL] != 0 && sSlotMachine->winnerRows[LEFT_REEL] != sSlotMachine->winnerRows[MIDDLE_REEL] && sSlotMachine->machineBias & BIAS_STRAIGHT_7)
    {
        symbol1 = GetSymbol(LEFT_REEL, sSlotMachine->winnerRows[LEFT_REEL] - sSlotMachine->reelExtraTurns[LEFT_REEL]);
        symbol2 = GetSymbol(MIDDLE_REEL, sSlotMachine->winnerRows[MIDDLE_REEL] - sSlotMachine->reelExtraTurns[MIDDLE_REEL]);
        if (MismatchedSyms_77(symbol1, symbol2))
        {
            row = 1;
            if (sSlotMachine->winnerRows[LEFT_REEL] == 1)
                row = 3;
            for (i = 0; i < 5; i++)
            {
                symbol3 = GetSymbol(RIGHT_REEL, row - (sSlotMachine->reelExtraTurns[RIGHT_REEL] + i));
                if (symbol1 == symbol3)
                {
                    sSlotMachine->reelExtraTurns[RIGHT_REEL] += i;
                    break;
                }
            }
        }
    }
    while (1)
    {
        symbol1 = GetSymbol(LEFT_REEL, 1 - sSlotMachine->reelExtraTurns[LEFT_REEL]);
        symbol2 = GetSymbol(MIDDLE_REEL, 2 - sSlotMachine->reelExtraTurns[MIDDLE_REEL]);
        symbol3 = GetSymbol(RIGHT_REEL, 3 - sSlotMachine->reelExtraTurns[RIGHT_REEL]);
        if (NeitherMatchNor7Mismatch(symbol1, symbol2, symbol3) || (MismatchedSyms_777(symbol1, symbol2, symbol3) && sSlotMachine->machineBias & BIAS_STRAIGHT_7))
            break;
        sSlotMachine->reelExtraTurns[RIGHT_REEL]++;
    }
    while (1)
    {
        symbol1 = GetSymbol(LEFT_REEL, 3 - sSlotMachine->reelExtraTurns[LEFT_REEL]);
        symbol2 = GetSymbol(MIDDLE_REEL, 2 - sSlotMachine->reelExtraTurns[MIDDLE_REEL]);
        symbol3 = GetSymbol(RIGHT_REEL, 1 - sSlotMachine->reelExtraTurns[RIGHT_REEL]);
        if (NeitherMatchNor7Mismatch(symbol1, symbol2, symbol3) || (MismatchedSyms_777(symbol1, symbol2, symbol3) && sSlotMachine->machineBias & BIAS_STRAIGHT_7))
            break;
        sSlotMachine->reelExtraTurns[RIGHT_REEL]++;
    }
}

#define tStopButtonState     data[0]
#define tStopButtonTimer     data[1]
#define tStopButtonReelIndex data[15]

static void PressStopReelButton(u8 reelIndex)
{
    u8 taskId = CreateTask(Task_PressStopReelButton, 5);
    gTasks[taskId].tStopButtonReelIndex = reelIndex;
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
    sReelStopButtonTasks[gTasks[taskId].tStopButtonState](gTasks + taskId, taskId);
}

static const s16 sReelButtonOffsets[] = {5, 10, 15};

static void StopReelButton_Press(struct Task *task, u8 taskId)
{
    SetReelButtonTilemap(sReelButtonOffsets[task->tStopButtonReelIndex], 0x62, 0x63, 0x72, 0x73);
    task->tStopButtonState++;
}

static void StopReelButton_Wait(struct Task *task, u8 taskId)
{
    if (++task->tStopButtonTimer > 11)
        task->tStopButtonState++;
}

static void StopReelButton_Unpress(struct Task *task, u8 taskId)
{
    SetReelButtonTilemap(sReelButtonOffsets[task->tStopButtonReelIndex], 0x42, 0x43, 0x52, 0x53);
    DestroyTask(taskId);
}

#undef tStopButtonState
#undef tStopButtonTimer
#undef tStopButtonReelIndex

static const u16 *const sLitMatchLinePalTable[];
static const u16 *const sDarkMatchLinePalTable[];
static const u8 sMatchLinePalOffsets[];

static void LightenMatchLine(u8 matchLineId)
{
    LoadPalette(sLitMatchLinePalTable[matchLineId], sMatchLinePalOffsets[matchLineId], 2);
}

static void DarkenMatchLine(u8 matchLineId)
{
    LoadPalette(sDarkMatchLinePalTable[matchLineId], sMatchLinePalOffsets[matchLineId], 2);
}

static const u8 sBetToMatchLineIds[][2];
static const u8 sMatchLinesPerBet[];

static void LightenBetTiles(u8 bet)
{
    u8 i;
    for (i = 0; i < sMatchLinesPerBet[bet]; i++)
        LightenMatchLine(sBetToMatchLineIds[bet][i]);
}

static void DarkenBetTiles(u8 bet)
{
    u8 i;
    for (i = 0; i < sMatchLinesPerBet[bet]; i++)
        DarkenMatchLine(sBetToMatchLineIds[bet][i]);
}

#define sMatchLineId     data[0]
#define sFlashing        data[1]
#define sNumFullFlashes  data[2]
#define sDelayTimer      data[3]
#define sColor           data[4]
#define sColorIncr       data[5]
#define sAtOriginalColor data[7]

static void CreateInvisibleFlashMatchLineSprites(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(sSlotMachine->flashMatchLineSpriteIds); i++)
    {
        u8 spriteId = CreateInvisibleSprite(SpriteCB_FlashMatchingLines);
        gSprites[spriteId].sMatchLineId = i;
        sSlotMachine->flashMatchLineSpriteIds[i] = spriteId;
    }
}

static void FlashMatchLine(u8 matchLineId)
{
    struct Sprite *sprite = gSprites + sSlotMachine->flashMatchLineSpriteIds[matchLineId];
    sprite->sFlashing = TRUE;
    sprite->sNumFullFlashes = 4;
    sprite->sDelayTimer = 0;
    sprite->sColor = 0;
    sprite->sColorIncr = 2;
    sprite->sAtOriginalColor = FALSE;
}

static bool8 IsMatchLineDoneFlashingBeforePayout(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(sSlotMachine->flashMatchLineSpriteIds); i++)
    {
        struct Sprite *sprite = &gSprites[sSlotMachine->flashMatchLineSpriteIds[i]];
        if (sprite->sFlashing && sprite->sNumFullFlashes)
            return FALSE;
    }
    return TRUE;
}

static bool8 TryStopMatchLinesFlashing(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(sSlotMachine->flashMatchLineSpriteIds); i++)
    {
        if (!TryStopMatchLineFlashing(sSlotMachine->flashMatchLineSpriteIds[i]))
            return FALSE;
    }
    return TRUE;
}

static bool8 TryStopMatchLineFlashing(u8 spriteId)
{
    struct Sprite *sprite = gSprites + spriteId;
    if (!sprite->sFlashing)
        return TRUE;
    if (sprite->sAtOriginalColor)
        sprite->sFlashing = FALSE;
    return sprite->sAtOriginalColor;
}

static void SpriteCB_FlashMatchingLines(struct Sprite *sprite)
{
    s16 maxColorChange;
    if (sprite->sFlashing)
    {
        if (!sprite->sDelayTimer--)
        {
            sprite->sAtOriginalColor = FALSE;
            sprite->sDelayTimer = 1;
            sprite->sColor += sprite->sColorIncr;
            maxColorChange = 4;
            if (sprite->sNumFullFlashes)
                maxColorChange = 8;
            if (sprite->sColor <= 0)
            {
                sprite->sAtOriginalColor = TRUE;
                sprite->sColorIncr = -sprite->sColorIncr;
                if (sprite->sNumFullFlashes)
                    sprite->sNumFullFlashes--;
            }
            else if (sprite->sColor >= maxColorChange)
                sprite->sColorIncr = -sprite->sColorIncr;
            if (sprite->sNumFullFlashes)
                sprite->sDelayTimer <<= 1;
        }
        MultiplyPaletteRGBComponents(sMatchLinePalOffsets[sprite->sMatchLineId], sprite->sColor, sprite->sColor, sprite->sColor);
    }
}

#undef sMatchLineId
#undef sFlashing
#undef sNumFullFlashes
#undef sDelayTimer
#undef sColor
#undef sColorIncr
#undef sAtOriginalColor

static const u16 *const sFlashingLightsPalTable[];
static const u16 *const sSlotMachineMenu_Pal;

#define tFlashTimer        data[1]
#define tLightPaletteIndex data[2]
#define tPaletteDirection  data[3]

static void FlashSlotMachineLights(void)
{
    u8 taskId = CreateTask(Task_FlashSlotMachineLights, 6);
    gTasks[taskId].tPaletteDirection = 1;
    Task_FlashSlotMachineLights(taskId);
}

static bool8 TryStopSlotMachineLights(void)
{
    u8 taskId = FindTaskIdByFunc(Task_FlashSlotMachineLights);
    if (!gTasks[taskId].tLightPaletteIndex)
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
    if (!task->tFlashTimer--)
    {
        task->tFlashTimer = 4;
        task->tLightPaletteIndex += task->tPaletteDirection;
        if (task->tLightPaletteIndex == 0 || task->tLightPaletteIndex == 2)
            task->tPaletteDirection = -task->tPaletteDirection;
    }
    LoadPalette(sFlashingLightsPalTable[task->tLightPaletteIndex], 0x10, 0x20);
}

#undef tFlashTimer
#undef tLightPaletteIndex
#undef tPaletteDirection

#define tState      data[0]
#define tNumBolts   data[1]
#define tSpriteId   data[2]
#define tTimer      data[2]
#define tAnimating  data[15]

static void CreatePikaPowerBoltTask(void)
{
    sSlotMachine->pikaPowerBoltTaskId = CreateTask(Task_CreatePikaPowerBolt, 8);
}

static void AddPikaPowerBolt(u8 pikaPowerBolts)
{
    struct Task *task = gTasks + sSlotMachine->pikaPowerBoltTaskId;
    ResetPikaPowerBoltTask(task);
    task->tState = PIKABOLT_TASK_ADD_BOLT;
    task->tNumBolts++;
    task->tAnimating = TRUE;
}

static void ResetPikaPowerBolts(void)
{
    struct Task *task = gTasks + sSlotMachine->pikaPowerBoltTaskId;
    ResetPikaPowerBoltTask(task);
    task->tState = PIKABOLT_TASK_CLEAR_ALL;
    task->tAnimating = TRUE;
}

static bool8 IsPikaPowerBoltAnimating(void)
{
    return gTasks[sSlotMachine->pikaPowerBoltTaskId].tAnimating;
}

static void (*const sPikaPowerBoltTasks[])(struct Task *task) =
{
    [PIKABOLT_TASK_IDLE] = PikaPowerBolt_Idle,
    [PIKABOLT_TASK_ADD_BOLT] = PikaPowerBolt_AddBolt,
    [PIKABOLT_TASK_WAIT_ANIM] = PikaPowerBolt_WaitAnim,
    [PIKABOLT_TASK_CLEAR_ALL] = PikaPowerBolt_ClearAll,
};

static void Task_CreatePikaPowerBolt(u8 taskId)
{
    sPikaPowerBoltTasks[gTasks[taskId].tState](gTasks + taskId);
}

static void PikaPowerBolt_Idle(struct Task *task)
{
}

static void PikaPowerBolt_AddBolt(struct Task *task)
{
    task->tSpriteId = CreatePikaPowerBoltSprite((task->tNumBolts << 3) + 20, 20);
    task->tState++;
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
    if (gSprites[task->tSpriteId].data[7])
    {
        s16 tileOffset = task->tNumBolts + 2;
        u8 tileVariant = 0;
        if (task->tNumBolts == 1)
            tileVariant = 1;
        else if (task->tNumBolts == 16)
            tileVariant = 2;
        vaddr[tileOffset + 0x40] = sPikaPowerTileTable[tileVariant][0];
        DestroyPikaPowerBoltSprite(task->tSpriteId);
        task->tState = PIKABOLT_TASK_IDLE;
        task->tAnimating = FALSE;
    }
}

static void PikaPowerBolt_ClearAll(struct Task *task)
{
    u16 *vaddr = (u16 *)BG_SCREEN_ADDR(29);
    s16 tileOffset = task->tNumBolts + 2;
    u8 tileVariant = 0;
    if (task->tNumBolts == 1)
        tileVariant = 1;
    else if (task->tNumBolts == 16)
        tileVariant = 2;
    if (task->tTimer == 0)
    {
        vaddr[tileOffset + 0x40] = sPikaPowerTileTable[tileVariant][1];
        task->tNumBolts--;
    }
    if (++task->tTimer >= 20)
        task->tTimer = 0;
    if (task->tNumBolts == 0)
    {
        task->tState = PIKABOLT_TASK_IDLE;
        task->tAnimating = FALSE;
    }
}

static void ResetPikaPowerBoltTask(struct Task *task)
{
    u8 i;

    for (i = 2; i < 16; i++)
        task->data[i] = 0;
}

static void LoadPikaPowerMeter(u8 pikaPowerBolts)
{
    s16 i;
    u8 tileVariant;
    s16 tileOffset = 3;
    u16 *vaddr = (u16 *)BG_SCREEN_ADDR(29);
    for (i = 0; i < pikaPowerBolts; i++, tileOffset++)
    {
        tileVariant = 0;
        if (i == 0)
            tileVariant = 1;
        else if (i == 15)
            tileVariant = 2;
        vaddr[tileOffset + 0x40] = sPikaPowerTileTable[tileVariant][0];
    }
    for (; i < 16; i++, tileOffset++)
    {
        tileVariant = 0;
        if (i == 0)
            tileVariant = 1;
        else if (i == 15)
            tileVariant = 2;
        vaddr[tileOffset + 0x40] = sPikaPowerTileTable[tileVariant][1];
    }
    gTasks[sSlotMachine->pikaPowerBoltTaskId].tNumBolts = pikaPowerBolts;
}

#undef tState
#undef tNumBolts
#undef tSpriteId
#undef tTimer
#undef tAnimating

#define tState               data[0]
#define tWindowOffset        data[1]
#define tReelSpeed           data[1]
#define tWindowColumn        data[2]
#define tTimer3              data[2]
#define tWindowTilemapOffset data[3]
#define tRtReelSpeed         data[4]
#define tTimer2              data[4]
#define tTimer1              data[5]
#define tExplodeChecks       data[6]

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
    [RT_TASK_INIT] = ReelTime_Init,
    [RT_TASK_WINDOW_ENTER] = ReelTime_WindowEnter,
    [RT_TASK_WAIT_START_PIKA] = ReelTime_WaitStartPikachu,
    [RT_TASK_PIKA_SPEEDUP1] = ReelTime_PikachuSpeedUp1,
    [RT_TASK_PIKA_SPEEDUP2] = ReelTime_PikachuSpeedUp2,
    [RT_TASK_WAIT_REEL] = ReelTime_WaitReel,
    [RT_TASK_CHECK_EXPLODE] = ReelTime_CheckExplode,
    [RT_TASK_LAND] = ReelTime_LandOnOutcome,
    [RT_TASK_PIKA_REACT] = ReelTime_PikachuReact,
    [RT_TASK_WAIT_CLEAR_POWER] = ReelTime_WaitClearPikaPower,
    [RT_TASK_CLOSE_WINDOW_SUCCESS] = ReelTime_CloseWindow,
    [RT_TASK_DESTROY_SPRITES] = ReelTime_DestroySprites,
    [RT_TASK_SET_REEL_SPEED] = ReelTime_SetReelSpeed,
    [RT_TASK_END_SUCCESS] = ReelTime_EndSuccess,
    [RT_TASK_EXPLODE] = ReelTime_ExplodeMachine,
    [RT_TASK_WAIT_EXPLODE] = ReelTime_WaitExplode,
    [RT_TASK_WAIT_SMOKE] = ReelTime_WaitSmoke,
    [RT_TASK_CLOSE_WINDOW_FAILURE] = ReelTime_CloseWindow,
    [RT_TASK_END_FAILURE] = ReelTime_EndFailure,
};

static void Task_ReelTime(u8 taskId)
{
    sReelTimeTasks[gTasks[taskId].tState](gTasks + taskId);
}

static void ReelTime_Init(struct Task *task)
{
    sSlotMachine->reelTimeSpinsLeft = 0;
    sSlotMachine->reeltimePixelOffset = 0;
    sSlotMachine->reeltimePosition = 0;
    task->tState++;
    task->tWindowOffset = 0;
    task->tWindowColumn = 30;
    task->tRtReelSpeed = 1280;
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
    s16 windowColumn;
    gSpriteCoordOffsetX -= 8;
    task->tWindowOffset += 8;
    windowColumn = ((task->tWindowOffset + 240) & 0xff) >> 3;
    REG_BG1HOFS = task->tWindowOffset & 0x1ff;
    if (windowColumn != task->tWindowColumn && task->tWindowTilemapOffset <= 18)
    {
        task->tWindowColumn = windowColumn;
        task->tWindowTilemapOffset = task->tWindowOffset >> 3;
        LoadReelTimeWindowTilemap(windowColumn, task->tWindowTilemapOffset);
    }
    if (task->tWindowOffset >= 200)
    {
        task->tState++;
        task->tWindowTilemapOffset = 0;
    }
    AdvanceReeltimeReel(task->tRtReelSpeed >> 8);
}

static void ReelTime_WaitStartPikachu(struct Task *task)
{
    AdvanceReeltimeReel(task->tRtReelSpeed >> 8);
    if (++task->tTimer1 >= 60)
    {
        task->tState++;
        CreateReelTimeBoltSprites();
        CreateReelTimePikachuAuraSprites();
    }
}

static void ReelTime_PikachuSpeedUp1(struct Task *task)
{
    int speedIdx;
    u8 pikachuAnimIds[] = {1, 1, 2, 2};
    s16 reelTimeBoltDelays[] = {0x40, 0x30, 0x18, 0x08};
    s16 pikachuAuraFlashDelays[] = {10, 8, 6, 4};

    AdvanceReeltimeReel(task->tRtReelSpeed >> 8);
    task->tRtReelSpeed -= 4;
    speedIdx = 4 - (task->tRtReelSpeed >> 8);
    SetReelTimeBoltDelay(reelTimeBoltDelays[speedIdx]);
    SetReelTimePikachuAuraFlashDelay(pikachuAuraFlashDelays[speedIdx]);
    StartSpriteAnimIfDifferent(gSprites + sSlotMachine->reelTimePikachuSpriteId, pikachuAnimIds[speedIdx]);
    if (task->tRtReelSpeed <= 0x100)
    {
        task->tState++;
        task->tRtReelSpeed = 0x100;
        task->tTimer1 = 0;
    }
}

static void ReelTime_PikachuSpeedUp2(struct Task *task)
{
    AdvanceReeltimeReel(task->tRtReelSpeed >> 8);
    if (++task->tTimer1 >= 80)
    {
        task->tState++;
        task->tTimer1 = 0;
        SetReelTimePikachuAuraFlashDelay(2);
        StartSpriteAnimIfDifferent(gSprites + sSlotMachine->reelTimePikachuSpriteId, 3);
    }
}

static void ReelTime_WaitReel(struct Task *task)
{
    AdvanceReeltimeReel(task->tRtReelSpeed >> 8);
    task->tRtReelSpeed = (u8)task->tRtReelSpeed + 0x80;
    if (++task->tTimer1 >= 80)
    {
        task->tState++;
        task->tTimer1 = 0;
    }
}

static void ReelTime_CheckExplode(struct Task *task)
{
    AdvanceReeltimeReel(task->tRtReelSpeed >> 8);
    task->tRtReelSpeed = (u8)task->tRtReelSpeed + 0x40;
    if (++task->tTimer1 >= 40)
    {
        task->tTimer1 = 0;
        if (sSlotMachine->reelTimeDraw)
        {
            if (sSlotMachine->reelTimeSpinsLeft <= task->tExplodeChecks)
                task->tState++;
        }
        else if (task->tExplodeChecks > 3)
        {
            task->tState++;
        }
        else if (ShouldReelTimeMachineExplode(task->tExplodeChecks))
        {
            task->tState = RT_TASK_EXPLODE;
        }
        task->tExplodeChecks++;
    }
}

static void ReelTime_LandOnOutcome(struct Task *task)
{
    s16 symbolOffset = sSlotMachine->reeltimePixelOffset % REELTIME_SYMBOL_HEIGHT;
    if (symbolOffset)
    {
        symbolOffset = AdvanceReeltimeReelToNextSymbol(task->tRtReelSpeed >> 8);
        task->tRtReelSpeed = (u8)task->tRtReelSpeed + 0x40;
    }
    else if (GetReelTimeSymbol(1) != sSlotMachine->reelTimeDraw)
    {
        AdvanceReeltimeReel(task->tRtReelSpeed >> 8);
        symbolOffset = sSlotMachine->reeltimePixelOffset % REELTIME_SYMBOL_HEIGHT;
        task->tRtReelSpeed = (u8)task->tRtReelSpeed + 0x40;
    }
    if (symbolOffset == 0 && GetReelTimeSymbol(1) == sSlotMachine->reelTimeDraw)
    {
        task->tRtReelSpeed = 0;
        task->tState++;
    }
}

static void ReelTime_PikachuReact(struct Task *task)
{
    if (++task->tTimer2 >= 60)
    {
        StopMapMusic();
        DestroyReelTimeBoltSprites();
        DestroyReelTimePikachuAuraSprites();
        task->tState++;
        if(sSlotMachine->reelTimeDraw == 0)
        {
            task->tTimer2 = 0xa0;
            StartSpriteAnimIfDifferent(gSprites + sSlotMachine->reelTimePikachuSpriteId, 5);
            PlayFanfare(MUS_TOO_BAD);
        }
        else
        {
            task->tTimer2 = 0xc0;
            StartSpriteAnimIfDifferent(gSprites + sSlotMachine->reelTimePikachuSpriteId, 4);
            gSprites[sSlotMachine->reelTimePikachuSpriteId].animCmdIndex = 0;
            if (sSlotMachine->pikaPowerBolts)
            {
                ResetPikaPowerBolts();
                sSlotMachine->pikaPowerBolts = 0;
            }
            PlayFanfare(MUS_SLOTS_WIN);
        }
    }
}

static void ReelTime_WaitClearPikaPower(struct Task *task)
{
    if ((task->tTimer2 == 0 || --task->tTimer2 == 0) && !IsPikaPowerBoltAnimating())
        task->tState++;
}

static void ReelTime_CloseWindow(struct Task *task)
{
    s16 windowColumn;
    gSpriteCoordOffsetX -= 8;
    task->tWindowOffset += 8;
    task->tWindowTilemapOffset += 8;
    windowColumn = ((task->tWindowOffset - 8) & 0xff) >> 3;
    REG_BG1HOFS = task->tWindowOffset & 0x1ff;
    if (task->tWindowTilemapOffset >> 3 <= 25)
        ClearReelTimeWindowTilemap(windowColumn);
    else
        task->tState++;
}

static void ReelTime_DestroySprites(struct Task *task)
{
    sSlotMachine->reelTimeSpinsUsed = 0;
    sSlotMachine->reelTimeSpinsLeft = sSlotMachine->reelTimeDraw;
    gSpriteCoordOffsetX = 0;
    REG_BG1HOFS = 0;
    sSlotMachine->reelSpeed = REEL_NORMAL_SPEED;
    DestroyReelTimePikachuSprite();
    DestroyReelTimeMachineSprites();
    DestroyReelTimeShadowSprites();
    PlayNewMapMusic(sSlotMachine->backupMapMusic);
    if (sSlotMachine->reelTimeSpinsLeft == 0)
    {
        DestroyTask(FindTaskIdByFunc(Task_ReelTime));
    }
    else
    {
        CreateDigitalDisplayScene(DIG_DISPLAY_REEL_TIME);
        task->tReelSpeed = ReelTimeSpeed();
        task->tTimer3 = 0;
        task->tWindowTilemapOffset = 0;
        task->tState++;
    }
}

static void ReelTime_SetReelSpeed(struct Task *task)
{
    if (sSlotMachine->reelSpeed == task->tReelSpeed)
        task->tState++;
    else if (sSlotMachine->reelPixelOffsets[LEFT_REEL] % REEL_SYMBOL_HEIGHT == 0 && (++task->tTimer3 & 0x07) == 0)
        sSlotMachine->reelSpeed >>= 1;
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
    gSprites[sSlotMachine->reelTimeShadowSpriteIds[0]].invisible = TRUE;
    StartSpriteAnimIfDifferent(gSprites + sSlotMachine->reelTimePikachuSpriteId, 5);
    task->tState++;
    task->tTimer2 = 4;
    task->tTimer1 = 0;
    StopMapMusic();
    PlayFanfare(MUS_TOO_BAD);
    PlaySE(SE_M_EXPLOSION);
}

static void ReelTime_WaitExplode(struct Task *task)
{
    gSpriteCoordOffsetY = task->tTimer2;
    REG_BG1VOFS = task->tTimer2;
    if (task->tTimer1 & 0x01)
        task->tTimer2 = -task->tTimer2;
    if ((++task->tTimer1 & 0x1f) == 0)
        task->tTimer2 >>= 1;
    if (task->tTimer2 == 0)
    {
        DestroyReelTimeExplosionSprite();
        CreateReelTimeDuckSprites();
        CreateBrokenReelTimeMachineSprite();
        CreateReelTimeSmokeSprite();
        gSprites[sSlotMachine->reelTimeShadowSpriteIds[0]].invisible = FALSE;
        task->tState++;
        task->tTimer1 = 0;
    }
}

static void ReelTime_WaitSmoke(struct Task *task)
{
    gSpriteCoordOffsetY = 0;
    REG_BG1VOFS = 0;
    if (IsReelTimeSmokeAnimFinished())
    {
        task->tState++;
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

#undef tState
#undef tWindowOffset
#undef tReelSpeed
#undef tWindowColumn
#undef tTimer3
#undef tWindowTilemapOffset
#undef tRtReelSpeed
#undef tTimer2
#undef tTimer1
#undef tExplodeChecks

static const u16 sReelTimeWindow_Tilemap[];

static void LoadReelTimeWindowTilemap(s16 x, s16 tilemapOffset)
{
    s16 i;

    for (i = 4; i < 15; i++)
    {
        u16 tile = sReelTimeWindow_Tilemap[tilemapOffset + (i - 4) * 20];
        ((u16 *)BG_SCREEN_ADDR(28))[32 * i + x] = tile;
    }
}

static void ClearReelTimeWindowTilemap(s16 x)
{
    s16 i;

    for (i = 4; i < 15; i++)
    {
        ((u16 *)BG_SCREEN_ADDR(28))[32 * i + x] = 0;
    }
}

#define tState        data[0]
#define tDigDisplayId data[1]

static void OpenInfoBox(u8 digDisplayId)
{
    u8 taskId = CreateTask(Task_InfoBox, 1);
    gTasks[taskId].tDigDisplayId = digDisplayId;
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
    sInfoBoxTasks[gTasks[taskId].tState](gTasks + taskId);
}

static void InfoBox_FadeIn(struct Task *task)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    task->tState++;
}

static void InfoBox_WaitFade(struct Task *task)
{
    if (!gPaletteFade.active)
        task->tState++;
}

static void InfoBox_DrawWindowAndText(struct Task *task)
{
    DestroyDigitalDisplayScene();
    LoadInfoBoxTilemap();
    BasicInitMenuWindow(&gSlotMachineInfoBoxWindowTemplate);
    Menu_PrintTextPixelCoords(gOtherText_ReelTime, 10, 32, 1);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    task->tState++;
}

static void InfoBox_WaitInput(struct Task *task)
{
    if (JOY_NEW(B_BUTTON | SELECT_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        task->tState++;
    }
}

static void InfoBox_RestoreSlotMachineDisplay(struct Task *task)
{
    Menu_EraseScreen();
    BasicInitMenuWindow(&gSlotMachineMainWindowTemplate);
    LoadMenuAndReelOverlayTilemaps();
    CreateDigitalDisplayScene(task->tDigDisplayId);
    LoadPikaPowerMeter(sSlotMachine->pikaPowerBolts);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    task->tState++;
}

static void InfoBox_FreeTask(struct Task *task)
{
    DestroyTask(FindTaskIdByFunc(Task_InfoBox));
}

#undef tState
#undef tDigDisplayId

#define tDigDisplayState           data[0]
#define tDigDisplaySceneId         data[1]
#define tDigDisplaySpriteId(i)     data[4 + (i)]

static void CreateDigitalDisplayTask(void)
{
    u8 i;
    struct Task *task;
    i = CreateTask(Task_DigitalDisplay, 3);
    sSlotMachine->digDisplayTaskId = i;
    task = gTasks + i;
    task->tDigDisplaySceneId = -1;
    for (i = 4; i < 16; i++)
        task->data[i] = MAX_SPRITES;
}

static void LoadSlotMachineReelOverlay(void);

static void CreateDigitalDisplayScene(u8 sceneId)
{
    u8 i;
    struct Task *task;

    DestroyDigitalDisplayScene();

    task = gTasks + sSlotMachine->digDisplayTaskId;
    task->tDigDisplaySceneId = sceneId;

    for (i = 0; sDigitalDisplayScenes[sceneId][i].spriteTemplateId != 0xFF; i++)
    {
        u8 spriteId;
        spriteId = CreateStdDigitalDisplaySprite(
                sDigitalDisplayScenes[sceneId][i].spriteTemplateId,
                sDigitalDisplayScenes[sceneId][i].dispInfoId,
                sDigitalDisplayScenes[sceneId][i].spriteId
        );
        task->tDigDisplaySpriteId(i) = spriteId;

#ifdef GERMAN
        if (sceneId == DIG_DISPLAY_BONUS_REG && i <= 2)
            gSprites[spriteId].invisible = TRUE;
#endif
    }
}

static void AddDigitalDisplaySprite(u8 templateIdx, SpriteCallback callback, s16 x, s16 y, s16 spriteId)
{
    u8 i;
    struct Task *task = gTasks + sSlotMachine->digDisplayTaskId;
    for (i = 4; i < 16; i++)
    {
        if (task->data[i] == MAX_SPRITES)
        {
            task->data[i] = CreateDigitalDisplaySprite(templateIdx, callback, x, y, spriteId);
            break;
        }
    }
}

static void (*const sDigitalDisplaySceneExitCallbacks[])(void);

void DestroyDigitalDisplayScene(void)
{
    u8 i;
    struct Task *task = gTasks + sSlotMachine->digDisplayTaskId;
    if ((u16)task->tDigDisplaySceneId != 0xFFFF)
        sDigitalDisplaySceneExitCallbacks[task->tDigDisplaySceneId]();
    // task data[4..15] stores the sprite IDs for the active display scene.
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
    struct Task *task = gTasks + sSlotMachine->digDisplayTaskId;
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
    sDigitalDisplayTasks[gTasks[taskId].tDigDisplayState](gTasks + taskId);
}

static void DigitalDisplay_Idle(struct Task *task)
{
}

#undef tDigDisplayState
#undef tDigDisplaySceneId
#undef tDigDisplaySpriteId

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
    sprite->data[2] = sSlotMachine->reelPixelOffsets[sprite->data[0]] + sprite->data[1];
    sprite->data[2] %= 120;
    sprite->y = sSlotMachine->reelShockOffsets[sprite->data[0]] + 28 + sprite->data[2];
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

static void CreateCoinNumberSprite(s16 x, s16 y, u8 isPayout, s16 digitMultiplier)
{
    struct Sprite *sprite = gSprites + CreateSprite(&sSpriteTemplate_CoinNumber, x, y, 13);
    sprite->oam.priority = 2;
    sprite->data[0] = isPayout;
    sprite->data[1] = digitMultiplier;
    sprite->data[2] = digitMultiplier * 10;
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
    sSlotMachine->reelTimePikachuSpriteId = spriteId;
}

static void DestroyReelTimePikachuSprite(void)
{
    DestroySprite(gSprites + sSlotMachine->reelTimePikachuSpriteId);
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
    sSlotMachine->reelTimeMachineSpriteIds[0] = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimeMachine, 0x170, 0x54, 7);
    sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sprite->coordOffsetEnabled = TRUE;
    SetSubspriteTables(sprite, sSubspriteTable_ReelTimeMachine);
    sSlotMachine->reelTimeMachineSpriteIds[1] = spriteId;
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
    sSlotMachine->reelTimeBrokenMachineSpriteId = spriteId;
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeNumbers;

static void CreateReelTimeNumberSprites(void)
{
    u8 i;
    s16 symbolOffset;
    for (i = 0, symbolOffset = 0; i < 3; i++, symbolOffset += REELTIME_SYMBOL_HEIGHT)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeNumbers, 0x170, 0x00, 10);
        struct Sprite *sprite = &gSprites[spriteId];
        sprite->oam.priority = 1;
        sprite->coordOffsetEnabled = TRUE;
        sprite->data[7] = symbolOffset;
        sSlotMachine->reelTimeNumberSpriteIds[i] = spriteId;
    }
}

static void SpriteCB_ReelTimeNumbers(struct Sprite *sprite)
{
    s16 symbolOffset = (u16)(sSlotMachine->reeltimePixelOffset + sprite->data[7]);
    symbolOffset %= (REELTIME_SYMBOL_HEIGHT * 2);
    sprite->y = symbolOffset + 59;
    StartSpriteAnimIfDifferent(sprite, GetReelTimeSymbol(symbolOffset / REELTIME_SYMBOL_HEIGHT));
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
    sSlotMachine->reelTimeShadowSpriteIds[0] = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimeShadow, 0x120, 0x68, 4);
    sprite = &gSprites[spriteId];
    sprite->coordOffsetEnabled = TRUE;
    sprite->oam.priority = 1;
    SetSubspriteTables(sprite, sSubspriteTable_ReelTimeShadow);
    sSlotMachine->reelTimeShadowSpriteIds[1] = spriteId;
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
    sSlotMachine->reelTimeNumberGapSpriteId = spriteId;
}

static void DestroyReelTimeMachineSprites(void)
{
    u8 i;

    DestroySprite(&gSprites[sSlotMachine->reelTimeNumberGapSpriteId]);
    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->reelTimeMachineSpriteIds[i]]);
    for (i = 0; i < 3; i++)
        DestroySprite(&gSprites[sSlotMachine->reelTimeNumberSpriteIds[i]]);
}

static void DestroyReelTimeShadowSprites(void)
{
    u8 i;

    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->reelTimeShadowSpriteIds[i]]);
}

static void DestroyBrokenReelTimeMachineSprite(void)
{
    DestroySprite(&gSprites[sSlotMachine->reelTimeBrokenMachineSpriteId]);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeBolt;

static void CreateReelTimeBoltSprites(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeBolt, 0x98, 0x20, 5);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sprite->hFlip = TRUE;
    sSlotMachine->reelTimeBoltSpriteIds[0] = spriteId;
    sprite->data[0] = 8;
    sprite->data[1] = -1;
    sprite->data[2] = -1;
    sprite->data[7] = 0x20;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimeBolt, 0xb8, 0x20, 5);
    sprite = &gSprites[spriteId];
    sprite->oam.priority = 1;
    sSlotMachine->reelTimeBoltSpriteIds[1] = spriteId;
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

static void SetReelTimeBoltDelay(s16 delay)
{
    gSprites[sSlotMachine->reelTimeBoltSpriteIds[0]].data[7] = delay;
    gSprites[sSlotMachine->reelTimeBoltSpriteIds[1]].data[7] = delay;
}

static void DestroyReelTimeBoltSprites(void)
{
    u8 i;

    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->reelTimeBoltSpriteIds[i]]);
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
    sSlotMachine->reelTimePikachuAuraSpriteIds[0] = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_ReelTimePikachuAura, 0x68, 0x50, 3);
    gSprites[spriteId].oam.priority = 1;
    gSprites[spriteId].hFlip = TRUE;
    sSlotMachine->reelTimePikachuAuraSpriteIds[1] = spriteId;
}

static const u8 gUnknown_083ECC58[2]; // don't remove this until decompiled through sInitialReelPositions

static void SpriteCB_ReelTimePikachuAura(struct Sprite *sprite)
{
    u8 auraBrightnesses[] = {16, 0};
    if (sprite->data[0] && --sprite->data[6] <= 0)
    {
        MultiplyInvertedPaletteRGBComponents((IndexOfSpritePaletteTag(PALTAG_PIKA_AURA) << 4) + 0x103, auraBrightnesses[sprite->data[5]], auraBrightnesses[sprite->data[5]], auraBrightnesses[sprite->data[5]]);
        ++sprite->data[5];
        sprite->data[5] &= 1;
        sprite->data[6] = sprite->data[7];
    }
}

static void SetReelTimePikachuAuraFlashDelay(s16 delay)
{
    gSprites[sSlotMachine->reelTimePikachuAuraSpriteIds[0]].data[7] = delay;
}

static void DestroyReelTimePikachuAuraSprites(void)
{
    u8 i;
    MultiplyInvertedPaletteRGBComponents((IndexOfSpritePaletteTag(PALTAG_PIKA_AURA) << 4) + 0x103, 0, 0, 0);
    for (i = 0; i < 2; i++)
        DestroySprite(&gSprites[sSlotMachine->reelTimePikachuAuraSpriteIds[i]]);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeExplosion;

static void CreateReelTimeExplosionSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeExplosion, 0xa8, 0x50, 6);
    gSprites[spriteId].oam.priority = 1;
    sSlotMachine->reelTimeExplosionSpriteId = spriteId;
}

static void SpriteCB_ReelTimeExplosion(struct Sprite *sprite)
{
    sprite->y2 = gSpriteCoordOffsetY;
}

static void DestroyReelTimeExplosionSprite(void)
{
    DestroySprite(&gSprites[sSlotMachine->reelTimeExplosionSpriteId]);
}

static const struct SpriteTemplate sSpriteTemplate_ReelTimeDuck;

static void CreateReelTimeDuckSprites(void)
{
    u8 i;
    u16 duckAngles[] = {0x0, 0x40, 0x80, 0xC0};
    for (i = 0; i < 4; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_ReelTimeDuck, 0x50 - gSpriteCoordOffsetX, 0x44, 0);
        struct Sprite *sprite = &gSprites[spriteId];
        sprite->oam.priority = 1;
        sprite->coordOffsetEnabled = TRUE;
        sprite->data[0] = duckAngles[i];
        sSlotMachine->reelTimeDuckSpriteIds[i] = spriteId;
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
        DestroySprite(&gSprites[sSlotMachine->reelTimeDuckSpriteIds[i]]);
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
    sSlotMachine->reelTimeSmokeSpriteId = spriteId;
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
    return gSprites[sSlotMachine->reelTimeSmokeSpriteId].data[7];
}

static void DestroyReelTimeSmokeSprite(void)
{
    struct Sprite *sprite = &gSprites[sSlotMachine->reelTimeSmokeSpriteId];
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

u8 CreateStdDigitalDisplaySprite(u8 templateIdx, u8 dispInfoId, s16 spriteId)
{
    return CreateDigitalDisplaySprite(templateIdx, sDigitalDisplay_SpriteCallbacks[dispInfoId], sDigitalDisplay_SpriteCoords[dispInfoId][0], sDigitalDisplay_SpriteCoords[dispInfoId][1], spriteId);
}

static const struct SpriteTemplate *const sSpriteTemplates_DigitalDisplay[NUM_DIG_DISPLAY_SPRITES];
static const struct SubspriteTable *const sSubspriteTables_DigitalDisplay[NUM_DIG_DISPLAY_SPRITES];

static u8 CreateDigitalDisplaySprite(u8 templateIdx, SpriteCallback callback, s16 x, s16 y, s16 internalSpriteId)
{
    u8 spriteId = CreateSprite(sSpriteTemplates_DigitalDisplay[templateIdx], x, y, 16);
    struct Sprite *sprite = &gSprites[spriteId];
    sprite->oam.priority = 3;
    sprite->callback = callback;
    sprite->data[6] = internalSpriteId;
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
    s16 xOffsets[] = {4, -4, 4, -4};
    s16 yOffsets[] = {4, 4, -4, -4};

    if (sprite->data[1]++ >= 16)
    {
        sprite->subspriteTableNum ^= 1;
        sprite->data[1] = 0;
    }
    sprite->x2 = 0;
    sprite->y2 = 0;
    if (sprite->subspriteTableNum != 0)
    {
        sprite->x2 = xOffsets[sprite->data[6]];
        sprite->y2 = yOffsets[sprite->data[6]];
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
        StartSpriteAnim(sprite, sSlotMachine->reelTimeSpinsLeft - 1);
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
        LoadPalette(sPokeballShiningPalTable[sprite->data[1]], (IndexOfSpritePaletteTag(PALTAG_DIG_DISPLAY) << 4) + 0x100, 0x20);
        if (++sprite->data[2] >= 4)
        {
            sprite->data[1]++;
            sprite->data[2] = 0;
        }
    }
    else
    {
        LoadPalette(sPokeballShiningPalTable[sprite->data[1]], (IndexOfSpritePaletteTag(PALTAG_DIG_DISPLAY) << 4) + 0x100, 0x20);
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
    s16 xOffsets[] = {0, -40, 0, 0, 48, 0, 24, 0};
    s16 yOffsets[] = {-32, 0, -32, -48, 0, -48, 0, -48};
    s16 delays[] = {16, 12, 16, 0, 0, 4, 8, 8};

    switch (sprite->data[0])
    {
    case 0:
        sprite->x2 = xOffsets[sprite->data[6]];
        sprite->y2 = yOffsets[sprite->data[6]];
        sprite->data[1] = delays[sprite->data[6]];
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
    s16 angles[] = {160, 192, 224, 104, 80, 64, 48, 24};

    if (sprite->data[0] == 0)
    {
        sprite->data[0]++;
        sprite->data[1] = 12;
    }
    sprite->x2 = Cos(angles[sprite->data[6]], sprite->data[1]);
    sprite->y2 = Sin(angles[sprite->data[6]], sprite->data[1]);
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
            AddDigitalDisplaySprite(DIG_SPRITE_A_BUTTON, SpriteCallbackDummy, 0xd0, 0x74, 0);
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

static const u16 *const sDigitalDisplay_Pal;

static void EndDigitalDisplayScene_Win(void)
{
    LoadPalette(sDigitalDisplay_Pal, (IndexOfSpritePaletteTag(PALTAG_DIG_DISPLAY) << 4) + 0x100, 0x20);
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
static const struct SpritePalette sSlotMachineSpritePalettes[];

static void LoadSlotMachineGfx(void)
{
    LoadReelBackground();
    LZDecompressWram(gSlotMachineDigitalDisplay_Gfx, eSlotMachineGfxBuffer);
    LZDecompressWram(sReelTimeGfx, eSlotMachineReelTimeGfxBuffer);
    LoadSpriteSheets(sSlotMachineSpriteSheets);
    LoadSpritePalettes(sSlotMachineSpritePalettes);
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
    LZDecompressWram(gSlotMachineMenu_Gfx, eSlotMachineGfxBuffer);

    DmaCopyLarge16(3, eSlotMachineGfxBuffer, BG_VRAM, SLOTMACHINE_GFX_TILES * 32, 0x1000);

    LoadPalette(gSlotMachineMenu_Pal, 0, 160);
    LoadPalette(sUnkPalette, 208, 32);
}

static void LoadMenuAndReelOverlayTilemaps(void)
{
    CpuCopy16(gSlotMachineMenu_Tilemap, BG_SCREEN_ADDR(29), 20 * 32 * 2);
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

static void SetReelButtonTilemap(s16 x, u16 topLeft, u16 topRight, u16 bottomLeft, u16 bottomRight)
{
    u16 *vram = BG_SCREEN_ADDR(29);

    vram[15 * 32 + x] = topLeft;
    vram[15 * 32 + 1 + x] = topRight;
    vram[16 * 32 + x] = bottomLeft;
    vram[16 * 32 + 1 + x] = bottomRight;
}

static void LoadInfoBoxTilemap(void)
{
    s16 y, x;
    u16 *screen;

    CpuCopy16(gSlotMachineInfoBox_Tilemap, BG_SCREEN_ADDR(29), 20 * 32 * 2);

    screen = BG_SCREEN_ADDR(30);
    for (y = 0; y < 20; y++)
    {
        for (x = 0; x < 30; x++)
            screen[x + y * 32] = 0;
    }
}

static const u8 sReelSymbols[NUM_REELS][SYMBOLS_PER_REEL] =
{
    {
        SYMBOL_7_RED,
        SYMBOL_CHERRY,
        SYMBOL_AZURILL,
        SYMBOL_REPLAY,
        SYMBOL_POWER,
        SYMBOL_LOTAD,
        SYMBOL_7_BLUE,
        SYMBOL_LOTAD,
        SYMBOL_CHERRY,
        SYMBOL_POWER,
        SYMBOL_REPLAY,
        SYMBOL_AZURILL,
        SYMBOL_7_RED,
        SYMBOL_POWER,
        SYMBOL_LOTAD,
        SYMBOL_REPLAY,
        SYMBOL_AZURILL,
        SYMBOL_7_BLUE,
        SYMBOL_POWER,
        SYMBOL_LOTAD,
        SYMBOL_REPLAY
    },
    {
        SYMBOL_7_RED,
        SYMBOL_CHERRY,
        SYMBOL_REPLAY,
        SYMBOL_LOTAD,
        SYMBOL_AZURILL,
        SYMBOL_CHERRY,
        SYMBOL_REPLAY,
        SYMBOL_POWER,
        SYMBOL_POWER,
        SYMBOL_LOTAD,
        SYMBOL_7_BLUE,
        SYMBOL_LOTAD,
        SYMBOL_REPLAY,
        SYMBOL_CHERRY,
        SYMBOL_AZURILL,
        SYMBOL_LOTAD,
        SYMBOL_REPLAY,
        SYMBOL_CHERRY,
        SYMBOL_LOTAD,
        SYMBOL_REPLAY,
        SYMBOL_CHERRY
    },
    {
        SYMBOL_7_RED,
        SYMBOL_POWER,
        SYMBOL_7_BLUE,
        SYMBOL_REPLAY,
        SYMBOL_LOTAD,
        SYMBOL_AZURILL,
        SYMBOL_REPLAY,
        SYMBOL_LOTAD,
        SYMBOL_POWER,
        SYMBOL_AZURILL,
        SYMBOL_REPLAY,
        SYMBOL_LOTAD,
        SYMBOL_AZURILL,
        SYMBOL_POWER,
        SYMBOL_REPLAY,
        SYMBOL_LOTAD,
        SYMBOL_AZURILL,
        SYMBOL_POWER,
        SYMBOL_REPLAY,
        SYMBOL_LOTAD,
        SYMBOL_CHERRY
    },
};

static const u8 sReelTimeSymbols[] = {
    1, 0, 5, 4, 3, 2
};

static const s16 sInitialReelPositions[NUM_REELS][2] = {
    {0,  6},
    {0, 10},
    {0,  2}
};

static const u8 sSpecialDrawOdds[][MAX_BET] = {
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
    SYMBOL_REPLAY,
    SYMBOL_CHERRY,
    SYMBOL_LOTAD,
    SYMBOL_AZURILL,
    SYMBOL_POWER,
    SYMBOL_7_RED,
    SYMBOL_7_RED,
    SYMBOL_7_RED
};

static const u16 sBiasesSpecial[] = {
    BIAS_STRAIGHT_7,
    BIAS_REELTIME,
    BIAS_MIXED_7
};

static const u16 sBiasesRegular[] = {
    BIAS_POWER,
    BIAS_AZURILL,
    BIAS_LOTAD,
    BIAS_CHERRY,
    BIAS_REPLAY
};

static const u8 sSymbolToMatch[] = {
    MATCH_RED_7,
    MATCH_BLUE_7,
    MATCH_AZURILL,
    MATCH_LOTAD,
    MATCH_CHERRY,
    MATCH_POWER,
    MATCH_REPLAY
};

static const u16 sSlotMatchFlags[] = {
    MATCH_FLAG(MATCH_CHERRY),
    MATCH_FLAG(MATCH_TOPBOT_CHERRY),
    MATCH_FLAG(MATCH_REPLAY),
    MATCH_FLAG(MATCH_LOTAD),
    MATCH_FLAG(MATCH_AZURILL),
    MATCH_FLAG(MATCH_POWER),
    MATCH_FLAG(MATCH_MIXED_7),
    MATCH_FLAG(MATCH_RED_7),
    MATCH_FLAG(MATCH_BLUE_7)
};

static const u16 sSlotPayouts[] = {
    2, 4, 0, 6, 12, 3, 90, 300, 300
};

static const s16 sDigitalDisplay_SpriteCoords[][2] = {
    [DIG_DISPINFO_INSERT]          = { 0xd0, 0x38},
    [DIG_DISPINFO_STOP_S]          = { 0xb8, 0x00},
    [DIG_DISPINFO_STOP_T]          = { 0xc8, 0x08},
    [DIG_DISPINFO_STOP_O]          = { 0xd8, 0x10},
    [DIG_DISPINFO_STOP_P]          = { 0xe8, 0x18},
    [DIG_DISPINFO_A_BUTTON_STOP]   = { 0xd0, 0x48},
    [DIG_DISPINFO_POKE_BALL_ROCKING] = { 0xd0, 0x08},
    [DIG_DISPINFO_WIN]             = { 0xd0, 0x40},
    [DIG_DISPINFO_LOSE]            = { 0xd0, 0x38},
    [DIG_DISPINFO_SMOKE_NW]        = { 0xc0, 0x58},
    [DIG_DISPINFO_SMOKE_NE]        = { 0xe0, 0x58},
    [DIG_DISPINFO_SMOKE_SW]        = { 0xc0, 0x78},
    [DIG_DISPINFO_SMOKE_SE]        = { 0xe0, 0x78},
    [DIG_DISPINFO_REEL]            = { 0x90, 0x38},
    [DIG_DISPINFO_TIME]            = {0x110, 0x58},
    [DIG_DISPINFO_NUMBER]          = { 0xa8, 0x70},
    [DIG_DISPINFO_DPAD]            = { 0xd0, 0x54},
    [DIG_DISPINFO_POKE_BALL_SHINING] = { 0xd0, 0x70},
    [DIG_DISPINFO_REG_R]           = { 0xbc, 0x34},
    [DIG_DISPINFO_REG_E]           = { 0xd0, 0x34},
    [DIG_DISPINFO_REG_G]           = { 0xe4, 0x34},
    [DIG_DISPINFO_REG_BONUS_B]     = { 0xb8, 0x48},
    [DIG_DISPINFO_REG_BONUS_O]     = { 0xc4, 0x48},
    [DIG_DISPINFO_REG_BONUS_N]     = { 0xd0, 0x48},
    [DIG_DISPINFO_REG_BONUS_U]     = { 0xdc, 0x48},
    [DIG_DISPINFO_REG_BONUS_S]     = { 0xe8, 0x48},
    [DIG_DISPINFO_BIG_B]           = { 0xbc, 0x34},
    [DIG_DISPINFO_BIG_I]           = { 0xd0, 0x34},
    [DIG_DISPINFO_BIG_G]           = { 0xe4, 0x34},
    [DIG_DISPINFO_BIG_BONUS_B]     = { 0xb8, 0x48},
    [DIG_DISPINFO_BIG_BONUS_O]     = { 0xc4, 0x48},
    [DIG_DISPINFO_BIG_BONUS_N]     = { 0xd0, 0x48},
    [DIG_DISPINFO_BIG_BONUS_U]     = { 0xdc, 0x48},
    [DIG_DISPINFO_BIG_BONUS_S]     = { 0xe8, 0x48},
    [DIG_DISPINFO_A_BUTTON_START]  = { 0x00, 0x00}
};

static const SpriteCallback sDigitalDisplay_SpriteCallbacks[] = {
    [DIG_DISPINFO_INSERT]            = SpriteCB_DigitalDisplay_Static,
    [DIG_DISPINFO_STOP_S]            = SpriteCB_DigitalDisplay_Stop,
    [DIG_DISPINFO_STOP_T]            = SpriteCB_DigitalDisplay_Stop,
    [DIG_DISPINFO_STOP_O]            = SpriteCB_DigitalDisplay_Stop,
    [DIG_DISPINFO_STOP_P]            = SpriteCB_DigitalDisplay_Stop,
    [DIG_DISPINFO_A_BUTTON_STOP]     = SpriteCB_DigitalDisplay_AButtonStop,
    [DIG_DISPINFO_POKE_BALL_ROCKING] = SpriteCB_DigitalDisplay_PokeballRocking,
    [DIG_DISPINFO_WIN]               = SpriteCB_DigitalDisplay_Static,
    [DIG_DISPINFO_LOSE]              = SpriteCB_DigitalDisplay_Static,
    [DIG_DISPINFO_SMOKE_NW]          = SpriteCB_DigitalDisplay_Smoke,
    [DIG_DISPINFO_SMOKE_NE]          = SpriteCB_DigitalDisplay_SmokeNE,
    [DIG_DISPINFO_SMOKE_SW]          = SpriteCB_DigitalDisplay_SmokeSW,
    [DIG_DISPINFO_SMOKE_SE]          = SpriteCB_DigitalDisplay_SmokeSE,
    [DIG_DISPINFO_REEL]              = SpriteCB_DigitalDisplay_Reel,
    [DIG_DISPINFO_TIME]              = SpriteCB_DigitalDisplay_Time,
    [DIG_DISPINFO_NUMBER]            = SpriteCB_DigitalDisplay_ReelTimeNumber,
    [DIG_DISPINFO_DPAD]              = SpriteCB_DigitalDisplay_Static,
    [DIG_DISPINFO_POKE_BALL_SHINING] = SpriteCB_DigitalDisplay_PokeballShining,
    [DIG_DISPINFO_REG_R]             = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_REG_E]             = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_REG_G]             = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_REG_BONUS_B]       = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_REG_BONUS_O]       = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_REG_BONUS_N]       = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_REG_BONUS_U]       = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_REG_BONUS_S]       = SpriteCB_DigitalDisplay_RegBonus,
    [DIG_DISPINFO_BIG_B]             = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_BIG_I]             = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_BIG_G]             = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_BIG_BONUS_B]       = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_BIG_BONUS_O]       = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_BIG_BONUS_N]       = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_BIG_BONUS_U]       = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_BIG_BONUS_S]       = SpriteCB_DigitalDisplay_BigBonus,
    [DIG_DISPINFO_A_BUTTON_START]    = SpriteCB_DigitalDisplay_AButtonStart
};

static const struct DigitalDisplaySprite sDigitalDisplay_InsertBet[] = {
    {DIG_SPRITE_EMPTY, DIG_DISPINFO_A_BUTTON_START, 0},
    {DIG_SPRITE_INSERT, DIG_DISPINFO_INSERT, 0},
    {DIG_SPRITE_D_PAD, DIG_DISPINFO_DPAD, 0},
    DIG_SPRITE_DUMMY
};

static const struct DigitalDisplaySprite sDigitalDisplay_StopReel[] = {
    {DIG_SPRITE_STOP_S, DIG_DISPINFO_STOP_S, 0},
    {DIG_SPRITE_STOP_T, DIG_DISPINFO_STOP_T, 0},
    {DIG_SPRITE_STOP_O, DIG_DISPINFO_STOP_O, 0},
    {DIG_SPRITE_STOP_P, DIG_DISPINFO_STOP_P, 0},
    {DIG_SPRITE_A_BUTTON, DIG_DISPINFO_A_BUTTON_STOP, 0},
    {DIG_SPRITE_POKE_BALL, DIG_DISPINFO_POKE_BALL_ROCKING, 0},
    DIG_SPRITE_DUMMY
};

static const struct DigitalDisplaySprite sDigitalDisplay_Win[] = {
    {DIG_SPRITE_WIN, DIG_DISPINFO_WIN, 0},
    {DIG_SPRITE_POKE_BALL, DIG_DISPINFO_POKE_BALL_SHINING, 0},
    DIG_SPRITE_DUMMY
};

static const struct DigitalDisplaySprite sDigitalDisplay_Lose[] = {
    {DIG_SPRITE_LOSE, DIG_DISPINFO_LOSE, 0},
    {DIG_SPRITE_SMOKE, DIG_DISPINFO_SMOKE_NW, 0},
    {DIG_SPRITE_SMOKE, DIG_DISPINFO_SMOKE_NE, 1},
    {DIG_SPRITE_SMOKE, DIG_DISPINFO_SMOKE_SW, 2},
    {DIG_SPRITE_SMOKE, DIG_DISPINFO_SMOKE_SE, 3},
    DIG_SPRITE_DUMMY
};

static const struct DigitalDisplaySprite sDigitalDisplay_ReelTime[] = {
    {DIG_SPRITE_REEL, DIG_DISPINFO_REEL, 0},
    {DIG_SPRITE_TIME, DIG_DISPINFO_TIME, 0},
    {DIG_SPRITE_NUMBER, DIG_DISPINFO_NUMBER, 0},
    DIG_SPRITE_DUMMY
};

static const struct DigitalDisplaySprite sDigitalDisplay_BonusBig[] = {
    {DIG_SPRITE_BIG_B, DIG_DISPINFO_BIG_B, 0},
    {DIG_SPRITE_BIG_I, DIG_DISPINFO_BIG_I, 1},
    {DIG_SPRITE_BIG_G, DIG_DISPINFO_BIG_G, 2},
    {DIG_SPRITE_BONUS_B, DIG_DISPINFO_BIG_BONUS_B, 3},
    {DIG_SPRITE_BONUS_O, DIG_DISPINFO_BIG_BONUS_O, 4},
    {DIG_SPRITE_BONUS_N, DIG_DISPINFO_BIG_BONUS_N, 5},
    {DIG_SPRITE_BONUS_U, DIG_DISPINFO_BIG_BONUS_U, 6},
    {DIG_SPRITE_BONUS_S, DIG_DISPINFO_BIG_BONUS_S, 7},
    {DIG_SPRITE_POKE_BALL, DIG_DISPINFO_POKE_BALL_SHINING, 0},
    DIG_SPRITE_DUMMY
};

static const struct DigitalDisplaySprite sDigitalDisplay_BonusRegular[] = {
    {DIG_SPRITE_REG_R, DIG_DISPINFO_REG_R, 0},
    {DIG_SPRITE_REG_E, DIG_DISPINFO_REG_E, 1},
    {DIG_SPRITE_REG_G, DIG_DISPINFO_REG_G, 2},
    {DIG_SPRITE_BONUS_B, DIG_DISPINFO_REG_BONUS_B, 3},
    {DIG_SPRITE_BONUS_O, DIG_DISPINFO_REG_BONUS_O, 4},
    {DIG_SPRITE_BONUS_N, DIG_DISPINFO_REG_BONUS_N, 5},
    {DIG_SPRITE_BONUS_U, DIG_DISPINFO_REG_BONUS_U, 6},
    {DIG_SPRITE_BONUS_S, DIG_DISPINFO_REG_BONUS_S, 7},
    {DIG_SPRITE_POKE_BALL, DIG_DISPINFO_POKE_BALL_SHINING, 0},
    DIG_SPRITE_DUMMY
};

static const struct DigitalDisplaySprite *const sDigitalDisplayScenes[] = {
    [DIG_DISPLAY_INSERT_BET] = sDigitalDisplay_InsertBet,
    [DIG_DISPLAY_STOP_REEL]  = sDigitalDisplay_StopReel,
    [DIG_DISPLAY_WIN]        = sDigitalDisplay_Win,
    [DIG_DISPLAY_LOSE]       = sDigitalDisplay_Lose,
    [DIG_DISPLAY_REEL_TIME]  = sDigitalDisplay_ReelTime,
    [DIG_DISPLAY_BONUS_REG]  = sDigitalDisplay_BonusRegular,
    [DIG_DISPLAY_BONUS_BIG]  = sDigitalDisplay_BonusBig
};

static void (*const sDigitalDisplaySceneExitCallbacks[])(void) = {
    [DIG_DISPLAY_INSERT_BET] = EndDigitalDisplayScene_InsertBet,
    [DIG_DISPLAY_STOP_REEL]  = EndDigitalDisplayScene_StopReel,
    [DIG_DISPLAY_WIN]        = EndDigitalDisplayScene_Win,
    [DIG_DISPLAY_LOSE]       = EndDigitalDisplayScene_Dummy,
    [DIG_DISPLAY_REEL_TIME]  = EndDigitalDisplayScene_Dummy,
    [DIG_DISPLAY_BONUS_REG]  = EndDigitalDisplayScene_Win,
    [DIG_DISPLAY_BONUS_BIG]  = EndDigitalDisplayScene_Win
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
    {gSlotMachineReelTimeNumber0, 0x80},
    {gSlotMachineReelTimeNumber1, 0x80},
    {gSlotMachineReelTimeNumber2, 0x80},
    {gSlotMachineReelTimeNumber3, 0x80},
    {gSlotMachineReelTimeNumber4, 0x80},
    {gSlotMachineReelTimeNumber5, 0x80}
};

static const struct SpriteFrameImage sImageTable_ReelTimeShadow[] = {
    {gSlotMachineReelTimeShadow, 0x200}
};

static const struct SpriteFrameImage sImageTable_ReelTimeNumberGap[] = {
    {gSlotMachineReelTimeNumberGap_Gfx, 0x40}
};

static const struct SpriteFrameImage sImageTable_ReelTimeBolt[] = {
    {gSlotMachineReelTimeBolt0, 0x100},
    {gSlotMachineReelTimeBolt1, 0x100}
};

static const struct SpriteFrameImage sImageTable_ReelTimePikachuAura[] = {
    {gSlotMachineReelTimePikaAura, 0x400}
};

static const struct SpriteFrameImage sImageTable_ReelTimeExplosion[] = {
    {gSlotMachineReelTimeExplosion0, 0x200},
    {gSlotMachineReelTimeExplosion1, 0x200}
};

static const struct SpriteFrameImage sImageTable_ReelTimeDuck[] = {
    {gSlotMachineReelTimeDuck, 0x20}
};

static const struct SpriteFrameImage sImageTable_ReelTimeSmoke[] = {
    {gSlotMachineReelTimeSmoke, 0x80}
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
    {gSlotMachinePikaPowerBolt, 0x20}
};

static const union AnimCmd sAnim_SingleFrame[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeDuck[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_ReelTimePikachu_Still[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimePikachu_ChargingSlow[] = {
    ANIMCMD_FRAME(1, 16),
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_ReelTimePikachu_ChargingMedium[] = {
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_ReelTimePikachu_ChargingFast[] = {
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_ReelTimePikachu_Cheering[] = {
    ANIMCMD_FRAME(2, 32),
    ANIMCMD_FRAME(3, 32),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_ReelTimePikachu_FellOver[] = {
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeNumber_0[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeNumber_1[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeNumber_2[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeNumber_3[] = {
    ANIMCMD_FRAME(3, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeNumber_4[] = {
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeNumber_5[] = {
    ANIMCMD_FRAME(5, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ReelTimeBolt[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_ReelTimeExplosion[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_FRAME(1, 16),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_DigitalDisplay_AButton_Flashing[] = {
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_DigitalDisplay_AButton_Static[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_DigitalDisplay_DPad_Flashing[] = {
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_DigitalDisplay_Pokeball_Rocking[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_FRAME(1, 16),
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_FRAME(1, 16, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_DigitalDisplay_Pokeball_Static[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_DigitalDisplay_Number_1[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_DigitalDisplay_Number_2[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_DigitalDisplay_Number_3[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_DigitalDisplay_Number_4[] = {
    ANIMCMD_FRAME(3, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_DigitalDisplay_Number_5[] = {
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_SingleFrame[] = {
    sAnim_SingleFrame
};

static const union AnimCmd *const sAnims_ReelTimeDuck[] = {
    sAnim_ReelTimeDuck
};

static const union AnimCmd *const sAnims_ReelTimePikachu[] = {
    sAnim_ReelTimePikachu_Still,
    sAnim_ReelTimePikachu_ChargingSlow,
    sAnim_ReelTimePikachu_ChargingMedium,
    sAnim_ReelTimePikachu_ChargingFast,
    sAnim_ReelTimePikachu_Cheering,
    sAnim_ReelTimePikachu_FellOver
};

static const union AnimCmd *const sAnims_ReelTimeNumbers[] = {
    sAnim_ReelTimeNumber_0,
    sAnim_ReelTimeNumber_1,
    sAnim_ReelTimeNumber_2,
    sAnim_ReelTimeNumber_3,
    sAnim_ReelTimeNumber_4,
    sAnim_ReelTimeNumber_5
};

static const union AnimCmd *const sAnims_ReelTimeBolt[] = {
    sAnim_ReelTimeBolt
};

static const union AnimCmd *const sAnims_ReelTimeExplosion[] = {
    sAnim_ReelTimeExplosion
};

static const union AnimCmd *const sAnims_DigitalDisplay_AButton[] = {
    sAnim_DigitalDisplay_AButton_Flashing,
    sAnim_DigitalDisplay_AButton_Static
};

static const union AnimCmd *const sAnims_DigitalDisplay_DPad[] = {
    sAnim_DigitalDisplay_DPad_Flashing
};

static const union AnimCmd *const sAnims_DigitalDisplay_Pokeball[] = {
    sAnim_DigitalDisplay_Pokeball_Rocking,
    sAnim_DigitalDisplay_Pokeball_Static
};

static const union AnimCmd *const sAnims_DigitalDisplay_Number[] = {
    sAnim_DigitalDisplay_Number_1,
    sAnim_DigitalDisplay_Number_2,
    sAnim_DigitalDisplay_Number_3,
    sAnim_DigitalDisplay_Number_4,
    sAnim_DigitalDisplay_Number_5
};

static const union AffineAnimCmd sAffineAnim_ReelTimeSmoke[] = {
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0x1, 0x1, 0, 1),
    AFFINEANIMCMD_LOOP(255),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_ReelTimeSmoke[] = {
    sAffineAnim_ReelTimeSmoke
};

static const union AffineAnimCmd sAffineAnim_PikaPowerBolt[] = {
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

static const union AffineAnimCmd *const sAffineAnims_PikaPowerBolt[] = {
    sAffineAnim_PikaPowerBolt
};

static const struct SpriteTemplate sSpriteTemplate_ReelSymbol = {
    GFXTAG_7_RED, PALTAG_REEL, &sOam_32x32, sAnims_SingleFrame, NULL, gDummySpriteAffineAnimTable, SpriteCB_ReelSymbol
};

static const struct SpriteTemplate sSpriteTemplate_CoinNumber = {
    GFXTAG_NUM_0, PALTAG_MISC, &sOam_8x16, sAnims_SingleFrame, NULL, gDummySpriteAffineAnimTable, SpriteCB_CoinNumber
};

static const struct SpriteTemplate sSpriteTemplate_ReelBackground = {
    GFXTAG_REEL_BG, PALTAG_REEL, &sOam_64x64, sAnims_SingleFrame, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimePikachu = {
    0xFFFF, PALTAG_REEL_TIME_PIKACHU, &sOam_64x64, sAnims_ReelTimePikachu, sImageTable_ReelTimePikachu, gDummySpriteAffineAnimTable, SpriteCB_ReelTimePikachu
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeMachineAntennae = {
    0xFFFF, PALTAG_REEL_TIME_MISC, &sOam_8x16, sAnims_SingleFrame, sImageTable_ReelTimeMachineAntennae, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeMachine = {
    0xFFFF, PALTAG_REEL_TIME_MACHINE, &sOam_8x16, sAnims_SingleFrame, sImageTable_ReelTimeMachine, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_BrokenReelTimeMachine = {
    0xFFFF, PALTAG_REEL_TIME_MACHINE, &sOam_8x16, sAnims_SingleFrame, sImageTable_BrokenReelTimeMachine, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeNumbers = {
    0xFFFF, PALTAG_MISC, &sOam_16x16, sAnims_ReelTimeNumbers, sImageTable_ReelTimeNumbers, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeNumbers
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeShadow = {
    0xFFFF, PALTAG_MISC, &sOam_16x16, sAnims_SingleFrame, sImageTable_ReelTimeShadow, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeNumberGap = {
    0xFFFF, PALTAG_MISC, &sOam_16x16, sAnims_SingleFrame, sImageTable_ReelTimeNumberGap, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeBolt = {
    0xFFFF, PALTAG_MISC, &sOam_16x32, sAnims_ReelTimeBolt, sImageTable_ReelTimeBolt, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeBolt
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimePikachuAura = {
    0xFFFF, PALTAG_PIKA_AURA, &sOam_32x64, sAnims_SingleFrame, sImageTable_ReelTimePikachuAura, gDummySpriteAffineAnimTable, SpriteCB_ReelTimePikachuAura
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeExplosion = {
    0xFFFF, PALTAG_EXPLOSION, &sOam_32x32, sAnims_ReelTimeExplosion, sImageTable_ReelTimeExplosion, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeExplosion
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeDuck = {
    0xFFFF, PALTAG_MISC, &sOam_8x8, sAnims_ReelTimeDuck, sImageTable_ReelTimeDuck, gDummySpriteAffineAnimTable, SpriteCB_ReelTimeDuck
};

static const struct SpriteTemplate sSpriteTemplate_ReelTimeSmoke = {
    0xFFFF, PALTAG_MISC, &sOam_16x16, sAnims_SingleFrame, sImageTable_ReelTimeSmoke, sAffineAnims_ReelTimeSmoke, SpriteCB_ReelTimeSmoke
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Reel = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Reel, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Time = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Time, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Insert = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Insert, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Stop = {
    GFXTAG_STOP, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Stop, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Win = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_64x32, sAnims_SingleFrame, sImageTable_DigitalDisplay_Win, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Lose = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_64x32, sAnims_SingleFrame, sImageTable_DigitalDisplay_Lose, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Bonus = {
    GFXTAG_BONUS, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Bonus, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Big = {
    GFXTAG_BIG, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Big, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Reg = {
    GFXTAG_REG, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Reg, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_AButton = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_32x32, sAnims_DigitalDisplay_AButton, sImageTable_DigitalDisplay_AButton, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Smoke = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_SingleFrame, sImageTable_DigitalDisplay_Smoke, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Number = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_16x16, sAnims_DigitalDisplay_Number, sImageTable_DigitalDisplay_Number, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_Pokeball = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_DigitalDisplay_Pokeball, sImageTable_DigitalDisplay_Pokeball, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_DigitalDisplay_DPad = {
    0xFFFF, PALTAG_DIG_DISPLAY, &sOam_8x8, sAnims_DigitalDisplay_DPad, sImageTable_DigitalDisplay_DPad, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_PikaPowerBolt = {
    0xFFFF, PALTAG_MISC, &sOam_8x8, sAnims_SingleFrame, sImageTable_PikaPowerBolt, sAffineAnims_PikaPowerBolt, SpriteCB_PikaPowerBolt
};

static const struct Subsprite sSubsprites_ReelBackground[] = {
    {-64, -64, ST_OAM_SQUARE, 3, 0x0, 3},
    {0, -64, ST_OAM_SQUARE, 3, 0x0, 3},
    {-64, 0, ST_OAM_SQUARE, 3, 0x0, 3},
    {0, 0, ST_OAM_SQUARE, 3, 0x0, 3}
};

static const struct SubspriteTable sSubspriteTable_ReelBackground[] = {
    {4, sSubsprites_ReelBackground}
};

static const struct Subsprite sSubsprites_ReelTimeMachineAntennae[] = {
    {-32, -12, ST_OAM_H_RECTANGLE, 1, 0x0, 1},
    {0, -12, ST_OAM_H_RECTANGLE, 1, 0x4, 1},
    {-32, -4, ST_OAM_H_RECTANGLE, 1, 0x8, 1},
    {0, -4, ST_OAM_H_RECTANGLE, 1, 0xc, 1},
    {-32, 4, ST_OAM_H_RECTANGLE, 1, 0x10, 1},
    {0, 4, ST_OAM_H_RECTANGLE, 1, 0x14, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeMachineAntennae[] = {
    {6, sSubsprites_ReelTimeMachineAntennae}
};

static const struct Subsprite sSubsprites_ReelTimeMachine[] = {
    {-32, -20, ST_OAM_H_RECTANGLE, 3, 0x0, 1},
    {-32, 12, ST_OAM_H_RECTANGLE, 1, 0x20, 1},
    {0, 12, ST_OAM_H_RECTANGLE, 1, 0x24, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeMachine[] = {
    {3, sSubsprites_ReelTimeMachine}
};

static const struct Subsprite sSubsprites_BrokenReelTimeMachine[] = {
    {-32, -24, ST_OAM_H_RECTANGLE, 3, 0x0, 1},
    {-32, 8, ST_OAM_H_RECTANGLE, 1, 0x20, 1},
    {0, 8, ST_OAM_H_RECTANGLE, 1, 0x24, 1},
    {-32, 16, ST_OAM_H_RECTANGLE, 1, 0x28, 1},
    {0, 16, ST_OAM_H_RECTANGLE, 1, 0x2c, 1}
};

static const struct SubspriteTable sSubspriteTable_BrokenReelTimeMachine[] = {
    {5, sSubsprites_BrokenReelTimeMachine}
};

static const struct Subsprite sSubsprites_ReelTimeShadow[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 1},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 1},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 1},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeShadow[] = {
    {4, sSubsprites_ReelTimeShadow}
};

static const struct Subsprite sSubsprites_ReelTimeNumberGap[] = {
    {-8, -12, ST_OAM_H_RECTANGLE, 0, 0x0, 1},
    {-8, -4, ST_OAM_H_RECTANGLE, 0, 0x0, 1},
    {-8, 4, ST_OAM_H_RECTANGLE, 0, 0x0, 1}
};

static const struct SubspriteTable sSubspriteTable_ReelTimeNumberGap[] = {
    {3, sSubsprites_ReelTimeNumberGap}
};

static const struct Subsprite sSubsprites_DigitalDisplay_Reel[] = {
    {-32, -24, ST_OAM_H_RECTANGLE, 3, 0x0, 3},
    {-32, 8, ST_OAM_H_RECTANGLE, 1, 0x20, 3},
    {0, 8, ST_OAM_H_RECTANGLE, 1, 0x24, 3},
    {-32, 16, ST_OAM_H_RECTANGLE, 1, 0x28, 3},
    {0, 16, ST_OAM_H_RECTANGLE, 1, 0x2c, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_Reel[] = {
    {5, sSubsprites_DigitalDisplay_Reel}
};

static const struct Subsprite sSubsprites_DigitalDisplay_Time[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_Time[] = {
    {4, sSubsprites_DigitalDisplay_Time}
};

static const struct Subsprite sSubsprites_DigitalDisplay_Insert[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_Insert[] = {
    {4, sSubsprites_DigitalDisplay_Insert}
};

static const struct Subsprite sSubsprites_DigitalDisplay_Unused1[] = {
    {-32, -8, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -8, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, 0, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, 0, ST_OAM_H_RECTANGLE, 1, 0xc, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_Unused1[] = {
    {4, sSubsprites_DigitalDisplay_Unused1}
};

static const struct Subsprite sSubsprites_DigitalDisplay_Win[] = {
    {-32, -12, ST_OAM_H_RECTANGLE, 1, 0x0, 3},
    {0, -12, ST_OAM_H_RECTANGLE, 1, 0x4, 3},
    {-32, -4, ST_OAM_H_RECTANGLE, 1, 0x8, 3},
    {0, -4, ST_OAM_H_RECTANGLE, 1, 0xc, 3},
    {-32, 4, ST_OAM_H_RECTANGLE, 1, 0x10, 3},
    {0, 4, ST_OAM_H_RECTANGLE, 1, 0x14, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_Win[] = {
    {6, sSubsprites_DigitalDisplay_Win}
};

static const struct Subsprite sSubsprites_DigitalDisplay_SmokeBig[] = {
    {-16, -16, ST_OAM_SQUARE, 2, 0x0, 3}
};

static const struct Subsprite sSubsprites_DigitalDisplay_SmokeSmall[] = {
    {-8, -8, ST_OAM_SQUARE, 1, 0x10, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_Smoke[] = {
    {1, sSubsprites_DigitalDisplay_SmokeBig},
    {1, sSubsprites_DigitalDisplay_SmokeSmall}
};

static const struct Subsprite sSubsprites_DigitalDisplay_Pokeball[] = {
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

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_Pokeball[] = {
    12, sSubsprites_DigitalDisplay_Pokeball
};

static const struct Subsprite sSubsprites_DigitalDisplay_DPad[] = {
    {-16, -12, ST_OAM_H_RECTANGLE, 2, 0x0, 3},
    {-16, 4, ST_OAM_H_RECTANGLE, 0, 0x8, 3},
    {0, 4, ST_OAM_H_RECTANGLE, 0, 0xa, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_DPad[] = {
    {3, sSubsprites_DigitalDisplay_DPad}
};

static const struct Subsprite sSubsprites_DigitalDisplay_StopS[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0x8, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_StopS[] = {
    {2, sSubsprites_DigitalDisplay_StopS}
};

static const struct Subsprite sSubsprites_DigitalDisplay_StopT[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x2, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xa, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_StopT[] = {
    {2, sSubsprites_DigitalDisplay_StopT}
};

static const struct Subsprite sSubsprites_DigitalDisplay_StopO[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x4, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xc, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_StopO[] = {
    {2, sSubsprites_DigitalDisplay_StopO}
};

static const struct Subsprite sSubsprites_DigitalDisplay_StopP[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x6, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xe, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_StopP[] = {
    {2, sSubsprites_DigitalDisplay_StopP}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BonusB[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0x8, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BonusB[] = {
    {2, sSubsprites_DigitalDisplay_BonusB}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BonusO[] = {
    {-4, -8, ST_OAM_SQUARE, 0, 0x2, 3},
    {-4, 0, ST_OAM_SQUARE, 0, 0xa, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BonusO[] = {
    {2, sSubsprites_DigitalDisplay_BonusO}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BonusN[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x3, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xb, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BonusN[] = {
    {2, sSubsprites_DigitalDisplay_BonusN}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BonusU[] = {
    {-4, -8, ST_OAM_SQUARE, 0, 0x5, 3},
    {-4, 0, ST_OAM_SQUARE, 0, 0xd, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BonusU[] = {
    {2, sSubsprites_DigitalDisplay_BonusU}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BonusS[] = {
    {-8, -8, ST_OAM_H_RECTANGLE, 0, 0x6, 3},
    {-8, 0, ST_OAM_H_RECTANGLE, 0, 0xe, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BonusS[] = {
    {2, sSubsprites_DigitalDisplay_BonusS}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BigB[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x2, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0x8, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xa, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x10, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x12, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BigB[] = {
    {6, sSubsprites_DigitalDisplay_BigB}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BigI[] = {
    {-8, -12, ST_OAM_H_RECTANGLE, 0, 0x3, 3},
    {-8, -4, ST_OAM_H_RECTANGLE, 0, 0xb, 3},
    {-8, 4, ST_OAM_H_RECTANGLE, 0, 0x13, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BigI[] = {
    {3, sSubsprites_DigitalDisplay_BigI}
};

static const struct Subsprite sSubsprites_DigitalDisplay_BigG[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x5, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x7, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0xd, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xf, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x15, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x17, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_BigG[] = {
    {6, sSubsprites_DigitalDisplay_BigG}
};

static const struct Subsprite sSubsprites_DigitalDisplay_RegR[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x0, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x2, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0x8, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xa, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x10, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x12, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_RegR[] = {
    {6, sSubsprites_DigitalDisplay_RegR}
};

static const struct Subsprite sSubsprites_DigitalDisplay_RegE[] = {
    {-8, -12, ST_OAM_H_RECTANGLE, 0, 0x3, 3},
    {-8, -4, ST_OAM_H_RECTANGLE, 0, 0xb, 3},
    {-8, 4, ST_OAM_H_RECTANGLE, 0, 0x13, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_RegE[] = {
    {3, sSubsprites_DigitalDisplay_RegE}
};

static const struct Subsprite sSubsprites_DigitalDisplay_RegG[] = {
    {-12, -12, ST_OAM_H_RECTANGLE, 0, 0x5, 3},
    {4, -12, ST_OAM_SQUARE, 0, 0x7, 3},
    {-12, -4, ST_OAM_H_RECTANGLE, 0, 0xd, 3},
    {4, -4, ST_OAM_SQUARE, 0, 0xf, 3},
    {-12, 4, ST_OAM_H_RECTANGLE, 0, 0x15, 3},
    {4, 4, ST_OAM_SQUARE, 0, 0x17, 3}
};

static const struct SubspriteTable sSubspriteTable_DigitalDisplay_RegG[] = {
    {6, sSubsprites_DigitalDisplay_RegG}
};

static const struct SpriteTemplate *const sSpriteTemplates_DigitalDisplay[NUM_DIG_DISPLAY_SPRITES] = {
    [DIG_SPRITE_REEL]      = &sSpriteTemplate_DigitalDisplay_Reel,
    [DIG_SPRITE_TIME]      = &sSpriteTemplate_DigitalDisplay_Time,
    [DIG_SPRITE_INSERT]    = &sSpriteTemplate_DigitalDisplay_Insert,
    [DIG_SPRITE_WIN]       = &sSpriteTemplate_DigitalDisplay_Win,
    [DIG_SPRITE_LOSE]      = &sSpriteTemplate_DigitalDisplay_Lose,
    [DIG_SPRITE_A_BUTTON]  = &sSpriteTemplate_DigitalDisplay_AButton,
    [DIG_SPRITE_SMOKE]     = &sSpriteTemplate_DigitalDisplay_Smoke,
    [DIG_SPRITE_NUMBER]    = &sSpriteTemplate_DigitalDisplay_Number,
    [DIG_SPRITE_POKE_BALL] = &sSpriteTemplate_DigitalDisplay_Pokeball,
    [DIG_SPRITE_D_PAD]     = &sSpriteTemplate_DigitalDisplay_DPad,
    [DIG_SPRITE_STOP_S]    = &sSpriteTemplate_DigitalDisplay_Stop,
    [DIG_SPRITE_STOP_T]    = &sSpriteTemplate_DigitalDisplay_Stop,
    [DIG_SPRITE_STOP_O]    = &sSpriteTemplate_DigitalDisplay_Stop,
    [DIG_SPRITE_STOP_P]    = &sSpriteTemplate_DigitalDisplay_Stop,
    [DIG_SPRITE_BONUS_B]   = &sSpriteTemplate_DigitalDisplay_Bonus,
    [DIG_SPRITE_BONUS_O]   = &sSpriteTemplate_DigitalDisplay_Bonus,
    [DIG_SPRITE_BONUS_N]   = &sSpriteTemplate_DigitalDisplay_Bonus,
    [DIG_SPRITE_BONUS_U]   = &sSpriteTemplate_DigitalDisplay_Bonus,
    [DIG_SPRITE_BONUS_S]   = &sSpriteTemplate_DigitalDisplay_Bonus,
    [DIG_SPRITE_BIG_B]     = &sSpriteTemplate_DigitalDisplay_Big,
    [DIG_SPRITE_BIG_I]     = &sSpriteTemplate_DigitalDisplay_Big,
    [DIG_SPRITE_BIG_G]     = &sSpriteTemplate_DigitalDisplay_Big,
    [DIG_SPRITE_REG_R]     = &sSpriteTemplate_DigitalDisplay_Reg,
    [DIG_SPRITE_REG_E]     = &sSpriteTemplate_DigitalDisplay_Reg,
    [DIG_SPRITE_REG_G]     = &sSpriteTemplate_DigitalDisplay_Reg,
    [DIG_SPRITE_EMPTY]     = &gDummySpriteTemplate
};

static const struct SubspriteTable *const sSubspriteTables_DigitalDisplay[NUM_DIG_DISPLAY_SPRITES] = {
    [DIG_SPRITE_REEL]      = sSubspriteTable_DigitalDisplay_Reel,
    [DIG_SPRITE_TIME]      = sSubspriteTable_DigitalDisplay_Time,
    [DIG_SPRITE_INSERT]    = sSubspriteTable_DigitalDisplay_Insert,
    [DIG_SPRITE_WIN]       = sSubspriteTable_DigitalDisplay_Win,
    [DIG_SPRITE_LOSE]      = NULL,
    [DIG_SPRITE_A_BUTTON]  = NULL,
    [DIG_SPRITE_SMOKE]     = sSubspriteTable_DigitalDisplay_Smoke,
    [DIG_SPRITE_NUMBER]    = NULL,
    [DIG_SPRITE_POKE_BALL] = sSubspriteTable_DigitalDisplay_Pokeball,
    [DIG_SPRITE_D_PAD]     = sSubspriteTable_DigitalDisplay_DPad,
    [DIG_SPRITE_STOP_S]    = sSubspriteTable_DigitalDisplay_StopS,
    [DIG_SPRITE_STOP_T]    = sSubspriteTable_DigitalDisplay_StopT,
    [DIG_SPRITE_STOP_O]    = sSubspriteTable_DigitalDisplay_StopO,
    [DIG_SPRITE_STOP_P]    = sSubspriteTable_DigitalDisplay_StopP,
    [DIG_SPRITE_BONUS_B]   = sSubspriteTable_DigitalDisplay_BonusB,
    [DIG_SPRITE_BONUS_O]   = sSubspriteTable_DigitalDisplay_BonusO,
    [DIG_SPRITE_BONUS_N]   = sSubspriteTable_DigitalDisplay_BonusN,
    [DIG_SPRITE_BONUS_U]   = sSubspriteTable_DigitalDisplay_BonusU,
    [DIG_SPRITE_BONUS_S]   = sSubspriteTable_DigitalDisplay_BonusS,
    [DIG_SPRITE_BIG_B]     = sSubspriteTable_DigitalDisplay_BigB,
    [DIG_SPRITE_BIG_I]     = sSubspriteTable_DigitalDisplay_BigI,
    [DIG_SPRITE_BIG_G]     = sSubspriteTable_DigitalDisplay_BigG,
    [DIG_SPRITE_REG_R]     = sSubspriteTable_DigitalDisplay_RegR,
    [DIG_SPRITE_REG_E]     = sSubspriteTable_DigitalDisplay_RegE,
    [DIG_SPRITE_REG_G]     = sSubspriteTable_DigitalDisplay_RegG,
    [DIG_SPRITE_EMPTY]     = NULL
};

static const struct SpriteSheet sSlotMachineSpriteSheets[] = {
    {gSlotMachineReelSymbol1Tiles, 0x200, GFXTAG_7_RED},
    {gSlotMachineReelSymbol2Tiles, 0x200, GFXTAG_7_BLUE},
    {gSlotMachineReelSymbol3Tiles, 0x200, GFXTAG_AZURILL},
    {gSlotMachineReelSymbol4Tiles, 0x200, GFXTAG_LOTAD},
    {gSlotMachineReelSymbol5Tiles, 0x200, GFXTAG_CHERRY},
    {gSlotMachineReelSymbol6Tiles, 0x200, GFXTAG_POWER},
    {gSlotMachineReelSymbol7Tiles, 0x200, GFXTAG_REPLAY},
    {gSlotMachineNumber0Tiles, 0x40, GFXTAG_NUM_0},
    {gSlotMachineNumber1Tiles, 0x40, GFXTAG_NUM_1},
    {gSlotMachineNumber2Tiles, 0x40, GFXTAG_NUM_2},
    {gSlotMachineNumber3Tiles, 0x40, GFXTAG_NUM_3},
    {gSlotMachineNumber4Tiles, 0x40, GFXTAG_NUM_4},
    {gSlotMachineNumber5Tiles, 0x40, GFXTAG_NUM_5},
    {gSlotMachineNumber6Tiles, 0x40, GFXTAG_NUM_6},
    {gSlotMachineNumber7Tiles, 0x40, GFXTAG_NUM_7},
    {gSlotMachineNumber8Tiles, 0x40, GFXTAG_NUM_8},
    {gSlotMachineNumber9Tiles, 0x40, GFXTAG_NUM_9},
    {eSlotMachineGfxBuffer + 0x0A00, 0x200, GFXTAG_STOP},
    {eSlotMachineGfxBuffer + 0x1400, 0x200, GFXTAG_BONUS},
    {eSlotMachineGfxBuffer + 0x1600, 0x300, GFXTAG_BIG},
    {eSlotMachineGfxBuffer + 0x1900, 0x300, GFXTAG_REG},
    {}
};

static const struct SpriteSheet sReelBackgroundSpriteSheet = {
    eSlotMachineGfxBuffer + 0x0000, 0x800, GFXTAG_REEL_BG
};

static const u8 *const sReelBackground_Tilemap = gSlotMachineReelBackground_Tilemap;

// Ruby and Sapphire use distinct match-line palette data.
#ifdef SAPPHIRE
static const u16 sLitMatchLines_Pal[] = INCBIN_U16("graphics/slot_machine/lit_match_lines_sapphire.gbapal");
#elif defined(RUBY)
static const u16 sLitMatchLines_Pal[] = INCBIN_U16("graphics/slot_machine/lit_match_lines_ruby.gbapal");
#endif // RS

static const u16 *const sLitMatchLinePalTable[] = {
    sLitMatchLines_Pal + 10,
    sLitMatchLines_Pal + 11,
    sLitMatchLines_Pal + 12,
    sLitMatchLines_Pal + 13,
    sLitMatchLines_Pal + 14
};

static const u16 *const sDarkMatchLinePalTable[] = {
    gSlotMachineMenu_Pal + 74,
    gSlotMachineMenu_Pal + 75,
    gSlotMachineMenu_Pal + 76,
    gSlotMachineMenu_Pal + 77,
    gSlotMachineMenu_Pal + 78
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

// Ruby and Sapphire use distinct flashing-light palette data.
#ifdef SAPPHIRE
static const u16 sFlashingLightsInside_Pal[] = INCBIN_U16("graphics/slot_machine/flashing_lights_inside_sapphire.gbapal");
static const u16 sFlashingLightsMiddle_Pal[] = INCBIN_U16("graphics/slot_machine/flashing_lights_middle_sapphire.gbapal");
static const u16 sFlashingLightsOutside_Pal[] = INCBIN_U16("graphics/slot_machine/flashing_lights_outside_sapphire.gbapal");
#elif defined (RUBY)
static const u16 sFlashingLightsInside_Pal[] = INCBIN_U16("graphics/slot_machine/flashing_lights_inside_ruby.gbapal");
static const u16 sFlashingLightsMiddle_Pal[] = INCBIN_U16("graphics/slot_machine/flashing_lights_middle_ruby.gbapal");
static const u16 sFlashingLightsOutside_Pal[] = INCBIN_U16("graphics/slot_machine/flashing_lights_outside_ruby.gbapal");
#endif // RS

static const u16 *const sFlashingLightsPalTable[] = {
    sFlashingLightsInside_Pal,
    sFlashingLightsMiddle_Pal,
    sFlashingLightsOutside_Pal
};

static const u16 *const sSlotMachineMenu_Pal = gSlotMachineMenu_Pal + 16;

static const u16 sPokeballShining0_Pal[] = INCBIN_U16("graphics/slot_machine/pokeball_shining_0.gbapal");
static const u16 sPokeballShining1_Pal[] = INCBIN_U16("graphics/slot_machine/pokeball_shining_1.gbapal");
static const u16 sPokeballShining2_Pal[] = INCBIN_U16("graphics/slot_machine/pokeball_shining_2.gbapal");

static const u16 *const sPokeballShiningPalTable[] = {
    sPokeballShining0_Pal,
    sPokeballShining1_Pal,
    sPokeballShining2_Pal,
    gSlotMachineDigitalDisplay_Pal
};

static const u16 *const sDigitalDisplay_Pal = gSlotMachineDigitalDisplay_Pal;

static const u16 sUnkPalette[] = INCBIN_U16("graphics/slot_machine/unknown_palette.bin");

static const struct SpritePalette sSlotMachineSpritePalettes[] = {
    {gSlotMachineReelSymbols_Pal, PALTAG_REEL},
    {gSlotMachineReelTimePikachu_Pal, PALTAG_REEL_TIME_PIKACHU},
    {gSlotMachineReelTimeMisc_Pal, PALTAG_REEL_TIME_MISC},
    {gSlotMachineReelTimeMachine_Pal, PALTAG_REEL_TIME_MACHINE},
    {gSlotMachineMisc_Pal, PALTAG_MISC},
    {gSlotMachineReelTimeExplosion_Pal, PALTAG_EXPLOSION},
    {gSlotMachineDigitalDisplay_Pal, PALTAG_DIG_DISPLAY},
    {gSlotMachineMisc_Pal, PALTAG_PIKA_AURA},
    {}
};

static const u8 sReelTimeGfx[] = INCBIN_U8("graphics/slot_machine/reel_time_gfx.4bpp.lz");

static const u16 sReelTimeWindow_Tilemap[] = INCBIN_U16("graphics/slot_machine/reel_time_window.bin");

#if DEBUG

static void DebugSlot_ToggleCherryBias(void)
{
    sDebugSlotForcedBias |= BIAS_CHERRY;
    sDebugSlotSelectedBias = (sDebugSlotSelectedBias == BIAS_CHERRY) ? 0 : BIAS_CHERRY;
}

static void DebugSlot_ToggleReplayBias(void)
{
    sDebugSlotForcedBias |= BIAS_REPLAY;
    sDebugSlotSelectedBias = (sDebugSlotSelectedBias == BIAS_REPLAY) ? 0 : BIAS_REPLAY;
}

static void DebugSlot_ToggleLotadBias(void)
{
    sDebugSlotForcedBias |= BIAS_LOTAD;
    sDebugSlotSelectedBias = (sDebugSlotSelectedBias == BIAS_LOTAD) ? 0 : BIAS_LOTAD;
}

static void DebugSlot_ToggleAzurillBias(void)
{
    sDebugSlotForcedBias |= BIAS_AZURILL;
    sDebugSlotSelectedBias = (sDebugSlotSelectedBias == BIAS_AZURILL) ? 0 : BIAS_AZURILL;
}

static void DebugSlot_TogglePowerBias(void)
{
    sDebugSlotForcedBias |= BIAS_POWER;
    sDebugSlotSelectedBias = (sDebugSlotSelectedBias == BIAS_POWER) ? 0 : BIAS_POWER;
}

static void DebugSlot_ToggleMixed7Bias(void)
{
    sDebugSlotForcedBias |= BIAS_MIXED_7;
    sDebugSlotSelectedBias = (sDebugSlotSelectedBias == BIAS_MIXED_7) ? 0 : BIAS_MIXED_7;
}

static void DebugSlot_ToggleStraight7Bias(void)
{
    sDebugSlotForcedBias |= BIAS_STRAIGHT_7;
    sDebugSlotSelectedBias = (sDebugSlotSelectedBias == BIAS_STRAIGHT_7) ? 0 : BIAS_STRAIGHT_7;
}

static void DebugSlot_ForceReelTimeBias(void)
{
    sDebugSlotForcedBias |= BIAS_REELTIME;
}

static void DebugSlot_PrintMachineSetting(void)
{
    u8 text[2];

    ConvertIntToDecimalStringN(text, sSlotMachine->machineId + 1, 2, 1);
    Menu_PrintText(text, 6, 1);
}

static const u8 sDebugSlotText_Setting[] = DTR("SETTEI", "SET");
static const u8 sDebugSlotText_Spins[] = DTR("MAWASITA", "TURNED");
static const u8 sDebugSlotText_Returned[] = DTR("MODOSI", "RETURN");
static const u8 sDebugSlotText_Consumed[] = DTR("NOMARE", "CONSUMED");
static const u8 sDebugSlotText_Before7[] = DTR("MAE　7", "BEFORE 7");
static const u8 sDebugSlotText_ChangeMachine[] = DTR("LR  HENKOU", "LR: CHANGE");
static const u8 sDebugSlotText_AutoSpin[] = DTR("START  JIDOUSU", "START: AUTO");
static const u8 sDebugSlotText_ForceBias[] = DTR("SELECT  SETTEI", "SELECT: FORCE");
// Irregular Romaji: 抽選 (ちゅうせん/chuusen)
static const u8 sDebugSlotText_Drawings[] = DTR("TYUHSEN", "DRAWINGS");
static const u8 sDebugSlotText_Cherry[] = _("CHERRY");
static const u8 sDebugSlotText_Replay[] = _("REPLAY");
static const u8 sDebugSlotText_Lotad[] = DTR("HASUBO", "LOTAD");
static const u8 sDebugSlotText_Azurill[] = DTR("RURIRI", "AZURILL");
static const u8 sDebugSlotText_Lightning[] = DTR("INAZU", "LIGHTNING");
static const u8 sDebugSlotText_Reg[] = _("REG");
static const u8 sDebugSlotText_Big[] = _("BIG");
static const u8 sDebugSlotText_ReelTime[] = DTR("BD", "REEL TIME");
static const u8 sDebugSlotText_Red7[] = _("R7");
static const u8 sDebugSlotText_Blue7[] = _("B7");
static const u8 sDebugSlotText_InsertCoin[] = DTR("A  COIN", "A: COIN");
static const u8 sDebugSlotText_SelectDrawing[] = DTR("TYUHSEN", "DRAWINGS");
static const u8 sDebugSlotText_Adjust100[] = _("UD  100");
static const u8 sDebugSlotText_Adjust1000[] = _("LR  1000");
static const u8 sDebugSlotText_SelectionMarker[] = _("×");

void DebugSlot_DrawStats(void)
{
    u8 text[5];

    Menu_PrintText(sDebugSlotText_Setting, 1, 1);
    Menu_PrintText(sDebugSlotText_Spins, 1, 3);
    Menu_PrintText(sDebugSlotText_Returned, 1, 5);
    Menu_PrintText(sDebugSlotText_Consumed, 1, 7);
    Menu_PrintText(sDebugSlotText_Before7, 1, 9);
    Menu_PrintText(sDebugSlotText_ChangeMachine, 1, 11);
    Menu_PrintText(sDebugSlotText_AutoSpin, 1, 13);
    Menu_PrintText(sDebugSlotText_ForceBias, 1, 15);
    Menu_PrintText(sDebugSlotText_InsertCoin, 1, 17);
    Menu_PrintText(sDebugSlotText_Drawings, 15, 1);
    Menu_PrintText(sDebugSlotText_Cherry, 15, 3);
    Menu_PrintText(sDebugSlotText_Replay, 15, 5);
    Menu_PrintText(sDebugSlotText_Lotad, 15, 7);
    Menu_PrintText(sDebugSlotText_Azurill, 15, 9);
    Menu_PrintText(sDebugSlotText_Lightning, 15, 11);
    Menu_PrintText(sDebugSlotText_Reg, 15, 13);
    Menu_PrintText(sDebugSlotText_Big, 15, 15);
    Menu_PrintText(sDebugSlotText_ReelTime, 15, 17);
    if (sSlotMachine->luckyGame == 0)
        Menu_PrintText(sDebugSlotText_Red7, 10, 9);
    else
        Menu_PrintText(sDebugSlotText_Blue7, 10, 9);

#define PRINT_NUMBER(n, x, y)                  \
    ConvertIntToDecimalStringN(text, n, 2, 4); \
    Menu_PrintText(text, x, y);

    PRINT_NUMBER(sSlotMachine->debugSpinCount, 10, 3);
    PRINT_NUMBER(sSlotMachine->debugPayoutTotal, 10, 5);
    PRINT_NUMBER(sSlotMachine->netCoinLoss, 10, 7);

#if DEBUG_FIX
#define OFFSET 24 // wider window
#else
#define OFFSET 20
#endif
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsCherry, OFFSET, 3);
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsReplay, OFFSET, 5);
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsLotad, OFFSET, 7);
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsAzurill, OFFSET, 9);
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsPower, OFFSET, 11);
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsMixed7, OFFSET, 13);
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsStraight7, OFFSET, 15);
    PRINT_NUMBER(sSlotMachine->debugBiasDrawsReelTime, OFFSET, 17);
#undef OFFSET
#undef PRINT_NUMBER

    if (sDebugSlotSelectedBias != 0)
    {
        u8 y = 0;

        switch (sDebugSlotSelectedBias)
        {
        case BIAS_CHERRY:
            y = 3;
            break;
        case BIAS_REPLAY:
            y = 5;
            break;
        case BIAS_LOTAD:
            y = 7;
            break;
        case BIAS_AZURILL:
            y = 9;
            break;
        case BIAS_POWER:
            y = 11;
            break;
        case BIAS_MIXED_7:
            y = 13;
            break;
        case BIAS_STRAIGHT_7:
            y = 15;
            break;
        }
        Menu_PrintText(sDebugSlotText_SelectionMarker, 23, y);
    }
    DebugSlot_PrintMachineSetting();
}

static void DebugSlot_AddCounterClamped(s32 *counter, s32 amount)
{
    *counter += amount;
    if (*counter > 9999)
        *counter = 9999;
}

static void DebugSlot_ResetCounters(void)
{
    sDebugSlotSelectedBias = 0;
    sDebugSlotForceBiasPending = 0;
    sDebugSlotForcedBias = 0;
    sDebugSlotAutoSpinEnabled = 0;
    sSlotMachine->debugSpinCount = 0;
    sSlotMachine->debugPayoutTotal = 0;
    sSlotMachine->debugBiasDrawsCherry = 0;
    sSlotMachine->debugBiasDrawsReplay = 0;
    sSlotMachine->debugBiasDrawsLotad = 0;
    sSlotMachine->debugBiasDrawsAzurill = 0;
    sSlotMachine->debugBiasDrawsPower = 0;
    sSlotMachine->debugBiasDrawsMixed7 = 0;
    sSlotMachine->debugBiasDrawsStraight7 = 0;
    sSlotMachine->debugBiasDrawsReelTime = 0;
    sSlotMachine->debugMatchedStraight7Count = 0;
}

static void DebugSlot_OpenMenu(void)
{
    CreateTask(DebugSlot_Task_Menu, 0);
}

static u8 DebugSlot_IsMenuClosed(void)
{
    if (FindTaskIdByFunc(DebugSlot_Task_Menu) == 0xFF)
        return 1;
    else
        return 0;
}

static const struct {const u8 *text; void (*func)();} sDebugSlotBiasChoices[] =
{
    {sDebugSlotText_Cherry, DebugSlot_ToggleCherryBias},
    {sDebugSlotText_Replay, DebugSlot_ToggleReplayBias},
    {sDebugSlotText_Lotad, DebugSlot_ToggleLotadBias},
    {sDebugSlotText_Azurill, DebugSlot_ToggleAzurillBias},
    {sDebugSlotText_Lightning, DebugSlot_TogglePowerBias},
    {sDebugSlotText_Reg, DebugSlot_ToggleMixed7Bias},
    {sDebugSlotText_Big, DebugSlot_ToggleStraight7Bias},
    {sDebugSlotText_ReelTime, DebugSlot_ForceReelTimeBias},
};

static void DebugSlot_Task_Menu(u8 taskId)
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

        DebugSlot_DrawStats();
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
            sSlotMachine->machineId--;
            if ((s8)sSlotMachine->machineId < 0)  // Why? It's unsigned
                sSlotMachine->machineId = 5;
            DebugSlot_PrintMachineSetting();
            break;
        }
        if (JOY_NEW(DPAD_RIGHT))
        {
            sSlotMachine->machineId++;
            if (sSlotMachine->machineId > 5)
                sSlotMachine->machineId = 0;
            DebugSlot_PrintMachineSetting();
            break;
        }
        if (JOY_NEW(A_BUTTON))
        {
            task->data[0] = 3;
            Menu_EraseScreen();
            Menu_DrawStdWindowFrame(0, 0, 9, 5);
            Menu_PrintText(sDebugSlotText_Adjust100, 1, 1);
            Menu_PrintText(sDebugSlotText_Adjust1000, 1, 3);
            break;
        }
        if (JOY_NEW(SELECT_BUTTON))
        {
            sDebugSlotForceBiasPending = 0;
            sDebugSlotForcedBias = 0;
            Menu_EraseScreen();
            Menu_DrawStdWindowFrame(0, 0, 10, 19);
            Menu_PrintText(sDebugSlotText_SelectDrawing, 1, 1);
            Menu_PrintItems(2, 3, 8, (void *)sDebugSlotBiasChoices);
            InitMenu(0, 1, 3, 8, 0, 9);
            task->data[0]++;
        }
        if (JOY_NEW(START_BUTTON))
        {
            sDebugSlotAutoSpinEnabled = 1;
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
            sDebugSlotForceBiasPending = 1;
            sDebugSlotBiasChoices[selection].func();
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

static const u8 sDebugSlotText_CountError[] = DTR("·カウントエラーがおきました", "Count error occured.");
static const u8 sDebugSlotText_ReelProcessingError[] = DTR("·リールそうさで　エラーが　おきました", "Reel processing error occurred.");
static const u8 sDebugSlotText_FlagOffError[] = DTR("·フラグオフエラーが　おきました", "FLAG OFF error occurred.");
static const u8 sDebugSlotText_BonusUseError[] = DTR("·ボーナスこやくの　エラーが　おきました", "BONUS use error occurred."); // TRN

static void DebugSlot_CheckErrors(void)
{
    if (sSlotMachine->matches & (MATCH_FLAG(MATCH_RED_7) | MATCH_FLAG(MATCH_BLUE_7)))
    {
        sSlotMachine->debugMatchedStraight7Count++;
        if (sSlotMachine->debugMatchedStraight7Count > 9999)
            sSlotMachine->debugMatchedStraight7Count = 9999;
        if (sSlotMachine->debugMatchedStraight7Count != sSlotMachine->debugBiasDrawsStraight7)
        {
            Menu_PrintText(sDebugSlotText_CountError, 4, 15);
            sDebugSlotAutoSpinEnabled = 0;
        }
        if (!(sSlotMachine->machineBias & BIAS_STRAIGHT_7))
        {
            Menu_PrintText(sDebugSlotText_FlagOffError, 4, 17);
            sDebugSlotAutoSpinEnabled = 0;
        }
    }
    else if (sSlotMachine->matches != 0)
    {
        if ((sSlotMachine->machineBias & BIAS_STRAIGHT_7) && !(sSlotMachine->matches & (MATCH_FLAG(MATCH_CHERRY) | MATCH_FLAG(MATCH_TOPBOT_CHERRY))))
        {
            Menu_PrintText(sDebugSlotText_BonusUseError, 4, 2);
            sDebugSlotAutoSpinEnabled = 0;
        }
    }
    if (sSlotMachine->matches == 0 && sSlotMachine->bet == MAX_BET && !(sSlotMachine->machineBias & BIAS_STRAIGHT_7))
    {
        u8 leftTopSymbol = GetSymbolAtRest(LEFT_REEL, 1);
        u8 leftMiddleSymbol = GetSymbolAtRest(LEFT_REEL, 2);
        u8 leftBottomSymbol = GetSymbolAtRest(LEFT_REEL, 3);

        u8 middleTopSymbol = GetSymbolAtRest(MIDDLE_REEL, 1);
        u8 middleMiddleSymbol = GetSymbolAtRest(MIDDLE_REEL, 2);
        u8 middleBottomSymbol = GetSymbolAtRest(MIDDLE_REEL, 3);

        u8 rightTopSymbol = GetSymbolAtRest(RIGHT_REEL, 1);
        u8 rightMiddleSymbol = GetSymbolAtRest(RIGHT_REEL, 2);
        u8 rightBottomSymbol = GetSymbolAtRest(RIGHT_REEL, 3);

        if ((leftTopSymbol == SYMBOL_7_RED && middleTopSymbol == SYMBOL_7_BLUE && rightTopSymbol == SYMBOL_7_RED)
         || (leftMiddleSymbol == SYMBOL_7_RED && middleMiddleSymbol == SYMBOL_7_BLUE && rightMiddleSymbol == SYMBOL_7_RED)
         || (leftBottomSymbol == SYMBOL_7_RED && middleBottomSymbol == SYMBOL_7_BLUE && rightBottomSymbol == SYMBOL_7_RED)
         || (leftTopSymbol == SYMBOL_7_RED && middleMiddleSymbol == SYMBOL_7_BLUE && rightBottomSymbol == SYMBOL_7_RED)
         || (leftBottomSymbol == SYMBOL_7_RED && middleMiddleSymbol == SYMBOL_7_BLUE && rightTopSymbol == SYMBOL_7_RED)
         || (leftTopSymbol == SYMBOL_7_BLUE && middleTopSymbol == SYMBOL_7_RED && rightTopSymbol == SYMBOL_7_BLUE)
         || (leftMiddleSymbol == SYMBOL_7_BLUE && middleMiddleSymbol == SYMBOL_7_RED && rightMiddleSymbol == SYMBOL_7_BLUE)
         || (leftBottomSymbol == SYMBOL_7_BLUE && middleBottomSymbol == SYMBOL_7_RED && rightBottomSymbol == SYMBOL_7_BLUE)
         || (leftTopSymbol == SYMBOL_7_BLUE && middleMiddleSymbol == SYMBOL_7_RED && rightBottomSymbol == SYMBOL_7_BLUE)
         || (leftBottomSymbol == SYMBOL_7_BLUE && middleMiddleSymbol == SYMBOL_7_RED && rightTopSymbol == SYMBOL_7_BLUE))
        {
            Menu_PrintText(sDebugSlotText_ReelProcessingError, 4, 0);
            sDebugSlotAutoSpinEnabled = 0;
        }
    }
}

#endif
