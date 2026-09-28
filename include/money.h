#ifndef GUARD_MONEY_H
#define GUARD_MONEY_H

bool8 IsEnoughMoney(u32 money, u32 cost);
void AddMoney(u32 *moneyPtr, u32 amount);
void RemoveMoney(u32 *moneyPtr, u32 amount);
void GetMoneyAmountText(u8 *buffer, u32 amount, u8 fieldWidth);
void PrintMoneyAmount(u32 amount, u8 width, u8 x, u8 y);
void PrintMoneyAmountRightAligned(u32 amount, u8 right, u8 top);
void Draw10000Sprite(u8 x, u8 y, s32 hasControlCode);
void ChangeAmountInMoneyBox(u32 amount, u8 x, u8 y);
void DrawMoneyBox(u32 amount, u8 x, u8 y);
void HideMoneyBox(u8 x, u8 y);
bool8 IsEnoughForCostInVar0x8005(void);
void SubtractMoneyFromVar0x8005(void);

#endif // GUARD_MONEY_H
