#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern const union AnimCmd *const gPowerAbsorptionOrbAnimTable[];

static void AnimMimicOrb(struct Sprite *sprite);

// copy_orb
// Used in Mimic.

const union AffineAnimCmd gMimicOrbAffineAnimCmds1[] = {
    AFFINEANIMCMD_FRAME(0, 0, 0, 0),
    AFFINEANIMCMD_FRAME(48, 48, 0, 14),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gMimicOrbAffineAnimCmds2[] = {
    AFFINEANIMCMD_FRAME(-16, -16, 0, 1),
    AFFINEANIMCMD_JUMP(0),
};

const union AffineAnimCmd *const gMimicOrbAffineAnimTable[] = {
    gMimicOrbAffineAnimCmds1,
    gMimicOrbAffineAnimCmds2,
};

const struct SpriteTemplate gMimicOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_ORBS,
    .paletteTag = ANIM_TAG_ORBS,
    .oam = &gOamData_AffineDouble_ObjNormal_16x16,
    .anims = gPowerAbsorptionOrbAnimTable,
    .images = NULL,
    .affineAnims = gMimicOrbAffineAnimTable,
    .callback = AnimMimicOrb,
};

static void AnimMimicOrb(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
    {
        if (GetBattlerSide(gBattleAnimTarget) == 0)
        {
            s16 a = gBattleAnimArgs[0];
            gBattleAnimArgs[0] = -a;
        }

        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 0) + gBattleAnimArgs[0];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 1) + gBattleAnimArgs[1];
        sprite->invisible = TRUE;
        sprite->data[0]++;
        break;
    }
    case 1:
    {
        sprite->invisible = FALSE;
        if (sprite->affineAnimEnded)
        {
            ChangeSpriteAffineAnim(sprite, 1);
            sprite->data[0] = 25;
            sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
            sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
            sprite->callback = InitAndRunAnimFastLinearTranslation;
            StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
            break;
        }
    }
    }
}
