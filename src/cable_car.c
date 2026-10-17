
// Includes
#include "global.h"
#include "ewram.h"
#include "overworld.h"
#include "palette.h"
#include "random.h"
#include "main.h"
#include "menu.h"
#include "script.h"
#include "task.h"
#include "sound.h"
#include "start_menu.h"
#include "graphics.h"
#include "constants/songs.h"
#include "decompress.h"
#include "field_weather.h"
#include "event_object_movement.h"
#include "scanline_effect.h"
#include "event_data.h"
#include "cable_car_util.h"
#include "constants/event_objects.h"
#include "constants/weather.h"

// Static type declarations

struct CableCarEwramStruct1
{
    u8 bgTaskId;
    u8 state;
    u8 weather;
    u8 unk_0003;
    u16 weatherDelay;
    u16 timer;
    u8 bg0HorizontalOffset;
    u8 bg0VerticalOffset;
    u8 unused0[2];
    u8 bg1HorizontalOffset;
    u8 bg1VerticalOffset;
    u8 unused1[6];
    u8 bg3HorizontalOffset;
    u8 bg3VerticalOffset;
    u8 unk_0016;
    u8 unk_0017;
    u8 groundTileIdx;
    u8 groundSegmentXStart;
    u8 groundSegmentYStart;
    u8 groundTilemapOffset;
    u8 groundTimer;
    u8 groundXOffset;
    u8 groundYOffset;
    u8 groundXBase;
    u8 groundYBase;
    u16 groundTileBuffer[9][12];
    u8 unused3[2];
    u16 unk_00fc[0x20][0x20];
    u16 unk_08fc[0x20][0x20];
}; // size 0x10FC

struct CableCarEwramStruct2
{
    /* 0x000 */ u16 groundTilemap[0xb4];
    /* 0x168 */ u16 treesTilemap[0x1e0];
    /* 0x528 */ u16 bgMountainsTilemap[0x258];
    /* 0x9d8 */ u16 pylonPoleTilemap[0x628]; // size not actually known
};

// Static RAM declarations

static EWRAM_DATA struct CableCarEwramStruct1 *sCableCar = NULL;
static EWRAM_DATA u8 sGroundX_Up = 0;
static EWRAM_DATA u8 sGroundY_Up = 0;
static EWRAM_DATA u8 sGroundSegmentY_Up = 0;
static EWRAM_DATA u8 sGroundX_Down = 0;
static EWRAM_DATA u8 sGroundY_Down = 0;
static EWRAM_DATA u8 sGroundSegmentY_Down = 0;
EWRAM_DATA u32 filler_02039280 = 0;

// Static ROM declarations

static void CB2_LoadCableCar(void);
static void CB2_CableCar(void);
static void Task_CableCar(u8 taskId);
static void Task_AnimateBgGoingUp(u8 taskId);
static void Task_AnimateBgGoingDown(u8 taskId);
static void VBlankCB_CableCar(void);
static void SpriteCB_Cable(struct Sprite *sprite);
static void SpriteCB_CableCar(struct Sprite *sprite);
static void SpriteCB_HikerGoingUp(struct Sprite *sprite);
static void SpriteCB_HikerGoingDown(struct Sprite *sprite);
static void SetBgRegs(bool8 active);
static void CreateCableCarSprites(void);
static void AnimateGroundGoingUp(void);
static void AnimateGroundGoingDown(void);
static void DrawNextGroundSegmentGoingUp(void);
static void DrawNextGroundSegmentGoingDown(void);
static void InitGroundTilemapData(bool8 goingDown);

// .rodata

#if DEBUG

void debug_sub_8138D74(void);
void debug_sub_8138D8C(void);
u8 debug_sub_8138C14(void);
u8 debug_sub_8138C34(void);
u8 debug_sub_810CD9C(void);

const u8 Str_842DBD0[] = _("CABLE CAR U");
const u8 Str_842DBDC[] = _("CABLE CAR D");
const u8 Str_842DBE8[] = _("ROULETTE1");
const u8 Str_842DBF2[] = _("ROULETTE3");
const u8 Str_842DBFC[] = _("View a MAIL");

const struct MenuAction gUnkDebug4Menu[] =
{
    {Str_842DBD0, (u8 (*)(void))debug_sub_8138D74},  // why do these two functions have a different prototype?
    {Str_842DBDC, (u8 (*)(void))debug_sub_8138D8C},
    {Str_842DBE8, debug_sub_8138C14},
    {Str_842DBF2, debug_sub_8138C34},
    {Str_842DBFC, debug_sub_810CD9C},
};

#endif

static const u8 sGround_Tilemap[] = INCBIN_U8("graphics/cable_car/ground.bin.lz");

static const u8 sTrees_Tilemap[] = INCBIN_U8("graphics/cable_car/trees.bin.lz");

static const u8 sBgMountains_Tilemap[] = INCBIN_U8("graphics/cable_car/bg_mountains.bin.lz");

static const u16 sPylonTop_Tilemap[] = {
    0x3000,
    0x3001,
    0x3002,
    0x3003,
    0x3004,
    0x3005,
    0x3006,
    0x3007,
    0x3008,
    0x3009
};

static const u8 sPylonPole_Tilemap[] = INCBIN_U8("graphics/cable_car/pylon_pole.bin.lz");

static const struct CompressedSpriteSheet sSpriteSheets[] = {
    { gCableCar_Gfx,     0x800, 1 },
    { gCableCarDoor_Gfx,  0x40, 2 },
    { gCableCarCable_Gfx,  0x80, 3 },
    {                             }
};
static const struct SpritePalette sSpritePalettes[] = {
    { gCableCar_Pal, 1 },
    {                  }
};

static const struct OamData sOam_CableCar = {
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .size = 3,
    .priority = 2
};

static const struct OamData sOam_CableCarDoor = {
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .shape = ST_OAM_H_RECTANGLE,
    .priority = 2
};

static const struct OamData sOam_Cable = {
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .size = 1,
    .priority = 2
};

static const struct SpriteTemplate gSpriteTemplate_8401D40[] =
{
    {
        .tileTag = 1,
        .paletteTag = 1,
        .oam = &sOam_CableCar,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_CableCar,
    },
    {
        .tileTag = 2,
        .paletteTag = 1,
        .oam = &sOam_CableCarDoor,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_CableCar,
    },
    {
        .tileTag = 3,
        .paletteTag = 1,
        .oam = &sOam_Cable,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_Cable,
    }
};

#if DEBUG

extern u8 unk_203955C;

extern const u8 Roulette_EventScript_Table1[];
extern const u8 Roulette_EventScript_Table2[];

u8 debug_sub_8138C14(void)
{
    unk_203955C = 1;
    ScriptContext_SetupScript(Roulette_EventScript_Table1);
    CloseMenu();
    return 1;
}

u8 debug_sub_8138C34(void)
{
    unk_203955C = 1;
    ScriptContext_SetupScript(Roulette_EventScript_Table2);
    CloseMenu();
    return 1;
}

u8 debug_sub_8138C54(void)
{
    if (gMain.newKeys == DPAD_UP)
        Menu_MoveCursor(-1);
    if (gMain.newKeys == DPAD_DOWN)
        Menu_MoveCursor(1);
    if (gMain.newKeys == A_BUTTON)
        return gUnkDebug4Menu[Menu_GetCursorPos()].func();
    if (gMain.newKeys == (R_BUTTON | A_BUTTON))
    {
        gSpecialVar_0x8004 = 1;
        return gUnkDebug4Menu[Menu_GetCursorPos()].func();
    }
    if (gMain.newKeys == B_BUTTON)
    {
        CloseMenu();
        return 1;
    }
    return 0;
}

u8 debug_sub_8138CC4(void)
{
    gSpecialVar_0x8004 = 0;
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(19, 0, 29, 12);
    Menu_PrintItems(20, 1, 5, gUnkDebug4Menu);
    InitMenu(0, 20, 1, 5, 0, 8);
    gMenuCallback = debug_sub_8138C54;
    return 0;
}

#endif

static void Task_LoadCableCar(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_LoadCableCar);
        DestroyTask(taskId);
    }
}

void CableCar(void)
{
    LockPlayerFieldControls();
    CreateTask(Task_LoadCableCar, 1);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

#if DEBUG

void debug_sub_8138D74(void)
{
    gSpecialVar_0x8004 = 0;
    CloseMenu();
    CableCar();
}

void debug_sub_8138D8C(void)
{
    gSpecialVar_0x8004 = 1;
    CloseMenu();
    CableCar();
}

#endif

static void CB2_LoadCableCar(void)
{
    u8 i;
    u16 imebak;

    switch (gMain.state)
    {
        case 0:
        default:
            SetVBlankCallback(NULL);
            ScanlineEffect_Stop();
            DmaFill16Large(3, 0, VRAM, VRAM_SIZE, 0x1000);
            DmaFill32Defvars(3, 0, OAM, OAM_SIZE);
            DmaFill16Defvars(3, 0, PLTT, PLTT_SIZE);
            sCableCar = eCableCar1;
            DmaFill16Large(3, 0, eCableCar1, 0x10FC, 0x1000);
            gMain.state++;
            break;
        case 1:
            ResetSpriteData();
            ResetTasks();
            FreeAllSpritePalettes();
            ResetPaletteFade();
            StartWeather();
            for (i = 0; i < 20; i++)
            {
                gWeatherPtr->sprites.s2.ashSprites[i] = NULL;
            }
            InitMapMusic();
            ResetMapMusic();
            gSpriteCoordOffsetX = gSpriteCoordOffsetY = 0;
            gMain.state++;
            break;
        case 2:
            for (i = 0; i < 3; i++)
            {
                LoadCompressedSpriteSheet(&sSpriteSheets[i]);
            }
            LoadSpritePalettes(sSpritePalettes);
            LZDecompressWram(sGround_Tilemap, eCableCar2->groundTilemap);
            LZDecompressWram(sTrees_Tilemap, eCableCar2->treesTilemap);
            LZDecompressWram(sBgMountains_Tilemap, eCableCar2->bgMountainsTilemap);
            LZDecompressWram(sPylonPole_Tilemap, eCableCar2->pylonPoleTilemap);
            LoadPalette(gCableCarBg_Pal, 0, 0x80);
            LZ77UnCompVram(gCableCarBg_Gfx, (u16 *)BG_VRAM);
            gMain.state++;
            break;
        case 3:
            CreateCableCarSprites();
            RunTasks();
            gMain.state++;
            break;
        case 4:
            if (sCableCar->weather == 7)
            {
                gMain.state++;
            }
            else if (gWeatherPtr->sprites.s2.ashSprites[0] != NULL)
            {
                for (i = 0; i < 20; i++)
                {
                    if (gWeatherPtr->sprites.s2.ashSprites[i] != NULL)
                    {
                        gWeatherPtr->sprites.s2.ashSprites[i]->oam.priority = 0;
                    }
                }
                gMain.state++;
            }
            break;
        case 5:
            CableCarUtil_CopyWrapped((void *)BG_SCREEN_ADDR(29), eCableCar2->treesTilemap, 0, 17, 32, 15);
            CableCarUtil_CopyWrapped((void *)BG_SCREEN_ADDR(30), eCableCar2->bgMountainsTilemap, 0, 0, 30, 20);
            CableCarUtil_CopyWrapped(sCableCar->unk_08fc, sPylonTop_Tilemap, 0, 0, 5, 2);
            CableCarUtil_CopyWrapped(sCableCar->unk_08fc, eCableCar2->pylonPoleTilemap, 0, 2, 2, 20);
            gMain.state++;
            break;
        case 6:
            InitGroundTilemapData(gSpecialVar_0x8004);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x48, 0, 14, 12, 3);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x6C, 12, 17, 12, 3);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x90, 24, 20, 12, 3);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x00, 0, 17, 12, 3);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x24, 0, 20, 12, 3);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x00, 12, 20, 12, 3);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x24, 12, 23, 12, 3);
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x00, 24, 23, 12, 3);
            gMain.state++;
            break;
        case 7:
            BeginNormalPaletteFade(0xFFFFFFFF, 3, 16, 0, RGB(0, 0, 0));
            FadeInNewBGM(MUS_CABLE_CAR, 1);
            SetBgRegs(1);
            gMain.state++;
            break;
        case 8:
            imebak = REG_IME;
            REG_IME = 0;
            REG_IE |= INTR_FLAG_VBLANK;
            REG_IME = imebak;
            SetVBlankCallback(VBlankCB_CableCar);
            SetMainCallback2(CB2_CableCar);
            CreateTask(Task_CableCar, 0);
            if (gSpecialVar_0x8004 == 0)
            {
                sCableCar->bgTaskId = CreateTask(Task_AnimateBgGoingUp, 1);
            }
            else
            {
                sCableCar->bgTaskId = CreateTask(Task_AnimateBgGoingDown, 1);
            }
            break;
    }
}

static void CB2_CableCar(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    MapMusicMain();
}

static void CB2_EndCableCar(void)
{
    u8 i;

    i = 0;
    SetBgRegs(0);
    gSpriteCoordOffsetX = 0;
    SetCurrentAndNextWeatherNoDelay(WEATHER_NONE);
    for (; i < 20; i++)
    {
        gWeatherPtr->sprites.s2.ashSprites[i] = NULL;
    }
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade();
    DmaFill32Large(3, 0, gSharedMem, 0x20000, 0x1000);
    sCableCar = NULL;
    DmaFill16Large(3, 0, VRAM, VRAM_SIZE, 0x1000);
    DmaFill32Defvars(3, 0, OAM, OAM_SIZE);
    DmaFill16Defvars(3, 0, PLTT, PLTT_SIZE);
    WarpIntoMap();
    gFieldCallback = NULL;
    SetMainCallback2(CB2_LoadMap);
}

static void Task_CableCar(u8 taskId)
{
    u8 i;

    i = 0;
    sCableCar->timer++;
    switch (sCableCar->state)
    {
        case 0:
            if (sCableCar->timer == sCableCar->weatherDelay)
            {
                SetNextWeather(sCableCar->weather);
                sCableCar->state = 1;
            }
            break;
        case 1:
            switch (sCableCar->weather)
            {
                case 7:
                    if (gWeatherPtr->sprites.s2.ashSprites[0] != NULL && gWeatherPtr->sprites.s2.ashSprites[0]->oam.priority != 0)
                    {
                        for (; i < 20; i++)
                        {
                            if (gWeatherPtr->sprites.s2.ashSprites[i] != NULL)
                            {
                                gWeatherPtr->sprites.s2.ashSprites[i]->oam.priority = 0;
                            }
                        }
                        sCableCar->state = 2;
                    }
                    break;
                case 2:
                    if (gWeatherPtr->currWeather == 2)
                    {
                        sCableCar->state = 2;
                    }
                    else if (sCableCar->timer >= sCableCar->weatherDelay + 8)
                    {
                        for (; i < 20; i++)
                        {
                            if (gWeatherPtr->sprites.s2.ashSprites[i] != NULL)
                            {
                                gWeatherPtr->sprites.s2.ashSprites[i]->invisible ^= TRUE;
                            }
                        }
                    }
                    break;
            }
            break;
        case 2:
            if (sCableCar->timer == 570)
            {
                sCableCar->state = 3;
                BeginNormalPaletteFade(0xFFFFFFFF, 3, 0, 16, RGB(0, 0, 0));
                FadeOutBGM(4);
            }
            break;
        case 3:
            if (!gPaletteFade.active)
            {
                sCableCar->state = 255;
            }
            break;
        case 255:
            SetVBlankCallback(NULL);
            DestroyTask(taskId);
            DestroyTask(sCableCar->bgTaskId);
            SetMainCallback2(CB2_EndCableCar);
            break;
    }
}

static void Task_AnimateBgGoingUp(u8 taskId)
{
    if (sCableCar->state != 255)
    {
        sCableCar->bg3HorizontalOffset--;
        if ((sCableCar->timer % 2) == 0)
        {
            sCableCar->bg3VerticalOffset--;
        }
        if ((sCableCar->timer % 8) == 0)
        {
            sCableCar->bg1HorizontalOffset--;
            sCableCar->bg1VerticalOffset--;
        }
        switch (sCableCar->bg3HorizontalOffset)
        {
            case 175:
                CableCarUtil_FillWrapped(sCableCar->unk_08fc, 0, 0, 22, 2, 10);
                break;
            case 40:
                CableCarUtil_FillWrapped(sCableCar->unk_08fc, 0, 3, 0, 2, 2);
                break;
            case 32:
                CableCarUtil_FillWrapped(sCableCar->unk_08fc, 0, 2, 0, 1, 2);
                break;
            case 16:
                CableCarUtil_CopyWrapped(sCableCar->unk_08fc, sPylonTop_Tilemap, 0, 0, 5, 2);
                CableCarUtil_CopyWrapped(sCableCar->unk_08fc, eCableCar2->pylonPoleTilemap, 0, 2, 2, 30);
                sCableCar->bg3VerticalOffset = 64;
                break;
        }
    }
    AnimateGroundGoingUp();
    gSpriteCoordOffsetX = (gSpriteCoordOffsetX + 1) % 128;
}

static void Task_AnimateBgGoingDown(u8 taskId)
{
    if (sCableCar->state != 255)
    {
        sCableCar->bg3HorizontalOffset++;
        if ((sCableCar->timer % 2) == 0)
        {
            sCableCar->bg3VerticalOffset++;
        }
        if ((sCableCar->timer % 8) == 0)
        {
            sCableCar->bg1HorizontalOffset++;
            sCableCar->bg1VerticalOffset++;
        }
        switch (sCableCar->bg3HorizontalOffset)
        {
            case 176:
                CableCarUtil_CopyWrapped(sCableCar->unk_08fc, eCableCar2->pylonPoleTilemap, 0, 2, 2, 30);
                break;
            case 16:
                CableCarUtil_FillWrapped(sCableCar->unk_08fc, 0, 2, 0, 3, 2);
                CableCarUtil_FillWrapped(sCableCar->unk_08fc, 0, 0, 22, 2, 10);
                sCableCar->bg3VerticalOffset = 192;
                break;
            case 32:
                sCableCar->unk_08fc[0][2] = (sPylonTop_Tilemap + 2)[0];
                sCableCar->unk_08fc[0][3] = (sPylonTop_Tilemap + 2)[1];
                sCableCar->unk_08fc[1][2] = (sPylonTop_Tilemap + 2)[5];
                sCableCar->unk_08fc[1][3] = (sPylonTop_Tilemap + 2)[6];
                break;
            case 40:
                sCableCar->unk_08fc[0][4] = (sPylonTop_Tilemap + 4)[0];
                sCableCar->unk_08fc[1][4] = (sPylonTop_Tilemap + 4)[5];
                break;
        }
    }
    AnimateGroundGoingDown();
    if (sCableCar->timer < sCableCar->weatherDelay) {
        gSpriteCoordOffsetX = (gSpriteCoordOffsetX + 247) % 248;
    }
    else
    {
        gWeatherPtr->ashBaseSpritesX = (gWeatherPtr->ashBaseSpritesX + 247) % 248;
    }
}

static void VBlankCB_CableCar(void)
{
    DmaCopy16(3, sCableCar->unk_00fc, BG_SCREEN_ADDR(28), 0x800);
    DmaCopy16(3, sCableCar->unk_08fc, BG_SCREEN_ADDR(31), 0x800);
    REG_BG3HOFS = sCableCar->bg3HorizontalOffset;
    REG_BG3VOFS = sCableCar->bg3VerticalOffset;
    REG_BG1HOFS = sCableCar->bg1HorizontalOffset;
    REG_BG1VOFS = sCableCar->bg1VerticalOffset;
    REG_BG0HOFS = sCableCar->bg0HorizontalOffset;
    REG_BG0VOFS = sCableCar->bg0VerticalOffset;
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void SpriteCB_Cable(struct Sprite *sprite)
{

}

static void SpriteCB_CableCar(struct Sprite *sprite)
{
    if (sCableCar->state != 255)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sprite->x = sprite->data[0] - (u8)(0.14f * S16TOPOSFLOAT(sCableCar->timer));
            sprite->y = sprite->data[1] - (u8)(0.067f * S16TOPOSFLOAT(sCableCar->timer));
        }
        else
        {
            sprite->x = sprite->data[0] + (u8)(0.14f * S16TOPOSFLOAT(sCableCar->timer));
            sprite->y = sprite->data[1] + (u8)(0.067f * S16TOPOSFLOAT(sCableCar->timer));
        }
    }
}

static void SpriteCB_Player(struct Sprite *sprite)
{
    if (sCableCar->state != 255)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sprite->x = sprite->data[0] - (u8)(0.14f * S16TOPOSFLOAT(sCableCar->timer));
            sprite->y = sprite->data[1] - (u8)(0.067f * S16TOPOSFLOAT(sCableCar->timer));
        }
        else
        {
            sprite->x = sprite->data[0] + (u8)(0.14f * S16TOPOSFLOAT(sCableCar->timer));
            sprite->y = sprite->data[1] + (u8)(0.067f * S16TOPOSFLOAT(sCableCar->timer));
        }
        switch (sprite->data[2])
        {
            case 0:
                sprite->y2 = 17;
                if (sprite->data[3] ++ > 9)
                {
                    sprite->data[3] = 0;
                    sprite->data[2] ++;
                }
                break;
            default:
                sprite->y2 = 16;
                if (sprite->data[3] ++ > 9)
                {
                    sprite->data[3] = 0;
                    sprite->data[2] = 0;
                }
                break;
        }
    }
}

static void SpriteCB_HikerGoingUp(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->x += 2 * sprite->centerToCornerVecX;
        sprite->y += 16 + sprite->centerToCornerVecY;
    }
    if (++sprite->data[0] >= sprite->data[2])
    {
        switch (sprite->data[1])
        {
            case 0:
                sprite->x++;
                if ((sprite->data[0] % 4) == 0)
                {
                    sprite->y++;
                }
                break;
            case 1:
                if ((sprite->data[0] % 2) != 0)
                {
                    sprite->x++;
                    if ((sprite->x % 4) == 0)
                    {
                        sprite->y++;
                    }
                }
                break;
        }
        if (sprite->y > 0xa0)
        {
            DestroySprite(sprite);
        }
    }
}

static void SpriteCB_HikerGoingDown(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->y += 16 + sprite->centerToCornerVecY;
    }
    if (++sprite->data[0] >= sprite->data[2])
    {
        switch (sprite->data[1])
        {
            case 0:
                sprite->x--;
                if ((sprite->data[0] % 4) == 0)
                {
                    sprite->y--;
                }
                break;
            case 1:
                if ((sprite->data[0] % 2) != 0)
                {
                    sprite->x--;
                    if ((sprite->x % 4) == 0)
                    {
                        sprite->y--;
                    }
                }
                break;
        }
        if (sprite->y < 0x50)
        {
            DestroySprite(sprite);
        }
    }
}

static void SetBgRegs(bool8 active)
{
    switch (active)
    {
        case FALSE:
        default:
            REG_WININ = 0;
            REG_WINOUT = 0;
            REG_WIN0H = 0;
            REG_WIN1H = 0;
            REG_WIN0V = 0;
            REG_WIN1V = 0;
            REG_DISPCNT = 0;
            REG_BG3CNT = 0;
            REG_BG2CNT = 0;
            REG_BG1CNT = 0;
            REG_BG0CNT = 0;
            REG_BG3HOFS = 0;
            REG_BG3VOFS = 0;
            REG_BG2HOFS = 0;
            REG_BG2VOFS = 0;
            REG_BG1HOFS = 0;
            REG_BG1VOFS = 0;
            REG_BG0HOFS = 0;
            REG_BG0VOFS = 0;
            REG_BLDCNT = 0;
            break;
        case TRUE:
            REG_WININ = 0;
            REG_WINOUT = 0;
            REG_WIN0H = 0;
            REG_WIN1H = 0;
            REG_WIN0V = 0;
            REG_WIN1V = 0;
            if (gSpecialVar_0x8004 == 0)
            {
                sCableCar->bg3HorizontalOffset = 0xb0;
                sCableCar->bg3VerticalOffset = 0x10;
                sCableCar->bg1HorizontalOffset = 0x00;
                sCableCar->bg1VerticalOffset = 0x50;
                sCableCar->bg0VerticalOffset = 0;
                sCableCar->bg0VerticalOffset = 0;
            }
            else
            {
                sCableCar->bg3HorizontalOffset = 0x60;
                sCableCar->bg3VerticalOffset = 0xe8;
                sCableCar->bg1HorizontalOffset = 0x00;
                sCableCar->bg1VerticalOffset = 0x04;
                sCableCar->bg0VerticalOffset = 0;
                sCableCar->bg0VerticalOffset = 0;
            }
            REG_BG3HOFS = sCableCar->bg3HorizontalOffset;
            REG_BG3VOFS = sCableCar->bg3VerticalOffset;
            REG_BG2HOFS = 0;
            REG_BG2VOFS = 0;
            REG_BG1HOFS = sCableCar->bg1HorizontalOffset;
            REG_BG1VOFS = sCableCar->bg1VerticalOffset;
            REG_BG0HOFS = sCableCar->bg0HorizontalOffset;
            REG_BG0VOFS = sCableCar->bg0VerticalOffset;
            REG_BG0CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(28) | BGCNT_WRAP;
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(29) | BGCNT_WRAP;
            REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_SCREENBASE(30) | BGCNT_WRAP;
            REG_BG3CNT = BGCNT_PRIORITY(0) | BGCNT_SCREENBASE(31) | BGCNT_WRAP;
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
            REG_BLDCNT = 0x3f00;
            break;
    }
}

static void CreateCableCarSprites(void)
{
    u8 spriteId;
    u8 i;

    u8 playerGraphicsIds[2] = {
        OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL,
        OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
    };
    u16 rval = Random();
    u8 hikerGraphicsIds[4] = {
        OBJ_EVENT_GFX_HIKER,
        OBJ_EVENT_GFX_CAMPER,
        OBJ_EVENT_GFX_PICNICKER,
        OBJ_EVENT_GFX_POOCHYENA
    };
    s16 hikerCoords[2][2] = {
        {   0,  80 },
        { 240, 146 }
    };
    u8 hikerMovementDelayTable[4] = {
        0,
        60,
        120,
        170
    };
    void (*hikerCallbacks[2])(struct Sprite *) = {
        SpriteCB_HikerGoingUp,
        SpriteCB_HikerGoingDown
    };

    switch (gSpecialVar_0x8004)
    {
        case 0:
        default:
            spriteId = CreateObjectGraphicsSprite(playerGraphicsIds[gSaveBlock2.playerGender], SpriteCB_Player, 0xc8, 0x49, 0x66);
            if (spriteId != MAX_SPRITES)
            {
                gSprites[spriteId].oam.priority = 2;
                gSprites[spriteId].x2 = 0x08;
                gSprites[spriteId].y2 = 0x10;
                gSprites[spriteId].data[0] = 0xc8;
                gSprites[spriteId].data[1] = 0x49;
            }
            spriteId = CreateSprite(&gSpriteTemplate_8401D40[0], 0xb0, 0x2b, 0x67);
            gSprites[spriteId].x2 = gSprites[spriteId].y2 = 0x20;
            gSprites[spriteId].data[0] = 0xb0;
            gSprites[spriteId].data[1] = 0x2b;
            spriteId = CreateSprite(&gSpriteTemplate_8401D40[1], 0xc8, 0x63, 0x65);
            gSprites[spriteId].x2 = 8;
            gSprites[spriteId].y2 = 4;
            gSprites[spriteId].data[0] = 0xc8;
            gSprites[spriteId].data[1] = 0x63;
            sCableCar->weather = 7;
            sCableCar->weatherDelay = 0x15e;
            SetCurrentAndNextWeatherNoDelay(WEATHER_SUNNY);
            break;
        case 1:
            CableCarUtil_CopyWrapped(sCableCar->unk_00fc, eCableCar2->groundTilemap + 0x24, 0x18, 0x1a, 0x0c, 0x03);
            spriteId = CreateObjectGraphicsSprite(playerGraphicsIds[gSaveBlock2.playerGender], SpriteCB_Player, 0x80, 0x27, 0x66);
            if (spriteId != MAX_SPRITES)
            {
                gSprites[spriteId].oam.priority = 2;
                gSprites[spriteId].x2 = 0x08;
                gSprites[spriteId].y2 = 0x10;
                gSprites[spriteId].data[0] = 0x80;
                gSprites[spriteId].data[1] = 0x27;
            }
            spriteId = CreateSprite(&gSpriteTemplate_8401D40[0], 0x68, 0x09, 0x67);
            gSprites[spriteId].x2 = gSprites[spriteId].y2 = 0x20;
            gSprites[spriteId].data[0] = 0x68;
            gSprites[spriteId].data[1] = 0x09;
            spriteId = CreateSprite(&gSpriteTemplate_8401D40[1], 0x80, 0x41, 0x65);
            gSprites[spriteId].x2 = 8;
            gSprites[spriteId].y2 = 4;
            gSprites[spriteId].data[0] = 0x80;
            gSprites[spriteId].data[1] = 0x41;
            sCableCar->weather = 2;
            sCableCar->weatherDelay = 0x109;
            SetCurrentAndNextWeatherNoDelay(WEATHER_VOLCANIC_ASH);
            break;
    }
    for (i = 0; i < 9; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_8401D40[2], 16 * i + 0x60, 8 * i - 8, 0x68);
        gSprites[spriteId].x2 = 8;
        gSprites[spriteId].y2 = 8;
    }
    if ((rval % 64) == 0)
    {
        spriteId = CreateObjectGraphicsSprite(hikerGraphicsIds[rval % 3], hikerCallbacks[gSpecialVar_0x8004], hikerCoords[gSpecialVar_0x8004][0], hikerCoords[gSpecialVar_0x8004][1], 0x6a);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].oam.priority = 2;
            gSprites[spriteId].x2 = -gSprites[spriteId].centerToCornerVecX;
            gSprites[spriteId].y2 = -gSprites[spriteId].centerToCornerVecY;
            if (gSpecialVar_0x8004 == 0)
            {
                if (rval % 2)
                {
                    StartSpriteAnim(&gSprites[spriteId], 6);
                    gSprites[spriteId].data[1] = 1;
                    gSprites[spriteId].y += 2;
                }
                else
                {
                    StartSpriteAnim(&gSprites[spriteId], 7);
                    gSprites[spriteId].data[1] = 0;
                }
            }
            else
            {
                if (rval % 2)
                {
                    StartSpriteAnim(&gSprites[spriteId], 7);
                    gSprites[spriteId].data[1] = 1;
                    gSprites[spriteId].y += 2;
                }
                else
                {
                    StartSpriteAnim(&gSprites[spriteId], 6);
                    gSprites[spriteId].data[1] = 0;
                }
            }
            gSprites[spriteId].data[2] = hikerMovementDelayTable[rval % 4];
        }
    }
}

static void BufferNextGroundSegment(void)
{
    u8 i;
    u8 j;
    u8 k;
    u8 offset;

    for (i = 0, k = 0, offset = 0x24 * (sCableCar->groundTilemapOffset + 2); i < 3; i++)
    {
        for (j = 0; j < 12; j++)
        {
            sCableCar->groundTileBuffer[i][j] = *(eCableCar2->groundTilemap + (offset++));
            sCableCar->groundTileBuffer[i + 3][j] = eCableCar2->groundTilemap[k];
            sCableCar->groundTileBuffer[i + 6][j] = (eCableCar2->groundTilemap + 0x24)[k];
            k++;
        }
    }
    sCableCar->groundTilemapOffset = (sCableCar->groundTilemapOffset + 1) % 3;
}

static void AnimateGroundGoingUp(void)
{
    sCableCar->groundTimer = (sCableCar->groundTimer + 1) % 0x60;
    sCableCar->bg0HorizontalOffset = sCableCar->groundXBase - sCableCar->groundXOffset;
    sCableCar->bg0VerticalOffset = sCableCar->groundYBase - sCableCar->groundYOffset;
    sCableCar->groundXOffset++;
    if ((sCableCar->groundXOffset % 4) == 0)
    {
        sCableCar->groundYOffset++;
    }
    if (sCableCar->groundXOffset > 16)
    {
        DrawNextGroundSegmentGoingUp();
    }
}

static void AnimateGroundGoingDown(void)
{
    sCableCar->groundTimer = (sCableCar->groundTimer + 1) % 0x60;
    sCableCar->bg0HorizontalOffset = sCableCar->groundXBase + sCableCar->groundXOffset;
    sCableCar->bg0VerticalOffset = sCableCar->groundYBase + sCableCar->groundYOffset;
    sCableCar->groundXOffset++;
    if ((sCableCar->groundXOffset % 4) == 0)
    {
        sCableCar->groundYOffset++;
    }
    if (sCableCar->groundXOffset > 16)
    {
        DrawNextGroundSegmentGoingDown();
    }
}

static void DrawNextGroundSegmentGoingUp(void)
{
    u8 i = 0;

    sCableCar->groundXOffset = sCableCar->groundYOffset = 0;
    sCableCar->groundXBase = sCableCar->bg0HorizontalOffset;
    sCableCar->groundYBase = sCableCar->bg0VerticalOffset;
    sCableCar->groundSegmentXStart = (sCableCar->groundSegmentXStart + 30) % 32;
    sCableCar->groundTileIdx -= 2;
    sGroundSegmentY_Up = (sCableCar->groundSegmentYStart + 23) % 32;
    for (i = 0; i < 9; i++)
    {
        sGroundX_Up = sCableCar->groundSegmentXStart;
        sGroundY_Up = (sGroundSegmentY_Up + i) % 32;
        sCableCar->unk_00fc[sGroundY_Up][sGroundX_Up] = sCableCar->groundTileBuffer[i][sCableCar->groundTileIdx];
        sGroundX_Up = (sGroundX_Up + 1) % 32;
        sCableCar->unk_00fc[sGroundY_Up][sGroundX_Up] = sCableCar->groundTileBuffer[i][sCableCar->groundTileIdx + 1];
    }
    sGroundX_Up = (sCableCar->groundSegmentXStart + 30) % 32;
    CableCarUtil_FillWrapped(sCableCar->unk_00fc, 0, sGroundX_Up, 0, 2, 32);
    if (sCableCar->groundTileIdx == 0)
    {
        sCableCar->groundSegmentYStart = (sCableCar->groundSegmentYStart + 29) % 32;
        sCableCar->groundTileIdx = 12;
        BufferNextGroundSegment();
        sGroundX_Up = (sCableCar->groundSegmentYStart + 1) % 32;
        CableCarUtil_FillWrapped(sCableCar->unk_00fc, 0, 0, sGroundX_Up, 32, 9);
    }
}

static void DrawNextGroundSegmentGoingDown(void)
{
    u8 i = 0;

    sCableCar->groundXOffset = sCableCar->groundYOffset = 0;
    sCableCar->groundXBase = sCableCar->bg0HorizontalOffset;
    sCableCar->groundYBase = sCableCar->bg0VerticalOffset;
    sCableCar->groundSegmentXStart = (sCableCar->groundSegmentXStart + 2) % 32;
    sCableCar->groundTileIdx += 2;
    sGroundSegmentY_Down = sCableCar->groundSegmentYStart;
    for (i = 0; i < 9; i++)
    {
        sGroundX_Down = sCableCar->groundSegmentXStart;
        sGroundY_Down = (sGroundSegmentY_Down + i) % 32;
        sCableCar->unk_00fc[sGroundY_Down][sGroundX_Down] = sCableCar->groundTileBuffer[i][sCableCar->groundTileIdx];
        sGroundX_Down = (sGroundX_Down + 1) % 32;
        sCableCar->unk_00fc[sGroundY_Down][sGroundX_Down] = sCableCar->groundTileBuffer[i][sCableCar->groundTileIdx + 1];
    }
    sGroundY_Down = (sCableCar->groundSegmentYStart + 23) % 32;
    CableCarUtil_FillWrapped(sCableCar->unk_00fc, 0, sCableCar->groundSegmentXStart, sGroundY_Down, 2, 9);
    if (sCableCar->groundTileIdx == 10)
    {
        sCableCar->groundSegmentYStart = (sCableCar->groundSegmentYStart + 3) % 32;
        sCableCar->groundTileIdx = 0xfe;
        BufferNextGroundSegment();
    }
}

static void InitGroundTilemapData(bool8 goingDown)
{
    switch (goingDown)
    {
        case 0:
        default:
            sCableCar->groundTilemapOffset = 2;
            sCableCar->groundSegmentXStart = 0;
            sCableCar->groundSegmentYStart = 20;
            sCableCar->groundTileIdx = 12;
            BufferNextGroundSegment();
            DrawNextGroundSegmentGoingUp();
            break;
        case 1:
            sCableCar->groundTilemapOffset = 2;
            sCableCar->groundSegmentXStart = 28;
            sCableCar->groundSegmentYStart = 20;
            sCableCar->groundTileIdx = 4;
            BufferNextGroundSegment();
            DrawNextGroundSegmentGoingDown();
            break;
    }
    sCableCar->groundTimer = 0;
}
