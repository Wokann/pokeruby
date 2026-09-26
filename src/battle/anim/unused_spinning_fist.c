#include "global.h"
#include "battle_anim.h"
#include "constants/battle.h"
#include "rom_8077ABC.h"
#include "trig.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimUnusedSpinningFist(struct Sprite *sprite);
static void AnimUnusedSpinningFist_Step(struct Sprite *sprite);

static const union AffineAnimCmd sAffineAnim_UnusedSpinningFist[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 20),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -16, 60),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_UnusedSpinningFist[] =
{
    sAffineAnim_UnusedSpinningFist,
};

// Unused
static const struct SpriteTemplate sUnusedSpinningFistSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_UnusedSpinningFist,
    .callback = AnimUnusedSpinningFist,
};

static void AnimUnusedSpinningFist(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        sprite->x -= gBattleAnimArgs[0];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
    }

    sprite->callback = AnimUnusedSpinningFist_Step;
}

static void AnimUnusedSpinningFist_Step(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
    {
        DestroySpriteAndMatrix(sprite);
    }
}
