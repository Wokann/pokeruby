#include "global.h"
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

struct BerryBlenderData
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
void m4aMPlayPitchControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s16 pitch);
void m4aMPlayTempoControl(struct MusicPlayerInfo *mplayInfo, u16 tempo);
void m4aMPlayStop(struct MusicPlayerInfo *mplayInfo);
void MenuCursor_SetPos814A880(u8 a1, u8 a2);
u8 MenuCursor_Create814A5C0(u8 a1, u16 a2, u8 a3, u16 a4, u8 a5);
s8 GetFirstFreePokeblockSlot(void);
#ifdef GERMAN
extern void de_sub_8073110();
#endif

extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u8 gUnknown_020297ED;
extern u8 byte_3002A68;

extern const struct WindowTemplate gWindowTemplate_81E6F68;
extern const u8 *const gPokeblockNames[];


// ewram
static EWRAM_DATA u8 gUnknown_020297DC = 0;
static EWRAM_DATA u32 gUnknown_020297E0 = 0;
static EWRAM_DATA u32 gUnknown_020297E4 = 0;
static EWRAM_DATA u8 gUnknown_020297E8 = 0;

// iwram common
u16 gUnknown_03004830;
u8 gInGameOpponentsNo;
u16 gUnknown_03004840[10];
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

static void sub_80514A4(void);
static void sub_80514F0(void);
static void sub_804E56C(void);
static void Blender_SetPlayerNamesLocal(u8 opponentsNum);
static void sub_8051474(void);
static void sub_804E9F8(void);
static void sub_804F378(void);
static void sub_8051414(struct BgAffineDstData *dest);
static void SetPlayerIdMaps(void);
static void sub_80501FC(void);
static bool8 UpdateBlenderLandScreenShake(void);
static void sub_804F2A8(void);
static void ResetLinkCmds(void);
static void CreateParticleSprites(void);
static void SpriteCB_ScoreSymbolBest(struct Sprite* sprite);
static void ShakeBgCoordForHit(s16* coord, u16 speed);
static void TryUpdateProgressBar(u16 current, u16 limit);
static void sub_8051A3C(u16 speed);
static void RestoreBgCoords(void);
static void sub_805123C(void);
static void sub_8050954(void);
static bool8 Blender_PrintBlendingRanking(void);
bool8 Blender_PrintBlendingResults(void);
static void sub_80510E8(void);
static void sub_8050E30(void);
static void sub_805197C(u16 value, u16 limit);
static void Blender_PrintMadePokeblockString(struct Pokeblock* pokeblock, u8* dst);
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
    m4aMPlayPitchControl(&gMPlayInfo_SE2, 0xFFFF, (gBerryBlenderData->speed - 128) * 2);
}

static void VBlankCB0_BerryBlender(void)
{
    sub_80514A4();
    sub_80514F0();
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

static bool8 sub_804E2EC(void)
{
    switch (gBerryBlenderData->loadGfxState)
    {
    case 0:
        LZDecompressWram(gBerryBlenderCenter_Gfx, eBerryBlenderGfxBuffer);
        gBerryBlenderData->loadGfxState++;
        break;
    case 1:
        DmaCopy16Defvars(3, sBlenderCenter_Tilemap, (void *)(VRAM + 0x4000), 0x400);
        LoadPalette(sBlenderCenter_Pal, 0, 0x100);
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
            LoadPalette(sBlenderOuter_Pal, 0x80, 0x20);
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

static void sub_804E4FC(void)
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

    Blender_SetPlayerNamesLocal(gSpecialVar_0x8004);
    SetMainCallback2(sub_804E56C);
}

static void sub_804E56C(void)
{
    s32 i;
    switch (gBerryBlenderData->mainState)
    {
    case 0:
        REG_DISPCNT = 0;
        ResetSpriteData();
        FreeAllSpritePalettes();
        SetVBlankCallback(NULL);
        Text_LoadWindowTemplate(&gWindowTemplate_81E6F68);
        InitMenuWindow(&gWindowTemplate_81E6F68);
        gBerryBlenderData->mainState++;
        gBerryBlenderData->maxProgressBarValue = 0;
        gBerryBlenderData->progressBarValue = 0;
        gBerryBlenderData->centerScale = 0x50;
        gBerryBlenderData->bg_X = 0;
        gBerryBlenderData->bg_Y = 0;
        gBerryBlenderData->loadGfxState = 0;
        sub_8051474();
        break;
    case 1:
        if (sub_804E2EC())
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
        sub_8051474();
        gBerryBlenderData->mainState++;
        break;
    case 3:
        sub_804E4FC();
        if (!gPaletteFade.active)
        {
            gBerryBlenderData->mainState++;
        }
        break;
    case 4:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        MenuPrintMessage(sText_BerryBlenderStart, 1, 15);
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
            ChooseBerryForMachine();
            gBerryBlenderData->mainState = 0;
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

static void Blender_SetPlayerNamesLocal(u8 opponentsNum)
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

void sub_804E990(void)
{
    s32 i;

    REG_DISPCNT = 0;
    gBerryBlenderData = eBerryBlenderData;
    gBerryBlenderData->mainState = 0;
    gBerryBlenderData->field_134 = 0;
    for (i = 0; i < BLENDER_MAX_PLAYERS; i++)
    {
        gBerryBlenderData->chosenItemId[i] = 0;
    }
    Blender_SetPlayerNamesLocal(gSpecialVar_0x8004);
    if (gSpecialVar_0x8004 == 0)
        SetMainCallback2(sub_804E9F8);
    else
        SetMainCallback2(sub_804F378);
}

static void sub_804E9F8(void)
{
    int i, j;
    switch (gBerryBlenderData->mainState)
    {
    case 0:
        ResetSpriteData();
        FreeAllSpritePalettes();
        ResetTasks();
        SetVBlankCallback(VBlankCB0_BerryBlender);
        Text_LoadWindowTemplate(&gWindowTemplate_81E6F68);
        InitMenuWindow(&gWindowTemplate_81E6F68);
        gLinkType = 0x4422;
        gBerryBlenderData->mainState++;
        gBerryBlenderData->field_4E = 0;
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
        if (sub_804E2EC())
        {
            gBerryBlenderData->mainState++;
            sub_8051474();
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
        sub_804E4FC();
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
                SendBlockRequest(4);
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
                CreateBerrySprite(gBerryBlenderData->chosenItemId[gBerryBlenderData->playerToThrowBerry], i);
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
            sub_8051414(&gBerryBlenderData->field_168);
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
            sub_804F2A8();
        }
        sub_8051414(&gBerryBlenderData->field_168);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            gBerryBlenderData->framesToWait = 0;
            gBerryBlenderData->mainState++;
        }
        sub_8051414(&gBerryBlenderData->field_168);
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
        SetMainCallback2(sub_80501FC);
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

static void sub_804F0F4(void)
{
    REG_DISPCNT = 0;

    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();

    SetVBlankCallback(VBlankCB0_BerryBlender);

    Text_LoadWindowTemplate(&gWindowTemplate_81E6F68);
    InitMenuWindow(&gWindowTemplate_81E6F68);

    gLinkType = 0x4422;

    gBerryBlenderData->field_4E = 0;
    gBerryBlenderData->speed = 0;
    gBerryBlenderData->arrowPos = 0;
    gBerryBlenderData->maxRPM = 0;
    gBerryBlenderData->bg_X = 0;
    gBerryBlenderData->bg_Y = 0;
    gBerryBlenderData->mainState++;
}

static u8 GetArrowProximity(u16 arrowPos, u8 playerId)
{
    u32 pos = (arrowPos / 256) + 24;
    u8 arrowId = gBerryBlenderData->playerIdToArrowId[playerId];
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

static void sub_804F1BC(u16 playerBerryItemId, u8 playersNum, struct BlenderBerry* playerBerry)
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

static void sub_804F2A8(void)
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
            Menu_PrintTextPixelCoords(gStringVar1, gUnknown_082162D4[i][0] * 8 + 1, gUnknown_082162D4[i][1] * 8, 1);
        }
    }
}

static void sub_804F378(void)
{
    s32 i, j;
    switch (gBerryBlenderData->mainState)
    {
    case 0:
        sub_804F0F4();
        SetPlayerBerryData(0, gSpecialVar_ItemId);
        ConvertItemToBlenderBerry(&gBerryBlenderData->blendedBerries[0], gSpecialVar_ItemId);
        sub_804F1BC(gSpecialVar_ItemId, gBerryBlenderData->numPlayers, &gBerryBlenderData->blendedBerries[0]);
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
        if (sub_804E2EC())
        {
            gBerryBlenderData->mainState++;
            sub_8051474();
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
            sub_804E4FC();
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
            u32 playerId = sPlayerIdMap[gBerryBlenderData->numPlayers - 2][i];
            if (gBerryBlenderData->playerToThrowBerry == playerId)
            {
                CreateBerrySprite(gBerryBlenderData->chosenItemId[gBerryBlenderData->playerToThrowBerry], i);
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
        sub_8051414(&gBerryBlenderData->field_168);
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
            sub_804F2A8();
        }
        sub_8051414(&gBerryBlenderData->field_168);
        break;
    case 15:
        if (UpdateBlenderLandScreenShake())
        {
            gBerryBlenderData->mainState++;
        }
        sub_8051414(&gBerryBlenderData->field_168);
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
        SetMainCallback2(sub_80501FC);

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
    if (GetArrowProximity(gBerryBlenderData->arrowPos, 1) == 2)
    {
        if (gTasks[taskId].data[0] == 0)
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

            gTasks[taskId].data[0] = 1;
        }
    }
    else
        gTasks[taskId].data[0] = 0;
}

static void Task_HandleOpponent2(u8 taskId)
{
    u32 var1 = (gBerryBlenderData->arrowPos + 0x1800) & 0xFFFF;
    u32 arrowId = gBerryBlenderData->playerIdToArrowId[2] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[arrowId] + 20 && (var1 >> 8) < sArrowHitRangeStart[arrowId] + 40)
    {
        if (gTasks[taskId].data[0] == 0)
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

    var1 = (gBerryBlenderData->arrowPos + 0x1800) & 0xFFFF;
    arrowId = gBerryBlenderData->playerIdToArrowId[3] & 0xFF;
    if ((var1 >> 8) > sArrowHitRangeStart[arrowId] + 20 && (var1 >> 8) < sArrowHitRangeStart[arrowId] + 40)
    {
        if (gTasks[taskId].data[0] == 0)
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
        if (gBerryBlenderData->speed < 1500)
            gBerryBlenderData->speed += (384 / sNumPlayersToSpeedDivisor[gBerryBlenderData->numPlayers]);
        else
        {
            gBerryBlenderData->speed += (128 / sNumPlayersToSpeedDivisor[gBerryBlenderData->numPlayers]);
            ShakeBgCoordForHit(&gBerryBlenderData->bg_X, (gBerryBlenderData->speed / 100) - 10);
            ShakeBgCoordForHit(&gBerryBlenderData->bg_Y, (gBerryBlenderData->speed / 100) - 10);
        }
        break;
    case 0x5432:
        if (gBerryBlenderData->speed < 1500)
            gBerryBlenderData->speed += (256 / sNumPlayersToSpeedDivisor[gBerryBlenderData->numPlayers]);
        break;
    case 0x2345:
        gBerryBlenderData->speed -= (256 / sNumPlayersToSpeedDivisor[gBerryBlenderData->numPlayers]);
        if (gBerryBlenderData->speed < 128)
            gBerryBlenderData->speed = 128;
        break;
    }
}

static void sub_804FE70(void)
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
            u32 arrowId = gBerryBlenderData->playerIdToArrowId[i];
            if (gRecvCmds[2][i] == 0x4523)
            {
                UpdateSpeedFromHit(0x4523);
                gBerryBlenderData->progressBarValue += (gBerryBlenderData->speed / 55);
                if (gBerryBlenderData->progressBarValue >= 1000)
                    gBerryBlenderData->progressBarValue = 1000;
                CreateScoreSymbolSprite(0x4523, arrowId);
                gBerryBlenderData->scores[i][SCORE_BEST]++;
            }
            else if (gRecvCmds[2][i] == 0x5432)
            {
                UpdateSpeedFromHit(0x5432);
                gBerryBlenderData->progressBarValue += (gBerryBlenderData->speed / 70);
                CreateScoreSymbolSprite(0x5432, arrowId);
                gBerryBlenderData->scores[i][SCORE_GOOD]++;
            }
            else if (gRecvCmds[2][i] == 0x2345)
            {
                CreateScoreSymbolSprite(0x2345, arrowId);
                UpdateSpeedFromHit(0x2345);
                // looks like a bug
                if (gBerryBlenderData->field_4.win_field_F > 1000)
                    gBerryBlenderData->progressBarValue = 1000;
                if (gBerryBlenderData->scores[i][SCORE_MISS] < 999)
                    gBerryBlenderData->scores[i][SCORE_MISS]++;
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

static void sub_80500A8(void)
{
    bool8 pressedA = 0;
    u8 arrowId = gBerryBlenderData->playerIdToArrowId[GetMultiplayerId()];
    if (gBerryBlenderData->gameEndState == 0)
    {
        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A && JOY_NEW(A_BUTTON))
            pressedA = ((gMain.heldKeysRaw & (A_BUTTON | L_BUTTON)) != (A_BUTTON | L_BUTTON));
        else if (JOY_NEW(A_BUTTON))
            pressedA = 1;
        if (pressedA)
        {
            u8 proximity;
            StartSpriteAnim(&gSprites[gBerryBlenderData->playerArrowSpriteIds[gBerryBlenderData->arrowIdToPlayerId[arrowId]]], arrowId + 4);
            proximity = GetArrowProximity(gBerryBlenderData->arrowPos, GetMultiplayerId());
            if (proximity == 2)
                gSendCmd[2] = 0x4523;
            else if (proximity == 1)
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
    if (gUnknown_020297ED && JOY_NEW(L_BUTTON))
        gBerryBlenderData->perfectOpponents ^= 1;
}

static void sub_80501FC(void)
{
    sub_8051474();
    if (gBerryBlenderData->gameFrameTime < (99 * 60 * 60) + (59 * 60)) // game time can't be longer than 99 minutes and 59 seconds, can't print 3 digits
        gBerryBlenderData->gameFrameTime++;
    sub_80500A8();
    SetLinkDebugValues((u16)(gBerryBlenderData->speed), gBerryBlenderData->progressBarValue);
    sub_804FE70();
    TryUpdateProgressBar(gBerryBlenderData->progressBarValue, 1000);
    sub_8051A3C(gBerryBlenderData->speed);
    RestoreBgCoords();
    sub_805123C();
    if (gBerryBlenderData->gameEndState == 0 && gBerryBlenderData->maxProgressBarValue >= 1000)
    {
        gBerryBlenderData->progressBarValue = 1000;
        gBerryBlenderData->gameEndState = 1;
        SetMainCallback2(sub_8050954);
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

u32 Blender_GetPokeblockColor(struct BlenderBerry* berries, s16* _flavors, u8 numPlayers, u8 negativeFlavors)
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
                return (sPokeblockPresentFlavors[1] << 16) | PBLOCK_CLR_LITEBLUE;
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
                return (sPokeblockPresentFlavors[0] << 16) | PBLOCK_CLR_LITEBLUE;
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
    pokeblock->color = Blender_GetPokeblockColor(berries, &sPokeblockFlavors[0], numPlayers, numNegatives);
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
    u32 frames = (u16)(gBerryBlenderData->gameFrameTime);
    u16 maxRPM = gBerryBlenderData->maxRPM;
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
    gBerryBlenderData->yesNoAnswer = value;
    MenuCursor_SetPos814A880(192, (gBerryBlenderData->yesNoAnswer * 16) + 72);
}

static void sub_80508FC(void)
{
    gBerryBlenderData->yesNoAnswer = 0;
    Menu_DrawStdWindowFrame(23, 8, 28, 13);
    MenuCursor_Create814A5C0(0, -1, 12, 0x2D9F, 32);
    Menu_PrintText(gOtherText_YesNoTerminating, 24, 9);
    sub_80508D4(gBerryBlenderData->yesNoAnswer);
}

static void sub_8050954(void)
{
    u8 i;
    u8 multiplayerID; // unused

    sub_8051474();
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
            gBerryBlenderData->field_1BC = gBerryBlenderData->gameFrameTime;
            gBerryBlenderData->field_1C0 = gBerryBlenderData->maxRPM;
            SendBlock(0, &gBerryBlenderData->field_1BC, 40);
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
        if (Blender_PrintBlendingRanking())
            gBerryBlenderData->gameEndState++;
        break;
    case 6:
        if (Blender_PrintBlendingResults())
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
        MenuPrintMessage(sText_WouldLikeToBlendAnotherBerry, 1, 15);
        break;
    case 8:
        if (Menu_UpdateWindowText())
            gBerryBlenderData->gameEndState++;
        break;
    case 9:
        gBerryBlenderData->yesNoAnswer = 0;
        sub_80508FC();
        gBerryBlenderData->gameEndState++;
        break;
    case 10:
        if (JOY_NEW(DPAD_UP))
        {
            if (gBerryBlenderData->yesNoAnswer != 0)
                PlaySE(SE_SELECT);
            sub_80508D4(0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            if (gBerryBlenderData->yesNoAnswer != 1)
                PlaySE(SE_SELECT);
            sub_80508D4(1);
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
            sub_80508D4(1);
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
            SetMainCallback2(sub_80510E8);
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
            SetMainCallback2(sub_8050E30);
            gBerryBlenderData->gameEndState = 0;
            gBerryBlenderData->mainState = 0;
        }
        break;
    }
    RestoreBgCoords();
    sub_8051A3C(gBerryBlenderData->speed);
    sub_805123C();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

bool8 sub_8050CE8(void)
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
        sub_8125E2C();
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
            if (sub_8125E6C())
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

static void sub_8050E30(void)
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
        StringAppend(gStringVar4, sText_ApostropheSPokeblockCaseIsFull);
#else
        StringCopy(gStringVar4, sText_ApostropheSPokeblockCaseIsFull);
        de_sub_8073110(gStringVar4, gLinkPlayers[gBerryBlenderData->canceledPlayerId].name);
#endif
        MenuPrintMessage(gStringVar4, 1, 15);
        break;
    case 2:
        gBerryBlenderData->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
#ifdef ENGLISH
        StringCopy(gStringVar4, gLinkPlayers[gBerryBlenderData->canceledPlayerId].name);
        StringAppend(gStringVar4, sText_HasNoBerriesToPut);
#else
        StringCopy(gStringVar4, sText_HasNoBerriesToPut);
        de_sub_8073110(gStringVar4, gLinkPlayers[gBerryBlenderData->canceledPlayerId].name);
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
        if (sub_8050CE8())
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

    sub_805123C();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_80510E8(void)
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
        MenuPrintMessage(sText_YourPokeblockCaseIsFull, 1, 15);
        break;
    case 2:
        gBerryBlenderData->gameEndState++;
        DestroyMenuCursor();
        Menu_EraseWindowRect(23, 8, 28, 13);
        MenuPrintMessage(sText_RunOutOfBerriesForBlending, 1, 15);
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

    sub_805123C();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_805123C(void)
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

static void sub_8051414(struct BgAffineDstData *dest)
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

static void sub_8051474(void)
{
    gBerryBlenderData->field_58 = gBerryBlenderData->arrowPos;
    gBerryBlenderData->arrowPos += gBerryBlenderData->speed;
    sub_8051414(&gBerryBlenderData->field_168);
}

static void sub_80514A4(void)
{
    REG_BG2PA = gBerryBlenderData->field_168.pa;
    REG_BG2PB = gBerryBlenderData->field_168.pb;
    REG_BG2PC = gBerryBlenderData->field_168.pc;
    REG_BG2PD = gBerryBlenderData->field_168.pd;
    REG_BG2X = gBerryBlenderData->field_168.dx;
    REG_BG2Y = gBerryBlenderData->field_168.dy;
}

static void sub_80514F0(void)
{
    REG_BG1HOFS = gBerryBlenderData->bg_X;
    REG_BG1VOFS = gBerryBlenderData->bg_Y;
    REG_BG0HOFS = gBerryBlenderData->bg_X;
    REG_BG0VOFS = gBerryBlenderData->bg_Y;
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

        rand = gBerryBlenderData->arrowPos + (Random() % 20);

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
    gBerryBlenderData->chosenItemId[playerId] = itemId;
    ConvertItemToBlenderBerry(&gBerryBlenderData->blendedBerries[playerId], itemId);
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
                Text_EraseWindowRect(&gBerryBlenderData->field_4, 0, 0, 16, 20);
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
            if (byte_3002A68 > 4 && gReceivedRemoteLinkPlayers == 1)
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

static void TryUpdateProgressBar(u16 current, u16 limit)
{
    if (gBerryBlenderData->maxProgressBarValue < current)
    {
        gBerryBlenderData->maxProgressBarValue += 2;
        sub_805197C(gBerryBlenderData->maxProgressBarValue, limit);
    }
}

static void sub_805197C(u16 value, u16 limit)
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

static void sub_8051A3C(u16 speed)
{
    u8 i;
    u8 digits[5];

    u32 currentRPM = ArrowSpeedToRPM(speed);
    if (gBerryBlenderData->maxRPM < currentRPM)
        gBerryBlenderData->maxRPM = currentRPM;
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
    RestoreBgCoord(&gBerryBlenderData->bg_X);
    RestoreBgCoord(&gBerryBlenderData->bg_Y);
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

static void TryUpdateBerryBlenderRecord(void)
{
    if (gSaveBlock1.berryBlenderRecords[gBerryBlenderData->numPlayers - 2] < gBerryBlenderData->maxRPM)
        gSaveBlock1.berryBlenderRecords[gBerryBlenderData->numPlayers - 2] = gBerryBlenderData->maxRPM;
}

bool8 Blender_PrintBlendingResults(void)
{
    u16 i;

    struct Pokeblock pokeblock;
    u8 flavors[6];
    u8 text[2][10];
    u16 berryIds[4]; // unused

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
            u8* txtPtr;
            u16 secondsPassed, minutes, seconds;

            Menu_DrawStdWindowFrame(4, 2, 25, 17);
            MenuPrint_Centered(sText_BlendingResults, 5, 3, 160);
            for (i = 0; i < gBerryBlenderData->numPlayers; i++)
            {
                u8 place = gBerryBlenderData->playerPlaces[i];
                txtPtr = text[0];

                StringCopy(txtPtr, gBerryBlenderData->blendedBerries[place].name);
                ConvertInternationalString(txtPtr, gLinkPlayers[place].language);
#ifdef ENGLISH
                StringAppend(txtPtr, sText_SpaceBerry);
#else
                de_sub_8073174(txtPtr, sText_SpaceBerry);
#endif
                txtPtr = gBerryBlenderData->stringVar;
                txtPtr = ConvertIntToDecimalStringSimple(txtPtr, i + 1);
                txtPtr[0] = CHAR_SPACE;
                txtPtr[1] = CHAR_PERIOD;
                txtPtr[2] = CHAR_SPACE;
                txtPtr += 3;
                txtPtr = AlignStringInMenuWindow(txtPtr, gLinkPlayers[place].name, 88, 0);
                AlignStringInMenuWindow(txtPtr, text[0], 157, 0);
                Menu_PrintText(gBerryBlenderData->stringVar, 5, gUnknown_082165E9[gBerryBlenderData->numPlayers] + (i * gUnknown_082165EE[gBerryBlenderData->numPlayers]));
            }
            ConvertIntToDecimalStringN(text[0], gBerryBlenderData->maxRPM % 100, 2, 2);
            txtPtr = gBerryBlenderData->stringVar;
            txtPtr = StringCopy(txtPtr, sText_MaximumSpeed);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlenderData->maxRPM / 100, 121, 1);

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
            Menu_PrintText(gBerryBlenderData->stringVar, 5, 13);

            secondsPassed = gBerryBlenderData->gameFrameTime / 60;
            seconds = secondsPassed % 60;
            minutes = secondsPassed / 60;
            ConvertIntToDecimalStringN(text[0], minutes, 2, 2);
            ConvertIntToDecimalStringN(text[1], seconds, 2, 2);
            txtPtr = gBerryBlenderData->stringVar;
            txtPtr = StringCopy(txtPtr, sText_Time);

#ifdef ENGLISH
            txtPtr = AlignStringInMenuWindow(txtPtr, text[0], 102, 1);
#else
            txtPtr = AlignStringInMenuWindow(txtPtr, text[0], 99, 1);
#endif
            txtPtr = StringAppend(txtPtr, sText_Min);

            txtPtr = AlignStringInMenuWindow(txtPtr, text[1], 136, 1);
            StringCopy(txtPtr, sText_Sec);

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
                berryIds[i] = gBerryBlenderData->chosenItemId[i] - 133;
        }
        Debug_SetStageVars();
        CalculatePokeblock(gBerryBlenderData->blendedBerries, &pokeblock, gBerryBlenderData->numPlayers, flavors, gBerryBlenderData->maxRPM);
        Blender_PrintMadePokeblockString(&pokeblock, gBerryBlenderData->stringVar);
        CreateTask(Task_PlayPokeblockFanfare, 6);
#if DEBUG
        ConvertIntToHexStringN(text[0], GetBerryBlenderKeySendAttempts(), 0, 4);
        StringAppend(text[0], sText_NewParagraph);
        StringAppend(gBerryBlenderData->stringVar, text[0]);
#endif
        MenuPrintMessage(gBerryBlenderData->stringVar, 1, 15);
        RemoveBagItem(gSpecialVar_ItemId, 1);
        GivePokeblock(&pokeblock);
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

static void Blender_PrintMadePokeblockString(struct Pokeblock* pokeblock, u8* dst)
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

    flavorLvl = sub_810C9B0(pokeblock);
    feel = sub_810C9E8(pokeblock);

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

static void Blender_SortScores(void)
{
    u8 i;
    u8 places[4];
    u32 points[4];

    for (i = 0; i < gBerryBlenderData->numPlayers; i++)
        places[i] = i;
    for (i = 0; i < gBerryBlenderData->numPlayers; i++)
    {
        points[i] = 1000000 * gBerryBlenderData->scores[i][SCORE_BEST];
        points[i] += 1000 * gBerryBlenderData->scores[i][SCORE_GOOD];
        points[i] += 1000 - gBerryBlenderData->scores[i][SCORE_MISS];
    }
    SortBasedOnPoints(places, gBerryBlenderData->numPlayers, points);
    for (i = 0; i < gBerryBlenderData->numPlayers; i++)
        gBerryBlenderData->playerPlaces[i] = places[i];
}

static bool8 Blender_PrintBlendingRanking(void)
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
        MenuPrint_Centered(sText_Ranking, 5, 3, 160);

        gBerryBlenderData->scoreIconIds[SCORE_BEST] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 140, 52, 0);
        gSprites[gBerryBlenderData->scoreIconIds[SCORE_BEST]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[gBerryBlenderData->scoreIconIds[SCORE_BEST]], 3);

        gBerryBlenderData->scoreIconIds[SCORE_GOOD] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 164, 52, 0);
        gSprites[gBerryBlenderData->scoreIconIds[SCORE_GOOD]].callback = SpriteCallbackDummy;

        gBerryBlenderData->scoreIconIds[SCORE_MISS] = CreateSprite(&sSpriteTemplate_ScoreSymbols, 188, 52, 0);
        gSprites[gBerryBlenderData->scoreIconIds[SCORE_MISS]].callback = SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[gBerryBlenderData->scoreIconIds[SCORE_MISS]], 1);

        Blender_SortScores();

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

            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlenderData->scores[place][SCORE_BEST], 108, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlenderData->scores[place][SCORE_GOOD], 132, 1);
            txtPtr = AlignInt1InMenuWindow(txtPtr, gBerryBlenderData->scores[place][SCORE_MISS], 156, 1);

            Menu_PrintText(gBerryBlenderData->stringVar, 5, i * gUnknown_082165F3[gBerryBlenderData->numPlayers] + 8);
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
        PlayBGM(gBerryBlenderData->savedMusic);
        DestroyTask(taskId);
    }
}
