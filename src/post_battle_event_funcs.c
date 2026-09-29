#include "global.h"
#include "constants/heal_locations.h"
#include "contest_util.h"
#include "event_data.h"
#include "hall_of_fame.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"

extern bool8 gHasHallOfFameRecords;

int GameClear(void)
{
    int i;
    bool32 ribbonGet;

    HealPlayerParty();

    if (FlagGet(FLAG_SYS_GAME_CLEAR) == TRUE)
    {
        gHasHallOfFameRecords = TRUE;
    }
    else
    {
        gHasHallOfFameRecords = FALSE;
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

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u8 ribbonValue;
        u8 *ribbonValuePtr = &ribbonValue;
        if (GetMonData(&gPlayerParty[i], MON_DATA_SANITY_HAS_SPECIES)
         && !GetMonData(&gPlayerParty[i], MON_DATA_SANITY_IS_EGG)
         && !GetMonData(&gPlayerParty[i], MON_DATA_CHAMPION_RIBBON))
        {
            *ribbonValuePtr = TRUE;
            SetMonData(&gPlayerParty[i], MON_DATA_CHAMPION_RIBBON, ribbonValuePtr);
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

bool8 SetCB2WhiteOut(void)
{
    SetMainCallback2(CB2_WhiteOut);
    return FALSE;
}
