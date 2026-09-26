#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimSleepLetterZ(struct Sprite *sprite);
static void AnimSleepLetterZ_Step(struct Sprite *sprite);

// Used by Rest and the sleep turn when the Pokemon is still asleep.

const union AnimCmd gSleepLetterZAnimCmds[] =
{
    ANIMCMD_FRAME(0, 40),
    ANIMCMD_END,
};

const union AnimCmd *const gSleepLetterZAnimTable[] =
{
    gSleepLetterZAnimCmds,
};

const union AffineAnimCmd gSleepLetterZAffineAnimCmds1[] =
{
    AFFINEANIMCMD_FRAME(0x14, 0x14, -30, 0),
    AFFINEANIMCMD_FRAME(0x8, 0x8, 1, 24),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSleepLetterZAffineAnimCmds1_2[] =
{
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 24),
    AFFINEANIMCMD_LOOP(10),
};

const union AffineAnimCmd gSleepLetterZAffineAnimCmds2[] =
{
    AFFINEANIMCMD_FRAME(0x14, 0x14, 30, 0),
    AFFINEANIMCMD_FRAME(0x8, 0x8, -1, 24),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSleepLetterZAffineAnimCmds2_2[] =
{
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 24),
    AFFINEANIMCMD_LOOP(10),
};

const union AffineAnimCmd *const gSleepLetterZAffineAnimTable[] =
{
    gSleepLetterZAffineAnimCmds1,
    gSleepLetterZAffineAnimCmds2,
};

const struct SpriteTemplate gSleepLetterZSpriteTemplate =
{
    .tileTag = ANIM_TAG_LETTER_Z,
    .paletteTag = ANIM_TAG_LETTER_Z,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gSleepLetterZAnimTable,
    .images = NULL,
    .affineAnims = gSleepLetterZAffineAnimTable,
    .callback = AnimSleepLetterZ,
};

static void AnimSleepLetterZ(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        sprite->data[3] = 1;
    }
    else
    {
        sprite->x -= gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        sprite->data[3] = 0xFFFF;
        StartSpriteAffineAnim(sprite, 1);
    }

    sprite->callback = AnimSleepLetterZ_Step;
}

static void AnimSleepLetterZ_Step(struct Sprite *sprite)
{
    sprite->y2 = -(sprite->data[0] / 0x28);
    sprite->x2 = sprite->data[4] / 10;
    sprite->data[4] += sprite->data[3] * 2;
    sprite->data[0] += sprite->data[1];
    if (++sprite->data[1] > 0x3C)
        DestroySpriteAndMatrix(sprite);
}
