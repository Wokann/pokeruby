#if DEBUG
#include "global.h"
#include "decompress.h"
#include "ewram.h"
#include "random.h"
#include "gba/flash_internal.h"
#include "constants/items.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "debug.h"
#include "palette.h"
#include "scanline_effect.h"
#include "main.h"
#include "task.h"
#include "sprite.h"
#include "text.h"
#include "menu.h"
#include "script.h"
#include "overworld.h"
#include "data2.h"
#include "fieldmap.h"
#include "save.h"
#include "sound.h"
#include "pokedex.h"
#include "load_save.h"
#include "pokemon_storage_system.h"
#include "battle.h"
#include "battle_bg.h"
#include "item.h"
#include "pokemon_summary_screen.h"
#include "pokemon_icon.h"
#include "field_effect.h"

struct WatanabeDebugPokemonEditorField {
    u32 minValue;
    u32 maxValue;
    u8 skipValueAdjustment;
    u8 digitCount;
};

struct WatanabeDebugBattleMode {
    u8 battleType;
    u8 battlerCount;
};

struct WatanabeDebugMenuItem {
    const u8 *label;
    union {
        struct WatanabeDebugBattleMode battleMode;
        u32 trainerAiMask;
        u8 battleEnvironment;
        u8 categoryLength;
        u8 status;
        const struct WatanabeDebugPokemonEditorField *pokemonEditorField;
    } data;
};

struct WatanabePokemonCreatorState {
    struct Pokemon pokemon;
    u32 values[0x40];
    u32 previousValue;
    u8 categoryIndex;
    u8 fieldIndex;
    u8 digitIndex;
    u8 textBuffer[259];
};

enum WatanabePokemonCreatorValue {
    WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM,
    WATANABE_POKEMON_CREATOR_VALUE_LEVEL,
    WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE,
    WATANABE_POKEMON_CREATOR_VALUE_OT_ID,
    WATANABE_POKEMON_CREATOR_VALUE_PERSONALITY,
    WATANABE_POKEMON_CREATOR_VALUE_GENDER,
    WATANABE_POKEMON_CREATOR_VALUE_NATURE,
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_1,
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_2,
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_3,
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_4,
    WATANABE_POKEMON_CREATOR_VALUE_HELD_ITEM,
    WATANABE_POKEMON_CREATOR_VALUE_ABILITY_SLOT,
    WATANABE_POKEMON_CREATOR_VALUE_HP_IV,
    WATANABE_POKEMON_CREATOR_VALUE_HP_EV,
    WATANABE_POKEMON_CREATOR_VALUE_ATTACK_IV,
    WATANABE_POKEMON_CREATOR_VALUE_ATTACK_EV,
    WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_IV,
    WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_EV,
    WATANABE_POKEMON_CREATOR_VALUE_SPEED_IV,
    WATANABE_POKEMON_CREATOR_VALUE_SPEED_EV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_IV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_EV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_IV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_EV,
    WATANABE_POKEMON_CREATOR_VALUE_COOL,
    WATANABE_POKEMON_CREATOR_VALUE_BEAUTY,
    WATANABE_POKEMON_CREATOR_VALUE_CUTE,
    WATANABE_POKEMON_CREATOR_VALUE_SMART,
    WATANABE_POKEMON_CREATOR_VALUE_TOUGH,
    WATANABE_POKEMON_CREATOR_VALUE_SHEEN,
    WATANABE_POKEMON_CREATOR_VALUE_FRIENDSHIP,
    WATANABE_POKEMON_CREATOR_VALUE_POKERUS,
    WATANABE_POKEMON_CREATOR_VALUE_IS_EGG,
    WATANABE_POKEMON_CREATOR_VALUE_STATUS,
    WATANABE_POKEMON_CREATOR_VALUE_HP,
    WATANABE_POKEMON_CREATOR_VALUE_ATTACK,
    WATANABE_POKEMON_CREATOR_VALUE_DEFENSE,
    WATANABE_POKEMON_CREATOR_VALUE_SPEED,
    WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK,
    WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE,
};

#define WATANABE_POKEMON_CREATOR_FIELD_SPECIAL_STATS  0xFD
#define WATANABE_POKEMON_CREATOR_FIELD_PHYSICAL_STATS 0xFE

struct WatanabeGraphicsBrowserState {
    u16 graphicsId;
    u8 frontSpriteId;
    u8 backSpriteId;
    u8 iconSpriteId;
    u8 isColorPickerActive;
    u8 spritePaletteNum;
    u8 paletteColorIndex;
    u8 palettePulsePhase;
    u8 useShinyPalette;
    u8 selectedRgbComponent;
    struct PlttData selectedColor;
    u16 savedPalette[16];
};

#define eWatanabePokemonCreatorState (*(struct WatanabePokemonCreatorState *)(gSharedMem + 0x18000))
#define eWatanabeGraphicsBrowserState (*(struct WatanabeGraphicsBrowserState *)(gSharedMem + 0x18000))

EWRAM_DATA u8 gWatanabeSizeComparisonPalette[0x20] = { 0 };
EWRAM_DATA u8 gWatanabeBattleDebugState[0x10] = { 0 };
EWRAM_DATA struct WatanabePokemonCreatorState *gWatanabePokemonCreatorState = NULL;
EWRAM_DATA struct WatanabeGraphicsBrowserState *gWatanabeGraphicsBrowserState = NULL;

u32 gWatanabeTrainerAiFlags;

#define sWatanabeBattleDebugSelectingEnemyParty gWatanabeBattleDebugState[0]
#define sWatanabeBattleDebugPlayerPartyIndex    gWatanabeBattleDebugState[1]
#define sWatanabeBattleDebugEnemyPartyIndex     gWatanabeBattleDebugState[2]
#define sWatanabeBattleDebugBattleModeIndex     gWatanabeBattleDebugState[3]
#define sWatanabeBattleDebugTrainerAiMenuOffset gWatanabeBattleDebugState[4]
#define sWatanabeBattleDebugTrainerAiCursor     gWatanabeBattleDebugState[5]
#define sWatanabeBattleDebugTerrainMenuOffset   gWatanabeBattleDebugState[6]
#define sWatanabeBattleDebugTerrainCursor       gWatanabeBattleDebugState[7]
#define sWatanabeBattleDebugGenderSelection     gWatanabeBattleDebugState[8]
#define sWatanabeBattleDebugBattlerCount        gWatanabeBattleDebugState[12]
#define sWatanabeBattleDebugSavedPlayerGender   gWatanabeBattleDebugState[13]
extern s8 gBattleEnvironment;

void Task_WatanabeDebugInitSizeComparison(u8 taskId);
void Task_WatanabeDebugHandleSizeComparisonInput(u8 taskId);
void WatanabeDebug_InitBattleSetupScreen(u8);
void Task_WatanabeDebugInitPartySelection(u8 taskId);
void Task_WatanabeDebugShowPlayerPartyCursor(u8 taskId);
void Task_WatanabeDebugHandlePlayerPartyInput(u8 taskId);
void Task_WatanabeDebugShowEnemyPartyCursor(u8 taskId);
void Task_WatanabeDebugHandleEnemyPartyInput(u8 taskId);
void WatanabeDebug_DrawPlayerParty(void);
void WatanabeDebug_DrawEnemyParty(void);
bool8 WatanabeDebug_HasEnoughBattleMons(void);
void Task_WatanabeDebugInitBattleModeMenu(u8 taskId);
void Task_WatanabeDebugShowBattleModeCursor(u8 taskId);
void Task_WatanabeDebugHandleBattleModeInput(u8 taskId);
void Task_WatanabeDebugShowTrainerAiCursor(u8 taskId);
void Task_WatanabeDebugHandleTrainerAiInput(u8 taskId);
void WatanabeDebug_DrawTrainerAiOptions(u8);
void Task_WatanabeDebugInitBattleTerrainMenu(u8 taskId);
void Task_WatanabeDebugShowBattleTerrainCursor(u8 taskId);
void Task_WatanabeDebugHandleBattleTerrainInput(u8 taskId);
void WatanabeDebug_DrawBattleTerrainOptions(u8);
void Task_WatanabeDebugInitGenderMenu(u8 taskId);
void Task_WatanabeDebugShowGenderCursor(u8 taskId);
void Task_WatanabeDebugHandleGenderInput(u8 taskId);
void Task_WatanabeDebugStartBattle(u8 taskId);
void Task_WatanabeDebugStartBattleStage2(u8 taskId);
void Task_WatanabeDebugStartBattleStage3(u8 taskId);
void Task_WatanabeDebugStartBattleStage4(u8 taskId);
void Task_WatanabeDebugStartBattleStage5(u8 taskId);
void Task_WatanabeDebugLaunchBattle(u8 taskId);
void Task_WatanabeDebugInitPokemonCreator(u8 taskId);
void Task_WatanabeDebugDrawPokemonCreator(u8 taskId);
void Task_WatanabeDebugHandlePokemonCreatorInput(u8 taskId);
void Task_WatanabeDebugHandlePokemonFieldInput(u8 taskId);
void Task_WatanabeDebugWaitForPokemonCreated(u8 taskId);
void WatanabeDebug_RecreatePokemon(void);
void WatanabeDebug_RecalculatePokemonForExperience(void);
void WatanabeDebug_RecalculatePokemonStats(void);
void WatanabeDebug_CreatePokemon(void);
void WatanabeDebug_MovePokemonCreatorCursor(u8);
void WatanabeDebug_DrawPokemonCreatorFields(void);
void WatanabeDebug_RedrawPokemonCreatorFields(void);
void WatanabeDebug_AdjustPokemonCreatorValue(u8);
void WatanabeDebug_CopyText(u8 *, const u8 *, u8);
void WatanabeDebug_AppendColoredNumber(u8 *, u32, u8, u8);
void WatanabeDebug_PrintHexValue(u16, u8, u8, u8);
void Task_WatanabeDebugInitPokemonGraphicsBrowser(u8 taskId);
void Task_WatanabeDebugLoadPokemonGraphics(u8 taskId);
void Task_WatanabeDebugRefreshPokemonPalette(u8 taskId);
void Task_WatanabeDebugHandlePokemonGraphicsInput(u8 taskId);
void Task_WatanabeDebugReloadPokemonGraphics(u8 taskId);
void Task_WatanabeDebugHandlePokemonColorInput(u8 taskId);
void Task_WatanabeDebugApplyPokemonPaletteColor(u8 taskId);
void SpriteCB_WatanabeDebugColorPicker(struct Sprite *);
void Task_WatanabeDebugInitTrainerGraphicsBrowser(u8 taskId);
void Task_WatanabeDebugLoadTrainerGraphics(u8 taskId);
void Task_WatanabeDebugRefreshTrainerPalette(u8 taskId);
void Task_WatanabeDebugHandleTrainerGraphicsInput(u8 taskId);
void Task_WatanabeDebugReloadTrainerGraphics(u8 taskId);
void Task_WatanabeDebugHandleTrainerColorInput(u8 taskId);
void Task_WatanabeDebugApplyTrainerPaletteColor(u8 taskId);

#define SPRITETAG_WATANABE 0x1000

u8 sWatanabeDebugSpriteBrowserTiles[];
u16 sWatanabeDebugSpriteBrowserPalette[];

struct SpriteSheet sWatanabeDebugSpriteBrowserSpriteSheets[] = {
    {sWatanabeDebugSpriteBrowserTiles, 0x800, SPRITETAG_WATANABE},
    {}
};
struct SpritePalette sWatanabeDebugSpriteBrowserSpritePalettes[] = {
    {sWatanabeDebugSpriteBrowserPalette, SPRITETAG_WATANABE},
    {}
};

void SpriteCB_WatanabeDebugDummy(struct Sprite *sprite)
{

}

void WatanabeDebug_InitScreen(void)
{
    SetVBlankCallback(NULL);
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
    DmaFill16Large(3, 0, VRAM, VRAM_SIZE, 0x1000);
    DmaFill32Defvars(3, 0, OAM, OAM_SIZE);
    DmaFill16Defvars(3, 0, PLTT, PLTT_SIZE);
    ResetPaletteFade();
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade(); // I really don't want palette fade here
    FreeAllSpritePalettes();
    LoadSpriteSheet(sWatanabeDebugSpriteBrowserSpriteSheets);
    LoadSpritePalette(sWatanabeDebugSpriteBrowserSpritePalettes);
    Text_LoadWindowTemplate(&gMenuTextWindowLoadTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
}

void CB2_WatanabeDebugRun(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void CB2_WatanabeDebugWaitForFade(void)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_WatanabeDebugRun);
    }

    else
    {
        AnimateSprites();
        BuildOamBuffer();
        UpdatePaletteFade();
    }
}

void Task_WatanabeDebugExitToField(u8 taskId)
{
    DestroyTask(taskId);
    UnlockPlayerFieldControls();
    SetMainCallback2(CB2_ReturnToFieldFadeFromBlack);
}

void VBlankCB_WatanabeDebug(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void WatanabeDebug_PrintDecimal3(u16 value, u8 x, u8 y)
{
    u8 i;
    u8 digit;
    u8 text[4];
    bool8 hasHundredsDigit;

    for (i = 0; i < 3; i++)
        text[i] = CHAR_SPACE;
    text[3] = EOS;

    hasHundredsDigit = FALSE;
    digit = value / 100;
    if (digit)
    {
        text[0] = CHAR_0 + digit;
        hasHundredsDigit = TRUE;
    }

    digit = (value % 100) / 10;
    if (digit || hasHundredsDigit)
    {
        text[1] = CHAR_0 + digit;
    }

    text[2] = CHAR_0 + ((value % 100) % 10);

    Menu_PrintText(text, x, y);
}

void WatanabeDebug_PrintSpeciesName(u16 species, u8 x, u8 y)
{
    u8 speciesName[11];
    u8 i;

    for (i = 0; i < 10; i++)
        speciesName[i] = CHAR_SPACE;
    speciesName[i] = EOS;

    for (i = 0; gSpeciesNames[species][i] != EOS && i < 10; i++)
        speciesName[i] = gSpeciesNames[species][i];

    Menu_PrintText(speciesName, x, y);
}

u16 WatanabeDebug_AdvanceSpeciesId(u8 direction, u16 species)
{
    switch (direction)
    {
        default:
        case 0:
            if (species > SPECIES_BULBASAUR)
                species--;
            else
                species = NUM_SPECIES - 1;
            break;
        case 1:
            if (species < NUM_SPECIES - 1)
                species++;
            else
                species = SPECIES_BULBASAUR;
            break;
    }
    return species;
}

u16 WatanabeDebug_AdvanceTrainerGraphicsId(u8 direction, u16 graphicsId)
{
    switch (direction)
    {
        default:
        case 0:
            if (graphicsId)
                graphicsId--;
            else
                graphicsId = 0x52;
            break;
        case 1:
            if (graphicsId < 0x52)
                graphicsId++;
            else
                graphicsId = 0;
            break;
    }
    return graphicsId;
}

void WatanabeDebug_DrawCursor(u8 cursorY, u8 left, u8 top, u8 bottom, bool8 showCursor)
{
    u8 cursor[] = _("▶");
    Menu_BlankWindowRect(left, top, left, bottom);
    if (showCursor)
        Menu_PrintText(cursor, left, cursorY);
}

const u8 sWatanabeDebugFlashSignature[] = {0x25, 0x20, 0x01, 0x08};

void WatanabeDebug_WriteFlashSignature(void)
{
    ProgramFlashSectorAndVerify(30, (void *)sWatanabeDebugFlashSignature);
}

void InitWatanabeDebugMenu(void)
{
    switch (gMain.state)
    {
        default:
        case 0:
            SaveMapView();
            WriteSaveBlock2();
            gMain.state = 1;
            break;
        case 1:
            if (WriteSaveBlock1Sector())
                gMain.state++;
            break;
        case 2:
            PlaySE(SE_SAVE);
            gMain.state++;
            break;
        case 3:
            if (!IsSEPlaying())
            {
                UnlockPlayerFieldControls();
                SetMainCallback2(CB2_ReturnToFieldFadeFromBlack);
            }
            break;
    }
}

void InitSizeComparison(void)
{
    u8 taskId;
    u16 imeBak;
    WatanabeDebug_InitScreen();
    gReservedSpritePaletteCount = 1;
    LoadPalette(gWatanabeSizeComparisonPalette, 0x100, 0x20);

    imeBak = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = imeBak;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    SetVBlankCallback(VBlankCB_WatanabeDebug);
    SetMainCallback2(CB2_WatanabeDebugWaitForFade);
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_OBJ_ON;
    taskId = CreateTask(Task_WatanabeDebugInitSizeComparison, 0);
    gTasks[taskId].data[0] = 0xfc;
}

const u8 sText_WatanabeSizeComparisonScale[] = DTR("ひりつ", "SCALE");
const u8 sText_WatanabeSizeComparisonHeight[] = DTR("たかさ", "HEIGHT");
const u8 sText_WatanabeSizeComparisonShrink[] = DTR("Lキー▶しゅくしょう", "{LEFT_ARROW}: SHRINK");
const u8 sText_WatanabeSizeComparisonEnlarge[] = DTR("Rキー▶かくだい", "{RIGHT_ARROW}: ENLARGE");
const u8 sText_WatanabeSizeComparisonMoveUp[] = DTR("Uキー▶うえ　いどう", "{UP_ARROW}: MOVE UP");
const u8 sText_WatanabeSizeComparisonMoveDown[] = DTR("Dキー▶した　いどう", "{DOWN_ARROW}: MOVE DOWN");
const u8 sText_WatanabeSizeComparisonSwitchControl[] = DTR("Aボタン▶そうさきりかえ", "A: SWITCH CONTROL");
const u8 sText_WatanabeSizeComparisonQuit[] = DTR("Bボタン▶やめる", "B: QUIT");
const u8 sText_WatanabeSizeComparisonStart[] = DTR("スタート:", "START:");
const u8 sText_WatanabeSizeComparisonSelect[] = DTR(":セレクト", ":SELECT");

#ifdef NONMATCHING
void Task_WatanabeDebugInitSizeComparison(u8 taskId)
{
    u16 natDexNum = gTasks[taskId].data[0];
    u16 species = NationalPokedexNumToSpecies(natDexNum);
    u16 spriteId;
    u16 spriteId2;
    struct SaveBlock2 *saveBlock2 = &gSaveBlock2;

    Menu_DrawStdWindowFrame(6, 0, 23, 9);

    Menu_DrawStdWindowFrame(0, 0, 5, 9);
    Menu_PrintText(sText_WatanabeSizeComparisonScale, 1, 1);
    Menu_PrintText(sText_WatanabeSizeComparisonHeight, 1, 5);

    Menu_DrawStdWindowFrame(24, 0, 29, 9);
    Menu_PrintText(sWatanabeDebugFlashSignature, 25, 1);
    Menu_PrintText(sText_WatanabeSizeComparisonHeight, 25, 5);

    Menu_DrawStdWindowFrame(0, 10, 11, 19);
    Menu_PrintText(sText_WatanabeSizeComparisonShrink, 1, 11);
    Menu_PrintText(sText_WatanabeSizeComparisonEnlarge, 1, 13);
    Menu_PrintText(sText_WatanabeSizeComparisonMoveUp, 1, 15);
    Menu_PrintText(sText_WatanabeSizeComparisonMoveDown, 1, 17);

    Menu_DrawStdWindowFrame(12, 10, 29, 15);
    Menu_PrintText(sText_WatanabeSizeComparisonSwitchControl, 13, 11);
    Menu_PrintText(sText_WatanabeSizeComparisonQuit, 13, 13);

    Menu_DrawStdWindowFrame(12, 16, 29, 19);
    Menu_PrintText(sText_WatanabeSizeComparisonStart, 13, 17);
    Menu_PrintText(sText_WatanabeSizeComparisonSelect, 24, 17);

    WatanabeDebug_PrintSpeciesName(species, 18, 17);

    spriteId = CreateSizeScreenTrainerPic(saveBlock2->playerGender, 0x98, 0x28, 0);
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
    gSprites[spriteId].oam.matrixNum = 2;
    gSprites[spriteId].oam.priority = 0;
    gSprites[spriteId].oam.paletteNum = 0;
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(0));
    gSprites[spriteId].y2 = gPokedexEntries[natDexNum].trainerOffset;
    SetOamMatrix(2, gPokedexEntries[natDexNum].trainerScale, 0, 0, gPokedexEntries[natDexNum].trainerScale);
    gTasks[taskId].data[3] = spriteId;
    gTasks[taskId].data[4] = gPokedexEntries[natDexNum].trainerOffset;
    gTasks[taskId].data[6] = gPokedexEntries[natDexNum].trainerScale;

    spriteId2 = CreateMonSpriteFromNationalDexNumber(natDexNum, 0x58, 0x28, 1);
    gSprites[spriteId2].oam.affineMode = ST_OAM_AFFINE_NORMAL;
    gSprites[spriteId2].oam.matrixNum = 1;
    gSprites[spriteId2].oam.priority = 0;
    gSprites[spriteId2].oam.paletteNum = 0;
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(0));
    gSprites[spriteId2].y2 = gPokedexEntries[natDexNum].pokemonOffset;
    SetOamMatrix(1, gPokedexEntries[natDexNum].pokemonScale, 0, 0, gPokedexEntries[natDexNum].pokemonScale);
    gTasks[taskId].data[2] = spriteId2;
    gTasks[taskId].data[5] = gPokedexEntries[natDexNum].pokemonOffset;
    gTasks[taskId].data[7] = gPokedexEntries[natDexNum].pokemonScale;
    gTasks[taskId].data[8] = 0;
    gTasks[taskId].data[9] = 0;

    REG_WININ = 0x3F1F;
    REG_WINOUT = 0x001F;
    REG_WIN0H = ((gTasks[taskId].data[8] * 64 + 0x38) << 8) + (gTasks[taskId].data[8] * 64 + 0x78);
    REG_WIN0V = 0x0848;
    REG_WIN1H = 0x31BF;
    REG_WIN1V = 0x014F;
    REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BD | BLDCNT_EFFECT_DARKEN;
    REG_BLDALPHA = 0;
    REG_BLDY = 7;
    REG_DISPCNT |= DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
    gTasks[taskId].func = Task_WatanabeDebugHandleSizeComparisonInput;
}
#else
NAKED void Task_WatanabeDebugInitSizeComparison(u8 taskId)
{
    asm("\tpush\t{r4, r5, r6, r7, lr}\n"
        "\tmov\tr7, sl\n"
        "\tmov\tr6, r9\n"
        "\tmov\tr5, r8\n"
        "\tpush\t{r5, r6, r7}\n"
        "\tadd\tsp, sp, #0xffffffd8\n"
        "\tlsl\tr0, r0, #0x18\n"
        "\tlsr\tr0, r0, #0x18\n"
        "\tldr\tr1, ._56        @ gTasks\n"
        "\tlsl\tr2, r0, #0x2\n"
        "\tadd\tr0, r2, r0\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr7, r0, r1\n"
        "\tldrh\tr0, [r7, #0x8]\n"
        "\tstr\tr0, [sp, #0x4]\n"
        "\tbl\tNationalPokedexNumToSpecies\n"
        "\tmov\tr1, sp\n"
        "\tstrh\tr0, [r1, #0x8]\n"
        "\tadd\tr2, r0, #0\n"
        "\tlsl\tr2, r2, #0x10\n"
        "\tstr\tr2, [sp, #0xc]\n"
        "\tlsr\tr2, r2, #0x10\n"
        "\tmov\tr9, r2\n"
        "\tldr\tr3, ._56 + 4    @ gSaveBlock2\n"
        "\tmov\tr8, r3\n"
        "\tmov\tr0, #0x6\n"
        "\tmov\tr1, #0x0\n"
        "\tmov\tr2, #0x17\n"
        "\tmov\tr3, #0x9\n"
        "\tbl\tMenu_DrawStdWindowFrame\n"
        "\tmov\tr0, #0x0\n"
        "\tmov\tr1, #0x0\n"
        "\tmov\tr2, #0x5\n"
        "\tmov\tr3, #0x9\n"
        "\tbl\tMenu_DrawStdWindowFrame\n"
        "\tldr\tr5, ._56 + 8    @ sWatanabeDebugFlashSignature\n"
        "\tadd\tr0, r5, #0\n"
        "\tmov\tr1, #0x1\n"
        "\tmov\tr2, #0x1\n"
        "\tbl\tMenu_PrintText\n"
        "\tldr\tr4, ._56 + 12   @ sText_WatanabeSizeComparisonHeight\n"
        "\tadd\tr0, r4, #0\n"
        "\tmov\tr1, #0x1\n"
        "\tmov\tr2, #0x5\n"
        "\tbl\tMenu_PrintText\n"
        "\tmov\tr0, #0x18\n"
        "\tmov\tr1, #0x0\n"
        "\tmov\tr2, #0x1d\n"
        "\tmov\tr3, #0x9\n"
        "\tbl\tMenu_DrawStdWindowFrame\n"
        "\tadd\tr0, r5, #0\n"
        "\tmov\tr1, #0x19\n"
        "\tmov\tr2, #0x1\n"
        "\tbl\tMenu_PrintText\n"
        "\tadd\tr0, r4, #0\n"
        "\tmov\tr1, #0x19\n"
        "\tmov\tr2, #0x5\n"
        "\tbl\tMenu_PrintText\n"
        "\tmov\tr0, #0x0\n"
        "\tmov\tr1, #0xa\n"
        "\tmov\tr2, #0xb\n"
        "\tmov\tr3, #0x13\n"
        "\tbl\tMenu_DrawStdWindowFrame\n"
        "\tldr\tr0, ._56 + 16   @ sText_WatanabeSizeComparisonShrink\n"
        "\tmov\tr1, #0x1\n"
        "\tmov\tr2, #0xb\n"
        "\tbl\tMenu_PrintText\n"
        "\tldr\tr0, ._56 + 20   @ sText_WatanabeSizeComparisonEnlarge\n"
        "\tmov\tr1, #0x1\n"
        "\tmov\tr2, #0xd\n"
        "\tbl\tMenu_PrintText\n"
        "\tldr\tr0, ._56 + 24   @ sText_WatanabeSizeComparisonMoveUp\n"
        "\tmov\tr1, #0x1\n"
        "\tmov\tr2, #0xf\n"
        "\tbl\tMenu_PrintText\n"
        "\tldr\tr0, ._56 + 28   @ sText_WatanabeSizeComparisonMoveDown\n"
        "\tmov\tr1, #0x1\n"
        "\tmov\tr2, #0x11\n"
        "\tbl\tMenu_PrintText\n"
        "\tmov\tr0, #0xc\n"
        "\tmov\tr1, #0xa\n"
        "\tmov\tr2, #0x1d\n"
        "\tmov\tr3, #0xf\n"
        "\tbl\tMenu_DrawStdWindowFrame\n"
        "\tldr\tr0, ._56 + 32   @ sText_WatanabeSizeComparisonSwitchControl\n"
        "\tmov\tr1, #0xd\n"
        "\tmov\tr2, #0xb\n"
        "\tbl\tMenu_PrintText\n"
        "\tldr\tr0, ._56 + 36   @ sText_WatanabeSizeComparisonQuit\n"
        "\tmov\tr1, #0xd\n"
        "\tmov\tr2, #0xd\n"
        "\tbl\tMenu_PrintText\n"
        "\tmov\tr0, #0xc\n"
        "\tmov\tr1, #0x10\n"
        "\tmov\tr2, #0x1d\n"
        "\tmov\tr3, #0x13\n"
        "\tbl\tMenu_DrawStdWindowFrame\n"
        "\tldr\tr0, ._56 + 40   @ sText_WatanabeSizeComparisonStart\n"
        "\tmov\tr1, #0xd\n"
        "\tmov\tr2, #0x11\n"
        "\tbl\tMenu_PrintText\n"
        "\tldr\tr0, ._56 + 44   @ sText_WatanabeSizeComparisonSelect\n"
        "\tmov\tr1, #0x18\n"
        "\tmov\tr2, #0x11\n"
        "\tbl\tMenu_PrintText\n"
        "\tmov\tr0, r9\n"
        "\tmov\tr1, #0x12\n"
        "\tmov\tr2, #0x11\n"
        "\tbl\tWatanabeDebug_PrintSpeciesName\n"
        "\tmov\tr1, r8\n"
        "\tldrb\tr0, [r1, #0x8]\n"
        "\tmov\tr1, #0x98\n"
        "\tmov\tr2, #0x28\n"
        "\tmov\tr3, #0x0\n"
        "\tbl\tCreateSizeScreenTrainerPic\n"
        "\tmov\tr2, sp\n"
        "\tstrh\tr0, [r2, #0x10]\n"
        "\tadd\tr3, r0, #0\n"
        "\tlsl\tr3, r3, #0x10\n"
        "\tstr\tr3, [sp, #0x18]\n"
        "\tlsr\tr0, r3, #0x10\n"
        "\tstr\tr0, [sp, #0x14]\n"
        "\tlsl\tr4, r0, #0x4\n"
        "\tadd\tr4, r4, r0\n"
        "\tlsl\tr4, r4, #0x2\n"
        "\tldr\tr1, ._56 + 48   @ gSprites\n"
        "\tadd\tr4, r4, r1\n"
        "\tldrb\tr1, [r4, #0x1]\n"
        "\tmov\tr2, #0x4\n"
        "\tneg\tr2, r2\n"
        "\tmov\tsl, r2\n"
        "\tmov\tr0, sl\n"
        "\tand\tr0, r0, r1\n"
        "\tmov\tr3, #0x1\n"
        "\torr\tr0, r0, r3\n"
        "\tstrb\tr0, [r4, #0x1]\n"
        "\tldrb\tr1, [r4, #0x3]\n"
        "\tmov\tr0, #0x3f\n"
        "\tneg\tr0, r0\n"
        "\tmov\tr9, r0\n"
        "\tand\tr0, r0, r1\n"
        "\tmov\tr1, #0x4\n"
        "\torr\tr0, r0, r1\n"
        "\tstrb\tr0, [r4, #0x3]\n"
        "\tldrb\tr1, [r4, #0x5]\n"
        "\tsub\tr2, r2, #0x9\n"
        "\tmov\tr8, r2\n"
        "\tmov\tr0, r8\n"
        "\tand\tr0, r0, r1\n"
        "\tmov\tr3, #0xf\n"
        "\tand\tr0, r0, r3\n"
        "\tstrb\tr0, [r4, #0x5]\n"
        "\tmov\tr0, #0x0\n"
        "\tbl\tGetSpritePaletteTagByPaletteNum\n"
        "\tlsl\tr0, r0, #0x10\n"
        "\tlsr\tr0, r0, #0x10\n"
        "\tbl\tFreeSpritePaletteByTag\n"
        "\tldr\tr0, ._56 + 52   @ gPokedexEntries\n"
        "\tldr\tr1, [sp, #0x4]\n"
        "\tlsl\tr5, r1, #0x3\n"
        "\tadd\tr5, r5, r1\n"
        "\tlsl\tr5, r5, #0x2\n"
        "\tadd\tr5, r5, r0\n"
        "\tldrh\tr6, [r5, #0x20]\n"
        "\tstrh\tr6, [r4, #0x26]\n"
        "\tldrh\tr4, [r5, #0x1e]\n"
        "\tstr\tr4, [sp]\n"
        "\tmov\tr0, #0x2\n"
        "\tadd\tr1, r4, #0\n"
        "\tmov\tr2, #0x0\n"
        "\tmov\tr3, #0x0\n"
        "\tbl\tSetOamMatrix\n"
        "\tmov\tr2, sp\n"
        "\tldrh\tr2, [r2, #0x14]\n"
        "\tstrh\tr2, [r7, #0xe]\n"
        "\tstrh\tr6, [r7, #0x10]\n"
        "\tstrh\tr4, [r7, #0x14]\n"
        "\tldr\tr0, [sp, #0x4]\n"
        "\tmov\tr1, #0x58\n"
        "\tmov\tr2, #0x28\n"
        "\tmov\tr3, #0x1\n"
        "\tbl\tCreateMonSpriteFromNationalDexNumber\n"
        "\tmov\tr3, sp\n"
        "\tstrh\tr0, [r3, #0x1c]\n"
        "\tlsl\tr0, r0, #0x10\n"
        "\tstr\tr0, [sp, #0x24]\n"
        "\tlsr\tr1, r0, #0x10\n"
        "\tstr\tr1, [sp, #0x20]\n"
        "\tlsl\tr4, r1, #0x4\n"
        "\tadd\tr4, r4, r1\n"
        "\tlsl\tr4, r4, #0x2\n"
        "\tldr\tr2, ._56 + 48   @ gSprites\n"
        "\tadd\tr4, r4, r2\n"
        "\tldrb\tr0, [r4, #0x1]\n"
        "\tmov\tr3, sl\n"
        "\tand\tr3, r3, r0\n"
        "\tmov\tr0, #0x1\n"
        "\torr\tr3, r3, r0\n"
        "\tstrb\tr3, [r4, #0x1]\n"
        "\tldrb\tr0, [r4, #0x3]\n"
        "\tmov\tr1, r9\n"
        "\tand\tr1, r1, r0\n"
        "\tmov\tr0, #0x2\n"
        "\torr\tr1, r1, r0\n"
        "\tstrb\tr1, [r4, #0x3]\n"
        "\tldrb\tr0, [r4, #0x5]\n"
        "\tmov\tr2, r8\n"
        "\tand\tr2, r2, r0\n"
        "\tmov\tr3, #0xf\n"
        "\tand\tr2, r2, r3\n"
        "\tstrb\tr2, [r4, #0x5]\n"
        "\tmov\tr0, #0x0\n"
        "\tbl\tGetSpritePaletteTagByPaletteNum\n"
        "\tlsl\tr0, r0, #0x10\n"
        "\tlsr\tr0, r0, #0x10\n"
        "\tbl\tFreeSpritePaletteByTag\n"
        "\tldrh\tr0, [r5, #0x1c]\n"
        "\tmov\tr8, r0\n"
        "\tstrh\tr0, [r4, #0x26]\n"
        "\tldrh\tr4, [r5, #0x1a]\n"
        "\tstr\tr4, [sp]\n"
        "\tmov\tr0, #0x1\n"
        "\tadd\tr1, r4, #0\n"
        "\tmov\tr2, #0x0\n"
        "\tmov\tr3, #0x0\n"
        "\tbl\tSetOamMatrix\n"
        "\tmov\tr1, sp\n"
        "\tldrh\tr1, [r1, #0x20]\n"
        "\tstrh\tr1, [r7, #0xc]\n"
        "\tmov\tr2, r8\n"
        "\tstrh\tr2, [r7, #0x12]\n"
        "\tstrh\tr4, [r7, #0x16]\n"
        "\tmov\tr3, #0x0\n"
        "\tstrh\tr3, [r7, #0x18]\n"
        "\tstrh\tr3, [r7, #0x1a]\n"
        "\tldr\tr1, ._56 + 56   @ 0x4000048\n"
        "\tldr\tr2, ._56 + 60   @ 0x3f1f\n"
        "\tadd\tr0, r2, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tadd\tr1, r1, #0x2\n"
        "\tmov\tr0, #0x1f\n"
        "\tstrh\tr0, [r1]\n"
        "\tldr\tr2, ._56 + 64   @ 0x4000040\n"
        "\tmov\tr3, #0x18\n"
        "\tldsh\tr1, [r7, r3]\n"
        "\tlsl\tr1, r1, #0x6\n"
        "\tadd\tr0, r1, #0\n"
        "\tadd\tr0, r0, #0x38\n"
        "\tlsl\tr0, r0, #0x8\n"
        "\tadd\tr1, r1, #0x78\n"
        "\tadd\tr0, r0, r1\n"
        "\tstrh\tr0, [r2]\n"
        "\tldr\tr1, ._56 + 68   @ 0x4000044\n"
        "\tldr\tr2, ._56 + 72   @ 0x848\n"
        "\tadd\tr0, r2, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tsub\tr1, r1, #0x2\n"
        "\tldr\tr3, ._56 + 76   @ 0x31bf\n"
        "\tadd\tr0, r3, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tadd\tr1, r1, #0x4\n"
        "\tldr\tr2, ._56 + 80   @ 0x14f\n"
        "\tadd\tr0, r2, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tadd\tr1, r1, #0xa\n"
        "\tmov\tr0, #0xe1\n"
        "\tstrh\tr0, [r1]\n"
        "\tldr\tr0, ._56 + 84   @ 0x4000052\n"
        "\tmov\tr3, #0x0\n"
        "\tstrh\tr3, [r0]\n"
        "\tadd\tr1, r1, #0x4\n"
        "\tmov\tr0, #0x7\n"
        "\tstrh\tr0, [r1]\n"
        "\tmov\tr2, #0x80\n"
        "\tlsl\tr2, r2, #0x13\n"
        "\tldrh\tr0, [r2]\n"
        "\tmov\tr3, #0xc0\n"
        "\tlsl\tr3, r3, #0x7\n"
        "\tadd\tr1, r3, #0\n"
        "\torr\tr0, r0, r1\n"
        "\tstrh\tr0, [r2]\n"
        "\tldr\tr0, ._56 + 88   @ Task_WatanabeDebugHandleSizeComparisonInput\n"
        "\tstr\tr0, [r7]\n"
        "\tadd\tsp, sp, #0x28\n"
        "\tpop\t{r3, r4, r5}\n"
        "\tmov\tr8, r3\n"
        "\tmov\tr9, r4\n"
        "\tmov\tsl, r5\n"
        "\tpop\t{r4, r5, r6, r7}\n"
        "\tpop\t{r0}\n"
        "\tbx\tr0\n"
        "._57:\n"
        "\t.align\t2, 0\n"
        "._56:\n"
        "\t.word\tgTasks\n"
        "\t.word\tgSaveBlock2\n"
        "\t.word\tsWatanabeDebugFlashSignature+0x4\n"
        "\t.word\tsText_WatanabeSizeComparisonHeight\n"
        "\t.word\tsText_WatanabeSizeComparisonShrink\n"
        "\t.word\tsText_WatanabeSizeComparisonEnlarge\n"
        "\t.word\tsText_WatanabeSizeComparisonMoveUp\n"
        "\t.word\tsText_WatanabeSizeComparisonMoveDown\n"
        "\t.word\tsText_WatanabeSizeComparisonSwitchControl\n"
        "\t.word\tsText_WatanabeSizeComparisonQuit\n"
        "\t.word\tsText_WatanabeSizeComparisonStart\n"
        "\t.word\tsText_WatanabeSizeComparisonSelect\n"
        "\t.word\tgSprites\n"
        "\t.word\tgPokedexEntries\n"
        "\t.word\t0x4000048\n"
        "\t.word\t0x3f1f\n"
        "\t.word\t0x4000040\n"
        "\t.word\t0x4000044\n"
        "\t.word\t0x848\n"
        "\t.word\t0x31bf\n"
        "\t.word\t0x14f\n"
        "\t.word\t0x4000052\n"
        "\t.word\tTask_WatanabeDebugHandleSizeComparisonInput+1");
}
#endif // NONMATCHING

void Task_WatanabeDebugHandleSizeComparisonInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        SetMainCallback2(CB2_WatanabeDebugWaitForFade);
        gTasks[taskId].func = Task_WatanabeDebugExitToField;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[8] ^= 1;
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        if (gTasks[taskId].data[8])
        {
            if (JOY_HELD(R_BUTTON))
                gTasks[taskId].data[6] += 16;
            else
                gTasks[taskId].data[6] += 1;
            if ((u16)gTasks[taskId].data[6] > 0x8000)
                gTasks[taskId].data[6] = 0x8000;
        }
        else
        {
            if (JOY_HELD(R_BUTTON))
                gTasks[taskId].data[7] += 16;
            else
                gTasks[taskId].data[7] += 1;
            if ((u16)gTasks[taskId].data[7] > 0x8000)
                gTasks[taskId].data[7] = 0x8000;
        }
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        if (gTasks[taskId].data[8])
        {
            if (JOY_HELD(R_BUTTON))
                gTasks[taskId].data[6] -= 16;
            else
                gTasks[taskId].data[6] -= 1;
            if (gTasks[taskId].data[6] < 0x100)
                gTasks[taskId].data[6] = 0x100;
        }
        else
        {
            if (JOY_HELD(R_BUTTON))
                gTasks[taskId].data[7] -= 16;
            else
                gTasks[taskId].data[7] -= 1;
            if (gTasks[taskId].data[7] < 0x100)
                gTasks[taskId].data[7] = 0x100;
        }
    }
    else if (JOY_REPT(DPAD_UP))
    {
        if (gTasks[taskId].data[8])
        {
            gTasks[taskId].data[4]--;
        }
        else
        {
            gTasks[taskId].data[5]--;
        }
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (gTasks[taskId].data[8])
        {
            gTasks[taskId].data[4]++;
        }
        else
        {
            gTasks[taskId].data[5]++;
        }
    }
    else if (JOY_REPT(START_BUTTON))
    {
        if (gTasks[taskId].data[0] < 0x182)
            gTasks[taskId].data[0]++;
        else
            gTasks[taskId].data[0] = 1;
        DestroySprite(gSprites + gTasks[taskId].data[2]);
        DestroySprite(gSprites + gTasks[taskId].data[3]);
        gTasks[taskId].func = Task_WatanabeDebugInitSizeComparison;
    }
    else if (JOY_REPT(SELECT_BUTTON))
    {
        if (gTasks[taskId].data[0] > 1)
            gTasks[taskId].data[0]--;
        else
            gTasks[taskId].data[0] = 0x182;
        DestroySprite(gSprites + gTasks[taskId].data[2]);
        DestroySprite(gSprites + gTasks[taskId].data[3]);
        gTasks[taskId].func = Task_WatanabeDebugInitSizeComparison;
    }
    else
    {
        u16 scale = gTasks[taskId].data[7];
        u16 offset = gTasks[taskId].data[5];
        SetOamMatrix(1, scale, 0, 0, scale);
        WatanabeDebug_PrintHexValue(scale, 1, 3, 4);
        WatanabeDebug_PrintHexValue(offset, 1, 7, 4);
        gSprites[gTasks[taskId].data[2]].y2 = offset;

        scale = gTasks[taskId].data[6];
        offset = gTasks[taskId].data[4];
        SetOamMatrix(2, scale, 0, 0, scale);
        WatanabeDebug_PrintHexValue(scale, 25, 3, 4);
        WatanabeDebug_PrintHexValue(offset, 25, 7, 4);
        gSprites[gTasks[taskId].data[3]].y2 = offset;

        REG_WIN0H = ((gTasks[taskId].data[8] * 64 + 0x38) << 8) + (gTasks[taskId].data[8] * 64 + 0x78);
    }
}

void WatanabeDebug_FormatSpeciesName(u8 * dest, u16 species)
{
    u8 i;

    for (i = 0; i < 10; i++)
        dest[i] = CHAR_SPACE;
    dest[i] = EOS;

    dest[0] = CHAR_HYPHEN;

    if (species != SPECIES_NONE)
    {
        for (i = 0; gSpeciesNames[species][i] != EOS && i < 10; i++)
            dest[i] = gSpeciesNames[species][i];
    }
}

void WatanabeDebug_RecalculatePartyStats(bool8 enemyParty)
{
    u8 i;
    u16 maxHp;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (enemyParty)
        {
            struct Pokemon *pokemon = gEnemyParty + i;
            if (GetMonData(pokemon, MON_DATA_SPECIES) != SPECIES_NONE)
            {
                CalculateMonStats(pokemon);
                maxHp = GetMonData(pokemon, MON_DATA_MAX_HP);
                SetMonData(pokemon, MON_DATA_HP, &maxHp);
            }
        }
        else
        {
            struct Pokemon *pokemon = gPlayerParty + i;
            if (GetMonData(pokemon, MON_DATA_SPECIES) != SPECIES_NONE)
            {
                CalculateMonStats(pokemon);
                maxHp = GetMonData(pokemon, MON_DATA_MAX_HP);
                SetMonData(pokemon, MON_DATA_HP, &maxHp);
            }
        }
    }
}

void InitBattleForDebug(void)
{
    SavePlayerParty();
    sWatanabeBattleDebugSavedPlayerGender = gSaveBlock2.playerGender;
    sWatanabeBattleDebugSelectingEnemyParty = 0;
    sWatanabeBattleDebugPlayerPartyIndex = 0;
    sWatanabeBattleDebugEnemyPartyIndex = 0;
    sWatanabeBattleDebugBattleModeIndex = 0;
    sWatanabeBattleDebugTrainerAiMenuOffset = 0;
    sWatanabeBattleDebugTrainerAiCursor = 0;
    sWatanabeBattleDebugTerrainMenuOffset = 0;
    sWatanabeBattleDebugTerrainCursor = 0;
    sWatanabeBattleDebugGenderSelection = 0;
    sWatanabeBattleDebugBattlerCount = 0;
    gWatanabeTrainerAiFlags = 0;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    WatanabeDebug_InitBattleSetupScreen(0);
    CreateTask(Task_WatanabeDebugInitBattleModeMenu, 0);
}

void WatanabeDebug_RestartBattleSetup(void)
{
    sWatanabeBattleDebugPlayerPartyIndex = 0;
    sWatanabeBattleDebugEnemyPartyIndex = 0;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    WatanabeDebug_InitBattleSetupScreen(0);
    CreateTask(Task_WatanabeDebugInitBattleModeMenu, 0);
}

const u8 sText_WatanabeBattleModeWild[] = DTR("やせいたいせん", "Wild BATTLE");
const u8 sText_WatanabeBattleModeSingle[] = DTR("1VS1たいせん", "1v1 BATTLE");
const u8 sText_WatanabeBattleModeDouble[] = DTR("2VS2たいせん", "2v2 BATTLE");
const u8 sText_WatanabeBattleModeLinkSingle[] = DTR("{COLOR BLUE}1VS1つうしんたいせん",
                                         "{COLOR BLUE}LINK 1v1 BATTLE");
const u8 sText_WatanabeBattleModeLinkDouble[] = DTR("{COLOR BLUE}2VS2つうしんたいせん",
                                         "{COLOR BLUE}LINK 2v2 BATTLE");

const struct WatanabeDebugMenuItem sWatanabeBattleModeItems[] = {
	{sText_WatanabeBattleModeWild, {.battleMode = {0x00, 0x01}}},
	{sText_WatanabeBattleModeSingle, {.battleMode = {0x08, 0x01}}},
	{sText_WatanabeBattleModeDouble, {.battleMode = {0x09, 0x02}}},
	{sText_WatanabeBattleModeLinkSingle, {.battleMode = {0x0A, 0x01}}},
	{sText_WatanabeBattleModeLinkDouble, {.battleMode = {0x0B, 0x02}}}
};

const u8 sText_WatanabeTrainerAiMode0[] = DTR("きほんタイプ", "Base type");
const u8 sText_WatanabeTrainerAiMode1[] = DTR("AIタイプ2", "AI type 2");
const u8 sText_WatanabeTrainerAiMode2[] = DTR("AIタイプ3", "AI type 3");
const u8 sText_WatanabeTrainerAiMode3[] = DTR("AIタイプ4", "AI type 4");
const u8 sText_WatanabeTrainerAiMode4[] = DTR("AIタイプ5", "AI type 5");
const u8 sText_WatanabeTrainerAiMode5[] = DTR("AIタイプ6", "AI type 6");
const u8 sText_WatanabeTrainerAiMode6[] = DTR("AIタイプ7", "AI type 7");
const u8 sText_WatanabeTrainerAiMode7[] = DTR("AIタイプ8", "AI type 8");
const u8 sText_WatanabeTrainerAiMode8[] = DTR("AIタイプ9", "AI type 9");
const u8 sText_WatanabeTrainerAiMode9[] = DTR("AIタイプ10", "AI type 10");

const struct WatanabeDebugMenuItem sWatanabeTrainerAiModeItems[] = {
	{sText_WatanabeTrainerAiMode0, {.trainerAiMask = 0x1}},
	{sText_WatanabeTrainerAiMode1, {.trainerAiMask = 0x2}},
	{sText_WatanabeTrainerAiMode2, {.trainerAiMask = 0x4}},
	{sText_WatanabeTrainerAiMode3, {.trainerAiMask = 0x8}},
	{sText_WatanabeTrainerAiMode4, {.trainerAiMask = 0x10}},
	{sText_WatanabeTrainerAiMode5, {.trainerAiMask = 0x20}},
	{sText_WatanabeTrainerAiMode6, {.trainerAiMask = 0x40}},
	{sText_WatanabeTrainerAiMode7, {.trainerAiMask = 0x80}},
	{sText_WatanabeTrainerAiMode8, {.trainerAiMask = 0x100}},
	{sText_WatanabeTrainerAiMode9, {.trainerAiMask = 0x200}}
};

const u8 sText_WatanabeBattleTerrainGrass[] = DTR("くさ", "GRASS");
const u8 sText_WatanabeBattleTerrainTallGrass[] = DTR("ながい　くさ", "TALL GRASS");
const u8 sText_WatanabeBattleTerrainSand[] = DTR("すな", "SAND");
const u8 sText_WatanabeBattleTerrainWater[] = DTR("みず", "WATER");
const u8 sText_WatanabeBattleTerrainSea[] = DTR("うみ", "SEA");
const u8 sText_WatanabeBattleTerrainPond[] = DTR("いけ", "POND");
const u8 sText_WatanabeBattleTerrainRocky[] = DTR("いわ", "ROCKY");
const u8 sText_WatanabeBattleTerrainCave[] = DTR("どうくつ", "CAVE");
const u8 sText_WatanabeBattleTerrainOther[] = DTR("そのた", "OTHER");

const struct WatanabeDebugMenuItem sWatanabeBattleTerrainItems[] = {
    {sText_WatanabeBattleTerrainGrass, {.battleEnvironment = 0x0}},
    {sText_WatanabeBattleTerrainTallGrass, {.battleEnvironment = 0x1}},
    {sText_WatanabeBattleTerrainSand, {.battleEnvironment = 0x2}},
    {sText_WatanabeBattleTerrainWater, {.battleEnvironment = 0x3}},
    {sText_WatanabeBattleTerrainSea, {.battleEnvironment = 0x4}},
    {sText_WatanabeBattleTerrainPond, {.battleEnvironment = 0x5}},
    {sText_WatanabeBattleTerrainRocky, {.battleEnvironment = 0x6}},
    {sText_WatanabeBattleTerrainCave, {.battleEnvironment = 0x7}},
    {sText_WatanabeBattleTerrainOther, {.battleEnvironment = 0x8}}
};

void WatanabeDebug_ReturnFromPlayerPartyEditor(void)
{
    WatanabeDebug_RecalculatePartyStats(0);
    WatanabeDebug_InitBattleSetupScreen(0);
    CreateTask(Task_WatanabeDebugInitPartySelection, 0);
}

void WatanabeDebug_ReturnFromEnemyPartyEditor(void)
{
    WatanabeDebug_RecalculatePartyStats(1);
    WatanabeDebug_InitBattleSetupScreen(1);
    CreateTask(Task_WatanabeDebugInitPartySelection, 0);
}

void WatanabeDebug_InitBattleSetupScreen(UNUSED u8 enemyParty)
{
    u16 imeBak;
    WatanabeDebug_InitScreen();
    REG_WIN0H = 0;
    REG_WIN0V = 0;
    REG_WIN1H = 0;
    REG_WIN1V = 0;
    REG_WININ = 0x1111;
    REG_WINOUT = 0x0031;
    REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BD | BLDCNT_EFFECT_DARKEN;
    REG_BLDALPHA = 0;
    REG_BLDY = 7;

    imeBak = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = imeBak;

    SetVBlankCallback(VBlankCB_WatanabeDebug);
    SetMainCallback2(CB2_WatanabeDebugWaitForFade);
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
    REG_WIN0H = 0x01EF;
    REG_WIN0V = 0x819F;
}

void Task_WatanabeDebugInitPartySelection(u8 taskId)
{
     u8 instructions[] = DTR("ポケモンを　えらんでね", "Choose POKéMON");
     u8 nextText[] = DTR("{COLOR RED}START:つぎへ", "{COLOR RED}START: NEXT");

    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(instructions, 1, 17);
    Menu_PrintText(nextText, 20, 17);

    WatanabeDebug_DrawPlayerParty();
    WatanabeDebug_DrawEnemyParty();

    if (sWatanabeBattleDebugSelectingEnemyParty)
        gTasks[taskId].func = Task_WatanabeDebugShowEnemyPartyCursor;
    else
        gTasks[taskId].func = Task_WatanabeDebugShowPlayerPartyCursor;
}

void Task_WatanabeDebugShowPlayerPartyCursor(u8 taskId)
{
    WatanabeDebug_DrawCursor(sWatanabeBattleDebugPlayerPartyIndex * 2 + 3, 1, 1, 14, 1);
    REG_WIN1H = 0x0177;
    REG_WIN1V = 0x017F;
    gTasks[taskId].func = Task_WatanabeDebugHandlePlayerPartyInput;
}

void Task_WatanabeDebugHandlePlayerPartyInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_OpenPokeStorageForMon(gPlayerParty + sWatanabeBattleDebugPlayerPartyIndex, WatanabeDebug_ReturnFromPlayerPartyEditor);
        DestroyTask(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_WatanabeDebugInitGenderMenu;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        if (WatanabeDebug_HasEnoughBattleMons())
        {
            PlaySE(SE_SELECT);
            CalculatePlayerPartyCount();
            CalculateEnemyPartyCount();
            gTasks[taskId].func = Task_WatanabeDebugInitBattleTerrainMenu;
        }
    }
    else if (JOY_NEW(R_BUTTON | DPAD_RIGHT))
    {
        WatanabeDebug_DrawCursor(0, 1, 1, 14, 0);
        gTasks[taskId].func = Task_WatanabeDebugShowEnemyPartyCursor;
    }
    else if (JOY_REPT(DPAD_UP) && sWatanabeBattleDebugPlayerPartyIndex != 0)
    {
        sWatanabeBattleDebugPlayerPartyIndex--;
        gTasks[taskId].func = Task_WatanabeDebugShowPlayerPartyCursor;
    }
    else if (JOY_REPT(DPAD_DOWN) && sWatanabeBattleDebugPlayerPartyIndex < 5 && GetMonData(gPlayerParty + sWatanabeBattleDebugPlayerPartyIndex, MON_DATA_SPECIES) != SPECIES_NONE)
    {
        sWatanabeBattleDebugPlayerPartyIndex++;
        gTasks[taskId].func = Task_WatanabeDebugShowPlayerPartyCursor;
    }
}

void Task_WatanabeDebugShowEnemyPartyCursor(u8 taskId)
{
    WatanabeDebug_DrawCursor(sWatanabeBattleDebugEnemyPartyIndex * 2 + 3, 16, 1, 14, 1);
    REG_WIN1H = 0x79EF;
    REG_WIN1V = 0x017F;
    gTasks[taskId].func = Task_WatanabeDebugHandleEnemyPartyInput;
}

void Task_WatanabeDebugHandleEnemyPartyInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Debug_OpenPokeStorageForMon(gEnemyParty + sWatanabeBattleDebugEnemyPartyIndex, WatanabeDebug_ReturnFromEnemyPartyEditor);
        DestroyTask(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_WatanabeDebugInitGenderMenu;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        if (WatanabeDebug_HasEnoughBattleMons())
        {
            PlaySE(SE_SELECT);
            CalculatePlayerPartyCount();
            CalculateEnemyPartyCount();
            gTasks[taskId].func = Task_WatanabeDebugInitBattleTerrainMenu;
        }
    }
    else if (JOY_NEW(L_BUTTON | DPAD_LEFT))
    {
        WatanabeDebug_DrawCursor(0, 16, 1, 14, 0);
        gTasks[taskId].func = Task_WatanabeDebugShowPlayerPartyCursor;
    }
    else if (JOY_REPT(DPAD_UP) && sWatanabeBattleDebugEnemyPartyIndex != 0)
    {
        sWatanabeBattleDebugEnemyPartyIndex--;
        gTasks[taskId].func = Task_WatanabeDebugShowEnemyPartyCursor;
    }
    else if (JOY_REPT(DPAD_DOWN) && sWatanabeBattleDebugEnemyPartyIndex < 5 && GetMonData(gEnemyParty + sWatanabeBattleDebugEnemyPartyIndex, MON_DATA_SPECIES) != SPECIES_NONE)
    {
        sWatanabeBattleDebugEnemyPartyIndex++;
        gTasks[taskId].func = Task_WatanabeDebugShowEnemyPartyCursor;
    }
}

void WatanabeDebug_DrawPlayerParty(void)
{
    u8 i;
    u8 partyTitle[] = DTR("{COLOR RED}じぶんの　ポケモン", "{COLOR RED}Enemy's POKéMON");
    u8 speciesName[POKEMON_NAME_LENGTH + 1];

    Menu_DrawStdWindowFrame(0, 0, 14, 15);
    Menu_PrintText(partyTitle, 2, 1);
    for (i = 0; i < PARTY_SIZE; i++)
    {
        WatanabeDebug_FormatSpeciesName(speciesName, GetMonData(gPlayerParty + i, MON_DATA_SPECIES));
        Menu_PrintText(speciesName, 2, 2 * i + 3);
    }
}

void WatanabeDebug_DrawEnemyParty(void)
{
    u8 i;
    u8 partyTitle[] = DTR("{COLOR RED}あいての　ポケモン", "{COLOR RED}Your POKéMON");
    u8 speciesName[POKEMON_NAME_LENGTH + 1];

    Menu_DrawStdWindowFrame(15, 0, 29, 15);
    Menu_PrintText(partyTitle, 17, 1);
    for (i = 0; i < PARTY_SIZE; i++)
    {
        WatanabeDebug_FormatSpeciesName(speciesName, GetMonData(gEnemyParty + i, MON_DATA_SPECIES));
        Menu_PrintText(speciesName, 17, 2 * i + 3);
    }
}

bool8 WatanabeDebug_HasEnoughBattleMons(void)
{
    u8 partyIndex = sWatanabeBattleDebugBattlerCount - 1;
    if (GetMonData(gPlayerParty + partyIndex, MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(gEnemyParty + partyIndex, MON_DATA_SPECIES) != SPECIES_NONE)
        return TRUE;
    return FALSE;
}

void Task_WatanabeDebugInitBattleModeMenu(u8 taskId)
{
    u8 i;
    u8 instructions[] = DTR("たいせんモードを　えらんでね", "Choose a battle mode");
    u8 nextText[] = DTR("{COLOR RED}START:つぎへ", "{COLOR RED}START: NEXT");
    u8 battleModeLabel[] = DTR("{COLOR RED}バトルモード　せんたく", "{COLOR RED}Battle mode:");
    u8 trainerAiLabel[] = DTR("{COLOR RED}トレーナーAI　せんたく", "{COLOR RED}TRAINER AI mode:");

    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(instructions, 1, 17);
    Menu_PrintText(nextText, 20, 17);

    Menu_DrawStdWindowFrame(0, 0, 14, 15);
    Menu_PrintText(battleModeLabel, 2, 1);

    for (i = 0; i < ARRAY_COUNT(sWatanabeBattleModeItems); i++)
        Menu_PrintText(sWatanabeBattleModeItems[i].label, 2, 2 * i + 3);

    Menu_DrawStdWindowFrame(15, 0, 29, 15);
    Menu_PrintText(trainerAiLabel, 17, 1);

    WatanabeDebug_DrawTrainerAiOptions(sWatanabeBattleDebugTrainerAiMenuOffset);

    gTasks[taskId].func = Task_WatanabeDebugShowBattleModeCursor;
}

void Task_WatanabeDebugShowBattleModeCursor(u8 taskId)
{
    WatanabeDebug_DrawCursor(2 * sWatanabeBattleDebugBattleModeIndex + 3, 1, 1, 14, 1);
    REG_WIN1H = 0x0177;
    REG_WIN1V = 0x017F;
    gTasks[taskId].func = Task_WatanabeDebugHandleBattleModeInput;
}

void Task_WatanabeDebugHandleBattleModeInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        SetMainCallback2(CB2_WatanabeDebugWaitForFade);
        LoadPlayerParty();
        gSaveBlock2.playerGender = sWatanabeBattleDebugSavedPlayerGender;
        gBattleTypeFlags = 0;
        gDebugBattleFlags = 0;
        gTasks[taskId].func = Task_WatanabeDebugExitToField;
    }
    else if (JOY_NEW(START_BUTTON) || JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gBattleTypeFlags = sWatanabeBattleModeItems[sWatanabeBattleDebugBattleModeIndex].data.battleMode.battleType;
        gDebugBattleFlags = 8;
        sWatanabeBattleDebugBattlerCount = sWatanabeBattleModeItems[sWatanabeBattleDebugBattleModeIndex].data.battleMode.battlerCount;
        if (sWatanabeBattleDebugBattleModeIndex == 1 || sWatanabeBattleDebugBattleModeIndex == 2)
        {
            WatanabeDebug_DrawCursor(0, 1, 1, 14, 0);
            gTasks[taskId].func = Task_WatanabeDebugShowTrainerAiCursor;
        }
        else
        {
            gTasks[taskId].func = Task_WatanabeDebugInitGenderMenu;
        }
    }
    else if (JOY_REPT(DPAD_UP) && sWatanabeBattleDebugBattleModeIndex > 0)
    {
        sWatanabeBattleDebugBattleModeIndex--;
        gTasks[taskId].func = Task_WatanabeDebugShowBattleModeCursor;
    }
    else if (JOY_REPT(DPAD_DOWN) && sWatanabeBattleDebugBattleModeIndex < 2)
    {
        sWatanabeBattleDebugBattleModeIndex++;
        gTasks[taskId].func = Task_WatanabeDebugShowBattleModeCursor;
    }
}

void Task_WatanabeDebugShowTrainerAiCursor(u8 taskId)
{
    WatanabeDebug_DrawCursor(sWatanabeBattleDebugTrainerAiCursor * 2 + 3, 16, 1, 14, 1);
    REG_WIN1H = 0x79EF;
    REG_WIN1V = 0x017F;
    gTasks[taskId].func = Task_WatanabeDebugHandleTrainerAiInput;
}

void Task_WatanabeDebugHandleTrainerAiInput(u8 taskId)
{
    u8 selectedOption = sWatanabeBattleDebugTrainerAiMenuOffset + sWatanabeBattleDebugTrainerAiCursor;
    
    if (JOY_NEW(A_BUTTON))
    {
        u32 mask = sWatanabeTrainerAiModeItems[selectedOption].data.trainerAiMask;
        if (gWatanabeTrainerAiFlags & mask)
            gWatanabeTrainerAiFlags &= (mask ^ 0xFFFF);
        else
            gWatanabeTrainerAiFlags |= mask;
        WatanabeDebug_DrawTrainerAiOptions(sWatanabeBattleDebugTrainerAiMenuOffset);
        gTasks[taskId].func = Task_WatanabeDebugShowTrainerAiCursor;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        WatanabeDebug_DrawCursor(0, 16, 1, 14, 0);
        gTasks[taskId].func = Task_WatanabeDebugShowBattleModeCursor;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTrainerBattleOpponent = SECRET_BASE_OPPONENT;
        sSecretBase->gender = 0;
        sSecretBase->playerName[0] = 0xAC;
        sSecretBase->playerName[1] = 0xFF;
        sSecretBase->trainerId[0] = 0;
        sSecretBase->trainerId[1] = 0;
        sSecretBase->trainerId[2] = 0;
        sSecretBase->trainerId[3] = 0;
        gTasks[taskId].func = Task_WatanabeDebugInitGenderMenu;
    }
    else if (JOY_REPT(DPAD_UP))
    {
        if (sWatanabeBattleDebugTrainerAiCursor > 0)
            sWatanabeBattleDebugTrainerAiCursor--;
        else if (sWatanabeBattleDebugTrainerAiMenuOffset > 0)
        {
            sWatanabeBattleDebugTrainerAiMenuOffset--;
            WatanabeDebug_DrawTrainerAiOptions(sWatanabeBattleDebugTrainerAiMenuOffset);
        }
        gTasks[taskId].func = Task_WatanabeDebugShowTrainerAiCursor;
    }
    else if (JOY_REPT(DPAD_DOWN) && sWatanabeBattleDebugTrainerAiCursor < 9)
    {
        if (sWatanabeBattleDebugTrainerAiCursor < 5)
            sWatanabeBattleDebugTrainerAiCursor++;
        else if (sWatanabeBattleDebugTrainerAiMenuOffset < 4)
        {
            sWatanabeBattleDebugTrainerAiMenuOffset++;
            WatanabeDebug_DrawTrainerAiOptions(sWatanabeBattleDebugTrainerAiMenuOffset);
        }
        gTasks[taskId].func = Task_WatanabeDebugShowTrainerAiCursor;
    }
}

void WatanabeDebug_DrawTrainerAiOptions(u8 menuOffset)
{
    u8 i;
    u8 j;
    u8 optionText[15];

    for (i = 0; i < 6; i++)
    {
        if (i + menuOffset < 10)
        {
            for (j = 0; sWatanabeTrainerAiModeItems[i + menuOffset].label[j] != EOS && j < 12; j++)
            {
                optionText[j + 3] = sWatanabeTrainerAiModeItems[i + menuOffset].label[j];
            }
            for (; j < 12; j++)
            {
                optionText[j + 3] = CHAR_SPACE;
            }
            optionText[14] = EOS;
            optionText[0] = EXT_CTRL_CODE_BEGIN;
            optionText[1] = 0x01;
            if (gWatanabeTrainerAiFlags & (1 << (i + menuOffset)))
                optionText[2] = 0x03;
            else
                optionText[2] = 0x01;
            Menu_PrintText(optionText, 17, 2 * i + 3);
        }
    }
}

void Task_WatanabeDebugInitBattleTerrainMenu(u8 taskId)
{
    u8 instructions[] = DTR("たいせんモードを　えらんでね", "Choose a battle mode");
    u8 beginText[] = DTR("{COLOR RED}START:かいし", "{COLOR RED}START: BEGIN");
    u8 battleTerrainLabel[] = DTR("{COLOR RED}バトルちけい　せんたく", "{COLOR RED}Battle terrain:");

    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(instructions, 1, 17);
    Menu_PrintText(beginText, 20, 17);

    Menu_DrawStdWindowFrame(0, 0, 14, 15);
    Menu_PrintText(battleTerrainLabel, 2, 1);
    WatanabeDebug_DrawBattleTerrainOptions(sWatanabeBattleDebugTerrainMenuOffset);

    Menu_DrawStdWindowFrame(15, 0, 29,15);
    gTasks[taskId].func = Task_WatanabeDebugShowBattleTerrainCursor;
}

void Task_WatanabeDebugShowBattleTerrainCursor(u8 taskId)
{
    WatanabeDebug_DrawCursor(2 * sWatanabeBattleDebugTerrainCursor + 3, 1, 1, 14, 1);
    REG_WIN1H = 0x0177;
    REG_WIN1V = 0x017F;
    gTasks[taskId].func = Task_WatanabeDebugHandleBattleTerrainInput;
}

void Task_WatanabeDebugHandleBattleTerrainInput(u8 taskId)
{
    u8 selectedTerrain = sWatanabeBattleDebugTerrainMenuOffset + sWatanabeBattleDebugTerrainCursor;
    if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_WatanabeDebugInitPartySelection;
    }
    else if (JOY_NEW(START_BUTTON) || JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gBattleEnvironment = sWatanabeBattleTerrainItems[selectedTerrain].data.battleEnvironment;
        gTasks[taskId].func = Task_WatanabeDebugStartBattle;
    }
    else if (JOY_REPT(DPAD_UP))
    {
        if (sWatanabeBattleDebugTerrainCursor > 0)
            sWatanabeBattleDebugTerrainCursor--;
        else if (sWatanabeBattleDebugTerrainMenuOffset > 0)
        {
            sWatanabeBattleDebugTerrainMenuOffset--;
            WatanabeDebug_DrawBattleTerrainOptions(sWatanabeBattleDebugTerrainMenuOffset);
        }
        gTasks[taskId].func = Task_WatanabeDebugShowBattleTerrainCursor;
    }
    else if (JOY_REPT(DPAD_DOWN) && sWatanabeBattleDebugTerrainCursor < 8)
    {
        if (sWatanabeBattleDebugTerrainCursor < 5)
            sWatanabeBattleDebugTerrainCursor++;
        else if (sWatanabeBattleDebugTerrainMenuOffset < 3)
        {
            sWatanabeBattleDebugTerrainMenuOffset++;
            WatanabeDebug_DrawBattleTerrainOptions(sWatanabeBattleDebugTerrainMenuOffset);
        }
        gTasks[taskId].func = Task_WatanabeDebugShowBattleTerrainCursor;
    }
}

void WatanabeDebug_DrawBattleTerrainOptions(u8 menuOffset)
{
    u8 i;
    Menu_BlankWindowRect(1, 3, 13, 14);

    for (i = 0; i < 6; i++)
    {
        if (i < 9)
            Menu_PrintText(sWatanabeBattleTerrainItems[i + menuOffset].label, 2, 2 * i + 3);
    }
}

void Task_WatanabeDebugInitGenderMenu(u8 taskId)
{
    u8 instructions[] = DTR("じぶんの　せいべつを　えらんでね", "Choose a gender");
    u8 nextText[] = DTR("{COLOR RED}START:つぎへ", "{COLOR RED}START: NEXT");
    u8 genderLabel[] = DTR("{COLOR RED}せいべつ　せんたく", "{COLOR RED}Gender:");
    u8 maleText[] = DTR("おとこ", "BOY");
    u8 femaleText[] = DTR("おんな", "GIRL");

    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(instructions, 1, 17);
    Menu_PrintText(nextText, 20, 17);

    Menu_DrawStdWindowFrame(0, 0, 14, 15);
    Menu_PrintText(genderLabel, 2, 1);

    Menu_BlankWindowRect(1, 3, 13, 14);
    Menu_PrintText(genderLabel, 2, 1);
    Menu_PrintText(maleText, 2, 3);
    Menu_PrintText(femaleText, 2, 5);

    Menu_DrawStdWindowFrame(15, 0, 29, 15);
    gTasks[taskId].func = Task_WatanabeDebugShowGenderCursor;
}

void Task_WatanabeDebugShowGenderCursor(u8 taskId)
{
    WatanabeDebug_DrawCursor(sWatanabeBattleDebugGenderSelection * 2 + 3, 1, 1, 14, 1);
    REG_WIN1H = 0x0177;
    REG_WIN1V = 0x017F;
    gTasks[taskId].func = Task_WatanabeDebugHandleGenderInput;
}

void Task_WatanabeDebugHandleGenderInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_WatanabeDebugInitBattleModeMenu;
    }
    else if (JOY_NEW(START_BUTTON) || JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSaveBlock2.playerGender = sWatanabeBattleDebugGenderSelection;
        gTasks[taskId].func = Task_WatanabeDebugInitPartySelection;
    }
    else if (JOY_REPT(DPAD_UP) && sWatanabeBattleDebugGenderSelection > 0)
    {
        sWatanabeBattleDebugGenderSelection--;
        gTasks[taskId].func = Task_WatanabeDebugShowGenderCursor;
    }
    else if (JOY_REPT(DPAD_DOWN) && sWatanabeBattleDebugGenderSelection < 1)
    {
        sWatanabeBattleDebugGenderSelection++;
        gTasks[taskId].func = Task_WatanabeDebugShowGenderCursor;
    }
}

void Task_WatanabeDebugStartBattle(u8 taskId)
{
    gTasks[taskId].func = Task_WatanabeDebugStartBattleStage2;
}

void Task_WatanabeDebugStartBattleStage2(u8 taskId)
{
    gTasks[taskId].func = Task_WatanabeDebugStartBattleStage3;
}

void Task_WatanabeDebugStartBattleStage3(u8 taskId)
{
    gTasks[taskId].func = Task_WatanabeDebugStartBattleStage4;
}

void Task_WatanabeDebugStartBattleStage4(u8 taskId)
{
    gTasks[taskId].func = Task_WatanabeDebugStartBattleStage5;
}

void Task_WatanabeDebugStartBattleStage5(u8 taskId)
{
    gTasks[taskId].func = Task_WatanabeDebugLaunchBattle;
}

void Task_WatanabeDebugLaunchBattle(u8 taskId)
{
    PlayBattleBGM();
    gMain.savedCallback = WatanabeDebug_RestartBattleSetup;
    SetMainCallback2(DebugInitBattleSprites);
}

void WatanabeDebug_InitPokemonCreatorState(void)
{
    u16 i;

    gWatanabePokemonCreatorState = &eWatanabePokemonCreatorState;

    for (i = 0; i < 64; i++)
    {
        gWatanabePokemonCreatorState->values[i] = 0;
    }

    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM] = 0xfc;
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_OT_ID] = gSaveBlock2.playerTrainerId[0] | (gSaveBlock2.playerTrainerId[1] << 8) | (gSaveBlock2.playerTrainerId[2] << 16) | (gSaveBlock2.playerTrainerId[3] << 24);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_PERSONALITY] = (Random() << 16) | Random();
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_LEVEL] = 1;
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE] = 1;
    WatanabeDebug_RecreatePokemon();
}

bool8 WatanabeDebug_AddCreatedPokemonToPartyOrPc(struct Pokemon *pokemon)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(gPlayerParty + i, MON_DATA_SPECIES, NULL) == SPECIES_NONE)
        {
            gPlayerParty[i] = *pokemon;
            gPlayerPartyCount = i + 1;
            return FALSE;
        }
    }
    SendMonToPC(&gWatanabePokemonCreatorState->pokemon);
    return TRUE;
}

const u8 sText_WatanabePokemonEditorSpecies[] = _("Kind");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSpecies = {1, 0x00000182, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorLevel[] = _("Level");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldLevel = {1, 0x00000064, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorExperience[] = _("EXP Points");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldExperience = {1, 0x001E8480, 0x00, 0x07};

const u8 sText_WatanabePokemonEditorOtId[] = _("ID");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldOtId = {0, 0xFFFFFFFF, 0x00, 0x0A};

const u8 sText_WatanabePokemonEditorPersonality[] = _("Rnd Numbers");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldPersonality = {0, 0xFFFFFFFF, 0x00, 0x0A};

const u8 sText_WatanabePokemonEditorGender[] = _("Sex");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldGender = {0, 0x00000002, 0x00, 0xFF};

const u8 sText_WatanabePokemonEditorNature[] = _("Character");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldNature = {0, 0x00000018, 0x00, 0xFF};

const u8 sText_WatanabePokemonEditorMove1[] = _("Move1");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldMove1 = {0, 0x00000162, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorMove2[] = _("Move2");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldMove2 = {0, 0x00000162, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorMove3[] = _("Move3");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldMove3 = {0, 0x00000162, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorMove4[] = _("Move4");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldMove4 = {0, 0x00000162, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorHeldItem[] = _("Hold");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldHeldItem = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorAbilitySlot[] = _("SP. Ability");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldAbilitySlot = {0, 0x00000001, 0x00, 0xFF};

const u8 sText_WatanabePokemonEditorHpIv[] = _("HPrnd");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldHpIv = {0, 0x0000001F, 0x00, 0x02};

const u8 sText_WatanabePokemonEditorHpEv[] = _("HPexp");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldHpEv = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorAttackIv[] = _("ATKrnd");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldAttackIv = {0, 0x0000001F, 0x00, 0x02};

const u8 sText_WatanabePokemonEditorAttackEv[] = _("ATKexp");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldAttackEv = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorDefenseIv[] = _("DEFrnd");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldDefenseIv = {0, 0x0000001F, 0x00, 0x02};

const u8 sText_WatanabePokemonEditorDefenseEv[] = _("DEFexp");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldDefenseEv = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorSpeedIv[] = _("SPDrnd");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSpeedIv = {0, 0x0000001F, 0x00, 0x02};

const u8 sText_WatanabePokemonEditorSpeedEv[] = _("SPDexp");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSpeedEv = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorSpAttackIv[] = _("SP. ATK$rnd");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSpAttackIv = {0, 0x0000001F, 0x00, 0x02};

const u8 sText_WatanabePokemonEditorSpAttackEv[] = _("SP. ATK$exp");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSpAttackEv = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorSpDefenseIv[] = _("SP. DEF$rnd");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSpDefenseIv = {0, 0x0000001F, 0x00, 0x02};

const u8 sText_WatanabePokemonEditorSpDefenseEv[] = _("SP. DEF$exp");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSpDefenseEv = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorCool[] = _("Cool");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldCool = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorBeauty[] = _("Beauty");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldBeauty = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorCute[] = _("Cute");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldCute = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorSmart[] = _("Smart");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSmart = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorTough[] = _("Tough");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldTough = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorSheen[] = _("Sheen");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldSheen = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorFriendship[] = _("Tameness");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldFriendship = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorPokerus[] = _("{PK}RUS");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldPokerus = {0, 0x000000FF, 0x00, 0x03};

const u8 sText_WatanabePokemonEditorIsEgg[] = _("EGG$");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldIsEgg = {0, 0x00000001, 0x00, 0xFF};

const u8 sText_WatanabePokemonEditorStatus[] = _("Status");
const struct WatanabeDebugPokemonEditorField sWatanabePokemonEditorFieldStatus = {0, 0x00000006, 0x00, 0xFF};

const u8 sText_WatanabePokemonEditorHp[] = _("{COLOR RED}HP　　");
const u8 sText_WatanabePokemonEditorAttack[] = _("{COLOR RED}ATK　");
const u8 sText_WatanabePokemonEditorDefense[] = _("{COLOR RED}DEF　");
const u8 sText_WatanabePokemonEditorSpeed[] = _("{COLOR RED}SPEED");
const u8 sText_WatanabePokemonEditorSpAttack[] = _("{COLOR RED}SP。ATK");
const u8 sText_WatanabePokemonEditorSpDefense[] = _("{COLOR RED}SP。DEF");
const u8 sText_WatanabePokemonEditorNone[] = DTR("なし", "without");
const u8 sText_WatanabePokemonEditorMale[] = _("♂");
const u8 sText_WatanabePokemonEditorFemale[] = _("♀");
const u8 sText_WatanabePokemonEditorDash[] = _("ー");
const u8 sText_WatanabePokemonEditorOn[] = _("ON");
const u8 sText_WatanabePokemonEditorOff[] = _("OFF");

const struct WatanabeDebugMenuItem sWatanabePokemonEditorFieldItems[] = {
    {sText_WatanabePokemonEditorSpecies, {.pokemonEditorField = &sWatanabePokemonEditorFieldSpecies}},
    {sText_WatanabePokemonEditorLevel, {.pokemonEditorField = &sWatanabePokemonEditorFieldLevel}},
    {sText_WatanabePokemonEditorExperience, {.pokemonEditorField = &sWatanabePokemonEditorFieldExperience}},
    {sText_WatanabePokemonEditorOtId, {.pokemonEditorField = &sWatanabePokemonEditorFieldOtId}},
    {sText_WatanabePokemonEditorPersonality, {.pokemonEditorField = &sWatanabePokemonEditorFieldPersonality}},
    {sText_WatanabePokemonEditorGender, {.pokemonEditorField = &sWatanabePokemonEditorFieldGender}},
    {sText_WatanabePokemonEditorNature, {.pokemonEditorField = &sWatanabePokemonEditorFieldNature}},
    {sText_WatanabePokemonEditorMove1, {.pokemonEditorField = &sWatanabePokemonEditorFieldMove1}},
    {sText_WatanabePokemonEditorMove2, {.pokemonEditorField = &sWatanabePokemonEditorFieldMove2}},
    {sText_WatanabePokemonEditorMove3, {.pokemonEditorField = &sWatanabePokemonEditorFieldMove3}},
    {sText_WatanabePokemonEditorMove4, {.pokemonEditorField = &sWatanabePokemonEditorFieldMove4}},
    {sText_WatanabePokemonEditorHeldItem, {.pokemonEditorField = &sWatanabePokemonEditorFieldHeldItem}},
    {sText_WatanabePokemonEditorAbilitySlot, {.pokemonEditorField = &sWatanabePokemonEditorFieldAbilitySlot}},
    {sText_WatanabePokemonEditorHpIv, {.pokemonEditorField = &sWatanabePokemonEditorFieldHpIv}},
    {sText_WatanabePokemonEditorHpEv, {.pokemonEditorField = &sWatanabePokemonEditorFieldHpEv}},
    {sText_WatanabePokemonEditorAttackIv, {.pokemonEditorField = &sWatanabePokemonEditorFieldAttackIv}},
    {sText_WatanabePokemonEditorAttackEv, {.pokemonEditorField = &sWatanabePokemonEditorFieldAttackEv}},
    {sText_WatanabePokemonEditorDefenseIv, {.pokemonEditorField = &sWatanabePokemonEditorFieldDefenseIv}},
    {sText_WatanabePokemonEditorDefenseEv, {.pokemonEditorField = &sWatanabePokemonEditorFieldDefenseEv}},
    {sText_WatanabePokemonEditorSpeedIv, {.pokemonEditorField = &sWatanabePokemonEditorFieldSpeedIv}},
    {sText_WatanabePokemonEditorSpeedEv, {.pokemonEditorField = &sWatanabePokemonEditorFieldSpeedEv}},
    {sText_WatanabePokemonEditorSpAttackIv, {.pokemonEditorField = &sWatanabePokemonEditorFieldSpAttackIv}},
    {sText_WatanabePokemonEditorSpAttackEv, {.pokemonEditorField = &sWatanabePokemonEditorFieldSpAttackEv}},
    {sText_WatanabePokemonEditorSpDefenseIv, {.pokemonEditorField = &sWatanabePokemonEditorFieldSpDefenseIv}},
    {sText_WatanabePokemonEditorSpDefenseEv, {.pokemonEditorField = &sWatanabePokemonEditorFieldSpDefenseEv}},
    {sText_WatanabePokemonEditorCool, {.pokemonEditorField = &sWatanabePokemonEditorFieldCool}},
    {sText_WatanabePokemonEditorBeauty, {.pokemonEditorField = &sWatanabePokemonEditorFieldBeauty}},
    {sText_WatanabePokemonEditorCute, {.pokemonEditorField = &sWatanabePokemonEditorFieldCute}},
    {sText_WatanabePokemonEditorSmart, {.pokemonEditorField = &sWatanabePokemonEditorFieldSmart}},
    {sText_WatanabePokemonEditorTough, {.pokemonEditorField = &sWatanabePokemonEditorFieldTough}},
    {sText_WatanabePokemonEditorSheen, {.pokemonEditorField = &sWatanabePokemonEditorFieldSheen}},
    {sText_WatanabePokemonEditorFriendship, {.pokemonEditorField = &sWatanabePokemonEditorFieldFriendship}},
    {sText_WatanabePokemonEditorPokerus, {.pokemonEditorField = &sWatanabePokemonEditorFieldPokerus}},
    {sText_WatanabePokemonEditorIsEgg, {.pokemonEditorField = &sWatanabePokemonEditorFieldIsEgg}},
    {sText_WatanabePokemonEditorStatus, {.pokemonEditorField = &sWatanabePokemonEditorFieldStatus}}
};

const u8 sWatanabePokemonEditorCategoryGeneral[] = {
    WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM,
    WATANABE_POKEMON_CREATOR_VALUE_LEVEL,
    WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE,
    WATANABE_POKEMON_CREATOR_VALUE_OT_ID,
    WATANABE_POKEMON_CREATOR_VALUE_PERSONALITY,
    WATANABE_POKEMON_CREATOR_VALUE_GENDER,
    WATANABE_POKEMON_CREATOR_VALUE_NATURE,
    EOS
};

const u8 sWatanabePokemonEditorCategoryMisc[] = {
    WATANABE_POKEMON_CREATOR_VALUE_STATUS,
    WATANABE_POKEMON_CREATOR_VALUE_FRIENDSHIP,
    WATANABE_POKEMON_CREATOR_VALUE_POKERUS,
    EOS
};

const u8 sWatanabePokemonEditorCategoryMoves[] = {
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_1,
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_2,
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_3,
    WATANABE_POKEMON_CREATOR_VALUE_MOVE_4,
    WATANABE_POKEMON_CREATOR_VALUE_HELD_ITEM,
    WATANABE_POKEMON_CREATOR_VALUE_ABILITY_SLOT,
    EOS
};

const u8 sWatanabePokemonEditorCategoryPhysicalStats[] = {
    WATANABE_POKEMON_CREATOR_VALUE_HP_IV,
    WATANABE_POKEMON_CREATOR_VALUE_HP_EV,
    WATANABE_POKEMON_CREATOR_VALUE_ATTACK_IV,
    WATANABE_POKEMON_CREATOR_VALUE_ATTACK_EV,
    WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_IV,
    WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_EV,
    WATANABE_POKEMON_CREATOR_FIELD_PHYSICAL_STATS,
    EOS
};

const u8 sWatanabePokemonEditorCategorySpecialStats[] = {
    WATANABE_POKEMON_CREATOR_VALUE_SPEED_IV,
    WATANABE_POKEMON_CREATOR_VALUE_SPEED_EV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_IV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_EV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_IV,
    WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_EV,
    WATANABE_POKEMON_CREATOR_FIELD_SPECIAL_STATS,
    EOS
};

const u8 sWatanabePokemonEditorCategoryContestStats[] = {
    WATANABE_POKEMON_CREATOR_VALUE_COOL,
    WATANABE_POKEMON_CREATOR_VALUE_BEAUTY,
    WATANABE_POKEMON_CREATOR_VALUE_CUTE,
    WATANABE_POKEMON_CREATOR_VALUE_SMART,
    WATANABE_POKEMON_CREATOR_VALUE_TOUGH,
    WATANABE_POKEMON_CREATOR_VALUE_SHEEN,
    EOS
};

const struct WatanabeDebugMenuItem sWatanabePokemonEditorCategories[] = {
    {sWatanabePokemonEditorCategoryGeneral, {.categoryLength = 0x5}},
    {sWatanabePokemonEditorCategoryMisc, {.categoryLength = 0x3}},
    {sWatanabePokemonEditorCategoryMoves, {.categoryLength = 0x6}},
    {sWatanabePokemonEditorCategoryPhysicalStats, {.categoryLength = 0x6}},
    {sWatanabePokemonEditorCategorySpecialStats, {.categoryLength = 0x6}},
    {sWatanabePokemonEditorCategoryContestStats, {.categoryLength = 0x6}}
};

#if (ENGLISH && REVISION == 0)
const u8 sText_WatanabePokemonEditorStatusHealthy[] = _("ふつう");
const u8 sText_WatanabePokemonEditorStatusSleep[] = _("ねむり");
const u8 sText_WatanabePokemonEditorStatusPoison[] = _("どく");
const u8 sText_WatanabePokemonEditorStatusBurn[] = _("やけど");
const u8 sText_WatanabePokemonEditorStatusFreeze[] = _("こおり");
const u8 sText_WatanabePokemonEditorStatusParalysis[] = _("まひ");
const u8 sText_WatanabePokemonEditorStatusToxic[] = _("どくどく");
#else
const u8 sText_WatanabePokemonEditorStatusHealthy[] = _("ーーー");
const u8 sText_WatanabePokemonEditorStatusSleep[] = _("SLP");
const u8 sText_WatanabePokemonEditorStatusPoison[] = _("PSN");
const u8 sText_WatanabePokemonEditorStatusBurn[] = _("BRN");
const u8 sText_WatanabePokemonEditorStatusFreeze[] = _("FRZ");
const u8 sText_WatanabePokemonEditorStatusParalysis[] = _("PAR");
const u8 sText_WatanabePokemonEditorStatusToxic[] = _("PSN2");
#endif

const struct WatanabeDebugMenuItem sWatanabePokemonEditorStatusItems[] = {
    {sText_WatanabePokemonEditorStatusHealthy, {.status = 0x0}},
    {sText_WatanabePokemonEditorStatusSleep, {.status = 0x1}},
    {sText_WatanabePokemonEditorStatusPoison, {.status = 0x8}},
    {sText_WatanabePokemonEditorStatusBurn, {.status = 0x10}},
    {sText_WatanabePokemonEditorStatusFreeze, {.status = 0x20}},
    {sText_WatanabePokemonEditorStatusParalysis, {.status = 0x40}},
    {sText_WatanabePokemonEditorStatusToxic, {.status = 0x80}}
};

#if (ENGLISH && REVISION == 0)
const u8 sText_WatanabePokemonAddedToParty[] = _("ポケモンを てもちに くわえました");
const u8 sText_WatanabePokemonAddedToPc[] = _("ポケモンを ボックスに くわえました");
#else
const u8 sText_WatanabePokemonAddedToParty[] = _("PKMN{RIGHT_ARROW}LIST");
const u8 sText_WatanabePokemonAddedToPc[] = _("PKMN{RIGHT_ARROW}BOX");
#endif
void InitCreatePokemon(void)
{
    u8 headerText[] = _("Create POKéMON   LR:Shift  START:Add");

    WatanabeDebug_InitScreen();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    REG_BLDCNT = 0x0000;
    REG_BLDALPHA = 0x0000;
    REG_BLDY = 0x0000;

    {
        u16 imeBak = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = imeBak;
    }

    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;

    SetVBlankCallback(VBlankCB_WatanabeDebug);
    SetMainCallback2(CB2_WatanabeDebugWaitForFade);

    Menu_DrawStdWindowFrame(0, 0, 29, 3);
    Menu_PrintText(headerText, 1, 1);
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_OBJ_ON;
    CreateTask(Task_WatanabeDebugInitPokemonCreator, 0);
    WatanabeDebug_InitPokemonCreatorState();
    gWatanabePokemonCreatorState->categoryIndex = 0;
    gWatanabePokemonCreatorState->fieldIndex = 0;
}

void Task_WatanabeDebugInitPokemonCreator(u8 taskId)
{
    gTasks[taskId].func = Task_WatanabeDebugDrawPokemonCreator;
}

void Task_WatanabeDebugDrawPokemonCreator(u8 taskId)
{
    WatanabeDebug_DrawPokemonCreatorFields();
    WatanabeDebug_MovePokemonCreatorCursor(0);
    gTasks[taskId].func = Task_WatanabeDebugHandlePokemonCreatorInput;
}

void Task_WatanabeDebugHandlePokemonCreatorInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        gWatanabePokemonCreatorState->previousValue = gWatanabePokemonCreatorState->values[sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[gWatanabePokemonCreatorState->fieldIndex]];
        gWatanabePokemonCreatorState->digitIndex = 0;
        WatanabeDebug_RedrawPokemonCreatorFields();
        gTasks[taskId].func = Task_WatanabeDebugHandlePokemonFieldInput;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        SetMainCallback2(CB2_WatanabeDebugWaitForFade);
        gTasks[taskId].func = Task_WatanabeDebugExitToField;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        bool8 sentToPc;
        PlaySE(SE_SELECT);
        WatanabeDebug_CreatePokemon();
        sentToPc = WatanabeDebug_AddCreatedPokemonToPartyOrPc(&gWatanabePokemonCreatorState->pokemon);
        Menu_DrawStdWindowFrame(3, 8, 26, 11);
        if (sentToPc)
            Menu_PrintText(sText_WatanabePokemonAddedToPc, 4, 9);
        else
            Menu_PrintText(sText_WatanabePokemonAddedToParty, 4, 9);
        gTasks[taskId].func = Task_WatanabeDebugWaitForPokemonCreated;
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        u32 nationalDexNum = gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM];
        GetSetPokedexFlag(nationalDexNum, 2);
        GetSetPokedexFlag(nationalDexNum, 3);
        PlaySE(SE_SELECT);
    }
    else if (JOY_NEW(DPAD_UP))
    {
        WatanabeDebug_MovePokemonCreatorCursor(2);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        WatanabeDebug_MovePokemonCreatorCursor(1);
    }
    else if (JOY_NEW(L_BUTTON) && gWatanabePokemonCreatorState->categoryIndex > 0)
    {
        gWatanabePokemonCreatorState->categoryIndex--;
        gWatanabePokemonCreatorState->fieldIndex = 0;
        gTasks[taskId].func = Task_WatanabeDebugDrawPokemonCreator;
    }
    else if (JOY_NEW(R_BUTTON) && gWatanabePokemonCreatorState->categoryIndex < 5)
    {
        gWatanabePokemonCreatorState->categoryIndex++;
        gWatanabePokemonCreatorState->fieldIndex = 0;
        gTasks[taskId].func = Task_WatanabeDebugDrawPokemonCreator;
    }
}

void Task_WatanabeDebugHandlePokemonFieldInput(u8 taskId)
{
    u8 fieldId = sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[gWatanabePokemonCreatorState->fieldIndex];
    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].func = Task_WatanabeDebugDrawPokemonCreator;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gWatanabePokemonCreatorState->values[fieldId] = gWatanabePokemonCreatorState->previousValue;
        gTasks[taskId].func = Task_WatanabeDebugDrawPokemonCreator;
    }
    else if (JOY_REPT(DPAD_UP))
    {
        WatanabeDebug_AdjustPokemonCreatorValue(1);
        WatanabeDebug_RedrawPokemonCreatorFields();
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        WatanabeDebug_AdjustPokemonCreatorValue(2);
        WatanabeDebug_RedrawPokemonCreatorFields();
    }
    else if (sWatanabePokemonEditorFieldItems[fieldId].data.pokemonEditorField->digitCount != 0xFF)
    {
        if (JOY_NEW(DPAD_LEFT) && gWatanabePokemonCreatorState->digitIndex < sWatanabePokemonEditorFieldItems[fieldId].data.pokemonEditorField->digitCount - 1)
        {
            gWatanabePokemonCreatorState->digitIndex++;
            WatanabeDebug_RedrawPokemonCreatorFields();
        }
        else if (JOY_NEW(DPAD_RIGHT) && gWatanabePokemonCreatorState->digitIndex > 0)
        {
            gWatanabePokemonCreatorState->digitIndex--;
            WatanabeDebug_RedrawPokemonCreatorFields();
        }
    }
}

void Task_WatanabeDebugWaitForPokemonCreated(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].func = Task_WatanabeDebugDrawPokemonCreator;
    }
}

u8 WatanabeDebug_FormatPokemonField(u8 *text, u8 fieldId, u8 textColor)
{
    u16 i;
    u8 numberTextLength;
    u8 digitCount;

    if (fieldId == WATANABE_POKEMON_CREATOR_FIELD_PHYSICAL_STATS)
    {
        WatanabeDebug_CopyText(text, sText_WatanabePokemonEditorHp, 7);
        WatanabeDebug_AppendColoredNumber(text + 7, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_HP], 3, 1);
        text[19] = CHAR_SPACE;
        text[20] = CHAR_SPACE;
        WatanabeDebug_CopyText(text + 21, sText_WatanabePokemonEditorAttack, 7);
        WatanabeDebug_AppendColoredNumber(text + 28, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_ATTACK], 3, 1);
        text[40] = CHAR_SPACE;
        text[41] = CHAR_SPACE;
        WatanabeDebug_CopyText(text + 42, sText_WatanabePokemonEditorDefense, 9);
        WatanabeDebug_AppendColoredNumber(text + 49, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_DEFENSE], 3, 1);
        return 0;
    }
    if (fieldId == WATANABE_POKEMON_CREATOR_FIELD_SPECIAL_STATS)
    {
        WatanabeDebug_CopyText(text, sText_WatanabePokemonEditorSpeed, 9);
        WatanabeDebug_AppendColoredNumber(text + 9, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SPEED], 3, 1);
        text[21] = CHAR_SPACE;
        text[22] = CHAR_SPACE;
        WatanabeDebug_CopyText(text + 23, sText_WatanabePokemonEditorSpAttack, 9);
        WatanabeDebug_AppendColoredNumber(text + 32, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK], 3, 1);
        text[44] = CHAR_SPACE;
        text[45] = CHAR_SPACE;
        WatanabeDebug_CopyText(text + 46, sText_WatanabePokemonEditorSpDefense, 9);
        WatanabeDebug_AppendColoredNumber(text + 55, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE], 3, 1);
        return 0;
    }
    for (i = 0; i < 12; i++)
        text[i] = CHAR_SPACE;
    WatanabeDebug_CopyText(text, sWatanabePokemonEditorFieldItems[fieldId].label, 11);
    for (i = 12; i < 256; i++)
        text[i] = EOS;
    digitCount = sWatanabePokemonEditorFieldItems[fieldId].data.pokemonEditorField->digitCount;
    numberTextLength = digitCount * 4;
    switch (fieldId)
    {
        default:
            text[0] = EOS;
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM:
            WatanabeDebug_AppendColoredNumber(text + 10, gWatanabePokemonCreatorState->values[fieldId], digitCount, textColor);
            text[10 + numberTextLength] = CHAR_SPACE;
            text[11 + numberTextLength] = EXT_CTRL_CODE_BEGIN;
            text[12 + numberTextLength] = 0x01;
            text[13 + numberTextLength] = textColor;
            WatanabeDebug_CopyText(text + 14 + numberTextLength, gSpeciesNames[NationalPokedexNumToSpecies(gWatanabePokemonCreatorState->values[fieldId])], 10);
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_GENDER:
            switch (gWatanabePokemonCreatorState->values[fieldId])
            {
                case 0:
                    WatanabeDebug_CopyText(text + 10, sText_WatanabePokemonEditorFemale, 2);
                    break;
                case 1:
                    WatanabeDebug_CopyText(text + 10, sText_WatanabePokemonEditorNone, 2);
                    break;
                case 2:
                    WatanabeDebug_CopyText(text + 10, sText_WatanabePokemonEditorMale, 2);
                    break;
            }
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_MOVE_1 ... WATANABE_POKEMON_CREATOR_VALUE_MOVE_4:
            WatanabeDebug_AppendColoredNumber(text + 10, gWatanabePokemonCreatorState->values[fieldId], digitCount, textColor);
            text[10 + numberTextLength] = CHAR_SPACE;
            text[11 + numberTextLength] = EXT_CTRL_CODE_BEGIN;
            text[12 + numberTextLength] = 0x01;
            text[13 + numberTextLength] = textColor;
            WatanabeDebug_CopyText(text + 14 + numberTextLength, gMoveNames[gWatanabePokemonCreatorState->values[fieldId]], 12);
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_HELD_ITEM:
            WatanabeDebug_AppendColoredNumber(text + 10, gWatanabePokemonCreatorState->values[fieldId], digitCount, textColor);
            text[10 + numberTextLength] = CHAR_SPACE;
            text[11 + numberTextLength] = EXT_CTRL_CODE_BEGIN;
            text[12 + numberTextLength] = 0x01;
            text[13 + numberTextLength] = textColor;
            if (gWatanabePokemonCreatorState->values[fieldId] != ITEM_NONE)
                WatanabeDebug_CopyText(text + 14 + numberTextLength, GetItemName(gWatanabePokemonCreatorState->values[fieldId]), 9);
            else
                WatanabeDebug_CopyText(text + 14 + numberTextLength, sText_WatanabePokemonEditorNone, 9);
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_ABILITY_SLOT:
            text += 12;
            WatanabeDebug_CopyText(text, gAbilityNames[GetAbilityBySpecies(NationalPokedexNumToSpecies(gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM]), gWatanabePokemonCreatorState->values[fieldId])], 12);
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_STATUS:
            WatanabeDebug_CopyText(text + 10, sWatanabePokemonEditorStatusItems[gWatanabePokemonCreatorState->values[fieldId]].label, 4);
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_LEVEL ... WATANABE_POKEMON_CREATOR_VALUE_PERSONALITY:
        case WATANABE_POKEMON_CREATOR_VALUE_HP_IV ... WATANABE_POKEMON_CREATOR_VALUE_POKERUS:
            WatanabeDebug_AppendColoredNumber(text + 12, gWatanabePokemonCreatorState->values[fieldId], digitCount, textColor);
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_IS_EGG:
            if (gWatanabePokemonCreatorState->values[fieldId])
                WatanabeDebug_CopyText(text + 10, sText_WatanabePokemonEditorOn, 4);
            else
                WatanabeDebug_CopyText(text + 10, sText_WatanabePokemonEditorOff, 4);
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_NATURE:
            WatanabeDebug_CopyText(text + 10, gNatureNames[gWatanabePokemonCreatorState->values[fieldId]], 5);
            break;
    }
    return 0;
}

u32 WatanabeDebug_GetPokemonCreatorValue(u8 fieldId)
{
    return gWatanabePokemonCreatorState->values[fieldId];
}

void WatanabeDebug_SetPokemonCreatorValue(u32 value, u8 fieldId)
{
    switch (fieldId)
    {
        default:
            gWatanabePokemonCreatorState->values[fieldId] = value;
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM:
        case WATANABE_POKEMON_CREATOR_VALUE_OT_ID:
        case WATANABE_POKEMON_CREATOR_VALUE_PERSONALITY:
            gWatanabePokemonCreatorState->values[fieldId] = value;
            WatanabeDebug_RecreatePokemon();
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_LEVEL:
            gWatanabePokemonCreatorState->values[fieldId] = value;
            gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE] = gExperienceTables[gBaseStats[NationalPokedexNumToSpecies(gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM])].growthRate][value];
            WatanabeDebug_RecreatePokemon();
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE:
            gWatanabePokemonCreatorState->values[fieldId] = value;
            WatanabeDebug_RecalculatePokemonForExperience();
            break;
        case WATANABE_POKEMON_CREATOR_VALUE_HP_IV ... WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_EV:
            gWatanabePokemonCreatorState->values[fieldId] = value;
            WatanabeDebug_RecalculatePokemonStats();
            break;
    }
}

void WatanabeDebug_RecreatePokemon(void)
{
    u32 ivs = gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_HP_IV] & 0x1f;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_ATTACK_IV] & 0x1f) << 5;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_IV] & 0x1f) << 10;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SPEED_IV] & 0x1f) << 15;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_IV] & 0x1f) << 20;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_IV] & 0x1f) << 25;
    CreateMon(&gWatanabePokemonCreatorState->pokemon, NationalPokedexNumToSpecies(gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM]), gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_LEVEL], ivs, TRUE, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_PERSONALITY], TRUE, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_OT_ID]);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_GENDER] = (u8)(GetMonGender(&gWatanabePokemonCreatorState->pokemon) + 2);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_NATURE] = GetNature(&gWatanabePokemonCreatorState->pokemon);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_1] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE1);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_2] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE2);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_3] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE3);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_4] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE4);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_EXP);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_HP] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MAX_HP);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_ATTACK] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ATK);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_DEFENSE] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_DEF);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SPEED] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPEED);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPATK);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPDEF);
}

void WatanabeDebug_RecalculatePokemonForExperience(void)
{
    u32 zero = 0;
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_EXP, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE1, &zero);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE2, &zero);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE3, &zero);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE4, &zero);
    CalculateMonStats(&gWatanabePokemonCreatorState->pokemon);
    GiveMonInitialMoveset(&gWatanabePokemonCreatorState->pokemon);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_LEVEL] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_LEVEL);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_1] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE1);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_2] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE2);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_3] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE3);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_4] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MOVE4);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_HP] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MAX_HP);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_ATTACK] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ATK);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_DEFENSE] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_DEF);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SPEED] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPEED);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPATK);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPDEF);
}

void WatanabeDebug_RecalculatePokemonStats(void)
{
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_HP_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_HP_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_HP_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_HP_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ATK_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_ATTACK_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ATK_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_ATTACK_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_DEF_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_DEF_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPEED_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SPEED_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPEED_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SPEED_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPATK_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPATK_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPDEF_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPDEF_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_EV);
    CalculateMonStats(&gWatanabePokemonCreatorState->pokemon);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_HP] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MAX_HP);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_ATTACK] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ATK);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_DEFENSE] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_DEF);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SPEED] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPEED);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPATK);
    gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE] = GetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPDEF);
}

void WatanabeDebug_CreatePokemon(void)
{
    u32 ivs;
    u8 eggName[] = DTR("たまご", "EGG");
#if (ENGLISH && REVISION == 0)
    u8 otName[] = _("デバッグポケ1");
#else
    u8 otName[] = _("DebugーG");
#endif
    u8 language;
    u16 metLocation;

    ivs = gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_HP_IV] & 0x1f;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_ATTACK_IV] & 0x1f) << 5;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_IV] & 0x1f) << 10;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SPEED_IV] & 0x1f) << 15;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_IV] & 0x1f) << 20;
    ivs |= (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_IV] & 0x1f) << 25;

    CreateMon(&gWatanabePokemonCreatorState->pokemon, NationalPokedexNumToSpecies(gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_NATIONAL_DEX_NUM]), gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_LEVEL], ivs, TRUE, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_PERSONALITY], TRUE, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_OT_ID]);

    if (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_1] != MOVE_NONE)
        SetMonMoveSlot(&gWatanabePokemonCreatorState->pokemon, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_1], 0);
    if (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_2] != MOVE_NONE)
        SetMonMoveSlot(&gWatanabePokemonCreatorState->pokemon, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_2], 1);
    if (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_3] != MOVE_NONE)
        SetMonMoveSlot(&gWatanabePokemonCreatorState->pokemon, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_3], 2);
    if (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_4] != MOVE_NONE)
        SetMonMoveSlot(&gWatanabePokemonCreatorState->pokemon, gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_MOVE_4], 3);

    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_EXP, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_EXPERIENCE);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_HP_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_HP_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_HP_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_HP_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ATK_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_ATTACK_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ATK_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_ATTACK_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_DEF_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_DEF_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_DEFENSE_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPEED_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SPEED_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPEED_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SPEED_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPATK_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPATK_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_ATTACK_EV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPDEF_IV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_IV);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SPDEF_EV, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SP_DEFENSE_EV);

    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_COOL, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_COOL);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_BEAUTY, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_BEAUTY);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_CUTE, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_CUTE);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SMART, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SMART);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_TOUGH, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_TOUGH);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_SHEEN, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_SHEEN);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_FRIENDSHIP, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_FRIENDSHIP);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_POKERUS, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_POKERUS);

    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_HELD_ITEM, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_HELD_ITEM);
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_ABILITY_NUM, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_ABILITY_SLOT);

    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_STATUS, &sWatanabePokemonEditorStatusItems[gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_STATUS]].data.status);

    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_IS_EGG, gWatanabePokemonCreatorState->values + WATANABE_POKEMON_CREATOR_VALUE_IS_EGG);
    if (gWatanabePokemonCreatorState->values[WATANABE_POKEMON_CREATOR_VALUE_IS_EGG])
    {
        SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_NICKNAME, eggName);
        language = LANGUAGE_JAPANESE;
        SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_LANGUAGE, &language);
    }

    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_OT_NAME, otName);
    metLocation = 0xff;
    SetMonData(&gWatanabePokemonCreatorState->pokemon, MON_DATA_MET_LOCATION, &metLocation);
    CalculateMonStats(&gWatanabePokemonCreatorState->pokemon);
}

void WatanabeDebug_MovePokemonCreatorCursor(u8 direction)
{
    u8 cursor[] = _("▶");
    u8 blank[] = _(" ");

    switch (direction)
    {
        case 0:
            Menu_PrintText(cursor, 1, 2 * gWatanabePokemonCreatorState->fieldIndex + 5);
            break;
        case 1:
            if (gWatanabePokemonCreatorState->fieldIndex < sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].data.categoryLength - 1)
            {
                Menu_PrintText(blank, 1, 2 * gWatanabePokemonCreatorState->fieldIndex + 5);
                gWatanabePokemonCreatorState->fieldIndex++;
                Menu_PrintText(cursor, 1, 2 * gWatanabePokemonCreatorState->fieldIndex + 5);
            }
            break;
        case 2:
            if (gWatanabePokemonCreatorState->fieldIndex > 0)
            {
                Menu_PrintText(blank, 1, 2 * gWatanabePokemonCreatorState->fieldIndex + 5);
                gWatanabePokemonCreatorState->fieldIndex--;
                Menu_PrintText(cursor, 1, 2 * gWatanabePokemonCreatorState->fieldIndex + 5);
            }
            break;
    }
}

void WatanabeDebug_DrawPokemonCreatorFields(void)
{
    u16 i;

    Menu_DrawStdWindowFrame(0, 4, 29, 19);
    for (i = 0; sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[i] != EOS; i++)
    {
        gWatanabePokemonCreatorState->textBuffer[0] = EXT_CTRL_CODE_BEGIN;
        gWatanabePokemonCreatorState->textBuffer[1] = 0x01;
        gWatanabePokemonCreatorState->textBuffer[2] = 0x01;
        WatanabeDebug_FormatPokemonField(gWatanabePokemonCreatorState->textBuffer + 3, sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[i], 1);
        Menu_PrintText(gWatanabePokemonCreatorState->textBuffer, 2, 2 * i + 5);
    }
}

void WatanabeDebug_RedrawPokemonCreatorFields(void)
{
    u16 i;

    Menu_BlankWindowRect(2, 5, 28, 18);
    for (i = 0; sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[i] != EOS; i++)
    {
        gWatanabePokemonCreatorState->textBuffer[0] = EXT_CTRL_CODE_BEGIN;
        gWatanabePokemonCreatorState->textBuffer[1] = 0x01;
        if (i == gWatanabePokemonCreatorState->fieldIndex)
        {
            gWatanabePokemonCreatorState->textBuffer[2] = 0x02;
            WatanabeDebug_FormatPokemonField(gWatanabePokemonCreatorState->textBuffer + 3, sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[i], 2);
        }
        else
        {
            gWatanabePokemonCreatorState->textBuffer[2] = 0x01;
            WatanabeDebug_FormatPokemonField(gWatanabePokemonCreatorState->textBuffer + 3, sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[i], 1);
        }
        Menu_PrintText(gWatanabePokemonCreatorState->textBuffer, 2, 2 * i + 5);
    }
}

const u32 sWatanabePowersOfTen[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000
};

void WatanabeDebug_AdjustPokemonCreatorValue(u8 direction)
{
    u32 value = WatanabeDebug_GetPokemonCreatorValue(sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[gWatanabePokemonCreatorState->fieldIndex]);
    u32 minValue = sWatanabePokemonEditorFieldItems[sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[gWatanabePokemonCreatorState->fieldIndex]].data.pokemonEditorField->minValue;
    u32 maxValue = sWatanabePokemonEditorFieldItems[sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[gWatanabePokemonCreatorState->fieldIndex]].data.pokemonEditorField->maxValue;
    u32 adjustment = sWatanabePowersOfTen[gWatanabePokemonCreatorState->digitIndex];

    if (sWatanabePokemonEditorFieldItems[sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[gWatanabePokemonCreatorState->fieldIndex]].data.pokemonEditorField->skipValueAdjustment == 0)
    {
        switch (direction)
        {
            case 1:
                if (value < maxValue - adjustment)
                    value += adjustment;
                else if (value < maxValue)
                    value = maxValue;
                else
                    value = minValue;
                break;
            case 2:
                if (value > minValue + adjustment)
                    value -= adjustment;
                else if (value > minValue)
                    value = minValue;
                else
                    value = maxValue;
                break;
        }
    }
    WatanabeDebug_SetPokemonCreatorValue(value, sWatanabePokemonEditorCategories[gWatanabePokemonCreatorState->categoryIndex].label[gWatanabePokemonCreatorState->fieldIndex]);
}

void WatanabeDebug_AppendColoredDigit(u8 *text, u8 digit, u8 textColor, u8 digitIndex)
{
    *text++ = EXT_CTRL_CODE_BEGIN;
    *text++ = 0x01;
    if (textColor == 2)
    {
        if (digitIndex != gWatanabePokemonCreatorState->digitIndex)
            *text = 0x02;
        else
            *text = 0x04;
    }
    else
        *text = 0x01;
    text[1] = digit + CHAR_0;
}

void WatanabeDebug_AppendColoredNumber(u8 *text, u32 value, u8 digitCount, u8 textColor)
{
    u8 textOffset = 0;

    switch (digitCount)
    {
        default:
        case 10:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 1000000000, textColor, 9);
            value %= 1000000000;
            textOffset += 4;
        case  9:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 100000000, textColor, 8);
            value %= 100000000;
            textOffset += 4;
        case  8:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 10000000, textColor, 7);
            value %= 10000000;
            textOffset += 4;
        case  7:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 1000000, textColor, 6);
            value %= 1000000;
            textOffset += 4;
        case  6:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 100000, textColor, 5);
            value %= 100000;
            textOffset += 4;
        case  5:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 10000, textColor, 4);
            value %= 10000;
            textOffset += 4;
        case  4:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 1000, textColor, 3);
            value %= 1000;
            textOffset += 4;
        case  3:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 100, textColor, 2);
            value %= 100;
            textOffset += 4;
        case  2:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value / 10, textColor, 1);
            value %= 10;
            textOffset += 4;
        case  1:
            WatanabeDebug_AppendColoredDigit(text + textOffset, value, textColor, 0);
            break;
    }
}

void WatanabeDebug_CopyText(u8 *dest, const u8 *src, u8 maxLength)
{
    u8 i;

    for (i = 0; src[i] != EOS && i < maxLength; i++)
        dest[i] = src[i];
}

void WatanabeDebug_PrintHexValue(u16 value, u8 x, u8 y, u8 digitCount)
{
    u8 hexText[5];
    u8 i;

    for (i = 0; i < 4; i++)
        hexText[i] = 0;
    hexText[4] = EOS;

    switch (digitCount)
    {
        default:
            hexText[0] = value >> 12;
        case 3:
            hexText[1] = (value & 0x0F00) >> 8;
        case 2:
            hexText[2] = (value & 0x00F0) >> 4;
        case 1:
            hexText[3] = (value & 0x000F);
            break;
    }

    for (i = 0; i < 4; i++)
    {
        if (hexText[i] < 10)
            hexText[i] += CHAR_0;
        else
            hexText[i] += CHAR_A - 10;
    }

    Menu_PrintText(hexText + (u8)(4 - digitCount), x, y);
}

struct OamData sWatanabeDebugColorPickerOamData = {
    .y = 0xa0
};

union AnimCmd sWatanabeDebugColorPickerAnim0[] = {
    ANIMCMD_FRAME(38, 30),
    ANIMCMD_END
};

union AnimCmd sWatanabeDebugColorPickerAnim1[] = {
    ANIMCMD_FRAME(39, 30),
    ANIMCMD_END
};

union AnimCmd sWatanabeDebugColorPickerAnim2[] = {
    ANIMCMD_FRAME(40, 30),
    ANIMCMD_END
};

const union AnimCmd *sWatanabeDebugColorPickerAnimTable[] = {
    sWatanabeDebugColorPickerAnim0,
    sWatanabeDebugColorPickerAnim1,
    sWatanabeDebugColorPickerAnim2
};

struct SpriteTemplate sWatanabeDebugColorPickerTemplate = {
    SPRITETAG_WATANABE,
    SPRITETAG_WATANABE,
    &sWatanabeDebugColorPickerOamData,
    sWatanabeDebugColorPickerAnimTable,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_WatanabeDebugColorPicker
};

u16 sWatanabeDebugSpriteBrowserPalette[] = INCBIN_U16("graphics/debug/sprite_browser.gbapal");
u8 sWatanabeDebugSpriteBrowserTiles[] = INCBIN_U8("graphics/debug/sprite_browser.4bpp");

void InitSeePokemonGraphics(void)
{
    u16 pal;
    u8 spriteId;
    WatanabeDebug_InitScreen();
    DmaCopy16Defvars(3, sWatanabeDebugSpriteBrowserTiles, BG_SCREEN_ADDR(28), sizeof(sWatanabeDebugSpriteBrowserTiles));
    LoadPalette(sWatanabeDebugSpriteBrowserPalette, 0x80, sizeof(sWatanabeDebugSpriteBrowserPalette));
    pal = RGB(31, 31, 31);
    LoadPalette(&pal, 0, sizeof(pal));
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    REG_WIN0H = 0;
    REG_WIN0V = 0;
    REG_WIN1H = 0;
    REG_WIN1V = 0;
    REG_WININ = 0x3F;
    REG_WINOUT = 0x1F;
    REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_OBJ | BLDCNT_TGT1_BD | BLDCNT_EFFECT_DARKEN;
    REG_BLDALPHA = 0;
    REG_BLDY = 0x07;
    {
        u16 imeBak = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = imeBak;
    }
    SetVBlankCallback(VBlankCB_WatanabeDebug);
    SetMainCallback2(CB2_WatanabeDebugWaitForFade);
    LoadMonIconPalettes();
    REG_BG0CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(31);
    REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(30);
    REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON;
    CreateTask(Task_WatanabeDebugInitPokemonGraphicsBrowser, 0);
    gWatanabeGraphicsBrowserState = &eWatanabeGraphicsBrowserState;
    gWatanabeGraphicsBrowserState->graphicsId = 0x115;
    gWatanabeGraphicsBrowserState->frontSpriteId = 0;
    gWatanabeGraphicsBrowserState->backSpriteId = 0;
    gWatanabeGraphicsBrowserState->isColorPickerActive = 0;
    gWatanabeGraphicsBrowserState->paletteColorIndex = 0;
    gWatanabeGraphicsBrowserState->selectedRgbComponent = 0;
    gWatanabeGraphicsBrowserState->palettePulsePhase = 0;
    spriteId = CreateSprite(&sWatanabeDebugColorPickerTemplate, 0x6C, 0x74, 0);
    gSprites[spriteId].data[0] = 0;
    StartSpriteAnim(gSprites + spriteId, 0);
    spriteId = CreateSprite(&sWatanabeDebugColorPickerTemplate, 0x6C, 0x74, 0);
    gSprites[spriteId].data[0] = 1;
    StartSpriteAnim(gSprites + spriteId, 1);
    spriteId = CreateSprite(&sWatanabeDebugColorPickerTemplate, 0x6C, 0x74, 0);
    gSprites[spriteId].data[0] = 2;
    StartSpriteAnim(gSprites + spriteId, 2);
}

const u16 sWatanabePalettePulseValues[] = {
    0x0000,
    0x0002,
    0x0004,
    0x0006,
    0x0008,
    0x000a,
    0x000c,
    0x000e,
    0x0010,
    0x0012,
    0x0014,
    0x0016,
    0x0018,
    0x001a,
    0x001c,
    0x001e,
    0x001f,
    0x001e,
    0x001c,
    0x001a,
    0x0018,
    0x0016,
    0x0014,
    0x0012,
    0x0010,
    0x000e,
    0x000c,
    0x000a,
    0x0008,
    0x0006,
    0x0004,
    0x0002
};

const u8 sText_WatanabeGraphicsBrowserPrevious[] = _("{COLOR RED}L{ESCAPE}{LEFT_ARROW}");
const u8 sText_WatanabeGraphicsBrowserNext[] = _("{COLOR RED}{ESCAPE}{RIGHT_ARROW}R");
const u8 sText_WatanabeGraphicsBrowserDevelopmentNumber[] = DTR("かいはつナンバー", "Devel No.");
const u8 sText_WatanabeGraphicsBrowserGraphicsNumber[] = DTR("グラフィックナンバー", "Graphic No.");
const u8 sText_WatanabeGraphicsBrowserSetColor[] = DTR("{COLOR RED}A:カラーせってい", "{COLOR RED}A: SET COLOR");
const u8 sText_WatanabeGraphicsBrowserConfirmCancel[] = DTR("{COLOR RED}A:けってい　B:キャンセル", "{COLOR RED}A: CONFIRM  B: CANCEL");
const u8 sText_WatanabeGraphicsBrowserColon[] = _(":");
void Task_WatanabeDebugInitPokemonGraphicsBrowser(u8 taskId)
{
    u8 colorPickerTileIds[] = {0x00, 0x10, 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x20, 0x21};
    u8 i;

    Menu_DrawStdWindowFrame(10, 0, 15, 7);
    Menu_DrawStdWindowFrame(0, 0, 9, 9);
    Menu_DrawStdWindowFrame(0, 10, 9, 19);
    Menu_DrawStdWindowFrame(16, 0, 29, 7);
    Menu_PrintText(sText_WatanabeGraphicsBrowserPrevious, 17, 1);
    Menu_PrintText(sText_WatanabeGraphicsBrowserNext, 27, 1);
    Menu_PrintText(sText_WatanabeGraphicsBrowserDevelopmentNumber, 17, 5);

    Menu_DrawStdWindowFrame(10, 8, 29, 12);
    for (i = 0; i < 15; i++)
        ((u16 *)(VRAM + 0xFA56))[i] = 0xA311 + i;
    for (i = 0; i < 15; i++)
        ((u16 *)(VRAM + 0xF256))[i] = 0x8301 + i;

    Menu_PrintText(sText_WatanabeGraphicsBrowserSetColor, 20, 10);
    Menu_DrawStdWindowFrame(10, 13, 29, 19);

    colorPickerTileIds[0] = 0x23;
    for (i = 0; i < 18; i++)
        ((u16 *)(VRAM + 0xF396))[i] = 0x9300 + colorPickerTileIds[i];
    colorPickerTileIds[0] = 0x24;
    for (i = 0; i < 18; i++)
        ((u16 *)(VRAM + 0xF3D6))[i] = 0x9300 + colorPickerTileIds[i];
    colorPickerTileIds[0] = 0x25;
    for (i = 0; i < 18; i++)
        ((u16 *)(VRAM + 0xF416))[i] = 0x9300 + colorPickerTileIds[i];

    Menu_PrintText(sText_WatanabeGraphicsBrowserConfirmCancel, 15, 17);

    REG_WIN0H = 0x51EF;
    REG_WIN0V = 0x699F;

    gTasks[taskId].func = Task_WatanabeDebugLoadPokemonGraphics;
}

void Task_WatanabeDebugLoadPokemonGraphics(u8 taskId)
{
    DecompressPicFromTable(gMonFrontPicTable + gWatanabeGraphicsBrowserState->graphicsId, gMonFrontPicCoords[gWatanabeGraphicsBrowserState->graphicsId].coords, gMonFrontPicCoords[gWatanabeGraphicsBrowserState->graphicsId].y_offset, gMonSpriteGfxBufferPtrs[0], gMonSpriteGfxBufferPtrs[1], gWatanabeGraphicsBrowserState->graphicsId);
    LoadCompressedSpritePalette(gMonPaletteTable + gWatanabeGraphicsBrowserState->graphicsId);
    SetMultiuseSpriteTemplateToPokemon(gWatanabeGraphicsBrowserState->graphicsId, 1);
    gWatanabeGraphicsBrowserState->frontSpriteId = CreateSprite(&gCreatingSpriteTemplate, 0x28, 0x28, 0);
    gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].callback = SpriteCB_WatanabeDebugDummy;
    gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].oam.priority = 0;

    DecompressPicFromTable(gMonBackPicTable + gWatanabeGraphicsBrowserState->graphicsId, gMonBackPicCoords[gWatanabeGraphicsBrowserState->graphicsId].coords, gMonBackPicCoords[gWatanabeGraphicsBrowserState->graphicsId].y_offset, gMonSpriteGfxBufferPtrs[0], gMonSpriteGfxBufferPtrs[2], gWatanabeGraphicsBrowserState->graphicsId);
    LoadCompressedSpritePalette(gMonPaletteTable + gWatanabeGraphicsBrowserState->graphicsId);
    SetMultiuseSpriteTemplateToPokemon(gWatanabeGraphicsBrowserState->graphicsId, 2);
    gWatanabeGraphicsBrowserState->backSpriteId = CreateSprite(&gCreatingSpriteTemplate, 0x28, 0x78, 0);
    gSprites[gWatanabeGraphicsBrowserState->backSpriteId].callback = SpriteCB_WatanabeDebugDummy;
    gSprites[gWatanabeGraphicsBrowserState->backSpriteId].oam.priority = 0;

    gWatanabeGraphicsBrowserState->iconSpriteId = CreateMonIcon(gWatanabeGraphicsBrowserState->graphicsId, SpriteCB_MonIcon, 0x68, 0x2C, 0, 0);

    DrawFootprint(SpeciesToNationalPokedexNum(gWatanabeGraphicsBrowserState->graphicsId), 2, 0x3fc);

    ((u16 *)(VRAM + 0xF858))[0] = 0xF3FC;
    ((u16 *)(VRAM + 0xF858))[1] = 0xF3FD;
    ((u16 *)(VRAM + 0xF858))[32] = 0xF3FE;
    ((u16 *)(VRAM + 0xF858))[33] = 0xF3FF;

    WatanabeDebug_PrintSpeciesName(gWatanabeGraphicsBrowserState->graphicsId, 17, 3);
    WatanabeDebug_PrintDecimal3(gWatanabeGraphicsBrowserState->graphicsId, 26, 5);

    gWatanabeGraphicsBrowserState->spritePaletteNum = gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].oam.paletteNum;
    CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferUnfaded + 0x80, 0x20);
    CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferFaded + 0x80, 0x20);

    gTasks[taskId].func = Task_WatanabeDebugRefreshPokemonPalette;

    gWatanabeGraphicsBrowserState->useShinyPalette = 0;
    StopCryAndClearCrySongs();
    PlayCry_Normal(gWatanabeGraphicsBrowserState->graphicsId, 0);
}

void Task_WatanabeDebugRefreshPokemonPalette(u8 taskId)
{
    u16 paletteColor;
    CpuCopy16(gPlttBufferUnfaded + 0x80, gWatanabeGraphicsBrowserState->savedPalette, 32);
    paletteColor = gPlttBufferUnfaded[gWatanabeGraphicsBrowserState->paletteColorIndex + 0x81];
    gWatanabeGraphicsBrowserState->selectedColor.r = paletteColor & 0x1f;
    gWatanabeGraphicsBrowserState->selectedColor.g = (paletteColor & 0x3e0) >> 5;
    gWatanabeGraphicsBrowserState->selectedColor.b = (paletteColor & 0x7c00) >> 10;
    gWatanabeGraphicsBrowserState->selectedColor.unused_15 = 0;
    WatanabeDebug_PrintHexValue(gWatanabeGraphicsBrowserState->paletteColorIndex + 1, 11, 10, 2);
    Menu_PrintText(sText_WatanabeGraphicsBrowserColon, 13, 10);
    WatanabeDebug_PrintHexValue(paletteColor, 14, 10, 4);
    gTasks[taskId].func = Task_WatanabeDebugHandlePokemonGraphicsInput;
}

#ifdef NONMATCHING
void Task_WatanabeDebugHandlePokemonGraphicsInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        SetMainCallback2(CB2_WatanabeDebugWaitForFade);
        gTasks[taskId].func = Task_WatanabeDebugExitToField;
    }
    else if (JOY_REPT(R_BUTTON))
    {
        gWatanabeGraphicsBrowserState->graphicsId = WatanabeDebug_AdvanceSpeciesId(0, gWatanabeGraphicsBrowserState->graphicsId);
        gTasks[taskId].func = Task_WatanabeDebugReloadPokemonGraphics;
    }
    else if (JOY_REPT(L_BUTTON))
    {
        gWatanabeGraphicsBrowserState->graphicsId = WatanabeDebug_AdvanceSpeciesId(1, gWatanabeGraphicsBrowserState->graphicsId);
        gTasks[taskId].func = Task_WatanabeDebugReloadPokemonGraphics;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        gWatanabeGraphicsBrowserState->isColorPickerActive = 1;
        REG_WIN0H = 0x51EF;
        REG_WIN0V = 0x4167;
        gTasks[taskId].func = Task_WatanabeDebugHandlePokemonColorInput;
    }
    else if (JOY_REPT(DPAD_RIGHT) && gWatanabeGraphicsBrowserState->paletteColorIndex < 14)
    {
        ((u16 *)PLTT)[0x81 + gWatanabeGraphicsBrowserState->paletteColorIndex] = 0;
        do; while(0);
        gWatanabeGraphicsBrowserState->paletteColorIndex++;
        gTasks[taskId].func = Task_WatanabeDebugRefreshPokemonPalette;
    }
    else if (JOY_REPT(DPAD_LEFT) && gWatanabeGraphicsBrowserState->paletteColorIndex > 0)
    {
        ((u16 *)PLTT)[0x81 + gWatanabeGraphicsBrowserState->paletteColorIndex] = 0;
        do; while(0);
        gWatanabeGraphicsBrowserState->paletteColorIndex--;
        gTasks[taskId].func = Task_WatanabeDebugRefreshPokemonPalette;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        gWatanabeGraphicsBrowserState->useShinyPalette ^= 1;
        if (gWatanabeGraphicsBrowserState->useShinyPalette)
            LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(gWatanabeGraphicsBrowserState->graphicsId, 0, 0), gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, 0x20);
        else
            LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(gWatanabeGraphicsBrowserState->graphicsId, 0, 9), gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, 0x20);
        CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferUnfaded + 0x80, 32);
        CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferFaded + 0x80, 32);
        gTasks[taskId].func = Task_WatanabeDebugRefreshPokemonPalette;
    }
    else
    {
        if (JOY_NEW(SELECT_BUTTON))
        {
            StopCryAndClearCrySongs();
            PlayCry_Normal(gWatanabeGraphicsBrowserState->graphicsId, 0);
        }
        gWatanabeGraphicsBrowserState->palettePulsePhase += 4;
        gWatanabeGraphicsBrowserState->palettePulsePhase &= 0x1f;
        ((u16 *)PLTT)[0xa1 + gWatanabeGraphicsBrowserState->paletteColorIndex] = sWatanabePalettePulseValues[gWatanabeGraphicsBrowserState->palettePulsePhase];
    }
}
#else
NAKED void Task_WatanabeDebugHandlePokemonGraphicsInput(u8 taskId)
{
    asm(".equiv PLTT, 0x05000000\n"
        "\tpush\t{r4, r5, r6, lr}\n"
        "\tadd\tsp, sp, #0xfffffffc\n"
        "\tlsl\tr0, r0, #0x18\n"
        "\tlsr\tr6, r0, #0x18\n"
        "\tldr\tr1, ._674       @ gMain\n"
        "\tldrh\tr2, [r1, #0x2e]\n"
        "\tmov\tr0, #0x2\n"
        "\tand\tr0, r0, r2\n"
        "\tadd\tr4, r1, #0\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._672\t@cond_branch\n"
        "\tmov\tr0, #0x5\n"
        "\tbl\tPlaySE\n"
        "\tmov\tr0, #0x1\n"
        "\tneg\tr0, r0\n"
        "\tmov\tr1, #0x0\n"
        "\tstr\tr1, [sp]\n"
        "\tmov\tr2, #0x0\n"
        "\tmov\tr3, #0x10\n"
        "\tbl\tBeginNormalPaletteFade\n"
        "\tldr\tr0, ._674 + 4   @ CB2_WatanabeDebugWaitForFade\n"
        "\tbl\tSetMainCallback2\n"
        "\tldr\tr1, ._674 + 8   @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._674 + 12  @ Task_WatanabeDebugExitToField\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._703\n"
        "._675:\n"
        "\t.align\t2, 0\n"
        "._674:\n"
        "\t.word\tgMain\n"
        "\t.word\tCB2_WatanabeDebugWaitForFade+1\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugExitToField+1\n"
        "._672:\n"
        "\tldrh\tr1, [r4, #0x30]\n"
        "\tmov\tr0, #0x80\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._676\t@cond_branch\n"
        "\tldr\tr4, ._678       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr0, [r4]\n"
        "\tldrh\tr1, [r0]\n"
        "\tmov\tr0, #0x0\n"
        "\tb\t._677\n"
        "._679:\n"
        "\t.align\t2, 0\n"
        "._678:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "._676:\n"
        "\tmov\tr0, #0x80\n"
        "\tlsl\tr0, r0, #0x2\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._680\t@cond_branch\n"
        "\tldr\tr4, ._682       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr0, [r4]\n"
        "\tldrh\tr1, [r0]\n"
        "\tmov\tr0, #0x1\n"
        "._677:\n"
        "\tbl\tWatanabeDebug_AdvanceSpeciesId\n"
        "\tldr\tr1, [r4]\n"
        "\tstrh\tr0, [r1]\n"
        "\tldr\tr1, ._682 + 4   @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._682 + 8   @ Task_WatanabeDebugReloadPokemonGraphics\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._703\n"
        "._683:\n"
        "\t.align\t2, 0\n"
        "._682:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugReloadPokemonGraphics+1\n"
        "._680:\n"
        "\tmov\tr3, #0x1\n"
        "\tand\tr3, r3, r2\n"
        "\tcmp\tr3, #0\n"
        "\tbeq\t._684\t@cond_branch\n"
        "\tldr\tr0, ._686       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr1, [r0]\n"
        "\tmov\tr0, #0x1\n"
        "\tstrb\tr0, [r1, #0x5]\n"
        "\tldr\tr1, ._686 + 4   @ 0x4000040\n"
        "\tldr\tr2, ._686 + 8   @ 0x51ef\n"
        "\tadd\tr0, r2, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tadd\tr1, r1, #0x4\n"
        "\tldr\tr2, ._686 + 12  @ 0x4167\n"
        "\tadd\tr0, r2, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tldr\tr1, ._686 + 16  @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._686 + 20  @ Task_WatanabeDebugHandlePokemonColorInput\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._703\n"
        "._687:\n"
        "\t.align\t2, 0\n"
        "._686:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\t0x4000040\n"
        "\t.word\t0x51ef\n"
        "\t.word\t0x4167\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugHandlePokemonColorInput+1\n"
        "._684:\n"
        "\tmov\tr0, #0x10\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._689\t@cond_branch\n"
        "\tldr\tr1, ._691       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr2, [r1]\n"
        "\tldrb\tr0, [r2, #0x7]\n"
        "\tcmp\tr0, #0xd\n"
        "\tbhi\t._689\t@cond_branch\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tldr\tr2, ._691 + 4   @ PLTT + 0x102\n"
        "\tadd\tr0, r0, r2\n"
        "\tstrh\tr3, [r0]\n"
        "\tldr\tr1, [r1]\n"
        "\tldrb\tr0, [r1, #0x7]\n"
        "\tadd\tr0, r0, #0x1\n"
        "\tstrb\tr0, [r1, #0x7]\n"
        "\tb\t._695\n"
        "._692:\n"
        "\t.align\t2, 0\n"
        "._691:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\tPLTT + 0x102\n"
        "._689:\n"
        "\tldrh\tr1, [r4, #0x30]\n"
        "\tmov\tr0, #0x20\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._694\t@cond_branch\n"
        "\tldr\tr2, ._696       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr1, [r2]\n"
        "\tldrb\tr0, [r1, #0x7]\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._694\t@cond_branch\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tldr\tr1, ._696 + 4   @ PLTT + 0x102\n"
        "\tadd\tr0, r0, r1\n"
        "\tmov\tr1, #0x0\n"
        "\tstrh\tr1, [r0]\n"
        "\tldr\tr1, [r2]\n"
        "\tldrb\tr0, [r1, #0x7]\n"
        "\tsub\tr0, r0, #0x1\n"
        "\tstrb\tr0, [r1, #0x7]\n"
        "\tb\t._695\n"
        "._697:\n"
        "\t.align\t2, 0\n"
        "._696:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\tPLTT + 0x102\n"
        "._694:\n"
        "\tldrh\tr1, [r4, #0x2e]\n"
        "\tmov\tr0, #0x8\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._698\t@cond_branch\n"
        "\tldr\tr4, ._701       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr2, [r4]\n"
        "\tldrb\tr0, [r2, #0x9]\n"
        "\tmov\tr1, #0x1\n"
        "\teor\tr0, r0, r1\n"
        "\tstrb\tr0, [r2, #0x9]\n"
        "\tldr\tr1, [r4]\n"
        "\tldrb\tr0, [r1, #0x9]\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._699\t@cond_branch\n"
        "\tldrh\tr0, [r1]\n"
        "\tmov\tr1, #0x0\n"
        "\tmov\tr2, #0x0\n"
        "\tbl\tGetMonSpritePalFromSpeciesAndPersonality\n"
        "\tldr\tr1, [r4]\n"
        "\tldrb\tr1, [r1, #0x6]\n"
        "\tlsl\tr1, r1, #0x14\n"
        "\tmov\tr2, #0x80\n"
        "\tlsl\tr2, r2, #0x11\n"
        "\tadd\tr1, r1, r2\n"
        "\tlsr\tr1, r1, #0x10\n"
        "\tmov\tr2, #0x20\n"
        "\tbl\tLoadCompressedPalette\n"
        "\tb\t._700\n"
        "._702:\n"
        "\t.align\t2, 0\n"
        "._701:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "._699:\n"
        "\tldrh\tr0, [r1]\n"
        "\tmov\tr1, #0x0\n"
        "\tmov\tr2, #0x9\n"
        "\tbl\tGetMonSpritePalFromSpeciesAndPersonality\n"
        "\tldr\tr1, [r4]\n"
        "\tldrb\tr1, [r1, #0x6]\n"
        "\tlsl\tr1, r1, #0x14\n"
        "\tmov\tr2, #0x80\n"
        "\tlsl\tr2, r2, #0x11\n"
        "\tadd\tr1, r1, r2\n"
        "\tlsr\tr1, r1, #0x10\n"
        "\tmov\tr2, #0x20\n"
        "\tbl\tLoadCompressedPalette\n"
        "._700:\n"
        "\tldr\tr5, ._704       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr0, [r5]\n"
        "\tldrb\tr0, [r0, #0x6]\n"
        "\tlsl\tr0, r0, #0x5\n"
        "\tldr\tr4, ._704 + 4   @ gPlttBufferUnfaded\n"
        "\tadd\tr0, r0, r4\n"
        "\tldr\tr2, ._704 + 8   @ 0xffffff00\n"
        "\tadd\tr1, r4, r2\n"
        "\tmov\tr2, #0x10\n"
        "\tbl\tCpuSet\n"
        "\tldr\tr0, [r5]\n"
        "\tldrb\tr0, [r0, #0x6]\n"
        "\tlsl\tr0, r0, #0x5\n"
        "\tadd\tr0, r0, r4\n"
        "\tldr\tr1, ._704 + 12  @ gPlttBufferFaded\n"
        "\tmov\tr2, #0x10\n"
        "\tbl\tCpuSet\n"
        "._695:\n"
        "\tldr\tr1, ._704 + 16  @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._704 + 20  @ Task_WatanabeDebugRefreshPokemonPalette\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._703\n"
        "._705:\n"
        "\t.align\t2, 0\n"
        "._704:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\tgPlttBufferUnfaded+0x200\n"
        "\t.word\t0xffffff00\n"
        "\t.word\tgPlttBufferFaded+0x100\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugRefreshPokemonPalette+1\n"
        "._698:\n"
        "\tmov\tr0, #0x4\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._706\t@cond_branch\n"
        "\tbl\tStopCryAndClearCrySongs\n"
        "\tldr\tr0, ._707       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr0, [r0]\n"
        "\tldrh\tr0, [r0]\n"
        "\tmov\tr1, #0x0\n"
        "\tbl\tPlayCry_Normal\n"
        "._706:\n"
        "\tldr\tr2, ._707       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr1, [r2]\n"
        "\tldrb\tr0, [r1, #0x8]\n"
        "\tadd\tr0, r0, #0x4\n"
        "\tstrb\tr0, [r1, #0x8]\n"
        "\tldr\tr3, [r2]\n"
        "\tldrb\tr1, [r3, #0x8]\n"
        "\tmov\tr0, #0x1f\n"
        "\tand\tr0, r0, r1\n"
        "\tstrb\tr0, [r3, #0x8]\n"
        "\tldr\tr0, [r2]\n"
        "\tldrb\tr1, [r0, #0x7]\n"
        "\tlsl\tr1, r1, #0x1\n"
        "\tldr\tr2, ._707 + 4   @ PLTT + 0x142\n"
        "\tadd\tr1, r1, r2\n"
        "\tldr\tr2, ._707 + 8   @ sWatanabePalettePulseValues\n"
        "\tldrb\tr0, [r0, #0x8]\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tadd\tr0, r0, r2\n"
        "\tldrh\tr0, [r0]\n"
        "\tstrh\tr0, [r1]\n"
        "._703:\n"
        "\tadd\tsp, sp, #0x4\n"
        "\tpop\t{r4, r5, r6}\n"
        "\tpop\t{r0}\n"
        "\tbx\tr0\n"
        "._708:\n"
        "\t.align\t2, 0\n"
        "._707:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\tPLTT + 0x142\n"
        "\t.word\tsWatanabePalettePulseValues");
}
#endif // NONMATCHING

void Task_WatanabeDebugReloadPokemonGraphics(u8 taskId)
{
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].oam.paletteNum));
    DestroySprite(gSprites + gWatanabeGraphicsBrowserState->frontSpriteId);
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(gSprites[gWatanabeGraphicsBrowserState->backSpriteId].oam.paletteNum));
    DestroySprite(gSprites + gWatanabeGraphicsBrowserState->backSpriteId);
    FreeAndDestroyMonIconSprite(gSprites + gWatanabeGraphicsBrowserState->iconSpriteId);
    gTasks[taskId].func = Task_WatanabeDebugLoadPokemonGraphics;
}

void Task_WatanabeDebugHandlePokemonColorInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        gWatanabeGraphicsBrowserState->isColorPickerActive = 0;
        REG_WIN0H = 0x51EF;
        REG_WIN0V = 0x699F;
        gTasks[taskId].func = Task_WatanabeDebugRefreshPokemonPalette;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gWatanabeGraphicsBrowserState->isColorPickerActive = 0;
        REG_WIN0H = 0x51EF;
        REG_WIN0V = 0x699F;
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferUnfaded + 0x80, 32);
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferFaded + 0x80, 32);
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferUnfaded + 0x100 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16, 32);
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferFaded + 0x100 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16, 32);
        gTasks[taskId].func = Task_WatanabeDebugRefreshPokemonPalette;
    }
    else if (JOY_REPT(DPAD_DOWN) && gWatanabeGraphicsBrowserState->selectedRgbComponent < 2)
    {
        gWatanabeGraphicsBrowserState->selectedRgbComponent++;
    }
    else if (JOY_REPT(DPAD_UP) && gWatanabeGraphicsBrowserState->selectedRgbComponent > 0)
    {
        gWatanabeGraphicsBrowserState->selectedRgbComponent--;
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        switch (gWatanabeGraphicsBrowserState->selectedRgbComponent)
        {
            case 0:
                if (gWatanabeGraphicsBrowserState->selectedColor.r < 31)
                    gWatanabeGraphicsBrowserState->selectedColor.r++;
                gTasks[taskId].func = Task_WatanabeDebugApplyPokemonPaletteColor;
                break;
            case 1:
                if (gWatanabeGraphicsBrowserState->selectedColor.g < 31)
                    gWatanabeGraphicsBrowserState->selectedColor.g++;
                gTasks[taskId].func = Task_WatanabeDebugApplyPokemonPaletteColor;
                break;
            case 2:
                if (gWatanabeGraphicsBrowserState->selectedColor.b < 31)
                    gWatanabeGraphicsBrowserState->selectedColor.b++;
                gTasks[taskId].func = Task_WatanabeDebugApplyPokemonPaletteColor;
                break;
        }
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        switch (gWatanabeGraphicsBrowserState->selectedRgbComponent)
        {
            case 0:
                if (gWatanabeGraphicsBrowserState->selectedColor.r > 0)
                    gWatanabeGraphicsBrowserState->selectedColor.r--;
                gTasks[taskId].func = Task_WatanabeDebugApplyPokemonPaletteColor;
                break;
            case 1:
                if (gWatanabeGraphicsBrowserState->selectedColor.g > 0)
                    gWatanabeGraphicsBrowserState->selectedColor.g--;
                gTasks[taskId].func = Task_WatanabeDebugApplyPokemonPaletteColor;
                break;
            case 2:
                if (gWatanabeGraphicsBrowserState->selectedColor.b > 0)
                    gWatanabeGraphicsBrowserState->selectedColor.b--;
                gTasks[taskId].func = Task_WatanabeDebugApplyPokemonPaletteColor;
                break;
        }
    }
}

void Task_WatanabeDebugApplyPokemonPaletteColor(u8 taskId)
{
    u16 paletteColor = (gWatanabeGraphicsBrowserState->selectedColor.r & 0x001f) + ((gWatanabeGraphicsBrowserState->selectedColor.g << 5) & 0x03e0) + ((gWatanabeGraphicsBrowserState->selectedColor.b << 10) & 0x7c00);
    gPlttBufferUnfaded[gWatanabeGraphicsBrowserState->paletteColorIndex + 0x81] = paletteColor;
    gPlttBufferFaded[gWatanabeGraphicsBrowserState->paletteColorIndex + 0x81] = paletteColor;
    gPlttBufferUnfaded[0x101 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + gWatanabeGraphicsBrowserState->paletteColorIndex] = paletteColor;
    gPlttBufferFaded[0x101 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + gWatanabeGraphicsBrowserState->paletteColorIndex] = paletteColor;
    WatanabeDebug_PrintHexValue(paletteColor, 14, 10, 4);
    gTasks[taskId].func = Task_WatanabeDebugHandlePokemonColorInput;
}

void SpriteCB_WatanabeDebugColorPicker(struct Sprite *sprite)
{
    if (!gWatanabeGraphicsBrowserState->isColorPickerActive)
        sprite->invisible = TRUE;
    else
    {
        u8 shade;
        switch (sprite->data[0])
        {
            default:
            case 0:
                shade = gWatanabeGraphicsBrowserState->selectedColor.r;
                break;
            case 1:
                shade = gWatanabeGraphicsBrowserState->selectedColor.g;
                break;
            case 2:
                shade = gWatanabeGraphicsBrowserState->selectedColor.b;
                break;
        }
        sprite->x2 = 4 * shade;
        sprite->y2 = 8 * sprite->data[0];
        sprite->data[1]++;
        if (sprite->data[0] == gWatanabeGraphicsBrowserState->selectedRgbComponent && !(sprite->data[1] & 0x08))
            sprite->invisible = TRUE;
        else
            sprite->invisible = FALSE;
    }
}

void InitSeeTrainers(void)
{
    u8 spriteId;
    u16 imeBak;
    WatanabeDebug_InitScreen();
    DmaCopy16Defvars(3, sWatanabeDebugSpriteBrowserTiles, VRAM + 0xe000, 0x800);
    LoadPalette(sWatanabeDebugSpriteBrowserPalette, 0x80, 0x60);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    REG_WIN0H = 0;
    REG_WIN0V = 0;
    REG_WIN1H = 0;
    REG_WIN1V = 0;
    REG_WININ = 0x3F;
    REG_WINOUT = 0x1F;
    REG_BLDCNT = 0xF1;
    REG_BLDALPHA = 0;
    REG_BLDY = 7;
    imeBak = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = imeBak;
    SetVBlankCallback(VBlankCB_WatanabeDebug);
    SetMainCallback2(CB2_WatanabeDebugWaitForFade);
    REG_BG0CNT = 0x1F0B;
    REG_BG1CNT = 0x1E0A;
    REG_DISPCNT = 0x3340;
    CreateTask(Task_WatanabeDebugInitTrainerGraphicsBrowser, 0);
    gWatanabeGraphicsBrowserState = &eWatanabeGraphicsBrowserState;
    gWatanabeGraphicsBrowserState->graphicsId = 0;
    gWatanabeGraphicsBrowserState->frontSpriteId = 0;
    gWatanabeGraphicsBrowserState->backSpriteId = 0;
    gWatanabeGraphicsBrowserState->isColorPickerActive = 0;
    gWatanabeGraphicsBrowserState->paletteColorIndex = 0;
    gWatanabeGraphicsBrowserState->selectedRgbComponent = 0;
    gWatanabeGraphicsBrowserState->palettePulsePhase = 0;
    spriteId = CreateSprite(&sWatanabeDebugColorPickerTemplate, 0x6c, 0x74, 0);
    gSprites[spriteId].data[0] = 0;
    StartSpriteAnim(gSprites + spriteId, 0);
    spriteId = CreateSprite(&sWatanabeDebugColorPickerTemplate, 0x6c, 0x74, 0);
    gSprites[spriteId].data[0] = 1;
    StartSpriteAnim(gSprites + spriteId, 1);
    spriteId = CreateSprite(&sWatanabeDebugColorPickerTemplate, 0x6c, 0x74, 0);
    gSprites[spriteId].data[0] = 2;
    StartSpriteAnim(gSprites + spriteId, 2);
}

void Task_WatanabeDebugInitTrainerGraphicsBrowser(u8 taskId)
{
    u8 colorPickerTileIds[] = {0x00, 0x10, 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x20, 0x21};
    u8 i;

    Menu_DrawStdWindowFrame(9, 0, 14, 7);
    Menu_DrawStdWindowFrame(0, 0, 9, 9);
    Menu_DrawStdWindowFrame(0, 10, 9, 19);
    Menu_DrawStdWindowFrame(14, 0, 29, 7);
    Menu_PrintText(sText_WatanabeGraphicsBrowserPrevious, 15, 1);
    Menu_PrintText(sText_WatanabeGraphicsBrowserNext, 25, 1);
    Menu_PrintText(sText_WatanabeGraphicsBrowserDevelopmentNumber, 15, 5);

    Menu_DrawStdWindowFrame(10, 8, 29, 12);
    for (i = 0; i < 15; i++)
        ((u16 *)(VRAM + 0xFA56))[i] = 0xA311 + i;
    for (i = 0; i < 15; i++)
        ((u16 *)(VRAM + 0xF256))[i] = 0x8301 + i;

    Menu_PrintText(sText_WatanabeGraphicsBrowserSetColor, 20, 10);
    Menu_DrawStdWindowFrame(10, 13, 29, 19);

    colorPickerTileIds[0] = 0x23;
    for (i = 0; i < 18; i++)
        ((u16 *)(VRAM + 0xF396))[i] = 0x9300 + colorPickerTileIds[i];
    colorPickerTileIds[0] = 0x24;
    for (i = 0; i < 18; i++)
        ((u16 *)(VRAM + 0xF3D6))[i] = 0x9300 + colorPickerTileIds[i];
    colorPickerTileIds[0] = 0x25;
    for (i = 0; i < 18; i++)
        ((u16 *)(VRAM + 0xF416))[i] = 0x9300 + colorPickerTileIds[i];

    Menu_PrintText(sText_WatanabeGraphicsBrowserConfirmCancel, 15, 17);

    REG_WIN0H = 0x51EF;
    REG_WIN0V = 0x699F;

    gTasks[taskId].func = Task_WatanabeDebugLoadTrainerGraphics;
}

void Task_WatanabeDebugLoadTrainerGraphics(u8 taskId)
{
    DecompressPicFromTable(gTrainerFrontPicTable + gWatanabeGraphicsBrowserState->graphicsId, gTrainerFrontPicCoords[gWatanabeGraphicsBrowserState->graphicsId].coords, gTrainerFrontPicCoords[gWatanabeGraphicsBrowserState->graphicsId].y_offset, gMonSpriteGfxBufferPtrs[0], gMonSpriteGfxBufferPtrs[1], gWatanabeGraphicsBrowserState->graphicsId);
    LoadCompressedSpritePalette(gTrainerFrontPicPaletteTable + gWatanabeGraphicsBrowserState->graphicsId);
    SetMultiuseSpriteTemplateToTrainerBack(gWatanabeGraphicsBrowserState->graphicsId, 1);
    gWatanabeGraphicsBrowserState->frontSpriteId = CreateSprite(&gCreatingSpriteTemplate, 0x28, 0x28, 0);
    gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].callback = SpriteCB_WatanabeDebugDummy;
    gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].oam.priority = 0;

    WatanabeDebug_PrintDecimal3(gWatanabeGraphicsBrowserState->graphicsId, 26, 5);

    gWatanabeGraphicsBrowserState->spritePaletteNum = gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].oam.paletteNum;
    CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferUnfaded + 0x80, 0x20);
    CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferFaded + 0x80, 0x20);

    gTasks[taskId].func = Task_WatanabeDebugRefreshTrainerPalette;

    gWatanabeGraphicsBrowserState->useShinyPalette = 0;
}

void Task_WatanabeDebugRefreshTrainerPalette(u8 taskId)
{
    u16 paletteColor;
    CpuCopy16(gPlttBufferUnfaded + 0x80, gWatanabeGraphicsBrowserState->savedPalette, 32);
    paletteColor = gPlttBufferUnfaded[gWatanabeGraphicsBrowserState->paletteColorIndex + 0x81];
    gWatanabeGraphicsBrowserState->selectedColor.r = paletteColor & 0x1f;
    gWatanabeGraphicsBrowserState->selectedColor.g = (paletteColor & 0x3e0) >> 5;
    gWatanabeGraphicsBrowserState->selectedColor.b = (paletteColor & 0x7c00) >> 10;
    gWatanabeGraphicsBrowserState->selectedColor.unused_15 = 0;
    WatanabeDebug_PrintHexValue(gWatanabeGraphicsBrowserState->paletteColorIndex + 1, 11, 10, 2);
    Menu_PrintText(sText_WatanabeGraphicsBrowserColon, 13, 10);
    WatanabeDebug_PrintHexValue(paletteColor, 14, 10, 4);
    gTasks[taskId].func = Task_WatanabeDebugHandleTrainerGraphicsInput;
}

#ifdef NONMATCHING
void Task_WatanabeDebugHandleTrainerGraphicsInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        SetMainCallback2(CB2_WatanabeDebugWaitForFade);
        gTasks[taskId].func = Task_WatanabeDebugExitToField;
    }
    else if (JOY_REPT(R_BUTTON))
    {
        gWatanabeGraphicsBrowserState->graphicsId = WatanabeDebug_AdvanceTrainerGraphicsId(0, gWatanabeGraphicsBrowserState->graphicsId);
        gTasks[taskId].func = Task_WatanabeDebugReloadTrainerGraphics;
    }
    else if (JOY_REPT(L_BUTTON))
    {
        gWatanabeGraphicsBrowserState->graphicsId = WatanabeDebug_AdvanceTrainerGraphicsId(1, gWatanabeGraphicsBrowserState->graphicsId);
        gTasks[taskId].func = Task_WatanabeDebugReloadTrainerGraphics;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        gWatanabeGraphicsBrowserState->isColorPickerActive = 1;
        REG_WIN0H = 0x51EF;
        REG_WIN0V = 0x4167;
        gTasks[taskId].func = Task_WatanabeDebugHandleTrainerColorInput;
    }
    else if (JOY_REPT(DPAD_RIGHT) && gWatanabeGraphicsBrowserState->paletteColorIndex < 14)
    {
        ((u16 *)PLTT)[0x81 + gWatanabeGraphicsBrowserState->paletteColorIndex] = 0;
        do; while (0); // this matches the asm here...
        gWatanabeGraphicsBrowserState->paletteColorIndex++;
        gTasks[taskId].func = Task_WatanabeDebugRefreshTrainerPalette;
    }
    else if (JOY_REPT(DPAD_LEFT) && gWatanabeGraphicsBrowserState->paletteColorIndex > 0)
    {
        ((u16 *)PLTT)[0x81 + gWatanabeGraphicsBrowserState->paletteColorIndex] = 0;
        do; while (0); // ... but not here
        gWatanabeGraphicsBrowserState->paletteColorIndex--;
        gTasks[taskId].func = Task_WatanabeDebugRefreshTrainerPalette;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        gWatanabeGraphicsBrowserState->useShinyPalette ^= 1;
        if (gWatanabeGraphicsBrowserState->useShinyPalette)
        {
            LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(gWatanabeGraphicsBrowserState->graphicsId, 0, 0), gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, 0x20);
        }
        else
        {
            LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(gWatanabeGraphicsBrowserState->graphicsId, 0, 9), gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, 0x20);
        }
        CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferUnfaded + 0x80, 32);
        CpuCopy16(gPlttBufferUnfaded + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + 0x100, gPlttBufferFaded + 0x80, 32);
        gTasks[taskId].func = Task_WatanabeDebugRefreshTrainerPalette;
    }
    else
    {
        gWatanabeGraphicsBrowserState->palettePulsePhase += 4;
        gWatanabeGraphicsBrowserState->palettePulsePhase &= 0x1f;
        ((u16 *)PLTT)[0xa1 + gWatanabeGraphicsBrowserState->paletteColorIndex] = sWatanabePalettePulseValues[gWatanabeGraphicsBrowserState->palettePulsePhase];
    }
}
#else
NAKED void Task_WatanabeDebugHandleTrainerGraphicsInput(u8 taskId)
{
    asm("\tpush\t{r4, r5, r6, lr}\n"
        "\tadd\tsp, sp, #0xfffffffc\n"
        "\tlsl\tr0, r0, #0x18\n"
        "\tlsr\tr6, r0, #0x18\n"
        "\tldr\tr1, ._792       @ gMain\n"
        "\tldrh\tr3, [r1, #0x2e]\n"
        "\tmov\tr0, #0x2\n"
        "\tand\tr0, r0, r3\n"
        "\tadd\tr4, r1, #0\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._790\t@cond_branch\n"
        "\tmov\tr0, #0x5\n"
        "\tbl\tPlaySE\n"
        "\tmov\tr0, #0x1\n"
        "\tneg\tr0, r0\n"
        "\tmov\tr1, #0x0\n"
        "\tstr\tr1, [sp]\n"
        "\tmov\tr2, #0x0\n"
        "\tmov\tr3, #0x10\n"
        "\tbl\tBeginNormalPaletteFade\n"
        "\tldr\tr0, ._792 + 4   @ CB2_WatanabeDebugWaitForFade\n"
        "\tbl\tSetMainCallback2\n"
        "\tldr\tr1, ._792 + 8   @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._792 + 12  @ Task_WatanabeDebugExitToField\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._821\n"
        "._793:\n"
        "\t.align\t2, 0\n"
        "._792:\n"
        "\t.word\tgMain\n"
        "\t.word\tCB2_WatanabeDebugWaitForFade+1\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugExitToField+1\n"
        "._790:\n"
        "\tldrh\tr1, [r4, #0x30]\n"
        "\tmov\tr0, #0x80\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._794\t@cond_branch\n"
        "\tldr\tr4, ._796       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr0, [r4]\n"
        "\tldrh\tr1, [r0]\n"
        "\tmov\tr0, #0x0\n"
        "\tb\t._795\n"
        "._797:\n"
        "\t.align\t2, 0\n"
        "._796:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "._794:\n"
        "\tmov\tr0, #0x80\n"
        "\tlsl\tr0, r0, #0x2\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._798\t@cond_branch\n"
        "\tldr\tr4, ._800       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr0, [r4]\n"
        "\tldrh\tr1, [r0]\n"
        "\tmov\tr0, #0x1\n"
        "._795:\n"
        "\tbl\tWatanabeDebug_AdvanceTrainerGraphicsId\n"
        "\tldr\tr1, [r4]\n"
        "\tstrh\tr0, [r1]\n"
        "\tldr\tr1, ._800 + 4   @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._800 + 8   @ Task_WatanabeDebugReloadTrainerGraphics\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._821\n"
        "._801:\n"
        "\t.align\t2, 0\n"
        "._800:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugReloadTrainerGraphics+1\n"
        "._798:\n"
        "\tmov\tr2, #0x1\n"
        "\tand\tr2, r2, r3\n"
        "\tcmp\tr2, #0\n"
        "\tbeq\t._802\t@cond_branch\n"
        "\tldr\tr0, ._804       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr1, [r0]\n"
        "\tmov\tr0, #0x1\n"
        "\tstrb\tr0, [r1, #0x5]\n"
        "\tldr\tr1, ._804 + 4   @ 0x4000040\n"
        "\tldr\tr2, ._804 + 8   @ 0x51ef\n"
        "\tadd\tr0, r2, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tadd\tr1, r1, #0x4\n"
        "\tldr\tr2, ._804 + 12  @ 0x4167\n"
        "\tadd\tr0, r2, #0\n"
        "\tstrh\tr0, [r1]\n"
        "\tldr\tr1, ._804 + 16  @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._804 + 20  @ Task_WatanabeDebugHandleTrainerColorInput\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._821\n"
        "._805:\n"
        "\t.align\t2, 0\n"
        "._804:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\t0x4000040\n"
        "\t.word\t0x51ef\n"
        "\t.word\t0x4167\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugHandleTrainerColorInput+1\n"
        "._802:\n"
        "\tmov\tr0, #0x10\n"
        "\tand\tr0, r0, r1\n"
        "\tldr\tr3, ._809       @ gWatanabeGraphicsBrowserState\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._807\t@cond_branch\n"
        "\tldr\tr1, [r3]\n"
        "\tldrb\tr0, [r1, #0x7]\n"
        "\tcmp\tr0, #0xd\n"
        "\tbhi\t._807\t@cond_branch\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tldr\tr1, ._809 + 4   @ 0x5000102\n"
        "\tadd\tr0, r0, r1\n"
        "\tstrh\tr2, [r0]\n"
        "\tldr\tr1, [r3]\n"
        "\tldrb\tr0, [r1, #0x7]\n"
        "\tadd\tr0, r0, #0x1\n"
        "\tstrb\tr0, [r1, #0x7]\n"
        "\tb\t._813\n"
        "._810:\n"
        "\t.align\t2, 0\n"
        "._809:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\t0x5000102\n"
        "._807:\n"
        "\tldrh\tr1, [r4, #0x30]\n"
        "\tmov\tr0, #0x20\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._812\t@cond_branch\n"
        "\tldr\tr1, [r3]\n"
        "\tldrb\tr0, [r1, #0x7]\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._812\t@cond_branch\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tldr\tr2, ._814       @ 0x5000102\n"
        "\tadd\tr0, r0, r2\n"
        "\tmov\tr1, #0x0\n"
        "\tstrh\tr1, [r0]\n"
        "\tldr\tr1, [r3]\n"
        "\tldrb\tr0, [r1, #0x7]\n"
        "\tsub\tr0, r0, #0x1\n"
        "\tstrb\tr0, [r1, #0x7]\n"
        "\tb\t._813\n"
        "._815:\n"
        "\t.align\t2, 0\n"
        "._814:\n"
        "\t.word\t0x5000102\n"
        "._812:\n"
        "\tldrh\tr1, [r4, #0x2e]\n"
        "\tmov\tr0, #0x8\n"
        "\tand\tr0, r0, r1\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._816\t@cond_branch\n"
        "\tldr\tr4, ._819       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr2, [r4]\n"
        "\tldrb\tr0, [r2, #0x9]\n"
        "\tmov\tr1, #0x1\n"
        "\teor\tr0, r0, r1\n"
        "\tstrb\tr0, [r2, #0x9]\n"
        "\tldr\tr1, [r4]\n"
        "\tldrb\tr0, [r1, #0x9]\n"
        "\tcmp\tr0, #0\n"
        "\tbeq\t._817\t@cond_branch\n"
        "\tldrh\tr0, [r1]\n"
        "\tmov\tr1, #0x0\n"
        "\tmov\tr2, #0x0\n"
        "\tbl\tGetMonSpritePalFromSpeciesAndPersonality\n"
        "\tldr\tr1, [r4]\n"
        "\tldrb\tr1, [r1, #0x6]\n"
        "\tlsl\tr1, r1, #0x14\n"
        "\tmov\tr2, #0x80\n"
        "\tlsl\tr2, r2, #0x11\n"
        "\tadd\tr1, r1, r2\n"
        "\tlsr\tr1, r1, #0x10\n"
        "\tmov\tr2, #0x20\n"
        "\tbl\tLoadCompressedPalette\n"
        "\tb\t._818\n"
        "._820:\n"
        "\t.align\t2, 0\n"
        "._819:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "._817:\n"
        "\tldrh\tr0, [r1]\n"
        "\tmov\tr1, #0x0\n"
        "\tmov\tr2, #0x9\n"
        "\tbl\tGetMonSpritePalFromSpeciesAndPersonality\n"
        "\tldr\tr1, [r4]\n"
        "\tldrb\tr1, [r1, #0x6]\n"
        "\tlsl\tr1, r1, #0x14\n"
        "\tmov\tr2, #0x80\n"
        "\tlsl\tr2, r2, #0x11\n"
        "\tadd\tr1, r1, r2\n"
        "\tlsr\tr1, r1, #0x10\n"
        "\tmov\tr2, #0x20\n"
        "\tbl\tLoadCompressedPalette\n"
        "._818:\n"
        "\tldr\tr5, ._822       @ gWatanabeGraphicsBrowserState\n"
        "\tldr\tr0, [r5]\n"
        "\tldrb\tr0, [r0, #0x6]\n"
        "\tlsl\tr0, r0, #0x5\n"
        "\tldr\tr4, ._822 + 4   @ gPlttBufferUnfaded\n"
        "\tadd\tr0, r0, r4\n"
        "\tldr\tr2, ._822 + 8   @ 0xffffff00\n"
        "\tadd\tr1, r4, r2\n"
        "\tmov\tr2, #0x10\n"
        "\tbl\tCpuSet\n"
        "\tldr\tr0, [r5]\n"
        "\tldrb\tr0, [r0, #0x6]\n"
        "\tlsl\tr0, r0, #0x5\n"
        "\tadd\tr0, r0, r4\n"
        "\tldr\tr1, ._822 + 12  @ gPlttBufferFaded\n"
        "\tmov\tr2, #0x10\n"
        "\tbl\tCpuSet\n"
        "._813:\n"
        "\tldr\tr1, ._822 + 16  @ gTasks\n"
        "\tlsl\tr0, r6, #0x2\n"
        "\tadd\tr0, r0, r6\n"
        "\tlsl\tr0, r0, #0x3\n"
        "\tadd\tr0, r0, r1\n"
        "\tldr\tr1, ._822 + 20  @ Task_WatanabeDebugRefreshTrainerPalette\n"
        "\tstr\tr1, [r0]\n"
        "\tb\t._821\n"
        "._823:\n"
        "\t.align\t2, 0\n"
        "._822:\n"
        "\t.word\tgWatanabeGraphicsBrowserState\n"
        "\t.word\tgPlttBufferUnfaded+0x200\n"
        "\t.word\t0xffffff00\n"
        "\t.word\tgPlttBufferFaded+0x100\n"
        "\t.word\tgTasks\n"
        "\t.word\tTask_WatanabeDebugRefreshTrainerPalette+1\n"
        "._816:\n"
        "\tldr\tr1, [r3]\n"
        "\tldrb\tr0, [r1, #0x8]\n"
        "\tadd\tr0, r0, #0x4\n"
        "\tstrb\tr0, [r1, #0x8]\n"
        "\tldr\tr2, [r3]\n"
        "\tldrb\tr1, [r2, #0x8]\n"
        "\tmov\tr0, #0x1f\n"
        "\tand\tr0, r0, r1\n"
        "\tstrb\tr0, [r2, #0x8]\n"
        "\tldr\tr0, [r3]\n"
        "\tldrb\tr1, [r0, #0x7]\n"
        "\tlsl\tr1, r1, #0x1\n"
        "\tldr\tr2, ._824       @ 0x5000142\n"
        "\tadd\tr1, r1, r2\n"
        "\tldr\tr2, ._824 + 4   @ sWatanabePalettePulseValues\n"
        "\tldrb\tr0, [r0, #0x8]\n"
        "\tlsl\tr0, r0, #0x1\n"
        "\tadd\tr0, r0, r2\n"
        "\tldrh\tr0, [r0]\n"
        "\tstrh\tr0, [r1]\n"
        "._821:\n"
        "\tadd\tsp, sp, #0x4\n"
        "\tpop\t{r4, r5, r6}\n"
        "\tpop\t{r0}\n"
        "\tbx\tr0\n"
        "._825:\n"
        "\t.align\t2, 0\n"
        "._824:\n"
        "\t.word\t0x5000142\n"
        "\t.word\tsWatanabePalettePulseValues");
}
#endif // NONMATCHING

void Task_WatanabeDebugReloadTrainerGraphics(u8 taskId)
{
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(gSprites[gWatanabeGraphicsBrowserState->frontSpriteId].oam.paletteNum));
    DestroySprite(gSprites + gWatanabeGraphicsBrowserState->frontSpriteId);
    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(gSprites[gWatanabeGraphicsBrowserState->backSpriteId].oam.paletteNum));
    DestroySprite(gSprites + gWatanabeGraphicsBrowserState->backSpriteId);
    FreeAndDestroyMonIconSprite(gSprites + gWatanabeGraphicsBrowserState->iconSpriteId);
    gTasks[taskId].func = Task_WatanabeDebugLoadTrainerGraphics;
}

void Task_WatanabeDebugHandleTrainerColorInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        gWatanabeGraphicsBrowserState->isColorPickerActive = 0;
        REG_WIN0H = 0x51EF;
        REG_WIN0V = 0x699F;
        gTasks[taskId].func = Task_WatanabeDebugRefreshTrainerPalette;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gWatanabeGraphicsBrowserState->isColorPickerActive = 0;
        REG_WIN0H = 0x51EF;
        REG_WIN0V = 0x699F;
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferUnfaded + 0x80, 32);
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferFaded + 0x80, 32);
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferUnfaded + 0x100 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16, 32);
        CpuCopy16(gWatanabeGraphicsBrowserState->savedPalette, gPlttBufferFaded + 0x100 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16, 32);
        gTasks[taskId].func = Task_WatanabeDebugRefreshTrainerPalette;
    }
    else if (JOY_REPT(DPAD_DOWN) && gWatanabeGraphicsBrowserState->selectedRgbComponent < 2)
    {
        gWatanabeGraphicsBrowserState->selectedRgbComponent++;
    }
    else if (JOY_REPT(DPAD_UP) && gWatanabeGraphicsBrowserState->selectedRgbComponent > 0)
    {
        gWatanabeGraphicsBrowserState->selectedRgbComponent--;
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        switch (gWatanabeGraphicsBrowserState->selectedRgbComponent)
        {
            case 0:
                if (gWatanabeGraphicsBrowserState->selectedColor.r < 31)
                    gWatanabeGraphicsBrowserState->selectedColor.r++;
                gTasks[taskId].func = Task_WatanabeDebugApplyTrainerPaletteColor;
                break;
            case 1:
                if (gWatanabeGraphicsBrowserState->selectedColor.g < 31)
                    gWatanabeGraphicsBrowserState->selectedColor.g++;
                gTasks[taskId].func = Task_WatanabeDebugApplyTrainerPaletteColor;
                break;
            case 2:
                if (gWatanabeGraphicsBrowserState->selectedColor.b < 31)
                    gWatanabeGraphicsBrowserState->selectedColor.b++;
                gTasks[taskId].func = Task_WatanabeDebugApplyTrainerPaletteColor;
                break;
        }
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        switch (gWatanabeGraphicsBrowserState->selectedRgbComponent)
        {
            case 0:
                if (gWatanabeGraphicsBrowserState->selectedColor.r > 0)
                    gWatanabeGraphicsBrowserState->selectedColor.r--;
                gTasks[taskId].func = Task_WatanabeDebugApplyTrainerPaletteColor;
                break;
            case 1:
                if (gWatanabeGraphicsBrowserState->selectedColor.g > 0)
                    gWatanabeGraphicsBrowserState->selectedColor.g--;
                gTasks[taskId].func = Task_WatanabeDebugApplyTrainerPaletteColor;
                break;
            case 2:
                if (gWatanabeGraphicsBrowserState->selectedColor.b > 0)
                    gWatanabeGraphicsBrowserState->selectedColor.b--;
                gTasks[taskId].func = Task_WatanabeDebugApplyTrainerPaletteColor;
                break;
        }
    }
}

void Task_WatanabeDebugApplyTrainerPaletteColor(u8 taskId)
{
    u16 paletteColor = (gWatanabeGraphicsBrowserState->selectedColor.r & 0x001f) + ((gWatanabeGraphicsBrowserState->selectedColor.g << 5) & 0x03e0) + ((gWatanabeGraphicsBrowserState->selectedColor.b << 10) & 0x7c00);
    gPlttBufferUnfaded[gWatanabeGraphicsBrowserState->paletteColorIndex + 0x81] = paletteColor;
    gPlttBufferFaded[gWatanabeGraphicsBrowserState->paletteColorIndex + 0x81] = paletteColor;
    gPlttBufferUnfaded[0x101 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + gWatanabeGraphicsBrowserState->paletteColorIndex] = paletteColor;
    gPlttBufferFaded[0x101 + gWatanabeGraphicsBrowserState->spritePaletteNum * 16 + gWatanabeGraphicsBrowserState->paletteColorIndex] = paletteColor;
    WatanabeDebug_PrintHexValue(paletteColor, 14, 10, 4);
    gTasks[taskId].func = Task_WatanabeDebugHandleTrainerColorInput;
}

#endif // DEBUG
