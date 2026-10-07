#include "global.h"
#include "palette_util.h"
#include "roulette.h"

static u8 sub_81249E4(struct RouletteFlashPalette *);
static u8 sub_8124BEC(struct RouletteFlashPalette *);
u8 unref_sub_81249B0(struct RouletteFlashUtil *, u8);

extern u16 gPlttBufferFaded[];
extern u16 gPlttBufferUnfaded[];

void RouletteFlash_Reset(struct RouletteFlashUtil *r0)
{
    r0->enabled = 0;
    r0->flags = 0;
    memset(&r0->palettes, 0, sizeof(r0->palettes));
}

u8 RouletteFlash_Add(struct RouletteFlashUtil *r0, u8 r1, const struct RouletteFlashSettings *r2)
{
    if (!(r1 < 16) || (r0->palettes[r1].available))
        return 0xFF;

    r0->palettes[r1].settings.color = r2->color;
    r0->palettes[r1].settings.paletteOffset = r2->paletteOffset;
    r0->palettes[r1].settings.numColors = r2->numColors;
    r0->palettes[r1].settings.delay = r2->delay;
    r0->palettes[r1].settings.unk6 = r2->unk6;
    r0->palettes[r1].settings.numFadeCycles = r2->numFadeCycles;
    r0->palettes[r1].settings.unk7_5 = r2->unk7_5;
    r0->palettes[r1].settings.colorDeltaDir = r2->colorDeltaDir;
    r0->palettes[r1].state = 0;
    r0->palettes[r1].available = 1;
    r0->palettes[r1].fadeCycleCounter = 0;
    r0->palettes[r1].delayCounter = 0;
    if (r0->palettes[r1].settings.colorDeltaDir < 0)
        r0->palettes[r1].colorDelta = 0xFF;
    else
        r0->palettes[r1].colorDelta = 1;
    return r1;
}

u8 unref_sub_81249B0(struct RouletteFlashUtil *r0, u8 r1)
{
    if (r1 >= 16)
        return 0xFF;
    if (!r0->palettes[r1].available)
        return 0xFF;
    memset(&r0->palettes[r1], 0, sizeof(r0->palettes[r1]));
    return r1;
}

u8 sub_81249E4(struct RouletteFlashPalette *r0)
{
    u8 i;
    u8 returnval;

    for (i = 0; i < r0->settings.numColors; i++)
    {
        struct PlttData *faded =   (struct PlttData *)&gPlttBufferFaded[r0->settings.paletteOffset + i];
        struct PlttData *unfaded = (struct PlttData *)&gPlttBufferUnfaded[r0->settings.paletteOffset + i];

        switch (r0->state)
        {
        case 1:
            if (faded->r + r0->colorDelta >= 0 && faded->r + r0->colorDelta < 32)
                faded->r += r0->colorDelta;
            if (faded->g + r0->colorDelta >= 0 && faded->g + r0->colorDelta < 32)
                faded->g += r0->colorDelta;
            if (faded->b + r0->colorDelta >= 0 && faded->b + r0->colorDelta < 32)
                faded->b += r0->colorDelta;
            break;
        case 2:
            if (r0->colorDelta < 0)
            {
                if (faded->r + r0->colorDelta >= unfaded->r)
                    faded->r += r0->colorDelta;
                if (faded->g + r0->colorDelta >= unfaded->g)
                    faded->g += r0->colorDelta;
                if (faded->b + r0->colorDelta >= unfaded->b)
                    faded->b += r0->colorDelta;
            }
            else
            {
                if (faded->r + r0->colorDelta <= unfaded->r)
                    faded->r += r0->colorDelta;
                if (faded->g + r0->colorDelta <= unfaded->g)
                    faded->g += r0->colorDelta;
                if (faded->b + r0->colorDelta <= unfaded->b)
                    faded->b += r0->colorDelta;
            }
            break;
        }
    }
    if ((u32)r0->fadeCycleCounter++ != r0->settings.numFadeCycles)
    {
        returnval = 0;
    }
    else
    {
        r0->fadeCycleCounter = 0;
        r0->colorDelta *= -1;
        if (r0->state == 1)
            r0->state++;
        else
            r0->state--;
        returnval = 1;
    }
    return returnval;
}

u8 sub_8124BEC(struct RouletteFlashPalette *r0)
{
    u8 rg2 = 0;

    switch (r0->state)
    {
    case 1:
        for (rg2 = 0; rg2 < r0->settings.numColors; rg2++)
            gPlttBufferFaded[r0->settings.paletteOffset + rg2] = r0->settings.color;
        r0->state++;
        break;
    case 2:
        for (rg2 = 0; rg2 < r0->settings.numColors; rg2++)
            gPlttBufferFaded[r0->settings.paletteOffset + rg2] = gPlttBufferUnfaded[r0->settings.paletteOffset + rg2];
        r0->state--;
        break;
    }
    return 1;
}

void RouletteFlash_Run(struct RouletteFlashUtil *r0)
{
    u8 i = 0;

    if (r0->enabled)
    {
        for (i = 0; i < 16; i++)
        {
            if ((r0->flags >> i) & 1)
            {
                if (--r0->palettes[i].delayCounter == 0xFF) // if underflow ?
                {
                    if (r0->palettes[i].settings.color & 0x8000) // PlttData->unused_15 ?
                        sub_81249E4(&r0->palettes[i]);
                    else
                        sub_8124BEC(&r0->palettes[i]);
                    r0->palettes[i].delayCounter = r0->palettes[i].settings.delay;
                }
            }
        }
    }
}

void RouletteFlash_Enable(struct RouletteFlashUtil *r0, u16 r1)
{
    u8 i = 0;

    r0->enabled++;
    for (i = 0; i < 16; i++)
    {
        if ((r1 >> i) & 1)
        {
            if (r0->palettes[i].available)
            {
                r0->flags |= 1 << i;
                r0->palettes[i].state = 1;
            }
        }
    }
}

void RouletteFlash_Stop(struct RouletteFlashUtil *r0, u16 r1)
{
    u8 i;

    for (i = 0; i < 16; i++)
    {
        if ((r0->flags >> i) & 1)
        {
            if (r0->palettes[i].available)
            {
                if ((r1 >> i) & 1)
                {
                    u32 offset = r0->palettes[i].settings.paletteOffset;
                    u16 *faded = &gPlttBufferFaded[offset];
                    u16 *unfaded = &gPlttBufferUnfaded[offset];
                    memcpy(faded, unfaded, r0->palettes[i].settings.numColors * 2);
                    r0->palettes[i].state = 0;
                    r0->palettes[i].fadeCycleCounter = 0;
                    r0->palettes[i].delayCounter = 0;
                    if (r0->palettes[i].settings.colorDeltaDir < 0)
                        r0->palettes[i].colorDelta = 0xFF;
                    else
                        r0->palettes[i].colorDelta = 0x1;
                }
            }
        }
    }
    if (r1 == 0xFFFF)
    {
        r0->enabled = 0;
        r0->flags = 0;
    }
    else
    {
        r0->flags = r0->flags & ~r1;
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
