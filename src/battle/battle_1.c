#include "global.h"
#include "data2.h"
#include "battle_setup.h"
#include "battle.h"
#include "graphics.h"
#include "gba/types.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/opponents.h"
#include "constants/trainers.h"

const struct MonCoords gTrainerBackPicCoords[] = {
    [TRAINER_BACK_PIC_BRENDAN] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_MAY] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_WALLY] = {.size = 8, .y_offset = 4},
};

extern const u8 gTrainerBackPic_RubySapphireBrendan[];
extern const u8 gTrainerBackPic_RubySapphireMay[];
extern const u8 gTrainerBackPic_Wally[];

const struct CompressedSpriteSheet gTrainerBackPicTable[] = {
    [TRAINER_BACK_PIC_BRENDAN] = {
        .data = gTrainerBackPic_RubySapphireBrendan,
        .size = TRAINER_BACK_PIC_SIZE,
        .tag = TRAINER_BACK_PIC_BRENDAN,
    },
    [TRAINER_BACK_PIC_MAY] = {
        .data = gTrainerBackPic_RubySapphireMay,
        .size = TRAINER_BACK_PIC_SIZE,
        .tag = TRAINER_BACK_PIC_MAY,
    },
    [TRAINER_BACK_PIC_WALLY] = {
        .data = gTrainerBackPic_Wally,
        .size = TRAINER_BACK_PIC_SIZE,
        .tag = TRAINER_BACK_PIC_WALLY,
    },
};

extern const u8 gTrainerPalette_RubySapphireBrendan[];
extern const u8 gTrainerPalette_RubySapphireMay[];
extern const u8 gTrainerPalette_Wally[];

const struct CompressedSpritePalette gTrainerBackPicPaletteTable[] = {
    [TRAINER_BACK_PIC_BRENDAN] = {
        .data = gTrainerPalette_RubySapphireBrendan,
        .tag = TRAINER_BACK_PIC_BRENDAN,
    },
    [TRAINER_BACK_PIC_MAY] = {
        .data = gTrainerPalette_RubySapphireMay,
        .tag = TRAINER_BACK_PIC_MAY,
    },
    [TRAINER_BACK_PIC_WALLY] = {
        .data = gTrainerPalette_Wally,
        .tag = TRAINER_BACK_PIC_WALLY,
    },
};

// This determines how much higher above the usual position the enemy Pokémon
// is during battle. Species that float or fly have nonzero values.
const u8 gEnemyMonElevation[NUM_SPECIES] = {
    [SPECIES_BUTTERFREE] = 8,
    [SPECIES_BEEDRILL] = 8,
    [SPECIES_PIDGEY] = 16,
    [SPECIES_PIDGEOT] = 4,
    [SPECIES_FEAROW] = 4,
    [SPECIES_ZUBAT] = 8,
    [SPECIES_GOLBAT] = 8,
    [SPECIES_VENOMOTH] = 8,
    [SPECIES_GEODUDE] = 16,
    [SPECIES_MAGNEMITE] = 16,
    [SPECIES_MAGNETON] = 8,
    [SPECIES_GASTLY] = 4,
    [SPECIES_HAUNTER] = 4,
    [SPECIES_VOLTORB] = 10,
    [SPECIES_ELECTRODE] = 12,
    [SPECIES_KOFFING] = 8,
    [SPECIES_WEEZING] = 6,
    [SPECIES_AERODACTYL] = 6,
    [SPECIES_ARTICUNO] = 6,
    [SPECIES_ZAPDOS] = 8,
    [SPECIES_MOLTRES] = 5,
    [SPECIES_DRAGONITE] = 3,
    [SPECIES_MEW] = 8,
    [SPECIES_LEDIAN] = 8,
    [SPECIES_CROBAT] = 6,
    [SPECIES_HOPPIP] = 11,
    [SPECIES_SKIPLOOM] = 12,
    [SPECIES_JUMPLUFF] = 6,
    [SPECIES_YANMA] = 8,
    [SPECIES_MISDREAVUS] = 8,
    [SPECIES_UNOWN] = 5,
    [SPECIES_GLIGAR] = 6,
    [SPECIES_LUGIA] = 3,
    [SPECIES_HO_OH] = 6,
    [SPECIES_CELEBI] = 15,
    [SPECIES_BEAUTIFLY] = 8,
    [SPECIES_DUSTOX] = 10,
    [SPECIES_NINJASK] = 10,
    [SPECIES_SHEDINJA] = 8,
    [SPECIES_WINGULL] = 16,
    [SPECIES_PELIPPER] = 8,
    [SPECIES_MASQUERAIN] = 10,
    [SPECIES_BALTOY] = 4,
    [SPECIES_CLAYDOL] = 4,
    [SPECIES_FLYGON] = 3,
    [SPECIES_GLALIE] = 12,
    [SPECIES_LUNATONE] = 13,
    [SPECIES_SOLROCK] = 4,
    [SPECIES_SWABLU] = 12,
    [SPECIES_ALTARIA] = 8,
    [SPECIES_DUSKULL] = 4,
    [SPECIES_SHUPPET] = 12,
    [SPECIES_BANETTE] = 8,
    [SPECIES_CASTFORM] = 16,
    [SPECIES_BELDUM] = 8,
    [SPECIES_RAYQUAZA] = 6,
    [SPECIES_LATIAS] = 6,
    [SPECIES_LATIOS] = 6,
    [SPECIES_JIRACHI] = 12,
    [SPECIES_DEOXYS] = 4,
    [SPECIES_CHIMECHO] = 12,
};

#include "../data/trainer_parties.h"

#if ENGLISH
#include "../data/text/trainer_class_names_en.h"
#include "../data/trainers_en.h"
#include "../data/text/species_names_en.h"
#include "../data/text/move_names_en.h"
#elif GERMAN
#include "../data/text/trainer_class_names_de.h"
#include "../data/trainers_de.h"
#include "../data/text/species_names_de.h"
#include "../data/text/move_names_de.h"
#endif

const u32 gUnusedBattleData = 0x101;
