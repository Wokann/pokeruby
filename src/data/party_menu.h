static const u16 sTMHMMoves[] =
{
    MOVE_FOCUS_PUNCH,
    MOVE_DRAGON_CLAW,
    MOVE_WATER_PULSE,
    MOVE_CALM_MIND,
    MOVE_ROAR,
    MOVE_TOXIC,
    MOVE_HAIL,
    MOVE_BULK_UP,
    MOVE_BULLET_SEED,
    MOVE_HIDDEN_POWER,
    MOVE_SUNNY_DAY,
    MOVE_TAUNT,
    MOVE_ICE_BEAM,
    MOVE_BLIZZARD,
    MOVE_HYPER_BEAM,
    MOVE_LIGHT_SCREEN,
    MOVE_PROTECT,
    MOVE_RAIN_DANCE,
    MOVE_GIGA_DRAIN,
    MOVE_SAFEGUARD,
    MOVE_FRUSTRATION,
    MOVE_SOLAR_BEAM,
    MOVE_IRON_TAIL,
    MOVE_THUNDERBOLT,
    MOVE_THUNDER,
    MOVE_EARTHQUAKE,
    MOVE_RETURN,
    MOVE_DIG,
    MOVE_PSYCHIC,
    MOVE_SHADOW_BALL,
    MOVE_BRICK_BREAK,
    MOVE_DOUBLE_TEAM,
    MOVE_REFLECT,
    MOVE_SHOCK_WAVE,
    MOVE_FLAMETHROWER,
    MOVE_SLUDGE_BOMB,
    MOVE_SANDSTORM,
    MOVE_FIRE_BLAST,
    MOVE_ROCK_TOMB,
    MOVE_AERIAL_ACE,
    MOVE_TORMENT,
    MOVE_FACADE,
    MOVE_SECRET_POWER,
    MOVE_REST,
    MOVE_ATTRACT,
    MOVE_THIEF,
    MOVE_STEEL_WING,
    MOVE_SKILL_SWAP,
    MOVE_SNATCH,
    MOVE_OVERHEAT,
    MOVE_CUT,
    MOVE_FLY,
    MOVE_SURF,
    MOVE_STRENGTH,
    MOVE_FLASH,
    MOVE_ROCK_SMASH,
    MOVE_WATERFALL,
    MOVE_DIVE,
};

//FIXME

u8 *const gTileBufferPtr = gTileBuffer;

static const u8 sHeldItemGfx[] = INCBIN_U8("graphics/party_menu/hold_icons.4bpp");
static const u16 sHeldItemPalette[] = INCBIN_U16("graphics/party_menu/hold_icons.gbapal");

static const struct SpriteSheet sSpriteSheet_HeldItem =
{
    .data = sHeldItemGfx, .size = sizeof(sHeldItemGfx), .tag = TAG_HELD_ITEM
};

static const struct SpritePalette sSpritePalette_HeldItem =
{
    .data = sHeldItemPalette, .tag = TAG_HELD_ITEM
};

static const struct OamData sOamData_HeldItem =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_HeldItem[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_HeldMail[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_HeldItem[] = {
    sSpriteAnim_HeldItem,
    sSpriteAnim_HeldMail,
};

static const struct SpriteTemplate sSpriteTemplate_HeldItem = {
    .tileTag = TAG_HELD_ITEM,
    .paletteTag = TAG_HELD_ITEM,
    .oam = &sOamData_HeldItem,
    .anims = sSpriteAnimTable_HeldItem,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_UpdateHeldItemIconPosition
};

// Texts that can be displayed in the bottom of the party menu.
static const u8 *const PartyMenuPromptTexts[] = {
    [PARTY_MSG_CHOOSE_MON] = gText_ChoosePokemon,
    [PARTY_MSG_MOVE_TO_WHERE] = gText_MoveToWhere,
    [2] = gText_TeachWhichPokemon2,
    [PARTY_MSG_USE_ON_WHICH_MON] = gText_UseOnWhichPokemon,
    [PARTY_MSG_GIVE_TO_WHICH_MON] = gText_GiveToWhichPokemon,
    [PARTY_MSG_DO_WHAT_WITH_MON] = gText_DoWhatWithPokemon,
    [PARTY_MSG_NOTHING_TO_CUT] = gText_NothingToCut,
    [PARTY_MSG_CANT_SURF_HERE] = gText_CantSurfHere,
    [PARTY_MSG_ALREADY_SURFING] = gText_AlreadySurfing,
    [PARTY_MSG_CANT_USE_HERE] = gText_CantUseHere,
    [PARTY_MSG_RESTORE_WHICH_MOVE] = gText_RestoreWhichMove,
    [PARTY_MSG_BOOST_PP_WHICH_MOVE] = gText_BoostPP,
    [12] = gOtherText_CancelWithTerminator,
    [PARTY_MSG_DO_WHAT_WITH_ITEM] = gText_DoWhatWithItem,
    [PARTY_MSG_NO_MON_FOR_BATTLE] = gText_NoPokemonForBattle,
    [PARTY_MSG_CHOOSE_MON_2] = gText_ChoosePokemon2,
    [PARTY_MSG_NOT_ENOUGH_HP] = gText_NotEnoughHp,
    [17] = OtherText_ThreePokeNeeded,
    [PARTY_MSG_MONS_CANT_BE_SAME] = gText_PokemonCantBeSame,
    [PARTY_MSG_NO_SAME_HOLD_ITEMS] = gText_NoIdenticalHoldItems,
    [PARTY_MSG_TEACH_WHICH_MON] = gText_TeachWhichPokemon,
};

static const struct Coords8 gUnknown_08376678[8][PARTY_SIZE] = {
    {{16, 40}, {104, 18}, {104, 42}, {104, 66}, {104, 90}, {104, 114}}, // PARTY_LAYOUT_SINGLE
    {{16, 24}, { 16, 80}, {104, 18}, {104, 50}, {104, 82}, {104, 114}}, // PARTY_LAYOUT_DOUBLE
    {{16, 24}, { 16, 80}, {104, 26}, {104, 50}, {104, 82}, {104, 106}}, // PARTY_LAYOUT_MULTI
    {{16, 24}, {104, 26}, {104, 50}, { 16, 80}, {104, 82}, {104, 106}}, // PARTY_LAYOUT_MULTI_SHOWCASE
    {{ 5,  4}, { 16,  1}, { 16,  4}, { 16,  7}, { 16, 10}, { 16,  13}},
    {{ 5,  2}, {  5,  9}, { 16,  1}, { 16,  5}, { 16,  9}, { 16,  13}},
    {{ 5,  2}, {  5,  9}, { 16,  2}, { 16,  5}, { 16,  9}, { 16,  12}},
    {{ 5,  2}, { 16,  2}, { 16,  5}, {  5,  9}, { 16,  9}, { 16,  12}},
};

static const struct Coords8 gUnknown_08376738[12][PARTY_SIZE] = {
    {{6,  5}, {17,  2}, {17,  5}, {17,  8}, {17, 11}, {17, 14}},
    {{6,  3}, { 6, 10}, {17,  2}, {17,  6}, {17, 10}, {17, 14}},
    {{6,  3}, { 6, 10}, {17,  3}, {17,  6}, {17, 10}, {17, 13}},
    {{6,  3}, {17,  3}, {17,  6}, { 6, 10}, {17, 10}, {17, 13}},
    {{3,  7}, {22,  2}, {22,  5}, {22,  8}, {22, 11}, {22, 14}},
    {{3,  5}, { 3, 12}, {22,  2}, {22,  6}, {22, 10}, {22, 14}},
    {{3,  5}, { 3, 12}, {22,  3}, {22,  6}, {22, 10}, {22, 13}},
    {{3,  5}, {22,  3}, {22,  6}, { 3, 12}, {22, 10}, {22, 13}},
    {{7,  7}, {26,  2}, {26,  5}, {26,  8}, {26, 11}, {26, 14}},
    {{7,  5}, { 7, 12}, {26,  2}, {26,  6}, {26, 10}, {26, 14}},
    {{7,  5}, { 7, 12}, {26,  3}, {26,  6}, {26, 10}, {26, 13}},
    {{7,  5}, {26,  3}, {26,  6}, { 7, 12}, {26, 10}, {26, 13}},
};

static u16 *const gUnknown_08376858[PARTY_LAYOUT_COUNT][PARTY_SIZE] = {
    {(u16*)(BG_VRAM + 0xF1C8), (u16*)(BG_VRAM + 0xF0AE), (u16*)(BG_VRAM + 0xF16E), (u16*)(BG_VRAM + 0xF22E), (u16*)(BG_VRAM + 0xF2EE), (u16*)(BG_VRAM + 0xF3AE)}, // PARTY_LAYOUT_SINGLE
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF0AE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF3AE)}, // PARTY_LAYOUT_DOUBLE
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF0EE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF36E)}, // PARTY_LAYOUT_MULTI
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF0EE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF36E)}, // PARTY_LAYOUT_MULTI_SHOWCASE
};

static const struct Coords8 gUnknown_083768B8[3][PARTY_SIZE + 2] = {
    {{8,  44}, {92,  22}, {92,  46}, {92,  70}, {92,  94}, {92, 118}, {196, 136}, {196, 152}}, // PARTY_LAYOUT_SINGLE
    {{8,  28}, { 8,  84}, {92,  22}, {92,  54}, {92,  86}, {92, 118}, {196, 136}, {196, 152}}, // PARTY_LAYOUT_DOUBLE
    {{8,  28}, { 8,  84}, {92,  30}, {92,  54}, {92,  86}, {92, 110}, {196, 136}, {196, 152}}, // PARTY_LAYOUT_MULTI
};

static u16 *const gUnknown_08376918[2][PARTY_SIZE] = {
    {(u16*)(BG_VRAM + 0xF1C6), (u16*)(BG_VRAM + 0xF06C), (u16*)(BG_VRAM + 0xF12C), (u16*)(BG_VRAM + 0xF1EC), (u16*)(BG_VRAM + 0xF2AC), (u16*)(BG_VRAM + 0xF36C)},
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF0AE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF3AE)},
};

static const struct PartyMenuWindowCoords gUnknown_08376948[2][PARTY_SIZE] = {
    {{2, 4, 10, 9}, {16, 1, 29,  3}, {16, 4, 29, 6}, {16, 7, 29, 9}, {16, 10, 29, 12}, {16, 13, 29, 15}},
    {{2, 2, 10, 7}, { 2, 9, 10, 14}, {16, 1, 29, 3}, {16, 5, 29, 7}, {16,  9, 29, 11}, {16, 13, 29, 15}},
};

static const struct PartyMenuWindowCoords gUnknown_08376978[2][PARTY_SIZE] = {
    {{2, 7, 10,  9}, {21, 1, 29,  3}, {21, 4, 29,  6}, {21, 7, 29, 9}, {21, 10, 29, 12}, {21, 13, 29, 15}},
    {{2, 2, 10,  7}, { 2, 9, 10, 14}, {16, 1, 29,  3}, {16, 5, 29, 7}, {16,  9, 29, 11}, {16, 13, 29, 15}},
};

// This is actually a 2x6x2 array, but the code reads it as a flat array.
static const u8 gUnknown_083769A8[] = {
    0, 3,   11, 1,   11, 4,    11, 7,    11, 10,    11, 13,
    0, 1,    0, 8,   11, 1,    11, 5,    11,  9,    11, 13, // Double battle
};

// This is actually a 2x6x2 array, but the code reads it as a flat array.
//FIXME: sub_806B908() accesses this data via gUnknown_083769A8 (directly above this). This means these
// two arrays might be a struct, rather than separate arrays.
static const u8 gUnknown_083769C0[] = {
    0, 1,    0, 8,   11, 2,    11, 5,    11, 9,    11, 12,
    0, 1,    0, 8,   11, 2,    11, 5,    11, 9,    11, 12, // Double battle
};

static const u8 gUnknown_083769D8[] = {
    0x24,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x27,
    0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x37,
    0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x37,
    0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x37,
    0x44,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x47,
    0x44,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x47,
    0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x57,
};

static const u8 gUnknown_08376A25[] = {
    0x50,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x53,
    0x60,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x63,
    0x70,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x73,
};

static const u8 gUnknown_08376A5E[] = {
    0x20,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x23,
    0x30,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x33,
    0x40,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x43,
};

static const u8 gUnusedData_08376A97[] = {
    0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,0x05,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x0F,0x06,0x05,0x01,0x07,
    0x08,0x09,0x0F,0x16,0x15,0x11,0x17,0x18,0x19,0x0F,0x09,
    0x0A,0x0B,0x05,0x0C,0x0F,0x0F,0x19,0x1A,0x1B,0x15,0x1C,
    0x0F,0x0F,0x0D,0x0B,0x05,0x0C,0x0F,0x0F,0x0F,0x1D,0x1B,
    0x15,0x1C,0x0F,0x0F,0x0F,0x06,0x05,0x0B,0x05,0x0C,0x0F,
    0x0F,0x16,0x15,0x1B,0x15,0x1C,0x0F,0x0F,0x0F,0x0F,0x20,
    0x0C,0x09,0x0F,0x0F,0x0F,0x1F,0x30,0x1C,0x19,0x0F,0x0F,
    0x0F,0x25,0x0F,0x22,0x24,0x0F,0x0F,0x0F,0x35,0x41,0x32,
    0x34,0x0F,0x0F,0x0F,0x26,0x0F,0x23,0x0E,0x0F,0x0F,0x0F,
    0x36,0x43,0x33,0x1E,0x0F,0x0F,0x27,0x28,0x29,0x03,0x2A,
    0x0F,0x0F,0x37,0x38,0x39,0x13,0x3A,0x0F,0x0F,0x27,0x28,
    0x29,0x03,0x04,0x05,0x0F,0x37,0x38,0x39,0x13,0x14,0x15,
    0x0F,0x2B,0x2C,0x02,0x28,0x29,0x2D,0x21,0x3B,0x3C,0x12,
    0x38,0x39,0x3D,0x31,0x2B,0x2C,0x02,0x28,0x29,0x08,0x09,
    0x3B,0x3C,0x12,0x38,0x39,0x18,0x19,0x2E,0x2F,0x2B,0x2C,
    0x02,0x27,0x2D,0x3E,0x3F,0x3B,0x3C,0x12,0x45,0x3D,
};

static const TaskFunc gUnknown_08376B54[] = {
    ItemUseMoveMenu_HandleMoveSelection,
    ItemUseMoveMenu_HandleCancel,
};

#define PartyMonOAMSettings(x, y, palette, shape, size, priority, tileOffset) \
    ((shape)   << 14) | (y),                                                  \
    ((size)    << 14) | (x),                                                  \
    ((palette) << 12) | ((priority) << 10) | (tileOffset)

static const u16 PartyMonOAMSettings_LeftColumn[] = {
    PartyMonOAMSettings(24,  0, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1,  0),
    PartyMonOAMSettings(56,  0, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1,  4),
    PartyMonOAMSettings(24,  8, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1,  8),
    PartyMonOAMSettings(56,  8, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 12),
    PartyMonOAMSettings(32, 16, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 16),
    PartyMonOAMSettings(37, 32, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 24),
    PartyMonOAMSettings(69, 32, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 28),
    0xFFFF,
};

static const u16 PartyMonOAMSettings_RightColumn[] = {
    PartyMonOAMSettings( 24,  0, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1,  0),
    PartyMonOAMSettings( 56,  0, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1,  4),
    PartyMonOAMSettings( 24,  8, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1,  8),
    PartyMonOAMSettings( 56,  8, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 12),
    PartyMonOAMSettings( 32, 16, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 16),
    PartyMonOAMSettings(101, 16, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 24),
    PartyMonOAMSettings(133, 16, 15, ST_OAM_H_RECTANGLE, SPRITE_SIZE(32x8), 1, 28),
    0xFFFF,
};

// Controls where and how the mons' text appears in the party menu screen (nickname, HP, and level).
static struct PartyMonTextSettingsStruct const PartyMonTextSettings[PARTY_LAYOUT_COUNT][PARTY_SIZE] =
{
    [PARTY_LAYOUT_SINGLE] = {
        { 1,  4, PartyMonOAMSettings_LeftColumn},
        {12,  1, PartyMonOAMSettings_RightColumn},
        {12,  4, PartyMonOAMSettings_RightColumn},
        {12,  7, PartyMonOAMSettings_RightColumn},
        {12, 10, PartyMonOAMSettings_RightColumn},
        {12, 13, PartyMonOAMSettings_RightColumn},
    },
    [PARTY_LAYOUT_DOUBLE] = {
        { 1,  2, PartyMonOAMSettings_LeftColumn},
        { 1,  9, PartyMonOAMSettings_LeftColumn},
        {12,  1, PartyMonOAMSettings_RightColumn},
        {12,  5, PartyMonOAMSettings_RightColumn},
        {12,  9, PartyMonOAMSettings_RightColumn},
        {12, 13, PartyMonOAMSettings_RightColumn},
    },
    [PARTY_LAYOUT_MULTI] = {
        { 1,  2, PartyMonOAMSettings_LeftColumn},
        { 1,  9, PartyMonOAMSettings_LeftColumn},
        {12,  2, PartyMonOAMSettings_RightColumn},
        {12,  5, PartyMonOAMSettings_RightColumn},
        {12,  9, PartyMonOAMSettings_RightColumn},
        {12, 12, PartyMonOAMSettings_RightColumn},
    },
    [PARTY_LAYOUT_MULTI_SHOWCASE] = {
        { 1,  2, PartyMonOAMSettings_LeftColumn},
        {12,  2, PartyMonOAMSettings_RightColumn},
        {12,  5, PartyMonOAMSettings_RightColumn},
        { 1,  9, PartyMonOAMSettings_LeftColumn},
        {12,  9, PartyMonOAMSettings_RightColumn},
        {12, 12, PartyMonOAMSettings_RightColumn},
    },
};

static const struct PartyMenuHandlersStruct PartyMenuHandlers[] =
{
    {HandleDefaultPartyMenu,         SetupDefaultPartyMenu,            0}, // PARTY_MENU_TYPE_FIELD
    {HandleBattlePartyMenu,          SetUpBattlePartyMenu,             0}, // PARTY_MENU_TYPE_IN_BATTLE
    {HandleSelectPartyMenu,          SetupContestPartyMenu,            0}, // PARTY_MENU_TYPE_CONTEST
    {HandleSelectPartyMenu,          SetupDefaultPartyMenu,            0}, // PARTY_MENU_TYPE_CHOOSE_MON
    {HandleBattleTowerPartyMenu,     SetupBattleTowerPartyMenu,        0}, // PARTY_MENU_TYPE_BATTLE_TOWER
    {HandleLinkMultiBattlePartyMenu, SetupLinkMultiBattlePartyMenu, 0xFF}, // PARTY_MENU_TYPE_MULTI_SHOWCASE
    {HandleDaycarePartyMenu,         SetupDefaultPartyMenu,         0x0F}, // PARTY_MENU_TYPE_DAYCARE
    {HandleMoveTutorPartyMenu,       SetupMoveTutorPartyMenu,          0}, // PARTY_MENU_TYPE_MOVE_RELEARNER
};
