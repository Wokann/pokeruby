#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimGuillotinePincer(struct Sprite *sprite);
static void AnimGuillotinePincer_Step1(struct Sprite *sprite);
static void AnimGuillotinePincer_Step2(struct Sprite *sprite);
static void AnimGuillotinePincer_Step3(struct Sprite *sprite);

// guillotine (does a reverse grip where the slices can reverse at a given period.)
// Used in Guillotine.

const union AnimCmd gGuillotineAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(32, 1),
    ANIMCMD_END,
};

const union AnimCmd gGuillotineAnimCmds2[] =
{
    ANIMCMD_FRAME(0, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 1, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gGuillotineAnimTable[] =
{
    gGuillotineAnimCmds1,
    gGuillotineAnimCmds2,
};

const struct SpriteTemplate gGuillotineSpriteTemplate =
{
    .tileTag = ANIM_TAG_CUT,
    .paletteTag = ANIM_TAG_CUT,
    .oam = &gOamData_AffineOff_ObjBlend_32x32,
    .anims = gGuillotineAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimGuillotinePincer,
};

static void AnimGuillotinePincer(struct Sprite *sprite)
{
    s16 startXOffset = 32;
    s16 startYOffset = -32;
    s16 endXOffset = 16;
    s16 endYOffset = -16;
    if (gBattleAnimArgs[0])
    {
        startXOffset = -32;
        startYOffset = 32;
        endXOffset = -16;
        endYOffset = 16;
        StartSpriteAnim(sprite, gBattleAnimArgs[0]);
    }

    sprite->x += startXOffset;
    sprite->y += startYOffset;
    sprite->data[0] = 6;
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) + endXOffset;
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + endYOffset;
    InitAnimLinearTranslation(sprite);
    sprite->data[5] = gBattleAnimArgs[0];
    sprite->data[6] = sprite->data[0];
    sprite->callback = AnimGuillotinePincer_Step1;
}

static void AnimGuillotinePincer_Step1(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite) && sprite->animEnded)
    {
        SeekSpriteAnim(sprite, 0);
        sprite->animPaused = 1;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 2;
        sprite->y2 = -2;
        sprite->data[0] = sprite->data[6];
        sprite->data[1] ^= 1;
        sprite->data[2] ^= 1;
        sprite->data[4] = 0;
        sprite->data[3] = 0;
        sprite->callback = AnimGuillotinePincer_Step2;
    }
}

static void AnimGuillotinePincer_Step2(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = -sprite->x2;
        sprite->y2 = -sprite->y2;
    }

    sprite->data[3] ^= 1;
    if (++sprite->data[4] == 51)
    {
        sprite->y2 = 0;
        sprite->x2 = 0;
        sprite->data[4] = 0;
        sprite->data[3] = 0;
        sprite->animPaused = 0;
        StartSpriteAnim(sprite, sprite->data[5] ^ 1);
        sprite->callback = AnimGuillotinePincer_Step3;
    }
}

static void AnimGuillotinePincer_Step3(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
        DestroyAnimSprite(sprite);
}
