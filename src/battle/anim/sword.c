#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimSwordsDanceBlade(struct Sprite *sprite);
static void AnimSwordsDanceBlade_Step(struct Sprite *sprite);

// Animates a sword that rises into the air after a brief pause.
// arg 0: x pixel offset
// arg 1: y pixel offset

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
    InitSpritePosToAnimAttacker(sprite, FALSE);
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
