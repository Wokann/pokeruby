const struct MonCoords gTrainerBackPicCoords[] = {
    {8, 4},
    {8, 4},
    {8, 4},
};

extern const u8 gTrainerBackPic_Brendan[];
extern const u8 gTrainerBackPic_May[];
extern const u8 gTrainerBackPic_Wally[];

const struct CompressedSpriteSheet gTrainerBackPicTable[] = {
    {gTrainerBackPic_Brendan, 0x2000, 0},
    {gTrainerBackPic_May, 0x2000, 1},
    {gTrainerBackPic_Wally, 0x2000, 2}
};

extern const u8 gTrainerPalette_Brendan[];
extern const u8 gTrainerPalette_May[];
extern const u8 gTrainerPalette_Wally[];

const struct CompressedSpritePalette gTrainerBackPicPaletteTable[] = {
    {gTrainerPalette_Brendan, 0},
    {gTrainerPalette_May, 1},
    {gTrainerPalette_Wally, 2}
};
