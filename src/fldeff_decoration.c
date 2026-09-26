#include "global.h"
#include "event_data.h"
#include "field_camera.h"
#include "field_effect.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "constants/field_effects.h"
#include "constants/metatile_labels.h"
#include "constants/songs.h"

extern const u8 gSandPillar0_Gfx[];
extern const u8 gSandPillar1_Gfx[];
extern const u8 gSandPillar2_Gfx[];
extern const u16 gTilesetPalettes_SecretBase[][16];

const struct OamData gOamData_SandPillar =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 2,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AnimCmd gSpriteAnim_SandPillar[] =
{
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_SandPillar[] =
{
    gSpriteAnim_SandPillar,
};

const struct SpriteFrameImage gSpriteImageTable_SandPillar[] =
{
    {gSandPillar0_Gfx, 0x100},
    {gSandPillar1_Gfx, 0x100},
    {gSandPillar2_Gfx, 0x100},
};

void SpriteCB_SandPillar_BreakTop(struct Sprite *);
void SpriteCB_SandPillar_BreakBase(struct Sprite *);
void SpriteCB_SandPillar_End(struct Sprite *);
const struct SpriteTemplate gSpriteTemplate_SandPillar =
{
    .tileTag = 0xFFFF,
    .paletteTag = FLDEFF_PAL_TAG_SAND_PILLAR,
    .oam = &gOamData_SandPillar,
    .anims = gSpriteAnimTable_SandPillar,
    .images = gSpriteImageTable_SandPillar,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SandPillar_BreakTop,
};

// This uses one of the secret base palettes, so there is no "09.pal" file.
const struct SpritePalette gSpritePalette_SandPillar = {gTilesetPalettes_SecretBase[5], FLDEFF_PAL_TAG_SAND_PILLAR};

extern const struct SpriteTemplate *const gFieldEffectObjectTemplatePointers[36];

static void Task_PopSecretBaseBalloon(u8);
static void DoBalloonSoundEffect(s16);

void PopSecretBaseBalloon(s16 metatileId, s16 x, s16 y)
{
    u8 taskId = CreateTask(Task_PopSecretBaseBalloon, 0);

    gTasks[taskId].data[0] = metatileId;
    gTasks[taskId].data[1] = x;
    gTasks[taskId].data[2] = y;
    gTasks[taskId].data[3] = 0;
    gTasks[taskId].data[4] = 1;
}

static void Task_PopSecretBaseBalloon(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (data[3]  == 6)
        data[3] = 0;
    else
        data[3]++;

    if (data[3] == 0)
    {
        if (data[4] == 2)
            DoBalloonSoundEffect(data[0]);
        MapGridSetMetatileIdAt(data[1], data[2], data[0] + data[4]);
        CurrentMapDrawMetatileAt(data[1], data[2]);
        if (data[4] == 3)
            DestroyTask(taskId);
        else
            data[4]++;
    }
}

static void DoBalloonSoundEffect(s16 metatileId)
{
    switch (metatileId)
    {
    case METATILE_SecretBase_RedBalloon:
        PlaySE(SE_BALLOON_RED);
        break;
    case METATILE_SecretBase_BlueBalloon:
        PlaySE(SE_BALLOON_BLUE);
        break;
    case METATILE_SecretBase_YellowBalloon:
        PlaySE(SE_BALLOON_YELLOW);
        break;
    case METATILE_SecretBase_MudBall:
        PlaySE(SE_MUD_BALL);
        break;
    }
}

bool8 FldEff_Nop47(void)
{
    return FALSE;
}

bool8 FldEff_Nop48(void)
{
    return FALSE;
}

static void DoSecretBaseBreakableDoorEffect(s16 x, s16 y)
{
    PlaySE(SE_BREAKABLE_DOOR);
    MapGridSetMetatileIdAt(x, y, METATILE_SecretBase_BreakableDoor_BottomOpen);
    MapGridSetMetatileIdAt(x, y - 1, METATILE_SecretBase_BreakableDoor_TopOpen);
    CurrentMapDrawMetatileAt(x, y);
    CurrentMapDrawMetatileAt(x, y - 1);
}

static void Task_ShatterSecretBaseBreakableDoor(u8 taskId)
{
    if (gTasks[taskId].data[0] == 7)
    {
        DoSecretBaseBreakableDoorEffect(gTasks[taskId].data[1], gTasks[taskId].data[2]);
        DestroyTask(taskId);
    }
    else
    {
        gTasks[taskId].data[0]++;
    }
}

void ShatterSecretBaseBreakableDoor(s16 x, s16 y)
{
    u8 dir = GetPlayerFacingDirection();
    if (dir == DIR_SOUTH)
    {
        DoSecretBaseBreakableDoorEffect(x, y);
    }
    else if (dir == DIR_NORTH)
    {
        u8 taskId = CreateTask(Task_ShatterSecretBaseBreakableDoor, 5);
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = x;
        gTasks[taskId].data[2] = y;
    }
}

#define tMetatileID data[0]
static void Task_SecretBaseMusicNoteMatSound(u8 taskId)
{
    if (gTasks[taskId].data[1] == 7)
    {
        switch (gTasks[taskId].tMetatileID)
        {
        case METATILE_SecretBase_NoteMat_C_Low:
            PlaySE(SE_NOTE_C);
            break;
        case METATILE_SecretBase_NoteMat_D:
            PlaySE(SE_NOTE_D);
            break;
        case METATILE_SecretBase_NoteMat_E:
            PlaySE(SE_NOTE_E);
            break;
        case METATILE_SecretBase_NoteMat_F:
            PlaySE(SE_NOTE_F);
            break;
        case METATILE_SecretBase_NoteMat_G:
            PlaySE(SE_NOTE_G);
            break;
        case METATILE_SecretBase_NoteMat_A:
            PlaySE(SE_NOTE_A);
            break;
        case METATILE_SecretBase_NoteMat_B:
            PlaySE(SE_NOTE_B);
            break;
        case METATILE_SecretBase_NoteMat_C_High:
            PlaySE(SE_NOTE_C_HIGH);
            break;
        }

        DestroyTask(taskId);
    }
    else
    {
        gTasks[taskId].data[1]++;
    }
}

void PlaySecretBaseMusicNoteMatSound(s16 metatileId)
{
    u8 taskId = CreateTask(Task_SecretBaseMusicNoteMatSound, 5);
    gTasks[taskId].tMetatileID = metatileId;
    gTasks[taskId].data[1] = 0;
}
#undef tMetatileID

void SpriteCB_GlitterMatSparkle(struct Sprite *sprite)
{
    sprite->data[0]++;
    if (sprite->data[0] == 8)
        PlaySE(SE_M_HEAL_BELL);
    if (sprite->data[0] >= 32)
        DestroySprite(sprite);
}

void DoSecretBaseGlitterMatSparkle(void)
{
    s16 x = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.x;
    s16 y = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y;
    u8 spriteId;

    sub_8060470(&x, &y, 8, 4);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SPARKLE], x, y, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].coordOffsetEnabled = TRUE;
        gSprites[spriteId].oam.priority = 1;
        gSprites[spriteId].oam.paletteNum = 5;
        gSprites[spriteId].callback = SpriteCB_GlitterMatSparkle;
        gSprites[spriteId].data[0] = 0;
    }
}

bool8 FldEff_SandPillar(void)
{
    s16 x, y;

    LockPlayerFieldControls();
    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    gFieldEffectArguments[5] = x;
    gFieldEffectArguments[6] = y;

    switch (GetPlayerFacingDirection())
    {
    case DIR_SOUTH:
        CreateSprite(
            &gSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x + 8,
            gSprites[gPlayerAvatar.spriteId].oam.y + 32,
            0);
        break;
    case DIR_NORTH:
        CreateSprite(
            &gSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x + 8,
            gSprites[gPlayerAvatar.spriteId].oam.y,
            148);
        break;
    case DIR_WEST:
        CreateSprite(
            &gSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x - 8,
            gSprites[gPlayerAvatar.spriteId].oam.y + 16,
            148);
        break;
    case DIR_EAST:
        CreateSprite(
            &gSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x + 24,
            gSprites[gPlayerAvatar.spriteId].oam.y + 16,
            148);
        break;
    }

    return FALSE;
}

void SpriteCB_SandPillar_BreakTop(struct Sprite *sprite)
{
    PlaySE(SE_M_ROCK_THROW);
    if (MapGridGetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1) == METATILE_SecretBase_SandOrnament_TopWall)
        MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1, METATILE_SecretBase_Wall_TopMid | MAPGRID_COLLISION_MASK);
    else
        MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1, METATILE_SecretBase_SandOrnament_BrokenTop);
    MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6], METATILE_SecretBase_Ground);
    CurrentMapDrawMetatileAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1);
    CurrentMapDrawMetatileAt(gFieldEffectArguments[5], gFieldEffectArguments[6]);
    sprite->data[0] = 0;
    sprite->callback = SpriteCB_SandPillar_BreakBase;
}

void SpriteCB_SandPillar_BreakBase(struct Sprite *sprite)
{
    if (sprite->data[0] < 18)
    {
        sprite->data[0]++;
    }
    else
    {
        MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6], METATILE_SecretBase_SandOrnament_BrokenBase | MAPGRID_COLLISION_MASK);
        CurrentMapDrawMetatileAt(gFieldEffectArguments[5], gFieldEffectArguments[6]);
        sprite->data[0] = 0;
        sprite->callback = SpriteCB_SandPillar_End;
    }
}

void SpriteCB_SandPillar_End(struct Sprite *sprite)
{
    FieldEffectStop(sprite, FLDEFF_SAND_PILLAR);
    ScriptContext_Enable();
}

void InteractWithShieldOrTVDecoration(void)
{
    s16 x, y;
    s32 metatileId;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);

    metatileId = MapGridGetMetatileIdAt(x, y);

    switch (metatileId)
    {
    case METATILE_SecretBase_GoldShield_Base1:
        ConvertIntToDecimalStringN(gStringVar1, 100, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(gStringVar2, gSecretBaseText_GoldRank);
        gSpecialVar_Result = 0;
        break;
    case METATILE_SecretBase_SilverShield_Base1:
        ConvertIntToDecimalStringN(gStringVar1, 50, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringCopy(gStringVar2, gSecretBaseText_SilverRank);
        gSpecialVar_Result = 0;
        break;
    case METATILE_SecretBase_TV:
        gSpecialVar_Result = 1;
        break;
    case METATILE_SecretBase_RoundTV:
        gSpecialVar_Result = 2;
        break;
    case METATILE_SecretBase_CuteTV:
        gSpecialVar_Result = 3;
        break;
    }
}
