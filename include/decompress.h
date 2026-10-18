#ifndef GUARD_DECOMPRESS_H
#define GUARD_DECOMPRESS_H

#include "sprite.h"

#define gDecompressionBuffer            (gSharedMem + 0x1E000)

void LZDecompressWram(const void *src, void *dest);
void LZDecompressVram(const void *src, void *dest);
void LoadCompressedSpriteSheet(const struct CompressedSpriteSheet *src);
void LoadCompressedSpriteSheetOverrideBuffer(const struct CompressedSpriteSheet *src, void *buffer);
void LoadCompressedSpritePalette(const struct CompressedSpritePalette *src);
void LoadCompressedSpritePaletteOverrideBuffer(const struct CompressedSpritePalette *src, void *buffer);
void DecompressPicFromTable_2(const struct CompressedSpriteSheet *src, u8 coords, u8 y_offset, void *d, void *dest, s32 species);
void HandleLoadSpecialPokePic(const struct CompressedSpriteSheet *src, u32 coords, u32 y_offset,
    void *decompBuf, void *dest, s32 species, u32 personality);
void LoadSpecialPokePic(const struct CompressedSpriteSheet *src, u32 b, u32 c,
    void *decompBuffer, void *dest, s32 species, u32 personality, u32 isFrontPic);
void Unused_LZDecompressWramIndirect(const void **src, void *dest);

#endif // GUARD_DECOMPRESS_H
