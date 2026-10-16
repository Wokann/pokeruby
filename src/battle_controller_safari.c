#include "global.h"
#include "item_menu.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_message.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_gfx_sfx_util.h"
#include "data.h"
#include "ewram.h"
#include "link.h"
#include "main.h"
#include "menu_cursor.h"
#include "palette.h"
#include "pokeball.h"
#include "pokeblock.h"
#include "sound.h"
#include "text.h"
#include "util.h"
#include "constants/songs.h"

extern struct Window gWindowTemplate_Contest_MoveDescription;
extern u8 gDisplayedStringBattle[];

extern const u8 gText_WhatWillPkmnDo2[];
extern const u8 gText_SafariZoneMenu[];

extern u16 gBattleTypeFlags;
extern u8 gBattlerInMenuId;
extern u8 gBattleOutcome;

extern u8 GetBattlerSide(u8 battler);
extern u8 GetBattlerAtPosition(u8 position);
extern u8 GetBattlerPosition(u8 battler);
extern u8 GetBattlerSpriteSubpriority();

#if ENGLISH
#define SUB_812BB10_TILE_DATA_OFFSET 440
#elif GERMAN
#define SUB_812BB10_TILE_DATA_OFFSET 444
#endif

// this file's functions
static void SafariHandleGetMonData(void);
static void SafariHandleGetRawMonData(void);
static void SafariHandleSetMonData(void);
static void SafariHandleSetRawMonData(void);
static void SafariHandleLoadMonSprite(void);
static void SafariHandleSwitchInAnim(void);
static void SafariHandleReturnMonToBall(void);
static void SafariHandleDrawTrainerPic(void);
static void SafariHandleTrainerSlide(void);
static void SafariHandleTrainerSlideBack(void);
static void SafariHandleFaintAnimation(void);
static void SafariHandlePaletteFade(void);
static void SafariHandleSuccessBallThrowAnim(void);
static void SafariHandleBallThrowAnim(void);
static void SafariHandlePause(void);
static void SafariHandleMoveAnimation(void);
static void SafariHandlePrintString(void);
static void SafariHandlePrintSelectionString(void);
static void SafariHandleChooseAction(void);
static void SafariHandleYesNoBox(void);
static void SafariHandleChooseMove(void);
static void SafariHandleChooseItem(void);
static void SafariHandleChoosePokemon(void);
static void SafariHandleCmd23(void);
static void SafariHandleHealthBarUpdate(void);
static void SafariHandleExpUpdate(void);
static void SafariHandleStatusIconUpdate(void);
static void SafariHandleStatusAnimation(void);
static void SafariHandleStatusXor(void);
static void SafariHandleDataTransfer(void);
static void SafariHandleDMA3Transfer(void);
static void SafariHandlePlayBGM(void);
static void SafariHandleCmd32(void);
static void SafariHandleTwoReturnValues(void);
static void SafariHandleChosenMonReturnValue(void);
static void SafariHandleOneReturnValue(void);
static void SafariHandleOneReturnValue_Duplicate(void);
static void SafariHandleClearUnkVar(void);
static void SafariHandleSetUnkVar(void);
static void SafariHandleClearUnkFlag(void);
static void SafariHandleToggleUnkFlag(void);
static void SafariHandleHitAnimation(void);
static void SafariHandleCantSwitch(void);
static void SafariHandlePlaySE(void);
static void SafariHandlecmd44(void);
static void SafariHandleFaintingCry(void);
static void SafariHandleIntroSlide(void);
static void SafariHandleIntroTrainerBallThrow(void);
static void SafariHandleDrawPartyStatusSummary(void);
static void SafariHandleHidePartyStatusSummary(void);
static void SafariHandleEndBounceEffect(void);
static void SafariHandleSpriteInvisibility(void);
static void SafariHandleBattleAnimation(void);
static void SafariHandleLinkStandbyMsg(void);
static void SafariHandleResetActionMoveSelection(void);
static void SafariHandleEndLinkBattle(void);
static void SafariCmdEnd(void);

// const data
typedef void (*BattleBufferCmd) (void);
static const BattleBufferCmd sSafariBufferCommands[] =
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
    SafariHandlecmd44,
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

static void SafariBufferExecCompleted(void);
static void CompleteOnSpecialAnimDone(void);
static void CompleteOnBattlerSpriteCallbackDummy(void);
static void SafariBufferRunCommand(void);
static void CompleteWhenChosePokeblock(void);

static void UNUSED SpriteCB_Null4(void)
{
}

void SetControllerToSafari(void)
{
    gBattlerControllerFuncs[gActiveBattler] = SafariBufferRunCommand;
}

static void SafariBufferRunCommand(void)
{
    if (gBattleControllerExecFlags & gBitTable[gActiveBattler])
    {
        if (gBattleBufferA[gActiveBattler][0] < 0x39)
            sSafariBufferCommands[gBattleBufferA[gActiveBattler][0]]();
        else
            SafariBufferExecCompleted();
    }
}

static void HandleInputChooseAction(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroyMenuCursor();

        // Useless switch statement.
        switch (gActionSelectionCursor[gActiveBattler])
        {
        case 0:
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, 5, 0);
            break;
        case 1:
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, 6, 0);
            break;
        case 2:
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, 7, 0);
            break;
        case 3:
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, 8, 0);
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
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        if (!(gActionSelectionCursor[gActiveBattler] & 1))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 1;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gActionSelectionCursor[gActiveBattler] & 2)
        {
            PlaySE(SE_SELECT);
            nullsub_8(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 2;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (!(gActionSelectionCursor[gActiveBattler] & 2))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 2;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
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
        SwapHpBarsWithHpText();
    }
#endif
}

static void CompleteOnBattlerSpriteCallbackDummy(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        SafariBufferExecCompleted();
}

static void CompleteOnInactiveTextPrinter(void)
{
    if (gWindowTemplate_Contest_MoveDescription.state == 0)
        SafariBufferExecCompleted();
}

static void SafariSetBattleEndCallbacks(void)
{
    if (!gPaletteFade.active)
    {
        gMain.inBattle = FALSE;
        gMain.callback1 = gPreBattleCallback1;
        SetMainCallback2(gMain.savedCallback);
    }
}

static void CompleteOnSpecialAnimDone(void)
{
    if (!gDoingBattleAnim || !gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        SafariBufferExecCompleted();
}

static void SafariOpenPokeblockCase(void)
{
    if (!gPaletteFade.active)
    {
        gBattlerControllerFuncs[gActiveBattler] = CompleteWhenChosePokeblock;
        OpenPokeblockCaseInBattle();
    }
}

static void CompleteWhenChosePokeblock(void)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        BtlController_EmitOneReturnValue(B_COMM_TO_ENGINE, gSpecialVar_ItemId);
        SafariBufferExecCompleted();
    }
}

static void CompleteOnFinishedBattleAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].animFromTableActive)
        SafariBufferExecCompleted();
}

static void SafariBufferExecCompleted(void)
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

static void SafariHandleGetMonData(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleGetRawMonData(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleSetMonData(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleSetRawMonData(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleLoadMonSprite(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleSwitchInAnim(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleReturnMonToBall(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleDrawTrainerPic(void)
{
    DecompressTrainerBackPic(gSaveBlock2.playerGender, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(gSaveBlock2.playerGender, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gMultiuseSpriteTemplate,
      80,
      (8 - gTrainerBackPicCoords[gSaveBlock2.playerGender].size) * 4 + 80,
      30);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = 240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = -2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBattlerSpriteCallbackDummy;
}

static void SafariHandleTrainerSlide(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleTrainerSlideBack(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleFaintAnimation(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandlePaletteFade(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleSuccessBallThrowAnim(void)
{
    gBattleAnimationInfo->ballThrowCaseId = 4;
    gDoingBattleAnim = TRUE;
    InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, GetBattlerAtPosition(1), 4);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnSpecialAnimDone;
}

static void SafariHandleBallThrowAnim(void)
{
    u8 ballThrowCaseId = gBattleBufferA[gActiveBattler][1];

    gBattleAnimationInfo->ballThrowCaseId = ballThrowCaseId;
    gDoingBattleAnim = TRUE;
    InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, GetBattlerAtPosition(1), 4);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnSpecialAnimDone;
}

static void SafariHandlePause(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleMoveAnimation(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandlePrintString(void)
{
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    BufferStringBattle(*(u16 *)&gBattleBufferA[gActiveBattler][2]);
    Contest_StartTextPrinter(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 144, 2, 15);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnInactiveTextPrinter;
}

static void SafariHandlePrintSelectionString(void)
{
    if (GetBattlerSide(gActiveBattler) == 0)
        SafariHandlePrintString();
    else
        SafariBufferExecCompleted();
}

static void SafariHandleChooseAction(void)
{
    s32 i;

    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 160;
    gWindowTemplate_Contest_MoveDescription.paletteNum = 0;
    Text_FillWindowRectDefPalette(&gWindowTemplate_Contest_MoveDescription, 10, 2, 15, 27, 18);
    Text_FillWindowRectDefPalette(&gWindowTemplate_Contest_MoveDescription, 10, 2, 35, 16, 36);
    gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseAction;

    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gText_SafariZoneMenu, 400, 18, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    MenuCursor_Create814A5C0(0, 0xFFFF, 12, 11679, 0);

    for (i = 0; i < 4; i++)
        nullsub_8(i);

    ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
    BattleStringExpandPlaceholdersToDisplayedString(gText_WhatWillPkmnDo2);

    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, SUB_812BB10_TILE_DATA_OFFSET, 2, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
}

static void SafariHandleYesNoBox(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleChooseMove(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleChooseItem(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gBattlerControllerFuncs[gActiveBattler] = SafariOpenPokeblockCase;
    gBattlerInMenuId = gActiveBattler;
}

static void SafariHandleChoosePokemon(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleCmd23(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleHealthBarUpdate(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleExpUpdate(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleStatusIconUpdate(void)
{
    UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 11);
    SafariBufferExecCompleted();
}

static void SafariHandleStatusAnimation(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleStatusXor(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleDataTransfer(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleDMA3Transfer(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandlePlayBGM(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleCmd32(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleTwoReturnValues(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleChosenMonReturnValue(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleOneReturnValue(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleOneReturnValue_Duplicate(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleClearUnkVar(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleSetUnkVar(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleClearUnkFlag(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleToggleUnkFlag(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleHitAnimation(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleCantSwitch(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandlePlaySE(void)
{
    s8 pan;

    if (GetBattlerSide(gActiveBattler) == 0)
        pan = -64;
    else
        pan = 63;
    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    SafariBufferExecCompleted();
}

static void SafariHandlecmd44(void)
{
    PlayFanfare(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    SafariBufferExecCompleted();
}

static void SafariHandleFaintingCry(void)
{
    u16 species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

    PlayCry_Normal(species, 25);
    SafariBufferExecCompleted();
}

static void SafariHandleIntroSlide(void)
{
    HandleIntroSlide(gBattleBufferA[gActiveBattler][1]);
    gIntroSlideFlags |= 1;
    SafariBufferExecCompleted();
}

static void SafariHandleIntroTrainerBallThrow(void)
{
    UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 10);
    StartHealthboxSlideIn(gActiveBattler);
    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
    SafariBufferExecCompleted();
}

static void SafariHandleDrawPartyStatusSummary(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleHidePartyStatusSummary(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleEndBounceEffect(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleSpriteInvisibility(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleBattleAnimation(void)
{
    u8 animationId = gBattleBufferA[gActiveBattler][1];
    u16 argument = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

    if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, animationId, argument) != 0)
        SafariBufferExecCompleted();
    else
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedBattleAnimation;
}

static void SafariHandleLinkStandbyMsg(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleResetActionMoveSelection(void)
{
    SafariBufferExecCompleted();
}

static void SafariHandleEndLinkBattle(void)
{
    gBattleOutcome = gBattleBufferA[gActiveBattler][1];
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    SafariBufferExecCompleted();
    if ((gBattleTypeFlags & BATTLE_TYPE_LINK) && !(gBattleTypeFlags & BATTLE_TYPE_IS_MASTER))
        gBattlerControllerFuncs[gActiveBattler] = SafariSetBattleEndCallbacks;
}

static void SafariCmdEnd(void)
{
}
