#include "global.h"
#include "battle_anim_81258BC.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_message.h"
#include "battle.h"
#include "data2.h"
#include "ewram.h"
#include "link.h"
#include "main.h"
#include "menu_cursor.h"
#include "palette.h"
#include "pokeball.h"
#include "sound.h"
#include "text.h"
#include "util.h"
#include "constants/songs.h"

extern struct Window gWindowTemplate_Contest_MoveDescription;
extern u8 gDisplayedStringBattle[];
extern u8 gActionSelectionCursor[];

extern const u8 BattleText_PlayerMenu[];
extern u8 gActiveBattler;
extern const u8 BattleText_MenuOptionsSafari[];

extern void *gBattlerControllerFuncs[];
extern u8 gBattleBufferA[][0x200];
extern bool8 gDoingBattleAnim;
extern u8 gBattlerSpriteIds[];
extern struct SpriteTemplate gCreatingSpriteTemplate;
extern u16 gBattleTypeFlags;
extern u32 gBattleControllerExecFlags;
extern u16 gSpecialVar_ItemId;
extern MainCallback gPreBattleCallback1;
extern u8 gBattlerInMenuId;
extern u8 gHealthboxSpriteIds[];
extern u16 gBattlerPartyIndexes[];
extern u16 gIntroSlideFlags;
extern u8 gBattleOutcome;

extern u8 GetBattlerSide(u8);
extern u8 GetBattlerAtPosition(u8);
extern u8 GetBattlerPosition(u8);
extern void DecompressTrainerBackPic();
extern u8 GetBattlerSpriteSubpriority();
extern void SpriteCB_TrainerSlideIn(struct Sprite *);
extern void OpenPokeblockCaseInBattle(void);
extern void HandleIntroSlide();
extern bool8 TryHandleLaunchBattleTableAnimation();

#if ENGLISH
#define SUB_812BB10_TILE_DATA_OFFSET 440
#elif GERMAN
#define SUB_812BB10_TILE_DATA_OFFSET 444
#endif

// this file's functions
void SafariHandleGetMonData(void);
void SafariHandleGetRawMonData(void);
void SafariHandleSetMonData(void);
void SafariHandleSetRawMonData(void);
void SafariHandleLoadMonSprite(void);
void SafariHandleSwitchInAnim(void);
void SafariHandleReturnMonToBall(void);
void SafariHandleDrawTrainerPic(void);
void SafariHandleTrainerSlide(void);
void SafariHandleTrainerSlideBack(void);
void SafariHandleFaintAnimation(void);
void SafariHandlePaletteFade(void);
void SafariHandleSuccessBallThrowAnim(void);
void SafariHandleBallThrowAnim(void);
void SafariHandlePause(void);
void SafariHandleMoveAnimation(void);
void SafariHandlePrintString(void);
void SafariHandlePrintSelectionString(void);
void SafariHandleChooseAction(void);
void SafariHandleYesNoBox(void);
void SafariHandleChooseMove(void);
void SafariHandleChooseItem(void);
void SafariHandleChoosePokemon(void);
void SafariHandleCmd23(void);
void SafariHandleHealthBarUpdate(void);
void SafariHandleExpUpdate(void);
void SafariHandleStatusIconUpdate(void);
void SafariHandleStatusAnimation(void);
void SafariHandleStatusXor(void);
void SafariHandleDataTransfer(void);
void SafariHandleDMA3Transfer(void);
void SafariHandlePlayBGM(void);
void SafariHandleCmd32(void);
void SafariHandleTwoReturnValues(void);
void SafariHandleChosenMonReturnValue(void);
void SafariHandleOneReturnValue(void);
void SafariHandleOneReturnValue_Duplicate(void);
void SafariHandleClearUnkVar(void);
void SafariHandleSetUnkVar(void);
void SafariHandleClearUnkFlag(void);
void SafariHandleToggleUnkFlag(void);
void SafariHandleHitAnimation(void);
void SafariHandleCantSwitch(void);
void SafariHandlePlaySE(void);
void SafariHandlePlayFanfareOrBGM(void);
void SafariHandleFaintingCry(void);
void SafariHandleIntroSlide(void);
void SafariHandleIntroTrainerBallThrow(void);
void SafariHandleDrawPartyStatusSummary(void);
void SafariHandleHidePartyStatusSummary(void);
void SafariHandleEndBounceEffect(void);
void SafariHandleSpriteInvisibility(void);
void SafariHandleBattleAnimation(void);
void SafariHandleLinkStandbyMsg(void);
void SafariHandleResetActionMoveSelection(void);
void SafariHandleEndLinkBattle(void);
void SafariCmdEnd(void);

// const data
typedef void (*BattleBufferCmd) (void);
const BattleBufferCmd gSafariBufferCommands[] =
{
    SafariHandleGetMonData,
    SafariHandleGetRawMonData,
    SafariHandleSetMonData,
    SafariHandleSetRawMonData,
    SafariHandleLoadMonSprite,
    SafariHandleSwitchInAnim,
    SafariHandleReturnMonToBall,
    SafariHandleDrawTrainerPic,
    SafariHandleTrainerSlide,
    SafariHandleTrainerSlideBack,
    SafariHandleFaintAnimation,
    SafariHandlePaletteFade,
    SafariHandleSuccessBallThrowAnim,
    SafariHandleBallThrowAnim,
    SafariHandlePause,
    SafariHandleMoveAnimation,
    SafariHandlePrintString,
    SafariHandlePrintSelectionString,
    SafariHandleChooseAction,
    SafariHandleYesNoBox,
    SafariHandleChooseMove,
    SafariHandleChooseItem,
    SafariHandleChoosePokemon,
    SafariHandleCmd23,
    SafariHandleHealthBarUpdate,
    SafariHandleExpUpdate,
    SafariHandleStatusIconUpdate,
    SafariHandleStatusAnimation,
    SafariHandleStatusXor,
    SafariHandleDataTransfer,
    SafariHandleDMA3Transfer,
    SafariHandlePlayBGM,
    SafariHandleCmd32,
    SafariHandleTwoReturnValues,
    SafariHandleChosenMonReturnValue,
    SafariHandleOneReturnValue,
    SafariHandleOneReturnValue_Duplicate,
    SafariHandleClearUnkVar,
    SafariHandleSetUnkVar,
    SafariHandleClearUnkFlag,
    SafariHandleToggleUnkFlag,
    SafariHandleHitAnimation,
    SafariHandleCantSwitch,
    SafariHandlePlaySE,
    SafariHandlePlayFanfareOrBGM,
    SafariHandleFaintingCry,
    SafariHandleIntroSlide,
    SafariHandleIntroTrainerBallThrow,
    SafariHandleDrawPartyStatusSummary,
    SafariHandleHidePartyStatusSummary,
    SafariHandleEndBounceEffect,
    SafariHandleSpriteInvisibility,
    SafariHandleBattleAnimation,
    SafariHandleLinkStandbyMsg,
    SafariHandleResetActionMoveSelection,
    SafariHandleEndLinkBattle,
    SafariCmdEnd,
};
// code

void SafariBufferExecCompleted(void);
void CompleteOnSpecialAnimDone(void);
static void CompleteOnBattlerSpriteCallbackDummy(void);
void SafariBufferRunCommand(void);
void CompleteWhenChosePokeblock(void);

void SpriteCB_Null4(void)
{
}

void SetControllerToSafari(void)
{
    gBattlerControllerFuncs[gActiveBattler] = SafariBufferRunCommand;
}

void SafariBufferRunCommand(void)
{
    if (gBattleControllerExecFlags & gBitTable[gActiveBattler])
    {
        if (gBattleBufferA[gActiveBattler][0] < 0x39)
            gSafariBufferCommands[gBattleBufferA[gActiveBattler][0]]();
        else
            SafariBufferExecCompleted();
    }
}

void HandleInputChooseAction(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroyMenuCursor();

        // Useless switch statement.
        switch (gActionSelectionCursor[gActiveBattler])
        {
        case 0:
            BtlController_EmitTwoReturnValues(1, 5, 0);
            break;
        case 1:
            BtlController_EmitTwoReturnValues(1, 6, 0);
            break;
        case 2:
            BtlController_EmitTwoReturnValues(1, 7, 0);
            break;
        case 3:
            BtlController_EmitTwoReturnValues(1, 8, 0);
            break;
        }
        SafariBufferExecCompleted();
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        if (gActionSelectionCursor[gActiveBattler] & 1)
        {
            PlaySE(SE_SELECT);
            nullsub_8(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 1;
            sub_802E3E4(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        if (!(gActionSelectionCursor[gActiveBattler] & 1))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 1;
            sub_802E3E4(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gActionSelectionCursor[gActiveBattler] & 2)
        {
            PlaySE(SE_SELECT);
            nullsub_8(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 2;
            sub_802E3E4(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (!(gActionSelectionCursor[gActiveBattler] & 2))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 2;
            sub_802E3E4(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
#if DEBUG
    else if (JOY_NEW(R_BUTTON))
    {
        if (!gBattleHealthBoxInfo[gActiveBattler].animFromTableActive)
            TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 4, 0);
    }
    else if (JOY_NEW(START_BUTTON))
    {
        sub_804454C();
    }
#endif
}

static void CompleteOnBattlerSpriteCallbackDummy(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        SafariBufferExecCompleted();
}

void CompleteOnBattleTextWindowIdle(void)
{
    if (gWindowTemplate_Contest_MoveDescription.state == 0)
        SafariBufferExecCompleted();
}

void SafariSetBattleEndCallbacks(void)
{
    if (!gPaletteFade.active)
    {
        gMain.inBattle = FALSE;
        gMain.callback1 = gPreBattleCallback1;
        SetMainCallback2(gMain.savedCallback);
    }
}

void CompleteOnSpecialAnimDone(void)
{
    if (!gDoingBattleAnim || !gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        SafariBufferExecCompleted();
}

void SafariOpenPokeblockCase(void)
{
    if (!gPaletteFade.active)
    {
        gBattlerControllerFuncs[gActiveBattler] = CompleteWhenChosePokeblock;
        OpenPokeblockCaseInBattle();
    }
}

void CompleteWhenChosePokeblock(void)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        BtlController_EmitOneReturnValue(1, gSpecialVar_ItemId);
        SafariBufferExecCompleted();
    }
}

static void CompleteOnFinishedBattleAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].animFromTableActive)
        SafariBufferExecCompleted();
}

void SafariBufferExecCompleted(void)
{
    gBattlerControllerFuncs[gActiveBattler] = SafariBufferRunCommand;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        u8 playerId = GetMultiplayerId();

        PrepareBufferDataTransferLink(2, 4, &playerId);
        gBattleBufferA[gActiveBattler][0] = 0x38;
    }
    else
    {
        gBattleControllerExecFlags &= ~gBitTable[gActiveBattler];
    }
}

static void UNUSED CompleteOnFinishedStatusAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].statusAnimActive)
        SafariBufferExecCompleted();
}

void SafariHandleGetMonData(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleGetRawMonData(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleSetMonData(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleSetRawMonData(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleLoadMonSprite(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleSwitchInAnim(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleReturnMonToBall(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleDrawTrainerPic(void)
{
    DecompressTrainerBackPic(gSaveBlock2.playerGender, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(gSaveBlock2.playerGender, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gCreatingSpriteTemplate,
      80,
      (8 - gTrainerBackPicCoords[gSaveBlock2.playerGender].coords) * 4 + 80,
      30);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = 240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = -2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBattlerSpriteCallbackDummy;
}

void SafariHandleTrainerSlide(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleTrainerSlideBack(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleFaintAnimation(void)
{
    SafariBufferExecCompleted();
}

void SafariHandlePaletteFade(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleSuccessBallThrowAnim(void)
{
    ewram17840.unk8 = 4;
    gDoingBattleAnim = 1;
    InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, GetBattlerAtPosition(1), 4);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnSpecialAnimDone;
}

void SafariHandleBallThrowAnim(void)
{
    u8 var = gBattleBufferA[gActiveBattler][1];

    ewram17840.unk8 = var;
    gDoingBattleAnim = 1;
    InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, GetBattlerAtPosition(1), 4);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnSpecialAnimDone;
}

// TODO: spell Pause correctly
void SafariHandlePause(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleMoveAnimation(void)
{
    SafariBufferExecCompleted();
}

void SafariHandlePrintString(void)
{
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    BufferStringBattle(*(u16 *)&gBattleBufferA[gActiveBattler][2]);
    Contest_StartTextPrinter(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 144, 2, 15);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBattleTextWindowIdle;
}

void SafariHandlePrintSelectionString(void)
{
    if (GetBattlerSide(gActiveBattler) == 0)
        SafariHandlePrintString();
    else
        SafariBufferExecCompleted();
}

void SafariHandleChooseAction(void)
{
    int i;

    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 160;
    gWindowTemplate_Contest_MoveDescription.paletteNum = 0;
    Text_FillWindowRectDefPalette(&gWindowTemplate_Contest_MoveDescription, 10, 2, 15, 27, 18);
    Text_FillWindowRectDefPalette(&gWindowTemplate_Contest_MoveDescription, 10, 2, 35, 16, 36);
    gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseAction;

    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_MenuOptionsSafari, 400, 18, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    MenuCursor_Create814A5C0(0, 0xFFFF, 12, 11679, 0);

    for (i = 0; i < 4; i++)
        nullsub_8(i);

    sub_802E3E4(gActionSelectionCursor[gActiveBattler], 0);
    BattleStringExpandPlaceholdersToDisplayedString(BattleText_PlayerMenu);

    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, SUB_812BB10_TILE_DATA_OFFSET, 2, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
}

void SafariHandleYesNoBox(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleChooseMove(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleChooseItem(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gBattlerControllerFuncs[gActiveBattler] = SafariOpenPokeblockCase;
    gBattlerInMenuId = gActiveBattler;
}

void SafariHandleChoosePokemon(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleCmd23(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleHealthBarUpdate(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleExpUpdate(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleStatusIconUpdate(void)
{
    UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 11);
    SafariBufferExecCompleted();
}

void SafariHandleStatusAnimation(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleStatusXor(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleDataTransfer(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleDMA3Transfer(void)
{
    SafariBufferExecCompleted();
}

void SafariHandlePlayBGM(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleCmd32(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleTwoReturnValues(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleChosenMonReturnValue(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleOneReturnValue(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleOneReturnValue_Duplicate(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleClearUnkVar(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleSetUnkVar(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleClearUnkFlag(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleToggleUnkFlag(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleHitAnimation(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleCantSwitch(void)
{
    SafariBufferExecCompleted();
}

void SafariHandlePlaySE(void)
{
    s8 pan;

    if (GetBattlerSide(gActiveBattler) == 0)
        pan = -64;
    else
        pan = 63;
    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    SafariBufferExecCompleted();
}

void SafariHandlePlayFanfareOrBGM(void)
{
    PlayFanfare(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    SafariBufferExecCompleted();
}

void SafariHandleFaintingCry(void)
{
    u16 species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

    PlayCry_Normal(species, 25);
    SafariBufferExecCompleted();
}

void SafariHandleIntroSlide(void)
{
    HandleIntroSlide(gBattleBufferA[gActiveBattler][1]);
    gIntroSlideFlags |= 1;
    SafariBufferExecCompleted();
}

void SafariHandleIntroTrainerBallThrow(void)
{
    UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 10);
    StartHealthboxSlideIn(gActiveBattler);
    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
    SafariBufferExecCompleted();
}

void SafariHandleDrawPartyStatusSummary(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleHidePartyStatusSummary(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleEndBounceEffect(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleSpriteInvisibility(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleBattleAnimation(void)
{
    u8 r3 = gBattleBufferA[gActiveBattler][1];
    u16 r4 = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

    if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, r3, r4) != 0)
        SafariBufferExecCompleted();
    else
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedBattleAnimation;
}

void SafariHandleLinkStandbyMsg(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleResetActionMoveSelection(void)
{
    SafariBufferExecCompleted();
}

void SafariHandleEndLinkBattle(void)
{
    gBattleOutcome = gBattleBufferA[gActiveBattler][1];
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    SafariBufferExecCompleted();
    if ((gBattleTypeFlags & BATTLE_TYPE_LINK) && !(gBattleTypeFlags & BATTLE_TYPE_WILD))
        gBattlerControllerFuncs[gActiveBattler] = SafariSetBattleEndCallbacks;
}

void SafariCmdEnd(void)
{
}
