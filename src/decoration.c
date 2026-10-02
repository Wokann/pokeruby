#include "global.h"
#include "constants/decorations.h"
#include "main.h"
#include "constants/event_objects.h"
#include "constants/metatile_labels.h"
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
#include "field_fadetransition.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "event_data.h"
#include "field_weather.h"
#include "decoration.h"
#include "decoration_inventory.h"
#include "shop.h"
#include "ewram.h"

#define PLACE_DECORATION_SELECTOR_TAG 3000
#define PLACE_DECORATION_PLAYER_TAG   8
#define NUM_DECORATION_FLAGS (FLAG_DECORATION_15 - FLAG_DECORATION_2 + 1)

#define tCursorX data[0]
#define tCursorY data[1]
#define tState data[2]
#define tInitialX data[3]
#define tInitialY data[4]
#define tDecorWidth data[5]
#define tDecorHeight data[6]
#define tButton data[10]
#define tDecorationMenuCommand data[11]
#define tDecorationItemsMenuCommand data[12]
#define tDecorationContextIndex data[13]

#define DECOR_MENU_PLACE 0
#define DECOR_MENU_TOSS 1
#define DECOR_MENU_SELL 2
#define DECOR_MENU_TRADE 3

#define DECOR_ITEMS_MENU_PLACE 0
#define DECOR_ITEMS_MENU_PUT_AWAY 1

struct DecorationPCContext
{
    /* 0x00 */ u8 *items;
    /* 0x04 */ u8 *pos;
    /* 0x08 */ u8 size;
    /* 0x09 */ u8 isPlayerRoom;
};

struct DecorTilemap
{
    const u8 *tiles;
    const u8 *y;
    const u8 *x;
    u8 size;
};

struct PlaceDecorationGraphicsDataBuffer
{
    /*0x000; 0x02038900*/ const struct Decoration *decoration;
    /*0x004; 0x02038904*/ u16 tiles[0x40];
    /*0x084; 0x02038984*/ u8 image[0x800];
    /*0x884; 0x02039184*/ u16 palette[16];
};

struct DecorationMovementInfo
{
    u8 shape;
    u8 size;
    u8 cameraX;
    u8 cameraY;
};

struct DecorRearrangementDataBuffer
{
    u8 idx;
    u8 width;
    u8 height;
    u16 flagId;
};

void ExitTraderMenu(u8); // src/trader
extern void ReshowPlayerPC(u8 taskId); // src/player_pc
extern u8 SecretBase_EventScript_SetDecoration[];
extern u8 SecretBase_EventScript_PutAwayDecoration[];
void LoadDecorationMenuPalette(void);
void HandleDecorationActionsMenuInput(u8);
void PrintCurMainMenuDescription(void);
void DecorationMenuAction_Cancel(u8);
void InitDecorationCategoriesWindow(u8);
void HandleDecorationCategoriesMenuInput(u8);
void ExitDecorationCategoriesMenu(u8);
void ReturnToActionsMenuFromCategories(u8);
void ShowDecorationItemsWindow(u8);
void HandleDecorationItemsMenuInput(u8);
void UpdateDecorationItemsDisplay(u8, u8);
void ShowDecorationItemsListWindow(u8);
void ShowDecorationCategorySummaryWindow(u8);
#ifdef GERMAN
int PrintDecorationItemDescription(u8);
#else
void PrintDecorationItemDescription(u8);
#endif
void InitDecorationItemsMenuScrollAndCursor(u8);
void Task_PlaceDecoration(u8);
void SetUpDecorationShape(u8);
void AddDecorationIconObjectFromObjectEvent(struct PlaceDecorationGraphicsDataBuffer *, u8);
void SetUpPlacingDecorationPlayerAvatar(u8, struct PlaceDecorationGraphicsDataBuffer *);
void AttemptPlaceDecoration_(u8);
void PlaceDecorationPrompt(u8);
void PlaceDecoration_(u8);
void CancelDecoratingPrompt(u8);
void CancelDecorating_(u8);
void c1_overworld_prev_quest(u8);
void FieldCB_InitDecorationItemsWindow(void);
void ResetCursorMovement(void);
void ContinueDecorating(u8);
void CantPlaceDecorationPrompt(u8);
void ClearPlaceDecorationTileBuffers(struct PlaceDecorationGraphicsDataBuffer *);
void CopyPalette(u16 *, u16);
void SetDecorSelectionBoxTiles(struct PlaceDecorationGraphicsDataBuffer *);
void SetDecorSelectionMetatiles(struct PlaceDecorationGraphicsDataBuffer *);
void SetDecorSelectionBoxOamAttributes(u8);
void InitializeDecorationCursorSprite(struct Sprite *);
void UpdateDecorationCursorBlink(struct Sprite *);
bool8 HasDecorationsInUse(u8);
void Task_ContinuePuttingAwayDecorations(u8);
void ContinuePuttingAwayDecorations(u8);
void AttemptPutAwayDecoration_(u8);
void ContinuePuttingAwayDecorationsPrompt(u8);
void AttemptMarkDecorUnderCursorForRemoval(u8);
void ReturnDecorationPrompt(u8);
void StopPuttingAwayDecorations_(u8);
void Task_StopPuttingAwayDecorations(u8);
void StopPuttingAwayDecorationsPrompt(u8);
void FieldCB_StopPuttingAwayDecorations(void);
void InitializeCameraSprite1(struct Sprite *);
void LoadPlayerSpritePalette(void);
void FreePlayerSpritePalette(void);
void TossDecorationPrompt(u8);
void SetRecycledDecoration(u8);

void DecorationMenuAction_Decorate(u8);
void DecorationItemsMenuAction_AttemptPlace(u8);
void DecorationItemsMenuAction_Cancel(u8);
void DecorationMenuAction_PutAway(u8);
void DecorationMenuAction_Toss(u8);
void DecorationItemsMenuAction_AttemptToss(u8);
void DecorationItemsMenuAction_Trade(u8);

void ReturnToDecorationItemsMenu(u8);
void AttemptPlaceDecoration(u8);
void AttemptCancelPlaceDecoration(u8);
void PlaceDecoration(u8);
void CancelDecorating(u8);
void AttemptPutAwayDecoration(u8);
void AttemptCancelPutAwayDecoration(u8);
void PutAwayDecoration(u8);
void StopPuttingAwayDecorations(u8);
void TossDecoration(u8);

EWRAM_DATA u8 *gCurDecorationItems = NULL;
EWRAM_DATA static u8 sDecorationActionsCursorPos = 0;
EWRAM_DATA static u8 sNumOwnedDecorationsInCurCategory = 0;
EWRAM_DATA static u8 sSecretBaseItemsIndicesBuffer[DECOR_MAX_SECRET_BASE] = {0};
EWRAM_DATA static u8 sPlayerRoomItemsIndicesBuffer[DECOR_MAX_PLAYERS_HOUSE] = {0};
EWRAM_DATA static u8 sDecorationsCursorPos = 0;
EWRAM_DATA static u8 sDecorationsCursorMax = 0;
EWRAM_DATA static u8 sDecorationsScrollOffset = 0;
EWRAM_DATA u8 gCurDecorationIndex = 0;
EWRAM_DATA static u8 sCurDecorationCategory = DECORCAT_DESK;
EWRAM_DATA u8 gDecorationMarkerSpriteIds[8] = {0};
EWRAM_DATA static struct PlaceDecorationGraphicsDataBuffer sPlaceDecorationGraphicsDataBuffer = {0};
EWRAM_DATA static u16 sCurDecorMapX = 0;
EWRAM_DATA static u16 sCurDecorMapY = 0;
EWRAM_DATA static u8 sDecor_CameraSpriteObjectIdx1 = 0;
EWRAM_DATA static u8 sDecor_CameraSpriteObjectIdx2 = 0;
EWRAM_DATA static u8 sDecorationLastDirectionMoved = 0;
EWRAM_DATA static struct OamData sDecorSelectorOam = {0};
EWRAM_DATA static struct DecorRearrangementDataBuffer sDecorRearrangementDataBuffer[DECOR_MAX_SECRET_BASE] = {0};
EWRAM_DATA static u8 sCurDecorSelectedInRearrangement = 0;

static const struct DecorationMovementInfo sDecorationMovementInfo[];
static const struct SpritePalette sSpritePal_PlaceDecoration;
static const struct SpriteTemplate sDecorationSelectorSpriteTemplate;
static const struct YesNoFuncTable sPlaceDecorationYesNoFunctions;
static const struct YesNoFuncTable sCancelDecoratingYesNoFunctions;
static const struct YesNoFuncTable sPlacePutAwayYesNoFunctions[];
static const u8 sDecorationStandElevations[];
static const u8 sDecorationSlideElevation[];

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
    gText_Desk,
    gText_Chair,
    gText_Plant,
    gText_Ornament,
    gText_Mat,
    gText_Poster,
    gText_Doll,
    gText_Cushion
};

static const struct MenuAction2 sDecorationMainMenuActions[] = {
    {gText_Decorate, DecorationMenuAction_Decorate},
    {gText_PutAway, DecorationMenuAction_PutAway},
    {gText_Toss2, DecorationMenuAction_Toss},
    {gText_Exit, DecorationMenuAction_Cancel}
};

static const u8 *const sSecretBasePCMenuItemDescriptions[] = {
    gText_PutOutSelectedDecorItem,
    gText_StoreChosenDecorInPC,
    gText_ThrowAwayUnwantedDecors,
    gText_GoBackPrevMenu
};

static const struct YesNoFuncTable sDecorationItemsMenuActions[] = {
    {DecorationItemsMenuAction_AttemptPlace, DecorationItemsMenuAction_Cancel},
    {DecorationItemsMenuAction_AttemptToss, DecorationItemsMenuAction_Cancel},
    {DecorationItemsMenuAction_AttemptToss, DecorationItemsMenuAction_Cancel},
    {DecorationItemsMenuAction_Trade, DecorationItemsMenuAction_Cancel}
};

// text

extern u8 SecretBase_EventScript_PCCancel[];

void AddDecorationActionsWindow(void)
{
    LoadDecorationMenuPalette();
    Menu_DrawStdWindowFrame(0, 0, 10, 9);
    Menu_PrintItems(1, 1, 4, (const struct MenuAction *)sDecorationMainMenuActions);
    InitMenu(0, 1, 1, 4, sDecorationActionsCursorPos, 9);
}

void InitDecorationActionsWindow(void)
{
    sDecorationActionsCursorPos = 0;
    LockPlayerFieldControls();
    AddDecorationActionsWindow();
    PrintCurMainMenuDescription();
}

void OpenDecorationActionsMenu(u8 taskId)
{
    InitDecorationActionsWindow();
    gTasks[taskId].func = HandleDecorationActionsMenuInput;
}

void DoSecretBaseDecorationMenu(u8 taskId)
{
    OpenDecorationActionsMenu(taskId);
    sDecorationContext.items = gSaveBlock1.secretBases[0].decorations;
    sDecorationContext.pos = gSaveBlock1.secretBases[0].decorationPos;
    sDecorationContext.size = sizeof gSaveBlock1.secretBases[0].decorations;
    sDecorationContext.isPlayerRoom = 0;
}

void DoPlayerRoomDecorationMenu(u8 taskId)
{
    OpenDecorationActionsMenu(taskId);
    sDecorationContext.items = gSaveBlock1.playerRoomDecor;
    sDecorationContext.pos = gSaveBlock1.playerRoomDecorPos;
    sDecorationContext.size = sizeof gSaveBlock1.playerRoomDecor;
    sDecorationContext.isPlayerRoom = 1;
}

void LoadDecorationMenuPalette(void)
{
    u16 palettes[3] = {RGB(24, 24, 24),
                       RGB(28, 28, 28),
                       RGB(31, 31, 31)};
    LoadPalette(&palettes[2], 0xdf, 2);
    LoadPalette(&palettes[1], 0xd1, 2);
    LoadPalette(&palettes[0], 0xd8, 2);
}

void HandleDecorationActionsMenuInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            sDecorationActionsCursorPos = Menu_MoveCursor(-1);
            PrintCurMainMenuDescription();
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            sDecorationActionsCursorPos = Menu_MoveCursor(1);
            PrintCurMainMenuDescription();
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sDecorationMainMenuActions[sDecorationActionsCursorPos].func(taskId);
        } else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            DecorationMenuAction_Cancel(taskId);
        }
    }
}

void PrintCurMainMenuDescription(void)
{
    Menu_BlankWindowRect(2, 15, 27, 18);
    Menu_PrintText(sSecretBasePCMenuItemDescriptions[sDecorationActionsCursorPos], 2, 15);
}

void DecorationMenuAction_Cancel(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 10, 9);
    Menu_BlankWindowRect(2, 15, 27, 18);
    FreeSpritePaletteByTag(6);
    if (sDecorationContext.isPlayerRoom == FALSE)
    {
        ScriptContext_SetupScript(SecretBase_EventScript_PCCancel);
        DestroyTask(taskId);
    } else
    {
        ReshowPlayerPC(taskId);
    }
}

void ReturnToDecorationCategoriesAfterInvalidSelection(u8 taskId)
{
    InitDecorationCategoriesWindow(taskId);
}

void ReturnToDecorationActionsAfterInvalidSelection(u8 taskId)
{
    InitMenu(0, 1, 1, 4, sDecorationActionsCursorPos, 9);
    PrintCurMainMenuDescription();
    gTasks[taskId].func = HandleDecorationActionsMenuInput;
}

void PrintDecorationCategoryMenuItem(u8 category, u8 left, u8 top, u8 textPalette) // PrintDecorationCategorySelectionMenuString
{
    u8 *str;
    u8 defaultPaletteNum;

    defaultPaletteNum = Menu_GetTextWindowPaletteNum();
    str = gStringVar4;
    str[0] = EXT_CTRL_CODE_BEGIN;
    str[1] = 5;
    str[2] = textPalette;
    str += 3;
    str = StringCopy(str, sDecorationCategoryNames[category]);
    str = AlignInt1InMenuWindow(str, GetNumOwnedDecorationsInCategory(category), 0x56, 1);
    *str++ = 0xba;
    str = AlignInt1InMenuWindow(str, gDecorationInventories[category].size, 0x68, 1);
    str[0] = EXT_CTRL_CODE_BEGIN;
    str[1] = 5;
    str[2] = defaultPaletteNum;
    str[3] = EOS;
    Menu_PrintText(gStringVar4, left, top);
}

void PrintDecorationCategoryMenuItems(u8 taskId) // PrintDecorationCategorySelectionMenuStrings
{
    u8 category;
    Menu_DrawStdWindowFrame(0, 0, 14, 19);
    for (category = 0; category < DECORCAT_COUNT; category++)
    {
        if (sDecorationContext.isPlayerRoom == TRUE && gTasks[taskId].tDecorationMenuCommand == DECOR_MENU_PLACE && category != DECORCAT_DOLL && category != DECORCAT_CUSHION)
        {
            PrintDecorationCategoryMenuItem(category, 1, 2 * category + 1, 13); // Selectable
        } else
        {
            PrintDecorationCategoryMenuItem(category, 1, 2 * category + 1, 255); // Unselectable
        }
    }
    Menu_PrintText(gText_Exit, 1, 17);
}

void InitDecorationCategoriesWindow(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);
    PrintDecorationCategoryMenuItems(taskId);
    InitMenu(0, 1, 1, 9, sCurDecorationCategory, 13);
    gTasks[taskId].func = HandleDecorationCategoriesMenuInput;
}

void HandleDecorationCategoriesMenuInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(-1);
        } else if (JOY_REPT(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(1);
        } else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sCurDecorationCategory = Menu_GetCursorPos();
            if (sCurDecorationCategory != DECORCAT_COUNT)
            {
                sNumOwnedDecorationsInCurCategory = GetNumOwnedDecorationsInCategory(sCurDecorationCategory);
                if (sNumOwnedDecorationsInCurCategory)
                {
                    CondenseDecorationsInCategory(sCurDecorationCategory);
                    gCurDecorationItems = gDecorationInventories[sCurDecorationCategory].items;
                    InitDecorationItemsMenuScrollAndCursor(taskId);
                    ResetScrollIndicatorState();
                    InitDecorationMarkerSpriteIds(gDecorationMarkerSpriteIds, ARRAY_COUNT(gDecorationMarkerSpriteIds));
                    LoadScrollIndicatorPalette();
                    gTasks[taskId].func = ShowDecorationItemsWindow;
                } else
                {
                    Menu_DestroyCursor();
                    Menu_EraseWindowRect(0, 0, 14, 19);
                    DisplayItemMessageOnField(taskId, gText_NoDecorations, ReturnToDecorationCategoriesAfterInvalidSelection, 0);
                }
            } else
            {
                ExitDecorationCategoriesMenu(taskId);
            }
        } else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            ExitDecorationCategoriesMenu(taskId);
        }
    }
}

void ExitDecorationCategoriesMenu(u8 taskId)
{
    if (gTasks[taskId].tDecorationMenuCommand != DECOR_MENU_TRADE)
    {
        ReturnToActionsMenuFromCategories(taskId);
    } else
    {
        ExitTraderMenu(taskId);
    }
}

void ReturnToActionsMenuFromCategories(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 14, 19);
    if (gTasks[taskId].tDecorationMenuCommand != DECOR_MENU_SELL)
    {
        AddDecorationActionsWindow();
        Menu_DisplayDialogueFrame();
        PrintCurMainMenuDescription();
        gTasks[taskId].func = HandleDecorationActionsMenuInput;
    } else
    {
        Shop_RunExitSellMenuTask(taskId);
    }
}

void ShowDecorationCategoriesWindow(u8 taskId)
{
    gTasks[taskId].tDecorationMenuCommand = DECOR_MENU_TRADE;
    sCurDecorationCategory = DECORCAT_DESK;
    InitDecorationCategoriesWindow(taskId);
}

void CopyDecorationCategoryName(u8 *dest, u8 category)
{
    StringCopy(dest, sDecorationCategoryNames[category]);
}

void InitDecorationItemsWindow(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);

    ShowDecorationItemsListWindow(taskId);
    ShowDecorationCategorySummaryWindow(sCurDecorationCategory);

#if ENGLISH
    Menu_DrawStdWindowFrame(15, 12, 29, 19);
#elif GERMAN
    if ((sDecorationsCursorPos + sDecorationsScrollOffset) != sNumOwnedDecorationsInCurCategory)
    {
        Menu_DrawStdWindowFrame(15, 12, 29, 19);
    }
#endif

    PrintDecorationItemDescription(sDecorationsCursorPos + sDecorationsScrollOffset);
    InitMenu(0, 1, 2, sDecorationsCursorMax + 1, sDecorationsCursorPos, 13);
}

void ShowDecorationItemsWindow(u8 taskId)
{
    InitDecorationItemsWindow(taskId);
    gTasks[taskId].func = HandleDecorationItemsMenuInput;
}

void MoveDecorationItemsCursor(u8 taskId /*r8*/, s8 cursorVector /*r5*/, s8 bgVector /*r7*/)
{
    int wasAtCancelItem /*r10*/;
    u8 scrollOffset;

    wasAtCancelItem = sDecorationsCursorPos + sDecorationsScrollOffset == sNumOwnedDecorationsInCurCategory;
    PlaySE(SE_SELECT);
    if (cursorVector != 0)
    {
        sDecorationsCursorPos = Menu_MoveCursor(cursorVector);
    }
    if (bgVector != 0)
    {
        scrollOffset = sDecorationsScrollOffset;
        sDecorationsScrollOffset = scrollOffset + bgVector;
        UpdateDecorationItemsDisplay(taskId, 1);
    }
    if (sDecorationsCursorPos + sDecorationsScrollOffset != sNumOwnedDecorationsInCurCategory)
    {
        if (wasAtCancelItem)
        {
            Menu_DrawStdWindowFrame(15, 12, 29, 19);
        }
        PrintDecorationItemDescription(sDecorationsCursorPos + sDecorationsScrollOffset);
    } else
    {
        Menu_EraseWindowRect(15, 12, 29, 19);
    }
}

void HandleDecorationItemsMenuInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_ANY) == DPAD_UP)
        {
            if (sDecorationsCursorPos != 0)
            {
                MoveDecorationItemsCursor(taskId, -1, 0);
            } else if (sDecorationsScrollOffset != 0)
            {
                MoveDecorationItemsCursor(taskId, 0, -1);
            }
        }
        if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
        {
            if (sDecorationsCursorPos != sDecorationsCursorMax)
            {
                MoveDecorationItemsCursor(taskId, 1, 0);
            } else if (sDecorationsScrollOffset + sDecorationsCursorPos != sNumOwnedDecorationsInCurCategory)
            {
                MoveDecorationItemsCursor(taskId, 0, 1);
            }
        }
        if (JOY_NEW(A_BUTTON))
        {
            Menu_DestroyCursor();
            PlaySE(SE_SELECT);
            gCurDecorationIndex = sDecorationsCursorPos + sDecorationsScrollOffset;
            if (gCurDecorationIndex == sNumOwnedDecorationsInCurCategory)
            {
                sDecorationItemsMenuActions[gTasks[taskId].tDecorationMenuCommand].noFunc(taskId);
            } else
            {
                sDecorationItemsMenuActions[gTasks[taskId].tDecorationMenuCommand].yesFunc(taskId);
            }
        } else if (JOY_NEW(B_BUTTON))
        {
            Menu_DestroyCursor();
            PlaySE(SE_SELECT);
            sDecorationItemsMenuActions[gTasks[taskId].tDecorationMenuCommand].noFunc(taskId);
        }
    }
}

static const u8 sText_DisabledDecorationItem[] = _("{PALETTE 13}{STR_VAR_1}");

void UpdateDecorationItemsDisplay(u8 taskId, u8 unused)
{
    u16 i;
    u16 j;
    u8 row;
    if (sDecorationsScrollOffset != 0 || (DestroyScrollIndicatorArrow(TOP_ARROW), sDecorationsScrollOffset != 0))
    {
        CreateScrollIndicatorArrow(TOP_ARROW, 0x3c, 0x08);
    }
    if (sDecorationsScrollOffset + 7 == sNumOwnedDecorationsInCurCategory)
    {
        DestroyScrollIndicatorArrow(BOTTOM_ARROW);
    }
    if (sDecorationsScrollOffset + 7 < sNumOwnedDecorationsInCurCategory)
    {
        CreateScrollIndicatorArrow(BOTTOM_ARROW, 0x3c, 0x98);
    }
    for (i=sDecorationsScrollOffset; i<sDecorationsScrollOffset+8; i++)
    {
        row = 2 * (i - sDecorationsScrollOffset) + 2;
        if (gDecorationMarkerSpriteIds[i - sDecorationsScrollOffset])
        {
            DestroyDecorationMarkerSprite(&gDecorationMarkerSpriteIds[i - sDecorationsScrollOffset]);
        }
        if (i == sNumOwnedDecorationsInCurCategory)
        {
            Menu_PrintTextWithNewlineIndent(gText_Exit, 0x08, 8 * row, 0x68, 1);
            break;
        }
        if (gCurDecorationItems[i])
        {
            if (sDecorationContext.isPlayerRoom == TRUE && sCurDecorationCategory != DECORCAT_DOLL && sCurDecorationCategory != DECORCAT_CUSHION && gTasks[taskId].tDecorationMenuCommand == DECOR_MENU_PLACE)
            {
                StringCopy(gStringVar1, gDecorations[gCurDecorationItems[i]].name);
                Menu_PrintTextWithNewlineIndent(sText_DisabledDecorationItem, 0x08, 8 * row, 0x68, 1);
            } else
            {
                Menu_PrintTextWithNewlineIndent(gDecorations[gCurDecorationItems[i]].name, 0x08, 8 * row, 0x68, 1);
            }
            for (j = 0; j < ARRAY_COUNT(sSecretBaseItemsIndicesBuffer); j++)
            {
                if (sSecretBaseItemsIndicesBuffer[j] - 1 == i)
                {
                    CreateDecorationMarkerSprite(4, &gDecorationMarkerSpriteIds[i - sDecorationsScrollOffset], 0x6c, (i - sDecorationsScrollOffset) * 16 + 24);
                    break;
                }
            }
            for (j = 0; j < ARRAY_COUNT(sPlayerRoomItemsIndicesBuffer); j++)
            {
                if (sPlayerRoomItemsIndicesBuffer[j] - 1 == i)
                {
                    CreateDecorationMarkerSprite(5, &gDecorationMarkerSpriteIds[i - sDecorationsScrollOffset], 0x6c, (i - sDecorationsScrollOffset) * 16 + 24);
                    break;
                }
            }
        }
    }
}

void ShowDecorationItemsListWindow(u8 taskId)
{
    Menu_DrawStdWindowFrame(0, 0, 14, 19);
    UpdateDecorationItemsDisplay(taskId, 0);
}

void ShowDecorationCategorySummaryWindow(u8 category)
{
    Menu_DrawStdWindowFrame(15, 0, 29, 3);
    PrintDecorationCategoryMenuItem(category, 16, 1, 0xff);
}

#if ENGLISH
void PrintDecorationItemDescription(u8 itemIndex)
{
    Menu_PrintTextWithNewlineIndentAndClear(gDecorations[gCurDecorationItems[itemIndex]].description, 0x80, 0x68, 0x68, 0x30, 0x1);
}
#elif GERMAN
int PrintDecorationItemDescription(u8 itemIndex)
{
    if (itemIndex == sNumOwnedDecorationsInCurCategory)
        return FALSE;

    Menu_PrintTextWithNewlineIndentAndClear(gDecorations[gCurDecorationItems[itemIndex]].description, 0x80, 0x68, 0x68, 0x30, 0x1);
    return TRUE;
}
#endif

void RemoveDecorationItemsOtherWindows(void)
{
    Menu_EraseWindowRect(15, 0, 29, 3);
    Menu_EraseWindowRect(15, 12, 29, 19);
}

void ReturnToDecorationItemsMenu(u8 taskId)
{
    LoadScrollIndicatorPalette();
    gTasks[taskId].func = ShowDecorationItemsWindow;
}

bool8 IsDecorationIndexInSecretBase(u8 idx)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(sSecretBaseItemsIndicesBuffer); i++)
    {
        if (sSecretBaseItemsIndicesBuffer[i] == idx)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void IdentifyOwnedDecorationsCurrentlyInUse(u8 taskId)
{
    u16 i;
    u16 j;
    u16 k;
    u16 count;

    count = 0;
    for (i = 0; i < ARRAY_COUNT(sSecretBaseItemsIndicesBuffer); i++)
    {
        sSecretBaseItemsIndicesBuffer[i] = 0;
        if (i < ARRAY_COUNT(sPlayerRoomItemsIndicesBuffer))
        {
            sPlayerRoomItemsIndicesBuffer[i] = 0;
        }
    }
    for (i = 0; i < ARRAY_COUNT(sSecretBaseItemsIndicesBuffer); i++)
    {
        if (gSaveBlock1.secretBases[0].decorations[i] != 0)
        {
            for (j=0; j<gDecorationInventories[sCurDecorationCategory].size; j++)
            {
                if (gCurDecorationItems[j] == gSaveBlock1.secretBases[0].decorations[i])
                {
                    for (k = 0; k < count && sSecretBaseItemsIndicesBuffer[k] != j + 1; k++);
                    if (k == count)
                    {
                        sSecretBaseItemsIndicesBuffer[count] = j + 1;
                        count++;
                        break;
                    }
                }
            }
        }
    }
    count = 0;
    for (i = 0; i < ARRAY_COUNT(sPlayerRoomItemsIndicesBuffer); i++)
    {
        if (gSaveBlock1.playerRoomDecor[i] != 0)
        {
            for (j=0; j<gDecorationInventories[sCurDecorationCategory].size; j++)
            {
                if (gCurDecorationItems[j] == gSaveBlock1.playerRoomDecor[i] && !IsDecorationIndexInSecretBase(j + 1))
                {
                    for (k = 0; k < count && sPlayerRoomItemsIndicesBuffer[k] != j + 1; k++);
                    if (k == count)
                    {
                        sPlayerRoomItemsIndicesBuffer[count] = j + 1;
                        count++;
                        break;
                    }
                }
            }
        }
    }
}

void InitDecorationItemsMenuLimits(void)
{
    if (sNumOwnedDecorationsInCurCategory <= 7)
    {
        sDecorationsCursorMax = sNumOwnedDecorationsInCurCategory;
    } else
    {
        sDecorationsCursorMax = 7;
    }
}

void InitDecorationItemsMenuScrollAndCursor(u8 taskId)
{
    IdentifyOwnedDecorationsCurrentlyInUse(taskId);
    InitDecorationItemsMenuLimits();
    sDecorationsCursorPos = 0;
    sDecorationsScrollOffset = 0;
}

void RemoveDecorationItemsWindow(void)
{
    DestroyDecorationMarkerSprites(gDecorationMarkerSpriteIds, ARRAY_COUNT(gDecorationMarkerSpriteIds));
    DestroyScrollIndicatorArrow(TOP_ARROW);
    DestroyScrollIndicatorArrow(BOTTOM_ARROW);
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 14, 19);
}

bool8 IsSelectedDecorInThePC(void)
{
    u16 i;
    int selectedIndex;
    for (i = 0; i < ARRAY_COUNT(sSecretBaseItemsIndicesBuffer); i++)
    {
        selectedIndex = sDecorationsScrollOffset + sDecorationsCursorPos + 1;
        if (sSecretBaseItemsIndicesBuffer[i] == selectedIndex || (i < ARRAY_COUNT(sPlayerRoomItemsIndicesBuffer) && sPlayerRoomItemsIndicesBuffer[i] == selectedIndex))
        {
            return FALSE;
        }
    }
    return TRUE;
}

void ReturnToDecorationItemsAfterInvalidSelection(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        LoadScrollIndicatorPalette();
        gTasks[taskId].func = ShowDecorationItemsWindow;
    }
}

void ReinitDecorationCategoriesWindow(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 14, 19);
    InitDecorationCategoriesWindow(taskId);
}

void DecorationItemsMenuAction_Cancel(u8 taskId)
{
    DestroyDecorationMarkerSprites(gDecorationMarkerSpriteIds, ARRAY_COUNT(gDecorationMarkerSpriteIds));
    DestroyScrollIndicatorArrow(TOP_ARROW);
    DestroyScrollIndicatorArrow(BOTTOM_ARROW);
    FreeScrollIndicatorPalette();
    gTasks[taskId].func = ReinitDecorationCategoriesWindow;
}

void RefreshDecorationItemsAfterToss(u8 taskId)
{
    sNumOwnedDecorationsInCurCategory--;
    if (sDecorationsScrollOffset + 7 > sNumOwnedDecorationsInCurCategory && sDecorationsScrollOffset != 0)
    {
        sDecorationsScrollOffset--;
    }
    CondenseDecorationsInCategory(sCurDecorationCategory);
    IdentifyOwnedDecorationsCurrentlyInUse(taskId);
    InitDecorationItemsMenuLimits();
}

void SetInitialPositions(u8 taskId)
{
    gTasks[taskId].tInitialX = gSaveBlock1.pos.x;
    gTasks[taskId].tInitialY = gSaveBlock1.pos.y;
    PlayerGetDestCoords(&gTasks[taskId].tCursorX, &gTasks[taskId].tCursorY);
}

void WarpToInitialPosition(u8 taskId)
{
    DrawWholeMapView();
    SetWarpDestination(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1, gTasks[taskId].tInitialX, gTasks[taskId].tInitialY);
    WarpIntoMap();
}
void DecorationMenuAction_Decorate(u8 taskId)
{
    if (!GetNumOwnedDecorations())
    {
        DisplayItemMessageOnField(taskId, gText_NoDecorations, ReturnToDecorationActionsAfterInvalidSelection, 0);
    } else
    {
        gTasks[taskId].tDecorationMenuCommand = DECOR_MENU_PLACE;
        sCurDecorationCategory = DECORCAT_DESK;
        InitDecorationCategoriesWindow(taskId);
    }
}

u16 GetDecorationElevation(u8 decoration, u8 tileIndex)
{
    u16 elevation;

    elevation = ELEVATION_INVALID;

    switch (decoration)
    {
        case DECOR_STAND:
            elevation = sDecorationStandElevations[tileIndex] << MAPGRID_ELEVATION_SHIFT;
            return elevation;
        case DECOR_SLIDE:
            elevation = sDecorationSlideElevation[tileIndex] << MAPGRID_ELEVATION_SHIFT;
            return elevation;
        default:
            return elevation;
    }
}

void ShowDecorationOnMap_(u16 mapX, u16 mapY, u8 decWidth, u8 decHeight, u16 decoration)
{
    u16 i;
    u16 j;
    u16 attributes;
    u16 impassableFlag;
    u16 overlapsWall;
    u16 elevation;
    s16 x;
    s16 y;

    for (i=0; i<decHeight; i++)
    {
        y = mapY - decHeight + 1 + i;
        for (j=0; j<decWidth; j++)
        {
            x = mapX + j;
            attributes = GetMetatileAttributesById(NUM_TILES_IN_PRIMARY + gDecorations[decoration].tiles[i * decWidth + j]);
            if (MetatileBehavior_IsSecretBaseImpassable(attributes) == TRUE || (gDecorations[decoration].permission != DECORPERM_PASS_FLOOR && (attributes >> METATILE_ATTR_LAYER_SHIFT) != METATILE_LAYER_TYPE_NORMAL))
                impassableFlag = MAPGRID_IMPASSABLE;
            else
                impassableFlag = 0;

            if (gDecorations[decoration].permission != DECORPERM_NA_WALL && MetatileBehavior_IsSecretBaseNorthWall(MapGridGetMetatileBehaviorAt(x, y)) == TRUE)
                overlapsWall = 1;
            else
                overlapsWall = 0;

            elevation = GetDecorationElevation(gDecorations[decoration].id, i * decWidth + j);
            if (elevation != ELEVATION_INVALID)
            {
                MapGridSetMetatileEntryAt(x, y, (gDecorations[decoration].tiles[i * decWidth + j] + (NUM_TILES_IN_PRIMARY | overlapsWall)) | impassableFlag | elevation);
            } else
            {
                MapGridSetMetatileIdAt(x, y, (gDecorations[decoration].tiles[i * decWidth + j] + (NUM_TILES_IN_PRIMARY | overlapsWall)) | impassableFlag);
            }
        }
    }
}

void ShowDecorationOnMap(u16 mapX, u16 mapY, u16 decoration)
{
    switch (gDecorations[decoration].shape)
    {
        case DECORSHAPE_1x1:
            ShowDecorationOnMap_(mapX, mapY, 1, 1, decoration);
            break;
        case DECORSHAPE_2x1:
            ShowDecorationOnMap_(mapX, mapY, 2, 1, decoration);
            break;
        case DECORSHAPE_3x1: // unused
            ShowDecorationOnMap_(mapX, mapY, 3, 1, decoration);
            break;
        case DECORSHAPE_4x2:
            ShowDecorationOnMap_(mapX, mapY, 4, 2, decoration);
            break;
        case DECORSHAPE_2x2:
            ShowDecorationOnMap_(mapX, mapY, 2, 2, decoration);
            break;
        case DECORSHAPE_1x2:
            ShowDecorationOnMap_(mapX, mapY, 1, 2, decoration);
            break;
        case DECORSHAPE_1x3: // unused
            ShowDecorationOnMap_(mapX, mapY, 1, 3, decoration);
            break;
        case DECORSHAPE_2x4:
            ShowDecorationOnMap_(mapX, mapY, 2, 4, decoration);
            break;
        case DECORSHAPE_3x3:
            ShowDecorationOnMap_(mapX, mapY, 3, 3, decoration);
            break;
        case DECORSHAPE_3x2:
            ShowDecorationOnMap_(mapX, mapY, 3, 2, decoration);
            break;
    }
}

void SetDecoration(void)
{
    u8 decorationFlagOffset;
    u8 objectEventIndex;

    for (decorationFlagOffset = 0; decorationFlagOffset < NUM_DECORATION_FLAGS; decorationFlagOffset++)
    {
        if (FlagGet(FLAG_DECORATION_2 + decorationFlagOffset) == TRUE)
        {
            FlagClear(FLAG_DECORATION_2 + decorationFlagOffset);
            for (objectEventIndex = 0; objectEventIndex < gMapHeader.events->objectEventCount; objectEventIndex++)
            {
                if (gMapHeader.events->objectEvents[objectEventIndex].flagId == FLAG_DECORATION_2 + decorationFlagOffset)
                {
                    break;
                }
            }
            VarSet(VAR_OBJ_GFX_ID_0 + (gMapHeader.events->objectEvents[objectEventIndex].graphicsId - OBJ_EVENT_GFX_VAR_0), sPlaceDecorationGraphicsDataBuffer.decoration->tiles[0]);
            gSpecialVar_0x8005 = gMapHeader.events->objectEvents[objectEventIndex].localId;
            gSpecialVar_0x8006 = sCurDecorMapX;
            gSpecialVar_0x8007 = sCurDecorMapY;
            TrySpawnObjectEvent(gSpecialVar_0x8005, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
            TryMoveObjectEventToMapCoords(gSpecialVar_0x8005, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, gSpecialVar_0x8006, gSpecialVar_0x8007);
            TryOverrideObjectEventTemplateCoords(gSpecialVar_0x8005, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
            break;
        }
    }
}

bool8 HasDecorationSpace(void)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] == DECOR_NONE)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void DecorationItemsMenuAction_AttemptPlace(u8 taskId)
{
    if (sDecorationContext.isPlayerRoom == TRUE && sCurDecorationCategory != DECORCAT_DOLL && sCurDecorationCategory != DECORCAT_CUSHION)
    {
        RemoveDecorationItemsWindow();
        RemoveDecorationItemsOtherWindows();
        DisplayItemMessageOnField(taskId, gText_CantPlaceInRoom, ReturnToDecorationItemsAfterInvalidSelection, 0);
    } else if (IsSelectedDecorInThePC() == TRUE)
    {
        if (HasDecorationSpace() == TRUE)
        {
            FadeScreen(FADE_TO_BLACK, 0);
            gTasks[taskId].tState = 0;
            gTasks[taskId].func = Task_PlaceDecoration;
        } else
        {
            RemoveDecorationItemsWindow();
            RemoveDecorationItemsOtherWindows();
            ConvertIntToDecimalStringN(gStringVar1, sDecorationContext.size, STR_CONV_MODE_RIGHT_ALIGN, 2);
            if (!sDecorationContext.isPlayerRoom)
            {
                StringExpandPlaceholders(gStringVar4, gSecretBaseText_NoMoreDecor);
            } else
            {
                StringExpandPlaceholders(gStringVar4, gSecretBaseText_NoMoreDecor2);
            }
            DisplayItemMessageOnField(taskId, gStringVar4, ReturnToDecorationItemsAfterInvalidSelection, 0);
        }
    } else
    {
        RemoveDecorationItemsWindow();
        RemoveDecorationItemsOtherWindows();
        DisplayItemMessageOnField(taskId, gText_InUseAlready, ReturnToDecorationItemsAfterInvalidSelection, 0);
    }
}

void Task_PlaceDecoration(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
        case 0:
            if (!gPaletteFade.active)
            {
                SetInitialPositions(taskId);
                DestroyScrollIndicatorArrow(TOP_ARROW);
                DestroyScrollIndicatorArrow(BOTTOM_ARROW);
                DestroyDecorationMarkerSprites(gDecorationMarkerSpriteIds, ARRAY_COUNT(gDecorationMarkerSpriteIds));
                FreeScrollIndicatorPalette();
                gTasks[taskId].tState = 1;
            }
            break;
        case 1:
            gPaletteFade.bufferTransferDisabled = 1;
            AddDecorationIconObjectFromObjectEvent(&sPlaceDecorationGraphicsDataBuffer, gCurDecorationItems[gCurDecorationIndex]);
            SetUpDecorationShape(taskId);
            SetUpPlacingDecorationPlayerAvatar(taskId, &sPlaceDecorationGraphicsDataBuffer);
            FadeInFromBlack();
            gPaletteFade.bufferTransferDisabled = 0;
            gTasks[taskId].tState = 2;
            break;
        case 2:
            if (IsWeatherNotFadingIn() == TRUE)
            {
                gTasks[taskId].tDecorationItemsMenuCommand = DECOR_ITEMS_MENU_PLACE;
                ContinueDecorating(taskId);
            }
            break;
    }
}

void AddDecorationIconObjectFromObjectEvent(struct PlaceDecorationGraphicsDataBuffer *data, u8 decor)
{
    RemoveDecorationItemsWindow();
    RemoveDecorationItemsOtherWindows();
    ClearPlaceDecorationTileBuffers(data);
    data->decoration = &gDecorations[decor];
    if (gDecorations[decor].permission != DECORPERM_SPRITE)
    {
        SetDecorSelectionMetatiles(data);
        SetDecorSelectionBoxOamAttributes(data->decoration->shape);
        SetDecorSelectionBoxTiles(data);
        CopyPalette(data->palette, ((u16 *)gMapHeader.mapLayout->secondaryTileset->metatiles + NUM_TILES_PER_METATILE * data->decoration->tiles[0])[7] >> 12);
        LoadSpritePalette(&sSpritePal_PlaceDecoration);
        sDecor_CameraSpriteObjectIdx1 = gSprites[gFieldCamera.trackedSpriteId].data[0];
        gFieldCamera.trackedSpriteId = CreateSprite(&sDecorationSelectorSpriteTemplate, sDecorationMovementInfo[data->decoration->shape].cameraX, sDecorationMovementInfo[data->decoration->shape].cameraY, 0);
    } else
    {
        sDecor_CameraSpriteObjectIdx1 = gSprites[gFieldCamera.trackedSpriteId].data[0];
        gFieldCamera.trackedSpriteId = CreateObjectGraphicsSprite(data->decoration->tiles[0], InitializeDecorationCursorSprite, sDecorationMovementInfo[data->decoration->shape].cameraX, sDecorationMovementInfo[data->decoration->shape].cameraY, 1);
        gSprites[gFieldCamera.trackedSpriteId].oam.priority = 1;
    }
}

void SetUpPlacingDecorationPlayerAvatar(u8 taskId, struct PlaceDecorationGraphicsDataBuffer *data)
{
    u8 x;

    x = 16 * (u8)gTasks[taskId].tDecorWidth + sDecorationMovementInfo[data->decoration->shape].cameraX - 8 * ((u8)gTasks[taskId].tDecorWidth - 1);
    if (data->decoration->shape == DECORSHAPE_3x1 || data->decoration->shape == DECORSHAPE_3x3 || data->decoration->shape == DECORSHAPE_3x2)
    {
        x -= 8;
    }
    if (gSaveBlock2.playerGender == MALE)
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(OBJ_EVENT_GFX_BRENDAN_DECORATING, SpriteCallbackDummy, x, 0x48, 0);
    } else
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(OBJ_EVENT_GFX_MAY_DECORATING, SpriteCallbackDummy, x, 0x48, 0);
    }
    gSprites[sDecor_CameraSpriteObjectIdx2].oam.priority = 1;
    DestroySprite(&gSprites[sDecor_CameraSpriteObjectIdx1]);
    sDecor_CameraSpriteObjectIdx1 = gFieldCamera.trackedSpriteId;
}

void SetUpDecorationShape(u8 taskId)
{
    switch (gDecorations[gCurDecorationItems[gCurDecorationIndex]].shape)
    {
        case DECORSHAPE_1x1:
            gTasks[taskId].tDecorWidth = 1;
            gTasks[taskId].tDecorHeight = 1;
            break;
        case DECORSHAPE_2x1:
            gTasks[taskId].tDecorWidth = 2;
            gTasks[taskId].tDecorHeight = 1;
            break;
        case DECORSHAPE_3x1:
            gTasks[taskId].tDecorWidth = 3;
            gTasks[taskId].tDecorHeight = 1;
            break;
        case DECORSHAPE_4x2:
            gTasks[taskId].tDecorWidth = 4;
            gTasks[taskId].tDecorHeight = 2;
            break;
        case DECORSHAPE_2x2:
            gTasks[taskId].tDecorWidth = 2;
            gTasks[taskId].tDecorHeight = 2;
            break;
        case DECORSHAPE_1x2:
            gTasks[taskId].tDecorWidth = 1;
            gTasks[taskId].tDecorHeight = 2;
            break;
        case DECORSHAPE_1x3:
            gTasks[taskId].tDecorWidth = 1;
            gTasks[taskId].tDecorHeight = 3;
            gTasks[taskId].tCursorY++;
            break;
        case DECORSHAPE_2x4:
            gTasks[taskId].tDecorWidth = 2;
            gTasks[taskId].tDecorHeight = 4;
            break;
        case DECORSHAPE_3x3:
            gTasks[taskId].tDecorWidth = 3;
            gTasks[taskId].tDecorHeight = 3;
            break;
        case DECORSHAPE_3x2:
            gTasks[taskId].tDecorWidth = 3;
            gTasks[taskId].tDecorHeight = 2;
            break;
    }
}

void AttemptPlaceDecoration(u8 taskId)
{
    gTasks[taskId].tButton = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 1;
    gSprites[sDecor_CameraSpriteObjectIdx2].data[7] = 1;
    ResetCursorMovement();
    AttemptPlaceDecoration_(taskId);
}

void AttemptCancelPlaceDecoration(u8 taskId)
{
    gTasks[taskId].tButton = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 1;
    gSprites[sDecor_CameraSpriteObjectIdx2].data[7] = 1;
    ResetCursorMovement();
    DisplayItemMessageOnField(taskId, gText_CancelDecorating, CancelDecoratingPrompt, 0);
}

bool8 IsBlockDecorationOnNormalLayer(u8 metatileBehavior, u16 layerType)
{
    if (MetatileBehavior_IsBlockDecoration(metatileBehavior) != TRUE || layerType != 0)
    {
        return FALSE;
    }
    return TRUE;
}

bool8 IsntInitialPosition(u8 taskId, s16 x, s16 y, u16 layerType)
{
    if (x == gTasks[taskId].tInitialX + MAP_OFFSET && y == gTasks[taskId].tInitialY + MAP_OFFSET && layerType != METATILE_LAYER_TYPE_NORMAL)
    {
        return FALSE;
    }
    return TRUE;
}

bool8 IsFloorOrBoardAndHole(u16 metatileBehavior, const struct Decoration *decoration)
{
    if (MetatileBehavior_IsBlockDecoration(metatileBehavior) != TRUE)
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

bool8 CanPlaceDecoration(u8 taskId, const struct Decoration *decoration)
{
    u8 i;
    u8 j;
    u8 behaviorAt;
    u16 layerType;
    u8 mapY;
    u8 mapX;
    s16 curY;
    s16 curX;
    mapY = gTasks[taskId].tDecorHeight;
    mapX = gTasks[taskId].tDecorWidth;
    switch (decoration->permission)
    {
        case DECORPERM_SOLID_FLOOR:
        case DECORPERM_PASS_FLOOR:
            for (i=0; i<mapY; i++)
            {
                curY = gTasks[taskId].tCursorY - i;
                for (j=0; j<mapX; j++)
                {
                    curX = gTasks[taskId].tCursorX + j;
                    behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                    layerType = GetMetatileAttributesById(NUM_TILES_IN_PRIMARY + decoration->tiles[(mapY - 1 - i) * mapX + j]) & METATILE_ATTR_LAYER_MASK;
                    if (!IsFloorOrBoardAndHole(behaviorAt, decoration))
                    {
                        return FALSE;
                    }
                    if (!IsntInitialPosition(taskId, curX, curY, layerType))
                    {
                        return FALSE;
                    }
                    behaviorAt = GetObjectEventIdByPosition(curX, curY, ELEVATION_TRANSITION);
                    if (behaviorAt != ELEVATION_TRANSITION && behaviorAt != OBJECT_EVENTS_COUNT)
                    {
                        return FALSE;
                    }
                }
            }
            break;
        case DECORPERM_BEHIND_FLOOR:
            for (i=0; i<mapY-1; i++)
            {
                curY = gTasks[taskId].tCursorY - i;
                for (j=0; j<mapX; j++)
                {
                    curX = gTasks[taskId].tCursorX + j;
                    behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                    layerType = GetMetatileAttributesById(NUM_TILES_IN_PRIMARY + decoration->tiles[(mapY - 1 - i) * mapX + j]) & METATILE_ATTR_LAYER_MASK;
                    if (!MetatileBehavior_IsNormal(behaviorAt) && !IsBlockDecorationOnNormalLayer(behaviorAt, layerType))
                    {
                        return FALSE;
                    }
                    if (!IsntInitialPosition(taskId, curX, curY, layerType))
                    {
                        return FALSE;
                    }
                    if (GetObjectEventIdByPosition(curX, curY, ELEVATION_TRANSITION) != OBJECT_EVENTS_COUNT)
                    {
                        return FALSE;
                    }
                }
            }
            curY = gTasks[taskId].tCursorY - mapY + 1;
            for (j=0; j<mapX; j++)
            {
                curX = gTasks[taskId].tCursorX + j;
                behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                layerType = GetMetatileAttributesById(NUM_TILES_IN_PRIMARY + decoration->tiles[j]) & METATILE_ATTR_LAYER_MASK;
                if (!MetatileBehavior_IsNormal(behaviorAt) && !MetatileBehavior_IsSecretBaseNorthWall(behaviorAt))
                {
                    return FALSE;
                }
                if (!IsntInitialPosition(taskId, curX, curY, layerType))
                {
                    return FALSE;
                }
                behaviorAt = GetObjectEventIdByPosition(curX, curY, ELEVATION_TRANSITION);
                if (behaviorAt != ELEVATION_TRANSITION && behaviorAt != OBJECT_EVENTS_COUNT)
                {
                    return FALSE;
                }
            }
            break;
        case DECORPERM_NA_WALL:
            for (i=0; i<mapY; i++)
            {
                curY = gTasks[taskId].tCursorY - i;
                for (j=0; j<mapX; j++)
                {
                    curX = gTasks[taskId].tCursorX + j;
                    if (!MetatileBehavior_IsSecretBaseNorthWall(MapGridGetMetatileBehaviorAt(curX, curY)))
                    {
                        return FALSE;
                    }
                    if (MapGridGetMetatileIdAt(curX, curY + 1) == METATILE_SecretBase_SandOrnament_BrokenBase)
                    {
                        return FALSE;
                    }
                }
            }
            break;
        case DECORPERM_SPRITE:
            curY = gTasks[taskId].tCursorY;
            for (j=0; j<mapX; j++)
            {
                curX = gTasks[taskId].tCursorX + j;
                behaviorAt = MapGridGetMetatileBehaviorAt(curX, curY);
                if (decoration->shape == DECORSHAPE_1x2)
                {
                    if (!MetatileBehavior_IsLargeMatCenter(behaviorAt))
                    {
                        return FALSE;
                    }
                }
                else if (!MetatileBehavior_IsSecretBaseLargeMatEdge(behaviorAt))
                {
                    if (!MetatileBehavior_IsLargeMatCenter(behaviorAt))
                    {
                        return FALSE;
                    }
                }
                if (GetObjectEventIdByPosition(curX, curY, ELEVATION_TRANSITION) != OBJECT_EVENTS_COUNT)
                {
                    return FALSE;
                }
            }
            break;
    }
    return TRUE;
}

void AttemptPlaceDecoration_(u8 taskId)
{
    if (CanPlaceDecoration(taskId, &gDecorations[gCurDecorationItems[gCurDecorationIndex]]) == TRUE)
    {
        DisplayItemMessageOnField(taskId, gText_PlaceItHere, PlaceDecorationPrompt, 0);
    } else
    {
        PlaySE(SE_FAILURE);
        DisplayItemMessageOnField(taskId, gText_CantBePlacedHere, CantPlaceDecorationPrompt, 0);
    }
}

void PlaceDecorationPrompt(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sPlaceDecorationYesNoFunctions);
}

void PlaceDecoration(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    PlaceDecoration_(taskId);
    if (gDecorations[gCurDecorationItems[gCurDecorationIndex]].permission != DECORPERM_SPRITE)
    {
        ShowDecorationOnMap(gTasks[taskId].tCursorX, gTasks[taskId].tCursorY, gCurDecorationItems[gCurDecorationIndex]);
    } else
    {
        sCurDecorMapX = gTasks[taskId].tCursorX - MAP_OFFSET;
        sCurDecorMapY = gTasks[taskId].tCursorY - MAP_OFFSET;
        ScriptContext_SetupScript(SecretBase_EventScript_SetDecoration);
    }
    gSprites[sDecor_CameraSpriteObjectIdx1].y += 2;
    CancelDecorating_(taskId);
}

void PlaceDecoration_(u8 taskId)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] == DECOR_NONE)
        {
            sDecorationContext.items[i] = gCurDecorationItems[gCurDecorationIndex];
            sDecorationContext.pos[i] = ((gTasks[taskId].tCursorX - MAP_OFFSET) << 4) + (gTasks[taskId].tCursorY - MAP_OFFSET);
            break;
        }
    }
    if (!sDecorationContext.isPlayerRoom)
    {
        for (i = 0; i < ARRAY_COUNT(sSecretBaseItemsIndicesBuffer); i++)
        {
            if (sSecretBaseItemsIndicesBuffer[i] == 0)
            {
                sSecretBaseItemsIndicesBuffer[i] = gCurDecorationIndex + 1;
                break;
            }
        }
    } else
    {
        for (i = 0; i < ARRAY_COUNT(sPlayerRoomItemsIndicesBuffer); i++)
        {
            if (sPlayerRoomItemsIndicesBuffer[i] == 0)
            {
                sPlayerRoomItemsIndicesBuffer[i] = gCurDecorationIndex + 1;
                break;
            }
        }
    }
}

void CancelDecoratingPrompt(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sCancelDecoratingYesNoFunctions);
}

void CancelDecorating(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    CancelDecorating_(taskId);
}

void CancelDecorating_(u8 taskId)
{
    FadeScreen(FADE_TO_BLACK, 0);
    gTasks[taskId].tState = 0;
    gTasks[taskId].func = c1_overworld_prev_quest;
}

void c1_overworld_prev_quest(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
        case 0:
            LockPlayerFieldControls();
            if (!gPaletteFade.active)
            {
                WarpToInitialPosition(taskId);
                gTasks[taskId].tState = 1;
            }
            break;
        case 1:
            FreePlayerSpritePalette();
            FreeSpritePaletteByTag(PLACE_DECORATION_SELECTOR_TAG);
            gFieldCallback = &FieldCB_InitDecorationItemsWindow;
            SetMainCallback2(CB2_ReturnToField);
            DestroyTask(taskId);
            break;
    }
}

void Task_WaitForDecorationWeather(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        gTasks[taskId].func = HandleDecorationItemsMenuInput;
    }
}

void FieldCB_InitDecorationItemsWindow(void)
{
    LockPlayerFieldControls();
    LoadScrollIndicatorPalette();
    FadeInFromBlack();
    InitDecorationItemsWindow(CreateTask(Task_WaitForDecorationWeather, 8));
}

bool8 ApplyCursorMovement_IsInvalid(u8 taskId)
{
    s16 *data;

    data = gTasks[taskId].data;
    if (sDecorationLastDirectionMoved == DIR_SOUTH && tCursorY - tDecorHeight - 6 < 0)
    {
        tCursorY++;
        return FALSE;
    } else if (sDecorationLastDirectionMoved == DIR_NORTH && tCursorY - MAP_OFFSET >= gMapHeader.mapLayout->height)
    {
        tCursorY--;
        return FALSE;
    } else if (sDecorationLastDirectionMoved == DIR_WEST && tCursorX - MAP_OFFSET < 0)
    {
        tCursorX++;
        return FALSE;
    } else if (sDecorationLastDirectionMoved == DIR_EAST && tCursorX + tDecorWidth - (MAP_OFFSET + 1) >= gMapHeader.mapLayout->width)
    {
        tCursorX--;
        return FALSE;
    }
    return TRUE;
}

bool8 IsHoldingDirection(void)
{
    if (JOY_HELD(DPAD_ANY) != DPAD_UP && JOY_HELD(DPAD_ANY) != DPAD_DOWN && JOY_HELD(DPAD_ANY) != DPAD_LEFT && JOY_HELD(DPAD_ANY) != DPAD_RIGHT)
    {
        return FALSE;
    }
    return TRUE;
}

void ResetCursorMovement(void)
{
    sDecorationLastDirectionMoved = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[2] = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].data[3] = 0;
}

void Task_SelectLocation(u8 taskId)
{
    if (!gSprites[sDecor_CameraSpriteObjectIdx1].data[4])
    {
        if (gTasks[taskId].tButton == A_BUTTON)
        {
            sPlacePutAwayYesNoFunctions[gTasks[taskId].tDecorationItemsMenuCommand].yesFunc(taskId);
            return;
        } else if (gTasks[taskId].tButton == B_BUTTON)
        {
            sPlacePutAwayYesNoFunctions[gTasks[taskId].tDecorationItemsMenuCommand].noFunc(taskId);
            return;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_UP)
        {
            sDecorationLastDirectionMoved = DIR_SOUTH;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] =  0;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] = -2;
            gTasks[taskId].tCursorY--;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_DOWN)
        {
            sDecorationLastDirectionMoved = DIR_NORTH;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] =  0;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] =  2;
            gTasks[taskId].tCursorY++;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_LEFT)
        {
            sDecorationLastDirectionMoved = DIR_WEST;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] = -2;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] =  0;
            gTasks[taskId].tCursorX--;
        }
        if (JOY_HELD(DPAD_ANY) == DPAD_RIGHT)
        {
            sDecorationLastDirectionMoved = DIR_EAST;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[2] =  2;
            gSprites[sDecor_CameraSpriteObjectIdx1].data[3] =  0;
            gTasks[taskId].tCursorX++;
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
    if (!gTasks[taskId].tButton)
    {
        if (JOY_NEW(A_BUTTON))
        {
            gTasks[taskId].tButton = A_BUTTON;
        }
        if (JOY_NEW(B_BUTTON))
        {
            gTasks[taskId].tButton = B_BUTTON;
        }
    }
}

void ContinueDecorating(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 0;
    gTasks[taskId].tButton = 0;
    gTasks[taskId].func = Task_SelectLocation;
}

void CantPlaceDecorationPrompt(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        ContinueDecorating(taskId);
    }
}

void ClearPlaceDecorationTileBuffers(struct PlaceDecorationGraphicsDataBuffer *data)
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

void CopyPalette(u16 *dest, u16 paletteNum)
{
    u16 i;
    for (i=0; i<16; i++)
    {
        dest[i] = ((u16 *)gMapHeader.mapLayout->primaryTileset->palettes)[16 * paletteNum + i];
    }
}

void CopyTile(u8 *dest, u16 tile)
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

void SetDecorSelectionBoxTiles(struct PlaceDecorationGraphicsDataBuffer *data)
{
    u16 i;
    for (i=0; i<0x40; i++)
        CopyTile(&data->image[i * 32], data->tiles[i]);
}

u16 GetMetatile(u16 tile)
{
    return ((u16 *)gMapHeader.mapLayout->secondaryTileset->metatiles)[tile] & 0xfff;
}

#include "data/decoration/tilemaps.h"

static const struct DecorationMovementInfo sDecorationMovementInfo[] = {
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
    .paletteTag = PLACE_DECORATION_SELECTOR_TAG,
    .oam = &sDecorSelectorOam,
    .anims = sDecorSelectorAnimCmds,
    .images = sDecorSelectorSpriteFrameImages,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = InitializeDecorationCursorSprite
};

static const struct SpritePalette sSpritePal_PlaceDecoration = {.data = (u16 *)&sPlaceDecorationGraphicsDataBuffer.palette, .tag = PLACE_DECORATION_SELECTOR_TAG};

static const struct YesNoFuncTable sPlaceDecorationYesNoFunctions = {.yesFunc = PlaceDecoration, .noFunc = ContinueDecorating};
static const struct YesNoFuncTable sCancelDecoratingYesNoFunctions = {.yesFunc = CancelDecorating, .noFunc = ContinueDecorating};
static const struct YesNoFuncTable sPlacePutAwayYesNoFunctions[] = {
    {.yesFunc = AttemptPlaceDecoration, .noFunc = AttemptCancelPlaceDecoration},
    {.yesFunc = AttemptPutAwayDecoration, .noFunc = AttemptCancelPutAwayDecoration}
};

static const u8 sDecorationStandElevations[] = {4, 4, 4, 4, 0, 3, 3, 0};
static const u8 sDecorationSlideElevation[] = {4, 4, 4, 4, 0, 4, 3, 0};

static const u16 sBrendanPalette[] = INCBIN_U16("graphics/decorations/brendan.gbapal");
static const u16 sMayPalette[] = INCBIN_U16("graphics/decorations/may.gbapal");
static const struct YesNoFuncTable sReturnDecorationYesNoFunctions = {.yesFunc = PutAwayDecoration, .noFunc = ContinuePuttingAwayDecorations};
static const struct YesNoFuncTable sStopPuttingAwayDecorationsYesNoFunctions = {.yesFunc = StopPuttingAwayDecorations, .noFunc = ContinuePuttingAwayDecorations};
static const u32 sDecorationPuttingAwayCursor[] = INCBIN_U32("graphics/decorations/put_away_cursor.4bpp");
static const struct SpritePalette sSpritePal_PuttingAwayCursorBrendan = {.data = sBrendanPalette, .tag = PLACE_DECORATION_PLAYER_TAG};
static const struct SpritePalette sSpritePal_PuttingAwayCursorMay = {.data = sMayPalette, .tag = PLACE_DECORATION_PLAYER_TAG};
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
    .paletteTag = PLACE_DECORATION_PLAYER_TAG,
    .oam = &sPuttingAwayCursorOamData,
    .anims = sPuttingAwayCursorAnimCmds,
    .images = sPuttingAwayCursorPicTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = InitializeCameraSprite1
};

static const struct YesNoFuncTable sTossDecorationYesNoFunctions = {.yesFunc = TossDecoration, .noFunc = ReturnToDecorationItemsMenu};

void SetDecorSelectionMetatiles(struct PlaceDecorationGraphicsDataBuffer *data)
{
    u8 i;
    u8 shape;
    shape = data->decoration->shape;
    for (i=0; i<sDecorTilemaps[shape].size; i++)
    {
        data->tiles[sDecorTilemaps[shape].tiles[i]] = GetMetatile(data->decoration->tiles[sDecorTilemaps[shape].y[i]] * NUM_TILES_PER_METATILE + sDecorTilemaps[shape].x[i]);
    }
}

void SetDecorSelectionBoxOamAttributes(u8 decorShape)
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

void InitializeDecorationCursorSprite(struct Sprite *sprite)
{
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    sprite->data[5] = 0;
    sprite->data[6] = 0;
    sprite->data[7] = 0;
    sprite->callback = UpdateDecorationCursorBlink;
}

void UpdateDecorationCursorBlink(struct Sprite *sprite)
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

void DecorationMenuAction_PutAway(u8 taskId)
{
    if (HasDecorationsInUse(taskId) == TRUE)
    {
        FadeScreen(FADE_TO_BLACK, 0);
        gTasks[taskId].tState = 0;
        gTasks[taskId].func = Task_ContinuePuttingAwayDecorations;
    } else
    {
        DisplayItemMessageOnField(taskId, gText_NoDecorationsInUse, ReturnToDecorationActionsAfterInvalidSelection, 0);
    }
}

void ClearDecorationContextIndex(u8 idx)
{
    sDecorationContext.items[idx] = DECOR_NONE;
    sDecorationContext.pos[idx] = 0;
}

void PutAwayDecorationIteration(void)
{
    u16 i;
    gSpecialVar_0x8005 = 0;
    gSpecialVar_Result = FALSE;
    if (gSpecialVar_0x8004 == sCurDecorSelectedInRearrangement)
    {
        gSpecialVar_Result = TRUE;
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

void ClearRearrangementNonSprites(void)
{
    u8 i;
    u8 j;
    u8 k;
    u8 x;
    u8 y;
    u8 permission;
    for (i=0; i<sCurDecorSelectedInRearrangement; i++)
    {
        permission = gDecorations[sDecorationContext.items[sDecorRearrangementDataBuffer[i].idx]].permission;
        x = sDecorationContext.pos[sDecorRearrangementDataBuffer[i].idx] >> 4;
        y = sDecorationContext.pos[sDecorRearrangementDataBuffer[i].idx] & 0xf;
        if (permission != DECORPERM_SPRITE)
        {
            if (sDecorationContext.items[sDecorRearrangementDataBuffer[i].idx] == DECOR_SAND_ORNAMENT && MapGridGetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET) == METATILE_SecretBase_SandOrnament_BrokenBase)
            {
                sDecorRearrangementDataBuffer[i].height++;
            }
            for (j=0; j<sDecorRearrangementDataBuffer[i].height; j++)
            {
                for (k=0; k<sDecorRearrangementDataBuffer[i].width; k++)
                {
                    MapGridSetMetatileEntryAt(x + MAP_OFFSET + k, y + MAP_OFFSET - j, ((u16 *)gMapHeader.mapLayout->map)[(x + k) + gMapHeader.mapLayout->width * (y - j)] | (ELEVATION_DEFAULT << MAPGRID_ELEVATION_SHIFT));
                }
            }
            ClearDecorationContextIndex(sDecorRearrangementDataBuffer[i].idx);
        }
    }
}

void Task_PutAwayDecoration(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
        case 0:
            ClearRearrangementNonSprites();
            gTasks[taskId].tState = 1;
            break;
        case 1:
            if (!gPaletteFade.active)
            {
                DrawWholeMapView();
                ScriptContext_SetupScript(SecretBase_EventScript_PutAwayDecoration);
                Menu_EraseWindowRect(0, 0, 29, 19);
                gTasks[taskId].tState = 2;
            }
            break;
        case 2:
            LockPlayerFieldControls();
            IdentifyOwnedDecorationsCurrentlyInUse(taskId);
            FadeInFromBlack();
            gTasks[taskId].tState = 3;
            break;
        case 3:
            if (IsWeatherNotFadingIn() == TRUE)
            {
                gTasks[taskId].tDecorationContextIndex = -1;
                DisplayItemMessageOnField(taskId, gText_DecorationReturnedToPC, ContinuePuttingAwayDecorationsPrompt, 0);
            }
            break;
    }
}

bool8 HasDecorationsInUse(u8 taskId)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] != DECOR_NONE)
        {
            gTasks[taskId].tDecorationContextIndex = i;
            return TRUE;
        }
    }
    return FALSE;
}

void SetUpPuttingAwayDecorationPlayerAvatar(void)
{
    GetPlayerFacingDirection();
    Menu_EraseWindowRect(0, 0, 29, 19);
    sDecor_CameraSpriteObjectIdx1 = gSprites[gFieldCamera.trackedSpriteId].data[0];
    LoadPlayerSpritePalette();
    gFieldCamera.trackedSpriteId = CreateSprite(&sPuttingAwayCursorSpriteTemplate, 0x78, 0x50, 0);
    if (gSaveBlock2.playerGender == MALE)
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(OBJ_EVENT_GFX_BRENDAN_DECORATING, SpriteCallbackDummy, 0x88, 0x48, 0);
    } else
    {
        sDecor_CameraSpriteObjectIdx2 = CreateObjectGraphicsSprite(OBJ_EVENT_GFX_MAY_DECORATING, SpriteCallbackDummy, 0x88, 0x48, 0);
    }
    gSprites[sDecor_CameraSpriteObjectIdx2].oam.priority = 1;
    DestroySprite(&gSprites[sDecor_CameraSpriteObjectIdx1]);
    sDecor_CameraSpriteObjectIdx1 = gFieldCamera.trackedSpriteId;
    gSprites[sDecor_CameraSpriteObjectIdx1].oam.priority = 1;
}

void Task_ContinuePuttingAwayDecorations(u8 taskId)
{
    s16 *data;
    data = gTasks[taskId].data;
    switch (tState)
    {
        case 0:
            if (!gPaletteFade.active)
            {
                SetInitialPositions(taskId);
                tState = 1;
                tDecorHeight = 1;
                tDecorWidth = 1;
                Menu_DestroyCursor();
            }
            break;
        case 1:
            SetUpPuttingAwayDecorationPlayerAvatar();
            FadeInFromBlack();
            tState = 2;
            break;
        case 2:
            if (IsWeatherNotFadingIn() == TRUE)
            {
                tDecorationItemsMenuCommand = DECOR_ITEMS_MENU_PUT_AWAY;
                ContinuePuttingAwayDecorations(taskId);
            }
            break;
    }
}

void ContinuePuttingAwayDecorations(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    gSprites[sDecor_CameraSpriteObjectIdx1].data[7] = 0;
    gSprites[sDecor_CameraSpriteObjectIdx1].invisible = FALSE;
    gSprites[sDecor_CameraSpriteObjectIdx1].callback = InitializeCameraSprite1;
    gSprites[sDecor_CameraSpriteObjectIdx2].x = 0x88;
    gSprites[sDecor_CameraSpriteObjectIdx2].y = 0x48;
    gTasks[taskId].tButton = 0;
    gTasks[taskId].func = Task_SelectLocation;
}

void AttemptPutAwayDecoration(u8 taskId)
{
    gTasks[taskId].tButton = 0;
    ResetCursorMovement();
    AttemptPutAwayDecoration_(taskId);
}

void AttemptCancelPutAwayDecoration(u8 taskId)
{
    gTasks[taskId].tButton = 0;
    ResetCursorMovement();
    gSprites[sDecor_CameraSpriteObjectIdx1].invisible = FALSE;
    gSprites[sDecor_CameraSpriteObjectIdx1].callback = SpriteCallbackDummy;
    DisplayItemMessageOnField(taskId, gText_StopPuttingAwayDecorations, StopPuttingAwayDecorationsPrompt, 0);
}

void AttemptPutAwayDecoration_(u8 taskId)
{
    u8 metatileBehavior;
    s16 *data;
    AttemptMarkDecorUnderCursorForRemoval(taskId);
    if (sCurDecorSelectedInRearrangement != 0)
    {
        DisplayItemMessageOnField(taskId, gText_ReturnDecorationToPC, ReturnDecorationPrompt, 0);
    } else
    {
        data = gTasks[taskId].data;
        metatileBehavior = MapGridGetMetatileBehaviorAt(tCursorX, tCursorY);
        if (MetatileBehavior_IsSecretBasePC(metatileBehavior) == TRUE
         || MetatileBehavior_IsPlayerRoomPCOn(metatileBehavior) == TRUE)
        {
            gSprites[sDecor_CameraSpriteObjectIdx1].invisible = FALSE;
            gSprites[sDecor_CameraSpriteObjectIdx1].callback = SpriteCallbackDummy;
            DisplayItemMessageOnField(taskId, gText_StopPuttingAwayDecorations, StopPuttingAwayDecorationsPrompt, 0);
        } else
        {
            DisplayItemMessageOnField(taskId, gText_NoDecorationHere, ContinuePuttingAwayDecorationsPrompt, 0);
        }
    }
}

void ContinuePuttingAwayDecorationsPrompt(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        ContinuePuttingAwayDecorations(taskId);
    }
}

void SetDecorRearrangementShape(u8 decor, struct DecorRearrangementDataBuffer *data)
{
    if (gDecorations[decor].shape == DECORSHAPE_1x1)
    {
        data->width = 1;
        data->height = 1;
    } else if (gDecorations[decor].shape == DECORSHAPE_2x1)
    {
        data->width = 2;
        data->height = 1;
    } else if (gDecorations[decor].shape == DECORSHAPE_3x1)
    {
        data->width = 3;
        data->height = 1;
    } else if (gDecorations[decor].shape == DECORSHAPE_4x2)
    {
        data->width = 4;
        data->height = 2;
    } else if (gDecorations[decor].shape == DECORSHAPE_2x2)
    {
        data->width = 2;
        data->height = 2;
    } else if (gDecorations[decor].shape == DECORSHAPE_1x2)
    {
        data->width = 1;
        data->height = 2;
    } else if (gDecorations[decor].shape == DECORSHAPE_1x3)
    {
        data->width = 1;
        data->height = 3;
    } else if (gDecorations[decor].shape == DECORSHAPE_2x4)
    {
        data->width = 2;
        data->height = 4;
    } else if (gDecorations[decor].shape == DECORSHAPE_3x3)
    {
        data->width = 3;
        data->height = 3;
    } else if (gDecorations[decor].shape == DECORSHAPE_3x2)
    {
        data->width = 3;
        data->height = 2;
    }
}

void SetCameraSpritePosition(u8 x, u8 y)
{
    gSprites[sDecor_CameraSpriteObjectIdx1].invisible = TRUE;
    gSprites[sDecor_CameraSpriteObjectIdx1].callback = SpriteCallbackDummy;
    gSprites[sDecor_CameraSpriteObjectIdx2].x = 0x88 + x * 16;
    gSprites[sDecor_CameraSpriteObjectIdx2].y = 0x48 + y * 16;
}

bool8 DecorationIsUnderCursor(u8 taskId, u8 idx, struct DecorRearrangementDataBuffer *data)
{
    u8 x;
    u8 y;
    u8 xOff;
    u8 yOff;
    x = gTasks[taskId].tCursorX - MAP_OFFSET;
    y = gTasks[taskId].tCursorY - MAP_OFFSET;
    xOff = sDecorationContext.pos[idx] >> 4;
    yOff = sDecorationContext.pos[idx] & 0xf;
    if (sDecorationContext.items[idx] == DECOR_SAND_ORNAMENT && MapGridGetMetatileIdAt(xOff + MAP_OFFSET, yOff + MAP_OFFSET) == METATILE_SecretBase_SandOrnament_BrokenBase)
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

void SetDecorRearrangementFlagIdIfFlagUnset(void)
{
    u8 xOff;
    u8 yOff;
    u16 i;
    xOff = sDecorationContext.pos[sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].idx] >> 4;
    yOff = sDecorationContext.pos[sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].idx] & 0xf;
    for (i = 0; i < ARRAY_COUNT(gSaveBlock1.objectEventTemplates); i++)
    {
        if (gSaveBlock1.objectEventTemplates[i].x == xOff && gSaveBlock1.objectEventTemplates[i].y == yOff && !FlagGet(gSaveBlock1.objectEventTemplates[i].flagId))
        {
            sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].flagId = gSaveBlock1.objectEventTemplates[i].flagId;
            break;
        }
    }
}

bool8 AttemptMarkSpriteDecorUnderCursorForRemoval(u8 taskId)
{
    u16 i;
    for (i=0; i<sDecorationContext.size; i++)
    {
        if (sDecorationContext.items[i] != DECOR_NONE)
        {
            if (gDecorations[sDecorationContext.items[i]].permission == DECORPERM_SPRITE)
            {
                SetDecorRearrangementShape(sDecorationContext.items[i], sDecorRearrangementDataBuffer);
                if (DecorationIsUnderCursor(taskId, i, sDecorRearrangementDataBuffer) == TRUE)
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

void MarkSpriteDecorsInBoundsForRemoval(u8 left, u8 top, u8 right, u8 bottom)
{
    u8 i;
    u8 xOff;
    u8 yOff;
    u8 decor;
    for (i=0; i<sDecorationContext.size; i++)
    {
        decor = sDecorationContext.items[i];
        xOff = sDecorationContext.pos[i] >> 4;
        yOff = sDecorationContext.pos[i] & 0xf;
        if (decor != DECOR_NONE && gDecorations[decor].permission == DECORPERM_SPRITE && left <= xOff && top <= yOff && right >= xOff && bottom >= yOff)
        {
            sDecorRearrangementDataBuffer[sCurDecorSelectedInRearrangement].idx = i;
            SetDecorRearrangementFlagIdIfFlagUnset();
            sCurDecorSelectedInRearrangement++;
        }
    }
}

void AttemptMarkDecorUnderCursorForRemoval(u8 taskId)
{
    u8 i;
    u8 xOff;
    u8 yOff;
    u8 decorOrTop;
    u32 right;

    sCurDecorSelectedInRearrangement = 0;
    if (AttemptMarkSpriteDecorUnderCursorForRemoval(taskId) != TRUE)
    {
        for (i = 0; i < sDecorationContext.size; i++)
        {
            decorOrTop = sDecorationContext.items[i];
            if (decorOrTop != DECOR_NONE)
            {
                SetDecorRearrangementShape(decorOrTop, sDecorRearrangementDataBuffer);
                if (DecorationIsUnderCursor(taskId, i, sDecorRearrangementDataBuffer) == TRUE)
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
            decorOrTop = yOff - sDecorRearrangementDataBuffer[0].height + 1;
            right = sDecorRearrangementDataBuffer[0].width + xOff - 1;

            MarkSpriteDecorsInBoundsForRemoval(xOff, decorOrTop, right, yOff);
        }
    }
}

void ReturnDecorationPrompt(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sReturnDecorationYesNoFunctions);
}

void PutAwayDecoration(u8 taskId)
{
    FadeScreen(FADE_TO_BLACK, 0);
    gTasks[taskId].tState = 0;
    gTasks[taskId].func = Task_PutAwayDecoration;
}

void StopPuttingAwayDecorationsPrompt(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sStopPuttingAwayDecorationsYesNoFunctions);
}

void StopPuttingAwayDecorations(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
    StopPuttingAwayDecorations_(taskId);
}

void StopPuttingAwayDecorations_(u8 taskId)
{
    FadeScreen(FADE_TO_BLACK, 0);
    gTasks[taskId].tState = 0;
    gTasks[taskId].func = Task_StopPuttingAwayDecorations;
}

void Task_StopPuttingAwayDecorations(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
        case 0:
            if (!gPaletteFade.active)
            {
                WarpToInitialPosition(taskId);
                gTasks[taskId].tState = 1;
            }
            break;
        case 1:
            FreePlayerSpritePalette();
            gFieldCallback = FieldCB_StopPuttingAwayDecorations;
            SetMainCallback2(CB2_ReturnToField);
            DestroyTask(taskId);
            break;
    }
}

void Task_ReinitializeDecorationMenuHandler(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        gTasks[taskId].func = HandleDecorationActionsMenuInput;
    }
}

void FieldCB_StopPuttingAwayDecorations(void)
{
    FadeInFromBlack();
    Menu_DisplayDialogueFrame();
    InitDecorationActionsWindow();
    CreateTask(Task_ReinitializeDecorationMenuHandler, 8);
}

void InitializeCameraSprite1(struct Sprite *sprite)
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

void LoadPlayerSpritePalette(void)
{
    if (gSaveBlock2.playerGender == MALE)
    {
        LoadSpritePalette(&sSpritePal_PuttingAwayCursorBrendan);
    } else
    {
        LoadSpritePalette(&sSpritePal_PuttingAwayCursorMay);
    }
}

void FreePlayerSpritePalette(void)
{
    FreeSpritePaletteByTag(PLACE_DECORATION_PLAYER_TAG);
}

void DecorationMenuAction_Toss(u8 taskId)
{
    if (!GetNumOwnedDecorations())
    {
        DisplayItemMessageOnField(taskId, gText_NoDecorations, ReturnToDecorationActionsAfterInvalidSelection, 0);
    } else
    {
        gTasks[taskId].tDecorationMenuCommand = DECOR_MENU_TOSS;
        sCurDecorationCategory = DECORCAT_DESK;
        InitDecorationCategoriesWindow(taskId);
    }
}

void OpenDecorationTradeCategories(u8 taskId)
{
    if (!GetNumOwnedDecorations())
    {
        DisplayItemMessageOnField(taskId, gText_NoDecorations, ReturnToDecorationActionsAfterInvalidSelection, 0);
    } else
    {
        gTasks[taskId].tDecorationMenuCommand = DECOR_MENU_SELL;
        sCurDecorationCategory = DECORCAT_DESK;
        InitDecorationCategoriesWindow(taskId);
    }
}

void DecorationItemsMenuAction_AttemptToss(u8 taskId)
{
    RemoveDecorationItemsWindow();
    RemoveDecorationItemsOtherWindows();
    if (IsSelectedDecorInThePC() == TRUE)
    {
        StringCopy(gStringVar1, gDecorations[gCurDecorationItems[gCurDecorationIndex]].name);
        StringExpandPlaceholders(gStringVar4, gText_DecorationWillBeDiscarded);
        DisplayItemMessageOnField(taskId, gStringVar4, TossDecorationPrompt, 0);
    } else
    {
        DisplayItemMessageOnField(taskId, gText_CantThrowAwayInUse, ReturnToDecorationItemsAfterInvalidSelection, 0);
    }
}

void TossDecorationPrompt(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sTossDecorationYesNoFunctions);
}

void TossDecoration(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 14);
    SetRecycledDecoration(gCurDecorationItems[gCurDecorationIndex]);
    gCurDecorationItems[gCurDecorationIndex] = DECOR_NONE;
    RefreshDecorationItemsAfterToss(taskId);
    DisplayItemMessageOnField(taskId, gText_DecorationThrownAway, ReturnToDecorationItemsAfterInvalidSelection, 0);
}
