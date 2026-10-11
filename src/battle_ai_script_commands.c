#include "global.h"
#include "battle_ai_script_commands.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "battle.h"
#include "battle_main.h"
#include "constants/battle_move_effects.h"
#include "data.h"
#include "item.h"
#include "constants/moves.h"
#include "pokemon.h"
#include "random.h"
#include "battle_anim.h"
#include "constants/species.h"
#include "util.h"
#include "ewram.h"

extern u8 gUnknown_02023A14_50;
extern u32 gUnknown_02023A14_4C;
extern u16 gBattleTypeFlags;
extern u16 gBattleWeather;
extern u8 gActiveBattler;
extern u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT];
extern u16 gCurrentMove;
extern int gBattleMoveDamage;
extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u8 gAbsentBattlerFlags;
extern u8 gMoveResultFlags;
extern u16 gDynamicBasePower;
extern u16 gLastMoves[MAX_BATTLERS_COUNT];
extern u32 gStatuses3[MAX_BATTLERS_COUNT];
extern u16 gSideStatuses[2];
extern struct BattlePokemon gBattleMons[MAX_BATTLERS_COUNT];
extern u8 gCritMultiplier;
extern u16 gTrainerBattleOpponent_A;
extern u8 *gBattleAI_ScriptsTable[];

/*
gAIScriptPtr is a pointer to the next battle AI cmd command to read.
when a command finishes processing, gAIScriptPtr is incremented by
the number of bytes that the current command had reserved for arguments
in order to read the next command correctly. refer to battle_ai_scripts.s for the
AI scripts.
*/
EWRAM_DATA u8 *gAIScriptPtr = NULL;

// AI states
enum
{
    AIState_SettingUp,
    AIState_Processing,
    AIState_FinishedProcessing,
    AIState_DoNotProcess
};

static void BattleAI_DoAIProcessing(void);
static void AIStackPushVar(u8 *var);
static bool8 AIStackPop(void);

static void Cmd_if_random_less_than(void);
static void Cmd_if_random_greater_than(void);
static void Cmd_if_random_equal(void);
static void Cmd_if_random_not_equal(void);
static void Cmd_score(void);
static void Cmd_if_hp_less_than(void);
static void Cmd_if_hp_more_than(void);
static void Cmd_if_hp_equal(void);
static void Cmd_if_hp_not_equal(void);
static void Cmd_if_status(void);
static void Cmd_if_not_status(void);
static void Cmd_if_status2(void);
static void Cmd_if_not_status2(void);
static void Cmd_if_status3(void);
static void Cmd_if_not_status3(void);
static void Cmd_if_side_affecting(void);
static void Cmd_if_not_side_affecting(void);
static void Cmd_if_less_than(void);
static void Cmd_if_more_than(void);
static void Cmd_if_equal(void);
static void Cmd_if_not_equal(void);
static void Cmd_if_less_than_ptr(void);
static void Cmd_if_more_than_ptr(void);
static void Cmd_if_equal_ptr(void);
static void Cmd_if_not_equal_ptr(void);
static void Cmd_if_move(void);
static void Cmd_if_not_move(void);
static void Cmd_if_in_bytes(void);
static void Cmd_if_not_in_bytes(void);
static void Cmd_if_in_hwords(void);
static void Cmd_if_not_in_hwords(void);
static void Cmd_if_user_has_attacking_move(void);
static void Cmd_if_user_has_no_attacking_moves(void);
static void Cmd_get_turn_count(void);
static void Cmd_get_type(void);
static void Cmd_get_considered_move_power(void);
static void Cmd_get_how_powerful_move_is(void);
static void Cmd_get_last_used_battler_move(void);
static void Cmd_if_equal_(void);
static void Cmd_if_not_equal_(void);
static void Cmd_if_user_goes(void);
static void Cmd_if_user_doesnt_go(void);
static void Cmd_nop_2A(void);
static void Cmd_nop_2B(void);
static void Cmd_count_usable_party_mons(void);
static void Cmd_get_considered_move(void);
static void Cmd_get_considered_move_effect(void);
static void BattleAICmd_get_ability(void);
static void Cmd_get_highest_type_effectiveness(void);
static void Cmd_if_type_effectiveness(void);
static void Cmd_nop_32(void);
static void Cmd_nop_33(void);
static void BattleAICmd_if_status_in_party(void);
static void BattleAICmd_if_status_not_in_party(void);
static void Cmd_get_weather(void);
static void Cmd_if_effect(void);
static void Cmd_if_not_effect(void);
static void Cmd_if_stat_level_less_than(void);
static void Cmd_if_stat_level_more_than(void);
static void Cmd_if_stat_level_equal(void);
static void Cmd_if_stat_level_not_equal(void);
static void Cmd_if_can_faint(void);
static void Cmd_if_cant_faint(void);
static void BattleAICmd_if_has_move(void);
static void BattleAICmd_if_dont_have_move(void);
static void BattleAICmd_if_move_effect(void);
static void BattleAICmd_if_not_move_effect(void);
static void Cmd_if_any_move_disabled_or_encored(void);
static void Cmd_if_curr_move_disabled_or_encored(void);
static void Cmd_flee(void);
static void Cmd_if_random_safari_flee(void);
static void Cmd_watch(void);
static void BattleAICmd_get_hold_effect(void);
static void Cmd_get_gender(void);
static void Cmd_is_first_turn_for(void);
static void Cmd_get_stockpile_count(void);
static void Cmd_is_double_battle(void);
static void Cmd_get_used_held_item(void);
static void Cmd_get_move_type_from_result(void);
static void Cmd_get_move_power_from_result(void);
static void Cmd_get_move_effect_from_result(void);
static void Cmd_get_protect_count(void);
static void Cmd_nop_52(void);
static void Cmd_nop_53(void);
static void Cmd_nop_54(void);
static void Cmd_nop_55(void);
static void Cmd_nop_56(void);
static void Cmd_nop_57(void);
static void Cmd_call(void);
static void Cmd_goto(void);
static void Cmd_end(void);
static void Cmd_if_level_cond(void);
static void Cmd_if_target_taunted(void);
static void Cmd_if_target_not_taunted(void);

typedef void (*BattleAICmdFunc)(void);

static const BattleAICmdFunc sBattleAICmdTable[] =
{
    Cmd_if_random_less_than,         // 0x0
    Cmd_if_random_greater_than,      // 0x1
    Cmd_if_random_equal,             // 0x2
    Cmd_if_random_not_equal,         // 0x3
    Cmd_score,                       // 0x4
    Cmd_if_hp_less_than,             // 0x5
    Cmd_if_hp_more_than,             // 0x6
    Cmd_if_hp_equal,                 // 0x7
    Cmd_if_hp_not_equal,             // 0x8
    Cmd_if_status,                   // 0x9
    Cmd_if_not_status,               // 0xA
    Cmd_if_status2,                  // 0xB
    Cmd_if_not_status2,              // 0xC
    Cmd_if_status3,                  // 0xD
    Cmd_if_not_status3,              // 0xE
    Cmd_if_side_affecting,                  // 0xF
    Cmd_if_not_side_affecting,              // 0x10
    Cmd_if_less_than,                // 0x11
    Cmd_if_more_than,                // 0x12
    Cmd_if_equal,                    // 0x13
    Cmd_if_not_equal,                // 0x14
    Cmd_if_less_than_ptr,             // 0x15
    Cmd_if_more_than_ptr,             // 0x16
    Cmd_if_equal_ptr,                 // 0x17
    Cmd_if_not_equal_ptr,             // 0x18
    Cmd_if_move,                     // 0x19
    Cmd_if_not_move,                 // 0x1A
    Cmd_if_in_bytes,                 // 0x1B
    Cmd_if_not_in_bytes,             // 0x1C
    Cmd_if_in_hwords,                 // 0x1D
    Cmd_if_not_in_hwords,             // 0x1E
    Cmd_if_user_has_attacking_move,          // 0x1F
    Cmd_if_user_has_no_attacking_moves,         // 0x20
    Cmd_get_turn_count,              // 0x21
    Cmd_get_type,                    // 0x22
    Cmd_get_considered_move_power,              // 0x23
    Cmd_get_how_powerful_move_is,       // 0x24
    Cmd_get_last_used_battler_move,                    // 0x25
    Cmd_if_equal_,                // 0x26
    Cmd_if_not_equal_,            // 0x27
    Cmd_if_user_goes,           // 0x28
    Cmd_if_user_doesnt_go,       // 0x29
    Cmd_nop_2A,                  // 0x2A
    Cmd_nop_2B,                  // 0x2B
    Cmd_count_usable_party_mons,         // 0x2C
    Cmd_get_considered_move,         // 0x2D
    Cmd_get_considered_move_effect,  // 0x2E
    BattleAICmd_get_ability,                 // 0x2F
    Cmd_get_highest_type_effectiveness, // 0x30
    Cmd_if_type_effectiveness,             // 0x31
    Cmd_nop_32,                  // 0x32
    Cmd_nop_33,                  // 0x33
    BattleAICmd_if_status_in_party,          // 0x34
    BattleAICmd_if_status_not_in_party,      // 0x35
    Cmd_get_weather,                 // 0x36
    Cmd_if_effect,                   // 0x37
    Cmd_if_not_effect,               // 0x38
    Cmd_if_stat_level_less_than,     // 0x39
    Cmd_if_stat_level_more_than,     // 0x3A
    Cmd_if_stat_level_equal,         // 0x3B
    Cmd_if_stat_level_not_equal,     // 0x3C
    Cmd_if_can_faint,                // 0x3D
    Cmd_if_cant_faint,               // 0x3E
    BattleAICmd_if_has_move,                 // 0x3F
    BattleAICmd_if_dont_have_move,           // 0x40
    BattleAICmd_if_move_effect,              // 0x41
    BattleAICmd_if_not_move_effect,          // 0x42
    Cmd_if_any_move_disabled_or_encored,     // 0x43
    Cmd_if_curr_move_disabled_or_encored,                  // 0x44
    Cmd_flee,                        // 0x45
    Cmd_if_random_safari_flee,               // 0x46
    Cmd_watch,                       // 0x47
    BattleAICmd_get_hold_effect,             // 0x48
    Cmd_get_gender,                  // 0x49
    Cmd_is_first_turn_for,               // 0x4A
    Cmd_get_stockpile_count,         // 0x4B
    Cmd_is_double_battle,            // 0x4C
    Cmd_get_used_held_item,               // 0x4D
    Cmd_get_move_type_from_result,   // 0x4E
    Cmd_get_move_power_from_result,  // 0x4F
    Cmd_get_move_effect_from_result, // 0x50
    Cmd_get_protect_count,           // 0x51
    Cmd_nop_52,                  // 0x52
    Cmd_nop_53,                  // 0x53
    Cmd_nop_54,                  // 0x54
    Cmd_nop_55,                  // 0x55
    Cmd_nop_56,                  // 0x56
    Cmd_nop_57,                  // 0x57
    Cmd_call,                        // 0x58
    Cmd_goto,                        // 0x59
    Cmd_end,                         // 0x5A
    Cmd_if_level_cond,            // 0x5B
    Cmd_if_target_taunted,                  // 0x5C
    Cmd_if_target_not_taunted,              // 0x5D
};

static const u16 sIgnoredPowerfulMoveEffects[] =
{
    EFFECT_EXPLOSION,
    EFFECT_DREAM_EATER,
    EFFECT_RAZOR_WIND,
    EFFECT_SKY_ATTACK,
    EFFECT_RECHARGE,
    EFFECT_SKULL_BASH,
    EFFECT_SOLAR_BEAM,
    EFFECT_SPIT_UP,
    EFFECT_FOCUS_PUNCH,
    EFFECT_SUPERPOWER,
    EFFECT_ERUPTION,
    EFFECT_OVERHEAT,
    0xFFFF
};

// if the AI is a Link battle, safari, battle tower, or ereader, it will ignore considering item uses.
void BattleAI_HandleItemUseBeforeAISetup(void)
{
    s32 i;
    u8 *data;

    MEMSET_ALT(BATTLE_HISTORY, 0, sizeof(struct BattleHistory), i, data);

    if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER)
     && gTrainerBattleOpponent_A != 0x400
     && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_SAFARI | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_EREADER_TRAINER)))
    {
        for (i = 0; i < MAX_TRAINER_ITEMS; i++)
        {
            if (gTrainers[gTrainerBattleOpponent_A].items[i] != 0)
            {
                BATTLE_HISTORY->trainerItems[BATTLE_HISTORY->itemsNo] = gTrainers[gTrainerBattleOpponent_A].items[i];
                BATTLE_HISTORY->itemsNo++;
            }
        }
    }

    BattleAI_SetupAIData();
}

void BattleAI_SetupAIData(void)
{
    s32 i;
    u8 moveLimitations;
    u8 *data;

    // clear AI data and set default move score to 100. strange that they didn't use memset here.
    MEMSET_ALT(AI_THINKING_STRUCT, 0, sizeof(struct AI_ThinkingStruct), i, data);

    for (i = 0; i < MAX_MON_MOVES; i++)
        AI_THINKING_STRUCT->score[i] = 100;

    moveLimitations = CheckMoveLimitations(gActiveBattler, 0, 0xFF);

    // do not consider moves the AI cannot select
    // also, roll simulated RNG for moves that have a degree of
    // randomness.
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBitTable[i] & moveLimitations)
            AI_THINKING_STRUCT->score[i] = 0;

        AI_THINKING_STRUCT->simulatedRNG[i] = 100 - (Random() % 16);
    }

    // clear AI stack.
    AI_STACK->size = 0;
    gBattlerAttacker = gActiveBattler;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        gBattlerTarget = Random() & 2; // just pick somebody to target.

        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
            gBattlerTarget ^= 2;
    }
    else
        gBattlerTarget = gActiveBattler ^ 1;

    // special AI flag cases.
    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
        AI_THINKING_STRUCT->aiFlags = 0x40000000;
    else if (gBattleTypeFlags & BATTLE_TYPE_ROAMER)
        AI_THINKING_STRUCT->aiFlags = 0x20000000;
    else if (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE)
        AI_THINKING_STRUCT->aiFlags = 0x80000000;
#if GERMAN || (ENGLISH && DEBUG && REVISION == 0)
    else if (gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER) || gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
        AI_THINKING_STRUCT->aiFlags = 7;
#endif
    else // otherwise, just set aiFlags to whatever flags the trainer has set in their data.
        AI_THINKING_STRUCT->aiFlags = gTrainers[gTrainerBattleOpponent_A].aiFlags;
#if DEBUG
    if (gUnknown_02023A14_50 & 1)
        AI_THINKING_STRUCT->aiFlags = gUnknown_02023A14_4C;
#endif
}

u8 BattleAI_GetAIActionToUse(void)
{
    u8 currentMoveArray[MAX_MON_MOVES];
    u8 consideredMoveArray[MAX_MON_MOVES];
    u8 numOfBestMoves;
    s32 i;

    sub_810745C();
    while (AI_THINKING_STRUCT->aiFlags != 0)
    {
        if (AI_THINKING_STRUCT->aiFlags & 1)
        {
            AI_THINKING_STRUCT->aiState = AIState_SettingUp;
            BattleAI_DoAIProcessing();
        }
        AI_THINKING_STRUCT->aiFlags >>= 1;
        AI_THINKING_STRUCT->aiLogicId++;
        AI_THINKING_STRUCT->movesetIndex = 0;
    }

    // special flee or watch cases for safari.
    if (AI_THINKING_STRUCT->aiAction & (AI_ACTION_FLEE)) // flee
        return 4;
    if (AI_THINKING_STRUCT->aiAction & (AI_ACTION_WATCH)) // watch
        return 5;

    numOfBestMoves = 1;
    currentMoveArray[0] = AI_THINKING_STRUCT->score[0];
    consideredMoveArray[0] = 0;

    for (i = 1; i < MAX_MON_MOVES; i++)
    {
        if (currentMoveArray[0] < AI_THINKING_STRUCT->score[i])
        {
            numOfBestMoves = 1;
            currentMoveArray[0] = AI_THINKING_STRUCT->score[i];
            consideredMoveArray[0] = i;
        }
        if (currentMoveArray[0] == AI_THINKING_STRUCT->score[i])
        {
            currentMoveArray[numOfBestMoves] = AI_THINKING_STRUCT->score[i];
            consideredMoveArray[numOfBestMoves++] = i;
        }
    }

    return consideredMoveArray[Random() % numOfBestMoves]; // break any ties that exist.
}

static void BattleAI_DoAIProcessing(void)
{
    while (AI_THINKING_STRUCT->aiState != AIState_FinishedProcessing)
    {
        switch (AI_THINKING_STRUCT->aiState)
        {
        case AIState_DoNotProcess: //Needed to match.
            break;
        case AIState_SettingUp:
            gAIScriptPtr = gBattleAI_ScriptsTable[AI_THINKING_STRUCT->aiLogicId]; // set the AI ptr.
            if (gBattleMons[gBattlerAttacker].pp[AI_THINKING_STRUCT->movesetIndex] == 0)
            {
                AI_THINKING_STRUCT->moveConsidered = MOVE_NONE; // don't consider a move you have 0 PP for, idiot.
            }
            else
            {
                AI_THINKING_STRUCT->moveConsidered = gBattleMons[gBattlerAttacker].moves[AI_THINKING_STRUCT->movesetIndex];
            }
            AI_THINKING_STRUCT->aiState++;
            break;
        case AIState_Processing:
            if (AI_THINKING_STRUCT->moveConsidered != MOVE_NONE)
                sBattleAICmdTable[*gAIScriptPtr](); // run AI command.
            else
            {
                AI_THINKING_STRUCT->score[AI_THINKING_STRUCT->movesetIndex] = 0; // definitely do not consider any move that has 0 PP.
                AI_THINKING_STRUCT->aiAction |= AI_ACTION_DONE;
            }
            if (AI_THINKING_STRUCT->aiAction & AI_ACTION_DONE)
            {
                AI_THINKING_STRUCT->movesetIndex++;
                if (AI_THINKING_STRUCT->movesetIndex < MAX_MON_MOVES && (AI_THINKING_STRUCT->aiAction & AI_ACTION_DO_NOT_ATTACK) == 0)
                    AI_THINKING_STRUCT->aiState = AIState_SettingUp; // as long as their are more moves to process, keep setting this to setup state.
                else
                    AI_THINKING_STRUCT->aiState++; // done processing.
                AI_THINKING_STRUCT->aiAction &= (AI_ACTION_FLEE | AI_ACTION_WATCH | AI_ACTION_DO_NOT_ATTACK |
                AI_ACTION_UNK5 | AI_ACTION_UNK6 | AI_ACTION_UNK7 | AI_ACTION_UNK8); // disable AI_ACTION_DONE.
            }
            break;
        }
    }
}

void sub_810745C(void)
{
    s32 i;

    for (i = 0; i < 8; i++)
    {
        if (BATTLE_HISTORY->usedMoves[gBattlerTarget >> 1][i] == 0)
        {
            BATTLE_HISTORY->usedMoves[gBattlerTarget >> 1][i] = gLastMoves[gBattlerTarget];
            return;
        }
    }
}

void unref_sub_81074A0(u8 battler)
{
    s32 i;

    for (i = 0; i < 8; i++)
        BATTLE_HISTORY->usedMoves[battler / 2][i] = 0;
}

void RecordAbilityBattle(u8 battler, u8 abilityId)
{
    if (GetBattlerSide(battler) == 0)
        BATTLE_HISTORY->abilities[GetBattlerPosition(battler) & 1] = abilityId;
}

void RecordItemEffectBattle(u8 battler, u8 itemEffect)
{
    if (GetBattlerSide(battler) == 0)
        BATTLE_HISTORY->itemEffects[GetBattlerPosition(battler) & 1] = itemEffect;
}

static void Cmd_if_random_less_than(void)
{
    if (Random() % 256 < gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_random_greater_than(void)
{
    if (Random() % 256 > gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_random_equal(void)
{
    if (Random() % 256 == gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_random_not_equal(void)
{
    if (Random() % 256 != gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_score(void)
{
    AI_THINKING_STRUCT->score[AI_THINKING_STRUCT->movesetIndex] += gAIScriptPtr[1]; // add the result to the array of the move consider's score.

    if (AI_THINKING_STRUCT->score[AI_THINKING_STRUCT->movesetIndex] < 0) // if the score is negative, flatten it to 0.
        AI_THINKING_STRUCT->score[AI_THINKING_STRUCT->movesetIndex] = 0;

    gAIScriptPtr += 2; // AI return.
}

static void Cmd_if_hp_less_than(void)
{
    u16 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if ((u32)(100 * gBattleMons[battler].hp / gBattleMons[battler].maxHP) < gAIScriptPtr[2])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
    else
        gAIScriptPtr += 7;
}

static void Cmd_if_hp_more_than(void)
{
    u16 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if ((u32)(100 * gBattleMons[battler].hp / gBattleMons[battler].maxHP) > gAIScriptPtr[2])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
    else
        gAIScriptPtr += 7;
}

static void Cmd_if_hp_equal(void)
{
    u16 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if ((u32)(100 * gBattleMons[battler].hp / gBattleMons[battler].maxHP) == gAIScriptPtr[2])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
    else
        gAIScriptPtr += 7;
}

static void Cmd_if_hp_not_equal(void)
{
    u16 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if ((u32)(100 * gBattleMons[battler].hp / gBattleMons[battler].maxHP) != gAIScriptPtr[2])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
    else
        gAIScriptPtr += 7;
}

static void Cmd_if_status(void)
{
    u16 battler;
    u32 status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gBattleMons[battler].status1 & status) != 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_not_status(void)
{
    u16 battler;
    u32 status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gBattleMons[battler].status1 & status) == 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_status2(void)
{
    u16 battler;
    u32 status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gBattleMons[battler].status2 & status) != 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_not_status2(void)
{
    u16 battler;
    u32 status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gBattleMons[battler].status2 & status) == 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_status3(void)
{
    u16 battler;
    u32 status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gStatuses3[battler] & status) != 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_not_status3(void)
{
    u16 battler;
    u32 status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gStatuses3[battler] & status) == 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_side_affecting(void)
{
    u16 battler;
    u32 side, status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    side = GetBattlerPosition(battler) & 1;
    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gSideStatuses[side] & status) != 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_not_side_affecting(void)
{
    u16 battler;
    u32 side, status;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    side = GetBattlerPosition(battler) & 1;
    status = T1_READ_32(gAIScriptPtr + 2);

    if ((gSideStatuses[side] & status) == 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
    else
        gAIScriptPtr += 10;
}

static void Cmd_if_less_than(void)
{
    if (AI_THINKING_STRUCT->funcResult < gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_more_than(void)
{
    if (AI_THINKING_STRUCT->funcResult > gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_equal(void)
{
    if (AI_THINKING_STRUCT->funcResult == gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_not_equal(void)
{
    if (AI_THINKING_STRUCT->funcResult != gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_less_than_ptr(void)
{
    u8 *value = T1_READ_PTR(gAIScriptPtr + 1);

    if (AI_THINKING_STRUCT->funcResult < *value)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
    else
        gAIScriptPtr += 9;
}

static void Cmd_if_more_than_ptr(void)
{
    u8 *value = T1_READ_PTR(gAIScriptPtr + 1);

    if (AI_THINKING_STRUCT->funcResult > *value)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
    else
        gAIScriptPtr += 9;
}

static void Cmd_if_equal_ptr(void)
{
    u8 *value = T1_READ_PTR(gAIScriptPtr + 1);

    if (AI_THINKING_STRUCT->funcResult == *value)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
    else
        gAIScriptPtr += 9;
}

static void Cmd_if_not_equal_ptr(void)
{
    u8 *value = T1_READ_PTR(gAIScriptPtr + 1);

    if (AI_THINKING_STRUCT->funcResult != *value)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
    else
        gAIScriptPtr += 9;
}

static void Cmd_if_move(void)
{
    u16 move = T1_READ_16(gAIScriptPtr + 1);

    if (AI_THINKING_STRUCT->moveConsidered == move)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
    else
        gAIScriptPtr += 7;
}

static void Cmd_if_not_move(void)
{
    u16 move = T1_READ_16(gAIScriptPtr + 1);

    if (AI_THINKING_STRUCT->moveConsidered != move)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
    else
        gAIScriptPtr += 7;
}

static void Cmd_if_in_bytes(void)
{
    u8 *ptr = T1_READ_PTR(gAIScriptPtr + 1);

    while (*ptr != 0xFF)
    {
        if (AI_THINKING_STRUCT->funcResult == *ptr)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
            return;
        }
        ptr++;
    }
    gAIScriptPtr += 9;
}

static void Cmd_if_not_in_bytes(void)
{
    u8 *ptr = T1_READ_PTR(gAIScriptPtr + 1);

    while (*ptr != 0xFF)
    {
        if (AI_THINKING_STRUCT->funcResult == *ptr)
        {
            gAIScriptPtr += 9;
            return;
        }
        ptr++;
    }
    gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
}

static void Cmd_if_in_hwords(void)
{
    u16 *ptr = (u16 *)T1_READ_PTR(gAIScriptPtr + 1);

    while (*ptr != 0xFFFF)
    {
        if (AI_THINKING_STRUCT->funcResult == *ptr)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
            return;
        }
        ptr++;
    }
    gAIScriptPtr += 9;
}

static void Cmd_if_not_in_hwords(void)
{
    u16 *ptr = (u16 *)T1_READ_PTR(gAIScriptPtr + 1);

    while (*ptr != 0xFFFF)
    {
        if (AI_THINKING_STRUCT->funcResult == *ptr)
        {
            gAIScriptPtr += 9;
            return;
        }
        ptr++;
    }
    gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 5);
}

static void Cmd_if_user_has_attacking_move(void)
{
    s32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBattleMons[gBattlerAttacker].moves[i] != 0
            && gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].power != 0)
            break;
    }
    if (i == MAX_MON_MOVES)
        gAIScriptPtr += 5;
    else
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
}

static void Cmd_if_user_has_no_attacking_moves(void)
{
    s32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBattleMons[gBattlerAttacker].moves[i] != 0
         && gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].power != 0)
            break;
    }
    if (i != MAX_MON_MOVES)
        gAIScriptPtr += 5;
    else
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
}

static void Cmd_get_turn_count(void)
{
    AI_THINKING_STRUCT->funcResult = gBattleResults.battleTurnCounter;
    gAIScriptPtr += 1;
}

static void Cmd_get_type(void)
{
    switch (gAIScriptPtr[1])
    {
    case 1: // player primary type
        AI_THINKING_STRUCT->funcResult = gBattleMons[gBattlerAttacker].type1;
        break;
    case 0: // enemy primary type
        AI_THINKING_STRUCT->funcResult = gBattleMons[gBattlerTarget].type1;
        break;
    case 3: // player secondary type
        AI_THINKING_STRUCT->funcResult = gBattleMons[gBattlerAttacker].type2;
        break;
    case 2: // enemy secondary type
        AI_THINKING_STRUCT->funcResult = gBattleMons[gBattlerTarget].type2;
        break;
    case 4: // type of move being pointed to
        AI_THINKING_STRUCT->funcResult = gBattleMoves[AI_THINKING_STRUCT->moveConsidered].type;
        break;
    }
    gAIScriptPtr += 2;
}

static void Cmd_get_considered_move_power(void)
{
    AI_THINKING_STRUCT->funcResult = gBattleMoves[AI_THINKING_STRUCT->moveConsidered].power;
    gAIScriptPtr += 1;
}

static void Cmd_get_how_powerful_move_is(void)
{
    s32 i, checkedMove;
    s32 moveDmgs[MAX_MON_MOVES];

    for (i = 0; sIgnoredPowerfulMoveEffects[i] != 0xFFFF; i++)
    {
        if (gBattleMoves[AI_THINKING_STRUCT->moveConsidered].effect == sIgnoredPowerfulMoveEffects[i])
            break;
    }

    if (gBattleMoves[AI_THINKING_STRUCT->moveConsidered].power > 1
        && sIgnoredPowerfulMoveEffects[i] == 0xFFFF)
    {
        gDynamicBasePower = 0;
        gBattleStruct->dynamicMoveType = 0;
        gBattleStruct->dmgMultiplier = 1;
        gMoveResultFlags = 0;
        gCritMultiplier = 1;

        for (checkedMove = 0; checkedMove < MAX_MON_MOVES; checkedMove++)
        {
            for (i = 0; sIgnoredPowerfulMoveEffects[i] != 0xFFFF; i++)
            {
                if (gBattleMoves[gBattleMons[gBattlerAttacker].moves[checkedMove]].effect == sIgnoredPowerfulMoveEffects[i])
                    break;
            }

            if (gBattleMons[gBattlerAttacker].moves[checkedMove] != MOVE_NONE
                && sIgnoredPowerfulMoveEffects[i] == 0xFFFF
                && gBattleMoves[gBattleMons[gBattlerAttacker].moves[checkedMove]].power > 1)
            {
                gCurrentMove = gBattleMons[gBattlerAttacker].moves[checkedMove];
                AI_CalcDmg(gBattlerAttacker, gBattlerTarget);
                TypeCalc(gCurrentMove, gBattlerAttacker, gBattlerTarget);
                moveDmgs[checkedMove] = gBattleMoveDamage * AI_THINKING_STRUCT->simulatedRNG[checkedMove] / 100;
                if (moveDmgs[checkedMove] == 0)
                    moveDmgs[checkedMove] = 1;
            }
            else
            {
                moveDmgs[checkedMove] = 0;
            }
        }

        for (checkedMove = 0; checkedMove < MAX_MON_MOVES; checkedMove++)
        {
            if (moveDmgs[checkedMove] > moveDmgs[AI_THINKING_STRUCT->movesetIndex])
                break;
        }

        if (checkedMove == MAX_MON_MOVES)
            AI_THINKING_STRUCT->funcResult = 2; // Is the most powerful.
        else
            AI_THINKING_STRUCT->funcResult = 1; // Not the most powerful.
    }
    else
    {
        AI_THINKING_STRUCT->funcResult = 0; // Highly discouraged in terms of power.
    }

    gAIScriptPtr++;
}

static void Cmd_get_last_used_battler_move(void)
{
    if (gAIScriptPtr[1] == AI_USER)
        AI_THINKING_STRUCT->funcResult = gLastMoves[gBattlerAttacker];
    else
        AI_THINKING_STRUCT->funcResult = gLastMoves[gBattlerTarget];

    gAIScriptPtr += 2;
}

static void Cmd_if_equal_(void)
{
    if (gAIScriptPtr[1] == AI_THINKING_STRUCT->funcResult)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_not_equal_(void)
{
    if (gAIScriptPtr[1] != AI_THINKING_STRUCT->funcResult)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_user_goes(void)
{
    if (GetWhoStrikesFirst(gBattlerAttacker, gBattlerTarget, TRUE) == gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_user_doesnt_go(void)
{
    if (GetWhoStrikesFirst(gBattlerAttacker, gBattlerTarget, TRUE) != gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_nop_2A(void)
{
}

static void Cmd_nop_2B(void)
{
}

static void Cmd_count_usable_party_mons(void)
{
    struct Pokemon *party;
    int i;
    u8 battler;
    u8 battlerOnField1, battlerOnField2;

    AI_THINKING_STRUCT->funcResult = 0;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (GetBattlerSide(battler) == 0)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        u32 position;
        battlerOnField1 = gBattlerPartyIndexes[battler];
        position = GetBattlerPosition(battler) ^ 2;
        battlerOnField2 = gBattlerPartyIndexes[GetBattlerAtPosition(position)];
    }
    else
    {
        battlerOnField1 = gBattlerPartyIndexes[battler];
        battlerOnField2 = gBattlerPartyIndexes[battler];
    }

    for (i = 0; i < 6; i++)
    {
        if (i != battlerOnField1 && i != battlerOnField2
         && GetMonData(&party[i], MON_DATA_HP) != 0
         && GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG) != SPECIES_NONE
         && GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG)
        {
            AI_THINKING_STRUCT->funcResult++;
        }
    }

    gAIScriptPtr += 2;
}

static void Cmd_get_considered_move(void)
{
    AI_THINKING_STRUCT->funcResult = AI_THINKING_STRUCT->moveConsidered;
    gAIScriptPtr += 1;
}

static void Cmd_get_considered_move_effect(void)
{
    AI_THINKING_STRUCT->funcResult = gBattleMoves[AI_THINKING_STRUCT->moveConsidered].effect;
    gAIScriptPtr += 1;
}

static void BattleAICmd_get_ability(void)
{
    u8 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
    {
        u16 side = GetBattlerPosition(battler) & 1;

        if (BATTLE_HISTORY->abilities[side] != 0)
        {
            AI_THINKING_STRUCT->funcResult = BATTLE_HISTORY->abilities[side];
            gAIScriptPtr += 2;
            return;
        }

        // abilities that prevent fleeing.
        if (gBattleMons[battler].ability == ABILITY_SHADOW_TAG
        || gBattleMons[battler].ability == ABILITY_MAGNET_PULL
        || gBattleMons[battler].ability == ABILITY_ARENA_TRAP)
        {
            AI_THINKING_STRUCT->funcResult = gBattleMons[battler].ability;
            gAIScriptPtr += 2;
            return;
        }

        if (gSpeciesInfo[gBattleMons[battler].species].ability1 != ABILITY_NONE)
        {
            if (gSpeciesInfo[gBattleMons[battler].species].ability2 != ABILITY_NONE)
            {
                // AI has no knowledge of opponent, so it guesses which ability.
                if (Random() % 2)
                {
                    AI_THINKING_STRUCT->funcResult = gSpeciesInfo[gBattleMons[battler].species].ability1;
                }
                else
                {
                    AI_THINKING_STRUCT->funcResult = gSpeciesInfo[gBattleMons[battler].species].ability2;
                }
            }
            else
            {
                AI_THINKING_STRUCT->funcResult = gSpeciesInfo[gBattleMons[battler].species].ability1; // it's definitely ability 1.
            }
        }
        else
        {
            AI_THINKING_STRUCT->funcResult = gSpeciesInfo[gBattleMons[battler].species].ability2; // AI cant actually reach this part since every mon has at least 1 ability.
        }
    }
    else
    {
        // The AI knows its own ability.
        AI_THINKING_STRUCT->funcResult = gBattleMons[battler].ability;
    }
    gAIScriptPtr += 2;
}

static void Cmd_get_highest_type_effectiveness(void)
{
    s32 i;

    gDynamicBasePower = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->dmgMultiplier = 1;
    gMoveResultFlags = 0;
    gCritMultiplier = 1;
    AI_THINKING_STRUCT->funcResult = 0;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        gBattleMoveDamage = 40;
        gCurrentMove = gBattleMons[gBattlerAttacker].moves[i];

        if (gCurrentMove)
        {
            TypeCalc(gCurrentMove, gBattlerAttacker, gBattlerTarget);

            // reduce by 1/3.
            if (gBattleMoveDamage == 120)
                gBattleMoveDamage = 80;
            if (gBattleMoveDamage == 240)
                gBattleMoveDamage = 160;
            if (gBattleMoveDamage == 30)
                gBattleMoveDamage = 20;
            if (gBattleMoveDamage == 15)
                gBattleMoveDamage = 10;

            if (gMoveResultFlags & 8) // if it's a status move, it wont do anything.
                gBattleMoveDamage = 0;

            if (AI_THINKING_STRUCT->funcResult < gBattleMoveDamage)
                AI_THINKING_STRUCT->funcResult = gBattleMoveDamage;
        }
    }
    gAIScriptPtr += 1;
}

static void Cmd_if_type_effectiveness(void)
{
    u8 damageVar;

    gDynamicBasePower = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->dmgMultiplier = 1;
    gMoveResultFlags = 0;
    gCritMultiplier = 1;

    gBattleMoveDamage = 40;
    gCurrentMove = AI_THINKING_STRUCT->moveConsidered;

    TypeCalc(gCurrentMove, gBattlerAttacker, gBattlerTarget);

    if (gBattleMoveDamage == 120)
        gBattleMoveDamage = 80;
    if (gBattleMoveDamage == 240)
        gBattleMoveDamage = 160;
    if (gBattleMoveDamage == 30)
        gBattleMoveDamage = 20;
    if (gBattleMoveDamage == 15)
        gBattleMoveDamage = 10;

    if (gMoveResultFlags & 8)
        gBattleMoveDamage = 0;

    // store gBattleMoveDamage in a u8 variable because gAIScriptPtr[1] is a u8.
    damageVar = gBattleMoveDamage;

    if (damageVar == gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_nop_32(void)
{
}

static void Cmd_nop_33(void)
{
}

static void BattleAICmd_if_status_in_party(void)
{
    struct Pokemon *party;
    struct Pokemon *partyPtr;
    int i;
    u32 statusToCompareTo;

    // for whatever reason, game freak put the party pointer into 2 variables instead of 1. it's possible at some point the switch encompassed the whole function and used each respective variable creating largely duplicate code.
    switch (gAIScriptPtr[1])
    {
    case 1:
        party = partyPtr = gEnemyParty;
        break;
    default:
        party = partyPtr = gPlayerParty;
        break;
    }

    statusToCompareTo = T1_READ_32(gAIScriptPtr + 2);

    for (i = 0; i < 6; i++)
    {
        u16 species = GetMonData(&party[i], MON_DATA_SPECIES);
        u16 hp = GetMonData(&party[i], MON_DATA_HP);
        u32 status = GetMonData(&party[i], MON_DATA_STATUS);

        if (species != SPECIES_NONE && species != SPECIES_EGG && hp != 0 && status == statusToCompareTo)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6); // WHAT. why is this being merged into the above switch
            return;
        }
    }

    gAIScriptPtr += 10;
}

// bugged, doesnt return properly. also unused
static void BattleAICmd_if_status_not_in_party(void)
{
    struct Pokemon *party;
    struct Pokemon *partyPtr;
    int i;
    u32 statusToCompareTo;

    switch (gAIScriptPtr[1])
    {
    case 1:
        party = partyPtr = gEnemyParty;
        break;
    default:
        party = partyPtr = gPlayerParty;
        break;
    }

    statusToCompareTo = T1_READ_32(gAIScriptPtr + 2);

    for (i = 0; i < 6; i++)
    {
        u16 species = GetMonData(&party[i], MON_DATA_SPECIES);
        u16 hp = GetMonData(&party[i], MON_DATA_HP);
        u32 status = GetMonData(&party[i], MON_DATA_STATUS);

        // everytime the status is found, the AI's logic jumps further and further past its intended destination. this results in a broken AI macro and is probably why it is unused.
        if (species != SPECIES_NONE && species != SPECIES_EGG && hp != 0 && status == statusToCompareTo)
            gAIScriptPtr += 10; // doesnt return?
    }
    gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 6);
}

static void Cmd_get_weather(void)
{
    if (gBattleWeather & B_WEATHER_RAIN)
        AI_THINKING_STRUCT->funcResult = AI_WEATHER_RAIN;
    if (gBattleWeather & B_WEATHER_SANDSTORM)
        AI_THINKING_STRUCT->funcResult = AI_WEATHER_SANDSTORM;
    if (gBattleWeather & B_WEATHER_SUN)
        AI_THINKING_STRUCT->funcResult = AI_WEATHER_SUN;
    if (gBattleWeather & B_WEATHER_HAIL)
        AI_THINKING_STRUCT->funcResult = AI_WEATHER_HAIL;

    gAIScriptPtr += 1;
}

static void Cmd_if_effect(void)
{
    if (gBattleMoves[AI_THINKING_STRUCT->moveConsidered].effect == gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_not_effect(void)
{
    if (gBattleMoves[AI_THINKING_STRUCT->moveConsidered].effect != gAIScriptPtr[1])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
    else
        gAIScriptPtr += 6;
}

static void Cmd_if_stat_level_less_than(void)
{
    u32 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (gBattleMons[battler].statStages[gAIScriptPtr[2]] < gAIScriptPtr[3])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
    else
        gAIScriptPtr += 8;
}

static void Cmd_if_stat_level_more_than(void)
{
    u32 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (gBattleMons[battler].statStages[gAIScriptPtr[2]] > gAIScriptPtr[3])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
    else
        gAIScriptPtr += 8;
}

static void Cmd_if_stat_level_equal(void)
{
    u32 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (gBattleMons[battler].statStages[gAIScriptPtr[2]] == gAIScriptPtr[3])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
    else
        gAIScriptPtr += 8;
}

static void Cmd_if_stat_level_not_equal(void)
{
    u32 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (gBattleMons[battler].statStages[gAIScriptPtr[2]] != gAIScriptPtr[3])
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
    else
        gAIScriptPtr += 8;
}

static void Cmd_if_can_faint(void)
{
    if (gBattleMoves[AI_THINKING_STRUCT->moveConsidered].power < 2)
    {
        gAIScriptPtr += 5;
        return;
    }

    gDynamicBasePower = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->dmgMultiplier = 1;
    gMoveResultFlags = 0;
    gCritMultiplier = 1;
    gCurrentMove = AI_THINKING_STRUCT->moveConsidered;
    AI_CalcDmg(gBattlerAttacker, gBattlerTarget);
    TypeCalc(gCurrentMove, gBattlerAttacker, gBattlerTarget);

    gBattleMoveDamage = gBattleMoveDamage * AI_THINKING_STRUCT->simulatedRNG[AI_THINKING_STRUCT->movesetIndex] / 100;

    // moves always do at least 1 damage.
    if (gBattleMoveDamage == 0)
        gBattleMoveDamage = 1;

    if (gBattleMons[gBattlerTarget].hp <= gBattleMoveDamage)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
    else
        gAIScriptPtr += 5;
}

static void Cmd_if_cant_faint(void)
{
    if (gBattleMoves[AI_THINKING_STRUCT->moveConsidered].power < 2)
    {
        gAIScriptPtr += 5;
        return;
    }

    gDynamicBasePower = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->dmgMultiplier = 1;
    gMoveResultFlags = 0;
    gCritMultiplier = 1;
    gCurrentMove = AI_THINKING_STRUCT->moveConsidered;
    AI_CalcDmg(gBattlerAttacker, gBattlerTarget);
    TypeCalc(gCurrentMove, gBattlerAttacker, gBattlerTarget);

    gBattleMoveDamage = gBattleMoveDamage * AI_THINKING_STRUCT->simulatedRNG[AI_THINKING_STRUCT->movesetIndex] / 100;

    // this macro is missing the damage 0 = 1 assumption.

    if (gBattleMons[gBattlerTarget].hp > gBattleMoveDamage)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
    else
        gAIScriptPtr += 5;
}

static void BattleAICmd_if_has_move(void)
{
    int i;
    u16 *movePtr = (u16 *)(gAIScriptPtr + 2);

    switch (gAIScriptPtr[1])
    {
    case 1:
    case 3:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] == *movePtr)
                break;
        }
        if (i == MAX_MON_MOVES)
            gAIScriptPtr += 8;
        else
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
        break;
    case 0:
    case 2:
        for (i = 0; i < 8; i++)
        {
            if (BATTLE_HISTORY->usedMoves[gBattlerTarget >> 1][i] == *movePtr)
                break;
        }
        if (i == 8)
            gAIScriptPtr += 8;
         else
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
        break;
    }
}

static void BattleAICmd_if_dont_have_move(void)
{
    int i;
    u16 *movePtr = (u16 *)(gAIScriptPtr + 2);

    switch (gAIScriptPtr[1])
    {
    case 1:
    case 3:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] == *movePtr)
                break;
        }
        if (i != MAX_MON_MOVES)
            gAIScriptPtr += 8;
        else
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
        break;
    case 0:
    case 2:
        for (i = 0; i < 8; i++)
        {
            if (BATTLE_HISTORY->usedMoves[gBattlerTarget >> 1][i] == *movePtr)
                break;
        }
        if (i != 8)
            gAIScriptPtr += 8;
        else
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 4);
        break;
    }
}

static void BattleAICmd_if_move_effect(void)
{
    int i;

    switch (gAIScriptPtr[1])
    {
    case 1:
    case 3:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] != 0 && gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].effect == gAIScriptPtr[2])
                break;
        }
        if (i != MAX_MON_MOVES)
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
        else
            gAIScriptPtr += 7;
        break;
    case 0:
    case 2:
        for (i = 0; i < 8; i++)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] != 0 && gBattleMoves[BATTLE_HISTORY->usedMoves[gBattlerTarget >> 1][i]].effect == gAIScriptPtr[2])
                break;
        }
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
    }
}

static void BattleAICmd_if_not_move_effect(void)
{
    int i;

    switch (gAIScriptPtr[1])
    {
    case 1:
    case 3:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] != 0 && gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].effect == gAIScriptPtr[2])
                break;
        }
        if (i != MAX_MON_MOVES)
            gAIScriptPtr += 7;
        else
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
        break;
    case 0:
    case 2:
        for (i = 0; i < 8; i++)
        {
            if (BATTLE_HISTORY->usedMoves[gBattlerTarget >> 1][i] != 0 && gBattleMoves[BATTLE_HISTORY->usedMoves[gBattlerTarget >> 1][i]].effect == gAIScriptPtr[2])
                break;
        }
        gAIScriptPtr += 7;
    }
}

static void Cmd_if_any_move_disabled_or_encored(void)
{
    u8 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (gAIScriptPtr[2] == 0)
    {
        if (gDisableStructs[battler].disabledMove == 0)
        {
            gAIScriptPtr += 7;
            return;
        }
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
        return;
    }
    else if (gAIScriptPtr[2] != 1) // ignore the macro if its not 0 or 1.
    {
        gAIScriptPtr += 7;
        return;
    }
    else if (gDisableStructs[battler].encoredMove != 0)
    {
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 3);
        return;
    }
    gAIScriptPtr += 7;
}

static void Cmd_if_curr_move_disabled_or_encored(void)
{
    switch (gAIScriptPtr[1])
    {
    case 0: // _08109348
        if (gDisableStructs[gActiveBattler].disabledMove == AI_THINKING_STRUCT->moveConsidered)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
            return;
        }
        gAIScriptPtr += 6;
        return;
    case 1: // _08109370
        if (gDisableStructs[gActiveBattler].encoredMove == AI_THINKING_STRUCT->moveConsidered)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
            return;
        }
        gAIScriptPtr += 6;
        return;
    default:
        gAIScriptPtr += 6;
        return;
    }
}

static void Cmd_flee(void)
{
    AI_THINKING_STRUCT->aiAction |= (AI_ACTION_DONE | AI_ACTION_FLEE | AI_ACTION_DO_NOT_ATTACK); // what matters is AI_ACTION_FLEE being enabled.
}

static void Cmd_if_random_safari_flee(void)
{
    u8 safariFleeRate = gBattleStruct->safariEscapeFactor * 5; // safari flee rate, from 0-20

    if ((u8)(Random() % 100) < safariFleeRate)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
    else
        gAIScriptPtr += 5;
}

static void Cmd_watch(void)
{
    AI_THINKING_STRUCT->aiAction |= (AI_ACTION_DONE | AI_ACTION_WATCH | AI_ACTION_DO_NOT_ATTACK); // what matters is AI_ACTION_WATCH being enabled.
}

static void BattleAICmd_get_hold_effect(void)
{
    u8 battler;
    u16 side;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    if (GetBattlerSide(battler) == 0)
    {
        side = (GetBattlerPosition(battler) & 1);
        AI_THINKING_STRUCT->funcResult = BATTLE_HISTORY->itemEffects[side];
    }
    else
        AI_THINKING_STRUCT->funcResult = GetItemHoldEffect(gBattleMons[battler].item);

    gAIScriptPtr += 2;
}

static void Cmd_get_gender(void)
{
    u8 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    AI_THINKING_STRUCT->funcResult = GetGenderFromSpeciesAndPersonality(gBattleMons[battler].species, gBattleMons[battler].personality);

    gAIScriptPtr += 2;
}

static void Cmd_is_first_turn_for(void)
{
    u8 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    AI_THINKING_STRUCT->funcResult = gDisableStructs[battler].isFirstTurn;

    gAIScriptPtr += 2;
}

static void Cmd_get_stockpile_count(void)
{
    u8 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    AI_THINKING_STRUCT->funcResult = gDisableStructs[battler].stockpileCounter;

    gAIScriptPtr += 2;
}

static void Cmd_is_double_battle(void)
{
    AI_THINKING_STRUCT->funcResult = gBattleTypeFlags & BATTLE_TYPE_DOUBLE;

    gAIScriptPtr += 1;
}

static void Cmd_get_used_held_item(void)
{
    u8 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    AI_THINKING_STRUCT->funcResult = gSharedMem[offsetof(struct BattleStruct, usedHeldItems) + 2 * battler];

    gAIScriptPtr += 2;
}

static void Cmd_get_move_type_from_result(void)
{
    AI_THINKING_STRUCT->funcResult = gBattleMoves[AI_THINKING_STRUCT->funcResult].type;

    gAIScriptPtr += 1;
}

static void Cmd_get_move_power_from_result(void)
{
    AI_THINKING_STRUCT->funcResult = gBattleMoves[AI_THINKING_STRUCT->funcResult].power;

    gAIScriptPtr += 1;
}

static void Cmd_get_move_effect_from_result(void)
{
    AI_THINKING_STRUCT->funcResult = gBattleMoves[AI_THINKING_STRUCT->funcResult].effect;

    gAIScriptPtr += 1;
}

static void Cmd_get_protect_count(void)
{
    u8 battler;

    if (gAIScriptPtr[1] == AI_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;

    AI_THINKING_STRUCT->funcResult = gDisableStructs[battler].protectUses;

    gAIScriptPtr += 2;
}

static void Cmd_nop_52(void)
{
}

static void Cmd_nop_53(void)
{
}

static void Cmd_nop_54(void)
{
}

static void Cmd_nop_55(void)
{
}

static void Cmd_nop_56(void)
{
}

static void Cmd_nop_57(void)
{
}

static void Cmd_call(void)
{
    AIStackPushVar(gAIScriptPtr + 5);
    gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
}

static void Cmd_goto(void)
{
    gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
}

static void Cmd_end(void)
{
    if (AIStackPop() == FALSE)
        AI_THINKING_STRUCT->aiAction |= AI_ACTION_DONE;
}

static void Cmd_if_level_cond(void)
{
    switch (gAIScriptPtr[1])
    {
    case 0: // greater than
        if (gBattleMons[gBattlerAttacker].level > gBattleMons[gBattlerTarget].level)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
            return;
        }
        gAIScriptPtr += 6;
        return;
    case 1: // less than
        if (gBattleMons[gBattlerAttacker].level < gBattleMons[gBattlerTarget].level)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
            return;
        }
        gAIScriptPtr += 6;
        return;
    case 2: // equal
        if (gBattleMons[gBattlerAttacker].level == gBattleMons[gBattlerTarget].level)
        {
            gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 2);
            return;
        }
        gAIScriptPtr += 6;
        return;
    }
}

static void Cmd_if_target_taunted(void)
{
    if (gDisableStructs[gBattlerTarget].tauntTimer != 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
    else
        gAIScriptPtr += 5;
}

static void Cmd_if_target_not_taunted(void)
{
    if (gDisableStructs[gBattlerTarget].tauntTimer == 0)
        gAIScriptPtr = T1_READ_PTR(gAIScriptPtr + 1);
    else
        gAIScriptPtr += 5;
}

static void AIStackPushVar(u8 *var)
{
    AI_STACK->ptr[AI_STACK->size++] = var;
}

// unused
void AIStackPushVar_cursor(void)
{
    AI_STACK->ptr[AI_STACK->size++] = gAIScriptPtr;
}

static bool8 AIStackPop(void)
{
    if (AI_STACK->size != 0)
    {
        --AI_STACK->size;
        gAIScriptPtr = AI_STACK->ptr[AI_STACK->size];
        return TRUE;
    }
    else
        return FALSE;
}
