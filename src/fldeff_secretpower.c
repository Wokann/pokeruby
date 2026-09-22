
#include "global.h"
#include "event_data.h"
#include "fieldmap.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "pokemon_menu.h"
#include "rom6.h"
#include "script.h"
#include "secret_base.h"
#include "sound.h"
#include "constants/field_effects.h"
#include "constants/metatile_behaviors.h"
#include "constants/songs.h"

extern u8 SecretBase_EventScript_CaveUseSecretPower[];
extern u8 SecretBase_EventScript_TreeUseSecretPower[];
extern u8 SecretBase_EventScript_ShrubUseSecretPower[];

static const u8 sSecretPowerCave_Gfx[] = INCBIN_U8("graphics/field_effects/pics/secret_power_cave.4bpp");
static const u8 sFiller[32] = {0};
static const u16 sSecretPowerCave_Pal[] = INCBIN_U16("graphics/field_effects/palettes/secret_power_cave.gbapal");
static const u8 sSecretPowerShrub_Gfx[] = INCBIN_U8("graphics/field_effects/pics/secret_power_shrub.4bpp");
static const u8 sSecretPowerTree_Gfx[] = INCBIN_U8("graphics/field_effects/pics/secret_power_tree.4bpp");
static const u16 sSecretPowerPlant_Pal[] = INCBIN_U16("graphics/field_effects/palettes/secret_power_plant.gbapal");
const u8 gSandPillar0_Gfx[] = INCBIN_U8("graphics/field_effect_objects/pics/sand_pillar/0.4bpp");
const u8 gSandPillar1_Gfx[] = INCBIN_U8("graphics/field_effect_objects/pics/sand_pillar/1.4bpp");
const u8 gSandPillar2_Gfx[] = INCBIN_U8("graphics/field_effect_objects/pics/sand_pillar/2.4bpp");

static const struct OamData sOam_SecretPower =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_SecretPowerCave[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_VineDropLeft[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_VineRiseLeft[] =
{
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_VineDropRight[] =
{
    ANIMCMD_FRAME(0, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(2, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(3, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 8, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_VineRiseRight[] =
{
    ANIMCMD_FRAME(4, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(3, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(2, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 8, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_SecretPowerShrub[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnimTable_SecretPowerCave[] =
{
    sAnim_SecretPowerCave,
};

static const union AnimCmd *const sAnimTable_SecretPowerTree[] =
{
    sAnim_VineDropLeft,
    sAnim_VineRiseLeft,
    sAnim_VineDropRight,
    sAnim_VineRiseRight,
};

static const union AnimCmd *const sAnimTable_SecretPowerShrub[] =
{
    sAnim_SecretPowerShrub,
};

static const struct SpriteFrameImage sPicTable_SecretPowerCave[] =
{
    overworld_frame(sSecretPowerCave_Gfx, 2, 2, 0),
    overworld_frame(sSecretPowerCave_Gfx, 2, 2, 1),
    overworld_frame(sSecretPowerCave_Gfx, 2, 2, 2),
    overworld_frame(sSecretPowerCave_Gfx, 2, 2, 3),
    overworld_frame(sSecretPowerCave_Gfx, 2, 2, 4),
};

static const struct SpriteFrameImage sPicTable_SecretPowerTree[] =
{
    overworld_frame(sSecretPowerTree_Gfx, 2, 2, 0),
    overworld_frame(sSecretPowerTree_Gfx, 2, 2, 1),
    overworld_frame(sSecretPowerTree_Gfx, 2, 2, 2),
    overworld_frame(sSecretPowerTree_Gfx, 2, 2, 3),
    overworld_frame(sSecretPowerTree_Gfx, 2, 2, 4),
    // The sixth frame is unused; the tree-vine metatile is used instead.
};

static const struct SpriteFrameImage sPicTable_SecretPowerShrub[] =
{
    overworld_frame(sSecretPowerShrub_Gfx, 2, 2, 0),
    overworld_frame(sSecretPowerShrub_Gfx, 2, 2, 1),
    overworld_frame(sSecretPowerShrub_Gfx, 2, 2, 2),
    overworld_frame(sSecretPowerShrub_Gfx, 2, 2, 3),
    overworld_frame(sSecretPowerShrub_Gfx, 2, 2, 4),
};

static void SpriteCB_CaveEntranceInit(struct Sprite *);
static const struct SpriteTemplate sSpriteTemplate_SecretPowerCave =
{
    .tileTag = 0xFFFF,
    .paletteTag = FLDEFF_PAL_TAG_SECRET_POWER_TREE,
    .oam = &sOam_SecretPower,
    .anims = sAnimTable_SecretPowerCave,
    .images = sPicTable_SecretPowerCave,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CaveEntranceInit,
};

static void SpriteCB_TreeEntranceInit(struct Sprite *);
static const struct SpriteTemplate sSpriteTemplate_SecretPowerTree =
{
    .tileTag = 0xFFFF,
    .paletteTag = FLDEFF_PAL_TAG_SECRET_POWER_PLANT,
    .oam = &sOam_SecretPower,
    .anims = sAnimTable_SecretPowerTree,
    .images = sPicTable_SecretPowerTree,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_TreeEntranceInit,
};

static void SpriteCB_ShrubEntranceInit(struct Sprite *);
static const struct SpriteTemplate sSpriteTemplate_SecretPowerShrub =
{
    .tileTag = 0xFFFF,
    .paletteTag = FLDEFF_PAL_TAG_SECRET_POWER_PLANT,
    .oam = &sOam_SecretPower,
    .anims = sAnimTable_SecretPowerShrub,
    .images = sPicTable_SecretPowerShrub,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_ShrubEntranceInit,
};

const struct SpritePalette gSpritePalette_SecretPower_Cave = {sSecretPowerCave_Pal, FLDEFF_PAL_TAG_SECRET_POWER_TREE};
const struct SpritePalette gSpritePalette_SecretPower_Plant = {sSecretPowerPlant_Pal, FLDEFF_PAL_TAG_SECRET_POWER_PLANT};

static void FieldCallback_SecretBaseCave(void);
static void StartSecretBaseCaveFieldEffect(void);
static void SpriteCB_CaveEntranceOpen(struct Sprite *);
static void SpriteCB_CaveEntranceEnd(struct Sprite *);
static void FieldCallback_SecretBaseTree(void);
static void StartSecretBaseTreeFieldEffect(void);
static void SpriteCB_TreeEntranceOpen(struct Sprite *);
static void SpriteCB_TreeEntranceEnd(struct Sprite *);
static void FieldCallback_SecretBaseShrub(void);
static void StartSecretBaseShrubFieldEffect(void);
static void SpriteCB_ShrubEntranceOpen(struct Sprite *sprite);
static void SpriteCB_ShrubEntranceEnd(struct Sprite *sprite);

static void SetCurrentSecretBase(void)
{
    SetCurrentSecretBaseFromPosition(&gPlayerFacingPosition, gMapHeader.events);
    SetCurrentSecretBaseVar();
}

static void AdjustSecretPowerSpritePixelOffsets(void)
{
    switch (gFieldEffectArguments[1])
    {
    case DIR_SOUTH:
        gFieldEffectArguments[5] = 8;
        gFieldEffectArguments[6] = 40;
        break;
    case DIR_NORTH:
        gFieldEffectArguments[5] = 8;
        gFieldEffectArguments[6] = 8;
        break;
    case DIR_WEST:
        gFieldEffectArguments[5] = -8;
        gFieldEffectArguments[6] = 24;
        break;
    case DIR_EAST:
        gFieldEffectArguments[5] = 24;
        gFieldEffectArguments[6] = 24;
        break;
    }
}

#if DEBUG

void Debug_SetUpFieldMove_SecretPower(void)
{
    u8 metatile;

    CheckPlayerHasSecretBase();

    if (gSpecialVar_Result == 1 || GetPlayerFacingDirection() != DIR_NORTH)
    {
        UnlockPlayerFieldControls();
        return;
    }
    
    GetXYCoordsOneStepInFrontOfPlayer(&gPlayerFacingPosition.x, &gPlayerFacingPosition.y);
    metatile = MapGridGetMetatileBehaviorAt(gPlayerFacingPosition.x, gPlayerFacingPosition.y);
    if (MetatileBehavior_IsSecretBaseCave(metatile) == TRUE)
    {
        SetCurrentSecretBase();
        gLastFieldPokeMenuOpened = 0;
        FieldCallback_SecretBaseCave();
    }
    else if (MetatileBehavior_IsSecretBaseTree(metatile) == TRUE)
    {
        SetCurrentSecretBase();
        gLastFieldPokeMenuOpened = 0;
        FieldCallback_SecretBaseTree();
    }
    else if (MetatileBehavior_IsSecretBaseShrub(metatile) == TRUE)
    {
        SetCurrentSecretBase();
        gLastFieldPokeMenuOpened = 0;
        FieldCallback_SecretBaseShrub();
    }
    else
    {
        UnlockPlayerFieldControls();
    }
}

#endif

bool8 SetUpFieldMove_SecretPower(void)
{
    u8 behavior;

    CheckPlayerHasSecretBase();
    if (gSpecialVar_Result == 1 || GetPlayerFacingDirection() != DIR_NORTH)
        return FALSE;

    GetXYCoordsOneStepInFrontOfPlayer(&gPlayerFacingPosition.x, &gPlayerFacingPosition.y);
    behavior = MapGridGetMetatileBehaviorAt(gPlayerFacingPosition.x, gPlayerFacingPosition.y);

    if (MetatileBehavior_IsSecretBaseCave(behavior) == TRUE)
    {
        SetCurrentSecretBase();
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_SecretBaseCave;
        return TRUE;
    }

    if (MetatileBehavior_IsSecretBaseTree(behavior) == TRUE)
    {
        SetCurrentSecretBase();
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_SecretBaseTree;
        return TRUE;
    }

    if (MetatileBehavior_IsSecretBaseShrub(behavior) == TRUE)
    {
        SetCurrentSecretBase();
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_SecretBaseShrub;
        return TRUE;
    }

    return FALSE;
}

static void FieldCallback_SecretBaseCave(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    ScriptContext_SetupScript(SecretBase_EventScript_CaveUseSecretPower);
}

bool8 FldEff_UseSecretPowerCave(void)
{
    u8 taskId = oei_task_add();

    gTasks[taskId].data[8] = (uintptr_t)StartSecretBaseCaveFieldEffect >> 16;
    gTasks[taskId].data[9] = (uintptr_t)StartSecretBaseCaveFieldEffect;

    return FALSE;
}

static void StartSecretBaseCaveFieldEffect(void)
{
    FieldEffectActiveListRemove(FLDEFF_USE_SECRET_POWER_CAVE);
    FieldEffectStart(FLDEFF_SECRET_POWER_CAVE);
}

bool8 FldEff_SecretPowerCave(void)
{
    AdjustSecretPowerSpritePixelOffsets();
    CreateSprite(
        &sSpriteTemplate_SecretPowerCave,
        gSprites[gPlayerAvatar.spriteId].oam.x + gFieldEffectArguments[5],
        gSprites[gPlayerAvatar.spriteId].oam.y + gFieldEffectArguments[6],
        148);
    return FALSE;
}

static void SpriteCB_CaveEntranceInit(struct Sprite *sprite)
{
    PlaySE(SE_M_ROCK_THROW);
    sprite->data[0] = 0;
    sprite->callback = SpriteCB_CaveEntranceOpen;
}

static void SpriteCB_CaveEntranceOpen(struct Sprite *sprite)
{
    if (sprite->data[0] < 40)
    {
        sprite->data[0]++;
        if (sprite->data[0] == 20)
            ToggleSecretBaseEntranceMetatile();
    }
    else
    {
        sprite->data[0] = 0;
        sprite->callback = SpriteCB_CaveEntranceEnd;
    }
}

static void SpriteCB_CaveEntranceEnd(struct Sprite *sprite)
{
    FieldEffectStop(sprite, FLDEFF_SECRET_POWER_CAVE);
    ScriptContext_Enable();
}

static void FieldCallback_SecretBaseTree(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    ScriptContext_SetupScript(SecretBase_EventScript_TreeUseSecretPower);
}

bool8 FldEff_UseSecretPowerTree(void)
{
    u8 taskId = oei_task_add();

    gTasks[taskId].data[8] = (uintptr_t)StartSecretBaseTreeFieldEffect >> 16;
    gTasks[taskId].data[9] = (uintptr_t)StartSecretBaseTreeFieldEffect;

    return FALSE;
}

static void StartSecretBaseTreeFieldEffect(void)
{
    FieldEffectActiveListRemove(FLDEFF_USE_SECRET_POWER_TREE);
    FieldEffectStart(FLDEFF_SECRET_POWER_TREE);
}

bool8 FldEff_SecretPowerTree(void)
{
    s16 behavior = MapGridGetMetatileBehaviorAt(gPlayerFacingPosition.x, gPlayerFacingPosition.y) & 0xFFF;

    if (behavior == MB_SECRET_BASE_SPOT_TREE_1)
        gFieldEffectArguments[7] = 0;

    if (behavior == MB_SECRET_BASE_SPOT_TREE_2)
        gFieldEffectArguments[7] = 2;

    AdjustSecretPowerSpritePixelOffsets();
    CreateSprite(
        &sSpriteTemplate_SecretPowerTree,
        gSprites[gPlayerAvatar.spriteId].oam.x + gFieldEffectArguments[5],
        gSprites[gPlayerAvatar.spriteId].oam.y + gFieldEffectArguments[6],
        148);

    if (gFieldEffectArguments[7] == 1 || gFieldEffectArguments[7] == 3)
        ToggleSecretBaseEntranceMetatile();

    return FALSE;
}

static void SpriteCB_TreeEntranceInit(struct Sprite *sprite)
{
    PlaySE(SE_M_SCRATCH);
    sprite->animNum = gFieldEffectArguments[7];
    sprite->data[0] = 0;
    sprite->callback = SpriteCB_TreeEntranceOpen;
}

static void SpriteCB_TreeEntranceOpen(struct Sprite *sprite)
{
    sprite->data[0]++;

    if (sprite->data[0] >= 40)
    {
        if (gFieldEffectArguments[7] == 0 || gFieldEffectArguments[7] == 2)
            ToggleSecretBaseEntranceMetatile();

        sprite->data[0] = 0;
        sprite->callback = SpriteCB_TreeEntranceEnd;
    }
}

static void SpriteCB_TreeEntranceEnd(struct Sprite *sprite)
{
    FieldEffectStop(sprite, FLDEFF_SECRET_POWER_TREE);
    ScriptContext_Enable();
}

static void FieldCallback_SecretBaseShrub(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    ScriptContext_SetupScript(SecretBase_EventScript_ShrubUseSecretPower);
}

bool8 FldEff_UseSecretPowerShrub(void)
{
    u8 taskId = oei_task_add();

    gTasks[taskId].data[8] = (uintptr_t)StartSecretBaseShrubFieldEffect >> 16;
    gTasks[taskId].data[9] = (uintptr_t)StartSecretBaseShrubFieldEffect;

    return FALSE;
}

static void StartSecretBaseShrubFieldEffect(void)
{
    FieldEffectActiveListRemove(FLDEFF_USE_SECRET_POWER_SHRUB);
    FieldEffectStart(FLDEFF_SECRET_POWER_SHRUB);
}

bool8 FldEff_SecretPowerShrub(void)
{
    AdjustSecretPowerSpritePixelOffsets();
    CreateSprite(
        &sSpriteTemplate_SecretPowerShrub,
        gSprites[gPlayerAvatar.spriteId].oam.x + gFieldEffectArguments[5],
        gSprites[gPlayerAvatar.spriteId].oam.y + gFieldEffectArguments[6],
        148);
    return FALSE;
}

static void SpriteCB_ShrubEntranceInit(struct Sprite *sprite)
{
    PlaySE(SE_M_POISON_POWDER);
    sprite->data[0] = 0;
    sprite->callback = SpriteCB_ShrubEntranceOpen;
}

static void SpriteCB_ShrubEntranceOpen(struct Sprite *sprite)
{
    if (sprite->data[0] < 40)
    {
        sprite->data[0]++;
        if (sprite->data[0] == 20)
            ToggleSecretBaseEntranceMetatile();
    }
    else
    {
        sprite->data[0] = 0;
        sprite->callback = SpriteCB_ShrubEntranceEnd;
    }
}

static void SpriteCB_ShrubEntranceEnd(struct Sprite *sprite)
{
    FieldEffectStop(sprite, FLDEFF_SECRET_POWER_SHRUB);
    ScriptContext_Enable();
}
