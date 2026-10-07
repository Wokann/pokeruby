#include "global.h"
#include "battle.h"
#include "battle_gfx_sfx_util.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "data.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "palette.h"
#include "pokeball.h"
#include "pokemon.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "util.h"
#include "ewram.h"

extern u8 gActiveBattler;
extern u8 gBattleBufferA[][0x200];
extern u8 gBattlerSpriteIds[];
extern u16 gBattlerPartyIndexes[];
extern u8 gHealthboxSpriteIds[];
extern u16 gBattleTypeFlags;
extern u8 gBattleMonForms[];
extern void (*gBattlerControllerFuncs[])(void);
extern u8 gAnimFriendship;
extern u32 gTransformedPersonalities[];
extern u8 gAnimScriptActive;
extern void (*gAnimScriptCallback)(void);
extern u8 gDisplayedStringBattle[];
extern bool8 gDoingBattleAnim;
extern u8 gBattleOutcome;
extern u16 gIntroSlideFlags;
extern u8 gBattlerStatusSummaryTaskId[];
extern u8 gAnimMoveTurn;
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern u8 gBattleControllerData[];
extern u32 gBattleControllerExecFlags;
extern MainCallback gPreBattleCallback1;
extern struct MusicPlayerInfo gMPlayInfo_BGM;

extern u8 GetBattlerSpriteDefault_Y();
extern u8 GetBattlerPosition(u8);
extern void sub_8037A74(void);
static void SwitchIn_TryShinyAnim(void);
static void FreeMonSpriteAfterSwitchOutAnim(void);
extern s32 GetBattlerMultiplayerId(u16);
static void CompleteOnBattlerSpriteCallbackDummy(void);
extern void SetSpritePrimaryCoordsFromSecondaryCoords();
extern void StartAnimLinearTranslation(struct Sprite *);
extern void sub_80375B4(void);
extern void sub_8010384(struct Sprite *);
static void HideHealthboxAfterMonFaint(void);
extern void BufferStringBattle();
extern void sub_8037C2C(void);
static void CompleteOnHealthbarDone(void);
static void CompleteOnFinishedStatusAnimation(void);
static void DoHitAnimBlinkSpriteEffect(void);
extern u8 GetBattlerSide(u8);
extern void sub_803A3A8(struct Sprite *);
extern void Task_HidePartyStatusSummary(u8);
static void LinkOpponentDummy(void);
extern bool8 IsDoubleBattle(void);
extern void sub_8037840(void);
extern void sub_8031B74();
static void CompleteOnFinishedBattleAnimation(void);
extern void sub_8037F34(void);
static void LinkOpponentBufferExecCompleted(void);

// this file's functions

static u32 CopyLinkOpponentMonData(u8 monId, u8 *dst);
static void LinkOpponentBufferRunCommand(void);
void sub_8037D2C(void);
static void SetLinkOpponentMonData(u8 monId);
void sub_8039430(u8 battler, u8 dontClearSubstituteBit);
static void DoSwitchOutAnimation(void);
static void LinkOpponentDoMoveAnimation(void);
static void Task_StartSendOutAnim(u8 taskId);
static void EndDrawPartyStatusSummary(void);

static void LinkOpponentHandleGetMonData(void);
static void LinkOpponentHandleGetRawMonData(void);
static void LinkOpponentHandleSetMonData(void);
static void LinkOpponentHandleSetRawMonData(void);
static void LinkOpponentHandleLoadMonSprite(void);
static void LinkOpponentHandleSwitchInAnim(void);
static void LinkOpponentHandleReturnMonToBall(void);
void LinkOpponentHandleTrainerThrow(void);
static void LinkOpponentHandleTrainerSlide(void);
static void LinkOpponentHandleTrainerSlideBack(void);
static void LinkOpponentHandleFaintAnimation(void);
static void LinkOpponentHandlePaletteFade(void);
static void LinkOpponentHandleSuccessBallThrowAnim(void);
static void LinkOpponentHandleBallThrowAnim(void);
static void LinkOpponentHandlePause(void);
static void LinkOpponentHandleMoveAnimation(void);
static void LinkOpponentHandlePrintString(void);
static void LinkOpponentHandlePrintSelectionString(void);
static void LinkOpponentHandleChooseAction(void);
static void LinkOpponentHandleYesNoBox(void);
static void LinkOpponentHandleChooseMove(void);
static void LinkOpponentHandleChooseItem(void);
static void LinkOpponentHandleChoosePokemon(void);
static void LinkOpponentHandleCmd23(void);
static void LinkOpponentHandleHealthBarUpdate(void);
static void LinkOpponentHandleExpUpdate(void);
static void LinkOpponentHandleStatusIconUpdate(void);
static void LinkOpponentHandleStatusAnimation(void);
static void LinkOpponentHandleStatusXor(void);
static void LinkOpponentHandleDataTransfer(void);
static void LinkOpponentHandleDMA3Transfer(void);
static void LinkOpponentHandlePlayBGM(void);
static void LinkOpponentHandleCmd32(void);
static void LinkOpponentHandleTwoReturnValues(void);
static void LinkOpponentHandleChosenMonReturnValue(void);
static void LinkOpponentHandleOneReturnValue(void);
static void LinkOpponentHandleOneReturnValue_Duplicate(void);
static void LinkOpponentHandleClearUnkVar(void);
static void LinkOpponentHandleSetUnkVar(void);
static void LinkOpponentHandleClearUnkFlag(void);
static void LinkOpponentHandleToggleUnkFlag(void);
static void LinkOpponentHandleHitAnimation(void);
static void LinkOpponentHandleCantSwitch(void);
static void LinkOpponentHandlePlaySE(void);
void LinkOpponentHandlecmd44(void);
static void LinkOpponentHandleFaintingCry(void);
static void LinkOpponentHandleIntroSlide(void);
static void LinkOpponentHandleIntroTrainerBallThrow(void);
static void LinkOpponentHandleDrawPartyStatusSummary(void);
static void LinkOpponentHandleHidePartyStatusSummary(void);
static void LinkOpponentHandleEndBounceEffect(void);
static void LinkOpponentHandleSpriteInvisibility(void);
static void LinkOpponentHandleBattleAnimation(void);
static void LinkOpponentHandleLinkStandbyMsg(void);
static void LinkOpponentHandleResetActionMoveSelection(void);
void LinkOpponentHandlecmd55(void);
static void LinkOpponentCmdEnd(void);

// const data

typedef void (*BattleBufferCmd) (void);
static const BattleBufferCmd sLinkOpponentBufferCommands[] =
{
    LinkOpponentHandleGetMonData,
    LinkOpponentHandleGetRawMonData,
    LinkOpponentHandleSetMonData,
    LinkOpponentHandleSetRawMonData,
    LinkOpponentHandleLoadMonSprite,
    LinkOpponentHandleSwitchInAnim,
    LinkOpponentHandleReturnMonToBall,
    LinkOpponentHandleTrainerThrow,
    LinkOpponentHandleTrainerSlide,
    LinkOpponentHandleTrainerSlideBack,
    LinkOpponentHandleFaintAnimation,
    LinkOpponentHandlePaletteFade,
    LinkOpponentHandleSuccessBallThrowAnim,
    LinkOpponentHandleBallThrowAnim,
    LinkOpponentHandlePause,
    LinkOpponentHandleMoveAnimation,
    LinkOpponentHandlePrintString,
    LinkOpponentHandlePrintSelectionString,
    LinkOpponentHandleChooseAction,
    LinkOpponentHandleYesNoBox,
    LinkOpponentHandleChooseMove,
    LinkOpponentHandleChooseItem,
    LinkOpponentHandleChoosePokemon,
    LinkOpponentHandleCmd23,
    LinkOpponentHandleHealthBarUpdate,
    LinkOpponentHandleExpUpdate,
    LinkOpponentHandleStatusIconUpdate,
    LinkOpponentHandleStatusAnimation,
    LinkOpponentHandleStatusXor,
    LinkOpponentHandleDataTransfer,
    LinkOpponentHandleDMA3Transfer,
    LinkOpponentHandlePlayBGM,
    LinkOpponentHandleCmd32,
    LinkOpponentHandleTwoReturnValues,
    LinkOpponentHandleChosenMonReturnValue,
    LinkOpponentHandleOneReturnValue,
    LinkOpponentHandleOneReturnValue_Duplicate,
    LinkOpponentHandleClearUnkVar,
    LinkOpponentHandleSetUnkVar,
    LinkOpponentHandleClearUnkFlag,
    LinkOpponentHandleToggleUnkFlag,
    LinkOpponentHandleHitAnimation,
    LinkOpponentHandleCantSwitch,
    LinkOpponentHandlePlaySE,
    LinkOpponentHandlecmd44,
    LinkOpponentHandleFaintingCry,
    LinkOpponentHandleIntroSlide,
    LinkOpponentHandleIntroTrainerBallThrow,
    LinkOpponentHandleDrawPartyStatusSummary,
    LinkOpponentHandleHidePartyStatusSummary,
    LinkOpponentHandleEndBounceEffect,
    LinkOpponentHandleSpriteInvisibility,
    LinkOpponentHandleBattleAnimation,
    LinkOpponentHandleLinkStandbyMsg,
    LinkOpponentHandleResetActionMoveSelection,
    LinkOpponentHandlecmd55,
    LinkOpponentCmdEnd
};

// code

static void LinkOpponentDummy(void)
{
}

void SetControllerToLinkOpponent(void)
{
    gBattlerControllerFuncs[gActiveBattler] = LinkOpponentBufferRunCommand;
}

static void LinkOpponentBufferRunCommand(void)
{
    if (gBattleControllerExecFlags & gBitTable[gActiveBattler])
    {
        if (gBattleBufferA[gActiveBattler][0] <= 0x38)
            sLinkOpponentBufferCommands[gBattleBufferA[gActiveBattler][0]]();
        else
            LinkOpponentBufferExecCompleted();
    }
}

static void CompleteOnBattlerSpriteCallbackDummy(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        LinkOpponentBufferExecCompleted();
}

void sub_80375B4(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        sub_8031B74(gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineParam);
        gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum = gSprites[gBattlerSpriteIds[gActiveBattler]].data[5];
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        LinkOpponentBufferExecCompleted();
    }
}

static void Intro_DelayAndEnd(void)
{
    if ((--gBattleHealthBoxInfo[gActiveBattler].unk9) == 0xFF)
    {
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 0;
        LinkOpponentBufferExecCompleted();
    }
}

void sub_8037680(void)
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

    if (r6)
    {
        if (GetBattlerPosition(gActiveBattler) == 1)
        {
            if (!gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim || !gBattleHealthBoxInfo[gActiveBattler ^ 2].finishedShinyMonAnim)
                return;
            gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
            gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
            gBattleHealthBoxInfo[gActiveBattler ^ 2].triedShinyMonAnim = FALSE;
            gBattleHealthBoxInfo[gActiveBattler ^ 2].finishedShinyMonAnim = FALSE;
            FreeSpriteTilesByTag(0x27F9);
            FreeSpritePaletteByTag(0x27F9);
        }
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            if (GetBattlerPosition(gActiveBattler) == 1)
                m4aMPlayContinue(&gMPlayInfo_BGM);
        }
        else
        {
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 256);
        }
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 3;
        gBattlerControllerFuncs[gActiveBattler] = Intro_DelayAndEnd;
    }
}

void sub_8037840(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler, &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]]);
    if (!gBattleHealthBoxInfo[gActiveBattler ^ 2].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler ^ 2].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler ^ 2, &gEnemyParty[gBattlerPartyIndexes[gActiveBattler ^ 2]]);
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler ^ 2].ballAnimActive)
    {
        if ((gBattleTypeFlags & BATTLE_TYPE_MULTI) && GetBattlerPosition(gActiveBattler) == 3)
        {
            if (++gBattleHealthBoxInfo[gActiveBattler].unk9 == 1)
                return;
            gBattleHealthBoxInfo[gActiveBattler].unk9 = 0;
        }
        if (IsDoubleBattle() && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            DestroySprite(&gSprites[gBattleControllerData[gActiveBattler ^ 2]]);
            UpdateHealthboxAttribute(
              gHealthboxSpriteIds[gActiveBattler ^ 2],
              &gEnemyParty[gBattlerPartyIndexes[gActiveBattler ^ 2]],
              0);
            StartHealthboxSlideIn(gActiveBattler ^ 2);
            SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler ^ 2]);
            SetBattlerShadowSpriteCallback(
              gActiveBattler ^ 2,
              GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler ^ 2]], MON_DATA_SPECIES));
        }
        DestroySprite(&gSprites[gBattleControllerData[gActiveBattler]]);
        UpdateHealthboxAttribute(
          gHealthboxSpriteIds[gActiveBattler],
          &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]],
          0);
        StartHealthboxSlideIn(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
        SetBattlerShadowSpriteCallback(
          gActiveBattler,
          GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES));

        gBattleAnimationInfo->introAnimActive = 0;
        gBattlerControllerFuncs[gActiveBattler] = sub_8037680;
    }
}

void sub_8037A74(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].animEnded == TRUE
     && gSprites[gBattlerSpriteIds[gActiveBattler]].x2 == 0)
    {
        if (!gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        {
            TryShinyAnimation(gActiveBattler, &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]]);
            return;
        }
        if (gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim)
        {
            gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
            gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
            FreeSpriteTilesByTag(0x27F9);
            FreeSpritePaletteByTag(0x27F9);
            LinkOpponentBufferExecCompleted();
            return;
        }
    }
}

static void CompleteOnHealthbarDone(void)
{
    s16 hpValue = MoveBattleBar(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], 0, 0);

    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
    if (hpValue != -1)
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], hpValue, 0);
    else
        LinkOpponentBufferExecCompleted();
}

static void HideHealthboxAfterMonFaint(void)
{
    if (!gSprites[gBattlerSpriteIds[gActiveBattler]].inUse)
    {
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        LinkOpponentBufferExecCompleted();
    }
}

static void FreeMonSpriteAfterSwitchOutAnim(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        HideBattlerShadowSprite(gActiveBattler);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        LinkOpponentBufferExecCompleted();
    }
}

void sub_8037C2C(void)
{
    if (gWindowTemplate_Contest_MoveDescription.state == 0)
        LinkOpponentBufferExecCompleted();
}

static void DoHitAnimBlinkSpriteEffect(void)
{
    u8 spriteId = gBattlerSpriteIds[gActiveBattler];

    if (gSprites[spriteId].data[1] == 32)
    {
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].invisible = FALSE;
        gDoingBattleAnim = FALSE;
        LinkOpponentBufferExecCompleted();
    }
    else
    {
        if (((u16)gSprites[spriteId].data[1] % 4) == 0)
            gSprites[spriteId].invisible ^= 1;
        gSprites[spriteId].data[1]++;
    }
}

static void SwitchIn_ShowSubstitute(void)
{
    if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 6);
        gBattlerControllerFuncs[gActiveBattler] = sub_8037D2C;
    }
}

void sub_8037D2C(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        CreateTask(Task_PlayerController_RestoreBgmAfterCry, 10);
        LinkOpponentBufferExecCompleted();
    }
}

void sub_8037D64(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim)
    {
        gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
        FreeSpriteTilesByTag(0x27F9);
        FreeSpritePaletteByTag(0x27F9);
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], 0);
        UpdateHealthboxAttribute(
          gHealthboxSpriteIds[gActiveBattler],
          &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]],
          0);
        StartHealthboxSlideIn(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
        CopyBattleSpriteInvisibility(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = SwitchIn_ShowSubstitute;
    }
}

static void SwitchIn_TryShinyAnim(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler, &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]]);
    if (gSprites[gBattleControllerData[gActiveBattler]].callback == SpriteCallbackDummy
     && !gBattleHealthBoxInfo[gActiveBattler].ballAnimActive)
    {
        DestroySprite(&gSprites[gBattleControllerData[gActiveBattler]]);
        SetBattlerShadowSpriteCallback(gActiveBattler, GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES));
        gBattlerControllerFuncs[gActiveBattler] = sub_8037D64;
    }
}

void sub_8037EF0(void)
{
    if (gReceivedRemoteLinkPlayers == 0)
    {
        m4aSongNumStop(SE_LOW_HEALTH);
        gMain.inBattle = FALSE;
        gMain.callback1 = gPreBattleCallback1;
        SetMainCallback2(CB2_InitEndLinkBattle);
    }
}

void sub_8037F34(void)
{
    if (!gPaletteFade.active)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            SetCloseLinkCallback();
            gBattlerControllerFuncs[gActiveBattler] = sub_8037EF0;
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

static void CompleteOnFinishedStatusAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].statusAnimActive)
        LinkOpponentBufferExecCompleted();
}

static void CompleteOnFinishedBattleAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].animFromTableActive)
        LinkOpponentBufferExecCompleted();
}

static void LinkOpponentBufferExecCompleted(void)
{
    gBattlerControllerFuncs[gActiveBattler] = LinkOpponentBufferRunCommand;
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

static void LinkOpponentHandleGetMonData(void)
{
    u8 monData[0x100];
    u32 size = 0;
    u8 monToCheck;
    s32 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        size = CopyLinkOpponentMonData(gBattlerPartyIndexes[gActiveBattler], monData);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                size += CopyLinkOpponentMonData(i, monData + size);
            monToCheck >>= 1;
        }
    }
    BtlController_EmitDataTransfer(B_COMM_TO_ENGINE, size, monData);
    LinkOpponentBufferExecCompleted();
}

static u32 CopyLinkOpponentMonData(u8 monId, u8 *dst)
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
        battleMon.species = GetMonData(&gEnemyParty[monId], MON_DATA_SPECIES);
        battleMon.item = GetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM);
        for (size = 0; size < 4; size++)
        {
            battleMon.moves[size] = GetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + size);
            battleMon.pp[size] = GetMonData(&gEnemyParty[monId], MON_DATA_PP1 + size);
        }
        battleMon.ppBonuses = GetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES);
        battleMon.friendship = GetMonData(&gEnemyParty[monId], MON_DATA_FRIENDSHIP);
        battleMon.experience = GetMonData(&gEnemyParty[monId], MON_DATA_EXP);
        battleMon.hpIV = GetMonData(&gEnemyParty[monId], MON_DATA_HP_IV);
        battleMon.attackIV = GetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV);
        battleMon.defenseIV = GetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV);
        battleMon.speedIV = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV);
        battleMon.spAttackIV = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV);
        battleMon.spDefenseIV = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV);
        battleMon.personality = GetMonData(&gEnemyParty[monId], MON_DATA_PERSONALITY);
        battleMon.status1 = GetMonData(&gEnemyParty[monId], MON_DATA_STATUS);
        battleMon.level = GetMonData(&gEnemyParty[monId], MON_DATA_LEVEL);
        battleMon.hp = GetMonData(&gEnemyParty[monId], MON_DATA_HP);
        battleMon.maxHP = GetMonData(&gEnemyParty[monId], MON_DATA_MAX_HP);
        battleMon.attack = GetMonData(&gEnemyParty[monId], MON_DATA_ATK);
        battleMon.defense = GetMonData(&gEnemyParty[monId], MON_DATA_DEF);
        battleMon.speed = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED);
        battleMon.spAttack = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK);
        battleMon.spDefense = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF);
        battleMon.isEgg = GetMonData(&gEnemyParty[monId], MON_DATA_IS_EGG);
        battleMon.abilityNum = GetMonData(&gEnemyParty[monId], MON_DATA_ABILITY_NUM);
        battleMon.otId = GetMonData(&gEnemyParty[monId], MON_DATA_OT_ID);
        GetMonData(&gEnemyParty[monId], MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(battleMon.nickname, nickname);
        GetMonData(&gEnemyParty[monId], MON_DATA_OT_NAME, battleMon.otName);

        MEMCPY_ALT(&battleMon, dst, sizeof(battleMon), size, src);
        break;
    case 1:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPECIES);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 2:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 3:
        for (size = 0; size < 4; size++)
        {
            moveData.moves[size] = GetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + size);
            moveData.pp[size] = GetMonData(&gEnemyParty[monId], MON_DATA_PP1 + size);
        }
        moveData.ppBonuses = GetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES);
        MEMCPY_ALT(&moveData, dst, sizeof(moveData), size, src);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - 4);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 8:
        for (size = 0; size < 4; size++)
            dst[size] = GetMonData(&gEnemyParty[monId], MON_DATA_PP1 + size);
        dst[size] = GetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES);
        size++;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - 9);
        size = 1;
        break;
    case 17:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_OT_ID);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case 18:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_EXP);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case 19:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_HP_EV);
        size = 1;
        break;
    case 20:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_ATK_EV);
        size = 1;
        break;
    case 21:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_DEF_EV);
        size = 1;
        break;
    case 22:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED_EV);
        size = 1;
        break;
    case 23:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK_EV);
        size = 1;
        break;
    case 24:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_EV);
        size = 1;
        break;
    case 25:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_FRIENDSHIP);
        size = 1;
        break;
    case 26:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_POKERUS);
        size = 1;
        break;
    case 27:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_MET_LOCATION);
        size = 1;
        break;
    case 28:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_MET_LEVEL);
        size = 1;
        break;
    case 29:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_MET_GAME);
        size = 1;
        break;
    case 30:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_POKEBALL);
        size = 1;
        break;
    case 31:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_HP_IV);
        dst[1] = GetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV);
        dst[2] = GetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV);
        dst[3] = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV);
        dst[4] = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV);
        dst[5] = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV);
        size = 6;
        break;
    case 32:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_HP_IV);
        size = 1;
        break;
    case 33:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV);
        size = 1;
        break;
    case 34:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV);
        size = 1;
        break;
    case 35:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV);
        size = 1;
        break;
    case 36:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV);
        size = 1;
        break;
    case 37:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV);
        size = 1;
        break;
    case 38:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_PERSONALITY);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case 39:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_CHECKSUM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 40:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_STATUS);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case 41:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_LEVEL);
        size = 1;
        break;
    case 42:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 43:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_MAX_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 44:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_ATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 45:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_DEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 46:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 47:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 48:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case 49:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_COOL);
        size = 1;
        break;
    case 50:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY);
        size = 1;
        break;
    case 51:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_CUTE);
        size = 1;
        break;
    case 52:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SMART);
        size = 1;
        break;
    case 53:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_TOUGH);
        size = 1;
        break;
    case 54:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SHEEN);
        size = 1;
        break;
    case 55:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_COOL_RIBBON);
        size = 1;
        break;
    case 56:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY_RIBBON);
        size = 1;
        break;
    case 57:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_CUTE_RIBBON);
        size = 1;
        break;
    case 58:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SMART_RIBBON);
        size = 1;
        break;
    case 59:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_TOUGH_RIBBON);
        size = 1;
        break;
    }
    return size;
}

static void LinkOpponentHandleGetRawMonData(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleSetMonData(void)
{
    u8 i;
    u8 monToCheck;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        SetLinkOpponentMonData(gBattlerPartyIndexes[gActiveBattler]);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                SetLinkOpponentMonData(i);
            monToCheck >>= 1;
        }
    }
    LinkOpponentBufferExecCompleted();
}

static void SetLinkOpponentMonData(u8 monId)
{
    struct BattlePokemon *battlePokemon = (struct BattlePokemon *)&gBattleBufferA[gActiveBattler][3];
    struct MovePPInfo *moveData = (struct MovePPInfo *)&gBattleBufferA[gActiveBattler][3];
    s32 i;

    switch (gBattleBufferA[gActiveBattler][1])
    {
    case 0:
        {
            u8 iv;

            SetMonData(&gEnemyParty[monId], MON_DATA_SPECIES, &battlePokemon->species);
            SetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM, &battlePokemon->item);
            for (i = 0; i < 4; i++)
            {
                SetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + i, &battlePokemon->moves[i]);
                SetMonData(&gEnemyParty[monId], MON_DATA_PP1 + i, &battlePokemon->pp[i]);
            }
            SetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES, &battlePokemon->ppBonuses);
            SetMonData(&gEnemyParty[monId], MON_DATA_FRIENDSHIP, &battlePokemon->friendship);
            SetMonData(&gEnemyParty[monId], MON_DATA_EXP, &battlePokemon->experience);
            iv = battlePokemon->hpIV;
            SetMonData(&gEnemyParty[monId], MON_DATA_HP_IV, &iv);
            iv = battlePokemon->attackIV;
            SetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV, &iv);
            iv = battlePokemon->defenseIV;
            SetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV, &iv);
            iv = battlePokemon->speedIV;
            SetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV, &iv);
            iv = battlePokemon->spAttackIV;
            SetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV, &iv);
            iv = battlePokemon->spDefenseIV;
            SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV, &iv);
            SetMonData(&gEnemyParty[monId], MON_DATA_PERSONALITY, &battlePokemon->personality);
            SetMonData(&gEnemyParty[monId], MON_DATA_STATUS, &battlePokemon->status1);
            SetMonData(&gEnemyParty[monId], MON_DATA_LEVEL, &battlePokemon->level);
            SetMonData(&gEnemyParty[monId], MON_DATA_HP, &battlePokemon->hp);
            SetMonData(&gEnemyParty[monId], MON_DATA_MAX_HP, &battlePokemon->maxHP);
            SetMonData(&gEnemyParty[monId], MON_DATA_ATK, &battlePokemon->attack);
            SetMonData(&gEnemyParty[monId], MON_DATA_DEF, &battlePokemon->defense);
            SetMonData(&gEnemyParty[monId], MON_DATA_SPEED, &battlePokemon->speed);
            SetMonData(&gEnemyParty[monId], MON_DATA_SPATK, &battlePokemon->spAttack);
            SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF, &battlePokemon->spDefense);
        }
        break;
    case 1:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPECIES, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 2:
        SetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 3:
        for (i = 0; i < 4; i++)
        {
            SetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + i, &moveData->moves[i]);
            SetMonData(&gEnemyParty[monId], MON_DATA_PP1 + i, &moveData->pp[i]);
        }
        SetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES, &moveData->ppBonuses);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        SetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - 4, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 8:
        SetMonData(&gEnemyParty[monId], MON_DATA_PP1, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP2, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP3, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP4, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES, &gBattleBufferA[gActiveBattler][7]);
        break;
    case 9:
    case 10:
    case 11:
    case 12:
        SetMonData(&gEnemyParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - 9, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 17:
        SetMonData(&gEnemyParty[monId], MON_DATA_OT_ID, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 18:
        SetMonData(&gEnemyParty[monId], MON_DATA_EXP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 19:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 20:
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 21:
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 22:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 23:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 24:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 25:
        SetMonData(&gEnemyParty[monId], MON_DATA_FRIENDSHIP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 26:
        SetMonData(&gEnemyParty[monId], MON_DATA_POKERUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 27:
        SetMonData(&gEnemyParty[monId], MON_DATA_MET_LOCATION, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 28:
        SetMonData(&gEnemyParty[monId], MON_DATA_MET_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 29:
        SetMonData(&gEnemyParty[monId], MON_DATA_MET_GAME, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 30:
        SetMonData(&gEnemyParty[monId], MON_DATA_POKEBALL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 31:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][7]);
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][8]);
        break;
    case 32:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 33:
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 34:
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 35:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 36:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 37:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 38:
        SetMonData(&gEnemyParty[monId], MON_DATA_PERSONALITY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 39:
        SetMonData(&gEnemyParty[monId], MON_DATA_CHECKSUM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 40:
        SetMonData(&gEnemyParty[monId], MON_DATA_STATUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 41:
        SetMonData(&gEnemyParty[monId], MON_DATA_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 42:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 43:
        SetMonData(&gEnemyParty[monId], MON_DATA_MAX_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 44:
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 45:
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 46:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 47:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 48:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 49:
        SetMonData(&gEnemyParty[monId], MON_DATA_COOL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 50:
        SetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 51:
        SetMonData(&gEnemyParty[monId], MON_DATA_CUTE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 52:
        SetMonData(&gEnemyParty[monId], MON_DATA_SMART, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 53:
        SetMonData(&gEnemyParty[monId], MON_DATA_TOUGH, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 54:
        SetMonData(&gEnemyParty[monId], MON_DATA_SHEEN, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 55:
        SetMonData(&gEnemyParty[monId], MON_DATA_COOL_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 56:
        SetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 57:
        SetMonData(&gEnemyParty[monId], MON_DATA_CUTE_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 58:
        SetMonData(&gEnemyParty[monId], MON_DATA_SMART_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case 59:
        SetMonData(&gEnemyParty[monId], MON_DATA_TOUGH_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    }
}

static void LinkOpponentHandleSetRawMonData(void)
{
    u8 *dst;
    u8 i;

    MEMSET_ALT(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]] + gBattleBufferA[gActiveBattler][1],
        gBattleBufferA[gActiveBattler][3 + i], gBattleBufferA[gActiveBattler][2], i, dst);

    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleLoadMonSprite(void)
{
    u16 species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

    BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    SetMultiuseSpriteTemplateToPokemon(species, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gMultiuseSpriteTemplate,
      GetBattlerSpriteCoord(gActiveBattler, 2),
      GetBattlerSpriteDefault_Y(gActiveBattler),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], gBattleMonForms[gActiveBattler]);
    SetBattlerShadowSpriteCallback(gActiveBattler, GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES));
    gBattlerControllerFuncs[gActiveBattler] = sub_8037A74;
}

static void LinkOpponentHandleSwitchInAnim(void)
{
    gBattlerPartyIndexes[gActiveBattler] = gBattleBufferA[gActiveBattler][1];
    sub_8039430(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerControllerFuncs[gActiveBattler] = SwitchIn_TryShinyAnim;
}

void sub_8039430(u8 battler, u8 dontClearSubstituteBit)
{
    u16 species;

    ClearTemporarySpeciesSpriteData(battler, dontClearSubstituteBit);
    gBattlerPartyIndexes[battler] = gBattleBufferA[battler][1];
    species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
    gBattleControllerData[battler] = CreateInvisibleSpriteWithCallback(SpriteCB_WaitForBattlerBallReleaseAnim);
    BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[battler]], battler);
    SetMultiuseSpriteTemplateToPokemon(species, GetBattlerPosition(battler));
    gBattlerSpriteIds[battler] = CreateSprite(
      &gMultiuseSpriteTemplate,
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
    gSprites[gBattleControllerData[battler]].data[0] = DoPokeballSendOutAnimation(0, 0xFE);
}

static void LinkOpponentHandleReturnMonToBall(void)
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
        HideBattlerShadowSprite(gActiveBattler);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        LinkOpponentBufferExecCompleted();
    }
}

static void DoSwitchOutAnimation(void)
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
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 2);
            gBattlerControllerFuncs[gActiveBattler] = FreeMonSpriteAfterSwitchOutAnim;
        }
        break;
    }
}

void LinkOpponentHandleTrainerThrow(void)
{
    s16 xOffset;
    u32 gender;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (GetBattlerPosition(gActiveBattler) & 2)
            xOffset = -16;
        else
            xOffset = 16;
        gender = gLinkPlayers[GetBattlerMultiplayerId(gActiveBattler)].gender;
    }
    else
    {
        xOffset = 0;
        gender = gLinkPlayers[GetMultiplayerId() ^ 1].gender;
    }
    DecompressTrainerFrontPic(gender, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(gender, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gMultiuseSpriteTemplate,
      176 + xOffset, 40 + 4 * (8 - gTrainerFrontPicCoords[gender].size),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = IndexOfSpritePaletteTag(gTrainerFrontPicPaletteTable[gender].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[5] = gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum = GetSpriteTileStartByTag(gTrainerFrontPicTable[gender].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineParam = gender;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBattlerSpriteCallbackDummy;
}

static void LinkOpponentHandleTrainerSlide(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleTrainerSlideBack(void)
{
    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 35;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 280;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCallbackDummy);
    gBattlerControllerFuncs[gActiveBattler] = sub_80375B4;
}

static void LinkOpponentHandleFaintAnimation(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].animationState == 0)
    {
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 5);
        gBattleHealthBoxInfo[gActiveBattler].animationState++;
    }
    else if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
        PlaySE12WithPanning(SE_FAINT, 63);
        gSprites[gBattlerSpriteIds[gActiveBattler]].callback = sub_8010384;
        gBattlerControllerFuncs[gActiveBattler] = HideHealthboxAfterMonFaint;
    }
}

static void LinkOpponentHandlePaletteFade(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleSuccessBallThrowAnim(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleBallThrowAnim(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandlePause(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleMoveAnimation(void)
{
    if (!IsBattleSEPlaying(gActiveBattler))
    {
        u32 move = gBattleBufferA[gActiveBattler][1]
               | (gBattleBufferA[gActiveBattler][2] << 8);

        gAnimMoveTurn = gBattleBufferA[gActiveBattler][3];
        gAnimMovePower = gBattleBufferA[gActiveBattler][4]
                          | (gBattleBufferA[gActiveBattler][5] << 8);
        gAnimMoveDmg = gBattleBufferA[gActiveBattler][6]
                          | (gBattleBufferA[gActiveBattler][7] << 8)
                          | (gBattleBufferA[gActiveBattler][8] << 16)
                          | (gBattleBufferA[gActiveBattler][9] << 24);
        gAnimFriendship = gBattleBufferA[gActiveBattler][10];
        gWeatherMoveAnim = gBattleBufferA[gActiveBattler][12]
                          | (gBattleBufferA[gActiveBattler][13] << 8);
        gAnimDisableStructPtr = (struct DisableStruct *)&gBattleBufferA[gActiveBattler][16];
        gTransformedPersonalities[gActiveBattler] = gAnimDisableStructPtr->transformedMonPersonality;

        // Dead code. IsMoveWithoutAnimation always returns 0.
        if (IsMoveWithoutAnimation(move, gAnimMoveTurn) != 0)
        {
            LinkOpponentBufferExecCompleted();
        }
        else
        {
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            gBattlerControllerFuncs[gActiveBattler] = LinkOpponentDoMoveAnimation;
        }
    }
}

static void LinkOpponentDoMoveAnimation(void)
{
    u16 move = gBattleBufferA[gActiveBattler][1]
           | (gBattleBufferA[gActiveBattler][2] << 8);
    u8 multihit = gBattleBufferA[gActiveBattler][11];

    switch (gBattleHealthBoxInfo[gActiveBattler].animationState)
    {
    case 0:
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute && !gBattleSpriteInfo[gActiveBattler].flag_x8)
        {
            gBattleSpriteInfo[gActiveBattler].flag_x8 = 1;
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 5);
        }
        gBattleHealthBoxInfo[gActiveBattler].animationState = 1;
        break;
    case 1:
        if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        {
            SetBattlerSpriteAffineMode(ST_OAM_AFFINE_OFF);
            DoMoveAnim(move);
            gBattleHealthBoxInfo[gActiveBattler].animationState = 2;
        }
        break;
    case 2:
        gAnimScriptCallback();
        if (!gAnimScriptActive)
        {
            SetBattlerSpriteAffineMode(ST_OAM_AFFINE_NORMAL);
            if ((gBattleSpriteInfo[gActiveBattler].behindSubstitute) && multihit <= 1)
            {
                InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 6);
                gBattleSpriteInfo[gActiveBattler].flag_x8 = 0;
            }
            gBattleHealthBoxInfo[gActiveBattler].animationState = 3;
        }
        break;
    case 3:
        if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        {
            CopyAllBattleSpritesInvisibilities();
            TrySetBehindSubstituteSpriteBit(
              gActiveBattler,
              gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            LinkOpponentBufferExecCompleted();
        }
        break;
    }
}

static void LinkOpponentHandlePrintString(void)
{
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    BufferStringBattle(*(u16 *)&gBattleBufferA[gActiveBattler][2]);
    Contest_StartTextPrinter(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 144, 2, 15);
    gBattlerControllerFuncs[gActiveBattler] = sub_8037C2C;
}

static void LinkOpponentHandlePrintSelectionString(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleChooseAction(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleYesNoBox(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleChooseMove(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleChooseItem(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleChoosePokemon(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleCmd23(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleHealthBarUpdate(void)
{
    s16 hpVal;

    LoadBattleBarGfx(0);
    hpVal = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);
    if (hpVal != 0x7FFF)
    {
        u32 maxHP = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);
        u32 curHP = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, curHP, hpVal);
    }
    else
    {
        u32 maxHP = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, 0, hpVal);
    }
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnHealthbarDone;
}

static void LinkOpponentHandleExpUpdate(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleStatusIconUpdate(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == 0)
    {
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], 9);
        gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

static void LinkOpponentHandleStatusAnimation(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == 0)
    {
        InitAndLaunchChosenStatusAnimation(
          gBattleBufferA[gActiveBattler][1],
          gBattleBufferA[gActiveBattler][2]
          | (gBattleBufferA[gActiveBattler][3] << 8)
          | (gBattleBufferA[gActiveBattler][4] << 16)
          | (gBattleBufferA[gActiveBattler][5] << 24));
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

static void LinkOpponentHandleStatusXor(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleDataTransfer(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleDMA3Transfer(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandlePlayBGM(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleCmd32(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleTwoReturnValues(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleChosenMonReturnValue(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleOneReturnValue(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleOneReturnValue_Duplicate(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleClearUnkVar(void)
{
    gUnusedControllerStruct.unk = 0;
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleSetUnkVar(void)
{
    gUnusedControllerStruct.unk = gBattleBufferA[gActiveBattler][1];
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleClearUnkFlag(void)
{
    gUnusedControllerStruct.flag = 0;
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleToggleUnkFlag(void)
{
    gUnusedControllerStruct.flag ^= 1;
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleHitAnimation(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].invisible == TRUE)
    {
        LinkOpponentBufferExecCompleted();
    }
    else
    {
        gDoingBattleAnim = TRUE;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
        DoHitAnimHealthboxEffect(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = DoHitAnimBlinkSpriteEffect;
    }
}

static void LinkOpponentHandleCantSwitch(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandlePlaySE(void)
{
    s8 pan;

    if (GetBattlerSide(gActiveBattler) == 0)
        pan = -64;
    else
        pan = 63;
    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    LinkOpponentBufferExecCompleted();
}

void LinkOpponentHandlecmd44(void)
{
    PlayFanfare(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleFaintingCry(void)
{
    PlayCry_ByMode(
      GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES),
      25, 5);
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleIntroSlide(void)
{
    HandleIntroSlide(gBattleBufferA[gActiveBattler][1]);
    gIntroSlideFlags |= 1;
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleIntroTrainerBallThrow(void)
{
    u8 taskId;

    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 35;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 280;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], sub_803A3A8);
    taskId = CreateTask(Task_StartSendOutAnim, 5);
    gTasks[taskId].data[0] = gActiveBattler;
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    gBattleAnimationInfo->introAnimActive = 1;
    gBattlerControllerFuncs[gActiveBattler] = LinkOpponentDummy;
}

static void Task_StartSendOutAnim(u8 taskId)
{
    u8 savedActiveBank;

    savedActiveBank = gActiveBattler;
    gActiveBattler = gTasks[taskId].data[0];
    if (!IsDoubleBattle() || (gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_8039430(gActiveBattler, 0);
    }
    else
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_8039430(gActiveBattler, 0);
        gActiveBattler ^= 2;
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_8039430(gActiveBattler, 0);
        gActiveBattler ^= 2;
    }
    gBattlerControllerFuncs[gActiveBattler] = sub_8037840;
    gActiveBattler = savedActiveBank;
    DestroyTask(taskId);
}

void sub_803A3A8(struct Sprite *sprite)
{
    sub_8031B74(sprite->oam.affineParam);
    sprite->oam.tileNum = sprite->data[5];
    FreeSpriteOamMatrix(sprite);
    DestroySprite(sprite);
}

static void LinkOpponentHandleDrawPartyStatusSummary(void)
{
    if (gBattleBufferA[gActiveBattler][1] != 0 && GetBattlerSide(gActiveBattler) == 0)
    {
        LinkOpponentBufferExecCompleted();
        return;
    }

    gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown = 1;
    if (gBattleBufferA[gActiveBattler][2] != 0)
    {
        if (gBattleHealthBoxInfo[gActiveBattler].opponentDrawPartyStatusSummaryDelay < 2)
        {
            gBattleHealthBoxInfo[gActiveBattler].opponentDrawPartyStatusSummaryDelay++;
            return;
        }
        else
        {
            gBattleHealthBoxInfo[gActiveBattler].opponentDrawPartyStatusSummaryDelay = 0;
        }
    }
    gBattlerStatusSummaryTaskId[gActiveBattler] = CreatePartyStatusSummarySprites(
      gActiveBattler,
      (struct HpAndStatus *)&gBattleBufferA[gActiveBattler][4],
      gBattleBufferA[gActiveBattler][1],
      gBattleBufferA[gActiveBattler][2]);
    gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer = 0;
    if (gBattleBufferA[gActiveBattler][2] != 0)
        gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer = 0x5D;
    gBattlerControllerFuncs[gActiveBattler] = EndDrawPartyStatusSummary;
}

static void EndDrawPartyStatusSummary(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer++ >= 93)
    {
        gBattleHealthBoxInfo[gActiveBattler].partyStatusDelayTimer = 0;
        LinkOpponentBufferExecCompleted();
    }
}

static void LinkOpponentHandleHidePartyStatusSummary(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleEndBounceEffect(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleSpriteInvisibility(void)
{
    if (IsBattlerSpritePresent(gActiveBattler) != 0)
    {
        gSprites[gBattlerSpriteIds[gActiveBattler]].invisible = gBattleBufferA[gActiveBattler][1];
        CopyBattleSpriteInvisibility(gActiveBattler);
    }
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleBattleAnimation(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == 0)
    {
        u8 animationId = gBattleBufferA[gActiveBattler][1];
        u16 argument = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

        if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, animationId, argument) != 0)
            LinkOpponentBufferExecCompleted();
        else
            gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedBattleAnimation;
    }
}

static void LinkOpponentHandleLinkStandbyMsg(void)
{
    LinkOpponentBufferExecCompleted();
}

static void LinkOpponentHandleResetActionMoveSelection(void)
{
    LinkOpponentBufferExecCompleted();
}

void LinkOpponentHandlecmd55(void)
{
    if (gBattleBufferA[gActiveBattler][1] == 3)
        gBattleOutcome = gBattleBufferA[gActiveBattler][1];
    else
        gBattleOutcome = gBattleBufferA[gActiveBattler][1] ^ 3;
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    LinkOpponentBufferExecCompleted();
    gBattlerControllerFuncs[gActiveBattler] = sub_8037F34;
}

static void LinkOpponentCmdEnd(void)
{
}
