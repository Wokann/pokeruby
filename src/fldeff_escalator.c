#include "global.h"
#include "task.h"
#include "fieldmap.h"
#include "field_camera.h"
#include "field_player_avatar.h"
#include "constants/metatile_labels.h"

extern u8 gEscalatorAnimTaskId;

#define ESCALATOR_STAGES     3
#define LAST_ESCALATOR_STAGE (ESCALATOR_STAGES - 1)

#define tState            data[0]
#define tTransitionStage  data[1]
#define tGoingUp          data[2]
#define tDrawingEscalator data[3]
#define tPlayerX          data[4]
#define tPlayerY          data[5]

void SetEscalatorMetatile(u8 taskId, const s16 *metatileIds, u16 metatileMasks)
{
    s16 x = gTasks[taskId].tPlayerX - 1;
    s16 y = gTasks[taskId].tPlayerY - 1;
    s16 transitionStage = gTasks[taskId].tTransitionStage;
    s16 i;
    s16 j;
    
    if (gTasks[taskId].tGoingUp == 0)
    {
        for (i = 0; i < ESCALATOR_STAGES; i++)
        {
            for (j = 0; j < ESCALATOR_STAGES; j++)
            {
                s16 metatileId = MapGridGetMetatileIdAt(x + j, y + i);
                
                if (metatileIds[transitionStage] == metatileId)
                {
                    if (transitionStage != LAST_ESCALATOR_STAGE)
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[transitionStage + 1]);
                    else
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[0]);
                }
            }
        }
    }
    else
    {
        for (i = 0; i < ESCALATOR_STAGES; i++)
        {
            for (j = 0; j < ESCALATOR_STAGES; j++)
            {
                s16 metatileId = MapGridGetMetatileIdAt(x + j, y + i);
                
                if (metatileIds[LAST_ESCALATOR_STAGE - transitionStage] == metatileId)
                {
                    if (transitionStage != LAST_ESCALATOR_STAGE)
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[1 - transitionStage]);
                    else
                        MapGridSetMetatileIdAt(x + j, y + i, metatileMasks | metatileIds[LAST_ESCALATOR_STAGE]);
                }
            }
        }
    }
}

const u16 sEscalatorMetatiles_1F_0[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile0_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile0_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile0_Frame0)
};

const u16 sEscalatorMetatiles_1F_1[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile1_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile1_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile1_Frame0)
};

const u16 sEscalatorMetatiles_1F_2[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile2_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile2_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile2_Frame0)
};

const u16 sEscalatorMetatiles_1F_3[] = {
    METATILE_ID(PokemonCenter, Escalator1F_Tile3_Frame2),
    METATILE_ID(PokemonCenter, Escalator1F_Tile3_Frame1),
    METATILE_ID(PokemonCenter, Escalator1F_Tile3_Frame0)
};

const u16 sEscalatorMetatiles_2F_0[] = {
    METATILE_ID(PokemonCenter, Escalator2F_Tile0_Frame0),
    METATILE_ID(PokemonCenter, Escalator2F_Tile0_Frame1),
    METATILE_ID(PokemonCenter, Escalator2F_Tile0_Frame2)
};

const u16 sEscalatorMetatiles_2F_1[] = {
    METATILE_ID(PokemonCenter, Escalator2F_Tile1_Frame0),
    METATILE_ID(PokemonCenter, Escalator2F_Tile1_Frame1),
    METATILE_ID(PokemonCenter, Escalator2F_Tile1_Frame2)
};

const u16 sEscalatorMetatiles_2F_2[] = {
    METATILE_ID(PokemonCenter, Escalator2F_Tile2_Frame0),
    METATILE_ID(PokemonCenter, Escalator2F_Tile2_Frame1),
    METATILE_ID(PokemonCenter, Escalator2F_Tile2_Frame2)
};

void Task_DrawEscalator(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    tDrawingEscalator = TRUE;

    switch (tState)
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

    tState = (tState + 1) & 7;
    if (tState == 0)
    {
        DrawWholeMapView();
        tTransitionStage = (tTransitionStage + 1) % ESCALATOR_STAGES;
        tDrawingEscalator = FALSE;
    }
}

u8 CreateEscalatorTask(bool16 goingUp)
{
    u8 taskId = CreateTask(Task_DrawEscalator, 0);
    s16 *data = gTasks[taskId].data;

    PlayerGetDestCoords(&tPlayerX, &tPlayerY);
    tState = 0;
    tTransitionStage = 0;
    tGoingUp = goingUp;
    Task_DrawEscalator(taskId);
    return taskId;
}

void StartEscalator(bool8 goingUp)
{
    gEscalatorAnimTaskId = CreateEscalatorTask(goingUp);
}

void StopEscalator(void)
{
    DestroyTask(gEscalatorAnimTaskId);
}

bool8 IsEscalatorMoving(void)
{
    if (gTasks[gEscalatorAnimTaskId].tDrawingEscalator == FALSE && gTasks[gEscalatorAnimTaskId].tTransitionStage == LAST_ESCALATOR_STAGE)
        return FALSE;
    else
        return TRUE;
}

#undef tState
#undef tTransitionStage
#undef tGoingUp
#undef tDrawingEscalator
#undef tPlayerX
#undef tPlayerY
