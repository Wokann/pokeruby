#include "global.h"
#include "battle_anim.h"
#include "rom_8077ABC.h"
#include "trig.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern const union AnimCmd *const gMusicNotesAnimTable[];

void AnimPerishSongMusicNote2(struct Sprite *sprite);
void AnimPerishSongMusicNote(struct Sprite *sprite);
static void AnimPerishSongMusicNote_Step1(struct Sprite *);
static void AnimPerishSongMusicNote_Step2(struct Sprite *);

// note_spin (spins music notes around, and rotates them)
// Used in Perish Song.

const union AffineAnimCmd gPerishSongMusicNoteAffineAnimCmds1[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 5),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gPerishSongMusicNoteAffineAnimCmds2[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -8, 16),
    AFFINEANIMCMD_END_ALT(1),
};

const union AffineAnimCmd gPerishSongMusicNoteAffineAnimCmds3[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 8, 16),
    AFFINEANIMCMD_END_ALT(1),
};

const union AffineAnimCmd *const gPerishSongMusicNoteAffineAnimTable[] =
{
    gPerishSongMusicNoteAffineAnimCmds1,
    gPerishSongMusicNoteAffineAnimCmds2,
    gPerishSongMusicNoteAffineAnimCmds3,
};

const struct SpriteTemplate gPerishSongMusicNoteSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES_2,
    .paletteTag = ANIM_TAG_MUSIC_NOTES_2,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gMusicNotesAnimTable,
    .images = NULL,
    .affineAnims = gPerishSongMusicNoteAffineAnimTable,
    .callback = AnimPerishSongMusicNote,
};

const struct SpriteTemplate gPerishSongMusicNote2SpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES_2,
    .paletteTag = ANIM_TAG_MUSIC_NOTES_2,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gMusicNotesAnimTable,
    .images = NULL,
    .affineAnims = gPerishSongMusicNoteAffineAnimTable,
    .callback = AnimPerishSongMusicNote2,
};

void AnimPerishSongMusicNote2(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->data[1] = 120 - gBattleAnimArgs[0];
        sprite->invisible = TRUE;
    }

    if (++sprite->data[0] == sprite->data[1])
    {
        SetGrayscaleOrOriginalPalette(sprite->oam.paletteNum + 16, FALSE);
    }

    if (sprite->data[0] == sprite->data[1] + 80)
    {
        DestroyAnimSprite(sprite);
    }
}

void AnimPerishSongMusicNote(struct Sprite *sprite)
{
    int index;
    int var2;

    if (sprite->data[0] == 0)
    {
        sprite->x = 120;
        sprite->y = (gBattleAnimArgs[0] + (((u16)gBattleAnimArgs[0]) >> 31)) / 2 - 15;

        StartSpriteAnim(sprite, gBattleAnimArgs[1]);

        sprite->data[5] = 120;
        sprite->data[3] = gBattleAnimArgs[2];
    }

    sprite->data[0]++;

    sprite->data[1] = (sprite->data[0] + ((u16)sprite->data[0] >> 31)) / 2;
    index = ((sprite->data[0] * 3) + (u16)sprite->data[3]);
    var2 = 0xFF;
    sprite->data[6] = (sprite->data[6] + 10) & 0xFF;

    index &= var2;
    sprite->x2 = Cos(index, 100);

    sprite->y2 = sprite->data[1] + Sin(index, 10) + Cos(sprite->data[6], 4);

    if (sprite->data[0] > sprite->data[5])
    {
        sprite->callback = AnimPerishSongMusicNote_Step1;

        sprite->data[0] = 0;
        SetSpritePrimaryCoordsFromSecondaryCoords(sprite);
        sprite->data[2] = 5;
        sprite->data[4] = 0;
        sprite->data[3] = 0;

        StartSpriteAffineAnim(sprite, 1);
    }
}

static void AnimPerishSongMusicNote_Step1(struct Sprite *sprite)
{
    if (++sprite->data[0] > 10)
    {
        sprite->data[0] = 0;
        sprite->callback = AnimPerishSongMusicNote_Step2;
    }
}

static void AnimPerishSongMusicNote_Step2(struct Sprite *sprite)
{
    sprite->data[3] += sprite->data[2];
    sprite->y2 = sprite->data[3];

    sprite->data[2]++;

    if (sprite->data[3] > 48 && sprite->data[2] > 0)
    {
        sprite->data[2] = sprite->data[4] - 5;
        sprite->data[4]++;
    }

    if (sprite->data[4] > 3)
    {
        int var1 = sprite->data[2];
        sprite->invisible = var1 - (((s32)(var1 + ((u32)var1 >> 31)) >> 1) << 1);
        DestroyAnimSprite(sprite);
    }

    if (sprite->data[4] == 4)
    {
        DestroyAnimSprite(sprite);
    }
}
