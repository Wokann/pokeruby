
// Includes
#include "global.h"
#include "dma3.h"
#include "ewram.h"
#include "data.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "palette.h"
#include "string_util.h"
#include "text.h"
#include "menu.h"
#include "item.h"
#include "pokemon_summary_screen.h"
#include "pokemon_storage_system.h"
#include "sound.h"
#include "constants/songs.h"

// Static type declarations

struct Wallpaper {
    const u8 *tiles;
    u32 size;
    const u8 *tilemap;
    const u16 *palettes;
};

// Static RAM declarations

EWRAM_DATA static struct Pokemon sSavedMovingMon = {};
#if DEBUG
EWRAM_DATA u8 unk_2038790 = 0;
EWRAM_DATA u32 unk_2038794 = 0;
EWRAM_DATA u32 unk_2038798 = 0;
#endif
EWRAM_DATA static s8 sCursorArea = 0;
EWRAM_DATA static s8 sCursorPosition = 0;
EWRAM_DATA static bool8 sIsMonBeingMoved = FALSE;
EWRAM_DATA static u8 sMovingMonOrigBoxId = 0;
EWRAM_DATA static u8 sMovingMonOrigBoxPos = 0;
EWRAM_DATA static bool8 sAutoActionOn = 0;
EWRAM_DATA static u8 sSavedCursorPosition = 0;
EWRAM_DATA static struct UnkUtil *sUnkUtil = NULL;

// Static ROM declarations

static s8 DetermineBoxScrollDirection(u8 boxId);
static void LoadWallpaperGfx(u8 boxId, s8 direction);
static void CopyWallpaperTilemap(u16 *buffer, const u16 *tilemap, s8 direction, u8 offset);
static void ClearLowestWallpaperTiles(u16 *buffer);
static void InitBoxTitle(u8 boxId);
static void CreateIncomingBoxTitle(u8 boxId, s8 direction);
static void CycleBoxTitleSprites(void);
static void SpriteCB_IncomingBoxTitle(struct Sprite *sprite);
static void SpriteCB_OutgoingBoxTitle(struct Sprite *sprite);
static void CycleBoxTitleColor(void);
s16 sub_809A6D0(u8 width);
static void CreateBoxScrollArrows(void);
static void StartBoxScrollArrowsSlide(s8 direction);
static void StopBoxScrollArrowsSlide(void);
static void DoCursorNewPosUpdate(void);
static void SpriteCB_Arrow(struct Sprite *sprite);
static bool8 MonPlaceChange_Grab(void);
static bool8 MonPlaceChange_Place(void);
static bool8 MonPlaceChange_Shift(void);
static bool8 MonPlaceChange_CursorDown(void);
static bool8 MonPlaceChange_CursorUp(void);
static void MoveMon(void);
static void PlaceMon(void);
static void SetMovingMonData(u8 boxId, u8 position);
static void SetPlacedMonData(u8 boxId, u8 position);
static void PurgeMonOrBoxMon(u8 boxId, u8 position);
static void SetShiftedMonData(u8 boxId, u8 position);
static bool8 IsCursorOnBoxTitle(void);
static void TryRefreshDisplayMon(void);
static void ReshowDisplayMon(void);
static void SetDisplayMonData(void *pokemon, u8 mode);
static bool8 SetMenuTexts_Mon(void);
static void CreateCursorSprites(void);
static void ToggleCursorAutoAction(void);
static s8 GetMenuItemTextId(u8 menuIdx);
static void AddBoxOptionsMenu(void);
void sub_809D0BC(struct UnkUtilData*);
static void UnkUtil_DmaRun(struct UnkUtilData*);

// .rodata

#include "data/wallpapers.h"

const u16 PCPal_Arrow[] = INCBIN_U16("graphics/pokemon_storage/arrow.gbapal");
static const u8 sArrow_Gfx[] = INCBIN_U8("graphics/pokemon_storage/arrow.4bpp");

static const struct SpriteSheet sSpriteSheet_Arrow = {sArrow_Gfx, 0x80, 6};
const struct SpritePalette gUnknown_083BB290 = {PCPal_Arrow, 0xdacf};

static const struct OamData sOamData_BoxTitle = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 2
};

static const union AnimCmd sAnim_BoxTitle_Left[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_BoxTitle_Right[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_BoxTitle[] = {
    sAnim_BoxTitle_Left,
    sAnim_BoxTitle_Right
};

static const struct SpriteTemplate sSpriteTemplate_BoxTitle = {
    3,
    0xdac8,
    &sOamData_BoxTitle,
    sAnims_BoxTitle,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const struct OamData sOamData_Arrow = {
    .shape = ST_OAM_V_RECTANGLE,
    .priority = 2
};

static const union AnimCmd sAnim_Arrow_Left[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Arrow_Right[] = {
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Arrow[] = {
    sAnim_Arrow_Left,
    sAnim_Arrow_Right
};

static const struct SpriteTemplate sSpriteTemplate_Arrow = {
    6,
    0xdacf,
    &sOamData_Arrow,
    sAnims_Arrow,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_Arrow
};

const u16 HandCursorPalette[] = INCBIN_U16("graphics/pokemon_storage/hand_cursor_pal.bin");
const u16 HandCursorAltPalette[] = INCBIN_U16("graphics/pokemon_storage/hand_cursor_alt_pal.bin");
const u8 gHandCursor_Gfx[] = INCBIN_U8("graphics/pokemon_storage/hand_cursor.4bpp");
const u8 gHandCursorShadow_Gfx[] = INCBIN_U8("graphics/pokemon_storage/hand_cursor_shadow.4bpp");

static bool8 (*const sPlaceChangeFuncs[])(void) = {
    MonPlaceChange_Grab,
    MonPlaceChange_Place,
    MonPlaceChange_Shift
};

extern const struct SpriteSheet gHandCursorSpriteSheets[3];
extern const struct SpritePalette gHandCursorSpritePalettes[3];
extern const struct SpriteTemplate gSpriteTemplate_Cursor;
extern const struct SpriteTemplate gSpriteTemplate_CursorShadow;
extern const u8 *const gMenuTexts[];
extern struct PokemonStorageSystemFunc gInputFuncs[];

// .text

void sub_8099BF8(u8 boxId)
{
    gPokemonStorageSystemPtr->wallpaperOffset = FALSE;
    gPokemonStorageSystemPtr->bg2_X = 0;
    DmaFill32(3, 0, BG_SCREEN_ADDR(26), 0x1000);
    LoadWallpaperGfx(boxId, 0);
    InitBoxTitle(boxId);
    CreateBoxScrollArrows();
    InitBoxMonSprites(boxId);
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(26) | BGCNT_TXT512x256;
}

void SetUpScrollToBox(u8 boxId)
{
    s8 direction = DetermineBoxScrollDirection(boxId);
    LoadWallpaperGfx(boxId, direction);
    gPokemonStorageSystemPtr->scrollSpeed = direction * 6;
    gPokemonStorageSystemPtr->scrollTimer = 0x20;
    gPokemonStorageSystemPtr->scrollToBoxIdUnused = boxId;
    gPokemonStorageSystemPtr->scrollUnused2 = direction > 0 ? 0 : 5;
    gPokemonStorageSystemPtr->scrollDirectionUnused = direction;
    gPokemonStorageSystemPtr->scrollUnused3 = direction > 0 ? 0x108 : 0x38;
    gPokemonStorageSystemPtr->scrollUnused4 = direction > 0 ? 0 : 5;
    gPokemonStorageSystemPtr->unk_08c4 = direction;
    gPokemonStorageSystemPtr->scrollUnused5 = 0;
    gPokemonStorageSystemPtr->scrollUnused6 = 2;
    InitBoxMonIconScroll(boxId, direction);
    CreateIncomingBoxTitle(boxId, direction);
    StartBoxScrollArrowsSlide(direction);
}

bool8 ScrollToBox(void)
{
    bool8 iconsScrolling = UpdateBoxMonIconScroll();
    if (gPokemonStorageSystemPtr->scrollTimer != 0)
    {
        gPokemonStorageSystemPtr->bg2_X += gPokemonStorageSystemPtr->scrollSpeed;
        gPokemonStorageSystemPtr->bg2_X &= 0x1ff;
        if (--gPokemonStorageSystemPtr->scrollTimer == 0)
        {
            CycleBoxTitleSprites();
            StopBoxScrollArrowsSlide();
        }
        return TRUE;
    }
    return iconsScrolling;
}

static s8 DetermineBoxScrollDirection(u8 boxId)
{
    u8 currentBox = StorageGetCurrentBox();
    u8 i;

    for (i = 0; currentBox != boxId; i++)
    {
        if (++currentBox >= 14)
            currentBox = 0;
    }
    return i <= 6 ? 1 : -1;
}

void SetWallpaperForCurrentBox(u8 wallpaperId)
{
    u8 boxId = StorageGetCurrentBox();
    gPokemonStorage.boxWallpapers[boxId] = wallpaperId;
    gPokemonStorageSystemPtr->wallpaperChangeState = 0;
}

bool8 DoWallpaperGfxChange(void)
{
    switch (gPokemonStorageSystemPtr->wallpaperChangeState)
    {
        case 0:
            BeginNormalPaletteFade(gPokemonStorageSystemPtr->wallpaperPalBits, 1, 0, 16, RGB_WHITEALPHA);
            gPokemonStorageSystemPtr->wallpaperChangeState++;
            break;
        case 1:
            if (!UpdatePaletteFade())
            {
                u8 curBox = StorageGetCurrentBox();
                LoadWallpaperGfx(curBox, 0);
                CycleBoxTitleColor();
                BeginNormalPaletteFade(gPokemonStorageSystemPtr->wallpaperPalBits, 1, 16, 0, RGB_WHITEALPHA);
                gPokemonStorageSystemPtr->wallpaperChangeState++;
            }
            break;
        case 2:
            if (!UpdatePaletteFade())
                gPokemonStorageSystemPtr->wallpaperChangeState++;
            break;
        case 3:
            return FALSE;
    }
    return TRUE;
}

static void LoadWallpaperGfx(u8 boxId, s8 direction)
{
    const struct Wallpaper *wallpaper;

    if (direction)
    {
        gPokemonStorageSystemPtr->wallpaperOffset = gPokemonStorageSystemPtr->wallpaperOffset ? FALSE : TRUE;
        ClearLowestWallpaperTiles(BG_SCREEN_ADDR(26));
    }
    wallpaper = sWallpapers +gPokemonStorage.boxWallpapers[boxId];
    LoadPalette(wallpaper->palettes, gPokemonStorageSystemPtr->wallpaperOffset * 0x30 + 0x40, 0x60);
    LZ77UnCompWram(wallpaper->tilemap, gPokemonStorageSystemPtr->wallpaperTilemap);
    CopyWallpaperTilemap(BG_SCREEN_ADDR(26), gPokemonStorageSystemPtr->wallpaperTilemap, direction, gPokemonStorageSystemPtr->wallpaperOffset);
    LZ77UnCompVram(wallpaper->tiles, BG_CHAR_ADDR(2) + (gPokemonStorageSystemPtr->wallpaperOffset << 13));
}

/*
    Copies the wallpaper rectangle tilemap to a buffer,
    depending on the wallpaper type.

    - buffer    : Where to copy the wallpaper to.
    - tilemap   : The wallpaper tilemap to copy.
    - direction : The direction of the wallpaper.
    - offset    : The offset of the wallpaper tilemap.

    Also depends on stored BG2 X variable in PSS.
*/
static void CopyWallpaperTilemap(u16 *buffer, const u16 *tilemap, s8 direction, u8 offset)
{
    u16 *dest; // For either all of the wallpaper tilemap, or the first half of it.
    u16 *dest2; // The second half of wallpaper tilemap; used in case of two halves.
    u16 *fillDest; // For blanking; used when direction is set.

    u16 x, y; // Temps for copying the tilemap.

    // Temps for copying the tile and palette metadata of the wallpaper.
    u16 tileMeta;
    u16 tileOffset = offset * 256;
    u16 paletteMeta;
    u16 paletteOffset = (offset * 3 + 4) << 12;

    s16 rectWidth; // The width of the wallpaper, or it's first half.
    s16 rectWidth2; // Used in case of two halves.
    s16 rectX = ((gPokemonStorageSystemPtr->bg2_X / 8 + 10) + (direction * 24)) & 0x3F;

    s16 fillX; // For blanking; used when direction is set.

    /*
        Bull**** agbcc behavior discovered here
        while attempting to match this function:
        x + (y + z) is NOT the same as x + y + z.
        Even though it SHOULDN'T make a difference.

        Rather, it's the same as x + z + y. That's
        old codegen for you.
    */
    if (rectX < 13)
    {
        // Copy the wallpaper in full.
        rectWidth = 20;
        rectWidth2 = 0;
        dest = buffer + (rectX + 64);
        dest2 = NULL;
    }
    else if (rectX < 32)
    {
        // Split copying the wallpaper into two operations.
        rectWidth = 32 - rectX;
        rectWidth2 = 20 - rectWidth;
        dest = buffer + (rectX + 64);
        dest2 = buffer + 0x440;
    }
    else if (rectX < 45)
    {
        // Copy the wallpaper in full.
        rectWidth = 20;
        rectWidth2 = 0;
        dest = buffer + (rectX + 0x420);
        dest2 = NULL;
    }
    else
    {
        // Split copying the wallpaper into two operations.
        rectWidth = 64 - rectX;
        rectWidth2 = 20 - rectWidth;
        dest = buffer + (rectX + 0x420);
        dest2 = buffer + 0x40;
    }

    for (y = 0; y < 18; y++)
    {
        for(x = 0; x < rectWidth; x++)
        {
            // Get the tile and palette metadata for each
            // 2 bytes, and copy over into the buffer.
            tileMeta = (*tilemap & 0xfff) + tileOffset;
            paletteMeta =  (*tilemap & 0xf000) + paletteOffset;
            dest[x] = paletteMeta | tileMeta;
            tilemap++;
        }
        for(x = 0; x < rectWidth2; x++)
        {
            // Repeat the same process again in case we need
            // to copy halves.
            tileMeta = (*tilemap & 0xfff) + tileOffset;
            paletteMeta =  (*tilemap & 0xf000) + paletteOffset;
            dest2[x] = paletteMeta | tileMeta;
            tilemap++;
        }
        dest += 0x20;
        dest2 += 0x20;
    }

    if (direction != 0)
    {
        fillX = (direction > 0 ? rectX + 20 : rectX - 4) & 0x3F;

        if(fillX < 32)
            fillDest = buffer + (fillX + 0x40);
        else
            fillDest = buffer + (fillX + 0x420);

        for(x = 0; x < 4; x++)
        {
            for(y = 0; y < 18; y++)
            {
                *fillDest = 0; // Blank the buffer.
                fillDest += 0x20;
            }
            // Needs to be one statement, or else it won't match.
            // fillX = ++fillX & 0x3F; matches, but to have consistency
            // with ClearLowestWallpaperTiles, which requires var + 1
            // instead for a similar operation, we'll use that.
            fillX = (fillX + 1) & 0x3F;

            if (fillX < 32)
                fillDest = buffer + (fillX + 0x40);
            else
                fillDest = buffer + (fillX + 0x420);
        }
    }
}

/*
    Clears the lowest line of a wallpaper buffer.
    Depends on stored BG2 X variable in PSS.

    - buffer : Wallpaper buffer.
*/
static void ClearLowestWallpaperTiles(u16 *buffer)
{
    u16 x;
    s16 rectX = ((gPokemonStorageSystemPtr->bg2_X / 8) + 30) & 0x3F;

    if (rectX < 32)
        buffer += rectX + 0x260;
    else
        buffer += rectX + 0x640;

    for (x = 0; x < 44; x++)
    {
        *buffer++ = 0;
        // Needs to be one statement in order to match.
        rectX = (rectX + 1) & 0x3F;
        if (rectX == 0)
            buffer -= 0x420;
        if (rectX == 32)
            buffer += 0x3e0;
    }
}

void sub_809A1BC(const u8 *dst, const u8 *string)
{
    u8 *r5 = gUnknown_083B6DB8;
    DmaClear16(3, r5, 0x200);
    Text_InitWindow8004E3C(&gWindowTemplate_81E6D38, r5, string);
    DmaCopy16(3, r5 + 0x000, dst + 0x000, 0x80);
    DmaCopy16(3, r5 + 0x100, dst + 0x080, 0x80);
    DmaCopy16(3, r5 + 0x080, dst + 0x100, 0x80);
    DmaCopy16(3, r5 + 0x180, dst + 0x180, 0x80);
}

static const struct SpriteTemplate sSpriteTemplate_BoxTitle;

static void InitBoxTitle(u8 boxId)
{
    u8 tagIndex;
    s16 x;
    u16 i;

    struct SpriteSheet spriteSheet = {gPokemonStorageSystemPtr->boxTitleTiles, 0x200, 3};
    struct SpritePalette palettes[] = {
        {gPokemonStorageSystemPtr->boxTitlePal, 0xdac8},
        {gPokemonStorageSystemPtr->boxTitlePal, 0xdac9},
        {}
    };

    u16 wallpaperId = gPokemonStorage.boxWallpapers[boxId];
    gPokemonStorageSystemPtr->boxTitlePal[14] = sBoxTitleColors[wallpaperId][0];
    gPokemonStorageSystemPtr->boxTitlePal[15] = sBoxTitleColors[wallpaperId][1];
    LoadSpritePalettes(palettes);
    gPokemonStorageSystemPtr->wallpaperPalBits = 0x3f0;
    tagIndex = IndexOfSpritePaletteTag(0xdac8);
    gPokemonStorageSystemPtr->boxTitlePalOffset = 0x10e + 16 * tagIndex;
    gPokemonStorageSystemPtr->wallpaperPalBits |= 0x10000 << tagIndex;
    tagIndex = IndexOfSpritePaletteTag(0xdac9);
    gPokemonStorageSystemPtr->boxTitleAltPalOffset = 0x10e + 16 * tagIndex;
    gPokemonStorageSystemPtr->wallpaperPalBits |= 0x10000 << tagIndex;
    sub_809A1BC(gPokemonStorageSystemPtr->boxTitleTiles, gPokemonStorage.boxNames[boxId]);
    LoadSpriteSheet(&spriteSheet);
    x = sub_809A6D0(GetStringWidthInMenuWindow(gPokemonStorage.boxNames[boxId]));
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_BoxTitle, x + i * 32, 0x1c, 23);
        gPokemonStorageSystemPtr->curBoxTitleSprites[i] = gSprites + spriteId;
        StartSpriteAnim(gPokemonStorageSystemPtr->curBoxTitleSprites[i], i);
    }
    gPokemonStorageSystemPtr->boxTitleCycleId = 0;
}

static void CreateIncomingBoxTitle(u8 boxId, s8 direction)
{
    u16 palOffset;
    s16 x;
    s16 adjustedX;
    u16 i;
    struct SpriteSheet spriteSheet = {gPokemonStorageSystemPtr->boxTitleTiles, 0x200, 3};
    struct SpriteTemplate template = sSpriteTemplate_BoxTitle;

    gPokemonStorageSystemPtr->boxTitleCycleId = gPokemonStorageSystemPtr->boxTitleCycleId ? FALSE : TRUE;
    if (gPokemonStorageSystemPtr->boxTitleCycleId == 0)
    {
        spriteSheet.tag = 3;
        palOffset = gPokemonStorageSystemPtr->boxTitlePalOffset;
    }
    else
    {
        spriteSheet.tag = 4;
        palOffset = gPokemonStorageSystemPtr->boxTitlePalOffset;
        template.tileTag = 4;
        template.paletteTag = 0xdac9;
    }
    sub_809A1BC(gPokemonStorageSystemPtr->boxTitleTiles, gPokemonStorage.boxNames[boxId]);
    LoadSpriteSheet(&spriteSheet);
    LoadPalette(sBoxTitleColors[gPokemonStorage.boxWallpapers[boxId]], palOffset, 0x04);
    x = sub_809A6D0(GetStringWidthInMenuWindow(gPokemonStorage.boxNames[boxId]));
    adjustedX = x + direction * 192;
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&template, i * 32 + adjustedX, 0x1c, 23);
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i] = gSprites + spriteId;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->data[0] = (-direction) * 6;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->data[1] = i * 32 + x;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->data[2] = 1;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->callback = SpriteCB_IncomingBoxTitle;
        StartSpriteAnim(gPokemonStorageSystemPtr->nextBoxTitleSprites[i], i);
        gPokemonStorageSystemPtr->curBoxTitleSprites[i]->data[0] = (-direction) * 6;
        gPokemonStorageSystemPtr->curBoxTitleSprites[i]->data[1] = 1;
        gPokemonStorageSystemPtr->curBoxTitleSprites[i]->callback = SpriteCB_OutgoingBoxTitle;
    }
}

static void CycleBoxTitleSprites(void)
{
    if (gPokemonStorageSystemPtr->boxTitleCycleId == 0)
        FreeSpriteTilesByTag(4);
    else
        FreeSpriteTilesByTag(3);
    gPokemonStorageSystemPtr->curBoxTitleSprites[0] = gPokemonStorageSystemPtr->nextBoxTitleSprites[0];
    gPokemonStorageSystemPtr->curBoxTitleSprites[1] = gPokemonStorageSystemPtr->nextBoxTitleSprites[1];
}

static void SpriteCB_IncomingBoxTitle(struct Sprite *sprite)
{
    if (sprite->data[2])
        sprite->data[2]--;
    else if ((sprite->x += sprite->data[0]) == sprite->data[1])
        sprite->callback = SpriteCallbackDummy;
}

static void SpriteCB_OutgoingBoxTitle(struct Sprite *sprite)
{
    if (sprite->data[1])
        sprite->data[1]--;
    else
    {
        sprite->x += sprite->data[0];
        sprite->data[2] = sprite->x + sprite->x2;
        if (sprite->data[2] < 0x40 || sprite->data[2] > 0x100)
            DestroySprite(sprite);
    }
}

static void CycleBoxTitleColor(void)
{
    u8 boxId = StorageGetCurrentBox();
    u8 wallpaperId = gPokemonStorage.boxWallpapers[boxId];
    if (gPokemonStorageSystemPtr->boxTitleCycleId == 0)
        CpuCopy16(sBoxTitleColors[wallpaperId], gPlttBufferUnfaded + gPokemonStorageSystemPtr->boxTitlePalOffset, 4);
    else
        CpuCopy16(sBoxTitleColors[wallpaperId], gPlttBufferUnfaded + gPokemonStorageSystemPtr->boxTitleAltPalOffset, 4);
}

s16 sub_809A6D0(u8 width)
{
    return 0xb0 - width / 2;
}

static void CreateBoxScrollArrows(void)
{
    u16 i;

    LoadSpriteSheet(&sSpriteSheet_Arrow);
    LoadSpritePalette(&gUnknown_083BB290);
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_Arrow, 0x5c + i * 0x88, 0x1c, 21);
        if (spriteId != MAX_SPRITES)
        {
            struct Sprite *sprite = gSprites + spriteId;
            StartSpriteAnim(sprite, i);
            sprite->data[3] = (i == 0) ? -1 : 1;
            gPokemonStorageSystemPtr->arrowSprites[i] = sprite;
        }
    }
    if (IsCursorOnBoxTitle())
        AnimateBoxScrollArrows(TRUE);
}

static void StartBoxScrollArrowsSlide(s8 direction)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        gPokemonStorageSystemPtr->arrowSprites[i]->x2 = 0;
        gPokemonStorageSystemPtr->arrowSprites[i]->data[0] = 2;
    }
    if (direction < 0)
    {
        gPokemonStorageSystemPtr->arrowSprites[0]->data[1] = 29;
        gPokemonStorageSystemPtr->arrowSprites[1]->data[1] = 5;
        gPokemonStorageSystemPtr->arrowSprites[0]->data[2] = 0x48;
        gPokemonStorageSystemPtr->arrowSprites[1]->data[2] = 0x48;
    }
    else
    {
        gPokemonStorageSystemPtr->arrowSprites[0]->data[1] = 5;
        gPokemonStorageSystemPtr->arrowSprites[1]->data[1] = 29;
        gPokemonStorageSystemPtr->arrowSprites[0]->data[2] = 0xF8;
        gPokemonStorageSystemPtr->arrowSprites[1]->data[2] = 0xF8;
    }
    gPokemonStorageSystemPtr->arrowSprites[0]->data[7] = 0;
    gPokemonStorageSystemPtr->arrowSprites[1]->data[7] = 1;
}

static void StopBoxScrollArrowsSlide(void)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        gPokemonStorageSystemPtr->arrowSprites[i]->x = 0x88 * i + 0x5c;
        gPokemonStorageSystemPtr->arrowSprites[i]->x2 = 0;
        gPokemonStorageSystemPtr->arrowSprites[i]->invisible = FALSE;
    }
    AnimateBoxScrollArrows(TRUE);
}

void AnimateBoxScrollArrows(bool8 animate)
{
    u16 i;
    if (animate)
    {
        for (i = 0; i < 2; i++)
        {
            gPokemonStorageSystemPtr->arrowSprites[i]->data[0] = 1;
            gPokemonStorageSystemPtr->arrowSprites[i]->data[1] = 0;
            gPokemonStorageSystemPtr->arrowSprites[i]->data[2] = 0;
            gPokemonStorageSystemPtr->arrowSprites[i]->data[4] = 0;
        }
    }
        else
    {
        for (i = 0; i < 2; i++)
        {
            gPokemonStorageSystemPtr->arrowSprites[i]->data[0] = 0;
        }
    }
}

static void SpriteCB_Arrow(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
        case 0:
            sprite->x2 = 0;
            break;
        case 1:
            if (++sprite->data[1] > 3)
            {
                sprite->data[1] = 0;
                sprite->x2 += sprite->data[3];
                if (++sprite->data[2] > 5)
                {
                    sprite->data[2] = 0;
                    sprite->x2 = 0;
                }
            }
            break;
        case 2:
            sprite->data[0] = 3;
            break;
        case 3:
            sprite->x -= gPokemonStorageSystemPtr->scrollSpeed;
            if (sprite->x < 0x49 || sprite->x > 0xf7)
                sprite->invisible = TRUE;
            if (--sprite->data[1] == 0)
            {
                sprite->x = sprite->data[2];
                sprite->invisible = FALSE;
                sprite->data[0] = 4;
            }
            break;
        case 4:
            sprite->x -= gPokemonStorageSystemPtr->scrollSpeed;
            break;
    }
}

struct Sprite *CreateChooseBoxArrows(u16 x, u16 y, u8 animId, u8 priority, u8 subpriority)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_Arrow, x, y, subpriority);
    if (spriteId == MAX_SPRITES)
        return NULL;
    animId %= 2;
    StartSpriteAnim(gSprites + spriteId, animId);
    gSprites[spriteId].oam.priority = priority;
    gSprites[spriteId].callback = SpriteCallbackDummy;
    return gSprites + spriteId;
}

void InitCursor(void)
{
    if (gPokemonStorageSystemPtr->boxOption != 1)
        sCursorArea = 0;
    else
        sCursorArea = 1;
    sCursorPosition = 0;
    sIsMonBeingMoved = FALSE;
    sMovingMonOrigBoxId = 0;
    sMovingMonOrigBoxPos = 0;
    sAutoActionOn = 0;
    ClearSavedCursorPos();
    CreateCursorSprites();
    gPokemonStorageSystemPtr->cursorPrevHorizPos = 1;
    TryRefreshDisplayMon();
}

void InitCursorOnReopen(void)
{
    CreateCursorSprites();
    ReshowDisplayMon();
    gPokemonStorageSystemPtr->cursorPrevHorizPos = 1;
    if (sIsMonBeingMoved)
        CreateMovingMonIcon();
}

static void GetCursorCoordsByPos(u8 cursorArea, u8 cursorPosition, u16 *x, u16 *y)
{
    switch (cursorArea)
    {
        case 0:
            *x = (cursorPosition % 6) * 24 + 100;
            *y = (cursorPosition / 6) * 24 +  32;
            break;
        case 1:
            if (cursorPosition == 0)
            {
                *x = 0x68;
                *y = 0x34;
            }
            else if (cursorPosition == 6)
            {
                *x = 0x98;
                *y = 0x84;
            }
            else
            {
                *x = 0x98;
                *y = (cursorPosition - 1) * 24 + 4;
            }
            break;
        case 2:
            *x = 0xa2;
            *y = 0x0c;
            break;
        case 3:
            *y = sIsMonBeingMoved ? 8 : 14;
            *x = cursorPosition * 0x58 + 0x78;
            break;
        case 4:
            *x = 0xa0;
            *y = 0x60;
            break;
    }
}

static u16 GetSpeciesAtCursorPosition(void)
{
    switch (sCursorArea)
    {
        case 1:
            return GetMonData(gPlayerParty + sCursorPosition, MON_DATA_SPECIES);
        case 0:
            return GetBoxMonData(gPokemonStorage.boxes[StorageGetCurrentBox()] + sCursorPosition, MON_DATA_SPECIES);
        default:
            return SPECIES_NONE;
    }
}

bool8 UpdateCursorPos(void)
{
    s16 tmp;
    if (gPokemonStorageSystemPtr->cursorMoveSteps == 0)
        return FALSE;
    if (--gPokemonStorageSystemPtr->cursorMoveSteps)
    {
        gPokemonStorageSystemPtr->cursorNewX += gPokemonStorageSystemPtr->cursorSpeedX;
        gPokemonStorageSystemPtr->cursorNewY += gPokemonStorageSystemPtr->cursorSpeedY;
        gPokemonStorageSystemPtr->cursorSprite->x = gPokemonStorageSystemPtr->cursorNewX >> 8;
        gPokemonStorageSystemPtr->cursorSprite->y = gPokemonStorageSystemPtr->cursorNewY >> 8;
        if (gPokemonStorageSystemPtr->cursorSprite->x > 0x100)
        {
            tmp = gPokemonStorageSystemPtr->cursorSprite->x - 0x100;
            gPokemonStorageSystemPtr->cursorSprite->x = tmp + 0x40;
        }
        if (gPokemonStorageSystemPtr->cursorSprite->x < 0x40)
        {
            tmp = 0x40 - gPokemonStorageSystemPtr->cursorSprite->x;
            gPokemonStorageSystemPtr->cursorSprite->x = 0x100 - tmp;
        }
        if (gPokemonStorageSystemPtr->cursorSprite->y > 0xb0)
        {
            tmp = gPokemonStorageSystemPtr->cursorSprite->y - 0xb0;
            gPokemonStorageSystemPtr->cursorSprite->y = tmp - 0x10;
        }
        if (gPokemonStorageSystemPtr->cursorSprite->y < -0x10)
        {
            tmp = -0x10 - gPokemonStorageSystemPtr->cursorSprite->y;
            gPokemonStorageSystemPtr->cursorSprite->y = 0xb0 - tmp;
        }
        if (gPokemonStorageSystemPtr->cursorFlipTimer && --gPokemonStorageSystemPtr->cursorFlipTimer == 0)
            gPokemonStorageSystemPtr->cursorSprite->vFlip = gPokemonStorageSystemPtr->cursorSprite->vFlip ? FALSE : TRUE;
    }
    else
    {
        gPokemonStorageSystemPtr->cursorSprite->x = gPokemonStorageSystemPtr->cursorTargetX;
        gPokemonStorageSystemPtr->cursorSprite->y = gPokemonStorageSystemPtr->cursorTargetY;
        DoCursorNewPosUpdate();
    }
    return TRUE;
}

static void InitNewCursorPos(u8 newCursorArea, u8 newCursorPosition)
{
    u16 x;
    u16 y;

    GetCursorCoordsByPos(newCursorArea, newCursorPosition, &x, &y);
    gPokemonStorageSystemPtr->newCursorArea = newCursorArea;
    gPokemonStorageSystemPtr->newCursorPosition = newCursorPosition;
    gPokemonStorageSystemPtr->cursorTargetX = x;
    gPokemonStorageSystemPtr->cursorTargetY = y;
}

static void InitCursorMove(void)
{
    int yDistance;
    int xDistance;

    if (gPokemonStorageSystemPtr->cursorVerticalWrap || gPokemonStorageSystemPtr->cursorHorizontalWrap)
        gPokemonStorageSystemPtr->cursorMoveSteps = 12;
    else
        gPokemonStorageSystemPtr->cursorMoveSteps = 6;
    if (gPokemonStorageSystemPtr->cursorFlipTimer)
        gPokemonStorageSystemPtr->cursorFlipTimer = gPokemonStorageSystemPtr->cursorMoveSteps >> 1;
    switch (gPokemonStorageSystemPtr->cursorVerticalWrap)
    {
        default:
            yDistance = gPokemonStorageSystemPtr->cursorTargetY - gPokemonStorageSystemPtr->cursorSprite->y;
            break;
        case -1:
            yDistance = gPokemonStorageSystemPtr->cursorTargetY - 0xc0 - gPokemonStorageSystemPtr->cursorSprite->y;
            break;
        case 1:
            yDistance = gPokemonStorageSystemPtr->cursorTargetY + 0xc0 - gPokemonStorageSystemPtr->cursorSprite->y;
            break;
    }
    switch (gPokemonStorageSystemPtr->cursorHorizontalWrap)
    {
        default:
            xDistance = gPokemonStorageSystemPtr->cursorTargetX - gPokemonStorageSystemPtr->cursorSprite->x;
            break;
        case -1:
            xDistance = gPokemonStorageSystemPtr->cursorTargetX - 0xc0 - gPokemonStorageSystemPtr->cursorSprite->x;
            break;
        case 1:
            xDistance = gPokemonStorageSystemPtr->cursorTargetX + 0xc0 - gPokemonStorageSystemPtr->cursorSprite->x;
            break;
    }
    yDistance <<= 8;
    xDistance <<= 8;
    gPokemonStorageSystemPtr->cursorSpeedX = xDistance / gPokemonStorageSystemPtr->cursorMoveSteps;
    gPokemonStorageSystemPtr->cursorSpeedY = yDistance / gPokemonStorageSystemPtr->cursorMoveSteps;
    gPokemonStorageSystemPtr->cursorNewX = gPokemonStorageSystemPtr->cursorSprite->x << 8;
    gPokemonStorageSystemPtr->cursorNewY = gPokemonStorageSystemPtr->cursorSprite->y << 8;
}

static void SetCursorPosition(u8 newCursorArea, u8 newCursorPosition)
{
    InitNewCursorPos(newCursorArea, newCursorPosition);
    InitCursorMove();
    if (!sIsMonBeingMoved)
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 1);
    if (newCursorArea == 1 && sCursorArea != 1)
    {
        gPokemonStorageSystemPtr->cursorPrevHorizPos = newCursorArea;
        gPokemonStorageSystemPtr->cursorShadowSprite->invisible = TRUE;
    }
    switch (newCursorArea)
    {
        case 0:
            break;
        case 1 ... 3:
            gPokemonStorageSystemPtr->cursorShadowSprite->invisible = TRUE;
            gPokemonStorageSystemPtr->cursorShadowSprite->oam.priority = 1;
            break;
    }
}

static void DoCursorNewPosUpdate(void)
{
    sCursorArea = gPokemonStorageSystemPtr->newCursorArea;
    sCursorPosition = gPokemonStorageSystemPtr->newCursorPosition;
    if (!sIsMonBeingMoved)
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 0);
    TryRefreshDisplayMon();
    switch (sCursorArea)
    {
        case 2:
            AnimateBoxScrollArrows(TRUE);
            break;
        case 1:
            gPokemonStorageSystemPtr->cursorShadowSprite->subpriority = 12;
            break;
        case 0:
            gPokemonStorageSystemPtr->cursorShadowSprite->oam.priority = 2;
            gPokemonStorageSystemPtr->cursorShadowSprite->subpriority = 20;
            gPokemonStorageSystemPtr->cursorShadowSprite->invisible = FALSE;
            break;
    }
}

void SetCursorInParty(void)
{
    u8 partyCount;
    if (!sIsMonBeingMoved)
        partyCount = 0;
    else
    {
        partyCount = CalculatePlayerPartyCount();
        if (partyCount > PARTY_SIZE - 1)
            partyCount = PARTY_SIZE - 1;
    }
    if (gPokemonStorageSystemPtr->cursorSprite->vFlip)
        gPokemonStorageSystemPtr->cursorFlipTimer = 1;
    SetCursorPosition(1, partyCount);
}

void SetCursorBoxPosition(u8 cursorBoxPosition)
{
    SetCursorPosition(0, cursorBoxPosition);
}

void ClearSavedCursorPos(void)
{
    sSavedCursorPosition = 0;
}

void SaveCursorPos(void)
{
    sSavedCursorPosition = sCursorPosition;
}

u8 GetSavedCursorPos(void)
{
    return sSavedCursorPosition;
}

void InitMonPlaceChange(u8 type)
{
    gPokemonStorageSystemPtr->monPlaceChangeFunc = sPlaceChangeFuncs[type];
    gPokemonStorageSystemPtr->monPlaceChangeState = 0;
}

bool8 DoMonPlaceChange(void)
{
    return gPokemonStorageSystemPtr->monPlaceChangeFunc();
}

static bool8 MonPlaceChange_Grab(void)
{
    switch (gPokemonStorageSystemPtr->monPlaceChangeState)
    {
        case 0:
            if (sIsMonBeingMoved)
                return FALSE;
            StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 2);
            gPokemonStorageSystemPtr->monPlaceChangeState++;
            break;
        case 1:
            if (!MonPlaceChange_CursorDown())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 3);
                MoveMon();
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 2:
            if (!MonPlaceChange_CursorUp())
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            break;
        case 3:
            return FALSE;
    }
    return TRUE;
}

static bool8 MonPlaceChange_Place(void)
{
    switch (gPokemonStorageSystemPtr->monPlaceChangeState)
    {
        case 0:
            if (!MonPlaceChange_CursorDown())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 2);
                PlaceMon();
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 1:
            if (!MonPlaceChange_CursorUp())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 0);
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 2:
            return FALSE;
    }
    return TRUE;
}

static bool8 MonPlaceChange_Shift(void)
{
    switch (gPokemonStorageSystemPtr->monPlaceChangeState)
    {
        case 0:
            switch (sCursorArea)
            {
                case 1:
                    gPokemonStorageSystemPtr->shiftBoxId = 14;
                    break;
                case 0:
                    gPokemonStorageSystemPtr->shiftBoxId = StorageGetCurrentBox();
                    break;
                default:
                    return FALSE;
            }
            StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 2);
            SaveMonSpriteAtPos(gPokemonStorageSystemPtr->shiftBoxId, sCursorPosition);
            gPokemonStorageSystemPtr->monPlaceChangeState++;
            break;
        case 1:
            if (!MoveShiftingMons())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 3);
                SetShiftedMonData(gPokemonStorageSystemPtr->shiftBoxId, sCursorPosition);
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 2:
            return FALSE;
    }
    return TRUE;
}

static bool8 MonPlaceChange_CursorDown(void)
{
    switch (gPokemonStorageSystemPtr->cursorSprite->y2)
    {
        default:
            gPokemonStorageSystemPtr->cursorSprite->y2++;
            break;
        case 0:
            gPokemonStorageSystemPtr->cursorSprite->y2++;
            break;
        case 8:
            return FALSE;
    }
    return TRUE;
}

static bool8 MonPlaceChange_CursorUp(void)
{
    switch (gPokemonStorageSystemPtr->cursorSprite->y2)
    {
        case 0:
            return FALSE;
        default:
            gPokemonStorageSystemPtr->cursorSprite->y2--;
            break;
    }
    return TRUE;
}

static void MoveMon(void)
{
    switch (sCursorArea)
    {
        case 1:
            SetMovingMonData(14, sCursorPosition);
            SetMovingMonSprite(0, sCursorPosition);
            break;
        case 0:
            SetMovingMonData(StorageGetCurrentBox(), sCursorPosition);
            SetMovingMonSprite(1, sCursorPosition);
            break;
        default:
            return;
    }
    sIsMonBeingMoved = TRUE;
}

static void PlaceMon(void)
{
    u8 boxId;
    switch (sCursorArea)
    {
        case 1:
            SetPlacedMonData(14, sCursorPosition);
            SetPlacedMonSprite(14, sCursorPosition);
            break;
        case 0:
            boxId = StorageGetCurrentBox();
            SetPlacedMonData(boxId, sCursorPosition);
            SetPlacedMonSprite(boxId, sCursorPosition);
            break;
        default:
            return;
    }
    sIsMonBeingMoved = FALSE;
}

void RefreshDisplayMon(void)
{
    TryRefreshDisplayMon();
}

static void SetMovingMonData(u8 boxId, u8 position)
{
    if (boxId == 14)
        gPokemonStorageSystemPtr->movingMon = gPlayerParty[sCursorPosition];
    else
        BoxMonToMon(gPokemonStorage.boxes[boxId] + position, &gPokemonStorageSystemPtr->movingMon);
    PurgeMonOrBoxMon(boxId, position);
    sMovingMonOrigBoxId = boxId;
    sMovingMonOrigBoxPos = position;
}

static void SetPlacedMonData(u8 boxId, u8 position)
{
    if (boxId == 14)
    {
        gPlayerParty[position] = gPokemonStorageSystemPtr->movingMon;
    }
    else
    {
        BoxMonRestorePP(&gPokemonStorageSystemPtr->movingMon.box);
        gPokemonStorage.boxes[boxId][position] = gPokemonStorageSystemPtr->movingMon.box;
    }
}

static void PurgeMonOrBoxMon(u8 boxId, u8 position)
{
    if (boxId == 14)
        ZeroMonData(gPlayerParty + position);
    else
        ZeroBoxMonData(gPokemonStorage.boxes[boxId] + position);
}

static void SetShiftedMonData(u8 boxId, u8 position)
{
    if (boxId == 14)
        gPokemonStorageSystemPtr->tempMon = gPlayerParty[position];
    else
        BoxMonToMon(gPokemonStorage.boxes[boxId] + position, &gPokemonStorageSystemPtr->tempMon);
    SetPlacedMonData(boxId, position);
    gPokemonStorageSystemPtr->movingMon = gPokemonStorageSystemPtr->tempMon;
    SetDisplayMonData(&gPokemonStorageSystemPtr->movingMon, 0);
    sMovingMonOrigBoxId = boxId;
    sMovingMonOrigBoxPos = position;
}

bool8 TryStorePartyMonInBox(u8 boxId)
{
    s16 boxPosition = GetFirstFreeBoxSpot(boxId);
    if (boxPosition == -1)
        return FALSE;
    if (sIsMonBeingMoved)
    {
        SetPlacedMonData(boxId, boxPosition);
        DestroyMovingMonIcon();
        sIsMonBeingMoved = FALSE;
    }
    else
    {
        SetMovingMonData(14, sCursorPosition);
        SetPlacedMonData(boxId, boxPosition);
        DestroyPartyMonIcon(sCursorPosition);
    }
    if (boxId == StorageGetCurrentBox())
        CreateBoxMonIconAtPos(boxPosition);
    StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 1);
    return TRUE;
}

void ResetSelectionAfterDeposit(void)
{
    StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 0);
    TryRefreshDisplayMon();
}

void InitReleaseMon(void)
{
    u8 mode;

    if (sIsMonBeingMoved)
        mode = 2;
    else if (sCursorArea == 1)
        mode = 0;
    else
        mode = 1;
    SetReleaseMon(mode, sCursorPosition);
    StringCopy(gPokemonStorageSystemPtr->releaseMonName, gPokemonStorageSystemPtr->displayMonName);
}

bool8 TryHideReleaseMon(void)
{
    if (!TryHideReleaseMonSprite())
    {
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 0);
        return FALSE;
    }
    return TRUE;
}

void ReleaseMon(void)
{
    DestroyReleaseMonIcon();
    if (sIsMonBeingMoved)
        sIsMonBeingMoved = FALSE;
    else
    {
        u8 boxId;
        if (sCursorArea == 1)
            boxId = 14;
        else
            boxId = StorageGetCurrentBox();
        PurgeMonOrBoxMon(boxId, sCursorPosition);
    }
    TryRefreshDisplayMon();
}

void TrySetCursorFistAnim(void)
{
    if (sIsMonBeingMoved)
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 3);
}

void InitCanReleaseMonVars(void)
{
    u16 knownMoves;
    if (sIsMonBeingMoved)
    {
        gPokemonStorageSystemPtr->tempMon = gPokemonStorageSystemPtr->movingMon;
        gPokemonStorageSystemPtr->releaseBoxId = -1;
        gPokemonStorageSystemPtr->releaseBoxPos = -1;
    }
    else
    {
        if (sCursorArea == 1)
        {
            gPokemonStorageSystemPtr->tempMon = gPlayerParty[sCursorPosition];
            gPokemonStorageSystemPtr->releaseBoxId = 14;
        }
        else
        {
            BoxMonToMon(gPokemonStorage.boxes[gPokemonStorage.currentBox] + sCursorPosition, &gPokemonStorageSystemPtr->tempMon);
            gPokemonStorageSystemPtr->releaseBoxId = gPokemonStorage.currentBox;
        }
        gPokemonStorageSystemPtr->releaseBoxPos = sCursorPosition;
    }
    gPokemonStorageSystemPtr->unk_267e = 0;
    gPokemonStorageSystemPtr->unk_267f = 0;
    gPokemonStorageSystemPtr->restrictedMoveList[0] = MOVE_SURF;
    gPokemonStorageSystemPtr->restrictedMoveList[1] = MOVE_DIVE;
    gPokemonStorageSystemPtr->restrictedMoveList[2] = MOVES_COUNT;
    knownMoves = GetMonData(&gPokemonStorageSystemPtr->tempMon, MON_DATA_KNOWN_MOVES, gPokemonStorageSystemPtr->restrictedMoveList);
    gPokemonStorageSystemPtr->unk_267e = knownMoves & 1;
    gPokemonStorageSystemPtr->unk_267f = (knownMoves >> 1) & 1;
    if (gPokemonStorageSystemPtr->unk_267e || gPokemonStorageSystemPtr->unk_267f)
        gPokemonStorageSystemPtr->releaseStatusResolved = 0;
    else
    {
        gPokemonStorageSystemPtr->releaseStatusResolved = 1;
        gPokemonStorageSystemPtr->canReleaseMon = 1;
    }
    gPokemonStorageSystemPtr->releaseCheckState = 0;
}

s8 RunCanReleaseMon(void)
{
    u16 i;
    u16 knownMoves;

    if (gPokemonStorageSystemPtr->releaseStatusResolved)
        return gPokemonStorageSystemPtr->canReleaseMon;
    switch (gPokemonStorageSystemPtr->releaseCheckState)
    {
        case 0:
            for (i = 0; i < PARTY_SIZE; i++)
            {
                if (gPokemonStorageSystemPtr->releaseBoxId != 14 || gPokemonStorageSystemPtr->releaseBoxPos != i)
                {
                    knownMoves = GetMonData(gPlayerParty + i, MON_DATA_KNOWN_MOVES, gPokemonStorageSystemPtr->restrictedMoveList);
                    if (knownMoves & 1)
                        gPokemonStorageSystemPtr->unk_267e = 0;
                    if (knownMoves & 2)
                        gPokemonStorageSystemPtr->unk_267f = 0;
                }
            }
            if (gPokemonStorageSystemPtr->unk_267e == 0 && gPokemonStorageSystemPtr->unk_267f == 0)
            {
                gPokemonStorageSystemPtr->releaseStatusResolved = 1;
                gPokemonStorageSystemPtr->canReleaseMon = 1;
            }
            else
            {
                gPokemonStorageSystemPtr->releaseCheckBoxId = 0;
                gPokemonStorageSystemPtr->releaseCheckBoxPos = 0;
                gPokemonStorageSystemPtr->releaseCheckState++;
            }
            break;
        case 1:
            for (i = 0; i < 5; i++)
            {
                knownMoves = GetBoxMonData(gPokemonStorage.boxes[gPokemonStorageSystemPtr->releaseCheckBoxId] + gPokemonStorageSystemPtr->releaseCheckBoxPos, MON_DATA_KNOWN_MOVES, gPokemonStorageSystemPtr->restrictedMoveList);
                if (knownMoves && (gPokemonStorageSystemPtr->releaseBoxId != gPokemonStorageSystemPtr->releaseCheckBoxId || gPokemonStorageSystemPtr->releaseBoxPos != gPokemonStorageSystemPtr->releaseCheckBoxPos))
                {
                    if (knownMoves & 1)
                        gPokemonStorageSystemPtr->unk_267e = 0;
                    if (knownMoves & 2)
                        gPokemonStorageSystemPtr->unk_267f = 0;
                }
                if (++gPokemonStorageSystemPtr->releaseCheckBoxPos >= 30)
                {
                    gPokemonStorageSystemPtr->releaseCheckBoxPos = 0;
                    if (++gPokemonStorageSystemPtr->releaseCheckBoxId >= 14)
                    {
                        gPokemonStorageSystemPtr->releaseStatusResolved = 1;
                        gPokemonStorageSystemPtr->canReleaseMon = 0;
                        break;
                    }
                }
            }
            if (gPokemonStorageSystemPtr->unk_267e == 0 && gPokemonStorageSystemPtr->unk_267f == 0)
            {
                gPokemonStorageSystemPtr->releaseStatusResolved = 1;
                gPokemonStorageSystemPtr->canReleaseMon = 1;
            }
            break;
    }
    return -1;
}

void SaveMovingMon(void)
{
    if (sIsMonBeingMoved)
        sSavedMovingMon = gPokemonStorageSystemPtr->movingMon;
}

void LoadSavedMovingMon(void)
{
    if (sIsMonBeingMoved)
    {
        if (sMovingMonOrigBoxId == 14)
            gPokemonStorageSystemPtr->movingMon = sSavedMovingMon;
        else
            gPokemonStorageSystemPtr->movingMon.box = sSavedMovingMon.box;
    }
}

void InitSummaryScreenData(void)
{
    if (sIsMonBeingMoved)
    {
        SaveMovingMon();
        gPokemonStorageSystemPtr->summaryMon.mon = &sSavedMovingMon;
        gPokemonStorageSystemPtr->summaryStartPos = 0;
        gPokemonStorageSystemPtr->summaryMaxPos = 0;
        gPokemonStorageSystemPtr->summaryScreenMode = 0;
    }
    else if (sCursorArea == 1)
    {
        gPokemonStorageSystemPtr->summaryMon.mon = gPlayerParty;
        gPokemonStorageSystemPtr->summaryStartPos = sCursorPosition;
        gPokemonStorageSystemPtr->summaryMaxPos = CountPartyMons() - 1;
        gPokemonStorageSystemPtr->summaryScreenMode = 0;
    }
    else
    {
        gPokemonStorageSystemPtr->summaryMon.box = gPokemonStorage.boxes[gPokemonStorage.currentBox];
        gPokemonStorageSystemPtr->summaryStartPos = sCursorPosition;
        gPokemonStorageSystemPtr->summaryMaxPos = 30 - 1;
        gPokemonStorageSystemPtr->summaryScreenMode = 5;
    }
}

void SetSelectionAfterSummaryScreen(void)
{
    if (sIsMonBeingMoved)
        LoadSavedMovingMon();
    else
        sCursorPosition = pssData.curMonIndex;
}

s16 CompactPartySlots(void)
{
    s16 retVal = -1;
    u16 i;
    u16 last;

    for (i = 0, last = 0; i < PARTY_SIZE; i++)
    {
        if ((u16)GetMonData(gPlayerParty + i, MON_DATA_SPECIES) != SPECIES_NONE)
        {
            if (i != last)
                gPlayerParty[last] = gPlayerParty[i];
            last++;
        }
        else if (retVal == -1)
            retVal = i;
    }
    for (; last < PARTY_SIZE; last++)
        ZeroMonData(gPlayerParty + last);
    return retVal;
}

void sub_809BDD8(u8 markings)
{
    gPokemonStorageSystemPtr->displayMonMarkings = markings;
    if (sIsMonBeingMoved)
        SetMonData(&gPokemonStorageSystemPtr->movingMon, MON_DATA_MARKINGS, &markings);
    else
    {
        if (sCursorArea == 1)
            SetMonData(gPlayerParty + sCursorPosition, MON_DATA_MARKINGS, &markings);
        if (sCursorArea == 0)
            SetBoxMonData(gPokemonStorage.boxes[StorageGetCurrentBox()] + sCursorPosition, MON_DATA_MARKINGS, &markings);
    }
}

bool8 IsRemovingLastPartyMon(void)
{
    if (sCursorArea == 1 && !sIsMonBeingMoved && CountPartyAliveNonEggMonsExcept(sCursorPosition) == 0)
        return TRUE;
    return FALSE;
}

bool8 CanShiftMon(void)
{
    if (sIsMonBeingMoved)
    {
        if (sCursorArea == 1 && CountPartyAliveNonEggMonsExcept(sCursorPosition) == 0)
        {
            if (gPokemonStorageSystemPtr->displayMonIsEgg || GetMonData(&gPokemonStorageSystemPtr->movingMon, MON_DATA_HP) == 0)
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

bool8 IsMonBeingMoved(void)
{
    return sIsMonBeingMoved;
}

static bool8 IsCursorOnBoxTitle(void)
{
    return sCursorArea == 2 ? TRUE : FALSE;
}

bool8 IsCursorOnCloseBox(void)
{
    return (sCursorArea == 3 && sCursorPosition == 1) ? TRUE : FALSE;
}

static void TryRefreshDisplayMon(void)
{
    gPokemonStorageSystemPtr->setMosaic = sIsMonBeingMoved ? 0 : 1;
    if (!sIsMonBeingMoved)
    {
        switch (sCursorArea)
        {
            case 1:
                if (sCursorPosition < PARTY_SIZE)
                {
                    SetDisplayMonData(gPlayerParty + sCursorPosition, 0);
                    break;
                }
                // fallthrough
            case 2:
            case 3:
                SetDisplayMonData(NULL, 2);
                break;
            case 0:
                SetDisplayMonData(gPokemonStorage.boxes[StorageGetCurrentBox()] + sCursorPosition, 1);
                break;
        }
    }
}

static void ReshowDisplayMon(void)
{
    if (sIsMonBeingMoved)
        SetDisplayMonData(&sSavedMovingMon.box, 0);
    else
        TryRefreshDisplayMon();
}

static void SetDisplayMonData(void *pokemon, u8 mode)
{
    u8 *txtPtr;
    u16 gender = MON_MALE;
    gPokemonStorageSystemPtr->displayMonItemId = 0;
    if (mode == 0)
    {
        struct Pokemon *mon = (struct Pokemon *)pokemon;
        gPokemonStorageSystemPtr->displayMonSpecies = GetMonData(pokemon, MON_DATA_SPECIES_OR_EGG);
        if (gPokemonStorageSystemPtr->displayMonSpecies != SPECIES_NONE)
        {
            gPokemonStorageSystemPtr->displayMonIsEgg = GetMonData(mon, MON_DATA_IS_EGG);
            GetMonData(mon, MON_DATA_NICKNAME, gPokemonStorageSystemPtr->displayMonName);
            StringGet_Nickname(gPokemonStorageSystemPtr->displayMonName);
            gPokemonStorageSystemPtr->displayMonLevel = GetMonData(mon, MON_DATA_LEVEL);
            gPokemonStorageSystemPtr->displayMonMarkings = GetMonData(mon, MON_DATA_MARKINGS);
            gPokemonStorageSystemPtr->displayMonPersonality = GetMonData(mon, MON_DATA_PERSONALITY);
            gPokemonStorageSystemPtr->displayMonPalette = GetMonFrontSpritePal(mon);
            gender = GetMonGender(mon);
            gPokemonStorageSystemPtr->displayMonItemId = GetMonData(mon, MON_DATA_HELD_ITEM);
        }
    }
    else if (mode == 1)
    {
        struct BoxPokemon *boxMon = (struct BoxPokemon *)pokemon;
        gPokemonStorageSystemPtr->displayMonSpecies = GetBoxMonData(pokemon, MON_DATA_SPECIES_OR_EGG);
        if (gPokemonStorageSystemPtr->displayMonSpecies != SPECIES_NONE)
        {
            u32 otId = GetBoxMonData(boxMon, MON_DATA_OT_ID);
            gPokemonStorageSystemPtr->displayMonIsEgg = GetBoxMonData(boxMon, MON_DATA_IS_EGG);
            GetBoxMonData(boxMon, MON_DATA_NICKNAME, gPokemonStorageSystemPtr->displayMonName);
            StringGet_Nickname(gPokemonStorageSystemPtr->displayMonName);
            gPokemonStorageSystemPtr->displayMonLevel = GetLevelFromBoxMonExp(boxMon);
            gPokemonStorageSystemPtr->displayMonMarkings = GetBoxMonData(boxMon, MON_DATA_MARKINGS);
            gPokemonStorageSystemPtr->displayMonPersonality = GetBoxMonData(boxMon, MON_DATA_PERSONALITY);
            gPokemonStorageSystemPtr->displayMonPalette = GetMonSpritePalFromSpeciesAndPersonality(gPokemonStorageSystemPtr->displayMonSpecies, otId, gPokemonStorageSystemPtr->displayMonPersonality);
            gender = GetGenderFromSpeciesAndPersonality(gPokemonStorageSystemPtr->displayMonSpecies, gPokemonStorageSystemPtr->displayMonPersonality);
            gPokemonStorageSystemPtr->displayMonItemId = GetBoxMonData(boxMon, MON_DATA_HELD_ITEM);
        }
    }
    else
    {
        gPokemonStorageSystemPtr->displayMonSpecies = SPECIES_NONE;
    }
    if (gPokemonStorageSystemPtr->displayMonSpecies == SPECIES_NONE)
    {
        gPokemonStorageSystemPtr->displayMonName[0] = EOS;
        gPokemonStorageSystemPtr->displayMonNameText[0] = EOS;
        gPokemonStorageSystemPtr->displayMonSpeciesName[0] = EOS;
        gPokemonStorageSystemPtr->displayMonGenderLvlText[0] = EOS;
        gPokemonStorageSystemPtr->displayMonItemName[0] = EOS;
    }
    else if (gPokemonStorageSystemPtr->displayMonIsEgg)
    {
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = 0x04; // COLOR_HIGHLIGHT_SHADOW
        txtPtr[2] = 0x0F; // WHITE2
        txtPtr[3] = 0x00; // TRANSPARENT
        txtPtr[4] = 0x01; // DARK_GREY
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText + 5;
        txtPtr = StringCopy(txtPtr, gPokemonStorageSystemPtr->displayMonName);
        gPokemonStorageSystemPtr->displayMonSpeciesName[0] = EOS;
        gPokemonStorageSystemPtr->displayMonGenderLvlText[0] = EOS;
        gPokemonStorageSystemPtr->displayMonItemName[0] = EOS;
    }
    else
    {
        if (gPokemonStorageSystemPtr->displayMonSpecies == SPECIES_NIDORAN_M || gPokemonStorageSystemPtr->displayMonSpecies == SPECIES_NIDORAN_F)
            gender = MON_GENDERLESS;
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = 0x04; // COLOR_HIGHLIGHT_SHADOW
        txtPtr[2] = 0x0F; // WHITE2
        txtPtr[3] = 0x00; // TRANSPARENT
        txtPtr[4] = 0x01; // DARK_GREY
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText + 5;
        txtPtr = StringCopy(txtPtr, gPokemonStorageSystemPtr->displayMonName);
        txtPtr = gPokemonStorageSystemPtr->displayMonSpeciesName;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = 0x04; // COLOR_HIGHLIGHT_SHADOW
        txtPtr[2] = 0x0F; // WHITE2
        txtPtr[3] = 0x00; // TRANSPARENT
        txtPtr[4] = 0x01; // DARK_GREY
        txtPtr[5] = EXT_CTRL_CODE_BEGIN;
        txtPtr[6] = 0x13; // CLEAR_TO
        txtPtr[7] = 7;
        txtPtr[8] = CHAR_SLASH;
        txtPtr = gPokemonStorageSystemPtr->displayMonSpeciesName + 9;
        txtPtr = StringCopy(txtPtr, gSpeciesNames[gPokemonStorageSystemPtr->displayMonSpecies]);
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = 0x13; // CLEAR_TO
        txtPtr[2] = 0x50;
        txtPtr[3] = EOS;
        txtPtr = gPokemonStorageSystemPtr->displayMonGenderLvlText;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = 0x04; // COLOR_HIGHLIGHT_SHADOW
        txtPtr[2] = 0x0F; // WHITE2
        txtPtr[3] = 0x00; // TRANSPARENT
        txtPtr[4] = 0x01; // DARK_GREY
        txtPtr[5] = EXT_CTRL_CODE_BEGIN;
        txtPtr[6] = 0x13; // CLEAR_TO
        txtPtr[7] = 8;
        txtPtr = gPokemonStorageSystemPtr->displayMonGenderLvlText + 8;
        txtPtr[0] = 0x34; // LV
        txtPtr = gPokemonStorageSystemPtr->displayMonGenderLvlText + 9;
        txtPtr = AlignInt1InMenuWindow(txtPtr, gPokemonStorageSystemPtr->displayMonLevel, 0x22, STR_CONV_MODE_RIGHT_ALIGN);
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = 0x11; // CLEAR
        txtPtr[2] = 8;
        txtPtr += 3;
        switch (gender)
        {
            case MON_MALE:
                txtPtr[0] = EXT_CTRL_CODE_BEGIN;
                txtPtr[1] = 0x04; // COLOR_HIGHLIGHT_SHADOW
                txtPtr[2] = 0x04; // BLUE
                txtPtr[3] = 0x00; // TRANSPARENT
                txtPtr[4] = 0x05; // YELLOW
                txtPtr[5] = CHAR_MALE;
                txtPtr += 6;
                break;
            case MON_FEMALE:
                txtPtr[0] = EXT_CTRL_CODE_BEGIN;
                txtPtr[1] = 0x04; // COLOR_HIGHLIGHT_SHADOW
                txtPtr[2] = 0x06; // CYAN
                txtPtr[3] = 0x00; // TRANSPARENT
                txtPtr[4] = 0x07; // MAGENTA
                txtPtr[5] = CHAR_FEMALE;
                txtPtr += 6;
                break;
        }
        txtPtr[0] = EOS;
        txtPtr = gPokemonStorageSystemPtr->displayMonItemName;
        if (gPokemonStorageSystemPtr->displayMonItemId)
        {
            txtPtr[0] = EXT_CTRL_CODE_BEGIN;
            txtPtr[1] = 0x04; // COLOR_HIGHLIGHT_SHADOW
            txtPtr[2] = 0x0F; // WHITE2
            txtPtr[3] = 0x00; // TRANSPARENT
            txtPtr[4] = 0x01; // DARK_GREY
            txtPtr = gPokemonStorageSystemPtr->displayMonItemName + 5;
            txtPtr[0] = EXT_CTRL_CODE_BEGIN;
            txtPtr[1] = 0x06; // size
            txtPtr[2] = 0x04;
            txtPtr = gPokemonStorageSystemPtr->displayMonItemName + 8;
            txtPtr = StringCopy(txtPtr, GetItemName(gPokemonStorageSystemPtr->displayMonItemId));
            txtPtr[0] = EXT_CTRL_CODE_BEGIN;
            txtPtr[1] = 0x07; // UNKNOWN_7;
            txtPtr += 2;
        }
        txtPtr[0] = EOS;
    }
}

u8 InBoxInput_Normal(void)
{
    u8 retVal;
    s8 cursorArea = sCursorArea;
    s8 cursorPosition = sCursorPosition;

    do
    {
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;

        if (JOY_REPEAT(DPAD_UP))
        {
            retVal = 1;
            if (sCursorPosition >= 6)
            {
                cursorPosition -= 6;
            }
            else
            {
                cursorArea = 2;
                cursorPosition = 0;
            }
            break;
        }
        if (JOY_REPEAT(DPAD_DOWN))
        {
            retVal = 1;
            cursorPosition += 6;
            if (cursorPosition >= 30)
            {
                cursorArea = 3;
                cursorPosition -= 30;
                cursorPosition /= 3;
                gPokemonStorageSystemPtr->cursorVerticalWrap = 1;
                gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            }
            break;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            retVal = 1;
            if (sCursorPosition % 6)
                cursorPosition--;
            else
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = -1;
                cursorPosition += 5;
            }
            break;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            retVal = 1;
            if ((sCursorPosition + 1) % 6)
                cursorPosition++;
            else
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = 1;
                cursorPosition -= 5;
            }
            break;
        }
        if (JOY_NEW(START_BUTTON))
        {
            retVal = 1;
            cursorArea = 2;
            cursorPosition = 0;
            break;
        }
        if ((JOY_NEW(A_BUTTON)) && SetMenuTexts_Mon())
        {
            if (sAutoActionOn == 0)
                return 8;
            switch (GetMenuItemTextId(0))
            {
            case 1:
                return 11;
            case 2:
                return 12;
            case 3:
                return 13;
            case 4:
                return 14;
            case 5:
                return 15;
            }
        }
        if (JOY_NEW(B_BUTTON))
            return 16;
        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR)
        {
            if (JOY_HELD(L_BUTTON))
                return 10;
            if (JOY_HELD(R_BUTTON))
                return 9;
        }
        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return 0;
        }
        retVal = 0;
    } while (0);
    if (retVal)
        SetCursorPosition(cursorArea, cursorPosition);
    return retVal;
}

u8 HandleInput_InParty(void)
{
    u8 retVal;
    bool8 gotoBox;
    s8 cursorArea;
    s8 cursorPosition;

    do
    {
        cursorArea = sCursorArea;
        cursorPosition = sCursorPosition;
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;
        gotoBox = FALSE;
        retVal = 0;

        if (JOY_REPEAT(DPAD_UP))
        {
            if (--cursorPosition < 0)
                cursorPosition = 6;
            if (cursorPosition != sCursorPosition)
                retVal = 1;
            break;
        }

        if (JOY_REPEAT(DPAD_DOWN))
        {
            if (++cursorPosition > 6)
                cursorPosition = 0;
            if (cursorPosition != sCursorPosition)
                retVal = 1;
            break;
        }

        if (JOY_REPEAT(DPAD_LEFT) && sCursorPosition != 0)
        {
            retVal = 1;
            gPokemonStorageSystemPtr->cursorPrevHorizPos = sCursorPosition;
            cursorPosition = 0;
            break;
        }

        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if (sCursorPosition == 0)
            {
                retVal = 1;
                cursorPosition = gPokemonStorageSystemPtr->cursorPrevHorizPos;
            }
            else
            {
                retVal = 6;
                cursorArea = 0;
                cursorPosition = 0;
            }
            break;
        }

        if (JOY_NEW(A_BUTTON))
        {
            if (sCursorPosition == 6)
            {
                if (gPokemonStorageSystemPtr->boxOption == 1)
                    return 4;

                gotoBox = TRUE;
            }
            else if (SetMenuTexts_Mon())
            {
                if (!sAutoActionOn)
                    return 8;

                switch (GetMenuItemTextId(0))
                {
                case 1:
                    return 11;
                case 2:
                    return 12;
                case 3:
                    return 13;
                case 4:
                    return 14;
                case 5:
                    return 15;
                }
            }
        }

        if (JOY_NEW(B_BUTTON))
        {
            if (gPokemonStorageSystemPtr->boxOption == 1)
                return 16;

            gotoBox = TRUE;
        }

        if (gotoBox)
        {
            retVal = 6;
            cursorArea = 0;
            cursorPosition = 0;
            break;
        }

        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return 0;
        }
    } while (0);

    if (retVal != 0)
    {
        SetCursorPosition(cursorArea, cursorPosition);
    }

    return retVal;
}

u8 HandleInput_OnBox(void)
{
    u8 retVal;
    s8 cursorArea;
    s8 cursorPosition;

    do
    {
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;

        if (JOY_REPEAT(DPAD_UP))
        {
            retVal = 1;
            cursorArea = 3;
            cursorPosition = 0;
            gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            break;
        }
        else if (JOY_REPEAT(DPAD_DOWN))
        {
            retVal = 1;
            cursorArea = 0;
            cursorPosition = 2;
            break;
        }

        if (JOY_HELD(DPAD_LEFT))
            return 10;
        if (JOY_HELD(DPAD_RIGHT))
            return 9;

        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR)
        {
            if (JOY_HELD(L_BUTTON))
                return 10;
            if (JOY_HELD(R_BUTTON))
                return 9;
        }

        if (JOY_NEW(A_BUTTON))
        {
            AnimateBoxScrollArrows(FALSE);
            AddBoxOptionsMenu();
            return 7;
        }

        if (JOY_NEW(B_BUTTON))
            return 16;

        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return 0;
        }

        retVal = 0;
    } while (0);

    if (retVal != 0)
    {
        if (cursorArea != 2)
            AnimateBoxScrollArrows(FALSE);
        SetCursorPosition(cursorArea, cursorPosition);
    }

    return retVal;
}

u8 HandleInput_OnButtons(void)
{
    u8 retVal;
    s8 cursorArea;
    s8 cursorPosition;

    do
    {
        cursorArea = sCursorArea;
        cursorPosition = sCursorPosition;
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;

        if (JOY_REPEAT(DPAD_UP))
        {
            retVal = 1;
            cursorArea = 0;
            gPokemonStorageSystemPtr->cursorVerticalWrap = -1;
            if (sCursorPosition == 0)
                cursorPosition = 24;
            else
                cursorPosition = 29;
            gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            break;
        }

        if (JOY_REPEAT(DPAD_DOWN | START_BUTTON))
        {
            retVal = 1;
            cursorArea = 2;
            cursorPosition = 0;
            gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            break;
        }

        if (JOY_REPEAT(DPAD_LEFT))
        {
            retVal = 1;
            if (--cursorPosition < 0)
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = -1;
                cursorPosition = 1;
            }
            break;
        }

        if (JOY_REPEAT(DPAD_RIGHT))
        {
            retVal = 1;
            if (++cursorPosition > 1)
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = 1;
                cursorPosition = 0;
            }
            break;
        }

        if (JOY_NEW(A_BUTTON))
            return (cursorPosition == 0) ? 5 : 4;
        if (JOY_NEW(B_BUTTON))
            return 16;

        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return 0;
        }

        retVal = 0;
    } while (0);

    if (retVal != 0)
    {
        SetCursorPosition(cursorArea, cursorPosition);
    }

    return retVal;
}

u8 HandleInput(void)
{
    u16 i = 0;
    while (gInputFuncs[i].func != NULL)
    {
        if (gInputFuncs[i].area == sCursorArea)
            return gInputFuncs[i].func();
        i++;
    }

    return 0;
}

static void AddBoxOptionsMenu(void)
{
    sub_809CDCC();
    SetMenuText(9);
    SetMenuText(10);
    SetMenuText(11);
    SetMenuText(0);
}

#if DEBUG
u8 debug_sub_80AA40C(void)
{
    if (!GetSpeciesAtCursorPosition())
        return 0;

    sub_809CDCC();
    SetMenuText(32);
    SetMenuText(6);
    return 1;
}
#endif

static bool8 SetMenuTexts_Mon(void)
{
    u16 species;

#if DEBUG
    if (unk_2038790)
        return debug_sub_80AA40C();
#endif

    sub_809CDCC();
    species = GetSpeciesAtCursorPosition();

    switch (gPokemonStorageSystemPtr->boxOption)
    {
    case 1:
        if (species)
            SetMenuText(1);
        else
            return 0;
        break;
    case 0:
        if (species)
            SetMenuText(2);
        else
            return 0;
        break;
    case 2:
        if (sIsMonBeingMoved)
        {
            if (species)
                SetMenuText(4);
            else
                SetMenuText(5);
        }
        else
        {
            if (species)
                SetMenuText(3);
            else
                return 0;
        }
        break;
    default:
        return 0;
    }

    SetMenuText(6);
    if (gPokemonStorageSystemPtr->boxOption == 2)
    {
        if (!sCursorArea)
            SetMenuText(2);
        else
            SetMenuText(1);
    }

    SetMenuText(8);
    SetMenuText(7);
    SetMenuText(0);
    return 1;
}

void SpriteCB_CursorShadow(struct Sprite *sprite)
{
    sprite->x = gPokemonStorageSystemPtr->cursorSprite->x;
    sprite->y = gPokemonStorageSystemPtr->cursorSprite->y + 20;
}

void sub_809CB94(struct Pokemon *mon)
{
    if (sCursorArea == 1)
    {
        mon->box = gPlayerParty[sCursorPosition].box;
    }
    else
    {
        u8 boxId = StorageGetCurrentBox();
        mon->box = gPokemonStorage.boxes[boxId][sCursorPosition];
    }
}

static void CreateCursorSprites(void)
{
    u16 x, y;
    u8 spriteId;
    int priority, subpriority;
    struct SpriteSheet spriteSheets[3];
    struct SpritePalette spritePalettes[3];

    memcpy(spriteSheets, gHandCursorSpriteSheets, 24);
    memcpy(spritePalettes, gHandCursorSpritePalettes, 24);
    LoadSpriteSheets(spriteSheets);
    LoadSpritePalettes(spritePalettes);
    gPokemonStorageSystemPtr->cursorPalNums[0] = IndexOfSpritePaletteTag(0xDAC6);
    gPokemonStorageSystemPtr->cursorPalNums[1] = IndexOfSpritePaletteTag(0xDAD1);

    GetCursorCoordsByPos(sCursorArea, sCursorPosition, &x, &y);
    spriteId = CreateSprite(&gSpriteTemplate_Cursor, x, y, 6);
    if (spriteId != MAX_SPRITES)
    {
        gPokemonStorageSystemPtr->cursorSprite = &gSprites[spriteId];
        gPokemonStorageSystemPtr->cursorSprite->oam.paletteNum = gPokemonStorageSystemPtr->cursorPalNums[sAutoActionOn];
        if (sIsMonBeingMoved)
            StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, 3);
    }
    else
    {
        gPokemonStorageSystemPtr->cursorSprite = NULL;
    }

    if (sCursorArea == 1)
    {
        subpriority = 12;
        priority = 1;
    }
    else
    {
        subpriority = 20;
        priority = 2;
    }

    spriteId = CreateSprite(&gSpriteTemplate_CursorShadow, 0, 0, subpriority);
    if (spriteId != MAX_SPRITES)
    {
        gPokemonStorageSystemPtr->cursorShadowSprite = &gSprites[spriteId];
        gPokemonStorageSystemPtr->cursorShadowSprite->oam.priority = priority;
        if (sCursorArea)
            gPokemonStorageSystemPtr->cursorShadowSprite->invisible = TRUE;
    }
    else
    {
        gPokemonStorageSystemPtr->cursorShadowSprite = NULL;
    }
}

static void ToggleCursorAutoAction(void)
{
    sAutoActionOn = !sAutoActionOn;
    gPokemonStorageSystemPtr->cursorSprite->oam.paletteNum = gPokemonStorageSystemPtr->cursorPalNums[sAutoActionOn];
}

void sub_809CDCC(void)
{
    gPokemonStorageSystemPtr->menuItemsCount = 0;
    gPokemonStorageSystemPtr->menuWidth = 0;
}

void SetMenuText(u8 textId)
{
    u8 width;
    u8 var0;

    if (gPokemonStorageSystemPtr->menuItemsCount < 7)
    {
        struct StorageMenu *menu = &gPokemonStorageSystemPtr->menuItems[gPokemonStorageSystemPtr->menuItemsCount];
        menu->text = gMenuTexts[textId];
        menu->textId = textId;
        width = GetStringWidthInMenuWindow(menu->text);
        var0 = (width + 7) / 8;
        if (var0 > gPokemonStorageSystemPtr->menuWidth)
            gPokemonStorageSystemPtr->menuWidth = var0;

        gPokemonStorageSystemPtr->menuItemsCount++;
    }
}

static s8 GetMenuItemTextId(u8 menuIdx)
{
    if (menuIdx >= gPokemonStorageSystemPtr->menuItemsCount)
        return -1;
    else
        return gPokemonStorageSystemPtr->menuItems[menuIdx].textId;
}

void AddMenu(void)
{
    gPokemonStorageSystemPtr->unk_11bc = 14 - gPokemonStorageSystemPtr->menuItemsCount * 2;
    gPokemonStorageSystemPtr->unk_11ba = 28 - gPokemonStorageSystemPtr->menuWidth;
    Menu_DrawStdWindowFrame(gPokemonStorageSystemPtr->unk_11ba, gPokemonStorageSystemPtr->unk_11bc, 29, 15);
    Menu_PrintItems(
        gPokemonStorageSystemPtr->unk_11ba + 1,
        gPokemonStorageSystemPtr->unk_11bc + 1,
        gPokemonStorageSystemPtr->menuItemsCount,
        gPokemonStorageSystemPtr->menuItems);
    InitMenu(
        0,
        gPokemonStorageSystemPtr->unk_11ba + 1,
        gPokemonStorageSystemPtr->unk_11bc + 1,
        gPokemonStorageSystemPtr->menuItemsCount,
        0,
        gPokemonStorageSystemPtr->menuWidth);
}

s16 HandleMenuInput(void)
{
    s32 input = -2;

    do
    {
        if (!(JOY_NEW(A_BUTTON)))
        {
            if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                input++;
            }

            if (JOY_NEW(DPAD_UP))
            {
                PlaySE(SE_SELECT);
                Menu_MoveCursor(-1);
            }
            else if (JOY_NEW(DPAD_DOWN))
            {
                PlaySE(SE_SELECT);
                Menu_MoveCursor(1);
            }
        }
        else
        {
            input = Menu_GetCursorPos();
        }

        if (input != -2)
        {
            Menu_DestroyCursor();
            Menu_EraseWindowRect(
                gPokemonStorageSystemPtr->unk_11ba,
                gPokemonStorageSystemPtr->unk_11bc,
                29,
                15);
        }
    } while (0);

    if (input >= 0)
    {
        input = gPokemonStorageSystemPtr->menuItems[input].textId;
    }

    return input;
}

void UnkUtil_Init(struct UnkUtil *util, struct UnkUtilData *data, u8 max)
{
    sUnkUtil = util;
    util->data = data;
    util->max = max;
    util->numActive = 0;
}

void UnkUtil_Run(void)
{
    u16 i;
    struct UnkUtilData *data;

    if (sUnkUtil->numActive)
    {
        for (i = 0; i < sUnkUtil->numActive; i++)
        {
            data = &sUnkUtil->data[i];
            data->func(data);
        }

        sUnkUtil->numActive = 0;
    }
}

bool8 sub_809D034(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height)
{
    struct UnkUtilData *data;

    if (sUnkUtil->numActive >= sUnkUtil->max)
        return FALSE;

    data = &sUnkUtil->data[sUnkUtil->numActive++];
    data->size = width * 2;
    data->dest = dest + (dTop * 64) + (dLeft * 2);
    data->src = src + (sTop * 64) + (sLeft * 2);
    data->height = height;
    data->func = sub_809D0BC;
    return TRUE;
}

void sub_809D0BC(struct UnkUtilData *data)
{
    u32 i;
    const u8 *src = data->src;
    u8 *dest = data->dest;
    int height = data->height;
    int width = data->size;
    for (i = 0; i < height; i++)
    {
        CpuCopy16(src, dest, width);
        dest += 64;
        src += 64;
    }
}

void sub_809D104(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height)
{
    const u8 *from = src + (sTop * 64) + (sLeft * 2);
    u8 *to = dest + (dTop * 64) + (dLeft * 2);
    u8 *end = dest + (dTop + height) * 64;
    int width2 = width * 2;
    while (to < end)
    {
        CpuCopy16(from, to, width2);
        to += 64;
        from += 64;
    }
}

bool8 UnkUtil_DmaAdd(void *dest, u16 dLeft, u16 dTop, u16 width, u16 height)
{
    struct UnkUtilData *data;

    if (sUnkUtil->numActive >= sUnkUtil->max)
        return FALSE;

    data = &sUnkUtil->data[sUnkUtil->numActive++];
    data->size = width * 2;
    data->dest = dest + ((dTop * 32) + dLeft) * 2;
    data->height = height;
    data->func = UnkUtil_DmaRun;
    return TRUE;
}

static void UnkUtil_DmaRun(struct UnkUtilData *data)
{
    u16 i;

    for (i = 0; i < data->height; i++)
    {
        Dma3FillLarge_(0, data->dest, data->size, 16);
        data->dest += 64;
    }
}
