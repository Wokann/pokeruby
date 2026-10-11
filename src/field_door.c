#include "global.h"
#include "field_door.h"
#include "field_camera.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "task.h"
#include "constants/songs.h"

struct DoorGraphics
{
    u16 metatileNum;
    u8 sound;
    const void *tiles;
    const void *palettes;
};

struct DoorAnimFrame
{
    u8 time;
    u16 offset;
};

const u8 gDoorAnimTiles_Littleroot[][0x100] =
{
    INCBIN_U8("graphics/door_anims/04/0.4bpp"),
    INCBIN_U8("graphics/door_anims/04/1.4bpp"),
    INCBIN_U8("graphics/door_anims/04/2.4bpp"),
};

// TODO: Make these blank palette includes?
asm(".space 32");

const u8 gDoorAnimTiles_BirchsLab[][0x100] =
{
    INCBIN_U8("graphics/door_anims/05/0.4bpp"),
    INCBIN_U8("graphics/door_anims/05/1.4bpp"),
    INCBIN_U8("graphics/door_anims/05/2.4bpp"),
};

const u8 gDoorAnimTiles_FallarborLightRoof[][0x100] =
{
    INCBIN_U8("graphics/door_anims/08/0.4bpp"),
    INCBIN_U8("graphics/door_anims/08/1.4bpp"),
    INCBIN_U8("graphics/door_anims/08/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Lilycove[][0x100] =
{
    INCBIN_U8("graphics/door_anims/15/0.4bpp"),
    INCBIN_U8("graphics/door_anims/15/1.4bpp"),
    INCBIN_U8("graphics/door_anims/15/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_LilycoveWooden[][0x100] =
{
    INCBIN_U8("graphics/door_anims/16/0.4bpp"),
    INCBIN_U8("graphics/door_anims/16/1.4bpp"),
    INCBIN_U8("graphics/door_anims/16/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_General[][0x100] =
{
    INCBIN_U8("graphics/door_anims/00/0.4bpp"),
    INCBIN_U8("graphics/door_anims/00/1.4bpp"),
    INCBIN_U8("graphics/door_anims/00/2.4bpp"),
};

static const u8 sDoorAnimTiles_PokeCenter[][0x100] =
{
    INCBIN_U8("graphics/door_anims/poke_center/0.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_center/1.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_center/2.4bpp"),
};

static const u8 sDoorAnimTiles_Gym[][0x100] =
{
    INCBIN_U8("graphics/door_anims/gym/0.4bpp"),
    INCBIN_U8("graphics/door_anims/gym/1.4bpp"),
    INCBIN_U8("graphics/door_anims/gym/2.4bpp"),
};

static const u8 sDoorAnimTiles_PokeMart[][0x100] =
{
    INCBIN_U8("graphics/door_anims/poke_mart/0.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_mart/1.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_mart/2.4bpp"),
};

const u8 gDoorAnimTiles_RustboroTan[][0x100] =
{
    INCBIN_U8("graphics/door_anims/06/0.4bpp"),
    INCBIN_U8("graphics/door_anims/06/1.4bpp"),
    INCBIN_U8("graphics/door_anims/06/2.4bpp"),
};

const u8 gDoorAnimTiles_RustboroGray[][0x100] =
{
    INCBIN_U8("graphics/door_anims/07/0.4bpp"),
    INCBIN_U8("graphics/door_anims/07/1.4bpp"),
    INCBIN_U8("graphics/door_anims/07/2.4bpp"),
};

const u8 gDoorAnimTiles_Oldale[][0x100] =
{
    INCBIN_U8("graphics/door_anims/09/0.4bpp"),
    INCBIN_U8("graphics/door_anims/09/1.4bpp"),
    INCBIN_U8("graphics/door_anims/09/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Mauville[][0x100] =
{
    INCBIN_U8("graphics/door_anims/10/0.4bpp"),
    INCBIN_U8("graphics/door_anims/10/1.4bpp"),
    INCBIN_U8("graphics/door_anims/10/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Verdanturf[][0x100] =
{
    INCBIN_U8("graphics/door_anims/11/0.4bpp"),
    INCBIN_U8("graphics/door_anims/11/1.4bpp"),
    INCBIN_U8("graphics/door_anims/11/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Slateport[][0x100] =
{
    INCBIN_U8("graphics/door_anims/12/0.4bpp"),
    INCBIN_U8("graphics/door_anims/12/1.4bpp"),
    INCBIN_U8("graphics/door_anims/12/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Dewford[][0x100] =
{
    INCBIN_U8("graphics/door_anims/13/0.4bpp"),
    INCBIN_U8("graphics/door_anims/13/1.4bpp"),
    INCBIN_U8("graphics/door_anims/13/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_Contest[][0x100] =
{
    INCBIN_U8("graphics/door_anims/contest/0.4bpp"),
    INCBIN_U8("graphics/door_anims/contest/1.4bpp"),
    INCBIN_U8("graphics/door_anims/contest/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Mossdeep[][0x100] =
{
    INCBIN_U8("graphics/door_anims/17/0.4bpp"),
    INCBIN_U8("graphics/door_anims/17/1.4bpp"),
    INCBIN_U8("graphics/door_anims/17/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_SootopolisPeakedRoof[][0x100] =
{
    INCBIN_U8("graphics/door_anims/18/0.4bpp"),
    INCBIN_U8("graphics/door_anims/18/1.4bpp"),
    INCBIN_U8("graphics/door_anims/18/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_Sootopolis[][0x100] =
{
    INCBIN_U8("graphics/door_anims/sootopolis/0.4bpp"),
    INCBIN_U8("graphics/door_anims/sootopolis/1.4bpp"),
    INCBIN_U8("graphics/door_anims/sootopolis/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_PokemonLeague[][0x100] =
{
    INCBIN_U8("graphics/door_anims/pokemon_league/0.4bpp"),
    INCBIN_U8("graphics/door_anims/pokemon_league/1.4bpp"),
    INCBIN_U8("graphics/door_anims/pokemon_league/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Pacifidlog[][0x100] =
{
    INCBIN_U8("graphics/door_anims/21/0.4bpp"),
    INCBIN_U8("graphics/door_anims/21/1.4bpp"),
    INCBIN_U8("graphics/door_anims/21/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_PetalburgGym[][0x100] =
{
    INCBIN_U8("graphics/door_anims/22/0.4bpp"),
    INCBIN_U8("graphics/door_anims/22/1.4bpp"),
    INCBIN_U8("graphics/door_anims/22/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_CyclingRoad[][0x100] =
{
    INCBIN_U8("graphics/door_anims/23/0.4bpp"),
    INCBIN_U8("graphics/door_anims/23/1.4bpp"),
    INCBIN_U8("graphics/door_anims/23/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_LilycoveDeptStore[][0x100] =
{
    INCBIN_U8("graphics/door_anims/24/0.4bpp"),
    INCBIN_U8("graphics/door_anims/24/1.4bpp"),
    INCBIN_U8("graphics/door_anims/24/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_SafariZone[][0x100] =
{
    INCBIN_U8("graphics/door_anims/25/0.4bpp"),
    INCBIN_U8("graphics/door_anims/25/1.4bpp"),
    INCBIN_U8("graphics/door_anims/25/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_MossdeepSpaceCenter[][0x100] =
{
    INCBIN_U8("graphics/door_anims/26/0.4bpp"),
    INCBIN_U8("graphics/door_anims/26/1.4bpp"),
    INCBIN_U8("graphics/door_anims/26/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_CableClub[][0x100] =
{
    INCBIN_U8("graphics/door_anims/cable_club/0.4bpp"),
    INCBIN_U8("graphics/door_anims/cable_club/1.4bpp"),
    INCBIN_U8("graphics/door_anims/cable_club/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_AbandonedShip[][0x100] =
{
    INCBIN_U8("graphics/door_anims/abandoned_ship/0.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship/1.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_FallarborDarkRoof[][0x100] =
{
    INCBIN_U8("graphics/door_anims/29/0.4bpp"),
    INCBIN_U8("graphics/door_anims/29/1.4bpp"),
    INCBIN_U8("graphics/door_anims/29/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_AbandonedShipRoom[][0x100] =
{
    INCBIN_U8("graphics/door_anims/abandoned_ship_room/0.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship_room/1.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship_room/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_LilycoveDeptStoreElevator[][0x100] =
{
    INCBIN_U8("graphics/door_anims/lilycove_dept_store_elevator/0.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_dept_store_elevator/1.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_dept_store_elevator/2.4bpp"),
};

asm(".space 32");

const u8 DoorAnimTiles_32[][0x100] =
{
    INCBIN_U8("graphics/door_anims/32/0.4bpp"),
    INCBIN_U8("graphics/door_anims/32/1.4bpp"),
    INCBIN_U8("graphics/door_anims/32/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_BattleTowerElevator[][0x100] =
{
    INCBIN_U8("graphics/door_anims/battle_tower_elevator/0.4bpp"),
    INCBIN_U8("graphics/door_anims/battle_tower_elevator/1.4bpp"),
    INCBIN_U8("graphics/door_anims/battle_tower_elevator/2.4bpp"),
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

static const u8 sDoorAnimPalettes_General[] = {1, 1, 1, 1, 1, 1, 1, 1};
static const u8 sDoorAnimPalettes_PokeCenter[] = {1, 1, 1, 1, 1, 1, 1, 1};
static const u8 sDoorAnimPalettes_Gym[] = {5, 5, 5, 5, 5, 5, 5, 5};
static const u8 sDoorAnimPalettes_PokeMart[] = {0, 0, 1, 1, 1, 1, 1, 1};
const u8 gDoorAnimPalettes_Littleroot[] = {6, 6, 6, 6, 6, 6, 6, 6};
static const u8 sDoorAnimPalettes_BirchsLab[] = {8, 8, 8, 8, 8, 8, 8, 8};
static const u8 sDoorAnimPalettes_RustboroTan[] = {11, 11, 11, 11, 11, 11, 11, 11};
static const u8 sDoorAnimPalettes_RustboroGray[] = {10, 10, 10, 10, 10, 10, 10, 10};
static const u8 sDoorAnimPalettes_FallarborLightRoof[] = {7, 7, 7, 7, 7, 7, 7, 7};
static const u8 sDoorAnimPalettes_Lilycove[] = {8, 8, 8, 8, 8, 8, 8, 8};
const u8 gDoorAnimPalettes_Oldale[] = {9, 9, 9, 9, 9, 9, 9, 9};
static const u8 sDoorAnimPalettes_Mossdeep[] = {9, 9, 1, 1, 1, 1, 1, 1};
static const u8 sDoorAnimPalettes_PokemonLeague[] = {8, 8, 8, 8, 8, 8, 8, 8};
static const u8 sDoorAnimPalettes_Pacifidlog[] = {9, 9, 9, 9, 9, 9, 9, 9};
static const u8 sDoorAnimPalettes_SootopolisPeakedRoof[] = {6, 6, 6, 6, 6, 6, 6, 6};
static const u8 sDoorAnimPalettes_Sootopolis[] = {6, 6, 6, 6, 6, 6, 6, 6};
static const u8 sDoorAnimPalettes_Dewford[] = {0, 0, 5, 5, 5, 5, 5, 5};
static const u8 sDoorAnimPalettes_Slateport[] = {6, 6, 1, 1, 1, 1, 1, 1};
static const u8 sDoorAnimPalettes_Mauville[] = {7, 7, 7, 7, 7, 7, 7, 7};
static const u8 sDoorAnimPalettes_Verdanturf[] = {6, 6, 5, 5, 5, 5, 5, 5};
static const u8 sDoorAnimPalettes_LilycoveWooden[] = {5, 5, 5, 5, 5, 5, 5, 5};
static const u8 sDoorAnimPalettes_Contest[] = {1, 1, 1, 1, 1, 1, 1, 1};
const u8 gDoorAnimPalettes_PetalburgGym[] = {6, 6, 7, 7, 7, 7, 7, 7};
static const u8 sDoorAnimPalettes_CyclingRoad[] = {7, 7, 7, 7, 7, 7, 7, 7};
static const u8 sDoorAnimPalettes_LilycoveDeptStore[] = {5, 5, 5, 5, 5, 5, 5, 5};
static const u8 sDoorAnimPalettes_SafariZone[] = {9, 9, 9, 9, 9, 9, 9, 9};
static const u8 sDoorAnimPalettes_MossdeepSpaceCenter[] = {8, 8, 8, 8, 8, 8, 8, 8};
static const u8 sDoorAnimPalettes_CableClub[] = {6, 6, 6, 6, 6, 6, 6, 6};
static const u8 sDoorAnimPalettes_AbandonedShip[] = {7, 7, 7, 7, 7, 7, 7, 7};
static const u8 sDoorAnimPalettes_FallarborDarkRoof[] = {11, 11, 7, 7, 7, 7, 7, 7};
static const u8 sDoorAnimPalettes_AbandonedShipRoom[] = {7, 7, 7, 7, 7, 7, 7, 7};
static const u8 sDoorAnimPalettes_LilycoveDeptStoreElevator[] = {6, 6, 7, 7, 7, 7, 7, 7};
const u8 DoorAnimPalettes_830F9A4[] = {9, 9, 9, 9, 9, 9, 9, 9};
static const u8 sDoorAnimPalettes_BattleTowerElevator[] = {7, 7, 7, 7, 7, 7, 7, 7};

static const struct DoorGraphics sDoorAnimGraphicsTable[] =
{
    {0x021, 0, gDoorAnimTiles_General, sDoorAnimPalettes_General}, // door 00
    {0x061, 1, sDoorAnimTiles_PokeCenter, sDoorAnimPalettes_PokeCenter}, // door 01
    {0x1CD, 1, sDoorAnimTiles_Gym, sDoorAnimPalettes_Gym}, // door 02
    {0x041, 1, sDoorAnimTiles_PokeMart, sDoorAnimPalettes_PokeMart}, // door 03
    {0x248, 0, gDoorAnimTiles_Littleroot, gDoorAnimPalettes_Littleroot}, // door 04
    {0x249, 0, gDoorAnimTiles_BirchsLab, sDoorAnimPalettes_BirchsLab}, // door 05
    {0x22F, 0, gDoorAnimTiles_RustboroTan, sDoorAnimPalettes_RustboroTan}, // door 06
    {0x21F, 0, gDoorAnimTiles_RustboroGray, sDoorAnimPalettes_RustboroGray}, // door 07
    {0x2A5, 0, gDoorAnimTiles_FallarborLightRoof, sDoorAnimPalettes_FallarborLightRoof}, // door 08
    {0x287, 0, gDoorAnimTiles_Oldale, gDoorAnimPalettes_Oldale}, // door 09
    {0x2AC, 0, gDoorAnimTiles_Mauville, sDoorAnimPalettes_Mauville}, // door 10
    {0x3A1, 0, gDoorAnimTiles_Verdanturf, sDoorAnimPalettes_Verdanturf}, // door 11
    {0x2DC, 0, gDoorAnimTiles_Slateport, sDoorAnimPalettes_Slateport}, // door 12
    {0x225, 0, gDoorAnimTiles_Dewford, sDoorAnimPalettes_Dewford}, // door 13
    {0x1DB, 1, sDoorAnimTiles_Contest, sDoorAnimPalettes_Contest}, // door 14
    {0x246, 0, gDoorAnimTiles_Lilycove, sDoorAnimPalettes_Lilycove}, // door 15
    {0x28E, 0, gDoorAnimTiles_LilycoveWooden, sDoorAnimPalettes_LilycoveWooden}, // door 16
    {0x2A1, 0, gDoorAnimTiles_Mossdeep, sDoorAnimPalettes_Mossdeep}, // door 17
    {0x21C, 0, gDoorAnimTiles_SootopolisPeakedRoof, sDoorAnimPalettes_SootopolisPeakedRoof}, // door 18
    {0x21E, 0, sDoorAnimTiles_Sootopolis, sDoorAnimPalettes_Sootopolis}, // door 19
    {0x21D, 1, sDoorAnimTiles_PokemonLeague, sDoorAnimPalettes_PokemonLeague}, // door 20
    {0x21A, 0, gDoorAnimTiles_Pacifidlog, sDoorAnimPalettes_Pacifidlog}, // door 21
    {0x224, 0, gDoorAnimTiles_PetalburgGym, gDoorAnimPalettes_PetalburgGym}, // door 22
    {0x289, 0, gDoorAnimTiles_CyclingRoad, sDoorAnimPalettes_CyclingRoad}, // door 23
    {0x30C, 1, gDoorAnimTiles_LilycoveDeptStore, sDoorAnimPalettes_LilycoveDeptStore}, // door 24
    {0x32D, 1, gDoorAnimTiles_SafariZone, sDoorAnimPalettes_SafariZone}, // door 25
    {0x2ED, 1, gDoorAnimTiles_MossdeepSpaceCenter, sDoorAnimPalettes_MossdeepSpaceCenter}, // door 26
    {0x264, 1, sDoorAnimTiles_CableClub, sDoorAnimPalettes_CableClub}, // door 27
    {0x22B, 0, sDoorAnimTiles_AbandonedShip, sDoorAnimPalettes_AbandonedShip}, // door 28
    {0x2F7, 0, gDoorAnimTiles_FallarborDarkRoof, sDoorAnimPalettes_FallarborDarkRoof}, // door 29
    {0x297, 0, sDoorAnimTiles_AbandonedShipRoom, sDoorAnimPalettes_AbandonedShipRoom}, // door 30
    {0x285, 1, sDoorAnimTiles_LilycoveDeptStoreElevator, sDoorAnimPalettes_LilycoveDeptStoreElevator}, // door 31
    {0x25D, 1, DoorAnimTiles_32, DoorAnimPalettes_830F9A4}, // door 32
    {0x20E, 1, sDoorAnimTiles_BattleTowerElevator, sDoorAnimPalettes_BattleTowerElevator}, // door 33
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

static bool32 AnimateDoorFrame(struct DoorGraphics *gfx, struct DoorAnimFrame *frames, s16 *data)
{
    if (data[TD_COUNTER] == 0)
        DrawDoor(gfx, &frames[data[TD_FRAME]], data[TD_X], data[TD_Y]);
    if (data[TD_COUNTER] == frames[data[TD_FRAME]].time)
    {
        data[TD_COUNTER] = 0;
        data[TD_FRAME]++;
        if (frames[data[TD_FRAME]].time == 0)
            return FALSE;
        else
            return TRUE;
    }
    data[TD_COUNTER]++;
    return TRUE;
}

static void Task_AnimateDoor(u8 taskId)
{
    u16 *data = gTasks[taskId].data;
    struct DoorAnimFrame *frames = (struct DoorAnimFrame *)(data[TD_FRAMELIST] << 16 | data[TD_FRAMELIST + 1]);
    struct DoorGraphics *gfx = (struct DoorGraphics *)(data[TD_GFX] << 16 | data[TD_GFX + 1]);

    if (AnimateDoorFrame(gfx, frames, data) == FALSE)
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
        s16 *data = gTasks[taskId].data;

        data[TD_X] = x;
        data[TD_Y] = y;

        data[TD_FRAMELIST + 1] = (u32)frames;
        data[TD_FRAMELIST] = (u32)frames >> 16;

        data[TD_GFX + 1] = (u32)gfx;
        data[TD_GFX] = (u32)gfx >> 16;

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

static void UNUSED Debug_FieldAnimateDoorOpen(u32 x, u32 y)
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
    if (GetDoorSoundType(sDoorAnimGraphicsTable, x, y) == 0)
        return SE_DOOR;
    else
        return SE_SLIDING_DOOR;
}
