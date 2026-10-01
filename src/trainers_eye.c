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

    gPokenavState->trainerEyesCount = 0;

    for (i = 0; i < REMATCH_TABLE_ENTRIES; i++)
    {
        if (HasTrainerBeenFought(gRematchTable[i].trainerIds[0]))
        {
            gPokenavState->trainersEye[gPokenavState->trainerEyesCount].opponentId = gRematchTable[i].trainerIds[0];
            gPokenavState->trainersEye[gPokenavState->trainerEyesCount].rematchTableIdx = i;
            gPokenavState->trainersEye[gPokenavState->trainerEyesCount].rematchNo = gSaveBlock1.trainerRematches[i];
            gPokenavState->trainersEye[gPokenavState->trainerEyesCount].regionMapSectionId = Overworld_GetMapHeaderByGroupAndId(gRematchTable[i].mapGroup, gRematchTable[i].mapNum)->regionMapSectionId;
            gPokenavState->trainerEyesCount++;
        }
    }

    for (i = 0; i < 13; i++)
    {
        {
            if (HasTrainerBeenFought(sGymLeaderTrainersEye[i].opponentId))
            {
                gPokenavState->trainersEye[gPokenavState->trainerEyesCount].opponentId = sGymLeaderTrainersEye[i].opponentId;
                gPokenavState->trainersEye[gPokenavState->trainerEyesCount].regionMapSectionId = sGymLeaderTrainersEye[i].regionMapSectionId;
                gPokenavState->trainersEye[gPokenavState->trainerEyesCount].rematchNo = 0;
                gPokenavState->trainersEye[gPokenavState->trainerEyesCount].rematchTableIdx = i + 0x38;
                gPokenavState->trainerEyesCount++;
            }
        }
    }

    gPokenavState->listCursorRow = 0;
    gPokenavState->firstVisibleListIndex = 0;
    gPokenavState->listSelectedIndex = 0;
    gPokenavState->lastVisibleListIndex = gPokenavState->trainerEyesCount <= 8 ? gPokenavState->trainerEyesCount - 1 : 7;
    gPokenavState->lastListIndex = gPokenavState->trainerEyesCount - 1;
    gPokenavState->hasListScrollArrows = gPokenavState->lastListIndex < 8 ? 0 : 1;
}

void PrepareTrainerEyesDetailPortrait(void)
{
    s32 portraitSlot = 0;
    gPokenavState->portraitSlot = portraitSlot;
    LoadTrainerEyesPortraitGfx(0);
    gPokenavState->trainerEyesPortraitXOffset = -72;
    CreateOrUpdateTrainerEyesPortrait(0);
    gPokenavState->detailSelectionIndex = gPokenavState->listSelectedIndex;
}

void LoadTrainerEyesPortraitGfx(u8 slot)
{
    u16 opponentId = gPokenavState->trainersEye[gPokenavState->listSelectedIndex].opponentId;
    u8 trainerPicId = gTrainers[opponentId].trainerPic;
    DecompressPicFromTable(&gTrainerFrontPicTable[trainerPicId], gTrainerFrontPicCoords[trainerPicId].size, 1, gPokenavState->spriteGfxScratchBuffer, gPokenavState->spriteGfxBuffers[slot], trainerPicId);
    LZ77UnCompWram(gTrainerFrontPicPaletteTable[trainerPicId].data, gPokenavState->portraitPaletteBuffers[slot]);
}

bool8 SlideTrainerEyesPortraitIn(void)
{
    if (gPokenavState->trainerEyesPortraitXOffset == 0)
    {
        return FALSE;
    }
    if ((gPokenavState->trainerEyesPortraitXOffset += 8) >= 0)
    {
        gPokenavState->trainerEyesPortraitXOffset = 0;
        return FALSE;
    }
    return TRUE;
}

bool8 SlideTrainerEyesPortraitOut(void)
{
    if (gPokenavState->trainerEyesPortraitXOffset == -72)
    {
        return FALSE;
    }
    if ((gPokenavState->trainerEyesPortraitXOffset -= 8) <= -72)
    {
        gPokenavState->trainerEyesPortraitXOffset = -72;
        return FALSE;
    }
    return TRUE;
}

void InitTrainerEyesBackgroundPulse(void)
{
    gPokenavState->trainerEyesPulseFrameCounter = 0;
    gPokenavState->trainerEyesPulseState = 0;
    REG_BLDCNT = BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_DARKEN;
    REG_BLDY = 0;
    REG_WININ = 0x3F3F;
    REG_WINOUT = 0x1F1F;
    SetTrainerEyesDetailWindow(0);
}

void UpdateTrainerEyesBackgroundPulse(void)
{
    if (++gPokenavState->trainerEyesPulseFrameCounter > 8)
    {
        gPokenavState->trainerEyesPulseFrameCounter = 0;
        ++gPokenavState->trainerEyesPulseState;
        if (gPokenavState->trainerEyesPulseState & 1)
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
    const struct Trainer *trainer = &gTrainers[gPokenavState->trainersEye[listIndex].opponentId];
    u8 *ptr = buffer;

    ptr = buffer;
    if (listIndex < gPokenavState->trainerEyesCount)
    {
#if ENGLISH
        ptr = StringCopy(ptr, gTrainerClassNames[trainer->trainerClass]);
#elif GERMAN
        ptr = StringCopy(ptr, GetGermanTrainerClassName(0, gPokenavState->trainersEye[listIndex].opponentId));
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
    gPokenavState->listSelectedIndex += delta;
    if (gPokenavState->listSelectedIndex < 0)
    {
        gPokenavState->listSelectedIndex = gPokenavState->lastListIndex;
    }
    if (gPokenavState->listSelectedIndex > gPokenavState->lastListIndex)
    {
        gPokenavState->listSelectedIndex = 0;
    }
    gPokenavState->detailSelectionDelta = delta;
    gPokenavState->detailSelectionIndex = gPokenavState->listSelectedIndex;
    gPokenavState->detailTransitionState = 0;
}

bool8 UpdateTrainerEyesDetailSelection(void)
{
    switch (gPokenavState->detailTransitionState)
    {
    case 0:
        if (!SlideTrainerEyesPortraitOut())
            gPokenavState->detailTransitionState++;
        break;
    case 1:
        LoadTrainerEyesPortraitGfx(0);
        gPokenavState->detailTransitionState++;
        break;
    case 2:
        CreateOrUpdateTrainerEyesPortrait(0);
        gPokenavState->detailTransitionState++;
        break;
    case 3:
        BufferTrainerEyesListRowText(gPokenavState->pokenavTextBuffer, gPokenavState->listSelectedIndex);
        UpdateTrainerEyesNameSprites(gPokenavState->pokenavTextBuffer);
        RedrawSelectedTrainerEyesListRow();
        PrintTrainerEyesLocation(gPokenavState->listSelectedIndex);
        gPokenavState->detailTransitionState++;
        break;
    case 4:
        LoadTrainerEyesDescriptionLines();
        gPokenavState->trainerEyesDescriptionLoadStep = 0;
        gPokenavState->detailTransitionState++;
        // fallthrough
    case 5:
        if (gPokenavState->trainerEyesDescriptionLoadStep < 2){
            PrintTrainerEyesDescriptionStep();
            gPokenavState->trainerEyesDescriptionLoadStep++;
            break;
        }
        gPokenavState->detailTransitionState++;
        // fallthrough
    case 6:
        if (!SlideTrainerEyesPortraitIn() && !PrintTrainerEyesDescriptionStep())
            gPokenavState->detailTransitionState++;
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

void CalculateNumAdditionalSparkles(u8 monIndex)
{
    u8 sheen = GetMonData(&gPlayerParty[monIndex], MON_DATA_SHEEN);
    gPokenavState->conditionNumSparkles[gPokenavState->portraitSlot] = sheen != 255 ? sheen / 29 : 9;
}
