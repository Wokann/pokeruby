#include "global.h"
#include "battle_anim.h"
#include "constants/battle.h"
#include "constants/songs.h"
#include "rom_8077ABC.h"
#include "sound.h"
#include "trig.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u16 gBattleTypeFlags;
extern u8 gAnimCustomPanning;

static void AnimWillOWispOrb(struct Sprite *sprite);
static void AnimWillOWispOrb_Step(struct Sprite *);

// Orbs used in Will-O-Wisp.

static const union AnimCmd sAnim_WillOWispOrb_0[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_FRAME(12, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_WillOWispOrb_1[] =
{
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_WillOWispOrb_2[] =
{
    ANIMCMD_FRAME(20, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_WillOWispOrb_3[] =
{
    ANIMCMD_FRAME(20, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_WillOWispOrb[] =
{
    sAnim_WillOWispOrb_0,
    sAnim_WillOWispOrb_1,
    sAnim_WillOWispOrb_2,
    sAnim_WillOWispOrb_3,
};

const struct SpriteTemplate gWillOWispOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_WISP_ORB,
    .paletteTag = ANIM_TAG_WISP_ORB,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = sAnims_WillOWispOrb,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimWillOWispOrb,
};

static void AnimWillOWispOrb(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        InitSpritePosToAnimAttacker(sprite, FALSE);
        StartSpriteAnim(sprite, gBattleAnimArgs[2]);
        sprite->data[7] = gBattleAnimArgs[2];

        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        {
            sprite->data[4] = 4;
        }
        else
        {
            sprite->data[4] = -4;
        }

        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimTarget);
        sprite->data[0]++;
        break;
    case 1:
        sprite->data[1] += 192;
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        {
            sprite->y2 = -(sprite->data[1] >> 8);
        }
        else
        {
            sprite->y2 = sprite->data[1] >> 8;
        }

        sprite->x2 = Sin(sprite->data[2], sprite->data[4]);
        sprite->data[2] = (sprite->data[2] + 4) & 0xFF;

        if (++sprite->data[3] == 1)
        {
            sprite->data[3] = 0;
            sprite->data[0]++;
        }
        break;
    case 2:
        sprite->x2 = Sin(sprite->data[2], sprite->data[4]);
        sprite->data[2] = (sprite->data[2] + 4) & 0xFF;

        if (++sprite->data[3] == 31)
        {
            sprite->x += sprite->x2;
            sprite->y += sprite->y2;
            sprite->y2 = 0;
            sprite->x2 = 0;

            sprite->data[0] = 256;
            sprite->data[1] = sprite->x;
            sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
            sprite->data[3] = sprite->y;
            sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);

            InitAnimLinearTranslationWithSpeed(sprite);
            sprite->callback = AnimWillOWispOrb_Step;
        }
        break;
    }
}

static void AnimWillOWispOrb_Step(struct Sprite *sprite)
{
    s16 initialData5;
    s16 newData5;

    if (!AnimTranslateLinear(sprite))
    {
        sprite->x2 += Sin(sprite->data[5], 16);
        initialData5 = sprite->data[5];
        sprite->data[5] = (sprite->data[5] + 4) & 0xFF;
        newData5 = sprite->data[5];

        if ((initialData5 == 0 || initialData5 > 196) && newData5 > 0 && sprite->data[7] == 0)
        {
            PlaySE12WithPanning(SE_M_FLAME_WHEEL, gAnimCustomPanning);
        }
    }
    else
    {
        DestroyAnimSprite(sprite);
    }
}
