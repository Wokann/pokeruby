#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern void SetSpriteNextToMonHead(u8 battler, struct Sprite* sprite);

void sub_80CF040(struct Sprite* sprite);
void AnimFollowMeFinger(struct Sprite* sprite);
static void AnimTauntFinger(struct Sprite* sprite);
static void sub_80CF088(struct Sprite* sprite);
static void AnimFollowMeFinger_Step1(struct Sprite* sprite);
static void AnimFollowMeFinger_Step2(struct Sprite* sprite);
static void AnimTauntFinger_Step1(struct Sprite* sprite);
static void AnimTauntFinger_Step2(struct Sprite* sprite);

// wave_finger
// Used by Metronome, Follow Me, and Taunt.

const union AffineAnimCmd gSpriteAffineAnim_83D7238[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_FRAME(0x1E, 0x1E, 0, 8),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83D7250[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 4, 11),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -4, 11),
    AFFINEANIMCMD_LOOP(2),
    AFFINEANIMCMD_FRAME(0xFFE2, 0xFFE2, 0, 8),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83D7278[] =
{
    AFFINEANIMCMD_FRAME(16, 16, 0, 0),
    AFFINEANIMCMD_FRAME(30, 30, 0, 8),
    AFFINEANIMCMD_FRAME(0, 0, 0, 16),
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0, 0, 4, 11),
    AFFINEANIMCMD_FRAME(0, 0, -4, 11),
    AFFINEANIMCMD_LOOP(2),
    AFFINEANIMCMD_FRAME(-30, -30, 0, 8),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D72C0[] =
{
    gSpriteAffineAnim_83D7238,
    gSpriteAffineAnim_83D7250,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D72C8 =
{
    .tileTag = ANIM_TAG_FINGER,
    .paletteTag = ANIM_TAG_FINGER,
    .oam = &gOamData_AffineDouble_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D72C0,
    .callback = sub_80CF040,
};

const struct SpriteTemplate gFollowMeFingerSpriteTemplate =
{
    .tileTag = ANIM_TAG_FINGER,
    .paletteTag = ANIM_TAG_FINGER,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D72C0,
    .callback = AnimFollowMeFinger,
};

const union AnimCmd gTauntFingerAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

const union AnimCmd gTauntFingerAnimCmds2[] =
{
    ANIMCMD_FRAME(0, 1, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gTauntFingerAnimCmds3[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_END,
};

const union AnimCmd gTauntFingerAnimCmds4[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gTauntFingerAnimTable[] =
{
    gTauntFingerAnimCmds1,
    gTauntFingerAnimCmds2,
    gTauntFingerAnimCmds3,
    gTauntFingerAnimCmds4,
};

const struct SpriteTemplate gTauntFingerSpriteTemplate =
{
    .tileTag = ANIM_TAG_FINGER_2,
    .paletteTag = ANIM_TAG_FINGER_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gTauntFingerAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimTauntFinger,
};

void sub_80CF040(struct Sprite* sprite)
{
    u8 bank;
    if (gBattleAnimArgs[0] == 0)
        bank = gBattleAnimAttacker;
    else
        bank = gBattleAnimTarget;

    SetSpriteNextToMonHead(bank, sprite);
    sprite->data[0] = 0;
    StoreSpriteCallbackInData6(sprite, sub_80CF088);
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
}

static void sub_80CF088(struct Sprite* sprite)
{
    if (++sprite->data[0] > 16)
    {
        StartSpriteAffineAnim(sprite, 1);
        StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
        sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
    }
}

void AnimFollowMeFinger(struct Sprite* sprite)
{
    u8 battler;
    if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    sprite->x = GetBattlerSpriteCoord(battler, BATTLER_COORD_X);
    sprite->y = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_TOP);
    if (sprite->y <= 9)
        sprite->y = 10;

    sprite->data[0] = 1;
    sprite->data[1] = 0;
    sprite->data[2] = sprite->subpriority;
    sprite->data[3] = sprite->subpriority + 4;
    sprite->data[4] = 0;
    StoreSpriteCallbackInData6(sprite, AnimFollowMeFinger_Step1);
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
}

static void AnimFollowMeFinger_Step1(struct Sprite* sprite)
{
    if (++sprite->data[4] > 12)
        sprite->callback = AnimFollowMeFinger_Step2;
}

static void AnimFollowMeFinger_Step2(struct Sprite* sprite)
{
    s16 x1;
    s16 x2;
    sprite->data[1] += 4;
    if (sprite->data[1] > 0xFE)
    {
        if (--sprite->data[0] == 0)
        {
            sprite->x2 = 0;
            sprite->callback = sub_80CF088;
            return;
        }
        else
        {
            sprite->data[1] &= 0xFF;
        }
    }

    if (sprite->data[1] > 0x4F)
        sprite->subpriority = sprite->data[3];

    if (sprite->data[1] > 0x9F)
        sprite->subpriority = sprite->data[2];

    x1 = gSineTable[sprite->data[1]];
    x2 = x1 >> 3;
    sprite->x2 = (x1 >> 3) + (x2 >> 1);
}

static void AnimTauntFinger(struct Sprite* sprite)
{
    u8 battler;
    if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    SetSpriteNextToMonHead(battler, sprite);
    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
    {
        StartSpriteAnim(sprite, 0);
        sprite->data[0] = 2;
    }
    else
    {
        StartSpriteAnim(sprite, 1);
        sprite->data[0] = 3;
    }

    sprite->callback = AnimTauntFinger_Step1;
}

static void AnimTauntFinger_Step1(struct Sprite* sprite)
{
    if (++sprite->data[1] > 10)
    {
        sprite->data[1] = 0;
        StartSpriteAnim(sprite, sprite->data[0]);
        StoreSpriteCallbackInData6(sprite, AnimTauntFinger_Step2);
        sprite->callback = RunStoredCallbackWhenAnimEnds;
    }
}

static void AnimTauntFinger_Step2(struct Sprite* sprite)
{
    if (++sprite->data[1] > 5)
        DestroyAnimSprite(sprite);
}
