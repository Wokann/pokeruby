#ifndef GUARD_CONTEST_H
#define GUARD_CONTEST_H

#include "constants/contest.h"

enum
{
    CONTEST_WINNER_ARTIST,
    CONTEST_WINNER_NORMAL,
    CONTEST_WINNER_SUPER,
    CONTEST_WINNER_HYPER_1,
    CONTEST_WINNER_HYPER_2,
    CONTEST_WINNER_HYPER_3,
    CONTEST_WINNER_MASTER_1,
    CONTEST_WINNER_MASTER_2,
    CONTEST_WINNER_MASTER_3,
    NUM_CONTEST_HALL_WINNERS = CONTEST_WINNER_MASTER_3,
    MUSEUM_CONTEST_WINNERS_START = CONTEST_WINNER_MASTER_3,
    CONTEST_WINNER_MUSEUM_COOL,
    CONTEST_WINNER_MUSEUM_BEAUTY,
    CONTEST_WINNER_MUSEUM_CUTE,
    CONTEST_WINNER_MUSEUM_SMART,
    CONTEST_WINNER_MUSEUM_TOUGH,
};

enum
{
    CONTEST_STRING_MORE_CONSCIOUS,
    CONTEST_STRING_NO_APPEAL,
    CONTEST_STRING_SETTLE_DOWN,
    CONTEST_STRING_OBLIVIOUS_TO_OTHERS,
    CONTEST_STRING_LESS_AWARE,
    CONTEST_STRING_STOPPED_CARING,
    CONTEST_STRING_STARTLE_ATTEMPT,
    CONTEST_STRING_DAZZLE_ATTEMPT,
    CONTEST_STRING_JUDGE_LOOK_AWAY2,
    CONTEST_STRING_UNNERVE_ATTEMPT,
    CONTEST_STRING_NERVOUS,
    CONTEST_STRING_UNNERVE_WAITING,
    CONTEST_STRING_TAUNT_WELL,
    CONTEST_STRING_REGAINED_FORM,
    CONTEST_STRING_JAM_WELL,
    CONTEST_STRING_HUSTLE_STANDOUT,
    CONTEST_STRING_WORK_HARD_UNNOTICED,
    CONTEST_STRING_WORK_BEFORE,
    CONTEST_STRING_APPEAL_NOT_WELL,
    CONTEST_STRING_WORK_PRECEDING,
    CONTEST_STRING_APPEAL_NOT_WELL2,
    CONTEST_STRING_APPEAL_NOT_SHOWN_WELL,
    CONTEST_STRING_APPEAL_SLIGHTLY_WELL,
    CONTEST_STRING_APPEAL_PRETTY_WELL,
    CONTEST_STRING_APPEAL_EXCELLENTLY,
    CONTEST_STRING_APPEAL_DUD,
    CONTEST_STRING_APPEAL_NOT_VERY_WELL,
    CONTEST_STRING_APPEAL_SLIGHTLY_WELL2,
    CONTEST_STRING_APPEAL_PRETTY_WELL2,
    CONTEST_STRING_APPEAL_VERY_WELL,
    CONTEST_STRING_APPEAL_EXCELLENTLY2,
    CONTEST_STRING_SAME_TYPE_GOOD,
    CONTEST_STRING_DIFF_TYPE_GOOD,
    CONTEST_STRING_STOOD_OUT_AS_MUCH,
    CONTEST_STRING_NOT_AS_WELL,
    CONTEST_STRING_CONDITION_ROSE,
    CONTEST_STRING_HOT_STATUS,
    CONTEST_STRING_MOVE_UP_LINE,
    CONTEST_STRING_MOVE_BACK_LINE,
    CONTEST_STRING_SCRAMBLE_ORDER,
    CONTEST_STRING_JUDGE_EXPECTANTLY2,
    CONTEST_STRING_WENT_OVER_WELL,
    CONTEST_STRING_WENT_OVER_VERY_WELL,
    CONTEST_STRING_APPEAL_COMBO_EXCELLENTLY,
    CONTEST_STRING_AVERT_GAZE,
    CONTEST_STRING_AVOID_SEEING,
    CONTEST_STRING_NOT_FAZED,
    CONTEST_STRING_LITTLE_DISTRACTED,
    CONTEST_STRING_ATTEMPT_STARTLE,
    CONTEST_STRING_LOOKED_DOWN,
    CONTEST_STRING_TURNED_BACK,
    CONTEST_STRING_UTTER_CRY,
    CONTEST_STRING_LEAPT_UP,
    CONTEST_STRING_TRIPPED_OVER,
    CONTEST_STRING_MESSED_UP2,
    CONTEST_STRING_FAILED_TARGET_NERVOUS,
    CONTEST_STRING_FAILED_ANYONE_NERVOUS,
    CONTEST_STRING_IGNORED,
    CONTEST_STRING_NO_CONDITION_IMPROVE,
    CONTEST_STRING_BAD_CONDITION_WEAK_APPEAL,
    CONTEST_STRING_UNAFFECTED,
    CONTEST_STRING_ATTRACTED_ATTENTION,
    CONTEST_STRING_NONE = 255
};

struct ContestPokemon
{
    /*0x00*/ u16 species;
    /*0x02*/ u8 nickname[POKEMON_NAME_LENGTH + 1];
    /*0x0D*/ u8 trainerName[8];
    /*0x15*/ u8 trainerGfxId;
    /*0x18*/ u32 aiFlags;
    /*0x1C*/ u8 whichRank:2;
             u8 aiPool_Cool:1;
             u8 aiPool_Beauty:1;
             u8 aiPool_Cute:1;
             u8 aiPool_Smart:1;
             u8 aiPool_Tough:1;
    /*0x1E*/ u16 moves[4]; // moves
    /*0x26*/ u8 cool; // cool
    /*0x27*/ u8 beauty; // beauty
    /*0x28*/ u8 cute; // cute
    /*0x29*/ u8 smart; // smart
    /*0x2A*/ u8 tough; // tough
    /*0x2B*/ u8 sheen; // sheen
    /*0x2C*/ u8 unk2C[12];
    /*0x38*/ u32 personality;  // personality
    /*0x3C*/ u32 otId;  // otId
}; // wow

struct ContestAIInfo {
 /*0x00*/ u8 aiState;
 /*0x02*/ u16 nextMove;
 /*0x04*/ u8 nextMoveIndex;
 /*0x05*/ u8 moveScores[4];
 /*0x09*/ u8 aiAction;
 /*0x0A*/ u8 filler[0x6]; // TODO: don't know what's here
 /*0x10*/ u8 currentAIFlag;
 /*0x14*/ u32 aiFlags;
 /*0x18*/ s16 scriptResult;
 /*0x1A*/ s16 vars[3];
 /*0x20*/ u32 stack[8];
 /*0x40*/ u8 stackSize;
 /*0x41*/ u8 contestantId;
};

extern struct ContestPokemon gContestMons[];

void ResetLinkContestBoolean(void);
void LoadContestBgAfterMoveAnim(void);
void CB2_StartContest(void);
void CreateContestMonFromParty(u8 partyIndex);
void SetContestants(u8 contestType, u8 rank);
u8 GetContestEntryEligibility(struct Pokemon *pkmn);
u16 CalculateContestantRound1Points(u8 who, u8 contestCategory);
void CalculateRound1Points(u8 contestCategory);
bool8 IsSpeciesNotUnown(u16 species);
void CalculateFinalScores(void);
void SortContestants(bool8 useRanking);
bool8 SaveContestWinner(u8 rank);
u8 GetContestWinnerSaveIdx(u8 rank, bool8 shift);
void Contest_ResetWinners(void);
s8 Contest_GetMoveExcitement(u16 move);

// Contest Shared EWRAM

struct ContestTempSave
{
    /*0x18004*/ u16 cachedWindowPalettes[16][16];
    /*0x18204*/ u16 cachedPlttBufferUnfaded[0x200];
    /*0x18604*/ u16 cachedPlttBufferFaded[0x200];
    /*0x18A04*/ u8 savedJunk[0x800];
};

struct ContestMoveAnimData
{
    u16 species;
    u16 targetSpecies;
    bool8 hasTargetAnim:1;
    u8 contestant;
    u32 personality;
    u32 otId;
    u32 targetPersonality;
};

struct Contest
{
    /*0x19204*/ u8 playerMoveChoice;
    /*0x19205*/ u8 appealNumber;
    /*0x19206*/ u8 unk19206[4];    // seems to only be used by an unref function
    /*0x1920A*/ bool16 unk1920A_0:1;  // Task active flags?
                bool16 unk1920A_1:1;
                bool16 unk1920A_2:1;
                bool16 unk1920A_3:1;
                bool16 waitForJudgeSpeechBubble:1;
                bool16 isShowingApplauseMeter:1;
                bool16 applauseMeterIsMoving:1;
                bool16 animatingAudience:1;
    /*0x1920B*/ bool16 waitForAudienceBlend:1;
                bool16 sliderHeartsAnimating:1;
                bool16 waitForLink:1;
    /*0x1920C*/ u8 mainTaskId;
    /*0x1920D*/ u8 unk1920D[4];
    /*0x19211*/ u8 judgeAttentionTaskId;
    /*0x19212*/ u8 blendTaskId;
    /*0x19213*/ u8 filler19213;
    /*0x19214*/ u8 turnNumber;
    /*0x19215*/ u8 currentContestant;
    /*0x19216*/ u8 judgeSpeechBubbleSpriteId;
    /*0x19217*/ s8 applauseLevel;
    /*0x19218*/ u8 prevTurnOrder[4];
    /*0x1921C*/ u32 unusedRng;   // saved RNG value?
                u16 moveHistory[5][4];  // move history?
                u8 excitementHistory[5][4];  // excitement history
                u8 applauseMeterSpriteId;    // sprite ID
    /*0x1925D*/ u8 contestSetupState;
    /*0x1925E*/ u8 moveAnimTurnCount;
};

struct ContestantStatus
{
 /*0x00*/ s16 baseAppeal;
 /*0x02*/ s16 appeal;
 /*0x04*/ s16 pointTotal;
 /*0x06*/ u16 currMove;
 /*0x08*/ u16 prevMove;
 /*0x0A*/ u8 moveCategory;
 /*0x0B*/ u8 ranking:2;
          u8 unkB_2:2; // unused
          u8 moveRepeatCount:3;
          bool8 noMoreTurns:1;  // used a one-time move?
 /*0x0C*/ bool8 nervous:1;
          u8 numTurnsSkipped:2;
 /*0x0D*/ s8 condition;
 /*0x0E*/ u8 jam;
 /*0x0F*/ u8 jamReduction;

          // Flags set by move effect
 /*0x10*/ bool8 resistant:1;
          bool8 immune:1;
          bool8 moreEasilyStartled:1;
          bool8 usedRepeatableMove:1;
          u8 conditionMod:2; // 1: just raised condition; 2: appeal greatly improved by condition
          u8 turnOrderMod:2; // 1: defined; 2: random
 /*0x11*/ u8 turnOrderModAction:2; // 1: made first; 2: made last; 3: made random
          bool8 turnSkipped:1;
          bool8 exploded:1;
          bool8 overrideCategoryExcitementMod:1;
          bool8 appealTripleCondition:1;

 /*0x12*/ u8 jamSafetyCount;
 /*0x13*/ u8 effectStringId;   // status action?
 /*0x14*/ u8 effectStringId2;
 /*0x15*/ bool8 repeatedMove:1;
          bool8 unk15_1:1; // unused
          bool8 repeatedPrevMove:1;
          bool8 completedComboFlag:1;
          bool8 hasJudgesAttention:1;
          bool8 judgesAttentionWasRemoved:1;
          bool8 usedComboMove:1;
 /*0x16*/ bool8 completedCombo;
 /*0x17*/ u8 comboAppealBonus;
 /*0x18*/ u8 repeatJam;
 /*0x19*/ u8 nextTurnOrder;  // turn position
 /*0x1A*/ u8 attentionLevel;  // How much the Pokemon "stood out"
 /*0x1B*/ u8 contestantAnimTarget;
};

struct ContestGraphicsState
{
    u8 sliderHeartSpriteId;
    u8 nextTurnSpriteId;
    bool8 sliderUpdating:1;
    bool8 boxBlinking:1;
    bool8 updatingAppealHearts:1;
};

struct ContestExcitement
{
    s8 moveExcitement;
    bool8 frozen:1;
    u8 freezer:3;
    s8 excitementAppealBonus;
};

struct ContestAppealMoveResults
{
    u8 turnOrder[4];
    s16 jam;
    s16 jam2;
    u8 jamQueue[5];
    u8 unnervedPokes[4];
    u8 contestant;
};

struct ContestFinalStandings
{
    s32 totalPoints;
    s32 round1Points;
    s32 random;
    s32 contestant;
};

extern u8 gContestPlayerMonIndex;
extern u8 gLinkContestFlags;
extern u32 gContestRngValue;
extern u8 gContestantTurnOrder[4];
extern s16 gContestMonRound1Points[4];
extern s16 gContestMonTotalPoints[4];
extern s16 gContestMonAppealPointTotals[4];
extern s16 gContestMonRound2Points[4];
extern u8 gContestFinalStandings[4];
extern u8 gContestantTurnOrder[4];
extern u8 gContestLinkLeaderIndex;
extern u16 gSpecialVar_ContestRank;
extern u16 gSpecialVar_ContestCategory;
extern u8 gContestMonPartyIndex;

void SetContestantEffectStringID(u8 contestant, u8 effectStringId);
void SetContestantEffectStringID2(u8 contestant, u8 effectStringId);
void MakeContestantNervous(u8 p);
bool8 Contest_IsMonsTurnDisabled(u8 contestant);
bool8 IsContestantAllowedToCombo(u8 contestant);
void SetStartledString(u8 contestant, u8 jam);

// Ruby keeps its fixed shared-memory views instead of heap resources.
#define gCurContestWinner               ((*(struct ContestWinner*)(gSharedMem + 0x15DE0)))
#define gCurContestWinnerIsForArtist    (gSharedMem[0x15DDF])
#define gCurContestWinnerSaveIdx        (gSharedMem[0x15DDE])
#define eContest                        (*(struct Contest *)(gSharedMem + 0x19204))
#define eContestantStatus               ((struct ContestantStatus *)(gSharedMem + 0x19260))
#define eContestAppealResults           (*(struct ContestAppealMoveResults *)(gSharedMem + 0x192D0))
#define eContestAI                      ((struct ContestAIInfo *)(gSharedMem + 0x192E4))
#define eContestExcitement              (*(struct ContestExcitement *)(gSharedMem + 0x19328))
#define eContestGfxState                ((struct ContestGraphicsState *)(gSharedMem + 0x19338))
#define eUnzippedContestAudience_Gfx    (&gSharedMem[0x15800]) // overlaps other contest vars
#define eContestDebugMode         (gSharedMem[0x18000])
#define eContestTempSave                (*(struct ContestTempSave *)(gSharedMem + 0x18004))

#endif // GUARD_CONTEST_H
