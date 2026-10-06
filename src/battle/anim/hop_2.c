#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"


extern void InitItemBagData(struct Sprite* sprite, s16 c);
extern bool8 moveAlongLinearPath(struct Sprite* sprite);
extern void AnimItemSteal_Step2(struct Sprite* sprite);
extern const union AnimCmd *const gFallingBagAnimTable[];
extern const union AffineAnimCmd *const gFallingBagAffineAnimTable[];

void sub_80CBAE8(struct Sprite* sprite);
static void sub_80CBB60(struct Sprite* sprite);

// hop_2
// Used in item steal.

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D677C =
{
    .tileTag = ANIM_TAG_ITEM_BAG,
    .paletteTag = ANIM_TAG_ITEM_BAG,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gFallingBagAnimTable,
    .images = NULL,
    .affineAnims = gFallingBagAffineAnimTable,
    .callback = sub_80CBAE8,
};

void sub_80CBAE8(struct Sprite* sprite)
{
    s16 p1;
    s16 p2;
    InitSpritePosToAnimTarget(sprite, FALSE);
    p1 = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
    p2 = GetBattlerSpriteCoord(gBattleAnimAttacker, 1);
    if ((gBattleAnimTarget ^ 2) == gBattleAnimAttacker)
    {
        sprite->data[6] = p1;
        sprite->data[7] = p2 + 10;
        InitItemBagData(sprite, 0x3c);
        sprite->data[3] = 1;
    }
    else
    {
        sprite->data[6] = p1;
        sprite->data[7] = p2 + 10;
        InitItemBagData(sprite, 0x3c);
        sprite->data[3] = 3;
    }

    sprite->data[4] = 0x3C;
    sprite->callback = sub_80CBB60;
}

static void sub_80CBB60(struct Sprite* sprite)
{
    int zero;
    sprite->data[0] += ((sprite->data[3] * 128) / sprite->data[4]);
    zero = 0;
    if (sprite->data[0] > 0x7F)
    {
        sprite->data[1]++;
        sprite->data[0] = zero;
    }

    sprite->y2 = Sin(sprite->data[0] + 0x80, 30 - sprite->data[1] * 8);
    if (sprite->y2 == 0)
    {
        PlaySE12WithPanning(0x7D, BattleAnimAdjustPanning(SOUND_PAN_TARGET));
    }

    if (moveAlongLinearPath(sprite))
    {
        sprite->y2 = 0;
        sprite->data[0] = 0;
        sprite->callback = AnimItemSteal_Step2;
        PlaySE12WithPanning(0x7D, BattleAnimAdjustPanning(SOUND_PAN_ATTACKER));
    }
}
