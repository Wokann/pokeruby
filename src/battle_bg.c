#include "global.h"
#include "battle.h"
#include "battle_bg.h"
#include "battle_message.h"
#include "data.h"
#include "battle_gfx_sfx_util.h"
#include "battle_main.h"
#include "decompress.h"
#include "ewram.h"
#include "graphics.h"
#include "link.h"
#include "main.h"
#include "overworld.h"
#include "palette.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "trig.h"
#include "sound.h"
#include "constants/map_types.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "constants/rgb.h"

extern u16 gBattleTypeFlags;
extern u8 gBattleOutcome;

extern struct Window gWindowTemplate_Contest_MoveDescription;

extern const u8 gGameVersion;
extern u16 gBattleTypeFlags;
extern u16 gTrainerBattleOpponent_A;

extern u8 gBattleEnvironment;

extern u16 gBattleEnvironmentPalette_Groudon[];
extern u16 gBattleEnvironmentPalette_Kyogre[];
extern u16 gBattleEnvironmentPalette_BuildingLeader[];
extern u16 gBattleEnvironmentPalette_StadiumSteven[];
extern u16 gBattleEnvironmentPalette_BuildingGym[];
extern u16 gBattleEnvironmentPalette_StadiumMagma[];
extern u16 gBattleEnvironmentPalette_StadiumAqua[];
extern u16 gBattleEnvironmentPalette_StadiumSidney[];
extern u16 gBattleEnvironmentPalette_StadiumPhoebe[];
extern u16 gBattleEnvironmentPalette_StadiumGlacia[];
extern u16 gBattleEnvironmentPalette_StadiumDrake[];
extern u16 gBattleEnvironmentPalette_BattleTower[];

extern u16 gBattleVSFrame_Tilemap[];
extern u16 gBattleVSFrame_Pal[];

extern u16 gBattle_BG1_X;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG2_X;
extern u16 gBattle_BG2_Y;


struct LinkResultWindow {
    struct Window *window;
    u16 offset;
    u8 left;
    u8 top;
    u8 *dest;
};

#define gLinkResultWindows gUnknown_081F9680
extern const struct LinkResultWindow gLinkResultWindows[];

static const struct OamData sVsLetter_V_OamData = {
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .size = SPRITE_SIZE(64x64)
};

static const struct OamData sVsLetter_S_OamData = {
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 64
};

static const union AffineAnimCmd sVsLetterAffineAnimCmds0[] = {
    AFFINEANIMCMD_FRAME(0x80, 0x80, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sVsLetterAffineAnimCmds1[] = {
    AFFINEANIMCMD_FRAME(0x80, 0x80, 0, 0),
    AFFINEANIMCMD_FRAME(0x18, 0x18, 0, -128),
    AFFINEANIMCMD_FRAME(0x18, 0x18, 0, -128),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sVsLetterAffineAnimTable[] = {
    sVsLetterAffineAnimCmds0,
    sVsLetterAffineAnimCmds1
};

#define TAG_VS_LETTERS 10000

static const struct SpriteTemplate sVsLetter_V_SpriteTemplate = {
    .tileTag = TAG_VS_LETTERS,
    .paletteTag = TAG_VS_LETTERS,
    .oam = &sVsLetter_V_OamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sVsLetterAffineAnimTable,
    .callback = SpriteCB_VsLetterDummy
};

static const struct SpriteTemplate sVsLetter_S_SpriteTemplate = {
    .tileTag = TAG_VS_LETTERS,
    .paletteTag = TAG_VS_LETTERS,
    .oam = &sVsLetter_S_OamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sVsLetterAffineAnimTable,
    .callback = SpriteCB_VsLetterDummy
};


static const struct CompressedSpriteSheet sVsLettersSpriteSheet[] = {
    {gVsLettersGfx, 4096, TAG_VS_LETTERS},
};

struct BattleBackground
{
    const void *tileset;
    const void *tilemap;
    const void *entryTileset;
    const void *entryTilemap;
    const void *palette;
};

static const struct BattleBackground sBattleEnvironmentTable[] =
{
    [BATTLE_ENVIRONMENT_GRASS] =
    {
        .tileset = gBattleEnvironmentTiles_TallGrass,
        .tilemap = gBattleEnvironmentTilemap_TallGrass,
        .entryTileset = gBattleEnvironmentAnimTiles_TallGrass,
        .entryTilemap = gBattleEnvironmentAnimTilemap_TallGrass,
        .palette = gBattleEnvironmentPalette_TallGrass,
    },

    [BATTLE_ENVIRONMENT_LONG_GRASS] =
    {
        .tileset = gBattleEnvironmentTiles_LongGrass,
        .tilemap = gBattleEnvironmentTilemap_LongGrass,
        .entryTileset = gBattleEnvironmentAnimTiles_LongGrass,
        .entryTilemap = gBattleEnvironmentAnimTilemap_LongGrass,
        .palette = gBattleEnvironmentPalette_LongGrass,
    },

    [BATTLE_ENVIRONMENT_SAND] =
    {
        .tileset = gBattleEnvironmentTiles_Sand,
        .tilemap = gBattleEnvironmentTilemap_Sand,
        .entryTileset = gBattleEnvironmentAnimTiles_Sand,
        .entryTilemap = gBattleEnvironmentAnimTilemap_Sand,
        .palette = gBattleEnvironmentPalette_Sand,
    },

    [BATTLE_ENVIRONMENT_UNDERWATER] =
    {
        .tileset = gBattleEnvironmentTiles_Underwater,
        .tilemap = gBattleEnvironmentTilemap_Underwater,
        .entryTileset = gBattleEnvironmentAnimTiles_Underwater,
        .entryTilemap = gBattleEnvironmentAnimTilemap_Underwater,
        .palette = gBattleEnvironmentPalette_Underwater,
    },

    [BATTLE_ENVIRONMENT_WATER] =
    {
        .tileset = gBattleEnvironmentTiles_Water,
        .tilemap = gBattleEnvironmentTilemap_Water,
        .entryTileset = gBattleEnvironmentAnimTiles_Water,
        .entryTilemap = gBattleEnvironmentAnimTilemap_Water,
        .palette = gBattleEnvironmentPalette_Water,
    },

    [BATTLE_ENVIRONMENT_POND] =
    {
        .tileset = gBattleEnvironmentTiles_PondWater,
        .tilemap = gBattleEnvironmentTilemap_PondWater,
        .entryTileset = gBattleEnvironmentAnimTiles_PondWater,
        .entryTilemap = gBattleEnvironmentAnimTilemap_PondWater,
        .palette = gBattleEnvironmentPalette_PondWater,
    },

    [BATTLE_ENVIRONMENT_MOUNTAIN] =
    {
        .tileset = gBattleEnvironmentTiles_Rock,
        .tilemap = gBattleEnvironmentTilemap_Rock,
        .entryTileset = gBattleEnvironmentAnimTiles_Rock,
        .entryTilemap = gBattleEnvironmentAnimTilemap_Rock,
        .palette = gBattleEnvironmentPalette_Rock,
    },

    [BATTLE_ENVIRONMENT_CAVE] =
    {
        .tileset = gBattleEnvironmentTiles_Cave,
        .tilemap = gBattleEnvironmentTilemap_Cave,
        .entryTileset = gBattleEnvironmentAnimTiles_Cave,
        .entryTilemap = gBattleEnvironmentAnimTilemap_Cave,
        .palette = gBattleEnvironmentPalette_Cave,
    },

    [BATTLE_ENVIRONMENT_BUILDING] =
    {
        .tileset = gBattleEnvironmentTiles_Building,
        .tilemap = gBattleEnvironmentTilemap_Building,
        .entryTileset = gBattleEnvironmentAnimTiles_Building,
        .entryTilemap = gBattleEnvironmentAnimTilemap_Building,
        .palette = gBattleEnvironmentPalette_Building,
    },

    [BATTLE_ENVIRONMENT_PLAIN] =
    {
        .tileset = gBattleEnvironmentTiles_Building,
        .tilemap = gBattleEnvironmentTilemap_Building,
        .entryTileset = gBattleEnvironmentAnimTiles_Building,
        .entryTilemap = gBattleEnvironmentAnimTilemap_Building,
        .palette = gBattleEnvironmentPalette_Plain,
    },
};

static void CB2_UnusedBattleInit(void);

void UnusedBattleInit(void)
{
    u8 spriteId;
    ResetSpriteData();
    spriteId = CreateSprite(&gUnusedBattleInitSprite, 0, 0, 0);
    gSprites[spriteId].invisible = TRUE;
    SetMainCallback2(CB2_UnusedBattleInit);
}

static void CB2_UnusedBattleInit(void)
{
    AnimateSprites();
    BuildOamBuffer();
}

void sub_800D6D4(void)
{
    u16 ime = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = ime;
    REG_DISPSTAT = DISPSTAT_VBLANK_INTR;
    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(24) | BGCNT_TXT256x512;
    REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(1) | BGCNT_16COLOR | BGCNT_SCREENBASE(28) | BGCNT_TXT256x512;
    REG_BG2CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(1) | BGCNT_16COLOR | BGCNT_SCREENBASE(30) | BGCNT_TXT512x256;
    REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(2) | BGCNT_16COLOR | BGCNT_SCREENBASE(26) | BGCNT_TXT512x256;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_BLDY = 0;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_OBJWIN_ON;
}

void ApplyPlayerChosenFrameToBattleMenu(void)
{
    TextWindow_SetBaseTileNum(0x12);
    TextWindow_LoadStdFrameGraphicsOverridePal(&gWindowTemplate_Contest_MoveDescription, 1);
    TextWindow_SetBaseTileNum(0x22);
    TextWindow_LoadStdFrameGraphicsOverridePal(&gWindowTemplate_Contest_MoveDescription, 1);
    gPlttBufferUnfaded[BG_PLTT_ID(5) + 12] = RGB_CYAN;
    gPlttBufferUnfaded[BG_PLTT_ID(5) + 13] = RGB(9, 9, 9);
    gPlttBufferUnfaded[BG_PLTT_ID(5) + 14] = RGB_WHITE;
    gPlttBufferUnfaded[BG_PLTT_ID(5) + 15] = RGB(26, 26, 25);
    CpuSet(&gPlttBufferUnfaded[BG_PLTT_ID(5) + 12], &gPlttBufferFaded[BG_PLTT_ID(5) + 12], PLTT_SIZEOF(4) / sizeof(u16));
    FillAroundBattleWindows();
}

void DrawMainBattleBackground(void)
{
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_EREADER_TRAINER /*| BATTLE_TYPE_x2000000*/))
    {
        LZDecompressVram(gBattleEnvironmentTiles_Building, (void*)(BG_CHAR_ADDR(2)));
        LZDecompressVram(gBattleEnvironmentTilemap_Building, (void*)(BG_SCREEN_ADDR(26)));
        LoadCompressedPalette(gBattleEnvironmentPalette_BattleTower, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON)
    {
        if (gGameVersion == VERSION_RUBY)
        {
            LZDecompressVram(gBattleEnvironmentTiles_Cave, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Cave, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_Groudon, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
        }
        else
        {
            LZDecompressVram(gBattleEnvironmentTiles_Water, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Water, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_Kyogre, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
        }
    }
    else
    {
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            u8 trainerClass = gTrainers[gTrainerBattleOpponent_A].trainerClass;
            if (trainerClass == TRAINER_CLASS_LEADER)
            {
                LZDecompressVram(gBattleEnvironmentTiles_Building, (void*)(BG_CHAR_ADDR(2)));
                LZDecompressVram(gBattleEnvironmentTilemap_Building, (void*)(BG_SCREEN_ADDR(26)));
                LoadCompressedPalette(gBattleEnvironmentPalette_BuildingLeader, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                return;
            }
            else if (trainerClass == TRAINER_CLASS_CHAMPION)
            {
                LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void*)(BG_CHAR_ADDR(2)));
                LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void*)(BG_SCREEN_ADDR(26)));
                LoadCompressedPalette(gBattleEnvironmentPalette_StadiumSteven, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                return;
            }
        }

        switch (GetCurrentMapBattleScene())
        {
        case MAP_BATTLE_SCENE_NORMAL:
            LZDecompressVram(sBattleEnvironmentTable[gBattleEnvironment].tileset, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(sBattleEnvironmentTable[gBattleEnvironment].tilemap, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(sBattleEnvironmentTable[gBattleEnvironment].palette, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_GYM:
            LZDecompressVram(gBattleEnvironmentTiles_Building, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Building, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_BuildingGym, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_MAGMA:
            LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_StadiumMagma, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_AQUA:
            LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_StadiumAqua, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_SIDNEY:
            LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_StadiumSidney, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_PHOEBE:
            LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_StadiumPhoebe, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_GLACIA:
            LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_StadiumGlacia, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_DRAKE:
            LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_StadiumDrake, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        case MAP_BATTLE_SCENE_BATTLE_TOWER:
            LZDecompressVram(gBattleEnvironmentTiles_Building, (void*)(BG_CHAR_ADDR(2)));
            LZDecompressVram(gBattleEnvironmentTilemap_Building, (void*)(BG_SCREEN_ADDR(26)));
            LoadCompressedPalette(gBattleEnvironmentPalette_BattleTower, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        }
    }
}

void LoadBattleTextboxAndBackground(void)
{
    LZDecompressVram(gBattleTextboxTiles, (void*)(BG_CHAR_ADDR(0)));
    CpuSet(gBattleTextboxTilemap, (void *)(BG_SCREEN_ADDR(24)), 2 * BG_SCREEN_SIZE / sizeof(u16));
    LoadCompressedPalette(gBattleTextboxPalette, BG_PLTT_ID(0), 2 * PLTT_SIZE_4BPP);
    ApplyPlayerChosenFrameToBattleMenu();

    DrawMainBattleBackground();

    #if DEBUG
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        debug_sub_8008218((void*)(VRAM + 0x600), 0, (void*)(VRAM + 0xC000), 1);
        debug_sub_8008264(257, 3, 1, 3, 1);
        debug_sub_8008264(257, 3, 21, 3, 1);
        debug_sub_8008264(257, 3, 41, 3, 1);
    }
    #endif
}

static void sub_800DAF8(u8 taskId, u8 multiplayerId, u8 *dest)
{
    int i;
    u16 pokeballStatuses = 0;
    u16 tiles[PARTY_SIZE];

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI) {
        if (gTasks[taskId].data[5]) {
            switch (multiplayerId) {
            case 0:
                pokeballStatuses = gTasks[taskId].data[3] & 0x3f;
                break;
            case 1:
                pokeballStatuses = (gTasks[taskId].data[4] & 0xfc0) >> 6;
                break;
            case 2:
                pokeballStatuses = (gTasks[taskId].data[3] & 0xfc0) >> 6;
                break;
            case 3:
                pokeballStatuses = gTasks[taskId].data[4] & 0x3f;
                break;
            }
        } else {
            switch (multiplayerId) {
            case 0:
                pokeballStatuses = gTasks[taskId].data[3] & 0x3f;
                break;
            case 1:
                pokeballStatuses = gTasks[taskId].data[4] & 0x3f;
                break;
            case 2:
                pokeballStatuses = (gTasks[taskId].data[3] & 0xfc0) >> 6;
                break;
            case 3:
                pokeballStatuses = (gTasks[taskId].data[4] & 0xfc0) >> 6;
                break;
            }
        }
        for (i = 0; i < PARTY_SIZE / 2; i++) {
            tiles[i] = ((pokeballStatuses & (3 << (i * 2))) >> (i * 2)) + 0x6001;
        }
        CpuSet(tiles, dest, PARTY_SIZE / 2);
    } else {
        if (multiplayerId == gBattleStruct->multiplayerId) {
            pokeballStatuses = gTasks[taskId].data[3];
        } else {
            pokeballStatuses = gTasks[taskId].data[4];
        }
        for (i = 0; i < PARTY_SIZE; i++) {
            tiles[i] = ((pokeballStatuses & (3 << (i * 2))) >> (i * 2)) + 0x6001;
        }
        CpuSet(tiles, dest, PARTY_SIZE);
    }
}


#if ENGLISH
#define LEFT_MESSAGE_X 6
#define RIGHT_MESSAGE_X 21
#define TILE_OFFSET_LOSS 168
#elif GERMAN
#define LEFT_MESSAGE_X 5
#define RIGHT_MESSAGE_X 20
#define TILE_OFFSET_LOSS 172
#endif
#define TILE_OFFSET_WIN 160
#define CENTER_MESSAGE_X 13
#define MESSAGE_Y 2

#define PRINT_MESSAGE(text, tileDataStartOffset, x)                             \
{                                                                               \
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, text, tileDataStartOffset, x, MESSAGE_Y);    \
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);                                            \
}

#define PRINT_MESSAGE_LEFT(text, tileDataStartOffset)       PRINT_MESSAGE(text, tileDataStartOffset, LEFT_MESSAGE_X)
#define PRINT_MESSAGE_RIGHT(text, tileDataStartOffset)      PRINT_MESSAGE(text, tileDataStartOffset, RIGHT_MESSAGE_X)

static void PrintLinkBattleWinLossTie(void)
{

    if (gBattleOutcome == B_OUTCOME_DREW)
    {
        PRINT_MESSAGE(gText_Draw, TILE_OFFSET_WIN, CENTER_MESSAGE_X);
        return;
    }

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        // Double battle?

        if (gBattleOutcome == B_OUTCOME_WON)
        {

            // id = player position?
            switch (gLinkPlayers[gBattleStruct->multiplayerId].id)
            {
            case 0:
            case 2:
                PRINT_MESSAGE_LEFT(gText_Win, TILE_OFFSET_WIN);
                PRINT_MESSAGE_RIGHT(gText_Loss, TILE_OFFSET_LOSS);
                return;

            case 1:
            case 3:
                PRINT_MESSAGE_RIGHT(gText_Win, TILE_OFFSET_WIN)
                PRINT_MESSAGE_LEFT(gText_Loss, TILE_OFFSET_LOSS)
                return;
            }
        }
        else
        {

            switch (gLinkPlayers[gBattleStruct->multiplayerId].id)
            {
            case 1:
            case 3:
                PRINT_MESSAGE_LEFT(gText_Win, TILE_OFFSET_WIN);
                PRINT_MESSAGE_RIGHT(gText_Loss, TILE_OFFSET_LOSS);
                return;

            case 0:
            case 2:
                PRINT_MESSAGE_RIGHT(gText_Win, TILE_OFFSET_WIN);
                PRINT_MESSAGE_LEFT(gText_Loss, TILE_OFFSET_LOSS);
                return;
            }
        }

        return;
    }


    if (gBattleOutcome == B_OUTCOME_WON)
    {
        if (gLinkPlayers[gBattleStruct->multiplayerId].id != 0)
        {
            PRINT_MESSAGE_RIGHT(gText_Win, TILE_OFFSET_WIN);
            PRINT_MESSAGE_LEFT(gText_Loss, TILE_OFFSET_LOSS);
        }
        else
        {
            PRINT_MESSAGE_LEFT(gText_Win, TILE_OFFSET_WIN);
            PRINT_MESSAGE_RIGHT(gText_Loss, TILE_OFFSET_LOSS);
        }
    }
    else
    {
        if (gLinkPlayers[gBattleStruct->multiplayerId].id != 0)
        {
            PRINT_MESSAGE_LEFT(gText_Win, TILE_OFFSET_WIN);
            PRINT_MESSAGE_RIGHT(gText_Loss, TILE_OFFSET_LOSS);
        }
        else
        {
            PRINT_MESSAGE_RIGHT(gText_Win, TILE_OFFSET_WIN);
            PRINT_MESSAGE_LEFT(gText_Loss, TILE_OFFSET_LOSS);
        }
    }
}


void InitLinkBattleVsScreen(u8 taskId)
{
    u8 palId;
    int i;

    switch (gTasks[taskId].data[0]) {

    case 0:
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI) {
            for (i = 0; i < MAX_LINK_PLAYERS; i++) {
                u8 windowId = (gLinkPlayers[i].id & (MAX_LINK_PLAYERS - 1));
                Text_InitWindow8002E4C(
                    gLinkResultWindows[windowId].window,
                    gLinkPlayers[i].name,
                    gLinkResultWindows[windowId].offset,
                    gLinkResultWindows[windowId].left,
                    gLinkResultWindows[windowId].top,
                    1);
                Text_PrintWindow8002F44(gLinkResultWindows[windowId].window);
                sub_800DAF8(taskId, windowId, gLinkResultWindows[windowId].dest);
            }
        } else {
            u8 windowId = 4;

            u8 playerId = gBattleStruct->multiplayerId;
            u8 opponentId = gBattleStruct->multiplayerId ^ BIT_SIDE;
            if (gLinkPlayers[playerId].id) {
                opponentId = gBattleStruct->multiplayerId;
                playerId = gBattleStruct->multiplayerId ^ BIT_SIDE;
            }

            Text_InitWindow8002E4C(
                gLinkResultWindows[windowId].window,
                gLinkPlayers[playerId].name,
                gLinkResultWindows[windowId].offset,
                gLinkResultWindows[windowId].left,
                gLinkResultWindows[windowId].top,
                1);
            Text_PrintWindow8002F44(gLinkResultWindows[windowId].window);
            sub_800DAF8(taskId, playerId, gLinkResultWindows[windowId].dest);

            Text_InitWindow8002E4C(
                gLinkResultWindows[windowId + 1].window,
                gLinkPlayers[opponentId].name,
                gLinkResultWindows[windowId + 1].offset,
                gLinkResultWindows[windowId + 1].left,
                gLinkResultWindows[windowId + 1].top,
                1);
            Text_PrintWindow8002F44(gLinkResultWindows[windowId + 1].window);
            sub_800DAF8(taskId, opponentId, gLinkResultWindows[windowId + 1].dest);
        }
        gTasks[taskId].data[0]++;
        break;

    case 1:
        palId = AllocSpritePalette(TAG_VS_LETTERS);
        gPlttBufferUnfaded[OBJ_PLTT_ID(palId) + 15] = gPlttBufferFaded[OBJ_PLTT_ID(palId) + 15] = RGB_WHITE;
        gBattleStruct->linkBattleVsSpriteId_V = CreateSprite(&sVsLetter_V_SpriteTemplate, 108, 80, 0);
        gBattleStruct->linkBattleVsSpriteId_S = CreateSprite(&sVsLetter_S_SpriteTemplate, 132, 80, 0);
        gSprites[gBattleStruct->linkBattleVsSpriteId_V].invisible = TRUE;
        gSprites[gBattleStruct->linkBattleVsSpriteId_S].invisible = TRUE;
        gTasks[taskId].data[0]++;
        break;

    case 2:
        if (gTasks[taskId].data[5]) {
            gBattle_BG1_X = (-20) - (Sin2(gTasks[taskId].data[1]) / 32);
            gBattle_BG2_X = (-140) - (Sin2(gTasks[taskId].data[2]) / 32);
            gBattle_BG1_Y = -36;
            gBattle_BG2_Y = -36;
        } else {
            gBattle_BG1_X = (-20) - (Sin2(gTasks[taskId].data[1]) / 32);
            gBattle_BG1_Y = (-164) + (Cos2(gTasks[taskId].data[1]) / 32);
            gBattle_BG2_X = (-140) - (Sin2(gTasks[taskId].data[2]) / 32);
            gBattle_BG2_Y = (-164) + (Cos2(gTasks[taskId].data[2]) / 32);
        }
        if (gTasks[taskId].data[2]) {
            gTasks[taskId].data[2] -= 2;
            gTasks[taskId].data[1] += 2;
        } else {
            if (gTasks[taskId].data[5]) {
                PrintLinkBattleWinLossTie();
            }
            PlaySE(SE_M_HARDEN);
            DestroyTask(taskId);
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].invisible = FALSE;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].invisible = FALSE;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].oam.tileNum += 0x40;
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].data[0] = 0;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].data[0] = 1;
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].data[1] = gSprites[gBattleStruct->linkBattleVsSpriteId_V].x;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].data[1] = gSprites[gBattleStruct->linkBattleVsSpriteId_S].x;
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].data[2] = 0;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].data[2] = 0;
        }
        break;
    }
}

void DrawBattleEntryBackground(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        LZDecompressVram(gBattleVSFrame_Gfx, (void *)(BG_CHAR_ADDR(1)));
        LZDecompressVram(gBattleVSFrame_Tilemap, (void *)(BG_SCREEN_ADDR(28)));
        LZDecompressVram(gBattleVSFrame_Tilemap, (void *)(BG_SCREEN_ADDR(30)));
        LZDecompressVram(gVsLettersGfx, (void *)OBJ_VRAM0);
        LoadCompressedPalette(gBattleVSFrame_Pal, BG_PLTT_ID(6), PLTT_SIZE_4BPP);
        REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(1) | BGCNT_16COLOR | BGCNT_SCREENBASE(28) | BGCNT_TXT512x256;
        REG_WININ = WININ_WIN0_BG1 | WININ_WIN0_BG2 | WININ_WIN0_OBJ | WININ_WIN0_CLR;
        REG_WINOUT = WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR;
        gBattle_BG1_Y = 0xff5c;
        gBattle_BG2_Y = 0xff5c;
        LoadCompressedSpriteSheet(sVsLettersSpriteSheet);
    }
    else if (gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK))
    {
        LZDecompressVram(gBattleEnvironmentAnimTiles_Building, (void *)(BG_CHAR_ADDR(1)));
        LZDecompressVram(gBattleEnvironmentAnimTilemap_Building, (void *)(BG_SCREEN_ADDR(28)));
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON)
    {
        if (gGameVersion == VERSION_RUBY)
        {
            LZDecompressVram(gBattleEnvironmentAnimTiles_Cave, (void *)(BG_CHAR_ADDR(1)));
            LZDecompressVram(gBattleEnvironmentAnimTilemap_Cave, (void *)(BG_SCREEN_ADDR(28)));
        }
        else
        {
            LZDecompressVram(gBattleEnvironmentAnimTiles_Underwater, (void *)(BG_CHAR_ADDR(1)));
            LZDecompressVram(gBattleEnvironmentAnimTilemap_Underwater, (void *)(BG_SCREEN_ADDR(28)));
        }
    }
    else
    {
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_LEADER)
            {
                LZDecompressVram(gBattleEnvironmentAnimTiles_Building, (void *)(BG_CHAR_ADDR(1)));
                LZDecompressVram(gBattleEnvironmentAnimTilemap_Building, (void *)(BG_SCREEN_ADDR(28)));
                return;
            }
            else if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_CHAMPION)
            {
                LZDecompressVram(gBattleEnvironmentAnimTiles_Building, (void *)(BG_CHAR_ADDR(1)));
                LZDecompressVram(gBattleEnvironmentAnimTilemap_Building, (void *)(BG_SCREEN_ADDR(28)));
                return;
            }
        }
        if (GetCurrentMapBattleScene() == MAP_BATTLE_SCENE_NORMAL)
        {
            LZDecompressVram(sBattleEnvironmentTable[gBattleEnvironment].entryTileset, (void *)(BG_CHAR_ADDR(1)));
            LZDecompressVram(sBattleEnvironmentTable[gBattleEnvironment].entryTilemap, (void *)(BG_SCREEN_ADDR(28)));
        }
        else
        {
            LZDecompressVram(gBattleEnvironmentAnimTiles_Building, (void *)(BG_CHAR_ADDR(1)));
            LZDecompressVram(gBattleEnvironmentAnimTilemap_Building, (void *)(BG_SCREEN_ADDR(28)));
        }
    }
}

int LoadChosenBattleElement(u8 caseId)
{
    int ret = FALSE;

    switch (caseId)
    {
    case 0:
        LZDecompressVram(&gBattleTextboxTiles, (void *)(BG_CHAR_ADDR(0)));
        break;
    case 1:
        CpuCopy16(gBattleTextboxTilemap, (void *)(BG_SCREEN_ADDR(24)), 2 * BG_SCREEN_SIZE);
        break;
    case 2:
        LoadCompressedPalette(gBattleTextboxPalette, BG_PLTT_ID(0), 2 * PLTT_SIZE_4BPP);
        break;
    case 3: // tiles
        if (!(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK)))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON)
            {
                if (gGameVersion == VERSION_RUBY)
                {
                    LZDecompressVram(gBattleEnvironmentTiles_Cave, (void *)(BG_CHAR_ADDR(2)));
                    break;
                }
                else
                {
                    LZDecompressVram(gBattleEnvironmentTiles_Water, (void *)(BG_CHAR_ADDR(2)));
                    break;
                }
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                {
                    if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_LEADER)
                    {
                        LZDecompressVram(gBattleEnvironmentTiles_Building, (void *)(BG_CHAR_ADDR(2)));
                        break;
                    }
                    else if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_CHAMPION)
                    {
                        LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void *)(BG_CHAR_ADDR(2)));
                        break;
                    }
                }
                switch (GetCurrentMapBattleScene())
                {
                case MAP_BATTLE_SCENE_NORMAL:
                    LZDecompressVram(sBattleEnvironmentTable[gBattleEnvironment].tileset, (void *)(BG_CHAR_ADDR(2)));
                    break;
                case MAP_BATTLE_SCENE_MAGMA:
                    LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void *)(BG_CHAR_ADDR(2)));
                    break;
                case MAP_BATTLE_SCENE_AQUA:
                    LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void *)(BG_CHAR_ADDR(2)));
                    break;
                case MAP_BATTLE_SCENE_SIDNEY:
                    LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void *)(BG_CHAR_ADDR(2)));
                    break;
                case MAP_BATTLE_SCENE_PHOEBE:
                    LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void *)(BG_CHAR_ADDR(2)));
                    break;
                case MAP_BATTLE_SCENE_GLACIA:
                    LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void *)(BG_CHAR_ADDR(2)));
                    break;
                case MAP_BATTLE_SCENE_DRAKE:
                    LZDecompressVram(gBattleEnvironmentTiles_Stadium, (void *)(BG_CHAR_ADDR(2)));
                    break;
                case MAP_BATTLE_SCENE_GYM:
                case MAP_BATTLE_SCENE_BATTLE_TOWER:
                    LZDecompressVram(gBattleEnvironmentTiles_Building, (void *)(BG_CHAR_ADDR(2)));
                    break;
                }
                break;
            }
        }
        else
        {
            LZDecompressVram(gBattleEnvironmentTiles_Building, (void *)(BG_CHAR_ADDR(2)));
            break;
        }
    case 4: // tilemap
        if (!(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK)))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON)
            {
                if (gGameVersion == VERSION_RUBY)
                {
                    LZDecompressVram(gBattleEnvironmentTilemap_Cave, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                }
                else
                {
                    LZDecompressVram(gBattleEnvironmentTilemap_Water, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                }
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                {
                    if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_LEADER)
                    {
                        LZDecompressVram(gBattleEnvironmentTilemap_Building, (void *)(BG_SCREEN_ADDR(26)));
                        break;
                    }
                    else if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_CHAMPION)
                    {
                        LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void *)(BG_SCREEN_ADDR(26)));
                        break;
                    }
                }
                switch (GetCurrentMapBattleScene())
                {
                case MAP_BATTLE_SCENE_NORMAL:
                    LZDecompressVram(sBattleEnvironmentTable[gBattleEnvironment].tilemap, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                case MAP_BATTLE_SCENE_MAGMA:
                    LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                case MAP_BATTLE_SCENE_AQUA:
                    LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                case MAP_BATTLE_SCENE_SIDNEY:
                    LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                case MAP_BATTLE_SCENE_PHOEBE:
                    LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                case MAP_BATTLE_SCENE_GLACIA:
                    LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                case MAP_BATTLE_SCENE_DRAKE:
                    LZDecompressVram(gBattleEnvironmentTilemap_Stadium, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                case MAP_BATTLE_SCENE_GYM:
                case MAP_BATTLE_SCENE_BATTLE_TOWER:
                    LZDecompressVram(gBattleEnvironmentTilemap_Building, (void *)(BG_SCREEN_ADDR(26)));
                    break;
                }
                break;
            }
        }
        else
        {
            LZDecompressVram(gBattleEnvironmentTilemap_Building, (void *)(BG_SCREEN_ADDR(26)));
            break;
        }
    case 5: // palette
        if (!(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK)))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON)
            {
                if (gGameVersion == VERSION_RUBY)
                {
                    LoadCompressedPalette(gBattleEnvironmentPalette_Groudon, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                }
                else
                {
                    LoadCompressedPalette(gBattleEnvironmentPalette_Kyogre, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                }
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                {
                    if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_LEADER)
                    {
                        LoadCompressedPalette(gBattleEnvironmentPalette_BuildingLeader, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                        break;
                    }
                    else if (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_CHAMPION)
                    {
                        LoadCompressedPalette(gBattleEnvironmentPalette_StadiumSteven, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                        break;
                    }
                }
                switch (GetCurrentMapBattleScene())
                {
                case MAP_BATTLE_SCENE_NORMAL:
                    LoadCompressedPalette(sBattleEnvironmentTable[gBattleEnvironment].palette, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_GYM:
                    LoadCompressedPalette(gBattleEnvironmentPalette_BuildingGym, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_MAGMA:
                    LoadCompressedPalette(gBattleEnvironmentPalette_StadiumMagma, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_AQUA:
                    LoadCompressedPalette(gBattleEnvironmentPalette_StadiumAqua, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_SIDNEY:
                    LoadCompressedPalette(gBattleEnvironmentPalette_StadiumSidney, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_PHOEBE:
                    LoadCompressedPalette(gBattleEnvironmentPalette_StadiumPhoebe, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_GLACIA:
                    LoadCompressedPalette(gBattleEnvironmentPalette_StadiumGlacia, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_DRAKE:
                    LoadCompressedPalette(gBattleEnvironmentPalette_StadiumDrake, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                case MAP_BATTLE_SCENE_BATTLE_TOWER:
                    LoadCompressedPalette(gBattleEnvironmentPalette_BattleTower, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
                    break;
                }
                break;
            }
        }
        else
        {
            LoadCompressedPalette(gBattleEnvironmentPalette_BattleTower, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            break;
        }
    case 6:
        ApplyPlayerChosenFrameToBattleMenu();
        break;
    default:
        ret = TRUE;
        break;
    }
    return ret;
}
