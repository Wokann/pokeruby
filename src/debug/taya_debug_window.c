#if DEBUG
#include "global.h"
#include "constants/easy_chat.h"
#include "constants/species.h"
#include "constants/opponents.h"
#include "random.h"
#include "palette.h"
#include "ewram.h"
#include "menu.h"
#include "easy_chat.h"
#include "event_data.h"
#include "string_util.h"
#include "main.h"
#include "start_menu.h"
#include "field_weather.h"
#include "mauville_old_man.h"
#include "pokemon_storage_system.h"
#include "lottery_corner.h"
#include "battle_setup.h"
#include "region_map.h"
#include "overworld.h"

bool8 debug_sub_8090808(void);
bool8 TayaDebugMenu_Weather(void);
bool8 TayaDebugMenu_LanettesPC(void);
bool8 TayaDebugMenu_SimpleText(void);
bool8 TayaDebugMenu_OldMan(void);
bool8 TayaDebugMenu_Trend(void);
bool8 TayaDebugMenu_TrendR(void);
bool8 TayaDebugMenu_TownFlags(void);
bool8 TayaDebugMenu_AwardARibbon(void);
bool8 TayaDebugMenu_PKMNLottery(void);
bool8 TayaDebugMenu_Trainer(void);
bool8 TayaDebugMenu_PokenavD(void);
void TayaDebugRibbon_Init(void);
void TayaDebugRibbon_PrintOptions(void);

EWRAM_DATA s8 sTayaTopMenuPage = 0;

struct TayaLuckyNumberEwramStruct {
    u8 digit;
    u8 charbuf[7];
    u16 curLuckyId;
    s32 tempLuckyId;
    u16 digitDeltaMagnitude;
};

struct TayaMonDataEwramStruct {
    u8 data[6][3];
    u8 charbuf[0x40];
    u8 state;
    u8 x;
    u8 y;
    u8 redraw;
    u8 maxVal;
};

#define eTayaLuckyNumber (*(struct TayaLuckyNumberEwramStruct *)gSharedMem)
#define eTayaMonData (*(struct TayaMonDataEwramStruct *)gSharedMem)

const u8 sText_TayaEasyChat_Profile[] = _("Profile");
const u8 sText_TayaEasyChat_BattleStart[] = _("Battle's　start");
const u8 sText_TayaEasyChat_GoodSaying[] = _("Good　saying");
const u8 sText_TayaEasyChat_BattleLoss[] = _("Lose　a　battle");
const u8 sText_TayaEasyChat_Mail[] = _("MAIL");
const u8 sText_TayaEasyChat_MailSalutation[] = _("MAIL　salutation");
const u8 sText_TayaEasyChat_BardsSong[] = _("BARD's　song");
const u8 sText_TayaEasyChat_Interview[] = _("Interview");
const u8 sText_TayaEasyChat_BattleTowerInterview[] = _("Interview(BT)");

const struct {
    const u8 * text;
    u32 round1Points;
} sEasyChatOptions_TayaDebug[] = {
    {sText_TayaEasyChat_Profile, 0x0},
    {sText_TayaEasyChat_BattleStart, 0x1},
    {sText_TayaEasyChat_GoodSaying, 0xD},
    {sText_TayaEasyChat_BattleLoss, 0x3},
    {sText_TayaEasyChat_Mail, 0x4},
    {sText_TayaEasyChat_MailSalutation, 0x5},
    {sText_TayaEasyChat_BardsSong, 0x6},
    {sText_TayaEasyChat_Interview, 0x7},
    {sText_TayaEasyChat_BattleTowerInterview, 0xC}
};

const u16 sBardTestLyrics[] = {0x45b, 0x430, 0x20f};

const u8 sText_TayaOldMan_Bard[] = _("BARD");
const u8 sText_TayaOldMan_Hipster[] = _("HIPSTER");
const u8 sText_TayaOldMan_Trader[] = _("RECYCLE");
const u8 sText_TayaOldMan_Storyteller[] = _("STORYTELLER");
const u8 sText_TayaOldMan_GiddyGuy[] = _("GIDDY GUY");
const u8 sText_TayaOldMan_FlagOff[] = _("Flag OFF");
const u8 sText_TayaOldMan_21Letters[] = _("21 letters");
const u8 sText_TayaOldMan_BecomeHip[] = DTR("ナウくなる", "BECOME HIP");

const struct {
    const u8 * text;
    u8 filler[4];
} sOldManOptions_TayaDebug[] = {
    {sText_TayaOldMan_Bard},
    {sText_TayaOldMan_Hipster},
    {sText_TayaOldMan_Trader},
    {sText_TayaOldMan_Storyteller},
    {sText_TayaOldMan_GiddyGuy},
    {sText_TayaOldMan_FlagOff},
    {sText_TayaOldMan_21Letters},
    {sText_TayaOldMan_BecomeHip}
};

const u8 gUnknown_Debug_083C4F94[] = DTR("しょうしょうおまちください！", "Please wait!");
const u8 sText_TayaDebug_Weather[] = _("Weather");
const u8 sText_TayaDebug_LanettesPC[] = _("LANETTE'S PC");
const u8 sText_TayaDebug_SimpleText[] = _("SimpleText");
const u8 sText_TayaDebug_OldMan[] = _("Old　man");
const u8 sText_TayaDebug_Trend[] = _("Trend");
const u8 sText_TayaDebug_TrendR[] = _("Trend R");
const u8 sText_TayaDebug_TownFlags[] = _("Town flag");
const u8 sText_TayaDebug_AwardRibbon[] = _("Award a ribbon");
const u8 sText_TayaDebug_PokemonLottery[] = _("{PKMN}LOTTERY");
const u8 sText_TayaDebug_Trainer[] = _("Trainer");
const u8 sText_TayaDebug_PokenavD[] = _("POKéNAV D");

const struct MenuAction sMenuActions_TayaDebug_Page1[] = {
    {sText_TayaDebug_Weather, TayaDebugMenu_Weather},
    {sText_TayaDebug_LanettesPC, TayaDebugMenu_LanettesPC},
    {sText_TayaDebug_SimpleText, TayaDebugMenu_SimpleText},
    {sText_TayaDebug_OldMan, TayaDebugMenu_OldMan},
    {sText_TayaDebug_Trend, TayaDebugMenu_Trend},
    {sText_TayaDebug_TrendR, TayaDebugMenu_TrendR},
    {sText_TayaDebug_TownFlags, TayaDebugMenu_TownFlags},
    {sText_TayaDebug_AwardRibbon, TayaDebugMenu_AwardARibbon},
    {sText_TayaDebug_PokemonLottery, TayaDebugMenu_PKMNLottery}
};

const struct MenuAction sMenuActions_TayaDebug_Page2[] = {
    {sText_TayaDebug_Trainer, TayaDebugMenu_Trainer},
    {sText_TayaDebug_PokenavD, TayaDebugMenu_PokenavD}
};

const struct {
    const struct MenuAction *menuActions;
    u8 nitems;
} sMenuPages_TayaDebug[] = {
    {sMenuActions_TayaDebug_Page1, 9},
    {sMenuActions_TayaDebug_Page2, 2}
};

const u8 sText_TayaRibbon_Champion[] = _("CHANP");
const u8 sText_TayaRibbon_Cool[] = _("COOL");
const u8 sText_TayaRibbon_Beauty[] = _("BEAUTY");
const u8 sText_TayaRibbon_Cute[] = _("CUTE");
const u8 sText_TayaRibbon_Smart[] = _("SMART");
const u8 sText_TayaRibbon_Tough[] = _("TOUGH");
const u8 sText_TayaRibbon_Winning[] = _("WIN");
const u8 sText_TayaRibbon_Victory[] = _("VICTORY");
const u8 sText_TayaRibbon_Artist[] = _("BROMIDE");
const u8 sText_TayaRibbon_Effort[] = _("ACCESSIT");
const u8 sText_TayaRibbon_Marine[] = _("MARINE");
const u8 sText_TayaRibbon_Land[] = _("LAND");
const u8 sText_TayaRibbon_Sky[] = _("SKY");
const u8 sText_TayaRibbon_Country[] = _("COUNTRY");
const u8 sText_TayaRibbon_National[] = _("NATIONAL");
const u8 sText_TayaRibbon_Earth[] = _("EARTH");
const u8 sText_TayaRibbon_World[] = _("WORLD");
const u8 sText_TayaRibbon_Blank[] = _("　　　　　");

const struct {
    const u8 * text;
    u16 param;
} sRibbonOptions_TayaDebug[][3] = {
    {
        {sText_TayaRibbon_Champion, MON_DATA_CHAMPION_RIBBON},
        {sText_TayaRibbon_Cool, MON_DATA_COOL_RIBBON},
        {sText_TayaRibbon_Beauty, MON_DATA_BEAUTY_RIBBON}
    }, {
        {sText_TayaRibbon_Cute, MON_DATA_CUTE_RIBBON},
        {sText_TayaRibbon_Smart, MON_DATA_SMART_RIBBON},
        {sText_TayaRibbon_Tough, MON_DATA_TOUGH_RIBBON}
    }, {
        {sText_TayaRibbon_Winning, MON_DATA_WINNING_RIBBON},
        {sText_TayaRibbon_Victory, MON_DATA_VICTORY_RIBBON},
        {sText_TayaRibbon_Artist, MON_DATA_ARTIST_RIBBON}
    }, {
        {sText_TayaRibbon_Effort, MON_DATA_EFFORT_RIBBON},
        {sText_TayaRibbon_Marine, MON_DATA_MARINE_RIBBON},
        {sText_TayaRibbon_Land, MON_DATA_LAND_RIBBON}
    }, {
        {sText_TayaRibbon_Sky, MON_DATA_SKY_RIBBON},
        {sText_TayaRibbon_Country, MON_DATA_COUNTRY_RIBBON},
        {sText_TayaRibbon_National, MON_DATA_NATIONAL_RIBBON}
    }, {
        {sText_TayaRibbon_Earth, MON_DATA_EARTH_RIBBON},
        {sText_TayaRibbon_World, MON_DATA_WORLD_RIBBON},
        {sText_TayaRibbon_Blank, 0 /* sentinel */}
    }
};

const u8 sText_TayaRibbon_Select[] = _("Select Ribbon");

bool8 TayaDebugMenu_Trend(void)
{
    u8 sp00[32];
    u8 sp20[8];
    u16 i;
    struct DewfordTrend *trend;

    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 30, 11);
    trend = gSaveBlock1.dewfordTrends;

    for (i = 0; i < SAVED_TRENDS_COUNT; i++)
    {
        u8 * r4;

        sp00[0] = trend->gainingTrendiness ? CHAR_0 + 1 : CHAR_0 + 0;
        CopyEasyChatWord(sp20, trend->words[0]);
        r4 = StringCopyPadded(sp00 + 1, sp20, CHAR_SPACE, 7);
        CopyEasyChatWord(sp20, trend->words[1]);
        r4 = StringCopyPadded(r4, sp20, CHAR_SPACE, 8);
        r4 = ConvertIntToDecimalStringN(r4, trend->trendiness, STR_CONV_MODE_RIGHT_ALIGN, 3);
        *r4++ = CHAR_SPACE;
        r4 = ConvertIntToDecimalStringN(r4, trend->maxTrendiness, STR_CONV_MODE_RIGHT_ALIGN, 3);
        *r4++ = CHAR_SPACE;
        ConvertIntToDecimalStringN(r4, trend->rand, STR_CONV_MODE_RIGHT_ALIGN, 5);
        Menu_PrintText(sp00, 1, 2 * i + 1);
        trend++;
    }
    gMenuCallback = debug_sub_8090808;
    return FALSE;
}

bool8 debug_sub_8090808(void)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        Menu_EraseScreen();
        CloseMenu();
        return TRUE;
    }

    return FALSE;
}

bool8 TayaDebugMenu_TrendR(void)
{
    u16 i;
    u16 j;

    for (i = 0; i < SAVED_TRENDS_COUNT; i++)
    {
        for (j = 0; j < 2; j++)
        {
            gSaveBlock1.dewfordTrends[i].words[j] = GetRandomEasyChatWordFromGroup(Random() % 22);
        }
    }
    Menu_EraseScreen();
    CloseMenu();
    return TRUE;
}

bool8 TayaDebugMenu_WaitForEasyChatFade(void)
{
    if (!UpdatePaletteFade())
    {
        ShowEasyChatScreen();
        return TRUE;
    }

    return FALSE;
}

bool8 TayaDebugMenu_HandleEasyChatInput(void)
{
    s8 input = Menu_ProcessInput();

    switch (input)
    {
        case -1:
            CloseMenu();
            return TRUE;
        case -2:
            return FALSE;
        default:
            gSpecialVar_0x8004 = sEasyChatOptions_TayaDebug[input].round1Points;
            switch (gSpecialVar_0x8004)
            {
                case 5:
                case 7:
                case 8:
                case 11:
                case 12:
                    gSpecialVar_0x8005 = 0;
                    gSpecialVar_0x8006 = 0;
                default:
                    FadeScreen(1, 0);
                    gMenuCallback = TayaDebugMenu_WaitForEasyChatFade;
                    break;
            }
            return FALSE;
    }
}

bool8 TayaDebugMenu_SimpleText(void)
{
    Menu_DrawStdWindowFrame(0, 0, 12, 19);
    Menu_PrintItems(1, 1, ARRAY_COUNT(sEasyChatOptions_TayaDebug), sEasyChatOptions_TayaDebug);
    InitMenu(0, 1, 1, ARRAY_COUNT(sEasyChatOptions_TayaDebug), 0, 11);
    gMenuCallback = TayaDebugMenu_HandleEasyChatInput;
    return FALSE;
}

bool8 TayaDebugMenu_HandleOldManInput(void)
{
    s8 input = Menu_ProcessInput();

    switch (input)
    {
        case -1:
            CloseMenu();
            return TRUE;
        default:
            if (input < 5)
            {
                DebugSetMauvilleOldMan(input);
                CloseMenu();
                return TRUE;
            }
            break;
        case -2:
            return FALSE;
    }

    if (input == 5)
    {
        ResetMauvilleOldManFlag();
    }
    else if (input == 6)
    {
        u16 i;

        for (i = 0; i < 3; i++)
        {
            union OldMan *oldMan = &gSaveBlock1.oldMan;
            oldMan->bard.songLyrics[i] = sBardTestLyrics[i];
            oldMan->bard.newSongLyrics[i] = sBardTestLyrics[i];
            gSaveBlock1.easyChats.unk2B28[i] = sBardTestLyrics[i];
        }
    }
    else if (input == 7)
    {
        u16 i;

        for (i = 0; i < NUM_TRENDY_SAYINGS; i++)
        {
            UnlockTrendySaying(i);
        }
    }
    CloseMenu();
    return TRUE;
}

bool8 TayaDebugMenu_OldMan(void)
{
    Menu_DrawStdWindowFrame(0, 0, 10, 17);
    Menu_PrintItems(1, 1, ARRAY_COUNT(sOldManOptions_TayaDebug), sOldManOptions_TayaDebug);
    InitMenu(0, 1, 1, ARRAY_COUNT(sOldManOptions_TayaDebug), GetCurrentMauvilleOldMan(), 9);
    gMenuCallback = TayaDebugMenu_HandleOldManInput;
    return FALSE;
}

bool8 TayaDebugMenu_LanettesPC(void)
{
    Menu_EraseScreen();
    ShowPokemonStorageSystemPC();
    return TRUE;
}

bool8 TayaDebugMenu_TownFlags(void)
{
    FlagSet(FLAG_VISITED_LITTLEROOT_TOWN);
    FlagSet(FLAG_VISITED_OLDALE_TOWN);
    FlagSet(FLAG_VISITED_DEWFORD_TOWN);
    FlagSet(FLAG_VISITED_LAVARIDGE_TOWN);
    FlagSet(FLAG_VISITED_FALLARBOR_TOWN);
    FlagSet(FLAG_VISITED_VERDANTURF_TOWN);
    FlagSet(FLAG_VISITED_PACIFIDLOG_TOWN);
    FlagSet(FLAG_VISITED_PETALBURG_CITY);
    FlagSet(FLAG_VISITED_SLATEPORT_CITY);
    FlagSet(FLAG_VISITED_MAUVILLE_CITY);
    FlagSet(FLAG_VISITED_RUSTBORO_CITY);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    FlagSet(FLAG_VISITED_LILYCOVE_CITY);
    FlagSet(FLAG_VISITED_MOSSDEEP_CITY);
    FlagSet(FLAG_VISITED_SOOTOPOLIS_CITY);
    FlagSet(FLAG_VISITED_EVER_GRANDE_CITY);
    FlagSet(FLAG_LANDMARK_BATTLE_TOWER);
    FlagSet(FLAG_LANDMARK_SOUTHERN_ISLAND);
    FlagSet(FLAG_LANDMARK_FIERY_PATH);
    FlagSet(FLAG_SYS_POKEMON_LEAGUE_FLY);
    FlagSet(FLAG_LANDMARK_ISLAND_CAVE);
    FlagSet(FLAG_LANDMARK_DESERT_RUINS);
    FlagSet(FLAG_LANDMARK_FOSSIL_MANIACS_HOUSE);
    FlagSet(FLAG_LANDMARK_SCORCHED_SLAB);
    FlagSet(FLAG_LANDMARK_ANCIENT_TOMB);
    FlagSet(FLAG_LANDMARK_TUNNELERS_REST_HOUSE);
    FlagSet(FLAG_LANDMARK_HUNTERS_HOUSE);
    FlagSet(FLAG_LANDMARK_SEALED_CHAMBER);
    FlagSet(FLAG_LANDMARK_FLOWER_SHOP);
    FlagSet(FLAG_LANDMARK_MR_BRINEY_HOUSE);
    FlagSet(FLAG_LANDMARK_ABANDONED_SHIP);
    FlagSet(FLAG_LANDMARK_SEASHORE_HOUSE);
    FlagSet(FLAG_LANDMARK_NEW_MAUVILLE);
    FlagSet(FLAG_LANDMARK_OLD_LADY_REST_SHOP);
    FlagSet(FLAG_LANDMARK_TRICK_HOUSE);
    FlagSet(FLAG_LANDMARK_WINSTRATE_FAMILY);
    FlagSet(FLAG_LANDMARK_GLASS_WORKSHOP);
    FlagSet(FLAG_LANDMARK_LANETTES_HOUSE);
    FlagSet(FLAG_LANDMARK_POKEMON_DAYCARE);
    FlagSet(FLAG_LANDMARK_SEAFLOOR_CAVERN);
    FlagSet(FLAG_SYS_RIBBON_GET);
    CloseMenu();
    return TRUE;
}

bool8 TayaDebugMenu_AwardARibbon(void)
{
    BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
    SetMainCallback2(TayaDebugRibbon_Init);
    CloseMenu();
    return TRUE;
}

void debug_sub_8090C44(void)
{
    ConvertIntToDecimalStringN(eTayaLuckyNumber.charbuf, eTayaLuckyNumber.curLuckyId, STR_CONV_MODE_LEADING_ZEROS, 5);
    Menu_PrintText(eTayaLuckyNumber.charbuf, 1, 1);
    StringFill(eTayaLuckyNumber.charbuf, CHAR_SPACE, 5);
    eTayaLuckyNumber.charbuf[eTayaLuckyNumber.digit] = 0x79;
    Menu_PrintText(eTayaLuckyNumber.charbuf, 1, 3);
}

bool8 debug_sub_8090C88(void)
{
    bool8 r8 = TRUE;

    do
    {
        if (JOY_NEW(DPAD_LEFT) && eTayaLuckyNumber.digit != 0)
        {
            eTayaLuckyNumber.digit--;
            break;
        }
        if (JOY_NEW(DPAD_RIGHT) && eTayaLuckyNumber.digit < 4)
        {
            eTayaLuckyNumber.digit++;
            break;
        }
        if (JOY_REPT(DPAD_UP))
        {
            u8 r4;

            eTayaLuckyNumber.tempLuckyId = eTayaLuckyNumber.curLuckyId;
            eTayaLuckyNumber.digitDeltaMagnitude = 10000;
            for (r4 = 0; r4 < eTayaLuckyNumber.digit; r4++)
                eTayaLuckyNumber.digitDeltaMagnitude /= 10;
            eTayaLuckyNumber.tempLuckyId += eTayaLuckyNumber.digitDeltaMagnitude;
            if (eTayaLuckyNumber.tempLuckyId > 0xFFFF)
                eTayaLuckyNumber.tempLuckyId = 0xFFFF;
            if (eTayaLuckyNumber.curLuckyId != eTayaLuckyNumber.tempLuckyId)
            {
                eTayaLuckyNumber.curLuckyId = eTayaLuckyNumber.tempLuckyId;
                break;
            }
        }
        if (JOY_REPT(DPAD_DOWN))
        {
            u8 r4;

            eTayaLuckyNumber.tempLuckyId = eTayaLuckyNumber.curLuckyId;
            eTayaLuckyNumber.digitDeltaMagnitude = 10000;
            for (r4 = 0; r4 < eTayaLuckyNumber.digit; r4++)
                eTayaLuckyNumber.digitDeltaMagnitude /= 10;
            eTayaLuckyNumber.tempLuckyId -= eTayaLuckyNumber.digitDeltaMagnitude;
            if (eTayaLuckyNumber.tempLuckyId < 0)
                eTayaLuckyNumber.tempLuckyId = 0;
            if (eTayaLuckyNumber.curLuckyId != eTayaLuckyNumber.tempLuckyId)
            {
                eTayaLuckyNumber.curLuckyId = eTayaLuckyNumber.tempLuckyId;
                break;
            }
        }
        if (JOY_NEW(B_BUTTON))
        {
            CloseMenu();
            return TRUE;
        }
        if (JOY_NEW(A_BUTTON))
        {
            SetLotteryNumber16_Unused(eTayaLuckyNumber.curLuckyId);
            CloseMenu();
            return TRUE;
        }
        r8 = FALSE;
    } while (0);

    if (r8)
        debug_sub_8090C44();
    return FALSE;
}

bool8 TayaDebugMenu_PKMNLottery(void)
{
    Menu_DrawStdWindowFrame(0, 0, 6, 5);
    RetrieveLotteryNumber();
    eTayaLuckyNumber.curLuckyId = gSpecialVar_Result;
    eTayaLuckyNumber.digit = 0;
    debug_sub_8090C44();
    gMenuCallback = debug_sub_8090C88;
    return FALSE;
}

bool8 TayaDebugMenu_Trainer(void)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(gTrainerEyeTrainers); i++)
        SetTrainerFlag(gTrainerEyeTrainers[i].opponentIDs[0]);

    SetTrainerFlag(TRAINER_ROXANNE);
    SetTrainerFlag(TRAINER_BRAWLY);
    SetTrainerFlag(TRAINER_WATTSON);
    SetTrainerFlag(TRAINER_FLANNERY);
    SetTrainerFlag(TRAINER_NORMAN);
    SetTrainerFlag(TRAINER_WINONA);
    SetTrainerFlag(TRAINER_TATE_AND_LIZA);
    SetTrainerFlag(TRAINER_WALLACE);
    SetTrainerFlag(TRAINER_SIDNEY);
    SetTrainerFlag(TRAINER_PHOEBE);
    SetTrainerFlag(TRAINER_GLACIA);
    SetTrainerFlag(TRAINER_DRAKE);
    SetTrainerFlag(TRAINER_STEVEN);
    CloseMenu();
    return TRUE;
}

bool8 TayaDebugMenu_PokenavD(void)
{
    u16 i;
    u16 j;
    
    Menu_DisplayDialogueFrame();
    
    for (i = 0; i < 14; i++)
    {
        StringCopy(gSharedMem, gUnknown_Debug_083C4F94);
        gSharedMem[i + 1] = EOS;
        Menu_PrintText(gSharedMem, 2, 15);
        for (j = 0; j < 30; j++)
        {
            struct BoxPokemon *boxPokemon;
            u32 otId = Random() + 1;
            u16 level = (Random() % 100) + 1;
            u16 species = (Random() % 386) + 1;
            if (species >= SPECIES_OLD_UNOWN_B)
            {
                species += SPECIES_TREECKO - SPECIES_OLD_UNOWN_B;
                if (species >= NUM_SPECIES)
                    species = SPECIES_BULBASAUR;
            }
            boxPokemon = gPokemonStorage.boxes[i] + j;
            CreateBoxMon(boxPokemon, species, level, 32, FALSE, 0, TRUE, otId);

            otId = Random() & 0xff;
            SetBoxMonData(boxPokemon, MON_DATA_COOL, &otId);

            otId = Random() & 0xff;
            SetBoxMonData(boxPokemon, MON_DATA_BEAUTY, &otId);

            otId = Random() & 0xff;
            SetBoxMonData(boxPokemon, MON_DATA_CUTE, &otId);

            otId = Random() & 0xff;
            SetBoxMonData(boxPokemon, MON_DATA_SMART, &otId);

            otId = Random() & 0xff;
            SetBoxMonData(boxPokemon, MON_DATA_TOUGH, &otId);

            otId = Random() & 0xff;
            SetBoxMonData(boxPokemon, MON_DATA_SHEEN, &otId);

            otId = (Random() & 3) + 1;
            SetBoxMonData(boxPokemon, MON_DATA_COOL_RIBBON, &otId);

            otId = (Random() & 3) + 1;
            SetBoxMonData(boxPokemon, MON_DATA_BEAUTY_RIBBON, &otId);

            otId = (Random() & 3) + 1;
            SetBoxMonData(boxPokemon, MON_DATA_CUTE_RIBBON, &otId);

            otId = (Random() & 3) + 1;
            SetBoxMonData(boxPokemon, MON_DATA_TOUGH_RIBBON, &otId);

            otId = (Random() & 3) + 1;
            SetBoxMonData(boxPokemon, MON_DATA_SMART_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_CHAMPION_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_WINNING_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_VICTORY_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_ARTIST_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_EFFORT_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_MARINE_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_LAND_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_SKY_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_COUNTRY_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_NATIONAL_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_EARTH_RIBBON, &otId);

            otId = Random() & 1;
            SetBoxMonData(boxPokemon, MON_DATA_WORLD_RIBBON, &otId);
        }
    }
    TayaDebugMenu_TownFlags();
    TayaDebugMenu_Trainer();
    CloseMenu();
    return TRUE;
}

bool8 TayaDebugMenu_HandleInput(void)
{
    s8 input = Menu_ProcessInput();
    s8 r4;

    switch (input)
    {
        default:
            gMenuCallback = sMenuPages_TayaDebug[sTayaTopMenuPage].menuActions[input].func;
            return FALSE;
        case -2:
            r4 = sTayaTopMenuPage;
            if (JOY_NEW(DPAD_LEFT))
            {
                sTayaTopMenuPage--;
                if (sTayaTopMenuPage < 0)
                    sTayaTopMenuPage = 1;
            }

            if (JOY_NEW(DPAD_RIGHT))
            {
                sTayaTopMenuPage++;
                if ((u8)sTayaTopMenuPage > 1)
                    sTayaTopMenuPage = 0;
            }
            if (r4 != sTayaTopMenuPage)
            {
                Menu_EraseScreen();
                Menu_DrawStdWindowFrame(0, 0, 11, 19);
                Menu_PrintItems(1, 1, sMenuPages_TayaDebug[sTayaTopMenuPage].nitems, sMenuPages_TayaDebug[sTayaTopMenuPage].menuActions);
                InitMenu(0, 1, 1, sMenuPages_TayaDebug[sTayaTopMenuPage].nitems, 0, 10);
            }
            return FALSE;
        case -1:
            CloseMenu();
            return TRUE;
    }
}

bool8 InitTayaDebugWindow(void)
{
    sTayaTopMenuPage = 0;
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 11, 19);
    Menu_PrintItems(1, 1, 9, sMenuPages_TayaDebug[0].menuActions);
    InitMenu(0, 1, 1, 9, 0, 10);
    gMenuCallback = TayaDebugMenu_HandleInput;
    return FALSE;
}

bool8 debug_sub_80912D8(void)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_OpenDebugRegionMap);
        return TRUE;
    }
    return FALSE;
}

bool8 debug_sub_8091300(void)
{
    FadeScreen(1, 0);
    gMenuCallback = debug_sub_80912D8;
    return FALSE;
}

void TayaDebugRibbon_VBlankCallback(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void TayaDebugRibbon_MainCallback(void)
{
    AnimateSprites();
    BuildOamBuffer();

    switch (eTayaMonData.state)
    {
        case 0:
            eTayaMonData.redraw = 0;
            if (JOY_NEW(DPAD_UP))
            {
                if (eTayaMonData.y != 0)
                {
                    eTayaMonData.y--;
                    eTayaMonData.redraw = 1;
                }
            }
            if (JOY_NEW(DPAD_DOWN))
            {
                if (eTayaMonData.x != 2)
                {
                    if (eTayaMonData.y < 5)
                    {
                        eTayaMonData.y++;
                        eTayaMonData.redraw = 1;
                    }
                }
                else
                {
                    if (eTayaMonData.y < 4)
                    {
                        eTayaMonData.y++;
                        eTayaMonData.redraw = 1;
                    }
                }
            }
            if (JOY_NEW(DPAD_LEFT))
            {
                if (eTayaMonData.x != 0)
                {
                    eTayaMonData.x--;
                    eTayaMonData.redraw = 1;
                }
            }
            if (JOY_NEW(DPAD_RIGHT))
            {
                if (eTayaMonData.y != 5)
                {
                    if (eTayaMonData.x < 2)
                    {
                        eTayaMonData.x++;
                        eTayaMonData.redraw = 1;
                    }
                }
                else
                {
                    if (eTayaMonData.x < 1)
                    {
                        eTayaMonData.x++;
                        eTayaMonData.redraw = 1;
                    }
                }
            }
            if (eTayaMonData.redraw)
            {
                TayaDebugRibbon_PrintOptions();
            }
            else if (JOY_NEW(A_BUTTON))
            {
                u16 param = sRibbonOptions_TayaDebug[eTayaMonData.y][eTayaMonData.x].param;
                if (param >= MON_DATA_COOL_RIBBON && param <= MON_DATA_TOUGH_RIBBON)
                    eTayaMonData.maxVal = 4;
                else
                    eTayaMonData.maxVal = 1;
                eTayaMonData.state = 1;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
                SetMainCallback2(sub_80546F0);
            }
            break;
        case 1:
            eTayaMonData.redraw = 0;
            if (JOY_NEW(DPAD_UP))
            {
                if (eTayaMonData.data[eTayaMonData.y][eTayaMonData.x] < eTayaMonData.maxVal)
                {
                    eTayaMonData.data[eTayaMonData.y][eTayaMonData.x]++;
                    eTayaMonData.redraw = 1;
                }
            }
            if (JOY_NEW(DPAD_DOWN))
            {
                if (eTayaMonData.data[eTayaMonData.y][eTayaMonData.x] != 0)
                {
                    eTayaMonData.data[eTayaMonData.y][eTayaMonData.x]--;
                    eTayaMonData.redraw = 1;
                }
            }
            if (eTayaMonData.redraw)
            {
                TayaDebugRibbon_PrintOptions();
            }
            else
            {
                if (JOY_NEW(B_BUTTON))
                {
                    eTayaMonData.data[eTayaMonData.y][eTayaMonData.x] = GetMonData(gPlayerParty, sRibbonOptions_TayaDebug[eTayaMonData.y][eTayaMonData.x].param);
                    TayaDebugRibbon_PrintOptions();
                    eTayaMonData.state = 0;
                }
                if (JOY_NEW(A_BUTTON))
                {
                    if (sRibbonOptions_TayaDebug[eTayaMonData.y][eTayaMonData.x].param)
                        SetMonData(gPlayerParty, sRibbonOptions_TayaDebug[eTayaMonData.y][eTayaMonData.x].param, &eTayaMonData.data[eTayaMonData.y][eTayaMonData.x]);
                    eTayaMonData.state = 0;
                }
            }
            break;
    }
}

void TayaDebugRibbon_Init(void)
{
    u8 i;
    u8 j;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 3; j++)
        {
            u16 param = sRibbonOptions_TayaDebug[i][j].param;
            if (param)
                eTayaMonData.data[i][j] = GetMonData(gPlayerParty, param);
            else
                eTayaMonData.data[i][j] = 0;
        }
    }
    Text_LoadWindowTemplate(&gWindowTemplate_81E7224);
    InitMenuWindow(&gWindowTemplate_81E7224);
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 29, 3);
    Menu_PrintText(sText_TayaRibbon_Select, 1, 1);
    Menu_DrawStdWindowFrame(0, 4, 29, 17);
    Menu_DrawStdWindowFrame(0, 18, 29, 21);
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_OBJ_ON;
    eTayaMonData.x = 0;
    eTayaMonData.y = 0;
    eTayaMonData.state = 0;
    TayaDebugRibbon_PrintOptions();
    SetVBlankCallback(TayaDebugRibbon_VBlankCallback);
    SetMainCallback2(TayaDebugRibbon_MainCallback);
}

void TayaDebugRibbon_PrintOptions(void)
{
    u8 i;
    u8 j;

    for (i = 0; i < 6; i++)
    {
        u8 * buffer = eTayaMonData.charbuf;
        for (j = 0; j < 3 && !(i == 5 && j == 2); j++)
        {
            if (eTayaMonData.x == j && eTayaMonData.y == i)
                *buffer++ = 0xEF;
            else
            {
                *buffer++ = CHAR_SPACE;
                *buffer++ = CHAR_SPACE;
            }
            buffer = StringCopy(buffer, sRibbonOptions_TayaDebug[i][j].text);
            *buffer++ = CHAR_SPACE;
            buffer = ConvertIntToDecimalStringN(buffer, eTayaMonData.data[i][j], STR_CONV_MODE_LEFT_ALIGN, 1);
            *buffer++ = CHAR_SPACE;
        }
        buffer[-1] = EOS;
        Menu_PrintText(eTayaMonData.charbuf, 1, i * 2 + 5);
    }
}

#endif // DEBUG
