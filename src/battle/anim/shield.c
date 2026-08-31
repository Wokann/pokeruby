#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "palette.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

void AnimProtect(struct Sprite* sprite);
static void AnimProtect_Step(struct Sprite* sprite);

// shield
// Used by Protect.

const struct SpriteTemplate gProtectSpriteTemplate =
{
    .tileTag = ANIM_TAG_PROTECT,
    .paletteTag = ANIM_TAG_PROTECT,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimProtect,
};

void AnimProtect(struct Sprite* sprite)
{
    if (IsContest())
    {
        gBattleAnimArgs[1] += 8;
    }

    sprite->x = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_X) + gBattleAnimArgs[0];
    sprite->y = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_Y) + gBattleAnimArgs[1];
    if (GetBattlerSide(gBattleAnimAttacker) == 0 || IsContest())
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimAttacker) + 1;
    else
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimAttacker);

    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[2] = (IndexOfSpritePaletteTag(ANIM_TAG_PROTECT) << 4) + 0x100;
    sprite->data[7] = 16;
    REG_BLDCNT = 0x3F40;
    REG_BLDALPHA = (sprite->data[7] << 8) | (16 - sprite->data[7]);
    sprite->callback = AnimProtect_Step;
}

static void AnimProtect_Step(struct Sprite* sprite)
{
    int savedPal;
    int i;
    sprite->data[5] += 0x60;
    sprite->x2 = -(sprite->data[5] >> 8);
    sprite->data[1]++;
    if (sprite->data[1] > 1)
    {
        sprite->data[1] = 0;
        savedPal = gPlttBufferFaded[sprite->data[2] + 1];
        i = 0;
        do
        {
            gPlttBufferFaded[sprite->data[2] + ++i] = gPlttBufferFaded[sprite->data[2] + i + 1];
        } while ( i <= 5 );

        gPlttBufferFaded[sprite->data[2] + 7] = savedPal;
    }

    if (sprite->data[7] > 6 && sprite->data[0] >0 && ++sprite->data[6] > 1)
    {
        sprite->data[6] = 0;
        sprite->data[7] -= 1;
        REG_BLDALPHA = (sprite->data[7] << 8) | (16 - sprite->data[7]);;
    }

    if (sprite->data[0] >0)
    {
        sprite->data[0] -= 1;
    }
    else if (++sprite->data[6] > 1)
    {
        sprite->data[6] = 0;
        sprite->data[7]++;
        REG_BLDALPHA = (sprite->data[7] << 8) | (16 - sprite->data[7]);
        if (sprite->data[7] == 16)
        {
            sprite->invisible = TRUE;
            sprite->callback = DestroyAnimSpriteAndDisableBlend;
        }
    }
}
