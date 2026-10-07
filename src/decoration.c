#include "global.h"
#include "constants/decorations.h"
#include "main.h"
#include "constants/event_objects.h"
#include "overworld.h"
#include "sound.h"
#include "constants/songs.h"
#include "string_util.h"
#include "menu.h"
#include "menu_helpers.h"
#include "strings.h"
#include "script.h"
#include "palette.h"
#include "field_player_avatar.h"
#include "field_camera.h"
#include "field_screen_effect.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "event_data.h"
#include "field_weather.h"
#include "decoration.h"
#include "decoration_inventory.h"
#include "shop.h"
#include "ewram.h"

struct DecorationPCContext
{
    /* 0x00 */ u8 *items;
    /* 0x04 */ u8 *pos;
    /* 0x08 */ u8 size;
    /* 0x09 */ u8 isPlayerRoom;
};

struct PlaceDecorationGraphicsDataBuffer
{
    /*0x000; 0x02038900*/ const struct Decoration *decoration;
    /*0x004; 0x02038904*/ u16 tiles[0x40];
    /*0x084; 0x02038984*/ u8 image[0x800];
    /*0x884; 0x02039184*/ u16 palette[16];
};

struct DecorRearrangementDataBuffer
{
    u8 idx;
    u8 width;
    u8 height;
    u16 flagId;
};

static void InitDecorationActionsWindow(void);
static void ShowDecorationItemsWindow(u8 taskId);
static bool8 IsDecorationIndexInSecretBase(u8 idx);
static void IdentifyOwnedDecorationsCurrentlyInUseInternal(u8 taskId);
static void SetInitialPositions(u8 taskId);
static void WarpToInitialPosition(u8 taskId);
static u16 GetDecorationElevation(u8 decoId, u8 tileIndex);
static void ShowDecorationOnMap_(u16 mapX, u16 mapY, u8 decWidth, u8 decHeight, u16 decIdx);
static bool8 HasDecorationSpace/*IsThereRoomForMoreDecorations*/(void);
static void SetUpDecorationShape(u8 taskId);
static void AttemptPlaceDecoration(u8 taskId);
static bool8 IsSecretBaseTrainerSpot(u8 metatileBehavior, u16 layerType);
static bool8 IsntInitialPosition(u8 taskId, s16 x, s16 y, u16 layerType);
static bool8 IsFloorOrBoardAndHole(u16 metatileBehavior, const struct Decoration *decoration);
static bool8 CanPlaceDecoration(u8 taskId, const struct Decoration *decoration);
static void PlaceDecoration_(u8 taskId);
static void CancelDecorating_(u8 taskId);
static bool8 ApplyCursorMovement_IsInvalid(u8 taskId);
static bool8 IsHoldingDirection(void);
static void ResetCursorMovement(void);
static void Task_SelectLocation(u8 taskId);
static void CantPlaceDecorationPrompt(u8 taskId);
static void SetDecorSelectionBoxTiles(struct PlaceDecorationGraphicsDataBuffer *data);
static void SetDecorSelectionMetatiles(struct PlaceDecorationGraphicsDataBuffer *data);
static void InitializePuttingAwayCursorSprite(struct Sprite *sprite);
static void InitializePuttingAwayCursorSprite2(struct Sprite *sprite);
static void ClearDecorationContextIndex(u8 idx);
static void AttemptPutAwayDecoration(u8 taskId);
static void ContinuePuttingAwayDecorationsPrompt(u8 taskId);
static void SetDecorRearrangementShape(u8 decorIdx, struct DecorRearrangementDataBuffer *data);
static void SetCameraSpritePosition(u8 x, u8 y);
static void SetDecorRearrangementFlagIdIfFlagUnset(void);
static bool8 AttemptMarkSpriteDecorUnderCursorForRemoval(u8 taskId);
static void MarkSpriteDecorsInBoundsForRemoval(u8 left, u8 top, u8 right, u8 bottom);
static void AttemptMarkDecorUnderCursorForRemoval(u8 taskId);
static void PutAwayDecoration(u8 taskId);
static void StopPuttingAwayDecorations_(u8 taskId);
static void Task_StopPuttingAwayDecorations(u8 taskId);
static void InitializeCameraSprite1(struct Sprite *sprite);
static void LoadPlayerSpritePalette(void);
static void FreePlayerSpritePalette(void);
static const struct UnkStruct_803EC860 sDecorTilemaps[];
static const struct UnkStruct_083EC900 sDecorationMovementInfo[];
static const struct SpritePalette sSpritePal_PlaceDecoration;
static const struct SpritePalette sSpritePal_PuttingAwayCursorBrendan;
static const struct SpritePalette sSpritePal_PuttingAwayCursorMay;
static const struct SpriteTemplate sDecorationSelectorSpriteTemplate;
static const struct SpriteTemplate sPuttingAwayCursorSpriteTemplate;
static struct PlaceDecorationGraphicsDataBuffer sPlaceDecorationGraphicsDataBuffer;
static struct OamData sDecorSelectorOam;
static u8 sDecorationActionsCursorPos;
static u8 sNumOwnedDecorationsInCurCategory;
static u8 sDecorationsCursorPos;
static u8 sDecorationsScrollOffset;
static u8 sCurDecorationCategory;
static u8 sSecretBaseItemsIndicesBuffer[16];
static u8 sPlayerRoomItemsIndicesBuffer[12];
static u16 sCurDecorMapX;
static u16 sCurDecorMapY;
static u8 sDecor_CameraSpriteObjectIdx1;
static u8 sDecor_CameraSpriteObjectIdx2;
static u8 sDecorationLastDirectionMoved;
static u8 sCurDecorSelectedInRearrangement;
static const struct YesNoFuncTable sPlacePutAwayYesNoFunctions[];
static struct DecorRearrangementDataBuffer sDecorRearrangementDataBuffer[16];
static const u8 *const sDecorationCategoryNames[];
static const u8 *const sSecretBasePCMenuItemDescriptions[];
static const struct MenuAction2 sDecorationMainMenuActions[];
static const u8 sDecorationStandElevations[];
static const u8 sDecorationSlideElevation[];
static const struct YesNoFuncTable sPlaceDecorationYesNoFunctions;
static const struct YesNoFuncTable sCancelDecoratingYesNoFunctions;
static const struct YesNoFuncTable sReturnDecorationYesNoFunctions;
static const struct YesNoFuncTable sStopPuttingAwayDecorationsYesNoFunctions;
static const struct YesNoFuncTable sTossDecorationYesNoFunctions;

EWRAM_DATA u8 *gCurDecorationItems = NULL;
EWRAM_DATA static u8 sDecorationActionsCursorPos = 0;
EWRAM_DATA static u8 sNumOwnedDecorationsInCurCategory = 0;
EWRAM_DATA static u8 sSecretBaseItemsIndicesBuffer[16] = {0};
EWRAM_DATA static u8 sPlayerRoomItemsIndicesBuffer[12] = {0};
EWRAM_DATA static u8 sDecorationsCursorPos = 0;
EWRAM_DATA u8 gUnknown_020388F3 = 0;
EWRAM_DATA static u8 sDecorationsScrollOffset = 0;
EWRAM_DATA u8 gCurDecorationIndex = 0;
EWRAM_DATA static u8 sCurDecorationCategory = 0;
EWRAM_DATA u8 gUnknown_020388F7[8] = {0};
EWRAM_DATA static struct PlaceDecorationGraphicsDataBuffer sPlaceDecorationGraphicsDataBuffer = {0};
EWRAM_DATA static u16 sCurDecorMapX = 0;
EWRAM_DATA static u16 sCurDecorMapY = 0;
EWRAM_DATA static u8 sDecor_CameraSpriteObjectIdx1 = 0;
EWRAM_DATA static u8 sDecor_CameraSpriteObjectIdx2 = 0;
EWRAM_DATA static u8 sDecorationLastDirectionMoved = 0;
EWRAM_DATA static struct OamData sDecorSelectorOam = {0};
EWRAM_DATA static struct DecorRearrangementDataBuffer sDecorRearrangementDataBuffer[16] = {0};
EWRAM_DATA static u8 sCurDecorSelectedInRearrangement = 0;

#if ENGLISH
#include "data/decoration/description.h"
#elif GERMAN
#include "data/decoration/description_de.h"
#endif

#include "data/decoration/tiles.h"

#if ENGLISH
#include "data/decoration/header.h"
#elif GERMAN
#include "data/decoration/header_de.h"
#endif

static const u8 *const sDecorationCategoryNames[] = {
    SecretBaseText_Desk,
    SecretBaseText_Chair,
    SecretBaseText_Plant,
    SecretBaseText_Ornament,
    SecretBaseText_Mat,
    SecretBaseText_Poster,
    SecretBaseText_Doll,
    SecretBaseText_Cushion
};

static const struct MenuAction2 sDecorationMainMenuActions[] = {
    {SecretBaseText_Decorate, sub_80FF160},
    {SecretBaseText_PutAway, sub_8100A0C},
    {SecretBaseText_Toss, sub_8101700},
    {gText_Exit, gpu_pal_decompress_alloc_tag_and_upload}
};

static const u8 *const sSecretBasePCMenuItemDescriptions[] = {
    SecretBaseText_PutOutDecor,
    SecretBaseText_StoreChosenDecor,
    SecretBaseText_ThrowAwayDecor,
    gText_GoBackPrevMenu
};

const struct YesNoFuncTable gUnknown_083EC634[] = {
    {sub_80FF5BC, sub_80FF058},
    {sub_81017A0, sub_80FF058},
    {sub_81017A0, sub_80FF058},
    {sub_8109D04, sub_80FF058}
};

// text

extern u8 SecretBase_EventScript_PCCancel[];

void sub_80FE1DC(void)
{
    sub_80FE2B4();
    Menu_DrawStdWindowFrame(0, 0, 10, 9);
    Menu_PrintItems(1, 1, 4, (const struct MenuAction *)sDecorationMainMenuActions);
    InitMenu(0, 1, 1, 4, sDecorationActionsCursorPos, 9);
}

static void InitDecorationActionsWindow(void)
{
    sDecorationActionsCursorPos = 0;
    LockPlayerFieldControls();
    sub_80FE1DC();
    sub_80FE394();
}

void DecorationPC(u8 taskId)
{
    InitDecorationActionsWindow();
    gTasks[taskId].func = Task_DecorationPCProcessMenuInput;
}

void DoSecretBaseDecorationMenu(u8 taskId)
{
    DecorationPC(taskId);
    sDecorationContext.items = gSaveBlock1.secretBases[0].decorations;
    sDecorationContext.pos = gSaveBlock1.secretBases[0].decorationPositions;
    sDecorationContext.size = sizeof gSaveBlock1.secretBases[0].decorations;
    sDecorationContext.isPlayerRoom = 0;
}

void DoPlayerRoomDecorationMenu(u8 taskId)
{
    DecorationPC(taskId);
    sDecorationContext.items = gSaveBlock1.playerRoomDecorations;
    sDecorationContext.pos = gSaveBlock1.playerRoomDecorationPositions;
    sDecorationContext.size = sizeof gSaveBlock1.playerRoomDecorations;
    sDecorationContext.isPlayerRoom = 1;
}

void sub_80FE2B4(void)
{
    u16 palettes[3] = {RGB(24, 24, 24),
                       RGB(28, 28, 28),
                       RGB(31, 31, 31)};
    LoadPalette(&palettes[2], 0xdf, 2);
    LoadPalette(&palettes[1], 0xd1, 2);
    LoadPalette(&palettes[0], 0xd8, 2);
}

void Task_DecorationPCProcessMenuInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            sDecorationActionsCursorPos = Menu_MoveCursor(-1);
            sub_80FE394();
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            sDecorationActionsCursorPos = Menu_MoveCursor(1);
            sub_80FE394();
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sDecorationMainMenuActions[sDecorationActionsCursorPos].func(taskId);
        } else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gpu_pal_decompress_alloc_tag_and_upload(taskId);
        }
    }
}

void sub_80FE394(void)
{
    Menu_BlankWindowRect(2, 15, 27, 18);
    Menu_PrintText(sSecretBasePCMenuItemDescriptions[sDecorationActionsCursorPos], 2, 15);
}

void gpu_pal_decompress_alloc_tag_and_upload(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 10, 9);
    Menu_BlankWindowRect(2, 15, 27, 18);
    FreeSpritePaletteByTag(6);
    if (sDecorationContext.isPlayerRoom == 0)
    {
        ScriptContext_SetupScript(SecretBase_EventScript_PCCancel);
        DestroyTask(taskId);
    } else
    {
        ReshowPlayerPC(taskId);
    }
}

void sub_80FE418(u8 taskId)
{
    sub_80FE5AC(taskId);
}

void sub_80FE428(u8 taskId)
{
    InitMenu(0, 1, 1, 4, sDecorationActionsCursorPos, 9);
    sub_80FE394();
    gTasks[taskId].func = Task_DecorationPCProcessMenuInput;
}

void sub_80FE470(u8 category, u8 left, u8 top, u8 palIdx) // PrintDecorationCategorySelectionMenuString
{
    u8 *strptr;
    u8 v0;
    v0 = Menu_GetTextWindowPaletteNum();
    // PALETTE {palIdx}
    strptr = gStringVar4;
    strptr[0] = EXT_CTRL_CODE_BEGIN;
    strptr[1] = 5;
    strptr[2] = palIdx;
    strptr += 3;
    strptr = StringCopy(strptr, sDecorationCategoryNames[category]);
    strptr = AlignInt1InMenuWindow(strptr, GetNumOwnedDecorationsInCategory(category), 0x56, 1);
    *strptr++ = 0xba;
    strptr = AlignInt1InMenuWindow(strptr, gDecorationInventories[category].size, 0x68, 1);
    strptr[0] = EXT_CTRL_CODE_BEGIN;
    strptr[1] = 5;
    strptr[2] = v0;
    strptr[3] = EOS;
    Menu_PrintText(gStringVar4, left, top);
}

void sub_80FE528(u8 taskId) // PrintDecorationCategorySelectionMenuStrings
{
    u8 category;
    Menu_DrawStdWindowFrame(0, 0, 14, 19);
    for (category=0; category<8; category++)
    {
        if (sDecorationContext.isPlayerRoom == 1 && gTasks[taskId].data[11] == 0 && category != DECORCAT_DOLL && category != DECORCAT_CUSHION)
        {
            sub_80FE470(category, 1, 2 * category + 1, 13); // Selectable
        } else
        {
            sub_80FE470(category, 1, 2 * category + 1, 255); // Unselectable
        }
    }
    Menu_PrintText(gText_Exit, 1, 17);
}

void sub_80FE5AC(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);
    sub_80FE528(taskId);
    InitMenu(0, 1, 1, 9, sCurDecorationCategory, 13);
    gTasks[taskId].func = sub_80FE604;
}

void sub_80FE604(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPEAT(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(-1);
        } else if (JOY_REPEAT(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(1);
        } else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sCurDecorationCategory = Menu_GetCursorPos();
            if (sCurDecorationCategory != 8)
            {
                sNumOwnedDecorationsInCurCategory = GetNumOwnedDecorationsInCategory(sCurDecorationCategory);
                if (sNumOwnedDecorationsInCurCategory)
                {
                    CondenseDecorationsInCategory(sCurDecorationCategory);
                    gCurDecorationItems = gDecorationInventories[sCurDecorationCategory].items;
                    sub_80FEF50(taskId);
                    ClearVerticalScrollIndicatorPalettes();
                    sub_80F9480(gUnknown_020388F7, 8);
                    LoadScrollIndicatorPalette();
                    gTasks[taskId].func = ShowDecorationItemsWindow;
                } else
                {
                    Menu_DestroyCursor();
                    Menu_EraseWindowRect(0, 0, 14, 19);
                    DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, sub_80FE418, 0);
                }
            } else
            {
                sub_80FE728(taskId);
            }
        } else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            sub_80FE728(taskId);
        }
    }
}

void sub_80FE728(u8 taskId)
{
    if (gTasks[taskId].data[11] != 3)
    {
        sub_80FE758(taskId);
    } else
    {
        sub_8109DAC(taskId);
    }
}

void sub_80FE758(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 14, 19);
    if (gTasks[taskId].data[11] != 2)
    {
        sub_80FE1DC();
        Menu_DisplayDialogueFrame();
        sub_80FE394();
        gTasks[taskId].func = Task_DecorationPCProcessMenuInput;
    } else
    {
        Shop_RunExitSellMenuTask(taskId);
    }
}

void sub_80FE7A8(u8 taskId)
{
    gTasks[taskId].data[11] = 3;
    sCurDecorationCategory = 0;
    sub_80FE5AC(taskId);
}

void CopyDecorationCategoryName(u8 *dest, u8 category)
{
    StringCopy(dest, sDecorationCategoryNames[category]);
}

void sub_80FE7EC(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);

    sub_80FEC94(taskId);
    sub_80FECB8(sCurDecorationCategory);

#if ENGLISH
    Menu_DrawStdWindowFrame(15, 12, 29, 19);
#elif GERMAN
    if ((sDecorationsCursorPos + sDecorationsScrollOffset) != sNumOwnedDecorationsInCurCategory)
    {
        Menu_DrawStdWindowFrame(15, 12, 29, 19);
    }
#endif

    sub_80FECE0(sDecorationsCursorPos + sDecorationsScrollOffset);
    InitMenu(0, 1, 2, gUnknown_020388F3 + 1, sDecorationsCursorPos, 13);
}

static void ShowDecorationItemsWindow(u8 taskId)
{
    sub_80FE7EC(taskId);
    gTasks[taskId].func = sub_80FE948;
}

void sub_80FE894(u8 taskId /*r8*/, s8 cursorVector /*r5*/, s8 bgVector /*r7*/)
{
    int v0 /*r10*/;
    u8 v1;
    v0 = sDecorationsCursorPos + sDecorationsScrollOffset == sNumOwnedDecorationsInCurCategory;
    PlaySE(SE_SELECT);
    if (cursorVector != 0)
    {
        sDecorationsCursorPos = Menu_MoveCursor(cursorVector);
    }
    if (bgVector != 0)
    {
        v1 = sDecorationsScrollOffset;
        sDecorationsScrollOffset = v1 + bgVector;
        sub_80FEABC(taskId, 1);
    }
    if (sDecorationsCursorPos + sDecorationsScrollOffset != sNumOwnedDecorationsInCurCategory)
    {
        if (v0)
        {
            Menu_DrawStdWindowFrame(15, 12, 29, 19);
        }
        sub_80FECE0(sDecorationsCursorPos + sDecorationsScrollOffset);
    } else
    {
        Menu_EraseWindowRect(15, 12, 29, 19);
    }
}

void sub_80FE948(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPEAT(DPAD_ANY) == DPAD_UP)
        {
            if (sDecorationsCursorPos != 0)
            {
                sub_80FE894(taskId, -1, 0);
            } else if (sDecorationsScrollOffset != 0)
            {
                sub_80FE894(taskId, 0, -1);
            }
        }
        if (JOY_REPEAT(DPAD_ANY) == DPAD_DOWN)
        {
            if (sDecorationsCursorPos != gUnknown_020388F3)
            {
                sub_80FE894(taskId, 1, 0);
            } else if (sDecorationsScrollOffset + sDecorationsCursorPos != sNumOwnedDecorationsInCurCategory)
            {
                sub_80FE894(taskId, 0, 1);
            }
        }
        if (JOY_NEW(A_BUTTON))
        {
            Menu_DestroyCursor();
            PlaySE(SE_SELECT);
            gCurDecorationIndex = sDecorationsCursorPos + sDecorationsScrollOffset;
            if (gCurDecorationIndex == sNumOwnedDecorationsInCurCategory)
            {
                gUnknown_083EC634[gTasks[taskId].data[11]].noFunc(taskId);
            } else
            {
                gUnknown_083EC634[gTasks[taskId].data[11]].yesFunc(taskId);
            }
        } else if (JOY_NEW(B_BUTTON))
        {
            Menu_DestroyCursor();
            PlaySE(SE_SELECT);
            gUnknown_083EC634[gTasks[taskId].data[11]].noFunc(taskId);
        }
    }
}

const u8 gUnknown_083EC65A[] = _("{PALETTE 13}{STR_VAR_1}");

void sub_80FEABC(u8 taskId, u8 dummy1)
{
    u16 i;
    u16 j;
    u8 ni;
    if (sDecorationsScrollOffset != 0 || (DestroyVerticalScrollIndicator(TOP_ARROW), sDecorationsScrollOffset != 0))
    {
        CreateVerticalScrollIndicators(TOP_ARROW, 0x3c, 0x08);
    }
    if (sDecorationsScrollOffset + 7 == sNumOwnedDecorationsInCurCategory)
    {
        DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    }
    if (sDecorationsScrollOffset + 7 < sNumOwnedDecorationsInCurCategory)
    {
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 0x3c, 0x98);
    }
    for (i=sDecorationsScrollOffset; i<sDecorationsScrollOffset+8; i++)
    {
        ni = 2 * (i - sDecorationsScrollOffset) + 2;
        if (gUnknown_020388F7[i - sDecorationsScrollOffset])
        {
            sub_80F94F8(&gUnknown_020388F7[i - sDecorationsScrollOffset]);
        }
        if (i == sNumOwnedDecorationsInCurCategory)
        {
            sub_8072A18(gText_Exit, 0x08, 8 * ni, 0x68, 1);
            break;
        }
        if (gCurDecorationItems[i])
        {
            if (sDecorationContext.isPlayerRoom == 1 && sCurDecorationCategory != DECORCAT_DOLL && sCurDecorationCategory != DECORCAT_CUSHION && gTasks[taskId].data[11] == 0)
            {
                StringCopy(gStringVar1, gDecorations[gCurDecorationItems[i]].name);
                sub_8072A18(gUnknown_083EC65A, 0x08, 8 * ni, 0x68, 1);
            } else
            {
                sub_8072A18(gDecorations[gCurDecorationItems[i]].name, 0x08, 8 * ni, 0x68, 1);
            }
            for (j=0; j<16; j++)
            {
                if (sSecretBaseItemsIndicesBuffer[j] - 1 == i)
                {
                    sub_80F94A4(4, &gUnknown_020388F7[i - sDecorationsScrollOffset], 0x6c, (i - sDecorationsScrollOffset) * 16 + 24);
                    break;
                }
            }
            for (j=0; j<12; j++)
            {
                if (sPlayerRoomItemsIndicesBuffer[j] - 1 == i)
                {
                    sub_80F94A4(5, &gUnknown_020388F7[i - sDecorationsScrollOffset], 0x6c, (i - sDecorationsScrollOffset) * 16 + 24);
                    break;
                }
            }
        }
    }
}

void sub_80FEC94(u8 taskId)
{
    Menu_DrawStdWindowFrame(0, 0, 14, 19);
    sub_80FEABC(taskId, 0);
}

void sub_80FECB8(u8 category)
{
    Menu_DrawStdWindowFrame(15, 0, 29, 3);
    sub_80FE470(category, 16, 1, 0xff);
}

#if ENGLISH
void sub_80FECE0(u8 itemIndex)
{
    sub_8072AB0(gDecorations[gCurDecorationItems[itemIndex]].description, 0x80, 0x68, 0x68, 0x30, 0x1);
}
#elif GERMAN
int sub_80FECE0(u8 itemIndex)
{
    if (itemIndex == sNumOwnedDecorationsInCurCategory)
        return FALSE;

    sub_8072AB0(gDecorations[gCurDecorationItems[itemIndex]].description, 0x80, 0x68, 0x68, 0x30, 0x1);
    return TRUE;
}
#endif

void sub_80FED1C(void)
{
    Menu_EraseWindowRect(15, 0, 29, 3);
    Menu_EraseWindowRect(15, 12, 29, 19);
}

void sub_80FED3C(u8 taskId)
{
    LoadScrollIndicatorPalette();
    gTasks[taskId].func = ShowDecorationItemsWindow;
}

static bool8 IsDecorationIndexInSecretBase(u8 idx)
{
    u8 i;
    for (i=0; i<16; i++)
    {
        if (sSecretBaseItemsIndicesBuffer[i] == idx)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void IdentifyOwnedDecorationsCurrentlyInUseInternal(u8 taskId)
{
    u16 i;
    u16 j;
    u16 k;
    u16 count;
    count = 0;
    for (i=0; i<16; i++)
    {
        sSecretBaseItemsIndicesBuffer[i] = 0;
        if (i < 12)
        {
            sPlayerRoomItemsIndicesBuffer[i] = 0;
        }
    }
    for (i=0; i<16; i++)
    {
        if (gSaveBlock1.secretBases[0].decorations[i] != 0)
        {
            for (j=0; j<gDecorationInventories[sCurDecorationCategory].size; j++)
            {
                if (gCurDecorationItems[j] == gSaveBlock1.secretBases[0].decorations[i])
                {
                    for (k=0; k<count && sSecretBaseItemsIndicesBuffer[k]!=j+1; k++);
                    if (k == count)
                    {
                        sSecretBaseItemsIndicesBuffer[count] = j+1;
                        count++;
                        break;
                    }
                }
            }
        }
    }
    count = 0;
    for (i=0; i<12; i++)
    {
        if (gSaveBlock1.playerRoomDecorations[i] != 0)
        {
            for (j=0; j<gDecorationInventories[sCurDecorationCategory].size; j++)
            {
                if (gCurDecorationItems[j] == gSaveBlock1.playerRoomDecorations[i] && !IsDecorationIndexInSecretBase(j + 1))
                {
                    for (k=0; k<count && sPlayerRoomItemsIndicesBuffer[k]!=j+1; k++);
                    if (k == count)
                    {
                        sPlayerRoomItemsIndicesBuffer[count] = j+1;
                        count++;
                        break;
                    }
                }
            }
        }
    }
}

void sub_80FEF28(void)
{
    if (sNumOwnedDecorationsInCurCategory <= 7)
    {
        gUnknown_020388F3 = sNumOwnedDecorationsInCurCategory;
    } else
    {
        gUnknown_020388F3 = 7;
    }
}

void sub_80FEF50(u8 taskId)
{
    IdentifyOwnedDecorationsCurrentlyInUseInternal(taskId);
    sub_80FEF28();
    sDecorationsCursorPos = 0;
    sDecorationsScrollOffset = 0;
}

void sub_80FEF74(void)
{
    sub_80F9520(gUnknown_020388F7, 8);
    DestroyVerticalScrollIndicator(TOP_ARROW);
    DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 14, 19);
}

bool8 IsSelectedDecorInThePC(void)
{
    u16 i;
    int v0;
    for (i=0; i<16; i++)
    {
        v0 = sDecorationsScrollOffset + sDecorationsCursorPos + 1;
        if (sSecretBaseItemsIndicesBuffer[i] == v0 || (i < 12 && sPlayerRoomItemsIndicesBuffer[i] == v0))
        {
            return FALSE;
        }
    }
    return TRUE;
}

void sub_80FEFF4(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        LoadScrollIndicatorPalette();
        gTasks[taskId].func = ShowDecorationItemsWindow;
    }
}

void sub_80FF034(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 14, 19);
    sub_80FE5AC(taskId);
}

void sub_80FF058(u8 taskId)
{
    sub_80F9520(gUnknown_020388F7, 8);
    DestroyVerticalScrollIndicator(TOP_ARROW);
    DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    BuyMenuFreeMemory();
    gTasks[taskId].func = sub_80FF034;
}

void sub_80FF098(u8 taskId)
{
    sNumOwnedDecorationsInCurCategory--;
    if (sDecorationsScrollOffset + 7 > sNumOwnedDecorationsInCurCategory && sDecorationsScrollOffset != 0)
    {
        sDecorationsScrollOffset--;
    }
    CondenseDecorationsInCategory(sCurDecorationCategory);
    IdentifyOwnedDecorationsCurrentlyInUseInternal(taskId);
    sub_80FEF28();
}

static void SetInitialPositions(u8 taskId)
{
    gTasks[taskId].data[3] = gSaveBlock1.pos.x;
    gTasks[taskId].data[4] = gSaveBlock1.pos.y;
    PlayerGetDestCoords(&gTasks[taskId].data[0], &gTasks[taskId].data[1]);
}

static void WarpToInitialPosition(u8 taskId)
{
    DrawWholeMapView();
    SetWarpDestination(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1, gTasks[taskId].data[3], gTasks[taskId].data[4]);
    WarpIntoMap();
}
void sub_80FF160(u8 taskId)
{
    if (!GetNumOwnedDecorations())
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, sub_80FE428, 0);
    } else
    {
        gTasks[taskId].data[11] = 0;
        sCurDecorationCategory = 0;
        sub_80FE5AC(taskId);
    }
}

static u16 GetDecorationElevation(u8 decoId, u8 tileIndex)
{
    u16 elevation;
    elevation = 0xffff;

    switch (decoId)
    {
        case DECOR_STAND:
            elevation = sDecorationStandElevations[tileIndex] << 12;
            return elevation;
        case DECOR_SLIDE:
            elevation = sDecorationSlideElevation[tileIndex] << 12;
            return elevation;
        default:
            return elevation;
    }
}

static void ShowDecorationOnMap_(u16 mapX, u16 mapY, u8 decWidth, u8 decHeight, u16 decIdx)
{
    u16 j;
    u16 i;
    u16 attributes;
    u16 impassableFlag;
    u16 overlapsWall;
    u16 elevation;
    s16 x;
    s16 y;

    for (j=0; j<decHeight; j++)
    {
        y = mapY - decHeight + 1 + j;
        for (i=0; i<decWidth; i++)
        {
            x = mapX + i;
            attributes = GetMetatileAttributesById(0x200 + gDecorations[decIdx].tiles[j * decWidth + i]);
            if (MetatileBehavior_IsSecretBaseImpassable(attributes) == TRUE || (gDecorations[decIdx].permission != DECORPERM_PASS_FLOOR && (attributes >> 12) != 0))
                impassableFlag = 0xc00; // impassable collision
            else
                impassableFlag = 0x000; // passable collision

            if (gDecorations[decIdx].permission != DECORPERM_NA_WALL && MetatileBehavior_IsSecretBaseNorthWall(MapGridGetMetatileBehaviorAt(x, y)) == 1)
                overlapsWall = 1;
            else
                overlapsWall = 0;

            elevation = GetDecorationElevation(gDecorations[decIdx].id, j * decWidth + i);
            if (elevation != 0xffff)
            {
                MapGridSetMetatileEntryAt(x, y, (gDecorations[decIdx].tiles[j * decWidth + i] + (0x200 | overlapsWall)) | impassableFlag | elevation);
            } else
            {
                MapGridSetMetatileIdAt(x, y, (gDecorations[decIdx].tiles[j * decWidth + i] + (0x200 | overlapsWall)) | impassableFlag);
            }
        }
    }
}

void ShowDecorationOnMap(u16 mapX, u16 mapY, u16 decIdx)
{
    switch (gDecorations[decIdx].shape)
    {
        case DECORSHAPE_1x1:
            ShowDecorationOnMap_(mapX, mapY, 1, 1, decIdx);
            break;
        case DECORSHAPE_2x1:
            ShowDecorationOnMap_(mapX, mapY, 2, 1, decIdx);
            break;
        case DECORSHAPE_3x1: // unused
            ShowDecorationOnMap_(mapX, mapY, 3, 1, decIdx);
            break;
        case DECORSHAPE_4x2:
            ShowDecorationOnMap_(mapX, mapY, 4, 2, decIdx);
            break;
        case DECORSHAPE_2x2:
            ShowDecorationOnMap_(mapX, mapY, 2, 2, decIdx);
            break;
        case DECORSHAPE_1x2:
            ShowDecorationOnMap_(mapX, mapY, 1, 2, decIdx);
            break;
        case DECORSHAPE_1x3: // unused
            ShowDecorationOnMap_(mapX, mapY, 1, 3, decIdx);
            break;
        case DECORSHAPE_2x4:
            ShowDecorationOnMap_(mapX, mapY, 2, 4, decIdx);
            break;
        case DECORSHAPE_3x3:
            ShowDecorationOnMap_(mapX, mapY, 3, 3, decIdx);
            break;
        case DECORSHAPE_3x2:
            ShowDecorationOnMap_(mapX, mapY, 3, 2, decIdx);
            break;
    }
}

void SetDecoration(void)
{
    u8 i;
    u8 j;
    for (i=0; i<14; i++)
    {
        if (FlagGet(i + FLAG_DECORATION_2) == 1)
        {
            FlagClear(i + FLAG_DECORATION_2);
            for (j=0; j<gMapHeader.events->objectEventCount; j++)
            {
                if (gMapHeader.events->objectEvents[j].flagId == i + FLAG_DECORATION_2)
                {
                    break;
                }
            }
            VarSet(0x3f20 + gMapHeader.events->objectEvents[j].graphicsId, sPlaceDecorationGraphicsDataBuffer.decoration->tiles[0]);
            gSpecialVar_0x8005 = gMapHeader.events->objectEvents[j].localId;
            gSpecialVar_0x8006 = sCurDecorMapX;
            gSpecialVar_0x8007 = sCurDecorMapY;
            TrySpawnObjectEvent(gSpecialVar_0x8005, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
            TryMoveObjectEventToMapCoords(gSpecialVar_0x8005, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, gSpecialVar_0x8006, gSpecialVar_0x8007);
            TryOverrideObjectEventTemplateCoords(gSpecialVar_0x8005, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
            break;
        }
    }
}

static bool8 HasDecorationSpace/*IsThereRoomForMoreDecorations*/(void)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void sub_80FF5BC(u8 taskId)
{
    if (sDecorationContext.isPlayerRoom == 1 && sCurDecorationCategory != DECORCAT_DOLL && sCurDecorationCategory != DECORCAT_CUSHION)
    {
        sub_80FEF74();
        sub_80FED1C();
        DisplayItemMessageOnField(taskId, gSecretBaseText_DecorCantPlace, sub_80FEFF4, 0);
    } else if (IsSelectedDecorInThePC() == TRUE)
    {
        if (HasDecorationSpace() == TRUE)
        {
            FadeScreen(1, 0);
            gTasks[taskId].data[2] = 0;
            gTasks[taskId].func = sub_80FF6AC;
        } else
        {
            sub_80FEF74();
            sub_80FED1C();
            ConvertIntToDecimalStringN(gStringVar1, sDecorationContext.size, STR_CONV_MODE_RIGHT_ALIGN, 2);
            if (!sDecorationContext.isPlayerRoom)
            {
                StringExpandPlaceholders(gStringVar4, gSecretBaseText_NoMoreDecor);
            } else
            {
                StringExpandPlaceholders(gStringVar4, gSecretBaseText_NoMoreDecor2);
            }
            DisplayItemMessageOnField(taskId, gStringVar4, sub_80FEFF4, 0);
        }
    } else
    {
        sub_80FEF74();
        sub_80FED1C();
        DisplayItemMessageOnField(taskId, gSecretBaseText_InUseAlready, sub_80FEFF4, 0);
    }
}

void sub_80FF6AC(u8 taskId)
{
    switch (gTasks[taskId].data[2])
    {
        case 0:
            if (!gPaletteFade.active)
            {
                SetInitialPositions(taskId);
                DestroyVerticalScrollIndicator(TOP_ARROW);
                DestroyVerticalScrollIndicator(BOTTOM_ARROW);
                sub_80F9520(gUnknown_020388F7, 8);
                BuyMenuFreeMemory();
                gTasks[taskId].data[2] = 1;
            }
            break;
        case 1:
            gPaletteFade.bufferTransferDisabled = 1;
            AddDecorationIconObjectFromObjectEvent(&sPlaceDecorationGraphicsDataBuffer, gCurDecorationItems[gCurDecorationIndex]);
            SetUpDecorationShape(taskId);
            SetUpPlacingDecorationPlayerAvatar(taskId, &sPlaceDecorationGraphicsDataBuffer);
            FadeInFromBlack();
            gPaletteFade.bufferTransferDisabled = 0;
            gTasks[taskId].data[2] = 2;
            break;
        case 2:
            if (IsWeatherNotFadingIn() == TRUE)
            {
                gTasks[taskId].data[12] = 0;
                sub_810065C(taskId);
            }
            break;
    }
}

void AddDecorationIconObjectFromObjectEvent(struct PlaceDecorationGraphicsDataBuffer * data, u8 decoIdx)
{
    sub_80FEF74();
    sub_80FED1C();
    sub_81006D0(data);
    data->decoration = &gDecorations[decoIdx];
    if (gDecorations[decoIdx].permission != DECORPERM_SPRITE)
    {
        SetDecorSelectionMetatiles(data);
        sub_8100930(data->decoration->shape);
        SetDecorSelectionBoxTiles(data);
        sub_810070C(data->palette, ((u16 *)gMapHeader.mapLayout->secondaryTileset->metatiles + 8 * data->decoration->tiles[0])[7] >> 12);
        LoadSpritePalette(&sSpritePal_PlaceDecoration);
        sDecor_CameraSpriteObjectIdx1 = gSprites[gFieldCamera.spriteId].data[0];
        gFieldCamera.spriteId = CreateSprite(&sDecorationSelectorSpriteTemplate, sDecorationMovementInfo[data->decoration->shape].cameraX,  sDecorationMovementInfo[data->decoration->shape].cameraY, 0);
    } else
    {
        sDecor_CameraSpriteObjectIdx1 = gSprites[gFieldCamera.spriteId].data[0];
        gFieldCamera.spriteId = CreateObjectGraphicsSprite(data->decoration->tiles[0], InitializePuttingAwayCursorSprite, sDecorationMovementInfo[data->decoration->shape].cameraX,  sDecorationMovementInfo[data->decoration->shape].cameraY, 1);
        gSprites[gFieldCamera.spriteId].oam.priority = 1;
    }
}

void SetUpPlacingDecorationPlayerAvatar(u8 taskId, struct PlaceDecorationGraphicsDataBuffer *data)
{
    u8 x;
    x = 16 * (u8)gTasks[taskId].data[5] + sDecorationMovementInfo[data->decoration->shape].cameraX - 8 * ((u8)gTasks[taskId].data[5] - 1);
    if (data->decoration->shape == DECORSHAPE_3x1 || data->decoration->shape == DECORSHAPE_3x3 || data->decoration->shape == DECORSHAPE_3x2)
    {
        x -= 8;
    }
    if (gSaveBlock2.playerGender == MALE)
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(0xc1, SpriteCallbackDummy, x, 0x48, 0);
    } else
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(0xc2, SpriteCallbackDummy, x, 0x48, 0);
    }
    gSprites[sDecor_CameraSpriteObjectIdx2].oam.priority = 1;
    DestroySprite(&gSprites[sDecor_CameraSpriteObjectIdx1]);
    sDecor_CameraSpriteObjectIdx1 = gFieldCamera.spriteId;
}

static void SetUpDecorationShape(u8 taskId)
{
    switch (gDecorations[gCurDecorationItems[gCurDecorationIndex]].shape)
    {
        case DECORSHAPE_1x1:
            gTasks[taskId].data[5] = 1;
            gTasks[taskId].data[6] = 1;
            break;
        case DECORSHAPE_2x1:
            gTasks[taskId].data[5] = 2;
            gTasks[taskId].data[6] = 1;
            break;
        case DECORSHAPE_3x1:
            gTasks[taskId].data[5] = 3;
            gTasks[taskId].data[6] = 1;
            break;
        case DECORSHAPE_4x2:
            gTasks[taskId].data[5] = 4;
            gTasks[taskId].data[6] = 2;
            break;
        case DECORSHAPE_2x2:
            gTasks[taskId].data[5] = 2;
            gTasks[taskId].data[6] = 2;
            break;
        case DECORSHAPE_1x2:
            gTasks[taskId].data[5] = 1;
            gTasks[taskId].data[6] = 2;
            break;
        case DECORSHAPE_1x3:
            gTasks[taskId].data[5] = 1;
            gTasks[taskId].data[6] = 3;
            gTasks[taskId].data[1]++;
            break;
        case DECORSHAPE_2x4:
            gTasks[taskId].data[5] = 2;
            gTasks[taskId].data[6] = 4;
            break;
        case DECORSHAPE_3x3:
            gTasks[taskId].data[5] = 3;
            gTasks[taskId].data[6] = 3;
            break;
        case DECORSHAPE_3x2:
            gTasks[taskId].data[5] = 3;
            gTasks[taskId].data[6] = 2;
            break;
    }
}

static void AttemptPlaceDecoration(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 1;
    gSprites[sDecor_CameraSpriteObjectIdx2].data[7] = 1;
    ResetCursorMovement();
    sub_8100038(taskId);
}

void sub_80FFB08(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 1;
    gSprites[sDecor_CameraSpriteObjectIdx2].data[7] = 1;
    ResetCursorMovement();
    DisplayItemMessageOnField(taskId, gSecretBaseText_CancelDecorating, sub_8100248, 0);
}

static bool8 IsSecretBaseTrainerSpot(u8 metatileBehavior, u16 layerType)
{
    if (MetatileBehavior_IsSecretBaseTrainerSpot(metatileBehavior) != TRUE || layerType != 0)
    {
        return FALSE;
    }
    return TRUE;
}

static bool8 IsntInitialPosition(u8 taskId, s16 x, s16 y, u16 layerType)
{
    if (x == gTasks[taskId].data[3] + 7 && y == gTasks[taskId].data[4] + 7 && layerType != 0)
    {
        return FALSE;
    }
    return TRUE;
}

static bool8 IsFloorOrBoardAndHole(u16 metatileBehavior, const struct Decoration *decoration)
{
    if (MetatileBehavior_IsSecretBaseTrainerSpot(metatileBehavior) != TRUE)
    {
        if (decoration->id == DECOR_SOLID_BOARD && MetatileBehavior_IsSecretBaseHole(metatileBehavior) == TRUE)
        {
            return TRUE;
        }
        if (MetatileBehavior_IsNormal(metatileBehavior))
        {
            return TRUE;
        }
    }
    return FALSE;
}

static bool8 CanPlaceDecoration(u8 taskId, const struct Decoration *decoration)
{
    u8 i;
    u8 j;
    u8 behaviorAt;
    u16 layerType;
    u8 mapY;
    u8 mapX;
    s16 curY;
    s16 curX;
    mapY = gTasks[taskId].data[6];
    mapX = gTasks[taskId].data[5];
    switch (decoration->permission)
    {
        case DECORPERM_SOLID_FLOOR:
        case DECORPERM_PASS_FLOOR:
            for (i=0; i<mapY; i++)
            {
                curY = gTasks[taskId].data[1] - i;
                for (j=0; j<mapX; j++)
                {
                    curX = gTasks[taskId].data[0] + j;
                    behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                    layerType = GetMetatileAttributesById(0x200 + decoration->tiles[(mapY - 1 - i) * mapX + j]) & 0xf000;
                    if (!IsFloorOrBoardAndHole(behaviorAt, decoration))
                    {
                        return FALSE;
                    }
                    if (!IsntInitialPosition(taskId, curX, curY, layerType))
                    {
                        return FALSE;
                    }
                    behaviorAt = GetObjectEventIdByPosition(curX, curY, 0);
                    if (behaviorAt != 0 && behaviorAt != 16)
                    {
                        return FALSE;
                    }
                }
            }
            break;
        case DECORPERM_BEHIND_FLOOR:
            for (i=0; i<mapY-1; i++)
            {
                curY = gTasks[taskId].data[1] - i;
                for (j=0; j<mapX; j++)
                {
                    curX = gTasks[taskId].data[0] + j;
                    behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                    layerType = GetMetatileAttributesById(0x200 + decoration->tiles[(mapY - 1 - i) * mapX + j]) & 0xf000;
                    if (!MetatileBehavior_IsNormal(behaviorAt) && !IsSecretBaseTrainerSpot(behaviorAt, layerType))
                    {
                        return FALSE;
                    }
                    if (!IsntInitialPosition(taskId, curX, curY, layerType))
                    {
                        return FALSE;
                    }
                    if (GetObjectEventIdByPosition(curX, curY, 0) != 16)
                    {
                        return FALSE;
                    }
                }
            }
            curY = gTasks[taskId].data[1] - mapY + 1;
            for (j=0; j<mapX; j++)
            {
                curX = gTasks[taskId].data[0] + j;
                behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                layerType = GetMetatileAttributesById(0x200 + decoration->tiles[j]) & 0xf000;
                if (!MetatileBehavior_IsNormal(behaviorAt) && !MetatileBehavior_IsSecretBaseNorthWall(behaviorAt))
                {
                    return FALSE;
                }
                if (!IsntInitialPosition(taskId, curX, curY, layerType))
                {
                    return FALSE;
                }
                behaviorAt = GetObjectEventIdByPosition(curX, curY, 0);
                if (behaviorAt != 0 && behaviorAt != 16)
                {
                    return FALSE;
                }
            }
            break;
        case DECORPERM_NA_WALL:
            for (i=0; i<mapY; i++)
            {
                curY = gTasks[taskId].data[1] - i;
                for (j=0; j<mapX; j++)
                {
                    curX = gTasks[taskId].data[0] + j;
                    if (!MetatileBehavior_IsSecretBaseNorthWall(MapGridGetMetatileBehaviorAt(curX, curY)))
                    {
                        return FALSE;
                    }
                    if (MapGridGetMetatileIdAt(curX, curY + 1) == 0x28c)
                    {
                        return FALSE;
                    }
                }
            }
            break;
        case DECORPERM_SPRITE:
            curY = gTasks[taskId].data[1];
            for (j=0; j<mapX; j++)
            {
                curX = gTasks[taskId].data[0] + j;
                behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                if (decoration->shape == DECORSHAPE_1x2)
                {
                    if (!MetatileBehavior_HoldsLargeDecoration(behaviorAt))
                    {
                        return FALSE;
                    }
                }
                else if (!MetatileBehavior_HoldsSmallDecoration(behaviorAt))
                {
                    if (!MetatileBehavior_HoldsLargeDecoration(behaviorAt))
                    {
                        return FALSE;
                    }
                }
                if (GetObjectEventIdByPosition(curX, curY, 0) != 16)
                {
                    return FALSE;
                }
            }
            break;
    }
    return TRUE;
}

void sub_8100038(u8 taskId)
{
    if (CanPlaceDecoration(taskId, &gDecorations[gCurDecorationItems[gCurDecorationIndex]]) == 1)
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_PlaceItHere, sub_81000A0, 0);
    } else
    {
        PlaySE(SE_FAILURE);
        DisplayItemMessageOnField(taskId, gSecretBaseText_CantBePlacedHere, CantPlaceDecorationPrompt, 0);
    }
}

void sub_81000A0(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sPlaceDecorationYesNoFunctions);
}

void sub_81000C4(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    PlaceDecoration_(taskId);
    if (gDecorations[gCurDecorationItems[gCurDecorationIndex]].permission != DECORPERM_SPRITE)
    {
        ShowDecorationOnMap(gTasks[taskId].data[0], gTasks[taskId].data[1], gCurDecorationItems[gCurDecorationIndex]);
    } else
    {
        sCurDecorMapX = gTasks[taskId].data[0] - 7;
        sCurDecorMapY = gTasks[taskId].data[1] - 7;
        ScriptContext_SetupScript(gUnknown_081A2F7B);
    }
    gSprites[sDecor_CameraSpriteObjectIdx1].y += 2;
    CancelDecorating_(taskId);
}

static void PlaceDecoration_(u8 taskId)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] == 0)
        {
            sDecorationContext.items[i] = gCurDecorationItems[gCurDecorationIndex];
            sDecorationContext.pos[i] = ((gTasks[taskId].data[0] - 7) << 4) + (gTasks[taskId].data[1] - 7);
            break;
        }
    }
    if (!sDecorationContext.isPlayerRoom)
    {
        for (i=0; i<16; i++)
        {
            if (sSecretBaseItemsIndicesBuffer[i] == 0)
            {
                sSecretBaseItemsIndicesBuffer[i] = gCurDecorationIndex + 1;
                break;
            }
        }
    } else
    {
        for (i=0; i<12; i++)
        {
            if (sPlayerRoomItemsIndicesBuffer[i] == 0)
            {
                sPlayerRoomItemsIndicesBuffer[i] = gCurDecorationIndex + 1;
                break;
            }
        }
    }
}

void sub_8100248(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sCancelDecoratingYesNoFunctions);
}

void sub_810026C(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    CancelDecorating_(taskId);
}

static void CancelDecorating_(u8 taskId)
{
    FadeScreen(1, 0);
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].func = c1_overworld_prev_quest;
}

void c1_overworld_prev_quest(u8 taskId)
{
    switch (gTasks[taskId].data[2])
    {
        case 0:
            LockPlayerFieldControls();
            if (!gPaletteFade.active)
            {
                WarpToInitialPosition(taskId);
                gTasks[taskId].data[2] = 1;
            }
            break;
        case 1:
            FreePlayerSpritePalette();
            FreeSpritePaletteByTag(0xbb8);
            gFieldCallback = &sub_8100364;
            SetMainCallback2(CB2_ReturnToField);
            DestroyTask(taskId);
            break;
    }
}

void sub_8100334(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        gTasks[taskId].func = sub_80FE948;
    }
}

void sub_8100364(void)
{
    LockPlayerFieldControls();
    LoadScrollIndicatorPalette();
    FadeInFromBlack();
    sub_80FE7EC(CreateTask(sub_8100334, 8));
}

static bool8 ApplyCursorMovement_IsInvalid(u8 taskId)
{
    s16 *data;
    data = gTasks[taskId].data;
    if (sDecorationLastDirectionMoved == DIR_SOUTH && data[1] - data[6] - 6 < 0)
    {
        data[1]++;
        return FALSE;
    } else if (sDecorationLastDirectionMoved == DIR_NORTH && data[1] - 7 >= gMapHeader.mapLayout->height)
    {
        data[1]--;
        return FALSE;
    } else if (sDecorationLastDirectionMoved == DIR_WEST && data[0] - 7 < 0)
    {
        data[0]++;
        return FALSE;
    } else if (sDecorationLastDirectionMoved == DIR_EAST && data[0] + data[5] - 8 >= gMapHeader.mapLayout->width)
    {
        data[0]--;
        return FALSE;
    }
    return TRUE;
}

static bool8 IsHoldingDirection(void)
{
    if (JOY_HELD(DPAD_ANY) != DPAD_UP && JOY_HELD(DPAD_ANY) != DPAD_DOWN && JOY_HELD(DPAD_ANY) != DPAD_LEFT && JOY_HELD(DPAD_ANY) != DPAD_RIGHT)
    {
        return FALSE;
    }
    return TRUE;
}

static void ResetCursorMovement(void)
{
    sDecorationLastDirectionMoved = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[2] = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[3] = 0;
}

static void Task_SelectLocation(u8 taskId)
{
    if (!gSprites[sDecor_CameraSpriteObjectIdx1].data[4])
    {
        if (gTasks[taskId].data[10] == 1)
        {
            sPlacePutAwayYesNoFunctions[gTasks[taskId].data[12]].yesFunc(taskId);
            return;
        } else if (gTasks[taskId].data[10] == 2)
        {
            sPlacePutAwayYesNoFunctions[gTasks[taskId].data[12]].noFunc(taskId);
            return;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_UP)
        {
            sDecorationLastDirectionMoved = DIR_SOUTH;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] =  0;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] = -2;
            gTasks[taskId].data[1]--;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_DOWN)
        {
            sDecorationLastDirectionMoved = DIR_NORTH;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] =  0;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] =  2;
            gTasks[taskId].data[1]++;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_LEFT)
        {
            sDecorationLastDirectionMoved = DIR_WEST;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] = -2;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] =  0;
            gTasks[taskId].data[0]--;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_RIGHT)
        {
            sDecorationLastDirectionMoved = DIR_EAST;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] =  2;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] =  0;
            gTasks[taskId].data[0]++;
        }
        if (!IsHoldingDirection() || !ApplyCursorMovement_IsInvalid(taskId))
        {
            ResetCursorMovement();
        }
    }
    if (sDecorationLastDirectionMoved)
    {
        gSprites[sDecor_CameraSpriteObjectIdx1].data[4]++;
        gSprites[sDecor_CameraSpriteObjectIdx1].data[4] &= 7;
    }
    if (!gTasks[taskId].data[10])
    {
        if (JOY_NEW(A_BUTTON))
        {
            gTasks[taskId].data[10] = A_BUTTON;
        }
        if (JOY_NEW(B_BUTTON))
        {
            gTasks[taskId].data[10] = B_BUTTON;
        }
    }
}

void sub_810065C(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 0;
    gTasks[taskId].data[10] = 0;
    gTasks[taskId].func = Task_SelectLocation;
}

static void CantPlaceDecorationPrompt(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        sub_810065C(taskId);
    }
}

void sub_81006D0(struct PlaceDecorationGraphicsDataBuffer *data)
{
    u16 i;
    for (i=0; i<0x800; i++)
    {
        data->image[i] = 0;
    }
    for (i=0; i<0x40; i++)
    {
        data->tiles[i] = 0;
    }
}

void sub_810070C(u16 *dest, u16 pal)
{
    u16 i;
    for (i=0; i<16; i++)
    {
        dest[i] = ((u16 *)gMapHeader.mapLayout->primaryTileset->palettes)[16 * pal + i];
    }
}

void sub_8100740(u8 *dest, u16 tile)
{
    u8 buffer[32];
    u16 mode;
    u16 i;
    mode = tile >> 10;
    if (tile != 0)
    {
        tile &= 0x3ff;
    }
    for (i=0; i<32; i++)
    {
        buffer[i] = ((u8 *)gMapHeader.mapLayout->primaryTileset->tiles)[tile * 32 + i];
    }
    switch (mode)
    {
        case 0:
            for (i=0; i<32; i++)
            {
                dest[i] = buffer[i];
            }
            break;
        case 1:
            for (i=0; i<8; i++)
            {
                dest[4*i] = (buffer[4*(i+1) - 1] >> 4) + ((buffer[4*(i+1) - 1] & 0xf) << 4);
                dest[4*i + 1] = (buffer[4*(i+1) - 2] >> 4) + ((buffer[4*(i+1) - 2] & 0xf) << 4);
                dest[4*i + 2] = (buffer[4*(i+1) - 3] >> 4) + ((buffer[4*(i+1) - 3] & 0xf) << 4);
                dest[4*i + 3] = (buffer[4*(i+1) - 4] >> 4) + ((buffer[4*(i+1) - 4] & 0xf) << 4);
            }
            break;
        case 2:
            for (i=0; i<8; i++)
            {
                dest[4*i] = buffer[4*(7-i)];
                dest[4*i + 1] = buffer[4*(7-i) + 1];
                dest[4*i + 2] = buffer[4*(7-i) + 2];
                dest[4*i + 3] = buffer[4*(7-i) + 3];
            }
            break;
        case 3:
            for (i=0; i<32; i++)
            {
                dest[i] = (buffer[31-i] >> 4) + ((buffer[31-i] & 0xf) << 4);
            }
            break;
    }
}

static void SetDecorSelectionBoxTiles(struct PlaceDecorationGraphicsDataBuffer *data)
{
    u16 i;
    for (i=0; i<0x40; i++)
        sub_8100740(&data->image[i * 32], data->tiles[i]);
}

u16 sub_810089C(u16 tile)
{
    return ((u16 *)gMapHeader.mapLayout->secondaryTileset->metatiles)[tile] & 0xfff;
}

#include "data/decoration/tilemaps.h"

static const struct UnkStruct_083EC900 sDecorationMovementInfo[] = {
    {0, 1, 0x78, 0x4e},
    {1, 2, 0x80, 0x4e},
    {1, 3, 0x90, 0x56},
    {1, 3, 0x90, 0x46},
    {0, 2, 0x80, 0x46},
    {2, 2, 0x78, 0x46},
    {2, 3, 0x80, 0x56},
    {2, 3, 0x80, 0x36},
    {0, 3, 0x90, 0x46},
    {1, 3, 0x90, 0x46}
};

static const union AnimCmd sDecorSelectorAnimCmd0[] = {
    ANIMCMD_FRAME(.imageValue = 0, .duration = 0),
    ANIMCMD_END
};

static const union AnimCmd *const sDecorSelectorAnimCmds[] = {
    sDecorSelectorAnimCmd0
};

static const struct SpriteFrameImage sDecorSelectorSpriteFrameImages[] = {
    {.data = sPlaceDecorationGraphicsDataBuffer.image, .size = sizeof(sPlaceDecorationGraphicsDataBuffer.image)}
};

static const struct SpriteTemplate sDecorationSelectorSpriteTemplate = {
    .tileTag = 0xffff,
    .paletteTag = 3000,
    .oam = &sDecorSelectorOam,
    .anims = sDecorSelectorAnimCmds,
    .images = sDecorSelectorSpriteFrameImages,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = InitializePuttingAwayCursorSprite
};

static const struct SpritePalette sSpritePal_PlaceDecoration = {.data = (u16 *)&sPlaceDecorationGraphicsDataBuffer.palette, .tag = 3000};

static const struct YesNoFuncTable sPlaceDecorationYesNoFunctions = {.yesFunc = sub_81000C4, .noFunc = sub_810065C};
static const struct YesNoFuncTable sCancelDecoratingYesNoFunctions = {.yesFunc = sub_810026C, .noFunc = sub_810065C};
static const struct YesNoFuncTable sPlacePutAwayYesNoFunctions[] = {
    {.yesFunc = AttemptPlaceDecoration, .noFunc = sub_80FFB08},
    {.yesFunc = AttemptPutAwayDecoration, .noFunc = sub_8100FB4}
};

static const u8 sDecorationStandElevations[] = {4, 4, 4, 4, 0, 3, 3, 0};
static const u8 sDecorationSlideElevation[] = {4, 4, 4, 4, 0, 4, 3, 0};

static const u16 sBrendanPalette[] = INCBIN_U16("graphics/decorations/brendan.gbapal");
static const u16 sMayPalette[] = INCBIN_U16("graphics/decorations/may.gbapal");
static const struct YesNoFuncTable sReturnDecorationYesNoFunctions = {.yesFunc = PutAwayDecoration, .noFunc = sub_8100EEC};
static const struct YesNoFuncTable sStopPuttingAwayDecorationsYesNoFunctions = {.yesFunc = sub_8101590, .noFunc = sub_8100EEC};
static const u32 sDecorationPuttingAwayCursor[] = INCBIN_U32("graphics/decorations/put_away_cursor.4bpp");
static const struct SpritePalette sSpritePal_PuttingAwayCursorBrendan = {.data = sBrendanPalette, .tag = 8};
static const struct SpritePalette sSpritePal_PuttingAwayCursorMay = {.data = sMayPalette, .tag = 8};
static const struct OamData sPuttingAwayCursorOamData = {
    .size = 1, .priority = 1
};

static const union AnimCmd sPuttingAwayCursorAnimCmd0[] = {
    ANIMCMD_FRAME(.imageValue = 0, .duration = 0),
    ANIMCMD_END
};

static const union AnimCmd *const sPuttingAwayCursorAnimCmds[] = {
    sPuttingAwayCursorAnimCmd0
};

static const struct SpriteFrameImage sPuttingAwayCursorPicTable[] = {
    obj_frame_tiles(sDecorationPuttingAwayCursor)
};

static const struct SpriteTemplate sPuttingAwayCursorSpriteTemplate = {
    .tileTag = 0xffff,
    .paletteTag = 8,
    .oam = &sPuttingAwayCursorOamData,
    .anims = sPuttingAwayCursorAnimCmds,
    .images = sPuttingAwayCursorPicTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = InitializeCameraSprite1
};

static const struct YesNoFuncTable sTossDecorationYesNoFunctions = {.yesFunc = sub_8101848, .noFunc = sub_80FED3C};

static void SetDecorSelectionMetatiles(struct PlaceDecorationGraphicsDataBuffer *data)
{
    u8 i;
    u8 shape;
    shape = data->decoration->shape;
    for (i=0; i<sDecorTilemaps[shape].size; i++)
    {
        data->tiles[sDecorTilemaps[shape].tiles[i]] = sub_810089C(data->decoration->tiles[sDecorTilemaps[shape].y[i]] * 8 + sDecorTilemaps[shape].x[i]);
    }
}

void sub_8100930(u8 decorShape)
{
    sDecorSelectorOam.y = 0;
    sDecorSelectorOam.affineMode = 0;
    sDecorSelectorOam.objMode = 0;
    sDecorSelectorOam.mosaic = 0;
    sDecorSelectorOam.bpp = 0;
    sDecorSelectorOam.shape = sDecorationMovementInfo[decorShape].shape;
    sDecorSelectorOam.x = 0;
    sDecorSelectorOam.matrixNum = 0;
    sDecorSelectorOam.size = sDecorationMovementInfo[decorShape].size;
    sDecorSelectorOam.tileNum = 0;
    sDecorSelectorOam.priority = 1;
    sDecorSelectorOam.paletteNum = 0;
}

static void InitializePuttingAwayCursorSprite(struct Sprite *sprite)
{
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    sprite->data[5] = 0;
    sprite->data[6] = 0;
    sprite->data[7] = 0;
    sprite->callback = InitializePuttingAwayCursorSprite2;
}

static void InitializePuttingAwayCursorSprite2(struct Sprite *sprite)
{
    if (sprite->data[7] == 0)
    {
        if (sprite->data[6] < 15)
        {
            sprite->invisible = FALSE;
        } else
        {
            sprite->invisible = TRUE;
        }
        sprite->data[6] = (sprite->data[6] + 1) & 0x1f;
    } else
    {
        sprite->invisible = FALSE;
    }
}

void sub_8100A0C(u8 taskId)
{
    if (sub_8100D38(taskId) == 1)
    {
        FadeScreen(1, 0);
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].func = sub_8100E70;
    } else
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecorInUse, sub_80FE428, 0);
    }
}

static void ClearDecorationContextIndex(u8 idx)
{
    sDecorationContext.items[idx] = 0;
    sDecorationContext.pos[idx] = 0;
}

void PutAwayDecorationIteration(void)
{
    u16 i;
    gSpecialVar_0x8005 = 0;
    gSpecialVar_Result = 0;
    if (gSpecialVar_0x8004 == sCurDecorSelectedInRearrangement)
    {
        gSpecialVar_Result = 1;
    } else if (gDecorations[sDecorationContext.items[sDecorRearrangementDataBuffer[gSpecialVar_0x8004].idx]].permission == DECORPERM_SPRITE)
    {
        gSpecialVar_0x8005 = sDecorRearrangementDataBuffer[gSpecialVar_0x8004].flagId;
        ClearDecorationContextIndex(sDecorRearrangementDataBuffer[gSpecialVar_0x8004].idx);
        for (i=0; i<gMapHeader.events->objectEventCount; i++)
        {
            if (gMapHeader.events->objectEvents[i].flagId == gSpecialVar_0x8005)
            {
                gSpecialVar_0x8006 = gMapHeader.events->objectEvents[i].localId;
                break;
            }
        }
    }
}

void GetObjectEventLocalIdByFlag(void)
{
    u8 i;
    for (i=0; i<gMapHeader.events->objectEventCount; i++)
    {
        if (gMapHeader.events->objectEvents[i].flagId == gSpecialVar_0x8004)
        {
            gSpecialVar_0x8005 = gMapHeader.events->objectEvents[i].localId;
            break;
        }
    }
}

void sub_8100B6C(void)
{
    u8 i;
    u8 y;
    u8 x;
    u8 posX;
    u8 posY;
    u8 permission;
    for (i=0; i<sCurDecorSelectedInRearrangement; i++)
    {
        permission = gDecorations[sDecorationContext.items[sDecorRearrangementDataBuffer[i].idx]].permission;
        posX = sDecorationContext.pos[sDecorRearrangementDataBuffer[i].idx] >> 4;
        posY = sDecorationContext.pos[sDecorRearrangementDataBuffer[i].idx] & 0xf;
        if (permission != DECORPERM_SPRITE)
        {
            if (sDecorationContext.items[sDecorRearrangementDataBuffer[i].idx] == DECOR_SAND_ORNAMENT && MapGridGetMetatileIdAt(posX + 7, posY + 7) == 0x28c)
            {
                sDecorRearrangementDataBuffer[i].height++;
            }
            for (y=0; y<sDecorRearrangementDataBuffer[i].height; y++)
            {
                for (x=0; x<sDecorRearrangementDataBuffer[i].width; x++)
                {
                    MapGridSetMetatileEntryAt(posX + 7 + x, posY + 7 - y, ((u16 *)gMapHeader.mapLayout->map)[(posX + x) + gMapHeader.mapLayout->width * (posY - y)] | 0x3000);
                }
            }
            ClearDecorationContextIndex(sDecorRearrangementDataBuffer[i].idx);
        }
    }
}

void sub_8100C88(u8 taskId)
{
    switch (gTasks[taskId].data[2])
    {
        case 0:
            sub_8100B6C();
            gTasks[taskId].data[2] = 1;
            break;
        case 1:
            if (!gPaletteFade.active)
            {
                DrawWholeMapView();
                ScriptContext_SetupScript(gUnknown_081A2F8A);
                Menu_EraseWindowRect(0, 0, 29, 19);
                gTasks[taskId].data[2] = 2;
            }
            break;
        case 2:
            LockPlayerFieldControls();
            IdentifyOwnedDecorationsCurrentlyInUseInternal(taskId);
            FadeInFromBlack();
            gTasks[taskId].data[2] = 3;
            break;
        case 3:
            if (IsWeatherNotFadingIn() == TRUE)
            {
                gTasks[taskId].data[13] = -1;
                DisplayItemMessageOnField(taskId, gSecretBaseText_DecorReturned, ContinuePuttingAwayDecorationsPrompt, 0);
            }
            break;
    }
}

bool8 sub_8100D38(u8 taskId)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] != 0)
        {
            gTasks[taskId].data[13] = i;
            return TRUE;
        }
    }
    return FALSE;
}

void SetUpPuttingAwayDecorationPlayerAvatar(void)
{
    GetPlayerFacingDirection();
    Menu_EraseWindowRect(0, 0, 29, 19);
    sDecor_CameraSpriteObjectIdx1 = gSprites[gFieldCamera.spriteId].data[0];
    LoadPlayerSpritePalette();
    gFieldCamera.spriteId = CreateSprite(&sPuttingAwayCursorSpriteTemplate, 0x78, 0x50, 0);
    if (gSaveBlock2.playerGender == MALE)
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(0xc1, SpriteCallbackDummy, 0x88, 0x48, 0);
    } else
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(0xc2, SpriteCallbackDummy, 0x88, 0x48, 0);
    }
    gSprites[sDecor_CameraSpriteObjectIdx2].oam.priority = 1;
    DestroySprite(&gSprites[sDecor_CameraSpriteObjectIdx1]);
    sDecor_CameraSpriteObjectIdx1 = gFieldCamera.spriteId;
    gSprites[sDecor_CameraSpriteObjectIdx1].oam.priority = 1;
}

void sub_8100E70(u8 taskId)
{
    s16 *data;
    data = gTasks[taskId].data;
    switch (data[2])
    {
        case 0:
            if (!gPaletteFade.active)
            {
                SetInitialPositions(taskId);
                data[2] = 1;
                data[6] = 1;
                data[5] = 1;
                Menu_DestroyCursor();
            }
            break;
        case 1:
            SetUpPuttingAwayDecorationPlayerAvatar();
            FadeInFromBlack();
            data[2] = 2;
            break;
        case 2:
            if (IsWeatherNotFadingIn() == TRUE)
            {
                data[12] = 1;
                sub_8100EEC(taskId);
            }
            break;
    }
}

void sub_8100EEC(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].invisible = FALSE;
    gSprites[sDecor_CameraSpriteObjectIdx1].callback = InitializeCameraSprite1;
    gSprites[sDecor_CameraSpriteObjectIdx2].x = 0x88;
    gSprites[sDecor_CameraSpriteObjectIdx2].y = 0x48;
    gTasks[taskId].data[10] = 0;
    gTasks[taskId].func = Task_SelectLocation;
}

static void AttemptPutAwayDecoration(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    ResetCursorMovement();
    sub_8101024(taskId);
}

void sub_8100FB4(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    ResetCursorMovement();
    gSprites[sDecor_CameraSpriteObjectIdx1].invisible = FALSE;
    gSprites[sDecor_CameraSpriteObjectIdx1].callback = SpriteCallbackDummy;
    DisplayItemMessageOnField(taskId, gSecretBaseText_StopPuttingAwayDecor, sub_810156C, 0);
}

void sub_8101024(u8 taskId)
{
    u8 metatileBehavior;
    s16 *data;
    AttemptMarkDecorUnderCursorForRemoval(taskId);
    if (sCurDecorSelectedInRearrangement != 0)
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_ReturnDecor, sub_8101518, 0);
    } else
    {
        data = gTasks[taskId].data;
        metatileBehavior = MapGridGetMetatileBehaviorAt(data[0], data[1]);
        if (MetatileBehavior_IsSecretBasePC(metatileBehavior) == TRUE
         || MetatileBehavior_IsPlayerRoomPCOn(metatileBehavior) == TRUE)
        {
            gSprites[sDecor_CameraSpriteObjectIdx1].invisible = FALSE;
            gSprites[sDecor_CameraSpriteObjectIdx1].callback = SpriteCallbackDummy;
            DisplayItemMessageOnField(taskId, gSecretBaseText_StopPuttingAwayDecor, sub_810156C, 0);
        } else
        {
            DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecor, ContinuePuttingAwayDecorationsPrompt, 0);
        }
    }
}

static void ContinuePuttingAwayDecorationsPrompt(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        sub_8100EEC(taskId);
    }
}

static void SetDecorRearrangementShape(u8 decorIdx, struct DecorRearrangementDataBuffer *data)
{
    if (gDecorations[decorIdx].shape == DECORSHAPE_1x1)
    {
        data->width = 1;
        data->height = 1;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_2x1)
    {
        data->width = 2;
        data->height = 1;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_3x1)
    {
        data->width = 3;
        data->height = 1;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_4x2)
    {
        data->width = 4;
        data->height = 2;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_2x2)
    {
        data->width = 2;
        data->height = 2;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_1x2)
    {
        data->width = 1;
        data->height = 2;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_1x3)
    {
        data->width = 1;
        data->height = 3;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_2x4)
    {
        data->width = 2;
        data->height = 4;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_3x3)
    {
        data->width = 3;
        data->height = 3;
    } else if (gDecorations[decorIdx].shape == DECORSHAPE_3x2)
    {
        data->width = 3;
        data->height = 2;
    }
}

static void SetCameraSpritePosition(u8 x, u8 y)
{
    gSprites[sDecor_CameraSpriteObjectIdx1].invisible = TRUE;
    gSprites[sDecor_CameraSpriteObjectIdx1].callback = SpriteCallbackDummy;
    gSprites[sDecor_CameraSpriteObjectIdx2].x = 0x88 + x * 16;
    gSprites[sDecor_CameraSpriteObjectIdx2].y = 0x48 + y * 16;
}

bool8 sub_8101200(u8 taskId, u8 idx, struct DecorRearrangementDataBuffer *data)
{
    u8 x;
    u8 y;
    u8 xOff;
    u8 yOff;
    x = gTasks[taskId].data[0] - 7;
    y = gTasks[taskId].data[1] - 7;
    xOff = sDecorationContext.pos[idx] >> 4;
    yOff = sDecorationContext.pos[idx] & 0xf;
    if (sDecorationContext.items[idx] == DECOR_SAND_ORNAMENT && MapGridGetMetatileIdAt(xOff + 7, yOff + 7) == 0x28c)
    {
        data->height--;
    }
    if (x >= xOff && x < xOff + data->width && y > yOff - data->height && y <= yOff)
    {
        SetCameraSpritePosition(data->width - (x - xOff + 1), yOff - y);
        return TRUE;
    }
    return FALSE;
}

static void SetDecorRearrangementFlagIdIfFlagUnset(void)
{
    u8 xOff;
    u8 yOff;
    u16 i;
    xOff = sDecorationContext.pos[sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].idx] >> 4;
    yOff = sDecorationContext.pos[sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].idx] & 0xf;
    for (i=0; i<0x40; i++)
    {
        if (gSaveBlock1.objectEventTemplates[i].x == xOff && gSaveBlock1.objectEventTemplates[i].y == yOff && !FlagGet(gSaveBlock1.objectEventTemplates[i].flagId))
        {
            sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].flagId = gSaveBlock1.objectEventTemplates[i].flagId;
            break;
        }
    }
}

static bool8 AttemptMarkSpriteDecorUnderCursorForRemoval(u8 taskId)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] != 0)
        {
            if (gDecorations[sDecorationContext.items[i]].permission == DECORPERM_SPRITE)
            {
                SetDecorRearrangementShape(sDecorationContext.items[i], sDecorRearrangementDataBuffer);
                if (sub_8101200(taskId, i, sDecorRearrangementDataBuffer) == TRUE)
                {
                    sDecorRearrangementDataBuffer->idx = i;
                    SetDecorRearrangementFlagIdIfFlagUnset();
                    sCurDecorSelectedInRearrangement = 1;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

static void MarkSpriteDecorsInBoundsForRemoval(u8 left, u8 top, u8 right, u8 bottom)
{
    u8 i;
    u8 xOff;
    u8 yOff;
    u8 decorIdx;
    for (i=0; i<sDecorationContext.size; i++)
    {
        decorIdx = sDecorationContext.items[i];
        xOff = sDecorationContext.pos[i] >> 4;
        yOff = sDecorationContext.pos[i] & 0xf;
        if (decorIdx != 0 && gDecorations[decorIdx].permission == DECORPERM_SPRITE && left <= xOff && top <= yOff && right >= xOff && bottom >= yOff)
        {
            sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].idx = i;
            SetDecorRearrangementFlagIdIfFlagUnset();
            sCurDecorSelectedInRearrangement++;
        }
    }
}

static void AttemptMarkDecorUnderCursorForRemoval(u8 taskId)
{
    u8 i;
    u8 xOff;
    u8 yOff;
    u8 var1;
    u32 var2;

    sCurDecorSelectedInRearrangement = 0;
    if (AttemptMarkSpriteDecorUnderCursorForRemoval(taskId) != TRUE)
    {
        for (i = 0; i < sDecorationContext.size; i++)
        {
            var1 = sDecorationContext.items[i];
            if (var1 != 0)
            {
                SetDecorRearrangementShape(var1, sDecorRearrangementDataBuffer);
                if (sub_8101200(taskId, i, sDecorRearrangementDataBuffer) == TRUE)
                {
                    sDecorRearrangementDataBuffer[0].idx = i;
                    sCurDecorSelectedInRearrangement++;
                    break;
                }
            }
        }
        if (sCurDecorSelectedInRearrangement != 0)
        {
            xOff = sDecorationContext.pos[sDecorRearrangementDataBuffer[0].idx] >> 4;
            yOff = sDecorationContext.pos[sDecorRearrangementDataBuffer[0].idx] & 0xf;
            var1 = yOff - sDecorRearrangementDataBuffer[0].height + 1;
            var2 = sDecorRearrangementDataBuffer[0].width + xOff - 1;

            MarkSpriteDecorsInBoundsForRemoval(xOff, var1, var2, yOff);
        }
    }
}

void sub_8101518(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sReturnDecorationYesNoFunctions);
}

static void PutAwayDecoration(u8 taskId)
{
    FadeScreen(1, 0);
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].func = sub_8100C88;
}

void sub_810156C(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sStopPuttingAwayDecorationsYesNoFunctions);
}

void sub_8101590(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    StopPuttingAwayDecorations_(taskId);
}

static void StopPuttingAwayDecorations_(u8 taskId)
{
    FadeScreen(1, 0);
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].func = Task_StopPuttingAwayDecorations;
}

static void Task_StopPuttingAwayDecorations(u8 taskId)
{
    switch (gTasks[taskId].data[2])
    {
        case 0:
            if (!gPaletteFade.active)
            {
                WarpToInitialPosition(taskId);
                gTasks[taskId].data[2] = 1;
            }
            break;
        case 1:
            FreePlayerSpritePalette();
            gFieldCallback = sub_8101678;
            SetMainCallback2(CB2_ReturnToField);
            DestroyTask(taskId);
            break;
    }
}

void sub_8101648(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        gTasks[taskId].func = Task_DecorationPCProcessMenuInput;
    }
}

void sub_8101678(void)
{
    FadeInFromBlack();
    Menu_DisplayDialogueFrame();
    InitDecorationActionsWindow();
    CreateTask(sub_8101648, 8);
}

static void InitializeCameraSprite1(struct Sprite *sprite)
{
    sprite->data[0] = (sprite->data[0] + 1) & 0x1f;
    if (sprite->data[0] >= 16)
    {
        sprite->invisible = TRUE;
    } else
    {
        sprite->invisible = FALSE;
    }
}

static void LoadPlayerSpritePalette(void)
{
    if (gSaveBlock2.playerGender == MALE)
    {
        LoadSpritePalette(&sSpritePal_PuttingAwayCursorBrendan);
    } else
    {
        LoadSpritePalette(&sSpritePal_PuttingAwayCursorMay);
    }
}

static void FreePlayerSpritePalette(void)
{
    FreeSpritePaletteByTag(8);
}

void sub_8101700(u8 taskId)
{
    if (!GetNumOwnedDecorations())
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, sub_80FE428, 0);
    } else
    {
        gTasks[taskId].data[11] = 1;
        sCurDecorationCategory = 0;
        sub_80FE5AC(taskId);
    }
}

void sub_8101750(u8 taskId)
{
    if (!GetNumOwnedDecorations())
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, sub_80FE428, 0);
    } else
    {
        gTasks[taskId].data[11] = 2;
        sCurDecorationCategory = 0;
        sub_80FE5AC(taskId);
    }
}

void sub_81017A0(u8 taskId)
{
    sub_80FEF74();
    sub_80FED1C();
    if (IsSelectedDecorInThePC() == TRUE)
    {
        StringCopy(gStringVar1, gDecorations[gCurDecorationItems[gCurDecorationIndex]].name);
        StringExpandPlaceholders(gStringVar4, gSecretBaseText_WillBeDiscarded);
        DisplayItemMessageOnField(taskId, gStringVar4, sub_8101824, 0);
    } else
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_DecorInUse, sub_80FEFF4, 0);
    }
}

void sub_8101824(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sTossDecorationYesNoFunctions);
}

void sub_8101848(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 14);
    sub_8109A30(gCurDecorationItems[gCurDecorationIndex]);
    gCurDecorationItems[gCurDecorationIndex] = DECOR_NONE;
    sub_80FF098(taskId);
    DisplayItemMessageOnField(taskId, gSecretBaseText_DecorThrownAway, sub_80FEFF4, 0);
}
