#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern u8 gBattlerSpriteIds[];

static void AnimVibrateBattlerBack(struct Sprite *sprite);

// Unused

const struct SpriteTemplate gVibrateBattlerBackSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimVibrateBattlerBack,
};

static void AnimVibrateBattlerBack_Step(struct Sprite *sprite)
{
    s16 temp;
    gSprites[sprite->data[2]].x2 += sprite->data[1];
    temp = sprite->data[1];
    sprite->data[1] = -temp;
    if (sprite->data[0] == 0)
    {
        gSprites[sprite->data[2]].x2 = 0;
        DestroySpriteAndMatrix(sprite);
    }

    sprite->data[0]--;
}

static void AnimVibrateBattlerBack(struct Sprite *sprite)
{
    u8 spriteId;
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    spriteId = gBattlerSpriteIds[gBattleAnimTarget];
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        sprite->x -= gBattleAnimArgs[0];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
    }

    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = spriteId;
    sprite->callback = AnimVibrateBattlerBack_Step;
    sprite->invisible = TRUE;
}
