#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"
#include "palette.h"


static void AnimVoidLines(struct Sprite *sprite);
static void AnimVoidLines_Step(struct Sprite *sprite);

// unused_9? (Most likely an unused effect.)
// I cannot find any reference to this sprite template used to call this.

static const struct SpriteTemplate sVoidLinesSpriteTemplate =
{
    .tileTag = ANIM_TAG_VOID_LINES,
    .paletteTag = ANIM_TAG_VOID_LINES,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimVoidLines,
};

static void AnimVoidLines(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 0);
    sprite->data[0] = 0x100 + (IndexOfSpritePaletteTag(sVoidLinesSpriteTemplate.paletteTag) << 4);
    sprite->callback = AnimVoidLines_Step;
}

static void AnimVoidLines_Step(struct Sprite *sprite)
{
    u16 val;
    u16 id;
    int i;
    if (++sprite->data[1] == 2)
    {
        sprite->data[1] = 0;
        id = sprite->data[0];
        val = gPlttBufferFaded[id + 8];
        for (i = 8; i < 16; i++)
        {
            gPlttBufferFaded[id + i] = gPlttBufferFaded[id + i + 1];
        }
        gPlttBufferFaded[id + 15] = val;

        if (++sprite->data[2] == 0x18)
            DestroyAnimSprite(sprite);
    }
}
