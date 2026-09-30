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
extern u8 DoPokeballSendOutAnimation();
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
void MoveSelectionDisplayMoveName(s32, const u8 *);
void MoveSelectionDisplayMoveNames(void);
void MoveSelectionDisplayPPNumber(void);
void MoveSelectionDisplayMoveType(void);
void MoveSelectionCreateCursorAt(u8, int);
void MoveSelectionDestroyCursorAt(u8);
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
                // Add item to bag if it is a ball
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
    u8 arr[4] = {0, 2, 3, 1};
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
            u8 var = GetBattlerPosition(gMultiUsePlayerCursor);

            for (i = 0; i < 4; i++)
            {
                if (var == arr[i])
                    break;
            }
            do
            {
                i--;
                if (i < 0)
                    i = 3;
                gMultiUsePlayerCursor = GetBattlerAtPosition(arr[i]);
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
            u8 var = GetBattlerPosition(gMultiUsePlayerCursor);

            for (i = 0; i < 4; i++)
            {
                if (var == arr[i])
                    break;
            }
            do
            {
                i++;
                if (i > 3)
                    i = 0;
                gMultiUsePlayerCursor = GetBattlerAtPosition(arr[i]);
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
    u32 r8 = 0;
#if DEBUG
    u8 count = 0;
#endif
    struct ChooseMoveStruct *r6 = (struct ChooseMoveStruct *)(gBattleBufferA[gActiveBattler] + 4);

    if (JOY_NEW(A_BUTTON))
    {
        u32 r4;

        PlaySE(SE_SELECT);

        if (r6->moves[gMoveSelectionCursor[gActiveBattler]] == MOVE_CURSE)
        {
            if (r6->monType1 != TYPE_GHOST && r6->monType2 != TYPE_GHOST)
                r4 = MOVE_TARGET_USER;
            else
                r4 = MOVE_TARGET_SELECTED;
        }
        else
            r4 = gBattleMoves[r6->moves[gMoveSelectionCursor[gActiveBattler]]].target;

        if (r4 & 0x10)
            gMultiUsePlayerCursor = gActiveBattler;
        else
            gMultiUsePlayerCursor = GetBattlerAtPosition((GetBattlerPosition(gActiveBattler) & 1) ^ 1);

        if (gBattleBufferA[gActiveBattler][1] == 0)
        {
            if ((r4 & 2) && gBattleBufferA[gActiveBattler][2] == 0)
                r8++;
        }
        else
        {
            if (!(r4 & 0x7D))
                r8++;
            if (r6->currentPp[gMoveSelectionCursor[gActiveBattler]] == 0)
            {
                r8 = 0;
            }
            else if (!(r4 & 0x12) && CountAliveMons(0) <= 1)
            {
                gMultiUsePlayerCursor = GetDefaultMoveTarget(gActiveBattler);
                r8 = 0;
            }
        }
        if (r8 == 0)
        {
            DestroyMenuCursor();
            BtlController_EmitTwoReturnValues(1, 10, gMoveSelectionCursor[gActiveBattler] | (gMultiUsePlayerCursor << 8));
            PlayerBufferExecCompleted();
        }
        else
        {
            gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseTarget;
            if (r4 & 0x12)
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
        u8 pp[4];
        u8 filler18[8];  // what is this?
    } sp0;
    //struct ChooseMoveStruct sp0;
    u8 totalPPBonuses;

    if (JOY_NEW(A_BUTTON | SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gMoveSelectionCursor[gActiveBattler] != gMultiUsePlayerCursor)
        {
            struct ChooseMoveStruct *r9 = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
            s32 i;

            i = r9->moves[gMoveSelectionCursor[gActiveBattler]];
            r9->moves[gMoveSelectionCursor[gActiveBattler]] = r9->moves[gMultiUsePlayerCursor];
            r9->moves[gMultiUsePlayerCursor] = i;

            i = r9->currentPp[gMoveSelectionCursor[gActiveBattler]];
            r9->currentPp[gMoveSelectionCursor[gActiveBattler]] = r9->currentPp[gMultiUsePlayerCursor];
            r9->currentPp[gMultiUsePlayerCursor] = i;

            i = r9->maxPp[gMoveSelectionCursor[gActiveBattler]];
            r9->maxPp[gMoveSelectionCursor[gActiveBattler]] = r9->maxPp[gMultiUsePlayerCursor];
            r9->maxPp[gMultiUsePlayerCursor] = i;

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
                gBattleMons[gActiveBattler].moves[i] = r9->moves[i];
                gBattleMons[gActiveBattler].pp[i] = r9->currentPp[i];
            }
            if (!(gBattleMons[gActiveBattler].status2 & STATUS2_TRANSFORMED))
            {
                for (i = 0; i < 4; i++)
                {
                    sp0.moves[i] = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1 + i);
                    sp0.pp[i] = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_PP1 + i);
                }

                totalPPBonuses = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_PP_BONUSES);
                for (i = 0; i < 4; i++)
                    perMovePPBonuses[i] = (totalPPBonuses & (3 << (i * 2))) >> (i * 2);

                i = sp0.moves[gMoveSelectionCursor[gActiveBattler]];
                sp0.moves[gMoveSelectionCursor[gActiveBattler]] = sp0.moves[gMultiUsePlayerCursor];
                sp0.moves[gMultiUsePlayerCursor] = i;

                i = sp0.pp[gMoveSelectionCursor[gActiveBattler]];
                sp0.pp[gMoveSelectionCursor[gActiveBattler]] = sp0.pp[gMultiUsePlayerCursor];
                sp0.pp[gMultiUsePlayerCursor] = i;

                totalPPBonuses = perMovePPBonuses[gMoveSelectionCursor[gActiveBattler]];
                perMovePPBonuses[gMoveSelectionCursor[gActiveBattler]] = perMovePPBonuses[gMultiUsePlayerCursor];
                perMovePPBonuses[gMultiUsePlayerCursor] = totalPPBonuses;

                totalPPBonuses = 0;
                for (i = 0; i < 4; i++)
                    totalPPBonuses |= perMovePPBonuses[i] << (i * 2);

                for (i = 0; i < 4; i++)
                {
                    SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MOVE1 + i, &sp0.moves[i]);
                    SetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_PP1 + i, &sp0.pp[i]);
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
    if (--gBattleHealthBoxInfo[gActiveBattler].unk9 == 0xFF)
    {
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 0;
        PlayerBufferExecCompleted();
    }
}

void Intro_WaitForShinyAnimAndHealthbox(void)
{
    bool8 r6 = FALSE;

    if (!IsDoubleBattle() || (IsDoubleBattle() && (gBattleTypeFlags & BATTLE_TYPE_MULTI)))
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
            r6 = TRUE;
    }
    else
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy
         && gSprites[gHealthboxSpriteIds[gActiveBattler ^ 2]].callback == SpriteCallbackDummy)
            r6 = TRUE;
    }
    if (IsCryPlayingOrClearCrySongs())
        r6 = FALSE;

    if (r6 && gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim && gBattleHealthBoxInfo[gActiveBattler ^ 2].finishedShinyMonAnim)
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
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 3;
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
    s16 r4 = MoveBattleBar(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], 0, 0);

    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
    if (r4 != -1)
    {
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], r4, 0);
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

// Rare Candy usage, maybe?
void Task_GiveExpToMon(u8 taskId)
{
    u32 pkmnIndex = (u8)gTasks[taskId].data[0];
    u8 bank = gTasks[taskId].data[2];
    s16 gainedExp = gTasks[taskId].data[1];

    if (IsDoubleBattle() == TRUE || pkmnIndex != gBattlerPartyIndexes[bank])
    {
        struct Pokemon *pkmn = &gPlayerParty[pkmnIndex];
        u16 species = GetMonData(pkmn, MON_DATA_SPECIES);
        u8 level = GetMonData(pkmn, MON_DATA_LEVEL);
        u32 currExp = GetMonData(pkmn, MON_DATA_EXP);
        u32 nextLvlExp = gExperienceTables[gBaseStats[species].growthRate][level + 1];

        if (currExp + gainedExp >= nextLvlExp)
        {
            u8 savedActiveBank;

            SetMonData(pkmn, MON_DATA_EXP, &nextLvlExp);
            CalculateMonStats(pkmn);
            gainedExp -= nextLvlExp - currExp;
            savedActiveBank = gActiveBattler;
            gActiveBattler = bank;
            BtlController_EmitTwoReturnValues(1, 11, gainedExp);
            gActiveBattler = savedActiveBank;

            if (IsDoubleBattle() == TRUE
             && ((u16)pkmnIndex == gBattlerPartyIndexes[bank] || (u16)pkmnIndex == gBattlerPartyIndexes[bank ^ 2]))
                gTasks[taskId].func = Task_LaunchLvlUpAnim;
            else
                gTasks[taskId].func = DestroyExpTaskAndCompleteOnInactiveTextPrinter;
        }
        else
        {
            currExp += gainedExp;
            SetMonData(pkmn, MON_DATA_EXP, &currExp);
            gBattlerControllerFuncs[bank] = CompleteOnInactiveTextPrinter;
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
    u8 pkmnIndex = gTasks[taskId].data[0];
    s32 r9 = gTasks[taskId].data[1];
    u8 bank = gTasks[taskId].data[2];
    struct Pokemon *pkmn = &gPlayerParty[pkmnIndex];
    u8 level = GetMonData(pkmn, MON_DATA_LEVEL);
    u16 species = GetMonData(pkmn, MON_DATA_SPECIES);
    u32 exp = GetMonData(pkmn, MON_DATA_EXP);
    u32 currLvlExp = gExperienceTables[gBaseStats[species].growthRate][level];
    u32 expToNextLvl;

    exp -= currLvlExp;
    expToNextLvl = gExperienceTables[gBaseStats[species].growthRate][level + 1] - currLvlExp;
    SetBattleBarStruct(bank, gHealthboxSpriteIds[bank], expToNextLvl, exp, -r9);
    PlaySE(SE_EXP);
    gTasks[taskId].func = Task_GiveExpWithExpBar;
}

void Task_GiveExpWithExpBar(u8 taskId)
{
    if (gTasks[taskId].data[10] < 13)
    {
        gTasks[taskId].data[10]++;
    }
    else
    {
        u8 monId = gTasks[taskId].data[0];
        s16 gainedExp = gTasks[taskId].data[1];
        u8 battlerId = gTasks[taskId].data[2];
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
    u8 bank = gTasks[taskId].data[2];
    u8 pkmnIndex = gTasks[taskId].data[0];

    if (IsDoubleBattle() == TRUE && pkmnIndex == gBattlerPartyIndexes[bank ^ 2])
        bank ^= 2;
    InitAndLaunchSpecialAnimation(bank, bank, bank, 0);
    gTasks[taskId].func = Task_UpdateLvlInHealthbox;
}

void Task_UpdateLvlInHealthbox(u8 taskId)
{
    u8 bank = gTasks[taskId].data[2];

    if (!gBattleHealthBoxInfo[bank].specialAnimActive)
    {
        u8 pkmnIndex = gTasks[taskId].data[0];

        GetMonData(&gPlayerParty[pkmnIndex], MON_DATA_LEVEL);  // Unused return value
        if (IsDoubleBattle() == TRUE && pkmnIndex == gBattlerPartyIndexes[bank ^ 2])
            UpdateHealthboxAttribute(gHealthboxSpriteIds[bank ^ 2], &gPlayerParty[pkmnIndex], 0);
        else
            UpdateHealthboxAttribute(gHealthboxSpriteIds[bank], &gPlayerParty[pkmnIndex], 0);
        gTasks[taskId].func = DestroyExpTaskAndCompleteOnInactiveTextPrinter;
    }
}

void DestroyExpTaskAndCompleteOnInactiveTextPrinter(u8 taskId)
{
    u8 pkmnIndex;
    u8 bank;

    pkmnIndex = gTasks[taskId].data[0];
    GetMonData(&gPlayerParty[pkmnIndex], MON_DATA_LEVEL);  // Unused return value
    bank = gTasks[taskId].data[2];
    gBattlerControllerFuncs[bank] = CompleteOnInactiveTextPrinter;
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
        u8 r4;

        gBattlerControllerFuncs[gActiveBattler] = WaitForMonSelection;
        r4 = gTasks[gBattleControllerData[gActiveBattler]].data[0];
        DestroyTask(gBattleControllerData[gActiveBattler]);
        OpenBattlePartyMenuWithAction(r4);
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

void MoveSelectionDisplayMoveName(s32 a, const u8 *b)
{
    struct ChooseMoveStruct *r4 = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];

    StringCopy(gDisplayedStringBattle, b);
    StringAppend(gDisplayedStringBattle, gMoveNames[r4->moves[a]]);
    Text_InitWindow(
      &gWindowTemplate_Contest_MoveDescription,
      gDisplayedStringBattle,
      0x300 + a * 20,
      (a & 1) ? 11 : 1,
      (a < 2) ? 0x37 : 0x39);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
}

void MoveSelectionDisplayMoveNames(void)
{
    struct ChooseMoveStruct *r4 = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
    s32 i;

    gNumberOfMovesToChoose = 0;
    Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 0x1016, 1, 0x37, 0x14, 0x3A);
    for (i = 0; i < 4; i++)
    {
        MoveSelectionDestroyCursorAt(i);
        MoveSelectionDisplayMoveName(i, BattleText_Format);
        if (r4->moves[i] != 0)
            gNumberOfMovesToChoose++;
    }
}

void MoveSelectionDisplayPPNumber(void)
{
    if (gBattleBufferA[gActiveBattler][2] != 1)
    {
        struct ChooseMoveStruct *r4 = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
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
        str = ConvertIntToDecimalStringN(str, r4->currentPp[gMoveSelectionCursor[gActiveBattler]], 1, 2);
        *str++ = CHAR_SLASH;
        ConvertIntToDecimalStringN(str, r4->maxPp[gMoveSelectionCursor[gActiveBattler]], 1, 2);
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
        struct ChooseMoveStruct *r4 = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
        u8 *str = gDisplayedStringBattle;

        str = StringCopy(str, BattleText_Format);
        StringCopy(str, gTypeNames[gBattleMoves[r4->moves[gMoveSelectionCursor[gActiveBattler]]].type]);
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

void MoveSelectionCreateCursorAt(u8 a, int unused)
{
    SetOutlineCursorWidth(0x48);
    SetOutlineCursorPosition(sMoveSelectionCursorCoords[a][0], sMoveSelectionCursorCoords[a][1]);
}

void MoveSelectionDestroyCursorAt(u8 a)
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
    u8 unkData[0x100];
    u32 offset = 0;
    u8 r4;
    s32 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        offset += CopyPlayerMonData(gBattlerPartyIndexes[gActiveBattler], unkData);
    }
    else
    {
        r4 = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (r4 & 1)
                offset += CopyPlayerMonData(i, unkData + offset);
            r4 >>= 1;
        }
    }
    BtlController_EmitDataTransfer(1, offset, unkData);
    PlayerBufferExecCompleted();
}

// Duplicate of the Link Partner mon-data copy routine.
u32 CopyPlayerMonData(u8 a, u8 *buffer)
{
    struct BattlePokemon battlePokemon;
    struct MovePPInfo moveData;
    u8 nickname[20];
    u8 *src;
    s16 data16;
    u32 data32;
    s32 size = 0;

    switch (gBattleBufferA[gActiveBattler][1])
    {
    case 0:
        battlePokemon.species = GetMonData(&gPlayerParty[a], MON_DATA_SPECIES);
        battlePokemon.item = GetMonData(&gPlayerParty[a], MON_DATA_HELD_ITEM);
        for (size = 0; size < 4; size++)
        {
            battlePokemon.moves[size] = GetMonData(&gPlayerParty[a], MON_DATA_MOVE1 + size);
            battlePokemon.pp[size] = GetMonData(&gPlayerParty[a], MON_DATA_PP1 + size);
        }
        battlePokemon.ppBonuses = GetMonData(&gPlayerParty[a], MON_DATA_PP_BONUSES);
        battlePokemon.friendship = GetMonData(&gPlayerParty[a], MON_DATA_FRIENDSHIP);
        battlePokemon.experience = GetMonData(&gPlayerParty[a], MON_DATA_EXP);
        battlePokemon.hpIV = GetMonData(&gPlayerParty[a], MON_DATA_HP_IV);
        battlePokemon.attackIV = GetMonData(&gPlayerParty[a], MON_DATA_ATK_IV);
        battlePokemon.defenseIV = GetMonData(&gPlayerParty[a], MON_DATA_DEF_IV);
        battlePokemon.speedIV = GetMonData(&gPlayerParty[a], MON_DATA_SPEED_IV);
        battlePokemon.spAttackIV = GetMonData(&gPlayerParty[a], MON_DATA_SPATK_IV);
        battlePokemon.spDefenseIV = GetMonData(&gPlayerParty[a], MON_DATA_SPDEF_IV);
        battlePokemon.personality = GetMonData(&gPlayerParty[a], MON_DATA_PERSONALITY);
        battlePokemon.status1 = GetMonData(&gPlayerParty[a], MON_DATA_STATUS);
        battlePokemon.level = GetMonData(&gPlayerParty[a], MON_DATA_LEVEL);
        battlePokemon.hp = GetMonData(&gPlayerParty[a], MON_DATA_HP);
        battlePokemon.maxHP = GetMonData(&gPlayerParty[a], MON_DATA_MAX_HP);
        battlePokemon.attack = GetMonData(&gPlayerParty[a], MON_DATA_ATK);
        battlePokemon.defense = GetMonData(&gPlayerParty[a], MON_DATA_DEF);
        battlePokemon.speed = GetMonData(&gPlayerParty[a], MON_DATA_SPEED);
        battlePokemon.spAttack = GetMonData(&gPlayerParty[a], MON_DATA_SPATK);
        battlePokemon.spDefense = GetMonData(&gPlayerParty[a], MON_DATA_SPDEF);
        battlePokemon.isEgg = GetMonData(&gPlayerParty[a], MON_DATA_IS_EGG);
        battlePokemon.altAbility = GetMonData(&gPlayerParty[a], MON_DATA_ALT_ABILITY);
        battlePokemon.otId = GetMonData(&gPlayerParty[a], MON_DATA_OT_ID);
        GetMonData(&gPlayerParty[a], MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(battlePokemon.nickname, nickname);
        GetMonData(&gPlayerParty[a], MON_DATA_OT_NAME, battlePokemon.otName);
        MEMCPY_ALT(&battlePokemon, buffer, sizeof(battlePokemon), size, src);
        break;
    case 1:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_SPECIES);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 2:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_HELD_ITEM);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 3:
        for (size = 0; size < 4; size++)
        {
            moveData.moves[size] = GetMonData(&gPlayerParty[a], MON_DATA_MOVE1 + size);
            moveData.pp[size] = GetMonData(&gPlayerParty[a], MON_DATA_PP1 + size);
        }
        moveData.ppBonuses = GetMonData(&gPlayerParty[a], MON_DATA_PP_BONUSES);
        MEMCPY_ALT(&moveData, buffer, sizeof(moveData), size, src);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - 4);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 8:
        for (size = 0; size < 4; size++)
            buffer[size] = GetMonData(&gPlayerParty[a], MON_DATA_PP1 + size);
        buffer[size] = GetMonData(&gPlayerParty[a], MON_DATA_PP_BONUSES);
        size++;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - 9);
        size = 1;
        break;
    case 17:
        data32 = GetMonData(&gPlayerParty[a], MON_DATA_OT_ID);
        buffer[0] = (data32 & 0x000000FF);
        buffer[1] = (data32 & 0x0000FF00) >> 8;
        buffer[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case 18:
        data32 = GetMonData(&gPlayerParty[a], MON_DATA_EXP);
        buffer[0] = (data32 & 0x000000FF);
        buffer[1] = (data32 & 0x0000FF00) >> 8;
        buffer[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case 19:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_HP_EV);
        size = 1;
        break;
    case 20:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_ATK_EV);
        size = 1;
        break;
    case 21:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_DEF_EV);
        size = 1;
        break;
    case 22:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SPEED_EV);
        size = 1;
        break;
    case 23:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SPATK_EV);
        size = 1;
        break;
    case 24:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SPDEF_EV);
        size = 1;
        break;
    case 25:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_FRIENDSHIP);
        size = 1;
        break;
    case 26:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_POKERUS);
        size = 1;
        break;
    case 27:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_MET_LOCATION);
        size = 1;
        break;
    case 28:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_MET_LEVEL);
        size = 1;
        break;
    case 29:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_MET_GAME);
        size = 1;
        break;
    case 30:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_POKEBALL);
        size = 1;
        break;
    case 31:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_HP_IV);
        buffer[1] = GetMonData(&gPlayerParty[a], MON_DATA_ATK_IV);
        buffer[2] = GetMonData(&gPlayerParty[a], MON_DATA_DEF_IV);
        buffer[3] = GetMonData(&gPlayerParty[a], MON_DATA_SPEED_IV);
        buffer[4] = GetMonData(&gPlayerParty[a], MON_DATA_SPATK_IV);
        buffer[5] = GetMonData(&gPlayerParty[a], MON_DATA_SPDEF_IV);
        size = 6;
        break;
    case 32:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_HP_IV);
        size = 1;
        break;
    case 33:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_ATK_IV);
        size = 1;
        break;
    case 34:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_DEF_IV);
        size = 1;
        break;
    case 35:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SPEED_IV);
        size = 1;
        break;
    case 36:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SPATK_IV);
        size = 1;
        break;
    case 37:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SPDEF_IV);
        size = 1;
        break;
    case 38:
        data32 = GetMonData(&gPlayerParty[a], MON_DATA_PERSONALITY);
        buffer[0] = (data32 & 0x000000FF);
        buffer[1] = (data32 & 0x0000FF00) >> 8;
        buffer[2] = (data32 & 0x00FF0000) >> 16;
        buffer[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case 39:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_CHECKSUM);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 40:
        data32 = GetMonData(&gPlayerParty[a], MON_DATA_STATUS);
        buffer[0] = (data32 & 0x000000FF);
        buffer[1] = (data32 & 0x0000FF00) >> 8;
        buffer[2] = (data32 & 0x00FF0000) >> 16;
        buffer[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case 41:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_LEVEL);
        size = 1;
        break;
    case 42:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_HP);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 43:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_MAX_HP);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 44:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_ATK);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 45:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_DEF);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 46:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_SPEED);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 47:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_SPATK);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 48:
        data16 = GetMonData(&gPlayerParty[a], MON_DATA_SPDEF);
        buffer[0] = data16;
        buffer[1] = data16 >> 8;
        size = 2;
        break;
    case 49:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_COOL);
        size = 1;
        break;
    case 50:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_BEAUTY);
        size = 1;
        break;
    case 51:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_CUTE);
        size = 1;
        break;
    case 52:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SMART);
        size = 1;
        break;
    case 53:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_TOUGH);
        size = 1;
        break;
    case 54:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SHEEN);
        size = 1;
        break;
    case 55:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_COOL_RIBBON);
        size = 1;
        break;
    case 56:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_BEAUTY_RIBBON);
        size = 1;
        break;
    case 57:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_CUTE_RIBBON);
        size = 1;
        break;
    case 58:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_SMART_RIBBON);
        size = 1;
        break;
    case 59:
        buffer[0] = GetMonData(&gPlayerParty[a], MON_DATA_TOUGH_RIBBON);
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
    u8 r4;
    u8 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        SetPlayerMonData(gBattlerPartyIndexes[gActiveBattler]);
    }
    else
    {
        r4 = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (r4 & 1)
                SetPlayerMonData(i);
            r4 >>= 1;
        }
    }
    PlayerBufferExecCompleted();
}

// Duplicate of SetLinkPartnerMonData
void SetPlayerMonData(u8 a)
{
    struct BattlePokemon *battlePokemon = (struct BattlePokemon *)&gBattleBufferA[gActiveBattler][3];
    struct MovePPInfo *moveData = (struct MovePPInfo *)&gBattleBufferA[gActiveBattler][3];
    s32 i;

    switch (gBattleBufferA[gActiveBattler][1])
    {
    case 0:
        {
            u8 iv;

            SetMonData(&gPlayerParty[a], MON_DATA_SPECIES, &battlePokemon->species);
            SetMonData(&gPlayerParty[a], MON_DATA_HELD_ITEM, &battlePokemon->item);
            for (i = 0; i < 4; i++)
            {
                SetMonData(&gPlayerParty[a], MON_DATA_MOVE1 + i, &battlePokemon->moves[i]);
                SetMonData(&gPlayerParty[a], MON_DATA_PP1 + i, &battlePokemon->pp[i]);
            }
            SetMonData(&gPlayerParty[a], MON_DATA_PP_BONUSES, &battlePokemon->ppBonuses);
            SetMonData(&gPlayerParty[a], MON_DATA_FRIENDSHIP, &battlePokemon->friendship);
            SetMonData(&gPlayerParty[a], MON_DATA_EXP, &battlePokemon->experience);
            iv = battlePokemon->hpIV;
            SetMonData(&gPlayerParty[a], MON_DATA_HP_IV, &iv);
            iv = battlePokemon->attackIV;
            SetMonData(&gPlayerParty[a], MON_DATA_ATK_IV, &iv);
            iv = battlePokemon->defenseIV;
            SetMonData(&gPlayerParty[a], MON_DATA_DEF_IV, &iv);
            iv = battlePokemon->speedIV;
            SetMonData(&gPlayerParty[a], MON_DATA_SPEED_IV, &iv);
            iv = battlePokemon->spAttackIV;
            SetMonData(&gPlayerParty[a], MON_DATA_SPATK_IV, &iv);
            iv = battlePokemon->spDefenseIV;
            SetMonData(&gPlayerParty[a], MON_DATA_SPDEF_IV, &iv);
            SetMonData(&gPlayerParty[a], MON_DATA_PERSONALITY, &battlePokemon->personality);
            SetMonData(&gPlayerParty[a], MON_DATA_STATUS, &battlePokemon->status1);
            SetMonData(&gPlayerParty[a], MON_DATA_LEVEL, &battlePokemon->level);
            SetMonData(&gPlayerParty[a], MON_DATA_HP, &battlePokemon->hp);
            SetMonData(&gPlayerParty[a], MON_DATA_MAX_HP, &battlePokemon->maxHP);
            SetMonData(&gPlayerParty[a], MON_DATA_ATK, &battlePokemon->attack);
            SetMonData(&gPlayerParty[a], MON_DATA_DEF, &battlePokemon->defense);
            SetMonData(&gPlayerParty[a], MON_DATA_SPEED, &battlePokemon->speed);
            SetMonData(&gPlayerParty[a], MON_DATA_SPATK, &battlePokemon->spAttack);
            SetMonData(&gPlayerParty[a], MON_DATA_SPDEF, &battlePokemon->spDefense);
        }
        break;
    case 1:
        SetMonData(&gPlayerParty[a], MON_DATA_SPECIES, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 2:
        SetMonData(&gPlayerParty[a], MON_DATA_HELD_ITEM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 3:
        for (i = 0; i < 4; i++)
        {
            SetMonData(&gPlayerParty[a], MON_DATA_MOVE1 + i, &moveData->moves[i]);
            SetMonData(&gPlayerParty[a], MON_DATA_PP1 + i, &moveData->pp[i]);
        }
        SetMonData(&gPlayerParty[a], MON_DATA_PP_BONUSES, &moveData->ppBonuses);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        SetMonData(&gPlayerParty[a], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - 4, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 8:
        SetMonData(&gPlayerParty[a], MON_DATA_PP1, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gPlayerParty[a], MON_DATA_PP2, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gPlayerParty[a], MON_DATA_PP3, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gPlayerParty[a], MON_DATA_PP4, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gPlayerParty[a], MON_DATA_PP_BONUSES, &gBattleBufferA[gActiveBattler][7]);
        break;
    case 9:
    case 10:
    case 11:
    case 12:
        SetMonData(&gPlayerParty[a], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - 9, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 17:
        SetMonData(&gPlayerParty[a], MON_DATA_OT_ID, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 18:
        SetMonData(&gPlayerParty[a], MON_DATA_EXP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 19:
        SetMonData(&gPlayerParty[a], MON_DATA_HP_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 20:
        SetMonData(&gPlayerParty[a], MON_DATA_ATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 21:
        SetMonData(&gPlayerParty[a], MON_DATA_DEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 22:
        SetMonData(&gPlayerParty[a], MON_DATA_SPEED_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 23:
        SetMonData(&gPlayerParty[a], MON_DATA_SPATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 24:
        SetMonData(&gPlayerParty[a], MON_DATA_SPDEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 25:
        SetMonData(&gPlayerParty[a], MON_DATA_FRIENDSHIP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 26:
        SetMonData(&gPlayerParty[a], MON_DATA_POKERUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 27:
        SetMonData(&gPlayerParty[a], MON_DATA_MET_LOCATION, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 28:
        SetMonData(&gPlayerParty[a], MON_DATA_MET_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 29:
        SetMonData(&gPlayerParty[a], MON_DATA_MET_GAME, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 30:
        SetMonData(&gPlayerParty[a], MON_DATA_POKEBALL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 31:
        SetMonData(&gPlayerParty[a], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gPlayerParty[a], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gPlayerParty[a], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gPlayerParty[a], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gPlayerParty[a], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][7]);
        SetMonData(&gPlayerParty[a], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][8]);
        break;
    case 32:
        SetMonData(&gPlayerParty[a], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 33:
        SetMonData(&gPlayerParty[a], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 34:
        SetMonData(&gPlayerParty[a], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 35:
        SetMonData(&gPlayerParty[a], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 36:
        SetMonData(&gPlayerParty[a], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 37:
        SetMonData(&gPlayerParty[a], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 38:
        SetMonData(&gPlayerParty[a], MON_DATA_PERSONALITY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 39:
        SetMonData(&gPlayerParty[a], MON_DATA_CHECKSUM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 40:
        SetMonData(&gPlayerParty[a], MON_DATA_STATUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 41:
        SetMonData(&gPlayerParty[a], MON_DATA_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 42:
        SetMonData(&gPlayerParty[a], MON_DATA_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 43:
        SetMonData(&gPlayerParty[a], MON_DATA_MAX_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 44:
        SetMonData(&gPlayerParty[a], MON_DATA_ATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 45:
        SetMonData(&gPlayerParty[a], MON_DATA_DEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 46:
        SetMonData(&gPlayerParty[a], MON_DATA_SPEED, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 47:
        SetMonData(&gPlayerParty[a], MON_DATA_SPATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 48:
        SetMonData(&gPlayerParty[a], MON_DATA_SPDEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 49:
        SetMonData(&gPlayerParty[a], MON_DATA_COOL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 50:
        SetMonData(&gPlayerParty[a], MON_DATA_BEAUTY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 51:
        SetMonData(&gPlayerParty[a], MON_DATA_CUTE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 52:
        SetMonData(&gPlayerParty[a], MON_DATA_SMART, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 53:
        SetMonData(&gPlayerParty[a], MON_DATA_TOUGH, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 54:
        SetMonData(&gPlayerParty[a], MON_DATA_SHEEN, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 55:
        SetMonData(&gPlayerParty[a], MON_DATA_COOL_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 56:
        SetMonData(&gPlayerParty[a], MON_DATA_BEAUTY_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 57:
        SetMonData(&gPlayerParty[a], MON_DATA_CUTE_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 58:
        SetMonData(&gPlayerParty[a], MON_DATA_SMART_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 59:
        SetMonData(&gPlayerParty[a], MON_DATA_TOUGH_RIBBON, &gBattleBufferA[gActiveBattler][3]);
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

void StartSendOutAnim(u8 bank, u8 b)
{
    u16 species;

    ClearTemporarySpeciesSpriteData(bank, b);
    gBattlerPartyIndexes[bank] = gBattleBufferA[bank][1];
    species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[bank]], MON_DATA_SPECIES);
    gBattleControllerData[bank] = CreateInvisibleSpriteWithCallback(SpriteCB_WaitForBattlerBallReleaseAnim);
    SetMultiuseSpriteTemplateToPokemon(species, GetBattlerPosition(bank));
    gBattlerSpriteIds[bank] = CreateSprite(
      &gCreatingSpriteTemplate,
      GetBattlerSpriteCoord(bank, 2),
      GetBattlerSpriteDefault_Y(bank),
      GetBattlerSpriteSubpriority(bank));
    gSprites[gBattleControllerData[bank]].data[1] = gBattlerSpriteIds[bank];
    gSprites[gBattlerSpriteIds[bank]].data[0] = bank;
    gSprites[gBattlerSpriteIds[bank]].data[2] = species;
    gSprites[gBattlerSpriteIds[bank]].oam.paletteNum = bank;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[bank]], gBattleMonForms[bank]);
    gSprites[gBattlerSpriteIds[bank]].invisible = TRUE;
    gSprites[gBattlerSpriteIds[bank]].callback = SpriteCallbackDummy;
    gSprites[gBattleControllerData[bank]].data[0] = DoPokeballSendOutAnimation(0, 0xFF);
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
    s16 r7;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (GetBattlerPosition(gActiveBattler) & 2)
            r7 = 16;
        else
            r7 = -16;
    }
    else
    {
        r7 = 0;
    }
    DecompressTrainerBackPic(gSaveBlock2.playerGender, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(gSaveBlock2.playerGender, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gCreatingSpriteTemplate,
      r7 + 80,
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
    u8 var = gBattleBufferA[gActiveBattler][1];

    // WTF is this??
    while (var != 0)
        var--;

    PlayerBufferExecCompleted();
}

void PlayerHandleMoveAnimation(void)
{
    if (!IsBattleSEPlaying(gActiveBattler))
    {
        u16 r0 = gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8);

        gAnimMoveTurn = gBattleBufferA[gActiveBattler][3];
        gAnimMovePower = gBattleBufferA[gActiveBattler][4] | (gBattleBufferA[gActiveBattler][5] << 8);
        gAnimMoveDmg = gBattleBufferA[gActiveBattler][6] | (gBattleBufferA[gActiveBattler][7] << 8) | (gBattleBufferA[gActiveBattler][8] << 16) | (gBattleBufferA[gActiveBattler][9] << 24);
        gAnimFriendship = gBattleBufferA[gActiveBattler][10];
        gWeatherMoveAnim = gBattleBufferA[gActiveBattler][12] | (gBattleBufferA[gActiveBattler][13] << 8);
        gAnimDisableStructPtr = (struct DisableStruct *)&gBattleBufferA[gActiveBattler][16];
        gTransformedPersonalities[gActiveBattler] = gAnimDisableStructPtr->transformedMonPersonality;
        if (IsMoveWithoutAnimation(r0, gAnimMoveTurn) != 0)
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
    u16 r4 = gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8);
    u8 r7 = gBattleBufferA[gActiveBattler][11];

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
            DoMoveAnim(r4);
            gBattleHealthBoxInfo[gActiveBattler].animationState = 2;
        }
        break;
    case 2:
        gAnimScriptCallback();
        if (!gAnimScriptActive)
        {
            SetBattlerSpriteAffineMode(1);
            if (gBattleSpriteInfo[gActiveBattler].behindSubstitute == 1 && r7 < 2)
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
    int r4;

    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 160;
    Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 10, 2, 15, 27, 18);
    Text_FillWindowRect(&gWindowTemplate_Contest_MoveDescription, 10, 2, 35, 16, 38);

    gBattlerControllerFuncs[gActiveBattler] = HandleInputChooseAction;

    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, BattleText_MenuOptions, 400, 18, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    CreateOutlineCursor(0, 0xFFFF, 12, 11679, 0);

    for (r4 = 0; r4 < 4; r4++)
        ActionSelectionDestroyCursorAt(r4);

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
    s16 r7;

    LoadBattleBarGfx(0);
    r7 = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);
    if (r7 != 0x7FFF)
    {
        u32 maxHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);
        u32 curHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, curHP, r7);
    }
    else
    {
        u32 maxHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, 0, r7);
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], 0, 0);
    }
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnHealthbarDone;
}

void PlayerHandleExpUpdate(void)
{
    u8 r7 = gBattleBufferA[gActiveBattler][1];

    if (GetMonData(&gPlayerParty[r7], MON_DATA_LEVEL) >= 100)
    {
        PlayerBufferExecCompleted();
    }
    else
    {
        u16 r4;
        u8 taskId;

        LoadBattleBarGfx(1);
        GetMonData(&gPlayerParty[r7], MON_DATA_SPECIES);  // unused return value
        r4 = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);
        taskId = CreateTask(Task_GiveExpToMon, 10);
        gTasks[taskId].data[0] = r7;
        gTasks[taskId].data[1] = r4;
        gTasks[taskId].data[2] = gActiveBattler;
        gBattlerControllerFuncs[gActiveBattler] = BattleControllerDummy;
    }
}

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
    u32 val1 = gBattleBufferA[gActiveBattler][1]
            | (gBattleBufferA[gActiveBattler][2] << 8)
            | (gBattleBufferA[gActiveBattler][3] << 16)
            | (gBattleBufferA[gActiveBattler][4] << 24);
    u16 val2 = gBattleBufferA[gActiveBattler][5] | (gBattleBufferA[gActiveBattler][6] << 8);

    Dma3CopyLarge16_(&gBattleBufferA[gActiveBattler][7], (u8 *)val1, val2);
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

    PlayCry3(species, -25, 5);
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
    gBattleHealthBoxInfo[4].unk9 |= 1;
    gBattlerControllerFuncs[gActiveBattler] = BattleControllerDummy;
}

void SpriteCB_FreePlayerSpriteLoadMonSprite(struct Sprite *sprite)
{
    u8 r4 = sprite->data[5];

    FreeSpriteOamMatrix(sprite);
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(sprite->oam.paletteNum));
    DestroySprite(sprite);
    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[r4]], r4);
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[r4]], 0);
}

void Task_StartSendOutAnim(u8 taskId)
{
    if (gTasks[taskId].data[1] < 31)
    {
        gTasks[taskId].data[1]++;
    }
    else
    {
        u8 savedActiveBank = gActiveBattler;

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
        gActiveBattler = savedActiveBank;
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
        u8 val2 = gBattleBufferA[gActiveBattler][1];
        u16 val = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

        if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, val2, val))
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
