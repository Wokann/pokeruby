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
extern const u8 gUnknown_08D09C48[]; // graphics.s

const struct CompressedSpriteSheet gUnknown_0820A47C =
{ gBattleWindowLargeGfx, 4096, 0xd6ff };

const struct CompressedSpriteSheet gUnknown_0820A484 =
{ gBattleWindowSmallGfx, 4096, 0xd701 };

const struct CompressedSpriteSheet gUnknown_0820A48C[] =
{
    { gBattleWindowSmall2Gfx, 2048, 0xd6ff },
    { gBattleWindowSmall2Gfx, 2048, 0xd700 },
};

const struct CompressedSpriteSheet gUnknown_0820A49C[] =
{
    { gBattleWindowSmall3Gfx, 2048, 0xd701 },
    { gBattleWindowSmall3Gfx, 2048, 0xd702 },
};

const struct CompressedSpriteSheet gUnknown_0820A4AC =
{ gBattleWindowLarge2Gfx, 4096, 0xd70b };

const struct CompressedSpriteSheet gUnknown_0820A4B4[] =
{
    { gBlankGfxCompressed, 256, 0xd704 },
    { gBlankGfxCompressed, 288, 0xd705 },
    { gBlankGfxCompressed, 256, 0xd706 },
    { gBlankGfxCompressed, 288, 0xd707 },
};

const struct SpritePalette gUnknown_0820A4D4[] =
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

void sub_80315E8(u8);
u8 sub_803163C(u8);
void sub_80316CC(u8);
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

void unref_sub_8031364(struct Sprite *sprite, bool8 stupid)
{
    sprite->animPaused = TRUE;
    sprite->callback = SpriteCallbackDummy;
    if (!stupid)
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

bool8 TryHandleLaunchBattleTableAnimation(u8 a, u8 b, u8 c, u8 d, u16 e)
{
    u8 taskId;

    if (d == 0 && (e & 0x80))
    {
        gBattleMonForms[a] = e & 0x7F;
        return TRUE;
    }
    if (gBattleSpriteInfo[a].behindSubstitute && sub_803163C(d) == 0)
        return TRUE;
    if (gBattleSpriteInfo[a].behindSubstitute && d == 2 && gSprites[gBattlerSpriteIds[a]].invisible)
    {
        LoadBattleMonGfxAndAnimate(a, TRUE, gBattlerSpriteIds[a]);
        ClearBehindSubstituteBit(a);
        return TRUE;
    }
    gBattleAnimAttacker = b;
    gBattleAnimTarget = c;
    gBattleAnimationInfo.animArg = e;
    LaunchBattleAnimation(gBattleAnims_General, d, 0);
    taskId = CreateTask(sub_80315E8, 10);
    gTasks[taskId].data[0] = a;
    gBattleHealthBoxInfo[gTasks[taskId].data[0]].animFromTableActive = 1;
    return FALSE;
}

void sub_80315E8(u8 taskId)
{
    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        gBattleHealthBoxInfo[gTasks[taskId].data[0]].animFromTableActive = 0;
        DestroyTask(taskId);
    }
}

u8 sub_803163C(u8 a)
{
    switch (a)
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

void InitAndLaunchSpecialAnimation(u8 a, u8 b, u8 c, u8 d)
{
    u8 taskId;

    gBattleAnimAttacker = b;
    gBattleAnimTarget = c;
    LaunchBattleAnimation(gBattleAnims_Special, d, 0);
    taskId = CreateTask(sub_80316CC, 10);
    gTasks[taskId].data[0] = a;
    gBattleHealthBoxInfo[gTasks[taskId].data[0]].specialAnimActive = 1;
}

void sub_80316CC(u8 taskId)
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
        gBattleHealthBoxInfo[battler].unk8++;
        if (gBattleHealthBoxInfo[gActiveBattler].unk8 < 30)
            return TRUE;
        m4aMPlayStop(&gMPlayInfo_SE1);
        m4aMPlayStop(&gMPlayInfo_SE2);
    }
    if (zero == 0)
    {
        gBattleHealthBoxInfo[battler].unk8 = 0;
        return FALSE;
    }
    return TRUE;
}

void BattleLoadOpponentMonSpriteGfx(struct Pokemon *pkmn, u8 b)
{
    u32 personalityValue;
    u16 species;
    u32 r7;
    u32 otId;
    u8 var;
    u16 paletteOffset;
    const u8 *lzPaletteData;

    personalityValue = GetMonData(pkmn, MON_DATA_PERSONALITY);
    if (gBattleSpriteInfo[b].transformSpecies == 0)
    {
        species = GetMonData(pkmn, MON_DATA_SPECIES);
        r7 = personalityValue;
    }
    else
    {
        species = gBattleSpriteInfo[b].transformSpecies;
        r7 = gTransformedPersonalities[b];
    }
    otId = GetMonData(pkmn, MON_DATA_OT_ID);
    var = GetBattlerPosition(b);
    HandleLoadSpecialPokePic(
      &gMonFrontPicTable[species],
      gMonFrontPicCoords[species].coords,
      gMonFrontPicCoords[species].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[var],
      species,
      r7);
    paletteOffset = 0x100 + b * 16;
    if (gBattleSpriteInfo[b].transformSpecies == 0)
        lzPaletteData = GetMonSpritePal(pkmn);
    else
        lzPaletteData = GetMonSpritePalFromOtIdPersonality(species, otId, personalityValue);
    LZDecompressWram(lzPaletteData, gSharedMem);
    LoadPalette(gSharedMem, paletteOffset, 0x20);
    LoadPalette(gSharedMem, 0x80 + b * 16, 0x20);
    if (species == SPECIES_CASTFORM)
    {
        paletteOffset = 0x100 + b * 16;
        LZDecompressWram(lzPaletteData, ewram16400);
        LoadPalette(ewram16400 + gBattleMonForms[b] * 32, paletteOffset, 0x20);
    }
    if (gBattleSpriteInfo[b].transformSpecies != 0)
    {
        BlendPalette(paletteOffset, 16, 6, RGB(31, 31, 31));
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
    }
}

void BattleLoadPlayerMonSpriteGfx(struct Pokemon *pkmn, u8 b)
{
    u32 personalityValue;
    u16 species;
    u32 r7;
    u32 otId;
    u8 var;
    u16 paletteOffset;
    const u8 *lzPaletteData;

    personalityValue = GetMonData(pkmn, MON_DATA_PERSONALITY);
    if (gBattleSpriteInfo[b].transformSpecies == 0)
    {
        species = GetMonData(pkmn, MON_DATA_SPECIES);
        r7 = personalityValue;
    }
    else
    {
        species = gBattleSpriteInfo[b].transformSpecies;
        r7 = gTransformedPersonalities[b];
    }
    otId = GetMonData(pkmn, MON_DATA_OT_ID);
    var = GetBattlerPosition(b);
    HandleLoadSpecialPokePic(
      &gMonBackPicTable[species],
      gMonBackPicCoords[species].coords,
      gMonBackPicCoords[species].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[var],
      species,
      r7);
    paletteOffset = 0x100 + b * 16;
    if (gBattleSpriteInfo[b].transformSpecies == 0)
        lzPaletteData = GetMonSpritePal(pkmn);
    else
        lzPaletteData = GetMonSpritePalFromOtIdPersonality(species, otId, personalityValue);
    LZDecompressWram(lzPaletteData, gSharedMem);
    LoadPalette(gSharedMem, paletteOffset, 0x20);
    LoadPalette(gSharedMem, 0x80 + b * 16, 0x20);
    if (species == SPECIES_CASTFORM)
    {
        paletteOffset = 0x100 + b * 16;
        LZDecompressWram(lzPaletteData, ewram16400);
        LoadPalette(ewram16400 + gBattleMonForms[b] * 32, paletteOffset, 0x20);
    }
    if (gBattleSpriteInfo[b].transformSpecies != 0)
    {
        BlendPalette(paletteOffset, 16, 6, RGB(31, 31, 31));
        CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
    }
}

void unref_sub_8031A64(void)
{
}

void BattleGfxSfxDummy2(u16 species)
{
}

void sub_8031A6C(u16 a, u8 b)
{
    u8 status;
    struct CompressedSpriteSheet spriteSheet;

    status = GetBattlerPosition(b);
    DecompressPicFromTable(
      &gTrainerFrontPicTable[a],
      gTrainerFrontPicCoords[a].coords,
      gTrainerFrontPicCoords[a].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[status],
      0);
    spriteSheet.data = gMonSpriteGfxBufferPtrs[status];
    spriteSheet.size = gTrainerFrontPicTable[a].size;
    spriteSheet.tag = gTrainerFrontPicTable[a].tag;
    LoadCompressedSpriteSheet(&spriteSheet);
    LoadCompressedSpritePalette(&gTrainerFrontPicPaletteTable[a]);
}

void DecompressTrainerBackPic(u16 a, u8 b)
{
    u8 status;

    status = GetBattlerPosition(b);
    DecompressPicFromTable(
      &gTrainerBackPicTable[a],
      gTrainerBackPicCoords[a].coords,
      gTrainerBackPicCoords[a].y_offset,
      eBattleInterfaceGfxBuffer,
      gMonSpriteGfxBufferPtrs[status],
      0);
    LoadCompressedPalette(gTrainerBackPicPaletteTable[a].data, 0x100 + b * 16, 32);
}

void BattleGfxSfxDummy3(u8 gender)
{
}

void sub_8031B74(u16 a)
{
    FreeSpritePaletteByTag(gTrainerFrontPicPaletteTable[a].tag);
    FreeSpriteTilesByTag(gTrainerFrontPicTable[a].tag);
}

void unref_sub_8031BA0(void)
{
    u8 count;
    u8 i;

    LoadSpritePalette(&gUnknown_0820A4D4[0]);
    LoadSpritePalette(&gUnknown_0820A4D4[1]);
    if (!IsDoubleBattle())
    {
        LoadCompressedSpriteSheet(&gUnknown_0820A47C);
        LoadCompressedSpriteSheet(&gUnknown_0820A484);
        count = 2;
    }
    else
    {
        LoadCompressedSpriteSheet(&gUnknown_0820A48C[0]);
        LoadCompressedSpriteSheet(&gUnknown_0820A48C[1]);
        LoadCompressedSpriteSheet(&gUnknown_0820A49C[0]);
        LoadCompressedSpriteSheet(&gUnknown_0820A49C[1]);
        count = 4;
    }
    for (i = 0; i < count; i++)
        LoadCompressedSpriteSheet(&gUnknown_0820A4B4[gBattlerPositions[i]]);
}

bool8 BattleLoadAllHealthBoxesGfx(u8 a)
{
    bool8 retVal = FALSE;

    if (a != 0)
    {
        if (a == 1)
        {
            LoadSpritePalette(&gUnknown_0820A4D4[0]);
            LoadSpritePalette(&gUnknown_0820A4D4[1]);
        }
        else if (!IsDoubleBattle())
        {
            if (a == 2)
            {
                if (gBattleTypeFlags & 0x80)
                    LoadCompressedSpriteSheet(&gUnknown_0820A4AC);
                else
                    LoadCompressedSpriteSheet(&gUnknown_0820A47C);
            }
            else if (a == 3)
                LoadCompressedSpriteSheet(&gUnknown_0820A484);
            else if (a == 4)
                LoadCompressedSpriteSheet(&gUnknown_0820A4B4[gBattlerPositions[0]]);
            else if (a == 5)
                LoadCompressedSpriteSheet(&gUnknown_0820A4B4[gBattlerPositions[1]]);
            else
                retVal = TRUE;
        }
        else
        {
            if (a == 2)
                LoadCompressedSpriteSheet(&gUnknown_0820A48C[0]);
            else if (a == 3)
                LoadCompressedSpriteSheet(&gUnknown_0820A48C[1]);
            else if (a == 4)
                LoadCompressedSpriteSheet(&gUnknown_0820A49C[0]);
            else if (a == 5)
                LoadCompressedSpriteSheet(&gUnknown_0820A49C[1]);
            else if (a == 6)
                LoadCompressedSpriteSheet(&gUnknown_0820A4B4[gBattlerPositions[0]]);
            else if (a == 7)
                LoadCompressedSpriteSheet(&gUnknown_0820A4B4[gBattlerPositions[1]]);
            else if (a == 8)
                LoadCompressedSpriteSheet(&gUnknown_0820A4B4[gBattlerPositions[2]]);
            else if (a == 9)
                LoadCompressedSpriteSheet(&gUnknown_0820A4B4[gBattlerPositions[3]]);
            else
                retVal = TRUE;
        }
    }
    return retVal;
}

void LoadBattleBarGfx(u8 a)
{
    LZDecompressWram(gUnknown_08D09C48, eBattleInterfaceGfxBuffer);
}

bool8 BattleInitAllSprites(u8 *pState, u8 *b)
{
    bool8 retVal = FALSE;

    switch (*pState)
    {
    case 0:
        ClearSpritesBattlerHealthboxAnimData();
        (*pState)++;
        break;
    case 1:
        if (BattleLoadAllHealthBoxesGfx(*b) == 0)
        {
            (*b)++;
        }
        else
        {
            *b = 0;
            (*pState)++;
        }
        break;
    case 2:
        (*pState)++;
        break;
    case 3:
        if ((gBattleTypeFlags & 0x80) && *b == 0)
            gHealthboxSpriteIds[*b] = CreateSafariPlayerHealthboxSprites();
        else
            gHealthboxSpriteIds[*b] = CreateBattlerHealthboxSprites(*b);
        (*b)++;
        if (*b == gBattlersCount)
        {
            *b = 0;
            (*pState)++;
        }
        break;
    case 4:
        InitBattlerHealthboxCoords(*b);
        if (gBattlerPositions[*b] <= 1)
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[*b], FALSE);
        else
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[*b], TRUE);
        (*b)++;
        if (*b == gBattlersCount)
        {
            *b = 0;
            (*pState)++;
        }
        break;
    case 5:
        if (GetBattlerSide(*b) == 0)
        {
            if (!(gBattleTypeFlags & 0x80))
                UpdateHealthboxAttribute(gHealthboxSpriteIds[*b], &gPlayerParty[gBattlerPartyIndexes[*b]], 0);
        }
        else
        {
            UpdateHealthboxAttribute(gHealthboxSpriteIds[*b], &gEnemyParty[gBattlerPartyIndexes[*b]], 0);
        }
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[*b]);
        (*b)++;
        if (*b == gBattlersCount)
        {
            *b = 0;
            (*pState)++;
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
        LoadPalette(ewram16400 + gBattleAnimationInfo.animArg * 32, paletteOffset, 32);
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
              eBattleInterfaceGfxBuffer,
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
                  eBattleInterfaceGfxBuffer,
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
                  eBattleInterfaceGfxBuffer,
                  gMonSpriteGfxBufferPtrs[position],
                  targetSpecies,
                  gTransformedPersonalities[battlerAtk]);
            }
        }
        DmaCopy32Defvars(3, gMonSpriteGfxBufferPtrs[position], (void *)(VRAM + 0x10000 + gSprites[gBattlerSpriteIds[battlerAtk]].oam.tileNum * 32), 0x800);
        paletteOffset = 0x100 + battlerAtk * 16;
        lzPaletteData = GetMonSpritePalFromOtIdPersonality(targetSpecies, otId, personalityValue);
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

void HandleLowHpMusicChange(struct Pokemon *pkmn, u8 b)
{
    u16 hp = GetMonData(pkmn, MON_DATA_HP);
    u16 maxHP = GetMonData(pkmn, MON_DATA_MAX_HP);

    if (GetHPBarLevel(hp, maxHP) == 1)
    {
        if (!gBattleSpriteInfo[b].lowHpSong)
        {
            if (!gBattleSpriteInfo[b ^ 2].lowHpSong)
                PlaySE(SE_LOW_HEALTH);
            gBattleSpriteInfo[b].lowHpSong = 1;
        }
    }
    else
    {
        gBattleSpriteInfo[b].lowHpSong = 0;
        if (!IsDoubleBattle())
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            return;
        }
        if (IsDoubleBattle() && !gBattleSpriteInfo[b ^ 2].lowHpSong)
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            return;
        }
    }
}

void BattleStopLowHpSound(void)
{
    u8 r4 = GetBattlerAtPosition(0);

    gBattleSpriteInfo[r4].lowHpSong = 0;
    if (IsDoubleBattle())
        gBattleSpriteInfo[r4 ^ 2].lowHpSong = 0;
    m4aSongNumStop(SE_LOW_HEALTH);
}

u8 unref_sub_8032604(struct Pokemon *pkmn)
{
    u16 hp = GetMonData(pkmn, MON_DATA_HP);
    u16 maxHP = GetMonData(pkmn, MON_DATA_MAX_HP);

    return GetHPBarLevel(hp, maxHP);
}

void sub_8032638(void)
{
    if (gMain.inBattle)
    {
        u8 r8 = GetBattlerAtPosition(0);
        u8 r9 = GetBattlerAtPosition(2);
        u8 r4 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[r8]);
        u8 r5 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[r9]);

        if (GetMonData(&gPlayerParty[r4], MON_DATA_HP) != 0)
            HandleLowHpMusicChange(&gPlayerParty[r4], r8);
        if (IsDoubleBattle())
        {
            if (GetMonData(&gPlayerParty[r5], MON_DATA_HP) != 0)
                HandleLowHpMusicChange(&gPlayerParty[r5], r9);
        }
    }
}

void SetBattlerSpriteAffineMode(u8 a)
{
    s32 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (IsBattlerSpritePresent(i) != 0)
        {
            gSprites[gBattlerSpriteIds[i]].oam.affineMode = a;
            if (a == 0)
            {
                gBattleHealthBoxInfo[i].unk6 = gSprites[gBattlerSpriteIds[i]].oam.matrixNum;
                gSprites[gBattlerSpriteIds[i]].oam.matrixNum = 0;
            }
            else
            {
                gSprites[gBattlerSpriteIds[i]].oam.matrixNum = gBattleHealthBoxInfo[i].unk6;
            }
        }
    }
}

void LoadAndCreateEnemyShadowSprites(void)
{
    u8 r5;

    LoadCompressedSpriteSheet(&gSpriteSheet_EnemyShadow);
    r5 = GetBattlerAtPosition(1);
    gBattleHealthBoxInfo[r5].unk7 = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(r5, 0), GetBattlerSpriteCoord(r5, 1) + 32, 0xC8);
    gSprites[gBattleHealthBoxInfo[r5].unk7].data[0] = r5;
    if (IsDoubleBattle())
    {
        r5 = GetBattlerAtPosition(3);
        gBattleHealthBoxInfo[r5].unk7 = CreateSprite(&gSpriteTemplate_EnemyShadow, GetBattlerSpriteCoord(r5, 0), GetBattlerSpriteCoord(r5, 1) + 32, 0xC8);
        gSprites[gBattleHealthBoxInfo[r5].unk7].data[0] = r5;
    }
}

void SpriteCB_EnemyShadow(struct Sprite *sprite)
{
    bool8 invisible = FALSE;
    u8 r4 = sprite->data[0];
    struct Sprite *r7 = &gSprites[gBattlerSpriteIds[r4]];

    if (!r7->inUse || IsBattlerSpritePresent(r4) == 0)
    {
        sprite->callback = SpriteCB_SetInvisible;
        return;
    }
    if (gAnimScriptActive || r7->invisible)
        invisible = TRUE;
    else if (gBattleSpriteInfo[r4].transformSpecies != 0 && gEnemyMonElevation[gBattleSpriteInfo[r4].transformSpecies] == 0)
        invisible = TRUE;
    if (gBattleSpriteInfo[r4].behindSubstitute)
        invisible = TRUE;
    sprite->x = r7->x;
    sprite->x2 = r7->x2;
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
            gSprites[gBattleHealthBoxInfo[battler].unk7].callback = SpriteCB_EnemyShadow;
        else
            gSprites[gBattleHealthBoxInfo[battler].unk7].callback = SpriteCB_SetInvisible;
    }
}

void HideBattlerShadowSprite(u8 battler)
{
    gSprites[gBattleHealthBoxInfo[battler].unk7].callback = SpriteCB_SetInvisible;
}

void SetBattleMenuWindowTransparentPixelsToOpaque(void)
{
    u16 *tilePixels = (u16 *)(VRAM + 0x240);
    s32 tile;
    s32 pixelWord;

    for (tile = 0; tile < 9; tile++)
    {
        for (pixelWord = 0; pixelWord < 16; pixelWord++)
        {
            if (!(*tilePixels & 0xF000))
                *tilePixels |= 0xF000;
            if (!(*tilePixels & 0x0F00))
                *tilePixels |= 0x0F00;
            if (!(*tilePixels & 0x00F0))
                *tilePixels |= 0x00F0;
            if (!(*tilePixels & 0x000F))
                *tilePixels |= 0x000F;
            tilePixels++;
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
