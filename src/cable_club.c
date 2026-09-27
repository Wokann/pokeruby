#include "global.h"
#include "cable_club.h"
#include "battle.h"
#include "battle_records.h"
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
extern const u8 gUnknown_081A4932[];
extern const u8 gUnknown_081A4975[];
extern const u8 gUnknown_081A49B6[];
extern const u8 gText_PleaseWaitForLink[];
extern struct
{
    u8 field0;
    u8 field1;
} gUnknown_020297D8;

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

#ifdef GERMAN
const u8 TrainerCardColorName_Bronze[] = _("BRONZE");
const u8 TrainerCardColorName_Copper[] = _("KUPFER");
const u8 TrainerCardColorName_Silver[] = _("SILBER");
const u8 TrainerCardColorName_Gold[] = _("GOLD");
#else
const u8 TrainerCardColorName_Bronze[] = _("BRONZE");
const u8 TrainerCardColorName_Copper[] = _("COPPER");
const u8 TrainerCardColorName_Silver[] = _("SILVER");
const u8 TrainerCardColorName_Gold[] = _("GOLD");
#endif

const u8 *const gTrainerCardColorNames[] =
{
    TrainerCardColorName_Bronze,
    TrainerCardColorName_Copper,
    TrainerCardColorName_Silver,
    TrainerCardColorName_Gold,
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

    PrintHex(gShouldAdvanceLinkState, 2, 0, 2);
    PrintHex((u8)gBlockSendBuffer[0], 22, 5, 4);
    for (i = 0; i < 4; i++)
    {
        PrintHex(gLinkPlayerPending[i], 5 + i * 2, 0, 1);
        PrintHex(gBlockRecvBuffer[i][0], 22, 6 + i, 4);
    }
    PrintHex(gLinkStatus, 15, 0, 8);
    PrintHex(gLink.state, 2, 10, 2);
    PrintHex(GetMultiplayerId(), 7, 12, 2);
    PrintHex(GetBlockReceivedStatus(), 7, 10, 2);
    PrintHex(gReceivedRemoteLinkPlayers, 2, 12, 1);
    PrintHex(gSpecialVar_Result, 11, 8, 2);
    PrintHex((gLinkStatus & 0x1C) >> 2, 11, 10, 2);
    PrintHex(IsLinkConnectionEstablished(), 11, 12, 1);
    PrintHex(IsLinkTaskFinished(), 15, 10, 1);
    PrintHex(Debug_GetLinkupTaskState(gTasks[gTasks[taskId].data[0]].func), 15, 12, 2);
    PrintHex((uintptr_t)gLinkCallback, 2, 13, 8);
    PrintHex(HasLinkErrorOccurred(), 2, 2, 1);
    for (i = 0; i < 4; i++)
        PrintHex(gLinkPlayers[i].linkType, 2 + i * 6, 3, 4);
    PrintHex(REG_SIOCNT, 2, 6, 4);
    PrintHex(Debug_GetLinkStateFlags(), 25, 3, 1);
}
#endif

static void CreateLinkupTask(u8 minPlayers, u8 maxPlayers)
{
#if DEBUG
    InitLinkTestBG_Unused(12, 0, 31, 2);
#endif
    if (FindTaskIdByFunc(Task_LinkupStart) == 0xFF)
    {
        u8 linkupTaskId;
#if DEBUG
        u8 debugTaskId;
#endif

        linkupTaskId = CreateTask(Task_LinkupStart, 80);
        gTasks[linkupTaskId].data[1] = minPlayers;
        gTasks[linkupTaskId].data[2] = maxPlayers;

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
    MenuPrint_Centered(gOtherText_PLink, 19, 11, 72);
}

static void ClearLinkPlayerCountWindow()
{
    Menu_EraseWindowRect(18, 10, 28, 13);
}

static void UpdateLinkPlayerCountDisplay(u8 taskId, u8 numPlayers)
{
    s16 *taskData = &gTasks[taskId].data[3];

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
            return 1;
        ConvertIntToDecimalStringN(gStringVar1, playerCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        return 4;
    case EXCHANGE_TIMED_OUT:
        return 0;
    case EXCHANGE_IN_PROGRESS:
        return 3;
    default:
        return 0;
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
    if (GetSioMultiSI() == 1)
    {
        gTasks[taskId].func = Task_LinkupConnectionError;
        return TRUE;
    }
    return FALSE;
}

void Task_DelayedBlockRequest(u8 taskId)
{
    gTasks[taskId].data[0]++;
    if (gTasks[taskId].data[0] == 10)
    {
        sub_8007E9C(2);
        DestroyTask(taskId);
    }
}

static void Task_LinkupStart(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (data[0] == 0)
    {
        OpenLinkTimed();
        sub_80082EC();
        ResetLinkPlayers();
    }
    else if (data[0] > 9)
    {
        gTasks[taskId].func = Task_LinkupAwaitConnection;
    }
    data[0]++;
}

static void Task_LinkupAwaitConnection(u8 taskId)
{
    u32 playerCount = GetLinkPlayerCount_2();

    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckLinkCanceled(taskId) == TRUE
     || playerCount < 2)
        return;

    SetSuppressLinkErrorMessage(TRUE);
    gTasks[taskId].data[3] = 0;
    if (IsLinkMaster() == TRUE)
    {
        PlaySE(SE_PIN);
        ShowFieldAutoScrollMessage(gUnknown_081A4932);
        gTasks[taskId].func = Task_LinkupConfirmWhenReady;
    }
    else
    {
        PlaySE(SE_BOO);
        ShowFieldAutoScrollMessage(gUnknown_081A49B6);
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
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].func = Task_LinkupAwaitConfirmation;
    }
}

static void Task_LinkupAwaitConfirmation(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    s32 linkPlayerCount = GetLinkPlayerCount_2();

    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckSioErrored(taskId) == TRUE
     || CheckLinkErrored(taskId) == TRUE)
        return;

    UpdateLinkPlayerCountDisplay(taskId, linkPlayerCount);

    if (!JOY_NEW(A_BUTTON))
        return;

#if ENGLISH
    if (linkPlayerCount < taskData[1])
        return;

    sub_80081C8(linkPlayerCount);
    ClearLinkPlayerCountWindow();
    ConvertIntToDecimalStringN(gStringVar1, linkPlayerCount, STR_CONV_MODE_LEFT_ALIGN, 1);
    ShowFieldAutoScrollMessage((u8 *)gUnknown_081A4975);
    gTasks[taskId].func = Task_LinkupTryConfirmation;
#elif GERMAN
    if ((gLinkType == 0x2255 && (u32)linkPlayerCount > 1)
     || (gLinkType != 0x2255 && taskData[1] <= linkPlayerCount))
    {
        sub_80081C8(linkPlayerCount);
        ClearLinkPlayerCountWindow();
        ConvertIntToDecimalStringN(gStringVar1, linkPlayerCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        ShowFieldAutoScrollMessage((u8 *)gUnknown_081A4975);
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
            ShowFieldAutoScrollMessage(gUnknown_081A4932);
            gTasks[taskId].func = Task_LinkupConfirmWhenReady;
        }
        else if (JOY_HELD(B_BUTTON))
        {
            ShowFieldAutoScrollMessage(gUnknown_081A4932);
            gTasks[taskId].func = Task_LinkupConfirmWhenReady;
        }
        else if (JOY_HELD(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sub_8007F4C();
            gTasks[taskId].func = Task_LinkupConfirm;
        }
    }
}

static void Task_LinkupConfirm(u8 taskId)
{
    u8 minPlayers = gTasks[taskId].data[1];
    u8 maxPlayers = gTasks[taskId].data[2];

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
        if (gSpecialVar_Result != 0)
            gTasks[taskId].func = Task_LinkupCheckStatusAfterConfirm;
    }
}

static void Task_LinkupExchangeDataWithLeader(u8 taskId)
{
    u8 minPlayers;
    u8 maxPlayers;

    minPlayers = gTasks[taskId].data[1];
    maxPlayers = gTasks[taskId].data[2];

    if (CheckLinkCanceledBeforeConnection(taskId) == TRUE
     || CheckLinkErrored(taskId) == TRUE)
        return;

#if DEBUG
    UpdateLinkPlayerCountDisplay(taskId, GetLinkPlayerCount_2());
#endif

    gSpecialVar_Result = ExchangeDataAndGetLinkupStatus(minPlayers, maxPlayers);
    if (gSpecialVar_Result == 0)
        return;
    if (gSpecialVar_Result == 3)
    {
        SetCloseLinkCallback();
        HideFieldMessageBox();
        gTasks[taskId].func = Task_StopLinkup;
    }
    else
    {
        gFieldLinkPlayerCount = GetLinkPlayerCount_2();
        gUnknown_03004860 = GetMultiplayerId();
        sub_80081C8(gFieldLinkPlayerCount);
        TrainerCard_GenerateCardForPlayer((struct TrainerCard *)gBlockSendBuffer);
        gTasks[taskId].func = Task_LinkupAwaitTrainerCardData;
    }
}

static void Task_LinkupCheckStatusAfterConfirm(u8 taskId)
{
    if (CheckLinkErrored(taskId) == TRUE)
        return;

    if (gSpecialVar_Result == 3)
    {
        SetCloseLinkCallback();
        HideFieldMessageBox();
        gTasks[taskId].func = Task_StopLinkup;
    }
    else
    {
        gFieldLinkPlayerCount = GetLinkPlayerCount_2();
        gUnknown_03004860 = GetMultiplayerId();
        sub_80081C8(gFieldLinkPlayerCount);
        TrainerCard_GenerateCardForPlayer((struct TrainerCard *)gBlockSendBuffer);
        gTasks[taskId].func = Task_LinkupAwaitTrainerCardData;
        sub_8007E9C(2);
    }
}

static void Task_LinkupAwaitTrainerCardData(u8 taskId)
{
    u8 index;
    struct TrainerCard *trainerCards;

    if (CheckLinkErrored(taskId) == TRUE)
        return;

    if (GetBlockReceivedStatus() != sub_8008198())
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

    if (gSpecialVar_Result == 1)
    {
#if ENGLISH
        u16 linkType;
        linkType = gLinkType;
        // FIXME: ClearLinkPlayerCountWindow doesn't take any arguments
        ClearLinkPlayerCountWindow(0x4411, linkType);
#elif GERMAN
        if (gLinkType != 0x4411)
        {
            if (gLinkType == 0x6601)
                deUnkValue2 = 1;
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
    gSpecialVar_Result = 5;
    ClearLinkPlayerCountWindow();
    HideFieldMessageBox();
    ScriptContext_Enable();
    DestroyTask(taskId);
}

static void Task_LinkupConnectionError(u8 taskId)
{
    gSpecialVar_Result = 6;
    ClearLinkPlayerCountWindow();
    HideFieldMessageBox();
    ScriptContext_Enable();
    DestroyTask(taskId);
}

static bool8 TryLinkTimeout(u8 taskId)
{
    gTasks[taskId].data[4]++;
    if (gTasks[taskId].data[4] > 600)
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
    case 1:
        minPlayers = 2;
        gLinkType = 0x2233;
        break;
    case 2:
        minPlayers = 2;
        gLinkType = 0x2244;
        break;
    case 5:
        minPlayers = 4;
        maxPlayers = 4;
        gLinkType = 0x2255;
        break;
    }

    CreateLinkupTask(minPlayers, maxPlayers);
}

void TryTradeLinkup(void)
{
    gLinkType = 0x1133;
    gBattleTypeFlags = 0;
    CreateLinkupTask(2, 2);
}

void TryRecordMixLinkup(void)
{
    gSpecialVar_Result = 0;
    gLinkType = 0x3311;
    gBattleTypeFlags = 0;
    CreateLinkupTask(2, 4);
}

static void Task_ValidateMixingGameLanguage(u8 taskId)
{
    int playerCount;
    int i;

    switch (gTasks[taskId].data[0])
    {
    case 0:
        if (gSpecialVar_Result == 1)
        {
            playerCount = GetLinkPlayerCount();
            for (i = 0; i < playerCount; i++)
            {
                if (gLinkPlayers[i].language == LANGUAGE_JAPANESE)
                {
                    gSpecialVar_Result = 7;
                    sub_8008480();
                    gTasks[taskId].data[0] = 1;
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

    if (taskId == 0xFF)
    {
        taskId = CreateTask(Task_ValidateMixingGameLanguage, 80);
        gTasks[taskId].data[0] = 0;
    }
}

void TryBerryBlenderLinkup(void)
{
    gLinkType = 0x4411;
    gBattleTypeFlags = 0;
    CreateLinkupTask(2, 4);
}

void TryContestGModeLinkup(void)
{
    gLinkType = 0x6601;
    gBattleTypeFlags = 0;
    CreateLinkupTask(4, 4);
}

u8 CreateTask_ReestablishCableClubLink(void)
{
    if (FuncIsActiveTask(Task_ReestablishLink) != FALSE)
        return 0xFF;

    switch (gSpecialVar_0x8004)
    {
    case 1:
        gLinkType = 0x2233;
        break;
    case 2:
        gLinkType = 0x2244;
        break;
    case 5:
        gLinkType = 0x2255;
        break;
    case 3:
        gLinkType = 0x1111;
        break;
    case 4:
        gLinkType = 0x3322;
        break;
    }

    return CreateTask(Task_ReestablishLink, 80);
}

static void Task_ReestablishLink(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (data[0] == 0)
    {
        OpenLink();
        ResetLinkPlayers();
        CreateTask(Task_WaitForLinkPlayerConnection, 80);
    }
    else if (data[0] >= 10)
    {
        gTasks[taskId].func = Task_ReestablishLinkAwaitConnection;
    }
    data[0]++;
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
        sub_8007F4C();
        gTasks[taskId].func = Task_ReestablishLinkAwaitConfirmation;
    }
}

static void Task_ReestablishLinkAwaitConfirmation(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers == TRUE
     && IsLinkPlayerDataExchangeComplete() == TRUE)
    {
        sub_800826C();
        sub_8007B14();
        DestroyTask(taskId);
    }
}

void CableClubSaveGame(void)
{
    SaveGame();
}

static void Task_StartWiredCableClubBattle(u8 taskId)
{
    struct Task* task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        FadeScreen(1, 0);
        gLinkType = 0x2211;
        ClearLinkCallback_2();
        task->data[0]++;
        break;
    case 1:
        if (!gPaletteFade.active)
            task->data[0]++;
        break;
    case 2:
        task->data[1]++;
        if (task->data[1] > 20)
            task->data[0]++;
        break;
    case 3:
        SetCloseLinkCallback();
        task->data[0]++;
        break;
    case 4:
        if (!gReceivedRemoteLinkPlayers)
            task->data[0]++;
        break;
    case 5:
        if (gLinkPlayers[0].trainerId & 1)
            current_map_music_set__default_for_battle(MUS_VS_GYM_LEADER);
        else
            current_map_music_set__default_for_battle(MUS_VS_TRAINER);

        switch (gSpecialVar_0x8004)
        {
        case 1:
            gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK;
            break;
        case 2:
            gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK | BATTLE_TYPE_DOUBLE;
            break;
        case 5:
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

static void CB2_ReturnFromCableClubBattle(void)
{
    Overworld_ResetMapMusic();
    LoadPlayerParty();
    SavePlayerBag();
    UpdateTrainerFansAfterLinkBattle();

    if (gSpecialVar_0x8004 != 5)
        UpdateLinkBattleRecords(gUnknown_03004860 ^ 1);

    gMain.savedCallback = sub_805465C;
    SetMainCallback2(sub_8071B28);
}

void CleanupLinkRoomState(void)
{
    if (gSpecialVar_0x8004 == 1 || gSpecialVar_0x8004 == 2 || gSpecialVar_0x8004 == 5)
    {
        LoadPlayerParty();
        SavePlayerBag();
    }
    copy_saved_warp2_bank_and_enter_x_to_warp1(0x7F);
}

void ExitLinkRoom(void)
{
    sub_805559C();
}

static void Task_EnterCableClubSeat(u8 taskId)
{
    struct Task* task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        ShowFieldMessage(gText_PleaseWaitForLink);
        task->data[0] = 1;
        break;
    case 1:
        if (IsFieldMessageBoxHidden())
        {
            sub_8055574();
            sub_8007270(gSpecialVar_0x8005);
            task->data[0] = 2;
        }
        break;
    case 2:
        switch (sub_80554F8())
        {
        case 0:
            break;
        case 1:
            HideFieldMessageBox();
            task->data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        case 2:
            task->data[0] = 3;
            break;
        }
        break;
    case 3:
        sub_8055588();
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

    switch (task->data[0])
    {
    case 0:
        LockPlayerFieldControls();
        FadeScreen(1, 0);
        ClearLinkCallback_2();
        task->data[0]++;
        break;
    case 1:
        if (!gPaletteFade.active)
            task->data[0]++;
        break;
    case 2:
        gUnknown_020297D8.field0 = 0;
        gUnknown_020297D8.field1 = 0;
        m4aMPlayAllStop();
        SetCloseLinkCallback();
        task->data[0]++;
        break;
    case 3:
        if (!gReceivedRemoteLinkPlayers)
        {
            SetMainCallback2(sub_8047CD8);
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
    gLinkType = 0x2211;
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
    TrainerCard_ShowLinkCard(gSpecialVar_0x8006, CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

bool32 GetLinkTrainerCardColor(u8 linkPlayerIndex)
{
    u32 trainerCardColorIndex;

    gSpecialVar_0x8006 = linkPlayerIndex;
    StringCopy(gStringVar1, gLinkPlayers[linkPlayerIndex].name);

    trainerCardColorIndex = sub_80934C4(linkPlayerIndex);
    if (trainerCardColorIndex == 0)
        return FALSE;

    StringCopy(gStringVar2, gTrainerCardColorNames[trainerCardColorIndex - 1]);
    return TRUE;
}

void Task_WaitForLinkPlayerConnection(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->data[0]++;
    if (task->data[0] > 300)
    {
        CloseLink();
        SetMainCallback2(CB2_LinkError);
        DestroyTask(taskId);
    }

    if (gReceivedRemoteLinkPlayers)
        DestroyTask(taskId);
}

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

void ExitLinkToScript(u8 taskId)
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
