#include "global.h"
#include "battle.h"
#include "battle_main.h"
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
#include "data.h"
#include "ewram.h"
#include "constants/abilities.h"

static void InitLinkBtlControllers(void);
static void UNUSED BtlController_EmitGetRawMonData(u8 bufferId, u8 monId, u8 bytes);
static void UNUSED BtlController_EmitPaletteFade(u8 bufferId);
static void UNUSED BtlController_EmitSuccessBallThrowAnim(u8 bufferId);
static void UNUSED BtlController_EmitCmd23(u8 bufferId);
static void UNUSED BtlController_EmitStatusXor(u8 bufferId, u8 b);
static void UNUSED BtlController_EmitClearUnkVar(u8 bufferId);
static void UNUSED BtlController_EmitSetUnkVar(u8 bufferId, u8 b);
static void UNUSED BtlController_EmitClearUnkFlag(u8 bufferId);
static void UNUSED BtlController_EmitToggleUnkFlag(u8 bufferId);

extern u16 gBattleTypeFlags;
extern u16 gBattleWeather;
extern struct BattlePokemon gBattleMons[];

static EWRAM_DATA u8 sLinkSendTaskId = 0;
static EWRAM_DATA u8 sLinkReceiveTaskId = 0;
static EWRAM_DATA u8 sUnused = 0;

extern u32 gUnusedFirstBattleVar1;
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
extern u8 gUnusedFirstBattleVar2;
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

#if DEBUG
extern u8 gUnknown_02023A14_50;
#endif

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
    if (gUnknown_02023A14_50 & 0x80)
    {
        gSharedMem[0x160fd] = 0;
        gSharedMem[0x160fe] = 0;
        gSharedMem[0x160ff] = 0;
        ((u32 *) gBattleBuffersTransferData)[64]++;
    }
#endif
}

void sub_800B950(void)
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
        gBattlerPositions[0] = 0;
        gBattlerControllerFuncs[1] = SetControllerToOpponent;
        gBattlerPositions[1] = 1;
        gBattlersCount = 2;
    }
    else
    {
        gBattleMainFunc = BeginBattleIntro;
        gBattlerControllerFuncs[0] = SetControllerToPlayer;
        gBattlerPositions[0] = 0;
        gBattlerControllerFuncs[1] = SetControllerToOpponent;
        gBattlerPositions[1] = 1;
        gBattlerControllerFuncs[2] = SetControllerToPlayer;
        gBattlerPositions[2] = 2;
        gBattlerControllerFuncs[3] = SetControllerToOpponent;
        gBattlerPositions[3] = 3;
        gBattlersCount = 4;
    }
}

static void InitLinkBtlControllers(void)
{
    u8 multiplayerId;
    int i;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        if (gBattleTypeFlags & BATTLE_TYPE_IS_MASTER)
        {
            gBattleMainFunc = BeginBattleIntro;
            gBattlerControllerFuncs[0] = SetControllerToPlayer;
            gBattlerPositions[0] = 0;
            gBattlerControllerFuncs[1] = SetControllerToLinkOpponent;
            gBattlerPositions[1] = 1;
            gBattlersCount = 2;
        }
        else
        {
            gBattlerControllerFuncs[1] = SetControllerToPlayer;
            gBattlerPositions[1] = 0;
            gBattlerControllerFuncs[0] = SetControllerToLinkOpponent;
            gBattlerPositions[0] = 1;
            gBattlersCount = 2;
        }
        return;
    }
    if ((gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE)) == BATTLE_TYPE_DOUBLE)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_IS_MASTER)
        {
            gBattleMainFunc = BeginBattleIntro;
            gBattlerControllerFuncs[0] = SetControllerToPlayer;
            gBattlerPositions[0] = 0;
            gBattlerControllerFuncs[1] = SetControllerToLinkOpponent;
            gBattlerPositions[1] = 1;
            gBattlerControllerFuncs[2] = SetControllerToPlayer;
            gBattlerPositions[2] = 2;
            gBattlerControllerFuncs[3] = SetControllerToLinkOpponent;
            gBattlerPositions[3] = 3;
            gBattlersCount = 4;
        }
        else
        {
            gBattlerControllerFuncs[1] = SetControllerToPlayer;
            gBattlerPositions[1] = 0;
            gBattlerControllerFuncs[0] = SetControllerToLinkOpponent;
            gBattlerPositions[0] = 1;
            gBattlerControllerFuncs[3] = SetControllerToPlayer;
            gBattlerPositions[3] = 2;
            gBattlerControllerFuncs[2] = SetControllerToLinkOpponent;
            gBattlerPositions[2] = 3;
            gBattlersCount = 4;

        }
        return;
    }
    multiplayerId = GetMultiplayerId();
    if (gBattleTypeFlags & BATTLE_TYPE_IS_MASTER)
        gBattleMainFunc = BeginBattleIntro;
    for (i = 0; i < 4; i++)
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
                gBattlerPositions[gLinkPlayers[i].id] = 0;
                gBattlerPartyIndexes[gLinkPlayers[i].id] = 0;
                break;
            case 1:
            case 2:
                gBattlerPositions[gLinkPlayers[i].id] = 2;
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
                    gBattlerPositions[gLinkPlayers[i].id] = 0;
                    gBattlerPartyIndexes[gLinkPlayers[i].id] = 0;
                    break;
                case 1:
                case 2:
                    gBattlerPositions[gLinkPlayers[i].id] = 2;
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
                    gBattlerPositions[gLinkPlayers[i].id] = 1;
                    gBattlerPartyIndexes[gLinkPlayers[i].id] = 0;
                    break;
                case 1:
                case 2:
                    gBattlerPositions[gLinkPlayers[i].id] = 3;
                    gBattlerPartyIndexes[gLinkPlayers[i].id] = 3;
                    break;
                }
            }
        }
    }
    gBattlersCount = 4;
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
                         && GetMonData(&gPlayerParty[j], MON_DATA_SPECIES_OR_EGG) != SPECIES_NONE
                         && GetMonData(&gPlayerParty[j], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
                         && GetMonData(&gPlayerParty[j], MON_DATA_IS_EGG) == 0)
                        {
                            gBattlerPartyIndexes[i] = j;
                            break;
                        }
                    }
                    else
                    {
                        if (GetMonData(&gEnemyParty[j], MON_DATA_HP) != 0
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES_OR_EGG) != SPECIES_NONE
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
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
                         && GetMonData(&gPlayerParty[j], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
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
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES_OR_EGG) != SPECIES_NONE
                         && GetMonData(&gEnemyParty[j], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
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
        case 0:
            for (i = 0; i < size; i++)
            {
                gBattleBufferA[gActiveBattler][i] = *data;
                data++;
            }
            break;
        case 1:
            for (i = 0; i < size; i++)
            {
                gBattleBufferB[gActiveBattler][i] = *data;
                data++;
            }
            break;
        }
    }
}

static void CreateTasksForSendRecvLinkBuffers(void)
{
    sLinkSendTaskId = CreateTask(Task_HandleSendLinkBuffersData, 0);
    gTasks[sLinkSendTaskId].data[11] = 0;
    gTasks[sLinkSendTaskId].data[12] = 0;
    gTasks[sLinkSendTaskId].data[13] = 0;
    gTasks[sLinkSendTaskId].data[14] = 0;
    gTasks[sLinkSendTaskId].data[15] = 0;

    sLinkReceiveTaskId = CreateTask(Task_HandleCopyReceivedLinkBuffersData, 0);
    gTasks[sLinkReceiveTaskId].data[12] = 0;
    gTasks[sLinkReceiveTaskId].data[13] = 0;
    gTasks[sLinkReceiveTaskId].data[14] = 0;
    gTasks[sLinkReceiveTaskId].data[15] = 0;

    sUnused = 0;
    CpuFill16(0, &gSharedMem[BSTRUCT_OFF(linkSend)], sizeof(gBattleStruct->linkSend) + sizeof(gBattleStruct->linkRecv));
}

void PrepareBufferDataTransferLink(u8 bufferId, u16 size, u8 *data)
{
    s32 alignedSize;
    int i;

    alignedSize = size - size % 4 + 4;
    if (gTasks[sLinkSendTaskId].data[14] + alignedSize + 9 > 0x1000)
    {
        gTasks[sLinkSendTaskId].data[12] = gTasks[sLinkSendTaskId].data[14];
        gTasks[sLinkSendTaskId].data[14] = 0;
    }

    BCOMM(linkSend, state, gTasks[sLinkSendTaskId].data[14], 0) = bufferId;
    BCOMM(linkSend, activeBattler, gTasks[sLinkSendTaskId].data[14], 0) = gActiveBattler;
    BCOMM(linkSend, battlerAttacker, gTasks[sLinkSendTaskId].data[14], 0) = gBattlerAttacker;
    BCOMM(linkSend, battlerTarget, gTasks[sLinkSendTaskId].data[14], 0) = gBattlerTarget;
    BCOMM(linkSend, size, gTasks[sLinkSendTaskId].data[14], 0) = alignedSize;
    BCOMM(linkSend, size, gTasks[sLinkSendTaskId].data[14], 1) = (alignedSize & 0x0000FF00) >> 8;
    BCOMM(linkSend, absentBattlerFlags, gTasks[sLinkSendTaskId].data[14], 0) = gAbsentBattlerFlags;
    BCOMM(linkSend, effectBattler, gTasks[sLinkSendTaskId].data[14], 0) = gEffectBattler;

    for (i = 0; i < size; i++)
        BCOMM(linkSend, data, gTasks[sLinkSendTaskId].data[14], i) = data[i];
    gTasks[sLinkSendTaskId].data[14] = gTasks[sLinkSendTaskId].data[14] + alignedSize + 8;
}

void Task_HandleSendLinkBuffersData(u8 taskId)
{
    u16 blockSize;

    switch (gTasks[taskId].data[11])
    {
    case 0:
        gTasks[taskId].data[10] = 100;
        gTasks[taskId].data[11]++;
        break;
    case 1:
        gTasks[taskId].data[10]--;
        if (gTasks[taskId].data[10] == 0)
            gTasks[taskId].data[11]++;
        break;
    case 2:
        GetLinkPlayerCount_2();
        if (IsLinkMaster())
        {
            CheckShouldAdvanceLinkState();
            gTasks[taskId].data[11]++;
            break;
        }
        gTasks[taskId].data[11]++;
        break;
    case 3:
        if (gTasks[taskId].data[15] != gTasks[taskId].data[14])
        {
            if (gTasks[taskId].data[13] == 0)
            {
                if (gTasks[taskId].data[15] > gTasks[taskId].data[14]
                 && gTasks[taskId].data[15] == gTasks[taskId].data[12])
                {
                    gTasks[taskId].data[12] = 0;
                    gTasks[taskId].data[15] = 0;
                }
                blockSize = (BCOMM(linkSend, size, gTasks[taskId].data[15], 0) | (BCOMM(linkSend, size, gTasks[taskId].data[15], 1) << 8)) + 8;
                SendBlock(BitmaskAllOtherLinkPlayers(), &gSharedMem[BSTRUCT_OFF(linkSend) + gTasks[taskId].data[15]], blockSize);
                gTasks[taskId].data[11]++;
            }
            else
            {
                gTasks[taskId].data[13]--;
                break;
            }
        }
        break;
    case 4:
        if (IsLinkTaskFinished())
        {
            blockSize = (BCOMM(linkSend, size, gTasks[taskId].data[15], 0) | (BCOMM(linkSend, size, gTasks[taskId].data[15], 1) << 8));
            gTasks[taskId].data[13] = 1;
            gTasks[taskId].data[15] = gTasks[taskId].data[15] + blockSize + 8;
            gTasks[taskId].data[11] = 3;
        }
        break;
    case 5:
        gTasks[taskId].data[13]--;
        if (gTasks[taskId].data[13] == 0)
        {
            gTasks[taskId].data[13] = 1;
            gTasks[taskId].data[11] = 3;
        }
        break;
    }
}

//fix me
void sub_800C35C(void)
{
    u8 i;  //r4
    s32 j;  //r2
    u16 dataSize;  //r6
    u16 *recvBuffer;  //r3
    u8 *dest;  //r5
    u8 *src;  //r4

    if (gReceivedRemoteLinkPlayers != 0 && (gBattleTypeFlags & 0x20) && gLinkPlayers[0].linkType == 0x2211)
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
                if (gTasks[sLinkReceiveTaskId].data[14] + 9 + dataSize > 0x1000)
                {
                    gTasks[sLinkReceiveTaskId].data[12] = gTasks[sLinkReceiveTaskId].data[14];
                    gTasks[sLinkReceiveTaskId].data[14] = 0;
                }
                //_0800C402
                dest = &gSharedMem[BSTRUCT_OFF(linkRecv) + gTasks[sLinkReceiveTaskId].data[14]];
                src = (u8 *)recvBuffer;
                for (j = 0; j < dataSize + 8; j++)
                    dest[j] = src[j];
                gTasks[sLinkReceiveTaskId].data[14] = gTasks[sLinkReceiveTaskId].data[14] + dataSize + 8;
            }
            //_0800C446
        }
    }
}

static void Task_HandleCopyReceivedLinkBuffersData(u8 taskId)
{
    u16 blockSize;
    u8 battlerId;
    u8 playerId;

    if (gTasks[taskId].data[15] != gTasks[taskId].data[14])
    {
        if (gTasks[taskId].data[15] > gTasks[taskId].data[14]
         && gTasks[taskId].data[15] == gTasks[taskId].data[12])
        {
            gTasks[taskId].data[12] = 0;
            gTasks[taskId].data[15] = 0;
        }
        battlerId = BCOMM(linkRecv, activeBattler, gTasks[taskId].data[15], 0);
        blockSize = BCOMM(linkRecv, size, gTasks[taskId].data[15], 0)
                 | (BCOMM(linkRecv, size, gTasks[taskId].data[15], 1) << 8);
        switch (BCOMM(linkRecv, state, gTasks[taskId].data[15], 0))
        {
        case 0:
            if (gBattleControllerExecFlags & gBitTable[battlerId])
                return;
            memcpy(gBattleBufferA[battlerId], &BCOMM(linkRecv, data, gTasks[taskId].data[15], 0), blockSize);
            MarkBattlerReceivedLinkData(battlerId);
            if (!(gBattleTypeFlags & BATTLE_TYPE_IS_MASTER))
            {
                gBattlerAttacker    = BCOMM(linkRecv, battlerAttacker, gTasks[taskId].data[15], 0);
                gBattlerTarget      = BCOMM(linkRecv, battlerTarget, gTasks[taskId].data[15], 0);
                gAbsentBattlerFlags = BCOMM(linkRecv, absentBattlerFlags, gTasks[taskId].data[15], 0);
                gEffectBattler      = BCOMM(linkRecv, effectBattler, gTasks[taskId].data[15], 0);
            }
            break;
        case 1:
            memcpy(gBattleBufferB[battlerId], &BCOMM(linkRecv, data, gTasks[taskId].data[15], 0), blockSize);
            break;
        case 2:
            playerId = BCOMM(linkRecv, data, gTasks[taskId].data[15], 0);
            gBattleControllerExecFlags &= ~(gBitTable[battlerId] << (playerId * 4));
            break;
        }

        gTasks[taskId].data[15] = gTasks[taskId].data[15] + blockSize + 8;
    }
}

void BtlController_EmitGetMonData(u8 bufferId, u8 requestId, u8 monToCheck)
{
    gBattleBuffersTransferData[0] = CONTROLLER_GETMONDATA;
    gBattleBuffersTransferData[1] = requestId;
    gBattleBuffersTransferData[2] = monToCheck;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

static void UNUSED BtlController_EmitGetRawMonData(u8 bufferId, u8 monId, u8 bytes)
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

static void UNUSED BtlController_EmitSetRawMonData(u8 bufferId, u8 monId, u8 bytes, void *data)
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

static void UNUSED BtlController_EmitPaletteFade(u8 bufferId)
{
    gBattleBuffersTransferData[0] = CONTROLLER_PALETTEFADE;
    gBattleBuffersTransferData[1] = CONTROLLER_PALETTEFADE;
    gBattleBuffersTransferData[2] = CONTROLLER_PALETTEFADE;
    gBattleBuffersTransferData[3] = CONTROLLER_PALETTEFADE;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

static void UNUSED BtlController_EmitSuccessBallThrowAnim(u8 bufferId)
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

static void UNUSED BtlController_EmitPause(u8 bufferId, u8 toWait, void *data)
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

    gBattleBuffersTransferData[0] = 16;
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

    for (i = 0; i < 4; i++)
        stringInfo->abilities[i] = gBattleMons[i].ability;
    for (i = 0; i < 0x10; i++)
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

    gBattleBuffersTransferData[0] = 17;
    gBattleBuffersTransferData[1] = 17;
    gBattleBuffersTransferData[2] = stringId;
    gBattleBuffersTransferData[3] = (stringId & 0xFF00) >> 8;

    stringInfo = (struct BattleMsgData*)(&gBattleBuffersTransferData[4]);
    stringInfo->currentMove = gCurrentMove;
    stringInfo->originallyUsedMove = gChosenMove;
    stringInfo->lastItem = gLastUsedItem;
    stringInfo->lastAbility = gLastUsedAbility;
    stringInfo->scrActive = gBattleStruct->battler;
    stringInfo->bakScriptPartyIdx = gBattleStruct->scriptPartyIdx;

    for (i = 0; i < 4; i++)
        stringInfo->abilities[i] = gBattleMons[i].ability;
    for (i = 0; i < 0x10; i++)
    {
        stringInfo->textBuffs[0][i] = gBattleTextBuff1[i];
        stringInfo->textBuffs[1][i] = gBattleTextBuff2[i];
        stringInfo->textBuffs[2][i] = gBattleTextBuff3[i];
    }
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, sizeof(struct BattleMsgData) + 4);
}

void BtlController_EmitChooseAction(u8 bufferId, u8 action, u16 itemId)
{
    gBattleBuffersTransferData[0] = 18;
    gBattleBuffersTransferData[1] = action;
    gBattleBuffersTransferData[2] = itemId;
    gBattleBuffersTransferData[3] = (itemId & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitUnknownYesNoBox(u8 bufferId, u8 b)
{
    gBattleBuffersTransferData[0] = 19;
    gBattleBuffersTransferData[1] = b;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitChooseMove(u8 bufferId, bool8 isDoubleBattle, bool8 noPPNumber, u8 *movePPData)
{
    u32 i;

    gBattleBuffersTransferData[0] = 20;
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

    gBattleBuffersTransferData[0] = 21;
    for (i = 0; i < 3; i++)
        gBattleBuffersTransferData[1 + i] = battlePartyOrder[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitChoosePokemon(u8 bufferId, u8 caseId, u8 slotId, u8 abilityId, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = 22;
    gBattleBuffersTransferData[1] = caseId;
    gBattleBuffersTransferData[2] = slotId;
    gBattleBuffersTransferData[3] = abilityId;
    for (i = 0; i < 3; i++)
        gBattleBuffersTransferData[4 + i] = data[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 8);  //but only 7 bytes were written
}

static void UNUSED BtlController_EmitCmd23(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 23;
    gBattleBuffersTransferData[1] = 23;
    gBattleBuffersTransferData[2] = 23;
    gBattleBuffersTransferData[3] = 23;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

// FIXME: I think this function is supposed to take s16 as its second argument,
// but battle_4.c expects u16
void BtlController_EmitHealthBarUpdate(u8 bufferId, u16 hpValue)
{
    gBattleBuffersTransferData[0] = 24;
    gBattleBuffersTransferData[1] = 0;
    gBattleBuffersTransferData[2] = (s16)hpValue;
    gBattleBuffersTransferData[3] = ((s16)hpValue & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

// FIXME: I think this function is supposed to take s16 as its third argument,
// but battle_4.c expects u16
void BtlController_EmitExpUpdate(u8 bufferId, u8 partyId, u16 expPoints)
{
    gBattleBuffersTransferData[0] = 25;
    gBattleBuffersTransferData[1] = partyId;
    gBattleBuffersTransferData[2] = (s16)expPoints;
    gBattleBuffersTransferData[3] = ((s16)expPoints & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitStatusIconUpdate(u8 bufferId, u32 status1, u32 status2)
{
    gBattleBuffersTransferData[0] = 26;
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
    gBattleBuffersTransferData[0] = 27;
    gBattleBuffersTransferData[1] = status2;
    gBattleBuffersTransferData[2] = status;
    gBattleBuffersTransferData[3] = (status & 0x0000FF00) >> 8;
    gBattleBuffersTransferData[4] = (status & 0x00FF0000) >> 16;
    gBattleBuffersTransferData[5] = (status & 0xFF000000) >> 24;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 6);
}

static void UNUSED BtlController_EmitStatusXor(u8 bufferId, u8 b)
{
    gBattleBuffersTransferData[0] = 28;
    gBattleBuffersTransferData[1] = b;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitDataTransfer(u8 bufferId, u16 size, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = 29;
    gBattleBuffersTransferData[1] = 29;
    gBattleBuffersTransferData[2] = size;
    gBattleBuffersTransferData[3] = (size & 0xFF00) >> 8;
    for (i = 0; i < size; i++)
        gBattleBuffersTransferData[4 + i] = *(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, size + 4);
}

static void UNUSED BtlController_EmitDMA3Transfer(u8 bufferId, u32 dst, u16 size, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = 30;
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

static void UNUSED BtlController_EmitPlayBGM(u8 bufferId, u16 songId, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = 31;
    gBattleBuffersTransferData[1] = songId;
    gBattleBuffersTransferData[2] = (songId & 0xFF00) >> 8;
    for (i = 0; i < songId; i++)
        gBattleBuffersTransferData[3 + i] = *(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, songId + 3);
}

static void UNUSED BtlController_EmitCmd32(u8 bufferId, u16 size, u8 *data)
{
    int i;

    gBattleBuffersTransferData[0] = 32;
    gBattleBuffersTransferData[1] = size;
    gBattleBuffersTransferData[2] = (size & 0xFF00) >> 8;
    for (i = 0; i < size; i++)
        gBattleBuffersTransferData[3 + i] = *(data++);
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, size + 3);
}

void BtlController_EmitTwoReturnValues(u8 bufferId, u8 ret8, u16 ret16)
{
    gBattleBuffersTransferData[0] = 33;
    gBattleBuffersTransferData[1] = ret8;
    gBattleBuffersTransferData[2] = ret16;
    gBattleBuffersTransferData[3] = (ret16 & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitChosenMonReturnValue(u8 bufferId, u8 partyId, u8 *battlePartyOrder)
{
    int i;

    gBattleBuffersTransferData[0] = 34;
    gBattleBuffersTransferData[1] = partyId;
    for (i = 0; i < 3; i++)
        gBattleBuffersTransferData[2 + i] = battlePartyOrder[i];
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 5);
}

void BtlController_EmitOneReturnValue(u8 bufferId, u16 ret)
{
    gBattleBuffersTransferData[0] = 35;
    gBattleBuffersTransferData[1] = ret;
    gBattleBuffersTransferData[2] = (ret & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitOneReturnValue_Duplicate(u8 bufferId, u16 ret)
{
    gBattleBuffersTransferData[0] = 36;
    gBattleBuffersTransferData[1] = ret;
    gBattleBuffersTransferData[2] = (ret & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

static void UNUSED BtlController_EmitClearUnkVar(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 37;
    gBattleBuffersTransferData[1] = 37;
    gBattleBuffersTransferData[2] = 37;
    gBattleBuffersTransferData[3] = 37;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

static void UNUSED BtlController_EmitSetUnkVar(u8 bufferId, u8 b)
{
    gBattleBuffersTransferData[0] = 38;
    gBattleBuffersTransferData[1] = b;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

static void UNUSED BtlController_EmitClearUnkFlag(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 39;
    gBattleBuffersTransferData[1] = 39;
    gBattleBuffersTransferData[2] = 39;
    gBattleBuffersTransferData[3] = 39;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

static void UNUSED BtlController_EmitToggleUnkFlag(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 40;
    gBattleBuffersTransferData[1] = 40;
    gBattleBuffersTransferData[2] = 40;
    gBattleBuffersTransferData[3] = 40;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitHitAnimation(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 41;
    gBattleBuffersTransferData[1] = 41;
    gBattleBuffersTransferData[2] = 41;
    gBattleBuffersTransferData[3] = 41;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitCantSwitch(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 42;
    gBattleBuffersTransferData[1] = 42;
    gBattleBuffersTransferData[2] = 42;
    gBattleBuffersTransferData[3] = 42;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitPlaySE(u8 bufferId, u16 songId)
{
    gBattleBuffersTransferData[0] = 43;
    gBattleBuffersTransferData[1] = songId;
    gBattleBuffersTransferData[2] = (songId & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitPlayFanfareOrBGM(u8 bufferId, u16 songId)
{
    gBattleBuffersTransferData[0] = 44;
    gBattleBuffersTransferData[1] = songId;
    gBattleBuffersTransferData[2] = (songId & 0xFF00) >> 8;
    gBattleBuffersTransferData[3] = 0;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitFaintingCry(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 45;
    gBattleBuffersTransferData[1] = 45;
    gBattleBuffersTransferData[2] = 45;
    gBattleBuffersTransferData[3] = 45;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitIntroSlide(u8 bufferId, u8 environmentId)
{
    gBattleBuffersTransferData[0] = 46;
    gBattleBuffersTransferData[1] = environmentId;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitIntroTrainerBallThrow(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 47;
    gBattleBuffersTransferData[1] = 47;
    gBattleBuffersTransferData[2] = 47;
    gBattleBuffersTransferData[3] = 47;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitDrawPartyStatusSummary(u8 bufferId, struct HpAndStatus *hpAndStatus, u8 flags)
{
    int i;

    gBattleBuffersTransferData[0] = 48;
    gBattleBuffersTransferData[1] = flags & 0x7F;
    gBattleBuffersTransferData[2] = (flags & 0x80) >> 7;
    gBattleBuffersTransferData[3] = 48;
    for (i = 0; i < 48; i++)
        gBattleBuffersTransferData[4 + i] = *(i + (u8*)(hpAndStatus));
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 52);
}

void BtlController_EmitHidePartyStatusSummary(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 49;
    gBattleBuffersTransferData[1] = 49;
    gBattleBuffersTransferData[2] = 49;
    gBattleBuffersTransferData[3] = 49;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitEndBounceEffect(u8 bufferId)
{
    gBattleBuffersTransferData[0] = 50;
    gBattleBuffersTransferData[1] = 50;
    gBattleBuffersTransferData[2] = 50;
    gBattleBuffersTransferData[3] = 50;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitSpriteInvisibility(u8 bufferId, bool8 isInvisible)
{
    gBattleBuffersTransferData[0] = 51;
    gBattleBuffersTransferData[1] = isInvisible;
    gBattleBuffersTransferData[2] = 51;
    gBattleBuffersTransferData[3] = 51;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitBattleAnimation(u8 bufferId, u8 animationId, u16 argument)
{
    gBattleBuffersTransferData[0] = 52;
    gBattleBuffersTransferData[1] = animationId;
    gBattleBuffersTransferData[2] = argument;
    gBattleBuffersTransferData[3] = (argument & 0xFF00) >> 8;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 4);
}

void BtlController_EmitLinkStandbyMsg(u8 bufferId, u8 mode)
{
    gBattleBuffersTransferData[0] = 53;
    gBattleBuffersTransferData[1] = mode;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitResetActionMoveSelection(u8 bufferId, u8 caseId)
{
    gBattleBuffersTransferData[0] = 54;
    gBattleBuffersTransferData[1] = caseId;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}

void BtlController_EmitCmd55(u8 bufferId, u8 battleOutcome)
{
    gBattleBuffersTransferData[0] = 55;
    gBattleBuffersTransferData[1] = battleOutcome;
    PrepareBufferDataTransfer(bufferId, gBattleBuffersTransferData, 2);
}
