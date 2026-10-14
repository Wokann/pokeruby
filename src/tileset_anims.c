#include "global.h"
#include "tileset_anims.h"

struct Dma
{
    const void *src;
    const void *dest;
    u16 size;
};

EWRAM_DATA static struct Dma sTilesetDMA3TransferBuffer[20] = {0};

static u8 sTilesetDMA3TransferBufferSize;
static u16 sPrimaryTilesetAnimCounter;
static u16 sPrimaryTilesetAnimCounterMax;
static u16 sSecondaryTilesetAnimCounter;
static u16 sSecondaryTilesetAnimCounterMax;
static void (*sPrimaryTilesetAnimCallback)(u16);
static void (*sSecondaryTilesetAnimCallback)(u16);

static void _InitPrimaryTilesetAnimation(void);
static void _InitSecondaryTilesetAnimation(void);

static void TilesetAnim_General(u16);
static void TilesetAnim_Building(u16);

static void QueueAnimTiles_General_Flower(u16);
static void QueueAnimTiles_General_Water(u16);
static void QueueAnimTiles_General_SandWaterEdge(u16);
static void QueueAnimTiles_General_Waterfall(u16);
static void QueueAnimTiles_General_LandWaterEdge(u16);

static void TilesetAnim_Rustboro(u16);
static void TilesetAnim_Mauville(u16);
static void TilesetAnim_Lavaridge(u16);
static void TilesetAnim_EverGrande(u16);
static void TilesetAnim_Pacifidlog(u16);
static void TilesetAnim_Underwater(u16);
static void TilesetAnim_SootopolisGym(u16);
static void TilesetAnim_Cave(u16);
static void TilesetAnim_EliteFour(u16);
static void TilesetAnim_MauvilleGym(u16);
static void TilesetAnim_BikeShop(u16);

static void QueueAnimTiles_Rustboro_WindyWater(u16, u8);
static void QueueAnimTiles_Rustboro_Fountain(u16);
static void QueueAnimTiles_Mauville_Flowers(u16, u8);
static void QueueAnimTiles_Lavaridge_Steam(u8);
static void QueueAnimTiles_Lavaridge_Lava(u16);
static void QueueAnimTiles_EverGrande_Flowers(u16, u8);
static void QueueAnimTiles_Pacifidlog_LogBridges(u8);
static void QueueAnimTiles_Pacifidlog_WaterCurrents(u8);
static void QueueAnimTiles_Underwater_Seaweed(u8);
static void QueueAnimTiles_Cave_Lava(u16);

static void QueueAnimTiles_Building_TVTurnedOn(u16);
static void QueueAnimTiles_MauvilleGym_ElectricGates(u16);
static void QueueAnimTiles_SootopolisGym_Waterfalls(u16);
static void QueueAnimTiles_EliteFour_GroundLights(u16);
static void QueueAnimTiles_EliteFour_WallLights(u16);
static void QueueAnimTiles_BikeShop_BlinkingLights(u16);

const u8 gTilesetAnims_General_Flower_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/flower/1.4bpp");
const u8 gTilesetAnims_General_Flower_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/flower/0.4bpp");
const u8 gTilesetAnims_General_Flower_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/flower/2.4bpp");
asm(".space 32");

const u8 *const gTilesetAnims_General_Flower[] =
{
    gTilesetAnims_General_Flower_Frame0,
    gTilesetAnims_General_Flower_Frame1,
    gTilesetAnims_General_Flower_Frame0,
    gTilesetAnims_General_Flower_Frame2,
};

const u8 gTilesetAnims_General_Water_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/water/0.4bpp");
const u8 gTilesetAnims_General_Water_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/water/1.4bpp");
const u8 gTilesetAnims_General_Water_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/water/2.4bpp");
const u8 gTilesetAnims_General_Water_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/water/3.4bpp");
const u8 gTilesetAnims_General_Water_Frame4[] = INCBIN_U8("data/tilesets/primary/general/anim/water/4.4bpp");
const u8 gTilesetAnims_General_Water_Frame5[] = INCBIN_U8("data/tilesets/primary/general/anim/water/5.4bpp");
const u8 gTilesetAnims_General_Water_Frame6[] = INCBIN_U8("data/tilesets/primary/general/anim/water/6.4bpp");
const u8 gTilesetAnims_General_Water_Frame7[] = INCBIN_U8("data/tilesets/primary/general/anim/water/7.4bpp");

const u8 *const gTilesetAnims_General_Water[] =
{
    gTilesetAnims_General_Water_Frame0,
    gTilesetAnims_General_Water_Frame1,
    gTilesetAnims_General_Water_Frame2,
    gTilesetAnims_General_Water_Frame3,
    gTilesetAnims_General_Water_Frame4,
    gTilesetAnims_General_Water_Frame5,
    gTilesetAnims_General_Water_Frame6,
    gTilesetAnims_General_Water_Frame7,
};

const u8 gTilesetAnims_General_SandWaterEdge_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/sand_water_edge/0.4bpp");
const u8 gTilesetAnims_General_SandWaterEdge_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/sand_water_edge/1.4bpp");
const u8 gTilesetAnims_General_SandWaterEdge_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/sand_water_edge/2.4bpp");
const u8 gTilesetAnims_General_SandWaterEdge_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/sand_water_edge/3.4bpp");
const u8 gTilesetAnims_General_SandWaterEdge_Frame4[] = INCBIN_U8("data/tilesets/primary/general/anim/sand_water_edge/4.4bpp");
const u8 gTilesetAnims_General_SandWaterEdge_Frame5[] = INCBIN_U8("data/tilesets/primary/general/anim/sand_water_edge/5.4bpp");
const u8 gTilesetAnims_General_SandWaterEdge_Frame6[] = INCBIN_U8("data/tilesets/primary/general/anim/sand_water_edge/6.4bpp");

const u8 *const gTilesetAnims_General_SandWaterEdge[] =
{
    gTilesetAnims_General_SandWaterEdge_Frame0,
    gTilesetAnims_General_SandWaterEdge_Frame1,
    gTilesetAnims_General_SandWaterEdge_Frame2,
    gTilesetAnims_General_SandWaterEdge_Frame3,
    gTilesetAnims_General_SandWaterEdge_Frame4,
    gTilesetAnims_General_SandWaterEdge_Frame5,
    gTilesetAnims_General_SandWaterEdge_Frame6,
    gTilesetAnims_General_SandWaterEdge_Frame0,
};

const u8 gTilesetAnims_General_Waterfall_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/waterfall/0.4bpp");
const u8 gTilesetAnims_General_Waterfall_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/waterfall/1.4bpp");
const u8 gTilesetAnims_General_Waterfall_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/waterfall/2.4bpp");
const u8 gTilesetAnims_General_Waterfall_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/waterfall/3.4bpp");

const u8 *const gTilesetAnims_General_Waterfall[] =
{
    gTilesetAnims_General_Waterfall_Frame0,
    gTilesetAnims_General_Waterfall_Frame1,
    gTilesetAnims_General_Waterfall_Frame2,
    gTilesetAnims_General_Waterfall_Frame3,
};

const u8 gTilesetAnims_General_LandWaterEdge_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/land_water_edge/0.4bpp");
const u8 gTilesetAnims_General_LandWaterEdge_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/land_water_edge/1.4bpp");
const u8 gTilesetAnims_General_LandWaterEdge_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/land_water_edge/2.4bpp");
const u8 gTilesetAnims_General_LandWaterEdge_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/land_water_edge/3.4bpp");

const u8 *const gTilesetAnims_General_LandWaterEdge[] =
{
    gTilesetAnims_General_LandWaterEdge_Frame0,
    gTilesetAnims_General_LandWaterEdge_Frame1,
    gTilesetAnims_General_LandWaterEdge_Frame2,
    gTilesetAnims_General_LandWaterEdge_Frame3,
};

const u8 gTilesetAnims_Lavaridge_Steam_Frame0[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/steam/0.4bpp");
const u8 gTilesetAnims_Lavaridge_Steam_Frame1[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/steam/1.4bpp");
const u8 gTilesetAnims_Lavaridge_Steam_Frame2[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/steam/2.4bpp");
const u8 gTilesetAnims_Lavaridge_Steam_Frame3[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/steam/3.4bpp");

const u8 *const gTilesetAnims_Lavaridge_Steam[] =
{
    gTilesetAnims_Lavaridge_Steam_Frame0,
    gTilesetAnims_Lavaridge_Steam_Frame1,
    gTilesetAnims_Lavaridge_Steam_Frame2,
    gTilesetAnims_Lavaridge_Steam_Frame3,
};

const u8 gTilesetAnims_Pacifidlog_LogBridges_Frame0[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/log_bridges/0.4bpp");
const u8 gTilesetAnims_Pacifidlog_LogBridges_Frame1[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/log_bridges/1.4bpp");
const u8 gTilesetAnims_Pacifidlog_LogBridges_Frame2[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/log_bridges/2.4bpp");

const u8 *const gTilesetAnims_Pacifidlog_LogBridges[] =
{
    gTilesetAnims_Pacifidlog_LogBridges_Frame0,
    gTilesetAnims_Pacifidlog_LogBridges_Frame1,
    gTilesetAnims_Pacifidlog_LogBridges_Frame2,
    gTilesetAnims_Pacifidlog_LogBridges_Frame1,
};

const u8 gTilesetAnims_Underwater_Seaweed_Frame0[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/seaweed/0.4bpp");
const u8 gTilesetAnims_Underwater_Seaweed_Frame1[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/seaweed/1.4bpp");
const u8 gTilesetAnims_Underwater_Seaweed_Frame2[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/seaweed/2.4bpp");
const u8 gTilesetAnims_Underwater_Seaweed_Frame3[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/seaweed/3.4bpp");

const u8 *const gTilesetAnims_Underwater_Seaweed[] =
{
    gTilesetAnims_Underwater_Seaweed_Frame0,
    gTilesetAnims_Underwater_Seaweed_Frame1,
    gTilesetAnims_Underwater_Seaweed_Frame2,
    gTilesetAnims_Underwater_Seaweed_Frame3
};

const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame0[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/0.4bpp");
const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame1[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/1.4bpp");
const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame2[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/2.4bpp");
const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame3[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/3.4bpp");
const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame4[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/4.4bpp");
const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame5[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/5.4bpp");
const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame6[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/6.4bpp");
const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame7[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/water_currents/7.4bpp");

const u8 *const gTilesetAnims_Pacifidlog_WaterCurrents[] =
{
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame0,
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame1,
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame2,
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame3,
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame4,
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame5,
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame6,
    gTilesetAnims_Pacifidlog_WaterCurrents_Frame7
};

const u8 gTilesetAnims_Mauville_Flower1_Frame0[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_1/0.4bpp");
const u8 gTilesetAnims_Mauville_Flower1_Frame1[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_1/1.4bpp");
const u8 gTilesetAnims_Mauville_Flower1_Frame2[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_1/2.4bpp");
const u8 gTilesetAnims_Mauville_Flower1_Frame3[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_1/3.4bpp");
const u8 gTilesetAnims_Mauville_Flower1_Frame4[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_1/4.4bpp");

const u8 gTilesetAnims_Mauville_Flower2_Frame0[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_2/0.4bpp");
const u8 gTilesetAnims_Mauville_Flower2_Frame1[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_2/1.4bpp");
const u8 gTilesetAnims_Mauville_Flower2_Frame2[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_2/2.4bpp");
const u8 gTilesetAnims_Mauville_Flower2_Frame3[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_2/3.4bpp");
const u8 gTilesetAnims_Mauville_Flower2_Frame4[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/flower_2/4.4bpp");

const void *const gTilesetAnims_Mauville_Flower1_VDests[] =
{
    (void *)(VRAM + 0x4C00),
    (void *)(VRAM + 0x4C80),
    (void *)(VRAM + 0x4D00),
    (void *)(VRAM + 0x4D80),
    (void *)(VRAM + 0x4E00),
    (void *)(VRAM + 0x4E80),
    (void *)(VRAM + 0x4F00),
    (void *)(VRAM + 0x4F80),
};

const void *const gTilesetAnims_Mauville_Flower2_VDests[] =
{
    (void *)(VRAM + 0x5000),
    (void *)(VRAM + 0x5080),
    (void *)(VRAM + 0x5100),
    (void *)(VRAM + 0x5180),
    (void *)(VRAM + 0x5200),
    (void *)(VRAM + 0x5280),
    (void *)(VRAM + 0x5300),
    (void *)(VRAM + 0x5380),
};

const u8 *const gTilesetAnims_Mauville_Flower1[] =
{
    gTilesetAnims_Mauville_Flower1_Frame0,
    gTilesetAnims_Mauville_Flower1_Frame0,
    gTilesetAnims_Mauville_Flower1_Frame1,
    gTilesetAnims_Mauville_Flower1_Frame2,
    gTilesetAnims_Mauville_Flower1_Frame3,
    gTilesetAnims_Mauville_Flower1_Frame3,
    gTilesetAnims_Mauville_Flower1_Frame3,
    gTilesetAnims_Mauville_Flower1_Frame3,
    gTilesetAnims_Mauville_Flower1_Frame3,
    gTilesetAnims_Mauville_Flower1_Frame3,
    gTilesetAnims_Mauville_Flower1_Frame2,
    gTilesetAnims_Mauville_Flower1_Frame1,
};

const u8 *const gTilesetAnims_Mauville_Flower2[] =
{
    gTilesetAnims_Mauville_Flower2_Frame0,
    gTilesetAnims_Mauville_Flower2_Frame0,
    gTilesetAnims_Mauville_Flower2_Frame1,
    gTilesetAnims_Mauville_Flower2_Frame2,
    gTilesetAnims_Mauville_Flower2_Frame3,
    gTilesetAnims_Mauville_Flower2_Frame3,
    gTilesetAnims_Mauville_Flower2_Frame3,
    gTilesetAnims_Mauville_Flower2_Frame3,
    gTilesetAnims_Mauville_Flower2_Frame3,
    gTilesetAnims_Mauville_Flower2_Frame3,
    gTilesetAnims_Mauville_Flower2_Frame2,
    gTilesetAnims_Mauville_Flower2_Frame1,
};

const u8 *const gTilesetAnims_Mauville_Flower1_B[] =
{
    gTilesetAnims_Mauville_Flower1_Frame0,
    gTilesetAnims_Mauville_Flower1_Frame0,
    gTilesetAnims_Mauville_Flower1_Frame4,
    gTilesetAnims_Mauville_Flower1_Frame4,
};

const u8 *const gTilesetAnims_Mauville_Flower2_B[] =
{
    gTilesetAnims_Mauville_Flower2_Frame0,
    gTilesetAnims_Mauville_Flower2_Frame0,
    gTilesetAnims_Mauville_Flower2_Frame4,
    gTilesetAnims_Mauville_Flower2_Frame4,
};

const u8 gTilesetAnims_Rustboro_WindyWater_Frame0[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/0.4bpp");
const u8 gTilesetAnims_Rustboro_WindyWater_Frame1[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/1.4bpp");
const u8 gTilesetAnims_Rustboro_WindyWater_Frame2[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/2.4bpp");
const u8 gTilesetAnims_Rustboro_WindyWater_Frame3[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/3.4bpp");
const u8 gTilesetAnims_Rustboro_WindyWater_Frame4[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/4.4bpp");
const u8 gTilesetAnims_Rustboro_WindyWater_Frame5[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/5.4bpp");
const u8 gTilesetAnims_Rustboro_WindyWater_Frame6[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/6.4bpp");
const u8 gTilesetAnims_Rustboro_WindyWater_Frame7[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/windy_water/7.4bpp");

const void *const gTilesetAnims_Rustboro_WindyWater_VDests[] =
{
    (void *)(VRAM + 0x5000),
    (void *)(VRAM + 0x5080),
    (void *)(VRAM + 0x5100),
    (void *)(VRAM + 0x5180),
    (void *)(VRAM + 0x5200),
    (void *)(VRAM + 0x5280),
    (void *)(VRAM + 0x5300),
    (void *)(VRAM + 0x5380),
};

const u8 *const gTilesetAnims_Rustboro_WindyWater[] =
{
    gTilesetAnims_Rustboro_WindyWater_Frame0,
    gTilesetAnims_Rustboro_WindyWater_Frame1,
    gTilesetAnims_Rustboro_WindyWater_Frame2,
    gTilesetAnims_Rustboro_WindyWater_Frame3,
    gTilesetAnims_Rustboro_WindyWater_Frame4,
    gTilesetAnims_Rustboro_WindyWater_Frame5,
    gTilesetAnims_Rustboro_WindyWater_Frame6,
    gTilesetAnims_Rustboro_WindyWater_Frame7,
};

const u8 gTilesetAnims_Rustboro_Fountain_Frame0[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/fountain/0.4bpp");
const u8 gTilesetAnims_Rustboro_Fountain_Frame1[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/fountain/1.4bpp");

const u8 *const gTilesetAnims_Rustboro_Fountain[] =
{
    gTilesetAnims_Rustboro_Fountain_Frame0,
    gTilesetAnims_Rustboro_Fountain_Frame1,
};

const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame0[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/0.4bpp");
const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame1[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/1.4bpp");
const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame2[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/2.4bpp");
const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame3[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/3.4bpp");
// unused anim tiles for cave
const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame4[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/4.4bpp");
const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame5[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/5.4bpp");
const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame6[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/6.4bpp");
const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame7[] = INCBIN_U8("data/tilesets/secondary/cave/anim/lava/7.4bpp");
asm(".space 32");

const u8 *const gTilesetAnims_Lavaridge_Cave_Lava[] =
{
    gTilesetAnims_Lavaridge_Cave_Lava_Frame0,
    gTilesetAnims_Lavaridge_Cave_Lava_Frame1,
    gTilesetAnims_Lavaridge_Cave_Lava_Frame2,
    gTilesetAnims_Lavaridge_Cave_Lava_Frame3,
};

const u8 gTilesetAnims_EverGrande_Flowers_Frame0[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/0.4bpp");
const u8 gTilesetAnims_EverGrande_Flowers_Frame1[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/1.4bpp");
const u8 gTilesetAnims_EverGrande_Flowers_Frame2[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/2.4bpp");
const u8 gTilesetAnims_EverGrande_Flowers_Frame3[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/3.4bpp");
const u8 gTilesetAnims_EverGrande_Flowers_Frame4[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/4.4bpp");
const u8 gTilesetAnims_EverGrande_Flowers_Frame5[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/5.4bpp");
const u8 gTilesetAnims_EverGrande_Flowers_Frame6[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/6.4bpp");
const u8 gTilesetAnims_EverGrande_Flowers_Frame7[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/flowers/7.4bpp");
asm(".space 32");

const void *const gTilesetAnims_EverGrande_VDests[] =
{
    (void *)(VRAM + 0x5C00),
    (void *)(VRAM + 0x5C80),
    (void *)(VRAM + 0x5D00),
    (void *)(VRAM + 0x5D80),
    (void *)(VRAM + 0x5E00),
    (void *)(VRAM + 0x5E80),
    (void *)(VRAM + 0x5F00),
    (void *)(VRAM + 0x5F80),
};

const u8 *const gTilesetAnims_EverGrande_Flowers[] =
{
    gTilesetAnims_EverGrande_Flowers_Frame0,
    gTilesetAnims_EverGrande_Flowers_Frame1,
    gTilesetAnims_EverGrande_Flowers_Frame2,
    gTilesetAnims_EverGrande_Flowers_Frame3,
    gTilesetAnims_EverGrande_Flowers_Frame4,
    gTilesetAnims_EverGrande_Flowers_Frame5,
    gTilesetAnims_EverGrande_Flowers_Frame6,
    gTilesetAnims_EverGrande_Flowers_Frame7,
};

const u8 gTilesetAnims_Building_TvTurnedOn_Frame0[] = INCBIN_U8("data/tilesets/primary/building/anim/tv_turned_on/0.4bpp");
const u8 gTilesetAnims_Building_TvTurnedOn_Frame1[] = INCBIN_U8("data/tilesets/primary/building/anim/tv_turned_on/1.4bpp");

const u8 *const gTilesetAnims_Building_TvTurnedOn[] =
{
    gTilesetAnims_Building_TvTurnedOn_Frame0,
    gTilesetAnims_Building_TvTurnedOn_Frame1,
};

const u8 gTilesetAnims_SootopolisGym_SideWaterfall_Frame0[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/side_waterfall/0.4bpp");
const u8 gTilesetAnims_SootopolisGym_SideWaterfall_Frame1[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/side_waterfall/1.4bpp");
const u8 gTilesetAnims_SootopolisGym_SideWaterfall_Frame2[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/side_waterfall/2.4bpp");
const u8 gTilesetAnims_SootopolisGym_FrontWaterfall_Frame0[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/front_waterfall/0.4bpp");
const u8 gTilesetAnims_SootopolisGym_FrontWaterfall_Frame1[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/front_waterfall/1.4bpp");
const u8 gTilesetAnims_SootopolisGym_FrontWaterfall_Frame2[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/front_waterfall/2.4bpp");

const u8 *const gTilesetAnims_SootopolisGym_SideWaterfall[] =
{
    gTilesetAnims_SootopolisGym_SideWaterfall_Frame0,
    gTilesetAnims_SootopolisGym_SideWaterfall_Frame1,
    gTilesetAnims_SootopolisGym_SideWaterfall_Frame2,
};

const u8 *const gTilesetAnims_SootopolisGym_FrontWaterfall[] =
{
    gTilesetAnims_SootopolisGym_FrontWaterfall_Frame0,
    gTilesetAnims_SootopolisGym_FrontWaterfall_Frame1,
    gTilesetAnims_SootopolisGym_FrontWaterfall_Frame2,
};

const u8 gTilesetAnims_EliteFour_FloorLight_Frame0[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/floor_light/0.4bpp");
const u8 gTilesetAnims_EliteFour_FloorLight_Frame1[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/floor_light/1.4bpp");
const u8 gTilesetAnims_EliteFour_WallLights_Frame0[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/wall_lights/0.4bpp");
const u8 gTilesetAnims_EliteFour_WallLights_Frame1[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/wall_lights/1.4bpp");
const u8 gTilesetAnims_EliteFour_WallLights_Frame2[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/wall_lights/2.4bpp");
const u8 gTilesetAnims_EliteFour_WallLights_Frame3[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/wall_lights/3.4bpp");
asm(".space 32");

const u8 *const gTilesetAnims_EliteFour_WallLights[] =
{
    gTilesetAnims_EliteFour_WallLights_Frame0,
    gTilesetAnims_EliteFour_WallLights_Frame1,
    gTilesetAnims_EliteFour_WallLights_Frame2,
    gTilesetAnims_EliteFour_WallLights_Frame3,
};

const u8 *const gTilesetAnims_EliteFour_FloorLight[] =
{
    gTilesetAnims_EliteFour_FloorLight_Frame0,
    gTilesetAnims_EliteFour_FloorLight_Frame1,
};

const u8 gTilesetAnims_MauvilleGym_ElectricGates_Frame0[] = INCBIN_U8("data/tilesets/secondary/mauville_gym/anim/electric_gates/0.4bpp");
const u8 gTilesetAnims_MauvilleGym_ElectricGates_Frame1[] = INCBIN_U8("data/tilesets/secondary/mauville_gym/anim/electric_gates/1.4bpp");
asm(".space 32");

const u8 *const gTilesetAnims_MauvilleGym_ElectricGates[] =
{
    gTilesetAnims_MauvilleGym_ElectricGates_Frame0,
    gTilesetAnims_MauvilleGym_ElectricGates_Frame1,
};

const u8 gTilesetAnims_BikeShop_BlinkingLights_Frame0[] = INCBIN_U8("data/tilesets/secondary/bike_shop/anim/blinking_lights/0.4bpp");
const u8 gTilesetAnims_BikeShop_BlinkingLights_Frame1[] = INCBIN_U8("data/tilesets/secondary/bike_shop/anim/blinking_lights/1.4bpp");
asm(".space 32");

const u8 *const gTilesetAnims_BikeShop_BlinkingLights[] =
{
    gTilesetAnims_BikeShop_BlinkingLights_Frame0,
    gTilesetAnims_BikeShop_BlinkingLights_Frame1,
};

static void ResetTilesetAnimBuffer(void)
{
    sTilesetDMA3TransferBufferSize = 0;
    CpuFill32(0, &sTilesetDMA3TransferBuffer, sizeof(sTilesetDMA3TransferBuffer));
}

static void AppendTilesetAnimToBuffer(const void *src, const void *dest, u16 size)
{
    if (sTilesetDMA3TransferBufferSize < 20)
    {
        sTilesetDMA3TransferBuffer[sTilesetDMA3TransferBufferSize].src = src;
        sTilesetDMA3TransferBuffer[sTilesetDMA3TransferBufferSize].dest = dest;
        sTilesetDMA3TransferBuffer[sTilesetDMA3TransferBufferSize].size = size;
        sTilesetDMA3TransferBufferSize++;
    }
}

void TransferTilesetAnimsBuffer(void)
{
    int i;
    for (i = 0; i < sTilesetDMA3TransferBufferSize; i++)
    {
        DmaCopy16(3,
            sTilesetDMA3TransferBuffer[i].src,
            sTilesetDMA3TransferBuffer[i].dest,
            sTilesetDMA3TransferBuffer[i].size);
    }
    sTilesetDMA3TransferBufferSize = 0;
}

void InitTilesetAnimations(void)
{
    ResetTilesetAnimBuffer();
    _InitPrimaryTilesetAnimation();
    _InitSecondaryTilesetAnimation();
}

void InitSecondaryTilesetAnimation(void)
{
    _InitSecondaryTilesetAnimation();
}

void UpdateTilesetAnimations(void)
{
    ResetTilesetAnimBuffer();
    if (++sPrimaryTilesetAnimCounter >= sPrimaryTilesetAnimCounterMax)
    {
        sPrimaryTilesetAnimCounter = 0;
    }
    if (++sSecondaryTilesetAnimCounter >= sSecondaryTilesetAnimCounterMax)
    {
        sSecondaryTilesetAnimCounter = 0;
    }
    if (sPrimaryTilesetAnimCallback)
    {
        sPrimaryTilesetAnimCallback(sPrimaryTilesetAnimCounter);
    }
    if (sSecondaryTilesetAnimCallback)
    {
        sSecondaryTilesetAnimCallback(sSecondaryTilesetAnimCounter);
    }
}

static void _InitPrimaryTilesetAnimation(void)
{
    sPrimaryTilesetAnimCounter = 0;
    sPrimaryTilesetAnimCounterMax = 0;
    sPrimaryTilesetAnimCallback = 0;
    if (gMapHeader.mapLayout->primaryTileset)
    {
        if (gMapHeader.mapLayout->primaryTileset->callback)
        {
            gMapHeader.mapLayout->primaryTileset->callback();
        }
    }
}

static void _InitSecondaryTilesetAnimation(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = 0;
    sSecondaryTilesetAnimCallback = 0;
    if (gMapHeader.mapLayout->secondaryTileset)
    {
        if (gMapHeader.mapLayout->secondaryTileset->callback)
        {
            gMapHeader.mapLayout->secondaryTileset->callback();
        }
    }
}

void InitTilesetAnim_General(void)
{
    sPrimaryTilesetAnimCounter = 0;
    sPrimaryTilesetAnimCounterMax = 0x100;
    sPrimaryTilesetAnimCallback = TilesetAnim_General;
}

void InitTilesetAnim_Building(void)
{
    sPrimaryTilesetAnimCounter = 0;
    sPrimaryTilesetAnimCounterMax = 0x100;
    sPrimaryTilesetAnimCallback = TilesetAnim_Building;
}

static void TilesetAnim_General(u16 timer)
{
    int v1;
    v1 = timer % 0x10;
    if (v1 == 0) QueueAnimTiles_General_Flower(timer / 0x10);
    if (v1 == 1) QueueAnimTiles_General_Water(timer / 0x10);
    if (v1 == 2) QueueAnimTiles_General_SandWaterEdge(timer / 0x10);
    if (v1 == 3) QueueAnimTiles_General_Waterfall(timer / 0x10);
    if (v1 == 4) QueueAnimTiles_General_LandWaterEdge(timer / 0x10);
}

static void TilesetAnim_Building(u16 timer)
{
    if (timer % 8 == 0)
    {
        QueueAnimTiles_Building_TVTurnedOn(timer / 8);
    }
}

static void QueueAnimTiles_General_Flower(u16 timer)
{
    u16 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_Flower[i], BG_TILE_ADDR(127), 0x80);
}

static void QueueAnimTiles_General_Water(u16 timer)
{
    u8 i;
    i = timer % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_Water[i], BG_TILE_ADDR(108), 0x3c0);
}

static void QueueAnimTiles_General_SandWaterEdge(u16 timer)
{
    u16 i;
    i = timer % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_SandWaterEdge[i], BG_TILE_ADDR(116), 0x140);
}

static void QueueAnimTiles_General_Waterfall(u16 timer)
{
    u16 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_Waterfall[i], BG_TILE_ADDR(124), 0xc0);
}

void InitTilesetAnim_Petalburg(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_Rustboro(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_Rustboro;
}

void InitTilesetAnim_Dewford(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_Slateport(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_Mauville(void)
{
    sSecondaryTilesetAnimCounter = sPrimaryTilesetAnimCounter;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_Mauville;
}

void InitTilesetAnim_Lavaridge(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_Lavaridge;
}

void InitTilesetAnim_Fallarbor(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_Fortree(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_Lilycove(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_Mossdeep(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_EverGrande(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_EverGrande;
}

void InitTilesetAnim_Pacifidlog(void)
{
    sSecondaryTilesetAnimCounter = sPrimaryTilesetAnimCounter;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_Pacifidlog;
}

void InitTilesetAnim_Sootopolis(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = 0;
}

void InitTilesetAnim_Underwater(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = 0x80;
    sSecondaryTilesetAnimCallback = TilesetAnim_Underwater;
}

void InitTilesetAnim_SootopolisGym(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = 0xf0;
    sSecondaryTilesetAnimCallback = TilesetAnim_SootopolisGym;
}

void InitTilesetAnim_Cave(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_Cave;
}

void InitTilesetAnim_EliteFour(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = 0x80;
    sSecondaryTilesetAnimCallback = TilesetAnim_EliteFour;
}

void InitTilesetAnim_MauvilleGym(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_MauvilleGym;
}

void InitTilesetAnim_BikeShop(void)
{
    sSecondaryTilesetAnimCounter = 0;
    sSecondaryTilesetAnimCounterMax = sPrimaryTilesetAnimCounterMax;
    sSecondaryTilesetAnimCallback = TilesetAnim_BikeShop;
}

static void TilesetAnim_Rustboro(u16 timer)
{
    int v1;
    v1 = timer % 8;
    if (v1 == 0)
    {
        QueueAnimTiles_Rustboro_WindyWater(timer / 8, 0);
        QueueAnimTiles_Rustboro_Fountain(timer / 8);
    }
    if (v1 == 1) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 1);
    if (v1 == 2) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 2);
    if (v1 == 3) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 3);
    if (v1 == 4) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 4);
    if (v1 == 5) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 5);
    if (v1 == 6) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 6);
    if (v1 == 7) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 7);
}

static void TilesetAnim_Mauville(u16 timer)
{
    int v1;
    v1 = timer % 8;
    if (v1 == 0) QueueAnimTiles_Mauville_Flowers(timer / 8, 0);
    if (v1 == 1) QueueAnimTiles_Mauville_Flowers(timer / 8, 1);
    if (v1 == 2) QueueAnimTiles_Mauville_Flowers(timer / 8, 2);
    if (v1 == 3) QueueAnimTiles_Mauville_Flowers(timer / 8, 3);
    if (v1 == 4) QueueAnimTiles_Mauville_Flowers(timer / 8, 4);
    if (v1 == 5) QueueAnimTiles_Mauville_Flowers(timer / 8, 5);
    if (v1 == 6) QueueAnimTiles_Mauville_Flowers(timer / 8, 6);
    if (v1 == 7) QueueAnimTiles_Mauville_Flowers(timer / 8, 7);
}

static void TilesetAnim_Lavaridge(u16 timer)
{
    int v1;
    v1 = timer % 0x10;
    if (v1 == 0) QueueAnimTiles_Lavaridge_Steam(timer / 0x10);
    if (v1 == 1) QueueAnimTiles_Lavaridge_Lava(timer / 0x10);
}

static void TilesetAnim_EverGrande(u16 timer)
{
    int v1;
    v1 = timer % 8;
    if (v1 == 0) QueueAnimTiles_EverGrande_Flowers(timer / 8, 0);
    if (v1 == 1) QueueAnimTiles_EverGrande_Flowers(timer / 8, 1);
    if (v1 == 2) QueueAnimTiles_EverGrande_Flowers(timer / 8, 2);
    if (v1 == 3) QueueAnimTiles_EverGrande_Flowers(timer / 8, 3);
    if (v1 == 4) QueueAnimTiles_EverGrande_Flowers(timer / 8, 4);
    if (v1 == 5) QueueAnimTiles_EverGrande_Flowers(timer / 8, 5);
    if (v1 == 6) QueueAnimTiles_EverGrande_Flowers(timer / 8, 6);
    if (v1 == 7) QueueAnimTiles_EverGrande_Flowers(timer / 8, 7);
}

static void TilesetAnim_Pacifidlog(u16 timer)
{
    int v1;
    v1 = timer % 0x10;
    if (v1 == 0) QueueAnimTiles_Pacifidlog_LogBridges(timer / 0x10);
    if (v1 == 1) QueueAnimTiles_Pacifidlog_WaterCurrents(timer / 0x10);
}

static void TilesetAnim_Underwater(u16 timer)
{
    int v1;
    v1 = timer % 0x10;
    if (v1 == 0) QueueAnimTiles_Underwater_Seaweed(timer / 0x10);
}

static void TilesetAnim_Cave(u16 timer)
{
    int v1;
    v1 = timer % 0x10;
    if (v1 == 1) QueueAnimTiles_Cave_Lava(timer / 0x10);
}

static void QueueAnimTiles_General_LandWaterEdge(u16 timer)
{
    u16 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_LandWaterEdge[i], BG_TILE_ADDR(120), 0x140);
}

static void QueueAnimTiles_Lavaridge_Steam(u8 timer)
{
    u8 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Steam[i], BG_TILE_ADDR(200), 0x80);
    i = (timer + 2) % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Steam[i], BG_TILE_ADDR(201), 0x80);
}

static void QueueAnimTiles_Pacifidlog_LogBridges(u8 timer)
{
    u8 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Pacifidlog_LogBridges[i], BG_TILE_ADDR(244), 0x3c0);
}

static void QueueAnimTiles_Underwater_Seaweed(u8 timer)
{
    u8 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Underwater_Seaweed[i], BG_TILE_ADDR(252), 0x80);
}

static void QueueAnimTiles_Pacifidlog_WaterCurrents(u8 timer)
{
    u8 i;
    i = timer % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_Pacifidlog_WaterCurrents[i], BG_TILE_ADDR(252), 0x100);
}

static void QueueAnimTiles_Mauville_Flowers(u16 timer_div, u8 timer_mod)
{
    int v1;
    timer_div -= timer_mod;
    if (timer_div < 12)
    {
        v1 = timer_div % 12;
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower1[v1], gTilesetAnims_Mauville_Flower1_VDests[timer_mod], 0x80);
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower2[v1], gTilesetAnims_Mauville_Flower2_VDests[timer_mod], 0x80);
    }
    else
    {
        v1 = timer_div % 4;
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower1_B[v1], gTilesetAnims_Mauville_Flower1_VDests[timer_mod], 0x80);
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower2_B[v1], gTilesetAnims_Mauville_Flower2_VDests[timer_mod], 0x80);
    }
}

static void QueueAnimTiles_Rustboro_WindyWater(u16 timer_div, u8 timer_mod)
{
    int v1;
    timer_div -= timer_mod;

    v1 = timer_div % 8;
    if (gTilesetAnims_Rustboro_WindyWater[v1])
    {
        AppendTilesetAnimToBuffer(gTilesetAnims_Rustboro_WindyWater[v1], gTilesetAnims_Rustboro_WindyWater_VDests[timer_mod], 0x80);
    }
}

static void QueueAnimTiles_Rustboro_Fountain(u16 timer)
{
    u16 i;
    i = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_Rustboro_Fountain[i], BG_TILE_ADDR(240), 0x80);
}

static void QueueAnimTiles_Lavaridge_Lava(u16 timer)
{
    u16 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Cave_Lava[i], BG_TILE_ADDR(168), 0x80);
}

static void QueueAnimTiles_EverGrande_Flowers(u16 timer_div, u8 timer_mod)
{
    int v1;
    timer_div -= timer_mod;
    v1 = timer_div % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_EverGrande_Flowers[v1], gTilesetAnims_EverGrande_VDests[timer_mod], 0x80);
}

static void QueueAnimTiles_Cave_Lava(u16 timer)
{
    u16 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Cave_Lava[i], BG_TILE_ADDR(232), 0x80);
}

static void TilesetAnim_MauvilleGym(u16 timer)
{
    int v1;
    v1 = timer % 2;
    if (!v1)
    {
        QueueAnimTiles_MauvilleGym_ElectricGates(timer / 2);
    }
}

static void TilesetAnim_SootopolisGym(u16 timer)
{
    int v1;
    v1 = timer % 8;
    if (!v1)
    {
        QueueAnimTiles_SootopolisGym_Waterfalls(timer / 8);
    }
}

static void TilesetAnim_EliteFour(u16 timer)
{
    if (timer % 0x40 == 0)
    {
        QueueAnimTiles_EliteFour_GroundLights(timer / 0x40);
    }
    if (timer % 8 == 1)
    {
        QueueAnimTiles_EliteFour_WallLights(timer / 8);
    }
}

static void TilesetAnim_BikeShop(u16 timer)
{
    if (timer % 4 == 0)
    {
        QueueAnimTiles_BikeShop_BlinkingLights(timer / 4);
    }
}

static void QueueAnimTiles_Building_TVTurnedOn(u16 timer)
{
    u16 i;
    i = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_Building_TvTurnedOn[i], BG_TILE_ADDR(124), 0x80);
}

static void QueueAnimTiles_SootopolisGym_Waterfalls(u16 timer)
{
    u16 i;
    i = timer % 3;
    AppendTilesetAnimToBuffer(gTilesetAnims_SootopolisGym_SideWaterfall[i], BG_TILE_ADDR(252), 0x180);
    AppendTilesetAnimToBuffer(gTilesetAnims_SootopolisGym_FrontWaterfall[i], BG_TILE_ADDR(244), 0x280);
}

static void QueueAnimTiles_EliteFour_WallLights(u16 timer)
{
    u16 i;
    i = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_EliteFour_WallLights[i], BG_TILE_ADDR(254), 0x20);
}

static void QueueAnimTiles_EliteFour_GroundLights(u16 timer)
{
    u16 i;
    i = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_EliteFour_FloorLight[i], BG_TILE_ADDR(248), 0x80);
}

static void QueueAnimTiles_MauvilleGym_ElectricGates(u16 timer)
{
    u16 i;
    i = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_MauvilleGym_ElectricGates[i], BG_TILE_ADDR(164), 0x200);
}

static void QueueAnimTiles_BikeShop_BlinkingLights(u16 timer)
{
    u16 i;
    i = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_BikeShop_BlinkingLights[i], BG_TILE_ADDR(252), 0x120);
}
