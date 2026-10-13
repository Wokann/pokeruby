const struct MonCoords gTrainerBackPicCoords[] = {
    {8, 4},
    {8, 4},
    {8, 4},
};

extern const u8 gTrainerBackPic_RubySapphireBrendan[];
extern const u8 gTrainerBackPic_RubySapphireMay[];
extern const u8 gTrainerBackPic_Wally[];

const struct CompressedSpriteSheet gTrainerBackPicTable[] = {
    {gTrainerBackPic_RubySapphireBrendan, 0x2000, 0},
    {gTrainerBackPic_RubySapphireMay, 0x2000, 1},
    {gTrainerBackPic_Wally, 0x2000, 2}
};

extern const u8 gTrainerPalette_RubySapphireBrendan[];
extern const u8 gTrainerPalette_RubySapphireMay[];
extern const u8 gTrainerPalette_Wally[];

const struct CompressedSpritePalette gTrainerBackPicPaletteTable[] = {
    {gTrainerPalette_RubySapphireBrendan, 0},
    {gTrainerPalette_RubySapphireMay, 1},
    {gTrainerPalette_Wally, 2}
};
