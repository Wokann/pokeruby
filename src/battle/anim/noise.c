#include "global.h"
#include "battle_anim.h"

static void AnimJaggedMusicNote(struct Sprite *sprite);
static void AnimJaggedMusicNote_Step(struct Sprite *);

// noise (moving music note)
// Used in Uproar.

const struct SpriteTemplate gJaggedMusicNoteSpriteTemplate =
{
    .tileTag = ANIM_TAG_JAGGED_MUSIC_NOTE,
    .paletteTag = ANIM_TAG_JAGGED_MUSIC_NOTE,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimJaggedMusicNote,
};

static void AnimJaggedMusicNote(struct Sprite *sprite)
{
    int var1;
    u8 battler = gBattleAnimArgs[0] == 0 ? gBattleAnimAttacker : gBattleAnimTarget;

    if (GetBattlerSide(battler) == 1)
    {
        gBattleAnimArgs[1] *= -1;
    }

    sprite->x = GetBattlerSpriteCoord(battler, 2)  + gBattleAnimArgs[1];
    sprite->y = GetBattlerSpriteCoord(battler, 3) + gBattleAnimArgs[2];
    sprite->data[0] = 0;
    sprite->data[1] = (u16)sprite->x << 3;
    sprite->data[2] = (u16)sprite->y << 3;

    var1 = gBattleAnimArgs[1] << 3;
    if (var1 < 0)
        var1 += 7;
    sprite->data[3] = var1 >> 3;

    var1 = gBattleAnimArgs[2] << 3;
    if (var1 < 0)
        var1 += 7;
    sprite->data[4] = var1 >> 3;

    sprite->oam.tileNum += gBattleAnimArgs[3] * 16;
    sprite->callback = AnimJaggedMusicNote_Step;
}

static void AnimJaggedMusicNote_Step(struct Sprite *sprite)
{
    sprite->data[1] += sprite->data[3];
    sprite->data[2] += sprite->data[4];

    sprite->x = sprite->data[1] >> 3;
    sprite->y = sprite->data[2] >> 3;

    if (++sprite->data[0] > 16)
    {
        DestroyAnimSprite(sprite);
    }
}
