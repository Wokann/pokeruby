#include "global.h"
#include "graphics.h"
#include "main.h"
#include "menu_cursor.h"
#include "text_window.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "mon_markings.h"

#ifdef GERMAN
#define MENU_TEXT_SPRITE_X_OFFSET 24
#else
#define MENU_TEXT_SPRITE_X_OFFSET 32
#endif

static const u16 sMonMarkings_Pal[] = INCBIN_U16("graphics/interface/mon_markings.gbapal");
static const u8 sMonMarkings_Gfx[] = INCBIN_U8("graphics/interface/mon_markings.4bpp");

static const struct OamData sOamData_MenuWindow =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct OamData sOamData_8x8 =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_Marking_CircleOff[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Marking_CircleOn[] =
{
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Marking_SquareOff[] =
{
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Marking_SquareOn[] =
{
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Marking_TriangleOff[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Marking_TriangleOn[] =
{
    ANIMCMD_FRAME(5, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Marking_HeartOff[] =
{
    ANIMCMD_FRAME(6, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Marking_HeartOn[] =
{
    ANIMCMD_FRAME(7, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Cursor[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_OKCancelText[] =
{
    ANIMCMD_FRAME(9, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_MenuSprite[] =
{
    sAnim_Marking_CircleOff,
    sAnim_Marking_CircleOn,
    sAnim_Marking_SquareOff,
    sAnim_Marking_SquareOn,
    sAnim_Marking_TriangleOff,
    sAnim_Marking_TriangleOn,
    sAnim_Marking_HeartOff,
    sAnim_Marking_HeartOn,
    sAnim_Cursor,
    sAnim_OKCancelText,
};

static const union AnimCmd sAnim_MenuWindow_UpperHalf[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MenuWindow_LowerHalf[] =
{
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_MenuWindow[] =
{
    sAnim_MenuWindow_UpperHalf,
    sAnim_MenuWindow_LowerHalf,
};

static const struct OamData sOamData_MarkingCombo =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_MarkingCombo_AllOff[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_Circle[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_Square[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_CircleSquare[] =
{
    ANIMCMD_FRAME(12, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_Triangle[] =
{
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_CircleTriangle[] =
{
    ANIMCMD_FRAME(20, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_SquareTriangle[] =
{
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_CircleSquareTriangle[] =
{
    ANIMCMD_FRAME(28, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_Heart[] =
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_CircleHeart[] =
{
    ANIMCMD_FRAME(36, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_SquareHeart[] =
{
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_CircleSquareHeart[] =
{
    ANIMCMD_FRAME(44, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_TriangleHeart[] =
{
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_CircleTriangleHeart[] =
{
    ANIMCMD_FRAME(52, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_SquareTriangleHeart[] =
{
    ANIMCMD_FRAME(56, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MarkingCombo_AllOn[] =
{
    ANIMCMD_FRAME(60, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_MarkingCombo[] =
{
    sAnim_MarkingCombo_AllOff,
    sAnim_MarkingCombo_Circle,
    sAnim_MarkingCombo_Square,
    sAnim_MarkingCombo_CircleSquare,
    sAnim_MarkingCombo_Triangle,
    sAnim_MarkingCombo_CircleTriangle,
    sAnim_MarkingCombo_SquareTriangle,
    sAnim_MarkingCombo_CircleSquareTriangle,
    sAnim_MarkingCombo_Heart,
    sAnim_MarkingCombo_CircleHeart,
    sAnim_MarkingCombo_SquareHeart,
    sAnim_MarkingCombo_CircleSquareHeart,
    sAnim_MarkingCombo_TriangleHeart,
    sAnim_MarkingCombo_CircleTriangleHeart,
    sAnim_MarkingCombo_SquareTriangleHeart,
    sAnim_MarkingCombo_AllOn,
};

static EWRAM_DATA struct MonMarkingsMenu *sMenu = NULL;

static void CreateMonMarkingsMenuSprites(s16, s16, u16, u16);
static void SpriteCB_Dummy(struct Sprite *);
static void SpriteCB_Marking(struct Sprite *);
static void SpriteCB_Cursor(struct Sprite *);
static struct Sprite *CreateMarkingComboSprite(u16, u16, const u16 *, u16);

void InitMonMarkingsMenu(struct MonMarkingsMenu *ptr)
{
    sMenu = ptr;
    sMenu->spriteSheetLoadRequired = FALSE;
}

static void BufferMenuWindowTiles(void)
{
    const struct FrameGraphics *frame = GetWindowFrameTilesPal(gSaveBlock2.optionsWindowFrameType);
    sMenu->frameTiles = frame->tiles;
    sMenu->framePalette = frame->palette;
    sMenu->tileLoadState = 0;
    CpuFill16(0, sMenu->windowSpriteTiles, sizeof(sMenu->windowSpriteTiles));
}

static bool8 BufferMenuFrameTiles(void)
{
    u16 i;
    u8 *dest = sMenu->windowSpriteTiles + sMenu->tileLoadState * 0x100;

    switch (sMenu->tileLoadState)
    {
    case 0:
        CpuFastCopy(sMenu->frameTiles, dest, TILE_SIZE_4BPP);
        for (i = 0; i < 6; i++)
        {
            CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP, dest + TILE_SIZE_4BPP * (i + 1), TILE_SIZE_4BPP);
        }
        CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP * 2, dest + TILE_SIZE_4BPP * 7, TILE_SIZE_4BPP);
        sMenu->tileLoadState++;
        break;
    default:
        CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP * 3, dest, TILE_SIZE_4BPP);
        for (i = 0; i < 6; i++)
        {
            CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP * 4, dest + TILE_SIZE_4BPP * (i + 1), TILE_SIZE_4BPP);
        }
        CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP * 5, dest + TILE_SIZE_4BPP * 7, TILE_SIZE_4BPP);
        sMenu->tileLoadState++;
        break;
    case 13:
        CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP * 6, dest, TILE_SIZE_4BPP);
        for (i = 0; i < 6; i++)
        {
            CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP * 7, dest + TILE_SIZE_4BPP * (i + 1), TILE_SIZE_4BPP);
        }
        CpuFastCopy(sMenu->frameTiles + TILE_SIZE_4BPP * 8, dest + TILE_SIZE_4BPP * 7, TILE_SIZE_4BPP);
        sMenu->tileLoadState++;
        return FALSE;
    case 14:
        return FALSE;
    }

    return TRUE;
}

void BufferMonMarkingsMenuTiles(void)
{
    BufferMenuWindowTiles();
    while (BufferMenuFrameTiles())
        ;
}

void OpenMonMarkingsMenu(u8 markings, s16 x, s16 y)
{
    u16 i;
    sMenu->cursorPos = 0;
    sMenu->markings = markings;
    for (i = 0; i < 4; i++)
        sMenu->markingsArray[i] = (sMenu->markings >> i) & 1;
    DestroyMenuCursor();
    CreateMonMarkingsMenuSprites(x, y, sMenu->baseTileTag, sMenu->basePaletteTag);
}

void FreeMonMarkingsMenu(void)
{
    u16 i;

    for (i = 0; i < 3; i++)
    {
        FreeSpriteTilesByTag(sMenu->baseTileTag + i);
    }

    FreeSpritePaletteByTag(sMenu->basePaletteTag);
    FreeSpritePaletteByTag(sMenu->basePaletteTag + 1);

    for (i = 0; i < 2; i++)
    {
        if (!sMenu->windowSprites[i])
            return;
        DestroySprite(sMenu->windowSprites[i]);
    }

    for (i = 0; i < 4; i++)
    {
        if (!sMenu->markingSprites[i])
            return;
        DestroySprite(sMenu->markingSprites[i]);
    }

    DestroyMenuCursor();

    if (sMenu->textSprite)
        DestroySprite(sMenu->textSprite);
}

bool8 HandleMonMarkingsMenuInput(void)
{
    u16 i;
    struct SpriteSheet sheets[3] =
    {
        { sMenu->windowSpriteTiles, 0x1000, sMenu->baseTileTag },
        { gMonMarkingsMenu_Gfx, 0x520, sMenu->baseTileTag + 1 },
        { NULL, 0, 0 }
    };

    if (sMenu->spriteSheetLoadRequired)
    {
        LoadTilesForSpriteSheets(sheets);
        sMenu->spriteSheetLoadRequired = FALSE;
    }

    if (JOY_NEW(DPAD_UP))
    {
        s8 pos;
        PlaySE(SE_SELECT);
        pos = --sMenu->cursorPos;
        if (pos < 0)
            sMenu->cursorPos = 5;
        return TRUE;
    }

    if (JOY_NEW(DPAD_DOWN))
    {
        s8 pos;
        PlaySE(SE_SELECT);
        pos = ++sMenu->cursorPos;
        if (pos > 5)
            sMenu->cursorPos = 0;
        return TRUE;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);

        switch (sMenu->cursorPos)
        {
        case 4:
            sMenu->markings = 0;
            for (i = 0; i < 4; i++)
                sMenu->markings |= sMenu->markingsArray[i] << i;
            return FALSE;
        case 5:
            return FALSE;
        }

        sMenu->markingsArray[sMenu->cursorPos] = !sMenu->markingsArray[sMenu->cursorPos];
        return TRUE;
    }

    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        return FALSE;
    }

    return TRUE;
}

static void CreateMonMarkingsMenuSprites(s16 x, s16 y, u16 baseTileTag, u16 basePaletteTag)
{
    u16 i;
    u8 spriteId;

    struct SpriteSheet sheets[] =
    {
        { sMenu->windowSpriteTiles, 0x1000, baseTileTag },
        { gMonMarkingsMenu_Gfx, 0x520, baseTileTag + 1 },
        { NULL, 0 }
    };

    struct SpritePalette palettes[] =
    {
        { sMenu->framePalette, basePaletteTag },
        { gMonMarkingsMenu_Pal, basePaletteTag + 1},
        { NULL, 0 }
    };

    struct SpriteTemplate template =
    {
        baseTileTag,
        basePaletteTag,
        &sOamData_MenuWindow,
        sAnims_MenuWindow,
        NULL,
        gDummySpriteAffineAnimTable,
        SpriteCB_Dummy,
    };

    sMenu->spriteSheetLoadRequired = TRUE;
    AllocTilesForSpriteSheets(sheets);
    LoadSpritePalettes(palettes);

    for (i = 0; i < 2; i++)
    {
        spriteId = CreateSprite(&template, x + 32, y + 32, 2);
        if (spriteId != 64)
        {
            sMenu->windowSprites[i] = &gSprites[spriteId];
            StartSpriteAnim(&gSprites[spriteId], i);
        }
        else
        {
            sMenu->windowSprites[i] = NULL;
            return;
        }
    }

    sMenu->windowSprites[1]->y = y + 96;

    template.tileTag++;
    template.paletteTag++;
    template.anims = sAnims_MenuSprite;
    template.callback = SpriteCB_Marking;
    template.oam = &sOamData_8x8;

    for (i = 0; i < 4; i++)
    {
        spriteId = CreateSprite(&template, x + 32, y + 16 + 16 * i, 1);
        if (spriteId != 64)
        {
            sMenu->markingSprites[i] = &gSprites[spriteId];
            gSprites[spriteId].data[0] = i;
        }
        else
        {
            sMenu->markingSprites[i] = NULL;
            return;
        }
    }

    template.callback = SpriteCallbackDummy;

    spriteId = CreateSprite(&template, 0, 0, 1);

    if (spriteId != 64)
    {
        sMenu->textSprite = &gSprites[spriteId];
        sMenu->textSprite->oam.shape = ST_OAM_H_RECTANGLE;
        sMenu->textSprite->oam.size = 3;
        StartSpriteAnim(sMenu->textSprite, 9);
        sMenu->textSprite->x = x + MENU_TEXT_SPRITE_X_OFFSET;
        sMenu->textSprite->y = y + 80;
        CalcCenterToCornerVec(sMenu->textSprite, 1, 2, 0);
    }
    else
    {
        sMenu->textSprite = NULL;
    }

    sMenu->cursorBaseY = y + 8;
    CreateOutlineCursor(0, basePaletteTag + 1, 15, 0, 0x30);
    SetOutlineCursorPosition(x + 8, sMenu->cursorBaseY);
    SetOutlineCursorCallback(SpriteCB_Cursor);
}

static void SpriteCB_Dummy(struct Sprite *sprite)
{
}

static void SpriteCB_Marking(struct Sprite *sprite)
{
    if (sMenu->markingsArray[sprite->data[0]])
        StartSpriteAnim(sprite, 2 * sprite->data[0] + 1);
    else
        StartSpriteAnim(sprite, 2 * sprite->data[0]);
}

static void SpriteCB_Cursor(struct Sprite *sprite)
{
    sprite->y = 16 * sMenu->cursorPos + sMenu->cursorBaseY;
}

struct Sprite *CreateMonMarkingAllCombosSprite(u16 tileTag, u16 paletteTag, const u16 *palette)
{
    if (!palette)
        palette = sMonMarkings_Pal;
    return CreateMarkingComboSprite(tileTag, paletteTag, palette, 16);
}

struct Sprite *CreateMonMarkingComboSprite(u16 tileTag, u16 paletteTag, const u16 *palette)
{
    if (!palette)
        palette = sMonMarkings_Pal;
    return CreateMarkingComboSprite(tileTag, paletteTag, palette, 1);
}

static struct Sprite *CreateMarkingComboSprite(u16 tileTag, u16 paletteTag, const u16 *palette, u16 size)
{
    u8 spriteId;
    struct SpriteTemplate sprTemplate;
    struct SpriteSheet sheet = { sMonMarkings_Gfx, 0x80, tileTag };
    struct SpritePalette sprPalette = { palette, paletteTag };

    sprTemplate.tileTag = tileTag;
    sprTemplate.paletteTag = paletteTag;
    sprTemplate.oam = &sOamData_MarkingCombo;
    sprTemplate.anims = sAnims_MarkingCombo;
    sprTemplate.images = NULL;
    sprTemplate.affineAnims = gDummySpriteAffineAnimTable;
    sprTemplate.callback = SpriteCB_Dummy;

    sheet.size = size * 0x80;

    LoadSpriteSheet(&sheet);
    LoadSpritePalette(&sprPalette);

    spriteId = CreateSprite(&sprTemplate, 0, 0, 0);
    if (spriteId != 64)
        return  &gSprites[spriteId];
    else
        return NULL;
}

void UpdateMonMarkingTiles(u8 markings, void *dest)
{
    DmaCopy16Defvars(3, sMonMarkings_Gfx + markings * 0x80, dest, 0x80);
}
