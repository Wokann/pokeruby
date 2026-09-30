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

struct BerryBlenderData
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
void SetOutlineCursorPosition(u8 a1, u8 a2);
u8 CreateOutlineCursor(u8 a1, u16 a2, u8 a3, u16 a4, u8 a5);
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
struct BerryBlenderData* gBerryBlenderData;

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
static void InitLocalPlayers(u8 NoOfOpponents);
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
static void ShakeBgCoordForHit(s16* a0, u16 a1);
static void TryUpdateProgressBar(u16 a0, u16 a1);
static void UpdateRPM(u16 a0);
static void RestoreBgCoords(void);
static void ProcessLinkPlayerCmds(void);
static void CB2_EndBlenderGame(void);
static bool8 PrintBlendingRanking(void);
bool8 PrintBlendingResults(void);
static void CB2_CheckPlayAgainLocal(void);
static void CB2_CheckPlayAgainLink(void);
static void UpdateProgressBar(u16 a0, u16 a1);
static void PrintMadePokeblockString(struct Pokeblock* pokeblock, u8* dst);
static void Task_PlayPokeblockFanfare(u8 taskID);
static void CB2_BerryBlenderDebugMenu(void);
static void Task_HandleOpponent1(u8 taskID);
static void Task_HandleOpponent2(u8 taskID);
static void Task_HandleOpponent3(u8 taskID);
static void SpriteCB_PlayerArrow(struct Sprite* sprite);
static void SpriteCB_ScoreSymbol(struct Sprite* sprite);
static void SpriteCB_CountdownNumber(struct Sprite* sprite);
static void SpriteCB_Start(struct Sprite* sprite);

// const data
static const u16 sBlenderCenterPal[] = INCBIN_U16("graphics/berry_blender/center.gbapal");
static const u8 sBlenderCenterMap[] = INCBIN_U8("graphics/berry_blender/center_map.bin");
static const u16 sBlenderOuterPal[] = INCBIN_U16("graphics/berry_blender/outer.gbapal");

// unreferenced pals?
static const u16 sUnused_Pal[] = INCBIN_U16("graphics/unused/unknown/821604C.gbapal");
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
    m4aMPlayPitchControl(&gMPlayInfo_SE2, 0xFFFF, (gBerryBlenderData->speed - 128) * 2);
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
    switch (gBerryBlenderData->loadGfxState)
    {
    case 0:
        LZDecompressWram(gBerryBlenderCenter_Gfx, eBerryBlenderGfxBuffer);
        gBerryBlenderData->loadGfxState++;
        break;
    case 1:
        DmaCopy16Defvars(3, sBlenderCenterMap, (void *)(VRAM + 0x4000), 0x400);
        LoadPalette(sBlenderCenterPal, 0, 0x100);
        gBerryBlenderData->loadGfxState++;
        break;
    case 2:
        DmaCopyLarge16(3, eBerryBlenderGfxBuffer, (void *)(VRAM + 0x0), 0x2000, 0x1000);
        gBerryBlenderData->loadGfxState++;
        break;
    case 3:
        LZDecompressWram(gBerryBlenderOuter_Gfx, eBerryBlenderGfxBuffer);
        gBerryBlenderData->loadGfxState++;
        break;
    case 4:
        LZDecompressWram(gBerryBlenderOuter_Tilemap, eBerryBlenderGfxBuffer + 0x3000);
        gBerryBlenderData->loadGfxState++;
        break;
    case 5:
        DmaCopy16Defvars(3, eBerryBlenderGfxBuffer + 0x0000, (void *)(VRAM + 0xE000), 0x1000);
        gBerryBlenderData->loadGfxState++;
        break;
    case 6:
		DmaCopy16Defvars(3, eBerryBlenderGfxBuffer + 0x1000, (void *)(VRAM + 0xF000), 0x1000);
        gBerryBlenderData->loadGfxState++;
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
            LoadPalette(sBlenderOuterPal, 0x80, 0x20);
            gBerryBlenderData->loadGfxState++;
        }
        break;
    case 8:
        LoadSpriteSheet(&sSpriteSheet_PlayerArrow);
        LoadSpriteSheet(&sSpriteSheet_Particles);
        LoadSpriteSheet(&sSpriteSheet_ScoreSymbols);
        gBerryBlenderData->loadGfxState++;
        break;
    case 9:
        LoadSpriteSheet(&sSpriteSheet_CountdownNumbers);
        LoadSpriteSheet(&sSpriteSheet_Start);
        LoadSpritePalette(&sSpritePal_PlayerArrow);
        LoadSpritePalette(&sSpritePal_BlenderMisc);
        gBerryBlenderData->loadGfxState = 0;
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
    u8* field6F; //this temp value is needed to match

    gBerryBlenderData = eBerryBlenderData;

    field6F = &gBerryBlenderData->gameEndState;
    gBerryBlenderData->mainState = 0;
    *field6F = 0;

    InitLocalPlayers(gSpecialVar_0x8004);
    SetMainCallback2(CB2_LoadBerryBlender);
}

static void CB2_LoadBerryBlender(void)
{
    s32 i;
    switch (gBerryBlenderData->mainState)
    {
    case 0:
        REG_DISPCNT = 0;
        ResetSpriteData();
        FreeAllSpritePalettes();
        SetVBlankCallback(NULL);
        Text_LoadWindowTemplate(&gBerryBlenderWindowTemplate);
        InitMenuWindow(&gBerryBlenderWindowTemplate);
        gBerryBlenderData->mainState++;
        gBerryBlenderData->maxProgressBarValue = 0;
        gBerryBlenderData->progressBarValue = 0;
        gBerryBlenderData->centerScale = 0x50;
        gBerryBlenderData->bg_X = 0;
        gBerryBlenderData->bg_Y = 0;
        gBerryBlenderData->loadGfxState = 0;
        UpdateBlenderCenter();
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
            {
                gBerryBlenderData->playerArrowSpriteIds[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
                StartSpriteAnim(&gSprites[gBerryBlenderData->playerArrowSpriteIds[i]], i + 8);
            }
            SetVBlankCallback(VBlankCB0_BerryBlender);
            gBerryBlenderData->mainState++;
        }
        break;
    case 2:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        UpdateBlenderCenter();
        gBerryBlenderData->mainState++;
        break;
    case 3:
        DrawBlenderBg();
        if (!gPaletteFade.active)
        {
            gBerryBlenderData->mainState++;
        }
        break;
    case 4:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        MenuPrintMessage(gOtherText_BlenderChooseBerry, 1, 15);
        gBerryBlenderData->mainState++;
        break;
    case 5:
        if (Menu_UpdateWindowText())
        {
            gBerryBlenderData->mainState++;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
        break;
    case 6:
        if (!gPaletteFade.active)
        {
            ChooseBerryForBlender();
            gBerryBlenderData->mainState = 0;
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

void SetBerrySpriteData(struct Sprite* sprite, s16 a2, s16 a3, s16 a4, s16 a5, s16 a6)
{
    sprite->data[0] = a3;
    sprite->data[1] = a2;
    sprite->data[2] = a3;
    sprite->data[3] = a4;
    sprite->data[4] = 10;
    sprite->data[5] = 0;
    sprite->data[6] = a5;
    sprite->data[7] = a6;
    sprite->callback = SpriteCB_Berry;
}

static void CreateBlenderBerrySprite(u16 a0, u8 a1)
{
    u8 spriteID = CreateSpinningBerrySprite(a0 + 123, 0, 80, a1 & 1);
    SetBerrySpriteData(&gSprites[spriteID], sBerrySpriteData[a1][0], sBerrySpriteData[a1][1], sBerrySpriteData[a1][2], sBerrySpriteData[a1][3], sBerrySpriteData[a1][4]);
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

static void InitLocalPlayers(u8 NoOfOpponents)
{
    int i;
    if (NoOfOpponents)
    {
        for (i = 0; i < 4; i++)
            gLinkPlayers[i].language = GAME_LANGUAGE;
    }
    switch (NoOfOpponents)
    {
    case 0:
        gInGameOpponentsNo = 0;
        break;
    case 1:
        gInGameOpponentsNo = 1;
        gBerryBlenderData->numPlayers = 2;
        StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
        StringCopy(gLinkPlayers[1].name, sBlenderOpponentsNames[0]);
        break;
    case 2:
        gInGameOpponentsNo = 2;
        gBerryBlenderData->numPlayers = 3;
        StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
        StringCopy(gLinkPlayers[1].name, sBlenderOpponentsNames[0]);
        StringCopy(gLinkPlayers[2].name, sBlenderOpponentsNames[1]);
        break;
    case 3:
        gInGameOpponentsNo = 3;
        gBerryBlenderData->numPlayers = 4;
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
    gBerryBlenderData = eBerryBlenderData;
    gBerryBlenderData->mainState = 0;
    gBerryBlenderData->unused_134 = 0;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        gBerryBlenderData->chosenItemId[i] = 0;
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
    switch (gBerryBlenderData->mainState)
    {
    case 0:
        ResetSpriteData();
        FreeAllSpritePalettes();
        ResetTasks();
        SetVBlankCallback(VBlankCB0_BerryBlender);
        Text_LoadWindowTemplate(&gBerryBlenderWindowTemplate);
        InitMenuWindow(&gBerryBlenderWindowTemplate);
        gLinkType = LINKTYPE_BERRY_BLENDER;
        gBerryBlenderData->mainState++;
        gBerryBlenderData->unused_4E = 0;
        gBerryBlenderData->slowdownTimer = 0;
        gBerryBlenderData->bg_X = 0;
        gBerryBlenderData->bg_Y = 0;
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlenderData->playerContinueResponses[i] = 0;
            for (j = 0; j < 3; j++)
            {
                gBerryBlenderData->scores[i][j] = 0;
            }
        }
        gBerryBlenderData->playAgainState = 0;
        gBerryBlenderData->speed = 0;
        gBerryBlenderData->arrowPos = 0;
        gBerryBlenderData->maxRPM = 0;
        gBerryBlenderData->loadGfxState = 0;
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            gBerryBlenderData->mainState++;
            UpdateBlenderCenter();
        }
        break;
    case 2:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlenderData->playerArrowSpriteIds2[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
            StartSpriteAnim(&gSprites[gBerryBlenderData->playerArrowSpriteIds2[i]], i + 8);
        }
        gBerryBlenderData->mainState++;
        break;
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gBerryBlenderData->mainState++;
        break;
    case 4:
        DrawBlenderBg();
        if (!gPaletteFade.active)
        {
            gBerryBlenderData->mainState++;
        }
        break;
    case 5:
        Menu_DrawStdWindowFrame(0, 13, 29, 19);
        Menu_PrintText(gOtherText_LinkStandby3, 1, 14);
        gBerryBlenderData->mainState = 8;
        gBerryBlenderData->framesToWait = 0;
        break;
    case 8:
        gBerryBlenderData->mainState++;
        gBerryBlenderData->playerToThrowBerry = 0;
        ConvertItemToBlenderBerry(&gBerryBlenderData->blendedBerries[0], gSpecialVar_ItemId);
        memcpy(gBlockSendBuffer, &gBerryBlenderData->blendedBerries[0], sizeof(struct BlenderBerry));
        SetLinkStandbyCallback();
        gBerryBlenderData->framesToWait = 0;
        break;
    case 9:
        if (IsLinkTaskFinished())
        {
            ResetBlockReceivedFlags();
            if (GetMultiplayerId() == 0)
                SendBlockRequest(BLOCK_REQ_SIZE_40);
            gBerryBlenderData->mainState++;
        }
        break;
    case 10:
        if (++gBerryBlenderData->framesToWait > 20)
        {
            Menu_EraseScreen();
            if (GetBlockReceivedStatus() == GetSavedLinkPlayerCountAsBitFlags())
            {
                for (i = 0; i < GetLinkPlayerCount(); i++)
                {
                    memcpy(&gBerryBlenderData->blendedBerries[i], &gBlockRecvBuffer[i][0], sizeof(struct BlenderBerry));
                    gBerryBlenderData->chosenItemId[i] = gBerryBlenderData->blendedBerries[i].itemId;
                }
                ResetBlockReceivedFlags();
                gBerryBlenderData->mainState++;
            }
        }
        break;
    case 11:
        gBerryBlenderData->numPlayers = GetLinkPlayerCount();
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (gBerryBlenderData->playerToThrowBerry == sPlayerIdMap[gBerryBlenderData->numPlayers - 2][i])
            {
                CreateBlenderBerrySprite(gBerryBlenderData->chosenItemId[gBerryBlenderData->playerToThrowBerry], i);
                break;
            }
        }
        gBerryBlenderData->framesToWait = 0;
        gBerryBlenderData->mainState++;
        gBerryBlenderData->playerToThrowBerry++;
        break;
    case 12:
        if (++gBerryBlenderData->framesToWait > 60)
        {
            if (gBerryBlenderData->playerToThrowBerry >= gBerryBlenderData->numPlayers)
            {
                gBerryBlenderData->mainState++;
                gBerryBlenderData->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlenderData->numPlayers - 2]] - 22528;
            }
            else
                gBerryBlenderData->mainState--;
            gBerryBlenderData->framesToWait = 0;
        }
        break;
    case 13:
        if (IsLinkTaskFinished())
        {
            gBerryBlenderData->mainState++;
            DrawBlenderCenter(&gBerryBlenderData->bgAffineDst);
        }
        break;
    case 14:
        REG_DISPCNT |= 0x400;
        gBerryBlenderData->arrowPos += 0x200;
        gBerryBlenderData->centerScale += 4;
        if (gBerryBlenderData->centerScale > 255)
        {
            gBerryBlenderData->mainState++;
            gBerryBlenderData->centerScale = 256;
            gBerryBlenderData->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlenderData->numPlayers - 2]];
            REG_BG2CNT = 0x4882;
            gBerryBlenderData->framesToWait = 0;
            SetPlayerIdMaps();
            PrintPlayerNames();
        }
        DrawBlenderCenter(&gBerryBlenderData->bgAffineDst);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->mainState++;
        }
        DrawBlenderCenter(&gBerryBlenderData->bgAffineDst);
        break;
    case 16:
        CreateSprite(&sSpriteTemplate_CountdownNumbers, 120, -16, 3);
        gBerryBlenderData->mainState++;
        break;
    case 18:
        gBerryBlenderData->mainState++;
        break;
    case 19:
        SetLinkStandbyCallback();
        gBerryBlenderData->mainState++;
        break;
    case 20:
        if (IsLinkTaskFinished())
        {
            SetBerryBlenderLinkCallback();
            gBerryBlenderData->mainState++;
        }
        break;
    case 21:
        gBerryBlenderData->speed = 128;
        gBerryBlenderData->gameFrameTime = 0;
        SetMainCallback2(CB2_PlayBlender);
        if (GetCurrentMapMusic() != 403)
        {
            gBerryBlenderData->savedMusic = GetCurrentMapMusic();
        }
        PlayBGM(MUS_CYCLING);
        break;
    case 100:
        Menu_DrawStdWindowFrame(0, 13, 29, 19);
        MenuPrintMessage(gOtherText_LinkNotFound, 1, 15);
        gBerryBlenderData->mainState++;
        break;
    case 101:
        if (Menu_UpdateWindowText())
            gBerryBlenderData->mainState++;
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

    gBerryBlenderData->unused_4E = 0;
    gBerryBlenderData->speed = 0;
    gBerryBlenderData->arrowPos = 0;
    gBerryBlenderData->maxRPM = 0;
    gBerryBlenderData->bg_X = 0;
    gBerryBlenderData->bg_Y = 0;
    gBerryBlenderData->mainState++;
}

static u8 GetArrowProximity(u16 arrowPos, u8 a1)
{
    u32 var1 = (arrowPos / 256) + 24;
    u8 arrID = gBerryBlenderData->playerIdToArrowId[a1];
    u32 var2 = sArrowHitRangeStart[arrID];

    if (var1 >= var2 && var1 < var2 + 48)
    {
        if (var1 >= var2 + 20 && var1 < var2 + 28)
            return 2;
        else
            return 1;
    }
    else
        return 0;
}

static void SetOpponentsBerryData(u16 itemId, u8 a1, struct BlenderBerry* berry)
{
    u16 r4 = 0;
    u16 i;
    if (itemId == ITEM_ENIGMA_BERRY)
    {
        for (i = 0; i < 5; i++)
        {
            if (berry->flavors[r4] > berry->flavors[i])
                r4 = i;
        }
        r4 += 5;
    }
    else
    {
        r4 = itemId - 133;
        if (r4 >= 5)
            r4 = (r4 % 5) + 5;
    }
    for (i = 0; i < a1 - 1; i++)
    {
        SetPlayerBerryData(i + 1, sOpponentBerrySets[r4][i] + 133);
    }
}

static void SetPlayerIdMaps(void)
{
    s32 i, j;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        gBerryBlenderData->playerIdToArrowId[i] = 0xFF;
        gBerryBlenderData->arrowIdToPlayerId[i] = sPlayerIdMap[gBerryBlenderData->numPlayers - 2][i];
    }
    for (j = 0; j < BLENDER_MAX_PLAYERS; j++)
    {
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (gBerryBlenderData->arrowIdToPlayerId[i] == j)
                gBerryBlenderData->playerIdToArrowId[j] = i;
        }
    }
}

static void PrintPlayerNames(void)
{
    int i;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        if (gBerryBlenderData->arrowIdToPlayerId[i] != 0xFF)
        {
            u8* stringPtr = gStringVar1;

            gBerryBlenderData->playerArrowSpriteIds[gBerryBlenderData->arrowIdToPlayerId[i]] = gBerryBlenderData->playerArrowSpriteIds2[i];
            StartSpriteAnim(&gSprites[gBerryBlenderData->playerArrowSpriteIds[gBerryBlenderData->arrowIdToPlayerId[i]]], i);
            if (GetMultiplayerId() == gBerryBlenderData->arrowIdToPlayerId[i])
                stringPtr = StringCopy(stringPtr, sRedColorString);
            StringCopy(stringPtr, gLinkPlayers[gBerryBlenderData->arrowIdToPlayerId[i]].name);
            Menu_PrintTextPixelCoords(gStringVar1, sPlayerNameTextCoords[i][0] * 8 + 1, sPlayerNameTextCoords[i][1] * 8, 1);
        }
    }
}

static void CB2_StartBlenderLocal(void)
{
    s32 i, j;
    switch (gBerryBlenderData->mainState)
    {
    case 0:
        InitBlenderBgs();
        SetPlayerBerryData(0, gSpecialVar_ItemId);
        ConvertItemToBlenderBerry(&gBerryBlenderData->blendedBerries[0], gSpecialVar_ItemId);
        SetOpponentsBerryData(gSpecialVar_ItemId, gBerryBlenderData->numPlayers, &gBerryBlenderData->blendedBerries[0]);
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlenderData->playerContinueResponses[i] = 0;
            for (j = 0; j < 3; j++)
            {
                gBerryBlenderData->scores[i][j] = 0;
            }
        }
        gBerryBlenderData->playAgainState = 0;
        gBerryBlenderData->loadGfxState = 0;
        break;
    case 1:
        if (LoadBerryBlenderGfx())
        {
            gBerryBlenderData->mainState++;
            UpdateBlenderCenter();
        }
        break;
    case 2:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            gBerryBlenderData->playerArrowSpriteIds2[i] = CreateSprite(&sSpriteTemplate_PlayerArrow, sPlayerArrowPos[i][0], sPlayerArrowPos[i][1], 1);
            StartSpriteAnim(&gSprites[gBerryBlenderData->playerArrowSpriteIds2[i]], i + 8);
        }
        gBerryBlenderData->mainState++;
        break;
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gBerryBlenderData->mainState++;
        gBerryBlenderData->framesToWait = 0;
        break;
    case 4:
        if (++gBerryBlenderData->framesToWait == 2)
            DrawBlenderBg();
        if (!gPaletteFade.active)
            gBerryBlenderData->mainState = 8;
        break;
    case 8:
        gBerryBlenderData->mainState = 11;
        gBerryBlenderData->playerToThrowBerry = 0;
        break;
    case 11:
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            u32 var = sPlayerIdMap[gBerryBlenderData->numPlayers - 2][i];
            if (gBerryBlenderData->playerToThrowBerry == var)
            {
                CreateBlenderBerrySprite(gBerryBlenderData->chosenItemId[gBerryBlenderData->playerToThrowBerry], i);
                break;
            }
        }
        gBerryBlenderData->framesToWait = 0;
        gBerryBlenderData->mainState++;
        gBerryBlenderData->playerToThrowBerry++;
        break;
    case 12:
        if (++gBerryBlenderData->framesToWait > 60)
        {
            if (gBerryBlenderData->playerToThrowBerry >= gBerryBlenderData->numPlayers)
            {
                gBerryBlenderData->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlenderData->numPlayers - 2]] - 22528;
                gBerryBlenderData->mainState++;
            }
            else
                gBerryBlenderData->mainState--;
            gBerryBlenderData->framesToWait = 0;
        }
        break;
    case 13:
        gBerryBlenderData->mainState++;
        SetPlayerIdMaps();
        PlaySE(SE_FALL);
        DrawBlenderCenter(&gBerryBlenderData->bgAffineDst);
        break;
    case 14:
        REG_DISPCNT |= 0x400;
        gBerryBlenderData->arrowPos += 0x200;
        gBerryBlenderData->centerScale += 4;
        if (gBerryBlenderData->centerScale > 255)
        {
            gBerryBlenderData->mainState++;
            gBerryBlenderData->centerScale = 256;
            gBerryBlenderData->arrowPos = sArrowStartPos[sArrowStartPosIds[gBerryBlenderData->numPlayers - 2]];
            REG_BG2CNT = 0x4882;
            gBerryBlenderData->framesToWait = 0;
            PlaySE(SE_TRUCK_DOOR);;
            PrintPlayerNames();
        }
        DrawBlenderCenter(&gBerryBlenderData->bgAffineDst);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            gBerryBlenderData->mainState++;
        }
        DrawBlenderCenter(&gBerryBlenderData->bgAffineDst);
        break;
    case 16:
        CreateSprite(&sSpriteTemplate_CountdownNumbers, 120, -16, 3);
        gBerryBlenderData->mainState++;
        break;
    case 18:
        gBerryBlenderData->mainState++;
        break;
    case 19:
        gBerryBlenderData->mainState++;
        break;
    case 20:
        gBerryBlenderData->mainState++;
        break;
    case 21:
        ResetLinkCmds();
        gBerryBlenderData->speed = 128;
        gBerryBlenderData->gameFrameTime = 0;
        gBerryBlenderData->perfectOpponents = 0;
        gBerryBlenderData->slowdownTimer = 0;
        SetMainCallback2(CB2_PlayBlender);

        for (i = 0; i < gSpecialVar_0x8004; i++)
        {
            gBerryBlenderData->opponentTaskIds[i] = CreateTask(sLocalOpponentTasks[i], 10 + i);
        }

        if (GetCurrentMapMusic() != 403)
        {
            gBerryBlenderData->savedMusic = GetCurrentMapMusic();
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

static void Task_OpponentMiss(u8 taskID)
{
   if(++gTasks[taskID].data[0] > gTasks[taskID].data[1])
   {
        gRecvCmds[2][gTasks[taskID].data[2]] = 0x2345;
        DestroyTask(taskID);
   }
}

static void CreateOpponentMissTask(u8 a0, u8 a1)
{
    u8 taskID = CreateTask(Task_OpponentMiss, 80);
    gTasks[taskID].data[1] = a1;
    gTasks[taskID].data[2] = a0;
}

static void Task_HandleOpponent1(u8 taskID)
{
    if (GetArrowProximity(gBerryBlenderData->arrowPos, 1) == 2)
    {
        if (gTasks[taskID].data[0] == 0)
        {
            if (gBerryBlenderData->perfectOpponents == 0)
            {
                u8 rand = Random() / 655;
                if (gBerryBlenderData->speed < 500)
                {
                    if (rand > 75)
                        gRecvCmds[2][1] = 0x4523;
                    else
                        gRecvCmds[2][1] = 0x5432;
                    gRecvCmds[2][1] = 0x5432; // ???
                }
                else if (gBerryBlenderData->speed < 1500)
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

            gTasks[taskID].data[0] = 1;
        }
    }
    else
        gTasks[taskID].data[0] = 0;
}

static void Task_HandleOpponent2(u8 taskID)
{
    u32 var1 = (gBerryBlenderData->arrowPos + 0x1800) & 0xFFFF;
    u32 var2 = gBerryBlenderData->playerIdToArrowId[2] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[var2] + 20 && (var1 >> 8) < sArrowHitRangeStart[var2] + 40)
    {
        if (gTasks[taskID].data[0] == 0)
        {
            if (gBerryBlenderData->perfectOpponents == 0)
            {
                u8 rand = Random() / 655;
                if (gBerryBlenderData->speed < 500)
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

                gTasks[taskID].data[0] = 1;
            }
            else
            {
                gRecvCmds[2][2] = 0x4523;
                gTasks[taskID].data[0] = 1;
            }
        }
    }
    else
        gTasks[taskID].data[0] = 0;
}

static void Task_HandleOpponent3(u8 taskID)
{
    u32 var1, var2;

    var1 = (gBerryBlenderData->arrowPos + 0x1800) & 0xFFFF;
    var2 = gBerryBlenderData->playerIdToArrowId[3] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[var2] + 20 && (var1 >> 8) < sArrowHitRangeStart[var2] + 40)
    {
        if (gTasks[taskID].data[0] == 0)
        {
            if (gBerryBlenderData->perfectOpponents == 0)
            {
                u8 rand = (Random() / 655);
                if (gBerryBlenderData->speed < 500)
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
                gTasks[taskID].data[0] = 1;
            }
            else
            {
                gRecvCmds[2][3] = 0x4523;
                gTasks[taskID].data[0] = 1;
            }
        }
    }
    else
        gTasks[taskID].data[0] = 0;
}

static void CreateScoreSymbolSprite(u16 a0, u8 a1)
{
    u8 spriteID;

    spriteID = CreateSprite(&sSpriteTemplate_ScoreSymbols,
                            sPlayerArrowPos[a1][0] - (10 * sPlayerArrowQuadrant[a1][0]),
                            sPlayerArrowPos[a1][1] - (10 * sPlayerArrowQuadrant[a1][1]),
                            1);
    if (a0 == 0x4523)
    {
        StartSpriteAnim(&gSprites[spriteID], 2);
        gSprites[spriteID].callback = SpriteCB_ScoreSymbolBest;
        PlaySE(SE_ICE_STAIRS);
    }
    else if (a0 == 0x5432)
    {
        StartSpriteAnim(&gSprites[spriteID], 0);
        PlaySE(SE_SUCCESS);
    }
    else if (a0 == 0x2345)
    {
        StartSpriteAnim(&gSprites[spriteID], 1);
        PlaySE(SE_FAILURE);
    }
    CreateParticleSprites();
}

static void UpdateSpeedFromHit(u16 a0)
{
    UpdateHitPitch();
    switch (a0)
    {
    case 0x4523:
        if (gBerryBlenderData->speed < 1500)
            gBerryBlenderData->speed += (384 / sBerryMasterBerries[gBerryBlenderData->numPlayers]);
        else
        {
            gBerryBlenderData->speed += (128 / sBerryMasterBerries[gBerryBlenderData->numPlayers]);
            ShakeBgCoordForHit(&gBerryBlenderData->bg_X, (gBerryBlenderData->speed / 100) - 10);
            ShakeBgCoordForHit(&gBerryBlenderData->bg_Y, (gBerryBlenderData->speed / 100) - 10);
        }
        break;
    case 0x5432:
        if (gBerryBlenderData->speed < 1500)
            gBerryBlenderData->speed += (256 / sBerryMasterBerries[gBerryBlenderData->numPlayers]);
        break;
    case 0x2345:
        gBerryBlenderData->speed -= (256 / sBerryMasterBerries[gBerryBlenderData->numPlayers]);
        if (gBerryBlenderData->speed < 128)
            gBerryBlenderData->speed = 128;
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
    for (i = 0; i < gBerryBlenderData->numPlayers; i++)
    {
        if (gRecvCmds[0][i] == 0x4444)
        {
            u32 var = gBerryBlenderData->playerIdToArrowId[i];
            if (gRecvCmds[2][i] == 0x4523)
            {
                UpdateSpeedFromHit(0x4523);
                gBerryBlenderData->progressBarValue += (gBerryBlenderData->speed / 55);
                if (gBerryBlenderData->progressBarValue >= 1000)
                    gBerryBlenderData->progressBarValue = 1000;
                CreateScoreSymbolSprite(0x4523, var);
                gBerryBlenderData->scores[i][BLENDER_SCORE_BEST]++;
            }
            else if (gRecvCmds[2][i] == 0x5432)
            {
                UpdateSpeedFromHit(0x5432);
                gBerryBlenderData->progressBarValue += (gBerryBlenderData->speed / 70);
                CreateScoreSymbolSprite(0x5432, var);
                gBerryBlenderData->scores[i][BLENDER_SCORE_GOOD]++;
            }
            else if (gRecvCmds[2][i] == 0x2345)
            {
                CreateScoreSymbolSprite(0x2345, var);
                UpdateSpeedFromHit(0x2345);
                // looks like a bug
                if (gBerryBlenderData->menuWindow.win_field_F > 1000)
                    gBerryBlenderData->progressBarValue = 1000;
                if (gBerryBlenderData->scores[i][BLENDER_SCORE_MISS] < 999)
                    gBerryBlenderData->scores[i][BLENDER_SCORE_MISS]++;
            }
            if (gRecvCmds[2][i] == 0x2345 || gRecvCmds[2][i] == 0x4523 || gRecvCmds[2][i] == 0x5432)
            {
                if (gBerryBlenderData->speed > 1500)
                    m4aMPlayTempoControl(&gMPlayInfo_BGM, ((gBerryBlenderData->speed - 750) / 20) + 256);
                else
                    m4aMPlayTempoControl(&gMPlayInfo_BGM, 256);
            }
        }
    }
    if (gSpecialVar_0x8004 != 0)
    {
        for (i = 0; i < gBerryBlenderData->numPlayers; i++)
        {
            gRecvCmds[0][i] = 0;
            gRecvCmds[2][i] = 0;
        }
    }
}

static void HandlePlayerInput(void)
{
    bool8 A_pressed = 0;
    u8 var2 = gBerryBlenderData->playerIdToArrowId[GetMultiplayerId()];
    if (gBerryBlenderData->gameEndState == 0)
    {
        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A && JOY_NEW(A_BUTTON))
            A_pressed = ((gMain.heldKeysRaw & (A_BUTTON | L_BUTTON)) != (A_BUTTON | L_BUTTON));
        else if (JOY_NEW(A_BUTTON))
            A_pressed = 1;
        if (A_pressed)
        {
            u8 var3;
            StartSpriteAnim(&gSprites[gBerryBlenderData->playerArrowSpriteIds[gBerryBlenderData->arrowIdToPlayerId[var2]]], var2 + 4);
            var3 = GetArrowProximity(gBerryBlenderData->arrowPos, GetMultiplayerId());
            if (var3 == 2)
                gSendCmd[2] = 0x4523;
            else if (var3 == 1)
                gSendCmd[2] = 0x5432;
            else
                gSendCmd[2] = 0x2345;
        }
    }
    if (++gBerryBlenderData->slowdownTimer > 5)
    {
        if (gBerryBlenderData->speed > 128)
            gBerryBlenderData->speed--;
        gBerryBlenderData->slowdownTimer = 0;
    }
    if (gIsDebugMode && JOY_NEW(L_BUTTON))
        gBerryBlenderData->perfectOpponents ^= 1;
}

static void CB2_PlayBlender(void)
{
    UpdateBlenderCenter();
    if (gBerryBlenderData->gameFrameTime < (99 * 60 * 60) + (59 * 60)) // game time can't be longer than 99 minutes and 59 seconds, can't print 3 digits
        gBerryBlenderData->gameFrameTime++;
    HandlePlayerInput();
    SetLinkDebugValues((u16)(gBerryBlenderData->speed), gBerryBlenderData->progressBarValue);
    UpdateOpponentScores();
    TryUpdateProgressBar(gBerryBlenderData->progressBarValue, 1000);
    UpdateRPM(gBerryBlenderData->speed);
    RestoreBgCoords();
    ProcessLinkPlayerCmds();
    if (gBerryBlenderData->gameEndState == 0 && gBerryBlenderData->maxProgressBarValue >= 1000)
    {
        gBerryBlenderData->progressBarValue = 1000;
        gBerryBlenderData->gameEndState = 1;
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

u32 CalculatePokeblockColor(struct BlenderBerry* berries, s16* a1, u8 numPlayers, u8 a3)
{
    s16 vars[5];
    s32 i;
    s32 r6;
    u8 r2;

    // bug, writing one index too far
#ifdef UBFIX
    for (i = 0; i < 5; i++)
#else
    for (i = 0; i <= 5; i++)
#endif //UBFIX
        vars[i] = a1[i];
    r6 = 0;
    for (i = 0; i < 5; i++)
    {
        if (vars[i] == 0)
            r6++;
    }
    if (r6 == 5 || a3 > 3)
        return PBLOCK_CLR_BLACK;
    for (i = 0; i < numPlayers; i++)
    {
        for (r6 = 0; r6 < numPlayers; r6++)
        {
            if (berries[i].itemId == berries[r6].itemId && i != r6
                && (berries[i].itemId != ITEM_ENIGMA_BERRY || AreBlenderBerriesSame(berries, i, r6)))
                    return PBLOCK_CLR_BLACK;
        }
    }
    r2 = 0;
    for (r2 = 0, i = 0; i < 5; i++)
    {
        if (vars[i] > 0)
            r2++;
    }
    if (r2 > 3)
        return PBLOCK_CLR_WHITE;
    if (r2 == 3)
        return PBLOCK_CLR_GRAY;
    for (i = 0; i < 5; i++)
    {
        if (vars[i] > 50)
            return PBLOCK_CLR_GOLD;
    }
    if (r2 == 1 && vars[0] > 0)
        return PBLOCK_CLR_RED;
    if (r2 == 1 && vars[1] > 0)
        return PBLOCK_CLR_BLUE;
    if (r2 == 1 && vars[2] > 0)
        return PBLOCK_CLR_PINK;
    if (r2 == 1 && vars[3] > 0)
        return PBLOCK_CLR_GREEN;
    if (r2 == 1 && vars[4] > 0)
        return PBLOCK_CLR_YELLOW;
    if (r2 == 2)
    {
        s32 var = 0;
        for (i = 0; i < 5; i++)
        {
            if (vars[i] > 0)
                sPokeblockPresentFlavors[var++] = i;
        }
        if (vars[sPokeblockPresentFlavors[0]] >= vars[sPokeblockPresentFlavors[1]])
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
    s32 var3;
    s32 var4;
    s32 var6;
    u8 r10;

    for (i = 0; i < 6; i++)
        sPokeblockFlavors[i] = 0;
    for (i = 0; i < numPlayers; i++)
    {
        for (j = 0; j < 6; j++)
            sPokeblockFlavors[j] += berries[i].flavors[j];
    }

    var6 = sPokeblockFlavors[0];
    sPokeblockFlavors[0] -= sPokeblockFlavors[1];
    sPokeblockFlavors[1] -= sPokeblockFlavors[2];
    sPokeblockFlavors[2] -= sPokeblockFlavors[3];
    sPokeblockFlavors[3] -= sPokeblockFlavors[4];
    sPokeblockFlavors[4] -= var6;

    var6 = 0;
    for (i = 0; i < 5; i++)
    {
        if (sPokeblockFlavors[i] < 0)
        {
            sPokeblockFlavors[i] = 0;
            var6++;
        }
    }
    r10 = var6;
    for (i = 0; i < 5; i++)
    {
        if (sPokeblockFlavors[i] > 0)
        {
            if (sPokeblockFlavors[i] < var6)
                sPokeblockFlavors[i] = 0;
            else
                sPokeblockFlavors[i] -= var6;
        }
    }
    for (i = 0; i < 5; i++)
    {
        sDebug_PokeblockFactorFlavors[i] = sPokeblockFlavors[i];
    }

    var6 = maxRPM / 333 + 100;
    sDebug_PokeblockFactorRPM = var6;

    for (i = 0; i < 5; i++)
    {
        var3 = sPokeblockFlavors[i];
        var3 = (var3 * var6) / 10;
        var4 = var3 % 10;
        var3 /= 10;
        if (var4 > 4)
            var3++;
        sPokeblockFlavors[i] = var3;
    }
    for (i = 0; i < 5; i++)
    {
        sDebug_PokeblockFactorFlavorsAfterRPM[i] = sPokeblockFlavors[i];
    }
    pokeblock->color = CalculatePokeblockColor(berries, &sPokeblockFlavors[0], numPlayers, r10);
    sPokeblockFlavors[5] = (sPokeblockFlavors[5] / numPlayers) - numPlayers;
    if (sPokeblockFlavors[5] < 0)
        sPokeblockFlavors[5] = 0;
    if (pokeblock->color == 12)
    {
        var6 = Random() % 10;
        for (i = 0; i < 5; i++)
        {
            if ((sBlackPokeblockFlavorFlags[var6] >> i) & 1)
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
    u32 frames = (u16)(gBerryBlenderData->gameFrameTime);
    u16 maxRPM = gBerryBlenderData->maxRPM;
    s16 var = 0;

    if (frames < 900)
        var = 5;
    else if ((u16)(frames - 900) < 600)
        var = 4;
    else if ((u16)(frames - 1500) < 600)
        var = 3;
    else if ((u16)(frames - 2100) < 900)
        var = 2;
    else if ((u16)(frames - 3300) < 300)
        var = 1;
    Debug_SetGameTimeStage(var);

    var = 0;
    if (maxRPM <= 64)
    {
        if (maxRPM >= 50 && maxRPM < 100)
            var = -1;
        else if (maxRPM >= 100 && maxRPM < 150)
            var = -2;
        else if (maxRPM >= 150 && maxRPM < 200)
            var = -3;
        else if (maxRPM >= 200 && maxRPM < 250)
            var = -4;
        else if (maxRPM >= 250 && maxRPM < 300)
            var = -5;
        else if (maxRPM >= 350 && maxRPM < 400)
            var = -6;
        else if (maxRPM >= 400 && maxRPM < 450)
            var = -7;
        else if (maxRPM >= 500 && maxRPM < 550)
            var = -8;
        else if (maxRPM >= 550 && maxRPM < 600)
            var = -9;
        else if (maxRPM >= 600)
            var = -10;
    }
    Debug_SetMaxRPMStage(var);
}

static void SetBlenderContinueMenuCursor(u8 value)
{
    gBerryBlenderData->yesNoAnswer = value;
    SetOutlineCursorPosition(192, (gBerryBlenderData->yesNoAnswer * 16) + 72);
}

static void ShowBlenderContinueMenu(void)
{
    gBerryBlenderData->yesNoAnswer = 0;
    Menu_DrawStdWindowFrame(23, 8, 28, 13);
    CreateOutlineCursor(0, -1, 12, 0x2D9F, 32);
    Menu_PrintText(gOtherText_YesNoTerminating, 24, 9);
    SetBlenderContinueMenuCursor(gBerryBlenderData->yesNoAnswer);
}

static void CB2_EndBlenderGame(void)
{
    u8 i;
    u8 multiplayerID; // unused

    UpdateBlenderCenter();
    multiplayerID = GetMultiplayerId();
    switch (gBerryBlenderData->gameEndState)
    {
    case 1:
        ClearLinkCallback();
        m4aMPlayTempoControl(&gMPlayInfo_BGM, 256);
        for (i = 0; i < gSpecialVar_0x8004; i++)
        {
            DestroyTask(gBerryBlenderData->opponentTaskIds[i]);
        }
        gBerryBlenderData->gameEndState++;
        break;
    case 2:
        gBerryBlenderData->speed -= 32;
        if (gBerryBlenderData->speed <= 0)
        {
            gBerryBlenderData->speed = 0;
            if (gReceivedRemoteLinkPlayers != 0)
                gBerryBlenderData->gameEndState++;
            else
                gBerryBlenderData->gameEndState = 5;
            gBerryBlenderData->mainState = 0;
            m4aMPlayStop(&gMPlayInfo_SE2);
        }
        UpdateHitPitch();
        break;
    case 3:
        if (/*multiplayerID != 0*/ GetMultiplayerId() != 0)
            gBerryBlenderData->gameEndState++;
        else if (IsLinkTaskFinished())
        {
            gBerryBlenderData->smallBlock.time = gBerryBlenderData->gameFrameTime;
            gBerryBlenderData->smallBlock.maxRPM = gBerryBlenderData->maxRPM;
            SendBlock(0, &gBerryBlenderData->smallBlock.time, 40);
            gBerryBlenderData->gameEndState++;
        }
        break;
    case 4:
        if (GetBlockReceivedStatus())
        {
            u32* ptr = ((u32*)(&gBlockRecvBuffer[0][0]));
            gBerryBlenderData->maxRPM = gBlockRecvBuffer[0][2];
            gBerryBlenderData->gameFrameTime = *ptr;
            gBerryBlenderData->gameEndState++;
            ResetBlockReceivedFlags();
        }
        break;
    case 5:
        if (PrintBlendingRanking())
            gBerryBlenderData->gameEndState++;
        break;
    case 6:
        if (PrintBlendingResults())
        {
            if (gInGameOpponentsNo == 0)
                IncrementGameStat(GAME_STAT_POKEBLOCKS_WITH_FRIENDS);
            else
                IncrementGameStat(GAME_STAT_POKEBLOCKS);
            gBerryBlenderData->gameEndState++;
        }
        break;
    case 7:
        gBerryBlenderData->gameEndState++;
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        MenuPrintMessage(gOtherText_BlendAnotherBerryPrompt, 1, 15);
        break;
    case 8:
        if (Menu_UpdateWindowText())
            gBerryBlenderData->gameEndState++;
        break;
    case 9:
        gBerryBlenderData->yesNoAnswer = 0;
        ShowBlenderContinueMenu();
        gBerryBlenderData->gameEndState++;
        break;
    case 10:
        if (JOY_NEW(DPAD_UP))
        {
            if (gBerryBlenderData->yesNoAnswer != 0)
                PlaySE(SE_SELECT);
            SetBlenderContinueMenuCursor(0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            if (gBerryBlenderData->yesNoAnswer != 1)
                PlaySE(SE_SELECT);
            SetBlenderContinueMenuCursor(1);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBerryBlenderData->gameEndState++;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBerryBlenderData->gameEndState++;
            SetBlenderContinueMenuCursor(1);
        }
        break;
    case 11:
        gSendCmd[0] = 0x2FFF;
        if (gBerryBlenderData->yesNoAnswer == 0)
        {
            if (IsBagPocketNonEmpty(BAG_BERRIES) == FALSE) // is empty
            {
                gBerryBlenderData->playAgainState = 2;
                gSendCmd[1] = 0x9999;
            }
            else if (GetFirstFreePokeblockSlot() == -1)
            {
                gBerryBlenderData->playAgainState = 3;
                gSendCmd[1] = 0xAAAA;
            }
            else
            {
                gBerryBlenderData->playAgainState = 0;
                gSendCmd[1] = 0x7779;
            }
            gBerryBlenderData->gameEndState++;
        }
        else
        {
            gBerryBlenderData->playAgainState = 1;
            gSendCmd[1] = 0x8888;
            gBerryBlenderData->gameEndState++;
        }
        break;
    case 12:
        if (gInGameOpponentsNo)
        {
            SetMainCallback2(CB2_CheckPlayAgainLocal);
            gBerryBlenderData->gameEndState = 0;
            gBerryBlenderData->mainState = 0;
        }
        else
        {
            MenuPrintMessage(gOtherText_LinkStandby3, 1, 15);
            gBerryBlenderData->gameEndState++;
        }
        break;
    case 13:
        if (Menu_UpdateWindowText())
        {
            SetMainCallback2(CB2_CheckPlayAgainLink);
            gBerryBlenderData->gameEndState = 0;
            gBerryBlenderData->mainState = 0;
        }
        break;
    }
    RestoreBgCoords();
    UpdateRPM(gBerryBlenderData->speed);
    ProcessLinkPlayerCmds();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

bool8 LinkPlayAgainHandleSaving(void)
{
    switch (gBerryBlenderData->linkPlayAgainState)
    {
    case 0:
        SetLinkStandbyCallback();
        gBerryBlenderData->linkPlayAgainState = 1;
        gBerryBlenderData->framesToWait = 0;
        break;
    case 1:
        if (IsLinkTaskFinished())
        {
            gBerryBlenderData->linkPlayAgainState++;
            gSoftResetDisabled = TRUE;
        }
        break;
    case 2:
        WriteSaveBlock2();
        gBerryBlenderData->linkPlayAgainState++;
        gBerryBlenderData->framesToWait = 0;
        break;
    case 3:
        if (++gBerryBlenderData->framesToWait == 10)
        {
            SetLinkStandbyCallback();
            gBerryBlenderData->linkPlayAgainState++;
        }
        break;
    case 4:
        if (IsLinkTaskFinished())
        {
            if (WriteSaveBlock1Sector())
                gBerryBlenderData->linkPlayAgainState = 5;
            else
            {
                gBerryBlenderData->framesToWait = 0;
                gBerryBlenderData->linkPlayAgainState = 3;
            }
        }
        break;
    case 5:
        gBerryBlenderData->linkPlayAgainState++;
        gBerryBlenderData->framesToWait = 0;
        break;
    case 6:
        if (++gBerryBlenderData->framesToWait > 5)
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
    switch (gBerryBlenderData->gameEndState)
    {
    case 0:
        if (gBerryBlenderData->playerContinueResponses[0] == 0x2222)
            gBerryBlenderData->gameEndState = 5;
        else if (gBerryBlenderData->playerContinueResponses[0] == 0x1111)
        {
            if (gBerryBlenderData->canceledPlayerCmd == 0x9999)
                gBerryBlenderData->gameEndState = 2;
            else if (gBerryBlenderData->canceledPlayerCmd == 0xAAAA)
                gBerryBlenderData->gameEndState = 1;
            else
                gBerryBlenderData->gameEndState = 5;
        }
        break;
    case 1:
        gBerryBlenderData->gameEndState = 3;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
#ifdef ENGLISH
        StringCopy(gStringVar4, gLinkPlayers[gBerryBlenderData->canceledPlayerId].name);
        StringAppend(gStringVar4, gOtherText_OtherCaseIsFull);
#else
        StringCopy(gStringVar4, gOtherText_OtherCaseIsFull);
        GermanStringReplaceNamePlaceholder(gStringVar4, gLinkPlayers[gBerryBlenderData->canceledPlayerId].name);
#endif
        MenuPrintMessage(gStringVar4, 1, 15);
        break;
    case 2:
        gBerryBlenderData->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
#ifdef ENGLISH
        StringCopy(gStringVar4, gLinkPlayers[gBerryBlenderData->canceledPlayerId].name);
        StringAppend(gStringVar4, gOtherText_NoBerriesForBlend);
#else
        StringCopy(gStringVar4, gOtherText_NoBerriesForBlend);
        GermanStringReplaceNamePlaceholder(gStringVar4, gLinkPlayers[gBerryBlenderData->canceledPlayerId].name);
#endif
        MenuPrintMessage(gStringVar4, 1, 15);
        break;
    case 3:
        if (Menu_UpdateWindowText())
        {
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->gameEndState++;
        }
        break;
    case 4:
        if (++gBerryBlenderData->framesToWait > 60)
            gBerryBlenderData->gameEndState = 5;
        break;
    case 5:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(gMultiText_Saving, 2, 15);
        SetLinkStandbyCallback();
        gBerryBlenderData->gameEndState++;
        break;
    case 6:
        if (IsLinkTaskFinished())
        {
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->gameEndState++;
            gBerryBlenderData->linkPlayAgainState = 0;
        }
        break;
    case 7:
        if (LinkPlayAgainHandleSaving())
        {
            PlaySE(SE_SAVE);
            gBerryBlenderData->gameEndState++;
        }
        break;
    case 8:
        gBerryBlenderData->gameEndState++;
        SetLinkStandbyCallback();
        break;
    case 9:
        if (IsLinkTaskFinished())
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gBerryBlenderData->gameEndState++;
        }
        break;
    case 10:
        if (!gPaletteFade.active)
        {
            if (gBerryBlenderData->playerContinueResponses[0] == 0x2222)
                SetMainCallback2(DoBerryBlending);
            else
            {
                gBerryBlenderData->framesToWait = 0;
                gBerryBlenderData->gameEndState++;
            }
        }
        break;
    case 11:
        if (++gBerryBlenderData->framesToWait > 30)
        {
            SetCloseLinkCallback();
            gBerryBlenderData->gameEndState++;
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
    switch (gBerryBlenderData->gameEndState)
    {
    case 0:
        if (gBerryBlenderData->playAgainState < 2)
            gBerryBlenderData->gameEndState = 9;
        if (gBerryBlenderData->playAgainState == 2)
            gBerryBlenderData->gameEndState = 2;
        if (gBerryBlenderData->playAgainState == 3)
            gBerryBlenderData->gameEndState =1;
        break;
    case 1:
        gBerryBlenderData->gameEndState = 3;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
        MenuPrintMessage(gOtherText_CaseIsFull, 1, 15);
        break;
    case 2:
        gBerryBlenderData->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
        MenuPrintMessage(gOtherText_OutOfBerries, 1, 15);
        break;
    case 3:
        if (Menu_UpdateWindowText())
            gBerryBlenderData->gameEndState = 9;
        break;
    case 9:
        BeginFastPaletteFade(3);
        gBerryBlenderData->gameEndState++;
        break;
    case 10:
        if (!gPaletteFade.active)
        {
            if (gBerryBlenderData->playAgainState == 0)
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
                    gBerryBlenderData->canceledPlayerCmd = 0x8888;
                    gBerryBlenderData->canceledPlayerId = gRecvCmds[3][0];
                    break;
                case 0x9999:
                    gBerryBlenderData->canceledPlayerCmd = 0x9999;
                    gBerryBlenderData->canceledPlayerId = gRecvCmds[3][0];
                    break;
                case 0xAAAA:
                    gBerryBlenderData->canceledPlayerCmd = 0xAAAA;
                    gBerryBlenderData->canceledPlayerId = gRecvCmds[3][0];
                    break;
                }
                gBerryBlenderData->playerContinueResponses[0] = 0x1111;
            }
            else if (gRecvCmds[1][0] == 0x2222)
                gBerryBlenderData->playerContinueResponses[0] = 0x2222;
        }
        if (GetMultiplayerId() == 0 && gBerryBlenderData->playerContinueResponses[0] != 0x1111 && gBerryBlenderData->playerContinueResponses[0] != 0x2222)
        {
            u8 i;
            for (i = 0; i < GetLinkPlayerCount(); i++)
            {
                if (gRecvCmds[0][i] == 0x2FFF)
                {
                    switch (gRecvCmds[1][i])
                    {
                    case 0x8888:
                        gBerryBlenderData->playerContinueResponses[i] = 0x8888;
                        break;
                    case 0x7779:
                        gBerryBlenderData->playerContinueResponses[i] = 0x7779;
                        break;
                    case 0x9999:
                        gBerryBlenderData->playerContinueResponses[i] = 0x9999;
                        break;
                    case 0xAAAA:
                        gBerryBlenderData->playerContinueResponses[i] = 0xAAAA;
                        break;
                    }
                }
            }
            for (i = 0; i < GetLinkPlayerCount(); i++)
            {
                if (gBerryBlenderData->playerContinueResponses[i] == 0)
                    break;
            }
            if (i == GetLinkPlayerCount())
            {
                for (i = 0; i < GetLinkPlayerCount(); i++)
                {
                    if (gBerryBlenderData->playerContinueResponses[i] != 0x7779)
                        break;
                }
                gSendCmd[0] = 0x2FFF;
                if (i == GetLinkPlayerCount())
                    gSendCmd[1] = 0x2222;
                else
                {
                    gSendCmd[1] = 0x1111;
                    gSendCmd[2] = gBerryBlenderData->playerContinueResponses[i];
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
    affineSrc.scrX = 120 - gBerryBlenderData->bg_X;
    affineSrc.scrY = 80 - gBerryBlenderData->bg_Y;
    affineSrc.sx = gBerryBlenderData->centerScale;
    affineSrc.sy = gBerryBlenderData->centerScale;
    affineSrc.alpha = gBerryBlenderData->arrowPos;
    BgAffineSet(&affineSrc, dest, 1);
}

static void UpdateBlenderCenter(void)
{
    gBerryBlenderData->previousArrowPos = gBerryBlenderData->arrowPos;
    gBerryBlenderData->arrowPos += gBerryBlenderData->speed;
    DrawBlenderCenter(&gBerryBlenderData->bgAffineDst);
}

static void ApplyBlenderCenterAffine(void)
{
    REG_BG2PA = gBerryBlenderData->bgAffineDst.pa;
    REG_BG2PB = gBerryBlenderData->bgAffineDst.pb;
    REG_BG2PC = gBerryBlenderData->bgAffineDst.pc;
    REG_BG2PD = gBerryBlenderData->bgAffineDst.pd;
    REG_BG2X = gBerryBlenderData->bgAffineDst.dx;
    REG_BG2Y = gBerryBlenderData->bgAffineDst.dy;
}

static void SetBgPos(void)
{
    REG_BG1HOFS = gBerryBlenderData->bg_X;
    REG_BG1VOFS = gBerryBlenderData->bg_Y;
    REG_BG0HOFS = gBerryBlenderData->bg_X;
    REG_BG0VOFS = gBerryBlenderData->bg_Y;
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
        u8 spriteID;

        rand = gBerryBlenderData->arrowPos + (Random() % 20);

        x = gSineTable[(rand & 0xFF) + 64] / 4;
        y = gSineTable[(rand & 0xFF)] / 4;

        spriteID = CreateSprite(&sSpriteTemplate_Particles, x + 120, y + 80, 1);
        gSprites[spriteID].data[0] = 16 - (Random() % 32);
        gSprites[spriteID].data[1] = 16 - (Random() % 32);

        gSprites[spriteID].callback = SpriteCB_Particle;
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
    gBerryBlenderData->chosenItemId[playerId] = itemId;
    ConvertItemToBlenderBerry(&gBerryBlenderData->blendedBerries[playerId], itemId);
}

void UnusedTask_WaitForBlenderLinkPlayers(u8 taskID)
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
                Text_EraseWindowRect(&gBerryBlenderData->menuWindow, 0, 0, 16, 20);
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
                DestroyTask(taskID);
            }
        }
        else
        {
            if (task->data[0] == 10)
                Menu_PrintText(gOtherText_PleaseWait, 3, 10);
            if (++task->data[0] > 120)
                task->data[0] = 0;
            if (gLinkDummyCommandReceived > 4 && gReceivedRemoteLinkPlayers == 1)
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
            gBerryBlenderData->mainState++;
            DestroySprite(sprite);
        }
        break;
    }
    sprite->y2 = sprite->data[1];
}

static void TryUpdateProgressBar(u16 a0, u16 a1)
{
    if (gBerryBlenderData->maxProgressBarValue < a0)
    {
        gBerryBlenderData->maxProgressBarValue += 2;
        UpdateProgressBar(gBerryBlenderData->maxProgressBarValue, a1);
    }
}

static void UpdateProgressBar(u16 a0, u16 a1)
{
    s32 var1, var2, var3, var4;
    u16* vram;

    vram = (u16*)(VRAM + 0x6000);
    var1 = (a0 * 64) / a1;
    var2 = var1 / 8;
    for (var4 = 0; var4 < var2; var4++)
    {
        vram[11 + var4] = 0x81E9;
        vram[43 + var4] = 0x81F9;
    }
    var3 = var1 % 8;
    if (var3 != 0)
    {
        vram[11 + var4] = var3 - 32287;
        vram[43 + var4] = var3 - 32271;
        var4++;
    }
    for (; var4 < 8; var4++)
    {
        vram[11 + var4] = 33249;
        vram[43 + var4] = 33249 + 16;
    }
}

static u32 ArrowSpeedToRPM(u16 a0)
{
    return 360000 * a0 / 0x10000;
}

static void UpdateRPM(u16 a0)
{
    u8 i;
    u8 palAdders[5];

    u32 var = ArrowSpeedToRPM(a0);
    if (gBerryBlenderData->maxRPM < var)
        gBerryBlenderData->maxRPM = var;
    for (i = 0; i < 5; i++)
    {
        palAdders[i] = var % 10;
        var /= 10;
    }
    *((u16*)(VRAM + 0x6458)) = palAdders[4] + 0x8172;
    *((u16*)(VRAM + 0x645A)) = palAdders[3] + 0x8172;
    *((u16*)(VRAM + 0x645C)) = palAdders[2] + 0x8172;
    *((u16*)(VRAM + 0x6460)) = palAdders[1] + 0x8172;
    *((u16*)(VRAM + 0x6462)) = palAdders[0] + 0x8172;
}

static void ShakeBgCoordForHit(s16* a0, u16 a1)
{
    if (*a0 == 0)
        *a0 = (Random() % a1) - (a1 / 2);
}

static void RestoreBgCoord(s16* a0)
{
    if (*a0 < 0 )
        (*a0)++;
    if (*a0 > 0 )
        (*a0)--;
}

static void RestoreBgCoords(void)
{
    RestoreBgCoord(&gBerryBlenderData->bg_X);
    RestoreBgCoord(&gBerryBlenderData->bg_Y);
}

static void BlenderLandShakeBgCoord(s16* a0, u16 a1)
{
    s32 var;
    if (a1 < 10)
        var = 16;
    else
        var = 8;
    if (*a0 == 0)
        *a0 = (Random() % var) - (var / 2);
    else
    {
        if (*a0 < 0)
            (*a0)++;
        if (*a0 > 0)
            (*a0)--;
    }
}

static bool8 UpdateBlenderLandScreenShake(void)
{
    if (gBerryBlenderData->framesToWait == 0)
    {
        gBerryBlenderData->bg_X = 0;
        gBerryBlenderData->bg_Y = 0;
    }
    gBerryBlenderData->framesToWait++;
    BlenderLandShakeBgCoord(&gBerryBlenderData->bg_X, gBerryBlenderData->framesToWait);
    BlenderLandShakeBgCoord(&gBerryBlenderData->bg_Y, gBerryBlenderData->framesToWait);
    if (gBerryBlenderData->framesToWait == 20)
    {
        gBerryBlenderData->bg_X = 0;
        gBerryBlenderData->bg_Y = 0;
        return TRUE;
    }
    else
        return FALSE;
}

static void SpriteCB_PlayerArrow(struct Sprite* sprite)
{
   sprite->x2 = -(gBerryBlenderData->bg_X);
   sprite->y2 = -(gBerryBlenderData->bg_Y);
}

/*static*/ void TryUpdateBerryBlenderRecord(void)
{
    if (gSaveBlock1.berryBlenderRecords[gBerryBlenderData->numPlayers - 2] < gBerryBlenderData->maxRPM)
        gSaveBlock1.berryBlenderRecords[gBerryBlenderData->numPlayers - 2] = gBerryBlenderData->maxRPM;
}

bool8 PrintBlendingResults(void)
{
    u16 i;

    struct Pokeblock pokeblock;
    u8 flavors[6];
    u8 text[2][10];
    u16 berryIDs[4]; // unused

    switch (gBerryBlenderData->mainState)
    {
    case 0:
        gBerryBlenderData->mainState++;
        gBerryBlenderData->framesToWait = 17;
        break;
    case 1:
        gBerryBlenderData->framesToWait -= 10;
        if (gBerryBlenderData->framesToWait < 0)
        {
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->mainState++;
        }
        break;
    case 2:
        if (++gBerryBlenderData->framesToWait > 20)
        {
            for (i = 0; i < 3; i++)
                DestroySprite(&gSprites[gBerryBlenderData->scoreIconIds[i]]);
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->mainState++;
        }
        break;
    case 3:
        {
            u8* textPtr;
            u16 secondsPassed, minutes, seconds;

            Menu_DrawStdWindowFrame(4, 2, 25, 17);
            MenuPrint_Centered(gOtherText_ResultsOfBlending, 5, 3, 160);
            for (i = 0; i < gBerryBlenderData->numPlayers; i++)
            {
                u8 place = gBerryBlenderData->playerPlaces[i];
                textPtr = text[0];

                StringCopy(textPtr, gBerryBlenderData->blendedBerries[place].name);
                ConvertInternationalString(textPtr, gLinkPlayers[place].language);
#ifdef ENGLISH
                StringAppend(textPtr, gOtherText_Berry);
#else
                GermanStringFormatName(textPtr, gOtherText_Berry);
#endif
                textPtr = gBerryBlenderData->stringVar;
                textPtr = ConvertIntToDecimalStringSimple(textPtr, i + 1);
                textPtr[0] = CHAR_SPACE;
                textPtr[1] = CHAR_PERIOD;
                textPtr[2] = CHAR_SPACE;
                textPtr += 3;
                textPtr = AlignStringInMenuWindow(textPtr, gLinkPlayers[place].name, 88, 0);
                AlignStringInMenuWindow(textPtr, text[0], 157, 0);
                Menu_PrintText(gBerryBlenderData->stringVar, 5, sResultsPlayerNameStartY[gBerryBlenderData->numPlayers] + (i * sResultsPlayerNameSpacingY[gBerryBlenderData->numPlayers]));
            }
            ConvertIntToDecimalStringN(text[0], gBerryBlenderData->maxRPM % 100, 2, 2);
            textPtr = gBerryBlenderData->stringVar;
            textPtr = StringCopy(textPtr, gOtherText_MaxSpeed);
            textPtr = AlignInt1InMenuWindow(textPtr, gBerryBlenderData->maxRPM / 100, 121, 1);

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
            Menu_PrintText(gBerryBlenderData->stringVar, 5, 13);

            secondsPassed = gBerryBlenderData->gameFrameTime / 60;
            seconds = secondsPassed % 60;
            minutes = secondsPassed / 60;
            ConvertIntToDecimalStringN(text[0], minutes, 2, 2);
            ConvertIntToDecimalStringN(text[1], seconds, 2, 2);
            textPtr = gBerryBlenderData->stringVar;
            textPtr = StringCopy(textPtr, gOtherText_RequiredTime);

#ifdef ENGLISH
            textPtr = AlignStringInMenuWindow(textPtr, text[0], 102, 1);
#else
            textPtr = AlignStringInMenuWindow(textPtr, text[0], 99, 1);
#endif
            textPtr = StringAppend(textPtr, gOtherText_Min);

            textPtr = AlignStringInMenuWindow(textPtr, text[1], 136, 1);
            StringCopy(textPtr, gOtherText_Sec);

            Menu_PrintText(gBerryBlenderData->stringVar, 5, 15);

            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->mainState++;
        }
        break;
    case 4:
        if (JOY_NEW(A_BUTTON))
            gBerryBlenderData->mainState++;
        break;
    case 5:
        Menu_EraseScreen();
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
        {
            if (gBerryBlenderData->chosenItemId[i] != 0)
                berryIDs[i] = gBerryBlenderData->chosenItemId[i] - 133;
        }
        Debug_SetStageVars();
        CalculatePokeblock(gBerryBlenderData->blendedBerries, &pokeblock, gBerryBlenderData->numPlayers, flavors, gBerryBlenderData->maxRPM);
        PrintMadePokeblockString(&pokeblock, gBerryBlenderData->stringVar);
        CreateTask(Task_PlayPokeblockFanfare, 6);
#if DEBUG
        ConvertIntToHexStringN(text[0], GetBerryBlenderKeySendAttempts(), 0, 4);
        StringAppend(text[0], sText_NewParagraph);
        StringAppend(gBerryBlenderData->stringVar, text[0]);
#endif
        MenuPrintMessage(gBerryBlenderData->stringVar, 1, 15);
        RemoveBagItem(gSpecialVar_ItemId, 1);
        AddPokeblock(&pokeblock);
        gBerryBlenderData->mainState++;
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
    u8 flavourLvl, feel;

    dst[0] = EOS;
    StringCopy(dst, gPokeblockNames[pokeblock->color]);
#ifdef ENGLISH
    StringAppend(dst, gOtherText_PokeBlockMade);
#else
    GermanStringFormatName(dst, gOtherText_PokeBlockMade);
#endif
    StringAppend(dst, sNewLineString_0);

    flavourLvl = GetHighestPokeblocksFlavorLevel(pokeblock);
    feel = GetPokeblocksFeel(pokeblock);

    StringAppend(dst, gOtherText_BlockLevelIs);
    ConvertIntToDecimalStringN(text, flavourLvl, 0, 3);
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

    for (i = 0; i < gBerryBlenderData->numPlayers; i++)
        places[i] = i;
    for (i = 0; i < gBerryBlenderData->numPlayers; i++)
    {
        points[i] = 1000000 * gBerryBlenderData->scores[i][BLENDER_SCORE_BEST];
        points[i] += 1000 * gBerryBlenderData->scores[i][BLENDER_SCORE_GOOD];
        points[i] += 1000 - gBerryBlenderData->scores[i][BLENDER_SCORE_MISS];
    }
    SortBasedOnPoints(places, gBerryBlenderData->numPlayers, points);
    for (i = 0; i < gBerryBlenderData->numPlayers; i++)
        gBerryBlenderData->playerPlaces[i] = places[i];
}

static bool8 PrintBlendingRanking(void)
{
    u16 i;
    switch (gBerryBlenderData->mainState)
    {
    case 0:
        gBerryBlenderData->mainState++;
        gBerryBlenderData->framesToWait = 255;
        break;
    case 1:
        gBerryBlenderData->framesToWait -= 10;
        if (gBerryBlenderData->framesToWait < 0)
        {
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->mainState++;
        }
        break;
    case 2:
        if (++gBerryBlenderData->framesToWait > 20)
        {
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->mainState++;
        }
        break;
    case 3:
        Menu_DrawStdWindowFrame(4, 2, 25, 17);
        MenuPrint_Centered(gOtherText_Ranking, 5, 3, 160);

        gBerryBlenderData->scoreIconIds[BLENDER_SCORE_BEST] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 140, 52, 0);
        gSprites[gBerryBlenderData->scoreIconIds[BLENDER_SCORE_BEST]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[gBerryBlenderData->scoreIconIds[BLENDER_SCORE_BEST]], 3);

        gBerryBlenderData->scoreIconIds[BLENDER_SCORE_GOOD] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 164, 52, 0);
        gSprites[gBerryBlenderData->scoreIconIds[BLENDER_SCORE_GOOD]].callback = SpriteCallbackDummy;

        gBerryBlenderData->scoreIconIds[BLENDER_SCORE_MISS] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 188, 52, 0);
        gSprites[gBerryBlenderData->scoreIconIds[BLENDER_SCORE_MISS]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[gBerryBlenderData->scoreIconIds[BLENDER_SCORE_MISS]], 1);

        SortScores();

        for (i = 0; i < gBerryBlenderData->numPlayers; i++)
        {
            u8 place = gBerryBlenderData->playerPlaces[i];
            u8* txtPtr = gBerryBlenderData->stringVar;

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

            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlenderData->scores[place][BLENDER_SCORE_BEST], 108, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlenderData->scores[place][BLENDER_SCORE_GOOD], 132, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlenderData->scores[place][BLENDER_SCORE_MISS], 156, 1);

            Menu_PrintText(gBerryBlenderData->stringVar, 5, i * sRankingPlayerNameSpacingY[gBerryBlenderData->numPlayers] + 8);
        }
        gBerryBlenderData->framesToWait = 0;
        gBerryBlenderData->mainState++;
        break;
    case 4:
        if (++gBerryBlenderData->framesToWait > 20)
            gBerryBlenderData->mainState++;
        break;
    case 5:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBerryBlenderData->mainState++;
        }
        break;
    case 6:
        gBerryBlenderData->mainState = 0;
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

static void Task_PlayPokeblockFanfare(u8 taskID)
{
    if (gTasks[taskID].data[0] == 0)
    {
        PlayFanfare(MUS_LEVEL_UP);
        gTasks[taskID].data[0]++;
    }
    if (IsFanfareTaskInactive())
    {
        PlayBGM(gBerryBlenderData->savedMusic);
        DestroyTask(taskID);
    }
}
