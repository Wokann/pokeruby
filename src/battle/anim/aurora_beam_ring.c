#include "global.h"
#include "battle_anim.h"
#include "palette.h"
#include "rom_8077ABC.h"
#include "sprite.h"
#include "task.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimAuroraBeamRings(struct Sprite *sprite);
static void AnimAuroraBeamRings_Step(struct Sprite *sprite);
static void AnimTask_RotateAuroraRingColors_Step(u8 taskId);

static const union AnimCmd sAnim_AuroraBeamRing_0[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_AuroraBeamRing_1[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_AuroraBeamRing[] =
{
    sAnim_AuroraBeamRing_0,
    sAnim_AuroraBeamRing_1,
};

static const union AffineAnimCmd sAffineAnim_AuroraBeamRing[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 1),
    AFFINEANIMCMD_FRAME(0x60, 0x60, 0, 1),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_AuroraBeamRing[] =
{
    sAffineAnim_AuroraBeamRing,
};

// Multi-colored rings used in Aurora Beam.
const struct SpriteTemplate gAuroraBeamRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_RAINBOW_RINGS,
    .paletteTag = ANIM_TAG_RAINBOW_RINGS,
    .oam = &gOamData_AffineDouble_ObjNormal_8x16,
    .anims = sAnims_AuroraBeamRing,
    .images = NULL,
    .affineAnims = sAffineAnims_AuroraBeamRing,
    .callback = AnimAuroraBeamRings,
};

// Animates the colorful rings in Aurora Beam linearly towards the target mon.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: target x offset
// arg 3: target y offset
// arg 4: duration
static void AnimAuroraBeamRings(struct Sprite *sprite)
{
    s16 targetXOffset;

    InitSpritePosToAnimAttacker(sprite, TRUE);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        targetXOffset = -gBattleAnimArgs[2];
    else
        targetXOffset = gBattleAnimArgs[2];
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) + targetXOffset;
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + gBattleAnimArgs[3];
    InitAnimLinearTranslation(sprite);
    sprite->callback = AnimAuroraBeamRings_Step;
    sprite->affineAnimPaused = TRUE;
    sprite->callback(sprite);
}

// Grows the rings in Aurora Beam.
// arg 7: if -1, grow the rings
static void AnimAuroraBeamRings_Step(struct Sprite *sprite)
{
    if ((u16)gBattleAnimArgs[7] == 0xFFFF)
    {
        StartSpriteAnim(sprite, 1);
        sprite->affineAnimPaused = FALSE;
    }
    if (AnimTranslateLinear(sprite))
        DestroyAnimSprite(sprite);
}

// Updates the palette on the rainbow rings used in Aurora Beam to make them appear to be rotating counterclockwise
void AnimTask_RotateAuroraRingColors(u8 taskId)
{
    gTasks[taskId].data[0] = gBattleAnimArgs[0];
    gTasks[taskId].data[2] = OBJ_PLTT_ID(IndexOfSpritePaletteTag(ANIM_TAG_RAINBOW_RINGS));
    gTasks[taskId].func = AnimTask_RotateAuroraRingColors_Step;
}

static void AnimTask_RotateAuroraRingColors_Step(u8 taskId)
{
    int i;
    u16 palIndex;

    if (++gTasks[taskId].data[10] == 3)
    {
        u16 rgbBuffer;

        gTasks[taskId].data[10] = 0;
        palIndex = gTasks[taskId].data[2] + 1;
        rgbBuffer = gPlttBufferFaded[palIndex];
        for (i = 1; i < 8; i++)
            gPlttBufferFaded[palIndex + i - 1] = gPlttBufferFaded[palIndex + i];
        gPlttBufferFaded[palIndex + 7] = rgbBuffer;
    }
    if (++gTasks[taskId].data[11] == gTasks[taskId].data[0])
        DestroyAnimVisualTask(taskId);
}
