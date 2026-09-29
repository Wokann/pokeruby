#include "global.h"
#include "palette.h"
#include "constants/songs.h"
#include "sound.h"
#include "field_weather.h"
#include "overworld.h"
#include "field_fadetransition.h"
#include "menu.h"
#include "main.h"
#include "strings.h"
#include "string_util.h"
#include "event_data.h"
#include "script.h"
#include "pokemon_storage_system.h"

static void CreateMainMenu(u8 whichMenu);
static void ChooseBoxMenu_CreateSprites(u8 curBox);
static void ChooseBoxMenu_DestroySprites(void);
static void ChooseBoxMenu_MoveRight(void);
static void ChooseBoxMenu_MoveLeft(void);
static void ChooseBoxMenu_PrintInfo(void);
static void DrawChooseBoxMenuText(const u8 *text, u16 tileX, u16 tileY);
static void SpriteCB_ChooseBoxArrow(struct Sprite *sprite);

EWRAM_DATA static u8 sPreviousBoxOption = 0;
EWRAM_DATA static struct ChooseBoxMenu *sChooseBoxMenu = NULL;

static const struct StorageMenuText sMainMenuTexts[] = {
    {PCText_WithdrawPoke, PCText_MovePokeToParty},
    {PCText_DepositPoke,  PCText_StorePokeInBox},
    {PCText_MovePoke,     PCText_OrganizeBoxesParty},
    {PCText_SeeYa,        PCText_ReturnToPrevMenu}
};

#if DEBUG
static const u16 sDebugBoxMonSpecies[2] = {0};
#endif

static const union AnimCmd sAnim_ChooseBoxMenu_TopLeft[] = {
    ANIMCMD_FRAME( 0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ChooseBoxMenu_BottomLeft[] = {
    ANIMCMD_FRAME( 4, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ChooseBoxMenu_TopRight[] = {
    ANIMCMD_FRAME( 6, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_ChooseBoxMenu_BottomRight[] = {
    ANIMCMD_FRAME(10, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_ChooseBoxMenu[] = {
    sAnim_ChooseBoxMenu_TopLeft,
    sAnim_ChooseBoxMenu_BottomLeft,
    sAnim_ChooseBoxMenu_TopRight,
    sAnim_ChooseBoxMenu_BottomRight
};

static const union AffineAnimCmd sAffineAnim_ChooseBoxMenu[] = {
    AFFINEANIMCMD_FRAME(0xe0, 0xe0, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_ChooseBoxMenu[] = {
    sAffineAnim_ChooseBoxMenu
};

static const u16 sChooseBoxMenu_Pal[] = INCBIN_U16("graphics/pokemon_storage/box_selection_popup.gbapal");

static const u8 sChooseBoxMenuCenter_Gfx[] = INCBIN_U8("graphics/pokemon_storage/box_selection_popup_center.4bpp");

static const u8 sChooseBoxMenuSides_Gfx[] = INCBIN_U8("graphics/pokemon_storage/box_selection_popup_sides.4bpp");

u8 CountMonsInBox(u8 boxId)
{
    u16 i;
    u16 count;

    for (i = 0, count = 0; i < IN_BOX_COUNT; i++)
    {
        if (GetBoxMonData(gPokemonStorage.boxes[boxId] + i, MON_DATA_SPECIES) != 0)
            count++;
    }
    return count;
}

s16 GetFirstFreeBoxSpot(u8 boxId)
{
    u16 i;

    for (i = 0; i < IN_BOX_COUNT; i++)
    {
        if (GetBoxMonData(gPokemonStorage.boxes[boxId] + i, MON_DATA_SPECIES) == 0)
            return i;
    }
    return -1;
}

u8 CountPartyNonEggMons(void)
{
    u16 i;
    u16 count;

    for (i = 0, count = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *pokemon = gPlayerParty + i;
        if (GetMonData(pokemon, MON_DATA_SPECIES) != 0 && !GetMonData(pokemon, MON_DATA_IS_EGG))
            count++;
    }
    return count;
}

u8 CountPartyAliveNonEggMonsExcept(u8 slotToIgnore)
{
    u16 i;
    u16 count;

    for (i = 0, count = 0; i < PARTY_SIZE; i++)
    {
        if (i != slotToIgnore)
        {
            struct Pokemon *pokemon = gPlayerParty + i;
            if (GetMonData(pokemon, MON_DATA_SPECIES) != 0 && !GetMonData(pokemon, MON_DATA_IS_EGG) && GetMonData(pokemon, MON_DATA_HP) != 0)
                count++;
        }
    }
    return count;
}

u8 CountPartyAliveNonEggMons_IgnoreVar0x8004Slot(void)
{
    return CountPartyAliveNonEggMonsExcept(gSpecialVar_0x8004);
}

u8 CountPartyMons(void)
{
    u16 i;
    u16 count;

    for (i = 0, count = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(gPlayerParty + i, MON_DATA_SPECIES) != 0)
            count++;
    }
    return count;
}

u8 *StringCopyAndFillWithSpaces(u8 *dst, const u8 *src, u16 n)
{
    u8 *str = StringCopy(dst, src);
    while (str < dst + n)
    {
        *str++ = CHAR_SPACE;
    }
    *str = EOS;
    return str;
}

void CopyTilemapRect(u16 *dest, u16 destLeft, u16 destTop, const u16 *src, u16 srcLeft, u16 srcTop, u16 destWidth, u16 destHeight, u16 srcWidth)
{
    u16 i;

    destWidth *= 2;
    dest += destTop * 0x20 + destLeft;
    src += srcTop * srcWidth + srcLeft;
    for (i = 0; i < destHeight; i++)
    {
        CpuCopy16(src, dest, destWidth);
        dest += 0x20;
        src += srcWidth;
    }
}

#define MAX_DMA_BLOCK_SIZE 0x1000
#define Dma3FillLarge_(value, dest, size, bit)             \
{                                                          \
    void *_dest = dest;                                    \
    u32 _size = size;                                      \
    while (1)                                              \
    {                                                      \
        if (_size <= MAX_DMA_BLOCK_SIZE)                   \
        {                                                  \
            DmaFill##bit(3, value, _dest, _size);          \
            break;                                         \
        }                                                  \
        DmaFill##bit(3, value, _dest, MAX_DMA_BLOCK_SIZE); \
        _dest += MAX_DMA_BLOCK_SIZE;                       \
        _size -= MAX_DMA_BLOCK_SIZE;                       \
    }                                                      \
}

#define Dma3FillLarge16_(value, dest, size) Dma3FillLarge_(value, dest, size, 16)
#define Dma3FillLarge32_(value, dest, size) Dma3FillLarge_(value, dest, size, 32)

void UnusedClearTilemapRect(u16 *dest, u16 destLeft, u16 destTop, u16 width, u16 height)
{
    u16 i;

    dest += destTop * 0x20 + destLeft;
    width *= 2;
    for (i = 0; i < height; dest += 0x20, i++)
        Dma3FillLarge16_(0, dest, width);
}

s16 AdvanceStorageMonIndex(struct BoxPokemon *boxMons, s8 currIndex, u8 maxIndex, u8 mode)
{
    s16 i;
    s16 direction;
    if (mode == 0 || mode == 2)
    {
        direction = 1;
    }
    else
    {
        direction = -1;
    }
    if (mode == 2 || mode == 3)
    {
        for (i = currIndex + direction; i >= 0 && i <= maxIndex; i += direction)
        {
            if (GetBoxMonData(boxMons + i, MON_DATA_SPECIES) != 0)
                return i;
        }
    }
    else
    {
        for (i = currIndex + direction; i >= 0 && i <= maxIndex; i += direction)
        {
            if (GetBoxMonData(boxMons + i, MON_DATA_SPECIES) != 0 && !GetBoxMonData(boxMons + i, MON_DATA_IS_EGG))
                return i;
        }
    }
    return -1;
}

static void ClearBottomWindow(void)
{
    Menu_BlankWindowRect(2, 15, 27, 18);
}

enum
{
    PC_MAIN_MENU_STATE_LOAD,
    PC_MAIN_MENU_STATE_FADE_IN,
    PC_MAIN_MENU_STATE_HANDLE_INPUT,
    PC_MAIN_MENU_STATE_ERROR_MSG,
    PC_MAIN_MENU_STATE_ENTER_STORAGE,
};

enum
{
    PC_MENU_OPTION_WITHDRAW,
    PC_MENU_OPTION_DEPOSIT,
    PC_MENU_OPTION_MOVE,
    PC_MENU_OPTION_EXIT,
};

#define tState          data[0]
#define tSelectedOption data[1]
#define tInput          data[2]
#define tNextOption     data[3]

static void Task_PCMainMenu(u8 taskId)
{
    struct Task *task = gTasks + taskId;
    switch (task->tState)
    {
        case PC_MAIN_MENU_STATE_LOAD:
            CreateMainMenu(task->tSelectedOption);
            Menu_DisplayDialogueFrame();
            Menu_PrintText(sMainMenuTexts[task->tSelectedOption].desc, 2, 15);
            task->tState++;
            break;
        case PC_MAIN_MENU_STATE_FADE_IN:
            if (IsWeatherNotFadingIn())
            {
                task->tState++;
            }
            break;
        case PC_MAIN_MENU_STATE_HANDLE_INPUT:
            task->tInput = Menu_ProcessInput();
            switch(task->tInput)
            {
                case -2:
                    task->tNextOption = task->tSelectedOption;
                    if (JOY_NEW(DPAD_UP) && --task->tNextOption < 0)
                        task->tNextOption = PC_MENU_OPTION_EXIT;

                    if (JOY_NEW(DPAD_DOWN) && ++task->tNextOption > PC_MENU_OPTION_EXIT)
                        task->tNextOption = PC_MENU_OPTION_WITHDRAW;
                    if (task->tSelectedOption != task->tNextOption)
                    {
                        task->tSelectedOption = task->tNextOption;
                        ClearBottomWindow();
                        Menu_PrintText(sMainMenuTexts[task->tSelectedOption].desc, 2, 15);
                    }
                    break;
                case -1:
                case PC_MENU_OPTION_EXIT:
                    Menu_DestroyCursor();
                    Menu_EraseWindowRect(0, 0, 13, 9);
                    UnlockPlayerFieldControls();
                    ScriptContext_Enable();
                    DestroyTask(taskId);
                    break;
                default:
                    if (task->tInput == PC_MENU_OPTION_WITHDRAW && CountPartyMons() == PARTY_SIZE)
                    {
                        ClearBottomWindow();
                        Menu_PrintText(gPCText_PartyFull2, 2, 15);
                        task->tState = PC_MAIN_MENU_STATE_ERROR_MSG;
                    }
                    else if (task->tInput == PC_MENU_OPTION_DEPOSIT && CountPartyMons() == 1)
                    {
                        ClearBottomWindow();
                        Menu_PrintText(gPCText_OnlyOne, 2, 15);
                        task->tState = PC_MAIN_MENU_STATE_ERROR_MSG;
                    }
                    else
                    {
                        FadeScreen(1, 0);
                        task->tState = PC_MAIN_MENU_STATE_ENTER_STORAGE;
                    }
                    break;
            }
            break;
        case PC_MAIN_MENU_STATE_ERROR_MSG:
            if (JOY_NEW(A_BUTTON | B_BUTTON))
            {
                ClearBottomWindow();
                Menu_PrintText(sMainMenuTexts[task->tSelectedOption].desc, 2, 15);
                task->tState = PC_MAIN_MENU_STATE_HANDLE_INPUT;
            }
            else if (JOY_NEW(DPAD_UP))
            {
                if (--task->tSelectedOption < 0)
                    task->tSelectedOption = PC_MENU_OPTION_EXIT;
                Menu_MoveCursor(-1);
                task->tSelectedOption = Menu_GetCursorPos();
                ClearBottomWindow();
                Menu_PrintText(sMainMenuTexts[task->tSelectedOption].desc, 2, 15);
                task->tState = PC_MAIN_MENU_STATE_HANDLE_INPUT;
            }
            else if (JOY_NEW(DPAD_DOWN))
            {
                if (++task->tSelectedOption > PC_MENU_OPTION_EXIT)
                    task->tSelectedOption = PC_MENU_OPTION_WITHDRAW;
                Menu_MoveCursor(1);
                task->tSelectedOption = Menu_GetCursorPos();
                ClearBottomWindow();
                Menu_PrintText(sMainMenuTexts[task->tSelectedOption].desc, 2, 15);
                task->tState = PC_MAIN_MENU_STATE_HANDLE_INPUT;
            }
            break;
        case PC_MAIN_MENU_STATE_ENTER_STORAGE:
            if (!gPaletteFade.active)
            {
                EnterPokeStorage(task->tInput);
                DestroyTask(taskId);
            }
            break;
    }
}

void ShowPokemonStorageSystemPC(void)
{
    u8 taskId = CreateTask(Task_PCMainMenu, 80);
    gTasks[taskId].tState = PC_MAIN_MENU_STATE_LOAD;
    gTasks[taskId].tSelectedOption = PC_MENU_OPTION_WITHDRAW;
    LockPlayerFieldControls();
}

static void FieldTask_ReturnToPcMenu(void)
{
    u8 taskId = CreateTask(Task_PCMainMenu, 80);
    gTasks[taskId].tState = PC_MAIN_MENU_STATE_LOAD;
    gTasks[taskId].tSelectedOption = sPreviousBoxOption;
    FadeInFromBlack();
}

#undef tState
#undef tSelectedOption
#undef tInput
#undef tNextOption

static void CreateMainMenu(u8 whichMenu)
{
    Menu_DrawStdWindowFrame(0, 0, 13, 9);
    Menu_PrintItems(1, 1, PC_MENU_OPTION_EXIT + 1, (const struct MenuAction *)sMainMenuTexts);
    InitMenu(0, 1, 1, PC_MENU_OPTION_EXIT + 1, whichMenu, 12);
}

void CB2_ExitPokeStorage(void)
{
    sPreviousBoxOption = gPokemonStorageSystemPtr->boxOption;
    gFieldCallback = FieldTask_ReturnToPcMenu;
    SetMainCallback2(CB2_ReturnToField);
}

void ResetPokemonStorageSystem(void)
{
    u16 boxId;
    u16 boxPosition;

    gPokemonStorage.currentBox = 0;
    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            ZeroBoxMonData(gPokemonStorage.boxes[boxId] + boxPosition);
        }
    }
    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        u8 *dest = StringCopy(gPokemonStorage.boxNames[boxId], gPCText_BOX);
        ConvertIntToDecimalStringN(dest, boxId + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
    }
    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        gPokemonStorage.wallpaper[boxId] = boxId & 0x03;
    }
}

#if DEBUG
NAKED
void Debug_FillPokemonStorage(void)
{
	asm("\
	push	{r4, r5, r6, r7, lr}\n\
	mov	r7, sl\n\
	mov	r6, r9\n\
	mov	r5, r8\n\
	push	{r5, r6, r7}\n\
	add	sp, sp, #0xffffffe4\n\
	mov	r0, #0x0\n\
	mov	r8, r0\n\
	mov	r7, r8\n\
	mov	r1, #0x14\n\
	str	r1, [sp, #0x18]\n\
	mov	r2, #0xff\n\
	mov	r9, r2\n\
._162:\n\
	mov	r5, #0x0\n\
	cmp	r7, #0\n\
	beq	._160	@cond_branch\n\
	mov	r0, #0xe\n\
	mov	r8, r0\n\
	b	._158\n\
._160:\n\
	bl	Random\n\
	lsl	r0, r0, #0x10\n\
	lsr	r0, r0, #0x10\n\
	str	r0, [sp, #0x14]\n\
	mov	r2, r8\n\
	lsl	r1, r2, #0x2\n\
	add r1, r1, r8\n\
	lsl	r3, r1, #0x4\n\
	sub	r3, r3, r1\n\
	lsl	r3, r3, #0x5\n\
	lsl	r1, r5, #0x2\n\
	add	r1, r1, r5\n\
	lsl	r1, r1, #0x4\n\
	ldr	r2, ._163       @ gPokemonStorage\n\
	add	r1, r1, r2\n\
	add	r6, r3, r1\n\
	ldr	r2, ._163 + 4   @ sDebugBoxMonSpecies\n\
	lsl	r1, r7, #0x1\n\
	add	r1, r1, r2\n\
	ldrh	r4, [r1]\n\
	add	r5, r5, #0x1\n\
	lsl	r2, r5, #0x18\n\
	lsr	r2, r2, #0x18\n\
	mov	r1, #0x0\n\
	str	r1, [sp]\n\
	str	r1, [sp, #0x4]\n\
	mov	r1, #0x1\n\
	mov	sl, r1\n\
	str	r1, [sp, #0x8]\n\
	str	r0, [sp, #0xc]\n\
	add	r0, r6, #0\n\
	add	r1, r4, #0\n\
	mov	r3, #0x20\n\
	bl	CreateBoxMon\n\
	cmp	r4, #0\n\
	beq	._157	@cond_branch\n\
	cmp	r4, #0xac\n\
	bne	._156	@cond_branch\n\
	add	r0, sp, #0x10\n\
	mov	r2, sl\n\
	strb	r2, [r0]\n\
	add	r0, r6, #0\n\
	mov	r1, #0x2d\n\
	add	r2, sp, #0x10\n\
	bl	SetBoxMonData\n\
._156:\n\
	bl	Random\n\
	mov	r1, r9\n\
	and	r1, r1, r0\n\
	str	r1, [sp, #0x14]\n\
	add	r4, sp, #0x14\n\
	add	r0, r6, #0\n\
	mov	r1, #0x16\n\
	add	r2, r4, #0\n\
	bl	SetBoxMonData\n\
	bl	Random\n\
	mov	r1, r9\n\
	and	r1, r1, r0\n\
	str	r1, [sp, #0x14]\n\
	add	r0, r6, #0\n\
	mov	r1, #0x17\n\
	add	r2, r4, #0\n\
	bl	SetBoxMonData\n\
	bl	Random\n\
	mov	r1, r9\n\
	and	r1, r1, r0\n\
	str	r1, [sp, #0x14]\n\
	add	r0, r6, #0\n\
	mov	r1, #0x18\n\
	add	r2, r4, #0\n\
	bl	SetBoxMonData\n\
	bl	Random\n\
	mov	r1, r9\n\
	and	r1, r1, r0\n\
	str	r1, [sp, #0x14]\n\
	add	r0, r6, #0\n\
	mov	r1, #0x21\n\
	add	r2, r4, #0\n\
	bl	SetBoxMonData\n\
	bl	Random\n\
	mov	r1, r9\n\
	and	r1, r1, r0\n\
	str	r1, [sp, #0x14]\n\
	add	r0, r6, #0\n\
	mov	r1, #0x2f\n\
	add	r2, r4, #0\n\
	bl	SetBoxMonData\n\
	bl	Random\n\
	mov	r1, r9\n\
	and	r1, r1, r0\n\
	str	r1, [sp, #0x14]\n\
	add	r0, r6, #0\n\
	mov	r1, #0x30\n\
	add	r2, r4, #0\n\
	bl	SetBoxMonData\n\
	ldr	r0, [sp, #0x18]\n\
	cmp	r0, #0\n\
	beq	._157	@cond_branch\n\
	sub	r0, r0, #0x1\n\
	lsl	r0, r0, #0x10\n\
	lsr	r0, r0, #0x10\n\
	str	r0, [sp, #0x18]\n\
	mov	r1, sl\n\
	str	r1, [sp, #0x14]\n\
	add	r0, r6, #0\n\
	mov	r1, #0x32\n\
	add	r2, r4, #0\n\
	bl	SetBoxMonData\n\
._157:\n\
	add	r0, r7, #1\n\
	lsl	r0, r0, #0x10\n\
	lsr	r7, r0, #0x10\n\
	lsl	r0, r5, #0x10\n\
	lsr	r5, r0, #0x10\n\
	cmp	r5, #0x1d\n\
	bhi	._158	@cond_branch\n\
	cmp	r7, #0\n\
	bne	._159	@cond_branch\n\
	b	._160\n\
._159:\n\
	mov	r2, #0xe\n\
	mov	r8, r2\n\
._158:\n\
	mov	r0, r8\n\
	add	r0, r0, #0x1\n\
	lsl	r0, r0, #0x10\n\
	lsr	r0, r0, #0x10\n\
	mov	r8, r0\n\
	cmp	r0, #0xd\n\
	bhi	._161	@cond_branch\n\
	b	._162\n\
._161:\n\
	add	sp, sp, #0x1c\n\
	pop	{r3, r4, r5}\n\
	mov	r8, r3\n\
	mov	r9, r4\n\
	mov	sl, r5\n\
	pop	{r4, r5, r6, r7}\n\
	pop	{r0}\n\
	bx	r0\n\
._164:\n\
	.align	2, 0\n\
._163:\n\
	.word	gPokemonStorage+0x4\n\
	.word	sDebugBoxMonSpecies");
}
#endif

void LoadChooseBoxMenuGfx(struct ChooseBoxMenu *menu, u16 tileTag, u16 palTag, u8 subpriority)
{
    struct SpritePalette palette = {
        sChooseBoxMenu_Pal, palTag
    };
    struct SpriteSheet sheets[] = {
        {sChooseBoxMenuCenter_Gfx, 0x800, tileTag},
        {sChooseBoxMenuSides_Gfx,  0x180, tileTag + 1},
        {}
    };
    LoadSpritePalette(&palette);
    LoadSpriteSheets(sheets);
    sChooseBoxMenu = menu;
    menu->tileTag = tileTag;
    menu->paletteTag = palTag;
    menu->subpriority = subpriority;
}

void FreeChooseBoxMenu(void)
{
    FreeSpritePaletteByTag(sChooseBoxMenu->paletteTag);
    FreeSpriteTilesByTag(sChooseBoxMenu->tileTag);
    FreeSpriteTilesByTag(sChooseBoxMenu->tileTag + 1);
}

void CreateChooseBoxMenuSprites(u8 curBox)
{
    ChooseBoxMenu_CreateSprites(curBox);
}

void DestroyChooseBoxMenuSprites(void)
{
    ChooseBoxMenu_DestroySprites();
}

u8 HandleChooseBoxMenuInput(void)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        return BOXID_CANCELED;
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        return sChooseBoxMenu->curBox;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        ChooseBoxMenu_MoveLeft();
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        ChooseBoxMenu_MoveRight();
    }
    return BOXID_NONE_CHOSEN;
}

static void ChooseBoxMenu_CreateSprites(u8 curBox)
{
    u16 i;
    u8 spriteId;
    struct SpriteTemplate template;
    struct OamData oamData = {};
    oamData.size = 3;
    oamData.paletteNum = 1;
    template = (struct SpriteTemplate){
        0, 0, &oamData, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
    };

    sChooseBoxMenu->curBox = curBox;
    template.tileTag = sChooseBoxMenu->tileTag;
    template.paletteTag = sChooseBoxMenu->paletteTag;

    spriteId = CreateSprite(&template, 0xA0, 0x60, 0);
    sChooseBoxMenu->menuSprite = gSprites + spriteId;

    oamData.shape = ST_OAM_V_RECTANGLE;
    oamData.size = 1;
    template.tileTag = sChooseBoxMenu->tileTag + 1;
    template.anims = sAnims_ChooseBoxMenu;
    for (i = 0; i < 4; i++)
    {
        u16 animId;
        spriteId = CreateSprite(&template, 0x7c, 0x50, sChooseBoxMenu->subpriority);
        sChooseBoxMenu->menuSideSprites[i] = gSprites + spriteId;
        animId = 0;
        if (i & 2)
        {
            sChooseBoxMenu->menuSideSprites[i]->x = 0xc4;
            animId = 2;
        }
        if (i & 1)
        {
            sChooseBoxMenu->menuSideSprites[i]->y = 0x70;
            sChooseBoxMenu->menuSideSprites[i]->oam.size = 0;
            animId++;
        }
        StartSpriteAnim(sChooseBoxMenu->menuSideSprites[i], animId);
    }
    for (i = 0; i < 2; i++)
    {
        sChooseBoxMenu->arrowSprites[i] = CreateChooseBoxArrows(72 * i + 0x7c, 0x58, i, 0, sChooseBoxMenu->subpriority);
        if (sChooseBoxMenu->arrowSprites[i])
        {
            sChooseBoxMenu->arrowSprites[i]->data[0] = (i == 0 ? -1 : 1);
            sChooseBoxMenu->arrowSprites[i]->callback = SpriteCB_ChooseBoxArrow;
        }
    }
    ChooseBoxMenu_PrintInfo();
}

static void ChooseBoxMenu_DestroySprites(void)
{
    u16 i;
    if (sChooseBoxMenu->menuSprite)
    {
        DestroySprite(sChooseBoxMenu->menuSprite);
        sChooseBoxMenu->menuSprite = NULL;
    }
    for (i = 0; i < 4; i++)
    {
        if (sChooseBoxMenu->menuSideSprites[i])
        {
            DestroySprite(sChooseBoxMenu->menuSideSprites[i]);
            sChooseBoxMenu->menuSideSprites[i] = NULL;
        }
    }
    for (i = 0; i < 2; i++)
    {
        if (sChooseBoxMenu->arrowSprites[i])
            DestroySprite(sChooseBoxMenu->arrowSprites[i]);
    }
}

static void ChooseBoxMenu_MoveRight(void)
{
    if (++sChooseBoxMenu->curBox >= TOTAL_BOXES_COUNT)
        sChooseBoxMenu->curBox = 0;
    ChooseBoxMenu_PrintInfo();
}

static void ChooseBoxMenu_MoveLeft(void)
{
    sChooseBoxMenu->curBox = (sChooseBoxMenu->curBox == 0 ? TOTAL_BOXES_COUNT - 1 : sChooseBoxMenu->curBox - 1);
    ChooseBoxMenu_PrintInfo();
}

static void ChooseBoxMenu_PrintInfo(void)
{
    u8 numInBox = CountMonsInBox(sChooseBoxMenu->curBox);
    u8 *stringVar = gStringVar1;

    stringVar[0] = EXT_CTRL_CODE_BEGIN;
    stringVar[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
    stringVar[2] = TEXT_COLOR_WHITE2;
    stringVar[3] = TEXT_COLOR_DARK_GREY;
    stringVar[4] = TEXT_COLOR_LIGHT_BLUE;
    stringVar += 5;

    stringVar = StringCopy(stringVar, gPokemonStorage.boxNames[sChooseBoxMenu->curBox]);

    stringVar[0] = CHAR_NEWLINE;
    stringVar[1] = EXT_CTRL_CODE_BEGIN;
    stringVar[2] = EXT_CTRL_CODE_CLEAR;
    if (numInBox < 10)
        stringVar[3] = 0x28;
    else
        stringVar[3] = 0x22;
    stringVar += 4;

    stringVar = ConvertIntToDecimalStringSimple(stringVar, numInBox);

    stringVar[0] = CHAR_SLASH;
    stringVar[1] = CHAR_0 + 3;
    stringVar[2] = CHAR_0 + 0;
    stringVar[3] = EOS;
    DrawChooseBoxMenuText(gStringVar1, 0, 1);
}

static void DrawChooseBoxMenuText(const u8 *text, u16 tileX, u16 tileY)
{
    u16 *tileDest = (u16 *)(BG_CHAR_ADDR(4) + (GetSpriteTileStartByTag(sChooseBoxMenu->tileTag) * 32) + tileY * 256 + tileX * 32);
    u8 *tileBuffer = gUnknown_083B6DB8;
    DmaFill16(3, 0x1111, tileBuffer, 0x400);
    Text_InitWindow8004E3C(&gWindowTemplate_81E6D38, tileBuffer, text);
    DmaCopy16(3, tileBuffer, tileDest, 0x400);
}

static void SpriteCB_ChooseBoxArrow(struct Sprite *sprite)
{
    if (++sprite->data[1] > 3)
    {
        sprite->data[1] = 0;
        sprite->x2 += sprite->data[0];
        if (++sprite->data[2] > 5)
        {
            sprite->data[2] = 0;
            sprite->x2 = 0;
        }
    }
}
