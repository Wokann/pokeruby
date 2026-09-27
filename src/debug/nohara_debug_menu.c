#if DEBUG
#include "global.h"
#include "constants/flags.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/items.h"
#include "main.h"
#include "menu.h"
#include "start_menu.h"
#include "tv.h"
#include "event_data.h"
#include "string_util.h"
#include "field_specials.h"
#include "sound.h"

static u8 sTVDataTypeIndex;
static u8 sTVDataSlotIndex;
static u8 sTVShowActiveStateDisplayed;

bool8 NoharaDebugMenu_HandleInput(void);
bool8 NoharaDebugMenu_TV(void);
bool8 NoharaDebugMenu_TV_HandleInput(void);
void NoharaDebugMenu_TV_ShowSecretTypePrompt(void);
bool8 NoharaDebugMenu_TV_ClearAllActiveFlags(void);
bool8 NoharaDebugMenu_TV_SetPopulatedShowsActive(void);
bool8 NoharaDebugMenu_TV_ShowSlotMachineHitTable(void);
bool8 NoharaDebugMenu_TV_HandleSecretTypePrompt(void);
void NoharaDebugMenu_TV_PrintDataKinds(void);
void NoharaDebugMenu_TV_PrintDataStates(void);
bool8 NoharaDebugMenu_TV_EnableBroadcasts(void);
void NoharaDebugMenu_TV_OpenCreateShowMenu(void);
bool8 NoharaDebugMenu_TV_HandleCreateShowInput(void);
void NoharaDebugMenu_TV_CreateShow(u8, u8);
bool8 NoharaDebugMenu_TV_ClearShowData(void);
void NoharaDebugMenu_TV_OpenCreateCommercialMenu(void);
bool8 NoharaDebugMenu_TV_HandleCreateCommercialInput(void);
void NoharaDebugMenu_TV_CreateCommercial(u8, u8);
bool8 NoharaDebugMenu_TV_FillEmptySlots(void);
bool8 NoharaDebugMenu_Fan(void);
bool8 NoharaDebugMenu_Fan_HandleInput(void);
bool8 NoharaDebugMenu_Fan_Start(void);
bool8 NoharaDebugMenu_Fan_GainRandomFan(void);
bool8 NoharaDebugMenu_Fan_LoseRandomFan(void);
bool8 NoharaDebugMenu_Fan_ShowPoints(void);
bool8 NoharaDebugMenu_Fan_AddSixHours(void);
bool8 NoharaDebugMenu_Fan_GainAfterEliteFour(void);
bool8 NoharaDebugMenu_Fan_GainAfterSecretBase(void);
bool8 NoharaDebugMenu_Fan_GainAfterContest(void);
bool8 NoharaDebugMenu_Fan_GainAfterBattleTower(void);
bool8 NoharaDebugMenu_WaitForAButton(void);
bool8 NoharaDebugMenu_BattleVSDad(void);
bool8 NoharaDebugMenu_DadAfterBattle(void);
bool8 NoharaDebugMenu_SootopolisCity(void);
bool8 NoharaDebugMenu_ResetMrBriney(void);
bool8 NoharaDebugMenu_Yes9999(void);
bool8 NoharaDebugMenu_LegendsFlagOn(void);
bool8 NoharaDebugMenu_AddNumWinningStreaks(void);

const u8 sString_NoharaDebug_TV[] = _("TV");
const u8 sString_NoharaDebug_FanClub[] = DTR("Fan", "LILYCOVE TRAINER FAN CLUB");
const u8 sString_NoharaDebug_BattleVsDad[] = _("Battle (vs Dad)");
const u8 sString_NoharaDebug_DadAfterBattle[] = _("Dad after battle");
const u8 sString_NoharaDebug_SootopolisCity[] = _("SOOTOPOLIS CITY");
const u8 sString_NoharaDebug_ResetMrBriney[] = DTR("Embark", "Reset Mr. BRINEY");
const u8 sString_NoharaDebug_Yes9999[] = _("Yes 9999");
const u8 sString_NoharaDebug_LegendsFlagOn[] = _("Legends flag ON");
const u8 sString_NoharaDebug_AddWinningStreaks[] = _("Add num of winning streaks");

const struct MenuAction sMenuActions_NoharaDebug[] = {
    {sString_NoharaDebug_TV, NoharaDebugMenu_TV},
    {sString_NoharaDebug_FanClub, NoharaDebugMenu_Fan},
    {sString_NoharaDebug_BattleVsDad, NoharaDebugMenu_BattleVSDad},
    {sString_NoharaDebug_DadAfterBattle, NoharaDebugMenu_DadAfterBattle},
    {sString_NoharaDebug_SootopolisCity, NoharaDebugMenu_SootopolisCity},
    {sString_NoharaDebug_ResetMrBriney, NoharaDebugMenu_ResetMrBriney},
    {sString_NoharaDebug_Yes9999, NoharaDebugMenu_Yes9999},
    {sString_NoharaDebug_LegendsFlagOn, NoharaDebugMenu_LegendsFlagOn},
    {sString_NoharaDebug_AddWinningStreaks, NoharaDebugMenu_AddNumWinningStreaks}
};

bool8 InitNoharaDebugMenu(void)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 19, 19);
    Menu_PrintItems(1, 1, ARRAY_COUNT(sMenuActions_NoharaDebug), sMenuActions_NoharaDebug);
    InitMenu(0, 1, 1, ARRAY_COUNT(sMenuActions_NoharaDebug), 0, 18);
    gMenuCallback = NoharaDebugMenu_HandleInput;
    return FALSE;
}

bool8 NoharaDebugMenu_HandleInput(void)
{
    s8 input = Menu_ProcessInput();
    switch (input)
    {
        default:
            gMenuCallback = sMenuActions_NoharaDebug[input].func;
            return FALSE;
        case -2:
            return FALSE;
        case -1:
            CloseMenu();
            return TRUE;
    }
}

#if (ENGLISH && REVISION == 0)
const u8 sText_NoharaDebug_TV_SecretTypePrompt[] = _("ひみつがたを すぐみれるように    ひだりキーで\n"
                                       "データタイプを へんこうしますか？  きりかえ");
#else
const u8 sText_NoharaDebug_TV_SecretTypePrompt[] = _("Want to change data type{CLEAR_TO 143}Press　Left\n"
                                       "to see secret type now？{CLEAR_TO 143}to　switch");
#endif

const u8 sText_NoharaDebug_TV_CreateShowPrompt[] = _("Choose the TV data you wish to\n"
                                       "create to check a transmission。");

const u8 sText_NoharaDebug_TV_TransmissionFrequencyPrompt[] = DTR("ほうそうの　はっせいりつを　セット\n"
                                         "してください　？ぶんの1に　なります",
                                         "Please set the transmission frequency\n"
                                         "Part 1");

const u8 sString_NoharaDebug_TV_Survey[] = _("SURVEY");
const u8 sString_NoharaDebug_TV_RecentHappenings[] = _("RECENT HAPPENINGS");
const u8 sString_NoharaDebug_TV_PokemonFanClub[] = _("POKéMON FAN CLUB");
const u8 sString_NoharaDebug_TV_Gym[] = DTR("ジム", "GYM");
const u8 sString_NoharaDebug_TV_Outbreaks[] = _("OUTBREAKS");
const u8 sString_NoharaDebug_TV_NameRater[] = _("NAME RATER");
const u8 sString_NoharaDebug_TV_Contest[] = _("CONTEST");
const u8 sString_NoharaDebug_TV_Introduction[] = _("INTRODUCTION");
const u8 sString_NoharaDebug_TV_Shopping[] = _("SHOPPING");
const u8 sString_NoharaDebug_TV_Misfortune[] = _("MISFORTUNE");
const u8 sString_NoharaDebug_TV_Fishing[] = _("FISHING");
const u8 sString_NoharaDebug_TV_MemorableDay[] = _("MEMORABLE DAY");
const u8 sString_NoharaDebug_TV_BravoTrainer[] = _("BRAVO TRAINER");

const u8 sTVShowTypes_NoharaDebug[] = {
    TVSHOW_FAN_CLUB_LETTER,
    TVSHOW_RECENT_HAPPENINGS,
    TVSHOW_PKMN_FAN_CLUB_OPINIONS,
    TVSHOW_MASS_OUTBREAK,
    TVSHOW_NAME_RATER_SHOW,
    TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE,
    TVSHOW_POKEMON_TODAY_CAUGHT,
    TVSHOW_SMART_SHOPPER,
    TVSHOW_POKEMON_TODAY_FAILED,
    TVSHOW_FISHING_ADVICE,
    TVSHOW_WORLD_OF_MASTERS,
    TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE
};

const u8 *const sTVShowNames_NoharaDebug[] = {
    sString_NoharaDebug_TV_Survey,
    sString_NoharaDebug_TV_RecentHappenings,
    sString_NoharaDebug_TV_PokemonFanClub,
    sString_NoharaDebug_TV_Outbreaks,
    sString_NoharaDebug_TV_NameRater,
    sString_NoharaDebug_TV_Contest,
    sString_NoharaDebug_TV_Introduction,
    sString_NoharaDebug_TV_Shopping,
    sString_NoharaDebug_TV_Misfortune,
    sString_NoharaDebug_TV_Fishing,
    sString_NoharaDebug_TV_MemorableDay,
    sString_NoharaDebug_TV_BravoTrainer
};

const u8 gDebugText_BigSale[] = _("BIG SALE");

const u8 sString_NoharaDebug_TV_ServiceDay[] = _("SERVICE DAY");
const u8 sString_NoharaDebug_TV_ClearOutSale[] = _("CLEAR-OUT SALE");
const u8 sTVCommercialTypes_NoharaDebug[] = {1, 2, 3};

const u8 *const sTVCommercialNames_NoharaDebug[] = {
    gDebugText_BigSale,
    sString_NoharaDebug_TV_ServiceDay,
    sString_NoharaDebug_TV_ClearOutSale
};

const u8 sString_NoharaDebug_TV_SecretType[] = _("Secret type");
const u8 sString_NoharaDebug_TV_Start[] = _("Start");
const u8 sString_NoharaDebug_TV_CreateTV[] = _("Create TV");
const u8 sString_NoharaDebug_TV_AllClear[] = _("All clear");
const u8 sString_NoharaDebug_TV_AllSeen[] = _("All seen");
const u8 sString_NoharaDebug_TV_CreateCM[] = _("Create CM");
const u8 sString_NoharaDebug_TV_NotYetSeen[] = _("Not yet seen");
const u8 sString_NoharaDebug_TV_HitTable[] = _("Hit Table");
const u8 sString_NoharaDebug_TV_SetFull[] = _("Set full");

// Normaly these would be struct MenuAction, but the prototype of
// member .func is not consistent.
const struct {
    const u8 * text;
    void *func;
} sMenuActions_NoharaDebug_TV[] = {
    {sString_NoharaDebug_TV_SecretType, NoharaDebugMenu_TV_ShowSecretTypePrompt},
    {sString_NoharaDebug_TV_Start, NoharaDebugMenu_TV_EnableBroadcasts},
    {sString_NoharaDebug_TV_CreateTV, NoharaDebugMenu_TV_OpenCreateShowMenu},
    {sString_NoharaDebug_TV_AllClear, NoharaDebugMenu_TV_ClearShowData},
    {sString_NoharaDebug_TV_AllSeen, NoharaDebugMenu_TV_ClearAllActiveFlags},
    {sString_NoharaDebug_TV_CreateCM, NoharaDebugMenu_TV_OpenCreateCommercialMenu},
    {sString_NoharaDebug_TV_NotYetSeen, NoharaDebugMenu_TV_SetPopulatedShowsActive},
    {sString_NoharaDebug_TV_HitTable, NoharaDebugMenu_TV_ShowSlotMachineHitTable},
    {sString_NoharaDebug_TV_SetFull, NoharaDebugMenu_TV_FillEmptySlots}
};

bool8 NoharaDebugMenu_TV(void)
{
    sTVShowActiveStateDisplayed = 0;
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 10, 19);
    Menu_PrintItems(1, 1, ARRAY_COUNT(sMenuActions_NoharaDebug_TV), sMenuActions_NoharaDebug_TV);
    InitMenu(0, 1, 1, ARRAY_COUNT(sMenuActions_NoharaDebug_TV), 0, 9);
    gMenuCallback = NoharaDebugMenu_TV_HandleInput;
    return FALSE;
}

bool8 NoharaDebugMenu_TV_HandleInput(void)
{
    s8 input = Menu_ProcessInput();
    switch (input)
    {
        default:
            gMenuCallback = (MenuFunc)sMenuActions_NoharaDebug_TV[input].func;
            return FALSE;
        case -2:
            return FALSE;
        case -1:
            CloseMenu();
            return TRUE;
    }
}

void NoharaDebugMenu_TV_ShowSecretTypePrompt(void)
{
    NoharaDebugMenu_TV_PrintDataKinds();
    sub_8071F40(sText_NoharaDebug_TV_SecretTypePrompt);
    DisplayYesNoMenu(3, 3, 1);
    gMenuCallback = NoharaDebugMenu_TV_HandleSecretTypePrompt;
}

void NoharaDebugMenu_TV_ActivateSelectedInactiveShows(void)
{
    u8 i;

    for (i = 0; i < 24; i++)
    {
        if (gSaveBlock1.tvShows[i].common.kind >= TVSHOW_POKEMON_TODAY_CAUGHT &&
            gSaveBlock1.tvShows[i].common.kind < TVSHOW_MASS_OUTBREAK &&
            !gSaveBlock1.tvShows[i].common.active)
            gSaveBlock1.tvShows[i].common.active = TRUE;
    }
}

bool8 NoharaDebugMenu_TV_ClearAllActiveFlags(void)
{
    u8 i;

    for (i = 0; i < 24; i++)
        gSaveBlock1.tvShows[i].common.active = FALSE;
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_TV_SetPopulatedShowsActive(void)
{
    u8 i;

    for (i = 0; i < 24; i++)
    {
        if (gSaveBlock1.tvShows[i].common.kind != 0)
            gSaveBlock1.tvShows[i].common.active = TRUE;
    }
    CloseMenu();
    return TRUE;
}

const u8 sSlotMachineHitTableCoords[][12] = {
    {0x0C, 0x04},
    {0x0C, 0x08},
    {0x0C, 0x0A},
    {0x0E, 0x06},
    {0x0E, 0x08},
    {0x0E, 0x0A},
    {0x10, 0x04},
    {0x10, 0x06},
    {0x10, 0x0A},
    {0x12, 0x04},
    {0x12, 0x06},
    {0x12, 0x0A}
};

bool8 NoharaDebugMenu_TV_ShowSlotMachineHitTable(void)
{
    u8 i;

    for (i = 0; i < 12; i++)
    {
        gSpecialVar_0x8004 = i;
        ConvertIntToDecimalStringN(gStringVar1, GetSlotMachineId(), STR_CONV_MODE_LEFT_ALIGN, 1);
        Menu_PrintText(gStringVar1, sSlotMachineHitTableCoords[i][0], sSlotMachineHitTableCoords[i][1]);
    }
    gSpecialVar_0x8004 = 0;
    gMenuCallback = NoharaDebugMenu_WaitForAButton;
    return FALSE;
}

bool8 NoharaDebugMenu_TV_HandleSecretTypePrompt(void)
{
    if (JOY_NEW(DPAD_LEFT))
    {
        Menu_EraseWindowRect(10, 0, 29, 13);
        sTVShowActiveStateDisplayed ^= 1;
        if (sTVShowActiveStateDisplayed)
            NoharaDebugMenu_TV_PrintDataStates();
        else
            NoharaDebugMenu_TV_PrintDataKinds();
        return FALSE;
    }
    else
    {
        s8 input = Menu_ProcessInputNoWrap_();
        switch (input)
        {
            case -2:
                return FALSE;
            case 0:
                NoharaDebugMenu_TV_ActivateSelectedInactiveShows();
                // fallthrough
            case -1:
            default:
                CloseMenu();
                return TRUE;
        }
    }
}

void NoharaDebugMenu_TV_PrintDataKinds(void)
{
    u8 i;

    for (i = 0; i < 5; i++)
    {
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock1.tvShows[i].common.kind, STR_CONV_MODE_LEFT_ALIGN, 2);
        Menu_PrintText(gStringVar1, i * 2 + 10, 0);
    }

    for (i = 5; i < 24; i++)
    {
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock1.tvShows[i].common.kind, STR_CONV_MODE_LEFT_ALIGN, 2);
        if (i < 15)
            Menu_PrintText(gStringVar1, i * 2, 3);
        else
            Menu_PrintText(gStringVar1, i * 2 - 20, 6);
    }

    for (i = 0; i < 16; i++)
    {
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock1.pokeNews[i].kind, STR_CONV_MODE_LEFT_ALIGN, 2);
        if (i < 8)
            Menu_PrintText(gStringVar1, i * 2 + 10, 9);
        else
            Menu_PrintText(gStringVar1, i * 2 - 6, 12);
    }
}

void NoharaDebugMenu_TV_PrintDataStates(void)
{
    u8 i;

    for (i = 0; i < 5; i++)
    {
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock1.tvShows[i].common.active, STR_CONV_MODE_LEFT_ALIGN, 2);
        Menu_PrintText(gStringVar1, i * 2 + 10, 0);
    }

    for (i = 5; i < 24; i++)
    {
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock1.tvShows[i].common.active, STR_CONV_MODE_LEFT_ALIGN, 2);
        if (i < 15)
            Menu_PrintText(gStringVar1, i * 2, 3);
        else
            Menu_PrintText(gStringVar1, i * 2 - 20, 6);
    }

    for (i = 0; i < 16; i++)
    {
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock1.pokeNews[i].state, STR_CONV_MODE_LEFT_ALIGN, 2);
        if (i < 8)
            Menu_PrintText(gStringVar1, i * 2 + 10, 9);
        else
            Menu_PrintText(gStringVar1, i * 2 - 6, 12);
    }
}

bool8 NoharaDebugMenu_TV_EnableBroadcasts(void)
{
    FlagSet(FLAG_SYS_TV_START);
    FlagSet(FLAG_VISITED_MAUVILLE_CITY);
    CloseMenu();
    return TRUE;
}

void NoharaDebugMenu_TV_OpenCreateShowMenu(void)
{
    sTVDataTypeIndex = 0;
    sub_8071F40(sText_NoharaDebug_TV_CreateShowPrompt);
    Menu_BlankWindowRect(13, 6, 26, 8);
    Menu_PrintText(sTVShowNames_NoharaDebug[0], 14, 7);
    Menu_BlankWindowRect(22, 1, 24, 2);
    ConvertIntToDecimalStringN(gStringVar1, 0, STR_CONV_MODE_LEFT_ALIGN, 2);
    Menu_PrintText(gStringVar1, 23, 1);
    gMenuCallback = NoharaDebugMenu_TV_HandleCreateShowInput;
}

bool8 NoharaDebugMenu_TV_HandleCreateShowInput(void)
{
    bool8 updateDisplay = FALSE;
    if (JOY_NEW(DPAD_UP))
    {
        sTVDataSlotIndex++;
        if (sTVDataSlotIndex == 24)
            sTVDataSlotIndex = 0;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        if (sTVDataSlotIndex == 0)
            sTVDataSlotIndex = 24;
        sTVDataSlotIndex--;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        sTVDataTypeIndex++;
        if (sTVDataTypeIndex == 12)
            sTVDataTypeIndex = 0;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (sTVDataTypeIndex == 0)
            sTVDataTypeIndex = 12;
        sTVDataTypeIndex--;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }
    if (updateDisplay)
    {
        Menu_BlankWindowRect(13, 6, 26, 8);
        Menu_PrintText(sTVShowNames_NoharaDebug[sTVDataTypeIndex], 14, 7);
        Menu_BlankWindowRect(22, 1, 24, 2);
        ConvertIntToDecimalStringN(gStringVar1, sTVDataSlotIndex, STR_CONV_MODE_LEFT_ALIGN, 2);
        Menu_PrintText(gStringVar1, 23, 1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_PIN);
        NoharaDebugMenu_TV_CreateShow(sTVDataSlotIndex, sTVShowTypes_NoharaDebug[sTVDataTypeIndex]);
    }
    if (JOY_NEW(B_BUTTON | START_BUTTON))
    {
        sub_80BF588(gSaveBlock1.tvShows);
        CloseMenu();
        return TRUE;
    }
    return FALSE;
}

void NoharaDebugMenu_TV_CreateShow(u8 a0, u8 a1)
{
    u8 i;
    u8 leadMonIndex = GetLeadMonIndex();
    u8 group;

// This is garbage.
#define GF_ACCESS(x) ((struct x*)(&(gSaveBlock1.tvShows[a0])))
#define TERU_ACCESS(x) show->x
#define DECLARE_TERU_POINTER TVShow * show = gSaveBlock1.tvShows + a0

    gSaveBlock1.tvShows[a0].common.kind = a1;
    gSaveBlock1.tvShows[a0].common.active = TRUE;
    for (i = 0; i < 0x22; i++)
        gSaveBlock1.tvShows[a0].common.pad02[i] = 1;

    group = GetTVGroupByShowId(a1);
    switch (group)
    {
        case 2:
        case 4:
            sub_80BE160(&gSaveBlock1.tvShows[a0]);
            break;
        case 3:
            sub_80BE138(&gSaveBlock1.tvShows[a0]);
            break;
    }

#if (ENGLISH && REVISION == 0)
    switch (a1)
    {
        case TVSHOW_FAN_CLUB_LETTER:
        case TVSHOW_RECENT_HAPPENINGS:
        {
            GF_ACCESS(TVShowFanClubLetter)->species = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowFanClubLetter)->playerName, gSaveBlock2.playerName);
            GF_ACCESS(TVShowFanClubLetter)->language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
        {
            GF_ACCESS(TVShowFanclubOpinions)->var02 = 1;
            StringCopy(GF_ACCESS(TVShowFanclubOpinions)->playerName, gSaveBlock2.playerName);
            GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_NICKNAME, GF_ACCESS(TVShowFanclubOpinions)->var10);
            GF_ACCESS(TVShowFanclubOpinions)->language = GAME_LANGUAGE;
            GF_ACCESS(TVShowFanclubOpinions)->var0E = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_LANGUAGE);
            break;
        }
        case TVSHOW_UNKN_SHOWTYPE_04:
        {
            break;
        }
        case TVSHOW_NAME_RATER_SHOW:
        {
            GF_ACCESS(TVShowNameRaterShow)->species = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_SPECIES);
            GF_ACCESS(TVShowNameRaterShow)->var1C = 1;
            StringCopy(GF_ACCESS(TVShowNameRaterShow)->trainerName, gSaveBlock2.playerName);
            GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_NICKNAME, GF_ACCESS(TVShowNameRaterShow)->pokemonName);
            GF_ACCESS(TVShowNameRaterShow)->language = GAME_LANGUAGE;
            GF_ACCESS(TVShowNameRaterShow)->pokemonNameLanguage = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_LANGUAGE);
            break;
        }
        case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
        {
            GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->species = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->playerName, gSaveBlock2.playerName);
            GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_NICKNAME, GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->pokemonNickname);
            GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->language = GAME_LANGUAGE;
            GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->var1f = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_LANGUAGE);
            break;
        }
        case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
        {
            GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->species = SPECIES_BULBASAUR;
            GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->defeatedSpecies = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->trainerName, gSaveBlock2.playerName);
            StringCopy(GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->enemyTrainerName, gSaveBlock2.playerName);
            GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_MASS_OUTBREAK:
        {
            GF_ACCESS(TVShowMassOutbreak)->species = SPECIES_BULBASAUR;
            GF_ACCESS(TVShowMassOutbreak)->daysLeft = 1;
            break;
        }
        case TVSHOW_POKEMON_TODAY_CAUGHT:
        {
            GF_ACCESS(TVShowPokemonToday)->species = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowPokemonToday)->playerName, gSaveBlock2.playerName);
            GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_NICKNAME, GF_ACCESS(TVShowPokemonToday)->nickname);
            GF_ACCESS(TVShowPokemonToday)->language = GAME_LANGUAGE;
            GF_ACCESS(TVShowPokemonToday)->language2 = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_LANGUAGE);
            break;
        }
        case TVSHOW_SMART_SHOPPER:
        {
            StringCopy(GF_ACCESS(TVShowSmartShopper)->playerName, gSaveBlock2.playerName);
            GF_ACCESS(TVShowSmartShopper)->language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_POKEMON_TODAY_FAILED:
        {
            GF_ACCESS(TVShowPokemonTodayFailed)->species = SPECIES_BULBASAUR;
            GF_ACCESS(TVShowPokemonTodayFailed)->species2 = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowPokemonTodayFailed)->playerName, gSaveBlock2.playerName);
            GF_ACCESS(TVShowPokemonTodayFailed)->language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_FISHING_ADVICE:
        {
            GF_ACCESS(TVShowPokemonAngler)->var04 = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowPokemonAngler)->playerName, gSaveBlock2.playerName);
            GF_ACCESS(TVShowPokemonAngler)->language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_WORLD_OF_MASTERS:
        {
            GF_ACCESS(TVShowWorldOfMasters)->var04 = SPECIES_BULBASAUR;
            GF_ACCESS(TVShowWorldOfMasters)->var08 = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowWorldOfMasters)->playerName, gSaveBlock2.playerName);
            GF_ACCESS(TVShowWorldOfMasters)->language = GAME_LANGUAGE;
            break;
        }
    }
#else
// Murakawa must have really hated working with GF code. He devised his own, less complicated
// access method after US rev0. Also, this iteration of the code has his self inserts: TERUKUN,
// TERU, and TERUDA. Who all love Wigglytuff.
    switch (a1)
    {
        case TVSHOW_FAN_CLUB_LETTER:
        case TVSHOW_RECENT_HAPPENINGS:
        {
            DECLARE_TERU_POINTER;
            
            GF_ACCESS(TVShowFanClubLetter)->species = SPECIES_BULBASAUR; // Yet he didn't change Game Freak's lines.
            StringCopy(GF_ACCESS(TVShowFanClubLetter)->playerName, gSaveBlock2.playerName);
            TERU_ACCESS(fanclubLetter).language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
        {
            DECLARE_TERU_POINTER;
            
            GF_ACCESS(TVShowFanclubOpinions)->var02 = 1;
            StringCopy(GF_ACCESS(TVShowFanclubOpinions)->playerName, gSaveBlock2.playerName);
            GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_NICKNAME, GF_ACCESS(TVShowFanclubOpinions)->var10);
            TERU_ACCESS(fanclubOpinions).language = GAME_LANGUAGE;
            TERU_ACCESS(fanclubOpinions).var0E = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_LANGUAGE);
            break;
        }
        case TVSHOW_UNKN_SHOWTYPE_04:
        {
            break;
        }
        case TVSHOW_NAME_RATER_SHOW:
        {
            u16 species = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_SPECIES);
            DECLARE_TERU_POINTER;
            
            GF_ACCESS(TVShowNameRaterShow)->species = species;
            GF_ACCESS(TVShowNameRaterShow)->var1C = 1;
            StringCopy(GF_ACCESS(TVShowNameRaterShow)->trainerName, gSaveBlock2.playerName);
            GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_NICKNAME, GF_ACCESS(TVShowNameRaterShow)->pokemonName);
            TERU_ACCESS(nameRaterShow).language = GAME_LANGUAGE;
            TERU_ACCESS(nameRaterShow).pokemonNameLanguage = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_LANGUAGE);
            break;
        }
        case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
        {
            DECLARE_TERU_POINTER;
            
            GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->species = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->playerName, gSaveBlock2.playerName);
            GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_NICKNAME, GF_ACCESS(TVShowBravoTrainerPokemonProfiles)->pokemonNickname);
            TERU_ACCESS(bravoTrainer).language = GAME_LANGUAGE;
            TERU_ACCESS(bravoTrainer).var1f = GetMonData(&gPlayerParty[leadMonIndex], MON_DATA_LANGUAGE);
            break;
        }
        case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
        {
            DECLARE_TERU_POINTER;
            
            GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->species = SPECIES_BULBASAUR;
            GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->defeatedSpecies = SPECIES_BULBASAUR;
            StringCopy(GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->trainerName, gSaveBlock2.playerName);
            StringCopy(GF_ACCESS(TVShowBravoTrainerBattleTowerSpotlight)->enemyTrainerName, gSaveBlock2.playerName);
            TERU_ACCESS(bravoTrainerTower).language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_MASS_OUTBREAK:
        {
            GF_ACCESS(TVShowMassOutbreak)->species = SPECIES_BULBASAUR;
            GF_ACCESS(TVShowMassOutbreak)->daysLeft = 1;
            break;
        }
        case TVSHOW_POKEMON_TODAY_CAUGHT:
        {
            DECLARE_TERU_POINTER;
            u8 playerNameTerukun[] = _("TERUKUN");
            u8 nicknameTeruteruda[] = _("TERUTERUDA");

            TERU_ACCESS(pokemonToday).var12 = 255;
            StringCopy(TERU_ACCESS(pokemonToday).playerName, playerNameTerukun);
            StringCopy(TERU_ACCESS(pokemonToday).nickname, nicknameTeruteruda);
            TERU_ACCESS(pokemonToday).ball = ITEM_PREMIER_BALL;
            TERU_ACCESS(pokemonToday).species = SPECIES_WIGGLYTUFF;
            TERU_ACCESS(pokemonToday).language = GAME_LANGUAGE;
            TERU_ACCESS(pokemonToday).language2 = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_SMART_SHOPPER:
        {
            DECLARE_TERU_POINTER;
            u8 playerNameTerukun[] = _("TERUKUN");
            int ii;

            for (ii = 0; ii < 3; ii++)
                TERU_ACCESS(smartshopperShow).itemAmounts[ii] = 254;
            TERU_ACCESS(smartshopperShow).priceReduced = TRUE;
            TERU_ACCESS(smartshopperShow).shopLocation = 40;
            for (ii = 0; ii < 3; ii++)
                TERU_ACCESS(smartshopperShow).itemIds[ii] = ITEM_ENERGY_POWDER;
            StringCopy(TERU_ACCESS(smartshopperShow).playerName, playerNameTerukun);
            TERU_ACCESS(smartshopperShow).language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_POKEMON_TODAY_FAILED:
        {
            DECLARE_TERU_POINTER;
            u8 playerNameTerukun[] = _("TERUKUN");

            TERU_ACCESS(pokemonTodayFailed).species = SPECIES_WIGGLYTUFF;
            TERU_ACCESS(pokemonTodayFailed).species2 = SPECIES_WIGGLYTUFF;
            TERU_ACCESS(pokemonTodayFailed).var12 = 3;
            TERU_ACCESS(pokemonTodayFailed).var10 = 0xff;
            TERU_ACCESS(pokemonTodayFailed).var11 = 1;
            StringCopy(TERU_ACCESS(pokemonTodayFailed).playerName, playerNameTerukun);
            TERU_ACCESS(pokemonTodayFailed).language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_FISHING_ADVICE:
        {
            DECLARE_TERU_POINTER;
            u8 playerNameTerukun[] = _("TERUKUN");

            TERU_ACCESS(pokemonAngler).var02 = 0xff;
            TERU_ACCESS(pokemonAngler).var03 = 0;
            TERU_ACCESS(pokemonAngler).var04 = 40;
            StringCopy(TERU_ACCESS(pokemonAngler).playerName, playerNameTerukun);
            TERU_ACCESS(pokemonAngler).language = GAME_LANGUAGE;
            break;
        }
        case TVSHOW_WORLD_OF_MASTERS:
        {
            DECLARE_TERU_POINTER;
            u8 playerNameTerukun[] = _("TERUKUN");

            TERU_ACCESS(worldOfMasters).var02 = 0xffff;
            TERU_ACCESS(worldOfMasters).var06 = 0xffff;
            TERU_ACCESS(worldOfMasters).var04 = 40;
            TERU_ACCESS(worldOfMasters).var08 = 40;
            TERU_ACCESS(worldOfMasters).var0a = 3;
            StringCopy(TERU_ACCESS(worldOfMasters).playerName, playerNameTerukun);
            TERU_ACCESS(worldOfMasters).language = GAME_LANGUAGE;
            break;
        }
    }
#endif
}

bool8 NoharaDebugMenu_TV_ClearShowData(void)
{
    ClearTVShowData();
    CloseMenu();
    return TRUE;
}

void NoharaDebugMenu_TV_OpenCreateCommercialMenu(void)
{
    sTVDataTypeIndex = 0;
    sub_8071F40(sText_NoharaDebug_TV_CreateShowPrompt);
    Menu_BlankWindowRect(13, 6, 23, 8);
    Menu_PrintText(sTVCommercialNames_NoharaDebug[0], 14, 7);
    Menu_BlankWindowRect(22, 1, 24, 2);
    ConvertIntToDecimalStringN(gStringVar1, 0, STR_CONV_MODE_LEFT_ALIGN, 2);
    Menu_PrintText(gStringVar1, 23, 1);
    gMenuCallback = NoharaDebugMenu_TV_HandleCreateCommercialInput;
}

bool8 NoharaDebugMenu_TV_HandleCreateCommercialInput(void)
{
    bool8 updateDisplay = FALSE;

    if (JOY_NEW(DPAD_UP))
    {
        sTVDataSlotIndex++;
        if (sTVDataSlotIndex == 16)
            sTVDataSlotIndex = 0;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }

    if (JOY_NEW(DPAD_DOWN))
    {
        if (sTVDataSlotIndex == 0)
            sTVDataSlotIndex = 16;
        sTVDataSlotIndex--;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }

    if (JOY_NEW(DPAD_RIGHT))
    {
        sTVDataTypeIndex++;
        if (sTVDataTypeIndex == 3)
            sTVDataTypeIndex = 0;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }

    if (JOY_NEW(DPAD_LEFT))
    {
        if (sTVDataTypeIndex == 0)
            sTVDataTypeIndex = 3;
        sTVDataTypeIndex--;
        PlaySE(SE_SELECT);
        updateDisplay = TRUE;
    }

    if (updateDisplay)
    {
        Menu_BlankWindowRect(13, 6, 23, 8);
        Menu_PrintText(sTVCommercialNames_NoharaDebug[sTVDataTypeIndex], 14, 7);
        Menu_BlankWindowRect(22, 1, 24, 2);
        ConvertIntToDecimalStringN(gStringVar1, sTVDataSlotIndex, STR_CONV_MODE_LEFT_ALIGN, 2);
        Menu_PrintText(gStringVar1, 23, 1);
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_PIN);
        NoharaDebugMenu_TV_CreateCommercial(sTVDataSlotIndex, sTVCommercialTypes_NoharaDebug[sTVDataTypeIndex]);
    }

    if (JOY_NEW(B_BUTTON | START_BUTTON))
    {
        sub_80BEC40();
        CloseMenu();
        return TRUE;
    }

    return FALSE;
}

void NoharaDebugMenu_TV_CreateCommercial(u8 a0, u8 a1)
{
    gSaveBlock1.pokeNews[a0].kind = a1;
    gSaveBlock1.pokeNews[a0].state = 1;
    gSaveBlock1.pokeNews[a0].days = 4;
}

bool8 NoharaDebugMenu_TV_FillEmptySlots(void)
{
    u8 i;
    u8 j;

    j = 0;
    for (i = 0; i < 24; i++)
    {
        if (gSaveBlock1.tvShows[i].common.kind == 0)
        {
            if (j == 12)
                j = 0;
            NoharaDebugMenu_TV_CreateShow(i, sTVShowTypes_NoharaDebug[j]);
            gSaveBlock1.tvShows[i].common.active = FALSE;
            j++;
        }
    }

    j = 0;
    for (i = 0; i < 16; i++)
    {
        if (gSaveBlock1.pokeNews[i].kind == 0)
        {
            if (j == 3)
                j = 0;
            NoharaDebugMenu_TV_CreateCommercial(i, sTVCommercialTypes_NoharaDebug[j]);
            j++;
        }
    }

    CloseMenu();
    return TRUE;
}
// These represent the people in Lilycove Fan Club.
// TRN: These translations are probably wrong but it is really hard to tell with
// the abbreviations. They are mosly based on observation
// The first one is selected by default when there are no fans.
const u8 sString_NoharaDebug_Fan_Member1[] = DTR("1　スクル", "1 LASS/NONE"); // school (girl)
const u8 sString_NoharaDebug_Fan_Member2[] = DTR("2　ミドル", "2 MIDDLE AGE MAN"); // middle
const u8 sString_NoharaDebug_Fan_Member3[] = DTR("3　オジヨ", "3 DAUGHTER"); // おしょう (daughter)
const u8 sString_NoharaDebug_Fan_Member4[] = DTR("4　ボーヤ", "4 YOUNG BOY"); // 坊や (boy)
const u8 sString_NoharaDebug_Fan_Member5[] = DTR("5　ボーイ", "5 BOY"); // boy
const u8 sString_NoharaDebug_Fan_Member6[] = DTR("6　ヤング", "6 YOUNG MAN"); // young
const u8 sString_NoharaDebug_Fan_Member7[] = DTR("7　ヲーカ", "7 MOM"); // お母さん? (probably typo)
const u8 sString_NoharaDebug_Fan_Member8[] = DTR("8　オルド", "8 OLD LADY"); // old

const u8 *const sFanMemberNames_NoharaDebug[] = {
    sString_NoharaDebug_Fan_Member1,
    sString_NoharaDebug_Fan_Member2,
    sString_NoharaDebug_Fan_Member3,
    sString_NoharaDebug_Fan_Member4,
    sString_NoharaDebug_Fan_Member5,
    sString_NoharaDebug_Fan_Member6,
    sString_NoharaDebug_Fan_Member7,
    sString_NoharaDebug_Fan_Member8
};

const u8 sString_NoharaDebug_Fan_Start[] = _("Start"); // Starts the "Oh! I've heard of you' script"
const u8 sString_NoharaDebug_Fan_Increase[] = _("Increase"); // Increases popularity
const u8 sString_NoharaDebug_Fan_Reduce[] = _("Reduce"); // reduces popularity
const u8 sString_NoharaDebug_Fan_Points[] = _("Points");
const u8 sString_NoharaDebug_Fan_AddSixHours[] = _("Play time 6");
const u8 sString_NoharaDebug_Fan_EliteFour[] = _("P ELITE FOUR");
const u8 sString_NoharaDebug_Fan_SecretBase[] = _("P SECRET BASE");
const u8 sString_NoharaDebug_Fan_Contest[] = _("P CONTEST");
const u8 sString_NoharaDebug_Fan_BattleTower[] = _("P BATTLE TOWER");

const struct MenuAction sMenuActions_NoharaDebug_Fan[] = {
    {sString_NoharaDebug_Fan_Start, NoharaDebugMenu_Fan_Start},
    {sString_NoharaDebug_Fan_Increase, NoharaDebugMenu_Fan_GainRandomFan},
    {sString_NoharaDebug_Fan_Reduce, NoharaDebugMenu_Fan_LoseRandomFan},
    {sString_NoharaDebug_Fan_Points, NoharaDebugMenu_Fan_ShowPoints},
    {sString_NoharaDebug_Fan_AddSixHours, NoharaDebugMenu_Fan_AddSixHours},
    {sString_NoharaDebug_Fan_EliteFour, NoharaDebugMenu_Fan_GainAfterEliteFour},
    {sString_NoharaDebug_Fan_SecretBase, NoharaDebugMenu_Fan_GainAfterSecretBase},
    {sString_NoharaDebug_Fan_Contest, NoharaDebugMenu_Fan_GainAfterContest},
    {sString_NoharaDebug_Fan_BattleTower, NoharaDebugMenu_Fan_GainAfterBattleTower}
};

bool8 NoharaDebugMenu_Fan(void)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 11, 19);
    Menu_PrintItems(1, 1, ARRAY_COUNT(sMenuActions_NoharaDebug_Fan), sMenuActions_NoharaDebug_Fan);
    InitMenu(0, 1, 1, ARRAY_COUNT(sMenuActions_NoharaDebug_Fan), 0, 10);
    gMenuCallback = NoharaDebugMenu_Fan_HandleInput;
    return FALSE;
}

bool8 NoharaDebugMenu_Fan_HandleInput(void)
{
    s8 input = Menu_ProcessInput();
    switch (input)
    {
        default:
            gMenuCallback = sMenuActions_NoharaDebug_Fan[input].func;
            return FALSE;
        case -2:
            return FALSE;
        case -1:
            CloseMenu();
            return TRUE;
    }
}

bool8 NoharaDebugMenu_Fan_Start(void)
{
    ResetFanClub();
    UpdateTrainerFanClubGameClear();
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_Fan_GainRandomFan(void)
{
    u8 fanIndex = PlayerGainRandomTrainerFan();
    Menu_PrintText(sFanMemberNames_NoharaDebug[gFanClubMemberIdsForGainingFans[fanIndex] - 8], 14, 7);
    gMenuCallback = NoharaDebugMenu_WaitForAButton;
    return FALSE;
}

bool8 NoharaDebugMenu_Fan_LoseRandomFan(void)
{
    u8 fanIndex = PlayerLoseRandomTrainerFan();
    Menu_PrintText(sFanMemberNames_NoharaDebug[gFanClubMemberIdsForLosingFans[fanIndex] - 8], 14, 7);
    gMenuCallback = NoharaDebugMenu_WaitForAButton;
    return FALSE;
}

bool8 NoharaDebugMenu_WaitForAButton(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        CloseMenu();
        return TRUE;
    }

    return FALSE;
}

bool8 NoharaDebugMenu_Fan_ShowPoints(void)
{
    ConvertIntToDecimalStringN(gStringVar1, gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] & 0x7F, STR_CONV_MODE_LEFT_ALIGN, 2);
    Menu_PrintText(gStringVar1, 16, 7);
    gMenuCallback = NoharaDebugMenu_WaitForAButton;
    return FALSE;
}

bool8 NoharaDebugMenu_Fan_AddSixHours(void)
{
    gSaveBlock2.playTimeHours += 6;
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_Fan_GainAfterEliteFour(void)
{
    TryGainNewFanFromCounter(0);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_Fan_GainAfterSecretBase(void)
{
    TryGainNewFanFromCounter(1);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_Fan_GainAfterContest(void)
{
    TryGainNewFanFromCounter(2);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_Fan_GainAfterBattleTower(void)
{
    TryGainNewFanFromCounter(3);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_BattleVSDad(void)
{
    VarSet(VAR_PETALBURG_GYM_STATE, 6);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_DadAfterBattle(void)
{
    VarSet(VAR_PETALBURG_GYM_STATE, 7);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_SootopolisCity(void)
{
    FlagSet(FLAG_LEGEND_ESCAPED_SEAFLOOR_CAVERN);
    FlagSet(FLAG_LEGENDARY_BATTLE_COMPLETED);
    FlagClear(FLAG_HIDE_WALLACE_SOOTOPOLIS_GYM);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_ResetMrBriney(void)
{
    FlagClear(FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY);
    VarSet(VAR_BRINEY_HOUSE_STATE, 1);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_Yes9999(void)
{
    VarSet(VAR_ASH_GATHER_COUNT, 9999);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_LegendsFlagOn(void)
{
    FlagSet(FLAG_REGI_DOORS_OPENED);
    CloseMenu();
    return TRUE;
}

bool8 NoharaDebugMenu_AddNumWinningStreaks(void)
{
    if (gSaveBlock2.battleTower.bestBattleTowerWinStreak < 50)
        gSaveBlock2.battleTower.bestBattleTowerWinStreak = 50;
    else if (gSaveBlock2.battleTower.bestBattleTowerWinStreak < 100)
        gSaveBlock2.battleTower.bestBattleTowerWinStreak = 100;
    else if (gSaveBlock2.battleTower.bestBattleTowerWinStreak < 1000)
        gSaveBlock2.battleTower.bestBattleTowerWinStreak = 1000;
    else if (gSaveBlock2.battleTower.bestBattleTowerWinStreak < 5000)
        gSaveBlock2.battleTower.bestBattleTowerWinStreak = 9990;
    else if (gSaveBlock2.battleTower.bestBattleTowerWinStreak < 9990)
        gSaveBlock2.battleTower.bestBattleTowerWinStreak = 9999;
    CloseMenu();
    return TRUE;
}

#endif
