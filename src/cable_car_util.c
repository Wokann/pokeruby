#include "global.h"
#include "cable_car_util.h"

void CableCarUtil_FillWrapped(void *tilemap, u16 value, u8 left, u8 top, u8 width, u8 height)
{
    u8 row;
    u8 column;
    u8 x;
    u8 y;

    for (row = 0, y = top; row < height; row++)
    {
        for (x = left, column = 0; column < width; column++)
        {
            *(u16 *)&((u8 *)tilemap)[y * 64 + x * 2] = value;
            x = (x + 1) % 32;
        }
        y = (y + 1) % 32;
    }
}

void CableCarUtil_CopyWrapped(void *tilemap, const u16 *source, u8 left, u8 top, u8 width, u8 height)
{
    u8 row;
    u8 column;
    u8 x;
    u8 y;
    const u16 *src;

    for (row = 0, src = source, y = top; row < height; row++)
    {
        for (x = left, column = 0; column < width; column++)
        {
            *(u16 *)&((u8 *)tilemap)[y * 64 + x * 2] = *src++;
            x = (x + 1) % 32;
        }
        y = (y + 1) % 32;
    }
}
