#include "global.h"
#include "new_game.h"
#include "battle_records.h"
#include "berry.h"
#include "clock.h"
#include "constants/maps.h"
#include "constants/species.h"
#include "contest.h"
#include "contest_util.h"
#include "decoration_inventory.h"
#include "dewford_trend.h"
#include "easy_chat.h"
#include "event_data.h"
#include "field_specials.h"
#include "item_menu.h"
#include "lottery_corner.h"
#include "mail.h"
#include "mauville_old_man.h"
#include "overworld.h"
#include "play_time.h"
#include "player_pc.h"
#include "pokeblock.h"
#include "pokedex.h"
#include "pokemon_size_record.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "roamer.h"
#include "rtc.h"
#include "script.h"
#include "secret_base.h"
#include "text.h"
#include "tv.h"

EWRAM_DATA u8 gDifferentSaveFile = 0;
EWRAM_DATA u8 gIsDebugMode = 0;

extern u8 gPlayerPartyCount;
extern u8 gUnusedPokedexU8;
extern u16 gSaveFileStatus;

extern u8 EventScript_ResetAllMapFlags[];

static const struct ContestWinner sEmptyMuseumPortrait =
{
    .monName = {EOS},
    .trainerName = {EOS},
};

#if DEBUG
const s8 gDebugWarpDestinations[][2] =
{
    { MAP_GROUP(MAP_INSIDE_OF_TRUCK),                   MAP_NUM(MAP_INSIDE_OF_TRUCK) },
    { MAP_GROUP(MAP_OLDALE_TOWN_POKEMON_CENTER_2F),     MAP_NUM(MAP_OLDALE_TOWN_POKEMON_CENTER_2F) },
    { MAP_GROUP(MAP_LITTLEROOT_TOWN),                   MAP_NUM(MAP_LITTLEROOT_TOWN) },
    { MAP_GROUP(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F), MAP_NUM(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F) }
};

const u8 sDebugNickname_Krote[] = _("KRÖTE");
#endif

void SetTrainerId(u32 trainerId, u8 *dst)
{
    dst[0] = trainerId;
    dst[1] = trainerId >> 8;
    dst[2] = trainerId >> 16;
    dst[3] = trainerId >> 24;
}

void CopyTrainerId(u8 *dst, u8 *src)
{
    s32 i;
    for (i = 0; i < 4; i++)
        dst[i] = src[i];
}

void InitPlayerTrainerId(void)
{
    SetTrainerId((Random() << 16) | Random(), gSaveBlock2.playerTrainerId);
}

// L=A isnt set here for some reason.
void SetDefaultOptions(void)
{
    gSaveBlock2.optionsTextSpeed = OPTIONS_TEXT_SPEED_MID;
    gSaveBlock2.optionsWindowFrameType = 0;
    gSaveBlock2.optionsSound = OPTIONS_SOUND_MONO;
    gSaveBlock2.optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    gSaveBlock2.optionsBattleSceneOff = FALSE;
    gSaveBlock2.regionMapZoom = FALSE;
}

void ClearPokedexFlags(void)
{
    gUnusedPokedexU8 = 0;
    memset(&gSaveBlock2.pokedex.owned, 0, sizeof(gSaveBlock2.pokedex.owned));
    memset(&gSaveBlock2.pokedex.seen, 0, sizeof(gSaveBlock2.pokedex.seen));
}

void ClearAllContestWinnerPics(void)
{
    s32 i;

    Contest_ResetWinners();
    for (i = 0; i < 5; i++)
        gSaveBlock1.museumPortraits[i] = sEmptyMuseumPortrait;
}

void ClearBattleTowerData(void)
{
    CpuFill32(0, &gSaveBlock2.battleTower, sizeof(gSaveBlock2.battleTower));
}

#if DEBUG
void DebugSetNextWarpDestination()
{
    u8 i;

    for (i = 0; i < 4; i++)
    {
        if (gDebugWarpDestinations[i][0] == gSaveBlock1.location.mapGroup &&
            gDebugWarpDestinations[i][1] == gSaveBlock1.location.mapNum)
        {
            break;
        }
    }

    i++;
    if (i >= 4)
    {
        i = 0;
    }

    Overworld_SetWarpDestination(gDebugWarpDestinations[i][0], gDebugWarpDestinations[i][1], -1, -1, -1);
}
#endif

void WarpToTruck(void)
{
    Overworld_SetWarpDestination(MAP_GROUP(MAP_INSIDE_OF_TRUCK), MAP_NUM(MAP_INSIDE_OF_TRUCK), -1, -1, -1);
    WarpIntoMap();
}

void Sav2_ClearSetDefault(void)
{
    CpuFill16(0, &gSaveBlock2, sizeof(gSaveBlock2));
    SetDefaultOptions();
}

void ResetMenuAndMonGlobals(void)
{
    gDifferentSaveFile = 0;
#if DEBUG
    gIsDebugMode = 0;
#endif
    ResetPokedexScrollPositions();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetBagScrollPositions();
}

void NewGameInitData(void)
{
    if (gSaveFileStatus == 0 || gSaveFileStatus == 2)
        RtcReset();

    gDifferentSaveFile = 1;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetPokedex();
    ClearBattleTowerData();
    memset(&gSaveBlock1, 0, sizeof(gSaveBlock1));
    ClearAllMail();
    gSaveBlock2.specialSaveWarp = 0;
    InitPlayerTrainerId();
    PlayTimeCounter_Reset();
    ClearPokedexFlags();
    InitEventData();
    ClearTVShowData();
    ResetGabbyAndTy();
    ClearSecretBases();
    ClearBerryTrees();
    gSaveBlock1.money = 3000;
    ResetLinkContestBoolean();
    ResetGameStats();
    ClearAllContestWinnerPics();
    ClearPlayerLinkBattleRecords();
    InitShroomishSizeRecord();
    InitBarboachSizeRecord();
    gPlayerPartyCount = 0;
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ClearRoamerData();
    ClearRoamerLocationData();
    gSaveBlock1.registeredItem = 0;
    ClearBag();
    NewGameInitPCItems();
    ClearPokeblocks();
    ClearDecorationInventories();
    InitEasyChatPhrases();
    SetMauvilleOldMan();
    InitDewfordTrend();
    ResetFanClub();
    ResetLotteryCorner();
    WarpToTruck();
    RunScriptImmediately(EventScript_ResetAllMapFlags);
}

#if DEBUG
extern void Debug_FillPokemonStorage(void);
extern void Debug_InitBagItems(void);
extern void Debug_FillPokeblockCase(void);
extern void SetDexPokemonPokenavFlags(void);

void Debug_NewGameInit(bool32 setKroteNickname)
{
    gIsDebugMode = 1;
    NewGameInitData();
    gSaveBlock1.money = 999999;
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKENAV_GET);
    FlagSet(FLAG_SYS_B_DASH);
    ScriptGiveMon(SPECIES_TREECKO, 99, 0, 0, 0, 0);

    if (setKroteNickname == TRUE)
        SetMonData(&gPlayerParty[0], MON_DATA_NICKNAME, sDebugNickname_Krote);

    Debug_FillPokemonStorage();
    Debug_InitBagItems();
    Debug_FillPokeblockCase();
    FlagSet(FLAG_SYS_HIPSTER_MEET);
    UnlockRandomTrendySaying();
    SetDexPokemonPokenavFlags();
    InitTimeBasedEvents();
}
#endif
