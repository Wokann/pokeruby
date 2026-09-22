#include "global.h"
#include "battle_anim.h"
#include "contest.h"
#include "ewram.h"
#include "rom_8077ABC.h"
#include "sound.h"
#include "task.h"
#include "constants/sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u16 gBattlerPartyIndexes[];
extern u16 gAnimSpeciesByBanks[];
extern u8 gAnimCustomPanning;

static void SoundTask_FireBlast_Step1(u8 taskId);
static void SoundTask_FireBlast_Step2(u8 taskId);
static void SoundTask_LoopSEAdjustPanning_Step(u8 taskId);
static void SoundTask_AdjustPanningVar_Step(u8 taskId);

// used in 1 move:
//         Move_FIRE_BLAST
void SoundTask_FireBlast(u8 taskId)
{
    s8 sourcePan, targetPan, panIncrement;
    
    TASK.data[0] = gBattleAnimArgs[0];
    TASK.data[1] = gBattleAnimArgs[1];

    sourcePan = BattleAnimAdjustPanning(SOUND_PAN_ATTACKER_NEG);
    targetPan = BattleAnimAdjustPanning(SOUND_PAN_TARGET);
    panIncrement = CalculatePanIncrement(sourcePan, targetPan, 2);

    TASK.data[2] = sourcePan;
    TASK.data[3] = targetPan;
    TASK.data[4] = panIncrement;
    TASK.data[10] = 10;

    TASK.func = SoundTask_FireBlast_Step1;
}

static void SoundTask_FireBlast_Step1(u8 taskId)
{
    s16 pan = TASK.data[2];
    s8 dPan = TASK.data[4];

    if (++TASK.data[11] == 111)
    {
        TASK.data[10] = 5;
        TASK.data[11] = 0;
        TASK.func = SoundTask_FireBlast_Step2;
    } 
    else
    {
        if (++TASK.data[10] == 11)
        {
            TASK.data[10] = 0;
            PlaySE12WithPanning(TASK.data[0], pan);
        }
        pan += dPan;
        TASK.data[2] = KeepPanInRange(pan, dPan);
    }
}

static void SoundTask_FireBlast_Step2(u8 taskId)
{
    s8 pan;

    if (++TASK.data[10] == 6)
    {
        TASK.data[10] = 0;

        pan = BattleAnimAdjustPanning(SOUND_PAN_TARGET);
        PlaySE12WithPanning(TASK.data[1], pan);

        if (++TASK.data[11] == 2)
        {
            DestroyAnimSoundTask(taskId);
        }
    }
}

// used in 7 moves:
//         Move_ICE_BEAM, Move_AURORA_BEAM, Move_PSYBEAM,
//         Move_PSYWAVE, Move_SHADOW_BALL, Move_TRI_ATTACK,
//         Move_HYPER_BEAM
void SoundTask_LoopSEAdjustPanning(u8 taskId)
{
    u16 songId = gBattleAnimArgs[0];
    s8 targetPan = gBattleAnimArgs[2];
    s8 panIncrement = gBattleAnimArgs[3];
    u8 r10 = gBattleAnimArgs[4]; // number of times the sound must be played
    u8 r7 = gBattleAnimArgs[5];
    u8 r9 = gBattleAnimArgs[6];
    s8 sourcePan = BattleAnimAdjustPanning(gBattleAnimArgs[1]);

    targetPan = BattleAnimAdjustPanning(targetPan);
    panIncrement = CalculatePanIncrement(sourcePan, targetPan, panIncrement);

    TASK.data[0] = songId;
    TASK.data[1] = sourcePan;
    TASK.data[2] = targetPan;
    TASK.data[3] = panIncrement;
    TASK.data[4] = r10;
    TASK.data[5] = r7;
    TASK.data[6] = r9;
    TASK.data[10] = 0;
    TASK.data[11] = sourcePan;
    TASK.data[12] = r9;

    TASK.func = SoundTask_LoopSEAdjustPanning_Step;
    TASK.func(taskId);
}

static void SoundTask_LoopSEAdjustPanning_Step(u8 taskId)
{
    if (TASK.data[12]++ == TASK.data[6])
    {
        TASK.data[12] = 0;
        PlaySE12WithPanning(TASK.data[0], TASK.data[11]);

        if (--TASK.data[4] == 0)
        {
            DestroyAnimSoundTask(taskId);
            return;
        }
    }

    if (TASK.data[10]++ == TASK.data[5])
    {
        u16 dPan, oldPan;
        TASK.data[10] = 0;
        dPan = TASK.data[3];
        oldPan = TASK.data[11];
        TASK.data[11] = dPan + oldPan;
        TASK.data[11] = KeepPanInRange(TASK.data[11], oldPan);
    }
}

// Used by Move_HOWL, Move_ROAR, and Move_GROWL.
void SoundTask_PlayCryWithMode(u8 taskId)
{
    u16 species = 0;
    s8 pan = BattleAnimAdjustPanning(SOUND_PAN_ATTACKER_NEG);

    if (IsContest())
    {
        if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
            species = gContestResources__moveAnim.species;
        else
            DestroyAnimVisualTask(taskId);
    }
    else
    {
        u8 battler;
        if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
            battler = gBattleAnimAttacker;
        else if (gBattleAnimArgs[0] == ANIM_BATTLER_TARGET)
            battler = gBattleAnimTarget;
        else if (gBattleAnimArgs[0] == ANIM_BATTLER_ATK_PARTNER)
            battler = gBattleAnimAttacker ^ 0x2;
        else
            battler = gBattleAnimTarget ^ 0x2;

        if (gBattleAnimArgs[0] == ANIM_BATTLER_TARGET || gBattleAnimArgs[0] == ANIM_BATTLER_DEF_PARTNER)
        {
            if (!IsAnimBankSpriteVisible(battler))
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

    if (species != 0)
    {
        s16 mode = gBattleAnimArgs[1];
        if (mode == ANIM_CRY_MODE_DEFAULT)
            PlayCry_Normal(species, pan);
        else
            PlayCry3(species, pan, mode);
    }

    DestroyAnimVisualTask(taskId);
}

void SoundTask_PlayHyperVoiceCry(u8 taskId)
{
    u16 species;
    s8 pan = BattleAnimAdjustPanning(SOUND_PAN_ATTACKER_NEG);

    if (IsContest())
        species = gContestResources__moveAnim.species;
    else
        species = gAnimSpeciesByBanks[gBattleAnimAttacker];

    if (species != 0)
        PlayCry3(species, pan, CRY_MODE_HYPER_VOICE);

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

// used in 2 moves:
//         Move_CONFUSE_RAY, Move_WILL_O_WISP
void SoundTask_AdjustPanningVar(u8 taskId)
{
    u8 r5 = gBattleAnimArgs[1];
    s8 panIncrement = gBattleAnimArgs[2];
    s16 r9 = gBattleAnimArgs[3];
    s8 r1 = gBattleAnimArgs[0];

    s8 sourcePan = BattleAnimAdjustPanning(r1);
    s8 targetPan = BattleAnimAdjustPanning(r5);
    panIncrement = CalculatePanIncrement(sourcePan, targetPan, panIncrement);

    TASK.data[1] = sourcePan;
    TASK.data[2] = targetPan;
    TASK.data[3] = panIncrement;
    TASK.data[5] = r9;
    TASK.data[10] = 0;
    TASK.data[11] = sourcePan;

    TASK.func = SoundTask_AdjustPanningVar_Step;
    TASK.func(taskId);
}

static void SoundTask_AdjustPanningVar_Step(u8 taskId)
{
    u16 dPan = TASK.data[3];

    if (TASK.data[10]++ == TASK.data[5])
    {
        u16 oldPan;
        TASK.data[10] = 0;
        oldPan = TASK.data[11];
        TASK.data[11] = dPan + oldPan;
        TASK.data[11] = KeepPanInRange(TASK.data[11], oldPan);
    }

    gAnimCustomPanning = TASK.data[11];

    if (TASK.data[11] == TASK.data[2])
    {
        DestroyAnimVisualTask(taskId);
    }
}
