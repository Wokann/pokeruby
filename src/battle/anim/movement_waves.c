#include "global.h"
#include "battle_anim.h"
#include "rom_8077ABC.h"
#include "sprite.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimMovementWaves(struct Sprite *sprite);
static void AnimMovementWaves_Step(struct Sprite *sprite);

const union AnimCmd gMovementWavesAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(16, 8),
    ANIMCMD_FRAME(32, 8),
    ANIMCMD_FRAME(16, 8),
    ANIMCMD_END,
};

const union AnimCmd gMovementWavesAnimCmds2[] =
{
    ANIMCMD_FRAME(16, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 8, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gMovementWavesAnimTable[] =
{
    gMovementWavesAnimCmds1,
    gMovementWavesAnimCmds2,
};

const struct SpriteTemplate gMovementWavesSpriteTemplate =
{
    .tileTag = ANIM_TAG_MOVEMENT_WAVES,
    .paletteTag = ANIM_TAG_MOVEMENT_WAVES,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gMovementWavesAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimMovementWaves,
};

static void AnimMovementWaves(struct Sprite *sprite)
{
    if (gBattleAnimArgs[2] == 0)
    {
        DestroyAnimSprite(sprite);
    }
    else
    {
        if (gBattleAnimArgs[0] == 0)
        {
            sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
            sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
        }
        else
        {
            sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
            sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);
        }

        if (gBattleAnimArgs[1] == 0)
            sprite->x += 32;
        else
            sprite->x -= 32;

        sprite->data[0] = gBattleAnimArgs[2];
        sprite->data[1] = gBattleAnimArgs[1];
        StartSpriteAnim(sprite, sprite->data[1]);
        sprite->callback = AnimMovementWaves_Step;
    }
}

static void AnimMovementWaves_Step(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        if (--sprite->data[0] != 0)
            StartSpriteAnim(sprite, sprite->data[1]);
        else
            DestroyAnimSprite(sprite);
    }
}
