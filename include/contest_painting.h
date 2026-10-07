#ifndef GUARD_CONTESTPAINTING_H
#define GUARD_CONTESTPAINTING_H

#define MOSAIC_BIT_BG_HSIZE (0)
#define MOSAIC_BIT_BG_VSIZE (4)
#define MOSAIC_BIT_OBJ_HSIZE (8)
#define MOSAIC_BIT_OBJ_VSIZE (12)

struct ContestPaintingBuffers
{
    u16 pixels[0x1000];
    u16 palette[0x100];
};

struct LabelPair
{
    const u8 *prefix;
    const u8 *suffix;
};

void SetContestWinnerForPainting(u32 contestWinnerId);
void CB2_ContestPainting(void);

#endif
