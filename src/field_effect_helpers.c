#include "global.h"
#include "event_object_movement.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "sound.h"
#include "sprite.h"
#include "constants/field_effects.h"
#include "constants/songs.h"

static void UpdateObjectReflectionSprite(struct Sprite *);
static void LoadObjectReflectionPalette(struct ObjectEvent *objectEvent, struct Sprite *sprite);
static void LoadObjectHighBridgeReflectionPalette(struct ObjectEvent *, u8);
static void LoadObjectRegularReflectionPalette(struct ObjectEvent *, u8);
static void FadeFootprintsTireTracks_Step0(struct Sprite *);
static void FadeFootprintsTireTracks_Step1(struct Sprite *);
static void UpdateAshFieldEffect_Wait(struct Sprite *);
static void UpdateAshFieldEffect_Show(struct Sprite *);
static void UpdateAshFieldEffect_End(struct Sprite *);
static void UpdateGrassFieldEffectSubpriority(struct Sprite *, u8, u8);
static void UpdateFeetInFlowingWaterFieldEffect(struct Sprite *);
static void SynchronizeSurfAnim(struct ObjectEvent *, struct Sprite *);
static void SynchronizeSurfPosition(struct ObjectEvent *, struct Sprite *);
static void UpdateBobbingEffect(struct ObjectEvent *, struct Sprite *, struct Sprite *);
static void SpriteCB_UnderwaterSurfBlob(struct Sprite *);
static u32 ShowDisguiseFieldEffect(u8, u8, u8);

#define sWaitFldEff data[0]
#define sJumpElevation data[0]
#define sJumpFldEff data[1]

#define sReflectionObjEventId      data[0]
#define sReflectionObjEventLocalId data[1]
#define sReflectionVerticalOffset  data[2]
#define sIsStillReflection         data[7]

void SetUpReflection(struct ObjectEvent *objectEvent, struct Sprite *sprite, bool8 stillReflection)
{
    struct Sprite *reflectionSprite;

    reflectionSprite = &gSprites[CreateCopySpriteAt(sprite, sprite->x, sprite->y, 0x98)];
    reflectionSprite->callback = UpdateObjectReflectionSprite;
    reflectionSprite->oam.priority = 3;
    reflectionSprite->oam.paletteNum = gReflectionEffectPaletteMap[reflectionSprite->oam.paletteNum];
    reflectionSprite->usingSheet = TRUE;
    reflectionSprite->anims = gDummySpriteAnimTable;
    StartSpriteAnim(reflectionSprite, 0);
    reflectionSprite->affineAnims = gDummySpriteAffineAnimTable;
    reflectionSprite->affineAnimBeginning = TRUE;
    reflectionSprite->subspriteMode = SUBSPRITES_OFF;
    reflectionSprite->sReflectionObjEventId = sprite->data[0];
    reflectionSprite->sReflectionObjEventLocalId = objectEvent->localId;
    reflectionSprite->sIsStillReflection = stillReflection;
    LoadObjectReflectionPalette(objectEvent, reflectionSprite);

    if (!stillReflection)
        reflectionSprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
}

static s16 GetReflectionVerticalOffset(struct ObjectEvent *objectEvent)
{
    return GetObjectEventGraphicsInfo(objectEvent->graphicsId)->height - 2;
}

static void LoadObjectReflectionPalette(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    u8 bridgeType;
    u16 bridgeReflectionVerticalOffsets[] = { 12, 28, 44 };
    sprite->sReflectionVerticalOffset = 0;
    if (!GetObjectEventGraphicsInfo(objectEvent->graphicsId)->disableReflectionPaletteLoad && ((bridgeType = MetatileBehavior_GetBridgeType(objectEvent->previousMetatileBehavior)) || (bridgeType = MetatileBehavior_GetBridgeType(objectEvent->currentMetatileBehavior))))
    {
        sprite->sReflectionVerticalOffset = bridgeReflectionVerticalOffsets[bridgeType - 1];
        LoadObjectHighBridgeReflectionPalette(objectEvent, sprite->oam.paletteNum);
    }
    else
    {
        LoadObjectRegularReflectionPalette(objectEvent, sprite->oam.paletteNum);
    }
}

static void LoadObjectRegularReflectionPalette(struct ObjectEvent *objectEvent, u8 paletteIndex)
{
    const struct ObjectEventGraphicsInfo *graphicsInfo;

    graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
    if (graphicsInfo->bridgeReflectionPaletteTag != 0x11ff)
    {
        if (graphicsInfo->paletteSlot == 0)
        {
            LoadPlayerObjectReflectionPalette(graphicsInfo->paletteTag, paletteIndex);
        }
        else if (graphicsInfo->paletteSlot == 10)
        {
            LoadSpecialObjectReflectionPalette(graphicsInfo->paletteTag, paletteIndex);
        }
        else
        {
            PatchObjectPalette(GetObjectPaletteTag(paletteIndex), paletteIndex);
        }
        UpdateSpritePaletteWithWeather(paletteIndex);
    }
}

// When walking on a bridge high above water (Route 120), the reflection is a solid dark blue color.
// This is so the sprite blends in with the dark water metatile underneath the bridge.
static void LoadObjectHighBridgeReflectionPalette(struct ObjectEvent *objectEvent, u8 paletteNum)
{
    const struct ObjectEventGraphicsInfo *graphicsInfo;

    graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
    if (graphicsInfo->bridgeReflectionPaletteTag != 0x11ff)
    {
        PatchObjectPalette(graphicsInfo->bridgeReflectionPaletteTag, paletteNum);
        UpdateSpritePaletteWithWeather(paletteNum);
    }
}

static void UpdateObjectReflectionSprite(struct Sprite *reflectionSprite)
{
    struct ObjectEvent *objectEvent;
    struct Sprite *mainSprite;

    objectEvent = &gObjectEvents[reflectionSprite->sReflectionObjEventId];
    mainSprite = &gSprites[objectEvent->spriteId];
    if (!objectEvent->active || !objectEvent->hasReflection || objectEvent->localId != reflectionSprite->sReflectionObjEventLocalId)
    {
        reflectionSprite->inUse = FALSE;
    }
    else
    {
        reflectionSprite->oam.paletteNum = gReflectionEffectPaletteMap[mainSprite->oam.paletteNum];
        reflectionSprite->oam.shape = mainSprite->oam.shape;
        reflectionSprite->oam.size = mainSprite->oam.size;
        reflectionSprite->oam.matrixNum = mainSprite->oam.matrixNum | 0x10;
        reflectionSprite->oam.tileNum = mainSprite->oam.tileNum;
        reflectionSprite->subspriteTables = mainSprite->subspriteTables;
        reflectionSprite->subspriteTableNum = mainSprite->subspriteTableNum;
        reflectionSprite->invisible = mainSprite->invisible;
        reflectionSprite->x = mainSprite->x;
        // sReflectionVerticalOffset holds an additional offset for the high bridges on Route 120.
        reflectionSprite->y = mainSprite->y + GetReflectionVerticalOffset(objectEvent) + reflectionSprite->sReflectionVerticalOffset;
        reflectionSprite->centerToCornerVecX = mainSprite->centerToCornerVecX;
        reflectionSprite->centerToCornerVecY = mainSprite->centerToCornerVecY;
        reflectionSprite->x2 = mainSprite->x2;
        reflectionSprite->y2 = -mainSprite->y2;
        reflectionSprite->coordOffsetEnabled = mainSprite->coordOffsetEnabled;

        // Check if the reflection is not still.
        if (reflectionSprite->sIsStillReflection == FALSE)
        {
            // Sets the reflection sprite's rot/scale matrix to the appropriate
            // matrix based on whether or not the main sprite is horizontally flipped.
            // If the sprite is facing to the east, then it is flipped, and its matrixNum is 8.
            reflectionSprite->oam.matrixNum = 0;
            if (mainSprite->oam.matrixNum & 0x8)
                reflectionSprite->oam.matrixNum = 1;
        }
    }
}

#undef sReflectionObjEventId
#undef sReflectionObjEventLocalId
#undef sReflectionVerticalOffset
#undef sIsStillReflection

#define sPrevX data[0]
#define sPrevY data[1]

u8 CreateWarpArrowSprite(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_ARROW], 0, 0, 0x52);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->oam.priority = 1;
        sprite->coordOffsetEnabled = TRUE;
        sprite->invisible = TRUE;
    }
    return spriteId;
}

void SetSpriteInvisible(u8 spriteId)
{
    gSprites[spriteId].invisible = TRUE;
}

void ShowWarpArrowSprite(u8 spriteId, u8 direction, s16 x, s16 y)
{
    s16 x2;
    s16 y2;
    struct Sprite *sprite;

    sprite = &gSprites[spriteId];
    if (sprite->invisible || sprite->sPrevX != x || sprite->sPrevY != y)
    {
        SetSpritePosToMapCoords(x, y, &x2, &y2);
        sprite = &gSprites[spriteId];
        sprite->x = x2 + 8;
        sprite->y = y2 + 8;
        sprite->invisible = FALSE;
        sprite->sPrevX = x;
        sprite->sPrevY = y;
        StartSpriteAnim(sprite, direction - 1);
    }
}

#undef sPrevX
#undef sPrevY

static const u8 sShadowEffectTemplateIds[] = {
    FLDEFFOBJ_SHADOW_S,
    FLDEFFOBJ_SHADOW_M,
    FLDEFFOBJ_SHADOW_L,
    FLDEFFOBJ_SHADOW_XL
};

const u16 gShadowVerticalOffsets[] = {
    4,
    4,
    4,
    16
};

#define sShadowLocalId  data[0]
#define sShadowMapNum   data[1]
#define sShadowMapGroup data[2]
#define sShadowYOffset  data[3]

u32 FldEff_Shadow(void)
{
    u8 objectEventId;
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    u8 spriteId;

    objectEventId = GetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    graphicsInfo = GetObjectEventGraphicsInfo(gObjectEvents[objectEventId].graphicsId);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[sShadowEffectTemplateIds[graphicsInfo->shadowSize]], 0, 0, 0x94);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].coordOffsetEnabled = TRUE;
        gSprites[spriteId].sShadowLocalId = gFieldEffectArguments[0];
        gSprites[spriteId].sShadowMapNum = gFieldEffectArguments[1];
        gSprites[spriteId].sShadowMapGroup = gFieldEffectArguments[2];
        gSprites[spriteId].sShadowYOffset = (graphicsInfo->height >> 1) - gShadowVerticalOffsets[graphicsInfo->shadowSize];
    }
    return 0;
}

void UpdateShadowFieldEffect(struct Sprite *sprite)
{
    u8 objectEventId;
    struct ObjectEvent *objectEvent;
    struct Sprite *linkedSprite;

    if (TryGetObjectEventIdByLocalIdAndMap(sprite->sShadowLocalId, sprite->sShadowMapNum, sprite->sShadowMapGroup, &objectEventId))
    {
        FieldEffectStop(sprite, FLDEFF_SHADOW);
    }
    else
    {
        objectEvent = &gObjectEvents[objectEventId];
        linkedSprite = &gSprites[objectEvent->spriteId];
        sprite->oam.priority = linkedSprite->oam.priority;
        sprite->x = linkedSprite->x;
        sprite->y = linkedSprite->y + sprite->sShadowYOffset;
        if (!objectEvent->active || !objectEvent->hasShadow
         || MetatileBehavior_IsPokeGrass(objectEvent->currentMetatileBehavior)
         || MetatileBehavior_IsSurfableWaterOrUnderwater(objectEvent->currentMetatileBehavior)
         || MetatileBehavior_IsSurfableWaterOrUnderwater(objectEvent->previousMetatileBehavior)
         || MetatileBehavior_IsReflective(objectEvent->currentMetatileBehavior)
         || MetatileBehavior_IsReflective(objectEvent->previousMetatileBehavior))
        {
            FieldEffectStop(sprite, FLDEFF_SHADOW);
        }
    }
}

#undef sShadowLocalId
#undef sShadowMapNum
#undef sShadowMapGroup
#undef sShadowYOffset

#define sElevation   data[0]
#define sX           data[1]
#define sY           data[2]
#define sMapNum      data[3]
#define sLocalId     data[3] >> 8
#define sMapGroup    data[4]
#define sCurrentMap  data[5]
#define sObjectMoved data[7]

u32 FldEff_TallGrass(void)
{
    s16 x;
    s16 y;
    u8 spriteId;
    struct Sprite *sprite;

    x = gFieldEffectArguments[0];
    y = gFieldEffectArguments[1];
    SetSpritePosToOffsetMapCoords(&x, &y, 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_TALL_GRASS], x, y, 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sElevation = gFieldEffectArguments[2];
        sprite->sX = gFieldEffectArguments[0];
        sprite->sY = gFieldEffectArguments[1];
        sprite->sMapNum = gFieldEffectArguments[4];
        sprite->sMapGroup = gFieldEffectArguments[5];
        sprite->sCurrentMap = gFieldEffectArguments[6];
        if (gFieldEffectArguments[7])
        {
            SeekSpriteAnim(sprite, 4);
        }
    }
    return 0;
}

void UpdateTallGrassFieldEffect(struct Sprite *sprite)
{
    u8 mapNum;
    u8 mapGroup;
    u8 metatileBehavior;
    u8 localId;
    u8 objectEventId;
    struct ObjectEvent *objectEvent;

    mapNum = sprite->sCurrentMap >> 8;
    mapGroup = sprite->sCurrentMap;
    if (gCamera.active && (gSaveBlock1.location.mapNum != mapNum || gSaveBlock1.location.mapGroup != mapGroup))
    {
        sprite->sX -= gCamera.x;
        sprite->sY -= gCamera.y;
        sprite->sCurrentMap = ((u8)gSaveBlock1.location.mapNum << 8) | (u8)gSaveBlock1.location.mapGroup;
    }
    localId = sprite->sLocalId;
    mapNum = sprite->sMapNum;
    mapGroup = sprite->sMapGroup;
    metatileBehavior = MapGridGetMetatileBehaviorAt(sprite->sX, sprite->sY);
    if (TryGetObjectEventIdByLocalIdAndMap(localId, mapNum, mapGroup, &objectEventId) || !MetatileBehavior_IsTallGrass(metatileBehavior) || (sprite->sObjectMoved && sprite->animEnded))
    {
        FieldEffectStop(sprite, FLDEFF_TALL_GRASS);
    }
    else
    {
        objectEvent = &gObjectEvents[objectEventId];
        if ((objectEvent->currentCoords.x != sprite->sX || objectEvent->currentCoords.y != sprite->sY) && (objectEvent->previousCoords.x != sprite->sX || objectEvent->previousCoords.y != sprite->sY))
            sprite->sObjectMoved = TRUE;

        metatileBehavior = 0;
        if (sprite->animCmdIndex == 0)
            metatileBehavior = 4;

        UpdateObjectEventSpriteVisibility(sprite, 0);
        UpdateGrassFieldEffectSubpriority(sprite, sprite->sElevation, metatileBehavior);
    }
}

u32 FldEff_JumpTallGrass(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 12);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_JUMP_TALL_GRASS], gFieldEffectArguments[0], gFieldEffectArguments[1], 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sJumpElevation = gFieldEffectArguments[2];
        sprite->sJumpFldEff = FLDEFF_JUMP_TALL_GRASS;
    }
    return 0;
}

u8 FindTallGrassFieldEffectSpriteId(u8 localId, u8 mapNum, u8 mapGroup, s16 x, s16 y)
{
    struct Sprite *sprite;
    u8 i;

    for (i = 0; i < MAX_SPRITES; i ++)
    {
        if (gSprites[i].inUse)
        {
            sprite = &gSprites[i];
            if (sprite->callback == UpdateTallGrassFieldEffect && (x == sprite->sX && y == sprite->sY) && (localId == (u8)(sprite->sLocalId) && mapNum == (sprite->sMapNum & 0xFF) && mapGroup == sprite->sMapGroup))
                return i;
        }
    }

    return MAX_SPRITES;
}

u32 FldEff_LongGrass(void)
{
    s16 x;
    s16 y;
    u8 spriteId;
    struct Sprite *sprite;

    x = gFieldEffectArguments[0];
    y = gFieldEffectArguments[1];
    SetSpritePosToOffsetMapCoords(&x, &y, 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_LONG_GRASS], x, y, 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = ZCoordToPriority(gFieldEffectArguments[2]);
        sprite->sElevation = gFieldEffectArguments[2];
        sprite->sX = gFieldEffectArguments[0];
        sprite->sY = gFieldEffectArguments[1];
        sprite->sMapNum = gFieldEffectArguments[4];
        sprite->sMapGroup = gFieldEffectArguments[5];
        sprite->sCurrentMap = gFieldEffectArguments[6];
        if (gFieldEffectArguments[7])
        {
            SeekSpriteAnim(sprite, 6);
        }
    }
    return 0;
}

void UpdateLongGrassFieldEffect(struct Sprite *sprite)
{
    u8 mapNum;
    u8 mapGroup;
    u8 metatileBehavior;
    u8 localId;
    u8 objectEventId;
    struct ObjectEvent *objectEvent;

    mapNum = sprite->sCurrentMap >> 8;
    mapGroup = sprite->sCurrentMap;
    if (gCamera.active && (gSaveBlock1.location.mapNum != mapNum || gSaveBlock1.location.mapGroup != mapGroup))
    {
        sprite->sX -= gCamera.x;
        sprite->sY -= gCamera.y;
        sprite->sCurrentMap = ((u8)gSaveBlock1.location.mapNum << 8) | (u8)gSaveBlock1.location.mapGroup;
    }
    localId = sprite->sLocalId;
    mapNum = sprite->sMapNum;
    mapGroup = sprite->sMapGroup;
    metatileBehavior = MapGridGetMetatileBehaviorAt(sprite->sX, sprite->sY);
    if (TryGetObjectEventIdByLocalIdAndMap(localId, mapNum, mapGroup, &objectEventId) || !MetatileBehavior_IsLongGrass(metatileBehavior) || (sprite->sObjectMoved && sprite->animEnded))
    {
        FieldEffectStop(sprite, FLDEFF_LONG_GRASS);
    }
    else
    {
        objectEvent = &gObjectEvents[objectEventId];
        if ((objectEvent->currentCoords.x != sprite->sX || objectEvent->currentCoords.y != sprite->sY) && (objectEvent->previousCoords.x != sprite->sX || objectEvent->previousCoords.y != sprite->sY))
        {
            sprite->sObjectMoved = TRUE;
        }
        UpdateObjectEventSpriteVisibility(sprite, 0);
        UpdateGrassFieldEffectSubpriority(sprite, sprite->sElevation, 0);
    }
}

#undef sElevation
#undef sX
#undef sY
#undef sMapNum
#undef sLocalId
#undef sMapGroup
#undef sCurrentMap
#undef sObjectMoved

u32 FldEff_JumpLongGrass(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_JUMP_LONG_GRASS], gFieldEffectArguments[0], gFieldEffectArguments[1], 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sJumpElevation = gFieldEffectArguments[2];
        sprite->sJumpFldEff = FLDEFF_JUMP_LONG_GRASS;
    }
    return 0;
}

#define sLocalId  data[0]
#define sMapNum   data[1]
#define sMapGroup data[2]
#define sPrevX    data[3]
#define sPrevY    data[4]

u32 FldEff_ShortGrass(void)
{
    u8 objectEventId;
    struct ObjectEvent *objectEvent;
    u8 spriteId;
    struct Sprite *sprite;

    objectEventId = GetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    objectEvent = &gObjectEvents[objectEventId];
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SHORT_GRASS], 0, 0, 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &(gSprites[spriteId]);
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gSprites[objectEvent->spriteId].oam.priority;
        sprite->sLocalId = gFieldEffectArguments[0];
        sprite->sMapNum = gFieldEffectArguments[1];
        sprite->sMapGroup = gFieldEffectArguments[2];
        sprite->sPrevX = gSprites[objectEvent->spriteId].x;
        sprite->sPrevY = gSprites[objectEvent->spriteId].y;
    }
    return 0;
}

void UpdateShortGrassFieldEffect(struct Sprite *sprite)
{
    u8 objectEventId;
    s16 x;
    s16 y;
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    struct Sprite *linkedSprite;

    if (TryGetObjectEventIdByLocalIdAndMap(sprite->sLocalId, sprite->sMapNum, sprite->sMapGroup, &objectEventId) || !gObjectEvents[objectEventId].inShortGrass)
    {
        FieldEffectStop(sprite, FLDEFF_SHORT_GRASS);
    }
    else
    {
        graphicsInfo = GetObjectEventGraphicsInfo(gObjectEvents[objectEventId].graphicsId);
        linkedSprite = &gSprites[gObjectEvents[objectEventId].spriteId];
        y = linkedSprite->y;
        x = linkedSprite->x;
        if (x != sprite->sPrevX || y != sprite->sPrevY)
        {
            sprite->sPrevX = x;
            sprite->sPrevY = y;
            if (sprite->animEnded)
            {
                StartSpriteAnim(sprite, 0);
            }
        }
        sprite->x = x;
        sprite->y = y;
        sprite->y2 = (graphicsInfo->height >> 1) - 8;
        sprite->subpriority = linkedSprite->subpriority - 1;
        sprite->oam.priority = linkedSprite->oam.priority;
        UpdateObjectEventSpriteVisibility(sprite, linkedSprite->invisible);
    }
}

#undef sLocalId
#undef sMapNum
#undef sMapGroup
#undef sPrevX
#undef sPrevY

#define sState  data[0]
#define sTimer  data[1]
#define sFldEff data[7]

u32 FldEff_SandFootprints(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SAND_FOOTPRINTS], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sFldEff = FLDEFF_SAND_FOOTPRINTS;
        StartSpriteAnim(sprite, gFieldEffectArguments[4]);
    }
    return 0;
}

u32 FldEff_DeepSandFootprints(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_DEEP_SAND_FOOTPRINTS], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sFldEff = FLDEFF_DEEP_SAND_FOOTPRINTS;
        StartSpriteAnim(sprite, gFieldEffectArguments[4]);
    }
    return spriteId;
}

u32 FldEff_BikeTireTracks(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_BIKE_TIRE_TRACKS], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sFldEff = FLDEFF_BIKE_TIRE_TRACKS;
        StartSpriteAnim(sprite, gFieldEffectArguments[4]);
    }
    return spriteId;
}

void (*const gFadeFootprintsTireTracksFuncs[])(struct Sprite *) = {
    FadeFootprintsTireTracks_Step0,
    FadeFootprintsTireTracks_Step1
};

void UpdateFootprintsTireTracksFieldEffect(struct Sprite *sprite)
{
    gFadeFootprintsTireTracksFuncs[sprite->sState](sprite);
}

static void FadeFootprintsTireTracks_Step0(struct Sprite *sprite)
{
    // Wait 40 frames before the flickering starts.
    if (++sprite->sTimer > 40)
        sprite->sState = 1;

    UpdateObjectEventSpriteVisibility(sprite, FALSE);
}

static void FadeFootprintsTireTracks_Step1(struct Sprite *sprite)
{
    sprite->invisible ^= 1;
    sprite->sTimer++;
    UpdateObjectEventSpriteVisibility(sprite, sprite->invisible);
    if (sprite->sTimer > 56)
    {
        FieldEffectStop(sprite, sprite->sFldEff);
    }
}

#undef sState
#undef sTimer
#undef sFldEff

#define sLocalId  data[0]
#define sMapNum   data[1]
#define sMapGroup data[2]

u32 FldEff_Splash(void)
{
    u8 objectEventId;
    struct ObjectEvent *objectEvent;
    u8 spriteId;
    struct Sprite *sprite;
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    struct Sprite *linkedSprite;

    objectEventId = GetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    objectEvent = &gObjectEvents[objectEventId];
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SPLASH], 0, 0, 0);
    if (spriteId != MAX_SPRITES)
    {
        graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        linkedSprite = &gSprites[objectEvent->spriteId];
        sprite->oam.priority = linkedSprite->oam.priority;
        sprite->sLocalId = gFieldEffectArguments[0];
        sprite->sMapNum = gFieldEffectArguments[1];
        sprite->sMapGroup = gFieldEffectArguments[2];
        sprite->y2 = (graphicsInfo->height >> 1) - 4;
        PlaySE(SE_PUDDLE);
    }
    return 0;
}

void UpdateSplashFieldEffect(struct Sprite *sprite)
{
    u8 objectEventId;

    if (sprite->animEnded || TryGetObjectEventIdByLocalIdAndMap(sprite->sLocalId, sprite->sMapNum, sprite->sMapGroup, &objectEventId))
    {
        FieldEffectStop(sprite, FLDEFF_SPLASH);
    }
    else
    {
        sprite->x = gSprites[gObjectEvents[objectEventId].spriteId].x;
        sprite->y = gSprites[gObjectEvents[objectEventId].spriteId].y;
        UpdateObjectEventSpriteVisibility(sprite, FALSE);
    }
}

#undef sLocalId
#undef sMapNum
#undef sMapGroup

u32 FldEff_JumpSmallSplash(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 12);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_JUMP_SMALL_SPLASH], gFieldEffectArguments[0], gFieldEffectArguments[1], 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sJumpElevation = gFieldEffectArguments[2];
        sprite->sJumpFldEff = FLDEFF_JUMP_SMALL_SPLASH;
    }
    return 0;
}

u32 FldEff_JumpBigSplash(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_JUMP_BIG_SPLASH], gFieldEffectArguments[0], gFieldEffectArguments[1], 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sJumpElevation = gFieldEffectArguments[2];
        sprite->sJumpFldEff = FLDEFF_JUMP_BIG_SPLASH;
    }
    return 0;
}

#define sLocalId  data[0]
#define sMapNum   data[1]
#define sMapGroup data[2]
#define sPrevX    data[3]
#define sPrevY    data[4]

u32 FldEff_FeetInFlowingWater(void)
{
    u8 objectEventId;
    struct ObjectEvent *objectEvent;
    u8 spriteId;
    struct Sprite *sprite;
    const struct ObjectEventGraphicsInfo *graphicsInfo;

    objectEventId = GetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    objectEvent = &gObjectEvents[objectEventId];
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SPLASH], 0, 0, 0);
    if (spriteId != MAX_SPRITES)
    {
        graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
        sprite = &gSprites[spriteId];
        sprite->callback = UpdateFeetInFlowingWaterFieldEffect;
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gSprites[objectEvent->spriteId].oam.priority;
        sprite->sLocalId = gFieldEffectArguments[0];
        sprite->sMapNum = gFieldEffectArguments[1];
        sprite->sMapGroup = gFieldEffectArguments[2];
        sprite->sPrevX = -1;
        sprite->sPrevY = -1;
        sprite->y2 = (graphicsInfo->height >> 1) - 4;
        StartSpriteAnim(sprite, 1);
    }
    return 0;
}

static void UpdateFeetInFlowingWaterFieldEffect(struct Sprite *sprite)
{
    u8 objectEventId;
    struct Sprite *linkedSprite;
    struct ObjectEvent *objectEvent;

    if (TryGetObjectEventIdByLocalIdAndMap(sprite->sLocalId, sprite->sMapNum, sprite->sMapGroup, &objectEventId) || !gObjectEvents[objectEventId].inShallowFlowingWater)
    {
        FieldEffectStop(sprite, FLDEFF_FEET_IN_FLOWING_WATER);
    }
    else
    {
        objectEvent = &gObjectEvents[objectEventId];
        linkedSprite = &gSprites[objectEvent->spriteId];
        sprite->x = linkedSprite->x;
        sprite->y = linkedSprite->y;
        sprite->subpriority = linkedSprite->subpriority;
        UpdateObjectEventSpriteVisibility(sprite, FALSE);
        if (objectEvent->currentCoords.x != sprite->sPrevX || objectEvent->currentCoords.y != sprite->sPrevY)
        {
            sprite->sPrevX = objectEvent->currentCoords.x;
            sprite->sPrevY = objectEvent->currentCoords.y;
            if (!sprite->invisible)
            {
                PlaySE(SE_PUDDLE);
            }
        }
    }
}

#undef sLocalId
#undef sMapNum
#undef sMapGroup
#undef sPrevX
#undef sPrevY

u32 FldEff_Ripple(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_RIPPLE], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sWaitFldEff = FLDEFF_RIPPLE;
    }
    return 0;
}

#define sLocalId  data[0]
#define sMapNum   data[1]
#define sMapGroup data[2]
#define sPrevX    data[3]
#define sPrevY    data[4]

u32 FldEff_HotSpringsWater(void)
{
    u8 objectEventId;
    struct ObjectEvent *objectEvent;
    u8 spriteId;
    struct Sprite *sprite;

    objectEventId = GetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    objectEvent = &gObjectEvents[objectEventId];
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_HOT_SPRINGS_WATER], 0, 0, 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gSprites[objectEvent->spriteId].oam.priority;
        sprite->sLocalId = gFieldEffectArguments[0];
        sprite->sMapNum = gFieldEffectArguments[1];
        sprite->sMapGroup = gFieldEffectArguments[2];
        sprite->sPrevX = gSprites[objectEvent->spriteId].x;
        sprite->sPrevY = gSprites[objectEvent->spriteId].y;
    }
    return 0;
}

void UpdateHotSpringsWaterFieldEffect(struct Sprite *sprite)
{
    u8 objectEventId;
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    struct Sprite *linkedSprite;

    if (TryGetObjectEventIdByLocalIdAndMap(sprite->sLocalId, sprite->sMapNum, sprite->sMapGroup, &objectEventId) || !gObjectEvents[objectEventId].inHotSprings)
    {
        FieldEffectStop(sprite, FLDEFF_HOT_SPRINGS_WATER);
    }
    else
    {
        graphicsInfo = GetObjectEventGraphicsInfo(gObjectEvents[objectEventId].graphicsId);
        linkedSprite = &gSprites[gObjectEvents[objectEventId].spriteId];
        sprite->x = linkedSprite->x;
        sprite->y = (graphicsInfo->height >> 1) + linkedSprite->y - 8;
        sprite->subpriority = linkedSprite->subpriority - 1;
        UpdateObjectEventSpriteVisibility(sprite, FALSE);
    }
}

#undef sLocalId
#undef sMapNum
#undef sMapGroup
#undef sPrevX
#undef sPrevY

u32 FldEff_UnusedGrass(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_UNUSED_GRASS], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sWaitFldEff = FLDEFF_UNUSED_GRASS;
    }
    return 0;
}

u32 FldEff_UnusedGrass2(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_UNUSED_GRASS_2], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sWaitFldEff = FLDEFF_UNUSED_GRASS_2;
    }
    return 0;
}

u32 FldEff_UnusedSand(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_UNUSED_SAND], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sWaitFldEff = FLDEFF_UNUSED_SAND;
    }
    return 0;
}

u32 FldEff_WaterSurfacing(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_WATER_SURFACING], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sWaitFldEff = FLDEFF_WATER_SURFACING;
    }
    return 0;
}

#define sAshState      data[0]
#define sAshX          data[1]
#define sAshY          data[2]
#define sAshMetatileId data[3]
#define sAshDelay      data[4]

void StartAshFieldEffect(s16 x, s16 y, u16 metatileId, s16 delay)
{
    gFieldEffectArguments[0] = x;
    gFieldEffectArguments[1] = y;
    gFieldEffectArguments[2] = 0x52;
    gFieldEffectArguments[3] = 1;
    gFieldEffectArguments[4] = metatileId;
    gFieldEffectArguments[5] = delay;
    FieldEffectStart(FLDEFF_ASH);
}

u32 FldEff_Ash(void)
{
    s16 x;
    s16 y;
    u8 spriteId;
    struct Sprite *sprite;

    x = gFieldEffectArguments[0];
    y = gFieldEffectArguments[1];
    SetSpritePosToOffsetMapCoords(&x, &y, 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_ASH], x, y, gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sAshX = gFieldEffectArguments[0];
        sprite->sAshY = gFieldEffectArguments[1];
        sprite->sAshMetatileId = gFieldEffectArguments[4];
        sprite->sAshDelay = gFieldEffectArguments[5];
    }
    return 0;
}

void (*const gAshFieldEffectFuncs[])(struct Sprite *) = {
    UpdateAshFieldEffect_Wait,
    UpdateAshFieldEffect_Show,
    UpdateAshFieldEffect_End
};

void UpdateAshFieldEffect(struct Sprite *sprite)
{
    gAshFieldEffectFuncs[sprite->sAshState](sprite);
}

static void UpdateAshFieldEffect_Wait(struct Sprite *sprite)
{
    sprite->invisible = TRUE;
    sprite->animPaused = TRUE;
    if (--sprite->sAshDelay == 0)
        sprite->sAshState = 1;
}

static void UpdateAshFieldEffect_Show(struct Sprite *sprite)
{
    sprite->invisible = FALSE;
    sprite->animPaused = FALSE;
    MapGridSetMetatileIdAt(sprite->sAshX, sprite->sAshY, sprite->sAshMetatileId);
    CurrentMapDrawMetatileAt(sprite->sAshX, sprite->sAshY);
    gObjectEvents[gPlayerAvatar.objectEventId].triggerGroundEffectsOnMove = TRUE;
    sprite->sAshState = 2;
}

static void UpdateAshFieldEffect_End(struct Sprite *sprite)
{
    UpdateObjectEventSpriteVisibility(sprite, FALSE);
    if (sprite->animEnded)
        FieldEffectStop(sprite, FLDEFF_ASH);
}

#undef sAshState
#undef sAshX
#undef sAshY
#undef sAshMetatileId
#undef sAshDelay

#define sBitfield     data[0]
#define sPlayerOffset data[1]
#define sPlayerObjId  data[2]
#define sVelocity     data[3]
#define sTimer        data[4]
#define sIntervalIdx  data[5]
#define sPrevX        data[6]
#define sPrevY        data[7]

u32 FldEff_SurfBlob(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SURF_BLOB], gFieldEffectArguments[0], gFieldEffectArguments[1], 0x96);
    if (spriteId !=MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.paletteNum = 0;
        sprite->sPlayerObjId = gFieldEffectArguments[2];
        sprite->sVelocity = -1;
        sprite->sPrevX = -1;
        sprite->sPrevY = -1;
    }
    FieldEffectActiveListRemove(FLDEFF_SURF_BLOB);
    return spriteId;
}

void SetSurfBlob_BobState(u8 spriteId, u8 state)
{
    gSprites[spriteId].sBitfield = (gSprites[spriteId].sBitfield & ~0xF) | (state & 0xF);
}

void SetSurfBlob_DontSyncAnim(u8 spriteId, u8 dontSync)
{
    gSprites[spriteId].sBitfield = (gSprites[spriteId].sBitfield & ~0xF0) | ((dontSync & 0xF) << 4);
}

void SetSurfBlob_PlayerOffset(u8 spriteId, u8 hasOffset, s16 offset)
{
    gSprites[spriteId].sBitfield = (gSprites[spriteId].sBitfield & ~0xF00) | ((hasOffset & 0xF) << 8);
    gSprites[spriteId].sPlayerOffset = offset;
}

static u8 GetSurfBlob_BobState(struct Sprite *sprite)
{
    return sprite->sBitfield & 0xF;
}

static u8 GetSurfBlob_DontSyncAnim(struct Sprite *sprite)
{
    return (sprite->sBitfield & 0xF0) >> 4;
}

static u8 GetSurfBlob_HasPlayerOffset(struct Sprite *sprite)
{
    return (sprite->sBitfield & 0xF00) >> 8;
}

void UpdateSurfBlobFieldEffect(struct Sprite *sprite)
{
    struct ObjectEvent *objectEvent;
    struct Sprite *linkedSprite;

    objectEvent = &gObjectEvents[sprite->sPlayerObjId];
    linkedSprite = &gSprites[objectEvent->spriteId];
    SynchronizeSurfAnim(objectEvent, sprite);
    SynchronizeSurfPosition(objectEvent, sprite);
    UpdateBobbingEffect(objectEvent, linkedSprite, sprite);
    sprite->oam.priority = linkedSprite->oam.priority;
}

static void SynchronizeSurfAnim(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    u8 surfBlobDirectionAnims[] = {
        0, // DIR_NONE
        0, // DIR_SOUTH
        1, // DIR_NORTH
        2, // DIR_WEST
        3, // DIR_EAST
    };

    if (GetSurfBlob_DontSyncAnim(sprite) == 0)
        StartSpriteAnimIfDifferent(sprite, surfBlobDirectionAnims[objectEvent->movementDirection]);
}

static void SynchronizeSurfPosition(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    u8 i;
    s16 x = objectEvent->currentCoords.x;
    s16 y = objectEvent->currentCoords.y;
    s32 spriteY = sprite->y2;

    if (spriteY == 0 && (x != sprite->sPrevX || y != sprite->sPrevY))
    {
        sprite->sIntervalIdx = spriteY;
        sprite->sPrevX = x;
        sprite->sPrevY = y;
        for (i = DIR_SOUTH; i <= DIR_EAST; i++, x = sprite->sPrevX, y = sprite->sPrevY)
        {
            MoveCoords(i, &x, &y);
            if (MapGridGetElevationAt(x, y) == 3)
            {
                sprite->sIntervalIdx++;
                break;
            }
        }
    }
}

static void UpdateBobbingEffect(struct ObjectEvent *objectEvent, struct Sprite *linkedSprite, struct Sprite *sprite)
{
    u16 intervals[] = {3, 7};
    u8 bobState = GetSurfBlob_BobState(sprite);
    if (bobState != 0)
    {
        if (((u16)(++ sprite->sTimer) & intervals[sprite->sIntervalIdx]) == 0)
        {
            sprite->y2 += sprite->sVelocity;
        }
        if ((sprite->sTimer & 0x0F) == 0)
        {
            sprite->sVelocity = -sprite->sVelocity;
        }
        if (bobState != 2)
        {
            if (GetSurfBlob_HasPlayerOffset(sprite) == 0)
            {
                linkedSprite->y2 = sprite->y2;
            }
            else
            {
                linkedSprite->y2 = sprite->sPlayerOffset + sprite->y2;
            }
            sprite->x = linkedSprite->x;
            sprite->y = linkedSprite->y + 8;
        }
    }
}

#undef sBitfield
#undef sPlayerOffset
#undef sPlayerObjId
#undef sVelocity
#undef sTimer
#undef sIntervalIdx
#undef sPrevX
#undef sPrevY

#define sSpriteId data[0]
#define sBobY     data[1]
#define sTimer    data[2]

u8 StartUnderwaterSurfBlobBobbing(u8 blobSpriteId)
{
    u8 spriteId;
    struct Sprite *sprite;

    spriteId = CreateSpriteAtEnd(&gDummySpriteTemplate, 0, 0, -1);
    sprite = &gSprites[spriteId];
    sprite->callback = SpriteCB_UnderwaterSurfBlob;
    sprite->invisible = TRUE;
    sprite->sSpriteId = blobSpriteId;
    sprite->sBobY = 1;
    return spriteId;
}

static void SpriteCB_UnderwaterSurfBlob(struct Sprite *sprite)
{
    struct Sprite *oldSprite;

    oldSprite = &gSprites[sprite->sSpriteId];
    if (((sprite->sTimer++) & 0x03) == 0)
    {
        oldSprite->y2 += sprite->sBobY;
    }
    if ((sprite->sTimer & 0x0F) == 0)
    {
        sprite->sBobY = -sprite->sBobY;
    }
}

#undef sSpriteId
#undef sBobY
#undef sTimer

u32 FldEff_Dust(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 12);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_GROUND_IMPACT_DUST], gFieldEffectArguments[0], gFieldEffectArguments[1], 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->sJumpElevation = gFieldEffectArguments[2];
        sprite->sJumpFldEff = FLDEFF_DUST;
    }
    return 0;
}

#define sLocalId  data[0]
#define sMapNum   data[1]
#define sMapGroup data[2]
#define sPrevX    data[3]
#define sPrevY    data[4]

u32 FldEff_SandPile(void)
{
    u8 objectEventId;
    struct ObjectEvent *objectEvent;
    u8 spriteId;
    struct Sprite *sprite;
    const struct ObjectEventGraphicsInfo *graphicsInfo;

    objectEventId = GetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    objectEvent = &gObjectEvents[objectEventId];
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SAND_PILE], 0, 0, 0);
    if (spriteId != MAX_SPRITES)
    {
        graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gSprites[objectEvent->spriteId].oam.priority;
        sprite->sLocalId = gFieldEffectArguments[0];
        sprite->sMapNum = gFieldEffectArguments[1];
        sprite->sMapGroup = gFieldEffectArguments[2];
        sprite->sPrevX = gSprites[objectEvent->spriteId].x;
        sprite->sPrevY = gSprites[objectEvent->spriteId].y;
        sprite->y2 = (graphicsInfo->height >> 1) - 2;
        SeekSpriteAnim(sprite, 2);
    }
    return 0;
}

void UpdateSandPileFieldEffect(struct Sprite *sprite)
{
    u8 objectEventId;
    s16 x;
    s16 y;

    if (TryGetObjectEventIdByLocalIdAndMap(sprite->sLocalId, sprite->sMapNum, sprite->sMapGroup, &objectEventId) || !gObjectEvents[objectEventId].inSandPile)
    {
        FieldEffectStop(sprite, FLDEFF_SAND_PILE);
    }
    else
    {
        y = gSprites[gObjectEvents[objectEventId].spriteId].y;
        x = gSprites[gObjectEvents[objectEventId].spriteId].x;
        if (x != sprite->sPrevX || y != sprite->sPrevY)
        {
            sprite->sPrevX = x;
            sprite->sPrevY = y;
            if (sprite->animEnded)
            {
                StartSpriteAnim(sprite, 0);
            }
        }
        sprite->x = x;
        sprite->y = y;
        sprite->subpriority = gSprites[gObjectEvents[objectEventId].spriteId].subpriority;
        UpdateObjectEventSpriteVisibility(sprite, FALSE);
    }
}

#undef sLocalId
#undef sMapNum
#undef sMapGroup
#undef sPrevX
#undef sPrevY

u32 FldEff_Bubbles(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 0);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_BUBBLES], gFieldEffectArguments[0], gFieldEffectArguments[1], 0x52);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = 1;
    }
    return 0;
}

#define sY data[0]

void UpdateBubblesFieldEffect(struct Sprite *sprite)
{
    sprite->sY += 0x80;
    sprite->sY &= 0x100;
    sprite->y -= sprite->sY >> 8;
    UpdateObjectEventSpriteVisibility(sprite, FALSE);
    if (sprite->invisible || sprite->animEnded)
    {
        FieldEffectStop(sprite, FLDEFF_BUBBLES);
    }
}

#undef sY

u32 FldEff_BerryTreeGrowthSparkle(void)
{
    u8 spriteId;
    struct Sprite *sprite;

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 4);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SPARKLE], gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2]);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->oam.priority = gFieldEffectArguments[3];
        sprite->oam.paletteNum = 5;
        sprite->sWaitFldEff = FLDEFF_BERRY_TREE_GROWTH_SPARKLE;
    }
    return 0;
}

#define sDisguiseState      data[0]
#define sDisguiseFldEff     data[1]
#define sDisguiseLocalId    data[2]
#define sDisguiseMapNum     data[3]
#define sDisguiseMapGroup   data[4]
#define sDisguiseReadyToEnd data[7]

u32 ShowTreeDisguiseFieldEffect(void)
{
    return ShowDisguiseFieldEffect(FLDEFF_TREE_DISGUISE, 24, 4);
}


u32 ShowMountainDisguiseFieldEffect(void)
{
    return ShowDisguiseFieldEffect(FLDEFF_MOUNTAIN_DISGUISE, 25, 3);
}


u32 ShowSandDisguiseFieldEffect(void)
{
    return ShowDisguiseFieldEffect(FLDEFF_SAND_DISGUISE, 28, 2);
}

static u32 ShowDisguiseFieldEffect(u8 fldEff, u8 fldEffObj, u8 paletteNum)
{
    u8 spriteId;
    struct Sprite *sprite;

    if (TryGetObjectEventIdByLocalIdAndMap(gFieldEffectArguments[0], gFieldEffectArguments[1], gFieldEffectArguments[2], &spriteId))
    {
        FieldEffectActiveListRemove(fldEff);
        return MAX_SPRITES;
    }
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[fldEffObj], 0, 0, 0);
    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled ++;
        sprite->oam.paletteNum = paletteNum;
        sprite->sDisguiseFldEff = fldEff;
        sprite->sDisguiseLocalId = gFieldEffectArguments[0];
        sprite->sDisguiseMapNum = gFieldEffectArguments[1];
        sprite->sDisguiseMapGroup = gFieldEffectArguments[2];
    }
    return spriteId;
}

void UpdateDisguiseFieldEffect(struct Sprite *sprite)
{
    u8 objectEventId;
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    struct Sprite *linkedSprite;

    if (TryGetObjectEventIdByLocalIdAndMap(sprite->sDisguiseLocalId, sprite->sDisguiseMapNum, sprite->sDisguiseMapGroup, &objectEventId))
    {
        FieldEffectStop(sprite, sprite->sDisguiseFldEff);
    }

    graphicsInfo = GetObjectEventGraphicsInfo(gObjectEvents[objectEventId].graphicsId);
    linkedSprite = &gSprites[gObjectEvents[objectEventId].spriteId];
    sprite->invisible = linkedSprite->invisible;
    sprite->x = linkedSprite->x;
    sprite->y = (graphicsInfo->height >> 1) + linkedSprite->y - 16;
    sprite->subpriority = linkedSprite->subpriority - 1;
    if (sprite->sDisguiseState == 1)
    {
        sprite->sDisguiseState++;
        StartSpriteAnim(sprite, 1);
    }
    if (sprite->sDisguiseState == 2 && sprite->animEnded)
    {
        sprite->sDisguiseReadyToEnd = 1;
    }
    if (sprite->sDisguiseState == 3)
    {
        FieldEffectStop(sprite, sprite->sDisguiseFldEff);
    }
}

void StartRevealDisguise(struct ObjectEvent *objectEvent)
{
    if (objectEvent->directionSequenceIndex == 1)
    {
        gSprites[objectEvent->fieldEffectSpriteId].sDisguiseState++;
    }
}

bool8 UpdateRevealDisguise(struct ObjectEvent *objectEvent)
{
    struct Sprite *sprite;

    if (objectEvent->directionSequenceIndex == 2)
    {
        return TRUE;
    }
    if (objectEvent->directionSequenceIndex == 0)
    {
        return TRUE;
    }
    sprite = &gSprites[objectEvent->fieldEffectSpriteId];
    if (sprite->sDisguiseReadyToEnd)
    {
        objectEvent->directionSequenceIndex = 2;
        sprite->sDisguiseState++;
        return TRUE;
    }
    return FALSE;
}

#undef sDisguiseState
#undef sDisguiseFldEff
#undef sDisguiseLocalId
#undef sDisguiseMapNum
#undef sDisguiseMapGroup
#undef sDisguiseReadyToEnd

#define sFinished data[0]
#define sEndTimer data[1]

u32 FldEff_Sparkle(void)
{
    u8 spriteId;

    gFieldEffectArguments[0] += MAP_OFFSET;
    gFieldEffectArguments[1] += MAP_OFFSET;
    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SMALL_SPARKLE], gFieldEffectArguments[0], gFieldEffectArguments[1], 0x52);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].oam.priority = gFieldEffectArguments[2];
        gSprites[spriteId].coordOffsetEnabled = TRUE;
    }
    return 0;
}

void UpdateSparkleFieldEffect(struct Sprite *sprite)
{
    if (sprite->sFinished == 0)
    {
        if (sprite->animEnded)
        {
            sprite->invisible = TRUE;
            sprite->sFinished++;
        }

        if (sprite->sFinished == 0)
            return;
    }

    if (++sprite->sEndTimer > 34)
        FieldEffectStop(sprite, FLDEFF_SPARKLE);
}

#undef sFinished
#undef sEndTimer

void UpdateJumpImpactEffect(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        FieldEffectStop(sprite, sprite->sJumpFldEff);
    }
    else
    {
        UpdateObjectEventSpriteVisibility(sprite, FALSE);
        SetObjectSubpriorityByZCoord(sprite->sJumpElevation, sprite, 0);
    }
}

void WaitFieldEffectSpriteAnim(struct Sprite *sprite)
{
    if (sprite->animEnded)
        FieldEffectStop(sprite, sprite->sWaitFldEff);
    else
        UpdateObjectEventSpriteVisibility(sprite, FALSE);
}

static void UpdateGrassFieldEffectSubpriority(struct Sprite *sprite, u8 elevation, u8 subpriority)
{
    u8 i;
    s16 var, xhi, lyhi, yhi, ylo;
    const struct ObjectEventGraphicsInfo *graphicsInfo; // Unused Variable
    struct Sprite *linkedSprite;

    SetObjectSubpriorityByZCoord(elevation, sprite, subpriority);
    for (i = 0; i < OBJECT_EVENTS_COUNT; i ++)
    {
        struct ObjectEvent *objectEvent = &gObjectEvents[i];
        if (objectEvent->active)
        {
            graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
            linkedSprite = &gSprites[objectEvent->spriteId];
            xhi = sprite->x + sprite->centerToCornerVecX;
            var = sprite->x - sprite->centerToCornerVecX;
            if (xhi < linkedSprite->x && var > linkedSprite->x)
            {
                lyhi = linkedSprite->y + linkedSprite->centerToCornerVecY;
                var = linkedSprite->y;
                ylo = sprite->y - sprite->centerToCornerVecY;
                yhi = ylo + linkedSprite->centerToCornerVecY;
                if ((lyhi < yhi || lyhi < ylo) && var > yhi && sprite->subpriority <= linkedSprite->subpriority)
                {
                    sprite->subpriority = linkedSprite->subpriority + 2;
                    break;
                }
            }
        }
    }
}
