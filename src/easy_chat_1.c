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

const u16 gMysteryEventPhrase[] = {EC_WORD_MYSTERY, EC_WORD_EVENT, EC_WORD_IS, EC_WORD_EXCITING};

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

const struct ScanlineEffectParams gUnknown_083DB698 =
{
    &REG_BG3VOFS,
    ((DMA_ENABLE | DMA_START_HBLANK | DMA_REPEAT | DMA_DEST_RELOAD) << 16) | 1,
    1
};

const u8 gUnknown_083DB6A4[] = {4, 0, 0, 0, 1, 5, 0, 2, 2, 3, 2, 2, 2, 3};

// choose by alphabet keyboard
const u8 gUnknown_083DB6B2[][16] =
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
    bool8 unk8;
};

const struct EasyChatPrompt gUnknown_083DB6F4[] =
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

const u8 gUnknown_083DB7C0[][2] =
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

void sub_80E62F8(void);
void sub_80E6424(void);
void sub_80E6554(void);
void sub_80E6630(void);
void sub_80E6690(void);
void sub_80E682C(void (*)(void));
void sub_80E69F8(void);
void sub_80E6A6C(void);
void sub_80E6A88(void);
void sub_80E6AA8(void);
void sub_80E6AC4(void);
void sub_80E6AE4(void);
void sub_80E6BC0(void);
void sub_80E6C84(void);
void sub_80E6D7C(void);
void sub_80E6F68(void);
void sub_80E6FC8(void);
void sub_80E7114(void);
void sub_80E718C(void);
void sub_80E7218(void);
void sub_80E7294(void);
void sub_80E7324(void);
void sub_80E73D0(void);
void sub_80E7458(void);
void sub_80E752C(void);
void sub_80E7574(void);
bool8 sub_80E75D8(void);
bool8 sub_80E77C8(void);
void sub_80E7A98(void);
void sub_80E7AD4(void);
bool8 sub_80E7B40(void);
void sub_80E7D30(void);
void sub_80E7D6C(void);
static void SaveCurrentPhrase(void);
bool8 sub_80E7DD0(void);
void sub_80E7E50(void);
void sub_80E7F00(u16, u16);
u8 sub_80E7FA8(void);
static bool8 IsCurrentPhraseEmpty(void);
u8 sub_80E8094(void);
u8 sub_80E810C(void);
void sub_80E81C0(void);
void sub_80E81FC(void);
void sub_80E8218(void);

void sub_80E8398();
void sub_80E8420(void);
void sub_80E8504(void);
void sub_80E87CC();

void sub_80E88F0(void);
void sub_80E8958();

void sub_80E8BF4();
void sub_80E8CEC(void);
void sub_80E8D54(void);
void sub_80E8D8C();
void sub_80E8DD8(void);
void sub_80E91D4();
void sub_80E9368();
void sub_80E95A4(void);
void sub_80E9744(void);
void sub_80E98C4(void);
void sub_80E9974(void);
void sub_80E9A14(void);
void sub_80E9A4C(void);
void sub_80E9AD4(void);
void sub_80E9C94(void);
void sub_80E9D00(void);
void sub_80E9D7C(void);
void sub_80E9E08();
bool8 sub_80E9E54(void);
void sub_80E9E98(void);
u8 sub_80E9EA8(void);
u8 sub_80E9F50(void);
u8 sub_80E9FD4(void);
u8 sub_80EA014(void);
u8 sub_80EA050(void);
u8 sub_80EA0E4(void);
u8 sub_80EA184(void);
void sub_80EAC5C(void);
void sub_80EAD08(void);
void sub_80EAECC(void);
void LoadEasyChatStrings(void);
void sub_80EB0B0(void);
u8 *sub_80EB218(u8 *, u16, u16);
bool8 sub_80EB680(u16 *, u16, u16, u16);

void ShowEasyChatScreen(void)
{
    u8 displayedPersonType = 3;
    u16 *words;

    switch (gSpecialVar_0x8004)
    {
    case 0:
        words = gSaveBlock1.easyChats.easyChatProfile;
        break;
    case 1:
        words = gSaveBlock1.easyChats.easyChatBattleStart;
        break;
    case 2:
        words = gSaveBlock1.easyChats.easyChatBattleWon;
        break;
    case 3:
        words = gSaveBlock1.easyChats.easyChatBattleLost;
        break;
    case 4:
        words = gSaveBlock1.mail[gSpecialVar_0x8005].words;
        break;
    case 6:
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
        words = gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubLetter.words;
        displayedPersonType = gSpecialVar_0x8006;
        break;
    case 7:
        // TODO: Is this the right TV show?
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubOpinions.words[gSpecialVar_0x8006];
        displayedPersonType = 1;
        break;
    case 8:
        // TODO: Is this the right TV show?
        words = &gSaveBlock1.tvShows[gSpecialVar_0x8005].fanclubOpinions.species;
        displayedPersonType = 0;
        break;
    case 9:
        words = NULL;
        break;
    case 10:
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
    case 13:
        gEasyChatStruct->unk9C7C = 0xFFFF;
        gEasyChatStruct->unk9C7E = -1;
        words = &gEasyChatStruct->unk9C7C;
        break;
    default:
        return;
    }
    sub_80E62A0(gSpecialVar_0x8004, words, CB2_ReturnToFieldContinueScript, displayedPersonType);
}

void sub_80E62A0(u8 type, u16 *words, void (*exitCallback)(void), u8 displayedPersonType)
{
    gEasyChatStruct->exitCallback = exitCallback;
    gEasyChatStruct->savedPhrase = words;
    gEasyChatStruct->type = type;
    gEasyChatStruct->displayedPersonType = displayedPersonType;
    if (type == 9)
    {
        gEasyChatStruct->savedPhrase = &gEasyChatStruct->unk9C7C;
        gEasyChatStruct->unk9C7C = gSaveBlock1.dewfordTrends[0].words[0];
        gEasyChatStruct->unk9C7E = gSaveBlock1.dewfordTrends[0].words[1];
    }
    SetMainCallback2(sub_80E62F8);
}

void sub_80E62F8(void)
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
        sub_80EAD08();
        ScanlineEffect_SetParams(gUnknown_083DB698);
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
        sub_80E6424();
        break;
    case 4:
        sub_80E8DD8();
        break;
    case 5:
        sub_80E8218();
        sub_80E8CEC();
        break;
    case 6:
        sub_80E69F8();
        sub_80E682C(sub_80E6AA8);
        SetVBlankCallback(sub_80E6A6C);
        break;
    case 7:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        break;
    case 8:
        REG_DISPCNT = 0x1F40;
        SetMainCallback2(sub_80E6A88);
        FlagSet(FLAG_SYS_CHAT_USED);
        break;
    }
    gMain.state++;
}

void sub_80E6424(void)
{
    gEasyChatStruct->frameId = gUnknown_083DB6A4[gEasyChatStruct->type];
    switch (gEasyChatStruct->frameId)
    {
    case 4:
        gEasyChatStruct->maxWords = 4;
        gEasyChatStruct->numColumns = 2;
        gEasyChatStruct->numRows = 2;
        gEasyChatStruct->unk88 = 5;
        gEasyChatStruct->unk8A = 4;
        break;
    case 5:
        gEasyChatStruct->maxWords = 4;
        gEasyChatStruct->numColumns = 1;
        gEasyChatStruct->numRows = 4;
        gEasyChatStruct->unk88 = 16;
        gEasyChatStruct->unk8A = 2;
        break;
    case 0:
        gEasyChatStruct->maxWords = 6;
        gEasyChatStruct->numColumns = 2;
        gEasyChatStruct->numRows = 3;
        gEasyChatStruct->unk88 = 4;
        gEasyChatStruct->unk8A = 3;
        break;
    case 1:
        gEasyChatStruct->maxWords = 9;
        gEasyChatStruct->numColumns = 2;
        gEasyChatStruct->numRows = 5;
        gEasyChatStruct->unk88 = 4;
        gEasyChatStruct->unk8A = 0;
        break;
    case 2:
        gEasyChatStruct->maxWords = 1;
        gEasyChatStruct->numColumns = 1;
        gEasyChatStruct->numRows = 1;
        gEasyChatStruct->unk88 = 16;
        gEasyChatStruct->unk8A = 4;
        break;
    case 3:
        gEasyChatStruct->maxWords = 2;
        gEasyChatStruct->numColumns = 2;
        gEasyChatStruct->numRows = 1;
        gEasyChatStruct->unk88 = 5;
        gEasyChatStruct->unk8A = 3;
        break;
    }
    gEasyChatStruct->mainCursorRow = 0;
    gEasyChatStruct->mainCursorColumn = 0;
    gEasyChatStruct->unk87 = 0;
    gEasyChatStruct->inAlphabetMode = 0;
    gEasyChatStruct->unk1BA = 0;
    gEasyChatStruct->unk1BE = 2;
    sub_80E6554();
    sub_80EAECC();
    LoadEasyChatStrings();
    sub_80E7E50();
    sub_80E6630();
    sub_80E6690();
}

void sub_80E6554(void)
{
    u16 r4 = 0;
    u16 r7;
    u16 r5;

    for (r7 = 0; ; r7++)
    {
        for (r5 = 0; r5 < 2; r5++)
        {
            gEasyChatStruct->unlockedGroupIds[r7][r5] = r4++;
            if (r4 == 17)
                break;
        }
        if (r4 == 17)
            break;
    }
    gEasyChatStruct->numUnlockedGroups = 17;
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
            gEasyChatStruct->unlockedGroupIds[r7][r5] = r4;
            gEasyChatStruct->unk78[r4 - 17] = 1;  // hmm...
            gEasyChatStruct->numUnlockedGroups++;
        }
        else
        {
            gEasyChatStruct->unk78[r4 - 17] = 0;
        }
        r4++;
    }
    gEasyChatStruct->keyboardLastRow = (gEasyChatStruct->numUnlockedGroups + 1) / 2;
}

void sub_80E6630(void)
{
    u8 i;
    u8 r3;

    for (i = 0; i < 4; i++)
    {
        const u8 *row = gUnknown_083DB6B2[i];

        for (r3 = 0; row[r3] != EOS; r3++)
        {
            if (row[r3] != CHAR_SPACE)
                gEasyChatStruct->unk40[i][r3] = row[r3] + 0x46;
            else
                gEasyChatStruct->unk40[i][r3] = CHAR_SPACE;
        }
    }
}

void sub_80E6690(void)
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
        const struct EasyChatPrompt *prompt = &gUnknown_083DB6F4[gUnknown_083DB7C0[gEasyChatStruct->type][i]];

        r3 = StringCopy(pointers[i * 2 + 0], prompt->text1);
        if (prompt->unk8)
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

void sub_80E682C(void (*func)(void))
{
    gEasyChatStruct->unk20 = func;
    gEasyChatStruct->funcState = 0;
}

void sub_80E683C(void)
{
    u16 i;

    if (gEasyChatStruct->inAlphabetMode == 0)
    {
        for (i = 0; i < gEasyChatStruct->keyboardLastRow; i++)
            gEasyChatStruct->unk1AA[i] = 2;
        gEasyChatStruct->unk1AA[i - 1] = gEasyChatStruct->numUnlockedGroups % 2;
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
    gEasyChatStruct->keyboardScrollOffset = 0;
    gEasyChatStruct->unk1B7 = 0;
    sub_80E9A4C();
}

void sub_80E68E8(void)
{
    sub_80EB0B0();
    if (gEasyChatStruct->inAlphabetMode == 0)
    {
        u16 i;
        u8 groupId;

        groupId = gEasyChatStruct->unk1B8;
        gEasyChatStruct->unk9A28 = (gEasyChatStruct->unk4178[groupId] + 1) / 2;
        for (i = 0; i < gEasyChatStruct->unk9A28; i++)
            gEasyChatStruct->unk99A6[i] = 2;
        i--;
        gEasyChatStruct->unk99A6[i] = gEasyChatStruct->unk4178[groupId] % 2;
        if (gEasyChatStruct->unk99A6[i] == 0)
            gEasyChatStruct->unk99A6[i] = 2;
    }
    else
    {
        u16 i;
        u8 groupId;

        groupId = gEasyChatStruct->unk1B8;
        gEasyChatStruct->unk9A28 = (gEasyChatStruct->numUnlockedAlphabetWords[groupId] + 1) / 2;
        for (i = 0; i < gEasyChatStruct->unk9A28; i++)
            gEasyChatStruct->unk99A6[i] = 2;
        i--;
        gEasyChatStruct->unk99A6[i] = gEasyChatStruct->numUnlockedAlphabetWords[groupId] % 2;
        if (gEasyChatStruct->unk99A6[i] == 0)
            gEasyChatStruct->unk99A6[i] = 2;
    }
    gEasyChatStruct->unk99A4 = 0;
    gEasyChatStruct->wordSelectColumn = 0;
    gEasyChatStruct->wordSelectScrollOffset = 0;
    sub_80E9A4C();
}

void sub_80E69F8(void)
{
    switch (gEasyChatStruct->type)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 9:
    case 13:
    default:
        sub_80E9368(gEasyChatStruct->type);
        break;
    case 5:
    case 7:
    case 8:
    case 10:
    case 11:
    case 12:
        sub_80E9368(gEasyChatStruct->type);
        sub_80E8BF4(gEasyChatStruct->displayedPersonType, gEasyChatStruct->frameId);
        break;
    }
}

void sub_80E6A6C(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    sub_80EAC5C();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void sub_80E6A88(void)
{
    gEasyChatStruct->unk20();
    AnimateSprites();
    BuildOamBuffer();
    sub_80EAD08();
}

void sub_80E6AA8(void)
{
    if (!UpdatePaletteFade())
        sub_80E682C(sub_80E6AC4);
}

void sub_80E6AC4(void)
{
    sub_80E88F0();
    sub_80E8398(0);
    sub_80E91D4(0);
    sub_80E682C(sub_80E6AE4);
}

void sub_80E6AE4(void)
{
    gEasyChatStruct->unk87 = sub_80E75D8();
    if (gEasyChatStruct->unk87)
        PlaySE(SE_SELECT);
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gEasyChatStruct->mainCursorRow == gEasyChatStruct->numRows)
        {
            switch (gEasyChatStruct->mainCursorColumn)
            {
            case 0:
                sub_80E682C(sub_80E6BC0);
                return;
            case 1:
                sub_80E682C(sub_80E6C84);
                return;
            case 2:
                sub_80E682C(sub_80E6D7C);
                return;
            }
        }
        else
        {
            gEasyChatStruct->unk27 = gEasyChatStruct->mainCursorRow * gEasyChatStruct->numColumns + gEasyChatStruct->mainCursorColumn;
            sub_80E7574();
            sub_80E682C(sub_80E6F68);
            return;
        }
    }
    if (JOY_NEW(B_BUTTON))
    {
        sub_80E682C(sub_80E6C84);
    }
}

void sub_80E6BC0(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        sub_80E8398(2);
        if (gEasyChatStruct->type == 6)
        {
            sub_80E91D4(6);
            gEasyChatStruct->funcState = 100;
        }
        else
        {
            sub_80E91D4(2);
            DisplayYesNoMenu(23, 8, 1);
            Menu_MoveCursor(1);
            gEasyChatStruct->funcState++;
        }
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            sub_80E7D6C();
            sub_80E98C4();
            sub_80E95A4();
            gEasyChatStruct->funcState++;
            break;
        case -1:
        case 1:
            gEasyChatStruct->funcState++;
            break;
        }
        break;
    case 2:
        sub_80E81FC();
        sub_80E682C(sub_80E6AC4);
        break;
    case 100:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
            sub_80E682C(sub_80E6AC4);
        break;
    }
}

void sub_80E6C84(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        sub_80E8398(2);
        sub_80E91D4(3);
        DisplayYesNoMenu(23, 8, 0);
        Menu_MoveCursor(1);
        if (gEasyChatStruct->type == 9
         || gEasyChatStruct->type == 4
         || gEasyChatStruct->type == 7
         || gEasyChatStruct->type == 8
         || gEasyChatStruct->type == 10
         || gEasyChatStruct->type == 11
         || gEasyChatStruct->type == 12
         || gEasyChatStruct->type == 5
         || gEasyChatStruct->type == 13)
            gEasyChatStruct->funcState = 2;
        else
            gEasyChatStruct->funcState++;
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            sub_80E91D4(4);
            DisplayYesNoMenu(23, 8, 0);
            Menu_MoveCursor(1);
            gEasyChatStruct->funcState++;
            break;
        case -1:
        case 1:
            gEasyChatStruct->funcState = 0xFF;
            break;
        }
        break;
    case 2:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            gSpecialVar_Result = 0;
            sub_80E682C(sub_80E752C);
            break;
        case -1:
        case 1:
            gEasyChatStruct->funcState = 0xFF;
            break;
        }
        break;
    case 0xFF:
        Menu_DestroyCursor();
        sub_80E81FC();
        sub_80E682C(sub_80E6AC4);
        break;
    }
}

void sub_80E6D7C(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        sub_80E8398(2);
        if (IsCurrentPhraseEmpty())
        {
            sub_80E91D4(5);
            gEasyChatStruct->funcState = 10;
            break;
        }
        if (gEasyChatStruct->type == 9)
        {
            if (sub_80E7FA8() == 0)
            {
                sub_80E91D4(8);
                gEasyChatStruct->funcState = 10;
                break;
            }
            if (gEasyChatStruct->currentPhrase[0] == 0xFFFF || gEasyChatStruct->currentPhrase[1] == 0xFFFF)
            {
                sub_80E91D4(9);
                gEasyChatStruct->funcState = 10;
                break;
            }
        }
        if (gEasyChatStruct->type == 4 && sub_80E7FA8() == 0)
        {
            sub_80E682C(sub_80E6C84);
        }
        else
        {
            sub_80E91D4(1);
            sub_80E9744();
            DisplayYesNoMenu(23, 8, 0);
            Menu_MoveCursor(0);
            gEasyChatStruct->funcState++;
        }
        break;
    case 1:
        switch (Menu_ProcessInputNoWrap_())
        {
        case 0:
            gSpecialVar_Result = (sub_80E7FA8() != 0);
            SaveCurrentPhrase();
            if (gEasyChatStruct->type == 0)
                gSpecialVar_0x8004 = sub_80E8094();
            if (gEasyChatStruct->type == 9)  // dewford trend?
            {
                sub_80E81C0();
                gSpecialVar_0x8004 = TrySetTrendyPhrase(&gEasyChatStruct->unk9C7C);
            }
            if (gEasyChatStruct->type == 13)
            {
                if (gEasyChatStruct->currentPhrase[0] == 0xFFFF || gEasyChatStruct->currentPhrase[1] == 0xFFFF)
                    gSpecialVar_Result = 0;
                gSpecialVar_0x8004 = sub_80E810C();
            }
            sub_80E682C(sub_80E752C);
            break;
        case -1:
        case 1:
            Menu_DestroyCursor();
            sub_80E81FC();
            if (gEasyChatStruct->type == 6 && sub_80E7FA8() != 0)
            {
                gEasyChatStruct->funcState = 100;
            }
            else
            {
                sub_80E95A4();
                sub_80E682C(sub_80E6AC4);
            }
            break;
        }
        break;
    case 10:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
            sub_80E682C(sub_80E6AC4);
        break;
    case 100:
        sub_80E91D4(7);
        gEasyChatStruct->funcState++;
        // fall through
    case 101:
        if (JOY_NEW(A_BUTTON))
            gEasyChatStruct->funcState++;
        break;
    case 102:
        sub_80E7E50();
        sub_80E95A4();
        sub_80E682C(sub_80E6AC4);
        break;
    }
}

void sub_80E6F68(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        sub_80E8398(1);
        sub_80E91D4(10);
        sub_80E683C();
        sub_80E9974();
        sub_80E9E98();
        gEasyChatStruct->funcState++;
        break;
    case 1:
        if (sub_80E9EA8() != 0)
        {
            sub_80E8D8C(1);
            sub_80E8420();
            sub_80E8958(0);
            sub_80E682C(sub_80E6FC8);
        }
        break;
    }
}

void sub_80E6FC8(void)
{
    gEasyChatStruct->unk96 = sub_80E77C8();
    if (gEasyChatStruct->unk1C0 != 0)
    {
        PlaySE(SE_SELECT);
        gEasyChatStruct->unk1C4 = sub_80E6FC8;
        sub_80E682C(sub_80E7458);
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
                    sub_80E682C(sub_80E718C);
                    break;
                case 2:
                    if (gEasyChatStruct->type != 6)
                    {
                        sub_80E7F00(gEasyChatStruct->unk27, 0xFFFF);
                        sub_80E7574();
                        sub_80E95A4();
                    }
                    break;
                case 3:
                    sub_80E682C(sub_80E7114);
                    break;
                }
            }
            else
            {
                if (gEasyChatStruct->inAlphabetMode == 0
                 || gEasyChatStruct->numUnlockedAlphabetWords[gEasyChatStruct->unk40[gEasyChatStruct->unk1A8][gEasyChatStruct->unk1A9]] != 0)
                {
                    PlaySE(SE_SELECT);
                    sub_80E7AD4();
                    sub_80E682C(sub_80E7218);
                }
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            sub_80E682C(sub_80E7114);
        }
        else if (JOY_NEW(SELECT_BUTTON))
        {
            sub_80E682C(sub_80E718C);
        }
    }
}

void sub_80E7114(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        sub_80E8504();
        sub_80E9E98();
        sub_80E88F0();
        sub_80E8D8C(0);
        gEasyChatStruct->funcState++;
        break;
    case 1:
    case 2:
        gEasyChatStruct->funcState++;
        break;
    case 3:
        if (sub_80E9F50() != 0)
            gEasyChatStruct->funcState++;
        break;
    case 4:
        sub_80E682C(sub_80E6AC4);
        break;
    }
}

void sub_80E718C(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        sub_80E8504();
        sub_80E9E98();
        sub_80E88F0();
        gEasyChatStruct->funcState++;
        sub_80E8D54();
        break;
    case 1:
        if (sub_80E9FD4() != 0)
        {
            gEasyChatStruct->inAlphabetMode = !gEasyChatStruct->inAlphabetMode;
            sub_80E683C();
            sub_80E9974();
            sub_80E9E98();
            gEasyChatStruct->funcState++;
        }
        break;
    default:
        gEasyChatStruct->funcState++;
        break;
    case 8:
        if (sub_80EA014() != 0)
        {
            sub_80E8420();
            sub_80E8958(0);
            sub_80E682C(sub_80E6FC8);
        }
        break;
    }
}

void sub_80E7218(void)
{
    switch (gEasyChatStruct->funcState)
    {
    default:
        gEasyChatStruct->funcState++;
        break;
    case 8:
        sub_80E8D8C(0);
        sub_80E8504();
        sub_80E9AD4();
        sub_80E68E8();
        sub_80E88F0();
        sub_80E9E98();
        gEasyChatStruct->funcState++;
        break;
    case 9:
        if (sub_80EA050() != 0)
        {
            sub_80E9C94();
            gEasyChatStruct->funcState++;
        }
        break;
    case 10:
        sub_80E87CC(1);
        sub_80E8958(1);
        sub_80E682C(sub_80E7294);
        break;
    case 11:
        break;
    }
}

void sub_80E7294(void)
{
    gEasyChatStruct->unk1B9 = sub_80E7B40();
    if (gEasyChatStruct->unk1C0 != 0)
    {
        PlaySE(SE_SELECT);
        gEasyChatStruct->unk1C4 = sub_80E7294;
        sub_80E682C(sub_80E7458);
    }
    else
    {
        if (gEasyChatStruct->unk1B9)
            PlaySE(SE_SELECT);
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sub_80E682C(sub_80E7324);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            sub_80E682C(sub_80E73D0);
        }
    }
}

void sub_80E7324(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        if (!sub_80E7DD0())
        {
            sub_80E682C(sub_80E7294);
        }
        else
        {
            sub_80E88F0();
            sub_80E87CC(0);
            gEasyChatStruct->funcState++;
        }
        break;
    case 1:
        gEasyChatStruct->funcState++;
        break;
    case 2:
        sub_80E9E98();
        gEasyChatStruct->funcState++;
        break;
    case 3:
        if (sub_80EA184() != 0)
            gEasyChatStruct->funcState++;
        break;
    case 4:
        if (gEasyChatStruct->type == 6 && sub_80E7FA8() != 0)
            sub_80E682C(sub_80E6D7C);
        else
            sub_80E682C(sub_80E6AC4);
        break;
    }
}

void sub_80E73D0(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        sub_80E87CC(0);
        sub_80E88F0();
        gEasyChatStruct->funcState++;
        break;
    case 1:
        sub_80E9AD4();
        sub_80E9E98();
        gEasyChatStruct->funcState++;
        break;
    case 2:
        if (sub_80EA0E4() != 0)
        {
            sub_80E8D8C(1);
            sub_80E9A14();
            gEasyChatStruct->funcState++;
        }
        break;
    case 3:
        sub_80E8420();
        sub_80E8958(0);
        gEasyChatStruct->funcState++;
        break;
    case 4:
        sub_80E9974();
        sub_80E682C(sub_80E6FC8);
        break;
    }
}

void sub_80E7458(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        if (gEasyChatStruct->unk1C4 == sub_80E6FC8)
            sub_80E9D7C();
        else
            sub_80E9D00();
        sub_80E9E08(gEasyChatStruct->unk1BE);
        gEasyChatStruct->funcState++;
        break;
    case 1:
        if (sub_80E9E54())
        {
            if (gEasyChatStruct->unk1C4 == sub_80E6FC8)
            {
                sub_80E9D7C();
                gEasyChatStruct->keyboardScrollOffset += gEasyChatStruct->unk1C0;
                sub_80E7A98();
                gEasyChatStruct->unk96 = TRUE;
            }
            else
            {
                gEasyChatStruct->wordSelectScrollOffset += gEasyChatStruct->unk1C0;
                sub_80E7D30();
                gEasyChatStruct->unk1B9 = 1;
            }
            gEasyChatStruct->unk1BE = 2;
            sub_80E682C(gEasyChatStruct->unk1C4);
        }
        break;
    }
}

void sub_80E752C(void)
{
    switch (gEasyChatStruct->funcState)
    {
    case 0:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gEasyChatStruct->funcState++;
        break;
    case 1:
        if (!UpdatePaletteFade())
            SetMainCallback2(gEasyChatStruct->exitCallback);
        break;
    }
}

void sub_80E7574(void)
{
    if (gEasyChatStruct->type == 1
     && gEasyChatStruct->unk7E[gEasyChatStruct->mainCursorRow] == 2
     && GetEasyChatWordStringLength(gEasyChatStruct->currentPhrase[gEasyChatStruct->unk27]) != 7)
        gEasyChatStruct->unk7D = 1;
    else
        gEasyChatStruct->unk7D = 0;
    gEasyChatStruct->unk7D = 0;  // What the hell?
}

bool8 sub_80E75D8(void)
{
    bool8 pressedUpDown = FALSE;
    u8 r0;

    if (JOY_NEW(START_BUTTON))
    {
        gEasyChatStruct->mainCursorRow = gEasyChatStruct->numRows;
        gEasyChatStruct->mainCursorColumn = 2;
        return TRUE;
    }

    if (JOY_REPEAT(DPAD_UP))
    {
        gEasyChatStruct->mainCursorRow--;
        if (gEasyChatStruct->mainCursorRow < 0)
            gEasyChatStruct->mainCursorRow = gEasyChatStruct->numRows;
        pressedUpDown = TRUE;
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        gEasyChatStruct->mainCursorRow++;
        if (gEasyChatStruct->mainCursorRow > gEasyChatStruct->numRows)
            gEasyChatStruct->mainCursorRow = 0;
        pressedUpDown = TRUE;
    }

    if (pressedUpDown)
    {
        if (gEasyChatStruct->frameId == 2)
        {
            if (gEasyChatStruct->mainCursorRow == gEasyChatStruct->numRows)
                gEasyChatStruct->mainCursorColumn = 2;
            else
                gEasyChatStruct->mainCursorColumn = 0;
            return TRUE;
        }
        else
        {
            if (gEasyChatStruct->mainCursorColumn >= gEasyChatStruct->numColumns)
                gEasyChatStruct->mainCursorColumn = gEasyChatStruct->numColumns - 1;
            if (gEasyChatStruct->mainCursorRow != gEasyChatStruct->numRows)
            {
                r0 = gEasyChatStruct->mainCursorRow * gEasyChatStruct->numColumns + gEasyChatStruct->mainCursorColumn;
                if (r0 >= gEasyChatStruct->maxWords)
                    gEasyChatStruct->mainCursorColumn = r0 - gEasyChatStruct->maxWords;
            }
            return TRUE;
        }
    }
    else
    {
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if (--gEasyChatStruct->mainCursorColumn < 0)
            {
                if (gEasyChatStruct->mainCursorRow == gEasyChatStruct->numRows)
                {
                    gEasyChatStruct->mainCursorColumn = 2;
                }
                else
                {
                    gEasyChatStruct->mainCursorColumn = gEasyChatStruct->numColumns - 1;
                    r0 = gEasyChatStruct->mainCursorRow * gEasyChatStruct->numColumns + gEasyChatStruct->mainCursorColumn;
                    if (r0 >= gEasyChatStruct->maxWords)
                        gEasyChatStruct->mainCursorColumn = r0 - gEasyChatStruct->maxWords;
                }
            }
            return TRUE;
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if (gEasyChatStruct->mainCursorRow == gEasyChatStruct->numRows)
            {
                if (++gEasyChatStruct->mainCursorColumn > 2)
                    gEasyChatStruct->mainCursorColumn = 0;
            }
            else
            {
                if (++gEasyChatStruct->mainCursorColumn >= gEasyChatStruct->numColumns)
                    gEasyChatStruct->mainCursorColumn = 0;
                r0 = gEasyChatStruct->mainCursorRow * gEasyChatStruct->numColumns + gEasyChatStruct->mainCursorColumn;
                if (r0 >= gEasyChatStruct->maxWords)
                    gEasyChatStruct->mainCursorColumn = r0 - gEasyChatStruct->maxWords;
            }
            return TRUE;
        }
    }
    return FALSE;
}

bool8 sub_80E77C8(void)
{
    bool8 pressedLeftRight = FALSE;
    bool8 pressedUpDown;

    if (gEasyChatStruct->unk1B7 != 0)
    {
        if (JOY_REPEAT(DPAD_UP))
        {
            gEasyChatStruct->unk1A8--;
            if (gEasyChatStruct->unk1A8 < 1)
                gEasyChatStruct->unk1A8 = 3;
            return TRUE;
        }
        else if (JOY_REPEAT(DPAD_DOWN))
        {
            gEasyChatStruct->unk1A8++;
            if (gEasyChatStruct->unk1A8 > 3)
                gEasyChatStruct->unk1A8 = 1;
            return TRUE;
        }
    }
    else
    {
        if (gEasyChatStruct->inAlphabetMode == 1)
        {
            pressedUpDown = FALSE;

            if (JOY_REPEAT(DPAD_UP))
            {
                gEasyChatStruct->unk1A8--;
                if (gEasyChatStruct->unk1A8 < 0)
                    gEasyChatStruct->unk1A8 = 3;
                pressedUpDown = TRUE;
            }
            else if (JOY_REPEAT(DPAD_DOWN))
            {
                gEasyChatStruct->unk1A8++;
                if (gEasyChatStruct->unk1A8 > 3)
                    gEasyChatStruct->unk1A8 = 0;
                pressedUpDown = TRUE;
            }

            if (pressedUpDown)
            {
                sub_80E7A98();
                return TRUE;
            }
        }
        else
        {
            pressedUpDown = FALSE;
            gEasyChatStruct->unk1C0 = 0;

            if (JOY_REPEAT(DPAD_UP))
            {
                if (gEasyChatStruct->unk1A8 == 0)
                    return FALSE;
                gEasyChatStruct->unk1A8--;
                if (gEasyChatStruct->unk1A8 < gEasyChatStruct->keyboardScrollOffset)
                    gEasyChatStruct->unk1C0 = -1;
                pressedUpDown = TRUE;
            }
            else if (JOY_REPEAT(DPAD_DOWN))
            {
                if (gEasyChatStruct->unk1A8 >= gEasyChatStruct->keyboardLastRow - 1)
                    return FALSE;
                gEasyChatStruct->unk1A8++;
                if (gEasyChatStruct->unk1A8 > gEasyChatStruct->keyboardScrollOffset + 3)
                    gEasyChatStruct->unk1C0 = 1;
                pressedUpDown = TRUE;
            }

            if (pressedUpDown)
            {
                if (gEasyChatStruct->unk1C0 == 0)
                {
                    sub_80E7A98();
                    return TRUE;
                }
                return FALSE;
            }
        }
    }

    if (JOY_REPEAT(DPAD_LEFT))
    {
        if (gEasyChatStruct->unk1A9 != 0)
            gEasyChatStruct->unk1A9--;
        else
            gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8];
        pressedLeftRight = TRUE;
    }
    else if (JOY_REPEAT(DPAD_RIGHT))
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
            gEasyChatStruct->unk1A8 -= gEasyChatStruct->keyboardScrollOffset;
            if (gEasyChatStruct->unk1A8 == 0)
            {
                gEasyChatStruct->unk1A8 = 1;
                gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8];
            }
        }
        else if (r9 != 0)
        {
            gEasyChatStruct->unk1A8 += gEasyChatStruct->keyboardScrollOffset;
            if (gEasyChatStruct->unk1A9 != 0)
                gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8] - 1;
        }
        return TRUE;
    }

    return FALSE;
}

void sub_80E7A98(void)
{
    if (gEasyChatStruct->unk1A9 >= gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8])
        gEasyChatStruct->unk1A9 = gEasyChatStruct->unk1AA[gEasyChatStruct->unk1A8] - 1;
}

void sub_80E7AD4(void)
{
    if (gEasyChatStruct->inAlphabetMode == 0)
        gEasyChatStruct->unk1B8 = gEasyChatStruct->unlockedGroupIds[gEasyChatStruct->unk1A8][gEasyChatStruct->unk1A9];
    else
        gEasyChatStruct->unk1B8 = gEasyChatStruct->unk40[gEasyChatStruct->unk1A8][gEasyChatStruct->unk1A9];
}

bool8 sub_80E7B40(void)
{
    bool8 pressedUpDown = FALSE;

    gEasyChatStruct->unk1C0 = 0;
    if (JOY_REPEAT(DPAD_UP))
    {
        if (gEasyChatStruct->unk99A4 == 0)
            return FALSE;
        gEasyChatStruct->unk99A4--;
        if (gEasyChatStruct->unk99A4 < gEasyChatStruct->wordSelectScrollOffset)
        {
            gEasyChatStruct->unk1C0 = -1;
            return FALSE;
        }
        pressedUpDown = TRUE;
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        if (gEasyChatStruct->unk99A4 >= gEasyChatStruct->unk9A28 - 1)
            return FALSE;
        gEasyChatStruct->unk99A4++;
        if (gEasyChatStruct->unk99A4 >= gEasyChatStruct->wordSelectScrollOffset + 4)
        {
            gEasyChatStruct->unk1C0 = 1;
            return FALSE;
        }
        pressedUpDown = TRUE;
    }

    if (pressedUpDown)
    {
        sub_80E7D30();
        return TRUE;
    }

    if (JOY_REPEAT(DPAD_LEFT))
    {
        gEasyChatStruct->wordSelectColumn--;
        if (gEasyChatStruct->wordSelectColumn < 0)
            gEasyChatStruct->wordSelectColumn = gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4] - 1;
        return TRUE;
    }
    else if (JOY_REPEAT(DPAD_RIGHT))
    {
        gEasyChatStruct->wordSelectColumn++;
        if (gEasyChatStruct->wordSelectColumn >= gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4])
            gEasyChatStruct->wordSelectColumn = 0;
        return TRUE;
    }

    if (JOY_NEW(START_BUTTON))
    {
        if (gEasyChatStruct->wordSelectScrollOffset != 0)
        {
            gEasyChatStruct->unk1C0 = -gEasyChatStruct->wordSelectScrollOffset;
            if (gEasyChatStruct->unk1C0 < -4)
                gEasyChatStruct->unk1C0 = -4;
        }
        gEasyChatStruct->unk99A4 += gEasyChatStruct->unk1C0;
        gEasyChatStruct->unk1BE = 4;
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        if (gEasyChatStruct->wordSelectScrollOffset < gEasyChatStruct->unk9A28 - 4)
        {
            gEasyChatStruct->unk1C0 = gEasyChatStruct->unk9A28 - 4 - gEasyChatStruct->wordSelectScrollOffset;
            if (gEasyChatStruct->unk1C0 > 4)
                gEasyChatStruct->unk1C0 = 4;
        }
        gEasyChatStruct->unk99A4 += gEasyChatStruct->unk1C0;
        gEasyChatStruct->unk1BE = 4;
    }

    return FALSE;
}

void sub_80E7D30(void)
{
    if (gEasyChatStruct->wordSelectColumn >= gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4])
        gEasyChatStruct->wordSelectColumn = gEasyChatStruct->unk99A6[gEasyChatStruct->unk99A4] - 1;
}

void sub_80E7D6C(void)
{
    u16 i;

    for (i = 0; i < gEasyChatStruct->maxWords; i++)
        sub_80E7F00(i, 0xFFFF);
}

static void SaveCurrentPhrase(void)
{
    u16 i;

    for (i = 0; i < gEasyChatStruct->maxWords; i++)
        gEasyChatStruct->savedPhrase[i] = gEasyChatStruct->currentPhrase[i];
}

bool8 sub_80E7DD0(void)
{
    u16 easyChatWord = gEasyChatStruct->selectedGroupWords[gEasyChatStruct->unk99A4][gEasyChatStruct->wordSelectColumn];

    if (gEasyChatStruct->unk7D != 0
     && gEasyChatStruct->unk7E[gEasyChatStruct->mainCursorRow] > 1
     && GetEasyChatWordStringLength(easyChatWord) == 7)
        return FALSE;

    sub_80E7F00(gEasyChatStruct->unk27, easyChatWord);
    sub_80E95A4();
    return TRUE;
}

void sub_80E7E50(void)
{
    u16 r5 = 0;
    u16 i;
    u16 j;

    for (i = 0; i < gEasyChatStruct->numRows; i++)
    {
        gEasyChatStruct->unk7E[i] = 0;
        for (j = 0; j < gEasyChatStruct->numColumns; j++)
        {
            gEasyChatStruct->currentPhrase[r5] = gEasyChatStruct->savedPhrase[r5];
            gEasyChatStruct->unk8C[i][j] = 0;
            r5++;
        }
    }
}

void sub_80E7F00(u16 index, u16 easyChatWord)
{
    u16 r5 = index / gEasyChatStruct->numColumns;
    u16 r8 = index % gEasyChatStruct->numColumns;
    u16 r4 = GetEasyChatWordStringLength(gEasyChatStruct->currentPhrase[index]);
    u16 r3 = GetEasyChatWordStringLength(easyChatWord);

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
    gEasyChatStruct->currentPhrase[index] = easyChatWord;
}

u8 sub_80E7FA8(void)
{
    u16 r8 = 0;
    u16 i;
    u8 *r1;
    u8 *r2;

    for (i = 0; i < gEasyChatStruct->maxWords; i++)
    {
        sub_80EB218(gEasyChatStruct->unk9E14, gEasyChatStruct->savedPhrase[i], 0);
        sub_80EB218(gEasyChatStruct->unk9E41, gEasyChatStruct->currentPhrase[i], 0);
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

static bool8 IsCurrentPhraseEmpty(void)
{
    u16 i;

    for (i = 0; i < gEasyChatStruct->maxWords; i++)
    {
        if (gEasyChatStruct->currentPhrase[i] != 0xFFFF)
            return FALSE;
    }
    return TRUE;
}

// CheckMysteryEventPhrase
bool8 sub_80E8094(void)
{
    u16 i;
    u8 *r3;
    u8 *r4;

    for (i = 0; i < 4; i++)
    {
        sub_80EB218(gEasyChatStruct->unk9E14, gEasyChatStruct->currentPhrase[i], 0);
        sub_80EB218(gEasyChatStruct->unk9E41, gMysteryEventPhrase[i], 0);
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

u8 sub_80E810C(void)
{
    u16 i;

    for (i = 0; i < 5; i++)
    {
        u8 *ptr;
        u8 *r3;

        ptr = sub_80EB218(gEasyChatStruct->unk9E6E, gEasyChatStruct->currentPhrase[0], 0);
        *ptr++ = CHAR_SPACE;
        sub_80EB218(ptr, gEasyChatStruct->currentPhrase[1], 0);

        ptr = sub_80EB218(gEasyChatStruct->unk9EEE, sBerryMasterWifePhrases[i][0], 0);
        *ptr++ = CHAR_SPACE;
        sub_80EB218(ptr, sBerryMasterWifePhrases[i][1], 0);

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

void sub_80E81C0(void)
{
    u8 *ptr;

    ptr = sub_80EB218(gStringVar2, gEasyChatStruct->unk9C7C, 0);
    *ptr++ = CHAR_SPACE;
    sub_80EB218(ptr, gEasyChatStruct->unk9C7E, 0);
}

void sub_80E81FC(void)
{
    PlaySE(SE_SELECT);
    sub_80E95A4();
    Menu_EraseWindowRect(0, 0, 29, 13);
}
