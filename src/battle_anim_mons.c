#include "global.h"
#include "constants/battle_anim.h"
#include "constants/species.h"
#include "battle_anim.h"
#include "battle.h"
#include "contest.h"
#include "data.h"
#include "decompress.h"
#include "palette.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "util.h"
#include "ewram.h"

static void UNUSED TranslateSpriteInLissajousCurve(struct Sprite *sprite);
static void AnimPosToTranslateLinear(struct Sprite *sprite);
static void TranslateSpriteLinearFixedPointIconFrame(struct Sprite *sprite);
static void UNUSED TranslateSpriteToBattleTargetPos(struct Sprite *sprite);
static void UNUSED TranslateSpriteToBattleAttackerPos(struct Sprite *sprite);
static void UNUSED EndUnkPaletteAnim(struct Sprite *sprite);
static void InitAnimFastLinearTranslation(struct Sprite *sprite);
static void AnimFastTranslateLinearWaitEnd(struct Sprite *sprite);
static bool8 ShouldRotScaleSpeciesBeFlipped(void);
static u8 UNUSED GetSpritePalIdxByPosition(u8 position);
static void AnimThrowProjectile_Step(struct Sprite *sprite);
static void AnimTask_AlphaFadeIn_Step(u8 taskId);
static void AnimTask_BlendPalInAndOutSetup(struct Task *task);
static void AnimTask_BlendMonInAndOut_Step(u8 taskId);
static u16 GetBattlerYDeltaFromSpriteId(u8 spriteId);
static void UNUSED SetPriorityForVisibleBattlers(u8 priority);
static void AnimTask_AttackerPunchWithTrace_Step(u8 taskId);
static void CreateBattlerTrace(struct Task *task, u8 taskId);
static void AnimBattlerTrace(struct Sprite *sprite);
static void AnimWeatherBallUp_Step(struct Sprite *sprite);

#define IS_DOUBLE_BATTLE() ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? TRUE : FALSE)

struct Struct_2017810
{
    u8 filler_0[6];
    u8 field_6;
    u8 filler_7[5];
};

#define BG1CNT (*(vBgCnt *)REG_ADDR_BG1CNT)
#define BG2CNT (*(vBgCnt *)REG_ADDR_BG2CNT)
#define BG3CNT (*(vBgCnt *)REG_ADDR_BG3CNT)

extern struct Struct_2017810 unk_2017810[];

EWRAM_DATA static union AffineAnimCmd *sAnimTaskAffineAnim = NULL;
EWRAM_DATA u32 filler_0202F7D8[3] = {0};

static const struct UCoords8 sBattlerCoords[][4] =
{
    {
        { 72, 80 },
        { 176, 40 },
        { 48, 40 },
        { 112, 80 },
    },
    {
        { 32, 80 },
        { 200, 40 },
        { 90, 88 },
        { 152, 32 },
    },
};

// One entry for each of the four Castform forms.
const struct MonCoords gCastformFrontSpriteCoords[] =
{
    { 0x44, 17 }, // NORMAL
    { 0x66, 9 }, // SUN
    { 0x46, 9 }, // RAIN
    { 0x86, 8 }, // HAIL
};

static const u8 sCastformElevations[] =
{
    13, // NORMAL
    14, // SUN
    13, // RAIN
    13, // HAIL
};

// Y position of the backsprite for each of the four Castform forms.
static const u8 sCastformBackSpriteYCoords[] =
{
    0, // NORMAL
    0, // SUN
    0, // RAIN
    0, // HAIL
};

static const struct SpriteTemplate sSpriteTemplates_MoveEffectMons[] =
{
    {
        .tileTag = 55125,
        .paletteTag = 55125,
        .oam = &gOamData_AffineNormal_ObjNormal_64x64,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    },
    {
        .tileTag = 55126,
        .paletteTag = 55126,
        .oam = &gOamData_AffineNormal_ObjNormal_64x64,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    }
};

static const struct SpriteSheet sSpriteSheets_MoveEffectMons[] =
{
    { gMiscBlank_Gfx, 0x800, 55125, },
    { gMiscBlank_Gfx, 0x800, 55126, },
};

// pkmn_form.c

u8 GetBattlerSpriteCoord(u8 battler, u8 coordType)
{
    u8 retVal;
    u16 species;
    struct BattleSpriteInfo *spriteInfo;

    if (IsContest())
    {
        if (coordType == 3 && battler == 3)
            coordType = 1;
    }
    switch (coordType)
    {
    case 0:
    case 2:
        retVal = sBattlerCoords[IS_DOUBLE_BATTLE()][GetBattlerPosition(battler)].x;
        break;
    case 1:
        retVal = sBattlerCoords[IS_DOUBLE_BATTLE()][GetBattlerPosition(battler)].y;
        break;
    case 3:
    case 4:
    default:
        if (IsContest())
        {
            if (gContestResources__moveAnim.hasTargetAnim)
                species = gContestResources__moveAnim.targetSpecies;
            else
                species = gContestResources__moveAnim.species;
        }
        else
        {
            if (GetBattlerSide(battler))
            {
                spriteInfo = &gBattleSpriteInfo[battler];
                if (!spriteInfo->transformSpecies)
                    species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
                else
                    species = spriteInfo->transformSpecies;
            }
            else
            {
                spriteInfo = &gBattleSpriteInfo[battler];
                if (!spriteInfo->transformSpecies)
                    species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
                else
                    species = spriteInfo->transformSpecies;
            }
        }
        if (coordType == 3)
            retVal = GetBattlerSpriteFinal_Y(battler, species, 1);
        else
            retVal = GetBattlerSpriteFinal_Y(battler, species, 0);
        break;
    }
    return retVal;
}

u8 GetBattlerYDelta(u8 battler, u16 species)
{
    u16 letter;
    u32 personality;
    struct BattleSpriteInfo *spriteInfo;
    u8 ret;
    u16 coordSpecies;

    if (GetBattlerSide(battler) == 0 || IsContest())
    {
        if (species == SPECIES_UNOWN)
        {
            if (IsContest())
            {
                if (gContestResources__moveAnim.hasTargetAnim)
                    personality = gContestResources__moveAnim.targetPersonality;
                else
                    personality = gContestResources__moveAnim.personality;
            }
            else
            {
                spriteInfo = &gBattleSpriteInfo[battler];
                if (!spriteInfo->transformSpecies)
                    personality = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);
                else
                    personality = gTransformedPersonalities[battler];
            }
            letter = GET_UNOWN_LETTER(personality);
            if (!letter)
                coordSpecies = species;
            else
                coordSpecies = letter + SPECIES_UNOWN_B - 1;
            ret = gMonBackPicCoords[coordSpecies].y_offset;
        }
        else if (species == SPECIES_CASTFORM)
        {
            ret = sCastformBackSpriteYCoords[gBattleMonForms[battler]];
        }
        else if (species > NUM_SPECIES)
        {
            ret = gMonBackPicCoords[0].y_offset;
        }
        else
        {
            ret = gMonBackPicCoords[species].y_offset;
        }
    }
    else
    {
        if (species == SPECIES_UNOWN)
        {
            spriteInfo = &gBattleSpriteInfo[battler];
            if (!spriteInfo->transformSpecies)
                personality = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);
            else
                personality = gTransformedPersonalities[battler];
            letter = GET_UNOWN_LETTER(personality);
            if (!letter)
                coordSpecies = species;
            else
                coordSpecies = letter + SPECIES_UNOWN_B - 1;
            ret = gMonFrontPicCoords[coordSpecies].y_offset;
        }
        else if (species == SPECIES_CASTFORM)
        {
            ret = gCastformFrontSpriteCoords[gBattleMonForms[battler]].y_offset;
        }
        else if (species > NUM_SPECIES)
        {
            ret = gMonFrontPicCoords[0].y_offset;
        }
        else
        {
            ret = gMonFrontPicCoords[species].y_offset;
        }
    }
    return ret;
}

u8 GetBattlerElevation(u8 battler, u16 species)
{
    u8 ret = 0;
    if (GetBattlerSide(battler) == 1)
    {
        if (!IsContest())
        {
            if (species == SPECIES_CASTFORM)
                ret = sCastformElevations[gBattleMonForms[battler]];
            else if (species > NUM_SPECIES)
                ret = gEnemyMonElevation[0];
            else
                ret = gEnemyMonElevation[species];
        }
    }
    return ret;
}

u8 GetBattlerSpriteFinal_Y(u8 battler, u16 species, bool8 a3)
{
    u16 offset;
    u8 y;

    if (GetBattlerSide(battler) == 0 || IsContest())
    {
        offset = GetBattlerYDelta(battler, species);
    }
    else
    {
        offset = GetBattlerYDelta(battler, species);
        offset -= GetBattlerElevation(battler, species);
    }
    y = offset + sBattlerCoords[IS_DOUBLE_BATTLE()][GetBattlerPosition(battler)].y;
    if (a3)
    {
        if (GetBattlerSide(battler) == 0)
            y += 8;
        if (y > 104)
            y = 104;
    }
    return y;
}

u8 GetBattlerSpriteCoord2(u8 battler, u8 coordType)
{
    u16 species;
    struct BattleSpriteInfo *spriteInfo;
    if (coordType == 3 || coordType == 4)
    {
        if (IsContest())
        {
            if (gContestResources__moveAnim.hasTargetAnim)
                species = gContestResources__moveAnim.targetSpecies;
            else
                species = gContestResources__moveAnim.species;
        }
        else
        {
            spriteInfo = &gBattleSpriteInfo[battler];
            if (!spriteInfo->transformSpecies)
                species = gAnimBattlerSpecies[battler];
            else
                species = spriteInfo->transformSpecies;
        }
        if (coordType == 3)
            return GetBattlerSpriteFinal_Y(battler, species, 1);
        else
            return GetBattlerSpriteFinal_Y(battler, species, 0);
    }
    else
    {
        return GetBattlerSpriteCoord(battler, coordType);
    }
}

u8 GetBattlerSpriteDefault_Y(u8 battler)
{
    return GetBattlerSpriteCoord(battler, 4);
}

u8 GetSubstituteSpriteDefault_Y(u8 battler)
{
    u16 y;
    if (GetBattlerSide(battler) != 0)
        y = GetBattlerSpriteCoord(battler, 1) + 16;
    else
        y = GetBattlerSpriteCoord(battler, 1) + 17;
    return y;
}

u8 GetBattlerYCoordWithElevation(u8 battler)
{
    u16 species;
    u8 y;
    struct BattleSpriteInfo *spriteInfo;

    y = GetBattlerSpriteCoord(battler, 1);
    if (!IsContest())
    {
        if (GetBattlerSide(battler) != 0)
        {
            spriteInfo = &gBattleSpriteInfo[battler];
            if (!spriteInfo->transformSpecies) {
                species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
            } else {
                species = spriteInfo->transformSpecies;
            }
        }
        else
        {
            spriteInfo = &gBattleSpriteInfo[battler];
            if (!spriteInfo->transformSpecies)
                species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
            else
                species = spriteInfo->transformSpecies;
        }
        if (GetBattlerSide(battler) != 0)
            y -= GetBattlerElevation(battler, species);
    }
    return y;
}

u8 GetAnimBattlerSpriteId(u8 animBattler)
{
    u8 *sprites;

    if (animBattler == ANIM_ATTACKER)
    {
        if (IsBattlerSpritePresent(gBattleAnimAttacker))
        {
            sprites = gBattlerSpriteIds;
            return sprites[gBattleAnimAttacker];
        }
        else
        {
            return 0xff;
        }
    }
    else if (animBattler == ANIM_TARGET)
    {
        if (IsBattlerSpritePresent(gBattleAnimTarget))
        {
            sprites = gBattlerSpriteIds;
            return sprites[gBattleAnimTarget];
        }
        else
        {
            return 0xff;
        }
    }
    else if (animBattler == ANIM_ATK_PARTNER)
    {
        if (!IsBattlerSpriteVisible(gBattleAnimAttacker ^ 2))
            return 0xff;
        else
            return gBattlerSpriteIds[gBattleAnimAttacker ^ 2];
    }
    else
    {
        if (IsBattlerSpriteVisible(gBattleAnimTarget ^ 2))
            return gBattlerSpriteIds[gBattleAnimTarget ^ 2];
        else
            return 0xff;
    }
}

void StoreSpriteCallbackInData6(struct Sprite *sprite, void (*callback)(struct Sprite*))
{
    sprite->data[6] = (u32)(callback) & 0xffff;
    sprite->data[7] = (u32)(callback) >> 16;
}

void SetCallbackToStoredInData6(struct Sprite *sprite)
{
    u32 callback = (u16)sprite->data[6] | (sprite->data[7] << 16);
    sprite->callback = (void (*)(struct Sprite *))callback;
}

void TranslateSpriteInCircle(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], sprite->data[1]);
        sprite->y2 = Cos(sprite->data[0], sprite->data[1]);
        sprite->data[0] += sprite->data[2];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        sprite->data[3]--;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void TranslateSpriteInGrowingCircle(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], (sprite->data[5] >> 8) + sprite->data[1]);
        sprite->y2 = Cos(sprite->data[0], (sprite->data[5] >> 8) + sprite->data[1]);
        sprite->data[0] += sprite->data[2];
        sprite->data[5] += sprite->data[4];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        sprite->data[3]--;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

static void UNUSED TranslateSpriteInLissajousCurve(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], sprite->data[1]);
        sprite->y2 = Cos(sprite->data[4], sprite->data[1]);
        sprite->data[0] += sprite->data[2];
        sprite->data[4] += sprite->data[5];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        if (sprite->data[4] >= 0x100)
            sprite->data[4] -= 0x100;
        else if (sprite->data[4] < 0)
            sprite->data[4] += 0x100;
        sprite->data[3]--;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void TranslateSpriteInEllipse(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], sprite->data[1]);
        sprite->y2 = Cos(sprite->data[0], sprite->data[4]);
        sprite->data[0] += sprite->data[2];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        sprite->data[3]--;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

// Simply waits until the sprite's data[0] hits zero.
// This is used to let sprite anims or affine anims to run for a designated
// duration.
void WaitAnimForDuration(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
        sprite->data[0]--;
    else
        SetCallbackToStoredInData6(sprite);
}

static void AnimPosToTranslateLinear(struct Sprite *sprite)
{
    ConvertPosDataToTranslateLinearData(sprite);
    sprite->callback = TranslateSpriteLinear;
    sprite->callback(sprite);
}

void ConvertPosDataToTranslateLinearData(struct Sprite *sprite)
{
    s16 old;
    int xDiff;

    if (sprite->data[1] > sprite->data[2])
        sprite->data[0] = -sprite->data[0];
    xDiff = sprite->data[2] - sprite->data[1];
    old = sprite->data[0];
    sprite->data[0] = abs(xDiff / sprite->data[0]);
    sprite->data[2] = (sprite->data[4] - sprite->data[3]) / sprite->data[0];
    sprite->data[1] = old;
}

void TranslateSpriteLinear(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        sprite->data[0]--;
        sprite->x2 += sprite->data[1];
        sprite->y2 += sprite->data[2];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void TranslateSpriteLinearFixedPoint(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        sprite->data[0]--;
        sprite->data[3] += sprite->data[1];
        sprite->data[4] += sprite->data[2];
        sprite->x2 = sprite->data[3] >> 8;
        sprite->y2 = sprite->data[4] >> 8;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

static void TranslateSpriteLinearFixedPointIconFrame(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        sprite->data[0]--;
        sprite->data[3] += sprite->data[1];
        sprite->data[4] += sprite->data[2];
        sprite->x2 = sprite->data[3] >> 8;
        sprite->y2 = sprite->data[4] >> 8;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
    UpdateMonIconFrame(sprite);
}

static void UNUSED TranslateSpriteToBattleTargetPos(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x + sprite->x2;
    sprite->data[3] = sprite->y + sprite->y2;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->callback = AnimPosToTranslateLinear;
}

void TranslateSpriteLinearById(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        sprite->data[0]--;
        gSprites[sprite->data[3]].x2 += sprite->data[1];
        gSprites[sprite->data[3]].y2 += sprite->data[2];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

// Same as TranslateMonBGUntil, but it operates on sub-pixel values
// to handle slower translations.
void TranslateSpriteLinearByIdFixedPoint(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        sprite->data[0]--;
        sprite->data[3] += sprite->data[1];
        sprite->data[4] += sprite->data[2];
        gSprites[sprite->data[5]].x2 = sprite->data[3] >> 8;
        gSprites[sprite->data[5]].y2 = sprite->data[4] >> 8;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void TranslateSpriteLinearAndFlicker(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        sprite->data[0]--;
        sprite->x2 = sprite->data[2] >> 8;
        sprite->data[2] += sprite->data[1];
        sprite->y2 = sprite->data[4] >> 8;
        sprite->data[4] += sprite->data[3];
        if (sprite->data[0] % sprite->data[5] == 0)
        {
            if (sprite->data[5])
                sprite->invisible ^= 1;
        }
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void DestroySpriteAndMatrix(struct Sprite *sprite)
{
    FreeSpriteOamMatrix(sprite);
    DestroyAnimSprite(sprite);
}

static void UNUSED TranslateSpriteToBattleAttackerPos(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x + sprite->x2;
    sprite->data[3] = sprite->y + sprite->y2;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    sprite->callback = AnimPosToTranslateLinear;
}

static void UNUSED EndUnkPaletteAnim(struct Sprite *sprite)
{
    PaletteStruct_ResetById(sprite->data[5]);
    DestroySpriteAndMatrix(sprite);
}

void RunStoredCallbackWhenAffineAnimEnds(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
        SetCallbackToStoredInData6(sprite);
}

void RunStoredCallbackWhenAnimEnds(struct Sprite *sprite)
{
    if (sprite->animEnded)
        SetCallbackToStoredInData6(sprite);
}

void DestroyAnimSpriteAndDisableBlend(struct Sprite *sprite)
{
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    DestroyAnimSprite(sprite);
}

void DestroyAnimVisualTaskAndDisableBlend(u8 taskId)
{
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    DestroyAnimVisualTask(taskId);
}

void SetSpriteCoordsToAnimAttackerCoords(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
}

void SetAnimSpriteInitialXOffset(struct Sprite *sprite, s16 xOffset)
{
    u16 attackerX = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
    u16 targetX = GetBattlerSpriteCoord(gBattleAnimTarget, 0);

    if (attackerX > targetX)
    {
        sprite->x -= xOffset;
    }
    else if (attackerX < targetX)
    {
        sprite->x += xOffset;
    }
    else
    {
        if (GetBattlerSide(gBattleAnimAttacker) != 0)
            sprite->x -= xOffset;
        else
            sprite->x += xOffset;
    }
}

void InitAnimArcTranslation(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitAnimLinearTranslation(sprite);
    sprite->data[6] = 0x8000 / sprite->data[0];
    sprite->data[7] = 0;
}

bool8 TranslateAnimHorizontalArc(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
        return TRUE;
    sprite->data[7] += sprite->data[6];
    sprite->y2 += Sin((u8)(sprite->data[7] >> 8), sprite->data[5]);
    return FALSE;
}

void SetSpritePrimaryCoordsFromSecondaryCoords(struct Sprite *sprite)
{
    sprite->x += sprite->x2;
    sprite->y += sprite->y2;
    sprite->x2 = 0;
    sprite->y2 = 0;
}

void InitSpritePosToAnimTarget(struct Sprite *sprite, bool8 respectMonPicOffsets)
{
    if (!respectMonPicOffsets)
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimTarget, 0);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimTarget, 1);
    }
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}

void InitSpritePosToAnimAttacker(struct Sprite *sprite, bool8 respectMonPicOffsets)
{
    if (!respectMonPicOffsets)
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimAttacker, 0);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimAttacker, 1);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimAttacker, 2);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimAttacker, 3);
    }
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}

u8 GetBattlerSide(u8 battler)
{
    return gBattlerPositions[battler] & 1;
}

u8 GetBattlerPosition(u8 battler)
{
    return gBattlerPositions[battler];
}

u8 GetBattlerAtPosition(u8 position)
{
    u8 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattlerPositions[i] == position)
            break;
    }
    return i;
}

bool8 IsBattlerSpritePresent(u8 battler)
{
    if (IsContest())
    {
        if (gBattleAnimAttacker == battler)
            return TRUE;
        if (gBattleAnimTarget == battler)
            return TRUE;
        return FALSE;
    }
    else
    {
        if (gBattlerPositions[battler] == 0xff)
            return FALSE;
        if (GetBattlerSide(battler) != B_SIDE_PLAYER)
        {
            if (GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_HP) != 0)
                return TRUE;
        }
        else
        {
            if (GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_HP) != 0)
                return TRUE;
        }
        return FALSE;
    }
}

bool8 IsDoubleBattle(void)
{
    return IS_DOUBLE_BATTLE();
}

void GetBattleAnimBg1Data(struct BattleAnimBgData *out)
{
    if (IsContest())
    {
        out->bgTiles = (u8 *)(VRAM + 0x8000);
        out->bgTilemap = (u8 *)(VRAM + 0xf000);
        out->paletteId = 0xe;
    }
    else
    {
        out->bgTiles = (u8 *)(VRAM + 0x4000);
        out->bgTilemap = (u8 *)(VRAM + 0xe000);
        out->paletteId = 0x8;
    }
}

void GetBgDataForTransform(struct BattleAnimBgData *out, u8 battler)
{
    if (IsContest())
    {
        out->bgTiles = (u8 *)(VRAM + 0x8000);
        out->bgTilemap = (u8 *)(VRAM + 0xf000);
        out->paletteId = 0xe;
    }
    else if (GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) == 1)
    {
        out->bgTiles = (u8 *)(VRAM + 0x4000);
        out->bgTilemap = (u8 *)(VRAM + 0xe000);
        out->paletteId = 0x8;
    }
    else
    {
        out->bgTiles = (u8 *)(VRAM + 0x6000);
        out->bgTilemap = (u8 *)(VRAM + 0xf000);
        out->paletteId = 0x9;
    }
}

u8 GetBattleBgPaletteNum(void)
{
    if (IsContest())
        return 1;
    return 2;
}

void UpdateAnimBg3ScreenSize(bool8 largeScreenSize)
{
    if (!largeScreenSize)
    {
        BG3CNT.screenSize = 0;
        BG3CNT.areaOverflowMode = 1;
    }
    else if (IsContest())
    {
        BG3CNT.screenSize = 0;
        BG3CNT.areaOverflowMode = 1;
    }
    else
    {
        BG3CNT.screenSize = 1;
        BG3CNT.areaOverflowMode = 0;
    }
}

void Trade_MoveSelectedMonToTarget(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitSpriteDataForLinearTranslation(sprite);
    sprite->callback = TranslateSpriteLinearFixedPointIconFrame;
    sprite->callback(sprite);
}

void InitSpriteDataForLinearTranslation(struct Sprite *sprite)
{
    s16 x = (sprite->data[2] - sprite->data[1]) << 8;
    s16 y = (sprite->data[4] - sprite->data[3]) << 8;
    sprite->data[1] = x / sprite->data[0];
    sprite->data[2] = y / sprite->data[0];
    sprite->data[4] = 0;
    sprite->data[3] = 0;
}

void InitAnimLinearTranslation(struct Sprite *sprite)
{
    int x = sprite->data[2] - sprite->data[1];
    int y = sprite->data[4] - sprite->data[3];
    bool8 movingLeft = x < 0;
    bool8 movingUp = y < 0;
    u16 xDelta = abs(x) << 8;
    u16 yDelta = abs(y) << 8;

    xDelta = xDelta / sprite->data[0];
    yDelta = yDelta / sprite->data[0];

    if (movingLeft)
        xDelta |= 1;
    else
        xDelta &= ~1;

    if (movingUp)
        yDelta |= 1;
    else
        yDelta &= ~1;

    sprite->data[1] = xDelta;
    sprite->data[2] = yDelta;
    sprite->data[4] = 0;
    sprite->data[3] = 0;
}

void StartAnimLinearTranslation(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitAnimLinearTranslation(sprite);
    sprite->callback = AnimTranslateLinear_WithFollowup;
    sprite->callback(sprite);
}

bool8 AnimTranslateLinear(struct Sprite *sprite)
{
    u16 v1, v2, x, y;

    if (!sprite->data[0])
        return TRUE;

    v1 = sprite->data[1];
    v2 = sprite->data[2];
    x = sprite->data[3];
    y = sprite->data[4];
    x += v1;
    y += v2;

    if (v1 & 1)
        sprite->x2 = -(x >> 8);
    else
        sprite->x2 = x >> 8;

    if (v2 & 1)
        sprite->y2 = -(y >> 8);
    else
        sprite->y2 = y >> 8;

    sprite->data[3] = x;
    sprite->data[4] = y;
    sprite->data[0]--;
    return FALSE;
}

void AnimTranslateLinear_WithFollowup(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
        SetCallbackToStoredInData6(sprite);
}

void InitAnimLinearTranslationWithSpeed(struct Sprite *sprite)
{
    int v1 = abs(sprite->data[2] - sprite->data[1]) << 8;
    sprite->data[0] = v1 / sprite->data[0];
    InitAnimLinearTranslation(sprite);
}

void InitAnimLinearTranslationWithSpeedAndPos(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitAnimLinearTranslationWithSpeed(sprite);
    sprite->callback = AnimTranslateLinear_WithFollowup;
    sprite->callback(sprite);
}

static void InitAnimFastLinearTranslation(struct Sprite *sprite)
{
    int xDiff = sprite->data[2] - sprite->data[1];
    int yDiff = sprite->data[4] - sprite->data[3];
    bool8 x_sign = xDiff < 0;
    bool8 y_sign = yDiff < 0;
    u16 x2 = abs(xDiff) << 4;
    u16 y2 = abs(yDiff) << 4;

    x2 /= sprite->data[0];
    y2 /= sprite->data[0];

    if (x_sign)
        x2 |= 1;
    else
        x2 &= ~1;

    if (y_sign)
        y2 |= 1;
    else
        y2 &= ~1;

    sprite->data[1] = x2;
    sprite->data[2] = y2;
    sprite->data[4] = 0;
    sprite->data[3] = 0;
}

void InitAndRunAnimFastLinearTranslation(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitAnimFastLinearTranslation(sprite);
    sprite->callback = AnimFastTranslateLinearWaitEnd;
    sprite->callback(sprite);
}

bool8 AnimFastTranslateLinear(struct Sprite *sprite)
{
    u16 v1, v2, x, y;

    if (!sprite->data[0])
        return TRUE;

    v1 = sprite->data[1];
    v2 = sprite->data[2];
    x = sprite->data[3];
    y = sprite->data[4];
    x += v1;
    y += v2;

    if (v1 & 1)
        sprite->x2 = -(x >> 4);
    else
        sprite->x2 = x >> 4;

    if (v2 & 1)
        sprite->y2 = -(y >> 4);
    else
        sprite->y2 = y >> 4;

    sprite->data[3] = x;
    sprite->data[4] = y;
    sprite->data[0]--;
    return FALSE;
}

static void AnimFastTranslateLinearWaitEnd(struct Sprite *sprite)
{
    if (AnimFastTranslateLinear(sprite))
        SetCallbackToStoredInData6(sprite);
}

void InitAnimFastLinearTranslationWithSpeed(struct Sprite *sprite)
{
    int xDiff = abs(sprite->data[2] - sprite->data[1]) << 4;
    sprite->data[0] = xDiff / sprite->data[0];
    InitAnimFastLinearTranslation(sprite);
}

void InitAnimFastLinearTranslationWithSpeedAndPos(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitAnimFastLinearTranslationWithSpeed(sprite);
    sprite->callback = AnimFastTranslateLinearWaitEnd;
    sprite->callback(sprite);
}

void SetSpriteRotScale(u8 spriteId, s16 xScale, s16 yScale, u16 rotation)
{
    int i;
    struct ObjAffineSrcData src;
    struct OamMatrix matrix;

    src.xScale = xScale;
    src.yScale = yScale;
    src.rotation = rotation;
    if (ShouldRotScaleSpeciesBeFlipped())
        src.xScale = -src.xScale;
    i = gSprites[spriteId].oam.matrixNum;
    ObjAffineSet(&src, &matrix, 1, 2);
    gOamMatrices[i].a = matrix.a;
    gOamMatrices[i].b = matrix.b;
    gOamMatrices[i].c = matrix.c;
    gOamMatrices[i].d = matrix.d;
}

static bool8 ShouldRotScaleSpeciesBeFlipped(void)
{
    if (IsContest())
    {
        if (gSprites[GetAnimBattlerSpriteId(0)].data[2] == 0xc9 /* XXX SPECIES_UNOWN? */)
            return FALSE;
        return TRUE;
    }
    return FALSE;
}

void PrepareBattlerSpriteForRotScale(u8 spriteId, u8 objMode)
{
    u8 battler = gSprites[spriteId].data[0];

    if (IsContest() || IsBattlerSpriteVisible(battler))
        gSprites[spriteId].invisible = FALSE;
    gSprites[spriteId].oam.objMode = objMode;
    gSprites[spriteId].affineAnimPaused = TRUE;
    if (!IsContest() && !gSprites[spriteId].oam.affineMode)
        gSprites[spriteId].oam.matrixNum = gBattleHealthBoxInfo[battler].matrixNum;
    gSprites[spriteId].oam.affineMode = 3;
    CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
}

void ResetSpriteRotScale(u8 spriteId)
{
    SetSpriteRotScale(spriteId, 0x100, 0x100, 0);
    gSprites[spriteId].oam.affineMode = 1;
    gSprites[spriteId].oam.objMode = 0;
    gSprites[spriteId].affineAnimPaused = FALSE;
    CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
}

void SetBattlerSpriteYOffsetFromRotation(u8 spriteId)
{
    u16 matrixNum = gSprites[spriteId].oam.matrixNum;
    s16 c = gOamMatrices[matrixNum].c;

    if (c < 0)
        c = -c;
    gSprites[spriteId].y2 = c >> 3;
}

// related to obj_id_set_rotscale
void TrySetSpriteRotScale(struct Sprite *sprite, bool8 recalcCenterVector, s16 xScale, s16 yScale, u16 rotation)
{
    int i;
    struct ObjAffineSrcData src;
    struct OamMatrix matrix;

    if (sprite->oam.affineMode & 1)
    {
        sprite->affineAnimPaused = TRUE;
        if (recalcCenterVector)
            CalcCenterToCornerVec(sprite, sprite->oam.shape, sprite->oam.size, sprite->oam.affineMode);
        src.xScale = xScale;
        src.yScale = yScale;
        src.rotation = rotation;
        if (ShouldRotScaleSpeciesBeFlipped())
            src.xScale = -src.xScale;
        i = sprite->oam.matrixNum;
        ObjAffineSet(&src, &matrix, 1, 2);
        gOamMatrices[i].a = matrix.a;
        gOamMatrices[i].b = matrix.b;
        gOamMatrices[i].c = matrix.c;
        gOamMatrices[i].d = matrix.d;
    }
}

void ResetSpriteRotScale_PreserveAffine(struct Sprite *sprite)
{
    TrySetSpriteRotScale(sprite, TRUE, 0x100, 0x100, 0);
    sprite->affineAnimPaused = FALSE;
    CalcCenterToCornerVec(sprite, sprite->oam.shape, sprite->oam.size, sprite->oam.affineMode);
}

static u16 ArcTan2_(s16 x, s16 y)
{
    return ArcTan2(x, y);
}

u16 ArcTan2Neg(s16 x, s16 y)
{
    u16 var = ArcTan2_(x, y);
    return -var;
}

void SetGrayscaleOrOriginalPalette(u16 paletteNum, bool8 restoreOriginalColor)
{
    int i;
    struct PlttData *originalColor;
    struct PlttData *destColor;
    u16 average;

    paletteNum *= 0x10;

    if (!restoreOriginalColor)
    {
        for (i = 0; i < 0x10; i++)
        {
            originalColor = (struct PlttData *)&gPlttBufferUnfaded[paletteNum + i];
            average = originalColor->r + originalColor->g + originalColor->b;
            average /= 3;

            destColor = (struct PlttData *)&gPlttBufferFaded[paletteNum + i];
            destColor->r = average;
            destColor->g = average;
            destColor->b = average;
        }
    }
    else
    {
        CpuCopy32(&gPlttBufferUnfaded[paletteNum], &gPlttBufferFaded[paletteNum], 0x20);
    }
}

u32 GetBattlePalettesMask(bool8 battleBackground, bool8 attacker, bool8 target, bool8 attackerPartner, bool8 targetPartner, bool8 anim1, bool8 anim2)
{
    u32 selectedPalettes = 0;
    u32 shift;

    if (battleBackground)
    {
        if (!IsContest())
            selectedPalettes = 0xe;
        else
            selectedPalettes = 1 << GetBattleBgPaletteNum();
    }
    if (attacker)
    {
        shift = gBattleAnimAttacker + 16;
        selectedPalettes |= 1 << shift;
    }
    if (target) {
        shift = gBattleAnimTarget + 16;
        selectedPalettes |= 1 << shift;
    }
    if (attackerPartner)
    {
        if (IsBattlerSpriteVisible(gBattleAnimAttacker ^ 2))
        {
            shift = (gBattleAnimAttacker ^ 2) + 16;
            selectedPalettes |= 1 << shift;
        }
    }
    if (targetPartner)
    {
        if (IsBattlerSpriteVisible(gBattleAnimTarget ^ 2))
        {
            shift = (gBattleAnimTarget ^ 2) + 16;
            selectedPalettes |= 1 << shift;
        }
    }
    if (anim1)
    {
        if (!IsContest())
            selectedPalettes |= 0x100;
        else
            selectedPalettes |= 0x4000;
    }
    if (anim2)
    {
        if (!IsContest())
            selectedPalettes |= 0x200;
    }
    return selectedPalettes;
}

u32 GetBattleMonSpritePalettesMask(u8 playerLeft, u8 playerRight, u8 opponentLeft, u8 opponentRight)
{
    u32 selectedPalettes = 0;
    u32 shift;

    if (IsContest())
    {
        if (playerLeft)
        {
            selectedPalettes |= 1 << 18;
            return selectedPalettes;
        }
    } else {
        if (playerLeft) {
            if (IsBattlerSpriteVisible(GetBattlerAtPosition(0))) {
                selectedPalettes |= 1 << (GetBattlerAtPosition(0) + 16);
            }
        }
        if (playerRight) {
            if (IsBattlerSpriteVisible(GetBattlerAtPosition(2))) {
                shift = GetBattlerAtPosition(2) + 16;
                selectedPalettes |= 1 << shift;
            }
        }
        if (opponentLeft) {
            if (IsBattlerSpriteVisible(GetBattlerAtPosition(1))) {
                shift = GetBattlerAtPosition(1) + 16;
                selectedPalettes |= 1 << shift;
            }
        }
        if (opponentRight) {
            if (IsBattlerSpriteVisible(GetBattlerAtPosition(3))) {
                shift = GetBattlerAtPosition(3) + 16;
                selectedPalettes |= 1 << shift;
            }
        }
    }
    return selectedPalettes;
}

u8 GetSpritePalIdxByBattler(u8 battler)
{
    return battler;
}

static u8 UNUSED GetSpritePalIdxByPosition(u8 position)
{
    return GetBattlerAtPosition(position);
}

void AnimSpriteOnMonPos(struct Sprite *sprite)
{
    bool8 respectMonPicOffsets;

    if (!sprite->data[0])
    {
        if (!gBattleAnimArgs[3])
            respectMonPicOffsets = TRUE;
        else
            respectMonPicOffsets = FALSE;
        if (!gBattleAnimArgs[2])
            InitSpritePosToAnimAttacker(sprite, respectMonPicOffsets);
        else
            InitSpritePosToAnimTarget(sprite, respectMonPicOffsets);
        sprite->data[0]++;

    }
    else if (sprite->animEnded || sprite->affineAnimEnded)
    {
        DestroySpriteAndMatrix(sprite);
    }
}

// Linearly translates a sprite to a target position on the
// other mon's sprite.
// arg 0: initial x offset
// arg 1: initial y offset
// arg 2: target x offset
// arg 3: target y offset
// arg 4: duration
// arg 5: lower 8 bits = location on attacking mon, upper 8 bits = location on target mon pick to target
void TranslateAnimSpriteToTargetMonLocation(struct Sprite *sprite)
{
    bool8 respectMonPicOffsets;
    u8 coordType;

    if (!(gBattleAnimArgs[5] & 0xff00))
        respectMonPicOffsets = TRUE;
    else
        respectMonPicOffsets = FALSE;

    if (!(gBattleAnimArgs[5] & 0xff))
        coordType = 3;
    else
        coordType = 1;

    InitSpritePosToAnimAttacker(sprite, respectMonPicOffsets);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, coordType) + gBattleAnimArgs[3];
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimThrowProjectile(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);
    if (GetBattlerSide(gBattleAnimAttacker))
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[3];
    sprite->data[5] = gBattleAnimArgs[5];
    InitAnimArcTranslation(sprite);
    sprite->callback = AnimThrowProjectile_Step;
}

static void AnimThrowProjectile_Step(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
        DestroyAnimSprite(sprite);
}

void AnimTravelDiagonally(struct Sprite *sprite)
{
    bool8 respectMonPicOffsets;
    u8 battler, coordType;

    if (!gBattleAnimArgs[6])
    {
        respectMonPicOffsets = TRUE;
        coordType = 3;
    }
    else
    {
        respectMonPicOffsets = FALSE;
        coordType = 1;
    }
    if (!gBattleAnimArgs[5])
    {
        InitSpritePosToAnimAttacker(sprite, respectMonPicOffsets);
        battler = gBattleAnimAttacker;
    }
    else
    {
        InitSpritePosToAnimTarget(sprite, respectMonPicOffsets);
        battler = gBattleAnimTarget;
    }
    if (GetBattlerSide(gBattleAnimAttacker))
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    InitSpritePosToAnimTarget(sprite, respectMonPicOffsets);
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(battler, 2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(battler, coordType) + gBattleAnimArgs[3];
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

s16 CloneBattlerSpriteWithBlend(u8 animBattler)
{
    u16 i;
    u8 spriteId = GetAnimBattlerSpriteId(animBattler);

    if (spriteId != 0xff)
    {
        for (i = 0; i < 0x40; i++)
        {
            if (!gSprites[i].inUse)
            {
                gSprites[i] = gSprites[spriteId];
                gSprites[i].oam.objMode = 1;
                gSprites[i].invisible = FALSE;
                return i;
            }
        }
    }
    return -1;
}

void DestroySpriteWithActiveSheet(struct Sprite *sprite)
{
    sprite->usingSheet = TRUE;
    DestroySprite(sprite);
}

void AnimTask_AlphaFadeIn(u8 taskId)
{
    s16 v1 = 0;
    s16 v2 = 0;

    if (gBattleAnimArgs[2] > gBattleAnimArgs[0])
        v2 = 1;
    if (gBattleAnimArgs[2] < gBattleAnimArgs[0])
        v2 = -1;
    if (gBattleAnimArgs[3] > gBattleAnimArgs[1])
        v1 = 1;
    if (gBattleAnimArgs[3] < gBattleAnimArgs[1])
        v1 = -1;

    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = gBattleAnimArgs[4];
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = gBattleAnimArgs[0];
    gTasks[taskId].data[4] = gBattleAnimArgs[1];
    gTasks[taskId].data[5] = v2;
    gTasks[taskId].data[6] = v1;
    gTasks[taskId].data[7] = gBattleAnimArgs[2];
    gTasks[taskId].data[8] = gBattleAnimArgs[3];
    REG_BLDALPHA = (gBattleAnimArgs[1] << 8) | gBattleAnimArgs[0];
    gTasks[taskId].func = AnimTask_AlphaFadeIn_Step;
}

static void AnimTask_AlphaFadeIn_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (++task->data[0] > task->data[1])
    {
        task->data[0] = 0;
        if (++task->data[2] & 1)
        {
            if (task->data[3] != task->data[7])
                task->data[3] += task->data[5];
        }
        else
        {
            if (task->data[4] != task->data[8])
                task->data[4] += task->data[6];
        }
        REG_BLDALPHA = (task->data[4] << 8) | task->data[3];
        if (task->data[3] == task->data[7] && task->data[4] == task->data[8])
        {
            DestroyAnimVisualTask(taskId);
            return;
        }
    }
}

// Linearly blends a mon's sprite colors with a target color with increasing
// strength, and then blends out to the original color.
// arg 0: anim bank
// arg 1: blend color
// arg 2: target blend coefficient
// arg 3: initial delay
// arg 4: number of times to blend in and out
void AnimTask_BlendMonInAndOut(u8 task)
{
    u8 spriteId = GetAnimBattlerSpriteId(gBattleAnimArgs[0]);
    if (spriteId == 0xff)
    {
        DestroyAnimVisualTask(task);
        return;
    }
    gTasks[task].data[0] = (gSprites[spriteId].oam.paletteNum * 0x10) + 0x101;
    AnimTask_BlendPalInAndOutSetup(&gTasks[task]);
}

static void AnimTask_BlendPalInAndOutSetup(struct Task *task)
{
    task->data[1] = gBattleAnimArgs[1];
    task->data[2] = 0;
    task->data[3] = gBattleAnimArgs[2];
    task->data[4] = 0;
    task->data[5] = gBattleAnimArgs[3];
    task->data[6] = 0;
    task->data[7] = gBattleAnimArgs[4];
    task->func = AnimTask_BlendMonInAndOut_Step;
}

static void AnimTask_BlendMonInAndOut_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (++task->data[4] >= task->data[5])
    {
        task->data[4] = 0;
        if (!task->data[6])
        {
            task->data[2]++;
            BlendPalette(task->data[0], 15, task->data[2], task->data[1]);
            if (task->data[2] == task->data[3])
                task->data[6] = 1;
        }
        else
        {
            task->data[2]--;
            BlendPalette(task->data[0], 15, task->data[2], task->data[1]);
            if (!task->data[2])
            {
                if (--task->data[7])
                {
                    task->data[4] = 0;
                    task->data[6] = 0;
                }
                else
                {
                    DestroyAnimVisualTask(taskId);
                    return;
                }
            }
        }
    }
}

void AnimTask_BlendPalInAndOutByTag(u8 task)
{
    u8 palette = IndexOfSpritePaletteTag(gBattleAnimArgs[0]);

    if (palette == 0xff)
    {
        DestroyAnimVisualTask(task);
        return;
    }
    gTasks[task].data[0] = (palette * 0x10) + 0x101;
    AnimTask_BlendPalInAndOutSetup(&gTasks[task]);
}

void PrepareAffineAnimInTaskData(struct Task *task, u8 spriteId, const union AffineAnimCmd *affineAnimCmds)
{
    task->data[7] = 0;
    task->data[8] = 0;
    task->data[9] = 0;
    task->data[15] = spriteId;
    task->data[10] = 0x100;
    task->data[11] = 0x100;
    task->data[12] = 0;
    StorePointerInVars(&task->data[13], &task->data[14], affineAnimCmds);
    PrepareBattlerSpriteForRotScale(spriteId, 0);
}

bool8 RunAffineAnimFromTaskData(struct Task *task)
{
    sAnimTaskAffineAnim = LoadPointerFromVars(task->data[13], task->data[14]) + (task->data[7] << 3);
    switch (sAnimTaskAffineAnim->type)
    {
    default:
        if (!sAnimTaskAffineAnim->frame.duration)
        {
            task->data[10] = sAnimTaskAffineAnim->frame.xScale;
            task->data[11] = sAnimTaskAffineAnim->frame.yScale;
            task->data[12] = sAnimTaskAffineAnim->frame.rotation;
            task->data[7]++;
            sAnimTaskAffineAnim++;
        }
        task->data[10] += sAnimTaskAffineAnim->frame.xScale;
        task->data[11] += sAnimTaskAffineAnim->frame.yScale;
        task->data[12] += sAnimTaskAffineAnim->frame.rotation;
        SetSpriteRotScale(task->data[15], task->data[10], task->data[11], task->data[12]);
        SetBattlerSpriteYOffsetFromYScale(task->data[15]);
        if (++task->data[8] >= sAnimTaskAffineAnim->frame.duration)
        {
            task->data[8] = 0;
            task->data[7]++;
        }
        break;
    case AFFINEANIMCMDTYPE_JUMP:
        task->data[7] = sAnimTaskAffineAnim->jump.target;
        break;
    case AFFINEANIMCMDTYPE_LOOP:
        if (sAnimTaskAffineAnim->loop.count)
        {
            if (task->data[9])
            {
                if (!--task->data[9])
                {
                    task->data[7]++;
                    break;
                }
            }
            else
            {
                task->data[9] = sAnimTaskAffineAnim->loop.count;
            }
            if (!task->data[7])
            {
                break;
            }
            for (;;)
            {
                task->data[7]--;
                sAnimTaskAffineAnim--;
                if (sAnimTaskAffineAnim->type == AFFINEANIMCMDTYPE_LOOP)
                {
                    task->data[7]++;
                    return TRUE;
                }
                if (!task->data[7])
                    return TRUE;
            }
        }
        task->data[7]++;
        break;
    case 0x7fff:
        gSprites[task->data[15]].y2 = 0;
        ResetSpriteRotScale(task->data[15]);
        return FALSE;
    }

    return TRUE;
}

void SetBattlerSpriteYOffsetFromYScale(u8 spriteId)
{
    int var = 0x40 - GetBattlerYDeltaFromSpriteId(spriteId) * 2;
    u16 matrix = gSprites[spriteId].oam.matrixNum;
    int var2 = (var << 8) / gOamMatrices[matrix].d;

    if (var2 > 0x80)
        var2 = 0x80;
    gSprites[spriteId].y2 = (var - var2) / 2;
}

void SetBattlerSpriteYOffsetFromOtherYScale(u8 spriteId, u8 otherSpriteId)
{
    int var = 0x40 - GetBattlerYDeltaFromSpriteId(otherSpriteId) * 2;
    u16 matrix = gSprites[spriteId].oam.matrixNum;
    int var2 = (var << 8) / gOamMatrices[matrix].d;

    if (var2 > 0x80)
        var2 = 0x80;
    gSprites[spriteId].y2 = (var - var2) / 2;
}

static u16 GetBattlerYDeltaFromSpriteId(u8 spriteId)
{
    struct BattleSpriteInfo *spriteInfo;
    u8 battler = gSprites[spriteId].data[0];
    u16 species;
    u16 i;

    for (i = 0; i < (sizeof(gBattlerSpriteIds) / sizeof(u8)); i++)
    {
        if (gBattlerSpriteIds[i] == spriteId)
        {
            if (IsContest())
            {
                species = gContestResources__moveAnim.species;
                return gMonBackPicCoords[species].y_offset;
            }
            else
            {
                if (!GetBattlerSide(i))
                {
                    spriteInfo = &gBattleSpriteInfo[battler];
                    if (!spriteInfo->transformSpecies)
                        species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[i]], MON_DATA_SPECIES);
                    else
                        species = spriteInfo->transformSpecies;
                    return gMonBackPicCoords[species].y_offset;
                }
                else
                {
                    spriteInfo = &gBattleSpriteInfo[battler];
                    if (!spriteInfo->transformSpecies)
                        species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[i]], MON_DATA_SPECIES);
                    else
                        species = spriteInfo->transformSpecies;
                    return gMonFrontPicCoords[species].y_offset;
                }
            }
        }
    }
    return 0x40;
}

void StorePointerInVars(s16 *lo, s16 *hi, const void *ptr)
{
    *lo = ((intptr_t) ptr) & 0xffff;
    *hi = (((intptr_t) ptr) >> 16) & 0xffff;
}

void *LoadPointerFromVars(s16 lo, s16 hi)
{
    return (void *)((u16)lo | ((u16)hi << 16));
}

// possible new file

void PrepareEruptAnimTaskData(struct Task *task, u8 spriteId, s16 xScaleStart, s16 yScaleStart, s16 xScaleEnd, s16 yScaleEnd, u16 duration)
{
    task->data[8] = duration;
    task->data[15] = spriteId; // spriteId
    task->data[9] = xScaleStart;
    task->data[10] = yScaleStart;
    task->data[13] = xScaleEnd;
    task->data[14] = yScaleEnd;
    task->data[11] = (xScaleEnd - xScaleStart) / duration;
    task->data[12] = (yScaleEnd - yScaleStart) / duration;
}

u8 UpdateEruptAnimTask(struct Task *task)
{
    if (!task->data[8])
        return 0;
        
    if (--task->data[8] != 0)
    {
        task->data[9] += task->data[11];
        task->data[10] += task->data[12];
    }
    else
    {
        task->data[9] = task->data[13];
        task->data[10] = task->data[14];
    }
    SetSpriteRotScale(task->data[15], task->data[9], task->data[10], 0);
    if (task->data[8])
        SetBattlerSpriteYOffsetFromYScale(task->data[15]);
    else
        gSprites[task->data[15]].y2 = 0;
    return task->data[8];
}

void AnimTask_GetFrustrationPowerLevel(u8 taskId)
{
    u16 powerLevel;

    if (gAnimFriendship <= 30)
        powerLevel = 0;
    else if (gAnimFriendship <= 100)
        powerLevel = 1;
    else if (gAnimFriendship <= 200)
        powerLevel = 2;
    else
        powerLevel = 3;
    gBattleAnimArgs[7] = powerLevel;
    DestroyAnimVisualTask(taskId);
}

static void UNUSED SetPriorityForVisibleBattlers(u8 priority)
{
    if (IsBattlerSpriteVisible(gBattleAnimTarget))
        gSprites[gBattlerSpriteIds[gBattleAnimTarget]].oam.priority = priority;
    if (IsBattlerSpriteVisible(gBattleAnimAttacker))
        gSprites[gBattlerSpriteIds[gBattleAnimAttacker]].oam.priority = priority;
    if (IsBattlerSpriteVisible(gBattleAnimTarget ^ 2))
        gSprites[gBattlerSpriteIds[gBattleAnimTarget ^ 2]].oam.priority = priority;
    if (IsBattlerSpriteVisible(gBattleAnimAttacker ^ 2))
        gSprites[gBattlerSpriteIds[gBattleAnimAttacker ^ 2]].oam.priority = priority;
}

void InitPrioritiesForVisibleBattlers(void)
{
    int i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (IsBattlerSpriteVisible(i))
        {
            gSprites[gBattlerSpriteIds[i]].subpriority = GetBattlerSpriteSubpriority(i);
            gSprites[gBattlerSpriteIds[i]].oam.priority = 2;
        }
    }
}

u8 GetBattlerSpriteSubpriority(u8 battler)
{
    u8 position;
    u8 subpriority;

    if (IsContest())
    {
        if (battler == ANIM_ATK_PARTNER)
            return 30;
        else
            return 40;
    }
    else
    {
        position = GetBattlerPosition(battler);
        if (position == B_POSITION_PLAYER_LEFT)
            subpriority = 30;
        else if (position == B_POSITION_PLAYER_RIGHT)
            subpriority = 20;
        else if (position == B_POSITION_OPPONENT_LEFT)
            subpriority = 40;
        else
            subpriority = 50;
    }
    return subpriority;
}

u8 GetBattlerSpriteBGPriority(u8 battler)
{
    u8 position = GetBattlerPosition(battler);

    if (IsContest())
        return 2;
    if (position == 0 || position == 3)
        return BG2CNT.priority;
    else
        return BG1CNT.priority;
}

u8 GetBattlerSpriteBGPriorityRank(u8 battler)
{
    u8 position;

    if (!IsContest())
    {
        position = GetBattlerPosition(battler);
        if (position == 0 || position == 3)
            return 2;
        else
            return 1;
    }
    return 1;
}

u8 CreateAdditionalMonSpriteForMoveAnim(u16 species, bool8 isBackpic, u8 id, s16 x, s16 y, u8 subpriority, u32 personality, u32 trainerId)
{
    u8 spriteId;
    u16 sheet = LoadSpriteSheet(&sSpriteSheets_MoveEffectMons[id]);
    u16 palette = AllocSpritePalette(sSpriteTemplates_MoveEffectMons[id].paletteTag);

    if (!isBackpic)
    {
        LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(species, trainerId, personality), (palette * 0x10) + 0x100, 0x20);
        LoadSpecialPokePic(
            &gMonFrontPicTable[species],
            gMonFrontPicCoords[species].size,
            gMonFrontPicCoords[species].y_offset,
            (void *)EWRAM,
            (void *)EWRAM,
            species,
            personality,
            1
        );
    }
    else
    {
        LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(species, trainerId, personality), (palette * 0x10) + 0x100, 0x20);
        LoadSpecialPokePic(
            &gMonBackPicTable[species],
            gMonBackPicCoords[species].size,
            gMonBackPicCoords[species].y_offset,
            (void *)EWRAM,
            (void *)EWRAM,
            species,
            personality,
            0
        );
    }

    DmaCopy32Defvars(3, (void *)EWRAM, (void *)(OBJ_VRAM0 + (sheet * 0x20)), 0x800);

    if (!isBackpic)
        spriteId = CreateSprite(&sSpriteTemplates_MoveEffectMons[id], x, y + gMonFrontPicCoords[species].y_offset, subpriority);
    else
        spriteId = CreateSprite(&sSpriteTemplates_MoveEffectMons[id], x, y + gMonBackPicCoords[species].y_offset, subpriority);

    if (IsContest())
    {
        gSprites[spriteId].affineAnims = gAffineAnims_BattleSpriteContest;
        StartSpriteAffineAnim(&gSprites[spriteId], 0);
    }
    return spriteId;
}

void DestroySpriteAndFreeResources_(struct Sprite *sprite)
{
    DestroySpriteAndFreeResources(sprite);
}

s16 GetBattlerSpriteCoordAttr(u8 battler, u8 attr)
{
    u16 species;
    u32 personality;
    u16 letter;
    u16 unownSpecies;
    int ret;
    const struct MonCoords *coords;
    struct BattleSpriteInfo *spriteInfo;

    if (IsContest())
    {
        if (gContestResources__moveAnim.hasTargetAnim)
        {
            species = gContestResources__moveAnim.targetSpecies;
            personality = gContestResources__moveAnim.targetPersonality;
        }
        else
        {
            species = gContestResources__moveAnim.species;
            personality = gContestResources__moveAnim.personality;
        }
        if (species == SPECIES_UNOWN)
        {
            letter = GET_UNOWN_LETTER(personality);
            if (!letter)
                unownSpecies = SPECIES_UNOWN;
            else
                unownSpecies = letter + SPECIES_UNOWN_B - 1;
            coords = &gMonBackPicCoords[unownSpecies];
        }
        else if (species == SPECIES_CASTFORM)
        {
            coords = &gCastformFrontSpriteCoords[gBattleMonForms[battler]];
        }
        else if (species <= SPECIES_EGG)
        {
            coords = &gMonBackPicCoords[species];
        }
        else
        {
            coords = &gMonBackPicCoords[0];
        }
    }
    else
    {
        if (!GetBattlerSide(battler))
        {
            spriteInfo = &gBattleSpriteInfo[battler];
            if (!spriteInfo->transformSpecies)
            {
                species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
                personality = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);
            }
            else
            {
                species = spriteInfo->transformSpecies;
                personality = gTransformedPersonalities[battler];
            }
            if (species == SPECIES_UNOWN)
            {
                letter = GET_UNOWN_LETTER(personality);
                if (!letter)
                    unownSpecies = SPECIES_UNOWN;
                else
                    unownSpecies = letter + SPECIES_UNOWN_B - 1;
                coords = &gMonBackPicCoords[unownSpecies];
            }
            else if (species > SPECIES_EGG)
            {
                coords = &gMonBackPicCoords[0];
            }
            else
            {
                coords = &gMonBackPicCoords[species];
            }
        }
        else
        {
            spriteInfo = &gBattleSpriteInfo[battler];
            if (!spriteInfo->transformSpecies)
            {
                species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
                personality = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);
            }
            else
            {
                species = spriteInfo->transformSpecies;
                personality = gTransformedPersonalities[battler];
            }
            if (species == SPECIES_UNOWN)
            {
                letter = GET_UNOWN_LETTER(personality);
                if (!letter)
                    unownSpecies = SPECIES_UNOWN;
                else
                    unownSpecies = letter + SPECIES_UNOWN_B - 1;
                coords = &gMonFrontPicCoords[unownSpecies];
            }
            else if (species == SPECIES_CASTFORM)
            {
                coords = &gCastformFrontSpriteCoords[gBattleMonForms[battler]];
            }
            else if (species > SPECIES_EGG)
            {
                coords = &gMonFrontPicCoords[0];
            }
            else
            {
                coords = &gMonFrontPicCoords[species];
            }
        }
    }

    switch (attr)
    {
    case 0:
        return (coords->size & 0xf) * 8;
    case 1:
        return (coords->size >> 4) * 8;
    case 4:
        return GetBattlerSpriteCoord(battler, 2) - ((coords->size >> 4) * 4);
    case 5:
        return GetBattlerSpriteCoord(battler, 2) + ((coords->size >> 4) * 4);
    case 2:
        return GetBattlerSpriteCoord(battler, 3) - ((coords->size & 0xf) * 4);
    case 3:
        return GetBattlerSpriteCoord(battler, 3) + ((coords->size & 0xf) * 4);
    case 6:
        ret = GetBattlerSpriteCoord(battler, 1) + 0x1f;
        return ret - coords->y_offset;
    default:
        return 0;
    }
}

void SetAverageBattlerPositions(u8 battler, bool8 respectMonPicOffsets, s16 *x, s16 *y)
{
    u8 xCoordType, yCoordType;
    s16 battlerX, battlerY;
    s16 partnerX, partnerY;

    if (!respectMonPicOffsets)
    {
        xCoordType = 0;
        yCoordType = 1;
    }
    else
    {
        xCoordType = 2;
        yCoordType = 3;
    }
    battlerX = GetBattlerSpriteCoord(battler, xCoordType);
    battlerY = GetBattlerSpriteCoord(battler, yCoordType);
    if (IsDoubleBattle() && !IsContest())
    {
        partnerX = GetBattlerSpriteCoord(battler ^ 2, xCoordType);
        partnerY = GetBattlerSpriteCoord(battler ^ 2, yCoordType);
    }
    else
    {
        partnerX = battlerX;
        partnerY = battlerY;
    }
    *x = (battlerX + partnerX) / 2;
    *y = (battlerY + partnerY) / 2;
}

u8 CreateInvisibleSpriteCopy(int battler, u8 spriteId, int species)
{
    u8 newSpriteId = CreateInvisibleSpriteWithCallback(SpriteCallbackDummy);
    gSprites[newSpriteId] = gSprites[spriteId];
    gSprites[newSpriteId].usingSheet = TRUE;
    gSprites[newSpriteId].oam.priority = 0;
    gSprites[newSpriteId].oam.objMode = 2;
    gSprites[newSpriteId].oam.tileNum = gSprites[spriteId].oam.tileNum;
    gSprites[newSpriteId].callback = SpriteCallbackDummy;
    return newSpriteId;
}

// unused_orb

void AnimTranslateLinearAndFlicker_Flipped(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    if (GetBattlerSide(gBattleAnimAttacker))
    {
        sprite->x -= gBattleAnimArgs[0];
        gBattleAnimArgs[3] = -gBattleAnimArgs[3];
        sprite->hFlip = TRUE;
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
    }
    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[3] = gBattleAnimArgs[4];
    sprite->data[5] = gBattleAnimArgs[5];
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = TranslateSpriteLinearAndFlicker;
}

void AnimTranslateLinearAndFlicker(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker))
    {
        sprite->x -= gBattleAnimArgs[0];
        gBattleAnimArgs[3] *= -1;
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
    }
    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[3] = gBattleAnimArgs[4];
    sprite->data[5] = gBattleAnimArgs[5];
    StartSpriteAnim(sprite, gBattleAnimArgs[6]);
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = TranslateSpriteLinearAndFlicker;
}

// file_2

void AnimSpinningSparkle(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    if (GetBattlerSide(gBattleAnimAttacker))
        sprite->x -= gBattleAnimArgs[0];
    else
        sprite->x += gBattleAnimArgs[0];
    sprite->y += gBattleAnimArgs[1];
    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// file_3 (punch effect?)

void AnimTask_AttackerPunchWithTrace(u8 taskId)
{
    u16 src;
    u16 dest;
    struct Task *task = &gTasks[taskId];
    task->data[0] = GetAnimBattlerSpriteId(0);
    task->data[1] = (GetBattlerSide(gBattleAnimAttacker)) ? -8 : 8;
    task->data[2] = 0;
    task->data[3] = 0;
    gSprites[task->data[0]].x2 -= task->data[0];
    task->data[4] = AllocSpritePalette(10097);
    task->data[5] = 0;

    dest = (task->data[4] + 0x10) * 0x10;
    src = (gSprites[task->data[0]].oam.paletteNum + 0x10) * 0x10;
    task->data[6] = GetBattlerSpriteSubpriority(gBattleAnimAttacker);
    if (task->data[6] == 20 || task->data[6] == 40)
        task->data[6] = 2;
    else
        task->data[6] = 3;
    CpuCopy32(&gPlttBufferUnfaded[src], &gPlttBufferFaded[dest], 0x20);
    BlendPalette(dest, 16, gBattleAnimArgs[1], gBattleAnimArgs[0]);
    task->func = AnimTask_AttackerPunchWithTrace_Step;
}

static void AnimTask_AttackerPunchWithTrace_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[2])
    {
    case 0:
        CreateBattlerTrace(task, taskId);
        gSprites[task->data[0]].x2 += task->data[1];
        if (++task->data[3] == 5)
        {
            task->data[3]--;
            task->data[2]++;
        }
        break;
    case 1:
        CreateBattlerTrace(task, taskId);
        gSprites[task->data[0]].x2 -= task->data[1];
        if (--task->data[3] == 0)
        {
            gSprites[task->data[0]].x2 = 0;
            task->data[2]++;
        }
        break;
    case 2:
        if (!task->data[5])
        {
            FreeSpritePaletteByTag(10097);
            DestroyAnimVisualTask(taskId);
        }
        break;
    }
}

static void CreateBattlerTrace(struct Task *task, u8 taskId)
{
    s16 spriteId = CloneBattlerSpriteWithBlend(0);
    if (spriteId >= 0)
    {
        gSprites[spriteId].oam.priority = task->data[6];
        gSprites[spriteId].oam.paletteNum = task->data[4];
        gSprites[spriteId].data[0] = 8;
        gSprites[spriteId].data[1] = taskId;
        gSprites[spriteId].data[2] = spriteId;
        gSprites[spriteId].x2 = gSprites[task->data[0]].x2;
        gSprites[spriteId].callback = AnimBattlerTrace;
        task->data[5]++;
    }
}

static void AnimBattlerTrace(struct Sprite *sprite)
{
    if (--sprite->data[0] == 0)
    {
        gTasks[sprite->data[1]].data[5]--;
        DestroySpriteWithActiveSheet(sprite);
    }
}

// file_4

void AnimWeatherBallUp(struct Sprite *sprite) {
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    if (!GetBattlerSide(gBattleAnimAttacker))
        sprite->data[0] = 5;
    else
        sprite->data[0] = -10;
    sprite->data[1] = -40;
    sprite->callback = AnimWeatherBallUp_Step;
}

static void AnimWeatherBallUp_Step(struct Sprite *sprite)
{
    sprite->data[2] += sprite->data[0];
    sprite->data[3] += sprite->data[1];
    sprite->x2 = sprite->data[2] / 10;
    sprite->y2 = sprite->data[3] / 10;
    if (sprite->data[1] < -20)
        sprite->data[1]++;
    if (sprite->y + sprite->y2 < -32)
        DestroyAnimSprite(sprite);
}

void AnimWeatherBallDown(struct Sprite *sprite)
{
    int x;
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[2] = sprite->x + gBattleAnimArgs[4];
    sprite->data[4] = sprite->y + gBattleAnimArgs[5];
    if (!GetBattlerSide(gBattleAnimTarget))
    {
        x = (u16)gBattleAnimArgs[4] + 30;
        sprite->x += x;
        sprite->y = gBattleAnimArgs[5] - 20;
    }
    else
    {
        x = (u16)gBattleAnimArgs[4] - 30;
        sprite->x += x;
        sprite->y = gBattleAnimArgs[5] - 80;
    }
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
