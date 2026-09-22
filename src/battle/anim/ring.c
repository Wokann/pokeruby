#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "main.h"
#include "blend_palette.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern u8 gBattlerSpriteIds[];

static void AnimBlendThinRing(struct Sprite* sprite);
static void AnimHyperVoiceRing(struct Sprite* sprite);
static void AnimUproarRing(struct Sprite* sprite);
static void AnimSoftBoiledEgg(struct Sprite *sprite);
static void AnimHyperVoiceRing_WaitEnd(struct Sprite* sprite);
static void AnimSoftBoiledEgg_Step1(struct Sprite *sprite);
static void AnimSoftBoiledEgg_Step2(struct Sprite *sprite);
static void AnimSoftBoiledEgg_Step3(struct Sprite *sprite);
static void AnimSoftBoiledEgg_Step3_Callback1(struct Sprite *sprite);
static void AnimSoftBoiledEgg_Step3_Callback2(struct Sprite *sprite);
static void AnimSoftBoiledEgg_Step4(struct Sprite *sprite);
static void AnimSoftBoiledEgg_Step4_Callback(struct Sprite *sprite);

// ring (a ring that stretches outward from the Pokemon.)
// Used in Aromatherapy and Heal Bell.

const struct SpriteTemplate gSnoreZSpriteTemplate =
{
    .tileTag = ANIM_TAG_SNORE_Z,
    .paletteTag = ANIM_TAG_SNORE_Z,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimTravelDiagonally,
};

const union AnimCmd gExplosionAnimCmds[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gExplosionAnimTable[] =
{
    gExplosionAnimCmds,
};

const struct SpriteTemplate gExplosionSpriteTemplate =
{
    .tileTag = ANIM_TAG_EXPLOSION,
    .paletteTag = ANIM_TAG_EXPLOSION,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gExplosionAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSpriteOnMonPos,
};

const union AffineAnimCmd gSoftBoiledEggAffineAnimCmds1[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -8, 2),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 8, 4),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -8, 2),
    AFFINEANIMCMD_JUMP(0),
};

const union AffineAnimCmd gSoftBoiledEggAffineAnimCmds2[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSoftBoiledEggAffineAnimCmds3[] =
{
    AFFINEANIMCMD_FRAME(0xFFF8, 0x4, 0, 8),
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0x10, 0xFFF8, 0, 8),
    AFFINEANIMCMD_FRAME(0xFFF0, 0x8, 0, 8),
    AFFINEANIMCMD_LOOP(1),
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 15),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSoftBoiledEggAffineAnimTable[] =
{
    gSoftBoiledEggAffineAnimCmds1,
    gSoftBoiledEggAffineAnimCmds2,
    gSoftBoiledEggAffineAnimCmds3,
};

const struct SpriteTemplate gSoftBoiledEggSpriteTemplate =
{
    .tileTag = ANIM_TAG_BREAKING_EGG,
    .paletteTag = ANIM_TAG_BREAKING_EGG,
    .oam = &gOamData_AffineDouble_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSoftBoiledEggAffineAnimTable,
    .callback = AnimSoftBoiledEgg,
};

const union AffineAnimCmd gThinRingExpandingAffineAnimCmds1[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 30),
    AFFINEANIMCMD_END_ALT(1),
};

const union AffineAnimCmd gThinRingExpandingAffineAnimCmds2[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 15),
    AFFINEANIMCMD_END_ALT(1),
};

const union AffineAnimCmd gHyperVoiceRingAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_FRAME(0xB, 0xB, 0, 45),
    AFFINEANIMCMD_END_ALT(1),
};

const union AffineAnimCmd *const gThinRingExpandingAffineAnimTable[] =
{
    gThinRingExpandingAffineAnimCmds1,
    gThinRingExpandingAffineAnimCmds2,
};

const union AffineAnimCmd *const gHyperVoiceRingAffineAnimTable[] =
{
    gHyperVoiceRingAffineAnimCmds,
};

const struct SpriteTemplate gThinRingExpandingSpriteTemplate =
{
    .tileTag = ANIM_TAG_THIN_RING,
    .paletteTag = ANIM_TAG_THIN_RING,
    .oam = &gOamData_AffineDouble_ObjNormal_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gThinRingExpandingAffineAnimTable,
    .callback = AnimSpriteOnMonPos,
};

const union AffineAnimCmd gThinRingShrinkingAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x200, 0x200, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFF0, 0xFFF0, 0, 30),
    AFFINEANIMCMD_END_ALT(1),
};

const union AffineAnimCmd *const gThinRingShrinkingAffineAnimTable[] =
{
    gThinRingShrinkingAffineAnimCmds,
};

const struct SpriteTemplate gThinRingShrinkingSpriteTemplate =
{
    .tileTag = ANIM_TAG_THIN_RING,
    .paletteTag = ANIM_TAG_THIN_RING,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gThinRingShrinkingAffineAnimTable,
    .callback = AnimSpriteOnMonPos,
};

const struct SpriteTemplate gBlendThinRingExpandingSpriteTemplate =
{
    .tileTag = ANIM_TAG_THIN_RING,
    .paletteTag = ANIM_TAG_THIN_RING,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gThinRingExpandingAffineAnimTable,
    .callback = AnimBlendThinRing,
};

const struct SpriteTemplate gHyperVoiceRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_THIN_RING,
    .paletteTag = ANIM_TAG_THIN_RING,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gHyperVoiceRingAffineAnimTable,
    .callback = AnimHyperVoiceRing,
};

const struct SpriteTemplate gUproarRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_THIN_RING,
    .paletteTag = ANIM_TAG_THIN_RING,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gThinRingExpandingAffineAnimTable,
    .callback = AnimUproarRing,
};

static void AnimBlendThinRing(struct Sprite* sprite)
{
    u8 bank = 0;
    u16 sp0 = 0;
    u16 sp1 = 0;
    u8 r4;

    if (gBattleAnimArgs[2] == 0)
        bank = gBattleAnimAttacker;
    else
        bank = gBattleAnimTarget;

    r4 = gBattleAnimArgs[3] ^ 1;
    if (IsDoubleBattle() && IsAnimBankSpriteVisible(bank ^ 2))
    {
        SetAverageBattlerPositions(bank, r4, &sp0, &sp1);
        if (r4 == 0)
            r4 = GetBattlerSpriteCoord(bank, 0);
        else
            r4 = GetBattlerSpriteCoord(bank, 2);

        if (GetBattlerSide(bank) != 0)
            gBattleAnimArgs[0] -= (sp0 - r4) - gBattleAnimArgs[0];  // This is weird.
        else
            gBattleAnimArgs[0] = sp0 - r4;
    }

    sprite->callback = AnimSpriteOnMonPos;
    AnimSpriteOnMonPos(sprite);
}

static void AnimHyperVoiceRing_WaitEnd(struct Sprite* sprite)
{
    if (AnimTranslateLinear(sprite))
    {
        FreeSpriteOamMatrix(sprite);
        DestroyAnimSprite(sprite);
    }
}

static void AnimHyperVoiceRing(struct Sprite* sprite)
{
    u16 r9 = 0;
    u16 r6 = 0;
    s16 sp0 = 0;
    s16 sp1 = 0;
    u8 sp4;
    u8 bankr7;
    u8 bankr8;
    u8 r10;

    if (gBattleAnimArgs[5] == 0)
    {
        bankr7 = gBattleAnimAttacker;
        bankr8 = gBattleAnimTarget;
    }
    else
    {
        bankr7 = gBattleAnimTarget;
        bankr8 = gBattleAnimAttacker;
    }

    if (gBattleAnimArgs[6] == 0)
    {
        r10 = 0;
        sp4 = 1;
    }
    else
    {
        r10 = 2;
        sp4 = 3;
    }

    if (GetBattlerSide(bankr7) != 0)
    {
        r9 = GetBattlerSpriteCoord(bankr7, r10) + gBattleAnimArgs[0];
        if (IsAnimBankSpriteVisible(bankr8 ^ 2))
            sprite->subpriority = gSprites[gBattlerSpriteIds[bankr8 ^ 2]].subpriority - 1;
        else
            sprite->subpriority = gSprites[gBattlerSpriteIds[bankr8]].subpriority - 1;
    }
    else
    {
        r9 = GetBattlerSpriteCoord(bankr7, r10) - gBattleAnimArgs[0];
        if (gMain.inBattle && IsAnimBankSpriteVisible(bankr7 ^ 2))
        {
            if (gSprites[gBattlerSpriteIds[bankr7]].x < gSprites[gBattlerSpriteIds[bankr7 ^ 2]].x)
                sprite->subpriority = gSprites[gBattlerSpriteIds[bankr7 ^ 2]].subpriority + 1;
            else
                sprite->subpriority = gSprites[gBattlerSpriteIds[bankr7]].subpriority - 1;
        }
        else
        {
            sprite->subpriority = gSprites[gBattlerSpriteIds[bankr7]].subpriority - 1;
        }

    }

    r6 = GetBattlerSpriteCoord(bankr7, sp4) + gBattleAnimArgs[1];
    if (gMain.inBattle && IsAnimBankSpriteVisible(bankr8 ^ 2))
    {
        SetAverageBattlerPositions(bankr8, gBattleAnimArgs[6], &sp0, &sp1);
    }
    else
    {
        sp0 = GetBattlerSpriteCoord(bankr8, r10);
        sp1 = GetBattlerSpriteCoord(bankr8, sp4);
    }

    if (GetBattlerSide(bankr8))
        sp0 += gBattleAnimArgs[3];
    else
        sp0 -= gBattleAnimArgs[3];

    sp1 += gBattleAnimArgs[4];
    sprite->x = sprite->data[1] = r9;
    sprite->y = sprite->data[3] = r6;
    sprite->data[2] = sp0;
    sprite->data[4] = sp1;
    sprite->data[0] = gBattleAnimArgs[0];
    InitAnimLinearTranslation(sprite);
    sprite->callback = AnimHyperVoiceRing_WaitEnd;
    AnimHyperVoiceRing_WaitEnd(sprite);
}

static void AnimUproarRing(struct Sprite* sprite)
{
    u8 index = IndexOfSpritePaletteTag(0x27DB);
    if (index != 0xFF)
    {
        BlendPalette(((index << 20) + 0x1010000) >> 16, 15, gBattleAnimArgs[5], gBattleAnimArgs[4]);
    }

    StartSpriteAffineAnim(sprite, 1);
    sprite->callback = AnimSpriteOnMonPos;
    AnimSpriteOnMonPos(sprite);
}

static void AnimSoftBoiledEgg(struct Sprite *sprite)
{
    s16 r1;
    InitSpritePosToAnimAttacker(sprite, 0);
    r1 = (GetBattlerSide(gBattleAnimAttacker)) ? -0xA0 : 0xA0;
    sprite->data[0] = 0x380;
    sprite->data[1] = r1;
    sprite->data[7] = gBattleAnimArgs[2];
    sprite->callback = AnimSoftBoiledEgg_Step1;
}

static void AnimSoftBoiledEgg_Step1(struct Sprite *sprite)
{
    s16 add;
    sprite->y2 -= (sprite->data[0] >> 8);
    sprite->x2 = sprite->data[1] >> 8;
    sprite->data[0] -= 32;
    add = (GetBattlerSide(gBattleAnimAttacker)) ? -0xA0 : 0xA0;
    sprite->data[1] += add;
    if (sprite->y2 > 0)
    {
        sprite->y += sprite->y2;
        sprite->x += sprite->x2;
        sprite->y2 = 0;
        sprite->x2 = 0;
        sprite->data[0] = 0;
        StartSpriteAffineAnim(sprite, 1);
        sprite->callback = AnimSoftBoiledEgg_Step2;
    }
}

static void AnimSoftBoiledEgg_Step2(struct Sprite *sprite)
{
    if (sprite->data[0]++ > 19)
    {
        StartSpriteAffineAnim(sprite, 2);
        sprite->callback = AnimSoftBoiledEgg_Step3;
    }
}

static void AnimSoftBoiledEgg_Step3(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
    {
        StartSpriteAffineAnim(sprite, 1);
        sprite->data[0] = 0;
        if (sprite->data[7] == 0)
        {
            sprite->oam.tileNum += 16;
            sprite->callback = AnimSoftBoiledEgg_Step3_Callback1;
        }
        else
        {
            sprite->oam.tileNum += 32;
            sprite->callback = AnimSoftBoiledEgg_Step4;
        }
    }
}

static void AnimSoftBoiledEgg_Step3_Callback1(struct Sprite *sprite)
{
    sprite->y2 -= 2;
    if (++sprite->data[0] == 9)
    {
        sprite->data[0] = 16;
        sprite->data[1] = 0;
        REG_BLDCNT = 0x3F40;
        REG_BLDALPHA = sprite->data[0];
        sprite->callback = AnimSoftBoiledEgg_Step3_Callback2;
    }
}

static void AnimSoftBoiledEgg_Step3_Callback2(struct Sprite *sprite)
{
    if (sprite->data[1]++ % 3 == 0)
    {
        sprite->data[0]--;
        REG_BLDALPHA = sprite->data[0] | ((16 - sprite->data[0]) << 8);
        if (sprite->data[0] == 0)
            sprite->callback = AnimSoftBoiledEgg_Step4;
    }
}

static void AnimSoftBoiledEgg_Step4(struct Sprite *sprite)
{
    if ((u16)gBattleAnimArgs[7] == 0xFFFF)
    {
        sprite->invisible = TRUE;
        if (sprite->data[7] == 0)
            sprite->callback = AnimSoftBoiledEgg_Step4_Callback;
        else
            sprite->callback = DestroyAnimSprite;
    }
}

static void AnimSoftBoiledEgg_Step4_Callback(struct Sprite *sprite)
{
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    DestroyAnimSprite(sprite);
}
