#ifndef GUARD_DATA_H
#define GUARD_DATA_H

#include "sprite.h"
#include "battle.h"

struct ScanlineEffectParams;

struct MonCoords
{
    // This would use a bitfield, but CreateAdditionalMonSpriteForMoveAnim
    // uses it as a u8 and casting won't match.
    u8 size; // u8 width:4, height:4;
    u8 y_offset;
};

// IV + LEVEL + SPECIES
struct TrainerMonNoItemDefaultMoves
{
    u16 iv;
    u8 level;
    u16 species;
};

// IV + LEVEL + SPECIES + MOVES
struct TrainerMonNoItemCustomMoves
{
    u16 iv;
    u8 level;
    u16 species;
    u16 moves[4];
};

// IV + LEVEL + SPECIES + ITEMS
struct TrainerMonItemDefaultMoves
{
    u16 iv;
    u8 level;
    u16 species;
    u16 heldItem;
};

// IV + LEVEL + SPECIES + ITEMS + MOVES
struct TrainerMonItemCustomMoves
{
    u16 iv;
    u8 level;
    u16 species;
    u16 heldItem;
    u16 moves[4];
};

union TrainerMonPtr
{
    const struct TrainerMonNoItemDefaultMoves *NoItemDefaultMoves;
    const struct TrainerMonNoItemCustomMoves *NoItemCustomMoves;
    const struct TrainerMonItemDefaultMoves *ItemDefaultMoves;
    const struct TrainerMonItemCustomMoves *ItemCustomMoves;
};

struct Trainer
{
    /*0x00*/ u8 partyFlags;
    /*0x01*/ u8 trainerClass;
    /*0x02*/ u8 encounterMusic_gender;
    /*0x03*/ u8 trainerPic;
    /*0x04*/ u8 trainerName[12];
    /*0x10*/ u16 items[4];
    /*0x18*/ bool8 doubleBattle;
    /*0x1C*/ u32 aiFlags;
    /*0x20*/ u8 partySize;
    /*0x24*/ union TrainerMonPtr party;
};

extern const struct OamData gOamData_81F96F0;
extern const struct OamData gOamData_81F96E8;
extern const struct SpriteFrameImage gBattlerPicTable_PlayerLeft[];
extern const struct SpriteFrameImage gBattlerPicTable_OpponentLeft[];
extern const struct SpriteFrameImage gBattlerPicTable_PlayerRight[];
extern const struct SpriteFrameImage gBattlerPicTable_OpponentRight[];
extern const union AffineAnimCmd *const gAffineAnims_BattleSpritePlayerSide[];
extern const union AffineAnimCmd *const gAffineAnims_BattleSpriteOpponentSide[];

extern const union AffineAnimCmd *const gAffineAnims_BattleSpriteContest[];
extern const union AnimCmd *const gAnims_MonPic[];
extern const struct MonCoords gMonFrontPicCoords[];
extern const struct MonCoords gMonBackPicCoords[];
extern const struct CompressedSpriteSheet gMonFrontPicTable[];
extern const struct CompressedSpriteSheet gMonBackPicTable[];
extern const struct CompressedSpritePalette gMonPaletteTable[];
extern const struct CompressedSpritePalette gMonShinyPaletteTable[];
extern const union AnimCmd *const *const gTrainerFrontAnimsPtrTable[];
extern const union AnimCmd *const *const gTrainerBackAnimsPtrTable[];
extern const struct MonCoords gTrainerBackPicCoords[];
extern const struct CompressedSpriteSheet gTrainerBackPicTable[];
extern const struct CompressedSpritePalette gTrainerBackPicPaletteTable[];
extern const u8 gEnemyMonElevation[];
extern const u8 gTrainerClassNames[][13];
extern const struct Trainer gTrainers[];
extern const u8 gSpeciesNames[][11];
extern const u8 gMoveNames[][13];
extern const u8 gAbilityNames[][13];
extern const u8 gTypeNames[][7];
extern const struct ScanlineEffectParams gUnknown_081F9674;
extern const u8 gUnknown_081F96C8[];
extern u8 *const gMonSpriteGfx_Sprite_ptr[];
//extern u8 (*const gMonSpriteGfx_Sprite_ptr[])[0x800];
extern const struct BattleMove gBattleMoves[];

// src/data/trainer_graphics/front_pic_coords.inc
extern const struct MonCoords gTrainerFrontPicCoords[];

// src/data/trainer_graphics/front_pic_table.inc
extern const struct CompressedSpriteSheet gTrainerFrontPicTable[];

// src/data/trainer_graphics/front_pic_palette_table.inc
extern const struct CompressedSpritePalette gTrainerFrontPicPaletteTable[];


#endif // GUARD_DATA_H
