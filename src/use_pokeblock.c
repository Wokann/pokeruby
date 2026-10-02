//

// Modified by Dizzy Egg on 8/15/17.
//

#include "global.h"
#include "main.h"
#include "overworld.h"
#include "string_util.h"
#include "strings.h"
#include "sprite.h"
#include "pokemon.h"
#include "pokenav.h"
#include "palette.h"
#include "text.h"
#include "menu.h"
#include "sound.h"
#include "constants/berry.h"
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

static const u32 sConditionToMonData[] =
{
    MON_DATA_COOL,
    MON_DATA_TOUGH,
    MON_DATA_SMART,
    MON_DATA_CUTE,
    MON_DATA_BEAUTY
};

static const u8 sConditionToFlavor[] =
{
    FLAVOR_SPICY,
    FLAVOR_SOUR,
    FLAVOR_BITTER,
    FLAVOR_SWEET,
    FLAVOR_DRY
};

static const u8 *const sConditionNames[] =
{
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

extern u16 gSpecialVar_ItemId; // FIXME: remove after merge of #349 Pokeblock

static void SetUsePokeblockCallback(void (*const)(void));
static void CB2_ReturnToUsePokeblockMenu(void);
static void CB2_ShowUsePokeblockMenuForResults(void);
static void CB2_UsePokeblockMenu(void);
static void VBlankCB_UsePokeblockMenu(void);
static void LoadUsePokeblockMenu(void);
static void RunUsePokeblockMenuLoader(void);
static void ShowUsePokeblockMenu(void);
static void UsePokeblockMenu(void);
static void ShowUsePokeblockMenuForResults(void);
void ScanlineEffect_InitHBlankDmaTransfer(void);
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
    sInfo = &gPokenavState->usePokeblockSession;
    sInfo->pokeblock = pokeblock;
    sInfo->exitCallback = callback;
    gPokenavState->unkD162 = 2;
    SetUsePokeblockCallback(LoadUsePokeblockMenu);
    SetMainCallback2(CB2_UsePokeblockMenu);
}

static void CB2_ReturnAndChooseMonToGivePokeblock(void)
{
    sInfo->pokeblock = sPokeblock;
    sInfo->exitCallback = sExitCallback;
    gPokeblockMonId = GetSelectionIdFromPartyId(gPokeblockMonId);
    sInfo->monInTopHalf = gPokeblockMonId < 4 ? FALSE : TRUE;
    gPokenavState->unkD162 = 2;
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
    ConditionGraph_DrawAtYOffset(6);
    ScanlineEffect_InitHBlankDmaTransfer();
}

static void SetUsePokeblockCallback(void (*const func)(void))
{
    sInfo->callback = func;
    sInfo->mainState = 0;
}

static void LoadUsePokeblockMenu(void)
{
    bool32 isLinkActive;
    switch (sInfo->mainState)
    {
        case 0:
            isLinkActive = IsOverworldLinkActive();
            gPokenavState->isLinkActive = isLinkActive;
            if ((bool8)isLinkActive == FALSE)
            {
                sInfo->isMenuLoaded = 0;
                SetUsePokeblockCallback(RunUsePokeblockMenuLoader);
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
            Text_LoadWindowTemplate(&gPokenavConditionWindowTemplate);
            sInfo->mainState++;
            break;
        case 4:
            MultistepInitMenuWindowBegin(&gPokenavConditionWindowTemplate);
            sInfo->mainState++;
            break;
        case 5:
            if (MultistepInitMenuWindowContinue())
            {
                sInfo->mainState++;
            }
            break;
        case 6:
            gPokenavState->isConditionGraphSearchMode = 0;
            gPokenavState->conditionGraphScrollCallback = NULL;
            gPokenavState->menuVerticalOffset = 0x20;
            sInfo->mainState++;
            break;
        case 7:
            InitPokenavMenuHeaderGfx();
            sInfo->mainState++;
            // fallthrough
        case 8:
            if (!LoadPokenavMenuHeaderGfxStep())
            {
                sInfo->mainState++;
            }
            break;
        case 9:
            BeginPokenavLeftHeaderLoad(1);
            sInfo->mainState++;
            // fallthrough
        case 10:
            if (!LoadPokenavLeftHeaderStep(1))
            {
                sInfo->mainState++;
            }
            break;
        case 11:
            gKeyRepeatStartDelay = 20;
            gPokenavState->playerPartyCount = CalculatePlayerPartyCount();
            gPokenavState->conditionGraph.needsDraw = 0;
            gPokenavState->portraitSprite = NULL;
            BuildPartyConditionGraphMonList();
            gPokenavState->setupStep = 0;
            sInfo->mainState++;
            break;
        case 12:
            if (!LoadPokeblockConditionGraphScreenStep())
            {
                REG_BG2VOFS = 6;
                REG_BG3VOFS = 6;
                sInfo->mainState++;
            }
            break;
        case 13:
            CreateOrUpdatePokenavPortraitSprite(0);
            gPokenavState->portraitSprite->y2 = 0xffd8;
            sInfo->mainState++;
            break;
        case 14:
            if (!SlidePokenavMonInfoHeaderIn())
            {
                sInfo->mainState++;
            }
            break;
        case 15:
            CreateConditionPartyPokeballIndicators();
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
            CalcPokeblockConditionGraphPositions(gPokenavState->conditionGraph.conditions[0], gPokenavState->conditionGraph.savedPositions[0]);
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
            ConditionGraph_Update(gPokenavState->conditionGraph.savedPositions[0]);
            sInfo->mainState++;
            break;
        case 20:
            PrintPokeblockMonNature();
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
            sInfo->isMenuLoaded = 1;
            SetUsePokeblockCallback(ShowUsePokeblockMenu);
            break;
    }
}

static void RunUsePokeblockMenuLoader(void)
{
    while (!sInfo->isMenuLoaded)
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
                LoadConditionSparkles();
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
                BeginConditionGraphMonScroll(TRUE);
                DestroyConditionSparkleSprites();
                sInfo->mainState = 1;
            }
            else if (JOY_HELD(DPAD_DOWN))
            {
                PlaySE(SE_SELECT);
                BeginConditionGraphMonScroll(FALSE);
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
                if (gPokenavState->detailSelectionIndex == gPokenavState->conditionGraphMonCount - 1)
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
            if (!UpdateConditionGraphMonScroll())
            {
                sInfo->mainState++;
            }
            break;
        case 2:
            if (!Overworld_IsRecvQueueAtMax())
            {
                PrintPokeblockMonNature();
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
            gPokeblockMonId = GetPartyIdFromSelectionId(gPokenavState->detailSelectionIndex);
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
            if (gPokenavState->detailSelectionIndex != gPokeblockMonId)
            {
                BeginConditionGraphMonScroll(sInfo->monInTopHalf);
                sInfo->mainState++;
            }
            else
            {
                sInfo->mainState = 3;
            }
            break;
        case 1:
            if (!UpdateConditionGraphMonScroll())
            {
                sInfo->mainState++;
            }
            break;
        case 2:
            if (!Overworld_IsRecvQueueAtMax())
            {
                PrintPokeblockMonNature();
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
                LoadConditionSparkles();
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
            sInfo->mon = &gPlayerParty[gPokenavState->monList[gPokenavState->detailSelectionIndex].monId];
            DestroyConditionSparkleSprites();
            sInfo->mainState++;
            break;
        case 1:
            if (JOY_NEW(A_BUTTON | B_BUTTON))
                sInfo->mainState++;
            break;
        case 2:
            CalculateConditionEnhancements();
            CalcPokeblockConditionGraphPositions(sInfo->conditionsAfterBlock, gPokenavState->conditionGraph.savedPositions[3]);
            StartPokeblockConditionGraphReset(gPokenavState->conditionGraph.savedPositions[gPokenavState->portraitSlot], gPokenavState->conditionGraph.savedPositions[3]);
            LoadAndCreateUpDownSprites();
            sInfo->mainState++;
            break;
        case 3:
            if (!UpdatePokeblockConditionGraphReset())
            {
                CalculateNumAdditionalSparkles(GetPartyIdFromSelectionId(gPokenavState->detailSelectionIndex));
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
            StopConditionGraphScanlineEffect();
            sInfo->mainState++;
            break;
        case 3:
            SetMainCallback2(sInfo->exitCallback);
            break;
    }
}

static void AskUsePokeblock(void)
{
    GetMonData(&gPlayerParty[GetPartyIdFromSelectionId(gPokenavState->detailSelectionIndex)], MON_DATA_NICKNAME, sInfo->stringBuffer);
    StringGet_Nickname(sInfo->stringBuffer);
    StringAppend(sInfo->stringBuffer, gText_GetsAPokeBlockQuestion);
    BasicInitMenuWindow(&gMenuWindowTemplateBg0);
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
        BasicInitMenuWindow(&gPokenavConditionWindowTemplate);
    }
    return menuItem;
}

static void PrintFirstEnhancement(void)
{
    BasicInitMenuWindow(&gMenuWindowTemplateBg0);
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
    BasicInitMenuWindow(&gMenuWindowTemplateBg0);
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(gOtherText_WontEat, 1, 17);
}

static void EraseMenuWindow(void)
{
    Menu_EraseScreen();
    BasicInitMenuWindow(&gPokenavConditionWindowTemplate);
}

static void PrintMenuWindowText(const u8 *message)
{
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(message, 1, 17);
}

void BufferEnhancedText(u8 *dest, u8 condition, s16 enhancement)
{
    if (enhancement)
    {
        // This is a joke.
        if (enhancement > 0)
            enhancement = 0;

        if (enhancement < 0)
            // matches, but can also be a variety of values too
            { u8 unusedVar = -unusedVar; } // see water.c for a similar behavior

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
    mon += gPokenavState->monList[gPokenavState->detailSelectionIndex].monId;
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
    struct Pokemon *mon = gPlayerParty;
    mon += gPokenavState->monList[gPokenavState->detailSelectionIndex].monId;
    if (GetMonData(mon, MON_DATA_SHEEN) == 255)
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

u8 GetPartyIdFromPokeblockSelection(u8 selectionId)
{
    return GetPartyIdFromSelectionId(selectionId);
}

static void LoadAndCreateUpDownSprites(void)
{
    u16 condition;
    u8 spriteId;
    LoadSpriteSheet(&sSpriteSheet_UpDown);
    LoadSpritePalette(&sSpritePalette_UpDown);
    sInfo->numEnhancements = 0;
    for (condition=0; condition<5; condition++)
    {
        if (sInfo->enhancements[condition] != 0)
        {
            spriteId = CreateSprite(&sSpriteTemplate_UpDown, sUpDownCoordsOnGraph[condition][0], sUpDownCoordsOnGraph[condition][1], 0);
            if (spriteId != MAX_SPRITES)
            {
                if (sInfo->enhancements[condition] != 0)
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
