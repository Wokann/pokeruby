#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_ai_script_commands.h"
#include "battle_anim.h"
#include "battle_util.h"
#include "cable_club.h"
#include "constants/items.h"
#include "link.h"
#include "pokemon.h"
#include "party_menu.h"
#include "constants/species.h"
#include "task.h"
#include "util.h"
#include "battle_message.h"
#include "data2.h"
#include "ewram.h"
#include "constants/abilities.h"

extern u16 gBattleTypeFlags;
extern u16 gBattleWeather;
extern struct BattlePokemon gBattleMons[];

static EWRAM_DATA u8 sLinkSendTaskId = 0;
static EWRAM_DATA u8 sLinkReceiveTaskId = 0;
static EWRAM_DATA u8 sUnused = 0;

extern u8 gBattleBufferA[][0x200];
extern u8 gBattleBufferB[][0x200];
extern u8 gActiveBattler;
extern u32 gBattleControllerExecFlags;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerPositions[];
extern u16 gCurrentMove;
extern u16 gChosenMove;
extern u16 gLastUsedItem;
extern u8 gLastUsedAbility;
extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u8 gEffectBattler;
extern u8 gPotentialItemEffectBattler;
extern u8 gAbsentBattlerFlags;
extern u8 gMultiHitCounter;
extern u8 gBattleOutcome;
extern u8 gActionSelectionCursor[];
extern u8 gMoveSelectionCursor[];
extern u8 gBattleTextBuff1[];
extern u8 gBattleTextBuff2[];
extern u8 gBattleTextBuff3[];
extern void (*gBattleMainFunc)(void);
extern void (*gBattlerControllerFuncs[])(void);

u8 gBattleBuffersTransferData[0x170];

// this file's funcionts
static void CreateTasksForSendRecvLinkBuffers(void);
static void InitLinkBtlControllers(void);
static void InitSinglePlayerBtlControllers(void);
static void SetBattlePartyIds(void);
static void Task_HandleSendLinkBuffersData(u8 taskId);
static void Task_HandleCopyReceivedLinkBuffersData(u8 taskId);

void HandleLinkBattleSetup(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        OpenLink();
        CreateTask(Task_WaitForLinkPlayerConnection, 0);
        CreateTasksForSendRecvLinkBuffers();
    }
}

void SetUpBattleVarsAndBirchPoochyena(void)
{
    s32 i;

    gBattleMainFunc = BeginBattleIntroDummy;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gBattlerControllerFuncs[i] = BattleControllerDummy;
        gBattlerPositions[i] = 0xFF;
        gActionSelectionCursor[i] = 0;
        gMoveSelectionCursor[i] = 0;
    }

    HandleLinkBattleSetup();
    gBattleControllerExecFlags = 0;
    ClearBattleAnimationVars();
    ClearBattleMonForms();
    BattleAI_HandleItemUseBeforeAISetup();
    if (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE)
    {
        ZeroEnemyPartyMons();
        CreateMon(&gEnemyParty[0], SPECIES_POOCHYENA, 2, 32, 0, 0, 0, 0);
        i = ITEM_NONE;
        SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &i);
    }
    gUnusedFirstBattleVar1 = 0;
    gUnusedFirstBattleVar2 = 0;

#if DEBUG
    if (gDebugBattleFlags & 0x80)
    {
        gSharedMem[0x160fd] = 0;
        gSharedMem[0x160fe] = 0;
        gSharedMem[0x160ff] = 0;
        ((u32 *) gBattleBuffersTransferData)[64]++;
    }
#endif
}

void InitBattleControllers(void)
{
    s32 i;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        InitLinkBtlControllers();
    else
        InitSinglePlayerBtlControllers();

    SetBattlePartyIds();

    if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        for (i = 0; i < gBattlersCount; i++)
            BufferBattlePartyCurrentOrderBySide(i, 0);
    }
}

static void InitSinglePlayerBtlControllers(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        gBattleMainFunc = BeginBattleIntro;
        if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
            gBattlerControllerFuncs[0] = SetControllerToSafari;
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL)
            gBattlerControllerFuncs[0] = SetControllerToWally;
        else
            gBattlerControllerFuncs[0] = SetControllerToPlayer;
        gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
        gBattlerControllerFuncs[1] = SetControllerToOpponent;
        gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
        gBattlersCount = 2;
    }
    else
    {
        gBattleMainFunc = BeginBattleIntro;
        gBattlerControllerFuncs[0] = SetControllerToPlayer;
        gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
        gBattlerControllerFuncs[1] = SetControllerToOpponent;
        gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
        gBattlerControllerFuncs[2] = SetControllerToPlayer;
        gBattlerPositions[2] = B_POSITION_PLAYER_RIGHT;
        gBattlerControllerFuncs[3] = SetControllerToOpponent;
        gBattlerPositions[3] = B_POSITION_OPPONENT_RIGHT;
        gBattlersCount = MAX_BATTLERS_COUNT;
    }
}

static void InitLinkBtlControllers(void)
{
    u8 multiplayerId;
    int i;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        if (gBattleTypeFlags & BATTLE_TYPE_WILD)
        {
            gBattleMainFunc = BeginBattleIntro;
            gBattlerControllerFuncs[0] = SetControllerToPlayer;
            gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
            gBattlerControllerFuncs[1] = SetControllerToLinkOpponent;
            gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
            gBattlersCount = 2;
        }
        else
        {
            gBattlerControllerFuncs[1] = SetControllerToPlayer;
            gBattlerPositions[1] = B_POSITION_PLAYER_LEFT;
            gBattlerControllerFuncs[0] = SetControllerToLinkOpponent;
            gBattlerPositions[0] = B_POSITION_OPPONENT_LEFT;
            gBattlersCount = 2;
        }
        return;
    }
    if ((gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE)) == BATTLE_TYPE_DOUBLE)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_WILD)
        {
            gBattleMainFunc = BeginBattleIntro;
            gBattlerControllerFuncs[0] = SetControllerToPlayer;
            gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
            gBattlerControllerFuncs[1] = SetControllerToLinkOpponent;
            gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
            gBattlerControllerFuncs[2] = SetControllerToPlayer;
            gBattlerPositions[2] = B_POSITION_PLAYER_RIGHT;
            gBattlerControllerFuncs[3] = SetControllerToLinkOpponent;
            gBattlerPositions[3] = B_POSITION_OPPONENT_RIGHT;
            gBattlersCount = MAX_BATTLERS_COUNT;
        }
        else
        {
            gBattlerControllerFuncs[1] = SetControllerToPlayer;
            gBattlerPositions[1] = B_POSITION_PLAYER_LEFT;
            gBattlerControllerFuncs[0] = SetControllerToLinkOpponent;
            gBattlerPositions[0] = B_POSITION_OPPONENT_LEFT;
            gBattlerControllerFuncs[3] = SetControllerToPlayer;
            gBattlerPositions[3] = B_POSITION_PLAYER_RIGHT;
            gBattlerControllerFuncs[2] = SetControllerToLinkOpponent;
            gBattlerPositions[2] = B_POSITION_OPPONENT_RIGHT;
            gBattlersCount = MAX_BATTLERS_COUNT;

        }
        return;
    }
    multiplayerId = GetMultiplayerId();
    if (gBattleTypeFlags & BATTLE_TYPE_WILD)
        gBattleMainFunc = BeginBattleIntro;
    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        switch (gLinkPlayers[i].id)
        {
        case 0:
        case 3:
            BufferBattlePartyCurrentOrderBySide(gLinkPlayers[i].id, 0);
            break;
        case 1:
        case 2:
            BufferBattlePartyCurrentOrderBySide(gLinkPlayers[i].id, 1);
            break;
        }

        if (i == multiplayerId)
        {
            gBattlerControllerFuncs[gLinkPlayers[i].id] = SetControllerToPlayer;
            switch (gLinkPlayers[i].id)
            {
            case 0:
            case 3:
                gBattlerPositions[gLinkPlayers[i].id] = B_POSITION_PLAYER_LEFT;
                gBattlerPartyIndexes[gLinkPlayers[i].id] = 0;
                break;
            case 1:
            case 2:
                gBattlerPositions[gLinkPlayers[i].id] = B_POSITION_PLAYER_RIGHT;
                gBattlerPartyIndexes[gLinkPlayers[i].id] = 3;
                break;
            }
        }
        else
        {
            if ((!(gLinkPlayers[i].id & 1) && !(gLinkPlayers[multiplayerId].id & 1))
             || ((gLinkPlayers[i].id & 1) && (gLinkPlayers[multiplayerId].id & 1)))
            {
                gBattlerControllerFuncs[gLinkPlayers[i].id] = SetControllerToLinkPartner;
                switch (gLinkPlayers[i].id)
                {
                case 0:
                case 3:
                    gBattlerPositions[gLinkPlayers[i].id] = B_POSITION_PLAYER_LEFT;
                    gBattlerPartyIndexes[gLinkPlayers[i].id] = 0;
                    break;
                case 1:
                case 2:
                    gBattlerPositions[gLinkPlayers[i].id] = B_POSITION_PLAYER_RIGHT;
                    gBattlerPartyIndexes[gLinkPlayers[i].id] = 3;
                    break;
                }
            }
            else
            {
                gBattlerControllerFuncs[gLinkPlayers[i].id] = SetControllerToLinkOpponent;
                switch (gLinkPlayers[i].id)
                {
                case 0:
                case 3:
                    gBattlerPositions[gLinkPlayers[i].id] = B_POSITION_OPPONENT_LEFT;
                    gBattlerPartyIndexes[gLinkPlayers[i].id] = 0;
                    break;
                case 1:
                case 2:
                    gBattlerPositions[gLinkPlayers[i].id] = B_POSITION_OPPONENT_RIGHT;
                    gBattlerPartyIndexes[gLinkPlayers[i].id] = 3;
                    break;
                }
            }
        }
    }
    gBattlersCount = MAX_BATTLERS_COUNT;
}

static void SetBattlePartyIds(void)
{
    s32 i, j;

    if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        for (i = 0; i < gBattlersCount; i++)
        {
            for (j = 0; j < PARTY_SIZE; j++)
            {
                if (i < 2)
                {
                    if (GET_BATTLER_SIDE2(i) == B_SIDE_PLAYER)
                    {
                        if (GetMonData(&gPlayerParty[j], MON_DATA_HP) != 0
                         && GetMonData(&gPlayerParty[j], MON_DATA_SPECIES2) != SPECIES_NONE
                         && GetMonData(&gPlayerParty[j], MON_DATA_SPECIES2) != SPECIES_EGG
                         && GetMonData(&gPlayerParty[j], MON_DATA_IS_EGG) == 0)
                        {
                            gBattlerPartyIndexes[i] = j;
                            break;
                        }
                    }
                    else
                    {
                        if (GetMonData(&gEnemyParty[j], MON_DATA_HP) != 0
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES2) != SPECIES_NONE
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES2) != SPECIES_EGG
                         && GetMonData(&gEnemyParty[j], MON_DATA_IS_EGG) == 0)
                        {
                            gBattlerPartyIndexes[i] = j;
                            break;
                        }
                    }
                }
                else
                {
                    if (GET_BATTLER_SIDE2(i) == B_SIDE_PLAYER)
                    {
                        if (GetMonData(&gPlayerParty[j], MON_DATA_HP) != 0
                         && GetMonData(&gPlayerParty[j], MON_DATA_SPECIES) != SPECIES_NONE  // Probably a typo by Game Freak. The rest use SPECIES2
                         && GetMonData(&gPlayerParty[j], MON_DATA_SPECIES2) != SPECIES_EGG
                         && GetMonData(&gPlayerParty[j], MON_DATA_IS_EGG) == 0
                         && gBattlerPartyIndexes[i - 2] != j)
                        {
                            gBattlerPartyIndexes[i] = j;
                            break;
                        }
                    }
                    else
                    {
                        if (GetMonData(&gEnemyParty[j], MON_DATA_HP) != 0
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES2) != SPECIES_NONE
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES2) != SPECIES_EGG
                         && GetMonData(&gEnemyParty[j], MON_DATA_IS_EGG) == 0
                         && gBattlerPartyIndexes[i - 2] != j)
                        {
                            gBattlerPartyIndexes[i] = j;
                            break;
                        }
                    }
                }
            }
        }
    }
}

static void PrepareBufferDataTransfer(u8 bufferId, u8 *data, u16 size)
{
    s32 i;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        PrepareBufferDataTransferLink(bufferId, size, data);
    }
    else
    {
        switch (bufferId)
        {
        case B_COMM_TO_CONTROLLER:
            for (i = 0; i < size; i++)
            {
                gBattleBufferA[gActiveBattler][i] = *data;
                data++;
            }
            break;
        case B_COMM_TO_ENGINE:
            for (i = 0; i < size; i++)
            {
                gBattleBufferB[gActiveBattler][i] = *data;
                data++;
            }
            break;
        }
    }
}

#define tInitialDelayTimer      data[10]
#define tState                  data[11]
#define tCurrentBlock_WrapFrom  data[12]
#define tBlockSendDelayTimer    data[13]
#define tCurrentBlock_End       data[14]
#define tCurrentBlock_Start     data[15]

enum
{
    LINK_BUFF_BUFFER_ID,
    LINK_BUFF_ACTIVE_BATTLER,
    LINK_BUFF_ATTACKER,
    LINK_BUFF_TARGET,
    LINK_BUFF_SIZE_LO,
    LINK_BUFF_SIZE_HI,
    LINK_BUFF_ABSENT_BATTLER_FLAGS,
    LINK_BUFF_EFFECT_BATTLER,
    LINK_BUFF_DATA,
};

enum
{
    SENDTASK_STATE_INITIALIZE,
    SENDTASK_STATE_INITIAL_DELAY,
    SENDTASK_STATE_COUNT_PLAYERS,
    SENDTASK_STATE_BEGIN_SEND_BLOCK,
    SENDTASK_STATE_FINISH_SEND_BLOCK,
    SENDTASK_STATE_UNUSED_STATE,
};

static void CreateTasksForSendRecvLinkBuffers(void)
{
    sLinkSendTaskId = CreateTask(Task_HandleSendLinkBuffersData, 0);
    gTasks[sLinkSendTaskId].tState = 0;
    gTasks[sLinkSendTaskId].tCurrentBlock_WrapFrom = 0;
    gTasks[sLinkSendTaskId].tBlockSendDelayTimer = 0;
    gTasks[sLinkSendTaskId].tCurrentBlock_End = 0;
    gTasks[sLinkSendTaskId].tCurrentBlock_Start = 0;

    sLinkReceiveTaskId = CreateTask(Task_HandleCopyReceivedLinkBuffersData, 0);
    gTasks[sLinkReceiveTaskId].tCurrentBlock_WrapFrom = 0;
    gTasks[sLinkReceiveTaskId].tBlockSendDelayTimer = 0;
    gTasks[sLinkReceiveTaskId].tCurrentBlock_End = 0;
    gTasks[sLinkReceiveTaskId].tCurrentBlock_Start = 0;

    sUnused = 0;
    CpuFill16(0, &gSharedMem[BSTRUCT_OFF(linkSend)], sizeof(gBattleStruct->linkSend) + sizeof(gBattleStruct->linkRecv));
}

void PrepareBufferDataTransferLink(u8 bufferId, u16 size, u8 *data)
{
    s32 alignedSize;
    int i;

    alignedSize = size - size % 4 + 4;
    if (gTasks[sLinkSendTaskId].tCurrentBlock_End + alignedSize + LINK_BUFF_DATA + 1 > BATTLE_BUFFER_LINK_SIZE)
    {
        gTasks[sLinkSendTaskId].tCurrentBlock_WrapFrom = gTasks[sLinkSendTaskId].tCurrentBlock_End;
        gTasks[sLinkSendTaskId].tCurrentBlock_End = 0;
    }

    BCOMM(linkSend, bufferId, gTasks[sLinkSendTaskId].tCurrentBlock_End, 0) = bufferId;
    BCOMM(linkSend, activeBattler, gTasks[sLinkSendTaskId].tCurrentBlock_End, 0) = gActiveBattler;
    BCOMM(linkSend, battlerAttacker, gTasks[sLinkSendTaskId].tCurrentBlock_End, 0) = gBattlerAttacker;
    BCOMM(linkSend, battlerTarget, gTasks[sLinkSendTaskId].tCurrentBlock_End, 0) = gBattlerTarget;
    BCOMM(linkSend, size, gTasks[sLinkSendTaskId].tCurrentBlock_End, 0) = alignedSize;
    BCOMM(linkSend, size, gTasks[sLinkSendTaskId].tCurrentBlock_End, 1) = (alignedSize & 0x0000FF00) >> 8;
    BCOMM(linkSend, absentBattlerFlags, gTasks[sLinkSendTaskId].tCurrentBlock_End, 0) = gAbsentBattlerFlags;
    BCOMM(linkSend, effectBattler, gTasks[sLinkSendTaskId].tCurrentBlock_End, 0) = gEffectBattler;

    for (i = 0; i < size; i++)
        BCOMM(linkSend, data, gTasks[sLinkSendTaskId].tCurrentBlock_End, i) = data[i];
    gTasks[sLinkSendTaskId].tCurrentBlock_End = gTasks[sLinkSendTaskId].tCurrentBlock_End + alignedSize + LINK_BUFF_DATA;
}

static void Task_HandleSendLinkBuffersData(u8 taskId)
{
    u16 blockSize;

    switch (gTasks[taskId].tState)
    {
    case SENDTASK_STATE_INITIALIZE:
        gTasks[taskId].tInitialDelayTimer = 100;
        gTasks[taskId].tState++;
        break;
    case SENDTASK_STATE_INITIAL_DELAY:
        gTasks[taskId].tInitialDelayTimer--;
        if (gTasks[taskId].tInitialDelayTimer == 0)
            gTasks[taskId].tState++;
        break;
    case SENDTASK_STATE_COUNT_PLAYERS:
        GetLinkPlayerCount_2();
        if (IsLinkMaster())
        {
            CheckShouldAdvanceLinkState();
            gTasks[taskId].tState++;
            break;
        }
        gTasks[taskId].tState++;
        break;
    case SENDTASK_STATE_BEGIN_SEND_BLOCK:
        if (gTasks[taskId].tCurrentBlock_Start != gTasks[taskId].tCurrentBlock_End)
        {
            if (gTasks[taskId].tBlockSendDelayTimer == 0)
            {
                if (gTasks[taskId].tCurrentBlock_Start > gTasks[taskId].tCurrentBlock_End
                 && gTasks[taskId].tCurrentBlock_Start == gTasks[taskId].tCurrentBlock_WrapFrom)
                {
                    gTasks[taskId].tCurrentBlock_WrapFrom = 0;
                    gTasks[taskId].tCurrentBlock_Start = 0;
                }
                blockSize = (BCOMM(linkSend, size, gTasks[taskId].tCurrentBlock_Start, 0) | (BCOMM(linkSend, size, gTasks[taskId].tCurrentBlock_Start, 1) << 8)) + LINK_BUFF_DATA;
                SendBlock(BitmaskAllOtherLinkPlayers(), &gSharedMem[BSTRUCT_OFF(linkSend) + gTasks[taskId].tCurrentBlock_Start], blockSize);
                gTasks[taskId].tState++;
            }
            else
            {
                gTasks[taskId].tBlockSendDelayTimer--;
                break;
            }
        }
        break;
    case SENDTASK_STATE_FINISH_SEND_BLOCK:
        if (IsLinkTaskFinished())
        {
            blockSize = BCOMM(linkSend, size, gTasks[taskId].tCurrentBlock_Start, 0) | (BCOMM(linkSend, size, gTasks[taskId].tCurrentBlock_Start, 1) << 8);
            gTasks[taskId].tBlockSendDelayTimer = 1;
            gTasks[taskId].tCurrentBlock_Start = gTasks[taskId].tCurrentBlock_Start + blockSize + LINK_BUFF_DATA;
            gTasks[taskId].tState = SENDTASK_STATE_BEGIN_SEND_BLOCK;
        }
        break;
    case SENDTASK_STATE_UNUSED_STATE:
        gTasks[taskId].tBlockSendDelayTimer--;
        if (gTasks[taskId].tBlockSendDelayTimer == 0)
        {
            gTasks[taskId].tBlockSendDelayTimer = 1;
            gTasks[taskId].tState = SENDTASK_STATE_BEGIN_SEND_BLOCK;
        }
        break;
    }
}

void TryReceiveLinkBattleData(void)
{
    u8 i;
    s32 j;
    u16 dataSize;
    u16 *recvBuffer;
    u8 *dest;
    u8 *src;

    if (gReceivedRemoteLinkPlayers != 0 && (gBattleTypeFlags & BATTLE_TYPE_LINK_IN_BATTLE) && gLinkPlayers[0].linkType == LINKTYPE_BATTLE)
    {
        for (i = 0; i < GetLinkPlayerCount(); i++)
        {
            if (GetBlockReceivedStatus() & gBitTable[i])
            {
                ResetBlockReceivedFlag(i);
                recvBuffer = gBlockRecvBuffer[i];
#ifndef NONMATCHING
                asm("");
                recvBuffer = gBlockRecvBuffer[i];
#endif
                dataSize = gBlockRecvBuffer[i][2];
                if (gTasks[sLinkReceiveTaskId].tCurrentBlock_End + LINK_BUFF_DATA + 1 + dataSize > BATTLE_BUFFER_LINK_SIZE)
                {
                    gTasks[sLinkReceiveTaskId].tCurrentBlock_WrapFrom = gTasks[sLinkReceiveTaskId].tCurrentBlock_End;
                    gTasks[sLinkReceiveTaskId].tCurrentBlock_End = 0;
                }
                dest = &gSharedMem[BSTRUCT_OFF(linkRecv) + gTasks[sLinkReceiveTaskId].tCurrentBlock_End];
                src = (u8 *)recvBuffer;
                for (j = 0; j < dataSize + LINK_BUFF_DATA; j++)
                    dest[j] = src[j];
                gTasks[sLinkReceiveTaskId].tCurrentBlock_End = gTasks[sLinkReceiveTaskId].tCurrentBlock_End + dataSize + LINK_BUFF_DATA;
            }
        }
    }
}

static void Task_HandleCopyReceivedLinkBuffersData(u8 taskId)
{
    u16 blockSize;
    u8 battler;
    u8 playerId;

    if (gTasks[taskId].tCurrentBlock_Start != gTasks[taskId].tCurrentBlock_End)
    {
        if (gTasks[taskId].tCurrentBlock_Start > gTasks[taskId].tCurrentBlock_End
         && gTasks[taskId].tCurrentBlock_Start == gTasks[taskId].tCurrentBlock_WrapFrom)
        {
            gTasks[taskId].tCurrentBlock_WrapFrom = 0;
            gTasks[taskId].tCurrentBlock_Start = 0;
        }
        battler = BCOMM(linkRecv, activeBattler, gTasks[taskId].tCurrentBlock_Start, 0);
        blockSize = BCOMM(linkRecv, size, gTasks[taskId].tCurrentBlock_Start, 0)
                 | (BCOMM(linkRecv, size, gTasks[taskId].tCurrentBlock_Start, 1) << 8);
        switch (BCOMM(linkRecv, bufferId, gTasks[taskId].tCurrentBlock_Start, 0))
        {
        case B_COMM_TO_CONTROLLER:
            if (gBattleControllerExecFlags & gBitTable[battler])
                return;
            memcpy(gBattleBufferA[battler], &BCOMM(linkRecv, data, gTasks[taskId].tCurrentBlock_Start, 0), blockSize);
            MarkBattlerReceivedLinkData(battler);
            if (!(gBattleTypeFlags & BATTLE_TYPE_WILD))
            {
                gBattlerAttacker    = BCOMM(linkRecv, battlerAttacker, gTasks[taskId].tCurrentBlock_Start, 0);
                gBattlerTarget      = BCOMM(linkRecv, battlerTarget, gTasks[taskId].tCurrentBlock_Start, 0);
                gAbsentBattlerFlags = BCOMM(linkRecv, absentBattlerFlags, gTasks[taskId].tCurrentBlock_Start, 0);
                gEffectBattler      = BCOMM(linkRecv, effectBattler, gTasks[taskId].tCurrentBlock_Start, 0);
            }
            break;
        case B_COMM_TO_ENGINE:
            memcpy(gBattleBufferB[battler], &BCOMM(linkRecv, data, gTasks[taskId].tCurrentBlock_Start, 0), blockSize);
            break;
        case B_COMM_CONTROLLER_IS_DONE:
            playerId = BCOMM(linkRecv, data, gTasks[taskId].tCurrentBlock_Start, 0);
            gBattleControllerExecFlags &= ~(gBitTable[battler] << (playerId * 4));
            break;
        }

        gTasks[taskId].tCurrentBlock_Start = gTasks[taskId].tCurrentBlock_Start + blockSize + LINK_BUFF_DATA;
    }
}

#undef tInitialDelayTimer
#undef tState
#undef tCurrentBlock_WrapFrom
#undef tBlockSendDelayTimer
#undef tCurrentBlock_End
#undef tCurrentBlock_Start

void BtlController_EmitGetMonData(u8 bufferId, u8 requestId, u8 monToCheck)
{
    gBattleBuffersTransferData[0] = CONTROLLER_GETMONDATA;
    gBattleBuffersTransferData[1] = requestId;
    gBattleBuffersTransferData[2] = monToCheck;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitGetRawMonData(u8 bufferId, u8 monId, u8 bytes)
{
    gBattleBuffersTransferData[0] = CONTROLLER_GETRAWMONDATA;
    gBattleBuffersTransferData[1] = monId;
    gBattleBuffersTransferData[2] = bytes;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitSetMonData(u8 bufferId, u8 requestId, u8 monToCheck, u8 bytes, void *data)
{
    s32 i;

    gBattleBuffersTransferData[0] = CONTROLLER_SETMONDATA;
    gBattleBuffersTransferData[1] = requestId;
    gBattleBuffersTransferData[2] = monToCheck;
    for (i = 0; i < bytes; i++)
        gBattleBuffersTransferData[3 + i] = *(u8*)(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 3 + bytes);
}

void BtlController_EmitSetRawMonData(u8 bufferId, u8 monId, u8 bytes, void *data)
{
    s32 i;

    gBattleBuffersTransferData[0] = CONTROLLER_SETRAWMONDATA;
    gBattleBuffersTransferData[1] = monId;
    gBattleBuffersTransferData[2] = bytes;
    for (i = 0; i < bytes; i++)
        gBattleBuffersTransferData[3 + i] = *(u8*)(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, bytes + 3);
}

void BtlController_EmitLoadMonSprite(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_LOADMONSPRITE;
    gBattleBuffersTransferData[1] = CONTROLLER_LOADMONSPRITE;
    gBattleBuffersTransferData[2] = CONTROLLER_LOADMONSPRITE;
    gBattleBuffersTransferData[3] = CONTROLLER_LOADMONSPRITE;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitSwitchInAnim(u8 bufferId, u8 partyId, bool8 dontClearSubstituteBit)
{
    gBattleBuffersTransferData[0] = CONTROLLER_SWITCHINANIM;
    gBattleBuffersTransferData[1] = partyId;
    gBattleBuffersTransferData[2] = dontClearSubstituteBit;
    gBattleBuffersTransferData[3] = 5;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitReturnMonToBall(u8 bufferId, bool8 skipAnim)
{
    gBattleBuffersTransferData[0] = CONTROLLER_RETURNMONTOBALL;
    gBattleBuffersTransferData[1] = skipAnim;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitDrawTrainerPic(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_DRAWTRAINERPIC;
    gBattleBuffersTransferData[1] = CONTROLLER_DRAWTRAINERPIC;
    gBattleBuffersTransferData[2] = CONTROLLER_DRAWTRAINERPIC;
    gBattleBuffersTransferData[3] = CONTROLLER_DRAWTRAINERPIC;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitTrainerSlide(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_TRAINERSLIDE;
    gBattleBuffersTransferData[1] = CONTROLLER_TRAINERSLIDE;
    gBattleBuffersTransferData[2] = CONTROLLER_TRAINERSLIDE;
    gBattleBuffersTransferData[3] = CONTROLLER_TRAINERSLIDE;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitTrainerSlideBack(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_TRAINERSLIDEBACK;
    gBattleBuffersTransferData[1] = CONTROLLER_TRAINERSLIDEBACK;
    gBattleBuffersTransferData[2] = CONTROLLER_TRAINERSLIDEBACK;
    gBattleBuffersTransferData[3] = CONTROLLER_TRAINERSLIDEBACK;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitFaintAnimation(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_FAINTANIMATION;
    gBattleBuffersTransferData[1] = CONTROLLER_FAINTANIMATION;
    gBattleBuffersTransferData[2] = CONTROLLER_FAINTANIMATION;
    gBattleBuffersTransferData[3] = CONTROLLER_FAINTANIMATION;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitPaletteFade(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_PALETTEFADE;
    gBattleBuffersTransferData[1] = CONTROLLER_PALETTEFADE;
    gBattleBuffersTransferData[2] = CONTROLLER_PALETTEFADE;
    gBattleBuffersTransferData[3] = CONTROLLER_PALETTEFADE;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitSuccessBallThrowAnim(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_SUCCESSBALLTHROWANIM;
    gBattleBuffersTransferData[1] = CONTROLLER_SUCCESSBALLTHROWANIM;
    gBattleBuffersTransferData[2] = CONTROLLER_SUCCESSBALLTHROWANIM;
    gBattleBuffersTransferData[3] = CONTROLLER_SUCCESSBALLTHROWANIM;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitBallThrowAnim(u8 bufferId, u8 caseId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_BALLTHROWANIM;
    gBattleBuffersTransferData[1] = caseId;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitPause(u8 bufferId, u8 toWait, void *data)
{
    s32 i;

    gBattleBuffersTransferData[0] = CONTROLLER_PAUSE;
    gBattleBuffersTransferData[1] = toWait;
    for (i = 0; i < toWait * 3; i++)
        gBattleBuffersTransferData[2 + i] = *(u8*)(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, toWait * 3 + 2);
}

void BtlController_EmitMoveAnimation(u8 bufferId, u16 move, u8 turnOfMove, u16 movePower, s32 dmg, u8 friendship, struct DisableStruct *disableStructPtr)
{
    gBattleBuffersTransferData[0] = CONTROLLER_MOVEANIMATION;
    gBattleBuffersTransferData[1] = move;
    gBattleBuffersTransferData[2] = (move & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = turnOfMove;
    gBattleBuffersTransferData[4] = movePower;
    gBattleBuffersTransferData[5] = (movePower & 0xFF00) >> 8;
    gBattleBuffersTransferData[6] = dmg;
    gBattleBuffersTransferData[7] = (dmg & 0x0000FF00) >> 8;
    gBattleBuffersTransferData[8] = (dmg & 0x00FF0000) >> 16;
    gBattleBuffersTransferData[9] = (dmg & 0xFF000000) >> 24;
    gBattleBuffersTransferData[10] = friendship;
    gBattleBuffersTransferData[11] = gMultiHitCounter;
    if (WEATHER_HAS_EFFECT2)
    {
        gBattleBuffersTransferData[12] = gBattleWeather;
        gBattleBuffersTransferData[13] = (gBattleWeather & 0xFF00) >> 8;
    }
    else
    {
        gBattleBuffersTransferData[12] = 0;
        gBattleBuffersTransferData[13] = 0;
    }
    gBattleBuffersTransferData[14] = 0;
    gBattleBuffersTransferData[15] = 0;
    memcpy(&gBattleBuffersTransferData[16], disableStructPtr, sizeof(struct DisableStruct));
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 16 + sizeof(struct DisableStruct));
}

void BtlController_EmitPrintString(u8 bufferId, u16 stringId)
{
    s32 i;
    struct BattleMsgData* stringInfo;

    gBattleBuffersTransferData[0] = CONTROLLER_PRINTSTRING;
    gBattleBuffersTransferData[1] = gBattleOutcome;
    gBattleBuffersTransferData[2] = stringId;
    gBattleBuffersTransferData[3] = (stringId & 0xFF00) >> 8;

    stringInfo = (struct BattleMsgData*)(&gBattleBuffersTransferData[4]);
    stringInfo->currentMove = gCurrentMove;
    stringInfo->originallyUsedMove = gChosenMove;
    stringInfo->lastItem = gLastUsedItem;
    stringInfo->lastAbility = gLastUsedAbility;
    stringInfo->scrActive = gBattleStruct->battler;
    stringInfo->bakScriptPartyIdx = gBattleStruct->scriptPartyIdx;
    stringInfo->hpScale = gBattleStruct->hpScale;
    stringInfo->itemEffectBattler = gPotentialItemEffectBattler;
    stringInfo->moveType = gBattleMoves[gCurrentMove].type;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        stringInfo->abilities[i] = gBattleMons[i].ability;
    for (i = 0; i < TEXT_BUFF_ARRAY_COUNT; i++)
    {
        stringInfo->textBuffs[0][i] = gBattleTextBuff1[i];
        stringInfo->textBuffs[1][i] = gBattleTextBuff2[i];
        stringInfo->textBuffs[2][i] = gBattleTextBuff3[i];
    }
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, sizeof(struct BattleMsgData) + 4);
}

void BtlController_EmitPrintSelectionString(u8 bufferId, u16 stringId)
{
    s32 i;
    struct BattleMsgData* stringInfo;

    gBattleBuffersTransferData[0] = CONTROLLER_PRINTSTRINGPLAYERONLY;
    gBattleBuffersTransferData[1] = CONTROLLER_PRINTSTRINGPLAYERONLY;
    gBattleBuffersTransferData[2] = stringId;
    gBattleBuffersTransferData[3] = (stringId & 0xFF00) >> 8;

    stringInfo = (struct BattleMsgData*)(&gBattleBuffersTransferData[4]);
    stringInfo->currentMove = gCurrentMove;
    stringInfo->originallyUsedMove = gChosenMove;
    stringInfo->lastItem = gLastUsedItem;
    stringInfo->lastAbility = gLastUsedAbility;
    stringInfo->scrActive = gBattleStruct->battler;
    stringInfo->bakScriptPartyIdx = gBattleStruct->scriptPartyIdx;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        stringInfo->abilities[i] = gBattleMons[i].ability;
    for (i = 0; i < TEXT_BUFF_ARRAY_COUNT; i++)
    {
        stringInfo->textBuffs[0][i] = gBattleTextBuff1[i];
        stringInfo->textBuffs[1][i] = gBattleTextBuff2[i];
        stringInfo->textBuffs[2][i] = gBattleTextBuff3[i];
    }
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, sizeof(struct BattleMsgData) + 4);
}

void BtlController_EmitChooseAction(u8 bufferId, u8 action, u16 itemId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_CHOOSEACTION;
    gBattleBuffersTransferData[1] = action;
    gBattleBuffersTransferData[2] = itemId;
    gBattleBuffersTransferData[3] = (itemId & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

// Ruby's legacy Yes/No command retains an otherwise unused payload byte.
void BtlController_EmitYesNoBox(u8 bufferId, u8 unused)
{
    gBattleBuffersTransferData[0] = CONTROLLER_YESNOBOX;
    gBattleBuffersTransferData[1] = unused;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitChooseMove(u8 bufferId, bool8 isDoubleBattle, bool8 noPPNumber, u8 *movePPData)
{
    u32 i;

    gBattleBuffersTransferData[0] = CONTROLLER_CHOOSEMOVE;
    gBattleBuffersTransferData[1] = isDoubleBattle;
    gBattleBuffersTransferData[2] = noPPNumber;
    gBattleBuffersTransferData[3] = 0;
    for (i = 0; i < 20; i++)
        gBattleBuffersTransferData[4 + i] = movePPData[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 24);
}

void BtlController_EmitChooseItem(u8 bufferId, u8 *battlePartyOrder)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_OPENBAG;
    for (i = 0; i < 3; i++)
        gBattleBuffersTransferData[1 + i] = battlePartyOrder[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitChoosePokemon(u8 bufferId, u8 caseId, u8 slotId, u8 abilityId, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_CHOOSEPOKEMON;
    gBattleBuffersTransferData[1] = caseId;
    gBattleBuffersTransferData[2] = slotId;
    gBattleBuffersTransferData[3] = abilityId;
    for (i = 0; i < 3; i++)
        gBattleBuffersTransferData[4 + i] = data[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 8);  //but only 7 bytes were written
}

void BtlController_EmitCmd23(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_23;
    gBattleBuffersTransferData[1] = CONTROLLER_23;
    gBattleBuffersTransferData[2] = CONTROLLER_23;
    gBattleBuffersTransferData[3] = CONTROLLER_23;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

// FIXME: I think this function is supposed to take s16 as its second argument,
// but battle_4.c expects u16
void BtlController_EmitHealthBarUpdate(u8 bufferId, u16 hpValue)
{
    gBattleBuffersTransferData[0] = CONTROLLER_HEALTHBARUPDATE;
    gBattleBuffersTransferData[1] = 0;
    gBattleBuffersTransferData[2] = (s16)hpValue;
    gBattleBuffersTransferData[3] = ((s16)hpValue & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

// FIXME: I think this function is supposed to take s16 as its third argument,
// but battle_4.c expects u16
void BtlController_EmitExpUpdate(u8 bufferId, u8 partyId, u16 expPoints)
{
    gBattleBuffersTransferData[0] = CONTROLLER_EXPUPDATE;
    gBattleBuffersTransferData[1] = partyId;
    gBattleBuffersTransferData[2] = (s16)expPoints;
    gBattleBuffersTransferData[3] = ((s16)expPoints & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitStatusIconUpdate(u8 bufferId, u32 status1, u32 status2)
{
    gBattleBuffersTransferData[0] = CONTROLLER_STATUSICONUPDATE;
    gBattleBuffersTransferData[1] = status1;
    gBattleBuffersTransferData[2] = (status1 & 0x0000FF00) >> 8;
    gBattleBuffersTransferData[3] = (status1 & 0x00FF0000) >> 16;
    gBattleBuffersTransferData[4] = (status1 & 0xFF000000) >> 24;
    gBattleBuffersTransferData[5] = status2;
    gBattleBuffersTransferData[6] = (status2 & 0x0000FF00) >> 8;
    gBattleBuffersTransferData[7] = (status2 & 0x00FF0000) >> 16;
    gBattleBuffersTransferData[8] = (status2 & 0xFF000000) >> 24;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 9);
}

void BtlController_EmitStatusAnimation(u8 bufferId, bool8 status2, u32 status)
{
    gBattleBuffersTransferData[0] = CONTROLLER_STATUSANIMATION;
    gBattleBuffersTransferData[1] = status2;
    gBattleBuffersTransferData[2] = status;
    gBattleBuffersTransferData[3] = (status & 0x0000FF00) >> 8;
    gBattleBuffersTransferData[4] = (status & 0x00FF0000) >> 16;
    gBattleBuffersTransferData[5] = (status & 0xFF000000) >> 24;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 6);
}

void BtlController_EmitStatusXor(u8 bufferId, u8 value)
{
    gBattleBuffersTransferData[0] = CONTROLLER_STATUSXOR;
    gBattleBuffersTransferData[1] = value;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitDataTransfer(u8 bufferId, u16 size, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_DATATRANSFER;
    gBattleBuffersTransferData[1] = CONTROLLER_DATATRANSFER;
    gBattleBuffersTransferData[2] = size;
    gBattleBuffersTransferData[3] = (size & 0xFF00) >> 8;
    for (i = 0; i < size; i++)
        gBattleBuffersTransferData[4 + i] = *(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, size + 4);
}

void BtlController_EmitDMA3Transfer(u8 bufferId, u32 dst, u16 size, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_DMA3TRANSFER;
    gBattleBuffersTransferData[1] = dst;
    gBattleBuffersTransferData[2] = (dst & 0x0000FF00) >> 8;
    gBattleBuffersTransferData[3] = (dst & 0x00FF0000) >> 16;
    gBattleBuffersTransferData[4] = (dst & 0xFF000000) >> 24;
    gBattleBuffersTransferData[5] = size;
    gBattleBuffersTransferData[6] = (size & 0xFF00) >> 8;
    for (i = 0; i < size; i++)
        gBattleBuffersTransferData[7 + i] = *(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, size + 7);
}

void BtlController_EmitPlayBGM(u8 bufferId, u16 songId, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_PLAYBGM;
    gBattleBuffersTransferData[1] = songId;
    gBattleBuffersTransferData[2] = (songId & 0xFF00) >> 8;
    for (i = 0; i < songId; i++)
        gBattleBuffersTransferData[3 + i] = *(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, songId + 3);
}

void BtlController_EmitCmd32(u8 bufferId, u16 size, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_32;
    gBattleBuffersTransferData[1] = size;
    gBattleBuffersTransferData[2] = (size & 0xFF00) >> 8;
    for (i = 0; i < size; i++)
        gBattleBuffersTransferData[3 + i] = *(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, size + 3);
}

void BtlController_EmitTwoReturnValues(u8 bufferId, u8 ret8, u16 ret16)
{
    gBattleBuffersTransferData[0] = CONTROLLER_TWORETURNVALUES;
    gBattleBuffersTransferData[1] = ret8;
    gBattleBuffersTransferData[2] = ret16;
    gBattleBuffersTransferData[3] = (ret16 & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitChosenMonReturnValue(u8 bufferId, u8 partyId, u8 *battlePartyOrder)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_CHOSENMONRETURNVALUE;
    gBattleBuffersTransferData[1] = partyId;
    for (i = 0; i < 3; i++)
        gBattleBuffersTransferData[2 + i] = battlePartyOrder[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 5);
}

void BtlController_EmitOneReturnValue(u8 bufferId, u16 ret)
{
    gBattleBuffersTransferData[0] = CONTROLLER_ONERETURNVALUE;
    gBattleBuffersTransferData[1] = ret;
    gBattleBuffersTransferData[2] = (ret & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitOneReturnValue_Duplicate(u8 bufferId, u16 ret)
{
    gBattleBuffersTransferData[0] = CONTROLLER_ONERETURNVALUE_DUPLICATE;
    gBattleBuffersTransferData[1] = ret;
    gBattleBuffersTransferData[2] = (ret & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitClearUnkVar(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_CLEARUNKVAR;
    gBattleBuffersTransferData[1] = CONTROLLER_CLEARUNKVAR;
    gBattleBuffersTransferData[2] = CONTROLLER_CLEARUNKVAR;
    gBattleBuffersTransferData[3] = CONTROLLER_CLEARUNKVAR;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitSetUnkVar(u8 bufferId, u8 value)
{
    gBattleBuffersTransferData[0] = CONTROLLER_SETUNKVAR;
    gBattleBuffersTransferData[1] = value;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitClearUnkFlag(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_CLEARUNKFLAG;
    gBattleBuffersTransferData[1] = CONTROLLER_CLEARUNKFLAG;
    gBattleBuffersTransferData[2] = CONTROLLER_CLEARUNKFLAG;
    gBattleBuffersTransferData[3] = CONTROLLER_CLEARUNKFLAG;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitToggleUnkFlag(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_TOGGLEUNKFLAG;
    gBattleBuffersTransferData[1] = CONTROLLER_TOGGLEUNKFLAG;
    gBattleBuffersTransferData[2] = CONTROLLER_TOGGLEUNKFLAG;
    gBattleBuffersTransferData[3] = CONTROLLER_TOGGLEUNKFLAG;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitHitAnimation(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_HITANIMATION;
    gBattleBuffersTransferData[1] = CONTROLLER_HITANIMATION;
    gBattleBuffersTransferData[2] = CONTROLLER_HITANIMATION;
    gBattleBuffersTransferData[3] = CONTROLLER_HITANIMATION;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitCantSwitch(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_CANTSWITCH;
    gBattleBuffersTransferData[1] = CONTROLLER_CANTSWITCH;
    gBattleBuffersTransferData[2] = CONTROLLER_CANTSWITCH;
    gBattleBuffersTransferData[3] = CONTROLLER_CANTSWITCH;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitPlaySE(u8 bufferId, u16 songId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_PLAYSE;
    gBattleBuffersTransferData[1] = songId;
    gBattleBuffersTransferData[2] = (songId & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitPlayFanfareOrBGM(u8 bufferId, u16 songId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_PLAYFANFAREORBGM;
    gBattleBuffersTransferData[1] = songId;
    gBattleBuffersTransferData[2] = (songId & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitFaintingCry(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_FAINTINGCRY;
    gBattleBuffersTransferData[1] = CONTROLLER_FAINTINGCRY;
    gBattleBuffersTransferData[2] = CONTROLLER_FAINTINGCRY;
    gBattleBuffersTransferData[3] = CONTROLLER_FAINTINGCRY;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitIntroSlide(u8 bufferId, u8 environmentId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_INTROSLIDE;
    gBattleBuffersTransferData[1] = environmentId;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitIntroTrainerBallThrow(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_INTROTRAINERBALLTHROW;
    gBattleBuffersTransferData[1] = CONTROLLER_INTROTRAINERBALLTHROW;
    gBattleBuffersTransferData[2] = CONTROLLER_INTROTRAINERBALLTHROW;
    gBattleBuffersTransferData[3] = CONTROLLER_INTROTRAINERBALLTHROW;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitDrawPartyStatusSummary(u8 bufferId, struct HpAndStatus *hpAndStatus, u8 flags)
{
    int i;

    gBattleBuffersTransferData[0] = CONTROLLER_DRAWPARTYSTATUSSUMMARY;
    gBattleBuffersTransferData[1] = flags & ~PARTY_SUMM_SKIP_DRAW_DELAY;
    gBattleBuffersTransferData[2] = (flags & PARTY_SUMM_SKIP_DRAW_DELAY) >> 7;
    gBattleBuffersTransferData[3] = CONTROLLER_DRAWPARTYSTATUSSUMMARY;
    for (i = 0; i < 48; i++)
        gBattleBuffersTransferData[4 + i] = *(i + (u8*)(hpAndStatus));
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 52);
}

void BtlController_EmitHidePartyStatusSummary(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_HIDEPARTYSTATUSSUMMARY;
    gBattleBuffersTransferData[1] = CONTROLLER_HIDEPARTYSTATUSSUMMARY;
    gBattleBuffersTransferData[2] = CONTROLLER_HIDEPARTYSTATUSSUMMARY;
    gBattleBuffersTransferData[3] = CONTROLLER_HIDEPARTYSTATUSSUMMARY;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitEndBounceEffect(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_ENDBOUNCE;
    gBattleBuffersTransferData[1] = CONTROLLER_ENDBOUNCE;
    gBattleBuffersTransferData[2] = CONTROLLER_ENDBOUNCE;
    gBattleBuffersTransferData[3] = CONTROLLER_ENDBOUNCE;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitSpriteInvisibility(u8 bufferId, bool8 isInvisible)
{
    gBattleBuffersTransferData[0] = CONTROLLER_SPRITEINVISIBILITY;
    gBattleBuffersTransferData[1] = isInvisible;
    gBattleBuffersTransferData[2] = CONTROLLER_SPRITEINVISIBILITY;
    gBattleBuffersTransferData[3] = CONTROLLER_SPRITEINVISIBILITY;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitBattleAnimation(u8 bufferId, u8 animationId, u16 argument)
{
    gBattleBuffersTransferData[0] = CONTROLLER_BATTLEANIMATION;
    gBattleBuffersTransferData[1] = animationId;
    gBattleBuffersTransferData[2] = argument;
    gBattleBuffersTransferData[3] = (argument & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitLinkStandbyMsg(u8 bufferId, u8 mode)
{
    gBattleBuffersTransferData[0] = CONTROLLER_LINKSTANDBYMSG;
    gBattleBuffersTransferData[1] = mode;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitResetActionMoveSelection(u8 bufferId, u8 caseId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_RESETACTIONMOVESELECTION;
    gBattleBuffersTransferData[1] = caseId;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitEndLinkBattle(u8 bufferId, u8 battleOutcome)
{
    gBattleBuffersTransferData[0] = CONTROLLER_ENDLINKBATTLE;
    gBattleBuffersTransferData[1] = battleOutcome;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}
