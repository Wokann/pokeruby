#include "global.h"
#include "battle.h"
#include "battle_ai_script_commands.h"
#include "battle_ai_switch_items.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "data.h"
#include "battle_anim_throw.h"
#include "battle_tower.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "palette.h"
#include "pokeball.h"
#include "pokemon.h"
#include "random.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/sound.h"
#include "constants/songs.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "util.h"
#include "ewram.h"
#include "constants/moves.h"

extern u8 gActiveBattler;
extern u8 gBattleBufferA[][0x200];
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerSpriteIds[];
extern u8 gBattleMonForms[];
extern struct SpriteTemplate gCreatingSpriteTemplate;
extern void (*gBattlerControllerFuncs[])(void);
extern u8 gBattleControllerData[];
extern u8 gHealthboxSpriteIds[];
extern u16 gBattleTypeFlags;
extern u16 gTrainerBattleOpponent;
extern s32 gAnimMoveDmg;
extern u16 gAnimMovePower;
extern u8 gAnimFriendship;
extern u16 gWeatherMoveAnim;
extern u32 gTransformedPersonalities[];
extern u8 gAnimMoveTurn;
extern u8 gAnimScriptActive;
extern void (*gAnimScriptCallback)(void);
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern u8 gDisplayedStringBattle[];
extern u8 gBattlerTarget;
extern u8 gAbsentBattlerFlags;
extern bool8 gDoingBattleAnim;
extern u16 gIntroSlideFlags;
extern u8 gBattlerStatusSummaryTaskId[];
extern MainCallback gPreBattleCallback1;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u32 gBattleControllerExecFlags;

extern u8 GetBattlerSpriteDefault_Y();
extern void BattleLoadOpponentMonSpriteGfx(struct Pokemon *, u8);
extern u8 GetBattlerPosition(u8);
extern void SetBattlerShadowSpriteCallback(u8, u16);
extern void SpriteCB_WaitForBattlerBallReleaseAnim(struct Sprite *);
extern void HideBattlerShadowSprite(u8 battler);
extern u8 GetBattleTowerTrainerFrontSpriteId(void);
extern void SpriteCB_TrainerSlideIn(struct Sprite *);
extern void StartAnimLinearTranslation(struct Sprite *);
extern void SetSpritePrimaryCoordsFromSecondaryCoords();
extern void StoreSpriteCallbackInData6();
extern void SpriteCB_FaintOpponentMon(struct Sprite *);
extern u8 IsMoveWithoutAnimation();
extern void SetBattlerSpriteAffineMode();
extern void CopyAllBattleSpritesInvisibilities(void);
extern void TrySetBehindSubstituteSpriteBit();
extern void BufferStringBattle();
extern void AI_TrySwitchOrUseItem(void);
extern u8 GetBattlerAtPosition(u8);
extern void DoHitAnimHealthboxEffect();
extern u8 GetBattlerSide(u8);
extern void HandleIntroSlide();
extern bool8 IsDoubleBattle(void);
extern u8 TryHandleLaunchBattleTableAnimation();

static void OpponentDummy(void);
static void OpponentBufferRunCommand(void);
static void CompleteOnBattlerSpriteCallbackDummy(void);
static void CompleteOnBankSpriteCallbackDummy2(void);
static void FreeTrainerSpriteAfterSlide(void);
static void Intro_DelayAndEnd(void);
static void Intro_WaitForShinyAnimAndHealthbox(void);
static void Intro_TryShinyAnimShowHealthbox(void);
static void TryShinyAnimAfterMonAnim(void);
static void CompleteOnHealthbarDone(void);
static void HideHealthboxAfterMonFaint(void);
static void FreeMonSpriteAfterSwitchOutAnim(void);
static void CompleteOnInactiveTextPrinter(void);
static void DoHitAnimBlinkSpriteEffect(void);
static void SwitchIn_ShowSubstitute(void);
static void SwitchIn_HandleSoundAndEnd(void);
static void SwitchIn_ShowHealthbox(void);
static void SwitchIn_TryShinyAnim(void);
static void CompleteOnFinishedStatusAnimation(void);
static void CompleteOnFinishedBattleAnimation(void);
static void OpponentBufferExecCompleted(void);
static u32 GetOpponentMonData(u8 monId, u8 *dst);
static void SetOpponentMonData(u8 monId);
static void StartSendOutAnim(u8 battler, bool8 dontClearSubstituteBit);
static void DoSwitchOutAnimation(void);
static void OpponentDoMoveAnimation(void);
static void SpriteCB_FreeOpponentSprite(struct Sprite *sprite);
static void Task_StartSendOutAnim(u8 taskId);
static void EndDrawPartyStatusSummary(void);

static void OpponentHandleGetMonData(void);
static void OpponentHandleGetRawMonData(void);
static void OpponentHandleSetMonData(void);
static void OpponentHandleSetRawMonData(void);
static void OpponentHandleLoadMonSprite(void);
static void OpponentHandleSwitchInAnim(void);
static void OpponentHandleReturnMonToBall(void);
static void OpponentHandleDrawTrainerPic(void);
static void OpponentHandleTrainerSlide(void);
static void OpponentHandleTrainerSlideBack(void);
static void OpponentHandleFaintAnimation(void);
static void OpponentHandlePaletteFade(void);
static void OpponentHandleSuccessBallThrowAnim(void);
static void OpponentHandleBallThrow(void);
static void OpponentHandlePause(void);
static void OpponentHandleMoveAnimation(void);
static void OpponentHandlePrintString(void);
static void OpponentHandlePrintSelectionString(void);
static void OpponentHandleChooseAction(void);
static void OpponentHandleYesNoBox(void);
static void OpponentHandleChooseMove(void);
static void OpponentHandleChooseItem(void);
static void OpponentHandleChoosePokemon(void);
static void OpponentHandleCmd23(void);
static void OpponentHandleHealthBarUpdate(void);
static void OpponentHandleExpUpdate(void);
static void OpponentHandleStatusIconUpdate(void);
static void OpponentHandleStatusAnimation(void);
static void OpponentHandleStatusXor(void);
static void OpponentHandleDataTransfer(void);
static void OpponentHandleDMA3Transfer(void);
static void OpponentHandlePlayBGM(void);
static void OpponentHandleCmd32(void);
static void OpponentHandleTwoReturnValues(void);
static void OpponentHandleChosenMonReturnValue(void);
static void OpponentHandleOneReturnValue(void);
static void OpponentHandleOneReturnValue_Duplicate(void);
static void OpponentHandleClearUnkVar(void);
static void OpponentHandleSetUnkVar(void);
static void OpponentHandleClearUnkFlag(void);
static void OpponentHandleToggleUnkFlag(void);
static void OpponentHandleHitAnimation(void);
static void OpponentHandleCantSwitch(void);
static void OpponentHandlePlaySE(void);
static void OpponentHandlePlayFanfareOrBGM(void);
static void OpponentHandleFaintingCry(void);
static void OpponentHandleIntroSlide(void);
static void OpponentHandleIntroTrainerBallThrow(void);
static void OpponentHandleDrawPartyStatusSummary(void);
static void OpponentHandleHidePartyStatusSummary(void);
static void OpponentHandleEndBounceEffect(void);
static void OpponentHandleSpriteInvisibility(void);
static void OpponentHandleBattleAnimation(void);
static void OpponentHandleLinkStandbyMsg(void);
static void OpponentHandleResetActionMoveSelection(void);
static void OpponentHandleEndLinkBattle(void);
static void OpponentCmdEnd(void);

// const data
typedef void (*BattleBufferCmd) (void);
static const BattleBufferCmd sOpponentBufferCommands[] =
{
    OpponentHandleGetMonData,
    OpponentHandleGetRawMonData,
    OpponentHandleSetMonData,
    OpponentHandleSetRawMonData,
    OpponentHandleLoadMonSprite,
    OpponentHandleSwitchInAnim,
    OpponentHandleReturnMonToBall,
    OpponentHandleDrawTrainerPic,
    OpponentHandleTrainerSlide,
    OpponentHandleTrainerSlideBack,
    OpponentHandleFaintAnimation,
    OpponentHandlePaletteFade,
    OpponentHandleSuccessBallThrowAnim,
    OpponentHandleBallThrow,
    OpponentHandlePause,
    OpponentHandleMoveAnimation,
    OpponentHandlePrintString,
    OpponentHandlePrintSelectionString,
    OpponentHandleChooseAction,
    OpponentHandleYesNoBox,
    OpponentHandleChooseMove,
    OpponentHandleChooseItem,
    OpponentHandleChoosePokemon,
    OpponentHandleCmd23,
    OpponentHandleHealthBarUpdate,
    OpponentHandleExpUpdate,
    OpponentHandleStatusIconUpdate,
    OpponentHandleStatusAnimation,
    OpponentHandleStatusXor,
    OpponentHandleDataTransfer,
    OpponentHandleDMA3Transfer,
    OpponentHandlePlayBGM,
    OpponentHandleCmd32,
    OpponentHandleTwoReturnValues,
    OpponentHandleChosenMonReturnValue,
    OpponentHandleOneReturnValue,
    OpponentHandleOneReturnValue_Duplicate,
    OpponentHandleClearUnkVar,
    OpponentHandleSetUnkVar,
    OpponentHandleClearUnkFlag,
    OpponentHandleToggleUnkFlag,
    OpponentHandleHitAnimation,
    OpponentHandleCantSwitch,
    OpponentHandlePlaySE,
    OpponentHandlePlayFanfareOrBGM,
    OpponentHandleFaintingCry,
    OpponentHandleIntroSlide,
    OpponentHandleIntroTrainerBallThrow,
    OpponentHandleDrawPartyStatusSummary,
    OpponentHandleHidePartyStatusSummary,
    OpponentHandleEndBounceEffect,
    OpponentHandleSpriteInvisibility,
    OpponentHandleBattleAnimation,
    OpponentHandleLinkStandbyMsg,
    OpponentHandleResetActionMoveSelection,
    OpponentHandleEndLinkBattle,
    OpponentCmdEnd,
};

static const u8 sUnused[] = {0xB0, 0xB0, 0xC8, 0x98, 0x28, 0x28, 0x28, 0x20};

// code

static void OpponentDummy(void)
{
}

void SetControllerToOpponent(void)
{
    gBattlerControllerFuncs[gActiveBattler] = OpponentBufferRunCommand;
}

static void OpponentBufferRunCommand(void)
{
    if (gBattleControllerExecFlags & gBitTable[gActiveBattler])
    {
        if (gBattleBufferA[gActiveBattler][0] <= CONTROLLER_TERMINATOR_NOP)
            sOpponentBufferCommands[gBattleBufferA[gActiveBattler][0]]();
        else
            OpponentBufferExecCompleted();
    }
}

static void CompleteOnBattlerSpriteCallbackDummy(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        OpponentBufferExecCompleted();
}

// Duplicate of CompleteOnBattlerSpriteCallbackDummy
static void CompleteOnBankSpriteCallbackDummy2(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        OpponentBufferExecCompleted();
}

static void FreeTrainerSpriteAfterSlide(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        FreeTrainerFrontPicPalette(gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineParam);
        gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum = gSprites[gBattlerSpriteIds[gActiveBattler]].data[5];
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        OpponentBufferExecCompleted();
    }
}

static void Intro_DelayAndEnd(void)
{
    if ((--gBattleHealthBoxInfo[gActiveBattler].introEndDelay) == 0xFF)
    {
        gBattleHealthBoxInfo[gActiveBattler].introEndDelay = 0;
        OpponentBufferExecCompleted();
    }
}

static void Intro_WaitForShinyAnimAndHealthbox(void)
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

    if (healthboxAnimDone
     && gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim
     && gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].finishedShinyMonAnim)
    {
        gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].finishedShinyMonAnim = FALSE;
        FreeSpriteTilesByTag(0x27F9);
        FreeSpritePaletteByTag(0x27F9);
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            m4aMPlayContinue(&gMPlayInfo_BGM);
        else
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 256);
        gBattleHealthBoxInfo[gActiveBattler].introEndDelay = 3;
        gBattlerControllerFuncs[gActiveBattler] = Intro_DelayAndEnd;
    }
}

static void Intro_TryShinyAnimShowHealthbox(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler, &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]]);
    if (!gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].ballAnimActive
     && !gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].triedShinyMonAnim)
        TryShinyAnimation(BATTLE_PARTNER(gActiveBattler), &gEnemyParty[gBattlerPartyIndexes[BATTLE_PARTNER(gActiveBattler)]]);
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive
     && !gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].ballAnimActive)
    {
        if (IsDoubleBattle() && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            DestroySprite(&gSprites[gBattleControllerData[BATTLE_PARTNER(gActiveBattler)]]);
            UpdateHealthboxAttribute(
              gHealthboxSpriteIds[BATTLE_PARTNER(gActiveBattler)],
              &gEnemyParty[gBattlerPartyIndexes[BATTLE_PARTNER(gActiveBattler)]],
              0);
            StartHealthboxSlideIn(BATTLE_PARTNER(gActiveBattler));
            SetHealthboxSpriteVisible(gHealthboxSpriteIds[BATTLE_PARTNER(gActiveBattler)]);
            SetBattlerShadowSpriteCallback(
              BATTLE_PARTNER(gActiveBattler),
              GetMonData(&gEnemyParty[gBattlerPartyIndexes[BATTLE_PARTNER(gActiveBattler)]], MON_DATA_SPECIES));
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

        gBattleAnimationInfo.introAnimActive = FALSE;
        gBattlerControllerFuncs[gActiveBattler] = Intro_WaitForShinyAnimAndHealthbox;
    }
}

static void TryShinyAnimAfterMonAnim(void)
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
            OpponentBufferExecCompleted();
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
        OpponentBufferExecCompleted();
}

static void HideHealthboxAfterMonFaint(void)
{
    if (!gSprites[gBattlerSpriteIds[gActiveBattler]].inUse)
    {
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        OpponentBufferExecCompleted();
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
        OpponentBufferExecCompleted();
    }
}

static void CompleteOnInactiveTextPrinter(void)
{
    if (gWindowTemplate_Contest_MoveDescription.state == 0)
        OpponentBufferExecCompleted();
}

static void DoHitAnimBlinkSpriteEffect(void)
{
    u8 spriteId = gBattlerSpriteIds[gActiveBattler];

    if (gSprites[spriteId].data[1] == 32)
    {
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].invisible = FALSE;
        gDoingBattleAnim = 0;
        OpponentBufferExecCompleted();
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
        gBattlerControllerFuncs[gActiveBattler] = SwitchIn_HandleSoundAndEnd;
    }
}

static void SwitchIn_HandleSoundAndEnd(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        CreateTask(Task_PlayerController_RestoreBgmAfterCry, 10);
        OpponentBufferExecCompleted();
    }
}

static void SwitchIn_ShowHealthbox(void)
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
        gBattlerControllerFuncs[gActiveBattler] = SwitchIn_ShowHealthbox;
    }
}

static void CompleteOnFinishedStatusAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].statusAnimActive)
        OpponentBufferExecCompleted();
}

static void CompleteOnFinishedBattleAnimation(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].animFromTableActive)
        OpponentBufferExecCompleted();
}

static void OpponentBufferExecCompleted(void)
{
    gBattlerControllerFuncs[gActiveBattler] = OpponentBufferRunCommand;
    gBattleControllerExecFlags &= ~gBitTable[gActiveBattler];
}

static void OpponentHandleGetMonData(void)
{
    u8 monData[256];
    int size = 0;
    s32 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        size = GetOpponentMonData(gBattlerPartyIndexes[gActiveBattler], monData);
    }
    else
    {
        u8 monToCheck = gBattleBufferA[gActiveBattler][2];

        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                size += GetOpponentMonData(i, monData + size);
            monToCheck >>= 1;
        }
    }
    BtlController_EmitDataTransfer(1, size, monData);
    OpponentBufferExecCompleted();
}

static u32 GetOpponentMonData(u8 monId, u8 *dst)
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

static void OpponentHandleGetRawMonData(void)
{
    struct BattlePokemon battleMon;
    u8 i;
    // TODO: Maybe fix this. Integrating this into MEMSET_ALT is too hard.
    u8 *src = (u8 *)&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]] + gBattleBufferA[gActiveBattler][1];
    u8 *dst;

    MEMSET_ALT(&battleMon + gBattleBufferA[gActiveBattler][1], src[i], gBattleBufferA[gActiveBattler][2], i, dst);
    BtlController_EmitDataTransfer(1, gBattleBufferA[gActiveBattler][2], dst);
    OpponentBufferExecCompleted();
}

static void OpponentHandleSetMonData(void)
{
    u8 i;
    u8 monToCheck;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        SetOpponentMonData(gBattlerPartyIndexes[gActiveBattler]);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                SetOpponentMonData(i);
            monToCheck >>= 1;
        }
    }
    OpponentBufferExecCompleted();
}

static void SetOpponentMonData(u8 monId)
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

static void OpponentHandleSetRawMonData(void)
{
    u8 *dst;
    u8 i;

    MEMSET_ALT(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]] + gBattleBufferA[gActiveBattler][1], gBattleBufferA[gActiveBattler][3 + i],
        gBattleBufferA[gActiveBattler][2], i, dst);
    OpponentBufferExecCompleted();
}

static void OpponentHandleLoadMonSprite(void)
{
    u16 species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

    BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    SetMultiuseSpriteTemplateToPokemon(species, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gCreatingSpriteTemplate,
      GetBattlerSpriteCoord(gActiveBattler, 2),
      GetBattlerSpriteDefault_Y(gActiveBattler),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = species;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], gBattleMonForms[gActiveBattler]);
    SetBattlerShadowSpriteCallback(gActiveBattler, GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES));
    gBattlerControllerFuncs[gActiveBattler] = TryShinyAnimAfterMonAnim;
}

static void OpponentHandleSwitchInAnim(void)
{
    gBattlerPartyIndexes[gActiveBattler] = gBattleBufferA[gActiveBattler][1];

    StartSendOutAnim(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerControllerFuncs[gActiveBattler] = SwitchIn_TryShinyAnim;
}

static void StartSendOutAnim(u8 battler, bool8 dontClearSubstituteBit)
{
    u16 species;

    ClearTemporarySpeciesSpriteData(battler, dontClearSubstituteBit);
    gBattlerPartyIndexes[battler] = gBattleBufferA[battler][1];
    species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
    gBattleControllerData[battler] = CreateInvisibleSpriteWithCallback(SpriteCB_WaitForBattlerBallReleaseAnim);
    BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[battler]], battler);
    SetMultiuseSpriteTemplateToPokemon(species, GetBattlerPosition(battler));
    gBattlerSpriteIds[battler] = CreateSprite(
      &gCreatingSpriteTemplate,
      GetBattlerSpriteCoord(battler, 2),
      GetBattlerSpriteDefault_Y(battler),
      GetBattlerSpriteSubpriority(battler));
    gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
    gSprites[gBattlerSpriteIds[battler]].data[2] = species;
    gSprites[gBattleControllerData[battler]].data[1] = gBattlerSpriteIds[battler];
    gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[battler]], gBattleMonForms[battler]);
    gSprites[gBattlerSpriteIds[battler]].invisible = TRUE;
    gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
    gSprites[gBattleControllerData[battler]].data[0] = DoPokeballSendOutAnimation(0, POKEBALL_OPPONENT_SENDOUT);
}

static void OpponentHandleReturnMonToBall(void)
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
        OpponentBufferExecCompleted();
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

static void OpponentHandleDrawTrainerPic(void)
{
    u32 trainerPicId;

#if DEBUG
    if (gDebugBattleFlags & 0x10)
    {
        trainerPicId = gSharedMem[BSTRUCT_OFF(debugTrainerPicIndex)];
    }
    else
#endif
    {
        if (gTrainerBattleOpponent == SECRET_BASE_OPPONENT)
            trainerPicId = GetSecretBaseTrainerPicIndex();
        else if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
            trainerPicId = GetBattleTowerTrainerFrontSpriteId();
        else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
            trainerPicId = GetEReaderTrainerPicIndex();
        else
            trainerPicId = gTrainers[gTrainerBattleOpponent].trainerPic;
    }

    DecompressTrainerFrontPic(trainerPicId, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(trainerPicId, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gCreatingSpriteTemplate,
      0xB0,
      40 + 4 * (8 - gTrainerFrontPicCoords[trainerPicId].size),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = IndexOfSpritePaletteTag(gTrainerFrontPicPaletteTable[trainerPicId].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[5] = gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum = GetSpriteTileStartByTag(gTrainerFrontPicTable[trainerPicId].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineParam = trainerPicId;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBattlerSpriteCallbackDummy;
}

static void OpponentHandleTrainerSlide(void)
{
    u32 trainerPicId;

    if (gTrainerBattleOpponent == SECRET_BASE_OPPONENT)
        trainerPicId = GetSecretBaseTrainerPicIndex();
    else if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
        trainerPicId = GetBattleTowerTrainerFrontSpriteId();
    else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
        trainerPicId = GetEReaderTrainerPicIndex();
    else
        trainerPicId = gTrainers[gTrainerBattleOpponent].trainerPic;

    DecompressTrainerFrontPic(trainerPicId, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(trainerPicId, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gCreatingSpriteTemplate,
      0xB0,
      40 + 4 * (8 - gTrainerFrontPicCoords[trainerPicId].size),
      0x1E);
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = 96;
    gSprites[gBattlerSpriteIds[gActiveBattler]].x += 32;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = -2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = IndexOfSpritePaletteTag(gTrainerFrontPicPaletteTable[trainerPicId].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[5] = gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum = GetSpriteTileStartByTag(gTrainerFrontPicTable[trainerPicId].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineParam = trainerPicId;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBankSpriteCallbackDummy2;
}

static void OpponentHandleTrainerSlideBack(void)
{
    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 35;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 280;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCallbackDummy);
    gBattlerControllerFuncs[gActiveBattler] = FreeTrainerSpriteAfterSlide;
}

static void OpponentHandleFaintAnimation(void)
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
        gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_FaintOpponentMon;
        gBattlerControllerFuncs[gActiveBattler] = HideHealthboxAfterMonFaint;
    }
}

static void OpponentHandlePaletteFade(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleSuccessBallThrowAnim(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleBallThrow(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandlePause(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleMoveAnimation(void)
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
            OpponentBufferExecCompleted();
        }
        else
        {
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            gBattlerControllerFuncs[gActiveBattler] = OpponentDoMoveAnimation;
        }
    }
}

static void OpponentDoMoveAnimation(void)
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
            OpponentBufferExecCompleted();
        }
        break;
    }
}

static void OpponentHandlePrintString(void)
{
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    BufferStringBattle(*(u16 *)&gBattleBufferA[gActiveBattler][2]);
    Contest_StartTextPrinter(&gWindowTemplate_Contest_MoveDescription, gDisplayedStringBattle, 144, 2, 15);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnInactiveTextPrinter;
}

static void OpponentHandlePrintSelectionString(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleChooseAction(void)
{
    AI_TrySwitchOrUseItem();
    OpponentBufferExecCompleted();
}

static void OpponentHandleYesNoBox(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleChooseMove(void)
{
    u8 chosenMoveId;
    u16 move;
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
#if DEBUG
    u16 battler;
    if (gDebugBattleFlags & 4)
    {
        // Move selection cycle
        move = MOVE_NONE;
        while (move == MOVE_NONE)
        {
            move = moveInfo->moves[gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) + ((gActiveBattler & BIT_FLANK) >> 1)]];
            chosenMoveId = gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) + ((gActiveBattler & BIT_FLANK) >> 1)];
            if (move != MOVE_NONE)
            {
                gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) +  ((gActiveBattler & BIT_FLANK) >> 1)]++;
            }
            else
            {
                gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) +  ((gActiveBattler & BIT_FLANK) >> 1)] = 0;
            }
            if (gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) +  ((gActiveBattler & BIT_FLANK) >> 1)] >= 4)
            {
                gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) +  ((gActiveBattler & BIT_FLANK) >> 1)] = 0;
            }
        }
        if (gBattleMoves[move].target & (MOVE_TARGET_USER_OR_SELECTED | MOVE_TARGET_USER))
        {
            battler = gActiveBattler;
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        {
            if (gDebugBattleFlags & 2)
            {
                if (gBattleMoves[move].target == MOVE_TARGET_SELECTED)
                {
                    battler = GetBattlerAtPosition(GetBattlerPosition(gActiveBattler) ^ BIT_FLANK);
                }
                else
                {
                    battler = GetBattlerAtPosition(0);
                }
            }
            else
            {
                battler = (Random() & BIT_FLANK);
            }
        }
        else
        {
            battler = GetBattlerAtPosition(0);
        }
        BtlController_EmitTwoReturnValues(1, B_ACTION_EXEC_SCRIPT, (battler << 8) | chosenMoveId);
        OpponentBufferExecCompleted();
    }
    else
#endif //DEBUG
    if (gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_SAFARI | BATTLE_TYPE_ROAMER))
    {
        BattleAI_SetupAIData();
        chosenMoveId = BattleAI_ChooseMoveOrAction();

        switch (chosenMoveId)
        {
        case 5: // AI_CHOICE_WATCH
            BtlController_EmitTwoReturnValues(1, B_ACTION_SAFARI_WATCH_CAREFULLY, 0);
            break;
        case 4: // AI_CHOICE_FLEE
            BtlController_EmitTwoReturnValues(1, B_ACTION_RUN, 0);
            break;
        default:
            if (gBattleMoves[moveInfo->moves[chosenMoveId]].target & (MOVE_TARGET_USER_OR_SELECTED | MOVE_TARGET_USER))
                gBattlerTarget = gActiveBattler;
            if (gBattleMoves[moveInfo->moves[chosenMoveId]].target & MOVE_TARGET_BOTH)
            {
                gBattlerTarget = GetBattlerAtPosition(0);
                if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
                    gBattlerTarget = GetBattlerAtPosition(2);
            }
            BtlController_EmitTwoReturnValues(1, B_ACTION_EXEC_SCRIPT, chosenMoveId | (gBattlerTarget << 8));
            break;
        }
        OpponentBufferExecCompleted();
    }
    else
    {
        while (1)
        {
            chosenMoveId = Random() & 3;
            move = moveInfo->moves[chosenMoveId];
            if (move != MOVE_NONE)
            {
                break;
            }
        }

        if (gBattleMoves[move].target & (MOVE_TARGET_USER_OR_SELECTED | MOVE_TARGET_USER))
            BtlController_EmitTwoReturnValues(1, B_ACTION_EXEC_SCRIPT, chosenMoveId | (gActiveBattler << 8));
        else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            BtlController_EmitTwoReturnValues(1, B_ACTION_EXEC_SCRIPT, chosenMoveId | (GetBattlerAtPosition(Random() & BIT_FLANK) << 8));
        else
            BtlController_EmitTwoReturnValues(1, B_ACTION_EXEC_SCRIPT, chosenMoveId | (GetBattlerAtPosition(0) << 8));
        OpponentBufferExecCompleted();
    }
}

static void OpponentHandleChooseItem(void)
{
    // The AI will only ever use an item whose ID fits in 8 bits.
    // If you want the AI to use an item with a higher ID, uncomment the code
    // in the line below.
    BtlController_EmitOneReturnValue(1, gSharedMem[BSTRUCT_OFF(AI_usedItemId) + (gActiveBattler / 2) * 2]/* | (gSharedMem[BSTRUCT_OFF(AI_usedItemId) + (gActiveBattler / 2) * 2 + 1] */);
    OpponentBufferExecCompleted();
}

static void OpponentHandleChoosePokemon(void)
{
    s32 chosenMonId;

    if (gSharedMem[BSTRUCT_OFF(AI_monToSwitchIntoId) + (GetBattlerPosition(gActiveBattler) / 2)] == PARTY_SIZE)
    {
        u8 battler1;
        u8 battler2;

        chosenMonId = GetMostSuitableMonToSwitchInto();
        if (chosenMonId == 6)
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            {
                battler2 = GetBattlerAtPosition(1);
                battler1 = battler2;
            }
            else
            {
                battler1 = GetBattlerAtPosition(1);
                battler2 = GetBattlerAtPosition(3);
            }
            for (chosenMonId = 0; chosenMonId < 6; chosenMonId++)
            {
                if (GetMonData(&gEnemyParty[chosenMonId], MON_DATA_HP) != 0
                 && chosenMonId != gBattlerPartyIndexes[battler1]
                 && chosenMonId != gBattlerPartyIndexes[battler2])
                    break;
            }
        }
    }
    else
    {
        chosenMonId = gSharedMem[BSTRUCT_OFF(AI_monToSwitchIntoId) + (GetBattlerPosition(gActiveBattler) / 2)];
        gSharedMem[BSTRUCT_OFF(AI_monToSwitchIntoId) + (GetBattlerPosition(gActiveBattler) / 2)] = PARTY_SIZE;
    }
    gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + gActiveBattler] = chosenMonId;
    BtlController_EmitChosenMonReturnValue(1, chosenMonId, 0);
    OpponentBufferExecCompleted();
}

static void OpponentHandleCmd23(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleHealthBarUpdate(void)
{
    s16 hpVal;

    LoadBattleBarGfx(0);
    hpVal = (gBattleBufferA[gActiveBattler][3] << 8) | gBattleBufferA[gActiveBattler][2];
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

static void OpponentHandleExpUpdate(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleStatusIconUpdate(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == 0)
    {
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], 9);
        gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

static void OpponentHandleStatusAnimation(void)
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

static void OpponentHandleStatusXor(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleDataTransfer(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleDMA3Transfer(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandlePlayBGM(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleCmd32(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleTwoReturnValues(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleChosenMonReturnValue(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleOneReturnValue(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleOneReturnValue_Duplicate(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleClearUnkVar(void)
{
    gUnusedControllerStruct.unk = 0;
    OpponentBufferExecCompleted();
}

static void OpponentHandleSetUnkVar(void)
{
    gUnusedControllerStruct.unk = gBattleBufferA[gActiveBattler][1];
    OpponentBufferExecCompleted();
}

static void OpponentHandleClearUnkFlag(void)
{
    gUnusedControllerStruct.flag = 0;
    OpponentBufferExecCompleted();
}

static void OpponentHandleToggleUnkFlag(void)
{
    gUnusedControllerStruct.flag ^= 1;
    OpponentBufferExecCompleted();
}

static void OpponentHandleHitAnimation(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].invisible == TRUE)
    {
        OpponentBufferExecCompleted();
    }
    else
    {
        gDoingBattleAnim = TRUE;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
        DoHitAnimHealthboxEffect(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = DoHitAnimBlinkSpriteEffect;
    }
}

static void OpponentHandleCantSwitch(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandlePlaySE(void)
{
    s8 pan;

    if (GetBattlerSide(gActiveBattler) == 0)
        pan = -64;
    else
        pan = 63;
    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    OpponentBufferExecCompleted();
}

static void OpponentHandlePlayFanfareOrBGM(void)
{
    PlayFanfare(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    OpponentBufferExecCompleted();
}

static void OpponentHandleFaintingCry(void)
{
    PlayCry_ByMode(
      GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES),
      25, CRY_MODE_FAINT);
    OpponentBufferExecCompleted();
}

static void OpponentHandleIntroSlide(void)
{
    HandleIntroSlide(gBattleBufferA[gActiveBattler][1]);
    gIntroSlideFlags |= 1;
    OpponentBufferExecCompleted();
}

static void OpponentHandleIntroTrainerBallThrow(void)
{
    u8 taskId;

    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 35;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 280;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCB_FreeOpponentSprite);
    taskId = CreateTask(Task_StartSendOutAnim, 5);
    gTasks[taskId].data[0] = gActiveBattler;
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    gBattleAnimationInfo.introAnimActive = TRUE;
    gBattlerControllerFuncs[gActiveBattler] = OpponentDummy;
}

static void SpriteCB_FreeOpponentSprite(struct Sprite *sprite)
{
    FreeTrainerFrontPicPalette(sprite->oam.affineParam);
    sprite->oam.tileNum = sprite->data[5];
    FreeSpriteOamMatrix(sprite);
    DestroySprite(sprite);
}

static void Task_StartSendOutAnim(u8 taskId)
{
    u8 savedActiveBattler;

    savedActiveBattler = gActiveBattler;
    gActiveBattler = gTasks[taskId].data[0];
    if (!IsDoubleBattle() || (gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        StartSendOutAnim(gActiveBattler, FALSE);
    }
    else
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        StartSendOutAnim(gActiveBattler, FALSE);
        gActiveBattler ^= 2;
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        StartSendOutAnim(gActiveBattler, FALSE);
        gActiveBattler ^= 2;
    }
    gBattlerControllerFuncs[gActiveBattler] = Intro_TryShinyAnimShowHealthbox;
    gActiveBattler = savedActiveBattler;
    DestroyTask(taskId);
}

static void OpponentHandleDrawPartyStatusSummary(void)
{
    if (gBattleBufferA[gActiveBattler][1] != 0 && GetBattlerSide(gActiveBattler) == 0)
    {
        OpponentBufferExecCompleted();
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
        OpponentBufferExecCompleted();
    }
}

static void OpponentHandleHidePartyStatusSummary(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    OpponentBufferExecCompleted();
}

static void OpponentHandleEndBounceEffect(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleSpriteInvisibility(void)
{
    if (IsBattlerSpritePresent(gActiveBattler) != 0)
    {
        gSprites[gBattlerSpriteIds[gActiveBattler]].invisible = gBattleBufferA[gActiveBattler][1];
        CopyBattleSpriteInvisibility(gActiveBattler);
    }
    OpponentBufferExecCompleted();
}

static void OpponentHandleBattleAnimation(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == 0)
    {
        u8 animationId = gBattleBufferA[gActiveBattler][1];
        u16 argument = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

        if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, animationId, argument) != 0)
            OpponentBufferExecCompleted();
        else
            gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedBattleAnimation;
    }
}

static void OpponentHandleLinkStandbyMsg(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleResetActionMoveSelection(void)
{
    OpponentBufferExecCompleted();
}

static void OpponentHandleEndLinkBattle(void)
{
    if ((gBattleTypeFlags & BATTLE_TYPE_LINK) && !(gBattleTypeFlags & BATTLE_TYPE_WILD))
    {
        gMain.inBattle = FALSE;
        gMain.callback1 = gPreBattleCallback1;
        SetMainCallback2(gMain.savedCallback);
    }
    OpponentBufferExecCompleted();
}

static void OpponentCmdEnd(void)
{
}
