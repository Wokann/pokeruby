#include "global.h"
#include "event_data.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "fldeff_misc.h"
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
#include "constants/songs.h"


static const struct OamData sOam_SandPillar =
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

static const union AnimCmd sAnim_SandPillar[] =
{
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnimTable_SandPillar[] =
{
    sAnim_SandPillar,
};

static const struct SpriteFrameImage sPicTable_SandPillar[] =
{
    {gSandPillar0_Gfx, 0x100},
    {gSandPillar1_Gfx, 0x100},
    {gSandPillar2_Gfx, 0x100},
};

static void SpriteCB_SandPillar_BreakTop(struct Sprite *);
static void SpriteCB_SandPillar_BreakBase(struct Sprite *);
static void SpriteCB_SandPillar_End(struct Sprite *);
static const struct SpriteTemplate sSpriteTemplate_SandPillar =
{
    .tileTag = 0xFFFF,
    .paletteTag = 4110,
    .oam = &sOam_SandPillar,
    .anims = sAnimTable_SandPillar,
    .images = sPicTable_SandPillar,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SandPillar_BreakTop,
};

// This uses one of the Secret Base tileset palettes.
const struct SpritePalette gSpritePalette_SandPillar = {gTilesetPalettes_SecretBase[5], 0x100E};

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
    case 824:
        PlaySE(SE_BALLOON_RED);
        break;
    case 828:
        PlaySE(SE_BALLOON_BLUE);
        break;
    case 832:
        PlaySE(SE_BALLOON_YELLOW);
        break;
    case 552:
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
    MapGridSetMetatileIdAt(x, y, 630);
    MapGridSetMetatileIdAt(x, y - 1, 622);
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

static void Task_SecretBaseMusicNoteMatSound(u8 taskId)
{
    if (gTasks[taskId].data[1] == 7)
    {
        switch (gTasks[taskId].data[0]) // metatileId
        {
        case 632:
            PlaySE(SE_NOTE_C);
            break;
        case 633:
            PlaySE(SE_NOTE_D);
            break;
        case 634:
            PlaySE(SE_NOTE_E);
            break;
        case 635:
            PlaySE(SE_NOTE_F);
            break;
        case 636:
            PlaySE(SE_NOTE_G);
            break;
        case 637:
            PlaySE(SE_NOTE_A);
            break;
        case 638:
            PlaySE(SE_NOTE_B);
            break;
        case 691:
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
    gTasks[taskId].data[0] = metatileId;
    gTasks[taskId].data[1] = 0;
}

static void SpriteCB_GlitterMatSparkle(struct Sprite *sprite)
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

    SetSpritePosToOffsetMapCoords(&x, &y, 8, 4);
    spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[22], x, y, 0);
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
            &sSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x + 8,
            gSprites[gPlayerAvatar.spriteId].oam.y + 32,
            0);
        break;
    case DIR_NORTH:
        CreateSprite(
            &sSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x + 8,
            gSprites[gPlayerAvatar.spriteId].oam.y,
            148);
        break;
    case DIR_WEST:
        CreateSprite(
            &sSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x - 8,
            gSprites[gPlayerAvatar.spriteId].oam.y + 16,
            148);
        break;
    case DIR_EAST:
        CreateSprite(
            &sSpriteTemplate_SandPillar,
            gSprites[gPlayerAvatar.spriteId].oam.x + 24,
            gSprites[gPlayerAvatar.spriteId].oam.y + 16,
            148);
        break;
    }

    return FALSE;
}

static void SpriteCB_SandPillar_BreakTop(struct Sprite *sprite)
{
    PlaySE(SE_M_ROCK_THROW);
    if (MapGridGetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1) == 646)
        MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1, 3586);
    else
        MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1, 644);
    MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6], 522);
    CurrentMapDrawMetatileAt(gFieldEffectArguments[5], gFieldEffectArguments[6] - 1);
    CurrentMapDrawMetatileAt(gFieldEffectArguments[5], gFieldEffectArguments[6]);
    sprite->data[0] = 0;
    sprite->callback = SpriteCB_SandPillar_BreakBase;
}

static void SpriteCB_SandPillar_BreakBase(struct Sprite *sprite)
{
    if (sprite->data[0] < 18)
    {
        sprite->data[0]++;
    }
    else
    {
        MapGridSetMetatileIdAt(gFieldEffectArguments[5], gFieldEffectArguments[6], 3724);
        CurrentMapDrawMetatileAt(gFieldEffectArguments[5], gFieldEffectArguments[6]);
        sprite->data[0] = 0;
        sprite->callback = SpriteCB_SandPillar_End;
    }
}

static void SpriteCB_SandPillar_End(struct Sprite *sprite)
{
    FieldEffectStop(sprite, FLDEFF_SAND_PILLAR);
    ScriptContext_Enable();
}

void GetShieldToyTVDecorationInfo(void)
{
    s16 x, y;
    s32 metatileId;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);

    metatileId = MapGridGetMetatileIdAt(x, y);

    switch (metatileId)
    {
    case 822:
        ConvertIntToDecimalStringN(gStringVar1, 100, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(gStringVar2, gText_Gold);
        gSpecialVar_Result = 0;
        break;
    case 734:
        ConvertIntToDecimalStringN(gStringVar1, 50, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringCopy(gStringVar2, gText_Silver);
        gSpecialVar_Result = 0;
        break;
    case 756:
        gSpecialVar_Result = 1;
        break;
    case 757:
        gSpecialVar_Result = 2;
        break;
    case 758:
        gSpecialVar_Result = 3;
        break;
    }
}
