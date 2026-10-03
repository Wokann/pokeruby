#include "global.h"
#include "braille_puzzles.h"
#include "event_data.h"
#include "field_camera.h"
#include "field_effect.h"
#include "fieldmap.h"
#include "main.h"
#include "event_object_lock.h"
#include "menu.h"
#include "pokemon.h"
#include "pokemon_menu.h"
#include "fldeff.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "text.h"
#include "constants/field_effects.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/metatile_labels.h"

extern u8 IslandCave_EventScript_OpenRegiEntrance[]; // Regice event script

static void Task_SealedChamberShakingEffect(u8 taskId);
static void UseFlyAncientTomb_Callback(void);
static void UseFlyAncientTomb_Finish(void);
static void Task_BrailleWait(u8 taskId);
static bool32 BrailleWait_CheckButtonPress(void);

bool8 ShouldDoBrailleDigEffect(void)
{
    if (!FlagGet(FLAG_SYS_BRAILLE_DIG)
     && (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_SEALED_CHAMBER_OUTER_ROOM)
     && gSaveBlock1.location.mapNum == MAP_NUM(MAP_SEALED_CHAMBER_OUTER_ROOM)))
    {
        if (gSaveBlock1.pos.x == 10 && gSaveBlock1.pos.y == 3)
            return TRUE;
        else if (gSaveBlock1.pos.x == 9 && gSaveBlock1.pos.y == 3)
            return TRUE;
        else if (gSaveBlock1.pos.x == 11 && gSaveBlock1.pos.y == 3)
            return TRUE;
    }

    return FALSE;
}

void DoBrailleDigEffect(void)
{
    MapGridSetMetatileIdAt(9 + MAP_OFFSET, 1 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopLeft));
    MapGridSetMetatileIdAt(10 + MAP_OFFSET, 1 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopMid));
    MapGridSetMetatileIdAt(11 + MAP_OFFSET, 1 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopRight));
    MapGridSetMetatileIdAt(9 + MAP_OFFSET, 2 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomLeft) | MAPGRID_IMPASSABLE);
    MapGridSetMetatileIdAt(10 + MAP_OFFSET, 2 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomMid));
    MapGridSetMetatileIdAt(11 + MAP_OFFSET, 2 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomRight) | MAPGRID_IMPASSABLE);
    DrawWholeMapView();
    PlaySE(SE_BANG);
    FlagSet(FLAG_SYS_BRAILLE_DIG);
    UnlockPlayerFieldControls();
}

bool8 CheckRelicanthWailord(void)
{
    // First comes Relicanth.
    if (GetMonData(&gPlayerParty[0], MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_RELICANTH)
    {
        CalculatePlayerPartyCount();
        // Last comes Wailord
        if (GetMonData(&gPlayerParty[gPlayerPartyCount - 1], MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_WAILORD)
            return TRUE;
    }
    return FALSE;
}

bool8 ShouldDoBrailleStrengthEffect(void)
{
    if (!FlagGet(FLAG_SYS_BRAILLE_STRENGTH) && (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_DESERT_RUINS) && gSaveBlock1.location.mapNum == MAP_NUM(MAP_DESERT_RUINS)))
    {
        if (gSaveBlock1.pos.x == 10 && gSaveBlock1.pos.y == 23)
            return TRUE;
        else if (gSaveBlock1.pos.x == 9 && gSaveBlock1.pos.y == 23)
            return TRUE;
        else if (gSaveBlock1.pos.x == 11 && gSaveBlock1.pos.y == 23)
            return TRUE;
    }

    return FALSE;
}

void DoBrailleStrengthEffect(void)
{
    FieldEffectActiveListRemove(FLDEFF_USE_STRENGTH);
    MapGridSetMetatileIdAt(7 + MAP_OFFSET, 19 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopLeft));
    MapGridSetMetatileIdAt(8 + MAP_OFFSET, 19 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopMid));
    MapGridSetMetatileIdAt(9 + MAP_OFFSET, 19 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopRight));
    MapGridSetMetatileIdAt(7 + MAP_OFFSET, 20 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomLeft) | MAPGRID_IMPASSABLE);
    MapGridSetMetatileIdAt(8 + MAP_OFFSET, 20 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomMid));
    MapGridSetMetatileIdAt(9 + MAP_OFFSET, 20 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomRight) | MAPGRID_IMPASSABLE);
    DrawWholeMapView();
    PlaySE(SE_BANG);
    FlagSet(FLAG_SYS_BRAILLE_STRENGTH);
    UnlockPlayerFieldControls();
}

bool8 ShouldDoBrailleFlyEffect(void)
{
    if (!FlagGet(FLAG_SYS_BRAILLE_FLY) && (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_ANCIENT_TOMB) && gSaveBlock1.location.mapNum == MAP_NUM(MAP_ANCIENT_TOMB)))
    {
        if (gSaveBlock1.pos.x == 8 && gSaveBlock1.pos.y == 25)
            return TRUE;
    }

    return FALSE;
}

void DoBrailleFlyEffect(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    FieldEffectStart(FLDEFF_USE_FLY_ANCIENT_TOMB);
}

bool8 FldEff_UseFlyAncientTomb(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (u32)UseFlyAncientTomb_Callback >> 16;
    gTasks[taskId].data[9] = (u32)UseFlyAncientTomb_Callback;
    return FALSE;
}

static void UseFlyAncientTomb_Callback(void)
{
    FieldEffectActiveListRemove(FLDEFF_USE_FLY_ANCIENT_TOMB);
    UseFlyAncientTomb_Finish();
}

static void UseFlyAncientTomb_Finish(void)
{
    MapGridSetMetatileIdAt(7 + MAP_OFFSET, 19 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopLeft));
    MapGridSetMetatileIdAt(8 + MAP_OFFSET, 19 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopMid));
    MapGridSetMetatileIdAt(9 + MAP_OFFSET, 19 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_TopRight));
    MapGridSetMetatileIdAt(7 + MAP_OFFSET, 20 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomLeft) | MAPGRID_IMPASSABLE);
    MapGridSetMetatileIdAt(8 + MAP_OFFSET, 20 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomMid));
    MapGridSetMetatileIdAt(9 + MAP_OFFSET, 20 + MAP_OFFSET, METATILE_ID(Cave, SealedChamberEntrance_BottomRight) | MAPGRID_IMPASSABLE);
    DrawWholeMapView();
    PlaySE(SE_BANG);
    FlagSet(FLAG_SYS_BRAILLE_FLY);
    UnlockPlayerFieldControls();
}

enum
{
    BRAILLE_WAIT_STATE_INIT,
    BRAILLE_WAIT_STATE_WAIT_FOR_INPUT,
    BRAILLE_WAIT_STATE_WAIT_FOR_CANCEL,
    BRAILLE_WAIT_STATE_COMPLETE_DELAY,
    BRAILLE_WAIT_STATE_OPEN_ENTRANCE,
};

#define tBrailleWaitState data[0]
#define tBrailleWaitTimer data[1]

#define BRAILLE_WAIT_TASK_PRIORITY 0x50
#define BRAILLE_WAIT_DURATION      (2 * 60 * 60)
#define BRAILLE_WAIT_CANCEL_DELAY  30

void DoBrailleWait(void)
{
    if (!FlagGet(FLAG_SYS_BRAILLE_WAIT))
        CreateTask(Task_BrailleWait, BRAILLE_WAIT_TASK_PRIORITY);
}

static void Task_BrailleWait(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tBrailleWaitState)
    {
    case BRAILLE_WAIT_STATE_INIT:
        tBrailleWaitTimer = BRAILLE_WAIT_DURATION;
        tBrailleWaitState = BRAILLE_WAIT_STATE_WAIT_FOR_INPUT;
        break;
    case BRAILLE_WAIT_STATE_WAIT_FOR_INPUT:
        if (BrailleWait_CheckButtonPress() != FALSE)
        {
            Menu_EraseScreen();
            PlaySE(SE_SELECT);
            tBrailleWaitState = BRAILLE_WAIT_STATE_WAIT_FOR_CANCEL;
        }
        else
        {
            tBrailleWaitTimer--;
            if (tBrailleWaitTimer == 0)
            {
                Menu_EraseScreen();
                tBrailleWaitState = BRAILLE_WAIT_STATE_COMPLETE_DELAY;
                tBrailleWaitTimer = BRAILLE_WAIT_CANCEL_DELAY;
            }
        }
        break;
    case BRAILLE_WAIT_STATE_WAIT_FOR_CANCEL:
        if (BrailleWait_CheckButtonPress() == FALSE)
        {
            tBrailleWaitTimer--;
            if (tBrailleWaitTimer == 0)
                tBrailleWaitState = BRAILLE_WAIT_STATE_OPEN_ENTRANCE;
            break;
        }
        ScriptUnfreezeObjectEvents();
        DestroyTask(taskId);
        UnlockPlayerFieldControls();
        break;
    case BRAILLE_WAIT_STATE_COMPLETE_DELAY:
        tBrailleWaitTimer--;
        if (tBrailleWaitTimer == 0)
            tBrailleWaitState = BRAILLE_WAIT_STATE_OPEN_ENTRANCE;
        break;
    case BRAILLE_WAIT_STATE_OPEN_ENTRANCE:
        ScriptUnfreezeObjectEvents();
        ScriptContext_SetupScript(IslandCave_EventScript_OpenRegiEntrance);
        DestroyTask(taskId);
        break;
    }
}

static bool32 BrailleWait_CheckButtonPress(void)
{
    u16 keyMask = A_BUTTON | B_BUTTON | START_BUTTON | SELECT_BUTTON | DPAD_ANY;

    if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR)
        keyMask |= L_BUTTON | R_BUTTON;
    if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
        keyMask |= L_BUTTON;

    if (JOY_NEW(keyMask))
        return TRUE;
    else
        return FALSE;
}

#undef tBrailleWaitState
#undef tBrailleWaitTimer
#undef BRAILLE_WAIT_TASK_PRIORITY
#undef BRAILLE_WAIT_DURATION
#undef BRAILLE_WAIT_CANCEL_DELAY

#define tDelayCounter  data[1]
#define tShakeCounter  data[2]
#define tVerticalPan   data[4]
#define tDelay         data[5]
#define tNumShakes     data[6]

void DoSealedChamberShakingEffect_Long(void)
{
    u8 taskId = CreateTask(Task_SealedChamberShakingEffect, 9);

    gTasks[taskId].tDelayCounter = 0;
    gTasks[taskId].tShakeCounter = 0;
    gTasks[taskId].tVerticalPan = 2;
    gTasks[taskId].tDelay = 5;
    gTasks[taskId].tNumShakes = 50;
    SetCameraPanningCallback(NULL);
}

void DoSealedChamberShakingEffect_Short(void)
{
    u8 taskId = CreateTask(Task_SealedChamberShakingEffect, 9);

    gTasks[taskId].tDelayCounter = 0;
    gTasks[taskId].tShakeCounter = 0;
    gTasks[taskId].tVerticalPan = 3;
    gTasks[taskId].tDelay = 5;
    gTasks[taskId].tNumShakes = 2;
    SetCameraPanningCallback(NULL);
}

static void Task_SealedChamberShakingEffect(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->tDelayCounter++;

    if (task->tDelayCounter % task->tDelay == 0)
    {
        task->tDelayCounter = 0;
        task->tShakeCounter++;
        task->tVerticalPan = -task->tVerticalPan;
        SetCameraPanning(0, task->tVerticalPan);
        if (task->tShakeCounter == task->tNumShakes)
        {
            DestroyTask(taskId);
            ScriptContext_Enable();
            InstallCameraPanAheadCallback();
        }
    }
}

#undef tDelayCounter
#undef tShakeCounter
#undef tVerticalPan
#undef tDelay
#undef tNumShakes
