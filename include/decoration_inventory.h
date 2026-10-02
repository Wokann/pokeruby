#ifndef GUARD_DECORATION_INVENTORY_H
#define GUARD_DECORATION_INVENTORY_H

struct DecorationInventory
{
    u8 *items;
    u8 size;
};

extern const struct DecorationInventory gDecorationInventories[];

void ClearDecorationInventories(void);
s8 GetFirstEmptyDecorSlot(u8 category);
bool8 CheckHasDecoration(u8 decorationId);
bool8 DecorationAdd(u8 decorationId);
bool8 DecorationCheckSpace(u8 decorationId);
s8 DecorationRemove(u8 decorationId);
void CondenseDecorationsInCategory(u8 category);
u8 GetNumOwnedDecorationsInCategory(u8 category);
u8 GetNumOwnedDecorations(void);
#if DEBUG
void Debug_GiveAllDecorations(void);
#endif // DEBUG

#endif // GUARD_DECORATION_INVENTORY_H
