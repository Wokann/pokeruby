#include "global.h"
#include "main.h"
#include "palette.h"
#include "sprite.h"
#include "pc_screen_effect.h"

static void HBlankCB_PCScreenBlend(void);
static void HBlankCB_PCScreenReveal(void);
static void SpriteCB_PCScreenMoveOutward(struct Sprite *);
static void SpriteCB_PCScreenMoveInward(struct Sprite *);
static void EnablePCScreenHBlank(IntrFunc);
static void DisablePCScreenHBlank(void);

struct OamData gPCScreenEffectOam = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 1
};

union AnimCmd gPCScreenEffectAnim[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd *gPCScreenEffectAnimTable[] = {
    gPCScreenEffectAnim
};

u16 gPCScreenEffectPal[] = INCBIN_U16("graphics/pc_screen_effect/palette.gbapal");
u8 gPCScreenEffectGfx[] = INCBIN_U8("graphics/pc_screen_effect/tiles.4bpp");

EWRAM_DATA struct PCScreenEffectStruct *gPCScreenEffect = NULL;

void StartPCScreenOpenEffect(struct PCScreenEffectStruct *effect)
{
    u16 i;

    struct SpriteSheet spriteSheet = { gPCScreenEffectGfx, sizeof(gPCScreenEffectGfx), 0 };
    struct SpritePalette spritePalette = { gPCScreenEffectPal, 0 };
    struct SpriteTemplate spriteTemplate =
        {
            0,
            0,
            &gPCScreenEffectOam,
            gPCScreenEffectAnimTable,
            NULL,
            gDummySpriteAffineAnimTable,
            SpriteCB_PCScreenMoveOutward,
        };

    spriteSheet.tag = effect->tileTag;
    spriteTemplate.tileTag =  effect->tileTag;
    spritePalette.tag = effect->paletteTag;
    spriteTemplate.paletteTag = effect->paletteTag;

    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&spritePalette);

    effect->revealRadius = 1;
    effect->spritesFinished = 0;
    effect->state = 0;
    effect->selectedPalettes  = ~(0x10000 << IndexOfSpritePaletteTag(effect->paletteTag)) & 0xFFFF0000;

    if (effect->spriteSpeed == 0)
        effect->spriteSpeed = 16;

    if (effect->revealSpeed == 0)
        effect->revealSpeed = 20;

    gPCScreenEffect = effect;

    for (i = 0; i < 8; i++)
    {
        u8 spriteId = CreateSprite(&spriteTemplate, 32 * i + 8, 80, 0);
        if (spriteId == MAX_SPRITES)
            break;
        gSprites[spriteId].data[0] = (i < 4) ? -effect->spriteSpeed : effect->spriteSpeed;
    }

    REG_BLDCNT = 191;
    REG_BLDY = 16;
}

bool8 UpdatePCScreenOpenEffect(void)
{
    if (gPCScreenEffect->state == 0)
    {
        BlendPalettes(gPCScreenEffect->selectedPalettes, 16, FADE_COLOR_WHITE);
        EnablePCScreenHBlank(HBlankCB_PCScreenBlend);
        gPCScreenEffect->state++;
    }

    if (gPCScreenEffect->spritesFinished < 8)
        return FALSE;

    gPCScreenEffect->revealRadius += gPCScreenEffect->revealSpeed;

    if (gPCScreenEffect->revealRadius >= 80)
    {
        gPCScreenEffect->revealRadius = 80;
        REG_BLDCNT = 0;
        REG_BLDY = 0;
        DisablePCScreenHBlank();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void StartPCScreenCloseEffect(struct PCScreenEffectStruct *effect)
{
    u16 i;
    u8 spriteId;

    struct SpriteSheet spriteSheet = { gPCScreenEffectGfx, sizeof(gPCScreenEffectGfx), 0 };
    struct SpritePalette spritePalette = { gPCScreenEffectPal, 0 };
    struct SpriteTemplate spriteTemplate =
        {
            0,
            0,
            &gPCScreenEffectOam,
            gPCScreenEffectAnimTable,
            NULL,
            gDummySpriteAffineAnimTable,
            SpriteCB_PCScreenMoveInward,
        };

    spriteSheet.tag = effect->tileTag;
    spriteTemplate.tileTag = effect->tileTag;
    spritePalette.tag = effect->paletteTag;
    spriteTemplate.paletteTag = effect->paletteTag;

    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&spritePalette);

    effect->revealRadius = 0x50;
    effect->state = 0;
    effect->spritesFinished = 0;
    effect->selectedPalettes = 0xffff0000 & ~(0x10000 << IndexOfSpritePaletteTag(effect->paletteTag));
    if (effect->spriteSpeed == 0)
        effect->spriteSpeed = 16;
    if (effect->revealSpeed == 0)
        effect->revealSpeed = 20;
    gPCScreenEffect = effect;

    for (i = 0; i < 8; i++)
    {
        if (i < 4)
        {
            spriteId = CreateSprite(&spriteTemplate, i * 32 - 0x70, 0x50, 0);
            if (spriteId == MAX_SPRITES)
                break;
            gSprites[spriteId].data[0] = effect->spriteSpeed;
            gSprites[spriteId].data[1] = 1;
        }
        else
        {
            // Fakematching
            spriteId = CreateSprite(&spriteTemplate, ((i << 21) + (0x80 << 16)) >> 16, 0x50, 0);
            if (spriteId == MAX_SPRITES)
                break;
            gSprites[spriteId].data[0] = -effect->spriteSpeed;
            gSprites[spriteId].data[1] = -1;
        }
        gSprites[spriteId].data[2] = i * 32 + 8;
        gSprites[spriteId].data[4] = 0;
        gSprites[spriteId].invisible = TRUE;
    }
    REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG3 | BLDCNT_TGT1_OBJ | BLDCNT_TGT1_BD | BLDCNT_EFFECT_DARKEN;
    REG_BLDY = 16;
    EnablePCScreenHBlank(HBlankCB_PCScreenReveal);
}

bool8 UpdatePCScreenCloseEffect(void)
{
    switch (gPCScreenEffect->state)
    {
        case 0:
            gPCScreenEffect->revealRadius -= gPCScreenEffect->revealSpeed;
            if (gPCScreenEffect->revealRadius < 2)
            {
                BlendPalettes(gPCScreenEffect->selectedPalettes, 16, FADE_COLOR_WHITE);
                SetHBlankCallback(HBlankCB_PCScreenBlend);
                gPCScreenEffect->revealRadius = 1;
                gPCScreenEffect->state++;
            }
            break;
        case 1:
            if (gPCScreenEffect->spritesFinished == 8)
            {
                BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
                gPCScreenEffect->state++;
            }
            break;
        case 2:
            REG_BLDCNT = 0;
            REG_BLDY = 0;
            FreeSpriteTilesByTag(gPCScreenEffect->tileTag);
            FreeSpritePaletteByTag(gPCScreenEffect->paletteTag);
            DisablePCScreenHBlank();
            gPCScreenEffect->state++;
            return TRUE;
        default:
            return TRUE;
    }
    return FALSE;
}

static void HBlankCB_PCScreenBlend(void)
{
    vu16 vcount = REG_VCOUNT & 0xFF;
    if (vcount == 0x50)
        REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_LIGHTEN;
    else
        REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG3 | BLDCNT_TGT1_OBJ | BLDCNT_TGT1_BD | BLDCNT_EFFECT_DARKEN;
}

static void HBlankCB_PCScreenReveal(void)
{
    vu16 vcount = REG_VCOUNT & 0xFF;
    if (vcount > 0x50 - gPCScreenEffect->revealRadius && vcount < 0x50 + gPCScreenEffect->revealRadius)
        REG_BLDY = 0;
    else
        REG_BLDY = 16;
}

static void SpriteCB_PCScreenMoveOutward(struct Sprite *sprite)
{
    sprite->x += sprite->data[0];
    if (sprite->x < -0x08 || sprite->x > 0xf8)
    {
        DestroySprite(sprite);
        gPCScreenEffect->spritesFinished++;
        if (gPCScreenEffect->spritesFinished == 8)
        {
            FreeSpriteTilesByTag(gPCScreenEffect->tileTag);
            FreeSpritePaletteByTag(gPCScreenEffect->paletteTag);
            BlendPalettes(gPCScreenEffect->selectedPalettes, 0, FADE_COLOR_WHITE);
            SetHBlankCallback(HBlankCB_PCScreenReveal);
        }
    }
}

static void SpriteCB_PCScreenMoveInward(struct Sprite *sprite)
{
    if (sprite->data[4] == 0 && gPCScreenEffect->revealRadius == 1)
    {
        sprite->x += sprite->data[0];
        if (sprite->x > -0x10 && sprite->x < 0x100)
            sprite->invisible = FALSE;
        if (sprite->data[1] > 0)
        {
            if (sprite->x >= sprite->data[2])
                sprite->data[4] = 1;
        }
        else
        {
            if (sprite->x <= sprite->data[2])
                sprite->data[4] = 1;
        }
        if (sprite->data[4])
        {
            gPCScreenEffect->spritesFinished++;
            sprite->x = sprite->data[2];
        }
    }
}

static void EnablePCScreenHBlank(IntrFunc cb)
{
    u16 imeBak;
    INTR_CHECK |= INTR_FLAG_HBLANK;
    REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;
    imeBak = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_HBLANK;
    REG_IME = imeBak;
    gMain.intrCheck |= INTR_FLAG_HBLANK;
    SetHBlankCallback(cb);
}

static void DisablePCScreenHBlank(void)
{
    u16 imeBak;
    INTR_CHECK &= ~INTR_FLAG_HBLANK;
    REG_DISPSTAT &= ~DISPSTAT_HBLANK_INTR;
    imeBak = REG_IME;
    REG_IME = 0;
    REG_IE &= ~INTR_FLAG_HBLANK;
    REG_IME = imeBak;
    gMain.intrCheck &= ~INTR_FLAG_HBLANK;
    SetHBlankCallback(NULL);
}
