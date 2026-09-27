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
#include "constants/songs.h"
#include "pokeblock.h"

#define GFX_TAG_CONDITIONUPDOWN 0

#ifdef GERMAN
extern const u16 ConditionUpDownPalette[16];
extern const u8 ConditionUpDownTiles[0x200];
#else
const u16 ConditionUpDownPalette[] = INCBIN_U16("graphics/misc/condition_up_down.gbapal");
const u8 ConditionUpDownTiles[] = INCBIN_U8("graphics/misc/condition_up_down.4bpp");
#endif

static const u32 sContestStatsMonData[] = {
    MON_DATA_COOL,
    MON_DATA_TOUGH,
    MON_DATA_SMART,
    MON_DATA_CUTE,
    MON_DATA_BEAUTY
};

static const u8 gUnknown_0840612C[] = {
    0, 4, 3, 2, 1
};

static const u8 *const sContestStatNames[] = {
    OtherText_Coolness,
    OtherText_Toughness,
    OtherText_Smartness,
    OtherText_Cuteness,
    OtherText_Beauty
};

static const struct SpriteSheet gSpriteSheet_ConditionUpDown = {
    ConditionUpDownTiles,
    sizeof ConditionUpDownTiles,
    GFX_TAG_CONDITIONUPDOWN
};

static const struct SpritePalette gSpritePalette_ConditionUpDown = {
    ConditionUpDownPalette,
    GFX_TAG_CONDITIONUPDOWN
};

static const s16 gUnknown_08406158[][2] = {
    {0x9c, 0x1e},
    {0x75, 0x35},
    {0x75, 0x70},
    {0xc5, 0x70},
    {0xc5, 0x35}
};

static const struct OamData gOamData_840616C = {
    .shape = 1,
    .size = 2,
    .priority = 1
};

static const union AnimCmd gSpriteAnim_8406174[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_840617C[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

static const union AnimCmd *const gSpriteAnimTable_8406184[] = {
    gSpriteAnim_8406174,
    gSpriteAnim_840617C
};

static const struct SpriteTemplate gSpriteTemplate_840618C = {
    GFX_TAG_CONDITIONUPDOWN,
    GFX_TAG_CONDITIONUPDOWN,
    &gOamData_840616C,
    gSpriteAnimTable_8406184,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static EWRAM_DATA struct UnkPokenavStruct_Sub1 *gUnknown_02039304 = NULL;
static EWRAM_DATA MainCallback gUnknown_02039308 = NULL;
static EWRAM_DATA struct Pokeblock *gUnknown_0203930C = NULL;
EWRAM_DATA u8 gPokeblockMonID = 0;
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
static s8 ProcessPokeblockYesNoInput(void);
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
    gUnknown_02039304 = &gPokenavStructPtr->unkD164;
    gUnknown_02039304->pokeblock = pokeblock;
    gUnknown_02039304->callback = callback;
    gPokenavStructPtr->unkD162 = 2;
    SetUsePokeblockCallback(LoadUsePokeblockMenu);
    SetMainCallback2(CB2_UsePokeblockMenu);
}

static void CB2_ReturnAndChooseMonToGivePokeblock(void)
{
    gUnknown_02039304->pokeblock = gUnknown_0203930C;
    gUnknown_02039304->callback = gUnknown_02039308;
    gPokeblockMonID = GetSelectionIdFromPartyId(gPokeblockMonID);
    gUnknown_02039304->unk56 = gPokeblockMonID < 4 ? 0 : 1;
    gPokenavStructPtr->unkD162 = 2;
    SetUsePokeblockCallback(LoadUsePokeblockMenu);
    SetMainCallback2(CB2_ReturnToUsePokeblockMenu);
}

static void CB2_ReturnToUsePokeblockMenu(void)
{
    gUnknown_02039304->unk0();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    if (gUnknown_02039304->unk0 == ShowUsePokeblockMenu)
    {
        REG_DISPCNT = 0;
        gUnknown_02039304->unk50 = 0;
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
    gUnknown_02039304->unk0();
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
    gUnknown_02039304->unk0 = func;
    gUnknown_02039304->unk50 = 0;
}

static void LoadUsePokeblockMenu(void)
{
    bool32 c1LinkRelatedActive;
    switch (gUnknown_02039304->unk50)
    {
        case 0:
            c1LinkRelatedActive = is_c1_link_related_active();
            gPokenavStructPtr->unk6DAC = c1LinkRelatedActive;
            if ((bool8)c1LinkRelatedActive == FALSE)
            {
                gUnknown_02039304->unk55 = 0;
                SetUsePokeblockCallback(RunUsePokeblockMenuLoader);
                gUnknown_02039304->unk50++;
            }
            break;
        case 1:
            ResetSpriteData();
            FreeAllSpritePalettes();
            gUnknown_02039304->unk50++;
            break;
        case 2:
            SetVBlankCallback(NULL);
            gUnknown_02039304->unk50++;
            break;
        case 3:
            Text_LoadWindowTemplate(&gWindowTemplate_81E7080);
            gUnknown_02039304->unk50++;
            break;
        case 4:
            MultistepInitMenuWindowBegin(&gWindowTemplate_81E7080);
            gUnknown_02039304->unk50++;
            break;
        case 5:
            if (MultistepInitMenuWindowContinue())
            {
                gUnknown_02039304->unk50++;
            }
            break;
        case 6:
            gPokenavStructPtr->isConditionGraphSearchMode = 0;
            gPokenavStructPtr->unk87E0 = NULL;
            gPokenavStructPtr->menuVerticalOffset = 0x20;
            gUnknown_02039304->unk50++;
            break;
        case 7:
            InitPokenavMenuHeaderGfx();
            gUnknown_02039304->unk50++;
            // fallthrough
        case 8:
            if (!LoadPokenavMenuHeaderGfxStep())
            {
                gUnknown_02039304->unk50++;
            }
            break;
        case 9:
            BeginPokenavLeftHeaderLoad(1);
            gUnknown_02039304->unk50++;
            // fallthrough
        case 10:
            if (!LoadPokenavLeftHeaderStep(1))
            {
                gUnknown_02039304->unk50++;
            }
            break;
        case 11:
            gKeyRepeatStartDelay = 20;
            gPokenavStructPtr->unk8828 = CalculatePlayerPartyCount();
            gPokenavStructPtr->unk9344 = 0;
            gPokenavStructPtr->portraitSprite = NULL;
            BuildPartyConditionGraphMonList();
            gPokenavStructPtr->setupStep = 0;
            gUnknown_02039304->unk50++;
            break;
        case 12:
            if (!LoadPokeblockConditionGraphScreenStep())
            {
                REG_BG2VOFS = 6;
                REG_BG3VOFS = 6;
                gUnknown_02039304->unk50++;
            }
            break;
        case 13:
            CreateOrUpdatePokenavPortraitSprite(0);
            gPokenavStructPtr->portraitSprite->y2 = 0xffd8;
            gUnknown_02039304->unk50++;
            break;
        case 14:
            if (!SlidePokenavMonInfoHeaderIn())
            {
                gUnknown_02039304->unk50++;
            }
            break;
        case 15:
            CreateConditionPartyPokeballIndicators();
            gUnknown_02039304->unk50++;
            break;
        case 16:
            DmaClear32(3, BG_SCREEN_ADDR(31), 0x800);
            REG_BG1VOFS = 0;
            REG_BG1HOFS = 0;
            REG_BG1CNT = BGCNT_SCREENBASE(31);
            gUnknown_02039304->unk50++;
            break;
        case 17:
            CalcPokeblockConditionGraphPositions(gPokenavStructPtr->unk8ff0[0], gPokenavStructPtr->unk9004[0]);
            ConditionGraph_InitResetScanline();
            gUnknown_02039304->unk50++;
            break;
        case 18:
            if (!ConditionGraph_ResetScanline())
            {
                gUnknown_02039304->unk50++;
            }
            break;
        case 19:
            ConditionGraph_Update(gPokenavStructPtr->unk9004[0]);
            gUnknown_02039304->unk50++;
            break;
        case 20:
            PrintPokeblockMonNature();
            gUnknown_02039304->unk50++;
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
            gUnknown_02039304->unk55 = 1;
            SetUsePokeblockCallback(ShowUsePokeblockMenu);
            break;
    }
}

static void RunUsePokeblockMenuLoader(void)
{
    while (!gUnknown_02039304->unk55)
    {
        LoadUsePokeblockMenu();
    }
}

static void ShowUsePokeblockMenu(void)
{
    switch (gUnknown_02039304->unk50)
    {
        case 0:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            SetVBlankCallback(VBlankCB_UsePokeblockMenu);
            gUnknown_02039304->unk50++;
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
    switch (gUnknown_02039304->unk50)
    {
        case 0:
            if (JOY_HELD(DPAD_UP))
            {
                PlaySE(SE_SELECT);
                BeginConditionGraphMonScroll(TRUE);
                DestroyConditionSparkleSprites();
                gUnknown_02039304->unk50 = 1;
            }
            else if (JOY_HELD(DPAD_DOWN))
            {
                PlaySE(SE_SELECT);
                BeginConditionGraphMonScroll(FALSE);
                DestroyConditionSparkleSprites();
                gUnknown_02039304->unk50 = 1;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                gUnknown_02039304->unk50 = 3;
            }
            else if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                if (gPokenavStructPtr->unk87DC == gPokenavStructPtr->unk87DA - 1)
                {
                    gUnknown_02039304->unk50 = 3;
                }
                else
                {
                    gUnknown_02039304->unk50 = 5;
                }
            }
            break;
        case 1:
            if (!UpdateConditionGraphMonScroll())
            {
                gUnknown_02039304->unk50++;
            }
            break;
        case 2:
            if (!Overworld_IsRecvQueueAtMax())
            {
                PrintPokeblockMonNature();
                CreateConditionSparkleSprites();
                gUnknown_02039304->unk50 = 0;
            }
            break;
        case 3:
            SetUsePokeblockCallback(CloseUsePokeblockMenu);
            break;
        case 4:
            break;
        case 5:
            AskUsePokeblock();
            gUnknown_02039304->unk50++;
            break;
        case 6:
            switch (ProcessPokeblockYesNoInput())
            {
                case 1:
                case -1:
                    gUnknown_02039304->unk50 = 0;
                    break;
                case 0:
                    if (IsSheenMaxed())
                    {
                        PrintWontEatAnymore();
                        gUnknown_02039304->unk50 = 7;
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
                gUnknown_02039304->unk50 = 0;
            }
            break;
    }
}

static void FeedPokeblockToMon(void)
{
    switch (gUnknown_02039304->unk50)
    {
        case 0:
            gPokeblockMonID = GetPartyIdFromSelectionId(gPokenavStructPtr->unk87DC);
            gUnknown_02039308 = gUnknown_02039304->callback;
            gUnknown_0203930C = gUnknown_02039304->pokeblock;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_02039304->unk50++;
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
    switch (gUnknown_02039304->unk50)
    {
        case 0:
            if (gPokenavStructPtr->unk87DC != gPokeblockMonID)
            {
                BeginConditionGraphMonScroll(gUnknown_02039304->unk56);
                gUnknown_02039304->unk50++;
            }
            else
            {
                gUnknown_02039304->unk50 = 3;
            }
            break;
        case 1:
            if (!UpdateConditionGraphMonScroll())
            {
                gUnknown_02039304->unk50++;
            }
            break;
        case 2:
            if (!Overworld_IsRecvQueueAtMax())
            {
                PrintPokeblockMonNature();
                gUnknown_02039304->unk50 = 0;
            }
            break;
        case 3:
            BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
            gUnknown_02039304->unk50++;
            break;
        case 4:
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
            gUnknown_02039304->unk50++;
            break;
        case 5:
            SetVBlankCallback(VBlankCB_UsePokeblockMenu);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gUnknown_02039304->unk50++;
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
    switch (gUnknown_02039304->unk50)
    {
        case 0:
            gUnknown_02039304->pokemon = &gPlayerParty[0];
            gUnknown_02039304->pokemon = &gPlayerParty[gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].partyIdx];
            DestroyConditionSparkleSprites();
            gUnknown_02039304->unk50++;
            break;
        case 1:
            if (JOY_NEW(A_BUTTON | B_BUTTON))
                gUnknown_02039304->unk50++;
            break;
        case 2:
            CalculateConditionEnhancements();
            CalcPokeblockConditionGraphPositions(gUnknown_02039304->unk5c, gPokenavStructPtr->unk9004[3]);
            StartPokeblockConditionGraphReset(gPokenavStructPtr->unk9004[gPokenavStructPtr->unk8fe9], gPokenavStructPtr->unk9004[3]);
            LoadAndCreateUpDownSprites();
            gUnknown_02039304->unk50++;
            break;
        case 3:
            if (!UpdatePokeblockConditionGraphReset())
            {
                CalculateNumAdditionalSparkles(GetPartyIdFromSelectionId(gPokenavStructPtr->unk87DC));
                CreateConditionSparkleSprites();
                gUnknown_02039304->unk52 = 0;
                gUnknown_02039304->unk50++;
            }
            break;
        case 4:
            if ((++gUnknown_02039304->unk52) > 16)
            {
                PrintFirstEnhancement();
                gUnknown_02039304->unk50++;
            }
            break;
        case 5:
            if (JOY_NEW(A_BUTTON | B_BUTTON) && !TryPrintNextEnhancement())
            {
                PokeblockClearIfExists((u8)gSpecialVar_ItemId);
                SetUsePokeblockCallback(CloseUsePokeblockMenu);
            }
            break;
    }
}

static void CloseUsePokeblockMenu(void)
{
    switch (gUnknown_02039304->unk50)
    {
        case 0:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_02039304->unk50++;
            break;
        case 1:
            if (!gPaletteFade.active)
            {
                gUnknown_02039304->unk50 = 2;
            }
            break;
        case 2:
            StopConditionGraphScanlineEffect();
            gUnknown_02039304->unk50++;
            break;
        case 3:
            SetMainCallback2(gUnknown_02039304->callback);
            break;
    }
}

static void AskUsePokeblock(void)
{
    GetMonData(&gPlayerParty[GetPartyIdFromSelectionId(gPokenavStructPtr->unk87DC)], MON_DATA_NICKNAME, gUnknown_02039304->stringBuffer);
    StringGet_Nickname(gUnknown_02039304->stringBuffer);
    StringAppend(gUnknown_02039304->stringBuffer, gOtherText_GetsAPokeBlock);
    BasicInitMenuWindow(&gWindowTemplate_81E709C);
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(gUnknown_02039304->stringBuffer, 1, 17);
    DisplayYesNoMenu(23, 10, 1);
    Menu_MoveCursor(0);
}

static s8 ProcessPokeblockYesNoInput(void)
{
    s8 retval = Menu_ProcessInputNoWrap();
    if ((u8)(retval + 1) < 3)
    {
        Menu_EraseScreen();
        BasicInitMenuWindow(&gWindowTemplate_81E7080);
    }
    return retval;
}

static void PrintFirstEnhancement(void)
{
    BasicInitMenuWindow(&gWindowTemplate_81E709C);
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    for (gUnknown_02039304->unk53 = 0; gUnknown_02039304->unk53 < 5 && gUnknown_02039304->unk61[gUnknown_02039304->unk53] == 0; gUnknown_02039304->unk53++);
    if (gUnknown_02039304->unk53 < 5)
    {
        BufferEnhancedText(gUnknown_02039304->stringBuffer, gUnknown_02039304->unk53, gUnknown_02039304->unk61[gUnknown_02039304->unk53]);
    }
    else
    {
        BufferEnhancedText(gUnknown_02039304->stringBuffer, gUnknown_02039304->unk53, 0);
    }
    PrintMenuWindowText(gUnknown_02039304->stringBuffer);
}

static bool8 TryPrintNextEnhancement(void)
{
    while (1)
    {
        gUnknown_02039304->unk53++;
        if (gUnknown_02039304->unk53 < 5)
        {
            if (gUnknown_02039304->unk61[gUnknown_02039304->unk53] != 0)
                break;
        }
        else
        {
            gUnknown_02039304->unk53 = 5;
            return FALSE;
        }
    }
    BufferEnhancedText(gUnknown_02039304->stringBuffer, gUnknown_02039304->unk53, gUnknown_02039304->unk61[gUnknown_02039304->unk53]);
    PrintMenuWindowText(gUnknown_02039304->stringBuffer);
    return TRUE;
}

static void PrintWontEatAnymore(void)
{
    BasicInitMenuWindow(&gWindowTemplate_81E709C);
    Menu_DrawStdWindowFrame(0, 16, 29, 19);
    Menu_PrintText(gOtherText_WontEat, 1, 17);
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

void BufferEnhancedText(u8 *dest, u8 statId, s16 enhanced)
{
    if (enhanced)
    {
        // This is a joke.
        if (enhanced > 0)
            enhanced = 0;

        if (enhanced < 0)
            // matches, but can also be a variety of values too
            { u8 unk = -unk; } // see water.c for a similar behavior

        StringCopy(dest, sContestStatNames[statId]);
        StringAppend(dest, gOtherText_WasEnhanced);
    }
    else
    {
        StringCopy(dest, gOtherText_NothingChanged);
    }
}

static void GetMonConditions(struct Pokemon *pokemon, u8 *data)
{
    u16 i;
    for (i=0; i<5; i++)
    {
        data[i] = GetMonData(pokemon, sContestStatsMonData[i]);
    }
}

static void AddPokeblockToConditions(struct Pokeblock *pokeblock, struct Pokemon *pokemon)
{
    u16 i;
    s16 cstat;
    u8 data;
    if (GetMonData(pokemon, MON_DATA_SHEEN) != 255)
    {
        CalculatePokeblockEffectiveness(pokeblock, pokemon);
        for (i=0; i<5; i++)
        {
            data = GetMonData(pokemon, sContestStatsMonData[i]);
            cstat = data + gUnknown_02039304->unk66[i];
            if (cstat < 0)
                cstat = 0;
            if (cstat > 255)
                cstat = 255;
            data = cstat;
            SetMonData(pokemon, sContestStatsMonData[i], &data);
        }
        cstat = (u8)GetMonData(pokemon, MON_DATA_SHEEN);
        cstat = cstat + pokeblock->feel;
        if (cstat > 255)
            cstat = 255;
        data = cstat;
        SetMonData(pokemon, MON_DATA_SHEEN, &data);
    }
}

static void CalculateConditionEnhancements(void)
{
    u16 i;
    struct Pokemon *pokemon = gPlayerParty;
    pokemon += gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].partyIdx;
    GetMonConditions(pokemon, gUnknown_02039304->unk57);
    AddPokeblockToConditions(gUnknown_02039304->pokeblock, pokemon);
    GetMonConditions(pokemon, gUnknown_02039304->unk5c);
    for (i=0; i<5; i++)
    {
        gUnknown_02039304->unk61[i] = gUnknown_02039304->unk5c[i] - gUnknown_02039304->unk57[i];
    }
}

static void CalculatePokeblockEffectiveness(struct Pokeblock *pokeblock, struct Pokemon *pokemon)
{
    s8 direction;
    s8 i;
    s16 amount;
    s8 boost;
    s8 taste;
    gUnknown_02039304->unk66[0] = pokeblock->spicy;
    gUnknown_02039304->unk66[1] = pokeblock->sour;
    gUnknown_02039304->unk66[2] = pokeblock->bitter;
    gUnknown_02039304->unk66[3] = pokeblock->sweet;
    gUnknown_02039304->unk66[4] = pokeblock->dry;
    if (gPokeblockGain > 0)
        direction = 1;
    else if (gPokeblockGain < 0)
        direction = -1;
    else
        return;
    for (i=0; i<5; i++)
    {
        amount = gUnknown_02039304->unk66[i];
        boost = amount / 10;
        if (amount % 10 >= 5) // round to the nearest
            boost++;
        taste = sub_8040A54(pokemon, gUnknown_0840612C[i]);
        if (taste == direction)
        {
            gUnknown_02039304->unk66[i] += boost * taste;
        }
    }
}

static bool8 IsSheenMaxed(void)
{
    struct Pokemon *pokemon = gPlayerParty;
    pokemon += gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].partyIdx;
    if (GetMonData(pokemon, MON_DATA_SHEEN) == 255)
        return TRUE;
    return FALSE;
}

static u8 GetPartyIdFromSelectionId(u8 a0)
{
    u8 i;
    for (i=0; i<PARTY_SIZE; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            if (a0 == 0)
                return i;
            a0--;
        }
    }
    return 0;
}

static u8 GetSelectionIdFromPartyId(u8 a0)
{
    u8 ct;
    u8 i;
    for (i=0, ct=0; i<a0; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            ct++;
        }
    }
    return a0 - ct;
}

u8 GetPartyIdFromPokeblockSelection(u8 a0)
{
    return GetPartyIdFromSelectionId(a0);
}

static void LoadAndCreateUpDownSprites(void)
{
    u16 flavor;
    u8 spriteidx;
    LoadSpriteSheet(&gSpriteSheet_ConditionUpDown);
    LoadSpritePalette(&gSpritePalette_ConditionUpDown);
    gUnknown_02039304->unk54 = 0;
    for (flavor=0; flavor<5; flavor++)
    {
        if (gUnknown_02039304->unk61[flavor] != 0)
        {
            spriteidx = CreateSprite(&gSpriteTemplate_840618C, gUnknown_08406158[flavor][0], gUnknown_08406158[flavor][1], 0);
            if (spriteidx != MAX_SPRITES)
            {
                if (gUnknown_02039304->unk61[flavor] != 0)
                {
                    gSprites[spriteidx].callback = SpriteCB_UpDown;
                }
                gUnknown_02039304->unk54++;
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
        gUnknown_02039304->unk54--;
    }
}
