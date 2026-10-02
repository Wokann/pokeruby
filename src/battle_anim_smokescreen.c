#include "global.h"
#include "data.h"
#include "decompress.h"
#include "sprite.h"
#include "util.h"


static void SpriteCB_SmokescreenImpactMain(struct Sprite *);
void SpriteCB_SmokescreenImpact(struct Sprite *);
void SpriteCB_SetInvisible(struct Sprite *);

extern const u8 gSmokescreenImpactTiles[];
extern const u8 gSmokescreenImpactPalette[];
extern const u8 gEnemyMonShadow_Gfx[];

#define TAG_SMOKESCREEN 55019
#define PALTAG_SHADOW 55039
#define GFXTAG_SHADOW 55129

const struct CompressedSpriteSheet gSmokescreenImpactSpriteSheet =
{
    .data = gSmokescreenImpactTiles,
    .size = 0x180,
    .tag = TAG_SMOKESCREEN,
};

const struct CompressedSpritePalette gSmokescreenImpactSpritePalette =
{
    .data = gSmokescreenImpactPalette,
    .tag = TAG_SMOKESCREEN,
};

const struct OamData gOamData_SmokescreenImpact =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AnimCmd gAnim_SmokescreenImpact_0[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_END
};

const union AnimCmd gAnim_SmokescreenImpact_1[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 4, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd gAnim_SmokescreenImpact_2[] =
{
    ANIMCMD_FRAME(0, 4, .vFlip = TRUE),
    ANIMCMD_FRAME(4, 4, .vFlip = TRUE),
    ANIMCMD_FRAME(8, 4, .vFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd gAnim_SmokescreenImpact_3[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE, .vFlip = TRUE),
    ANIMCMD_FRAME(4, 4, .hFlip = TRUE, .vFlip = TRUE),
    ANIMCMD_FRAME(8, 4, .hFlip = TRUE, .vFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd *const gAnims_SmokescreenImpact[] =
{
    gAnim_SmokescreenImpact_0,
    gAnim_SmokescreenImpact_1,
    gAnim_SmokescreenImpact_2,
    gAnim_SmokescreenImpact_3,
};

const struct SpriteTemplate gSmokescreenImpactSpriteTemplate =
{
    .tileTag = TAG_SMOKESCREEN,
    .paletteTag = TAG_SMOKESCREEN,
    .oam = &gOamData_SmokescreenImpact,
    .anims = gAnims_SmokescreenImpact,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SmokescreenImpact,
};

const struct CompressedSpriteSheet gSpriteSheet_EnemyShadow =
{
    .data = gEnemyMonShadow_Gfx,
    .size = 0x80,
    .tag = GFXTAG_SHADOW,
};

const struct OamData sOamData_EnemyShadow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0,
};

const struct SpriteTemplate gSpriteTemplate_EnemyShadow =
{
    .tileTag = GFXTAG_SHADOW,
    .paletteTag = PALTAG_SHADOW,
    .oam = &sOamData_EnemyShadow,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SetInvisible,
};

#define sActiveSprites data[0]
#define sPersist       data[1]

#define sMainSpriteId data[0]


u8 SmokescreenImpact(s16 x, s16 y, bool8 persist)
{
    u8 mainSpriteId;
    u8 spriteId1, spriteId2, spriteId3, spriteId4;
    struct Sprite *mainSprite;

    if (GetSpriteTileStartByTag(gSmokescreenImpactSpriteSheet.tag) == 0xFFFF)
    {
        LoadCompressedSpriteSheet(&gSmokescreenImpactSpriteSheet);
        LoadCompressedSpritePalette(&gSmokescreenImpactSpritePalette);
    }

    mainSpriteId = CreateInvisibleSpriteWithCallback(SpriteCB_SmokescreenImpactMain);
    mainSprite = &gSprites[mainSpriteId];
    mainSprite->sPersist = persist;

    spriteId1 = CreateSprite(&gSmokescreenImpactSpriteTemplate, x - 16, y - 16, 2);
    gSprites[spriteId1].sMainSpriteId = mainSpriteId;
    mainSprite->sActiveSprites++;
    AnimateSprite(&gSprites[spriteId1]);

    spriteId2 = CreateSprite(&gSmokescreenImpactSpriteTemplate, x, y - 16, 2);
    gSprites[spriteId2].sMainSpriteId = mainSpriteId;
    mainSprite->sActiveSprites++;
    StartSpriteAnim(&gSprites[spriteId2], 1);
    AnimateSprite(&gSprites[spriteId2]);

    spriteId3 = CreateSprite(&gSmokescreenImpactSpriteTemplate, x - 16, y, 2);
    gSprites[spriteId3].sMainSpriteId = mainSpriteId;
    mainSprite->sActiveSprites++;
    StartSpriteAnim(&gSprites[spriteId3], 2);
    AnimateSprite(&gSprites[spriteId3]);

    spriteId4 = CreateSprite(&gSmokescreenImpactSpriteTemplate, x, y, 2);
    gSprites[spriteId4].sMainSpriteId = mainSpriteId;
    mainSprite->sActiveSprites++;
    StartSpriteAnim(&gSprites[spriteId4], 3);
    AnimateSprite(&gSprites[spriteId4]);

    return mainSpriteId;
}

static void SpriteCB_SmokescreenImpactMain(struct Sprite *sprite)
{
    if (sprite->sActiveSprites == 0)
    {
        FreeSpriteTilesByTag(gSmokescreenImpactSpriteSheet.tag);
        FreeSpritePaletteByTag(gSmokescreenImpactSpritePalette.tag);
        if (!sprite->sPersist)
            DestroySprite(sprite);
        else
            sprite->callback = SpriteCallbackDummy;
    }
}

void SpriteCB_SmokescreenImpact(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        gSprites[sprite->sMainSpriteId].sActiveSprites--;
        DestroySprite(sprite);
    }
}

#undef sMainSpriteId
#undef sPersist
#undef sActiveSprites
