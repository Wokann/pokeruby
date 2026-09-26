#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimCirclingMusicNote(struct Sprite *sprite);
static void AnimCirclingMusicNote_Step(struct Sprite *sprite);

// Unused circling music note effect.

static const union AnimCmd sAnim_CirclingMusicNote_Eighth[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_BeamedEighth[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_SlantedBeamedEighth[] =
{
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_Quarter[] =
{
    ANIMCMD_FRAME(12, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_QuarterRest[] =
{
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_EighthRest[] =
{
    ANIMCMD_FRAME(20, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_Eighth_Flipped[] =
{
    ANIMCMD_FRAME(0, 1, .vFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_BeamedEighth_Flipped[] =
{
    ANIMCMD_FRAME(4, 1, .vFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_SlantedBeamedEighth_Flipped[] =
{
    ANIMCMD_FRAME(8, 1, .vFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CirclingMusicNote_Quarter_Flipped[] =
{
    ANIMCMD_FRAME(12, 1, .vFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_CirclingMusicNote[] =
{
    sAnim_CirclingMusicNote_Eighth,
    sAnim_CirclingMusicNote_BeamedEighth,
    sAnim_CirclingMusicNote_SlantedBeamedEighth,
    sAnim_CirclingMusicNote_Quarter,
    sAnim_CirclingMusicNote_QuarterRest,
    sAnim_CirclingMusicNote_EighthRest,
    sAnim_CirclingMusicNote_Eighth_Flipped,
    sAnim_CirclingMusicNote_BeamedEighth_Flipped,
    sAnim_CirclingMusicNote_SlantedBeamedEighth_Flipped,
    sAnim_CirclingMusicNote_Quarter_Flipped,
};

static const struct SpriteTemplate sCirclingMusicNoteSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES,
    .paletteTag = ANIM_TAG_MUSIC_NOTES,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = sAnims_CirclingMusicNote,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimCirclingMusicNote,
};

static void UNUSED UnusedFlickerAnim(struct Sprite *sprite)
{
    if (sprite->data[2] > 1)
    {
        if (sprite->data[3] & 1)
        {
            sprite->invisible = FALSE;
            gSprites[sprite->data[0]].invisible = FALSE;
            gSprites[sprite->data[1]].invisible = FALSE;
        }
        else
        {
            sprite->invisible = TRUE;
            gSprites[sprite->data[0]].invisible = TRUE;
            gSprites[sprite->data[1]].invisible = TRUE;
        }

        sprite->data[2] = 0;
        sprite->data[3]++;
    }
    else
    {
        sprite->data[2]++;
    }

    if (sprite->data[3] == 10)
    {
        DestroySprite(&gSprites[sprite->data[0]]);
        DestroySprite(&gSprites[sprite->data[1]]);
        DestroyAnimSprite(sprite);
    }
}

static void AnimCirclingMusicNote(struct Sprite *sprite)
{
    sprite->data[0] = gBattleAnimArgs[2];
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        sprite->x -= gBattleAnimArgs[0];
    else
        sprite->x += gBattleAnimArgs[0];

    StartSpriteAnim(sprite, gBattleAnimArgs[5]);
    sprite->data[1] = -gBattleAnimArgs[3];
    sprite->y += gBattleAnimArgs[1];
    sprite->data[3] = gBattleAnimArgs[4];
    sprite->callback = AnimCirclingMusicNote_Step;
    AnimCirclingMusicNote_Step(sprite);
}

static void AnimCirclingMusicNote_Step(struct Sprite *sprite)
{
    sprite->x2 = Cos(sprite->data[0], 100);
    sprite->y2 = Sin(sprite->data[0], 20);
    if (sprite->data[0] <= 0x7F)
        sprite->subpriority = 0;
    else
        sprite->subpriority = 14;

    sprite->data[0] = (sprite->data[0] + sprite->data[1]) & 0xFF;
    sprite->data[5] += 0x82;
    sprite->y2 += sprite->data[5] >> 8;
    sprite->data[2]++;
    if (sprite->data[2] == sprite->data[3])
        DestroyAnimSprite(sprite);
}
