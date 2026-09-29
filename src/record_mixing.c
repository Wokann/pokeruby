#include "global.h"
#include "record_mixing.h"
#include "battle_tower.h"
#include "cable_club.h"
#include "daycare.h"
#include "dewford_trend.h"
#include "event_data.h"
#include "fldeff_recordmixing.h"
#include "item.h"
#include "constants/items.h"
#include "load_save.h"
#include "link.h"
#include "mauville_old_man.h"
#include "menu.h"
#include "mystery_event_script.h"
#include "random.h"
#include "overworld.h"
#include "save.h"
#include "script.h"
#include "secret_base.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings2.h"
#include "task.h"
#include "tv.h"
#include "ewram.h"

EWRAM_DATA struct RecordMixingDayCareMail gRecordMixMail = {0};
extern u16 gSpecialVar_0x8005;

u32 gRecordStructSize;

static bool8 sReadyToReceive;
static u8 sBattleTowerPlayerIndices[MAX_LINK_PLAYERS];

void *gSecretBasesSave = &gSaveBlock1.secretBases;
void *gTvShowsSave = &gSaveBlock1.tvShows;
void *gPokeNewsSave = &gSaveBlock1.pokeNews;
void *gOldManSave = &gSaveBlock1.oldMan;
void *gDewfordTrendsSave = &gSaveBlock1.dewfordTrends;
struct RecordMixingDayCareMail *gRecordMixMailSave = &gRecordMixMail;
struct BattleTowerRecord *gBattleTowerSave = &gSaveBlock2.battleTower.playerRecord;

#define BUFFER_CHUNK_SIZE 200
#define NUM_RECORD_MIXING_CHUNKS 24
#define NUM_SWAP_COMBOS 3

void RecordMixingPlayerSpotTriggered(void)
{
    CreateTask_EnterCableClubSeat(Task_RecordMixing_Main);
}

struct PlayerRecords
{
    struct SecretBaseRecord secretBases[SECRET_BASES_COUNT];
    TVShow tvShows[TV_SHOWS_COUNT];
    struct PokeNews pokeNews[POKE_NEWS_COUNT];
    union OldMan oldMan;
    struct DewfordTrend dewfordTrends[SAVED_TRENDS_COUNT];
    struct RecordMixingDayCareMail daycareMailRecord;
    struct BattleTowerRecord battleTowerRecord;
    u16 giftItem;
    u16 filler11CA[0x33];
};

void PrepareExchangePacket(void);
void ReceiveExchangePacket(u32 multiplayerId);
void Task_RecordMixing_SoundEffect(u8 taskId);
void Task_MixingRecordsRecv(u8 taskId);
void Task_SendPacket(u8 taskId);
void Task_CopyReceiveBuffer(u8 taskId);
void Task_WaitReceivePacket(u8 taskId);
void Task_ReceivePacket(u8 taskId);
void Task_SendPacket_SwitchToReceive(u8 taskId);
void *LoadPtrFromTaskData(const u16 *taskData);
void StorePtrInTaskData(void *ptr, u16 *taskData);
u8 GetMultiplayerId_(void);
u16 *GetPlayerRecvBuffer(u8 player);
void SetRecordStructSize(void);
void ShufflePlayerIndices(u8 *playerIndices);
void ReceiveOldManData(u8 *records, size_t recordSize, u8 multiplayerId);
void ReceiveBattleTowerData(void *records, size_t recordSize, u8 multiplayerId);
u8 GetDaycareMailItemId(struct DayCareMail *mail);
void SwapDaycareMail(u8 *records, size_t recordSize, u8 (*idxs)[2], u8 playerSlot1, u8 playerSlot2);
u8 CalculateDaycareMailRandSum(const u8 *shows);
void ReceiveDaycareMailData(struct RecordMixingDayCareMail *records, size_t recordSize, u8 multiplayerId, TVShow *shows);
void ReceiveGiftItem(u16 *item, u8 multiplayerId);
void Task_DoRecordMixing(u8 taskId);

void PrepareExchangePacket(void)
{
    SetPlayerSecretBaseParty();
    DeactivateAllNormalTVShows();

    memcpy(eSentRecord.secretBases, gSecretBasesSave, sizeof(eSentRecord.secretBases));
    memcpy(eSentRecord.tvShows, gTvShowsSave, sizeof(eSentRecord.tvShows));
    memcpy(eSentRecord.pokeNews, gPokeNewsSave, sizeof(eSentRecord.pokeNews));
    memcpy(&eSentRecord.oldMan, gOldManSave, sizeof(eSentRecord.oldMan));
    memcpy(eSentRecord.dewfordTrends, gDewfordTrendsSave, sizeof(eSentRecord.dewfordTrends));
    gRecordMixMail.mail[0] = gSaveBlock1.daycare.misc.mail[0];
    gRecordMixMail.mail[1] = gSaveBlock1.daycare.misc.mail[1];
    InitDaycareMailRecordMixing(gSaveBlock1.daycare.mons, &gRecordMixMail);
    memcpy(&eSentRecord.daycareMailRecord, gRecordMixMailSave, sizeof(struct RecordMixingDayCareMail));
    memcpy(&eSentRecord.battleTowerRecord, gBattleTowerSave, sizeof(struct BattleTowerRecord));

    if (GetMultiplayerId() == 0)
        eSentRecord.giftItem = GetRecordMixingGift();
}

void ReceiveExchangePacket(u32 multiplayerId)
{
    ReceiveSecretBasesData(eReceivedRecords.secretBases, sizeof(struct PlayerRecords), multiplayerId);
    ReceiveTvShowsData((u8 *)eReceivedRecords.tvShows, sizeof(struct PlayerRecords), multiplayerId);
    ReceivePokeNewsData(eReceivedRecords.pokeNews, sizeof(struct PlayerRecords), multiplayerId);
    ReceiveOldManData((u8 *)&eReceivedRecords.oldMan, sizeof(struct PlayerRecords), multiplayerId);
    ReceiveDewfordTrendData(eReceivedRecords.dewfordTrends, sizeof(struct PlayerRecords), multiplayerId);
    ReceiveDaycareMailData(&eReceivedRecords.daycareMailRecord,
        sizeof(struct PlayerRecords),
        multiplayerId,
        eReceivedRecords.tvShows);
    ReceiveBattleTowerData(&eReceivedRecords.battleTowerRecord, sizeof(struct PlayerRecords), multiplayerId);
    ReceiveGiftItem(&eReceivedRecords.giftItem, multiplayerId);
}

#define tCounter data[0]

void Task_RecordMixing_SoundEffect(u8 taskId)
{
    gTasks[taskId].tCounter++;
    if (gTasks[taskId].tCounter == 50)
    {
        PlaySE(SE_M_ATTRACT);
        gTasks[taskId].tCounter = 0;
    }
}

#undef tCounter


#define tState        data[0]
#define tTimer        data[8]
#define tLinkTaskId   data[10]
#define tSndEffTaskId data[15]

void Task_RecordMixing_Main(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:        // init
        SetLocalLinkPlayerId(gSpecialVar_0x8005);
        VarSet(VAR_TEMP_MIXED_RECORDS, 1);
        sReadyToReceive = FALSE;
        PrepareExchangePacket();
        CreateRecordMixingLights();
        tState = 1;
        tLinkTaskId = CreateTask(Task_MixingRecordsRecv, 0x50);
        tSndEffTaskId = CreateTask(Task_RecordMixing_SoundEffect, 0x51);
        break;
    case 1:        // wait for Task_MixingRecordsRecv
        if (!gTasks[tLinkTaskId].isActive)
        {
            tState = 2;
            FlagSet(FLAG_SYS_MIX_RECORD);
            DestroyRecordMixingLights();
            DestroyTask(tSndEffTaskId);
        }
        break;
    case 2:
        tLinkTaskId = CreateTask(Task_DoRecordMixing, 10);
        tState = 3;
        PlaySE(SE_M_BATON_PASS);
        break;
    case 3:        // wait for Task_DoRecordMixing
        if (!gTasks[tLinkTaskId].isActive)
        {
            tState = 4;
            tLinkTaskId = CreateTask_ReestablishCableClubLink();
            Menu_ClearWindowText();
            Menu_PrintText(gOtherText_MixingComplete, 2, 15);
            tTimer = 0;
        }
        break;
    case 4:        // wait 60 frames
        tTimer++;
        if (tTimer > 60)
            tState = 5;
        break;
    case 5:
        if (!gTasks[tLinkTaskId].isActive)
        {
            SetLinkWaitingForScript();
            Menu_EraseScreen();
            DestroyTask(taskId);
            ScriptContext_Enable();
        }
        break;
    }
}

#undef tState
#undef tTimer
#undef tLinkTaskId
#undef tSndEffTaskId

#define tState             data[0]
#define tSentRecord        data[2]
#define tNumChunksSent     data[4]
#define tMultiplayerId     data[5]
#define tLinkPlayerCount   data[6]
#define tCopyTaskId        data[10]
#define tLinkStateDelay    data[12]
#define tParentTaskId      data[0]
#define tNumChunksRecv(i)  data[1 + (i)]
#define tRecvRecords       data[5]

void Task_MixingRecordsRecv(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        SetRecordStructSize();
        Menu_DisplayDialogueFrame();
        Menu_PrintText(gOtherText_MixingRecordsWithFriend, 2, 15);
        task->data[8] = 0x708;
        task->tState = 400;
        ClearLinkCallback_2();
        break;
    case 100:        // wait 20 frames
        task->tLinkStateDelay++;
        if (task->tLinkStateDelay > 20)
        {
            task->tLinkStateDelay = 0;
            task->tState = 101;
        }
        break;
    case 101:
        {
            u8 players = GetLinkPlayerCount_2();

            if (IsLinkMaster() == 1)
            {
                if (players == GetSavedPlayerCount())
                {
                    PlaySE(SE_PIN);
                    task->tState = 201;
                    task->tLinkStateDelay = 0;
                }
            }
            else
            {
                PlaySE(SE_BOO);
                task->tState = 301;
            }
        }
        break;
    case 201:
        if (GetSavedPlayerCount() == GetLinkPlayerCount_2())
        {
            if (++task->tLinkStateDelay > GetLinkPlayerCount_2() * 30)
            {
                CheckShouldAdvanceLinkState();
                task->tState = 1;
            }
        }
        break;
    case 301:
        if (GetSavedPlayerCount() == GetLinkPlayerCount_2())
            task->tState = 1;
        break;
    case 400:        // wait 20 frames
        task->tLinkStateDelay++;
        if (task->tLinkStateDelay > 20)
        {
            task->tState = 1;
            task->tLinkStateDelay = 0;
        }
        break;
    case 1:        // wait for handshake
        if (gReceivedRemoteLinkPlayers)
        {
            ConvertIntToDecimalStringN(gStringVar1, GetMultiplayerId_(), 2, 2);
            task->tState = 5;
        }
        break;
    case 2:
        {
            u8 subTaskId;

            task->tLinkPlayerCount = GetLinkPlayerCount_2();
            task->tState = 0;
            task->tMultiplayerId = GetMultiplayerId_();
            task->func = Task_SendPacket;
            StorePtrInTaskData(&eSentRecord, &task->tSentRecord);
            subTaskId = CreateTask(Task_CopyReceiveBuffer, 0x50);
            task->tCopyTaskId = subTaskId;
            gTasks[subTaskId].tParentTaskId = taskId;
            StorePtrInTaskData((u8 *)&eReceivedRecords, &gTasks[subTaskId].tRecvRecords);
        }
        break;
    case 5:        // wait 60 frames
        task->data[10]++;
        if (task->data[10] > 60)
        {
            task->data[10] = 0;
            task->tState = 2;
        }
        break;
    }
}

void Task_SendPacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    // Send the fixed-size record in 24 chunks.

    switch (task->tState)
    {
    case 0: //Copy record data to send buffer
        {
            void *recordData = (u8 *)LoadPtrFromTaskData(&task->tSentRecord) + BUFFER_CHUNK_SIZE * task->tNumChunksSent;

            memcpy(gBlockSendBuffer, recordData, BUFFER_CHUNK_SIZE);
            task->tState++;
        }
        break;
    case 1:
        if (GetMultiplayerId() == 0)
            SendBlockRequest(BLOCK_REQ_SIZE_200);
        task->tState++;
        break;
    case 2:
        break;
    case 3:
        task->tNumChunksSent++;
        if ((u16)task->tNumChunksSent == NUM_RECORD_MIXING_CHUNKS)
            task->tState++;
        else
            task->tState = 0;
        break;
    case 4:
        if (!gTasks[task->tCopyTaskId].isActive)
            task->func = Task_SendPacket_SwitchToReceive;
    }
}

void Task_CopyReceiveBuffer(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s32 recvStatus = GetBlockReceivedStatus();
    u8 handledPlayers = 0;

    if (recvStatus == GetSavedLinkPlayerCountAsBitFlags())
    {
        u8 player;

        for (player = 0; player < GetLinkPlayerCount(); player++)
        {
            void *src;
            u8 *dst;

            if ((recvStatus >> player) & 1)
            {
                dst = LoadPtrFromTaskData(&task->tRecvRecords) + task->tNumChunksRecv(player) * BUFFER_CHUNK_SIZE + player * sizeof(struct PlayerRecords);
                src = GetPlayerRecvBuffer(player);
                if ((task->tNumChunksRecv(player) + 1) * BUFFER_CHUNK_SIZE > sizeof(struct PlayerRecords))
                    memcpy(dst, src, sizeof(struct PlayerRecords) - task->tNumChunksRecv(player) * BUFFER_CHUNK_SIZE);
                else
                    memcpy(dst, src, BUFFER_CHUNK_SIZE);
                ResetBlockReceivedFlag(player);
                task->tNumChunksRecv(player)++;
                if ((u16)task->tNumChunksRecv(player) == NUM_RECORD_MIXING_CHUNKS)
                    handledPlayers++;
            }
        }
        gTasks[task->tParentTaskId].tState++;
    }
    if (handledPlayers == GetLinkPlayerCount())
        DestroyTask(taskId);
}

void Task_WaitReceivePacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (!gTasks[task->tCopyTaskId].isActive)
        DestroyTask(taskId);
}

void Task_ReceivePacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->func = Task_WaitReceivePacket;
    if (sReadyToReceive == TRUE)
        ReceiveExchangePacket(task->tMultiplayerId);
}

void Task_SendPacket_SwitchToReceive(u8 taskId)
{
    gTasks[taskId].func = Task_ReceivePacket;
    sReadyToReceive = TRUE;
}

#undef tState
#undef tSentRecord
#undef tNumChunksSent
#undef tMultiplayerId
#undef tLinkPlayerCount
#undef tCopyTaskId
#undef tLinkStateDelay
#undef tParentTaskId
#undef tNumChunksRecv
#undef tRecvRecords

void *LoadPtrFromTaskData(const u16 *taskData)
{
    return (void *)(taskData[0] | (taskData[1] << 16));
}

void StorePtrInTaskData(void *ptr, u16 *taskData)
{
    taskData[0] = (u32)ptr;
    taskData[1] = (u32)ptr >> 16;
}

u8 GetMultiplayerId_(void)
{
    return GetMultiplayerId();
}

u16 *GetPlayerRecvBuffer(u8 player)
{
    return gBlockRecvBuffer[player];
}

void SetRecordStructSize(void)
{
    gRecordStructSize = sizeof(struct PlayerRecords);
}

const u8 gPlayerIdxOrders_2Player[] = {1, 0};

const u8 gPlayerIdxOrders_3Player[][3] =
{
    {1, 2, 0},
    {2, 0, 1},
};

const u8 gPlayerIdxOrders_4Player[][4] =
{
    {1, 0, 3, 2},
    {3, 0, 1, 2},
    {2, 0, 3, 1},
    {1, 3, 0, 2},
    {2, 3, 0, 1},
    {3, 2, 0, 1},
    {1, 2, 3, 0},
    {2, 3, 1, 0},
    {3, 2, 1, 0},
};

void ShufflePlayerIndices(u8 *playerIndices)
{
    u32 i;
    u32 linkTrainerId;
    u32 players = GetLinkPlayerCount();

    switch (players)
    {
    case 2:
        for (i = 0; i < ARRAY_COUNT(gPlayerIdxOrders_2Player); i++)
            playerIndices[i] = gPlayerIdxOrders_2Player[i];
        break;
    case 3:
        linkTrainerId = GetLinkPlayerTrainerId(0) % ARRAY_COUNT(gPlayerIdxOrders_3Player);
        for (i = 0; i < ARRAY_COUNT(gPlayerIdxOrders_3Player[0]); i++)
            playerIndices[i] = gPlayerIdxOrders_3Player[linkTrainerId][i];
        break;
    case 4:
        linkTrainerId = GetLinkPlayerTrainerId(0) % ARRAY_COUNT(gPlayerIdxOrders_4Player);
        for (i = 0; i < ARRAY_COUNT(gPlayerIdxOrders_4Player[0]); i++)
            playerIndices[i] = gPlayerIdxOrders_4Player[linkTrainerId][i];
        break;
    default:
        break;
    }
}

void ReceiveOldManData(u8 *records, size_t recordSize, u8 multiplayerId)
{
    u8 playerIndices[MAX_LINK_PLAYERS];
    u8 *oldManSave;

    ShufflePlayerIndices(playerIndices);
    // This layout is not necessarily the original source structure, but it matches.
    memcpy(records + multiplayerId * recordSize, (oldManSave = gOldManSave), 0x40);
    memcpy(oldManSave, records + playerIndices[multiplayerId] * recordSize, 0x40);
    ResetMauvilleOldManFlag();
}

void ReceiveBattleTowerData(void *records, size_t recordSize, u8 multiplayerId)
{
    ShufflePlayerIndices(sBattleTowerPlayerIndices);
    memcpy(records + recordSize * multiplayerId, records + recordSize * sBattleTowerPlayerIndices[multiplayerId], sizeof(struct BattleTowerRecord));
    PutNewBattleTowerRecord(records + recordSize * multiplayerId);
}

u8 GetDaycareMailItemId(struct DayCareMail *mail)
{
    return mail->message.itemId;
}

void SwapDaycareMail(u8 *records, size_t recordSize, u8 (*idxs)[2], u8 playerSlot1, u8 playerSlot2)
{
    struct DayCareMail *mail1 = (struct DayCareMail *)(records + recordSize * idxs[playerSlot1][0]);
    struct DayCareMail *sourceMail = mail1 + idxs[playerSlot1][1];
    struct DayCareMail tempMail = *sourceMail;
    struct DayCareMail *mail2 = (struct DayCareMail *)(records + recordSize * idxs[playerSlot2][0]);

    mail1 += idxs[playerSlot1][1];
    *mail1 = *(mail2 + idxs[playerSlot2][1]);

    mail2 += idxs[playerSlot2][1];
    *mail2 = tempMail;
}

u8 CalculateDaycareMailRandSum(const u8 *shows)
{
    int i;
    u8 sum = 0;

    for (i = 0; i < 0x100; i++)
        sum += shows[i];
    return sum;
}

const u8 gDaycareMailSwapIds_3Player[NUM_SWAP_COMBOS][2] =
{
    {0, 1},
    {1, 2},
    {2, 0},
};

const u8 gDaycareMailSwapIds_4Player[NUM_SWAP_COMBOS][4] =
{
    {0, 1, 2, 3},
    {0, 2, 1, 3},
    {0, 3, 2, 1},
};

void ReceiveDaycareMailData(struct RecordMixingDayCareMail *records, size_t recordSize, u8 multiplayerId, TVShow *shows)
{
    {
        u16 i, j;
        u8 linkPlayerCount;
        u8 tableId;
        struct RecordMixingDayCareMail *mixMail;
        u8 playerSlot1, playerSlot2;
        void *idxsPtr;
        u8 unusedArr1[MAX_LINK_PLAYERS];
        u8 unusedArr2[MAX_LINK_PLAYERS];
        struct RecordMixingDayCareMail *unusedMixMail[MAX_LINK_PLAYERS];
        bool8 canHoldItem[MAX_LINK_PLAYERS][2];
        u8 idxs[MAX_LINK_PLAYERS][2];
        u8 numDaycareCanHold;
        u16 oldSeed;

        oldSeed = Random();
        SeedRng(gLinkPlayers[0].trainerId);
        linkPlayerCount = GetLinkPlayerCount();
        for (i = 0; i < MAX_LINK_PLAYERS; i++)
        {
            unusedArr1[i] = 0xFF;
            unusedArr2[i] = 0;
            canHoldItem[i][0] = FALSE;
            canHoldItem[i][1] = FALSE;
        }
        numDaycareCanHold = 0;
        for (i = 0; i < linkPlayerCount; i++)
        {
            mixMail = (void *)records + recordSize * i;
            if (mixMail->numDaycareMons == 0)
                continue;

            for (j = 0; j < mixMail->numDaycareMons; j++)
            {
                if (!mixMail->itemsHeld[j])
                    canHoldItem[i][j] = TRUE;
            }
        }

        j = 0;
        for (i = 0; i < linkPlayerCount; i++)
        {
            mixMail = (void *)records + recordSize * i;
            if (canHoldItem[i][0] == TRUE || canHoldItem[i][1] == TRUE)
                numDaycareCanHold++;

            if (canHoldItem[i][0] == TRUE && canHoldItem[i][1] == FALSE)
            {
                idxs[j][0] = i;
                idxs[j][1] = 0;
                j++;
            }
            else if (canHoldItem[i][0] == FALSE && canHoldItem[i][1] == TRUE)
            {
                idxs[j][0] = i;
                idxs[j][1] = 1;
                j++;
            }
            else if (canHoldItem[i][0] == TRUE && canHoldItem[i][1] == TRUE)
            {
                u32 itemId1, itemId2;

                idxs[j][0] = i;
                itemId1 = GetDaycareMailItemId(&mixMail->mail[0]);
                itemId2 = GetDaycareMailItemId(&mixMail->mail[1]);
                if (!itemId1 && itemId2)
                {
                    register u8 one asm("r0") = 1; // boo, a fakematch
                    idxs[j][1] = one;
                }
                else if ((itemId1 && itemId2) || (!itemId1 && !itemId2))
                {
                    idxs[j][1] = Random() % 2;
                }
                else if (itemId1 && !itemId2)
                {
                    idxs[j][1] = 0;
                }
                j++;
            }
        }

        for (i = 0; i < MAX_LINK_PLAYERS; i++)
        {
            mixMail = &records[recordSize * multiplayerId];
            unusedMixMail[i] = mixMail;
        }

        tableId = CalculateDaycareMailRandSum((void *)shows) % NUM_SWAP_COMBOS;
        switch (numDaycareCanHold)
        {
        case 2:
            SwapDaycareMail((void *)records, recordSize, idxs, 0, 1);
            break;
        case 3:
            playerSlot1 = gDaycareMailSwapIds_3Player[tableId][0];
            playerSlot2 = gDaycareMailSwapIds_3Player[tableId][1];
            SwapDaycareMail((void *)records, recordSize, idxs, playerSlot1, playerSlot2);
            break;
        case 4:
            idxsPtr = idxs;
            playerSlot1 = gDaycareMailSwapIds_4Player[tableId][0];
            playerSlot2 = gDaycareMailSwapIds_4Player[tableId][1];
            SwapDaycareMail((void *)records, recordSize, idxsPtr, playerSlot1, playerSlot2);
            playerSlot1 = gDaycareMailSwapIds_4Player[tableId][2];
            playerSlot2 = gDaycareMailSwapIds_4Player[tableId][3];
            SwapDaycareMail((void *)records, recordSize, idxsPtr, playerSlot1, playerSlot2);
            break;
        }

        mixMail = (void *)records + multiplayerId * recordSize;
        gSaveBlock1.daycare.misc.mail[0] = mixMail->mail[0];
        gSaveBlock1.daycare.misc.mail[1] = mixMail->mail[1];
        //memcpy(&gSaveBlock1.daycare.misc.mail[0], &mixMail->mail[0], sizeof(struct DayCareMail));
        //memcpy(&gSaveBlock1.daycare.misc.mail[1], &mixMail->mail[1], sizeof(struct DayCareMail));
        SeedRng(oldSeed);
    }
}

void ReceiveGiftItem(u16 *item, u8 multiplayerId)
{
    if (multiplayerId != 0 && *item != 0)
    {
        if (GetPocketByItemId(*item) == POCKET_KEY_ITEMS)
        {
            if (!CheckBagHasItem(*item, 1) && !CheckPCHasItem(*item, 1) && AddBagItem(*item, 1))
            {
                VarSet(VAR_TEMP_RECORD_MIX_GIFT_ITEM, *item);
                StringCopy(gStringVar1, gLinkPlayers[0].name);
                if (*item == ITEM_EON_TICKET)
                    FlagSet(FLAG_SYS_HAS_EON_TICKET);
            }
            else
            {
                VarSet(VAR_TEMP_RECORD_MIX_GIFT_ITEM, ITEM_NONE);
            }
        }
        else
        {
            if (AddBagItem(*item, 1) == TRUE)
            {
                VarSet(VAR_TEMP_RECORD_MIX_GIFT_ITEM, *item);
                StringCopy(gStringVar1, gLinkPlayers[0].name);
            }
            else
            {
                VarSet(VAR_TEMP_RECORD_MIX_GIFT_ITEM, ITEM_NONE);
            }
        }
    }
}

#define tState data[0]
#define tDelay data[1]

void Task_DoRecordMixing(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        task->tState++;
        break;
    case 1:
        task->tState++;
        break;
    case 2:
        SetContinueGameWarpStatusToDynamicWarp();
        WriteSaveBlock2();
        task->tState++;
        break;
    case 3:
        if (WriteSaveBlock1Sector() != 0)
        {
            ClearContinueGameWarpStatus2();
            task->tState++;
            task->tDelay = 0;
        }
        break;
    case 4:
        task->tDelay++;
        if (task->tDelay > 10)
        {
            SetCloseLinkCallback();
            task->tState++;
        }
        break;
    case 5:
        if (!gReceivedRemoteLinkPlayers)
            DestroyTask(taskId);
        break;
    }
}

#undef tState
#undef tDelay
