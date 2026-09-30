#ifndef GUARD_LINK_H
#define GUARD_LINK_H

#define MAX_LINK_PLAYERS 4
#define CMD_LENGTH 8
#define QUEUE_CAPACITY 50
#define BLOCK_BUFFER_SIZE 0x100

#define LINKTYPE_TRADE               0x1111
#define LINKTYPE_TRADE_CONNECTING    0x1122
#define LINKTYPE_TRADE_SETUP         0x1133
#define LINKTYPE_TRADE_DISCONNECTED  0x1144
#define LINKTYPE_BATTLE              0x2211
#define LINKTYPE_SINGLE_BATTLE       0x2233
#define LINKTYPE_DOUBLE_BATTLE       0x2244
#define LINKTYPE_MULTI_BATTLE        0x2255
#define LINKTYPE_RECORD_MIX_BEFORE   0x3311
#define LINKTYPE_RECORD_MIX_AFTER    0x3322
#define LINKTYPE_BERRY_BLENDER_SETUP 0x4411
#define LINKTYPE_BERRY_BLENDER       0x4422
#define LINKTYPE_MYSTERY_EVENT       0x5501
#define LINKTYPE_CONTEST_GMODE       0x6601

enum
{
    BLOCK_REQ_SIZE_NONE, // Identical to 200
    BLOCK_REQ_SIZE_200,
    BLOCK_REQ_SIZE_100,
    BLOCK_REQ_SIZE_220,
    BLOCK_REQ_SIZE_40,
};

#define LINKCMD_SEND_LINK_TYPE          0x2222
#define LINKCMD_READY_EXIT_STANDBY      0x2FFE
#define LINKCMD_BLENDER_SEND_KEYS       0x4444
#define LINKCMD_DUMMY_1                 0x5555
#define LINKCMD_DUMMY_2                 0x5566
#define LINKCMD_READY_CLOSE_LINK        0x5FFF
#define LINKCMD_SEND_EMPTY              0x6666
#define LINKCMD_SEND_0xEE               0x7777
#define LINKCMD_CONT_BLOCK              0x8888
#define LINKCMD_BLENDER_NO_PBLOCK_SPACE 0xAAAA
#define LINKCMD_SEND_ITEM               0xAAAB
#define LINKCMD_READY_TO_TRADE          0xAABB
#define LINKCMD_INIT_BLOCK              0xBBBB
#define LINKCMD_READY_CANCEL_TRADE      0xBBCC
#define LINKCMD_SEND_HELD_KEYS          0xCAFE
#define LINKCMD_SEND_BLOCK_REQ          0xCCCC
#define LINKCMD_START_TRADE             0xCCDD
#define LINKCMD_SET_MONS_TO_TRADE       0xDDDD
#define LINKCMD_PLAYER_CANCEL_TRADE     0xDDEE
#define LINKCMD_REQUEST_CANCEL          0xEEAA
#define LINKCMD_BOTH_CANCEL_TRADE       0xEEBB
#define LINKCMD_PARTNER_CANCEL_TRADE    0xEECC
#define LINKCMD_NONE                    0xEFFF

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

#define EXTRACT_PLAYER_COUNT(status) \
(((status) & LINK_STAT_PLAYER_COUNT) >> LINK_STAT_PLAYER_COUNT_SHIFT)
#define EXTRACT_MASTER(status) \
(((status) >> LINK_STAT_MASTER_SHIFT) & 1)
#define EXTRACT_CONN_ESTABLISHED(status) \
(((status) >> LINK_STAT_CONN_ESTABLISHED_SHIFT) & 1)
#define EXTRACT_RECEIVED_NOTHING(status) \
(((status) >> LINK_STAT_RECEIVED_NOTHING_SHIFT) & 1)

#define MASTER_HANDSHAKE 0x8FFF
#define SLAVE_HANDSHAKE  0xB9A0

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
    EXCHANGE_IN_PROGRESS,
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
    u16 tempRecvBuffer[4];
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

extern const struct BlockRequest sBlockRequestLookupTable[5];

extern struct Link gLink;
extern u16 gRecvCmds[CMD_LENGTH][MAX_LINK_PLAYERS];
// TODO: Why is gBlockSendBuffer a u8 array, while gBlockRecvBuffer is a u16 array?
extern u8 gBlockSendBuffer[BLOCK_BUFFER_SIZE];
extern u16 gBlockRecvBuffer[MAX_LINK_PLAYERS][BLOCK_BUFFER_SIZE / 2];
extern u16 gLinkType;
extern u32 gLinkStatus;
extern u16 gSendCmd[CMD_LENGTH];
extern u8 gShouldAdvanceLinkState;
#if GERMAN
extern u8 gGermanDeferredBlockRequestType;
extern u8 gGermanLinkBlockRequestState;
#endif
extern struct LinkPlayer gLinkPlayers[];
extern u16 gLinkPartnersHeldKeys[];
extern bool8 gReceivedRemoteLinkPlayers;
extern bool8 gLinkOpen;
extern bool8 gLinkPlayerPending[MAX_LINK_PLAYERS];
extern void (*gLinkCallback)(void);
extern u8 gMysteryEventDebugRButtonCounter;

void Task_DestroySelf(u8);
void SetLocalLinkPlayerId(u8 playerId);
void OpenLink(void);
void CloseLink(void);
u16 LinkMain2(u16 *);
void StartSendingKeysToLink(void);
bool32 IsSendingKeysToLink(void);
void ClearLinkCallback(void);
void ClearLinkCallback_2(void);
u8 GetLinkPlayerCount(void);
void OpenLinkTimed(void);
u8 GetLinkPlayerDataExchangeStatusTimed(void);
bool8 IsLinkPlayerDataExchangeComplete(void);
u32 GetLinkPlayerTrainerId(u8);
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
void ResetBlockReceivedFlag(u8);
void CheckShouldAdvanceLinkState(void);
void SetLinkDebugValues(u32, u32);
u8 GetSavedLinkPlayerCountAsBitFlags(void);
void SaveLinkPlayers(u8);
u8 GetSavedPlayerCount(void);
u8 GetSavedMultiplayerId(void);
void CheckLinkPlayersMatchSaved(void);
void ResetLinkPlayerCount(void);
u8 GetLinkPlayerCount_2(void);
bool8 IsLinkMaster(void);
void SetCloseLinkCallback(void);
void SetCloseLinkCallbackHandleJP(void);
void SetLinkStandbyCallback(void);
void CB2_LinkError(void);
u8 GetSioMultiSI(void);
bool8 IsLinkConnectionEstablished(void);
void SetSuppressLinkErrorMessage(bool8);
bool8 HasLinkErrorOccurred(void);
void ResetSerial(void);
u32 LinkMain1(u8 *, u16 *, u16[CMD_LENGTH][MAX_LINK_PLAYERS]);
void LinkVSync(void);
void Timer3Intr(void);
void SerialCB(void);
void LinkTestScreen(void);
void InitDebugHexPrinter(u16 *digitsGfxDest, u32 gfxBase, u16 *tilemap, u32 displayId);
void PrintDebugHex(u32 value, int left, int top, int digitCount, int displayId);
u32 GetBerryBlenderKeySendAttempts(void);
void LinkTest_PrintHex(u32, u8, u8, u8);
void LoadLinkTestBgGfx(u8, u8, u8, u8);

#endif // GUARD_LINK_H
