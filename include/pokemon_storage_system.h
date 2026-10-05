#ifndef GUARD_POKEMON_STORAGE_SYSTEM_H
#define GUARD_POKEMON_STORAGE_SYSTEM_H

#include "pc_screen_effect.h"
#include "mon_markings.h"

struct PokemonStorageSystemFunc
{
    u8 (*func)(void);
    s8 area;
};

struct StorageMenu {
    const u8 *text;
    int textId;
};

struct PSS_MenuStringPtrs {
    const u8 *text;
    const u8 *desc;
};

struct UnkUtilData {
    const u8 *src;
    u8 *dest;
    u16 size;
    u16 height;
    void (*func)(struct UnkUtilData *data);
};

struct UnkUtil {
    struct UnkUtilData *data;
    u8 numActive;
    u8 max;
};

struct ChooseBoxMenu {
    struct Sprite *menuSprite;
    struct Sprite *menuSideSprites[4];
    u32 unused1[3];
    struct Sprite *arrowSprites[2];
    u8 filler_0028[0x214];
    u8 curBox;
    u8 unused3;
    u8 subpriority;
    u16 tileTag;
    u16 paletteTag;
}; // 0244

struct PokemonStorageSystemData {
    void (*unk_0000)(void);
    u8 state;
    u8 boxOption;
    u8 screenChangeType;
    u8 unk_0007;
    u16 unk_0008;
    u16 unk_000a;
    struct PCScreenEffectStruct unk_000c;
    struct UnkUtil unkUtil;
    struct UnkUtilData unkUtilData[8];
    u8 unk_00a8[0x800];
    u16 unk_08a8;
    u16 partyMenuY;
    u8 unk_08ac;
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
    u16 unk_08c4;
    u16 scrollUnused5;
    u16 scrollUnused6;
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
    u16 unkUnused1;
    s16 wallpaperSetId;
    s16 wallpaperId;
    u16 wallpaperTilemap[360];
    u8 wallpaperChangeState;
    struct Sprite *movingMonSprite;
    struct Sprite *partySprites[6]; // party
    struct Sprite *boxMonsSprites[30]; // box
    struct Sprite **shiftMonSpritePtr;
    struct Sprite **releaseMonSpritePtr;
    u16 numIconsPerSpecies[40];
    u16 iconSpeciesList[40];
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
    struct StorageMenu menuItems[6];
    u8 filler_11b0[8];
    u8 menuItemsCount;
    u8 menuWidth;
    u16 unk_11ba;
    u16 unk_11bc;
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
    struct PokemonMarkMenu markMenu;
    struct ChooseBoxMenu chooseBoxMenu;
    struct Pokemon movingMon;
    struct Pokemon tempMon;
    u8 canReleaseMon;
    u8 releaseStatusResolved;
    u8 unk_267e;
    u8 unk_267f;
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
    u8 unk_26a6[62];
    u8 releaseMonName[0x16];
    u16 displayMonPalOffset;
    u16 *displayMonTilePtr;
    struct Sprite *displayMonSprite;
    u16 displayMonPalBuffer[0x10];
    u8 filler_2724[0x60];
    u8 tileBuffer[0x800];
    u8 filler_2f84[0x1800];
    u8 unk_4784[0x800];
};

extern u8 gUnknown_02039760[0xC00];
extern struct PokemonStorageSystemData *const gPokemonStorageSystemPtr;
extern u8 *const gUnknown_083B6DB8;

u8 CountPartyMons(void);
s16 GetFirstFreeBoxSpot(u8 boxId);
u8 CountPartyAliveNonEggMonsExcept(u8 slotToIgnore);
u8 CountMonsInBox(u8 boxId);
void LoadChooseBoxMenuGfx(struct ChooseBoxMenu *menu, u16 tileTag, u16 palTag, u8 subpriority);
void CreateChooseBoxMenuSprites(u8 curBox);
u8 HandleChooseBoxMenuInput(void);
void DestroyChooseBoxMenuSprites(void);
void FreeChooseBoxMenu(void);
void CB2_ExitPokeStorage(void);
void EnterPokeStorage(u8 boxOption);
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
void sub_8099BF8(u8 a0);
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
void sub_809BDD8(u8 markings);
bool8 IsRemovingLastPartyMon(void);
bool8 CanShiftMon(void);
bool8 IsMonBeingMoved(void);
bool8 IsCursorOnCloseBox(void);
u8 HandleInput(void);
void sub_809CDCC(void);
void SetMenuText(u8 textId);
void AddMenu(void);
s16 HandleMenuInput(void);
void UnkUtil_Init(struct UnkUtil *util, struct UnkUtilData *data, u8 max);
void UnkUtil_Run(void);
bool8 sub_809D034(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height);
void sub_809D104(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height);
bool8 UnkUtil_DmaAdd(void *dest, u16 dLeft, u16 dTop, u16 width, u16 height);
void ShowPokemonStorageSystemPC(void);

void debug_sub_80A433C(struct Pokemon *pokemon, void (*func)(void));

#endif // GUARD_POKEMON_STORAGE_SYSTEM_H
