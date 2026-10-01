#ifndef GUARD_POKEMON_STORAGE_SYSTEM_H
#define GUARD_POKEMON_STORAGE_SYSTEM_H

#include "pc_screen_effect.h"
#include "mon_markings.h"

#define MAX_MON_ICONS max(IN_BOX_COUNT + PARTY_SIZE + 1, 40)

enum {
    PSS_OPTION_WITHDRAW,
    PSS_OPTION_DEPOSIT,
    PSS_OPTION_MOVE_MONS,
};

enum {
    PSS_MENU_CANCEL,
    PSS_MENU_STORE,
    PSS_MENU_WITHDRAW,
    PSS_MENU_MOVE,
    PSS_MENU_SHIFT,
    PSS_MENU_PLACE,
    PSS_MENU_SUMMARY,
    PSS_MENU_RELEASE,
    PSS_MENU_MARK,
    PSS_MENU_JUMP,
    PSS_MENU_WALLPAPER,
    PSS_MENU_NAME,
    PSS_MENU_SCENERY_1,
    PSS_MENU_SCENERY_2,
    PSS_MENU_SCENERY_3,
    PSS_MENU_ETCETERA,
    PSS_MENU_FOREST,
    PSS_MENU_CITY,
    PSS_MENU_DESERT,
    PSS_MENU_SAVANNA,
    PSS_MENU_CRAG,
    PSS_MENU_VOLCANO,
    PSS_MENU_SNOW,
    PSS_MENU_CAVE,
    PSS_MENU_BEACH,
    PSS_MENU_SEAFLOOR,
    PSS_MENU_RIVER,
    PSS_MENU_SKY,
    PSS_MENU_POLKADOT,
    PSS_MENU_POKECENTER,
    PSS_MENU_MACHINE,
    PSS_MENU_PLAIN,
    PSS_MENU_DEBUG_SET,
};

enum {
    PSS_INPUT_NONE,
    PSS_INPUT_MOVE_CURSOR,
    PSS_INPUT_UNUSED_2,
    PSS_INPUT_UNUSED_3,
    PSS_INPUT_CLOSE_BOX,
    PSS_INPUT_SHOW_PARTY,
    PSS_INPUT_HIDE_PARTY,
    PSS_INPUT_BOX_OPTIONS,
    PSS_INPUT_IN_MENU,
    PSS_INPUT_SCROLL_RIGHT,
    PSS_INPUT_SCROLL_LEFT,
    PSS_INPUT_DEPOSIT,
    PSS_INPUT_WITHDRAW,
    PSS_INPUT_MOVE_MON,
    PSS_INPUT_SHIFT_MON,
    PSS_INPUT_PLACE_MON,
    PSS_INPUT_PRESSED_B,
};

enum {
    PSS_CURSOR_AREA_IN_BOX,
    PSS_CURSOR_AREA_IN_PARTY,
    PSS_CURSOR_AREA_BOX_TITLE,
    PSS_CURSOR_AREA_BUTTONS,
};

enum {
    PSS_CURSOR_ANIM_BOUNCE,
    PSS_CURSOR_ANIM_STILL,
    PSS_CURSOR_ANIM_OPEN,
    PSS_CURSOR_ANIM_FIST,
};

enum {
    PSS_MODE_PARTY,
    PSS_MODE_BOX,
    PSS_MODE_MOVE,
};

enum {
    PSS_PLACE_CHANGE_GRAB,
    PSS_PLACE_CHANGE_PLACE,
    PSS_PLACE_CHANGE_SHIFT,
};

enum {
    PC_TEXT_EXIT_BOX,
    PC_TEXT_WHAT_YOU_DO,
    PC_TEXT_PICK_A_THEME,
    PC_TEXT_PICK_A_WALLPAPER,
    PC_TEXT_IS_SELECTED,
    PC_TEXT_JUMP_TO_WHICH_BOX,
    PC_TEXT_DEPOSIT_IN_WHICH_BOX,
    PC_TEXT_WAS_DEPOSITED,
    PC_TEXT_BOX_IS_FULL,
    PC_TEXT_RELEASE_POKE,
    PC_TEXT_WAS_RELEASED,
    PC_TEXT_BYE_BYE,
    PC_TEXT_MARK_POKE,
    PC_TEXT_LAST_POKE,
    PC_TEXT_PARTY_FULL,
    PC_TEXT_HOLDING_POKE,
    PC_TEXT_WHICH_ONE_WILL_TAKE,
    PC_TEXT_CANT_RELEASE_EGG,
    PC_TEXT_CONTINUE_BOX,
    PC_TEXT_CAME_BACK,
    PC_TEXT_WORRIED,
    PC_TEXT_SURPRISE,
    PC_TEXT_PLEASE_REMOVE_MAIL
};

#define BOXID_NONE_CHOSEN 200
#define BOXID_CANCELED     201

enum {
    PC_TEXT_FMT_NORMAL,
    PC_TEXT_FMT_MON_NAME,
    PC_TEXT_FMT_UNK_02,
    PC_TEXT_FMT_UNK_03,
    PC_TEXT_FMT_MON_NAME_2,
    PC_TEXT_FMT_UNK_05,
    PC_TEXT_FMT_MON_NAME_AFTER_EXCL_MARK
};

struct PokeStorageInputHandler
{
    u8 (*func)(void);
    s8 cursorArea;
};

struct StorageMessage {
    const u8 *text;
    u8 format;
};

struct PokeStorageMenuItem {
    const u8 *text;
    int menuItemId;
};

struct StorageMenuText {
    const u8 *text;
    const u8 *desc;
};

// Ruby schedules active storage tilemap transfers for VBlank. Emerald retains a
// related but unused utility under generic UnkUtil names.
struct TilemapTransfer {
    const u8 *source;
    u8 *destination;
    u16 rowSize;
    u16 height;
    void (*execute)(struct TilemapTransfer *transfer);
};

struct TilemapTransferQueue {
    struct TilemapTransfer *transfers;
    u8 transferCount;
    u8 capacity;
};

struct ChooseBoxMenu {
    struct Sprite *menuSprite;
    struct Sprite *menuSideSprites[4];
    u32 unused1[3];
    struct Sprite *arrowSprites[2];
    u8 unused2[0x214];
    u8 curBox;
    u8 unused3;
    u8 subpriority;
    u16 tileTag;
    u16 paletteTag;
}; // 0244

struct PokemonStorageSystemData {
    void (*callback)(void);
    u8 state;
    u8 boxOption;
    u8 screenChangeType;
    u8 scrollBackgroundTimer;
    u16 bg3_Y;
    u16 bg3_X;
    struct PCScreenEffectStruct pcScreenEffect;
    struct TilemapTransferQueue tilemapTransferQueue;
    struct TilemapTransfer tilemapTransfers[8];
    u8 partyMenuTilemapBuffer[0x800];
    u16 partyMenuUnused1;
    u16 partyMenuY;
    u8 partyMenuUnused2; // Unused.
    u8 partyMenuMoveTimer;
    u8 showPartyMenuState;
    u8 closeBoxFlashing;
    u8 closeBoxFlashTimer;
    bool8 closeBoxFlashState;
    s16 newCurrBoxId;
    u16 bg2_X;
    u16 scrollSpeed;
    u16 scrollTimer;
    bool8 wallpaperOffset;
    u8 scrollToBoxIdUnused;
    u16 scrollUnused2;
    u16 scrollDirectionUnused;
    u16 scrollUnused3;
    u16 scrollUnused4;
    u16 scrollUnused5;
    u16 scrollUnused6;
    u16 scrollUnused7;
    u8 boxTitleTiles[0x400];
    u8 boxTitleCycleId;
    u16 boxTitlePal[0x10];
    u16 boxTitlePalOffset;
    u16 boxTitleAltPalOffset;
    struct Sprite *curBoxTitleSprites[2];
    struct Sprite *nextBoxTitleSprites[2];
    struct Sprite *arrowSprites[2];
    u32 wallpaperPalBits;
    u8 filler_0d0c[0x50];
    u16 unkUnused1; // Never read.
    s16 wallpaperSetId;
    s16 wallpaperId;
    u16 wallpaperTilemap[360];
    u8 wallpaperChangeState;
    struct Sprite *movingMonSprite;
    struct Sprite *partySprites[PARTY_SIZE];
    struct Sprite *boxMonsSprites[IN_BOX_COUNT];
    struct Sprite **shiftMonSpritePtr;
    struct Sprite **releaseMonSpritePtr;
    u16 numIconsPerSpecies[MAX_MON_ICONS];
    u16 iconSpeciesList[MAX_MON_ICONS];
    u8 shiftTimer;
    u8 numPartyToCompact;
    u16 iconScrollDistance;
    s16 iconScrollPos;
    s16 iconScrollSpeed;
    u16 iconScrollNumIncoming;
    u8 iconScrollCurColumn;
    s8 iconScrollDirection;
    u8 iconScrollState;
    u8 iconScrollToBoxId;
    u8 filler_117e[2];
    struct PokeStorageMenuItem menuItems[6];
    u8 filler_11b0[8];
    u8 menuItemsCount;
    u8 menuWidth;
    u16 menuLeft;
    u16 menuTop;
    u8 filler_11be;
    struct Sprite *cursorSprite;
    struct Sprite *cursorShadowSprite;
    s32 cursorNewX;
    s32 cursorNewY;
    s32 cursorSpeedX;
    s32 cursorSpeedY;
    s16 cursorTargetX;
    s16 cursorTargetY;
    u16 cursorMoveSteps;
    s8 cursorVerticalWrap;
    s8 cursorHorizontalWrap;
    u8 newCursorArea;
    u8 newCursorPosition;
    s8 cursorPrevHorizPos;
    u8 cursorFlipTimer;
    u8 cursorPalNums[4];
    const u8 *displayMonPalette;
    u32 displayMonPersonality;
    u16 displayMonSpecies;
    u16 displayMonItemId;
    u8 filler_11f4[2];
    u8 setMosaic;
    u8 displayMonMarkings;
    u8 displayMonLevel;
    u8 displayMonIsEgg;
    u8 displayMonName[0x15];
    u8 displayMonNameText[0x25];
    u8 displayMonSpeciesName[0x25];
    u8 displayMonGenderLvlText[0x21];
    u8 displayMonItemName[0x2a];
    bool8 (*monPlaceChangeFunc)(void);
    u8 monPlaceChangeState;
    u8 shiftBoxId;
    struct Sprite *markingComboSprite;
    struct Sprite *waveformSprites[2];
    u16 *markingComboTilesPtr;
    struct MonMarkingsMenu markMenu;
    struct ChooseBoxMenu chooseBoxMenu;
    struct Pokemon movingMon;
    struct Pokemon tempMon;
    u8 canReleaseMon;
    u8 releaseStatusResolved;
    u8 unresolvedReleaseSurf;
    u8 unresolvedReleaseDive;
    s8 releaseCheckBoxId;
    s8 releaseCheckBoxPos;
    s8 releaseBoxId;
    s8 releaseBoxPos;
    u16 releaseCheckState;
    u16 restrictedMoveList[3];
    u8 summaryMaxPos;
    u8 summaryStartPos;
    u8 summaryScreenMode;
    union {
        struct Pokemon *mon;
        struct BoxPokemon *box;
    } summaryMon;
    u8 messageText[18];
    // Its address is used as the one-past-end boundary for messageText.
    u8 messageTextBoundary[62];
    u8 releaseMonName[0x16];
    u16 displayMonPalOffset;
    u16 *displayMonTilePtr;
    struct Sprite *displayMonSprite;
    u16 displayMonPalBuffer[0x10];
    u8 filler_2724[0x60];
    u8 tileBuffer[0x800];
    u8 filler_2f84[0x1800];
    u8 displayMonPicBuffer[0x800];
};

extern u8 gPSSMenuHeaderTilemapBuffer[0xC00];
extern struct PokemonStorageSystemData *const gPokemonStorageSystemPtr;
extern u8 *const gPokeStorageTileBuffer;

u8 CountPartyMons(void);
s16 GetFirstFreeBoxSpot(u8 boxId);
u8 CountPartyAliveNonEggMonsExcept(u8 slotToIgnore);
u8 CountPartyAliveNonEggMons_IgnoreVar0x8004Slot(void);
u8 CountMonsInBox(u8 boxId);
u8 *StringCopyAndFillWithSpaces(u8 *dst, const u8 *src, u16 n);
void LoadChooseBoxMenuGfx(struct ChooseBoxMenu *menu, u16 tileTag, u16 palTag, u8 subpriority);
void CreateChooseBoxMenuSprites(u8 curBox);
u8 HandleChooseBoxMenuInput(void);
void DestroyChooseBoxMenuSprites(void);
void FreeChooseBoxMenu(void);
void CB2_ExitPokeStorage(void);
void EnterPokeStorage(u8 whichMenu);
void ResetPokemonStorageSystem(void);
void InitMonIconFields(void);
void InitBoxMonSprites(u8 boxId);
u8 StorageGetCurrentBox(void);
void CreateMovingMonIcon(void);
void CreateBoxMonIconAtPos(u8 boxPosition);
void InitBoxMonIconScroll(u8 boxId, s8 direction);
bool8 UpdateBoxMonIconScroll(void);
void CreatePartyMonsSprites(bool8 visible);
void CompactPartySprites(void);
u8 GetNumPartySpritesCompacting(void);
void DestroyMovingMonIcon(void);
void MovePartySprites(s16 yDelta);
void DestroyPartyMonIcon(u8 partyId);
void DestroyAllPartyMonIcons(void);
void SetMovingMonSprite(u8 mode, u8 id);
void SetPlacedMonSprite(u8 boxId, u8 position);
void SaveMonSpriteAtPos(u8 boxId, u8 position);
bool8 MoveShiftingMons(void);
void SetReleaseMon(u8 mode, u8 position);
bool8 TryHideReleaseMonSprite(void);
void DestroyReleaseMonIcon(void);
void ReshowReleaseMon(void);
bool8 ResetReleaseMonSpritePtr(void);
struct Sprite *CreateChooseBoxArrows(u16 x, u16 y, u8 animId, u8 priority, u8 subpriority);
void InitPokeStorageBox(u8 boxId);
void SetUpScrollToBox(u8 boxId);
bool8 ScrollToBox(void);
void SetWallpaperForCurrentBox(u8 wallpaperId);
bool8 DoWallpaperGfxChange(void);
void AnimateBoxScrollArrows(bool8 animate);
void InitCursor(void);
void InitCursorOnReopen(void);
bool8 UpdateCursorPos(void);
void SetCursorBoxPosition(u8 cursorBoxPosition);
void ClearSavedCursorPos(void);
void SaveCursorPos(void);
u8 GetSavedCursorPos(void);
void SetCursorInParty(void);
void InitMonPlaceChange(u8 type);
bool8 DoMonPlaceChange(void);
void RefreshDisplayMon(void);
bool8 TryStorePartyMonInBox(u8 boxId);
void ResetSelectionAfterDeposit(void);
void InitReleaseMon(void);
bool8 TryHideReleaseMon(void);
void ReleaseMon(void);
void TrySetCursorFistAnim(void);
void InitCanReleaseMonVars(void);
s8 RunCanReleaseMon(void);
void SaveMovingMon(void);
void LoadSavedMovingMon(void);
void InitSummaryScreenData(void);
void SetSelectionAfterSummaryScreen(void);
s16 CompactPartySlots(void);
void SetPokeStorageMonMarkings(u8 markings);
bool8 IsRemovingLastPartyMon(void);
bool8 CanShiftMon(void);
bool8 IsMonBeingMoved(void);
bool8 IsCursorOnCloseBox(void);
u8 HandlePokeStorageInput(void);
void InitPokeStorageMenu(void);
void SetPokeStorageMenuText(u8 menuItemId);
void AddPokeStorageMenu(void);
s16 HandlePokeStorageMenuInput(void);
void InitTilemapTransferQueue(struct TilemapTransferQueue *queue, struct TilemapTransfer *transfers, u8 capacity);
void RunTilemapTransferQueue(void);
bool8 QueueTilemapCopy(u8 *destination, u16 destLeft, u16 destTop, const u8 *source, u16 sourceLeft, u16 sourceTop, u16 width, u16 height);
void CopyTilemapRectFixedWidth(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height);
bool8 QueueTilemapFill(void *destination, u16 destLeft, u16 destTop, u16 width, u16 height);
void ShowPokemonStorageSystemPC(void);

void Debug_OpenPokeStorageForMon(struct Pokemon *pokemon, void (*func)(void));

#endif // GUARD_POKEMON_STORAGE_SYSTEM_H
