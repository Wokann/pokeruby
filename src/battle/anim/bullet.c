#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "random.h"
#include "constants/songs.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimBulletSeed(struct Sprite* sprite);
static void AnimBulletSeed_Step1(struct Sprite* sprite);
static void AnimBulletSeed_Step2(struct Sprite* sprite);

// bullet (shoot seeds as ammunition.)
// Used by Bullet Seed.

const union AffineAnimCmd gBulletSeedAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 20, 1),
    AFFINEANIMCMD_JUMP(0),
};

const union AffineAnimCmd *const gBulletSeedAffineAnimTable[] =
{
    gBulletSeedAffineAnimCmds,
};

const struct SpriteTemplate gBulletSeedSpriteTemplate =
{
    .tileTag = ANIM_TAG_SEED,
    .paletteTag = ANIM_TAG_SEED,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gBulletSeedAffineAnimTable,
    .callback = AnimBulletSeed,
};

static void AnimBulletSeed(struct Sprite* sprite)
{
    InitSpritePosToAnimAttacker(sprite, TRUE);
    sprite->data[0] = 20;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->callback = StartAnimLinearTranslation;
    sprite->affineAnimPaused = 1;
    StoreSpriteCallbackInData6(sprite, AnimBulletSeed_Step1);
}

static void AnimBulletSeed_Step1(struct Sprite* sprite)
{
    int i;
    u16 rand;
    s16* ptr;
    PlaySE12WithPanning(SE_M_HORN_ATTACK, BattleAnimAdjustPanning(SOUND_PAN_TARGET));
    sprite->x += sprite->x2;
    sprite->y += sprite->y2;
    sprite->y2 = 0;
    sprite->x2 = 0;
    ptr = &sprite->data[7];
    for (i = 0; i < 8; i++)
    {
        ptr[i - 7] = 0;
    }

    rand = Random();
    sprite->data[6] = 0xFFF4 - (rand & 7);
    rand = Random();
    sprite->data[7] = (rand % 0xA0) + 0xA0;
    sprite->callback = AnimBulletSeed_Step2;
    sprite->affineAnimPaused = 0;
}

static void AnimBulletSeed_Step2(struct Sprite* sprite)
{
    sprite->data[0] += sprite->data[7];
    sprite->x2 = sprite->data[0] >> 8;
    if (sprite->data[7] & 1)
        sprite->x2 = -sprite->x2;

    sprite->y2 = Sin(sprite->data[1], sprite->data[6]);
    sprite->data[1] += 8;
    if (sprite->data[1] > 0x7E)
    {
        sprite->data[1] = 0;
        sprite->data[2] /= 2;
        if (++sprite->data[3] == 1)
            DestroyAnimSprite(sprite);
    }
}
