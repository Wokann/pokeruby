#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern const union AnimCmd *const gPetalDanceSmallFlowerAnimTable[];
extern const union AnimCmd *const gPetalDanceBigFlowerAnimTable[];

void AnimFlyingParticle(struct Sprite* sprite);
static void AnimFlyingParticle_Step(struct Sprite* sprite);

// flying_petals (petals fly across the screen.)
// Used by Aromatherapy.

const union AffineAnimCmd gAromatherapyBigFlowerAffineAnimCmds[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(0, 0, 4, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd *const gAromatherapyBigFlowerAffineAnimTable[] = {
    gAromatherapyBigFlowerAffineAnimCmds,
};

const struct SpriteTemplate gAromatherapySmallFlowerSpriteTemplate =
{
    .tileTag = ANIM_TAG_FLOWER,
    .paletteTag = ANIM_TAG_FLOWER,
    .oam = &gOamData_837DF24,
    .anims = gPetalDanceSmallFlowerAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFlyingParticle,
};

const struct SpriteTemplate gAromatherapyBigFlowerSpriteTemplate =
{
    .tileTag = ANIM_TAG_FLOWER,
    .paletteTag = ANIM_TAG_FLOWER,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gPetalDanceBigFlowerAnimTable,
    .images = NULL,
    .affineAnims = gAromatherapyBigFlowerAffineAnimTable,
    .callback = AnimFlyingParticle,
};

const union AffineAnimCmd gSpriteAffineAnim_83D68E8[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(0, 0, -10, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd gSpriteAffineAnim_83D6900[] = {
    AFFINEANIMCMD_FRAME(192, 192, 0, 0),
    AFFINEANIMCMD_FRAME(0, 0, -12, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd gSpriteAffineAnim_83D6918[] = {
    AFFINEANIMCMD_FRAME(143, 143, 0, 0),
    AFFINEANIMCMD_FRAME(0, 0, -15, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D6930[] = {
    gSpriteAffineAnim_83D68E8,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D6934[] = {
    gSpriteAffineAnim_83D6900,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D6938[] = {
    gSpriteAffineAnim_83D6918,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D693C =
{
    .tileTag = ANIM_TAG_SPARKLE_6,
    .paletteTag = ANIM_TAG_SPARKLE_6,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D6930,
    .callback = AnimFlyingParticle,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D6954 =
{
    .tileTag = ANIM_TAG_SPARKLE_6,
    .paletteTag = ANIM_TAG_SPARKLE_6,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D6934,
    .callback = AnimFlyingParticle,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D696C =
{
    .tileTag = ANIM_TAG_SPARKLE_6,
    .paletteTag = ANIM_TAG_SPARKLE_6,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D6938,
    .callback = AnimFlyingParticle,
};

void AnimFlyingParticle(struct Sprite* sprite)
{
    u8 bank;
    if (!gBattleAnimArgs[6])
        bank = gBattleAnimAttacker;
    else
        bank = gBattleAnimTarget;

    if (GetBattlerSide(bank) != 0)
    {
        sprite->data[4] = 0;
        sprite->data[2] = gBattleAnimArgs[3];
        sprite->x = 0xFFF0;
    }
    else
    {
        sprite->data[4] = 1;
        sprite->data[2] = -gBattleAnimArgs[3];
        sprite->x = 0x100;
    }

    sprite->data[1] = gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[3] = gBattleAnimArgs[4];
    switch (gBattleAnimArgs[5])
    {
    case 0:
        sprite->y = gBattleAnimArgs[0];
        sprite->oam.priority = GetBattlerSpriteBGPriority(bank);
        break;
    case 1:
        sprite->y = gBattleAnimArgs[0];
        sprite->oam.priority = GetBattlerSpriteBGPriority(bank) + 1;
        break;
    case 2:
        sprite->y = GetBattlerSpriteCoord(bank, 3) + gBattleAnimArgs[0];
        sprite->oam.priority = GetBattlerSpriteBGPriority(bank);
        break;
    case 3:
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[0];
        GetAnimBattlerSpriteId(1);
        sprite->oam.priority = GetBattlerSpriteBGPriority(bank) + 1;
        break;
    }

    sprite->callback = AnimFlyingParticle_Step;
}

static void AnimFlyingParticle_Step(struct Sprite* sprite)
{
    int a = sprite->data[7];
    sprite->data[7]++;
    sprite->y2 = (sprite->data[1] * gSineTable[sprite->data[0]]) >> 8;
    sprite->x2 = sprite->data[2] * a;
    sprite->data[0] = (sprite->data[3] * a) & 0xFF;
    if (sprite->data[4] == 0)
    {
        if (sprite->x2 + sprite->x <= 0xF7)
            return;
    }
    else
    {
        if (sprite->x2 + sprite->x > -16)
            return;
    }

    DestroySpriteAndMatrix(sprite);
}
