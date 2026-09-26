#include "global.h"
#include "constants/battle_anim.h"
#include "constants/species.h"
#include "rom_8077ABC.h"
#include "battle.h"
#include "battle_anim.h"
#include "blend_palette.h"
#include "contest.h"
#include "data2.h"
#include "decompress.h"
#include "palette.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "util.h"
#include "ewram.h"

#define GET_UNOWN_LETTER(personality) ((\
      (((personality & 0x03000000) >> 24) << 6) \
    | (((personality & 0x00030000) >> 16) << 4) \
    | (((personality & 0x00000300) >> 8) << 2) \
    | (((personality & 0x00000003) >> 0) << 0) \
) % 28)

#define IS_DOUBLE_BATTLE() ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? TRUE : FALSE)

#define NUM_BATTLE_SLOTS 4

#define gBattleMonPartyPositions gBattlerPartyIndexes
#define gCastformElevations gUnknownCastformData_0837F5A8
#define gCastformBackSpriteYCoords gUnknown_0837F5AC
#define gTransformPersonalities gTransformedPersonalities
#define gBattleMonSprites gBattlerSpriteIds

struct Struct_gUnknown_0837F578
{
    u8 field_0;
    u8 field_1;
};

struct Struct_2017810
{
    u8 filler_0[6];
    u8 field_6;
    u8 filler_7[5];
};

#define BG1CNT (*(vBgCnt *)REG_ADDR_BG1CNT)
#define BG2CNT (*(vBgCnt *)REG_ADDR_BG2CNT)
#define BG3CNT (*(vBgCnt *)REG_ADDR_BG3CNT)

extern const union AnimCmd *const gDummySpriteAnimTable[];
extern const union AffineAnimCmd *const gDummySpriteAffineAnimTable[];

extern u16 gBattleMonPartyPositions[];
extern u16 gBattleTypeFlags;
extern u32 gTransformPersonalities[NUM_BATTLE_SLOTS];
extern u8 gBattleMonForms[NUM_BATTLE_SLOTS];
extern u16 gAnimSpeciesByBanks[];
extern u8 gBattleMonSprites[NUM_BATTLE_SLOTS];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern s16 gBattleAnimArgs[8];
extern u8 gBattlerPositions[NUM_BATTLE_SLOTS];
extern u8 gBattlersCount; // gNumBattleMons?
extern struct OamMatrix gOamMatrices[];
extern struct Struct_2017810 unk_2017810[];
extern u8 gAnimFriendship;

extern u8 UpdateMonIconFrame(struct Sprite *sprite);

static void AnimTask_BlendPalInAndOutSetup(struct Task *task);
static void AnimTask_BlendMonInAndOut_Step(u8 taskId);
static void AnimThrowProjectile_Step(struct Sprite *sprite);

EWRAM_DATA union AffineAnimCmd *gUnknown_0202F7D4 = NULL;
EWRAM_DATA u32 filler_0202F7D8[3] = {0};

const struct Struct_gUnknown_0837F578 gUnknown_0837F578[][4] =
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
// Coords are probably front pic coords or back pic coords, but this data does not seem to be
// used during battle, party summary, or pokedex screens.
const struct MonCoords gCastformFrontSpriteCoords[] =
{
    { 0x44, 17 }, // NORMAL
    { 0x66, 9 }, // SUN
    { 0x46, 9 }, // RAIN
    { 0x86, 8 }, // HAIL
};

const u8 gCastformElevations[] =
{
    13, // NORMAL
    14, // SUN
    13, // RAIN
    13, // HAIL
};

// Y position of the backsprite for each of the four Castform forms.
const u8 gCastformBackSpriteYCoords[] =
{
    0, // NORMAL
    0, // SUN
    0, // RAIN
    0, // HAIL
};

const struct SpriteTemplate gSpriteTemplate_837F5B0[] =
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

const struct SpriteSheet gUnknown_0837F5E0[] =
{
    { gMiscBlank_Gfx, 0x800, 55125, },
    { gMiscBlank_Gfx, 0x800, 55126, },
};

// pkmn_form.c

u8 GetBattlerSpriteCoord(u8 slot, u8 a2)
{
    u8 var;
    u16 species;
    struct BattleSpriteInfo *transform;

    if (IsContest())
    {
        if (a2 == 3 && slot == 3)
            a2 = 1;
    }
    switch (a2)
    {
    case 0:
    case 2:
        var = gUnknown_0837F578[IS_DOUBLE_BATTLE()][GetBattlerPosition(slot)].field_0;
        break;
    case 1:
        var = gUnknown_0837F578[IS_DOUBLE_BATTLE()][GetBattlerPosition(slot)].field_1;
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
            if (GetBattlerSide(slot))
            {
                transform = &gBattleSpriteInfo[slot];
                if (!transform->transformSpecies)
                    species = GetMonData(&gEnemyParty[gBattleMonPartyPositions[slot]], MON_DATA_SPECIES);
                else
                    species = transform->transformSpecies;
            }
            else
            {
                transform = &gBattleSpriteInfo[slot];
                if (!transform->transformSpecies)
                    species = GetMonData(&gPlayerParty[gBattleMonPartyPositions[slot]], MON_DATA_SPECIES);
                else
                    species = transform->transformSpecies;
            }
        }
        if (a2 == 3)
            var = GetBattlerSpriteFinal_Y(slot, species, 1);
        else
            var = GetBattlerSpriteFinal_Y(slot, species, 0);
        break;
    }
    return var;
}

u8 sub_8077BFC(u8 slot, u16 species)
{
    u16 letter;
    u32 personality;
    struct BattleSpriteInfo *transform;
    u8 ret;
    u16 var;

    if (GetBattlerSide(slot) == 0 || IsContest())
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
                transform = &gBattleSpriteInfo[slot];
                if (!transform->transformSpecies)
                    personality = GetMonData(&gPlayerParty[gBattleMonPartyPositions[slot]], MON_DATA_PERSONALITY);
                else
                    personality = gTransformPersonalities[slot];
            }
            letter = GET_UNOWN_LETTER(personality);
            if (!letter)
                var = species;
            else
                var = letter + SPECIES_UNOWN_B - 1;
            ret = gMonBackPicCoords[var].y_offset;
        }
        else if (species == SPECIES_CASTFORM)
        {
            ret = gCastformBackSpriteYCoords[gBattleMonForms[slot]];
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
            transform = &gBattleSpriteInfo[slot];
            if (!transform->transformSpecies)
                personality = GetMonData(&gEnemyParty[gBattleMonPartyPositions[slot]], MON_DATA_PERSONALITY);
            else
                personality = gTransformPersonalities[slot];
            letter = GET_UNOWN_LETTER(personality);
            if (!letter)
                var = species;
            else
                var = letter + SPECIES_UNOWN_B - 1;
            ret = gMonFrontPicCoords[var].y_offset;
        }
        else if (species == SPECIES_CASTFORM)
        {
            ret = gCastformFrontSpriteCoords[gBattleMonForms[slot]].y_offset;
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

u8 sub_8077DD8(u8 slot, u16 species)
{
    u8 ret = 0;
    if (GetBattlerSide(slot) == 1)
    {
        if (!IsContest())
        {
            if (species == SPECIES_CASTFORM)
                ret = gCastformElevations[gBattleMonForms[slot]];
            else if (species > NUM_SPECIES)
                ret = gEnemyMonElevation[0];
            else
                ret = gEnemyMonElevation[species];
        }
    }
    return ret;
}

u8 GetBattlerSpriteFinal_Y(u8 slot, u16 species, u8 a3)
{
    u16 offset;
    u8 y;

    if (GetBattlerSide(slot) == 0 || IsContest())
    {
        offset = sub_8077BFC(slot, species);
    }
    else
    {
        offset = sub_8077BFC(slot, species);
        offset -= sub_8077DD8(slot, species);
    }
    y = offset + gUnknown_0837F578[IS_DOUBLE_BATTLE()][GetBattlerPosition(slot)].field_1;
    if (a3)
    {
        if (GetBattlerSide(slot) == 0)
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
    if (coordType == BATTLER_COORD_Y_PIC_OFFSET || coordType == BATTLER_COORD_Y_PIC_OFFSET_DEFAULT)
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
                species = gAnimSpeciesByBanks[battler];
            else
                species = spriteInfo->transformSpecies;
        }
        if (coordType == BATTLER_COORD_Y_PIC_OFFSET)
            return GetBattlerSpriteFinal_Y(battler, species, TRUE);
        else
            return GetBattlerSpriteFinal_Y(battler, species, FALSE);
    }
    else
    {
        return GetBattlerSpriteCoord(battler, coordType);
    }
}

u8 GetBattlerSpriteDefault_Y(u8 slot)
{
    return GetBattlerSpriteCoord(slot, 4);
}

u8 GetSubstituteSpriteDefault_Y(u8 battler)
{
    u16 y;
    if (GetBattlerSide(battler) != B_SIDE_PLAYER)
        y = GetBattlerSpriteCoord(battler, BATTLER_COORD_Y) + 16;
    else
        y = GetBattlerSpriteCoord(battler, BATTLER_COORD_Y) + 17;
    return y;
}

u8 GetBattlerYCoordWithElevation(u8 battler)
{
    u16 var;
    u8 r6;
    struct BattleSpriteInfo *transform;

    r6 = GetBattlerSpriteCoord(battler, 1);
    if (!IsContest())
    {
        if (GetBattlerSide(battler) != 0)
        {
            transform = &gBattleSpriteInfo[battler];
            if (!transform->transformSpecies) {
                var = GetMonData(&gEnemyParty[gBattleMonPartyPositions[battler]], MON_DATA_SPECIES);
            } else {
                var = transform->transformSpecies;
            }
        }
        else
        {
            transform = &gBattleSpriteInfo[battler];
            if (!transform->transformSpecies)
                var = GetMonData(&gPlayerParty[gBattleMonPartyPositions[battler]], MON_DATA_SPECIES);
            else
                var = transform->transformSpecies;
        }
        if (GetBattlerSide(battler) != 0)
            r6 -= sub_8077DD8(battler, var);
    }
    return r6;
}

u8 GetAnimBattlerSpriteId(u8 whichBank)
{
    u8 *sprites;

    if (whichBank == ANIM_BATTLER_ATTACKER)
    {
        if (IsBankSpritePresent(gBattleAnimAttacker))
        {
            sprites = gBattleMonSprites;
            return sprites[gBattleAnimAttacker];
        }
        else
        {
            return 0xff;
        }
    }
    else if (whichBank == ANIM_BATTLER_TARGET)
    {
        if (IsBankSpritePresent(gBattleAnimTarget))
        {
            sprites = gBattleMonSprites;
            return sprites[gBattleAnimTarget];
        }
        else
        {
            return 0xff;
        }
    }
    else if (whichBank == ANIM_BATTLER_ATK_PARTNER)
    {
        if (!IsAnimBankSpriteVisible(gBattleAnimAttacker ^ 2))
            return 0xff;
        else
            return gBattleMonSprites[gBattleAnimAttacker ^ 2];
    }
    else
    {
        if (IsAnimBankSpriteVisible(gBattleAnimTarget ^ 2))
            return gBattleMonSprites[gBattleAnimTarget ^ 2];
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

void unref_sub_80781F0(struct Sprite *sprite)
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

void sub_8078278(struct Sprite *sprite)
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

void sub_80782F8(struct Sprite *sprite)
{
    sub_8078314(sprite);
    sprite->callback = TranslateSpriteLinear;
    sprite->callback(sprite);
}

void sub_8078314(struct Sprite *sprite)
{
    s16 old;
    int v1;

    if (sprite->data[1] > sprite->data[2])
        sprite->data[0] = -sprite->data[0];
    v1 = sprite->data[2] - sprite->data[1];
    old = sprite->data[0];
    sprite->data[0] = abs(v1 / sprite->data[0]);
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

void sub_80783D0(struct Sprite *sprite)
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

void unref_sub_8078414(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x + sprite->x2;
    sprite->data[3] = sprite->y + sprite->y2;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->callback = sub_80782F8;
}

void TranslateMonBGUntil(struct Sprite *sprite)
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
void TranslateMonBGSubPixelUntil(struct Sprite *sprite)
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

void unref_sub_8078588(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x + sprite->x2;
    sprite->data[3] = sprite->y + sprite->y2;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    sprite->callback = sub_80782F8;
}

void unref_sub_80785CC(struct Sprite *sprite)
{
    ResetPaletteStructByUid(sprite->data[5]);
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

void sub_8078634(u8 task)
{
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    DestroyAnimVisualTask(task);
}

void SetSpriteCoordsToAnimAttackerCoords(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
}

void SetAnimSpriteInitialXOffset(struct Sprite *sprite, s16 xOffset)
{
    u16 attackerX = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X);
    u16 targetX = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X);

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
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
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
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_X);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_Y);
    }
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}

void InitSpritePosToAnimAttacker(struct Sprite *sprite, bool8 respectMonPicOffsets)
{
    if (!respectMonPicOffsets)
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_X);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_Y);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    }
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}

u8 GetBattlerSide(u8 slot)
{
    return gBattlerPositions[slot] & 1;
}

u8 GetBattlerPosition(u8 slot)
{
    return gBattlerPositions[slot];
}

u8 GetBattlerAtPosition(u8 slot)
{
    u8 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattlerPositions[i] == slot)
            break;
    }
    return i;
}

bool8 IsBankSpritePresent(u8 slot)
{
    if (IsContest())
    {
        if (gBattleAnimAttacker == slot)
            return TRUE;
        if (gBattleAnimTarget == slot)
            return TRUE;
        return FALSE;
    }
    else
    {
        if (gBattlerPositions[slot] == 0xff)
            return FALSE;
        if (GetBattlerSide(slot) != B_SIDE_PLAYER)
        {
            if (GetMonData(&gEnemyParty[gBattleMonPartyPositions[slot]], MON_DATA_HP) != 0)
                return TRUE;
        }
        else
        {
            if (GetMonData(&gPlayerParty[gBattleMonPartyPositions[slot]], MON_DATA_HP) != 0)
                return TRUE;
        }
        return FALSE;
    }
}

bool8 IsDoubleBattle()
{
    return IS_DOUBLE_BATTLE();
}

void GetBattleAnimBg1Data(struct BattleAnimBgData *animBg)
{
    if (IsContest())
    {
        animBg->bgTiles = (u8 *)(VRAM + 0x8000);
        animBg->bgTilemap = (u8 *)(VRAM + 0xf000);
        animBg->paletteId = 0xe;
    }
    else
    {
        animBg->bgTiles = (u8 *)(VRAM + 0x4000);
        animBg->bgTilemap = (u8 *)(VRAM + 0xe000);
        animBg->paletteId = 0x8;
    }
}

void GetBgDataForTransform(struct BattleAnimBgData *animBg, u8 battler)
{
    if (IsContest())
    {
        animBg->bgTiles = (u8 *)(VRAM + 0x8000);
        animBg->bgTilemap = (u8 *)(VRAM + 0xf000);
        animBg->paletteId = 0xe;
    }
    else if (GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) == 1)
    {
        animBg->bgTiles = (u8 *)(VRAM + 0x4000);
        animBg->bgTilemap = (u8 *)(VRAM + 0xe000);
        animBg->paletteId = 0x8;
    }
    else
    {
        animBg->bgTiles = (u8 *)(VRAM + 0x6000);
        animBg->bgTilemap = (u8 *)(VRAM + 0xf000);
        animBg->paletteId = 0x9;
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

void sub_8078A34(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitSpriteDataForLinearTranslation(sprite);
    sprite->callback = sub_80783D0;
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
    int xDelta = abs(sprite->data[2] - sprite->data[1]) << 8;
    sprite->data[0] = xDelta / sprite->data[0];
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

void InitAnimFastLinearTranslation(struct Sprite *sprite)
{
    int x = sprite->data[2] - sprite->data[1];
    int y = sprite->data[4] - sprite->data[3];
    bool8 x_sign = x < 0;
    bool8 y_sign = y < 0;
    u16 x2 = abs(x) << 4;
    u16 y2 = abs(y) << 4;

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

void AnimFastTranslateLinearWaitEnd(struct Sprite *sprite)
{
    if (AnimFastTranslateLinear(sprite))
        SetCallbackToStoredInData6(sprite);
}

void InitAnimFastLinearTranslationWithSpeed(struct Sprite *sprite)
{
    int v1 = abs(sprite->data[2] - sprite->data[1]) << 4;
    sprite->data[0] = v1 / sprite->data[0];
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

bool8 ShouldRotScaleSpeciesBeFlipped(void)
{
    if (IsContest())
    {
        if (gSprites[GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER)].data[2] == SPECIES_UNOWN)
            return FALSE;
        return TRUE;
    }
    return FALSE;
}

void PrepareBattlerSpriteForRotScale(u8 spriteId, u8 objMode)
{
    u8 battler = gSprites[spriteId].data[0];

    if (IsContest() || IsAnimBankSpriteVisible(battler))
        gSprites[spriteId].invisible = FALSE;
    gSprites[spriteId].oam.objMode = objMode;
    gSprites[spriteId].affineAnimPaused = TRUE;
    if (!IsContest() && !gSprites[spriteId].oam.affineMode)
        gSprites[spriteId].oam.matrixNum = gBattleHealthBoxInfo[battler].unk6;
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_DOUBLE;
    CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
}

void ResetSpriteRotScale(u8 spriteId)
{
    SetSpriteRotScale(spriteId, 0x100, 0x100, 0);
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
    gSprites[spriteId].oam.objMode = ST_OAM_OBJ_NORMAL;
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

// related to SetSpriteRotScale
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

static u16 ArcTan2_(s16 a, s16 b)
{
    return ArcTan2(a, b);
}

u16 ArcTan2Neg(s16 a, s16 b)
{
    u16 var = ArcTan2_(a, b);
    return -var;
}

void SetGrayscaleOrOriginalPalette(u16 paletteNum, bool8 restoreOriginalColor)
{
    int i;
    struct PlttData *originalColor;
    struct PlttData *destColor;
    u16 average;
    u16 paletteOffset;

    paletteOffset = paletteNum * 0x10;

    if (!restoreOriginalColor)
    {
        for (i = 0; i < 0x10; i++)
        {
            originalColor = (struct PlttData *)&gPlttBufferUnfaded[paletteOffset + i];
            average = originalColor->r + originalColor->g + originalColor->b;
            average /= 3;

            destColor = (struct PlttData *)&gPlttBufferFaded[paletteOffset + i];
            destColor->r = average;
            destColor->g = average;
            destColor->b = average;
        }
    }
    else
    {
        CpuCopy32(&gPlttBufferUnfaded[paletteOffset], &gPlttBufferFaded[paletteOffset], 0x20);
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
        if (IsAnimBankSpriteVisible(gBattleAnimAttacker ^ 2))
        {
            shift = (gBattleAnimAttacker ^ 2) + 16;
            selectedPalettes |= 1 << shift;
        }
    }
    if (targetPartner)
    {
        if (IsAnimBankSpriteVisible(gBattleAnimTarget ^ 2))
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
            if (IsAnimBankSpriteVisible(GetBattlerAtPosition(0))) {
                selectedPalettes |= 1 << (GetBattlerAtPosition(0) + 16);
            }
        }
        if (playerRight) {
            if (IsAnimBankSpriteVisible(GetBattlerAtPosition(2))) {
                shift = GetBattlerAtPosition(2) + 16;
                selectedPalettes |= 1 << shift;
            }
        }
        if (opponentLeft) {
            if (IsAnimBankSpriteVisible(GetBattlerAtPosition(1))) {
                shift = GetBattlerAtPosition(1) + 16;
                selectedPalettes |= 1 << shift;
            }
        }
        if (opponentRight) {
            if (IsAnimBankSpriteVisible(GetBattlerAtPosition(3))) {
                shift = GetBattlerAtPosition(3) + 16;
                selectedPalettes |= 1 << shift;
            }
        }
    }
    return selectedPalettes;
}

u8 sub_80793A8(u8 a1)
{
    return a1;
}

u8 unref_sub_80793B0(u8 a1)
{
    return GetBattlerAtPosition(a1);
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
    bool8 v1;
    u8 v2;

    if (!(gBattleAnimArgs[5] & 0xff00))
        v1 = TRUE;
    else
        v1 = FALSE;

    if (!(gBattleAnimArgs[5] & 0xff))
        v2 = 3;
    else
        v2 = 1;

    InitSpritePosToAnimAttacker(sprite, v1);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, v2) + gBattleAnimArgs[3];
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimThrowProjectile(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, TRUE);
    if (GetBattlerSide(gBattleAnimAttacker))
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + gBattleAnimArgs[3];
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
    bool8 r4;
    u8 slot, r7;

    if (!gBattleAnimArgs[6])
    {
        r4 = TRUE;
        r7 = 3;
    }
    else
    {
        r4 = FALSE;
        r7 = 1;
    }
    if (!gBattleAnimArgs[5])
    {
        InitSpritePosToAnimAttacker(sprite, r4);
        slot = gBattleAnimAttacker;
    }
    else
    {
        InitSpritePosToAnimTarget(sprite, r4);
        slot = gBattleAnimTarget;
    }
    if (GetBattlerSide(gBattleAnimAttacker))
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    InitSpritePosToAnimTarget(sprite, r4);
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(slot, 2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(slot, r7) + gBattleAnimArgs[3];
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

static void AnimTask_AlphaFadeIn_Step(u8 taskId);

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

void PrepareAffineAnimInTaskData(struct Task *task, u8 a2, const void *a3)
{
    task->data[7] = 0;
    task->data[8] = 0;
    task->data[9] = 0;
    task->data[15] = a2;
    task->data[10] = 0x100;
    task->data[11] = 0x100;
    task->data[12] = 0;
    StorePointerInVars(&task->data[13], &task->data[14], a3);
    PrepareBattlerSpriteForRotScale(a2, 0);
}

bool8 RunAffineAnimFromTaskData(struct Task *task)
{
    gUnknown_0202F7D4 = LoadPointerFromVars(task->data[13], task->data[14]) + (task->data[7] << 3);
    switch (gUnknown_0202F7D4->type)
    {
    default:
        if (!gUnknown_0202F7D4->frame.duration)
        {
            task->data[10] = gUnknown_0202F7D4->frame.xScale;
            task->data[11] = gUnknown_0202F7D4->frame.yScale;
            task->data[12] = gUnknown_0202F7D4->frame.rotation;
            task->data[7]++;
            gUnknown_0202F7D4++;
        }
        task->data[10] += gUnknown_0202F7D4->frame.xScale;
        task->data[11] += gUnknown_0202F7D4->frame.yScale;
        task->data[12] += gUnknown_0202F7D4->frame.rotation;
        SetSpriteRotScale(task->data[15], task->data[10], task->data[11], task->data[12]);
        SetBattlerSpriteYOffsetFromYScale(task->data[15]);
        if (++task->data[8] >= gUnknown_0202F7D4->frame.duration)
        {
            task->data[8] = 0;
            task->data[7]++;
        }
        break;
    case AFFINEANIMCMDTYPE_JUMP:
        task->data[7] = gUnknown_0202F7D4->jump.target;
        break;
    case AFFINEANIMCMDTYPE_LOOP:
        if (gUnknown_0202F7D4->loop.count)
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
                task->data[9] = gUnknown_0202F7D4->loop.count;
            }
            if (!task->data[7])
            {
                break;
            }
            for (;;)
            {
                task->data[7]--;
                gUnknown_0202F7D4--;
                if (gUnknown_0202F7D4->type == AFFINEANIMCMDTYPE_LOOP)
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
    u16 matrixNum = gSprites[spriteId].oam.matrixNum;
    int var2 = (var << 8) / gOamMatrices[matrixNum].d;

    if (var2 > 0x80)
        var2 = 0x80;
    gSprites[spriteId].y2 = (var - var2) / 2;
}

void SetBattlerSpriteYOffsetFromOtherYScale(u8 spriteId, u8 otherSpriteId)
{
    int var = 0x40 - GetBattlerYDeltaFromSpriteId(otherSpriteId) * 2;
    u16 matrixNum = gSprites[spriteId].oam.matrixNum;
    int var2 = (var << 8) / gOamMatrices[matrixNum].d;

    if (var2 > 0x80)
        var2 = 0x80;
    gSprites[spriteId].y2 = (var - var2) / 2;
}

u16 GetBattlerYDeltaFromSpriteId(u8 spriteId)
{
    struct BattleSpriteInfo *spriteInfo;
    u8 battler = gSprites[spriteId].data[0];
    u16 species;
    u16 i;

    for (i = 0; i < (sizeof(gBattleMonSprites) / sizeof(u8)); i++)
    {
        if (gBattleMonSprites[i] == spriteId)
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
                        species = GetMonData(&gPlayerParty[gBattleMonPartyPositions[i]], MON_DATA_SPECIES);
                    else
                        species = spriteInfo->transformSpecies;
                    return gMonBackPicCoords[species].y_offset;
                }
                else
                {
                    spriteInfo = &gBattleSpriteInfo[battler];
                    if (!spriteInfo->transformSpecies)
                        species = GetMonData(&gEnemyParty[gBattleMonPartyPositions[i]], MON_DATA_SPECIES);
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
    task->data[15] = spriteId;
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
    gBattleAnimArgs[ARG_RET_ID] = powerLevel;
    DestroyAnimVisualTask(taskId);
}

void unref_sub_8079D20(u8 priority)
{
    if (IsAnimBankSpriteVisible(gBattleAnimTarget))
        gSprites[gBattleMonSprites[gBattleAnimTarget]].oam.priority = priority;
    if (IsAnimBankSpriteVisible(gBattleAnimAttacker))
        gSprites[gBattleMonSprites[gBattleAnimAttacker]].oam.priority = priority;
    if (IsAnimBankSpriteVisible(gBattleAnimTarget ^ 2))
        gSprites[gBattleMonSprites[gBattleAnimTarget ^ 2]].oam.priority = priority;
    if (IsAnimBankSpriteVisible(gBattleAnimAttacker ^ 2))
        gSprites[gBattleMonSprites[gBattleAnimAttacker ^ 2]].oam.priority = priority;
}

void UpdateBattlerSpritePriorities()
{
    int i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (IsAnimBankSpriteVisible(i))
        {
            gSprites[gBattleMonSprites[i]].subpriority = GetBattlerSpriteSubpriority(i);
            gSprites[gBattleMonSprites[i]].oam.priority = 2;
        }
    }
}

u8 GetBattlerSpriteSubpriority(u8 bank)
{
    u8 identity;
    u8 ret;

    if (IsContest())
    {
        if (bank == ANIM_BATTLER_ATK_PARTNER)
            return 30;
        else
            return 40;
    }
    else
    {
        identity = GetBattlerPosition(bank);
        if (identity == B_POSITION_PLAYER_LEFT)
            ret = 30;
        else if (identity == B_POSITION_PLAYER_RIGHT)
            ret = 20;
        else if (identity == B_POSITION_OPPONENT_LEFT)
            ret = 40;
        else
            ret = 50;
    }
    return ret;
}

u8 GetBattlerSpriteBGPriority(u8 slot)
{
    u8 status = GetBattlerPosition(slot);

    if (IsContest())
        return 2;
    if (status == 0 || status == 3)
        return BG2CNT.priority;
    else
        return BG1CNT.priority;
}

u8 GetBattlerSpriteBGPriorityRank(u8 battler)
{
    u8 status;

    if (!IsContest())
    {
        status = GetBattlerPosition(battler);
        if (status == 0 || status == 3)
            return 2;
        else
            return 1;
    }
    return 1;
}

u8 sub_8079F44(u16 species, bool8 isBackpic, u8 a3, s16 a4, s16 a5, u8 a6, u32 a7, u32 a8)
{
    u8 sprite;
    u16 sheet = LoadSpriteSheet(&gUnknown_0837F5E0[a3]);
    u16 palette = AllocSpritePalette(gSpriteTemplate_837F5B0[a3].paletteTag);

    if (!isBackpic)
    {
        LoadCompressedPalette(GetMonSpritePalFromOtIdPersonality(species, a8, a7), (palette * 0x10) + 0x100, 0x20);
        LoadSpecialPokePic(
            &gMonFrontPicTable[species],
            gMonFrontPicCoords[species].coords,
            gMonFrontPicCoords[species].y_offset,
            (void *)EWRAM,
            (void *)EWRAM,
            species,
            a7,
            1
        );
    }
    else
    {
        LoadCompressedPalette(GetMonSpritePalFromOtIdPersonality(species, a8, a7), (palette * 0x10) + 0x100, 0x20);
        LoadSpecialPokePic(
            &gMonBackPicTable[species],
            gMonBackPicCoords[species].coords,
            gMonBackPicCoords[species].y_offset,
            (void *)EWRAM,
            (void *)EWRAM,
            species,
            a7,
            0
        );
    }

    DmaCopy32Defvars(3, (void *)EWRAM, (void *)(OBJ_VRAM0 + (sheet * 0x20)), 0x800);

    if (!isBackpic)
        sprite = CreateSprite(&gSpriteTemplate_837F5B0[a3], a4, a5 + gMonFrontPicCoords[species].y_offset, a6);
    else
        sprite = CreateSprite(&gSpriteTemplate_837F5B0[a3], a4, a5 + gMonBackPicCoords[species].y_offset, a6);

    if (IsContest())
    {
        gSprites[sprite].affineAnims = gAffineAnims_BattleSpriteContest;
        StartSpriteAffineAnim(&gSprites[sprite], 0);
    }
    return sprite;
}

void DestroySpriteAndFreeResources_(struct Sprite *sprite)
{
    DestroySpriteAndFreeResources(sprite);
}

s16 GetBattlerSpriteCoordAttr(u8 slot, u8 a2)
{
    u16 species;
    u32 personality;
    u16 letter;
    u16 var;
    int ret;
    const struct MonCoords *coords;
    struct BattleSpriteInfo *transform;

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
                var = SPECIES_UNOWN;
            else
                var = letter + SPECIES_UNOWN_B - 1;
            coords = &gMonBackPicCoords[var];
        }
        else if (species == SPECIES_CASTFORM)
        {
            coords = &gCastformFrontSpriteCoords[gBattleMonForms[slot]];
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
        if (!GetBattlerSide(slot))
        {
            transform = &gBattleSpriteInfo[slot];
            if (!transform->transformSpecies)
            {
                species = GetMonData(&gPlayerParty[gBattleMonPartyPositions[slot]], MON_DATA_SPECIES);
                personality = GetMonData(&gPlayerParty[gBattleMonPartyPositions[slot]], MON_DATA_PERSONALITY);
            }
            else
            {
                species = transform->transformSpecies;
                personality = gTransformPersonalities[slot];
            }
            if (species == SPECIES_UNOWN)
            {
                letter = GET_UNOWN_LETTER(personality);
                if (!letter)
                    var = SPECIES_UNOWN;
                else
                    var = letter + SPECIES_UNOWN_B - 1;
                coords = &gMonBackPicCoords[var];
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
            transform = &gBattleSpriteInfo[slot];
            if (!transform->transformSpecies)
            {
                species = GetMonData(&gEnemyParty[gBattleMonPartyPositions[slot]], MON_DATA_SPECIES);
                personality = GetMonData(&gEnemyParty[gBattleMonPartyPositions[slot]], MON_DATA_PERSONALITY);
            }
            else
            {
                species = transform->transformSpecies;
                personality = gTransformPersonalities[slot];
            }
            if (species == SPECIES_UNOWN)
            {
                letter = GET_UNOWN_LETTER(personality);
                if (!letter)
                    var = SPECIES_UNOWN;
                else
                    var = letter + SPECIES_UNOWN_B - 1;
                coords = &gMonFrontPicCoords[var];
            }
            else if (species == SPECIES_CASTFORM)
            {
                coords = &gCastformFrontSpriteCoords[gBattleMonForms[slot]];
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

    switch (a2)
    {
    case 0:
        return (coords->coords & 0xf) * 8;
    case 1:
        return (coords->coords >> 4) * 8;
    case 4:
        return GetBattlerSpriteCoord(slot, 2) - ((coords->coords >> 4) * 4);
    case 5:
        return GetBattlerSpriteCoord(slot, 2) + ((coords->coords >> 4) * 4);
    case 2:
        return GetBattlerSpriteCoord(slot, 3) - ((coords->coords & 0xf) * 4);
    case 3:
        return GetBattlerSpriteCoord(slot, 3) + ((coords->coords & 0xf) * 4);
    case 6:
        ret = GetBattlerSpriteCoord(slot, 1) + 0x1f;
        return ret - coords->y_offset;
    default:
        return 0;
    }
}

void SetAverageBattlerPositions(u8 slot, bool8 a2, s16 *x, s16 *y)
{
    u8 v1, v2;
    s16 v3, v4;
    s16 v5, v6;

    if (!a2)
    {
        v1 = 0;
        v2 = 1;
    }
    else
    {
        v1 = 2;
        v2 = 3;
    }
    v3 = GetBattlerSpriteCoord(slot, v1);
    v4 = GetBattlerSpriteCoord(slot, v2);
    if (IsDoubleBattle() && !IsContest())
    {
        v5 = GetBattlerSpriteCoord(slot ^ 2, v1);
        v6 = GetBattlerSpriteCoord(slot ^ 2, v2);
    }
    else
    {
        v5 = v3;
        v6 = v4;
    }
    *x = (v3 + v5) / 2;
    *y = (v4 + v6) / 2;
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

void sub_807A544(struct Sprite *sprite)
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

void sub_807A5C4(struct Sprite *sprite)
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

// Task and sprite data for AnimTask_AttackerPunchWithTrace

#define tBattlerSpriteId data[0]
#define tMoveSpeed       data[1]
#define tState           data[2]
#define tCounter         data[3]
#define tPaletteNum      data[4]
#define tNumTracesActive data[5]
#define tPriority        data[6]

#define sActiveTime data[0]
#define sTaskId     data[1]
#define sSpriteId   data[2]

static void AnimTask_AttackerPunchWithTrace_Step(u8 taskId);
static void CreateBattlerTrace(struct Task *task, u8 taskId);
static void AnimBattlerTrace(struct Sprite *sprite);

// Slides attacker to the right and back with a cloned trace of the specified color
// arg0: Trace palette blend color
// arg1: Trace palette blend coefficient

void AnimTask_AttackerPunchWithTrace(u8 taskId)
{
    u16 src;
    u16 dest;
    struct Task *task = &gTasks[taskId];

    task->tBattlerSpriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    task->tMoveSpeed = (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER) ? -8 : 8;
    task->tState = 0;
    task->tCounter = 0;
    gSprites[task->tBattlerSpriteId].x2 -= task->tBattlerSpriteId;
    task->tPaletteNum = AllocSpritePalette(ANIM_TAG_BENT_SPOON);
    task->tNumTracesActive = 0;

    dest = (task->tPaletteNum + 0x10) * 0x10;
    src = (gSprites[task->tBattlerSpriteId].oam.paletteNum + 0x10) * 0x10;
    task->tPriority = GetBattlerSpriteSubpriority(gBattleAnimAttacker);
    if (task->tPriority == 20 || task->tPriority == 40)
        task->tPriority = 2;
    else
        task->tPriority = 3;
    CpuCopy32(&gPlttBufferUnfaded[src], &gPlttBufferFaded[dest], 0x20);
    BlendPalette(dest, 16, gBattleAnimArgs[1], gBattleAnimArgs[0]);
    task->func = AnimTask_AttackerPunchWithTrace_Step;
}

static void AnimTask_AttackerPunchWithTrace_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->tState)
    {
    case 0:
        CreateBattlerTrace(task, taskId);
        gSprites[task->tBattlerSpriteId].x2 += task->tMoveSpeed;
        if (++task->tCounter == 5)
        {
            task->tCounter--;
            task->tState++;
        }
        break;
    case 1:
        CreateBattlerTrace(task, taskId);
        gSprites[task->tBattlerSpriteId].x2 -= task->tMoveSpeed;
        if (--task->tCounter == 0)
        {
            gSprites[task->tBattlerSpriteId].x2 = 0;
            task->tState++;
        }
        break;
    case 2:
        if (!task->tNumTracesActive)
        {
            FreeSpritePaletteByTag(ANIM_TAG_BENT_SPOON);
            DestroyAnimVisualTask(taskId);
        }
        break;
    }
}

static void CreateBattlerTrace(struct Task *task, u8 taskId)
{
    s16 spriteId = CloneBattlerSpriteWithBlend(ANIM_BATTLER_ATTACKER);
    if (spriteId >= 0)
    {
        gSprites[spriteId].oam.priority = task->tPriority;
        gSprites[spriteId].oam.paletteNum = task->tPaletteNum;
        gSprites[spriteId].sActiveTime = 8;
        gSprites[spriteId].sTaskId = taskId;
        gSprites[spriteId].sSpriteId = spriteId;
        gSprites[spriteId].x2 = gSprites[task->tBattlerSpriteId].x2;
        gSprites[spriteId].callback = AnimBattlerTrace;
        task->tNumTracesActive++;
    }
}

static void AnimBattlerTrace(struct Sprite *sprite)
{
    if (--sprite->sActiveTime == 0)
    {
        gTasks[sprite->sTaskId].tNumTracesActive--;
        DestroySpriteWithActiveSheet(sprite);
    }
}

#undef tBattlerSpriteId
#undef tMoveSpeed
#undef tState
#undef tCounter
#undef tPaletteNum
#undef tNumTracesActive
#undef tPriority

#undef sActiveTime
#undef sTaskId
#undef sSpriteId

// file_4

static void AnimWeatherBallUp_Step(struct Sprite *sprite);

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
