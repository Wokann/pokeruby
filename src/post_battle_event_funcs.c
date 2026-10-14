#include "global.h"
#include "constants/heal_locations.h"
#include "script_pokemon_util.h"
#include "event_data.h"
#include "credits.h"
#include "hall_of_fame.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"

int GameClear(void)
{
    int i;
    bool32 ribbonGet;

    HealPlayerParty();

    if (FlagGet(FLAG_SYS_GAME_CLEAR) == TRUE)
    {
        gHasHallOfFameRecords = 1;
    }
    else
    {
        gHasHallOfFameRecords = 0;
        FlagSet(FLAG_SYS_GAME_CLEAR);
    }

    if (!GetGameStat(GAME_STAT_FIRST_HOF_PLAY_TIME))
        SetGameStat(GAME_STAT_FIRST_HOF_PLAY_TIME, (gSaveBlock2.playTimeHours << 16) | (gSaveBlock2.playTimeMinutes << 8) | gSaveBlock2.playTimeSeconds);

    SetContinueGameWarpStatus();

    if (gSaveBlock2.playerGender == MALE)
        SetContinueGameWarpToHealLocation(HEAL_LOCATION_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F);
    else
        SetContinueGameWarpToHealLocation(HEAL_LOCATION_LITTLEROOT_TOWN_MAYS_HOUSE_2F);

    ribbonGet = FALSE;

    for (i = 0; i < 6; i++)
    {
        u8 val;
        u8 *ptr = &val;
        if (GetMonData(&gPlayerParty[i], MON_DATA_SANITY_HAS_SPECIES)
         && !GetMonData(&gPlayerParty[i], MON_DATA_SANITY_IS_EGG)
         && !GetMonData(&gPlayerParty[i], MON_DATA_CHAMPION_RIBBON))
        {
            *ptr = 1;
            SetMonData(&gPlayerParty[i], MON_DATA_CHAMPION_RIBBON, ptr);
            ribbonGet = TRUE;
        }
    }

    if (ribbonGet == TRUE)
    {
        IncrementGameStat(GAME_STAT_RECEIVED_RIBBONS);
        FlagSet(FLAG_SYS_RIBBON_GET);
    }

    SetMainCallback2(CB2_DoHallOfFameScreen);
    return 0;
}

int sp0C8_whiteout_maybe(void)
{
    SetMainCallback2(CB2_WhiteOut);
    return 0;
}
