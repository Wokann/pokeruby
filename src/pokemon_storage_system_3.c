
// Includes
#include "global.h"
#include "constants/species.h"
#include "sprite.h"
#include "trig.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"

// Static type declarations

enum {
    RELEASE_ANIM_RELEASE,
    RELEASE_ANIM_CAME_BACK,
};

// Static RAM declarations

// Static ROM declarations

static void SpriteCB_BoxMonIconScrollOut(struct Sprite *sprite);
static void MovePartySpriteToNextSlot(struct Sprite *sprite, u16 partyId);
static void SpriteCB_MovePartyMonToNextSlot(struct Sprite *sprite);
static void SpriteCB_HeldMon(struct Sprite *sprite);
static struct Sprite *CreateMonIconSprite(u16 species, u32 personality, s16 x, s16 y, u8 oamPriority, u8 subpriority);
static void DestroyBoxMonIcon(struct Sprite *sprite);

// .rodata

static const struct OamData sOamData_MonIcon;
static const struct SpriteTemplate sSpriteTemplate_MonIcon = {
    0x000f,
    0xdac0,
    &sOamData_MonIcon,
    gDummySpriteAnimTable,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const struct OamData sOamData_MonIcon = {
    .size = 2
};

static const union AffineAnimCmd sAffineAnim_ReleaseMon_Release[] = {
    AFFINEANIMCMD_FRAME(-2, -2, 0, 120),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_ReleaseMon_CameBack[] = {
    AFFINEANIMCMD_FRAME(16, 16, 0,  0),
    AFFINEANIMCMD_FRAME(16, 16, 0, 15),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_ReleaseMon[] = {
    [RELEASE_ANIM_RELEASE] = sAffineAnim_ReleaseMon_Release,
    [RELEASE_ANIM_CAME_BACK] = sAffineAnim_ReleaseMon_CameBack
};

// .text

u8 StorageGetCurrentBox(void)
{
    return gPokemonStorage.currentBox;
}

void InitMonIconFields(void)
{
    u16 i;

    LoadMonIconPalettes();
    for (i = 0; i < MAX_MON_ICONS; i++)
        gPokemonStorageSystemPtr->numIconsPerSpecies[i] = 0;
    for (i = 0; i < MAX_MON_ICONS; i++)
        gPokemonStorageSystemPtr->iconSpeciesList[i] = SPECIES_NONE;
    for (i = 0; i < PARTY_SIZE; i++)
        gPokemonStorageSystemPtr->partySprites[i] = NULL;
    for (i = 0; i < IN_BOX_COUNT; i++)
        gPokemonStorageSystemPtr->boxMonsSprites[i] = NULL;
    gPokemonStorageSystemPtr->movingMonSprite = NULL;
    gPokemonStorageSystemPtr->unkUnused1 = 0;
}

void CreateMovingMonIcon(void)
{
    u32 personality = GetMonData(&gPokemonStorageSystemPtr->movingMon, MON_DATA_PERSONALITY);
    gPokemonStorageSystemPtr->movingMonSprite = CreateMonIconSprite(GetMonData(&gPokemonStorageSystemPtr->movingMon, MON_DATA_SPECIES2), personality, 0, 0, 1, 7);
    gPokemonStorageSystemPtr->movingMonSprite->callback = SpriteCB_HeldMon;
}

void InitBoxMonSprites(u8 boxId)
{
    struct BoxPokemon *box = gPokemonStorage.boxes[boxId];
    u16 i;
    u16 k = 0;
    for (i = 0; i < 5; i++)
    {
        u16 j;
        for (j = 0; j < 6; j++)
        {
            u16 species = GetBoxMonData(box, MON_DATA_SPECIES2);
            if (species != SPECIES_NONE)
                gPokemonStorageSystemPtr->boxMonsSprites[k] = CreateMonIconSprite(species, GetBoxMonData(box, MON_DATA_PERSONALITY), 24 * j + 0x64, 24 * i + 0x2c, 2, 18 - j);
            else
                gPokemonStorageSystemPtr->boxMonsSprites[k] = NULL;
            box++;
            k++;
        }
    }
}

void CreateBoxMonIconAtPos(u8 boxPosition)
{
    struct BoxPokemon *mon = gPokemonStorage.boxes[StorageGetCurrentBox()] + boxPosition;
    u16 species = GetBoxMonData(mon, MON_DATA_SPECIES2);
    if (species != SPECIES_NONE)
    {
        s16 x = 24 * (boxPosition % 6) + 0x64;
        s16 y = 24 * (boxPosition / 6) + 0x2c;
        gPokemonStorageSystemPtr->boxMonsSprites[boxPosition] = CreateMonIconSprite(species, GetBoxMonData(mon, MON_DATA_PERSONALITY), x, y, 2, 18 - (boxPosition % 6));
    }
}

#define sDistance      data[1]
#define sSpeed         data[2]
#define sScrollInDestX data[3]
#define sDelay         data[4]
#define sScrollOutX    data[5]

static void StartBoxMonIconsScrollOut(s16 speed)
{
    u16 boxPosition;

    for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
    {
        if (gPokemonStorageSystemPtr->boxMonsSprites[boxPosition])
        {
            gPokemonStorageSystemPtr->boxMonsSprites[boxPosition]->sSpeed = speed;
            gPokemonStorageSystemPtr->boxMonsSprites[boxPosition]->sDelay = 1;
            gPokemonStorageSystemPtr->boxMonsSprites[boxPosition]->callback = SpriteCB_BoxMonIconScrollOut;
        }
    }
}

static void SpriteCB_BoxMonIconScrollIn(struct Sprite *sprite)
{
    if (sprite->sDistance != 0)
    {
        sprite->sDistance--;
        sprite->x += sprite->sSpeed;
    }
    else
    {
        gPokemonStorageSystemPtr->iconScrollNumIncoming--;
        sprite->x = sprite->sScrollInDestX;
        sprite->callback = SpriteCallbackDummy;
    }
}

static void SpriteCB_BoxMonIconScrollOut(struct Sprite *sprite)
{
    if (sprite->sDelay != 0)
    {
        sprite->sDelay--;
    }
    else
    {
        sprite->x += sprite->sSpeed;
        sprite->sScrollOutX = sprite->x + sprite->x2;
        if (sprite->sScrollOutX < 0x45 || sprite->sScrollOutX > 0xfb)
            sprite->callback = SpriteCallbackDummy;
    }
}

static void DestroyBoxMonIconsInColumn(u8 column)
{
    u16 row;
    u8 boxPosition = column;

    for (row = 0; row < 5; row++)
    {
        if (gPokemonStorageSystemPtr->boxMonsSprites[boxPosition])
        {
            DestroyBoxMonIcon(gPokemonStorageSystemPtr->boxMonsSprites[boxPosition]);
            gPokemonStorageSystemPtr->boxMonsSprites[boxPosition] = NULL;
        }
        boxPosition += 6;
    }
}

static u8 CreateBoxMonIconsInColumn(u8 column, u16 distance, s16 speed)
{
    u16 row;
    u16 xDest;
    u16 y;
    u8 iconsCreated;
    u8 subpriority;
    u16 curX;

    y = 0x2c;
    xDest = 24 * column + 0x64;
    curX = xDest - (distance + 1) * speed;
    subpriority = 18 - column;
    iconsCreated = 0;

    for (row = 0; row < 5; row++)
    {
        u16 species = GetBoxMonData(gPokemonStorage.boxes[gPokemonStorageSystemPtr->iconScrollToBoxId] + column, MON_DATA_SPECIES2);
        if (species != SPECIES_NONE)
        {
            gPokemonStorageSystemPtr->boxMonsSprites[column] = CreateMonIconSprite(species, GetBoxMonData(gPokemonStorage.boxes[gPokemonStorageSystemPtr->iconScrollToBoxId] + column, MON_DATA_PERSONALITY), curX, y, 2, subpriority);
            if (gPokemonStorageSystemPtr->boxMonsSprites[column])
            {
                gPokemonStorageSystemPtr->boxMonsSprites[column]->sDistance = distance;
                gPokemonStorageSystemPtr->boxMonsSprites[column]->sSpeed = speed;
                gPokemonStorageSystemPtr->boxMonsSprites[column]->sScrollInDestX = xDest;
                gPokemonStorageSystemPtr->boxMonsSprites[column]->callback = SpriteCB_BoxMonIconScrollIn;
                iconsCreated++;
            }
        }
        column += 6;
        y += 24;
    }
    return iconsCreated;
}

void InitBoxMonIconScroll(u8 boxId, s8 direction)
{
    gPokemonStorageSystemPtr->iconScrollState = 0;
    gPokemonStorageSystemPtr->iconScrollToBoxId = boxId;
    gPokemonStorageSystemPtr->iconScrollDirection = direction;
    gPokemonStorageSystemPtr->iconScrollDistance = 32;
    gPokemonStorageSystemPtr->iconScrollSpeed = -6 * direction;
    gPokemonStorageSystemPtr->iconScrollNumIncoming = 0;
    if (direction > 0)
        gPokemonStorageSystemPtr->iconScrollCurColumn = 0;
    else
        gPokemonStorageSystemPtr->iconScrollCurColumn = 5;
    gPokemonStorageSystemPtr->iconScrollPos = 24 * gPokemonStorageSystemPtr->iconScrollCurColumn + 0x64;
    StartBoxMonIconsScrollOut(gPokemonStorageSystemPtr->iconScrollSpeed);
}

bool8 UpdateBoxMonIconScroll(void)
{
    if (gPokemonStorageSystemPtr->iconScrollDistance)
        gPokemonStorageSystemPtr->iconScrollDistance--;
    switch (gPokemonStorageSystemPtr->iconScrollState)
    {
        case 0:
            gPokemonStorageSystemPtr->iconScrollPos += gPokemonStorageSystemPtr->iconScrollSpeed;
            if (gPokemonStorageSystemPtr->iconScrollPos < 0x41 || gPokemonStorageSystemPtr->iconScrollPos > 0xfb)
            {
                DestroyBoxMonIconsInColumn(gPokemonStorageSystemPtr->iconScrollCurColumn);
                gPokemonStorageSystemPtr->iconScrollPos += 24 * gPokemonStorageSystemPtr->iconScrollDirection;
                gPokemonStorageSystemPtr->iconScrollState++;
            }
            break;
        case 1:
            gPokemonStorageSystemPtr->iconScrollPos += gPokemonStorageSystemPtr->iconScrollSpeed;
            gPokemonStorageSystemPtr->iconScrollNumIncoming += CreateBoxMonIconsInColumn(gPokemonStorageSystemPtr->iconScrollCurColumn, gPokemonStorageSystemPtr->iconScrollDistance, gPokemonStorageSystemPtr->iconScrollSpeed);
            if ((gPokemonStorageSystemPtr->iconScrollDirection > 0 && gPokemonStorageSystemPtr->iconScrollCurColumn == 5) || (gPokemonStorageSystemPtr->iconScrollDirection < 0 && gPokemonStorageSystemPtr->iconScrollCurColumn == 0))
            {
                gPokemonStorageSystemPtr->iconScrollState++;
            }
            else
            {
                gPokemonStorageSystemPtr->iconScrollCurColumn += gPokemonStorageSystemPtr->iconScrollDirection;
                gPokemonStorageSystemPtr->iconScrollState = 0;
            }
            break;
        case 2:
            if (gPokemonStorageSystemPtr->iconScrollNumIncoming == 0)
            {
                gPokemonStorageSystemPtr->iconScrollDistance++;
                return FALSE;
            }
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

#undef sDistance
#undef sSpeed
#undef sScrollInDestX
#undef sDelay
#undef sScrollOutX

void CreatePartyMonsSprites(bool8 visible)
{
    u16 count;
    u16 i;
    u16 species = GetMonData(gPlayerParty + 0, MON_DATA_SPECIES2);
    u32 personality = GetMonData(gPlayerParty + 0, MON_DATA_PERSONALITY);
    gPokemonStorageSystemPtr->partySprites[0] = CreateMonIconSprite(species, personality, 0x68, 0x40, 1, 11);
    count = 1;
    for (i = 1; i < PARTY_SIZE; i++)
    {
        species = GetMonData(gPlayerParty + i, MON_DATA_SPECIES2);
        if (species != SPECIES_NONE)
        {
            personality = GetMonData(gPlayerParty + i, MON_DATA_PERSONALITY);
            gPokemonStorageSystemPtr->partySprites[i] = CreateMonIconSprite(species, personality, 0x98, (i - 1) * 24 + 0x10, 1, 11);
            count++;
        }
        else
        {
            gPokemonStorageSystemPtr->partySprites[i] = NULL;
        }
    }
    if (!visible)
    {
        for (i = 0; i < count; i++)
        {
            // this routine assumes CompactPartySlots has been called
            gPokemonStorageSystemPtr->partySprites[i]->y -= 0xa0;
            gPokemonStorageSystemPtr->partySprites[i]->invisible = TRUE;
        }
    }
}

void CompactPartySprites(void)
{
    u16 i;
    u16 count;

    gPokemonStorageSystemPtr->numPartyToCompact = 0;
    for (i = 0, count = 0; i < PARTY_SIZE; i++)
    {
        if (gPokemonStorageSystemPtr->partySprites[i])
        {
            if (i != count)
            {
                MovePartySpriteToNextSlot(gPokemonStorageSystemPtr->partySprites[i], count);
                gPokemonStorageSystemPtr->partySprites[i] = NULL;
                gPokemonStorageSystemPtr->numPartyToCompact++;
            }
            count++;
        }
    }
}

u8 GetNumPartySpritesCompacting(void)
{
    return gPokemonStorageSystemPtr->numPartyToCompact;
}

#define sPartyId   data[1]
#define sMonX      data[2]
#define sMonY      data[3]
#define sSpeedX    data[4]
#define sSpeedY    data[5]
#define sMoveSteps data[6]

static void MovePartySpriteToNextSlot(struct Sprite *sprite, u16 partyId)
{
    s16 x;
    s16 y;

    sprite->sPartyId = partyId;
    if (partyId == 0)
    {
        x = 0x68;
        y = 0x40;
    }
    else
    {
        x = 0x98;
        y = 24 * (partyId - 1) + 0x10;
    }
    sprite->sMonX = sprite->x << 3;
    sprite->sMonY = sprite->y << 3;
    sprite->sSpeedX = (x * 8 - sprite->sMonX) / 8;
    sprite->sSpeedY = (y * 8 - sprite->sMonY) / 8;
    sprite->sMoveSteps = 8;
    sprite->callback = SpriteCB_MovePartyMonToNextSlot;
}

static void SpriteCB_MovePartyMonToNextSlot(struct Sprite *sprite)
{
    if (sprite->sMoveSteps)
    {
        sprite->sMonX += sprite->sSpeedX;
        sprite->sMonY += sprite->sSpeedY;
        sprite->x = sprite->sMonX >> 3;
        sprite->y = sprite->sMonY >> 3;
        sprite->sMoveSteps--;
    }
    else
    {
        if (sprite->sPartyId == 0)
        {
            sprite->x = 0x68;
            sprite->y = 0x40;
        }
        else
        {
            sprite->x = 0x98;
            sprite->y = (sprite->sPartyId - 1) * 24 + 0x10;
        }
        sprite->callback = SpriteCallbackDummy;
        gPokemonStorageSystemPtr->partySprites[sprite->sPartyId] = sprite;
        gPokemonStorageSystemPtr->numPartyToCompact--;
    }
}

#undef sPartyId
#undef sMonX
#undef sMonY
#undef sSpeedX
#undef sSpeedY
#undef sMoveSteps

void DestroyMovingMonIcon(void)
{
    if (gPokemonStorageSystemPtr->movingMonSprite)
    {
        DestroyBoxMonIcon(gPokemonStorageSystemPtr->movingMonSprite);
        gPokemonStorageSystemPtr->movingMonSprite = NULL;
    }
}

void MovePartySprites(s16 yDelta)
{
    u16 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (gPokemonStorageSystemPtr->partySprites[i])
        {
            s16 yy;
            gPokemonStorageSystemPtr->partySprites[i]->y += yDelta;
            yy = gPokemonStorageSystemPtr->partySprites[i]->y + gPokemonStorageSystemPtr->partySprites[i]->y2 + gPokemonStorageSystemPtr->partySprites[i]->centerToCornerVecY;
            if (yy < -0x10 || yy > 0xb0)
                gPokemonStorageSystemPtr->partySprites[i]->invisible = TRUE;
            else
                gPokemonStorageSystemPtr->partySprites[i]->invisible = FALSE;
        }
    }
}

void DestroyPartyMonIcon(u8 partyId)
{
    if (gPokemonStorageSystemPtr->partySprites[partyId])
    {
        DestroyBoxMonIcon(gPokemonStorageSystemPtr->partySprites[partyId]);
        gPokemonStorageSystemPtr->partySprites[partyId] = NULL;
    }
}

void DestroyAllPartyMonIcons(void)
{
    u16 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (gPokemonStorageSystemPtr->partySprites[i])
        {
            DestroyBoxMonIcon(gPokemonStorageSystemPtr->partySprites[i]);
            gPokemonStorageSystemPtr->partySprites[i] = NULL;
        }
    }
}

void SetMovingMonSprite(u8 mode, u8 id)
{
    if (mode == 0)
    {
        gPokemonStorageSystemPtr->movingMonSprite = gPokemonStorageSystemPtr->partySprites[id];
        gPokemonStorageSystemPtr->partySprites[id] = NULL;
    }
    else if (mode == 1)
    {
        gPokemonStorageSystemPtr->movingMonSprite = gPokemonStorageSystemPtr->boxMonsSprites[id];
        gPokemonStorageSystemPtr->boxMonsSprites[id] = NULL;
    }
    else
    {
        return;
    }
    gPokemonStorageSystemPtr->movingMonSprite->callback = SpriteCB_HeldMon;
    gPokemonStorageSystemPtr->movingMonSprite->oam.priority = 1;
    gPokemonStorageSystemPtr->movingMonSprite->subpriority = 7;
}

void SetPlacedMonSprite(u8 boxId, u8 position)
{
    if (boxId == TOTAL_BOXES_COUNT)
    {
        gPokemonStorageSystemPtr->partySprites[position] = gPokemonStorageSystemPtr->movingMonSprite;
        gPokemonStorageSystemPtr->partySprites[position]->oam.priority = 1;
        gPokemonStorageSystemPtr->partySprites[position]->subpriority = 11;
    }
    else
    {
        gPokemonStorageSystemPtr->boxMonsSprites[position] = gPokemonStorageSystemPtr->movingMonSprite;
        gPokemonStorageSystemPtr->boxMonsSprites[position]->oam.priority = 2;
        gPokemonStorageSystemPtr->boxMonsSprites[position]->subpriority = 18 - (position % 6);
    }
    gPokemonStorageSystemPtr->movingMonSprite->callback = SpriteCallbackDummy;
    gPokemonStorageSystemPtr->movingMonSprite = NULL;
}

void SaveMonSpriteAtPos(u8 boxId, u8 position)
{
    if (boxId == TOTAL_BOXES_COUNT)
        gPokemonStorageSystemPtr->shiftMonSpritePtr = gPokemonStorageSystemPtr->partySprites + position;
    else
        gPokemonStorageSystemPtr->shiftMonSpritePtr = gPokemonStorageSystemPtr->boxMonsSprites + position;
    gPokemonStorageSystemPtr->movingMonSprite->callback = SpriteCallbackDummy;
    gPokemonStorageSystemPtr->shiftTimer = 0;
}

bool8 MoveShiftingMons(void)
{
    if (gPokemonStorageSystemPtr->shiftTimer == 16)
        return FALSE;
    gPokemonStorageSystemPtr->shiftTimer++;
    if (gPokemonStorageSystemPtr->shiftTimer & 1)
    {
        (*gPokemonStorageSystemPtr->shiftMonSpritePtr)->y--;
        gPokemonStorageSystemPtr->movingMonSprite->y++;
    }
    (*gPokemonStorageSystemPtr->shiftMonSpritePtr)->x2 = gSineTable[gPokemonStorageSystemPtr->shiftTimer * 8] / 16;
    gPokemonStorageSystemPtr->movingMonSprite->x2 = -(gSineTable[gPokemonStorageSystemPtr->shiftTimer * 8] / 16);
    if (gPokemonStorageSystemPtr->shiftTimer == 8)
    {
        gPokemonStorageSystemPtr->movingMonSprite->oam.priority = (*gPokemonStorageSystemPtr->shiftMonSpritePtr)->oam.priority;
        gPokemonStorageSystemPtr->movingMonSprite->subpriority = (*gPokemonStorageSystemPtr->shiftMonSpritePtr)->subpriority;
        (*gPokemonStorageSystemPtr->shiftMonSpritePtr)->oam.priority = 1;
        (*gPokemonStorageSystemPtr->shiftMonSpritePtr)->subpriority = 7;
    }
    if (gPokemonStorageSystemPtr->shiftTimer == 16)
    {
        struct Sprite *sprite = gPokemonStorageSystemPtr->movingMonSprite;
        gPokemonStorageSystemPtr->movingMonSprite = *(gPokemonStorageSystemPtr->shiftMonSpritePtr);
        (*gPokemonStorageSystemPtr->shiftMonSpritePtr) = sprite;
        gPokemonStorageSystemPtr->movingMonSprite->callback = SpriteCB_HeldMon;
        (*gPokemonStorageSystemPtr->shiftMonSpritePtr)->callback = SpriteCallbackDummy;
    }
    return TRUE;
}

void SetReleaseMon(u8 mode, u8 position)
{
    switch (mode)
    {
        case 0:
            gPokemonStorageSystemPtr->releaseMonSpritePtr = gPokemonStorageSystemPtr->partySprites + position;
            break;
        case 1:
            gPokemonStorageSystemPtr->releaseMonSpritePtr = gPokemonStorageSystemPtr->boxMonsSprites + position;
            break;
        case 2:
            gPokemonStorageSystemPtr->releaseMonSpritePtr = &gPokemonStorageSystemPtr->movingMonSprite;
            break;
        default:
            return;
    }
    if (*gPokemonStorageSystemPtr->releaseMonSpritePtr)
    {
        InitSpriteAffineAnim(*gPokemonStorageSystemPtr->releaseMonSpritePtr);
        (*gPokemonStorageSystemPtr->releaseMonSpritePtr)->oam.affineMode = ST_OAM_AFFINE_NORMAL;
        (*gPokemonStorageSystemPtr->releaseMonSpritePtr)->affineAnims = sAffineAnims_ReleaseMon;
        StartSpriteAffineAnim(*gPokemonStorageSystemPtr->releaseMonSpritePtr, RELEASE_ANIM_RELEASE);
    }
}

bool8 TryHideReleaseMonSprite(void)
{
    if (*gPokemonStorageSystemPtr->releaseMonSpritePtr == NULL || (*gPokemonStorageSystemPtr->releaseMonSpritePtr)->invisible)
        return FALSE;
    if ((*gPokemonStorageSystemPtr->releaseMonSpritePtr)->affineAnimEnded)
    {
        (*gPokemonStorageSystemPtr->releaseMonSpritePtr)->invisible = TRUE;
    }
    return TRUE;
}

void DestroyReleaseMonIcon(void)
{
    if (*gPokemonStorageSystemPtr->releaseMonSpritePtr)
    {
        FreeOamMatrix((*gPokemonStorageSystemPtr->releaseMonSpritePtr)->oam.matrixNum);
        DestroyBoxMonIcon(*gPokemonStorageSystemPtr->releaseMonSpritePtr);
        *gPokemonStorageSystemPtr->releaseMonSpritePtr = NULL;
    }
}

void ReshowReleaseMon(void)
{
    if (*gPokemonStorageSystemPtr->releaseMonSpritePtr)
    {
        (*gPokemonStorageSystemPtr->releaseMonSpritePtr)->invisible = FALSE;
        StartSpriteAffineAnim(*gPokemonStorageSystemPtr->releaseMonSpritePtr, RELEASE_ANIM_CAME_BACK);
    }
}

bool8 ResetReleaseMonSpritePtr(void)
{
    if (gPokemonStorageSystemPtr->releaseMonSpritePtr == NULL)
        return FALSE;
    if ((*gPokemonStorageSystemPtr->releaseMonSpritePtr)->affineAnimEnded)
        gPokemonStorageSystemPtr->releaseMonSpritePtr = NULL;
    return TRUE;
}

static void SpriteCB_HeldMon(struct Sprite *sprite)
{
    sprite->x = gPokemonStorageSystemPtr->cursorSprite->x;
    sprite->y = gPokemonStorageSystemPtr->cursorSprite->y + gPokemonStorageSystemPtr->cursorSprite->y2 + 4;
}

static u16 TryLoadMonIconTiles(u16 species)
{
    u16 i;
    u16 retval;

    for (i = 0; i < MAX_MON_ICONS; i++)
    {
        if (gPokemonStorageSystemPtr->iconSpeciesList[i] == species)
            break;
    }
    if (i == MAX_MON_ICONS)
    {
        for (i = 0; i < MAX_MON_ICONS; i++)
        {
            if (gPokemonStorageSystemPtr->iconSpeciesList[i] == 0)
                break;
        }
    }
    if (i != MAX_MON_ICONS)
    {
        gPokemonStorageSystemPtr->iconSpeciesList[i] = species;
        gPokemonStorageSystemPtr->numIconsPerSpecies[i]++;
        retval = i * 16;
        CpuCopy32(gMonIconTable[species], BG_CHAR_ADDR(4) + 32 * retval, 0x200);
        return retval;
    }
    return 0xFFFF;
}

static void RemoveSpeciesFromIconList(u16 species)
{
    u16 i;

    for (i = 0; i < MAX_MON_ICONS; i++)
    {
        if (gPokemonStorageSystemPtr->iconSpeciesList[i] == species)
        {
            if (--gPokemonStorageSystemPtr->numIconsPerSpecies[i] == 0)
                gPokemonStorageSystemPtr->iconSpeciesList[i] = SPECIES_NONE;
            break;
        }
    }
}

static struct Sprite *CreateMonIconSprite(u16 species, u32 personality, s16 x, s16 y, u8 oamPriority, u8 subpriority)
{
    struct SpriteTemplate template = sSpriteTemplate_MonIcon;
    u16 tileNum;
    u8 spriteId;

    species = GetIconSpecies(species, personality);
    template.paletteTag = 0xdac0 + gMonIconPaletteIndices[species];
    tileNum = TryLoadMonIconTiles(species);
    if (tileNum == 0xFFFF)
        return NULL;
    spriteId = CreateSprite(&template, x, y, subpriority);
    if (spriteId == MAX_SPRITES)
    {
        RemoveSpeciesFromIconList(species);
        return NULL;
    }
    gSprites[spriteId].oam.tileNum = tileNum;
    gSprites[spriteId].oam.priority = oamPriority;
    gSprites[spriteId].data[0] = species;
    return gSprites + spriteId;
}

static void DestroyBoxMonIcon(struct Sprite *sprite)
{
    RemoveSpeciesFromIconList(sprite->data[0]);
    DestroySprite(sprite);
}
