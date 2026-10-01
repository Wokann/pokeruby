#include "global.h"
#include "tv.h"
#include "battle_tower.h"
#include "contest.h"
#include "contest_painting.h"
#include "data2.h"
#include "easy_chat.h"
#include "event_data.h"
#include "fieldmap.h"
#include "field_message_box.h"
#include "field_camera.h"
#include "constants/flags.h"
#include "random.h"
#include "string_util.h"
#include "text.h"
#include "constants/species.h"
#include "pokedex.h"
#include "naming_screen.h"
#include "overworld.h"
#include "constants/maps.h"
#include "strings.h"
#include "battle.h"
#include "link.h"
#include "constants/easy_chat.h"
#include "constants/tv.h"
#include "event_object_movement.h"
#include "field_specials.h"
#include "item.h"
#include "shop.h"
#include "constants/items.h"
#include "link.h"
#include "constants/maps.h"
#include "naming_screen.h"
#include "pokedex.h"
#include "region_map.h"
#include "random.h"
#include "overworld.h"
#include "rtc.h"
#include "script_menu.h"
#include "constants/species.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "ewram.h"
#include "constants/event_objects.h"
#include "constants/moves.h"
#include "constants/region_map_sections.h"
#include "constants/metatile_labels.h"
#include "constants/script_menu.h"

#define LAST_TVSHOW_IDX (TV_SHOWS_COUNT - 1)

struct TVShowSlot
{
    s8 slot;
};

struct OutbreakPokemon
{
    /*0x00*/ u16 species;
    /*0x02*/ u16 moves[4];
    /*0x0A*/ u8 level;
    /*0x0B*/ u8 location;
};

extern u8 gContestMonPartyIndex;

struct TVShowSlot sCurTVShowSlot;

extern u16 gSpecialVar_LastTalked;

extern u16 gSpecialVar_ContestCategory;
extern u16 gSpecialVar_ContestRank;
extern u8 gBattleOutcome;
extern u16 gLastUsedItem;

static EWRAM_DATA u16 sPokemonAnglerSpecies = 0;
static EWRAM_DATA u16 sPokemonAnglerAttemptCounters = 0;
static EWRAM_DATA u8 sRecordMixingPartnersWithoutShowsToShare = 0;
static EWRAM_DATA ALIGNED(4) u8 sTVShowState = 0;

static u8 sTVShowMixingNumPlayers;
static u8 sTVShowNewsMixingNumPlayers;
static s8 sTVShowMixingCurSlot;

static const struct OutbreakPokemon sPokeOutbreakSpeciesList[5] =
{
    {
        .species = SPECIES_SURSKIT,
        .moves = {MOVE_BUBBLE, MOVE_QUICK_ATTACK, MOVE_NONE, MOVE_NONE},
        .level = 3,
        .location = MAPSEC_ROUTE_102,
    },
    {
        .species = SPECIES_SURSKIT,
        .moves = {MOVE_BUBBLE, MOVE_QUICK_ATTACK, MOVE_NONE, MOVE_NONE},
        .level = 15,
        .location = MAPSEC_ROUTE_114,
    },
    {
        .species = SPECIES_SURSKIT,
        .moves = {MOVE_BUBBLE, MOVE_QUICK_ATTACK, MOVE_NONE, MOVE_NONE},
        .level = 15,
        .location = MAPSEC_ROUTE_117,
    },
    {
        .species = SPECIES_SURSKIT,
        .moves = {MOVE_BUBBLE, MOVE_QUICK_ATTACK, MOVE_NONE, MOVE_NONE},
        .level = 28,
        .location = MAPSEC_ROUTE_120,
    },
    {
        .species = SPECIES_SKITTY,
        .moves = {MOVE_GROWL, MOVE_TACKLE, MOVE_NONE, MOVE_NONE},
        .level = 15,
        .location = MAPSEC_ROUTE_116,
    },
};

extern const u8 gPokeNewsTextSlateport_Upcoming[];
extern const u8 gPokeNewsTextGameCorner_Upcoming[];
extern const u8 gPokeNewsTextLilycove_Upcoming[];
extern const u8 gPokeNewsTextSlateport_Ongoing[];
extern const u8 gPokeNewsTextGameCorner_Ongoing[];
extern const u8 gPokeNewsTextLilycove_Ongoing[];
extern const u8 gPokeNewsTextSlateport_Ending[];
extern const u8 gPokeNewsTextGameCorner_Ending[];
extern const u8 gPokeNewsTextLilycove_Ending[];
extern const u8 gTVFanClubText00[];
extern const u8 gTVFanClubText01[];
extern const u8 gTVFanClubText02[];
extern const u8 gTVFanClubText03[];
extern const u8 gTVFanClubText04[];
extern const u8 gTVFanClubText05[];
extern const u8 gTVFanClubText06[];
extern const u8 gTVFanClubText07[];
extern const u8 gTVRecentHappeningsText00[];
extern const u8 gTVRecentHappeningsText01[];
extern const u8 gTVRecentHappeningsText02[];
extern const u8 gTVRecentHappeningsText03[];
extern const u8 gTVRecentHappeningsText04[];
extern const u8 gTVRecentHappeningsText05[];
extern const u8 gTVFanClubOpinionsText00[];
extern const u8 gTVFanClubOpinionsText01[];
extern const u8 gTVFanClubOpinionsText02[];
extern const u8 gTVFanClubOpinionsText03[];
extern const u8 gTVFanClubOpinionsText04[];
extern const u8 gTVMassOutbreakText00[];
extern const u8 gTVPokemonTodaySuccessfulText00[];
extern const u8 gTVPokemonTodaySuccessfulText01[];
extern const u8 gTVPokemonTodaySuccessfulText02[];
extern const u8 gTVPokemonTodaySuccessfulText03[];
extern const u8 gTVPokemonTodaySuccessfulText04[];
extern const u8 gTVPokemonTodaySuccessfulText05[];
extern const u8 gTVPokemonTodaySuccessfulText06[];
extern const u8 gTVPokemonTodaySuccessfulText07[];
extern const u8 gTVPokemonTodaySuccessfulText08[];
extern const u8 gTVPokemonTodaySuccessfulText09[];
extern const u8 gTVPokemonTodaySuccessfulText10[];
extern const u8 gTVPokemonTodaySuccessfulText11[];
extern const u8 SmartShopper_Text_Intro[];
extern const u8 SmartShopper_Text_ClerkNormal[];
extern const u8 SmartShopper_Text_RandomComment1[];
extern const u8 SmartShopper_Text_RandomComment2[];
extern const u8 SmartShopper_Text_RandomComment3[];
extern const u8 SmartShopper_Text_RandomComment4[];
extern const u8 SmartShopper_Text_SecondItem[];
extern const u8 SmartShopper_Text_ThirdItem[];
extern const u8 SmartShopper_Text_DuringSale[];
extern const u8 SmartShopper_Text_OutroNormal[];
extern const u8 SmartShopper_Text_IsVIP[];
extern const u8 SmartShopper_Text_ClerkMax[];
extern const u8 SmartShopper_Text_OutroMax[];
extern const u8 gTVBravoTrainerText00[];
extern const u8 gTVBravoTrainerText01[];
extern const u8 gTVBravoTrainerText02[];
extern const u8 gTVBravoTrainerText03[];
extern const u8 gTVBravoTrainerText04[];
extern const u8 gTVBravoTrainerText05[];
extern const u8 gTVBravoTrainerText06[];
extern const u8 gTVBravoTrainerText07[];
extern const u8 gTVBravoTrainerText08[];
extern const u8 BravoTrainerBattleTower_Text_Intro[];
extern const u8 BravoTrainerBattleTower_Text_NewRecord[];
extern const u8 BravoTrainerBattleTower_Text_Lost[];
extern const u8 BravoTrainerBattleTower_Text_Won[];
extern const u8 BravoTrainerBattleTower_Text_LostFinal[];
extern const u8 BravoTrainerBattleTower_Text_Satisfied[];
extern const u8 BravoTrainerBattleTower_Text_Unsatisfied[];
extern const u8 BravoTrainerBattleTower_Text_None1[];
extern const u8 BravoTrainerBattleTower_Text_None2[];
extern const u8 BravoTrainerBattleTower_Text_None3[];
extern const u8 BravoTrainerBattleTower_Text_None4[];
extern const u8 BravoTrainerBattleTower_Text_Response[];
extern const u8 BravoTrainerBattleTower_Text_ResponseSatisfied[];
extern const u8 BravoTrainerBattleTower_Text_ResponseUnsatisfied[];
extern const u8 BravoTrainerBattleTower_Text_Outro[];
extern const u8 gTVNameRaterText00[];
extern const u8 gTVNameRaterText01[];
extern const u8 gTVNameRaterText02[];
extern const u8 gTVNameRaterText03[];
extern const u8 gTVNameRaterText04[];
extern const u8 gTVNameRaterText05[];
extern const u8 gTVNameRaterText06[];
extern const u8 gTVNameRaterText07[];
extern const u8 gTVNameRaterText08[];
extern const u8 gTVNameRaterText09[];
extern const u8 gTVNameRaterText10[];
extern const u8 gTVNameRaterText11[];
extern const u8 gTVNameRaterText12[];
extern const u8 gTVNameRaterText13[];
extern const u8 gTVNameRaterText14[];
extern const u8 gTVNameRaterText15[];
extern const u8 gTVNameRaterText16[];
extern const u8 gTVNameRaterText17[];
extern const u8 gTVNameRaterText18[];
extern const u8 gTVPokemonTodayFailedText00[];
extern const u8 gTVPokemonTodayFailedText01[];
extern const u8 gTVPokemonTodayFailedText02[];
extern const u8 gTVPokemonTodayFailedText03[];
extern const u8 gTVPokemonTodayFailedText04[];
extern const u8 gTVPokemonTodayFailedText05[];
extern const u8 gTVPokemonTodayFailedText06[];
extern const u8 gTVPokemonAnglerText00[];
extern const u8 gTVPokemonAnglerText01[];
extern const u8 gTVWorldOfMastersText00[];
extern const u8 gTVWorldOfMastersText01[];
extern const u8 gTVWorldOfMastersText02[];
extern const u8 gTVInSearchOfTrainersText00[];
extern const u8 gTVInSearchOfTrainersText01[];
extern const u8 gTVInSearchOfTrainersText02[];
extern const u8 gTVInSearchOfTrainersText03[];
extern const u8 gTVInSearchOfTrainersText04[];
extern const u8 gTVInSearchOfTrainersText05[];
extern const u8 gTVInSearchOfTrainersText06[];
extern const u8 gTVInSearchOfTrainersText07[];
extern const u8 gTVInSearchOfTrainersText08[];

static const u8 *const sPokeNewsTextGroup_Upcoming[] =
{
    NULL,
    gPokeNewsTextSlateport_Upcoming,
    gPokeNewsTextGameCorner_Upcoming,
    gPokeNewsTextLilycove_Upcoming,
};

static const u8 *const sPokeNewsTextGroup_Ongoing[] =
{
    NULL,
    gPokeNewsTextSlateport_Ongoing,
    gPokeNewsTextGameCorner_Ongoing,
    gPokeNewsTextLilycove_Ongoing,
};

static const u8 *const sPokeNewsTextGroup_Ending[] =
{
    NULL,
    gPokeNewsTextSlateport_Ending,
    gPokeNewsTextGameCorner_Ending,
    gPokeNewsTextLilycove_Ending,
};

u8 *const gTVStringVarPtrs[] =
{
    gStringVar1,
    gStringVar2,
    gStringVar3,
};

static const u8 *const sTVFanClubTextGroup[] =
{
    gTVFanClubText00,
    gTVFanClubText01,
    gTVFanClubText02,
    gTVFanClubText03,
    gTVFanClubText04,
    gTVFanClubText05,
    gTVFanClubText06,
    gTVFanClubText07,
};

static const u8 *const sTVRecentHappeninssTextGroup[] =
{
    gTVRecentHappeningsText00,
    gTVRecentHappeningsText01,
    gTVRecentHappeningsText02,
    gTVRecentHappeningsText03,
    gTVRecentHappeningsText04,
    gTVRecentHappeningsText05,
};

static const u8 *const sTVFanClubOpinionsTextGroup[] =
{
    gTVFanClubOpinionsText00,
    gTVFanClubOpinionsText01,
    gTVFanClubOpinionsText02,
    gTVFanClubOpinionsText03,
    gTVFanClubOpinionsText04,
};

static const u8 *const sTVMassOutbreakTextGroup[] =
{
    gTVMassOutbreakText00,
};

static const u8 *const sTVPokemonTodaySuccessfulTextGroup[] =
{
    gTVPokemonTodaySuccessfulText00,
    gTVPokemonTodaySuccessfulText01,
    gTVPokemonTodaySuccessfulText02,
    gTVPokemonTodaySuccessfulText03,
    gTVPokemonTodaySuccessfulText04,
    gTVPokemonTodaySuccessfulText05,
    gTVPokemonTodaySuccessfulText06,
    gTVPokemonTodaySuccessfulText07,
    gTVPokemonTodaySuccessfulText08,
    gTVPokemonTodaySuccessfulText09,
    gTVPokemonTodaySuccessfulText10,
    gTVPokemonTodaySuccessfulText11,
};

static const u8 *const sTVTodaysSmartShopperTextGroup[] =
{
    SmartShopper_Text_Intro,
    SmartShopper_Text_ClerkNormal,
    SmartShopper_Text_RandomComment1,
    SmartShopper_Text_RandomComment2,
    SmartShopper_Text_RandomComment3,
    SmartShopper_Text_RandomComment4,
    SmartShopper_Text_SecondItem,
    SmartShopper_Text_ThirdItem,
    SmartShopper_Text_DuringSale,
    SmartShopper_Text_OutroNormal,
    SmartShopper_Text_IsVIP,
    SmartShopper_Text_ClerkMax,
    SmartShopper_Text_OutroMax,
};

static const u8 *const sTVBravoTrainerTextGroup[] =
{
    gTVBravoTrainerText00,
    gTVBravoTrainerText01,
    gTVBravoTrainerText02,
    gTVBravoTrainerText03,
    gTVBravoTrainerText04,
    gTVBravoTrainerText05,
    gTVBravoTrainerText06,
    gTVBravoTrainerText07,
    gTVBravoTrainerText08,
};

static const u8 *const sTVBravoTrainerBattleTowerTextGroup[] =
{
    [BRAVOTOWER_STATE_INTRO]                = BravoTrainerBattleTower_Text_Intro,
    [BRAVOTOWER_STATE_NEW_RECORD]           = BravoTrainerBattleTower_Text_NewRecord,
    [BRAVOTOWER_STATE_LOST]                 = BravoTrainerBattleTower_Text_Lost,
    [BRAVOTOWER_STATE_WON]                  = BravoTrainerBattleTower_Text_Won,
    [BRAVOTOWER_STATE_LOST_FINAL]           = BravoTrainerBattleTower_Text_LostFinal,
    [BRAVOTOWER_STATE_SATISFIED]            = BravoTrainerBattleTower_Text_Satisfied,
    [BRAVOTOWER_STATE_UNSATISFIED]          = BravoTrainerBattleTower_Text_Unsatisfied,
    [BRAVOTOWER_STATE_UNUSED_1]             = BravoTrainerBattleTower_Text_None1,
    [BRAVOTOWER_STATE_UNUSED_2]             = BravoTrainerBattleTower_Text_None2,
    [BRAVOTOWER_STATE_UNUSED_3]             = BravoTrainerBattleTower_Text_None3,
    [BRAVOTOWER_STATE_UNUSED_4]             = BravoTrainerBattleTower_Text_None4,
    [BRAVOTOWER_STATE_RESPONSE]             = BravoTrainerBattleTower_Text_Response,
    [BRAVOTOWER_STATE_RESPONSE_SATISFIED]   = BravoTrainerBattleTower_Text_ResponseSatisfied,
    [BRAVOTOWER_STATE_RESPONSE_UNSATISFIED] = BravoTrainerBattleTower_Text_ResponseUnsatisfied,
    [BRAVOTOWER_STATE_OUTRO]                = BravoTrainerBattleTower_Text_Outro,
};

static const u8 *const sTVNameRaterTextGroup[] =
{
    gTVNameRaterText00,
    gTVNameRaterText01,
    gTVNameRaterText02,
    gTVNameRaterText03,
    gTVNameRaterText04,
    gTVNameRaterText05,
    gTVNameRaterText06,
    gTVNameRaterText07,
    gTVNameRaterText08,
    gTVNameRaterText09,
    gTVNameRaterText10,
    gTVNameRaterText11,
    gTVNameRaterText12,
    gTVNameRaterText13,
    gTVNameRaterText14,
    gTVNameRaterText15,
    gTVNameRaterText16,
    gTVNameRaterText17,
    gTVNameRaterText18,
};

static const u8 *const sTVPokemonTodayFailedTextGroup[] =
{
    gTVPokemonTodayFailedText00,
    gTVPokemonTodayFailedText01,
    gTVPokemonTodayFailedText02,
    gTVPokemonTodayFailedText03,
    gTVPokemonTodayFailedText04,
    gTVPokemonTodayFailedText05,
    gTVPokemonTodayFailedText06,
};

static const u8 *const sTVPokemonAnglerTextGroup[] =
{
    gTVPokemonAnglerText00,
    gTVPokemonAnglerText01,
};

static const u8 *const sTVWorldOfMastersTextGroup[] =
{
    gTVWorldOfMastersText00,
    gTVWorldOfMastersText01,
    gTVWorldOfMastersText02,
};

static const u8 *const sTVInSearchOfTrainersTextGroup[] =
{
    gTVInSearchOfTrainersText00,
    gTVInSearchOfTrainersText01,
    gTVInSearchOfTrainersText02,
    gTVInSearchOfTrainersText03,
    gTVInSearchOfTrainersText04,
    gTVInSearchOfTrainersText05,
    gTVInSearchOfTrainersText06,
    gTVInSearchOfTrainersText07,
    gTVInSearchOfTrainersText08,
};

void ClearTVShowData(void)
{
    u8 showIdx;
    u8 dataIdx;

    for (showIdx = 0; showIdx < TV_SHOWS_COUNT; showIdx++)
    {
        gSaveBlock1.tvShows[showIdx].common.kind = TVSHOW_OFF_AIR;
        gSaveBlock1.tvShows[showIdx].common.active = FALSE;
        for (dataIdx = 0; dataIdx < ARRAY_COUNT(gSaveBlock1.tvShows[showIdx].commonInit.data); dataIdx++)
            gSaveBlock1.tvShows[showIdx].commonInit.data[dataIdx] = 0;
    }
    ClearPokeNews();
}

bool8 IsRecordMixShowAlreadySpawned(u8);
void SortPurchasesByQuantity(void);

void InterviewBefore_FanClubLetter(void);
void InterviewBefore_RecentHappenings(void);
void InterviewBefore_PkmnFanClubOpinions(void);
void InterviewBefore_Dummy(void);
void InterviewBefore_NameRater(void);
void InterviewBefore_BravoTrainerPkmnProfile(void);
void InterviewBefore_BravoTrainerBTProfile(void);


u8 GetTVGroupByShowId(u8);


void InterviewAfter_FanClubLetter(void);
void InterviewAfter_RecentHappenings(void);
void InterviewAfter_PkmnFanClubOpinions(void);
void InterviewAfter_Dummy(void);
void InterviewAfter_BravoTrainerPokemonProfile(void);
void InterviewAfter_BravoTrainerBattleTowerProfile(void);

u8 GetRandomActiveShowIdx(void)
{
    u8 i;
    u8 j;
    u8 selIdx;
    TVShow *show;

    for (i = NUM_NORMAL_TVSHOW_SLOTS; i < LAST_TVSHOW_IDX; i++)
    {
        if (gSaveBlock1.tvShows[i].common.kind == TVSHOW_OFF_AIR)
        {
            break;
        }
    }

    j = Random() % i;
    selIdx = j;

    do
    {
        if (GetTVGroupByShowId(gSaveBlock1.tvShows[j].common.kind) != TVGROUP_OUTBREAK)
        {
            if (gSaveBlock1.tvShows[j].common.active == TRUE)
            {
                return j;
            }
        }
        else
        {
            show = &gSaveBlock1.tvShows[j];
            if (show->massOutbreak.daysBeforeOutbreak == 0 && show->massOutbreak.active == TRUE)
            {
                return j;
            }
        }
        if (j == 0)
        {
            j = LAST_TVSHOW_IDX - 1;
        }
        else
        {
            j--;
        }
    } while (j != selIdx);

    return 0xFF;
}

u8 FindFirstActiveTVShowThatIsNotAMassOutbreak(void);

u8 FindAnyTVShowOnTheAir(void)
{
    u8 slot = GetRandomActiveShowIdx();

    if (slot == 0xff)
        return 0xff;
    if (gSaveBlock1.outbreakPokemonSpecies != 0 && gSaveBlock1.tvShows[slot].common.kind == TVSHOW_MASS_OUTBREAK)
        return FindFirstActiveTVShowThatIsNotAMassOutbreak();
    return slot;
}

u8 CheckForPlayersHouseNews(void);
void SetTVMetatilesOnMap(int, int, u16);
bool8 FindAnyPokeNewsOnTheAir(void);
bool8 IsGabbyAndTyShowOnTheAir(void);

void UpdateTVScreensOnMap(int width, int height)
{
    FlagSet(FLAG_SYS_TV_WATCH);
    switch (CheckForPlayersHouseNews())
    {
    case 1:
        SetTVMetatilesOnMap(width, height, METATILE_ID(Building, TV_On));
        break;
    case 2:
        break;
    default:
        if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_LILYCOVE_CITY_COVE_LILY_MOTEL_1F)
         && gSaveBlock1.location.mapNum == MAP_NUM(MAP_LILYCOVE_CITY_COVE_LILY_MOTEL_1F))
        {
            SetTVMetatilesOnMap(width, height, METATILE_ID(Building, TV_On));
        }
        else if (FlagGet(FLAG_SYS_TV_START) && (FindAnyTVShowOnTheAir() != 0xff || FindAnyPokeNewsOnTheAir() != 0xff || IsGabbyAndTyShowOnTheAir()))
        {
            FlagClear(FLAG_SYS_TV_WATCH);
            SetTVMetatilesOnMap(width, height, METATILE_ID(Building, TV_On));
        }
        break;
    }
}

void SetTVMetatilesOnMap(int width, int height, u16 tileId)
{
    int x;
    int y;

    for (y=0; y<height; y++)
    {
        for (x=0; x<width; x++)
        {
            if (MapGridGetMetatileBehaviorAt(x, y) == 0x86)
                MapGridSetMetatileIdAt(x, y, tileId | MAPGRID_IMPASSABLE);
        }
    }
}

void TurnOffTVScreen(void)
{
    SetTVMetatilesOnMap(gBackupMapLayout.width, gBackupMapLayout.height, METATILE_ID(Building, TV_Off));
    DrawWholeMapView();
}

u8 GetSelectedTVShow(void)
{
    return gSaveBlock1.tvShows[gSpecialVar_0x8004].common.kind;
}

u8 FindFirstActiveTVShowThatIsNotAMassOutbreak(void)
{
    u8 showIdx;
    for (showIdx = 0; showIdx < LAST_TVSHOW_IDX; showIdx++)
    {
        if (gSaveBlock1.tvShows[showIdx].common.kind != TVSHOW_OFF_AIR
         && gSaveBlock1.tvShows[showIdx].common.kind != TVSHOW_MASS_OUTBREAK
         && gSaveBlock1.tvShows[showIdx].common.active == TRUE)
            return showIdx;
    }
    return 0xff;
}

u8 GetNextActiveShowIfMassOutbreak(void)
{
    TVShow *tvShow;
    tvShow = &gSaveBlock1.tvShows[gSpecialVar_0x8004];
    if (tvShow->common.kind == TVSHOW_MASS_OUTBREAK && gSaveBlock1.outbreakPokemonSpecies)
        return FindFirstActiveTVShowThatIsNotAMassOutbreak();
    return gSpecialVar_0x8004;
}

void ResetGabbyAndTy(void)
{
    gSaveBlock1.gabbyAndTyData.mon1 = 0;
    gSaveBlock1.gabbyAndTyData.mon2 = 0;
    gSaveBlock1.gabbyAndTyData.lastMove = 0;
    gSaveBlock1.gabbyAndTyData.quote = 0xffff;
    gSaveBlock1.gabbyAndTyData.battleTookMoreThanOneTurn = 0;
    gSaveBlock1.gabbyAndTyData.playerLostAMon = 0;
    gSaveBlock1.gabbyAndTyData.playerUsedHealingItem = 0;
    gSaveBlock1.gabbyAndTyData.playerThrewABall = 0;
    gSaveBlock1.gabbyAndTyData.onAir = 0;
    gSaveBlock1.gabbyAndTyData.valA_5 = 0;
    gSaveBlock1.gabbyAndTyData.battleTookMoreThanOneTurn2 = 0;
    gSaveBlock1.gabbyAndTyData.playerLostAMon2 = 0;
    gSaveBlock1.gabbyAndTyData.playerUsedHealingItem2 = 0;
    gSaveBlock1.gabbyAndTyData.playerThrewABall2 = 0;
    gSaveBlock1.gabbyAndTyData.valB_4 = 0;
    gSaveBlock1.gabbyAndTyData.valB_5 = 0;
    gSaveBlock1.gabbyAndTyData.mapnum = 0;
    gSaveBlock1.gabbyAndTyData.battleNum = 0;
}

void TakeGabbyAndTyOffTheAir(void);

void GabbyAndTyBeforeInterview(void)
{
    u8 i;

    gSaveBlock1.gabbyAndTyData.mon1 = gBattleResults.playerMon1Species;
    gSaveBlock1.gabbyAndTyData.mon2 = gBattleResults.playerMon2Species;
    gSaveBlock1.gabbyAndTyData.lastMove = gBattleResults.lastUsedMovePlayer;
    if (gSaveBlock1.gabbyAndTyData.battleNum != 0xff)
        gSaveBlock1.gabbyAndTyData.battleNum ++;
    gSaveBlock1.gabbyAndTyData.battleTookMoreThanOneTurn = gBattleResults.playerMonWasDamaged;

    if (gBattleResults.playerFaintCounter)
        gSaveBlock1.gabbyAndTyData.playerLostAMon = 1;
    else
        gSaveBlock1.gabbyAndTyData.playerLostAMon = 0;

    if (gBattleResults.numHealingItemsUsed)
        gSaveBlock1.gabbyAndTyData.playerUsedHealingItem = 1;
    else
        gSaveBlock1.gabbyAndTyData.playerUsedHealingItem = 0;

    if (!gBattleResults.usedMasterBall)
    {
        for (i=0; i<11; i++)
        {
            if (gBattleResults.catchAttempts[i] != 0)
            {
                gSaveBlock1.gabbyAndTyData.playerThrewABall = 1;
                break;
            }
        }
    }
    else
    {
        gSaveBlock1.gabbyAndTyData.playerThrewABall = 1;
    }

    TakeGabbyAndTyOffTheAir();
    if (gSaveBlock1.gabbyAndTyData.lastMove == 0)
        FlagSet(FLAG_TEMP_1);
}

void GabbyAndTyAfterInterview(void)
{
    gSaveBlock1.gabbyAndTyData.battleTookMoreThanOneTurn2 = gSaveBlock1.gabbyAndTyData.battleTookMoreThanOneTurn;
    gSaveBlock1.gabbyAndTyData.playerLostAMon2 = gSaveBlock1.gabbyAndTyData.playerLostAMon;
    gSaveBlock1.gabbyAndTyData.playerUsedHealingItem2 = gSaveBlock1.gabbyAndTyData.playerUsedHealingItem;
    gSaveBlock1.gabbyAndTyData.playerThrewABall2 = gSaveBlock1.gabbyAndTyData.playerThrewABall;
    gSaveBlock1.gabbyAndTyData.onAir = 1;
    gSaveBlock1.gabbyAndTyData.mapnum = gMapHeader.regionMapSectionId;
    IncrementGameStat(GAME_STAT_GOT_INTERVIEWED);
}

void TakeGabbyAndTyOffTheAir(void)
{
    gSaveBlock1.gabbyAndTyData.onAir = 0;
}

u8 GabbyAndTyGetBattleNum(void)
{
    if (gSaveBlock1.gabbyAndTyData.battleNum >= 6)
        return (gSaveBlock1.gabbyAndTyData.battleNum % 3) + 6;
    else
        return gSaveBlock1.gabbyAndTyData.battleNum;
}

bool8 IsGabbyAndTyShowOnTheAir(void)
{
    return gSaveBlock1.gabbyAndTyData.onAir;
}

bool8 GabbyAndTyGetLastQuote(void)
{
    if (gSaveBlock1.gabbyAndTyData.quote == 0xffff)
        return FALSE;

    CopyEasyChatWord(gStringVar1, gSaveBlock1.gabbyAndTyData.quote);
    gSaveBlock1.gabbyAndTyData.quote |= 0xffff;
    return TRUE;
}

u8 GabbyAndTyGetLastBattleTrivia(void)
{
    if (!gSaveBlock1.gabbyAndTyData.battleTookMoreThanOneTurn2)
        return 1;
    if (gSaveBlock1.gabbyAndTyData.playerThrewABall2)
        return 2;
    if (gSaveBlock1.gabbyAndTyData.playerUsedHealingItem2)
        return 3;
    if (gSaveBlock1.gabbyAndTyData.playerLostAMon2)
        return 4;
    return 0;
}

void GetGabbyAndTyLocalIds(void)
{
    switch (GabbyAndTyGetBattleNum())
    {
    case 1:
        gSpecialVar_0x8004 = LOCALID_ROUTE111_GABBY_1;
        gSpecialVar_0x8005 = LOCALID_ROUTE111_TY_1;
        break;
    case 2:
        gSpecialVar_0x8004 = LOCALID_ROUTE118_GABBY_1;
        gSpecialVar_0x8005 = LOCALID_ROUTE118_TY_1;
        break;
    case 3:
        gSpecialVar_0x8004 = LOCALID_ROUTE120_GABBY_1;
        gSpecialVar_0x8005 = LOCALID_ROUTE120_TY_1;
        break;
    case 4:
        gSpecialVar_0x8004 = LOCALID_ROUTE111_GABBY_2;
        gSpecialVar_0x8005 = LOCALID_ROUTE111_TY_2;
        break;
    case 5:
        gSpecialVar_0x8004 = LOCALID_ROUTE118_GABBY_2;
        gSpecialVar_0x8005 = LOCALID_ROUTE118_TY_2;
        break;
    case 6:
        gSpecialVar_0x8004 = LOCALID_ROUTE120_GABBY_2;
        gSpecialVar_0x8005 = LOCALID_ROUTE120_TY_2;
        break;
    case 7:
        gSpecialVar_0x8004 = LOCALID_ROUTE111_GABBY_3;
        gSpecialVar_0x8005 = LOCALID_ROUTE111_TY_3;
        break;
    case 8:
        gSpecialVar_0x8004 = LOCALID_ROUTE118_GABBY_3;
        gSpecialVar_0x8005 = LOCALID_ROUTE118_TY_3;
        break;
    }
}

void InterviewAfter(void)
{
    switch (gSpecialVar_0x8005)
    {
    case TVSHOW_FAN_CLUB_LETTER:
        InterviewAfter_FanClubLetter();
        break;
    case TVSHOW_RECENT_HAPPENINGS:
        InterviewAfter_RecentHappenings();
        break;
    case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
        InterviewAfter_PkmnFanClubOpinions();
        break;
    case TVSHOW_DUMMY:
        InterviewAfter_Dummy();
        break;
    case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
        InterviewAfter_BravoTrainerPokemonProfile();
        break;
    case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
        InterviewAfter_BravoTrainerBattleTowerProfile();
        break;
    }
}

static u8 GetStringLanguage(u8 *str)
{
    u8 lang = GAME_LANGUAGE;

    if (str[0] == EXT_CTRL_CODE_BEGIN && str[1] == 0x15)
        lang = LANGUAGE_JAPANESE;
    return lang;
}

void TryPutPokemonTodayOnAir(void)
{
    u8 i;
    u16 ballsUsed;
    u16 itemLastUsed;

    ballsUsed = 0;
    TryPutRandomPokeNewsOnAir();
    TryStartRandomMassOutbreak();

    if (gBattleResults.caughtMonSpecies == 0)
    {
        TryPutPokemonTodayFailedOnTheAir();
    }
    else
    {
        InitWorldOfMastersShowAttempt();
        if (BernoulliTrial(0xffff) == 0 && StringCompareWithoutExtCtrlCodes(gSpeciesNames[gBattleResults.caughtMonSpecies], gBattleResults.caughtMonNick) != 0)
        {
            sCurTVShowSlot.slot = FindFirstEmptyRecordMixTVShowSlot(gSaveBlock1.tvShows);
            if (sCurTVShowSlot.slot != -1 && IsRecordMixShowAlreadySpawned(TVSHOW_POKEMON_TODAY_CAUGHT) != 1)
            {
                for (i = 0; i < 11; i++)
                    ballsUsed += gBattleResults.catchAttempts[i];
                if (ballsUsed != 0 || gBattleResults.usedMasterBall != 0)
                {
                    struct TVShowPokemonToday *pokemonToday;

                    ballsUsed = 0;
                    pokemonToday = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].pokemonToday;
                    pokemonToday->kind = TVSHOW_POKEMON_TODAY_CAUGHT;
                    pokemonToday->active = ballsUsed;
                    if (gBattleResults.usedMasterBall != 0)
                    {
                        ballsUsed = 1;
                        itemLastUsed = ITEM_MASTER_BALL;
                    }
                    else
                    {
                        for (i = 0; i < 11; i++)
                            ballsUsed += gBattleResults.catchAttempts[i];
                        if (ballsUsed > 0xff)
                            ballsUsed = 0xff;
                        itemLastUsed = gLastUsedItem;
                    }
                    pokemonToday->nBallsUsed = ballsUsed;
                    pokemonToday->ball = itemLastUsed;
                    StringCopy(pokemonToday->playerName, gSaveBlock2.playerName);
                    StringCopy(pokemonToday->nickname, gBattleResults.caughtMonNick);
                    pokemonToday->species = gBattleResults.caughtMonSpecies;
                    StorePlayerIdInRecordMixShow((TVShow *)pokemonToday);
                    pokemonToday->language = GAME_LANGUAGE;
                    pokemonToday->language2 = GetStringLanguage(pokemonToday->nickname);
                    StripExtCtrlCodes(pokemonToday->nickname);
                }
            }
        }
    }
}

void InitWorldOfMastersShowAttempt(void)
{
    struct TVShowWorldOfMasters *worldOfMasters = &gSaveBlock1.tvShows[24].worldOfMasters;

    if (worldOfMasters->kind != TVSHOW_WORLD_OF_MASTERS)
    {
        DeleteTVShowInArrayByIdx(gSaveBlock1.tvShows, 24);
        worldOfMasters->steps = GetGameStat(GAME_STAT_STEPS);
        worldOfMasters->kind = TVSHOW_WORLD_OF_MASTERS;
    }
    worldOfMasters->numPokeCaught++;
    worldOfMasters->caughtPoke = gBattleResults.caughtMonSpecies;
    worldOfMasters->species = gBattleResults.playerMon1Species;
    worldOfMasters->location = gMapHeader.regionMapSectionId;
}

void TryPutPokemonTodayFailedOnTheAir(void)
{
    u8 i;
    u16 ballsUsed;
    u8 zero = 0;

    if (BernoulliTrial(0xffff) == 0)
    {
        for (i = 0, ballsUsed = 0; i < 11; i++)
            ballsUsed += gBattleResults.catchAttempts[i];
        if (ballsUsed > 0xff)
            ballsUsed = 0xff;
        if (ballsUsed > 2 && gBattleOutcome == 1)
        {
            sCurTVShowSlot.slot = FindFirstEmptyRecordMixTVShowSlot(gSaveBlock1.tvShows);
            if (sCurTVShowSlot.slot != -1 && IsRecordMixShowAlreadySpawned(TVSHOW_POKEMON_TODAY_FAILED) != 1)
            {
                struct TVShowPokemonTodayFailed *pokemonTodayFailed = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].pokemonTodayFailed;

                zero = 0;
                pokemonTodayFailed->kind = TVSHOW_POKEMON_TODAY_FAILED;
                pokemonTodayFailed->active = zero;
                pokemonTodayFailed->species = gBattleResults.playerMon1Species;
                pokemonTodayFailed->species2 = gBattleResults.lastOpponentSpecies;
                pokemonTodayFailed->nBallsUsed = ballsUsed;
                pokemonTodayFailed->outcome = gBattleOutcome;
                pokemonTodayFailed->location = gMapHeader.regionMapSectionId;
                StringCopy(pokemonTodayFailed->playerName, gSaveBlock2.playerName);
                StorePlayerIdInRecordMixShow((TVShow *)pokemonTodayFailed);
                pokemonTodayFailed->language = GAME_LANGUAGE;
            }
        }
    }
}


void StorePlayerIdInRecordMixShow(TVShow *show)
{
    u32 playerId = GetPlayerIDAsU32();

    show->common.srcTrainerId2Lo = playerId & 0xFF;
    show->common.srcTrainerId2Hi = playerId >> 8;
    show->common.srcTrainerIdLo = playerId & 0xFF;
    show->common.srcTrainerIdHi = playerId >> 8;
    show->common.trainerIdLo = playerId & 0xFF;
    show->common.trainerIdHi = playerId >> 8;
}

void StorePlayerIdInNormalShow(TVShow *show)
{
    u32 playerId = GetPlayerIDAsU32();

    show->common.srcTrainerIdLo = playerId & 0xFF;
    show->common.srcTrainerIdHi = playerId >> 8;
    show->common.trainerIdLo = playerId & 0xFF;
    show->common.trainerIdHi = playerId >> 8;
}

void InterviewAfter_BravoTrainerPokemonProfile(void)
{
    struct TVShowBravoTrainerPokemonProfiles *bravoTrainerSrc = &gSaveBlock1.tvShows[24].bravoTrainer;

    if (bravoTrainerSrc->kind == TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE)
    {
        struct TVShowBravoTrainerPokemonProfiles *bravoTrainerNew = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].bravoTrainer;

        bravoTrainerNew->kind = TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE;
        bravoTrainerNew->active = 1;
        bravoTrainerNew->species = bravoTrainerSrc->species;
        StringCopy(bravoTrainerNew->playerName, gSaveBlock2.playerName);
        StringCopy(bravoTrainerNew->pokemonNickname, bravoTrainerSrc->pokemonNickname);
        bravoTrainerNew->contestCategory = bravoTrainerSrc->contestCategory;
        bravoTrainerNew->contestRank = bravoTrainerSrc->contestRank;
        bravoTrainerNew->move = bravoTrainerSrc->move;
        bravoTrainerNew->contestResult = bravoTrainerSrc->contestResult;
        bravoTrainerNew->contestCategory = bravoTrainerSrc->contestCategory;
        StorePlayerIdInNormalShow((TVShow *)bravoTrainerNew);
        bravoTrainerNew->language = GAME_LANGUAGE;
        bravoTrainerNew->pokemonNameLanguage = GetStringLanguage(bravoTrainerNew->pokemonNickname);
        StripExtCtrlCodes(bravoTrainerNew->pokemonNickname);
    }
}

void BravoTrainerPokemonProfile_BeforeInterview1(u16 move)
{
    struct TVShowBravoTrainerPokemonProfiles *bravoTrainer = &gSaveBlock1.tvShows[24].bravoTrainer;

    InterviewBefore_BravoTrainerPkmnProfile();
    sCurTVShowSlot.slot = FindFirstEmptyNormalTVShowSlot(gSaveBlock1.tvShows);
    if (sCurTVShowSlot.slot != -1)
    {
        DeleteTVShowInArrayByIdx(gSaveBlock1.tvShows, 24);
        bravoTrainer->move = move;
        bravoTrainer->kind = TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE;
    }
}

void BravoTrainerPokemonProfile_BeforeInterview2(u8 contestStandingPlace)
{
    struct TVShowBravoTrainerPokemonProfiles *bravoTrainer = &gSaveBlock1.tvShows[24].bravoTrainer;

    sCurTVShowSlot.slot = FindFirstEmptyNormalTVShowSlot(gSaveBlock1.tvShows);
    if (sCurTVShowSlot.slot != -1)
    {
        bravoTrainer->contestResult = contestStandingPlace;
        bravoTrainer->contestCategory = gSpecialVar_ContestCategory;
        bravoTrainer->contestRank = (u8)gSpecialVar_ContestRank;
        bravoTrainer->species = GetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_SPECIES, NULL);
        GetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_NICKNAME, bravoTrainer->pokemonNickname);
    }
}

void InterviewAfter_BravoTrainerBattleTowerProfile(void)
{
    struct TVShowBravoTrainerBattleTowerSpotlight *bravoTrainerTower = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].bravoTrainerTower;

    bravoTrainerTower->kind = TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE;
    bravoTrainerTower->active = 1;
    StringCopy(bravoTrainerTower->playerName, gSaveBlock2.playerName);
    StringCopy(bravoTrainerTower->opponentName, gSaveBlock2.battleTower.defeatedByTrainerName);
    bravoTrainerTower->species = gSaveBlock2.battleTower.firstMonSpecies;
    bravoTrainerTower->defeatedSpecies = gSaveBlock2.battleTower.defeatedBySpecies;
    bravoTrainerTower->numFights = GetCurrentBattleTowerWinStreak(gSaveBlock2.battleTower.lastStreakLevelType);
    bravoTrainerTower->wonTheChallenge = gSaveBlock2.battleTower.battleOutcome;
    if (gSaveBlock2.battleTower.lastStreakLevelType == 0)
        bravoTrainerTower->btLevel = 50;
    else
        bravoTrainerTower->btLevel = 100;
    bravoTrainerTower->interviewResponse = gSpecialVar_0x8004;
    StorePlayerIdInNormalShow((TVShow *)bravoTrainerTower);
    bravoTrainerTower->language = GAME_LANGUAGE;
}

void TryPutSmartShopperOnAir(void)
{
    u8 isInactive;
    u8 i;

    isInactive = BernoulliTrial(0x5555);
    if (isInactive == FALSE)
    {
        sCurTVShowSlot.slot = FindFirstEmptyRecordMixTVShowSlot(gSaveBlock1.tvShows);
        if (sCurTVShowSlot.slot != -1 && IsRecordMixShowAlreadySpawned(TVSHOW_SMART_SHOPPER) != TRUE)
        {
            SortPurchasesByQuantity();
            if (gMartPurchaseHistory[0].quantity >= 20)
            {
                struct TVShowSmartShopper *smartShopper = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].smartshopperShow;

                smartShopper->kind = TVSHOW_SMART_SHOPPER;
                smartShopper->active = isInactive;
                smartShopper->shopLocation = gMapHeader.regionMapSectionId;
                for (i = 0; i < SMARTSHOPPER_NUM_ITEMS; i++)
                {
                    smartShopper->itemIds[i] = gMartPurchaseHistory[i].itemId;
                    smartShopper->itemAmounts[i] = gMartPurchaseHistory[i].quantity;
                }
                smartShopper->priceReduced = IsPokeNewsActive(POKENEWS_SLATEPORT);
                StringCopy(smartShopper->playerName, gSaveBlock2.playerName);
                StorePlayerIdInRecordMixShow((TVShow *)smartShopper);
                smartShopper->language = GAME_LANGUAGE;
            }
        }
    }
}

void PutNameRaterShowOnTheAir(void)
{
    InterviewBefore_NameRater();
    if (gSpecialVar_Result == TRUE)
        return;

    GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_NICKNAME, gStringVar1);
    if (StringLength(gSaveBlock2.playerName) > 1 && StringLength(gStringVar1) > 1)
    {
        struct TVShowNameRaterShow *nameRaterShow = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].nameRaterShow;

        nameRaterShow->kind = TVSHOW_NAME_RATER_SHOW;
        nameRaterShow->active = TRUE;
        nameRaterShow->species = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES, NULL);
        nameRaterShow->random = Random() % 3;
        nameRaterShow->random2 = Random() % 2;
        nameRaterShow->randomSpecies = GetRandomDifferentSpeciesSeenByPlayer(nameRaterShow->species);
        StringCopy(nameRaterShow->trainerName, gSaveBlock2.playerName);
        GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_NICKNAME, nameRaterShow->pokemonName);
        StorePlayerIdInNormalShow((TVShow *)nameRaterShow);
        nameRaterShow->language = GAME_LANGUAGE;
        nameRaterShow->pokemonNameLanguage = GetStringLanguage(nameRaterShow->pokemonName);
        StripExtCtrlCodes(nameRaterShow->pokemonName);
    }
}

void StartMassOutbreak(void)
{
    struct TVShowMassOutbreak *massOutbreak = &gSaveBlock1.tvShows[gSpecialVar_0x8004].massOutbreak;

    gSaveBlock1.outbreakPokemonSpecies = massOutbreak->species;
    gSaveBlock1.outbreakLocationMapNum = massOutbreak->locationMapNum;
    gSaveBlock1.outbreakLocationMapGroup = massOutbreak->locationMapGroup;
    gSaveBlock1.outbreakPokemonLevel = massOutbreak->level;
    gSaveBlock1.outbreakUnused1 = massOutbreak->unused1;
    gSaveBlock1.outbreakUnused2 = massOutbreak->unused2;
    gSaveBlock1.outbreakPokemonMoves[0] = massOutbreak->moves[0];
    gSaveBlock1.outbreakPokemonMoves[1] = massOutbreak->moves[1];
    gSaveBlock1.outbreakPokemonMoves[2] = massOutbreak->moves[2];
    gSaveBlock1.outbreakPokemonMoves[3] = massOutbreak->moves[3];
    gSaveBlock1.outbreakUnused3 = massOutbreak->unused3;
    gSaveBlock1.outbreakPokemonProbability = massOutbreak->probability;
    gSaveBlock1.outbreakDaysLeft = 2;
}

void InterviewAfter_FanClubLetter(void)
{
    struct TVShowFanClubLetter *fanclubLetter = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].fanclubLetter;

    fanclubLetter->kind = TVSHOW_FAN_CLUB_LETTER;
    fanclubLetter->active = 1;
    StringCopy(fanclubLetter->playerName, gSaveBlock2.playerName);
    fanclubLetter->species = GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_SPECIES, NULL);
    StorePlayerIdInNormalShow((TVShow *)fanclubLetter);
    fanclubLetter->language = GAME_LANGUAGE;
}

void InterviewAfter_RecentHappenings(void)
{
    struct TVShowRecentHappenings *recentHappenings = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].recentHappenings;

    recentHappenings->kind = TVSHOW_RECENT_HAPPENINGS;
    recentHappenings->active = 1;
    StringCopy(recentHappenings->playerName, gSaveBlock2.playerName);
    recentHappenings->species = 0;
    StorePlayerIdInNormalShow((TVShow *)recentHappenings);
    recentHappenings->language = GAME_LANGUAGE;
}

void InterviewAfter_PkmnFanClubOpinions(void)
{
    u8 monIndex;
    struct TVShowFanClubOpinions *fanclubOpinions = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].fanclubOpinions;

    fanclubOpinions->kind = TVSHOW_PKMN_FAN_CLUB_OPINIONS;
    fanclubOpinions->active = 1;
    monIndex = GetLeadMonIndex();
    fanclubOpinions->friendshipHighNybble = GetMonData(&gPlayerParty[monIndex], MON_DATA_FRIENDSHIP, NULL) / 16;
    fanclubOpinions->questionAsked = gSpecialVar_0x8007;
    StringCopy(fanclubOpinions->playerName, gSaveBlock2.playerName);
    GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_NICKNAME, fanclubOpinions->nickname);
    fanclubOpinions->species = GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_SPECIES, NULL);
    StorePlayerIdInNormalShow((TVShow *)fanclubOpinions);
    fanclubOpinions->language = GAME_LANGUAGE;
    fanclubOpinions->pokemonNameLanguage = GetStringLanguage(fanclubOpinions->nickname);
    StripExtCtrlCodes(fanclubOpinions->nickname);
}

void InterviewAfter_Dummy(void)
{
}

void TryStartRandomMassOutbreak(void)
{
    if (FlagGet(FLAG_SYS_GAME_CLEAR))
    {
        u8 i;

        for (i = 0; i < LAST_TVSHOW_IDX; i++)
        {
            if (gSaveBlock1.tvShows[i].massOutbreak.kind == TVSHOW_MASS_OUTBREAK)
                return;
        }
        if (BernoulliTrial(0x147))
            return;

        sCurTVShowSlot.slot = FindFirstEmptyNormalTVShowSlot(gSaveBlock1.tvShows);
        if (sCurTVShowSlot.slot != -1)
        {
            u16 rand = Random();
            u16 val = rand % 5;
            s32 val2 = sCurTVShowSlot.slot;
            struct TVShowMassOutbreak *massOutbreak = &gSaveBlock1.tvShows[val2].massOutbreak;

            massOutbreak->kind = TVSHOW_MASS_OUTBREAK;
            massOutbreak->active = 1;
            massOutbreak->level = sPokeOutbreakSpeciesList[val].level;
            massOutbreak->unused1 = 0;
            massOutbreak->unused3 = 0;
            massOutbreak->species = sPokeOutbreakSpeciesList[val].species;
            massOutbreak->unused2 = 0;
            massOutbreak->moves[0] = sPokeOutbreakSpeciesList[val].moves[0];
            massOutbreak->moves[1] = sPokeOutbreakSpeciesList[val].moves[1];
            massOutbreak->moves[2] = sPokeOutbreakSpeciesList[val].moves[2];
            massOutbreak->moves[3] = sPokeOutbreakSpeciesList[val].moves[3];
            massOutbreak->locationMapNum = sPokeOutbreakSpeciesList[val].location;
            massOutbreak->locationMapGroup = 0;
            massOutbreak->unused4 = 0;
            massOutbreak->probability = 0x32;
            massOutbreak->unused5 = 0;
            massOutbreak->daysBeforeOutbreak = 0x01;
            StorePlayerIdInNormalShow((TVShow *)massOutbreak);
            massOutbreak->language = GAME_LANGUAGE;
        }
    }
}

void EndMassOutbreak(void)
{
    gSaveBlock1.outbreakPokemonSpecies = 0;
    gSaveBlock1.outbreakLocationMapNum = 0;
    gSaveBlock1.outbreakLocationMapGroup = 0;
    gSaveBlock1.outbreakPokemonLevel = 0;
    gSaveBlock1.outbreakUnused1 = 0;
    gSaveBlock1.outbreakUnused2 = 0;
    gSaveBlock1.outbreakPokemonMoves[0] = 0;
    gSaveBlock1.outbreakPokemonMoves[1] = 0;
    gSaveBlock1.outbreakPokemonMoves[2] = 0;
    gSaveBlock1.outbreakPokemonMoves[3] = 0;
    gSaveBlock1.outbreakUnused3 = 0;
    gSaveBlock1.outbreakPokemonProbability = 0;
    gSaveBlock1.outbreakDaysLeft = 0;
}

void UpdateTVShowsPerDay(u16 days)
{
    UpdateTimeBeforeMassOutbreak(days);
    TryEndMassOutbreak(days);
    UpdatePokeNewsCountdown(days);
    ResolveWorldOfMastersShow(days);
}

void UpdateTimeBeforeMassOutbreak(u16 days)
{
    u8 showIdx;

    if (gSaveBlock1.outbreakPokemonSpecies == 0)
    {
        for (showIdx = 0; showIdx < LAST_TVSHOW_IDX; showIdx++)
        {
            if (gSaveBlock1.tvShows[showIdx].massOutbreak.kind == TVSHOW_MASS_OUTBREAK
             && gSaveBlock1.tvShows[showIdx].massOutbreak.active == TRUE)
            {
                struct TVShowMassOutbreak *massOutbreak = &gSaveBlock1.tvShows[showIdx].massOutbreak;

                if (massOutbreak->daysBeforeOutbreak < days)
                    massOutbreak->daysBeforeOutbreak = 0;
                else
                    massOutbreak->daysBeforeOutbreak -= days;
                break;
            }
        }
    }
}

void TryEndMassOutbreak(u16 days)
{
    if (gSaveBlock1.outbreakDaysLeft <= days)
        EndMassOutbreak();
    else
        gSaveBlock1.outbreakDaysLeft -= days;
}

void RecordFishingAttemptForTV(bool8 caughtFish)
{
    u8 attemptCount;
    u8 otherAttemptCount;

    if (caughtFish)
    {
        attemptCount = sPokemonAnglerAttemptCounters >> 8;
        if (attemptCount > 4)
            TryPutFishingAdviceOnAir();
        sPokemonAnglerAttemptCounters &= 0xFF;
        otherAttemptCount = sPokemonAnglerAttemptCounters & 0xFF;
        if (otherAttemptCount != 0xFF)
            sPokemonAnglerAttemptCounters++;
    }
    else
    {
        attemptCount = sPokemonAnglerAttemptCounters & 0xFF;
        if (attemptCount > 4)
            TryPutFishingAdviceOnAir();
        sPokemonAnglerAttemptCounters &= 0xFF00;
        otherAttemptCount = sPokemonAnglerAttemptCounters >> 8;
        if (otherAttemptCount != 0xFF)
            sPokemonAnglerAttemptCounters += 0x100;
    }
}

void TryPutFishingAdviceOnAir(void)
{
    sCurTVShowSlot.slot = FindFirstEmptyRecordMixTVShowSlot(gSaveBlock1.tvShows);
    if (sCurTVShowSlot.slot != -1 && IsRecordMixShowAlreadySpawned(TVSHOW_FISHING_ADVICE) != 1)
    {
        struct TVShowPokemonAngler *pokemonAngler = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].pokemonAngler;
        register u8 zero asm("r1") = 0;

        asm(""::"r"(zero));
        pokemonAngler->kind = TVSHOW_FISHING_ADVICE;
        pokemonAngler->active = zero;
        pokemonAngler->nBites = sPokemonAnglerAttemptCounters & 0xFF;
        pokemonAngler->nFails = sPokemonAnglerAttemptCounters >> 8;
        pokemonAngler->species = sPokemonAnglerSpecies;
        StringCopy(pokemonAngler->playerName, gSaveBlock2.playerName);
        StorePlayerIdInRecordMixShow((TVShow *)pokemonAngler);
        pokemonAngler->language = GAME_LANGUAGE;
    }
}

void SetPokemonAnglerSpecies(u16 species)
{
    sPokemonAnglerSpecies = species;
}

void TryPutWorldOfMastersOnAir(void);

void ResolveWorldOfMastersShow(u16 days)
{
    struct TVShowWorldOfMasters *worldOfMasters = &gSaveBlock1.tvShows[LAST_TVSHOW_IDX].worldOfMasters;

    if (worldOfMasters->kind == TVSHOW_WORLD_OF_MASTERS)
    {
        if (worldOfMasters->numPokeCaught < 20)
            DeleteTVShowInArrayByIdx(gSaveBlock1.tvShows, LAST_TVSHOW_IDX);
        else
            TryPutWorldOfMastersOnAir();
    }
}
void TryPutWorldOfMastersOnAir(void)
{
    struct TVShowWorldOfMasters *worldOfMastersSrc = &gSaveBlock1.tvShows[LAST_TVSHOW_IDX].worldOfMasters;

    if (BernoulliTrial(0xFFFF) == 0)
    {
        sCurTVShowSlot.slot = FindFirstEmptyRecordMixTVShowSlot(gSaveBlock1.tvShows);
        if (sCurTVShowSlot.slot != -1 && IsRecordMixShowAlreadySpawned(TVSHOW_WORLD_OF_MASTERS) != 1)
        {
            struct TVShowWorldOfMasters *worldOfMastersDst = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].worldOfMasters;

            worldOfMastersDst->kind = TVSHOW_WORLD_OF_MASTERS;
            worldOfMastersDst->active = 0;
            worldOfMastersDst->numPokeCaught = worldOfMastersSrc->numPokeCaught;
            worldOfMastersDst->steps = GetGameStat(GAME_STAT_STEPS) - worldOfMastersSrc->steps;
            worldOfMastersDst->caughtPoke = worldOfMastersSrc->caughtPoke;
            worldOfMastersDst->species = worldOfMastersSrc->species;
            worldOfMastersDst->location = worldOfMastersSrc->location;
            StringCopy(worldOfMastersDst->playerName, gSaveBlock2.playerName);
            StorePlayerIdInRecordMixShow((TVShow *)worldOfMastersDst);
            worldOfMastersDst->language = GAME_LANGUAGE;
        }
    }
}

void TryPutRandomPokeNewsOnAir(void)
{
    u16 newsKind;

    if (FlagGet(FLAG_SYS_GAME_CLEAR) != 0)
    {
        sCurTVShowSlot.slot = GetFirstEmptyPokeNewsSlot(gSaveBlock1.pokeNews);
        if (sCurTVShowSlot.slot != -1 && BernoulliTrial(0x28f) != TRUE)
        {
            newsKind = (Random() % NUM_POKENEWS_TYPES) + POKENEWS_SLATEPORT;
            if (IsAddingPokeNewsDisallowed(newsKind) != TRUE)
            {
                gSaveBlock1.pokeNews[sCurTVShowSlot.slot].kind = newsKind;
                gSaveBlock1.pokeNews[sCurTVShowSlot.slot].dayCountdown = POKENEWS_COUNTDOWN;
                gSaveBlock1.pokeNews[sCurTVShowSlot.slot].state = POKENEWS_STATE_UPCOMING;
            }
        }
    }
}

int GetFirstEmptyPokeNewsSlot(struct PokeNews *pokeNews)
{
    s8 i;

    for (i = 0; i < POKE_NEWS_COUNT; i++)
    {
        if (pokeNews[i].kind == POKENEWS_NONE)
            return i;
    }
    return -1;
}

void ClearPokeNews(void)
{
    u8 i;

    for (i = 0; i < POKE_NEWS_COUNT; i++)
        ClearPokeNewsBySlot(i);
}

void ClearPokeNewsBySlot(u8 slot)
{
    gSaveBlock1.pokeNews[slot].kind = POKENEWS_NONE;
    gSaveBlock1.pokeNews[slot].state = POKENEWS_STATE_INACTIVE;
    gSaveBlock1.pokeNews[slot].dayCountdown = 0;
}

void CompactPokeNews(void)
{
    u8 i, j;

    for (i = 0; i < POKE_NEWS_COUNT - 1; i++)
    {
        if (gSaveBlock1.pokeNews[i].kind == POKENEWS_NONE)
        {
            for (j = i + 1; j < POKE_NEWS_COUNT; j++)
            {
                if (gSaveBlock1.pokeNews[j].kind != POKENEWS_NONE)
                {
                    gSaveBlock1.pokeNews[i] = gSaveBlock1.pokeNews[j];
                    ClearPokeNewsBySlot(j);
                    break;
                }
            }
        }
    }
}

u8 FindAnyPokeNewsOnTheAir(void)
{
    u8 i;
    for (i = 0; i < POKE_NEWS_COUNT; i++)
    {
        if (gSaveBlock1.pokeNews[i].kind != POKENEWS_NONE
         && gSaveBlock1.pokeNews[i].state == POKENEWS_STATE_UPCOMING
         && gSaveBlock1.pokeNews[i].dayCountdown < POKENEWS_COUNTDOWN - 1)
            return i;
    }
    return 0xFF;
}

void DoPokeNews(void)
{
    u8 i;
    i = FindAnyPokeNewsOnTheAir();
    if (i == 0xff)
    {
        gSpecialVar_Result = FALSE;
        return;
    }
    if (gSaveBlock1.pokeNews[i].dayCountdown == 0)
    {
        gSaveBlock1.pokeNews[i].state = POKENEWS_STATE_ACTIVE;
        if (gLocalTime.hours < 20)
            ShowFieldMessage(sPokeNewsTextGroup_Ongoing[gSaveBlock1.pokeNews[i].kind]);
        else
            ShowFieldMessage(sPokeNewsTextGroup_Ending[gSaveBlock1.pokeNews[i].kind]);
    }
    else
    {
        u16 dayCountdown = gSaveBlock1.pokeNews[i].dayCountdown;

        ConvertIntToDecimalStringN(gStringVar1, dayCountdown, 0, 1);
        gSaveBlock1.pokeNews[i].state = POKENEWS_STATE_INACTIVE;
        ShowFieldMessage(sPokeNewsTextGroup_Upcoming[gSaveBlock1.pokeNews[i].kind]);
    }
    gSpecialVar_Result = 1;
}

bool8 IsPokeNewsActive(u8 newsKind)
{
    u8 i;

    if (newsKind == POKENEWS_NONE)
        return FALSE;
    for (i=0; i<POKE_NEWS_COUNT; i++)
    {
        if (gSaveBlock1.pokeNews[i].kind == newsKind)
        {
            if (gSaveBlock1.pokeNews[i].state == POKENEWS_STATE_ACTIVE && ShouldApplyPokeNewsEffect(newsKind) != FALSE)
                return TRUE;
            else
                return FALSE;
        }
    }
    return FALSE;
}

bool8 ShouldApplyPokeNewsEffect(u8 newsKind)
{
    switch (newsKind)
    {
    case POKENEWS_SLATEPORT:
        if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_SLATEPORT_CITY)
         && gSaveBlock1.location.mapNum == MAP_NUM(MAP_SLATEPORT_CITY)
         && gSpecialVar_LastTalked == 0x1a)
            return TRUE;
        else
            return FALSE;
        break;
    case POKENEWS_LILYCOVE:
        if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ROOFTOP)
         && gSaveBlock1.location.mapNum == MAP_NUM(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ROOFTOP))
            return TRUE;
        else
            return FALSE;
        break;
    default:
        return TRUE;
    }
}

bool8 IsAddingPokeNewsDisallowed(u8 newsKind)
{
    u8 i;

    if (newsKind == POKENEWS_NONE)
        return TRUE;
    for (i=0; i<POKE_NEWS_COUNT; i++)
    {
        if (gSaveBlock1.pokeNews[i].kind == newsKind)
            return TRUE;
    }
    return FALSE;
}

void UpdatePokeNewsCountdown(u16 days)
{
    u8 i;

    for (i=0; i<POKE_NEWS_COUNT; i++)
    {
        if (gSaveBlock1.pokeNews[i].kind != POKENEWS_NONE)
        {
            if (gSaveBlock1.pokeNews[i].dayCountdown < days)
            {
                ClearPokeNewsBySlot(i);
            }
            else
            {
                if (gSaveBlock1.pokeNews[i].state == POKENEWS_STATE_INACTIVE && FlagGet(FLAG_SYS_GAME_CLEAR) == TRUE)
                    gSaveBlock1.pokeNews[i].state = POKENEWS_STATE_UPCOMING;
                gSaveBlock1.pokeNews[i].dayCountdown -= days;
            }
        }
    }
    CompactPokeNews();
}

void CopyContestRankToStringVar(u8 varIdx, u8 rank)
{
    switch (rank)
    {
    case CONTEST_RANK_NORMAL:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_NORMAL]);
        break;
    case CONTEST_RANK_SUPER:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_SUPER]);
        break;
    case CONTEST_RANK_HYPER:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_HYPER]);
        break;
    case CONTEST_RANK_MASTER:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_MASTER]);
        break;
    }
}

void CopyContestCategoryToStringVar(u8 varIdx, u8 category)
{
    switch (category)
    {
    case CONTEST_CATEGORY_COOL:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_COOL]);
        break;
    case CONTEST_CATEGORY_BEAUTY:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_BEAUTY]);
        break;
    case CONTEST_CATEGORY_CUTE:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_CUTE]);
        break;
    case CONTEST_CATEGORY_SMART:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_SMART]);
        break;
    case CONTEST_CATEGORY_TOUGH:
        StringCopy(gTVStringVarPtrs[varIdx], gStdStrings[STDSTRING_TOUGH]);
        break;
    }
}

void SetContestCategoryStringVarForInterview(void)
{
    struct TVShowBravoTrainerPokemonProfiles *bravoTrainer = &gSaveBlock1.tvShows[gSpecialVar_0x8004].bravoTrainer;

    CopyContestCategoryToStringVar(1, bravoTrainer->contestCategory);
}

void ConvertIntToDecimalString(u8 varIdx, s32 value)
{
    size_t nDigits = CountDigits(value);

    ConvertIntToDecimalStringN(gTVStringVarPtrs[varIdx], value, 0, nDigits);
}

size_t CountDigits(int value)
{
    if (value / 10 == 0)
        return 1;
    else if (value / 100 == 0)
        return 2;
    else if (value / 1000 == 0)
        return 3;
    else if (value / 10000 == 0)
        return 4;
    else if (value / 100000 == 0)
        return 5;
    else if (value / 1000000 == 0)
        return 6;
    else if (value / 10000000 == 0)
        return 7;
    else if (value / 100000000 == 0)
        return 8;
    else
        return 1;
}

void SmartShopper_BufferPurchaseTotal(u8 varIdx, struct TVShowSmartShopper *smartShopper)
{
    u8 i;
    s32 price;
    price = 0;

    for (i = 0; i < SMARTSHOPPER_NUM_ITEMS; i++)
    {
        if (smartShopper->itemIds[i])
            price += GetItemPrice(smartShopper->itemIds[i]) * smartShopper->itemAmounts[i];
    }
    if (smartShopper->priceReduced == TRUE)
        ConvertIntToDecimalString(varIdx, price >> 1);
    else
        ConvertIntToDecimalString(varIdx, price);
}

bool8 IsRecordMixShowAlreadySpawned(u8 showIdx)
{
    u8 i;
    //TVShow *tvShows;
    TVShow *tvShows = gSaveBlock1.tvShows;
    u32 trainerId = GetPlayerIDAsU32();

    for (i = NUM_NORMAL_TVSHOW_SLOTS; i < LAST_TVSHOW_IDX; i++)
    {
        if (tvShows[i].common.kind == showIdx)
        {
            if ((trainerId & 0xFF) == tvShows[i].common.trainerIdLo && ((trainerId >> 8) & 0xFF) == tvShows[i].common.trainerIdHi)
                return TRUE;
        }
    }
    return FALSE;
}

void SortPurchasesByQuantity(void)
{
    u8 i, j;
    u16 tempItemId, tempQuantity;

    for (i = 0; i < SMARTSHOPPER_NUM_ITEMS - 1; i++)
    {
        for (j = i + 1; j < SMARTSHOPPER_NUM_ITEMS; j++)
        {
            if (gMartPurchaseHistory[i].quantity < gMartPurchaseHistory[j].quantity)
            {
                tempItemId = gMartPurchaseHistory[i].itemId;
                tempQuantity = gMartPurchaseHistory[i].quantity;
                gMartPurchaseHistory[i].itemId = gMartPurchaseHistory[j].itemId;
                gMartPurchaseHistory[i].quantity = gMartPurchaseHistory[j].quantity;
                gMartPurchaseHistory[j].itemId = tempItemId;
                gMartPurchaseHistory[j].quantity = tempQuantity;
            }
        }
    }
}

void TryReplaceOldTVShowOfKind(u8 showType)
{
    u8 i;

    for (i = 0; i < NUM_NORMAL_TVSHOW_SLOTS; i++)
    {
        if (gSaveBlock1.tvShows[i].common.kind == showType)
        {
            if(gSaveBlock1.tvShows[i].common.active == 1)
            {
                gSpecialVar_Result = 1;
            }
            else
            {
                DeleteTVShowInArrayByIdx(gSaveBlock1.tvShows, i);
                CompactTVShowArray(gSaveBlock1.tvShows);
                Script_FindFirstEmptyNormalTVShowSlot();
            }
            return;
        }
    }
    Script_FindFirstEmptyNormalTVShowSlot();
}

void InterviewBefore(void)
{
    gSpecialVar_Result = FALSE;
    switch (gSpecialVar_0x8005)
    {
    case TVSHOW_FAN_CLUB_LETTER:
        InterviewBefore_FanClubLetter();
        break;
    case TVSHOW_RECENT_HAPPENINGS:
        InterviewBefore_RecentHappenings();
        break;
    case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
        InterviewBefore_PkmnFanClubOpinions();
        break;
    case TVSHOW_DUMMY:
        InterviewBefore_Dummy();
        break;
    case TVSHOW_NAME_RATER_SHOW:
        InterviewBefore_NameRater();
        break;
    case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
        InterviewBefore_BravoTrainerPkmnProfile();
        break;
    case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
        InterviewBefore_BravoTrainerBTProfile();
        break;
    }
}

void InterviewBefore_FanClubLetter(void)
{
    struct TVShowFanClubLetter *fanclubLetter;

    TryReplaceOldTVShowOfKind(TVSHOW_FAN_CLUB_LETTER);
    if (gSpecialVar_Result == 0)
    {
        StringCopy(gStringVar1, gSpeciesNames[GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_SPECIES, 0)]);
        fanclubLetter = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].fanclubLetter;
        InitializeEasyChatWordArray(fanclubLetter->words, 6);
    }
}

void InterviewBefore_RecentHappenings(void)
{
    struct TVShowRecentHappenings *recentHappenings;

    TryReplaceOldTVShowOfKind(TVSHOW_RECENT_HAPPENINGS);
    if (gSpecialVar_Result == 0)
    {
        recentHappenings = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].recentHappenings;
        InitializeEasyChatWordArray(recentHappenings->words, 6);
    }
}

void InterviewBefore_PkmnFanClubOpinions(void)
{
    struct TVShowFanClubOpinions *fanclubOpinions;

    TryReplaceOldTVShowOfKind(TVSHOW_PKMN_FAN_CLUB_OPINIONS);
    if (gSpecialVar_Result == 0)
    {
        StringCopy(gStringVar1, gSpeciesNames[GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_SPECIES, 0)]);
        GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_NICKNAME, gStringVar2);
        StringGet_Nickname(gStringVar2);
        fanclubOpinions = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].fanclubOpinions;
        InitializeEasyChatWordArray(fanclubOpinions->words, 2);
    }
}

void InterviewBefore_Dummy(void)
{
    gSpecialVar_Result = 1;
}

void InterviewBefore_NameRater(void)
{
    TryReplaceOldTVShowOfKind(TVSHOW_NAME_RATER_SHOW);
}

void InterviewBefore_BravoTrainerPkmnProfile(void)
{
    struct TVShowBravoTrainerPokemonProfiles *bravoTrainer;

    TryReplaceOldTVShowOfKind(TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE);
    if (gSpecialVar_Result == 0)
    {
        bravoTrainer = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].bravoTrainer;
        InitializeEasyChatWordArray(bravoTrainer->words, 2);
    }
}

void InterviewBefore_BravoTrainerBTProfile(void)
{
    struct TVShowBravoTrainerBattleTowerSpotlight *bravoTrainerTower;

    TryReplaceOldTVShowOfKind(TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE);
    if (gSpecialVar_Result == 0)
    {
        bravoTrainerTower = &gSaveBlock1.tvShows[sCurTVShowSlot.slot].bravoTrainerTower;
        InitializeEasyChatWordArray(bravoTrainerTower->words, 1); // wrong struct ident, fix later
    }
}

#if ENGLISH
u8 IsPartyMonNicknamedOrNotEnglish(u8 monIndex)
{
    u32 species;

    GetMonData(&gPlayerParty[monIndex], MON_DATA_NICKNAME, &gStringVar1);
    species = GetMonData(&gPlayerParty[monIndex], MON_DATA_SPECIES, NULL);
    if (StringCompareWithoutExtCtrlCodes(gSpeciesNames[species], gStringVar1) == 0)
        return FALSE;

    return TRUE;
}
#elif GERMAN
u8 IsPartyMonNicknamedOrNotEnglish(u8 pmMonIndex)
{
    u8 langData[4];
    u32 species;
    u8 *tmp;

    GetMonData(&gPlayerParty[pmMonIndex], MON_DATA_NICKNAME, &gStringVar1);

    tmp = langData;
    tmp[0] = GetMonData(&gPlayerParty[pmMonIndex], MON_DATA_LANGUAGE, &langData);
    if (tmp[0] != GAME_LANGUAGE)
        return TRUE;

    species = GetMonData(&gPlayerParty[pmMonIndex], MON_DATA_SPECIES, NULL);
    if (StringCompareWithoutExtCtrlCodes(gSpeciesNames[species], gStringVar1))
        return TRUE;

    return FALSE;
}
#endif

u8 IsLeadMonNicknamedOrNotEnglish(void)
{
    return IsPartyMonNicknamedOrNotEnglish(GetLeadMonIndex());
}

void DeleteTVShowInArrayByIdx(TVShow tvShows[], u8 showIdx)
{
    u8 idx;

    tvShows[showIdx].common.kind = TVSHOW_OFF_AIR;
    tvShows[showIdx].common.active = FALSE;
    for (idx = 0; idx < 34; idx++)
        tvShows[showIdx].commonInit.data[idx] = 0;
}

void CompactTVShowArray(TVShow tvShows[])
{
    u8 showIdx;
    u8 nextShowIdx;

    for (showIdx = 0; showIdx < NUM_NORMAL_TVSHOW_SLOTS - 1; showIdx++)
    {
        if (tvShows[showIdx].common.kind == TVSHOW_OFF_AIR)
        {
            for (nextShowIdx = showIdx + 1; nextShowIdx < NUM_NORMAL_TVSHOW_SLOTS; nextShowIdx++)
            {
                if (tvShows[nextShowIdx].common.kind != TVSHOW_OFF_AIR)
                {
                    tvShows[showIdx] = tvShows[nextShowIdx];
                    DeleteTVShowInArrayByIdx(tvShows, nextShowIdx);
                    break;
                }
            }
        }
    }
    for (showIdx = NUM_NORMAL_TVSHOW_SLOTS; showIdx < LAST_TVSHOW_IDX; showIdx++)
    {
        if (tvShows[showIdx].common.kind == TVSHOW_OFF_AIR)
        {
            for (nextShowIdx = showIdx + 1; nextShowIdx < LAST_TVSHOW_IDX; nextShowIdx++)
            {
                if (tvShows[nextShowIdx].common.kind != TVSHOW_OFF_AIR)
                {
                    tvShows[showIdx] = tvShows[nextShowIdx];
                    DeleteTVShowInArrayByIdx(gSaveBlock1.tvShows, nextShowIdx);
                    break;
                }
            }
        }
    }
}

u16 GetRandomDifferentSpeciesAndNameSeenByPlayer(u8 varIdx, u16 excludedSpecies)
{
    u16 species = GetRandomDifferentSpeciesSeenByPlayer(excludedSpecies);

    StringCopy(gTVStringVarPtrs[varIdx], gSpeciesNames[species]);
    return species;
}

u16 GetRandomDifferentSpeciesSeenByPlayer(u16 species)
{
    u16 rspecies;
    u16 cspecies;

    rspecies = (Random() % (NUM_SPECIES - 1)) + 1;
    cspecies = rspecies;
    while (GetSetPokedexFlag(SpeciesToNationalPokedexNum(cspecies), 0) != 1 || cspecies == species)
    {
        if (cspecies == SPECIES_BULBASAUR)
            cspecies = NUM_SPECIES - 1;
        else
            cspecies --;
        if (cspecies == rspecies)
        {
            cspecies = species;
            return cspecies;
        }
    }
    return cspecies;
}

void Script_FindFirstEmptyNormalTVShowSlot(void)
{
    sCurTVShowSlot.slot = FindFirstEmptyNormalTVShowSlot(gSaveBlock1.tvShows);
    gSpecialVar_0x8006 = sCurTVShowSlot.slot;
    if (sCurTVShowSlot.slot == -1)
        gSpecialVar_Result = TRUE;
    else
        gSpecialVar_Result = 0;
}

s8 FindFirstEmptyNormalTVShowSlot(TVShow tvShows[])
{
    u8 idx;

    for (idx = 0; idx < NUM_NORMAL_TVSHOW_SLOTS; idx++)
    {
        if (tvShows[idx].common.kind == TVSHOW_OFF_AIR)
            return idx;
    }
    return -1;
}

s8 FindFirstEmptyRecordMixTVShowSlot(TVShow tvShows[])
{
    s8 idx;

    for (idx = NUM_NORMAL_TVSHOW_SLOTS; idx < LAST_TVSHOW_IDX; idx++)
    {
        if (tvShows[idx].common.kind == TVSHOW_OFF_AIR)
            return idx;
    }
    return -1;
}

bool8 BernoulliTrial(u16 ratio)
{
    if (Random() <= ratio)
        return FALSE;
    return TRUE;
}

void GetRandomWordFromShow(TVShow *show)
{
    u8 i = Random() % 6;

    while (1)
    {
        if (i == 6)
            i = 0;
        if (show->recentHappenings.words[i] != 0xFFFF)
            break;
        i++;
    }
    CopyEasyChatWord(gStringVar3, show->recentHappenings.words[i]);
}

u8 GetRandomNameRaterStateFromName(struct TVShowNameRaterShow *show)
{
    u16 nameSum = 0;
    u8 i = 0;

    if (show->pokemonName[0] != 0xFF)
    {
        while (i < 11 && show->pokemonName[i] != 0xFF)
        {
            nameSum += show->pokemonName[i];
            i++;
        }
    }
    return nameSum & 0x7;
}


void GetNicknameSubstring(u8 varIdx, u8 whichPosition, u8 charParam, u16 whichString, u16 species, struct TVShowNameRaterShow *show)
{
    u8 chars[3];
    u16 nameLength;
    u8 i;

    for (i = 0; i < ARRAY_COUNT(chars); i++)
        chars[i] = EOS;

    if (whichString == 0)
    {
        nameLength = StringLength(show->trainerName);
        if (charParam == 0)
        {
            chars[0] = show->trainerName[whichPosition];
        }
        else if (charParam == 1)
        {
            chars[0] = show->trainerName[nameLength - whichPosition];
        }
        else if (charParam == 2) {
            chars[0] = show->trainerName[whichPosition];
            chars[1] = show->trainerName[whichPosition + 1];
        }
        else
        {
            chars[0] = show->trainerName[nameLength - (whichPosition + 2)];
            chars[1] = show->trainerName[nameLength - (whichPosition + 1)];
        }
    }
    else if (whichString == 1)
    {
        nameLength = StringLength(show->pokemonName);
        if (charParam == 0)
        {
            chars[0] = show->pokemonName[whichPosition];
        }
        else if (charParam == 1)
        {
            chars[0] = show->pokemonName[nameLength - whichPosition];
        }
        else if (charParam == 2)
        {
            chars[0] = show->pokemonName[whichPosition];
            chars[1] = show->pokemonName[whichPosition + 1];
        }
        else
        {
            chars[0] = show->pokemonName[nameLength - (whichPosition + 2)];
            chars[1] = show->pokemonName[nameLength - (whichPosition + 1)];
        }
    }
    else
    {
        nameLength = StringLength(gSpeciesNames[species]);
        if (charParam == 0)
        {
            chars[0] = gSpeciesNames[species][whichPosition];
        }
        else if (charParam == 1)
        {
            chars[0] = gSpeciesNames[species][nameLength - whichPosition];
        }
        else if (charParam == 2)
        {
            chars[0] = gSpeciesNames[species][whichPosition];
            chars[1] = gSpeciesNames[species][whichPosition + 1];
        }
        else
        {
            chars[0] = gSpeciesNames[species][nameLength - (whichPosition + 2)];
            chars[1] = gSpeciesNames[species][nameLength - (whichPosition + 1)];
        }
    }
    StringCopy(gTVStringVarPtrs[varIdx], chars);
}

bool8 IsTVShowAlreadyInQueue(void)
{
    u8 i;

    for (i = 0; i < NUM_NORMAL_TVSHOW_SLOTS; i++)
    {
        if (gSaveBlock1.tvShows[i].common.kind == gSpecialVar_0x8004)
            return TRUE;
    }
    return FALSE;
}

bool8 TryPutNameRaterShowOnTheAir(void)
{
    GetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_NICKNAME, &gStringVar1);
    if (!StringCompareWithoutExtCtrlCodes(gStringVar3, gStringVar1))
        return FALSE;
    PutNameRaterShowOnTheAir();
    return TRUE;
}

void ChangePokemonNickname_CB(void);

void ChangePokemonNickname(void)
{
    u16 spec;
    u16 gender;
    u32 pval;

    GetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_NICKNAME, &gStringVar3);
    GetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_NICKNAME, &gStringVar2);
    spec = GetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_SPECIES, 0);
    gender = GetMonGender(&(gPlayerParty[gSpecialVar_0x8004]));
    pval = GetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_PERSONALITY, 0);
    DoNamingScreen(NAMING_SCREEN_NICKNAME, gStringVar2, spec, gender, pval, ChangePokemonNickname_CB);
}

void ChangePokemonNickname_CB(void)
{
    SetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_NICKNAME, gStringVar2);
    CB2_ReturnToFieldContinueScriptPlayMapMusic();
}

void BufferMonNickname(void)
{
    GetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_NICKNAME, &gStringVar1);
    StringGet_Nickname(gStringVar1);
}

void IsMonOTIDNotPlayers(void)
{
    if (GetPlayerIDAsU32() == GetMonData(&(gPlayerParty[gSpecialVar_0x8004]), MON_DATA_OT_ID, 0))
        gSpecialVar_Result = 0;
    else
        gSpecialVar_Result = 1;
}

u8 GetTVGroupByShowId(u8 kind)
{
    if (kind == TVSHOW_OFF_AIR)
        return TVGROUP_NONE;

    if (kind >= TVGROUP_NORMAL_START && kind <= TVGROUP_NORMAL_END)
        return TVGROUP_NORMAL;

    if (kind >= TVGROUP_RECORD_MIX_START && kind <= TVGROUP_RECORD_MIX_END)
        return TVGROUP_RECORD_MIX;

    if (kind >= TVGROUP_OUTBREAK_START && kind <= TVGROUP_OUTBREAK_END)
        return TVGROUP_OUTBREAK;

    return TVGROUP_NONE;
}

u32 GetPlayerIDAsU32(void)
{
    return (gSaveBlock2.playerTrainerId[3] << 24) | (gSaveBlock2.playerTrainerId[2] << 16) | (gSaveBlock2.playerTrainerId[1] << 8) | (gSaveBlock2.playerTrainerId[0]);
}

u8 CheckForPlayersHouseNews(void)
{
    if (gSaveBlock1.location.mapGroup != MAP_GROUP(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F))
        return 0;
    if (gSaveBlock2.playerGender == MALE)
    {
        if (gSaveBlock1.location.mapNum != MAP_NUM(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F))
            return 0;
    }
    else
    {
        if (gSaveBlock1.location.mapNum != MAP_NUM(MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F))
            return 0;
    }
    if (FlagGet(FLAG_SYS_TV_LATI) == 1)
        return 1;
    if (FlagGet(FLAG_SYS_TV_HOME) == 1)
        return 2;
    return 1;
}

void GetMomOrDadStringForTVMessage(void)
{
    if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F))
    {
        if (gSaveBlock2.playerGender == MALE)
        {
            if (gSaveBlock1.location.mapNum == MAP_NUM(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F))
            {
                StringCopy(gStringVar1, gOtherText_Mom);
                VarSet(VAR_TEMP_3, 1);
            }
        }
        else
        {
            if (gSaveBlock1.location.mapNum == MAP_NUM(MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F))
            {
                StringCopy(gStringVar1, gOtherText_Mom);
                VarSet(VAR_TEMP_3, 1);
            }
        }
    }
    if (VarGet(VAR_TEMP_3) == 1)
    {
        StringCopy(gStringVar1, gOtherText_Mom);
    }
    else if (VarGet(VAR_TEMP_3) == 2)
    {
        StringCopy(gStringVar1, gOtherText_Dad);
    }
    else if (VarGet(VAR_TEMP_3) > 2)
    {
        if (VarGet(VAR_TEMP_3) % 2 == 0)
            StringCopy(gStringVar1, gOtherText_Mom);
        else
            StringCopy(gStringVar1, gOtherText_Dad);
    }
    else
    {
        if (Random() % 2 != 0)
        {
            StringCopy(gStringVar1, gOtherText_Mom);
            VarSet(VAR_TEMP_3, 1);
        }
        else
        {
            StringCopy(gStringVar1, gOtherText_Dad);
            VarSet(VAR_TEMP_3, 2);
        }
    }
}

void HideBattleTowerReporter(void)
{
    VarSet(VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 0);
    RemoveObjectEventByLocalIdAndMap(LOCALID_TOWER_LOBBY_REPORTER, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
}

void SetMixedTVShows(TVShow player1[TV_SHOWS_COUNT], TVShow player2[TV_SHOWS_COUNT], TVShow player3[TV_SHOWS_COUNT], TVShow player4[TV_SHOWS_COUNT]);

void DeleteExcessMixedShows(void);
void DeactivateShowsWithUnseenSpecies(void);
void DeactivateGameCompleteShowsIfNotUnlocked(void);

void ReceiveTvShowsData(u8 *src, u32 size, u8 playersLinkId)
{
    u8 i;
    union TVShow (*tvShows)[TV_SHOWS_COUNT];

    for (i = 0; i < 4; i++)
        memcpy(sRecordMixTvShows[i], &src[i * size], TV_SHOWS_COUNT * sizeof(TVShow));
    tvShows = sRecordMixTvShows;
    switch (playersLinkId)
    {
    case 0:
        SetMixedTVShows(gSaveBlock1.tvShows, tvShows[1], tvShows[2], tvShows[3]);
        break;
    case 1:
        SetMixedTVShows(tvShows[0], gSaveBlock1.tvShows, tvShows[2], tvShows[3]);
        break;
    case 2:
        SetMixedTVShows(tvShows[0], tvShows[1], gSaveBlock1.tvShows, tvShows[3]);
        break;
    case 3:
        SetMixedTVShows(tvShows[0], tvShows[1], tvShows[2], gSaveBlock1.tvShows);
        break;
    }
    CompactTVShowArray(gSaveBlock1.tvShows);
    DeleteExcessMixedShows();
    CompactTVShowArray(gSaveBlock1.tvShows);
    DeactivateShowsWithUnseenSpecies();
    DeactivateGameCompleteShowsIfNotUnlocked();
}

s8 FindInactiveShowInArray(TVShow tvShows[]);
bool8 TryMixTVShow(TVShow *destShows[TV_SHOWS_COUNT], TVShow *srcShows[TV_SHOWS_COUNT], u8 sourcePlayerIdx);
u8 TryMixNormalTVShow(TVShow *tv1, TVShow *tv2, u8 idx);
u8 TryMixRecordMixTVShow(TVShow *tv1, TVShow *tv2, u8 idx);
u8 TryMixOutbreakTVShow(TVShow *tv1, TVShow *tv2, u8 idx);

void SetMixedTVShows(TVShow player1[TV_SHOWS_COUNT], TVShow player2[TV_SHOWS_COUNT], TVShow player3[TV_SHOWS_COUNT], TVShow player4[TV_SHOWS_COUNT])
{
    u8 i, j;
    TVShow **showLists[4];

    showLists[0] = &player1;
    showLists[1] = &player2;
    showLists[2] = &player3;
    showLists[3] = &player4;
    sTVShowMixingNumPlayers = GetLinkPlayerCount();
    while (1)
    {
        for (i=0; i<sTVShowMixingNumPlayers; i++)
        {
            if (i == 0)
                sRecordMixingPartnersWithoutShowsToShare = i;
            sTVShowMixingCurSlot = FindInactiveShowInArray(showLists[i][0]);
            if (sTVShowMixingCurSlot == -1)
            {
                sRecordMixingPartnersWithoutShowsToShare++;
                if (sRecordMixingPartnersWithoutShowsToShare == sTVShowMixingNumPlayers)
                    return;
            }
            else
            {
                for (j=0; j<sTVShowMixingNumPlayers-1; j++)
                {
                    sCurTVShowSlot.slot = FindFirstEmptyRecordMixTVShowSlot(showLists[(i + j + 1) % sTVShowMixingNumPlayers][0]);
                    if (sCurTVShowSlot.slot != -1
                     && TryMixTVShow(&showLists[(i + j + 1) % sTVShowMixingNumPlayers][0], &showLists[i][0], (i + j + 1) % sTVShowMixingNumPlayers) == TRUE)
                        break;
                }
                if (j == sTVShowMixingNumPlayers - 1)
                    DeleteTVShowInArrayByIdx(showLists[i][0], sTVShowMixingCurSlot);
            }
        }
    }
}

bool8 TryMixTVShow(TVShow *destShows[TV_SHOWS_COUNT], TVShow *srcShows[TV_SHOWS_COUNT], u8 sourcePlayerIdx)
{
    u8 success;
    u8 group;
    TVShow *dest;
    TVShow *src;

    dest = *destShows;
    src = *srcShows;
    success = FALSE;
    group = GetTVGroupByShowId(src[sTVShowMixingCurSlot].common.kind);
    switch (group)
    {
    case TVGROUP_NORMAL:
        success = TryMixNormalTVShow(&dest[sCurTVShowSlot.slot], &src[sTVShowMixingCurSlot], sourcePlayerIdx);
        break;
    case TVGROUP_RECORD_MIX:
        success = TryMixRecordMixTVShow(&dest[sCurTVShowSlot.slot], &src[sTVShowMixingCurSlot], sourcePlayerIdx);
        break;
    case TVGROUP_OUTBREAK:
        success = TryMixOutbreakTVShow(&dest[sCurTVShowSlot.slot], &src[sTVShowMixingCurSlot], sourcePlayerIdx);
        break;
    }
    if (success == TRUE)
    {
        DeleteTVShowInArrayByIdx(src, sTVShowMixingCurSlot);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

u8 TryMixNormalTVShow(TVShow *dest, TVShow *src, u8 sourcePlayerIdx)
{
    u32 linkTrainerId = GetLinkPlayerTrainerId(sourcePlayerIdx);

    if ((linkTrainerId & 0xFF) == src->common.trainerIdLo && ((linkTrainerId >> 8) & 0xFF) == src->common.trainerIdHi)
        return FALSE;
    src->common.trainerIdLo = src->common.srcTrainerIdLo;
    src->common.trainerIdHi = src->common.srcTrainerIdHi;
    src->common.srcTrainerIdLo = linkTrainerId & 0xFF;
    src->common.srcTrainerIdHi = linkTrainerId >> 8;
    *dest = *src;
    dest->common.active = TRUE;
    return TRUE;
}

u8 TryMixRecordMixTVShow(TVShow *dest, TVShow *src, u8 sourcePlayerIdx)
{
    u32 linkTrainerId = GetLinkPlayerTrainerId(sourcePlayerIdx);
    if ((linkTrainerId & 0xFF) == src->common.srcTrainerIdLo && ((linkTrainerId >> 8) & 0xFF) == src->common.srcTrainerIdHi)
        return FALSE;
    if ((linkTrainerId & 0xFF) == src->common.trainerIdLo && ((linkTrainerId >> 8) & 0xFF) == src->common.trainerIdHi)
        return FALSE;
    src->common.srcTrainerIdLo = src->common.srcTrainerId2Lo;
    src->common.srcTrainerIdHi = src->common.srcTrainerId2Hi;
    src->common.srcTrainerId2Lo = linkTrainerId & 0xFF;
    src->common.srcTrainerId2Hi = linkTrainerId >> 8;
    *dest = *src;
    dest->common.active = TRUE;
    return TRUE;
}

u8 TryMixOutbreakTVShow(TVShow *dest, TVShow *src, u8 sourcePlayerIdx)
{
    u32 linkTrainerId = GetLinkPlayerTrainerId(sourcePlayerIdx);
    if ((linkTrainerId & 0xFF) == src->common.trainerIdLo && ((linkTrainerId >> 8) & 0xFF) == src->common.trainerIdHi)
        return FALSE;
    src->common.trainerIdLo = src->common.srcTrainerIdLo;
    src->common.trainerIdHi = src->common.srcTrainerIdHi;
    src->common.srcTrainerIdLo = linkTrainerId & 0xFF;
    src->common.srcTrainerIdHi = linkTrainerId >> 8;
    *dest = *src;
    dest->common.active = TRUE;
    dest->massOutbreak.daysBeforeOutbreak = 1;
    return TRUE;
}

s8 FindInactiveShowInArray(TVShow tvShows[])
{
    u8 i;

    for (i = 0; i < LAST_TVSHOW_IDX; i++)
    {
        if (tvShows[i].common.active == FALSE && (u8)(tvShows[i].common.kind - 1) < TVGROUP_OUTBREAK_END)
            return i;
    }
    return -1;
}

void DeactivateShowsWithUnseenSpecies(void)
{
    u16 i;
    u16 species;

    for (i = 0; i < LAST_TVSHOW_IDX; i++)
    {
        switch (gSaveBlock1.tvShows[i].common.kind)
        {
            case TVSHOW_OFF_AIR:
            case TVSHOW_RECENT_HAPPENINGS:
            case TVSHOW_SMART_SHOPPER:
            case TVSHOW_MASS_OUTBREAK:
                break;
            case TVSHOW_FAN_CLUB_LETTER:
                species = (&gSaveBlock1.tvShows[i])->fanclubLetter.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
                species = (&gSaveBlock1.tvShows[i])->fanclubOpinions.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_DUMMY:
                species = (&gSaveBlock1.tvShows[i])->dummy.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_NAME_RATER_SHOW:
                species = (&gSaveBlock1.tvShows[i])->nameRaterShow.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                species = (&gSaveBlock1.tvShows[i])->nameRaterShow.randomSpecies;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
                species = (&gSaveBlock1.tvShows[i])->bravoTrainer.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
                species = (&gSaveBlock1.tvShows[i])->bravoTrainerTower.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                species = (&gSaveBlock1.tvShows[i])->bravoTrainerTower.defeatedSpecies;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_POKEMON_TODAY_CAUGHT:
                species = (&gSaveBlock1.tvShows[i])->pokemonToday.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_POKEMON_TODAY_FAILED:
                species = (&gSaveBlock1.tvShows[i])->pokemonTodayFailed.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                species = (&gSaveBlock1.tvShows[i])->pokemonTodayFailed.species2;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_FISHING_ADVICE:
                species = (&gSaveBlock1.tvShows[i])->pokemonAngler.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            case TVSHOW_WORLD_OF_MASTERS:
                species = (&gSaveBlock1.tvShows[i])->worldOfMasters.species;
                DeactivateShowIfNotSeenSpecies(species, i);
                species = (&gSaveBlock1.tvShows[i])->worldOfMasters.caughtPoke;
                DeactivateShowIfNotSeenSpecies(species, i);
                break;
            default:
                DeactivateShow(i);
        }
    }
}

void DeactivateShow(u8 showIdx)
{
    gSaveBlock1.tvShows[showIdx].common.active = FALSE;
}

void DeactivateShowIfNotSeenSpecies(u16 species, u8 showIdx)
{
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), 0) == FALSE)
        gSaveBlock1.tvShows[showIdx].common.active = FALSE;
}

void DeactivateGameCompleteShowsIfNotUnlocked(void)
{
    u16 i;

    if (FlagGet(FLAG_SYS_GAME_CLEAR) == TRUE)
        return;
    for (i = 0; i < LAST_TVSHOW_IDX; i++)
    {
        if (gSaveBlock1.tvShows[i].common.kind == TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE)
            gSaveBlock1.tvShows[i].common.active = FALSE;
        else if (gSaveBlock1.tvShows[i].common.kind == TVSHOW_MASS_OUTBREAK)
            gSaveBlock1.tvShows[i].common.active = FALSE;
    }
}

void DeactivateAllNormalTVShows(void)
{
    u8 i;

    for (i = 0; i < NUM_NORMAL_TVSHOW_SLOTS; i++)
    {
        if (GetTVGroupByShowId(gSaveBlock1.tvShows[i].common.kind) == TVGROUP_NORMAL)
            gSaveBlock1.tvShows[i].common.active = FALSE;
    }
}

void DeleteExcessMixedShows(void)
{
    s8 showIdx;
    s8 numEmptyMixSlots;
    numEmptyMixSlots = 0;

    for (showIdx = NUM_NORMAL_TVSHOW_SLOTS; showIdx < LAST_TVSHOW_IDX; showIdx++)
    {
        if (gSaveBlock1.tvShows[showIdx].common.kind == TVSHOW_OFF_AIR)
            numEmptyMixSlots++;
    }
    for (showIdx = 0; showIdx < NUM_NORMAL_TVSHOW_SLOTS - numEmptyMixSlots; showIdx++)
        DeleteTVShowInArrayByIdx(gSaveBlock1.tvShows, showIdx + NUM_NORMAL_TVSHOW_SLOTS);
}

void SetMixedPokeNews(struct PokeNews player1[POKE_NEWS_COUNT], struct PokeNews player2[POKE_NEWS_COUNT], struct PokeNews player3[POKE_NEWS_COUNT], struct PokeNews player4[POKE_NEWS_COUNT]);
void ClearInvalidPokeNews(void);
void ClearPokeNewsIfGameNotComplete(void);
s8 GetPokeNewsSlotIfActive(struct PokeNews pokeNews[POKE_NEWS_COUNT], u8 idx);
void InitTryMixPokeNewsShow(struct PokeNews *dest[POKE_NEWS_COUNT], struct PokeNews *src[POKE_NEWS_COUNT]);

void ReceivePokeNewsData(void *src, u32 size, u8 playersLinkId)
{
    struct PokeNews (*pokeNews)[POKE_NEWS_COUNT];
    u8 i;

    for (i = 0; i < 4; i++)
        memcpy(sRecordMixPokeNews[i], src + i * size, 64);
    
    pokeNews = sRecordMixPokeNews;
    switch (playersLinkId)
    {
    case 0:
        SetMixedPokeNews(gSaveBlock1.pokeNews, pokeNews[1], pokeNews[2], pokeNews[3]);
        break;
    case 1:
        SetMixedPokeNews(pokeNews[0], gSaveBlock1.pokeNews, pokeNews[2], pokeNews[3]);
        break;
    case 2:
        SetMixedPokeNews(pokeNews[0], pokeNews[1], gSaveBlock1.pokeNews, pokeNews[3]);
        break;
    case 3:
        SetMixedPokeNews(pokeNews[0], pokeNews[1], pokeNews[2], gSaveBlock1.pokeNews);
        break;
    }
    ClearInvalidPokeNews();
    ClearPokeNewsIfGameNotComplete();
}

void SetMixedPokeNews(struct PokeNews player1[POKE_NEWS_COUNT], struct PokeNews player2[POKE_NEWS_COUNT], struct PokeNews player3[POKE_NEWS_COUNT], struct PokeNews player4[POKE_NEWS_COUNT])
{
    u8 i;
    u8 j;
    u8 k;
    struct PokeNews **newsLists[4];

    newsLists[0] = &player1;
    newsLists[1] = &player2;
    newsLists[2] = &player3;
    newsLists[3] = &player4;
    sTVShowNewsMixingNumPlayers = GetLinkPlayerCount();
    for (i = 0; i < POKE_NEWS_COUNT; i++)
    {
        for (j = 0; j < sTVShowNewsMixingNumPlayers; j++)
        {
            sTVShowMixingCurSlot = GetPokeNewsSlotIfActive(*newsLists[j], i);
            if (sTVShowMixingCurSlot != -1)
            {
                for (k = 0; k < sTVShowNewsMixingNumPlayers-1; k++)
                {
                    sCurTVShowSlot.slot = GetFirstEmptyPokeNewsSlot(*newsLists[(j + k + 1) % sTVShowNewsMixingNumPlayers]);
                    if (sCurTVShowSlot.slot != -1)
                        InitTryMixPokeNewsShow(newsLists[(j + k + 1) % sTVShowNewsMixingNumPlayers], newsLists[j]);
                }
            }
        }
    }
}

void InitTryMixPokeNewsShow(struct PokeNews *dest[POKE_NEWS_COUNT], struct PokeNews *src[POKE_NEWS_COUNT])
{
    struct PokeNews *destPokeNews;
    struct PokeNews *srcPokeNews;

    destPokeNews = dest[0];
    srcPokeNews = src[0];
    srcPokeNews += sTVShowMixingCurSlot;
    TryMixPokeNewsShow(destPokeNews, srcPokeNews, sCurTVShowSlot.slot);
}

bool8 TryMixPokeNewsShow(struct PokeNews *dest, struct PokeNews *src, s8 slot)
{
    u8 i;

    if (src->kind == POKENEWS_NONE)
        return FALSE;
    for (i = 0; i < POKE_NEWS_COUNT; i++)
    {
        if (dest[i].kind == src->kind)
            return FALSE;
    }
    dest[slot].kind = src->kind;
    dest[slot].state = POKENEWS_STATE_UPCOMING;
    dest[slot].dayCountdown = src->dayCountdown;
    return TRUE;
}

s8 GetPokeNewsSlotIfActive(struct PokeNews *pokeNews, u8 idx)
{
    if (pokeNews[idx].kind == POKENEWS_NONE)
        return -1;
    return idx;
}

void ClearInvalidPokeNews(void)
{
    u8 i;

    for (i = 0; i < POKE_NEWS_COUNT; i++)
    {
        if (gSaveBlock1.pokeNews[i].kind > POKENEWS_LILYCOVE)
            ClearPokeNewsBySlot(i);
    }
    CompactPokeNews();
}

void ClearPokeNewsIfGameNotComplete(void)
{
    u8 i;

    if (FlagGet(FLAG_SYS_GAME_CLEAR) != TRUE)
    {
        for (i = 0; i < POKE_NEWS_COUNT; i++)
            gSaveBlock1.pokeNews[i].state = POKENEWS_STATE_INACTIVE;
    }
}

void DoTVShow(void)
{
    if (gSaveBlock1.tvShows[gSpecialVar_0x8004].common.active != 0)
    {
        switch (gSaveBlock1.tvShows[gSpecialVar_0x8004].common.kind)
        {
        case TVSHOW_FAN_CLUB_LETTER:
            DoTVShowPokemonFanClubLetter();
            break;
        case TVSHOW_RECENT_HAPPENINGS:
            DoTVShowRecentHappenings();
            break;
        case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
            DoTVShowPokemonFanClubOpinions();
            break;
        case TVSHOW_DUMMY:
            DoTVShowDummiedOut();
            break;
        case TVSHOW_MASS_OUTBREAK:
            DoTVShowPokemonNewsMassOutbreak();
            break;
        case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
            DoTVShowBravoTrainerPokemonProfile();
            break;
        case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
            DoTVShowBravoTrainerBattleTower();
            break;
        case TVSHOW_POKEMON_TODAY_CAUGHT:
            DoTVShowPokemonTodaySuccessfulCapture();
            break;
        case TVSHOW_SMART_SHOPPER:
            DoTVShowTodaysSmartShopper();
            break;
        case TVSHOW_NAME_RATER_SHOW:
            DoTVShowTheNameRaterShow();
            break;
        case TVSHOW_POKEMON_TODAY_FAILED:
            DoTVShowPokemonTodayFailedCapture();
            break;
        case TVSHOW_FISHING_ADVICE:
            DoTVShowPokemonAngler();
            break;
        case TVSHOW_WORLD_OF_MASTERS:
            DoTVShowTheWorldOfMasters();
            break;
        }
    }
}

void TVShowConvertInternationalString(u8 *dest, u8 *src, u8 language)
{
    StringCopy(dest, src);
    if (language < LANGUAGE_ENGLISH)
        ConvertInternationalString(dest, LANGUAGE_JAPANESE);
}

void DoTVShowBravoTrainerPokemonProfile(void)
{
    struct TVShowBravoTrainerPokemonProfiles *bravoTrainer = &gSaveBlock1.tvShows[gSpecialVar_0x8004].bravoTrainer;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch(state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, bravoTrainer->playerName, bravoTrainer->language);
        CopyContestCategoryToStringVar(1, bravoTrainer->contestCategory);
        CopyContestRankToStringVar(2, bravoTrainer->contestRank);
        if (!StringCompareWithoutExtCtrlCodes(gSpeciesNames[bravoTrainer->species], bravoTrainer->pokemonNickname))
            sTVShowState = 8;
        else
            sTVShowState = 1;
        break;
    case 1:
        StringCopy(gStringVar1, gSpeciesNames[bravoTrainer->species]);
        TVShowConvertInternationalString(gStringVar2, bravoTrainer->pokemonNickname, bravoTrainer->pokemonNameLanguage);
        CopyContestCategoryToStringVar(2, bravoTrainer->contestCategory);
        sTVShowState = 2;
        break;
    case 2:
        TVShowConvertInternationalString(gStringVar1, bravoTrainer->playerName, bravoTrainer->language);
        if (bravoTrainer->contestResult == 0) // placed first
            sTVShowState = 3;
        else
            sTVShowState = 4;
        break;
    case 3:
        TVShowConvertInternationalString(gStringVar1, bravoTrainer->playerName, bravoTrainer->language);
        CopyEasyChatWord(gStringVar2, bravoTrainer->words[0]);
        ConvertIntToDecimalString(2, bravoTrainer->contestResult + 1);
        sTVShowState = 5;
        break;
    case 4:
        TVShowConvertInternationalString(gStringVar1, bravoTrainer->playerName, bravoTrainer->language);
        CopyEasyChatWord(gStringVar2, bravoTrainer->words[0]);
        ConvertIntToDecimalString(2, bravoTrainer->contestResult + 1);
        sTVShowState = 5;
        break;
    case 5:
        TVShowConvertInternationalString(gStringVar1, bravoTrainer->playerName, bravoTrainer->language);
        CopyContestCategoryToStringVar(1, bravoTrainer->contestCategory);
        CopyEasyChatWord(gStringVar3, bravoTrainer->words[1]);
        if (bravoTrainer->move)
            sTVShowState = 6;
        else
            sTVShowState = 7;
        break;
    case 6:
        StringCopy(gStringVar1, gSpeciesNames[bravoTrainer->species]);
        StringCopy(gStringVar2, gMoveNames[bravoTrainer->move]);
        CopyEasyChatWord(gStringVar3, bravoTrainer->words[1]);
        sTVShowState = 7;
        break;
    case 7:
        TVShowConvertInternationalString(gStringVar1, bravoTrainer->playerName, bravoTrainer->language);
        StringCopy(gStringVar2, gSpeciesNames[bravoTrainer->species]);
        TVShowDone();
        break;
    case 8:
        StringCopy(gStringVar1, gSpeciesNames[bravoTrainer->species]);
        sTVShowState = 2;
        break;
    }
    ShowFieldMessage(sTVBravoTrainerTextGroup[state]);
}

void DoTVShowBravoTrainerBattleTower(void)
{
    struct TVShowBravoTrainerBattleTowerSpotlight *bravoTrainerTower = &gSaveBlock1.tvShows[gSpecialVar_0x8004].bravoTrainerTower;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch(state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->playerName, bravoTrainerTower->language);
        StringCopy(gStringVar2, gSpeciesNames[bravoTrainerTower->species]);
        if (bravoTrainerTower->numFights >= 7)
            sTVShowState = 1;
        else
            sTVShowState = 2;
        break;
    case 1:
        ConvertIntToDecimalString(0, bravoTrainerTower->btLevel);
        ConvertIntToDecimalString(1, bravoTrainerTower->numFights);
        if (bravoTrainerTower->wonTheChallenge == 1)
            sTVShowState = 3;
        else
            sTVShowState = 4;
        break;
    case 2:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->opponentName, bravoTrainerTower->language);
        ConvertIntToDecimalString(1, bravoTrainerTower->numFights + 1);
        if (bravoTrainerTower->interviewResponse == 0)
            sTVShowState = 5;
        else
            sTVShowState = 6;
        break;
    case 3:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->opponentName, bravoTrainerTower->language);
        StringCopy(gStringVar2, gSpeciesNames[bravoTrainerTower->defeatedSpecies]);
        if (bravoTrainerTower->interviewResponse == 0)
            sTVShowState = 5;
        else
            sTVShowState = 6;
        break;
    case 4:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->opponentName, bravoTrainerTower->language);
        StringCopy(gStringVar2, gSpeciesNames[bravoTrainerTower->defeatedSpecies]);
        if (bravoTrainerTower->interviewResponse == 0)
            sTVShowState = 5;
        else
            sTVShowState = 6;
        break;
    case 5:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->opponentName, bravoTrainerTower->language);
        sTVShowState = 11;
        break;
    case 6:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->opponentName, bravoTrainerTower->language);
        sTVShowState = 11;
        break;
    case 7:
        sTVShowState = 11;
        break;
    case 8:
    case 9:
    case 10:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->playerName, bravoTrainerTower->language);
        sTVShowState = 11;
        break;
    case 11:
        CopyEasyChatWord(gStringVar1, bravoTrainerTower->words[0]);
        if (bravoTrainerTower->interviewResponse == 0)
            sTVShowState = 12;
        else
            sTVShowState = 13;
        break;
    case 12:
    case 13:
        CopyEasyChatWord(gStringVar1, bravoTrainerTower->words[0]);
        TVShowConvertInternationalString(gStringVar2, bravoTrainerTower->playerName, bravoTrainerTower->language);
        TVShowConvertInternationalString(gStringVar3, bravoTrainerTower->opponentName, bravoTrainerTower->language);
        sTVShowState = 14;
        break;
    case 14:
        TVShowConvertInternationalString(gStringVar1, bravoTrainerTower->playerName, bravoTrainerTower->language);
        StringCopy(gStringVar2, gSpeciesNames[bravoTrainerTower->species]);
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVBravoTrainerBattleTowerTextGroup[state]);
}

void DoTVShowTodaysSmartShopper(void)
{
    struct TVShowSmartShopper *smartShopper = &gSaveBlock1.tvShows[gSpecialVar_0x8004].smartshopperShow;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch(state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, smartShopper->playerName, smartShopper->language);
        GetMapName(gStringVar2, smartShopper->shopLocation, 0);
        if (smartShopper->itemAmounts[0] >= 0xff)
            sTVShowState = 11;
        else
            sTVShowState = 1;
        break;
    case 1:
        TVShowConvertInternationalString(gStringVar1, smartShopper->playerName, smartShopper->language);
        StringCopy(gStringVar2, GetItemName(smartShopper->itemIds[0]));
        ConvertIntToDecimalString(2, smartShopper->itemAmounts[0]);
        sTVShowState += (Random() % 4) + 1;
        break;
    case 2:
    case 4:
    case 5:
        if (smartShopper->itemIds[1] != 0)
            sTVShowState = 6;
        else
            sTVShowState = 10;
        break;
    case 3:
        ConvertIntToDecimalString(2, smartShopper->itemAmounts[0] + 1);
        if (smartShopper->itemIds[1] != 0)
            sTVShowState = 6;
        else
            sTVShowState = 10;
        break;
    case 6:
        StringCopy(gStringVar2, GetItemName(smartShopper->itemIds[1]));
        ConvertIntToDecimalString(2, smartShopper->itemAmounts[1]);
        if (smartShopper->itemIds[2] != 0)
            sTVShowState = 7;
        else if (smartShopper->priceReduced == 1)
            sTVShowState = 8;
        else
            sTVShowState = 9;
        break;
    case 7:
        StringCopy(gStringVar2, GetItemName(smartShopper->itemIds[2]));
        ConvertIntToDecimalString(2, smartShopper->itemAmounts[2]);
        if (smartShopper->priceReduced == 1)
            sTVShowState = 8;
        else
            sTVShowState = 9;
        break;
    case 8:
        if (smartShopper->itemAmounts[0] < 0xff)
            sTVShowState = 9;
        else
            sTVShowState = 12;
        break;
    case 9:
        SmartShopper_BufferPurchaseTotal(1, smartShopper);
        TVShowDone();
        break;
    case 10:
        if (smartShopper->priceReduced == 1)
            sTVShowState = 8;
        else
            sTVShowState = 9;
        break;
    case 11:
        TVShowConvertInternationalString(gStringVar1, smartShopper->playerName, smartShopper->language);
        StringCopy(gStringVar2, GetItemName(smartShopper->itemIds[0]));
        if (smartShopper->priceReduced == 1)
            sTVShowState = 8;
        else
            sTVShowState = 12;
        break;
    case 12:
        TVShowConvertInternationalString(gStringVar1, smartShopper->playerName, smartShopper->language);
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVTodaysSmartShopperTextGroup[state]);
}

void DoTVShowTheNameRaterShow(void)
{
    struct TVShowNameRaterShow *nameRaterShow = &gSaveBlock1.tvShows[gSpecialVar_0x8004].nameRaterShow;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, nameRaterShow->trainerName, nameRaterShow->language);
        StringCopy(gStringVar2, gSpeciesNames[nameRaterShow->species]);
        TVShowConvertInternationalString(gStringVar3, nameRaterShow->pokemonName, nameRaterShow->pokemonNameLanguage);
        sTVShowState = GetRandomNameRaterStateFromName(nameRaterShow) + 1;
        break;
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        if (nameRaterShow->random == 0)
            sTVShowState = 9;
        else if (nameRaterShow->random == 1)
            sTVShowState = 10;
        else if (nameRaterShow->random == 2)
            sTVShowState = 11;
        break;
    case 2:
        TVShowConvertInternationalString(gStringVar1, nameRaterShow->trainerName, nameRaterShow->language);
        if (nameRaterShow->random == 0)
            sTVShowState = 9;
        else if (nameRaterShow->random == 1)
            sTVShowState = 10;
        else if (nameRaterShow->random == 2)
            sTVShowState = 11;
        break;
    case 9:
    case 10:
    case 11:
        GetNicknameSubstring(0, 1, 0, 1, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar3, gStringVar1, nameRaterShow->pokemonNameLanguage);
        GetNicknameSubstring(0, 0, 0, 1, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar2, gStringVar1, nameRaterShow->pokemonNameLanguage);
        TVShowConvertInternationalString(gStringVar1, nameRaterShow->pokemonName, nameRaterShow->pokemonNameLanguage);
        sTVShowState = 12;
        break;
    case 13:
        GetNicknameSubstring(0, 0, 3, 1, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar3, gStringVar1, nameRaterShow->pokemonNameLanguage);
        GetNicknameSubstring(0, 0, 2, 0, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar2, gStringVar1, nameRaterShow->language);
        TVShowConvertInternationalString(gStringVar1, nameRaterShow->trainerName, nameRaterShow->language);
        sTVShowState = 14;
        break;
    case 14:
        GetNicknameSubstring(0, 0, 3, 0, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar3, gStringVar1, nameRaterShow->language);
        GetNicknameSubstring(0, 0, 2, 1, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar2, gStringVar1, nameRaterShow->pokemonNameLanguage);
        TVShowConvertInternationalString(gStringVar1, nameRaterShow->trainerName, nameRaterShow->language);
        sTVShowState = 18;
        break;
    case 15:
        GetNicknameSubstring(1, 0, 2, 1, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar1, gStringVar2, nameRaterShow->pokemonNameLanguage);
        StringCopy(gStringVar2, gSpeciesNames[nameRaterShow->species]);
        GetNicknameSubstring(2, 0, 3, 2, nameRaterShow->species, nameRaterShow);
        sTVShowState = 16;
        break;
    case 16:
        GetNicknameSubstring(0, 0, 3, 1, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar3, gStringVar1, nameRaterShow->pokemonNameLanguage);
        GetNicknameSubstring(0, 0, 2, 2, nameRaterShow->species, nameRaterShow);
        sTVShowState = 17;
        break;
    case 17:
        GetNicknameSubstring(1, 0, 2, 1, 0, nameRaterShow);
        TVShowConvertInternationalString(gStringVar1, gStringVar2, nameRaterShow->pokemonNameLanguage);
        GetNicknameSubstring(2, 0, 3, 2, nameRaterShow->randomSpecies, nameRaterShow);
        StringCopy(gStringVar2, gSpeciesNames[nameRaterShow->randomSpecies]);
        sTVShowState = 18;
        break;
    case 12:
        state = 18;
        sTVShowState = 18;
    case 18:
        TVShowConvertInternationalString(gStringVar1, nameRaterShow->pokemonName, nameRaterShow->pokemonNameLanguage);
        TVShowConvertInternationalString(gStringVar2, nameRaterShow->trainerName, nameRaterShow->language);
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVNameRaterTextGroup[state]);
}

void DoTVShowPokemonTodaySuccessfulCapture(void)
{
    struct TVShowPokemonToday *pokemonToday = &gSaveBlock1.tvShows[gSpecialVar_0x8004].pokemonToday;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, pokemonToday->playerName, pokemonToday->language);
        StringCopy(gStringVar2, gSpeciesNames[pokemonToday->species]);
        TVShowConvertInternationalString(gStringVar3, pokemonToday->nickname, pokemonToday->language2);
        if (pokemonToday->ball == ITEM_MASTER_BALL)
            sTVShowState = 5;
        else
            sTVShowState = 1;
        break;
    case 1:
        sTVShowState = 2;
        break;
    case 2:
        StringCopy(gStringVar2, GetItemName(pokemonToday->ball));
        ConvertIntToDecimalString(2, pokemonToday->nBallsUsed);
        if (pokemonToday->nBallsUsed < 4)
            sTVShowState = 3;
        else
            sTVShowState = 4;
        break;
    case 3:
        TVShowConvertInternationalString(gStringVar1, pokemonToday->playerName, pokemonToday->language);
        StringCopy(gStringVar2, gSpeciesNames[pokemonToday->species]);
        TVShowConvertInternationalString(gStringVar3, pokemonToday->nickname, pokemonToday->language2);
        sTVShowState = 6;
        break;
    case 4:
        sTVShowState = 6;
        break;
    case 5:
        TVShowConvertInternationalString(gStringVar1, pokemonToday->playerName, pokemonToday->language);
        StringCopy(gStringVar2, gSpeciesNames[pokemonToday->species]);
        sTVShowState = 6;
        break;
    case 6:
        TVShowConvertInternationalString(gStringVar1, pokemonToday->playerName, pokemonToday->language);
        StringCopy(gStringVar2, gSpeciesNames[pokemonToday->species]);
        TVShowConvertInternationalString(gStringVar3, pokemonToday->nickname, pokemonToday->language2);
        sTVShowState += (Random() % 4) + 1;
        break;
    case 7:
    case 8:
        StringCopy(gStringVar1, gSpeciesNames[pokemonToday->species]);
        TVShowConvertInternationalString(gStringVar2, pokemonToday->nickname, pokemonToday->language2);
        GetRandomDifferentSpeciesAndNameSeenByPlayer(2, pokemonToday->species);
        sTVShowState = 11;
        break;
    case 9:
    case 10:
        StringCopy(gStringVar1, gSpeciesNames[pokemonToday->species]);
        TVShowConvertInternationalString(gStringVar2, pokemonToday->nickname, pokemonToday->language2);
        sTVShowState = 11;
        break;
    case 11:
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVPokemonTodaySuccessfulTextGroup[state]);
}

void DoTVShowPokemonTodayFailedCapture(void)
{
    struct TVShowPokemonTodayFailed *pokemonTodayFailed = &gSaveBlock1.tvShows[gSpecialVar_0x8004].pokemonTodayFailed;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, pokemonTodayFailed->playerName, pokemonTodayFailed->language);
        StringCopy(gStringVar2, gSpeciesNames[pokemonTodayFailed->species]);
        sTVShowState = 1;
        break;
    case 1:
        TVShowConvertInternationalString(gStringVar1, pokemonTodayFailed->playerName, pokemonTodayFailed->language);
        GetMapName(gStringVar2, pokemonTodayFailed->location, 0);
        StringCopy(gStringVar3, gSpeciesNames[pokemonTodayFailed->species2]);
        if (pokemonTodayFailed->outcome == 1)
            sTVShowState = 3;
        else
            sTVShowState = 2;
        break;
    case 2:
    case 3:
        TVShowConvertInternationalString(gStringVar1, pokemonTodayFailed->playerName, pokemonTodayFailed->language);
        ConvertIntToDecimalString(1, pokemonTodayFailed->nBallsUsed);
        if ((Random() % 3) == 0)
            sTVShowState = 5;
        else
            sTVShowState = 4;
        break;
    case 4:
    case 5:
        TVShowConvertInternationalString(gStringVar1, pokemonTodayFailed->playerName, pokemonTodayFailed->language);
        sTVShowState = 6;
        break;
    case 6:
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVPokemonTodayFailedTextGroup[state]);
}

void DoTVShowPokemonFanClubLetter(void)
{
    struct TVShowFanClubLetter *fanclubLetter = &gSaveBlock1.tvShows[gSpecialVar_0x8004].fanclubLetter;
    u8 state;
    u16 rval;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, fanclubLetter->playerName, fanclubLetter->language);
        StringCopy(gStringVar2, gSpeciesNames[fanclubLetter->species]);
        sTVShowState = 50;
        break;
    case 1:
        rval = (Random() % 4) + 1;
        if (rval == 1)
            sTVShowState = 2;
        else
            sTVShowState = rval + 2;
        break;
    case 2:
        sTVShowState = 51;
        break;
    case 3:
        sTVShowState += (Random() % 3) + 1;
        break;
    case 4:
    case 5:
    case 6:
        GetRandomWordFromShow((TVShow *)fanclubLetter);
        sTVShowState = 7;
        break;
    case 7:
        rval = (Random() % 0x1f) + 0x46;
        ConvertIntToDecimalString(2, rval);
        TVShowDone();
        break;
    case 50:
        ConvertEasyChatWordsToString(gStringVar4, fanclubLetter->words, 2, 2);
        ShowFieldMessage(gStringVar4);
        sTVShowState = 1;
        return;
    case 51:
        ConvertEasyChatWordsToString(gStringVar4, fanclubLetter->words, 2, 2);
        ShowFieldMessage(gStringVar4);
        sTVShowState = 3;
        return;
    }
    ShowFieldMessage(sTVFanClubTextGroup[state]);
}

void DoTVShowRecentHappenings(void)
{
    struct TVShowRecentHappenings *recentHappenings = &gSaveBlock1.tvShows[gSpecialVar_0x8004].recentHappenings;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, recentHappenings->playerName, recentHappenings->language);
        GetRandomWordFromShow((TVShow *)recentHappenings);
        sTVShowState = 50;
        break;
    case 1:
        sTVShowState += 1 + (Random() % 3);
        break;
    case 2:
    case 3:
    case 4:
        sTVShowState = 5;
        break;
    case 5:
        TVShowDone();
        break;
    case 50:
        ConvertEasyChatWordsToString(gStringVar4, recentHappenings->words, 2, 2);
        ShowFieldMessage(gStringVar4);
        sTVShowState = 1;
        return;
    }
    ShowFieldMessage(sTVRecentHappeninssTextGroup[state]);
}

void DoTVShowPokemonFanClubOpinions(void)
{
    struct TVShowFanClubOpinions *fanclubOpinions = &gSaveBlock1.tvShows[gSpecialVar_0x8004].fanclubOpinions;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, fanclubOpinions->playerName, fanclubOpinions->language);
        StringCopy(gStringVar2, gSpeciesNames[fanclubOpinions->species]);
        TVShowConvertInternationalString(gStringVar3, fanclubOpinions->nickname, fanclubOpinions->pokemonNameLanguage);
        sTVShowState = fanclubOpinions->questionAsked + 1;
        break;
    case 1:
    case 2:
    case 3:
        TVShowConvertInternationalString(gStringVar1, fanclubOpinions->playerName, fanclubOpinions->language);
        StringCopy(gStringVar2, gSpeciesNames[fanclubOpinions->species]);
        CopyEasyChatWord(gStringVar3, fanclubOpinions->words[0]);
        sTVShowState = 4;
        break;
    case 4:
        TVShowConvertInternationalString(gStringVar1, fanclubOpinions->playerName, fanclubOpinions->language);
        CopyEasyChatWord(gStringVar3, fanclubOpinions->words[1]);
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVFanClubOpinionsTextGroup[state]);
}

void DoTVShowDummiedOut(void)
{
}

void DoTVShowPokemonNewsMassOutbreak(void)
{
    struct TVShowMassOutbreak *massOutbreak = &gSaveBlock1.tvShows[gSpecialVar_0x8004].massOutbreak;

    GetMapName(gStringVar1, massOutbreak->locationMapNum, 0);
    StringCopy(gStringVar2, gSpeciesNames[massOutbreak->species]);
    TVShowDone();
    StartMassOutbreak();
    ShowFieldMessage(sTVMassOutbreakTextGroup[sTVShowState]);
}

void DoTVShowInSearchOfTrainers(void)
{
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        GetMapName(gStringVar1, gSaveBlock1.gabbyAndTyData.mapnum, 0);
        if (gSaveBlock1.gabbyAndTyData.battleNum > 1)
            sTVShowState = 1;
        else
            sTVShowState = 2;
        break;
    case 1:
        sTVShowState = 2;
        break;
    case 2:
        if (gSaveBlock1.gabbyAndTyData.battleTookMoreThanOneTurn == 0)
            sTVShowState = 4;
        else if (gSaveBlock1.gabbyAndTyData.playerThrewABall != 0)
            sTVShowState = 5;
        else if (gSaveBlock1.gabbyAndTyData.playerUsedHealingItem != 0)
            sTVShowState = 6;
        else if (gSaveBlock1.gabbyAndTyData.playerLostAMon != 0)
            sTVShowState = 7;
        else
            sTVShowState = 3;
        break;
    case 3:
        StringCopy(gStringVar1, gSpeciesNames[gSaveBlock1.gabbyAndTyData.mon1]);
        StringCopy(gStringVar2, gMoveNames[gSaveBlock1.gabbyAndTyData.lastMove]);
        StringCopy(gStringVar3, gSpeciesNames[gSaveBlock1.gabbyAndTyData.mon2]);
        sTVShowState = 8;
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        sTVShowState = 8;
        break;
    case 8:
        CopyEasyChatWord(gStringVar1, gSaveBlock1.gabbyAndTyData.quote);
        StringCopy(gStringVar2, gSpeciesNames[gSaveBlock1.gabbyAndTyData.mon1]);
        StringCopy(gStringVar3, gSpeciesNames[gSaveBlock1.gabbyAndTyData.mon2]);
        gSpecialVar_Result = 1;
        sTVShowState = 0;;
        TakeGabbyAndTyOffTheAir();
        break;
    }
    ShowFieldMessage(sTVInSearchOfTrainersTextGroup[state]);
}

void DoTVShowPokemonAngler(void)
{
    struct TVShowPokemonAngler *pokemonAngler = &gSaveBlock1.tvShows[gSpecialVar_0x8004].pokemonAngler;
    u8 state;

    gSpecialVar_Result = 0;
    if (pokemonAngler->nBites < pokemonAngler->nFails)
        sTVShowState = 0;
    else
        sTVShowState = 1;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, pokemonAngler->playerName, pokemonAngler->language);
        StringCopy(gStringVar2, gSpeciesNames[pokemonAngler->species]);
        ConvertIntToDecimalString(2, pokemonAngler->nFails);
        TVShowDone();
        break;
    case 1:
        TVShowConvertInternationalString(gStringVar1, pokemonAngler->playerName, pokemonAngler->language);
        StringCopy(gStringVar2, gSpeciesNames[pokemonAngler->species]);
        ConvertIntToDecimalString(2, pokemonAngler->nBites);
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVPokemonAnglerTextGroup[state]);
}

void DoTVShowTheWorldOfMasters(void)
{
    struct TVShowWorldOfMasters *worldOfMasters = &gSaveBlock1.tvShows[gSpecialVar_0x8004].worldOfMasters;
    u8 state;

    gSpecialVar_Result = 0;
    state = sTVShowState;
    switch (state)
    {
    case 0:
        TVShowConvertInternationalString(gStringVar1, worldOfMasters->playerName,
                                         worldOfMasters->language);
        ConvertIntToDecimalString(1, worldOfMasters->steps);
        ConvertIntToDecimalString(2, worldOfMasters->numPokeCaught);
        sTVShowState = 1;
        break;
    case 1:
        StringCopy(gStringVar1, gSpeciesNames[worldOfMasters->species]);
        sTVShowState = 2;
        break;
    case 2:
        TVShowConvertInternationalString(gStringVar1, worldOfMasters->playerName,
                                         worldOfMasters->language);
        GetMapName(gStringVar2, worldOfMasters->location, 0);
        StringCopy(gStringVar3, gSpeciesNames[worldOfMasters->caughtPoke]);
        TVShowDone();
        break;
    }
    ShowFieldMessage(sTVWorldOfMastersTextGroup[state]);
}

void TVShowDone(void)
{
    gSpecialVar_Result = 1;
    sTVShowState = 0;
    gSaveBlock1.tvShows[gSpecialVar_0x8004].common.active = 0;
}

void ResetTVShowState(void)
{
    sTVShowState = 0;
}
