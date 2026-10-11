#ifndef GUARD_SHOP_H
#define GUARD_SHOP_H

extern struct ItemSlot gMartPurchaseHistory[3];

void CreatePokemartMenu(const u16 *itemsForSale);
void CreateDecorationShop1Menu(const u16 *itemsForSale);
void CreateDecorationShop2Menu(const u16 *itemsForSale);
void Shop_RunExitSellMenuTask(u8 taskId);
void MapPostLoadHook_ReturnToShopMenu(void);

#endif // GUARD_SHOP_H
