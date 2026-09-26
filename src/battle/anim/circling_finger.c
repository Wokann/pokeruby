#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimCirclingFinger(struct Sprite *sprite);

// Unused

const struct SpriteTemplate gCirclingFingerSpriteTemplate =
{
    .tileTag = ANIM_TAG_FINGER,
    .paletteTag = ANIM_TAG_FINGER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimCirclingFinger,
};

#define sAmplitudeX  data[1]
#define sCircleSpeed data[2]
#define sMoveSteps   data[3]
#define sAmplitudeY  data[4]

static void AnimCirclingFinger(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
    sprite->sAmplitudeX = gBattleAnimArgs[2];
    sprite->sCircleSpeed = gBattleAnimArgs[4];
    sprite->sMoveSteps = gBattleAnimArgs[5];
    sprite->sAmplitudeY = gBattleAnimArgs[3];
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = TranslateSpriteInEllipse;
    sprite->callback(sprite);
}

#undef sAmplitudeX
#undef sCircleSpeed
#undef sMoveSteps
#undef sAmplitudeY
