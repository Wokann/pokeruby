#include "global.h"
#include "constants/easy_chat.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "dewford_trend.h"
#include "easy_chat.h"
#include "event_data.h"
#include "ewram.h"
#include "graphics.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "scanline_effect.h"

extern const struct WindowTemplate gWindowTemplate_81E6D54;
extern const struct WindowTemplate gWindowTemplate_81E6DA8;

extern void CB2_ReturnToFieldContinueScript(void);

static const u16 sMysteryEventPhrase[] = {EC_WORD_MYSTERY, EC_WORD_EVENT, EC_WORD_IS, EC_WORD_EXCITING};

static const u16 sBerryMasterWifePhrases[][2] =
{
#if ENGLISH
    {EC_WORD_GREAT, EC_WORD_BATTLE},
    {EC_WORD_CHALLENGE, EC_WORD_CONTEST},
    {EC_WORD_OVERWHELMING, EC_POKEMON(LATIAS)},
    {EC_WORD_COOL, EC_POKEMON(LATIOS)},
    {EC_WORD_SUPER, EC_WORD_HUSTLE},
#else
    {EC_WORD_GREAT, EC_WORD_FIGHT},
    {EC_WORD_CONTEST, EC_WORD_CHALLENGE},
    {EC_POKEMON(LATIAS), EC_WORD_OVERWHELMING},
    {EC_POKEMON(LATIOS), EC_WORD_COOL},
    {EC_WORD_SUPER, 0xFFFF},
#endif
};

// const pointer to gEasyChatStruct-> easy_chat might be two separate files.
struct Shared1000 *const gEasyChatStruct = (struct Shared1000 *)(gSharedMem + 0x1000);

static const struct ScanlineEffectParams sEasyChatScanlineParams =
{
    &REG_BG3VOFS,
    ((DMA_ENABLE | DMA_START_HBLANK | DMA_REPEAT | DMA_DEST_RELOAD) << 16) | 1,
    1
};

static const u8 sEasyChatLayoutByType[] = {4, 0, 0, 0, 1, 5, 0, 2, 2, 3, 2, 2, 2, 3};

// choose by alphabet keyboard
static const u8 sAlphabetKeyboardRows[][16] =
{
    _("ABCDEF "),
    _("GHIJKL"),
    _("MNOPQRS"),
    _("TUVWXYZ"),
};

struct EasyChatPrompt
{
    const u8 *text1;
    const u8 *text2;
    bool8 separateLines;
};

static const struct EasyChatPrompt sEasyChatPrompts[] =
{
    {OtherText_MakeProfilePage1, OtherText_MakeProfilePage2, TRUE},
    {OtherText_MakeMessagePage1, OtherText_MakeMessagePage2, TRUE},
    {OtherText_CombineNinePhrasesPage1, OtherText_CombineNinePhrasesPage2, TRUE},
    {OtherText_DescribeFeelingsPage1, OtherText_DescribeFeelingsPage2, TRUE},
    {OtherText_ImproveBardSongPage1, OtherText_ImproveBardSongPage2, TRUE},
    {OtherText_CombineTwoPhrasesPage1, OtherText_CombineTwoPhrasesPage2, TRUE},
    {OtherText_YourProfile, OtherText_ConfirmTrendyPage2, FALSE},
    {OtherText_YourFeelingBattle, OtherText_ConfirmTrendyPage2, TRUE},
    {OtherText_SetWinMessage, OtherText_ConfirmTrendyPage2, TRUE},
    {OtherText_SetLossMessage, OtherText_ConfirmTrendyPage2, TRUE},
    {OtherText_MailMessage, OtherText_ConfirmTrendyPage2, TRUE},
    {OtherText_MailSalutation, OtherText_ConfirmTrendyPage2, TRUE},
    {OtherText_NewSong, OtherText_ConfirmTrendyPage2, FALSE},
    {OtherText_TheAnswer, OtherText_ConfirmTrendyPage2, FALSE},
    {OtherText_ConfirmTrendyPage1, OtherText_ConfirmTrendyPage2, TRUE},
    {OtherText_HipsterPage1, OtherText_HipsterPage2, TRUE},
    {OtherText_WithFourPhrases, OtherText_CombineNinePhrasesPage2, TRUE},
};

static const u8 sEasyChatPromptPairsByType[][2] =
{
    { 0,  6},
    { 1,  7},
    { 1,  8},
    { 1,  9},
    { 2, 10},
    {16, 13},
    { 4, 12},
    { 3, 13},
    { 3, 13},
    { 5, 14},
    { 3, 13},
    { 3, 13},
    { 3, 13},
    {15, 13},
};

void CB2_InitEasyChatScreen(void);
void InitEasyChatScreenLayout(void);
void SetUnlockedEasyChatGroups(void);
void InitAlphabetKeyboardRows(void);
void InitEasyChatPromptText(void);
void SetEasyChatScreenCallback(void (*)(void));
void ShowEasyChatTitleAndPortrait(void);
void VBlankCB_EasyChatScreen(void);
void CB2_EasyChatScreen(void);
void WaitEasyChatFadeIn(void);
void InitEasyChatMainScreen(void);
void HandleEasyChatMainScreenInput(void);
void HandleEasyChatDeleteAllPrompt(void);
void HandleEasyChatExitPrompt(void);
void HandleEasyChatConfirmWordsPrompt(void);
void HandleEasyChatOpenKeyboard(void);
void HandleEasyChatKeyboardInput(void);
void HandleEasyChatCloseKeyboard(void);
void SwitchKeyboardMode(void);
void HandleEasyChatOpenWordSelect(void);
void HandleEasyChatWordSelectInput(void);
void SelectNewWord(void);
void HandleEasyChatReturnToKeyboard(void);
void ScrollEasyChatList(void);
void ExitEasyChatScreen(void);
void ClearUnusedField(void);
bool8 MoveMainCursor(void);
bool8 MoveKeyboardCursor(void);
void ReduceToValidKeyboardColumn(void);
void SelectWordGroupFromKeyboardCursor(void);
bool8 MoveWordSelectCursor(void);
void ReduceToValidWordSelectColumn(void);
void ResetCurrentPhrase(void);
void SaveCurrentPhrase(void);
bool8 TrySetSelectedWord(void);
void ResetCurrentPhraseToSaved(void);
void SetCurrentPhraseWord(u16, u16);
u8 DidPhraseChange(void);
bool8 IsCurrentPhraseEmpty(void);
u8 DidPlayerInputMysteryEventPhrase(void);
u8 DidPlayerInputABerryMasterWifePhrase(void);
void BufferCurrentPhraseToStringVar2(void);
void CloseEasyChatPrompt(void);
void InitEasyChatSprites(void);

void SetMainCursorState(u8 state);
void CreateRectangleCursorSprites(void);
void DestroyRectangleCursorSprites(void);
void SetWordSelectCursorVisibility(u8 visible);

void HideScrollIndicatorSprites(void);
void SetScrollIndicatorMode(u8 mode);

void CreateInterviewObjectEvents(u8 personType, u8 frameId);
void CreateModeWindowSprite(void);
void UpdateModeWindowAnim(void);
void SetModeWindowAnimForVisibility(u8 visible);
void InitEasyChatScreenGraphics(void);
void PrintEasyChatStdMessage(u8 messageId);
void PrintTitle(u8 type);
void PrintCurrentPhrase(void);
void PrintCurrentPhraseAsText(void);
void ClearPhraseWindow(void);
void PrintKeyboardText(void);
void RestoreKeyboardScrollOffset(void);
void ResetLowerWindowScroll(void);
void ClearLowerWindow(void);
void PrintInitialWordSelectText(void);
void PrintWordSelectRowsDuringScroll(void);
void PrintKeyboardRowsDuringScroll(void);
void InitLowerWindowScroll(u8 speed);
bool8 UpdateLowerWindowScroll(void);
void ResetLowerWindowAnimState(void);
bool8 OpenKeyboard(void);
bool8 CloseKeyboard(void);
bool8 StartSwitchKeyboardMode(void);
bool8 FinishSwitchKeyboardMode(void);
bool8 OpenWordSelect(void);
bool8 ReturnToKeyboard(void);
bool8 CloseWordSelect(void);
void CopyLowerWindowAnimToVram(void);
void UpdateEasyChatScanlineEffect(void);
u8 IsEasyChatGroupUnlocked(u8);
void SetUnlockedWordsByAlphabet(void);
void LoadEasyChatStrings(void);
void SetSelectedWordGroup(void);
u8 *CopyEasyChatWordPadded(u8 *, u16, u16);
u16 GetEasyChatWordStringLength(u16 easyChatWord);
bool8 CanPhraseFitInXRowsYCols(u16 *, u16, u16, u16);

void ShowEasyChatScreen(void)
{
    u8 displayedPersonType = 3;
    u16 *words;

    switch (gSpecialVar_0x8004)
    {
    case EASY_CHAT_TYPE_PROFILE:
        words = gSaveBlock1.easyChats.unk2B1C;
        break;
    case 1:
        words = gSaveBlock1.easyChats.unk2B28;
        break;
    case 2:
        words = gSaveBlock1.easyChats.unk2B34;
        break;
    case 3:
        words = gSaveBlock1.easyChats.unk2B40;
        break;
    case 4:
        words = gSaveBlock1.mail[gSpecialVar_0x8005].words;
        break;
    case EASY_CHAT_TYPE_BARD_SONG:
        {
            struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;
            u16 i;
            for (i = 0; i < 6; i++)
                bard->newSongLyrics[i] = bard->songLyrics[i];
            words = bard->newSongLyrics;
        }
        break;
    case 5:
        // TODO: Is this the right TV show?
        words = gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubLetter.pad04;
        displayedPersonType = gSpecialVar_0x8006;
        break;
    case 7:
        // TODO: Is this the right TV show?
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubOpinions.var1C[gSpecialVar_0x8006];
        displayedPersonType = 1;
        break;
    case 8:
        // TODO: Is this the right TV show?
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubOpinions.var02;
        displayedPersonType = 0;
        break;
    case EASY_CHAT_TYPE_TRENDY_PHRASE:
        words = NULL;
        break;
    case EASY_CHAT_TYPE_GABBY_AND_TY:
        words = &gSaveBlock1.gabbyAndTyData.quote;
        *words = 0xFFFF;
        displayedPersonType = 1;
        break;
    case 11:
        // TODO: Is this the right TV show?
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].bravoTrainer.var04[gSpecialVar_0x8006];
        displayedPersonType = 0;
        break;
    case 12:
        // TODO: Is this the right TV show?
        words = gSaveBlock1.tvShows[gSpecialVar_0x8005].bravoTrainerTower.var18;
        displayedPersonType = 1;
        break;
    case EASY_CHAT_TYPE_GOOD_SAYING:
        gEasyChatStruct->currentPhrase[0] = 0xFFFF;
        gEasyChatStruct->currentPhrase[1] = -1;
        words = gEasyChatStruct->currentPhrase;
        break;
    default:
        return;
    }
    DoEasyChatScreen(gSpecialVar_0x8004, words, CB2_ReturnToFieldContinueScript, displayedPersonType);
}

void DoEasyChatScreen(u8 type, u16 *words, void (*exitCallback)(void), u8 displayedPersonType)
{
    gEasyChatStruct->unk0 = exitCallback;
    gEasyChatStruct->unk4 = words;
    gEasyChatStruct->unk8 = type;
    gEasyChatStruct->unkB = displayedPersonType;
    if (type == EASY_CHAT_TYPE_TRENDY_PHRASE)
    {
        gEasyChatStruct->unk4 = gEasyChatStruct->currentPhrase;
        gEasyChatStruct->currentPhrase[0] = gSaveBlock1.dewfordTrends[0].words[0];
        gEasyChatStruct->currentPhrase[1] = gSaveBlock1.dewfordTrends[0].words[1];
    }
    SetMainCallback2(CB2_InitEasyChatScreen);
}

void CB2_InitEasyChatScreen(void)
{
    switch (gMain.state)
    {
    case 0:
    default:
        REG_DISPCNT = 0;
        SetVBlankCallback(0);
        ResetPaletteFade();
        ResetSpriteData();
        ScanlineEffect_Clear();
        ScanlineEffect_Stop();
        UpdateEasyChatScanlineEffect();
        ScanlineEffect_SetParams(sEasyChatScanlineParams);
        FreeSpriteTileRanges();
        FreeAllSpritePalettes();
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E6DA8);
        break;
    case 2:
        InitMenuWindow(&gWindowTemplate_81E6D54);
        InitMenuWindow(&gWindowTemplate_81E6DA8);
        Menu_EraseScreen();
        break;
    case 3:
        InitEasyChatScreenLayout();
        break;
    case 4:
        InitEasyChatScreenGraphics();
        break;
    case 5:
        InitEasyChatSprites();
        CreateModeWindowSprite();
        break;
    case EASY_CHAT_TYPE_BARD_SONG:
        ShowEasyChatTitleAndPortrait();
        SetEasyChatScreenCallback(WaitEasyChatFadeIn);
        SetVBlankCallback(VBlankCB_EasyChatScreen);
        break;
    case 7:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        break;
    case 8:
        REG_DISPCNT = 0x1F40;
        SetMainCallback2(CB2_EasyChatScreen);
        FlagSet(FLAG_SYS_CHAT_USED);
        break;
    }
    gMain.state++;
}

void InitEasyChatScreenLayout(void)
{
    gEasyChatStruct->unk9 = sEasyChatLayoutByType[gEasyChatStruct->unk8];
    switch (gEasyChatStruct->unk9)
    {
    case 4:
        gEasyChatStruct->unkA = 4;
        gEasyChatStruct->unk83 = 2;
        gEasyChatStruct->unk84 = 2;
        gEasyChatStruct->unk88 = 5;
        gEasyChatStruct->unk8A = 4;
        break;
    case 5:
        gEasyChatStruct->unkA = 4;
        gEasyChatStruct->unk83 = 1;
        gEasyChatStruct->unk84 = 4;
        gEasyChatStruct->unk88 = 16;
        gEasyChatStruct->unk8A = 2;
        break;
    case 0:
        gEasyChatStruct->unkA = 6;
        gEasyChatStruct->unk83 = 2;
        gEasyChatStruct->unk84 = 3;
        gEasyChatStruct->unk88 = 4;
        gEasyChatStruct->unk8A = 3;
        break;
    case 1:
        gEasyChatStruct->unkA = 9;
        gEasyChatStruct->unk83 = 2;
        gEasyChatStruct->unk84 = 5;
        gEasyChatStruct->unk88 = 4;
        gEasyChatStruct->unk8A = 0;
        break;
    case 2:
        gEasyChatStruct->unkA = 1;
        gEasyChatStruct->unk83 = 1;
        gEasyChatStruct->unk84 = 1;
        gEasyChatStruct->unk88 = 16;
        gEasyChatStruct->unk8A = 4;
        break;
    case 3:
        gEasyChatStruct->unkA = 2;
        gEasyChatStruct->unk83 = 2;
        gEasyChatStruct->unk84 = 1;
        gEasyChatStruct->unk88 = 5;
        gEasyChatStruct->unk8A = 3;
        break;
    }
    gEasyChatStruct->unk86 = 0;
    gEasyChatStruct->unk85 = 0;
    gEasyChatStruct->unk87 = 0;
    gEasyChatStruct->unk26 = 0;
    gEasyChatStruct->unk1BA = 0;
    gEasyChatStruct->unk1BE = 2;
    SetUnlockedEasyChatGroups();
    SetUnlockedWordsByAlphabet();
    LoadEasyChatStrings();
    ResetCurrentPhraseToSaved();
    InitAlphabetKeyboardRows();
    InitEasyChatPromptText();
}

void SetUnlockedEasyChatGroups(void)
{
    u16 r4 = 0;
    u16 r7;
    u16 r5;

    for (r7 = 0; ; r7++)
    {
        for (r5 = 0; r5 < 2; r5++)
        {
            gEasyChatStruct->unk2A[r7][r5] = r4++;
            if (r4 == 17)
                break;
        }
        if (r4 == 17)
            break;
    }
    gEasyChatStruct->unk28 = 17;
    while (r4 < 22)
    {
        if (IsEasyChatGroupUnlocked(r4) != 0)
        {
            r5++;
            if (r5 > 1)
            {
                r7++;
                r5 = 0;
            }
            gEasyChatStruct->unk2A[r7][r5] = r4;
            gEasyChatStruct->unk78[r4 - 17] = 1;  // hmm...
            gEasyChatStruct->unk28++;
        }
        else
        {
            gEasyChatStruct->unk78[r4 - 17] = 0;
        }
        r4++;
    }
    gEasyChatStruct->unk1B6 = (gEasyChatStruct->unk28 + 1) / 2;
}

void InitAlphabetKeyboardRows(void)
{
    u8 i;
    u8 r3;

    for (i = 0; i < 4; i++)
    {
        const u8 *row = sAlphabetKeyboardRows[i];

        for (r3 = 0; row[r3] != EOS; r3++)
        {
            if (row[r3] != CHAR_SPACE)
                gEasyChatStruct->unk40[i][r3] = row[r3] + 0x46;
            else
                gEasyChatStruct->unk40[i][r3] = CHAR_SPACE;
        }
    }
}

void InitEasyChatPromptText(void)
{
    u8 *pointers[] =
    {
        gEasyChatStruct->unk9C80, gEasyChatStruct->unk9CC9,
        gEasyChatStruct->unk9D12, gEasyChatStruct->unk9D5B,
    };
    u8 *r3;
    u16 i;

    for (i = 0; i < 2; i++)
    {
        const struct EasyChatPrompt *prompt = &sEasyChatPrompts[sEasyChatPromptPairsByType[gEasyChatStruct->unk8][i]];

        r3 = StringCopy(pointers[i * 2 + 0], prompt->text1);
        if (prompt->separateLines)
        {
            StringCopy(pointers[i * 2 + 1], prompt->text2);
        }
        else
        {
            *r3++ = CHAR_SPACE;
            StringCopy(r3, prompt->text2);
            *pointers[i * 2 + 1] = EOS;
        }
    }

    for (i = 0; i < 0x24; i++)
        gEasyChatStruct->unk9DA4[i] = 0;
    gEasyChatStruct->unk9DA4[i] = 0xFF;

    r3 = gEasyChatStruct->unk9F6E;
    r3[0] = EXT_CTRL_CODE_BEGIN;
    r3[1] = 0x11;
    r3[2] = 0xE0;
    r3[3] = 0xFF;
}

// Default profile phrase
static const u16 sDefaultProfileWords[] =
{
#if ENGLISH
    EC_WORD_I_AM,
    EC_WORD_A,
    EC_WORD_POKEMON,
    EC_WORD_GREAT,
#else
    EC_WORD_I_AM,
    EC_WORD_BIG,
    EC_WORD_IN,
    EC_WORD_POKEMON,
#endif
};

static const u16 sDefaultBattleStartWords[] =
{
    EC_WORD_ARE,
    EC_WORD_YOU,
    EC_WORD_READY,
    EC_WORD_QUES,
    EC_WORD_HERE_I_COME,
    EC_WORD_EXCL,
};

// ResetDefaultEasyChatPhrases
void InitEasyChatPhrases(void)
{
    u16 i;
    u16 j;

    for (i = 0; i < 4; i++)
        gSaveBlock1.easyChats.unk2B1C[i] = sDefaultProfileWords[i];

    for (i = 0; i < 6; i++)
        gSaveBlock1.easyChats.unk2B28[i] = sDefaultBattleStartWords[i];

    for (i = 0; i < 6; i++)
    {
        gSaveBlock1.easyChats.unk2B34[i] = 0xFFFF;
        gSaveBlock1.easyChats.unk2B40[i] = 0xFFFF;
    }

    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < 9; j++)
            gSaveBlock1.mail[i].words[j] = 0xFFFF;
    }

    for (i = 0; i < 64; i++)
        gSaveBlock1.unlockedTrendySayings[i] = 0;
}

void SetEasyChatScreenCallback(void (*func)(void))
{
    gEasyChatStruct->unk20 = func;
    gEasyChatStruct->unk24 = 0;
}

void InitKeyboardSelection(void)
{
    u16 i;

    if (gEasyChatStruct->unk26 == 0)
    {
        for (i = 0; i < gEasyChatStruct->unk1B6; i++)
            gEasyChatStruct->unk1AA[i] = 2;
        gEasyChatStruct->unk1AA[i - 1] = gEasyChatStruct->unk28 % 2;
        if (gEasyChatStruct->unk1AA[i - 1] == 0)
            gEasyChatStruct->unk1AA[i - 1] = 2;
    }
    else
    {
        gEasyChatStruct->unk1AA[0] = 7;
        gEasyChatStruct->unk1AA[1] = 6;
        gEasyChatStruct->unk1AA[2] = 7;
        gEasyChatStruct->unk1AA[3] = 7;
    }
    gEasyChatStruct->unk1A8 = 0;
    gEasyChatStruct->unk1A9 = 0;
    gEasyChatStruct->unk1B5 = 0;
    gEasyChatStruct->unk1B7 = 0;
    ResetLowerWindowScroll();
}

void InitWordSelectSelection(void)
{
    SetSelectedWordGroup();
    if (gEasyChatStruct->unk26 == 0)
    {
        u16 i;
        u8 r6;

        r6 = gEasyChatStruct->unk1B8;
        gEasyChatStruct->unk9A28 = (gEasyChatStruct->unk4178[r6] + 1) / 2;
        for (i = 0; i < gEasyChatStruct->unk9A28; i++)
            gEasyChatStruct->unk99A6[i] = 2;
        i--;
        gEasyChatStruct->unk99A6[i] = gEasyChatStruct->unk4178[r6] % 2;
        if (gEasyChatStruct->unk99A6[i] == 0)
            gEasyChatStruct->unk99A6[i] = 2;
    }
    else
    {
        u16 i;
        u8 r6;

        r6 = gEasyChatStruct->unk1B8;
        gEasyChatStruct->unk9A28 = (gEasyChatStruct->unk4142[r6] + 1) / 2;
        for (i = 0; i < gEasyChatStruct->unk9A28; i++)
            gEasyChatStruct->unk99A6[i] = 2;
        i--;
        gEasyChatStruct->unk99A6[i] = gEasyChatStruct->unk4142[r6] % 2;
        if (gEasyChatStruct->unk99A6[i] == 0)
            gEasyChatStruct->unk99A6[i] = 2;
    }
    gEasyChatStruct->unk99A4 = 0;
    gEasyChatStruct->unk99A5 = 0;
    gEasyChatStruct->unk9A29 = 0;
    ResetLowerWindowScroll();
}

void ShowEasyChatTitleAndPortrait(void)
{
    switch (gEasyChatStruct->unk8)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case EASY_CHAT_TYPE_BARD_SONG:
    case 9:
    case 13:
    default:
        PrintTitle(gEasyChatStruct->unk8);
        break;
    case 5:
    case 7:
    case 8:
    case EASY_CHAT_TYPE_GABBY_AND_TY:
    case 11:
    case 12:
        PrintTitle(gEasyChatStruct->unk8);
        CreateInterviewObjectEvents(gEasyChatStruct->unkB, gEasyChatStruct->unk9);
        break;
    }
}

void VBlankCB_EasyChatScreen(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    CopyLowerWindowAnimToVram();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void CB2_EasyChatScreen(void)
{
    gEasyChatStruct->unk20();
    AnimateSprites();
    BuildOamBuffer();
    UpdateEasyChatScanlineEffect();
}

void WaitEasyChatFadeIn(void)
{
    if (!UpdatePaletteFade())
        SetEasyChatScreenCallback(InitEasyChatMainScreen);
}

void InitEasyChatMainScreen(void)
{
    HideScrollIndicatorSprites();
    SetMainCursorState(0);
    PrintEasyChatStdMessage(0);
    SetEasyChatScreenCallback(HandleEasyChatMainScreenInput);
}

void HandleEasyChatMainScreenInput(void)
{
    gEasyChatStruct->unk87 = MoveMainCursor();
    if (gEasyChatStruct->unk87)
        PlaySE(SE_SELECT);
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gEasyChatStruct->unk86 == gEasyChatStruct->unk84)
        {
            switch (gEasyChatStruct->unk85)
            {
            case 0:
                SetEasyChatScreenCallback(HandleEasyChatDeleteAllPrompt);
                return;
            case 1:
                SetEasyChatScreenCallback(HandleEasyChatExitPrompt);
                return;
            case 2:
                SetEasyChatScreenCallback(HandleEasyChatConfirmWordsPrompt);
                return;
            }
        }
        else
        {
            gEasyChatStruct->unk27 = gEasyChatStruct->unk86 * gEasyChatStruct->unk83 + gEasyChatStruct->unk85;
            ClearUnusedField();
            SetEasyChatScreenCallback(HandleEasyChatOpenKeyboard);
            return;
        }
    }
    if (JOY_NEW(B_BUTTON))
    {
        SetEasyChatScreenCallback(HandleEasyChatExitPrompt);
    }
}

void HandleEasyChatDeleteAllPrompt(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        SetMainCursorState(2);
        if (gEasyChatStruct->unk8 == 6)
        {
            PrintEasyChatStdMessage(6);
            gEasyChatStruct->unk24 = 100;
        }
        else
        {
            PrintEasyChatStdMessage(2);
            DisplayYesNoMenu(23, 8, 1);
            Menu_MoveCursor(1);
            gEasyChatStruct->unk24++;
        }
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            ResetCurrentPhrase();
            ClearPhraseWindow();
            PrintCurrentPhrase();
            gEasyChatStruct->unk24++;
            break;
        case -1:
        case 1:
            gEasyChatStruct->unk24++;
            break;
        }
        break;
    case 2:
        CloseEasyChatPrompt();
        SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    case 100:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
            SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    }
}

void HandleEasyChatExitPrompt(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        SetMainCursorState(2);
        PrintEasyChatStdMessage(3);
        DisplayYesNoMenu(23, 8, 0);
        Menu_MoveCursor(1);
        if (gEasyChatStruct->unk8 == 9
         || gEasyChatStruct->unk8 == 4
         || gEasyChatStruct->unk8 == 7
         || gEasyChatStruct->unk8 == 8
         || gEasyChatStruct->unk8 == 10
         || gEasyChatStruct->unk8 == 11
         || gEasyChatStruct->unk8 == 12
         || gEasyChatStruct->unk8 == 5
         || gEasyChatStruct->unk8 == 13)
            gEasyChatStruct->unk24 = 2;
        else
            gEasyChatStruct->unk24++;
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            PrintEasyChatStdMessage(4);
            DisplayYesNoMenu(23, 8, 0);
            Menu_MoveCursor(1);
            gEasyChatStruct->unk24++;
            break;
        case -1:
        case 1:
            gEasyChatStruct->unk24 = 0xFF;
            break;
        }
        break;
    case 2:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            gSpecialVar_Result = 0;
            SetEasyChatScreenCallback(ExitEasyChatScreen);
            break;
        case -1:
        case 1:
            gEasyChatStruct->unk24 = 0xFF;
            break;
        }
        break;
    case 0xFF:
        Menu_DestroyCursor();
        CloseEasyChatPrompt();
        SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    }
}

void HandleEasyChatConfirmWordsPrompt(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        SetMainCursorState(2);
        if (IsCurrentPhraseEmpty())
        {
            PrintEasyChatStdMessage(5);
            gEasyChatStruct->unk24 = 10;
            break;
        }
        if (gEasyChatStruct->unk8 == 9)
        {
            if (DidPhraseChange() == 0)
            {
                PrintEasyChatStdMessage(8);
                gEasyChatStruct->unk24 = 10;
                break;
            }
            if (gEasyChatStruct->unkC[0] == 0xFFFF || gEasyChatStruct->unkC[1] == 0xFFFF)
            {
                PrintEasyChatStdMessage(9);
                gEasyChatStruct->unk24 = 10;
                break;
            }
        }
        if (gEasyChatStruct->unk8 == 4 && DidPhraseChange() == 0)
        {
            SetEasyChatScreenCallback(HandleEasyChatExitPrompt);
        }
        else
        {
            PrintEasyChatStdMessage(1);
            PrintCurrentPhraseAsText();
            DisplayYesNoMenu(23, 8, 0);
            Menu_MoveCursor(0);
            gEasyChatStruct->unk24++;
        }
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            gSpecialVar_Result = (DidPhraseChange() != 0);
            SaveCurrentPhrase();
            if (gEasyChatStruct->unk8 == 0)
                gSpecialVar_0x8004 = DidPlayerInputMysteryEventPhrase();
            if (gEasyChatStruct->unk8 == 9)  // dewford trend?
            {
                BufferCurrentPhraseToStringVar2();
                gSpecialVar_0x8004 = TrySetTrendyPhrase(gEasyChatStruct->currentPhrase);
            }
            if (gEasyChatStruct->unk8 == 13)
            {
                if (gEasyChatStruct->unkC[0] == 0xFFFF || gEasyChatStruct->unkC[1] == 0xFFFF)
                    gSpecialVar_Result = 0;
                gSpecialVar_0x8004 = DidPlayerInputABerryMasterWifePhrase();
            }
            SetEasyChatScreenCallback(ExitEasyChatScreen);
            break;
        case -1:
        case 1:
            Menu_DestroyCursor();
            CloseEasyChatPrompt();
            if (gEasyChatStruct->unk8 == 6 && DidPhraseChange() != 0)
            {
                gEasyChatStruct->unk24 = 100;
            }
            else
            {
                PrintCurrentPhrase();
                SetEasyChatScreenCallback(InitEasyChatMainScreen);
            }
            break;
        }
        break;
    case 10:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
            SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    case 100:
        PrintEasyChatStdMessage(7);
        gEasyChatStruct->unk24++;
        // fall through
    case 101:
        if (JOY_NEW(A_BUTTON))
            gEasyChatStruct->unk24++;
        break;
    case 102:
        ResetCurrentPhraseToSaved();
        PrintCurrentPhrase();
        SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    }
}

void HandleEasyChatOpenKeyboard(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        SetMainCursorState(1);
        PrintEasyChatStdMessage(10);
        InitKeyboardSelection();
        PrintKeyboardText();
        ResetLowerWindowAnimState();
        gEasyChatStruct->unk24++;
        break;
    case 1:
        if (OpenKeyboard() != 0)
        {
            SetModeWindowAnimForVisibility(1);
            CreateRectangleCursorSprites();
            SetScrollIndicatorMode(0);
            SetEasyChatScreenCallback(HandleEasyChatKeyboardInput);
        }
        break;
    }
}

void HandleEasyChatKeyboardInput(void)
{
    gEasyChatStruct->unk96 = MoveKeyboardCursor();
    if (gEasyChatStruct->unk1C0 != 0)
    {
        PlaySE(SE_SELECT);
        gEasyChatStruct->unk1C4 = HandleEasyChatKeyboardInput;
        SetEasyChatScreenCallback(ScrollEasyChatList);
    }
    else
    {
        if (gEasyChatStruct->unk96)
            PlaySE(SE_SELECT);
        if (JOY_NEW(A_BUTTON))
        {
            if (gEasyChatStruct->unk1B7 != 0)
            {
                PlaySE(SE_SELECT);
                switch (gEasyChatStruct->unk1A8)
                {
                case 1:
                    SetEasyChatScreenCallback(SwitchKeyboardMode);
                    break;
                case 2:
                    if (gEasyChatStruct->unk8 != 6)
                    {
                        SetCurrentPhraseWord(gEasyChatStruct->unk27, 0xFFFF);
                        ClearUnusedField();
                        PrintCurrentPhrase();
                    }
                    break;
                case 3:
                    SetEasyChatScreenCallback(HandleEasyChatCloseKeyboard);
                    break;
                }
            }
            else
            {
                if (gEasyChatStruct->unk26 == 0
                 || gEasyChatStruct->unk4142[gEasyChatStruct->unk40[gEasyChatStruct->unk1A8][gEasyChatStruct->unk1A9]] != 0)
                {
                    PlaySE(SE_SELECT);
                    SelectWordGroupFromKeyboardCursor();
                    SetEasyChatScreenCallback(HandleEasyChatOpenWordSelect);
                }
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            SetEasyChatScreenCallback(HandleEasyChatCloseKeyboard);
        }
        else if (JOY_NEW(SELECT_BUTTON))
        {
            SetEasyChatScreenCallback(SwitchKeyboardMode);
        }
    }
}

void HandleEasyChatCloseKeyboard(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        DestroyRectangleCursorSprites();
        ResetLowerWindowAnimState();
        HideScrollIndicatorSprites();
        SetModeWindowAnimForVisibility(0);
        gEasyChatStruct->unk24++;
        break;
    case 1:
    case 2:
        gEasyChatStruct->unk24++;
        break;
    case 3:
        if (CloseKeyboard() != 0)
            gEasyChatStruct->unk24++;
        break;
    case 4:
        SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    }
}

void SwitchKeyboardMode(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        DestroyRectangleCursorSprites();
        ResetLowerWindowAnimState();
        HideScrollIndicatorSprites();
        gEasyChatStruct->unk24++;
        UpdateModeWindowAnim();
        break;
    case 1:
        if (StartSwitchKeyboardMode() != 0)
        {
            gEasyChatStruct->unk26 = !gEasyChatStruct->unk26;
            InitKeyboardSelection();
            PrintKeyboardText();
            ResetLowerWindowAnimState();
            gEasyChatStruct->unk24++;
        }
        break;
    default:
        gEasyChatStruct->unk24++;
        break;
    case 8:
        if (FinishSwitchKeyboardMode() != 0)
        {
            CreateRectangleCursorSprites();
            SetScrollIndicatorMode(0);
            SetEasyChatScreenCallback(HandleEasyChatKeyboardInput);
        }
        break;
    }
}

void HandleEasyChatOpenWordSelect(void)
{
    switch (gEasyChatStruct->unk24)
    {
    default:
        gEasyChatStruct->unk24++;
        break;
    case 8:
        SetModeWindowAnimForVisibility(0);
        DestroyRectangleCursorSprites();
        ClearLowerWindow();
        InitWordSelectSelection();
        HideScrollIndicatorSprites();
        ResetLowerWindowAnimState();
        gEasyChatStruct->unk24++;
        break;
    case 9:
        if (OpenWordSelect() != 0)
        {
            PrintInitialWordSelectText();
            gEasyChatStruct->unk24++;
        }
        break;
    case 10:
        SetWordSelectCursorVisibility(1);
        SetScrollIndicatorMode(1);
        SetEasyChatScreenCallback(HandleEasyChatWordSelectInput);
        break;
    case 11:
        break;
    }
}

void HandleEasyChatWordSelectInput(void)
{
    gEasyChatStruct->unk1B9 = MoveWordSelectCursor();
    if (gEasyChatStruct->unk1C0 != 0)
    {
        PlaySE(SE_SELECT);
        gEasyChatStruct->unk1C4 = HandleEasyChatWordSelectInput;
        SetEasyChatScreenCallback(ScrollEasyChatList);
    }
    else
    {
        if (gEasyChatStruct->unk1B9)
            PlaySE(SE_SELECT);
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            SetEasyChatScreenCallback(SelectNewWord);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            SetEasyChatScreenCallback(HandleEasyChatReturnToKeyboard);
        }
    }
}

void SelectNewWord(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        if (!TrySetSelectedWord())
        {
            SetEasyChatScreenCallback(HandleEasyChatWordSelectInput);
        }
        else
        {
            HideScrollIndicatorSprites();
            SetWordSelectCursorVisibility(0);
            gEasyChatStruct->unk24++;
        }
        break;
    case 1:
        gEasyChatStruct->unk24++;
        break;
    case 2:
        ResetLowerWindowAnimState();
        gEasyChatStruct->unk24++;
        break;
    case 3:
        if (CloseWordSelect() != 0)
            gEasyChatStruct->unk24++;
        break;
    case 4:
        if (gEasyChatStruct->unk8 == 6 && DidPhraseChange() != 0)
            SetEasyChatScreenCallback(HandleEasyChatConfirmWordsPrompt);
        else
            SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    }
}

void HandleEasyChatReturnToKeyboard(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        SetWordSelectCursorVisibility(0);
        HideScrollIndicatorSprites();
        gEasyChatStruct->unk24++;
        break;
    case 1:
        ClearLowerWindow();
        ResetLowerWindowAnimState();
        gEasyChatStruct->unk24++;
        break;
    case 2:
        if (ReturnToKeyboard() != 0)
        {
            SetModeWindowAnimForVisibility(1);
            RestoreKeyboardScrollOffset();
            gEasyChatStruct->unk24++;
        }
        break;
    case 3:
        CreateRectangleCursorSprites();
        SetScrollIndicatorMode(0);
        gEasyChatStruct->unk24++;
        break;
    case 4:
        PrintKeyboardText();
        SetEasyChatScreenCallback(HandleEasyChatKeyboardInput);
        break;
    }
}

void ScrollEasyChatList(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        if (gEasyChatStruct->unk1C4 == HandleEasyChatKeyboardInput)
            PrintKeyboardRowsDuringScroll();
        else
            PrintWordSelectRowsDuringScroll();
        InitLowerWindowScroll(gEasyChatStruct->unk1BE);
        gEasyChatStruct->unk24++;
        break;
    case 1:
        if (UpdateLowerWindowScroll())
        {
            if (gEasyChatStruct->unk1C4 == HandleEasyChatKeyboardInput)
            {
                PrintKeyboardRowsDuringScroll();
                gEasyChatStruct->unk1B5 += gEasyChatStruct->unk1C0;
                ReduceToValidKeyboardColumn();
                gEasyChatStruct->unk96 = TRUE;
            }
            else
            {
                gEasyChatStruct->unk9A29 += gEasyChatStruct->unk1C0;
                ReduceToValidWordSelectColumn();
                gEasyChatStruct->unk1B9 = 1;
            }
            gEasyChatStruct->unk1BE = 2;
            SetEasyChatScreenCallback(gEasyChatStruct->unk1C4);
        }
        break;
    }
}

void ExitEasyChatScreen(void)
{
    switch (gEasyChatStruct->unk24)
    {
    case 0:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gEasyChatStruct->unk24++;
        break;
    case 1:
        if (!UpdatePaletteFade())
            SetMainCallback2(gEasyChatStruct->unk0);
        break;
    }
}

void ClearUnusedField(void)
{
    if (gEasyChatStruct->unk8 == 1
     && gEasyChatStruct->unk7E[gEasyChatStruct->unk86] == 2
     && GetEasyChatWordStringLength(gEasyChatStruct->unkC[gEasyChatStruct->unk27]) != 7)
        gEasyChatStruct->unk7D = 1;
    else
        gEasyChatStruct->unk7D = 0;
    gEasyChatStruct->unk7D = 0;  // What the hell?
}

bool8 MoveMainCursor(void)
{
    bool8 pressedUpDown = FALSE;
    u8 r0;

    if (JOY_NEW(START_BUTTON))
    {
        gEasyChatStruct->unk86 = gEasyChatStruct->unk84;
        gEasyChatStruct->unk85 = 2;
        return TRUE;
    }

    if (JOY_REPT(DPAD_UP))
    {
        gEasyChatStruct->unk86--;
        if (gEasyChatStruct->unk86 < 0)
            gEasyChatStruct->unk86 = gEasyChatStruct->unk84;
        pressedUpDown = TRUE;
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        gEasyChatStruct->unk86++;
        if (gEasyChatStruct->unk86 > gEasyChatStruct->unk84)
            gEasyChatStruct->unk86 = 0;
        pressedUpDown = TRUE;
    }

    if (pressedUpDown)
    {
        if (gEasyChatStruct->unk9 == 2)
        {
            if (gEasyChatStruct->unk86 == gEasyChatStruct->unk84)
                gEasyChatStruct->unk85 = 2;
            else
                gEasyChatStruct->unk85 = 0;
            return TRUE;
        }
        else
        {
            if (gEasyChatStruct->unk85 >= gEasyChatStruct->unk83)
                gEasyChatStruct->unk85 = gEasyChatStruct->unk83 - 1;
            if (gEasyChatStruct->unk86 != gEasyChatStruct->unk84)
            {
                r0 = gEasyChatStruct->unk86 * gEasyChatStruct->unk83 + gEasyChatStruct->unk85;
                if (r0 >= gEasyChatStruct->unkA)
                    gEasyChatStruct->unk85 = r0 - gEasyChatStruct->unkA;
            }
            return TRUE;
        }
    }
    else
    {
        if (JOY_REPT(DPAD_LEFT))
        {
            if (--gEasyChatStruct->unk85 < 0)
            {
                if (gEasyChatStruct->unk86 == gEasyChatStruct->unk84)
                {
                    gEasyChatStruct->unk85 = 2;
                }
                else
                {
                    gEasyChatStruct->unk85 = gEasyChatStruct->unk83 - 1;
                    r0 = gEasyChatStruct->unk86 * gEasyChatStruct->unk83 + gEasyChatStruct->unk85;
                    if (r0 >= gEasyChatStruct->unkA)
                        gEasyChatStruct->unk85 = r0 - gEasyChatStruct->unkA;
                }
            }
            return TRUE;
        }
        if (JOY_REPT(DPAD_RIGHT))
        {
            if (gEasyChatStruct->unk86 == gEasyChatStruct->unk84)
            {
                if (++gEasyChatStruct->unk85 > 2)
                    gEasyChatStruct->unk85 = 0;
            }
            else
            {
                if (++gEasyChatStruct->unk85 >= gEasyChatStruct->unk83)
                    gEasyChatStruct->unk85 = 0;
                r0 = gEasyChatStruct->unk86 * gEasyChatStruct->unk83 + gEasyChatStruct->unk85;
                if (r0 >= gEasyChatStruct->unkA)
                    gEasyChatStruct->unk85 = r0 - gEasyChatStruct->unkA;
            }
            return TRUE;
        }
    }
    return FALSE;
}

bool8 MoveKeyboardCursor(void)
{
    bool8 pressedLeftRight = FALSE;
    bool8 pressedUpDown;

    if (gEasyChatStruct->unk1B7 != 0)
    {
        if (JOY_REPT(DPAD_UP))
        {
            gEasyChatStruct->unk1A8--;
            if (gEasyChatStruct->unk1A8 < 1)
                gEasyChatStruct->unk1A8 = 3;
            return TRUE;
        }
        else if (JOY_REPT(DPAD_DOWN))
        {
            gEasyChatStruct->unk1A8++;
            if (gEasyChatStruct->unk1A8 > 3)
                gEasyChatStruct->unk1A8 = 1;
            return TRUE;
        }
    }
    else
    {
        if (gEasyChatStruct->unk26 == 1)
        {
            pressedUpDown = FALSE;

            if (JOY_REPT(DPAD_UP))
            {
                gEasyChatStruct->unk1A8--;
                if (gEasyChatStruct->unk1A8 < 0)
                    gEasyChatStruct->unk1A8 = 3;
                pressedUpDown = TRUE;
            }
            else if (JOY_REPT(DPAD_DOWN))
            {
                gEasyChatStruct->unk1A8++;
                if (gEasyChatStruct->unk1A8 > 3)
                    gEasyChatStruct->unk1A8 = 0;
                pressedUpDown = TRUE;
            }

            if (pressedUpDown)
            {
                ReduceToValidKeyboardColumn();
                return TRUE;
            }
        }
        else
        {
            pressedUpDown = FALSE;
            gEasyChatStruct->unk1C0 = 0;

            if (JOY_REPT(DPAD_UP))
            {
                if (gEasyChatStruct->unk1A8 == 0)
                    return FALSE;
                gEasyChatStruct->unk1A8--;
                if (gEasyChatStruct->unk1A8 < gEasyChatStruct->unk1B5)
                    gEasyChatStruct->unk1C0 = -1;
                pressedUpDown = TRUE;
            }
            else if (JOY_REPT(DPAD_DOWN))
            {
                if (gEasyChatStruct->unk1A8 >= gEasyChatStruct->unk1B6 - 1)
                    return FALSE;
                gEasyChatStruct->unk1A8++;
                if (gEasyChatStruct->unk1A8 > gEasyChatStruct->unk1B5 + 3)
                    gEasyChatStruct->unk1C0 = 1;
                pressedUpDown = TRUE;
            }

            if (pressedUpDown)
            {
                if (gEasyChatStruct->unk1C0 == 0)
                {
                    ReduceToValidKeyboardColumn();
                    return TRUE;
                }
                return FALSE;
            }
        }
    }

    if (JOY_REPT(DPAD_LEFT))
    {
        if (gEasyChatStruct->unk1A9 != 0)
            gEasyChatStruct->unk1A9--;
        else
            gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8];
        pressedLeftRight = TRUE;
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        if (gEasyChatStruct->unk1B7 != 0
         || gEasyChatStruct->unk1A9 == gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8])
            gEasyChatStruct->unk1A9 = 0;
        else
            gEasyChatStruct->unk1A9++;
        pressedLeftRight = TRUE;
    }

    if (pressedLeftRight)
    {
        s8 r9 = gEasyChatStruct->unk1B7;

        gEasyChatStruct->unk1B7 = (gEasyChatStruct->unk1A9 == gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8]);
        if (gEasyChatStruct->unk1B7 != 0)
        {
            gEasyChatStruct->unk1A8 -= gEasyChatStruct->unk1B5;
            if (gEasyChatStruct->unk1A8 == 0)
            {
                gEasyChatStruct->unk1A8 = 1;
                gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8];
            }
        }
        else if (r9 != 0)
        {
            gEasyChatStruct->unk1A8 += gEasyChatStruct->unk1B5;
            if (gEasyChatStruct->unk1A9 != 0)
                gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8] - 1;
        }
        return TRUE;
    }

    return FALSE;
}

void ReduceToValidKeyboardColumn(void)
{
    if (gEasyChatStruct->unk1A9 >= gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8])
        gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8] - 1;
}

void SelectWordGroupFromKeyboardCursor(void)
{
    if (gEasyChatStruct->unk26 == 0)
        gEasyChatStruct->unk1B8 = gEasyChatStruct->unk2A[gEasyChatStruct->unk1A8][gEasyChatStruct->unk1A9];
    else
        gEasyChatStruct->unk1B8 = gEasyChatStruct->unk40[gEasyChatStruct->unk1A8][gEasyChatStruct->unk1A9];
}

bool8 MoveWordSelectCursor(void)
{
    bool8 pressedUpDown = FALSE;

    gEasyChatStruct->unk1C0 = 0;
    if (JOY_REPT(DPAD_UP))
    {
        if (gEasyChatStruct->unk99A4 == 0)
            return FALSE;
        gEasyChatStruct->unk99A4--;
        if (gEasyChatStruct->unk99A4 < gEasyChatStruct->unk9A29)
        {
            gEasyChatStruct->unk1C0 = -1;
            return FALSE;
        }
        pressedUpDown = TRUE;
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (gEasyChatStruct->unk99A4 >= gEasyChatStruct->unk9A28 - 1)
            return FALSE;
        gEasyChatStruct->unk99A4++;
        if (gEasyChatStruct->unk99A4 >= gEasyChatStruct->unk9A29 + 4)
        {
            gEasyChatStruct->unk1C0 = 1;
            return FALSE;
        }
        pressedUpDown = TRUE;
    }

    if (pressedUpDown)
    {
        ReduceToValidWordSelectColumn();
        return TRUE;
    }

    if (JOY_REPT(DPAD_LEFT))
    {
        gEasyChatStruct->unk99A5--;
        if (gEasyChatStruct->unk99A5 < 0)
            gEasyChatStruct->unk99A5 = gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4] - 1;
        return TRUE;
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        gEasyChatStruct->unk99A5++;
        if (gEasyChatStruct->unk99A5 >= gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4])
            gEasyChatStruct->unk99A5 = 0;
        return TRUE;
    }

    if (JOY_NEW(START_BUTTON))
    {
        if (gEasyChatStruct->unk9A29 != 0)
        {
            gEasyChatStruct->unk1C0 = -gEasyChatStruct->unk9A29;
            if (gEasyChatStruct->unk1C0 < -4)
                gEasyChatStruct->unk1C0 = -4;
        }
        gEasyChatStruct->unk99A4 += gEasyChatStruct->unk1C0;
        gEasyChatStruct->unk1BE = 4;
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        if (gEasyChatStruct->unk9A29 < gEasyChatStruct->unk9A28 - 4)
        {
            gEasyChatStruct->unk1C0 = gEasyChatStruct->unk9A28 - 4 - gEasyChatStruct->unk9A29;
            if (gEasyChatStruct->unk1C0 > 4)
                gEasyChatStruct->unk1C0 = 4;
        }
        gEasyChatStruct->unk99A4 += gEasyChatStruct->unk1C0;
        gEasyChatStruct->unk1BE = 4;
    }

    return FALSE;
}

void ReduceToValidWordSelectColumn(void)
{
    if (gEasyChatStruct->unk99A5 >= gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4])
        gEasyChatStruct->unk99A5 = gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4] - 1;
}

void ResetCurrentPhrase(void)
{
    u16 i;

    for (i = 0; i < gEasyChatStruct->unkA; i++)
        SetCurrentPhraseWord(i, 0xFFFF);
}

void SaveCurrentPhrase(void)
{
    u16 i;

    for (i = 0; i < gEasyChatStruct->unkA; i++)
        gEasyChatStruct->unk4[i] = gEasyChatStruct->unkC[i];
}

bool8 TrySetSelectedWord(void)
{
    u16 r4 = gEasyChatStruct->unk9A2A[gEasyChatStruct->unk99A4][gEasyChatStruct->unk99A5];

    if (gEasyChatStruct->unk7D != 0
     && gEasyChatStruct->unk7E[gEasyChatStruct->unk86] > 1
     && GetEasyChatWordStringLength(r4) == 7)
        return FALSE;

    SetCurrentPhraseWord(gEasyChatStruct->unk27, r4);
    PrintCurrentPhrase();
    return TRUE;
}

void ResetCurrentPhraseToSaved(void)
{
    u16 r5 = 0;
    u16 i;
    u16 j;

    for (i = 0; i < gEasyChatStruct->unk84; i++)
    {
        gEasyChatStruct->unk7E[i] = 0;
        for (j = 0; j < gEasyChatStruct->unk83; j++)
        {
            gEasyChatStruct->unkC[r5] = gEasyChatStruct->unk4[r5];
            gEasyChatStruct->unk8C[i][j] = 0;
            r5++;
        }
    }
}

void SetCurrentPhraseWord(u16 a, u16 b)
{
    u16 r5 = a / gEasyChatStruct->unk83;
    u16 r8 = a % gEasyChatStruct->unk83;
    u16 r4 = GetEasyChatWordStringLength(gEasyChatStruct->unkC[a]);
    u16 r3 = GetEasyChatWordStringLength(b);

    if (r4 == 7)
    {
        if (r3 != 7)
            gEasyChatStruct->unk7E[r5]--;
    }
    else
    {
        if (r3 == 7)
            gEasyChatStruct->unk7E[r5]++;
    }
    r3 = 0;
    gEasyChatStruct->unk8C[r5][r8] = r3;
    gEasyChatStruct->unkC[a] = b;
}

u8 DidPhraseChange(void)
{
    u16 r8 = 0;
    u16 i;
    u8 *r1;
    u8 *r2;

    for (i = 0; i < gEasyChatStruct->unkA; i++)
    {
        CopyEasyChatWordPadded(gEasyChatStruct->unk9E14, gEasyChatStruct->unk4[i], 0);
        CopyEasyChatWordPadded(gEasyChatStruct->unk9E41, gEasyChatStruct->unkC[i], 0);
        r1 = gEasyChatStruct->unk9E14;
        r2 = gEasyChatStruct->unk9E41;
        while (*r1 == *r2 && *r1 != 0xFF)
        {
            r1++;
            r2++;
        }
        if (*r1 != *r2)
            r8++;
    }
    return r8;
}

bool8 IsCurrentPhraseEmpty(void)
{
    u16 i;

    for (i = 0; i < gEasyChatStruct->unkA; i++)
    {
        if (gEasyChatStruct->unkC[i] != 0xFFFF)
            return FALSE;
    }
    return TRUE;
}

// CheckMysteryEventPhrase
bool8 DidPlayerInputMysteryEventPhrase(void)
{
    u16 i;
    u8 *r3;
    u8 *r4;

    for (i = 0; i < 4; i++)
    {
        CopyEasyChatWordPadded(gEasyChatStruct->unk9E14, gEasyChatStruct->unkC[i], 0);
        CopyEasyChatWordPadded(gEasyChatStruct->unk9E41, sMysteryEventPhrase[i], 0);
        r3 = gEasyChatStruct->unk9E14;
        r4 = gEasyChatStruct->unk9E41;
        while (*r3 != 0xFF && *r4 != 0xFF)
        {
            if (*r3++ != *r4++)
                return FALSE;
        }
        if (*r3 != 0xFF || *r4 != 0xFF)
            return FALSE;
    }
    return TRUE;
}

u8 DidPlayerInputABerryMasterWifePhrase(void)
{
    u16 i;

    for (i = 0; i < 5; i++)
    {
        u8 *ptr;
        u8 *r3;

        ptr = CopyEasyChatWordPadded(gEasyChatStruct->unk9E6E, gEasyChatStruct->unkC[0], 0);
        *ptr++ = CHAR_SPACE;
        CopyEasyChatWordPadded(ptr, gEasyChatStruct->unkC[1], 0);

        ptr = CopyEasyChatWordPadded(gEasyChatStruct->unk9EEE, sBerryMasterWifePhrases[i][0], 0);
        *ptr++ = CHAR_SPACE;
        CopyEasyChatWordPadded(ptr, sBerryMasterWifePhrases[i][1], 0);

        ptr = gEasyChatStruct->unk9E6E;
        r3 = gEasyChatStruct->unk9EEE;
        while (*ptr != EOS && *r3 != EOS)
        {
            if (*ptr++ != *r3++)
                break;
        }
        if (*ptr == EOS && *r3 == EOS)
            return i + 1;
    }
    return 0;
}

void BufferCurrentPhraseToStringVar2(void)
{
    u8 *ptr;

    ptr = CopyEasyChatWordPadded(gStringVar2, gEasyChatStruct->currentPhrase[0], 0);
    *ptr++ = CHAR_SPACE;
    CopyEasyChatWordPadded(ptr, gEasyChatStruct->currentPhrase[1], 0);
}

void CloseEasyChatPrompt(void)
{
    PlaySE(SE_SELECT);
    PrintCurrentPhrase();
    Menu_EraseWindowRect(0, 0, 29, 13);
}
