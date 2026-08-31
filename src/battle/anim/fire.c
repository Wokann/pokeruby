#include "global.h"
#include "battle_anim.h"
#include "rom_8077ABC.h"
#include "trig.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;

void sub_80D4ED8(struct Sprite *sprite);
void AnimFireSpread(struct Sprite *sprite);
static void AnimLargeFlame(struct Sprite *);
static void AnimFirePlume(struct Sprite *);
static void AnimUnusedSmallEmber(struct Sprite *);
static void AnimLargeFlame_Step(struct Sprite *);
static void AnimUnusedSmallEmber_Step(struct Sprite *);

const union AnimCmd gSpriteAnim_83D9450[] =
{
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_JUMP(0),
};

const union AnimCmd gSpriteAnim_83D9460[] =
{
    ANIMCMD_FRAME(16, 4, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(48, 4, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gSpriteAnimTable_83D9470[] =
{
    gSpriteAnim_83D9450,
    gSpriteAnim_83D9460,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9478 =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9470,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D4ED8,
};

const struct SpriteTemplate gFireSpreadSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9470,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFireSpread,
};

static const union AnimCmd sAnim_LargeFlame[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(48, 3),
    ANIMCMD_FRAME(64, 3),
    ANIMCMD_FRAME(80, 3),
    ANIMCMD_FRAME(96, 3),
    ANIMCMD_FRAME(112, 3),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_LargeFlame[] =
{
    sAnim_LargeFlame,
};

static const union AnimCmd sAnim_FirePlume[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_FirePlume[] =
{
    sAnim_FirePlume,
};

static const union AffineAnimCmd sAffineAnim_LargeFlame[] =
{
    AFFINEANIMCMD_FRAME(0x32, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x20, 0x0, 0, 7),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_LargeFlame[] =
{
    sAffineAnim_LargeFlame,
};

const struct SpriteTemplate gLargeFlameSpriteTemplate =
{
    .tileTag = ANIM_TAG_FIRE,
    .paletteTag = ANIM_TAG_FIRE,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = sAnims_LargeFlame,
    .images = NULL,
    .affineAnims = sAffineAnims_LargeFlame,
    .callback = AnimLargeFlame,
};

const struct SpriteTemplate gLargeFlameScatterSpriteTemplate =
{
    .tileTag = ANIM_TAG_FIRE,
    .paletteTag = ANIM_TAG_FIRE,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_LargeFlame,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimLargeFlame,
};

const struct SpriteTemplate gFirePlumeSpriteTemplate =
{
    .tileTag = ANIM_TAG_FIRE_PLUME,
    .paletteTag = ANIM_TAG_FIRE_PLUME,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_FirePlume,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFirePlume,
};

// Unused
static const struct SpriteTemplate sUnusedEmberFirePlumeSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_FirePlume,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFirePlume,
};

const union AnimCmd gSpriteAnim_83D9568[] =
{
    ANIMCMD_FRAME(16, 6),
    ANIMCMD_FRAME(32, 6),
    ANIMCMD_FRAME(48, 6),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gSpriteAnimTable_83D9578[] =
{
    gSpriteAnim_83D9568,
};

const struct SpriteTemplate gSpriteTemplate_83D957C =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9578,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimUnusedSmallEmber,
};

void sub_80D4ED8(struct Sprite *sprite)
{
    sprite->data[0] = gBattleAnimArgs[0];
    sprite->data[1] = 0x3C;
    sprite->data[2] = 0x9;
    sprite->data[3] = 0x1E;
    sprite->data[4] = 0xFE00;

    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);

    sprite->callback = sub_8078174;
    sprite->callback(sprite);
}

void AnimFireSpread(struct Sprite *sprite)
{
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);

    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[1] = gBattleAnimArgs[2];
    sprite->data[2] = gBattleAnimArgs[3];

    sprite->callback = sub_8078394;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

static void AnimFirePlume(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);

    if (GetBattlerSide(gBattleAnimAttacker))
    {
        sprite->x -= gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        sprite->data[2] = -gBattleAnimArgs[4];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        sprite->data[2] = gBattleAnimArgs[4];
    }

    sprite->data[1] = gBattleAnimArgs[2];
    sprite->data[4] = gBattleAnimArgs[3];
    sprite->data[3] = gBattleAnimArgs[5];

    sprite->callback = AnimLargeFlame_Step;
}

static void AnimLargeFlame(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker))
    {
        sprite->x -= gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        sprite->data[2] = gBattleAnimArgs[4];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        sprite->data[2] = -gBattleAnimArgs[4];
    }

    sprite->data[1] = gBattleAnimArgs[2];
    sprite->data[4] = gBattleAnimArgs[3];
    sprite->data[3] = gBattleAnimArgs[5];

    sprite->callback = AnimLargeFlame_Step;
}

static void AnimLargeFlame_Step(struct Sprite *sprite)
{
    if (++sprite->data[0] < sprite->data[4])
    {
        sprite->x2 += sprite->data[2];
        sprite->y2 += sprite->data[3];
    }

    if (sprite->data[0] == sprite->data[1])
        DestroySpriteAndMatrix(sprite);
}

static void AnimUnusedSmallEmber(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);

    if (GetBattlerSide(gBattleAnimAttacker))
    {
        sprite->x -= gBattleAnimArgs[0];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->subpriority = 8;
    }

    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];
    sprite->data[3] = gBattleAnimArgs[5];
    sprite->data[4] = gBattleAnimArgs[6];
    sprite->data[5] = 0;

    sprite->callback = AnimUnusedSmallEmber_Step;
}

static void AnimUnusedSmallEmber_Step(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        if(sprite->data[5] > 10000)
            sprite->subpriority = 1;

        sprite->x2 = Sin(sprite->data[0], sprite->data[1] + (sprite->data[5] >> 8));
        sprite->y2 = Cos(sprite->data[0], sprite->data[1] + (sprite->data[5] >> 8));

        sprite->data[0] += sprite->data[2];
        sprite->data[5] += sprite->data[4];

        if (sprite->data[0] > 255)
            sprite->data[0] -= 256;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 256;

        sprite->data[3]--;
    }
    else
    {
        DestroySpriteAndMatrix(sprite);
    }
}
