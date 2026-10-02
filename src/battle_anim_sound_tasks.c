#include "global.h"
#include "contest.h"
#include "ewram.h"
#include "battle_anim.h"
#include "sound.h"
#include "task.h"
#include "constants/sound.h"
#include "constants/species.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u16 gBattlerPartyIndexes[];
extern u16 gAnimBattlerSpecies[];
extern u8 gAnimCustomPanning;

static void SoundTask_FireBlast_Step1(u8 taskId);
static void SoundTask_FireBlast_Step2(u8 taskId);
static void SoundTask_LoopSEAdjustPanning_Step(u8 taskId);
static void SoundTask_AdjustPanningVar_Step(u8 taskId);

#define tFireBlastLoopSongId  data[0]
#define tFireBlastEndSongId   data[1]
#define tFireBlastCurrentPan  data[2]
#define tFireBlastTargetPan   data[3]
#define tFireBlastPanIncrement data[4]
#define tFireBlastTimer       data[10]
#define tFireBlastPhaseCounter data[11]

// used in 1 move:
//         Move_FIRE_BLAST
void SoundTask_FireBlast(u8 taskId)
{
    s8 sourcePan, targetPan, panIncrement;
    
    TASK.tFireBlastLoopSongId = gBattleAnimArgs[0];
    TASK.tFireBlastEndSongId = gBattleAnimArgs[1];

    sourcePan = BattleAnimAdjustPanning(SOUND_PAN_ATTACKER);
    targetPan = BattleAnimAdjustPanning(SOUND_PAN_TARGET);
    panIncrement = CalculatePanIncrement(sourcePan, targetPan, 2);

    TASK.tFireBlastCurrentPan = sourcePan;
    TASK.tFireBlastTargetPan = targetPan;
    TASK.tFireBlastPanIncrement = panIncrement;
    TASK.tFireBlastTimer = 10;

    TASK.func = SoundTask_FireBlast_Step1;
}

static void SoundTask_FireBlast_Step1(u8 taskId)
{
    s16 pan = TASK.tFireBlastCurrentPan;
    s8 panIncrement = TASK.tFireBlastPanIncrement;

    if (++TASK.tFireBlastPhaseCounter == 111)
    {
        TASK.tFireBlastTimer = 5;
        TASK.tFireBlastPhaseCounter = 0;
        TASK.func = SoundTask_FireBlast_Step2;
    } 
    else
    {
        if (++TASK.tFireBlastTimer == 11)
        {
            TASK.tFireBlastTimer = 0;
            PlaySE12WithPanning(TASK.tFireBlastLoopSongId, pan);
        }
        pan += panIncrement;
        TASK.tFireBlastCurrentPan = KeepPanInRange(pan, panIncrement);
    }
}

static void SoundTask_FireBlast_Step2(u8 taskId)
{
    s8 pan;

    if (++TASK.tFireBlastTimer == 6)
    {
        TASK.tFireBlastTimer = 0;

        pan = BattleAnimAdjustPanning(SOUND_PAN_TARGET);
        PlaySE12WithPanning(TASK.tFireBlastEndSongId, pan);

        if (++TASK.tFireBlastPhaseCounter == 2)
        {
            DestroyAnimSoundTask(taskId);
        }
    }
}

#undef tFireBlastLoopSongId
#undef tFireBlastEndSongId
#undef tFireBlastCurrentPan
#undef tFireBlastTargetPan
#undef tFireBlastPanIncrement
#undef tFireBlastTimer
#undef tFireBlastPhaseCounter

#define tLoopSoundEffectId     data[0]
#define tLoopSourcePan         data[1]
#define tLoopTargetPan         data[2]
#define tLoopPanIncrement      data[3]
#define tLoopCount             data[4]
#define tLoopPanDelay          data[5]
#define tLoopSoundDelay        data[6]
#define tLoopPanDelayCounter   data[10]
#define tLoopCurrentPan        data[11]
#define tLoopSoundDelayCounter data[12]

// used in 7 moves:
//         Move_ICE_BEAM, Move_AURORA_BEAM, Move_PSYBEAM,
//         Move_PSYWAVE, Move_SHADOW_BALL, Move_TRI_ATTACK,
//         Move_HYPER_BEAM
void SoundTask_LoopSEAdjustPanning(u8 taskId)
{
    u16 songId = gBattleAnimArgs[0];
    s8 targetPan = gBattleAnimArgs[2];
    s8 panIncrement = gBattleAnimArgs[3];
    u8 numLoops = gBattleAnimArgs[4];
    u8 panDelay = gBattleAnimArgs[5];
    u8 soundDelay = gBattleAnimArgs[6];
    s8 sourcePan = BattleAnimAdjustPanning(gBattleAnimArgs[1]);

    targetPan = BattleAnimAdjustPanning(targetPan);
    panIncrement = CalculatePanIncrement(sourcePan, targetPan, panIncrement);

    TASK.tLoopSoundEffectId = songId;
    TASK.tLoopSourcePan = sourcePan;
    TASK.tLoopTargetPan = targetPan;
    TASK.tLoopPanIncrement = panIncrement;
    TASK.tLoopCount = numLoops;
    TASK.tLoopPanDelay = panDelay;
    TASK.tLoopSoundDelay = soundDelay;
    TASK.tLoopPanDelayCounter = 0;
    TASK.tLoopCurrentPan = sourcePan;
    TASK.tLoopSoundDelayCounter = soundDelay;

    TASK.func = SoundTask_LoopSEAdjustPanning_Step;
    TASK.func(taskId);
}

static void SoundTask_LoopSEAdjustPanning_Step(u8 taskId)
{
    if (TASK.tLoopSoundDelayCounter++ == TASK.tLoopSoundDelay)
    {
        TASK.tLoopSoundDelayCounter = 0;
        PlaySE12WithPanning(TASK.tLoopSoundEffectId, TASK.tLoopCurrentPan);

        if (--TASK.tLoopCount == 0)
        {
            DestroyAnimSoundTask(taskId);
            return;
        }
    }

    if (TASK.tLoopPanDelayCounter++ == TASK.tLoopPanDelay)
    {
        u16 panIncrement, oldPan;
        TASK.tLoopPanDelayCounter = 0;
        panIncrement = TASK.tLoopPanIncrement;
        oldPan = TASK.tLoopCurrentPan;
        TASK.tLoopCurrentPan = panIncrement + oldPan;
        TASK.tLoopCurrentPan = KeepPanInRange(TASK.tLoopCurrentPan, oldPan);
    }
}

#undef tLoopSoundEffectId
#undef tLoopSourcePan
#undef tLoopTargetPan
#undef tLoopPanIncrement
#undef tLoopCount
#undef tLoopPanDelay
#undef tLoopSoundDelay
#undef tLoopPanDelayCounter
#undef tLoopCurrentPan
#undef tLoopSoundDelayCounter

// Used by Move_HOWL, Move_ROAR, and Move_GROWL.
void SoundTask_PlayCryWithMode(u8 taskId)
{
    u16 species = SPECIES_NONE;
    s8 pan = BattleAnimAdjustPanning(SOUND_PAN_ATTACKER);

    if (IsContest())
    {
        if (gBattleAnimArgs[0] == ANIM_ATTACKER)
            species = gContestResources__moveAnim.species;
        else
            DestroyAnimVisualTask(taskId);
    }
    else
    {
        u8 battler;
        if (gBattleAnimArgs[0] == ANIM_ATTACKER)
            battler = gBattleAnimAttacker;
        else if (gBattleAnimArgs[0] == ANIM_TARGET)
            battler = gBattleAnimTarget;
        else if (gBattleAnimArgs[0] == ANIM_ATK_PARTNER)
            battler = gBattleAnimAttacker ^ 0x2;
        else
            battler = gBattleAnimTarget ^ 0x2;

        if (gBattleAnimArgs[0] == ANIM_TARGET || gBattleAnimArgs[0] == ANIM_DEF_PARTNER)
        {
            if (!IsBattlerSpriteVisible(battler))
            {
                DestroyAnimVisualTask(taskId);
                return;
            }
        }

        if (GetBattlerSide(battler))
            species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
        else
            species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
    }

    if (species != SPECIES_NONE)
    {
        s16 cryMode = gBattleAnimArgs[1];
        if (cryMode == ANIM_CRY_MODE_DEFAULT)
            PlayCry_Normal(species, pan);
        else
            PlayCry_ByMode(species, pan, cryMode);
    }

    DestroyAnimVisualTask(taskId);
}

void SoundTask_PlayHyperVoiceCry(u8 taskId)
{
    u16 species;
    s8 pan = BattleAnimAdjustPanning(SOUND_PAN_ATTACKER);

    if (IsContest())
        species = gContestResources__moveAnim.species;
    else
        species = gAnimBattlerSpecies[gBattleAnimAttacker];

    if (species != SPECIES_NONE)
        PlayCry_ByMode(species, pan, CRY_MODE_HYPER_VOICE);

    DestroyAnimVisualTask(taskId);
}

// used in 6 moves:
//         Move_SKY_ATTACK, Move_LUSTER_PURGE, Move_FLATTER,
//         Move_DRAGON_CLAW, Move_RETURN, Move_COSMIC_POWER,
void SoundTask_PlaySE1WithPanning(u8 taskId)
{
    u16 songId = gBattleAnimArgs[0];
    s8 pan = BattleAnimAdjustPanning(gBattleAnimArgs[1]);
    PlaySE1WithPanning(songId, pan);

    DestroyAnimVisualTask(taskId);
}

// used in 6 moves:
//         Move_SKY_ATTACK, Move_SUPERPOWER, Move_ENCORE,
//         Move_FLATTER, Move_RETURN, Move_COSMIC_POWER
void SoundTask_PlaySE2WithPanning(u8 taskId)
{
    u16 songId = gBattleAnimArgs[0];
    s8 pan = BattleAnimAdjustPanning(gBattleAnimArgs[1]);
    PlaySE2WithPanning(songId, pan);

    DestroyAnimVisualTask(taskId);
}

#define tAdjustSourcePan       data[1]
#define tAdjustTargetPan       data[2]
#define tAdjustPanIncrement    data[3]
#define tAdjustPanDelay        data[5]
#define tAdjustPanDelayCounter data[10]
#define tAdjustCurrentPan      data[11]

// used in 2 moves:
//         Move_CONFUSE_RAY, Move_WILL_O_WISP
void SoundTask_AdjustPanningVar(u8 taskId)
{
    s8 targetPan = gBattleAnimArgs[1];
    s8 panIncrement = gBattleAnimArgs[2];
    s16 panDelay = gBattleAnimArgs[3];
    s8 sourcePan = BattleAnimAdjustPanning(gBattleAnimArgs[0]);

    targetPan = BattleAnimAdjustPanning(targetPan);
    panIncrement = CalculatePanIncrement(sourcePan, targetPan, panIncrement);

    TASK.tAdjustSourcePan = sourcePan;
    TASK.tAdjustTargetPan = targetPan;
    TASK.tAdjustPanIncrement = panIncrement;
    TASK.tAdjustPanDelay = panDelay;
    TASK.tAdjustPanDelayCounter = 0;
    TASK.tAdjustCurrentPan = sourcePan;

    TASK.func = SoundTask_AdjustPanningVar_Step;
    TASK.func(taskId);
}

static void SoundTask_AdjustPanningVar_Step(u8 taskId)
{
    u16 panIncrement = TASK.tAdjustPanIncrement;

    if (TASK.tAdjustPanDelayCounter++ == TASK.tAdjustPanDelay)
    {
        u16 oldPan;
        TASK.tAdjustPanDelayCounter = 0;
        oldPan = TASK.tAdjustCurrentPan;
        TASK.tAdjustCurrentPan = panIncrement + oldPan;
        TASK.tAdjustCurrentPan = KeepPanInRange(TASK.tAdjustCurrentPan, oldPan);
    }

    gAnimCustomPanning = TASK.tAdjustCurrentPan;

    if (TASK.tAdjustCurrentPan == TASK.tAdjustTargetPan)
    {
        DestroyAnimVisualTask(taskId);
    }
}

#undef tAdjustSourcePan
#undef tAdjustTargetPan
#undef tAdjustPanIncrement
#undef tAdjustPanDelay
#undef tAdjustPanDelayCounter
#undef tAdjustCurrentPan
