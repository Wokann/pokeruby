#ifndef GUARD_DATA2_H
#define GUARD_DATA2_H

#include "constants/abilities.h"
#include "constants/global.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"
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

extern const struct OamData gOamData_BattleSpritePlayerSide;
extern const struct OamData gOamData_BattleSpriteOpponentSide;
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
extern const u8 gEnemyMonElevation[NUM_SPECIES];
extern const u8 gTrainerClassNames[][13];
extern const struct Trainer gTrainers[];
extern const u8 gSpeciesNames[][POKEMON_NAME_LENGTH + 1];
extern const u8 gMoveNames[MOVES_COUNT][MOVE_NAME_LENGTH + 1];
extern const u8 gAbilityNames[][ABILITY_NAME_LENGTH + 1];
extern const u8 *const gAbilityDescriptionPointers[ABILITIES_COUNT];
extern const u8 gTypeNames[NUMBER_OF_MON_TYPES][TYPE_NAME_LENGTH + 1];
extern const struct ScanlineEffectParams gIntroScanlineParams16Bit;
extern const u8 gText_ShedinjaJpnName[];
extern const struct CompressedSpriteSheet gSmokescreenImpactSpriteSheet;
extern const struct CompressedSpritePalette gSmokescreenImpactSpritePalette;
extern const struct SpriteTemplate gSmokescreenImpactSpriteTemplate;
extern u8 *const gMonSpriteGfxBufferPtrs[];
//extern u8 (*const gMonSpriteGfxBufferPtrs[])[0x800];
extern const struct BattleMove gBattleMoves[];

// data/graphics/trainers/front_pic_coordinates.inc
extern const struct MonCoords gTrainerFrontPicCoords[];

// data/graphics/trainers/front_pic_table.inc
extern const struct CompressedSpriteSheet gTrainerFrontPicTable[];

// data/graphics/trainers/front_pic_palette_table.inc
extern const struct CompressedSpritePalette gTrainerFrontPicPaletteTable[];


#endif // GUARD_DATA2_H
