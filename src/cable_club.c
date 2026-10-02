#include "global.h"
#include "cable_club.h"
#include "battle.h"
#include "battle_records.h"
#include "constants/maps.h"
#include "constants/songs.h"
#include "contest_util.h"
#include "event_data.h"
#include "field_message_box.h"
#include "field_specials.h"
#include "field_weather.h"
#include "link.h"
#include "load_save.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "record_mixing.h"
#include "script.h"
#include "sound.h"
#include "start_menu.h"
#include "string_util.h"
#include "strings2.h"
#include "task.h"
#include "text.h"
#include "trade.h"
#include "trainer_card.h"

extern u16 gBattleTypeFlags;
extern const u8 gText_ConfirmLinkWhenPlayersReady[];
extern const u8 gText_ConfirmStartLinkWithXPlayers[];
extern const u8 gText_AwaitingLinkup[];
extern const u8 gText_PleaseWaitForLink[];
extern u8 gSelectedTradeMonPositions[2];

static void Task_LinkupStart(u8 taskId);
static void Task_LinkupAwaitConnection(u8 taskId);
static void Task_LinkupConfirmWhenReady(u8 taskId);
static void Task_LinkupAwaitConfirmation(u8 taskId);
static void Task_LinkupTryConfirmation(u8 taskId);
static void Task_LinkupConfirm(u8 taskId);
static void Task_LinkupExchangeDataWithLeader(u8 taskId);
static void Task_LinkupCheckStatusAfterConfirm(u8 taskId);
static void Task_LinkupAwaitTrainerCardData(u8 taskId);
static void Task_StopLinkup(u8 taskId);
static void Task_LinkupFailed(u8 taskId);
static void Task_LinkupConnectionError(u8 taskId);
static bool8 TryLinkTimeout(u8 taskId);
static void Task_ValidateMixingGameLanguage(u8 taskId);
static void Task_ReestablishLink(u8 taskId);
static void Task_ReestablishLinkAwaitConnection(u8 taskId);
static void Task_ReestablishLinkLeader(u8 taskId);
static void Task_ReestablishLinkAwaitConfirmation(u8 taskId);
static void Task_StartWiredCableClubBattle(u8 taskId);
static void CB2_ReturnFromCableClubBattle(void);
static void Task_EnterCableClubSeat(u8 taskId);
static void Task_StartWiredTrade(u8 taskId);
static void Task_StartWiredTradeFromSeat(u8 taskId);
static void CreateTask_StartWiredTrade(void);
static void Task_WaitExitToScript(u8 taskId);
#if DEBUG
static u8 Debug_GetLinkStateFlags(void);
#endif

#define tState      data[0]
#define tMinPlayers data[1]
#define tMaxPlayers data[2]
#define tNumPlayers data[3]
#define tTimer      data[4]

#ifdef GERMAN
static const u8 sTrainerCardColorName_Bronze[] = _("BRONZE");
static const u8 sTrainerCardColorName_Copper[] = _("KUPFER");
static const u8 sTrainerCardColorName_Silver[] = _("SILBER");
static const u8 sTrainerCardColorName_Gold[] = _("GOLD");
#else
static const u8 sTrainerCardColorName_Bronze[] = _("BRONZE");
static const u8 sTrainerCardColorName_Copper[] = _("COPPER");
static const u8 sTrainerCardColorName_Silver[] = _("SILVER");
static const u8 sTrainerCardColorName_Gold[] = _("GOLD");
#endif

static const u8 *const sTrainerCardColorNames[] =
{
    sTrainerCardColorName_Bronze,
    sTrainerCardColorName_Copper,
    sTrainerCardColorName_Silver,
    sTrainerCardColorName_Gold,
};

#if DEBUG
u8 Debug_GetLinkupTaskState(TaskFunc func)
{
    if (func == Task_LinkupAwaitConnection)
        return 1;
    if (func == Task_LinkupConfirmWhenReady)
        return 17;
    if (func == Task_LinkupAwaitConfirmation)
        return 18;
    if (func == Task_LinkupConfirm)
        return 19;
    if (func == Task_LinkupTryConfirmation)
        return 20;
    if (func == Task_LinkupExchangeDataWithLeader)
        return 33;
    if (func == Task_LinkupAwaitTrainerCardData)
        return 2;
    if (func == Task_StopLinkup)
        return 3;
    return 0;
}

void Task_DebugDisplayLinkTest(u8 taskId)
{
    s32 i;
    
    if (!gTasks[gTasks[taskId].data[0]].isActive)
    {
        if (gTasks[taskId].data[1] == 5)
            DestroyTask(taskId);
        gTasks[taskId].data[1]++;
    }

    LinkTest_PrintHex(gShouldAdvanceLinkState, 2, 0, 2);
    LinkTest_PrintHex((u8)gBlockSendBuffer[0], 22, 5, 4);
    for (i = 0; i < 4; i++)
    {
        LinkTest_PrintHex(gLinkPlayerPending[i], 5 + i * 2, 0, 1);
        LinkTest_PrintHex(gBlockRecvBuffer[i][0], 22, 6 + i, 4);
    }
    LinkTest_PrintHex(gLinkStatus, 15, 0, 8);
    LinkTest_PrintHex(gLink.state, 2, 10, 2);
    LinkTest_PrintHex(GetMultiplayerId(), 7, 12, 2);
    LinkTest_PrintHex(GetBlockReceivedStatus(), 7, 10, 2);
    LinkTest_PrintHex(gReceivedRemoteLinkPlayers, 2, 12, 1);
    LinkTest_PrintHex(gSpecialVar_Result, 11, 8, 2);
    LinkTest_PrintHex((gLinkStatus & LINK_STAT_PLAYER_COUNT) >> LINK_STAT_PLAYER_COUNT_SHIFT, 11, 10, 2);
    LinkTest_PrintHex(IsLinkConnectionEstablished(), 11, 12, 1);
    LinkTest_PrintHex(IsLinkTaskFinished(), 15, 10, 1);
    LinkTest_PrintHex(Debug_GetLinkupTaskState(gTasks[gTasks[taskId].data[0]].func), 15, 12, 2);
    LinkTest_PrintHex((uintptr_t)gLinkCallback, 2, 13, 8);
    LinkTest_PrintHex(HasLinkErrorOccurred(), 2, 2, 1);
    for (i = 0; i < 4; i++)
        LinkTest_PrintHex(gLinkPlayers[i].linkType, 2 + i * 6, 3, 4);
    LinkTest_PrintHex(REG_SIOCNT, 2, 6, 4);
    LinkTest_PrintHex(Debug_GetLinkStateFlags(), 25, 3, 1);
}
#endif

static void CreateLinkupTask(u8 minPlayers, u8 maxPlayers)
{
#if DEBUG
    LoadLinkTestBgGfx(12, 0, 31, 2);
#endif
    if (FindTaskIdByFunc(Task_LinkupStart) == TASK_NONE)
    {
        u8 linkupTaskId;
#if DEBUG
        u8 debugTaskId;
#endif

        linkupTaskId = CreateTask(Task_LinkupStart, 80);
        gTasks[linkupTaskId].tMinPlayers = minPlayers;
        gTasks[linkupTaskId].tMaxPlayers = maxPlayers;

#if DEBUG
        debugTaskId = CreateTask(Task_DebugDisplayLinkTest, 80);
        gTasks[debugTaskId].data[0] = linkupTaskId;
#endif
    }
}

static void PrintNumPlayersInLink(u32 numPlayers)
{
    ConvertIntToDecimalStringN(gStringVar1, numPlayers, STR_CONV_MODE_LEFT_ALIGN, 1);
    Menu_DrawStdWindowFrame(18, 10, 28, 13);
    MenuPrint_Centered(gText_NumPlayerLink, 19, 11, 72);
}

static void ClearLinkPlayerCountWindow()
{
    Menu_EraseWindowRect(18, 10, 28, 13);
}

static void UpdateLinkPlayerCountDisplay(u8 taskId, u8 numPlayers)
{
    s16 *taskData = &gTasks[taskId].tNumPlayers;

    if (numPlayers != *taskData)
    {
        if (numPlayers <= 1)
            ClearLinkPlayerCountWindow();
        else
            PrintNumPlayersInLink(numPlayers);
        *taskData = numPlayers;
    }
}

static u32 ExchangeDataAndGetLinkupStatus(u8 minPlayers, u8 maxPlayers)
{
    int playerCount;

    switch (GetLinkPlayerDataExchangeStatusTimed())
    {
    case EXCHANGE_COMPLETE:
        playerCount = GetLinkPlayerCount_2();
        if (minPlayers <= playerCount && playerCount <= maxPlayers)
            return LINKUP_SUCCESS;
        ConvertIntToDecimalStringN(gStringVar1, playerCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        return LINKUP_WRONG_NUM_PLAYERS;
    case EXCHANGE_TIMED_OUT:
        return LINKUP_ONGOING;
    case EXCHANGE_IN_PROGRESS:
        return LINKUP_DIFF_SELECTIONS;
    default:
        return LINKUP_ONGOING;
    }
}

static bool32 CheckLinkErrored(u8 taskId)
{
    if (HasLinkErrorOccurred() == TRUE)
    {
        gTasks[taskId].func = Task_LinkupConnectionError;
        return TRUE;
    }
    return FALSE;
}

static bool32 CheckLinkCanceledBeforeConnection(u8 taskId)
{
    if (JOY_NEW(B_BUTTON)
     && IsLinkConnectionEstablished() == FALSE)
    {
        gTasks[taskId].func = Task_LinkupFailed;
        return TRUE;
    }
    return FALSE;
}

static bool32 CheckLinkCanceled(u8 taskId)
{
    if (IsLinkConnectionEstablished())
        SetSuppressLinkErrorMessage(TRUE);

    if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_LinkupFailed;
        return TRUE;
    }
    return FALSE;
}

static bool32 CheckSioErrored(u8 taskId)
{
    if (GetSioMultiSI() == TRUE)
    {
        gTasks[taskId].func = Task_LinkupConnectionError;
        return TRUE;
    }
    return FALSE;
}

static void UNUSED Task_DelayedBlockRequest(u8 taskId)
{
    gTasks[taskId].data[0]++;
    if (gTasks[taskId].data[0] == 10)
    {
        SendBlockRequest(BLOCK_REQ_SIZE_100);
        DestroyTask(taskId);
    }
}

static void Task_LinkupStart(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (tState == 0)
    {
        OpenLinkTimed();
        ResetLinkPlayerCount();
        ResetLinkPlayers();
    }
    else if (tState > 9)
    {
        gTasks[taskId].func = Task_LinkupAwaitConnection;
    }
    tState++;
}

static void Task_LinkupAwaitConnection(u8 taskId)
{
    u32 playerCount = GetLinkPlayerCount_2();

    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckLinkCanceled(taskId) == TRUE
     || playerCount < 2)
        return;

    SetSuppressLinkErrorMessage(TRUE);
    gTasks[taskId].tNumPlayers = 0;
    if (IsLinkMaster() == TRUE)
    {
        PlaySE(SE_PIN);
        ShowFieldAutoScrollMessage(gText_ConfirmLinkWhenPlayersReady);
        gTasks[taskId].func = Task_LinkupConfirmWhenReady;
    }
    else
    {
        PlaySE(SE_BOO);
        ShowFieldAutoScrollMessage(gText_AwaitingLinkup);
        gTasks[taskId].func = Task_LinkupExchangeDataWithLeader;
    }
}

static void Task_LinkupConfirmWhenReady(u8 taskId)
{
    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckSioErrored(taskId) == TRUE
     || CheckLinkErrored(taskId) == TRUE)
        return;

    if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN)
    {
        gTasks[taskId].tNumPlayers = 0;
        gTasks[taskId].func = Task_LinkupAwaitConfirmation;
    }
}

static void Task_LinkupAwaitConfirmation(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s32 linkPlayerCount = GetLinkPlayerCount_2();

    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckSioErrored(taskId) == TRUE
     || CheckLinkErrored(taskId) == TRUE)
        return;

    UpdateLinkPlayerCountDisplay(taskId, linkPlayerCount);

    if (!JOY_NEW(A_BUTTON))
        return;

#if ENGLISH
    if (linkPlayerCount < tMinPlayers)
        return;

    SaveLinkPlayers(linkPlayerCount);
    ClearLinkPlayerCountWindow();
    ConvertIntToDecimalStringN(gStringVar1, linkPlayerCount, STR_CONV_MODE_LEFT_ALIGN, 1);
    ShowFieldAutoScrollMessage((u8 *)gText_ConfirmStartLinkWithXPlayers);
    gTasks[taskId].func = Task_LinkupTryConfirmation;
#elif GERMAN
    if ((gLinkType == LINKTYPE_MULTI_BATTLE && (u32)linkPlayerCount > 1)
     || (gLinkType != LINKTYPE_MULTI_BATTLE && tMinPlayers <= linkPlayerCount))
    {
        SaveLinkPlayers(linkPlayerCount);
        ClearLinkPlayerCountWindow();
        ConvertIntToDecimalStringN(gStringVar1, linkPlayerCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        ShowFieldAutoScrollMessage((u8 *)gText_ConfirmStartLinkWithXPlayers);
        gTasks[taskId].func = Task_LinkupTryConfirmation;
    }
#endif
}

static void Task_LinkupTryConfirmation(u8 taskId)
{
    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckSioErrored(taskId) == TRUE
     || CheckLinkErrored(taskId) == TRUE)
        return;

    if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN)
    {
        if (GetSavedPlayerCount() != GetLinkPlayerCount_2())
        {
            ShowFieldAutoScrollMessage(gText_ConfirmLinkWhenPlayersReady);
            gTasks[taskId].func = Task_LinkupConfirmWhenReady;
        }
        else if (JOY_HELD(B_BUTTON))
        {
            ShowFieldAutoScrollMessage(gText_ConfirmLinkWhenPlayersReady);
            gTasks[taskId].func = Task_LinkupConfirmWhenReady;
        }
        else if (JOY_HELD(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            CheckShouldAdvanceLinkState();
            gTasks[taskId].func = Task_LinkupConfirm;
        }
    }
}

static void Task_LinkupConfirm(u8 taskId)
{
    u8 minPlayers = gTasks[taskId].tMinPlayers;
    u8 maxPlayers = gTasks[taskId].tMaxPlayers;

    if (CheckLinkErrored(taskId) == TRUE
     || TryLinkTimeout(taskId) == TRUE)
        return;

    if (GetLinkPlayerCount_2() != GetSavedPlayerCount())
    {
        gTasks[taskId].func = Task_LinkupConnectionError;
    }
    else
    {
        gSpecialVar_Result = ExchangeDataAndGetLinkupStatus(minPlayers, maxPlayers);
        if (gSpecialVar_Result != LINKUP_ONGOING)
            gTasks[taskId].func = Task_LinkupCheckStatusAfterConfirm;
    }
}

static void Task_LinkupExchangeDataWithLeader(u8 taskId)
{
    u8 minPlayers = gTasks[taskId].tMinPlayers;
    u8 maxPlayers = gTasks[taskId].tMaxPlayers;

    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckLinkErrored(taskId) == TRUE)
        return;

#if DEBUG
    UpdateLinkPlayerCountDisplay(taskId, GetLinkPlayerCount_2());
#endif

    gSpecialVar_Result = ExchangeDataAndGetLinkupStatus(minPlayers, maxPlayers);
    if (gSpecialVar_Result == LINKUP_ONGOING)
        return;
    if (gSpecialVar_Result == LINKUP_DIFF_SELECTIONS)
    {
        SetCloseLinkCallback();
        HideFieldMessageBox();
        gTasks[taskId].func = Task_StopLinkup;
    }
    else
    {
        gFieldLinkPlayerCount = GetLinkPlayerCount_2();
        gLocalLinkPlayerId = GetMultiplayerId();
        SaveLinkPlayers(gFieldLinkPlayerCount);
        TrainerCard_GenerateCardForPlayer((struct TrainerCard *)gBlockSendBuffer);
        gTasks[taskId].func = Task_LinkupAwaitTrainerCardData;
    }
}

static void Task_LinkupCheckStatusAfterConfirm(u8 taskId)
{
    if (CheckLinkErrored(taskId) == TRUE)
        return;

    if (gSpecialVar_Result == LINKUP_DIFF_SELECTIONS)
    {
        SetCloseLinkCallback();
        HideFieldMessageBox();
        gTasks[taskId].func = Task_StopLinkup;
    }
    else
    {
        gFieldLinkPlayerCount = GetLinkPlayerCount_2();
        gLocalLinkPlayerId = GetMultiplayerId();
        SaveLinkPlayers(gFieldLinkPlayerCount);
        TrainerCard_GenerateCardForPlayer((struct TrainerCard *)gBlockSendBuffer);
        gTasks[taskId].func = Task_LinkupAwaitTrainerCardData;
        SendBlockRequest(BLOCK_REQ_SIZE_100);
    }
}

static void Task_LinkupAwaitTrainerCardData(u8 taskId)
{
    u8 index;
    struct TrainerCard *trainerCards;

    if (CheckLinkErrored(taskId) == TRUE)
        return;

    if (GetBlockReceivedStatus() != GetSavedLinkPlayerCountAsBitFlags())
        return;

    index = 0;
    trainerCards = gTrainerCards;
    for (index = 0; index < GetLinkPlayerCount(); index++)
    {
        void *src;
        src = gBlockRecvBuffer[index];
        memcpy(&trainerCards[index], src, sizeof(struct TrainerCard));
    }

    SetSuppressLinkErrorMessage(FALSE);
    ResetBlockReceivedFlags();
    HideFieldMessageBox();

    if (gSpecialVar_Result == LINKUP_SUCCESS)
    {
#if ENGLISH
        u16 linkType;
        linkType = gLinkType;
        // FIXME: ClearLinkPlayerCountWindow doesn't take any arguments
        ClearLinkPlayerCountWindow(LINKTYPE_BERRY_BLENDER_SETUP, linkType);
#elif GERMAN
        if (gLinkType != LINKTYPE_BERRY_BLENDER_SETUP)
        {
            if (gLinkType == LINKTYPE_CONTEST_GMODE)
                gGermanLinkBlockRequestState = 1;
        }
        ClearLinkPlayerCountWindow();
#endif
        ScriptContext_Enable();
        DestroyTask(taskId);
        return;
    }

    SetCloseLinkCallback();
    gTasks[taskId].func = Task_StopLinkup;
}

static void Task_StopLinkup(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers == FALSE)
    {
        ClearLinkPlayerCountWindow();
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static void Task_LinkupFailed(u8 taskId)
{
    gSpecialVar_Result = LINKUP_FAILED;
    ClearLinkPlayerCountWindow();
    HideFieldMessageBox();
    ScriptContext_Enable();
    DestroyTask(taskId);
}

static void Task_LinkupConnectionError(u8 taskId)
{
    gSpecialVar_Result = LINKUP_CONNECTION_ERROR;
    ClearLinkPlayerCountWindow();
    HideFieldMessageBox();
    ScriptContext_Enable();
    DestroyTask(taskId);
}

static bool8 TryLinkTimeout(u8 taskId)
{
    gTasks[taskId].tTimer++;
    if (gTasks[taskId].tTimer > 600)
    {
        gTasks[taskId].func = Task_LinkupConnectionError;
        return TRUE;
    }

    return FALSE;
}

void TryBattleLinkup(void)
{
    u32 minPlayers = 2;
    u32 maxPlayers = 2;

    switch (gSpecialVar_0x8004)
    {
    case USING_SINGLE_BATTLE:
        minPlayers = 2;
        gLinkType = LINKTYPE_SINGLE_BATTLE;
        break;
    case USING_DOUBLE_BATTLE:
        minPlayers = 2;
        gLinkType = LINKTYPE_DOUBLE_BATTLE;
        break;
    case USING_MULTI_BATTLE:
        minPlayers = 4;
        maxPlayers = 4;
        gLinkType = LINKTYPE_MULTI_BATTLE;
        break;
    }

    CreateLinkupTask(minPlayers, maxPlayers);
}

void TryTradeLinkup(void)
{
    gLinkType = LINKTYPE_TRADE_SETUP;
    gBattleTypeFlags = 0;
    CreateLinkupTask(2, 2);
}

void TryRecordMixLinkup(void)
{
    gSpecialVar_Result = LINKUP_ONGOING;
    gLinkType = LINKTYPE_RECORD_MIX_BEFORE;
    gBattleTypeFlags = 0;
    CreateLinkupTask(2, 4);
}

static void Task_ValidateMixingGameLanguage(u8 taskId)
{
    int playerCount;
    int i;

    switch (gTasks[taskId].tState)
    {
    case 0:
        if (gSpecialVar_Result == LINKUP_SUCCESS)
        {
            playerCount = GetLinkPlayerCount();
            for (i = 0; i < playerCount; i++)
            {
                if (gLinkPlayers[i].language == LANGUAGE_JAPANESE)
                {
                    gSpecialVar_Result = LINKUP_FOREIGN_GAME;
                    SetCloseLinkCallbackHandleJP();
                    gTasks[taskId].tState = 1;
                    return;
                }
            }
        }
        ScriptContext_Enable();
        DestroyTask(taskId);
        break;
    case 1:
        if (gReceivedRemoteLinkPlayers == FALSE)
        {
            ScriptContext_Enable();
            DestroyTask(taskId);
        }
        break;
    }
}

void ValidateMixingGameLanguage(void)
{
    int taskId = FindTaskIdByFunc(Task_ValidateMixingGameLanguage);

    if (taskId == TASK_NONE)
    {
        taskId = CreateTask(Task_ValidateMixingGameLanguage, 80);
        gTasks[taskId].tState = 0;
    }
}

void TryBerryBlenderLinkup(void)
{
    gLinkType = LINKTYPE_BERRY_BLENDER_SETUP;
    gBattleTypeFlags = 0;
    CreateLinkupTask(2, 4);
}

void TryContestGModeLinkup(void)
{
    gLinkType = LINKTYPE_CONTEST_GMODE;
    gBattleTypeFlags = 0;
    CreateLinkupTask(4, 4);
}

#undef tMinPlayers
#undef tMaxPlayers
#undef tNumPlayers
#undef tTimer

u8 CreateTask_ReestablishCableClubLink(void)
{
    if (FuncIsActiveTask(Task_ReestablishLink) != FALSE)
        return TASK_NONE;

    switch (gSpecialVar_0x8004)
    {
    case USING_SINGLE_BATTLE:
        gLinkType = LINKTYPE_SINGLE_BATTLE;
        break;
    case USING_DOUBLE_BATTLE:
        gLinkType = LINKTYPE_DOUBLE_BATTLE;
        break;
    case USING_MULTI_BATTLE:
        gLinkType = LINKTYPE_MULTI_BATTLE;
        break;
    case USING_TRADE_CENTER:
        gLinkType = LINKTYPE_TRADE;
        break;
    case USING_RECORD_CORNER:
        gLinkType = LINKTYPE_RECORD_MIX_AFTER;
        break;
    }

    return CreateTask(Task_ReestablishLink, 80);
}

static void Task_ReestablishLink(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (tState == 0)
    {
        OpenLink();
        ResetLinkPlayers();
        CreateTask(Task_WaitForLinkPlayerConnection, 80);
    }
    else if (tState >= 10)
    {
        gTasks[taskId].func = Task_ReestablishLinkAwaitConnection;
    }
    tState++;
}

static void Task_ReestablishLinkAwaitConnection(u8 taskId)
{
    if (GetLinkPlayerCount_2() >= 2)
    {
        if (IsLinkMaster() == TRUE)
            gTasks[taskId].func = Task_ReestablishLinkLeader;
        else
            gTasks[taskId].func = Task_ReestablishLinkAwaitConfirmation;
    }
}

static void Task_ReestablishLinkLeader(u8 taskId)
{
    if (GetSavedPlayerCount() == GetLinkPlayerCount_2())
    {
        CheckShouldAdvanceLinkState();
        gTasks[taskId].func = Task_ReestablishLinkAwaitConfirmation;
    }
}

static void Task_ReestablishLinkAwaitConfirmation(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers == TRUE
     && IsLinkPlayerDataExchangeComplete() == TRUE)
    {
        CheckLinkPlayersMatchSaved();
        StartSendingKeysToLink();
        DestroyTask(taskId);
    }
}

void CableClubSaveGame(void)
{
    SaveGame();
}

#define tTimer data[1]

static void Task_StartWiredCableClubBattle(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        FadeScreen(FADE_TO_BLACK, 0);
        gLinkType = LINKTYPE_BATTLE;
        ClearLinkCallback_2();
        task->tState++;
        break;
    case 1:
        if (!gPaletteFade.active)
            task->tState++;
        break;
    case 2:
        task->tTimer++;
        if (task->tTimer > 20)
            task->tState++;
        break;
    case 3:
        SetCloseLinkCallback();
        task->tState++;
        break;
    case 4:
        if (!gReceivedRemoteLinkPlayers)
            task->tState++;
        break;
    case 5:
        if (gLinkPlayers[0].trainerId & 1)
            PlayMapChosenOrBattleBGM(MUS_VS_GYM_LEADER);
        else
            PlayMapChosenOrBattleBGM(MUS_VS_TRAINER);

        switch (gSpecialVar_0x8004)
        {
        case USING_SINGLE_BATTLE:
            gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK;
            break;
        case USING_DOUBLE_BATTLE:
            gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK | BATTLE_TYPE_DOUBLE;
            break;
        case USING_MULTI_BATTLE:
            ReducePlayerPartyToSelectedMons();
            gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI;
            break;
        }

        SetMainCallback2(CB2_InitBattle);
        gMain.savedCallback = CB2_ReturnFromCableClubBattle;
        DestroyTask(taskId);
        break;
    }
}

#undef tTimer

static void CB2_ReturnFromCableClubBattle(void)
{
    Overworld_ResetMapMusic();
    LoadPlayerParty();
    SavePlayerBag();
    UpdateTrainerFansAfterLinkBattle();

    if (gSpecialVar_0x8004 != USING_MULTI_BATTLE)
        UpdatePlayerLinkBattleRecords(gLocalLinkPlayerId ^ 1);

    gMain.savedCallback = CB2_ReturnToFieldFromMultiplayer;
    SetMainCallback2(CB2_SetUpSaveAfterLinkBattle);
}

void CleanupLinkRoomState(void)
{
    if (gSpecialVar_0x8004 == USING_SINGLE_BATTLE
     || gSpecialVar_0x8004 == USING_DOUBLE_BATTLE
     || gSpecialVar_0x8004 == USING_MULTI_BATTLE)
    {
        LoadPlayerParty();
        SavePlayerBag();
    }
    SetWarpDestinationToDynamicWarp(WARP_ID_DYNAMIC);
}

void ExitLinkRoom(void)
{
    QueueExitLinkRoomKey();
}

static void Task_EnterCableClubSeat(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        ShowFieldMessage(gText_PleaseWaitForLink);
        task->tState = 1;
        break;
    case 1:
        if (IsFieldMessageBoxHidden())
        {
            SetInCableClubSeat();
            SetLocalLinkPlayerId(gSpecialVar_0x8005);
            task->tState = 2;
        }
        break;
    case 2:
        switch (GetCableClubPartnersReady())
        {
        case CABLE_SEAT_WAITING:
            break;
        case CABLE_SEAT_SUCCESS:
            HideFieldMessageBox();
            task->tState = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        case CABLE_SEAT_FAILED:
            task->tState = 3;
            break;
        }
        break;
    case 3:
        SetLinkWaitingForScript();
        HideFieldMessageBox();
        Menu_EraseScreen();
        DestroyTask(taskId);
        ScriptContext_Enable();
        break;
    }
}

void CreateTask_EnterCableClubSeat(TaskFunc followupFunc)
{
    u8 taskId = CreateTask(Task_EnterCableClubSeat, 80);
    SetTaskFuncWithFollowupFunc(taskId, Task_EnterCableClubSeat, followupFunc);
    ScriptContext_Stop();
}

static void Task_StartWiredTrade(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        LockPlayerFieldControls();
        FadeScreen(FADE_TO_BLACK, 0);
        ClearLinkCallback_2();
        task->tState++;
        break;
    case 1:
        if (!gPaletteFade.active)
            task->tState++;
        break;
    case 2:
        gSelectedTradeMonPositions[0] = 0;
        gSelectedTradeMonPositions[1] = 0;
        m4aMPlayAllStop();
        SetCloseLinkCallback();
        task->tState++;
        break;
    case 3:
        if (!gReceivedRemoteLinkPlayers)
        {
            SetMainCallback2(CB2_StartCreateTradeMenu);
            DestroyTask(taskId);
        }
        break;
    }
}

static void Task_StartWiredTradeFromSeat(u8 taskId)
{
    CreateTask_StartWiredTrade();
    DestroyTask(taskId);
}

void PlayerEnteredTradeSeat(void)
{
    CreateTask_EnterCableClubSeat(Task_StartWiredTradeFromSeat);
}

static void CreateTask_StartWiredTrade(void)
{
    CreateTask(Task_StartWiredTrade, 80);
}

void Script_StartWiredTrade(void)
{
    CreateTask_StartWiredTrade();
    ScriptContext_Stop();
}

void ColosseumPlayerSpotTriggered(void)
{
    gLinkType = LINKTYPE_BATTLE;
    CreateTask_EnterCableClubSeat(Task_StartWiredCableClubBattle);
}

void CreateTask_RecordMixingFromCableClubSeat(void)
{
    u8 taskId = CreateTask(Task_EnterCableClubSeat, 80);
    SetTaskFuncWithFollowupFunc(taskId, Task_EnterCableClubSeat, Task_RecordMixing_Main);
    ScriptContext_Stop();
}

void Script_ShowLinkTrainerCard(void)
{
    ShowTrainerCardInLink(gSpecialVar_0x8006, CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

bool32 GetLinkTrainerCardColor(u8 linkPlayerIndex)
{
    u32 numStars;

    gSpecialVar_0x8006 = linkPlayerIndex;
    StringCopy(gStringVar1, gLinkPlayers[linkPlayerIndex].name);

    numStars = GetTrainerCardStars(linkPlayerIndex);
    if (numStars == 0)
        return FALSE;

    StringCopy(gStringVar2, sTrainerCardColorNames[numStars - 1]);
    return TRUE;
}

#define tTimer data[0]

void Task_WaitForLinkPlayerConnection(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->tTimer++;
    if (task->tTimer > 300)
    {
        CloseLink();
        SetMainCallback2(CB2_LinkError);
        DestroyTask(taskId);
    }

    if (gReceivedRemoteLinkPlayers)
        DestroyTask(taskId);
}

#undef tTimer

#if DEBUG
u16 gDebugLinkTestFlags;
u32 gDebugLinkTestCounter;

static void Task_DebugUpdateLinkTestFlags(u8 taskId);

void Debug_UpdateLinkTestFlags(void)
{
    if (!FuncIsActiveTask(Task_DebugUpdateLinkTestFlags))
        CreateTask(Task_DebugUpdateLinkTestFlags, 80);
    gDebugLinkTestCounter++;
}

static void Task_DebugUpdateLinkTestFlags(u8 taskId)
{
    gTasks[taskId].data[0]++;
    if (gTasks[taskId].data[0] == 30)
    {
        gTasks[taskId].data[0] = 0;
        gDebugLinkTestFlags |= 1;
    }
}
#endif

static void Task_WaitExitToScript(u8 taskId)
{
    if (!gReceivedRemoteLinkPlayers)
    {
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static void UNUSED ExitLinkToScript(u8 taskId)
{
    SetCloseLinkCallback();
    gTasks[taskId].func = Task_WaitExitToScript;
}

#if DEBUG
EWRAM_DATA static u8 sDebugLinkStateFlags = 0;

void Debug_ResetLinkStateFlags(void)
{
    sDebugLinkStateFlags = 0;
}

void Debug_SetLinkStateFlag(u8 flagId)
{
    sDebugLinkStateFlags |= 1 << flagId;
}

static u8 Debug_GetLinkStateFlags(void)
{
    return sDebugLinkStateFlags;
}
#endif

#undef tState
