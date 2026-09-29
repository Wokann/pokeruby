#include "global.h"
#include "battle.h"
#include "battle_anim_throw.h"
#include "contest.h"
#include "data2.h"
#include "daycare.h"
#include "decompress.h"
#include "event_data.h"
#include "ewram.h"
#include "item.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "move_relearner.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokeball.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "region_map.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "strings2.h"
#include "task.h"
#include "tv.h"
#include "constants/items.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/region_map_sections.h"

#define sSummaryScreenSpriteIds         (&gSharedMem[0x1A000])
#define sMoveSelectorSpriteIds          ((u8 *)(gSharedMem + 0x1A009))

static void SummaryScreen_PrintPokemonInfoLabels(void);
static void SummaryScreen_PrintPokemonSkillsLabels(void);
static void RefreshInfoPage(struct Pokemon *);
static void RefreshSkillsPage(struct Pokemon *);
static void RefreshMovesPage(struct Pokemon *);
static void PrintInfoPageText(struct Pokemon *);
static void PrintSkillsPageText(struct Pokemon *);
static void PrintMoveNamesAndPP(struct Pokemon *);
static void CB2_InitSummaryScreen(void);
static void Task_SetHandleReplaceMoveInput(u8);
static void SetDefaultTilemaps(void);
static void SummaryScreen_LoadPalettes(void);
static void CreateMonMarkingsSprite(struct Pokemon *);
static void ResetSpriteIds(void);
static bool8 DecompressGraphics(void);
static bool8 HasMoreThanOneMove(struct Pokemon *);
static void PrintSummaryWindowHeaderText(void);
static void RemoveAndCreateMonMarkingsSprite(struct Pokemon *);
static void ClearInfoPageText(void);
static void ClearSkillsPageText(void);
static void ClearMoveNamesAndPP(void);
extern u8 CreateMonSprite(struct Pokemon *);
static void SummaryScreen_PrintColoredIntPixelCoords(s32, u8, u8, u8, u8, u16, s32);
static void PrintMonSpeciesLevelAndGender(struct Pokemon *);
static void PokemonSummaryScreen_PrintTrainerMemo(struct Pokemon *, u8, u8);
static void PokemonSummaryScreen_PrintEggTrainerMemo(struct Pokemon *, u8, u8);
static void SummaryScreen_PrintColoredTextPixelCoords(const u8 *, u8, u8, u16, s32);
static void SummaryScreen_PrintColoredTextCentered(const u8 *, u8, u8, u8, u16);
static void PrintHeldItemName(u16, u8, u8);
static void PrintRibbonCount(struct Pokemon *);
static void DrawExperienceProgressBar(struct Pokemon *, u8, u8);
static void CloseSummaryScreen(u8 taskId);
static void CreateMoveTypeIcons(void);
static void InitBGs(void);
static void SwitchToMoveSelection(u8);
void ChangeSummaryPokemon(u8, s8);
static bool8 CanReplaceMove(u8);
static void ShowCantForgetHMsWindow(u8, u8);
static void CloseMoveSelectMode(u8);
static void Task_HandleInput_MovePositionSwitch(u8);
static void ExitMovePositionSwitchMode(u8, s8);
static void SetMainMoveSelectorColor(u8);
static void SetTaskWaitForLinkRecv(int);
static void Task_HandleInput_MoveSelect(u8);
static void Task_HandleInput(u8);
static void DestroyMoveSelectorSprites(u8);
static void TilemapFiveMovesDisplay(u8);
static void CreateMoveSelectorSprites(u8);
void DrawPagination(void);
static void PrintMoveDetailLabels(u8);
static void PrintNewMoveDetailsOrCancelText(struct Pokemon *);
static void HidePageSpecificSprites(void);
static void Task_SlideAppealJamWindow(u8);
static void Task_SlidePowerAccWindow(u8);
extern void Task_ChangeSummaryMon(u8);
extern s8 AdvanceMonIndex(s8);
extern s8 AdvanceMultiBattleMonIndex(s8);
static bool8 RefreshSummaryScreen(void);
static void CreateCaughtBallSprite(struct Pokemon *);
static u8 LoadMonGfxAndSprite(struct Pokemon *, u8 *);
static void DrawPokerusCuredSymbol(struct Pokemon *);
static void PositionStatusSlidingWindow(s8);
static void PrintMonInfo(struct Pokemon *);
static void HandlePageChangeInput(u8, s8);
static void ChangeSelectedMove(u8 taskId, s8 direction, u8 *moveIndexPtr);
static void PositionAppealJamSlidingWindow(s8, u8);
static void PositionPowerAccSlidingWindow(s8, u8);
static void SummaryScreen_PrintPokemonInfo(struct Pokemon *);
static void SummaryScreen_PrintPokemonSkills(struct Pokemon *);
static void SetSpriteInvisibility(u8, u8);
static void KeepMoveSelectorVisible(u8 a);
static void SetTypeSpritePosAndPal(u8, u8, u8, u8);
static u16 GetMonMove(struct Pokemon *, u8);
static void PrintMovePowerAndAccuracy(u16);
void DrawContestMoveHearts(u16);
static void PrintContestMoveDescription(u16);
static void PrintBattleMoveDescription(u16);
static void Task_WaitForLinkRecv(u8);
static void CopyMonToSummaryStruct(struct Pokemon *);
static void SpriteCB_MoveSelector(struct Sprite *sprite);
static void SpriteCB_Pokemon(struct Sprite *);
static void PrintMoveDetails(struct Pokemon *, u8 *);
static void DestroySpriteInArray(u8);
static u8 *SummaryScreen_SetTextColor(u8 *, u8);
static u8 *SummaryScreen_CopyColoredString(u8 *, const u8 *, u8);
static void PrintGenderSymbol(struct Pokemon *, u8, u8);
static void SummaryScreen_PrintColoredText(const u8 *, u8, u8, u8);

extern u8 AdvanceStorageMonIndex(struct BoxPokemon *, u8, u8, u8);

extern struct MusicPlayerInfo gMPlayInfo_BGM;
TaskFunc gTaskFuncForLinkRecv;
extern struct SpriteTemplate gCreatingSpriteTemplate;

extern const u8 gStatusPal_Icons[];
extern const u8 gStatusGfx_Icons[];
extern const u8 gMenuSummaryPal[];
extern const u8 gMenuSummaryGfx[];
extern const u8 gMoveTypes_Gfx[];
extern const u8 gMoveTypes_Pal[];
extern const u8 gStatusScreen_Pal[];
extern const u8 gStatusScreen_Tilemap[];
extern const u8 gUnknown_08E74688[];
extern const u8 gUnknown_08E74E88[];
extern const u16 gUnknown_08E73508[];
extern const u8 gStatusScreen_Gfx[];
extern const u8 gFontDefaultPalette[];
extern const u8 gAbilityNames[][13];
extern const u8 * const gAbilityDescriptions[];
extern const u8 * const gContestEffectStrings[];
extern const struct ContestMove gContestMoves[];
extern const struct ContestEffect gContestEffects[];
extern const u16 gUnknown_08E94510[];
extern const u16 gUnknown_08E94550[];
extern const u16 gUnknown_08E94590[];
extern const u8 gUnknown_08E73E88[];

EWRAM_DATA u8 gLastViewedMonIndex = 0;
EWRAM_DATA struct Sprite *sMarkingsSprite = NULL;

#if ENGLISH
#include "data/text/move_descriptions_en.h"
#include "data/text/nature_names_en.h"
#elif GERMAN
#include "data/text/move_descriptions_de.h"
#include "data/text/nature_names_de.h"
#endif

static const u8 * const sPageHeaderTexts[] = {
    gEmptyString_81E72B0,
    OtherText_PokeInfo,
    OtherText_PokeSkills,
    OtherText_BattleMoves,
    OtherText_ContestMoves,
    OtherText_Switch,
    OtherText_Info,
    gOtherText_CancelNoTerminator,
};

static const union AffineAnimCmd sUnusedSpriteAffineAnim[] = {
    AFFINEANIMCMD_FRAME(0xFF00, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sUnusedSpriteAffineAnimTable[] = {
    sUnusedSpriteAffineAnim,
};

#define TAG_MOVE_SELECTOR 30000
#define TAG_MON_STATUS    30001
#define TAG_MOVE_TYPES    30002
#define TAG_MON_MARKINGS  30003

static const struct OamData sOamData_MoveTypes =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sSpriteAnim_TypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCool[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_COOL + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryBeauty[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_BEAUTY + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCute[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_CUTE + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategorySmart[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_SMART + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryTough[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_TOUGH + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MoveTypes[NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT] = {
    sSpriteAnim_TypeNormal,
    sSpriteAnim_TypeFighting,
    sSpriteAnim_TypeFlying,
    sSpriteAnim_TypePoison,
    sSpriteAnim_TypeGround,
    sSpriteAnim_TypeRock,
    sSpriteAnim_TypeBug,
    sSpriteAnim_TypeGhost,
    sSpriteAnim_TypeSteel,
    sSpriteAnim_TypeMystery,
    sSpriteAnim_TypeFire,
    sSpriteAnim_TypeWater,
    sSpriteAnim_TypeGrass,
    sSpriteAnim_TypeElectric,
    sSpriteAnim_TypePsychic,
    sSpriteAnim_TypeIce,
    sSpriteAnim_TypeDragon,
    sSpriteAnim_TypeDark,
    sSpriteAnim_CategoryCool,
    sSpriteAnim_CategoryBeauty,
    sSpriteAnim_CategoryCute,
    sSpriteAnim_CategorySmart,
    sSpriteAnim_CategoryTough,
};

static const struct CompressedSpriteSheet sSpriteSheet_MoveTypes =
{
    .data = gMoveTypes_Gfx,
    .size = (NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT) * 0x100,
    .tag = TAG_MOVE_TYPES
};

static const struct SpriteTemplate sSpriteTemplate_MoveTypes =
{
    .tileTag = TAG_MOVE_TYPES,
    .paletteTag = TAG_MOVE_TYPES,
    .oam = &sOamData_MoveTypes,
    .anims = sSpriteAnimTable_MoveTypes,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const u8 sMoveTypeToOamPaletteNum[NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT] =
{
    [TYPE_NORMAL] = 13,
    [TYPE_FIGHTING] = 13,
    [TYPE_FLYING] = 14,
    [TYPE_POISON] = 14,
    [TYPE_GROUND] = 13,
    [TYPE_ROCK] = 13,
    [TYPE_BUG] = 15,
    [TYPE_GHOST] = 14,
    [TYPE_STEEL] = 13,
    [TYPE_MYSTERY] = 15,
    [TYPE_FIRE] = 13,
    [TYPE_WATER] = 14,
    [TYPE_GRASS] = 15,
    [TYPE_ELECTRIC] = 13,
    [TYPE_PSYCHIC] = 14,
    [TYPE_ICE] = 14,
    [TYPE_DRAGON] = 15,
    [TYPE_DARK] = 13,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_COOL] = 13,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_BEAUTY] = 14,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_CUTE] = 14,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_SMART] = 15,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_TOUGH] = 13,
};
static const struct OamData sOamData_MoveSelector = {
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_MoveSelector0[] = { ANIMCMD_FRAME(0, 0), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelector1[] = { ANIMCMD_FRAME(4, 0), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelector2[] = { ANIMCMD_FRAME(8, 0), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelector3[] = { ANIMCMD_FRAME(12, 0), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelectorLeft[] = { ANIMCMD_FRAME(16, 0), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelectorRight[] = { ANIMCMD_FRAME(16, 0, .hFlip = TRUE), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelectorMiddle[] = { ANIMCMD_FRAME(20, 0), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelector7[] = { ANIMCMD_FRAME(24, 0), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelector8[] = { ANIMCMD_FRAME(24, 0, .hFlip = TRUE), ANIMCMD_END, };
static const union AnimCmd sSpriteAnim_MoveSelector9[] = { ANIMCMD_FRAME(28, 0), ANIMCMD_END, };

static const union AnimCmd *const sSpriteAnimTable_MoveSelector[] = {
    sSpriteAnim_MoveSelector0,
    sSpriteAnim_MoveSelector1,
    sSpriteAnim_MoveSelector2,
    sSpriteAnim_MoveSelector3,
    sSpriteAnim_MoveSelectorLeft,
    sSpriteAnim_MoveSelectorRight,
    sSpriteAnim_MoveSelectorMiddle,
    sSpriteAnim_MoveSelector7,
    sSpriteAnim_MoveSelector8,
    sSpriteAnim_MoveSelector9,
};

static const struct CompressedSpriteSheet sMoveSelectorSpriteSheet = { gMenuSummaryGfx, 0x400, TAG_MOVE_SELECTOR };
static const struct CompressedSpritePalette sMoveSelectorSpritePal = { gMenuSummaryPal, TAG_MOVE_SELECTOR };

static const struct SpriteTemplate sMoveSelectorSpriteTemplate = {
    .tileTag = TAG_MOVE_SELECTOR,
    .paletteTag = TAG_MOVE_SELECTOR,
    .oam = &sOamData_MoveSelector,
    .anims = sSpriteAnimTable_MoveSelector,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct OamData sOamData_StatusCondition =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_StatusPoison[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusParalyzed[] = {
    ANIMCMD_FRAME(4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusSleep[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFrozen[] = {
    ANIMCMD_FRAME(12, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusBurn[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusPokerus[] = {
    ANIMCMD_FRAME(20, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFaint[] = {
    ANIMCMD_FRAME(24, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_StatusCondition[] =
{
    sSpriteAnim_StatusPoison,
    sSpriteAnim_StatusParalyzed,
    sSpriteAnim_StatusSleep,
    sSpriteAnim_StatusFrozen,
    sSpriteAnim_StatusBurn,
    sSpriteAnim_StatusPokerus,
    sSpriteAnim_StatusFaint,
};

static const struct CompressedSpriteSheet sStatusIconsSpriteSheet =
{
    .data = gStatusGfx_Icons,
    .size = 0x380,
    .tag = TAG_MON_STATUS
};

static const struct CompressedSpritePalette sStatusIconsSpritePalette =
{
    .data = gStatusPal_Icons,
    .tag = TAG_MON_STATUS
};

static const struct SpriteTemplate sSpriteTemplate_StatusCondition =
{
    .tileTag = TAG_MON_STATUS,
    .paletteTag = TAG_MON_STATUS,
    .oam = &sOamData_StatusCondition,
    .anims = sSpriteAnimTable_StatusCondition,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const u16 sMarkings_Pal[] = INCBIN_U16("graphics/summary_screen/mon_markings.gbapal");

#if ENGLISH
static const u16 gSummaryScreenTextTiles[] = INCBIN_U16("graphics/summary_screen/text.4bpp");
#elif GERMAN
extern const u16 gSummaryScreenTextTiles[];
#endif

static const u16 sSummaryScreenButtonTiles[] = INCBIN_U16("graphics/summary_screen/buttons.4bpp");

static const u16 sDexNumberNormalPalette[] = { RGB(26, 26, 23) };
static const u16 sDexNumberShinyPalette[] = { RGB(30, 30, 27) };

static void (*const sPageLabelPrinters[])(void) = {
    SummaryScreen_PrintPokemonInfoLabels,
    SummaryScreen_PrintPokemonSkillsLabels,
};

static void (*const sPageRefreshFuncs[])(struct Pokemon *) = {
    RefreshInfoPage,
    RefreshSkillsPage,
    RefreshMovesPage,
    RefreshMovesPage,
};

static void (*const sPageTextPrinters[])(struct Pokemon *) = {
    PrintInfoPageText,
    PrintSkillsPageText,
    PrintMoveNamesAndPP,
    PrintMoveNamesAndPP,
};

static const u8 sMultiBattleOrder[] = { 0, 2, 3, 1, 4, 5 };

static const u8 sRibbonsTextTemplate[] = _("{STR_VAR_1}{CLEAR_TO 64}");
static const u8 sHeldItemTextTemplate[] = _("{STR_VAR_1}{CLEAR_TO 72}");

struct TextColors {
    u8 id;
    u8 color;
    u8 background;
    u8 shadow;
};

// The TEXT_COLOR constants don't apply. Refer to the palette for this screen.
static const struct TextColors sSummaryScreenTextColors[] = {
    {  9,  1, 0,  2 }, // blue
    { 10,  3, 0,  4 }, // pink
    {  8,  5, 0,  6 }, // yellow
    { 11,  7, 0,  8 }, // blue2
    { 14,  9, 0, 10 }, // red
    { 12, 11, 0, 12 }, // pink2
    { 13, 13, 0, 14 }, // white
    { -1, 15, 0, 10 }, // black
};

void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
#if DEBUG
    if (gLinkOpen == TRUE)
        debug_sub_8008264(gLink.recvQueue.count, 20, 1, 2, 3);
#endif
}

void VBlank(void)
{
    REG_BG1HOFS = gBattle_BG1_X;
    REG_BG1VOFS = gBattle_BG1_Y;
    REG_BG2HOFS = gBattle_BG2_X;
    REG_BG2VOFS = gBattle_BG2_Y;
    REG_BG3HOFS = gBattle_BG3_X;
    REG_BG3VOFS = gBattle_BG3_Y;

    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void ShowPokemonSummaryScreen(struct Pokemon *party, u8 curMonIndex, u8 maxMonIndex, MainCallback callback, u8 mode)
{
    gPaletteFade.bufferTransferDisabled = 1;

    pssData.monList.partyMons = party;
    pssData.mode = mode;
    pssData.curMonIndex = curMonIndex;
    pssData.maxMonIndex = maxMonIndex;
    pssData.callback = callback;
    pssData.loadState = 0;
    pssData.firstMoveIndex = 4;
    pssData.newMove = 0;
    pssData.bgDisplayOrder = 0;
    pssData.lockMovesFlag = FALSE;

    if (mode >= PSS_MODE_PC_NORMAL)
        pssData.isBoxMon = TRUE;
    else
        pssData.isBoxMon = FALSE;

    switch (mode)
    {
    case PSS_MODE_NORMAL:
    case PSS_MODE_PC_NORMAL:
        pssData.minPageIndex = PSS_PAGE_INFO;
        pssData.maxPageIndex = PSS_PAGE_CONTEST_MOVES;
        pssData.unk77 = 0;
        pssData.unk78 = 0;
        pssData.headerTextId = 1;
        pssData.headerActionTextId = 7;
        break;
    case PSS_MODE_NO_MOVE_ORDER_EDIT:
        pssData.minPageIndex = PSS_PAGE_INFO;
        pssData.maxPageIndex = PSS_PAGE_CONTEST_MOVES;
        pssData.unk77 = 0;
        pssData.unk78 = 0;
        pssData.headerTextId = 1;
        pssData.headerActionTextId = 7;
        pssData.lockMovesFlag = TRUE;
        break;
    case PSS_MODE_SELECT_MOVE:
        pssData.minPageIndex = PSS_PAGE_BATTLE_MOVES;
        pssData.maxPageIndex = PSS_PAGE_CONTEST_MOVES;
        pssData.unk77 = 1;
        pssData.unk78 = 1;
        pssData.headerTextId = 3;
        pssData.headerActionTextId = 0;
        pssData.firstMoveIndex = 0;
        break;
    case PSS_MODE_MOVES_ONLY:
    case PSS_MODE_PC_MOVES_ONLY:
        pssData.minPageIndex = PSS_PAGE_BATTLE_MOVES;
        pssData.maxPageIndex = PSS_PAGE_CONTEST_MOVES;
        pssData.unk77 = 1;
        pssData.unk78 = 1;
        break;
    }

    pssData.currPageIndex = pssData.minPageIndex;
    SetMainCallback2(CB2_InitSummaryScreen);
}

void ShowSelectMovePokemonSummaryScreen(struct Pokemon *party, u8 curMonIndex, u8 maxMonIndex, MainCallback callback, u16 move)
{
    ShowPokemonSummaryScreen(party, curMonIndex, maxMonIndex, callback, PSS_MODE_SELECT_MOVE);
    pssData.newMove = move;
}

void CreateSummaryScreenInputTask(void)
{
    switch (pssData.mode)
    {
    case PSS_MODE_NORMAL:
    case PSS_MODE_PC_NORMAL:
        pssData.inputHandlingTaskId = CreateTask(Task_HandleInput, 0);
        break;
    case PSS_MODE_NO_MOVE_ORDER_EDIT:
        pssData.inputHandlingTaskId = CreateTask(Task_HandleInput, 0);
        break;
    case PSS_MODE_SELECT_MOVE:
    case PSS_MODE_MOVE_DELETER:
        pssData.inputHandlingTaskId = CreateTask(Task_SetHandleReplaceMoveInput, 0);
        break;
    case PSS_MODE_MOVES_ONLY:
    case PSS_MODE_PC_MOVES_ONLY:
        pssData.inputHandlingTaskId = CreateTask(Task_HandleInput_MoveSelect, 0);
        break;
    }
}

bool8 LoadGraphics(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        ResetSpriteData();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 2:
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 3:
        DmaClearLarge(3, (void *)(VRAM + 0x0), 0x10000, 0x1000, 32);
        gMain.state++;
        break;
    case 4:
        InitBGs();
        gMain.state++;
        break;
    case 5:
        Text_LoadWindowTemplate(&gWindowTemplate_81E6E6C);
        gMain.state++;
        break;
    case 6:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E6E6C);
        gMain.state++;
        break;
    case 7:
        if (MultistepInitMenuWindowContinue())
            gMain.state++;
        break;
    case 8:
        CreateSummaryScreenInputTask();
        gMain.state++;
        break;
    case 9:
        DmaCopy16Defvars(3, gSummaryScreenTextTiles, (void *)(VRAM + 0xD000), 320);
        DmaCopy16Defvars(3, sSummaryScreenButtonTiles, (void *)(VRAM + 0xD140), 256);
        pssData.loadState = 0;
        gMain.state++;
        break;
    case 10:
        if (DecompressGraphics())
        {
            pssData.loadState = 0;
            gMain.state++;
        }
        break;
    case 11:
        ResetSpriteIds();
        gMain.state++;
        break;
    case 12:
        CopyMonToSummaryStruct(&pssData.currentMon);
        if (!GetMonStatusAndPokerus(&pssData.currentMon))
            PositionStatusSlidingWindow(0);
        else
            PositionStatusSlidingWindow(10);
        DrawPokerusCuredSymbol(&pssData.currentMon);
        gMain.state++;
        break;
    case 13:
        CreateMoveTypeIcons();
        CreateMonMarkingsSprite(&pssData.currentMon);
        gMain.state++;
        break;
    case 14:
        CreateCaughtBallSprite(&pssData.currentMon);
        pssData.loadState = 0;
        gMain.state++;
        break;
    case 15:
        if ((pssData.monSpriteId = LoadMonGfxAndSprite(&pssData.currentMon, &pssData.loadState)) != 0xFF)
        {
            pssData.loadState = 0;
            gMain.state++;
        }
        break;
    case 16:
        SummaryScreen_LoadPalettes();
        DrawPagination();
        gMain.state++;
        break;
    case 17:
        if (pssData.currPageIndex <= PSS_PAGE_SKILLS)
            sPageLabelPrinters[pssData.currPageIndex]();
        gMain.state++;
        break;
    case 18:
        PrintMonInfo(&pssData.currentMon);
        gMain.state++;
        break;
    case 19:
        sPageTextPrinters[pssData.currPageIndex](&pssData.currentMon);
        gMain.state++;
        break;
    case 20:
        if (GetMonData(&pssData.currentMon, MON_DATA_IS_EGG))
            gBattle_BG3_X = 256;
        else
            gBattle_BG3_X = 0;
        gMain.state++;
        break;
    case 21:
        SetDefaultTilemaps();
        if (pssData.firstMoveIndex != 0)
        {
            PositionPowerAccSlidingWindow(0, 0);
            PositionAppealJamSlidingWindow(0, 0);
        }
        else
        {
            PositionPowerAccSlidingWindow(10, 0);
            PositionAppealJamSlidingWindow(10, 0);
        }
        PrintSummaryWindowHeaderText();
        gMain.state++;
        break;
    case 22:
        if (Overworld_IsRecvQueueAtMax() != TRUE)
            gMain.state++;
        break;
    default:
        SetVBlankCallback(VBlank);
        BeginHardwarePaletteFade(0xFF, 0, 16, 0, 1);
        SetMainCallback2(MainCB2);
        gPaletteFade.bufferTransferDisabled = 0;
#if DEBUG
        if (gLinkOpen == TRUE)
            debug_sub_8008218((void *)(VRAM + 0xDDE0), 0x8000, (void *)(VRAM + 0xF000), 3);
#endif
        return TRUE;
    }

    return FALSE;
}

static void CB2_InitSummaryScreen(void)
{
    while (LoadGraphics() != TRUE && MenuHelpers_IsLinkActive() != TRUE)
        ;
}

static void InitBGs(void)
{
    REG_BG0CNT = 0x1E08;
    REG_BG1CNT = 0x4801;
    REG_BG2CNT = 0x4A02;
    REG_BG3CNT = 0x5C03;

    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;

    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;

    REG_BLDCNT = 0;
    REG_DISPCNT = 0x1F40;
}

static bool8 DecompressGraphics(void)
{
    switch (pssData.loadState)
    {
    case 0:
        LZDecompressVram(gStatusScreen_Gfx, (void *)VRAM + 0);
        break;
    case 1:
        CpuSet(gUnknown_08E73508, (void *)VRAM + 0xE000, 0x400);
        break;
    case 2:
        LZDecompressVram(gUnknown_08E74E88, (void *)VRAM + 0xE800);
        break;
    case 3:
        LZDecompressVram(gStatusScreen_Tilemap, (void *)VRAM + 0x4800);
        break;
    case 4:
        CpuSet(gUnknown_08E73E88, (void *)VRAM + 0x5800, 0x400);
        break;
    case 5:
        CpuSet(gUnknown_08E74688, (void *)VRAM + 0x6800, 0x400);
        break;
    case 6:
        LoadCompressedPalette(gStatusScreen_Pal, 0, 160);
        break;
    case 7:
        LoadCompressedSpriteSheet(&sSpriteSheet_MoveTypes);
        break;
    case 8:
        LoadCompressedSpriteSheet(&sMoveSelectorSpriteSheet);
        break;
    case 9:
        LoadCompressedSpriteSheet(&sStatusIconsSpriteSheet);
        break;
    case 10:
        LoadCompressedSpritePalette(&sStatusIconsSpritePalette);
        break;
    case 11:
        LoadCompressedSpritePalette(&sMoveSelectorSpritePal);
        break;
    case 12:
        LoadCompressedPalette(gMoveTypes_Pal, 464, 96);
        pssData.loadState = 0;
        return TRUE;
    }

    pssData.loadState++;
    return FALSE;
}

static void SummaryScreen_LoadPalettes(void)
{
    LoadPalette(gUnknownPalette_81E6692 + 14, 129, 2);
    LoadPalette(gUnknownPalette_81E6692 + 15, 136, 2);
    LoadPalette(gUnknownPalette_81E6692 + 14, 143, 2);
    LoadPalette(gUnknownPalette_81E6692 + 15, 137, 2);
    LoadPalette(gUnknownPalette_81E6692 + 6,  209, 4);
    LoadPalette(gUnknownPalette_81E6692 + 10, 211, 4);
    LoadPalette(gUnknownPalette_81E6692 + 14, 213, 4);
    LoadPalette(gUnknownPalette_81E6692 + 6,  215, 4);
    LoadPalette(gUnknownPalette_81E6692 + 4,  217, 4);
    LoadPalette(gUnknownPalette_81E6692 + 8,  219, 4);
    LoadPalette(gUnknownPalette_81E6692 + 2,  221, 2);
    LoadPalette(gUnknownPalette_81E6692 + 3,  222, 2);
    LoadPalette(gUnknownPalette_81E6692 + 1,  223, 2);
    LoadPalette(gFontDefaultPalette,          240, 32);
    LoadPalette(gUnknownPalette_81E6692 + 3,  249, 2);
}

static void BeginCloseSummaryScreen(u8 taskId)
{
    PlaySE(SE_SELECT);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = CloseSummaryScreen;
}

static void CloseSummaryScreen(u8 taskId)
{
    if (Overworld_IsRecvQueueAtMax() != TRUE && !gPaletteFade.active)
    {
        gLastViewedMonIndex = pssData.curMonIndex;

        ResetSpriteData();
        FreeAllSpritePalettes();
        StopCryAndClearCrySongs();
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x100);
        SetMainCallback2(pssData.callback);
        DestroyTask(taskId);
    }
}

static void Task_HandleInput(u8 taskId)
{
    if (gPaletteFade.active)
        return;

    if (JOY_NEW(DPAD_UP))
    {
        ChangeSummaryPokemon(taskId, -1);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        ChangeSummaryPokemon(taskId, 1);
    }
    else if (JOY_NEW(DPAD_LEFT) || GetLRKeysPressed() == 1)
    {
        HandlePageChangeInput(taskId, -1);
    }
    else if (JOY_NEW(DPAD_RIGHT) || GetLRKeysPressed() == 2)
    {
        HandlePageChangeInput(taskId, 1);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (pssData.currPageIndex >= PSS_PAGE_BATTLE_MOVES)
            SwitchToMoveSelection(taskId);
        if (pssData.currPageIndex == PSS_PAGE_INFO)
            BeginCloseSummaryScreen(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        BeginCloseSummaryScreen(taskId);
    }
}

static void Task_HandleReplaceMoveInput(u8 taskId)
{
    if (gPaletteFade.active)
        return;

    if (JOY_NEW(DPAD_UP))
    {
        gTasks[taskId].data[0] = 4;
        ChangeSelectedMove(taskId, -1, &pssData.firstMoveIndex);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        gTasks[taskId].data[0] = 4;
        ChangeSelectedMove(taskId, 1, &pssData.firstMoveIndex);
    }
    else if (JOY_NEW(DPAD_LEFT) || GetLRKeysPressed() == 1)
    {
        if (pssData.currPageIndex == PSS_PAGE_CONTEST_MOVES && (pssData.firstMoveIndex != 4 || pssData.newMove != 0))
            Menu_EraseWindowRect(0, 14, 9, 18);
        HandlePageChangeInput(taskId, -1);
    }
    else if (JOY_NEW(DPAD_RIGHT) || GetLRKeysPressed() == 2)
    {
        if (pssData.currPageIndex != pssData.maxPageIndex)
        {
            if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES && (pssData.firstMoveIndex != 4 || pssData.newMove != 0))
                Menu_EraseWindowRect(0, 14, 9, 18);
            HandlePageChangeInput(taskId, 1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (CanReplaceMove(taskId) == TRUE || pssData.firstMoveIndex == 4)
        {
            pssData.secondMoveIndex = pssData.firstMoveIndex;
            gSpecialVar_0x8005 = pssData.secondMoveIndex;
            BeginCloseSummaryScreen(taskId);
        }
        else
        {
            PlaySE(SE_FAILURE);
            ShowCantForgetHMsWindow(taskId, pssData.firstMoveIndex);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        pssData.secondMoveIndex = 4;
        gSpecialVar_0x8005 = pssData.secondMoveIndex;
        BeginCloseSummaryScreen(taskId);
    }
}

static void Task_HandleInput_MoveSelect(u8 taskId)
{
    if (gPaletteFade.active)
        return;

    if (JOY_NEW(DPAD_UP))
    {
        gTasks[taskId].data[0] = 4;
        ChangeSelectedMove(taskId, -1, &pssData.firstMoveIndex);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        gTasks[taskId].data[0] = 4;
        ChangeSelectedMove(taskId, 1, &pssData.firstMoveIndex);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (pssData.firstMoveIndex != 4 && !pssData.lockMovesFlag)
        {
            if (!HasMoreThanOneMove(&pssData.currentMon))
            {
                PlaySE(SE_FAILURE);
            }
            else
            {
                PlaySE(SE_SELECT);

                pssData.secondMoveIndex = pssData.firstMoveIndex;
                SetMainMoveSelectorColor(1);
                CreateMoveSelectorSprites(19);

                gTasks[taskId].func = Task_HandleInput_MovePositionSwitch;
            }
        }
        else
        {
            PlaySE(SE_SELECT);
            CloseMoveSelectMode(taskId);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        CloseMoveSelectMode(taskId);
    }

}

static bool8 HasMoreThanOneMove(struct Pokemon *mon)
{
    u8 i;

    for (i = 1; i < 4; i++)
    {
        if (GetMonMove(mon, i))
            return TRUE;
    }

    return FALSE;
}

static void Task_HandleInput_MovePositionSwitch(u8 taskId)
{
    if (JOY_NEW(DPAD_UP))
    {
        gTasks[taskId].data[0] = 3;
        ChangeSelectedMove(taskId, -1, &pssData.secondMoveIndex);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        gTasks[taskId].data[0] = 3;
        ChangeSelectedMove(taskId, 1, &pssData.secondMoveIndex);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        ExitMovePositionSwitchMode(taskId, 1);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        ExitMovePositionSwitchMode(taskId, -1);
    }
}

static void SwapMonMoves(void)
{
    struct Pokemon *party = pssData.monList.partyMons;
    struct Pokemon *pkmn = &party[pssData.curMonIndex];
    u8 moveIndex1 = pssData.firstMoveIndex;
    u8 moveIndex2 = pssData.secondMoveIndex;

    u16 move1 = GetMonData(pkmn, MON_DATA_MOVE1 + moveIndex1);
    u16 move2 = GetMonData(pkmn, MON_DATA_MOVE1 + moveIndex2);
    u8 move1pp = GetMonData(pkmn, MON_DATA_PP1 + moveIndex1);
    u8 move2pp = GetMonData(pkmn, MON_DATA_PP1 + moveIndex2);
    u8 ppBonuses = GetMonData(pkmn, MON_DATA_PP_BONUSES);

    // Swap PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetMonData(pkmn, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetMonData(pkmn, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetMonData(pkmn, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetMonData(pkmn, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetMonData(pkmn, MON_DATA_PP_BONUSES, &ppBonuses);
}

static void SwapBoxMonMoves(void)
{
    struct BoxPokemon *boxMons = pssData.monList.boxMons;
    struct BoxPokemon *pkmn = &boxMons[pssData.curMonIndex];
    u8 moveIndex1 = pssData.firstMoveIndex;
    u8 moveIndex2 = pssData.secondMoveIndex;

    u16 move1 = GetBoxMonData(pkmn, MON_DATA_MOVE1 + moveIndex1);
    u16 move2 = GetBoxMonData(pkmn, MON_DATA_MOVE1 + moveIndex2);
    u8 move1pp = GetBoxMonData(pkmn, MON_DATA_PP1 + moveIndex1);
    u8 move2pp = GetBoxMonData(pkmn, MON_DATA_PP1 + moveIndex2);
    u8 ppBonuses = GetBoxMonData(pkmn, MON_DATA_PP_BONUSES);

    // Swap PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetBoxMonData(pkmn, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetBoxMonData(pkmn, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetBoxMonData(pkmn, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetBoxMonData(pkmn, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetBoxMonData(pkmn, MON_DATA_PP_BONUSES, &ppBonuses);
}

void Task_WaitForMoveSwapRefresh(u8 taskId)
{
    if (RefreshSummaryScreen())
    {
        pssData.loadState = 0;
        PrintMoveDetails(&pssData.currentMon, &pssData.firstMoveIndex);
        gTasks[taskId].func = Task_HandleInput_MoveSelect;
        SetTaskWaitForLinkRecv(taskId);
    }
}

static void ExitMovePositionSwitchMode(u8 taskId, s8 b)
{
    PlaySE(SE_SELECT);

    DestroyMoveSelectorSprites(19);
    SetMainMoveSelectorColor(0);

    if (b == 1)
    {
        if (pssData.firstMoveIndex != pssData.secondMoveIndex)
        {
            if (pssData.isBoxMon == FALSE)
                SwapMonMoves();
            else
                SwapBoxMonMoves();

            pssData.firstMoveIndex = pssData.secondMoveIndex;
            CopyMonToSummaryStruct(&pssData.currentMon);
            pssData.loadState = 1;

            gTasks[taskId].func = Task_WaitForMoveSwapRefresh;
            return;
        }
    }
    else
    {
        PrintMoveDetails(&pssData.currentMon, &pssData.firstMoveIndex);
    }

    gTasks[taskId].func = Task_HandleInput_MoveSelect;
    SetTaskWaitForLinkRecv(taskId);
}

static void ChangeSelectedMove(u8 taskId, s8 direction, u8 *moveIndexPtr)
{
    s8 i;
    s8 newMoveIndex;
    s8 var;
    bool8 bln;

    bln = TRUE;
    PlaySE(SE_SELECT);
    newMoveIndex = *moveIndexPtr;
    var = (s8)gTasks[taskId].data[0];
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        newMoveIndex += direction;
        if (newMoveIndex > var)
            newMoveIndex = 0;
        else if (newMoveIndex < 0)
            newMoveIndex = var;

        if (GetMonMove(&pssData.currentMon, newMoveIndex) != 0 || newMoveIndex == MAX_MON_MOVES)
            break;
    }

    if ((*moveIndexPtr == MAX_MON_MOVES
         && newMoveIndex != MAX_MON_MOVES
         && pssData.newMove == 0)
         || gTasks[taskId].data[13] == 1)
    {
        PositionPowerAccSlidingWindow(2, newMoveIndex);
        PositionAppealJamSlidingWindow(2, newMoveIndex);
        gTasks[taskId].data[13] = 0;
        bln = FALSE;
    }
    if (*moveIndexPtr != MAX_MON_MOVES
        && newMoveIndex == MAX_MON_MOVES
        && pssData.newMove == 0)
    {
        PositionPowerAccSlidingWindow(-2, newMoveIndex);
        PositionAppealJamSlidingWindow(-2, newMoveIndex);
    }

    *moveIndexPtr = newMoveIndex;
    // Get rid of the 'flicker' effect(while idle) when scrolling.
    if (moveIndexPtr == &pssData.firstMoveIndex)
        KeepMoveSelectorVisible(0);
    else
        KeepMoveSelectorVisible(1);

    if (bln)
        PrintMoveDetails(&pssData.currentMon, moveIndexPtr);
    SetTaskWaitForLinkRecv(taskId);
}

static void SwitchToMoveSelection(u8 taskId)
{
    PlaySE(SE_SELECT);

    pssData.firstMoveIndex = 0;
    PositionPowerAccSlidingWindow(2, 0);
    PositionAppealJamSlidingWindow(2, 0);

    if (!pssData.lockMovesFlag)
    {
        pssData.headerActionTextId = 5;
        PrintSummaryWindowHeaderText();
    }

    TilemapFiveMovesDisplay(0);
    PrintNewMoveDetailsOrCancelText(&pssData.currentMon);
    CreateMoveSelectorSprites(9);

    gTasks[taskId].func = Task_HandleInput_MoveSelect;
    SetTaskWaitForLinkRecv(taskId);
}

static void CloseMoveSelectMode(u8 taskId)
{
    if (pssData.firstMoveIndex != 4)
    {
        PositionPowerAccSlidingWindow(-2, 0);
        PositionAppealJamSlidingWindow(-2, 0);
    }

    DestroyMoveSelectorSprites(9);
    TilemapFiveMovesDisplay(1);

    Menu_EraseWindowRect(15, 12, 28, 13);
    Menu_EraseWindowRect(11, 15, 28, 18);

    pssData.headerActionTextId = 6;
    PrintSummaryWindowHeaderText();

    gTasks[taskId].func = Task_HandleInput;
}

static void Task_SetHandleReplaceMoveInput(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 5:
        CreateMoveSelectorSprites(9);
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].func = Task_HandleReplaceMoveInput;
        break;
    case 0:
        pssData.firstMoveIndex = 0;
        if (pssData.newMove != 0)
        {
            PositionPowerAccSlidingWindow(10, 0);
            PositionAppealJamSlidingWindow(10, pssData.firstMoveIndex);
        }

        TilemapFiveMovesDisplay(0);
        PrintNewMoveDetailsOrCancelText(&pssData.currentMon);
        // fall through
    default:
        gTasks[taskId].data[0]++;
        break;
    }
}

static void SetDefaultTilemaps(void)
{
    if (pssData.currPageIndex != PSS_PAGE_INFO)
    {
        DrawPagination();
        gBattle_BG1_X = 0x100;

        if (pssData.currPageIndex == PSS_PAGE_SKILLS)
            REG_BG1CNT = (REG_BG1CNT & 0xE0FF) + 0x800;

        if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
            REG_BG1CNT = (REG_BG1CNT & 0xE0FF) + 0xA00;

        if (pssData.currPageIndex == PSS_PAGE_CONTEST_MOVES)
            REG_BG1CNT = (REG_BG1CNT & 0xE0FF) + 0xC00;
    }
}

void PssScrollLeft(u8 taskId)
{
    u8 minus2;
    s16 *taskData = gTasks[taskId].data;

    switch (taskData[0])
    {
    case 0:
        if (pssData.bgDisplayOrder == 0)
        {
            if (pssData.currPageIndex != PSS_PAGE_INFO)
                gBattle_BG2_X = 0x100;

            if (pssData.currPageIndex == PSS_PAGE_SKILLS)
                REG_BG2CNT = (REG_BG2CNT & 0xE0FF) + 0x800;

            if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
                REG_BG2CNT = (REG_BG2CNT & 0xE0FF) + 0xA00;
        }
        else
        {
            if (pssData.currPageIndex != PSS_PAGE_INFO)
                gBattle_BG1_X = 0x100;

            if (pssData.currPageIndex == PSS_PAGE_SKILLS)
                REG_BG1CNT = (REG_BG1CNT & 0xE0FF) + 0x800;

            if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
                REG_BG1CNT = (REG_BG1CNT & 0xE0FF) + 0xA00;
        }

        taskData[0]++;
        break;
    case 1:
        if (pssData.bgDisplayOrder == 0)
        {
            int var2 = gBattle_BG1_X - 0x20;
            gBattle_BG1_X = var2;
            if (var2 << 16 == 0)
            {
                REG_BG1CNT = (REG_BG1CNT & 0xFFFC) + 2;
                REG_BG2CNT = (REG_BG2CNT & 0xFFFC) + 1;
                taskData[0]++;
            }
        }
        else
        {
            int var2 = gBattle_BG2_X - 0x20;
            gBattle_BG2_X = var2;
            if (var2 << 16 == 0)
            {
                REG_BG1CNT = (REG_BG1CNT & 0xFFFC) + 1;
                REG_BG2CNT = (REG_BG2CNT & 0xFFFC) + 2;
                taskData[0]++;
            }
        }
        break;
    case 2:
        pssData.headerTextId = pssData.currPageIndex + 1;
        minus2 = pssData.mode - 2;
        if (minus2 < 2)
        {
            pssData.headerActionTextId = 0;
            PrintNewMoveDetailsOrCancelText(&pssData.currentMon);
            PrintMoveDetails(&pssData.currentMon, &pssData.firstMoveIndex);
            PrintMoveDetailLabels(pssData.firstMoveIndex);
        }
        else
        {
            if (pssData.currPageIndex >= PSS_PAGE_BATTLE_MOVES && (!pssData.lockMovesFlag || pssData.mode == PSS_MODE_NO_MOVE_ORDER_EDIT))
                pssData.headerActionTextId = 6;
            else if (pssData.currPageIndex == PSS_PAGE_INFO)
                pssData.headerActionTextId = 7;
            else
                pssData.headerActionTextId = 0;
        }
        taskData[0]++;
        break;
    case 3:
        DrawPagination();
        PrintSummaryWindowHeaderText();
        taskData[0]++;
        break;
    case 4:
        sPageTextPrinters[pssData.currPageIndex](&pssData.currentMon);
        pssData.bgDisplayOrder ^= 1;
        taskData[0]++;
        break;
    case 5:
        if (Overworld_IsRecvQueueAtMax() != TRUE)
            gTasks[taskId].func = gTaskFuncForLinkRecv;
        break;
    }
}

void PssScrollRight(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    switch (taskData[0])
    {
    case 0:
        if (pssData.bgDisplayOrder == 0)
        {
            gBattle_BG2_X = 0;
            taskData[0]++;
        }
        else
        {
            gBattle_BG1_X = 0;
            taskData[0]++;
        }
        break;
    case 1:
        if (pssData.bgDisplayOrder == 0)
        {
            if (pssData.currPageIndex == PSS_PAGE_SKILLS)
                REG_BG2CNT = (REG_BG2CNT & 0xE0FC) + 0x801;

            if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
                REG_BG2CNT = (REG_BG2CNT & 0xE0FC) + 0xA01;

            if (pssData.currPageIndex == PSS_PAGE_CONTEST_MOVES)
                REG_BG2CNT = (REG_BG2CNT & 0xE0FC) + 0xC01;

            REG_BG1CNT = (REG_BG1CNT & 0xFFFC) + 2;
        }
        else
        {
            if (pssData.currPageIndex == PSS_PAGE_SKILLS)
                REG_BG1CNT = (REG_BG1CNT & 0xE0FC) + 0x801;

            if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
                REG_BG1CNT = (REG_BG1CNT & 0xE0FC) + 0xA01;

            if (pssData.currPageIndex == PSS_PAGE_CONTEST_MOVES)
                REG_BG1CNT = (REG_BG1CNT & 0xE0FC) + 0xC01;

            REG_BG2CNT = (REG_BG2CNT & 0xFFFC) + 2;
        }

        taskData[0]++;
        break;
    case 2:
        if (pssData.bgDisplayOrder == 0)
        {
            gBattle_BG2_X += 32;
            if (gBattle_BG2_X == 0x100)
                taskData[0]++;
        }
        else
        {
            gBattle_BG1_X += 32;
            if (gBattle_BG1_X == 0x100)
                taskData[0]++;
        }
        break;
    case 3:
        pssData.headerTextId = pssData.currPageIndex + 1;
        if (pssData.mode == 2 || pssData.mode == 3)
        {
            pssData.headerActionTextId = 0;
            PrintNewMoveDetailsOrCancelText(&pssData.currentMon);
            PrintMoveDetails(&pssData.currentMon, &pssData.firstMoveIndex);
            PrintMoveDetailLabels(pssData.firstMoveIndex);
        }
        else
        {
            if (pssData.currPageIndex >= PSS_PAGE_BATTLE_MOVES && (!pssData.lockMovesFlag || pssData.mode == PSS_MODE_NO_MOVE_ORDER_EDIT))
                pssData.headerActionTextId = 6;
            else if (pssData.currPageIndex == PSS_PAGE_INFO)
                pssData.headerActionTextId = 7;
            else
                pssData.headerActionTextId = 0;
        }
        taskData[0]++;
        break;
    case 4:
        DrawPagination();
        PrintSummaryWindowHeaderText();
        taskData[0]++;
        break;
    case 5:
        sPageTextPrinters[pssData.currPageIndex](&pssData.currentMon);
        pssData.bgDisplayOrder ^= 1;
        taskData[0]++;
        break;
    case 6:
        if (Overworld_IsRecvQueueAtMax() != TRUE)
            gTasks[taskId].func = gTaskFuncForLinkRecv;
        break;
    }
}

void ChangePage(u8 taskId, s8 direction)
{
    pssData.currPageIndex += direction;
    gTaskFuncForLinkRecv = gTasks[taskId].func;
    HidePageSpecificSprites();
    gTasks[taskId].data[0] = 0;

    if (direction == -1)
    {
        gTasks[taskId].func = PssScrollLeft;
    }
    else
    {
        gTasks[taskId].func = PssScrollRight;
        gTasks[taskId].func(taskId);
    }
}

static void HandlePageChangeInput(u8 taskId, s8 direction)
{
    if (!GetMonData(&pssData.currentMon, MON_DATA_IS_EGG))
    {
        if (direction == -1 && pssData.currPageIndex == pssData.minPageIndex)
            return;
        if (direction ==  1 && pssData.currPageIndex == pssData.maxPageIndex)
            return;
        if (FindTaskIdByFunc(Task_SlidePowerAccWindow) == 0xFF && FindTaskIdByFunc(Task_SlideAppealJamWindow) == 0xFF)
        {
            PlaySE(SE_SELECT);
            ChangePage(taskId, direction);
        }
    }
}

void ChangeSummaryPokemon(u8 taskId, s8 direction)
{
    s8 var3;

    if (pssData.isBoxMon == TRUE)
    {
        if (pssData.currPageIndex != PSS_PAGE_INFO)
        {
            if (direction == 1)
                direction = 0;
            else
                direction = 1;
        }
        else
        {
            if (direction == 1)
                direction = 2;
            else
                direction = 3;
        }

        var3 = AdvanceStorageMonIndex(pssData.monList.boxMons, pssData.curMonIndex, pssData.maxMonIndex, direction);
    }
    else
    {
        if (MenuHelpers_IsLinkActive() == TRUE && IsLinkDoubleBattle() == TRUE)
        {
            var3 = AdvanceMultiBattleMonIndex(direction);
        }
        else
        {
            var3 = AdvanceMonIndex(direction);
        }
    }

    if (var3 != -1)
    {
        PlaySE(SE_SELECT);
        if (GetMonStatusAndPokerus(&pssData.currentMon))
        {
            PositionStatusSlidingWindow(-2);
        }

        pssData.curMonIndex = var3;
        pssData.savedTaskFunc = gTasks[taskId].func;
        gTasks[taskId].func = Task_ChangeSummaryMon;
    }
}

s8 AdvanceMonIndex(s8 a)
{
    struct Pokemon *mons = pssData.monList.partyMons;
    s8 r6 = 0;
    
    if (pssData.currPageIndex == PSS_PAGE_INFO)
    {
        if (a == -1 && pssData.curMonIndex == 0)
            return -1;
        if (a == 1 && pssData.curMonIndex >= pssData.maxMonIndex)
            return -1;
        return pssData.curMonIndex + a;
    }

    // unlike Emerald, the do while needs to be inversed in order to match
    while (1)
    {
        r6 += a;
        if (pssData.curMonIndex + r6 < 0 || pssData.curMonIndex + r6 > pssData.maxMonIndex)
            return -1;
        if (GetMonData(&mons[pssData.curMonIndex + r6], MON_DATA_IS_EGG) == 0)
            return pssData.curMonIndex + r6;
    }
}

bool8 IsValidToViewInMulti(struct Pokemon *mon)
{
    if (GetMonData(mon, MON_DATA_SPECIES))
    {
        if (pssData.currPageIndex != PSS_PAGE_INFO || !GetMonData(mon, MON_DATA_IS_EGG))
            return TRUE;
    }
    return FALSE;
}

s8 GetNextMultiBattleMonIndex(u8 partyIndex)
{
    while (1)
    {
        partyIndex++;
        if (partyIndex == PARTY_SIZE)
            return -1;
        if (IsValidToViewInMulti(&gPlayerParty[sMultiBattleOrder[partyIndex]]) == TRUE)
            return sMultiBattleOrder[partyIndex];
    }
}

s8 GetPreviousMultiBattleMonIndex(u8 partyIndex)
{
    while (1)
    {
        if (partyIndex == 0)
            return -1;
        partyIndex--;
        if (IsValidToViewInMulti(&gPlayerParty[sMultiBattleOrder[partyIndex]]) == TRUE)
            return sMultiBattleOrder[partyIndex];
    }
}

s8 AdvanceMultiBattleMonIndex(s8 direction)
{
    u8 i;
    u8 curMonIndex = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sMultiBattleOrder[i] == pssData.curMonIndex)
        {
            curMonIndex = i;
            break;
        }
    }

    if ((direction == -1 && curMonIndex == 0) || (direction == 1 && curMonIndex == 5))
        return -1;
    if (direction != 1)
        return GetPreviousMultiBattleMonIndex(curMonIndex);
    else if (curMonIndex != 5)
        return GetNextMultiBattleMonIndex(curMonIndex);

    return -1;
}

void Task_ChangeSummaryMon(u8 taskId)
{
    switch (gMain.state)
    {
    case 0:
        StopCryAndClearCrySongs();
        gMain.state++;
        break;
    case 1:
        DestroySpriteAndFreeResources(&gSprites[pssData.monSpriteId]);
        gMain.state++;
        break;
    case 2:
        DestroySpriteAndFreeResources(&gSprites[pssData.ballSpriteId]);
        gMain.state++;
        break;
    case 3:
        pssData.loadState = 0;
        pssData.firstMoveIndex = 0;
        gMain.state++;
        break;
    case 4:
        CopyMonToSummaryStruct(&pssData.currentMon);
        if (GetMonStatusAndPokerus(&pssData.currentMon))
            PositionStatusSlidingWindow(2);
        DrawPokerusCuredSymbol(&pssData.currentMon);
        gMain.state++;
        break;
    case 5:
        if ((pssData.monSpriteId = LoadMonGfxAndSprite(&pssData.currentMon, &pssData.loadState)) != 0xFF)
        {
            pssData.loadState = 0;
            if (GetMonData(&pssData.currentMon, MON_DATA_IS_EGG))
                gBattle_BG3_X = 256;
            else
                gBattle_BG3_X = 0;
            gMain.state++;
        }
        break;
    case 6:
        RemoveAndCreateMonMarkingsSprite(&pssData.currentMon);
        gMain.state++;
        break;
    case 7:
        CreateCaughtBallSprite(&pssData.currentMon);
        gMain.state++;
        break;
    case 8:
        if (RefreshSummaryScreen())
        {
            pssData.loadState = 0;
            gMain.state++;
        }
        break;
    default:
        if (Overworld_IsRecvQueueAtMax() != TRUE)
        {
            gMain.state = 0;
            gTasks[taskId].func = pssData.savedTaskFunc;
        }
        break;
    }
}

static bool8 RefreshSummaryScreen(void)
{
    if (pssData.loadState == 0)
    {
        PrintMonInfo(&pssData.currentMon);
        pssData.loadState++;
        return FALSE;
    }
    else
    {
        sPageRefreshFuncs[pssData.currPageIndex](&pssData.currentMon);
        return TRUE;
    }
}

static void RefreshInfoPage(struct Pokemon *mon)
{
    ClearInfoPageText();
    SummaryScreen_PrintPokemonInfo(mon);
}

static void RefreshSkillsPage(struct Pokemon *mon)
{
    ClearSkillsPageText();
    SummaryScreen_PrintPokemonSkills(mon);
}

static void RefreshMovesPage(struct Pokemon *mon)
{
    ClearMoveNamesAndPP();
    PrintMoveNamesAndPP(mon);
}

static void CopyMonToSummaryStruct(struct Pokemon *mon)
{
    if (!pssData.isBoxMon)
    {
        struct Pokemon *mons = pssData.monList.partyMons;
        *mon = mons[pssData.curMonIndex];
    }
    else
    {
        struct BoxPokemon *mons = pssData.monList.boxMons;
        ExpandBoxMon(&mons[pssData.curMonIndex], mon);
    }
}

static u8 LoadMonGfxAndSprite(struct Pokemon *mon, u8 *state)
{
    u16 species;
    u32 personality;
    u32 otId;
    const struct CompressedSpritePalette *palette;

    switch (*state)
    {
    default:
        return CreateMonSprite(mon);
    case 0:
        species = GetMonData(mon, MON_DATA_SPECIES2);
        personality = GetMonData(mon, MON_DATA_PERSONALITY);

        HandleLoadSpecialPokePic(
            &gMonFrontPicTable[species],
            gMonFrontPicCoords[species].coords,
            gMonFrontPicCoords[species].y_offset,
            (void *)EWRAM,
            gMonSpriteGfx_Sprite_ptr[1],
            species,
            personality);
        *state += 1;
        return 0xFF;
    case 1:
        species = GetMonData(mon, MON_DATA_SPECIES2);
        personality = GetMonData(mon, MON_DATA_PERSONALITY);
        otId = GetMonData(mon, MON_DATA_OT_ID);

        palette = GetMonSpritePalStructFromOtIdPersonality(species, otId, personality);
        LoadCompressedSpritePalette(palette);
        SetMultiuseSpriteTemplateToPokemon(palette->tag, 1);
        *state += 1;
        return 0xFF;
    }
}

static u16 GetMonMove(struct Pokemon *mon, u8 moveId)
{
    switch (moveId)
    {
    case 0:
        return GetMonData(mon, MON_DATA_MOVE1);
    case 1:
        return GetMonData(mon, MON_DATA_MOVE2);
    case 2:
        return GetMonData(mon, MON_DATA_MOVE3);
    default:
        return GetMonData(mon, MON_DATA_MOVE4);
    }
}

static u16 GetMonMovePP(struct Pokemon *mon, u8 moveId)
{
    switch (moveId)
    {
    case 0:
        return GetMonData(mon, MON_DATA_PP1);
    case 1:
        return GetMonData(mon, MON_DATA_PP2);
    case 2:
        return GetMonData(mon, MON_DATA_PP3);
    default:
        return GetMonData(mon, MON_DATA_PP4);
    }
}

static bool8 CanReplaceMove(u8 taskId)
{
    struct Pokemon mon;
    u16 move;

    CopyMonToSummaryStruct(&mon);
    move = GetMonMove(&mon, pssData.firstMoveIndex);
    if (IsHMMove(move) == TRUE && pssData.mode != PSS_MODE_MOVE_DELETER)
        return FALSE;
    return TRUE;
}

void Task_HandleInputCantForgetHMsMoves(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (taskData[14] < 4)
    {
        taskData[14]++;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        gTasks[taskId].func = Task_HandleReplaceMoveInput;
        taskData[0] = 4;
        taskData[13] = 1;
        pssData.firstMoveIndex = taskData[15];
        ChangeSelectedMove(taskId, -1, &pssData.firstMoveIndex);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        gTasks[taskId].func = Task_HandleReplaceMoveInput;
        taskData[0] = 4;
        taskData[13] = 1;
        pssData.firstMoveIndex = taskData[15];
        ChangeSelectedMove(taskId, 1, &pssData.firstMoveIndex);
    }
    else if (JOY_NEW(DPAD_LEFT) || GetLRKeysPressed() == 1)
    {
        if (pssData.currPageIndex != PSS_PAGE_BATTLE_MOVES)
        {
            if (pssData.currPageIndex == PSS_PAGE_CONTEST_MOVES && (pssData.firstMoveIndex != 4 || pssData.newMove != 0))
                Menu_EraseWindowRect(0, 14, 9, 18);

            gTasks[taskId].func = Task_HandleReplaceMoveInput;

            HandlePageChangeInput(taskId, -1);
            PositionPowerAccSlidingWindow(1, taskData[15]);
            PositionAppealJamSlidingWindow(1, taskData[15]);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT) || GetLRKeysPressed() == 2)
    {
        if (pssData.currPageIndex != pssData.maxPageIndex)
        {
            if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES && (pssData.firstMoveIndex != 4 || pssData.newMove != 0))
                Menu_EraseWindowRect(0, 14, 9, 18);

            gTasks[taskId].func = Task_HandleReplaceMoveInput;

            HandlePageChangeInput(taskId, 1);
            PositionPowerAccSlidingWindow(1, taskData[15]);
            PositionAppealJamSlidingWindow(1, taskData[15]);
        }
    }
    else if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PositionPowerAccSlidingWindow(2, taskData[15]);
        PositionAppealJamSlidingWindow(2, taskData[15]);

        gTasks[taskId].func = Task_HandleReplaceMoveInput;
    }
}

static void ShowCantForgetHMsWindow(u8 taskId, u8 moveIndex)
{
    s16 *taskData = gTasks[taskId].data;
    taskData[14] = 0;
    taskData[15] = moveIndex;

    PositionPowerAccSlidingWindow(-2, 4);
    PositionAppealJamSlidingWindow(-2, 4);
    Menu_EraseWindowRect(11, 15, 28, 18);
    Menu_PrintText(gOtherText_CantForgetHMs, 11, 15);

    gTasks[taskId].func = Task_HandleInputCantForgetHMsMoves;
}

u8 GetMoveSlotToReplace(void)
{
    return pssData.secondMoveIndex;
}

static void SummaryScreen_PlaceTextTile(u8 tile, u8 palette, u8 x, u8 y)
{
    u16 *vramAddr = (u16 *)(VRAM + 0xF000) + x + y * 32;
    vramAddr[0] = (palette << 12) + 0x200 + tile * 2 + 0x80;
    vramAddr[32] = (palette << 12) + 0x200 + tile * 2 + 0x81;
}

static void SummaryScreen_PlaceTextTile_White(u8 tile, u8 x, u8 y)
{
    SummaryScreen_PlaceTextTile(tile, 15, x, y);
}

bool8 SetMonPicBackgroundPalette(struct Pokemon *mon)
{
    if (!IsShiny(mon))
    {
        LoadPalette(sDexNumberNormalPalette, 4, 2);
        return FALSE;
    }
    else
    {
        LoadPalette(sDexNumberShinyPalette, 4, 2);
        return TRUE;
    }
}

static void PrintMonInfo(struct Pokemon *mon)
{
    bool8 shiny;
    u16 dexNum;
    u8 *buffer;

    if (GetMonData(mon, MON_DATA_IS_EGG))
    {
        Menu_EraseWindowRect(1, 2, 4, 3);
        Menu_EraseWindowRect(3, 16, 9, 17);
        Menu_EraseWindowRect(0, 12, 11, 15);
        GetMonNickname(mon, gStringVar1);
        SummaryScreen_PrintColoredText(gStringVar1, 13, 3, 16);
        LoadPalette(sDexNumberNormalPalette, 4, 2);
    }
    else
    {
        shiny = SetMonPicBackgroundPalette(mon);
        dexNum = SpeciesToPokedexNum(GetMonData(mon, MON_DATA_SPECIES));
        if (dexNum != 0xFFFF)
        {
            if (!shiny)
            {
                SummaryScreen_PlaceTextTile_White(2, 1, 2);
                SummaryScreen_PrintColoredIntPixelCoords(dexNum, 13, 3, 2, 17, 16, 1);
            }
            else
            {
                SummaryScreen_PlaceTextTile(2, 8, 1, 2);
                SummaryScreen_PrintColoredIntPixelCoords(dexNum, 8, 3, 2, 17, 16, 1);
            }
        }
        else
        {
            Menu_EraseWindowRect(1, 2, 4, 3);
        }

        buffer = gStringVar1;
        buffer = SummaryScreen_SetTextColor(buffer, 13);
        buffer = GetMonNickname(mon, buffer);
        buffer[0] = EXT_CTRL_CODE_BEGIN;
        buffer[1] = 0x13;
        buffer[2] = 0x3C;
        buffer[3] = EOS;
        Menu_PrintText(gStringVar1, 1, 12);

        PrintMonSpeciesLevelAndGender(mon);
    }
}

static void HidePageSpecificSprites(void)
{
    u8 i;

    for (i = 0; i < 28; i++)
        SetSpriteInvisibility(i, 1);

    Menu_EraseWindowRect(11, 4, 29, 18);
}

static void SummaryScreen_PrintPokemonInfoLabels(void)
{
    Menu_PrintText(gOtherText_Type2, 11, 6);
    SummaryScreen_PlaceTextTile_White(0, 22, 4);
    SummaryScreen_PlaceTextTile_White(2, 23, 4);
}

static void SummaryScreen_PrintPokemonInfo(struct Pokemon *mon)
{
    u8 i;
    u8 *buffer;
    u16 friendship;
    u8 language;
    u16 species;
    u8 ability;

    for (i = 0; i < 5; i++)
        SetSpriteInvisibility(i, 1);

    Menu_EraseWindowRect(11, 9, 28, 12);
    if (GetMonData(mon, MON_DATA_IS_EGG))
    {
        buffer = gStringVar1;
        buffer = SummaryScreen_SetTextColor(buffer, 13);
        buffer = StringCopy(buffer, gOtherText_OriginalTrainer);
        buffer = StringCopy(buffer, gText_FiveMarks);
        buffer[0] = EXT_CTRL_CODE_BEGIN;
        buffer[1] = 0x13;
        buffer[2] = 0x4E;
        buffer[3] = EOS;
        Menu_PrintText(gStringVar1, 11, 4);

        SummaryScreen_PrintColoredTextPixelCoords(gText_FiveMarks, 13, 193, 32, 1);
        SetTypeSpritePosAndPal(TYPE_MYSTERY, 120, 48, 0);

        friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
        if (friendship <= 5)
            Menu_PrintText(gOtherText_EggAbout, 11, 9);
        else if (friendship <= 10)
            Menu_PrintText(gOtherText_EggSoon, 11, 9);
        else if (friendship <= 40)
            Menu_PrintText(gOtherText_EggSomeTime, 11, 9);
        else
            Menu_PrintText(gOtherText_EggLongTime, 11, 9);

        PokemonSummaryScreen_PrintEggTrainerMemo(mon, 11, 14);
    }
    else
    {
        GetMonData(mon, MON_DATA_OT_NAME, gStringVar2);
        language = GetMonData(mon, MON_DATA_LANGUAGE);
        ConvertInternationalString(gStringVar2, language);

        buffer = gStringVar1;
        buffer = SummaryScreen_SetTextColor(buffer, 13);
        buffer = StringCopy(buffer, gOtherText_OriginalTrainer);

        if (GetMonData(mon, MON_DATA_OT_GENDER) == MALE)
            buffer = SummaryScreen_SetTextColor(buffer, 9);
        else
            buffer = SummaryScreen_SetTextColor(buffer, 10);

        buffer = StringCopy(buffer, gStringVar2);
        buffer[0] = EXT_CTRL_CODE_BEGIN;
        buffer[1] = 0x13;
        buffer[2] = 0x4E;
        buffer[3] = EOS;
        Menu_PrintText(gStringVar1, 11, 4);

        SummaryScreen_PrintColoredIntPixelCoords(GetMonData(mon, MON_DATA_OT_ID) & 0xFFFF, 13, 5, 2, 193, 32, 1);

        species = GetMonData(mon, MON_DATA_SPECIES);
        SetTypeSpritePosAndPal(gBaseStats[species].type1, 120, 48, 0);
        if (gBaseStats[species].type1 != gBaseStats[species].type2)
            SetTypeSpritePosAndPal(gBaseStats[species].type2, 160, 48, 1);

        ability = GetAbilityBySpecies(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_ALT_ABILITY));
        SummaryScreen_PrintColoredText(gAbilityNames[ability], 13, 11, 9);
        Menu_PrintText(gAbilityDescriptions[ability], 11, 11);

        PokemonSummaryScreen_PrintTrainerMemo(mon, 11, 14);
    }
}

static void PrintInfoPageText(struct Pokemon *mon)
{
    SummaryScreen_PrintPokemonInfoLabels();
    SummaryScreen_PrintPokemonInfo(mon);
}

static void ClearInfoPageText(void)
{
    Menu_EraseWindowRect(14, 4, 18, 5);
    Menu_EraseWindowRect(25, 4, 30, 5);
    Menu_EraseWindowRect(11, 9, 28, 12);
    Menu_EraseWindowRect(11, 14, 28, 17);
}

static void SummaryScreen_PrintPokemonSkillsLabels(void)
{
    SummaryScreen_PrintColoredText(gOtherText_ExpPoints, 13, 11, 14);
    SummaryScreen_PrintColoredText(gOtherText_NextLv, 13, 11, 16);
    Menu_PrintText(gOtherText_Terminator18, 21, 16);

    SummaryScreen_PrintColoredTextCentered(gOtherText_HP, 13, 11, 7, 42);
    SummaryScreen_PrintColoredTextCentered(gOtherText_Attack, 13, 11, 9, 42);
    SummaryScreen_PrintColoredTextCentered(gOtherText_Defense, 13, 11, 11, 42);
    SummaryScreen_PrintColoredTextCentered(gOtherText_SpAtk, 13, 22, 7, 36);
    SummaryScreen_PrintColoredTextCentered(gOtherText_SpDef, 13, 22, 9, 36);
    SummaryScreen_PrintColoredTextCentered(gOtherText_Speed, 13, 22, 11, 36);
}

static void SummaryScreen_PrintPokemonSkills(struct Pokemon *mon)
{
    u8 i;
    u16 heldItem;
    u8 *buffer;

    for (i = 0; i < 5; i++)
        SetSpriteInvisibility(i, 1);

    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    PrintHeldItemName(heldItem, 11, 4);
    PrintRibbonCount(mon);

    buffer = gStringVar1;

    ConvertIntToDecimalStringSimple(buffer, GetMonData(mon, MON_DATA_EXP));
    MenuPrint_RightAligned(buffer, 29, 14);
    DrawExperienceProgressBar(mon, 23, 16);

    ConvertIntToDecimalStringSimple(buffer, GetMonData(mon, MON_DATA_ATK));
    MenuPrint_Centered(buffer, 16, 9, 50);

    ConvertIntToDecimalStringSimple(buffer, GetMonData(mon, MON_DATA_DEF));
    MenuPrint_Centered(buffer, 16, 11, 50);

    ConvertIntToDecimalStringSimple(buffer, GetMonData(mon, MON_DATA_SPATK));
    MenuPrint_Centered(buffer, 27, 7, 18);

    ConvertIntToDecimalStringSimple(buffer, GetMonData(mon, MON_DATA_SPDEF));
    MenuPrint_Centered(buffer, 27, 9, 18);

    ConvertIntToDecimalStringSimple(buffer, GetMonData(mon, MON_DATA_SPEED));
    MenuPrint_Centered(buffer, 27, 11, 18);

    buffer = AlignInt1InMenuWindow(buffer, GetMonData(mon, MON_DATA_HP), 24, 1);
    *buffer++ = CHAR_SLASH;
    buffer = AlignInt1InMenuWindow(buffer, GetMonData(mon, MON_DATA_MAX_HP), 48, 1);

    Menu_PrintTextPixelCoords(gStringVar1, 126, 56, 1);
}

static void PrintSkillsPageText(struct Pokemon *mon)
{
    SummaryScreen_PrintPokemonSkillsLabels();
    SummaryScreen_PrintPokemonSkills(mon);
}

static void ClearSkillsPageText(void)
{
    Menu_EraseWindowRect(11, 4, 19, 5);
    Menu_EraseWindowRect(16, 7, 21, 8);
    Menu_EraseWindowRect(17, 9, 21, 12);
    Menu_EraseWindowRect(27, 7, 29, 12);
    Menu_EraseWindowRect(22, 14, 28, 15);
    Menu_EraseWindowRect(23, 16, 28, 17);
}

static void PrintMoveDetailLabels(u8 a)
{
    if (pssData.newMove != 0 || a != 4)
    {
        if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
        {
            SummaryScreen_PrintColoredText(gOtherText_Power2, 13, 1, 15);
            SummaryScreen_PrintColoredText(gOtherText_Accuracy2, 13, 1, 17);
        }
        else
        {
            SummaryScreen_PrintColoredText(gOtherText_Appeal2, 13, 1, 15);
            SummaryScreen_PrintColoredText(gOtherText_Jam2, 13, 1, 17);
        }
    }
}

static void PrintMoveNamesAndPP(struct Pokemon *mon)
{
    u8 i;
    u16 move;
    u16 curPP;
    u8 ppBonuses;
    u8 maxPP;
    u8 *buffer;

    for (i = 0; i < 4; i++)
    {
        move = GetMonMove(mon, i);
        curPP = GetMonMovePP(mon, i);

        if (move == 0)
        {
            SetSpriteInvisibility(i, 1);
            SummaryScreen_PrintColoredText(gOtherText_OneDash, 13, 15, (2 * i) + 4);
            Menu_PrintText(gOtherText_TwoDashes, 26, (2 * i) + 4);
        }
        else
        {
            if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
                SetTypeSpritePosAndPal(gBattleMoves[move].type, 87, ((2 * i) + 4) * 8, i);
            else
                SetTypeSpritePosAndPal(gContestMoves[move].contestCategory + 18, 87, ((2 * i) + 4) * 8, i);

            SummaryScreen_PrintColoredText(gMoveNames[move], 13, 15, (2 * i) + 4);
            SummaryScreen_PlaceTextTile_White(1, 24, (2 * i) + 4);

            ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
            maxPP = CalculatePPWithBonus(move, ppBonuses, i);

            buffer = gStringVar1;
            buffer = AlignInt1InMenuWindow(buffer, curPP, 14, 1);
            *buffer++ = CHAR_SLASH;
            AlignInt1InMenuWindow(buffer, maxPP, 32, 1);
            Menu_PrintText(gStringVar1, 25, (2 * i) + 4);
        }
    }
}

static void PrintNewMoveDetailsOrCancelText(struct Pokemon *mon)
{
    u8 *buffer;
    u16 move;
    u8 pp;

    if (pssData.newMove == 0)
    {
        SummaryScreen_PrintColoredText(gOtherText_CancelNoTerminator, 13, 15, 12);
        return;
    }

    move = pssData.newMove;

    if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
        SetTypeSpritePosAndPal(gBattleMoves[move].type, 87, 96, 4);
    else
        SetTypeSpritePosAndPal(gContestMoves[move].contestCategory + 18, 87, 96, 4);

    if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
        SummaryScreen_PrintColoredText(gMoveNames[move], 10, 15, 12);
    else
        SummaryScreen_PrintColoredText(gMoveNames[move], 9, 15, 12);

    SummaryScreen_PlaceTextTile_White(1, 24, 12);

    buffer = gStringVar1;
    pp = gBattleMoves[move].pp;
    buffer = AlignInt1InMenuWindow(buffer, pp, 14, 1);
    *buffer++ = CHAR_SLASH;
    buffer = AlignInt1InMenuWindow(buffer, pp, 32, 1);
    Menu_PrintText(gStringVar1, 25, 12);
}

static void ClearMoveNamesAndPP(void)
{
    u8 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
        Menu_EraseWindowRect(15, (i * 2) + 4, 28, (i * 2) + 5);
}

u16 GetSelectedMove(struct Pokemon *mon, u8 firstMoveIndex)
{
    u16 move;

    if (firstMoveIndex != MAX_MON_MOVES)
    {
        move = GetMonMove(mon, firstMoveIndex);
    }
    else
    {
        if (pssData.newMove != 0)
            move = pssData.newMove;
        else
            move = 0xFFFF;
    }

    return move;
}

void PrintSelectedMoveStats(struct Pokemon *mon, u8 *firstMoveIndex)
{
    u16 move = GetSelectedMove(mon, *firstMoveIndex);

    if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
    {
        PrintMovePowerAndAccuracy(move);
        DrawContestMoveHearts(0xFFFF);
    }
    else
    {
        DrawContestMoveHearts(move);
    }
}

static void PrintMoveDetails(struct Pokemon *mon, u8 *firstMoveIndex)
{
    u16 move = GetSelectedMove(mon, *firstMoveIndex);
    Menu_EraseWindowRect(11, 15, 28, 18);

    if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
        PrintBattleMoveDescription(move);
    else
        PrintContestMoveDescription(move);

    PrintSelectedMoveStats(mon, firstMoveIndex);
}

static void PrintBattleMoveDescription(u16 move)
{
    if (move != 0xFFFF)
        Menu_PrintText(gMoveDescriptions[move - 1], 11, 15);
}

static void PrintContestMoveDescription(u16 move)
{
    if (move != 0xFFFF)
        Menu_PrintText(gContestEffectStrings[gContestMoves[move].effect], 11, 15);
}

static void PrintMovePowerAndAccuracy(u16 move)
{
    u8 *buffer;

    if (move != 0xFFFF)
    {
        if (gBattleMoves[move].power <= 1)
        {
            buffer = gStringVar1;
            buffer = AlignStringInMenuWindow(buffer, gOtherText_ThreeDashes2, 21, 1);
            Menu_PrintText(gStringVar1, 7, 15);
        }
        else
        {
            buffer = gStringVar1;
            buffer = AlignInt1InMenuWindow(buffer, gBattleMoves[move].power, 21, 1);
            Menu_PrintText(gStringVar1, 7, 15);
        }

        if (gBattleMoves[move].accuracy == 0)
        {
            buffer = gStringVar1;
            buffer = AlignStringInMenuWindow(buffer, gOtherText_ThreeDashes2, 21, 1);
            Menu_PrintText(gStringVar1, 7, 17);
        }
        else
        {
            buffer = gStringVar1;
            buffer = AlignInt1InMenuWindow(buffer, gBattleMoves[move].accuracy, 21, 1);
            Menu_PrintText(gStringVar1, 7, 17);
        }
    }
}

void DrawContestMoveHearts(u16 move)
{
    u8 i;
    u8 effectValue;
    u16 *tilemap = (u16 *)(VRAM + 0x6800);
    u16 tile;

    // The function didn't change much between Ruby and Emerald, but unlike
    // Emerald, shifts are oddly an absolute requirement to match.
    if (move != 0xFFFF)
    {
        effectValue = gContestEffects[gContestMoves[move].effect].appeal;
        if (effectValue != 0xFF)
            effectValue /= 10;

        for (i = 0; i < 8; i++)
        {
            if (effectValue != 0xFF && i < effectValue)
                tile = 0x103A;
            else
                tile = 0x1039;

            // The offset number has to go first, or else, r1 gets swapped with r0.
            tilemap[0x1E6 + ((i >> 2) << 5) + (i & 3)] = tile;
        }
    }

    if (move != 0xFFFF)
    {
        effectValue = gContestEffects[gContestMoves[move].effect].jam;
        if (effectValue != 0xFF)
            effectValue /= 10;

        for (i = 0; i < 8; i++)
        {
            if (effectValue != 0xFF && i < effectValue)
                tile = 0x103C;
            else
                tile = 0x103D;

            // match order above
            tilemap[0x226 + ((i >> 2) << 5) + (i & 3)] = tile;
        }
    }
}

bool8 DoesMonOTMatchOwner(struct Pokemon *mon)
{
    u32 trainerId;

    if (pssData.monList.partyMons == gEnemyParty)
    {
        u8 enemyId = GetMultiplayerId() ^ 1;
        trainerId = gLinkPlayers[enemyId].trainerId & 0xFFFF;
        StringCopy(gStringVar1, gLinkPlayers[enemyId].name);
        Text_StripExtCtrlCodes(gStringVar1);
    }
    else
    {
        trainerId = GetPlayerIDAsU32() & 0xFFFF;
        StringCopy(gStringVar1, gSaveBlock2.playerName);
    }

    if (trainerId != (GetMonData(mon, MON_DATA_OT_ID) & 0xFFFF))
        return FALSE;

    GetMonData(mon, MON_DATA_OT_NAME, gStringVar2);

    if (!StringCompareWithoutExtCtrlCodes(gStringVar1, gStringVar2))
        return TRUE;

    return FALSE;
}

static void PokemonSummaryScreen_PrintEggTrainerMemo(struct Pokemon *mon, u8 left, u8 top)
{
    u8 locationMet;
    u8 gameMet = GetMonData(mon, MON_DATA_MET_GAME);

    if (!(gameMet == VERSION_RUBY || gameMet == VERSION_SAPPHIRE || gameMet == VERSION_EMERALD))
    {
        Menu_PrintText(gOtherText_EggObtainedInTrade, left, top);
        return;
    }

    locationMet = GetMonData(mon, MON_DATA_MET_LOCATION);

    if (locationMet == METLOC_FATEFUL_ENCOUNTER)
    {
        // Eggs received from Pokemon Box.
        Menu_PrintText(gOtherText_EggNicePlace, left, top);
        return;
    }

    if (!DoesMonOTMatchOwner(mon))
    {
        Menu_PrintText(gOtherText_EggObtainedInTrade, left, top);
        return;
    }

    asm(""); // needed to match for some reason

    if (locationMet == METLOC_SPECIAL_EGG)
    {
        Menu_PrintText(gOtherText_EggHotSprings, left, top);
        return;
    }

    Menu_PrintText(gOtherText_EggDayCare, left, top);
}

static void PokemonSummaryScreen_PrintTrainerMemo(struct Pokemon *mon, u8 left, u8 top)
{
    u8 locationMet;
    u8 gameMet;
    u8 *ptr = gStringVar4;
    u8 nature = GetNature(mon);

#if ENGLISH
    ptr = SummaryScreen_CopyColoredString(ptr, gNatureNames[nature], 14);
    if (nature != NATURE_BOLD && nature != NATURE_GENTLE)
        ptr = StringCopy(ptr, gOtherText_Terminator4);
    ptr = StringCopy(ptr, gOtherText_Nature);
#elif GERMAN
    ptr = StringCopy(gStringVar4, gOtherText_Nature);
    ptr = SummaryScreen_CopyColoredString(ptr, gNatureNames[nature], 14);
    ptr = StringCopy(ptr, gOtherText_Terminator4);
#endif

    if (DoesMonOTMatchOwner(mon) == TRUE)
    {
        locationMet = GetMonData(mon, MON_DATA_MET_LOCATION);

        if (GetMonData(mon, MON_DATA_MET_LEVEL) == 0)
        {
            ptr = PokemonSummaryScreen_CopyPokemonLevel(ptr, EGG_HATCH_LEVEL);
            *ptr = CHAR_NEWLINE;
            ptr++;

            GetMapNameHandleEvilTeamHideout(gStringVar1, locationMet);
            ptr = SummaryScreen_CopyColoredString(ptr, gStringVar1, 14);
            StringCopy(ptr, gOtherText_Egg2);
        }
        else if (locationMet >= 88)
        {
            *ptr = CHAR_NEWLINE;
            ptr++;

            StringCopy(ptr, gOtherText_ObtainedInTrade);
        }
        else
        {
            u8 levelMet = GetMonData(mon, MON_DATA_MET_LEVEL);

            ptr = PokemonSummaryScreen_CopyPokemonLevel(ptr, levelMet);
            *ptr = CHAR_NEWLINE;
            ptr++;

            GetMapNameHandleEvilTeamHideout(gStringVar1, locationMet);
            ptr = SummaryScreen_CopyColoredString(ptr, gStringVar1, 14);
            StringCopy(ptr, gOtherText_Met);
        }
    }
    else
    {
        gameMet = GetMonData(mon, MON_DATA_MET_GAME);

        if (!(gameMet == VERSION_RUBY || gameMet == VERSION_SAPPHIRE || gameMet == VERSION_EMERALD))
        {
            *ptr = CHAR_NEWLINE;
            ptr++;

            StringCopy(ptr, gOtherText_ObtainedInTrade);
        }
        else
        {
            locationMet = GetMonData(mon, MON_DATA_MET_LOCATION);
            if (locationMet == METLOC_FATEFUL_ENCOUNTER)
            {
                u8 levelMet = GetMonData(mon, MON_DATA_MET_LEVEL);

                ptr = PokemonSummaryScreen_CopyPokemonLevel(ptr, levelMet);
                *ptr = CHAR_NEWLINE;
                ptr++;

                StringCopy(ptr, gOtherText_FatefulEncounter);
            }
            else if (locationMet >= MAPSEC_NONE)
            {
                *ptr = CHAR_NEWLINE;
                ptr++;

                StringCopy(ptr, gOtherText_ObtainedInTrade);
            }
            else
            {
                u8 levelMet = GetMonData(mon, MON_DATA_MET_LEVEL);

                ptr = PokemonSummaryScreen_CopyPokemonLevel(ptr, levelMet);
                *ptr = CHAR_NEWLINE;
                ptr++;

                GetMapNameHandleEvilTeamHideout(gStringVar1, locationMet);
                ptr = SummaryScreen_CopyColoredString(ptr, gStringVar1, 14);
                StringCopy(ptr, gOtherText_Met2);
            }
        }
    }

    Menu_PrintText(gStringVar4, left++, top++);
}

static void PrintMonSpeciesLevelAndGender(struct Pokemon *mon)
{
    u16 species;
    u8 *buffer;
    u8 level;

    species = GetMonData(mon, MON_DATA_SPECIES);

    buffer = gStringVar1;
    buffer = SummaryScreen_SetTextColor(buffer, 13);
    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x11;
    buffer[2] = 0x7;
    buffer[3] = CHAR_SLASH;
    buffer += 4;
    buffer = StringCopy(buffer, gSpeciesNames[species]);

    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x13;
    buffer[2] = 0x50;
    buffer[3] = EOS;

    Menu_PrintText(gStringVar1, 0, 14);
    Menu_EraseWindowRect(3, 16, 9, 17);

    level = GetMonData(mon, MON_DATA_LEVEL);

    buffer = SummaryScreen_SetTextColor(gStringVar1, 13);
    buffer[0] = CHAR_LV;
    buffer += 1;
    buffer = ConvertIntToDecimalStringSimple(buffer, level);

    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x13;
    buffer[2] = 0x20;
    buffer[3] = EOS;

    Menu_PrintText(gStringVar1, 3, 16);
    PrintGenderSymbol(mon, 7, 16);
}

static void PrintGenderSymbol(struct Pokemon *mon, u8 left, u8 top)
{
    const u8 *genderSymbol;
    u8 color;
    u8 bottom;
    u16 species = GetMonData(mon, MON_DATA_SPECIES2);

    if (species != SPECIES_NIDORAN_M && species != SPECIES_NIDORAN_F)
    {
        u8 gender = GetMonGender(mon);
        switch (gender)
        {
        default:
            bottom = top + 1;
            Menu_EraseWindowRect(left, top, left, bottom);
            return;
        case MON_MALE:
            genderSymbol = gOtherText_MaleSymbol2;
            color = 11;
            break;
        case MON_FEMALE:
            genderSymbol = gOtherText_FemaleSymbol2;
            color = 12;
            break;
        }

        SummaryScreen_PrintColoredText(genderSymbol, color, left, top);
    }
}

u8 GetNumRibbons(struct Pokemon *mon)
{
    u8 numRibbons = 0;
    numRibbons += GetMonData(mon, MON_DATA_COOL_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_BEAUTY_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_CUTE_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_SMART_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_TOUGH_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_CHAMPION_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_WINNING_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_VICTORY_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_ARTIST_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_EFFORT_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_MARINE_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_LAND_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_SKY_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_COUNTRY_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_NATIONAL_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_EARTH_RIBBON);
    numRibbons += GetMonData(mon, MON_DATA_WORLD_RIBBON);

    return numRibbons;
}

static void PrintRibbonCount(struct Pokemon *mon)
{
    u8 numRibbons = GetNumRibbons(mon);

    if (numRibbons == 0)
    {
        StringCopy(gStringVar1, gOtherText_None);
    }
    else
    {
        u8 ribbonsStringLength;
        u8 *text;

        StringCopy(gStringVar1, gOtherText_Ribbons00);
        ribbonsStringLength = StringLength(gStringVar1);

        text = &gStringVar1[ribbonsStringLength - 2];

        text[0] = EXT_CTRL_CODE_BEGIN;
        text[1] = 0x14;
        text[2] = 6;
        ConvertIntToDecimalStringN(&text[3], numRibbons, 1, 2);
    }

    Menu_PrintText(sRibbonsTextTemplate, 21, 4);
}

static void PrintHeldItemName(u16 itemId, u8 left, u8 top)
{
    if (itemId == ITEM_ENIGMA_BERRY
     && MenuHelpers_IsLinkActive() == TRUE
     && IsLinkDoubleBattle() == TRUE
     && (pssData.curMonIndex == 1 || pssData.curMonIndex == 4 || pssData.curMonIndex == 5))
        StringCopy(gStringVar1, GetItemName(itemId));
    else if (itemId == 0)
        StringCopy(gStringVar1, gOtherText_None);
    else
        CopyItemName(itemId, gStringVar1);

    Menu_PrintText(sHeldItemTextTemplate, left, top);
}

static void DrawExperienceProgressBar(struct Pokemon *mon, u8 left, u8 top)
{
    u32 curExperience;
    u8 level;
    u16 species;
    u8 i;
    u16 *vramAddr;
    u32 expToNextLevel = 0;
    s64 numExpProgressBarTicks = 0;

    curExperience = GetMonData(mon, MON_DATA_EXP);
    level = GetMonData(mon, MON_DATA_LEVEL);
    species = GetMonData(mon, MON_DATA_SPECIES);

    // The experience progress bar is shown as empty when the Pokemon is already level 100.
    if (level < 100)
    {
        u32 nextLevelExp;
        u32 expSinceLastLevel;
        u32 expBetweenLevels;
        u32 curLevelExperience;

        nextLevelExp = gExperienceTables[gBaseStats[species].growthRate][level + 1];
        expToNextLevel = nextLevelExp - curExperience;
        curLevelExperience = gExperienceTables[gBaseStats[species].growthRate][level];
        expBetweenLevels = (nextLevelExp - curLevelExperience);
        expSinceLastLevel = curExperience - curLevelExperience;

        // Calculate the number of 1-pixel "ticks" to illuminate in the experience progress bar.
        // There are 8 tiles that make up the bar, and each tile has 8 "ticks". Hence, the numerator
        // is multiplied by 64.
        numExpProgressBarTicks = (expSinceLastLevel * 64) / expBetweenLevels;
        if (numExpProgressBarTicks == 0 && expSinceLastLevel != 0)
        {
            // Ensure sure some exp. gain is visible in the progress bar.
            numExpProgressBarTicks = 1;
        }
    }

    ConvertIntToDecimalStringSimple(gStringVar1, expToNextLevel);
    MenuPrint_RightAligned(gStringVar1, left + 6, top);


    // Draw each of the 8 tiles that make up the experience progress bar.
    vramAddr = (u16 *)(VRAM + 0x4CAA);
    for (i = 0; i < 8; i++)
    {
        u16 tile;

        if (numExpProgressBarTicks >= 8)
            tile = 0x2062 + 8; // full exp. bar block
        else
            tile = 0x2062 + (numExpProgressBarTicks % 8);

        vramAddr[i] = tile;

        numExpProgressBarTicks -= 8;
        if (numExpProgressBarTicks < 0)
        {
            numExpProgressBarTicks = 0;
        }
    }
}

// Prints the text displayed in the top-left or top-right of the screen.
// Each of the 4 summary screens displays different text.
static void PrintSummaryWindowHeaderText(void)
{
    u8 *buffer = gStringVar1;

    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x12;
    buffer[2] = 0x2;

    buffer += 3;
    buffer = SummaryScreen_SetTextColor(buffer, 13);
    buffer = StringCopy(buffer, sPageHeaderTexts[pssData.headerTextId]);

    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x13;
    buffer[2] = 0x58;
    buffer[3] = EOS;

    Menu_PrintText(gStringVar1, 0, 0);

    if (pssData.headerActionTextId != 0)
    {
        SummaryScreen_PlaceTextTile_White(5, 23, 0);
        SummaryScreen_PlaceTextTile_White(6, 24, 0);
    }
    else
    {
        Menu_EraseWindowRect(23, 0, 24, 1);
    }

    buffer = gStringVar1;
    buffer = SummaryScreen_SetTextColor(buffer, 13);
    buffer = StringCopy(buffer, sPageHeaderTexts[pssData.headerActionTextId]);

    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x13;
    buffer[2] = 0x28;
    buffer[3] = EOS;

    Menu_PrintText(gStringVar1, 25, 0);
}

// If the given pokemon previously had the pokerus virus, a small
// dot will be drawn in between the pokeball and the mon's level.
static void DrawPokerusCuredSymbol(struct Pokemon *mon)
{
    u16 *vram1 = (u16 *)(VRAM + 0xE444);
    u16 *vram2 = (u16 *)(VRAM + 0xEC44);

    if (!CheckPartyPokerus(mon, 0) && CheckPartyHasHadPokerus(mon, 0))
    {
        *vram1 = 0x2C;
        *vram2 = 0x2C;
    }
    else
    {
        *vram1 = 0x081A;
        *vram2 = 0x081A;
    }
}

// Draws the 4 small navigation circles at the top of the pokemon summary screen.
void DrawPagination(void)
{
    u16 arr[8];
    u8 i;

    // Shifting 1 bit instead of multiplying by 2 is required to match.
    for (i = 0; i < 4; i++)
    {
        if (i < pssData.minPageIndex)
        {
            arr[i << 1] = 0x4040;
            arr[(i << 1) + 1] = 0x4040;
        }
        else if (i > pssData.maxPageIndex)
        {
            arr[i << 1] = 0x404A;
            arr[(i << 1) + 1] = 0x404A;
        }
        else
        {
            if (i < pssData.currPageIndex)
            {
                arr[i << 1] = 0x4046;
                arr[(i << 1) + 1] = 0x4046 + 1;
            }

            if (i == pssData.currPageIndex)
            {
                if (i != pssData.maxPageIndex)
                {
                    arr[i << 1] = 0x4041;
                    arr[(i << 1) + 1] = 0x4041 + 1;
                }
                else
                {
                    arr[i << 1] = 0x404B;
                    arr[(i << 1) + 1] = 0x404B + 1;
                }
            }

            if (i > pssData.currPageIndex)
            {
                if (i != pssData.maxPageIndex)
                {
                    arr[i << 1] = 0x4043;
                    arr[(i << 1) + 1] = 0x4043 + 1;
                }
                else
                {
                    arr[i << 1] = 0x4048;
                    arr[(i << 1) + 1] = 0x4048 + 1;
                }
            }
        }
    }

    DmaCopy16Defvars(3, arr, (void *)(VRAM + 0xE016), 16);

    for (i = 0; i < 8; i++)
    {
        arr[i] += 0x10;
    }

    DmaCopy16Defvars(3, arr, (void *)(VRAM + 0xE056), 16);
}

// Like DmaCopyLarge16 but the size check is up top
#define DmaCopyLargeCheckFirst16(_src,_dest,_size) { \
    const void * src = (const void *)(_src);         \
    void * dest = (void *)(_dest);                   \
    u32 size = (u32)(_size);                         \
    while (1)                                        \
    {                                                \
        if (size <= 0x1000)                          \
        {                                            \
            DmaCopy16(3, src, dest, size);           \
            break;                                   \
        }                                            \
        DmaCopy16(3, src, dest, 0x1000);             \
        src += 0x1000;                               \
        dest += 0x1000;                              \
        size -= 0x1000;                              \
    }                                                \
}

void CreateSetStatusSprite(void);

void Task_SlideStatusWindow(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    u8 i;

    data[1] += data[0];
    if (data[1] != 0)
    {
        DmaCopyLargeCheckFirst16(&gUnknown_08E73508[0x24A - data[1]], (void *)(BG_SCREEN_ADDR(28) + 0x480), data[1] * 2);
        DmaCopyLargeCheckFirst16(&gUnknown_08E73508[0x26A - data[1]], (void *)(BG_SCREEN_ADDR(28) + 0x4C0), data[1] * 2);
        DmaCopyLargeCheckFirst16(&gUnknown_08E73508[0x24A - data[1]], (void *)(BG_SCREEN_ADDR(29) + 0x480), data[1] * 2);
        DmaCopyLargeCheckFirst16(&gUnknown_08E73508[0x26A - data[1]], (void *)(BG_SCREEN_ADDR(29) + 0x4C0), data[1] * 2);
    }
    for (i = data[1]; i < 10; i++)
    {
        DmaCopy16Defvars(3, &data[2], (void *)(BG_SCREEN_ADDR(28) + 0x480 + 2 * i), 2);
        DmaCopy16Defvars(3, &data[2], (void *)(BG_SCREEN_ADDR(28) + 0x4C0 + 2 * i), 2);
        DmaCopy16Defvars(3, &data[2], (void *)(BG_SCREEN_ADDR(29) + 0x480 + 2 * i), 2);
        DmaCopy16Defvars(3, &data[2], (void *)(BG_SCREEN_ADDR(29) + 0x4C0 + 2 * i), 2);
    }
    if (data[0] == 0 || data[1] <= 0 || data[1] >= 10)
    {
        if (data[1] >= 10)
            SummaryScreen_PrintColoredText(gOtherText_Status, 13, 1, 18);
        CreateSetStatusSprite();
        DestroyTask(taskId);
    }
}

static void PositionStatusSlidingWindow(s8 a)
{
    u8 newTaskId;

    Menu_EraseWindowRect(1, 18, 5, 19);
    DestroySpriteInArray(29);

    newTaskId = CreateTask(Task_SlideStatusWindow, 0);
    gTasks[newTaskId].data[0] = a;

    if (a < 0)
        gTasks[newTaskId].data[1] = 10;
    else
        gTasks[newTaskId].data[1] = 0;

    gTasks[newTaskId].data[2] = 1;
}

// void Task_SlidePowerAccWindow(u8 taskId)
// {
//     u8 i;
//     s16 var1;

//     gTasks[taskId].data[1] += gTasks[taskId].data[0];

//     var1 = 0;
//     if (gTasks[taskId].data[1] >= 0)
//     {
//         var1 = 10;
//         if (gTasks[taskId].data[1] < 10)
//         {
//             var1 = gTasks[taskId].data[1];
//         }
//     }

//     if (var1 > 0)
//     {
//         u8 *vramAddr = (u8 *)(VRAM + 0x5B40);
//         for (i = 0; i < 7; i++)
//         {
//             CpuSet(&gUnknown_08E73E88[(i + 13) * 64], vramAddr, var1 & 0x1FFFFF);
//             vramAddr += 64;
//         }
//     }

//     if (var1 <= 9)
//     {
//         u8 *vramAddr = (u8 *)(VRAM + 0x5B40);
//         for (i = 0; i < 64; i++)
//         {
//             u16 val = gTasks[taskId].data[2];
//             CpuSet(&val, vramAddr, ((10 - var1) & 0x1FFFFF) | 0x800000);
//         }
//     }
//     else
//     {
//         Menu_EraseWindowRect(0, 19, 9, 19);
//     }

//     if (gTasks[taskId].data[0] == 0 || gTasks[taskId].data[1] < 0)
//     {
//         if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
//         {
//             Menu_EraseWindowRect(0, 14, 9, 18);
//             PrintMonSpeciesLevelAndGender(pssData.currentMon);

//             if (GetMonStatusAndPokerus(pssData.currentMon))
//             {
//                 SummaryScreen_PrintColoredText(gOtherText_Status, 13, 1, 18);
//             }

//             DestroyTask(taskId);
//         }
//     }

//     if (gTasks[taskId].data[1] > 9)
//     {
//         if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
//         {
//             PrintMoveDetailLabels(gTasks[taskId].data[3]);
//         }

//         PrintMoveDetails(pssData.currentMon, &gTasks[taskId].data[3]);
//         DestroyTask(taskId);
//     }
// }
NAKED
static void Task_SlidePowerAccWindow(u8 taskId)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    sub sp, 0x4\n\
    lsls r0, 24\n\
    lsrs r0, 24\n\
    mov r10, r0\n\
    lsls r0, 2\n\
    add r0, r10\n\
    lsls r0, 3\n\
    ldr r1, _080A13E4 @ =gTasks + 0x8\n\
    adds r7, r0, r1\n\
    ldrh r0, [r7]\n\
    ldrh r1, [r7, 0x2]\n\
    adds r0, r1\n\
    strh r0, [r7, 0x2]\n\
    lsls r0, 16\n\
    asrs r1, r0, 16\n\
    movs r0, 0\n\
    cmp r1, 0\n\
    blt _080A136A\n\
    movs r0, 0xA\n\
    cmp r1, 0xA\n\
    bgt _080A136A\n\
    ldrh r0, [r7, 0x2]\n\
_080A136A:\n\
    lsls r0, 16\n\
    asrs r2, r0, 16\n\
    mov r9, r0\n\
    cmp r2, 0\n\
    ble _080A13A6\n\
    movs r0, 0xA\n\
    subs r0, r2\n\
    lsls r0, 1\n\
    ldr r1, _080A13E8 @ =0x06005b40\n\
    adds r4, r0, r1\n\
    movs r5, 0\n\
    adds r6, r2, 0\n\
    ldr r0, _080A13EC @ =0x001fffff\n\
    mov r8, r0\n\
_080A1386:\n\
    adds r0, r5, 0\n\
    adds r0, 0xD\n\
    lsls r0, 6\n\
    ldr r1, _080A13F0 @ =gUnknown_08E73E88\n\
    adds r0, r1\n\
    adds r1, r4, 0\n\
    mov r2, r8\n\
    ands r2, r6\n\
    bl CpuSet\n\
    adds r4, 0x40\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x6\n\
    bls _080A1386\n\
_080A13A6:\n\
    mov r1, r9\n\
    asrs r2, r1, 16\n\
    cmp r2, 0x9\n\
    bgt _080A13F4\n\
    ldr r4, _080A13E8 @ =0x06005b40\n\
    movs r5, 0\n\
    mov r8, sp\n\
    movs r0, 0xA\n\
    subs r6, r0, r2\n\
    ldr r0, _080A13EC @ =0x001fffff\n\
    ands r6, r0\n\
    movs r0, 0x80\n\
    lsls r0, 17\n\
    mov r9, r0\n\
_080A13C2:\n\
    ldrh r0, [r7, 0x4]\n\
    mov r1, r8\n\
    strh r0, [r1]\n\
    mov r0, sp\n\
    adds r1, r4, 0\n\
    mov r2, r9\n\
    orrs r2, r6\n\
    bl CpuSet\n\
    adds r4, 0x40\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x6\n\
    bls _080A13C2\n\
    b _080A1400\n\
    .align 2, 0\n\
_080A13E4: .4byte gTasks + 0x8\n\
_080A13E8: .4byte 0x06005b40\n\
_080A13EC: .4byte 0x001fffff\n\
_080A13F0: .4byte gUnknown_08E73E88\n\
_080A13F4:\n\
    movs r0, 0\n\
    movs r1, 0x13\n\
    movs r2, 0x9\n\
    movs r3, 0x13\n\
    bl Menu_EraseWindowRect\n\
_080A1400:\n\
    movs r1, 0\n\
    ldrsh r0, [r7, r1]\n\
    cmp r0, 0\n\
    beq _080A1410\n\
    movs r1, 0x2\n\
    ldrsh r0, [r7, r1]\n\
    cmp r0, 0\n\
    bge _080A144A\n\
_080A1410:\n\
    ldr r4, _080A1480 @ =gSharedMem + 0x18000\n\
    ldrb r0, [r4, 0xB]\n\
    cmp r0, 0x2\n\
    bne _080A1444\n\
    movs r0, 0\n\
    movs r1, 0xE\n\
    movs r2, 0x9\n\
    movs r3, 0x12\n\
    bl Menu_EraseWindowRect\n\
    adds r4, 0x10\n\
    adds r0, r4, 0\n\
    bl PrintMonSpeciesLevelAndGender\n\
    adds r0, r4, 0\n\
    bl GetMonStatusAndPokerus\n\
    lsls r0, 24\n\
    cmp r0, 0\n\
    beq _080A1444\n\
    ldr r0, _080A1484 @ =gOtherText_Status\n\
    movs r1, 0xD\n\
    movs r2, 0x1\n\
    movs r3, 0x12\n\
    bl SummaryScreen_PrintColoredText\n\
_080A1444:\n\
    mov r0, r10\n\
    bl DestroyTask\n\
_080A144A:\n\
    movs r1, 0x2\n\
    ldrsh r0, [r7, r1]\n\
    cmp r0, 0x9\n\
    ble _080A1470\n\
    ldr r4, _080A1480 @ =gSharedMem + 0x18000\n\
    ldrb r0, [r4, 0xB]\n\
    cmp r0, 0x2\n\
    bne _080A1460\n\
    ldrb r0, [r7, 0x6]\n\
    bl PrintMoveDetailLabels\n\
_080A1460:\n\
    adds r0, r4, 0\n\
    adds r0, 0x10\n\
    adds r1, r7, 0x6\n\
    bl PrintMoveDetails\n\
    mov r0, r10\n\
    bl DestroyTask\n\
_080A1470:\n\
    add sp, 0x4\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_080A1480: .4byte gSharedMem + 0x18000\n\
_080A1484: .4byte gOtherText_Status\n\
    .syntax divided\n");
}

// Related to re-drawing the summary area underneath the pokemon's picture
// in all of the summary screen tabs.
static void PositionPowerAccSlidingWindow(s8 a, u8 b)
{
    u8 taskId;

    if (pssData.currPageIndex == PSS_PAGE_BATTLE_MOVES)
        Menu_EraseWindowRect(0, 14, 9, 19);

    taskId = FindTaskIdByFunc(Task_SlidePowerAccWindow);
    if (taskId == 0xFF)
        taskId = CreateTask(Task_SlidePowerAccWindow, 0);

    gTasks[taskId].data[0] = a;

    if (a < 0)
        gTasks[taskId].data[1] = 10;
    else
        gTasks[taskId].data[1] = 0;

    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = b;
}

NAKED
static void Task_SlideAppealJamWindow(u8 taskId)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    sub sp, 0x4\n\
    lsls r0, 24\n\
    lsrs r0, 24\n\
    mov r10, r0\n\
    lsls r0, 2\n\
    add r0, r10\n\
    lsls r0, 3\n\
    ldr r1, _080A15B0 @ =gTasks + 0x8\n\
    adds r7, r0, r1\n\
    ldrh r0, [r7]\n\
    ldrh r1, [r7, 0x2]\n\
    adds r0, r1\n\
    strh r0, [r7, 0x2]\n\
    lsls r0, 16\n\
    asrs r1, r0, 16\n\
    movs r0, 0\n\
    cmp r1, 0\n\
    blt _080A1536\n\
    movs r0, 0xA\n\
    cmp r1, 0xA\n\
    bgt _080A1536\n\
    ldrh r0, [r7, 0x2]\n\
_080A1536:\n\
    lsls r0, 16\n\
    asrs r2, r0, 16\n\
    mov r9, r0\n\
    cmp r2, 0\n\
    ble _080A1572\n\
    movs r0, 0xA\n\
    subs r0, r2\n\
    lsls r0, 1\n\
    ldr r1, _080A15B4 @ =0x06006b40\n\
    adds r4, r0, r1\n\
    movs r5, 0\n\
    adds r6, r2, 0\n\
    ldr r0, _080A15B8 @ =0x001fffff\n\
    mov r8, r0\n\
_080A1552:\n\
    adds r0, r5, 0\n\
    adds r0, 0xD\n\
    lsls r0, 6\n\
    ldr r1, _080A15BC @ =gUnknown_08E74688\n\
    adds r0, r1\n\
    adds r1, r4, 0\n\
    mov r2, r8\n\
    ands r2, r6\n\
    bl CpuSet\n\
    adds r4, 0x40\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x6\n\
    bls _080A1552\n\
_080A1572:\n\
    mov r1, r9\n\
    asrs r2, r1, 16\n\
    cmp r2, 0x9\n\
    bgt _080A15C0\n\
    ldr r4, _080A15B4 @ =0x06006b40\n\
    movs r5, 0\n\
    mov r8, sp\n\
    movs r0, 0xA\n\
    subs r6, r0, r2\n\
    ldr r0, _080A15B8 @ =0x001fffff\n\
    ands r6, r0\n\
    movs r0, 0x80\n\
    lsls r0, 17\n\
    mov r9, r0\n\
_080A158E:\n\
    ldrh r0, [r7, 0x4]\n\
    mov r1, r8\n\
    strh r0, [r1]\n\
    mov r0, sp\n\
    adds r1, r4, 0\n\
    mov r2, r9\n\
    orrs r2, r6\n\
    bl CpuSet\n\
    adds r4, 0x40\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x6\n\
    bls _080A158E\n\
    b _080A15CC\n\
    .align 2, 0\n\
_080A15B0: .4byte gTasks + 0x8\n\
_080A15B4: .4byte 0x06006b40\n\
_080A15B8: .4byte 0x001fffff\n\
_080A15BC: .4byte gUnknown_08E74688\n\
_080A15C0:\n\
    movs r0, 0\n\
    movs r1, 0x13\n\
    movs r2, 0x9\n\
    movs r3, 0x13\n\
    bl Menu_EraseWindowRect\n\
_080A15CC:\n\
    movs r1, 0\n\
    ldrsh r0, [r7, r1]\n\
    cmp r0, 0\n\
    beq _080A15DC\n\
    movs r1, 0x2\n\
    ldrsh r0, [r7, r1]\n\
    cmp r0, 0\n\
    bge _080A1616\n\
_080A15DC:\n\
    ldr r4, _080A164C @ =gSharedMem + 0x18000\n\
    ldrb r0, [r4, 0xB]\n\
    cmp r0, 0x3\n\
    bne _080A1610\n\
    movs r0, 0\n\
    movs r1, 0xE\n\
    movs r2, 0x9\n\
    movs r3, 0x12\n\
    bl Menu_EraseWindowRect\n\
    adds r4, 0x10\n\
    adds r0, r4, 0\n\
    bl PrintMonSpeciesLevelAndGender\n\
    adds r0, r4, 0\n\
    bl GetMonStatusAndPokerus\n\
    lsls r0, 24\n\
    cmp r0, 0\n\
    beq _080A1610\n\
    ldr r0, _080A1650 @ =gOtherText_Status\n\
    movs r1, 0xD\n\
    movs r2, 0x1\n\
    movs r3, 0x12\n\
    bl SummaryScreen_PrintColoredText\n\
_080A1610:\n\
    mov r0, r10\n\
    bl DestroyTask\n\
_080A1616:\n\
    movs r1, 0x2\n\
    ldrsh r0, [r7, r1]\n\
    cmp r0, 0x9\n\
    ble _080A163C\n\
    ldr r4, _080A164C @ =gSharedMem + 0x18000\n\
    ldrb r0, [r4, 0xB]\n\
    cmp r0, 0x3\n\
    bne _080A162C\n\
    ldrb r0, [r7, 0x6]\n\
    bl PrintMoveDetailLabels\n\
_080A162C:\n\
    adds r0, r4, 0\n\
    adds r0, 0x10\n\
    adds r1, r7, 0x6\n\
    bl PrintMoveDetails\n\
    mov r0, r10\n\
    bl DestroyTask\n\
_080A163C:\n\
    add sp, 0x4\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_080A164C: .4byte gSharedMem + 0x18000\n\
_080A1650: .4byte gOtherText_Status\n\
    .syntax divided\n");
}

static void PositionAppealJamSlidingWindow(s8 a, u8 b)
{
    u8 taskId;

    if (pssData.currPageIndex == PSS_PAGE_CONTEST_MOVES)
        Menu_EraseWindowRect(0, 14, 9, 19);

    taskId = FindTaskIdByFunc(Task_SlideAppealJamWindow);
    if (taskId == 0xFF)
        taskId = CreateTask(Task_SlideAppealJamWindow, 0);

    gTasks[taskId].data[0] = a;

    if (a < 0)
        gTasks[taskId].data[1] = 10;
    else
        gTasks[taskId].data[1] = 0;

    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = b;
}

// not enough registers allocated (need to allocate r8 and r9)
#ifdef NONMATCHING
static void TilemapFiveMovesDisplay(u8 a)
{
    u8 i;
    u16 *vramAddr = (u16 *)(VRAM + 0x6AD4);

    if (a == 0)
    {
        for (i = 0; i < 20; i++)
        {
            vramAddr[i] = gUnknown_08E94510[i] + 0x1000;
            vramAddr[i + 0x20] = gUnknown_08E94510[i] + 0x1000;
            vramAddr[i + 0x40] = gUnknown_08E94550[i] + 0x1000;
        }
    }
    else
    {
        i = 0;
        for (i = 0; i < 20; i++)
        {
            vramAddr[i] = gUnknown_08E94550[i] + 0x1000;
            vramAddr[i + 0x20] = gUnknown_08E94590[i] + 0x1000;
            vramAddr[i + 0x40] = gUnknown_08E94590[i] + 0x1000;
        }
    }

    vramAddr = (u16 *)(VRAM + 0x5AD4);

    if (a == 0)
    {
        for (i = 0; i < 20; i++)
        {
            vramAddr[i] = gUnknown_08E94510[i] + 0x3000;
            vramAddr[i + 0x20] = gUnknown_08E94510[i] + 0x3000;
            vramAddr[i + 0x40] = gUnknown_08E94550[i] + 0x3000;
        }
    }
    else
    {
        for (i = 0; i < 20; i++)
        {
            vramAddr[i] = gUnknown_08E94550[i] + 0x3000;
            vramAddr[i + 0x20] = gUnknown_08E94590[i] + 0x3000;
            vramAddr[i + 0x40] = gUnknown_08E94590[i] + 0x3000;
        }
    }
}
#else
NAKED
static void TilemapFiveMovesDisplay(u8 a)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r9\n\
    mov r6, r8\n\
    push {r6,r7}\n\
    lsls r0, 24\n\
    lsrs r0, 24\n\
    mov r9, r0\n\
    ldr r0, _080A1724 @ =0x06006ad4\n\
    mov r12, r0\n\
    mov r1, r9\n\
    cmp r1, 0\n\
    bne _080A172C\n\
    movs r5, 0\n\
    ldr r7, _080A1728 @ =gUnknown_08E94510\n\
    movs r0, 0x80\n\
    lsls r0, 5\n\
    adds r6, r0, 0\n\
    movs r1, 0x40\n\
    adds r1, r7\n\
    mov r8, r1\n\
_080A16F4:\n\
    lsls r2, r5, 1\n\
    mov r0, r12\n\
    adds r3, r2, r0\n\
    adds r4, r2, r7\n\
    ldrh r1, [r4]\n\
    adds r0, r6, r1\n\
    strh r0, [r3]\n\
    adds r1, r3, 0\n\
    adds r1, 0x40\n\
    ldrh r4, [r4]\n\
    adds r0, r6, r4\n\
    strh r0, [r1]\n\
    adds r3, 0x80\n\
    add r2, r8\n\
    ldrh r2, [r2]\n\
    adds r0, r6, r2\n\
    strh r0, [r3]\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x13\n\
    bls _080A16F4\n\
    b _080A1768\n\
    .align 2, 0\n\
_080A1724: .4byte 0x06006ad4\n\
_080A1728: .4byte gUnknown_08E94510\n\
_080A172C:\n\
    movs r5, 0\n\
    ldr r6, _080A17B0 @ =gUnknown_08E94550\n\
    movs r7, 0x80\n\
    lsls r7, 5\n\
    adds r4, r7, 0\n\
    movs r0, 0x40\n\
    adds r0, r6\n\
    mov r8, r0\n\
_080A173C:\n\
    lsls r1, r5, 1\n\
    mov r7, r12\n\
    adds r2, r1, r7\n\
    adds r0, r1, r6\n\
    ldrh r0, [r0]\n\
    adds r0, r4, r0\n\
    strh r0, [r2]\n\
    adds r3, r2, 0\n\
    adds r3, 0x40\n\
    add r1, r8\n\
    ldrh r7, [r1]\n\
    adds r0, r4, r7\n\
    strh r0, [r3]\n\
    adds r2, 0x80\n\
    ldrh r1, [r1]\n\
    adds r0, r4, r1\n\
    strh r0, [r2]\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x13\n\
    bls _080A173C\n\
_080A1768:\n\
    ldr r0, _080A17B4 @ =0x06005ad4\n\
    mov r12, r0\n\
    mov r1, r9\n\
    cmp r1, 0\n\
    bne _080A17BC\n\
    movs r5, 0\n\
    ldr r7, _080A17B8 @ =gUnknown_08E94510\n\
    movs r0, 0xC0\n\
    lsls r0, 6\n\
    adds r6, r0, 0\n\
    movs r1, 0x40\n\
    adds r1, r7\n\
    mov r8, r1\n\
_080A1782:\n\
    lsls r2, r5, 1\n\
    mov r0, r12\n\
    adds r3, r2, r0\n\
    adds r4, r2, r7\n\
    ldrh r1, [r4]\n\
    adds r0, r6, r1\n\
    strh r0, [r3]\n\
    adds r1, r3, 0\n\
    adds r1, 0x40\n\
    ldrh r4, [r4]\n\
    adds r0, r6, r4\n\
    strh r0, [r1]\n\
    adds r3, 0x80\n\
    add r2, r8\n\
    ldrh r2, [r2]\n\
    adds r0, r6, r2\n\
    strh r0, [r3]\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x13\n\
    bls _080A1782\n\
    b _080A17F8\n\
    .align 2, 0\n\
_080A17B0: .4byte gUnknown_08E94550\n\
_080A17B4: .4byte 0x06005ad4\n\
_080A17B8: .4byte gUnknown_08E94510\n\
_080A17BC:\n\
    movs r5, 0\n\
    ldr r6, _080A1804 @ =gUnknown_08E94550\n\
    movs r7, 0xC0\n\
    lsls r7, 6\n\
    adds r4, r7, 0\n\
    movs r0, 0x40\n\
    adds r0, r6\n\
    mov r8, r0\n\
_080A17CC:\n\
    lsls r1, r5, 1\n\
    mov r7, r12\n\
    adds r2, r1, r7\n\
    adds r0, r1, r6\n\
    ldrh r0, [r0]\n\
    adds r0, r4, r0\n\
    strh r0, [r2]\n\
    adds r3, r2, 0\n\
    adds r3, 0x40\n\
    add r1, r8\n\
    ldrh r7, [r1]\n\
    adds r0, r4, r7\n\
    strh r0, [r3]\n\
    adds r2, 0x80\n\
    ldrh r1, [r1]\n\
    adds r0, r4, r1\n\
    strh r0, [r2]\n\
    adds r0, r5, 0x1\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r5, 0x13\n\
    bls _080A17CC\n\
_080A17F8:\n\
    pop {r3,r4}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_080A1804: .4byte gUnknown_08E94550\n\
    .syntax divided\n");
}
#endif // NONMATCHING

u8 CreateMonSprite(struct Pokemon *mon)
{
    u16 species;
    u8 spriteId;

    species = GetMonData(mon, MON_DATA_SPECIES2);
    spriteId = CreateSprite(&gCreatingSpriteTemplate, 40, 64, 5);

    FreeSpriteOamMatrix(&gSprites[spriteId]);

    gSprites[spriteId].data[0] = species;
    gSprites[spriteId].callback = SpriteCB_Pokemon;

    if (!IsPokeSpriteNotFlipped(species))
        gSprites[spriteId].hFlip = TRUE;
    else
        gSprites[spriteId].hFlip = FALSE;

    return spriteId;
}

static void SpriteCB_Pokemon(struct Sprite *sprite)
{
    if (!gPaletteFade.active)
    {
        sprite->callback = SpriteCallbackDummy;

        if (!GetMonData(&pssData.currentMon, MON_DATA_IS_EGG))
            PlayCry_Normal(sprite->data[0], 0);
    }
}

static void ResetSpriteIds(void)
{
    u8 i;

    for (i = 0; i < 30; i++)
        sSummaryScreenSpriteIds[i] = 0xFF;
}

static void DestroySpriteInArray(u8 a)
{
    if (sSummaryScreenSpriteIds[a] != 0xFF)
    {
        DestroySprite(&gSprites[sSummaryScreenSpriteIds[a]]);
        sSummaryScreenSpriteIds[a] = 0xFF;
    }
}

static void SetSpriteInvisibility(u8 a, u8 invisible)
{
    gSprites[sSummaryScreenSpriteIds[a]].invisible = invisible;
}

static void CreateMoveTypeIcons(void)
{
    u8 i;

    for (i = 0; i < 5; i++)
    {
        if (sSummaryScreenSpriteIds[i] == 0xFF)
            sSummaryScreenSpriteIds[i] = CreateSprite(&sSpriteTemplate_MoveTypes, 0, 0, 2);

        SetSpriteInvisibility(i, 1);
    }
}

static void SetTypeSpritePosAndPal(u8 animNum, u8 x, u8 y, u8 d)
{
    StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[d]], animNum);

    gSprites[sSummaryScreenSpriteIds[d]].oam.paletteNum = sMoveTypeToOamPaletteNum[animNum];
    gSprites[sSummaryScreenSpriteIds[d]].x = x + 16;
    gSprites[sSummaryScreenSpriteIds[d]].y = y + 8;

    SetSpriteInvisibility(d, 0);
}

static void CreateMoveSelectorSprites(u8 a)
{
    u8 i;
    s16 x;
    u8 subPriority = 0;

    if (pssData.currPageIndex >= PSS_PAGE_BATTLE_MOVES)
    {
        if (a == 9)
            subPriority = 1;

        for (i = 0; i < 10; i++)
        {
            x = (i * 16) + 0x58;
            sSummaryScreenSpriteIds[a + i] = CreateSprite(&sMoveSelectorSpriteTemplate, x, 40, subPriority);

            if (i == 0)
                StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[a]], 4);
            else if (i == 9)
                StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[a + i]], 5);
            else
                StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[a + i]], 6);

            gSprites[sSummaryScreenSpriteIds[a + i]].callback = SpriteCB_MoveSelector;
            gSprites[sSummaryScreenSpriteIds[a + i]].data[0] = a;
            gSprites[sSummaryScreenSpriteIds[a + i]].data[1] = 0;
        }
    }
}

static void DestroyMoveSelectorSprites(u8 a)
{
    u8 i;

    for (i = 0; i < 10; i++)
        DestroySpriteInArray(a + i);
}

static void SetMainMoveSelectorColor(u8 a)
{
    u8 i;
    a *= 3;

    StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[9]], a + 4);

    for (i = 0; i < 8; i++)
        StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[10 + i]], a + 6);

    StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[18]], a + 5);
}

static void SpriteCB_MoveSelector(struct Sprite *sprite)
{
    if (sprite->animNum == 4 || sprite->animNum == 5 || sprite->animNum == 6)
    {
        sprite->data[1] = (sprite->data[1] + 1) & 0x1F;

        if (sprite->data[1] > 24)
            sprite->invisible = TRUE;
        else
            sprite->invisible = FALSE;
    }
    else
    {
        sprite->data[1] = 0;
        sprite->invisible = FALSE;
    }

    if (sprite->data[0] == 9)
        sprite->y2 = pssData.firstMoveIndex * 16;
    else
        sprite->y2 = pssData.secondMoveIndex * 16;
}

static void KeepMoveSelectorVisible(u8 a)
{
    u8 r3;

    a *= 10;
    for (r3 = a; r3 < a + 10; r3++)
    {
        gSprites[sMoveSelectorSpriteIds[r3]].data[1] = 0;
        gSprites[sMoveSelectorSpriteIds[r3]].invisible = FALSE;
    }
}

u8 GetPrimaryStatus(u32 status)
{
    if (status & (STATUS1_POISON | STATUS1_TOXIC_POISON))
        return STATUS_PRIMARY_POISON;
    if (status & STATUS1_PARALYSIS)
        return STATUS_PRIMARY_PARALYSIS;
    if (status & STATUS1_SLEEP)
        return STATUS_PRIMARY_SLEEP;
    if (status & STATUS1_FREEZE)
        return STATUS_PRIMARY_FREEZE;
    if (status & STATUS1_BURN)
        return STATUS_PRIMARY_BURN;

    return STATUS_PRIMARY_NONE;
}

u8 GetMonStatusAndPokerus(struct Pokemon *mon)
{
    u8 statusAilment;

    if (GetMonData(mon, MON_DATA_HP) == 0)
        return STATUS_PRIMARY_FAINTED;

    statusAilment = GetPrimaryStatus(GetMonData(mon, MON_DATA_STATUS));
    if (statusAilment == STATUS_PRIMARY_NONE)
    {
        if (!CheckPartyPokerus(mon, 0))
            return STATUS_PRIMARY_NONE;
        else
            return STATUS_PRIMARY_POKERUS;
    }

    return statusAilment;
}

void CreateSetStatusSprite(void)
{
    struct Pokemon mon;
    u8 statusAndPkrs;
    u8 statusAndPkrs2;

    CopyMonToSummaryStruct(&mon);
    statusAndPkrs = GetMonStatusAndPokerus(&mon);

    if (statusAndPkrs)
    {
        statusAndPkrs2 = statusAndPkrs - 1;
    }
    else
    {
        DestroySpriteInArray(29);
        return;
    }

    if (sSummaryScreenSpriteIds[29] == 0xFF)
        sSummaryScreenSpriteIds[29] = CreateSprite(&sSpriteTemplate_StatusCondition, 64, 152, 0);
    StartSpriteAnim(&gSprites[sSummaryScreenSpriteIds[29]], statusAndPkrs2);
}

static void CreateMonMarkingsSprite(struct Pokemon *mon)
{
    struct Sprite *sprite;

    sprite = CreateMonMarkingAllCombosSprite(TAG_MON_MARKINGS, TAG_MON_MARKINGS, sMarkings_Pal);
    sMarkingsSprite = sprite;

    if (sprite != NULL)
    {
        u8 markings = GetMonData(mon, MON_DATA_MARKINGS);
        StartSpriteAnim(sprite, markings);

        sMarkingsSprite->x = 60;
        sMarkingsSprite->y = 26;
    }
}

static void RemoveAndCreateMonMarkingsSprite(struct Pokemon *mon)
{
    DestroySprite(sMarkingsSprite);
    CreateMonMarkingsSprite(mon);
}

static void CreateCaughtBallSprite(struct Pokemon *mon)
{
    u8 ball = ItemIdToBallId(GetMonData(mon, MON_DATA_POKEBALL));
    LoadBallGfx(ball);

    pssData.ballSpriteId = CreateSprite(&gBallSpriteTemplates[ball], 6, 136, 0);
    gSprites[pssData.ballSpriteId].callback = SpriteCallbackDummy;
    gSprites[pssData.ballSpriteId].oam.priority = 3;
}

static u8 *SummaryScreen_SetTextColor(u8 *text, u8 id)
{
    if (id != 0xFF)
    {
        const struct TextColors *colors = sSummaryScreenTextColors;

        while (colors->id != 0xFF && colors->id != id)
            colors++;

        text[0] = EXT_CTRL_CODE_BEGIN;
        text[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
        text[2] = colors->color;
        text[3] = colors->background;
        text[4] = colors->shadow;

        text += 5;
    }

    return text;
}

static u8 *SummaryScreen_CopyColoredString(u8 *dest, const u8 *src, u8 id)
{
    u8 colors[3];

    Menu_GetTextColors(&colors[0], &colors[1], &colors[2]);

    dest = SummaryScreen_SetTextColor(dest, id);
    dest = StringCopy(dest, src);

    if (id != 0xFF)
    {
        dest[0] = EXT_CTRL_CODE_BEGIN;
        dest[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
        dest[2] = colors[0];
        dest[3] = colors[1];
        dest[4] = colors[2];
        dest[5] = 0xFF;

        dest += 5;
    }

    return dest;
}

static void SummaryScreen_PrintColoredTextPixelCoords(const u8 *text, u8 id, u8 left, u16 top, s32 e)
{
    SummaryScreen_CopyColoredString(gStringVar4, text, id);
    Menu_PrintTextPixelCoords(gStringVar4, left, top, (bool8)e);
}

static void SummaryScreen_PrintColoredTextCentered(const u8 *text, u8 id, u8 left, u8 top, u16 width)
{
    SummaryScreen_CopyColoredString(gStringVar4, text, id);
    MenuPrint_Centered(gStringVar4, left, top, width);
}

static void SummaryScreen_PrintColoredIntPixelCoords(s32 value, u8 id, u8 n, u8 mode, u8 left, u16 top, s32 e)
{
    ConvertIntToDecimalStringN(gStringVar1, value, mode, n);
    SummaryScreen_PrintColoredTextPixelCoords(gStringVar1, id, left, top, e);
}

static void SummaryScreen_PrintColoredText(const u8 *text, u8 id, u8 left, u8 top)
{
    SummaryScreen_CopyColoredString(gStringVar4, text, id);
    Menu_PrintText(gStringVar4, left, top);
}

u8 *PokemonSummaryScreen_CopyPokemonLevel(u8 *dest, u8 level)
{
    u8 buffer[12];

    *dest++ = CHAR_LV;

    if (level == 0)
        level = 5;

    ConvertIntToDecimalStringSimple(buffer, level);
    dest = SummaryScreen_CopyColoredString(dest, buffer, 14);
    dest = StringCopy(dest, gOtherText_Comma);

    return dest;
}

static void SetTaskWaitForLinkRecv(int taskId)
{
    gTaskFuncForLinkRecv = gTasks[taskId].func;
    gTasks[taskId].func = Task_WaitForLinkRecv;
    gTasks[taskId].func((u8)taskId);
}

static void Task_WaitForLinkRecv(u8 taskId)
{
    if (Overworld_IsRecvQueueAtMax() != TRUE)
        gTasks[taskId].func = gTaskFuncForLinkRecv;
}
