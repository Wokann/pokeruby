#include "global.h"
#include "fldeff.h"
#include "task.h"
#include "fieldmap.h"
#include "field_camera.h"
#include "field_player_avatar.h"
#include "constants/metatile_labels.h"

extern u8 gEscalatorAnim_TaskId;

static void SetEscalatorMetatile(u8 taskId, const s16 *metatileIds, u16 metatileMasks)
{
    s16 x = gTasks[taskId].data[4] - 1;
    s16 y = gTasks[taskId].data[5] - 1;
    s16 transitionStage = gTasks[taskId].data[1];
    s16 i;
    s16 j;
    
    if (gTasks[taskId].data[2] == 0)
    {
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                s16 metatileId = MapGridGetMetatileIdAt(x + j, y + i);
                
                if (metatileIds[transitionStage] == metatileId)
                {
                    if (transitionStage != 2)
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[transitionStage + 1]);
                    else
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[0]);
                }
            }
        }
    }
    else
    {
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                s16 metatileId = MapGridGetMetatileIdAt(x + j, y + i);
                
                if (metatileIds[2 - transitionStage] == metatileId)
                {
                    if (transitionStage != 2)
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[1 - transitionStage]);
                    else
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[2]);
                }
            }
        }
    }
}

static const u16 sEscalatorMetatiles_1F_0[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile0_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile0_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile0_Frame0)
};

static const u16 sEscalatorMetatiles_1F_1[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile1_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile1_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile1_Frame0)
};

static const u16 sEscalatorMetatiles_1F_2[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile2_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile2_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile2_Frame0)
};

static const u16 sEscalatorMetatiles_1F_3[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile3_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile3_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile3_Frame0)
};

static const u16 sEscalatorMetatiles_2F_0[] = {
    METATILE_ID(PokemonCenter, Escalator2F_Tile0_Frame0),
    METATILE_ID(PokemonCenter, Escalator2F_Tile0_Frame1),
    METATILE_ID(PokemonCenter, Escalator2F_Tile0_Frame2)
};

static const u16 sEscalatorMetatiles_2F_1[] = {
    METATILE_ID(PokemonCenter, Escalator2F_Tile1_Frame0),
    METATILE_ID(PokemonCenter, Escalator2F_Tile1_Frame1),
    METATILE_ID(PokemonCenter, Escalator2F_Tile1_Frame2)
};

static const u16 sEscalatorMetatiles_2F_2[] = {
    METATILE_ID(PokemonCenter, Escalator2F_Tile2_Frame0),
    METATILE_ID(PokemonCenter, Escalator2F_Tile2_Frame1),
    METATILE_ID(PokemonCenter, Escalator2F_Tile2_Frame2)
};

static void Task_DrawEscalator(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    data[3] = 1;

    switch (data[0])
    {
        case 0:
            SetEscalatorMetatile(taskId, sEscalatorMetatiles_1F_0, 0);
            break;
        case 1:
            SetEscalatorMetatile(taskId, sEscalatorMetatiles_1F_1, 0);
            break;
        case 2:
            SetEscalatorMetatile(taskId, sEscalatorMetatiles_1F_2, MAPGRID_COLLISION_MASK);
            break;
        case 3:
            SetEscalatorMetatile(taskId, sEscalatorMetatiles_1F_3, 0);
            break;
        case 4:
            SetEscalatorMetatile(taskId, sEscalatorMetatiles_2F_0, MAPGRID_COLLISION_MASK);
            break;
        case 5:
            SetEscalatorMetatile(taskId, sEscalatorMetatiles_2F_1, 0);
            break;
        case 6:
            SetEscalatorMetatile(taskId, sEscalatorMetatiles_2F_2, 0);
            break;
    }

    data[0] = (data[0] + 1) & 7;
    if (!data[0])
    {
        DrawWholeMapView();
        data[1] = (data[1] + 1) % 3;
        data[3] = 0;
    }
}

u8 CreateEscalatorTask(bool16 goingUp)
{
    u8 taskId = CreateTask(Task_DrawEscalator, 0);
    s16 *data = gTasks[taskId].data;

    PlayerGetDestCoords(&data[4], &data[5]);
    data[0] = 0;
    data[1] = 0;
    data[2] = goingUp;
    Task_DrawEscalator(taskId);
    return taskId;
}

void StartEscalator(bool8 goingUp)
{
    gEscalatorAnim_TaskId = CreateEscalatorTask(goingUp);
}

void StopEscalator(void)
{
    DestroyTask(gEscalatorAnim_TaskId);
}

bool8 IsEscalatorMoving(void)
{
    if (gTasks[gEscalatorAnim_TaskId].data[3] == 0 && gTasks[gEscalatorAnim_TaskId].data[1] == 2)
        return FALSE;
    else
        return TRUE;
}
