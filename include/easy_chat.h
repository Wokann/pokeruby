#ifndef GUARD_EASYCHAT_H
#define GUARD_EASYCHAT_H

struct Sprite;

struct EasyChatWindowAnimSource
{
    u16 tilemapX;
    u16 tilemapY;
    u16 tilemapXStep;
    u16 tilemapYStep;
    void *tilemap;
};

// Ruby keeps screen, control, and word-selection state in one fixed shared-memory block.
struct EasyChatScreen
{
    void (*exitCallback)(void);
    u16 *savedPhrase;
    u8 type;
    u8 templateId;
    u8 maxWords;
    u8 displayedPersonType;
    u16 currentPhrase[9];
    void (*callback)(void);
    u16 funcState;
    u8 inAlphabetMode;
    u8 selectedPhraseWord;
    u16 numUnlockedGroups;
    u8 unlockedGroupIds[11][2];
    u8 alphabetGroupIds[4][14];
    u8 optionalGroupUnlocked[5];
    u8 unusedLongWordLimit;
    u8 longWordCountsByRow[5];
    s8 numColumns;
    s8 numRows;
    s8 mainCursorColumn;
    s8 mainCursorRow;
    bool8 mainCursorMoved;
    u16 phraseFrameLeft;
    u16 phraseFrameTop;
    u8 phraseWordSpacing[0x5][2];
    u8 keyboardCursorMoved;
    u8 filler97;
    struct Sprite *mainCursorSprite;
    struct Sprite *rectangleCursorPrimarySprite;
    struct Sprite *rectangleCursorMiddleSprite;
    struct Sprite *rectangleCursorEndSprite;
    struct Sprite *wordSelectCursorSprite;
    struct Sprite *scrollIndicatorSprites[2];
    struct Sprite *pageScrollButtonSprites[2];
    u8 lowerWindowAnimDirty;
    u8 fillerBD;
    u16 lowerWindowAnimState;
    u8 fillerC0[2];
    u16 lowerWindowAnimRectCount;
    u16 lowerWindowAnimFrames;
    u8 fillerC6[2];
    u16 lowerWindowAnimRects[(0x148-0xC8)/16][8];
    struct EasyChatWindowAnimSource lowerWindowAnimSources[(0x1A8-0x148)/12];
    s8 keyboardRow;
    s8 keyboardColumn;
    s8 keyboardLastColumns[0xB5-0xAA];
    s8 keyboardScrollOffset;
    s8 keyboardLastRow;
    s8 inButtonWindow;
    u8 selectedGroupId;
    u8 wordSelectCursorMoved;
    u16 lowerWindowScrollOffset;
    u16 lowerWindowScrollDest;
    u8 lowerWindowScrollSpeed;
    s8 lowerWindowScrollStep;
    s8 lowerWindowScrollChange;
    u8 filler1C1[3];
    void (*postScrollCallback)(void);
    u8 wordGroupTilemapBuffer[0x500];
    u8 inputSelectTilemapBuffer[0xB78-0x6C8];
#if GERMAN
    u16 alphabetizedWords[27][270];
#else
    u16 alphabetizedWords[27][255];
#endif
    u16 numUnlockedWordsByAlphabet[(0x78-0x42)/2];
    u16 numUnlockedWordsByGroup[(0x41A4-0x4178)/2];
    const u8 *wordStrings[0x16][0x100];
    s8 wordSelectRow;
    s8 wordSelectColumn;
    s8 wordSelectLastColumns[130];
    s8 wordSelectLastRow;
    s8 wordSelectScrollOffset;
    u16 selectedGroupWords[(0x9C32-0x9A2A)/4][2];
    u8 framePaletteBlendIndex;
    u8 filler9C33;
    u16 framePaletteBlendColors[3][9];
    u8 filler9C6A[2];
    s8 *scrollOffsetPtr;
    u8 scrollIndicatorsActive;
    s8 scrollOffsetMax;
    u8 filler9C72[2];
    struct Sprite *modeWindowSprite;
    u8 filler9C78[4];
    // Ruby uses this two-word buffer for the Trendy Phrase and Good Saying screens.
    u16 temporaryPhrase[2];
    u8 instructionsText1[0xC9-0x80];
    u8 instructionsText2[0xD12-0xCC9];
    u8 confirmText1[0x5B-0x12];
    u8 confirmText2[0xA4-0x5B];
    u8 unusedTextBuffer[0xC9-0xA4];
    u8 groupNamePrintBuffer[0xE14 - 0xDC9];
    u8 wordPrintBuffer[0xE41 - 0xE14];
    u8 phraseCompareBuffer[0x6E - 0x41];
    u8 phraseTextBuffer[0xEE - 0x6E];
    u8 berryMasterWifePhraseBuffer[0xF6E - 0xEEE];
    u8 unusedTextControlBuffer[0x8E - 0x6E];
    u8 titlePrintBuffer[7];
};

// Kept as the linker-visible name used by Ruby's naked Easy Chat assembly.
extern struct EasyChatScreen *const gEasyChatStruct;
#define gEasyChatScreen gEasyChatStruct

void ShowEasyChatScreen(void);
void DoEasyChatScreen(u8 type, u16 *words, void (*exitCallback)(void), u8 displayedPersonType);
u16 GetRandomEasyChatWordFromGroup(u16 groupId);
void InitializeEasyChatWordArray(u16 *words, u16 length);
void InitEasyChatPhrases(void);
u8 IsEasyChatGroupUnlocked(u8 groupId);
u16 EasyChat_GetNumWordsInGroup(u8);
bool8 IsEasyChatWordInvalid(u16 easyChatWord);
u8 *CopyEasyChatWord(u8 *dest, u16 easyChatWord);
u8 *ConvertEasyChatWordsToString(u8 *dst, u16 *words, u16, u16);
u16 GetRandomEasyChatWordFromUnlockedGroup(u16 group);
void UnlockTrendySaying(u8);
u16 UnlockRandomTrendySaying(void);
u8 *ConvertEasyChatWordsToStringWithScroll(u8 *dest, u16 *words, u16 columns, u16 rows);

#if GERMAN
u32 GetEasyChatWordIndexInGroup(s32 groupId, s32 wordIndex);
#endif

#endif // GUARD_EASYCHAT_H
