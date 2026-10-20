#include "global.h"
#include "constants/rgb.h"
#include "pokedex.h"
#include "field_screen_effect.h"
#include "graphics.h"
#include "data.h"
#include "daycare.h"
#include "decompress.h"
#include "event_data.h"
#include "ewram.h"
#include "constants/items.h"
#include "main.h"
#include "menu.h"
#include "international_string_util.h"
#include "naming_screen.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "egg_hatch.h"
#include "random.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "trig.h"
#include "trade.h"

#define GFXTAG_EGG       12345
#define GFXTAG_EGG_SHARD 23456

#define PALTAG_EGG       54321

#define EGG_X (DISPLAY_WIDTH / 2)
#define EGG_Y (DISPLAY_HEIGHT / 2 - 5)

struct EggHatchData
{
    u8 eggSpriteId;
    u8 monSpriteId;
    u8 state;
    u8 delayTimer;
    u8 eggPartyId;
    struct Window window;
    u8 tileDataStartOffset;
    u8 unused_39;
    u8 eggShardVelocityId;
};

struct EggHatchData* gEggHatchData;

extern const u8 gText_HatchedFromEgg[];
extern const u8 gText_NicknameHatchPrompt[];

static void Task_EggHatch(u8 taskId);
static void CB2_LoadEggHatch(void);
static void CB2_EggHatch(void);
static void SpriteCB_Egg_Shake1(struct Sprite* sprite);
static void SpriteCB_Egg_Shake2(struct Sprite* sprite);
static void SpriteCB_Egg_Shake3(struct Sprite* sprite);
static void SpriteCB_Egg_WaitHatch(struct Sprite* sprite);
static void SpriteCB_Egg_Hatch(struct Sprite* sprite);
static void SpriteCB_Egg_Reveal(struct Sprite* sprite);
static void SpriteCB_EggShard(struct Sprite* sprite);
static void EggHatchPrintMessage2(u8* string);
static void EggHatchPrintMessage1(u8* string);
static bool8 EggHatchUpdateWindowText(void);
static void CreateRandomEggShardSprite(void);
static void CreateEggShardSprite(u8 x, u8 y, s16 velocityX, s16 velocityY, s16 acceleration, u8 spriteAnimIndex);

// graphics

static const u16 sEggPalette[] = INCBIN_U16("graphics/pokemon/egg/normal.gbapal");
static const u8 sEggHatchTiles[] = INCBIN_U8("graphics/pokemon/egg/hatch.4bpp");
static const u8 sEggShardTiles[] = INCBIN_U8("graphics/pokemon/egg/shard.4bpp");

static const struct OamData sOamData_Egg =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_Egg_Normal[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Egg_Cracked1[] =
{
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Egg_Cracked2[] =
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Egg_Cracked3[] =
{
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END
};

enum {
    EGG_ANIM_NORMAL,
    EGG_ANIM_CRACKED_1,
    EGG_ANIM_CRACKED_2,
    EGG_ANIM_CRACKED_3,
};

static const union AnimCmd *const sSpriteAnimTable_Egg[] =
{
    [EGG_ANIM_NORMAL]    = sSpriteAnim_Egg_Normal,
    [EGG_ANIM_CRACKED_1] = sSpriteAnim_Egg_Cracked1,
    [EGG_ANIM_CRACKED_2] = sSpriteAnim_Egg_Cracked2,
    [EGG_ANIM_CRACKED_3] = sSpriteAnim_Egg_Cracked3,
};

static const struct SpriteSheet sEggHatch_Sheet =
{
    .data = sEggHatchTiles,
    .size = sizeof(sEggHatchTiles),
    .tag = GFXTAG_EGG,
};

static const struct SpriteSheet sEggShards_Sheet =
{
    .data = sEggShardTiles,
    .size = sizeof(sEggShardTiles),
    .tag = GFXTAG_EGG_SHARD,
};

static const struct SpritePalette sEgg_SpritePalette =
{
    .data = sEggPalette,
    .tag = PALTAG_EGG
};

static const struct SpriteTemplate sSpriteTemplate_Egg =
{
    .tileTag = GFXTAG_EGG,
    .paletteTag = PALTAG_EGG,
    .oam = &sOamData_Egg,
    .anims = sSpriteAnimTable_Egg,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};


static const struct OamData sOamData_EggShard =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_EggShard0[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_EggShard1[] =
{
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_EggShard2[] =
{
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_EggShard3[] =
{
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_EggShard[] =
{
    sSpriteAnim_EggShard0,
    sSpriteAnim_EggShard1,
    sSpriteAnim_EggShard2,
    sSpriteAnim_EggShard3,
};

static const struct SpriteTemplate sSpriteTemplate_EggShard =
{
    .tileTag = GFXTAG_EGG_SHARD,
    .paletteTag = PALTAG_EGG,
    .oam = &sOamData_EggShard,
    .anims = sSpriteAnimTable_EggShard,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_EggShard
};

// actual code

static void CreateHatchedMon(struct Pokemon *egg, struct Pokemon *temp)
{
    u16 species;
    u32 personality, pokerus;
    u8 i, friendship, language, gameMet, markings;
    u16 moves[MAX_MON_MOVES];
    u32 ivs[NUM_STATS];


    species = GetMonData(egg, MON_DATA_SPECIES);

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        moves[i] = GetMonData(egg, MON_DATA_MOVE1 + i);
    }

    personality = GetMonData(egg, MON_DATA_PERSONALITY);

    for (i = 0; i < NUM_STATS; i++)
    {
        ivs[i] = GetMonData(egg, MON_DATA_HP_IV + i);
    }

    gameMet = GetMonData(egg, MON_DATA_MET_GAME);
    markings = GetMonData(egg, MON_DATA_MARKINGS);
    pokerus = GetMonData(egg, MON_DATA_POKERUS);
    //FRLGE also copy the eventLegal field to the hatched Pokemon. PCNY Wish Eggs traded to RS and hatched will not have their eventLegal bit set and will not be Fateful Encounters in future games.

    CreateMon(temp, species, EGG_HATCH_LEVEL, USE_RANDOM_IVS, TRUE, personality, OT_ID_PLAYER_ID, 0);

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        SetMonData(temp, MON_DATA_MOVE1 + i, (const u8 *) &moves[i]);
    }

    for (i = 0; i < NUM_STATS; i++)
    {
        SetMonData(temp, MON_DATA_HP_IV + i, (const u8 *) &ivs[i]);
    }

    language = GAME_LANGUAGE;
    SetMonData(temp, MON_DATA_LANGUAGE, &language);
    SetMonData(temp, MON_DATA_MET_GAME, &gameMet);
    SetMonData(temp, MON_DATA_MARKINGS, &markings);

    friendship = 120;
    SetMonData(temp, MON_DATA_FRIENDSHIP, &friendship);
    SetMonData(temp, MON_DATA_POKERUS, (const u8 *) &pokerus);

    *egg = *temp;
}

static void AddHatchedMonToParty(u8 id)
{
    u8 isEgg;
    u16 species;
    u8 name[POKEMON_NAME_LENGTH + 2];
    u16 ball;
    u16 metLevel;
    metloc_u8_t metLocation;
    struct Pokemon* mon = &gPlayerParty[id];

    CreateHatchedMon(mon, &gEnemyParty[0]);
    isEgg = FALSE;
    SetMonData(mon, MON_DATA_IS_EGG, &isEgg);

    species = GetMonData(mon, MON_DATA_SPECIES);
    GetSpeciesName(name, species);
    SetMonData(mon, MON_DATA_NICKNAME, name);

    species = SpeciesToNationalPokedexNum(species);
    GetSetPokedexFlag(species, FLAG_SET_SEEN);
    GetSetPokedexFlag(species, FLAG_SET_CAUGHT);

    GetMonNickname2(mon, gStringVar1);

    ball = ITEM_POKE_BALL;
    SetMonData(mon, MON_DATA_POKEBALL, (const u8*) &ball);

    metLevel = 0;
    SetMonData(mon, MON_DATA_MET_LEVEL, (const u8*)  &metLevel);

    metLocation = GetCurrentRegionMapSectionId();
    SetMonData(mon, MON_DATA_MET_LOCATION, &metLocation);

    MonRestorePP(mon);
    CalculateMonStats(mon);
}

void ScriptHatchMon(void)
{
    AddHatchedMonToParty(gSpecialVar_0x8004);
}

static bool8 _CheckDaycareMonReceivedMail(struct DayCare *daycare, u8 daycareId)
{
    u8 monNickname[max(32, POKEMON_NAME_BUFFER_SIZE)];
    GetBoxMonNickname(&daycare->mons[daycareId], monNickname);
    if (daycare->misc.mail[daycareId].message.itemId != ITEM_NONE)
    {
        if (StringCompareWithoutExtCtrlCodes(monNickname, daycare->misc.mail[daycareId].names + PLAYER_NAME_LENGTH + 1)
         || StringCompareWithoutExtCtrlCodes(gSaveBlock2.playerName, daycare->misc.mail[daycareId].names))
        {
            StringCopy(gStringVar1, monNickname);
            StringCopy(gStringVar2, daycare->misc.mail[daycareId].names);
            StringCopy(gStringVar3, daycare->misc.mail[daycareId].names + PLAYER_NAME_LENGTH + 1);
            ConvertInternationalPlayerName(gStringVar2);
            return TRUE;
        }
    }

    return FALSE;
}

bool8 CheckDaycareMonReceivedMail(void)
{
    return _CheckDaycareMonReceivedMail(&gSaveBlock1.daycare, gSpecialVar_0x8004);
}

static u8 EggHatchCreateMonSprite(u8 useAlt, u8 state, u8 partyId)
{
    u8 position = 0;
    u8 spriteId = 0;
    struct Pokemon* mon = NULL;

    if (useAlt == FALSE)
    {
        mon = &gPlayerParty[partyId];
        position = B_POSITION_OPPONENT_LEFT;
    }
    if (useAlt == TRUE)
    {
        mon = &gPlayerParty[partyId];
        position = B_POSITION_OPPONENT_RIGHT;
    }
    switch (state)
    {
    case 0:
        {
            u16 species = GetMonData(mon, MON_DATA_SPECIES);
            u32 pid = GetMonData(mon, MON_DATA_PERSONALITY);
            HandleLoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, gSharedMem, gMonSpriteGfx_Sprite_ptr[2 * useAlt + B_POSITION_OPPONENT_LEFT], species, pid);
            LoadCompressedSpritePalette(GetMonSpritePalStruct(mon));
        }
        break;
    case 1:
        SetMultiuseSpriteTemplateToPokemon(GetMonSpritePalStruct(mon)->tag, position);
        spriteId = CreateSprite(&gMultiuseSpriteTemplate, EGG_X, EGG_Y - 5, 6);
        gSprites[spriteId].invisible = TRUE;
        gSprites[spriteId].callback = SpriteCallbackDummy;
        break;
    }
    return spriteId;
}

static void VBlankCB_EggHatch(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void EggHatch(void)
{
    LockPlayerFieldControls();
    CreateTask(Task_EggHatch, 10);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}

static void Task_EggHatch(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_LoadEggHatch);
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        DestroyTask(taskId);
    }
}

static void CB2_LoadEggHatch(void)
{
    switch (gMain.state)
    {
    case 0:
        REG_DISPCNT = 0;
        gEggHatchData = eEggHatchData;
        gEggHatchData->eggPartyId = gSpecialVar_0x8004;
        gEggHatchData->eggShardVelocityId = 0;
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        SetVBlankCallback(VBlankCB_EggHatch);
        gMain.state++;
        gSpecialVar_0x8005 = GetCurrentMapMusic();
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E6F84);
        Text_InitWindowWithTemplate(&gEggHatchData->window, &gWindowTemplate_81E6F84);
        gEggHatchData->tileDataStartOffset = TextWindow_SetBaseTileNum(20);
        TextWindow_LoadStdFrameGraphics(&gEggHatchData->window);
        gMain.state++;
        break;
    case 2:
        LZDecompressVram(&gBattleTextboxTiles, (void*)(VRAM));
        CpuSet(&gBattleTextboxTilemap, gSharedMem, 0x800);
        DmaCopy16(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
        LoadCompressedPalette(&gBattleTextboxPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        gMain.state++;
        break;
    case 3:
        LoadSpriteSheet(&sEggHatch_Sheet);
        LoadSpriteSheet(&sEggShards_Sheet);
        LoadSpritePalette(&sEgg_SpritePalette);
        gMain.state++;
        break;
    case 4:
        gEggHatchData->eggSpriteId = CreateSprite(&sSpriteTemplate_Egg, EGG_X, EGG_Y, 5);
        AddHatchedMonToParty(gEggHatchData->eggPartyId);
        gMain.state++;
        break;
    case 5:
        EggHatchCreateMonSprite(FALSE, 0, gEggHatchData->eggPartyId);
        gMain.state++;
        break;
    case 6:
        gEggHatchData->monSpriteId = EggHatchCreateMonSprite(FALSE, 1, gEggHatchData->eggPartyId);
        gMain.state++;
        break;
    case 7:
        REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_16COLOR | BGCNT_SCREENBASE(12) | BGCNT_TXT512x256;
        LoadPalette(gTradeGba2_Pal, BG_PLTT_ID(1), 5 * PLTT_SIZE_4BPP);
        DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(1), 0x1300, 0x1000);
        DmaCopy16Defvars(3, gTradePlatform_Tilemap, BG_SCREEN_ADDR(12), 0x1000);
        gMain.state++;
        break;
    case 8:
        REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(5) | BGCNT_TXT256x256;

        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;

        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;

        REG_BG2HOFS = 0;
        REG_BG2VOFS = 0;

        SetMainCallback2(CB2_EggHatch);
        gEggHatchData->state = 0;
        break;
    }
}

static void EggHatchSetMonNickname(void)
{
    SetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_NICKNAME, gStringVar3);
    SetMainCallback2(CB2_ReturnToField);
}

#define tTimer data[0]

static void Task_EggHatchPlayBGM(u8 taskId)
{
    if (gTasks[taskId].tTimer == 0)
        StopMapMusic();
    if (gTasks[taskId].tTimer == 1)
        PlayBGM(MUS_EVOLUTION_INTRO);
    if (gTasks[taskId].tTimer > 60)
    {
        PlayBGM(MUS_EVOLUTION);
        DestroyTask(taskId);
        //return; task is destroyed, yet you increment the value?
    }
    gTasks[taskId].tTimer++;
}

static void CB2_EggHatch(void)
{
    switch (gEggHatchData->state)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
        gEggHatchData->state++;
        CreateTask(Task_EggHatchPlayBGM, 5);
        break;
    case 1:
        if (!gPaletteFade.active)
        {
            gEggHatchData->delayTimer = 0;
            gEggHatchData->state++;
        }
        break;
    case 2:
        if (++gEggHatchData->delayTimer > 30)
        {
            gEggHatchData->state++;
            gSprites[gEggHatchData->eggSpriteId].callback = SpriteCB_Egg_Shake1;
        }
        break;
    case 3:
        if (gSprites[gEggHatchData->eggSpriteId].callback == SpriteCallbackDummy)
            gEggHatchData->state++;
        break;
    case 4:
        GetMonNickname2(&gPlayerParty[gEggHatchData->eggPartyId], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_HatchedFromEgg);
        EggHatchPrintMessage2(gStringVar4);
        PlayFanfare(MUS_EVOLVED);
        gEggHatchData->state++;
        break;
    case 5:
        if (IsFanfareTaskInactive())
            gEggHatchData->state++;
        break;
    case 6:
        if (IsFanfareTaskInactive())
            gEggHatchData->state++;
        break;
    case 7:
        GetMonNickname2(&gPlayerParty[gEggHatchData->eggPartyId], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_NicknameHatchPrompt);
        EggHatchPrintMessage1(gStringVar4);
        gEggHatchData->state++;
        break;
    case 8:
        if (EggHatchUpdateWindowText())
        {
            Menu_DrawStdWindowFrame(22, 8, 27, 13);
            InitYesNoMenu(22, 8, 4);
            gEggHatchData->state++;
        }
        break;
    case 9:
        {
            s8 menuInput;
            if ((menuInput = Menu_ProcessInputNoWrap_()) != MENU_NOTHING_CHOSEN)
            {
                if (menuInput != MENU_B_PRESSED && menuInput != 1)
                {
                    u16 species;
                    u8 gender;
                    u32 personality;

                    GetMonNickname2(&gPlayerParty[gEggHatchData->eggPartyId], gStringVar3);
                    species = GetMonData(&gPlayerParty[gEggHatchData->eggPartyId], MON_DATA_SPECIES);
                    gender = GetMonGender(&gPlayerParty[gEggHatchData->eggPartyId]);
                    personality = GetMonData(&gPlayerParty[gEggHatchData->eggPartyId], MON_DATA_PERSONALITY, NULL);
                    DoNamingScreen(NAMING_SCREEN_NICKNAME, gStringVar3, species, gender, personality, EggHatchSetMonNickname);
                }
                else
                    gEggHatchData->state++;
            }
        }
        break;
    case 10:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gEggHatchData->state++;
        break;
    case 11:
        if (!gPaletteFade.active)
            SetMainCallback2(CB2_ReturnToField);
        break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

#define sTimer      data[0]
#define sSinIdx     data[1]
#define sDelayTimer data[2]

static void SpriteCB_Egg_Shake1(struct Sprite* sprite)
{
    if (++sprite->sTimer > 20)
    {
        sprite->callback = SpriteCB_Egg_Shake2;
        sprite->sTimer = 0;
    }
    else
    {
        sprite->sSinIdx = (sprite->sSinIdx + 20) & 0xFF;
        sprite->x2 = Sin(sprite->sSinIdx, 1);
        if (sprite->sTimer == 15)
        {
            PlaySE(SE_BALL);
            StartSpriteAnim(sprite, EGG_ANIM_CRACKED_1);
            CreateRandomEggShardSprite();
        }
    }
}

static void SpriteCB_Egg_Shake2(struct Sprite* sprite)
{
    if (++sprite->sDelayTimer > 30)
    {
        if (++sprite->sTimer > 20)
        {
            sprite->callback = SpriteCB_Egg_Shake3;
            sprite->sTimer = 0;
            sprite->sDelayTimer = 0;
        }
        else
        {
            sprite->sSinIdx = (sprite->sSinIdx + 20) & 0xFF;
            sprite->x2 = Sin(sprite->sSinIdx, 2);
            if (sprite->sTimer == 15)
            {
                PlaySE(SE_BALL);
                StartSpriteAnim(sprite, EGG_ANIM_CRACKED_2);
            }
        }
    }
}

static void SpriteCB_Egg_Shake3(struct Sprite* sprite)
{
    if (++sprite->sDelayTimer > 30)
    {
        if (++sprite->sTimer > 38)
        {
            u16 species;

            sprite->callback = SpriteCB_Egg_WaitHatch;
            sprite->sTimer = 0;
            species = GetMonData(&gPlayerParty[gEggHatchData->eggPartyId], MON_DATA_SPECIES);
            gSprites[gEggHatchData->monSpriteId].x2 = 0;
            gSprites[gEggHatchData->monSpriteId].y2 = gMonFrontPicCoords[species].y_offset;
        }
        else
        {
            sprite->sSinIdx = (sprite->sSinIdx + 20) & 0xFF;
            sprite->x2 = Sin(sprite->sSinIdx, 2);
            if (sprite->sTimer == 15)
            {
                PlaySE(SE_BALL);
                StartSpriteAnim(sprite, EGG_ANIM_CRACKED_2);
                CreateRandomEggShardSprite();
                CreateRandomEggShardSprite();
            }
            if (sprite->sTimer == 30)
                PlaySE(SE_BALL);
        }
    }
}

static void SpriteCB_Egg_WaitHatch(struct Sprite* sprite)
{
    if (++sprite->sTimer > 50)
    {
        sprite->callback = SpriteCB_Egg_Hatch;
        sprite->sTimer = 0;
    }
}

static void SpriteCB_Egg_Hatch(struct Sprite* sprite)
{
    s16 i;
    if (sprite->sTimer == 0)
        BeginNormalPaletteFade(PALETTES_ALL, -1, 0, 16, RGB_WHITEALPHA);
    if (sprite->sTimer < 4u)
    {
        for (i = 0; i <= 3; i++)
            CreateRandomEggShardSprite();
    }
    sprite->sTimer++;
    if (!gPaletteFade.active)
    {
        PlaySE(SE_EGG_HATCH);
        sprite->invisible = TRUE;
        sprite->callback = SpriteCB_Egg_Reveal;
        sprite->sTimer = 0;
    }
}

static void SpriteCB_Egg_Reveal(struct Sprite* sprite)
{
    if (sprite->sTimer == 0)
    {
        gSprites[gEggHatchData->monSpriteId].invisible = FALSE;
        StartSpriteAffineAnim(&gSprites[gEggHatchData->monSpriteId], BATTLER_AFFINE_EMERGE);
    }
    if (sprite->sTimer == 8)
        BeginNormalPaletteFade(PALETTES_ALL, -1, 16, 0, RGB_WHITEALPHA);
    if (sprite->sTimer <= 9)
        gSprites[gEggHatchData->monSpriteId].y -= 1;
    if (sprite->sTimer > 40)
        sprite->callback = SpriteCallbackDummy;
    sprite->sTimer++;
}

#define sVelocX data[1]
#define sVelocY data[2]
#define sAccelY data[3]
#define sDeltaX data[4]
#define sDeltaY data[5]

static void SpriteCB_EggShard(struct Sprite* sprite)
{
    sprite->sDeltaX += sprite->sVelocX;
    sprite->sDeltaY += sprite->sVelocY;

    sprite->x2 = sprite->sDeltaX / Q_8_8(1);
    sprite->y2 = sprite->sDeltaY / Q_8_8(1);

    sprite->sVelocY += sprite->sAccelY;

    if (sprite->y + sprite->y2 > sprite->y + 20 && sprite->sVelocY > 0)
        DestroySprite(sprite);
}

static const s16 sEggShardVelocities[][2] =
{
    {Q_8_8(-1.5),       Q_8_8(-3.75)},
    {Q_8_8(-5),         Q_8_8(-3)},
    {Q_8_8(3.5),        Q_8_8(-3)},
    {Q_8_8(-4),         Q_8_8(-3.75)},
    {Q_8_8(2),          Q_8_8(-1.5)},
    {Q_8_8(-0.5),       Q_8_8(-6.75)},
    {Q_8_8(5),          Q_8_8(-2.25)},
    {Q_8_8(-1.5),       Q_8_8(-3.75)},
    {Q_8_8(4.5),        Q_8_8(-1.5)},
    {Q_8_8(-1),         Q_8_8(-6.75)},
    {Q_8_8(4),          Q_8_8(-2.25)},
    {Q_8_8(-3.5),       Q_8_8(-3.75)},
    {Q_8_8(1),          Q_8_8(-1.5)},
    {Q_8_8(-3.515625),  Q_8_8(-6.75)},
    {Q_8_8(4.5),        Q_8_8(-2.25)},
    {Q_8_8(-0.5),       Q_8_8(-7.5)},
    {Q_8_8(1),          Q_8_8(-4.5)},
    {Q_8_8(-2.5),       Q_8_8(-2.25)},
    {Q_8_8(2.5),        Q_8_8(-7.5)},
};

static void CreateRandomEggShardSprite(void)
{
    u16 spriteAnimIndex;

    s16 velocityX = sEggShardVelocities[gEggHatchData->eggShardVelocityId][0];
    s16 velocityY = sEggShardVelocities[gEggHatchData->eggShardVelocityId][1];
    gEggHatchData->eggShardVelocityId++;
    spriteAnimIndex = Random() % ARRAY_COUNT(sSpriteAnimTable_EggShard);
    CreateEggShardSprite(EGG_X, EGG_Y - 15, velocityX, velocityY, Q_8_8(0.390625), spriteAnimIndex);
}

static void CreateEggShardSprite(u8 x, u8 y, s16 velocityX, s16 velocityY, s16 acceleration, u8 spriteAnimIndex)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_EggShard, x, y, 4);
    gSprites[spriteId].sVelocX = velocityX;
    gSprites[spriteId].sVelocY = velocityY;
    gSprites[spriteId].sAccelY = acceleration;
    StartSpriteAnim(&gSprites[spriteId], spriteAnimIndex);
}

static void EggHatchPrintMessage1(u8* string)
{
    Contest_StartTextPrinter(&gEggHatchData->window, string, gEggHatchData->tileDataStartOffset, 3, 15);
}

static void EggHatchPrintMessage2(u8* string)
{
    Text_InitWindowAndPrintText(&gEggHatchData->window, string, gEggHatchData->tileDataStartOffset, 3, 15);
}

static bool8 EggHatchUpdateWindowText(void)
{
    return Text_UpdateWindow(&gEggHatchData->window);
}
