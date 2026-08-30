#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

void AnimPresent(struct Sprite* sprite);
void AnimKnockOffItem(struct Sprite* sprite);

// hop (where a sprite "hops" across the screen)
// Used by Present, Trick, and the item knock off effect.

const union AnimCmd gFallingBagAnimCmds[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END,
};

const union AnimCmd *const gFallingBagAnimTable[] =
{
    gFallingBagAnimCmds,
};

const union AffineAnimCmd gFallingBagAffineAnimCmds1[] = {
    AFFINEANIMCMD_FRAME(0, 0, -4, 10),
    AFFINEANIMCMD_FRAME(0, 0, 4, 20),
    AFFINEANIMCMD_FRAME(0, 0, -4, 10),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gFallingBagAffineAnimCmds2[] = {
    AFFINEANIMCMD_FRAME(0, 0, -1, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 1, 2),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gFallingBagAffineAnimTable[] = {
    gFallingBagAffineAnimCmds1,
    gFallingBagAffineAnimCmds2,
};

const struct SpriteTemplate gPresentSpriteTemplate =
{
    .tileTag = ANIM_TAG_ITEM_BAG,
    .paletteTag = ANIM_TAG_ITEM_BAG,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gFallingBagAnimTable,
    .images = NULL,
    .affineAnims = gFallingBagAffineAnimTable,
    .callback = AnimPresent,
};

const struct SpriteTemplate gKnockOffItemSpriteTemplate =
{
    .tileTag = ANIM_TAG_ITEM_BAG,
    .paletteTag = ANIM_TAG_ITEM_BAG,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gFallingBagAnimTable,
    .images = NULL,
    .affineAnims = gFallingBagAffineAnimTable,
    .callback = AnimKnockOffItem,
};

void InitItemBagData(struct Sprite* sprite, s16 c)
{
    s32 a = (sprite->x * 256) | sprite->y;
    s32 b = (sprite->data[6] * 256) | sprite->data[7];
    c *= 256;
    sprite->data[5] = a;
    sprite->data[6] = b;
    sprite->data[7] = c;
}

bool8 moveAlongLinearPath(struct Sprite* sprite)
{
    u16 xStartPos = (u8)(sprite->data[5] >> 8);
    u16 yStartPos = (u8)sprite->data[5];
    s32 xEndPos = (u8)(sprite->data[6] >> 8);
    s32 yEndPos = (u8)sprite->data[6];
    s16 totalTime = sprite->data[7] >> 8;
    s16 currentTime = sprite->data[7] & 0xFF;
    s16 yEndPos_2;
    s16 r0;
    s32 var1;
    s32 vaxEndPos;

    if (xEndPos == 0)
    {
        xEndPos = -32;
    }
    else if (xEndPos == 255)
    {
        xEndPos = 0x110;
    }

    yEndPos_2 = yEndPos - yStartPos;
    r0 = xEndPos - xStartPos;
    var1 = r0 * currentTime / totalTime;
    vaxEndPos = yEndPos_2 * currentTime / totalTime;
    sprite->x = var1 + xStartPos;
    sprite->y = vaxEndPos + yStartPos;
    if (++currentTime == totalTime)
        return TRUE;

    sprite->data[7] = (totalTime << 8) | currentTime;
    return FALSE;
}

void AnimItemSteal_Step2(struct Sprite* sprite)
{
    if (sprite->data[0] == 10)
    {
        StartSpriteAffineAnim(sprite, 1);
    }

    sprite->data[0]++;
    if (sprite->data[0] > 50)
    {
        DestroyAnimSprite(sprite);
    }
}

void AnimItemSteal_Step1(struct Sprite* sprite)
{
    sprite->data[0] += sprite->data[3] * 128 / sprite->data[4];
    if (sprite->data[0] >= 128)
    {
        sprite->data[1]++;
        sprite->data[0] = 0;
    }

    sprite->y2 = Sin(sprite->data[0] + 128, 30 - sprite->data[1] * 8);
    if (moveAlongLinearPath(sprite))
    {
        sprite->y2 = 0;
        sprite->data[0] = 0;
        sprite->callback = AnimItemSteal_Step2;
    }
}

void AnimPresent(struct Sprite* sprite)
{
    s16 targetX;
    s16 targetY;
    InitSpritePosToAnimAttacker(sprite, 0);
    targetX = GetBattlerSpriteCoord(gBattleAnimTarget, 0);
    targetY = GetBattlerSpriteCoord(gBattleAnimTarget, 1);
    if ((gBattleAnimAttacker ^ 2) == gBattleAnimTarget)
    {
        sprite->data[6] = targetX;
        sprite->data[7] = targetY + 10;
        InitItemBagData(sprite, 0x3C);
        sprite->data[3] = 1;
    }
    else
    {
        sprite->data[6] = targetX;
        sprite->data[7] = targetY + 10;
        InitItemBagData(sprite, 0x3C);
        sprite->data[3] = 3;
    }

    sprite->data[4] = 0x3C;
    sprite->callback = AnimItemSteal_Step1;
}

void AnimKnockOffOpponentsItem(struct Sprite* sprite)
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
    if (moveAlongLinearPath(sprite))
    {
        sprite->y2 = zero;
        sprite->data[0] = zero;
        DestroyAnimSprite(sprite);
    }
}

void AnimKnockOffItem(struct Sprite* sprite)
{
    s16 targetY = GetBattlerSpriteCoord(gBattleAnimTarget, 1);
    if (GetBattlerSide(gBattleAnimTarget) == 0)
    {
        sprite->data[6] = 0;
        sprite->data[7] = targetY + 10;
        InitItemBagData(sprite, 0x28);
        sprite->data[3] = 3;
        sprite->data[4] = 0x3C;
        sprite->callback = AnimItemSteal_Step1;
    }
    else
    {
        sprite->data[6] = 255;
        sprite->data[7] = targetY + 10;
        if (IsContest())
            sprite->data[6] = 0;

        InitItemBagData(sprite, 0x28);
        sprite->data[3] = 3;
        sprite->data[4] = 0x3C;
        sprite->callback = AnimKnockOffOpponentsItem;
    }
}
