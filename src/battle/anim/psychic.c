#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "palette.h"
#include "rom_8077ABC.h"
#include "scanline_effect.h"
#include "sound.h"
#include "task.h"
#include "trig.h"
#include "constants/songs.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u16 gBattle_BG1_X;
extern u16 gBattle_BG2_X;
extern u8 gBattlerSpriteIds[];

void sub_80DB74C(struct Sprite *sprite);
void sub_80DBA4C(struct Sprite *sprite);
void sub_80DBAF4(struct Sprite *sprite);
void sub_80DBB70(struct Sprite *sprite);
static void AnimRedX(struct Sprite *sprite);
static void AnimSkillSwapOrb(struct Sprite *sprite);
void sub_80DC700(struct Sprite *sprite);
static void sub_80DB88C(struct Sprite *sprite);
static void sub_80DB8C0(struct Sprite *sprite);
static void sub_80DB92C(struct Sprite *sprite);
static void sub_80DB9E4(struct Sprite *sprite);
static void sub_80DBC00(struct Sprite *sprite);
static void sub_80DBC34(struct Sprite *sprite);
static void sub_80DBCD0(u8 taskId);
static void sub_80DBD58(u8 taskId);
static void AnimTask_ImprisonOrbs_Step(u8 taskId);
static void AnimRedX_Step(struct Sprite *sprite);
static void AnimTask_SkillSwap_Step(u8 taskId);
static void AnimTask_ExtrasensoryDistortion_Step(u8 taskId);
static void AnimTask_TransparentCloneGrowAndShrink_Step(u8 taskId);

static const union AffineAnimCmd sAffineAnim_PsychUpSpiral[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFFE, 0xFFFE, -10, 120),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_PsychUpSpiral[] =
{
    sAffineAnim_PsychUpSpiral,
};

const struct SpriteTemplate gPsychUpSpiralSpriteTemplate =
{
    .tileTag = ANIM_TAG_SPIRAL,
    .paletteTag = ANIM_TAG_SPIRAL,
    .oam = &gOamData_837E0BC,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_PsychUpSpiral,
    .callback = AnimSpriteOnMonPos,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA6A8 =
{
    .tileTag = ANIM_TAG_GREEN_LIGHT_WALL,
    .paletteTag = ANIM_TAG_GREEN_LIGHT_WALL,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DB74C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA6C0 =
{
    .tileTag = ANIM_TAG_BLUE_LIGHT_WALL,
    .paletteTag = ANIM_TAG_BLUE_LIGHT_WALL,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DB74C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA6D8 =
{
    .tileTag = ANIM_TAG_RED_LIGHT_WALL,
    .paletteTag = ANIM_TAG_RED_LIGHT_WALL,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DB74C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA6F0 =
{
    .tileTag = ANIM_TAG_GRAY_LIGHT_WALL,
    .paletteTag = ANIM_TAG_GRAY_LIGHT_WALL,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DB74C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA708 =
{
    .tileTag = ANIM_TAG_ORANGE_LIGHT_WALL,
    .paletteTag = ANIM_TAG_ORANGE_LIGHT_WALL,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DB74C,
};

const union AnimCmd gSpriteAnim_83DA720[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(48, 3),
    ANIMCMD_FRAME(64, 3),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83DA738[] =
{
    gSpriteAnim_83DA720,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA73C =
{
    .tileTag = ANIM_TAG_SPARKLE_4,
    .paletteTag = ANIM_TAG_SPARKLE_4,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83DA738,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DBA4C,
};

const union AnimCmd gSpriteAnim_83DA754[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_FRAME(12, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83DA768[] =
{
    gSpriteAnim_83DA754,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA76C =
{
    .tileTag = ANIM_TAG_SPARKLE_3,
    .paletteTag = ANIM_TAG_SPARKLE_3,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gSpriteAnimTable_83DA768,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DBA4C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA784 =
{
    .tileTag = ANIM_TAG_GOLD_RING,
    .paletteTag = ANIM_TAG_GOLD_RING,
    .oam = &gOamData_837DF74,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = TranslateAnimSpriteToTargetMonLocation,
};

const union AnimCmd gSpriteAnim_83DA79C[] =
{
    ANIMCMD_FRAME(8, 60, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 22, .hFlip = TRUE),
    ANIMCMD_LOOP(0),
    ANIMCMD_FRAME(16, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 5, .hFlip = TRUE),
    ANIMCMD_LOOP(1),
    ANIMCMD_FRAME(8, 22, .hFlip = TRUE),
    ANIMCMD_FRAME(24, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(40, 22, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83DA7DC[] =
{
    ANIMCMD_FRAME(8, 60),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(8, 22),
    ANIMCMD_LOOP(0),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_LOOP(1),
    ANIMCMD_FRAME(8, 22),
    ANIMCMD_FRAME(24, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(40, 22),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83DA81C[] =
{
    gSpriteAnim_83DA79C,
    gSpriteAnim_83DA7DC,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA824 =
{
    .tileTag = ANIM_TAG_BENT_SPOON,
    .paletteTag = ANIM_TAG_BENT_SPOON,
    .oam = &gOamData_837DF74,
    .anims = gSpriteAnimTable_83DA81C,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DBAF4,
};

const union AnimCmd gSpriteAnim_83DA83C[] =
{
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(16, 6),
    ANIMCMD_FRAME(32, 6),
    ANIMCMD_FRAME(48, 6),
    ANIMCMD_FRAME(64, 6),
    ANIMCMD_FRAME(80, 6),
    ANIMCMD_FRAME(96, 18),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83DA85C[] =
{
    gSpriteAnim_83DA83C,
};

const union AffineAnimCmd gSpriteAffineAnim_3DA860[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 4, 4),
    AFFINEANIMCMD_FRAME(0, 0, -4, 8),
    AFFINEANIMCMD_FRAME(0, 0, 4, 4),
    AFFINEANIMCMD_LOOP(2),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_083DA888[] =
{
    gSpriteAffineAnim_3DA860,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DA88C =
{
    .tileTag = ANIM_TAG_AMNESIA,
    .paletteTag = ANIM_TAG_AMNESIA,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83DA85C,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DBB70,
};

const union AffineAnimCmd gSpriteAffineAnim_083DA8A4[] =
{
    AFFINEANIMCMD_FRAME(-8, 10, 0, 16),
    AFFINEANIMCMD_FRAME(18, -18, 0, 16),
    AFFINEANIMCMD_FRAME(-20, 16, 0, 8),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_083DA8C4[] =
{
    AFFINEANIMCMD_FRAME(64, -4, 0, 20),
    AFFINEANIMCMD_FRAME(0, 0, 0, -56),
    AFFINEANIMCMD_END,
};

const struct SpriteTemplate gImprisonOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_HOLLOW_ORB,
    .paletteTag = ANIM_TAG_HOLLOW_ORB,
    .oam = &gOamData_837E04C,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

const struct SpriteTemplate gRedXSpriteTemplate =
{
    .tileTag = 10250,
    .paletteTag = 10250,
    .oam = &gOamData_837DF3C,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimRedX,
};

static const union AffineAnimCmd sAffineAnim_SkillSwapOrb_0[] =
{
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 8),
    AFFINEANIMCMD_FRAME(0x8, 0x8, 0, 8),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sAffineAnim_SkillSwapOrb_1[] =
{
    AFFINEANIMCMD_FRAME(0xF0, 0xF0, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 6),
    AFFINEANIMCMD_FRAME(0x8, 0x8, 0, 8),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 2),
    AFFINEANIMCMD_JUMP(1),
};

static const union AffineAnimCmd sAffineAnim_SkillSwapOrb_2[] =
{
    AFFINEANIMCMD_FRAME(0xD0, 0xD0, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 4),
    AFFINEANIMCMD_FRAME(0x8, 0x8, 0, 8),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 4),
    AFFINEANIMCMD_JUMP(1),
};

static const union AffineAnimCmd sAffineAnim_SkillSwapOrb_3[] =
{
    AFFINEANIMCMD_FRAME(0xB0, 0xB0, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 2),
    AFFINEANIMCMD_FRAME(0x8, 0x8, 0, 8),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 6),
    AFFINEANIMCMD_JUMP(1),
};

static const union AffineAnimCmd *const sAffineAnims_SkillSwapOrb[] =
{
    sAffineAnim_SkillSwapOrb_0,
    sAffineAnim_SkillSwapOrb_1,
    sAffineAnim_SkillSwapOrb_2,
    sAffineAnim_SkillSwapOrb_3,
};

const struct SpriteTemplate gSkillSwapOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_BLUEGREEN_ORB,
    .paletteTag = ANIM_TAG_BLUEGREEN_ORB,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_SkillSwapOrb,
    .callback = AnimSkillSwapOrb,
};

static const union AffineAnimCmd sAffineAnim_LusterPurgeCircle[] =
{
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 0),
    AFFINEANIMCMD_FRAME(0x4, 0x4, 0, 120),
    AFFINEANIMCMD_END_ALT(1),
};

static const union AffineAnimCmd *const sAffineAnims_LusterPurgeCircle[] =
{
    sAffineAnim_LusterPurgeCircle,
};

const struct SpriteTemplate gLusterPurgeCircleSpriteTemplate =
{
    .tileTag = ANIM_TAG_WHITE_CIRCLE_OF_LIGHT,
    .paletteTag = ANIM_TAG_WHITE_CIRCLE_OF_LIGHT,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_LusterPurgeCircle,
    .callback = AnimSpriteOnMonPos,
};

const union AffineAnimCmd gSpriteAffineAnim_83DA9F8[] =
{
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 0),
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 17),
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 10),
    AFFINEANIMCMD_FRAME(0x8, 0x8, 0, 10),
    AFFINEANIMCMD_LOOP(4),
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0xFFF0, 0xFFF0, 0, 5),
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 5),
    AFFINEANIMCMD_LOOP(7),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DAA50[] =
{
    AFFINEANIMCMD_FRAME(0xFFEC, 0x18, 0, 15),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83DAA60[] =
{
    gSpriteAffineAnim_83DA9F8,
    gSpriteAffineAnim_83DAA50,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DAA68 =
{
    .tileTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .paletteTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83DAA60,
    .callback = sub_80DC700,
};

void sub_80DB74C(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER || IsContest())
    {
        sprite->oam.priority = 2;
        sprite->subpriority = 200;
    }

    if (!IsContest())
    {
        u8 bankCopy;
        u8 bank = bankCopy = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        u8 identity = GetBattlerSpriteBGPriorityRank(bank);
        int var0 = 1;
        u8 toBG_2 = (identity ^ var0) != 0;

        if (IsAnimBankSpriteVisible(bank))
            MoveBattlerSpriteToBG(bank, toBG_2);

        bank = bankCopy ^ 2;
        if (IsAnimBankSpriteVisible(bank))
            MoveBattlerSpriteToBG(bank, toBG_2 ^ var0);
    }

    if (!IsContest() && IsDoubleBattle())
    {
        if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
        {
            sprite->x = 72;
            sprite->y = 80;
        }
        else
        {
            sprite->x = 176;
            sprite->y = 40;
        }
    }
    else
    {
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
            gBattleAnimArgs[0] = -gBattleAnimArgs[0];

        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 0) + gBattleAnimArgs[0];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1) + gBattleAnimArgs[1];
    }

    if (IsContest())
        sprite->y += 9;

    sprite->data[0] = 256 + IndexOfSpritePaletteTag(gBattleAnimArgs[2]) * 16;
    sprite->callback = sub_80DB88C;
    sprite->callback(sprite);
}

static void sub_80DB88C(struct Sprite *sprite)
{
    REG_BLDALPHA = ((16 - sprite->data[3]) << 8) | sprite->data[3];

    if (sprite->data[3] == 13)
        sprite->callback = sub_80DB8C0;
    else
        sprite->data[3]++;
}

static void sub_80DB8C0(struct Sprite *sprite)
{
    u16 color;
    u16 startOffset;
    int i;

    if (++sprite->data[1] == 2)
    {
        sprite->data[1] = 0;
        startOffset = sprite->data[0];
        color = gPlttBufferFaded[startOffset + 8];

        for (i = 8; i > 0; i--)
            gPlttBufferFaded[startOffset + i] = gPlttBufferFaded[startOffset + i - 1];

        gPlttBufferFaded[startOffset + 1] = color;

        if (++sprite->data[2] == 16)
            sprite->callback = sub_80DB92C;
    }
}

static void sub_80DB92C(struct Sprite *sprite)
{
    REG_BLDALPHA = ((16 - sprite->data[3]) << 8) | sprite->data[3];

    if (--sprite->data[3] == -1)
    {
        if (!IsContest())
        {
            u8 bankCopy;
            u8 bank = bankCopy = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

            if (IsAnimBankSpriteVisible(bank))
                gSprites[gBattlerSpriteIds[bank]].invisible = FALSE;

            bank = bankCopy ^ 2;
            if (IsAnimBankSpriteVisible(bank))
                gSprites[gBattlerSpriteIds[bank]].invisible = FALSE;
        }

        sprite->invisible = TRUE;
        sprite->callback = sub_80DB9E4;
    }
}

static void sub_80DB9E4(struct Sprite *sprite)
{
    if (!IsContest())
    {
        u8 bankCopy;
        u8 bank = bankCopy = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        u8 identity = GetBattlerSpriteBGPriorityRank(bank);
        int var0 = 1;
        u8 toBG_2 = (identity ^ var0) != 0;

        if (IsAnimBankSpriteVisible(bank))
            ResetBattleAnimBg(toBG_2);

        bank = bankCopy ^ 2;
        if (IsAnimBankSpriteVisible(bank))
            ResetBattleAnimBg(toBG_2 ^ var0);
    }

    sprite->callback = DestroyAnimSprite;
}

void sub_80DBA4C(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        int arg3 = gBattleAnimArgs[3];
        bool8 var0 = FALSE;
        if (arg3 == 0)
            var0 = TRUE;

        if (!IsContest() && IsDoubleBattle())
        {
            if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
            {
                sprite->x = 72 - gBattleAnimArgs[0];
                sprite->y = gBattleAnimArgs[1] + 80;
            }
            else
            {
                sprite->x = gBattleAnimArgs[0] + 176;
                sprite->y = gBattleAnimArgs[1] + 40;
            }
        }
        else
        {
            if (gBattleAnimArgs[2] == 0)
                InitSpritePosToAnimAttacker(sprite, var0);
            else
                sub_8078764(sprite, var0);
        }

        sprite->data[0]++;
    }
    else
    {
        if (sprite->animEnded || sprite->affineAnimEnded)
            DestroySpriteAndMatrix(sprite);
    }
}

void sub_80DBAF4(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);

    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        StartSpriteAnim(sprite, 1);
        sprite->x -= 40;
        sprite->y += 10;
        sprite->data[1] = -1;
    }
    else
    {
        sprite->x += 40;
        sprite->y -= 10;
        sprite->data[1] = 1;
    }

    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
}

void sub_80DBB70(struct Sprite *sprite)
{
    s16 x = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, 1) /  2;
    s16 y = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, 0) / -2;

    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_OPPONENT)
    {
        x = -x;
    }

    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) + x;
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + y;

    if (sprite->y < 16)
    {
        sprite->y = 16;
    }

    StoreSpriteCallbackInData6(sprite, sub_80DBC00);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
}

static void sub_80DBC00(struct Sprite *sprite)
{
    sprite->oam.affineMode = 1;
    sprite->affineAnims = gSpriteAffineAnimTable_083DA888;
    sprite->data[0] = 0;
    InitSpriteAffineAnim(sprite);
    sprite->callback = sub_80DBC34;
}

static void sub_80DBC34(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        if (sprite->affineAnimEnded)
        {
            FreeOamMatrix(sprite->oam.matrixNum);
            sprite->oam.affineMode = 0;
            sprite->data[1] = 18;
            sprite->data[0]++;
        }
        break;
    case 1:
        if (--sprite->data[1] == -1)
        {
            DestroyAnimSprite(sprite);
        }
        break;
    }
}

void sub_80DBC94(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u8 spriteId = GetAnimBattlerSpriteId(0);
    task->data[0] = spriteId;
    PrepareAffineAnimInTaskData(task, spriteId, &gSpriteAffineAnim_083DA8A4);
    task->func = sub_80DBCD0;
}

static void sub_80DBCD0(u8 taskId)
{
    if (!RunAffineAnimFromTaskData(&gTasks[taskId]))
    {
        DestroyAnimVisualTask(taskId);
    }
}

void sub_80DBCFC(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u8 spriteId = GetAnimBattlerSpriteId(0);
    task->data[0] = spriteId;
    task->data[1] = 0;
    task->data[2] = 0;
    task->data[3] = GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER ? 4 : 8;

    PrepareAffineAnimInTaskData(task, task->data[0], &gSpriteAffineAnim_083DA8C4);
    task->func = sub_80DBD58;
}

static void sub_80DBD58(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[1])
    {
    case 0:
        RunAffineAnimFromTaskData(task);
        if (++task->data[2] > 19)
            task->data[1]++;
        break;
    case 1:
        if (task->data[3] != 0)
        {
            gSprites[task->data[0]].y2 -= 8;
            task->data[3]--;
        }
        else
        {
            gSprites[task->data[0]].invisible = TRUE;
            gSprites[task->data[0]].x = 272;
            ResetSpriteRotScale(task->data[0]);
            DestroyAnimVisualTask(taskId);
        }
        break;
    }
}

void AnimTask_ImprisonOrbs(u8 taskId)
{
    u16 var0, var1;

    struct Task *task = &gTasks[taskId];

    task->data[3] = 16;
    task->data[4] = 0;
    task->data[13] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    task->data[14] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);

    var0 = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_WIDTH) / 3;
    var1 = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_HEIGHT) / 3;
    task->data[12] = var0 > var1 ? var0 : var1;

    REG_BLDCNT = BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND;
    REG_BLDALPHA = BLDALPHA_BLEND(16, 0);

    task->func = AnimTask_ImprisonOrbs_Step;
}

static void AnimTask_ImprisonOrbs_Step(u8 taskId)
{
    u16 i;
    u8 spriteId;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        if (++task->data[1] > 8)
        {
            task->data[1] = 0;
            spriteId = CreateSprite(&gImprisonOrbSpriteTemplate, task->data[13], task->data[14], 0);
            task->data[task->data[2] + 8] = spriteId;

            if (spriteId != MAX_SPRITES)
            {
                switch (task->data[2])
                {
                case 0:
                    gSprites[spriteId].x2 = task->data[12];
                    gSprites[spriteId].y2 = -task->data[12];
                    break;
                case 1:
                    gSprites[spriteId].x2 = -task->data[12];
                    gSprites[spriteId].y2 = task->data[12];
                    break;
                case 2:
                    gSprites[spriteId].x2 = task->data[12];
                    gSprites[spriteId].y2 = task->data[12];
                    break;
                case 3:
                    gSprites[spriteId].x2 = -task->data[12];
                    gSprites[spriteId].y2 = -task->data[12];
                    break;
                }
            }

            if (++task->data[2] == 5)
                task->data[0]++;
        }
        break;
    case 1:
        if (task->data[1] & 1)
            task->data[3]--;
        else
            task->data[4]++;

        REG_BLDALPHA = BLDALPHA_BLEND(task->data[3], task->data[4]);

        if (++task->data[1] == 32)
        {
            for (i = 8; i < 13; i++)
            {
                if (task->data[i] != MAX_SPRITES)
                    DestroySprite(&gSprites[task->data[i]]);
            }

            task->data[0]++;
        }
        break;
    case 2:
        task->data[0]++;
        break;
    case 3:
        REG_BLDALPHA = 0;
        REG_BLDCNT = 0;
        DestroyAnimVisualTask(taskId);
        break;
    }
}

static void AnimRedX_Step(struct Sprite *sprite)
{
    if (sprite->data[1] > sprite->data[0] - 10)
        sprite->invisible = sprite->data[1] & 1;

    if (sprite->data[1] == sprite->data[0])
        DestroyAnimSprite(sprite);

    sprite->data[1]++;
}

static void AnimRedX(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    }

    sprite->data[0] = gBattleAnimArgs[1];
    sprite->callback = AnimRedX_Step;
}

void AnimTask_SkillSwap(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (IsContest())
    {
        if (gBattleAnimArgs[0] == ANIM_BATTLER_TARGET)
        {
            task->data[10] = -10;
            task->data[11] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_RIGHT) - 8;
            task->data[12] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_TOP) + 8;
            task->data[13] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_RIGHT) - 8;
            task->data[14] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_TOP) + 8;
        }
        else
        {
            task->data[10] = 10;
            task->data[11] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_LEFT) + 8;
            task->data[12] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_BOTTOM) - 8;
            task->data[13] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_LEFT) + 8;
            task->data[14] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_BOTTOM) - 8;
        }
    }
    else
    {
        if (gBattleAnimArgs[0] == ANIM_BATTLER_TARGET)
        {
            task->data[10] = -10;
            task->data[11] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_LEFT) + 8;
            task->data[12] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_TOP) + 8;
            task->data[13] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_LEFT) + 8;
            task->data[14] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_TOP) + 8;
        }
        else
        {
            task->data[10] = 10;
            task->data[11] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_RIGHT) - 8;
            task->data[12] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_BOTTOM) - 8;
            task->data[13] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_RIGHT) - 8;
            task->data[14] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_BOTTOM) - 8;
        }
    }

    task->data[1] = 6;
    task->func = AnimTask_SkillSwap_Step;
}

static void AnimTask_SkillSwap_Step(u8 taskId)
{
    u8 spriteId;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        if (++task->data[1] > 6)
        {
            task->data[1] = 0;
            spriteId = CreateSprite(&gSkillSwapOrbSpriteTemplate, task->data[11], task->data[12], 0);
            if (spriteId != MAX_SPRITES)
            {
                gSprites[spriteId].data[0] = 16;
                gSprites[spriteId].data[2] = task->data[13];
                gSprites[spriteId].data[4] = task->data[14];
                gSprites[spriteId].data[5] = task->data[10];

                InitAnimArcTranslation(&gSprites[spriteId]);
                StartSpriteAffineAnim(&gSprites[spriteId], task->data[2] & 3);
            }

            if (++task->data[2] == 12)
                task->data[0]++;
        }
        break;
    case 1:
        if (++task->data[1] > 17)
            DestroyAnimVisualTask(taskId);
        break;
    }
}

static void AnimSkillSwapOrb(struct Sprite *sprite)
{
    if (TranslateAnimArc(sprite))
    {
        FreeOamMatrix(sprite->oam.matrixNum);
        DestroySprite(sprite);
    }
}

// The scanline effect that distorts the target during Extrasensory by segmenting the mon vertically and shifting the slices
// arg0: Stage. Stage 0 is a slight right distortion, 1 is a medium left distortion, and 2 is a severe right distortion
void AnimTask_ExtrasensoryDistortion(u8 taskId)
{
    s16 i;
    u8 yOffset;
    struct ScanlineEffectParams scanlineParams;
    struct Task *task = &gTasks[taskId];
    
    yOffset = GetBattlerYCoordWithElevation(gBattleAnimTarget);
    task->data[14] = yOffset - 32;

    switch (gBattleAnimArgs[0])
    {
    case 0:
        task->data[11] = 2;
        task->data[12] = 5;
        task->data[13] = 64;
        task->data[15] = yOffset + 32;
        break;
    case 1:
        task->data[11] = 2;
        task->data[12] = 5;
        task->data[13] = 192;
        task->data[15] = yOffset + 32;
        break;
    case 2:
        task->data[11] = 4;
        task->data[12] = 4;
        task->data[13] = 0;
        task->data[15] = yOffset + 32;
        break;
    }

    if (task->data[14] < 0)
        task->data[14] = 0;

    if (GetBattlerSpriteBGPriorityRank(gBattleAnimTarget) == 1)
    {
        task->data[10] = gBattle_BG1_X;
        scanlineParams.dmaDest = &REG_BG1HOFS;
    }
    else
    {
        task->data[10] = gBattle_BG2_X;
        scanlineParams.dmaDest = &REG_BG2HOFS;
    }

    i = task->data[14];
    while (i <= task->data[14] + 64)
    {
        gScanlineEffectRegBuffers[0][i] = task->data[10];
        gScanlineEffectRegBuffers[1][i] = task->data[10];
        i++;
    }

    scanlineParams.dmaControl = SCANLINE_EFFECT_DMACNT_16BIT;
    scanlineParams.initState = 1;
    scanlineParams.unused9 = 0;
    ScanlineEffect_SetParams(scanlineParams);

    task->func = AnimTask_ExtrasensoryDistortion_Step;
}

static void AnimTask_ExtrasensoryDistortion_Step(u8 taskId)
{
    s16 sineIndex, i;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        sineIndex = task->data[13];
        i = task->data[14];
        while (i <= task->data[15])
        {
            s16 var2 = (gSineTable[sineIndex] >> task->data[12]);
            if (var2 > 0)
            {
                var2 += (task->data[1] & 3);
            }
            else if (var2 < 0)
            {
                var2 -= (task->data[1] & 3);
            }

            gScanlineEffectRegBuffers[0][i] = task->data[10] + var2;
            gScanlineEffectRegBuffers[1][i] = task->data[10] + var2;

            sineIndex += task->data[11];
            i++;
        }

        if (++task->data[1] > 23)
        {
            task->data[0]++;
        }
        break;
    case 1:
        gScanlineEffect.state = 3;
        task->data[0]++;
        break;
    case 2:
        DestroyAnimVisualTask(taskId);
        break;
    }
}

// Creates a cloned transparent sprite of the battler that grows and then shrinks back to original size. Used by Extrasensory
// arg0: battler
void AnimTask_TransparentCloneGrowAndShrink(u8 taskId)
{
    s16 spriteId;
    s16 matrixNum;
    struct Task *task = &gTasks[taskId];
    
    matrixNum = AllocOamMatrix();
    if (matrixNum == 0xFF)
    {
        DestroyAnimVisualTask(taskId);
        return;
    }

    spriteId = CloneBattlerSpriteWithBlend(gBattleAnimArgs[0]);
    if (spriteId < 0)
    {
        FreeOamMatrix(matrixNum);
        DestroyAnimVisualTask(taskId);
        return;
    }

    gSprites[spriteId].callback = SpriteCallbackDummy;
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_DOUBLE;
    gSprites[spriteId].oam.matrixNum = matrixNum;
    gSprites[spriteId].affineAnimPaused = 1;
    gSprites[spriteId].subpriority++;
    SetSpriteRotScale(spriteId, 256, 256, 0);
    CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);

    task->data[13] = GetAnimBattlerSpriteId(gBattleAnimArgs[0]);
    task->data[14] = matrixNum;
    task->data[15] = spriteId;
    task->func = AnimTask_TransparentCloneGrowAndShrink_Step;
}

static void AnimTask_TransparentCloneGrowAndShrink_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        task->data[1] += 4;
        task->data[2] = 256 - (gSineTable[task->data[1]] >> 1);
        SetSpriteRotScale(task->data[15], task->data[2], task->data[2], 0);
        SetBattlerSpriteYOffsetFromOtherYScale(task->data[15], task->data[13]);
        if (task->data[1] == 48)
            task->data[0]++;
        break;
    case 1:
        task->data[1] -= 4;
        task->data[2] = 256 - (gSineTable[task->data[1]] >> 1);;
        SetSpriteRotScale(task->data[15], task->data[2], task->data[2], 0);
        SetBattlerSpriteYOffsetFromOtherYScale(task->data[15], task->data[13]);
        if (task->data[1] == 0)
            task->data[0]++;
        break;
    case 2:
        DestroySpriteWithActiveSheet(&gSprites[task->data[15]]);
        task->data[0]++;
        break;
    case 3:
        FreeOamMatrix(task->data[14]);
        DestroyAnimVisualTask(taskId);
        break;
    }
}

void sub_80DC700(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1);

        if (IsContest())
            sprite->y += 12;

        sprite->data[1] = 8;
        REG_BLDCNT = 0x3F40;
        REG_BLDALPHA = ((16 - sprite->data[1]) << 8) | sprite->data[1];
        sprite->data[0]++;
        break;
    case 1:
        if (sprite->affineAnimEnded)
        {
            PlaySE12WithPanning(SE_M_TELEPORT, BattleAnimAdjustPanning(SOUND_PAN_ATTACKER_NEG));
            ChangeSpriteAffineAnim(sprite, 1);
            sprite->data[0]++;
        }
        break;
    case 2:
        if (sprite->data[2]++ > 1)
        {
            sprite->data[2] = 0;
            sprite->data[1]--;
            REG_BLDALPHA = ((16 - sprite->data[1]) << 8) | sprite->data[1];

            if (sprite->data[1] == 0)
            {
                sprite->data[0]++;
                sprite->invisible = TRUE;
            }
        }

        sprite->data[3] += 896;
        sprite->y2 -= sprite->data[3] >> 8;
        sprite->data[3] &= 0xFF;
        break;
    case 3:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        DestroyAnimSprite(sprite);
        break;
    }
}
