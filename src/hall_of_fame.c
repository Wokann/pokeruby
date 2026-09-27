#include "global.h"
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
#include "data2.h"
#include "decompress.h"
#include "random.h"
#include "scanline_effect.h"
#include "trig.h"
#include "hof_pc.h"
#include "credits.h"
#include "pc_screen_effect.h"
#include "ewram.h"

static EWRAM_DATA u32 sHofFadePalettes = 0;

extern bool8 gHasHallOfFameRecords; // has hall of fame records
extern void (*gGameContinueCallback)(void);
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u8 gReservedSpritePaletteCount;
extern struct SpriteTemplate gCreatingSpriteTemplate;

extern const u8 gContestConfetti_Gfx[];
extern const u8 gContestConfetti_Pal[];
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
static void Task_HofPC_CopySaveData(u8 taskId);
static void Task_HofPC_PrintDataIsCorrupted(u8 taskId);
static void Task_HofPC_DrawSpritesPrintText(u8 taskId);
static void Task_HofPC_PrintMonInfo(u8 taskId);
static void Task_HofPC_HandleInput(u8 taskId);
static void Task_HofPC_HandlePaletteOnExit(u8 taskId);
static void Task_HofPC_HandleExit(u8 taskId);
static void Task_HofPC_ExitOnButtonPress(u8 taskId);

static void SpriteCB_GetOnScreenAndAnimate(struct Sprite* sprite);
static void SpriteCB_HofConfetti(struct Sprite* sprite);
static void SpriteCB_HallOfFame_Dummy(struct Sprite* sprite);

static void HallOfFame_PrintWelcomeText(u8 a0, u8 a1);
static void HallOfFame_PrintMonInfo(struct HallofFameMon* currMon, u8 a1, u8 a2);
static void HallOfFame_PrintPlayerInfo(u8 a0, u8 a1);
static void ClearVramOamPltt_LoadHofGfx(void);
static void LoadHofGfx(void);
static void SetHofBgDisplayRegs(void);
static u32 HallOfFame_LoadPokemonPic(u16 species, s16 posX, s16 posY, u16 pokeID, u32 tid, u32 pid);
static u32 HallOfFame_LoadTrainerPic(u16 trainerPicID, s16 posX, s16 posY, u16 a3);
static bool8 CreateHofConfettiSprite(void);

// data and gfx

static const struct CompressedSpriteSheet sHallOfFame_ConfettiSpriteSheet =
{
      gContestConfetti_Gfx, 0x220, 1001
};

static const u8 sUnused0[8] = {};

static const struct CompressedSpritePalette sHallOfFame_ConfettiSpritePalette =
{
      gContestConfetti_Pal, 1001
};

static const u8 sUnused1[8] = {};

static const s16 sHallOfFame_MonsFullTeamPositions[6][4] =
{
    {120,   210,    120,    40},
    {326,   220,    56,     40},
    {-86,   220,    184,    40},
    {120,   -62,    120,    88},
    {-25,   -62,    200,    88},
    {265,   -62,    40,     88}
};

static const s16 sHallOfFame_MonsHalfTeamPositions[3][4] =
{
    {120,   214,    120,    64},
    {281,   214,    56,     64},
    {-41,   214,    184,    64}
};

static const struct PCScreenEffectStruct sPCScreenEffectTemplate = {
    .tileTag = 0x3ea,
    .paletteTag = 0x3ea
};

static const u8 sUnused2[6] = {2, 1, 3, 6, 4, 5};

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

static const struct OamData sOamData_840B6D0 =
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

static const union AnimCmd sSpriteAnim_840B6D8[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B6E0[] =
{
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B6E8[] =
{
    ANIMCMD_FRAME(2, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B6F0[] =
{
    ANIMCMD_FRAME(3, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B6F8[] =
{
    ANIMCMD_FRAME(4, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B700[] =
{
    ANIMCMD_FRAME(5, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B708[] =
{
    ANIMCMD_FRAME(6, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B710[] =
{
    ANIMCMD_FRAME(7, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B718[] =
{
    ANIMCMD_FRAME(8, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B720[] =
{
    ANIMCMD_FRAME(9, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B728[] =
{
    ANIMCMD_FRAME(10, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B730[] =
{
    ANIMCMD_FRAME(11, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B738[] =
{
    ANIMCMD_FRAME(12, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B740[] =
{
    ANIMCMD_FRAME(13, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B748[] =
{
    ANIMCMD_FRAME(14, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B750[] =
{
    ANIMCMD_FRAME(15, 30),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_840B758[] =
{
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END
};

static const union AnimCmd* const sSpriteAnimTable_840B760[] =
{
    sSpriteAnim_840B6D8,
    sSpriteAnim_840B6E0,
    sSpriteAnim_840B6E8,
    sSpriteAnim_840B6F0,
    sSpriteAnim_840B6F8,
    sSpriteAnim_840B700,
    sSpriteAnim_840B708,
    sSpriteAnim_840B710,
    sSpriteAnim_840B718,
    sSpriteAnim_840B720,
    sSpriteAnim_840B728,
    sSpriteAnim_840B730,
    sSpriteAnim_840B738,
    sSpriteAnim_840B740,
    sSpriteAnim_840B748,
    sSpriteAnim_840B750,
    sSpriteAnim_840B758
};

static const struct SpriteTemplate sSpriteTemplate_840B7A4 =
{
    .tileTag = 1001,
    .paletteTag = 1001,
    .oam = &sOamData_840B6D0,
    .anims = sSpriteAnimTable_840B760,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_HofConfetti
};

// code

#define tDisplayedMonId  data[1]
#define tMonNumber       data[2]
#define tFrameCount      data[3]
#define tPlayerSpriteID  data[4]
#define tMonSpriteId(i)  data[i + 5]

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
        ClearVramOamPltt_LoadHofGfx();
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
        SetHofBgDisplayRegs();
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

static void CB2_DoHallOfFameScreenDontSaveData(void)
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
    struct HallofFameTeam* fameTeam = eHofMonPtr;

    gTasks[taskId].tMonNumber = 0; // valid pokes
    for (i = 0; i < 6; i++)
    {
        u8 nickname[12];
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
        {
            fameTeam->mon[i].species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2);
            fameTeam->mon[i].tid = GetMonData(&gPlayerParty[i], MON_DATA_OT_ID);
            fameTeam->mon[i].personality = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY);
            fameTeam->mon[i].lvl = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
            GetMonData(&gPlayerParty[i], MON_DATA_NICKNAME, nickname);
            for (j = 0; j < 10; j++)
            {
                fameTeam->mon[i].nickname[j] = nickname[j];
            }
            gTasks[taskId].tMonNumber++;
        }
        else
        {
            fameTeam->mon[i].species = 0;
            fameTeam->mon[i].tid = 0;
            fameTeam->mon[i].personality = 0;
            fameTeam->mon[i].lvl = 0;
            fameTeam->mon[i].nickname[0] = EOS;
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
    struct HallofFameTeam* fameTeam = eHofMonPtr;
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
    *lastSavedTeam = *fameTeam;
    Menu_DrawStdWindowFrame(2, 14, 27, 19);
    Menu_PrintText(gMenuText_HOFSaving, 3, 15);
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
    u8 spriteID;
    s16 xPos, yPos, field4, field6;

    struct HallofFameTeam* fameTeam = eHofMonPtr;
    u16 currMonId = gTasks[taskId].tDisplayedMonId;
    struct HallofFameMon* currMon = &fameTeam->mon[currMonId];

    if (gTasks[taskId].tMonNumber > 3)
    {
        xPos = sHallOfFame_MonsFullTeamPositions[currMonId][0];
        yPos = sHallOfFame_MonsFullTeamPositions[currMonId][1];
        field4 = sHallOfFame_MonsFullTeamPositions[currMonId][2];
        field6 = sHallOfFame_MonsFullTeamPositions[currMonId][3];
    }
    else
    {
        xPos = sHallOfFame_MonsHalfTeamPositions[currMonId][0];
        yPos = sHallOfFame_MonsHalfTeamPositions[currMonId][1];
        field4 = sHallOfFame_MonsHalfTeamPositions[currMonId][2];
        field6 = sHallOfFame_MonsHalfTeamPositions[currMonId][3];
    }

    spriteID = HallOfFame_LoadPokemonPic(currMon->species, xPos, yPos, currMonId, currMon->tid, currMon->personality);
    gSprites[spriteID].data[1] = field4;
    gSprites[spriteID].data[2] = field6;
    gSprites[spriteID].data[0] = 0;
    gSprites[spriteID].callback = SpriteCB_GetOnScreenAndAnimate;
    gTasks[taskId].tMonSpriteId(currMonId) = spriteID;
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = Task_Hof_PrintMonInfoAfterAnimating;
}

static void Task_Hof_PrintMonInfoAfterAnimating(u8 taskId)
{
    struct HallofFameTeam* fameTeam = eHofMonPtr;
    u16 currMonId = gTasks[taskId].tDisplayedMonId;
    struct HallofFameMon* currMon = &fameTeam->mon[currMonId];

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
    struct HallofFameTeam* fameTeam = eHofMonPtr;
    u16 currMonId = gTasks[taskId].tDisplayedMonId;
    struct HallofFameMon* currMon = &fameTeam->mon[currMonId];

    if (gTasks[taskId].tFrameCount != 0)
        gTasks[taskId].tFrameCount--;
    else
    {
        sHofFadePalettes |= (0x10000 << gSprites[gTasks[taskId].tMonSpriteId(currMonId)].oam.paletteNum);
        if (gTasks[taskId].tDisplayedMonId <= 4 && currMon[1].species != 0) // there is another pokemon to display
        {
            gTasks[taskId].tDisplayedMonId++;
            BeginNormalPaletteFade(sHofFadePalettes, 0, 12, 12, RGB(31, 26, 28));
            gSprites[gTasks[taskId].tMonSpriteId(currMonId)].oam.priority = 1;
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
            Menu_PrintText(gMenuText_HOFCongratulations, 4, 15);
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
        ClearVramOamPltt_LoadHofGfx();
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
        SetHofBgDisplayRegs();

        eHOFPCScreenEffect = sPCScreenEffectTemplate;

        StartPCScreenOpenEffect(&eHOFPCScreenEffect);
        gMain.state++;
        break;
    case 4:
        AnimateSprites();
        BuildOamBuffer();
        UpdatePaletteFade();
        if (UpdatePCScreenOpenEffect())
            gMain.state++;
        break;
    case 5:
        REG_BLDCNT = 0x3F42;
        REG_BLDALPHA = 0x710;
        REG_BLDY = 0;
        CreateTask(Task_HofPC_CopySaveData, 0);
        SetMainCallback2(CB2_HallOfFame);
        break;
    }
}

#define tCurrTeamNo     data[0]
#define tCurrPageNo     data[1]
#define tCurrMonId     data[2]
#define tMonNo        data[4]
#define tMonSpriteId(i) data[i + 5]

static void Task_HofPC_CopySaveData(u8 taskId)
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
            u16 spriteID;
            s16 posX, posY;
            if (gTasks[taskId].tMonNo > 3)
            {
                posX = sHallOfFame_MonsFullTeamPositions[i][2];
                posY = sHallOfFame_MonsFullTeamPositions[i][3];
            }
            else
            {
                posX = sHallOfFame_MonsHalfTeamPositions[i][2];
                posY = sHallOfFame_MonsHalfTeamPositions[i][3];
            }
            spriteID = HallOfFame_LoadPokemonPic(currMon->species, posX, posY, i, currMon->tid, currMon->personality);
            gSprites[spriteID].oam.priority = 1;
            gTasks[taskId].tMonSpriteId(i) = spriteID;
        }
        else
            gTasks[taskId].tMonSpriteId(i) = 0xFF;
    }

    BlendPalettes(0xFFFF0000, 12, RGB(31, 26, 28));

    stringPtr = gStringVar1;
    stringPtr = StringCopy(stringPtr, gMenuText_HOFNumber);
    stringPtr[0] = 0xFC;
    stringPtr[1] = 0x14;
    stringPtr[2] = 0x6;
    stringPtr += 3;
    stringPtr = ConvertIntToDecimalString(stringPtr, gTasks[taskId].tCurrPageNo);
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
        u16 spriteID = gTasks[taskId].tMonSpriteId(i);
        if (spriteID != 0xFF)
            gSprites[spriteID].oam.priority = 1;
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
                u8 spriteID = gTasks[taskId].tMonSpriteId(i);
                if (spriteID != 0xFF)
                {
                    FreeSpritePaletteByTag(GetSpritePaletteTagByPaletteNum(gSprites[spriteID].oam.paletteNum));
                    DestroySprite(&gSprites[spriteID]);
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
    StartPCScreenCloseEffect(&eHOFPCScreenEffect);
    gTasks[taskId].func = Task_HofPC_HandleExit;
}

static void Task_HofPC_HandleExit(u8 taskId)
{
    if (UpdatePCScreenCloseEffect())
    {
        DestroyTask(taskId);
        ReturnFromHallOfFamePC();
    }
}

static void Task_HofPC_PrintDataIsCorrupted(u8 taskId)
{
    Menu_DrawStdWindowFrame(2, 14, 27, 19);
    MenuPrintMessage(gMenuText_HOFCorrupt, 3, 15);
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

static void HallOfFame_PrintWelcomeText(u8 a0, u8 a1)
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
            stringPtr = StringCopy(stringPtr, gOtherText_Number2);
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

        stringPtr = StringCopy(gStringVar1, gOtherText_Level3);

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

        stringPtr = StringCopy(gStringVar1, gOtherText_IDNumber);
        ConvertIntToDecimalStringN(stringPtr, monData, 2, 5);

        Menu_PrintText(gStringVar1, a1 + 13, a2 + 3);
    }
}

#define ByteRead16(ptr) ((ptr)[0] | ((ptr)[1] << 8))

static void HallOfFame_PrintPlayerInfo(u8 a0, u8 a1)
{
    u8* stringPtr;
    u16 visibleTid;

    Menu_PrintText(gOtherText_Name, a0 + 1, a1 + 1);
    MenuPrint_RightAligned(gSaveBlock2.playerName, a0 + 14, a1 + 1);

    Menu_PrintText(gOtherText_IDNumber2, a0 + 1, a1 + 3);
    visibleTid = ByteRead16(gSaveBlock2.playerTrainerId);
    ConvertIntToDecimalStringN(gStringVar1, visibleTid, 2, 5);

    MenuPrint_RightAligned(gStringVar1, a0 + 14, a1 + 3);
    Menu_PrintText(gMainMenuString_Time, a0 + 1, a1 + 5);

    stringPtr = ConvertIntToDecimalString(gStringVar1, gSaveBlock2.playTimeHours);
    stringPtr[0] = CHAR_SPACE;
    stringPtr[1] = CHAR_COLON;
    stringPtr[2] = CHAR_SPACE;
    stringPtr += 3;

    stringPtr = ConvertIntToDecimalStringN(stringPtr, gSaveBlock2.playTimeMinutes, 2, 2);
    stringPtr[0] = EOS;

    MenuPrint_RightAligned(gStringVar1, a0 + 14, a1 + 5);
}

static void ClearVramOamPltt_LoadHofGfx(void)
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
    LoadCompressedObjectPic(&sHallOfFame_ConfettiSpriteSheet);
    LoadCompressedObjectPalette(&sHallOfFame_ConfettiSpritePalette);
    Text_LoadWindowTemplate(&gWindowTemplate_81E71B4);
    InitMenuWindow(&gWindowTemplate_81E71B4);
}

static void SetHofBgDisplayRegs(void)
{
    REG_BG1CNT = 0x700;
    REG_BG3CNT = 0x603;
    REG_DISPCNT = 0x1B40;
}

static void SpriteCB_GetOnScreenAndAnimate(struct Sprite* sprite)
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

void PrepareHallOfFameMonPicSpriteTemplate(u16 paletteTag, u8 animID)
{
    gCreatingSpriteTemplate = sUnknown_0840B6B8;
    gCreatingSpriteTemplate.paletteTag = paletteTag;
    gCreatingSpriteTemplate.images = sUnknown_0840B69C[animID];
    gCreatingSpriteTemplate.anims = gSpriteAnimTable_81E7C64;
}

void PrepareHallOfFameTrainerPicSpriteTemplate(u16 paletteTag, u8 animID)
{
    gCreatingSpriteTemplate = sUnknown_0840B6B8;
    gCreatingSpriteTemplate.paletteTag = paletteTag;
    gCreatingSpriteTemplate.images = sUnknown_0840B69C[animID];
    gCreatingSpriteTemplate.anims = gUnknown_081EC2A4[0];
}

static u32 HallOfFame_LoadPokemonPic(u16 species, s16 posX, s16 posY, u16 pokeID, u32 tid, u32 pid)
{
    u8 spriteID;
    const u8* pokePal;

    LoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].coords, gMonFrontPicCoords[species].y_offset, (void *)EWRAM, gUnknown_0840B5A0[pokeID], species, pid, 1);

    pokePal = GetMonSpritePalFromOtIdPersonality(species, tid, pid);
    LoadCompressedPalette(pokePal, 16 * pokeID + 256, 0x20);

    PrepareHallOfFameMonPicSpriteTemplate(pokeID, pokeID);
    spriteID = CreateSprite(&gCreatingSpriteTemplate, posX, posY, 10 - pokeID);
    gSprites[spriteID].oam.paletteNum = pokeID;
    return spriteID;
}

static u32 HallOfFame_LoadTrainerPic(u16 trainerPicID, s16 posX, s16 posY, u16 a3)
{
    u8 spriteID;

    DecompressPicFromTable_2(&gTrainerFrontPicTable[trainerPicID], gTrainerFrontPicCoords[trainerPicID].coords, gTrainerFrontPicCoords[trainerPicID].y_offset, (void*)EWRAM, gUnknown_0840B5A0[a3], trainerPicID);

    LoadCompressedPalette(gTrainerFrontPicPaletteTable[trainerPicID].data, 16 * a3 + 256, 0x20);
    PrepareHallOfFameTrainerPicSpriteTemplate(a3, a3);

    spriteID = CreateSprite(&gCreatingSpriteTemplate, posX, posY, 1);
    gSprites[spriteID].oam.paletteNum = a3;

    return spriteID;
}

static void SpriteCB_HofConfetti(struct Sprite* sprite)
{
    if (sprite->y2 > 120)
        DestroySprite(sprite);
    else
    {
        u16 rand;
        u8 tableID;

        sprite->y2++;
        sprite->y2 += sprite->data[1];

        tableID = sprite->data[0];
        rand = (Random() % 4) + 8;
        sprite->x2 = rand * gSineTable[tableID] / 256;

        sprite->data[0] += 4;
    }
}

static bool8 CreateHofConfettiSprite(void)
{
    u8 spriteID;
    struct Sprite* sprite;

    s16 posX = Random() % 240;
    s16 posY = -(Random() % 8);

    spriteID = CreateSprite(&sSpriteTemplate_840B7A4, posX, posY, 0);
    sprite = &gSprites[spriteID];

    StartSpriteAnim(sprite, Random() % 17);

    if (Random() & 3)
        sprite->data[1] = 0;
    else
        sprite->data[1] = 1;

    return 0;
}
