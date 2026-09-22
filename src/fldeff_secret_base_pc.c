#include "global.h"
#include "event_data.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "constants/field_effects.h"
#include "constants/metatile_labels.h"
#include "constants/songs.h"

static void Task_SecretBasePCTurnOn(u8);

#define tX     data[0]
#define tY     data[1]
#define tState data[2]

bool8 FldEff_SecretBasePCTurnOn(void)
{
    s16 x, y;
    u8 taskId;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    taskId = CreateTask(Task_SecretBasePCTurnOn, 0);
    gTasks[taskId].tX = x;
    gTasks[taskId].tY = y;
    gTasks[taskId].tState = 0;

    return FALSE;
}

static void Task_SecretBasePCTurnOn(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 4:
    case 12:
        MapGridSetMetatileIdAt(tX, tY, METATILE_SecretBase_PC_On);
        CurrentMapDrawMetatileAt(tX, tY);
        break;
    case 8:
    case 16:
        MapGridSetMetatileIdAt(tX, tY, METATILE_SecretBase_PC);
        CurrentMapDrawMetatileAt(tX, tY);
        break;
    case 20:
        MapGridSetMetatileIdAt(tX, tY, METATILE_SecretBase_PC_On);
        CurrentMapDrawMetatileAt(tX, tY);
        FieldEffectActiveListRemove(FLDEFF_PCTURN_ON);
        ScriptContext_Enable();
        DestroyTask(taskId);
        return;
    }

    tState++;
}

#undef tX
#undef tY
#undef tState

void DoSecretBasePCTurnOffEffect(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    PlaySE(SE_PC_OFF);

    if (!VarGet(VAR_CURRENT_SECRET_BASE))
        MapGridSetMetatileIdAt(x, y, METATILE_SecretBase_PC | MAPGRID_COLLISION_MASK);
    else
        MapGridSetMetatileIdAt(x, y, METATILE_SecretBase_RegisterPC | MAPGRID_COLLISION_MASK);

    CurrentMapDrawMetatileAt(x, y);
}
