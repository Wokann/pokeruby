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
#include "task.h"
#include "util.h"

struct TrainerCardData
{
    /*0x00*/ u8 mainState;
    /*0x01*/ bool8 isLink;
    /*0x02*/ u8 starCount;
    /*0x03*/ bool8 onBack;
    /*0x04*/ bool8 allowDmaCopy;
    /*0x05*/ bool8 timeColonVisible;
    /*0x06*/ u8 timeColonBlinkTimer;
    /*0x07*/ bool8 hasPokedex;
    /*0x08*/ bool8 hasHofResult;
    /*0x09*/ bool8 hasLinkResults;
    /*0x0A*/ bool8 hasBattleTowerResults;
    /*0x0B*/ bool8 hasLinkContestResults;
    /*0x0C*/ bool8 hasLinkPokeblockResults;
    /*0x0D*/ bool8 hasTrades;
    /*0x0E*/ bool8 badgeCount[8];
    /*0x16*/ u8 filler_16[10];
    /*0x20*/ u8 easyChatProfile[TRAINER_CARD_PROFILE_LENGTH][0x10];
    /*0x60*/ MainCallback callback2;
    /*0x64*/ struct TrainerCard trainerCard;
    /*0x9C*/ u8 language; // 0x9C
};

extern struct LinkPlayerObjectEvent gLinkPlayerObjectEvents[];

EWRAM_DATA struct TrainerCard gTrainerCards[4] = {0};

static const u16 sHoennTrainerCardFemaleBg_Pal[];
static const u16 sHoennTrainerCardBadges_Pal[];
static const u16 sTrainerCardStar_Pal[];
static const u16 sUnused_Pal[];
static const u16 sHoennTrainerCardBadges_Tilemap[][4];
static const u16 *const sHoennTrainerCardPals[];

static const u8 sHoennTrainerCardBadges_Gfx[] = INCBIN_U8("graphics/trainer_card/badges.4bpp");

static struct TrainerCardData *const sTrainerCardData = (struct TrainerCardData *)gSharedMem;

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
    .pokedexSeen = 411,
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
bool8 TrainerCard_StartFlipAnimation(struct Task *);
bool8 TrainerCard_WaitForFlipToFinish(struct Task *);
bool8 TrainerCard_FadeOut(struct Task *);
bool8 TrainerCard_WaitForFadeOutToFinishAndQuit(struct Task *);

static bool8 (*const sTrainerCardStateMachine[])(struct Task *) =
{
    TrainerCard_Init,
    TrainerCard_WaitForFadeInToFinish,
    TrainerCard_WaitForKeys,
    TrainerCard_StartFlipAnimation,
    TrainerCard_WaitForFlipToFinish,
    TrainerCard_FadeOut,
    TrainerCard_WaitForFadeOutToFinishAndQuit,
};

bool8 TrainerCard_InitFlipAnimation(struct Task *);
bool8 TrainerCard_ScaleDownFlipAnimation(struct Task *);
bool8 TrainerCard_SwitchToNewSide(struct Task *);
bool8 TrainerCard_ScaleUpFlipAnimation(struct Task *);
bool8 TrainerCard_FinishFlipAnimation(struct Task *);

static bool8 (*const sTrainerCardFlipAnimationStateMachine[])(struct Task *) =
{
    TrainerCard_InitFlipAnimation,
    TrainerCard_ScaleDownFlipAnimation,
    TrainerCard_SwitchToNewSide,
    TrainerCard_ScaleUpFlipAnimation,
    TrainerCard_FinishFlipAnimation,
};

// FIXME: Other signature than on save_menu_util.h
void FormatPlayTime(u8 *playtime, u16 hours, u16 minutes, s16 colon);
u16 GetPokedexSeenCount(void);

enum
{
    TRAINER_CARD_SETUP_TASK_IS_LINK,
    TRAINER_CARD_SETUP_TASK_CARD_INDEX,
    TRAINER_CARD_SETUP_TASK_CALLBACK,
};

enum
{
    TRAINER_CARD_STATE_INIT,
    TRAINER_CARD_STATE_WAIT_FOR_FADE_IN,
    TRAINER_CARD_STATE_WAIT_FOR_INPUT,
    TRAINER_CARD_STATE_START_FLIP,
    TRAINER_CARD_STATE_WAIT_FOR_FLIP,
    TRAINER_CARD_STATE_FADE_OUT,
    TRAINER_CARD_STATE_WAIT_FOR_FADE_OUT,
};

#define tTimeColonVisible         data[0]
#define tPreviousTimeColonVisible data[1]
#define tFlipState                data[0]
#define tCardTop                  data[1]

#define TRAINER_CARD_NUM_STAR_LEVELS 5

static void CB2_InitTrainerCard(void);
static void CB2_TrainerCard(void);
static void VblankCb_TrainerCard(void);
static void TrainerCard_InitScreenForPlayer(MainCallback callback);
static void TrainerCard_InitScreenForLinkPlayer(u8 cardId, MainCallback callback);
void TrainerCard_LoadDisplayedCard(void);
static void Task_TrainerCardSetup(u8 taskId);
static u32 GetCappedGameStat(u8 statId, u32 maxValue);
static u8 GetRubyTrainerStars(struct TrainerCard *trainerCard);
static void ResetTrainerCardDisplay(void);
static void EnableTrainerCardDisplay(void);
static void TrainerCard_ClearVram(void);
static void TrainerCard_ClearOam(void);
static void InitTrainerCardBgs(void);
static void InitTrainerCardData(void);
static void TrainerCard_SetDisplayFlags(void);
static void ResetTrainerCardTasksAndSprites(void);
static void InitTrainerCardWindow(void);
static void LoadTrainerCardGraphicsAndDrawCard(void);
static void TrainerCard_LoadTrainerGfx(void);
static void TrainerCard_InitNoop(void);
static void TrainerCard_StartStateMachine(void);
static void TrainerCard_CreateStateMachine(void);
static void TrainerCard_RunStateMachine(u8 taskId);
static void TrainerCard_CreatePrintPlayTimeTask(void);
static void TrainerCard_DestroyPlayTimeTask(void);
static void TrainerCard_Front_PrintPlayTime(u8 taskId);
static void TrainerCard_CreateFlipAnimationTask(void);
static u8 TrainerCard_HasFlipAnimationFinished(void);
static void TrainerCard_RunFlipAnimationStateMachine(u8 taskId);
static void TrainerCard_FlipAnimationHBlankCallback(void);
static void TrainerCard_DrawCard(void);
static void TrainerCard_DrawCardFront(void);
static void TrainerCard_DrawCardBack(void);
static void TrainerCard_ResetOffsetRegisters(void);
static void TrainerCard_CopyGraphics(void);
static void TrainerCard_LoadPalettes(void);
static void TrainerCard_LoadTrainerGraphics(void);
static void TrainerCard_LoadFrontTilemap(void);
static void TrainerCard_LoadBackTilemap(void);
static void TrainerCard_LoadBackgroundTilemap(void);
static void TrainerCard_LoadTrainerTilemap(void);
static void TrainerCard_DrawStars(void);
static void TrainerCard_DisplayBadges(void);
static void TrainerCard_ClearTrainerGraphics(void);
static void TrainerCard_ClearPokedexLabel(void);
static void TrainerCard_Front_PrintTexts(void);
static void TrainerCard_Back_PrintTexts(void);
static void TrainerCard_Front_PrintTrainerID(void);
static void TrainerCard_Front_PrintMoney(void);
static void TrainerCard_Front_PrintPokedexCount(void);
static void TrainerCard_GetFrontPlayTimeString(u8 *playTime, s16 colon);
static void TrainerCard_PrintEasyChatPhrase(void);
static void TrainerCard_Back_PrintName(void);
static void TrainerCard_Back_PrintHallOfFameTime_Label(void);
static void TrainerCard_Back_PrintHallOfFameTime(void);
static void TrainerCard_Back_PrintLinkBattlesLabel(void);
static void TrainerCard_Back_PrintLinkBattles(void);
static void TrainerCard_Back_PrintBattleTower_Label(void);
static void TrainerCard_Back_PrintBattleTower(void);
static void TrainerCard_Back_PrintLinkContests_Label(void);
static void TrainerCard_Back_PrintLinkContests(void);
static void TrainerCard_Back_PrintLinkPokeblocks_Label(void);
static void TrainerCard_Back_PrintLinkPokeblocks(void);
static void TrainerCard_Back_PrintPokemonTrades_Label(void);
static void TrainerCard_Back_PrintPokemonTrades(void);
void TrainerCard_PrintPlayerGender(u16 left, u16 top);

#if DEBUG
static u8 sDebugTrainerCardMode;
#endif

void ShowPlayerTrainerCard(MainCallback callback)
{
#if DEBUG
    sDebugTrainerCardMode = 0;
#endif
    TrainerCard_InitScreenForPlayer(callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sTrainerCardData->language = GAME_LANGUAGE;
}

void ShowTrainerCardInLink(u8 cardId, MainCallback callback)
{
#if DEBUG
    sDebugTrainerCardMode = 0;
#endif
    TrainerCard_InitScreenForLinkPlayer(cardId, callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sTrainerCardData->language = gLinkPlayers[gLinkPlayerObjectEvents[cardId].linkPlayerId].language;
}

#if DEBUG
void Debug_ShowPlayerTrainerCard(MainCallback callback)
{
    sDebugTrainerCardMode = TRUE;
    TrainerCard_InitScreenForPlayer(callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sTrainerCardData->language = GAME_LANGUAGE;
}

void Debug_ShowLinkTrainerCard(MainCallback callback)
{
    memcpy(&gTrainerCards[0], &sTestTrainerCard, sizeof(struct TrainerCard));
    sDebugTrainerCardMode = TRUE;
    TrainerCard_InitScreenForLinkPlayer(0, callback);
    SetMainCallback2(CB2_InitTrainerCard);
    sTrainerCardData->language = GAME_LANGUAGE;
}

void Debug_FillTrainerCardsWithTestData(void)
{
    int i;

    for (i = 0; i < MAX_LINK_PLAYERS; i++)
        memcpy(&gTrainerCards[i], &sTestTrainerCard, sizeof(struct TrainerCard));
}
#endif

static void CB2_InitTrainerCard(void)
{
    switch (gMain.state)
    {
    case 0:
        ResetTrainerCardDisplay();
        InitTrainerCardData();
        gMain.state++;
        break;
    case 1:
        TrainerCard_ClearVram();
        gMain.state++;
        break;
    case 2:
        TrainerCard_ClearOam();
        gMain.state++;
        break;
    case 3:
        InitTrainerCardBgs();
        ResetTrainerCardTasksAndSprites();
        gMain.state++;
        break;
    case 4:
        InitTrainerCardWindow();
        gMain.state++;
    case 5:
        if (MultistepInitMenuWindowContinue())
            gMain.state++;
        break;
    case 6:
        TrainerCard_LoadTrainerGfx();
        gMain.state++;
        break;
    case 7:
        LoadTrainerCardGraphicsAndDrawCard();
        gMain.state++;
        break;
    case 8:
        TrainerCard_InitNoop();
        TrainerCard_StartStateMachine();
        EnableTrainerCardDisplay();
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
    sTrainerCardData->timeColonBlinkTimer++;
    if (sTrainerCardData->timeColonBlinkTimer >= 60)
    {
        sTrainerCardData->timeColonBlinkTimer = 0;
        sTrainerCardData->timeColonVisible ^= 1;
    }
    if (sTrainerCardData->allowDmaCopy)
        DmaCopy16(3, &gScanlineEffectRegBuffers[0], &gScanlineEffectRegBuffers[1], 0x140);
}

static void TrainerCard_InitScreenForPlayer(MainCallback callBack)
{
    u8 taskId = CreateTask(Task_TrainerCardSetup, 0xFF);
    struct Task *task = &gTasks[taskId];
    task->data[TRAINER_CARD_SETUP_TASK_IS_LINK] = FALSE;
    StoreWordInTwoHalfwords(&task->data[TRAINER_CARD_SETUP_TASK_CALLBACK], (u32)callBack);
}

static void TrainerCard_InitScreenForLinkPlayer(u8 cardId, MainCallback callBack)
{
    u8 taskId = CreateTask(Task_TrainerCardSetup, 0xFF);

    struct Task *task = &gTasks[taskId];
    task->data[TRAINER_CARD_SETUP_TASK_IS_LINK] = TRUE;
    task->data[TRAINER_CARD_SETUP_TASK_CARD_INDEX] = cardId;
    StoreWordInTwoHalfwords(&task->data[TRAINER_CARD_SETUP_TASK_CALLBACK], (u32)callBack);
}

void TrainerCard_LoadDisplayedCard(void)
{
    u8 taskId = FindTaskIdByFunc(Task_TrainerCardSetup);
    struct Task *task = &gTasks[taskId];
    sTrainerCardData->isLink = task->data[TRAINER_CARD_SETUP_TASK_IS_LINK];

    LoadWordFromTwoHalfwords((u16 *)&task->data[TRAINER_CARD_SETUP_TASK_CALLBACK], (u32 *)&sTrainerCardData->callback2);

    if (sTrainerCardData->isLink)
    {
        sTrainerCardData->trainerCard = gTrainerCards[task->data[TRAINER_CARD_SETUP_TASK_CARD_INDEX]];
    }
    else
    {
        TrainerCard_GenerateCardForPlayer(&sTrainerCardData->trainerCard);
    }
}

static void Task_TrainerCardSetup(u8 taskId)
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
    trainerCard->pokedexSeen = GetPokedexSeenCount();

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

    for (i = 0; i < TRAINER_CARD_PROFILE_LENGTH; i++)
    {
        trainerCard->easyChatProfile[i] = gSaveBlock1.easyChats.easyChatProfile[i];
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

static void ResetTrainerCardDisplay(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    REG_DISPCNT = 0;
}

static void EnableTrainerCardDisplay(void)
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

static void TrainerCard_ClearVram(void)
{
    DmaFill16Large(3, 0, (void *)VRAM, 0x10000, 0x1000);
}

static void TrainerCard_ClearOam(void)
{
    DmaFill16Defvars(3, 0, (void *)OAM, 0x400);
}

static void InitTrainerCardBgs(void)
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

    TrainerCard_LoadDisplayedCard();
    sTrainerCardData->mainState = TRAINER_CARD_STATE_INIT;
    sTrainerCardData->onBack = FALSE;
    sTrainerCardData->allowDmaCopy = FALSE;
    sTrainerCardData->starCount = sTrainerCardData->trainerCard.stars;
    sTrainerCardData->timeColonVisible = 0;
    sTrainerCardData->timeColonBlinkTimer = 0;
    for (i = 0; i < TRAINER_CARD_PROFILE_LENGTH; i++)
        CopyEasyChatWord(sTrainerCardData->easyChatProfile[i], sTrainerCardData->trainerCard.easyChatProfile[i]);
    TrainerCard_SetDisplayFlags();
}

static void TrainerCard_SetDisplayFlags(void)
{
    sTrainerCardData->hasPokedex = 0;
    sTrainerCardData->hasHofResult = 0;
    sTrainerCardData->hasLinkResults = 0;
    sTrainerCardData->hasBattleTowerResults = 0;
    sTrainerCardData->hasLinkContestResults = 0;
    sTrainerCardData->hasLinkPokeblockResults = 0;
    sTrainerCardData->hasTrades = 0;
    memset(sTrainerCardData->badgeCount, 0, sizeof(sTrainerCardData->badgeCount));

    if (sTrainerCardData->trainerCard.hasPokedex)
        sTrainerCardData->hasPokedex++;

    if (sTrainerCardData->trainerCard.hofDebutHours != 0
     || sTrainerCardData->trainerCard.hofDebutMinutes != 0
     || sTrainerCardData->trainerCard.hofDebutSeconds != 0)
        sTrainerCardData->hasHofResult++;

    if (sTrainerCardData->trainerCard.linkBattleWins != 0 || sTrainerCardData->trainerCard.linkBattleLosses != 0)
        sTrainerCardData->hasLinkResults++;

    if (sTrainerCardData->trainerCard.battleTowerWins != 0 || sTrainerCardData->trainerCard.battleTowerStraightWins != 0)
        sTrainerCardData->hasBattleTowerResults++;

    if (sTrainerCardData->trainerCard.contestsWithFriends != 0)
        sTrainerCardData->hasLinkContestResults++;

    if (sTrainerCardData->trainerCard.pokeblocksWithFriends != 0)
        sTrainerCardData->hasLinkPokeblockResults++;

    if (sTrainerCardData->trainerCard.pokemonTrades != 0)
        sTrainerCardData->hasTrades++;

    if (!sTrainerCardData->isLink)
    {
        u32 badgeFlag;
        int i = 0;

        badgeFlag = FLAG_BADGE01_GET;
        while (1)
        {
            if (FlagGet(badgeFlag))
                sTrainerCardData->badgeCount[i]++;
            badgeFlag++;
            i++;
            if (badgeFlag > FLAG_BADGE08_GET)
            {
                break;
            }
        }
    }

#if DEBUG
    if (sDebugTrainerCardMode != 0)
    {
        sTrainerCardData->hasHofResult = TRUE;
        sTrainerCardData->hasLinkResults = TRUE;
        sTrainerCardData->hasBattleTowerResults = TRUE;
        sTrainerCardData->hasLinkContestResults = TRUE;
        sTrainerCardData->hasLinkPokeblockResults = TRUE;
        sTrainerCardData->hasTrades = TRUE;
        memset(sTrainerCardData->badgeCount, TRUE, sizeof(sTrainerCardData->badgeCount));
    }
#endif
}

static void ResetTrainerCardTasksAndSprites(void)
{
    ResetPaletteFade();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
}

static void InitTrainerCardWindow(void)
{
    Text_LoadWindowTemplate(&gWindowTemplate_TrainerCard_Back_Values);
    MultistepInitMenuWindowBegin(&gWindowTemplate_TrainerCard_Back_Values);
}

static void LoadTrainerCardGraphicsAndDrawCard()
{
    TrainerCard_ResetOffsetRegisters();
    TrainerCard_CopyGraphics();
    TrainerCard_LoadBackgroundTilemap();
    TrainerCard_DrawCard();
}

static void TrainerCard_LoadTrainerGfx()
{
    TrainerCard_LoadTrainerGraphics();
}

static void TrainerCard_InitNoop(void)
{
}

static void TrainerCard_StartStateMachine(void)
{
    TrainerCard_CreateStateMachine();
}

static void TrainerCard_CreateStateMachine(void)
{
    u8 taskId;

    taskId = CreateTask(TrainerCard_RunStateMachine, 0);
    TrainerCard_RunStateMachine(taskId);
}

static void TrainerCard_RunStateMachine(u8 taskId)
{
    while (sTrainerCardStateMachine[sTrainerCardData->mainState](&gTasks[taskId]) != 0)
        ;
}

bool8 TrainerCard_Init(struct Task *task)
{
    sTrainerCardData->timeColonVisible = gSaveBlock2.playTimeSeconds & 1;
    sTrainerCardData->timeColonBlinkTimer = gSaveBlock2.playTimeVBlanks;
    TrainerCard_CreatePrintPlayTimeTask();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    sTrainerCardData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForFadeInToFinish(struct Task *task)
{
    if (!gPaletteFade.active)
        sTrainerCardData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForKeys(struct Task *task)
{
    if (JOY_NEW(B_BUTTON))
    {
        sTrainerCardData->mainState = TRAINER_CARD_STATE_FADE_OUT;
        return TRUE;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        /* It appears that it was previously possible to return the the front side
           after viewing the back side. This was probably removed due to being
           unintuitive. */
        if (sTrainerCardData->onBack)
        {
            sTrainerCardData->mainState = TRAINER_CARD_STATE_FADE_OUT;
        }
        else
        {
            sTrainerCardData->onBack ^= TRUE;
            sTrainerCardData->mainState = TRAINER_CARD_STATE_START_FLIP;
        }
        return TRUE;
    }
#if DEBUG
    else if (sDebugTrainerCardMode && JOY_NEW(R_BUTTON))
    {
        sTrainerCardData->starCount++;
        sTrainerCardData->starCount %= TRAINER_CARD_NUM_STAR_LEVELS;
        TrainerCard_LoadPalettes();
        if (sTrainerCardData->onBack == 0)
            TrainerCard_DrawStars();
    }
#endif

    return FALSE;
}

bool8 TrainerCard_StartFlipAnimation(struct Task *task)
{
    TrainerCard_CreateFlipAnimationTask();
    PlaySE(SE_CARD);
    sTrainerCardData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForFlipToFinish(struct Task *task)
{
    if (TrainerCard_HasFlipAnimationFinished())
        sTrainerCardData->mainState = TRAINER_CARD_STATE_WAIT_FOR_INPUT;
    return FALSE;
}

bool8 TrainerCard_FadeOut(struct Task *task)
{
    TrainerCard_DestroyPlayTimeTask();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    sTrainerCardData->mainState++; /* Advance state machine */
    return FALSE;
}

bool8 TrainerCard_WaitForFadeOutToFinishAndQuit(struct Task *task)
{
    if (!gPaletteFade.active)
        SetMainCallback2(sTrainerCardData->callback2);
    return FALSE;
}

static void TrainerCard_CreatePrintPlayTimeTask(void)
{
    CreateTask(TrainerCard_Front_PrintPlayTime, 0);
    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Values);
}

static void TrainerCard_DestroyPlayTimeTask(void)
{
    u8 taskId = FindTaskIdByFunc(TrainerCard_Front_PrintPlayTime);

    if (taskId != 0xFF)
        DestroyTask(taskId);
}

static void TrainerCard_Front_PrintPlayTime(u8 taskId)
{
    u8 buffer[32];
    struct Task *task = &gTasks[taskId];

    if (sTrainerCardData->timeColonVisible != task->tPreviousTimeColonVisible)
    {
        task->tPreviousTimeColonVisible = sTrainerCardData->timeColonVisible;
        task->tTimeColonVisible ^= TRUE;
    }
    TrainerCard_GetFrontPlayTimeString(buffer, task->tTimeColonVisible);
    Menu_PrintText(buffer, 10, 12);
}

static void TrainerCard_CreateFlipAnimationTask(void)
{
    u8 taskId;

    taskId = CreateTask(TrainerCard_RunFlipAnimationStateMachine, 0);
    TrainerCard_RunFlipAnimationStateMachine(taskId);
}

static u8 TrainerCard_HasFlipAnimationFinished(void)
{
    if (FindTaskIdByFunc(TrainerCard_RunFlipAnimationStateMachine) == 0xFF)
        return TRUE;
    else
        return FALSE;
}

static void TrainerCard_RunFlipAnimationStateMachine(u8 taskId)
{
    while (sTrainerCardFlipAnimationStateMachine[gTasks[taskId].tFlipState](&gTasks[taskId]) != 0)
        ;
}

bool8 TrainerCard_InitFlipAnimation(struct Task *task)
{
    u32 i;

    sTrainerCardData->allowDmaCopy = FALSE;
    ScanlineEffect_Clear();
    for (i = 0; i < DISPLAY_HEIGHT; i++)
        gScanlineEffectRegBuffers[1][i] = -4;
    SetHBlankCallback(TrainerCard_FlipAnimationHBlankCallback);
    sTrainerCardData->allowDmaCopy = TRUE;
    task->tFlipState++;
    return FALSE;
}

bool8 TrainerCard_ScaleDownFlipAnimation(struct Task *task)
{
    s16 i;
    u32 cardHeight, r5, r10, cardTop, r6, var_24, cardBottom, var;

    sTrainerCardData->allowDmaCopy = FALSE;
    task->tCardTop += 3;
    if (task->tCardTop > 79)
        task->tCardTop = 79;

    cardTop = task->tCardTop;
    cardBottom = DISPLAY_HEIGHT - cardTop;
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
    for (var = var_24 >> 16; i < DISPLAY_HEIGHT;
        // WHAT?!
        gScanlineEffectRegBuffers[0][i] = var + -4,
        i++);

    sTrainerCardData->allowDmaCopy = TRUE;
    if (task->tCardTop > 74)
        task->tFlipState++;

    return FALSE;
}

bool8 TrainerCard_SwitchToNewSide(struct Task *task)
{
    TrainerCard_DestroyPlayTimeTask();
    TrainerCard_DrawCard();
    if (!sTrainerCardData->onBack) {
        /* This code never runs because it is impossible to flip the back side back to the front side */
        TrainerCard_CreatePrintPlayTimeTask();
    }
    task->tFlipState++;
    return TRUE;
}

bool8 TrainerCard_ScaleUpFlipAnimation(struct Task *task)
{
    s16 i;
    u32 cardHeight, r5, r10, cardTop, r6, var_24, cardBottom, var;

    sTrainerCardData->allowDmaCopy = FALSE;
    task->tCardTop -= 3;
    if (task->tCardTop <= 0)
        task->tCardTop = 0;

    cardTop = task->tCardTop;
    cardBottom = DISPLAY_HEIGHT - cardTop;
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
    for (var = var_24 >> 16; i < DISPLAY_HEIGHT;
        // WHAT?!
        gScanlineEffectRegBuffers[0][i] = var + -4,
        i++);

    sTrainerCardData->allowDmaCopy = TRUE;
    if (task->tCardTop <= 0)
        task->tFlipState++;

    return FALSE;
}

bool8 TrainerCard_FinishFlipAnimation(struct Task *task)
{
    u8 taskId;

    sTrainerCardData->allowDmaCopy = FALSE;
    SetHBlankCallback(NULL);
    TrainerCard_ResetOffsetRegisters();
    taskId = FindTaskIdByFunc(TrainerCard_RunFlipAnimationStateMachine);
    DestroyTask(taskId);
    return FALSE;
}

static void TrainerCard_FlipAnimationHBlankCallback(void)
{
    u16 bgVOffset = gScanlineEffectRegBuffers[1][REG_VCOUNT & 0xFF];

    REG_BG0VOFS = bgVOffset;
    REG_BG1VOFS = bgVOffset;
    REG_BG2VOFS = bgVOffset;
}

static void TrainerCard_DrawCard(void)
{
    if (sTrainerCardData->onBack)
        TrainerCard_DrawCardBack();
    else
        TrainerCard_DrawCardFront();
}

static void TrainerCard_DrawCardFront(void)
{
    Menu_EraseScreen();
    TrainerCard_ClearTrainerGraphics();
    TrainerCard_LoadFrontTilemap();
    TrainerCard_LoadTrainerTilemap();
    TrainerCard_DrawStars();
    TrainerCard_DisplayBadges();
    TrainerCard_Front_PrintTexts();
}

static void TrainerCard_DrawCardBack(void)
{
    Menu_EraseScreen();
    TrainerCard_ClearTrainerGraphics();
    TrainerCard_LoadBackTilemap();
    TrainerCard_Back_PrintTexts();
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
    LoadPalette(sUnused_Pal, 0xE0, 32);
    DmaCopyLarge16(3, gHoennTrainerCard_Gfx, (void *)VRAM, 0x1480, 0x1000);
    DmaCopy16Defvars(3, sHoennTrainerCardBadges_Gfx, (void *)(VRAM + 0x1480), 0x400);
}

static void TrainerCard_LoadPalettes(void)
{
    LoadPalette(sHoennTrainerCardPals[sTrainerCardData->starCount], 0, 48 * 2);
    LoadPalette(sHoennTrainerCardBadges_Pal, 48, 16 * 2);
    LoadPalette(sTrainerCardStar_Pal, 64, 16 * 2);
    if (sTrainerCardData->trainerCard.gender != MALE)
        LoadPalette(sHoennTrainerCardFemaleBg_Pal, 16, 16 * 2);
}

static void TrainerCard_LoadTrainerGraphics(void)
{
    LoadTrainerGfx_TrainerCard(sTrainerCardData->trainerCard.gender, 80, (void *)(VRAM + 0x1880));
}

static void TrainerCard_LoadFrontTilemap(void)
{
    const void *tilemaps[] = {gHoennTrainerCardFront_Tilemap, gHoennTrainerCardFrontLink_Tilemap};

    CpuFastSet(tilemaps[sTrainerCardData->isLink], (void *)(VRAM + 0x4800), 0x140);
}

static const u8 sText_HofTimeSeparator[] = _(" : ");

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
static const u16 sUnused_Pal[] = INCBIN_U16("graphics/trainer_card/unused.gbapal");
static const u16 sHoennTrainerCardBadges_Tilemap[][4] = INCBIN_U16("graphics/trainer_card/badges_map.bin");

static void TrainerCard_LoadBackTilemap(void)
{
    CpuFastSet(gHoennTrainerCardBack_Tilemap, (void *)(VRAM + 0x4800), 320);
}

static void TrainerCard_LoadBackgroundTilemap(void)
{
    CpuFastSet(gHoennTrainerCardBg_Tilemap, (void *)(VRAM + 0x5000), 320);
}

static void TrainerCard_LoadTrainerTilemap(void)
{
    u16 tileId = 0xC4;
    u16 *tilemap = (u16 *)(VRAM + 0x4000);
    s16 i;
    s16 j;

    for (i = 5; i < 13; i++)
    {
        for (j = 19; j < 27; j++, tileId++)
            tilemap[i * 32 + j] = tileId | 0x5000;
    }
}

static void TrainerCard_DrawStars(void)
{
    u16 *tilemap = (u16 *)(VRAM + 0x4000);
    s16 i = 15;
    s16 endTile = 15 + sTrainerCardData->starCount;

    while (i < endTile)
    {
        tilemap[6 * 32 + i] = 0x408F;
        i++;
    }
    while (i < 0x13)
    {
        tilemap[6 * 32 + i] = 0;
        i++;
    }
}

static void TrainerCard_DisplayBadges(void)
{
    if (!sTrainerCardData->isLink)
    {
        u16 *tilemap = (u16 *)(VRAM + 0x4000);
        s16 i;
        s16 tilemapLeft;

        for (i = 0, tilemapLeft = 4; i < 8; i++, tilemapLeft += 3)
        {
            if (sTrainerCardData->badgeCount[i] != 0)
            {
                tilemap[15 * 32 + tilemapLeft + 0] = sHoennTrainerCardBadges_Tilemap[i][0] | 0x3000;
                tilemap[15 * 32 + tilemapLeft + 1] = sHoennTrainerCardBadges_Tilemap[i][1] | 0x3000;
                tilemap[16 * 32 + tilemapLeft + 0] = sHoennTrainerCardBadges_Tilemap[i][2] | 0x3000;
                tilemap[16 * 32 + tilemapLeft + 1] = sHoennTrainerCardBadges_Tilemap[i][3] | 0x3000;
            }
        }
    }
}

static void TrainerCard_ClearTrainerGraphics(void)
{
    s16 i;
    u16 *tilemap;

    for (i = 0, tilemap = (u16 *)(VRAM + 0x4000); i < 0x400; i++, tilemap++)
        *tilemap = 0;
}

static void TrainerCard_ClearPokedexLabel(void)
{
    u16 *tilemap = (u16 *)(VRAM + 0x4800);
    u16 i;

    for (i = 3; i < 17; i++)
    {
        tilemap[10 * 32 + i] = 1;
        tilemap[11 * 32 + i] = 1;
    }
}

static void TrainerCard_Front_PrintTexts(void)
{
    u8 *buffer;

    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Values);

    buffer = gStringVar1;
    StringCopy(buffer, sTrainerCardData->trainerCard.playerName);
    ConvertInternationalString(buffer, sTrainerCardData->language);
    Menu_PrintText(buffer, 7, 5);

    TrainerCard_Front_PrintTrainerID();
    TrainerCard_Front_PrintMoney();
    TrainerCard_Front_PrintPokedexCount();
    TrainerCard_PrintEasyChatPhrase();
}

static void TrainerCard_Back_PrintTexts(void)
{
    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Values);
    TrainerCard_Back_PrintName();
    TrainerCard_Back_PrintHallOfFameTime_Label();
    TrainerCard_Back_PrintLinkBattlesLabel();
    TrainerCard_Back_PrintBattleTower_Label();
    TrainerCard_Back_PrintLinkContests_Label();
    TrainerCard_Back_PrintLinkPokeblocks_Label();
    TrainerCard_Back_PrintPokemonTrades_Label();

    BasicInitMenuWindow(&gWindowTemplate_TrainerCard_Back_Labels);
    TrainerCard_Back_PrintHallOfFameTime();
    TrainerCard_Back_PrintLinkBattles();
    TrainerCard_Back_PrintBattleTower();
    TrainerCard_Back_PrintLinkContests();
    TrainerCard_Back_PrintLinkPokeblocks();
    TrainerCard_Back_PrintPokemonTrades();
}

static void TrainerCard_Front_PrintTrainerID(void)
{
    u8 buffer[8];

    ConvertIntToDecimalStringN(buffer, sTrainerCardData->trainerCard.trainerId, STR_CONV_MODE_LEADING_ZEROS, 5);
    Menu_PrintText(buffer, 20, 2);
}

static void TrainerCard_Front_PrintMoney(void)
{
    PrintMoneyAmountRightAligned(sTrainerCardData->trainerCard.money, 16, 8);
}

static void TrainerCard_Front_PrintPokedexCount(void)
{
    u8 buffer[16];

    if (
#if DEBUG
     sDebugTrainerCardMode == 0 &&
#endif
     !sTrainerCardData->hasPokedex)
    {
        TrainerCard_ClearPokedexLabel();
    }
    else
    {
        ConvertIntToDecimalStringN(buffer, sTrainerCardData->trainerCard.pokedexSeen, STR_CONV_MODE_LEFT_ALIGN, 3);
        MenuPrint_RightAligned(buffer, 16, 10);
    }
}

static void TrainerCard_GetFrontPlayTimeString(u8 *playTimeString, s16 colon)
{
    u8 buffer[16];
    u16 playTimeHours;
    u16 playTimeMinutes;

    playTimeHours = gSaveBlock2.playTimeHours;
    playTimeMinutes = gSaveBlock2.playTimeMinutes;
    if (sTrainerCardData->isLink != 0)
    {
        playTimeHours = sTrainerCardData->trainerCard.playTimeHours;
        playTimeMinutes = sTrainerCardData->trainerCard.playTimeMinutes;
    }
    FormatPlayTime(buffer, playTimeHours, playTimeMinutes, colon);
    AlignStringInMenuWindow(playTimeString, buffer, 48, 1);
}

static void TrainerCard_PrintEasyChatPhrase(void)
{
    u8 *str;

    if (sTrainerCardData->isLink != 0)
    {
        str = gStringVar1;
        str = StringCopy(str, sTrainerCardData->easyChatProfile[0]);
        str[0] = 00;
        str++;
        str = StringCopy(str, sTrainerCardData->easyChatProfile[1]);
        Menu_PrintText(gStringVar1, 2, 14);

        str = gStringVar1;
        str = StringCopy(str, sTrainerCardData->easyChatProfile[2]);
        str[0] = 00;
        str++;
        str = StringCopy(str, sTrainerCardData->easyChatProfile[3]);
        Menu_PrintText(gStringVar1, 2, 16);
    }
}

static void TrainerCard_Back_PrintName(void)
{
    u8 *str;

    str = gStringVar1;
    StringCopy(str, sTrainerCardData->trainerCard.playerName);
    ConvertInternationalString(str, sTrainerCardData->language);

#if ENGLISH
    StringAppend(str, gOtherText_TrainersTrainerCard);
#elif GERMAN
    GermanStringFormatName(str, gOtherText_TrainersTrainerCard);
#endif

    MenuPrint_RightAligned(gStringVar1, 28, 2);
}

static void TrainerCard_Back_PrintHallOfFameTime_Label(void)
{
    if (sTrainerCardData->hasHofResult != 0)
        Menu_PrintText(gOtherText_FirstHOF, 3, 5);
}

static void TrainerCard_Back_PrintHallOfFameTime(void)
{
    u8 *str;

    if (sTrainerCardData->hasHofResult != 0)
    {
        str = gStringVar1;
        str = ConvertIntToDecimalStringN(str, sTrainerCardData->trainerCard.hofDebutHours, STR_CONV_MODE_RIGHT_ALIGN, 3);
        str = StringCopy(str, sText_HofTimeSeparator);
        str = ConvertIntToDecimalStringN(str, sTrainerCardData->trainerCard.hofDebutMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
        str = StringCopy(str, sText_HofTimeSeparator);
        str = ConvertIntToDecimalStringN(str, sTrainerCardData->trainerCard.hofDebutSeconds, STR_CONV_MODE_LEADING_ZEROS, 2);
        MenuPrint_RightAligned(gStringVar1, 28, 5);
    }
}

static void TrainerCard_Back_PrintLinkBattlesLabel(void)
{
    if (sTrainerCardData->hasLinkResults != 0)
        Menu_PrintText(gOtherText_LinkCableBattles, 3, 7);
}

static void TrainerCard_Back_PrintLinkBattles(void)
{
    u8 buffer[16];

    if (sTrainerCardData->hasLinkResults != 0)
    {
        ConvertIntToDecimalStringSimple(buffer, sTrainerCardData->trainerCard.linkBattleWins);
        MenuPrint_RightAligned(buffer, 22, 7);

        ConvertIntToDecimalStringSimple(buffer, sTrainerCardData->trainerCard.linkBattleLosses);
        MenuPrint_RightAligned(buffer, 28, 7);
    }
}

static void TrainerCard_Back_PrintBattleTower_Label(void)
{
    if (sTrainerCardData->hasBattleTowerResults != 0)
        Menu_PrintText(gOtherText_BattleTowerWinRecord, 3, 15);
}

static void TrainerCard_Back_PrintBattleTower(void)
{
    u8 buffer[16];

    if (sTrainerCardData->hasBattleTowerResults != 0)
    {
        AlignInt2InMenuWindow(buffer, sTrainerCardData->trainerCard.battleTowerWins, 24, 1);
        Menu_PrintTextPixelCoords(buffer, 112, 120, 0);

        AlignInt2InMenuWindow(buffer, sTrainerCardData->trainerCard.battleTowerStraightWins, 24, 1);
        Menu_PrintTextPixelCoords(buffer, 149, 120, 0);
    }
}

static void TrainerCard_Back_PrintLinkContests_Label(void)
{
    if (sTrainerCardData->hasLinkContestResults != 0)
        Menu_PrintText(gOtherText_ContestRecord, 3, 13);
}

static void TrainerCard_Back_PrintLinkContests(void)
{
    u8 buffer[8];

    if (sTrainerCardData->hasLinkContestResults != 0)
    {
        ConvertIntToDecimalStringN(buffer, sTrainerCardData->trainerCard.contestsWithFriends, STR_CONV_MODE_RIGHT_ALIGN, 3);
        MenuPrint_RightAligned(buffer, 28, 13);
    }
}

static void TrainerCard_Back_PrintLinkPokeblocks_Label(void)
{
    if (sTrainerCardData->hasLinkPokeblockResults != 0)
        Menu_PrintText(gOtherText_MixingRecord, 3, 11);
}

static void TrainerCard_Back_PrintLinkPokeblocks(void)
{
    u8 buffer[8];

    if (sTrainerCardData->hasLinkPokeblockResults != 0)
    {
        ConvertIntToDecimalStringN(buffer, sTrainerCardData->trainerCard.pokeblocksWithFriends, STR_CONV_MODE_RIGHT_ALIGN, 5);
        MenuPrint_RightAligned(buffer, 28, 11);
    }
}

static void TrainerCard_Back_PrintPokemonTrades_Label(void)
{
    if (sTrainerCardData->hasTrades != 0)
        Menu_PrintText(gOtherText_TradeRecord, 3, 9);
}

static void TrainerCard_Back_PrintPokemonTrades(void)
{
    u8 buffer[8];

    if (sTrainerCardData->hasTrades != 0)
    {
        ConvertIntToDecimalStringN(buffer, sTrainerCardData->trainerCard.pokemonTrades, STR_CONV_MODE_RIGHT_ALIGN, 5);
        MenuPrint_RightAligned(buffer, 28, 9);
    }
}

void TrainerCard_PrintPlayerGender(u16 left, u16 top)
{
    const u8 *text = gOtherText_Boy;

    if (gSaveBlock2.playerGender == FEMALE)
        text = gOtherText_Girl;
    Menu_PrintText(text, left, top);
}
