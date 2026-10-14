//

// Modified by Dizzy Egg on 8/15/17.
//

#include "global.h"
#include "item_menu.h"
#include "main.h"
#include "overworld.h"
#include "string_util.h"
#include "strings.h"
#include "sprite.h"
#include "pokemon.h"
#include "pokenav.h"
#include "graphics.h"
#include "scanline_effect.h"
#include "palette.h"
#include "text.h"
#include "menu.h"
#include "sound.h"
#include "constants/songs.h"
#include "pokeblock.h"

#define TAG_UP_DOWN 0

#ifdef GERMAN
extern const u16 gUsePokeblockUpDown_Pal[16];
extern const u8 gUsePokeblockUpDown_Gfx[0x200];
#else
const u16 gUsePokeblockUpDown_Pal[] = INCBIN_U16("graphics/pokeblock/use_screen/updown.gbapal");
const u8 gUsePokeblockUpDown_Gfx[] = INCBIN_U8("graphics/pokeblock/use_screen/updown.4bpp");
#endif

static const u32 sConditionToMonData[] = {
    MON_DATA_COOL,
    MON_DATA_TOUGH,
    MON_DATA_SMART,
    MON_DATA_CUTE,
    MON_DATA_BEAUTY
};

static const u8 sConditionToFlavor[] = {
    0, 4, 3, 2, 1
};

static const u8 *const sConditionNames[] = {
    gText_Coolness,
    gText_Toughness,
    gText_Smartness,
    gText_Cuteness,
    gText_Beauty3
};

static const struct SpriteSheet sSpriteSheet_UpDown = {
    gUsePokeblockUpDown_Gfx,
    sizeof gUsePokeblockUpDown_Gfx,
    TAG_UP_DOWN
};

static const struct SpritePalette sSpritePalette_UpDown = {
    gUsePokeblockUpDown_Pal,
    TAG_UP_DOWN
};

static const s16 sUpDownCoordsOnGraph[][2] = {
    {0x9c, 0x1e},
    {0x75, 0x35},
    {0x75, 0x70},
    {0xc5, 0x70},
    {0xc5, 0x35}
};

static const struct OamData sOam_UpDown = {
    .shape = 1,
    .size = 2,
    .priority = 1
};

static const union AnimCmd sAnim_Up[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Down[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_UpDown[] = {
    sAnim_Up,
    sAnim_Down
};

static const struct SpriteTemplate sSpriteTemplate_UpDown = {
    TAG_UP_DOWN,
    TAG_UP_DOWN,
    &sOam_UpDown,
    sAnims_UpDown,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static EWRAM_DATA struct UsePokeblockSession *sInfo = NULL;
static EWRAM_DATA MainCallback sExitCallback = NULL;
static EWRAM_DATA struct Pokeblock *sPokeblock = NULL;
EWRAM_DATA u8 gPokeblockMonId = 0;
EWRAM_DATA s16 gPokeblockGain = 0;


static void SetUsePokeblockCallback(void (*const)(void));
static void CB2_ReturnToUsePokeblockMenu(void);
static void CB2_ShowUsePokeblockMenuForResults(void);
static void CB2_UsePokeblockMenu(void);
static void VBlankCB_UsePokeblockMenu(void);
static void LoadUsePokeblockMenu(void);
static void sub_81365A0(void);
static void ShowUsePokeblockMenu(void);
static void UsePokeblockMenu(void);
static void ShowUsePokeblockMenuForResults(void);
static void CloseUsePokeblockMenu(void);
static u8 GetSelectionIdFromPartyId(u8);
static void AskUsePokeblock(void);
static s8 HandleAskUsePokeblockInput(void);
static bool8 IsSheenMaxed(void);
static void PrintWontEatAnymore(void);
static void FeedPokeblockToMon(void);
static void EraseMenuWindow(void);
static u8 GetPartyIdFromSelectionId(u8);
static void ShowPokeblockResults(void);
static void CalculateConditionEnhancements(void);
static void LoadAndCreateUpDownSprites(void);
static void PrintFirstEnhancement(void);
static bool8 TryPrintNextEnhancement(void);
static void BufferEnhancedText(u8 *, u8, s16);
static void PrintMenuWindowText(const u8 *);
static void CalculatePokeblockEffectiveness(struct Pokeblock *, struct Pokemon *);
static void SpriteCB_UpDown(struct Sprite *);

void ChooseMonToGivePokeblock(struct Pokeblock *pokeblock, MainCallback callback)
{
    sInfo = &gPokenavStructPtr->info;
    sInfo->pokeblock = pokeblock;
    sInfo->exitCallback = callback;
    gPokenavStructPtr->unkD162 = 2;
    SetUsePokeblockCallback(LoadUsePokeblockMenu);
    SetMainCallback2(CB2_UsePokeblockMenu);
}

static void CB2_ReturnAndChooseMonToGivePokeblock(void)
{
    sInfo->pokeblock = sPokeblock;
    sInfo->exitCallback = sExitCallback;
    gPokeblockMonId = GetSelectionIdFromPartyId(gPokeblockMonId);
    sInfo->monInTopHalf = gPokeblockMonId < 4 ? 0 : 1;
    gPokenavStructPtr->unkD162 = 2;
    SetUsePokeblockCallback(LoadUsePokeblockMenu);
    SetMainCallback2(CB2_ReturnToUsePokeblockMenu);
}

static void CB2_ReturnToUsePokeblockMenu(void)
{
    sInfo->callback();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    if (sInfo->callback == ShowUsePokeblockMenu)
    {
        REG_DISPCNT = 0;
        sInfo->mainState = 0;
        SetMainCallback2(CB2_ShowUsePokeblockMenuForResults);
    }
}

static void CB2_ShowUsePokeblockMenuForResults(void)
{
    ShowUsePokeblockMenuForResults();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_UsePokeblockMenu(void)
{
    sInfo->callback();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_UsePokeblockMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    sub_80F5CDC(6);
    ScanlineEffect_InitHBlankDmaTransfer();
}

static void SetUsePokeblockCallback(void (*const func)(void))
{
    sInfo->callback = func;
    sInfo->mainState = 0;
}

static void LoadUsePokeblockMenu(void)
{
    bool32 c1LinkRelatedActive;
    switch (sInfo->mainState)
    {
        case 0:
            c1LinkRelatedActive = IsOverworldLinkActive();
            gPokenavStructPtr->unk6DAC = c1LinkRelatedActive;
            if ((bool8)c1LinkRelatedActive == FALSE)
            {
                sInfo->unk55 = 0;
                SetUsePokeblockCallback(sub_81365A0);
                sInfo->mainState++;
            }
            break;
        case 1:
            ResetSpriteData();
            FreeAllSpritePalettes();
            sInfo->mainState++;
            break;
        case 2:
            SetVBlankCallback(NULL);
            sInfo->mainState++;
            break;
        case 3:
            Text_LoadWindowTemplate(&gWindowTemplate_81E7080);
            sInfo->mainState++;
            break;
        case 4:
            MultistepInitMenuWindowBegin(&gWindowTemplate_81E7080);
            sInfo->mainState++;
            break;
        case 5:
            if (MultistepInitMenuWindowContinue())
            {
                sInfo->mainState++;
            }
            break;
        case 6:
            gPokenavStructPtr->inSearchMode = 0;
            gPokenavStructPtr->unk87E0 = NULL;
            gPokenavStructPtr->unk030C = 0x20;
            sInfo->mainState++;
            break;
        case 7:
            sub_80F2688();
            sInfo->mainState++;
            // fallthrough
        case 8:
            if (!sub_80F26BC())
            {
                sInfo->mainState++;
            }
            break;
        case 9:
            sub_80F2C80(1);
            sInfo->mainState++;
            // fallthrough
        case 10:
            if (!sub_80F2CBC(1))
            {
                sInfo->mainState++;
            }
            break;
        case 11:
            gKeyRepeatStartDelay = 20;
            gPokenavStructPtr->unk8828 = CalculatePlayerPartyCount();
            gPokenavStructPtr->needsDraw = 0;
            gPokenavStructPtr->unk8768 = NULL;
            sub_80F4BD0();
            gPokenavStructPtr->unkD160 = 0;
            sInfo->mainState++;
            break;
        case 12:
            if (!sub_80F1778())
            {
                REG_BG2VOFS = 6;
                REG_BG3VOFS = 6;
                sInfo->mainState++;
            }
            break;
        case 13:
            sub_80F2E18(0);
            gPokenavStructPtr->unk8768->y2 = 0xffd8;
            sInfo->mainState++;
            break;
        case 14:
            if (!MoveConditionMonOnscreen())
            {
                sInfo->mainState++;
            }
            break;
        case 15:
            sub_80F33A8();
            sInfo->mainState++;
            break;
        case 16:
            DmaClear32(3, BG_SCREEN_ADDR(31), 0x800);
            REG_BG1VOFS = 0;
            REG_BG1HOFS = 0;
            REG_BG1CNT = BGCNT_SCREENBASE(31);
            sInfo->mainState++;
            break;
        case 17:
            sub_80F567C(gPokenavStructPtr->conditions[0], gPokenavStructPtr->savedPositions[0]);
            ConditionGraph_InitResetScanline();
            sInfo->mainState++;
            break;
        case 18:
            if (!ConditionGraph_ResetScanline())
            {
                sInfo->mainState++;
            }
            break;
        case 19:
            sub_80F556C(gPokenavStructPtr->savedPositions[0]);
            sInfo->mainState++;
            break;
        case 20:
            sub_80F1934();
            sInfo->mainState++;
            break;
        case 21:
            REG_WIN0H = 0xf0;
            REG_WIN1H = 0x9b;
            REG_WIN0V = 0x3273;
            REG_WIN1V = 0x3273;
            REG_WININ = 0x3f3f;
            REG_WINOUT = 0x1b;
            REG_BG0VOFS = 0x28;
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
            // fallthrough
        case 22:
            sInfo->unk55 = 1;
            SetUsePokeblockCallback(ShowUsePokeblockMenu);
            break;
    }
}

static void sub_81365A0(void)
{
    while (!sInfo->unk55)
    {
        LoadUsePokeblockMenu();
    }
}

static void ShowUsePokeblockMenu(void)
{
    switch (sInfo->mainState)
    {
        case 0:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            SetVBlankCallback(VBlankCB_UsePokeblockMenu);
            sInfo->mainState++;
            break;
        case 1:
            if (!gPaletteFade.active)
            {
                sub_80F3C94();
                CreateConditionSparkleSprites();
                SetUsePokeblockCallback(UsePokeblockMenu);
            }
            break;
    }
}

static void UsePokeblockMenu(void)
{
    switch (sInfo->mainState)
    {
        case 0:
            if (JOY_HELD(DPAD_UP))
            {
                PlaySE(SE_SELECT);
                sub_80F5060(TRUE);
                DestroyConditionSparkleSprites();
                sInfo->mainState = 1;
            }
            else if (JOY_HELD(DPAD_DOWN))
            {
                PlaySE(SE_SELECT);
                sub_80F5060(FALSE);
                DestroyConditionSparkleSprites();
                sInfo->mainState = 1;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                sInfo->mainState = 3;
            }
            else if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                if (gPokenavStructPtr->unk87DC == gPokenavStructPtr->unk87DA - 1)
                {
                    sInfo->mainState = 3;
                }
                else
                {
                    sInfo->mainState = 5;
                }
            }
            break;
        case 1:
            if (!gpu_sync_bg_show())
            {
                sInfo->mainState++;
            }
            break;
        case 2:
            if (!Overworld_IsRecvQueueAtMax())
            {
                sub_80F1934();
                CreateConditionSparkleSprites();
                sInfo->mainState = 0;
            }
            break;
        case 3:
            SetUsePokeblockCallback(CloseUsePokeblockMenu);
            break;
        case 4:
            break;
        case 5:
            AskUsePokeblock();
            sInfo->mainState++;
            break;
        case 6:
            switch (HandleAskUsePokeblockInput())
            {
                case 1:
                case -1:
                    sInfo->mainState = 0;
                    break;
                case 0:
                    if (IsSheenMaxed())
                    {
                        PrintWontEatAnymore();
                        sInfo->mainState = 7;
                    }
                    else
                    {
                        SetUsePokeblockCallback(FeedPokeblockToMon);
                    }
                    break;
            }
            break;
        case 7:
            if (JOY_NEW(A_BUTTON | B_BUTTON))
            {
                EraseMenuWindow();
                sInfo->mainState = 0;
            }
            break;
    }
}

static void FeedPokeblockToMon(void)
{
    switch (sInfo->mainState)
    {
        case 0:
            gPokeblockMonId = GetPartyIdFromSelectionId(gPokenavStructPtr->unk87DC);
            sExitCallback = sInfo->exitCallback;
            sPokeblock = sInfo->pokeblock;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            sInfo->mainState++;
            break;
        case 1:
            if (!gPaletteFade.active)
            {
                gMain.savedCallback = CB2_ReturnAndChooseMonToGivePokeblock;
                SetMainCallback2(PreparePokeblockFeedScene);
            }
            break;
    }
}

static void ShowUsePokeblockMenuForResults(void)
{
    switch (sInfo->mainState)
    {
        case 0:
            if (gPokenavStructPtr->unk87DC != gPokeblockMonId)
            {
                sub_80F5060(sInfo->monInTopHalf);
                sInfo->mainState++;
            }
            else
            {
                sInfo->mainState = 3;
            }
            break;
        case 1:
            if (!gpu_sync_bg_show())
            {
                sInfo->mainState++;
            }
            break;
        case 2:
            if (!Overworld_IsRecvQueueAtMax())
            {
                sub_80F1934();
                sInfo->mainState = 0;
            }
            break;
        case 3:
            BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
            sInfo->mainState++;
            break;
        case 4:
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
            sInfo->mainState++;
            break;
        case 5:
            SetVBlankCallback(VBlankCB_UsePokeblockMenu);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            sInfo->mainState++;
            break;
        case 6:
            if (!gPaletteFade.active)
            {
                sub_80F3C94();
                CreateConditionSparkleSprites();
                SetUsePokeblockCallback(ShowPokeblockResults);
                SetMainCallback2(CB2_UsePokeblockMenu);
            }
            break;
    }
}

static void ShowPokeblockResults(void)
{
    switch (sInfo->mainState)
    {
        case 0:
            sInfo->mon = &gPlayerParty[0];
            sInfo->mon = &gPlayerParty[gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].monId];
            DestroyConditionSparkleSprites();
            sInfo->mainState++;
            break;
        case 1:
            if (JOY_NEW(A_BUTTON | B_BUTTON))
                sInfo->mainState++;
            break;
        case 2:
            CalculateConditionEnhancements();
            sub_80F567C(sInfo->conditionsAfterBlock, gPokenavStructPtr->savedPositions[3]);
            sub_80F5550(gPokenavStructPtr->savedPositions[gPokenavStructPtr->loadId], gPokenavStructPtr->savedPositions[3]);
            LoadAndCreateUpDownSprites();
            sInfo->mainState++;
            break;
        case 3:
            if (!sub_80F555C())
            {
                CalculateNumAdditionalSparkles(GetPartyIdFromSelectionId(gPokenavStructPtr->unk87DC));
                CreateConditionSparkleSprites();
                sInfo->timer = 0;
                sInfo->mainState++;
            }
            break;
        case 4:
            if ((++sInfo->timer) > 16)
            {
                PrintFirstEnhancement();
                sInfo->mainState++;
            }
            break;
        case 5:
            if (JOY_NEW(A_BUTTON | B_BUTTON) && !TryPrintNextEnhancement())
            {
                TryClearPokeblock((u8)gSpecialVar_ItemId);
                SetUsePokeblockCallback(CloseUsePokeblockMenu);
            }
            break;
    }
}

static void CloseUsePokeblockMenu(void)
{
    switch (sInfo->mainState)
    {
        case 0:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            sInfo->mainState++;
            break;
        case 1:
            if (!gPaletteFade.active)
            {
                sInfo->mainState = 2;
            }
            break;
        case 2:
            sub_80F5BDC();
            sInfo->mainState++;
            break;
        case 3:
            SetMainCallback2(sInfo->exitCallback);
            break;
    }
}

static void AskUsePokeblock(void)
{
    GetMonData(&gPlayerParty[GetPartyIdFromSelectionId(gPokenavStructPtr->unk87DC)], MON_DATA_NICKNAME, sInfo->stringBuffer);
    StringGet_Nickname(sInfo->stringBuffer);
    StringAppend(sInfo->stringBuffer, gText_GetsAPokeBlockQuestion);
    BasicInitMenuWindow(&gWindowTemplate_81E709C);
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(sInfo->stringBuffer, 1, 17);
    DisplayYesNoMenu(23, 10, 1);
    Menu_MoveCursor(0);
}

static s8 HandleAskUsePokeblockInput(void)
{
    s8 menuItem = Menu_ProcessInputNoWrap();
    if ((u8)(menuItem + 1) < 3)
    {
        Menu_EraseScreen();
        BasicInitMenuWindow(&gWindowTemplate_81E7080);
    }
    return menuItem;
}

static void PrintFirstEnhancement(void)
{
    BasicInitMenuWindow(&gWindowTemplate_81E709C);
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    for (sInfo->condition = 0; sInfo->condition < 5 && sInfo->enhancements[sInfo->condition] == 0; sInfo->condition++);
    if (sInfo->condition < 5)
    {
        BufferEnhancedText(sInfo->stringBuffer, sInfo->condition, sInfo->enhancements[sInfo->condition]);
    }
    else
    {
        BufferEnhancedText(sInfo->stringBuffer, sInfo->condition, 0);
    }
    PrintMenuWindowText(sInfo->stringBuffer);
}

static bool8 TryPrintNextEnhancement(void)
{
    while (1)
    {
        sInfo->condition++;
        if (sInfo->condition < 5)
        {
            if (sInfo->enhancements[sInfo->condition] != 0)
                break;
        }
        else
        {
            sInfo->condition = 5;
            return FALSE;
        }
    }
    BufferEnhancedText(sInfo->stringBuffer, sInfo->condition, sInfo->enhancements[sInfo->condition]);
    PrintMenuWindowText(sInfo->stringBuffer);
    return TRUE;
}

static void PrintWontEatAnymore(void)
{
    BasicInitMenuWindow(&gWindowTemplate_81E709C);
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(gText_WontEatAnymore, 1, 17);
}

static void EraseMenuWindow(void)
{
    Menu_EraseScreen();
    BasicInitMenuWindow(&gWindowTemplate_81E7080);
}

static void PrintMenuWindowText(const u8 *message)
{
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(message, 1, 17);
}

static void BufferEnhancedText(u8 *dest, u8 condition, s16 enhancement)
{
    if (enhancement)
    {
        // This is a joke.
        if (enhancement > 0)
            enhancement = 0;

        if (enhancement < 0)
            // matches, but can also be a variety of values too
            { u8 unk = -unk; } // see water.c for a similar behavior

        StringCopy(dest, sConditionNames[condition]);
        StringAppend(dest, gText_WasEnhanced);
    }
    else
    {
        StringCopy(dest, gText_NothingChanged);
    }
}

static void GetMonConditions(struct Pokemon *mon, u8 *data)
{
    u16 i;
    for (i=0; i<5; i++)
    {
        data[i] = GetMonData(mon, sConditionToMonData[i]);
    }
}

static void AddPokeblockToConditions(struct Pokeblock *pokeblock, struct Pokemon *mon)
{
    u16 i;
    s16 stat;
    u8 data;
    if (GetMonData(mon, MON_DATA_SHEEN) != 255)
    {
        CalculatePokeblockEffectiveness(pokeblock, mon);
        for (i=0; i<5; i++)
        {
            data = GetMonData(mon, sConditionToMonData[i]);
            stat = data + sInfo->pokeblockStatBoosts[i];
            if (stat < 0)
                stat = 0;
            if (stat > 255)
                stat = 255;
            data = stat;
            SetMonData(mon, sConditionToMonData[i], &data);
        }
        stat = (u8)GetMonData(mon, MON_DATA_SHEEN);
        stat = stat + pokeblock->feel;
        if (stat > 255)
            stat = 255;
        data = stat;
        SetMonData(mon, MON_DATA_SHEEN, &data);
    }
}

static void CalculateConditionEnhancements(void)
{
    u16 i;
    struct Pokemon *mon = gPlayerParty;
    mon += gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].monId;
    GetMonConditions(mon, sInfo->conditionsBeforeBlock);
    AddPokeblockToConditions(sInfo->pokeblock, mon);
    GetMonConditions(mon, sInfo->conditionsAfterBlock);
    for (i=0; i<5; i++)
    {
        sInfo->enhancements[i] = sInfo->conditionsAfterBlock[i] - sInfo->conditionsBeforeBlock[i];
    }
}

static void CalculatePokeblockEffectiveness(struct Pokeblock *pokeblock, struct Pokemon *mon)
{
    s8 direction;
    s8 i;
    s16 amount;
    s8 boost;
    s8 flavor;
    sInfo->pokeblockStatBoosts[0] = pokeblock->spicy;
    sInfo->pokeblockStatBoosts[1] = pokeblock->sour;
    sInfo->pokeblockStatBoosts[2] = pokeblock->bitter;
    sInfo->pokeblockStatBoosts[3] = pokeblock->sweet;
    sInfo->pokeblockStatBoosts[4] = pokeblock->dry;
    if (gPokeblockGain > 0)
        direction = 1;
    else if (gPokeblockGain < 0)
        direction = -1;
    else
        return;
    for (i=0; i<5; i++)
    {
        amount = sInfo->pokeblockStatBoosts[i];
        boost = amount / 10;
        if (amount % 10 >= 5) // round to the nearest
            boost++;
        flavor = GetMonFlavorRelation(mon, sConditionToFlavor[i]);
        if (flavor == direction)
        {
            sInfo->pokeblockStatBoosts[i] += boost * flavor;
        }
    }
}

static bool8 IsSheenMaxed(void)
{
    struct Pokemon *pokemon = gPlayerParty;
    pokemon += gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].monId;
    if (GetMonData(pokemon, MON_DATA_SHEEN) == 255)
        return TRUE;
    return FALSE;
}

static u8 GetPartyIdFromSelectionId(u8 selectionId)
{
    u8 i;
    for (i=0; i<PARTY_SIZE; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            if (selectionId == 0)
                return i;
            selectionId--;
        }
    }
    return 0;
}

static u8 GetSelectionIdFromPartyId(u8 partyId)
{
    u8 numEggs;
    u8 i;
    for (i=0, numEggs=0; i<partyId; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            numEggs++;
        }
    }
    return partyId - numEggs;
}

u8 GetPartyIdFromSelectionId_(u8 selectionId)
{
    return GetPartyIdFromSelectionId(selectionId);
}

static void LoadAndCreateUpDownSprites(void)
{
    u16 i;
    u8 spriteId;
    LoadSpriteSheet(&sSpriteSheet_UpDown);
    LoadSpritePalette(&sSpritePalette_UpDown);
    sInfo->numEnhancements = 0;
    for (i=0; i<5; i++)
    {
        if (sInfo->enhancements[i] != 0)
        {
            spriteId = CreateSprite(&sSpriteTemplate_UpDown, sUpDownCoordsOnGraph[i][0], sUpDownCoordsOnGraph[i][1], 0);
            if (spriteId != MAX_SPRITES)
            {
                if (sInfo->enhancements[i] != 0)
                {
                    gSprites[spriteId].callback = SpriteCB_UpDown;
                }
                sInfo->numEnhancements++;
            }
        }
    }
}

static void SpriteCB_UpDown(struct Sprite *sprite)
{
    if (sprite->data[0] <= 5)
        sprite->y2 -= 2;
    else if (sprite->data[0] <= 11)
        sprite->y2 += 2;
    if ((++sprite->data[0]) > 60)
    {
        DestroySprite(sprite);
        sInfo->numEnhancements--;
    }
}
