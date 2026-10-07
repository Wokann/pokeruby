#include "global.h"
#include <stddef.h>
#include "record_mixing.h"
#include "battle_tower.h"
#include "cable_club.h"
#include "daycare.h"
#include "dewford_trend.h"
#include "event_data.h"
#include "fldeff_misc.h"
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
#include "strings.h"
#include "task.h"
#include "tv.h"
#include "ewram.h"

static EWRAM_DATA struct RecordMixingDaycareMail sRecordMixMail = {0};

u32 gRecordStructSize;

static bool8 sReadyToReceive;
static u8 gUnknown_0300071C[4];

static struct SecretBase *sSecretBasesSave = gSaveBlock1.secretBases;
static TVShow *sTvShowsSave = gSaveBlock1.tvShows;
static struct PokeNews *sPokeNewsSave = gSaveBlock1.pokeNews;
static void *recordMixingMauvilleMan = &gSaveBlock1.oldMan;
static struct DewfordTrend *sDewfordTrendsSave = gSaveBlock1.dewfordTrends;
static struct RecordMixingDaycareMail *sRecordMixMailSave = &sRecordMixMail;
static struct BattleTowerRecord *sBattleTowerSave = &gSaveBlock2.battleTower.playerRecord;

#define BUFFER_CHUNK_SIZE 200

static void PrepareExchangePacket(void);
static void ReceiveExchangePacket(u32 multiplayerId);
static void Task_RecordMixing_SoundEffect(u8 taskId);
static void Task_MixingRecordsRecv(u8 taskId);
static void Task_SendPacket(u8 taskId);
static void Task_CopyReceiveBuffer(u8 taskId);
static void Task_WaitReceivePacket(u8 taskId);
static void Task_ReceivePacket(u8 taskId);
static void Task_SendPacket_SwitchToReceive(u8 taskId);
static void *LoadPtrFromTaskData(const u16 *taskData);
static void StorePtrInTaskData(void *records, u16 *taskData);
static u8 GetMultiplayerId_(void);
static u16 *GetPlayerRecvBuffer(u8 player);
void sub_80B9A78(void);
static void ShufflePlayerIndices(u8 *data);
static void ReceiveOldManData(u8 *records, size_t recordSize, u8 multiplayerId);
static void ReceiveBattleTowerData(void *records, u32 recordSize, u8 multiplayerId);
static u8 GetDaycareMailItemId(struct DaycareMail *mail);
static void SwapDaycareMail(u8 *records, size_t recordSize, u8 idxs[][2], u8 playerSlot1, u8 playerSlot2);
static u8 CalculateDaycareMailRandSum(const u8 *src);
static void ReceiveDaycareMailData(struct RecordMixingDaycareMail *records, size_t recordSize, u8 multiplayerId, TVShow *shows);
static void ReceiveGiftItem(u16 *item, u8 multiplayerId);
static void Task_DoRecordMixing(u8 taskId);

void RecordMixingPlayerSpotTriggered(void)
{
    CreateTask_EnterCableClubSeat(Task_RecordMixing_Main);
}

struct PlayerRecordRS
{
    struct SecretBase secretBases[SECRET_BASES_COUNT];
    TVShow tvShows[TV_SHOWS_COUNT];
    struct PokeNews pokeNews[POKE_NEWS_COUNT];
    union OldMan oldMan;
    struct DewfordTrend dewfordTrends[5];
    struct RecordMixingDaycareMail daycareMail;
    struct BattleTowerRecord battleTowerRecord;
    u16 giftItem;
    u16 filler11CA[0x33];
};

#define eReceivedRecords                (*(struct PlayerRecordRS *)(gSharedMem + 0x08000))
#define eSentRecord                     (*(struct PlayerRecordRS *)(gSharedMem + 0x18000))

static void PrepareExchangePacket(void)
{
    SetPlayerSecretBaseParty();
    DeactivateAllNormalTVShows();

    memcpy(eSentRecord.secretBases, sSecretBasesSave, sizeof(eSentRecord.secretBases));
    memcpy(eSentRecord.tvShows, sTvShowsSave, sizeof(eSentRecord.tvShows));
    memcpy(eSentRecord.pokeNews, sPokeNewsSave, sizeof(eSentRecord.pokeNews));
    memcpy(&eSentRecord.oldMan, recordMixingMauvilleMan, sizeof(eSentRecord.oldMan));
    memcpy(eSentRecord.dewfordTrends, sDewfordTrendsSave, sizeof(eSentRecord.dewfordTrends));
    sRecordMixMail.mail[0] = gSaveBlock1.daycare.misc.mail[0];
    sRecordMixMail.mail[1] = gSaveBlock1.daycare.misc.mail[1];
    InitDaycareMailRecordMixing(gSaveBlock1.daycare.mons, &sRecordMixMail);
    memcpy(&eSentRecord.daycareMail, sRecordMixMailSave, sizeof(struct RecordMixingDaycareMail));
    memcpy(&eSentRecord.battleTowerRecord, sBattleTowerSave, sizeof(struct BattleTowerRecord));

    if (GetMultiplayerId() == 0)
        eSentRecord.giftItem = GetRecordMixingGift();
}

static void ReceiveExchangePacket(u32 multiplayerId)
{
    ReceiveSecretBasesData(eReceivedRecords.secretBases, sizeof(struct PlayerRecordRS), multiplayerId);
    ReceiveTvShowsData((u8 *)eReceivedRecords.tvShows, sizeof(struct PlayerRecordRS), multiplayerId);
    ReceivePokeNewsData(eReceivedRecords.pokeNews, sizeof(struct PlayerRecordRS), multiplayerId);
    ReceiveOldManData((u8 *)&eReceivedRecords.oldMan, sizeof(struct PlayerRecordRS), multiplayerId);
    ReceiveDewfordTrendData(eReceivedRecords.dewfordTrends, sizeof(struct PlayerRecordRS), multiplayerId);
    ReceiveDaycareMailData(&eReceivedRecords.daycareMail,
        sizeof(struct PlayerRecordRS),
        multiplayerId,
        eReceivedRecords.tvShows);
    ReceiveBattleTowerData(&eReceivedRecords.battleTowerRecord, sizeof(struct PlayerRecordRS), multiplayerId);
    ReceiveGiftItem(&eReceivedRecords.giftItem, multiplayerId);
}

#define tCounter data[0]

static void Task_RecordMixing_SoundEffect(u8 taskId)
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
#define tSoundTaskId  data[15]

void Task_RecordMixing_Main(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:        // init
        SetLocalLinkPlayerId(gSpecialVar_0x8005);
        VarSet(VAR_TEMP_0, 1);
        sReadyToReceive = 0;
        PrepareExchangePacket();
        CreateRecordMixingLights();
        tState = 1;
        data[10] = CreateTask(Task_MixingRecordsRecv, 0x50);
        tSoundTaskId = CreateTask(Task_RecordMixing_SoundEffect, 0x51);
        break;
    case 1:        // wait for Task_MixingRecordsRecv
        if (!gTasks[data[10]].isActive)
        {
            tState = 2;
            FlagSet(FLAG_SYS_MIX_RECORD);
            DestroyRecordMixingLights();
            DestroyTask(tSoundTaskId);
        }
        break;
    case 2:
        data[10] = CreateTask(Task_DoRecordMixing, 10);
        tState = 3;
        PlaySE(SE_M_BATON_PASS);
        break;
    case 3:        // wait for Task_DoRecordMixing
        if (!gTasks[data[10]].isActive)
        {
            tState = 4;
            data[10] = CreateTask_ReestablishCableClubLink();
            Menu_ClearWindowText();
            Menu_PrintText(gText_RecordMixingComplete, 2, 15);
            data[8] = 0;
        }
        break;
    case 4:        // wait 60 frames
        data[8]++;
        if (data[8] > 60)
            tState = 5;
        break;
    case 5:
        if (!gTasks[data[10]].isActive)
        {
            SetLinkWaitingForScript();
            Menu_EraseScreen();
            DestroyTask(taskId);
            ScriptContext_Enable();
        }
        break;
    }
}

static void Task_MixingRecordsRecv(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        sub_80B9A78();
        Menu_DisplayDialogueFrame();
        Menu_PrintText(gText_MixingRecords, 2, 15);
        task->data[8] = 0x708;
        task->tState = 400;
        ClearLinkCallback_2();
        break;
    case 100:        // wait 20 frames
        task->data[12]++;
        if (task->data[12] > 20)
        {
            task->data[12] = 0;
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
                    task->data[12] = 0;
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
            if (++task->data[12] > GetLinkPlayerCount_2() * 30)
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
        task->data[12]++;
        if (task->data[12] > 20)
        {
            task->tState = 1;
            task->data[12] = 0;
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

            task->data[6] = GetLinkPlayerCount_2();
            task->tState = 0;
            task->data[5] = GetMultiplayerId_();
            task->func = Task_SendPacket;
            StorePtrInTaskData(&eSentRecord, &task->data[2]);
            subTaskId = CreateTask(Task_CopyReceiveBuffer, 0x50);
            task->data[10] = subTaskId;
            gTasks[subTaskId].data[0] = taskId;
            StorePtrInTaskData((u8 *)&eReceivedRecords, &gTasks[subTaskId].data[5]);
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

static void Task_SendPacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    // does this send the data 24 times?

    switch (task->tState)
    {
    case 0: //Copy record data to send buffer
        {
            void *recordData = (u8 *)LoadPtrFromTaskData(&task->data[2]) + BUFFER_CHUNK_SIZE * task->data[4];

            memcpy(gBlockSendBuffer, recordData, BUFFER_CHUNK_SIZE);
            task->tState++;
        }
        break;
    case 1:
        if (GetMultiplayerId() == 0)
            SendBlockRequest(1);
        task->tState++;
        break;
    case 2:
        break;
    case 3:
        task->data[4]++;
        if ((u16)task->data[4] == 24)
            task->tState++;
        else
            task->tState = 0;
        break;
    case 4:
        if (!gTasks[task->data[10]].isActive)
            task->func = Task_SendPacket_SwitchToReceive;
    }
}

static void Task_CopyReceiveBuffer(u8 taskId)
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
            u8 *dest;

            if ((recvStatus >> player) & 1)
            {
                dest = LoadPtrFromTaskData(&task->data[5]) + task->data[player + 1] * BUFFER_CHUNK_SIZE + player * sizeof(struct PlayerRecordRS);
                src = GetPlayerRecvBuffer(player);
                if ((task->data[player + 1] + 1) * BUFFER_CHUNK_SIZE > sizeof(struct PlayerRecordRS))
                    memcpy(dest, src, sizeof(struct PlayerRecordRS) - task->data[player + 1] * BUFFER_CHUNK_SIZE);
                else
                    memcpy(dest, src, BUFFER_CHUNK_SIZE);
                ResetBlockReceivedFlag(player);
                task->data[player + 1]++;
                if ((u16)task->data[player + 1] == 0x18)
                    handledPlayers++;
            }
        }
        gTasks[task->data[0]].data[0]++;
    }
    if (handledPlayers == GetLinkPlayerCount())
        DestroyTask(taskId);
}

static void Task_WaitReceivePacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (!gTasks[task->data[10]].isActive)
        DestroyTask(taskId);
}

static void Task_ReceivePacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->func = Task_WaitReceivePacket;
    if (sReadyToReceive == 1)
        ReceiveExchangePacket(task->data[5]);
}

static void Task_SendPacket_SwitchToReceive(u8 taskId)
{
    gTasks[taskId].func = Task_ReceivePacket;
    sReadyToReceive = 1;
}

static void *LoadPtrFromTaskData(const u16 *taskData)
{
    return (void *)(taskData[0] | (taskData[1] << 16));
}

static void StorePtrInTaskData(void *records, u16 *taskData)
{
    taskData[0] = (u32)records;
    taskData[1] = (u32)records >> 16;
}

static u8 GetMultiplayerId_(void)
{
    return GetMultiplayerId();
}

static u16 *GetPlayerRecvBuffer(u8 player)
{
    return gBlockRecvBuffer[player];
}

void sub_80B9A78(void)
{
    gRecordStructSize = sizeof(struct PlayerRecordRS);
}

static const u8 sPlayerIdxOrders_2Player[2] = {1, 0};

static const u8 sPlayerIdxOrders_3Player[2][3] =
{
    {1, 2, 0},
    {2, 0, 1},
};

static const u8 sPlayerIdxOrders_4Player[9][4] =
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

static void ShufflePlayerIndices(u8 *data)
{
    u32 i;
    u32 linkTrainerId;
    u32 players = GetLinkPlayerCount();

    switch (players)
    {
    case 2:
        for (i = 0; i < 2; i++)
            data[i] = sPlayerIdxOrders_2Player[i];
        break;
    case 3:
        linkTrainerId = GetLinkPlayerTrainerId(0) % 2;
        for (i = 0; i < 3; i++)
            data[i] = sPlayerIdxOrders_3Player[linkTrainerId][i];
        break;
    case 4:
        linkTrainerId = GetLinkPlayerTrainerId(0) % 9;
        for (i = 0; i < 4; i++)
            data[i] = sPlayerIdxOrders_4Player[linkTrainerId][i];
        break;
    default:
        break;
    }
}

static void ReceiveOldManData(u8 *records, size_t recordSize, u8 multiplayerId)
{
    u8 mixIndices[4];
    u8 *ptr;

    ShufflePlayerIndices(mixIndices);
    //Probably not how it was originally written, but this matches.
    memcpy(records + multiplayerId * recordSize, (ptr = recordMixingMauvilleMan), 0x40);
    memcpy(ptr, records + mixIndices[multiplayerId] * recordSize, 0x40);
    ResetMauvilleOldManFlag();
}

static void ReceiveBattleTowerData(void *records, u32 recordSize, u8 multiplayerId)
{
    ShufflePlayerIndices(gUnknown_0300071C);
    memcpy(records + recordSize * multiplayerId, records + recordSize * gUnknown_0300071C[multiplayerId], sizeof(struct BattleTowerRecord));
    PutNewBattleTowerRecord(records + recordSize * multiplayerId);
}

static u8 GetDaycareMailItemId(struct DaycareMail *mail)
{
    return mail->message.itemId;
}

static void SwapDaycareMail(u8 *records, size_t recordSize, u8 idxs[][2], u8 playerSlot1, u8 playerSlot2)
{
    struct DaycareMail *r6 = (struct DaycareMail *)(records + recordSize * idxs[playerSlot1][0]);
    struct DaycareMail *src = r6 + idxs[playerSlot1][1];
    struct DaycareMail temp = *src;
    struct DaycareMail *r8 = (struct DaycareMail *)(records + recordSize * idxs[playerSlot2][0]);

    r6 += idxs[playerSlot1][1];
    *r6 = *(r8 + idxs[playerSlot2][1]);

    r8 += idxs[playerSlot2][1];
    *r8 = temp;
}

static u8 CalculateDaycareMailRandSum(const u8 *src)
{
    int i;
    u8 sum = 0;

    for (i = 0; i < 0x100; i++)
        sum += src[i];
    return sum;
}

static const u8 sDaycareMailSwapIds_3Player[][2] =
{
    {0, 1},
    {1, 2},
    {2, 0},
};

static const u8 sDaycareMailSwapIds_4Player[3][4] =
{
    {0, 1, 2, 3},
    {0, 2, 1, 3},
    {0, 3, 2, 1},
};

static void ReceiveDaycareMailData(struct RecordMixingDaycareMail *records, size_t recordSize, u8 multiplayerId, TVShow *shows)
{
    {
        u16 i, j;
        u8 linkPlayerCount;
        u8 tableId;
        struct RecordMixingDaycareMail *mixMail;
        u8 playerSlot1, playerSlot2;
        void *ptr;
        u8 unusedArr1[4];
        u8 unusedArr2[4];
        struct RecordMixingDaycareMail *unusedMixMail[4];
        bool8 canHoldItem[4][2];
        u8 idxs[4][2];
        u8 numDaycareCanHold;
        u16 oldSeed;

        oldSeed = Random();
        SeedRng(gLinkPlayers[0].trainerId);
        linkPlayerCount = GetLinkPlayerCount();
        for (i = 0; i < 4; i++)
        {
            unusedArr1[i] = 0xFF;
            unusedArr2[i] = 0;
            canHoldItem[i][0] = 0;
            canHoldItem[i][1] = 0;
        }
        numDaycareCanHold = 0;
        for (i = 0; i < linkPlayerCount; i++)
        {
            mixMail = (void *)records + recordSize * i;
            if (mixMail->numDaycareMons == 0)
                continue;

            for (j = 0; j < mixMail->numDaycareMons; j ++)
            {
                if (!mixMail->cantHoldItem[j])
                    canHoldItem[i][j] = 1;
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

        for (i = 0; i < 4; i++)
        {
            mixMail = &records[recordSize * multiplayerId];
            unusedMixMail[i] = mixMail;
        }

        tableId = CalculateDaycareMailRandSum((void *)shows) % 3;
        switch (numDaycareCanHold)
        {
        case 2:
            SwapDaycareMail((void *)records, recordSize, idxs, 0, 1);
            break;
        case 3:
            playerSlot1 = sDaycareMailSwapIds_3Player[tableId][0];
            playerSlot2 = sDaycareMailSwapIds_3Player[tableId][1];
            SwapDaycareMail((void *)records, recordSize, idxs, playerSlot1, playerSlot2);
            break;
        case 4:
            ptr = idxs;
            playerSlot1 = sDaycareMailSwapIds_4Player[tableId][0];
            playerSlot2 = sDaycareMailSwapIds_4Player[tableId][1];
            SwapDaycareMail((void *)records, recordSize, ptr, playerSlot1, playerSlot2);
            playerSlot1 = sDaycareMailSwapIds_4Player[tableId][2];
            playerSlot2 = sDaycareMailSwapIds_4Player[tableId][3];
            SwapDaycareMail((void *)records, recordSize, ptr, playerSlot1, playerSlot2);
            break;
        }

        mixMail = (void *)records + multiplayerId * recordSize;
        gSaveBlock1.daycare.misc.mail[0] = mixMail->mail[0];
        gSaveBlock1.daycare.misc.mail[1] = mixMail->mail[1];
        //memcpy(&gSaveBlock1.daycare.misc.mail[0], &mixMail->mail[0], sizeof(struct DaycareMail));
        //memcpy(&gSaveBlock1.daycare.misc.mail[1], &mixMail->mail[1], sizeof(struct DaycareMail));
        SeedRng(oldSeed);
    }
}

static void ReceiveGiftItem(u16 *item, u8 multiplayerId)
{
    if (multiplayerId != 0 && *item != 0)
    {
        if (GetPocketByItemId(*item) == POCKET_KEY_ITEMS)
        {
            if (!CheckBagHasItem(*item, 1) && !CheckPCHasItem(*item, 1) && AddBagItem(*item, 1))
            {
                VarSet(VAR_TEMP_1, *item);
                StringCopy(gStringVar1, gLinkPlayers[0].name);
                if (*item == ITEM_EON_TICKET)
                    FlagSet(FLAG_SYS_HAS_EON_TICKET);
            }
            else
            {
                VarSet(VAR_TEMP_1, ITEM_NONE);
            }
        }
        else
        {
            if (AddBagItem(*item, 1) == TRUE)
            {
                VarSet(VAR_TEMP_1, *item);
                StringCopy(gStringVar1, gLinkPlayers[0].name);
            }
            else
            {
                VarSet(VAR_TEMP_1, ITEM_NONE);
            }
        }
    }
}

static void Task_DoRecordMixing(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        task->data[0]++;
        break;
    case 1:
        task->data[0]++;
        break;
    case 2:
        SetSecretBase2Field_9_AndHideBG();
        sub_8125E2C();
        task->data[0]++;
        break;
    case 3:
        if (sub_8125E6C() != 0)
        {
            ClearSecretBase2Field_9_2();
            task->data[0]++;
            task->data[1] = 0;
        }
        break;
    case 4:
        task->data[1]++;
        if (task->data[1] > 10)
        {
            SetCloseLinkCallback();
            task->data[0]++;
        }
        break;
    case 5:
        if (!gReceivedRemoteLinkPlayers)
            DestroyTask(taskId);
        break;
    }
}
