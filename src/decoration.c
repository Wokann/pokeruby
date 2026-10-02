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
const u8 DecorDesc_SMALL_DESK[] = _(
    "A small desk built\n"
    "for one.");

const u8 DecorDesc_POKEMON_DESK[] = _(
    "A small desk built in\n"
    "the shape of a POKé\n"
    "BALL.");

const u8 DecorDesc_HEAVY_DESK[] = _(
    "A large desk made\n"
    "of steel. Put some\n"
    "decorations on it.");

const u8 DecorDesc_RAGGED_DESK[] = _(
    "A large desk made\n"
    "of wood. Put some\n"
    "decorations on it.");

const u8 DecorDesc_COMFORT_DESK[] = _(
    "A large desk made\n"
    "of leaves. Put some\n"
    "decorations on it.");

const u8 DecorDesc_PRETTY_DESK[] = _(
    "A huge desk made\n"
    "of glass. Holds lots\n"
    "of decorations.");

const u8 DecorDesc_BRICK_DESK[] = _(
    "A huge desk made\n"
    "of brick. Holds lots\n"
    "of decorations.");

const u8 DecorDesc_CAMP_DESK[] = _(
    "A huge desk made\n"
    "of logs. Put lots of\n"
    "decorations on it.");

const u8 DecorDesc_HARD_DESK[] = _(
    "A huge desk made\n"
    "of rocks. Holds\n"
    "many decorations.");

const u8 DecorDesc_SMALL_CHAIR[] = _(
    "A small chair made\n"
    "for one.");

const u8 DecorDesc_POKEMON_CHAIR[] = _(
    "A small chair built\n"
    "in the shape of a\n"
    "POKé BALL.");

const u8 DecorDesc_HEAVY_CHAIR[] = _(
    "A small chair made\n"
    "of steel.");

const u8 DecorDesc_PRETTY_CHAIR[] = _(
    "A small chair made\n"
    "of glass.");

const u8 DecorDesc_COMFORT_CHAIR[] = _(
    "A small chair made\n"
    "of leaves.");

const u8 DecorDesc_RAGGED_CHAIR[] = _(
    "A small chair made\n"
    "of wood.");

const u8 DecorDesc_BRICK_CHAIR[] = _(
    "A small chair made\n"
    "of brick.");

const u8 DecorDesc_CAMP_CHAIR[] = _(
    "A small chair made\n"
    "of logs.");

const u8 DecorDesc_HARD_CHAIR[] = _(
    "A small chair made\n"
    "of rock.");

const u8 DecorDesc_RED_PLANT[] = _(
    "A vivid red potted\n"
    "plant.");

const u8 DecorDesc_TROPICAL_PLANT[] = _(
    "A flowering tropical\n"
    "plant in a pot.");

const u8 DecorDesc_PRETTY_FLOWERS[] = _(
    "A pot of cute\n"
    "flowers.");

const u8 DecorDesc_COLORFUL_PLANT[] = _(
    "A large pot with\n"
    "many colorful\n"
    "flowers.");

const u8 DecorDesc_BIG_PLANT[] = _(
    "A large, umbrella-\n"
    "shaped plant in a\n"
    "big pot.");

const u8 DecorDesc_GORGEOUS_PLANT[] = _(
    "A large, impressive\n"
    "plant in a big pot.");

const u8 DecorDesc_RED_BRICK[] = _(
    "A red-colored brick.\n"
    "Decorations can be\n"
    "placed on top.");

const u8 DecorDesc_YELLOW_BRICK[] = _(
    "A yellow-colored\n"
    "brick. Put some\n"
    "decorations on top.");

const u8 DecorDesc_BLUE_BRICK[] = _(
    "A blue-colored\n"
    "brick. Put some\n"
    "decorations on top.");

const u8 DecorDesc_RED_BALLOON[] = _(
    "A red balloon filled\n"
    "with water. Bursts\n"
    "if stepped on.");

const u8 DecorDesc_BLUE_BALLOON[] = _(
    "A blue balloon filled\n"
    "with water. Bursts\n"
    "if stepped on.");

const u8 DecorDesc_YELLOW_BALLOON[] = _(
    "A yellow balloon\n"
    "filled with water.\n"
    "Pops if stepped on.");

const u8 DecorDesc_RED_TENT[] = _(
    "A large red tent.\n"
    "You can hide inside\n"
    "it.");

const u8 DecorDesc_BLUE_TENT[] = _(
    "A large blue tent.\n"
    "You can hide inside\n"
    "it.");

const u8 DecorDesc_SOLID_BOARD[] = _(
    "Place over a hole to\n"
    "cross to the other\n"
    "side.");

const u8 DecorDesc_SLIDE[] = _(
    "Use to slide down\n"
    "from the platform.");

const u8 DecorDesc_FENCE_LENGTH[] = _(
    "A small fence that\n"
    "blocks passage.");

const u8 DecorDesc_FENCE_WIDTH[] = _(
    "A small fence that\n"
    "blocks passage.");

const u8 DecorDesc_TIRE[] = _(
    "An old large tire.\n"
    "Decorations can be\n"
    "placed on top.");

const u8 DecorDesc_STAND[] = _(
    "A large pedestal\n"
    "with steps.");

const u8 DecorDesc_MUD_BALL[] = _(
    "A large ball of mud.\n"
    "Crumbles if stepped\n"
    "on.");

const u8 DecorDesc_BREAKABLE_DOOR[] = _(
    "A weird door that\n"
    "people can walk\n"
    "right through.");

const u8 DecorDesc_SAND_ORNAMENT[] = _(
    "An ornament made\n"
    "of sand. Crumbles if\n"
    "touched.");

const u8 DecorDesc_SILVER_SHIELD[] = _(
    "Awarded for 50\n"
    "straight wins at\n"
    "the BATTLE TOWER.");

const u8 DecorDesc_GOLD_SHIELD[] = _(
    "Awarded for 100\n"
    "straight wins at\n"
    "the BATTLE TOWER.");

const u8 DecorDesc_GLASS_ORNAMENT[] = _(
    "A glass replica of\n"
    "a famous sculpture\n"
    "at the ART MUSEUM.");

const u8 DecorDesc_TV[] = _(
    "A small, gray-\n"
    "colored toy TV.");

const u8 DecorDesc_ROUND_TV[] = _(
    "A toy TV modeled\n"
    "in the image of a\n"
    "SEEDOT.");

const u8 DecorDesc_CUTE_TV[] = _(
    "A toy TV modeled\n"
    "in the image of a\n"
    "SKITTY.");

const u8 DecorDesc_GLITTER_MAT[] = _(
    "An odd mat that\n"
    "glitters if stepped\n"
    "on.");

const u8 DecorDesc_JUMP_MAT[] = _(
    "A trick mat that\n"
    "jumps when it is\n"
    "stepped on.");

const u8 DecorDesc_SPIN_MAT[] = _(
    "A trick mat that\n"
    "spins around when\n"
    "stepped on.");

const u8 DecorDesc_C_LOW_NOTE_MAT[] = _(
    "A mat that plays\n"
    "a low C note when\n"
    "stepped on.");

const u8 DecorDesc_D_NOTE_MAT[] = _(
    "A mat that plays\n"
    "a D note when\n"
    "stepped on.");

const u8 DecorDesc_E_NOTE_MAT[] = _(
    "A mat that plays\n"
    "an E note when\n"
    "stepped on.");

const u8 DecorDesc_F_NOTE_MAT[] = _(
    "A mat that plays\n"
    "an F note when\n"
    "stepped on.");

const u8 DecorDesc_G_NOTE_MAT[] = _(
    "A mat that plays\n"
    "a G note when\n"
    "stepped on.");

const u8 DecorDesc_A_NOTE_MAT[] = _(
    "A mat that plays\n"
    "an A note when\n"
    "stepped on.");

const u8 DecorDesc_B_NOTE_MAT[] = _(
    "A mat that plays\n"
    "a B note when\n"
    "stepped on.");

const u8 DecorDesc_C_HIGH_NOTE_MAT[] = _(
    "A mat that plays\n"
    "a high C note when\n"
    "stepped on.");

const u8 DecorDesc_SURF_MAT[] = _(
    "A mat designed with\n"
    "a SURF image.\n"
    "Put items on top.");

const u8 DecorDesc_THUNDER_MAT[] = _(
    "A mat designed with\n"
    "a THUNDER image.\n"
    "Put items on top.");

const u8 DecorDesc_FIRE_BLAST_MAT[] = _(
    "A mat designed with\n"
    "a FIRE BLAST image.\n"
    "Put items on top.");

const u8 DecorDesc_POWDER_SNOW_MAT[] = _(
    "A mat with a POWDER\n"
    "SNOW image design.\n"
    "Put items on top.");

const u8 DecorDesc_ATTRACT_MAT[] = _(
    "A mat designed with\n"
    "an ATTRACT image.\n"
    "Put items on top.");

const u8 DecorDesc_FISSURE_MAT[] = _(
    "A mat designed with\n"
    "a FISSURE image.\n"
    "Put items on top.");

const u8 DecorDesc_SPIKES_MAT[] = _(
    "A mat designed with\n"
    "a SPIKES image.\n"
    "Put items on top.");

const u8 DecorDesc_BALL_POSTER[] = _(
    "A small poster\n"
    "printed with POKé\n"
    "BALLS.");

const u8 DecorDesc_GREEN_POSTER[] = _(
    "A small poster with\n"
    "a TREECKO print.");

const u8 DecorDesc_RED_POSTER[] = _(
    "A small poster with\n"
    "a TORCHIC print.");

const u8 DecorDesc_BLUE_POSTER[] = _(
    "A small poster with\n"
    "a MUDKIP print.");

const u8 DecorDesc_CUTE_POSTER[] = _(
    "A small poster with\n"
    "an AZURILL print.");

const u8 DecorDesc_PIKA_POSTER[] = _(
    "A large poster with\n"
    "a PIKACHU and\n"
    "PICHU print.");

const u8 DecorDesc_LONG_POSTER[] = _(
    "A large poster with\n"
    "a SEVIPER print.");

const u8 DecorDesc_SEA_POSTER[] = _(
    "A large poster with\n"
    "a RELICANTH print.");

const u8 DecorDesc_SKY_POSTER[] = _(
    "A large poster with\n"
    "a WINGULL print.");

const u8 DecorDesc_KISS_POSTER[] = _(
    "A large poster with\n"
    "a SMOOCHUM print.");

const u8 DecorDesc_PICHU_DOLL[] = _(
    "A PICHU doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_PIKACHU_DOLL[] = _(
    "A PIKACHU doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_MARILL_DOLL[] = _(
    "A MARILL doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_TOGEPI_DOLL[] = _(
#if REVISION >= 1
    "A TOGEPI doll.\n"
#else
    "A TOPGEPI doll.\n"
#endif
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_CYNDAQUIL_DOLL[] = _(
    "A CYNDAQUIL doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_CHIKORITA_DOLL[] = _(
    "A CHIKORITA doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_TOTODILE_DOLL[] = _(
    "A TOTODILE doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_JIGGLYPUFF_DOLL[] = _(
    "A JIGGLYPUFF doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_MEOWTH_DOLL[] = _(
    "A MEOWTH doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_CLEFAIRY_DOLL[] = _(
    "A CLEFAIRY doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_DITTO_DOLL[] = _(
    "A DITTO doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_SMOOCHUM_DOLL[] = _(
    "A SMOOCHUM doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_TREECKO_DOLL[] = _(
    "A TREECKO doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_TORCHIC_DOLL[] = _(
    "A TORCHIC doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_MUDKIP_DOLL[] = _(
    "A MUDKIP doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_DUSKULL_DOLL[] = _(
    "A DUSKULL doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_WYNAUT_DOLL[] = _(
    "A WYNAUT doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_BALTOY_DOLL[] = _(
    "A BALTOY doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_KECLEON_DOLL[] = _(
    "A KECLEON doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_AZURILL_DOLL[] = _(
    "An AZURILL doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_SKITTY_DOLL[] = _(
    "A SKITTY doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_SWABLU_DOLL[] = _(
    "A SWABLU doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_GULPIN_DOLL[] = _(
    "A GULPIN doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_LOTAD_DOLL[] = _(
    "A LOTAD doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_SEEDOT_DOLL[] = _(
    "A SEEDOT doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_PIKA_CUSHION[] = _(
    "A PIKACHU cushion.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_ROUND_CUSHION[] = _(
    "A MARILL cushion.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_KISS_CUSHION[] = _(
    "A SMOOCHUM\n"
    "cushion. Place it on\n"
    "a mat or a desk.");

const u8 DecorDesc_ZIGZAG_CUSHION[] = _(
    "A ZIGZAGOON\n"
    "cushion. Place it on\n"
    "a mat or a desk.");

const u8 DecorDesc_SPIN_CUSHION[] = _(
    "A SPINDA cushion.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_DIAMOND_CUSHION[] = _(
    "A SABLEYE cushion.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_BALL_CUSHION[] = _(
    "A BALL cushion.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_GRASS_CUSHION[] = _(
    "A grass-mark\n"
    "cushion. Place it on\n"
    "a mat or a desk.");

const u8 DecorDesc_FIRE_CUSHION[] = _(
    "A fire-mark\n"
    "cushion. Place it on\n"
    "a mat or a desk.");

const u8 DecorDesc_WATER_CUSHION[] = _(
    "A water-mark\n"
    "cushion. Place it on\n"
    "a mat or a desk.");

const u8 DecorDesc_SNORLAX_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_RHYDON_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_LAPRAS_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_VENUSAUR_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_CHARIZARD_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_BLASTOISE_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_WAILMER_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_REGIROCK_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_REGICE_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");

const u8 DecorDesc_REGISTEEL_DOLL[] = _(
    "A large doll.\n"
    "Place it on a mat\n"
    "or a desk.");
#elif GERMAN
#include "data/decoration/descriptions_de.h"
#endif

const u16 DecorGfx_SMALL_DESK[] = {
    0x87
};

const u16 DecorGfx_POKEMON_DESK[] = {
    0x8F
};

const u16 DecorGfx_HEAVY_DESK[] = {
    0x90,
    0x91,
    0x92,
    0x98,
    0x99,
    0x9A
};

const u16 DecorGfx_RAGGED_DESK[] = {
    0x93,
    0x94,
    0x95,
    0x9B,
    0x9C,
    0x9D
};

const u16 DecorGfx_COMFORT_DESK[] = {
    0x96,
    0x97,
    0xA3,
    0x9E,
    0x9F,
    0xAB
};

const u16 DecorGfx_PRETTY_DESK[] = {
    0xBD,
    0xBE,
    0xBF,
    0xC5,
    0xC6,
    0xC7,
    0xCD,
    0xCE,
    0xCF
};

const u16 DecorGfx_BRICK_DESK[] = {
    0xA0,
    0xA1,
    0xA2,
    0xA8,
    0xA9,
    0xAA,
    0xB0,
    0xB1,
    0xB2
};

const u16 DecorGfx_CAMP_DESK[] = {
    0xA4,
    0xA5,
    0xA6,
    0xAC,
    0xAD,
    0xAE,
    0xB4,
    0xB5,
    0xB6
};

const u16 DecorGfx_HARD_DESK[] = {
    0xA7,
    0xBB,
    0xBC,
    0xAF,
    0xC3,
    0xC4,
    0xB7,
    0xCB,
    0xCC
};

const u16 DecorGfx_SMALL_CHAIR[] = {
    0xB8
};

const u16 DecorGfx_POKEMON_CHAIR[] = {
    0xB9
};

const u16 DecorGfx_HEAVY_CHAIR[] = {
    0xBA
};

const u16 DecorGfx_PRETTY_CHAIR[] = {
    0xC0
};

const u16 DecorGfx_COMFORT_CHAIR[] = {
    0xC1
};

const u16 DecorGfx_RAGGED_CHAIR[] = {
    0xC2
};

const u16 DecorGfx_BRICK_CHAIR[] = {
    0xC8
};

const u16 DecorGfx_CAMP_CHAIR[] = {
    0xC9
};

const u16 DecorGfx_HARD_CHAIR[] = {
    0xCA
};

const u16 DecorGfx_RED_PLANT[] = {
    0xD0,
    0xD8
};

const u16 DecorGfx_TROPICAL_PLANT[] = {
    0xD2,
    0xDA
};

const u16 DecorGfx_PRETTY_FLOWERS[] = {
    0xD4,
    0xDC
};

const u16 DecorGfx_COLORFUL_PLANT[] = {
    0xE0,
    0xE2,
    0xE8,
    0xE9
};

const u16 DecorGfx_BIG_PLANT[] = {
    0xE4,
    0xE6,
    0xEC,
    0xED
};

const u16 DecorGfx_GORGEOUS_PLANT[] = {
    0xF0,
    0xF2,
    0xF8,
    0xF9
};

const u16 DecorGfx_RED_BRICK[] = {
    0x25,
    0x2D
};

const u16 DecorGfx_YELLOW_BRICK[] = {
    0x26,
    0x2E
};

const u16 DecorGfx_BLUE_BRICK[] = {
    0x27,
    0x2F
};

const u16 DecorGfx_RED_BALLOON[] = {
    0x138
};

const u16 DecorGfx_BLUE_BALLOON[] = {
    0x13C
};

const u16 DecorGfx_YELLOW_BALLOON[] = {
    0x140
};

const u16 DecorGfx_RED_TENT[] = {
    0x30,
    0x31,
    0x32,
    0x38,
    0x39,
    0x3A,
    0x40,
    0x41,
    0x3B
};

const u16 DecorGfx_BLUE_TENT[] = {
    0x48,
    0x49,
    0x68,
    0x50,
    0x51,
    0x70,
    0x58,
    0x59,
    0x69
};

const u16 DecorGfx_SOLID_BOARD[] = {
    0x34,
    0x3C
};

const u16 DecorGfx_SLIDE[] = {
    0x35,
    0x36,
    0x3D,
    0x3E,
    0x63,
    0x64,
    0x6F,
    0x77
};

const u16 DecorGfx_FENCE_LENGTH[] = {
    0x33
};

const u16 DecorGfx_FENCE_WIDTH[] = {
    0x2C
};

const u16 DecorGfx_TIRE[] = {
    0x80,
    0x81,
    0x88,
    0x89
};

const u16 DecorGfx_STAND[] = {
    0x6A,
    0x6B,
    0x6C,
    0x6D,
    0x72,
    0x73,
    0x74,
    0x75
};

const u16 DecorGfx_MUD_BALL[] = {
    0x28
};

const u16 DecorGfx_BREAKABLE_DOOR[] = {
    0x37,
    0x3F
};

const u16 DecorGfx_SAND_ORNAMENT[] = {
    0x85,
    0x8D
};

const u16 DecorGfx_SILVER_SHIELD[] = {
    0xD6,
    0xDE
};

const u16 DecorGfx_GOLD_SHIELD[] = {
    0x12E,
    0x136
};

const u16 DecorGfx_GLASS_ORNAMENT[] = {
    0x82,
    0x8A
};

const u16 DecorGfx_TV[] = {
    0xF4
};

const u16 DecorGfx_ROUND_TV[] = {
    0xF5
};

const u16 DecorGfx_CUTE_TV[] = {
    0xF6
};

const u16 DecorGfx_GLITTER_MAT[] = {
    0x60
};

const u16 DecorGfx_JUMP_MAT[] = {
    0x61
};

const u16 DecorGfx_SPIN_MAT[] = {
    0x62
};

const u16 DecorGfx_C_LOW_NOTE_MAT[] = {
    0x78
};

const u16 DecorGfx_D_NOTE_MAT[] = {
    0x79
};

const u16 DecorGfx_E_NOTE_MAT[] = {
    0x7A
};

const u16 DecorGfx_F_NOTE_MAT[] = {
    0x7B
};

const u16 DecorGfx_G_NOTE_MAT[] = {
    0x7C
};

const u16 DecorGfx_A_NOTE_MAT[] = {
    0x7D
};

const u16 DecorGfx_B_NOTE_MAT[] = {
    0x7E
};

const u16 DecorGfx_C_HIGH_NOTE_MAT[] = {
    0xB3
};

const u16 DecorGfx_SURF_MAT[] = {
    0x42,
    0x43,
    0x44,
    0x4A,
    0x4B,
    0x4C,
    0x52,
    0x53,
    0x54
};

const u16 DecorGfx_THUNDER_MAT[] = {
    0x45,
    0x46,
    0x47,
    0x4D,
    0x4E,
    0x4F,
    0x55,
    0x56,
    0x57
};

const u16 DecorGfx_FIRE_BLAST_MAT[] = {
    0x5A,
    0x5B,
    0x5C,
    0x5D,
    0x5E,
    0x5F,
    0x65,
    0x66,
    0x67
};

const u16 DecorGfx_POWDER_SNOW_MAT[] = {
    0x100,
    0x101,
    0x102,
    0x108,
    0x109,
    0x10A,
    0x110,
    0x111,
    0x112
};

const u16 DecorGfx_ATTRACT_MAT[] = {
    0x103,
    0x104,
    0x105,
    0x10B,
    0x10C,
    0x10D,
    0x113,
    0x114,
    0x115
};

const u16 DecorGfx_FISSURE_MAT[] = {
    0x106,
    0x107,
    0x118,
    0x10E,
    0x10F,
    0x120,
    0x116,
    0x117,
    0x128
};

const u16 DecorGfx_SPIKES_MAT[] = {
    0x119,
    0x11A,
    0x11B,
    0x121,
    0x122,
    0x123,
    0x129,
    0x12A,
    0x12B
};

const u16 DecorGfx_BALL_POSTER[] = {
    0x130
};

const u16 DecorGfx_GREEN_POSTER[] = {
    0x131
};

const u16 DecorGfx_RED_POSTER[] = {
    0x132
};

const u16 DecorGfx_BLUE_POSTER[] = {
    0x133
};

const u16 DecorGfx_CUTE_POSTER[] = {
    0x134
};

const u16 DecorGfx_PIKA_POSTER[] = {
    0x11C,
    0x11D
};

const u16 DecorGfx_LONG_POSTER[] = {
    0x11E,
    0x11F
};

const u16 DecorGfx_SEA_POSTER[] = {
    0x124,
    0x125
};

const u16 DecorGfx_SKY_POSTER[] = {
    0x126,
    0x127
};

const u16 DecorGfx_KISS_POSTER[] = {
    0x12C,
    0x12D
};

const u16 DecorGfx_PICHU_DOLL[] = {
    OBJ_EVENT_GFX_PICHU_DOLL
};

const u16 DecorGfx_PIKACHU_DOLL[] = {
    OBJ_EVENT_GFX_PIKACHU_DOLL
};

const u16 DecorGfx_MARILL_DOLL[] = {
    OBJ_EVENT_GFX_MARILL_DOLL
};

const u16 DecorGfx_TOGEPI_DOLL[] = {
    OBJ_EVENT_GFX_TOGEPI_DOLL
};

const u16 DecorGfx_CYNDAQUIL_DOLL[] = {
    OBJ_EVENT_GFX_CYNDAQUIL_DOLL
};

const u16 DecorGfx_CHIKORITA_DOLL[] = {
    OBJ_EVENT_GFX_CHIKORITA_DOLL
};

const u16 DecorGfx_TOTODILE_DOLL[] = {
    OBJ_EVENT_GFX_TOTODILE_DOLL
};

const u16 DecorGfx_JIGGLYPUFF_DOLL[] = {
    OBJ_EVENT_GFX_JIGGLYPUFF_DOLL
};

const u16 DecorGfx_MEOWTH_DOLL[] = {
    OBJ_EVENT_GFX_MEOWTH_DOLL
};

const u16 DecorGfx_CLEFAIRY_DOLL[] = {
    OBJ_EVENT_GFX_CLEFAIRY_DOLL
};

const u16 DecorGfx_DITTO_DOLL[] = {
    OBJ_EVENT_GFX_DITTO_DOLL
};

const u16 DecorGfx_SMOOCHUM_DOLL[] = {
    OBJ_EVENT_GFX_SMOOCHUM_DOLL
};

const u16 DecorGfx_TREECKO_DOLL[] = {
    OBJ_EVENT_GFX_TREECKO_DOLL
};

const u16 DecorGfx_TORCHIC_DOLL[] = {
    OBJ_EVENT_GFX_TORCHIC_DOLL
};

const u16 DecorGfx_MUDKIP_DOLL[] = {
    OBJ_EVENT_GFX_MUDKIP_DOLL
};

const u16 DecorGfx_DUSKULL_DOLL[] = {
    OBJ_EVENT_GFX_DUSKULL_DOLL
};

const u16 DecorGfx_WYNAUT_DOLL[] = {
    OBJ_EVENT_GFX_WYNAUT_DOLL
};

const u16 DecorGfx_BALTOY_DOLL[] = {
    OBJ_EVENT_GFX_BALTOY_DOLL
};

const u16 DecorGfx_KECLEON_DOLL[] = {
    OBJ_EVENT_GFX_KECLEON_DOLL
};

const u16 DecorGfx_AZURILL_DOLL[] = {
    OBJ_EVENT_GFX_AZURILL_DOLL
};

const u16 DecorGfx_SKITTY_DOLL[] = {
    OBJ_EVENT_GFX_SKITTY_DOLL
};

const u16 DecorGfx_SWABLU_DOLL[] = {
    OBJ_EVENT_GFX_SWABLU_DOLL
};

const u16 DecorGfx_GULPIN_DOLL[] = {
    OBJ_EVENT_GFX_GULPIN_DOLL
};

const u16 DecorGfx_LOTAD_DOLL[] = {
    OBJ_EVENT_GFX_LOTAD_DOLL
};

const u16 DecorGfx_SEEDOT_DOLL[] = {
    OBJ_EVENT_GFX_SEEDOT_DOLL
};

const u16 DecorGfx_PIKA_CUSHION[] = {
    OBJ_EVENT_GFX_PIKA_CUSHION
};

const u16 DecorGfx_ROUND_CUSHION[] = {
    OBJ_EVENT_GFX_ROUND_CUSHION
};

const u16 DecorGfx_KISS_CUSHION[] = {
    OBJ_EVENT_GFX_KISS_CUSHION
};

const u16 DecorGfx_ZIGZAG_CUSHION[] = {
    OBJ_EVENT_GFX_ZIGZAG_CUSHION
};

const u16 DecorGfx_SPIN_CUSHION[] = {
    OBJ_EVENT_GFX_SPIN_CUSHION
};

const u16 DecorGfx_DIAMOND_CUSHION[] = {
    OBJ_EVENT_GFX_DIAMOND_CUSHION
};

const u16 DecorGfx_BALL_CUSHION[] = {
    OBJ_EVENT_GFX_BALL_CUSHION
};

const u16 DecorGfx_GRASS_CUSHION[] = {
    OBJ_EVENT_GFX_GRASS_CUSHION
};

const u16 DecorGfx_FIRE_CUSHION[] = {
    OBJ_EVENT_GFX_FIRE_CUSHION
};

const u16 DecorGfx_WATER_CUSHION[] = {
    OBJ_EVENT_GFX_WATER_CUSHION
};

const u16 DecorGfx_SNORLAX_DOLL[] = {
    OBJ_EVENT_GFX_BIG_SNORLAX_DOLL
};

const u16 DecorGfx_RHYDON_DOLL[] = {
    OBJ_EVENT_GFX_BIG_RHYDON_DOLL
};

const u16 DecorGfx_LAPRAS_DOLL[] = {
    OBJ_EVENT_GFX_BIG_LAPRAS_DOLL
};

const u16 DecorGfx_VENUSAUR_DOLL[] = {
    OBJ_EVENT_GFX_BIG_VENUSAUR_DOLL
};

const u16 DecorGfx_CHARIZARD_DOLL[] = {
    OBJ_EVENT_GFX_BIG_CHARIZARD_DOLL
};

const u16 DecorGfx_BLASTOISE_DOLL[] = {
    OBJ_EVENT_GFX_BIG_BLASTOISE_DOLL
};

const u16 DecorGfx_WAILMER_DOLL[] = {
    OBJ_EVENT_GFX_BIG_WAILMER_DOLL
};

const u16 DecorGfx_REGIROCK_DOLL[] = {
    OBJ_EVENT_GFX_BIG_REGIROCK_DOLL
};

const u16 DecorGfx_REGICE_DOLL[] = {
    OBJ_EVENT_GFX_BIG_REGICE_DOLL
};

const u16 DecorGfx_REGISTEEL_DOLL[] = {
    OBJ_EVENT_GFX_BIG_REGISTEEL_DOLL
};

#if ENGLISH
const struct Decoration gDecorations[] = {
    {DECOR_NONE,            _("SMALL DESK"),      DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_DESK,         0, DecorDesc_SMALL_DESK, DecorGfx_SMALL_DESK},
    {DECOR_SMALL_DESK,      _("SMALL DESK"),      DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_DESK,      3000, DecorDesc_SMALL_DESK, DecorGfx_SMALL_DESK},
    {DECOR_POKEMON_DESK,    _("POKéMON DESK"),    DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_DESK,      3000, DecorDesc_POKEMON_DESK, DecorGfx_POKEMON_DESK},
    {DECOR_HEAVY_DESK,      _("HEAVY DESK"),      DECORPERM_SOLID_FLOOR,  DECORSHAPE_3x2, DECORCAT_DESK,      6000, DecorDesc_HEAVY_DESK, DecorGfx_HEAVY_DESK},
    {DECOR_RAGGED_DESK,     _("RAGGED DESK"),     DECORPERM_SOLID_FLOOR,  DECORSHAPE_3x2, DECORCAT_DESK,      6000, DecorDesc_RAGGED_DESK, DecorGfx_RAGGED_DESK},
    {DECOR_COMFORT_DESK,    _("COMFORT DESK"),    DECORPERM_SOLID_FLOOR,  DECORSHAPE_3x2, DECORCAT_DESK,      6000, DecorDesc_COMFORT_DESK, DecorGfx_COMFORT_DESK},
    {DECOR_PRETTY_DESK,     _("PRETTY DESK"),     DECORPERM_SOLID_FLOOR,  DECORSHAPE_3x3, DECORCAT_DESK,      9000, DecorDesc_PRETTY_DESK, DecorGfx_PRETTY_DESK},
    {DECOR_BRICK_DESK,      _("BRICK DESK"),      DECORPERM_SOLID_FLOOR,  DECORSHAPE_3x3, DECORCAT_DESK,      9000, DecorDesc_BRICK_DESK, DecorGfx_BRICK_DESK},
    {DECOR_CAMP_DESK,       _("CAMP DESK"),       DECORPERM_SOLID_FLOOR,  DECORSHAPE_3x3, DECORCAT_DESK,      9000, DecorDesc_CAMP_DESK, DecorGfx_CAMP_DESK},
    {DECOR_HARD_DESK,       _("HARD DESK"),       DECORPERM_SOLID_FLOOR,  DECORSHAPE_3x3, DECORCAT_DESK,      9000, DecorDesc_HARD_DESK, DecorGfx_HARD_DESK},
    {DECOR_SMALL_CHAIR,     _("SMALL CHAIR"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_SMALL_CHAIR, DecorGfx_SMALL_CHAIR},
    {DECOR_POKEMON_CHAIR,   _("POKéMON CHAIR"),   DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_POKEMON_CHAIR, DecorGfx_POKEMON_CHAIR},
    {DECOR_HEAVY_CHAIR,     _("HEAVY CHAIR"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_HEAVY_CHAIR, DecorGfx_HEAVY_CHAIR},
    {DECOR_PRETTY_CHAIR,    _("PRETTY CHAIR"),    DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_PRETTY_CHAIR, DecorGfx_PRETTY_CHAIR},
    {DECOR_COMFORT_CHAIR,   _("COMFORT CHAIR"),   DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_COMFORT_CHAIR, DecorGfx_COMFORT_CHAIR},
    {DECOR_RAGGED_CHAIR,    _("RAGGED CHAIR"),    DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_RAGGED_CHAIR, DecorGfx_RAGGED_CHAIR},
    {DECOR_BRICK_CHAIR,     _("BRICK CHAIR"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_BRICK_CHAIR, DecorGfx_BRICK_CHAIR},
    {DECOR_CAMP_CHAIR,      _("CAMP CHAIR"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_CAMP_CHAIR, DecorGfx_CAMP_CHAIR},
    {DECOR_HARD_CHAIR,      _("HARD CHAIR"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_CHAIR,     2000, DecorDesc_HARD_CHAIR, DecorGfx_HARD_CHAIR},
    {DECOR_RED_PLANT,       _("RED PLANT"),       DECORPERM_BEHIND_FLOOR, DECORSHAPE_1x2, DECORCAT_PLANT,     3000, DecorDesc_RED_PLANT, DecorGfx_RED_PLANT},
    {DECOR_TROPICAL_PLANT,  _("TROPICAL PLANT"),  DECORPERM_BEHIND_FLOOR, DECORSHAPE_1x2, DECORCAT_PLANT,     3000, DecorDesc_TROPICAL_PLANT, DecorGfx_TROPICAL_PLANT},
    {DECOR_PRETTY_FLOWERS,  _("PRETTY FLOWERS"),  DECORPERM_BEHIND_FLOOR, DECORSHAPE_1x2, DECORCAT_PLANT,     3000, DecorDesc_PRETTY_FLOWERS, DecorGfx_PRETTY_FLOWERS},
    {DECOR_COLORFUL_PLANT,  _("COLORFUL PLANT"),  DECORPERM_BEHIND_FLOOR, DECORSHAPE_2x2, DECORCAT_PLANT,     5000, DecorDesc_COLORFUL_PLANT, DecorGfx_COLORFUL_PLANT},
    {DECOR_BIG_PLANT,       _("BIG PLANT"),       DECORPERM_BEHIND_FLOOR, DECORSHAPE_2x2, DECORCAT_PLANT,     5000, DecorDesc_BIG_PLANT, DecorGfx_BIG_PLANT},
    {DECOR_GORGEOUS_PLANT,  _("GORGEOUS PLANT"),  DECORPERM_BEHIND_FLOOR, DECORSHAPE_2x2, DECORCAT_PLANT,     5000, DecorDesc_GORGEOUS_PLANT, DecorGfx_GORGEOUS_PLANT},
    {DECOR_RED_BRICK,       _("RED BRICK"),       DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x2, DECORCAT_ORNAMENT,   500, DecorDesc_RED_BRICK, DecorGfx_RED_BRICK},
    {DECOR_YELLOW_BRICK,    _("YELLOW BRICK"),    DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x2, DECORCAT_ORNAMENT,   500, DecorDesc_YELLOW_BRICK, DecorGfx_YELLOW_BRICK},
    {DECOR_BLUE_BRICK,      _("BLUE BRICK"),      DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x2, DECORCAT_ORNAMENT,   500, DecorDesc_BLUE_BRICK, DecorGfx_BLUE_BRICK},
    {DECOR_RED_BALLOON,     _("RED BALLOON"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_ORNAMENT,   500, DecorDesc_RED_BALLOON, DecorGfx_RED_BALLOON},
    {DECOR_BLUE_BALLOON,    _("BLUE BALLOON"),    DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_ORNAMENT,   500, DecorDesc_BLUE_BALLOON, DecorGfx_BLUE_BALLOON},
    {DECOR_YELLOW_BALLOON,  _("YELLOW BALLOON"),  DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_ORNAMENT,   500, DecorDesc_YELLOW_BALLOON, DecorGfx_YELLOW_BALLOON},
    {DECOR_RED_TENT,        _("RED TENT"),        DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_ORNAMENT, 10000, DecorDesc_RED_TENT, DecorGfx_RED_TENT},
    {DECOR_BLUE_TENT,       _("BLUE TENT"),       DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_ORNAMENT, 10000, DecorDesc_BLUE_TENT, DecorGfx_BLUE_TENT},
    {DECOR_SOLID_BOARD,     _("SOLID BOARD"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_1x2, DECORCAT_ORNAMENT,  3000, DecorDesc_SOLID_BOARD, DecorGfx_SOLID_BOARD},
    {DECOR_SLIDE,           _("SLIDE"),           DECORPERM_PASS_FLOOR,   DECORSHAPE_2x4, DECORCAT_ORNAMENT,  8000, DecorDesc_SLIDE, DecorGfx_SLIDE},
    {DECOR_FENCE_LENGTH,    _("FENCE LENGTH"),    DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_ORNAMENT,   500, DecorDesc_FENCE_LENGTH, DecorGfx_FENCE_LENGTH},
    {DECOR_FENCE_WIDTH,     _("FENCE WIDTH"),     DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_ORNAMENT,   500, DecorDesc_FENCE_WIDTH, DecorGfx_FENCE_WIDTH},
    {DECOR_TIRE,            _("TIRE"),            DECORPERM_SOLID_FLOOR,  DECORSHAPE_2x2, DECORCAT_ORNAMENT,   800, DecorDesc_TIRE, DecorGfx_TIRE},
    {DECOR_STAND,           _("STAND"),           DECORPERM_PASS_FLOOR,   DECORSHAPE_4x2, DECORCAT_ORNAMENT,  7000, DecorDesc_STAND, DecorGfx_STAND},
    {DECOR_MUD_BALL,        _("MUD BALL"),        DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_ORNAMENT,   200, DecorDesc_MUD_BALL, DecorGfx_MUD_BALL},
    {DECOR_BREAKABLE_DOOR,  _("BREAKABLE DOOR"),  DECORPERM_PASS_FLOOR,   DECORSHAPE_1x2, DECORCAT_ORNAMENT,  3000, DecorDesc_BREAKABLE_DOOR, DecorGfx_BREAKABLE_DOOR},
    {DECOR_SAND_ORNAMENT,   _("SAND ORNAMENT"),   DECORPERM_BEHIND_FLOOR, DECORSHAPE_1x2, DECORCAT_ORNAMENT,  3000, DecorDesc_SAND_ORNAMENT, DecorGfx_SAND_ORNAMENT},
    {DECOR_SILVER_SHIELD,   _("SILVER SHIELD"),   DECORPERM_BEHIND_FLOOR, DECORSHAPE_1x2, DECORCAT_ORNAMENT,     0, DecorDesc_SILVER_SHIELD, DecorGfx_SILVER_SHIELD},
    {DECOR_GOLD_SHIELD,     _("GOLD SHIELD"),     DECORPERM_BEHIND_FLOOR, DECORSHAPE_1x2, DECORCAT_ORNAMENT,     0, DecorDesc_GOLD_SHIELD, DecorGfx_GOLD_SHIELD},
    {DECOR_GLASS_ORNAMENT,  _("GLASS ORNAMENT"),  DECORPERM_BEHIND_FLOOR, DECORSHAPE_1x2, DECORCAT_ORNAMENT,     0, DecorDesc_GLASS_ORNAMENT, DecorGfx_GLASS_ORNAMENT},
    {DECOR_TV,              _("TV"),              DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_ORNAMENT,  3000, DecorDesc_TV, DecorGfx_TV},
    {DECOR_ROUND_TV,        _("ROUND TV"),        DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_ORNAMENT,  4000, DecorDesc_ROUND_TV, DecorGfx_ROUND_TV},
    {DECOR_CUTE_TV,         _("CUTE TV"),         DECORPERM_SOLID_FLOOR,  DECORSHAPE_1x1, DECORCAT_ORNAMENT,  4000, DecorDesc_CUTE_TV, DecorGfx_CUTE_TV},
    {DECOR_GLITTER_MAT,     _("GLITTER MAT"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,       2000, DecorDesc_GLITTER_MAT, DecorGfx_GLITTER_MAT},
    {DECOR_JUMP_MAT,        _("JUMP MAT"),        DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,       2000, DecorDesc_JUMP_MAT, DecorGfx_JUMP_MAT},
    {DECOR_SPIN_MAT,        _("SPIN MAT"),        DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,       2000, DecorDesc_SPIN_MAT, DecorGfx_SPIN_MAT},
    {DECOR_C_LOW_NOTE_MAT,  _("C Low NOTE MAT"),  DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_C_LOW_NOTE_MAT, DecorGfx_C_LOW_NOTE_MAT},
    {DECOR_D_NOTE_MAT,      _("D NOTE MAT"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_D_NOTE_MAT, DecorGfx_D_NOTE_MAT},
    {DECOR_E_NOTE_MAT,      _("E NOTE MAT"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_E_NOTE_MAT, DecorGfx_E_NOTE_MAT},
    {DECOR_F_NOTE_MAT,      _("F NOTE MAT"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_F_NOTE_MAT, DecorGfx_F_NOTE_MAT},
    {DECOR_G_NOTE_MAT,      _("G NOTE MAT"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_G_NOTE_MAT, DecorGfx_G_NOTE_MAT},
    {DECOR_A_NOTE_MAT,      _("A NOTE MAT"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_A_NOTE_MAT, DecorGfx_A_NOTE_MAT},
    {DECOR_B_NOTE_MAT,      _("B NOTE MAT"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_B_NOTE_MAT, DecorGfx_B_NOTE_MAT},
    {DECOR_C_HIGH_NOTE_MAT, _("C High NOTE MAT"), DECORPERM_PASS_FLOOR,   DECORSHAPE_1x1, DECORCAT_MAT,        500, DecorDesc_C_HIGH_NOTE_MAT, DecorGfx_C_HIGH_NOTE_MAT},
    {DECOR_SURF_MAT,        _("SURF MAT"),        DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_MAT,       4000, DecorDesc_SURF_MAT, DecorGfx_SURF_MAT},
    {DECOR_THUNDER_MAT,     _("THUNDER MAT"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_MAT,       4000, DecorDesc_THUNDER_MAT, DecorGfx_THUNDER_MAT},
    {DECOR_FIRE_BLAST_MAT,  _("FIRE BLAST MAT"),  DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_MAT,       4000, DecorDesc_FIRE_BLAST_MAT, DecorGfx_FIRE_BLAST_MAT},
    {DECOR_POWDER_SNOW_MAT, _("POWDER SNOW MAT"), DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_MAT,       4000, DecorDesc_POWDER_SNOW_MAT, DecorGfx_POWDER_SNOW_MAT},
    {DECOR_ATTRACT_MAT,     _("ATTRACT MAT"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_MAT,       4000, DecorDesc_ATTRACT_MAT, DecorGfx_ATTRACT_MAT},
    {DECOR_FISSURE_MAT,     _("FISSURE MAT"),     DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_MAT,       4000, DecorDesc_FISSURE_MAT, DecorGfx_FISSURE_MAT},
    {DECOR_SPIKES_MAT,      _("SPIKES MAT"),      DECORPERM_PASS_FLOOR,   DECORSHAPE_3x3, DECORCAT_MAT,       4000, DecorDesc_SPIKES_MAT, DecorGfx_SPIKES_MAT},
    {DECOR_BALL_POSTER,     _("BALL POSTER"),      DECORPERM_NA_WALL,     DECORSHAPE_1x1, DECORCAT_POSTER,    1000, DecorDesc_BALL_POSTER, DecorGfx_BALL_POSTER},
    {DECOR_GREEN_POSTER,    _("GREEN POSTER"),     DECORPERM_NA_WALL,     DECORSHAPE_1x1, DECORCAT_POSTER,    1000, DecorDesc_GREEN_POSTER, DecorGfx_GREEN_POSTER},
    {DECOR_RED_POSTER,      _("RED POSTER"),       DECORPERM_NA_WALL,     DECORSHAPE_1x1, DECORCAT_POSTER,    1000, DecorDesc_RED_POSTER, DecorGfx_RED_POSTER},
    {DECOR_BLUE_POSTER,     _("BLUE POSTER"),      DECORPERM_NA_WALL,     DECORSHAPE_1x1, DECORCAT_POSTER,    1000, DecorDesc_BLUE_POSTER, DecorGfx_BLUE_POSTER},
    {DECOR_CUTE_POSTER,     _("CUTE POSTER"),      DECORPERM_NA_WALL,     DECORSHAPE_1x1, DECORCAT_POSTER,    1000, DecorDesc_CUTE_POSTER, DecorGfx_CUTE_POSTER},
    {DECOR_PIKA_POSTER,     _("PIKA POSTER"),      DECORPERM_NA_WALL,     DECORSHAPE_2x1, DECORCAT_POSTER,    1500, DecorDesc_PIKA_POSTER, DecorGfx_PIKA_POSTER},
    {DECOR_LONG_POSTER,     _("LONG POSTER"),      DECORPERM_NA_WALL,     DECORSHAPE_2x1, DECORCAT_POSTER,    1500, DecorDesc_LONG_POSTER, DecorGfx_LONG_POSTER},
    {DECOR_SEA_POSTER,      _("SEA POSTER"),       DECORPERM_NA_WALL,     DECORSHAPE_2x1, DECORCAT_POSTER,    1500, DecorDesc_SEA_POSTER, DecorGfx_SEA_POSTER},
    {DECOR_SKY_POSTER,      _("SKY POSTER"),       DECORPERM_NA_WALL,     DECORSHAPE_2x1, DECORCAT_POSTER,    1500, DecorDesc_SKY_POSTER, DecorGfx_SKY_POSTER},
    {DECOR_KISS_POSTER,     _("KISS POSTER"),      DECORPERM_NA_WALL,     DECORSHAPE_2x1, DECORCAT_POSTER,    1500, DecorDesc_KISS_POSTER, DecorGfx_KISS_POSTER},
    {DECOR_PICHU_DOLL,      _("PICHU DOLL"),       DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_PICHU_DOLL, DecorGfx_PICHU_DOLL},
    {DECOR_PIKACHU_DOLL,    _("PIKACHU DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_PIKACHU_DOLL, DecorGfx_PIKACHU_DOLL},
    {DECOR_MARILL_DOLL,     _("MARILL DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_MARILL_DOLL, DecorGfx_MARILL_DOLL},
    {DECOR_TOGEPI_DOLL,     _("TOGEPI DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_TOGEPI_DOLL, DecorGfx_TOGEPI_DOLL},
    {DECOR_CYNDAQUIL_DOLL,  _("CYNDAQUIL DOLL"),   DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_CYNDAQUIL_DOLL, DecorGfx_CYNDAQUIL_DOLL},
    {DECOR_CHIKORITA_DOLL,  _("CHIKORITA DOLL"),   DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_CHIKORITA_DOLL, DecorGfx_CHIKORITA_DOLL},
    {DECOR_TOTODILE_DOLL,   _("TOTODILE DOLL"),    DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_TOTODILE_DOLL, DecorGfx_TOTODILE_DOLL},
    {DECOR_JIGGLYPUFF_DOLL, _("JIGGLYPUFF DOLL"),  DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_JIGGLYPUFF_DOLL, DecorGfx_JIGGLYPUFF_DOLL},
    {DECOR_MEOWTH_DOLL,     _("MEOWTH DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_MEOWTH_DOLL, DecorGfx_MEOWTH_DOLL},
    {DECOR_CLEFAIRY_DOLL,   _("CLEFAIRY DOLL"),    DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_CLEFAIRY_DOLL, DecorGfx_CLEFAIRY_DOLL},
    {DECOR_DITTO_DOLL,      _("DITTO DOLL"),       DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_DITTO_DOLL, DecorGfx_DITTO_DOLL},
    {DECOR_SMOOCHUM_DOLL,   _("SMOOCHUM DOLL"),    DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_SMOOCHUM_DOLL, DecorGfx_SMOOCHUM_DOLL},
    {DECOR_TREECKO_DOLL,    _("TREECKO DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_TREECKO_DOLL, DecorGfx_TREECKO_DOLL},
    {DECOR_TORCHIC_DOLL,    _("TORCHIC DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_TORCHIC_DOLL, DecorGfx_TORCHIC_DOLL},
    {DECOR_MUDKIP_DOLL,     _("MUDKIP DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_MUDKIP_DOLL, DecorGfx_MUDKIP_DOLL},
    {DECOR_DUSKULL_DOLL,    _("DUSKULL DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_DUSKULL_DOLL, DecorGfx_DUSKULL_DOLL},
    {DECOR_WYNAUT_DOLL,     _("WYNAUT DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_WYNAUT_DOLL, DecorGfx_WYNAUT_DOLL},
    {DECOR_BALTOY_DOLL,     _("BALTOY DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_BALTOY_DOLL, DecorGfx_BALTOY_DOLL},
    {DECOR_KECLEON_DOLL,    _("KECLEON DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_KECLEON_DOLL, DecorGfx_KECLEON_DOLL},
    {DECOR_AZURILL_DOLL,    _("AZURILL DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_AZURILL_DOLL, DecorGfx_AZURILL_DOLL},
    {DECOR_SKITTY_DOLL,     _("SKITTY DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_SKITTY_DOLL, DecorGfx_SKITTY_DOLL},
    {DECOR_SWABLU_DOLL,     _("SWABLU DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_SWABLU_DOLL, DecorGfx_SWABLU_DOLL},
    {DECOR_GULPIN_DOLL,     _("GULPIN DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_GULPIN_DOLL, DecorGfx_GULPIN_DOLL},
    {DECOR_LOTAD_DOLL,      _("LOTAD DOLL"),       DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_LOTAD_DOLL, DecorGfx_LOTAD_DOLL},
    {DECOR_SEEDOT_DOLL,     _("SEEDOT DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_DOLL,      3000, DecorDesc_SEEDOT_DOLL, DecorGfx_SEEDOT_DOLL},
    {DECOR_PIKA_CUSHION,    _("PIKA CUSHION"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_PIKA_CUSHION, DecorGfx_PIKA_CUSHION},
    {DECOR_ROUND_CUSHION,   _("ROUND CUSHION"),    DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_ROUND_CUSHION, DecorGfx_ROUND_CUSHION},
    {DECOR_KISS_CUSHION,    _("KISS CUSHION"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_KISS_CUSHION, DecorGfx_KISS_CUSHION},
    {DECOR_ZIGZAG_CUSHION,  _("ZIGZAG CUSHION"),   DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_ZIGZAG_CUSHION, DecorGfx_ZIGZAG_CUSHION},
    {DECOR_SPIN_CUSHION,    _("SPIN CUSHION"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_SPIN_CUSHION, DecorGfx_SPIN_CUSHION},
    {DECOR_DIAMOND_CUSHION, _("DIAMOND CUSHION"),  DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_DIAMOND_CUSHION, DecorGfx_DIAMOND_CUSHION},
    {DECOR_BALL_CUSHION,    _("BALL CUSHION"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_BALL_CUSHION, DecorGfx_BALL_CUSHION},
    {DECOR_GRASS_CUSHION,   _("GRASS CUSHION"),    DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_GRASS_CUSHION, DecorGfx_GRASS_CUSHION},
    {DECOR_FIRE_CUSHION,    _("FIRE CUSHION"),     DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_FIRE_CUSHION, DecorGfx_FIRE_CUSHION},
    {DECOR_WATER_CUSHION,   _("WATER CUSHION"),    DECORPERM_SPRITE,   DECORSHAPE_1x1, DECORCAT_CUSHION,   2000, DecorDesc_WATER_CUSHION, DecorGfx_WATER_CUSHION},
    {DECOR_SNORLAX_DOLL,    _("SNORLAX DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_SNORLAX_DOLL, DecorGfx_SNORLAX_DOLL},
    {DECOR_RHYDON_DOLL,     _("RHYDON DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_RHYDON_DOLL, DecorGfx_RHYDON_DOLL},
    {DECOR_LAPRAS_DOLL,     _("LAPRAS DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_LAPRAS_DOLL, DecorGfx_LAPRAS_DOLL},
    {DECOR_VENUSAUR_DOLL,   _("VENUSAUR DOLL"),    DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_VENUSAUR_DOLL, DecorGfx_VENUSAUR_DOLL},
    {DECOR_CHARIZARD_DOLL,  _("CHARIZARD DOLL"),   DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_CHARIZARD_DOLL, DecorGfx_CHARIZARD_DOLL},
    {DECOR_BLASTOISE_DOLL,  _("BLASTOISE DOLL"),   DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_BLASTOISE_DOLL, DecorGfx_BLASTOISE_DOLL},
    {DECOR_WAILMER_DOLL,    _("WAILMER DOLL"),     DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_WAILMER_DOLL, DecorGfx_WAILMER_DOLL},
    {DECOR_REGIROCK_DOLL,   _("REGIROCK DOLL"),    DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_REGIROCK_DOLL, DecorGfx_REGIROCK_DOLL},
    {DECOR_REGICE_DOLL,     _("REGICE DOLL"),      DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_REGICE_DOLL, DecorGfx_REGICE_DOLL},
    {DECOR_REGISTEEL_DOLL,  _("REGISTEEL DOLL"),   DECORPERM_SPRITE,   DECORSHAPE_1x2, DECORCAT_DOLL,     10000, DecorDesc_REGISTEEL_DOLL, DecorGfx_REGISTEEL_DOLL}
};
#elif GERMAN
#include "data/decoration/decorations.h"
#endif

static const u8 *const sDecorationCategoryNames[] = {
    SecretBaseText_Desk,
    SecretBaseText_Chair,
    SecretBaseText_Plant,
    SecretBaseText_Ornament,
    SecretBaseText_Mat,
    SecretBaseText_Poster,
    SecretBaseText_Doll,
    SecretBaseText_Cushion
};

static const struct MenuAction2 sDecorationMainMenuActions[] = {
    {SecretBaseText_Decorate, DecorationMenuAction_Decorate},
    {SecretBaseText_PutAway, DecorationMenuAction_PutAway},
    {SecretBaseText_Toss, DecorationMenuAction_Toss},
    {gText_Exit, DecorationMenuAction_Cancel}
};

static const u8 *const sSecretBasePCMenuItemDescriptions[] = {
    SecretBaseText_PutOutDecor,
    SecretBaseText_StoreChosenDecor,
    SecretBaseText_ThrowAwayDecor,
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
                    DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, ReturnToDecorationCategoriesAfterInvalidSelection, 0);
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
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, ReturnToDecorationActionsAfterInvalidSelection, 0);
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
        DisplayItemMessageOnField(taskId, gSecretBaseText_DecorCantPlace, ReturnToDecorationItemsAfterInvalidSelection, 0);
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
        DisplayItemMessageOnField(taskId, gSecretBaseText_InUseAlready, ReturnToDecorationItemsAfterInvalidSelection, 0);
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
    DisplayItemMessageOnField(taskId, gSecretBaseText_CancelDecorating, CancelDecoratingPrompt, 0);
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
        DisplayItemMessageOnField(taskId, gSecretBaseText_PlaceItHere, PlaceDecorationPrompt, 0);
    } else
    {
        PlaySE(SE_FAILURE);
        DisplayItemMessageOnField(taskId, gSecretBaseText_CantBePlacedHere, CantPlaceDecorationPrompt, 0);
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

static const u8 sDecorTilemap_1x1_Tiles[] = {0, 1, 2, 3};
static const u8 sDecorTilemap_3x1_Tiles[] = {0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13};
static const u8 sDecorTilemap_2x2_Tiles[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
static const u8 sDecorTilemap_1x3_Tiles[] = {0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 21};
static const u8 sDecorTilemap_2x1_Tiles[] = {0, 1, 2, 3, 4, 5, 6, 7};
static const u8 sDecorTilemap_4x2_Tiles[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
static const u8 sDecorTilemap_3x3_Tiles[] = {0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19, 20, 21, 24, 25, 26, 27, 28, 29, 32, 33, 34, 35, 36, 37, 40, 41, 42, 43, 44, 45};
static const u8 sDecorTilemap_3x2_Tiles[] = {0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19, 20, 21, 24, 25, 26, 27, 28, 29};

static const u8 sDecorTilemap_1x1_Y[] = {0, 0, 0, 0};
static const u8 sDecorTilemap_2x1_Y[] = {0, 0, 1, 1, 0, 0, 1, 1};
static const u8 sDecorTilemap_3x1_Y[] = {0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2};
static const u8 sDecorTilemap_4x2_Y[] = {0, 0, 1, 1, 2, 2, 3, 3, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 4, 4, 5, 5, 6, 6, 7, 7};
static const u8 sDecorTilemap_2x2_Y[] = {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3};
static const u8 sDecorTilemap_1x2_Y[] = {0, 0, 0, 0, 1, 1, 1, 1};
static const u8 sDecorTilemap_1x3_Y[] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2};
static const u8 sDecorTilemap_2x4_Y[] = {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7};
static const u8 sDecorTilemap_3x3_Y[] = {0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 6, 6, 7, 7, 8, 8};
static const u8 sDecorTilemap_3x2_Y[] = {0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 3, 3, 4, 4, 5, 5};

static const u8 sDecorTilemap_1x1_X[] = {4, 5, 6, 7};
static const u8 sDecorTilemap_2x1_X[] = {4, 5, 4, 5, 6, 7, 6, 7};
static const u8 sDecorTilemap_3x1_X[] = {4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7};
static const u8 sDecorTilemap_4x2_X[] = {4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7};
static const u8 sDecorTilemap_2x2_X[] = {4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7};
static const u8 sDecorTilemap_1x2_X[] = {4, 5, 6, 7, 4, 5, 6, 7};
static const u8 sDecorTilemap_1x3_X[] = {4, 5, 6, 7, 4, 5, 6, 7, 4, 5, 6, 7};
static const u8 sDecorTilemap_2x4_X[] = {4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7};
static const u8 sDecorTilemap_3x3_X[] = {4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7};
static const u8 sDecorTilemap_3x2_X[] = {4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7};

static const struct DecorTilemap sDecorTilemaps[] = {
    {sDecorTilemap_1x1_Tiles, sDecorTilemap_1x1_Y, sDecorTilemap_1x1_X, 0x4},
    {sDecorTilemap_2x1_Tiles, sDecorTilemap_2x1_Y, sDecorTilemap_2x1_X, 0x8},
    {sDecorTilemap_3x1_Tiles, sDecorTilemap_3x1_Y, sDecorTilemap_3x1_X, 0xc},
    {sDecorTilemap_4x2_Tiles, sDecorTilemap_4x2_Y, sDecorTilemap_4x2_X, 0x20},
    {sDecorTilemap_2x2_Tiles, sDecorTilemap_2x2_Y, sDecorTilemap_2x2_X, 0x10},
    {sDecorTilemap_2x1_Tiles, sDecorTilemap_1x2_Y, sDecorTilemap_1x2_X, 0x8},
    {sDecorTilemap_1x3_Tiles, sDecorTilemap_1x3_Y, sDecorTilemap_1x3_X, 0xc},
    {sDecorTilemap_4x2_Tiles, sDecorTilemap_2x4_Y, sDecorTilemap_2x4_X, 0x20},
    {sDecorTilemap_3x3_Tiles, sDecorTilemap_3x3_Y, sDecorTilemap_3x3_X, 0x24},
    {sDecorTilemap_3x2_Tiles, sDecorTilemap_3x2_Y, sDecorTilemap_3x2_X, 0x18}
};

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
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecorInUse, ReturnToDecorationActionsAfterInvalidSelection, 0);
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
                DisplayItemMessageOnField(taskId, gSecretBaseText_DecorReturned, ContinuePuttingAwayDecorationsPrompt, 0);
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
    DisplayItemMessageOnField(taskId, gSecretBaseText_StopPuttingAwayDecor, StopPuttingAwayDecorationsPrompt, 0);
}

void AttemptPutAwayDecoration_(u8 taskId)
{
    u8 metatileBehavior;
    s16 *data;
    AttemptMarkDecorUnderCursorForRemoval(taskId);
    if (sCurDecorSelectedInRearrangement != 0)
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_ReturnDecor, ReturnDecorationPrompt, 0);
    } else
    {
        data = gTasks[taskId].data;
        metatileBehavior = MapGridGetMetatileBehaviorAt(tCursorX, tCursorY);
        if (MetatileBehavior_IsSecretBasePC(metatileBehavior) == TRUE
         || MetatileBehavior_IsPlayerRoomPCOn(metatileBehavior) == TRUE)
        {
            gSprites[sDecor_CameraSpriteObjectIdx1].invisible = FALSE;
            gSprites[sDecor_CameraSpriteObjectIdx1].callback = SpriteCallbackDummy;
            DisplayItemMessageOnField(taskId, gSecretBaseText_StopPuttingAwayDecor, StopPuttingAwayDecorationsPrompt, 0);
        } else
        {
            DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecor, ContinuePuttingAwayDecorationsPrompt, 0);
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
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, ReturnToDecorationActionsAfterInvalidSelection, 0);
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
        DisplayItemMessageOnField(taskId, gSecretBaseText_NoDecors, ReturnToDecorationActionsAfterInvalidSelection, 0);
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
        StringExpandPlaceholders(gStringVar4, gSecretBaseText_WillBeDiscarded);
        DisplayItemMessageOnField(taskId, gStringVar4, TossDecorationPrompt, 0);
    } else
    {
        DisplayItemMessageOnField(taskId, gSecretBaseText_DecorInUse, ReturnToDecorationItemsAfterInvalidSelection, 0);
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
    DisplayItemMessageOnField(taskId, gSecretBaseText_DecorThrownAway, ReturnToDecorationItemsAfterInvalidSelection, 0);
}
