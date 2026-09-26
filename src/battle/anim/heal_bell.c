#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "ewram.h"
#include "palette.h"
#include "decompress.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern const u8 gBattleAnimSpritePal_MusicNotes2[];

static void AnimHealBellMusicNote(struct Sprite *sprite);

#define NUM_MUSIC_NOTE_PAL_TAGS 5

// Used in Heal Bell.

const union AnimCmd gBellAnimCmds[] =
{
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(16, 6),
    ANIMCMD_FRAME(32, 15),
    ANIMCMD_FRAME(16, 6),
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(16, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 15, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(16, 6),
    ANIMCMD_FRAME(32, 15),
    ANIMCMD_FRAME(16, 6),
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_END,
};

const union AnimCmd *const gBellAnimTable[] =
{
    gBellAnimCmds,
};

const struct SpriteTemplate gBellSpriteTemplate =
{
    .tileTag = ANIM_TAG_BELL,
    .paletteTag = ANIM_TAG_BELL,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gBellAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSpriteOnMonPos,
};

static const u16 sMusicNotePaletteTagsTable[] =
{
    ANIM_TAG_MUSIC_NOTES_2,
    ANIM_SPRITES_START - 1,
    ANIM_SPRITES_START - 2,
    ANIM_SPRITES_START - 3,
    ANIM_SPRITES_START - 4,
    0,
};

const struct SpriteTemplate gHealBellMusicNoteSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUSIC_NOTES_2,
    .paletteTag = ANIM_TAG_MUSIC_NOTES_2,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimHealBellMusicNote,
};

void AnimTask_LoadMusicNotesPals(u8 taskId)
{
    int i;
    u8 paletteNums[8];
    const void *compressedPaletteData;
    void *decompressedPaletteData;
    void *palette;
    paletteNums[0] = IndexOfSpritePaletteTag(ANIM_TAG_MUSIC_NOTES_2);
    for (i = 1; i < NUM_MUSIC_NOTE_PAL_TAGS; i++)
    {
        paletteNums[i] = AllocSpritePalette(ANIM_SPRITES_START - i);
    }

    compressedPaletteData = gBattleAnimSpritePal_MusicNotes2;
    decompressedPaletteData = (IsContest()) ? gSharedMem + 0x18000 - 0x3800: gSharedMem + 0x18000;
    LZDecompressWram(compressedPaletteData, decompressedPaletteData);
    for (i = 0; i < NUM_MUSIC_NOTE_PAL_TAGS; i++)
    {
        palette = ((IsContest()) ? gSharedMem + 0x14800 : gSharedMem + 0x18000) + (i << 5);
        LoadPalette(palette, (u16)OBJ_PLTT_ID(paletteNums[i]), PLTT_SIZE_4BPP);
    }

    DestroyAnimVisualTask(taskId);
}

void AnimTask_FreeMusicNotesPals(u8 taskId)
{
    int i;
    for (i = 0; i < NUM_MUSIC_NOTE_PAL_TAGS; i++)
    {
        FreeSpritePaletteByTag(sMusicNotePaletteTagsTable[i]);
    }

    DestroyAnimVisualTask(taskId);
}

static void SetMusicNotePalette(struct Sprite *sprite, u8 animNum, u8 paletteNum)
{
    u8 tile;
    tile = (paletteNum & 1);
    tile = ((-tile | tile) >> 31) & 32;
    sprite->oam.tileNum += tile + (animNum << 2);
    sprite->oam.paletteNum = IndexOfSpritePaletteTag(sMusicNotePaletteTagsTable[paletteNum >> 1]);
}

static void AnimHealBellMusicNote(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, FALSE);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y) + gBattleAnimArgs[3];
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    SetMusicNotePalette(sprite, gBattleAnimArgs[5], gBattleAnimArgs[6]);
}
