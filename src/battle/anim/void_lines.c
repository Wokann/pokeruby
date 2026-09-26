#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "palette.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimVoidLines(struct Sprite *sprite);
static void AnimVoidLines_Step(struct Sprite *sprite);

// Unused

const struct SpriteTemplate gVoidLinesSpriteTemplate =
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
    InitSpritePosToAnimAttacker(sprite, FALSE);
    sprite->data[0] = OBJ_PLTT_ID(IndexOfSpritePaletteTag(gVoidLinesSpriteTemplate.paletteTag));
    sprite->callback = AnimVoidLines_Step;
}

static void AnimVoidLines_Step(struct Sprite *sprite)
{
    u16 id, val;
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

        if (++sprite->data[2] == 24)
            DestroyAnimSprite(sprite);
    }
}
