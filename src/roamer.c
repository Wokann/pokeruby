#include "global.h"
#include "debug.h"
#include "roamer.h"
#include "pokemon.h"
#include "random.h"
#include "region_map.h"
#include "constants/maps.h"
#include "constants/species.h"

// Despite having a variable to track it, the roamer is
// hard-coded to only ever be in map group 0.
#define ROAMER_MAP_GROUP 0

enum
{
    MAP_GRP = 0, // map group
    MAP_NUM = 1, // map number
};

EWRAM_DATA static u8 sLocationHistory[3][2] = {0};
EWRAM_DATA static u8 sRoamerLocation[2] = {0};

#define ___ MAP_NUM(MAP_UNDEFINED) // For empty spots in the location table

static const u8 sRoamerLocations[][6] =
{
    { MAP_NUM(MAP_ROUTE110), MAP_NUM(MAP_ROUTE111), MAP_NUM(MAP_ROUTE117), MAP_NUM(MAP_ROUTE118), MAP_NUM(MAP_ROUTE134), ___ },
    { MAP_NUM(MAP_ROUTE111), MAP_NUM(MAP_ROUTE110), MAP_NUM(MAP_ROUTE117), MAP_NUM(MAP_ROUTE118), ___, ___ },
    { MAP_NUM(MAP_ROUTE117), MAP_NUM(MAP_ROUTE111), MAP_NUM(MAP_ROUTE110), MAP_NUM(MAP_ROUTE118), ___, ___ },
    { MAP_NUM(MAP_ROUTE118), MAP_NUM(MAP_ROUTE117), MAP_NUM(MAP_ROUTE110), MAP_NUM(MAP_ROUTE111), MAP_NUM(MAP_ROUTE119), MAP_NUM(MAP_ROUTE123) },
    { MAP_NUM(MAP_ROUTE119), MAP_NUM(MAP_ROUTE118), MAP_NUM(MAP_ROUTE120), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE120), MAP_NUM(MAP_ROUTE119), MAP_NUM(MAP_ROUTE121), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE121), MAP_NUM(MAP_ROUTE120), MAP_NUM(MAP_ROUTE122), MAP_NUM(MAP_ROUTE123), ___, ___ },
    { MAP_NUM(MAP_ROUTE122), MAP_NUM(MAP_ROUTE121), MAP_NUM(MAP_ROUTE123), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE123), MAP_NUM(MAP_ROUTE122), MAP_NUM(MAP_ROUTE118), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE124), MAP_NUM(MAP_ROUTE121), MAP_NUM(MAP_ROUTE125), MAP_NUM(MAP_ROUTE126), ___, ___ },
    { MAP_NUM(MAP_ROUTE125), MAP_NUM(MAP_ROUTE124), MAP_NUM(MAP_ROUTE127), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE126), MAP_NUM(MAP_ROUTE124), MAP_NUM(MAP_ROUTE127), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE127), MAP_NUM(MAP_ROUTE125), MAP_NUM(MAP_ROUTE126), MAP_NUM(MAP_ROUTE128), ___, ___ },
    { MAP_NUM(MAP_ROUTE128), MAP_NUM(MAP_ROUTE127), MAP_NUM(MAP_ROUTE129), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE129), MAP_NUM(MAP_ROUTE128), MAP_NUM(MAP_ROUTE130), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE130), MAP_NUM(MAP_ROUTE129), MAP_NUM(MAP_ROUTE131), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE131), MAP_NUM(MAP_ROUTE130), MAP_NUM(MAP_ROUTE132), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE132), MAP_NUM(MAP_ROUTE131), MAP_NUM(MAP_ROUTE133), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE133), MAP_NUM(MAP_ROUTE132), MAP_NUM(MAP_ROUTE134), ___, ___, ___ },
    { MAP_NUM(MAP_ROUTE134), MAP_NUM(MAP_ROUTE133), MAP_NUM(MAP_ROUTE110), ___, ___, ___ },
    { ___, ___, ___, ___, ___, ___ },
};

#undef ___

#define NUM_LOCATION_SETS (ARRAY_COUNT(sRoamerLocations) - 1)
#define NUM_LOCATIONS_PER_SET (ARRAY_COUNT(sRoamerLocations[0]))

void ClearRoamerData(void)
{
    memset(&gSaveBlock1.roamer, 0, sizeof(gSaveBlock1.roamer));
}

void ClearRoamerLocationData(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sLocationHistory); i++)
    {
        sLocationHistory[i][MAP_GRP] = 0;
        sLocationHistory[i][MAP_NUM] = 0;
    }

    sRoamerLocation[MAP_GRP] = 0;
    sRoamerLocation[MAP_NUM] = 0;
}

static void CreateInitialRoamerMon(void)
{
    struct Roamer *roamer;
    CreateMon(&gEnemyParty[0], ROAMER_SPECIES, 40, 0x20, 0, 0, 0, 0);
    roamer = &gSaveBlock1.roamer;
    roamer->species = ROAMER_SPECIES;
    roamer->level = 40;
    roamer->status = 0;
    roamer->active = TRUE;
    roamer->ivs = GetMonData(&gEnemyParty[0], MON_DATA_IVS);
    roamer->personality = GetMonData(&gEnemyParty[0], MON_DATA_PERSONALITY);
    roamer->hp = GetMonData(&gEnemyParty[0], MON_DATA_MAX_HP);
    roamer->cool = GetMonData(&gEnemyParty[0], MON_DATA_COOL);
    roamer->beauty = GetMonData(&gEnemyParty[0], MON_DATA_BEAUTY);
    roamer->cute = GetMonData(&gEnemyParty[0], MON_DATA_CUTE);
    roamer->smart = GetMonData(&gEnemyParty[0], MON_DATA_SMART);
    roamer->tough = GetMonData(&gEnemyParty[0], MON_DATA_TOUGH);
    sRoamerLocation[MAP_GRP] = ROAMER_MAP_GROUP;
    sRoamerLocation[MAP_NUM] = sRoamerLocations[Random() % NUM_LOCATION_SETS][0];
}

void InitRoamer(void)
{
    ClearRoamerData();
    ClearRoamerLocationData();
    CreateInitialRoamerMon();
}

void UpdateLocationHistoryForRoamer(void)
{
    sLocationHistory[2][MAP_GRP] = sLocationHistory[1][MAP_GRP];
    sLocationHistory[2][MAP_NUM] = sLocationHistory[1][MAP_NUM];

    sLocationHistory[1][MAP_GRP] = sLocationHistory[0][MAP_GRP];
    sLocationHistory[1][MAP_NUM] = sLocationHistory[0][MAP_NUM];

    sLocationHistory[0][MAP_GRP] = gSaveBlock1.location.mapGroup;
    sLocationHistory[0][MAP_NUM] = gSaveBlock1.location.mapNum;
}

void RoamerMoveToOtherLocationSet(void)
{
    u8 mapNum = 0;
    struct Roamer *roamer = &gSaveBlock1.roamer;

    if (!roamer->active)
        return;

    sRoamerLocation[MAP_GRP] = ROAMER_MAP_GROUP;

    while (1)
    {
        mapNum = sRoamerLocations[Random() % NUM_LOCATION_SETS][0];
        if (sRoamerLocation[MAP_NUM] != mapNum)
        {
            sRoamerLocation[MAP_NUM] = mapNum;
            return;
        }
    }
}

void RoamerMove(void)
{
    u8 locSet = 0;

    if ((Random() % 16) == 0)
    {
        RoamerMoveToOtherLocationSet();
    }
    else
    {
        struct Roamer *roamer = &gSaveBlock1.roamer;

        if (!roamer->active)
            return;

        while (locSet < NUM_LOCATION_SETS)
        {
            if (sRoamerLocation[MAP_NUM] == sRoamerLocations[locSet][0])
            {
                u8 mapNum;
                while (1)
                {
                    mapNum = sRoamerLocations[locSet][(Random() % (NUM_LOCATIONS_PER_SET - 1)) + 1];
                    if (!(sLocationHistory[2][MAP_GRP] == ROAMER_MAP_GROUP && sLocationHistory[2][MAP_NUM] == mapNum)
                     && mapNum != MAP_NUM(MAP_UNDEFINED))
                        break;
                }
                sRoamerLocation[MAP_NUM] = mapNum;
                return;
            }
            locSet++;
        }
    }
}

bool8 IsRoamerAt(u8 mapGroup, u8 mapNum)
{
    struct Roamer *roamer = &gSaveBlock1.roamer;

    if (roamer->active && mapGroup == sRoamerLocation[MAP_GRP] && mapNum == sRoamerLocation[MAP_NUM])
        return TRUE;
    else
        return FALSE;
}

void CreateRoamerMonInstance(void)
{
    struct Pokemon *mon = &gEnemyParty[0];
    struct Roamer *roamer = &gSaveBlock1.roamer;
    CreateMonWithIVsPersonality(mon, roamer->species, roamer->level, roamer->ivs, roamer->personality);
    SetMonData(mon, MON_DATA_STATUS, &roamer->status);
    SetMonData(mon, MON_DATA_HP, &roamer->hp);
    SetMonData(mon, MON_DATA_COOL, &roamer->cool);
    SetMonData(mon, MON_DATA_BEAUTY, &roamer->beauty);
    SetMonData(mon, MON_DATA_CUTE, &roamer->cute);
    SetMonData(mon, MON_DATA_SMART, &roamer->smart);
    SetMonData(mon, MON_DATA_TOUGH, &roamer->tough);
}

bool8 TryStartRoamerEncounter(void)
{
    if (IsRoamerAt(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum) == TRUE && (Random() % 4) == 0)
    {
        CreateRoamerMonInstance();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void UpdateRoamerHPStatus(struct Pokemon *mon)
{
    struct Roamer *roamer;
    u16 hp;
    u8 status;

    hp = GetMonData(mon, MON_DATA_HP);

    roamer = &gSaveBlock1.roamer;
    roamer->hp = hp;

    status = GetMonData(mon, MON_DATA_STATUS);

    roamer->status = status;

    RoamerMoveToOtherLocationSet();
}

void SetRoamerInactive(void)
{
    struct Roamer *roamer = &gSaveBlock1.roamer;
    roamer->active = FALSE;
}

void GetRoamerLocation(u8 *mapGroup, u8 *mapNum)
{
    *mapGroup = sRoamerLocation[MAP_GRP];
    *mapNum = sRoamerLocation[MAP_NUM];
}

#if DEBUG
void Debug_CreateRoamer(void)
{
    if (gSaveBlock1.location.mapGroup == ROAMER_MAP_GROUP)
    {
        CreateInitialRoamerMon();
        sRoamerLocation[MAP_GRP] = ROAMER_MAP_GROUP;
        sRoamerLocation[MAP_NUM] = gSaveBlock1.location.mapNum;
    }
}

void Debug_GetRoamerLocation(u8 *str)
{
    GetMapName(str, sRoamerLocation[MAP_NUM], ROAMER_MAP_GROUP);
}
#endif
