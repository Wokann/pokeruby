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
