#include "global.h"
#include "bard_music.h"
#include "mauville_old_man.h"
#include "easy_chat.h"
#include "constants/easy_chat.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_message_box.h"
#include "m4a.h"
#include "menu.h"
#include "overworld.h"
#include "random.h"
#include "script.h"
#include "constants/event_objects.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "trader.h"

#define WORD_TO_PITCH_TABLE_INDEX(a) (((a) % 4) + (((a) / 8) & 1))

extern struct MusicPlayerInfo gMPlayInfo_SE2;

extern u16 gSpecialVar_Result;
extern u16 gSpecialVar_0x8004;

struct BardSong gBardSong;

EWRAM_DATA static u16 sUnusedPitchTableIndex = 0;  // set but not used?

static const u16 sDefaultBardSongLyrics[] =
{
#ifdef ENGLISH
    EC_WORD_SISTER,
    EC_WORD_EATS,
    EC_WORD_SWEETS,
    EC_WORD_VORACIOUS,
    EC_WORD_AND,
    EC_WORD_DROOLING,
#else
    EC_WORD_SISTER,
    EC_WORD_MUST_BE,
    EC_WORD_SWEETS,
    EC_WORD_VORACIOUS,
    EC_WORD_DROOLING,
    EC_WORD_THICK,
#endif
};

static const u8 *const sGiddyAdjectives[] =
{
    GiddyText_SoPretty,
    GiddyText_SoDarling,
    GiddyText_SoRelaxed,
    GiddyText_SoSunny,
    GiddyText_SoDesirable,
    GiddyText_SoExciting,
    GiddyText_SoAmusing,
    GiddyText_SoMagical,
};

static const u8 *const sGiddyQuestions[] =
{
    GiddyText_ISoWantToGoOnAVacation,
    GiddyText_IBoughtCrayonsWith120Colors,
    GiddyText_WouldntItBeNiceIfWeCouldFloat,
    GiddyText_WhenYouWriteOnASandyBeach,
    GiddyText_WhatsTheBottomOfTheSeaLike,
    GiddyText_WhenYouSeeTheSettingSunDoesIt,
    GiddyText_LyingBackInTheGreenGrass,
    GiddyText_SecretBasesAreSoWonderful,
};

static void InitGiddyTaleList(void);
static void Task_BardSong(u8);
static void StartBardSong(bool8 useNewSongLyrics);
static void StorytellerSetup(void);
static void Storyteller_ResetFlag(void);

static void SetupBard(void)
{
    u16 i;
    struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;

    bard->id = MAUVILLE_MAN_BARD;
    bard->hasChangedSong = FALSE;
    for (i = 0; i < 6; i++)
        bard->songLyrics[i] = sDefaultBardSongLyrics[i];
}

static void SetupHipster(void)
{
    struct MauvilleManHipster *hipster = &gSaveBlock1.oldMan.hipster;

    hipster->id = MAUVILLE_MAN_HIPSTER;
    hipster->taughtWord = FALSE;
}

static void SetupStoryteller(void)
{
    StorytellerSetup();
}

static void SetupGiddy(void)
{
    struct MauvilleManGiddy *giddy = &gSaveBlock1.oldMan.giddy;

    giddy->id = MAUVILLE_MAN_GIDDY;
    giddy->taleCounter = 0;
}

static void SetupTrader(void)
{
    TraderSetup();
}

void SetMauvilleOldMan(void)
{
    u16 trainerId = (gSaveBlock2.playerTrainerId[1] << 8) | gSaveBlock2.playerTrainerId[0];

    // Determine man based on the last digit of the player's trainer ID.
    switch ((trainerId % 10) / 2)
    {
    case MAUVILLE_MAN_BARD:
        SetupBard();
        break;
    case MAUVILLE_MAN_HIPSTER:
        SetupHipster();
        break;
    case MAUVILLE_MAN_TRADER:
        SetupTrader();
        break;
    case MAUVILLE_MAN_STORYTELLER:
        SetupStoryteller();
        break;
    case MAUVILLE_MAN_GIDDY:
        SetupGiddy();
        break;
    }
    SetMauvilleOldManObjEventGfx();
}

#if DEBUG
void debug_sub_810B32C(u8 a)
{
    u8 i;
    u8 savedArr[8];

    switch (a)
    {
    case 0:
        SetupBard();
        break;
    case 1:
        for (i = 0; i < 8; i++)
            savedArr[i] = gSaveBlock1.unlockedTrendySayings[i];
        SetupHipster();
        for (i = 0; i < 8; i++)
            gSaveBlock1.unlockedTrendySayings[i] = savedArr[i];
        break;
    case 2:
        SetupTrader();
        break;
    case 3:
        SetupStoryteller();
        break;
    case 4:
        SetupGiddy();
        break;
    }
    SetMauvilleOldManObjEventGfx();
}
#endif

u8 GetCurrentMauvilleOldMan(void)
{
    struct MauvilleManCommon *common = &gSaveBlock1.oldMan.common;

    return common->id;
}

void Script_GetCurrentMauvilleMan(void)
{
    gSpecialVar_Result = GetCurrentMauvilleOldMan();
}

void HasBardSongBeenChanged(void)
{
    u16 *scriptResult = &gSpecialVar_Result; // why??
    struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;

    *scriptResult = bard->hasChangedSong;
}

void SaveBardSongLyrics(void)
{
    u16 i;
    struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;

    StringCopy(bard->playerName, gSaveBlock2.playerName);

    for (i = 0; i < 4; i++)
        bard->playerTrainerId[i] = gSaveBlock2.playerTrainerId[i];

    for (i = 0; i < 6; i++)
        bard->songLyrics[i] = bard->newSongLyrics[i];

    bard->hasChangedSong = TRUE;
}

// Copies lyrics into gStringVar4
static void PrepareSongText(void)
{
    struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;
    u16 specialVar = gSpecialVar_0x8004;  // It's a bit odd to use this temp variable, but it seems needed to match.
    u16 *lyrics;
    u16 paragraphNum;
    u8 *wordEnd;
    u8 *str;

    lyrics = bard->newSongLyrics;
    if (specialVar == 0)
        lyrics = bard->songLyrics;
    wordEnd = gStringVar4;
    str = wordEnd;
    // Put three words on each line
    for (paragraphNum = 0; paragraphNum < 2; paragraphNum++)
    {
        wordEnd = CopyEasyChatWord(wordEnd, *(lyrics++));
        while (wordEnd != str)
        {
            if (*str == CHAR_SPACE)
                *str = CHAR_BARD_WORD_DELIMIT;
            str++;
        }

        str++;
        *(wordEnd++) = CHAR_SPACE;

        wordEnd = CopyEasyChatWord(wordEnd, *(lyrics++));
        while (wordEnd != str)
        {
            if (*str == CHAR_SPACE)
                *str = CHAR_BARD_WORD_DELIMIT;
            str++;
        }

        str++;
        *(wordEnd++) = CHAR_NEWLINE;

        wordEnd = CopyEasyChatWord(wordEnd, *(lyrics++));
        while (wordEnd != str)
        {
            if (*str == CHAR_SPACE)
                *str = CHAR_BARD_WORD_DELIMIT;
            str++;
        }

        if (paragraphNum == 0)
        {
            *(wordEnd++) = EXT_CTRL_CODE_BEGIN;
            *(wordEnd++) = 15;
        }
    }
}

void PlayBardSong(void)
{
    StartBardSong(gSpecialVar_0x8004);
    Menu_DisplayDialogueFrame();
    ScriptContext_Stop();
}

void HasHipsterTaughtWord(void)
{
    u16 *scriptResult = &gSpecialVar_Result; // again??
    struct MauvilleManHipster *hipster = &gSaveBlock1.oldMan.hipster;

    *scriptResult = hipster->taughtWord;
}

void SetHipsterTaughtWord(void)
{
    struct MauvilleManHipster *hipster = &gSaveBlock1.oldMan.hipster;

    hipster->taughtWord = TRUE;
}

void HipsterTryTeachWord(void)
{
    u16 word = UnlockRandomTrendySaying();

    if (word == 0xFFFF)
    {
        gSpecialVar_Result = FALSE;
    }
    else
    {
        CopyEasyChatWord(gStringVar1, word);
        gSpecialVar_Result = TRUE;
    }
}

void GiddyShouldTellAnotherTale(void)
{
    struct MauvilleManGiddy *giddy = &gSaveBlock1.oldMan.giddy;

    if (giddy->taleCounter == 10)
    {
        gSpecialVar_Result = FALSE;
        giddy->taleCounter = 0;
    }
    else
    {
        gSpecialVar_Result = TRUE;
    }
}

void GenerateGiddyLine(void)
{
    struct MauvilleManGiddy *giddy = &gSaveBlock1.oldMan.giddy;

    if (giddy->taleCounter == 0)
        InitGiddyTaleList();

    if (giddy->randomWords[giddy->taleCounter] != 0xFFFF) // is not the last element of the array?
    {
        u8 *stringPtr;
        u32 adjective = Random();

        adjective %= 8;
        stringPtr = CopyEasyChatWord(gStringVar4, giddy->randomWords[giddy->taleCounter]);
        stringPtr = StringCopy(stringPtr, GiddyText_Is);
        stringPtr = StringCopy(stringPtr, sGiddyAdjectives[adjective]);
        StringCopy(stringPtr, GiddyText_DontYouAgree);
    }
    else
    {
        StringCopy(gStringVar4, sGiddyQuestions[giddy->questionList[giddy->questionNum++]]);
    }

    if (!(Random() % 10))
        giddy->taleCounter = 10;
    else
        giddy->taleCounter++;

    gSpecialVar_Result = TRUE;
}

static void InitGiddyTaleList(void)
{
    struct MauvilleManGiddy *giddy = &gSaveBlock1.oldMan.giddy;
    u16 wordGroupsAndCount[][2] =
    {
        {EC_GROUP_POKEMON, 0},
        {EC_GROUP_LIFESTYLE, 0},
        {EC_GROUP_HOBBIES,   0},
        {EC_GROUP_MOVE_1,    0},
        {EC_GROUP_MOVE_2,    0},
        {EC_GROUP_POKEMON_NATIONAL, 0}
    };
    u16 i;
    u16 totalWords;
    u16 temp;
    u16 var;

    for (i = 0; i < 8; i++)
    {
        giddy->questionList[i] = i;
    }

    for (i = 0; i < 8; i++)
    {
        var = Random() % (i + 1);
        temp = giddy->questionList[i];
        giddy->questionList[i] = giddy->questionList[var];
        giddy->questionList[var] = temp;
    }

    totalWords = 0;
    for (i = 0; i < 6; i++)
    {
        wordGroupsAndCount[i][1] = sub_80EAE88(wordGroupsAndCount[i][0]);
        totalWords += wordGroupsAndCount[i][1];
    }

    giddy->questionNum = 0;
    temp = 0;
    for (i = 0; i < 10; i++)
    {
        var = Random() % 10;
        if (var < 3 && temp < 8)
        {
            giddy->randomWords[i] = 0xFFFF;
            temp++;
        }
        else
        {
            s16 randWord = Random() % totalWords;
            for (var = 0; i < 6; var++)
            {
                if ((randWord -= wordGroupsAndCount[var][1]) <= 0)
                    break;
            }
            if (var == 6)
                var = 0;
            giddy->randomWords[i] = sub_80EB784(wordGroupsAndCount[var][0]);
        }
    }
}

static void ResetBardFlag(void)
{
    struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;

    bard->hasChangedSong = FALSE;
}

static void ResetHipsterFlag(void)
{
    struct MauvilleManHipster *hipster = &gSaveBlock1.oldMan.hipster;

    hipster->taughtWord = FALSE;
}

static void ResetTraderFlag(void)
{
    Trader_ResetFlag();
}

static void ResetStorytellerFlag(void)
{
    Storyteller_ResetFlag();
}

void ResetMauvilleOldManFlag(void)
{
    switch (GetCurrentMauvilleOldMan())
    {
    case MAUVILLE_MAN_BARD:
        ResetBardFlag();
        break;
    case MAUVILLE_MAN_HIPSTER:
        ResetHipsterFlag();
        break;
    case MAUVILLE_MAN_STORYTELLER:
        ResetStorytellerFlag();
        break;
    case MAUVILLE_MAN_TRADER:
        ResetTraderFlag();
        break;
    case MAUVILLE_MAN_GIDDY:
        break;
    }
    SetMauvilleOldManObjEventGfx();
}

#define tState data[0]
#define tCharIndex data[3]
#define tLyricsIndex data[4]
#define tUseNewSongLyrics data[5]

static void StartBardSong(bool8 useNewSongLyrics)
{
    u8 taskId = CreateTask(Task_BardSong, 0x50);

    gTasks[taskId].tUseNewSongLyrics = useNewSongLyrics;
}

static void BardSing(struct Task *task, struct BardSong *song)
{
    switch (task->tState)
    {
    case 0:  // Initialize song
        {
            struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;
            u16 *lyrics;
            s32 i;

            // Copy lyrics
            if (gSpecialVar_0x8004 == 0)
                lyrics = bard->songLyrics;
            else
                lyrics = bard->newSongLyrics;
            for (i = 0; i < 6; i++)
                song->lyrics[i] = lyrics[i];

            // Clear phonemes
            for (i = 0; i < 6; i++)
            {
                song->phonemes[i].sound = 0xFFFF;
                song->phonemes[i].length = 0;
                song->phonemes[i].pitch = 0;
                song->phonemes[i].volume = 0;
            }
            song->lyricsIndex = 0;
            song->currPhoneme = 0;
            song->length = 0;
        }
        break;
    case 1:  // Wait for BGM to end
        break;
    case 2:  // Initialize word
        {
            u16 easyChatWord = song->lyrics[song->lyricsIndex];
            const struct BardSound *sounds = GetWordSounds(EC_GROUP(easyChatWord), EC_INDEX(easyChatWord));

            song->length = 0;
            GetWordPhonemes(song, sounds, WORD_TO_PITCH_TABLE_INDEX(easyChatWord));
        }
        break;
    case 3:
    case 4:
        {
            struct BardPhoneme *phoneme = &song->phonemes[song->currPhoneme];

            switch (song->state)
            {
            case 0:
                if (song->phonemeTimer == 0)  // Timer has expired. Move to next phoneme
                {
                    if (song->currPhoneme == 6 || phoneme->sound == 0xFF)
                    {
                        song->state = 0xFE;
                        break;
                    }
                    song->phonemeTimer = phoneme->length;
                    if (phoneme->sound <= 50)
                    {
                        u16 phonemeTripletId = phoneme->sound / 3;

                        m4aSongNumStart(249 + phonemeTripletId * 3);
                    }
                    song->state = 1;
                }
                else
                {
                    if (song->voiceInflection > 10)
                        song->volume -= 2;
                    if (song->voiceInflection & 1)
                        song->pitch += 64;
                    else
                        song->pitch -= 64;
                    m4aMPlayVolumeControl(&gMPlayInfo_SE2, 0xFFFF, song->volume);
                    m4aMPlayPitchControl(&gMPlayInfo_SE2, 0xFFFF, song->pitch);
                    song->voiceInflection++;
                }
                song->phonemeTimer--;
                break;
            case 1:
                song->currPhoneme++;
                song->state = 0;
                if (phoneme->sound <= 50)
                {
                    song->volume = 0x100 + phoneme->volume * 16;
                    m4aMPlayVolumeControl(&gMPlayInfo_SE2, 0xFFFF, song->volume);
                    song->pitch = 0x200 + phoneme->pitch;
                    m4aMPlayPitchControl(&gMPlayInfo_SE2, 0xFFFF, song->pitch);
                }
                break;
            case 0xFE:
                m4aMPlayStop(&gMPlayInfo_SE2);
                song->state = 0xFF;
                break;
            }
        }
        break;
    case 5:
        break;
    }
}

static void Task_BardSong(u8 taskId)
{
    struct Task *task = &gTasks[taskId];  // r5

    BardSing(task, &gBardSong);
    switch (task->tState)
    {
    case 0:  // Initialize song
        PrepareSongText();
        Text_InitWindowWithTemplate(gMenuWindowPtr, &gMenuTextWindowTemplate);
        Contest_StartTextPrinter(gMenuWindowPtr, gStringVar4, 2, 4, 15);
        task->data[1] = 0;
        task->data[2] = 0;
        task->tCharIndex = 0;
        task->tLyricsIndex = 0;
        FadeOutBGMTemporarily(4);
        task->tState = 1;
        break;
    case 1:  // Wait for BGM to end
        if (IsBGMPausedOrStopped())
            task->tState = 2;
        break;
    case 2:  // Initialize word
        {
            struct MauvilleManBard *bard = &gSaveBlock1.oldMan.bard;
            u8 *str = gStringVar4 + task->tCharIndex;
            u16 wordLen = 0;

            while (*str != CHAR_SPACE
                && *str != CHAR_NEWLINE
                && *str != EXT_CTRL_CODE_BEGIN
                && *str != EOS)
            {
                str++;
                wordLen++;
            }
            if (!task->tUseNewSongLyrics)
                sUnusedPitchTableIndex = WORD_TO_PITCH_TABLE_INDEX(bard->songLyrics[task->tLyricsIndex]);
            else
                sUnusedPitchTableIndex = WORD_TO_PITCH_TABLE_INDEX(bard->newSongLyrics[task->tLyricsIndex]);
            gBardSong.length /= wordLen;
            if (gBardSong.length <= 0)
                gBardSong.length = 1;
            task->tLyricsIndex++;
            if (task->data[2] == 0)
            {
                task->tState = 3;
                task->data[1] = 0;
            }
            else
            {
                task->tState = 5;
                task->data[1] = 0;
            }
        }
        break;
    case 5:
        if (task->data[2] == 0)
            task->tState = 3;
        else
            task->data[2]--;
        break;
    case 3:
        if (gStringVar4[task->tCharIndex] == EOS)
        {
            FadeInNewBGM(MUS_POKE_CENTER, 6);
            m4aMPlayFadeOutTemporarily(&gMPlayInfo_SE2, 2);
            ScriptContext_Enable();
            DestroyTask(taskId);
        }
        else if (gStringVar4[task->tCharIndex] == CHAR_SPACE)
        {
            Text_PrintWindowSimple(gMenuWindowPtr);
            task->tCharIndex++;
            task->tState = 2;
            task->data[2] = 0;
        }
        else if (gStringVar4[task->tCharIndex] == CHAR_NEWLINE)
        {
            task->tCharIndex++;
            task->tState = 2;
            task->data[2] = 0;
        }
        else if (gStringVar4[task->tCharIndex] == EXT_CTRL_CODE_BEGIN)
        {
            task->tCharIndex += 2;  // skip over control codes
            task->tState = 2;
            task->data[2] = 8;
        }
        else if (gStringVar4[task->tCharIndex] == CHAR_BARD_WORD_DELIMIT)
        {
            gStringVar4[task->tCharIndex] = CHAR_SPACE;  // restore it back to a space
            Text_PrintWindowSimple(gMenuWindowPtr);
            task->tCharIndex++;
            task->data[2] = 0;
        }
        else
        {
            switch (task->data[1])
            {
            case 0:
                Text_PrintWindowSimple(gMenuWindowPtr);
                task->data[1]++;
                break;
            case 1:
                task->data[1]++;
                break;
            case 2:
                task->tCharIndex++;
                task->data[1] = 0;
                task->data[2] = gBardSong.length;
                task->tState = 4;
                break;
            }
        }
        break;
    case 4:
        task->data[2]--;
        if (task->data[2] == 0)
            task->tState = 3;
        break;
    }
}

void SetMauvilleOldManObjEventGfx(void)
{
    VarSet(VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_BARD + GetCurrentMauvilleOldMan());
}

struct Story
{
    u8 stat;
    u8 minVal;
    const u8 *title;
    const u8 *action;
    const u8 *fullText;
};

static const struct Story sStorytellerStories[] =
{
    {0x32, 1, MauvilleCity_PokemonCenter_1F_Text_SavedGameTitle,  MauvilleCity_PokemonCenter_1F_Text_SavedGameAction,  MauvilleCity_PokemonCenter_1F_Text_SavedGameStory},
    {0x02, 1, MauvilleCity_PokemonCenter_1F_Text_TrendsStartedTitle,  MauvilleCity_PokemonCenter_1F_Text_TrendsStartedAction,  MauvilleCity_PokemonCenter_1F_Text_TrendsStartedStory},
    {0x03, 1, MauvilleCity_PokemonCenter_1F_Text_BerriesPlantedTitle,  MauvilleCity_PokemonCenter_1F_Text_BerriesPlantedAction,  MauvilleCity_PokemonCenter_1F_Text_BerriesPlantedStory},
    {0x04, 1, MauvilleCity_PokemonCenter_1F_Text_BikeTradesTitle,  MauvilleCity_PokemonCenter_1F_Text_BikeTradesAction,  MauvilleCity_PokemonCenter_1F_Text_BikeTradesStory},
    {0x06, 1, MauvilleCity_PokemonCenter_1F_Text_InterviewsTitle,  MauvilleCity_PokemonCenter_1F_Text_InterviewsAction,  MauvilleCity_PokemonCenter_1F_Text_InterviewsStory},
    {0x09, 1, MauvilleCity_PokemonCenter_1F_Text_TrainerBattlesTitle,  MauvilleCity_PokemonCenter_1F_Text_TrainerBattlesAction,  MauvilleCity_PokemonCenter_1F_Text_TrainerBattlesStory},
    {0x0B, 1, MauvilleCity_PokemonCenter_1F_Text_PokemonCaughtTitle,  MauvilleCity_PokemonCenter_1F_Text_PokemonCaughtAction,  MauvilleCity_PokemonCenter_1F_Text_PokemonCaughtStory},
    {0x0C, 1, MauvilleCity_PokemonCenter_1F_Text_FishingPokemonCaughtTitle,  MauvilleCity_PokemonCenter_1F_Text_FishingPokemonCaughtAction,  MauvilleCity_PokemonCenter_1F_Text_FishingPokemonCaughtStory},
    {0x0D, 1, MauvilleCity_PokemonCenter_1F_Text_EggsHatchedTitle,  MauvilleCity_PokemonCenter_1F_Text_EggsHatchedAction,  MauvilleCity_PokemonCenter_1F_Text_EggsHatchedStory},
    {0x0E, 1, MauvilleCity_PokemonCenter_1F_Text_PokemonEvolvedTitle, MauvilleCity_PokemonCenter_1F_Text_PokemonEvolvedAction, MauvilleCity_PokemonCenter_1F_Text_PokemonEvolvedStory},
    {0x0F, 1, MauvilleCity_PokemonCenter_1F_Text_UsedPokemonCenterTitle, MauvilleCity_PokemonCenter_1F_Text_UsedPokemonCenterAction, MauvilleCity_PokemonCenter_1F_Text_UsedPokemonCenterStory},
    {0x10, 1, MauvilleCity_PokemonCenter_1F_Text_RestedAtHomeTitle, MauvilleCity_PokemonCenter_1F_Text_RestedAtHomeAction, MauvilleCity_PokemonCenter_1F_Text_RestedAtHomeStory},
    {0x11, 1, MauvilleCity_PokemonCenter_1F_Text_SafariGamesTitle, MauvilleCity_PokemonCenter_1F_Text_SafariGamesAction, MauvilleCity_PokemonCenter_1F_Text_SafariGamesStory},
    {0x12, 1, MauvilleCity_PokemonCenter_1F_Text_UsedCutTitle, MauvilleCity_PokemonCenter_1F_Text_UsedCutAction, MauvilleCity_PokemonCenter_1F_Text_UsedCutStory},
    {0x13, 1, MauvilleCity_PokemonCenter_1F_Text_UsedRockSmashTitle, MauvilleCity_PokemonCenter_1F_Text_UsedRockSmashAction, MauvilleCity_PokemonCenter_1F_Text_UsedRockSmashStory},
    {0x14, 1, MauvilleCity_PokemonCenter_1F_Text_MovedBasesTitle, MauvilleCity_PokemonCenter_1F_Text_MovedBasesAction, MauvilleCity_PokemonCenter_1F_Text_MovedBasesStory},
    {0x1A, 1, MauvilleCity_PokemonCenter_1F_Text_UsedSplashTitle, MauvilleCity_PokemonCenter_1F_Text_UsedSplashAction, MauvilleCity_PokemonCenter_1F_Text_UsedSplashStory},
    {0x1B, 1, MauvilleCity_PokemonCenter_1F_Text_UsedStruggleTitle, MauvilleCity_PokemonCenter_1F_Text_UsedStruggleAction, MauvilleCity_PokemonCenter_1F_Text_UsedStruggleStory},
    {0x1C, 1, MauvilleCity_PokemonCenter_1F_Text_SlotJackpotsTitle, MauvilleCity_PokemonCenter_1F_Text_SlotJackpotsAction, MauvilleCity_PokemonCenter_1F_Text_SlotJackpotsStory},
    {0x1D, 2, MauvilleCity_PokemonCenter_1F_Text_RouletteWinsTitle, MauvilleCity_PokemonCenter_1F_Text_RouletteWinsAction, MauvilleCity_PokemonCenter_1F_Text_RouletteWinsStory},
    {0x1E, 1, MauvilleCity_PokemonCenter_1F_Text_BattleTowerChallengesTitle, MauvilleCity_PokemonCenter_1F_Text_BattleTowerChallengesAction, MauvilleCity_PokemonCenter_1F_Text_BattleTowerChallengesStory},
    {0x21, 1, MauvilleCity_PokemonCenter_1F_Text_MadePokeblocksTitle, MauvilleCity_PokemonCenter_1F_Text_MadePokeblocksAction, MauvilleCity_PokemonCenter_1F_Text_MadePokeblocksStory},
    {0x24, 1, MauvilleCity_PokemonCenter_1F_Text_EnteredContestsTitle, MauvilleCity_PokemonCenter_1F_Text_EnteredContestsAction, MauvilleCity_PokemonCenter_1F_Text_EnteredContestsStory},
    {0x25, 1, MauvilleCity_PokemonCenter_1F_Text_WonContestsTitle, MauvilleCity_PokemonCenter_1F_Text_WonContestsAction, MauvilleCity_PokemonCenter_1F_Text_WonContestsStory},
    {0x26, 1, MauvilleCity_PokemonCenter_1F_Text_TimesShoppedTitle, MauvilleCity_PokemonCenter_1F_Text_TimesShoppedAction, MauvilleCity_PokemonCenter_1F_Text_TimesShoppedStory},
    {0x27, 1, MauvilleCity_PokemonCenter_1F_Text_UsedItemFinderTitle, MauvilleCity_PokemonCenter_1F_Text_UsedItemFinderAction, MauvilleCity_PokemonCenter_1F_Text_UsedItemFinderStory},
    {0x28, 1, MauvilleCity_PokemonCenter_1F_Text_TimesRainedTitle, MauvilleCity_PokemonCenter_1F_Text_TimesRainedAction, MauvilleCity_PokemonCenter_1F_Text_TimesRainedStory},
    {0x29, 1, MauvilleCity_PokemonCenter_1F_Text_CheckedPokedexTitle, MauvilleCity_PokemonCenter_1F_Text_CheckedPokedexAction, MauvilleCity_PokemonCenter_1F_Text_CheckedPokedexStory},
    {0x2A, 1, MauvilleCity_PokemonCenter_1F_Text_ReceivedRibbonsTitle, MauvilleCity_PokemonCenter_1F_Text_ReceivedRibbonsAction, MauvilleCity_PokemonCenter_1F_Text_ReceivedRibbonsStory},
    {0x2B, 1, MauvilleCity_PokemonCenter_1F_Text_LedgesJumpedTitle, MauvilleCity_PokemonCenter_1F_Text_LedgesJumpedAction, MauvilleCity_PokemonCenter_1F_Text_LedgesJumpedStory},
    {0x2C, 1, MauvilleCity_PokemonCenter_1F_Text_TVWatchedTitle, MauvilleCity_PokemonCenter_1F_Text_TVWatchedAction, MauvilleCity_PokemonCenter_1F_Text_TVWatchedStory},
    {0x2D, 1, MauvilleCity_PokemonCenter_1F_Text_CheckedClockTitle, MauvilleCity_PokemonCenter_1F_Text_CheckedClockAction, MauvilleCity_PokemonCenter_1F_Text_CheckedClockStory},
    {0x2E, 1, MauvilleCity_PokemonCenter_1F_Text_WonLotteryTitle, MauvilleCity_PokemonCenter_1F_Text_WonLotteryAction, MauvilleCity_PokemonCenter_1F_Text_WonLotteryStory},
    {0x2F, 1, MauvilleCity_PokemonCenter_1F_Text_UsedDaycareTitle, MauvilleCity_PokemonCenter_1F_Text_UsedDaycareAction, MauvilleCity_PokemonCenter_1F_Text_UsedDaycareStory},
    {0x30, 1, MauvilleCity_PokemonCenter_1F_Text_RodeCableCarTitle, MauvilleCity_PokemonCenter_1F_Text_RodeCableCarAction, MauvilleCity_PokemonCenter_1F_Text_RodeCableCarStory},
    {0x31, 1, MauvilleCity_PokemonCenter_1F_Text_HotSpringsTitle, MauvilleCity_PokemonCenter_1F_Text_HotSpringsAction, MauvilleCity_PokemonCenter_1F_Text_HotSpringsStory},
};

static void StorytellerSetup(void)
{
    struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;
    s32 i;

    storyteller->id = MAUVILLE_MAN_STORYTELLER;
    storyteller->alreadyRecorded = FALSE;
    for (i = 0; i < 4; i++)
    {
        storyteller->gameStatIDs[i] = 0;
        storyteller->trainerNames[0][i] = EOS;  // Maybe they meant storyteller->trainerNames[i][0] instead?
    }
}

static void Storyteller_ResetFlag(void)
{
    struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;

    storyteller->id = MAUVILLE_MAN_STORYTELLER;
    storyteller->alreadyRecorded = FALSE;
}

static u32 StorytellerGetGameStat(u8 stat)
{
    if (stat == NUM_GAME_STATS)
        stat = 0;
    return GetGameStat(stat);
}

static const struct Story *GetStoryByStat(u32 stat)
{
    s32 i;

    for (i = 0; i < 36; i++)
    {
        if (sStorytellerStories[i].stat == stat)
            return &sStorytellerStories[i];
    }
    return &sStorytellerStories[35];
}

static const u8 *GetStoryTitleByStat(u32 stat)
{
    return GetStoryByStat(stat)->title;
}

static const u8 *GetStoryTextByStat(u32 stat)
{
    return GetStoryByStat(stat)->fullText;
}

static const u8 *GetStoryActionByStat(u32 stat)
{
    return GetStoryByStat(stat)->action;
}

static u8 GetFreeStorySlot(void)
{
    u8 i;

    for (i = 0; i < 4; i++)
    {
        struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;

        if (storyteller->gameStatIDs[i] == 0)
            break;
    }
    return i;
}

static u32 StorytellerGetRecordedTrainerStat(u32 trainer)
{
    u8 *ptr = gSaveBlock1.oldMan.storyteller.statValues[trainer];

    return ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
}

static void StorytellerSetRecordedTrainerStat(u32 trainer, u32 val)
{
    u8 *ptr = gSaveBlock1.oldMan.storyteller.statValues[trainer];

    ptr[0] = val;
    ptr[1] = val >> 8;
    ptr[2] = val >> 16;
    ptr[3] = val >> 24;
}

static bool32 HasTrainerStatIncreased(u32 trainer)
{
    struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;

    if (StorytellerGetGameStat(storyteller->gameStatIDs[trainer]) > StorytellerGetRecordedTrainerStat(trainer))
        return TRUE;
    else
        return FALSE;
}

static void GetStoryByStattellerPlayerName(u32 player, void *dst)
{
    u8 *name = gSaveBlock1.oldMan.storyteller.trainerNames[player];

    memset(dst, EOS, 8);
    memcpy(dst, name, 7);
}

static void StorytellerSetPlayerName(u32 player, const u8 *src)
{
    u8 *name = gSaveBlock1.oldMan.storyteller.trainerNames[player];
    u8 len = StringLength(src);

    memset(name, EOS, 7);
    StringCopyN(name, src, len);
}

static void StorytellerRecordNewStat(u32 player, u32 stat)
{
    struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;

    storyteller->gameStatIDs[player] = stat;
    StorytellerSetPlayerName(player, gSaveBlock2.playerName);
    StorytellerSetRecordedTrainerStat(player, StorytellerGetGameStat(stat));
    ConvertIntToDecimalStringN(gStringVar1, StorytellerGetGameStat(stat), 0, 10);
    StringCopy(gStringVar2, GetStoryActionByStat(stat));
}

static void ScrambleStatList(u8 *arr, s32 count)
{
    s32 i;

    for (i = 0; i < count; i++)
        arr[i] = i;
    for (i = 0; i < count; i++)
    {
        u32 a = Random() % count;
        u32 b = Random() % count;
        u8 temp = arr[a];
        arr[a] = arr[b];
        arr[b] = temp;
    }
}

// What purpose does this struct even serve? Only the length field is used.
static const struct {u32 length; struct MauvilleManStoryteller *unused1; u32 unused2;} sStorytellerStuff =
{
    36,
    &gSaveBlock1.oldMan.storyteller,  // unused
    12,                                    // unused
};

static bool8 StorytellerInitializeRandomStat(void)
{
    u8 storyIds[sStorytellerStuff.length];
    s32 i;
    s32 j;

    ScrambleStatList(storyIds, 36);
    for (i = 0; i < 36; i++)
    {
        u8 stat = sStorytellerStories[storyIds[i]].stat;
        u8 minVal = sStorytellerStories[storyIds[i]].minVal;
        struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;

        for (j = 0; j < 4; j++)
        {
            if (gSaveBlock1.oldMan.storyteller.gameStatIDs[j] == stat)
                break;
        }
        if (j == 4 && StorytellerGetGameStat(stat) >= minVal)
        {
            storyteller->alreadyRecorded = TRUE;
            StorytellerRecordNewStat(GetFreeStorySlot(), stat);
            return TRUE;
        }
    }
    return FALSE;
}

static void StorytellerDisplayStory(u32 player)
{
    struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;
    u8 stat = storyteller->gameStatIDs[player];

    ConvertIntToDecimalStringN(gStringVar1, StorytellerGetRecordedTrainerStat(player), 0, 10);
    StringCopy(gStringVar2, GetStoryActionByStat(stat));
    GetStoryByStattellerPlayerName(player, gStringVar3);
    ShowFieldMessage(GetStoryTextByStat(stat));
}

static void PrintStoryList(void)
{
    s32 i;

    Menu_DrawStdWindowFrame(0, 0, 25, 4 + GetFreeStorySlot() * 2);
    for (i = 0; i < 4; i++)
    {
        struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;
        u8 gameStatID = storyteller->gameStatIDs[i];

        if (gameStatID == 0)
            break;
        Menu_PrintText(GetStoryTitleByStat(gameStatID), 1, 2 + i * 2);
    }
    Menu_PrintText(gPCText_Cancel, 1, 2 + i * 2);
}

static u8 sSelectedStory;

static void Task_StoryListMenu(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s32 selection;

    switch (task->data[0])
    {
    case 0:
        PrintStoryList();
        InitMenu(0, 1, 2, GetFreeStorySlot() + 1, 0, 24);
        task->data[0]++;
        break;
    case 1:
        selection = Menu_ProcessInput();
        if (selection == -2)
            break;
        if (selection == -1 || selection == GetFreeStorySlot())
        {
            gSpecialVar_Result = 0;
        }
        else
        {
            gSpecialVar_Result = 1;
            sSelectedStory = selection;
        }
        Menu_DestroyCursor();
        Menu_EraseWindowRect(0, 0, 25, 12);
        DestroyTask(taskId);
        ScriptContext_Enable();
        break;
    }
}

// Sets gSpecialVar_Result to TRUE if player selected a story
void StorytellerStoryListMenu(void)
{
    CreateTask(Task_StoryListMenu, 0x50);
}

void Script_StorytellerDisplayStory(void)
{
    StorytellerDisplayStory(sSelectedStory);
}

u8 StorytellerGetFreeStorySlot(void)
{
    return GetFreeStorySlot();
}

// Returns TRUE if stat has increased
bool8 StorytellerUpdateStat(void)
{
    struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;
    u8 stat = storyteller->gameStatIDs[sSelectedStory];

    if (HasTrainerStatIncreased(sSelectedStory) == TRUE)
    {
        StorytellerRecordNewStat(sSelectedStory, stat);
        return TRUE;
    }
    return FALSE;
}

bool8 HasStorytellerAlreadyRecorded(void)
{
    struct MauvilleManStoryteller *storyteller = &gSaveBlock1.oldMan.storyteller;

    if (storyteller->alreadyRecorded == FALSE)
        return FALSE;
    else
        return TRUE;
}

bool8 Script_StorytellerInitializeRandomStat(void)
{
    return StorytellerInitializeRandomStat();
}
