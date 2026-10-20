#ifndef GUARD_LINK_H
#define GUARD_LINK_H

#define MAX_LINK_PLAYERS 4
#define CMD_LENGTH 8
#define QUEUE_CAPACITY 50
#define BLOCK_BUFFER_SIZE 0x100

#define LINK_SLAVE 0
#define LINK_MASTER 8

#define LINK_STAT_LOCAL_ID               0x00000003
#define LINK_STAT_PLAYER_COUNT           0x0000001C
#define LINK_STAT_PLAYER_COUNT_SHIFT     2
#define LINK_STAT_MASTER                 0x00000020
#define LINK_STAT_MASTER_SHIFT           5
#define LINK_STAT_CONN_ESTABLISHED       0x00000040
#define LINK_STAT_CONN_ESTABLISHED_SHIFT 6
#define LINK_STAT_RECEIVED_NOTHING       0x00000100
#define LINK_STAT_RECEIVED_NOTHING_SHIFT 8
#define LINK_STAT_ERRORS                 0x0007F000

#define LINK_STAT_UNK_FLAG_9_SHIFT       9
#define LINK_STAT_ERROR_HARDWARE_SHIFT   12
#define LINK_STAT_ERROR_CHECKSUM_SHIFT   13
#define LINK_STAT_ERROR_QUEUE_FULL_SHIFT 14
#define LINK_STAT_ERROR_LAG_MASTER       0x00010000
#define LINK_STAT_ERROR_INVALID_ID       0x00020000
#define LINK_STAT_ERROR_LAG_SLAVE        0x00040000

#define EXTRACT_PLAYER_COUNT(status) \
(((status) & LINK_STAT_PLAYER_COUNT) >> LINK_STAT_PLAYER_COUNT_SHIFT)
#define EXTRACT_MASTER(status) \
(((status) >> LINK_STAT_MASTER_SHIFT) & 1)
#define EXTRACT_CONN_ESTABLISHED(status) \
(((status) >> LINK_STAT_CONN_ESTABLISHED_SHIFT) & 1)
#define EXTRACT_RECEIVED_NOTHING(status) \
(((status) >> LINK_STAT_RECEIVED_NOTHING_SHIFT) & 1)

#define LINKCMD_BLENDER_STOP            0x1111
#define LINKCMD_SEND_LINK_TYPE          0x2222
#define LINKCMD_BLENDER_SCORE_MISS      0x2345
#define LINKCMD_READY_EXIT_STANDBY      0x2FFE
#define LINKCMD_SEND_PACKET             0x2FFF
#define LINKCMD_BLENDER_SEND_KEYS       0x4444
#define LINKCMD_BLENDER_SCORE_BEST      0x4523
#define LINKCMD_BLENDER_SCORE_GOOD      0x5432
#define LINKCMD_DUMMY_1                 0x5555
#define LINKCMD_DUMMY_2                 0x5566
#define LINKCMD_READY_CLOSE_LINK        0x5FFF
#define LINKCMD_SEND_EMPTY              0x6666
#define LINKCMD_SEND_0xEE               0x7777
#define LINKCMD_BLENDER_PLAY_AGAIN      0x7779
#define LINKCMD_CONT_BLOCK              0x8888
#define LINKCMD_BLENDER_NO_BERRIES      0x9999
#define LINKCMD_BLENDER_NO_PBLOCK_SPACE 0xAAAA
#define LINKCMD_SEND_ITEM               0xAAAB
#define LINKCMD_READY_TO_TRADE          0xAABB
#define LINKCMD_READY_FINISH_TRADE      0xABCD
#define LINKCMD_INIT_BLOCK              0xBBBB
#define LINKCMD_READY_CANCEL_TRADE      0xBBCC
#define LINKCMD_SEND_HELD_KEYS          0xCAFE
#define LINKCMD_SEND_BLOCK_REQ          0xCCCC
#define LINKCMD_START_TRADE             0xCCDD
#define LINKCMD_CONFIRM_FINISH_TRADE    0xDCBA
#define LINKCMD_SET_MONS_TO_TRADE       0xDDDD
#define LINKCMD_PLAYER_CANCEL_TRADE     0xDDEE
#define LINKCMD_REQUEST_CANCEL          0xEEAA
#define LINKCMD_BOTH_CANCEL_TRADE       0xEEBB
#define LINKCMD_PARTNER_CANCEL_TRADE    0xEECC
#define LINKCMD_NONE                    0xEFFF

#define LINKTYPE_TRADE                 0x1111
#define LINKTYPE_TRADE_CONNECTING      0x1122
#define LINKTYPE_TRADE_DISCONNECTED    0x1144
#define LINKTYPE_BATTLE                0x2211
#define LINKTYPE_BERRY_BLENDER         0x4422

enum
{
    BLOCK_REQ_SIZE_NONE, // Identical to 200
    BLOCK_REQ_SIZE_200,
    BLOCK_REQ_SIZE_100,
    BLOCK_REQ_SIZE_220,
    BLOCK_REQ_SIZE_40,
};

#define MASTER_HANDSHAKE 0x8FFF
#define SLAVE_HANDSHAKE  0xB9A0

#define SIO_MULTI_CNT ((struct SioMultiCnt *)REG_ADDR_SIOCNT)

enum
{
    LINK_STATE_START0,
    LINK_STATE_START1,
    LINK_STATE_HANDSHAKE,
    LINK_STATE_INIT_TIMER,
    LINK_STATE_CONN_ESTABLISHED,
};

enum
{
    EXCHANGE_NOT_STARTED,
    EXCHANGE_COMPLETE,
    EXCHANGE_TIMED_OUT,
    EXCHANGE_DIFF_SELECTIONS,
};

enum
{
    QUEUE_FULL_NONE,
    QUEUE_FULL_SEND,
    QUEUE_FULL_RECV,
};

enum
{
    LAG_NONE,
    LAG_MASTER,
    LAG_SLAVE,
};

struct LinkPlayer
{
    /* 0x00 */ u16 version;
    /* 0x02 */ u16 lp_field_2;
    /* 0x04 */ u32 trainerId;
    /* 0x08 */ u8 name[11];
    /* 0x13 */ u8 gender;
    /* 0x14 */ u32 linkType;
    /* 0x18 */ u16 id;
    /* 0x1A */ u16 language;
};

struct LinkPlayerBlock
{
    u8 magic1[16];
    struct LinkPlayer linkPlayer;
    u8 magic2[16];
};

// circular queues

struct SendQueue
{
    u16 data[CMD_LENGTH][QUEUE_CAPACITY];
    u8 pos;
    u8 count;
};

struct RecvQueue
{
    u16 data[MAX_LINK_PLAYERS][CMD_LENGTH][QUEUE_CAPACITY];
    u8 pos;
    u8 count;
};

struct Link
{
    u8 isMaster; // 0: slave, 8: master
    u8 state;
    u8 localId; // local multi-player ID
    u8 playerCount;
    u16 handshakeBuffer[MAX_LINK_PLAYERS];
    bool8 receivedNothing;
    s8 serialIntrCounter;
    bool8 handshakeAsMaster;
    u8 link_field_F;

    // error conditions
    bool8 hardwareError; // hardware reported an error
    bool8 badChecksum; // checksum didn't match between devices
    u8 queueFull; // send or recv queue out of space
    u8 lag; // connection is lagging

    u16 checksum;

    u8 sendCmdIndex;
    u8 recvCmdIndex;

    struct SendQueue sendQueue;
    struct RecvQueue recvQueue;
};

struct BlockRequest {
    void * address;
    u32 size;
};

extern const struct BlockRequest sBlockRequests[5];

extern struct Link gLink;
extern u32 gLinkDebugSeed;
extern u32 gLinkDebugFlags;
extern bool8 gLinkErrorOccurred;
extern u8 gLinkDummy2;
extern bool8 gBlockReceivedStatus[MAX_LINK_PLAYERS];
extern u16 gLinkHeldKeys;
extern bool8 gReadyToExitStandby[MAX_LINK_PLAYERS];
extern bool8 gReadyToCloseLink[MAX_LINK_PLAYERS];
extern bool8 gSuppressLinkErrorMessage;
extern u8 gSavedLinkPlayerCount;
extern u8 gSavedMultiplayerId;
extern struct LinkTestBGInfo gLinkTestBGInfo;
extern u16 gLinkTestBlockChecksums[MAX_LINK_PLAYERS];
extern u8 gBlockRequestType;
extern u8 gLastSendQueueCount;
extern u8 gLastRecvQueueCount;
extern u16 gLinkSavedIme;
extern struct LinkPlayer gLocalLinkPlayer;
extern u32 gBerryBlenderKeySendAttempts;
extern u16 gRecvCmds[CMD_LENGTH][MAX_LINK_PLAYERS];
// TODO: Why is gBlockSendBuffer a u8 array, while gBlockRecvBuffer is a u16 array?
extern u8 gBlockSendBuffer[BLOCK_BUFFER_SIZE];
extern u16 gBlockRecvBuffer[MAX_LINK_PLAYERS][BLOCK_BUFFER_SIZE / sizeof(u16)];
extern u16 gLinkType;
extern u32 gLinkStatus;
extern u16 gSendCmd[CMD_LENGTH];
extern u8 gShouldAdvanceLinkState;
#if GERMAN
extern u8 deUnkValue1;
extern u8 deUnkValue2;
#endif
extern struct LinkPlayer gLinkPlayers[];
extern u16 gLinkPartnersHeldKeys[];
extern struct LinkPlayerBlock gLocalLinkPlayerBlock;
extern bool8 gReceivedRemoteLinkPlayers;
extern bool8 gLinkOpen;
extern bool8 gRemoteLinkPlayersNotReceived[MAX_LINK_PLAYERS];
extern void (*gLinkCallback)(void);
extern u8 gUnknown_Debug_30030E0;

void Task_DestroySelf(u8 taskId);
void SetLocalLinkPlayerId(u8 playerId);
void OpenLink(void);
void CloseLink(void);
u16 LinkMain2(u16 *heldKeys);
void StartSendingKeysToLink(void);
bool32 IsSendingKeysToLink(void);
void ClearLinkCallback(void);
void ClearLinkCallback_2(void);
u8 GetLinkPlayerCount(void);
void OpenLinkTimed(void);
u8 GetLinkPlayerDataExchangeStatusTimed(void);
bool8 IsLinkPlayerDataExchangeComplete(void);
u32 GetLinkPlayerTrainerId(u8 multiplayerId);
void ResetLinkPlayers(void);
void SetBerryBlenderLinkCallback(void);
void SendBerryBlenderNoSpaceForPokeblocks(void);
u8 GetMultiplayerId(void);
u8 BitmaskAllOtherLinkPlayers(void);
bool8 SendBlock(u8 unused, void *src, u16 size);
bool8 SendBlockRequest(u8 blockReqType);
bool8 IsLinkTaskFinished(void);
u8 GetBlockReceivedStatus(void);
void ResetBlockReceivedFlags(void);
void ResetBlockReceivedFlag(u8 multiplayerId);
void CheckShouldAdvanceLinkState(void);
void SetLinkDebugValues(u32 seed, u32 flags);
u8 GetSavedLinkPlayerCountAsBitFlags(void);
void SaveLinkPlayers(u8 playerCount);
u8 GetSavedPlayerCount(void);
u8 GetSavedMultiplayerId(void);
bool8 DoesLinkPlayerCountMatchSaved(void);
void CheckLinkPlayersMatchSaved(void);
void ResetLinkPlayerCount(void);
u8 GetLinkPlayerCount_2(void);
bool8 IsLinkMaster(void);
void SetCloseLinkCallback(void);
void SetCloseLinkCallbackHandleJP(void);
void SetLinkStandbyCallback(void);
void CB2_LinkError(void);
bool8 GetSioMultiSI(void);
bool8 IsLinkConnectionEstablished(void);
void SetSuppressLinkErrorMessage(bool8 flag);
bool8 HasLinkErrorOccurred(void);
void ResetSerial(void);
u32 LinkMain1(u8 *shouldAdvanceLinkState, u16 *sendCmd, u16 recvCmds[CMD_LENGTH][MAX_LINK_PLAYERS]);
void LinkVSync(void);
void Timer3Intr(void);
void SerialCB(void);
void LinkTestScreen(void);
void debug_sub_8008218(u16 *buffer, u32 arg1, u16 *arg2, u32 arg3);
void debug_sub_8008264(u32 value, int left, int top, int r3, int sp0);
u32 GetBerryBlenderKeySendAttempts(void);
void LinkTest_PrintHex(u32 num, u8 x, u8 y, u8 maxDigits);
void LoadLinkTestBgGfx(u8 paletteNum, u8 bgNum, u8 screenBaseBlock, u8 charBaseBlock);

#endif // GUARD_LINK_H
