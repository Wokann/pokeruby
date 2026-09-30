#include "global.h"
#include "main.h"
#include "decompress.h"
#include "battle_setup.h"
#include "overworld.h"
#include "pokenav.h"
#include "battle.h"
#include "data2.h"
#include "constants/opponents.h"
#include "constants/region_map_sections.h"
#include "text.h"
#include "de_string_util.h"
#include "string_util.h"

struct TrainersEyeGymLeadersAndE4
{
    u16 opponentId;
    u16 regionMapSectionId;
};

static const struct TrainersEyeGymLeadersAndE4 sGymLeaderTrainersEye[13] = {
    {TRAINER_ROXANNE,       MAPSEC_RUSTBORO_CITY},
    {TRAINER_BRAWLY,        MAPSEC_DEWFORD_TOWN},
    {TRAINER_WATTSON,       MAPSEC_MAUVILLE_CITY},
    {TRAINER_FLANNERY,      MAPSEC_LAVARIDGE_TOWN},
    {TRAINER_NORMAN,        MAPSEC_PETALBURG_CITY},
    {TRAINER_WINONA,        MAPSEC_FORTREE_CITY},
    {TRAINER_TATE_AND_LIZA, MAPSEC_MOSSDEEP_CITY},
    {TRAINER_WALLACE,       MAPSEC_SOOTOPOLIS_CITY},
    {TRAINER_SIDNEY,        MAPSEC_EVER_GRANDE_CITY},
    {TRAINER_PHOEBE,        MAPSEC_EVER_GRANDE_CITY},
    {TRAINER_GLACIA,        MAPSEC_EVER_GRANDE_CITY},
    {TRAINER_DRAKE,         MAPSEC_EVER_GRANDE_CITY},
    {TRAINER_STEVEN,        MAPSEC_EVER_GRANDE_CITY}
};

void LoadTrainerEyesPortraitGfx(u8);

void BuildTrainerEyesList(void)
{
    u16 i;

    gPokenavStructPtr->trainerEyesCount = 0;

    for (i = 0; i < REMATCH_TABLE_ENTRIES; i++)
    {
        if (HasTrainerBeenFought(gRematchTable[i].trainerIds[0]))
        {
            gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].opponentId = gRematchTable[i].trainerIds[0];
            gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].rematchTableIdx = i;
            gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].rematchNo = gSaveBlock1.trainerRematches[i];
            gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].regionMapSectionId = Overworld_GetMapHeaderByGroupAndId(gRematchTable[i].mapGroup, gRematchTable[i].mapNum)->regionMapSectionId;
            gPokenavStructPtr->trainerEyesCount++;
        }
    }

    for (i = 0; i < 13; i++)
    {
        {
            if (HasTrainerBeenFought(sGymLeaderTrainersEye[i].opponentId))
            {
                gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].opponentId = sGymLeaderTrainersEye[i].opponentId;
                gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].regionMapSectionId = sGymLeaderTrainersEye[i].regionMapSectionId;
                gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].rematchNo = 0;
                gPokenavStructPtr->trainersEye[gPokenavStructPtr->trainerEyesCount].rematchTableIdx = i + 0x38;
                gPokenavStructPtr->trainerEyesCount++;
            }
        }
    }

    gPokenavStructPtr->listCursorRow = 0;
    gPokenavStructPtr->firstVisibleListIndex = 0;
    gPokenavStructPtr->listSelectedIndex = 0;
    gPokenavStructPtr->lastVisibleListIndex = gPokenavStructPtr->trainerEyesCount <= 8 ? gPokenavStructPtr->trainerEyesCount - 1 : 7;
    gPokenavStructPtr->lastListIndex = gPokenavStructPtr->trainerEyesCount - 1;
    gPokenavStructPtr->hasListScrollArrows = gPokenavStructPtr->lastListIndex < 8 ? 0 : 1;
}

void PrepareTrainerEyesDetailPortrait(void)
{
    s32 portraitSlot = 0;
    gPokenavStructPtr->portraitSlot = portraitSlot;
    LoadTrainerEyesPortraitGfx(0);
    gPokenavStructPtr->trainerEyesPortraitXOffset = -72;
    CreateOrUpdateTrainerEyesPortrait(0);
    gPokenavStructPtr->detailSelectionIndex = gPokenavStructPtr->listSelectedIndex;
}

void LoadTrainerEyesPortraitGfx(u8 slot)
{
    u16 opponentId = gPokenavStructPtr->trainersEye[gPokenavStructPtr->listSelectedIndex].opponentId;
    u8 trainerPicId = gTrainers[opponentId].trainerPic;
    DecompressPicFromTable(&gTrainerFrontPicTable[trainerPicId], gTrainerFrontPicCoords[trainerPicId].coords, 1, gPokenavStructPtr->spriteGfxScratchBuffer, gPokenavStructPtr->spriteGfxBuffers[slot], trainerPicId);
    LZ77UnCompWram(gTrainerFrontPicPaletteTable[trainerPicId].data, gPokenavStructPtr->portraitPaletteBuffers[slot]);
}

bool8 SlideTrainerEyesPortraitIn(void)
{
    if (gPokenavStructPtr->trainerEyesPortraitXOffset == 0)
    {
        return FALSE;
    }
    if ((gPokenavStructPtr->trainerEyesPortraitXOffset += 8) >= 0)
    {
        gPokenavStructPtr->trainerEyesPortraitXOffset = 0;
        return FALSE;
    }
    return TRUE;
}

bool8 SlideTrainerEyesPortraitOut(void)
{
    if (gPokenavStructPtr->trainerEyesPortraitXOffset == -72)
    {
        return FALSE;
    }
    if ((gPokenavStructPtr->trainerEyesPortraitXOffset -= 8) <= -72)
    {
        gPokenavStructPtr->trainerEyesPortraitXOffset = -72;
        return FALSE;
    }
    return TRUE;
}

void InitTrainerEyesBackgroundPulse(void)
{
    gPokenavStructPtr->trainerEyesPulseFrameCounter = 0;
    gPokenavStructPtr->trainerEyesPulseState = 0;
    REG_BLDCNT = BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_DARKEN;
    REG_BLDY = 0;
    REG_WININ = 0x3F3F;
    REG_WINOUT = 0x1F1F;
    SetTrainerEyesDetailWindow(0);
}

void UpdateTrainerEyesBackgroundPulse(void)
{
    if (++gPokenavStructPtr->trainerEyesPulseFrameCounter > 8)
    {
        gPokenavStructPtr->trainerEyesPulseFrameCounter = 0;
        ++gPokenavStructPtr->trainerEyesPulseState;
        if (gPokenavStructPtr->trainerEyesPulseState & 1)
            REG_BLDY = 6;
        else
            REG_BLDY = 0;
    }
}

void SetTrainerEyesDetailWindow(bool8 showDetail)
{
    if (!showDetail)
    {
        REG_WIN0H = -0x1710;
        REG_WIN0V = 0x888;
    }
    else
    {
        REG_WIN0H = -0x1710;
        REG_WIN0V = 0x818;
    }
}

void StopTrainerEyesBackgroundPulse(void)
{
    REG_BLDCNT = 0;
    REG_BLDY = 0;
}

void BufferTrainerEyesListRowText(u8 *buffer, u16 listIndex)
{
    const struct Trainer *trainer = &gTrainers[gPokenavStructPtr->trainersEye[listIndex].opponentId];
    u8 *ptr = buffer;

    ptr = buffer;
    if (listIndex < gPokenavStructPtr->trainerEyesCount)
    {
#if ENGLISH
        ptr = StringCopy(ptr, gTrainerClassNames[trainer->trainerClass]);
#elif GERMAN
        ptr = StringCopy(ptr, GetGermanTrainerClassName(0, gPokenavStructPtr->trainersEye[listIndex].opponentId));
#endif

        ptr[0] = EXT_CTRL_CODE_BEGIN;
        ptr[1] = 0x13;
        ptr[2] = 0x4B;
        ptr += 3;
        ptr = StringCopy(ptr, trainer->trainerName);
    }

    ptr[0] = EXT_CTRL_CODE_BEGIN;
    ptr[1] = 0x13;
    ptr[2] = 0x80;
    ptr[3] = 0xFF;
}

void ChangeTrainerEyesDetailSelection(s8 delta)
{
    gPokenavStructPtr->listSelectedIndex += delta;
    if (gPokenavStructPtr->listSelectedIndex < 0)
    {
        gPokenavStructPtr->listSelectedIndex = gPokenavStructPtr->lastListIndex;
    }
    if (gPokenavStructPtr->listSelectedIndex > gPokenavStructPtr->lastListIndex)
    {
        gPokenavStructPtr->listSelectedIndex = 0;
    }
    gPokenavStructPtr->detailSelectionDelta = delta;
    gPokenavStructPtr->detailSelectionIndex = gPokenavStructPtr->listSelectedIndex;
    gPokenavStructPtr->detailTransitionState = 0;
}

bool8 UpdateTrainerEyesDetailSelection(void)
{
    switch (gPokenavStructPtr->detailTransitionState)
    {
    case 0:
        if (!SlideTrainerEyesPortraitOut())
            gPokenavStructPtr->detailTransitionState++;
        break;
    case 1:
        LoadTrainerEyesPortraitGfx(0);
        gPokenavStructPtr->detailTransitionState++;
        break;
    case 2:
        CreateOrUpdateTrainerEyesPortrait(0);
        gPokenavStructPtr->detailTransitionState++;
        break;
    case 3:
        BufferTrainerEyesListRowText(gPokenavStructPtr->pokenavTextBuffer, gPokenavStructPtr->listSelectedIndex);
        UpdateTrainerEyesNameSprites(gPokenavStructPtr->pokenavTextBuffer);
        RedrawSelectedTrainerEyesListRow();
        PrintTrainerEyesLocation(gPokenavStructPtr->listSelectedIndex);
        gPokenavStructPtr->detailTransitionState++;
        break;
    case 4:
        LoadTrainerEyesDescriptionLines();
        gPokenavStructPtr->trainerEyesDescriptionLoadStep = 0;
        gPokenavStructPtr->detailTransitionState++;
        // fallthrough
    case 5:
        if (gPokenavStructPtr->trainerEyesDescriptionLoadStep < 2){
            PrintTrainerEyesDescriptionStep();
            gPokenavStructPtr->trainerEyesDescriptionLoadStep++;
            break;
        }
        gPokenavStructPtr->detailTransitionState++;
        // fallthrough
    case 6:
        if (!SlideTrainerEyesPortraitIn() && !PrintTrainerEyesDescriptionStep())
            gPokenavStructPtr->detailTransitionState++;
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

void CalculateNumAdditionalSparkles(u8 monIndex)
{
    u8 sheen = GetMonData(&gPlayerParty[monIndex], MON_DATA_SHEEN);
    gPokenavStructPtr->conditionNumSparkles[gPokenavStructPtr->portraitSlot] = sheen != 255 ? sheen / 29 : 9;
}
