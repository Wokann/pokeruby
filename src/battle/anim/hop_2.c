#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "constants/songs.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern void InitItemBagData(struct Sprite* sprite, s16 c);
extern bool8 moveAlongLinearPath(struct Sprite* sprite);
extern void AnimItemSteal_Step2(struct Sprite* sprite);
extern const union AnimCmd *const gFallingBagAnimTable[];
extern const union AffineAnimCmd *const gFallingBagAffineAnimTable[];

void AnimItemSteal(struct Sprite* sprite);
static void AnimItemSteal_Step3(struct Sprite* sprite);

// hop_2
// Used in item steal.

const struct SpriteTemplate gItemStealSpriteTemplate =
{
    .tileTag = ANIM_TAG_ITEM_BAG,
    .paletteTag = ANIM_TAG_ITEM_BAG,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gFallingBagAnimTable,
    .images = NULL,
    .affineAnims = gFallingBagAffineAnimTable,
    .callback = AnimItemSteal,
};

void AnimItemSteal(struct Sprite* sprite)
{
    s16 attackerX;
    s16 attackerY;
    sub_8078764(sprite, FALSE);
    attackerX = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
    attackerY = GetBattlerSpriteCoord(gBattleAnimAttacker, 1);
    if ((gBattleAnimTarget ^ 2) == gBattleAnimAttacker)
    {
        sprite->data[6] = attackerX;
        sprite->data[7] = attackerY + 10;
        InitItemBagData(sprite, 0x3c);
        sprite->data[3] = 1;
    }
    else
    {
        sprite->data[6] = attackerX;
        sprite->data[7] = attackerY + 10;
        InitItemBagData(sprite, 0x3c);
        sprite->data[3] = 3;
    }

    sprite->data[4] = 0x3C;
    sprite->callback = AnimItemSteal_Step3;
}

static void AnimItemSteal_Step3(struct Sprite* sprite)
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
        PlaySE12WithPanning(SE_M_BUBBLE2, BattleAnimAdjustPanning(SOUND_PAN_TARGET));
    }

    if (moveAlongLinearPath(sprite))
    {
        sprite->y2 = 0;
        sprite->data[0] = 0;
        sprite->callback = AnimItemSteal_Step2;
        PlaySE12WithPanning(SE_M_BUBBLE2, BattleAnimAdjustPanning(SOUND_PAN_ATTACKER_NEG));
    }
}
