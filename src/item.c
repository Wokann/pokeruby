#include "global.h"
#include "constants/hold_effects.h"
#include "item.h"
#include "constants/items.h"
#include "item_menu.h"
#include "item_use.h"
#include "berry.h"
#include "string_util.h"
#include "strings.h"

extern u8 gCurSelectedItemSlotIndex;
extern struct BagPocket gBagPockets[POCKETS_COUNT];

#if ENGLISH
#include "data/text/item_descriptions.h"
#include "data/items.h"
#elif GERMAN
#include "data/text/item_descriptions_de.h"
#include "data/items_de.h"
#endif

static void CompactPCItems(void);

void CopyItemName(u16 itemId, u8 *dst)
{
    if (itemId == ITEM_ENIGMA_BERRY)
    {
        StringCopy(dst, GetBerryInfo(ITEM_TO_BERRY(ITEM_ENIGMA_BERRY))->name);
        StringAppend(dst, gText_Berry2);
    }
    else
    {
        StringCopy(dst, GetItemName(itemId));
    }
}

//Unreferenced
s8 CountUsedBagPocketSlots(u8 pocket)
{
    u8 i;

    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == 0)
            return i;
    }
    return -1;
}

bool8 IsBagPocketNonEmpty(u8 pocket)
{
    u8 i;

    for (i = 0; i < gBagPockets[pocket - 1].capacity; i++)
    {
        if (gBagPockets[pocket - 1].itemSlots[i].itemId != 0)
            return TRUE;
    }
    return FALSE;
}

bool8 CheckBagHasItem(u16 itemId, u16 count)
{
    u8 i;
    u8 pocket;

    if (GetItemPocket(itemId) == 0)
        return FALSE;
    pocket = GetItemPocket(itemId) - 1;
    //Check for item slots that contain the item
    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
        {
            //Does this item slot contain enough of the item?
            if (gBagPockets[pocket].itemSlots[i].quantity >= count)
                return TRUE;
            count -= gBagPockets[pocket].itemSlots[i].quantity;
            //Does this item slot and all previous slots contain enough of the item?
            if (count == 0)
                return TRUE;
        }
    }
    return FALSE;
}

bool8 CheckBagHasSpace(u16 itemId, u16 count)
{
    u8 i;
    u8 pocket;
    u16 slotCapacity;

    if (GetItemPocket(itemId) == 0)
        return FALSE;
    pocket = GetItemPocket(itemId) - 1;
    if (pocket != POCKET_BERRIES - 1)
        slotCapacity = 99;
    else
        slotCapacity = 999;

    //Check space in any existing item slots that already contain this item
    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
        {
            if (gBagPockets[pocket].itemSlots[i].quantity + count <= slotCapacity)
                return TRUE;
            if (pocket == POCKET_TM_HM - 1 || pocket == POCKET_BERRIES - 1)
                return FALSE;
            count -= slotCapacity - gBagPockets[pocket].itemSlots[i].quantity;
            if (count == 0)
                return TRUE;
        }
    }

    //Check space in empty item slots
    if (count > 0)
    {
        for (i = 0; i < gBagPockets[pocket].capacity; i++)
        {
            if (gBagPockets[pocket].itemSlots[i].itemId == 0)
            {
                if (count <= slotCapacity)
                    return TRUE;
                else
                    count -= slotCapacity;
            }
        }
        if (count > 0)
            return FALSE; //No more item slots. The bag is full
    }

    return TRUE;
}

// This function matches if gBagPockets is declared non-const,
// but it should be fixed anyway.
bool8 AddBagItem(u16 itemId, u16 count)
{
    u8 i;
    u8 pocket;
    u16 slotCapacity;
    struct ItemSlot newItems[64];

    if (GetItemPocket(itemId) == 0)
        return FALSE;
    pocket = GetItemPocket(itemId) - 1;
    //Copy the bag pocket
    memcpy(newItems, gBagPockets[pocket].itemSlots, gBagPockets[pocket].capacity * sizeof(struct ItemSlot));
    if (pocket != POCKET_BERRIES - 1)
        slotCapacity = 99;
    else
        slotCapacity = 999;

    //Use any item slots that already contain this item
    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (newItems[i].itemId == itemId)
        {
            if (newItems[i].quantity + count <= slotCapacity)
            {
                newItems[i].quantity += count;
                //Copy pocket back into the bag.
                memcpy(gBagPockets[pocket].itemSlots, newItems, gBagPockets[pocket].capacity * sizeof(struct ItemSlot));
                return TRUE;
            }
            if (pocket == POCKET_TM_HM - 1 || pocket == POCKET_BERRIES - 1)
                return FALSE;
            count -= slotCapacity - newItems[i].quantity;
            newItems[i].quantity = slotCapacity;
            if (count == 0)
                goto copy_items;
        }
    }

    //Put any remaining items into new item slots.
    if (count > 0)
    {
        for (i = 0; i < gBagPockets[pocket].capacity; i++)
        {
            if (newItems[i].itemId == 0)
            {
                newItems[i].itemId = itemId;
                if (count <= slotCapacity)
                {
                    newItems[i].quantity = count;
                    goto copy_items;
                }
                count -= slotCapacity;
                newItems[i].quantity = slotCapacity;
            }
        }
        if (count > 0)
            return FALSE;   //No more empty item slots. The bag is full.
    }

  copy_items:
    //Copy pocket back into the bag.
    memcpy(gBagPockets[pocket].itemSlots, newItems, gBagPockets[pocket].capacity * sizeof(struct ItemSlot));
    return TRUE;
}

bool8 RemoveBagItem(u16 itemId, u16 count)
{
    u8 i;
    u8 pocket;
    u16 totalQuantity = 0;

    if (GetItemPocket(itemId) == 0 || itemId == 0)
        return FALSE;
    pocket = GetItemPocket(itemId) - 1;

    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
            totalQuantity += gBagPockets[pocket].itemSlots[i].quantity;
    }
    if (totalQuantity < count)
        return FALSE;   //We don't have enough of the item

    if (gBagPockets[pocket].capacity > gCurSelectedItemSlotIndex
     && gBagPockets[pocket].itemSlots[gCurSelectedItemSlotIndex].itemId == itemId)
    {
        if (gBagPockets[pocket].itemSlots[gCurSelectedItemSlotIndex].quantity >= count)
        {
            gBagPockets[pocket].itemSlots[gCurSelectedItemSlotIndex].quantity -= count;
            count = 0;
        }
        else
        {
            count -= gBagPockets[pocket].itemSlots[gCurSelectedItemSlotIndex].quantity;
            gBagPockets[pocket].itemSlots[gCurSelectedItemSlotIndex].quantity = 0;
        }
        if (gBagPockets[pocket].itemSlots[gCurSelectedItemSlotIndex].quantity == 0)
            gBagPockets[pocket].itemSlots[gCurSelectedItemSlotIndex].itemId = 0;
        if (count == 0)
            return TRUE;
    }

    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
        {
            if (gBagPockets[pocket].itemSlots[i].quantity >= count)
            {
                gBagPockets[pocket].itemSlots[i].quantity -= count;
                count = 0;
            }
            else
            {
                count -= gBagPockets[pocket].itemSlots[i].quantity;
                gBagPockets[pocket].itemSlots[i].quantity = 0;
            }
            if (gBagPockets[pocket].itemSlots[i].quantity == 0)
                gBagPockets[pocket].itemSlots[i].itemId = 0;
            if (count == 0)
                return TRUE;
        }
    }
    return TRUE;
}

u8 GetPocketByItemId(u16 itemId)
{
    return GetItemPocket(itemId);
}

void ClearItemSlots(struct ItemSlot *itemSlots, u8 itemCount)
{
    u16 i;

    for (i = 0; i < itemCount; i++)
    {
        itemSlots[i].itemId = 0;
        itemSlots[i].quantity = 0;
    }
}

static s32 FindFreePCItemSlot(void)
{
    s8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1.pcItems[i].itemId == 0)
            return i;
    }
    return -1;
}

u8 CountUsedPCItemSlots(void)
{
    u8 usedSlots = 0;
    u8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1.pcItems[i].itemId != 0)
            usedSlots++;
    }
    return usedSlots;
}

bool8 CheckPCHasItem(u16 itemId, u16 count)
{
    u8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1.pcItems[i].itemId == itemId && gSaveBlock1.pcItems[i].quantity >= count)
            return TRUE;
    }
    return FALSE;
}

bool8 AddPCItem(u16 itemId, u16 count)
{
    u8 i;
    s8 freeSlot;
    struct ItemSlot newItems[PC_ITEMS_COUNT];

    //Copy PC items
    memcpy(newItems, gSaveBlock1.pcItems, sizeof(newItems));

    //Use any item slots that already contain this item
    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (newItems[i].itemId == itemId)
        {
            if (newItems[i].quantity + count <= 999)
            {
                newItems[i].quantity += count;
                memcpy(gSaveBlock1.pcItems, newItems, sizeof(gSaveBlock1.pcItems));
                return TRUE;
            }
            count += newItems[i].quantity - 999;
            newItems[i].quantity = 999;
            if (count == 0)
            {
                memcpy(gSaveBlock1.pcItems, newItems, sizeof(gSaveBlock1.pcItems));
                return TRUE;
            }
        }
    }

    //Put any remaining items into a new item slot.
    if (count > 0)
    {
        freeSlot = FindFreePCItemSlot();
        if (freeSlot == -1)
            return FALSE;
        newItems[freeSlot].itemId = itemId;
        newItems[freeSlot].quantity = count;
    }

    //Copy items back to the PC
    memcpy(gSaveBlock1.pcItems, newItems, sizeof(gSaveBlock1.pcItems));
    return TRUE;
}

void RemovePCItem(u8 index, u16 count)
{
    gSaveBlock1.pcItems[index].quantity -= count;
    if (gSaveBlock1.pcItems[index].quantity == 0)
    {
        gSaveBlock1.pcItems[index].itemId = 0;
        CompactPCItems();
    }
}

static void CompactPCItems(void)
{
    u16 i;
    u16 j;

    for (i = 0; i < PC_ITEMS_COUNT - 1; i++)
    {
        for (j = i + 1; j <= PC_ITEMS_COUNT - 1; j++)
        {
            if (gSaveBlock1.pcItems[i].itemId == 0)
            {
                struct ItemSlot temp = gSaveBlock1.pcItems[i];
                gSaveBlock1.pcItems[i] = gSaveBlock1.pcItems[j];
                gSaveBlock1.pcItems[j] = temp;
            }
        }
    }
}

void SwapRegisteredBike(void)
{
    switch (gSaveBlock1.registeredItem)
    {
    case ITEM_MACH_BIKE:
        gSaveBlock1.registeredItem = ITEM_ACRO_BIKE;
        break;
    case ITEM_ACRO_BIKE:
        gSaveBlock1.registeredItem = ITEM_MACH_BIKE;
        break;
    }
}

static u16 SanitizeItemId(u16 itemId)
{
    if (itemId >= ARRAY_COUNT(gItems))
        return 0;
    else
        return itemId;
}

const u8 *GetItemName(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].name;
}

u16 GetItemId(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].itemId;
}

u16 GetItemPrice(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].price;
}

u8 GetItemHoldEffect(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].holdEffect;
}

u8 GetItemHoldEffectParam(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].holdEffectParam;
}

const u8 *GetItemDescription(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].description;
}

bool32 ItemId_CopyDescription(u8 *dest, u32 itemId, u32 textLine)
{
    u32 curTextLine = textLine + 1;
    const u8 *description = gItems[SanitizeItemId(itemId)].description;
    u8 *str = dest;

    for (;;)
    {
        if (*description == 0xFF || *description == 0xFE)
        {
            curTextLine--;
            if (curTextLine == 0)
            {
                *str = 0xFF;
                return TRUE;
            }
            if (*description == 0xFF)
                return FALSE;
            str = dest;
            description++;
        }
        else
            *(str++) = *(description++);
    }
}

u8 GetItemImportance(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].importance;
}

// unused
u8 GetItemRegistrability(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].registrability;
}

u8 GetItemPocket(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].pocket;
}

u8 GetItemType(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].type;
}

ItemUseFunc GetItemFieldFunc(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].fieldUseFunc;
}

u8 GetItemBattleUsage(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].battleUsage;
}

ItemUseFunc GetItemBattleFunc(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].battleUseFunc;
}

u8 GetItemSecondaryId(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].secondaryId;
}
