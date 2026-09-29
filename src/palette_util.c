#include "global.h"
#include "palette_util.h"
#include "roulette.h"

static u8 RouletteFlash_FadePalette(struct RouletteFlashPalette *palette);
static u8 RouletteFlash_FlashPalette(struct RouletteFlashPalette *palette);
u8 RouletteFlash_Remove(struct RouletteFlashUtil *flash, u8 id);

extern u16 gPlttBufferFaded[];
extern u16 gPlttBufferUnfaded[];

void RouletteFlash_Reset(struct RouletteFlashUtil *flash)
{
    flash->enabled = 0;
    flash->flags = 0;
    memset(&flash->palettes, 0, sizeof(flash->palettes));
}

u8 RouletteFlash_Add(struct RouletteFlashUtil *flash, u8 id, const struct RouletteFlashSettings *settings)
{
    if (!(id < 16) || flash->palettes[id].available)
        return 0xFF;

    flash->palettes[id].settings.color = settings->color;
    flash->palettes[id].settings.paletteOffset = settings->paletteOffset;
    flash->palettes[id].settings.numColors = settings->numColors;
    flash->palettes[id].settings.delay = settings->delay;
    flash->palettes[id].settings.unk6 = settings->unk6;
    flash->palettes[id].settings.numFadeCycles = settings->numFadeCycles;
    flash->palettes[id].settings.unk7_5 = settings->unk7_5;
    flash->palettes[id].settings.colorDeltaDir = settings->colorDeltaDir;
    flash->palettes[id].state = 0;
    flash->palettes[id].available = TRUE;
    flash->palettes[id].fadeCycleCounter = 0;
    flash->palettes[id].delayCounter = 0;
    if (flash->palettes[id].settings.colorDeltaDir < 0)
        flash->palettes[id].colorDelta = 0xFF;
    else
        flash->palettes[id].colorDelta = 1;
    return id;
}

u8 RouletteFlash_Remove(struct RouletteFlashUtil *flash, u8 id)
{
    if (id >= 16)
        return 0xFF;
    if (!flash->palettes[id].available)
        return 0xFF;
    memset(&flash->palettes[id], 0, sizeof(flash->palettes[id]));
    return id;
}

u8 RouletteFlash_FadePalette(struct RouletteFlashPalette *palette)
{
    u8 i;
    u8 returnval;

    for (i = 0; i < palette->settings.numColors; i++)
    {
        struct PlttData *faded =   (struct PlttData *)&gPlttBufferFaded[palette->settings.paletteOffset + i];
        struct PlttData *unfaded = (struct PlttData *)&gPlttBufferUnfaded[palette->settings.paletteOffset + i];

        switch (palette->state)
        {
        case 1:
            if (faded->r + palette->colorDelta >= 0 && faded->r + palette->colorDelta < 32)
                faded->r += palette->colorDelta;
            if (faded->g + palette->colorDelta >= 0 && faded->g + palette->colorDelta < 32)
                faded->g += palette->colorDelta;
            if (faded->b + palette->colorDelta >= 0 && faded->b + palette->colorDelta < 32)
                faded->b += palette->colorDelta;
            break;
        case 2:
            if (palette->colorDelta < 0)
            {
                if (faded->r + palette->colorDelta >= unfaded->r)
                    faded->r += palette->colorDelta;
                if (faded->g + palette->colorDelta >= unfaded->g)
                    faded->g += palette->colorDelta;
                if (faded->b + palette->colorDelta >= unfaded->b)
                    faded->b += palette->colorDelta;
            }
            else
            {
                if (faded->r + palette->colorDelta <= unfaded->r)
                    faded->r += palette->colorDelta;
                if (faded->g + palette->colorDelta <= unfaded->g)
                    faded->g += palette->colorDelta;
                if (faded->b + palette->colorDelta <= unfaded->b)
                    faded->b += palette->colorDelta;
            }
            break;
        }
    }
    if ((u32)palette->fadeCycleCounter++ != palette->settings.numFadeCycles)
    {
        returnval = 0;
    }
    else
    {
        palette->fadeCycleCounter = 0;
        palette->colorDelta *= -1;
        if (palette->state == 1)
            palette->state++;
        else
            palette->state--;
        returnval = 1;
    }
    return returnval;
}

u8 RouletteFlash_FlashPalette(struct RouletteFlashPalette *palette)
{
    u8 i = 0;

    switch (palette->state)
    {
    case 1:
        for (i = 0; i < palette->settings.numColors; i++)
            gPlttBufferFaded[palette->settings.paletteOffset + i] = palette->settings.color;
        palette->state++;
        break;
    case 2:
        for (i = 0; i < palette->settings.numColors; i++)
            gPlttBufferFaded[palette->settings.paletteOffset + i] = gPlttBufferUnfaded[palette->settings.paletteOffset + i];
        palette->state--;
        break;
    }
    return 1;
}

void RouletteFlash_Run(struct RouletteFlashUtil *flash)
{
    u8 i = 0;

    if (flash->enabled)
    {
        for (i = 0; i < 16; i++)
        {
            if ((flash->flags >> i) & 1)
            {
                if (--flash->palettes[i].delayCounter == 0xFF)
                {
                    if (flash->palettes[i].settings.color & FLASHUTIL_USE_EXISTING_COLOR)
                        RouletteFlash_FadePalette(&flash->palettes[i]);
                    else
                        RouletteFlash_FlashPalette(&flash->palettes[i]);
                    flash->palettes[i].delayCounter = flash->palettes[i].settings.delay;
                }
            }
        }
    }
}

void RouletteFlash_Enable(struct RouletteFlashUtil *flash, u16 flags)
{
    u8 i = 0;

    flash->enabled++;
    for (i = 0; i < 16; i++)
    {
        if ((flags >> i) & 1)
        {
            if (flash->palettes[i].available)
            {
                flash->flags |= 1 << i;
                flash->palettes[i].state = 1;
            }
        }
    }
}

void RouletteFlash_Stop(struct RouletteFlashUtil *flash, u16 flags)
{
    u8 i;

    for (i = 0; i < 16; i++)
    {
        if ((flash->flags >> i) & 1)
        {
            if (flash->palettes[i].available)
            {
                if ((flags >> i) & 1)
                {
                    u32 offset = flash->palettes[i].settings.paletteOffset;
                    u16 *faded = &gPlttBufferFaded[offset];
                    u16 *unfaded = &gPlttBufferUnfaded[offset];
                    memcpy(faded, unfaded, flash->palettes[i].settings.numColors * 2);
                    flash->palettes[i].state = 0;
                    flash->palettes[i].fadeCycleCounter = 0;
                    flash->palettes[i].delayCounter = 0;
                    if (flash->palettes[i].settings.colorDeltaDir < 0)
                        flash->palettes[i].colorDelta = 0xFF;
                    else
                        flash->palettes[i].colorDelta = 0x1;
                }
            }
        }
    }
    if (flags == 0xFFFF)
    {
        flash->enabled = 0;
        flash->flags = 0;
    }
    else
    {
        flash->flags = flash->flags & ~flags;
    }
}
void FillTilemapRect(u16 *dest, u16 src, u8 left, u8 top, u8 width, u8 height)
{
    u16 *_dest;
    u8 i;
    u8 j;
    i = 0;
    dest = &dest[top * 32 + left];
    for (; i < height; i++)
    {
        _dest = dest + i * 32;
        for (j = 0; j < width; j++)
        {
            *_dest++ = src;
        }
    }
}
void SetTilemapRect(u16 *dest, u16 *src, u8 left, u8 top, u8 width, u8 height)
{
    u16 *_dest;
    u16 *_src = src;
    u8 i;
    u8 j;
    i = 0;
    dest = &dest[top * 32 + left];
    for (; i < height; i++)
    {
        _dest = dest + i * 32;
        for (j = 0; j < width; j++)
        {
            *_dest++ = *_src++;
        }
    }
}
