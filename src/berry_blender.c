#include "global.h"
#include "constants/rgb.h"
#include "item_menu_icons.h"
#include "item_menu.h"
#include "berry.h"
#include "berry_blender.h"
#include "decompress.h"
#include "event_data.h"
#include "graphics.h"
#include "ewram.h"
#include "item.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "menu_cursor.h"
#include "overworld.h"
#include "palette.h"
#include "pokeblock.h"
#include "random.h"
#include "save.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "strings2.h"
#include "task.h"
#include "text_window.h"
#include "trig.h"
#include "constants/songs.h"
#include "constants/items.h"
#include "constants/berry.h"
#define SCORE_BEST      0
#define SCORE_GOOD      1
#define SCORE_MISS      2

#define BLENDER_MAX_PLAYERS 4
#define NUM_SCORE_TYPES   3


struct BlenderBerry
{
    u16 itemId;
    u8 name[7];
    u8 flavors[5];
    u8 smoothness;
};

struct BerryBlender
{
    u8 mainState;
    u8 loadGfxState;
    struct Window field_4;
    u8 field_35;
    u8 field_36;
    u8 field_37;
    u8 field_38;
    u8 field_39;
    u8 field_3A;
    u8 field_3B;
    u8 field_3C;
    u8 field_3D;
    u8 field_3E;
    u8 field_3F;
    u8 field_40;
    u8 field_41;
    u8 field_42;
    u8 field_43;
    u8 field_44;
    u8 field_45;
    u8 field_46;
    u8 field_47;
    u8 field_48;
    u8 field_49;
    u8 field_4A;
    u8 field_4B;
    u8 field_4C;
    u8 field_4D;
    u16 field_4E;
    u8 scoreIconIds[3];
    u16 arrowPos;
    s16 speed;
    s16 field_58;
    u16 maxRPM;
    u8 playerArrowSpriteIds[BLENDER_MAX_PLAYERS];
    u8 playerArrowSpriteIds2[BLENDER_MAX_PLAYERS];
    u8 field_64;
    u8 field_65;
    u8 field_66;
    u8 field_67;
    u8 field_68;
    u8 field_69;
    u8 field_6A;
    u8 field_6B;
    u8 field_6C;
    u8 field_6D;
    u8 field_6E;
    u8 gameEndState;
    u16 playerContinueResponses[BLENDER_MAX_PLAYERS];
    u16 canceledPlayerCmd;
    u16 canceledPlayerId;
    u16 playAgainState;
    u8 slowdownTimer;
    u8 field_7F;
    u16 chosenItemId[BLENDER_MAX_PLAYERS];
    u8 numPlayers;
    u8 field_89;
    u8 field_8A;
    u8 field_8B;
    u8 field_8C;
    u8 field_8D;
    u8 field_8E;
    u8 field_8F;
    u8 field_90;
    u8 field_91;
    u8 field_92;
    u8 field_93;
    u16 field_94;
    u8 field_96;
    u8 field_97;
    u8 field_98;
    u8 field_99;
    u16 arrowIdToPlayerId[BLENDER_MAX_PLAYERS];
    u16 playerIdToArrowId[BLENDER_MAX_PLAYERS];
    u8 yesNoAnswer;
    u8 stringVar[129];
    u32 gameFrameTime;
    s32 framesToWait;
    u32 field_134;
    u8 field_138;
    u8 field_139;
    u8 field_13A;
    u8 field_13B;
    u8 playerToThrowBerry;
    u8 field_13D;
    u16 progressBarValue;
    u16 maxProgressBarValue;
    u16 centerScale;
    s16 bg_X;
    s16 bg_Y;
    u8 opponentTaskIds[3];
    u8 perfectOpponents;
    u16 scores[BLENDER_MAX_PLAYERS][3];
    u8 playerPlaces[BLENDER_MAX_PLAYERS];
    struct BgAffineDstData field_168;
    u16 savedMusic;
    struct BlenderBerry blendedBerries[BLENDER_MAX_PLAYERS];
    u32 field_1BC;
    u16 field_1C0;
    u16 field_1C2;
    u32 linkPlayAgainState;
};

struct BlenderDebug
{
    s8 cursorPos;
    s8 berries[4];
    struct Pokeblock pokeblock;
    u8 field_10;
    u8 spicy;
    u8 dry;
    u8 sweet;
    u8 bitter;
    u8 sour;
    u8 feel;
    s8 field_17;
    s8 field_18;
    s8 field_19;
    s16 BPM;
};

// other files functions
void MenuCursor_SetPos814A880(u8 x, u8 y);
u8 MenuCursor_Create814A5C0(u8 subpriority, u16 paletteTag, u8 a3, u16 a4, u8 rowWidth);
#ifdef GERMAN
extern void de_sub_8073110();
#endif

extern u8 gUnknown_020297ED;

extern const struct WindowTemplate gWindowTemplate_81E6F68;


// ewram
static EWRAM_DATA u8 gUnknown_020297DC = 0;
static EWRAM_DATA u32 gUnknown_020297E0 = 0;
static EWRAM_DATA u32 gUnknown_020297E4 = 0;
static EWRAM_DATA u8 gUnknown_020297E8 = 0;

// iwram common
u16 gUnknown_03004830;
u8 gInGameOpponentsNo;
u16 gUnknown_03004840[10];
struct BerryBlender* gBerryBlender;

// iwram bss
static s16 sPokeblockFlavors[8];
static s16 sPokeblockPresentFlavors[6];
static s16 sDebug_MaxRPMStage;
static s16 sDebug_GameTimeStage;
static s32 sDebug_PokeblockFactorFlavors[6];
static s32 sDebug_PokeblockFactorFlavorsAfterRPM[5];
static u32 sDebug_PokeblockFactorRPM;
static struct BlenderDebug sBlenderDebug;

// this file's functions

static void sub_80514A4(void);
static void SetBgPos(void);
static void CB2_LoadBerryBlender(void);
static void InitLocalPlayers(u8 opponentsNum);
static void UpdateBlenderCenter(void);
static void CB2_StartBlenderLink(void);
static void CB2_StartBlenderLocal(void);
static void DrawBlenderCenter(struct BgAffineDstData *dest);
static void SetPlayerIdMaps(void);
static void CB2_PlayBlender(void);
static bool8 UpdateBlenderLandScreenShake(void);
static void PrintPlayerNames(void);
static void ResetLinkCmds(void);
static void CreateParticleSprites(void);
static void SpriteCB_ScoreSymbolBest(struct Sprite* sprite);
static void ShakeBgCoordForHit(s16* coord, u16 speed);
static void TryUpdateProgressBar(u16 current, u16 limit);
static void UpdateRPM(u16 speed);
static void RestoreBgCoords(void);
static void ProcessLinkPlayerCmds(void);
static void CB2_EndBlenderGame(void);
static bool8 PrintBlendingRanking(void);
bool8 PrintBlendingResults(void);
static void CB2_CheckPlayAgainLocal(void);
static void CB2_CheckPlayAgainLink(void);
static void UpdateProgressBar(u16 value, u16 limit);
static void PrintMadePokeblockString(struct Pokeblock* pokeblock, u8* dst);
static void Task_PlayPokeblockFanfare(u8 taskId);
static void sub_8052AF8(void);
static void Task_HandleOpponent1(u8 taskId);
static void Task_HandleOpponent2(u8 taskId);
static void Task_HandleOpponent3(u8 taskId);
static void SpriteCB_PlayerArrow(struct Sprite* sprite);
static void SpriteCB_ScoreSymbol(struct Sprite* sprite);
static void SpriteCB_CountdownNumber(struct Sprite* sprite);
static void SpriteCB_Start(struct Sprite* sprite);

// const data
static const u16 sBlenderCenter_Pal[] = INCBIN_U16("graphics/berry_blender/center.gbapal");
static const u8 sBlenderCenter_Tilemap[] = INCBIN_U8("graphics/berry_blender/center_map.bin");
static const u16 sBlenderOuter_Pal[] = INCBIN_U16("graphics/berry_blender/outer.gbapal");

// unreferenced pals?
static const u16 sUnused_Pal[] = INCBIN_U16("graphics/berry_blender/unused.gbapal");
static const u16 sEmpty_Pal[224] = {0};

// unreferenced Japanese strings
static const u8 sUnknownJpnString0[] = _("▶");
static const u8 sUnknownJpnString1[] = _(" ");
static const u8 sUnknownJpnString2[] = _("カッコイ"); // "cool" (missing an イ at the end)
static const u8 sUnknownJpnString3[] = _("カワイイ"); // "cute"
static const u8 sUnknownJpnString4[] = _("ウツクシ"); // "beautiful" (missing an イ at the end)
static const u8 sUnknownJpnString5[] = _("カシコイ"); // "smart"
static const u8 sUnknownJpnString6[] = _("タクマシ"); // "tough" (missing an イ at the end)

static const u8 sText_NewParagraph[] = _("\p");

// unreferenced; These appear to be the first names of four people who worked on the game.
static const u8 sUnknownJpnString7[10] = _("てつじ"); // Tetsuji (Ohta)
static const u8 sUnknownJpnString8[10] = _("あきと"); // Akito (Mori)
static const u8 sUnknownJpnString9[10] = _("シゲル"); // Shigeru (Ohmori)
static const u8 sUnknownJpnString10[10] = _("ヨシノリ"); // Yoshinori (Matsuda)

static const u8 sUnknownText_2Pok[] = _("2Pok");
static const u8 sUnknownText_3Pok[] = _("3Pok");
static const u8 sUnknownText_4Pok[] = _("4Pok");
static const u8* const gUnknown_08216284[] =
{
      sUnknownText_2Pok, sUnknownText_3Pok, sUnknownText_4Pok
};

// unreferenced player strings
static const u8 sUnknown1PString[4] = _("1P");
static const u8 sUnknown2PString[4] = _("2P");
static const u8 sUnknown3PString[4] = _("3P");
static const u8 sUnknown4PString[4] = _("4P");

#ifdef ENGLISH
static const u8 sText_Mister[] = _("MISTER");
static const u8 sText_Laddie[] = _("LADDIE");
static const u8 sText_Lassie[] = _("LASSIE");
#else // GERMAN
static const u8 sText_Mister[] = _("OPI");
static const u8 sText_Laddie[] = _("KUMPEL");
static const u8 sText_Lassie[] = _("TUSSI");
#endif // ENGLISH
static const u8* const sBlenderOpponentsNames[] =
{
    sText_Mister, sText_Laddie, sText_Lassie
};

static const u8 sRedColorString[] = _("{COLOR RED}");
static const u8 sText_NewLine[] = _("\n");
static const u8 sSpaceString_0[] = _(" ");

static const s8 sPlayerArrowQuadrant[][2] =
{
    {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
};

static const u8 gUnknown_082162D4[][2] =
{
    {2, 6}, {23, 6}, {2, 12}, {23, 12}, {1, 6}, {22, 6}, {1, 12}, {22, 12}
};

static const u8 sPlayerArrowPos[][2] =
{
    {72, 32}, {168, 32}, {72, 128}, {168, 128}
};

static const u8 sPlayerIdMap[3][4] =
{
    {-1, 0, 1, -1}, {-1, 0, 1, 2}, {0, 1, 2, 3}
};

static const u16 sArrowStartPos[] = {0, 0xC000, 0x4000, 0x8000};
static const u8 sArrowStartPosIds[] = {1, 1, 0};
static const u8 sArrowHitRangeStart[] = {32, 224, 96, 160, 0};

static const TaskFunc sLocalOpponentTasks[] =
{
    Task_HandleOpponent1, Task_HandleOpponent2, Task_HandleOpponent3
};

static const struct OamData sOam_PlayerArrow =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_PlayerArrow_TopLeft[] =
{
    ANIMCMD_FRAME(16, 5, 1, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_TopRight[] =
{
    ANIMCMD_FRAME(16, 5, .vFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_BottomLeft[] =
{
    ANIMCMD_FRAME(16, 5, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_BottomRight[] =
{
    ANIMCMD_FRAME(16, 5, 0, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_TopLeft_Flash[] =
{
    ANIMCMD_FRAME(48, 2, 1, 1),
    ANIMCMD_FRAME(32, 5, 1, 1),
    ANIMCMD_FRAME(48, 3, 1, 1),
    ANIMCMD_FRAME(16, 5, 1, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_TopRight_Flash[] =
{
    ANIMCMD_FRAME(48, 2, .vFlip = TRUE),
    ANIMCMD_FRAME(32, 5, .vFlip = TRUE),
    ANIMCMD_FRAME(48, 3, .vFlip = TRUE),
    ANIMCMD_FRAME(16, 5, .vFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_BottomLeft_Flash[] =
{
    ANIMCMD_FRAME(48, 2, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(48, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 5, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_BottomRight_Flash[] =
{
    ANIMCMD_FRAME(48, 2, 0, 0),
    ANIMCMD_FRAME(32, 5, 0, 0),
    ANIMCMD_FRAME(48, 3, 0, 0),
    ANIMCMD_FRAME(16, 5, 0, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_TopLeft_Off[] =
{
    ANIMCMD_FRAME(0, 5, 1, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_TopRight_Off[] =
{
    ANIMCMD_FRAME(0, 5, .vFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_BottomLeft_Off[] =
{
    ANIMCMD_FRAME(0, 5, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PlayerArrow_BottomRight_Off[] =
{
    ANIMCMD_FRAME(0, 5, 0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_PlayerArrow[] =
{
    sAnim_PlayerArrow_TopLeft,
    sAnim_PlayerArrow_TopRight,
    sAnim_PlayerArrow_BottomLeft,
    sAnim_PlayerArrow_BottomRight,
    sAnim_PlayerArrow_TopLeft_Flash,
    sAnim_PlayerArrow_TopRight_Flash,
    sAnim_PlayerArrow_BottomLeft_Flash,
    sAnim_PlayerArrow_BottomRight_Flash,
    sAnim_PlayerArrow_TopLeft_Off,
    sAnim_PlayerArrow_TopRight_Off,
    sAnim_PlayerArrow_BottomLeft_Off,
    sAnim_PlayerArrow_BottomRight_Off
};

static const struct SpriteSheet sSpriteSheet_PlayerArrow =
{
    gBerryBlenderPlayerArrow_Gfx, 0x800, 46545
};

static const struct SpritePalette sSpritePal_BlenderMisc =
{
    gBerryBlenderMiscPalette, 46546
};

static const struct SpritePalette sSpritePal_PlayerArrow =
{
    gBerryBlenderArrowPalette, 12312
};

static const struct SpriteTemplate sSpriteTemplate_PlayerArrow =
{
    .tileTag = 46545,
    .paletteTag = 12312,
    .oam = &sOam_PlayerArrow,
    .anims = sAnims_PlayerArrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PlayerArrow
};

static const struct OamData sOam_ScoreSymbols =
{
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
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_ScoreSymbols_Good[] =
{
    ANIMCMD_FRAME(0, 20),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ScoreSymbols_Miss[] =
{
    ANIMCMD_FRAME(4, 20, 1, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ScoreSymbols_BestFlash[] =
{
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ScoreSymbols_BestStatic[] =
{
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_ScoreSymbols[] =
{
    sAnim_ScoreSymbols_Good,
    sAnim_ScoreSymbols_Miss,
    sAnim_ScoreSymbols_BestFlash,
    sAnim_ScoreSymbols_BestStatic,
};

static const struct SpriteSheet sSpriteSheet_ScoreSymbols =
{
    gBerryBlenderScoreSymbols_Gfx, 0x200, 48888
};

static const struct SpriteTemplate sSpriteTemplate_ScoreSymbols =
{
    .tileTag = 48888,
    .paletteTag = 46546,
    .oam = &sOam_ScoreSymbols,
    .anims = sAnims_ScoreSymbols,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_ScoreSymbol
};

static const struct OamData sOam_Particles =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_SparkleCrossToX[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_END
};

static const union AnimCmd sAnim_SparkleXToCross[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_END
};

static const union AnimCmd sAnim_SparkleFull[] =
{
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_FRAME(2, 2),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(3, 3),
    ANIMCMD_FRAME(2, 2),
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_END
};

static const union AnimCmd sAnim_GreenArrow[] =
{
    ANIMCMD_FRAME(5, 5, 1, 1),
    ANIMCMD_END
};

static const union AnimCmd sAnim_GreenDot[] =
{
    ANIMCMD_FRAME(6, 5, 1, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Particles[] =
{
    sAnim_SparkleCrossToX,
    sAnim_SparkleXToCross,
    sAnim_SparkleFull,
    sAnim_GreenArrow,
    sAnim_GreenDot,
};

static const struct SpriteSheet sSpriteSheet_Particles =
{
    gBerryBlenderParticles_Gfx, 0xE0, 23456
};

static const struct SpriteTemplate sSpriteTemplate_Particles =
{
    .tileTag = 23456,
    .paletteTag = 46546,
    .oam = &sOam_Particles,
    .anims = sAnims_Particles,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData sOam_CountdownNumbers =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_CountdownNumbers_3[] =
{
    ANIMCMD_FRAME(32, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_CountdownNumbers_2[] =
{
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_CountdownNumbers_1[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_CountdownNumbers[] =
{
    sAnim_CountdownNumbers_3,
    sAnim_CountdownNumbers_2,
    sAnim_CountdownNumbers_1,
};

static const struct SpriteSheet sSpriteSheet_CountdownNumbers =
{
    gBerryBlenderCountdownNumbers_Gfx, 0x600, 12345
};

static const struct SpriteTemplate sSpriteTemplate_CountdownNumbers =
{
    .tileTag = 12345,
    .paletteTag = 46546,
    .oam = &sOam_CountdownNumbers,
    .anims = sAnims_CountdownNumbers,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CountdownNumber
};

static const struct OamData sOam_Start =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_Start[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Start[] =
{
    sAnim_Start,
};

static const struct SpriteSheet sSpriteSheet_Start =
{
    gBerryBlenderStart_Gfx, 0x400, 12346
};

static const struct SpriteTemplate sSpriteTemplate_Start =
{
    .tileTag = 12346,
    .paletteTag = 46546,
    .oam = &sOam_Start,
    .anims = sAnims_Start,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Start
};

static const s16 sBerrySpriteData[][5] =
{
    {-10,  20,  10,   2,   1},
    {250,  20,  10,  -2,   1},
    {-10, 140,  10,   2,  -1},
    {250, 140,  10,  -2,  -1},
};

static const u8 sOpponentBerrySets[][3] =
{
    {4, 3, 2}, {0, 4, 3}, {1, 0, 4}, {2, 1, 0}, {3, 2, 1}, {0, 2, 3}, {1, 3, 4}, {2, 4, 0}, {3, 0, 1}, {4, 1, 2},
};

static const u8 sNumPlayersToSpeedDivisor[] = {1, 1, 2, 3, 4};
static const u8 sBlackPokeblockFlavorFlags[] = {0x1C, 0x16, 0x13, 0x1A, 0x19, 0x0E, 0x0D, 0x0B, 0x07, 0x15};
static const u8 gUnknown_082165E9[] = {6, 6, 6, 6, 5};
static const u8 gUnknown_082165EE[] = {3, 3, 3, 2, 2};
static const u8 gUnknown_082165F3[] = {3, 3, 3, 3, 2};

static const u8 sText_Space[] = _(" ");
static const u8 sText_BPM[] = _("BPM");
static const u8 sText_Dash[] = _("-");
static const u8 sNewLineString_1[] = _("\n");
static const u8 sNewLineString_2[] = _("\n");

static void UpdateHitPitch(void)
{
    m4aMPlayPitchControl(&gMPlayInfo_SE2, 0xFFFF, (gBerryBlender->speed - 128) * 2);
}

static void VBlankCB_BerryBlender(void)
{
    sub_80514A4();
    SetBgPos();
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void VBlankCB1_BerryBlender(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static bool8 LoadBerryBlenderGfx(void)
{
    switch (gBerryBlender->loadGfxState)
    {
    case 0:
        LZDecompressWram(gBerryBlenderCenter_Gfx, eBerryBlenderGfxBuffer);
        gBerryBlender->loadGfxState++;
        break;
    case 1:
        DmaCopy16Defvars(3, sBlenderCenter_Tilemap, (void *)(VRAM + 0x4000), 0x400);
        LoadPalette(sBlenderCenter_Pal, 0, 0x100);
        gBerryBlender->loadGfxState++;
        break;
    case 2:
        DmaCopyLarge16(3, eBerryBlenderGfxBuffer, (void *)(VRAM + 0x0), 0x2000, 0x1000);
        gBerryBlender->loadGfxState++;
        break;
    case 3:
        LZDecompressWram(gBerryBlenderOuter_Gfx, eBerryBlenderGfxBuffer);
        gBerryBlender->loadGfxState++;
        break;
    case 4:
        LZDecompressWram(gBerryBlenderOuter_Tilemap, eBerryBlenderGfxBuffer + 0x3000);
        gBerryBlender->loadGfxState++;
        break;
    case 5:
        DmaCopy16Defvars(3, eBerryBlenderGfxBuffer + 0x0000, (void *)(VRAM + 0xE000), 0x1000);
        gBerryBlender->loadGfxState++;
        break;
    case 6:
		DmaCopy16Defvars(3, eBerryBlenderGfxBuffer + 0x1000, (void *)(VRAM + 0xF000), 0x1000);
        gBerryBlender->loadGfxState++;
        break;
    case 7:
        {
            u16 i;
            u16* palStore = (u16*)(eBerryBlenderGfxBuffer + 0x3000);

            for (i = 0; i < 640; i++)
            {
                *(palStore + i) |= 0x100;
            }
            DmaCopy16Defvars(3, eBerryBlenderGfxBuffer + 0x3000, (void *)(VRAM + 0x6000), 0x500);
            LoadPalette(sBlenderOuter_Pal, 0x80, 0x20);
            gBerryBlender->loadGfxState++;
        }
        break;
    case 8:
        LoadSpriteSheet(&sSpriteSheet_PlayerArrow);
        LoadSpriteSheet(&sSpriteSheet_Particles);
        LoadSpriteSheet(&sSpriteSheet_ScoreSymbols);
        gBerryBlender->loadGfxState++;
        break;
    case 9:
        LoadSpriteSheet(&sSpriteSheet_CountdownNumbers);
        LoadSpriteSheet(&sSpriteSheet_Start);
        LoadSpritePalette(&sSpritePal_PlayerArrow);
        LoadSpritePalette(&sSpritePal_BlenderMisc);
        gBerryBlender->loadGfxState = 0;
        return TRUE;
    }
    return FALSE;
}

static void DrawBlenderBg(void)
{
    REG_DISPCNT = 0x1341;
    REG_BG2CNT = 0x4880;
    REG_BG1CNT = 0xC0D;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
}

void DoBerryBlending(void)
{
    u8* gameEndState; //this temp value is needed to match

    gBerryBlender = eBerryBlenderData;

    gameEndState = &gBerryBlender->gameEndState;
    gBerryBlender->mainState = 0;
    *gameEndState = 0;

    InitLocalPlayers(gSpecialVar_0x8004);
    SetMainCallback2(CB2_LoadBerryBlender);
}

static void CB2_LoadBerryBlender(void)
{
    s32 i;
    switch (gBerryBlender->mainState)
    {
    case 0:
        REG_DISPCNT = 0;
        ResetSpriteData();
        FreeAllSpritePalettes();
        SetVBlankCallback(NULL);
        Text_LoadWindowTemplate(&gWindowTemplate_81E6F68);
        InitMenuWindow(&gWindowTemplate_81E6F68);
        gBerryBlender->mainState++;
        gBerryBlender->maxProgressBarValue = 0;
        gBerryBlender->progressBarValue = 0;
        gBerryBlender->centerScale = 0x50;
        gBerryBlender->bg_X = 0;
        gBerryBlender->bg_Y = 0;
        gBerryBlender->loadGfxState = 0;
        UpdateBlenderCenter();
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
            {
                gBerryBlender->playerArrowSpriteIds[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
                StartSpriteAnim(&gSprites[gBerryBlender->playerArrowSpriteIds[i]], i + 8);
            }
            SetVBlankCallback(VBlankCB_BerryBlender);
            gBerryBlender->mainState++;
        }
        break;
    case 2:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        UpdateBlenderCenter();
        gBerryBlender->mainState++;
        break;
    case 3:
        DrawBlenderBg();
        if (!gPaletteFade.active)
        {
            gBerryBlender->mainState++;
        }
        break;
    case 4:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        MenuPrintMessage(sText_BerryBlenderStart, 1, 15);
        gBerryBlender->mainState++;
        break;
    case 5:
        if (Menu_UpdateWindowText())
        {
            gBerryBlender->mainState++;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
        break;
    case 6:
        if (!gPaletteFade.active)
        {
            ChooseBerryForMachine();
            gBerryBlender->mainState = 0;
        }
        break;
    }
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void SpriteCB_Berry(struct Sprite* sprite)
{
    sprite->data[1] += sprite->data[6];
    sprite->data[2] -= sprite->data[4];
    sprite->data[2] += sprite->data[7];
    sprite->data[0] += sprite->data[7];
    sprite->data[4]--;

    if (sprite->data[0] < sprite->data[2])
    {
        sprite->data[3] = sprite->data[4] = sprite->data[3] - 1;
        if (++sprite->data[5] > 3)
            DestroySprite(sprite);
        else
            PlaySE(SE_BALL_TRAY_EXIT);
    }
    sprite->x = sprite->data[1];
    sprite->y = sprite->data[2];
}

static void SetBerrySpriteData(struct Sprite* sprite, s16 x, s16 y, s16 bounceSpeed, s16 xSpeed, s16 ySpeed)
{
    sprite->data[0] = y;
    sprite->data[1] = x;
    sprite->data[2] = y;
    sprite->data[3] = bounceSpeed;
    sprite->data[4] = 10;
    sprite->data[5] = 0;
    sprite->data[6] = xSpeed;
    sprite->data[7] = ySpeed;
    sprite->callback = SpriteCB_Berry;
}

static void CreateBerrySprite(u16 itemId, u8 playerId)
{
    u8 spriteId = CreateSpinningBerrySprite(itemId + 123, 0, 80, playerId & 1);
    SetBerrySpriteData(&gSprites[spriteId], sBerrySpriteData[playerId][0], sBerrySpriteData[playerId][1], sBerrySpriteData[playerId][2], sBerrySpriteData[playerId][3], sBerrySpriteData[playerId][4]);
}

static void ConvertItemToBlenderBerry(struct BlenderBerry* berry, u16 itemId)
{
    const struct Berry *berryInfo = GetBerryInfo(itemId + 124);
    berry->itemId = itemId;
    StringCopy(berry->name, berryInfo->name);
    berry->flavors[FLAVOR_SPICY] = berryInfo->spicy;
    berry->flavors[FLAVOR_DRY] = berryInfo->dry;
    berry->flavors[FLAVOR_SWEET] = berryInfo->sweet;
    berry->flavors[FLAVOR_BITTER] = berryInfo->bitter;
    berry->flavors[FLAVOR_SOUR] = berryInfo->sour;
    berry->smoothness = berryInfo->smoothness;
}

static void InitLocalPlayers(u8 opponentsNum)
{
    int i;
    if (opponentsNum)
    {
        for (i = 0; i < 4; i++)
            gLinkPlayers[i].language = GAME_LANGUAGE;
    }
    switch (opponentsNum)
    {
    case 0:
        gInGameOpponentsNo = 0;
        break;
    case 1:
        gInGameOpponentsNo = 1;
        gBerryBlender->numPlayers = 2;
        StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
        StringCopy(gLinkPlayers[1].name, sBlenderOpponentsNames[0]);
        break;
    case 2:
        gInGameOpponentsNo = 2;
        gBerryBlender->numPlayers = 3;
        StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
        StringCopy(gLinkPlayers[1].name, sBlenderOpponentsNames[0]);
        StringCopy(gLinkPlayers[2].name, sBlenderOpponentsNames[1]);
        break;
    case 3:
        gInGameOpponentsNo = 3;
        gBerryBlender->numPlayers = 4;
        StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
        StringCopy(gLinkPlayers[1].name, sBlenderOpponentsNames[0]);
        StringCopy(gLinkPlayers[2].name, sBlenderOpponentsNames[1]);
        StringCopy(gLinkPlayers[3].name, sBlenderOpponentsNames[2]);
        break;
    }
}

void StartBlender(void)
{
    s32 i;

    REG_DISPCNT = 0;
    gBerryBlender = eBerryBlenderData;
    gBerryBlender->mainState = 0;
    gBerryBlender->field_134 = 0;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        gBerryBlender->chosenItemId[i] = 0;
    }
    InitLocalPlayers(gSpecialVar_0x8004);
    if (gSpecialVar_0x8004 == 0)
        SetMainCallback2(CB2_StartBlenderLink);
    else
        SetMainCallback2(CB2_StartBlenderLocal);
}

static void CB2_StartBlenderLink(void)
{
    s32 i, j;
    switch (gBerryBlender->mainState)
    {
    case 0:
        ResetSpriteData();
        FreeAllSpritePalettes();
        ResetTasks();
        SetVBlankCallback(VBlankCB_BerryBlender);
        Text_LoadWindowTemplate(&gWindowTemplate_81E6F68);
        InitMenuWindow(&gWindowTemplate_81E6F68);
        gLinkType = 0x4422;
        gBerryBlender->mainState++;
        gBerryBlender->field_4E = 0;
        gBerryBlender->slowdownTimer = 0;
        gBerryBlender->bg_X = 0;
        gBerryBlender->bg_Y = 0;
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlender->playerContinueResponses[i] = 0;
            for (j = 0; j < 3; j++)
            {
                gBerryBlender->scores[i][j] = 0;
            }
        }
        gBerryBlender->playAgainState = 0;
        gBerryBlender->speed = 0;
        gBerryBlender->arrowPos = 0;
        gBerryBlender->maxRPM = 0;
        gBerryBlender->loadGfxState = 0;
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            gBerryBlender->mainState++;
            UpdateBlenderCenter();
        }
        break;
    case 2:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlender->playerArrowSpriteIds2[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
            StartSpriteAnim(&gSprites[gBerryBlender->playerArrowSpriteIds2[i]], i + 8);
        }
        gBerryBlender->mainState++;
        break;
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gBerryBlender->mainState++;
        break;
    case 4:
        DrawBlenderBg();
        if (!gPaletteFade.active)
        {
            gBerryBlender->mainState++;
        }
        break;
    case 5:
        Menu_DrawStdWindowFrame(0, 13, 29, 19);
        Menu_PrintText(gOtherText_LinkStandby3, 1, 14);
        gBerryBlender->mainState = 8;
        gBerryBlender->framesToWait = 0;
        break;
    case 8:
        gBerryBlender->mainState++;
        gBerryBlender->playerToThrowBerry = 0;
        ConvertItemToBlenderBerry(&gBerryBlender->blendedBerries[0], gSpecialVar_ItemId);
        memcpy(gBlockSendBuffer, &gBerryBlender->blendedBerries[0], sizeof(struct BlenderBerry));
        SetLinkStandbyCallback();
        gBerryBlender->framesToWait = 0;
        break;
    case 9:
        if (IsLinkTaskFinished())
        {
            ResetBlockReceivedFlags();
            if (GetMultiplayerId() == 0)
                SendBlockRequest(4);
            gBerryBlender->mainState++;
        }
        break;
    case 10:
        if (++gBerryBlender->framesToWait > 20)
        {
            Menu_EraseScreen();
            if (GetBlockReceivedStatus() == GetSavedLinkPlayerCountAsBitFlags())
            {
                for (i = 0; i < GetLinkPlayerCount(); i++)
                {
                    memcpy(&gBerryBlender->blendedBerries[i], &gBlockRecvBuffer[i][0], sizeof(struct BlenderBerry));
                    gBerryBlender->chosenItemId[i] = gBerryBlender->blendedBerries[i].itemId;
                }
                ResetBlockReceivedFlags();
                gBerryBlender->mainState++;
            }
        }
        break;
    case 11:
        gBerryBlender->numPlayers = GetLinkPlayerCount();
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (gBerryBlender->playerToThrowBerry == sPlayerIdMap[gBerryBlender->numPlayers - 2][i])
            {
                CreateBerrySprite(gBerryBlender->chosenItemId[gBerryBlender->playerToThrowBerry], i);
                break;
            }
        }
        gBerryBlender->framesToWait = 0;
        gBerryBlender->mainState++;
        gBerryBlender->playerToThrowBerry++;
        break;
    case 12:
        if (++gBerryBlender->framesToWait > 60)
        {
            if (gBerryBlender->playerToThrowBerry >= gBerryBlender->numPlayers)
            {
                gBerryBlender->mainState++;
                gBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlender->numPlayers - 2]] - 22528;
            }
            else
                gBerryBlender->mainState--;
            gBerryBlender->framesToWait = 0;
        }
        break;
    case 13:
        if (IsLinkTaskFinished())
        {
            gBerryBlender->mainState++;
            DrawBlenderCenter(&gBerryBlender->field_168);
        }
        break;
    case 14:
        REG_DISPCNT |= 0x400;
        gBerryBlender->arrowPos += 0x200;
        gBerryBlender->centerScale += 4;
        if (gBerryBlender->centerScale > 255)
        {
            gBerryBlender->mainState++;
            gBerryBlender->centerScale = 256;
            gBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlender->numPlayers - 2]];
            REG_BG2CNT = 0x4882;
            gBerryBlender->framesToWait = 0;
            SetPlayerIdMaps();
            PrintPlayerNames();
        }
        DrawBlenderCenter(&gBerryBlender->field_168);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            gBerryBlender->framesToWait = 0;
            gBerryBlender->mainState++;
        }
        DrawBlenderCenter(&gBerryBlender->field_168);
        break;
    case 16:
        CreateSprite(&sSpriteTemplate_CountdownNumbers, 120, -16, 3);
        gBerryBlender->mainState++;
        break;
    case 18:
        gBerryBlender->mainState++;
        break;
    case 19:
        SetLinkStandbyCallback();
        gBerryBlender->mainState++;
        break;
    case 20:
        if (IsLinkTaskFinished())
        {
            SetBerryBlenderLinkCallback();
            gBerryBlender->mainState++;
        }
        break;
    case 21:
        gBerryBlender->speed = 128;
        gBerryBlender->gameFrameTime = 0;
        SetMainCallback2(CB2_PlayBlender);
        if (GetCurrentMapMusic() != 403)
        {
            gBerryBlender->savedMusic = GetCurrentMapMusic();
        }
        PlayBGM(MUS_CYCLING);
        break;
    case 100:
        Menu_DrawStdWindowFrame(0, 13, 29, 19);
        MenuPrintMessage(gOtherText_LinkNotFound, 1, 15);
        gBerryBlender->mainState++;
        break;
    case 101:
        if (Menu_UpdateWindowText())
            gBerryBlender->mainState++;
        break;
    case 102:
        if (!gPaletteFade.active)
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void InitBlenderBgs(void)
{
    REG_DISPCNT = 0;

    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();

    SetVBlankCallback(VBlankCB_BerryBlender);

    Text_LoadWindowTemplate(&gWindowTemplate_81E6F68);
    InitMenuWindow(&gWindowTemplate_81E6F68);

    gLinkType = 0x4422;

    gBerryBlender->field_4E = 0;
    gBerryBlender->speed = 0;
    gBerryBlender->arrowPos = 0;
    gBerryBlender->maxRPM = 0;
    gBerryBlender->bg_X = 0;
    gBerryBlender->bg_Y = 0;
    gBerryBlender->mainState++;
}

static u8 GetArrowProximity(u16 arrowPos, u8 playerId)
{
    u32 pos = (arrowPos / 256) + 24;
    u8 arrowId = gBerryBlender->playerIdToArrowId[playerId];
    u32 hitRangeStart = sArrowHitRangeStart[arrowId];

    if (pos >= hitRangeStart && pos < hitRangeStart + 48)
    {
        if (pos >= hitRangeStart + 20 && pos < hitRangeStart + 28)
            return 2;
        else
            return 1;
    }
    else
        return 0;
}

static void SetOpponentsBerryData(u16 playerBerryItemId, u8 playersNum, struct BlenderBerry* playerBerry)
{
    u16 opponentSetId = 0;
    u16 i;
    if (playerBerryItemId == ITEM_ENIGMA_BERRY)
    {
        for (i = 0; i < 5; i++)
        {
            if (playerBerry->flavors[opponentSetId] > playerBerry->flavors[i])
                opponentSetId = i;
        }
        opponentSetId += 5;
    }
    else
    {
        opponentSetId = playerBerryItemId - 133;
        if (opponentSetId >= 5)
            opponentSetId = (opponentSetId % 5) + 5;
    }
    for (i = 0; i < playersNum - 1; i++)
    {
        SetPlayerBerryData(i + 1, sOpponentBerrySets[opponentSetId][i] + 133);
    }
}

static void SetPlayerIdMaps(void)
{
    s32 i, j;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        gBerryBlender->playerIdToArrowId[i] = 0xFF;
        gBerryBlender->arrowIdToPlayerId[i] = sPlayerIdMap[gBerryBlender->numPlayers - 2][i];
    }
    for (j = 0; j < BLENDER_MAX_PLAYERS; j++)
    {
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (gBerryBlender->arrowIdToPlayerId[i] == j)
                gBerryBlender->playerIdToArrowId[j] = i;
        }
    }
}

static void PrintPlayerNames(void)
{
    s32 i;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        if (gBerryBlender->arrowIdToPlayerId[i] != 0xFF)
        {
            u8* stringPtr = gStringVar1;

            gBerryBlender->playerArrowSpriteIds[gBerryBlender->arrowIdToPlayerId[i]] = gBerryBlender->playerArrowSpriteIds2[i];
            StartSpriteAnim(&gSprites[gBerryBlender->playerArrowSpriteIds[gBerryBlender->arrowIdToPlayerId[i]]], i);
            if (GetMultiplayerId() == gBerryBlender->arrowIdToPlayerId[i])
                stringPtr = StringCopy(stringPtr, sRedColorString);
            StringCopy(stringPtr, gLinkPlayers[gBerryBlender->arrowIdToPlayerId[i]].name);
            Menu_PrintTextPixelCoords(gStringVar1, gUnknown_082162D4[i][0] * 8 + 1, gUnknown_082162D4[i][1] * 8, 1);
        }
    }
}

static void CB2_StartBlenderLocal(void)
{
    s32 i, j;
    switch (gBerryBlender->mainState)
    {
    case 0:
        InitBlenderBgs();
        SetPlayerBerryData(0, gSpecialVar_ItemId);
        ConvertItemToBlenderBerry(&gBerryBlender->blendedBerries[0], gSpecialVar_ItemId);
        SetOpponentsBerryData(gSpecialVar_ItemId, gBerryBlender->numPlayers, &gBerryBlender->blendedBerries[0]);
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlender->playerContinueResponses[i] = 0;
            for (j = 0; j < 3; j++)
            {
                gBerryBlender->scores[i][j] = 0;
            }
        }
        gBerryBlender->playAgainState = 0;
        gBerryBlender->loadGfxState = 0;
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            gBerryBlender->mainState++;
            UpdateBlenderCenter();
        }
        break;
    case 2:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlender->playerArrowSpriteIds2[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
            StartSpriteAnim(&gSprites[gBerryBlender->playerArrowSpriteIds2[i]], i + 8);
        }
        gBerryBlender->mainState++;
        break;
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gBerryBlender->mainState++;
        gBerryBlender->framesToWait = 0;
        break;
    case 4:
        if (++gBerryBlender->framesToWait == 2)
            DrawBlenderBg();
        if (!gPaletteFade.active)
            gBerryBlender->mainState = 8;
        break;
    case 8:
        gBerryBlender->mainState = 11;
        gBerryBlender->playerToThrowBerry = 0;
        break;
    case 11:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            u32 playerId = sPlayerIdMap[gBerryBlender->numPlayers - 2][i];
            if (gBerryBlender->playerToThrowBerry == playerId)
            {
                CreateBerrySprite(gBerryBlender->chosenItemId[gBerryBlender->playerToThrowBerry], i);
                break;
            }
        }
        gBerryBlender->framesToWait = 0;
        gBerryBlender->mainState++;
        gBerryBlender->playerToThrowBerry++;
        break;
    case 12:
        if (++gBerryBlender->framesToWait > 60)
        {
            if (gBerryBlender->playerToThrowBerry >= gBerryBlender->numPlayers)
            {
                gBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlender->numPlayers - 2]] - 22528;
                gBerryBlender->mainState++;
            }
            else
                gBerryBlender->mainState--;
            gBerryBlender->framesToWait = 0;
        }
        break;
    case 13:
        gBerryBlender->mainState++;
        SetPlayerIdMaps();
        PlaySE(SE_FALL);
        DrawBlenderCenter(&gBerryBlender->field_168);
        break;
    case 14:
        REG_DISPCNT |= 0x400;
        gBerryBlender->arrowPos += 0x200;
        gBerryBlender->centerScale += 4;
        if (gBerryBlender->centerScale > 255)
        {
            gBerryBlender->mainState++;
            gBerryBlender->centerScale = 256;
            gBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlender->numPlayers - 2]];
            REG_BG2CNT = 0x4882;
            gBerryBlender->framesToWait = 0;
            PlaySE(SE_TRUCK_DOOR);;
            PrintPlayerNames();
        }
        DrawBlenderCenter(&gBerryBlender->field_168);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            gBerryBlender->mainState++;
        }
        DrawBlenderCenter(&gBerryBlender->field_168);
        break;
    case 16:
        CreateSprite(&sSpriteTemplate_CountdownNumbers, 120, -16, 3);
        gBerryBlender->mainState++;
        break;
    case 18:
        gBerryBlender->mainState++;
        break;
    case 19:
        gBerryBlender->mainState++;
        break;
    case 20:
        gBerryBlender->mainState++;
        break;
    case 21:
        ResetLinkCmds();
        gBerryBlender->speed = 128;
        gBerryBlender->gameFrameTime = 0;
        gBerryBlender->perfectOpponents = 0;
        gBerryBlender->slowdownTimer = 0;
        SetMainCallback2(CB2_PlayBlender);

        for (i = 0; i < gSpecialVar_0x8004; i++)
        {
            gBerryBlender->opponentTaskIds[i] = CreateTask(sLocalOpponentTasks[i], 10 + i);
        }

        if (GetCurrentMapMusic() != 403)
        {
            gBerryBlender->savedMusic = GetCurrentMapMusic();
        }
        PlayBGM(MUS_CYCLING);
        PlaySE(SE_BERRY_BLENDER);
        UpdateHitPitch();
        break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void ResetLinkCmds(void)
{
    s32 i;
    for (i = 0; i < 4; i++)
    {
        gSendCmd[0] = 0;
        gSendCmd[2] = 0;
        gRecvCmds[0][i] = 0;
        gRecvCmds[2][i] = 0;
    }
}

static void Task_OpponentMiss(u8 taskId)
{
   if(++gTasks[taskId].data[0] > gTasks[taskId].data[1])
   {
        gRecvCmds[2][gTasks[taskId].data[2]] = 0x2345;
        DestroyTask(taskId);
   }
}

static void CreateOpponentMissTask(u8 playerId, u8 delay)
{
    u8 taskId = CreateTask(Task_OpponentMiss, 80);
    gTasks[taskId].data[1] = delay;
    gTasks[taskId].data[2] = playerId;
}

static void Task_HandleOpponent1(u8 taskId)
{
    if (GetArrowProximity(gBerryBlender->arrowPos, 1) == 2)
    {
        if (gTasks[taskId].data[0] == 0)
        {
            if (gBerryBlender->perfectOpponents == 0)
            {
                u8 rand = Random() / 655;
                if (gBerryBlender->speed < 500)
                {
                    if (rand > 75)
                        gRecvCmds[2][1] = 0x4523;
                    else
                        gRecvCmds[2][1] = 0x5432;
                    gRecvCmds[2][1] = 0x5432; // ???
                }
                else if (gBerryBlender->speed < 1500)
                {
                    if (rand > 80)
                        gRecvCmds[2][1] = 0x4523;
                    else
                    {
                        u8 value = rand - 21;
                        if (value < 60)
                            gRecvCmds[2][1] = 0x5432;
                        else if (rand < 10)
                            CreateOpponentMissTask(1, 5);
                    }
                }
                else if (rand <= 90)
                {
                    u8 value = rand - 71;
                    if (value < 20)
                        gRecvCmds[2][1] = 0x5432;
                    else if (rand < 30)
                        CreateOpponentMissTask(1, 5);
                }
                else
                   gRecvCmds[2][1] = 0x4523;
            }
            else
                gRecvCmds[2][1] = 0x4523;

            gTasks[taskId].data[0] = 1;
        }
    }
    else
        gTasks[taskId].data[0] = 0;
}

static void Task_HandleOpponent2(u8 taskId)
{
    u32 var1 = (gBerryBlender->arrowPos + 0x1800) & 0xFFFF;
    u32 arrowId = gBerryBlender->playerIdToArrowId[2] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[arrowId] + 20 && (var1 >> 8) < sArrowHitRangeStart[arrowId] + 40)
    {
        if (gTasks[taskId].data[0] == 0)
        {
            if (gBerryBlender->perfectOpponents == 0)
            {
                u8 rand = Random() / 655;
                if (gBerryBlender->speed < 500)
                {
                    if (rand > 66)
                        gRecvCmds[2][2] = 0x4523;
                    else
                        gRecvCmds[2][2] = 0x5432;
                }
                else
                {
                    u8 value;
                    if (rand > 65)
                        gRecvCmds[2][2] = 0x4523;
                    value = rand - 41;
                    if (value < 25)
                        gRecvCmds[2][2] = 0x5432;
                    if (rand < 10)
                        CreateOpponentMissTask(2, 5);
                }

                gTasks[taskId].data[0] = 1;
            }
            else
            {
                gRecvCmds[2][2] = 0x4523;
                gTasks[taskId].data[0] = 1;
            }
        }
    }
    else
        gTasks[taskId].data[0] = 0;
}

static void Task_HandleOpponent3(u8 taskId)
{
    u32 var1, arrowId;

    var1 = (gBerryBlender->arrowPos + 0x1800) & 0xFFFF;
    arrowId = gBerryBlender->playerIdToArrowId[3] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[arrowId] + 20 && (var1 >> 8) < sArrowHitRangeStart[arrowId] + 40)
    {
        if (gTasks[taskId].data[0] == 0)
        {
            if (gBerryBlender->perfectOpponents == 0)
            {
                u8 rand = (Random() / 655);
                if (gBerryBlender->speed < 500)
                {
                    if (rand > 88)
                        gRecvCmds[2][3] = 0x4523;
                    else
                        gRecvCmds[2][3] = 0x5432;
                }
                else
                {
                    if (rand > 60)
                        gRecvCmds[2][3] = 0x4523;
                    else
                    {
                        s8 value = rand - 56; // makes me wonder what the original code was
                        u8 value2 = value;
                        if (value2 < 5)
                            gRecvCmds[2][3] = 0x5432;
                    }
                    if (rand < 5)
                        CreateOpponentMissTask(3, 5);
                }
                gTasks[taskId].data[0] = 1;
            }
            else
            {
                gRecvCmds[2][3] = 0x4523;
                gTasks[taskId].data[0] = 1;
            }
        }
    }
    else
        gTasks[taskId].data[0] = 0;
}

static void CreateScoreSymbolSprite(u16 cmd, u8 arrowId)
{
    u8 spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_ScoreSymbols,
                            sPlayerArrowPos[arrowId][0] - (10 * sPlayerArrowQuadrant[arrowId][0]),
                            sPlayerArrowPos[arrowId][1] - (10 * sPlayerArrowQuadrant[arrowId][1]),
                            1);
    if (cmd == 0x4523)
    {
        StartSpriteAnim(&gSprites[spriteId], 2);
        gSprites[spriteId].callback = SpriteCB_ScoreSymbolBest;
        PlaySE(SE_ICE_STAIRS);
    }
    else if (cmd == 0x5432)
    {
        StartSpriteAnim(&gSprites[spriteId], 0);
        PlaySE(SE_SUCCESS);
    }
    else if (cmd == 0x2345)
    {
        StartSpriteAnim(&gSprites[spriteId], 1);
        PlaySE(SE_FAILURE);
    }
    CreateParticleSprites();
}

static void UpdateSpeedFromHit(u16 cmd)
{
    UpdateHitPitch();
    switch (cmd)
    {
    case 0x4523:
        if (gBerryBlender->speed < 1500)
            gBerryBlender->speed += (384 / sNumPlayersToSpeedDivisor[gBerryBlender->numPlayers]);
        else
        {
            gBerryBlender->speed += (128 / sNumPlayersToSpeedDivisor[gBerryBlender->numPlayers]);
            ShakeBgCoordForHit(&gBerryBlender->bg_X, (gBerryBlender->speed / 100) - 10);
            ShakeBgCoordForHit(&gBerryBlender->bg_Y, (gBerryBlender->speed / 100) - 10);
        }
        break;
    case 0x5432:
        if (gBerryBlender->speed < 1500)
            gBerryBlender->speed += (256 / sNumPlayersToSpeedDivisor[gBerryBlender->numPlayers]);
        break;
    case 0x2345:
        gBerryBlender->speed -= (256 / sNumPlayersToSpeedDivisor[gBerryBlender->numPlayers]);
        if (gBerryBlender->speed < 128)
            gBerryBlender->speed = 128;
        break;
    }
}

static void UpdateOpponentScores(void)
{
    s32 i;

    if (gSpecialVar_0x8004 != 0)
    {
        if (gSendCmd[2] != 0)
        {
            gRecvCmds[2][0] = gSendCmd[2];
            gRecvCmds[0][0] = 0x4444;
            gSendCmd[2] = 0;
        }
        for (i = 1; i < 4; i++)
        {
            if (gRecvCmds[2][i] != 0)
                gRecvCmds[0][i] = 0x4444;
        }
    }
    for (i = 0; i < gBerryBlender->numPlayers; i++)
    {
        if (gRecvCmds[0][i] == 0x4444)
        {
            u32 arrowId = gBerryBlender->playerIdToArrowId[i];
            if (gRecvCmds[2][i] == 0x4523)
            {
                UpdateSpeedFromHit(0x4523);
                gBerryBlender->progressBarValue += (gBerryBlender->speed / 55);
                if (gBerryBlender->progressBarValue >= 1000)
                    gBerryBlender->progressBarValue = 1000;
                CreateScoreSymbolSprite(0x4523, arrowId);
                gBerryBlender->scores[i][SCORE_BEST]++;
            }
            else if (gRecvCmds[2][i] == 0x5432)
            {
                UpdateSpeedFromHit(0x5432);
                gBerryBlender->progressBarValue += (gBerryBlender->speed / 70);
                CreateScoreSymbolSprite(0x5432, arrowId);
                gBerryBlender->scores[i][SCORE_GOOD]++;
            }
            else if (gRecvCmds[2][i] == 0x2345)
            {
                CreateScoreSymbolSprite(0x2345, arrowId);
                UpdateSpeedFromHit(0x2345);
                // looks like a bug
                if (gBerryBlender->field_4.win_field_F > 1000)
                    gBerryBlender->progressBarValue = 1000;
                if (gBerryBlender->scores[i][SCORE_MISS] < 999)
                    gBerryBlender->scores[i][SCORE_MISS]++;
            }
            if (gRecvCmds[2][i] == 0x2345 || gRecvCmds[2][i] == 0x4523 || gRecvCmds[2][i] == 0x5432)
            {
                if (gBerryBlender->speed > 1500)
                    m4aMPlayTempoControl(&gMPlayInfo_BGM, ((gBerryBlender->speed - 750) / 20) + 256);
                else
                    m4aMPlayTempoControl(&gMPlayInfo_BGM, 256);
            }
        }
    }
    if (gSpecialVar_0x8004 != 0)
    {
        for (i = 0; i < gBerryBlender->numPlayers; i++)
        {
            gRecvCmds[0][i] = 0;
            gRecvCmds[2][i] = 0;
        }
    }
}

static void HandlePlayerInput(void)
{
    bool8 pressedA = 0;
    u8 arrowId = gBerryBlender->playerIdToArrowId[GetMultiplayerId()];
    if (gBerryBlender->gameEndState == 0)
    {
        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A && JOY_NEW(A_BUTTON))
            pressedA = ((gMain.heldKeysRaw & (A_BUTTON | L_BUTTON)) != (A_BUTTON | L_BUTTON));
        else if (JOY_NEW(A_BUTTON))
            pressedA = 1;
        if (pressedA)
        {
            u8 proximity;
            StartSpriteAnim(&gSprites[gBerryBlender->playerArrowSpriteIds[gBerryBlender->arrowIdToPlayerId[arrowId]]], arrowId + 4);
            proximity = GetArrowProximity(gBerryBlender->arrowPos, GetMultiplayerId());
            if (proximity == 2)
                gSendCmd[2] = 0x4523;
            else if (proximity == 1)
                gSendCmd[2] = 0x5432;
            else
                gSendCmd[2] = 0x2345;
        }
    }
    if (++gBerryBlender->slowdownTimer > 5)
    {
        if (gBerryBlender->speed > 128)
            gBerryBlender->speed--;
        gBerryBlender->slowdownTimer = 0;
    }
    if (gUnknown_020297ED && JOY_NEW(L_BUTTON))
        gBerryBlender->perfectOpponents ^= 1;
}

static void CB2_PlayBlender(void)
{
    UpdateBlenderCenter();
    if (gBerryBlender->gameFrameTime < (99 * 60 * 60) + (59 * 60)) // game time can't be longer than 99 minutes and 59 seconds, can't print 3 digits
        gBerryBlender->gameFrameTime++;
    HandlePlayerInput();
    SetLinkDebugValues((u16)(gBerryBlender->speed), gBerryBlender->progressBarValue);
    UpdateOpponentScores();
    TryUpdateProgressBar(gBerryBlender->progressBarValue, 1000);
    UpdateRPM(gBerryBlender->speed);
    RestoreBgCoords();
    ProcessLinkPlayerCmds();
    if (gBerryBlender->gameEndState == 0 && gBerryBlender->maxProgressBarValue >= 1000)
    {
        gBerryBlender->progressBarValue = 1000;
        gBerryBlender->gameEndState = 1;
        SetMainCallback2(CB2_EndBlenderGame);
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static bool8 AreBlenderBerriesSame(struct BlenderBerry* berries, u8 index1, u8 index2)
{
    if (berries[index1].itemId != berries[index2].itemId
     || (StringCompare(berries[index1].name, berries[index2].name) == 0
      && (berries[index1].flavors[0] == berries[index2].flavors[0]
       && berries[index1].flavors[1] == berries[index2].flavors[1]
       && berries[index1].flavors[2] == berries[index2].flavors[2]
       && berries[index1].flavors[3] == berries[index2].flavors[3]
       && berries[index1].flavors[4] == berries[index2].flavors[4]
       && berries[index1].smoothness == berries[index2].smoothness)))
        return TRUE;
    else
        return FALSE;
}

u32 CalculatePokeblockColor(struct BlenderBerry* berries, s16* _flavors, u8 numPlayers, u8 negativeFlavors)
{
    s16 flavors[5];
    s32 i;
    s32 j;
    u8 numFlavors;

    // bug, writing one index too far
#ifdef UBFIX
    for (i = 0; i < 5; i++)
#else
    for (i = 0; i <= 5; i++)
#endif //UBFIX
        flavors[i] = _flavors[i];
    j = 0;
    for (i = 0; i < 5; i++)
    {
        if (flavors[i] == 0)
            j++;
    }
    if (j == 5 || negativeFlavors > 3)
        return PBLOCK_CLR_BLACK;
    for (i = 0; i < numPlayers; i++)
    {
        for (j = 0; j < numPlayers; j++)
        {
            if (berries[i].itemId == berries[j].itemId && i != j
                && (berries[i].itemId != ITEM_ENIGMA_BERRY || AreBlenderBerriesSame(berries, i, j)))
                    return PBLOCK_CLR_BLACK;
        }
    }
    numFlavors = 0;
    for (numFlavors = 0, i = 0; i < 5; i++)
    {
        if (flavors[i] > 0)
            numFlavors++;
    }
    if (numFlavors > 3)
        return PBLOCK_CLR_WHITE;
    if (numFlavors == 3)
        return PBLOCK_CLR_GRAY;
    for (i = 0; i < 5; i++)
    {
        if (flavors[i] > 50)
            return PBLOCK_CLR_GOLD;
    }
    if (numFlavors == 1 && flavors[0] > 0)
        return PBLOCK_CLR_RED;
    if (numFlavors == 1 && flavors[1] > 0)
        return PBLOCK_CLR_BLUE;
    if (numFlavors == 1 && flavors[2] > 0)
        return PBLOCK_CLR_PINK;
    if (numFlavors == 1 && flavors[3] > 0)
        return PBLOCK_CLR_GREEN;
    if (numFlavors == 1 && flavors[4] > 0)
        return PBLOCK_CLR_YELLOW;
    if (numFlavors == 2)
    {
        s32 idx = 0;
        for (i = 0; i < 5; i++)
        {
            if (flavors[i] > 0)
                sPokeblockPresentFlavors[idx++] = i;
        }
        if (flavors[sPokeblockPresentFlavors[0]] >= flavors[sPokeblockPresentFlavors[1]])
        {
            if (sPokeblockPresentFlavors[0] == 0)
                return (sPokeblockPresentFlavors[1] << 16) | PBLOCK_CLR_PURPLE;
            if (sPokeblockPresentFlavors[0] == 1)
                return (sPokeblockPresentFlavors[1] << 16) | PBLOCK_CLR_INDIGO;
            if (sPokeblockPresentFlavors[0] == 2)
                return (sPokeblockPresentFlavors[1] << 16) | PBLOCK_CLR_BROWN;
            if (sPokeblockPresentFlavors[0] == 3)
                return (sPokeblockPresentFlavors[1] << 16) | PBLOCK_CLR_LITE_BLUE;
            if (sPokeblockPresentFlavors[0] == 4)
                return (sPokeblockPresentFlavors[1] << 16) | PBLOCK_CLR_OLIVE;
        }
        else
        {
            if (sPokeblockPresentFlavors[1] == 0)
                return (sPokeblockPresentFlavors[0] << 16) | PBLOCK_CLR_PURPLE;
            if (sPokeblockPresentFlavors[1] == 1)
                return (sPokeblockPresentFlavors[0] << 16) | PBLOCK_CLR_INDIGO;
            if (sPokeblockPresentFlavors[1] == 2)
                return (sPokeblockPresentFlavors[0] << 16) | PBLOCK_CLR_BROWN;
            if (sPokeblockPresentFlavors[1] == 3)
                return (sPokeblockPresentFlavors[0] << 16) | PBLOCK_CLR_LITE_BLUE;
            if (sPokeblockPresentFlavors[1] == 4)
                return (sPokeblockPresentFlavors[0] << 16) | PBLOCK_CLR_OLIVE;
        }
    }
    return 0;
}

static void Debug_SetMaxRPMStage(s16 value)
{
    sDebug_MaxRPMStage = value;
}

static s16 UNUSED Debug_GetMaxRPMStage(void)
{
    return sDebug_MaxRPMStage;
}

static void Debug_SetGameTimeStage(s16 value)
{
    sDebug_GameTimeStage = value;
}

static s16 UNUSED Debug_GetGameTimeStage(void)
{
    return sDebug_GameTimeStage;
}

static void CalculatePokeblock(struct BlenderBerry* berries, struct Pokeblock* pokeblock, u8 numPlayers, u8* flavors, u16 maxRPM)
{
    s32 i;
    s32 j;
    s32 flavor;
    s32 remainder;
    s32 multiuseVar;
    u8 numNegatives;

    for (i = 0; i < 6; i++)
        sPokeblockFlavors[i] = 0;
    for (i = 0; i < numPlayers; i++)
    {
        for (j = 0; j < 6; j++)
            sPokeblockFlavors[j] += berries[i].flavors[j];
    }

    multiuseVar = sPokeblockFlavors[0];
    sPokeblockFlavors[0] -= sPokeblockFlavors[1];
    sPokeblockFlavors[1] -= sPokeblockFlavors[2];
    sPokeblockFlavors[2] -= sPokeblockFlavors[3];
    sPokeblockFlavors[3] -= sPokeblockFlavors[4];
    sPokeblockFlavors[4] -= multiuseVar;

    multiuseVar = 0;
    for (i = 0; i < 5; i++)
    {
        if (sPokeblockFlavors[i] < 0)
        {
            sPokeblockFlavors[i] = 0;
            multiuseVar++;
        }
    }
    numNegatives = multiuseVar;
    for (i = 0; i < 5; i++)
    {
        if (sPokeblockFlavors[i] > 0)
        {
            if (sPokeblockFlavors[i] < multiuseVar)
                sPokeblockFlavors[i] = 0;
            else
                sPokeblockFlavors[i] -= multiuseVar;
        }
    }
    for (i = 0; i < 5; i++)
    {
        sDebug_PokeblockFactorFlavors[i] = sPokeblockFlavors[i];
    }

    multiuseVar = maxRPM / 333 + 100;
    sDebug_PokeblockFactorRPM = multiuseVar;

    for (i = 0; i < 5; i++)
    {
        flavor = sPokeblockFlavors[i];
        flavor = (flavor * multiuseVar) / 10;
        remainder = flavor % 10;
        flavor /= 10;
        if (remainder > 4)
            flavor++;
        sPokeblockFlavors[i] = flavor;
    }
    for (i = 0; i < 5; i++)
    {
        sDebug_PokeblockFactorFlavorsAfterRPM[i] = sPokeblockFlavors[i];
    }
    pokeblock->color = CalculatePokeblockColor(berries, &sPokeblockFlavors[0], numPlayers, numNegatives);
    sPokeblockFlavors[5] = (sPokeblockFlavors[5] / numPlayers) - numPlayers;
    if (sPokeblockFlavors[5] < 0)
        sPokeblockFlavors[5] = 0;
    if (pokeblock->color == 12)
    {
        multiuseVar = Random() % 10;
        for (i = 0; i < 5; i++)
        {
            if ((sBlackPokeblockFlavorFlags[multiuseVar] >> i) & 1)
                sPokeblockFlavors[i] = 2;
            else
                sPokeblockFlavors[i] = 0;
        }
    }
    for (i = 0; i < 6; i++)
    {
        if (sPokeblockFlavors[i] > 255)
            sPokeblockFlavors[i] = 255;
    }
    pokeblock->spicy = sPokeblockFlavors[0];
    pokeblock->dry = sPokeblockFlavors[1];
    pokeblock->sweet = sPokeblockFlavors[2];
    pokeblock->bitter = sPokeblockFlavors[3];
    pokeblock->sour = sPokeblockFlavors[4];
    pokeblock->feel = sPokeblockFlavors[5];
    for (i = 0; i < 6; i++)
    {
        flavors[i] = sPokeblockFlavors[i];
    }
}

static void Debug_CalculatePokeblock(struct BlenderBerry* berries, struct Pokeblock* pokeblock, u8 numPlayers, u8* flavors, u16 maxRPM)
{
    CalculatePokeblock(berries, pokeblock, numPlayers, flavors, maxRPM);
}

static void Debug_SetStageVars(void)
{
    u32 frames = (u16)(gBerryBlender->gameFrameTime);
    u16 maxRPM = gBerryBlender->maxRPM;
    s16 stage = 0;

    if (frames < 900)
        stage = 5;
    else if ((u16)(frames - 900) < 600)
        stage = 4;
    else if ((u16)(frames - 1500) < 600)
        stage = 3;
    else if ((u16)(frames - 2100) < 900)
        stage = 2;
    else if ((u16)(frames - 3300) < 300)
        stage = 1;
    Debug_SetGameTimeStage(stage);

    stage = 0;
    if (maxRPM <= 64)
    {
        if (maxRPM >= 50 && maxRPM < 100)
            stage = -1;
        else if (maxRPM >= 100 && maxRPM < 150)
            stage = -2;
        else if (maxRPM >= 150 && maxRPM < 200)
            stage = -3;
        else if (maxRPM >= 200 && maxRPM < 250)
            stage = -4;
        else if (maxRPM >= 250 && maxRPM < 300)
            stage = -5;
        else if (maxRPM >= 350 && maxRPM < 400)
            stage = -6;
        else if (maxRPM >= 400 && maxRPM < 450)
            stage = -7;
        else if (maxRPM >= 500 && maxRPM < 550)
            stage = -8;
        else if (maxRPM >= 550 && maxRPM < 600)
            stage = -9;
        else if (maxRPM >= 600)
            stage = -10;
    }
    Debug_SetMaxRPMStage(stage);
}

static void sub_80508D4(u8 value)
{
    gBerryBlender->yesNoAnswer = value;
    MenuCursor_SetPos814A880(192, (gBerryBlender->yesNoAnswer * 16) + 72);
}

static void sub_80508FC(void)
{
    gBerryBlender->yesNoAnswer = 0;
    Menu_DrawStdWindowFrame(23, 8, 28, 13);
    MenuCursor_Create814A5C0(0, -1, 12, 0x2D9F, 32);
    Menu_PrintText(gOtherText_YesNoTerminating, 24, 9);
    sub_80508D4(gBerryBlender->yesNoAnswer);
}

static void CB2_EndBlenderGame(void)
{
    u8 i;
    u8 multiplayerID; // unused

    UpdateBlenderCenter();
    multiplayerID = GetMultiplayerId();
    switch (gBerryBlender->gameEndState)
    {
    case 1:
        ClearLinkCallback();
        m4aMPlayTempoControl(&gMPlayInfo_BGM, 256);
        for (i = 0; i < gSpecialVar_0x8004; i++)
        {
            DestroyTask(gBerryBlender->opponentTaskIds[i]);
        }
        gBerryBlender->gameEndState++;
        break;
    case 2:
        gBerryBlender->speed -= 32;
        if (gBerryBlender->speed <= 0)
        {
            gBerryBlender->speed = 0;
            if (gReceivedRemoteLinkPlayers != 0)
                gBerryBlender->gameEndState++;
            else
                gBerryBlender->gameEndState = 5;
            gBerryBlender->mainState = 0;
            m4aMPlayStop(&gMPlayInfo_SE2);
        }
        UpdateHitPitch();
        break;
    case 3:
        if (/*multiplayerID != 0*/ GetMultiplayerId() != 0)
            gBerryBlender->gameEndState++;
        else if (IsLinkTaskFinished())
        {
            gBerryBlender->field_1BC = gBerryBlender->gameFrameTime;
            gBerryBlender->field_1C0 = gBerryBlender->maxRPM;
            SendBlock(0, &gBerryBlender->field_1BC, 40);
            gBerryBlender->gameEndState++;
        }
        break;
    case 4:
        if (GetBlockReceivedStatus())
        {
            u32* ptr = ((u32*)(&gBlockRecvBuffer[0][0]));
            gBerryBlender->maxRPM = gBlockRecvBuffer[0][2];
            gBerryBlender->gameFrameTime = *ptr;
            gBerryBlender->gameEndState++;
            ResetBlockReceivedFlags();
        }
        break;
    case 5:
        if (PrintBlendingRanking())
            gBerryBlender->gameEndState++;
        break;
    case 6:
        if (PrintBlendingResults())
        {
            if (gInGameOpponentsNo == 0)
                IncrementGameStat(GAME_STAT_POKEBLOCKS_WITH_FRIENDS);
            else
                IncrementGameStat(GAME_STAT_POKEBLOCKS);
            gBerryBlender->gameEndState++;
        }
        break;
    case 7:
        gBerryBlender->gameEndState++;
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        MenuPrintMessage(sText_WouldLikeToBlendAnotherBerry, 1, 15);
        break;
    case 8:
        if (Menu_UpdateWindowText())
            gBerryBlender->gameEndState++;
        break;
    case 9:
        gBerryBlender->yesNoAnswer = 0;
        sub_80508FC();
        gBerryBlender->gameEndState++;
        break;
    case 10:
        if (JOY_NEW(DPAD_UP))
        {
            if (gBerryBlender->yesNoAnswer != 0)
                PlaySE(SE_SELECT);
            sub_80508D4(0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            if (gBerryBlender->yesNoAnswer != 1)
                PlaySE(SE_SELECT);
            sub_80508D4(1);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBerryBlender->gameEndState++;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBerryBlender->gameEndState++;
            sub_80508D4(1);
        }
        break;
    case 11:
        gSendCmd[0] = 0x2FFF;
        if (gBerryBlender->yesNoAnswer == 0)
        {
            if (IsBagPocketNonEmpty(POCKET_BERRIES) == FALSE) // is empty
            {
                gBerryBlender->playAgainState = 2;
                gSendCmd[1] = 0x9999;
            }
            else if (GetFirstFreePokeblockSlot() == -1)
            {
                gBerryBlender->playAgainState = 3;
                gSendCmd[1] = 0xAAAA;
            }
            else
            {
                gBerryBlender->playAgainState = 0;
                gSendCmd[1] = 0x7779;
            }
            gBerryBlender->gameEndState++;
        }
        else
        {
            gBerryBlender->playAgainState = 1;
            gSendCmd[1] = 0x8888;
            gBerryBlender->gameEndState++;
        }
        break;
    case 12:
        if (gInGameOpponentsNo)
        {
            SetMainCallback2(CB2_CheckPlayAgainLocal);
            gBerryBlender->gameEndState = 0;
            gBerryBlender->mainState = 0;
        }
        else
        {
            MenuPrintMessage(gOtherText_LinkStandby3, 1, 15);
            gBerryBlender->gameEndState++;
        }
        break;
    case 13:
        if (Menu_UpdateWindowText())
        {
            SetMainCallback2(CB2_CheckPlayAgainLink);
            gBerryBlender->gameEndState = 0;
            gBerryBlender->mainState = 0;
        }
        break;
    }
    RestoreBgCoords();
    UpdateRPM(gBerryBlender->speed);
    ProcessLinkPlayerCmds();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

bool8 LinkPlayAgainHandleSaving(void)
{
    switch (gBerryBlender->linkPlayAgainState)
    {
    case 0:
        SetLinkStandbyCallback();
        gBerryBlender->linkPlayAgainState = 1;
        gBerryBlender->framesToWait = 0;
        break;
    case 1:
        if (IsLinkTaskFinished())
        {
            gBerryBlender->linkPlayAgainState++;
            gSoftResetDisabled = TRUE;
        }
        break;
    case 2:
        WriteSaveBlock2();
        gBerryBlender->linkPlayAgainState++;
        gBerryBlender->framesToWait = 0;
        break;
    case 3:
        if (++gBerryBlender->framesToWait == 10)
        {
            SetLinkStandbyCallback();
            gBerryBlender->linkPlayAgainState++;
        }
        break;
    case 4:
        if (IsLinkTaskFinished())
        {
            if (WriteSaveBlock1Sector())
                gBerryBlender->linkPlayAgainState = 5;
            else
            {
                gBerryBlender->framesToWait = 0;
                gBerryBlender->linkPlayAgainState = 3;
            }
        }
        break;
    case 5:
        gBerryBlender->linkPlayAgainState++;
        gBerryBlender->framesToWait = 0;
        break;
    case 6:
        if (++gBerryBlender->framesToWait > 5)
        {
            gSoftResetDisabled = FALSE;
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void CB2_CheckPlayAgainLink(void)
{
    switch (gBerryBlender->gameEndState)
    {
    case 0:
        if (gBerryBlender->playerContinueResponses[0] == 0x2222)
            gBerryBlender->gameEndState = 5;
        else if (gBerryBlender->playerContinueResponses[0] == 0x1111)
        {
            if (gBerryBlender->canceledPlayerCmd == 0x9999)
                gBerryBlender->gameEndState = 2;
            else if (gBerryBlender->canceledPlayerCmd == 0xAAAA)
                gBerryBlender->gameEndState = 1;
            else
                gBerryBlender->gameEndState = 5;
        }
        break;
    case 1:
        gBerryBlender->gameEndState = 3;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
#ifdef ENGLISH
        StringCopy(gStringVar4, gLinkPlayers[gBerryBlender->canceledPlayerId].name);
        StringAppend(gStringVar4, sText_ApostropheSPokeblockCaseIsFull);
#else
        StringCopy(gStringVar4, sText_ApostropheSPokeblockCaseIsFull);
        de_sub_8073110(gStringVar4, gLinkPlayers[gBerryBlender->canceledPlayerId].name);
#endif
        MenuPrintMessage(gStringVar4, 1, 15);
        break;
    case 2:
        gBerryBlender->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
#ifdef ENGLISH
        StringCopy(gStringVar4, gLinkPlayers[gBerryBlender->canceledPlayerId].name);
        StringAppend(gStringVar4, sText_HasNoBerriesToPut);
#else
        StringCopy(gStringVar4, sText_HasNoBerriesToPut);
        de_sub_8073110(gStringVar4, gLinkPlayers[gBerryBlender->canceledPlayerId].name);
#endif
        MenuPrintMessage(gStringVar4, 1, 15);
        break;
    case 3:
        if (Menu_UpdateWindowText())
        {
            gBerryBlender->framesToWait = 0;
            gBerryBlender->gameEndState++;
        }
        break;
    case 4:
        if (++gBerryBlender->framesToWait > 60)
            gBerryBlender->gameEndState = 5;
        break;
    case 5:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(gText_SavingDontTurnOff2, 2, 15);
        SetLinkStandbyCallback();
        gBerryBlender->gameEndState++;
        break;
    case 6:
        if (IsLinkTaskFinished())
        {
            gBerryBlender->framesToWait = 0;
            gBerryBlender->gameEndState++;
            gBerryBlender->linkPlayAgainState = 0;
        }
        break;
    case 7:
        if (LinkPlayAgainHandleSaving())
        {
            PlaySE(SE_SAVE);
            gBerryBlender->gameEndState++;
        }
        break;
    case 8:
        gBerryBlender->gameEndState++;
        SetLinkStandbyCallback();
        break;
    case 9:
        if (IsLinkTaskFinished())
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gBerryBlender->gameEndState++;
        }
        break;
    case 10:
        if (!gPaletteFade.active)
        {
            if (gBerryBlender->playerContinueResponses[0] == 0x2222)
                SetMainCallback2(DoBerryBlending);
            else
            {
                gBerryBlender->framesToWait = 0;
                gBerryBlender->gameEndState++;
            }
        }
        break;
    case 11:
        if (++gBerryBlender->framesToWait > 30)
        {
            SetCloseLinkCallback();
            gBerryBlender->gameEndState++;
        }
        break;
    case 12:
        if (gReceivedRemoteLinkPlayers == 0)
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        break;
    }

    ProcessLinkPlayerCmds();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_CheckPlayAgainLocal(void)
{
    switch (gBerryBlender->gameEndState)
    {
    case 0:
        if (gBerryBlender->playAgainState < 2)
            gBerryBlender->gameEndState = 9;
        if (gBerryBlender->playAgainState == 2)
            gBerryBlender->gameEndState = 2;
        if (gBerryBlender->playAgainState == 3)
            gBerryBlender->gameEndState =1;
        break;
    case 1:
        gBerryBlender->gameEndState = 3;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
        MenuPrintMessage(sText_YourPokeblockCaseIsFull, 1, 15);
        break;
    case 2:
        gBerryBlender->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
        MenuPrintMessage(sText_RunOutOfBerriesForBlending, 1, 15);
        break;
    case 3:
        if (Menu_UpdateWindowText())
            gBerryBlender->gameEndState = 9;
        break;
    case 9:
        BeginFastPaletteFade(3);
        gBerryBlender->gameEndState++;
        break;
    case 10:
        if (!gPaletteFade.active)
        {
            if (gBerryBlender->playAgainState == 0)
                SetMainCallback2(DoBerryBlending);
            else
                SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        }
        break;
    }

    ProcessLinkPlayerCmds();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void ProcessLinkPlayerCmds(void)
{
    if (gReceivedRemoteLinkPlayers)
    {
        if (gRecvCmds[0][0] == 0x2FFF)
        {
            if (gRecvCmds[1][0] == 0x1111)
            {
                switch (gRecvCmds[2][0])
                {
                case 0x8888:
                    gBerryBlender->canceledPlayerCmd = 0x8888;
                    gBerryBlender->canceledPlayerId = gRecvCmds[3][0];
                    break;
                case 0x9999:
                    gBerryBlender->canceledPlayerCmd = 0x9999;
                    gBerryBlender->canceledPlayerId = gRecvCmds[3][0];
                    break;
                case 0xAAAA:
                    gBerryBlender->canceledPlayerCmd = 0xAAAA;
                    gBerryBlender->canceledPlayerId = gRecvCmds[3][0];
                    break;
                }
                gBerryBlender->playerContinueResponses[0] = 0x1111;
            }
            else if (gRecvCmds[1][0] == 0x2222)
                gBerryBlender->playerContinueResponses[0] = 0x2222;
        }
        if (GetMultiplayerId() == 0 && gBerryBlender->playerContinueResponses[0] != 0x1111 && gBerryBlender->playerContinueResponses[0] != 0x2222)
        {
            u8 i;
            for (i = 0; i < GetLinkPlayerCount(); i++)
            {
                if (gRecvCmds[0][i] == 0x2FFF)
                {
                    switch (gRecvCmds[1][i])
                    {
                    case 0x8888:
                        gBerryBlender->playerContinueResponses[i] = 0x8888;
                        break;
                    case 0x7779:
                        gBerryBlender->playerContinueResponses[i] = 0x7779;
                        break;
                    case 0x9999:
                        gBerryBlender->playerContinueResponses[i] = 0x9999;
                        break;
                    case 0xAAAA:
                        gBerryBlender->playerContinueResponses[i] = 0xAAAA;
                        break;
                    }
                }
            }
            for (i = 0; i < GetLinkPlayerCount(); i++)
            {
                if (gBerryBlender->playerContinueResponses[i] == 0)
                    break;
            }
            if (i == GetLinkPlayerCount())
            {
                for (i = 0; i < GetLinkPlayerCount(); i++)
                {
                    if (gBerryBlender->playerContinueResponses[i] != 0x7779)
                        break;
                }
                gSendCmd[0] = 0x2FFF;
                if (i == GetLinkPlayerCount())
                    gSendCmd[1] = 0x2222;
                else
                {
                    gSendCmd[1] = 0x1111;
                    gSendCmd[2] = gBerryBlender->playerContinueResponses[i];
                    gSendCmd[3] = i;
                }
            }
        }
    }
}

static void DrawBlenderCenter(struct BgAffineDstData *dest)
{
    struct BgAffineSrcData affineSrc;
    affineSrc.texX = 30720;
    affineSrc.texY = 20480;
    affineSrc.scrX = 120 - gBerryBlender->bg_X;
    affineSrc.scrY = 80 - gBerryBlender->bg_Y;
    affineSrc.sx = gBerryBlender->centerScale;
    affineSrc.sy = gBerryBlender->centerScale;
    affineSrc.alpha = gBerryBlender->arrowPos;
    BgAffineSet(&affineSrc, dest, 1);
}

static void UpdateBlenderCenter(void)
{
    gBerryBlender->field_58 = gBerryBlender->arrowPos;
    gBerryBlender->arrowPos += gBerryBlender->speed;
    DrawBlenderCenter(&gBerryBlender->field_168);
}

static void sub_80514A4(void)
{
    REG_BG2PA = gBerryBlender->field_168.pa;
    REG_BG2PB = gBerryBlender->field_168.pb;
    REG_BG2PC = gBerryBlender->field_168.pc;
    REG_BG2PD = gBerryBlender->field_168.pd;
    REG_BG2X = gBerryBlender->field_168.dx;
    REG_BG2Y = gBerryBlender->field_168.dy;
}

static void SetBgPos(void)
{
    REG_BG1HOFS = gBerryBlender->bg_X;
    REG_BG1VOFS = gBerryBlender->bg_Y;
    REG_BG0HOFS = gBerryBlender->bg_X;
    REG_BG0VOFS = gBerryBlender->bg_Y;
}

static void SpriteCB_Particle(struct Sprite* sprite)
{
    sprite->data[2] += sprite->data[0];
    sprite->data[3] += sprite->data[1];
    sprite->x2 = sprite->data[2] / 8;
    sprite->y2 = sprite->data[3] / 8;
    if (sprite->animEnded)
        DestroySprite(sprite);
}

static void CreateParticleSprites(void)
{
    s32 limit = (Random() % 2) + 1;
    s32 i;

    for (i = 0; i < limit; i++)
    {
        u16 rand;
        s32 x, y;
        u8 spriteId;

        rand = gBerryBlender->arrowPos + (Random() % 20);

        x = gSineTable[(rand & 0xFF) + 64] / 4;
        y = gSineTable[(rand & 0xFF)] / 4;

        spriteId = CreateSprite(&sSpriteTemplate_Particles, x + 120, y + 80, 1);
        gSprites[spriteId].data[0] = 16 - (Random() % 32);
        gSprites[spriteId].data[1] = 16 - (Random() % 32);

        gSprites[spriteId].callback = SpriteCB_Particle;
    }
}

static void SpriteCB_ScoreSymbol(struct Sprite* sprite)
{
    sprite->data[0]++;
    sprite->y2 = -(sprite->data[0] / 3);
    if (sprite->animEnded)
        DestroySprite(sprite);
}

static void SpriteCB_ScoreSymbolBest(struct Sprite* sprite)
{
    sprite->data[0]++;
    sprite->y2 = -(sprite->data[0] * 2);
    if (sprite->y2 < -12)
        sprite->y2 = -12;
    if (sprite->animEnded)
        DestroySprite(sprite);
}

void SetPlayerBerryData(u8 playerId, u16 itemId)
{
    gBerryBlender->chosenItemId[playerId] = itemId;
    ConvertItemToBlenderBerry(&gBerryBlender->blendedBerries[playerId], itemId);
}

void unref_sub_80516F8(u8 taskID)
{
    struct Task* task = &gTasks[taskID];
    if (gReceivedRemoteLinkPlayers)
    {
        s32 i;
        if (GetMultiplayerId() == 0)
        {
            if (++task->data[0] > 120)
                task->data[0] = 0;
            if (task->data[0] == 100)
            {
                Text_EraseWindowRect(&gBerryBlender->field_4, 0, 0, 16, 20);
                Menu_DrawStdWindowFrame(4, 4, 10, 12);
                for (i = 0; i < 3; i++)
                {
                    if (gLinkPlayers[i + 1].trainerId != 0)
                        Menu_PrintText(gUnknown_08216284[i], 5, (2 * i) + 5);
                    Menu_DrawStdWindowFrame(0, 13, 29, 19);
                    Menu_PrintText(sText_PressAToStart, 1, 15);
                }
            }
            if (JOY_NEW(A_BUTTON))
            {
                SendBerryBlenderNoSpaceForPokeblocks();
                DestroyTask(taskID);
            }
        }
        else
        {
            if (task->data[0] == 10)
                Menu_PrintText(sText_PleaseWaitAWhile, 3, 10);
            if (++task->data[0] > 120)
                task->data[0] = 0;
            if (gLinkDummy2 > 4 && gReceivedRemoteLinkPlayers == 1)
                DestroyTask(taskID);
        }
    }
}

static void SpriteCB_CountdownNumber(struct Sprite* sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->data[1] += 8;
        if (sprite->data[1] > 88)
        {
            sprite->data[1] = 88;
            sprite->data[0]++;
            PlaySE(SE_BALL_BOUNCE_1);
        }
        break;
    case 1:
        sprite->data[2] += 1;
        if (sprite->data[2] > 20)
        {
            sprite->data[0]++;
            sprite->data[2] = 0;
        }
        break;
    case 2:
        sprite->data[1] += 4;
        if (sprite->data[1] > 176)
        {
            if (++sprite->data[3] == 3)
            {
                DestroySprite(sprite);
                CreateSprite(&sSpriteTemplate_Start, 120, -20, 2);
            }
            else
            {
                sprite->data[0] = 0;
                sprite->data[1] = -16;
                StartSpriteAnim(sprite, sprite->data[3]);
            }
        }
        break;
    }
    sprite->y2 = sprite->data[1];
}

static void SpriteCB_Start(struct Sprite* sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->data[1] += 8;
        if (sprite->data[1] > 92)
        {
            sprite->data[1] = 92;
            sprite->data[0]++;
            PlaySE(SE_PIN);
        }
        break;
    case 1:
        sprite->data[2] += 1;
        if (sprite->data[2] > 20)
            sprite->data[0]++;
        break;
    case 2:
        sprite->data[1] += 4;
        if (sprite->data[1] > 176)
        {
            gBerryBlender->mainState++;
            DestroySprite(sprite);
        }
        break;
    }
    sprite->y2 = sprite->data[1];
}

static void TryUpdateProgressBar(u16 current, u16 limit)
{
    if (gBerryBlender->maxProgressBarValue < current)
    {
        gBerryBlender->maxProgressBarValue += 2;
        UpdateProgressBar(gBerryBlender->maxProgressBarValue, limit);
    }
}

static void UpdateProgressBar(u16 value, u16 limit)
{
    s32 amountFilled, maxFilledSegment, subSegmentsFilled, i;
    u16* vram;

    vram = (u16*)(VRAM + 0x6000);
    amountFilled = (value * 64) / limit;
    maxFilledSegment = amountFilled / 8;
    for (i = 0; i < maxFilledSegment; i++)
    {
        vram[11 + i] = 0x81E9;
        vram[43 + i] = 0x81F9;
    }
    subSegmentsFilled = amountFilled % 8;
    if (subSegmentsFilled != 0)
    {
        vram[11 + i] = subSegmentsFilled - 32287;
        vram[43 + i] = subSegmentsFilled - 32271;
        i++;
    }
    for (; i < 8; i++)
    {
        vram[11 + i] = 33249;
        vram[43 + i] = 33249 + 16;
    }
}

static u32 ArrowSpeedToRPM(u16 speed)
{
    return 360000 * speed / 0x10000;
}

static void UpdateRPM(u16 speed)
{
    u8 i;
    u8 digits[5];

    u32 currentRPM = ArrowSpeedToRPM(speed);
    if (gBerryBlender->maxRPM < currentRPM)
        gBerryBlender->maxRPM = currentRPM;
    for (i = 0; i < 5; i++)
    {
        digits[i] = currentRPM % 10;
        currentRPM /= 10;
    }
    *((u16*)(VRAM + 0x6458)) = digits[4] + 0x8172;
    *((u16*)(VRAM + 0x645A)) = digits[3] + 0x8172;
    *((u16*)(VRAM + 0x645C)) = digits[2] + 0x8172;
    *((u16*)(VRAM + 0x6460)) = digits[1] + 0x8172;
    *((u16*)(VRAM + 0x6462)) = digits[0] + 0x8172;
}

static void ShakeBgCoordForHit(s16* coord, u16 speed)
{
    if (*coord == 0)
        *coord = (Random() % speed) - (speed / 2);
}

static void RestoreBgCoord(s16* coord)
{
    if (*coord < 0 )
        (*coord)++;
    if (*coord > 0 )
        (*coord)--;
}

static void RestoreBgCoords(void)
{
    RestoreBgCoord(&gBerryBlender->bg_X);
    RestoreBgCoord(&gBerryBlender->bg_Y);
}

static void BlenderLandShakeBgCoord(s16* coord, u16 timer)
{
    s32 strength;
    if (timer < 10)
        strength = 16;
    else
        strength = 8;
    if (*coord == 0)
        *coord = (Random() % strength) - (strength / 2);
    else
    {
        if (*coord < 0)
            (*coord)++;
        if (*coord > 0)
            (*coord)--;
    }
}

static bool8 UpdateBlenderLandScreenShake(void)
{
    if (gBerryBlender->framesToWait == 0)
    {
        gBerryBlender->bg_X = 0;
        gBerryBlender->bg_Y = 0;
    }
    gBerryBlender->framesToWait++;
    BlenderLandShakeBgCoord(&gBerryBlender->bg_X, gBerryBlender->framesToWait);
    BlenderLandShakeBgCoord(&gBerryBlender->bg_Y, gBerryBlender->framesToWait);
    if (gBerryBlender->framesToWait == 20)
    {
        gBerryBlender->bg_X = 0;
        gBerryBlender->bg_Y = 0;
        return TRUE;
    }
    else
        return FALSE;
}

static void SpriteCB_PlayerArrow(struct Sprite* sprite)
{
   sprite->x2 = -(gBerryBlender->bg_X);
   sprite->y2 = -(gBerryBlender->bg_Y);
}

static void TryUpdateBerryBlenderRecord(void)
{
    if (gSaveBlock1.berryBlenderRecords[gBerryBlender->numPlayers - 2] < gBerryBlender->maxRPM)
        gSaveBlock1.berryBlenderRecords[gBerryBlender->numPlayers - 2] = gBerryBlender->maxRPM;
}

bool8 PrintBlendingResults(void)
{
    u16 i;

    struct Pokeblock pokeblock;
    u8 flavors[6];
    u8 text[2][10];
    u16 berryIds[4]; // unused

    switch (gBerryBlender->mainState)
    {
    case 0:
        gBerryBlender->mainState++;
        gBerryBlender->framesToWait = 17;
        break;
    case 1:
        gBerryBlender->framesToWait -= 10;
        if (gBerryBlender->framesToWait < 0)
        {
            gBerryBlender->framesToWait = 0;
            gBerryBlender->mainState++;
        }
        break;
    case 2:
        if (++gBerryBlender->framesToWait > 20)
        {
            for (i = 0; i < 3; i++)
                DestroySprite(&gSprites[gBerryBlender->scoreIconIds[i]]);
            gBerryBlender->framesToWait = 0;
            gBerryBlender->mainState++;
        }
        break;
    case 3:
        {
            u8* txtPtr;
            u16 secondsPassed, minutes, seconds;

            Menu_DrawStdWindowFrame(4, 2, 25, 17);
            MenuPrint_Centered(sText_BlendingResults, 5, 3, 160);
            for (i = 0; i < gBerryBlender->numPlayers; i++)
            {
                u8 place = gBerryBlender->playerPlaces[i];
                txtPtr = text[0];

                StringCopy(txtPtr, gBerryBlender->blendedBerries[place].name);
                ConvertInternationalString(txtPtr, gLinkPlayers[place].language);
#ifdef ENGLISH
                StringAppend(txtPtr, sText_SpaceBerry);
#else
                de_sub_8073174(txtPtr, sText_SpaceBerry);
#endif
                txtPtr = gBerryBlender->stringVar;
                txtPtr = ConvertIntToDecimalStringSimple(txtPtr, i + 1);
                txtPtr[0] = CHAR_SPACE;
                txtPtr[1] = CHAR_PERIOD;
                txtPtr[2] = CHAR_SPACE;
                txtPtr += 3;
                txtPtr = AlignStringInMenuWindow(txtPtr, gLinkPlayers[place].name, 88, 0);
                AlignStringInMenuWindow(txtPtr, text[0], 157, 0);
                Menu_PrintText(gBerryBlender->stringVar, 5, gUnknown_082165E9[gBerryBlender->numPlayers] + (i * gUnknown_082165EE[gBerryBlender->numPlayers]));
            }
            ConvertIntToDecimalStringN(text[0], gBerryBlender->maxRPM % 100, 2, 2);
            txtPtr = gBerryBlender->stringVar;
            txtPtr = StringCopy(txtPtr, sText_MaximumSpeed);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlender->maxRPM / 100, 121, 1);

#ifdef ENGLISH
            txtPtr[0] = CHAR_SPACE;
            txtPtr[1] = CHAR_PERIOD;
            txtPtr[2] = CHAR_SPACE;
            txtPtr += 3;
            txtPtr = AlignStringInMenuWindow(txtPtr, text[0], 142, 1);
#else
            *txtPtr++ = CHAR_COMMA;
            txtPtr = AlignStringInMenuWindow(txtPtr, text[0], 136, 1);
#endif
            StringCopy(txtPtr, sText_RPM);
            Menu_PrintText(gBerryBlender->stringVar, 5, 13);

            secondsPassed = gBerryBlender->gameFrameTime / 60;
            seconds = secondsPassed % 60;
            minutes = secondsPassed / 60;
            ConvertIntToDecimalStringN(text[0], minutes, 2, 2);
            ConvertIntToDecimalStringN(text[1], seconds, 2, 2);
            txtPtr = gBerryBlender->stringVar;
            txtPtr = StringCopy(txtPtr, sText_Time);

#ifdef ENGLISH
            txtPtr = AlignStringInMenuWindow(txtPtr, text[0], 102, 1);
#else
            txtPtr = AlignStringInMenuWindow(txtPtr, text[0], 99, 1);
#endif
            txtPtr = StringAppend(txtPtr, sText_Min);

            txtPtr = AlignStringInMenuWindow(txtPtr, text[1], 136, 1);
            StringCopy(txtPtr, sText_Sec);

            Menu_PrintText(gBerryBlender->stringVar, 5, 15);

            gBerryBlender->framesToWait = 0;
            gBerryBlender->mainState++;
        }
        break;
    case 4:
        if (JOY_NEW(A_BUTTON))
            gBerryBlender->mainState++;
        break;
    case 5:
        Menu_EraseScreen();
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (gBerryBlender->chosenItemId[i] != 0)
                berryIds[i] = gBerryBlender->chosenItemId[i] - 133;
        }
        Debug_SetStageVars();
        CalculatePokeblock(gBerryBlender->blendedBerries, &pokeblock, gBerryBlender->numPlayers, flavors, gBerryBlender->maxRPM);
        PrintMadePokeblockString(&pokeblock, gBerryBlender->stringVar);
        CreateTask(Task_PlayPokeblockFanfare, 6);
#if DEBUG
        ConvertIntToHexStringN(text[0], GetBerryBlenderKeySendAttempts(), 0, 4);
        StringAppend(text[0], sText_NewParagraph);
        StringAppend(gBerryBlender->stringVar, text[0]);
#endif
        MenuPrintMessage(gBerryBlender->stringVar, 1, 15);
        RemoveBagItem(gSpecialVar_ItemId, 1);
        AddPokeblock(&pokeblock);
        gBerryBlender->mainState++;
        break;
    case 6:
        if (Menu_UpdateWindowText())
        {
            TryUpdateBerryBlenderRecord();
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void PrintMadePokeblockString(struct Pokeblock* pokeblock, u8* dst)
{
    u8 text[12];
    u8 flavorLvl, feel;

    dst[0] = EOS;
    StringCopy(dst, gPokeblockNames[pokeblock->color]);
#ifdef ENGLISH
    StringAppend(dst, sText_WasMade);
#else
    de_sub_8073174(dst, sText_WasMade);
#endif
    StringAppend(dst, sText_NewLine);

    flavorLvl = GetHighestPokeblocksFlavorLevel(pokeblock);
    feel = GetPokeblocksFeel(pokeblock);

    StringAppend(dst, sText_TheLevelIs);
    ConvertIntToDecimalStringN(text, flavorLvl, 0, 3);
    StringAppend(dst, text);

    StringAppend(dst, sText_TheFeelIs);
    ConvertIntToDecimalStringN(text, feel, 0, 3);
    StringAppend(dst, text);

    StringAppend(dst, sText_Dot2);
    StringAppend(dst, sText_NewParagraph);
}

static void SortBasedOnPoints(u8* places, u8 playersNum, u32* scores)
{
    s32 i, j;
    for (i = 0; i < playersNum; i++)
    {
        for (j = 0; j < playersNum; j++)
        {
            if (scores[places[i]] > scores[places[j]])
            {
                u8 temp = places[i];
                places[i] = places[j];
                places[j] = temp;
            }
        }
    }
}

static void SortScores(void)
{
    u8 i;
    u8 places[4];
    u32 points[4];

    for (i = 0; i < gBerryBlender->numPlayers; i++)
        places[i] = i;
    for (i = 0; i < gBerryBlender->numPlayers; i++)
    {
        points[i] = 1000000 * gBerryBlender->scores[i][SCORE_BEST];
        points[i] += 1000 * gBerryBlender->scores[i][SCORE_GOOD];
        points[i] += 1000 - gBerryBlender->scores[i][SCORE_MISS];
    }
    SortBasedOnPoints(places, gBerryBlender->numPlayers, points);
    for (i = 0; i < gBerryBlender->numPlayers; i++)
        gBerryBlender->playerPlaces[i] = places[i];
}

static bool8 PrintBlendingRanking(void)
{
    u16 i;
    switch (gBerryBlender->mainState)
    {
    case 0:
        gBerryBlender->mainState++;
        gBerryBlender->framesToWait = 255;
        break;
    case 1:
        gBerryBlender->framesToWait -= 10;
        if (gBerryBlender->framesToWait < 0)
        {
            gBerryBlender->framesToWait = 0;
            gBerryBlender->mainState++;
        }
        break;
    case 2:
        if (++gBerryBlender->framesToWait > 20)
        {
            gBerryBlender->framesToWait = 0;
            gBerryBlender->mainState++;
        }
        break;
    case 3:
        Menu_DrawStdWindowFrame(4, 2, 25, 17);
        MenuPrint_Centered(sText_Ranking, 5, 3, 160);

        gBerryBlender->scoreIconIds[SCORE_BEST] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 140, 52, 0);
        gSprites[gBerryBlender->scoreIconIds[SCORE_BEST]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[gBerryBlender->scoreIconIds[SCORE_BEST]], 3);

        gBerryBlender->scoreIconIds[SCORE_GOOD] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 164, 52, 0);
        gSprites[gBerryBlender->scoreIconIds[SCORE_GOOD]].callback = SpriteCallbackDummy;

        gBerryBlender->scoreIconIds[SCORE_MISS] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 188, 52, 0);
        gSprites[gBerryBlender->scoreIconIds[SCORE_MISS]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[gBerryBlender->scoreIconIds[SCORE_MISS]], 1);

        SortScores();

        for (i = 0; i < gBerryBlender->numPlayers; i++)
        {
            u8 place = gBerryBlender->playerPlaces[i];
            u8* txtPtr = gBerryBlender->stringVar;

            txtPtr[0] = EXT_CTRL_CODE_BEGIN;
            txtPtr[1] = 0x13;
            txtPtr[2] = 4;
            txtPtr += 3;

            txtPtr = ConvertIntToDecimalStringSimple(txtPtr, i + 1);

            txtPtr[0] = CHAR_SPACE;
            txtPtr[1] = CHAR_PERIOD;
            txtPtr[2] = CHAR_SPACE;
            txtPtr += 3;

            txtPtr = StringCopy(txtPtr, gLinkPlayers[place].name);

            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlender->scores[place][SCORE_BEST], 108, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlender->scores[place][SCORE_GOOD], 132, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlender->scores[place][SCORE_MISS], 156, 1);

            Menu_PrintText(gBerryBlender->stringVar, 5, i * gUnknown_082165F3[gBerryBlender->numPlayers] + 8);
        }
        gBerryBlender->framesToWait = 0;
        gBerryBlender->mainState++;
        break;
    case 4:
        if (++gBerryBlender->framesToWait > 20)
            gBerryBlender->mainState++;
        break;
    case 5:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBerryBlender->mainState++;
        }
        break;
    case 6:
        gBerryBlender->mainState = 0;
        return TRUE;
    }
    return FALSE;
}

// debug menu goes here

void debug_sub_80524BC(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    SetVBlankCallback(VBlankCB1_BerryBlender);
    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    SeedRng(gMain.vblankCounter1);
    REG_DISPCNT = 0x1540;
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    sBlenderDebug.BPM = 8000;
    sBlenderDebug.field_10++;
    SetMainCallback2(sub_8052AF8);
}

// Partially fixes the clipping on longer names.
#if DEBUG_FIX
#  define BLENDER_DEBUG_BERRY_LEN 12
#else
#  define BLENDER_DEBUG_BERRY_LEN 6
#endif

static void BlenderDebug_PrintBerryData(void)
{
    u8 text[128];
    u8 i;

    StringCopy(text, sText_BPM);
    Menu_PrintText(text, 2, 0);

    ConvertIntToDecimalStringN(text, sBlenderDebug.BPM / 100, 2, 3);
    Menu_PrintText(text, 6, 0);

    for (i = 0; i < 4; i++)
    {
        u8 var;

        if (sBlenderDebug.cursorPos == i)
        {
            text[0] = 0xEF;
            CopyItemName(sBlenderDebug.berries[i] + 133, &text[1]);
        }
        else
        {
            CopyItemName(sBlenderDebug.berries[i] + 133, &text[0]);
            text[BLENDER_DEBUG_BERRY_LEN] = CHAR_SPACE;
            text[BLENDER_DEBUG_BERRY_LEN + 1] = EOS;
        }
        var = (i * 3) + 3;
        Menu_PrintText(text, 2, var);

        ConvertIntToDecimalStringN(&text[0], gBerries[sBlenderDebug.berries[i]].spicy, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToDecimalStringN(&text[3], gBerries[sBlenderDebug.berries[i]].dry, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToDecimalStringN(&text[6], gBerries[sBlenderDebug.berries[i]].sweet, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToDecimalStringN(&text[9], gBerries[sBlenderDebug.berries[i]].bitter, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToDecimalStringN(&text[12], gBerries[sBlenderDebug.berries[i]].sour, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToDecimalStringN(&text[15], gBerries[sBlenderDebug.berries[i]].smoothness, 2, 2);

        text[17] = EOS;
        Menu_PrintText(text, BLENDER_DEBUG_BERRY_LEN + 1, var);
    }
    if (sBlenderDebug.pokeblock.color != 0)
    {
        StringCopy(text, gPokeblockNames[sBlenderDebug.pokeblock.color]);
        Menu_PrintText(text, 2, 15);

        ConvertIntToHexStringN(&text[0], sBlenderDebug.spicy, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToHexStringN(&text[3], sBlenderDebug.dry, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToHexStringN(&text[6], sBlenderDebug.sweet, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToHexStringN(&text[9], sBlenderDebug.bitter, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToHexStringN(&text[12], sBlenderDebug.sour, 2, 2);
        StringAppend(text, sText_Space);

        ConvertIntToHexStringN(&text[15], sBlenderDebug.feel, 2, 2);

        text[17] = EOS;
        Menu_PrintText(text, BLENDER_DEBUG_BERRY_LEN + 1, 17);
    }
}

static void sub_80527BC(void)
{
    u8 text[70];
    u8 buffer[10];
    u16 i;

    if (gUnknown_020297DC == 1)
    {
        u16 j;
        for (j = 0; j < 10; j++)
            gUnknown_03004840[j] = 0;
        gUnknown_03004830 = Random();
        gUnknown_020297E0 = 0;
        gUnknown_020297DC = 2;
        for (i = 0; i < 200; i++)
            gSharedMem[i] = 0;
        gUnknown_020297E8 = 0;
    }
    for (i = 0; i < 100; i++)
    {
        if (((Random() >> 15) & 1) == gUnknown_020297E8)
            gUnknown_020297E0++;
        else
        {
            u16* ewramPtr = ((u16*)(gSharedMem));
            ewramPtr[gUnknown_020297E4] = gUnknown_020297E0;
            gUnknown_020297E4++;
            gUnknown_020297E0 = 0;
            gUnknown_020297E8 ^= 1;
        }
    }
    text[0] = EOS;

    ConvertIntToHexStringN(buffer, gUnknown_03004830, 2, 8);
    StringAppend(text, buffer);
    StringAppend(text, sText_Space);

    ConvertIntToHexStringN(buffer, gUnknown_020297E0, 2, 8);
    StringAppend(text, buffer);
    StringAppend(text, sNewLineString_1);

    if (gUnknown_020297DC == 3)
    {
        ConvertIntToHexStringN(buffer, gUnknown_020297E4, 2, 16);
        StringAppend(text, buffer);
        gUnknown_020297DC = 0;
    }

    Menu_PrintText(text, 2, 15);
}

static void sub_8052918(void)
{
    if (JOY_NEW(R_BUTTON))
    {
        sBlenderDebug.BPM += 1000;
        if (sBlenderDebug.BPM > 30000)
            sBlenderDebug.BPM = 1000;
        sBlenderDebug.field_10++;
    }
    if (JOY_NEW(L_BUTTON))
    {
        sBlenderDebug.BPM -= 1000;
        if (sBlenderDebug.BPM < 0)
            sBlenderDebug.BPM = 30000;
        sBlenderDebug.field_10++;
    }
    if (JOY_NEW(DPAD_UP))
    {
        sBlenderDebug.cursorPos -= 1;
        if (sBlenderDebug.cursorPos < 0)
            sBlenderDebug.cursorPos = 3;
        sBlenderDebug.field_10++;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        sBlenderDebug.cursorPos += 1;
        if (sBlenderDebug.cursorPos > 3)
            sBlenderDebug.cursorPos = 0;
        sBlenderDebug.field_10++;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (--sBlenderDebug.berries[sBlenderDebug.cursorPos] < 0)
            sBlenderDebug.berries[sBlenderDebug.cursorPos] = 42;
        sBlenderDebug.field_10++;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (++sBlenderDebug.berries[sBlenderDebug.cursorPos] > 42)
            sBlenderDebug.berries[sBlenderDebug.cursorPos] = 0;
        sBlenderDebug.field_10++;
    }
    if (JOY_NEW(A_BUTTON))
    {
        u16 berryIDs[4];
        struct BlenderBerry berries[4];

        u16 i, notEnigma = 0;
        for (i = 0; i < 4; i++)
        {
            if (sBlenderDebug.berries[i] != 42)
            {
                notEnigma++;
                berryIDs[i] = sBlenderDebug.berries[i];
                ConvertItemToBlenderBerry(&berries[i], sBlenderDebug.berries[i] + 133);
            }
            else
                break;
        }
        if (notEnigma > 1)
        {
            Debug_CalculatePokeblock(berries, &sBlenderDebug.pokeblock, notEnigma, &sBlenderDebug.spicy, sBlenderDebug.BPM);
            sBlenderDebug.field_10++;
        }
        else
            sBlenderDebug.pokeblock.color = 0xFF;
    }
    if (sBlenderDebug.field_10)
    {
        BlenderDebug_PrintBerryData();
        sBlenderDebug.field_10 = 0;
    }
    if (JOY_NEW(SELECT_BUTTON) && gUnknown_020297DC == 0)
    {
        gUnknown_020297DC++;
        gUnknown_020297E0 = 0;
        SeedRng(gMain.vblankCounter1);
    }
    if (gUnknown_020297DC != 0)
        sub_80527BC();
}

static void sub_8052AF8(void)
{
    sub_8052918();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

// debug menu ends
// blender record window begins

void ShowBerryBlenderRecordWindow(void)
{
    u8 text[30];
    s32 i;

    Menu_DrawStdWindowFrame(6, 3, 23, 16);
    Menu_PrintText(gText_BlenderMaxSpeedRecord, 8, 4);
    Menu_PrintText(gText_234Players, 8, 9);

    for (i = 0; i < 3; i++)
    {
        u32 record = gSaveBlock1.berryBlenderRecords[i];
        u8* txtPtr = AlignInt1InMenuWindow(text, record / 100, 18, 1);

#ifdef ENGLISH
        txtPtr[0] = CHAR_SPACE;
        txtPtr[1] = CHAR_PERIOD;
        txtPtr[2] = CHAR_SPACE;
        txtPtr += 3;
#else
        *txtPtr++ = CHAR_COMMA;
#endif

        txtPtr = ConvertIntToDecimalStringN(txtPtr, record % 100, 2, 2);
        StringAppend(txtPtr, sText_RPM);
        Menu_PrintText(text, 15, i * 2 + 9);
    }
}

static void Task_PlayPokeblockFanfare(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0)
    {
        PlayFanfare(MUS_LEVEL_UP);
        gTasks[taskId].data[0]++;
    }
    if (IsFanfareTaskInactive())
    {
        PlayBGM(gBerryBlender->savedMusic);
        DestroyTask(taskId);
    }
}
