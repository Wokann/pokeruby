#include "global.h"
#include "tileset_anims.h"

struct TilesetAnimDma
{
    const void *src;
    const void *dest;
    u16 size;
};

EWRAM_DATA static struct TilesetAnimDma sTilesetDMA3TransferBuffer[20] = {0};

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

static void QueueAnimTiles_Building_TvTurnedOn(u16);
static void QueueAnimTiles_MauvilleGym_ElectricGates(u16);
static void QueueAnimTiles_SootopolisGym_Waterfalls(u16);
static void QueueAnimTiles_EliteFour_GroundLights(u16);
static void QueueAnimTiles_EliteFour_WallLights(u16);
static void QueueAnimTiles_BikeShop_BlinkingLights(u16);

static const u8 gTilesetAnims_General_Flower_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/0/1.4bpp");
static const u8 gTilesetAnims_General_Flower_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/0/0.4bpp");
static const u8 gTilesetAnims_General_Flower_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/0/2.4bpp");
asm(".space 32");

static const u8 *const gTilesetAnims_General_Flower[] =
{
    gTilesetAnims_General_Flower_Frame0,
    gTilesetAnims_General_Flower_Frame1,
    gTilesetAnims_General_Flower_Frame0,
    gTilesetAnims_General_Flower_Frame2,
};

static const u8 gTilesetAnims_General_Water_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/1/0.4bpp");
static const u8 gTilesetAnims_General_Water_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/1/1.4bpp");
static const u8 gTilesetAnims_General_Water_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/1/2.4bpp");
static const u8 gTilesetAnims_General_Water_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/1/3.4bpp");
static const u8 gTilesetAnims_General_Water_Frame4[] = INCBIN_U8("data/tilesets/primary/general/anim/1/4.4bpp");
static const u8 gTilesetAnims_General_Water_Frame5[] = INCBIN_U8("data/tilesets/primary/general/anim/1/5.4bpp");
static const u8 gTilesetAnims_General_Water_Frame6[] = INCBIN_U8("data/tilesets/primary/general/anim/1/6.4bpp");
static const u8 gTilesetAnims_General_Water_Frame7[] = INCBIN_U8("data/tilesets/primary/general/anim/1/7.4bpp");

static const u8 *const gTilesetAnims_General_Water[] =
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

static const u8 gTilesetAnims_General_SandWaterEdge_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/2/0.4bpp");
static const u8 gTilesetAnims_General_SandWaterEdge_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/2/1.4bpp");
static const u8 gTilesetAnims_General_SandWaterEdge_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/2/2.4bpp");
static const u8 gTilesetAnims_General_SandWaterEdge_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/2/3.4bpp");
static const u8 gTilesetAnims_General_SandWaterEdge_Frame4[] = INCBIN_U8("data/tilesets/primary/general/anim/2/4.4bpp");
static const u8 gTilesetAnims_General_SandWaterEdge_Frame5[] = INCBIN_U8("data/tilesets/primary/general/anim/2/5.4bpp");
static const u8 gTilesetAnims_General_SandWaterEdge_Frame6[] = INCBIN_U8("data/tilesets/primary/general/anim/2/6.4bpp");

static const u8 *const gTilesetAnims_General_SandWaterEdge[] =
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

static const u8 gTilesetAnims_General_Waterfall_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/3/0.4bpp");
static const u8 gTilesetAnims_General_Waterfall_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/3/1.4bpp");
static const u8 gTilesetAnims_General_Waterfall_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/3/2.4bpp");
static const u8 gTilesetAnims_General_Waterfall_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/3/3.4bpp");

static const u8 *const gTilesetAnims_General_Waterfall[] =
{
    gTilesetAnims_General_Waterfall_Frame0,
    gTilesetAnims_General_Waterfall_Frame1,
    gTilesetAnims_General_Waterfall_Frame2,
    gTilesetAnims_General_Waterfall_Frame3,
};

static const u8 gTilesetAnims_General_LandWaterEdge_Frame0[] = INCBIN_U8("data/tilesets/primary/general/anim/4/0.4bpp");
static const u8 gTilesetAnims_General_LandWaterEdge_Frame1[] = INCBIN_U8("data/tilesets/primary/general/anim/4/1.4bpp");
static const u8 gTilesetAnims_General_LandWaterEdge_Frame2[] = INCBIN_U8("data/tilesets/primary/general/anim/4/2.4bpp");
static const u8 gTilesetAnims_General_LandWaterEdge_Frame3[] = INCBIN_U8("data/tilesets/primary/general/anim/4/3.4bpp");

static const u8 *const gTilesetAnims_General_LandWaterEdge[] =
{
    gTilesetAnims_General_LandWaterEdge_Frame0,
    gTilesetAnims_General_LandWaterEdge_Frame1,
    gTilesetAnims_General_LandWaterEdge_Frame2,
    gTilesetAnims_General_LandWaterEdge_Frame3,
};

static const u8 gTilesetAnims_Lavaridge_Steam_Frame0[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/0.4bpp");
static const u8 gTilesetAnims_Lavaridge_Steam_Frame1[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/1.4bpp");
static const u8 gTilesetAnims_Lavaridge_Steam_Frame2[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/2.4bpp");
static const u8 gTilesetAnims_Lavaridge_Steam_Frame3[] = INCBIN_U8("data/tilesets/secondary/lavaridge/anim/3.4bpp");

static const u8 *const gTilesetAnims_Lavaridge_Steam[] =
{
    gTilesetAnims_Lavaridge_Steam_Frame0,
    gTilesetAnims_Lavaridge_Steam_Frame1,
    gTilesetAnims_Lavaridge_Steam_Frame2,
    gTilesetAnims_Lavaridge_Steam_Frame3,
};

static const u8 gTilesetAnims_Pacifidlog_LogBridges_Frame0[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/0/0.4bpp");
static const u8 gTilesetAnims_Pacifidlog_LogBridges_Frame1[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/0/1.4bpp");
static const u8 gTilesetAnims_Pacifidlog_LogBridges_Frame2[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/0/2.4bpp");

static const u8 *const gTilesetAnims_Pacifidlog_LogBridges[] =
{
    gTilesetAnims_Pacifidlog_LogBridges_Frame0,
    gTilesetAnims_Pacifidlog_LogBridges_Frame1,
    gTilesetAnims_Pacifidlog_LogBridges_Frame2,
    gTilesetAnims_Pacifidlog_LogBridges_Frame1,
};

static const u8 gTilesetAnims_Underwater_Seaweed_Frame0[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/0.4bpp");
static const u8 gTilesetAnims_Underwater_Seaweed_Frame1[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/1.4bpp");
static const u8 gTilesetAnims_Underwater_Seaweed_Frame2[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/2.4bpp");
static const u8 gTilesetAnims_Underwater_Seaweed_Frame3[] = INCBIN_U8("data/tilesets/secondary/underwater/anim/3.4bpp");

static const u8 *const gTilesetAnims_Underwater_Seaweed[] =
{
    gTilesetAnims_Underwater_Seaweed_Frame0,
    gTilesetAnims_Underwater_Seaweed_Frame1,
    gTilesetAnims_Underwater_Seaweed_Frame2,
    gTilesetAnims_Underwater_Seaweed_Frame3
};

static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame0[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/0.4bpp");
static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame1[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/1.4bpp");
static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame2[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/2.4bpp");
static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame3[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/3.4bpp");
static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame4[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/4.4bpp");
static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame5[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/5.4bpp");
static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame6[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/6.4bpp");
static const u8 gTilesetAnims_Pacifidlog_WaterCurrents_Frame7[] = INCBIN_U8("data/tilesets/secondary/pacifidlog/anim/1/7.4bpp");

static const u8 *const gTilesetAnims_Pacifidlog_WaterCurrents[] =
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

static const u8 gTilesetAnims_Mauville_Flower1_Frame0[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/0/0.4bpp");
static const u8 gTilesetAnims_Mauville_Flower1_Frame1[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/0/1.4bpp");
static const u8 gTilesetAnims_Mauville_Flower1_Frame2[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/0/2.4bpp");
static const u8 gTilesetAnims_Mauville_Flower1_Frame3[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/0/3.4bpp");
static const u8 gTilesetAnims_Mauville_Flower1_Frame4[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/0/4.4bpp");

static const u8 gTilesetAnims_Mauville_Flower2_Frame0[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/1/0.4bpp");
static const u8 gTilesetAnims_Mauville_Flower2_Frame1[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/1/1.4bpp");
static const u8 gTilesetAnims_Mauville_Flower2_Frame2[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/1/2.4bpp");
static const u8 gTilesetAnims_Mauville_Flower2_Frame3[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/1/3.4bpp");
static const u8 gTilesetAnims_Mauville_Flower2_Frame4[] = INCBIN_U8("data/tilesets/secondary/mauville/anim/1/4.4bpp");

static const void *const gTilesetAnims_Mauville_Flower1_VDests[] =
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

static const void *const gTilesetAnims_Mauville_Flower2_VDests[] =
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

static const u8 *const gTilesetAnims_Mauville_Flower1[] =
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

static const u8 *const gTilesetAnims_Mauville_Flower2[] =
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

static const u8 *const gTilesetAnims_Mauville_Flower1_B[] =
{
    gTilesetAnims_Mauville_Flower1_Frame0,
    gTilesetAnims_Mauville_Flower1_Frame0,
    gTilesetAnims_Mauville_Flower1_Frame4,
    gTilesetAnims_Mauville_Flower1_Frame4,
};

static const u8 *const gTilesetAnims_Mauville_Flower2_B[] =
{
    gTilesetAnims_Mauville_Flower2_Frame0,
    gTilesetAnims_Mauville_Flower2_Frame0,
    gTilesetAnims_Mauville_Flower2_Frame4,
    gTilesetAnims_Mauville_Flower2_Frame4,
};

static const u8 gTilesetAnims_Rustboro_WindyWater_Frame0[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/0.4bpp");
static const u8 gTilesetAnims_Rustboro_WindyWater_Frame1[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/1.4bpp");
static const u8 gTilesetAnims_Rustboro_WindyWater_Frame2[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/2.4bpp");
static const u8 gTilesetAnims_Rustboro_WindyWater_Frame3[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/3.4bpp");
static const u8 gTilesetAnims_Rustboro_WindyWater_Frame4[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/4.4bpp");
static const u8 gTilesetAnims_Rustboro_WindyWater_Frame5[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/5.4bpp");
static const u8 gTilesetAnims_Rustboro_WindyWater_Frame6[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/6.4bpp");
static const u8 gTilesetAnims_Rustboro_WindyWater_Frame7[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/0/7.4bpp");

static const void *const gTilesetAnims_Rustboro_WindyWater_VDests[] =
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

static const u8 *const gTilesetAnims_Rustboro_WindyWater[] =
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

static const u8 gTilesetAnims_Rustboro_Fountain_Frame0[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/1/0.4bpp");
static const u8 gTilesetAnims_Rustboro_Fountain_Frame1[] = INCBIN_U8("data/tilesets/secondary/rustboro/anim/1/1.4bpp");

static const u8 *const gTilesetAnims_Rustboro_Fountain[] =
{
    gTilesetAnims_Rustboro_Fountain_Frame0,
    gTilesetAnims_Rustboro_Fountain_Frame1,
};

static const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame0[] = INCBIN_U8("data/tilesets/secondary/cave/anim/0.4bpp");
static const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame1[] = INCBIN_U8("data/tilesets/secondary/cave/anim/1.4bpp");
static const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame2[] = INCBIN_U8("data/tilesets/secondary/cave/anim/2.4bpp");
static const u8 gTilesetAnims_Lavaridge_Cave_Lava_Frame3[] = INCBIN_U8("data/tilesets/secondary/cave/anim/3.4bpp");
// unused anim tiles for cave
static const u8 gTilesetAnims_Unused1_Frame0[] = INCBIN_U8("data/tilesets/secondary/cave/anim/unused/0.4bpp");
static const u8 gTilesetAnims_Unused1_Frame1[] = INCBIN_U8("data/tilesets/secondary/cave/anim/unused/1.4bpp");
static const u8 gTilesetAnims_Unused1_Frame2[] = INCBIN_U8("data/tilesets/secondary/cave/anim/unused/2.4bpp");
static const u8 gTilesetAnims_Unused1_Frame3[] = INCBIN_U8("data/tilesets/secondary/cave/anim/unused/3.4bpp");
asm(".space 32");

static const u8 *const gTilesetAnims_Lavaridge_Cave_Lava[] =
{
    gTilesetAnims_Lavaridge_Cave_Lava_Frame0,
    gTilesetAnims_Lavaridge_Cave_Lava_Frame1,
    gTilesetAnims_Lavaridge_Cave_Lava_Frame2,
    gTilesetAnims_Lavaridge_Cave_Lava_Frame3,
};

static const u8 gTilesetAnims_EverGrande_Flowers_Frame0[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/0.4bpp");
static const u8 gTilesetAnims_EverGrande_Flowers_Frame1[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/1.4bpp");
static const u8 gTilesetAnims_EverGrande_Flowers_Frame2[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/2.4bpp");
static const u8 gTilesetAnims_EverGrande_Flowers_Frame3[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/3.4bpp");
static const u8 gTilesetAnims_EverGrande_Flowers_Frame4[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/4.4bpp");
static const u8 gTilesetAnims_EverGrande_Flowers_Frame5[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/5.4bpp");
static const u8 gTilesetAnims_EverGrande_Flowers_Frame6[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/6.4bpp");
static const u8 gTilesetAnims_EverGrande_Flowers_Frame7[] = INCBIN_U8("data/tilesets/secondary/ever_grande/anim/7.4bpp");
asm(".space 32");

static const void *const gTilesetAnims_EverGrande_VDests[] =
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

static const u8 *const gTilesetAnims_EverGrande_Flowers[] =
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

static const u8 gTilesetAnims_Building_TvTurnedOn_Frame0[] = INCBIN_U8("data/tilesets/primary/building/anim/0.4bpp");
static const u8 gTilesetAnims_Building_TvTurnedOn_Frame1[] = INCBIN_U8("data/tilesets/primary/building/anim/1.4bpp");

static const u8 *const gTilesetAnims_Building_TvTurnedOn[] =
{
    gTilesetAnims_Building_TvTurnedOn_Frame0,
    gTilesetAnims_Building_TvTurnedOn_Frame1,
};

static const u8 gTilesetAnims_SootopolisGym_SideWaterfall_Frame0[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/0/0.4bpp");
static const u8 gTilesetAnims_SootopolisGym_SideWaterfall_Frame1[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/0/1.4bpp");
static const u8 gTilesetAnims_SootopolisGym_SideWaterfall_Frame2[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/0/2.4bpp");
static const u8 gTilesetAnims_SootopolisGym_FrontWaterfall_Frame0[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/1/0.4bpp");
static const u8 gTilesetAnims_SootopolisGym_FrontWaterfall_Frame1[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/1/1.4bpp");
static const u8 gTilesetAnims_SootopolisGym_FrontWaterfall_Frame2[] = INCBIN_U8("data/tilesets/secondary/sootopolis_gym/anim/1/2.4bpp");

static const u8 *const gTilesetAnims_SootopolisGym_SideWaterfall[] =
{
    gTilesetAnims_SootopolisGym_SideWaterfall_Frame0,
    gTilesetAnims_SootopolisGym_SideWaterfall_Frame1,
    gTilesetAnims_SootopolisGym_SideWaterfall_Frame2,
};

static const u8 *const gTilesetAnims_SootopolisGym_FrontWaterfall[] =
{
    gTilesetAnims_SootopolisGym_FrontWaterfall_Frame0,
    gTilesetAnims_SootopolisGym_FrontWaterfall_Frame1,
    gTilesetAnims_SootopolisGym_FrontWaterfall_Frame2,
};

static const u8 gTilesetAnims_EliteFour_FloorLight_Frame0[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/1/0.4bpp");
static const u8 gTilesetAnims_EliteFour_FloorLight_Frame1[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/1/1.4bpp");
static const u8 gTilesetAnims_EliteFour_WallLights_Frame0[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/0/0.4bpp");
static const u8 gTilesetAnims_EliteFour_WallLights_Frame1[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/0/1.4bpp");
static const u8 gTilesetAnims_EliteFour_WallLights_Frame2[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/0/2.4bpp");
static const u8 gTilesetAnims_EliteFour_WallLights_Frame3[] = INCBIN_U8("data/tilesets/secondary/elite_four/anim/0/3.4bpp");
asm(".space 32");

static const u8 *const gTilesetAnims_EliteFour_WallLights[] =
{
    gTilesetAnims_EliteFour_WallLights_Frame0,
    gTilesetAnims_EliteFour_WallLights_Frame1,
    gTilesetAnims_EliteFour_WallLights_Frame2,
    gTilesetAnims_EliteFour_WallLights_Frame3,
};

static const u8 *const gTilesetAnims_EliteFour_FloorLight[] =
{
    gTilesetAnims_EliteFour_FloorLight_Frame0,
    gTilesetAnims_EliteFour_FloorLight_Frame1,
};

static const u8 gTilesetAnims_MauvilleGym_ElectricGates_Frame0[] = INCBIN_U8("data/tilesets/secondary/mauville_gym/anim/0.4bpp");
static const u8 gTilesetAnims_MauvilleGym_ElectricGates_Frame1[] = INCBIN_U8("data/tilesets/secondary/mauville_gym/anim/1.4bpp");
asm(".space 32");

static const u8 *const gTilesetAnims_MauvilleGym_ElectricGates[] =
{
    gTilesetAnims_MauvilleGym_ElectricGates_Frame0,
    gTilesetAnims_MauvilleGym_ElectricGates_Frame1,
};

static const u8 gTilesetAnims_BikeShop_BlinkingLights_Frame0[] = INCBIN_U8("data/tilesets/secondary/bike_shop/anim/0.4bpp");
static const u8 gTilesetAnims_BikeShop_BlinkingLights_Frame1[] = INCBIN_U8("data/tilesets/secondary/bike_shop/anim/1.4bpp");
asm(".space 32");

static const u8 *const gTilesetAnims_BikeShop_BlinkingLights[] =
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
    int timerMod;
    timerMod = timer % 0x10;
    if (timerMod == 0) QueueAnimTiles_General_Flower(timer / 0x10);
    if (timerMod == 1) QueueAnimTiles_General_Water(timer / 0x10);
    if (timerMod == 2) QueueAnimTiles_General_SandWaterEdge(timer / 0x10);
    if (timerMod == 3) QueueAnimTiles_General_Waterfall(timer / 0x10);
    if (timerMod == 4) QueueAnimTiles_General_LandWaterEdge(timer / 0x10);
}

static void TilesetAnim_Building(u16 timer)
{
    if (timer % 8 == 0)
    {
        QueueAnimTiles_Building_TvTurnedOn(timer / 8);
    }
}

static void QueueAnimTiles_General_Flower(u16 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_Flower[frame], BG_TILE_ADDR(127), 0x80);
}

static void QueueAnimTiles_General_Water(u16 timer)
{
    u8 frame;
    frame = timer % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_Water[frame], BG_TILE_ADDR(108), 0x3c0);
}

static void QueueAnimTiles_General_SandWaterEdge(u16 timer)
{
    int frame;
    frame = timer % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_SandWaterEdge[frame], BG_TILE_ADDR(116), 0x140);
}

static void QueueAnimTiles_General_Waterfall(u16 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_Waterfall[frame], BG_TILE_ADDR(124), 0xc0);
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
    int timerMod;
    timerMod = timer % 8;
    if (timerMod == 0)
    {
        QueueAnimTiles_Rustboro_WindyWater(timer / 8, 0);
        QueueAnimTiles_Rustboro_Fountain(timer / 8);
    }
    if (timerMod == 1) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 1);
    if (timerMod == 2) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 2);
    if (timerMod == 3) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 3);
    if (timerMod == 4) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 4);
    if (timerMod == 5) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 5);
    if (timerMod == 6) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 6);
    if (timerMod == 7) QueueAnimTiles_Rustboro_WindyWater(timer / 8, 7);
}

static void TilesetAnim_Mauville(u16 timer)
{
    int timerMod;
    timerMod = timer % 8;
    if (timerMod == 0) QueueAnimTiles_Mauville_Flowers(timer / 8, 0);
    if (timerMod == 1) QueueAnimTiles_Mauville_Flowers(timer / 8, 1);
    if (timerMod == 2) QueueAnimTiles_Mauville_Flowers(timer / 8, 2);
    if (timerMod == 3) QueueAnimTiles_Mauville_Flowers(timer / 8, 3);
    if (timerMod == 4) QueueAnimTiles_Mauville_Flowers(timer / 8, 4);
    if (timerMod == 5) QueueAnimTiles_Mauville_Flowers(timer / 8, 5);
    if (timerMod == 6) QueueAnimTiles_Mauville_Flowers(timer / 8, 6);
    if (timerMod == 7) QueueAnimTiles_Mauville_Flowers(timer / 8, 7);
}

static void TilesetAnim_Lavaridge(u16 timer)
{
    int timerMod;
    timerMod = timer % 0x10;
    if (timerMod == 0) QueueAnimTiles_Lavaridge_Steam(timer / 0x10);
    if (timerMod == 1) QueueAnimTiles_Lavaridge_Lava(timer / 0x10);
}

static void TilesetAnim_EverGrande(u16 timer)
{
    int timerMod;
    timerMod = timer % 8;
    if (timerMod == 0) QueueAnimTiles_EverGrande_Flowers(timer / 8, 0);
    if (timerMod == 1) QueueAnimTiles_EverGrande_Flowers(timer / 8, 1);
    if (timerMod == 2) QueueAnimTiles_EverGrande_Flowers(timer / 8, 2);
    if (timerMod == 3) QueueAnimTiles_EverGrande_Flowers(timer / 8, 3);
    if (timerMod == 4) QueueAnimTiles_EverGrande_Flowers(timer / 8, 4);
    if (timerMod == 5) QueueAnimTiles_EverGrande_Flowers(timer / 8, 5);
    if (timerMod == 6) QueueAnimTiles_EverGrande_Flowers(timer / 8, 6);
    if (timerMod == 7) QueueAnimTiles_EverGrande_Flowers(timer / 8, 7);
}

static void TilesetAnim_Pacifidlog(u16 timer)
{
    int timerMod;
    timerMod = timer % 0x10;
    if (timerMod == 0) QueueAnimTiles_Pacifidlog_LogBridges(timer / 0x10);
    if (timerMod == 1) QueueAnimTiles_Pacifidlog_WaterCurrents(timer / 0x10);
}

static void TilesetAnim_Underwater(u16 timer)
{
    int timerMod;
    timerMod = timer % 0x10;
    if (timerMod == 0) QueueAnimTiles_Underwater_Seaweed(timer / 0x10);
}

static void TilesetAnim_Cave(u16 timer)
{
    int timerMod;
    timerMod = timer % 0x10;
    if (timerMod == 1) QueueAnimTiles_Cave_Lava(timer / 0x10);
}

static void QueueAnimTiles_General_LandWaterEdge(u16 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_General_LandWaterEdge[frame], BG_TILE_ADDR(120), 0x140);
}

static void QueueAnimTiles_Lavaridge_Steam(u8 timer)
{
    u8 frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Steam[frame], BG_TILE_ADDR(200), 0x80);
    frame = (timer + 2) % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Steam[frame], BG_TILE_ADDR(201), 0x80);
}

static void QueueAnimTiles_Pacifidlog_LogBridges(u8 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Pacifidlog_LogBridges[frame], BG_TILE_ADDR(244), 0x3c0);
}

static void QueueAnimTiles_Underwater_Seaweed(u8 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Underwater_Seaweed[frame], BG_TILE_ADDR(252), 0x80);
}

static void QueueAnimTiles_Pacifidlog_WaterCurrents(u8 timer)
{
    int frame;
    frame = timer % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_Pacifidlog_WaterCurrents[frame], BG_TILE_ADDR(252), 0x100);
}

static void QueueAnimTiles_Mauville_Flowers(u16 timerDiv, u8 timerMod)
{
    int frame;
    timerDiv -= timerMod;
    if (timerDiv < 12)
    {
        frame = timerDiv % 12;
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower1[frame], gTilesetAnims_Mauville_Flower1_VDests[timerMod], 0x80);
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower2[frame], gTilesetAnims_Mauville_Flower2_VDests[timerMod], 0x80);
    }
    else
    {
        frame = timerDiv % 4;
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower1_B[frame], gTilesetAnims_Mauville_Flower1_VDests[timerMod], 0x80);
        AppendTilesetAnimToBuffer(gTilesetAnims_Mauville_Flower2_B[frame], gTilesetAnims_Mauville_Flower2_VDests[timerMod], 0x80);
    }
}

static void QueueAnimTiles_Rustboro_WindyWater(u16 timerDiv, u8 timerMod)
{
    int frame;
    timerDiv -= timerMod;

    frame = timerDiv % 8;
    if (gTilesetAnims_Rustboro_WindyWater[frame])
    {
        AppendTilesetAnimToBuffer(gTilesetAnims_Rustboro_WindyWater[frame], gTilesetAnims_Rustboro_WindyWater_VDests[timerMod], 0x80);
    }
}

static void QueueAnimTiles_Rustboro_Fountain(u16 timer)
{
    int frame;
    frame = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_Rustboro_Fountain[frame], BG_TILE_ADDR(240), 0x80);
}

static void QueueAnimTiles_Lavaridge_Lava(u16 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Cave_Lava[frame], BG_TILE_ADDR(168), 0x80);
}

static void QueueAnimTiles_EverGrande_Flowers(u16 timerDiv, u8 timerMod)
{
    int frame;
    timerDiv -= timerMod;
    frame = timerDiv % 8;
    AppendTilesetAnimToBuffer(gTilesetAnims_EverGrande_Flowers[frame], gTilesetAnims_EverGrande_VDests[timerMod], 0x80);
}

static void QueueAnimTiles_Cave_Lava(u16 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_Lavaridge_Cave_Lava[frame], BG_TILE_ADDR(232), 0x80);
}

static void TilesetAnim_MauvilleGym(u16 timer)
{
    int timerMod;
    timerMod = timer % 2;
    if (!timerMod)
    {
        QueueAnimTiles_MauvilleGym_ElectricGates(timer / 2);
    }
}

static void TilesetAnim_SootopolisGym(u16 timer)
{
    int timerMod;
    timerMod = timer % 8;
    if (!timerMod)
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

static void QueueAnimTiles_Building_TvTurnedOn(u16 timer)
{
    int frame;
    frame = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_Building_TvTurnedOn[frame], BG_TILE_ADDR(124), 0x80);
}

static void QueueAnimTiles_SootopolisGym_Waterfalls(u16 timer)
{
    int frame;
    frame = timer % 3;
    AppendTilesetAnimToBuffer(gTilesetAnims_SootopolisGym_SideWaterfall[frame], BG_TILE_ADDR(252), 0x180);
    AppendTilesetAnimToBuffer(gTilesetAnims_SootopolisGym_FrontWaterfall[frame], BG_TILE_ADDR(244), 0x280);
}

static void QueueAnimTiles_EliteFour_WallLights(u16 timer)
{
    int frame;
    frame = timer % 4;
    AppendTilesetAnimToBuffer(gTilesetAnims_EliteFour_WallLights[frame], BG_TILE_ADDR(254), 0x20);
}

static void QueueAnimTiles_EliteFour_GroundLights(u16 timer)
{
    int frame;
    frame = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_EliteFour_FloorLight[frame], BG_TILE_ADDR(248), 0x80);
}

static void QueueAnimTiles_MauvilleGym_ElectricGates(u16 timer)
{
    int frame;
    frame = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_MauvilleGym_ElectricGates[frame], BG_TILE_ADDR(164), 0x200);
}

static void QueueAnimTiles_BikeShop_BlinkingLights(u16 timer)
{
    int frame;
    frame = timer % 2;
    AppendTilesetAnimToBuffer(gTilesetAnims_BikeShop_BlinkingLights[frame], BG_TILE_ADDR(252), 0x120);
}
