#include "global.h"
#include "battle_anim.h"
#include "random.h"
#include "rom_8077ABC.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattlerAttacker;
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u8 gBattlerSpriteIds[];
extern u8 gBattlerPositions[];
extern u16 gBattle_BG1_X;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG2_X;
extern u16 gBattle_BG2_Y;
extern u16 gBattle_BG3_X;
extern u16 gBattle_BG3_Y;
extern u16 gBattle_WIN0H;
extern u16 gBattle_WIN0V;
extern u16 gBattlerPartyIndexes[];
extern u8 gAnimMoveTurn;

extern struct SpriteTemplate gBasicHitSplatSpriteTemplate;

void sub_080B08A0(struct Sprite *sprite);
void sub_80D902C(struct Sprite *sprite);
void sub_80D9078(struct Sprite *sprite);
void AnimBasicFistOrFoot(struct Sprite *sprite);
void sub_80D90F4(struct Sprite *sprite);
void sub_80D92D0(struct Sprite *sprite);
void sub_80D9378(struct Sprite *sprite);
void AnimSpinningKickOrPunch(struct Sprite *sprite);
void AnimSpinningKickOrPunch(struct Sprite *sprite);
void AnimStompFoot(struct Sprite *sprite);
void AnimDizzyPunchDuck(struct Sprite *sprite);
void sub_80D95D0(struct Sprite *sprite);
void sub_80D96B8(struct Sprite *sprite);
static void AnimSuperpowerOrb(struct Sprite *sprite);
static void AnimSuperpowerRock(struct Sprite *sprite);
static void AnimSuperpowerFireball(struct Sprite *sprite);
void sub_80D9B48(struct Sprite *sprite);
void sub_80D9BD4(struct Sprite *sprite);
void sub_80D9C40(struct Sprite *sprite);
static void sub_80D927C(struct Sprite *sprite);
static void sub_80D9328(struct Sprite *sprite);
static void sub_80D9404(struct Sprite *sprite);
static void AnimSpinningKickOrPunchFinish(struct Sprite *sprite);
static void AnimStompFootStep(struct Sprite *sprite);
static void AnimStompFootEnd(struct Sprite *sprite);
static void sub_80D9640(struct Sprite *sprite);
static void sub_80D97A0(struct Sprite *sprite);
static void AnimSuperpowerOrb_Step(struct Sprite *sprite);
static void AnimSuperpowerRock_Step1(struct Sprite *sprite);
static void AnimSuperpowerRock_Step2(struct Sprite *sprite);

const struct SpriteTemplate gSpriteTemplate_83D9F24 =
{
    .tileTag = ANIM_TAG_HUMANOID_FOOT,
    .paletteTag = ANIM_TAG_HUMANOID_FOOT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_080B08A0,
};

const union AnimCmd gSpriteAnim_83D9F3C[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83D9F44[] =
{
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83D9F4C[] =
{
    ANIMCMD_FRAME(32, 1),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83D9F54[] =
{
    ANIMCMD_FRAME(48, 1),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83D9F5C[] =
{
    ANIMCMD_FRAME(48, 1, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83D9F64[] =
{
    gSpriteAnim_83D9F3C,
};

const union AnimCmd *const gSpriteAnimTable_83D9F68[] =
{
    gSpriteAnim_83D9F44,
    gSpriteAnim_83D9F4C,
};

const union AnimCmd *const gSpriteAnimTable_83D9F70[] =
{
    gSpriteAnim_83D9F54,
    gSpriteAnim_83D9F5C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9F78 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D902C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9F90 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D9078,
};

const struct SpriteTemplate gFistFootSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBasicFistOrFoot,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9FC0 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D90F4,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9FD8 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F70,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D92D0,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9FF0 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F68,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D9378,
};

const union AffineAnimCmd gSpriteAffineAnim_83DA008[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 20, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83DA020[] =
{
    gSpriteAffineAnim_83DA008,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA024 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_837DFF4,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83DA020,
    .callback = AnimSpinningKickOrPunch,
};

const union AffineAnimCmd gSpriteAffineAnim_83DA03C[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFFC, 0xFFFC, 20, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83DA054[] =
{
    gSpriteAffineAnim_83DA03C,
};

const struct SpriteTemplate gMegaPunchKickSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_837DFF4,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83DA054,
    .callback = AnimSpinningKickOrPunch,
};

const struct SpriteTemplate gStompFootSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F68,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimStompFoot,
};

const struct SpriteTemplate gDizzyPunchDuckSpriteTemplate =
{
    .tileTag = ANIM_TAG_DUCK,
    .paletteTag = ANIM_TAG_DUCK,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimDizzyPunchDuck,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA0A0 =
{
    .tileTag = ANIM_TAG_BLUE_LIGHT_WALL,
    .paletteTag = ANIM_TAG_BLUE_LIGHT_WALL,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D95D0,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA0B8 =
{
    .tileTag = ANIM_TAG_TORN_METAL,
    .paletteTag = ANIM_TAG_TORN_METAL,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D96B8,
};

static const union AffineAnimCmd sAffineAnim_SuperpowerOrb[] =
{
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 0),
    AFFINEANIMCMD_FRAME(0x4, 0x4, 0, 64),
    AFFINEANIMCMD_FRAME(0xFFFA, 0xFFFA, 0, 8),
    AFFINEANIMCMD_FRAME(0x6, 0x6, 0, 8),
    AFFINEANIMCMD_JUMP(2),
};

static const union AffineAnimCmd *const sAffineAnims_SuperpowerOrb[] =
{
    sAffineAnim_SuperpowerOrb,
};

const struct SpriteTemplate gSuperpowerOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .paletteTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_SuperpowerOrb,
    .callback = AnimSuperpowerOrb,
};

const struct SpriteTemplate gSuperpowerRockSpriteTemplate =
{
    .tileTag = ANIM_TAG_FLAT_ROCK,
    .paletteTag = ANIM_TAG_FLAT_ROCK,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSuperpowerRock,
};

const struct SpriteTemplate gSuperpowerFireballSpriteTemplate =
{
    .tileTag = ANIM_TAG_METEOR,
    .paletteTag = ANIM_TAG_METEOR,
    .oam = &gOamData_837DF3C,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSuperpowerFireball,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA144 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D9B48,
};

const union AnimCmd gSpriteAnim_83DA15C[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83DA16C[] =
{
    ANIMCMD_FRAME(0, 4, .vFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .vFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .vFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83DA17C[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83DA18C[] =
{
    gSpriteAnim_83DA15C,
    gSpriteAnim_83DA16C,
    gSpriteAnim_83DA17C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA198 =
{
    .tileTag = ANIM_TAG_PURPLE_SCRATCH,
    .paletteTag = ANIM_TAG_PURPLE_SCRATCH,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83DA18C,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D9BD4,
};

const union AnimCmd gSpriteAnim_83DA1B0[] =
{
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(64, 6),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83DA1BC[] =
{
    ANIMCMD_FRAME(0, 6, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 6, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83DA1C8[] =
{
    ANIMCMD_FRAME(0, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 6, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83DA1D4[] =
{
    gSpriteAnim_83DA1B0,
    gSpriteAnim_83DA1BC,
    gSpriteAnim_83DA1C8,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA1E0 =
{
    .tileTag = ANIM_TAG_PURPLE_SWIPE,
    .paletteTag = ANIM_TAG_PURPLE_SWIPE,
    .oam = &gOamData_837DF3C,
    .anims = gSpriteAnimTable_83DA1D4,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D9BD4,
};

const union AffineAnimCmd gSpriteAffineAnim_83DA1F8[] =
{
    AFFINEANIMCMD_FRAME(0x200, 0x200, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFE0, 0xFFE0, 0, 8),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83DA210[] =
{
    gSpriteAffineAnim_83DA1F8,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA214 =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_837DFF4,
    .anims = gSpriteAnimTable_83D9F64,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83DA210,
    .callback = sub_80D9C40,
};

void sub_080B08A0(struct Sprite *sprite)
{
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = 15;
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void sub_80D902C(struct Sprite *sprite)
{
    if (gBattleAnimArgs[7] == 1 && GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        gBattleAnimArgs[1] = -gBattleAnimArgs[1];
        gBattleAnimArgs[3] = -gBattleAnimArgs[3];
    }

    StartSpriteAnim(sprite, gBattleAnimArgs[6]);
    gBattleAnimArgs[6] = 0;
    sub_8079534(sprite);
}

void sub_80D9078(struct Sprite *sprite)
{
    if (IsContest())
    {
        gBattleAnimArgs[1] = -gBattleAnimArgs[1];
        gBattleAnimArgs[3] = -gBattleAnimArgs[3];
    }

    sub_80D902C(sprite);
}

// Displays a basic fist or foot sprite for a given duration.
// Used by many fighting moves (and elemental "punch" moves).
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: duration
// arg 3: ? (todo: related to initial pixel offsets)
// arg 4: anim num
void AnimBasicFistOrFoot(struct Sprite *sprite)
{
    StartSpriteAnim(sprite, gBattleAnimArgs[4]);

    if (gBattleAnimArgs[3] == 0)
        InitSpritePosToAnimAttacker(sprite, 1);
    else
        sub_8078764(sprite, TRUE);

    sprite->data[0] = gBattleAnimArgs[2];
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void sub_80D90F4(struct Sprite *sprite)
{
    u8 bank;
    s16 xMod, yMod;
    s16 x, y;

    if (gBattleAnimArgs[0] == 0)
        bank = gBattleAnimAttacker;
    else
        bank = gBattleAnimTarget;

    if (gBattleAnimArgs[2] < 0)
        gBattleAnimArgs[2] = Random() % 5;

    StartSpriteAnim(sprite, gBattleAnimArgs[2]);
    sprite->x = GetBattlerSpriteCoord(bank, 2);
    sprite->y = GetBattlerSpriteCoord(bank, 3);

    xMod = GetBattlerSpriteCoordAttr(bank, 1) / 2;
    yMod = GetBattlerSpriteCoordAttr(bank, 0) / 4;

    x = Random() % xMod;
    y = Random() % yMod;

    if (Random() & 1)
        x *= -1;
    if (Random() & 1)
        y *= -1;

    if ((gBattlerPositions[bank] & 1) == 0)
        y += 0xFFF0;

    sprite->x += x;
    sprite->y += y;

    sprite->data[0] = gBattleAnimArgs[1];
    sprite->data[7] = CreateSprite(&gBasicHitSplatSpriteTemplate, sprite->x, sprite->y, sprite->subpriority + 1);
    if (sprite->data[7] != 64)
    {
        StartSpriteAffineAnim(&gSprites[sprite->data[7]], 0);
        gSprites[sprite->data[7]].callback = SpriteCallbackDummy;
    }

    sprite->callback = sub_80D927C;
}

static void sub_80D927C(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        if (sprite->data[7] != 64)
        {
            FreeOamMatrix(gSprites[sprite->data[7]].oam.matrixNum);
            DestroySprite(&gSprites[sprite->data[7]]);
        }

        DestroyAnimSprite(sprite);
    }
    else
    {
        sprite->data[0]--;
    }
}

void sub_80D92D0(struct Sprite *sprite)
{
    sub_8078764(sprite, TRUE);
    sprite->data[0] = 30;

    if (gBattleAnimArgs[2] == 0)
    {
        sprite->data[2] = sprite->x - 20;
    }
    else
    {
        sprite->data[2] = sprite->x + 20;
        sprite->hFlip = 1;
    }

    sprite->data[4] = sprite->y - 20;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, sub_80D9328);
}

static void sub_80D9328(struct Sprite *sprite)
{
    if (++sprite->data[5] == 11)
    {
        sprite->data[2] = sprite->x - sprite->x2;
        sprite->data[4] = sprite->y - sprite->y2;
        sprite->data[0] = 8;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->y2 = 0;
        sprite->x2 = 0;

        sprite->callback = StartAnimLinearTranslation;
        StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    }
}

void sub_80D9378(struct Sprite *sprite)
{
    if ((gBattleAnimAttacker ^ 2) == gBattleAnimTarget && GetBattlerPosition(gBattleAnimTarget) < 2)
        gBattleAnimArgs[0] *= -1;

    sub_8078764(sprite, TRUE);

    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->data[0] = gBattleAnimArgs[3];
    sprite->data[1] = sprite->x;
    sprite->data[2] = sprite->x + gBattleAnimArgs[2];
    sprite->data[3] = sprite->y;
    sprite->data[4] = sprite->y;

    InitAnimLinearTranslation(sprite);

    sprite->data[5] = gBattleAnimArgs[5];
    sprite->data[6] = gBattleAnimArgs[4];
    sprite->data[7] = 0;

    sprite->callback = sub_80D9404;
}

static void sub_80D9404(struct Sprite *sprite)
{
    if (!AnimTranslateLinear(sprite))
    {
        sprite->y2 += Sin(sprite->data[7] >> 8, sprite->data[5]);
        sprite->data[7] += sprite->data[6];
    }
    else
    {
        DestroyAnimSprite(sprite);
    }
}

// Animates the spinning, shrinking kick or punch, which then
// reappears at full size. Used by moves such as MOVE_MEGA_PUNCH and MOVE_MEGA_KICK.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: anim num
// arg 3: spin duration
void AnimSpinningKickOrPunch(struct Sprite *sprite)
{
    sub_8078764(sprite, TRUE);
    StartSpriteAnim(sprite, gBattleAnimArgs[2]);
    sprite->data[0] = gBattleAnimArgs[3];

    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, AnimSpinningKickOrPunchFinish);
}

static void AnimSpinningKickOrPunchFinish(struct Sprite *sprite)
{
    StartSpriteAffineAnim(sprite, 0);
    sprite->affineAnimPaused = 1;
    sprite->data[0] = 20;

    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// Animates MOVE_STOMP's foot that slides downward.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: initial wait duration
void AnimStompFoot(struct Sprite *sprite)
{
    sub_8078764(sprite, TRUE);
    sprite->data[0] = gBattleAnimArgs[2];

    sprite->callback = AnimStompFootStep;
}

static void AnimStompFootStep(struct Sprite *sprite)
{
    if (--sprite->data[0] == -1)
    {
        sprite->data[0] = 6;
        sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
        sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);

        sprite->callback = StartAnimLinearTranslation;
        StoreSpriteCallbackInData6(sprite, AnimStompFootEnd);
    }
}

static void AnimStompFootEnd(struct Sprite *sprite)
{
    sprite->data[0] = 15;

    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimDizzyPunchDuck(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sub_8078764(sprite, TRUE);
        sprite->data[1] = gBattleAnimArgs[2];
        sprite->data[2] = gBattleAnimArgs[3];
        sprite->data[0]++;
    }
    else
    {
        sprite->data[4] += sprite->data[1];
        sprite->x2 = sprite->data[4] >> 8;
        sprite->y2 = Sin(sprite->data[3], sprite->data[2]);
        sprite->data[3] = (sprite->data[3] + 3) & 0xFF;

        if (sprite->data[3] > 100)
            sprite->invisible = sprite->data[3] % 2;

        if (sprite->data[3] > 120)
            DestroyAnimSprite(sprite);
    }
}

void sub_80D95D0(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 0);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 1);
    }

    sprite->x += gBattleAnimArgs[1];
    sprite->y += gBattleAnimArgs[2];

    sprite->data[0] = 0;
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];
    sprite->data[3] = 0;
    sprite->callback = sub_80D9640;
}

static void sub_80D9640(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        if (--sprite->data[1] == 0)
        {
            if (sprite->data[2] == 0)
                DestroyAnimSprite(sprite);
            else
                sprite->data[0]++;
        }
        break;
    case 1:
        if (++sprite->data[1] > 1)
        {
            sprite->data[1] = 0;
            sprite->data[3]++;
            if (sprite->data[3] & 1)
                sprite->x2 = 2;
            else
                sprite->x2 = -2;
        }

        if (--sprite->data[2] == 0)
            DestroyAnimSprite(sprite);
        break;
    }
}

void sub_80D96B8(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 0) + gBattleAnimArgs[2];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1) + gBattleAnimArgs[3];
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 0) + gBattleAnimArgs[2];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 1) + gBattleAnimArgs[3];
    }

    sprite->oam.tileNum += gBattleAnimArgs[1] * 16;
    sprite->data[0] = 0;

    switch (gBattleAnimArgs[1])
    {
    case 0:
        sprite->data[6] = -3;
        sprite->data[7] = -3;
        break;
    case 1:
        sprite->data[6] = 3;
        sprite->data[7] = -3;
        break;
    case 2:
        sprite->data[6] = -3;
        sprite->data[7] = 3;
        break;
    case 3:
        sprite->data[6] = 3;
        sprite->data[7] = 3;
        break;
    default:
        DestroyAnimSprite(sprite);
        return;
    }

    sprite->callback = sub_80D97A0;
}

static void sub_80D97A0(struct Sprite *sprite)
{
    sprite->x += sprite->data[6];
    sprite->y += sprite->data[7];

    if (++sprite->data[0] > 40)
        DestroyAnimSprite(sprite);
}

static void AnimSuperpowerOrb(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
    {
        sprite->x = GetBattlerSpriteCoord(gBattlerAttacker, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord(gBattlerAttacker, BATTLER_COORD_Y_PIC_OFFSET);
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimAttacker);
        sprite->data[7] = gBattleAnimTarget;
    }
    else
    {
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimTarget);
        sprite->data[7] = gBattleAnimAttacker;
    }

    sprite->data[0] = 0;
    sprite->data[1] = 12;
    sprite->data[2] = 8;
    sprite->callback = AnimSuperpowerOrb_Step;
}

static void AnimSuperpowerOrb_Step(struct Sprite *sprite)
{
    if (++sprite->data[0] == 180)
    {
        REG_BLDCNT = 0;

        sprite->data[0] = 16;
        sprite->data[1] = sprite->x;
        sprite->data[2] = GetBattlerSpriteCoord(sprite->data[7], BATTLER_COORD_X_2);
        sprite->data[3] = sprite->y;
        sprite->data[4] = GetBattlerSpriteCoord(sprite->data[7], BATTLER_COORD_Y_PIC_OFFSET);

        InitAnimLinearTranslation(sprite);
        StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
        sprite->callback = AnimTranslateLinear_WithFollowup;
    }
}

static void AnimSuperpowerRock(struct Sprite *sprite)
{
    sprite->x = gBattleAnimArgs[0];
    sprite->y = 120;

    sprite->data[0] = gBattleAnimArgs[3];
    StorePointerInVars(&sprite->data[4], &sprite->data[5], (void *)(sprite->y << 8));

    sprite->data[6] = gBattleAnimArgs[1];
    sprite->oam.tileNum += gBattleAnimArgs[2] * 4;

    sprite->callback = AnimSuperpowerRock_Step1;
}

static void AnimSuperpowerRock_Step1(struct Sprite *sprite)
{
    void *var0;

    if (sprite->data[0] != 0)
    {
        var0 = LoadPointerFromVars(sprite->data[4], sprite->data[5]);
        var0 -= sprite->data[6];
        StorePointerInVars(&sprite->data[4], &sprite->data[5], var0);

        var0 = (void *)(((intptr_t)var0) >> 8);
        sprite->y = (intptr_t)var0;
        if (sprite->y < -8)
            DestroyAnimSprite(sprite);
        else
            sprite->data[0]--;
    }
    else
    {
        s16 pos0 = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
        s16 pos1 = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
        s16 pos2 = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
        s16 pos3 = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);

        sprite->data[0] = pos2 - pos0;
        sprite->data[1] = pos3 - pos1;
        sprite->data[2] = sprite->x << 4;
        sprite->data[3] = sprite->y << 4;

        sprite->callback = AnimSuperpowerRock_Step2;
    }
}

static void AnimSuperpowerRock_Step2(struct Sprite *sprite)
{
    u16 edgeX;

    sprite->data[2] += sprite->data[0];
    sprite->data[3] += sprite->data[1];
    sprite->x = sprite->data[2] >> 4;
    sprite->y = sprite->data[3] >> 4;

    edgeX = sprite->x + 8;
    if (edgeX > 256 || sprite->y < -8 || sprite->y > 120)
        DestroyAnimSprite(sprite);
}

static void AnimSuperpowerFireball(struct Sprite *sprite)
{
    u8 battler;

    if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
    {
        sprite->x = GetBattlerSpriteCoord(gBattlerAttacker, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord(gBattlerAttacker, BATTLER_COORD_Y_PIC_OFFSET);
        battler = gBattleAnimTarget;
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimAttacker);
    }
    else
    {
        battler = gBattleAnimAttacker;
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimTarget);
    }

    if (IsContest())
        sprite->oam.matrixNum |= 0x8;
    else if (GetBattlerSide(battler) == B_SIDE_PLAYER)
        sprite->oam.matrixNum |= 0x18;

    sprite->data[0] = 16;
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(battler, BATTLER_COORD_X_2);
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(battler, BATTLER_COORD_Y_PIC_OFFSET);

    InitAnimLinearTranslation(sprite);
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = AnimTranslateLinear_WithFollowup;
}

static void sub_80D9B24(struct Sprite *sprite)
{
    if (sprite->data[0] == sprite->data[4])
        DestroyAnimSprite(sprite);

    sprite->data[0]++;
}

void sub_80D9B48(struct Sprite *sprite)
{
    u8 turn;

    sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[0];
    sprite->data[3] = gBattleAnimArgs[1];
    sprite->data[4] = gBattleAnimArgs[2];

    turn = gAnimMoveTurn;
    if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
        turn++;

    if (turn & 1)
    {
        sprite->data[2] = -sprite->data[2];
        sprite->data[1]++;
    }

    StartSpriteAnim(sprite, sprite->data[1]);
    sprite->x2 = sprite->data[2];
    sprite->y2 = sprite->data[3];
    sprite->callback = sub_80D9B24;
}

void sub_80D9BD4(struct Sprite *sprite)
{
    if (gBattleAnimArgs[2] == 0)
        InitSpritePosToAnimAttacker(sprite, 0);
    else
        sub_8078764(sprite, FALSE);

    if (IsContest())
    {
        StartSpriteAnim(sprite, 2);
    }
    else if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        StartSpriteAnim(sprite, 1);
    }

    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void sub_80D9C40(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
    {
        sprite->data[1] = (sprite->data[1] + 40) & 0xFF;
        sprite->x2 = Sin(sprite->data[1], 2);

        if (++sprite->data[0] > 40)
        {
            DestroyAnimSprite(sprite);
        }
    }
}

void sub_80D9C80(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        UpdateAnimBg3ScreenSize(FALSE);
        task->data[8] = gBattleAnimArgs[0];
        task->data[0]++;
        break;
    case 1:
        if (--task->data[8] == -1)
            task->data[0]++;
        break;
    case 2:
    default:
        task->data[9] += 1280;
        break;
    }

    task->data[10] += 2816;

    if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
        gBattle_BG3_X += task->data[9] >> 8;
    else
        gBattle_BG3_X -= task->data[9] >> 8;

    gBattle_BG3_Y += task->data[10] >> 8;

    task->data[9] &= 0xFF;
    task->data[10] &= 0xFF;

    if (gBattleAnimArgs[7] == -1)
    {
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;
        UpdateAnimBg3ScreenSize(TRUE);
        DestroyAnimVisualTask(taskId);
    }
}
