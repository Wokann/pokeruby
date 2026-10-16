#ifndef GUARD_BATTLE_H
#define GUARD_BATTLE_H

#include "sprite.h"
#include "constants/battle.h"
#include "constants/battle_script_commands.h"
#include "battle_setup.h"
#include "battle_bg.h"
#include "main.h"

#define GET_BATTLER_POSITION(battler)     (gBattlerPositions[battler])
#define GET_BATTLER_SIDE(battler)         (GetBattlerPosition(battler) & BIT_SIDE)
#define GET_BATTLER_SIDE2(battler)        (GET_BATTLER_POSITION(battler) & BIT_SIDE)

// Battle Actions
// These determine what each battler will do in a turn
#define B_ACTION_USE_MOVE               0
#define B_ACTION_USE_ITEM               1
#define B_ACTION_SWITCH                 2
#define B_ACTION_RUN                    3
#define B_ACTION_SAFARI_WATCH_CAREFULLY 4
#define B_ACTION_SAFARI_BALL            5
#define B_ACTION_SAFARI_POKEBLOCK       6
#define B_ACTION_SAFARI_GO_NEAR         7
#define B_ACTION_SAFARI_RUN             8
// The exact purposes of these are unclear
#define B_ACTION_WALLY_THROW            9
#define B_ACTION_EXEC_SCRIPT            10 // when executing an action
#define B_ACTION_TRY_FINISH             11
#define B_ACTION_FINISHED               12 // when executing an action

#define B_ACTION_CANCEL_PARTNER         12 // when choosing an action
#define B_ACTION_NOTHING_FAINTED        13 // when choosing an action
#define B_ACTION_NONE                   0xFF

#define MAX_TRAINER_ITEMS 4
#define MAX_MON_MOVES 4

#define MOVE_TARGET_SELECTED            0x0
#define MOVE_TARGET_DEPENDS             0x1
#define MOVE_TARGET_USER_OR_SELECTED    0x2
#define MOVE_TARGET_RANDOM              0x4
#define MOVE_TARGET_BOTH                0x8
#define MOVE_TARGET_USER                0x10
#define MOVE_TARGET_FOES_AND_ALLY       0x20
#define MOVE_TARGET_OPPONENTS_FIELD     0x40

struct DisableStruct
{
    /*0x00*/ u32 transformedMonPersonality;
    /*0x04*/ u16 disabledMove;
    /*0x06*/ u16 encoredMove;
    /*0x08*/ u8 protectUses;
    /*0x09*/ u8 stockpileCounter;
    /*0x0A*/ u8 substituteHP;
    /*0x0B*/ u8 disableTimer : 4;
    /*0x0B*/ u8 disableTimerStartValue : 4;
    /*0x0C*/ u8 encoredMovePos;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 encoreTimer : 4;
    /*0x0E*/ u8 encoreTimerStartValue : 4;
    /*0x0F*/ u8 perishSongTimer : 4;
    /*0x0F*/ u8 perishSongTimerStartValue : 4;
    /*0x10*/ u8 furyCutterCounter;
    /*0x11*/ u8 rolloutTimer : 4;
    /*0x11*/ u8 rolloutTimerStartValue : 4;
    /*0x12*/ u8 chargeTimer : 4;
    /*0x12*/ u8 chargeTimerStartValue : 4;
    /*0x13*/ u8 tauntTimer:4;
    /*0x13*/ u8 tauntTimer2:4;
    /*0x14*/ u8 battlerPreventingEscape;
    /*0x15*/ u8 battlerWithSureHit;
    /*0x16*/ u8 isFirstTurn;
    /*0x17*/ u8 unk17;
    /*0x18*/ u8 truantCounter : 1;
    /*0x18*/ u8 unk18_a : 3;
    /*0x18*/ u8 mimickedMoves : 4;
    /*0x19*/ u8 rechargeTimer;
    /*0x1A*/ u8 unk1A[2];
};

struct ProtectStruct
{
    /*field0*/
    u32 protected:1;
    u32 endured:1;
    u32 noValidMoves:1;
    u32 helpingHand:1;
    u32 bounceMove:1;
    u32 stealMove:1;
    u32 flag0Unknown:1;
    u32 prlzImmobility:1;
    /*field1*/
    u32 confusionSelfDmg:1;
    u32 targetNotAffected:1;
    u32 chargingTurn:1;
    u32 fleeType:2; // For RunAway and Smoke Ball.
    u32 usedImprisonedMove:1;
    u32 loveImmobility:1;
    u32 usedDisabledMove:1;
    /*field2*/
    u32 usedTauntedMove:1;
    u32 flag2Unknown:1;
    u32 flinchImmobility:1;
    u32 notFirstStrike:1;
    u32 free : 4;
    /*field3*/
    u32 field3 : 8;
    s32 physicalDmg;
    s32 specialDmg;
    u8 physicalBattlerId;
    u8 specialBattlerId;
    u16 fieldE;
};

struct SpecialStatus
{
    u32 statLowered:1;
    u32 lightningRodRedirected:1;
    u32 restoredBattlerSprite: 1;
    u32 intimidatedMon:1;
    u32 traced:1;
    u32 ppNotAffectedByPressure:1;
    u32 faintedHasReplacement:1;
    u32 focusBanded:1;
    s32 shellBellDmg;
    s32 physicalDmg;
    s32 specialDmg;
    u8 physicalBattlerId;
    u8 specialBattlerId;
    u8 field12;
    u8 field13;
};

struct SideTimer
{
    u8 reflectTimer;        //0x0
    u8 lightscreenTimer;    //0x1
    u8 mistTimer;           //0x2
    u8 field3;              //0x3
    u16 field4;             //0x4
    u8 spikesAmount;        //0x6
    u8 safeguardTimer;      //0x7
    u8 followmeTimer;       //0x8
    u8 followmeTarget;      //0x9
    u8 fieldA;              //0xA
    u8 fieldB;              //0xB
};

struct WishFutureKnock
{
    u8 futureSightCounter[MAX_BATTLERS_COUNT];
    u8 futureSightAttacker[MAX_BATTLERS_COUNT];
    s32 futureSightDmg[MAX_BATTLERS_COUNT];
    u16 futureSightMove[MAX_BATTLERS_COUNT];
    u8 wishCounter[MAX_BATTLERS_COUNT];
    u8 wishMonId[MAX_BATTLERS_COUNT];
    u8 weatherDuration;
    u8 knockedOffMons[2];
};

struct AI_ThinkingStruct /* 0x2016800 */
{
    /*0x00*/ u8 aiState;
    /*0x01*/ u8 movesetIndex;
    /*0x02*/ u16 moveConsidered;
    /*0x04*/ s8 score[4]; // score?
    /*0x08*/ u32 funcResult;
    /*0x0C*/ u32 aiFlags;
    /*0x10*/ u8 aiAction;
    /*0x11*/ u8 aiLogicId;
    /*0x12*/ u8 filler12[6];
    /*0x18*/ u8 simulatedRNG[4];
};

struct BattleHistory
{
    /*0x00*/ u16 usedMoves[2][8]; // 0xFFFF means move not used (confuse self hit, etc)
    /*0x20*/ u8 abilities[MAX_BATTLERS_COUNT / 2];
    /*0x22*/ u8 itemEffects[MAX_BATTLERS_COUNT / 2];
    /*0x24*/ u16 trainerItems[MAX_BATTLERS_COUNT];
    /*0x2C*/ u8 itemsNo;
};

struct BattleScriptsStack
{
    const u8 *ptr[8];
    u8 size;
};

struct BattleCallbacksStack
{
    void* function[8];
    u8 size;
};

struct StatsArray
{
    u16 hp;
    u16 atk;
    u16 def;
    u16 spd;
    u16 spAtk;
    u16 spDef;
};

struct BattleResults
{
    u8 playerFaintCounter;    // 0x0
    u8 opponentFaintCounter;  // 0x1
    u8 totalMonSwitchCounter; // 0x2
    u8 numHealingItemsUsed;   // 0x3
    u8 numRevivesUsed;        // 0x4
    u8 playerMonWasDamaged:1; // 0x5
    u8 usedMasterBall:1;      // 0x5
    u16 playerMon1Species;         // 0x6
    u8 playerMon1Name[10];       // 0x8
    u8 unk12; // unused
    u8 battleTurnCounter;     // 0x13
    u8 playerMon2Name[10];       // 0x14
    u8 filler1E[2];
    u16 lastOpponentSpecies;  // 0x20
    u16 lastUsedMovePlayer;         // 0x22
    u16 lastUsedMoveOpponent;         // 0x24
    u16 playerMon2Species;      // 0x26
    u16 caughtMonSpecies;           // 0x28
    u8 caughtMonNick[10];        // 0x2A
    u8 filler34[2];
    u8 catchAttempts[11];
};

struct AI_Stack
{
    u8 *ptr[8];
    u8 size;
};

struct LinkBattlerHeader
{
    u8 versionSignatureLo;
    u8 versionSignatureHi;
    u8 vsScreenHealthFlagsLo;
    u8 vsScreenHealthFlagsHi;
    struct BattleEnigmaBerry battleEnigmaBerry;
};

union MultiBuffers
{
    struct LinkBattlerHeader linkBattlerHeader;
    struct MultiBattlePokemonTx multiBattleMons[3];
};

// Used internally
struct LinkBattleCommunicationHeader
{
    u8 state;
    u8 activeBattler;
    u8 battlerAttacker;
    u8 battlerTarget;
    u16 size;
    u8 absentBattlerFlags;
    u8 effectBattler;
    u8 data[0];
};

struct BattleStruct /* 0x2000000 */
{
    u8 filler00000[0x14000];
    /*0x14000*/ u8 linkSend[0x1000];
    /*0x15000*/ u8 linkRecv[0x1000];
    /*0x16000*/ u8 turnEffectsTracker;
    /*0x16001*/ u8 turnEffectsBattlerId;
    /*0x16002*/ u8 animTurn;
    /*0x16003*/ u8 battler;
    /*0x16004*/ u8 wrappedMove[MAX_BATTLERS_COUNT * 2];
    /*0x1600C*/ u8 moveendState;
    /*0x1600D*/ u8 unk1600D; // unused
    /*0x1600E*/ u8 turnCountersTracker;
    /*0x1600F*/ u8 getexpState;
    /*0x16010*/ u8 moveTarget[MAX_BATTLERS_COUNT];
    /*0x16014*/ u32 painSplitHp;
    /*0x16018*/ u8 expGetterMonId;
    /*0x16019*/ u8 unk16019; // unused
    /*0x1601A*/ u8 learnMoveState;
    /*0x1601B*/ u8 wildVictorySong;
    /*0x1601C*/ u8 dynamicMoveType;
    /*0x1601D*/ u8 focusPunchBattlerId;
    /*0x1601E*/ u8 statChanger;
    /*0x1601F*/ u8 dmgMultiplier;
    /*0x16020*/ u8 wrappedBy[MAX_BATTLERS_COUNT];
    /*0x16024*/ u16 assistPossibleMoves[PARTY_SIZE * MAX_MON_MOVES];
    /*0x16054*/ u8 battlerPreventingSwitchout;
    /*0x16055*/ u8 unk16055; // unused
    /*0x16056*/ u8 moneyMultiplier;
    /*0x16057*/ u8 savedTurnActionNumber;
    /*0x16058*/ u8 switchInAbilitiesCounter;
    /*0x16059*/ u8 faintedActionsState;
    /*0x1605A*/ u8 faintedActionsBattlerId;
    /*0x1605C*/ u16 expValue;
    /*0x1605E*/ u8 scriptPartyIdx;
    /*0x1605F*/ u8 sentInPokes;
    /*0x16060*/ bool8 selectionScriptFinished[MAX_BATTLERS_COUNT];
    /*0x16064*/ u8 battlerPartyIndexes[MAX_BATTLERS_COUNT];
    /*0x16068*/ u8 monToSwitchIntoId[MAX_BATTLERS_COUNT];
    /*0x1606C*/ u8 battlerPartyOrders[MAX_BATTLERS_COUNT][3];
    /*0x16078*/ u8 runTries;
    /*0x16079*/ u8 caughtMonNick[POKEMON_NAME_LENGTH + 1];
    /*0x16084*/ u8 battleStyle;
    /*0x16085*/ u8 unk16085;
    /*0x16086*/ u8 safariGoNearCounter;
    /*0x16087*/ u8 safariPkblThrowCounter;
    /*0x16088*/ u8 safariEscapeFactor;
    /*0x16089*/ u8 safariCatchFactor;
    /*0x1608A*/ u8 linkBattleVsSpriteId_V;
    /*0x1608B*/ u8 linkBattleVsSpriteId_S;
    /*0x1608C*/ u8 chosenMovePositions[MAX_BATTLERS_COUNT];
    /*0x16090*/ s32 bideDmg;
    /*0x16094*/ u8 stateIdAfterSelScript[4];
    /*0x16098*/ u8 unk16098[3]; // unused
    /*0x1609B*/ u8 formToChangeInto;
    /*0x1609C*/ u8 drawlvlupboxState;
    /*0x1609D*/ u8 prevSelectedPartySlot;
    /*0x1609E*/ u8 dbgAICycleMoveTracker[2]; // debug
    /*0x160A0*/ u8 stringMoveType;
    /*0x160A1*/ u8 animTargetsHit;
    /*0x160A2*/ u8 expGetterBattlerId;
    /*0x160A3*/ u8 unk160A3;
    /*0x160A4*/ u8 animArg1;
    /*0x160A5*/ u8 animArg2;
    /*0x160A6*/ u8 absentBattlerFlags;
    /*0x160A7*/ u8 pursuitDoublesAttacker;
    /*0x160A8*/ u8 wallyBattleState;
    /*0x160A9*/ u8 wallyMovesState;
    /*0x160AA*/ u8 wallyWaitFrames;
    /*0x160AB*/ u8 wallyMoveFrames;
    /*0x160AC*/ u16 lastTakenMove[2 * 2 * 2];
    /*0x160BC*/ u16 hpOnSwitchout[2];
    /*0x160C0*/ u8 abilityPreventingSwitchout;
    /*0x160C1*/ u8 hpScale;
    /*0x160C2*/ u16 savedBattleTypeFlags;
    /*0x160C4*/ MainCallback savedCallback;

    // Buffers used by the AI are indexed using (battler / 2)
    // i.e. the flank bit, because at no point in game is the
    // player's partner in a multi battle controlled by the AI.
    // This is changed in Emerald to support the Tabitha fight
    // in the Space Center.
    /*0x160C8*/ u8 AI_monToSwitchIntoId[2];
    /*0x160CA*/ u8 synchronizeMoveEffect;
    /*0x160CB*/ u8 multiplayerId;
    /*0x160CC*/ u16 usedHeldItems[MAX_BATTLERS_COUNT];
    // Space is reserved for two u16s, one for each opponent in
    // doubles. However, only the lower byte of each is ever used.
    /*0x160D4*/ u16 chosenItem[2];
    /*0x160D8*/ u8 AI_itemType[2];
    /*0x160DA*/ u8 AI_itemFlags[2];
    /*0x160DC*/ u8 statAnimPlayed;
    /*0x160DD*/ u8 intimidateBattler;
    /*0x160DE*/ u8 unk160DE;
    /*0x160DF*/ u8 unk160DF;
    /*0x160E0*/ u8 multihitString[6];
    /*0x160E6*/ bool8 overworldWeatherDone;
    /*0x160E7*/ u8 atkCancelerTracker;
    /*0x160E8*/ u16 choicedMove[MAX_BATTLERS_COUNT];
    /*0x160F0*/ u16 changedItems[MAX_BATTLERS_COUNT];
    /*0x160F8*/ u8 battlerWithAbility;
    /*0x160F9*/ u8 switchInItemsCounter;
    /*0x160FA*/ u8 levelUpHP;
    /*0x160FB*/ u8 unk160FB;
    /*0x160FC*/ u8 turnSideTracker;
    /*0x160FD*/ u8 unk160FD;
    /*0x160FE*/ u8 unk160FE;
    /*0x160FF*/ u8 unk160FF;
	/*0x16100*/ u16 lastTakenMoveFrom[2 * 4];
    /*0x16110*/ u8 wishPerishSongState;
    /*0x16111*/ u8 wishPerishSongBattlerId;
    /*0x16112*/ u8 multihitMoveEffect;
    /*0x16113*/ u8 givenExpMons;
};

struct ResourceFlags
{
    u32 flags[4];
};

struct BattleSpriteInfo
{
    u8 invisible:1;
    u8 lowHpSong:1;
    u8 behindSubstitute:1;
    u8 flag_x8:1;
    u8 hpNumbersNoBars:1;
    u16 transformSpecies;
};

struct BattleHealthboxInfo
{
 /*0x00*/ u8 partyStatusSummaryShown:1;
 /*0x00*/ u8 healthboxIsBouncing:1;
 /*0x00*/ u8 battlerIsBouncing:1;
 /*0x00*/ u8 ballAnimActive:1;
 /*0x00*/ u8 statusAnimActive:1;
 /*0x00*/ u8 animFromTableActive:1;
 /*0x00*/ u8 specialAnimActive:1;
 /*0x00*/ u8 triedShinyMonAnim:1;
 /*0x01*/ u8 finishedShinyMonAnim:1;
 /*0x01*/ u8 opponentDrawPartyStatusSummaryDelay:5;
 /*0x02*/ u8 healthboxBounceSpriteId;
 /*0x03*/ u8 battlerBounceSpriteId;
 /*0x04*/ u8 animationState;
 /*0x05*/ u8 partyStatusDelayTimer;
 /*0x06*/ u8 matrixNum;
 /*0x07*/ u8 shadowSpriteId;
 /*0x08*/ u8 soundTimer;
 /*0x09*/ u8 unk9;
 /*0x0A*/ u8 fillerA[2];
};

struct BattleAnimationInfo
{
    u16 animArg;
    u8 filler2[6];
    u8 ballThrowCaseId;
    u8 introAnimActive:1;
    u8 wildMonInvisible:1;
    u8 unkA;
    u16 ballSubpx;
};

struct UnusedControllerStruct
{
    u8 unk:7;
    u8 flag:1;
};

#define GET_MOVE_TYPE(move, typeArg)                        \
{                                                           \
    if (gBattleStruct->dynamicMoveType)                     \
        typeArg = gBattleStruct->dynamicMoveType & 0x3F;    \
    else                                                    \
        typeArg = gBattleMoves[move].type;                  \
}

#define IS_TYPE_PHYSICAL(moveType)(moveType < TYPE_MYSTERY)
#define IS_TYPE_SPECIAL(moveType)(moveType > TYPE_MYSTERY)

#define TARGET_TURN_DAMAGED (((gSpecialStatuses[gBattlerTarget].physicalDmg || gSpecialStatuses[gBattlerTarget].specialDmg)))

#define IS_BATTLER_OF_TYPE(battlerId, type)((gBattleMons[battlerId].type1 == type || gBattleMons[battlerId].type2 == type))
#define SET_BATTLER_TYPE(battlerId, type)   \
{                                           \
    gBattleMons[battlerId].type1 = type;    \
    gBattleMons[battlerId].type2 = type;    \
}

#define GET_STAT_BUFF_ID(n)((n & 0xF))              // first four bits 0x1, 0x2, 0x4, 0x8
#define GET_STAT_BUFF_VALUE2(n)((n & 0xF0))
#define GET_STAT_BUFF_VALUE(n)(((n >> 4) & 7))      // 0x10, 0x20, 0x40
#define STAT_BUFF_NEGATIVE 0x80                     // 0x80, the sign bit

#define SET_STAT_BUFF_VALUE(n)(((s8)(((s8)(n) << 4)) & 0xF0))

#define SET_STATCHANGER(statId, stage, goesDown)(gBattleScripting.statChanger = (statId) + (stage << 4) + (goesDown << 7))

extern u8 gBattleCommunication[];
extern struct BattleHistory unk_2016A00;
extern struct DisableStruct gDisableStructs[MAX_BATTLERS_COUNT];
extern struct BattleResults gBattleResults;
extern struct ProtectStruct gProtectStructs[MAX_BATTLERS_COUNT];
extern struct SpecialStatus gSpecialStatuses[MAX_BATTLERS_COUNT];
extern struct SideTimer gSideTimers[2];
extern struct WishFutureKnock gWishFutureKnock;
extern struct AI_ThinkingStruct gAIThinkingSpace;
extern struct UnusedControllerStruct gUnusedControllerStruct;

// used in many battle files, it seems as though Hisashi Sogabe wrote
// some sort of macro to replace the use of actually calling memset.
// Perhaps it was thought calling memset was much slower?

// The compiler wont allow us to locally declare ptr in this macro; some
// functions that invoke this macro will not match without this egregeous
// assumption about the variable names, so in order to avoid this assumption,
// we opt to pass the variables themselves, even though it is likely that
// Sogabe assumed the variables were named src and dest. Trust me: I tried to
// avoid assuming variable names, but the ROM just will not match without the
// assumptions. Therefore, these macros are bad practice, but I'm putting them
// here anyway.
#define MEMSET_ALT(data, c, size, var, dest)    \
{    \
    dest = (u8 *)data;    \
    for(var = 0; var < (u32)size; var++)    \
        dest[var] = c;    \
}    \

#define MEMCPY_ALT(data, dest, size, var, src)    \
{    \
    src = (u8 *)data;    \
    for(var = 0; var < (u32)size; var++)    \
        dest[var] = src[var];    \
}    \

typedef void (*BattleCmdFunc)(void);

extern u8 gBattleTextBuff1[];

extern u8 gBattleTextBuff1[];

extern u16 gBattle_BG0_X;
extern u16 gBattle_BG0_Y;
extern u16 gBattle_BG1_X;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG2_X;
extern u16 gBattle_BG2_Y;
extern u16 gBattle_BG3_X;
extern u16 gBattle_BG3_Y;
extern u16 gBattle_WIN0H;
extern u16 gBattle_WIN0V;
extern u16 gBattle_WIN1H;
extern u16 gBattle_WIN1V;

extern u8 gLastHitBy[MAX_BATTLERS_COUNT];

extern u8 gDisplayedStringBattle[];

extern u16 gBattleTypeFlags;
extern u8 gBattleOutcome;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT];
extern u8 gBattlerPositions[MAX_BATTLERS_COUNT];
extern u8 gBattlerSpriteIds[MAX_BATTLERS_COUNT];
extern u32 gTransformedPersonalities[MAX_BATTLERS_COUNT];
extern u8 gBattleMonForms[MAX_BATTLERS_COUNT];
extern u8 gActionSelectionCursor[MAX_BATTLERS_COUNT];
extern u8 gBattlerInMenuId;
extern u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
extern u8 gUnknown_02023A14_50;
extern u8 gChosenMovePos;
extern u16 gCalledMove;
extern s32 gBattleMoveDamage;
extern u16 gMoveToLearn;
extern s32 gBideDmg[MAX_BATTLERS_COUNT];
extern u8 gBideTarget[MAX_BATTLERS_COUNT];
extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u8 gBattlerFainted;
extern u8 gChosenActionByBattler[MAX_BATTLERS_COUNT];
extern u16 gChosenMoveByBattler[MAX_BATTLERS_COUNT];

// src/battle_2.o
void sub_800F104(void);
void sub_80101B8(void);
void c2_081284E0(void);
void sub_8011384(void);
void sub_8011800(void);
void sub_8011834(void);
void bc_801362C(void);
void sub_80119B4(void);
void BattleBeginFirstTurn(void);

// asm/battle_2.o
void sub_8012324(void);

void debug_sub_8010800(void);

// asm/battle_4.o
void AI_CalcDmg(u8, u8);
u8 TypeCalc(u16 move, u8 bank_atk, u8 bank_def);
u8 GetBattlerTurnOrderNum(u8 bank);

// asm/battle_5.o
void BattleControllerDummy(void);
void SetControllerToPlayer(void);
void nullsub_8(u8);
void CB2_SetUpReshowBattleScreenAfterMenu(void);
void CB2_SetUpReshowBattleScreenAfterMenu2(void);

int GameClear(void);

#endif // GUARD_BATTLE_H
