#include "global.h"
#include "text.h"
#include "string_util.h"

// static types

// static declarations

// rodata

extern const u32 gFont3LatinGlyphs[];

// text
void CopyFont3LatinGlyphs(const u8 *text, u8 *tileDest, u8 replaceTransparentPixels, u16 tileRowStride, u8 bgColor, u8 *tileBuffer)
{
    const u8 *glyphData;
    u16 i, textLength = StringLength(text);

    if (replaceTransparentPixels == 0)
    {
        for (i = 0; i < textLength; i++)
        {
            glyphData = (const u8 *)((u32)(gFont3LatinGlyphs) +
                                     (u32)((text[i] & 0xfff0) * 0x40 + (text[i] & 0xf) * 0x20));
            CpuCopy16(glyphData, tileDest, 32);
            CpuCopy16(glyphData + 0x200, tileDest + tileRowStride, 32);
            tileDest += 32;
        }
    }
    else
    {
        u8 bgColorHigh, pixelHigh, pixelLow;
        u16 copySize, tileRow, glyphPixel, bufferIndex;
        bufferIndex = 0;
        bgColorHigh = bgColor << 4;
        copySize = textLength * 32;
        for (tileRow = 0; tileRow < 2; tileRow++)
        {
            for (i = 0; i < textLength; i++)
            {
                glyphData = (const u8 *)((u32)(gFont3LatinGlyphs) +
                                         (u32)((text[i] & 0xfff0) * 0x40 + (text[i] & 0xf) * 0x20)) +
                            (tileRow * 0x200);

                for (glyphPixel = 0; glyphPixel < 32; glyphPixel++)
                {
                    pixelHigh = *glyphData & 0xf0;
                    pixelLow = *glyphData & 0x0f;
                    if (pixelHigh == 0)
                    {
                        pixelHigh = bgColorHigh;
                    }
                    if (pixelLow == 0)
                    {
                        pixelLow = bgColor;
                    }
                    tileBuffer[bufferIndex++] = pixelHigh | pixelLow;
                    glyphData++;
                }
            }
            CpuCopy16(tileBuffer, tileDest, copySize);
            tileDest += tileRowStride;
            bufferIndex = 0;
        }
    }
}

void CopyFont3LatinText(const u8 *text, u8 *tileDest, u8 replaceTransparentPixels, u8 bgColor, u8 *tileBuffer)
{
    u8 textBuffer[5];
    bool8 reachedEnd = FALSE;
    while (!reachedEnd)
    {
        u16 textBufferIndex;
        for (textBufferIndex = 0; textBufferIndex < 4; textBufferIndex++)
        {
            if (*text == EOS)
            {
                if (textBufferIndex == 0)
                {
                    return;
                }
                reachedEnd = TRUE;
                break;
            }
            else
            {
                textBuffer[textBufferIndex] = *text++;
            }
        }
        while (textBufferIndex < 4)
        {
            textBuffer[textBufferIndex++] = 0;
        }
        textBuffer[textBufferIndex] = EOS;
        CopyFont3LatinGlyphs(textBuffer, tileDest, replaceTransparentPixels, 0x80, bgColor, tileBuffer);
        tileDest += 0x100;
    }
}
