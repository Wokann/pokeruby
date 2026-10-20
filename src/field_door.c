#include "global.h"
#include "field_door.h"
#include "field_camera.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "task.h"
#include "constants/songs.h"
#include "constants/metatile_labels.h"

#define DOOR_SOUND_NORMAL  0
#define DOOR_SOUND_SLIDING 1

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

const u8 gDoorAnimTiles_Littleroot[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/littleroot/0.4bpp"),
    INCBIN_U8("graphics/door_anims/littleroot/1.4bpp"),
    INCBIN_U8("graphics/door_anims/littleroot/2.4bpp"),
};

// TODO: Make these blank palette includes?
asm(".space 32");

const u8 gDoorAnimTiles_BirchsLab[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/birchs_lab/0.4bpp"),
    INCBIN_U8("graphics/door_anims/birchs_lab/1.4bpp"),
    INCBIN_U8("graphics/door_anims/birchs_lab/2.4bpp"),
};

const u8 gDoorAnimTiles_FallarborLightRoof[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/fallarbor_light_roof/0.4bpp"),
    INCBIN_U8("graphics/door_anims/fallarbor_light_roof/1.4bpp"),
    INCBIN_U8("graphics/door_anims/fallarbor_light_roof/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Lilycove[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/lilycove/0.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove/1.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_LilycoveWooden[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/lilycove_wooden/0.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_wooden/1.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_wooden/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_General[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/general/0.4bpp"),
    INCBIN_U8("graphics/door_anims/general/1.4bpp"),
    INCBIN_U8("graphics/door_anims/general/2.4bpp"),
};

static const u8 sDoorAnimTiles_PokeCenter[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/poke_center/0.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_center/1.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_center/2.4bpp"),
};

static const u8 sDoorAnimTiles_Gym[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/gym/0.4bpp"),
    INCBIN_U8("graphics/door_anims/gym/1.4bpp"),
    INCBIN_U8("graphics/door_anims/gym/2.4bpp"),
};

static const u8 sDoorAnimTiles_PokeMart[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/poke_mart/0.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_mart/1.4bpp"),
    INCBIN_U8("graphics/door_anims/poke_mart/2.4bpp"),
};

const u8 gDoorAnimTiles_RustboroTan[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/rustboro_tan/0.4bpp"),
    INCBIN_U8("graphics/door_anims/rustboro_tan/1.4bpp"),
    INCBIN_U8("graphics/door_anims/rustboro_tan/2.4bpp"),
};

const u8 gDoorAnimTiles_RustboroGray[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/rustboro_gray/0.4bpp"),
    INCBIN_U8("graphics/door_anims/rustboro_gray/1.4bpp"),
    INCBIN_U8("graphics/door_anims/rustboro_gray/2.4bpp"),
};

const u8 gDoorAnimTiles_Oldale[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/oldale/0.4bpp"),
    INCBIN_U8("graphics/door_anims/oldale/1.4bpp"),
    INCBIN_U8("graphics/door_anims/oldale/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Mauville[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/mauville/0.4bpp"),
    INCBIN_U8("graphics/door_anims/mauville/1.4bpp"),
    INCBIN_U8("graphics/door_anims/mauville/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Verdanturf[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/verdanturf/0.4bpp"),
    INCBIN_U8("graphics/door_anims/verdanturf/1.4bpp"),
    INCBIN_U8("graphics/door_anims/verdanturf/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Slateport[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/slateport/0.4bpp"),
    INCBIN_U8("graphics/door_anims/slateport/1.4bpp"),
    INCBIN_U8("graphics/door_anims/slateport/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Dewford[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/dewford/0.4bpp"),
    INCBIN_U8("graphics/door_anims/dewford/1.4bpp"),
    INCBIN_U8("graphics/door_anims/dewford/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_Contest[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/contest/0.4bpp"),
    INCBIN_U8("graphics/door_anims/contest/1.4bpp"),
    INCBIN_U8("graphics/door_anims/contest/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Mossdeep[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/mossdeep/0.4bpp"),
    INCBIN_U8("graphics/door_anims/mossdeep/1.4bpp"),
    INCBIN_U8("graphics/door_anims/mossdeep/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_SootopolisPeakedRoof[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/sootopolis_peaked_roof/0.4bpp"),
    INCBIN_U8("graphics/door_anims/sootopolis_peaked_roof/1.4bpp"),
    INCBIN_U8("graphics/door_anims/sootopolis_peaked_roof/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_Sootopolis[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/sootopolis/0.4bpp"),
    INCBIN_U8("graphics/door_anims/sootopolis/1.4bpp"),
    INCBIN_U8("graphics/door_anims/sootopolis/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_PokemonLeague[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/pokemon_league/0.4bpp"),
    INCBIN_U8("graphics/door_anims/pokemon_league/1.4bpp"),
    INCBIN_U8("graphics/door_anims/pokemon_league/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_Pacifidlog[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/pacifidlog/0.4bpp"),
    INCBIN_U8("graphics/door_anims/pacifidlog/1.4bpp"),
    INCBIN_U8("graphics/door_anims/pacifidlog/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_PetalburgGym[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/petalburg_gym/0.4bpp"),
    INCBIN_U8("graphics/door_anims/petalburg_gym/1.4bpp"),
    INCBIN_U8("graphics/door_anims/petalburg_gym/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_CyclingRoad[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/cycling_road/0.4bpp"),
    INCBIN_U8("graphics/door_anims/cycling_road/1.4bpp"),
    INCBIN_U8("graphics/door_anims/cycling_road/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_LilycoveDeptStore[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/lilycove_dept_store/0.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_dept_store/1.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_dept_store/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_SafariZone[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/safari_zone/0.4bpp"),
    INCBIN_U8("graphics/door_anims/safari_zone/1.4bpp"),
    INCBIN_U8("graphics/door_anims/safari_zone/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_MossdeepSpaceCenter[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/mossdeep_space_center/0.4bpp"),
    INCBIN_U8("graphics/door_anims/mossdeep_space_center/1.4bpp"),
    INCBIN_U8("graphics/door_anims/mossdeep_space_center/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_CableClub[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/cable_club/0.4bpp"),
    INCBIN_U8("graphics/door_anims/cable_club/1.4bpp"),
    INCBIN_U8("graphics/door_anims/cable_club/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_AbandonedShip[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/abandoned_ship/0.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship/1.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship/2.4bpp"),
};

asm(".space 32");

const u8 gDoorAnimTiles_FallarborDarkRoof[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/fallarbor_dark_roof/0.4bpp"),
    INCBIN_U8("graphics/door_anims/fallarbor_dark_roof/1.4bpp"),
    INCBIN_U8("graphics/door_anims/fallarbor_dark_roof/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_AbandonedShipRoom[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/abandoned_ship_room/0.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship_room/1.4bpp"),
    INCBIN_U8("graphics/door_anims/abandoned_ship_room/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_LilycoveDeptStoreElevator[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/lilycove_dept_store_elevator/0.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_dept_store_elevator/1.4bpp"),
    INCBIN_U8("graphics/door_anims/lilycove_dept_store_elevator/2.4bpp"),
};

asm(".space 32");

const u8 DoorAnimTiles_32[][8 * TILE_SIZE_4BPP] =
{
    INCBIN_U8("graphics/door_anims/32/0.4bpp"),
    INCBIN_U8("graphics/door_anims/32/1.4bpp"),
    INCBIN_U8("graphics/door_anims/32/2.4bpp"),
};

asm(".space 32");

static const u8 sDoorAnimTiles_BattleTowerElevator[][8 * TILE_SIZE_4BPP] =
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
    {4, 8 * TILE_SIZE_4BPP},
    {4, 2 * 8 * TILE_SIZE_4BPP},
    {0, 0},
};

static const struct DoorAnimFrame sDoorCloseAnimFrames[] =
{
    {4, 2 * 8 * TILE_SIZE_4BPP},
    {4, 8 * TILE_SIZE_4BPP},
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
    {METATILE_General_Door,                          DOOR_SOUND_NORMAL,  gDoorAnimTiles_General, sDoorAnimPalettes_General}, // door 00
    {METATILE_General_Door_PokeCenter,               DOOR_SOUND_SLIDING, sDoorAnimTiles_PokeCenter, sDoorAnimPalettes_PokeCenter}, // door 01
    {METATILE_General_Door_Gym,                      DOOR_SOUND_SLIDING, sDoorAnimTiles_Gym, sDoorAnimPalettes_Gym}, // door 02
    {METATILE_General_Door_PokeMart,                 DOOR_SOUND_SLIDING, sDoorAnimTiles_PokeMart, sDoorAnimPalettes_PokeMart}, // door 03
    {METATILE_Petalburg_Door_Littleroot,             DOOR_SOUND_NORMAL,  gDoorAnimTiles_Littleroot, gDoorAnimPalettes_Littleroot}, // door 04
    {METATILE_Petalburg_Door_BirchsLab,              DOOR_SOUND_NORMAL,  gDoorAnimTiles_BirchsLab, sDoorAnimPalettes_BirchsLab}, // door 05
    {METATILE_Rustboro_Door_Tan,                     DOOR_SOUND_NORMAL,  gDoorAnimTiles_RustboroTan, sDoorAnimPalettes_RustboroTan}, // door 06
    {METATILE_Rustboro_Door_Gray,                    DOOR_SOUND_NORMAL,  gDoorAnimTiles_RustboroGray, sDoorAnimPalettes_RustboroGray}, // door 07
    {METATILE_Fallarbor_Door_LightRoof,              DOOR_SOUND_NORMAL,  gDoorAnimTiles_FallarborLightRoof, sDoorAnimPalettes_FallarborLightRoof}, // door 08
    {METATILE_Petalburg_Door_Oldale,                 DOOR_SOUND_NORMAL,  gDoorAnimTiles_Oldale, gDoorAnimPalettes_Oldale}, // door 09
    {METATILE_Mauville_Door,                         DOOR_SOUND_NORMAL,  gDoorAnimTiles_Mauville, sDoorAnimPalettes_Mauville}, // door 10
    {METATILE_Mauville_Door_Verdanturf,              DOOR_SOUND_NORMAL,  gDoorAnimTiles_Verdanturf, sDoorAnimPalettes_Verdanturf}, // door 11
    {METATILE_Slateport_Door,                        DOOR_SOUND_NORMAL,  gDoorAnimTiles_Slateport, sDoorAnimPalettes_Slateport}, // door 12
    {METATILE_Dewford_Door,                          DOOR_SOUND_NORMAL,  gDoorAnimTiles_Dewford, sDoorAnimPalettes_Dewford}, // door 13
    {METATILE_General_Door_Contest,                  DOOR_SOUND_SLIDING, sDoorAnimTiles_Contest, sDoorAnimPalettes_Contest}, // door 14
    {METATILE_Lilycove_Door,                         DOOR_SOUND_NORMAL,  gDoorAnimTiles_Lilycove, sDoorAnimPalettes_Lilycove}, // door 15
    {METATILE_Lilycove_Door_Wooden,                  DOOR_SOUND_NORMAL,  gDoorAnimTiles_LilycoveWooden, sDoorAnimPalettes_LilycoveWooden}, // door 16
    {METATILE_Mossdeep_Door,                         DOOR_SOUND_NORMAL,  gDoorAnimTiles_Mossdeep, sDoorAnimPalettes_Mossdeep}, // door 17
    {METATILE_Sootopolis_Door_PeakedRoof,            DOOR_SOUND_NORMAL,  gDoorAnimTiles_SootopolisPeakedRoof, sDoorAnimPalettes_SootopolisPeakedRoof}, // door 18
    {METATILE_Sootopolis_Door,                       DOOR_SOUND_NORMAL,  sDoorAnimTiles_Sootopolis, sDoorAnimPalettes_Sootopolis}, // door 19
    {METATILE_EverGrande_Door_PokemonLeague,         DOOR_SOUND_SLIDING, sDoorAnimTiles_PokemonLeague, sDoorAnimPalettes_PokemonLeague}, // door 20
    {METATILE_Pacifidlog_Door,                       DOOR_SOUND_NORMAL,  gDoorAnimTiles_Pacifidlog, sDoorAnimPalettes_Pacifidlog}, // door 21
    {METATILE_PetalburgGym_Door,                     DOOR_SOUND_NORMAL,  gDoorAnimTiles_PetalburgGym, gDoorAnimPalettes_PetalburgGym}, // door 22
    {METATILE_Mauville_Door_CyclingRoad,             DOOR_SOUND_NORMAL,  gDoorAnimTiles_CyclingRoad, sDoorAnimPalettes_CyclingRoad}, // door 23
    {METATILE_Lilycove_Door_DeptStore,               DOOR_SOUND_SLIDING, gDoorAnimTiles_LilycoveDeptStore, sDoorAnimPalettes_LilycoveDeptStore}, // door 24
    {METATILE_Lilycove_Door_SafariZone,              DOOR_SOUND_SLIDING, gDoorAnimTiles_SafariZone, sDoorAnimPalettes_SafariZone}, // door 25
    {METATILE_Mossdeep_Door_SpaceCenter,             DOOR_SOUND_SLIDING, gDoorAnimTiles_MossdeepSpaceCenter, sDoorAnimPalettes_MossdeepSpaceCenter}, // door 26
    {METATILE_PokemonCenter_Door_CableClub,          DOOR_SOUND_SLIDING, sDoorAnimTiles_CableClub, sDoorAnimPalettes_CableClub}, // door 27
    {METATILE_InsideShip_IntactDoor_Bottom_Unlocked, DOOR_SOUND_NORMAL,  sDoorAnimTiles_AbandonedShip, sDoorAnimPalettes_AbandonedShip}, // door 28
    {METATILE_Fallarbor_Door_DarkRoof,               DOOR_SOUND_NORMAL,  gDoorAnimTiles_FallarborDarkRoof, sDoorAnimPalettes_FallarborDarkRoof}, // door 29
    {METATILE_InsideShip_IntactDoor_Bottom_Interior, DOOR_SOUND_NORMAL,  sDoorAnimTiles_AbandonedShipRoom, sDoorAnimPalettes_AbandonedShipRoom}, // door 30
    {METATILE_Shop_Door_Elevator,                    DOOR_SOUND_SLIDING, sDoorAnimTiles_LilycoveDeptStoreElevator, sDoorAnimPalettes_LilycoveDeptStoreElevator}, // door 31
    {METATILE_Dewford_Door_BattleTower,              DOOR_SOUND_SLIDING, DoorAnimTiles_32, DoorAnimPalettes_830F9A4}, // door 32
    {METATILE_BattleTower_Door_Elevator,             DOOR_SOUND_SLIDING, sDoorAnimTiles_BattleTowerElevator, sDoorAnimPalettes_BattleTowerElevator}, // door 33
    {0, 0, NULL, NULL},
};

// Door animation tiles overwrite the final eight tiles of the map tileset.
#define DOOR_TILE_START_SIZE1 (NUM_TILES_TOTAL - 8)

static void CopyDoorTilesToVram(const void *src)
{
    CpuFastSet(src, (void *)(VRAM + TILE_OFFSET_4BPP(DOOR_TILE_START_SIZE1)), 8 * TILE_SIZE_4BPP / sizeof(u32));
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
    for (; i < NUM_TILES_PER_METATILE; i++)
    {
        tile = *(paletteNums++) << 12;
        tiles[i] = tile;
    }
}

static void DrawCurrentDoorAnimFrame(u32 x, u32 y, const u8 *paletteNums)
{
    u16 tiles[NUM_TILES_PER_METATILE];

    BuildDoorTiles(tiles, DOOR_TILE_START_SIZE1, paletteNums);
    DrawDoorMetatileAt(x, y - 1, tiles);
    BuildDoorTiles(tiles, DOOR_TILE_START_SIZE1 + 4, paletteNums + 4);
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
    TD_FRAMES_HI = 0,
    TD_FRAMES_LO,
    TD_GFX_HI = 2,
    TD_GFX_LO,
    TD_FRAME_ID = 4,
    TD_COUNTER,
    TD_X,
    TD_Y
};

static bool32 AnimateDoorFrame(struct DoorGraphics *gfx, struct DoorAnimFrame *frames, s16 *data)
{
    if (data[TD_COUNTER] == 0)
        DrawDoor(gfx, &frames[data[TD_FRAME_ID]], data[TD_X], data[TD_Y]);
    if (data[TD_COUNTER] == frames[data[TD_FRAME_ID]].time)
    {
        data[TD_COUNTER] = 0;
        data[TD_FRAME_ID]++;
        if (frames[data[TD_FRAME_ID]].time == 0)
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
    struct DoorAnimFrame *frames = (struct DoorAnimFrame *)(data[TD_FRAMES_HI] << 16 | data[TD_FRAMES_LO]);
    struct DoorGraphics *gfx = (struct DoorGraphics *)(data[TD_GFX_HI] << 16 | data[TD_GFX_LO]);

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
        return (s8)TASK_NONE;
    else
    {
        u8 taskId = CreateTask(Task_AnimateDoor, 0x50);
        s16 *data = gTasks[taskId].data;

        data[TD_X] = x;
        data[TD_Y] = y;

        data[TD_FRAMES_LO] = (u32)frames;
        data[TD_FRAMES_HI] = (u32)frames >> 16;

        data[TD_GFX_LO] = (u32)gfx;
        data[TD_GFX_HI] = (u32)gfx >> 16;

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
        return (s8)TASK_NONE;
    else
        return StartDoorAnimationTask(gfx, sDoorOpenAnimFrames, x, y);
}

static s8 StartDoorCloseAnimation(const struct DoorGraphics *gfx, u32 x, u32 y)
{
    gfx = GetDoorGraphics(gfx, MapGridGetMetatileIdAt(x, y));
    if (gfx == NULL)
        return (s8)TASK_NONE;
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
        return (s8)TASK_NONE;
    else
        return StartDoorCloseAnimation(sDoorAnimGraphicsTable, x, y);
}

s8 FieldAnimateDoorOpen(u32 x, u32 y)
{
    if (!MetatileBehavior_IsDoor(MapGridGetMetatileBehaviorAt(x, y)))
        return (s8)TASK_NONE;
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
