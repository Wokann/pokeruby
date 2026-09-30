#include "global.h"
#include "field_door.h"
#include "field_camera.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "task.h"
#include "constants/songs.h"

#define DOOR_SOUND_NORMAL  0
#define DOOR_SOUND_SLIDING 1

const u8 sDoorAnimTiles_Littleroot[][0x100] =
{
    INCBIN_U8("graphics/door_anims/04/0.4bpp"),
    INCBIN_U8("graphics/door_anims/04/1.4bpp"),
    INCBIN_U8("graphics/door_anims/04/2.4bpp"),
};

// TODO: Make these blank palette includes?
asm(".space 32");

const u8 sDoorAnimTiles_BirchsLab[][0x100] =
{
    INCBIN_U8("graphics/door_anims/05/0.4bpp"),
    INCBIN_U8("graphics/door_anims/05/1.4bpp"),
    INCBIN_U8("graphics/door_anims/05/2.4bpp"),
};

const u8 sDoorAnimTiles_FallarborLightRoof[][0x100] =
{
    INCBIN_U8("graphics/door_anims/08/0.4bpp"),
    INCBIN_U8("graphics/door_anims/08/1.4bpp"),
    INCBIN_U8("graphics/door_anims/08/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Lilycove[][0x100] =
{
    INCBIN_U8("graphics/door_anims/15/0.4bpp"),
    INCBIN_U8("graphics/door_anims/15/1.4bpp"),
    INCBIN_U8("graphics/door_anims/15/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_LilycoveWooden[][0x100] =
{
    INCBIN_U8("graphics/door_anims/16/0.4bpp"),
    INCBIN_U8("graphics/door_anims/16/1.4bpp"),
    INCBIN_U8("graphics/door_anims/16/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_General[][0x100] =
{
    INCBIN_U8("graphics/door_anims/00/0.4bpp"),
    INCBIN_U8("graphics/door_anims/00/1.4bpp"),
    INCBIN_U8("graphics/door_anims/00/2.4bpp"),
};

const u8 sDoorAnimTiles_PokeCenter[][0x100] =
{
    INCBIN_U8("graphics/door_anims/01/0.4bpp"),
    INCBIN_U8("graphics/door_anims/01/1.4bpp"),
    INCBIN_U8("graphics/door_anims/01/2.4bpp"),
};

const u8 sDoorAnimTiles_Gym[][0x100] =
{
    INCBIN_U8("graphics/door_anims/02/0.4bpp"),
    INCBIN_U8("graphics/door_anims/02/1.4bpp"),
    INCBIN_U8("graphics/door_anims/02/2.4bpp"),
};

const u8 sDoorAnimTiles_PokeMart[][0x100] =
{
    INCBIN_U8("graphics/door_anims/03/0.4bpp"),
    INCBIN_U8("graphics/door_anims/03/1.4bpp"),
    INCBIN_U8("graphics/door_anims/03/2.4bpp"),
};

const u8 sDoorAnimTiles_RustboroTan[][0x100] =
{
    INCBIN_U8("graphics/door_anims/06/0.4bpp"),
    INCBIN_U8("graphics/door_anims/06/1.4bpp"),
    INCBIN_U8("graphics/door_anims/06/2.4bpp"),
};

const u8 sDoorAnimTiles_RustboroGray[][0x100] =
{
    INCBIN_U8("graphics/door_anims/07/0.4bpp"),
    INCBIN_U8("graphics/door_anims/07/1.4bpp"),
    INCBIN_U8("graphics/door_anims/07/2.4bpp"),
};

const u8 sDoorAnimTiles_Oldale[][0x100] =
{
    INCBIN_U8("graphics/door_anims/09/0.4bpp"),
    INCBIN_U8("graphics/door_anims/09/1.4bpp"),
    INCBIN_U8("graphics/door_anims/09/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Mauville[][0x100] =
{
    INCBIN_U8("graphics/door_anims/10/0.4bpp"),
    INCBIN_U8("graphics/door_anims/10/1.4bpp"),
    INCBIN_U8("graphics/door_anims/10/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Verdanturf[][0x100] =
{
    INCBIN_U8("graphics/door_anims/11/0.4bpp"),
    INCBIN_U8("graphics/door_anims/11/1.4bpp"),
    INCBIN_U8("graphics/door_anims/11/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Slateport[][0x100] =
{
    INCBIN_U8("graphics/door_anims/12/0.4bpp"),
    INCBIN_U8("graphics/door_anims/12/1.4bpp"),
    INCBIN_U8("graphics/door_anims/12/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Dewford[][0x100] =
{
    INCBIN_U8("graphics/door_anims/13/0.4bpp"),
    INCBIN_U8("graphics/door_anims/13/1.4bpp"),
    INCBIN_U8("graphics/door_anims/13/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Contest[][0x100] =
{
    INCBIN_U8("graphics/door_anims/14/0.4bpp"),
    INCBIN_U8("graphics/door_anims/14/1.4bpp"),
    INCBIN_U8("graphics/door_anims/14/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Mossdeep[][0x100] =
{
    INCBIN_U8("graphics/door_anims/17/0.4bpp"),
    INCBIN_U8("graphics/door_anims/17/1.4bpp"),
    INCBIN_U8("graphics/door_anims/17/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_SootopolisPeakedRoof[][0x100] =
{
    INCBIN_U8("graphics/door_anims/18/0.4bpp"),
    INCBIN_U8("graphics/door_anims/18/1.4bpp"),
    INCBIN_U8("graphics/door_anims/18/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Sootopolis[][0x100] =
{
    INCBIN_U8("graphics/door_anims/19/0.4bpp"),
    INCBIN_U8("graphics/door_anims/19/1.4bpp"),
    INCBIN_U8("graphics/door_anims/19/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_PokemonLeague[][0x100] =
{
    INCBIN_U8("graphics/door_anims/20/0.4bpp"),
    INCBIN_U8("graphics/door_anims/20/1.4bpp"),
    INCBIN_U8("graphics/door_anims/20/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_Pacifidlog[][0x100] =
{
    INCBIN_U8("graphics/door_anims/21/0.4bpp"),
    INCBIN_U8("graphics/door_anims/21/1.4bpp"),
    INCBIN_U8("graphics/door_anims/21/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_PetalburgGym[][0x100] =
{
    INCBIN_U8("graphics/door_anims/22/0.4bpp"),
    INCBIN_U8("graphics/door_anims/22/1.4bpp"),
    INCBIN_U8("graphics/door_anims/22/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_CyclingRoad[][0x100] =
{
    INCBIN_U8("graphics/door_anims/23/0.4bpp"),
    INCBIN_U8("graphics/door_anims/23/1.4bpp"),
    INCBIN_U8("graphics/door_anims/23/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_LilycoveDeptStore[][0x100] =
{
    INCBIN_U8("graphics/door_anims/24/0.4bpp"),
    INCBIN_U8("graphics/door_anims/24/1.4bpp"),
    INCBIN_U8("graphics/door_anims/24/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_SafariZone[][0x100] =
{
    INCBIN_U8("graphics/door_anims/25/0.4bpp"),
    INCBIN_U8("graphics/door_anims/25/1.4bpp"),
    INCBIN_U8("graphics/door_anims/25/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_MossdeepSpaceCenter[][0x100] =
{
    INCBIN_U8("graphics/door_anims/26/0.4bpp"),
    INCBIN_U8("graphics/door_anims/26/1.4bpp"),
    INCBIN_U8("graphics/door_anims/26/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_CableClub[][0x100] =
{
    INCBIN_U8("graphics/door_anims/27/0.4bpp"),
    INCBIN_U8("graphics/door_anims/27/1.4bpp"),
    INCBIN_U8("graphics/door_anims/27/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_AbandonedShip[][0x100] =
{
    INCBIN_U8("graphics/door_anims/28/0.4bpp"),
    INCBIN_U8("graphics/door_anims/28/1.4bpp"),
    INCBIN_U8("graphics/door_anims/28/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_FallarborDarkRoof[][0x100] =
{
    INCBIN_U8("graphics/door_anims/29/0.4bpp"),
    INCBIN_U8("graphics/door_anims/29/1.4bpp"),
    INCBIN_U8("graphics/door_anims/29/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_AbandonedShipRoom[][0x100] =
{
    INCBIN_U8("graphics/door_anims/30/0.4bpp"),
    INCBIN_U8("graphics/door_anims/30/1.4bpp"),
    INCBIN_U8("graphics/door_anims/30/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_LilycoveDeptStoreElevator[][0x100] =
{
    INCBIN_U8("graphics/door_anims/31/0.4bpp"),
    INCBIN_U8("graphics/door_anims/31/1.4bpp"),
    INCBIN_U8("graphics/door_anims/31/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_BattleTowerOld[][0x100] =
{
    INCBIN_U8("graphics/door_anims/32/0.4bpp"),
    INCBIN_U8("graphics/door_anims/32/1.4bpp"),
    INCBIN_U8("graphics/door_anims/32/2.4bpp"),
};

asm(".space 32");

const u8 sDoorAnimTiles_BattleTowerElevator[][0x100] =
{
    INCBIN_U8("graphics/door_anims/33/0.4bpp"),
    INCBIN_U8("graphics/door_anims/33/1.4bpp"),
    INCBIN_U8("graphics/door_anims/33/2.4bpp"),
};

asm(".space 32");

static const struct DoorAnimFrame sDoorOpenAnimFrames[] =
{
    {4, -1},
    {4, 0},
    {4, 0x100},
    {4, 0x200},
    {0, 0},
};

static const struct DoorAnimFrame sDoorCloseAnimFrames[] =
{
    {4, 0x200},
    {4, 0x100},
    {4, 0},
    {4, -1},
    {0, 0},
};

const u8 sDoorAnimPalettes_General[] = {1, 1, 1, 1, 1, 1, 1, 1}; // door 00
const u8 sDoorAnimPalettes_PokeCenter[] = {1, 1, 1, 1, 1, 1, 1, 1}; // door 01
const u8 sDoorAnimPalettes_Gym[] = {5, 5, 5, 5, 5, 5, 5, 5}; // door 02
const u8 sDoorAnimPalettes_PokeMart[] = {0, 0, 1, 1, 1, 1, 1, 1}; // door 03
const u8 sDoorAnimPalettes_Littleroot[] = {6, 6, 6, 6, 6, 6, 6, 6}; // door 04
const u8 sDoorAnimPalettes_BirchsLab[] = {8, 8, 8, 8, 8, 8, 8, 8}; // door 05
const u8 sDoorAnimPalettes_RustboroTan[] = {11, 11, 11, 11, 11, 11, 11, 11}; // door 06
const u8 sDoorAnimPalettes_RustboroGray[] = {10, 10, 10, 10, 10, 10, 10, 10}; // door 07
const u8 sDoorAnimPalettes_FallarborLightRoof[] = {7, 7, 7, 7, 7, 7, 7, 7}; // door 08
const u8 sDoorAnimPalettes_Lilycove[] = {8, 8, 8, 8, 8, 8, 8, 8}; // door 15
const u8 sDoorAnimPalettes_Oldale[] = {9, 9, 9, 9, 9, 9, 9, 9}; // door 09
const u8 sDoorAnimPalettes_Mossdeep[] = {9, 9, 1, 1, 1, 1, 1, 1}; // door 17
const u8 sDoorAnimPalettes_PokemonLeague[] = {8, 8, 8, 8, 8, 8, 8, 8}; // door 20
const u8 sDoorAnimPalettes_Pacifidlog[] = {9, 9, 9, 9, 9, 9, 9, 9}; // door 21
const u8 sDoorAnimPalettes_SootopolisPeakedRoof[] = {6, 6, 6, 6, 6, 6, 6, 6}; // door 18
const u8 sDoorAnimPalettes_Sootopolis[] = {6, 6, 6, 6, 6, 6, 6, 6}; // door 19
const u8 sDoorAnimPalettes_Dewford[] = {0, 0, 5, 5, 5, 5, 5, 5}; // door 13
const u8 sDoorAnimPalettes_Slateport[] = {6, 6, 1, 1, 1, 1, 1, 1}; // door 12
const u8 sDoorAnimPalettes_Mauville[] = {7, 7, 7, 7, 7, 7, 7, 7}; // door 10
const u8 sDoorAnimPalettes_Verdanturf[] = {6, 6, 5, 5, 5, 5, 5, 5}; // door 11
const u8 sDoorAnimPalettes_LilycoveWooden[] = {5, 5, 5, 5, 5, 5, 5, 5}; // door 16
const u8 sDoorAnimPalettes_Contest[] = {1, 1, 1, 1, 1, 1, 1, 1}; // door 14
const u8 sDoorAnimPalettes_PetalburgGym[] = {6, 6, 7, 7, 7, 7, 7, 7}; // door 22
const u8 sDoorAnimPalettes_CyclingRoad[] = {7, 7, 7, 7, 7, 7, 7, 7}; // door 23
const u8 sDoorAnimPalettes_LilycoveDeptStore[] = {5, 5, 5, 5, 5, 5, 5, 5}; // door 24
const u8 sDoorAnimPalettes_SafariZone[] = {9, 9, 9, 9, 9, 9, 9, 9}; // door 25
const u8 sDoorAnimPalettes_MossdeepSpaceCenter[] = {8, 8, 8, 8, 8, 8, 8, 8}; // door 26
const u8 sDoorAnimPalettes_CableClub[] = {6, 6, 6, 6, 6, 6, 6, 6}; // door 27
const u8 sDoorAnimPalettes_AbandonedShip[] = {7, 7, 7, 7, 7, 7, 7, 7}; // door 28
const u8 sDoorAnimPalettes_FallarborDarkRoof[] = {11, 11, 7, 7, 7, 7, 7, 7}; // door 29
const u8 sDoorAnimPalettes_AbandonedShipRoom[] = {7, 7, 7, 7, 7, 7, 7, 7}; // door 30
const u8 sDoorAnimPalettes_LilycoveDeptStoreElevator[] = {6, 6, 7, 7, 7, 7, 7, 7}; // door 31
const u8 sDoorAnimPalettes_BattleTowerOld[] = {9, 9, 9, 9, 9, 9, 9, 9}; // door 32
const u8 sDoorAnimPalettes_BattleTowerElevator[] = {7, 7, 7, 7, 7, 7, 7, 7}; // door 33

static const struct DoorGraphics sDoorAnimGraphicsTable[] =
{
    {0x021, DOOR_SOUND_NORMAL, sDoorAnimTiles_General, sDoorAnimPalettes_General}, // door 00
    {0x061, DOOR_SOUND_SLIDING, sDoorAnimTiles_PokeCenter, sDoorAnimPalettes_PokeCenter}, // door 01
    {0x1CD, DOOR_SOUND_SLIDING, sDoorAnimTiles_Gym, sDoorAnimPalettes_Gym}, // door 02
    {0x041, DOOR_SOUND_SLIDING, sDoorAnimTiles_PokeMart, sDoorAnimPalettes_PokeMart}, // door 03
    {0x248, DOOR_SOUND_NORMAL, sDoorAnimTiles_Littleroot, sDoorAnimPalettes_Littleroot}, // door 04
    {0x249, DOOR_SOUND_NORMAL, sDoorAnimTiles_BirchsLab, sDoorAnimPalettes_BirchsLab}, // door 05
    {0x22F, DOOR_SOUND_NORMAL, sDoorAnimTiles_RustboroTan, sDoorAnimPalettes_RustboroTan}, // door 06
    {0x21F, DOOR_SOUND_NORMAL, sDoorAnimTiles_RustboroGray, sDoorAnimPalettes_RustboroGray}, // door 07
    {0x2A5, DOOR_SOUND_NORMAL, sDoorAnimTiles_FallarborLightRoof, sDoorAnimPalettes_FallarborLightRoof}, // door 08
    {0x287, DOOR_SOUND_NORMAL, sDoorAnimTiles_Oldale, sDoorAnimPalettes_Oldale}, // door 09
    {0x2AC, DOOR_SOUND_NORMAL, sDoorAnimTiles_Mauville, sDoorAnimPalettes_Mauville}, // door 10
    {0x3A1, DOOR_SOUND_NORMAL, sDoorAnimTiles_Verdanturf, sDoorAnimPalettes_Verdanturf}, // door 11
    {0x2DC, DOOR_SOUND_NORMAL, sDoorAnimTiles_Slateport, sDoorAnimPalettes_Slateport}, // door 12
    {0x225, DOOR_SOUND_NORMAL, sDoorAnimTiles_Dewford, sDoorAnimPalettes_Dewford}, // door 13
    {0x1DB, DOOR_SOUND_SLIDING, sDoorAnimTiles_Contest, sDoorAnimPalettes_Contest}, // door 14
    {0x246, DOOR_SOUND_NORMAL, sDoorAnimTiles_Lilycove, sDoorAnimPalettes_Lilycove}, // door 15
    {0x28E, DOOR_SOUND_NORMAL, sDoorAnimTiles_LilycoveWooden, sDoorAnimPalettes_LilycoveWooden}, // door 16
    {0x2A1, DOOR_SOUND_NORMAL, sDoorAnimTiles_Mossdeep, sDoorAnimPalettes_Mossdeep}, // door 17
    {0x21C, DOOR_SOUND_NORMAL, sDoorAnimTiles_SootopolisPeakedRoof, sDoorAnimPalettes_SootopolisPeakedRoof}, // door 18
    {0x21E, DOOR_SOUND_NORMAL, sDoorAnimTiles_Sootopolis, sDoorAnimPalettes_Sootopolis}, // door 19
    {0x21D, DOOR_SOUND_SLIDING, sDoorAnimTiles_PokemonLeague, sDoorAnimPalettes_PokemonLeague}, // door 20
    {0x21A, DOOR_SOUND_NORMAL, sDoorAnimTiles_Pacifidlog, sDoorAnimPalettes_Pacifidlog}, // door 21
    {0x224, DOOR_SOUND_NORMAL, sDoorAnimTiles_PetalburgGym, sDoorAnimPalettes_PetalburgGym}, // door 22
    {0x289, DOOR_SOUND_NORMAL, sDoorAnimTiles_CyclingRoad, sDoorAnimPalettes_CyclingRoad}, // door 23
    {0x30C, DOOR_SOUND_SLIDING, sDoorAnimTiles_LilycoveDeptStore, sDoorAnimPalettes_LilycoveDeptStore}, // door 24
    {0x32D, DOOR_SOUND_SLIDING, sDoorAnimTiles_SafariZone, sDoorAnimPalettes_SafariZone}, // door 25
    {0x2ED, DOOR_SOUND_SLIDING, sDoorAnimTiles_MossdeepSpaceCenter, sDoorAnimPalettes_MossdeepSpaceCenter}, // door 26
    {0x264, DOOR_SOUND_SLIDING, sDoorAnimTiles_CableClub, sDoorAnimPalettes_CableClub}, // door 27
    {0x22B, DOOR_SOUND_NORMAL, sDoorAnimTiles_AbandonedShip, sDoorAnimPalettes_AbandonedShip}, // door 28
    {0x2F7, DOOR_SOUND_NORMAL, sDoorAnimTiles_FallarborDarkRoof, sDoorAnimPalettes_FallarborDarkRoof}, // door 29
    {0x297, DOOR_SOUND_NORMAL, sDoorAnimTiles_AbandonedShipRoom, sDoorAnimPalettes_AbandonedShipRoom}, // door 30
    {0x285, DOOR_SOUND_SLIDING, sDoorAnimTiles_LilycoveDeptStoreElevator, sDoorAnimPalettes_LilycoveDeptStoreElevator}, // door 31
    {0x25D, DOOR_SOUND_SLIDING, sDoorAnimTiles_BattleTowerOld, sDoorAnimPalettes_BattleTowerOld}, // door 32
    {0x20E, DOOR_SOUND_SLIDING, sDoorAnimTiles_BattleTowerElevator, sDoorAnimPalettes_BattleTowerElevator}, // door 33
    {0, 0, NULL, NULL},
};

static void CopyDoorTilesToVram(const void *src)
{
    CpuFastSet(src, (void *)(VRAM + 0x7F00), 0x40);
}

static void BuildDoorTiles(u16 *tiles, u16 tileNum, const u8 *paletteNums)
{
    int i;
    u16 tile;

    for (i = 0; i < 4; i++)
    {
        tile = *(paletteNums++) << 12;
        tiles[i] = tile | (tileNum + i);
    }
    for (; i < 8; i++)
    {
        tile = *(paletteNums++) << 12;
        tiles[i] = tile;
    }
}

static void DrawCurrentDoorAnimFrame(u32 x, u32 y, const u8 *paletteNums)
{
    u16 tiles[8];

    BuildDoorTiles(tiles, 0x3F8, paletteNums);
    DrawDoorMetatileAt(x, y - 1, tiles);
    BuildDoorTiles(tiles, 0x3FC, paletteNums + 4);
    DrawDoorMetatileAt(x, y, tiles);
}

static void DrawClosedDoorTiles(u32 x, u32 y)
{
    CurrentMapDrawMetatileAt(x, y - 1);
    CurrentMapDrawMetatileAt(x, y);
}

static void DrawDoor(const struct DoorGraphics *gfx, const struct DoorAnimFrame *frame, u32 x, u32 y)
{
    if (frame->offset == 0xFFFF)
        DrawClosedDoorTiles(x, y);
    else
    {
        CopyDoorTilesToVram(gfx->tiles + frame->offset);
        DrawCurrentDoorAnimFrame(x, y, gfx->palettes);
    }
}

enum
{
    TD_FRAMELIST = 0,
    TD_GFX = 2,
    TD_FRAME = 4,
    TD_COUNTER,
    TD_X,
    TD_Y
};

static bool32 AnimateDoorFrame(struct DoorGraphics *gfx, struct DoorAnimFrame *frames, s16 *taskData)
{
    if (taskData[TD_COUNTER] == 0)
        DrawDoor(gfx, &frames[taskData[TD_FRAME]], taskData[TD_X], taskData[TD_Y]);
    if (taskData[TD_COUNTER] == frames[taskData[TD_FRAME]].time)
    {
        taskData[TD_COUNTER] = 0;
        taskData[TD_FRAME]++;
        if (frames[taskData[TD_FRAME]].time == 0)
            return FALSE;
        else
            return TRUE;
    }
    taskData[TD_COUNTER]++;
    return TRUE;
}

static void Task_AnimateDoor(u8 taskId)
{
    u16 *taskData = gTasks[taskId].data;
    struct DoorAnimFrame *frames = (struct DoorAnimFrame *)(taskData[TD_FRAMELIST] << 16 | taskData[TD_FRAMELIST + 1]);
    struct DoorGraphics *gfx = (struct DoorGraphics *)(taskData[TD_GFX] << 16 | taskData[TD_GFX + 1]);

    if (AnimateDoorFrame(gfx, frames, taskData) == FALSE)
        DestroyTask(taskId);
}

static const struct DoorAnimFrame *GetLastDoorFrame(const struct DoorAnimFrame *frame, const void *unused)
{
    while (frame->time != 0)
        frame++;
    return frame - 1;
}

static const struct DoorGraphics *GetDoorGraphics(const struct DoorGraphics *gfx, u16 metatileNum)
{
    while (gfx->tiles != NULL)
    {
        if (gfx->metatileNum == metatileNum)
            return gfx;
        gfx++;
    }
    return NULL;
}

static s8 StartDoorAnimationTask(const struct DoorGraphics *gfx, const struct DoorAnimFrame *frames, u32 x, u32 y)
{
    if (FuncIsActiveTask(Task_AnimateDoor) == TRUE)
        return -1;
    else
    {
        u8 taskId = CreateTask(Task_AnimateDoor, 0x50);
        s16 *taskData = gTasks[taskId].data;

        taskData[TD_X] = x;
        taskData[TD_Y] = y;

        taskData[TD_FRAMELIST + 1] = (u32)frames;
        taskData[TD_FRAMELIST] = (u32)frames >> 16;

        taskData[TD_GFX + 1] = (u32)gfx;
        taskData[TD_GFX] = (u32)gfx >> 16;

        return taskId;
    }
}

static void DrawClosedDoor(const struct DoorGraphics *gfx, u32 x, u32 y)
{
    DrawClosedDoorTiles(x, y);
}

static void DrawOpenedDoor(const struct DoorGraphics *gfx, u32 x, u32 y)
{
    gfx = GetDoorGraphics(gfx, MapGridGetMetatileIdAt(x, y));
    if (gfx != NULL)
        DrawDoor(gfx, GetLastDoorFrame(sDoorOpenAnimFrames, sDoorOpenAnimFrames), x, y);
}

static s8 StartDoorOpenAnimation(const struct DoorGraphics *gfx, u32 x, u32 y)
{
    gfx = GetDoorGraphics(gfx, MapGridGetMetatileIdAt(x, y));
    if (gfx == NULL)
        return -1;
    else
        return StartDoorAnimationTask(gfx, sDoorOpenAnimFrames, x, y);
}

static s8 StartDoorCloseAnimation(const struct DoorGraphics *gfx, u32 x, u32 y)
{
    gfx = GetDoorGraphics(gfx, MapGridGetMetatileIdAt(x, y));
    if (gfx == NULL)
        return -1;
    else
        return StartDoorAnimationTask(gfx, sDoorCloseAnimFrames, x, y);
}

static s8 GetDoorSoundType(const struct DoorGraphics *gfx, u32 x, u32 y)
{
    gfx = GetDoorGraphics(gfx, MapGridGetMetatileIdAt(x, y));
    if (gfx == NULL)
        return -1;
    else
        return gfx->sound;
}

void Debug_FieldAnimateDoorOpen(u32 x, u32 y)
{
    StartDoorOpenAnimation(sDoorAnimGraphicsTable, x, y);
}

void FieldSetDoorOpened(u32 x, u32 y)
{
    if (MetatileBehavior_IsDoor(MapGridGetMetatileBehaviorAt(x, y)))
        DrawOpenedDoor(sDoorAnimGraphicsTable, x, y);
}

void FieldSetDoorClosed(u32 x, u32 y)
{
    if (MetatileBehavior_IsDoor(MapGridGetMetatileBehaviorAt(x, y)))
        DrawClosedDoor(sDoorAnimGraphicsTable, x, y);
}

s8 FieldAnimateDoorClose(u32 x, u32 y)
{
    if (!MetatileBehavior_IsDoor(MapGridGetMetatileBehaviorAt(x, y)))
        return -1;
    else
        return StartDoorCloseAnimation(sDoorAnimGraphicsTable, x, y);
}

s8 FieldAnimateDoorOpen(u32 x, u32 y)
{
    if (!MetatileBehavior_IsDoor(MapGridGetMetatileBehaviorAt(x, y)))
        return -1;
    else
        return StartDoorOpenAnimation(sDoorAnimGraphicsTable, x, y);
}

bool8 FieldIsDoorAnimationRunning(void)
{
    return FuncIsActiveTask(Task_AnimateDoor);
}

u32 GetDoorSoundEffect(u32 x, u32 y)
{
    if (GetDoorSoundType(sDoorAnimGraphicsTable, x, y) == DOOR_SOUND_NORMAL)
        return SE_DOOR;
    else
        return SE_SLIDING_DOOR;
}
