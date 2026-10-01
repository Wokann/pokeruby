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

extern const struct WindowTemplate gEasyChatInputSelectWindowTemplate;
extern const struct WindowTemplate gEasyChatMessageWindowTemplate;

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

// Ruby keeps the unified Easy Chat state at a fixed shared-memory offset.
struct EasyChatScreen *const gEasyChatScreen = (struct EasyChatScreen *)(gSharedMem + 0x1000);

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
void SetCurrentPhraseWord(u16 phraseWordIndex, u16 wordId);
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
        words = gSaveBlock1.easyChats.easyChatProfile;
        break;
    case EASY_CHAT_TYPE_BATTLE_START:
        words = gSaveBlock1.easyChats.easyChatBattleStart;
        break;
    case EASY_CHAT_TYPE_BATTLE_WON:
        words = gSaveBlock1.easyChats.easyChatBattleWon;
        break;
    case EASY_CHAT_TYPE_BATTLE_LOST:
        words = gSaveBlock1.easyChats.easyChatBattleLost;
        break;
    case EASY_CHAT_TYPE_MAIL:
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
    case EASY_CHAT_TYPE_INTERVIEW:
        // TODO: Is this the right TV show?
        words = gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubLetter.words;
        displayedPersonType = gSpecialVar_0x8006;
        break;
    case EASY_CHAT_TYPE_FAN_CLUB:
        // TODO: Is this the right TV show?
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubOpinions.words[gSpecialVar_0x8006];
        displayedPersonType = 1;
        break;
    case EASY_CHAT_TYPE_DUMMY_SHOW:
        // TODO: Is this the right TV show?
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubOpinions.species;
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
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].bravoTrainer.words[gSpecialVar_0x8006];
        displayedPersonType = 0;
        break;
    case 12:
        // TODO: Is this the right TV show?
        words = gSaveBlock1.tvShows[gSpecialVar_0x8005].bravoTrainerTower.words;
        displayedPersonType = 1;
        break;
    case EASY_CHAT_TYPE_GOOD_SAYING:
        gEasyChatScreen->temporaryPhrase[0] = 0xFFFF;
        gEasyChatScreen->temporaryPhrase[1] = -1;
        words = gEasyChatScreen->temporaryPhrase;
        break;
    default:
        return;
    }
    DoEasyChatScreen(gSpecialVar_0x8004, words, CB2_ReturnToFieldContinueScript, displayedPersonType);
}

void DoEasyChatScreen(u8 type, u16 *words, void (*exitCallback)(void), u8 displayedPersonType)
{
    gEasyChatScreen->exitCallback = exitCallback;
    gEasyChatScreen->savedPhrase = words;
    gEasyChatScreen->type = type;
    gEasyChatScreen->displayedPersonType = displayedPersonType;
    if (type == EASY_CHAT_TYPE_TRENDY_PHRASE)
    {
        gEasyChatScreen->savedPhrase = gEasyChatScreen->temporaryPhrase;
        gEasyChatScreen->temporaryPhrase[0] = gSaveBlock1.dewfordTrends[0].words[0];
        gEasyChatScreen->temporaryPhrase[1] = gSaveBlock1.dewfordTrends[0].words[1];
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
        Text_LoadWindowTemplate(&gEasyChatMessageWindowTemplate);
        break;
    case 2:
        InitMenuWindow(&gEasyChatInputSelectWindowTemplate);
        InitMenuWindow(&gEasyChatMessageWindowTemplate);
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
    gEasyChatScreen->templateId = sEasyChatLayoutByType[gEasyChatScreen->type];
    switch (gEasyChatScreen->templateId)
    {
    case 4:
        gEasyChatScreen->maxWords = 4;
        gEasyChatScreen->numColumns = 2;
        gEasyChatScreen->numRows = 2;
        gEasyChatScreen->phraseFrameLeft = 5;
        gEasyChatScreen->phraseFrameTop = 4;
        break;
    case 5:
        gEasyChatScreen->maxWords = 4;
        gEasyChatScreen->numColumns = 1;
        gEasyChatScreen->numRows = 4;
        gEasyChatScreen->phraseFrameLeft = 16;
        gEasyChatScreen->phraseFrameTop = 2;
        break;
    case 0:
        gEasyChatScreen->maxWords = 6;
        gEasyChatScreen->numColumns = 2;
        gEasyChatScreen->numRows = 3;
        gEasyChatScreen->phraseFrameLeft = 4;
        gEasyChatScreen->phraseFrameTop = 3;
        break;
    case 1:
        gEasyChatScreen->maxWords = 9;
        gEasyChatScreen->numColumns = 2;
        gEasyChatScreen->numRows = 5;
        gEasyChatScreen->phraseFrameLeft = 4;
        gEasyChatScreen->phraseFrameTop = 0;
        break;
    case 2:
        gEasyChatScreen->maxWords = 1;
        gEasyChatScreen->numColumns = 1;
        gEasyChatScreen->numRows = 1;
        gEasyChatScreen->phraseFrameLeft = 16;
        gEasyChatScreen->phraseFrameTop = 4;
        break;
    case 3:
        gEasyChatScreen->maxWords = 2;
        gEasyChatScreen->numColumns = 2;
        gEasyChatScreen->numRows = 1;
        gEasyChatScreen->phraseFrameLeft = 5;
        gEasyChatScreen->phraseFrameTop = 3;
        break;
    }
    gEasyChatScreen->mainCursorRow = 0;
    gEasyChatScreen->mainCursorColumn = 0;
    gEasyChatScreen->mainCursorMoved = 0;
    gEasyChatScreen->inAlphabetMode = 0;
    gEasyChatScreen->lowerWindowScrollOffset = 0;
    gEasyChatScreen->lowerWindowScrollSpeed = 2;
    SetUnlockedEasyChatGroups();
    SetUnlockedWordsByAlphabet();
    LoadEasyChatStrings();
    ResetCurrentPhraseToSaved();
    InitAlphabetKeyboardRows();
    InitEasyChatPromptText();
}

void SetUnlockedEasyChatGroups(void)
{
    u16 groupId = 0;
    u16 row;
    u16 column;

    for (row = 0; ; row++)
    {
        for (column = 0; column < 2; column++)
        {
            gEasyChatScreen->unlockedGroupIds[row][column] = groupId++;
            if (groupId == EC_GROUP_EVENTS)
                break;
        }
        if (groupId == EC_GROUP_EVENTS)
            break;
    }
    gEasyChatScreen->numUnlockedGroups = EC_GROUP_EVENTS;
    while (groupId < EC_NUM_GROUPS)
    {
        if (IsEasyChatGroupUnlocked(groupId) != 0)
        {
            column++;
            if (column > 1)
            {
                row++;
                column = 0;
            }
            gEasyChatScreen->unlockedGroupIds[row][column] = groupId;
            gEasyChatScreen->optionalGroupUnlocked[groupId - EC_GROUP_EVENTS] = 1;
            gEasyChatScreen->numUnlockedGroups++;
        }
        else
        {
            gEasyChatScreen->optionalGroupUnlocked[groupId - EC_GROUP_EVENTS] = 0;
        }
        groupId++;
    }
    gEasyChatScreen->keyboardLastRow = (gEasyChatScreen->numUnlockedGroups + 1) / 2;
}

void InitAlphabetKeyboardRows(void)
{
    u8 i;
    u8 column;

    for (i = 0; i < 4; i++)
    {
        const u8 *row = sAlphabetKeyboardRows[i];

        for (column = 0; row[column] != EOS; column++)
        {
            if (row[column] != CHAR_SPACE)
                gEasyChatScreen->alphabetGroupIds[i][column] = row[column] + 0x46;
            else
                gEasyChatScreen->alphabetGroupIds[i][column] = CHAR_SPACE;
        }
    }
}

void InitEasyChatPromptText(void)
{
    u8 *pointers[] =
    {
        gEasyChatScreen->instructionsText1, gEasyChatScreen->instructionsText2,
        gEasyChatScreen->confirmText1, gEasyChatScreen->confirmText2,
    };
    u8 *text;
    u16 i;

    for (i = 0; i < 2; i++)
    {
        const struct EasyChatPrompt *prompt = &sEasyChatPrompts[sEasyChatPromptPairsByType[gEasyChatScreen->type][i]];

        text = StringCopy(pointers[i * 2 + 0], prompt->text1);
        if (prompt->separateLines)
        {
            StringCopy(pointers[i * 2 + 1], prompt->text2);
        }
        else
        {
            *text++ = CHAR_SPACE;
            StringCopy(text, prompt->text2);
            *pointers[i * 2 + 1] = EOS;
        }
    }

    for (i = 0; i < 0x24; i++)
        gEasyChatScreen->unusedTextBuffer[i] = 0;
    gEasyChatScreen->unusedTextBuffer[i] = 0xFF;

    text = gEasyChatScreen->unusedTextControlBuffer;
    text[0] = EXT_CTRL_CODE_BEGIN;
    text[1] = 0x11;
    text[2] = 0xE0;
    text[3] = 0xFF;
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
        gSaveBlock1.easyChats.easyChatProfile[i] = sDefaultProfileWords[i];

    for (i = 0; i < 6; i++)
        gSaveBlock1.easyChats.easyChatBattleStart[i] = sDefaultBattleStartWords[i];

    for (i = 0; i < 6; i++)
    {
        gSaveBlock1.easyChats.easyChatBattleWon[i] = 0xFFFF;
        gSaveBlock1.easyChats.easyChatBattleLost[i] = 0xFFFF;
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
    gEasyChatScreen->callback = func;
    gEasyChatScreen->funcState = 0;
}

void InitKeyboardSelection(void)
{
    u16 i;

    if (gEasyChatScreen->inAlphabetMode == 0)
    {
        for (i = 0; i < gEasyChatScreen->keyboardLastRow; i++)
            gEasyChatScreen->keyboardLastColumns[i] = 2;
        gEasyChatScreen->keyboardLastColumns[i - 1] = gEasyChatScreen->numUnlockedGroups % 2;
        if (gEasyChatScreen->keyboardLastColumns[i - 1] == 0)
            gEasyChatScreen->keyboardLastColumns[i - 1] = 2;
    }
    else
    {
        gEasyChatScreen->keyboardLastColumns[0] = 7;
        gEasyChatScreen->keyboardLastColumns[1] = 6;
        gEasyChatScreen->keyboardLastColumns[2] = 7;
        gEasyChatScreen->keyboardLastColumns[3] = 7;
    }
    gEasyChatScreen->keyboardRow = 0;
    gEasyChatScreen->keyboardColumn = 0;
    gEasyChatScreen->keyboardScrollOffset = 0;
    gEasyChatScreen->inButtonWindow = 0;
    ResetLowerWindowScroll();
}

void InitWordSelectSelection(void)
{
    SetSelectedWordGroup();
    if (gEasyChatScreen->inAlphabetMode == 0)
    {
        u16 i;
        u8 groupId;

        groupId = gEasyChatScreen->selectedGroupId;
        gEasyChatScreen->wordSelectLastRow = (gEasyChatScreen->numUnlockedWordsByGroup[groupId] + 1) / 2;
        for (i = 0; i < gEasyChatScreen->wordSelectLastRow; i++)
            gEasyChatScreen->wordSelectLastColumns[i] = 2;
        i--;
        gEasyChatScreen->wordSelectLastColumns[i] = gEasyChatScreen->numUnlockedWordsByGroup[groupId] % 2;
        if (gEasyChatScreen->wordSelectLastColumns[i] == 0)
            gEasyChatScreen->wordSelectLastColumns[i] = 2;
    }
    else
    {
        u16 i;
        u8 groupId;

        groupId = gEasyChatScreen->selectedGroupId;
        gEasyChatScreen->wordSelectLastRow = (gEasyChatScreen->numUnlockedWordsByAlphabet[groupId] + 1) / 2;
        for (i = 0; i < gEasyChatScreen->wordSelectLastRow; i++)
            gEasyChatScreen->wordSelectLastColumns[i] = 2;
        i--;
        gEasyChatScreen->wordSelectLastColumns[i] = gEasyChatScreen->numUnlockedWordsByAlphabet[groupId] % 2;
        if (gEasyChatScreen->wordSelectLastColumns[i] == 0)
            gEasyChatScreen->wordSelectLastColumns[i] = 2;
    }
    gEasyChatScreen->wordSelectRow = 0;
    gEasyChatScreen->wordSelectColumn = 0;
    gEasyChatScreen->wordSelectScrollOffset = 0;
    ResetLowerWindowScroll();
}

void ShowEasyChatTitleAndPortrait(void)
{
    switch (gEasyChatScreen->type)
    {
    case EASY_CHAT_TYPE_PROFILE:
    case EASY_CHAT_TYPE_BATTLE_START:
    case EASY_CHAT_TYPE_BATTLE_WON:
    case EASY_CHAT_TYPE_BATTLE_LOST:
    case EASY_CHAT_TYPE_MAIL:
    case EASY_CHAT_TYPE_BARD_SONG:
    case EASY_CHAT_TYPE_TRENDY_PHRASE:
    case EASY_CHAT_TYPE_GOOD_SAYING:
    default:
        PrintTitle(gEasyChatScreen->type);
        break;
    case EASY_CHAT_TYPE_INTERVIEW:
    case EASY_CHAT_TYPE_FAN_CLUB:
    case EASY_CHAT_TYPE_DUMMY_SHOW:
    case EASY_CHAT_TYPE_GABBY_AND_TY:
    case EASY_CHAT_TYPE_CONTEST_INTERVIEW:
    case EASY_CHAT_TYPE_BATTLE_TOWER_INTERVIEW:
        PrintTitle(gEasyChatScreen->type);
        CreateInterviewObjectEvents(gEasyChatScreen->displayedPersonType, gEasyChatScreen->templateId);
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
    gEasyChatScreen->callback();
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
    gEasyChatScreen->mainCursorMoved = MoveMainCursor();
    if (gEasyChatScreen->mainCursorMoved)
        PlaySE(SE_SELECT);
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gEasyChatScreen->mainCursorRow == gEasyChatScreen->numRows)
        {
            switch (gEasyChatScreen->mainCursorColumn)
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
            gEasyChatScreen->selectedPhraseWord = gEasyChatScreen->mainCursorRow * gEasyChatScreen->numColumns + gEasyChatScreen->mainCursorColumn;
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
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        SetMainCursorState(2);
        if (gEasyChatScreen->type == EASY_CHAT_TYPE_BARD_SONG)
        {
            PrintEasyChatStdMessage(6);
            gEasyChatScreen->funcState = 100;
        }
        else
        {
            PrintEasyChatStdMessage(2);
            DisplayYesNoMenu(23, 8, 1);
            Menu_MoveCursor(1);
            gEasyChatScreen->funcState++;
        }
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            ResetCurrentPhrase();
            ClearPhraseWindow();
            PrintCurrentPhrase();
            gEasyChatScreen->funcState++;
            break;
        case -1:
        case 1:
            gEasyChatScreen->funcState++;
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
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        SetMainCursorState(2);
        PrintEasyChatStdMessage(3);
        DisplayYesNoMenu(23, 8, 0);
        Menu_MoveCursor(1);
        if (gEasyChatScreen->type == EASY_CHAT_TYPE_TRENDY_PHRASE
         || gEasyChatScreen->type == EASY_CHAT_TYPE_MAIL
         || gEasyChatScreen->type == EASY_CHAT_TYPE_FAN_CLUB
         || gEasyChatScreen->type == EASY_CHAT_TYPE_DUMMY_SHOW
         || gEasyChatScreen->type == EASY_CHAT_TYPE_GABBY_AND_TY
         || gEasyChatScreen->type == EASY_CHAT_TYPE_CONTEST_INTERVIEW
         || gEasyChatScreen->type == EASY_CHAT_TYPE_BATTLE_TOWER_INTERVIEW
         || gEasyChatScreen->type == EASY_CHAT_TYPE_INTERVIEW
         || gEasyChatScreen->type == EASY_CHAT_TYPE_GOOD_SAYING)
            gEasyChatScreen->funcState = 2;
        else
            gEasyChatScreen->funcState++;
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            PrintEasyChatStdMessage(4);
            DisplayYesNoMenu(23, 8, 0);
            Menu_MoveCursor(1);
            gEasyChatScreen->funcState++;
            break;
        case -1:
        case 1:
            gEasyChatScreen->funcState = 0xFF;
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
            gEasyChatScreen->funcState = 0xFF;
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
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        SetMainCursorState(2);
        if (IsCurrentPhraseEmpty())
        {
            PrintEasyChatStdMessage(5);
            gEasyChatScreen->funcState = 10;
            break;
        }
        if (gEasyChatScreen->type == EASY_CHAT_TYPE_TRENDY_PHRASE)
        {
            if (DidPhraseChange() == 0)
            {
                PrintEasyChatStdMessage(8);
                gEasyChatScreen->funcState = 10;
                break;
            }
            if (gEasyChatScreen->currentPhrase[0] == 0xFFFF || gEasyChatScreen->currentPhrase[1] == 0xFFFF)
            {
                PrintEasyChatStdMessage(9);
                gEasyChatScreen->funcState = 10;
                break;
            }
        }
        if (gEasyChatScreen->type == EASY_CHAT_TYPE_MAIL && DidPhraseChange() == 0)
        {
            SetEasyChatScreenCallback(HandleEasyChatExitPrompt);
        }
        else
        {
            PrintEasyChatStdMessage(1);
            PrintCurrentPhraseAsText();
            DisplayYesNoMenu(23, 8, 0);
            Menu_MoveCursor(0);
            gEasyChatScreen->funcState++;
        }
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            gSpecialVar_Result = (DidPhraseChange() != 0);
            SaveCurrentPhrase();
            if (gEasyChatScreen->type == EASY_CHAT_TYPE_PROFILE)
                gSpecialVar_0x8004 = DidPlayerInputMysteryEventPhrase();
            if (gEasyChatScreen->type == EASY_CHAT_TYPE_TRENDY_PHRASE)
            {
                BufferCurrentPhraseToStringVar2();
                gSpecialVar_0x8004 = TrySetTrendyPhrase(gEasyChatScreen->temporaryPhrase);
            }
            if (gEasyChatScreen->type == EASY_CHAT_TYPE_GOOD_SAYING)
            {
                if (gEasyChatScreen->currentPhrase[0] == 0xFFFF || gEasyChatScreen->currentPhrase[1] == 0xFFFF)
                    gSpecialVar_Result = 0;
                gSpecialVar_0x8004 = DidPlayerInputABerryMasterWifePhrase();
            }
            SetEasyChatScreenCallback(ExitEasyChatScreen);
            break;
        case -1:
        case 1:
            Menu_DestroyCursor();
            CloseEasyChatPrompt();
            if (gEasyChatScreen->type == EASY_CHAT_TYPE_BARD_SONG && DidPhraseChange() != 0)
            {
                gEasyChatScreen->funcState = 100;
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
        gEasyChatScreen->funcState++;
        // fall through
    case 101:
        if (JOY_NEW(A_BUTTON))
            gEasyChatScreen->funcState++;
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
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        SetMainCursorState(1);
        PrintEasyChatStdMessage(10);
        InitKeyboardSelection();
        PrintKeyboardText();
        ResetLowerWindowAnimState();
        gEasyChatScreen->funcState++;
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
    gEasyChatScreen->keyboardCursorMoved = MoveKeyboardCursor();
    if (gEasyChatScreen->lowerWindowScrollChange != 0)
    {
        PlaySE(SE_SELECT);
        gEasyChatScreen->postScrollCallback = HandleEasyChatKeyboardInput;
        SetEasyChatScreenCallback(ScrollEasyChatList);
    }
    else
    {
        if (gEasyChatScreen->keyboardCursorMoved)
            PlaySE(SE_SELECT);
        if (JOY_NEW(A_BUTTON))
        {
            if (gEasyChatScreen->inButtonWindow != 0)
            {
                PlaySE(SE_SELECT);
                switch (gEasyChatScreen->keyboardRow)
                {
                case 1:
                    SetEasyChatScreenCallback(SwitchKeyboardMode);
                    break;
                case 2:
                    if (gEasyChatScreen->type != EASY_CHAT_TYPE_BARD_SONG)
                    {
                        SetCurrentPhraseWord(gEasyChatScreen->selectedPhraseWord, 0xFFFF);
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
                if (gEasyChatScreen->inAlphabetMode == 0
                 || gEasyChatScreen->numUnlockedWordsByAlphabet[gEasyChatScreen->alphabetGroupIds[gEasyChatScreen->keyboardRow][gEasyChatScreen->keyboardColumn]] != 0)
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
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        DestroyRectangleCursorSprites();
        ResetLowerWindowAnimState();
        HideScrollIndicatorSprites();
        SetModeWindowAnimForVisibility(0);
        gEasyChatScreen->funcState++;
        break;
    case 1:
    case 2:
        gEasyChatScreen->funcState++;
        break;
    case 3:
        if (CloseKeyboard() != 0)
            gEasyChatScreen->funcState++;
        break;
    case 4:
        SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    }
}

void SwitchKeyboardMode(void)
{
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        DestroyRectangleCursorSprites();
        ResetLowerWindowAnimState();
        HideScrollIndicatorSprites();
        gEasyChatScreen->funcState++;
        UpdateModeWindowAnim();
        break;
    case 1:
        if (StartSwitchKeyboardMode() != 0)
        {
            gEasyChatScreen->inAlphabetMode = !gEasyChatScreen->inAlphabetMode;
            InitKeyboardSelection();
            PrintKeyboardText();
            ResetLowerWindowAnimState();
            gEasyChatScreen->funcState++;
        }
        break;
    default:
        gEasyChatScreen->funcState++;
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
    switch (gEasyChatScreen->funcState)
    {
    default:
        gEasyChatScreen->funcState++;
        break;
    case 8:
        SetModeWindowAnimForVisibility(0);
        DestroyRectangleCursorSprites();
        ClearLowerWindow();
        InitWordSelectSelection();
        HideScrollIndicatorSprites();
        ResetLowerWindowAnimState();
        gEasyChatScreen->funcState++;
        break;
    case 9:
        if (OpenWordSelect() != 0)
        {
            PrintInitialWordSelectText();
            gEasyChatScreen->funcState++;
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
    gEasyChatScreen->wordSelectCursorMoved = MoveWordSelectCursor();
    if (gEasyChatScreen->lowerWindowScrollChange != 0)
    {
        PlaySE(SE_SELECT);
        gEasyChatScreen->postScrollCallback = HandleEasyChatWordSelectInput;
        SetEasyChatScreenCallback(ScrollEasyChatList);
    }
    else
    {
        if (gEasyChatScreen->wordSelectCursorMoved)
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
    switch (gEasyChatScreen->funcState)
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
            gEasyChatScreen->funcState++;
        }
        break;
    case 1:
        gEasyChatScreen->funcState++;
        break;
    case 2:
        ResetLowerWindowAnimState();
        gEasyChatScreen->funcState++;
        break;
    case 3:
        if (CloseWordSelect() != 0)
            gEasyChatScreen->funcState++;
        break;
    case 4:
        if (gEasyChatScreen->type == EASY_CHAT_TYPE_BARD_SONG && DidPhraseChange() != 0)
            SetEasyChatScreenCallback(HandleEasyChatConfirmWordsPrompt);
        else
            SetEasyChatScreenCallback(InitEasyChatMainScreen);
        break;
    }
}

void HandleEasyChatReturnToKeyboard(void)
{
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        SetWordSelectCursorVisibility(0);
        HideScrollIndicatorSprites();
        gEasyChatScreen->funcState++;
        break;
    case 1:
        ClearLowerWindow();
        ResetLowerWindowAnimState();
        gEasyChatScreen->funcState++;
        break;
    case 2:
        if (ReturnToKeyboard() != 0)
        {
            SetModeWindowAnimForVisibility(1);
            RestoreKeyboardScrollOffset();
            gEasyChatScreen->funcState++;
        }
        break;
    case 3:
        CreateRectangleCursorSprites();
        SetScrollIndicatorMode(0);
        gEasyChatScreen->funcState++;
        break;
    case 4:
        PrintKeyboardText();
        SetEasyChatScreenCallback(HandleEasyChatKeyboardInput);
        break;
    }
}

void ScrollEasyChatList(void)
{
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        if (gEasyChatScreen->postScrollCallback == HandleEasyChatKeyboardInput)
            PrintKeyboardRowsDuringScroll();
        else
            PrintWordSelectRowsDuringScroll();
        InitLowerWindowScroll(gEasyChatScreen->lowerWindowScrollSpeed);
        gEasyChatScreen->funcState++;
        break;
    case 1:
        if (UpdateLowerWindowScroll())
        {
            if (gEasyChatScreen->postScrollCallback == HandleEasyChatKeyboardInput)
            {
                PrintKeyboardRowsDuringScroll();
                gEasyChatScreen->keyboardScrollOffset += gEasyChatScreen->lowerWindowScrollChange;
                ReduceToValidKeyboardColumn();
                gEasyChatScreen->keyboardCursorMoved = TRUE;
            }
            else
            {
                gEasyChatScreen->wordSelectScrollOffset += gEasyChatScreen->lowerWindowScrollChange;
                ReduceToValidWordSelectColumn();
                gEasyChatScreen->wordSelectCursorMoved = 1;
            }
            gEasyChatScreen->lowerWindowScrollSpeed = 2;
            SetEasyChatScreenCallback(gEasyChatScreen->postScrollCallback);
        }
        break;
    }
}

void ExitEasyChatScreen(void)
{
    switch (gEasyChatScreen->funcState)
    {
    case 0:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gEasyChatScreen->funcState++;
        break;
    case 1:
        if (!UpdatePaletteFade())
            SetMainCallback2(gEasyChatScreen->exitCallback);
        break;
    }
}

void ClearUnusedField(void)
{
    if (gEasyChatScreen->type == EASY_CHAT_TYPE_BATTLE_START
     && gEasyChatScreen->longWordCountsByRow[gEasyChatScreen->mainCursorRow] == 2
     && GetEasyChatWordStringLength(gEasyChatScreen->currentPhrase[gEasyChatScreen->selectedPhraseWord]) != 7)
        gEasyChatScreen->unusedLongWordLimit = 1;
    else
        gEasyChatScreen->unusedLongWordLimit = 0;
    gEasyChatScreen->unusedLongWordLimit = 0;  // Reset immediately by the original flow.
}

bool8 MoveMainCursor(void)
{
    bool8 pressedUpDown = FALSE;
    u8 phraseWordIndex;

    if (JOY_NEW(START_BUTTON))
    {
        gEasyChatScreen->mainCursorRow = gEasyChatScreen->numRows;
        gEasyChatScreen->mainCursorColumn = 2;
        return TRUE;
    }

    if (JOY_REPT(DPAD_UP))
    {
        gEasyChatScreen->mainCursorRow--;
        if (gEasyChatScreen->mainCursorRow < 0)
            gEasyChatScreen->mainCursorRow = gEasyChatScreen->numRows;
        pressedUpDown = TRUE;
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        gEasyChatScreen->mainCursorRow++;
        if (gEasyChatScreen->mainCursorRow > gEasyChatScreen->numRows)
            gEasyChatScreen->mainCursorRow = 0;
        pressedUpDown = TRUE;
    }

    if (pressedUpDown)
    {
        if (gEasyChatScreen->templateId == 2)
        {
            if (gEasyChatScreen->mainCursorRow == gEasyChatScreen->numRows)
                gEasyChatScreen->mainCursorColumn = 2;
            else
                gEasyChatScreen->mainCursorColumn = 0;
            return TRUE;
        }
        else
        {
            if (gEasyChatScreen->mainCursorColumn >= gEasyChatScreen->numColumns)
                gEasyChatScreen->mainCursorColumn = gEasyChatScreen->numColumns - 1;
            if (gEasyChatScreen->mainCursorRow != gEasyChatScreen->numRows)
            {
                phraseWordIndex = gEasyChatScreen->mainCursorRow * gEasyChatScreen->numColumns + gEasyChatScreen->mainCursorColumn;
                if (phraseWordIndex >= gEasyChatScreen->maxWords)
                    gEasyChatScreen->mainCursorColumn = phraseWordIndex - gEasyChatScreen->maxWords;
            }
            return TRUE;
        }
    }
    else
    {
        if (JOY_REPT(DPAD_LEFT))
        {
            if (--gEasyChatScreen->mainCursorColumn < 0)
            {
                if (gEasyChatScreen->mainCursorRow == gEasyChatScreen->numRows)
                {
                    gEasyChatScreen->mainCursorColumn = 2;
                }
                else
                {
                    gEasyChatScreen->mainCursorColumn = gEasyChatScreen->numColumns - 1;
                    phraseWordIndex = gEasyChatScreen->mainCursorRow * gEasyChatScreen->numColumns + gEasyChatScreen->mainCursorColumn;
                    if (phraseWordIndex >= gEasyChatScreen->maxWords)
                        gEasyChatScreen->mainCursorColumn = phraseWordIndex - gEasyChatScreen->maxWords;
                }
            }
            return TRUE;
        }
        if (JOY_REPT(DPAD_RIGHT))
        {
            if (gEasyChatScreen->mainCursorRow == gEasyChatScreen->numRows)
            {
                if (++gEasyChatScreen->mainCursorColumn > 2)
                    gEasyChatScreen->mainCursorColumn = 0;
            }
            else
            {
                if (++gEasyChatScreen->mainCursorColumn >= gEasyChatScreen->numColumns)
                    gEasyChatScreen->mainCursorColumn = 0;
                phraseWordIndex = gEasyChatScreen->mainCursorRow * gEasyChatScreen->numColumns + gEasyChatScreen->mainCursorColumn;
                if (phraseWordIndex >= gEasyChatScreen->maxWords)
                    gEasyChatScreen->mainCursorColumn = phraseWordIndex - gEasyChatScreen->maxWords;
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

    if (gEasyChatScreen->inButtonWindow != 0)
    {
        if (JOY_REPT(DPAD_UP))
        {
            gEasyChatScreen->keyboardRow--;
            if (gEasyChatScreen->keyboardRow < 1)
                gEasyChatScreen->keyboardRow = 3;
            return TRUE;
        }
        else if (JOY_REPT(DPAD_DOWN))
        {
            gEasyChatScreen->keyboardRow++;
            if (gEasyChatScreen->keyboardRow > 3)
                gEasyChatScreen->keyboardRow = 1;
            return TRUE;
        }
    }
    else
    {
        if (gEasyChatScreen->inAlphabetMode == 1)
        {
            pressedUpDown = FALSE;

            if (JOY_REPT(DPAD_UP))
            {
                gEasyChatScreen->keyboardRow--;
                if (gEasyChatScreen->keyboardRow < 0)
                    gEasyChatScreen->keyboardRow = 3;
                pressedUpDown = TRUE;
            }
            else if (JOY_REPT(DPAD_DOWN))
            {
                gEasyChatScreen->keyboardRow++;
                if (gEasyChatScreen->keyboardRow > 3)
                    gEasyChatScreen->keyboardRow = 0;
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
            gEasyChatScreen->lowerWindowScrollChange = 0;

            if (JOY_REPT(DPAD_UP))
            {
                if (gEasyChatScreen->keyboardRow == 0)
                    return FALSE;
                gEasyChatScreen->keyboardRow--;
                if (gEasyChatScreen->keyboardRow < gEasyChatScreen->keyboardScrollOffset)
                    gEasyChatScreen->lowerWindowScrollChange = -1;
                pressedUpDown = TRUE;
            }
            else if (JOY_REPT(DPAD_DOWN))
            {
                if (gEasyChatScreen->keyboardRow >= gEasyChatScreen->keyboardLastRow - 1)
                    return FALSE;
                gEasyChatScreen->keyboardRow++;
                if (gEasyChatScreen->keyboardRow > gEasyChatScreen->keyboardScrollOffset + 3)
                    gEasyChatScreen->lowerWindowScrollChange = 1;
                pressedUpDown = TRUE;
            }

            if (pressedUpDown)
            {
                if (gEasyChatScreen->lowerWindowScrollChange == 0)
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
        if (gEasyChatScreen->keyboardColumn != 0)
            gEasyChatScreen->keyboardColumn--;
        else
            gEasyChatScreen->keyboardColumn = gEasyChatScreen->keyboardLastColumns[gEasyChatScreen->keyboardRow];
        pressedLeftRight = TRUE;
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        if (gEasyChatScreen->inButtonWindow != 0
         || gEasyChatScreen->keyboardColumn == gEasyChatScreen->keyboardLastColumns[gEasyChatScreen->keyboardRow])
            gEasyChatScreen->keyboardColumn = 0;
        else
            gEasyChatScreen->keyboardColumn++;
        pressedLeftRight = TRUE;
    }

    if (pressedLeftRight)
    {
        s8 wasInButtonWindow = gEasyChatScreen->inButtonWindow;

        gEasyChatScreen->inButtonWindow = (gEasyChatScreen->keyboardColumn == gEasyChatScreen->keyboardLastColumns[gEasyChatScreen->keyboardRow]);
        if (gEasyChatScreen->inButtonWindow != 0)
        {
            gEasyChatScreen->keyboardRow -= gEasyChatScreen->keyboardScrollOffset;
            if (gEasyChatScreen->keyboardRow == 0)
            {
                gEasyChatScreen->keyboardRow = 1;
                gEasyChatScreen->keyboardColumn = gEasyChatScreen->keyboardLastColumns[gEasyChatScreen->keyboardRow];
            }
        }
        else if (wasInButtonWindow != 0)
        {
            gEasyChatScreen->keyboardRow += gEasyChatScreen->keyboardScrollOffset;
            if (gEasyChatScreen->keyboardColumn != 0)
                gEasyChatScreen->keyboardColumn = gEasyChatScreen->keyboardLastColumns[gEasyChatScreen->keyboardRow] - 1;
        }
        return TRUE;
    }

    return FALSE;
}

void ReduceToValidKeyboardColumn(void)
{
    if (gEasyChatScreen->keyboardColumn >= gEasyChatScreen->keyboardLastColumns[gEasyChatScreen->keyboardRow])
        gEasyChatScreen->keyboardColumn = gEasyChatScreen->keyboardLastColumns[gEasyChatScreen->keyboardRow] - 1;
}

void SelectWordGroupFromKeyboardCursor(void)
{
    if (gEasyChatScreen->inAlphabetMode == 0)
        gEasyChatScreen->selectedGroupId = gEasyChatScreen->unlockedGroupIds[gEasyChatScreen->keyboardRow][gEasyChatScreen->keyboardColumn];
    else
        gEasyChatScreen->selectedGroupId = gEasyChatScreen->alphabetGroupIds[gEasyChatScreen->keyboardRow][gEasyChatScreen->keyboardColumn];
}

bool8 MoveWordSelectCursor(void)
{
    bool8 pressedUpDown = FALSE;

    gEasyChatScreen->lowerWindowScrollChange = 0;
    if (JOY_REPT(DPAD_UP))
    {
        if (gEasyChatScreen->wordSelectRow == 0)
            return FALSE;
        gEasyChatScreen->wordSelectRow--;
        if (gEasyChatScreen->wordSelectRow < gEasyChatScreen->wordSelectScrollOffset)
        {
            gEasyChatScreen->lowerWindowScrollChange = -1;
            return FALSE;
        }
        pressedUpDown = TRUE;
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (gEasyChatScreen->wordSelectRow >= gEasyChatScreen->wordSelectLastRow - 1)
            return FALSE;
        gEasyChatScreen->wordSelectRow++;
        if (gEasyChatScreen->wordSelectRow >= gEasyChatScreen->wordSelectScrollOffset + 4)
        {
            gEasyChatScreen->lowerWindowScrollChange = 1;
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
        gEasyChatScreen->wordSelectColumn--;
        if (gEasyChatScreen->wordSelectColumn < 0)
            gEasyChatScreen->wordSelectColumn = gEasyChatScreen->wordSelectLastColumns[gEasyChatScreen->wordSelectRow] - 1;
        return TRUE;
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        gEasyChatScreen->wordSelectColumn++;
        if (gEasyChatScreen->wordSelectColumn >= gEasyChatScreen->wordSelectLastColumns[gEasyChatScreen->wordSelectRow])
            gEasyChatScreen->wordSelectColumn = 0;
        return TRUE;
    }

    if (JOY_NEW(START_BUTTON))
    {
        if (gEasyChatScreen->wordSelectScrollOffset != 0)
        {
            gEasyChatScreen->lowerWindowScrollChange = -gEasyChatScreen->wordSelectScrollOffset;
            if (gEasyChatScreen->lowerWindowScrollChange < -4)
                gEasyChatScreen->lowerWindowScrollChange = -4;
        }
        gEasyChatScreen->wordSelectRow += gEasyChatScreen->lowerWindowScrollChange;
        gEasyChatScreen->lowerWindowScrollSpeed = 4;
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        if (gEasyChatScreen->wordSelectScrollOffset < gEasyChatScreen->wordSelectLastRow - 4)
        {
            gEasyChatScreen->lowerWindowScrollChange = gEasyChatScreen->wordSelectLastRow - 4 - gEasyChatScreen->wordSelectScrollOffset;
            if (gEasyChatScreen->lowerWindowScrollChange > 4)
                gEasyChatScreen->lowerWindowScrollChange = 4;
        }
        gEasyChatScreen->wordSelectRow += gEasyChatScreen->lowerWindowScrollChange;
        gEasyChatScreen->lowerWindowScrollSpeed = 4;
    }

    return FALSE;
}

void ReduceToValidWordSelectColumn(void)
{
    if (gEasyChatScreen->wordSelectColumn >= gEasyChatScreen->wordSelectLastColumns[gEasyChatScreen->wordSelectRow])
        gEasyChatScreen->wordSelectColumn = gEasyChatScreen->wordSelectLastColumns[gEasyChatScreen->wordSelectRow] - 1;
}

void ResetCurrentPhrase(void)
{
    u16 i;

    for (i = 0; i < gEasyChatScreen->maxWords; i++)
        SetCurrentPhraseWord(i, 0xFFFF);
}

void SaveCurrentPhrase(void)
{
    u16 i;

    for (i = 0; i < gEasyChatScreen->maxWords; i++)
        gEasyChatScreen->savedPhrase[i] = gEasyChatScreen->currentPhrase[i];
}

bool8 TrySetSelectedWord(void)
{
    u16 selectedWord = gEasyChatScreen->selectedGroupWords[gEasyChatScreen->wordSelectRow][gEasyChatScreen->wordSelectColumn];

    if (gEasyChatScreen->unusedLongWordLimit != 0
     && gEasyChatScreen->longWordCountsByRow[gEasyChatScreen->mainCursorRow] > 1
     && GetEasyChatWordStringLength(selectedWord) == 7)
        return FALSE;

    SetCurrentPhraseWord(gEasyChatScreen->selectedPhraseWord, selectedWord);
    PrintCurrentPhrase();
    return TRUE;
}

void ResetCurrentPhraseToSaved(void)
{
    u16 wordIndex = 0;
    u16 i;
    u16 j;

    for (i = 0; i < gEasyChatScreen->numRows; i++)
    {
        gEasyChatScreen->longWordCountsByRow[i] = 0;
        for (j = 0; j < gEasyChatScreen->numColumns; j++)
        {
            gEasyChatScreen->currentPhrase[wordIndex] = gEasyChatScreen->savedPhrase[wordIndex];
            gEasyChatScreen->phraseWordSpacing[i][j] = 0;
            wordIndex++;
        }
    }
}

void SetCurrentPhraseWord(u16 phraseWordIndex, u16 wordId)
{
    u16 row = phraseWordIndex / gEasyChatScreen->numColumns;
    u16 column = phraseWordIndex % gEasyChatScreen->numColumns;
    u16 oldWordLength = GetEasyChatWordStringLength(gEasyChatScreen->currentPhrase[phraseWordIndex]);
    u16 newWordLength = GetEasyChatWordStringLength(wordId);

    if (oldWordLength == 7)
    {
        if (newWordLength != 7)
            gEasyChatScreen->longWordCountsByRow[row]--;
    }
    else
    {
        if (newWordLength == 7)
            gEasyChatScreen->longWordCountsByRow[row]++;
    }
    newWordLength = 0;
    gEasyChatScreen->phraseWordSpacing[row][column] = newWordLength;
    gEasyChatScreen->currentPhrase[phraseWordIndex] = wordId;
}

u8 DidPhraseChange(void)
{
    u16 changedWordCount = 0;
    u16 i;
    u8 *savedWordText;
    u8 *currentWordText;

    for (i = 0; i < gEasyChatScreen->maxWords; i++)
    {
        CopyEasyChatWordPadded(gEasyChatScreen->wordPrintBuffer, gEasyChatScreen->savedPhrase[i], 0);
        CopyEasyChatWordPadded(gEasyChatScreen->phraseCompareBuffer, gEasyChatScreen->currentPhrase[i], 0);
        savedWordText = gEasyChatScreen->wordPrintBuffer;
        currentWordText = gEasyChatScreen->phraseCompareBuffer;
        while (*savedWordText == *currentWordText && *savedWordText != 0xFF)
        {
            savedWordText++;
            currentWordText++;
        }
        if (*savedWordText != *currentWordText)
            changedWordCount++;
    }
    return changedWordCount;
}

bool8 IsCurrentPhraseEmpty(void)
{
    u16 i;

    for (i = 0; i < gEasyChatScreen->maxWords; i++)
    {
        if (gEasyChatScreen->currentPhrase[i] != 0xFFFF)
            return FALSE;
    }
    return TRUE;
}

// CheckMysteryEventPhrase
bool8 DidPlayerInputMysteryEventPhrase(void)
{
    u16 i;
    u8 *inputWordText;
    u8 *expectedWordText;

    for (i = 0; i < 4; i++)
    {
        CopyEasyChatWordPadded(gEasyChatScreen->wordPrintBuffer, gEasyChatScreen->currentPhrase[i], 0);
        CopyEasyChatWordPadded(gEasyChatScreen->phraseCompareBuffer, sMysteryEventPhrase[i], 0);
        inputWordText = gEasyChatScreen->wordPrintBuffer;
        expectedWordText = gEasyChatScreen->phraseCompareBuffer;
        while (*inputWordText != 0xFF && *expectedWordText != 0xFF)
        {
            if (*inputWordText++ != *expectedWordText++)
                return FALSE;
        }
        if (*inputWordText != 0xFF || *expectedWordText != 0xFF)
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
        u8 *wifePhraseText;

        ptr = CopyEasyChatWordPadded(gEasyChatScreen->phraseTextBuffer, gEasyChatScreen->currentPhrase[0], 0);
        *ptr++ = CHAR_SPACE;
        CopyEasyChatWordPadded(ptr, gEasyChatScreen->currentPhrase[1], 0);

        ptr = CopyEasyChatWordPadded(gEasyChatScreen->berryMasterWifePhraseBuffer, sBerryMasterWifePhrases[i][0], 0);
        *ptr++ = CHAR_SPACE;
        CopyEasyChatWordPadded(ptr, sBerryMasterWifePhrases[i][1], 0);

        ptr = gEasyChatScreen->phraseTextBuffer;
        wifePhraseText = gEasyChatScreen->berryMasterWifePhraseBuffer;
        while (*ptr != EOS && *wifePhraseText != EOS)
        {
            if (*ptr++ != *wifePhraseText++)
                break;
        }
        if (*ptr == EOS && *wifePhraseText == EOS)
            return i + 1;
    }
    return 0;
}

void BufferCurrentPhraseToStringVar2(void)
{
    u8 *ptr;

    ptr = CopyEasyChatWordPadded(gStringVar2, gEasyChatScreen->temporaryPhrase[0], 0);
    *ptr++ = CHAR_SPACE;
    CopyEasyChatWordPadded(ptr, gEasyChatScreen->temporaryPhrase[1], 0);
}

void CloseEasyChatPrompt(void)
{
    PlaySE(SE_SELECT);
    PrintCurrentPhrase();
    Menu_EraseWindowRect(0, 0, 29, 13);
}
