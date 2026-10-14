#include "global.h"
#include "battle_anim.h"
#include "decompress.h"
#include "sprite.h"
#include "util.h"


static void SpriteCB_SmokescreenImpactMain(struct Sprite *);

extern const struct CompressedSpriteSheet sSmokescreenImpactSpriteSheet;
extern const struct CompressedSpritePalette sSmokescreenImpactSpritePalette;
extern const struct SpriteTemplate sSmokescreenImpactSpriteTemplate;


u8 SmokescreenImpact(s16 x, s16 y, bool8 persist)
{
    u8 mainSpriteId;
    u8 spriteId1, spriteId2, spriteId3, spriteId4;
    struct Sprite *mainSprite;

    if (GetSpriteTileStartByTag(sSmokescreenImpactSpriteSheet.tag) == 0xFFFF)
    {
        LoadCompressedSpriteSheet(&sSmokescreenImpactSpriteSheet);
        LoadCompressedSpritePalette(&sSmokescreenImpactSpritePalette);
    }

    mainSpriteId = CreateInvisibleSpriteWithCallback(SpriteCB_SmokescreenImpactMain);
    mainSprite = &gSprites[mainSpriteId];
    mainSprite->data[1] = persist;

    spriteId1 = CreateSprite(&sSmokescreenImpactSpriteTemplate, x - 16, y - 16, 2);
    gSprites[spriteId1].data[0] = mainSpriteId;
    mainSprite->data[0]++;
    AnimateSprite(&gSprites[spriteId1]);

    spriteId2 = CreateSprite(&sSmokescreenImpactSpriteTemplate, x, y - 16, 2);
    gSprites[spriteId2].data[0] = mainSpriteId;
    mainSprite->data[0]++;
    StartSpriteAnim(&gSprites[spriteId2], 1);
    AnimateSprite(&gSprites[spriteId2]);

    spriteId3 = CreateSprite(&sSmokescreenImpactSpriteTemplate, x - 16, y, 2);
    gSprites[spriteId3].data[0] = mainSpriteId;
    mainSprite->data[0]++;
    StartSpriteAnim(&gSprites[spriteId3], 2);
    AnimateSprite(&gSprites[spriteId3]);

    spriteId4 = CreateSprite(&sSmokescreenImpactSpriteTemplate, x, y, 2);
    gSprites[spriteId4].data[0] = mainSpriteId;
    mainSprite->data[0]++;
    StartSpriteAnim(&gSprites[spriteId4], 3);
    AnimateSprite(&gSprites[spriteId4]);

    return mainSpriteId;
}

static void SpriteCB_SmokescreenImpactMain(struct Sprite *sprite)
{
    if (!sprite->data[0])
    {
        FreeSpriteTilesByTag(sSmokescreenImpactSpriteSheet.tag);
        FreeSpritePaletteByTag(sSmokescreenImpactSpritePalette.tag);
        if (!sprite->data[1])
            DestroySprite(sprite);
        else
            sprite->callback = SpriteCallbackDummy;
    }
}

void SpriteCB_SmokescreenImpact(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        gSprites[sprite->data[0]].data[0]--;
        DestroySprite(sprite);
    }
}
