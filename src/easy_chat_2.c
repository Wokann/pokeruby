#include "global.h"
#include "constants/easy_chat.h"
#include "constants/event_objects.h"
#include "data2.h"
#include "decompress.h"
#include "easy_chat.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_message_box.h"
#include "graphics.h"
#include "menu.h"
#include "palette.h"
#include "pokedex.h"
#include "random.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "strings2.h"
#include "trig.h"
#include "scanline_effect.h"

#include "data/text/easy_chat/group_words.h"
#include "data/text/easy_chat/words_by_letter.h"
#include "data/text/easy_chat/group_sizes.h"
#include "data/text/easy_chat/group_orders.h"
#include "data/text/easy_chat/group_name_table.h"

extern void CopyTilemapRect();
extern void CopyTilemapRectFixedWidth(void *, u16, u16, const void *, u16, u16, u16, u16);

void CreateMainCursorSprite(void);
void SpriteCB_Cursor(struct Sprite *);
void UpdateRectangleCursorPos(void);
void SpriteCB_RectangleCursorPrimary(struct Sprite *);
void SpriteCB_RectangleCursorMiddle(struct Sprite *);
void SpriteCB_RectangleCursorEnd(struct Sprite *);
void CreateWordSelectCursorSprite(void);
void SpriteCB_WordSelectCursor(struct Sprite *);
void CreateScrollIndicatorSprites(void);
void SpriteCB_ScrollIndicator(struct Sprite *);
void InitEasyChatFramePaletteBlend(void);
void SetEasyChatMessageId(u8);
void PrintEasyChatStdMessage(u8);
void BufferFrameTilemap(void);
void PrintCurrentPhrase(void);
void PrintCurrentPhraseWords(u16, u16);
void PrintCurrentPhraseAsTextAt(u16, u16);
void ResetLowerWindowScroll(void);
void PrintEasyChatTextAt(const u8 *, u16, u16);
void BufferEasyChatGroupNamesRow(u8 *, u16);
void ClearLowerWindow(void);
void ClearWordSelectRow(u16);
bool8 UpdateLowerWindowAnim(void);
void InitLowerWindowExpandHorizontalAnim(void);
void InitLowerWindowCollapseHorizontalAnim(void);
void InitKeyboardExpandVerticalAnim(void);
void InitKeyboardCollapseVerticalAnim(void);
void InitKeyboardRightPanelExpandAnim(void);
void InitKeyboardRightPanelCollapseAnim(void);
void InitWordSelectRevealAnim(void);
void InitWordSelectRevealFinishAnim(void);
void InitWordSelectHideAnim(void);
void InitKeyboardRestoreAfterWordSelectAnim(void);
void InitWordSelectCollapseVerticalAnim(void);
void InitLowerWindowAnimRect(u16 *, u16, u16, u16, u16, u16, u16, u16, u16);
void UpdateLowerWindowAnimRect(u16 *);
void InitLowerWindowAnimSource(struct EasyChatWindowAnimSource *, u16, u16, u16, u16, void *);
void UpdateLowerWindowAnimSource(struct EasyChatWindowAnimSource *);
void CopyLowerWindowAnimRectToVram(void *, u16 *, struct EasyChatWindowAnimSource *);
u8 *CopyEasyChatGroupName(u8 *, u8, int);
u8 *CopyEasyChatWordPadded(u8 *, u16, u16);
u16 GetEasyChatWordStringLength(u16 easyChatWord);
bool8 IsEasyChatWordInvalid(u16 easyChatWord);
static bool8 IsTrendySayingUnlocked(u8);
static u8 GetNumTrendySayingsUnlocked(void);
static u16 GetRandomUnlockedTrendySaying(void);
u8 EasyChatIsNationalPokedexEnabled(void);
static u16 GetRandomUnlockedEasyChatPokemon(void);

static EWRAM_DATA u8 sRectangleCursorAnim = 0;

static u8 sEasyChatMessageId;

static const u16 sTriangleCursor_Pal[] = INCBIN_U16("graphics/misc/interview_pal0.gbapal");
static const u16 sRectangleCursor_Pal[] = INCBIN_U16("graphics/misc/interview_pal1.gbapal");
static const u8 sScrollIndicator_Gfx[] = INCBIN_U8("graphics/misc/interview_arrow.4bpp");
static const u8 sStartSelectButtons_Gfx[] = INCBIN_U8("graphics/misc/interview_buttons.4bpp");
static const u16 sRSInterviewFrame_Pal[] = INCBIN_U16("graphics/misc/interview_frame.gbapal");
static const u8 sRSInterviewFrame_Gfx[] = INCBIN_U8("graphics/misc/interview_frame.4bpp.lz");
static const u8 sTriangleCursor_Gfx[] = INCBIN_U8("graphics/misc/interview_triangle_cursor.4bpp");

void InitEasyChatSprites(void)
{
    struct SpriteSheet interviewSpriteSheets[] =
    {
        {sTriangleCursor_Gfx, 0x20, 0},
        {gInterviewOutlineCursorTiles, 0x700, 1},
        {sScrollIndicator_Gfx, 0x100, 2},
        {sStartSelectButtons_Gfx, 0x100, 3},
        {NULL, 0, 0},
    };
    struct SpritePalette interviewSpritePalettes[] =
    {
        {sTriangleCursor_Pal, 0},
        {sRectangleCursor_Pal, 1},
        {NULL, 0},
    };

    LoadSpriteSheets(interviewSpriteSheets);
    LoadSpritePalettes(interviewSpritePalettes);
    CreateMainCursorSprite();
    CreateWordSelectCursorSprite();
    CreateScrollIndicatorSprites();
}

static const struct OamData sOamData_TriangleCursor =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct SpriteTemplate sSpriteTemplate_TriangleCursor =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &sOamData_TriangleCursor,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Cursor,
};

void CreateMainCursorSprite(void)
{
    u8 spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_TriangleCursor, 0, 0, 0);
    gSprites[spriteId].y = gEasyChatScreen->phraseFrameTop * 8 + 8;
    gSprites[spriteId].x = gEasyChatScreen->phraseFrameLeft * 8 + 4;
    gSprites[spriteId].data[0] = 0;
    gSprites[spriteId].data[1] = 0;
    gEasyChatScreen->mainCursorSprite = &gSprites[spriteId];
}

void SpriteCB_Cursor(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        if (gEasyChatScreen->mainCursorMoved)
        {
            u16 row = gEasyChatScreen->mainCursorRow;
            u16 column = gEasyChatScreen->mainCursorColumn;
            u16 wordSpacing = 0;
            u16 i;

            for (i = 0; i < column; i++)
                wordSpacing += gEasyChatScreen->phraseWordSpacing[row][i];

            if (row == gEasyChatScreen->numRows)
            {
                sprite->y = 96;
                sprite->x = (column * 7  + 3) * 8 + 4;
            }
            else
            {
                sprite->y = (gEasyChatScreen->phraseFrameTop + row * 2) * 8 + 8;
                sprite->x = (gEasyChatScreen->phraseFrameLeft + wordSpacing + column * 11) * 8 + 4;
            }

            sprite->x2 = -6;
            sprite->data[0] = 0;
        }
        else
        {
            if (++sprite->data[0] > 2)
            {
                sprite->data[0] = 0;
                if (++sprite->x2 > 0)
                    sprite->x2 = -6;
            }
        }
    }
}

void SetMainCursorState(u8 state)
{
    switch (state)
    {
    case 0:
        gEasyChatScreen->mainCursorSprite->data[1] = state;
        gEasyChatScreen->mainCursorSprite->x2 = state;
        gEasyChatScreen->mainCursorSprite->invisible = FALSE;
        break;
    case 1:
        gEasyChatScreen->mainCursorSprite->data[1] = state;
        gEasyChatScreen->mainCursorSprite->x2 = 0;
        gEasyChatScreen->mainCursorSprite->invisible = FALSE;
        break;
    case 2:
        gEasyChatScreen->mainCursorSprite->data[1] = 1;
        gEasyChatScreen->mainCursorSprite->x2 = 0;
        gEasyChatScreen->mainCursorSprite->invisible = TRUE;
        break;
    }
}

static const struct OamData sOamData_RectangleCursor =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 1,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_RectCursorPrimary_Group[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorPrimary_Button[] =
{
#if ENGLISH
    ANIMCMD_FRAME(8, 5),
#else
    ANIMCMD_FRAME(0, 5),
#endif
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorPrimary_Others[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorPrimary_Letter[] =
{
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorMiddle_Group[] =
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorMiddle_Button[] =
{
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorMiddle_Others[] =
{
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorMiddle_Letter[] =
{
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorEnd_Group[] =
{
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorEnd_Button[] =
{
#if ENGLISH
    ANIMCMD_FRAME(40, 5),
#else
    ANIMCMD_FRAME(16, 5),
#endif
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorEnd_Others[] =
{
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RectCursorEnd_Letter[] =
{
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_RectCursorPrimary[] =
{
    sAnim_RectCursorPrimary_Group,
    sAnim_RectCursorPrimary_Button,
    sAnim_RectCursorPrimary_Others,
    sAnim_RectCursorPrimary_Letter,
};

static const union AnimCmd *const sAnims_RectCursorMiddle[] =
{
    sAnim_RectCursorMiddle_Group,
    sAnim_RectCursorMiddle_Button,
    sAnim_RectCursorMiddle_Others,
    sAnim_RectCursorMiddle_Letter,
};

static const union AnimCmd *const sAnims_RectCursorEnd[] =
{
    sAnim_RectCursorEnd_Group,
    sAnim_RectCursorEnd_Button,
    sAnim_RectCursorEnd_Others,
    sAnim_RectCursorEnd_Letter,
};

void CreateRectangleCursorSprites(void)
{
    struct SpriteTemplate spriteTemplate =
    {
        .tileTag = 1,
        .paletteTag = 1,
        .oam = &sOamData_RectangleCursor,
        .anims = sAnims_RectCursorPrimary,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_RectangleCursorPrimary,
    };
    u8 spriteId;

    spriteId = CreateSprite(&spriteTemplate, 0, 0, 3);
    gEasyChatScreen->rectangleCursorPrimarySprite = &gSprites[spriteId];

    spriteTemplate.anims = sAnims_RectCursorMiddle;
    spriteTemplate.callback = SpriteCB_RectangleCursorMiddle;
    spriteId = CreateSprite(&spriteTemplate, 0, 0, 4);
    gEasyChatScreen->rectangleCursorMiddleSprite = &gSprites[spriteId];

    spriteTemplate.anims = sAnims_RectCursorEnd;
    spriteTemplate.callback = SpriteCB_RectangleCursorEnd;
    spriteId = CreateSprite(&spriteTemplate, 0, 0, 5);
    gEasyChatScreen->rectangleCursorEndSprite = &gSprites[spriteId];

    gEasyChatScreen->rectangleCursorPrimarySprite->data[0] = 0;
    gEasyChatScreen->rectangleCursorMiddleSprite->data[0] = 0;
    gEasyChatScreen->rectangleCursorEndSprite->data[0] = 0;

    gEasyChatScreen->rectangleCursorPrimarySprite->data[3] = 0x0101 + IndexOfSpritePaletteTag(1) * 16;
    UpdateRectangleCursorPos();
    gEasyChatScreen->keyboardCursorMoved = 1;
    REG_BLDCNT = 0x3F40;
}

void DestroyRectangleCursorSprites(void)
{
    DestroySprite(gEasyChatScreen->rectangleCursorPrimarySprite);
    DestroySprite(gEasyChatScreen->rectangleCursorMiddleSprite);
    DestroySprite(gEasyChatScreen->rectangleCursorEndSprite);
}

void UpdateRectangleCursorPos(void)
{
    if (gEasyChatScreen->inButtonWindow != 0)
        sRectangleCursorAnim = 1;
    else if (gEasyChatScreen->inAlphabetMode == 0)
        sRectangleCursorAnim = 0;
    else if (gEasyChatScreen->keyboardRow == 0 && gEasyChatScreen->keyboardColumn == 6)
        sRectangleCursorAnim = 2;
    else
        sRectangleCursorAnim = 3;

    gEasyChatScreen->rectangleCursorPrimarySprite->data[2] = sRectangleCursorAnim;
    gEasyChatScreen->rectangleCursorMiddleSprite->data[2] = sRectangleCursorAnim;
    gEasyChatScreen->rectangleCursorEndSprite->data[2] = sRectangleCursorAnim;

    StartSpriteAnim(gEasyChatScreen->rectangleCursorPrimarySprite, sRectangleCursorAnim);
    StartSpriteAnim(gEasyChatScreen->rectangleCursorMiddleSprite, sRectangleCursorAnim);
    StartSpriteAnim(gEasyChatScreen->rectangleCursorEndSprite, sRectangleCursorAnim);

    gEasyChatScreen->rectangleCursorPrimarySprite->data[5] = 8;
    gEasyChatScreen->rectangleCursorPrimarySprite->data[6] = 8;
    gEasyChatScreen->rectangleCursorPrimarySprite->data[1] = 0;
}

static const s8 sAlphabetKeyboardColumnOffsets[][7] =
{
    {1,  3,  5,  8, 10, 12, 15},
    {1,  3,  5,  8, 10, 12,  0},
    {1,  3,  5,  8, 10, 12, 14},
    {1,  3,  5,  8, 10, 12, 14},
};

void SpriteCB_RectangleCursorPrimary(struct Sprite *sprite)
{
    sprite->data[0]++;
    if (sprite->data[0] & 1)
        sprite->data[5] = 8 + (gSineTable[sprite->data[1]] >> 5);
    else
        sprite->data[6] = 8 - (gSineTable[sprite->data[1]] >> 5);

    sprite->data[1] = (sprite->data[1] + 5) & 0xFF;

    REG_BLDALPHA = (sprite->data[6] << 8) | sprite->data[5];

    if (gEasyChatScreen->keyboardCursorMoved != 0)
    {
        UpdateRectangleCursorPos();

        gEasyChatScreen->rectangleCursorPrimarySprite->data[5] = 8;
        gEasyChatScreen->rectangleCursorPrimarySprite->data[6] = 8;
        gEasyChatScreen->rectangleCursorPrimarySprite->data[1] = 0;

        switch (sprite->data[2])
        {
        case 0:
            sprite->x = gEasyChatScreen->keyboardColumn * 88 + 32;
            sprite->y = (gEasyChatScreen->keyboardRow - gEasyChatScreen->keyboardScrollOffset) * 16 + 96;
            break;
        case 1:
#if ENGLISH
            sprite->x = 216;
#else
            sprite->x = 214;
#endif
            sprite->y = gEasyChatScreen->keyboardRow * 16 + 96;
            break;
        case 2:
            sprite->x = 151;
            sprite->y = 96;
            break;
        case 3:
            sprite->x = sAlphabetKeyboardColumnOffsets[gEasyChatScreen->keyboardRow][gEasyChatScreen->keyboardColumn] * 8 + 31;
            sprite->y = (gEasyChatScreen->keyboardRow - gEasyChatScreen->keyboardScrollOffset) * 16 + 96;
            break;
        }
    }
}


void SpriteCB_RectangleCursorMiddle(struct Sprite *sprite)
{
    if (gEasyChatScreen->keyboardCursorMoved != 0 && sprite->data[2] == 0)
    {
        sprite->x = gEasyChatScreen->rectangleCursorPrimarySprite->x + 32;
        sprite->y = gEasyChatScreen->rectangleCursorPrimarySprite->y;
    }
}

void SpriteCB_RectangleCursorEnd(struct Sprite *sprite)
{
    if (gEasyChatScreen->keyboardCursorMoved != 0)
    {
        switch (sprite->data[2])
        {
        case 0:
            sprite->x = gEasyChatScreen->rectangleCursorPrimarySprite->x + 64;
            sprite->y = gEasyChatScreen->rectangleCursorPrimarySprite->y;
            break;
#if GERMAN
        case 1:
            sprite->x = gEasyChatScreen->rectangleCursorPrimarySprite->x + 28;
            sprite->y = gEasyChatScreen->rectangleCursorPrimarySprite->y;
            break;
#endif
        case 2:
            sprite->x = gEasyChatScreen->rectangleCursorPrimarySprite->x + 21;
            sprite->y = gEasyChatScreen->rectangleCursorPrimarySprite->y;
            break;
        }
    }
}

void SetWordSelectCursorAnim(u8 visible)
{
    gEasyChatScreen->wordSelectCursorSprite->data[1] = (visible == 0);
    gEasyChatScreen->wordSelectCursorSprite->x2 = 0;
}

void SetWordSelectCursorVisibility(u8 visible)
{
    gEasyChatScreen->wordSelectCursorSprite->invisible = (visible == 0);
    SetWordSelectCursorAnim(visible);
    if (visible != 0)
        gEasyChatScreen->wordSelectCursorMoved = 1;
}

void CreateWordSelectCursorSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_TriangleCursor, 0, 0, 0);

    gSprites[spriteId].data[0] = 0;
    gSprites[spriteId].data[1] = 0;
    gEasyChatScreen->wordSelectCursorSprite = &gSprites[spriteId];
    gEasyChatScreen->wordSelectCursorSprite->callback = SpriteCB_WordSelectCursor;
    SetWordSelectCursorVisibility(0);
}

void SpriteCB_WordSelectCursor(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        if (gEasyChatScreen->wordSelectCursorMoved != 0)
        {
            sprite->x = gEasyChatScreen->wordSelectColumn * 88 + 44;
            sprite->y = (gEasyChatScreen->wordSelectRow - gEasyChatScreen->wordSelectScrollOffset) * 16 + 96;
            sprite->x2 = -6;
            sprite->data[0] = 0;
        }
        else
        {
            sprite->data[0]++;
            if (sprite->data[0] > 2)
            {
                sprite->data[0] = 0;
                sprite->x2++;
                if (sprite->x2 > 0)
                    sprite->x2 = -6;
            }
        }
    }
}

void HideScrollIndicatorSprites(void)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        gEasyChatScreen->scrollIndicatorSprites[i]->invisible = TRUE;
        gEasyChatScreen->scrollIndicatorSprites[i]->data[1] = 0;
        gEasyChatScreen->pageScrollButtonSprites[i]->invisible = TRUE;
        gEasyChatScreen->pageScrollButtonSprites[i]->data[1] = 0;
    }
    gEasyChatScreen->scrollIndicatorsActive = 0;
}

void SetScrollIndicatorMode(u8 animNum)
{
    s16 indicatorX;
    u16 i;

    if (animNum == 0)
    {
        indicatorX = 100;
        gEasyChatScreen->scrollOffsetPtr = &gEasyChatScreen->keyboardScrollOffset;
        if (gEasyChatScreen->inAlphabetMode == 0)
            gEasyChatScreen->scrollOffsetMax = gEasyChatScreen->keyboardLastRow - 4;
        else
            gEasyChatScreen->scrollOffsetMax = 0;
    }
    else
    {
        indicatorX = 120;
        gEasyChatScreen->scrollOffsetPtr = &gEasyChatScreen->wordSelectScrollOffset;
        gEasyChatScreen->scrollOffsetMax = gEasyChatScreen->wordSelectLastRow - 4;
        for (i = 0; i < 2; i++)
        {
            gEasyChatScreen->pageScrollButtonSprites[i]->invisible = FALSE;
            gEasyChatScreen->pageScrollButtonSprites[i]->data[1] = 1;
        }
    }

    if (gEasyChatScreen->scrollOffsetMax < 0)
        gEasyChatScreen->scrollOffsetMax = 0;

    for (i = 0; i < 2; i++)
    {
        gEasyChatScreen->scrollIndicatorSprites[i]->x = indicatorX;
        gEasyChatScreen->scrollIndicatorSprites[i]->invisible = FALSE;
        gEasyChatScreen->scrollIndicatorSprites[i]->data[1] = 1;
        StartSpriteAnim(gEasyChatScreen->scrollIndicatorSprites[i], animNum);
    }

    gEasyChatScreen->scrollIndicatorsActive = 1;
}

static const struct OamData sOamData_ScrollIndicator =
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
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_ScrollArrowKeyboard[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ScrollArrowWordSelect[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_ScrollArrow[] =
{
    sAnim_ScrollArrowKeyboard,
    sAnim_ScrollArrowWordSelect,
};

static const union AnimCmd sAnim_PageScrollStartButton[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_PageScrollSelectButton[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_PageScrollButtons[] =
{
    sAnim_PageScrollStartButton,
    sAnim_PageScrollSelectButton,
};

void CreateScrollIndicatorSprites(void)
{
    struct SpriteTemplate spriteTemplate =
    {
        .tileTag = 2,
        .paletteTag = 0,
        .oam = &sOamData_ScrollIndicator,
        .anims = sAnims_ScrollArrow,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_ScrollIndicator,
    };
    u16 i;
    u8 spriteId;

    for (i = 0; i < 2; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 0, 84 + i * 72 , 0);
        gEasyChatScreen->scrollIndicatorSprites[i] = &gSprites[spriteId];
        gEasyChatScreen->scrollIndicatorSprites[i]->data[0] = i;
        gEasyChatScreen->scrollIndicatorSprites[i]->vFlip = i;
    }

    spriteTemplate.tileTag = 3;
    spriteTemplate.anims = sAnims_PageScrollButtons;

    for (i = 0; i < 2; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 142 + i * 40, 88, 0);
        gEasyChatScreen->pageScrollButtonSprites[i] = &gSprites[spriteId];
        gEasyChatScreen->pageScrollButtonSprites[i]->data[0] = i;
        gEasyChatScreen->pageScrollButtonSprites[i]->oam.shape = 1;
        gEasyChatScreen->pageScrollButtonSprites[i]->oam.size = 1;
        StartSpriteAnim(gEasyChatScreen->pageScrollButtonSprites[i], i);
    }

    SetScrollIndicatorMode(0);
    HideScrollIndicatorSprites();
}

void SpriteCB_ScrollIndicator(struct Sprite *sprite)
{
    bool8 invisible;

    if (gEasyChatScreen->scrollIndicatorsActive != 0 && sprite->data[1] != 0)
    {
        if (sprite->data[0] == 0)
        {
            invisible = FALSE;
            if (*gEasyChatScreen->scrollOffsetPtr == 0)
                invisible = TRUE;
            sprite->invisible = invisible;
        }
        else
        {
            invisible = FALSE;
            if (*gEasyChatScreen->scrollOffsetPtr == gEasyChatScreen->scrollOffsetMax)
                invisible = TRUE;
            sprite->invisible = invisible;
        }
    }
}

// defined below
extern const struct SpriteTemplate sSpriteTemplate_InterviewFrame;

void CreateInterviewObjectEvents(u8 personType, u8 frameId)
{
    struct CompressedSpriteSheet interviewFrameSheet = {sRSInterviewFrame_Gfx, 2048, 0x0006};
    struct SpritePalette interviewFramePalette = {sRSInterviewFrame_Pal, 0x0004};
    s16 x;
    s16 y;
    u16 gabbyTyGfxId;
    u8 spriteId;

    switch (frameId)
    {
    case 2:
    default:
        x = 64;
        y = 40;
        break;
    case 5:
        x = 36;
        y = 48;
        break;
    }

    switch (personType)
    {
    case 0:  // Gabby
    default:
        gabbyTyGfxId = OBJ_EVENT_GFX_REPORTER_M;
        break;
    case 1:  // Ty
        gabbyTyGfxId = OBJ_EVENT_GFX_REPORTER_F;
        break;
    }

    LoadCompressedSpriteSheet(&interviewFrameSheet);
    LoadSpritePalette(&interviewFramePalette);
    CreateSprite(&sSpriteTemplate_InterviewFrame, x, y, 1);

    spriteId = CreateObjectGraphicsSprite(
      (gSaveBlock2.playerGender == MALE) ? OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL : OBJ_EVENT_GFX_RIVAL_MAY_NORMAL,
      SpriteCallbackDummy, x - 12, y, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].oam.priority = 0;
        StartSpriteAnim(&gSprites[spriteId], 3);
    }

    spriteId = CreateObjectGraphicsSprite(gabbyTyGfxId, SpriteCallbackDummy, x + 12, y, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].oam.priority = 0;
        StartSpriteAnim(&gSprites[spriteId], 2);
    }
}

static const struct OamData sOamData_InterviewFrame =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct SpriteTemplate sSpriteTemplate_InterviewFrame =
{
    .tileTag = 6,
    .paletteTag = 4,
    .oam = &sOamData_InterviewFrame,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct CompressedSpriteSheet sSpriteSheet_ModeWindow = {gMenuWordGroupIndicator_Gfx, 4096, 0x0007};

static const struct SpritePalette sSpritePalette_ModeWindow = {gMenuWordGroupIndicator_Pal, 0x0005};

static const struct OamData sOamData_ModeWindow =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_ModeWindowHidden[] =
{
    ANIMCMD_FRAME(96, 3),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ModeWindowGroup[] =
{
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(96, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ModeWindowAlphabet[] =
{
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(96, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ModeWindowRevealGroup[] =
{
    ANIMCMD_FRAME(64, 2),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ModeWindowRevealAlphabet[] =
{
    ANIMCMD_FRAME(64, 2),
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ModeWindowHide[] =
{
    ANIMCMD_FRAME(64, 2),
    ANIMCMD_FRAME(96, 2),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_ModeWindow[] =
{
    sAnim_ModeWindowHidden,
    sAnim_ModeWindowGroup,
    sAnim_ModeWindowAlphabet,
    sAnim_ModeWindowRevealGroup,
    sAnim_ModeWindowRevealAlphabet,
    sAnim_ModeWindowHide,
};

static const struct SpriteTemplate sSpriteTemplate_ModeWindow =
{
    .tileTag = 7,
    .paletteTag = 5,
    .oam = &sOamData_ModeWindow,
    .anims = sAnims_ModeWindow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

void CreateModeWindowSprite(void)
{
    u8 spriteId;

    LoadCompressedSpriteSheet(&sSpriteSheet_ModeWindow);
    LoadSpritePalette(&sSpritePalette_ModeWindow);

    spriteId = CreateSprite(&sSpriteTemplate_ModeWindow, 224, 88, 0);
    if (spriteId != MAX_SPRITES)
        gEasyChatScreen->modeWindowSprite = &gSprites[spriteId];
    else
        gEasyChatScreen->modeWindowSprite = NULL;
}

void UpdateModeWindowAnim(void)
{
    if (gEasyChatScreen->modeWindowSprite != NULL)
    {
        if (gEasyChatScreen->inAlphabetMode == 0)
            StartSpriteAnim(gEasyChatScreen->modeWindowSprite, 1);
        else
            StartSpriteAnim(gEasyChatScreen->modeWindowSprite, 2);
    }
}

void SetModeWindowAnimForVisibility(u8 visible)
{
    if (gEasyChatScreen->modeWindowSprite != NULL)
    {
        if (visible != 0)
        {
            if (gEasyChatScreen->inAlphabetMode == 0)
                StartSpriteAnim(gEasyChatScreen->modeWindowSprite, 3);
            else
                StartSpriteAnim(gEasyChatScreen->modeWindowSprite, 4);
        }
        else
        {
            StartSpriteAnim(gEasyChatScreen->modeWindowSprite, 5);
        }
    }
}

static const u16 sEasyChatBg2Secondary_Pal[] = INCBIN_U16("graphics/easy_chat/bg2_secondary.gbapal");

static const u16 sBlankPhraseWordTilemap[] =
{
    0x300, 0x300, 0x300, 0x300, 0x300, 0x300, 0x300, 0x300, 0x300,
    0x301, 0x301, 0x301, 0x301, 0x301, 0x300, 0x300, 0x300, 0x300,
};

static const u16 sEasyChatBg3_Pal[] = INCBIN_U16("graphics/easy_chat/bg3.gbapal");

// unused tilemap?
const u16 gUnusedEasyChatPhraseTilemap[] =
{
    0x5036, 0x5037, 0x5038, 0x5039, 0x503A, 0x503B,
    0x5046, 0x5047, 0x5048, 0x5049, 0x504A, 0x504B,
    0x5056, 0x5057, 0x5058, 0x5059, 0x505A, 0x505B,
    0x5030, 0x5031, 0x5032, 0x5033, 0x5034, 0x5035,
    0x5040, 0x5041, 0x5042, 0x5043, 0x5044, 0x5045,
    0x5050, 0x5051, 0x5052, 0x5053, 0x5054, 0x5055,
};

static const u8 sClearPhraseRowText[] = _("{CLEAR_TO 88}");

void InitEasyChatScreenGraphics(void)
{
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;

    REG_BG0CNT = 0x8B00;
    REG_BG1CNT = 0x0D09;

    CpuCopy16(gEasyChatWordGroup_Tilemap, gEasyChatScreen->wordGroupTilemapBuffer, 0x500);
    LZ77UnCompWram(gEasyChatInputSelect_Tilemap, gEasyChatScreen->inputSelectTilemapBuffer);
    LZ77UnCompVram(gMenuWordGroupFrame_Gfx, (void *)(VRAM + 0x8000));
    CpuCopy16(gEasyChatScreen->wordGroupTilemapBuffer, (void *)(VRAM + 0x6800), 0x500);
    LoadPalette(gMenuWordGroupFrame1_Pal, 64, 64);

    REG_BG2CNT = 0x0E02;

    DmaClear16(3, (void *)(VRAM + 0x7000), 0x800);
    DmaCopy16Defvars(3, gEasyChatBg2_Gfx, (void *)(VRAM + 0x6000), 64);
    LZ77UnCompVram(gEasyChatBg2_Tilemap, (void *)(VRAM + 0x7000));
    LoadPalette(gEasyChatBg2_Pal, 0, 32);
    LoadPalette(sEasyChatBg2Secondary_Pal, 16, 32);

    REG_BG3CNT = 0x0F0F;

    DmaClear32(3, (void *)(VRAM + 0x7800), 0x800);
    LoadPalette(sEasyChatBg3_Pal, 32, 32);
    gPlttBufferUnfaded[63] = RGB(31, 31, 31);
    gPlttBufferUnfaded[49] = RGB(27, 26, 27);
    gPlttBufferUnfaded[56] = RGB(28, 28, 28);
    gPlttBufferFaded[63] = RGB(31, 31, 31);
    gPlttBufferFaded[49] = RGB(27, 26, 27);
    gPlttBufferFaded[56] = RGB(28, 28, 28);

    gEasyChatScreen->lowerWindowAnimDirty = 0;
    InitEasyChatFramePaletteBlend();
    BufferFrameTilemap();
    SetEasyChatMessageId(10);
    PrintEasyChatStdMessage(0);
    PrintCurrentPhrase();

    REG_BLDCNT = 0;
}

// The components are ORed in the reverse order from the normal RGB macro
#define RGB_(r, g, b) ((((b) & 0x1F) << 10) | (((g) & 0x1F) << 5) | ((r) & 0x1F))

#define R_VAL(color) ((color) & 0x1F)
#define G_VAL(color) (((color) >> 5) & 0x1F)
#define B_VAL(color) (((color) >> 10) & 0x1F)

void InitEasyChatFramePaletteBlend(void)
{
    u16 i;
    u16 colorComponent;
    u16 blendStep;
    s32 startColors[3];
    s32 endColors[3];
    s32 colorSteps[3];

    for (i = 0; i < 3; i++)
    {
        startColors[0] = R_VAL(*(gMenuWordGroupFrame2_Pal + i + 1)) << 8;
        endColors[0] = R_VAL(*(gMenuWordGroupFrame2_Pal + i + 4)) << 8;
        startColors[1] = G_VAL(*(gMenuWordGroupFrame2_Pal + i + 1)) << 8;
        endColors[1] = G_VAL(*(gMenuWordGroupFrame2_Pal + i + 4)) << 8;
        startColors[2] = B_VAL(*(gMenuWordGroupFrame2_Pal + i + 1)) << 8;
        endColors[2] = B_VAL(*(gMenuWordGroupFrame2_Pal + i + 4)) << 8;

        for (colorComponent = 0; colorComponent < 3; colorComponent++)
            colorSteps[colorComponent] = (endColors[colorComponent] - startColors[colorComponent]) / 8;

        for (blendStep = 0; blendStep < 8; blendStep++)
        {
            gEasyChatScreen->framePaletteBlendColors[i][blendStep] = RGB_(startColors[0] >> 8, startColors[1] >> 8, startColors[2] >> 8);
            for (colorComponent = 0; colorComponent < 3; colorComponent++)
                startColors[colorComponent] += colorSteps[colorComponent];
        }
        gEasyChatScreen->framePaletteBlendColors[i][blendStep] = RGB_(endColors[0] >> 8, endColors[1] >> 8, endColors[2] >> 8);
    }
    gEasyChatScreen->framePaletteBlendIndex = 0;
}

void UpdateEasyChatFramePaletteBlend(s8 blendDelta)
{
    u16 i;

    gEasyChatScreen->framePaletteBlendIndex += blendDelta;
    for (i = 0; i < 3; i++)
    {
        gPlttBufferUnfaded[81 + i] = gEasyChatScreen->framePaletteBlendColors[i][gEasyChatScreen->framePaletteBlendIndex];
        gPlttBufferFaded[81 + i] = gEasyChatScreen->framePaletteBlendColors[i][gEasyChatScreen->framePaletteBlendIndex];
    }
}

void ResetEasyChatFramePaletteBlend(void)
{
    gEasyChatScreen->framePaletteBlendIndex = 0;
    UpdateEasyChatFramePaletteBlend(0);
}

void SetEasyChatMessageId(u8 messageId)
{
    sEasyChatMessageId = messageId;
}

u8 GetEasyChatMessageId(void)
{
    return sEasyChatMessageId;
}

void PrintEasyChatStdText(const u8 *text, u8 left, u8 top, u8 height)
{
    Menu_PrintTextWithNewlineIndentAndClear(text, left * 8, top * 8, 176, height * 8, 1);
}

void PrintEasyChatStdMessage(u8 messageId)
{
    BasicInitMenuWindow(&gEasyChatMessageWindowTemplate);

    if (messageId == 10)
    {
        Menu_EraseWindowRect(3, 14, 26, 19);
        SetEasyChatMessageId(10);
        return;
    }

    if (GetEasyChatMessageId() == 10)
        Menu_DrawStdWindowFrame(3, 14, 26, 19);

    SetEasyChatMessageId(messageId);
    switch (messageId)
    {
    case 0:
        PrintEasyChatStdText(gEasyChatScreen->instructionsText1, 4, 15, 2);
        PrintEasyChatStdText(gEasyChatScreen->instructionsText2, 4, 17, 2);
        break;
    case 1:
        PrintEasyChatStdText(gEasyChatScreen->confirmText1, 4, 15, 2);
        PrintEasyChatStdText(gEasyChatScreen->confirmText2, 4, 17, 2);
        break;
    case 2:
        PrintEasyChatStdText(gOtherText_TextDeletedConfirmPage1, 4, 15, 2);
        PrintEasyChatStdText(gOtherText_TextDeletedConfirmPage2, 4, 17, 2);
        break;
    case 3:
        switch (gEasyChatScreen->type)
        {
        case EASY_CHAT_TYPE_TRENDY_PHRASE:
            PrintEasyChatStdText(gOtherText_QuitGivingInfo, 4, 15, 4);
            break;
        case EASY_CHAT_TYPE_MAIL:
            PrintEasyChatStdText(gOtherText_StopGivingMail, 4, 15, 4);
            break;
        default:
            PrintEasyChatStdText(gOtherText_QuitEditing, 4, 15, 4);
            break;
        }
        break;
    case 4:
        PrintEasyChatStdText(gOtherText_EditedTextNoSavePage1, 4, 15, 2);
        PrintEasyChatStdText(gOtherText_EditedTextNoSavePage2, 4, 17, 2);
        break;
    case 5:
        PrintEasyChatStdText(gOtherText_EnterAPhraseOrWord, 4, 15, 4);
        break;
    case 6:
        PrintEasyChatStdText(gOtherText_TextNoDelete, 4, 15, 4);
        break;
    case 7:
        PrintEasyChatStdText(gOtherText_OnlyOnePhrase, 4, 15, 2);
        PrintEasyChatStdText(gOtherText_OriginalSongRestored, 4, 17, 2);
        break;
    case 8:
        PrintEasyChatStdText(gOtherText_TrendyAlready, 4, 15, 4);
        break;
    case 9:
        PrintEasyChatStdText(gOtherText_CombineTwoPhrases, 4, 15, 4);
        break;
    }
}

void PrintTitle(u8 type)
{
    const u8 *titleText;

    switch (type)
    {
    case EASY_CHAT_TYPE_PROFILE:
        titleText = gOtherText_Profile;
        break;
    case EASY_CHAT_TYPE_BATTLE_START:
        titleText = gOtherText_AtBattleStart;
        break;
    case EASY_CHAT_TYPE_BATTLE_WON:
        titleText = gOtherText_UponWinningBattle;
        break;
    case EASY_CHAT_TYPE_BATTLE_LOST:
        titleText = gOtherText_UponLosingBattle;
        break;
    case EASY_CHAT_TYPE_BARD_SONG:
        titleText = gOtherText_TheBardsSong;
        break;
    case EASY_CHAT_TYPE_TRENDY_PHRASE:
        titleText = gOtherText_WhatsHipHappening;
        break;
    case EASY_CHAT_TYPE_INTERVIEW:
    case EASY_CHAT_TYPE_FAN_CLUB:
    case EASY_CHAT_TYPE_DUMMY_SHOW:
    case EASY_CHAT_TYPE_GABBY_AND_TY:
    case EASY_CHAT_TYPE_CONTEST_INTERVIEW:
    case EASY_CHAT_TYPE_BATTLE_TOWER_INTERVIEW:
        titleText = gOtherText_Interview;
        break;
    case EASY_CHAT_TYPE_GOOD_SAYING:
        titleText = gOtherText_GoodSaying;
        break;
    default:
        titleText = NULL;
        break;
    }

    BasicInitMenuWindow(&gEasyChatTitleAndPhraseWindowTemplate);
    gEasyChatScreen->titlePrintBuffer[0] = EXT_CTRL_CODE_BEGIN;
    gEasyChatScreen->titlePrintBuffer[1] = 5;
    gEasyChatScreen->titlePrintBuffer[2] = 1;
    if (titleText != NULL)
    {
        AlignStringInMenuWindow(gEasyChatScreen->titlePrintBuffer + 3, titleText, 240, 2);
        Menu_PrintText(gEasyChatScreen->titlePrintBuffer, 0, 0);
    }
    else
    {
        u8 *str = gEasyChatScreen->titlePrintBuffer + 3;

        str[0] = EXT_CTRL_CODE_BEGIN;
        str[1] = 0x11;
        str[2] = 0x10;
        str[3] = EOS;

        Menu_PrintText(gEasyChatScreen->titlePrintBuffer, 0, 0);
        Menu_PrintText(gEasyChatScreen->titlePrintBuffer, 28, 0);
    }
    BasicInitMenuWindow(&gEasyChatMessageWindowTemplate);
}

void BufferFrameTilemap(void)
{
    switch (gEasyChatScreen->templateId)
    {
    case 4:
        CopyTilemapRectFixedWidth((void *)(VRAM + 0x6800), 3, 3, gEasyChatFrame_Tilemap, 0, 26, 24, 6);
        break;
    case 5:
        CopyTilemapRectFixedWidth((void *)(VRAM + 0x6800), 14, 2, gEasyChatFrame_Tilemap, 0, 18, 13, 8);
        break;
    case 0:
        CopyTilemapRectFixedWidth((void *)(VRAM + 0x6800), 2, 2, gEasyChatFrame_Tilemap, 0, 10, 26, 8);
        break;
    case 1:
        CopyTilemapRectFixedWidth((void *)(VRAM + 0x6800), 2, 0, gEasyChatFrame_Tilemap, 0, 0, 26, 10);
        break;
    case 2:
        CopyTilemapRectFixedWidth((void *)(VRAM + 0x6800), 14, 3, gEasyChatFrame_Tilemap, 13, 18, 13, 4);
        break;
    case 3:
        CopyTilemapRectFixedWidth((void *)(VRAM + 0x6800), 3, 2, gEasyChatFrame_Tilemap, 0, 32, 24, 4);
        break;
    }
}

void PrintCurrentPhrase(void)
{
    BasicInitMenuWindow(&gEasyChatTitleAndPhraseWindowTemplate);
    switch (gEasyChatScreen->templateId)
    {
    case 4:
        PrintCurrentPhraseWords(4, 6);
        break;
    case 5:
        PrintCurrentPhraseWords(2, 17);
        break;
    case 0:
        PrintCurrentPhraseWords(3, 5);
        break;
    case 1:
        PrintCurrentPhraseWords(0, 5);
        break;
    case 2:
        PrintCurrentPhraseWords(4, 17);
        break;
    case 3:
        PrintCurrentPhraseWords(3, 6);
        break;
    }
    BasicInitMenuWindow(&gEasyChatMessageWindowTemplate);
}

void PrintCurrentPhraseWords(u16 top, u16 left)
{
    u16 row;
    u16 column;
    u16 phraseWordIndex = 0;
    u16 y = top;

    for (row = 0; row < gEasyChatScreen->numRows; row++)
    {
        u16 x = left;

        for (column = 0; column < gEasyChatScreen->numColumns; column++)
        {
            if (phraseWordIndex >= gEasyChatScreen->maxWords)
            {
                Menu_PrintText(sClearPhraseRowText, x, y);
                break;
            }
            if (gEasyChatScreen->currentPhrase[phraseWordIndex] == 0xFFFF)
            {
                CopyTilemapRect((void *)(VRAM + 0x7000), x, y, sBlankPhraseWordTilemap, 0, 0, 9, 2, 9);
                x += 11;
            }
            else
            {
                CopyEasyChatWordPadded(gEasyChatScreen->wordPrintBuffer, gEasyChatScreen->currentPhrase[phraseWordIndex], 11);
                Menu_PrintText(gEasyChatScreen->wordPrintBuffer, x, y);
                x += gEasyChatScreen->phraseWordSpacing[row][column] + 11;
            }
            phraseWordIndex++;
        }
        y += 2;
    }
}

void PrintCurrentPhraseAsText(void)
{
    BasicInitMenuWindow(&gEasyChatTitleAndPhraseWindowTemplate);
    switch (gEasyChatScreen->templateId)
    {
    case 4:
        PrintCurrentPhraseAsTextAt(4, 6);
        break;
    case 5:
        PrintCurrentPhraseAsTextAt(2, 17);
        break;
    case 0:
        PrintCurrentPhraseAsTextAt(3, 5);
        break;
    case 1:
        PrintCurrentPhraseAsTextAt(0, 5);
        break;
    case 2:
        PrintCurrentPhraseAsTextAt(4, 17);
        break;
    case 3:
        PrintCurrentPhraseAsTextAt(3, 6);
        break;
    }
    BasicInitMenuWindow(&gEasyChatMessageWindowTemplate);
}

void PrintCurrentPhraseAsTextAt(u16 top, u16 left)
{
    u8 row;
    u8 phraseWordIndex = 0;

    asm("":::"r9");

    for (row = 0; row < gEasyChatScreen->numRows; row++)
    {
        u8 column;
        u8 *text = gEasyChatScreen->phraseTextBuffer;
        u32 textWidth;

        for (column = 0; column < gEasyChatScreen->numColumns; column++)
        {
            if (phraseWordIndex >= gEasyChatScreen->maxWords)
                break;
            if (gEasyChatScreen->currentPhrase[phraseWordIndex] != 0xFFFF)
            {
                text = CopyEasyChatWordPadded(text, gEasyChatScreen->currentPhrase[phraseWordIndex], 0);
                *text++ = CHAR_SPACE;
            }
            phraseWordIndex++;
        }

        textWidth = gEasyChatScreen->numColumns * 11;
        text[0] = EXT_CTRL_CODE_BEGIN;
        text[1] = 0x13;
        text[2] = textWidth * 8;
        text[3] = EOS;

        Menu_PrintText(gEasyChatScreen->phraseTextBuffer, left, top);

        top += 2;
    }
}

void ClearPhraseWindow(void)
{
    u8 topRow;
    u8 numRows;

    BasicInitMenuWindow(&gEasyChatTitleAndPhraseWindowTemplate);
    switch (gEasyChatScreen->templateId)
    {
    case 4:
        topRow = 4;
        numRows = 2;
        break;
    case 5:
        topRow = 3;
        numRows = 2;
        break;
    case 0:
        topRow = 3;
        numRows = 2;
        break;
    case 1:
        topRow = 1;
        numRows = 3;
        break;
    case 2:
        topRow = 4;
        numRows = 1;
        break;
    case 3:
        topRow = 3;
        numRows = 1;
        break;
    default:
        return;
    }
    Menu_BlankWindowRect(2, topRow, 27, numRows * 2 + topRow - 1);
    BasicInitMenuWindow(&gEasyChatMessageWindowTemplate);
}

void PrintKeyboardGroupNamesRow(u8 *text, u8 row)
{
    u8 visibleRow = row & 7;

    Menu_BlankWindowRect(2, visibleRow * 2, 31, visibleRow * 2 + 1);
    BufferEasyChatGroupNamesRow(text, row);
    PrintEasyChatTextAt(text, 2, visibleRow * 2);
}

#if ENGLISH

static const u8 sAlphabetKeyboardRowsText[][32] =
{
	_("{UNKNOWN_14 8} A B C  D E F  {UNKNOWN_14 0}others"),
	_("{UNKNOWN_14 8} G H I  J K L      "),
	_("{UNKNOWN_14 8} M N O  P Q R S    "),
	_("{UNKNOWN_14 8} T U V  W X Y Z    "),
};

#else

static const u8 sGermanAlphabetKeyboardRow0[] = _("{UNKNOWN_14 8} A B C  D E F  {UNKNOWN_14 0}mehr");
static const u8 sGermanAlphabetKeyboardRow1[] = _("{UNKNOWN_14 8} G H I  J K L      ");
static const u8 sGermanAlphabetKeyboardRow2[] = _("{UNKNOWN_14 8} M N O  P Q R S    ");
static const u8 sGermanAlphabetKeyboardRow3[] = _("{UNKNOWN_14 8} T U V  W X Y Z    ");
static const u8 *const sAlphabetKeyboardRowsText[] =
{
    sGermanAlphabetKeyboardRow0,
    sGermanAlphabetKeyboardRow1,
    sGermanAlphabetKeyboardRow2,
    sGermanAlphabetKeyboardRow3,
    sGermanAlphabetKeyboardRow0 + 21,  // pointer to "mehr". What is this for?
};

#endif

void PrintKeyboardText(void)
{
    BasicInitMenuWindow(&gEasyChatInputSelectWindowTemplate);
    if (gEasyChatScreen->inAlphabetMode == 0)
    {
        u16 i;

        for (i = gEasyChatScreen->keyboardScrollOffset; i < gEasyChatScreen->keyboardScrollOffset + 4; i++)
            PrintKeyboardGroupNamesRow(gEasyChatScreen->groupNamePrintBuffer, i);
    }
    else
    {
        ClearLowerWindow();
        PrintEasyChatTextAt(sAlphabetKeyboardRowsText[0], 2, 0);
        PrintEasyChatTextAt(sAlphabetKeyboardRowsText[1], 2, 2);
        PrintEasyChatTextAt(sAlphabetKeyboardRowsText[2], 2, 4);
        PrintEasyChatTextAt(sAlphabetKeyboardRowsText[3], 2, 6);
    }
}

void RestoreKeyboardScrollOffset(void)
{
    ResetLowerWindowScroll();
    if (gEasyChatScreen->inAlphabetMode == 0)
        gEasyChatScreen->lowerWindowScrollOffset += gEasyChatScreen->keyboardScrollOffset * 16;
}

void ResetLowerWindowScroll(void)
{
    gEasyChatScreen->lowerWindowScrollOffset = 0;
}

void PrintEasyChatTextAt(const u8 *text, u16 left, u16 top)
{
    top &= 0xF;
    Menu_PrintText(text, left, top);
}

void BufferEasyChatGroupNamesRow(u8 *dest, u16 row)
{
    u16 i;
    u16 groupIndex = row * 2;

    for (i = 0; i < 2; i++)
    {
        dest[0] = EXT_CTRL_CODE_BEGIN;
        dest[1] = 0x12;
        dest[2] = 88 * i;
        dest += 3;

        dest = CopyEasyChatGroupName(dest, gEasyChatScreen->unlockedGroupIds[row][i], 11);
        groupIndex++;
        if (groupIndex >= gEasyChatScreen->numUnlockedGroups)
            break;
    }
}

void ClearLowerWindow(void)
{
    u16 i;

    BasicInitMenuWindow(&gEasyChatInputSelectWindowTemplate);
    for (i = 0; i < 16; i++)
        ClearWordSelectRow(i);
}

void PrintWordSelectRow(u16 row)
{
    u8 *text;
    u16 i;

    ClearWordSelectRow(row);

    text = gEasyChatScreen->wordPrintBuffer;
    text[0] = EXT_CTRL_CODE_BEGIN;
    text[1] = 0x11;
    text[2] = 0x10;
    text += 3;

    if (gEasyChatScreen->unusedLongWordLimit != 0)
    {
        for (i = 0; i < gEasyChatScreen->wordSelectLastColumns[row]; i++)
        {
            if (GetEasyChatWordStringLength(gEasyChatScreen->selectedGroupWords[row][i]) == 7)
                BasicInitMenuWindow(&gEasyChatWordSelectLongWordWindowTemplate);
            else
                BasicInitMenuWindow(&gEasyChatInputSelectWindowTemplate);
            CopyEasyChatWordPadded(text, gEasyChatScreen->selectedGroupWords[row][i], 9);
            PrintEasyChatTextAt(gEasyChatScreen->wordPrintBuffer, i * 11 + 4, row * 2);
        }
    }
    else
    {
        BasicInitMenuWindow(&gEasyChatInputSelectWindowTemplate);
        for (i = 0; i < gEasyChatScreen->wordSelectLastColumns[row]; i++)
        {
            CopyEasyChatWordPadded(text, gEasyChatScreen->selectedGroupWords[row][i], 9);
            PrintEasyChatTextAt(gEasyChatScreen->wordPrintBuffer, i * 11 + 4, row * 2);
        }
    }
}

void ClearWordSelectRow(u16 row)
{
    row &= 7;
    Menu_BlankWindowRect(0, row * 2, 29, row * 2 + 1);
}

void PrintInitialWordSelectText(void)
{
    u16 rowsToPrint = min(gEasyChatScreen->wordSelectLastRow, 4);
    u16 i;

    for (i = 0; i < rowsToPrint; i++)
        PrintWordSelectRow(gEasyChatScreen->wordSelectScrollOffset + i);
    while (i < 4)
        ClearWordSelectRow(i++);
}

void PrintWordSelectRowsDuringScroll(void)
{
    u16 rowsToPrint;
    u16 row;

    if (gEasyChatScreen->lowerWindowScrollChange > 0)
    {
        rowsToPrint = gEasyChatScreen->lowerWindowScrollChange;
        row = gEasyChatScreen->wordSelectScrollOffset + 4;
    }
    else
    {
        rowsToPrint = -gEasyChatScreen->lowerWindowScrollChange;
        row = gEasyChatScreen->wordSelectScrollOffset - rowsToPrint;
    }
    while (--rowsToPrint != 0xFFFF)
        PrintWordSelectRow(row++);
}

void PrintKeyboardRowsDuringScroll(void)
{
    u16 rowsToPrint;
    u16 row;
    u16 i;

    BasicInitMenuWindow(&gEasyChatInputSelectWindowTemplate);
    if (gEasyChatScreen->lowerWindowScrollChange < 0)
    {
        rowsToPrint = -gEasyChatScreen->lowerWindowScrollChange;
        row = gEasyChatScreen->keyboardScrollOffset - rowsToPrint;
    }
    else
    {
        rowsToPrint = gEasyChatScreen->lowerWindowScrollChange;
        row = gEasyChatScreen->keyboardScrollOffset + 4;
    }
    for (i = 0; i < rowsToPrint; i++)
    {
        PrintKeyboardGroupNamesRow(gEasyChatScreen->groupNamePrintBuffer, row);
        row++;
    }
}

void InitLowerWindowScroll(u8 speed)
{
    gEasyChatScreen->lowerWindowScrollDest = (gEasyChatScreen->lowerWindowScrollOffset + gEasyChatScreen->lowerWindowScrollChange * 16) & 0x7F;
    gEasyChatScreen->lowerWindowScrollStep = (gEasyChatScreen->lowerWindowScrollChange < 0) ? -speed : speed;
}

bool8 UpdateLowerWindowScroll(void)
{
    gEasyChatScreen->lowerWindowScrollOffset = (gEasyChatScreen->lowerWindowScrollOffset + gEasyChatScreen->lowerWindowScrollStep) & 0x7F;
    return (gEasyChatScreen->lowerWindowScrollOffset == gEasyChatScreen->lowerWindowScrollDest);
}

void nullsub_23(void)
{
}

void ResetLowerWindowAnimState(void)
{
    gEasyChatScreen->lowerWindowAnimState = 0;
}

bool8 OpenKeyboard(void)
{
    switch (gEasyChatScreen->lowerWindowAnimState)
    {
    case 0:
        InitLowerWindowExpandHorizontalAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 1:
        if (UpdateLowerWindowAnim() != 0)
        {
            InitKeyboardExpandVerticalAnim();
            gEasyChatScreen->lowerWindowAnimState++;
        }
        break;
    case 2:
        if (UpdateLowerWindowAnim() != 0)
        {
            InitKeyboardRightPanelExpandAnim();
            gEasyChatScreen->lowerWindowAnimState++;
        }
        break;
    case 3:
        if (UpdateLowerWindowAnim() != 0)
            gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 4:
        gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 5:
        return TRUE;
    }
    return FALSE;
}

bool8 CloseKeyboard(void)
{
    switch (gEasyChatScreen->lowerWindowAnimState)
    {
    case 0:
        nullsub_23();
        InitKeyboardRightPanelCollapseAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 1:
        if (UpdateLowerWindowAnim() != 0)
            gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 2:
        InitKeyboardCollapseVerticalAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 3:
        if (UpdateLowerWindowAnim() != 0)
            gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 4:
        InitLowerWindowCollapseHorizontalAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 5:
        if (UpdateLowerWindowAnim() != 0)
            gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 6:
        return TRUE;
    }
    return FALSE;
}

bool8 StartSwitchKeyboardMode(void)
{
    switch (gEasyChatScreen->lowerWindowAnimState)
    {
    case 0:
        nullsub_23();
        InitKeyboardCollapseVerticalAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 1:
        if (UpdateLowerWindowAnim() != 0)
            return TRUE;
        break;
    }
    return FALSE;
}

bool8 FinishSwitchKeyboardMode(void)
{
    switch (gEasyChatScreen->lowerWindowAnimState)
    {
    case 0:
        InitKeyboardExpandVerticalAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 1:
        if (UpdateLowerWindowAnim() != 0)
            return TRUE;
        break;
    }
    return FALSE;
}

bool8 OpenWordSelect(void)
{
    switch (gEasyChatScreen->lowerWindowAnimState)
    {
    case 0:
        nullsub_23();
        InitKeyboardRightPanelCollapseAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 1:
        if (UpdateLowerWindowAnim() != 0)
        {
            InitWordSelectRevealAnim();
            gEasyChatScreen->lowerWindowAnimState++;
        }
        break;
    case 2:
        UpdateEasyChatFramePaletteBlend(1);
        if (UpdateLowerWindowAnim() != 0)
        {
            InitWordSelectRevealFinishAnim();
            gEasyChatScreen->lowerWindowAnimState++;
        }
        break;
    case 3:
        UpdateEasyChatFramePaletteBlend(1);
        if (UpdateLowerWindowAnim() != 0)
            gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 4:
        return TRUE;
    }
    return FALSE;
}

bool8 ReturnToKeyboard(void)
{
    switch (gEasyChatScreen->lowerWindowAnimState)
    {
    case 0:
        InitWordSelectHideAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 1:
        UpdateEasyChatFramePaletteBlend(-1);
        if (UpdateLowerWindowAnim() != 0)
        {
            InitKeyboardRestoreAfterWordSelectAnim();
            gEasyChatScreen->lowerWindowAnimState++;
        }
        break;
    case 2:
        UpdateEasyChatFramePaletteBlend(-1);
        if (UpdateLowerWindowAnim() != 0)
        {
            InitKeyboardRightPanelExpandAnim();
            gEasyChatScreen->lowerWindowAnimState++;
        }
        break;
    case 3:
        if (UpdateLowerWindowAnim() != 0)
            gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 4:
        return TRUE;
    }
    return FALSE;
}

bool8 CloseWordSelect(void)
{
    switch (gEasyChatScreen->lowerWindowAnimState)
    {
    case 0:
        InitWordSelectCollapseVerticalAnim();
        gEasyChatScreen->lowerWindowAnimState++;
        // fall through
    case 1:
        if (UpdateLowerWindowAnim() != 0)
            gEasyChatScreen->lowerWindowAnimState++;
        break;
    case 2:
        ResetEasyChatFramePaletteBlend();
        return TRUE;
    }
    return FALSE;
}

bool8 UpdateLowerWindowAnim(void)
{
    u16 i;

    if (gEasyChatScreen->lowerWindowAnimFrames != 0)
    {
        for (i = 0; i < gEasyChatScreen->lowerWindowAnimRectCount; i++)
        {
            UpdateLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[i]);
            UpdateLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[i]);
        }
        gEasyChatScreen->lowerWindowAnimDirty = 1;
        gEasyChatScreen->lowerWindowAnimFrames--;
        return FALSE;
    }
    return TRUE;
}

void InitLowerWindowExpandHorizontalAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 13, 14, 13, 15, -2, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 12, 14, 12, 15, 0, 0, 2, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 30, 0, -2, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[2], 13, 15, 13, 16, -2, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[2], 0, 9, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[3], 12, 15, 12, 16, 0, 0, 2, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[3], 30, 9, -2, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 4;
    gEasyChatScreen->lowerWindowAnimFrames = 6;
}

void InitLowerWindowCollapseHorizontalAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 1, 14, 13, 15, 2, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 12, 14, 24, 15, 0, 0, -2, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 18, 0, 2, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[2], 1, 15, 13, 16, 2, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[2], 0, 9, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[3], 12, 15, 24, 16, 0, 0, -2, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[3], 18, 9, 2, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[4], 1, 14, 1, 16, 0, 0, 2, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[4], 1, 14, 0, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[5], 24, 14, 24, 16, -2, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[5], 24, 14, -2, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 6;
    gEasyChatScreen->lowerWindowAnimFrames = 6;
}

void InitKeyboardExpandVerticalAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 1, 14, 13, 15, 0, -1, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 12, 14, 24, 15, 0, -1, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 18, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[2], 1, 15, 13, 16, 0, 0, 0, 1);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[2], 0, 9, 0, -1, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[3], 12, 15, 24, 16, 0, 0, 0, 1);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[3], 18, 9, 0, -1, gEasyChatScreen->inputSelectTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 4;
    gEasyChatScreen->lowerWindowAnimFrames = 4;
}

void InitKeyboardCollapseVerticalAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 1, 10, 13, 15, 0, 1, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 12, 10, 24, 15, 0, 1, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 18, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[2], 1, 15, 13, 20, 0, 0, 0, -1);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[2], 0, 5, 0, 1, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[3], 12, 15, 24, 20, 0, 0, 0, -1);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[3], 18, 5, 0, 1, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[4], 1, 10, 24, 10, 0, 0, 0, 1);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[4], 1, 10, 0, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[5], 1, 20, 24, 20, 0, -1, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[5], 1, 20, 0, -1, gEasyChatScreen->wordGroupTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 6;
    gEasyChatScreen->lowerWindowAnimFrames = 4;
}

void InitKeyboardRightPanelExpandAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 24, 12, 25, 20, 0, 0, 1, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 5, 10, -1, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 1;
    gEasyChatScreen->lowerWindowAnimFrames = 5;
}

void InitKeyboardRightPanelCollapseAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 24, 12, 30, 20, 0, 0, -1, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 10, 1, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 30, 12, 31, 20, -1, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 30, 12, -1, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 2;
    gEasyChatScreen->lowerWindowAnimFrames = 6;
}

void InitWordSelectRevealAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 20, 10, 25, 20, 0, 0, 1, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 25, 0, -1, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 0, 10, 1, 20, 0, 0, 1, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 0, 10, 0, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[2], 1, 10, 4, 20, 1, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[2], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 3;
    gEasyChatScreen->lowerWindowAnimFrames = 2;
}

void InitWordSelectRevealFinishAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 3, 10, 4, 20, 0, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 0, 10, 2, 20, 0, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 0, 10, 0, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 2;
    gEasyChatScreen->lowerWindowAnimFrames = 1;
}

void InitWordSelectHideAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 0, 10, 2, 20, 0, 0, -1, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 10, 0, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 3, 10, 5, 20, -1, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[2], 26, 10, 30, 20, -1, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[2], 26, 10, -1, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[3], 25, 10, 26, 20, -1, 0, -1, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[3], 29, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 4;
    gEasyChatScreen->lowerWindowAnimFrames = 2;
}

void InitKeyboardRestoreAfterWordSelectAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 24, 10, 30, 20, 0, 0, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 24, 10, 0, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 23, 10, 24, 20, 0, 0, -1, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 29, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 2;
    gEasyChatScreen->lowerWindowAnimFrames = 1;
}

void InitWordSelectCollapseVerticalAnim(void)
{
    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[0], 0, 10, 30, 15, 0, 1, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[0], 0, 0, 0, 0, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[1], 0, 15, 30, 20, 0, 0, 0, -1);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[1], 0, 5, 0, 1, gEasyChatScreen->inputSelectTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[2], 0, 10, 30, 10, 0, 0, 0, 1);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[2], 0, 10, 0, 0, gEasyChatScreen->wordGroupTilemapBuffer);

    InitLowerWindowAnimRect(gEasyChatScreen->lowerWindowAnimRects[3], 0, 20, 30, 20, 0, -1, 0, 0);
    InitLowerWindowAnimSource(&gEasyChatScreen->lowerWindowAnimSources[3], 0, 20, 0, -1, gEasyChatScreen->wordGroupTilemapBuffer);

    gEasyChatScreen->lowerWindowAnimRectCount = 4;
    gEasyChatScreen->lowerWindowAnimFrames = 5;
}

void InitLowerWindowAnimRect(u16 *rect, u16 left, u16 top, u16 right, u16 bottom, u16 leftStep, u16 topStep, u16 rightStep, u16 bottomStep)
{
    rect[0] = left;
    rect[1] = right;
    rect[2] = top;
    rect[3] = bottom;
    rect[4] = leftStep;
    rect[5] = rightStep;
    rect[6] = topStep;
    rect[7] = bottomStep;
}

void UpdateLowerWindowAnimRect(u16 *rect)
{
    rect[0] += rect[4];
    rect[1] += rect[5];
    rect[2] += rect[6];
    rect[3] += rect[7];
}

void InitLowerWindowAnimSource(struct EasyChatWindowAnimSource *source, u16 x, u16 y, u16 dx, u16 dy, void *tilemap)
{
    source->tilemapX = x;
    source->tilemapY = y;
    source->tilemapXStep = dx;
    source->tilemapYStep = dy;
    source->tilemap = tilemap;
}

void UpdateLowerWindowAnimSource(struct EasyChatWindowAnimSource *source)
{
    source->tilemapX += source->tilemapXStep;
    source->tilemapY += source->tilemapYStep;
}

void CopyLowerWindowAnimToVram(void)
{
    u16 i;

    if (gEasyChatScreen->lowerWindowAnimDirty != 0)
    {
        for (i = 0; i < gEasyChatScreen->lowerWindowAnimRectCount; i++)
            CopyLowerWindowAnimRectToVram((void *)(VRAM + 0x6800), gEasyChatScreen->lowerWindowAnimRects[i], &gEasyChatScreen->lowerWindowAnimSources[i]);
        gEasyChatScreen->lowerWindowAnimDirty = 0;
    }
}

void CopyLowerWindowAnimRectToVram(void *dst, u16 *rect, struct EasyChatWindowAnimSource *source)
{
    s16 width = rect[1] - rect[0];
    s16 height = rect[3] - rect[2];

    if (width > 0 && height > 0)
        CopyTilemapRectFixedWidth(dst, rect[0], rect[2], source->tilemap, source->tilemapX, source->tilemapY, width, height);
}

// Strange...
void UpdateEasyChatScanlineEffect(void)
{
    u16 *scanlineBuffer;
    u16 scanlineOffset;
    u16 i;

    scanlineBuffer = gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer];
    scanlineBuffer += 88;
    scanlineOffset = (gEasyChatScreen->lowerWindowScrollOffset - 88) & 0xFF;

    for (i = 0; i < 64; i++)
    {
        if (gEasyChatScreen->lowerWindowScrollOffset + i == 128)
            scanlineOffset = (-i - 88) & 0xFF;
        *scanlineBuffer++ = scanlineOffset;
    }
}

u8 *CopyEasyChatGroupName(u8 *dest, u8 group, int unused)
{
    return StringCopy(dest, gEasyChatGroupNames[group]);
}

bool8 IsEasyChatGroupUnlocked(u8 group)
{
    switch (group)
    {
    case EC_GROUP_TRENDY_SAYING:
        return FlagGet(FLAG_SYS_HIPSTER_MEET);
    case EC_GROUP_EVENTS:
    case EC_GROUP_MOVE_1:
    case EC_GROUP_MOVE_2:
        return FlagGet(FLAG_SYS_GAME_CLEAR);
    case EC_GROUP_POKEMON_NATIONAL:
        return EasyChatIsNationalPokedexEnabled();
    default:
        return TRUE;
    }
}

bool8 IsEasyChatIndexAndGroupUnlocked(u8 groupId, u16 wordIndex)
{
    switch (groupId)
    {
    case EC_GROUP_TRENDY_SAYING:
        if (gEasyChatScreen->alphabetGroupIds[2][11 + groupId] == 0)
            return FALSE;
        return IsTrendySayingUnlocked(wordIndex);
    case EC_GROUP_POKEMON:
        return GetSetPokedexFlag(SpeciesToNationalPokedexNum(wordIndex), 0);
    case EC_GROUP_EVENTS:
    case EC_GROUP_MOVE_1:
    case EC_GROUP_MOVE_2:
        if (gEasyChatScreen->alphabetGroupIds[2][11 + groupId] == 0)
            return FALSE;
        return TRUE;
    case EC_GROUP_POKEMON_NATIONAL:
        return EasyChatIsNationalPokedexEnabled();
    default:
        return TRUE;
    }
}

u16 EasyChat_GetNumWordsInGroup(u8 groupId)
{
    switch (groupId)
    {
    case EC_GROUP_POKEMON:
        return GetHoennPokedexCount(0);
    case EC_GROUP_TRENDY_SAYING:
        return GetNumTrendySayingsUnlocked();
    default:
        if (IsEasyChatGroupUnlocked(groupId))
            return gEasyChatGroupSizes[groupId];
        return FALSE;
    }
}

#if GERMAN
NAKED
void SetUnlockedWordsByAlphabet(void)
{
    asm(".syntax unified\n\
	push {r4-r7,lr}\n\
	mov r7, r10\n\
	mov r6, r9\n\
	mov r5, r8\n\
	push {r5-r7}\n\
	sub sp, 0x10\n\
	movs r7, 0\n\
	ldr r1, _080EAF08 @ =gEasyChatStruct\n\
	ldr r0, [r1]\n\
	ldr r3, _080EAF0C @ =0x0000446c\n\
	adds r3, r0, r3\n\
	str r3, [sp, 0x4]\n\
_080EAEE4:\n\
	ldr r1, _080EAF10 @ =gEasyChatWordsByLetter\n\
	lsls r2, r7, 1\n\
	adds r0, r2, r1\n\
	ldrh r4, [r0]\n\
	adds r3, r7, 0x1\n\
	lsls r0, r3, 1\n\
	adds r0, r1\n\
	ldrh r0, [r0]\n\
	str r0, [sp]\n\
	ldr r0, [sp, 0x4]\n\
	adds r1, r0, r2\n\
	movs r0, 0\n\
	strh r0, [r1]\n\
	mov r9, r0\n\
	adds r6, r4, 0\n\
	mov r10, r2\n\
	str r3, [sp, 0x8]\n\
	b _080EB00C\n\
	.align 2, 0\n\
_080EAF08: .4byte gEasyChatStruct\n\
_080EAF0C: .4byte 0x0000446c\n\
_080EAF10: .4byte gEasyChatWordsByLetter\n\
_080EAF14:\n\
	ldr r2, _080EAFAC @ =gEasyChatWordsAlphabetized\n\
	adds r0, r6, 0\n\
	adds r1, r0, 0x1\n\
	lsls r1, 16\n\
	lsrs r6, r1, 16\n\
	lsls r0, 1\n\
	adds r0, r2\n\
	ldrh r1, [r0]\n\
	adds r4, r1, 0\n\
	ldr r0, _080EAFB0 @ =0x0000feff\n\
	cmp r1, r0\n\
	bls _080EAFC8\n\
	movs r0, 0xFF\n\
	ands r1, r0\n\
	movs r3, 0\n\
	subs r0, r1, 0x1\n\
	lsls r0, 16\n\
	lsrs r5, r0, 16\n\
	ldr r0, _080EAFB4 @ =0x0000ffff\n\
	cmp r5, r0\n\
	beq _080EB00C\n\
	lsls r0, r7, 4\n\
	adds r0, r7\n\
	lsls r0, 3\n\
	subs r0, r7\n\
	lsls r0, 2\n\
	mov r8, r0\n\
_080EAF4A:\n\
	adds r1, r6, 0\n\
	adds r0, r1, 0x1\n\
	lsls r0, 16\n\
	lsrs r6, r0, 16\n\
	lsls r1, 1\n\
	ldr r0, _080EAFAC @ =gEasyChatWordsAlphabetized\n\
	adds r1, r0\n\
	ldrh r4, [r1]\n\
	lsrs r0, r4, 9\n\
	ldr r1, _080EAFB8 @ =0x000001ff\n\
	adds r2, r1, 0\n\
	adds r1, r4, 0\n\
	ands r1, r2\n\
	str r3, [sp, 0xC]\n\
	bl IsEasyChatIndexAndGroupUnlocked\n\
	lsls r0, 24\n\
	ldr r3, [sp, 0xC]\n\
	cmp r0, 0\n\
	beq _080EAF9E\n\
	cmp r3, 0\n\
	bne _080EAF9E\n\
	ldr r3, _080EAFBC @ =gEasyChatStruct\n\
	ldr r2, [r3]\n\
	mov r1, r9\n\
	adds r0, r1, 0x1\n\
	lsls r0, 16\n\
	lsrs r0, 16\n\
	mov r9, r0\n\
	lsls r1, 1\n\
	add r1, r8\n\
	ldr r3, _080EAFC0 @ =0x00000b78\n\
	adds r0, r2, r3\n\
	adds r0, r1\n\
	strh r4, [r0]\n\
	ldr r0, _080EAFC4 @ =0x0000446c\n\
	adds r2, r0\n\
	add r2, r10\n\
	ldrh r0, [r2]\n\
	adds r0, 0x1\n\
	strh r0, [r2]\n\
	movs r3, 0x1\n\
_080EAF9E:\n\
	subs r0, r5, 0x1\n\
	lsls r0, 16\n\
	lsrs r5, r0, 16\n\
	ldr r1, _080EAFB4 @ =0x0000ffff\n\
	cmp r5, r1\n\
	bne _080EAF4A\n\
	b _080EB00C\n\
	.align 2, 0\n\
_080EAFAC: .4byte gEasyChatWordsAlphabetized\n\
_080EAFB0: .4byte 0x0000feff\n\
_080EAFB4: .4byte 0x0000ffff\n\
_080EAFB8: .4byte 0x000001ff\n\
_080EAFBC: .4byte gEasyChatStruct\n\
_080EAFC0: .4byte 0x00000b78\n\
_080EAFC4: .4byte 0x0000446c\n\
_080EAFC8:\n\
	lsrs r0, r4, 9\n\
	ldr r3, _080EB030 @ =0x000001ff\n\
	adds r2, r3, 0\n\
	adds r1, r4, 0\n\
	ands r1, r2\n\
	bl IsEasyChatIndexAndGroupUnlocked\n\
	lsls r0, 24\n\
	cmp r0, 0\n\
	beq _080EB00C\n\
	ldr r0, _080EB034 @ =gEasyChatStruct\n\
	ldr r2, [r0]\n\
	mov r1, r9\n\
	adds r0, r1, 0x1\n\
	lsls r0, 16\n\
	lsrs r0, 16\n\
	mov r9, r0\n\
	lsls r1, 1\n\
	lsls r0, r7, 4\n\
	adds r0, r7\n\
	lsls r0, 3\n\
	subs r0, r7\n\
	lsls r0, 2\n\
	adds r1, r0\n\
	ldr r3, _080EB038 @ =0x00000b78\n\
	adds r0, r2, r3\n\
	adds r0, r1\n\
	strh r4, [r0]\n\
	ldr r0, _080EB03C @ =0x0000446c\n\
	adds r2, r0\n\
	add r2, r10\n\
	ldrh r0, [r2]\n\
	adds r0, 0x1\n\
	strh r0, [r2]\n\
_080EB00C:\n\
	ldr r1, [sp]\n\
	cmp r6, r1\n\
	bcs _080EB014\n\
	b _080EAF14\n\
_080EB014:\n\
	ldr r3, [sp, 0x8]\n\
	lsls r0, r3, 16\n\
	lsrs r7, r0, 16\n\
	cmp r7, 0x1A\n\
	bhi _080EB020\n\
	b _080EAEE4\n\
_080EB020:\n\
	add sp, 0x10\n\
	pop {r3-r5}\n\
	mov r8, r3\n\
	mov r9, r4\n\
	mov r10, r5\n\
	pop {r4-r7}\n\
	pop {r0}\n\
	bx r0\n\
	.align 2, 0\n\
_080EB030: .4byte 0x000001ff\n\
_080EB034: .4byte gEasyChatStruct\n\
_080EB038: .4byte 0x00000b78\n\
_080EB03C: .4byte 0x0000446c\n\
    .syntax divided\n");
}
#else
void SetUnlockedWordsByAlphabet(void)
{
    u16 i;
    u16 unlockedWordIndex;
    u16 wordListIndex;
    u16 numWords;
    bool8 hasUnlockedWord;

    for (i = 0; i < 27; i++)
    {
        u16 firstWordIndex = gEasyChatWordsByLetter[i];
        u16 nextLetterWordIndex = gEasyChatWordsByLetter[i + 1];

        gEasyChatScreen->numUnlockedWordsByAlphabet[i] = 0;
        unlockedWordIndex = 0;
        wordListIndex = firstWordIndex;
        while (wordListIndex < nextLetterWordIndex)
        {
            u16 wordId = gEasyChatWordsAlphabetized[wordListIndex++];

            if (wordId > 0xFEFF)
            {
                numWords = wordId & 0xFF;
                hasUnlockedWord = FALSE;
                while (--numWords != 0xFFFF)
                {
                    wordId = gEasyChatWordsAlphabetized[wordListIndex++];
                    if (IsEasyChatIndexAndGroupUnlocked(EC_GROUP(wordId), EC_INDEX(wordId)) && !hasUnlockedWord)
                    {
                        gEasyChatScreen->alphabetizedWords[i][unlockedWordIndex++] = wordId;
                        gEasyChatScreen->numUnlockedWordsByAlphabet[i]++;
                        hasUnlockedWord = TRUE;
                    }
                }
            }
            else
            {
                if (IsEasyChatIndexAndGroupUnlocked(EC_GROUP(wordId), EC_INDEX(wordId)))
                {
                    gEasyChatScreen->alphabetizedWords[i][unlockedWordIndex++] = wordId;
                    gEasyChatScreen->numUnlockedWordsByAlphabet[i]++;
                }
            }
        }
    }
}
#endif

// loads strings of all easy chat words except for the species and move names.
void LoadEasyChatStrings(void)
{
    u16 group;
    u16 index;

    for (group = 0; group < EC_NUM_GROUPS; group++)
    {
        if (group != EC_GROUP_POKEMON
         && group != EC_GROUP_POKEMON_NATIONAL
         && group != EC_GROUP_MOVE_1
         && group != EC_GROUP_MOVE_2)
        {
            const u8 *wordText = gEasyChatGroupWords[group];

            index = 0;
            while (*wordText != EOS)
            {
                gEasyChatScreen->wordStrings[group][index] = wordText;

                // Find the end of the current word
                while (*wordText != EOS)
                    wordText++;
                // Skip over the EOS
                wordText++;

                index++;
            }
        }
    }
}

void SetSelectedWordGroup(void)
{
    const u16 *groupWordOrder;
    u16 column;
    u16 row;
    u16 wordIndex;

    if (gEasyChatScreen->inAlphabetMode == 0)
    {
        u16 group = gEasyChatScreen->selectedGroupId;

        gEasyChatScreen->numUnlockedWordsByGroup[group] = 0;
        column = 0;
        row = 0;
        groupWordOrder = gEasyChatGroupOrders[group];
        for (wordIndex = 0; wordIndex < gEasyChatGroupSizes[gEasyChatScreen->selectedGroupId]; wordIndex++)
        {
            if (IsEasyChatIndexAndGroupUnlocked(group, groupWordOrder[wordIndex]))
            {
                gEasyChatScreen->selectedGroupWords[row][column] = ((group & 0x7F) << 9) | (groupWordOrder[wordIndex] & 0x1FF);
                gEasyChatScreen->numUnlockedWordsByGroup[group]++;
                column++;
                if (column > 1)
                {
                    column = 0;
                    row++;
                }
            }
        }
    }
    else
    {
        u8 alphabetGroupId = gEasyChatScreen->selectedGroupId;
        u16 column = 0;
        u16 row = 0;
        u16 wordIndex;

        for (wordIndex = 0; wordIndex < gEasyChatScreen->numUnlockedWordsByAlphabet[alphabetGroupId]; wordIndex++)
        {
            gEasyChatScreen->selectedGroupWords[row][column] = gEasyChatScreen->alphabetizedWords[alphabetGroupId][wordIndex];
            column++;
            if (column > 1)
            {
                column = 0;
                row++;
            }
        }
    }
}

u8 *CopyEasyChatWordPadded(u8 *dest, u16 easyChatWord, u16 totalChars)
{
    u8 *wordText;

    if (IsEasyChatWordInvalid(easyChatWord))
    {
        wordText = StringCopy(dest, gText_ThreeQuestionMarks);
    }
    else if (easyChatWord == 0xFFFF)
    {
        wordText = dest;
        wordText[0] = EOS;  // Why? It's going to get overwritten.
    }
    else
    {
        u16 group = EC_GROUP(easyChatWord);
        u16 index = EC_INDEX(easyChatWord);

        switch (group)
        {
        case EC_GROUP_POKEMON:
        case EC_GROUP_POKEMON_NATIONAL:
            wordText = StringCopy(dest, gSpeciesNames[index]);
            break;
        case EC_GROUP_MOVE_1:
        case EC_GROUP_MOVE_2:
            wordText = StringCopy(dest, gMoveNames[index]);
            break;
        default:
            wordText = StringCopy(dest, gEasyChatScreen->wordStrings[group][index]);
            break;
        }
    }

    wordText[0] = EXT_CTRL_CODE_BEGIN;
    wordText[1] = 0x13;
    wordText[2] = totalChars * 8;
    wordText += 3;

    *wordText = EOS;

    return wordText;
}

u16 GetEasyChatWordStringLength(u16 easyChatWord)
{
    const u8 *wordText;
    u16 length;

    if (IsEasyChatWordInvalid(easyChatWord))
    {
        return StringLength(gText_ThreeQuestionMarks);
    }
    else if (easyChatWord == 0xFFFF)
    {
        return 0;
    }
    else
    {
        u16 groupId = EC_GROUP(easyChatWord);
        u16 index = EC_INDEX(easyChatWord);

        switch (groupId)
        {
        case EC_GROUP_POKEMON:
        case EC_GROUP_POKEMON_NATIONAL:
            wordText = gSpeciesNames[index];
            break;
        case EC_GROUP_MOVE_1:
        case EC_GROUP_MOVE_2:
            wordText = gMoveNames[index];
            break;
        default:
            wordText = gEasyChatScreen->wordStrings[groupId][index];
            break;
        }
    }

    length = 0;
    while (*wordText != EOS)
    {
        wordText++;
        length++;
    }
    return length;
}

bool8 IsEasyChatWordInvalid(u16 easyChatWord)
{
    const u16 *wordList;
    u16 i;

    if (easyChatWord == 0xFFFF)
    {
        return FALSE;
    }
    else
    {
        u16 groupId = EC_GROUP(easyChatWord);
        u16 index = EC_INDEX(easyChatWord);

        if (groupId < EC_NUM_GROUPS)
        {
            switch (groupId)
            {
            case EC_GROUP_POKEMON:
            case EC_GROUP_POKEMON_NATIONAL:
            case EC_GROUP_MOVE_1:
            case EC_GROUP_MOVE_2:
                wordList = (u16 *)gEasyChatGroupWords[groupId];
                for (i = 0; i < gEasyChatGroupSizes[groupId]; i++)
                {
                    if (index == wordList[i])
                        return FALSE;
                }
                break;
            default:
                if (index < gEasyChatGroupSizes[groupId])
                    return FALSE;
                break;
            }
        }
        return TRUE;
    }
}

#if GERMAN
u32 GetEasyChatWordIndexInGroup(s32 group, s32 index)
{
    s32 i;
    u16 *words;
    u32 wordIndex = 0;

    switch (group)
    {
    case EC_GROUP_POKEMON:
    case EC_GROUP_POKEMON_NATIONAL:
    case EC_GROUP_MOVE_1:
    case EC_GROUP_MOVE_2:
        words = (u16 *)gEasyChatGroupWords[group];
        for (i = 0; i < gEasyChatGroupSizes[group]; i++)
        {
            if (index == words[i])
            {
                wordIndex = i;
                break;
            }
        }
        break;
    default:
        wordIndex = index;
        break;
    }

    return wordIndex;
}
#endif

// returns the end of the destination buffer text
u8 *CopyEasyChatWord(u8 *dest, u16 easyChatWord)
{
    u16 groupId;
    u16 wordIndex;
    const u8 *src;
    u16 i;

    if (IsEasyChatWordInvalid(easyChatWord))
        return StringCopy(dest, gText_ThreeQuestionMarks);

    if (easyChatWord == 0xFFFF)
    {
        *dest = EOS;
        return dest;
    }
    else
    {
        groupId = EC_GROUP(easyChatWord);
        wordIndex = EC_INDEX(easyChatWord);
        switch (groupId)
        {
        case EC_GROUP_POKEMON: // 0
        case EC_GROUP_POKEMON_NATIONAL: // 21
            dest = StringCopy(dest, gSpeciesNames[wordIndex]);
            break;
        case EC_GROUP_MOVE_1: // 18
        case EC_GROUP_MOVE_2: // 19
            dest = StringCopy(dest, gMoveNames[wordIndex]);
            break;
        default:
            src = gEasyChatGroupWords[groupId];
            for (i = wordIndex - 1; i != 0xFFFF; i--)
            {
                while (*src++ != EOS)
                    ;
            }
            dest = StringCopy(dest, src);
            break;
        }
        *dest = EOS;
        return dest;
    }
}

u8 *ConvertEasyChatWordsToString(u8 *dest, u16 *src, u16 columns, u16 rows)
{
    u16 row;
    u16 column;

    const u16 numColumns = columns - 1;

    for (row = 0; row < rows; row++)
    {
        u16 easyChatWord;

        for (column = 0; column < numColumns; column++)
        {
            dest = CopyEasyChatWord(dest, src[0]);

            if (src[0] != 0xFFFF)
            {
                dest[0] = CHAR_SPACE;
                dest++;
            }

            src++;
        }

        easyChatWord = src[0];
        src++;
        dest = CopyEasyChatWord(dest, easyChatWord);

        dest[0] = CHAR_NEWLINE;
        dest++;
    }

    dest--;
    dest[0] = EOS;

    return dest;
}

u8 *ConvertEasyChatWordsToStringWithScroll(u8 *dest, u16 *src, u16 columns, u16 rows)
{
    u16 row;
    u16 column;

    const u16 numColumns = columns - 1;

    for (row = 0; row < rows; row++)
    {
        u16 easyChatWord;

        for (column = 0; column < numColumns; column++)
        {
            dest = CopyEasyChatWord(dest, src[0]);

            if (src[0] != 0xFFFF)
            {
                dest[0] = CHAR_SPACE;
                dest++;
            }

            src++;
        }

        easyChatWord = src[0];
        src++;
        dest = CopyEasyChatWord(dest, easyChatWord);

        // Only difference with ConvertEasyChatWordsToString
        dest[0] = (row == 0) ? CHAR_NEWLINE : CHAR_PROMPT_SCROLL;
        dest++;
    }

    dest--;
    dest[0] = EOS;

    return dest;
}


u16 UnusedGetEasyChatWordStringLength(u16 easyChatWord)
{
    const u8 *wordText;
    u16 i;
    u16 length;
    int groupId;
    int wordIndex;


    if (easyChatWord == 0xFFFF)
        return 0;

    groupId = EC_GROUP(easyChatWord);
    wordIndex = EC_INDEX(easyChatWord);
    switch (groupId)
    {
    case EC_GROUP_POKEMON: // 0
    case EC_GROUP_POKEMON_NATIONAL: // 21
        wordText = gSpeciesNames[wordIndex];
        break;
    case EC_GROUP_MOVE_1: // 18
    case EC_GROUP_MOVE_2: // 19
        wordText = gMoveNames[wordIndex];
        break;
    default:
        wordText = gEasyChatGroupWords[groupId];
        for (i = wordIndex - 1; i != 0xFFFF; i--)
        {
            while (*wordText++ != EOS)
                ;
        }
        break;
    }

    length = 0;
    while (*wordText != EOS)
    {
        wordText++;
        length++;
    }

    return length;
}

bool8 CanPhraseFitInXRowsYCols(u16 *easyChatWords, u16 numRows, u16 numColumns, u16 maxLength)
{
    return FALSE;
}

void UnusedClearTvShowEasyChatWords(u8 showType, u16 showIndex)
{
    u16 *words;
    u16 wordCount;

    // FIXME: find actual tv shows used
    switch (showType)
    {
    case 5:
        wordCount = 6;
        words = (u16*)((void *)&gSaveBlock1.tvShows[showIndex] + 0x04);
        break;
    case 7:
        wordCount = 2;
        words = (u16*)((void *)&gSaveBlock1.tvShows[showIndex] + 0x1C);
        break;
    case 8:
        wordCount = 1;
        words = (u16*)((void *)&gSaveBlock1.tvShows[showIndex] + 0x02);
        break;
    default:
        return;
    }

    wordCount -= 1;
    while (wordCount != 0xFFFF)
    {
        *words = -1;
        words++;
        wordCount -= 1;
    }
}

void InitializeEasyChatWordArray(u16 *words, u16 length)
{
    u16 i;

    for (i = length - 1; i != 0xFFFF; i--)
    {
        *words = 0xFFFF;
        words++;
    }

}

u16 GetRandomEasyChatWordFromGroup(u16 groupId)
{
    u16 index = Random() % gEasyChatGroupSizes[groupId];

    if (groupId == EC_GROUP_POKEMON
     || groupId == EC_GROUP_POKEMON_NATIONAL
     || groupId == EC_GROUP_MOVE_1
     || groupId == EC_GROUP_MOVE_2)
    {
        index = ((u16 *)gEasyChatGroupWords[groupId])[index];
    }

    return ((groupId & 0x7F) << 9) | (index & 0x1FF);
}

u16 GetRandomEasyChatWordFromUnlockedGroup(u16 groupId)
{
    if (!IsEasyChatGroupUnlocked(groupId))
        return -1;

    if (groupId != EC_GROUP_POKEMON)
    {
        if (groupId == EC_GROUP_TRENDY_SAYING)
            return GetRandomUnlockedTrendySaying();
    }
    else
    {
        return GetRandomUnlockedEasyChatPokemon();
    }

    return GetRandomEasyChatWordFromGroup(groupId);
}

void ShowEasyChatProfile(void)
{
    u16 *easyChatWords;
    u16 columns;
    u16 rows;

    switch (gSpecialVar_0x8004)
    {
    case EASY_CHAT_TYPE_PROFILE:
        easyChatWords = gSaveBlock1.easyChats.easyChatProfile;
        columns = 2;
        rows = 2;
        break;
    case EASY_CHAT_TYPE_BATTLE_START:
        easyChatWords = gSaveBlock1.easyChats.easyChatBattleStart;
        if (CanPhraseFitInXRowsYCols(gSaveBlock1.easyChats.easyChatBattleStart, 3, 2, 20))
        {
            columns = 2;
            rows = 3;
        }
        else
        {
            columns = 3;
            rows = 2;
        }
        break;
    case EASY_CHAT_TYPE_BATTLE_WON:
        easyChatWords = gSaveBlock1.easyChats.easyChatBattleWon;
        columns = 3;
        rows = 2;
        break;
    case EASY_CHAT_TYPE_BATTLE_LOST:
        easyChatWords = gSaveBlock1.easyChats.easyChatBattleLost;
        columns = 3;
        rows = 2;
        break;
    default:
        return;
    }

    ConvertEasyChatWordsToString(gStringVar4, easyChatWords, columns, rows);
    ShowFieldAutoScrollMessage(gStringVar4);
}

void BufferDeepLinkPhrase(void)
{
    u16 groupId;
    u16 easyChatWord;

    if (Random() & 1)
        groupId = EC_GROUP_HOBBIES;
    else
        groupId = EC_GROUP_LIFESTYLE;

    easyChatWord = GetRandomEasyChatWordFromUnlockedGroup(groupId);
    CopyEasyChatWord(gStringVar2, easyChatWord);
}

static bool8 IsTrendySayingUnlocked(u8 wordIndex)
{
    int byteOffset;
    int shift;

    byteOffset = wordIndex / 8;
    shift = wordIndex % 8;
    return (gSaveBlock1.unlockedTrendySayings[byteOffset] >> shift) & 1;
}

void UnlockTrendySaying(u8 wordIndex)
{
    int byteOffset;
    int shift;

    if (wordIndex < NUM_TRENDY_SAYINGS)
    {
        byteOffset = wordIndex / 8;
        shift = wordIndex % 8;
        gSaveBlock1.unlockedTrendySayings[byteOffset] |= 1 << shift;
    }
}

static u8 GetNumTrendySayingsUnlocked(void)
{
    u8 i;
    u8 numUnlocked;

    for (i = 0, numUnlocked = 0; i < NUM_TRENDY_SAYINGS; i++)
    {
        if (IsTrendySayingUnlocked(i))
            numUnlocked++;
    }
    return numUnlocked;
}

u16 UnlockRandomTrendySaying(void)
{
    u16 i;
    u16 numUnlocked;
    u16 numToSkip;

    numUnlocked = GetNumTrendySayingsUnlocked();
    if (numUnlocked == NUM_TRENDY_SAYINGS)
        return -1;

    numToSkip = Random() % (NUM_TRENDY_SAYINGS - numUnlocked);
    for (i = 0; i < NUM_TRENDY_SAYINGS; i++)
    {
        if (IsTrendySayingUnlocked(i) == 0)
        {
            if (numToSkip == 0)
            {
                UnlockTrendySaying(i);
                return (i & 0x1FF) | 0x2800;
            }
            numToSkip--;
        }
    }
    return -1;
}

static u16 GetRandomUnlockedTrendySaying(void)
{
    u16 i;
    u16 numUnlocked;

    numUnlocked = GetNumTrendySayingsUnlocked();
    if (numUnlocked == 0)
        return -1;

    numUnlocked = Random() % numUnlocked;
    for (i = 0; i < NUM_TRENDY_SAYINGS; i++)
    {
        if (IsTrendySayingUnlocked(i))
        {
            if (numUnlocked == 0)
                return (i & 0x1FF) | 0x2800;
            numUnlocked--;
        }
    }

    return -1;
}

u8 EasyChatIsNationalPokedexEnabled(void)
{
    return IsNationalPokedexEnabled();
}

static u16 GetRandomUnlockedEasyChatPokemon(void)
{
    u16 *speciesList;
    u16 index;
    u16 i;
    u8 numWords;

    index = EasyChat_GetNumWordsInGroup(EC_GROUP_POKEMON);

    if (index == 0)
        return -1;

    index = Random() % index;
    speciesList = (u16 *)gEasyChatGroupWords[EC_GROUP_POKEMON];
    for (i = 0, numWords = gEasyChatGroupSizes[EC_GROUP_POKEMON]; i < numWords; i++)
    {
        const u16 dexNum = SpeciesToNationalPokedexNum(*speciesList);
        const u8 local2 = GetSetPokedexFlag(dexNum, 0);

        if (local2)
        {
            if (index == 0)
                return *speciesList & 0x1FF;
            index--;
        }
        speciesList++;
    }
    return -1;
}
