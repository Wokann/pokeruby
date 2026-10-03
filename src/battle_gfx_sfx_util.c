#include "global.h"
#include "battle.h"
#include "battle_gfx_sfx_util.h"
#include "battle_main.h"
#include "battle_anim.h"
#include "battle_anim_special.h"
#include "battle_interface.h"
#include "blend_palette.h"
#include "contest.h"
#include "data2.h"
#include "decompress.h"
#include "main.h"
#include "m4a.h"
#include "palette.h"
#include "pokemon.h"
#include "rom_8077ABC.h"
#include "rom_8094928.h"
#include "constants/songs.h"
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

static const struct CompressedSpriteSheet sSpriteSheet_SinglesPlayerHealthbox =
{ gHealthboxSinglesPlayerGfx, 4096, 0xd6ff };

static const struct CompressedSpriteSheet sSpriteSheet_SinglesOpponentHealthbox =
{ gHealthboxSinglesOpponentGfx, 4096, 0xd701 };

static const struct CompressedSpriteSheet sSpriteSheets_DoublesPlayerHealthbox[] =
{
    { gHealthboxDoublesPlayerGfx, 2048, 0xd6ff },
    { gHealthboxDoublesPlayerGfx, 2048, 0xd700 },
};

static const struct CompressedSpriteSheet sSpriteSheets_DoublesOpponentHealthbox[] =
{
    { gHealthboxDoublesOpponentGfx, 2048, 0xd701 },
    { gHealthboxDoublesOpponentGfx, 2048, 0xd702 },
};

static const struct CompressedSpriteSheet sSpriteSheet_SafariHealthbox =
{ gHealthboxSafariGfx, 4096, 0xd70b };

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
extern u8 sub_8077F7C(u8);
extern void sub_8094958(void);
extern void move_anim_start_t2();

static void Task_ClearBitWhenBattleTableAnimDone(u8);
static u8 ShouldAnimBeDoneRegardlessOfSubstitute(u8);
static void Task_ClearBitWhenSpecialAnimDone(u8);
static void ClearSpritesBattlerHealthboxAnimData(void);
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

static void UNUSED UnusedDoBattleSpriteAffineAnim(struct Sprite *sprite, bool8 pointless)
{
    sprite->animPaused = TRUE;
    sprite->callback = SpriteCallbackDummy;
    if (!pointless)
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

void InitAndLaunchChosenStatusAnimation(u8 isStatus2, u32 status)
{
    gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 1;
    if (isStatus2 == 0)
    {
        if (status == 0x20)
            move_anim_start_t2(gActiveBattler, 6);
        else if (status == 8 || (status & 0x80))
            move_anim_start_t2(gActiveBattler, 0);
        else if (status == 0x10)
            move_anim_start_t2(gActiveBattler, 2);
        else if (status & 7)
            move_anim_start_t2(gActiveBattler, 4);
        else if (status == 0x40)
            move_anim_start_t2(gActiveBattler, 5);
        else
            gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
    }
    else
    {
        if (status & 0x000F0000)
            move_anim_start_t2(gActiveBattler, 3);
        else if (status & 7)
            move_anim_start_t2(gActiveBattler, 1);
        else if (status & 0x10000000)
            move_anim_start_t2(gActiveBattler, 7);
        else if (status & 0x08000000)
            move_anim_start_t2(gActiveBattler, 8);
        else if (status & 0x0000E000)
            move_anim_start_t2(gActiveBattler, 9);
        else
            gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = 0;
    }
}

bool8 TryHandleLaunchBattleTableAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 tableId, u16 argument)
{
    u8 taskId;

    if (tableId == 0 && (argument & 0x80))
    {
        gBattleMonForms[activeBattler] = argument & 0x7F;
        return TRUE;
    }
    if (gBattleSpriteInfo[activeBattler].behindSubstitute && ShouldAnimBeDoneRegardlessOfSubstitute(tableId) == 0)
        return TRUE;
    if (gBattleSpriteInfo[activeBattler].behindSubstitute && tableId == 2 && gSprites[gBattlerSpriteIds[activeBattler]].invisible)
    {
        LoadBattleMonGfxAndAnimate(activeBattler, 1, gBattlerSpriteIds[activeBattler]);
        ClearBehindSubstituteBit(activeBattler);
        return TRUE;
    }
    gBattleAnimAttacker = atkBattler;
    gBattleAnimTarget = defBattler;
    gBattleAnimationInfo->animArg = argument;
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

static u8 ShouldAnimBeDoneRegardlessOfSubstitute(u8 animId)
{
    switch (animId)
    {
    case 2:
    case 10:
    case 11:
    case 12:
    case 13:
    case 17:
        return 1;
    default:
        return 0;
    }
}

void InitAndLaunchSpecialAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 tableId)
{
    u8 taskId;

    gBattleAnimAttacker = atkBattler;
    gBattleAnimTarget = defBattler;
    LaunchBattleAnimation(gBattleAnims_Special, tableId, 0);
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
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
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
        LZDecompressWram(lzPaletteData, ewram16400);
        LoadPalette(ewram16400 + gBattleMonForms[battler] * 32, paletteOffset, 0x20);
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
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
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
        LZDecompressWram(lzPaletteData, ewram16400);
        LoadPalette(ewram16400 + gBattleMonForms[battler] * 32, paletteOffset, 0x20);
    }
    if (gBattleSpriteInfo[battler].transformSpecies != 0)
    {
        BlendPalette(paletteOffset, 16, 6, RGB(31, 31, 31));
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
    }
}

static void UNUSED BattleGfxSfxDummy1(void)
{
}

void BattleGfxSfxDummy2(u16 unused)
{
}

void DecompressTrainerFrontPic(u16 frontPicId, u8 battler)
{
    u8 position;
    struct CompressedSpriteSheet spriteSheet;

    position = GetBattlerPosition(battler);
    DecompressPicFromTable_2(
      &gTrainerFrontPicTable[frontPicId],
      gTrainerFrontPicCoords[frontPicId].coords,
      gTrainerFrontPicCoords[frontPicId].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
      0);
    spriteSheet.data = gMonSpriteGfx_Sprite_ptr[position];
    spriteSheet.size = gTrainerFrontPicTable[frontPicId].size;
    spriteSheet.tag = gTrainerFrontPicTable[frontPicId].tag;
    LoadCompressedObjectPic(&spriteSheet);
    LoadCompressedObjectPalette(&gTrainerFrontPicPaletteTable[frontPicId]);
}

void DecompressTrainerBackPic(u16 backPicId, u8 battler)
{
    u8 position;

    position = GetBattlerPosition(battler);
    DecompressPicFromTable_2(
      &gTrainerBackPicTable[backPicId],
      gTrainerBackPicCoords[backPicId].coords,
      gTrainerBackPicCoords[backPicId].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
      0);
    LoadCompressedPalette(gTrainerBackPicPaletteTable[backPicId].data, 0x100 + battler * 16, 32);
}

void BattleGfxSfxDummy3(int unused)
{
}

void sub_8031B74(u16 frontPicId)
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
        LoadCompressedObjectPic(&sSpriteSheet_SinglesPlayerHealthbox);
        LoadCompressedObjectPic(&sSpriteSheet_SinglesOpponentHealthbox);
        numberOfBattlers = 2;
    }
    else
    {
        LoadCompressedObjectPic(&sSpriteSheets_DoublesPlayerHealthbox[0]);
        LoadCompressedObjectPic(&sSpriteSheets_DoublesPlayerHealthbox[1]);
        LoadCompressedObjectPic(&sSpriteSheets_DoublesOpponentHealthbox[0]);
        LoadCompressedObjectPic(&sSpriteSheets_DoublesOpponentHealthbox[1]);
        numberOfBattlers = 4;
    }
    for (i = 0; i < numberOfBattlers; i++)
        LoadCompressedObjectPic(&sSpriteSheets_HealthBar[gBattlerPositions[i]]);
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
                if (gBattleTypeFlags & 0x80)
                    LoadCompressedObjectPic(&sSpriteSheet_SafariHealthbox);
                else
                    LoadCompressedObjectPic(&sSpriteSheet_SinglesPlayerHealthbox);
            }
            else if (state == 3)
                LoadCompressedObjectPic(&sSpriteSheet_SinglesOpponentHealthbox);
            else if (state == 4)
                LoadCompressedObjectPic(&sSpriteSheets_HealthBar[gBattlerPositions[0]]);
            else if (state == 5)
                LoadCompressedObjectPic(&sSpriteSheets_HealthBar[gBattlerPositions[1]]);
            else
                retVal = TRUE;
        }
        else
        {
            if (state == 2)
                LoadCompressedObjectPic(&sSpriteSheets_DoublesPlayerHealthbox[0]);
            else if (state == 3)
                LoadCompressedObjectPic(&sSpriteSheets_DoublesPlayerHealthbox[1]);
            else if (state == 4)
                LoadCompressedObjectPic(&sSpriteSheets_DoublesOpponentHealthbox[0]);
            else if (state == 5)
                LoadCompressedObjectPic(&sSpriteSheets_DoublesOpponentHealthbox[1]);
            else if (state == 6)
                LoadCompressedObjectPic(&sSpriteSheets_HealthBar[gBattlerPositions[0]]);
            else if (state == 7)
                LoadCompressedObjectPic(&sSpriteSheets_HealthBar[gBattlerPositions[1]]);
            else if (state == 8)
                LoadCompressedObjectPic(&sSpriteSheets_HealthBar[gBattlerPositions[2]]);
            else if (state == 9)
                LoadCompressedObjectPic(&sSpriteSheets_HealthBar[gBattlerPositions[3]]);
            else
                retVal = TRUE;
        }
    }
    return retVal;
}

void LoadBattleBarGfx(u8 unused)
{
    LZDecompressWram(gBattleInterfaceGfx_BattleBar, eBattleInterfaceGfxBuffer);
}

u8 BattleInitAllSprites(u8 *state1, u8 *battler)
{
    bool8 retVal = FALSE;

    switch (*state1)
    {
    case 0:
        ClearSpritesBattlerHealthboxAnimData();
        (*state1)++;
        break;
    case 1:
        if (BattleLoadAllHealthBoxesGfx(*battler) == 0)
        {
            (*battler)++;
        }
        else
        {
            *battler = 0;
            (*state1)++;
        }
        break;
    case 2:
        (*state1)++;
        break;
    case 3:
        if ((gBattleTypeFlags & 0x80) && *battler == 0)
            gHealthboxSpriteIds[*battler] = battle_make_oam_safari_battle();
        else
            gHealthboxSpriteIds[*battler] = battle_make_oam_normal_battle(*battler);
        (*battler)++;
        if (*battler == gBattlersCount)
        {
            *battler = 0;
            (*state1)++;
        }
        break;
    case 4:
        sub_8043F44(*battler);
        if (gBattlerPositions[*battler] <= 1)
            nullsub_11(gHealthboxSpriteIds[*battler], 0);
        else
            nullsub_11(gHealthboxSpriteIds[*battler], 1);
        (*battler)++;
        if (*battler == gBattlersCount)
        {
            *battler = 0;
            (*state1)++;
        }
        break;
    case 5:
        if (GetBattlerSide(*battler) == 0)
        {
            if (!(gBattleTypeFlags & 0x80))
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
            (*state1)++;
        }
        break;
    case 6:
        LoadAndCreateEnemyShadowSprites();
        sub_8094958();
        retVal = TRUE;
        break;
    }
    return retVal;
}

void ClearSpritesHealthboxAnimData(void)
{
    memset(gBattleHealthBoxInfo, 0, 0x30);
    memset(gBattleAnimationInfo, 0, 0x10);
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
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[battlerAtk]], gBattleAnimationInfo->animArg);
        paletteOffset = 0x100 + battlerAtk * 16;
        LoadPalette(ewram16400 + gBattleAnimationInfo->animArg * 32, paletteOffset, 32);
        gBattleMonForms[battlerAtk] = gBattleAnimationInfo->animArg;
        if (gBattleSpriteInfo[battlerAtk].transformSpecies != 0)
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
            position = 0;
            targetSpecies = gContestResources__moveAnim.targetSpecies;
            personalityValue = gContestResources__moveAnim.personality;
            otId = gContestResources__moveAnim.otId;
            HandleLoadSpecialPokePic(
              &gMonBackPicTable[targetSpecies],
              gMonBackPicCoords[targetSpecies].coords,
              gMonBackPicCoords[targetSpecies].y_offset,
              eBattleInterfaceGfxBuffer,
              gMonSpriteGfx_Sprite_ptr[0],
              targetSpecies,
                gContestResources__moveAnim.targetPersonality);
        }
        else
        {
            position = GetBattlerPosition(battlerAtk);
            if (GetBattlerSide(battlerDef) == 1)
                targetSpecies = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battlerDef]], MON_DATA_SPECIES);
            else
                targetSpecies = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerDef]], MON_DATA_SPECIES);
            if (GetBattlerSide(battlerAtk) == 0)
            {
                personalityValue = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerAtk]], MON_DATA_PERSONALITY);
                otId = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerAtk]], MON_DATA_OT_ID);
                HandleLoadSpecialPokePic(
                  &gMonBackPicTable[targetSpecies],
                  gMonBackPicCoords[targetSpecies].coords,
                  gMonBackPicCoords[targetSpecies].y_offset,
                  eBattleInterfaceGfxBuffer,
                  gMonSpriteGfx_Sprite_ptr[position],
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
                  eBattleInterfaceGfxBuffer,
                  gMonSpriteGfx_Sprite_ptr[position],
                  targetSpecies,
                  gTransformedPersonalities[battlerAtk]);
            }
        }
        DmaCopy32Defvars(3, gMonSpriteGfx_Sprite_ptr[position], (void *)(VRAM + 0x10000 + gSprites[gBattlerSpriteIds[battlerAtk]].oam.tileNum * 32), 0x800);
        paletteOffset = 0x100 + battlerAtk * 16;
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(targetSpecies, otId, personalityValue);
        LZDecompressWram(lzPaletteData, gSharedMem);
        LoadPalette(gSharedMem, paletteOffset, 32);
        if (targetSpecies == SPECIES_CASTFORM)
        {
            u16 *paletteSrc = (u16 *)ewram16400; // TODO: avoid casting?

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
    const u8 *gSubstituteDollPal_;
    void *src;
    s32 i;

    if (loadMonSprite == 0)
    {
        if (IsContest())
            position = 0;
        else
            position = GetBattlerPosition(battler);
        if (IsContest())
            LZDecompressVram(gSubstituteDollBackGfx, gMonSpriteGfx_Sprite_ptr[position]);
        else if (GetBattlerSide(battler) != 0)
            LZDecompressVram(gSubstituteDollFrontGfx, gMonSpriteGfx_Sprite_ptr[position]);
        else
            LZDecompressVram(gSubstituteDollBackGfx, gMonSpriteGfx_Sprite_ptr[position]);
        // There is probably a way to do this without all the temp variables, but I couldn't figure it out.
        palOffset = battler * 16;
        gSubstituteDollPal_ = gSubstituteDollPal;
        src = gMonSpriteGfx_Sprite_ptr[position];
        for (i = 0; i < 3; i++)
            DmaCopy32(3, src, src + i * 0x800 + 0x800, 0x800);
        LoadCompressedPalette(gSubstituteDollPal_, 0x100 + palOffset, 32);
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
        gSprites[spriteId].y = sub_8077F7C(battler);
    else
        gSprites[spriteId].y = GetBattlerSpriteDefault_Y(battler);
}

void TrySetBehindSubstituteSpriteBit(u8 battler, u16 move)
{
    if (move == 0xA4)
        gBattleSpriteInfo[battler].behindSubstitute = 1;
}

void ClearBehindSubstituteBit(u8 battler)
{
    gBattleSpriteInfo[battler].behindSubstitute = 0;
}

void HandleLowHpMusicChange(struct Pokemon *mon, u8 battler)
{
    u16 hp = GetMonData(mon, MON_DATA_HP);
    u16 maxHP = GetMonData(mon, MON_DATA_MAX_HP);

    if (GetHPBarLevel(hp, maxHP) == 1)
    {
        if (!gBattleSpriteInfo[battler].lowHpSong)
        {
            if (!gBattleSpriteInfo[battler ^ 2].lowHpSong)
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
        if (IsDoubleBattle() && !gBattleSpriteInfo[battler ^ 2].lowHpSong)
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            return;
        }
    }
}

void BattleStopLowHpSound(void)
{
    u8 playerBattler = GetBattlerAtPosition(0);

    gBattleSpriteInfo[playerBattler].lowHpSong = 0;
    if (IsDoubleBattle())
        gBattleSpriteInfo[playerBattler ^ 2].lowHpSong = 0;
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
        u8 playerBattler1 = GetBattlerAtPosition(0);
        u8 playerBattler2 = GetBattlerAtPosition(2);
        u8 battler1PartyId = pokemon_order_func(gBattlerPartyIndexes[playerBattler1]);
        u8 battler2PartyId = pokemon_order_func(gBattlerPartyIndexes[playerBattler2]);

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
    u8 battler;

    LoadCompressedObjectPic(&gSpriteSheet_EnemyShadow);
    battler = GetBattlerAtPosition(1);
    gBattleHealthBoxInfo[battler].shadowSpriteId = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(battler, 0), GetBattlerSpriteCoord(battler, 1) + 32, 0xC8);
    gSprites[gBattleHealthBoxInfo[battler].shadowSpriteId].data[0] = battler;
    if (IsDoubleBattle())
    {
        battler = GetBattlerAtPosition(3);
        gBattleHealthBoxInfo[battler].shadowSpriteId = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(battler, 0), GetBattlerSpriteCoord(battler, 1) + 32, 0xC8);
        gSprites[gBattleHealthBoxInfo[battler].shadowSpriteId].data[0] = battler;
    }
}

void SpriteCB_EnemyShadow(struct Sprite *shadowSprite)
{
    bool8 invisible = FALSE;
    u8 battler = shadowSprite->data[0];
    struct Sprite *battlerSprite = &gSprites[gBattlerSpriteIds[battler]];

    if (!battlerSprite->inUse || IsBattlerSpritePresent(battler) == 0)
    {
        shadowSprite->callback = SpriteCB_SetInvisible;
        return;
    }
    if (gAnimScriptActive || battlerSprite->invisible)
        invisible = TRUE;
    else if (gBattleSpriteInfo[battler].transformSpecies != 0 && gEnemyMonElevation[gBattleSpriteInfo[battler].transformSpecies] == 0)
        invisible = TRUE;
    if (gBattleSpriteInfo[battler].behindSubstitute)
        invisible = TRUE;
    shadowSprite->x = battlerSprite->x;
    shadowSprite->x2 = battlerSprite->x2;
    shadowSprite->invisible = invisible;
}

void SpriteCB_SetInvisible(struct Sprite *sprite)
{
    sprite->invisible = TRUE;
}

void SetBattlerShadowSpriteCallback(u8 battler, u16 species)
{
    if (GetBattlerSide(battler) != 0)
    {
        if (gBattleSpriteInfo[battler].transformSpecies != 0)
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
    s32 i;
    s32 j;

    for (i = 0; i < 9; i++)
    {
        for (j = 0; j < 16; j++)
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

void ClearTemporarySpeciesSpriteData(u8 battler, u8 dontClearSubstitute)
{
    gBattleSpriteInfo[battler].transformSpecies = 0;
    gBattleMonForms[battler] = 0;
    if (dontClearSubstitute == 0)
        ClearBehindSubstituteBit(battler);
}
