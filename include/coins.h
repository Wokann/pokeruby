#ifndef GUARD_COINS_H
#define GUARD_COINS_H

void UpdateCoinsWindow(s32 coinAmount, u8 x, u8 y);
void ShowCoinsWindow(u32 coinAmount, u8 x, u8 y);
void HideCoinsWindow(u8 x, u8 y);
void PrintCoinsString(s32 coinAmount, u8 digitCount, u8 x, u8 y);
u16 GetCoins(void);
bool8 AddCoins(u16 toAdd);
bool8 RemoveCoins(u16 toSub);

#endif // GUARD_COINS_H
