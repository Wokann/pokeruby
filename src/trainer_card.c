#include "global.h"
#include "trainer_card.h"
#include "constants/songs.h"
#include "contest_util.h"
#include "easy_chat.h"
#include "event_data.h"
#include "ewram.h"
#include "field_effect.h"
#include "graphics.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "money.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings2.h"
#include "strings.h"
#include "task.h"
#include "util.h"

struct TrainerCardData
{
    /*0x00*/ u8 mainState;
    /*0x01*/ bool8 isLink;
    /*0x02*/ u8 starCount;
    /*0x03*/ bool8 onBack;
    /*0x04*/ bool8 allowDMACopy;
    /*0x05*/ bool8 showColon;
    /*0x06*/ u8 timeColonBlinkTimer; /* Used to flash colon */
    /*0x07*/ bool8 hasPokedex;
    /*0x08*/ bool8 hasHofResult;
    /*0x09*/ bool8 hasLinkResults;
    /*0x0A*/ bool8 hasBattleTowerWins;
    /*0x0B*/ bool8 showContestRecord;
    /*0x0C*/ bool8 showMixingRecord;
    /*0x0D*/ bool8 hasTrades;
    /*0x0E*/ bool8 badgeCount[8];
    /*0x16*/ u8 filler_16[10];
    /*0x20*/ u8 easyChatProfile[4][0x10];
    /*0x60*/ MainCallback *callback2;
    /*0x64*/ struct TrainerCard trainerCard;
    /*0x9C*/ u8 language; // 0x9C
};

extern struct LinkPlayerObjectEvent gLinkPlayerObjectEvents[];

EWRAM_DATA struct TrainerCard gTrainerCards[4] = {0};

static const u16 sHoennTrainerCardFemaleBg_Pal[];
static const u16 sHoennTrainerCardBadges_Pal[];
static const u16 sTrainerCardStar_Pal[];
extern const u16 gUnknown_083B5F6C[];
extern const u16 gTrainerCardBadgesMap[][4];

static const u8 sHoennTrainerCardBadges_Gfx[] = INCBIN_U8("graphics/trainer_card/badges.4bpp");

static struct TrainerCardData * const sData = (struct TrainerCardData *)gSharedMem;

#if DEBUG
const struct TrainerCard sTestTrainerCard =
{
    .gender = FEMALE,
    .stars = 4,
    .hasPokedex = TRUE,
    .caughtAllHoenn = TRUE,
    .hasAllPaintings = TRUE,
    .hofDebutHours = 999,
    .hofDebutMinutes = 99,
    .hofDebutSeconds = 99,
    .caughtMonsCount = 411,
    .trainerId = 12345,
    .playTimeHours = 99,
    .playTimeMinutes = 99,
    .linkBattleWins = 9999,
    .linkBattleLosses = 9999,
    .battleTowerWins = 9999,
    .battleTowerStraightWins = 9999,
    .contestsWithFriends = 999,
    .pokeblocksWithFriends = 0xFFFF,
    .pokemonTrades = 0xFFFF,
    .money = 99999,
    .easyChatProfile = {1, 2, 3, 4},
    .playerName = DTR("てすと", "TEST"),
};
#endif

bool8 TrainerCard_Init(struct Task *);
bool8 TrainerCard_WaitForFadeInToFinish(struct Task *);
bool8 TrainerCard_WaitForKeys(struct Task *);
bool8 TrainerCard_StartFlipAntimation(struct Task *);
bool8 TrainerCard_WaitForFlipToFinish(struct Task *);
bool8 TrainerCard_FadeOut(struct Task *);
bool8 TrainerCard_WaitForFadeOutToFinishAndQuit(struct Task *);

bool8 (*const TrainerCard_StateMachine[])(struct Task *) =
{
    TrainerCard_Init,
    TrainerCard_WaitForFadeInToFinish,
    TrainerCard_WaitForKeys,
    TrainerCard_StartFlipAntimation,
    TrainerCard_WaitForFlipToFinish,
    TrainerCard_FadeOut,
    TrainerCard_WaitForFadeOutToFinishAndQuit,
};

static bool8 Task_BeginCardFlip(struct Task *);
static bool8 Task_AnimateCardFlipDown(struct Task *);
static bool8 Task_DrawFlippedCardSide(struct Task *);
static bool8 Task_AnimateCardFlipUp(struct Task *);
static bool8 Task_EndCardFlip(struct Task *);

static bool8 (*const sTrainerCardFlipTasks[])(struct Task *) =
{
    Task_BeginCardFlip,
    Task_AnimateCardFlipDown,
    Task_DrawFlippedCardSide,
    Task_AnimateCardFlipUp,
    Task_EndCardFlip,
};

// FIXME: Other signature than on save_menu_util.h
void FormatPlayTime(u8 *playtime, u16 hours, u16 minutes, s16 colon);
u16 GetCaughtMonsCount(void);

enum
{
    TD_SHOWING_LINK_CARD,
    TD_CARD_INDEX,
    TD_CALLBACK,
};

static void CB2_InitTrainerCard(void);
static void CB2_TrainerCard(void);
static void VblankCb_TrainerCard(void);
static void TrainerCard_InitScreenForPlayer(MainCallback callBack);
static void TrainerCard_InitScreenForLinkPlayer(u8 arg1, MainCallback callBack);
void TrainerCard_FillTrainerCardStruct(void);
static void nullsub_60(u8);
static u32 GetCappedGameStat(u8 index, u32 maxVal);
static u8 GetRubyTrainerStars(struct TrainerCard *);
static void sub_8093534(void);
static void sub_8093550(void);
static void sub_8093598(void);
static void sub_80935EC(void);
static void sub_8093610(void);
static void InitTrainerCardData(void);
static void SetDataFromTrainerCard(void);
static void sub_80937A4(void);
static void sub_80937BC(void);
static void sub_80937D8(void);
static void sub_80937F0(void);
static void nullsub_15(void);
static void sub_8093800(void);
static void TrainerCard_CreateStateMachine(void);
static void Task_TrainerCard(u8 taskId);
static void TrainerCard_CreatePrintPlayTimeTask(void);
static void TrainerCard_DestoryPlayTimeTask(void);
static void PrintTimeOnCard(u8 taskId);
static void FlipTrainerCard(void);
static u8 IsCardFlipTaskActive(void);
static void Task_DoCardFlipTask(u8 taskId);
static void HblankCb_TrainerCard(void);
static void TrainerCard_DrawCard(void);
static void TrainerCard_DrawCardFront(void);
static void TrainerCard_DrawCardBack(void);
static void TrainerCard_ResetOffsetRegisters(void);
static void TrainerCard_CopyGraphics(void);
static void TrainerCard_LoadPalettes(void);
static void TrainerCard_LoadTrainerGraphics(void);
static void TrainerCard_LoadCardTileMap(void);
static void sub_8093F48(void);
static void sub_8093F64(void);
static void TrainerCard_LoadTrainerTilemap(void);
static void TrainerCard_DrawStars(void);
static void TrainerCard_DisplayBadges(void);
static void TrainerCard_ClearTrainerGraphics(void);
static void TrainerCard_ClearPokedexLabel(void);
static void PrintAllOnCardFront(void);
static void PrintAllOnCardBack(void);
static void PrintIdOnCard(void);
static void PrintMoneyOnCard(void);
static void PrintPokedexOnCard(void);
static void TrainerCard_Front_GetPlayTimeString(u8 *arg1, s16 colon);
static void PrintProfilePhraseOnCard(void);
static void PrintNameOnCardBack(void);
static void TrainerCard_Back_PrintHallOfFameTime_Label(void);
static void PrintHofDebutTimeOnCard(void);
static void TrainerCard_Back_PrintLinkBattlesLabel(void);
static void PrintLinkBattleResultsOnCard(void);
static void TrainerCard_Back_PrintBattleTower_Label(void);
static void PrintBattleFacilityStringOnCard(void);
static void TrainerCard_Back_PrintLinkContests_Label(void);
static void PrintContestStringOnCard(void);
static void TrainerCard_Back_PrintLinkPokeblocks_Label(void);
static void PrintPokeblockStringOnCard(void);
static void TrainerCard_Back_PrintPokemonTrades_Label(void);
static void PrintTradesStringOnCard(void);
void unref_sub_8094588(u16 left, u16 top);

#if DEBUG
static u8 gDebug_03000748;
#endif

void ShowPlayerTrainerCard(MainCallback callback)
{
#if DEBUG
    gDebug_03000748 = 0;
#endif
    TrainerCard_InitScreenForPlayer(callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sData->language = GAME_LANGUAGE;
}

void ShowTrainerCardInLink(u8 playerIndex, MainCallback callback)
{
#if DEBUG
    gDebug_03000748 = 0;
#endif
    TrainerCard_InitScreenForLinkPlayer(playerIndex, callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sData->language = gLinkPlayers[gLinkPlayerObjectEvents[playerIndex].linkPlayerId].language;
}

#if DEBUG
void debug_sub_80A0710(MainCallback callback)
{
    gDebug_03000748 = TRUE;
    TrainerCard_InitScreenForPlayer(callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sData->language = GAME_LANGUAGE;
}

void debug_sub_80A073C(MainCallback callback)
{
    memcpy(&gTrainerCards[0], &sTestTrainerCard, sizeof(struct TrainerCard));
    gDebug_03000748=TRUE;
    TrainerCard_InitScreenForLinkPlayer(0, callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sData->language = GAME_LANGUAGE;
}

void debug_sub_80A0780()
{
    int i;

    for (i = 0; i < 4; i++)
        memcpy(&gTrainerCards[i], &sTestTrainerCard, sizeof(struct TrainerCard));
}
#endif

static void CB2_InitTrainerCard(void)
{
    switch (gMain.state)
    {
    case 0:
        sub_8093534();
        InitTrainerCardData();
        gMain.state++;
        break;
    case 1:
        sub_8093598();
        gMain.state++;
        break;
    case 2:
        sub_80935EC();
        gMain.state++;
        break;
    case 3:
        sub_8093610();
        sub_80937A4();
        gMain.state++;
        break;
    case 4:
        sub_80937BC();
        gMain.state++;
    case 5:
        if (MultistepInitMenuWindowContinue())
            gMain.state++;
        break;
    case 6:
        sub_80937F0();
        gMain.state++;
        break;
    case 7:
        sub_80937D8();
        gMain.state++;
        break;
    case 8:
        nullsub_15();
        sub_8093800();
        sub_8093550();
        SetMainCallback2(CB2_TrainerCard);
        break;
    }
}

static void CB2_TrainerCard(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VblankCb_TrainerCard(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    sData->timeColonBlinkTimer++;
    if (sData->timeColonBlinkTimer >= 60)
    {
        sData->timeColonBlinkTimer = 0;
        sData->showColon ^= 1;
    }
    if (sData->allowDMACopy)
        DmaCopy16(3, &gScanlineEffectRegBuffers[0], &gScanlineEffectRegBuffers[1], 0x140);
}

static void TrainerCard_InitScreenForPlayer(MainCallback callback)
{
    u8 taskId = CreateTask(nullsub_60, 0xFF);
    struct Task *task = &gTasks[taskId];
    task->data[TD_SHOWING_LINK_CARD] = FALSE;
    StoreWordInTwoHalfwords(&task->data[TD_CALLBACK], (u32)callback);
}

static void TrainerCard_InitScreenForLinkPlayer(u8 playerIndex, MainCallback callback)
{
    u8 taskId = CreateTask(nullsub_60, 0xFF);

    struct Task *task = &gTasks[taskId];
    task->data[TD_SHOWING_LINK_CARD] = TRUE;
    task->data[TD_CARD_INDEX] = playerIndex;
    StoreWordInTwoHalfwords(&task->data[TD_CALLBACK], (u32)callback);
}

void TrainerCard_FillTrainerCardStruct(void)
{
    u8 taskId = FindTaskIdByFunc(nullsub_60);
    struct Task *task = &gTasks[taskId];
    sData->isLink = task->data[TD_SHOWING_LINK_CARD];

    LoadWordFromTwoHalfwords((u16 *)&task->data[TD_CALLBACK], (u32 *)&sData->callback2);

    if (sData->isLink)
    {
        sData->trainerCard = gTrainerCards[task->data[TD_CARD_INDEX]];
    }
    else
    {
        TrainerCard_GenerateCardForPlayer(&sData->trainerCard);
    }
}

static void nullsub_60(u8 taskid)
{
}

void TrainerCard_GenerateCardForPlayer(struct TrainerCard *trainerCard)
{
    u32 playTime;
    bool32 enteredHallOfFame;
    bool8 hasAllPaintings;
    u8 i;

    trainerCard->gender = gSaveBlock2.playerGender;
    trainerCard->playTimeHours = gSaveBlock2.playTimeHours;
    trainerCard->playTimeMinutes = gSaveBlock2.playTimeMinutes;

    playTime = GetGameStat(GAME_STAT_FIRST_HOF_PLAY_TIME);
    enteredHallOfFame = GetGameStat(GAME_STAT_ENTERED_HOF);
    if (!enteredHallOfFame)
    {
        playTime = 0;
    }
    trainerCard->hofDebutHours = playTime >> 16;
    trainerCard->hofDebutMinutes = (playTime >> 8) & 0xFF;
    trainerCard->hofDebutSeconds = playTime & 0xFF;

    trainerCard->hasPokedex = FlagGet(FLAG_SYS_POKEDEX_GET);
    trainerCard->caughtAllHoenn = HasAllHoennMons();
    trainerCard->caughtMonsCount = GetCaughtMonsCount();

    trainerCard->trainerId = (gSaveBlock2.playerTrainerId[1] << 8) | gSaveBlock2.playerTrainerId[0];

    // Link Cable Battles
    trainerCard->linkBattleWins = GetCappedGameStat(GAME_STAT_LINK_BATTLE_WINS, 9999);
    trainerCard->linkBattleLosses = GetCappedGameStat(GAME_STAT_LINK_BATTLE_LOSSES, 9999);

    // Contests w/ Friends
    trainerCard->contestsWithFriends = GetCappedGameStat(GAME_STAT_WON_LINK_CONTEST, 999);

    // Pokéblocks w/ Friends
    trainerCard->pokeblocksWithFriends = GetCappedGameStat(GAME_STAT_POKEBLOCKS_WITH_FRIENDS, 0xFFFF);

    // Pokémon Trades
    trainerCard->pokemonTrades = GetCappedGameStat(GAME_STAT_POKEMON_TRADES, 0xFFFF);

    // Battle Tower
    trainerCard->battleTowerWins = gSaveBlock2.battleTower.totalBattleTowerWins;
    trainerCard->battleTowerStraightWins = gSaveBlock2.battleTower.bestBattleTowerWinStreak;
    if (trainerCard->battleTowerWins > 9999)
    {
        trainerCard->battleTowerWins = 9999;
    }
    if (trainerCard->battleTowerStraightWins > 9999)
    {
        trainerCard->battleTowerStraightWins = 9999;
    }

    hasAllPaintings = FALSE;
    if (CountPlayerMuseumPaintings() > 4)
    {
        hasAllPaintings = TRUE;
    }
    trainerCard->hasAllPaintings = hasAllPaintings;

    trainerCard->money = gSaveBlock1.money;

    for (i = 0; i < 4; i++)
    {
        trainerCard->easyChatProfile[i] = gSaveBlock1.easyChats.unk2B1C[i];
    }

    for (i = 0; i < 8; i++)
    {
        trainerCard->playerName[i] = gSaveBlock2.playerName[i];
    }

    trainerCard->stars = GetRubyTrainerStars(trainerCard);
}

u8 GetTrainerCardStars(u8 cardId)
{
    return gTrainerCards[cardId].stars;
}

static u32 GetCappedGameStat(u8 statId, u32 maxValue)
{
    u32 statValue = GetGameStat(statId);

    if (statValue > maxValue)
    {
        statValue = maxValue;
    }

    return statValue;
}

static u8 GetRubyTrainerStars(struct TrainerCard *trainerCard)
{
    u8 stars = 0;

    if (trainerCard->hofDebutHours != 0 || trainerCard->hofDebutMinutes != 0 || trainerCard->hofDebutSeconds != 0)
    {
        stars++;
    }

    if (trainerCard->caughtAllHoenn)
    {
        stars++;
    }

    if (trainerCard->battleTowerStraightWins > 49)
    {
        stars++;
    }

    if (trainerCard->hasAllPaintings)
    {
        stars++;
    }

    return stars;
}

static void sub_8093534(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    REG_DISPCNT = 0;
}

static void sub_8093550(void)
{
    u16 backup;

    SetVBlankCallback(VblankCb_TrainerCard);

    backup = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK | INTR_FLAG_HBLANK;
    REG_IME = backup;

    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR | DISPSTAT_HBLANK_INTR;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
}

void sub_8093598(void)
{
    DmaFill16Large(3, 0, (void *)VRAM, 0x10000, 0x1000);
}

void sub_80935EC(void)
{
    DmaFill16Defvars(3, 0, (void *)OAM, 0x400);
}

void sub_8093610(void)
{
    REG_BG0CNT = 0;
    REG_BG1CNT = 0;
    REG_BG2CNT = 0;
    REG_BG3CNT = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;

    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(30) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(8)  | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(9)  | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(10) | BGCNT_16COLOR | BGCNT_TXT256x256;
}

static void InitTrainerCardData(void)
{
    u8 i;

    TrainerCard_FillTrainerCardStruct();
    sData->mainState = 0;
    sData->onBack = FALSE;
    sData->allowDMACopy = FALSE;
    sData->starCount = sData->trainerCard.stars;
    sData->showColon = 0;
    sData->timeColonBlinkTimer = 0;
    for (i = 0; i < 4; i++)
        CopyEasyChatWord(sData->easyChatProfile[i], sData->trainerCard.easyChatProfile[i]);
    SetDataFromTrainerCard();
}

static void SetDataFromTrainerCard(void)
{
    sData->hasPokedex = 0;
    sData->hasHofResult = 0;
    sData->hasLinkResults = 0;
    sData->hasBattleTowerWins = 0;
    sData->showContestRecord = 0;
    sData->showMixingRecord = 0;
    sData->hasTrades = 0;
    memset(sData->badgeCount, 0, sizeof(sData->badgeCount));

    if (sData->trainerCard.hasPokedex)
        sData->hasPokedex++;

    if (sData->trainerCard.hofDebutHours != 0
     || sData->trainerCard.hofDebutMinutes != 0
     || sData->trainerCard.hofDebutSeconds != 0)
        sData->hasHofResult++;

    if (sData->trainerCard.linkBattleWins != 0 || sData->trainerCard.linkBattleLosses != 0)
        sData->hasLinkResults++;

    if (sData->trainerCard.battleTowerWins != 0 || sData->trainerCard.battleTowerStraightWins != 0)
        sData->hasBattleTowerWins++;

    if (sData->trainerCard.contestsWithFriends != 0)
        sData->showContestRecord++;

    if (sData->trainerCard.pokeblocksWithFriends != 0)
        sData->showMixingRecord++;

    if (sData->trainerCard.pokemonTrades != 0)
        sData->hasTrades++;

    if (!sData->isLink)
    {
        u32 badgeFlag;
        int i = 0;

        badgeFlag = FLAG_BADGE01_GET;
        while (1)
        {
            if (FlagGet(badgeFlag))
                sData->badgeCount[i]++;
            badgeFlag++;
            i++;
            if (badgeFlag > FLAG_BADGE08_GET)
            {
                break;
            }
        }
    }

#if DEBUG
    if (gDebug_03000748 != 0)
    {
        sData->hasHofResult = TRUE;
        sData->hasLinkResults = TRUE;
        sData->hasBattleTowerWins = TRUE;
        sData->showContestRecord = TRUE;
        sData->showMixingRecord = TRUE;
        sData->hasTrades = TRUE;
        memset(sData->badgeCount, TRUE, sizeof(sData->badgeCount));
    }
#endif
}

void sub_80937A4()
{
    ResetPaletteFade();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
}

void sub_80937BC()
{
    Text_LoadWindowTemplate(&gWindowTemplate_TrainerCard_Back_Values);
    MultistepInitMenuWindowBegin(&gWindowTemplate_TrainerCard_Back_Values);
}

static void sub_80937D8()
{
    TrainerCard_ResetOffsetRegisters();
    TrainerCard_CopyGraphics();
    sub_8093F64();
    TrainerCard_DrawCard();
}

static void sub_80937F0()
{
    TrainerCard_LoadTrainerGraphics();
}

static void nullsub_15(void)
{
}

static void sub_8093800()
{
    TrainerCard_CreateStateMachine();
}

static void TrainerCard_CreateStateMachine(void)
{
    u8 taskId;

    taskId = CreateTask(Task_TrainerCard, 0);
    Task_TrainerCard(taskId);
}

static void Task_TrainerCard(u8 taskId)
{
    while (TrainerCard_StateMachine[sData->mainState](&gTasks[taskId]) != 0)
        ;
}

bool8 TrainerCard_Init(struct Task *task)
{
    sData->showColon = gSaveBlock2.playTimeSeconds & 1;
    sData->timeColonBlinkTimer = gSaveBlock2.playTimeVBlanks;
    TrainerCard_CreatePrintPlayTimeTask();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    sData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForFadeInToFinish(struct Task *task)
{
    if (!gPaletteFade.active)
        sData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForKeys(struct Task *task)
{
    if (JOY_NEW(B_BUTTON))
    {
        sData->mainState = 5; /* Jump to fadeout state */
        return TRUE;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        /* It appears that it was previously possible to return the the front side
           after viewing the back side. This was probably removed due to being
           unintuitive. */
        if (sData->onBack)
        {
            sData->mainState = 5; /* Jump to fadeout state */
        }
        else
        {
            sData->onBack ^= 1; /* Switch to back side  */
            sData->mainState = 3; /* Jump to start flip animation state */
        }
        return TRUE;
    }
#if DEBUG
    else if (gDebug_03000748 && JOY_NEW(R_BUTTON))
    {
        sData->starCount++;
        sData->starCount %= 5;
        TrainerCard_LoadPalettes();
        if (sData->onBack == 0)
            TrainerCard_DrawStars();
    }
#endif

    return FALSE;
}

bool8 TrainerCard_StartFlipAntimation(struct Task *task)
{
    FlipTrainerCard();
    PlaySE(SE_CARD);
    sData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForFlipToFinish(struct Task *task)
{
    if (IsCardFlipTaskActive())
        sData->mainState = 2; /* Return to wait for keys state */
    return FALSE;
}

bool8 TrainerCard_FadeOut(struct Task *task)
{
    TrainerCard_DestoryPlayTimeTask();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    sData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForFadeOutToFinishAndQuit(struct Task *task)
{
    if (!gPaletteFade.active)
        SetMainCallback2((MainCallback)sData->callback2);
    return FALSE;
}

static void TrainerCard_CreatePrintPlayTimeTask(void)
{
    CreateTask(PrintTimeOnCard, 0);
    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Values);
}

static void TrainerCard_DestoryPlayTimeTask(void)
{
    u8 taskId = FindTaskIdByFunc(PrintTimeOnCard);

    if (taskId != 0xFF)
        DestroyTask(taskId);
}

static void PrintTimeOnCard(u8 taskId)
{
    u8 buffer[32];
    struct Task *task = &gTasks[taskId];

    if (sData->showColon != task->data[TD_CARD_INDEX])
    {
        task->data[TD_CARD_INDEX] = sData->showColon;
        task->data[TD_SHOWING_LINK_CARD] ^= TRUE;
    }
    TrainerCard_Front_GetPlayTimeString(buffer, task->data[TD_SHOWING_LINK_CARD]);
    Menu_PrintText(buffer, 10, 12);
}

static void FlipTrainerCard(void)
{
    u8 taskId;

    taskId = CreateTask(Task_DoCardFlipTask, 0);
    Task_DoCardFlipTask(taskId);
}

static u8 IsCardFlipTaskActive(void)
{
    if (FindTaskIdByFunc(Task_DoCardFlipTask) == 0xFF)
        return TRUE;
    else
        return FALSE;
}

static void Task_DoCardFlipTask(u8 taskId)
{
    while (sTrainerCardFlipTasks[gTasks[taskId].data[0]](&gTasks[taskId]) != 0)
        ;
}

static bool8 Task_BeginCardFlip(struct Task *task)
{
    u32 i;

    sData->allowDMACopy = FALSE;
    ScanlineEffect_Clear();
    for (i = 0; i < 0xA0; i++)
        gScanlineEffectRegBuffers[1][i] = -4;
    SetHBlankCallback(HblankCb_TrainerCard);
    sData->allowDMACopy = TRUE;
    task->data[0]++;
    return FALSE;
}

static bool8 Task_AnimateCardFlipDown(struct Task *task)
{
    s16 i;
    u32 cardHeight, r5, r10, cardTop, r6, var_24, cardBottom, var;

    sData->allowDMACopy = FALSE;
    task->data[1] += 3;
    if (task->data[1] > 79)
        task->data[1] = 79;

    cardTop = task->data[1];
    cardBottom = 160 - cardTop;
    cardHeight = cardBottom - cardTop;
    r6 = -cardTop << 16;
    r5 = 0xA00000 / cardHeight;
    r5 += 0xFFFF0000;
    var_24 = r6 + r5 * cardHeight;
    r10 = r5 / cardHeight;
    r5 *= 2;

    for (i = 0; i < cardTop;
        // WHAT?!
        gScanlineEffectRegBuffers[0][i] = (u32)-i + -4,
        i++);
    for (; i < (s16)cardBottom; i++)
    {
        var = r6 >> 16;
        r6 += r5;
        r5 -= r10;
        gScanlineEffectRegBuffers[0][i] = var + -4;
    }
    for (var = var_24 >> 16; i < 160;
        // WHAT?!
        gScanlineEffectRegBuffers[0][i] = var + -4,
        i++);

    sData->allowDMACopy = TRUE;
    if (task->data[1] > 74)
        task->data[0]++;

    return FALSE;
}

static bool8 Task_DrawFlippedCardSide(struct Task *task)
{
    TrainerCard_DestoryPlayTimeTask();
    TrainerCard_DrawCard();
    if (!sData->onBack) {
        /* This code never runs because it is impossible to flip the back side back to the front side */
        TrainerCard_CreatePrintPlayTimeTask();
    }
    task->data[0]++;
    return TRUE;
}

static bool8 Task_AnimateCardFlipUp(struct Task *task)
{
    s16 i;
    u32 cardHeight, r5, r10, cardTop, r6, var_24, cardBottom, var;

    sData->allowDMACopy = FALSE;
    task->data[1] -= 3;
    if (task->data[1] <= 0)
        task->data[1] = 0;

    cardTop = task->data[1];
    cardBottom = 160 - cardTop;
    cardHeight = cardBottom - cardTop;
    r6 = -cardTop << 16;
    r5 = 0xA00000 / cardHeight;
    r5 += 0xFFFF0000;
    var_24 = r6 + r5 * cardHeight;
    r10 = r5 / cardHeight;
    r5 /= 2;

    for (i = 0; i < cardTop;
        // WHAT?!
        gScanlineEffectRegBuffers[0][i] = (u32)-i + -4,
        i++);
    for (; i < (s16)cardBottom; i++)
    {
        var = r6 >> 16;
        r6 += r5;
        r5 += r10;
        gScanlineEffectRegBuffers[0][i] = var + -4;
    }
    for (var = var_24 >> 16; i < 160;
        // WHAT?!
        gScanlineEffectRegBuffers[0][i] = var + -4,
        i++);

    sData->allowDMACopy = TRUE;
    if (task->data[1] <= 0)
        task->data[0]++;

    return FALSE;
}

static bool8 Task_EndCardFlip(struct Task *task)
{
    u8 taskId;

    sData->allowDMACopy = FALSE;
    SetHBlankCallback(NULL);
    TrainerCard_ResetOffsetRegisters();
    taskId = FindTaskIdByFunc(Task_DoCardFlipTask);
    DestroyTask(taskId);
    return FALSE;
}

static void HblankCb_TrainerCard(void)
{
    u16 bgVOffset = gScanlineEffectRegBuffers[1][REG_VCOUNT & 0xFF];

    REG_BG0VOFS = bgVOffset;
    REG_BG1VOFS = bgVOffset;
    REG_BG2VOFS = bgVOffset;
}

static void TrainerCard_DrawCard(void)
{
    if (sData->onBack)
        TrainerCard_DrawCardBack();
    else
        TrainerCard_DrawCardFront();
}

static void TrainerCard_DrawCardFront(void)
{
    Menu_EraseScreen();
    TrainerCard_ClearTrainerGraphics();
    TrainerCard_LoadCardTileMap();
    TrainerCard_LoadTrainerTilemap();
    TrainerCard_DrawStars();
    TrainerCard_DisplayBadges();
    PrintAllOnCardFront();
}

static void TrainerCard_DrawCardBack(void)
{
    Menu_EraseScreen();
    TrainerCard_ClearTrainerGraphics();
    sub_8093F48();
    PrintAllOnCardBack();
}

static void TrainerCard_ResetOffsetRegisters(void)
{
    REG_BG0VOFS = -4;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = -4;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = -4;
}

static void TrainerCard_CopyGraphics(void)
{
    TrainerCard_LoadPalettes();
    LoadPalette(gUnknown_083B5F6C, 0xE0, 32);
    DmaCopyLarge16(3, gHoennTrainerCard_Gfx, (void *)VRAM, 0x1480, 0x1000);
    DmaCopy16Defvars(3, sHoennTrainerCardBadges_Gfx, (void *)(VRAM + 0x1480), 0x400);
}

static const u16 *const sHoennTrainerCardPals[];

static void TrainerCard_LoadPalettes(void)
{
    LoadPalette(sHoennTrainerCardPals[sData->starCount], 0, 48 * 2);
    LoadPalette(sHoennTrainerCardBadges_Pal, 48, 16 * 2);
    LoadPalette(sTrainerCardStar_Pal, 64, 16 * 2);
    if (sData->trainerCard.gender != MALE)
        LoadPalette(sHoennTrainerCardFemaleBg_Pal, 16, 16 * 2);
}

static void TrainerCard_LoadTrainerGraphics(void)
{
    LoadTrainerGfx_TrainerCard(sData->trainerCard.gender, 80, (void *)(VRAM + 0x1880));
}

static void TrainerCard_LoadCardTileMap(void)
{
    const void *arr[] = {gHoennTrainerCardFront_Tilemap, gHoennTrainerCardFrontLink_Tilemap};

    CpuFastSet(arr[sData->isLink], (void *)(VRAM + 0x4800), 0x140);
}

// I don't really know where to put the data. It's in such a weird order.

const u8 gUnknown_083B5EF4[] = _(" : ");

static const u16 *const sHoennTrainerCardPals[] =
{
    gHoennTrainerCardGreen_Pal,
    gHoennTrainerCardBronze_Pal,
    gHoennTrainerCardCopper_Pal,
    gHoennTrainerCardSilver_Pal,
    gHoennTrainerCardGold_Pal,
};

static const u16 sHoennTrainerCardFemaleBg_Pal[] = INCBIN_U16("graphics/trainer_card/female_bg.gbapal");
static const u16 sHoennTrainerCardBadges_Pal[] = INCBIN_U16("graphics/trainer_card/badges.gbapal");
static const u16 sTrainerCardStar_Pal[] = INCBIN_U16("graphics/trainer_card/star.gbapal");
const u16 gUnknown_083B5F6C[] = INCBIN_U16("graphics/trainer_card/83B5F6C.gbapal");
const u16 gTrainerCardBadgesMap[][4] = INCBIN_U16("graphics/trainer_card/83B5F8C_map.bin");

static void sub_8093F48(void)
{
    CpuFastSet(gHoennTrainerCardBack_Tilemap, (void *)(VRAM + 0x4800), 320);
}

static void sub_8093F64(void)
{
    CpuFastSet(gHoennTrainerCardBg_Tilemap, (void *)(VRAM + 0x5000), 320);
}

static void TrainerCard_LoadTrainerTilemap(void)
{
    u16 r5 = 0xC4;
    u16 *ptr = (u16 *)(VRAM + 0x4000);
    s16 i;
    s16 j;

    for (i = 5; i < 13; i++)
    {
        for (j = 19; j < 27; j++, r5++)
            ptr[i * 32 + j] = r5 | 0x5000;
    }
}

static void TrainerCard_DrawStars(void)
{
    u16 *ptr = (u16 *)(VRAM + 0x4000);
    s16 i = 15;
    s16 var = 15 + sData->starCount;

    while (i < var)
    {
        ptr[6 * 32 + i] = 0x408F;
        i++;
    }
    while (i < 0x13)
    {
        ptr[6 * 32 + i] = 0;
        i++;
    }
}

static void TrainerCard_DisplayBadges(void)
{
    if (!sData->isLink)
    {
        u16 *ptr = (u16 *)(VRAM + 0x4000);
        s16 i;
        s16 r2;

        for (i = 0, r2 = 4; i < 8; i++, r2 += 3)
        {
            if (sData->badgeCount[i] != 0)
            {
                ptr[15 * 32 + r2 + 0] = gTrainerCardBadgesMap[i][0] | 0x3000;
                ptr[15 * 32 + r2 + 1] = gTrainerCardBadgesMap[i][1] | 0x3000;
                ptr[16 * 32 + r2 + 0] = gTrainerCardBadgesMap[i][2] | 0x3000;
                ptr[16 * 32 + r2 + 1] = gTrainerCardBadgesMap[i][3] | 0x3000;
            }
        }
    }
}

static void TrainerCard_ClearTrainerGraphics(void)
{
    s16 i;
    u16 *ptr;

    for (i = 0, ptr = (u16 *)(VRAM + 0x4000); i < 0x400; i++, ptr++)
        *ptr = 0;
}

static void TrainerCard_ClearPokedexLabel(void)
{
    u16 *ptr = (u16 *)(VRAM + 0x4800);
    u16 i;

    for (i = 3; i < 17; i++)
    {
        ptr[10 * 32 + i] = 1;
        ptr[11 * 32 + i] = 1;
    }
}

static void PrintAllOnCardFront(void)
{
    u8 *buffer;

    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Values);

    buffer = gStringVar1;
    StringCopy(buffer, sData->trainerCard.playerName);
    ConvertInternationalString(buffer, sData->language);
    Menu_PrintText(buffer, 7, 5);

    PrintIdOnCard();
    PrintMoneyOnCard();
    PrintPokedexOnCard();
    PrintProfilePhraseOnCard();
}

static void PrintAllOnCardBack(void)
{
    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Values);
    PrintNameOnCardBack();
    TrainerCard_Back_PrintHallOfFameTime_Label();
    TrainerCard_Back_PrintLinkBattlesLabel();
    TrainerCard_Back_PrintBattleTower_Label();
    TrainerCard_Back_PrintLinkContests_Label();
    TrainerCard_Back_PrintLinkPokeblocks_Label();
    TrainerCard_Back_PrintPokemonTrades_Label();

    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Labels);
    PrintHofDebutTimeOnCard();
    PrintLinkBattleResultsOnCard();
    PrintBattleFacilityStringOnCard();
    PrintContestStringOnCard();
    PrintPokeblockStringOnCard();
    PrintTradesStringOnCard();
}

static void PrintIdOnCard(void)
{
    u8 buffer[8];

    ConvertIntToDecimalStringN(buffer, sData->trainerCard.trainerId, STR_CONV_MODE_LEADING_ZEROS, 5);
    Menu_PrintText(buffer, 20, 2);
}

static void PrintMoneyOnCard(void)
{
    sub_80B7AEC(sData->trainerCard.money, 16, 8);
}

static void PrintPokedexOnCard(void)
{
    u8 buffer[16];

    if (
#if DEBUG
     gDebug_03000748 == 0 &&
#endif
     !sData->hasPokedex)
    {
        TrainerCard_ClearPokedexLabel();
    }
    else
    {
        ConvertIntToDecimalStringN(buffer, sData->trainerCard.caughtMonsCount, STR_CONV_MODE_LEFT_ALIGN, 3);
        MenuPrint_RightAligned(buffer, 16, 10);
    }
}

static void TrainerCard_Front_GetPlayTimeString(u8 *arg1, s16 colon)
{
    u8 buffer[16];
    u16 playTimeHours;
    u16 playTimeMinutes;

    playTimeHours = gSaveBlock2.playTimeHours;
    playTimeMinutes = gSaveBlock2.playTimeMinutes;
    if (sData->isLink != 0)
    {
        playTimeHours = sData->trainerCard.playTimeHours;
        playTimeMinutes = sData->trainerCard.playTimeMinutes;
    }
    FormatPlayTime(buffer, playTimeHours, playTimeMinutes, colon);
    AlignStringInMenuWindow(arg1, buffer, 48, 1);
}

static void PrintProfilePhraseOnCard(void)
{
    u8 *str;

    if (sData->isLink != 0)
    {
        str = gStringVar1;
        str = StringCopy(str, sData->easyChatProfile[0]);
        str[0] = 00;
        str++;
        str = StringCopy(str, sData->easyChatProfile[1]);
        Menu_PrintText(gStringVar1, 2, 14);

        str = gStringVar1;
        str = StringCopy(str, sData->easyChatProfile[2]);
        str[0] = 00;
        str++;
        str = StringCopy(str, sData->easyChatProfile[3]);
        Menu_PrintText(gStringVar1, 2, 16);
    }
}

static void PrintNameOnCardBack(void)
{
    u8 *str;

    str = gStringVar1;
    StringCopy(str, sData->trainerCard.playerName);
    ConvertInternationalString(str, sData->language);

#if ENGLISH
    StringAppend(str, gOtherText_TrainersTrainerCard);
#elif GERMAN
    de_sub_8073174(str, gOtherText_TrainersTrainerCard);
#endif

    MenuPrint_RightAligned(gStringVar1, 28, 2);
}

static void TrainerCard_Back_PrintHallOfFameTime_Label(void)
{
    if (sData->hasHofResult != 0)
        Menu_PrintText(gText_HallOfFameDebut, 3, 5);
}

static void PrintHofDebutTimeOnCard(void)
{
    u8 *str;

    if (sData->hasHofResult != 0)
    {
        str = gStringVar1;
        str = ConvertIntToDecimalStringN(str, sData->trainerCard.hofDebutHours, STR_CONV_MODE_RIGHT_ALIGN, 3);
        str = StringCopy(str, gUnknown_083B5EF4);
        str = ConvertIntToDecimalStringN(str, sData->trainerCard.hofDebutMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
        str = StringCopy(str, gUnknown_083B5EF4);
        str = ConvertIntToDecimalStringN(str, sData->trainerCard.hofDebutSeconds, STR_CONV_MODE_LEADING_ZEROS, 2);
        MenuPrint_RightAligned(gStringVar1, 28, 5);
    }
}

static void TrainerCard_Back_PrintLinkBattlesLabel(void)
{
    if (sData->hasLinkResults != 0)
        Menu_PrintText(gText_LinkCableBattles, 3, 7);
}

static void PrintLinkBattleResultsOnCard(void)
{
    u8 buffer[16];

    if (sData->hasLinkResults != 0)
    {
        ConvertIntToDecimalStringSimple(buffer, sData->trainerCard.linkBattleWins);
        MenuPrint_RightAligned(buffer, 22, 7);

        ConvertIntToDecimalStringSimple(buffer, sData->trainerCard.linkBattleLosses);
        MenuPrint_RightAligned(buffer, 28, 7);
    }
}

static void TrainerCard_Back_PrintBattleTower_Label(void)
{
    if (sData->hasBattleTowerWins != 0)
        Menu_PrintText(gText_BattleTower, 3, 15);
}

static void PrintBattleFacilityStringOnCard(void)
{
    u8 buffer[16];

    if (sData->hasBattleTowerWins != 0)
    {
        AlignInt2InMenuWindow(buffer, sData->trainerCard.battleTowerWins, 24, 1);
        Menu_PrintTextPixelCoords(buffer, 112, 120, 0);

        AlignInt2InMenuWindow(buffer, sData->trainerCard.battleTowerStraightWins, 24, 1);
        Menu_PrintTextPixelCoords(buffer, 149, 120, 0);
    }
}

static void TrainerCard_Back_PrintLinkContests_Label(void)
{
    if (sData->showContestRecord != 0)
        Menu_PrintText(gText_WonContestsWFriends, 3, 13);
}

static void PrintContestStringOnCard(void)
{
    u8 buffer[8];

    if (sData->showContestRecord != 0)
    {
        ConvertIntToDecimalStringN(buffer, sData->trainerCard.contestsWithFriends, STR_CONV_MODE_RIGHT_ALIGN, 3);
        MenuPrint_RightAligned(buffer, 28, 13);
    }
}

static void TrainerCard_Back_PrintLinkPokeblocks_Label(void)
{
    if (sData->showMixingRecord != 0)
        Menu_PrintText(gText_PokeblocksWithFriends, 3, 11);
}

static void PrintPokeblockStringOnCard(void)
{
    u8 buffer[8];

    if (sData->showMixingRecord != 0)
    {
        ConvertIntToDecimalStringN(buffer, sData->trainerCard.pokeblocksWithFriends, STR_CONV_MODE_RIGHT_ALIGN, 5);
        MenuPrint_RightAligned(buffer, 28, 11);
    }
}

static void TrainerCard_Back_PrintPokemonTrades_Label(void)
{
    if (sData->hasTrades != 0)
        Menu_PrintText(gText_PokemonTrades, 3, 9);
}

static void PrintTradesStringOnCard(void)
{
    u8 buffer[8];

    if (sData->hasTrades != 0)
    {
        ConvertIntToDecimalStringN(buffer, sData->trainerCard.pokemonTrades, STR_CONV_MODE_RIGHT_ALIGN, 5);
        MenuPrint_RightAligned(buffer, 28, 9);
    }
}

void unref_sub_8094588(u16 left, u16 top)
{
    const u8 *text = gOtherText_Boy;

    if (gSaveBlock2.playerGender == FEMALE)
        text = gOtherText_Girl;
    Menu_PrintText(text, left, top);
}
