#include "global.h"
#include "reshow_battle_screen.h"
#include "battle_anim_throw.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_message.h"
#include "battle.h"
#include "data2.h"
#include "ewram.h"
#include "item.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "menu_cursor.h"
#include "palette.h"
#include "party_menu.h"
#include "pokeball.h"
#include "pokemon.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "util.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/songs.h"

#if ENGLISH
#define PLAYER_ACTION_PROMPT_TILE_OFFSET 440
#elif GERMAN
#define PLAYER_ACTION_PROMPT_TILE_OFFSET 444
#endif

extern struct Window gWindowTemplate_Contest_MoveDescription;

extern void (*gBattlerControllerFuncs[])(void);

extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u32 gOamMatrixAllocBitmap;
extern u8 gIsDebugMode;
extern u8 gActiveBattler;
extern u8 gActionSelectionCursor[];
extern u8 gDisplayedStringBattle[];
extern u8 gMoveSelectionCursor[];
extern u8 gBattleBufferA[][0x200];
extern u8 gBattlerInMenuId;
extern u16 gBattlerPartyIndexes[];
extern u8 gHealthboxSpriteIds[];
extern u8 gDoingBattleAnim;
extern u8 gBattlerSpriteIds[];
extern u16 gBattleTypeFlags;
extern u8 gBattleOutcome;
extern void (*gAnimScriptCallback)(void);
extern bool8 gAnimScriptActive;
extern u16 gAnimMovePower;
extern s32 gAnimMoveDmg;
extern u8 gAnimFriendship;
extern u16 gWeatherMoveAnim;
extern u32 gTransformedPersonalities[];
extern u8 gBattleMonForms[];
extern u16 gIntroSlideFlags;
extern u8 gBattlerStatusSummaryTaskId[];
extern struct SpriteTemplate gCreatingSpriteTemplate;
extern u8 gAnimMoveTurn;
extern u8 gBattlePartyCurrentOrder[];
extern u8 gMultiUsePlayerCursor;
extern u8 gBattleControllerData[];

extern const u8 BattleText_OtherMenu[];
extern const u8 BattleText_MenuOptions[];
extern const u8 BattleText_PP[];

extern void MoveSelectionDisplayMoveNames(void);
extern void MoveSelectionDisplayPPNumber();
extern void MoveSelectionDisplayMoveType();
extern void OpenBagAndChooseItem(void);
extern void OpenPartyMenuToChooseMon(void);
extern void BattleStopLowHpSound(void);
extern void PlayerBufferExecCompleted(void);
extern void CompleteOnHealthbarDone(void);
extern void BattleControllerDummy(void);
extern void Task_GiveExpToMon(u8);
extern void CompleteOnFinishedStatusAnimation(void);
extern void DoHitAnimBlinkSpriteEffect(void);
extern void DoHitAnimHealthboxEffect();
extern u8 GetBattlerSide(u8);
extern void HandleIntroSlide();
extern void SetSpritePrimaryCoordsFromSecondaryCoords();
extern void StartAnimLinearTranslation(struct Sprite *);
extern void StoreSpriteCallbackInData6();
extern void BattleLoadPlayerMonSpriteGfx(struct Pokemon *, u8);
extern bool8 IsDoubleBattle(void);
extern void Intro_TryShinyAnimShowHealthbox(void);
extern bool8 IsBattlerSpritePresent(u8);
extern bool8 TryHandleLaunchBattleTableAnimation();
extern void CompleteOnFinishedBattleAnimation(void);
extern void PrintLinkStandbyMsg(void);
extern void SetBattleEndCallbacks(void);
extern void CompleteOnInactiveTextPrinter2(void);
extern void SetBattlerSpriteAffineMode();
extern void CopyAllBattleSpritesInvisibilities(void);
extern void TrySetBehindSubstituteSpriteBit();
extern u8 IsMoveWithoutAnimation();
static void CompleteOnSpecialAnimDone(void);
extern u8 GetBattlerAtPosition(u8);
extern void FreeMonSpriteAfterFaintAnim(void);
extern void SpriteCB_FaintSlideAnim(struct Sprite *);
extern void FreeTrainerSpriteAfterSlide(void);
extern void CompleteOnBankSpriteCallbackDummy2(void);
extern u8 GetBattlerPosition(u8);
extern void DecompressTrainerBackPic();
extern void SpriteCB_TrainerSlideIn(struct Sprite *);
extern void CompleteOnBattlerSpriteCallbackDummy(void);
extern u8 GetBattlerSpriteSubpriority();
extern void FreeMonSpriteAfterSwitchOutAnim(void);
extern void SpriteCB_WaitForBattlerBallReleaseAnim(struct Sprite *);
extern u8 GetBattlerSpriteCoord();
extern u8 GetBattlerSpriteDefault_Y();
extern void SwitchIn_TryShinyAnimShowHealthbox(void);
extern void CompleteOnBankSpritePosX_0(void);
extern u8 gActiveBattler;
extern void (*gBattlerControllerFuncs[])(void);
extern bool8 gDoingBattleAnim;
extern u16 gBattleTypeFlags;
extern u32 gBattleControllerExecFlags;
extern u8 gBattleBufferA[][0x200];
extern u8 gBattlerSpriteIds[];
extern u8 gActionSelectionCursor[];
extern u8 gMoveSelectionCursor[];
extern u8 gAbsentBattlerFlags;
extern u8 gMultiUsePlayerCursor;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern const u8 BattleText_SwitchWhich[];
extern u8 gNumberOfMovesToChoose;
extern struct BattlePokemon gBattleMons[];
extern MainCallback gPreBattleCallback1;
extern u8 gHealthboxSpriteIds[];
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u8 gBattleControllerData[];
extern u8 gPartyMenuActionResult;
extern u8 gBattlePartyCurrentOrder[];
extern u16 gSpecialVar_ItemId;
extern u8 gDisplayedStringBattle[];
extern const u8 BattleText_LinkStandby[];

extern void DoBounceEffect(u8, u8, s8, s8);
extern u8 GetBattlerPosition(u8);
extern u8 GetBattlerAtPosition(u8);
extern void EndBounceEffect(u8, u8);
extern void SpriteCB_ShowAsMoveTarget(struct Sprite *);
extern void SpriteCB_HideAsMoveTarget(struct Sprite *);
extern bool8 IsDoubleBattle();
extern void CB2_BagMenuFromBattle(void);

void PlayerHandleGetMonData(void);
void PlayerHandleGetRawMonData(void);
void PlayerHandleSetMonData(void);
void PlayerHandleSetRawMonData(void);
void PlayerHandleLoadMonSprite(void);
void PlayerHandleSwitchInAnim(void);
void PlayerHandleReturnMonToBall(void);
void PlayerHandleDrawTrainerPic(void);
void PlayerHandleTrainerSlide(void);
void PlayerHandleTrainerSlideBack(void);
void PlayerHandleFaintAnimation(void);
void PlayerHandlePaletteFade(void);
void PlayerHandleSuccessBallThrowAnim(void);
void PlayerHandleBallThrowAnim(void);
void PlayerHandlePause(void);
void PlayerHandleMoveAnimation(void);
void PlayerHandlePrintString(void);
void PlayerHandlePrintSelectionString(void);
void PlayerHandleChooseAction(void);
void PlayerHandleYesNoBox(void);
void PlayerHandleChooseMove(void);
void PlayerHandleChooseItem(void);
void PlayerHandleChoosePokemon(void);
void PlayerHandleCmd23(void);
void PlayerHandleHealthBarUpdate(void);
void PlayerHandleExpUpdate(void);
void PlayerHandleStatusIconUpdate(void);
void PlayerHandleStatusAnimation(void);
void PlayerHandleStatusXor(void);
void PlayerHandleDataTransfer(void);
void PlayerHandleDMA3Transfer(void);
void PlayerHandlePlayBGM(void);
void PlayerHandleCmd32(void);
void PlayerHandleTwoReturnValues(void);
void PlayerHandleChosenMonReturnValue(void);
void PlayerHandleOneReturnValue(void);
void PlayerHandleOneReturnValue_Duplicate(void);
void PlayerHandleClearUnkVar(void);
void PlayerHandleSetUnkVar(void);
void PlayerHandleClearUnkFlag(void);
void PlayerHandleToggleUnkFlag(void);
void PlayerHandleHitAnimation(void);
void PlayerHandleCantSwitch(void);
void PlayerHandlePlaySE(void);
void PlayerHandlePlayFanfareOrBGM(void);
void PlayerHandleFaintingCry(void);
void PlayerHandleIntroSlide(void);
void PlayerHandleIntroTrainerBallThrow(void);
void PlayerHandleDrawPartyStatusSummary(void);
void PlayerHandleHidePartyStatusSummary(void);
void PlayerHandleEndBounceEffect(void);
void PlayerHandleSpriteInvisibility(void);
void PlayerHandleBattleAnimation(void);
void PlayerHandleLinkStandbyMsg(void);
void PlayerHandleResetActionMoveSelection(void);
void PlayerHandleEndLinkBattle(void);
void PlayerCmdEnd(void);

static const u8 sText_DebugMoveTestTurn[] = _("ターン");

static void (*const sPlayerBufferCommands[])(void) =
{
    PlayerHandleGetMonData,
    PlayerHandleGetRawMonData,
    PlayerHandleSetMonData,
    PlayerHandleSetRawMonData,
    PlayerHandleLoadMonSprite,
    PlayerHandleSwitchInAnim,
    PlayerHandleReturnMonToBall,
    PlayerHandleDrawTrainerPic,
    PlayerHandleTrainerSlide,
    PlayerHandleTrainerSlideBack,
    PlayerHandleFaintAnimation,
    PlayerHandlePaletteFade,
    PlayerHandleSuccessBallThrowAnim,
    PlayerHandleBallThrowAnim,
    PlayerHandlePause,
    PlayerHandleMoveAnimation,
    PlayerHandlePrintString,
    PlayerHandlePrintSelectionString,
    PlayerHandleChooseAction,
    PlayerHandleYesNoBox,
    PlayerHandleChooseMove,
    PlayerHandleChooseItem,
    PlayerHandleChoosePokemon,
    PlayerHandleCmd23,
    PlayerHandleHealthBarUpdate,
    PlayerHandleExpUpdate,
    PlayerHandleStatusIconUpdate,
    PlayerHandleStatusAnimation,
    PlayerHandleStatusXor,
    PlayerHandleDataTransfer,
    PlayerHandleDMA3Transfer,
    PlayerHandlePlayBGM,
    PlayerHandleCmd32,
    PlayerHandleTwoReturnValues,
    PlayerHandleChosenMonReturnValue,
    PlayerHandleOneReturnValue,
    PlayerHandleOneReturnValue_Duplicate,
    PlayerHandleClearUnkVar,
    PlayerHandleSetUnkVar,
    PlayerHandleClearUnkFlag,
    PlayerHandleToggleUnkFlag,
    PlayerHandleHitAnimation,
    PlayerHandleCantSwitch,
    PlayerHandlePlaySE,
    PlayerHandlePlayFanfareOrBGM,
    PlayerHandleFaintingCry,
    PlayerHandleIntroSlide,
    PlayerHandleIntroTrainerBallThrow,
    PlayerHandleDrawPartyStatusSummary,
    PlayerHandleHidePartyStatusSummary,
    PlayerHandleEndBounceEffect,
    PlayerHandleSpriteInvisibility,
    PlayerHandleBattleAnimation,
    PlayerHandleLinkStandbyMsg,
    PlayerHandleResetActionMoveSelection,
    PlayerHandleEndLinkBattle,
    PlayerCmdEnd,
};

void PlayerBufferRunCommand(void);
void HandleInputChooseTarget(void);
static void HandleInputChooseMove(void);
void HandleMoveSwitching(void);
void SwitchIn_HandleSoundAndEnd(void);
void Task_PrepareToGiveExpWithExpBar(u8);
void Task_GiveExpWithExpBar(u8);
void Task_LaunchLvlUpAnim(u8);
void Task_UpdateLvlInHealthbox(u8);
void DestroyExpTaskAndCompleteOnInactiveTextPrinter(u8);
void WaitForMonSelection(void);
void CompleteWhenChoseItem(void);
void MoveSelectionDisplayMoveName(s32 cursorPosition, const u8 *format);
void MoveSelectionDisplayMoveNames(void);
void MoveSelectionDisplayPPNumber(void);
void MoveSelectionDisplayMoveType(void);
void MoveSelectionCreateCursorAt(u8 cursorPosition, int baseTileNum);
void MoveSelectionDestroyCursorAt(u8 cursorPosition);
void PrintLinkStandbyMsg(void);
u32 CopyPlayerMonData(u8, u8 *);
void SetPlayerMonData(u8);
void StartSendOutAnim(u8, u8);
void DoSwitchOutAnimation(void);
void PlayerDoMoveAnimation(void);
void SpriteCB_FreePlayerSpriteLoadMonSprite(struct Sprite *);
void Task_StartSendOutAnim(u8);
void EndDrawPartyStatusSummary(void);

void BattleControllerDummy(void)
{
}

void SetControllerToPlayer(void)
{
    gBattlerControllerFuncs[gActiveBattler] = PlayerBufferRunCommand;
    gDoingBattleAnim = FALSE;
}

void PlayerBufferExecCompleted(void)
{
    gBattlerControllerFuncs[gActiveBattler] = PlayerBufferRunCommand;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        u8 playerId = GetMultiplayerId();

        PrepareBufferDataTransferLink(B_COMM_CONTROLLER_IS_DONE, 4, &playerId);
        gBattleBufferA[gActiveBattler][0] = CONTROLLER_TERMINATOR_NOP;
    }
    else
    {
        gBattleControllerExecFlags &= ~gBitTable[gActiveBattler];
    }
}

void PlayerBufferRunCommand(void)
{
    if (gBattleControllerExecFlags & gBitTable[gActiveBattler])
    {
        if (gBattleBufferA[gActiveBattler][0] < 0x39)
            sPlayerBufferCommands[gBattleBufferA[gActiveBattler][0]]();
        else
            PlayerBufferExecCompleted();
    }
}

void CompleteOnBankSpritePosX_0(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].x2 == 0)
        PlayerBufferExecCompleted();
}

void HandleInputChooseAction(void)
{
    u16 itemId = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

    DoBounceEffect(gActiveBattler, 1, 7, 1);
    DoBounceEffect(gActiveBattler, 0, 7, 1);
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        DestroyMenuCursor();

        // Useless switch statement.
        switch (gActionSelectionCursor[gActiveBattler])
        {
        case 0:
            BtlController_EmitTwoReturnValues(1, 0, 0);
            break;
        case 1:
            BtlController_EmitTwoReturnValues(1, 1, 0);
            break;
        case 2:
            BtlController_EmitTwoReturnValues(1, 2, 0);
            break;
        case 3:
            BtlController_EmitTwoReturnValues(1, 3, 0);
            break;
        }
        PlayerBufferExecCompleted();
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        if (gActionSelectionCursor[gActiveBattler] & 1)
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 1;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        if (!(gActionSelectionCursor[gActiveBattler] & 1))
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 1;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gActionSelectionCursor[gActiveBattler] & 2)
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 2;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (!(gActionSelectionCursor[gActiveBattler] & 2))
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[gActiveBattler]);
            gActionSelectionCursor[gActiveBattler] ^= 2;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
         && GetBattlerPosition(gActiveBattler) == 2
         && !(gAbsentBattlerFlags & gBitTable[GetBattlerAtPosition(0)])
         && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            if (gBattleBufferA[gActiveBattler][1] == 1)
            {
                // Add item to bag if it is monId ball
                if (itemId <= ITEM_PREMIER_BALL)
                    AddBagItem(itemId, 1);
                else
                    return;
            }
            PlaySE(SE_SELECT);
            BtlController_EmitTwoReturnValues(1, 12, 0);
            PlayerBufferExecCompleted();
            DestroyMenuCursor();
        }
    }
    else if (JOY_NEW(START_BUTTON))
    {
        SwapHpBarsWithHpText();
    }
}

void UnusedEndBounceEffect(void)
{
    EndBounceEffect(gActiveBattler, 1);
    EndBounceEffect(gActiveBattler, 0);
    gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseTarget;
}

// TODO: fix this function
void HandleInputChooseTarget(void)
{
    u8 identities[4] = {0, 2, 3, 1};
    s32 i;

    DoBounceEffect(gMultiUsePlayerCursor, 1, 15, 1);
    i = 0;
    if (gBattlersCount != 0)
    {
        do
        {
            if (i != gMultiUsePlayerCursor)
                EndBounceEffect(i, 1);
            i++;
        } while (i < gBattlersCount);
    }
    if (JOY_NEW(A_BUTTON))
    {
        DestroyMenuCursor();
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;
        BtlController_EmitTwoReturnValues(1, 10, gMoveSelectionCursor[gActiveBattler] | (gMultiUsePlayerCursor << 8));
        EndBounceEffect(gMultiUsePlayerCursor, 1);
        PlayerBufferExecCompleted();
    }
    //_0802C3A8
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;
        gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseMove;
        DoBounceEffect(gActiveBattler, 1, 7, 1);
        DoBounceEffect(gActiveBattler, 0, 7, 1);
        EndBounceEffect(gMultiUsePlayerCursor, 1);
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;
        do
        {
            u8 currSelIdentity = GetBattlerPosition(gMultiUsePlayerCursor);

            for (i = 0; i < 4; i++)
            {
                if (currSelIdentity == identities[i])
                    break;
            }
            do
            {
                i--;
                if (i < 0)
                    i = 3;
                gMultiUsePlayerCursor = GetBattlerAtPosition(identities[i]);
            } while(gMultiUsePlayerCursor == gBattlersCount);
            i = 0;
            switch (GetBattlerPosition(gMultiUsePlayerCursor))
            {
            case 0:
            case 2:
                if (gActiveBattler == gMultiUsePlayerCursor)
                {
                    u32 moveId;

                    asm("":::"memory");
                    moveId = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1 + gMoveSelectionCursor[gActiveBattler]);
                    if (!(gBattleMoves[moveId].target & 2))
                        break;
                }
                i++;
                break;
            case 1:
            case 3:
                i++;
            }
            //_0802C500
            if (gAbsentBattlerFlags & gBitTable[gMultiUsePlayerCursor])
                i = 0;
        } while (i == 0);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_ShowAsMoveTarget;
    }
    //_0802C540
    else if (JOY_NEW(DPAD_RIGHT | DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;
        do
        {
            u8 currSelIdentity = GetBattlerPosition(gMultiUsePlayerCursor);

            for (i = 0; i < 4; i++)
            {
                if (currSelIdentity == identities[i])
                    break;
            }
            do
            {
                i++;
                if (i > 3)
                    i = 0;
                gMultiUsePlayerCursor = GetBattlerAtPosition(identities[i]);
            } while (gMultiUsePlayerCursor == gBattlersCount);
            i = 0;
            switch (GetBattlerPosition(gMultiUsePlayerCursor))
            {
            case 0:
            case 2:
                if (gActiveBattler == gMultiUsePlayerCursor)
                {
                    u32 moveId;

                    asm("":::"memory");
                    moveId = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1 + gMoveSelectionCursor[gActiveBattler]);
                    if (!(gBattleMoves[moveId].target & 2))
                        break;
                }
                i++;
                break;
            case 1:
            case 3:
                i++;
            }
            if (gAbsentBattlerFlags & gBitTable[gMultiUsePlayerCursor])
                i = 0;
        } while (i == 0);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_ShowAsMoveTarget;
    }
}

static const u8 sText_MoveSelectionHighlightFormat[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW WHITE LIGHT_BLUE WHITE2}");

void Debug_HandleMoveTestInput(void);

static void HandleInputChooseMove(void)
{
    u32 canSelectTarget = 0;
#if DEBUG
    u8 count = 0;
#endif
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(gBattleBufferA[gActiveBattler] + 4);

    if (JOY_NEW(A_BUTTON))
    {
        u32 moveTarget;

        PlaySE(SE_SELECT);

        if (moveInfo->moves[gMoveSelectionCursor[gActiveBattler]] == MOVE_CURSE)
        {
            if (moveInfo->monTypes[0] != TYPE_GHOST && moveInfo->monTypes[1] != TYPE_GHOST)
                moveTarget = MOVE_TARGET_USER;
            else
                moveTarget = MOVE_TARGET_SELECTED;
        }
        else
            moveTarget = gBattleMoves[moveInfo->moves[gMoveSelectionCursor[gActiveBattler]]].target;

        if (moveTarget & 0x10)
            gMultiUsePlayerCursor = gActiveBattler;
        else
            gMultiUsePlayerCursor = GetBattlerAtPosition((GetBattlerPosition(gActiveBattler) & 1) ^ 1);

        if (gBattleBufferA[gActiveBattler][1] == 0)
        {
            if ((moveTarget & 2) && gBattleBufferA[gActiveBattler][2] == 0)
                canSelectTarget++;
        }
        else
        {
            if (!(moveTarget & 0x7D))
                canSelectTarget++;
            if (moveInfo->currentPP[gMoveSelectionCursor[gActiveBattler]] == 0)
            {
                canSelectTarget = 0;
            }
            else if (!(moveTarget & 0x12) && CountAliveMonsInBattle(BATTLE_ALIVE_EXCEPT_ACTIVE) <= 1)
            {
                gMultiUsePlayerCursor = GetDefaultMoveTarget(gActiveBattler);
                canSelectTarget = 0;
            }
        }
        if (canSelectTarget == 0)
        {
            DestroyMenuCursor();
            BtlController_EmitTwoReturnValues(1, 10, gMoveSelectionCursor[gActiveBattler] | (gMultiUsePlayerCursor << 8));
            PlayerBufferExecCompleted();
        }
        else
        {
            gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseTarget;
            if (moveTarget & 0x12)
                gMultiUsePlayerCursor = gActiveBattler;
            else if (gAbsentBattlerFlags & gBitTable[GetBattlerAtPosition(1)])
                gMultiUsePlayerCursor = GetBattlerAtPosition(3);
            else
                gMultiUsePlayerCursor = GetBattlerAtPosition(1);
            gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_ShowAsMoveTarget;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        DestroyMenuCursor();
        PlaySE(SE_SELECT);
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 320;
        BtlController_EmitTwoReturnValues(1, 10, 0xFFFF);
        PlayerBufferExecCompleted();
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        if (gMoveSelectionCursor[gActiveBattler] & 1)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[gActiveBattler]);
            gMoveSelectionCursor[gActiveBattler] ^= 1;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0);
            MoveSelectionDisplayPPNumber();
            MoveSelectionDisplayMoveType();
        }
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        if (!(gMoveSelectionCursor[gActiveBattler] & 1)
         && (gMoveSelectionCursor[gActiveBattler] ^ 1) < gNumberOfMovesToChoose)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[gActiveBattler]);
            gMoveSelectionCursor[gActiveBattler] ^= 1;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0);
            MoveSelectionDisplayPPNumber();
            MoveSelectionDisplayMoveType();
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gMoveSelectionCursor[gActiveBattler] & 2)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[gActiveBattler]);
            gMoveSelectionCursor[gActiveBattler] ^= 2;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0);
            MoveSelectionDisplayPPNumber();
            MoveSelectionDisplayMoveType();
        }
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (!(gMoveSelectionCursor[gActiveBattler] & 2)
         && (gMoveSelectionCursor[gActiveBattler] ^ 2) < gNumberOfMovesToChoose)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[gActiveBattler]);
            gMoveSelectionCursor[gActiveBattler] ^= 2;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0);
            MoveSelectionDisplayPPNumber();
            MoveSelectionDisplayMoveType();
        }
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        if (gNumberOfMovesToChoose > 1 && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
        {
            MoveSelectionDisplayMoveName(gMoveSelectionCursor[gActiveBattler], sText_MoveSelectionHighlightFormat);
            if (gMoveSelectionCursor[gActiveBattler] != 0)
                gMultiUsePlayerCursor = 0;
            else
                gMultiUsePlayerCursor = gMoveSelectionCursor[gActiveBattler] + 1;
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 27);
            Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 0x17, 0x37, 0x1C, 0x3A);
            Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_SwitchWhich, 0x290, 0x17, 0x37);
            Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
            gBattlerControllerFuncs[gActiveBattler] = HandleMoveSwitching;
        }
    }
#if DEBUG
    else if (gIsDebugMode == 1 && JOY_NEW(START_BUTTON))
    {
        const u8 *moveName;
        s32 i;

        Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 1, 0x37, 16, 0x3A);
        moveName = gMoveNames[GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1)];
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, moveName, 0x100, 2, 0x37);
        ConvertIntToDecimalStringN(
            gDisplayedStringBattle,
            GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1),
            2, 3);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x110, 10, 0x37);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, sText_DebugMoveTestTurn, 0x116, 1, 0x39);
        ConvertIntToDecimalStringN(gDisplayedStringBattle, gAnimMoveTurn, 2, 3);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x11C, 4, 0x39);
        for (i = 0; i < 64; i++)
        {
            if (gSprites[i].inUse)
                count++;
        }
        ConvertIntToDecimalStringN(gDisplayedStringBattle, count, 2, 2);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x122, 8, 0x39);
        count = GetTaskCount();
        ConvertIntToDecimalStringN(gDisplayedStringBattle, count, 2, 2);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x126, 11, 0x39);
        for (i = 0, count = 0; i < 32; i++)
        {
            if (gOamMatrixAllocBitmap & (1 << i))
                count++;
        }
        ConvertIntToDecimalStringN(gDisplayedStringBattle, count, 2, 2);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x12A, 14, 0x39);
        gBattlerControllerFuncs[gActiveBattler] = Debug_HandleMoveTestInput;
    }
#endif
}

extern const u8 BattleText_Format[];

void HandleMoveSwitching(void)
{
    u8 perMovePPBonuses[4];
    struct
    {
        u16 moves[4];
        u8 currentPP[4];
        u8 filler18[8];  // what is this?
    } moveStruct;
    //struct ChooseMoveStruct moveStruct;
    u8 totalPPBonuses;

    if (JOY_NEW(A_BUTTON | SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gMoveSelectionCursor[gActiveBattler] != gMultiUsePlayerCursor)
        {
            struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
            s32 i;

            i = moveInfo->moves[gMoveSelectionCursor[gActiveBattler]];
            moveInfo->moves[gMoveSelectionCursor[gActiveBattler]] = moveInfo->moves[gMultiUsePlayerCursor];
            moveInfo->moves[gMultiUsePlayerCursor] = i;

            i = moveInfo->currentPP[gMoveSelectionCursor[gActiveBattler]];
            moveInfo->currentPP[gMoveSelectionCursor[gActiveBattler]] = moveInfo->currentPP[gMultiUsePlayerCursor];
            moveInfo->currentPP[gMultiUsePlayerCursor] = i;

            i = moveInfo->maxPP[gMoveSelectionCursor[gActiveBattler]];
            moveInfo->maxPP[gMoveSelectionCursor[gActiveBattler]] = moveInfo->maxPP[gMultiUsePlayerCursor];
            moveInfo->maxPP[gMultiUsePlayerCursor] = i;

            if (gDisableStructs[gActiveBattler].mimickedMoves & gBitTable[gMoveSelectionCursor[gActiveBattler]])
            {
                gDisableStructs[gActiveBattler].mimickedMoves &= ~gBitTable[gMoveSelectionCursor[gActiveBattler]];
                gDisableStructs[gActiveBattler].mimickedMoves |= gBitTable[gMultiUsePlayerCursor];
            }

            MoveSelectionDisplayMoveNames();

            for (i = 0; i < 4; i++)
                perMovePPBonuses[i] = (gBattleMons[gActiveBattler].ppBonuses & (3 << (i * 2))) >> (i * 2);
            totalPPBonuses = perMovePPBonuses[gMoveSelectionCursor[gActiveBattler]];
            perMovePPBonuses[gMoveSelectionCursor[gActiveBattler]] = perMovePPBonuses[gMultiUsePlayerCursor];
            perMovePPBonuses[gMultiUsePlayerCursor] = totalPPBonuses;

            totalPPBonuses = 0;
            for (i = 0; i < 4; i++)
                totalPPBonuses |= perMovePPBonuses[i] << (i * 2);
            gBattleMons[gActiveBattler].ppBonuses = totalPPBonuses;

            for (i = 0; i < 4; i++)
            {
                gBattleMons[gActiveBattler].moves[i] = moveInfo->moves[i];
                gBattleMons[gActiveBattler].pp[i] = moveInfo->currentPP[i];
            }
            if (!(gBattleMons[gActiveBattler].status2 & STATUS2_TRANSFORMED))
            {
                for (i = 0; i < 4; i++)
                {
                    moveStruct.moves[i] = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1 + i);
                    moveStruct.currentPP[i] = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_PP1 + i);
                }

                totalPPBonuses = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_PP_BONUSES);
                for (i = 0; i < 4; i++)
                    perMovePPBonuses[i] = (totalPPBonuses & (3 << (i * 2))) >> (i * 2);

                i = moveStruct.moves[gMoveSelectionCursor[gActiveBattler]];
                moveStruct.moves[gMoveSelectionCursor[gActiveBattler]] = moveStruct.moves[gMultiUsePlayerCursor];
                moveStruct.moves[gMultiUsePlayerCursor] = i;

                i = moveStruct.currentPP[gMoveSelectionCursor[gActiveBattler]];
                moveStruct.currentPP[gMoveSelectionCursor[gActiveBattler]] = moveStruct.currentPP[gMultiUsePlayerCursor];
                moveStruct.currentPP[gMultiUsePlayerCursor] = i;

                totalPPBonuses = perMovePPBonuses[gMoveSelectionCursor[gActiveBattler]];
                perMovePPBonuses[gMoveSelectionCursor[gActiveBattler]] = perMovePPBonuses[gMultiUsePlayerCursor];
                perMovePPBonuses[gMultiUsePlayerCursor] = totalPPBonuses;

                totalPPBonuses = 0;
                for (i = 0; i < 4; i++)
                    totalPPBonuses |= perMovePPBonuses[i] << (i * 2);

                for (i = 0; i < 4; i++)
                {
                    SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1 + i, &moveStruct.moves[i]);
                    SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_PP1 + i, &moveStruct.currentPP[i]);
                }
                SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_PP_BONUSES, &totalPPBonuses);
            }
        }
        else
        {
            MoveSelectionDisplayMoveName(gMultiUsePlayerCursor, BattleText_Format);
        }
        gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseMove;
        gMoveSelectionCursor[gActiveBattler] = gMultiUsePlayerCursor;
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0);
        Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 0x17, 0x37, 0x1C, 0x3A);
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_PP, 0x290, 0x17, 0x37);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
        MoveSelectionDisplayPPNumber();
        MoveSelectionDisplayMoveType();
    }
    if (JOY_NEW(B_BUTTON | SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0);
        MoveSelectionDisplayMoveName(gMoveSelectionCursor[gActiveBattler], BattleText_Format);
        gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseMove;
        Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 0x17, 0x37, 0x1C, 0x3A);
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_PP, 0x290, 0x17, 0x37);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
        MoveSelectionDisplayPPNumber();
        MoveSelectionDisplayMoveType();
    }
    if (JOY_NEW(DPAD_LEFT) && (gMultiUsePlayerCursor & 1))
    {
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0x1D);
        else
            MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);
        gMultiUsePlayerCursor ^= 1;
        PlaySE(SE_SELECT);
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
        else
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0x1B);
    }
    if (JOY_NEW(DPAD_RIGHT) && !(gMultiUsePlayerCursor & 1) && (gMultiUsePlayerCursor ^ 1) < gNumberOfMovesToChoose)
    {
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0x1D);
        else
            MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);
        gMultiUsePlayerCursor ^= 1;
        PlaySE(SE_SELECT);
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
        else
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0x1B);
    }
    if (JOY_NEW(DPAD_UP) && (gMultiUsePlayerCursor & 2))
    {
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0x1D);
        else
            MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);
        gMultiUsePlayerCursor ^= 2;
        PlaySE(SE_SELECT);
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
        else
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0x1B);
    }
    if (JOY_NEW(DPAD_DOWN) && !(gMultiUsePlayerCursor & 2) && (gMultiUsePlayerCursor ^ 2) < gNumberOfMovesToChoose)
    {
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0x1D);
        else
            MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);
        gMultiUsePlayerCursor ^= 2;
        PlaySE(SE_SELECT);
        if (gMultiUsePlayerCursor == gMoveSelectionCursor[gActiveBattler])
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
        else
            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0x1B);
    }
}

void SetLinkBattleEndCallbacks(void)
{
    if (gReceivedRemoteLinkPlayers == 0)
    {
        m4aSongNumStop(SE_LOW_HEALTH);
        gMain.inBattle = FALSE;
        gMain.callback1 = gPreBattleCallback1;
        SetMainCallback2(CB2_InitEndLinkBattle);
    }
}

void SetBattleEndCallbacks(void)
{
    if (!gPaletteFade.active)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            SetCloseLinkCallback();
            gBattlerControllerFuncs[gActiveBattler] = SetLinkBattleEndCallbacks;
        }
        else
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            gMain.inBattle = FALSE;
            gMain.callback1 = gPreBattleCallback1;
            SetMainCallback2(gMain.savedCallback);
        }
    }
}

#if DEBUG

void Debug_WaitForMoveTestAnimation(void);

void Debug_HandleMoveTestInput(void)
{
    s16 move = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1);

    switch (gMain.newAndRepeatedKeys)
    {
    case START_BUTTON:
        EndBounceEffect(gActiveBattler, 1);
        EndBounceEffect(gActiveBattler, 0);
        gBattlerAttacker = gActiveBattler;
        if ((gMain.heldKeysRaw & B_BUTTON) && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            gBattlerTarget = gBattlerAttacker ^ 2;
        else if ((gMain.heldKeysRaw & A_BUTTON) && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            gBattlerTarget = GetBattlerAtPosition(3);
        else
            gBattlerTarget = GetBattlerAtPosition(1);
        SetBattlerSpriteAffineMode(0);
        DoMoveAnim(move);
        gBattlerControllerFuncs[gActiveBattler] = Debug_WaitForMoveTestAnimation;
        break;
    case SELECT_BUTTON:
        EndBounceEffect(gActiveBattler, 1);
        EndBounceEffect(gActiveBattler, 0);
        gBattlerTarget = gActiveBattler;
        if ((gMain.heldKeysRaw & B_BUTTON) && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            gBattlerAttacker = gBattlerTarget ^ 2;
        else if ((gMain.heldKeysRaw & A_BUTTON) && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            gBattlerAttacker = GetBattlerAtPosition(3);
        else
            gBattlerAttacker = GetBattlerAtPosition(1);
        SetBattlerSpriteAffineMode(0);
        DoMoveAnim(move);
        gBattlerControllerFuncs[gActiveBattler] = Debug_WaitForMoveTestAnimation;
        break;
    case R_BUTTON:
        if ((gMain.heldKeysRaw & B_BUTTON) && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        {
            EndBounceEffect(gActiveBattler, 1);
            EndBounceEffect(gActiveBattler, 0);
            gBattlerAttacker = GetBattlerAtPosition(3);
            gBattlerTarget = GetBattlerAtPosition(1);
            SetBattlerSpriteAffineMode(0);
            DoMoveAnim(move);
            gBattlerControllerFuncs[gActiveBattler] = Debug_WaitForMoveTestAnimation;
        }
        else
        {
            move += 9;
    case DPAD_RIGHT:
            if (++move > 354)
                move = 1;
            SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1, &move);
            gBattleMons[gActiveBattler].moves[0] = move;
            Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 1, 0x37, 16, 0x38);
            Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gMoveNames[move], 0x100, 2, 0x37);
            ConvertIntToDecimalStringN(gDisplayedStringBattle, move, 2, 3);
            Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 272, 10, 0x37);
        }
        break;
    case L_BUTTON:
        if ((gMain.heldKeysRaw & B_BUTTON) && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        {
            EndBounceEffect(gActiveBattler, 1);
            EndBounceEffect(gActiveBattler, 0);
            gBattlerAttacker = GetBattlerAtPosition(1);
            gBattlerTarget = GetBattlerAtPosition(3);
            SetBattlerSpriteAffineMode(0);
            DoMoveAnim(move);
            gBattlerControllerFuncs[gActiveBattler] = Debug_WaitForMoveTestAnimation;
        }
        else
        {
            move -= 9;
    case DPAD_LEFT:
            if (--move <= 0)
                move = 354;
            SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1, &move);
            gBattleMons[gActiveBattler].moves[0] = move;
            Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 1, 0x37, 16, 0x38);
            Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gMoveNames[move], 0x100, 2, 0x37);
            ConvertIntToDecimalStringN(gDisplayedStringBattle, move, 2, 3);
            Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 272, 10, 0x37);
        }
        break;
    case DPAD_UP:
    case DPAD_DOWN:
        if (gMain.newAndRepeatedKeys == DPAD_UP)
            gAnimMoveTurn--;
        else
            gAnimMoveTurn++;
        ConvertIntToDecimalStringN(gDisplayedStringBattle, gAnimMoveTurn, 2, 3);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 284, 4, 0x39);
        break;
    }

    if ((gMain.heldKeysRaw & (L_BUTTON | R_BUTTON)) == (L_BUTTON | R_BUTTON))
    {
        u8 i;
        u32 move;

        for (i = 0; i < 4; i++)
        {
            StringCopy(gDisplayedStringBattle, BattleText_Format);
            move = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1 + i);
            StringAppend(gDisplayedStringBattle, gMoveNames[move]);
            Text_InitWindow(
                &gWindowTemplate_Contest_MoveDescription,
                gDisplayedStringBattle,
                0x100 + i * 16,
                (i & 1) ? 10 : 2,
                (i < 2) ? 0x37 : 0x39);
            Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
        }
        gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseMove;
    }
}

void Debug_WaitForMoveTestAnimation(void)
{
    u8 count = 0;

    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        s32 i;

        SetBattlerSpriteAffineMode(1);
        DoBounceEffect(gActiveBattler, 1, 7, 1);
        DoBounceEffect(gActiveBattler, 0, 7, 1);

        for (i = 0, count = 0; i < MAX_SPRITES; i++)
        {
            if (gSprites[i].inUse)
            count++;
        }
        ConvertIntToDecimalStringN(gDisplayedStringBattle, count, 2, 2);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 290, 8, 0x39);

        count = GetTaskCount();
        ConvertIntToDecimalStringN(gDisplayedStringBattle, count, 2, 2);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 294, 11, 0x39);

        for (i = 0, count = 0; i < 32; i++)
        {
            if (gOamMatrixAllocBitmap & (1 << i))
            count++;
        }
        ConvertIntToDecimalStringN(gDisplayedStringBattle, count, 2, 2);
        Text_InitWindowAndPrintText(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 298, 14, 0x39);

        gBattlerControllerFuncs[gActiveBattler] = Debug_HandleMoveTestInput;
    }
}

#endif

void CompleteOnBattlerSpriteCallbackDummy(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        PlayerBufferExecCompleted();
}

// duplicate of CompleteOnBattlerSpriteCallbackDummy
void CompleteOnBankSpriteCallbackDummy2(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        PlayerBufferExecCompleted();
}

void FreeTrainerSpriteAfterSlide(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        BattleGfxSfxDummy3(gSaveBlock2.playerGender);
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        PlayerBufferExecCompleted();
    }
}

void Intro_DelayAndEnd(void)
{
    if (--gBattleHealthBoxInfo[gActiveBattler].introEndDelay == 0xFF)
    {
        gBattleHealthBoxInfo[gActiveBattler].introEndDelay = 0;
        PlayerBufferExecCompleted();
    }
}

void Intro_WaitForShinyAnimAndHealthbox(void)
{
    bool8 healthboxAnimDone = FALSE;

    if (!IsDoubleBattle() || (IsDoubleBattle() && (gBattleTypeFlags & BATTLE_TYPE_MULTI)))
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
            healthboxAnimDone = TRUE;
    }
    else
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy
         && gSprites[gHealthboxSpriteIds[gActiveBattler ^ 2]].callback == SpriteCallbackDummy)
            healthboxAnimDone = TRUE;
    }
    if (IsCryPlayingOrClearCrySongs())
        healthboxAnimDone = FALSE;

    if (healthboxAnimDone && gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim && gBattleHealthBoxInfo[gActiveBattler ^ 2].finishedShinyMonAnim)
    {
        gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler ^ 2].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler ^ 2].finishedShinyMonAnim = FALSE;
        FreeSpriteTilesByTag(0x27F9);
        FreeSpritePaletteByTag(0x27F9);
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            m4aMPlayContinue(&gMPlayInfo_BGM);
        else
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 256);
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        if (IsDoubleBattle())
            HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler ^ 2]], gActiveBattler ^ 2);
        gBattleHealthBoxInfo[gActiveBattler].introEndDelay = 3;
        gBattlerControllerFuncs[gActiveBattler] = Intro_DelayAndEnd;
    }
}

void Intro_TryShinyAnimShowHealthbox(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler, &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]]);
    if (!gBattleHealthBoxInfo[gActiveBattler ^ 2].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler ^ 2].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler ^ 2, &gPlayerParty[gBattlerPartyIndexes[gActiveBattler ^ 2]]);
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler ^ 2].ballAnimActive)
    {
        if (IsDoubleBattle() && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            DestroySprite(&gSprites[gBattleControllerData[gActiveBattler ^ 2]]);
            UpdateHealthboxAttribute(
              gHealthboxSpriteIds[gActiveBattler ^ 2],
              &gPlayerParty[gBattlerPartyIndexes[gActiveBattler ^ 2]],
              0);
            StartHealthboxSlideIn(gActiveBattler ^ 2);
            SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler ^ 2]);
        }
        DestroySprite(&gSprites[gBattleControllerData[gActiveBattler]]);
        UpdateHealthboxAttribute(
          gHealthboxSpriteIds[gActiveBattler],
          &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]],
          0);
        StartHealthboxSlideIn(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
        gBattleAnimationInfo.introAnimActive = FALSE;
        gBattlerControllerFuncs[gActiveBattler] = Intro_WaitForShinyAnimAndHealthbox;
    }
}

void SwitchIn_CleanShinyAnimShowSubstitute(void)
{
    if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy
     && gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim)
    {
        gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
        FreeSpriteTilesByTag(0x27F9);
        FreeSpritePaletteByTag(0x27F9);
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 6);
        gBattlerControllerFuncs[gActiveBattler] = SwitchIn_HandleSoundAndEnd;
    }
}

void SwitchIn_HandleSoundAndEnd(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive && !IsCryPlayingOrClearCrySongs())
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x100);
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        PlayerBufferExecCompleted();
    }
}

void SwitchIn_TryShinyAnimShowHealthbox(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler, &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]]);
    if (gSprites[gBattleControllerData[gActiveBattler]].callback == SpriteCallbackDummy
     && !gBattleHealthBoxInfo[gActiveBattler].ballAnimActive)
    {
        DestroySprite(&gSprites[gBattleControllerData[gActiveBattler]]);
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 0);
        StartHealthboxSlideIn(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
        CopyBattleSpriteInvisibility(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = SwitchIn_CleanShinyAnimShowSubstitute;
    }
}

void Task_PlayerController_RestoreBgmAfterCry(u8 taskId)
{
    if (!IsCryPlayingOrClearCrySongs())
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x100);
        DestroyTask(taskId);
    }
}

void CompleteOnHealthbarDone(void)
{
    s16 hpValue = MoveBattleBar(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], 0, 0);

    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
    if (hpValue != -1)
    {
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], hpValue, 0);
    }
    else
    {
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        PlayerBufferExecCompleted();
    }
}

static void CompleteOnInactiveTextPrinter(void)
{
    if (gWindowTemplate_Contest_MoveDescription.state == 0)
        PlayerBufferExecCompleted();
}

#define tExpTask_monId      data[0]
#define tExpTask_gainedExp  data[1]
#define tExpTask_battler    data[2]
#define tExpTask_frames     data[10]

// Rare Candy usage, maybe?
void Task_GiveExpToMon(u8 taskId)
{
    u32 monId = (u8)gTasks[taskId].tExpTask_monId;
    u8 battler = gTasks[taskId].tExpTask_battler;
    s16 gainedExp = gTasks[taskId].tExpTask_gainedExp;

    if (IsDoubleBattle() == TRUE || monId != gBattlerPartyIndexes[battler])
    {
        struct Pokemon *mon = &gPlayerParty[monId];
        u16 species = GetMonData(mon, MON_DATA_SPECIES);
        u8 level = GetMonData(mon, MON_DATA_LEVEL);
        u32 currExp = GetMonData(mon, MON_DATA_EXP);
        u32 nextLvlExp = gExperienceTables[gBaseStats[species].growthRate][level + 1];

        if (currExp + gainedExp >= nextLvlExp)
        {
            u8 savedActiveBattler;

            SetMonData(mon, MON_DATA_EXP, &nextLvlExp);
            CalculateMonStats(mon);
            gainedExp -= nextLvlExp - currExp;
            savedActiveBattler = gActiveBattler;
            gActiveBattler = battler;
            BtlController_EmitTwoReturnValues(1, 11, gainedExp);
            gActiveBattler = savedActiveBattler;

            if (IsDoubleBattle() == TRUE
             && ((u16)monId == gBattlerPartyIndexes[battler] || (u16)monId == gBattlerPartyIndexes[battler ^ 2]))
                gTasks[taskId].func = Task_LaunchLvlUpAnim;
            else
                gTasks[taskId].func = DestroyExpTaskAndCompleteOnInactiveTextPrinter;
        }
        else
        {
            currExp += gainedExp;
            SetMonData(mon, MON_DATA_EXP, &currExp);
            gBattlerControllerFuncs[battler] = CompleteOnInactiveTextPrinter;
            DestroyTask(taskId);
        }
    }
    else
    {
        gTasks[taskId].func = Task_PrepareToGiveExpWithExpBar;
    }
}

void Task_PrepareToGiveExpWithExpBar(u8 taskId)
{
    u8 monIndex = gTasks[taskId].tExpTask_monId;
    s32 gainedExp = gTasks[taskId].tExpTask_gainedExp;
    u8 battler = gTasks[taskId].tExpTask_battler;
    struct Pokemon *mon = &gPlayerParty[monIndex];
    u8 level = GetMonData(mon, MON_DATA_LEVEL);
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 exp = GetMonData(mon, MON_DATA_EXP);
    u32 currLvlExp = gExperienceTables[gBaseStats[species].growthRate][level];
    u32 expToNextLvl;

    exp -= currLvlExp;
    expToNextLvl = gExperienceTables[gBaseStats[species].growthRate][level + 1] - currLvlExp;
    SetBattleBarStruct(battler, gHealthboxSpriteIds[battler], expToNextLvl, exp, -gainedExp);
    PlaySE(SE_EXP);
    gTasks[taskId].func = Task_GiveExpWithExpBar;
}

void Task_GiveExpWithExpBar(u8 taskId)
{
    if (gTasks[taskId].tExpTask_frames < 13)
    {
        gTasks[taskId].tExpTask_frames++;
    }
    else
    {
        u8 monId = gTasks[taskId].tExpTask_monId;
        s16 gainedExp = gTasks[taskId].tExpTask_gainedExp;
        u8 battlerId = gTasks[taskId].tExpTask_battler;
        s16 newExpPoints;

        newExpPoints = MoveBattleBar(battlerId, gHealthboxSpriteIds[battlerId], 1, 0);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[battlerId]);
        if (newExpPoints == -1)
        {
            u8 level;
            s32 currExp;
            u16 species;
            s32 expOnNextLvl;

            m4aSongNumStop(SE_EXP);
            level = GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL);
            currExp = GetMonData(&gPlayerParty[monId], MON_DATA_EXP);
            species = GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);
            expOnNextLvl = gExperienceTables[gBaseStats[species].growthRate][level + 1];

            if (currExp + gainedExp >= expOnNextLvl)
            {
                u8 savedActiveBattler;

                SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &expOnNextLvl);
                CalculateMonStats(&gPlayerParty[monId]);
                gainedExp -= expOnNextLvl - currExp;
                savedActiveBattler = gActiveBattler;
                gActiveBattler = battlerId;
                BtlController_EmitTwoReturnValues(1, 11, gainedExp);
                gActiveBattler = savedActiveBattler;
                gTasks[taskId].func = Task_LaunchLvlUpAnim;
            }
            else
            {
                currExp += gainedExp;
                SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &currExp);
                gBattlerControllerFuncs[battlerId] = CompleteOnInactiveTextPrinter;
                DestroyTask(taskId);
            }
        }
    }
}

void Task_LaunchLvlUpAnim(u8 taskId)
{
    u8 battler = gTasks[taskId].tExpTask_battler;
    u8 monIndex = gTasks[taskId].tExpTask_monId;

    if (IsDoubleBattle() == TRUE && monIndex == gBattlerPartyIndexes[battler ^ 2])
        battler ^= 2;
    InitAndLaunchSpecialAnimation(battler, battler, battler, 0);
    gTasks[taskId].func = Task_UpdateLvlInHealthbox;
}

void Task_UpdateLvlInHealthbox(u8 taskId)
{
    u8 battler = gTasks[taskId].tExpTask_battler;

    if (!gBattleHealthBoxInfo[battler].specialAnimActive)
    {
        u8 monIndex = gTasks[taskId].tExpTask_monId;

        GetMonData(&gPlayerParty[monIndex], MON_DATA_LEVEL);  // Unused return value
        if (IsDoubleBattle() == TRUE && monIndex == gBattlerPartyIndexes[battler ^ 2])
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler ^ 2], &gPlayerParty[monIndex], 0);
        else
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gPlayerParty[monIndex], 0);
        gTasks[taskId].func = DestroyExpTaskAndCompleteOnInactiveTextPrinter;
    }
}

void DestroyExpTaskAndCompleteOnInactiveTextPrinter(u8 taskId)
{
    u8 monIndex;
    u8 battler;

    monIndex = gTasks[taskId].tExpTask_monId;
    GetMonData(&gPlayerParty[monIndex], MON_DATA_LEVEL);  // Unused return value
    battler = gTasks[taskId].tExpTask_battler;
    gBattlerControllerFuncs[battler] = CompleteOnInactiveTextPrinter;
    DestroyTask(taskId);
}

void FreeMonSpriteAfterFaintAnim(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].y + gSprites[gBattlerSpriteIds[gActiveBattler]].y2 > DISPLAY_HEIGHT)
    {
        u16 species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

        BattleGfxSfxDummy2(species);
        FreeOamMatrix(gSprites[gBattlerSpriteIds[gActiveBattler]].oam.matrixNum);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        PlayerBufferExecCompleted();
    }
}

void FreeMonSpriteAfterSwitchOutAnim(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        PlayerBufferExecCompleted();
    }
}

// Duplicate of CompleteOnInactiveTextPrinter
void CompleteOnInactiveTextPrinter2(void)
{
    if (gWindowTemplate_Contest_MoveDescription.state == 0)
        PlayerBufferExecCompleted();
}

void OpenPartyMenuToChooseMon(void)
{
    if (!gPaletteFade.active)
    {
        u8 caseId;

        gBattlerControllerFuncs[gActiveBattler] = WaitForMonSelection;
        caseId = gTasks[gBattleControllerData[gActiveBattler]].data[0];
        DestroyTask(gBattleControllerData[gActiveBattler]);
        OpenBattlePartyMenuWithAction(caseId);
    }
}

void WaitForMonSelection(void)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        if (gPartyMenuActionResult == 1)
            BtlController_EmitChosenMonReturnValue(1, gSelectedMonPartyId, gBattlePartyCurrentOrder);
        else
            BtlController_EmitChosenMonReturnValue(1, 6, NULL);
        if ((gBattleBufferA[gActiveBattler][1] & 0xF) == 1)
            PrintLinkStandbyMsg();
        PlayerBufferExecCompleted();
    }
}

void OpenBagAndChooseItem(void)
{
    if (!gPaletteFade.active)
    {
        gBattlerControllerFuncs[gActiveBattler] = CompleteWhenChoseItem;
        ReshowBattleScreenDummy();
        CB2_BagMenuFromBattle();
    }
}

void CompleteWhenChoseItem(void)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        BtlController_EmitOneReturnValue(1, gSpecialVar_ItemId);
        PlayerBufferExecCompleted();
    }
}

static void CompleteOnSpecialAnimDone(void)
{
    if (!gDoingBattleAnim || !gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        PlayerBufferExecCompleted();
}

void DoHitAnimBlinkSpriteEffect(void)
{
    u8 spriteId = gBattlerSpriteIds[gActiveBattler];

    if (gSprites[spriteId].data[1] == 32)
    {
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].invisible = FALSE;
        gDoingBattleAnim = 0;
        PlayerBufferExecCompleted();
    }
    else
    {
        if (((u16)gSprites[spriteId].data[1] % 4) == 0)
            gSprites[spriteId].invisible ^= 1;
        gSprites[spriteId].data[1]++;
    }
}

void MoveSelectionDisplayMoveName(s32 cursorPosition, const u8 *format)
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];

    StringCopy(gDisplayedStringBattle, format);
    StringAppend(gDisplayedStringBattle, gMoveNames[moveInfo->moves[cursorPosition]]);
    Text_InitWindow(
      &gWindowTemplate_Contest_MoveDescription,
      gDisplayedStringBattle,
      0x300 + cursorPosition * 20,
      (cursorPosition & 1) ? 11 : 1,
      (cursorPosition < 2) ? 0x37 : 0x39);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
}

void MoveSelectionDisplayMoveNames(void)
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
    s32 i;

    gNumberOfMovesToChoose = 0;
    Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 1, 0x37, 0x14, 0x3A);
    for (i = 0; i < 4; i++)
    {
        MoveSelectionDestroyCursorAt(i);
        MoveSelectionDisplayMoveName(i, BattleText_Format);
        if (moveInfo->moves[i] != 0)
            gNumberOfMovesToChoose++;
    }
}

void MoveSelectionDisplayPPNumber(void)
{
    if (gBattleBufferA[gActiveBattler][2] != 1)
    {
        struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
        u8 *str = gDisplayedStringBattle;

        str = StringCopy(str, BattleText_Format);
        str[0] = EXT_CTRL_CODE_BEGIN;
        str[1] = 0x11;
        str[2] = 2;
        str += 3;
        str[0] = EXT_CTRL_CODE_BEGIN;
        str[1] = 0x14;
        str[2] = 6;
        str += 3;
        str = ConvertIntToDecimalStringN(str, moveInfo->currentPP[gMoveSelectionCursor[gActiveBattler]], 1, 2);
        *str++ = CHAR_SLASH;
        ConvertIntToDecimalStringN(str, moveInfo->maxPP[gMoveSelectionCursor[gActiveBattler]], 1, 2);
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x2A2, 0x19, 0x37);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    }
}

extern const u8 BattleText_ForgetMove[];
extern const u8 gTypeNames[][7];

void MoveSelectionDisplayMoveType(void)
{
    if (gBattleBufferA[gActiveBattler][2] == 1)
    {
        Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 0x17, 0x37, 0x1C, 0x3A);
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_ForgetMove, 0x290, 0x13, 0x37);
    }
    else
    {
        struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
        u8 *str = gDisplayedStringBattle;

        str = StringCopy(str, BattleText_Format);
        StringCopy(str, gTypeNames[gBattleMoves[moveInfo->moves[gMoveSelectionCursor[gActiveBattler]]].type]);
        Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 0x17, 0x39, 0x1C, 0x3A);
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x2C0, 0x17, 0x39);
    }
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
}

static const u8 sMoveSelectionCursorCoords[][2] =
{
    { 8, 120},
    {88, 120},
    { 8, 136},
    {88, 136},
};

static const u8 sActionSelectionCursorCoords[][2] =
{
    {144, 120},
    {190, 120},
    {144, 136},
    {190, 136},
    { 72,  72},
    { 32,  90},
    { 80,  80},
    { 80,  88},
};

void MoveSelectionCreateCursorAt(u8 cursorPosition, int baseTileNum)
{
    SetOutlineCursorWidth(0x48);
    SetOutlineCursorPosition(sMoveSelectionCursorCoords[cursorPosition][0], sMoveSelectionCursorCoords[cursorPosition][1]);
}

void MoveSelectionDestroyCursorAt(u8 cursorPosition)
{
}

void ActionSelectionCreateCursorAt(u8 cursorPosition, u8 baseTileNum)
{
    // Ruby keeps one outline cursor instead of Emerald's action-menu tilemap cursors.
    SetOutlineCursorWidth(0x2A);
    SetOutlineCursorPosition(sActionSelectionCursorCoords[cursorPosition][0], sActionSelectionCursorCoords[cursorPosition][1]);
}

void ActionSelectionDestroyCursorAt(u8 cursorPosition)
{
    // Repositioning the Ruby outline cursor needs no separate tile cleanup.
}

void CB2_SetUpReshowBattleScreenAfterMenu(void)
{
    SetMainCallback2(ReshowBattleScreenAfterMenu);
}

void CB2_SetUpReshowBattleScreenAfterMenu2(void)
{
    SetMainCallback2(ReshowBattleScreenAfterMenu);
}

void CompleteOnFinishedStatusAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].statusAnimActive)
        PlayerBufferExecCompleted();
}

void CompleteOnFinishedBattleAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].animFromTableActive)
        PlayerBufferExecCompleted();
}

void PrintLinkStandbyMsg(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 0;
        Contest_StartTextPrinter(&gWindowTemplate_Contest_MoveDescription, BattleText_LinkStandby, 0x90, 2, 15);
    }
}

void PlayerHandleGetMonData(void)
{
    u8 monData[0x100];
    u32 size = 0;
    u8 monToCheck;
    s32 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        size += CopyPlayerMonData(gBattlerPartyIndexes[gActiveBattler], monData);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                size += CopyPlayerMonData(i, monData + size);
            monToCheck >>= 1;
        }
    }
    BtlController_EmitDataTransfer(1, size, monData);
    PlayerBufferExecCompleted();
}

// Duplicate of the Link Partner mon-data copy routine.
u32 CopyPlayerMonData(u8 monId, u8 *dst)
{
    struct BattlePokemon battleMon;
    struct MovePPInfo moveData;
    u8 nickname[20];
    u8 *src;
    s16 data16;
    u32 data32;
    s32 size = 0;

    switch (gBattleBufferA[gActiveBattler][1])
    {
    case 0:
        battleMon.species = GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);
        battleMon.item = GetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM);
        for (size = 0; size < 4; size++)
        {
            battleMon.moves[size] = GetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + size);
            battleMon.pp[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + size);
        }
        battleMon.ppBonuses = GetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES);
        battleMon.friendship = GetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP);
        battleMon.experience = GetMonData(&gPlayerParty[monId], MON_DATA_EXP);
        battleMon.hpIV = GetMonData(&gPlayerParty[monId], MON_DATA_HP_IV);
        battleMon.attackIV = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV);
        battleMon.defenseIV = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV);
        battleMon.speedIV = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV);
        battleMon.spAttackIV = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV);
        battleMon.spDefenseIV = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV);
        battleMon.personality = GetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY);
        battleMon.status1 = GetMonData(&gPlayerParty[monId], MON_DATA_STATUS);
        battleMon.level = GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL);
        battleMon.hp = GetMonData(&gPlayerParty[monId], MON_DATA_HP);
        battleMon.maxHP = GetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP);
        battleMon.attack = GetMonData(&gPlayerParty[monId], MON_DATA_ATK);
        battleMon.defense = GetMonData(&gPlayerParty[monId], MON_DATA_DEF);
        battleMon.speed = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED);
        battleMon.spAttack = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK);
        battleMon.spDefense = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF);
        battleMon.isEgg = GetMonData(&gPlayerParty[monId], MON_DATA_IS_EGG);
        battleMon.abilityNum = GetMonData(&gPlayerParty[monId], MON_DATA_ABILITY_NUM);
        battleMon.otId = GetMonData(&gPlayerParty[monId], MON_DATA_OT_ID);
        GetMonData(&gPlayerParty[monId], MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(battleMon.nickname, nickname);
        GetMonData(&gPlayerParty[monId], MON_DATA_OT_NAME, battleMon.otName);
        MEMCPY_ALT(&battleMon, dst, sizeof(battleMon), size, src);
        break;
    case 1:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 2:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 3:
        for (size = 0; size < 4; size++)
        {
            moveData.moves[size] = GetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + size);
            moveData.pp[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + size);
        }
        moveData.ppBonuses = GetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES);
        MEMCPY_ALT(&moveData, dst, sizeof(moveData), size, src);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - 4);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 8:
        for (size = 0; size < 4; size++)
            dst[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + size);
        dst[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES);
        size++;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - 9);
        size = 1;
        break;
    case 17:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_OT_ID);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case 18:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_EXP);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case 19:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_HP_EV);
        size = 1;
        break;
    case 20:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_EV);
        size = 1;
        break;
    case 21:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_EV);
        size = 1;
        break;
    case 22:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_EV);
        size = 1;
        break;
    case 23:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_EV);
        size = 1;
        break;
    case 24:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_EV);
        size = 1;
        break;
    case 25:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP);
        size = 1;
        break;
    case 26:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_POKERUS);
        size = 1;
        break;
    case 27:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_MET_LOCATION);
        size = 1;
        break;
    case 28:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_MET_LEVEL);
        size = 1;
        break;
    case 29:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_MET_GAME);
        size = 1;
        break;
    case 30:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_POKEBALL);
        size = 1;
        break;
    case 31:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_HP_IV);
        dst[1] = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV);
        dst[2] = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV);
        dst[3] = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV);
        dst[4] = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV);
        dst[5] = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV);
        size = 6;
        break;
    case 32:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_HP_IV);
        size = 1;
        break;
    case 33:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV);
        size = 1;
        break;
    case 34:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV);
        size = 1;
        break;
    case 35:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV);
        size = 1;
        break;
    case 36:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV);
        size = 1;
        break;
    case 37:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV);
        size = 1;
        break;
    case 38:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case 39:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_CHECKSUM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 40:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_STATUS);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case 41:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL);
        size = 1;
        break;
    case 42:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 43:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 44:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_ATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 45:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_DEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 46:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 47:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 48:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 49:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_COOL);
        size = 1;
        break;
    case 50:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY);
        size = 1;
        break;
    case 51:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_CUTE);
        size = 1;
        break;
    case 52:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SMART);
        size = 1;
        break;
    case 53:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_TOUGH);
        size = 1;
        break;
    case 54:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SHEEN);
        size = 1;
        break;
    case 55:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_COOL_RIBBON);
        size = 1;
        break;
    case 56:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY_RIBBON);
        size = 1;
        break;
    case 57:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_CUTE_RIBBON);
        size = 1;
        break;
    case 58:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SMART_RIBBON);
        size = 1;
        break;
    case 59:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_TOUGH_RIBBON);
        size = 1;
        break;
    }
    return size;
}

void PlayerHandleGetRawMonData(void)
{
    struct BattlePokemon battleMon;
    u8 i;
    // TODO: Maybe fix this. Integrating this into MEMSET_ALT is too hard.
    u8 *src = (u8 *)&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]] + gBattleBufferA[gActiveBattler][1];
    u8 *dst;

    MEMSET_ALT(&battleMon + gBattleBufferA[gActiveBattler][1], src[i], gBattleBufferA[gActiveBattler][2], i, dst);
    BtlController_EmitDataTransfer(1, gBattleBufferA[gActiveBattler][2], dst);
    PlayerBufferExecCompleted();
}

void PlayerHandleSetMonData(void)
{
    u8 monToCheck;
    u8 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        SetPlayerMonData(gBattlerPartyIndexes[gActiveBattler]);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                SetPlayerMonData(i);
            monToCheck >>= 1;
        }
    }
    PlayerBufferExecCompleted();
}

// Duplicate of SetLinkPartnerMonData
void SetPlayerMonData(u8 monId)
{
    struct BattlePokemon *battlePokemon = (struct BattlePokemon *)&gBattleBufferA[gActiveBattler][3];
    struct MovePPInfo *moveData = (struct MovePPInfo *)&gBattleBufferA[gActiveBattler][3];
    s32 i;

    switch (gBattleBufferA[gActiveBattler][1])
    {
    case 0:
        {
            u8 iv;

            SetMonData(&gPlayerParty[monId], MON_DATA_SPECIES, &battlePokemon->species);
            SetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM, &battlePokemon->item);
            for (i = 0; i < 4; i++)
            {
                SetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + i, &battlePokemon->moves[i]);
                SetMonData(&gPlayerParty[monId], MON_DATA_PP1 + i, &battlePokemon->pp[i]);
            }
            SetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES, &battlePokemon->ppBonuses);
            SetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP, &battlePokemon->friendship);
            SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &battlePokemon->experience);
            iv = battlePokemon->hpIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_HP_IV, &iv);
            iv = battlePokemon->attackIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV, &iv);
            iv = battlePokemon->defenseIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV, &iv);
            iv = battlePokemon->speedIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV, &iv);
            iv = battlePokemon->spAttackIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV, &iv);
            iv = battlePokemon->spDefenseIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV, &iv);
            SetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY, &battlePokemon->personality);
            SetMonData(&gPlayerParty[monId], MON_DATA_STATUS, &battlePokemon->status1);
            SetMonData(&gPlayerParty[monId], MON_DATA_LEVEL, &battlePokemon->level);
            SetMonData(&gPlayerParty[monId], MON_DATA_HP, &battlePokemon->hp);
            SetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP, &battlePokemon->maxHP);
            SetMonData(&gPlayerParty[monId], MON_DATA_ATK, &battlePokemon->attack);
            SetMonData(&gPlayerParty[monId], MON_DATA_DEF, &battlePokemon->defense);
            SetMonData(&gPlayerParty[monId], MON_DATA_SPEED, &battlePokemon->speed);
            SetMonData(&gPlayerParty[monId], MON_DATA_SPATK, &battlePokemon->spAttack);
            SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF, &battlePokemon->spDefense);
        }
        break;
    case 1:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPECIES, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 2:
        SetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 3:
        for (i = 0; i < 4; i++)
        {
            SetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + i, &moveData->moves[i]);
            SetMonData(&gPlayerParty[monId], MON_DATA_PP1 + i, &moveData->pp[i]);
        }
        SetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES, &moveData->ppBonuses);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        SetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - 4, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 8:
        SetMonData(&gPlayerParty[monId], MON_DATA_PP1, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP2, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP3, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP4, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES, &gBattleBufferA[gActiveBattler][7]);
        break;
    case 9:
    case 10:
    case 11:
    case 12:
        SetMonData(&gPlayerParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - 9, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 17:
        SetMonData(&gPlayerParty[monId], MON_DATA_OT_ID, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 18:
        SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 19:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 20:
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 21:
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 22:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 23:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 24:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 25:
        SetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 26:
        SetMonData(&gPlayerParty[monId], MON_DATA_POKERUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 27:
        SetMonData(&gPlayerParty[monId], MON_DATA_MET_LOCATION, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 28:
        SetMonData(&gPlayerParty[monId], MON_DATA_MET_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 29:
        SetMonData(&gPlayerParty[monId], MON_DATA_MET_GAME, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 30:
        SetMonData(&gPlayerParty[monId], MON_DATA_POKEBALL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 31:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][7]);
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][8]);
        break;
    case 32:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 33:
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 34:
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 35:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 36:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 37:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 38:
        SetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 39:
        SetMonData(&gPlayerParty[monId], MON_DATA_CHECKSUM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 40:
        SetMonData(&gPlayerParty[monId], MON_DATA_STATUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 41:
        SetMonData(&gPlayerParty[monId], MON_DATA_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 42:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 43:
        SetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 44:
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 45:
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 46:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 47:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 48:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 49:
        SetMonData(&gPlayerParty[monId], MON_DATA_COOL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 50:
        SetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 51:
        SetMonData(&gPlayerParty[monId], MON_DATA_CUTE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 52:
        SetMonData(&gPlayerParty[monId], MON_DATA_SMART, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 53:
        SetMonData(&gPlayerParty[monId], MON_DATA_TOUGH, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 54:
        SetMonData(&gPlayerParty[monId], MON_DATA_SHEEN, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 55:
        SetMonData(&gPlayerParty[monId], MON_DATA_COOL_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 56:
        SetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 57:
        SetMonData(&gPlayerParty[monId], MON_DATA_CUTE_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 58:
        SetMonData(&gPlayerParty[monId], MON_DATA_SMART_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 59:
        SetMonData(&gPlayerParty[monId], MON_DATA_TOUGH_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    }
    HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
}

void PlayerHandleSetRawMonData(void)
{
    u8 i;
    u8 *dst;

    MEMSET_ALT(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]] + gBattleBufferA[gActiveBattler][1], gBattleBufferA[gActiveBattler][3 + i],
        gBattleBufferA[gActiveBattler][2], i, dst);
    PlayerBufferExecCompleted();
}

void PlayerHandleLoadMonSprite(void)
{
    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBankSpritePosX_0;
}

void PlayerHandleSwitchInAnim(void)
{
    ClearTemporarySpeciesSpriteData(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerPartyIndexes[gActiveBattler] = gBattleBufferA[gActiveBattler][1];
    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    gActionSelectionCursor[gActiveBattler] = 0;
    gMoveSelectionCursor[gActiveBattler] = 0;
    StartSendOutAnim(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerControllerFuncs[gActiveBattler] = SwitchIn_TryShinyAnimShowHealthbox;
}

void StartSendOutAnim(u8 battler, u8 dontClearSubstituteBit)
{
    u16 species;

    ClearTemporarySpeciesSpriteData(battler, dontClearSubstituteBit);
    gBattlerPartyIndexes[battler] = gBattleBufferA[battler][1];
    species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
    gBattleControllerData[battler] = CreateInvisibleSpriteWithCallback(SpriteCB_WaitForBattlerBallReleaseAnim);
    SetMultiuseSpriteTemplateToPokemon(species, GetBattlerPosition(battler));
    gBattlerSpriteIds[battler] = CreateSprite(
      &gCreatingSpriteTemplate,
      GetBattlerSpriteCoord(battler, 2),
      GetBattlerSpriteDefault_Y(battler),
      GetBattlerSpriteSubpriority(battler));
    gSprites[gBattleControllerData[battler]].data[1] = gBattlerSpriteIds[battler];
    gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
    gSprites[gBattlerSpriteIds[battler]].data[2] = species;
    gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[battler]], gBattleMonForms[battler]);
    gSprites[gBattlerSpriteIds[battler]].invisible = TRUE;
    gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
    gSprites[gBattleControllerData[battler]].data[0] = DoPokeballSendOutAnimation(0, POKEBALL_PLAYER_SENDOUT);
}

void PlayerHandleReturnMonToBall(void)
{
    if (gBattleBufferA[gActiveBattler][1] == 0)
    {
        gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
        gBattlerControllerFuncs[gActiveBattler] = DoSwitchOutAnimation;
    }
    else
    {
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        PlayerBufferExecCompleted();
    }
}

void DoSwitchOutAnimation(void)
{
    switch (gBattleHealthBoxInfo[gActiveBattler].animationState)
    {
    case 0:
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 5);
        gBattleHealthBoxInfo[gActiveBattler].animationState = 1;
        break;
    case 1:
        if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        {
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 1);
            gBattlerControllerFuncs[gActiveBattler] = FreeMonSpriteAfterSwitchOutAnim;
        }
    }
}

void PlayerHandleDrawTrainerPic(void)
{
    s16 xOffset;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (GetBattlerPosition(gActiveBattler) & 2)
            xOffset = 16;
        else
            xOffset = -16;
    }
    else
    {
        xOffset = 0;
    }
    DecompressTrainerBackPic(gSaveBlock2.playerGender, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(gSaveBlock2.playerGender, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gCreatingSpriteTemplate,
      xOffset + 80,
      (8 - gTrainerBackPicCoords[gSaveBlock2.playerGender].coords) * 4 + 80,
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = 240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = -2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBattlerSpriteCallbackDummy;
}

void PlayerHandleTrainerSlide(void)
{
    DecompressTrainerBackPic(gSaveBlock2.playerGender, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(gSaveBlock2.playerGender, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gCreatingSpriteTemplate,
      80,
      (8 - gTrainerBackPicCoords[gSaveBlock2.playerGender].coords) * 4 + 80,
      30);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -96;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBankSpriteCallbackDummy2;
}

void PlayerHandleTrainerSlideBack(void)
{
    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 50;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = -40;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCallbackDummy);
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], 1);
    gBattlerControllerFuncs[gActiveBattler] = FreeTrainerSpriteAfterSlide;
}

void PlayerHandleFaintAnimation(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].animationState == 0)
    {
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 5);
        gBattleHealthBoxInfo[gActiveBattler].animationState++;
    }
    else
    {
        if (gBattleHealthBoxInfo[gActiveBattler].specialAnimActive == 0)
        {
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
            PlaySE12WithPanning(SE_FAINT, -64);
            gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
            gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 5;
            gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_FaintSlideAnim;
            gBattlerControllerFuncs[gActiveBattler] = FreeMonSpriteAfterFaintAnim;
        }
    }
}

void PlayerHandlePaletteFade(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 2, 0, 16, RGB(0, 0, 0));
    PlayerBufferExecCompleted();
}

void PlayerHandleSuccessBallThrowAnim(void)
{
    gBattleAnimationInfo.ballThrowCaseId = BALL_3_SHAKES_SUCCESS;
    gDoingBattleAnim = TRUE;
    InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), B_ANIM_BALL_THROW);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnSpecialAnimDone;
}

void PlayerHandleBallThrowAnim(void)
{
    u8 ballThrowCaseId = gBattleBufferA[gActiveBattler][1];

    gBattleAnimationInfo.ballThrowCaseId = ballThrowCaseId;
    gDoingBattleAnim = TRUE;
    InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), B_ANIM_BALL_THROW);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnSpecialAnimDone;
}

void PlayerHandlePause(void)
{
    u8 timer = gBattleBufferA[gActiveBattler][1];

    // WTF is this??
    while (timer != 0)
        timer--;

    PlayerBufferExecCompleted();
}

void PlayerHandleMoveAnimation(void)
{
    if (!IsBattleSEPlaying(gActiveBattler))
    {
        u16 move = gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8);

        gAnimMoveTurn = gBattleBufferA[gActiveBattler][3];
        gAnimMovePower = gBattleBufferA[gActiveBattler][4] | (gBattleBufferA[gActiveBattler][5] << 8);
        gAnimMoveDmg = gBattleBufferA[gActiveBattler][6] | (gBattleBufferA[gActiveBattler][7] << 8) | (gBattleBufferA[gActiveBattler][8] << 16) | (gBattleBufferA[gActiveBattler][9] << 24);
        gAnimFriendship = gBattleBufferA[gActiveBattler][10];
        gWeatherMoveAnim = gBattleBufferA[gActiveBattler][12] | (gBattleBufferA[gActiveBattler][13] << 8);
        gAnimDisableStructPtr = (struct DisableStruct *)&gBattleBufferA[gActiveBattler][16];
        gTransformedPersonalities[gActiveBattler] = gAnimDisableStructPtr->transformedMonPersonality;
        if (IsMoveWithoutAnimation(move, gAnimMoveTurn) != 0)
        {
            // Dead code. IsMoveWithoutAnimation always returns 0.
            PlayerBufferExecCompleted();
        }
        else
        {
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            gBattlerControllerFuncs[gActiveBattler] = PlayerDoMoveAnimation;
        }
    }
}

void PlayerDoMoveAnimation(void)
{
    u16 move = gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8);
    u8 multihit = gBattleBufferA[gActiveBattler][11];

    switch (gBattleHealthBoxInfo[gActiveBattler].animationState)
    {
    case 0:
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute == 1 &&
            gBattleSpriteInfo[gActiveBattler].flag_x8 == 0)
        {
            gBattleSpriteInfo[gActiveBattler].flag_x8 = 1;
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 5);
        }
        gBattleHealthBoxInfo[gActiveBattler].animationState = 1;
        break;
    case 1:
        if (gBattleHealthBoxInfo[gActiveBattler].specialAnimActive == 0)
        {
            SetBattlerSpriteAffineMode(0);
            DoMoveAnim(move);
            gBattleHealthBoxInfo[gActiveBattler].animationState = 2;
        }
        break;
    case 2:
        gAnimScriptCallback();
        if (!gAnimScriptActive)
        {
            SetBattlerSpriteAffineMode(1);
            if (gBattleSpriteInfo[gActiveBattler].behindSubstitute == 1 && multihit < 2)
            {
                InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 6);
                gBattleSpriteInfo[gActiveBattler].flag_x8 = 0;
            }
            gBattleHealthBoxInfo[gActiveBattler].animationState = 3;
        }
        break;
    case 3:
        if (gBattleHealthBoxInfo[gActiveBattler].specialAnimActive == 0)
        {
            CopyAllBattleSpritesInvisibilities();
            TrySetBehindSubstituteSpriteBit(gActiveBattler, gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            PlayerBufferExecCompleted();
        }
        break;
    }
}

void PlayerHandlePrintString(void)
{
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    BufferStringBattle(*(u16 *)&gBattleBufferA[gActiveBattler][2]);
    Contest_StartTextPrinter(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 0x90, 2, 15);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnInactiveTextPrinter2;
}

void PlayerHandlePrintSelectionString(void)
{
    if (GetBattlerSide(gActiveBattler) == 0)
        PlayerHandlePrintString();
    else
        PlayerBufferExecCompleted();
}

void PlayerHandleChooseAction(void)
{
    int i;

    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 160;
    Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 10, 2, 15, 27, 18);
    Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 10, 2, 35, 16, 38);

    gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseAction;

    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_MenuOptions, 400, 18, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    CreateOutlineCursor(0, 0xFFFF, 12, 11679, 0);

    for (i = 0; i < 4; i++)
        ActionSelectionDestroyCursorAt(i);

    ActionSelectionCreateCursorAt(gActionSelectionCursor[gActiveBattler], 0);

    BattleStringExpandPlaceholdersToDisplayedString(BattleText_OtherMenu);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, PLAYER_ACTION_PROMPT_TILE_OFFSET, 2, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
}

void PlayerHandleYesNoBox()
{
}

void PlayerHandleChooseMove(void)
{
    CreateOutlineCursor(0, 0xFFFF, 12, 0x2D9F, 0);
    InitMoveSelectionsVarsAndStrings();
    gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseMove;
}

void InitMoveSelectionsVarsAndStrings(void)
{
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 320;
    MoveSelectionDisplayMoveNames();
    gMultiUsePlayerCursor = 0xFF;
    MoveSelectionCreateCursorAt(gMoveSelectionCursor[gActiveBattler], 0);
    if (gBattleBufferA[gActiveBattler][2] != 1)
    {
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_PP, 656, 23, 55);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    }
    MoveSelectionDisplayPPNumber();
    MoveSelectionDisplayMoveType();
}

void PlayerHandleChooseItem(void)
{
    s32 i;

    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gBattlerControllerFuncs[gActiveBattler] = OpenBagAndChooseItem;
    gBattlerInMenuId = gActiveBattler;
    for (i = 0; i < 3; i++)
        gBattlePartyCurrentOrder[i] = gBattleBufferA[gActiveBattler][1 + i];
}

void PlayerHandleChoosePokemon(void)
{
    s32 i;

    gBattleControllerData[gActiveBattler] = CreateTask(TaskDummy, 0xFF);
    gTasks[gBattleControllerData[gActiveBattler]].data[0] = gBattleBufferA[gActiveBattler][1] & 0xF;
    gBattleStruct->battlerPreventingSwitchout = gBattleBufferA[gActiveBattler][1] >> 4;
    gBattleStruct->prevSelectedPartySlot = gBattleBufferA[gActiveBattler][2];
    gBattleStruct->abilityPreventingSwitchout = gBattleBufferA[gActiveBattler][3];
    for (i = 0; i < 3; i++)
        gBattlePartyCurrentOrder[i] = gBattleBufferA[gActiveBattler][4 + i];
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gBattlerControllerFuncs[gActiveBattler] = OpenPartyMenuToChooseMon;
    gBattlerInMenuId = gActiveBattler;
}

void PlayerHandleCmd23(void)
{
    BattleStopLowHpSound();
    BeginNormalPaletteFade(0xFFFFFFFF, 2, 0, 16, RGB(0, 0, 0));
    PlayerBufferExecCompleted();
}

void PlayerHandleHealthBarUpdate(void)
{
    s16 hpVal;

    LoadBattleBarGfx(0);
    hpVal = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);
    if (hpVal != 0x7FFF)
    {
        u32 maxHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);
        u32 curHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, curHP, hpVal);
    }
    else
    {
        u32 maxHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, 0, hpVal);
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], 0, 0);
    }
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnHealthbarDone;
}

void PlayerHandleExpUpdate(void)
{
    u8 monId = gBattleBufferA[gActiveBattler][1];

    if (GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL) >= 100)
    {
        PlayerBufferExecCompleted();
    }
    else
    {
        u16 expPointsToGive;
        u8 taskId;

        LoadBattleBarGfx(1);
        GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);  // unused return value
        expPointsToGive = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);
        taskId = CreateTask(Task_GiveExpToMon, 10);
        gTasks[taskId].tExpTask_monId = monId;
        gTasks[taskId].tExpTask_gainedExp = expPointsToGive;
        gTasks[taskId].tExpTask_battler = gActiveBattler;
        gBattlerControllerFuncs[gActiveBattler] = BattleControllerDummy;
    }
}

#undef tExpTask_monId
#undef tExpTask_gainedExp
#undef tExpTask_battler
#undef tExpTask_frames

void PlayerHandleStatusIconUpdate(void)
{
    if (!IsBattleSEPlaying(gActiveBattler))
    {
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 9);
        gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

void PlayerHandleStatusAnimation(void)
{
    if (!IsBattleSEPlaying(gActiveBattler))
    {
        InitAndLaunchChosenStatusAnimation(
          gBattleBufferA[gActiveBattler][1],
          gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8) | (gBattleBufferA[gActiveBattler][4] << 16) | (gBattleBufferA[gActiveBattler][5] << 24));
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

void PlayerHandleStatusXor(void)
{
    u8 val = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_STATUS) ^ gBattleBufferA[gActiveBattler][1];

    SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_STATUS, &val);
    PlayerBufferExecCompleted();
}

void PlayerHandleDataTransfer(void)
{
    PlayerBufferExecCompleted();
}

void PlayerHandleDMA3Transfer(void)
{
    u32 dstArg = gBattleBufferA[gActiveBattler][1]
            | (gBattleBufferA[gActiveBattler][2] << 8)
            | (gBattleBufferA[gActiveBattler][3] << 16)
            | (gBattleBufferA[gActiveBattler][4] << 24);
    u16 sizeArg = gBattleBufferA[gActiveBattler][5] | (gBattleBufferA[gActiveBattler][6] << 8);

    Dma3CopyLarge16_(&gBattleBufferA[gActiveBattler][7], (u8 *)dstArg, sizeArg);
    PlayerBufferExecCompleted();
}

void PlayerHandlePlayBGM(void)
{
    PlayBGM(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    PlayerBufferExecCompleted();
}

void PlayerHandleCmd32(void)
{
    PlayerBufferExecCompleted();
}

void PlayerHandleTwoReturnValues(void)
{
    BtlController_EmitTwoReturnValues(1, 0, 0);
    PlayerBufferExecCompleted();
}

void PlayerHandleChosenMonReturnValue(void)
{
    BtlController_EmitChosenMonReturnValue(1, 0, 0);
    PlayerBufferExecCompleted();
}

void PlayerHandleOneReturnValue(void)
{
    BtlController_EmitOneReturnValue(1, 0);
    PlayerBufferExecCompleted();
}

void PlayerHandleOneReturnValue_Duplicate(void)
{
    BtlController_EmitOneReturnValue_Duplicate(1, 0);
    PlayerBufferExecCompleted();
}

void PlayerHandleClearUnkVar(void)
{
    gUnusedControllerStruct.unk = 0;
    PlayerBufferExecCompleted();
}

void PlayerHandleSetUnkVar(void)
{
    gUnusedControllerStruct.unk = gBattleBufferA[gActiveBattler][1];
    PlayerBufferExecCompleted();
}

void PlayerHandleClearUnkFlag(void)
{
    gUnusedControllerStruct.flag = 0;
    PlayerBufferExecCompleted();
}

void PlayerHandleToggleUnkFlag(void)
{
    gUnusedControllerStruct.flag ^= 1;
    PlayerBufferExecCompleted();
}

void PlayerHandleHitAnimation(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].invisible == TRUE)
    {
        PlayerBufferExecCompleted();
    }
    else
    {
        gDoingBattleAnim = 1;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
        DoHitAnimHealthboxEffect(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = DoHitAnimBlinkSpriteEffect;
    }
}

void PlayerHandleCantSwitch(void)
{
    PlayerBufferExecCompleted();
}

void PlayerHandlePlaySE(void)
{
    s8 pan;

    if (GetBattlerSide(gActiveBattler) == 0)
        pan = -64;
    else
        pan = 63;
    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    PlayerBufferExecCompleted();
}

void PlayerHandlePlayFanfareOrBGM(void)
{
    PlayFanfare(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    PlayerBufferExecCompleted();
}

void PlayerHandleFaintingCry(void)
{
    u16 species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

    PlayCry_ByMode(species, -25, 5);
    PlayerBufferExecCompleted();
}

void PlayerHandleIntroSlide(void)
{
    HandleIntroSlide(gBattleBufferA[gActiveBattler][1]);
    gIntroSlideFlags |= 1;
    PlayerBufferExecCompleted();
}

void PlayerHandleIntroTrainerBallThrow(void)
{
    u8 paletteNum;
    u8 taskId;

    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 50;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = -40;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[5] = gActiveBattler;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCB_FreePlayerSpriteLoadMonSprite);
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], 1);
    paletteNum = AllocSpritePalette(0xD6F8);
    LoadCompressedPalette(gTrainerBackPicPaletteTable[gSaveBlock2.playerGender].data, 0x100 + paletteNum * 16, 32);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = paletteNum;
    taskId = CreateTask(Task_StartSendOutAnim, 5);
    gTasks[taskId].data[0] = gActiveBattler;
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    gBattleHealthBoxInfo[4].introEndDelay |= 1;
    gBattlerControllerFuncs[gActiveBattler] = BattleControllerDummy;
}

void SpriteCB_FreePlayerSpriteLoadMonSprite(struct Sprite *sprite)
{
    u8 battler = sprite->data[5];

    FreeSpriteOamMatrix(sprite);
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(sprite->oam.paletteNum));
    DestroySprite(sprite);
    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[battler]], battler);
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[battler]], 0);
}

void Task_StartSendOutAnim(u8 taskId)
{
    if (gTasks[taskId].data[1] < 31)
    {
        gTasks[taskId].data[1]++;
    }
    else
    {
        u8 savedActiveBattler = gActiveBattler;

        gActiveBattler = gTasks[taskId].data[0];
        if (!IsDoubleBattle() || (gBattleTypeFlags & 0x40))
        {
            gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
            StartSendOutAnim(gActiveBattler, 0);
        }
        else
        {
            gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
            StartSendOutAnim(gActiveBattler, 0);
            gActiveBattler ^= 2;
            gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
            BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
            StartSendOutAnim(gActiveBattler, 0);
            gActiveBattler ^= 2;
        }
        gBattlerControllerFuncs[gActiveBattler] = Intro_TryShinyAnimShowHealthbox;
        gActiveBattler = savedActiveBattler;
        DestroyTask(taskId);
    }
}

void PlayerHandleDrawPartyStatusSummary(void)
{
    if (gBattleBufferA[gActiveBattler][1] != 0 && GetBattlerSide(gActiveBattler) == 0)
    {
        PlayerBufferExecCompleted();
    }
    else
    {
        gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown = 1;
        gBattlerStatusSummaryTaskId[gActiveBattler] = CreatePartyStatusSummarySprites(gActiveBattler, (struct HpAndStatus *)&gBattleBufferA[gActiveBattler][4], gBattleBufferA[gActiveBattler][1], gBattleBufferA[gActiveBattler][2]);
        gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer = 0;
        if (gBattleBufferA[gActiveBattler][2] != 0)
            gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer = 0x5D;
        gBattlerControllerFuncs[gActiveBattler] = EndDrawPartyStatusSummary;
    }
}

void EndDrawPartyStatusSummary(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer++ > 0x5C)
    {
        gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer = 0;
        PlayerBufferExecCompleted();
    }
}

void PlayerHandleHidePartyStatusSummary(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    PlayerBufferExecCompleted();
}

void PlayerHandleEndBounceEffect(void)
{
    EndBounceEffect(gActiveBattler, 1);
    EndBounceEffect(gActiveBattler, 0);
    PlayerBufferExecCompleted();
}

void PlayerHandleSpriteInvisibility(void)
{
    if (IsBattlerSpritePresent(gActiveBattler))
    {
        gSprites[gBattlerSpriteIds[gActiveBattler]].invisible = gBattleBufferA[gActiveBattler][1];
        CopyBattleSpriteInvisibility(gActiveBattler);
    }
    PlayerBufferExecCompleted();
}

void PlayerHandleBattleAnimation(void)
{
    if (!IsBattleSEPlaying(gActiveBattler))
    {
        u8 animationId = gBattleBufferA[gActiveBattler][1];
        u16 argument = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

        if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, animationId, argument))
            PlayerBufferExecCompleted();
        else
            gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedBattleAnimation;
    }
}

void PlayerHandleLinkStandbyMsg(void)
{
    switch (gBattleBufferA[gActiveBattler][1])
    {
    case 0:
        PrintLinkStandbyMsg();
        // fall through
    case 1:
        EndBounceEffect(gActiveBattler, 1);
        EndBounceEffect(gActiveBattler, 0);
        break;
    case 2:
        PrintLinkStandbyMsg();
        break;
    }
    PlayerBufferExecCompleted();
}

void PlayerHandleResetActionMoveSelection(void)
{
    switch (gBattleBufferA[gActiveBattler][1])
    {
    case 0:
        gActionSelectionCursor[gActiveBattler] = 0;
        gMoveSelectionCursor[gActiveBattler] = 0;
        break;
    case 1:
        gActionSelectionCursor[gActiveBattler] = 0;
        break;
    case 2:
        gMoveSelectionCursor[gActiveBattler] = 0;
        break;
    }
    PlayerBufferExecCompleted();
}

void PlayerHandleEndLinkBattle(void)
{
    gBattleOutcome = gBattleBufferA[gActiveBattler][1];
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    PlayerBufferExecCompleted();
    gBattlerControllerFuncs[gActiveBattler] = SetBattleEndCallbacks;
}

void PlayerCmdEnd(void)
{
}
