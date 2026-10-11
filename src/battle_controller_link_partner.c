#include "global.h"
#include "battle.h"
#include "battle_main.h"
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
#include "sound.h"
#include "constants/songs.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "util.h"
#include "ewram.h"

extern u16 gBattleTypeFlags;
extern u8 gDisplayedStringBattle[];
extern u8 gBattleBufferA[][0x200];
extern u8 gActiveBattler;
extern u32 gBattleControllerExecFlags;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerSpriteIds[];
extern u8 gBattleOutcome;
extern u16 gIntroSlideFlags;
extern u8 gBattlerStatusSummaryTaskId[];
extern u8 gDoingBattleAnim;
extern u32 gTransformedPersonalities[];
extern u8 gAnimFriendship;
extern u8 gAnimMoveTurn;
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern MainCallback gPreBattleCallback1;
extern void (*gBattlerControllerFuncs[])(void);
extern u8 gHealthboxSpriteIds[];
extern u8 gBattleControllerData[];
extern u8 gBattleMonForms[];
extern u8 gAnimScriptActive;
extern void (*gAnimScriptCallback)(void);

extern void Task_HidePartyStatusSummary(u8);
extern void BufferStringBattle();
extern void SetSpritePrimaryCoordsFromSecondaryCoords();
extern void StartAnimLinearTranslation(struct Sprite *);
extern void SpriteCB_FaintSlideAnim(struct Sprite *);
extern s32 GetBattlerMultiplayerId(u16);
extern u8 GetBattlerSpriteDefault_Y();

// this file's functions

static void LinkPartnerBufferRunCommand(void);
static void sub_811E0A0(void);
static void LinkPartnerBufferExecCompleted(void);
static u32 CopyLinkPartnerMonData(u8 monId, u8 *dst);
static void SetLinkPartnerMonData(u8);
static void sub_811F864(u8, u8);
static void DoSwitchOutAnimation(void);
static void LinkPartnerDoMoveAnimation(void);
static void Task_StartSendOutAnim(u8);
static void EndDrawPartyStatusSummary(void);

static void LinkPartnerHandleGetMonData(void);
static void LinkPartnerHandleGetRawMonData(void);
static void LinkPartnerHandleSetMonData(void);
static void LinkPartnerHandleSetRawMonData(void);
static void LinkPartnerHandleLoadMonSprite(void);
static void LinkPartnerHandleSwitchInAnim(void);
static void LinkPartnerHandleReturnMonToBall(void);
static void LinkPartnerHandleDrawTrainerPic(void);
static void LinkPartnerHandleTrainerSlide(void);
static void LinkPartnerHandleTrainerSlideBack(void);
static void LinkPartnerHandleFaintAnimation(void);
static void LinkPartnerHandlePaletteFade(void);
static void LinkPartnerHandleSuccessBallThrowAnim(void);
static void LinkPartnerHandleBallThrowAnim(void);
static void LinkPartnerHandlePause(void);
static void LinkPartnerHandleMoveAnimation(void);
static void LinkPartnerHandlePrintString(void);
static void LinkPartnerHandlePrintSelectionString(void);
static void LinkPartnerHandleChooseAction(void);
static void LinkPartnerHandleYesNoBox(void);
static void LinkPartnerHandleChooseMove(void);
static void LinkPartnerHandleChooseItem(void);
static void LinkPartnerHandleChoosePokemon(void);
static void LinkPartnerHandleCmd23(void);
static void LinkPartnerHandleHealthBarUpdate(void);
static void LinkPartnerHandleExpUpdate(void);
static void LinkPartnerHandleStatusIconUpdate(void);
static void LinkPartnerHandleStatusAnimation(void);
static void LinkPartnerHandleStatusXor(void);
static void LinkPartnerHandleDataTransfer(void);
static void LinkPartnerHandleDMA3Transfer(void);
static void LinkPartnerHandlePlayBGM(void);
static void LinkPartnerHandleCmd32(void);
static void LinkPartnerHandleTwoReturnValues(void);
static void LinkPartnerHandleChosenMonReturnValue(void);
static void LinkPartnerHandleOneReturnValue(void);
static void LinkPartnerHandleOneReturnValue_Duplicate(void);
static void LinkPartnerHandleClearUnkVar(void);
static void LinkPartnerHandleSetUnkVar(void);
static void LinkPartnerHandleClearUnkFlag(void);
static void LinkPartnerHandleToggleUnkFlag(void);
static void LinkPartnerHandleHitAnimation(void);
static void LinkPartnerHandleCantSwitch(void);
static void LinkPartnerHandlePlaySE(void);
static void LinkPartnerHandlecmd44(void);
static void LinkPartnerHandleFaintingCry(void);
static void LinkPartnerHandleIntroSlide(void);
static void LinkPartnerHandleIntroTrainerBallThrow(void);
static void LinkPartnerHandleDrawPartyStatusSummary(void);
static void LinkPartnerHandleHidePartyStatusSummary(void);
static void LinkPartnerHandleEndBounceEffect(void);
static void LinkPartnerHandleSpriteInvisibility(void);
static void LinkPartnerHandleBattleAnimation(void);
static void LinkPartnerHandleLinkStandbyMsg(void);
static void LinkPartnerHandleResetActionMoveSelection(void);
static void LinkPartnerHandleEndLinkBattle(void);
static void LinkPartnerCmdEnd(void);

// const data
typedef void (*BattleBufferCmd) (void);
static const BattleBufferCmd sLinkPartnerBufferCommands[] =
{
    LinkPartnerHandleGetMonData,
    LinkPartnerHandleGetRawMonData,
    LinkPartnerHandleSetMonData,
    LinkPartnerHandleSetRawMonData,
    LinkPartnerHandleLoadMonSprite,
    LinkPartnerHandleSwitchInAnim,
    LinkPartnerHandleReturnMonToBall,
    LinkPartnerHandleDrawTrainerPic,
    LinkPartnerHandleTrainerSlide,
    LinkPartnerHandleTrainerSlideBack,
    LinkPartnerHandleFaintAnimation,
    LinkPartnerHandlePaletteFade,
    LinkPartnerHandleSuccessBallThrowAnim,
    LinkPartnerHandleBallThrowAnim,
    LinkPartnerHandlePause,
    LinkPartnerHandleMoveAnimation,
    LinkPartnerHandlePrintString,
    LinkPartnerHandlePrintSelectionString,
    LinkPartnerHandleChooseAction,
    LinkPartnerHandleYesNoBox,
    LinkPartnerHandleChooseMove,
    LinkPartnerHandleChooseItem,
    LinkPartnerHandleChoosePokemon,
    LinkPartnerHandleCmd23,
    LinkPartnerHandleHealthBarUpdate,
    LinkPartnerHandleExpUpdate,
    LinkPartnerHandleStatusIconUpdate,
    LinkPartnerHandleStatusAnimation,
    LinkPartnerHandleStatusXor,
    LinkPartnerHandleDataTransfer,
    LinkPartnerHandleDMA3Transfer,
    LinkPartnerHandlePlayBGM,
    LinkPartnerHandleCmd32,
    LinkPartnerHandleTwoReturnValues,
    LinkPartnerHandleChosenMonReturnValue,
    LinkPartnerHandleOneReturnValue,
    LinkPartnerHandleOneReturnValue_Duplicate,
    LinkPartnerHandleClearUnkVar,
    LinkPartnerHandleSetUnkVar,
    LinkPartnerHandleClearUnkFlag,
    LinkPartnerHandleToggleUnkFlag,
    LinkPartnerHandleHitAnimation,
    LinkPartnerHandleCantSwitch,
    LinkPartnerHandlePlaySE,
    LinkPartnerHandlecmd44,
    LinkPartnerHandleFaintingCry,
    LinkPartnerHandleIntroSlide,
    LinkPartnerHandleIntroTrainerBallThrow,
    LinkPartnerHandleDrawPartyStatusSummary,
    LinkPartnerHandleHidePartyStatusSummary,
    LinkPartnerHandleEndBounceEffect,
    LinkPartnerHandleSpriteInvisibility,
    LinkPartnerHandleBattleAnimation,
    LinkPartnerHandleLinkStandbyMsg,
    LinkPartnerHandleResetActionMoveSelection,
    LinkPartnerHandleEndLinkBattle,
    LinkPartnerCmdEnd,
};
// code starts here

static void SpriteCB_Null2(void)
{
}

void SetControllerToLinkPartner(void)
{
    gBattlerControllerFuncs[gActiveBattler] = LinkPartnerBufferRunCommand;
}

static void LinkPartnerBufferRunCommand(void)
{
    if (gBattleControllerExecFlags & gBitTable[gActiveBattler])
    {
        if (gBattleBufferA[gActiveBattler][0] <= 0x38)
            sLinkPartnerBufferCommands[gBattleBufferA[gActiveBattler][0]]();
        else
            LinkPartnerBufferExecCompleted();
    }
}

static void CompleteOnBattlerSpriteCallbackDummy(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        LinkPartnerBufferExecCompleted();
}

static void FreeTrainerSpriteAfterSlide(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        BattleGfxSfxDummy3(0);
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        LinkPartnerBufferExecCompleted();
    }
}

static void Intro_DelayAndEnd(void)
{
    if ((--gBattleHealthBoxInfo[gActiveBattler].unk9) == 0xFF)
    {
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 0;
        LinkPartnerBufferExecCompleted();
    }
}

static void Intro_WaitForHealthbox(void)
{
    bool8 finished = FALSE;

    if (!IsDoubleBattle() || (IsDoubleBattle() && (gBattleTypeFlags & 0x40)))
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
            finished = TRUE;
    }
    else
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy
         && gSprites[gHealthboxSpriteIds[gActiveBattler ^ 2]].callback == SpriteCallbackDummy)
            finished = TRUE;
    }
    if (IsCryPlayingOrClearCrySongs())
        finished = FALSE;
    if (finished)
    {
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 3;
        gBattlerControllerFuncs[gActiveBattler] = Intro_DelayAndEnd;
    }
}

static void sub_811DCA0(void)
{
    u8 r2;

    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive)
    {
        // I couldn't get it to work as a bitfield here
        r2 = *((u8 *)&gBattleHealthBoxInfo[gActiveBattler ^ 2]) & 8;
        if (!r2 && (++gBattleHealthBoxInfo[gActiveBattler].unk9) != 1)
        {
            gBattleHealthBoxInfo[gActiveBattler].unk9 = r2;
            if (IsDoubleBattle() && !(gBattleTypeFlags & 0x40))
            {
                DestroySprite(&gSprites[gBattleControllerData[gActiveBattler ^ 2]]);
                UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler ^ 2], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler ^ 2]], 0);
                StartHealthboxSlideIn(gActiveBattler ^ 2);
                SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler ^ 2]);
            }
            DestroySprite(&gSprites[gBattleControllerData[gActiveBattler]]);
            UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 0);
            StartHealthboxSlideIn(gActiveBattler);
            SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
            gBattleHealthBoxInfo[4].unk9 = (s8)gBattleHealthBoxInfo[4].unk9 & ~1;
            gBattlerControllerFuncs[gActiveBattler] = Intro_WaitForHealthbox;
        }
    }
}

static void WaitForMonAnimAfterLoad(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].animEnded
     && gSprites[gBattlerSpriteIds[gActiveBattler]].x2 == 0)
        LinkPartnerBufferExecCompleted();
}

static void CompleteOnHealthbarDone(void)
{
    s16 hpValue;

    hpValue = MoveBattleBar(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], 0, 0);
    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
    if (hpValue != -1)
    {
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], hpValue, 0);
    }
    else
    {
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        LinkPartnerBufferExecCompleted();
    }
}

static void FreeMonSpriteAfterFaintAnim(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].y + gSprites[gBattlerSpriteIds[gActiveBattler]].y2 > 160)
    {
        BattleGfxSfxDummy2(GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES));
        FreeOamMatrix(gSprites[gBattlerSpriteIds[gActiveBattler]].oam.matrixNum);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        LinkPartnerBufferExecCompleted();
    }
}

static void FreeMonSpriteAfterSwitchOutAnim(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        LinkPartnerBufferExecCompleted();
    }
}

static void CompleteOnInactiveTextPrinter(void)
{
    if (gWindowTemplate_Contest_MoveDescription.state == 0)
        LinkPartnerBufferExecCompleted();
}

static void DoHitAnimBlinkSpriteEffect(void)
{
    u8 spriteId = gBattlerSpriteIds[gActiveBattler];
    if (gSprites[spriteId].data[1] == 32)
    {
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].invisible = FALSE;
        gDoingBattleAnim = FALSE;
        LinkPartnerBufferExecCompleted();
    }
    else
    {
        if ((gSprites[spriteId].data[1] % 4) == 0)
        {
            gSprites[spriteId].invisible ^= 1;
        }
        gSprites[spriteId].data[1]++;
    }
}

static void sub_811E034(void)
{
    if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 6);
        gBattlerControllerFuncs[gActiveBattler] = sub_811E0A0;
    }
}

static void sub_811E0A0(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        LinkPartnerBufferExecCompleted();
}

static void sub_811E0CC(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim)
    {
        gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
        FreeSpriteTilesByTag(0x27F9);
        FreeSpritePaletteByTag(0x27F9);
        CreateTask(Task_PlayerController_RestoreBgmAfterCry, 10);
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], 0);
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 0);
        StartHealthboxSlideIn(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
        CopyBattleSpriteInvisibility(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = sub_811E034;
    }
}

static void SwitchIn_TryShinyAnim(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler, &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]]);
    if (gSprites[gBattleControllerData[gActiveBattler]].callback == SpriteCallbackDummy
     && !gBattleHealthBoxInfo[gActiveBattler].ballAnimActive)
    {
        DestroySprite(&gSprites[gBattleControllerData[gActiveBattler]]);
        gBattlerControllerFuncs[gActiveBattler] = sub_811E0CC;
    }
}

void sub_811E258(void)
{
    if (gReceivedRemoteLinkPlayers == 0)
    {
        m4aSongNumStop(0x5A);
        gMain.inBattle = FALSE;
        gMain.callback1 = gPreBattleCallback1;
        SetMainCallback2(CB2_InitEndLinkBattle);
    }
}

void sub_811E29C(void)
{
    if (!gPaletteFade.active)
    {
        if (gBattleTypeFlags & 2)
        {
            SetCloseLinkCallback();
            gBattlerControllerFuncs[gActiveBattler] = sub_811E258;
        }
        else
        {
            m4aSongNumStop(0x5A);
            gMain.inBattle = FALSE;
            gMain.callback1 = gPreBattleCallback1;
            SetMainCallback2(gMain.savedCallback);
        }
    }
}

static void LinkPartnerBufferExecCompleted(void)
{
    u8 playerId;

    gBattlerControllerFuncs[gActiveBattler] = LinkPartnerBufferRunCommand;
    if (gBattleTypeFlags & 2)
    {
        playerId = GetMultiplayerId();
        PrepareBufferDataTransferLink(2, 4, &playerId);
        gBattleBufferA[gActiveBattler][0] = 0x38;
    }
    else
    {
        gBattleControllerExecFlags &= ~gBitTable[gActiveBattler];
    }
}

static void CompleteOnFinishedStatusAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].statusAnimActive)
        LinkPartnerBufferExecCompleted();
}

static void CompleteOnFinishedBattleAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].animFromTableActive)
        LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleGetMonData(void)
{
    u8 monData[256];
    int size = 0;
    s32 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        size = CopyLinkPartnerMonData(gBattlerPartyIndexes[gActiveBattler], monData);
    }
    else
    {
        u8 monToCheck = gBattleBufferA[gActiveBattler][2];

        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                size += CopyLinkPartnerMonData(i, monData + size);
            monToCheck >>= 1;
        }
    }
    BtlController_EmitDataTransfer(B_COMM_TO_ENGINE, size, monData);
    LinkPartnerBufferExecCompleted();
}

static u32 CopyLinkPartnerMonData(u8 monId, u8 *dst)
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

static void LinkPartnerHandleGetRawMonData(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleSetMonData(void)
{
    u8 i;
    u8 monToCheck;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        SetLinkPartnerMonData(gBattlerPartyIndexes[gActiveBattler]);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                SetLinkPartnerMonData(i);
            monToCheck >>= 1;
        }
    }
    LinkPartnerBufferExecCompleted();
}

static void SetLinkPartnerMonData(u8 monId)
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

static void LinkPartnerHandleSetRawMonData(void)
{
    u8 *dst;
    u8 i;

    MEMSET_ALT(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]] + gBattleBufferA[gActiveBattler][1],
        gBattleBufferA[gActiveBattler][3 + i], gBattleBufferA[gActiveBattler][2], i, dst);

    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleLoadMonSprite(void)
{
    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    SetMultiuseSpriteTemplateToPokemon(
      GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES),
      GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gMultiuseSpriteTemplate,
      GetBattlerSpriteCoord(gActiveBattler, 2),
      GetBattlerSpriteDefault_Y(gActiveBattler),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], gBattleMonForms[gActiveBattler]);
    gBattlerControllerFuncs[gActiveBattler] = WaitForMonAnimAfterLoad;
}

static void LinkPartnerHandleSwitchInAnim(void)
{
    ClearTemporarySpeciesSpriteData(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerPartyIndexes[gActiveBattler] = gBattleBufferA[gActiveBattler][1];
    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    sub_811F864(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerControllerFuncs[gActiveBattler] = SwitchIn_TryShinyAnim;
}

static void sub_811F864(u8 battler, u8 dontClearSubstituteBit)
{
    u16 species;

    ClearTemporarySpeciesSpriteData(battler, dontClearSubstituteBit);
    gBattlerPartyIndexes[battler] = gBattleBufferA[battler][1];
    species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
    gBattleControllerData[battler] = CreateInvisibleSpriteWithCallback(SpriteCB_WaitForBattlerBallReleaseAnim);
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
    gSprites[gBattleControllerData[battler]].data[0] = DoPokeballSendOutAnimation(0, 0xFF);
}

static void LinkPartnerHandleReturnMonToBall(void)
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
        LinkPartnerBufferExecCompleted();
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
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, 1);
            gBattlerControllerFuncs[gActiveBattler] = FreeMonSpriteAfterSwitchOutAnim;
        }
        break;
    }
}

static void LinkPartnerHandleDrawTrainerPic(void)
{
    s16 xOffset;
    u32 gender;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (GetBattlerPosition(gActiveBattler) & 2)
            xOffset = 16;
        else
            xOffset = -16;
        gender = gLinkPlayers[GetBattlerMultiplayerId(gActiveBattler)].gender;
    }
    else
    {
        xOffset = 0;
        gender = gLinkPlayers[GetMultiplayerId() ^ 1].gender;
    }
    DecompressTrainerBackPic(gender, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(gender, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gMultiuseSpriteTemplate,
      80 + xOffset, 80 + 4 * (8 - gTrainerBackPicCoords[gender].size),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = 240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = -2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBattlerSpriteCallbackDummy;
}

static void LinkPartnerHandleTrainerSlide(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleTrainerSlideBack(void)
{
    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 35;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = -40;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCallbackDummy);
    gBattlerControllerFuncs[gActiveBattler] = FreeTrainerSpriteAfterSlide;
}

static void LinkPartnerHandleFaintAnimation(void)
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
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        PlaySE12WithPanning(SE_FAINT, -64);
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 5;
        gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_FaintSlideAnim;
        gBattlerControllerFuncs[gActiveBattler] = FreeMonSpriteAfterFaintAnim;
    }
}

static void LinkPartnerHandlePaletteFade(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleSuccessBallThrowAnim(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleBallThrowAnim(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandlePause(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleMoveAnimation(void)
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

        if (IsMoveWithoutAnimation(move, gAnimMoveTurn) != 0)
            LinkPartnerBufferExecCompleted();
        else
        {
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            gBattlerControllerFuncs[gActiveBattler] = LinkPartnerDoMoveAnimation;
        }
    }
}

static void LinkPartnerDoMoveAnimation(void)
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
            LinkPartnerBufferExecCompleted();
        }
        break;
    }
}

static void LinkPartnerHandlePrintString(void)
{
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    BufferStringBattle(*(u16 *)&gBattleBufferA[gActiveBattler][2]);
    Contest_StartTextPrinter(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 144, 2, 15);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnInactiveTextPrinter;
}

static void LinkPartnerHandlePrintSelectionString(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleChooseAction(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleYesNoBox(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleChooseMove(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleChooseItem(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleChoosePokemon(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleCmd23(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleHealthBarUpdate(void)
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
    }
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnHealthbarDone;
}

static void LinkPartnerHandleExpUpdate(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleStatusIconUpdate(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == 0)
    {
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], 9);
        gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

static void LinkPartnerHandleStatusAnimation(void)
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

static void LinkPartnerHandleStatusXor(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleDataTransfer(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleDMA3Transfer(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandlePlayBGM(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleCmd32(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleTwoReturnValues(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleChosenMonReturnValue(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleOneReturnValue(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleOneReturnValue_Duplicate(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleClearUnkVar(void)
{
    gUnusedControllerStruct.unk = 0;
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleSetUnkVar(void)
{
    gUnusedControllerStruct.unk = gBattleBufferA[gActiveBattler][1];
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleClearUnkFlag(void)
{
    gUnusedControllerStruct.flag = 0;
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleToggleUnkFlag(void)
{
    gUnusedControllerStruct.flag ^= 1;
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleHitAnimation(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].invisible == TRUE)
    {
        LinkPartnerBufferExecCompleted();
    }
    else
    {
        gDoingBattleAnim = TRUE;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
        DoHitAnimHealthboxEffect(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = DoHitAnimBlinkSpriteEffect;
    }
}

static void LinkPartnerHandleCantSwitch(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandlePlaySE(void)
{
    s8 pan;

    if (GetBattlerSide(gActiveBattler) == 0)
        pan = -64;
    else
        pan = 63;
    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandlecmd44(void)
{
    PlayFanfare(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleFaintingCry(void)
{
    PlayCry_ByMode(
      GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES),
      -25, 5);
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleIntroSlide(void)
{
    HandleIntroSlide(gBattleBufferA[gActiveBattler][1]);
    gIntroSlideFlags |= 1;
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleIntroTrainerBallThrow(void)
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
    paletteNum = AllocSpritePalette(0xD6F9);
    LoadCompressedPalette(
      gTrainerBackPicPaletteTable[gLinkPlayers[GetBattlerMultiplayerId(gActiveBattler)].gender].data,
      0x100 + paletteNum * 16, 0x20);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = paletteNum;
    taskId = CreateTask(Task_StartSendOutAnim, 5);
    gTasks[taskId].data[0] = gActiveBattler;
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    gBattleAnimationInfo->introAnimActive = 1;
    gBattlerControllerFuncs[gActiveBattler] = SpriteCB_Null2;
}

static void Task_StartSendOutAnim(u8 taskId)
{
    u8 savedActiveBank;

    if (gTasks[taskId].data[1] < 24)
    {
        gTasks[taskId].data[1]++;
        return;
    }

    savedActiveBank = gActiveBattler;
    gActiveBattler = gTasks[taskId].data[0];
    if (!IsDoubleBattle() || (gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_811F864(gActiveBattler, 0);
    }
    else
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_811F864(gActiveBattler, 0);
        gActiveBattler ^= 2;
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        sub_811F864(gActiveBattler, 0);
        gActiveBattler ^= 2;
    }
    gBattlerControllerFuncs[gActiveBattler] = sub_811DCA0;
    gActiveBattler = savedActiveBank;
    DestroyTask(taskId);
}

static void LinkPartnerHandleDrawPartyStatusSummary(void)
{
    if (gBattleBufferA[gActiveBattler][1] != 0 && GetBattlerSide(gActiveBattler) == 0)
    {
        LinkPartnerBufferExecCompleted();
        return;
    }

    gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown = 1;
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
        LinkPartnerBufferExecCompleted();
    }
}

static void LinkPartnerHandleHidePartyStatusSummary(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleEndBounceEffect(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleSpriteInvisibility(void)
{
    if (IsBattlerSpritePresent(gActiveBattler) != 0)
    {
        gSprites[gBattlerSpriteIds[gActiveBattler]].invisible = gBattleBufferA[gActiveBattler][1];
        CopyBattleSpriteInvisibility(gActiveBattler);
    }
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleBattleAnimation(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == 0)
    {
        u8 animationId = gBattleBufferA[gActiveBattler][1];
        u16 argument = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

        if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, animationId, argument) != 0)
            LinkPartnerBufferExecCompleted();
        else
            gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedBattleAnimation;
    }
}

static void LinkPartnerHandleLinkStandbyMsg(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleResetActionMoveSelection(void)
{
    LinkPartnerBufferExecCompleted();
}

static void LinkPartnerHandleEndLinkBattle(void)
{
    gBattleOutcome = gBattleBufferA[gActiveBattler][1];
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    LinkPartnerBufferExecCompleted();
    gBattlerControllerFuncs[gActiveBattler] = sub_811E29C;
}

static void LinkPartnerCmdEnd(void)
{
}
