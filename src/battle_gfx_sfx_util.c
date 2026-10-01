#include "global.h"
#include "battle.h"
#include "battle_anim_throw.h"
#include "battle_interface.h"
#include "blend_palette.h"
#include "contest.h"
#include "data2.h"
#include "decompress.h"
#include "main.h"
#include "m4a.h"
#include "palette.h"
#include "pokemon.h"
#include "battle_anim.h"
#include "party_menu.h"
#include "constants/songs.h"
#include "constants/moves.h"
#include "sound.h"
#include "constants/species.h"
#include "sprite.h"
#include "task.h"
#include "text.h"
#include "gba/m4a_internal.h"
#include "ewram.h"
#include "graphics.h"

extern u8 gBattleBufferA[][0x200];
extern u8 gActiveBattler;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerPositions[];
extern u8 gBattlerSpriteIds[];
extern u16 gIntroSlideFlags;
extern u8 gDoingBattleAnim;
extern u32 gTransformedPersonalities[];
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern void (*gBattlerControllerFuncs[])(void);
extern u8 gHealthboxSpriteIds[];
extern u8 gBattleControllerData[];
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u32 gBitTable[];
extern u16 gBattleTypeFlags;
extern u8 gBattleMonForms[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern void (*gAnimScriptCallback)(void);
extern u8 gAnimScriptActive;
extern const u8 *const gBattleAnims_General[];
extern const u8 *const gBattleAnims_Special[];
extern const struct CompressedSpriteSheet gTrainerFrontPicTable[];
extern const struct MonCoords gTrainerFrontPicCoords[];
extern const struct CompressedSpritePalette gTrainerFrontPicPaletteTable[];
extern const struct CompressedSpriteSheet gSpriteSheet_EnemyShadow;
extern const struct SpriteTemplate gSpriteTemplate_EnemyShadow;
extern const u8 gSubstituteDollTilemap[]; // graphics.s
extern const u8 gSubstituteDollGfx[]; // graphics.s
extern const u8 gSubstituteDollPal[]; // graphics.s
static const struct CompressedSpriteSheet sSpriteSheet_SinglesPlayerHealthbox =
{ gBattleWindowLargeGfx, 4096, 0xd6ff };

static const struct CompressedSpriteSheet sSpriteSheet_SinglesOpponentHealthbox =
{ gBattleWindowSmallGfx, 4096, 0xd701 };

static const struct CompressedSpriteSheet sSpriteSheets_DoublesPlayerHealthbox[] =
{
    { gBattleWindowSmall2Gfx, 2048, 0xd6ff },
    { gBattleWindowSmall2Gfx, 2048, 0xd700 },
};

static const struct CompressedSpriteSheet sSpriteSheets_DoublesOpponentHealthbox[] =
{
    { gBattleWindowSmall3Gfx, 2048, 0xd701 },
    { gBattleWindowSmall3Gfx, 2048, 0xd702 },
};

static const struct CompressedSpriteSheet sSpriteSheet_SafariHealthbox =
{ gBattleWindowLarge2Gfx, 4096, 0xd70b };

static const struct CompressedSpriteSheet sSpriteSheets_HealthBar[] =
{
    { gBlankGfxCompressed, 256, 0xd704 },
    { gBlankGfxCompressed, 288, 0xd705 },
    { gBlankGfxCompressed, 256, 0xd706 },
    { gBlankGfxCompressed, 288, 0xd707 },
};

static const struct SpritePalette sSpritePalettes_HealthBoxHealthBar[] =
{
    { gBattleInterface_BallStatusBarPal, 0xD6FF },
    { gBattleInterface_BallDisplayPal, 0xD704 },
};

extern void Task_PlayerController_RestoreBgmAfterCry(u8);
extern u8 IsBattlerSpritePresent(u8);
extern u8 GetBattlerSpriteDefault_Y(u8);
extern u8 GetSubstituteSpriteDefault_Y(u8);
extern void BufferBattlePartyCurrentOrder(void);
extern void SetIdleSpriteCallback(struct Sprite *);

static void Task_ClearBitWhenBattleTableAnimDone(u8 taskId);
static bool8 ShouldAnimBeDoneRegardlessOfSubstitute(u8 animId);
static void Task_ClearBitWhenSpecialAnimDone(u8 taskId);
static void ClearSpritesBattlerHealthboxAnimData(void);
void LoadBattleMonGfxAndAnimate(u8, u8, u8);
void ClearBehindSubstituteBit(u8 battler);
void LoadAndCreateEnemyShadowSprites(void);
void SpriteCB_SetInvisible(struct Sprite *);
void SpriteCB_EnemyShadow(struct Sprite *);

void SpriteCB_WaitForBattlerBallReleaseAnim(struct Sprite *sprite)
{
    u8 spriteId = sprite->data[1];

    if (gSprites[spriteId].affineAnimEnded && !gSprites[spriteId].invisible)
    {
        if (gSprites[spriteId].animPaused)
            gSprites[spriteId].animPaused = FALSE;
        else if (gSprites[spriteId].animEnded)
        {
            gSprites[spriteId].callback = SetIdleSpriteCallback;
            StartSpriteAffineAnim(&gSprites[spriteId], 0);
            sprite->callback = SpriteCallbackDummy;
        }
    }
}

void UnusedDoBattleSpriteAffineAnim(struct Sprite *sprite, bool8 unused)
{
    sprite->animPaused = TRUE;
    sprite->callback = SpriteCallbackDummy;
    if (!unused)
        StartSpriteAffineAnim(sprite, 1);
    else
        StartSpriteAffineAnim(sprite, 1);
    AnimateSprite(sprite);
}

void SpriteCB_TrainerSlideIn(struct Sprite *sprite)
{
    if (!(gIntroSlideFlags & 1))
    {
        sprite->x2 += sprite->data[0];
        if (sprite->x2 == 0)
            sprite->callback = SpriteCallbackDummy;
    }
}

void InitAndLaunchChosenStatusAnimation(bool8 isStatus2, u32 status)
{
    gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 1;
    if (!isStatus2)
    {
        if (status == 0x20)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_FRZ);
        else if (status == 8 || (status & 0x80))
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_PSN);
        else if (status == 0x10)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_BRN);
        else if (status & 7)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_SLP);
        else if (status == 0x40)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_PRZ);
        else
            gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
    }
    else
    {
        if (status & 0x000F0000)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_INFATUATION);
        else if (status & 7)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_CONFUSION);
        else if (status & 0x10000000)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_CURSED);
        else if (status & 0x08000000)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_NIGHTMARE);
        else if (status & 0x0000E000)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_WRAPPED);
        else
            gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
    }
}

bool8 TryHandleLaunchBattleTableAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 tableId, u16 argument)
{
    u8 taskId;

    if (tableId == B_ANIM_CASTFORM_CHANGE && (argument & 0x80))
    {
        gBattleMonForms[activeBattler] = argument & 0x7F;
        return TRUE;
    }
    if (gBattleSpriteInfo[activeBattler].behindSubstitute
     && !ShouldAnimBeDoneRegardlessOfSubstitute(tableId))
        return TRUE;
    if (gBattleSpriteInfo[activeBattler].behindSubstitute
     && tableId == B_ANIM_SUBSTITUTE_FADE
     && gSprites[gBattlerSpriteIds[activeBattler]].invisible)
    {
        LoadBattleMonGfxAndAnimate(activeBattler, TRUE, gBattlerSpriteIds[activeBattler]);
        ClearBehindSubstituteBit(activeBattler);
        return TRUE;
    }
    gBattleAnimAttacker = atkBattler;
    gBattleAnimTarget = defBattler;
    gBattleAnimationInfo.animArg = argument;
    LaunchBattleAnimation(gBattleAnims_General, tableId, 0);
    taskId = CreateTask(Task_ClearBitWhenBattleTableAnimDone, 10);
    gTasks[taskId].data[0] = activeBattler;
    gBattleHealthBoxInfo[gTasks[taskId].data[0]].animFromTableActive = 1;
    return FALSE;
}

static void Task_ClearBitWhenBattleTableAnimDone(u8 taskId)
{
    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        gBattleHealthBoxInfo[gTasks[taskId].data[0]].animFromTableActive = 0;
        DestroyTask(taskId);
    }
}

static bool8 ShouldAnimBeDoneRegardlessOfSubstitute(u8 animId)
{
    switch (animId)
    {
    case B_ANIM_SUBSTITUTE_FADE:
    case B_ANIM_RAIN_CONTINUES:
    case B_ANIM_SUN_CONTINUES:
    case B_ANIM_SANDSTORM_CONTINUES:
    case B_ANIM_HAIL_CONTINUES:
    case B_ANIM_SNATCH_MOVE:
        return TRUE;
    default:
        return FALSE;
    }
}

void InitAndLaunchSpecialAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 animId)
{
    u8 taskId;

    gBattleAnimAttacker = atkBattler;
    gBattleAnimTarget = defBattler;
    LaunchBattleAnimation(gBattleAnims_Special, animId, 0);
    taskId = CreateTask(Task_ClearBitWhenSpecialAnimDone, 10);
    gTasks[taskId].data[0] = activeBattler;
    gBattleHealthBoxInfo[gTasks[taskId].data[0]].specialAnimActive = 1;
}

static void Task_ClearBitWhenSpecialAnimDone(u8 taskId)
{
    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        gBattleHealthBoxInfo[gTasks[taskId].data[0]].specialAnimActive = 0;
        DestroyTask(taskId);
    }
}

u8 IsMoveWithoutAnimation(int unused1, int unused2)
{
    return 0;
}

bool8 IsBattleSEPlaying(u8 battler)
{
    u8 zero = 0;

    if (IsSEPlaying())
    {
        gBattleHealthBoxInfo[battler].soundTimer++;
        if (gBattleHealthBoxInfo[gActiveBattler].soundTimer < 30)
            return TRUE;
        m4aMPlayStop(&gMPlayInfo_SE1);
        m4aMPlayStop(&gMPlayInfo_SE2);
    }
    if (zero == 0)
    {
        gBattleHealthBoxInfo[battler].soundTimer = 0;
        return FALSE;
    }
    return TRUE;
}

void BattleLoadOpponentMonSpriteGfx(struct Pokemon *mon, u8 battler)
{
    u32 monsPersonality;
    u16 species;
    u32 currentPersonality;
    u32 otId;
    u8 position;
    u16 paletteOffset;
    const u8 *lzPaletteData;

    monsPersonality = GetMonData(mon, MON_DATA_PERSONALITY);
    if (gBattleSpriteInfo[battler].transformSpecies == 0)
    {
        species = GetMonData(mon, MON_DATA_SPECIES);
        currentPersonality = monsPersonality;
    }
    else
    {
        species = gBattleSpriteInfo[battler].transformSpecies;
        currentPersonality = gTransformedPersonalities[battler];
    }
    otId = GetMonData(mon, MON_DATA_OT_ID);
    position = GetBattlerPosition(battler);
    HandleLoadSpecialPokePic(
      &gMonFrontPicTable[species],
      gMonFrontPicCoords[species].coords,
      gMonFrontPicCoords[species].y_offset,
      sBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[position],
      species,
      currentPersonality);
    paletteOffset = 0x100 + battler * 16;
    if (gBattleSpriteInfo[battler].transformSpecies == 0)
        lzPaletteData = GetMonFrontSpritePal(mon);
    else
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, otId, monsPersonality);
    LZDecompressWram(lzPaletteData, gSharedMem);
    LoadPalette(gSharedMem, paletteOffset, 0x20);
    LoadPalette(gSharedMem, 0x80 + battler * 16, 0x20);
    if (species == SPECIES_CASTFORM)
    {
        paletteOffset = 0x100 + battler * 16;
        LZDecompressWram(lzPaletteData, sCastformPalette);
        LoadPalette(sCastformPalette + gBattleMonForms[battler] * 32, paletteOffset, 0x20);
    }
    if (gBattleSpriteInfo[battler].transformSpecies != 0)
    {
        BlendPalette(paletteOffset, 16, 6, RGB(31, 31, 31));
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
    }
}

void BattleLoadPlayerMonSpriteGfx(struct Pokemon *mon, u8 battler)
{
    u32 monsPersonality;
    u16 species;
    u32 currentPersonality;
    u32 otId;
    u8 position;
    u16 paletteOffset;
    const u8 *lzPaletteData;

    monsPersonality = GetMonData(mon, MON_DATA_PERSONALITY);
    if (gBattleSpriteInfo[battler].transformSpecies == 0)
    {
        species = GetMonData(mon, MON_DATA_SPECIES);
        currentPersonality = monsPersonality;
    }
    else
    {
        species = gBattleSpriteInfo[battler].transformSpecies;
        currentPersonality = gTransformedPersonalities[battler];
    }
    otId = GetMonData(mon, MON_DATA_OT_ID);
    position = GetBattlerPosition(battler);
    HandleLoadSpecialPokePic(
      &gMonBackPicTable[species],
      gMonBackPicCoords[species].coords,
      gMonBackPicCoords[species].y_offset,
      sBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[position],
      species,
      currentPersonality);
    paletteOffset = 0x100 + battler * 16;
    if (gBattleSpriteInfo[battler].transformSpecies == 0)
        lzPaletteData = GetMonFrontSpritePal(mon);
    else
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, otId, monsPersonality);
    LZDecompressWram(lzPaletteData, gSharedMem);
    LoadPalette(gSharedMem, paletteOffset, 0x20);
    LoadPalette(gSharedMem, 0x80 + battler * 16, 0x20);
    if (species == SPECIES_CASTFORM)
    {
        paletteOffset = 0x100 + battler * 16;
        LZDecompressWram(lzPaletteData, sCastformPalette);
        LoadPalette(sCastformPalette + gBattleMonForms[battler] * 32, paletteOffset, 0x20);
    }
    if (gBattleSpriteInfo[battler].transformSpecies != 0)
    {
        BlendPalette(paletteOffset, 16, 6, RGB(31, 31, 31));
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
    }
}

void BattleGfxSfxDummy1(void)
{
}

void BattleGfxSfxDummy2(u16 species)
{
}

void DecompressTrainerFrontPic(u16 frontPicId, u8 battler)
{
    u8 position;
    struct CompressedSpriteSheet spriteSheet;

    position = GetBattlerPosition(battler);
    DecompressPicFromTable(
      &gTrainerFrontPicTable[frontPicId],
      gTrainerFrontPicCoords[frontPicId].coords,
      gTrainerFrontPicCoords[frontPicId].y_offset,
      sBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[position],
      0);
    spriteSheet.data = gMonSpriteGfxBufferPtrs[position];
    spriteSheet.size = gTrainerFrontPicTable[frontPicId].size;
    spriteSheet.tag = gTrainerFrontPicTable[frontPicId].tag;
    LoadCompressedSpriteSheet(&spriteSheet);
    LoadCompressedSpritePalette(&gTrainerFrontPicPaletteTable[frontPicId]);
}

void DecompressTrainerBackPic(u16 backPicId, u8 battler)
{
    u8 position;

    position = GetBattlerPosition(battler);
    DecompressPicFromTable(
      &gTrainerBackPicTable[backPicId],
      gTrainerBackPicCoords[backPicId].coords,
      gTrainerBackPicCoords[backPicId].y_offset,
      sBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[position],
      0);
    LoadCompressedPalette(gTrainerBackPicPaletteTable[backPicId].data, 0x100 + battler * 16, 32);
}

void BattleGfxSfxDummy3(u8 gender)
{
}

void FreeTrainerFrontPicPalette(u16 frontPicId)
{
    FreeSpritePaletteByTag(gTrainerFrontPicPaletteTable[frontPicId].tag);
    FreeSpriteTilesByTag(gTrainerFrontPicTable[frontPicId].tag);
}

void BattleLoadAllHealthBoxesGfxAtOnce(void)
{
    u8 numberOfBattlers;
    u8 i;

    LoadSpritePalette(&sSpritePalettes_HealthBoxHealthBar[0]);
    LoadSpritePalette(&sSpritePalettes_HealthBoxHealthBar[1]);
    if (!IsDoubleBattle())
    {
        LoadCompressedSpriteSheet(&sSpriteSheet_SinglesPlayerHealthbox);
        LoadCompressedSpriteSheet(&sSpriteSheet_SinglesOpponentHealthbox);
        numberOfBattlers = 2;
    }
    else
    {
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[0]);
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[1]);
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[0]);
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[1]);
        numberOfBattlers = 4;
    }
    for (i = 0; i < numberOfBattlers; i++)
        LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[i]]);
}

bool8 BattleLoadAllHealthBoxesGfx(u8 state)
{
    bool8 retVal = FALSE;

    if (state != 0)
    {
        if (state == 1)
        {
            LoadSpritePalette(&sSpritePalettes_HealthBoxHealthBar[0]);
            LoadSpritePalette(&sSpritePalettes_HealthBoxHealthBar[1]);
        }
        else if (!IsDoubleBattle())
        {
            if (state == 2)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
                    LoadCompressedSpriteSheet(&sSpriteSheet_SafariHealthbox);
                else
                    LoadCompressedSpriteSheet(&sSpriteSheet_SinglesPlayerHealthbox);
            }
            else if (state == 3)
                LoadCompressedSpriteSheet(&sSpriteSheet_SinglesOpponentHealthbox);
            else if (state == 4)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[0]]);
            else if (state == 5)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[1]]);
            else
                retVal = TRUE;
        }
        else
        {
            if (state == 2)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[0]);
            else if (state == 3)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[1]);
            else if (state == 4)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[0]);
            else if (state == 5)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[1]);
            else if (state == 6)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[0]]);
            else if (state == 7)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[1]]);
            else if (state == 8)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[2]]);
            else if (state == 9)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[3]]);
            else
                retVal = TRUE;
        }
    }
    return retVal;
}

void LoadBattleBarGfx(u8 unused)
{
    LZDecompressWram(gBattleInterfaceGfx_HpNumbers, sBattleInterfaceGfxBuffer);
}

bool8 BattleInitAllSprites(u8 *state, u8 *battler)
{
    bool8 retVal = FALSE;

    switch (*state)
    {
    case 0:
        ClearSpritesBattlerHealthboxAnimData();
        (*state)++;
        break;
    case 1:
        if (BattleLoadAllHealthBoxesGfx(*battler) == 0)
        {
            (*battler)++;
        }
        else
        {
            *battler = 0;
            (*state)++;
        }
        break;
    case 2:
        (*state)++;
        break;
    case 3:
        if ((gBattleTypeFlags & BATTLE_TYPE_SAFARI) && *battler == 0)
            gHealthboxSpriteIds[*battler] = CreateSafariPlayerHealthboxSprites();
        else
            gHealthboxSpriteIds[*battler] = CreateBattlerHealthboxSprites(*battler);
        (*battler)++;
        if (*battler == gBattlersCount)
        {
            *battler = 0;
            (*state)++;
        }
        break;
    case 4:
        InitBattlerHealthboxCoords(*battler);
        if (gBattlerPositions[*battler] <= B_POSITION_OPPONENT_LEFT)
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[*battler], FALSE);
        else
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[*battler], TRUE);
        (*battler)++;
        if (*battler == gBattlersCount)
        {
            *battler = 0;
            (*state)++;
        }
        break;
    case 5:
        if (GetBattlerSide(*battler) == B_SIDE_PLAYER)
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
                UpdateHealthboxAttribute(gHealthboxSpriteIds[*battler], &gPlayerParty[gBattlerPartyIndexes[*battler]], 0);
        }
        else
        {
            UpdateHealthboxAttribute(gHealthboxSpriteIds[*battler], &gEnemyParty[gBattlerPartyIndexes[*battler]], 0);
        }
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[*battler]);
        (*battler)++;
        if (*battler == gBattlersCount)
        {
            *battler = 0;
            (*state)++;
        }
        break;
    case 6:
        LoadAndCreateEnemyShadowSprites();
        BufferBattlePartyCurrentOrder();
        retVal = TRUE;
        break;
    }
    return retVal;
}

void ClearSpritesHealthboxAnimData(void)
{
    memset(gBattleHealthBoxInfo, 0, 0x30);
    memset(&gBattleAnimationInfo, 0, sizeof(gBattleAnimationInfo));
}

static void ClearSpritesBattlerHealthboxAnimData(void)
{
    ClearSpritesHealthboxAnimData();
    memset(gBattleSpriteInfo, 0, 0x10);
}

void CopyAllBattleSpritesInvisibilities(void)
{
    s32 i;

    for (i = 0; i < gBattlersCount; i++)
        gBattleSpriteInfo[i].invisible = gSprites[gBattlerSpriteIds[i]].invisible;
}

void CopyBattleSpriteInvisibility(u8 battler)
{
    gBattleSpriteInfo[battler].invisible = gSprites[gBattlerSpriteIds[battler]].invisible;
}

void HandleSpeciesGfxDataChange(u8 battlerAtk, u8 battlerDef, bool8 castform)
{
    u16 paletteOffset;
    u16 targetSpecies;
    u32 personalityValue;
    u32 otId;
    u8 position;
    const u8 *lzPaletteData;

    if (castform)
    {
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[battlerAtk]], gBattleAnimationInfo.animArg);
        paletteOffset = 0x100 + battlerAtk * 16;
        LoadPalette(sCastformPalette + gBattleAnimationInfo.animArg * 32, paletteOffset, 32);
        gBattleMonForms[battlerAtk] = gBattleAnimationInfo.animArg;
        if (gBattleSpriteInfo[battlerAtk].transformSpecies != SPECIES_NONE)
        {
            BlendPalette(paletteOffset, 16, 6, RGB(31, 31, 31));
            CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
        }
        gSprites[gBattlerSpriteIds[battlerAtk]].y = GetBattlerSpriteDefault_Y(battlerAtk);
    }
    else
    {
        if (IsContest())
        {
            position = B_POSITION_PLAYER_LEFT;
            targetSpecies = gContestResources__moveAnim.targetSpecies;
            personalityValue = gContestResources__moveAnim.personality;
            otId = gContestResources__moveAnim.otId;
            HandleLoadSpecialPokePic(
              &gMonBackPicTable[targetSpecies],
              gMonBackPicCoords[targetSpecies].coords,
              gMonBackPicCoords[targetSpecies].y_offset,
              sBattleInterfaceGfxBuffer,
              gMonSpriteGfxBufferPtrs[position],
              targetSpecies,
                gContestResources__moveAnim.targetPersonality);
        }
        else
        {
            position = GetBattlerPosition(battlerAtk);
            if (GetBattlerSide(battlerDef) == B_SIDE_OPPONENT)
                targetSpecies = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battlerDef]], MON_DATA_SPECIES);
            else
                targetSpecies = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerDef]], MON_DATA_SPECIES);
            if (GetBattlerSide(battlerAtk) == B_SIDE_PLAYER)
            {
                personalityValue = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerAtk]], MON_DATA_PERSONALITY);
                otId = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerAtk]], MON_DATA_OT_ID);
                HandleLoadSpecialPokePic(
                  &gMonBackPicTable[targetSpecies],
                  gMonBackPicCoords[targetSpecies].coords,
                  gMonBackPicCoords[targetSpecies].y_offset,
                  sBattleInterfaceGfxBuffer,
                  gMonSpriteGfxBufferPtrs[position],
                  targetSpecies,
                  gTransformedPersonalities[battlerAtk]);
            }
            else
            {
                personalityValue = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battlerAtk]], MON_DATA_PERSONALITY);
                otId = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battlerAtk]], MON_DATA_OT_ID);
                HandleLoadSpecialPokePic(
                  &gMonFrontPicTable[targetSpecies],
                  gMonFrontPicCoords[targetSpecies].coords,
                  gMonFrontPicCoords[targetSpecies].y_offset,
                  sBattleInterfaceGfxBuffer,
                  gMonSpriteGfxBufferPtrs[position],
                  targetSpecies,
                  gTransformedPersonalities[battlerAtk]);
            }
        }
        DmaCopy32Defvars(3, gMonSpriteGfxBufferPtrs[position], (void *)(VRAM + 0x10000 + gSprites[gBattlerSpriteIds[battlerAtk]].oam.tileNum * 32), 0x800);
        paletteOffset = 0x100 + battlerAtk * 16;
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(targetSpecies, otId, personalityValue);
        LZDecompressWram(lzPaletteData, gSharedMem);
        LoadPalette(gSharedMem, paletteOffset, 32);
        if (targetSpecies == SPECIES_CASTFORM)
        {
            u16 *paletteSrc = (u16 *)sCastformPalette; // TODO: avoid casting?

            LZDecompressWram(lzPaletteData, paletteSrc);
            LoadPalette(paletteSrc + gBattleMonForms[battlerDef] * 16, paletteOffset, 32);
        }
        BlendPalette(paletteOffset, 16, 6, RGB(31, 31, 31));
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
        if (!IsContest())
        {
            gBattleSpriteInfo[battlerAtk].transformSpecies = targetSpecies;
            gBattleMonForms[battlerAtk] = gBattleMonForms[battlerDef];
        }
        gSprites[gBattlerSpriteIds[battlerAtk]].y = GetBattlerSpriteDefault_Y(battlerAtk);
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[battlerAtk]], gBattleMonForms[battlerAtk]);
    }
}

void BattleLoadSubstituteOrMonSpriteGfx(u8 battler, u8 loadMonSprite)
{
    u8 position;
    u16 palOffset;
    const u8 *substituteDollPal;
    void *gfxSrc;
    s32 i;

    if (loadMonSprite == 0)
    {
        if (IsContest())
            position = 0;
        else
            position = GetBattlerPosition(battler);
        if (IsContest())
            LZDecompressVram(gSubstituteDollTilemap, gMonSpriteGfxBufferPtrs[position]);
        else if (GetBattlerSide(battler) != 0)
            LZDecompressVram(gSubstituteDollGfx, gMonSpriteGfxBufferPtrs[position]);
        else
            LZDecompressVram(gSubstituteDollTilemap, gMonSpriteGfxBufferPtrs[position]);
        // There is probably a way to do this without all the temp variables, but I couldn't figure it out.
        palOffset = battler * 16;
        substituteDollPal = gSubstituteDollPal;
        gfxSrc = gMonSpriteGfxBufferPtrs[position];
        for (i = 0; i < 3; i++)
            DmaCopy32(3, gfxSrc, gfxSrc + i * 0x800 + 0x800, 0x800);
        LoadCompressedPalette(substituteDollPal, 0x100 + palOffset, 32);
    }
    else
    {
        if (!IsContest())
        {
            if (GetBattlerSide(battler) != 0)
                BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[battler]], battler);
            else
                BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[battler]], battler);
        }
    }
}

void LoadBattleMonGfxAndAnimate(u8 battler, u8 loadMonSprite, u8 spriteId)
{
    BattleLoadSubstituteOrMonSpriteGfx(battler, loadMonSprite);
    StartSpriteAnim(&gSprites[spriteId], gBattleMonForms[battler]);
    if (loadMonSprite == 0)
        gSprites[spriteId].y = GetSubstituteSpriteDefault_Y(battler);
    else
        gSprites[spriteId].y = GetBattlerSpriteDefault_Y(battler);
}

void TrySetBehindSubstituteSpriteBit(u8 battler, u16 move)
{
    if (move == MOVE_SUBSTITUTE)
        gBattleSpriteInfo[battler].behindSubstitute = TRUE;
}

void ClearBehindSubstituteBit(u8 battler)
{
    gBattleSpriteInfo[battler].behindSubstitute = FALSE;
}

void HandleLowHpMusicChange(struct Pokemon *mon, u8 battler)
{
    u16 hp = GetMonData(mon, MON_DATA_HP);
    u16 maxHP = GetMonData(mon, MON_DATA_MAX_HP);

    if (GetHPBarLevel(hp, maxHP) == 1)
    {
        if (!gBattleSpriteInfo[battler].lowHpSong)
        {
            if (!gBattleSpriteInfo[BATTLE_PARTNER(battler)].lowHpSong)
                PlaySE(SE_LOW_HEALTH);
            gBattleSpriteInfo[battler].lowHpSong = 1;
        }
    }
    else
    {
        gBattleSpriteInfo[battler].lowHpSong = 0;
        if (!IsDoubleBattle())
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            return;
        }
        if (IsDoubleBattle() && !gBattleSpriteInfo[BATTLE_PARTNER(battler)].lowHpSong)
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            return;
        }
    }
}

void BattleStopLowHpSound(void)
{
    u8 playerBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

    gBattleSpriteInfo[playerBattler].lowHpSong = 0;
    if (IsDoubleBattle())
        gBattleSpriteInfo[BATTLE_PARTNER(playerBattler)].lowHpSong = 0;
    m4aSongNumStop(SE_LOW_HEALTH);
}

u8 GetMonHPBarLevel(struct Pokemon *mon)
{
    u16 hp = GetMonData(mon, MON_DATA_HP);
    u16 maxHP = GetMonData(mon, MON_DATA_MAX_HP);

    return GetHPBarLevel(hp, maxHP);
}

void HandleBattleLowHpMusicChange(void)
{
    if (gMain.inBattle)
    {
        u8 playerBattler1 = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        u8 playerBattler2 = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        u8 battler1PartyId = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[playerBattler1]);
        u8 battler2PartyId = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[playerBattler2]);

        if (GetMonData(&gPlayerParty[battler1PartyId], MON_DATA_HP) != 0)
            HandleLowHpMusicChange(&gPlayerParty[battler1PartyId], playerBattler1);
        if (IsDoubleBattle())
        {
            if (GetMonData(&gPlayerParty[battler2PartyId], MON_DATA_HP) != 0)
                HandleLowHpMusicChange(&gPlayerParty[battler2PartyId], playerBattler2);
        }
    }
}

void SetBattlerSpriteAffineMode(u8 affineMode)
{
    s32 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (IsBattlerSpritePresent(i) != 0)
        {
            gSprites[gBattlerSpriteIds[i]].oam.affineMode = affineMode;
            if (affineMode == 0)
            {
                gBattleHealthBoxInfo[i].matrixNum = gSprites[gBattlerSpriteIds[i]].oam.matrixNum;
                gSprites[gBattlerSpriteIds[i]].oam.matrixNum = 0;
            }
            else
            {
                gSprites[gBattlerSpriteIds[i]].oam.matrixNum = gBattleHealthBoxInfo[i].matrixNum;
            }
        }
    }
}

void LoadAndCreateEnemyShadowSprites(void)
{
    u8 opponentBattler;

    LoadCompressedSpriteSheet(&gSpriteSheet_EnemyShadow);
    opponentBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
    gBattleHealthBoxInfo[opponentBattler].shadowSpriteId = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(opponentBattler, 0), GetBattlerSpriteCoord(opponentBattler, 1) + 32, 0xC8);
    gSprites[gBattleHealthBoxInfo[opponentBattler].shadowSpriteId].data[0] = opponentBattler;
    if (IsDoubleBattle())
    {
        opponentBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        gBattleHealthBoxInfo[opponentBattler].shadowSpriteId = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(opponentBattler, 0), GetBattlerSpriteCoord(opponentBattler, 1) + 32, 0xC8);
        gSprites[gBattleHealthBoxInfo[opponentBattler].shadowSpriteId].data[0] = opponentBattler;
    }
}

void SpriteCB_EnemyShadow(struct Sprite *sprite)
{
    bool8 invisible = FALSE;
    u8 battler = sprite->data[0];
    struct Sprite *battlerSprite = &gSprites[gBattlerSpriteIds[battler]];

    if (!battlerSprite->inUse || IsBattlerSpritePresent(battler) == 0)
    {
        sprite->callback = SpriteCB_SetInvisible;
        return;
    }
    if (gAnimScriptActive || battlerSprite->invisible)
        invisible = TRUE;
    else if (gBattleSpriteInfo[battler].transformSpecies != 0
          && gEnemyMonElevation[gBattleSpriteInfo[battler].transformSpecies] == 0)
        invisible = TRUE;
    if (gBattleSpriteInfo[battler].behindSubstitute)
        invisible = TRUE;
    sprite->x = battlerSprite->x;
    sprite->x2 = battlerSprite->x2;
    sprite->invisible = invisible;
}

void SpriteCB_SetInvisible(struct Sprite *sprite)
{
    sprite->invisible = TRUE;
}

void SetBattlerShadowSpriteCallback(u8 battler, u16 species)
{
    if (GetBattlerSide(battler) != B_SIDE_PLAYER)
    {
        if (gBattleSpriteInfo[battler].transformSpecies != SPECIES_NONE)
            species = gBattleSpriteInfo[battler].transformSpecies;
        if (gEnemyMonElevation[species] != 0)
            gSprites[gBattleHealthBoxInfo[battler].shadowSpriteId].callback = SpriteCB_EnemyShadow;
        else
            gSprites[gBattleHealthBoxInfo[battler].shadowSpriteId].callback = SpriteCB_SetInvisible;
    }
}

void HideBattlerShadowSprite(u8 battler)
{
    gSprites[gBattleHealthBoxInfo[battler].shadowSpriteId].callback = SpriteCB_SetInvisible;
}

void FillAroundBattleWindows(void)
{
    u16 *vramPtr = (u16 *)(VRAM + 0x240);
    s32 tile;
    s32 pixelWord;

    for (tile = 0; tile < 9; tile++)
    {
        for (pixelWord = 0; pixelWord < 16; pixelWord++)
        {
            if (!(*vramPtr & 0xF000))
                *vramPtr |= 0xF000;
            if (!(*vramPtr & 0x0F00))
                *vramPtr |= 0x0F00;
            if (!(*vramPtr & 0x00F0))
                *vramPtr |= 0x00F0;
            if (!(*vramPtr & 0x000F))
                *vramPtr |= 0x000F;
            vramPtr++;
        }
    }
}

void ClearTemporarySpeciesSpriteData(u8 battler, bool8 dontClearSubstitute)
{
    gBattleSpriteInfo[battler].transformSpecies = 0;
    gBattleMonForms[battler] = 0;
    if (!dontClearSubstitute)
        ClearBehindSubstituteBit(battler);
}
