#include "global.h"
#include "constants/rgb.h"
#include "sprite.h"
#include "graphics.h"
#include "main.h"
#include "task.h"
#include "palette.h"
#include "sound.h"
#include "constants/songs.h"
#include "pokemon.h"
#include "text.h"
#include "strings.h"
#include "string_util.h"
#include "menu.h"
#include "save.h"
#include "constants/species.h"
#include "overworld.h"
#include "m4a.h"
#include "data.h"
#include "decompress.h"
#include "random.h"
#include "scanline_effect.h"
#include "trig.h"
#include "hall_of_fame.h"
#include "credits.h"
#include "pc_screen_effect.h"
#include "ewram.h"
static EWRAM_DATA u32 sHofFadePalettes = 0;

extern void (*gGameContinueCallback)(void);

extern const u8 gHallOfFame_Gfx[];
extern const u16 gHallOfFame_Pal[];

struct HallofFameMon
{
    u32 tid;
    u32 personality;
    u16 species : 9;
    u16 lvl : 7;
    u8 nickname[10];
};

struct HallofFameTeam
{
    struct HallofFameMon mon[6];
};

#define HALL_OF_FAME_MAX_TEAMS 50

static void Task_Hof_InitMonData(u8 taskId);
static void Task_Hof_SetMonDisplayTask(u8 taskId);
static void Task_Hof_InitTeamSaveData(u8 taskId);
static void Task_Hof_TrySaveData(u8 taskId);
static void Task_Hof_WaitToDisplayMon(u8 taskId);
static void Task_Hof_DisplayMon(u8 taskId);
static void Task_Hof_PrintMonInfoAfterAnimating(u8 taskId);
static void Task_Hof_TryDisplayAnotherMon(u8 taskId);
static void Task_Hof_PaletteFadeAndPrintWelcomeText(u8 taskId);
static void Task_Hof_DoConfetti(u8 taskId);
static void Task_Hof_WaitToDisplayPlayer(u8 taskId);
static void Task_Hof_DisplayPlayer(u8 taskId);
static void Task_Hof_WaitAndPrintPlayerInfo(u8 taskId);
static void Task_Hof_ExitOnKeyPressed(u8 taskId);
static void Task_Hof_HandlePaletteOnExit(u8 taskId);
static void Task_Hof_HandleExit(u8 taskId);
static void sub_8142A28(u8 taskId);
static void Task_HofPC_PrintDataIsCorrupted(u8 taskId);
static void Task_HofPC_DrawSpritesPrintText(u8 taskId);
static void Task_HofPC_PrintMonInfo(u8 taskId);
static void Task_HofPC_HandleInput(u8 taskId);
static void Task_HofPC_HandlePaletteOnExit(u8 taskId);
static void Task_HofPC_HandleExit(u8 taskId);
static void Task_HofPC_ExitOnButtonPress(u8 taskId);

static void sub_81435DC(struct Sprite* sprite);
static void SpriteCB_HofConfetti(struct Sprite* sprite);
static void SpriteCB_HallOfFame_Dummy(struct Sprite* sprite);

static void HallOfFame_PrintWelcomeText(u8 unusedPossiblyWindowId, u8 a1);
static void HallOfFame_PrintMonInfo(struct HallofFameMon* currMon, u8 a1, u8 a2);
static void HallOfFame_PrintPlayerInfo(u8 a0, u8 a1);
static void ClearVramOamPltt_LoadHofPal(void);
static void LoadHofGfx(void);
static void InitHofBgs(void);
static u32 HallOfFame_LoadPokemonPic(u16 species, s16 posX, s16 posY, u16 pokeID, u32 otId, u32 personality);
static u32 HallOfFame_LoadTrainerPic(u16 trainerPicId, s16 posX, s16 posY, u16 a3);
static bool8 CreateHofConfettiSprite(void);

// data and gfx

static const struct CompressedSpriteSheet sSpriteSheet_Confetti =
{
      gConfetti_Gfx, 0x220, 1001
};

static const u8 sUnused0[8] = {};

static const struct CompressedSpritePalette sSpritePalette_Confetti =
{
      gConfetti_Pal, 1001
};

static const u8 sUnused1[8] = {};

static const s16 sHallOfFame_MonFullTeamPositions[6][4] =
{
    {120,   210,    120,    40},
    {326,   220,    56,     40},
    {-86,   220,    184,    40},
    {120,   -62,    120,    88},
    {-25,   -62,    200,    88},
    {265,   -62,    40,     88}
};

static const s16 sHallOfFame_MonHalfTeamPositions[3][4] =
{
    {120,   214,    120,    64},
    {281,   214,    56,     64},
    {-41,   214,    184,    64}
};

static const struct PCScreenEffectStruct sPCScreenEffectTemplate = {
    .tileTag = 0x3ea,
    .paletteTag = 0x3ea
};

static const u8 sHallOfFame_SlotOrder[6] = {2, 1, 3, 6, 4, 5};

static const struct OamData sOamData_840B598 =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

void* const gUnknown_0840B5A0[] =
{
    eHofGfxPtr + 0x0000,
    eHofGfxPtr + 0x2000,
    eHofGfxPtr + 0x4000,
    eHofGfxPtr + 0x6000,
    eHofGfxPtr + 0x8000,
    eHofGfxPtr + 0xC000,
    eHofGfxPtr + 0x10000
};

static const struct SpriteFrameImage sSpriteImageTable_840B5BC[] =
{
    {eHofGfxPtr + 0x0000, 0x800},
    {eHofGfxPtr + 0x800, 0x800},
    {eHofGfxPtr + 0x1000, 0x800},
    {eHofGfxPtr + 0x1800, 0x800}
};

static const struct SpriteFrameImage sSpriteImageTable_840B5DC[] =
{
    {eHofGfxPtr + 0x2000, 0x800},
    {eHofGfxPtr + 0x2800, 0x800},
    {eHofGfxPtr + 0x3000, 0x800},
    {eHofGfxPtr + 0x3800, 0x800}
};

static const struct SpriteFrameImage sSpriteImageTable_840B5FC[] =
{
    {eHofGfxPtr + 0x4000, 0x800},
    {eHofGfxPtr + 0x4800, 0x800},
    {eHofGfxPtr + 0x5000, 0x800},
    {eHofGfxPtr + 0x5800, 0x800}
};

static const struct SpriteFrameImage sSpriteImageTable_840B61C[] =
{
    {eHofGfxPtr + 0x6000, 0x800},
    {eHofGfxPtr + 0x6800, 0x800},
    {eHofGfxPtr + 0x7000, 0x800},
    {eHofGfxPtr + 0x7800, 0x800}
};

static const struct SpriteFrameImage sSpriteImageTable_840B63C[] =
{
    {eHofGfxPtr + 0x8000, 0x800},
    {eHofGfxPtr + 0x8800, 0x800},
    {eHofGfxPtr + 0x9000, 0x800},
    {eHofGfxPtr + 0x9800, 0x800}
};

static const struct SpriteFrameImage sSpriteImageTable_840B65C[] =
{
    {eHofGfxPtr + 0xC000, 0x800},
    {eHofGfxPtr + 0xC800, 0x800},
    {eHofGfxPtr + 0xD000, 0x800},
    {eHofGfxPtr + 0xD800, 0x800}
};

static const struct SpriteFrameImage sSpriteImageTable_840B67C[] =
{
    {eHofGfxPtr + 0x10000, 0x800},
    {eHofGfxPtr + 0x10800, 0x800},
    {eHofGfxPtr + 0x11000, 0x800},
    {eHofGfxPtr + 0x11800, 0x800}
};

static const struct SpriteFrameImage* const sUnknown_0840B69C[7] =
{
    sSpriteImageTable_840B5BC,
    sSpriteImageTable_840B5DC,
    sSpriteImageTable_840B5FC,
    sSpriteImageTable_840B61C,
    sSpriteImageTable_840B63C,
    sSpriteImageTable_840B65C,
    sSpriteImageTable_840B67C
};

static const struct SpriteTemplate sUnknown_0840B6B8 =
{
    .tileTag = -1,
    .paletteTag = -1,
    .oam = &sOamData_840B598,
    .anims = NULL,
    .images = sSpriteImageTable_840B5BC,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_HallOfFame_Dummy
};

static const struct OamData sOamData_Confetti =
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
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_PinkConfettiA[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_RedConfettiA[] =
{
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_BlueConfettiA[] =
{
    ANIMCMD_FRAME(2, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_RedConfettiB[] =
{
    ANIMCMD_FRAME(3, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_BlueConfettiB[] =
{
    ANIMCMD_FRAME(4, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_YellowConfettiA[] =
{
    ANIMCMD_FRAME(5, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_WhiteConfettiA[] =
{
    ANIMCMD_FRAME(6, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_GreenConfettiA[] =
{
    ANIMCMD_FRAME(7, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PinkConfettiB[] =
{
    ANIMCMD_FRAME(8, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_BlueConfettiC[] =
{
    ANIMCMD_FRAME(9, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_YellowConfettiB[] =
{
    ANIMCMD_FRAME(10, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_WhiteConfettiB[] =
{
    ANIMCMD_FRAME(11, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_GreenConfettiB[] =
{
    ANIMCMD_FRAME(12, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PinkConfettiC[] =
{
    ANIMCMD_FRAME(13, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_RedConfettiC[] =
{
    ANIMCMD_FRAME(14, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_YellowConfettiC[] =
{
    ANIMCMD_FRAME(15, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_WhiteConfettiC[] =
{
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END
};

static const union AnimCmd* const sAnims_Confetti[] =
{
    sAnim_PinkConfettiA,
    sAnim_RedConfettiA,
    sAnim_BlueConfettiA,
    sAnim_RedConfettiB,
    sAnim_BlueConfettiB,
    sAnim_YellowConfettiA,
    sAnim_WhiteConfettiA,
    sAnim_GreenConfettiA,
    sAnim_PinkConfettiB,
    sAnim_BlueConfettiC,
    sAnim_YellowConfettiB,
    sAnim_WhiteConfettiB,
    sAnim_GreenConfettiB,
    sAnim_PinkConfettiC,
    sAnim_RedConfettiC,
    sAnim_YellowConfettiC,
    sAnim_WhiteConfettiC
};

static const struct SpriteTemplate sSpriteTemplate_HofConfetti =
{
    .tileTag = 1001,
    .paletteTag = 1001,
    .oam = &sOamData_Confetti,
    .anims = sAnims_Confetti,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_HofConfetti
};

// code

#define tDisplayedMonId     data[1]
#define tMonNumber          data[2]
#define tFrameCount         data[3]
#define tPlayerSpriteID     data[4]
#define tMonSpriteId(i)     data[i + 5]

static void VBlankCB_HallOfFame(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_HallOfFame(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static bool8 InitHallOfFameScreen(void)
{
    switch (gMain.state)
    {
    case 0:
    default:
        SetVBlankCallback(NULL);
        ClearVramOamPltt_LoadHofPal();
        gMain.state = 1;
        break;
    case 1:
        LoadHofGfx();
        gMain.state++;
        break;
    case 2:
        {
            u16 saved_IME;

            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            SetVBlankCallback(VBlankCB_HallOfFame);
            saved_IME = REG_IME;
            REG_IME = 0;
            REG_IE |= 1;
            REG_IME = saved_IME;
            REG_DISPSTAT |= 8;
            gMain.state++;
        }
        break;
    case 3:
        REG_BLDCNT = 0x3F42;
        REG_BLDALPHA = 0x710;
        REG_BLDY = 0;
        InitHofBgs();
        gMain.state++;
        break;
    case 4:
        UpdatePaletteFade();
        if (!gPaletteFade.active)
        {
            SetMainCallback2(CB2_HallOfFame);
            PlayBGM(MUS_HALL_OF_FAME);
            return 0;
        }
        break;
    }
    return 1;
}

void CB2_DoHallOfFameScreen(void)
{
    if (InitHallOfFameScreen() == 0)
    {
        u8 taskId = CreateTask(Task_Hof_InitMonData, 0);
        gTasks[taskId].data[0] = 0;
    }
}

void CB2_DoHallOfFameScreenDontSaveData(void)
{
    if (InitHallOfFameScreen() == 0)
    {
        u8 taskId = CreateTask(Task_Hof_InitMonData, 0);
        gTasks[taskId].data[0] = 1;
    }
}

static void Task_Hof_InitMonData(u8 taskId)
{
    u16 i, j;
    struct HallofFameTeam* fameMons = eHofMonPtr;

    gTasks[taskId].tMonNumber = 0; // valid pokes
    for (i = 0; i < 6; i++)
    {
        u8 nickname[12];
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
        {
            fameMons->mon[i].species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
            fameMons->mon[i].tid = GetMonData(&gPlayerParty[i], MON_DATA_OT_ID);
            fameMons->mon[i].personality = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY);
            fameMons->mon[i].lvl = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
            GetMonData(&gPlayerParty[i], MON_DATA_NICKNAME, nickname);
            for (j = 0; j < 10; j++)
            {
                fameMons->mon[i].nickname[j] = nickname[j];
            }
            gTasks[taskId].tMonNumber++;
        }
        else
        {
            fameMons->mon[i].species = 0;
            fameMons->mon[i].tid = 0;
            fameMons->mon[i].personality = 0;
            fameMons->mon[i].lvl = 0;
            fameMons->mon[i].nickname[0] = EOS;
        }
    }
    sHofFadePalettes = 0;
    gTasks[taskId].tDisplayedMonId = 0;
    gTasks[taskId].data[4] = 0xFF;
    for (i = 0; i < 6; i++)
    {
        gTasks[taskId].tMonSpriteId(i) = 0xFF;
    }
    if (gTasks[taskId].data[0])
        gTasks[taskId].func = Task_Hof_SetMonDisplayTask;
    else
        gTasks[taskId].func = Task_Hof_InitTeamSaveData;
}

static void Task_Hof_InitTeamSaveData(u8 taskId)
{
    u16 i;
    struct HallofFameTeam* fameMons = eHofMonPtr;
    struct HallofFameTeam* lastSavedTeam = (struct HallofFameTeam *)gDecompressionBuffer;

    if (gHasHallOfFameRecords == FALSE)
    {
        for (i = 0; i < 0x2000; i++)
            gSharedMem[0x1E000 + i] = 0; // gDecompressionBuffer[i] = 0;
    }
    else
        LoadGameSave(SAVE_HALL_OF_FAME);

    for (i = 0; i < HALL_OF_FAME_MAX_TEAMS; i++, lastSavedTeam++)
    {
        if (lastSavedTeam->mon[0].species == 0)
            break;
    }
    if (i >= HALL_OF_FAME_MAX_TEAMS)
    {
        struct HallofFameTeam *afterTeam = (struct HallofFameTeam *)gDecompressionBuffer;
        struct HallofFameTeam *beforeTeam = (struct HallofFameTeam *)gDecompressionBuffer;
        afterTeam++;
        for (i = 0; i < HALL_OF_FAME_MAX_TEAMS - 1; i++, beforeTeam++, afterTeam++)
        {
            *beforeTeam = *afterTeam;
        }
        lastSavedTeam--;
    }
    *lastSavedTeam = *fameMons;
    Menu_DrawStdWindowFrame(2, 14, 27, 19);
    Menu_PrintText(gText_HOFDexSaving, 3, 15);
    gTasks[taskId].func = Task_Hof_TrySaveData;
}

static void Task_Hof_TrySaveData(u8 taskId)
{
    gGameContinueCallback = CB2_DoHallOfFameScreenDontSaveData;
    TrySavingData(3);
    PlaySE(SE_SAVE);
    gTasks[taskId].func = Task_Hof_WaitToDisplayMon;
    gTasks[taskId].tFrameCount = 32;
}

static void Task_Hof_WaitToDisplayMon(u8 taskId)
{
    if (gTasks[taskId].tFrameCount)
        gTasks[taskId].tFrameCount--;
    else
        gTasks[taskId].func = Task_Hof_SetMonDisplayTask;
}

static void Task_Hof_SetMonDisplayTask(u8 taskId)
{
    Text_LoadWindowTemplate(&gWindowTemplate_81E7198);
    InitMenuWindow(&gWindowTemplate_81E7198);
    gTasks[taskId].func = Task_Hof_DisplayMon;
}

static void Task_Hof_DisplayMon(u8 taskId)
{
    u8 spriteId;
    s16 startX, startY, destX, destY;

    struct HallofFameTeam* fameMons = eHofMonPtr;
    u16 currMonId = gTasks[taskId].tDisplayedMonId;
    struct HallofFameMon* currMon = &fameMons->mon[currMonId];

    if (gTasks[taskId].tMonNumber > 3)
    {
        startX = sHallOfFame_MonFullTeamPositions[currMonId][0];
        startY = sHallOfFame_MonFullTeamPositions[currMonId][1];
        destX = sHallOfFame_MonFullTeamPositions[currMonId][2];
        destY = sHallOfFame_MonFullTeamPositions[currMonId][3];
    }
    else
    {
        startX = sHallOfFame_MonHalfTeamPositions[currMonId][0];
        startY = sHallOfFame_MonHalfTeamPositions[currMonId][1];
        destX = sHallOfFame_MonHalfTeamPositions[currMonId][2];
        destY = sHallOfFame_MonHalfTeamPositions[currMonId][3];
    }

    spriteId = HallOfFame_LoadPokemonPic(currMon->species, startX, startY, currMonId, currMon->tid, currMon->personality);
    gSprites[spriteId].data[1] = destX;
    gSprites[spriteId].data[2] = destY;
    gSprites[spriteId].data[0] = 0;
    gSprites[spriteId].callback = sub_81435DC;
    gTasks[taskId].tMonSpriteId(currMonId) = spriteId;
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = Task_Hof_PrintMonInfoAfterAnimating;
}

static void Task_Hof_PrintMonInfoAfterAnimating(u8 taskId)
{
    struct HallofFameTeam* fameMons = eHofMonPtr;
    u16 currMonId = gTasks[taskId].tDisplayedMonId;
    struct HallofFameMon* currMon = &fameMons->mon[currMonId];

    if (gSprites[gTasks[taskId].tMonSpriteId(currMonId)].data[0] != 0)
    {
        if (currMon->species != SPECIES_EGG)
            PlayCry_Normal(currMon->species, 0);
        HallOfFame_PrintMonInfo(currMon, 0, 14);
        gTasks[taskId].tFrameCount = 120;
        gTasks[taskId].func = Task_Hof_TryDisplayAnotherMon;
    }
}

static void Task_Hof_TryDisplayAnotherMon(u8 taskId)
{
    struct HallofFameTeam* fameMons = eHofMonPtr;
    u16 currPokeID = gTasks[taskId].tDisplayedMonId;
    struct HallofFameMon* currMon = &fameMons->mon[currPokeID];

    if (gTasks[taskId].tFrameCount != 0)
        gTasks[taskId].tFrameCount--;
    else
    {
        sHofFadePalettes |= (0x10000 << gSprites[gTasks[taskId].tMonSpriteId(currPokeID)].oam.paletteNum);
        if (gTasks[taskId].tDisplayedMonId <= 4 && currMon[1].species != 0) // there is another pokemon to display
        {
            gTasks[taskId].tDisplayedMonId++;
            BeginNormalPaletteFade(sHofFadePalettes, 0, 12, 12, RGB(31, 26, 28));
            gSprites[gTasks[taskId].tMonSpriteId(currPokeID)].oam.priority = 1;
            gTasks[taskId].func = Task_Hof_DisplayMon;
        }
        else
            gTasks[taskId].func = Task_Hof_PaletteFadeAndPrintWelcomeText;
    }
}

static void Task_Hof_PaletteFadeAndPrintWelcomeText(u8 taskId)
{
    u16 i;

    BeginNormalPaletteFade(0xFFFF0000, 0, 0, 0, RGB(0, 0, 0));
    for (i = 0; i < 6; i++)
    {
        if (gTasks[taskId].tMonSpriteId(i) != 0xFF)
            gSprites[gTasks[taskId].tMonSpriteId(i)].oam.priority = 0;
    }
    Menu_EraseWindowRect(0, 14, 29, 19);
    HallOfFame_PrintWelcomeText(0, 15);
    PlaySE(SE_APPLAUSE);
    gTasks[taskId].tFrameCount = 400;
    gTasks[taskId].func = Task_Hof_DoConfetti;
}

static void Task_Hof_DoConfetti(u8 taskId)
{
    if (gTasks[taskId].tFrameCount != 0)
    {
        gTasks[taskId].tFrameCount--;
        if ((gTasks[taskId].tFrameCount & 3) == 0 && gTasks[taskId].tFrameCount > 110)
            CreateHofConfettiSprite();
    }
    else
    {
        u16 i;
        for (i = 0; i < 6; i++)
        {
            if (gTasks[taskId].tMonSpriteId(i) != 0xFF)
                gSprites[gTasks[taskId].tMonSpriteId(i)].oam.priority = 1;
        }
        BeginNormalPaletteFade(sHofFadePalettes, 0, 12, 12, RGB(31, 26, 28));
        Menu_EraseWindowRect(0, 14, 29, 19);
        gTasks[taskId].tFrameCount = 7;
        gTasks[taskId].func = Task_Hof_WaitToDisplayPlayer;
    }
}

static void Task_Hof_WaitToDisplayPlayer(u8 taskId)
{
    if (gTasks[taskId].tFrameCount >= 16)
        gTasks[taskId].func = Task_Hof_DisplayPlayer;
    else
    {
        gTasks[taskId].tFrameCount++;
        REG_BLDALPHA = gTasks[taskId].tFrameCount * 256;
    }
}

static void Task_Hof_DisplayPlayer(u8 taskId)
{
    REG_DISPCNT = 0x1940;
    Text_LoadWindowTemplate(&gWindowTemplate_81E71B4);
    InitMenuWindow(&gWindowTemplate_81E71B4);

    gTasks[taskId].tPlayerSpriteID = HallOfFame_LoadTrainerPic(gSaveBlock2.playerGender, 120, 72, 6);
    gTasks[taskId].tFrameCount = 120;
    gTasks[taskId].func = Task_Hof_WaitAndPrintPlayerInfo;
}

static void Task_Hof_WaitAndPrintPlayerInfo(u8 taskId)
{
    if (gTasks[taskId].tFrameCount != 0)
        gTasks[taskId].tFrameCount--;
    else
    {
        if (gSprites[gTasks[taskId].tPlayerSpriteID].x != 160)
            gSprites[gTasks[taskId].tPlayerSpriteID].x++;
        else
        {
            Menu_DrawStdWindowFrame(1, 2, 15, 9);
            HallOfFame_PrintPlayerInfo(1, 2);
            Menu_DrawStdWindowFrame(2, 14, 27, 19);
            Menu_PrintText(gText_LeagueChamp, 4, 15);
            gTasks[taskId].func = Task_Hof_ExitOnKeyPressed;
        }
    }
}

static void Task_Hof_ExitOnKeyPressed(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        FadeOutBGM(4);
        gTasks[taskId].func = Task_Hof_HandlePaletteOnExit;
    }
}

static void Task_Hof_HandlePaletteOnExit(u8 taskId)
{
    CpuSet(gPlttBufferFaded, gPlttBufferUnfaded, 0x200);
    BeginNormalPaletteFade(0xFFFFFFFF, 8, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_Hof_HandleExit;
}

static void Task_Hof_HandleExit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2(CB2_StartCreditsSequence);
    }
}

#undef tDisplayedMonId
#undef tMonNumber
#undef tFrameCount
#undef tPlayerSpriteID
#undef tMonSpriteId

void CB2_DoHallOfFamePC(void)
{
    switch (gMain.state)
    {
    case 0:
    default:
        SetVBlankCallback(NULL);
        ClearVramOamPltt_LoadHofPal();
        gMain.state = 1;
        break;
    case 1:
        LoadHofGfx();
        gMain.state++;
        break;
    case 2:
        {
            u16 savedIme;

            SetVBlankCallback(VBlankCB_HallOfFame);
            savedIme = REG_IME;
            REG_IME = 0;
            REG_IE |= 1;
            REG_IME = savedIme;
            REG_DISPSTAT |= 8;
            gMain.state++;
        }
        break;
    case 3:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        InitHofBgs();

        eHOFPCScreenEffect = sPCScreenEffectTemplate;

        sub_80C5CD4(&eHOFPCScreenEffect);
        gMain.state++;
        break;
    case 4:
        AnimateSprites();
        BuildOamBuffer();
        UpdatePaletteFade();
        if (sub_80C5DCC())
            gMain.state++;
        break;
    case 5:
        REG_BLDCNT = 0x3F42;
        REG_BLDALPHA = 0x710;
        REG_BLDY = 0;
        CreateTask(sub_8142A28, 0);
        SetMainCallback2(CB2_HallOfFame);
        break;
    }
}

#define tCurrTeamNo         data[0]
#define tCurrPageNo         data[1]
#define tCurrMonId          data[2]
#define tMonNo              data[4]
#define tMonSpriteId(i)     data[i + 5]

static void sub_8142A28(u8 taskId)
{
    if (LoadGameSave(SAVE_HALL_OF_FAME) != SAVE_STATUS_OK)
        gTasks[taskId].func = Task_HofPC_PrintDataIsCorrupted;
    else
    {
        u16 *vram1, *vram2;

        u16 i;
        struct HallofFameTeam* savedTeams = (struct HallofFameTeam *)gDecompressionBuffer;
        for (i = 0; i < HALL_OF_FAME_MAX_TEAMS; i++, savedTeams++)
        {
            if (savedTeams->mon[0].species == 0)
                break;
        }
        if (i < HALL_OF_FAME_MAX_TEAMS)
            gTasks[taskId].tCurrTeamNo = i - 1;
        else
            gTasks[taskId].tCurrTeamNo = HALL_OF_FAME_MAX_TEAMS - 1;
        gTasks[taskId].tCurrPageNo = GetGameStat(10);

        for (i = 0, vram1 = (u16*)(VRAM + 0x381A), vram2 = (u16*)(VRAM + 0x385A); i <= 16; i++)
        {
            *(vram1 + i) = i + 3;
            *(vram2 + i) = i + 20;
        }
        Text_LoadWindowTemplate(&gWindowTemplate_81E7198);
        InitMenuWindow(&gWindowTemplate_81E7198);
        gTasks[taskId].func = Task_HofPC_DrawSpritesPrintText;
    }
}

static void Task_HofPC_DrawSpritesPrintText(u8 taskId)
{
    struct HallofFameTeam* savedTeams = (struct HallofFameTeam *)gDecompressionBuffer;
    struct HallofFameMon* currMon;
    u16 i;
    u8* stringPtr;

    for (i = 0; i < gTasks[taskId].tCurrTeamNo; i++)
        savedTeams++;

    currMon = &savedTeams->mon[0];
    sHofFadePalettes = 0;
    gTasks[taskId].tCurrMonId = 0;
    gTasks[taskId].tMonNo = 0;

    for (i = 0; i < 6; i++, currMon++)
    {
        if (currMon->species != 0)
            gTasks[taskId].tMonNo++;
    }

    currMon = &savedTeams->mon[0];

    for (i = 0; i < 6; i++, currMon++)
    {
        if (currMon->species != 0)
        {
            u16 spriteId;
            s16 posX, posY;
            if (gTasks[taskId].tMonNo > 3)
            {
                posX = sHallOfFame_MonFullTeamPositions[i][2];
                posY = sHallOfFame_MonFullTeamPositions[i][3];
            }
            else
            {
                posX = sHallOfFame_MonHalfTeamPositions[i][2];
                posY = sHallOfFame_MonHalfTeamPositions[i][3];
            }
            spriteId = HallOfFame_LoadPokemonPic(currMon->species, posX, posY, i, currMon->tid, currMon->personality);
            gSprites[spriteId].oam.priority = 1;
            gTasks[taskId].tMonSpriteId(i) = spriteId;
        }
        else
            gTasks[taskId].tMonSpriteId(i) = 0xFF;
    }

    BlendPalettes(0xFFFF0000, 12, RGB(31, 26, 28));

    stringPtr = gStringVar1;
    stringPtr = StringCopy(stringPtr, gText_HOFNumber);
    stringPtr[0] = 0xFC;
    stringPtr[1] = 0x14;
    stringPtr[2] = 0x6;
    stringPtr += 3;
    stringPtr = ConvertIntToDecimalStringSimple(stringPtr, gTasks[taskId].tCurrPageNo);
    stringPtr[0] = 0xFC;
    stringPtr[1] = 0x13;
    stringPtr[2] = 0xF0;
    stringPtr[3] = EOS;
    Menu_PrintText(gStringVar1, 0, 0);

    gTasks[taskId].func = Task_HofPC_PrintMonInfo;
}

static void Task_HofPC_PrintMonInfo(u8 taskId)
{
    struct HallofFameTeam* savedTeams = (struct HallofFameTeam *)gDecompressionBuffer;
    struct HallofFameMon* currMon;
    u16 i;
    u16 currMonID;

    for (i = 0; i < gTasks[taskId].tCurrTeamNo; i++)
        savedTeams++;

    for (i = 0; i < 6; i++)
    {
        u16 spriteId = gTasks[taskId].tMonSpriteId(i);
        if (spriteId != 0xFF)
            gSprites[spriteId].oam.priority = 1;
    }

    currMonID = gTasks[taskId].tMonSpriteId(gTasks[taskId].tCurrMonId);
    gSprites[currMonID].oam.priority = 0;
    sHofFadePalettes = (0x10000 << gSprites[currMonID].oam.paletteNum) ^ 0xFFFF0000;
    BlendPalettesUnfaded(sHofFadePalettes, 12, RGB(31, 26, 28));

    currMon = &savedTeams->mon[gTasks[taskId].tCurrMonId];
    if (currMon->species != SPECIES_EGG)
    {
        StopCryAndClearCrySongs();
        PlayCry_Normal(currMon->species, 0);
    }
    HallOfFame_PrintMonInfo(currMon, 0, 14);

    gTasks[taskId].func = Task_HofPC_HandleInput;
}

static void Task_HofPC_HandleInput(u8 taskId)
{
    u16 i;
    if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tCurrTeamNo != 0) // prepare another team to view
        {
            gTasks[taskId].tCurrTeamNo--;
            for (i = 0; i < 6; i++)
            {
                u8 spriteId = gTasks[taskId].tMonSpriteId(i);
                if (spriteId != 0xFF)
                {
                    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(gSprites[spriteId].oam.paletteNum));
                    DestroySprite(&gSprites[spriteId]);
                }
            }
            if (gTasks[taskId].tCurrPageNo != 0)
                gTasks[taskId].tCurrPageNo--;
            gTasks[taskId].func = Task_HofPC_DrawSpritesPrintText;
        }
        else // no more teams to view, turn off hall of fame PC
        {
            if (IsCryPlayingOrClearCrySongs())
            {
                StopCryAndClearCrySongs();
                m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x100);
            }
            gTasks[taskId].func = Task_HofPC_HandlePaletteOnExit;
        }
    }
    else if (JOY_NEW(B_BUTTON)) // turn off hall of fame PC
    {
        if (IsCryPlayingOrClearCrySongs())
        {
            StopCryAndClearCrySongs();
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x100);
        }
        gTasks[taskId].func = Task_HofPC_HandlePaletteOnExit;
    }
    else if (JOY_NEW(DPAD_UP) && gTasks[taskId].tCurrMonId != 0) // change poke -1
    {
        gTasks[taskId].tCurrMonId--;
        gTasks[taskId].func = Task_HofPC_PrintMonInfo;
    }
    else if (JOY_NEW(DPAD_DOWN) && gTasks[taskId].tCurrMonId < gTasks[taskId].tMonNo - 1) // change poke +1
    {
        gTasks[taskId].tCurrMonId++;
        gTasks[taskId].func = Task_HofPC_PrintMonInfo;
    }
}

static void Task_HofPC_HandlePaletteOnExit(u8 taskId)
{
    CpuSet(gPlttBufferFaded, gPlttBufferUnfaded, 0x200);
    eHOFPCScreenEffect = sPCScreenEffectTemplate;
    sub_80C5E38(&eHOFPCScreenEffect);
    gTasks[taskId].func = Task_HofPC_HandleExit;
}

static void Task_HofPC_HandleExit(u8 taskId)
{
    if (sub_80C5F98())
    {
        DestroyTask(taskId);
        ReturnFromHallOfFamePC();
    }
}

static void Task_HofPC_PrintDataIsCorrupted(u8 taskId)
{
    Menu_DrawStdWindowFrame(2, 14, 27, 19);
    MenuPrintMessage(gText_HOFCorrupted, 3, 15);
    gTasks[taskId].func = Task_HofPC_ExitOnButtonPress;
}

static void Task_HofPC_ExitOnButtonPress(u8 taskId)
{
    if (Menu_UpdateWindowText() && JOY_NEW(A_BUTTON))
        gTasks[taskId].func = Task_HofPC_HandlePaletteOnExit;
}

#undef tCurrTeamNo
#undef tCurrPageNo
#undef tCurrMonId
#undef tMonNo
#undef tMonSpriteId

static void HallOfFame_PrintWelcomeText(u8 unusedPossiblyWindowId, u8 a1)
{
    MenuPrint_Centered(gMenuText_WelcomeToHOFAndDexRating, 0, a1 + 1, 0xF0);
}

static void HallOfFame_PrintMonInfo(struct HallofFameMon* currMon, u8 a1, u8 a2)
{
    u8* stringPtr;
    u16 monData;
    u16 i;

    stringPtr = gStringVar1;
    stringPtr[0] = EXT_CTRL_CODE_BEGIN;
    stringPtr[1] = 0x13;
    stringPtr[2] = 0x28;
    stringPtr[3] = EOS;

    if (currMon->species != SPECIES_EGG)
    {
        monData = SpeciesToPokedexNum(currMon->species);
        if (monData != 0xFFFF)
        {
            stringPtr = StringCopy(stringPtr, gText_Number);
            ConvertIntToDecimalStringN(stringPtr, monData, 2, 3);
        }
    }

    Menu_PrintText(gStringVar1, a1 + 4, a2 + 1);
    stringPtr = gStringVar1;

    for (i = 0; i < 10 && currMon->nickname[i] != EOS; stringPtr[i] = currMon->nickname[i], i++) {}
    stringPtr += i;
    stringPtr[0] = EOS;

    if (currMon->species == SPECIES_EGG)
    {
        stringPtr[0] = EXT_CTRL_CODE_BEGIN;
        stringPtr[1] = 0x13;
        stringPtr[2] = 0xA0;
        stringPtr[3] = EOS;
        Menu_PrintText(gStringVar1, a1 + 9, a2 + 1);
        Menu_EraseWindowRect(0, a2 + 3, 29, a2 + 4);
    }
    else
    {

        stringPtr[0] = EXT_CTRL_CODE_BEGIN;
        stringPtr[1] = 0x13;
        stringPtr[2] = 0x3E;
        stringPtr += 3;

        stringPtr[0] = CHAR_SLASH;
        stringPtr++;

        for (i = 0; i < 10 && gSpeciesNames[currMon->species][i] != EOS; stringPtr[i] = gSpeciesNames[currMon->species][i], i++) {}

        stringPtr += i;
        stringPtr[0] = CHAR_SPACE;
        stringPtr++;

        if (currMon->species != SPECIES_NIDORAN_M && currMon->species != SPECIES_NIDORAN_F)
        {
            switch (GetGenderFromSpeciesAndPersonality(currMon->species, currMon->personality))
            {
            case MON_MALE:
                stringPtr[0] = CHAR_MALE;
                stringPtr++;
                break;
            case MON_FEMALE:
                stringPtr[0] = CHAR_FEMALE;
                stringPtr++;
                break;
            }
        }

        stringPtr[0] = EXT_CTRL_CODE_BEGIN;
        stringPtr[1] = 0x13;
        stringPtr[2] = 0xA0;
        stringPtr[3] = EOS;

        Menu_PrintText(gStringVar1, a1 + 9, a2 + 1);

        monData = currMon->lvl;

        stringPtr = StringCopy(gStringVar1, gText_Level);

        stringPtr[0] = EXT_CTRL_CODE_BEGIN;
        stringPtr[1] = 0x14;
        stringPtr[2] = 6;
        stringPtr += 3;

        stringPtr = ConvertIntToDecimalStringN(stringPtr, monData, 0, 3);

        stringPtr[0] = EXT_CTRL_CODE_BEGIN;
        stringPtr[1] = 0x13;
        stringPtr[2] = 0x30;
        stringPtr[3] = EOS;

        Menu_PrintText(gStringVar1, a1 + 7, a2 + 3);

        monData = currMon->tid;

        stringPtr = StringCopy(gStringVar1, gText_IdNumberSlash);
        ConvertIntToDecimalStringN(stringPtr, monData, 2, 5);

        Menu_PrintText(gStringVar1, a1 + 13, a2 + 3);
    }
}

#define ByteRead16(ptr) ((ptr)[0] | ((ptr)[1] << 8))

static void HallOfFame_PrintPlayerInfo(u8 a0, u8 a1)
{
    u8* stringPtr;
    u16 visibleTid;

    Menu_PrintText(gText_Name, a0 + 1, a1 + 1);
    MenuPrint_RightAligned(gSaveBlock2.playerName, a0 + 14, a1 + 1);

    Menu_PrintText(gText_IDNumber, a0 + 1, a1 + 3);
    visibleTid = ByteRead16(gSaveBlock2.playerTrainerId);
    ConvertIntToDecimalStringN(gStringVar1, visibleTid, 2, 5);

    MenuPrint_RightAligned(gStringVar1, a0 + 14, a1 + 3);
    Menu_PrintText(gText_Time, a0 + 1, a1 + 5);

    stringPtr = ConvertIntToDecimalStringSimple(gStringVar1, gSaveBlock2.playTimeHours);
    stringPtr[0] = CHAR_SPACE;
    stringPtr[1] = CHAR_COLON;
    stringPtr[2] = CHAR_SPACE;
    stringPtr += 3;

    stringPtr = ConvertIntToDecimalStringN(stringPtr, gSaveBlock2.playTimeMinutes, 2, 2);
    stringPtr[0] = EOS;

    MenuPrint_RightAligned(gStringVar1, a0 + 14, a1 + 5);
}

static void ClearVramOamPltt_LoadHofPal(void)
{
    u16 i;

    REG_DISPCNT = 0;

    REG_BG0CNT = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    REG_BG1CNT = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;

    REG_BG2CNT = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;

    REG_BG3CNT = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;

    DmaFill16Large(3, 0, VRAM, 0x18000, 0x1000);
    DmaFill32Defvars(3, 0, OAM, OAM_SIZE);
    DmaFill16Defvars(3, 0, PLTT, PLTT_SIZE);

    LZ77UnCompVram(gHallOfFame_Gfx, (void*)(VRAM));

    for (i = 0; i < 64; i++)
    {
        *((u16*)(VRAM + 0x3800) + i) = 1;
    }
    for (i = 0; i < 192; i++)
    {
        *((u16*)(VRAM + 0x3B80) + i) = 1;
    }
    for (i = 0; i < 1024; i++)
    {
        *((u16*)(VRAM + 0x3000) + i) = 2;
    }

    DmaFill16Large(3, 0, gSharedMem, 0x4000, 0x1000);
    ResetPaletteFade();
    LoadPalette(gHallOfFame_Pal, 0, 0x20);
}

static void LoadHofGfx(void)
{
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 8;
    LoadCompressedSpriteSheet(&sSpriteSheet_Confetti);
    LoadCompressedSpritePalette(&sSpritePalette_Confetti);
    Text_LoadWindowTemplate(&gWindowTemplate_81E71B4);
    InitMenuWindow(&gWindowTemplate_81E71B4);
}

static void InitHofBgs(void)
{
    REG_BG1CNT = 0x700;
    REG_BG3CNT = 0x603;
    REG_DISPCNT = 0x1B40;
}

static void sub_81435DC(struct Sprite* sprite)
{
    if (sprite->x != sprite->data[1] || sprite->y != sprite->data[2])
    {
        if (sprite->x < sprite->data[1])
            sprite->x += 15;
        if (sprite->x > sprite->data[1])
            sprite->x -= 15;

        if (sprite->y < sprite->data[2])
            sprite->y += 10;
        if (sprite->y > sprite->data[2])
            sprite->y -= 10;
    }
    else
    {
        sprite->data[0] = 1;
        sprite->callback = SpriteCB_HallOfFame_Dummy;
    }
}

static void SpriteCB_HallOfFame_Dummy(struct Sprite* sprite)
{

}

void sub_8143648(u16 paletteTag, u8 animID)
{
    gMultiuseSpriteTemplate = sUnknown_0840B6B8;
    gMultiuseSpriteTemplate.paletteTag = paletteTag;
    gMultiuseSpriteTemplate.images = sUnknown_0840B69C[animID];
    gMultiuseSpriteTemplate.anims = gAnims_MonPic;
}

void sub_8143680(u16 paletteTag, u8 animID)
{
    gMultiuseSpriteTemplate = sUnknown_0840B6B8;
    gMultiuseSpriteTemplate.paletteTag = paletteTag;
    gMultiuseSpriteTemplate.images = sUnknown_0840B69C[animID];
    gMultiuseSpriteTemplate.anims = gTrainerFrontAnimsPtrTable[0];
}

static u32 HallOfFame_LoadPokemonPic(u16 species, s16 posX, s16 posY, u16 pokeID, u32 otId, u32 personality)
{
    u8 spriteId;
    const u8* pokePal;

    LoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, (void *)EWRAM, gUnknown_0840B5A0[pokeID], species, personality, 1);

    pokePal = GetMonSpritePalFromSpeciesAndPersonality(species, otId, personality);
    LoadCompressedPalette(pokePal, 16 * pokeID + 256, 0x20);

    sub_8143648(pokeID, pokeID);
    spriteId = CreateSprite(&gMultiuseSpriteTemplate, posX, posY, 10 - pokeID);
    gSprites[spriteId].oam.paletteNum = pokeID;
    return spriteId;
}

static u32 HallOfFame_LoadTrainerPic(u16 trainerPicId, s16 posX, s16 posY, u16 a3)
{
    u8 spriteId;

    DecompressPicFromTable_2(&gTrainerFrontPicTable[trainerPicId], gTrainerFrontPicCoords[trainerPicId].size, gTrainerFrontPicCoords[trainerPicId].y_offset, (void*)EWRAM, gUnknown_0840B5A0[a3], trainerPicId);

    LoadCompressedPalette(gTrainerFrontPicPaletteTable[trainerPicId].data, 16 * a3 + 256, 0x20);
    sub_8143680(a3, a3);

    spriteId = CreateSprite(&gMultiuseSpriteTemplate, posX, posY, 1);
    gSprites[spriteId].oam.paletteNum = a3;

    return spriteId;
}

static void SpriteCB_HofConfetti(struct Sprite* sprite)
{
    if (sprite->y2 > 120)
        DestroySprite(sprite);
    else
    {
        u16 rand;
        u8 sineIdx;

        sprite->y2++;
        sprite->y2 += sprite->data[1];

        sineIdx = sprite->data[0];
        rand = (Random() % 4) + 8;
        sprite->x2 = rand * gSineTable[sineIdx] / 256;

        sprite->data[0] += 4;
    }
}

static bool8 CreateHofConfettiSprite(void)
{
    u8 spriteID;
    struct Sprite* sprite;

    s16 posX = Random() % 240;
    s16 posY = -(Random() % 8);

    spriteID = CreateSprite(&sSpriteTemplate_HofConfetti, posX, posY, 0);
    sprite = &gSprites[spriteID];

    StartSpriteAnim(sprite, Random() % 17);

    if (Random() & 3)
        sprite->data[1] = 0;
    else
        sprite->data[1] = 1;

    return 0;
}
