#include "global.h"
#include "berry_tag_screen.h"
#include "berry.h"
#include "decompress.h"
#include "event_object_movement.h"
#include "item_menu.h"
#include "item.h"
#include "constants/items.h"
#include "item_use.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "overworld.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"

// CreateBerryTagSprite takes an 8-bit zero-based berry index; the item ID wraps at 256.
#define BERRY_SPRITE_INDEX_OFFSET (0x100 - FIRST_BERRY_INDEX)
#define BERRIES_POCKET_INDEX (POCKET_BERRIES - 1)

struct BerryTagSharedMem
{
    /*0x00*/ u8 unused[0x1FFFF];
    /*0x1FFFF*/ u8 gfxState;
};

extern struct BerryTagSharedMem gSharedMem;
extern u16 gBattle_BG1_Y;

static EWRAM_DATA u8 sBerrySpriteId = 0;
static EWRAM_DATA s16 sFlavorCircleSpriteIds[5] = {0};

extern const struct CompressedSpriteSheet gBerryCheckCircleSpriteSheet;
extern const struct CompressedSpritePalette gBerryCheckCirclePaletteTable;

extern u8 gBerryCheck_Gfx[];
extern u8 gBerryCheck_Pal[];
extern u8 gBerryTag_Gfx[];
extern u8 gBerryTag_Tilemap[];

static const u8 *const sBerryFirmnessStrings[] =
{
    gBerryFirmnessString_VerySoft,
    gBerryFirmnessString_Soft,
    gBerryFirmnessString_Hard,
    gBerryFirmnessString_VeryHard,
    gBerryFirmnessString_SuperHard,
};

static void CB2_BerryTagScreen(void);
static void VBlankCB_BerryTagScreen(void);
static bool8 InitBerryTagScreen(void);
static void HandleInitBackgrounds(void);
static bool8 LoadBerryTagGfx(void);
static void Task_CloseBerryTagScreen(u8 taskId);
static void PrepareToCloseBerryTagScreen(u8 taskId);
static void Task_HandleInput(u8 taskId);
static void PrintAllBerryData(void);
static void CreateFlavorCircleSprites(u8 berry);
static void DestroyFlavorCircleSprites(void);
static void TryChangeDisplayedBerry(u8 taskId, s8 direction);
static void Task_DisplayAnotherBerry(u8 taskId);
static void HandleBagCursorPositionChange(s8 toMove);
static void PrintBerryDataAndCreateSprites(void);

static void CB2_BerryTagScreen(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
}

static void VBlankCB_BerryTagScreen(void)
{
    REG_BG0VOFS = gBattle_BG1_Y;
    REG_BG1VOFS = gBattle_BG1_Y;

    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static bool8 InitBerryTagScreen(void)
{
    u8 berry;
    u16 backup;

    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ResetVramOamAndBgCntRegs();
        HandleInitBackgrounds();
        REG_BLDCNT = 0;
        gMain.state += 1;
        break;
    case 1:
        ResetPaletteFade();
        gPaletteFade.bufferTransferDisabled = 1;
        gMain.state += 1;
        break;
    case 2:
        ResetSpriteData();
        gMain.state += 1;
        break;
    case 3:
        Text_LoadWindowTemplate(&gBerryTagWindowTemplate);
        gMain.state += 1;
        break;
    case 4:
        MultistepInitMenuWindowBegin(&gBerryTagWindowTemplate);
        gMain.state += 1;
        break;
    case 5:
        if (!MultistepInitMenuWindowContinue())
            break;
        gSharedMem.gfxState = 0;
        gMain.state += 1;
        break;
    case 6:
        if (!LoadBerryTagGfx())
            break;
        gSharedMem.gfxState = 0;
        gMain.state += 1;
        break;
    case 7:
        PrintAllBerryData();
        gMain.state += 1;
        break;
    case 8:
        berry = gSpecialVar_ItemId + BERRY_SPRITE_INDEX_OFFSET;
        sBerrySpriteId = CreateBerryTagSprite(berry, 56, 64);
        gMain.state += 1;
        break;
    case 9:
        CreateFlavorCircleSprites(gSpecialVar_ItemId + BERRY_SPRITE_INDEX_OFFSET);
        gMain.state += 1;
        break;
    case 10:
        backup = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = backup;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
        SetVBlankCallback(VBlankCB_BerryTagScreen);
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_ON | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_1D_MAP;
        gMain.state += 1;
        break;
    case 11:
        if (Overworld_IsRecvQueueAtMax() == TRUE)
            break;
        gMain.state += 1;
        break;
    case 12:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gPaletteFade.bufferTransferDisabled = 0;
        SetMainCallback2(CB2_BerryTagScreen);
        return TRUE;
    }

    return FALSE;
}

void DoBerryTagScreen(u8 taskId)
{
    do
    {
        if (InitBerryTagScreen() == TRUE)
        {
            CreateTask(Task_HandleInput, 0);
            return;
        }
    } while (MenuHelpers_IsLinkActive() != TRUE);
}

static void HandleInitBackgrounds(void)
{
    REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(5) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG2CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(6) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(7) | BGCNT_16COLOR | BGCNT_TXT256x256;
    gBattle_BG1_Y = 0;
}

bool8 LoadBerryTagGfx(void)
{
    u16 i;

    switch (gSharedMem.gfxState)
    {
    case 0:
        LZDecompressVram(gBerryCheck_Gfx, (void *)VRAM);
        gSharedMem.gfxState += 1;
        break;
    case 1:
        LZDecompressVram(gBerryTag_Gfx, (void *)VRAM + 0x2800);
        gSharedMem.gfxState += 1;
        break;
    case 2:
        LZDecompressVram(gBerryTag_Tilemap, (void *)VRAM + 0x3000);
        gSharedMem.gfxState += 1;
        break;
    case 3:
        for (i = 0; i < 0x400; i++)
        {
            if (gSaveBlock2.playerGender == MALE)
                gBGTilemapBuffers[2][i] = 0x4042;
            else
                gBGTilemapBuffers[2][i] = 0x5042;
        }
        DmaCopy16Defvars(3, gBGTilemapBuffers[2], (void *)(VRAM + 0x3800), 0x800);
        gSharedMem.gfxState += 1;
        break;
    case 4:
        LoadCompressedPalette(gBerryCheck_Pal, 0, 96 * 2);
        gSharedMem.gfxState += 1;
        break;
    case 5:
        LoadCompressedSpriteSheet(&gBerryCheckCircleSpriteSheet);
        gSharedMem.gfxState += 1;
        break;
    case 6:
        LoadCompressedSpritePalette(&gBerryCheckCirclePaletteTable);
        gSharedMem.gfxState = 0;
        return TRUE;
    }

    return FALSE;
}

static void Task_CloseBerryTagScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_ReturnToBagMenu);
        FreeBerryTagSpritePalettes();
        FreeAndReserveObjectSpritePalettes();
        DestroyTask(taskId);
    }
}

static void PrepareToCloseBerryTagScreen(u8 taskId)
{
    PlaySE(SE_SELECT);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_CloseBerryTagScreen;
}

static void Task_HandleInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_ANY) == DPAD_UP)
            TryChangeDisplayedBerry(taskId, -1);
        if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
            TryChangeDisplayedBerry(taskId, 1);
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
            PrepareToCloseBerryTagScreen(taskId);
    }
}

static void PrintAllBerryData(void)
{
    const struct Berry *berryInfo;
#ifdef UNITS_IMPERIAL
    u32 size;
    s32 sizeMajor;
    s32 sizeMinor;
#endif
#if GERMAN
    u8 buffer[16];
#endif

    berryInfo = GetBerryInfo(gSpecialVar_ItemId + BERRY_SPRITE_INDEX_OFFSET + 1);

    ConvertIntToDecimalStringN(gStringVar1, gSpecialVar_ItemId - FIRST_BERRY_INDEX + 1, STR_CONV_MODE_LEADING_ZEROS, 2);
    Menu_PrintText(gStringVar1, 12, 4);

#if ENGLISH
    Menu_PrintText(berryInfo->name, 14, 4);
#elif GERMAN
    StringCopy(buffer, berryInfo->name);
    StringAppend(buffer, gText_Berry2);
    Menu_PrintText(buffer, 14, 4);
#endif

    Menu_PrintText(berryInfo->description1, 4, 14);
    Menu_PrintText(berryInfo->description2, 4, 16);

#ifdef UNITS_IMPERIAL
    size = (berryInfo->size * 1000) / 254;
    if (size % 10 >= 5)
        size += 10;
    sizeMinor = (size % 100) / 10;
    sizeMajor = size / 100;
#endif

    Menu_PrintText(gText_SizeSlash, 11, 7);
    if (berryInfo->size != 0)
    {
#ifdef UNITS_IMPERIAL
        ConvertIntToDecimalStringN(gStringVar1, sizeMajor, STR_CONV_MODE_LEFT_ALIGN, 2);
        ConvertIntToDecimalStringN(gStringVar2, sizeMinor, STR_CONV_MODE_LEFT_ALIGN, 2);
#else
        ConvertIntToDecimalStringN(gStringVar1, berryInfo->size / 10, STR_CONV_MODE_LEFT_ALIGN, 2);
        ConvertIntToDecimalStringN(gStringVar2, berryInfo->size % 10, STR_CONV_MODE_LEFT_ALIGN, 2);
#endif
        Menu_PrintText(gText_Var1DotVar2, 16, 7);
    }
    else
    {
        Menu_PrintText(gText_ThreeMarks, 16, 7);
    }

    Menu_PrintText(gText_FirmSlash, 11, 9);
    if (berryInfo->firmness != 0)
        Menu_PrintText(sBerryFirmnessStrings[berryInfo->firmness - 1], 16, 9);
    else
        Menu_PrintText(gText_ThreeMarks, 16, 9);
}

static void CreateFlavorCircleSprites(u8 berry)
{
    const struct Berry *berryInfo;
    u16 i;

    berryInfo = GetBerryInfo(berry + 1);
    for (i = 0; i < 5; i++)
        sFlavorCircleSpriteIds[i] = (u16)sFlavorCircleSpriteIds[i] | 0xFFFF;

    // argument is the center of the circle
    if (berryInfo->spicy)
        sFlavorCircleSpriteIds[0] = CreateBerryFlavorCircleSprite(48);
    if (berryInfo->dry)
        sFlavorCircleSpriteIds[1] = CreateBerryFlavorCircleSprite(88);
    if (berryInfo->sweet)
        sFlavorCircleSpriteIds[2] = CreateBerryFlavorCircleSprite(128);
    if (berryInfo->bitter)
        sFlavorCircleSpriteIds[3] = CreateBerryFlavorCircleSprite(168);
    if (berryInfo->sour)
        sFlavorCircleSpriteIds[4] = CreateBerryFlavorCircleSprite(208);
}

static void DestroyFlavorCircleSprites(void)
{
    u16 i;

    for (i = 0; i < 5; i++)
    {
        if (sFlavorCircleSpriteIds[i] != -1)
        {
            DestroySprite(&gSprites[sFlavorCircleSpriteIds[i]]);
            sFlavorCircleSpriteIds[i] = -1;
        }
    }
}

#define tScrollSpeed data[0]
#define tCursorDelta data[1]

static void TryChangeDisplayedBerry(u8 taskId, s8 direction)
{
    u8 berryPocket = BERRIES_POCKET_INDEX;
    s16 *data = gTasks[taskId].data;

    if (gBagPocketScrollStates[berryPocket].scrollTop + gBagPocketScrollStates[berryPocket].cursorPos == 0
     && direction < 0)
        return;
    if (gBagPocketScrollStates[berryPocket].scrollTop + gBagPocketScrollStates[berryPocket].cursorPos + 1 == gBagPocketScrollStates[berryPocket].numSlots
     && direction > 0)
        return;

    PlaySE(SE_SELECT);
    if (gBagPocketScrollStates[berryPocket].scrollTop + gBagPocketScrollStates[berryPocket].cursorPos + direction < 0)
        tCursorDelta = -(gBagPocketScrollStates[berryPocket].scrollTop + gBagPocketScrollStates[berryPocket].cursorPos);
    else if (gBagPocketScrollStates[berryPocket].scrollTop + gBagPocketScrollStates[berryPocket].cursorPos + direction >= gBagPocketScrollStates[berryPocket].numSlots)
        tCursorDelta = gBagPocketScrollStates[berryPocket].numSlots - gBagPocketScrollStates[berryPocket].scrollTop - gBagPocketScrollStates[berryPocket].cursorPos - 1;
    else
        tCursorDelta = direction;

    gTasks[taskId].func = Task_DisplayAnotherBerry;

    if (direction < 0)
        tScrollSpeed = -16;
    else
        tScrollSpeed = 16;

}

static void Task_DisplayAnotherBerry(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    gBattle_BG1_Y = (gBattle_BG1_Y + tScrollSpeed) & 0xFF;
    if ((tScrollSpeed > 0 && gBattle_BG1_Y == 144)
     || (tScrollSpeed < 0 && gBattle_BG1_Y == 112))
    {
        HandleBagCursorPositionChange(gTasks[taskId].tCursorDelta);
        PrintBerryDataAndCreateSprites();
    }
    if (gBattle_BG1_Y == 0)
    {
        gTasks[taskId].tScrollSpeed = gBattle_BG1_Y;
        gTasks[taskId].tCursorDelta = gBattle_BG1_Y;
        gTasks[taskId].func = Task_HandleInput;
    }
}

static void HandleBagCursorPositionChange(s8 toMove)
{
    u8 berryPocket = BERRIES_POCKET_INDEX;

    if (toMove > 0)
    {
        if (gBagPocketScrollStates[berryPocket].cursorPos + toMove > 7)
        {
            gBagPocketScrollStates[berryPocket].scrollTop += gBagPocketScrollStates[berryPocket].cursorPos - 7 + toMove;
            gBagPocketScrollStates[berryPocket].cursorPos = 7;
        }
        else
        {
            gBagPocketScrollStates[berryPocket].cursorPos += toMove;
        }
    }
    else
    {
        if (gBagPocketScrollStates[berryPocket].cursorPos + toMove < 0)
        {
            gBagPocketScrollStates[berryPocket].scrollTop += gBagPocketScrollStates[berryPocket].cursorPos + toMove;
            gBagPocketScrollStates[berryPocket].cursorPos = 0;
        }
        else
        {
            gBagPocketScrollStates[berryPocket].cursorPos += toMove;
        }
    }
    gSpecialVar_ItemId = gCurrentBagPocketItemSlots[gBagPocketScrollStates[berryPocket].scrollTop + gBagPocketScrollStates[berryPocket].cursorPos].itemId;
    DestroySprite(&gSprites[sBerrySpriteId]);
    DestroyFlavorCircleSprites();
    FreeBerryTagSpritePalettes();
}

static void PrintBerryDataAndCreateSprites(void)
{
    Menu_EraseWindowRect(0, 4, 29, 19);
    PrintAllBerryData();

    // center of berry sprite
    sBerrySpriteId = CreateBerryTagSprite(gSpecialVar_ItemId + BERRY_SPRITE_INDEX_OFFSET, 56, 64);

    CreateFlavorCircleSprites(gSpecialVar_ItemId + BERRY_SPRITE_INDEX_OFFSET);
}
