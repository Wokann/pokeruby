#ifndef GUARD_POKEMON_STORAGE_SYSTEM_H
#define GUARD_POKEMON_STORAGE_SYSTEM_H

#include "pc_screen_effect.h"
#include "mon_markings.h"

#define MAX_MON_ICONS max(IN_BOX_COUNT + PARTY_SIZE + 1, 40)

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

struct PokemonStorageSystemFunc
{
    u8 (*func)(void);
    s8 unk4;
};

struct StorageMessage {
    const u8 *text;
    u8 format;
};

struct StorageText {
    const u8 *text;
    int textId;
};

struct StorageMenuText {
    const u8 *text;
    const u8 *desc;
};

struct UnkStruct_2000028 {
    const u8 *unk_00;
    u8 *unk_04;
    u16 unk_08;
    u16 unk_0a;
    void (*unk_0c)(struct UnkStruct_2000028 *data);
};

struct UnkStruct_2000020 {
    struct UnkStruct_2000028 *unk_00;
    u8 unk_04;
    u8 unk_05;
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
    struct UnkStruct_2000020 unkUtil;
    struct UnkStruct_2000028 unkUtilData[8];
    u8 partyMenuTilemapBuffer[0x800];
    u16 partyMenuUnused1;
    u16 partyMenuY;
    u8 unk_08ac;
    u8 partyMenuMoveTimer;
    u8 showPartyMenuState;
    u8 closeBoxFlashing;
    u8 closeBoxFlashTimer;
    bool8 closeBoxFlashState;
    s16 newCurrBoxId;
    u16 bg2_X;
    u16 unk_08b6;
    u16 unk_08b8;
    bool8 unk_08ba;
    u8 unk_08bb;
    u16 unk_08bc;
    u16 unk_08be;
    u16 unk_08c0;
    u16 unk_08c2;
    u16 unk_08c4;
    u16 unk_08c6;
    u16 unk_08c8;
    u8 unk_08ca[0x400];
    u8 unk_0cca;
    u16 unk_0ccc[0x10];
    u16 unk_0cec;
    u16 unk_0cee;
    struct Sprite *unk_0cf0[2];
    struct Sprite *unk_0cf8[2];
    struct Sprite *unk_0d00[2];
    u32 unk_0d08;
    u8 filler_0d0c[0x50];
    u16 unk_0d5c;
    s16 wallpaperSetId;
    s16 wallpaperId;
    u16 unk_0d62[360];
    u8 unk_1032;
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
    struct StorageText unk_1180[6];
    u8 filler_11b0[8];
    u8 unk_11b8;
    u8 unk_11b9;
    u16 unk_11ba;
    u16 unk_11bc;
    u8 filler_11be;
    struct Sprite *unk_11c0;
    struct Sprite *unk_11c4;
    s32 unk_11c8;
    s32 unk_11cc;
    s32 unk_11d0;
    s32 unk_11d4;
    s16 unk_11d8;
    s16 unk_11da;
    u16 unk_11dc;
    s8 unk_11de;
    s8 unk_11df;
    u8 unk_11e0;
    u8 unk_11e1;
    s8 unk_11e2;
    u8 unk_11e3;
    u8 unk_11e4[4];
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
    struct Pokemon unk_2618;
    u8 unk_267c;
    u8 unk_267d;
    u8 unk_267e;
    u8 unk_267f;
    s8 unk_2680;
    s8 unk_2681;
    s8 unk_2682;
    s8 unk_2683;
    u16 unk_2684;
    u16 unk_2686[3];
    u8 summaryMaxPos;
    u8 summaryStartPos;
    u8 summaryScreenMode;
    union {
        struct Pokemon *pokemon;
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
    u8 displayMonPicBuffer[0x800];
};

extern u8 gUnknown_02039760[0xC00];
extern struct PokemonStorageSystemData *const gPokemonStorageSystemPtr;
extern u8 *const gUnknown_083B6DB8;

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
void sub_8099BF8(u8 a0);
void sub_8099C70(u8 whichBox);
bool8 sub_8099D34(void);
void sub_8099DCC(u8 a0);
bool8 sub_8099E08(void);
void sub_809A860(bool8 a0);
void sub_809AA24(void);
void sub_809AA98(void);
bool8 sub_809AC00(void);
void sub_809B0C0(u8 a0);
void sub_809B0D4(void);
void sub_809B0E0(void);
u8 sub_809B0F4(void);
void sub_809B068(void);
void sub_809B100(u8 a0);
bool8 sub_809B130(void);
void sub_809B440(void);
bool8 sub_809B62C(u8);
void sub_809B6BC(void);
void sub_809B6DC(void);
bool8 sub_809B734(void);
void sub_809B760(void);
void sub_809B7AC(void);
void sub_809B7D4(void);
s8 sub_809B960(void);
void sub_809BB90(void);
void sub_809BBC0(void);
void sub_809BC18(void);
void sub_809BD14(void);
s16 CompactPartySlots(void);
void sub_809BDD8(u8 markings);
bool8 sub_809BE80(void);
bool8 sub_809BEBC(void);
bool8 sub_809BF20(void);
bool8 sub_809BF48(void);
u8 sub_809CA40(void);
void sub_809CDCC(void);
void sub_809CDEC(u8 a0);
void sub_809CE84(void);
s16 sub_809CF30(void);
void sub_809CFDC(struct UnkStruct_2000020 *a0, struct UnkStruct_2000028 *a1, u8 a2);
void sub_809CFF0(void);
bool8 sub_809D034(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height);
void sub_809D104(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height);
bool8 sub_809D16C(void *dest, u16 dLeft, u16 dTop, u16 width, u16 height);
void ShowPokemonStorageSystemPC(void);

void Debug_OpenPokeStorageForMon(struct Pokemon *pokemon, void (*func)(void));

#endif // GUARD_POKEMON_STORAGE_SYSTEM_H
