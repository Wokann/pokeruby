#include "global.h"
#include "constants/trainers.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_message.h"
#include "battle_gfx_sfx_util.h"
#include "battle_ai_script_commands.h"
#include "battle_ai_switch_items.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "data.h"
#include "battle_tower.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "palette.h"
#include "pokeball.h"
#include "pokemon.h"
#include "random.h"
#include "sound.h"
#include "constants/songs.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "util.h"
#include "ewram.h"
#include "constants/moves.h"
extern struct Window gWindowTemplate_Contest_MoveDescription;

extern void sub_8033018(void);
static void SwitchIn_TryShinyAnim(void);
static void FreeMonSpriteAfterSwitchOutAnim(void);
extern u8 get_trainer_class_pic_index(void);
static void CompleteOnBattlerSpriteCallbackDummy(void);
static void CompleteOnBankSpriteCallbackDummy2(void);
extern void sub_8032BBC(void);
static void HideHealthboxAfterMonFaint(void);
extern void sub_80331D0(void);
static void CompleteOnHealthbarDone(void);
static void CompleteOnFinishedStatusAnimation(void);
static void DoHitAnimBlinkSpriteEffect(void);
static void OpponentDummy(void);
extern void sub_8031B74();
extern void sub_8032E2C(void);
static void CompleteOnFinishedBattleAnimation(void);

// this file's functions

static void OpponentBufferExecCompleted(void);
static void OpponentBufferRunCommand(void);
static u32 GetOpponentMonData(u8 monId, u8 *dst);
static void SetOpponentMonData(u8 monId);
void sub_803495C(u8 battler, bool8 dontClearSubstituteBit);
static void DoSwitchOutAnimation(void);
static void OpponentDoMoveAnimation(void);
void sub_8035C10(struct Sprite *);
void sub_8035C44(u8);
static void EndDrawPartyStatusSummary(void);
void sub_80332D0(void);

static void OpponentHandleGetMonData(void);
static void OpponentHandleGetRawMonData(void);
static void OpponentHandleSetMonData(void);
static void OpponentHandleSetRawMonData(void);
static void OpponentHandleLoadMonSprite(void);
void OpponentHandleSendOutPoke(void);
static void OpponentHandleReturnMonToBall(void);
void OpponentHandleTrainerThrow(void);
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
void OpponentHandlecmd20(void);
static void OpponentHandleChooseItem(void);
void OpponentHandlecmd22(void);
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
void OpponentHandlecmd44(void);
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
static const BattleBufferCmd sOpponentBufferCommands[CONTROLLER_CMDS_COUNT] =
{
    [CONTROLLER_GETMONDATA]               = OpponentHandleGetMonData,
    [CONTROLLER_GETRAWMONDATA]            = OpponentHandleGetRawMonData,
    [CONTROLLER_SETMONDATA]               = OpponentHandleSetMonData,
    [CONTROLLER_SETRAWMONDATA]            = OpponentHandleSetRawMonData,
    [CONTROLLER_LOADMONSPRITE]            = OpponentHandleLoadMonSprite,
    [CONTROLLER_SWITCHINANIM]             = OpponentHandleSendOutPoke,
    [CONTROLLER_RETURNMONTOBALL]          = OpponentHandleReturnMonToBall,
    [CONTROLLER_DRAWTRAINERPIC]           = OpponentHandleTrainerThrow,
    [CONTROLLER_TRAINERSLIDE]             = OpponentHandleTrainerSlide,
    [CONTROLLER_TRAINERSLIDEBACK]         = OpponentHandleTrainerSlideBack,
    [CONTROLLER_FAINTANIMATION]           = OpponentHandleFaintAnimation,
    [CONTROLLER_PALETTEFADE]              = OpponentHandlePaletteFade,
    [CONTROLLER_SUCCESSBALLTHROWANIM]     = OpponentHandleSuccessBallThrowAnim,
    [CONTROLLER_BALLTHROWANIM]            = OpponentHandleBallThrow,
    [CONTROLLER_PAUSE]                    = OpponentHandlePause,
    [CONTROLLER_MOVEANIMATION]            = OpponentHandleMoveAnimation,
    [CONTROLLER_PRINTSTRING]              = OpponentHandlePrintString,
    [CONTROLLER_PRINTSTRINGPLAYERONLY]    = OpponentHandlePrintSelectionString,
    [CONTROLLER_CHOOSEACTION]             = OpponentHandleChooseAction,
    [CONTROLLER_YESNOBOX]                 = OpponentHandleYesNoBox,
    [CONTROLLER_CHOOSEMOVE]               = OpponentHandlecmd20,
    [CONTROLLER_OPENBAG]                  = OpponentHandleChooseItem,
    [CONTROLLER_CHOOSEPOKEMON]            = OpponentHandlecmd22,
    [CONTROLLER_23]                       = OpponentHandleCmd23,
    [CONTROLLER_HEALTHBARUPDATE]          = OpponentHandleHealthBarUpdate,
    [CONTROLLER_EXPUPDATE]                = OpponentHandleExpUpdate,
    [CONTROLLER_STATUSICONUPDATE]         = OpponentHandleStatusIconUpdate,
    [CONTROLLER_STATUSANIMATION]          = OpponentHandleStatusAnimation,
    [CONTROLLER_STATUSXOR]                = OpponentHandleStatusXor,
    [CONTROLLER_DATATRANSFER]             = OpponentHandleDataTransfer,
    [CONTROLLER_DMA3TRANSFER]             = OpponentHandleDMA3Transfer,
    [CONTROLLER_PLAYBGM]                  = OpponentHandlePlayBGM,
    [CONTROLLER_32]                       = OpponentHandleCmd32,
    [CONTROLLER_TWORETURNVALUES]          = OpponentHandleTwoReturnValues,
    [CONTROLLER_CHOSENMONRETURNVALUE]     = OpponentHandleChosenMonReturnValue,
    [CONTROLLER_ONERETURNVALUE]           = OpponentHandleOneReturnValue,
    [CONTROLLER_ONERETURNVALUE_DUPLICATE] = OpponentHandleOneReturnValue_Duplicate,
    [CONTROLLER_CLEARUNKVAR]              = OpponentHandleClearUnkVar,
    [CONTROLLER_SETUNKVAR]                = OpponentHandleSetUnkVar,
    [CONTROLLER_CLEARUNKFLAG]             = OpponentHandleClearUnkFlag,
    [CONTROLLER_TOGGLEUNKFLAG]            = OpponentHandleToggleUnkFlag,
    [CONTROLLER_HITANIMATION]             = OpponentHandleHitAnimation,
    [CONTROLLER_CANTSWITCH]               = OpponentHandleCantSwitch,
    [CONTROLLER_PLAYSE]                   = OpponentHandlePlaySE,
    [CONTROLLER_PLAYFANFAREORBGM]         = OpponentHandlecmd44,
    [CONTROLLER_FAINTINGCRY]              = OpponentHandleFaintingCry,
    [CONTROLLER_INTROSLIDE]               = OpponentHandleIntroSlide,
    [CONTROLLER_INTROTRAINERBALLTHROW]    = OpponentHandleIntroTrainerBallThrow,
    [CONTROLLER_DRAWPARTYSTATUSSUMMARY]   = OpponentHandleDrawPartyStatusSummary,
    [CONTROLLER_HIDEPARTYSTATUSSUMMARY]   = OpponentHandleHidePartyStatusSummary,
    [CONTROLLER_ENDBOUNCE]                = OpponentHandleEndBounceEffect,
    [CONTROLLER_SPRITEINVISIBILITY]       = OpponentHandleSpriteInvisibility,
    [CONTROLLER_BATTLEANIMATION]          = OpponentHandleBattleAnimation,
    [CONTROLLER_LINKSTANDBYMSG]           = OpponentHandleLinkStandbyMsg,
    [CONTROLLER_RESETACTIONMOVESELECTION] = OpponentHandleResetActionMoveSelection,
    [CONTROLLER_ENDLINKBATTLE]            = OpponentHandleEndLinkBattle,
    [CONTROLLER_TERMINATOR_NOP]           = OpponentCmdEnd,
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
        if (gBattleBufferA[gActiveBattler][0] < ARRAY_COUNT(sOpponentBufferCommands))
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

void sub_8032BBC(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        sub_8031B74(gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineParam);
        gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum = gSprites[gBattlerSpriteIds[gActiveBattler]].data[5];
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        OpponentBufferExecCompleted();
    }
}

static void Intro_DelayAndEnd(void)
{
    if ((--gBattleHealthBoxInfo[gActiveBattler].unk9) == 0xFF)
    {
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 0;
        OpponentBufferExecCompleted();
    }
}

void sub_8032C88(void)
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
         && gSprites[gHealthboxSpriteIds[BATTLE_PARTNER(gActiveBattler)]].callback == SpriteCallbackDummy)
            healthboxAnimDone = TRUE;
    }
    if (IsCryPlayingOrClearCrySongs())
        healthboxAnimDone = FALSE;

    if (healthboxAnimDone && gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim && gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].finishedShinyMonAnim)
    {
        gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].finishedShinyMonAnim = FALSE;
        FreeSpriteTilesByTag(ANIM_TAG_GOLD_STARS);
        FreeSpritePaletteByTag(ANIM_TAG_GOLD_STARS);
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            m4aMPlayContinue(&gMPlayInfo_BGM);
        else
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, 256);
        gBattleHealthBoxInfo[gActiveBattler].unk9 = 3;
        gBattlerControllerFuncs[gActiveBattler] = Intro_DelayAndEnd;
    }
}

void sub_8032E2C(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim)
        TryShinyAnimation(gActiveBattler, &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]]);
    if (!gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].ballAnimActive && !gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].triedShinyMonAnim)
        TryShinyAnimation(BATTLE_PARTNER(gActiveBattler), &gEnemyParty[gBattlerPartyIndexes[BATTLE_PARTNER(gActiveBattler)]]);
    if (!gBattleHealthBoxInfo[gActiveBattler].ballAnimActive && !gBattleHealthBoxInfo[BATTLE_PARTNER(gActiveBattler)].ballAnimActive)
    {
        if (IsDoubleBattle() && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            DestroySprite(&gSprites[gBattleControllerData[BATTLE_PARTNER(gActiveBattler)]]);
            UpdateHealthboxAttribute(
              gHealthboxSpriteIds[BATTLE_PARTNER(gActiveBattler)],
              &gEnemyParty[gBattlerPartyIndexes[BATTLE_PARTNER(gActiveBattler)]],
              HEALTHBOX_ALL);
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
          HEALTHBOX_ALL);
        StartHealthboxSlideIn(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
        SetBattlerShadowSpriteCallback(
          gActiveBattler,
          GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES));

        gBattleAnimationInfo->introAnimActive = FALSE;
        gBattlerControllerFuncs[gActiveBattler] = sub_8032C88;
    }
}

void sub_8033018(void)
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
            FreeSpriteTilesByTag(ANIM_TAG_GOLD_STARS);
            FreeSpritePaletteByTag(ANIM_TAG_GOLD_STARS);
            OpponentBufferExecCompleted();
            return;
        }
    }
}

static void CompleteOnHealthbarDone(void)
{
    s16 hpValue = MoveBattleBar(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], HEALTH_BAR, 0);

    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);
    if (hpValue != -1)
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], hpValue, HP_CURRENT);
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

void sub_80331D0(void)
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
        gDoingBattleAnim = FALSE;
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
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_MON_TO_SUBSTITUTE);
        gBattlerControllerFuncs[gActiveBattler] = sub_80332D0;
    }
}

void sub_80332D0(void)
{
    if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        CreateTask(Task_PlayerController_RestoreBgmAfterCry, 10);
        OpponentBufferExecCompleted();
    }
}

void sub_8033308(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim)
    {
        gBattleHealthBoxInfo[gActiveBattler].triedShinyMonAnim = FALSE;
        gBattleHealthBoxInfo[gActiveBattler].finishedShinyMonAnim = FALSE;
        FreeSpriteTilesByTag(ANIM_TAG_GOLD_STARS);
        FreeSpritePaletteByTag(ANIM_TAG_GOLD_STARS);
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], 0);
        UpdateHealthboxAttribute(
          gHealthboxSpriteIds[gActiveBattler],
          &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]],
          HEALTHBOX_ALL);
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
        gBattlerControllerFuncs[gActiveBattler] = sub_8033308;
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

        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (monToCheck & 1)
                size += GetOpponentMonData(i, monData + size);
            monToCheck >>= 1;
        }
    }
    BtlController_EmitDataTransfer(B_COMM_TO_ENGINE, size, monData);
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
    case REQUEST_ALL_BATTLE:
        battleMon.species = GetMonData(&gEnemyParty[monId], MON_DATA_SPECIES);
        battleMon.item = GetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM);
        for (size = 0; size < MAX_MON_MOVES; size++)
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
    case REQUEST_SPECIES_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPECIES);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_HELDITEM_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_MOVES_PP_BATTLE:
        for (size = 0; size < MAX_MON_MOVES; size++)
        {
            moveData.moves[size] = GetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + size);
            moveData.pp[size] = GetMonData(&gEnemyParty[monId], MON_DATA_PP1 + size);
        }
        moveData.ppBonuses = GetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES);
        MEMCPY_ALT(&moveData, dst, sizeof(moveData), size, src);
        break;
    case REQUEST_MOVE1_BATTLE:
    case REQUEST_MOVE2_BATTLE:
    case REQUEST_MOVE3_BATTLE:
    case REQUEST_MOVE4_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - REQUEST_MOVE1_BATTLE);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_PP_DATA_BATTLE:
        for (size = 0; size < MAX_MON_MOVES; size++)
            dst[size] = GetMonData(&gEnemyParty[monId], MON_DATA_PP1 + size);
        dst[size] = GetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES);
        size++;
        break;
    case REQUEST_PPMOVE1_BATTLE:
    case REQUEST_PPMOVE2_BATTLE:
    case REQUEST_PPMOVE3_BATTLE:
    case REQUEST_PPMOVE4_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - REQUEST_PPMOVE1_BATTLE);
        size = sizeof(*dst);
        break;
    case REQUEST_OTID_BATTLE:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_OT_ID);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case REQUEST_EXP_BATTLE:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_EXP);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case REQUEST_HP_EV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_HP_EV);
        size = sizeof(*dst);
        break;
    case REQUEST_ATK_EV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_ATK_EV);
        size = sizeof(*dst);
        break;
    case REQUEST_DEF_EV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_DEF_EV);
        size = sizeof(*dst);
        break;
    case REQUEST_SPEED_EV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED_EV);
        size = sizeof(*dst);
        break;
    case REQUEST_SPATK_EV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK_EV);
        size = sizeof(*dst);
        break;
    case REQUEST_SPDEF_EV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_EV);
        size = sizeof(*dst);
        break;
    case REQUEST_FRIENDSHIP_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_FRIENDSHIP);
        size = sizeof(*dst);
        break;
    case REQUEST_POKERUS_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_POKERUS);
        size = sizeof(*dst);
        break;
    case REQUEST_MET_LOCATION_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_MET_LOCATION);
        size = sizeof(*dst);
        break;
    case REQUEST_MET_LEVEL_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_MET_LEVEL);
        size = sizeof(*dst);
        break;
    case REQUEST_MET_GAME_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_MET_GAME);
        size = sizeof(*dst);
        break;
    case REQUEST_POKEBALL_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_POKEBALL);
        size = sizeof(*dst);
        break;
    case REQUEST_ALL_IVS_BATTLE:
        dst[STAT_HP] = GetMonData(&gEnemyParty[monId], MON_DATA_HP_IV);
        dst[STAT_ATK] = GetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV);
        dst[STAT_DEF] = GetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV);
        dst[STAT_SPEED] = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV);
        dst[STAT_SPATK] = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV);
        dst[STAT_SPDEF] = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV);
        size = NUM_STATS;
        break;
    case REQUEST_HP_IV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_HP_IV);
        size = sizeof(*dst);
        break;
    case REQUEST_ATK_IV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV);
        size = sizeof(*dst);
        break;
    case REQUEST_DEF_IV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV);
        size = sizeof(*dst);
        break;
    case REQUEST_SPEED_IV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV);
        size = sizeof(*dst);
        break;
    case REQUEST_SPATK_IV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV);
        size = sizeof(*dst);
        break;
    case REQUEST_SPDEF_IV_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV);
        size = sizeof(*dst);
        break;
    case REQUEST_PERSONALITY_BATTLE:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_PERSONALITY);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = sizeof(data32);
        break;
    case REQUEST_CHECKSUM_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_CHECKSUM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_STATUS_BATTLE:
        data32 = GetMonData(&gEnemyParty[monId], MON_DATA_STATUS);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = sizeof(data32);
        break;
    case REQUEST_LEVEL_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_LEVEL);
        size = sizeof(*dst);
        break;
    case REQUEST_HP_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_MAX_HP_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_MAX_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_ATK_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_ATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_DEF_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_DEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_SPEED_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPEED);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_SPATK_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_SPDEF_BATTLE:
        data16 = GetMonData(&gEnemyParty[monId], MON_DATA_SPDEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = sizeof(data16);
        break;
    case REQUEST_COOL_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_COOL);
        size = sizeof(*dst);
        break;
    case REQUEST_BEAUTY_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY);
        size = sizeof(*dst);
        break;
    case REQUEST_CUTE_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_CUTE);
        size = sizeof(*dst);
        break;
    case REQUEST_SMART_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SMART);
        size = sizeof(*dst);
        break;
    case REQUEST_TOUGH_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_TOUGH);
        size = sizeof(*dst);
        break;
    case REQUEST_SHEEN_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SHEEN);
        size = sizeof(*dst);
        break;
    case REQUEST_COOL_RIBBON_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_COOL_RIBBON);
        size = sizeof(*dst);
        break;
    case REQUEST_BEAUTY_RIBBON_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY_RIBBON);
        size = sizeof(*dst);
        break;
    case REQUEST_CUTE_RIBBON_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_CUTE_RIBBON);
        size = sizeof(*dst);
        break;
    case REQUEST_SMART_RIBBON_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_SMART_RIBBON);
        size = sizeof(*dst);
        break;
    case REQUEST_TOUGH_RIBBON_BATTLE:
        dst[0] = GetMonData(&gEnemyParty[monId], MON_DATA_TOUGH_RIBBON);
        size = sizeof(*dst);
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
    BtlController_EmitDataTransfer(B_COMM_TO_ENGINE, gBattleBufferA[gActiveBattler][2], dst);
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
        for (i = 0; i < PARTY_SIZE; i++)
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
    case REQUEST_ALL_BATTLE:
        {
            u8 iv;

            SetMonData(&gEnemyParty[monId], MON_DATA_SPECIES, &battlePokemon->species);
            SetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM, &battlePokemon->item);
            for (i = 0; i < MAX_MON_MOVES; i++)
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
    case REQUEST_SPECIES_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPECIES, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_HELDITEM_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_HELD_ITEM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MOVES_PP_BATTLE:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            SetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + i, &moveData->moves[i]);
            SetMonData(&gEnemyParty[monId], MON_DATA_PP1 + i, &moveData->pp[i]);
        }
        SetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES, &moveData->ppBonuses);
        break;
    case REQUEST_MOVE1_BATTLE:
    case REQUEST_MOVE2_BATTLE:
    case REQUEST_MOVE3_BATTLE:
    case REQUEST_MOVE4_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - REQUEST_MOVE1_BATTLE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_PP_DATA_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_PP1, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP2, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP3, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP4, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gEnemyParty[monId], MON_DATA_PP_BONUSES, &gBattleBufferA[gActiveBattler][7]);
        break;
    case REQUEST_PPMOVE1_BATTLE:
    case REQUEST_PPMOVE2_BATTLE:
    case REQUEST_PPMOVE3_BATTLE:
    case REQUEST_PPMOVE4_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - REQUEST_PPMOVE1_BATTLE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_OTID_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_OT_ID, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_EXP_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_EXP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_HP_EV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ATK_EV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_DEF_EV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPEED_EV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPATK_EV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPDEF_EV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_FRIENDSHIP_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_FRIENDSHIP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_POKERUS_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_POKERUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MET_LOCATION_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_MET_LOCATION, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MET_LEVEL_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_MET_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MET_GAME_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_MET_GAME, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_POKEBALL_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_POKEBALL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ALL_IVS_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][7]);
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][8]);
        break;
    case REQUEST_HP_IV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ATK_IV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_DEF_IV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPEED_IV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPATK_IV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPDEF_IV_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_PERSONALITY_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_PERSONALITY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_CHECKSUM_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_CHECKSUM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_STATUS_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_STATUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_LEVEL_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_HP_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MAX_HP_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_MAX_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ATK_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_ATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_DEF_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_DEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPEED_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPEED, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPATK_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPDEF_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SPDEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_COOL_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_COOL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_BEAUTY_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_CUTE_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_CUTE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SMART_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SMART, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_TOUGH_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_TOUGH, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SHEEN_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SHEEN, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_COOL_RIBBON_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_COOL_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_BEAUTY_RIBBON_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_BEAUTY_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_CUTE_RIBBON_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_CUTE_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SMART_RIBBON_BATTLE:
        SetMonData(&gEnemyParty[monId], MON_DATA_SMART_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_TOUGH_RIBBON_BATTLE:
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
      &gMultiuseSpriteTemplate,
      GetBattlerSpriteCoord(gActiveBattler, BATTLER_COORD_X_2),
      GetBattlerSpriteDefault_Y(gActiveBattler),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -DISPLAY_WIDTH;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = species;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], gBattleMonForms[gActiveBattler]);
    SetBattlerShadowSpriteCallback(gActiveBattler, GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES));
    gBattlerControllerFuncs[gActiveBattler] = sub_8033018;
}

void OpponentHandleSendOutPoke(void)
{
    gBattlerPartyIndexes[gActiveBattler] = gBattleBufferA[gActiveBattler][1];

    sub_803495C(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerControllerFuncs[gActiveBattler] = SwitchIn_TryShinyAnim;
}

void sub_803495C(u8 battler, bool8 dontClearSubstituteBit)
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
      GetBattlerSpriteCoord(battler, BATTLER_COORD_X_2),
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
    if (gBattleBufferA[gActiveBattler][1] == FALSE)
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
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SUBSTITUTE_TO_MON);
        gBattleHealthBoxInfo[gActiveBattler].animationState = 1;
        break;
    case 1:
        if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
        {
            gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SWITCH_OUT_OPPONENT_MON);
            gBattlerControllerFuncs[gActiveBattler] = FreeMonSpriteAfterSwitchOutAnim;
        }
        break;
    }
}

#define sSpeedX data[0]

void OpponentHandleTrainerThrow(void)
{
    u32 trainerPicId;

#if DEBUG
    if (gUnknown_02023A14_50 & 0x10)
    {
        trainerPicId = gSharedMem[BSTRUCT_OFF(unk160A3)];
    }
    else
#endif
    {
        if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
            trainerPicId = GetSecretBaseTrainerPicIndex();
        else if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
            trainerPicId = get_trainer_class_pic_index();
        else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
            trainerPicId = GetEreaderTrainerFrontSpriteId();
        else
            trainerPicId = gTrainers[gTrainerBattleOpponent_A].trainerPic;
    }

    DecompressTrainerFrontPic(trainerPicId, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(trainerPicId, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gMultiuseSpriteTemplate,
      0xB0,
      40 + 4 * (8 - gTrainerFrontPicCoords[trainerPicId].size),
      GetBattlerSpriteSubpriority(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = -DISPLAY_WIDTH;
    gSprites[gBattlerSpriteIds[gActiveBattler]].sSpeedX = 2;
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

    if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
        trainerPicId = GetSecretBaseTrainerPicIndex();
    else if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
        trainerPicId = get_trainer_class_pic_index();
    else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
        trainerPicId = GetEreaderTrainerFrontSpriteId();
    else
        trainerPicId = gTrainers[gTrainerBattleOpponent_A].trainerPic;

    DecompressTrainerFrontPic(trainerPicId, gActiveBattler);
    SetMultiuseSpriteTemplateToTrainerBack(trainerPicId, GetBattlerPosition(gActiveBattler));
    gBattlerSpriteIds[gActiveBattler] = CreateSprite(
      &gMultiuseSpriteTemplate,
      0xB0,
      40 + 4 * (8 - gTrainerFrontPicCoords[trainerPicId].size),
      0x1E);
    gSprites[gBattlerSpriteIds[gActiveBattler]].x2 = 96;
    gSprites[gBattlerSpriteIds[gActiveBattler]].x += 32;
    gSprites[gBattlerSpriteIds[gActiveBattler]].sSpeedX = -2;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = IndexOfSpritePaletteTag(gTrainerFrontPicPaletteTable[trainerPicId].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[5] = gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.tileNum = GetSpriteTileStartByTag(gTrainerFrontPicTable[trainerPicId].tag);
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineParam = trainerPicId;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = SpriteCB_TrainerSlideIn;
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBankSpriteCallbackDummy2;
}

#undef sSpeedX

static void OpponentHandleTrainerSlideBack(void)
{
    SetSpritePrimaryCoordsFromSecondaryCoords(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 35;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 280;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCallbackDummy);
    gBattlerControllerFuncs[gActiveBattler] = sub_8032BBC;
}

static void OpponentHandleFaintAnimation(void)
{
    if (gBattleHealthBoxInfo[gActiveBattler].animationState == 0)
    {
        if (gBattleSpriteInfo[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SUBSTITUTE_TO_MON);
        gBattleHealthBoxInfo[gActiveBattler].animationState++;
    }
    else if (!gBattleHealthBoxInfo[gActiveBattler].specialAnimActive)
    {
        gBattleHealthBoxInfo[gActiveBattler].animationState = 0;
        PlaySE12WithPanning(SE_FAINT, SOUND_PAN_TARGET);
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
        if (IsMoveWithoutAnimation(move, gAnimMoveTurn) != FALSE)
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
            gBattleSpriteInfo[gActiveBattler].flag_x8 = TRUE;
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SUBSTITUTE_TO_MON);
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
                InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_MON_TO_SUBSTITUTE);
                gBattleSpriteInfo[gActiveBattler].flag_x8 = FALSE;
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
    gBattlerControllerFuncs[gActiveBattler] = sub_80331D0;
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

void OpponentHandlecmd20(void)
{
    u8 chosenMoveId;
    u16 move;
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)&gBattleBufferA[gActiveBattler][4];
#if DEBUG
    u16 battler;
    if (gUnknown_02023A14_50 & 4)
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
            if (gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) +  ((gActiveBattler & BIT_FLANK) >> 1)] >= MAX_MON_MOVES)
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
            if (gUnknown_02023A14_50 & 2)
            {
                if (gBattleMoves[move].target == MOVE_TARGET_SELECTED)
                {
                    battler = GetBattlerAtPosition(GetBattlerPosition(gActiveBattler) ^ BIT_FLANK);
                }
                else
                {
                    battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
                }
            }
            else
            {
                battler = (Random() & BIT_FLANK);
            }
        }
        else
        {
            battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        }
        BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, (battler << 8) | chosenMoveId);
        OpponentBufferExecCompleted();
    }
    else
#endif //DEBUG
    if (gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_SAFARI | BATTLE_TYPE_ROAMER))
    {
        BattleAI_SetupAIData();
        chosenMoveId = BattleAI_GetAIActionToUse();

        switch (chosenMoveId)
        {
        case AI_CHOICE_WATCH:
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, B_ACTION_SAFARI_WATCH_CAREFULLY, 0);
            break;
        case AI_CHOICE_FLEE:
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, B_ACTION_RUN, 0);
            break;
        default:
            if (gBattleMoves[moveInfo->moves[chosenMoveId]].target & (MOVE_TARGET_USER_OR_SELECTED | MOVE_TARGET_USER))
                gBattlerTarget = gActiveBattler;
            if (gBattleMoves[moveInfo->moves[chosenMoveId]].target & MOVE_TARGET_BOTH)
            {
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
                if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
                    gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            }
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, chosenMoveId | (gBattlerTarget << 8));
            break;
        }
        OpponentBufferExecCompleted();
    }
    else
    {
        while (TRUE)
        {
            chosenMoveId = Random() & (MAX_MON_MOVES - 1);
            move = moveInfo->moves[chosenMoveId];
            if (move != MOVE_NONE)
            {
                break;
            }
        }

        if (gBattleMoves[move].target & (MOVE_TARGET_USER_OR_SELECTED | MOVE_TARGET_USER))
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, chosenMoveId | (gActiveBattler << 8));
        else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, chosenMoveId | (GetBattlerAtPosition(Random() & BIT_FLANK) << 8));
        else
            BtlController_EmitTwoReturnValues(B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, chosenMoveId | (GetBattlerAtPosition(B_POSITION_PLAYER_LEFT) << 8));
        OpponentBufferExecCompleted();
    }
}

static void OpponentHandleChooseItem(void)
{
    // The AI will only ever use an item whose ID fits in 8 bits.
    // If you want the AI to use an item with a higher ID, uncomment the code
    // in the line below.
    BtlController_EmitOneReturnValue(B_COMM_TO_ENGINE, gSharedMem[BSTRUCT_OFF(chosenItem) + (gActiveBattler / 2) * 2]/* | (gSharedMem[BSTRUCT_OFF(chosenItem) + (gActiveBattler / 2) * 2 + 1] */);
    OpponentBufferExecCompleted();
}

void OpponentHandlecmd22(void)
{
    s32 chosenMonId;

    if (gSharedMem[BSTRUCT_OFF(AI_monToSwitchIntoId) + (GetBattlerPosition(gActiveBattler) / 2)] == PARTY_SIZE)
    {
        u8 battler1;
        u8 battler2;

        chosenMonId = GetMostSuitableMonToSwitchInto();
        if (chosenMonId == PARTY_SIZE)
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            {
                battler2 = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
                battler1 = battler2;
            }
            else
            {
                battler1 = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
                battler2 = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            }
            for (chosenMonId = 0; chosenMonId < PARTY_SIZE; chosenMonId++)
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
    BtlController_EmitChosenMonReturnValue(B_COMM_TO_ENGINE, chosenMonId, NULL);
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
    if (hpVal != INSTANT_HP_BAR_DROP)
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
    if (IsBattleSEPlaying(gActiveBattler) == FALSE)
    {
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], HEALTHBOX_STATUS_ICON);
        gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = FALSE;
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

static void OpponentHandleStatusAnimation(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == FALSE)
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
    gUnusedControllerStruct.flag = FALSE;
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

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        pan = SOUND_PAN_ATTACKER;
    else
        pan = SOUND_PAN_TARGET;
    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    OpponentBufferExecCompleted();
}

void OpponentHandlecmd44(void)
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
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], sub_8035C10);
    taskId = CreateTask(sub_8035C44, 5);
    gTasks[taskId].data[0] = gActiveBattler;
    if (gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown)
        gTasks[gBattlerStatusSummaryTaskId[gActiveBattler]].func = Task_HidePartyStatusSummary;
    gBattleAnimationInfo->introAnimActive = TRUE;
    gBattlerControllerFuncs[gActiveBattler] = OpponentDummy;
}

void sub_8035C10(struct Sprite *sprite)
{
    sub_8031B74(sprite->oam.affineParam);
    sprite->oam.tileNum = sprite->data[5];
    FreeSpriteOamMatrix(sprite);
    DestroySprite(sprite);
}

void sub_8035C44(u8 taskId)
{
    u8 savedActiveBank;

    savedActiveBank = gActiveBattler;
    gActiveBattler = gTasks[taskId].data[0];
    if (!IsDoubleBattle() || (gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_803495C(gActiveBattler, FALSE);
    }
    else
    {
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_803495C(gActiveBattler, FALSE);
        gActiveBattler ^= BIT_FLANK;
        gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
        sub_803495C(gActiveBattler, FALSE);
        gActiveBattler ^= BIT_FLANK;
    }
    gBattlerControllerFuncs[gActiveBattler] = sub_8032E2C;
    gActiveBattler = savedActiveBank;
    DestroyTask(taskId);
}

static void OpponentHandleDrawPartyStatusSummary(void)
{
    if (gBattleBufferA[gActiveBattler][1] != 0 && GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
    {
        OpponentBufferExecCompleted();
        return;
    }

    gBattleHealthBoxInfo[gActiveBattler].partyStatusSummaryShown = TRUE;
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
    if (IsBattlerSpritePresent(gActiveBattler) != FALSE)
    {
        gSprites[gBattlerSpriteIds[gActiveBattler]].invisible = gBattleBufferA[gActiveBattler][1];
        CopyBattleSpriteInvisibility(gActiveBattler);
    }
    OpponentBufferExecCompleted();
}

static void OpponentHandleBattleAnimation(void)
{
    if (IsBattleSEPlaying(gActiveBattler) == FALSE)
    {
        u8 animationId = gBattleBufferA[gActiveBattler][1];
        u16 argument = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

        if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, animationId, argument) != FALSE)
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
    if ((gBattleTypeFlags & BATTLE_TYPE_LINK) && !(gBattleTypeFlags & BATTLE_TYPE_IS_MASTER))
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
