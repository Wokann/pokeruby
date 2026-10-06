#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimSwordsDanceBlade(struct Sprite *sprite);
static void AnimSwordsDanceBlade_Step(struct Sprite *sprite);

// sword (sword appears and floats upward.)
// Used in Swords Dance.

const union AffineAnimCmd gSwordsDanceBladeAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x14, 0x0, 0, 12),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 32),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSwordsDanceBladeAffineAnimTable[] =
{
    gSwordsDanceBladeAffineAnimCmds,
};

const struct SpriteTemplate gSwordsDanceBladeSpriteTemplate =
{
    .tileTag = ANIM_TAG_SWORD,
    .paletteTag = ANIM_TAG_SWORD,
    .oam = &gOamData_AffineNormal_ObjBlend_32x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSwordsDanceBladeAffineAnimTable,
    .callback = AnimSwordsDanceBlade,
};

static void AnimSwordsDanceBlade(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 0);
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
    StoreSpriteCallbackInData6(sprite, AnimSwordsDanceBlade_Step);
}

static void AnimSwordsDanceBlade_Step(struct Sprite *sprite)
{
    sprite->data[0] = 6;
    sprite->data[2] = sprite->x;
    sprite->data[4] = sprite->y - 32;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
