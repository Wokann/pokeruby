#include "global.h"
#include "fldeff_poison.h"
#include "task.h"
#include "constants/songs.h"
#include "sound.h"

#define tState  data[0]
#define tMosaic data[1]

static void Task_FieldPoisonEffect(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        tMosaic += 2;
        if (tMosaic > 8)
        {
            tState++;
        }
        break;
    case 1:
        tMosaic -= 2;
        if (tMosaic == 0)
        {
            tState++;
        }
        break;
    case 2:
        DestroyTask(taskId);
        return;
    }

    REG_MOSAIC = (tMosaic << 4) | tMosaic;
}

void FldEffPoison_Start(void)
{
    PlaySE(SE_FIELD_POISON);
    CreateTask(Task_FieldPoisonEffect, 80);
}

bool32 FldEffPoison_IsActive(void)
{
    return FuncIsActiveTask(Task_FieldPoisonEffect);
}

#undef tState
#undef tMosaic
