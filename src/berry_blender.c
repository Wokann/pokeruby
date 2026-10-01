#include "global.h"
#include "berry.h"
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
#include "strings2.h"
#include "task.h"
#include "text_window.h"
#include "trig.h"
#include "constants/songs.h"
#include "constants/items.h"

#define BLENDER_SCORE_BEST      0
#define BLENDER_SCORE_GOOD      1
#define BLENDER_SCORE_MISS      2

#define BLENDER_MAX_PLAYERS 4
#define BLENDER_SCORES_NO   3

#define FLAVOUR_SPICY       0
#define FLAVOUR_DRY         1
#define FLAVOUR_SWEET       2
#define FLAVOUR_BITTER      3
#define FLAVOUR_SOUR        4

struct BlenderBerry
{
    u16 itemId;
    u8 name[7];
    u8 flavors[5];
    u8 smoothness;
};

struct TimeAndRPM
{
    u32 time;
    u16 maxRPM;
};

struct BerryBlender
{
    u8 mainState;
    u8 loadGfxState;
    struct Window menuWindow;
    u8 unused_35[25];
    u16 unused_4E;
    u8 scoreIconIds[3];
    u16 arrowPos;
    s16 speed;
    s16 previousArrowPos;
    u16 maxRPM;
    u8 playerArrowSpriteIds[BLENDER_MAX_PLAYERS];
    u8 playerArrowSpriteIds2[BLENDER_MAX_PLAYERS];
    u8 unused_64[11];
    u8 gameEndState;
    u16 playerContinueResponses[BLENDER_MAX_PLAYERS];
    u16 canceledPlayerCmd;
    u16 canceledPlayerId;
    u16 playAgainState;
    u8 slowdownTimer;
    u8 unused_7F;
    u16 chosenItemId[BLENDER_MAX_PLAYERS];
    u8 numPlayers;
    u8 unused_89[11];
    u16 unused_94;
    u8 unused_96[4];
    u16 arrowIdToPlayerId[BLENDER_MAX_PLAYERS];
    u16 playerIdToArrowId[BLENDER_MAX_PLAYERS];
    u8 yesNoAnswer;
    u8 stringVar[129];
    u32 gameFrameTime;
    s32 framesToWait;
    u32 unused_134;
    u8 unused_138[4];
    u8 playerToThrowBerry;
    u8 unused_13D;
    u16 progressBarValue;
    u16 maxProgressBarValue;
    u16 centerScale;
    s16 bg_X;
    s16 bg_Y;
    u8 opponentTaskIds[3];
    u8 perfectOpponents;
    u16 scores[BLENDER_MAX_PLAYERS][3];
    u8 playerPlaces[BLENDER_MAX_PLAYERS];
    struct BgAffineDstData bgAffineDst;
    u16 savedMusic;
    struct BlenderBerry blendedBerries[BLENDER_MAX_PLAYERS];
    struct TimeAndRPM smallBlock;
    u32 linkPlayAgainState;
};

struct BlenderDebug
{
    s8 cursorPos;
    s8 berries[4];
    struct Pokeblock pokeblock;
    u8 needsRedraw;
    u8 spicy;
    u8 dry;
    u8 sweet;
    u8 bitter;
    u8 sour;
    u8 feel;
    s8 unused_17;
    s8 unused_18;
    s8 unused_19;
    s16 BPM;
};

// other files functions
void m4aMPlayPitchControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s16 pitch);
void m4aMPlayTempoControl(struct MusicPlayerInfo *mplayInfo, u16 tempo);
void m4aMPlayStop(struct MusicPlayerInfo *mplayInfo);
void ChooseBerryForBlender(void);
u8 CreateSpinningBerrySprite(u8 berryId, u8 x, u8 y, bool8 animate);
void SetOutlineCursorPosition(u8 x, u8 y);
u8 CreateOutlineCursor(u8 subpriority, u16 paletteTag, u8 tileIndex, u16 color, u8 width);
s8 GetFirstFreePokeblockSlot(void);
#ifdef GERMAN
extern void GermanStringReplaceNamePlaceholder();
#endif

extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u16 gSpecialVar_ItemId;
extern u8 gIsDebugMode;
extern u8 gLinkDummyCommandReceived;

extern const struct WindowTemplate gBerryBlenderWindowTemplate;
extern const struct Berry gBerries[];

// ewram
static EWRAM_DATA u8 sBlenderDebugRngTestState = 0;
static EWRAM_DATA u32 sBlenderDebugRngRunLength = 0;
static EWRAM_DATA u32 sBlenderDebugRngRunIndex = 0;
static EWRAM_DATA u8 sBlenderDebugRngBit = 0;

// iwram common
u16 gBlenderDebugRngSeed;
u8 gInGameOpponentsNo;
u16 gUnusedBlenderDebugRngData[10];
// Preserve this common symbol's original IWRAM placement for a matching ROM.
struct BerryBlender *gBerryBlenderData;
#define sBerryBlender gBerryBlenderData

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
void SetPlayerBerryData(u8 playerId, u16 itemId);

static void ApplyBlenderCenterAffine(void);
static void SetBgPos(void);
static void CB2_LoadBerryBlender(void);
static void InitLocalPlayers(u8 numOpponents);
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
void SpriteCB_ScoreSymbolBest(struct Sprite* sprite);
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
static void CB2_BerryBlenderDebugMenu(void);
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
static const u8 sUnusedJpnText_Arrow[] = _("▶");
static const u8 sUnusedJpnText_Space[] = _(" ");
static const u8 sUnusedJpnText_Cool[] = _("カッコイ"); // "cool" (missing an イ at the end)
static const u8 sUnusedJpnText_Cute[] = _("カワイイ"); // "cute"
static const u8 sUnusedJpnText_Beautiful[] = _("ウツクシ"); // "beautiful" (missing an イ at the end)
static const u8 sUnusedJpnText_Smart[] = _("カシコイ"); // "smart"
static const u8 sUnusedJpnText_Tough[] = _("タクマシ"); // "tough" (missing an イ at the end)

static const u8 sText_NewParagraph[] = _("\p");

// unreferenced; These appear to be the first names of four people who worked on the game.
static const u8 sUnusedJpnText_Tetsuji[10] = _("てつじ"); // Tetsuji (Ohta)
static const u8 sUnusedJpnText_Akito[10] = _("あきと"); // Akito (Mori)
static const u8 sUnusedJpnText_Shigeru[10] = _("シゲル"); // Shigeru (Ohmori)
static const u8 sUnusedJpnText_Yoshinori[10] = _("ヨシノリ"); // Yoshinori (Matsuda)

static const u8 sText_2Pok[] = _("2Pok");
static const u8 sText_3Pok[] = _("3Pok");
static const u8 sText_4Pok[] = _("4Pok");
static const u8* const sDebugText_PlayerCount[] =
{
      sText_2Pok, sText_3Pok, sText_4Pok
};

// unreferenced player strings
static const u8 sUnusedText_1P[4] = _("1P");
static const u8 sUnusedText_2P[4] = _("2P");
static const u8 sUnusedText_3P[4] = _("3P");
static const u8 sUnusedText_4P[4] = _("4P");

#ifdef ENGLISH
static const u8 sBlenderOpponentName1[] = _("MISTER");
static const u8 sBlenderOpponentName2[] = _("LADDIE");
static const u8 sBlenderOpponentName3[] = _("LASSIE");
#else // GERMAN
static const u8 sBlenderOpponentName1[] = _("OPI");
static const u8 sBlenderOpponentName2[] = _("KUMPEL");
static const u8 sBlenderOpponentName3[] = _("TUSSI");
#endif // ENGLISH
static const u8* const sBlenderOpponentsNames[] =
{
    sBlenderOpponentName1, sBlenderOpponentName2, sBlenderOpponentName3
};

static const u8 sRedColorString[] = _("{COLOR RED}");
static const u8 sNewLineString_0[] = _("\n");
static const u8 sSpaceString_0[] = _(" ");

static const s8 sPlayerArrowQuadrant[][2] =
{
    {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
};

static const u8 sPlayerNameTextCoords[][2] =
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

static const u8 sBerryMasterBerries[] = {1, 1, 2, 3, 4};
static const u8 sBlackPokeblockFlavorFlags[] = {0x1C, 0x16, 0x13, 0x1A, 0x19, 0x0E, 0x0D, 0x0B, 0x07, 0x15};
static const u8 sResultsPlayerNameStartY[] = {6, 6, 6, 6, 5};
static const u8 sResultsPlayerNameSpacingY[] = {3, 3, 3, 2, 2};
static const u8 sRankingPlayerNameSpacingY[] = {3, 3, 3, 3, 2};

static const u8 sText_Space[] = _(" ");
static const u8 sText_BPM[] = _("BPM");
static const u8 sText_Dash[] = _("-");
static const u8 sNewLineString_1[] = _("\n");
static const u8 sNewLineString_2[] = _("\n");

static void UpdateHitPitch(void)
{
    m4aMPlayPitchControl(&gMPlayInfo_SE2, 0xFFFF, (sBerryBlender->speed - 128) * 2);
}

static void VBlankCB0_BerryBlender(void)
{
    ApplyBlenderCenterAffine();
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
    switch (sBerryBlender->loadGfxState)
    {
    case 0:
        LZDecompressWram(gBerryBlenderCenter_Gfx, sBerryBlenderGfxBuffer);
        sBerryBlender->loadGfxState++;
        break;
    case 1:
        DmaCopy16Defvars(3, sBlenderCenter_Tilemap, (void *)(VRAM + 0x4000), 0x400);
        LoadPalette(sBlenderCenter_Pal, 0, 0x100);
        sBerryBlender->loadGfxState++;
        break;
    case 2:
        DmaCopyLarge16(3, sBerryBlenderGfxBuffer, (void *)(VRAM + 0x0), 0x2000, 0x1000);
        sBerryBlender->loadGfxState++;
        break;
    case 3:
        LZDecompressWram(gBerryBlenderOuter_Gfx, sBerryBlenderGfxBuffer);
        sBerryBlender->loadGfxState++;
        break;
    case 4:
        LZDecompressWram(gBerryBlenderOuter_Tilemap, sBerryBlenderGfxBuffer + 0x3000);
        sBerryBlender->loadGfxState++;
        break;
    case 5:
        DmaCopy16Defvars(3, sBerryBlenderGfxBuffer + 0x0000, (void *)(VRAM + 0xE000), 0x1000);
        sBerryBlender->loadGfxState++;
        break;
    case 6:
		DmaCopy16Defvars(3, sBerryBlenderGfxBuffer + 0x1000, (void *)(VRAM + 0xF000), 0x1000);
        sBerryBlender->loadGfxState++;
        break;
    case 7:
        {
            u16 i;
            u16* palStore = (u16*)(sBerryBlenderGfxBuffer + 0x3000);

            for (i = 0; i < 640; i++)
            {
                *(palStore + i) |= 0x100;
            }
            DmaCopy16Defvars(3, sBerryBlenderGfxBuffer + 0x3000, (void *)(VRAM + 0x6000), 0x500);
            LoadPalette(sBlenderOuter_Pal, 0x80, 0x20);
            sBerryBlender->loadGfxState++;
        }
        break;
    case 8:
        LoadSpriteSheet(&sSpriteSheet_PlayerArrow);
        LoadSpriteSheet(&sSpriteSheet_Particles);
        LoadSpriteSheet(&sSpriteSheet_ScoreSymbols);
        sBerryBlender->loadGfxState++;
        break;
    case 9:
        LoadSpriteSheet(&sSpriteSheet_CountdownNumbers);
        LoadSpriteSheet(&sSpriteSheet_Start);
        LoadSpritePalette(&sSpritePal_PlayerArrow);
        LoadSpritePalette(&sSpritePal_BlenderMisc);
        sBerryBlender->loadGfxState = 0;
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
    u8 *gameEndState; // This temporary is needed to match.

    sBerryBlender = sBerryBlenderSharedMem;

    gameEndState = &sBerryBlender->gameEndState;
    sBerryBlender->mainState = 0;
    *gameEndState = 0;

    InitLocalPlayers(gSpecialVar_0x8004);
    SetMainCallback2(CB2_LoadBerryBlender);
}

static void CB2_LoadBerryBlender(void)
{
    s32 i;
    switch (sBerryBlender->mainState)
    {
    case 0:
        REG_DISPCNT = 0;
        ResetSpriteData();
        FreeAllSpritePalettes();
        SetVBlankCallback(NULL);
        Text_LoadWindowTemplate(&gBerryBlenderWindowTemplate);
        InitMenuWindow(&gBerryBlenderWindowTemplate);
        sBerryBlender->mainState++;
        sBerryBlender->maxProgressBarValue = 0;
        sBerryBlender->progressBarValue = 0;
        sBerryBlender->centerScale = 0x50;
        sBerryBlender->bg_X = 0;
        sBerryBlender->bg_Y = 0;
        sBerryBlender->loadGfxState = 0;
        UpdateBlenderCenter();
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
            {
                sBerryBlender->playerArrowSpriteIds[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
                StartSpriteAnim(&gSprites[sBerryBlender->playerArrowSpriteIds[i]], i + 8);
            }
            SetVBlankCallback(VBlankCB0_BerryBlender);
            sBerryBlender->mainState++;
        }
        break;
    case 2:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        UpdateBlenderCenter();
        sBerryBlender->mainState++;
        break;
    case 3:
        DrawBlenderBg();
        if (!gPaletteFade.active)
        {
            sBerryBlender->mainState++;
        }
        break;
    case 4:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        MenuPrintMessage(gOtherText_BlenderChooseBerry, 1, 15);
        sBerryBlender->mainState++;
        break;
    case 5:
        if (Menu_UpdateWindowText())
        {
            sBerryBlender->mainState++;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
        break;
    case 6:
        if (!gPaletteFade.active)
        {
            ChooseBerryForBlender();
            sBerryBlender->mainState = 0;
        }
        break;
    }
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void SpriteCB_Berry(struct Sprite* sprite)
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

void SetBerrySpriteData(struct Sprite* sprite, s16 x, s16 y, s16 bounceSpeed, s16 xSpeed, s16 ySpeed)
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
    berry->flavors[FLAVOUR_SPICY] = berryInfo->spicy;
    berry->flavors[FLAVOUR_DRY] = berryInfo->dry;
    berry->flavors[FLAVOUR_SWEET] = berryInfo->sweet;
    berry->flavors[FLAVOUR_BITTER] = berryInfo->bitter;
    berry->flavors[FLAVOUR_SOUR] = berryInfo->sour;
    berry->smoothness = berryInfo->smoothness;
}

static void InitLocalPlayers(u8 numOpponents)
{
    int i;
    if (numOpponents)
    {
        for (i = 0; i < 4; i++)
            gLinkPlayers[i].language = GAME_LANGUAGE;
    }
    switch (numOpponents)
    {
    case 0:
        gInGameOpponentsNo = 0;
        break;
    case 1:
        gInGameOpponentsNo = 1;
        sBerryBlender->numPlayers = 2;
        StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
        StringCopy(gLinkPlayers[1].name, sBlenderOpponentsNames[0]);
        break;
    case 2:
        gInGameOpponentsNo = 2;
        sBerryBlender->numPlayers = 3;
        StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
        StringCopy(gLinkPlayers[1].name, sBlenderOpponentsNames[0]);
        StringCopy(gLinkPlayers[2].name, sBlenderOpponentsNames[1]);
        break;
    case 3:
        gInGameOpponentsNo = 3;
        sBerryBlender->numPlayers = 4;
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
    sBerryBlender = sBerryBlenderSharedMem;
    sBerryBlender->mainState = 0;
    sBerryBlender->unused_134 = 0;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        sBerryBlender->chosenItemId[i] = 0;
    }
    InitLocalPlayers(gSpecialVar_0x8004);
    if (gSpecialVar_0x8004 == 0)
        SetMainCallback2(CB2_StartBlenderLink);
    else
        SetMainCallback2(CB2_StartBlenderLocal);
}

static void CB2_StartBlenderLink(void)
{
    int i, j;
    switch (sBerryBlender->mainState)
    {
    case 0:
        ResetSpriteData();
        FreeAllSpritePalettes();
        ResetTasks();
        SetVBlankCallback(VBlankCB0_BerryBlender);
        Text_LoadWindowTemplate(&gBerryBlenderWindowTemplate);
        InitMenuWindow(&gBerryBlenderWindowTemplate);
        gLinkType = LINKTYPE_BERRY_BLENDER;
        sBerryBlender->mainState++;
        sBerryBlender->unused_4E = 0;
        sBerryBlender->slowdownTimer = 0;
        sBerryBlender->bg_X = 0;
        sBerryBlender->bg_Y = 0;
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            sBerryBlender->playerContinueResponses[i] = 0;
            for (j = 0; j < 3; j++)
            {
                sBerryBlender->scores[i][j] = 0;
            }
        }
        sBerryBlender->playAgainState = 0;
        sBerryBlender->speed = 0;
        sBerryBlender->arrowPos = 0;
        sBerryBlender->maxRPM = 0;
        sBerryBlender->loadGfxState = 0;
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            sBerryBlender->mainState++;
            UpdateBlenderCenter();
        }
        break;
    case 2:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            sBerryBlender->playerArrowSpriteIds2[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
            StartSpriteAnim(&gSprites[sBerryBlender->playerArrowSpriteIds2[i]], i + 8);
        }
        sBerryBlender->mainState++;
        break;
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        sBerryBlender->mainState++;
        break;
    case 4:
        DrawBlenderBg();
        if (!gPaletteFade.active)
        {
            sBerryBlender->mainState++;
        }
        break;
    case 5:
        Menu_DrawStdWindowFrame(0, 13, 29, 19);
        Menu_PrintText(gOtherText_LinkStandby3, 1, 14);
        sBerryBlender->mainState = 8;
        sBerryBlender->framesToWait = 0;
        break;
    case 8:
        sBerryBlender->mainState++;
        sBerryBlender->playerToThrowBerry = 0;
        ConvertItemToBlenderBerry(&sBerryBlender->blendedBerries[0], gSpecialVar_ItemId);
        memcpy(gBlockSendBuffer, &sBerryBlender->blendedBerries[0], sizeof(struct BlenderBerry));
        SetLinkStandbyCallback();
        sBerryBlender->framesToWait = 0;
        break;
    case 9:
        if (IsLinkTaskFinished())
        {
            ResetBlockReceivedFlags();
            if (GetMultiplayerId() == 0)
                SendBlockRequest(BLOCK_REQ_SIZE_40);
            sBerryBlender->mainState++;
        }
        break;
    case 10:
        if (++sBerryBlender->framesToWait > 20)
        {
            Menu_EraseScreen();
            if (GetBlockReceivedStatus() == GetSavedLinkPlayerCountAsBitFlags())
            {
                for (i = 0; i < GetLinkPlayerCount(); i++)
                {
                    memcpy(&sBerryBlender->blendedBerries[i], &gBlockRecvBuffer[i][0], sizeof(struct BlenderBerry));
                    sBerryBlender->chosenItemId[i] = sBerryBlender->blendedBerries[i].itemId;
                }
                ResetBlockReceivedFlags();
                sBerryBlender->mainState++;
            }
        }
        break;
    case 11:
        sBerryBlender->numPlayers = GetLinkPlayerCount();
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (sBerryBlender->playerToThrowBerry == sPlayerIdMap[sBerryBlender->numPlayers - 2][i])
            {
                CreateBerrySprite(sBerryBlender->chosenItemId[sBerryBlender->playerToThrowBerry], i);
                break;
            }
        }
        sBerryBlender->framesToWait = 0;
        sBerryBlender->mainState++;
        sBerryBlender->playerToThrowBerry++;
        break;
    case 12:
        if (++sBerryBlender->framesToWait > 60)
        {
            if (sBerryBlender->playerToThrowBerry >= sBerryBlender->numPlayers)
            {
                sBerryBlender->mainState++;
                sBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[sBerryBlender->numPlayers - 2]] - 22528;
            }
            else
                sBerryBlender->mainState--;
            sBerryBlender->framesToWait = 0;
        }
        break;
    case 13:
        if (IsLinkTaskFinished())
        {
            sBerryBlender->mainState++;
            DrawBlenderCenter(&sBerryBlender->bgAffineDst);
        }
        break;
    case 14:
        REG_DISPCNT |= 0x400;
        sBerryBlender->arrowPos += 0x200;
        sBerryBlender->centerScale += 4;
        if (sBerryBlender->centerScale > 255)
        {
            sBerryBlender->mainState++;
            sBerryBlender->centerScale = 256;
            sBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[sBerryBlender->numPlayers - 2]];
            REG_BG2CNT = 0x4882;
            sBerryBlender->framesToWait = 0;
            SetPlayerIdMaps();
            PrintPlayerNames();
        }
        DrawBlenderCenter(&sBerryBlender->bgAffineDst);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            sBerryBlender->framesToWait = 0;
            sBerryBlender->mainState++;
        }
        DrawBlenderCenter(&sBerryBlender->bgAffineDst);
        break;
    case 16:
        CreateSprite(&sSpriteTemplate_CountdownNumbers, 120, -16, 3);
        sBerryBlender->mainState++;
        break;
    case 18:
        sBerryBlender->mainState++;
        break;
    case 19:
        SetLinkStandbyCallback();
        sBerryBlender->mainState++;
        break;
    case 20:
        if (IsLinkTaskFinished())
        {
            SetBerryBlenderLinkCallback();
            sBerryBlender->mainState++;
        }
        break;
    case 21:
        sBerryBlender->speed = 128;
        sBerryBlender->gameFrameTime = 0;
        SetMainCallback2(CB2_PlayBlender);
        if (GetCurrentMapMusic() != 403)
        {
            sBerryBlender->savedMusic = GetCurrentMapMusic();
        }
        PlayBGM(MUS_CYCLING);
        break;
    case 100:
        Menu_DrawStdWindowFrame(0, 13, 29, 19);
        MenuPrintMessage(gOtherText_LinkNotFound, 1, 15);
        sBerryBlender->mainState++;
        break;
    case 101:
        if (Menu_UpdateWindowText())
            sBerryBlender->mainState++;
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

    SetVBlankCallback(VBlankCB0_BerryBlender);

    Text_LoadWindowTemplate(&gBerryBlenderWindowTemplate);
    InitMenuWindow(&gBerryBlenderWindowTemplate);

    gLinkType = LINKTYPE_BERRY_BLENDER;

    sBerryBlender->unused_4E = 0;
    sBerryBlender->speed = 0;
    sBerryBlender->arrowPos = 0;
    sBerryBlender->maxRPM = 0;
    sBerryBlender->bg_X = 0;
    sBerryBlender->bg_Y = 0;
    sBerryBlender->mainState++;
}

static u8 GetArrowProximity(u16 arrowPos, u8 playerId)
{
    u32 pos = (arrowPos / 256) + 24;
    u8 arrowId = sBerryBlender->playerIdToArrowId[playerId];
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
        sBerryBlender->playerIdToArrowId[i] = 0xFF;
        sBerryBlender->arrowIdToPlayerId[i] = sPlayerIdMap[sBerryBlender->numPlayers - 2][i];
    }
    for (j = 0; j < BLENDER_MAX_PLAYERS; j++)
    {
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (sBerryBlender->arrowIdToPlayerId[i] == j)
                sBerryBlender->playerIdToArrowId[j] = i;
        }
    }
}

static void PrintPlayerNames(void)
{
    int i;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        if (sBerryBlender->arrowIdToPlayerId[i] != 0xFF)
        {
            u8* stringPtr = gStringVar1;

            sBerryBlender->playerArrowSpriteIds[sBerryBlender->arrowIdToPlayerId[i]] = sBerryBlender->playerArrowSpriteIds2[i];
            StartSpriteAnim(&gSprites[sBerryBlender->playerArrowSpriteIds[sBerryBlender->arrowIdToPlayerId[i]]], i);
            if (GetMultiplayerId() == sBerryBlender->arrowIdToPlayerId[i])
                stringPtr = StringCopy(stringPtr, sRedColorString);
            StringCopy(stringPtr, gLinkPlayers[sBerryBlender->arrowIdToPlayerId[i]].name);
            Menu_PrintTextPixelCoords(gStringVar1, sPlayerNameTextCoords[i][0] * 8 + 1, sPlayerNameTextCoords[i][1] * 8, 1);
        }
    }
}

static void CB2_StartBlenderLocal(void)
{
    s32 i, j;
    switch (sBerryBlender->mainState)
    {
    case 0:
        InitBlenderBgs();
        SetPlayerBerryData(0, gSpecialVar_ItemId);
        ConvertItemToBlenderBerry(&sBerryBlender->blendedBerries[0], gSpecialVar_ItemId);
        SetOpponentsBerryData(gSpecialVar_ItemId, sBerryBlender->numPlayers, &sBerryBlender->blendedBerries[0]);
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            sBerryBlender->playerContinueResponses[i] = 0;
            for (j = 0; j < 3; j++)
            {
                sBerryBlender->scores[i][j] = 0;
            }
        }
        sBerryBlender->playAgainState = 0;
        sBerryBlender->loadGfxState = 0;
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            sBerryBlender->mainState++;
            UpdateBlenderCenter();
        }
        break;
    case 2:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            sBerryBlender->playerArrowSpriteIds2[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
            StartSpriteAnim(&gSprites[sBerryBlender->playerArrowSpriteIds2[i]], i + 8);
        }
        sBerryBlender->mainState++;
        break;
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        sBerryBlender->mainState++;
        sBerryBlender->framesToWait = 0;
        break;
    case 4:
        if (++sBerryBlender->framesToWait == 2)
            DrawBlenderBg();
        if (!gPaletteFade.active)
            sBerryBlender->mainState = 8;
        break;
    case 8:
        sBerryBlender->mainState = 11;
        sBerryBlender->playerToThrowBerry = 0;
        break;
    case 11:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            u32 playerId = sPlayerIdMap[sBerryBlender->numPlayers - 2][i];
            if (sBerryBlender->playerToThrowBerry == playerId)
            {
                CreateBerrySprite(sBerryBlender->chosenItemId[sBerryBlender->playerToThrowBerry], i);
                break;
            }
        }
        sBerryBlender->framesToWait = 0;
        sBerryBlender->mainState++;
        sBerryBlender->playerToThrowBerry++;
        break;
    case 12:
        if (++sBerryBlender->framesToWait > 60)
        {
            if (sBerryBlender->playerToThrowBerry >= sBerryBlender->numPlayers)
            {
                sBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[sBerryBlender->numPlayers - 2]] - 22528;
                sBerryBlender->mainState++;
            }
            else
                sBerryBlender->mainState--;
            sBerryBlender->framesToWait = 0;
        }
        break;
    case 13:
        sBerryBlender->mainState++;
        SetPlayerIdMaps();
        PlaySE(SE_FALL);
        DrawBlenderCenter(&sBerryBlender->bgAffineDst);
        break;
    case 14:
        REG_DISPCNT |= 0x400;
        sBerryBlender->arrowPos += 0x200;
        sBerryBlender->centerScale += 4;
        if (sBerryBlender->centerScale > 255)
        {
            sBerryBlender->mainState++;
            sBerryBlender->centerScale = 256;
            sBerryBlender->arrowPos = sArrowStartPos[sArrowStartPosIds[sBerryBlender->numPlayers - 2]];
            REG_BG2CNT = 0x4882;
            sBerryBlender->framesToWait = 0;
            PlaySE(SE_TRUCK_DOOR);;
            PrintPlayerNames();
        }
        DrawBlenderCenter(&sBerryBlender->bgAffineDst);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            sBerryBlender->mainState++;
        }
        DrawBlenderCenter(&sBerryBlender->bgAffineDst);
        break;
    case 16:
        CreateSprite(&sSpriteTemplate_CountdownNumbers, 120, -16, 3);
        sBerryBlender->mainState++;
        break;
    case 18:
        sBerryBlender->mainState++;
        break;
    case 19:
        sBerryBlender->mainState++;
        break;
    case 20:
        sBerryBlender->mainState++;
        break;
    case 21:
        ResetLinkCmds();
        sBerryBlender->speed = 128;
        sBerryBlender->gameFrameTime = 0;
        sBerryBlender->perfectOpponents = 0;
        sBerryBlender->slowdownTimer = 0;
        SetMainCallback2(CB2_PlayBlender);

        for (i = 0; i < gSpecialVar_0x8004; i++)
        {
            sBerryBlender->opponentTaskIds[i] = CreateTask(sLocalOpponentTasks[i], 10 + i);
        }

        if (GetCurrentMapMusic() != 403)
        {
            sBerryBlender->savedMusic = GetCurrentMapMusic();
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
    if (GetArrowProximity(sBerryBlender->arrowPos, 1) == 2)
    {
        if (gTasks[taskId].data[0] == 0)
        {
            if (sBerryBlender->perfectOpponents == 0)
            {
                u8 rand = Random() / 655;
                if (sBerryBlender->speed < 500)
                {
                    if (rand > 75)
                        gRecvCmds[2][1] = 0x4523;
                    else
                        gRecvCmds[2][1] = 0x5432;
                    gRecvCmds[2][1] = 0x5432; // ???
                }
                else if (sBerryBlender->speed < 1500)
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
    u32 var1 = (sBerryBlender->arrowPos + 0x1800) & 0xFFFF;
    u32 arrowId = sBerryBlender->playerIdToArrowId[2] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[arrowId] + 20 && (var1 >> 8) < sArrowHitRangeStart[arrowId] + 40)
    {
        if (gTasks[taskId].data[0] == 0)
        {
            if (sBerryBlender->perfectOpponents == 0)
            {
                u8 rand = Random() / 655;
                if (sBerryBlender->speed < 500)
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

    var1 = (sBerryBlender->arrowPos + 0x1800) & 0xFFFF;
    arrowId = sBerryBlender->playerIdToArrowId[3] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[arrowId] + 20 && (var1 >> 8) < sArrowHitRangeStart[arrowId] + 40)
    {
        if (gTasks[taskId].data[0] == 0)
        {
            if (sBerryBlender->perfectOpponents == 0)
            {
                u8 rand = (Random() / 655);
                if (sBerryBlender->speed < 500)
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
        if (sBerryBlender->speed < 1500)
            sBerryBlender->speed += (384 / sBerryMasterBerries[sBerryBlender->numPlayers]);
        else
        {
            sBerryBlender->speed += (128 / sBerryMasterBerries[sBerryBlender->numPlayers]);
            ShakeBgCoordForHit(&sBerryBlender->bg_X, (sBerryBlender->speed / 100) - 10);
            ShakeBgCoordForHit(&sBerryBlender->bg_Y, (sBerryBlender->speed / 100) - 10);
        }
        break;
    case 0x5432:
        if (sBerryBlender->speed < 1500)
            sBerryBlender->speed += (256 / sBerryMasterBerries[sBerryBlender->numPlayers]);
        break;
    case 0x2345:
        sBerryBlender->speed -= (256 / sBerryMasterBerries[sBerryBlender->numPlayers]);
        if (sBerryBlender->speed < 128)
            sBerryBlender->speed = 128;
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
    for (i = 0; i < sBerryBlender->numPlayers; i++)
    {
        if (gRecvCmds[0][i] == 0x4444)
        {
            u32 arrowId = sBerryBlender->playerIdToArrowId[i];
            if (gRecvCmds[2][i] == 0x4523)
            {
                UpdateSpeedFromHit(0x4523);
                sBerryBlender->progressBarValue += (sBerryBlender->speed / 55);
                if (sBerryBlender->progressBarValue >= 1000)
                    sBerryBlender->progressBarValue = 1000;
                CreateScoreSymbolSprite(0x4523, arrowId);
                sBerryBlender->scores[i][BLENDER_SCORE_BEST]++;
            }
            else if (gRecvCmds[2][i] == 0x5432)
            {
                UpdateSpeedFromHit(0x5432);
                sBerryBlender->progressBarValue += (sBerryBlender->speed / 70);
                CreateScoreSymbolSprite(0x5432, arrowId);
                sBerryBlender->scores[i][BLENDER_SCORE_GOOD]++;
            }
            else if (gRecvCmds[2][i] == 0x2345)
            {
                CreateScoreSymbolSprite(0x2345, arrowId);
                UpdateSpeedFromHit(0x2345);
                // looks like a bug
                if (sBerryBlender->menuWindow.win_field_F > 1000)
                    sBerryBlender->progressBarValue = 1000;
                if (sBerryBlender->scores[i][BLENDER_SCORE_MISS] < 999)
                    sBerryBlender->scores[i][BLENDER_SCORE_MISS]++;
            }
            if (gRecvCmds[2][i] == 0x2345 || gRecvCmds[2][i] == 0x4523 || gRecvCmds[2][i] == 0x5432)
            {
                if (sBerryBlender->speed > 1500)
                    m4aMPlayTempoControl(&gMPlayInfo_BGM, ((sBerryBlender->speed - 750) / 20) + 256);
                else
                    m4aMPlayTempoControl(&gMPlayInfo_BGM, 256);
            }
        }
    }
    if (gSpecialVar_0x8004 != 0)
    {
        for (i = 0; i < sBerryBlender->numPlayers; i++)
        {
            gRecvCmds[0][i] = 0;
            gRecvCmds[2][i] = 0;
        }
    }
}

static void HandlePlayerInput(void)
{
    bool8 pressedA = 0;
    u8 arrowId = sBerryBlender->playerIdToArrowId[GetMultiplayerId()];
    if (sBerryBlender->gameEndState == 0)
    {
        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A && JOY_NEW(A_BUTTON))
            pressedA = ((gMain.heldKeysRaw & (A_BUTTON | L_BUTTON)) != (A_BUTTON | L_BUTTON));
        else if (JOY_NEW(A_BUTTON))
            pressedA = 1;
        if (pressedA)
        {
            u8 proximity;
            StartSpriteAnim(&gSprites[sBerryBlender->playerArrowSpriteIds[sBerryBlender->arrowIdToPlayerId[arrowId]]], arrowId + 4);
            proximity = GetArrowProximity(sBerryBlender->arrowPos, GetMultiplayerId());
            if (proximity == 2)
                gSendCmd[2] = 0x4523;
            else if (proximity == 1)
                gSendCmd[2] = 0x5432;
            else
                gSendCmd[2] = 0x2345;
        }
    }
    if (++sBerryBlender->slowdownTimer > 5)
    {
        if (sBerryBlender->speed > 128)
            sBerryBlender->speed--;
        sBerryBlender->slowdownTimer = 0;
    }
    if (gIsDebugMode && JOY_NEW(L_BUTTON))
        sBerryBlender->perfectOpponents ^= 1;
}

static void CB2_PlayBlender(void)
{
    UpdateBlenderCenter();
    if (sBerryBlender->gameFrameTime < (99 * 60 * 60) + (59 * 60)) // game time can't be longer than 99 minutes and 59 seconds, can't print 3 digits
        sBerryBlender->gameFrameTime++;
    HandlePlayerInput();
    SetLinkDebugValues((u16)(sBerryBlender->speed), sBerryBlender->progressBarValue);
    UpdateOpponentScores();
    TryUpdateProgressBar(sBerryBlender->progressBarValue, 1000);
    UpdateRPM(sBerryBlender->speed);
    RestoreBgCoords();
    ProcessLinkPlayerCmds();
    if (sBerryBlender->gameEndState == 0 && sBerryBlender->maxProgressBarValue >= 1000)
    {
        sBerryBlender->progressBarValue = 1000;
        sBerryBlender->gameEndState = 1;
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

s16 Debug_GetMaxRPMStage(void)
{
    return sDebug_MaxRPMStage;
}

static void Debug_SetGameTimeStage(s16 value)
{
    sDebug_GameTimeStage = value;
}

s16 Debug_GetGameTimeStage(void)
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

/*static*/ void Debug_SetStageVars(void)
{
    u32 frames = (u16)(sBerryBlender->gameFrameTime);
    u16 maxRPM = sBerryBlender->maxRPM;
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

static void SetBlenderContinueMenuCursor(u8 value)
{
    sBerryBlender->yesNoAnswer = value;
    SetOutlineCursorPosition(192, (sBerryBlender->yesNoAnswer * 16) + 72);
}

static void ShowBlenderContinueMenu(void)
{
    sBerryBlender->yesNoAnswer = 0;
    Menu_DrawStdWindowFrame(23, 8, 28, 13);
    CreateOutlineCursor(0, -1, 12, 0x2D9F, 32);
    Menu_PrintText(gOtherText_YesNoTerminating, 24, 9);
    SetBlenderContinueMenuCursor(sBerryBlender->yesNoAnswer);
}

static void CB2_EndBlenderGame(void)
{
    u8 i;
    u8 multiplayerID; // unused

    UpdateBlenderCenter();
    multiplayerID = GetMultiplayerId();
    switch (sBerryBlender->gameEndState)
    {
    case 1:
        ClearLinkCallback();
        m4aMPlayTempoControl(&gMPlayInfo_BGM, 256);
        for (i = 0; i < gSpecialVar_0x8004; i++)
        {
            DestroyTask(sBerryBlender->opponentTaskIds[i]);
        }
        sBerryBlender->gameEndState++;
        break;
    case 2:
        sBerryBlender->speed -= 32;
        if (sBerryBlender->speed <= 0)
        {
            sBerryBlender->speed = 0;
            if (gReceivedRemoteLinkPlayers != 0)
                sBerryBlender->gameEndState++;
            else
                sBerryBlender->gameEndState = 5;
            sBerryBlender->mainState = 0;
            m4aMPlayStop(&gMPlayInfo_SE2);
        }
        UpdateHitPitch();
        break;
    case 3:
        if (/*multiplayerID != 0*/ GetMultiplayerId() != 0)
            sBerryBlender->gameEndState++;
        else if (IsLinkTaskFinished())
        {
            sBerryBlender->smallBlock.time = sBerryBlender->gameFrameTime;
            sBerryBlender->smallBlock.maxRPM = sBerryBlender->maxRPM;
            SendBlock(0, &sBerryBlender->smallBlock.time, 40);
            sBerryBlender->gameEndState++;
        }
        break;
    case 4:
        if (GetBlockReceivedStatus())
        {
            u32* ptr = ((u32*)(&gBlockRecvBuffer[0][0]));
            sBerryBlender->maxRPM = gBlockRecvBuffer[0][2];
            sBerryBlender->gameFrameTime = *ptr;
            sBerryBlender->gameEndState++;
            ResetBlockReceivedFlags();
        }
        break;
    case 5:
        if (PrintBlendingRanking())
            sBerryBlender->gameEndState++;
        break;
    case 6:
        if (PrintBlendingResults())
        {
            if (gInGameOpponentsNo == 0)
                IncrementGameStat(GAME_STAT_POKEBLOCKS_WITH_FRIENDS);
            else
                IncrementGameStat(GAME_STAT_POKEBLOCKS);
            sBerryBlender->gameEndState++;
        }
        break;
    case 7:
        sBerryBlender->gameEndState++;
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        MenuPrintMessage(gOtherText_BlendAnotherBerryPrompt, 1, 15);
        break;
    case 8:
        if (Menu_UpdateWindowText())
            sBerryBlender->gameEndState++;
        break;
    case 9:
        sBerryBlender->yesNoAnswer = 0;
        ShowBlenderContinueMenu();
        sBerryBlender->gameEndState++;
        break;
    case 10:
        if (JOY_NEW(DPAD_UP))
        {
            if (sBerryBlender->yesNoAnswer != 0)
                PlaySE(SE_SELECT);
            SetBlenderContinueMenuCursor(0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            if (sBerryBlender->yesNoAnswer != 1)
                PlaySE(SE_SELECT);
            SetBlenderContinueMenuCursor(1);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sBerryBlender->gameEndState++;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            sBerryBlender->gameEndState++;
            SetBlenderContinueMenuCursor(1);
        }
        break;
    case 11:
        gSendCmd[0] = 0x2FFF;
        if (sBerryBlender->yesNoAnswer == 0)
        {
            if (IsBagPocketNonEmpty(BAG_BERRIES) == FALSE) // is empty
            {
                sBerryBlender->playAgainState = 2;
                gSendCmd[1] = 0x9999;
            }
            else if (GetFirstFreePokeblockSlot() == -1)
            {
                sBerryBlender->playAgainState = 3;
                gSendCmd[1] = 0xAAAA;
            }
            else
            {
                sBerryBlender->playAgainState = 0;
                gSendCmd[1] = 0x7779;
            }
            sBerryBlender->gameEndState++;
        }
        else
        {
            sBerryBlender->playAgainState = 1;
            gSendCmd[1] = 0x8888;
            sBerryBlender->gameEndState++;
        }
        break;
    case 12:
        if (gInGameOpponentsNo)
        {
            SetMainCallback2(CB2_CheckPlayAgainLocal);
            sBerryBlender->gameEndState = 0;
            sBerryBlender->mainState = 0;
        }
        else
        {
            MenuPrintMessage(gOtherText_LinkStandby3, 1, 15);
            sBerryBlender->gameEndState++;
        }
        break;
    case 13:
        if (Menu_UpdateWindowText())
        {
            SetMainCallback2(CB2_CheckPlayAgainLink);
            sBerryBlender->gameEndState = 0;
            sBerryBlender->mainState = 0;
        }
        break;
    }
    RestoreBgCoords();
    UpdateRPM(sBerryBlender->speed);
    ProcessLinkPlayerCmds();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

bool8 LinkPlayAgainHandleSaving(void)
{
    switch (sBerryBlender->linkPlayAgainState)
    {
    case 0:
        SetLinkStandbyCallback();
        sBerryBlender->linkPlayAgainState = 1;
        sBerryBlender->framesToWait = 0;
        break;
    case 1:
        if (IsLinkTaskFinished())
        {
            sBerryBlender->linkPlayAgainState++;
            gSoftResetDisabled = TRUE;
        }
        break;
    case 2:
        WriteSaveBlock2();
        sBerryBlender->linkPlayAgainState++;
        sBerryBlender->framesToWait = 0;
        break;
    case 3:
        if (++sBerryBlender->framesToWait == 10)
        {
            SetLinkStandbyCallback();
            sBerryBlender->linkPlayAgainState++;
        }
        break;
    case 4:
        if (IsLinkTaskFinished())
        {
            if (WriteSaveBlock1Sector())
                sBerryBlender->linkPlayAgainState = 5;
            else
            {
                sBerryBlender->framesToWait = 0;
                sBerryBlender->linkPlayAgainState = 3;
            }
        }
        break;
    case 5:
        sBerryBlender->linkPlayAgainState++;
        sBerryBlender->framesToWait = 0;
        break;
    case 6:
        if (++sBerryBlender->framesToWait > 5)
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
    switch (sBerryBlender->gameEndState)
    {
    case 0:
        if (sBerryBlender->playerContinueResponses[0] == 0x2222)
            sBerryBlender->gameEndState = 5;
        else if (sBerryBlender->playerContinueResponses[0] == 0x1111)
        {
            if (sBerryBlender->canceledPlayerCmd == 0x9999)
                sBerryBlender->gameEndState = 2;
            else if (sBerryBlender->canceledPlayerCmd == 0xAAAA)
                sBerryBlender->gameEndState = 1;
            else
                sBerryBlender->gameEndState = 5;
        }
        break;
    case 1:
        sBerryBlender->gameEndState = 3;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
#ifdef ENGLISH
        StringCopy(gStringVar4, gLinkPlayers[sBerryBlender->canceledPlayerId].name);
        StringAppend(gStringVar4, gOtherText_OtherCaseIsFull);
#else
        StringCopy(gStringVar4, gOtherText_OtherCaseIsFull);
        GermanStringReplaceNamePlaceholder(gStringVar4, gLinkPlayers[sBerryBlender->canceledPlayerId].name);
#endif
        MenuPrintMessage(gStringVar4, 1, 15);
        break;
    case 2:
        sBerryBlender->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
#ifdef ENGLISH
        StringCopy(gStringVar4, gLinkPlayers[sBerryBlender->canceledPlayerId].name);
        StringAppend(gStringVar4, gOtherText_NoBerriesForBlend);
#else
        StringCopy(gStringVar4, gOtherText_NoBerriesForBlend);
        GermanStringReplaceNamePlaceholder(gStringVar4, gLinkPlayers[sBerryBlender->canceledPlayerId].name);
#endif
        MenuPrintMessage(gStringVar4, 1, 15);
        break;
    case 3:
        if (Menu_UpdateWindowText())
        {
            sBerryBlender->framesToWait = 0;
            sBerryBlender->gameEndState++;
        }
        break;
    case 4:
        if (++sBerryBlender->framesToWait > 60)
            sBerryBlender->gameEndState = 5;
        break;
    case 5:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(gMultiText_Saving, 2, 15);
        SetLinkStandbyCallback();
        sBerryBlender->gameEndState++;
        break;
    case 6:
        if (IsLinkTaskFinished())
        {
            sBerryBlender->framesToWait = 0;
            sBerryBlender->gameEndState++;
            sBerryBlender->linkPlayAgainState = 0;
        }
        break;
    case 7:
        if (LinkPlayAgainHandleSaving())
        {
            PlaySE(SE_SAVE);
            sBerryBlender->gameEndState++;
        }
        break;
    case 8:
        sBerryBlender->gameEndState++;
        SetLinkStandbyCallback();
        break;
    case 9:
        if (IsLinkTaskFinished())
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            sBerryBlender->gameEndState++;
        }
        break;
    case 10:
        if (!gPaletteFade.active)
        {
            if (sBerryBlender->playerContinueResponses[0] == 0x2222)
                SetMainCallback2(DoBerryBlending);
            else
            {
                sBerryBlender->framesToWait = 0;
                sBerryBlender->gameEndState++;
            }
        }
        break;
    case 11:
        if (++sBerryBlender->framesToWait > 30)
        {
            SetCloseLinkCallback();
            sBerryBlender->gameEndState++;
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
    switch (sBerryBlender->gameEndState)
    {
    case 0:
        if (sBerryBlender->playAgainState < 2)
            sBerryBlender->gameEndState = 9;
        if (sBerryBlender->playAgainState == 2)
            sBerryBlender->gameEndState = 2;
        if (sBerryBlender->playAgainState == 3)
            sBerryBlender->gameEndState =1;
        break;
    case 1:
        sBerryBlender->gameEndState = 3;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
        MenuPrintMessage(gOtherText_CaseIsFull, 1, 15);
        break;
    case 2:
        sBerryBlender->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
        MenuPrintMessage(gOtherText_OutOfBerries, 1, 15);
        break;
    case 3:
        if (Menu_UpdateWindowText())
            sBerryBlender->gameEndState = 9;
        break;
    case 9:
        BeginFastPaletteFade(3);
        sBerryBlender->gameEndState++;
        break;
    case 10:
        if (!gPaletteFade.active)
        {
            if (sBerryBlender->playAgainState == 0)
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
                    sBerryBlender->canceledPlayerCmd = 0x8888;
                    sBerryBlender->canceledPlayerId = gRecvCmds[3][0];
                    break;
                case 0x9999:
                    sBerryBlender->canceledPlayerCmd = 0x9999;
                    sBerryBlender->canceledPlayerId = gRecvCmds[3][0];
                    break;
                case 0xAAAA:
                    sBerryBlender->canceledPlayerCmd = 0xAAAA;
                    sBerryBlender->canceledPlayerId = gRecvCmds[3][0];
                    break;
                }
                sBerryBlender->playerContinueResponses[0] = 0x1111;
            }
            else if (gRecvCmds[1][0] == 0x2222)
                sBerryBlender->playerContinueResponses[0] = 0x2222;
        }
        if (GetMultiplayerId() == 0 && sBerryBlender->playerContinueResponses[0] != 0x1111 && sBerryBlender->playerContinueResponses[0] != 0x2222)
        {
            u8 i;
            for (i = 0; i < GetLinkPlayerCount(); i++)
            {
                if (gRecvCmds[0][i] == 0x2FFF)
                {
                    switch (gRecvCmds[1][i])
                    {
                    case 0x8888:
                        sBerryBlender->playerContinueResponses[i] = 0x8888;
                        break;
                    case 0x7779:
                        sBerryBlender->playerContinueResponses[i] = 0x7779;
                        break;
                    case 0x9999:
                        sBerryBlender->playerContinueResponses[i] = 0x9999;
                        break;
                    case 0xAAAA:
                        sBerryBlender->playerContinueResponses[i] = 0xAAAA;
                        break;
                    }
                }
            }
            for (i = 0; i < GetLinkPlayerCount(); i++)
            {
                if (sBerryBlender->playerContinueResponses[i] == 0)
                    break;
            }
            if (i == GetLinkPlayerCount())
            {
                for (i = 0; i < GetLinkPlayerCount(); i++)
                {
                    if (sBerryBlender->playerContinueResponses[i] != 0x7779)
                        break;
                }
                gSendCmd[0] = 0x2FFF;
                if (i == GetLinkPlayerCount())
                    gSendCmd[1] = 0x2222;
                else
                {
                    gSendCmd[1] = 0x1111;
                    gSendCmd[2] = sBerryBlender->playerContinueResponses[i];
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
    affineSrc.scrX = 120 - sBerryBlender->bg_X;
    affineSrc.scrY = 80 - sBerryBlender->bg_Y;
    affineSrc.sx = sBerryBlender->centerScale;
    affineSrc.sy = sBerryBlender->centerScale;
    affineSrc.alpha = sBerryBlender->arrowPos;
    BgAffineSet(&affineSrc, dest, 1);
}

static void UpdateBlenderCenter(void)
{
    sBerryBlender->previousArrowPos = sBerryBlender->arrowPos;
    sBerryBlender->arrowPos += sBerryBlender->speed;
    DrawBlenderCenter(&sBerryBlender->bgAffineDst);
}

static void ApplyBlenderCenterAffine(void)
{
    REG_BG2PA = sBerryBlender->bgAffineDst.pa;
    REG_BG2PB = sBerryBlender->bgAffineDst.pb;
    REG_BG2PC = sBerryBlender->bgAffineDst.pc;
    REG_BG2PD = sBerryBlender->bgAffineDst.pd;
    REG_BG2X = sBerryBlender->bgAffineDst.dx;
    REG_BG2Y = sBerryBlender->bgAffineDst.dy;
}

static void SetBgPos(void)
{
    REG_BG1HOFS = sBerryBlender->bg_X;
    REG_BG1VOFS = sBerryBlender->bg_Y;
    REG_BG0HOFS = sBerryBlender->bg_X;
    REG_BG0VOFS = sBerryBlender->bg_Y;
}

void SpriteCB_Particle(struct Sprite* sprite)
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

        rand = sBerryBlender->arrowPos + (Random() % 20);

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

void SpriteCB_ScoreSymbolBest(struct Sprite* sprite)
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
    sBerryBlender->chosenItemId[playerId] = itemId;
    ConvertItemToBlenderBerry(&sBerryBlender->blendedBerries[playerId], itemId);
}

void UnusedTask_WaitForBlenderLinkPlayers(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    if (gReceivedRemoteLinkPlayers)
    {
        s32 i;
        if (GetMultiplayerId() == 0)
        {
            if (++task->data[0] > 120)
                task->data[0] = 0;
            if (task->data[0] == 100)
            {
                Text_EraseWindowRect(&sBerryBlender->menuWindow, 0, 0, 16, 20);
                Menu_DrawStdWindowFrame(4, 4, 10, 12);
                for (i = 0; i < 3; i++)
                {
                    if (gLinkPlayers[i + 1].trainerId != 0)
                        Menu_PrintText(sDebugText_PlayerCount[i], 5, (2 * i) + 5);
                    Menu_DrawStdWindowFrame(0, 13, 29, 19);
                    Menu_PrintText(gOtherText_PressAToStart, 1, 15);
                }
            }
            if (JOY_NEW(A_BUTTON))
            {
                SendBerryBlenderNoSpaceForPokeblocks();
                DestroyTask(taskId);
            }
        }
        else
        {
            if (task->data[0] == 10)
                Menu_PrintText(gOtherText_PleaseWait, 3, 10);
            if (++task->data[0] > 120)
                task->data[0] = 0;
            if (gLinkDummyCommandReceived > 4 && gReceivedRemoteLinkPlayers == 1)
                DestroyTask(taskId);
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
            sBerryBlender->mainState++;
            DestroySprite(sprite);
        }
        break;
    }
    sprite->y2 = sprite->data[1];
}

static void TryUpdateProgressBar(u16 current, u16 limit)
{
    if (sBerryBlender->maxProgressBarValue < current)
    {
        sBerryBlender->maxProgressBarValue += 2;
        UpdateProgressBar(sBerryBlender->maxProgressBarValue, limit);
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
    if (sBerryBlender->maxRPM < currentRPM)
        sBerryBlender->maxRPM = currentRPM;
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
    RestoreBgCoord(&sBerryBlender->bg_X);
    RestoreBgCoord(&sBerryBlender->bg_Y);
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
    if (sBerryBlender->framesToWait == 0)
    {
        sBerryBlender->bg_X = 0;
        sBerryBlender->bg_Y = 0;
    }
    sBerryBlender->framesToWait++;
    BlenderLandShakeBgCoord(&sBerryBlender->bg_X, sBerryBlender->framesToWait);
    BlenderLandShakeBgCoord(&sBerryBlender->bg_Y, sBerryBlender->framesToWait);
    if (sBerryBlender->framesToWait == 20)
    {
        sBerryBlender->bg_X = 0;
        sBerryBlender->bg_Y = 0;
        return TRUE;
    }
    else
        return FALSE;
}

static void SpriteCB_PlayerArrow(struct Sprite* sprite)
{
   sprite->x2 = -(sBerryBlender->bg_X);
   sprite->y2 = -(sBerryBlender->bg_Y);
}

/*static*/ void TryUpdateBerryBlenderRecord(void)
{
    if (gSaveBlock1.berryBlenderRecords[sBerryBlender->numPlayers - 2] < sBerryBlender->maxRPM)
        gSaveBlock1.berryBlenderRecords[sBerryBlender->numPlayers - 2] = sBerryBlender->maxRPM;
}

bool8 PrintBlendingResults(void)
{
    u16 i;

    struct Pokeblock pokeblock;
    u8 flavors[6];
    u8 text[2][10];
    u16 berryIDs[4]; // unused

    switch (sBerryBlender->mainState)
    {
    case 0:
        sBerryBlender->mainState++;
        sBerryBlender->framesToWait = 17;
        break;
    case 1:
        sBerryBlender->framesToWait -= 10;
        if (sBerryBlender->framesToWait < 0)
        {
            sBerryBlender->framesToWait = 0;
            sBerryBlender->mainState++;
        }
        break;
    case 2:
        if (++sBerryBlender->framesToWait > 20)
        {
            for (i = 0; i < 3; i++)
                DestroySprite(&gSprites[sBerryBlender->scoreIconIds[i]]);
            sBerryBlender->framesToWait = 0;
            sBerryBlender->mainState++;
        }
        break;
    case 3:
        {
            u8* textPtr;
            u16 secondsPassed, minutes, seconds;

            Menu_DrawStdWindowFrame(4, 2, 25, 17);
            MenuPrint_Centered(gOtherText_ResultsOfBlending, 5, 3, 160);
            for (i = 0; i < sBerryBlender->numPlayers; i++)
            {
                u8 place = sBerryBlender->playerPlaces[i];
                textPtr = text[0];

                StringCopy(textPtr, sBerryBlender->blendedBerries[place].name);
                ConvertInternationalString(textPtr, gLinkPlayers[place].language);
#ifdef ENGLISH
                StringAppend(textPtr, gOtherText_Berry);
#else
                GermanStringFormatName(textPtr, gOtherText_Berry);
#endif
                textPtr = sBerryBlender->stringVar;
                textPtr = ConvertIntToDecimalStringSimple(textPtr, i + 1);
                textPtr[0] = CHAR_SPACE;
                textPtr[1] = CHAR_PERIOD;
                textPtr[2] = CHAR_SPACE;
                textPtr += 3;
                textPtr = AlignStringInMenuWindow(textPtr, gLinkPlayers[place].name, 88, 0);
                AlignStringInMenuWindow(textPtr, text[0], 157, 0);
                Menu_PrintText(sBerryBlender->stringVar, 5, sResultsPlayerNameStartY[sBerryBlender->numPlayers] + (i * sResultsPlayerNameSpacingY[sBerryBlender->numPlayers]));
            }
            ConvertIntToDecimalStringN(text[0], sBerryBlender->maxRPM % 100, 2, 2);
            textPtr = sBerryBlender->stringVar;
            textPtr = StringCopy(textPtr, gOtherText_MaxSpeed);
            textPtr = AlignInt1InMenuWindow(textPtr, sBerryBlender->maxRPM / 100, 121, 1);

#ifdef ENGLISH
            textPtr[0] = CHAR_SPACE;
            textPtr[1] = CHAR_PERIOD;
            textPtr[2] = CHAR_SPACE;
            textPtr += 3;
            textPtr = AlignStringInMenuWindow(textPtr, text[0], 142, 1);
#else
            *textPtr++ = CHAR_COMMA;
            textPtr = AlignStringInMenuWindow(textPtr, text[0], 136, 1);
#endif
            StringCopy(textPtr, gOtherText_RPM);
            Menu_PrintText(sBerryBlender->stringVar, 5, 13);

            secondsPassed = sBerryBlender->gameFrameTime / 60;
            seconds = secondsPassed % 60;
            minutes = secondsPassed / 60;
            ConvertIntToDecimalStringN(text[0], minutes, 2, 2);
            ConvertIntToDecimalStringN(text[1], seconds, 2, 2);
            textPtr = sBerryBlender->stringVar;
            textPtr = StringCopy(textPtr, gOtherText_RequiredTime);

#ifdef ENGLISH
            textPtr = AlignStringInMenuWindow(textPtr, text[0], 102, 1);
#else
            textPtr = AlignStringInMenuWindow(textPtr, text[0], 99, 1);
#endif
            textPtr = StringAppend(textPtr, gOtherText_Min);

            textPtr = AlignStringInMenuWindow(textPtr, text[1], 136, 1);
            StringCopy(textPtr, gOtherText_Sec);

            Menu_PrintText(sBerryBlender->stringVar, 5, 15);

            sBerryBlender->framesToWait = 0;
            sBerryBlender->mainState++;
        }
        break;
    case 4:
        if (JOY_NEW(A_BUTTON))
            sBerryBlender->mainState++;
        break;
    case 5:
        Menu_EraseScreen();
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (sBerryBlender->chosenItemId[i] != 0)
                berryIDs[i] = sBerryBlender->chosenItemId[i] - 133;
        }
        Debug_SetStageVars();
        CalculatePokeblock(sBerryBlender->blendedBerries, &pokeblock, sBerryBlender->numPlayers, flavors, sBerryBlender->maxRPM);
        PrintMadePokeblockString(&pokeblock, sBerryBlender->stringVar);
        CreateTask(Task_PlayPokeblockFanfare, 6);
#if DEBUG
        ConvertIntToHexStringN(text[0], GetBerryBlenderKeySendAttempts(), 0, 4);
        StringAppend(text[0], sText_NewParagraph);
        StringAppend(sBerryBlender->stringVar, text[0]);
#endif
        MenuPrintMessage(sBerryBlender->stringVar, 1, 15);
        RemoveBagItem(gSpecialVar_ItemId, 1);
        AddPokeblock(&pokeblock);
        sBerryBlender->mainState++;
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
    StringAppend(dst, gOtherText_PokeBlockMade);
#else
    GermanStringFormatName(dst, gOtherText_PokeBlockMade);
#endif
    StringAppend(dst, sNewLineString_0);

    flavorLvl = GetHighestPokeblocksFlavorLevel(pokeblock);
    feel = GetPokeblocksFeel(pokeblock);

    StringAppend(dst, gOtherText_BlockLevelIs);
    ConvertIntToDecimalStringN(text, flavorLvl, 0, 3);
    StringAppend(dst, text);

    StringAppend(dst, gOtherText_BlockFeelIs);
    ConvertIntToDecimalStringN(text, feel, 0, 3);
    StringAppend(dst, text);

    StringAppend(dst, gOtherText_Period);
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

    for (i = 0; i < sBerryBlender->numPlayers; i++)
        places[i] = i;
    for (i = 0; i < sBerryBlender->numPlayers; i++)
    {
        points[i] = 1000000 * sBerryBlender->scores[i][BLENDER_SCORE_BEST];
        points[i] += 1000 * sBerryBlender->scores[i][BLENDER_SCORE_GOOD];
        points[i] += 1000 - sBerryBlender->scores[i][BLENDER_SCORE_MISS];
    }
    SortBasedOnPoints(places, sBerryBlender->numPlayers, points);
    for (i = 0; i < sBerryBlender->numPlayers; i++)
        sBerryBlender->playerPlaces[i] = places[i];
}

static bool8 PrintBlendingRanking(void)
{
    u16 i;
    switch (sBerryBlender->mainState)
    {
    case 0:
        sBerryBlender->mainState++;
        sBerryBlender->framesToWait = 255;
        break;
    case 1:
        sBerryBlender->framesToWait -= 10;
        if (sBerryBlender->framesToWait < 0)
        {
            sBerryBlender->framesToWait = 0;
            sBerryBlender->mainState++;
        }
        break;
    case 2:
        if (++sBerryBlender->framesToWait > 20)
        {
            sBerryBlender->framesToWait = 0;
            sBerryBlender->mainState++;
        }
        break;
    case 3:
        Menu_DrawStdWindowFrame(4, 2, 25, 17);
        MenuPrint_Centered(gOtherText_Ranking, 5, 3, 160);

        sBerryBlender->scoreIconIds[BLENDER_SCORE_BEST] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 140, 52, 0);
        gSprites[sBerryBlender->scoreIconIds[BLENDER_SCORE_BEST]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[sBerryBlender->scoreIconIds[BLENDER_SCORE_BEST]], 3);

        sBerryBlender->scoreIconIds[BLENDER_SCORE_GOOD] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 164, 52, 0);
        gSprites[sBerryBlender->scoreIconIds[BLENDER_SCORE_GOOD]].callback = SpriteCallbackDummy;

        sBerryBlender->scoreIconIds[BLENDER_SCORE_MISS] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 188, 52, 0);
        gSprites[sBerryBlender->scoreIconIds[BLENDER_SCORE_MISS]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[sBerryBlender->scoreIconIds[BLENDER_SCORE_MISS]], 1);

        SortScores();

        for (i = 0; i < sBerryBlender->numPlayers; i++)
        {
            u8 place = sBerryBlender->playerPlaces[i];
            u8* txtPtr = sBerryBlender->stringVar;

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

            txtPtr = AlignInt1InMenuWindow(txtPtr, sBerryBlender->scores[place][BLENDER_SCORE_BEST], 108, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, sBerryBlender->scores[place][BLENDER_SCORE_GOOD], 132, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, sBerryBlender->scores[place][BLENDER_SCORE_MISS], 156, 1);

            Menu_PrintText(sBerryBlender->stringVar, 5, i * sRankingPlayerNameSpacingY[sBerryBlender->numPlayers] + 8);
        }
        sBerryBlender->framesToWait = 0;
        sBerryBlender->mainState++;
        break;
    case 4:
        if (++sBerryBlender->framesToWait > 20)
            sBerryBlender->mainState++;
        break;
    case 5:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sBerryBlender->mainState++;
        }
        break;
    case 6:
        sBerryBlender->mainState = 0;
        return TRUE;
    }
    return FALSE;
}

// debug menu goes here

void CB2_InitBerryBlenderDebugMenu(void)
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
    sBlenderDebug.needsRedraw++;
    SetMainCallback2(CB2_BerryBlenderDebugMenu);
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
        u8 y;

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
        y = (i * 3) + 3;
        Menu_PrintText(text, 2, y);

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
        Menu_PrintText(text, BLENDER_DEBUG_BERRY_LEN + 1, y);
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

static void PrintBerryBlenderDebugRngData(void)
{
    u8 text[70];
    u8 buffer[10];
    u16 i;

    if (sBlenderDebugRngTestState == 1)
    {
        u16 j;
        for (j = 0; j < 10; j++)
            gUnusedBlenderDebugRngData[j] = 0;
        gBlenderDebugRngSeed = Random();
        sBlenderDebugRngRunLength = 0;
        sBlenderDebugRngTestState = 2;
        for (i = 0; i < 200; i++)
            gSharedMem[i] = 0;
        sBlenderDebugRngBit = 0;
    }
    for (i = 0; i < 100; i++)
    {
        if (((Random() >> 15) & 1) == sBlenderDebugRngBit)
            sBlenderDebugRngRunLength++;
        else
        {
            u16* ewramPtr = ((u16*)(gSharedMem));
            ewramPtr[sBlenderDebugRngRunIndex] = sBlenderDebugRngRunLength;
            sBlenderDebugRngRunIndex++;
            sBlenderDebugRngRunLength = 0;
            sBlenderDebugRngBit ^= 1;
        }
    }
    text[0] = EOS;

    ConvertIntToHexStringN(buffer, gBlenderDebugRngSeed, 2, 8);
    StringAppend(text, buffer);
    StringAppend(text, sText_Space);

    ConvertIntToHexStringN(buffer, sBlenderDebugRngRunLength, 2, 8);
    StringAppend(text, buffer);
    StringAppend(text, sNewLineString_1);

    if (sBlenderDebugRngTestState == 3)
    {
        ConvertIntToHexStringN(buffer, sBlenderDebugRngRunIndex, 2, 16);
        StringAppend(text, buffer);
        sBlenderDebugRngTestState = 0;
    }

    Menu_PrintText(text, 2, 15);
}

static void HandleBerryBlenderDebugMenuInput(void)
{
    if (JOY_NEW(R_BUTTON))
    {
        sBlenderDebug.BPM += 1000;
        if (sBlenderDebug.BPM > 30000)
            sBlenderDebug.BPM = 1000;
        sBlenderDebug.needsRedraw++;
    }
    if (JOY_NEW(L_BUTTON))
    {
        sBlenderDebug.BPM -= 1000;
        if (sBlenderDebug.BPM < 0)
            sBlenderDebug.BPM = 30000;
        sBlenderDebug.needsRedraw++;
    }
    if (JOY_NEW(DPAD_UP))
    {
        sBlenderDebug.cursorPos -= 1;
        if (sBlenderDebug.cursorPos < 0)
            sBlenderDebug.cursorPos = 3;
        sBlenderDebug.needsRedraw++;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        sBlenderDebug.cursorPos += 1;
        if (sBlenderDebug.cursorPos > 3)
            sBlenderDebug.cursorPos = 0;
        sBlenderDebug.needsRedraw++;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (--sBlenderDebug.berries[sBlenderDebug.cursorPos] < 0)
            sBlenderDebug.berries[sBlenderDebug.cursorPos] = 42;
        sBlenderDebug.needsRedraw++;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (++sBlenderDebug.berries[sBlenderDebug.cursorPos] > 42)
            sBlenderDebug.berries[sBlenderDebug.cursorPos] = 0;
        sBlenderDebug.needsRedraw++;
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
            sBlenderDebug.needsRedraw++;
        }
        else
            sBlenderDebug.pokeblock.color = 0xFF;
    }
    if (sBlenderDebug.needsRedraw)
    {
        BlenderDebug_PrintBerryData();
        sBlenderDebug.needsRedraw = 0;
    }
    if (JOY_NEW(SELECT_BUTTON) && sBlenderDebugRngTestState == 0)
    {
        sBlenderDebugRngTestState++;
        sBlenderDebugRngRunLength = 0;
        SeedRng(gMain.vblankCounter1);
    }
    if (sBlenderDebugRngTestState != 0)
        PrintBerryBlenderDebugRngData();
}

static void CB2_BerryBlenderDebugMenu(void)
{
    HandleBerryBlenderDebugMenuInput();
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
    Menu_PrintText(gMultiText_BerryBlenderMaxSpeedRecord, 8, 4);
    Menu_PrintText(gMultiText_2P3P4P, 8, 9);

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
        StringAppend(txtPtr, gOtherText_RPM);
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
        PlayBGM(sBerryBlender->savedMusic);
        DestroyTask(taskId);
    }
}
