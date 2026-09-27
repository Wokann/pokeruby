#include "global.h"
#include "menu_cursor.h"
#include "palette.h"
#include "sprite.h"

EWRAM_DATA static u16 sMenuCursorPalette[0x10] = {};
EWRAM_DATA static struct Subsprite sMenuCursorSubsprites[10] = {0};
EWRAM_DATA static u8 sOutlineCursorSpriteId = 0;
EWRAM_DATA static u8 sOutlineCursorWindowSpriteId = 0;
EWRAM_DATA static u8 sBlendedOutlineCursorSpriteId = 0;
EWRAM_DATA static u8 sWasObjWindowEnabled = 0;
EWRAM_DATA static u8 sSavedWinOutHigh = 0;

#if ENGLISH
#include "data/menu_cursor_en.h"
#elif GERMAN
#include "data/menu_cursor_de.h"
#endif // ENGLISH/GERMAN

void InitOutlineCursorState(void)
{
    sOutlineCursorSpriteId = 0x40;
    sOutlineCursorWindowSpriteId = 0x40;
    sBlendedOutlineCursorSpriteId = 0x40;
    sWasObjWindowEnabled = 0;
    sSavedWinOutHigh = 0;
}

u8 CreateOutlineCursor(u8 subpriority, u16 paletteTag, u8 tileIndex, u16 color, u8 width)
{
    int templateIndex;
    struct Sprite *sprite;

    if (sOutlineCursorSpriteId != 0x40 || sOutlineCursorWindowSpriteId != 0x40)
        DestroyMenuCursor();

    templateIndex = 1;
    if (paletteTag == 0xFFFF)
    {
        sMenuCursorPalette[tileIndex & 0xF] = color;
        if (LoadSpritePalette(&gOutlineCursorSpritePalette) != 0xFF)
        {
            paletteTag = 0xFFF0;
            templateIndex = 0;
        }
    }

    LoadSpriteSheetDeferred(&gOutlineCursorSpriteSheets[tileIndex & 0xF]);
    sOutlineCursorSpriteId = CreateSprite(&gOutlineCursorSpriteTemplates[templateIndex], 0, 160, subpriority);
    sOutlineCursorWindowSpriteId = CreateSprite(&gOutlineCursorSpriteTemplates[2], 0, 160, subpriority);
    if (sOutlineCursorSpriteId != 0x40)
    {
        sprite = &gSprites[sOutlineCursorSpriteId];
        if (paletteTag == 0xFFFF)
            sprite->oam.paletteNum = 0;
        else
            sprite->oam.paletteNum = IndexOfSpritePaletteTag(paletteTag);
    }
    if (sOutlineCursorWindowSpriteId != 0x40)
    {
        sprite = &gSprites[sOutlineCursorWindowSpriteId];
        if (paletteTag == 0xFFFF)
            sprite->oam.paletteNum = 0;
        else
            sprite->oam.paletteNum = IndexOfSpritePaletteTag(paletteTag);

        if (!(REG_DISPCNT & (DISPCNT_WIN0_ON | DISPCNT_WIN1_ON)))
            *(u8 *)(REG_ADDR_WINOUT) |= 0x1F;
        sWasObjWindowEnabled = REG_DISPCNT >> 0xF;
        sSavedWinOutHigh = *(u8 *)(REG_BASE + REG_OFFSET_WINOUT + 1);
        REG_DISPCNT |= DISPCNT_OBJWIN_ON;
        *(u8 *)(REG_ADDR_WINOUT + 1) = 0x10;
    }
    SetOutlineCursorWidth(width);
    return sOutlineCursorSpriteId;
}

// unused
u8 CreateOutlineCursorWithPaletteNum(u8 subpriority, u8 paletteNum, u8 tileIndex, u8 width)
{
    u8 result;
    struct Sprite *spr;

    result = CreateOutlineCursor(subpriority, 0, tileIndex, 0, width);
    if (result != 0x40)
    {
        spr = &gSprites[sOutlineCursorSpriteId];
        spr->oam.paletteNum = paletteNum;
    }
    return result;
}

u8 CreateOutlineCursorWithPaletteColor(u8 subpriority, u16 color, u8 width)
{
    u16 i;
    u8 paletteNum = 0;
    u16 tileIndex = 0xF;

    for (i = 0; i <= 0xFF; i++)
    {
        if (gPlttBufferUnfaded[i] == color)
        {
            paletteNum = (u8)(i >> 4);
            tileIndex = i & 0xF;
        }
    }

    return CreateOutlineCursorWithPaletteNum(subpriority, paletteNum, tileIndex, width);
}

void DestroyMenuCursor(void)
{
    if (sOutlineCursorSpriteId != 0x40)
    {
        LoadTilesForSpriteSheet(&gOutlineCursorSpriteSheets[0]);
        DestroySpriteAndFreeResources(&gSprites[sOutlineCursorSpriteId]);
        sOutlineCursorSpriteId = 0x40;
    }

    if (sOutlineCursorWindowSpriteId != 0x40)
    {
        DestroySpriteAndFreeResources(&gSprites[sOutlineCursorWindowSpriteId]);
        sOutlineCursorWindowSpriteId = 0x40;
        if (!sWasObjWindowEnabled)
            REG_DISPCNT &= ~DISPCNT_OBJWIN_ON;
        *(u8 *)(REG_BASE + REG_OFFSET_WINOUT + 1) = sSavedWinOutHigh;
    }

    return;
}

void SetOutlineCursorPosition(u8 x, u8 y)
{
    struct Sprite *spr;

    if (sOutlineCursorSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorSpriteId];
        spr->invisible = FALSE;
        spr->centerToCornerVecX = 0;
        spr->centerToCornerVecY = 0;
        spr->x = x;
        spr->y = y;
    }

    if (sOutlineCursorWindowSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorWindowSpriteId];
        spr->invisible = FALSE;
        spr->centerToCornerVecX = 0;
        spr->centerToCornerVecY = 0;
        spr->x = x;
        spr->y = y;
    }

    return;
}

void HideOutlineCursor(void)
{
    struct Sprite *spr;

    if (sOutlineCursorSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorSpriteId];
        spr->invisible = TRUE;
    }

    if (sOutlineCursorWindowSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorWindowSpriteId];
        spr->invisible = TRUE;
    }

    return;
}

#if ENGLISH
#ifdef NONMATCHING
// Fix pls
void SetOutlineCursorWidth(u8 a)
{
    u8 r7;
    struct Subsprite *r4 = &sMenuCursorSubsprites[0];
    s16 r2 = -1;
    s32 _a = a;
    s16 r5;
    s16 i;

    *r4 = (struct Subsprite){.x = 0, .y = 0, .shape = 2, .size = 0, .tileOffset = 0, .priority = 0};
    r4->x = r2;
    r4++;
    r7 = 1;
    r2 = 1;
    r5 = a;
    i = r5;
    while ((i -= r2) >= 8)
    {
        if (i > 0x1F)
        {
            *r4 = gUnknown_0842F780;
            r4->x = r2;
            r2 += 32;
            r5 = a;
        }
        //_0814A9D4
        else
        {
            r5 = a;
            if (_a > 0x27 && i > 8)
            {
                *r4 = gUnknown_0842F780;
                r4->x = (r2 - 32) + (i & ~7);
                r2 += i & 0x18;
            }
            //_0814AA0A
            else
            {
                *r4 = gUnknown_0842F788;
                r4->x = r2;
                r2 += 8;
            }
        }
        //_0814AA20
        r4++;
        r7++;
        i = r5;
    }
    //_0814AA3A
    *r4 = gUnknown_0842F790;
    r4->x = r2 - 7 + i;
    r7++;
    if (sOutlineCursorSpriteId != 64)
        SetSubspriteTables(&gSprites[sOutlineCursorSpriteId], gDynamicOutlineCursorSubspriteTables + r7);
    if (sOutlineCursorWindowSpriteId != 64)
        SetSubspriteTables(&gSprites[sOutlineCursorWindowSpriteId], gDynamicOutlineCursorSubspriteTables + r7);
}
#else
NAKED
void SetOutlineCursorWidth(u8 a1)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    sub sp, 0x4\n\
    lsls r0, 24\n\
    ldr r4, _0814A9C4\n\
    ldr r2, _0814A9C8\n\
    lsrs r0, 24\n\
    str r0, [sp]\n\
    movs r0, 0\n\
    movs r1, 0\n\
    movs r1, 0x2\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    strh r2, [r4]\n\
    adds r4, 0x8\n\
    movs r7, 0x1\n\
    movs r2, 0x1\n\
    ldr r1, [sp]\n\
    subs r0, r1, 0x1\n\
    lsls r0, 16\n\
    lsrs r3, r0, 16\n\
    asrs r0, 16\n\
    cmp r0, 0x7\n\
    ble _0814AA3A\n\
    ldr r0, _0814A9CC\n\
    mov r12, r0\n\
    mov r8, r1\n\
    movs r1, 0x8\n\
    negs r1, r1\n\
    mov r10, r1\n\
    ldr r5, _0814A9D0\n\
    mov r9, r5\n\
_0814A99E:\n\
    lsls r0, r3, 16\n\
    asrs r3, r0, 16\n\
    cmp r3, 0x1F\n\
    ble _0814A9D4\n\
    mov r6, r12\n\
    ldr r0, [r6]\n\
    ldr r1, [r6, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    strh r2, [r4]\n\
    lsls r0, r2, 16\n\
    movs r1, 0x80\n\
    lsls r1, 14\n\
    adds r0, r1\n\
    lsrs r2, r0, 16\n\
    ldr r3, [sp]\n\
    lsls r5, r3, 16\n\
    b _0814AA20\n\
    .align 2, 0\n\
_0814A9C4: .4byte sMenuCursorSubsprites\n\
_0814A9C8: .4byte 0x0000ffff\n\
_0814A9CC: .4byte gUnknown_0842F780\n\
_0814A9D0: .4byte gUnknown_0842F788\n\
_0814A9D4:\n\
    ldr r6, [sp]\n\
    lsls r5, r6, 16\n\
    mov r0, r8\n\
    cmp r0, 0x27\n\
    ble _0814AA0A\n\
    cmp r3, 0x8\n\
    ble _0814AA0A\n\
    mov r6, r12\n\
    ldr r0, [r6]\n\
    ldr r1, [r6, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    lsls r1, r2, 16\n\
    asrs r1, 16\n\
    adds r2, r1, 0\n\
    subs r2, 0x20\n\
    adds r0, r3, 0\n\
    mov r6, r10\n\
    ands r0, r6\n\
    adds r2, r0\n\
    strh r2, [r4]\n\
    movs r0, 0x18\n\
    ands r0, r3\n\
    adds r1, r0\n\
    lsls r1, 16\n\
    lsrs r2, r1, 16\n\
    b _0814AA20\n\
_0814AA0A:\n\
    mov r3, r9\n\
    ldr r0, [r3]\n\
    ldr r1, [r3, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    strh r2, [r4]\n\
    lsls r0, r2, 16\n\
    movs r6, 0x80\n\
    lsls r6, 12\n\
    adds r0, r6\n\
    lsrs r2, r0, 16\n\
_0814AA20:\n\
    adds r4, 0x8\n\
    adds r0, r7, 0x1\n\
    lsls r0, 24\n\
    lsrs r7, r0, 24\n\
    asrs r1, r5, 16\n\
    lsls r0, r2, 16\n\
    asrs r0, 16\n\
    subs r1, r0\n\
    lsls r1, 16\n\
    lsrs r3, r1, 16\n\
    asrs r1, 16\n\
    cmp r1, 0x7\n\
    bgt _0814A99E\n\
_0814AA3A:\n\
    ldr r5, _0814AAA8\n\
    ldr r0, [r5]\n\
    ldr r1, [r5, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    lsls r1, r2, 16\n\
    asrs r1, 16\n\
    subs r1, 0x7\n\
    lsls r0, r3, 16\n\
    asrs r0, 16\n\
    adds r0, r1\n\
    strh r0, [r4]\n\
    adds r0, r7, 0x1\n\
    lsls r0, 24\n\
    lsrs r7, r0, 24\n\
    ldr r6, _0814AAAC\n\
    ldrb r0, [r6]\n\
    cmp r0, 0x40\n\
    beq _0814AA78\n\
    adds r1, r0, 0\n\
    lsls r0, r1, 4\n\
    adds r0, r1\n\
    lsls r0, 2\n\
    ldr r1, _0814AAB0\n\
    adds r2, r0, r1\n\
    lsls r1, r7, 3\n\
    ldr r0, _0814AAB4\n\
    adds r1, r0\n\
    adds r0, r2, 0\n\
    bl SetSubspriteTables\n\
_0814AA78:\n\
    ldr r1, _0814AAB8\n\
    ldrb r0, [r1]\n\
    cmp r0, 0x40\n\
    beq _0814AA98\n\
    adds r1, r0, 0\n\
    lsls r0, r1, 4\n\
    adds r0, r1\n\
    lsls r0, 2\n\
    ldr r1, _0814AAB0\n\
    adds r2, r0, r1\n\
    lsls r1, r7, 3\n\
    ldr r0, _0814AAB4\n\
    adds r1, r0\n\
    adds r0, r2, 0\n\
    bl SetSubspriteTables\n\
_0814AA98:\n\
    add sp, 0x4\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_0814AAA8: .4byte gUnknown_0842F790\n\
_0814AAAC: .4byte sOutlineCursorSpriteId\n\
_0814AAB0: .4byte gSprites\n\
_0814AAB4: .4byte gDynamicOutlineCursorSubspriteTables\n\
_0814AAB8: .4byte sOutlineCursorWindowSpriteId\n\
    .syntax divided\n");
}
#endif
#elif GERMAN
NAKED
void SetOutlineCursorWidth(u8 a1)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    sub sp, 0x4\n\
    lsls r0, 24\n\
    ldr r4, _0814A9C4 @ =sMenuCursorSubsprites\n\
    ldr r2, _0814A9C8 @ =0x0000ffff\n\
    lsrs r0, 24\n\
    str r0, [sp]\n\
    ldr r0, _0814A9CC @ =gUnknown_0842F780\n\
    ldr r1, [r0, 0x4]\n\
    ldr r0, [r0]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    strh r2, [r4]\n\
    adds r4, 0x8\n\
    movs r7, 0x1\n\
    movs r2, 0x1\n\
    ldr r1, [sp]\n\
    subs r0, r1, 0x1\n\
    lsls r0, 16\n\
    lsrs r3, r0, 16\n\
    asrs r0, 16\n\
    cmp r0, 0x7\n\
    ble _0814AA3E\n\
    ldr r0, _0814A9D0 @ =gUnknown_0842F788\n\
    mov r12, r0\n\
    mov r8, r1\n\
    movs r1, 0x8\n\
    negs r1, r1\n\
    mov r10, r1\n\
    ldr r5, _0814A9D4 @ =gUnknown_0842F790\n\
    mov r9, r5\n\
_0814A99E:\n\
    lsls r0, r3, 16\n\
    asrs r3, r0, 16\n\
    cmp r3, 0x1F\n\
    ble _0814A9D8\n\
    mov r6, r12\n\
    ldr r0, [r6]\n\
    ldr r1, [r6, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    strh r2, [r4]\n\
    lsls r0, r2, 16\n\
    movs r1, 0x80\n\
    lsls r1, 14\n\
    adds r0, r1\n\
    lsrs r2, r0, 16\n\
    ldr r3, [sp]\n\
    lsls r5, r3, 16\n\
    b _0814AA24\n\
    .align 2, 0\n\
_0814A9C4: .4byte sMenuCursorSubsprites\n\
_0814A9C8: .4byte 0x0000ffff\n\
_0814A9CC: .4byte gUnknown_0842F780\n\
_0814A9D0: .4byte gUnknown_0842F788\n\
_0814A9D4: .4byte gUnknown_0842F790\n\
_0814A9D8:\n\
    ldr r6, [sp]\n\
    lsls r5, r6, 16\n\
    mov r0, r8\n\
    cmp r0, 0x27\n\
    ble _0814AA0E\n\
    cmp r3, 0x8\n\
    ble _0814AA0E\n\
    mov r6, r12\n\
    ldr r0, [r6]\n\
    ldr r1, [r6, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    lsls r1, r2, 16\n\
    asrs r1, 16\n\
    adds r2, r1, 0\n\
    subs r2, 0x20\n\
    adds r0, r3, 0\n\
    mov r6, r10\n\
    ands r0, r6\n\
    adds r2, r0\n\
    strh r2, [r4]\n\
    movs r0, 0x18\n\
    ands r0, r3\n\
    adds r1, r0\n\
    lsls r1, 16\n\
    lsrs r2, r1, 16\n\
    b _0814AA24\n\
_0814AA0E:\n\
    mov r3, r9\n\
    ldr r0, [r3]\n\
    ldr r1, [r3, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    strh r2, [r4]\n\
    lsls r0, r2, 16\n\
    movs r6, 0x80\n\
    lsls r6, 12\n\
    adds r0, r6\n\
    lsrs r2, r0, 16\n\
_0814AA24:\n\
    adds r4, 0x8\n\
    adds r0, r7, 0x1\n\
    lsls r0, 24\n\
    lsrs r7, r0, 24\n\
    asrs r1, r5, 16\n\
    lsls r0, r2, 16\n\
    asrs r0, 16\n\
    subs r1, r0\n\
    lsls r1, 16\n\
    lsrs r3, r1, 16\n\
    asrs r1, 16\n\
    cmp r1, 0x7\n\
    bgt _0814A99E\n\
_0814AA3E:\n\
    ldr r5, _0814AAAC @ =gUnknown_0842F798\n\
    ldr r0, [r5]\n\
    ldr r1, [r5, 0x4]\n\
    str r0, [r4]\n\
    str r1, [r4, 0x4]\n\
    lsls r1, r2, 16\n\
    asrs r1, 16\n\
    subs r1, 0x7\n\
    lsls r0, r3, 16\n\
    asrs r0, 16\n\
    adds r0, r1\n\
    strh r0, [r4]\n\
    adds r0, r7, 0x1\n\
    lsls r0, 24\n\
    lsrs r7, r0, 24\n\
    ldr r6, _0814AAB0 @ =sOutlineCursorSpriteId\n\
    ldrb r0, [r6]\n\
    cmp r0, 0x40\n\
    beq _0814AA7C\n\
    adds r1, r0, 0\n\
    lsls r0, r1, 4\n\
    adds r0, r1\n\
    lsls r0, 2\n\
    ldr r1, _0814AAB4 @ =gSprites\n\
    adds r2, r0, r1\n\
    lsls r1, r7, 3\n\
    ldr r0, _0814AAB8 @ =gDynamicOutlineCursorSubspriteTables\n\
    adds r1, r0\n\
    adds r0, r2, 0\n\
    bl SetSubspriteTables\n\
_0814AA7C:\n\
    ldr r1, _0814AABC @ =sOutlineCursorWindowSpriteId\n\
    ldrb r0, [r1]\n\
    cmp r0, 0x40\n\
    beq _0814AA9C\n\
    adds r1, r0, 0\n\
    lsls r0, r1, 4\n\
    adds r0, r1\n\
    lsls r0, 2\n\
    ldr r1, _0814AAB4 @ =gSprites\n\
    adds r2, r0, r1\n\
    lsls r1, r7, 3\n\
    ldr r0, _0814AAB8 @ =gDynamicOutlineCursorSubspriteTables\n\
    adds r1, r0\n\
    adds r0, r2, 0\n\
    bl SetSubspriteTables\n\
_0814AA9C:\n\
    add sp, 0x4\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_0814AAAC: .4byte gUnknown_0842F798\n\
_0814AAB0: .4byte sOutlineCursorSpriteId\n\
_0814AAB4: .4byte gSprites\n\
_0814AAB8: .4byte gDynamicOutlineCursorSubspriteTables\n\
_0814AABC: .4byte sOutlineCursorWindowSpriteId\n\
    .syntax divided\n");
}
#endif

void SetOutlineCursorCallback(void (*callback)(struct Sprite *))
{
    struct Sprite *spr;

    if (sOutlineCursorSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorSpriteId];
        spr->callback = callback;
    }

    if (sOutlineCursorWindowSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorWindowSpriteId];
        spr->callback = callback;
    }

    return;
}

void UpdateOutlineCursorPaletteByColor(u16 color)
{
    struct Sprite *spr;
    u8 paletteNum;
    u8 tileIndex;
    u16 i;

    if (sOutlineCursorSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorSpriteId];
        if (spr->template->paletteTag == 0xFFFF)
        {
            for (paletteNum = 0, tileIndex = 0xF, i = 0; i <= 0xFF; i++)
            {
                if (gPlttBufferUnfaded[i] == color)
                {
                    paletteNum = i >> 4;
                    tileIndex = i & 0xF;
                }
            }
            spr->oam.paletteNum = paletteNum;
            RequestSpriteSheetCopy(&gOutlineCursorSpriteSheets[tileIndex & 0xF]);
        }
    }
    return;
}

void DestroyOutlineCursorWindowSprite(void)
{
    struct Sprite *spr;

    if (sOutlineCursorWindowSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorWindowSpriteId];
        FreeSpriteOamMatrix(spr);
        DestroySprite(spr);
        sOutlineCursorWindowSpriteId = 0x40;

        if (!sWasObjWindowEnabled)
            REG_DISPCNT &= ~DISPCNT_OBJWIN_ON;
        *(u8 *)(REG_ADDR_WINOUT + 1) = sSavedWinOutHigh;
    }
    return;
}

void SetOutlineCursorWindowSubsprites(int index)
{
    struct Sprite *spr;

    CpuCopy16(gUnknown_0842F5BC[index], &sMenuCursorSubsprites, 80);

    if (sOutlineCursorSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorSpriteId];
        SetSubspriteTables(spr, &gOutlineCursorWindowSubspriteTable[index]);
    }
    if (sOutlineCursorWindowSpriteId != 0x40)
    {
        spr = &gSprites[sOutlineCursorWindowSpriteId];
        SetSubspriteTables(spr, &gOutlineCursorWindowSubspriteTable[index]);
    }
    return;
}

u8 CreateBlendedOutlineCursor(u8 subpriority, u16 paletteTag, u8 tileIndex, u16 color, u8 width)
{
    int templateIndex;
    struct Sprite *sprite;

    if (sBlendedOutlineCursorSpriteId != 0x40)
        DestroyBlendedOutlineCursor();

    templateIndex = 1;

    if (paletteTag == 0xFFFF)
    {
        sMenuCursorPalette[tileIndex & 0xF] = color;
        if (LoadSpritePalette(&gBlendedOutlineCursorSpritePalette) != 0xFF )
        {
            paletteTag = 0xFFF1;
            templateIndex = 0;
        }
    }

    LoadSpriteSheetDeferred(&gBlendedOutlineCursorSpriteSheets[tileIndex & 0xF]);
#if ENGLISH
    sBlendedOutlineCursorSpriteId = CreateSprite(&gBlendedOutlineCursorSpriteTemplates[templateIndex], 0, 160, subpriority);
#elif GERMAN
    sBlendedOutlineCursorSpriteId = CreateSprite(&gBlendedOutlineCursorSpriteTemplates[templateIndex], 0, 161, subpriority);
#endif

    if (sBlendedOutlineCursorSpriteId != 0x40)
    {
        sprite = &gSprites[sBlendedOutlineCursorSpriteId];

        if (paletteTag == 0xFFFF)
            sprite->oam.paletteNum = 0;
        else
            sprite->oam.paletteNum = IndexOfSpritePaletteTag(paletteTag);
    }
    SetBlendedOutlineCursorWidth(width);

    return sBlendedOutlineCursorSpriteId;
}

void DestroyBlendedOutlineCursor(void)
{
    if (sBlendedOutlineCursorSpriteId != 0x40)
    {
        LoadTilesForSpriteSheet(&gBlendedOutlineCursorSpriteSheets[0]);
        DestroySpriteAndFreeResources(&gSprites[sBlendedOutlineCursorSpriteId]);
        sBlendedOutlineCursorSpriteId = 0x40;
    }
    return;
}

void SetBlendedOutlineCursorPosition(u8 x, u8 y)
{
    struct Sprite *spr;
    if (sBlendedOutlineCursorSpriteId != 0x40)
    {
        spr = &gSprites[sBlendedOutlineCursorSpriteId];
        spr->invisible = FALSE;
        spr->centerToCornerVecX = 0;
        spr->centerToCornerVecY = 0;
        spr->x = x;
        spr->y = y;
    }
    return;
}

void HideBlendedOutlineCursor()
{
    struct Sprite *spr;
    if (sBlendedOutlineCursorSpriteId != 0x40)
    {
        spr = &gSprites[sBlendedOutlineCursorSpriteId];
        spr->invisible = TRUE;
    }
    return;
}

void SetBlendedOutlineCursorWidth(u8 widthIndex)
{
    if (widthIndex > 0x12)
        widthIndex = 0;

    if (sBlendedOutlineCursorSpriteId != 0x40)
        SetSubspriteTables(&gSprites[sBlendedOutlineCursorSpriteId], &gBlendedOutlineCursorSubspriteTables[widthIndex]);
    return;
}

#if GERMAN
void nullsub_814B200(void)
{
}
#endif
