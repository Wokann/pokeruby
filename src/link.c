#include "global.h"
#include "cable_club.h"
#include "link.h"
#include "battle.h"
#include "berry.h"
#include "hall_of_fame.h"
#include "item_use.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "random.h"
#include "save.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings2.h"
#include "task.h"
#include "text.h"
#include "ewram.h"

#define SIO_MULTI_CNT ((struct SioMultiCnt *)REG_ADDR_SIOCNT)

struct BlockTransfer
{
    u16 pos;
    u16 size;
    void *src;
    bool8 active;
    u8 multiplayerId;
};

struct LinkTestBGInfo
{
    u32 screenBaseBlock;
    u32 paletteNum;
    u32 dummy_8;
    u32 dummy_C;
};

extern u16 gBattleTypeFlags;

extern u16 gHeldKeyCodeToSend;

extern void SetPlayerBerryData(u8 bank, u16 itemID);

static void InitLinkTestBG(u8, u8, u8, u8);
void LinkTestScreen();
static void InitLocalLinkPlayer(void);
static void VBlankCB_LinkTest(void);
static void InitLink(void);
static void Task_TriggerHandshake(u8);
static void TestBlockTransfer(u32, u32, u32);
static void LinkTestProcessKeyInput(void);
static void CB2_LinkTest(void);
static void HandleReceiveRemoteLinkPlayer(u8);
static void ProcessRecvCmds(u8);
static void BuildSendCmd(u16);
static void LinkCB_SendHeldKeys(void);
static void ResetBlockSend(void);
static bool8 InitBlockSend(void *, u32);
static void LinkCB_BlockSendBegin(void);
static void LinkCB_BlockSend(void);
static void LinkCB_BlockSendEnd(void);
static void LinkCB_BerryBlenderSendHeldKeys(void);
u32 GetBerryBlenderKeySendAttempts(void);
static void SetBlockReceivedFlag(u8);
static u16 LinkTestCalcBlockChecksum(void *, u16);
static void LinkTest_PrintNumChar(u8, u8, u8);
static void LinkCB_RequestPlayerDataExchange(void);
static void Task_PrintTestData(u8);
bool8 DoSavedLinkPlayerTrainerIdsMatch(void);
u8 GetDummy2(void);
static void LinkCB_ReadyCloseLink(void);
static void LinkCB_WaitCloseLink(void);
static void LinkCB_WaitCloseLinkWithJP(void);
static void LinkCB_ReadyCloseLinkWithJP(void);
static void LinkCB_Standby(void);
static void LinkCB_StandbyForAll(void);

static void CheckErrorStatus(void);
void CB2_PrintErrorMessage(void);
static u8 IsSioMultiMaster(void);
static void DisableSerial(void);
static void EnableSerial(void);
static void CheckMasterOrSlave(void);
static void InitTimer(void);
static void EnqueueSendCmd(u16 *);
static void DequeueRecvCmds(u16[CMD_LENGTH][MAX_LINK_PLAYERS]);
static void StartTransfer(void);
static bool8 DoHandshake(void);
static void DoRecv(void);
static void DoSend(void);
static void StopTimer(void);
static void SendRecvDone(void);
void ResetSendBuffer(void);
void ResetRecvBuffer(void);

static struct BlockTransfer sBlockSend;
static struct BlockTransfer sBlockRecv[MAX_LINK_PLAYERS];
static u32 sBlockSendDelayCounter;
static u32 sDummy1;
static u8 sDummy2;
static u32 sPlayerDataExchangeStatus;
static u32 sErrorLinkStatus;
static u32 sErrorLastRecvQueueCount;
static u32 sErrorLastSendQueueCount;
static u32 sDummy3;
static u8 sLinkTestLastBlockSendPos;
static u8 sLinkTestLastBlockRecvPos[MAX_LINK_PLAYERS];
static u8 sNumVBlanksWithoutSerialIntr;
static bool8 sSendBufferEmpty;
static u16 sSendNonzeroCheck;
static u16 sRecvNonzeroCheck;
static u8 sChecksumAvailable;
static u8 sHandshakePlayerCount;

u16 word_3002910[MAX_LINK_PLAYERS];
u32 gLinkDebugValue1;
struct LinkPlayerBlock gLocalLinkPlayerBlock;
bool8 gLinkErrorOccurred;
u32 gLinkDebugValue2;
bool8 gLinkPlayerPending[MAX_LINK_PLAYERS];
struct LinkPlayer gLinkPlayers[MAX_LINK_PLAYERS];
bool8 gBlockReceived[MAX_LINK_PLAYERS];
u16 gLinkHeldKeys;
u16 gLinkTimeOutCounter;
struct LinkPlayer gLocalLinkPlayer;
u16 gRecvCmds[CMD_LENGTH][MAX_LINK_PLAYERS];
u32 gLinkStatus;
bool8 gLinkDummyBool;
u8 byte_3002A68;
u8 gBlockSendBuffer[BLOCK_BUFFER_SIZE];
bool8 gReadyToExitStandby[MAX_LINK_PLAYERS];
u16 gLinkType;
bool8 gReadyToCloseLink[MAX_LINK_PLAYERS];
u16 gBlockRecvBuffer[MAX_LINK_PLAYERS][BLOCK_BUFFER_SIZE / 2];
bool8 gSuppressLinkErrorMessage;
u8 gSavedLinkPlayerCount;
u16 gSendCmd[CMD_LENGTH];
u8 gSavedMultiplayerId;
bool8 gReceivedRemoteLinkPlayers;
struct LinkTestBGInfo gLinkTestBGInfo;
void (*gLinkCallback)(void);
struct LinkPlayer gSavedLinkPlayers[MAX_LINK_PLAYERS];
u8 gShouldAdvanceLinkState;
u16 gLinkTestBlockChecksums[MAX_LINK_PLAYERS];
#if DEBUG
u8 gUnknown_Debug_30030E0;
#endif
u8 gBlockRequestType;
u8 gLastSendQueueCount;
struct Link gLink;
u8 gLastRecvQueueCount;
u16 gLinkSavedIme;

#ifdef GERMAN
u8 deUnkValue1;
u8 deUnkValue2;
#endif

EWRAM_DATA bool8 gLinkTestDebugValuesEnabled = 0;
EWRAM_DATA bool8 gLinkTestDummyBool = 0;
EWRAM_DATA u32 gFiller_20238B8 = 0;
EWRAM_DATA u32 gBerryBlenderKeySendAttempts = 0;
EWRAM_DATA bool8 gLinkOpen = 0;

static const u16 sLinkTestDigitPalette[] = INCBIN_U16("graphics/interface/link_test_digits.gbapal");
static const u32 sLinkTestDigitTiles[] = INCBIN_U32("graphics/interface/link_test_digits.4bpp");

static const u8 sDebugMessages[7][12] =
{
    _("せつぞく　ちゅうです"),
    _("せつぞく　できません"),
    _("かくにん　ちゅうです"),
    _("かくにん　できました"),
    _("かくにん　できません"),
    _("かくにん　を　かくにん"),
    _("かくにん　は　しっぱい"),
};

static const u8 sColorCodes[] = _("{HIGHLIGHT TRANSPARENT}{COLOR WHITE2}");

const struct BlockRequest sBlockRequestLookupTable[5] =
{
    {gBlockSendBuffer, 200},
    {gBlockSendBuffer, 200},
    {gBlockSendBuffer, 100},
    {gBlockSendBuffer, 220},
    {gBlockSendBuffer, 40},
};

static const u8 sTestString[] = _("テストな");

const u8 sMagic[] = "GameFreak inc.";

const u8 sEmptyString[] = _(" ");

void Task_DestroySelf(u8 taskId)
{
    DestroyTask(taskId);
}

static void InitLinkTestBG(u8 paletteNum, u8 bgNum, u8 screenBaseBlock, u8 charBaseBlock)
{
    LoadPalette(sLinkTestDigitPalette, 16 * paletteNum, 32);
    DmaCopy16(3, sLinkTestDigitTiles, BG_CHAR_ADDR(charBaseBlock), 0x220);

    gLinkTestBGInfo.screenBaseBlock = screenBaseBlock;
    gLinkTestBGInfo.paletteNum = paletteNum;

    switch (bgNum)
    {
    case 1:
        REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(screenBaseBlock) | BGCNT_CHARBASE(charBaseBlock);
        break;
    case 2:
        REG_BG2CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(screenBaseBlock) | BGCNT_CHARBASE(charBaseBlock);
        break;
    case 3:
        REG_BG3CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(screenBaseBlock) | BGCNT_CHARBASE(charBaseBlock);
        break;
    }
}

void LoadLinkTestBgGfx(u8 paletteNum, u8 bgNum, u8 screenBaseBlock, u8 charBaseBlock)
{
    LoadPalette(sLinkTestDigitPalette, 16 * paletteNum, 32);
    DmaCopy16(3, sLinkTestDigitTiles, BG_CHAR_ADDR(charBaseBlock), 0x220);

    gLinkTestBGInfo.screenBaseBlock = screenBaseBlock;
    gLinkTestBGInfo.paletteNum = paletteNum;

    *gBGControlRegs[bgNum] = (screenBaseBlock << 8) | (charBaseBlock << 2);
}

void LinkTestScreen(void)
{
    s32 i;
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    SetVBlankCallback(VBlankCB_LinkTest);
    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow((struct WindowTemplate *)&gMenuTextWindowTemplate);
    ResetBlockSend();
    gLinkType = 0x1111;
    OpenLink();
    SeedRng(gMain.vblankCounter1);

    for (i = 0; i < 4; i++)
    {
        // Very weird code, but nothing else seems to match.
        // The following would have the same effect:
        // gSaveBlock2.playerTrainerId[i] = Random();
        u8 *trainerId = gSaveBlock2.playerTrainerId;
        s32 r;
        s32 mask = 0xFF;
        trainerId[i] = (r = Random()) & mask;
    }

    InitLinkTestBG(0, 2, 4, 0);
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_1D_MAP;
    CreateTask(Task_DestroySelf, 0);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    sDummy3 = 0;
    InitLocalLinkPlayer();
    CreateTask(Task_PrintTestData, 0);
    SetMainCallback2(CB2_LinkTest);
}

void SetLocalLinkPlayerId(u8 a1)
{
    gLocalLinkPlayer.id = a1;
}

static void InitLocalLinkPlayer(void)
{
    s32 i;

    gLocalLinkPlayer.trainerId = gSaveBlock2.playerTrainerId[0]
                              | (gSaveBlock2.playerTrainerId[1] << 8)
                              | (gSaveBlock2.playerTrainerId[2] << 16)
                              | (gSaveBlock2.playerTrainerId[3] << 24);

    for (i = 0; i < (s32)sizeof(gLocalLinkPlayer.name); i++)
        gLocalLinkPlayer.name[i] = gSaveBlock2.playerName[i]; // UB: reads past the end of "playerName" array

    gLocalLinkPlayer.gender = gSaveBlock2.playerGender;
    gLocalLinkPlayer.linkType = gLinkType;
    gLocalLinkPlayer.language = gGameLanguage;
    gLocalLinkPlayer.version = gGameVersion + 0x4000;
    gLocalLinkPlayer.lp_field_2 = 0;
}

static void VBlankCB_LinkTest(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void InitLink(void)
{
    s32 i;

    for (i = 0; i < CMD_LENGTH; i++)
        gSendCmd[i] = 0xEFFF;

    gLinkOpen = TRUE;
    EnableSerial();
}

static void Task_TriggerHandshake(u8 taskId)
{
    gTasks[taskId].data[0]++;

    if (gTasks[taskId].data[0] == 5)
    {
        gShouldAdvanceLinkState = 1;
        DestroyTask(taskId);
    }
}

void OpenLink(void)
{
    s32 i;

    ResetSerial();
    InitLink();

    gLinkCallback = LinkCB_RequestPlayerDataExchange;
    gLinkVSyncDisabled = FALSE;
    gLinkErrorOccurred = FALSE;
    gSuppressLinkErrorMessage = FALSE;

    ResetBlockReceivedFlags();

    sDummy1 = 0;
    byte_3002A68 = 0;
    gLinkDummyBool = FALSE;
    gReceivedRemoteLinkPlayers = FALSE;

    for (i = 0; i < 4; i++)
    {
        gLinkPlayerPending[i] = TRUE;
        gReadyToCloseLink[i] = 0;
        gReadyToExitStandby[i] = 0;
    }

    CreateTask(Task_TriggerHandshake, 2);
}

void CloseLink(void)
{
    gReceivedRemoteLinkPlayers = FALSE;
    gLinkOpen = FALSE;
    DisableSerial();
}

static void TestBlockTransfer(u32 a1, u32 a2, u32 a3)
{
    u8 i;
    u8 val;

    if (sLinkTestLastBlockSendPos != sBlockSend.pos)
    {
        LinkTest_PrintHex(sBlockSend.pos, 2, 3, 2);
        sLinkTestLastBlockSendPos = sBlockSend.pos;
    }

    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        if (sLinkTestLastBlockRecvPos[i] != sBlockRecv[i].pos)
        {
            LinkTest_PrintHex(sBlockRecv[i].pos, 2, i + 4, 2);
            sLinkTestLastBlockRecvPos[i] = sBlockRecv[i].pos;
        }
    }

    val = GetBlockReceivedStatus();

    if (val == 0xF)
    {
        for (i = 0; i < MAX_LINK_PLAYERS; i++)
        {
            if ((val >> i) & 1)
            {
                gLinkTestBlockChecksums[i] = LinkTestCalcBlockChecksum(&gBlockRecvBuffer[i], sBlockRecv[i].size);
                ResetBlockReceivedFlag(i);
                if (gLinkTestBlockChecksums[i] != 834)
                {
                    gLinkTestDebugValuesEnabled = FALSE;
                    gLinkTestDummyBool = FALSE;
                }
            }
        }
    }
}

static void LinkTestProcessKeyInput(void)
{
    if (JOY_NEW(A_BUTTON))
        gShouldAdvanceLinkState = 1;
    if (JOY_HELD(B_BUTTON))
        InitBlockSend(gSharedMem + 0x4000, 0x2004);
    if (JOY_NEW(L_BUTTON))
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(2, 0, 0));
    if (JOY_NEW(START_BUTTON))
        SetSuppressLinkErrorMessage(TRUE);
    if (JOY_NEW(R_BUTTON))
        TrySavingData(SAVE_LINK);
    if (JOY_NEW(SELECT_BUTTON))
        SetCloseLinkCallback();
    if (gLinkTestDebugValuesEnabled)
    {
        u32 vblankCounter1 = gMain.vblankCounter1;
        u8 val = gLinkVSyncDisabled;
        if (!gLinkCallback)
            val = gLinkVSyncDisabled | 0x10;
        SetLinkDebugValues(vblankCounter1, val);
    }
}

static void CB2_LinkTest(void)
{
    LinkTestProcessKeyInput();
    TestBlockTransfer(1, 1, 0);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

u16 LinkMain2(u16 *heldKeys)
{
    u8 i;

    if (!gLinkOpen)
        return 0;

    for (i = 0; i < CMD_LENGTH; i++)
        gSendCmd[i] = 0;

    gLinkHeldKeys = *heldKeys;

    if (gLinkStatus & LINK_STAT_CONN_ESTABLISHED)
    {
        ProcessRecvCmds(SIO_MULTI_CNT->id);
        if (gLinkCallback)
            gLinkCallback();
        CheckErrorStatus();
    }

    return gLinkStatus;
}

static void HandleReceiveRemoteLinkPlayer(u8 multiplayerId)
{
    u32 pendingLinkPlayerCount = 0;
    s32 i;

    gLinkPlayerPending[multiplayerId] = FALSE;

    for (i = 0; i < GetLinkPlayerCount_2(); i++)
        pendingLinkPlayerCount += gLinkPlayerPending[i];

    if (pendingLinkPlayerCount == 0 && !gReceivedRemoteLinkPlayers)
        gReceivedRemoteLinkPlayers = TRUE;
}

static void ProcessRecvCmds(u8 unusedParam)
{
    u16 i;
    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        word_3002910[i] = 0;
        if (!gRecvCmds[0][i])
            continue;
        switch (gRecvCmds[0][i])
        {
        case 0x2222:
            InitLocalLinkPlayer();
            gLocalLinkPlayerBlock.linkPlayer = gLocalLinkPlayer;
            memcpy(gLocalLinkPlayerBlock.magic1, sMagic, sizeof(gLocalLinkPlayerBlock.magic1) - 1);
            memcpy(gLocalLinkPlayerBlock.magic2, sMagic, sizeof(gLocalLinkPlayerBlock.magic2) - 1);
            InitBlockSend(&gLocalLinkPlayerBlock, sizeof(gLocalLinkPlayerBlock));
            break;
        case 0x4444:
            word_3002910[i] = gRecvCmds[1][i];
            break;
        case 0x5555:
            byte_3002A68 = 1;
            break;
        case 0x5566:
            byte_3002A68 = 1;
            break;
        case 0xBBBB:
        {
            struct BlockTransfer *blockRecv = &sBlockRecv[i];
            blockRecv->pos = 0;
            blockRecv->size = gRecvCmds[1][i];
            blockRecv->multiplayerId = gRecvCmds[2][i];
            break;
        }
        case 0x8888:
            if (sBlockRecv[i].size > BLOCK_BUFFER_SIZE)
            {
                u16 *buffer = (u16 *)gSharedMem;
                u16 j;
                for (j = 0; j < CMD_LENGTH - 1; j++)
                    buffer[(sBlockRecv[i].pos / 2) + j] = gRecvCmds[j + 1][i];
            }
            else
            {
                u16 j;
                for (j = 0; j < CMD_LENGTH - 1; j++)
                    gBlockRecvBuffer[i][(sBlockRecv[i].pos / 2) + j] = gRecvCmds[j + 1][i];
            }

            sBlockRecv[i].pos += (CMD_LENGTH - 1) * 2;

            if (sBlockRecv[i].pos >= sBlockRecv[i].size)
            {
                if (gLinkPlayerPending[i] == TRUE)
                {
                    struct LinkPlayerBlock *block = (struct LinkPlayerBlock *)&gBlockRecvBuffer[i];
                    struct LinkPlayer *linkPlayer = &gLinkPlayers[i];
                    *linkPlayer = block->linkPlayer;

                    if (strcmp(block->magic1, sMagic)
                     || strcmp(block->magic2, sMagic))
                    {
                        SetMainCallback2(CB2_LinkError);
                    }
                    else
                    {
                        HandleReceiveRemoteLinkPlayer(i);
                    }

                    ConvertInternationalString(gLinkPlayers[i].name, gLinkPlayers[i].language);
                }
                else
                {
                    SetBlockReceivedFlag(i);
#if DEBUG
                    Debug_SetLinkStateFlag(i);
#endif
                }
            }
            break;
        case 0x5FFF:
            gReadyToCloseLink[i] = 1;
            break;
        case 0x2FFE:
            gReadyToExitStandby[i] = 1;
            break;
        case 0xAAAA:
            SetBerryBlenderLinkCallback();
            break;
        case 0xAAAB:
            SetPlayerBerryData(i, gRecvCmds[1][i]);
            break;
        case 0xCCCC:
#if defined(ENGLISH)
            SendBlock(0, sBlockRequestLookupTable[gRecvCmds[1][i]].address, sBlockRequestLookupTable[gRecvCmds[1][i]].size);
#elif defined(GERMAN)
            if (deUnkValue2 == 1)
            {
                deUnkValue2 = 2;
                deUnkValue1 = gRecvCmds[1][i];
            }
            else if (deUnkValue2 == 2 || deUnkValue2 == 3)
            {
                SendBlock(0, sBlockRequestLookupTable[gRecvCmds[1][i]].address, sBlockRequestLookupTable[gRecvCmds[1][i]].size);

                if (deUnkValue2 == 2)
                    deUnkValue2 = 1;
                else
                    deUnkValue2 = 0;
            }
            else
            {
                SendBlock(0, sBlockRequestLookupTable[gRecvCmds[1][i]].address, sBlockRequestLookupTable[gRecvCmds[1][i]].size);
            }
#endif
            break;
        case 0xCAFE:
            word_3002910[i] = gRecvCmds[1][i];
            break;
        }
    }
}

static void BuildSendCmd(u16 code)
{
    switch (code)
    {
    case 0x2222:
        gSendCmd[0] = 0x2222;
        gSendCmd[1] = gLinkType;
        break;
    case 0x2FFE:
        gSendCmd[0] = 0x2FFE;
        break;
    case 0x4444:
        gSendCmd[0] = 0x4444;
        gSendCmd[1] = gMain.heldKeys;
        break;
    case 0x5555:
        gSendCmd[0] = 0x5555;
        break;
    case 0x5566:
        gSendCmd[0] = 0x5566;
        break;
    case 0x6666:
        gSendCmd[0] = 0x6666;
        gSendCmd[1] = 0;
        break;
    case 0x7777:
    {
        u8 i;

        gSendCmd[0] = 0x7777;

        for (i = 0; i < 5; i++)
            gSendCmd[i + 1] = 0xEE;

        break;
    }
    case 0xBBBB:
        gSendCmd[0] = 0xBBBB;
        gSendCmd[1] = sBlockSend.size;
        gSendCmd[2] = sBlockSend.multiplayerId + 128;
        break;
    case 0xAAAA:
        gSendCmd[0] = 0xAAAA;
        break;
    case 0xAAAB:
        gSendCmd[0] = 0xAAAB;
        gSendCmd[1] = gSpecialVar_ItemId;
        break;
    case 0xCCCC:
        gSendCmd[0] = 0xCCCC;
        gSendCmd[1] = gBlockRequestType;
        break;
    case 0x5FFF:
        gSendCmd[0] = 0x5FFF;
        break;
    case 0xCAFE:
        if (!gHeldKeyCodeToSend || gLinkTransferringData)
            break;
        gSendCmd[0] = 0xCAFE;
        gSendCmd[1] = gHeldKeyCodeToSend;
        break;
    }
}

void StartSendingKeysToLink(void)
{
    gLinkCallback = LinkCB_SendHeldKeys;
}

bool32 IsSendingKeysToLink(void)
{
    if (gLinkCallback == LinkCB_SendHeldKeys)
        return TRUE;
    else
        return FALSE;
}

static void LinkCB_SendHeldKeys(void)
{
    if (gReceivedRemoteLinkPlayers == TRUE)
        BuildSendCmd(0xCAFE);
}

void ClearLinkCallback(void)
{
    gLinkCallback = NULL;
}

void ClearLinkCallback_2(void)
{
    gLinkCallback = NULL;
}

u8 GetLinkPlayerCount(void)
{
    return EXTRACT_PLAYER_COUNT(gLinkStatus);
}

void OpenLinkTimed(void)
{
    sPlayerDataExchangeStatus = EXCHANGE_NOT_STARTED;
    gLinkTimeOutCounter = 0;
#if defined(GERMAN)
    ResetBlockSend();
#endif
    OpenLink();
}

u8 GetLinkPlayerDataExchangeStatusTimed(void)
{
    s32 i;
    s32 count = 0;
    u32 index;

    if (gReceivedRemoteLinkPlayers == TRUE)
    {
        if (GetLinkPlayerCount() == 0)
        {
            gLinkErrorOccurred = TRUE;
            CloseLink();
        }

        i = 0;
        index = 0;

        while (i < GetLinkPlayerCount())
        {
            if (gLinkPlayers[index].linkType == gLinkPlayers[0].linkType)
                ++count;
            ++index;
            ++i;
        }

        if (count == GetLinkPlayerCount())
            sPlayerDataExchangeStatus = EXCHANGE_COMPLETE;
        else
            sPlayerDataExchangeStatus = EXCHANGE_IN_PROGRESS;
    }
    else
    {
        gLinkTimeOutCounter++;
        if (gLinkTimeOutCounter > 600)
            sPlayerDataExchangeStatus = EXCHANGE_TIMED_OUT;
    }

    return sPlayerDataExchangeStatus;
}

bool8 IsLinkPlayerDataExchangeComplete(void)
{
    u8 i;
    u8 count = 0;
    u8 isComplete;

    for (i = 0; i < GetLinkPlayerCount(); i++)
        if (gLinkPlayers[i].linkType == gLinkPlayers[0].linkType)
            count++;

    if (count == GetLinkPlayerCount())
    {
        isComplete = TRUE;
        sPlayerDataExchangeStatus = EXCHANGE_COMPLETE;
    }
    else
    {
        isComplete = FALSE;
        sPlayerDataExchangeStatus = EXCHANGE_IN_PROGRESS;
    }

    return isComplete;
}

u32 GetLinkPlayerTrainerId(u8 multiplayerId)
{
    return gLinkPlayers[multiplayerId].trainerId;
}

void ResetLinkPlayers(void)
{
    s32 i;
    for (i = 0; i < 4; i++)
        memset(&gLinkPlayers[i], 0, sizeof(struct LinkPlayer));
}

static void ResetBlockSend(void)
{
    sBlockSend.active = FALSE;
    sBlockSend.pos = 0;
    sBlockSend.size = 0;
    sBlockSend.src = NULL;
}

static bool8 InitBlockSend(void *data, u32 size)
{
    if (sBlockSend.active)
    {
        return FALSE;
    }
    else
    {
        sBlockSend.multiplayerId = GetMultiplayerId();
        sBlockSend.active = TRUE;
        sBlockSend.size = size;
        sBlockSend.pos = 0;

        if (size > BLOCK_BUFFER_SIZE)
        {
            sBlockSend.src = data;
        }
        else
        {
            if (data != gBlockSendBuffer)
                memcpy(gBlockSendBuffer, data, size);
            sBlockSend.src = gBlockSendBuffer;
        }

        BuildSendCmd(0xBBBB);
        gLinkCallback = LinkCB_BlockSendBegin;
        sBlockSendDelayCounter = 0;
        return TRUE;
    }
}

static void LinkCB_BlockSendBegin(void)
{
    sBlockSendDelayCounter++;
    if (sBlockSendDelayCounter > 2)
        gLinkCallback = LinkCB_BlockSend;
}

static void LinkCB_BlockSend(void)
{
    s32 i;
    u8 *buffer = sBlockSend.src;

    gSendCmd[0] = 0x8888;

    for (i = 0; i < CMD_LENGTH - 1; i++)
    {
        s32 offset = sBlockSend.pos + 2 * i;
        gSendCmd[i + 1] = (buffer[offset + 1] << 8) | buffer[offset];
    }

    sBlockSend.pos += (CMD_LENGTH - 1) * 2;

    if (sBlockSend.size <= sBlockSend.pos)
    {
        sBlockSend.active = FALSE;
        gLinkCallback = LinkCB_BlockSendEnd;
    }
}

static void LinkCB_BlockSendEnd(void)
{
    gLinkCallback = NULL;
}

static void LinkCB_BerryBlenderSendHeldKeys(void)
{
    GetMultiplayerId(); // whats the point of calling this if you dont use the multiplayer ID?
    BuildSendCmd(0x4444);
    gBerryBlenderKeySendAttempts++;
}

void SetBerryBlenderLinkCallback(void)
{
    gBerryBlenderKeySendAttempts = 0;
    gLinkCallback = LinkCB_BerryBlenderSendHeldKeys;
}

u32 GetBerryBlenderKeySendAttempts(void)
{
    return gBerryBlenderKeySendAttempts;
}

void SendBerryBlenderNoSpaceForPokeblocks(void)
{
    BuildSendCmd(0xAAAA);
}

u8 GetMultiplayerId(void)
{
    return SIO_MULTI_CNT->id;
}

u8 BitmaskAllOtherLinkPlayers(void)
{
    return ((1 << GetMultiplayerId()) ^ 0xF);
}

bool8 SendBlock(u8 a1, void *a2, u16 a3)
{
    return InitBlockSend(a2, a3);
}

bool8 SendBlockRequest(u8 a1)
{
    if (!gLinkCallback)
    {
        gBlockRequestType = a1;
        BuildSendCmd(0xCCCC);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool8 IsLinkTaskFinished(void)
{
    return gLinkCallback == NULL;
}

u8 GetBlockReceivedStatus(void)
{
    return (gBlockReceived[3] << 3)
         | (gBlockReceived[2] << 2)
         | (gBlockReceived[1] << 1)
         | gBlockReceived[0];
}

static void SetBlockReceivedFlag(u8 multiplayerId)
{
    gBlockReceived[multiplayerId] = TRUE;
}

void ResetBlockReceivedFlags(void)
{
    s32 i;
    for (i = 0; i < 4; i++)
        gBlockReceived[i] = FALSE;
}

void ResetBlockReceivedFlag(u8 multiplayerId)
{
    if (gBlockReceived[multiplayerId])
        gBlockReceived[multiplayerId] = FALSE;
}

void CheckShouldAdvanceLinkState(void)
{
    if ((gLinkStatus & LINK_STAT_MASTER) && EXTRACT_PLAYER_COUNT(gLinkStatus) > 1)
        gShouldAdvanceLinkState = 1;
}

static u16 LinkTestCalcBlockChecksum(void *data, u16 size)
{
    u16 sum = 0;
    u16 i;

    for (i = 0; i < size / 2; i++)
        sum += ((u16 *)data)[i];

    return sum;
}

static void LinkTest_PrintNumChar(u8 tileNum, u8 x, u8 y)
{
    u16 *tilemap = BG_SCREEN_ADDR(gLinkTestBGInfo.screenBaseBlock);
    tilemap[(32 * y) + x] = (gLinkTestBGInfo.paletteNum << 12) | (tileNum + 1);
}

void LinkTest_PrintHex(u32 num, u8 x, u8 y, u8 maxDigits)
{
    u8 buffer[16];
    s32 i;

    for (i = 0; i < maxDigits; i++)
    {
        buffer[i] = num & 0xF;
        num >>= 4;
    }

    for (i = maxDigits - 1; i >= 0; i--)
    {
        LinkTest_PrintNumChar(buffer[i], x, y);
        x++;
    }
}

#if DEBUG

EWRAM_DATA uintptr_t debugCharacterBase = 0;
EWRAM_DATA uintptr_t unk_20238C8 = 0;
EWRAM_DATA u16 *debugTileMap = NULL;
EWRAM_DATA u32 unk_20238D0 = 0;

void debug_sub_8008218(u16 *buffer, u32 arg1, u16 *arg2, u32 arg3)
{
    CpuSet(sLinkTestDigitTiles, buffer, 272);
    debugCharacterBase = (uintptr_t)buffer;
    unk_20238C8 = (uintptr_t)arg1;
    debugTileMap = arg2;
    unk_20238D0 = arg3;
}

void debug_sub_8008264(u32 value, int left, int top, int d, int e)
{
    s32 i;
    u32 buffer[8];

    if (unk_20238D0 == e)
    {
        u32 *ptr;
        u16 *tilemapDest;

        if (d > 8)
            d = 8;
        ptr = buffer;
        for (i = 0; i < d; i++)
        {
            *ptr++ = value & 0xF;
            value >>= 4;
        }

        tilemapDest = (u16 *)debugTileMap + top * 0x20 + left;
        ptr = buffer + d - 1;
        for (i = 0; i < d; i++)
        {
            *tilemapDest = (debugCharacterBase - unk_20238C8) / 32 + *ptr + 1;
            ptr--;
            tilemapDest++;
        }
    }
}

#endif

static void LinkCB_RequestPlayerDataExchange(void)
{
    // Only one request needs to be sent, so only the master sends it.
    if (gLinkStatus & LINK_STAT_MASTER)
        BuildSendCmd(0x2222);
    gLinkCallback = NULL;
}

void Task_PrintTestData(u8 taskId)
{
    s32 i;

    LinkTest_PrintHex(gShouldAdvanceLinkState, 2, 1, 2);
    LinkTest_PrintHex(gLinkStatus, 15, 1, 8);
    LinkTest_PrintHex(gLink.state, 2, 10, 2);
    LinkTest_PrintHex(EXTRACT_PLAYER_COUNT(gLinkStatus), 15, 10, 2);
    LinkTest_PrintHex(GetMultiplayerId(), 15, 12, 2);
    LinkTest_PrintHex(gLastSendQueueCount, 25, 1, 2);
    LinkTest_PrintHex(gLastRecvQueueCount, 25, 2, 2);
    LinkTest_PrintHex(GetBlockReceivedStatus(), 15, 5, 2);
    LinkTest_PrintHex(gLinkDebugValue1, 2, 12, 8);
    LinkTest_PrintHex(gLinkDebugValue2, 2, 13, 8);
    LinkTest_PrintHex(GetSioMultiSI(), 25, 5, 1);
    LinkTest_PrintHex(IsSioMultiMaster(), 25, 6, 1);
    LinkTest_PrintHex(IsLinkConnectionEstablished(), 25, 7, 1);
    LinkTest_PrintHex(HasLinkErrorOccurred(), 25, 8, 1);

    for (i = 0; i < MAX_LINK_PLAYERS; i++)
        LinkTest_PrintHex(gLinkTestBlockChecksums[i], 10, 4 + i, 4);
}

void SetLinkDebugValues(u32 value1, u32 value2)
{
    gLinkDebugValue1 = value1;
    gLinkDebugValue2 = value2;
}

u8 GetSavedLinkPlayerCountAsBitFlags(void)
{
    u8 result = 0;
    s32 i;

    for (i = 0; i < gSavedLinkPlayerCount; i++)
        result |= (1 << i);

    return result;
}

void SaveLinkPlayers(u8 playerCount)
{
    s32 i;

    gSavedLinkPlayerCount = playerCount;
    gSavedMultiplayerId = GetMultiplayerId();

    for (i = 0; i < 4; i++)
        gSavedLinkPlayers[i] = gLinkPlayers[i];
}

u8 GetSavedPlayerCount(void)
{
    return gSavedLinkPlayerCount;
}

u8 GetSavedMultiplayerId(void)
{
    return gSavedMultiplayerId;
}

bool8 DoSavedLinkPlayerTrainerIdsMatch(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < gSavedLinkPlayerCount; i++)
        if (gLinkPlayers[i].trainerId == gSavedLinkPlayers[i].trainerId)
            count++;

    if (count == gSavedLinkPlayerCount)
        return TRUE;
    else
        return FALSE;
}

void CheckLinkPlayersMatchSaved(void)
{
    u8 i;

    for (i = 0; i < gSavedLinkPlayerCount; i++)
    {
        if (gSavedLinkPlayers[i].trainerId != gLinkPlayers[i].trainerId
         || StringCompareWithoutExtCtrlCodes(gSavedLinkPlayers[i].name, gLinkPlayers[i].name))
        {
            gLinkErrorOccurred = TRUE;
            CloseLink();
            SetMainCallback2(CB2_LinkError);
        }
    }
}

void ResetLinkPlayerCount(void)
{
    gSavedLinkPlayerCount = 0;
    gSavedMultiplayerId = 0;
}

u8 GetLinkPlayerCount_2(void)
{
    return EXTRACT_PLAYER_COUNT(gLinkStatus);
}

bool8 IsLinkMaster(void)
{
    return EXTRACT_MASTER(gLinkStatus);
}

u8 GetDummy2(void)
{
    return sDummy2;
}

void SetCloseLinkCallback(void)
{
    if (!gLinkCallback)
    {
        gLinkCallback = LinkCB_ReadyCloseLink;
        gLinkDummyBool = FALSE;
    }
}

static void LinkCB_ReadyCloseLink(void)
{
    if (gLastRecvQueueCount == 0)
    {
        BuildSendCmd(0x5FFF);
        gLinkCallback = LinkCB_WaitCloseLink;
    }
}

static void LinkCB_WaitCloseLink(void)
{
    s32 i;
    s32 totalCount = GetLinkPlayerCount();
    s32 count = 0;

    for (i = 0; i < totalCount; i++)
        if (gReadyToCloseLink[i])
            count++;

    if (count == totalCount)
    {
        gBattleTypeFlags &= ~BATTLE_TYPE_20;
        gLinkVSyncDisabled = TRUE;
        CloseLink();
        gLinkCallback = NULL;
        gLinkDummyBool = TRUE;
    }
}

static void LinkCB_WaitCloseLinkWithJP(void)
{
    s32 i;
    s32 totalCount = GetLinkPlayerCount();
    s32 count = 0;

    for (i = 0; i < totalCount; i++)
    {
        if (gLinkPlayers[i].language == 1)
            count++;
        else if (gReadyToCloseLink[i])
            count++;
    }

    if (count == totalCount)
    {
        gBattleTypeFlags &= ~BATTLE_TYPE_20;
        gLinkVSyncDisabled = TRUE;
        CloseLink();
        gLinkCallback = 0;
        gLinkDummyBool = TRUE;
    }
}

static void LinkCB_ReadyCloseLinkWithJP(void)
{
    if (gLastRecvQueueCount == 0)
    {
        BuildSendCmd(0x5FFF);
        gLinkCallback = LinkCB_WaitCloseLinkWithJP;
    }
}

void SetCloseLinkCallbackHandleJP(void)
{
    if (!gLinkCallback)
    {
        gLinkCallback = LinkCB_ReadyCloseLinkWithJP;
        gLinkDummyBool = FALSE;
    }
}

void SetLinkStandbyCallback(void)
{
    if (!gLinkCallback)
        gLinkCallback = LinkCB_Standby;
    gLinkDummyBool = FALSE;
}

static void LinkCB_Standby(void)
{
    if (gLastRecvQueueCount == 0)
    {
        BuildSendCmd(0x2FFE);
        gLinkCallback = LinkCB_StandbyForAll;
    }
}

static void LinkCB_StandbyForAll(void)
{
    u8 totalCount = GetLinkPlayerCount();
    u8 count = 0;

    while (count < totalCount && gReadyToExitStandby[count])
        count++;

    if (count == totalCount)
    {
        u8 i;
        for (i = 0; i < 4; i++)
            gReadyToExitStandby[i] = 0;
        gLinkCallback = NULL;
    }
}

static void CheckErrorStatus(void)
{
    if (gLinkOpen)
    {
        if (gLinkStatus & LINK_STAT_ERRORS)
        {
            if (!gSuppressLinkErrorMessage)
            {
                sErrorLinkStatus = gLinkStatus;
                sErrorLastRecvQueueCount = gLastRecvQueueCount;
                sErrorLastSendQueueCount = gLastSendQueueCount;
                SetMainCallback2(CB2_LinkError);
            }
            gLinkErrorOccurred = TRUE;
            CloseLink();
        }
    }
}

void CB2_LinkError(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetPaletteFadeControl();
    FillPalette(0, 0, 2);
    ResetTasks();
    SetVBlankCallback(VBlankCB_LinkTest);
    Text_LoadWindowTemplate(&gWindowTemplate_81E7198);
    InitMenuWindow((struct WindowTemplate *)&gWindowTemplate_81E7198);
    Menu_EraseScreen();
    REG_BLDALPHA = 0;
    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON;
    gSoftResetDisabled = FALSE;
    CreateTask(Task_DestroySelf, 0);
    StopMapMusic();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    SetMainCallback2(CB2_PrintErrorMessage);
}

void CB2_PrintErrorMessage(void)
{
    u8 array[32] __attribute__((unused)); // unused
    u8 array2[32] __attribute__((unused)); // unused

    switch (gMain.state)
    {
    case 0:
        Menu_PrintTextPixelCoords(gMultiText_LinkError, 20, 56, 1);
#if DEBUG
        StringCopy(array, sColorCodes);

        ConvertIntToHexStringN(array2, sErrorLinkStatus, STR_CONV_MODE_LEADING_ZEROS, 8);
        StringAppend(array, array2);

        StringAppend(array, sEmptyString);

        ConvertIntToHexStringN(array2, sErrorLastSendQueueCount, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringAppend(array, array2);

        StringAppend(array, sEmptyString);

        ConvertIntToHexStringN(array2, sErrorLastRecvQueueCount, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringAppend(array, array2);

        Menu_PrintText(array, 2, 15);
#endif
        break;
    case 30:
    case 60:
        PlaySE(SE_BOO);
        break;
    case 90:
        PlaySE(SE_BOO);
        break;
    }

    if (gMain.state != 200)
        gMain.state++;
}

u8 GetSioMultiSI(void)
{
    return (REG_SIOCNT >> SIO_MULTI_SI_SHIFT) & SIO_MULTI_SI_MASK;
}

static bool8 IsSioMultiMaster(void)
{
    bool8 isMaster = FALSE;

    if ((REG_SIOCNT & SIO_MULTI_SD) && !(REG_SIOCNT & SIO_MULTI_SI))
        isMaster = TRUE;

    return isMaster;
}

bool8 IsLinkConnectionEstablished(void)
{
    return EXTRACT_CONN_ESTABLISHED(gLinkStatus);
}

void SetSuppressLinkErrorMessage(bool8 value)
{
    gSuppressLinkErrorMessage = value;
}

bool8 HasLinkErrorOccurred(void)
{
    return gLinkErrorOccurred;
}

static void DisableSerial(void)
{
    gLinkSavedIme = REG_IME;
    REG_IME = 0;
    REG_IE &= ~(INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL);
    REG_IME = gLinkSavedIme;

    REG_SIOCNT = 0;
    REG_TM3CNT_H = 0;
    REG_IF = INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL;

    CpuFill32(0, &gLink, sizeof(gLink));
}

static void EnableSerial(void)
{
    gLinkSavedIme = REG_IME;
    REG_IME = 0;
    REG_IE &= ~(INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL);
    REG_IME = gLinkSavedIme;

    REG_RCNT = 0;

    REG_SIOCNT = SIO_MULTI_MODE;
    REG_SIOCNT |= SIO_INTR_ENABLE | SIO_115200_BPS;

    gLinkSavedIme = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_SERIAL;
    REG_IME = gLinkSavedIme;

    REG_SIOMLT_SEND = 0;

    CpuFill32(0, &gLink, sizeof(gLink));

    sNumVBlanksWithoutSerialIntr = 0;
    sSendNonzeroCheck = 0;
    sRecvNonzeroCheck = 0;
    sChecksumAvailable = FALSE;
    sHandshakePlayerCount = 0;
    gLastSendQueueCount = 0;
    gLastRecvQueueCount = 0;
}

void ResetSerial(void)
{
    EnableSerial();
    DisableSerial();
}

u32 LinkMain1(u8 *shouldAdvanceLinkState, u16 *sendCmd, u16 recvCmds[CMD_LENGTH][MAX_LINK_PLAYERS])
{
    u32 retVal;
    u32 retVal2;

    switch (gLink.state)
    {
    case LINK_STATE_START0:
        DisableSerial();
        gLink.state = LINK_STATE_START1;
        break;
    case LINK_STATE_START1:
        if (*shouldAdvanceLinkState == 1)
        {
            EnableSerial();
            gLink.state = LINK_STATE_HANDSHAKE;
        }
        break;
    case LINK_STATE_HANDSHAKE:
        switch (*shouldAdvanceLinkState)
        {
        case 1:
            if (gLink.isMaster == 8 && gLink.playerCount > 1)
                gLink.handshakeAsMaster = TRUE;
            break;
        case 2:
            gLink.state = LINK_STATE_START0;
            REG_SIOMLT_SEND = 0;
            break;
        default:
            CheckMasterOrSlave();
            break;
        }
        break;
    case LINK_STATE_INIT_TIMER:
        InitTimer();
        gLink.state = LINK_STATE_CONN_ESTABLISHED;
    case LINK_STATE_CONN_ESTABLISHED:
        EnqueueSendCmd(sendCmd);
        DequeueRecvCmds(recvCmds);
        break;
    }

    *shouldAdvanceLinkState = 0;

    retVal = gLink.localId;
    retVal |= (gLink.playerCount << 2);

    if (gLink.isMaster == 8)
        retVal |= 0x20;

    {
        u32 receivedNothing = gLink.receivedNothing << 8;
        u32 link_field_F = gLink.link_field_F << 9;
        u32 hardwareError = gLink.hardwareError << 12;
        u32 badChecksum = gLink.badChecksum << 13;
        u32 queueFull = gLink.queueFull << 14;
        u32 val;

        if (gLink.state == LINK_STATE_CONN_ESTABLISHED)
        {
            val = 0x40;
            val |= receivedNothing;
            val |= retVal;
            val |= link_field_F;
            val |= hardwareError;
            val |= badChecksum;
            val |= queueFull;
        }
        else
        {
            val = retVal;
            val |= receivedNothing;
            val |= link_field_F;
            val |= hardwareError;
            val |= badChecksum;
            val |= queueFull;
        }

        retVal = val;
    }

    if (gLink.lag == LAG_MASTER)
        retVal |= 0x10000;

    if (gLink.localId > 3)
        retVal |= 0x20000;

    retVal2 = retVal;
    if (gLink.lag == LAG_SLAVE)
        retVal2 |= 0x40000;

    return retVal2;
}

static void CheckMasterOrSlave(void)
{
    u32 terminals = *(u32 *)REG_ADDR_SIOCNT & (SIO_MULTI_SD | SIO_MULTI_SI);

    if (terminals == SIO_MULTI_SD && !gLink.localId)
        gLink.isMaster = 8;
    else
        gLink.isMaster = 0;
}

static void InitTimer(void)
{
    if (gLink.isMaster)
    {
        REG_TM3CNT_L = 65339;
        REG_TM3CNT_H = TIMER_INTR_ENABLE | TIMER_64CLK;

        gLinkSavedIme = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_TIMER3;
        REG_IME = gLinkSavedIme;
    }
}

static void EnqueueSendCmd(u16 *sendCmd)
{
    gLinkSavedIme = REG_IME;
    REG_IME = 0;

    if (gLink.sendQueue.count < QUEUE_CAPACITY)
    {
        u8 i;
        u8 offset = gLink.sendQueue.pos + gLink.sendQueue.count;

        if (offset >= QUEUE_CAPACITY)
            offset -= QUEUE_CAPACITY;

        for (i = 0; i < CMD_LENGTH; i++)
        {
            sSendNonzeroCheck |= *sendCmd;
            gLink.sendQueue.data[i][offset] = *sendCmd;
            *sendCmd = 0;
            sendCmd++;
        }
    }
    else
    {
        gLink.queueFull = QUEUE_FULL_SEND;
    }

    if (sSendNonzeroCheck)
    {
        gLink.sendQueue.count++;
        sSendNonzeroCheck = 0;
    }

    REG_IME = gLinkSavedIme;
    gLastSendQueueCount = gLink.sendQueue.count;
}

void DequeueRecvCmds(u16 recvCmds[CMD_LENGTH][MAX_LINK_PLAYERS])
{
    u8 i;
    u8 j;

    gLinkSavedIme = REG_IME;
    REG_IME = 0;

    if (gLink.recvQueue.count == 0)
    {
        for (i = 0; i < CMD_LENGTH; i++)
            for (j = 0; j < gLink.playerCount; j++)
                recvCmds[i][j] = 0;

        gLink.receivedNothing = TRUE;
    }
    else
    {
        for (i = 0; i < CMD_LENGTH; i++)
            for (j = 0; j < gLink.playerCount; j++)
                recvCmds[i][j] = gLink.recvQueue.data[j][i][gLink.recvQueue.pos];

        gLink.recvQueue.count--;
        gLink.recvQueue.pos++;

        if (gLink.recvQueue.pos >= QUEUE_CAPACITY)
            gLink.recvQueue.pos = 0;

        gLink.receivedNothing = FALSE;
    }

    REG_IME = gLinkSavedIme;
}

void LinkVSync(void)
{
    if (gLink.isMaster)
    {
        switch (gLink.state)
        {
        case LINK_STATE_HANDSHAKE:
            StartTransfer();
            break;
        case LINK_STATE_CONN_ESTABLISHED:
            if (gLink.serialIntrCounter > 8)
            {
                if (gLink.lag == LAG_MASTER)
                    return;
                gLink.serialIntrCounter = 0;
            }
            else if (gLink.hardwareError != TRUE)
            {
                gLink.lag = LAG_MASTER;
                return;
            }
            StartTransfer();
            break;
        }
    }
    else if (gLink.state == LINK_STATE_CONN_ESTABLISHED
          || gLink.state == LINK_STATE_HANDSHAKE)
    {
        sNumVBlanksWithoutSerialIntr++;

        if (sNumVBlanksWithoutSerialIntr > 10)
        {
            if (gLink.state == LINK_STATE_CONN_ESTABLISHED)
                gLink.lag = LAG_SLAVE;

            if (gLink.state == LINK_STATE_HANDSHAKE)
            {
                gLink.playerCount = 0;
                gLink.link_field_F = 0;
            }
        }
    }
}

void Timer3Intr(void)
{
    StopTimer();
    StartTransfer();
}

void SerialCB(void)
{
    gLink.localId = SIO_MULTI_CNT->id;

    switch (gLink.state)
    {
    case LINK_STATE_CONN_ESTABLISHED:
        gLink.hardwareError = SIO_MULTI_CNT->error;
        DoRecv();
        DoSend();
        SendRecvDone();
        break;
    case LINK_STATE_HANDSHAKE:
        if (DoHandshake())
        {
            if (gLink.isMaster)
            {
                gLink.state = LINK_STATE_INIT_TIMER;
                gLink.serialIntrCounter = 8;
            }
            else
            {
                gLink.state = LINK_STATE_CONN_ESTABLISHED;
            }
        }
        break;
    }

    gLink.serialIntrCounter++;
    sNumVBlanksWithoutSerialIntr = 0;

    if (gLink.serialIntrCounter == 8)
        gLastRecvQueueCount = gLink.recvQueue.count;
}

static void StartTransfer(void)
{
    REG_SIOCNT |= SIO_START;
}

static bool8 DoHandshake(void)
{
    u8 i;
    u8 playerCount = 0;
    u16 minRecv = 0xFFFF;

    if (gLink.handshakeAsMaster == TRUE)
        REG_SIOMLT_SEND = MASTER_HANDSHAKE;
    else
        REG_SIOMLT_SEND = SLAVE_HANDSHAKE;

    *(u64 *)&gLink.tempRecvBuffer[0] = REG_SIOMLT_RECV;
    REG_SIOMLT_RECV = 0;

    gLink.handshakeAsMaster = FALSE;

    for (i = 0; i < 4; i++)
    {
        if ((gLink.tempRecvBuffer[i] & ~0x3) == SLAVE_HANDSHAKE
         || gLink.tempRecvBuffer[i] == MASTER_HANDSHAKE)
        {
            playerCount++;

            if (minRecv > gLink.tempRecvBuffer[i] && gLink.tempRecvBuffer[i] != 0)
                minRecv = gLink.tempRecvBuffer[i];
        }
        else
        {
            if (gLink.tempRecvBuffer[i] != 0xFFFF)
                playerCount = 0;
            break;
        }
    }

    gLink.playerCount = playerCount;

    // The handshake is successful when:
    // 1. There are multiple players.
    // 2. The number of players agrees with the last attempt.
    // 3. Player no. 0 is identifying as the master.
    if (gLink.playerCount > 1
     && gLink.playerCount == sHandshakePlayerCount
     && gLink.tempRecvBuffer[0] == MASTER_HANDSHAKE)
        return TRUE;

    if (gLink.playerCount > 1)
        gLink.link_field_F = (minRecv & 3) + 1;
    else
        gLink.link_field_F = 0;

    sHandshakePlayerCount = gLink.playerCount;

    return FALSE;
}

static void DoRecv(void)
{
    u16 recvBuffer[4];
    u8 i;

    *(u64 *)&recvBuffer[0] = REG_SIOMLT_RECV;

    if (gLink.sendCmdIndex == 0)
    {
        for (i = 0; i < gLink.playerCount; i++)
            if (gLink.checksum != recvBuffer[i] && sChecksumAvailable)
                    gLink.badChecksum = TRUE;

        gLink.checksum = 0;
        sChecksumAvailable = TRUE;
    }
    else
    {
        u8 index = gLink.recvQueue.pos + gLink.recvQueue.count;

        if (index >= QUEUE_CAPACITY)
            index -= QUEUE_CAPACITY;

        if (gLink.recvQueue.count < QUEUE_CAPACITY)
        {
            for (i = 0; i < gLink.playerCount; i++)
            {
                gLink.checksum += recvBuffer[i];
                sRecvNonzeroCheck |= recvBuffer[i];
                gLink.recvQueue.data[i][gLink.recvCmdIndex][index] = recvBuffer[i];
            }
        }
        else
        {
            gLink.queueFull = QUEUE_FULL_RECV;
        }

        gLink.recvCmdIndex++;

        if (gLink.recvCmdIndex == CMD_LENGTH && sRecvNonzeroCheck)
        {
            gLink.recvQueue.count++;
            sRecvNonzeroCheck = 0;
        }
    }
}

static void DoSend(void)
{
    if (gLink.sendCmdIndex == CMD_LENGTH)
    {
        REG_SIOMLT_SEND = gLink.checksum;

        if (!sSendBufferEmpty)
        {
            gLink.sendQueue.count--;
            gLink.sendQueue.pos++;

            if (gLink.sendQueue.pos >= QUEUE_CAPACITY)
                gLink.sendQueue.pos = 0;
        }
        else
        {
            sSendBufferEmpty = FALSE;
        }
    }
    else
    {
        if (!sSendBufferEmpty && gLink.sendQueue.count == 0)
            sSendBufferEmpty = TRUE;

        if (sSendBufferEmpty)
            REG_SIOMLT_SEND = 0;
        else
            REG_SIOMLT_SEND = gLink.sendQueue.data[gLink.sendCmdIndex][gLink.sendQueue.pos];

        gLink.sendCmdIndex++;
    }
}

static void StopTimer(void)
{
    if (gLink.isMaster)
    {
        REG_TM3CNT_H &= ~TIMER_ENABLE;
        REG_TM3CNT_L = 65339;
    }
}

static void SendRecvDone(void)
{
    if (gLink.recvCmdIndex == CMD_LENGTH)
    {
        gLink.sendCmdIndex = 0;
        gLink.recvCmdIndex = 0;
    }
    else if (gLink.isMaster)
    {
        REG_TM3CNT_H |= TIMER_ENABLE;
    }
}

void ResetSendBuffer(void)
{
    u8 i;
    u8 j;

    gLink.sendQueue.count = 0;
    gLink.sendQueue.pos = 0;

    for (i = 0; i < CMD_LENGTH; i++)
        for (j = 0; j < QUEUE_CAPACITY; j++)
            gLink.sendQueue.data[i][j] = 0xEFFF;
}

void ResetRecvBuffer(void)
{
    u8 i;
    u8 j;
    u8 k;

    gLink.recvQueue.count = 0;
    gLink.recvQueue.pos = 0;

    for (i = 0; i < 4; i++)
        for (j = 0; j < CMD_LENGTH; j++)
            for (k = 0; k < QUEUE_CAPACITY; k++)
                gLink.recvQueue.data[i][j][k] = 0xEFFF;
}
