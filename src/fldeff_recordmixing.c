#include "global.h"
#include "event_object_movement.h"
#include "fldeff_recordmixing.h"
#include "sprite.h"

extern const struct OamData gFieldOamData_32x8;

static const u8 sRecordMixLights_Gfx0[] = INCBIN_U8("graphics/field_effects/pics/record_mix_lights_0.4bpp");
static const u8 sRecordMixLights_Gfx1[] = INCBIN_U8("graphics/field_effects/pics/record_mix_lights_1.4bpp");
static const u8 sRecordMixLights_Gfx2[] = INCBIN_U8("graphics/field_effects/pics/record_mix_lights_2.4bpp");
static const u16 sRecordMixLights_Pal[] = INCBIN_U16("graphics/field_effects/palettes/record_mix_lights.gbapal");


static const struct SpriteFrameImage sPicTable_RecordMixLights[] =
{
    { sRecordMixLights_Gfx0, sizeof(sRecordMixLights_Gfx0) },
    { sRecordMixLights_Gfx1, sizeof(sRecordMixLights_Gfx1) },
    { sRecordMixLights_Gfx2, sizeof(sRecordMixLights_Gfx2) },
};

static const struct SpritePalette sSpritePalette_RecordMixLights = { sRecordMixLights_Pal, 0x1000 };

static const union AnimCmd sAnim_RecordMixLights[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_FRAME(2, 30),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnimTable_RecordMixLights[] =
{
    sAnim_RecordMixLights,
};

static const struct SpriteTemplate sSpriteTemplate_RecordMixLights =
{
    .tileTag = 0xFFFF,
    .paletteTag = 0x1000,
    .oam = &gFieldOamData_32x8,
    .anims = sAnimTable_RecordMixLights,
    .images = sPicTable_RecordMixLights,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

u8 CreateRecordMixingLights(void)
{
    u8 spriteId;

    LoadSpritePalette(&sSpritePalette_RecordMixLights);

    spriteId = CreateSprite(&sSpriteTemplate_RecordMixLights, 0, 0, 82);

    if (spriteId == MAX_SPRITES)
    {
        return MAX_SPRITES;
    }
    else
    {
        struct Sprite *sprite = &gSprites[spriteId];
        GetMapCoordsFromSpritePos(16, 13, &sprite->x, &sprite->y);
        sprite->coordOffsetEnabled = TRUE;
        sprite->x += 16;
        sprite->y += 2;
    }

    return spriteId;
}

void DestroyRecordMixingLights(void)
{
    int i;

    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].template == &sSpriteTemplate_RecordMixLights)
        {
            FreeSpritePalette(&gSprites[i]);
            DestroySprite(&gSprites[i]);
        }
    }
}
