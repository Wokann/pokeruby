#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "palette.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

void AnimWavyMusicNotes(struct Sprite* sprite);
void AnimFlyingMusicNotes(struct Sprite* sprite);
void AnimBellyDrumHand(struct Sprite* sprite);
void AnimSlowFlyingMusicNotes(struct Sprite* sprite);
static void AnimWavyMusicNotes_CalcVelocity(s16 x, s16 y, s16* velocX, s16* velocY, s8 xSpeedFactor);
static void AnimWavyMusicNotes_Step(struct Sprite* sprite);
static void AnimFlyingMusicNotes_Step(struct Sprite* sprite);
static void AnimSlowFlyingMusicNotes_Step(struct Sprite* sprite);

const union AnimCmd gWavyMusicNotesAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_END,
};

const union AnimCmd gWavyMusicNotesAnimCmds2[] =
{
    ANIMCMD_FRAME(4, 10),
    ANIMCMD_END,
};

const union AnimCmd gWavyMusicNotesAnimCmds3[] =
{
    ANIMCMD_FRAME(8, 41),
    ANIMCMD_END,
};

const union AnimCmd gWavyMusicNotesAnimCmds4[] =
{
    ANIMCMD_FRAME(12, 10),
    ANIMCMD_END,
};

const union AnimCmd gWavyMusicNotesAnimCmds5[] =
{
    ANIMCMD_FRAME(16, 10),
    ANIMCMD_END,
};

const union AnimCmd gWavyMusicNotesAnimCmds6[] =
{
    ANIMCMD_FRAME(20, 10),
    ANIMCMD_END,
};

const union AnimCmd gWavyMusicNotesAnimCmds7[] =
{
    ANIMCMD_FRAME(0, 10, .vFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gWavyMusicNotesAnimCmds8[] =
{
    ANIMCMD_FRAME(4, 10, .vFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gMusicNotesAnimTable[] =
{
    gWavyMusicNotesAnimCmds1,
    gWavyMusicNotesAnimCmds2,
    gWavyMusicNotesAnimCmds3,
    gWavyMusicNotesAnimCmds4,
    gWavyMusicNotesAnimCmds5,
    gWavyMusicNotesAnimCmds6,
    gWavyMusicNotesAnimCmds7,
    gWavyMusicNotesAnimCmds8,
};

const union AffineAnimCmd gWavyMusicNotesAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0xC, 0xC, 0, 16),
    AFFINEANIMCMD_FRAME(0xFFF4, 0xFFF4, 0, 16),
    AFFINEANIMCMD_JUMP(0),
};

const union AffineAnimCmd *const gMusicNotesAffineAnimTable[] =
{
    gWavyMusicNotesAffineAnimCmds,
};

const struct SpriteTemplate gWavyMusicNotesSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES,
    .paletteTag = ANIM_TAG_MUSIC_NOTES,
    .oam = &gOamData_AffineDouble_ObjNormal_16x16,
    .anims = gMusicNotesAnimTable,
    .images = NULL,
    .affineAnims = gMusicNotesAffineAnimTable,
    .callback = AnimWavyMusicNotes,
};

const u16 gParticlesColorBlendTable[][6] =
{
    {ANIM_TAG_MUSIC_NOTES,     RGB_WHITE, RGB(31, 26, 28), RGB(31, 22, 26), RGB(31, 17, 24), RGB(31, 13, 22)},
    {ANIM_TAG_BENT_SPOON,      RGB_WHITE, RGB(25, 31, 26), RGB(20, 31, 21), RGB(15, 31, 16), RGB(10, 31, 12)},
    {ANIM_TAG_SPHERE_TO_CUBE,  RGB_WHITE, RGB(31, 31, 24), RGB(31, 31, 17), RGB(31, 31, 10), RGB(31, 31, 3)},
    {ANIM_TAG_LARGE_FRESH_EGG, RGB_WHITE, RGB(26, 28, 31), RGB(21, 26, 31), RGB(16, 24, 31), RGB(12, 22, 31)},
};

const struct SpriteTemplate gFastFlyingMusicNotesSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES,
    .paletteTag = ANIM_TAG_MUSIC_NOTES,
    .oam = &gOamData_AffineDouble_ObjNormal_16x16,
    .anims = gMusicNotesAnimTable,
    .images = NULL,
    .affineAnims = gMusicNotesAffineAnimTable,
    .callback = AnimFlyingMusicNotes,
};

const struct SpriteTemplate gBellyDrumHandSpriteTemplate =
{
    .tileTag = ANIM_TAG_PURPLE_HAND_OUTLINE,
    .paletteTag = ANIM_TAG_PURPLE_HAND_OUTLINE,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBellyDrumHand,
};

const union AffineAnimCmd gSlowFlyingMusicNotesAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0xA0, 0xA0, 0, 0),
    AFFINEANIMCMD_FRAME(0x4, 0x4, 0, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd *const gSlowFlyingMusicNotesAffineAnimTable[] =
{
    gSlowFlyingMusicNotesAffineAnimCmds,
};

const struct SpriteTemplate gSlowFlyingMusicNotesSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES,
    .paletteTag = ANIM_TAG_MUSIC_NOTES,
    .oam = &gOamData_AffineDouble_ObjNormal_16x16,
    .anims = gMusicNotesAnimTable,
    .images = NULL,
    .affineAnims = gSlowFlyingMusicNotesAffineAnimTable,
    .callback = AnimSlowFlyingMusicNotes,
};

// musical (music notes, drum)
// Used by Grasswhistle, Belly Drum, and Sing.

// rainbow effect for musical notes
void AnimTask_MusicNotesRainbowBlend(u8 taskId)
{
    u16 i;
    u16 j;
    u16 index;

    index = IndexOfSpritePaletteTag(gParticlesColorBlendTable[0][0]);
    if (index != 0xFF)
    {
        index = (index << 4) + 0x100;
        for (i = 1; i < 6; i++)
        {
            gPlttBufferFaded[index + i] = gParticlesColorBlendTable[0][i];
        }
    }

    for (j = 1; j < 4; j++)
    {
        index = AllocSpritePalette(gParticlesColorBlendTable[j][0]);
        if (index != 0xFF)
        {
            index = (index << 4) + 0x100;
            for (i = 1; i < 6; i++)
            {
                gPlttBufferFaded[index + i] = gParticlesColorBlendTable[j][i];
            }
        }
    }
    DestroyAnimVisualTask(taskId);
}

// clears the rainbow effect for musical notes.
void AnimTask_MusicNotesClearRainbowBlend(u8 taskId)
{
    u16 i;
    for (i = 1; i < 4; i++)
    {
        FreeSpritePaletteByTag(gParticlesColorBlendTable[i][0]);
    }

    DestroyAnimVisualTask(taskId);
}

void AnimWavyMusicNotes(struct Sprite* sprite)
{
    u8 paletteIndex;
    u8 targetX;
    u8 targetY;
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    StartSpriteAnim(sprite, gBattleAnimArgs[0]);
    if ((paletteIndex = IndexOfSpritePaletteTag(gParticlesColorBlendTable[gBattleAnimArgs[1]][0])) != 0xFF)
        sprite->oam.paletteNum = paletteIndex;

    sprite->data[1] = gBattleAnimArgs[1];
    sprite->data[2] = 0;
    sprite->data[3] = gBattleAnimArgs[2];
    if (IsContest())
    {
        targetX = 0x30;
        targetY = 0x28;
    }
    else
    {
        targetX = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
        targetY = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    }

    sprite->data[4] = sprite->x << 4;
    sprite->data[5] = sprite->y << 4;
    AnimWavyMusicNotes_CalcVelocity(targetX - sprite->x, targetY - sprite->y, &sprite->data[6], &sprite->data[7], 0x28);
    sprite->callback = AnimWavyMusicNotes_Step;
}

static void AnimWavyMusicNotes_CalcVelocity(s16 x, s16 y, s16* velocX, s16* velocY, s8 xSpeedFactor)
{
    int scaledX;
    int duration;
    if (x < 0)
        xSpeedFactor = -xSpeedFactor;

    scaledX = x << 8;
    duration = scaledX / xSpeedFactor;
    if (duration == 0)
        duration = 1;

    *velocX = scaledX / duration;
    *velocY = (y << 8) / duration;
}

static void AnimWavyMusicNotes_Step(struct Sprite* sprite)
{
    int trigIdx;
    s16 y;
    int x;
    u8 paletteIndex;
    sprite->data[0]++;
    trigIdx = sprite->data[0] * 5 - ((sprite->data[0] * 5 / 256) << 8);
    sprite->data[4] += sprite->data[6];
    sprite->data[5] += sprite->data[7];
    sprite->x = sprite->data[4] >> 4;
    sprite->y = sprite->data[5] >> 4;
    sprite->y2 = Sin(trigIdx, 15);
    y = (u16)sprite->y;
    x = (u16)sprite->x;

    if ((u32)((x + 16) << 16) > (0x110) << 16 || y < -16 || y > 0x80)
    {
        DestroySpriteAndMatrix(sprite);
    }
    else
    {
        if (sprite->data[3] && ++sprite->data[2] > sprite->data[3])
        {
            sprite->data[2] = 0;
            if (++sprite->data[1] > 3)
                sprite->data[1] = 0;

            paletteIndex = IndexOfSpritePaletteTag(gParticlesColorBlendTable[sprite->data[1]][0]);
            if (paletteIndex != 0xFF)
                sprite->oam.paletteNum = paletteIndex;
        }
    }
}

// note_scatter
// Used by Teeter Dance.

void AnimFlyingMusicNotes(struct Sprite* sprite)
{
    int xOffset;
    if (GetBattlerSide(gBattleAnimAttacker) == 1)
    {
        xOffset = gBattleAnimArgs[1];
        *(u16*)&gBattleAnimArgs[1] = -xOffset;
    }

    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) + gBattleAnimArgs[1];
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + gBattleAnimArgs[2];
    StartSpriteAnim(sprite, gBattleAnimArgs[0]);
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = sprite->x << 4;
    sprite->data[5] = sprite->y << 4;
    sprite->data[6] = (gBattleAnimArgs[1] << 4) / 5;
    sprite->data[7] = (gBattleAnimArgs[2] << 7) / 5;
    sprite->callback = AnimFlyingMusicNotes_Step;
}

static void AnimFlyingMusicNotes_Step(struct Sprite* sprite)
{
    sprite->data[4] += sprite->data[6];
    sprite->data[5] += sprite->data[7];
    sprite->x = sprite->data[4] >> 4;
    sprite->y = sprite->data[5] >> 4;
    if (sprite->data[0] > 5 && sprite->data[3] == 0)
    {
        sprite->data[2] = (sprite->data[2] + 16) & 0xFF;
        sprite->x2 = Cos(sprite->data[2], 18);
        sprite->y2 = Sin(sprite->data[2], 18);
        if (sprite->data[2] == 0)
            sprite->data[3] = 1;
    }

    if (++sprite->data[0] == 0x30)
        DestroySpriteAndMatrix(sprite);
}

// drum (using hands to slap the Pokemon's belly in a rhythm.)
// Used in Belly Drum.

void AnimBellyDrumHand(struct Sprite* sprite)
{
    s16 xOffset;
    if (gBattleAnimArgs[0] == 1)
    {
        sprite->oam.matrixNum = 8;
        xOffset = 16;
    }
    else
    {
        xOffset = -16;
    }

    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) + xOffset;
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + 8;
    sprite->data[0] = 8;
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// note_scatter_2 (slower scatter of notes.)
// Used in Belly Drum.

void AnimSlowFlyingMusicNotes(struct Sprite* sprite)
{
    s16 xDiff;
    u8 paletteIndex;
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    sprite->y += 8;
    StartSpriteAnim(sprite, gBattleAnimArgs[1]);
    paletteIndex = IndexOfSpritePaletteTag(gParticlesColorBlendTable[gBattleAnimArgs[2]][0]);
    if (paletteIndex != 0xFF)
        sprite->oam.paletteNum = paletteIndex;

    xDiff = (gBattleAnimArgs[0] == 0) ? 0xFFE0 : 0x20;
    sprite->data[0] = 40;
    sprite->data[1] = sprite->x;
    sprite->data[2] = xDiff + sprite->data[1];
    sprite->data[3] = sprite->y;
    sprite->data[4] = sprite->data[3] - 40;
    InitAnimLinearTranslation(sprite);
    sprite->data[5] = gBattleAnimArgs[3];
    sprite->callback = AnimSlowFlyingMusicNotes_Step;
}

static void AnimSlowFlyingMusicNotes_Step(struct Sprite* sprite)
{
    if (AnimTranslateLinear(sprite) == 0)
    {
        s16 xOffset;
        xOffset = Sin(sprite->data[5], 8);
        if (sprite->x2 < 0)
            xOffset = -xOffset;

        sprite->x2 += xOffset;
        sprite->y2 += Sin(sprite->data[5], 4);
        sprite->data[5] = (sprite->data[5] + 8) & 0xFF;
    }
    else
    {
        DestroyAnimSprite(sprite);
    }
}
