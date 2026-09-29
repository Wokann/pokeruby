#include "global.h"
#include "data2.h"
#include "decompress.h"
#include "sprite.h"
#include "util.h"


static void SpriteCB_SmokescreenImpactMain(struct Sprite *);

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
