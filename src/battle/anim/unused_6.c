#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimBouncingMusicNote(struct Sprite *sprite);
static void AnimBouncingMusicNote_Step(struct Sprite *sprite);

// unused_6

static const union AnimCmd sAnim_BouncingMusicNote[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_BouncingMusicNote[] =
{
    sAnim_BouncingMusicNote,
};

static const struct SpriteTemplate sBouncingMusicNoteSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES,
    .paletteTag = ANIM_TAG_MUSIC_NOTES,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBouncingMusicNote,
};

static void AnimBouncingMusicNote(struct Sprite *sprite)
{
    u8 battler;
    if (gBattleAnimArgs[0] == 0)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    SetSpriteNextToMonHead(battler, sprite);
    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->callback = AnimBouncingMusicNote_Step;
}

static void AnimBouncingMusicNote_Step(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->y2 -= 3;
        if (++sprite->data[1] == 6)
            sprite->data[0]++;
        break;
    case 1:
        sprite->y2 += 3;
        if (--sprite->data[1] == 0)
            sprite->data[0]++;
        break;
    case 2:
        if (++sprite->data[1] == 0x40)
            DestroyAnimSprite(sprite);
        break;
    }
}
