#include "global.h"
#include "item_menu.h"
#include "constants/items.h"
#include "link.h"
#include "mail.h"
#include "main.h"
#include "constants/maps.h"
#include "menu.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "text.h"

#define SCROLL_INDICATOR_PAL_TAG 6

static void SpriteCB_VerticalScrollIndicator(struct Sprite *sprite);

static EWRAM_DATA u8 sVerticalScrollIndicatorIds[4] = {0};
static EWRAM_DATA struct YesNoFuncTable sYesNo = {0};

static TaskFunc sMessageNextTask;

static const u8 sSpriteGfx_VerticalScrollArrow_Up[] = INCBIN_U8("graphics/unknown_sprites/83E59A0/0.4bpp");

static const u8 sSpriteGfx_VerticalScrollArrow_Down[] = INCBIN_U8("graphics/unknown_sprites/83E59A0/1.4bpp");

static const u8 sSpriteGfx_HorizontalScrollArrow_Left[] = INCBIN_U8("graphics/unknown_sprites/83E59B0/0.4bpp");

static const u8 sSpriteGfx_HorizontalScrollArrow_Right[] = INCBIN_U8("graphics/unknown_sprites/83E59B0/1.4bpp");

static const u8 sSpriteGfx_DecorationMarker0[] = INCBIN_U8("graphics/unknown_sprites/83E59C0/0.4bpp");

static const u8 sSpriteGfx_DecorationMarker1[] = INCBIN_U8("graphics/unknown_sprites/83E59C0/1.4bpp");

static const u16 sScrollIndicatorPal[] = INCBIN_U16("graphics/interface/83E5948.gbapal");

static const struct SpritePalette sScrollIndicatorPalette = { sScrollIndicatorPal, SCROLL_INDICATOR_PAL_TAG };

static const struct OamData sOamData_DecorationMarker =
{
    .tileNum = 1
};

static const struct OamData sOamData_VerticalScrollArrow =
{
    .shape = ST_OAM_H_RECTANGLE,
    .tileNum = 1
};

static const struct OamData sOamData_HorizontalScrollArrow =
{
    .shape = ST_OAM_V_RECTANGLE,
    .tileNum = 1
};

static const union AnimCmd sAnim_Frame0[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Frame1[] =
{
    ANIMCMD_FRAME(1, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_TwoFrame[] =
{
    sAnim_Frame0,
    sAnim_Frame1,
};

static const struct SpriteFrameImage sSpriteImage_VerticalScrollArrow[] =
{
    { sSpriteGfx_VerticalScrollArrow_Up, 0x40 },
    { sSpriteGfx_VerticalScrollArrow_Down, 0x40 },
};

static const struct SpriteFrameImage sSpriteImage_HorizontalScrollArrow[] =
{
    { sSpriteGfx_HorizontalScrollArrow_Left, 0x40 },
    { sSpriteGfx_HorizontalScrollArrow_Right, 0x40 },
};

static const struct SpriteFrameImage sSpriteImage_DecorationMarker[] =
{
    { sSpriteGfx_DecorationMarker0, 0x20 },
    { sSpriteGfx_DecorationMarker1, 0x20 },
};

static const struct SpriteTemplate sSpriteTemplate_VerticalScrollArrow =
{
    .tileTag = 0xFFFF,
    .paletteTag = SCROLL_INDICATOR_PAL_TAG,
    .oam = &sOamData_VerticalScrollArrow,
    .anims = sAnims_TwoFrame,
    .images = sSpriteImage_VerticalScrollArrow,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_VerticalScrollIndicator,
};

static const struct SpriteTemplate sSpriteTemplate_HorizontalScrollArrow =
{
    .tileTag = 0xFFFF,
    .paletteTag = SCROLL_INDICATOR_PAL_TAG,
    .oam = &sOamData_HorizontalScrollArrow,
    .anims = sAnims_TwoFrame,
    .images = sSpriteImage_HorizontalScrollArrow,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_VerticalScrollIndicator,
};

static const struct SpriteTemplate sSpriteTemplate_DecorationMarker =
{
    .tileTag = 0xFFFF,
    .paletteTag = SCROLL_INDICATOR_PAL_TAG,
    .oam = &sOamData_DecorationMarker,
    .anims = sAnims_TwoFrame,
    .images = sSpriteImage_DecorationMarker,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

void ClearBGTilemapBuffers(void)
{
    memset(&gBGTilemapBuffers[1], 0, 0x800);
    memset(&gBGTilemapBuffers[2], 0, 0x800);
    memset(&gBGTilemapBuffers[3], 0, 0x800);
}

// display message box, fill box with tile if tile is not zero, print string
static void PrintMessage(const u8 *str, u16 tile)
{
    Menu_DisplayDialogueFrame();
    if (tile)
    {
        FillBgTilemapRect(&gBGTilemapBuffers[1][0], 2, 15, 26, 4, tile);
    }
    MenuPrintMessageDefaultCoords(str);
}

static void Task_ContinueTaskAfterMessagePrints(u8 taskId)
{
    if (Menu_UpdateWindowText() == TRUE)
    {
        sMessageNextTask(taskId);
    }
}

void DisplayItemMessageOnField(u8 taskId, const u8 *str, TaskFunc callback, u16 tile)
{
    PrintMessage(str, tile);
    sMessageNextTask = callback;
    gTasks[taskId].func = Task_ContinueTaskAfterMessagePrints;
}

static void Task_CallYesOrNoCallback(u8 taskId)
{
    switch (Menu_ProcessInputNoWrap_())
    {
    case 0:
        PlaySE(SE_SELECT);
        sYesNo.yesFunc(taskId);
        break;
    case 1:
    case -1:
        PlaySE(SE_SELECT);
        sYesNo.noFunc(taskId);
    }
}

void DoYesNoFuncWithChoice(u8 taskId, const struct YesNoFuncTable *funcTable)
{
    sYesNo = *funcTable;
    gTasks[taskId].func = Task_CallYesOrNoCallback;
}

static void PrintStringWithPalette(const u8 *str, u8 paletteNum, u8 left, u8 top)
{
    u8 paletteStr[] = _("{PALETTE 0}");

    if (paletteNum != 0xFF)
    {
        paletteStr[2] = paletteNum;
        StringCopy(gStringVar4, paletteStr);
        StringAppend(gStringVar4, str);
        paletteStr[2] = gMenuWindowPtr->paletteNum;
        StringAppend(gStringVar4, paletteStr);
    }
    else
    {
        StringCopy(gStringVar4, str);
    }

    Menu_PrintText(gStringVar4, left, top);
}

// unused
void PrintNumberWithPalette(s32 value, u8 paletteNum, u8 n, u8 mode, u8 left, u8 top)
{
    ConvertIntToDecimalStringN(gStringVar1, value, mode, n);
    if (paletteNum != 0xFF)
    {
        PrintStringWithPalette(gStringVar1, paletteNum, left, top);
    }
    else
    {
        Menu_PrintText(gStringVar1, left, top);
    }
}

// unused
void PrintTriangleCursorWithPalette(u8 left, u8 top, u8 paletteNum)
{
    u8 cursorStr[] = _("▶");
    PrintStringWithPalette(cursorStr, paletteNum, left, top);
}

u8 GetLRKeysPressed(void)
{
    if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR)
    {
        if (JOY_NEW(L_BUTTON))
        {
            return 1;
        }
        if (JOY_NEW(R_BUTTON))
        {
            return 2;
        }
    }

    return 0;
}

u8 GetLRKeysPressedAndHeld(void)
{
    if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR)
    {
        if (JOY_REPT(L_BUTTON))
        {
            return 1;
        }
        if (JOY_REPT(R_BUTTON))
        {
            return 2;
        }
    }

    return 0;
}

bool8 IsHoldingItemAllowed(u16 itemId)
{
    if (itemId != ITEM_ENIGMA_BERRY)
    {
        return TRUE;
    }

    if (!(gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_TRADE_CENTER) && gSaveBlock1.location.mapNum == MAP_NUM(MAP_TRADE_CENTER)))
    {
        return TRUE;
    }

    return FALSE;
}

bool8 IsWritingMailAllowed(u16 itemId)
{
    if (IsOverworldLinkActive() != TRUE)
    {
        return TRUE;
    }

    if (ItemIsMail(itemId) != TRUE)
    {
        return TRUE;
    }

    return FALSE;
}

bool8 MenuHelpers_IsLinkActive(void)
{
    if (IsOverworldLinkActive() == TRUE || gReceivedRemoteLinkPlayers == 1)
    {
        return TRUE;
    }

    return FALSE;
}

void ResetVramOamAndBgCntRegs(void)
{
    REG_DISPCNT = 0;
    REG_BG3CNT = 0;
    REG_BG2CNT = 0;
    REG_BG1CNT = 0;
    REG_BG0CNT = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    DmaFill16Large(3, 0, (u8 *)VRAM, 0x18000, 0x1000);
    DmaClear32(3, OAM, OAM_SIZE);
    DmaClear16(3, PLTT, PLTT_SIZE);
}

void ClearVideoCallbacks(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
}

void ClearVerticalScrollIndicatorPalettes(void)
{
    u8 i;

    for (i = 0; i < 4; i++)
    {
        sVerticalScrollIndicatorIds[i] = 0xFF;
    }

    FreeSpritePaletteByTag(SCROLL_INDICATOR_PAL_TAG);
    LoadSpritePalette(&sScrollIndicatorPalette);
}

void InitDecorationMarkerSpriteIds(u8 *data, u8 n)
{
    u8 i;

    for (i = 0; i < n; i++)
    {
        data[i] = 0xFF;
    }
}

void CreateDecorationMarkerSprite(u8 animNum, u8 *spriteId, s16 x, s16 y)
{
    if (animNum <= 5)
    {
        if (*spriteId == 0xFF)
        {
            *spriteId = CreateSprite(&sSpriteTemplate_DecorationMarker, x, y, 0);
        }
        animNum -= 4;
        StartSpriteAnim(&gSprites[*spriteId], animNum);
    }
}

void DestroyDecorationMarkerSprite(u8 *spriteId)
{
    if (*spriteId != 0xFF)
    {
        DestroySprite(&gSprites[*spriteId]);
        *spriteId = 0xFF;
    }
}

void DestroyDecorationMarkerSprites(u8 *data, u8 n)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        DestroyDecorationMarkerSprite(&data[i]);
    }
}

void CreateVerticalScrollIndicators(u8 id, u16 x, u16 y)
{
    if (id >= 4)
    {
        return;
    }

    switch (id)
    {
    case TOP_ARROW:
        if (sVerticalScrollIndicatorIds[TOP_ARROW] == 0xFF)
        {
            sVerticalScrollIndicatorIds[TOP_ARROW] = CreateSprite(&sSpriteTemplate_VerticalScrollArrow, x, y, 0);
            gSprites[sVerticalScrollIndicatorIds[TOP_ARROW]].data[2] = -1;
            if (sVerticalScrollIndicatorIds[BOTTOM_ARROW] != 0xFF)
            {
                gSprites[sVerticalScrollIndicatorIds[TOP_ARROW]].y2 = gSprites[sVerticalScrollIndicatorIds[BOTTOM_ARROW]].y2 * -1;
                gSprites[sVerticalScrollIndicatorIds[TOP_ARROW]].data[3] = gSprites[sVerticalScrollIndicatorIds[BOTTOM_ARROW]].data[3];
            }
            else
            {
                gSprites[sVerticalScrollIndicatorIds[TOP_ARROW]].data[3] = 0;
            }
        }
        break;

    case BOTTOM_ARROW:
        if (sVerticalScrollIndicatorIds[BOTTOM_ARROW] == 0xFF)
        {
            sVerticalScrollIndicatorIds[BOTTOM_ARROW] = CreateSprite(&sSpriteTemplate_VerticalScrollArrow, x, y, 0);
            gSprites[sVerticalScrollIndicatorIds[BOTTOM_ARROW]].data[2] = 1;
            if (sVerticalScrollIndicatorIds[TOP_ARROW] != 0xFF)
            {
                gSprites[sVerticalScrollIndicatorIds[BOTTOM_ARROW]].y2 = gSprites[sVerticalScrollIndicatorIds[TOP_ARROW]].y2 * -1;
                gSprites[sVerticalScrollIndicatorIds[BOTTOM_ARROW]].data[3] = gSprites[sVerticalScrollIndicatorIds[TOP_ARROW]].data[3];
            }
            else
            {
                gSprites[sVerticalScrollIndicatorIds[BOTTOM_ARROW]].data[3] = 0;
            }
        }
        break;

     case LEFT_ARROW:
        if (sVerticalScrollIndicatorIds[LEFT_ARROW] == 0xFF)
        {
            sVerticalScrollIndicatorIds[LEFT_ARROW] = CreateSprite(&sSpriteTemplate_HorizontalScrollArrow, x, y, 0);
            gSprites[sVerticalScrollIndicatorIds[LEFT_ARROW]].data[0] = -1;
            if (sVerticalScrollIndicatorIds[RIGHT_ARROW] != 0xFF)
            {
                gSprites[sVerticalScrollIndicatorIds[LEFT_ARROW]].x2 = gSprites[sVerticalScrollIndicatorIds[RIGHT_ARROW]].x2 * -1;
                gSprites[sVerticalScrollIndicatorIds[LEFT_ARROW]].data[1] = gSprites[sVerticalScrollIndicatorIds[RIGHT_ARROW]].data[1];
            }
            else
            {
                gSprites[sVerticalScrollIndicatorIds[LEFT_ARROW]].data[1] = 0;
            }
        }
        break;

    case RIGHT_ARROW:
        if (sVerticalScrollIndicatorIds[RIGHT_ARROW] == 0xFF)
        {
            sVerticalScrollIndicatorIds[RIGHT_ARROW] = CreateSprite(&sSpriteTemplate_HorizontalScrollArrow, x, y, 0);
            gSprites[sVerticalScrollIndicatorIds[RIGHT_ARROW]].data[0] = 1;
            if (sVerticalScrollIndicatorIds[LEFT_ARROW] != 0xFF)
            {
                gSprites[sVerticalScrollIndicatorIds[RIGHT_ARROW]].x2 = gSprites[sVerticalScrollIndicatorIds[LEFT_ARROW]].x2 * -1;
                gSprites[sVerticalScrollIndicatorIds[RIGHT_ARROW]].data[1] = gSprites[sVerticalScrollIndicatorIds[LEFT_ARROW]].data[1];
            }
            else
            {
                gSprites[sVerticalScrollIndicatorIds[RIGHT_ARROW]].data[1] = 0;
            }
        }
        break;
    }

    StartSpriteAnim(&gSprites[sVerticalScrollIndicatorIds[id]], id & 1);
}

void SetVerticalScrollIndicators(u8 id, bool8 invisible)
{
    if (id < 4 && sVerticalScrollIndicatorIds[id] != 0xFF)
    {
        gSprites[sVerticalScrollIndicatorIds[id]].invisible = invisible;
    }
}

void DestroyVerticalScrollIndicator(u8 id)
{
    if (id < 4 && sVerticalScrollIndicatorIds[id] != 0xFF)
    {
        DestroySprite(&gSprites[sVerticalScrollIndicatorIds[id]]);
        sVerticalScrollIndicatorIds[id] = 0xFF;
    }
}

void LoadScrollIndicatorPalette(void)
{
    LoadSpritePalette(&sScrollIndicatorPalette);
}

void BuyMenuFreeMemory(void)
{
    FreeSpritePaletteByTag(SCROLL_INDICATOR_PAL_TAG);
}

static void SpriteCB_VerticalScrollIndicator(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        sprite->x2 += sprite->data[0];
    }
    if (sprite->data[3] == 0)
    {
        sprite->y2 += sprite->data[2];
    }
    sprite->data[1]++;
    sprite->data[3]++;
    if (sprite->data[1] > 2)
    {
        sprite->data[1] = 0;
    }
    if (sprite->data[3] > 2)
    {
        sprite->data[3] = 0;
    }
    if (sprite->x2 == 8 || sprite->x2 == -8)
    {
        sprite->x2 = 0;
    }
    if (sprite->y2 == 8 || sprite->y2 == -8)
    {
        sprite->y2 = 0;
    }
}

void StopVerticalScrollIndicators(u8 id)
{
    if (sVerticalScrollIndicatorIds[id] != 0xFF && id < 4)
    {
        gSprites[sVerticalScrollIndicatorIds[id]].callback = SpriteCallbackDummy;
    }
}

void StartVerticalScrollIndicators(u8 id)
{
    if (sVerticalScrollIndicatorIds[id] != 0xFF && id < 4)
    {
        gSprites[sVerticalScrollIndicatorIds[id]].callback = SpriteCB_VerticalScrollIndicator;
    }
}

static void ResetVerticalScrollIndicators(u8 id)
{
    if (sVerticalScrollIndicatorIds[id] != 0xFF && id < 4)
    {
        gSprites[sVerticalScrollIndicatorIds[id]].x2 = 0;
        gSprites[sVerticalScrollIndicatorIds[id]].y2 = 0;
        gSprites[sVerticalScrollIndicatorIds[id]].data[1] = 0;
        gSprites[sVerticalScrollIndicatorIds[id]].data[3] = 0;
    }
}

void PauseVerticalScrollIndicator(u8 id)
{
    StopVerticalScrollIndicators(id);
    ResetVerticalScrollIndicators(id);
}

void SetVerticalScrollIndicatorPriority(u8 id, u8 priority)
{
    if (sVerticalScrollIndicatorIds[id] != 0xFF && id < 4)
    {
        gSprites[sVerticalScrollIndicatorIds[id]].oam.priority = priority;
    }
}
