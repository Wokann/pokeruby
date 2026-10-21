#include "global.h"
#include "constants/rgb.h"
#include "palette.h"
#include "util.h"
#include "decompress.h"
enum
{
    NORMAL_FADE,
    FAST_FADE,
    HARDWARE_FADE,
};

// These are structs for some unused palette system.
// The full functionality of this system is unknown.

#define NUM_PALETTE_STRUCTS 16

struct PaletteStructTemplate
{
    u16 id;
    u16 *src;
    bool16 pst_field_8_0:1;
    u16 unused:9;
    u16 size:5;
    u16 pst_field_9_7:1;
    u8 time1;
    u8 srcCount:5;
    u8 state:3;
    u8 time2;
};

struct PaletteStruct
{
    const struct PaletteStructTemplate *template;
    bool32 active:1;
    bool32 flag:1;
    u32 baseDestOffset:9;
    u16 destOffset:10;
    u16 srcIndex:7;
    u8 countdown1;
    u8 countdown2;
};

EWRAM_DATA u16 gPlttBufferUnfaded[PLTT_BUFFER_SIZE] = {0};
EWRAM_DATA u16 gPlttBufferFaded[PLTT_BUFFER_SIZE] = {0};
EWRAM_DATA static struct PaletteStruct sPaletteStructs[NUM_PALETTE_STRUCTS] = {0};
EWRAM_DATA struct PaletteFadeControl gPaletteFade = {0};
EWRAM_DATA static u32 UNUSED sFiller = 0;
EWRAM_DATA static u32 sPlttBufferTransferPending = 0;
EWRAM_DATA u8 gPaletteDecompressionBuffer[PLTT_SIZE] = {0};

static const struct PaletteStructTemplate sDummyPaletteStructTemplate = {
    .id = 0xFFFF,
    .state = 1
};

static void PaletteStruct_Copy(struct PaletteStruct *, u32 *);
static void PaletteStruct_Blend(struct PaletteStruct *, u32 *);
static void PaletteStruct_TryEnd(struct PaletteStruct *);
static u8 PaletteStruct_GetPalNum(u16);
static void PaletteStruct_Reset(u8 paletteNum);
static u8 UpdateNormalPaletteFade(void);
static void BeginFastPaletteFadeInternal(u8);
static u8 UpdateFastPaletteFade(void);
static u8 UpdateHardwarePaletteFade(void);
static void UpdateBlendRegisters(void);
static bool8 IsSoftwarePaletteFadeFinishing(void);

void LoadCompressedPalette(const void *src, u16 offset, u16 size)
{
    LZDecompressWram(src, gPaletteDecompressionBuffer);
    CpuCopy16(gPaletteDecompressionBuffer, gPlttBufferUnfaded + offset, size);
    CpuCopy16(gPaletteDecompressionBuffer, gPlttBufferFaded + offset, size);
}

void LoadPalette(const void *src, u16 offset, u16 size)
{
    CpuCopy16(src, gPlttBufferUnfaded + offset, size);
    CpuCopy16(src, gPlttBufferFaded + offset, size);
}

void FillPalette(u16 value, u16 offset, u16 size)
{
    CpuFill16(value, gPlttBufferUnfaded + offset, size);
    CpuFill16(value, gPlttBufferFaded + offset, size);
}

void TransferPlttBuffer(void)
{
    if (!gPaletteFade.bufferTransferDisabled)
    {
        void *src = gPlttBufferFaded;
        void *dest = (void *)PLTT;
        DmaCopy16(3, src, dest, PLTT_SIZE);
        sPlttBufferTransferPending = FALSE;
        if (gPaletteFade.mode == HARDWARE_FADE && gPaletteFade.active)
            UpdateBlendRegisters();
    }
}

u8 UpdatePaletteFade(void)
{
    u8 result;
    u8 dummy = 0;

    if (sPlttBufferTransferPending)
        return PALETTE_FADE_STATUS_LOADING;

    if (gPaletteFade.mode == NORMAL_FADE)
        result = UpdateNormalPaletteFade();
    else if (gPaletteFade.mode == FAST_FADE)
        result = UpdateFastPaletteFade();
    else
        result = UpdateHardwarePaletteFade();

    sPlttBufferTransferPending = gPaletteFade.multipurpose1 | dummy;

    return result;
}

void ResetPaletteFade(void)
{
    u8 i;

    for (i = 0; i < NUM_PALETTE_STRUCTS; i++)
        PaletteStruct_Reset(i);

    ResetPaletteFadeControl();
}

static void ReadPlttIntoBuffers(void)
{
    u16 i;
    u16 *pltt = (u16 *)PLTT;

    for (i = 0; i < (int)PLTT_BUFFER_SIZE; i++)
    {
        gPlttBufferUnfaded[i] = pltt[i];
        gPlttBufferFaded[i] = pltt[i];
    }
}

bool8 BeginNormalPaletteFade(u32 selectedPalettes, s8 delay, u8 startY, u8 targetY, u16 blendColor)
{
    u8 temp;
    register u32 color asm("r8") = blendColor;

    if (gPaletteFade.active)
    {
        return FALSE;
    }
    else
    {
        gPaletteFade.deltaY = 2;

        if (delay < 0)
        {
            gPaletteFade.deltaY += (delay * -1);
            delay = 0;
        }

        gPaletteFade_selectedPalettes = selectedPalettes;
        gPaletteFade.delayCounter = delay;
        gPaletteFade_delay = delay;
        gPaletteFade.y = startY;
        gPaletteFade.targetY = targetY;
        gPaletteFade.blendColor = color;
        gPaletteFade.active = TRUE;
        gPaletteFade.mode = NORMAL_FADE;

        if (startY < targetY)
            gPaletteFade.yDec = FALSE;
        else
            gPaletteFade.yDec = TRUE;

        UpdatePaletteFade();

        temp = gPaletteFade.bufferTransferDisabled;
        gPaletteFade.bufferTransferDisabled = FALSE;
        CpuCopy32(gPlttBufferFaded, (void *)PLTT, PLTT_SIZE);
        sPlttBufferTransferPending = FALSE;
        if (gPaletteFade.mode == HARDWARE_FADE && gPaletteFade.active)
            UpdateBlendRegisters();
        gPaletteFade.bufferTransferDisabled = temp;
        return TRUE;
    }
}

static bool8 UNUSED BeginPlttFade(u32 selectedPalettes, s8 delay, u8 startY, u8 targetY, u16 blendColor)
{
    ReadPlttIntoBuffers();
    return BeginNormalPaletteFade(selectedPalettes, delay, startY, targetY, blendColor);
}

static void UNUSED PaletteStruct_Run(u8 a1, u32 *unkFlags)
{
    u8 i;

    for (i = 0; i < NUM_PALETTE_STRUCTS; i++)
    {
        struct PaletteStruct *palstruct = &sPaletteStructs[i];
        if (palstruct->active)
        {
            if (palstruct->template->pst_field_8_0 == a1)
            {
                u8 val1 = palstruct->srcIndex;
                u8 val2 = palstruct->template->srcCount;
                if (val1 == val2)
                {
                    PaletteStruct_TryEnd(palstruct);
                    if (!palstruct->active)
                        continue;
                }
                if (palstruct->countdown1 == 0)
                    PaletteStruct_Copy(palstruct, unkFlags);
                else
                    palstruct->countdown1--;

                PaletteStruct_Blend(palstruct, unkFlags);
            }
        }
    }
}

static void PaletteStruct_Copy(struct PaletteStruct *palStruct, u32 *unkFlags)
{
    s32 srcIndex;
    s32 srcCount;
    u8 i = 0;
    u16 srcOffset = palStruct->srcIndex * palStruct->template->size;

    if (!palStruct->template->pst_field_8_0)
    {
        while (i < palStruct->template->size)
        {
            gPlttBufferUnfaded[palStruct->destOffset] = palStruct->template->src[srcOffset];
            gPlttBufferFaded[palStruct->destOffset] = palStruct->template->src[srcOffset];
            i++;
            palStruct->destOffset++;
            srcOffset++;
        }
    }
    else
    {
        while (i < palStruct->template->size)
        {
            gPlttBufferFaded[palStruct->destOffset] = palStruct->template->src[srcOffset];
            i++;
            palStruct->destOffset++;
            srcOffset++;
        }
    }

    palStruct->destOffset = palStruct->baseDestOffset;
    palStruct->countdown1 = palStruct->template->time1;
    palStruct->srcIndex++;

    srcIndex = palStruct->srcIndex;
    srcCount = palStruct->template->srcCount;

    if (srcIndex >= srcCount)
    {
        if (palStruct->countdown2)
            palStruct->countdown2--;
        palStruct->srcIndex = 0;
    }

    *unkFlags |= 1 << (palStruct->baseDestOffset >> 4);
}

static void PaletteStruct_Blend(struct PaletteStruct *palStruct, u32 *unkFlags)
{
    if (gPaletteFade.active && ((1 << (palStruct->baseDestOffset >> 4)) & gPaletteFade_selectedPalettes))
    {
        if (!palStruct->template->pst_field_8_0)
        {
            if (gPaletteFade.delayCounter != gPaletteFade_delay)
            {
                BlendPalette(
                    palStruct->baseDestOffset,
                    palStruct->template->size,
                    gPaletteFade.y,
                    gPaletteFade.blendColor);
            }
        }
        else
        {
            if (!gPaletteFade.delayCounter)
            {
                if (palStruct->countdown1 != palStruct->template->time1)
                {
                    u32 srcOffset = palStruct->srcIndex * palStruct->template->size;
                    u8 i;

                    for (i = 0; i < palStruct->template->size; i++)
                        gPlttBufferFaded[palStruct->baseDestOffset + i] = palStruct->template->src[srcOffset + i];
                }
            }
        }
    }
}

static void PaletteStruct_TryEnd(struct PaletteStruct *pal)
{
    if (!pal->countdown2)
    {
        s32 state = pal->template->state;

        if (!state)
        {
            pal->srcIndex = 0;
            pal->countdown1 = pal->template->time1;
            pal->countdown2 = pal->template->time2;
            pal->destOffset = pal->baseDestOffset;
        }
        else
        {
            if (state < 0)
                return;
            if (state > 2)
                return;
            PaletteStruct_ResetById(pal->template->id);
        }
    }
    else
    {
        pal->countdown2--;
    }
}

void PaletteStruct_ResetById(u16 id)
{
    u8 paletteNum = PaletteStruct_GetPalNum(id);
    if (paletteNum != NUM_PALETTE_STRUCTS)
        PaletteStruct_Reset(paletteNum);
}

static void PaletteStruct_Reset(u8 paletteNum)
{
    sPaletteStructs[paletteNum].template = &sDummyPaletteStructTemplate;
    sPaletteStructs[paletteNum].active = FALSE;
    sPaletteStructs[paletteNum].baseDestOffset = 0;
    sPaletteStructs[paletteNum].destOffset = 0;
    sPaletteStructs[paletteNum].srcIndex = 0;
    sPaletteStructs[paletteNum].flag = FALSE;
    sPaletteStructs[paletteNum].countdown1 = 0;
    sPaletteStructs[paletteNum].countdown2 = 0;
}

void ResetPaletteFadeControl(void)
{
    gPaletteFade.multipurpose1 = 0;
    gPaletteFade.multipurpose2 = 0;
    gPaletteFade.delayCounter = 0;
    gPaletteFade.y = 0;
    gPaletteFade.targetY = 0;
    gPaletteFade.blendColor = RGB_BLACK;
    gPaletteFade.active = FALSE;
    gPaletteFade.multipurpose2 = 0; // assign same value twice
    gPaletteFade.yDec = FALSE;
    gPaletteFade.bufferTransferDisabled = FALSE;
    gPaletteFade.shouldResetBlendRegisters = FALSE;
    gPaletteFade.hardwareFadeFinishing = FALSE;
    gPaletteFade.softwareFadeFinishing = FALSE;
    gPaletteFade.softwareFadeFinishingCounter = 0;
    gPaletteFade.objPaletteToggle = FALSE;
    gPaletteFade.deltaY = 2;
}

static void UNUSED PaletteStruct_SetUnusedFlag(u16 id)
{
    u8 paletteNum = PaletteStruct_GetPalNum(id);
    if (paletteNum != NUM_PALETTE_STRUCTS)
        sPaletteStructs[paletteNum].flag = TRUE;
}

static void UNUSED PaletteStruct_ClearUnusedFlag(u16 id)
{
    u8 paletteNum = PaletteStruct_GetPalNum(id);
    if (paletteNum != NUM_PALETTE_STRUCTS)
        sPaletteStructs[paletteNum].flag = FALSE;
}

static u8 PaletteStruct_GetPalNum(u16 id)
{
    u8 i;

    for (i = 0; i < NUM_PALETTE_STRUCTS; i++)
        if (sPaletteStructs[i].template->id == id)
            return i;

    return NUM_PALETTE_STRUCTS;
}

static u8 UpdateNormalPaletteFade(void)
{
    u16 paletteOffset;
    u16 selectedPalettes;

    if (!gPaletteFade.active)
        return PALETTE_FADE_STATUS_DONE;

    if (IsSoftwarePaletteFadeFinishing())
    {
        return gPaletteFade.active ? PALETTE_FADE_STATUS_ACTIVE : PALETTE_FADE_STATUS_DONE;
    }
    else
    {
        if (!gPaletteFade.objPaletteToggle)
        {
            if (gPaletteFade.delayCounter < gPaletteFade_delay)
            {
                gPaletteFade.delayCounter++;
                return PALETTE_FADE_STATUS_DELAY;
            }
            gPaletteFade.delayCounter = 0;
        }

        paletteOffset = BG_PLTT_OFFSET;

        if (!gPaletteFade.objPaletteToggle)
        {
            selectedPalettes = gPaletteFade_selectedPalettes;
        }
        else
        {
            selectedPalettes = HIHALF(gPaletteFade_selectedPalettes);
            paletteOffset = OBJ_PLTT_OFFSET;
        }

        while (selectedPalettes)
        {
            if (selectedPalettes & 1)
                BlendPalette(
                    paletteOffset,
                    PLTT_ID(1),
                    gPaletteFade.y,
                    gPaletteFade.blendColor);
            selectedPalettes >>= 1;
            paletteOffset += PLTT_ID(1);
        }

        gPaletteFade.objPaletteToggle ^= TRUE;

        if (!gPaletteFade.objPaletteToggle)
        {
            if (gPaletteFade.y == gPaletteFade.targetY)
            {
                gPaletteFade_selectedPalettes = 0;
                gPaletteFade.softwareFadeFinishing = TRUE;
            }
            else
            {
                s8 val;

                if (!gPaletteFade.yDec)
                {
                    val = gPaletteFade.y;
                    val += gPaletteFade.deltaY;
                    if (val > gPaletteFade.targetY)
                        val = gPaletteFade.targetY;
                    gPaletteFade.y = val;
                }
                else
                {
                    val = gPaletteFade.y;
                    val -= gPaletteFade.deltaY;
                    if (val < gPaletteFade.targetY)
                        val = gPaletteFade.targetY;
                    gPaletteFade.y = val;
                }
            }
        }

        return gPaletteFade.active ? PALETTE_FADE_STATUS_ACTIVE : PALETTE_FADE_STATUS_DONE;
    }
}

void InvertPlttBuffer(u32 selectedPalettes)
{
    u16 paletteOffset = BG_PLTT_OFFSET;

    while (selectedPalettes)
    {
        if (selectedPalettes & 1)
        {
            u8 i;
            for (i = 0; i < PLTT_ID(1); i++)
                gPlttBufferFaded[paletteOffset + i] = ~gPlttBufferFaded[paletteOffset + i];
        }
        selectedPalettes >>= 1;
        paletteOffset += PLTT_ID(1);
    }
}

void TintPlttBuffer(u32 selectedPalettes, s8 r, s8 g, s8 b)
{
    u16 paletteOffset = BG_PLTT_OFFSET;

    while (selectedPalettes)
    {
        if (selectedPalettes & 1)
        {
            u8 i;
            for (i = 0; i < PLTT_ID(1); i++)
            {
                struct PlttData *data = (struct PlttData *)&gPlttBufferFaded[paletteOffset + i];
                data->r += r;
                data->g += g;
                data->b += b;
            }
        }
        selectedPalettes >>= 1;
        paletteOffset += PLTT_ID(1);
    }
}

void UnfadePlttBuffer(u32 selectedPalettes)
{
    u16 paletteOffset = BG_PLTT_OFFSET;

    while (selectedPalettes)
    {
        if (selectedPalettes & 1)
        {
            u8 i;
            for (i = 0; i < PLTT_ID(1); i++)
                gPlttBufferFaded[paletteOffset + i] = gPlttBufferUnfaded[paletteOffset + i];
        }
        selectedPalettes >>= 1;
        paletteOffset += PLTT_ID(1);
    }
}

void BeginFastPaletteFade(u8 submode)
{
    gPaletteFade.deltaY = 2;
    BeginFastPaletteFadeInternal(submode);
}

static void BeginFastPaletteFadeInternal(u8 submode)
{
    gPaletteFade.y = 31;
    gPaletteFade_submode = submode & 0x3F;
    gPaletteFade.active = TRUE;
    gPaletteFade.mode = FAST_FADE;

    if (submode == FAST_FADE_IN_FROM_BLACK)
        CpuFill16(RGB_BLACK, gPlttBufferFaded, PLTT_SIZE);

    if (submode == FAST_FADE_IN_FROM_WHITE)
        CpuFill16(RGB_WHITE, gPlttBufferFaded, PLTT_SIZE);

    UpdatePaletteFade();
}

static u8 UpdateFastPaletteFade(void)
{
    u16 i;
    u16 paletteOffsetStart;
    u16 paletteOffsetEnd;
    s8 r0;
    s8 g0;
    s8 b0;
    s8 r;
    s8 g;
    s8 b;

    if (!gPaletteFade.active)
        return PALETTE_FADE_STATUS_DONE;

    if (IsSoftwarePaletteFadeFinishing())
        return gPaletteFade.active ? PALETTE_FADE_STATUS_ACTIVE : PALETTE_FADE_STATUS_DONE;

    if (gPaletteFade.objPaletteToggle)
    {
        paletteOffsetStart = OBJ_PLTT_OFFSET;
        paletteOffsetEnd = PLTT_BUFFER_SIZE;
    }
    else
    {
        paletteOffsetStart = BG_PLTT_OFFSET;
        paletteOffsetEnd = OBJ_PLTT_OFFSET;
    }

    switch (gPaletteFade_submode)
    {
    case FAST_FADE_IN_FROM_WHITE:
        for (i = paletteOffsetStart; i < paletteOffsetEnd; i++)
        {
            struct PlttData *unfaded;
            struct PlttData *faded;

            unfaded = (struct PlttData *)&gPlttBufferUnfaded[i];
            r0 = unfaded->r;
            g0 = unfaded->g;
            b0 = unfaded->b;

            faded = (struct PlttData *)&gPlttBufferFaded[i];
            r = faded->r - 2;
            g = faded->g - 2;
            b = faded->b - 2;

            if (r < r0)
                r = r0;
            if (g < g0)
                g = g0;
            if (b < b0)
                b = b0;

            gPlttBufferFaded[i] = RGB(r, g, b);
        }
        break;
    case FAST_FADE_OUT_TO_WHITE:
        for (i = paletteOffsetStart; i < paletteOffsetEnd; i++)
        {
            struct PlttData *data = (struct PlttData *)&gPlttBufferFaded[i];
            r = data->r + 2;
            g = data->g + 2;
            b = data->b + 2;

            if (r > 31)
                r = 31;
            if (g > 31)
                g = 31;
            if (b > 31)
                b = 31;

            gPlttBufferFaded[i] = RGB(r, g, b);
        }
        break;
    case FAST_FADE_IN_FROM_BLACK:
        for (i = paletteOffsetStart; i < paletteOffsetEnd; i++)
        {
            struct PlttData *unfaded;
            struct PlttData *faded;

            unfaded = (struct PlttData *)&gPlttBufferUnfaded[i];
            r0 = unfaded->r;
            g0 = unfaded->g;
            b0 = unfaded->b;

            faded = (struct PlttData *)&gPlttBufferFaded[i];
            r = faded->r + 2;
            g = faded->g + 2;
            b = faded->b + 2;

            if (r > r0)
                r = r0;
            if (g > g0)
                g = g0;
            if (b > b0)
                b = b0;

            gPlttBufferFaded[i] = RGB(r, g, b);
        }
        break;
    case FAST_FADE_OUT_TO_BLACK:
        for (i = paletteOffsetStart; i < paletteOffsetEnd; i++)
        {
            struct PlttData *data = (struct PlttData *)&gPlttBufferFaded[i];
            r = data->r - 2;
            g = data->g - 2;
            b = data->b - 2;

            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;

            gPlttBufferFaded[i] = RGB(r, g, b);
        }
    }

    gPaletteFade.objPaletteToggle ^= TRUE;

    if (gPaletteFade.objPaletteToggle)
        return gPaletteFade.active ? PALETTE_FADE_STATUS_ACTIVE : PALETTE_FADE_STATUS_DONE;

    if (gPaletteFade.y - gPaletteFade.deltaY < 0)
        gPaletteFade.y = 0;
    else
        gPaletteFade.y -= gPaletteFade.deltaY;

    if (gPaletteFade.y == 0)
    {
        switch (gPaletteFade_submode)
        {
        case FAST_FADE_IN_FROM_WHITE:
        case FAST_FADE_IN_FROM_BLACK:
            CpuCopy32(gPlttBufferUnfaded, gPlttBufferFaded, PLTT_SIZE);
            break;
        case FAST_FADE_OUT_TO_WHITE:
            CpuFill32(0xFFFFFFFF, gPlttBufferFaded, PLTT_SIZE);
            break;
        case FAST_FADE_OUT_TO_BLACK:
            CpuFill32(0x00000000, gPlttBufferFaded, PLTT_SIZE);
            break;
        }

        gPaletteFade.mode = NORMAL_FADE;
        gPaletteFade.softwareFadeFinishing = TRUE;
    }

    return gPaletteFade.active ? PALETTE_FADE_STATUS_ACTIVE : PALETTE_FADE_STATUS_DONE;
}

void BeginHardwarePaletteFade(u8 blendCnt, u8 delay, u8 y, u8 targetY, u8 shouldResetBlendRegisters)
{
    gPaletteFade_blendCnt = blendCnt;
    gPaletteFade.delayCounter = delay;
    gPaletteFade_delay = delay;
    gPaletteFade.y = y;
    gPaletteFade.targetY = targetY;
    gPaletteFade.active = TRUE;
    gPaletteFade.mode = HARDWARE_FADE;
    gPaletteFade.shouldResetBlendRegisters = shouldResetBlendRegisters & TRUE;
    gPaletteFade.hardwareFadeFinishing = FALSE;

    if (y < targetY)
        gPaletteFade.yDec = FALSE;
    else
        gPaletteFade.yDec = TRUE;
}

static u8 UpdateHardwarePaletteFade(void)
{
    if (!gPaletteFade.active)
        return PALETTE_FADE_STATUS_DONE;

    if (gPaletteFade.delayCounter < gPaletteFade_delay)
    {
        gPaletteFade.delayCounter++;
        return PALETTE_FADE_STATUS_DELAY;
    }

    gPaletteFade.delayCounter = 0;

    if (!gPaletteFade.yDec)
    {
        gPaletteFade.y++;
        if (gPaletteFade.y > gPaletteFade.targetY)
        {
            gPaletteFade.hardwareFadeFinishing++;
            gPaletteFade.y--;
        }
    }
    else
    {
        s32 y = gPaletteFade.y--;
        if (y - 1 < gPaletteFade.targetY)
        {
            gPaletteFade.hardwareFadeFinishing++;
            gPaletteFade.y++;
        }
    }

    if (gPaletteFade.hardwareFadeFinishing)
    {
        if (gPaletteFade.shouldResetBlendRegisters)
        {
            gPaletteFade_blendCnt = 0;
            gPaletteFade.y = 0;
        }
        gPaletteFade.shouldResetBlendRegisters = FALSE;
    }

    return gPaletteFade.active ? PALETTE_FADE_STATUS_ACTIVE : PALETTE_FADE_STATUS_DONE;
}

static void UpdateBlendRegisters(void)
{
    REG_BLDCNT = gPaletteFade_blendCnt;
    REG_BLDY = gPaletteFade.y;
    if (gPaletteFade.hardwareFadeFinishing)
    {
        gPaletteFade.hardwareFadeFinishing = FALSE;
        gPaletteFade.mode = NORMAL_FADE;
        gPaletteFade_blendCnt = 0;
        gPaletteFade.y = 0;
        gPaletteFade.active = FALSE;
    }
}

static bool8 IsSoftwarePaletteFadeFinishing(void)
{
    if (gPaletteFade.softwareFadeFinishing)
    {
        if (gPaletteFade.softwareFadeFinishingCounter == 4)
        {
            gPaletteFade.active = FALSE;
            gPaletteFade.softwareFadeFinishing = FALSE;
            gPaletteFade.softwareFadeFinishingCounter = 0;
        }
        else
        {
            gPaletteFade.softwareFadeFinishingCounter++;
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void BlendPalettes(u32 selectedPalettes, u8 coeff, u16 color)
{
    u16 paletteOffset;

    for (paletteOffset = BG_PLTT_OFFSET; selectedPalettes; paletteOffset += PLTT_ID(1))
    {
        if (selectedPalettes & 1)
            BlendPalette(paletteOffset, PLTT_ID(1), coeff, color);
        selectedPalettes >>= 1;
    }
}

void BlendPalettesUnfaded(u32 selectedPalettes, u8 coeff, u16 color)
{
    DmaCopy32Defvars(3, gPlttBufferUnfaded, gPlttBufferFaded, PLTT_SIZE);
    BlendPalettes(selectedPalettes, coeff, color);
}
