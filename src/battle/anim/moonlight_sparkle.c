#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimMoonlightSparkle(struct Sprite *sprite);
static void AnimMoonlightSparkle_Step(struct Sprite *sprite);

// Used in Moonlight.

const union AnimCmd gMoonlightSparkleAnimCmds[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(8, 8),
    ANIMCMD_FRAME(12, 8),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gMoonlightSparkleAnimTable[] =
{
    gMoonlightSparkleAnimCmds,
};

const struct SpriteTemplate gMoonlightSparkleSpriteTemplate =
{
    .tileTag = ANIM_TAG_GREEN_SPARKLE,
    .paletteTag = ANIM_TAG_GREEN_SPARKLE,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gMoonlightSparkleAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimMoonlightSparkle,
};

static void AnimMoonlightSparkle(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2) + gBattleAnimArgs[0];
    sprite->y = gBattleAnimArgs[1];
    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = 1;
    sprite->callback = AnimMoonlightSparkle_Step;
}

static void AnimMoonlightSparkle_Step(struct Sprite *sprite)
{
    if (++sprite->data[1] > 1)
    {
        sprite->data[1] = 0;
        if (sprite->data[2] < 120)
        {
            sprite->y++;
            sprite->data[2]++;
        }
    }

    if (sprite->data[0])
        DestroyAnimSprite(sprite);
}
