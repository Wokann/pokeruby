#include "global.h"
#include "constants/rgb.h"
#include "battle.h"
#include "battle_gfx_sfx_util.h"
#include "battle_controllers.h"
#include "battle_main.h"
#include "battle_anim.h"
#include "battle_interface.h"
#include "util.h"
#include "contest.h"
#include "data.h"
#include "decompress.h"
#include "main.h"
#include "m4a.h"
#include "palette.h"
#include "pokemon.h"
#include "party_menu.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "sound.h"
#include "constants/species.h"
#include "sprite.h"
#include "task.h"
#include "text.h"
#include "gba/m4a_internal.h"
#include "ewram.h"
#include "graphics.h"
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern const u8 *const gBattleAnims_General[];
extern const u8 *const gBattleAnims_Special[];
extern const struct CompressedSpriteSheet gSpriteSheet_EnemyShadow;
extern const struct SpriteTemplate gSpriteTemplate_EnemyShadow;

static const struct CompressedSpriteSheet sSpriteSheet_SinglesPlayerHealthbox =
{ gHealthboxSinglesPlayerGfx, 4096, TAG_HEALTHBOX_PLAYER1_TILE };

static const struct CompressedSpriteSheet sSpriteSheet_SinglesOpponentHealthbox =
{ gHealthboxSinglesOpponentGfx, 4096, TAG_HEALTHBOX_OPPONENT1_TILE };

static const struct CompressedSpriteSheet sSpriteSheets_DoublesPlayerHealthbox[] =
{
    [B_FLANK_LEFT] = { gHealthboxDoublesPlayerGfx, 2048, TAG_HEALTHBOX_PLAYER1_TILE },
    [B_FLANK_RIGHT] = { gHealthboxDoublesPlayerGfx, 2048, TAG_HEALTHBOX_PLAYER2_TILE },
};

static const struct CompressedSpriteSheet sSpriteSheets_DoublesOpponentHealthbox[] =
{
    [B_FLANK_LEFT] = { gHealthboxDoublesOpponentGfx, 2048, TAG_HEALTHBOX_OPPONENT1_TILE },
    [B_FLANK_RIGHT] = { gHealthboxDoublesOpponentGfx, 2048, TAG_HEALTHBOX_OPPONENT2_TILE },
};

static const struct CompressedSpriteSheet sSpriteSheet_SafariHealthbox =
{ gHealthboxSafariGfx, 4096, TAG_HEALTHBOX_SAFARI_TILE };

static const struct CompressedSpriteSheet sSpriteSheets_HealthBar[MAX_BATTLERS_COUNT] =
{
    [B_POSITION_PLAYER_LEFT] = { gBlankGfxCompressed, 256, TAG_HEALTHBAR_PLAYER1_TILE },
    [B_POSITION_OPPONENT_LEFT] = { gBlankGfxCompressed, 288, TAG_HEALTHBAR_OPPONENT1_TILE },
    [B_POSITION_PLAYER_RIGHT] = { gBlankGfxCompressed, 256, TAG_HEALTHBAR_PLAYER2_TILE },
    [B_POSITION_OPPONENT_RIGHT] = { gBlankGfxCompressed, 288, TAG_HEALTHBAR_OPPONENT2_TILE },
};

static const struct SpritePalette sSpritePalettes_HealthBoxHealthBar[] =
{
    { gBattleInterface_BallStatusBarPal, TAG_HEALTHBOX_PAL },
    { gBattleInterface_BallDisplayPal, TAG_HEALTHBAR_PAL },
};

static void Task_ClearBitWhenBattleTableAnimDone(u8);
static bool8 ShouldAnimBeDoneRegardlessOfSubstitute(u8 animId);
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

#define sSpeedX data[0]

void SpriteCB_TrainerSlideIn(struct Sprite *sprite)
{
    if (!(gIntroSlideFlags & 1))
    {
        sprite->x2 += sprite->sSpeedX;
        if (sprite->x2 == 0)
            sprite->callback = SpriteCallbackDummy;
    }
}

#undef sSpeedX

void InitAndLaunchChosenStatusAnimation(bool8 isStatus2, u32 status)
{
    gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = TRUE;
    if (isStatus2 == FALSE)
    {
        if (status == STATUS1_FREEZE)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_FRZ);
        else if (status == STATUS1_POISON || (status & STATUS1_TOXIC_POISON))
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_PSN);
        else if (status == STATUS1_BURN)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_BRN);
        else if (status & STATUS1_SLEEP)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_SLP);
        else if (status == STATUS1_PARALYSIS)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_PRZ);
        else
            gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = FALSE;
    }
    else
    {
        if (status & STATUS2_INFATUATION)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_INFATUATION);
        else if (status & STATUS2_CONFUSION)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_CONFUSION);
        else if (status & STATUS2_CURSED)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_CURSED);
        else if (status & STATUS2_NIGHTMARE)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_NIGHTMARE);
        else if (status & STATUS2_WRAPPED)
            LaunchStatusAnimation(gActiveBattler, B_ANIM_STATUS_WRAPPED);
        else
            gBattleHealthBoxInfo[gActiveBattler].statusAnimActive = FALSE;
    }
}

#define tBattlerId data[0]

bool8 TryHandleLaunchBattleTableAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 tableId, u16 argument)
{
    u8 taskId;

    if (tableId == B_ANIM_CASTFORM_CHANGE && (argument & CASTFORM_SUBSTITUTE))
    {
        gBattleMonForms[activeBattler] = argument & (u8)~CASTFORM_SUBSTITUTE;
        return TRUE;
    }
    if (gBattleSpriteInfo[activeBattler].behindSubstitute && ShouldAnimBeDoneRegardlessOfSubstitute(tableId) == FALSE)
        return TRUE;
    if (gBattleSpriteInfo[activeBattler].behindSubstitute && tableId == B_ANIM_SUBSTITUTE_FADE && gSprites[gBattlerSpriteIds[activeBattler]].invisible)
    {
        LoadBattleMonGfxAndAnimate(activeBattler, TRUE, gBattlerSpriteIds[activeBattler]);
        ClearBehindSubstituteBit(activeBattler);
        return TRUE;
    }
    gBattleAnimAttacker = atkBattler;
    gBattleAnimTarget = defBattler;
    gBattleAnimationInfo->animArg = argument;
    LaunchBattleAnimation(gBattleAnims_General, tableId, FALSE);
    taskId = CreateTask(Task_ClearBitWhenBattleTableAnimDone, 10);
    gTasks[taskId].tBattlerId = activeBattler;
    gBattleHealthBoxInfo[gTasks[taskId].tBattlerId].animFromTableActive = TRUE;
    return FALSE;
}

static void Task_ClearBitWhenBattleTableAnimDone(u8 taskId)
{
    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        gBattleHealthBoxInfo[gTasks[taskId].tBattlerId].animFromTableActive = FALSE;
        DestroyTask(taskId);
    }
}

#undef tBattlerId

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

#define tBattlerId data[0]

void InitAndLaunchSpecialAnimation(u8 activeBattler, u8 atkBattler, u8 defBattler, u8 tableId)
{
    u8 taskId;

    gBattleAnimAttacker = atkBattler;
    gBattleAnimTarget = defBattler;
    LaunchBattleAnimation(gBattleAnims_Special, tableId, FALSE);
    taskId = CreateTask(Task_ClearBitWhenSpecialAnimDone, 10);
    gTasks[taskId].tBattlerId = activeBattler;
    gBattleHealthBoxInfo[gTasks[taskId].tBattlerId].specialAnimActive = TRUE;
}

static void Task_ClearBitWhenSpecialAnimDone(u8 taskId)
{
    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        gBattleHealthBoxInfo[gTasks[taskId].tBattlerId].specialAnimActive = FALSE;
        DestroyTask(taskId);
    }
}

#undef tBattlerId

bool8 IsMoveWithoutAnimation(int move, int animationTurn)
{
    return FALSE;
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
    if (gBattleSpriteInfo[battler].transformSpecies == SPECIES_NONE)
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
      gMonFrontPicCoords[species].size,
      gMonFrontPicCoords[species].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
      species,
      currentPersonality);
    paletteOffset = OBJ_PLTT_ID(battler);
    if (gBattleSpriteInfo[battler].transformSpecies == SPECIES_NONE)
        lzPaletteData = GetMonFrontSpritePal(mon);
    else
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, otId, monsPersonality);
    LZDecompressWram(lzPaletteData, gSharedMem);
    LoadPalette(gSharedMem, paletteOffset, PLTT_SIZE_4BPP);
    LoadPalette(gSharedMem, BG_PLTT_ID(8) + BG_PLTT_ID(battler), PLTT_SIZE_4BPP);
    if (species == SPECIES_CASTFORM)
    {
        paletteOffset = OBJ_PLTT_ID(battler);
        LZDecompressWram(lzPaletteData, ewram16400);
        LoadPalette(ewram16400 + gBattleMonForms[battler] * PLTT_SIZE_4BPP, paletteOffset, PLTT_SIZE_4BPP);
    }
    if (gBattleSpriteInfo[battler].transformSpecies != SPECIES_NONE)
    {
        BlendPalette(paletteOffset, 16, 6, RGB_WHITE);
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, PLTT_SIZEOF(16));
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
    if (gBattleSpriteInfo[battler].transformSpecies == SPECIES_NONE)
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
      gMonBackPicCoords[species].size,
      gMonBackPicCoords[species].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
      species,
      currentPersonality);
    paletteOffset = OBJ_PLTT_ID(battler);
    if (gBattleSpriteInfo[battler].transformSpecies == SPECIES_NONE)
        lzPaletteData = GetMonFrontSpritePal(mon);
    else
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, otId, monsPersonality);
    LZDecompressWram(lzPaletteData, gSharedMem);
    LoadPalette(gSharedMem, paletteOffset, PLTT_SIZE_4BPP);
    LoadPalette(gSharedMem, BG_PLTT_ID(8) + BG_PLTT_ID(battler), PLTT_SIZE_4BPP);
    if (species == SPECIES_CASTFORM)
    {
        paletteOffset = OBJ_PLTT_ID(battler);
        LZDecompressWram(lzPaletteData, ewram16400);
        LoadPalette(ewram16400 + gBattleMonForms[battler] * PLTT_SIZE_4BPP, paletteOffset, PLTT_SIZE_4BPP);
    }
    if (gBattleSpriteInfo[battler].transformSpecies != SPECIES_NONE)
    {
        BlendPalette(paletteOffset, 16, 6, RGB_WHITE);
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, PLTT_SIZEOF(16));
    }
}

static void UNUSED BattleGfxSfxDummy1(void)
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
    DecompressPicFromTable_2(
      &gTrainerFrontPicTable[frontPicId],
      gTrainerFrontPicCoords[frontPicId].size,
      gTrainerFrontPicCoords[frontPicId].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
      SPECIES_NONE);
    spriteSheet.data = gMonSpriteGfx_Sprite_ptr[position];
    spriteSheet.size = gTrainerFrontPicTable[frontPicId].size;
    spriteSheet.tag = gTrainerFrontPicTable[frontPicId].tag;
    LoadCompressedSpriteSheet(&spriteSheet);
    LoadCompressedSpritePalette(&gTrainerFrontPicPaletteTable[frontPicId]);
}

void DecompressTrainerBackPic(u16 backPicId, u8 battler)
{
    u8 position;

    position = GetBattlerPosition(battler);
    DecompressPicFromTable_2(
      &gTrainerBackPicTable[backPicId],
      gTrainerBackPicCoords[backPicId].size,
      gTrainerBackPicCoords[backPicId].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfx_Sprite_ptr[position],
      SPECIES_NONE);
    LoadCompressedPalette(gTrainerBackPicPaletteTable[backPicId].data, OBJ_PLTT_ID(battler), PLTT_SIZE_4BPP);
}

void BattleGfxSfxDummy3(int gender)
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
        LoadCompressedSpriteSheet(&sSpriteSheet_SinglesPlayerHealthbox);
        LoadCompressedSpriteSheet(&sSpriteSheet_SinglesOpponentHealthbox);
        numberOfBattlers = NUM_BATTLE_SIDES;
    }
    else
    {
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[B_FLANK_LEFT]);
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[B_FLANK_RIGHT]);
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[B_FLANK_LEFT]);
        LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[B_FLANK_RIGHT]);
        numberOfBattlers = MAX_BATTLERS_COUNT;
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
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[B_BATTLER_0]]);
            else if (state == 5)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[B_BATTLER_1]]);
            else
                retVal = TRUE;
        }
        else
        {
            if (state == 2)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[B_FLANK_LEFT]);
            else if (state == 3)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesPlayerHealthbox[B_FLANK_RIGHT]);
            else if (state == 4)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[B_FLANK_LEFT]);
            else if (state == 5)
                LoadCompressedSpriteSheet(&sSpriteSheets_DoublesOpponentHealthbox[B_FLANK_RIGHT]);
            else if (state == 6)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[B_BATTLER_0]]);
            else if (state == 7)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[B_BATTLER_1]]);
            else if (state == 8)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[B_BATTLER_2]]);
            else if (state == 9)
                LoadCompressedSpriteSheet(&sSpriteSheets_HealthBar[gBattlerPositions[B_BATTLER_3]]);
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

bool8 BattleInitAllSprites(u8 *state1, u8 *battler)
{
    bool8 retVal = FALSE;

    switch (*state1)
    {
    case 0:
        ClearSpritesBattlerHealthboxAnimData();
        (*state1)++;
        break;
    case 1:
        if (BattleLoadAllHealthBoxesGfx(*battler) == FALSE)
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
        if ((gBattleTypeFlags & BATTLE_TYPE_SAFARI) && *battler == 0)
            gHealthboxSpriteIds[*battler] = CreateSafariPlayerHealthboxSprites();
        else
            gHealthboxSpriteIds[*battler] = CreateBattlerHealthboxSprites(*battler);
        (*battler)++;
        if (*battler == gBattlersCount)
        {
            *battler = 0;
            (*state1)++;
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
            (*state1)++;
        }
        break;
    case 5:
        if (GetBattlerSide(*battler) == B_SIDE_PLAYER)
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
                UpdateHealthboxAttribute(gHealthboxSpriteIds[*battler], &gPlayerParty[gBattlerPartyIndexes[*battler]], HEALTHBOX_ALL);
        }
        else
        {
            UpdateHealthboxAttribute(gHealthboxSpriteIds[*battler], &gEnemyParty[gBattlerPartyIndexes[*battler]], HEALTHBOX_ALL);
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
        BufferBattlePartyCurrentOrder();
        retVal = TRUE;
        break;
    }
    return retVal;
}

void ClearSpritesHealthboxAnimData(void)
{
    memset(gBattleHealthBoxInfo, 0, sizeof(struct BattleHealthboxInfo) * MAX_BATTLERS_COUNT);
    memset(gBattleAnimationInfo, 0, sizeof(struct BattleAnimationInfo));
}

static void ClearSpritesBattlerHealthboxAnimData(void)
{
    ClearSpritesHealthboxAnimData();
    memset(gBattleSpriteInfo, 0, sizeof(struct BattleSpriteInfo) * MAX_BATTLERS_COUNT);
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
        paletteOffset = OBJ_PLTT_ID(battlerAtk);
        LoadPalette(ewram16400 + gBattleAnimationInfo->animArg * PLTT_SIZE_4BPP, paletteOffset, PLTT_SIZE_4BPP);
        gBattleMonForms[battlerAtk] = gBattleAnimationInfo->animArg;
        if (gBattleSpriteInfo[battlerAtk].transformSpecies != SPECIES_NONE)
        {
            BlendPalette(paletteOffset, 16, 6, RGB_WHITE);
            CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, PLTT_SIZEOF(16));
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
              gMonBackPicCoords[targetSpecies].size,
              gMonBackPicCoords[targetSpecies].y_offset,
              eBattleInterfaceGfxBuffer,
              gMonSpriteGfx_Sprite_ptr[B_POSITION_PLAYER_LEFT],
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
                  gMonBackPicCoords[targetSpecies].size,
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
                  gMonFrontPicCoords[targetSpecies].size,
                  gMonFrontPicCoords[targetSpecies].y_offset,
                  eBattleInterfaceGfxBuffer,
                  gMonSpriteGfx_Sprite_ptr[position],
                  targetSpecies,
                  gTransformedPersonalities[battlerAtk]);
            }
        }
        DmaCopy32Defvars(3, gMonSpriteGfx_Sprite_ptr[position], (void *)(OBJ_VRAM0 + gSprites[gBattlerSpriteIds[battlerAtk]].oam.tileNum * TILE_SIZE_4BPP), MON_PIC_SIZE);
        paletteOffset = OBJ_PLTT_ID(battlerAtk);
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(targetSpecies, otId, personalityValue);
        LZDecompressWram(lzPaletteData, gSharedMem);
        LoadPalette(gSharedMem, paletteOffset, PLTT_SIZE_4BPP);
        if (targetSpecies == SPECIES_CASTFORM)
        {
            u16 *paletteSrc = (u16 *)ewram16400; // TODO: avoid casting?

            LZDecompressWram(lzPaletteData, paletteSrc);
            LoadPalette(paletteSrc + PLTT_ID(gBattleMonForms[battlerDef]), paletteOffset, PLTT_SIZE_4BPP);
        }
        BlendPalette(paletteOffset, 16, 6, RGB_WHITE);
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, PLTT_SIZEOF(16));
        if (!IsContest())
        {
            gBattleSpriteInfo[battlerAtk].transformSpecies = targetSpecies;
            gBattleMonForms[battlerAtk] = gBattleMonForms[battlerDef];
        }
        gSprites[gBattlerSpriteIds[battlerAtk]].y = GetBattlerSpriteDefault_Y(battlerAtk);
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[battlerAtk]], gBattleMonForms[battlerAtk]);
    }
}

void BattleLoadSubstituteOrMonSpriteGfx(u8 battler, bool8 loadMonSprite)
{
    u8 position;
    u16 palOffset;
    const u8 *gSubstituteDollPal_;
    void *src;
    s32 i;

    if (loadMonSprite == FALSE)
    {
        if (IsContest())
            position = B_POSITION_PLAYER_LEFT;
        else
            position = GetBattlerPosition(battler);
        if (IsContest())
            LZDecompressVram(gSubstituteDollBackGfx, gMonSpriteGfx_Sprite_ptr[position]);
        else if (GetBattlerSide(battler) != B_SIDE_PLAYER)
            LZDecompressVram(gSubstituteDollFrontGfx, gMonSpriteGfx_Sprite_ptr[position]);
        else
            LZDecompressVram(gSubstituteDollBackGfx, gMonSpriteGfx_Sprite_ptr[position]);
        // There is probably a way to do this without all the temp variables, but I couldn't figure it out.
        palOffset = PLTT_ID(battler);
        gSubstituteDollPal_ = gSubstituteDollPal;
        src = gMonSpriteGfx_Sprite_ptr[position];
        for (i = 0; i < MAX_MON_PIC_FRAMES - 1; i++)
            DmaCopy32(3, src, src + i * MON_PIC_SIZE + MON_PIC_SIZE, MON_PIC_SIZE);
        LoadCompressedPalette(gSubstituteDollPal_, OBJ_PLTT_OFFSET + palOffset, PLTT_SIZE_4BPP);
    }
    else
    {
        if (!IsContest())
        {
            if (GetBattlerSide(battler) != B_SIDE_PLAYER)
                BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[battler]], battler);
            else
                BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[battler]], battler);
        }
    }
}

void LoadBattleMonGfxAndAnimate(u8 battler, bool8 loadMonSprite, u8 spriteId)
{
    BattleLoadSubstituteOrMonSpriteGfx(battler, loadMonSprite);
    StartSpriteAnim(&gSprites[spriteId], gBattleMonForms[battler]);
    if (loadMonSprite == FALSE)
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

    if (GetHPBarLevel(hp, maxHP) == HP_BAR_RED)
    {
        if (!gBattleSpriteInfo[battler].lowHpSong)
        {
            if (!gBattleSpriteInfo[BATTLE_PARTNER(battler)].lowHpSong)
                PlaySE(SE_LOW_HEALTH);
            gBattleSpriteInfo[battler].lowHpSong = TRUE;
        }
    }
    else
    {
        gBattleSpriteInfo[battler].lowHpSong = FALSE;
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

    gBattleSpriteInfo[playerBattler].lowHpSong = FALSE;
    if (IsDoubleBattle())
        gBattleSpriteInfo[BATTLE_PARTNER(playerBattler)].lowHpSong = FALSE;
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
        if (IsBattlerSpritePresent(i) != FALSE)
        {
            gSprites[gBattlerSpriteIds[i]].oam.affineMode = affineMode;
            if (affineMode == ST_OAM_AFFINE_OFF)
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

#define tBattlerId data[0]

void LoadAndCreateEnemyShadowSprites(void)
{
    u8 battler;

    LoadCompressedSpriteSheet(&gSpriteSheet_EnemyShadow);
    battler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
    gBattleHealthBoxInfo[battler].shadowSpriteId = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(battler, BATTLER_COORD_X), GetBattlerSpriteCoord(battler, BATTLER_COORD_Y) + 32, 0xC8);
    gSprites[gBattleHealthBoxInfo[battler].shadowSpriteId].tBattlerId = battler;
    if (IsDoubleBattle())
    {
        battler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        gBattleHealthBoxInfo[battler].shadowSpriteId = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(battler, BATTLER_COORD_X), GetBattlerSpriteCoord(battler, BATTLER_COORD_Y) + 32, 0xC8);
        gSprites[gBattleHealthBoxInfo[battler].shadowSpriteId].tBattlerId = battler;
    }
}

void SpriteCB_EnemyShadow(struct Sprite *shadowSprite)
{
    bool8 invisible = FALSE;
    u8 battler = shadowSprite->tBattlerId;
    struct Sprite *battlerSprite = &gSprites[gBattlerSpriteIds[battler]];

    if (!battlerSprite->inUse || IsBattlerSpritePresent(battler) == FALSE)
    {
        shadowSprite->callback = SpriteCB_SetInvisible;
        return;
    }
    if (gAnimScriptActive || battlerSprite->invisible)
        invisible = TRUE;
    else if (gBattleSpriteInfo[battler].transformSpecies != SPECIES_NONE && gEnemyMonElevation[gBattleSpriteInfo[battler].transformSpecies] == 0)
        invisible = TRUE;
    if (gBattleSpriteInfo[battler].behindSubstitute)
        invisible = TRUE;
    shadowSprite->x = battlerSprite->x;
    shadowSprite->x2 = battlerSprite->x2;
    shadowSprite->invisible = invisible;
}

#undef tBattlerId

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

void ClearTemporarySpeciesSpriteData(u8 battler, bool8 dontClearSubstitute)
{
    gBattleSpriteInfo[battler].transformSpecies = SPECIES_NONE;
    gBattleMonForms[battler] = 0;
    if (dontClearSubstitute == FALSE)
        ClearBehindSubstituteBit(battler);
}
