#include "global.h"
#include "coins.h"
#include "constants/coins.h"
#include "menu.h"
#include "string_util.h"
#include "strings.h"

void UpdateCoinsWindow(s32 coinAmount, u8 x, u8 y)
{
    PrintCoinsString(coinAmount, MAX_COIN_DIGITS, x + 2, y + 1);
}

void ShowCoinsWindow(u32 coinAmount, u8 x, u8 y)
{
    Menu_DrawStdWindowFrame(x, y, x + 9, y + 3);
    UpdateCoinsWindow(coinAmount, x, y);
}

void HideCoinsWindow(u8 x, u8 y)
{
    Menu_EraseWindowRect(x, y, x + 9, y + 3);
}

void PrintCoinsString(s32 coinAmount, u8 digitCount, u8 x, u8 y)
{
    u8 coinString[16];
    u8 *stringVar;
    u8 alignmentOffset;
    u8 pixelOffset;

    ConvertIntToDecimalStringSimple(coinString, coinAmount);
    alignmentOffset = digitCount * 6 + 0x21 - 8 * (digitCount + 2);
    x -= alignmentOffset / 8;
    pixelOffset = alignmentOffset % 8;
    stringVar = gStringVar1;
    if (pixelOffset)
    {
        stringVar[0] = EXT_CTRL_CODE_BEGIN;
        stringVar[1] = EXT_CTRL_CODE_CLEAR;
        stringVar[2] = 8 - pixelOffset;
        stringVar += 3;
    }
    stringVar[0] = EXT_CTRL_CODE_BEGIN;
    stringVar[1] = EXT_CTRL_CODE_CLEAR;
    stringVar[2] = (digitCount - StringLength(coinString)) * 6;
    stringVar += 3;
    StringCopy(stringVar, coinString);
    Menu_PrintText(gText_Coins, x, y);
}

u16 GetCoins(void)
{
    return gSaveBlock1.coins;
}

bool8 AddCoins(u16 toAdd)
{
    u32 newAmount;

    if (GetCoins() >= MAX_COINS)
        return FALSE;
    newAmount = toAdd + gSaveBlock1.coins;
    if (gSaveBlock1.coins > (u16)newAmount)
        gSaveBlock1.coins = MAX_COINS;
    else
    {
        gSaveBlock1.coins = newAmount;
        if ((u16)newAmount > MAX_COINS)
            gSaveBlock1.coins = MAX_COINS;
    }
    return TRUE;
}

bool8 RemoveCoins(u16 toSub)
{
    if (GetCoins() >= toSub)
    {
        gSaveBlock1.coins -= toSub;
        return TRUE;
    }
    else
        return FALSE;
}
