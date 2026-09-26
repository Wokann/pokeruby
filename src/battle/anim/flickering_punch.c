#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimFlickeringPunch(struct Sprite *sprite);

// Unused flickering punch effect.

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_Normal[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_TurnedTopLeft[] = {
    AFFINEANIMCMD_FRAME(256, 256, 32, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_TurnedLeft[] = {
    AFFINEANIMCMD_FRAME(256, 256, 64, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_TurnedBottomLeft[] = {
    AFFINEANIMCMD_FRAME(256, 256, 96, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_UpsideDown[] = {
    AFFINEANIMCMD_FRAME(256, 256, -128, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_TurnedBottomRight[] = {
    AFFINEANIMCMD_FRAME(256, 256, -96, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_TurnedRight[] = {
    AFFINEANIMCMD_FRAME(256, 256, -64, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_FlickeringPunch_TurnedTopRight[] = {
    AFFINEANIMCMD_FRAME(256, 256, -32, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_FlickeringPunch[] = {
    sAffineAnim_FlickeringPunch_Normal,
    sAffineAnim_FlickeringPunch_TurnedTopLeft,
    sAffineAnim_FlickeringPunch_TurnedLeft,
    sAffineAnim_FlickeringPunch_TurnedBottomLeft,
    sAffineAnim_FlickeringPunch_UpsideDown,
    sAffineAnim_FlickeringPunch_TurnedBottomRight,
    sAffineAnim_FlickeringPunch_TurnedRight,
    sAffineAnim_FlickeringPunch_TurnedTopRight,
};

static const struct SpriteTemplate sFlickeringPunchSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_FlickeringPunch,
    .callback = AnimFlickeringPunch,
};

static void AnimFlickeringPunch(struct Sprite *sprite)
{
    sprite->x += gBattleAnimArgs[0];
    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[3] = gBattleAnimArgs[4];
    sprite->data[5] = gBattleAnimArgs[5];
    StartSpriteAffineAnim(sprite, gBattleAnimArgs[6]);
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = TranslateSpriteLinearAndFlicker;
}
