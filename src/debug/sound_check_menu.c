#include "global.h"
#include "sprite.h"
#include "palette.h"
#include "task.h"
#include "m4a.h"
#include "main.h"
#include "text.h"
#include "menu.h"
#include "constants/songs.h"
#include "title_screen.h"
#include "sound.h"
#include "pokedex_cry_screen.h"

// local task defines
#define tWindowSelected data[0]
#define tBgmIndex data[1]
#define tSeIndex data[2]
#define tBgmIndexOld data[3]
#define tSeIndexOld data[4]

#define NUM_SOUND_CHECK_BGM 118
#define NUM_SOUND_CHECK_SE 248
#define NUM_SOUND_CHECK_CRY_SPECIES 384

// window selections
enum
{
    MUS_WINDOW,
    SE_WINDOW
};

// driver test cry enums
enum
{
    CRY_TEST_VOICE,
    CRY_TEST_VOLUME,
    CRY_TEST_PANPOT,
    CRY_TEST_PITCH,
    CRY_TEST_LENGTH,
    CRY_TEST_RELEASE,
    CRY_TEST_PROGRESS,
    CRY_TEST_CHORUS,
    CRY_TEST_PRIORITY
};

// minmax range enums
enum
{
    MIN,
    MAX
};

extern struct ToneData gCryTable[];
extern struct ToneData gCryTable_Reverse[];

static EWRAM_DATA u8 sIsFastForwarding = 0;
static EWRAM_DATA u8 sDriverTest_IsCryPlaying = 0;
static EWRAM_DATA u8 sDriverTest_IsCryPlayingOld = 0;
static EWRAM_DATA s8 sDriverTestSelection = 0;
static EWRAM_DATA int sSoundTestParams[9] = {0};
static EWRAM_DATA u8 sDriverTest_Reverse = 0;
static EWRAM_DATA u8 sDriverTest_Stereo = 0;

u16 gSoundTestCryNum;

struct MusicPlayerInfo *sMPlayInfo_Cry;

extern struct MusicPlayerInfo gMPlayInfo_BGM;

void Task_InitSoundCheckMenu(u8);
void Task_HandleDrawingSoundCheckMenuText(u8);
void Task_ProcessSoundCheckMenuInputAndRedraw(u8);
void Task_ExitToTitleScreen(u8);
void HighlightSelectedWindow(u8);
void PrintSoundNumber(u16, u16, u16);
void PrintPaddedString(const u8 *const, u16, u16);
void Task_DrawDriverTestMenu(u8);
void Task_ProcessDriverTestInput(u8);
void AdjustSelectedDriverParam(s8);
void PrintDriverTestMenuText(void);
void UpdateDriverTestCursor(u8, u8);
void PrintSignedNumber(int, u16, u16, u8);
void Task_DrawPanTestMenu(u8);
void Task_ProcessPanTestInput(u8);
void PrintPanTestMenuText(void);
void Task_InitCryTest(u8);
void Task_ProcessCryTestInput(u8);
void PrintCryNumber(void);

void CB2_SoundCheckMenu(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void VBlankCB_SoundCheckMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();

    if (sIsFastForwarding != 0)
    {
        m4aSoundMain();
        m4aSoundMain();
        m4aSoundMain();
    }
}

// Called by the debug start menu.
void CB2_StartSoundCheckMenu(void)
{
    u8 taskId;

    SetVBlankCallback(NULL);
    REG_DISPCNT = 0;
    REG_BG2CNT = 0;
    REG_BG1CNT = 0;
    REG_BG0CNT = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, 0, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
    ResetPaletteFade();
    ResetTasks();
    ResetSpriteData();
    Text_LoadWindowTemplate(&gMenuTextWindowLoadTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    REG_WIN0H = WIN_RANGE(0, 0);
    REG_WIN0V = WIN_RANGE(0, 0);
    REG_WIN1H = WIN_RANGE(0, 0);
    REG_WIN1V = WIN_RANGE(0, 0);
    REG_WININ = 0x1111;
    REG_WINOUT = 0x31;
    REG_BLDCNT = 0xE1;
    REG_BLDALPHA = 0;
    REG_BLDY = 7;
    REG_IE = 1; // could be a typo of REG_IME
    REG_IE |= 1;
    REG_DISPSTAT |= 8;
    SetVBlankCallback(VBlankCB_SoundCheckMenu);
    SetMainCallback2(CB2_SoundCheckMenu);
    REG_DISPCNT = 0x7140;
    taskId = CreateTask(Task_InitSoundCheckMenu, 0);
    gTasks[taskId].tWindowSelected = MUS_WINDOW;
    gTasks[taskId].tBgmIndex = 0;
    gTasks[taskId].tSeIndex = 0;
    gTasks[taskId].tBgmIndexOld = 0;
    sIsFastForwarding = 0;
    gTasks[taskId].tBgmIndexOld = 0; // Preserved duplicate initialization.
    m4aSoundInit();
}

void Task_InitSoundCheckMenu(u8 taskId)
{
    u8 soundcheckStr[] = DTR("サウンドチェック", "SOUND CHECK");
    u8 bgmStr[] = _("BGM");
    u8 seStr[] = _("SE ");
    u8 abDescStr[] = DTR("A‥さいせい　B‥おわり", "A PLAY  B STOP");
    u8 upDownStr[] = _("L‥UP R‥DOWN");
    u8 driverStr[] = _("R‥DRIVER-TEST");

    if (!gPaletteFade.active)
    {
        Menu_DrawStdWindowFrame(2, 0, 27, 3);
        Menu_DrawStdWindowFrame(2, 5, 27, 10);
        Menu_DrawStdWindowFrame(2, 12, 27, 17);
        Menu_PrintText(soundcheckStr, 4, 1);
        Menu_PrintText(abDescStr, 14, 1);
        Menu_PrintText(bgmStr, 4, 6);
        Menu_PrintText(upDownStr, 14, 6);
        Menu_PrintText(seStr, 4, 13);
        Menu_PrintText(upDownStr, 14, 13);
        Menu_PrintText(driverStr, 14, 18);
        gTasks[taskId].func = Task_HandleDrawingSoundCheckMenuText;
        REG_WIN0H = WIN_RANGE(17, 223);
        REG_WIN0V = WIN_RANGE(1, 31);
    }
}

// ideally this should be a multi Coords8 struct, but it wont match when its treated like a struct.
static const u8 sDriverTestCursorCoords[] = { 1, 1, 1, 3, 1, 5, 1, 7, 1, 9, 1, 11, 1, 13, 1, 15, 1, 17 };

static const u8 *const gBGMNames[];
static const u8 *const gSENames[];

void Task_HandleDrawingSoundCheckMenuText(u8 taskId)
{
    HighlightSelectedWindow(gTasks[taskId].tWindowSelected);
    PrintSoundNumber(gTasks[taskId].tBgmIndex + MUS_STOP, 7, 8); // print by BGM index
    PrintPaddedString(gBGMNames[gTasks[taskId].tBgmIndex], 11, 8);
    PrintSoundNumber(gTasks[taskId].tSeIndex, 7, 15);
    PrintPaddedString(gSENames[gTasks[taskId].tSeIndex], 11, 15);
    gTasks[taskId].func = Task_ProcessSoundCheckMenuInputAndRedraw;
}

bool8 Task_ProcessSoundCheckMenuInput(u8 taskId)
{
    if (JOY_NEW(R_BUTTON)) // driver test
    {
        gTasks[taskId].func = Task_DrawDriverTestMenu;
    }
    else if (JOY_NEW(L_BUTTON))
    {
        gTasks[taskId].func = Task_DrawPanTestMenu;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        gTasks[taskId].func = Task_InitCryTest;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tWindowSelected != MUS_WINDOW)
        {
            if (gTasks[taskId].tSeIndexOld != 0)
            {
                if (gTasks[taskId].tSeIndex != 0)
                {
                    m4aSongNumStop(gTasks[taskId].tSeIndexOld);
                    m4aSongNumStart(gTasks[taskId].tSeIndex);
                    gTasks[taskId].tSeIndexOld = gTasks[taskId].tSeIndex;
                }
                else
                {
                    m4aSongNumStop(gTasks[taskId].tSeIndexOld);
                    gTasks[taskId].tSeIndexOld = 0;
                }
            }
            else if (gTasks[taskId].tSeIndex != 0)
            {
                m4aSongNumStart(gTasks[taskId].tSeIndex);
                gTasks[taskId].tSeIndexOld = gTasks[taskId].tSeIndex;
            }
        }
        else
        {
            if (gTasks[taskId].tBgmIndexOld != 0)
            {
                if (gTasks[taskId].tBgmIndex != 0)
                {
                    m4aSongNumStop(gTasks[taskId].tBgmIndexOld + MUS_STOP);
                    m4aSongNumStart(gTasks[taskId].tBgmIndex + MUS_STOP);
                    gTasks[taskId].tBgmIndexOld = gTasks[taskId].tBgmIndex;
                }
                else
                {
                    m4aSongNumStop(gTasks[taskId].tBgmIndexOld + MUS_STOP);
                    gTasks[taskId].tBgmIndexOld = 0;
                }
            }
            else if (gTasks[taskId].tBgmIndex != 0)
            {
                m4aSongNumStart(gTasks[taskId].tBgmIndex + MUS_STOP);
                gTasks[taskId].tBgmIndexOld = gTasks[taskId].tBgmIndex;
            }
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        m4aSongNumStart(SE_SELECT);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_ExitToTitleScreen;
    }
    else if (JOY_REPT(DPAD_UP))
    {
        gTasks[taskId].tWindowSelected ^= 1;
        return TRUE;
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        gTasks[taskId].tWindowSelected ^= 1;
        return TRUE;
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        if (gTasks[taskId].tWindowSelected != MUS_WINDOW)
        {
            if (gTasks[taskId].tSeIndex > 0)
                gTasks[taskId].tSeIndex--;
            else
                gTasks[taskId].tSeIndex = NUM_SOUND_CHECK_SE - 1;
        }
        else
        {
            if (gTasks[taskId].tBgmIndex > 0)
                gTasks[taskId].tBgmIndex--;
            else
                gTasks[taskId].tBgmIndex = NUM_SOUND_CHECK_BGM - 1;
        }
        return TRUE;
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        if (gTasks[taskId].tWindowSelected != MUS_WINDOW)
        {
            if (gTasks[taskId].tSeIndex < NUM_SOUND_CHECK_SE - 1)
                gTasks[taskId].tSeIndex++;
            else
                gTasks[taskId].tSeIndex = 0;
        }
        else
        {
            if (gTasks[taskId].tBgmIndex < NUM_SOUND_CHECK_BGM - 1)
                gTasks[taskId].tBgmIndex++;
            else
                gTasks[taskId].tBgmIndex = 0;
        }
        return TRUE;
    }
    else if (JOY_HELD(SELECT_BUTTON))
    {
        sIsFastForwarding = 1;
    }
    else
    {
        sIsFastForwarding = 0;
    }
    return FALSE;
}

void Task_ProcessSoundCheckMenuInputAndRedraw(u8 taskId)
{
    if (Task_ProcessSoundCheckMenuInput(taskId) != FALSE)
        gTasks[taskId].func = Task_HandleDrawingSoundCheckMenuText;
}

void Task_ExitToTitleScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2(CB2_InitTitleScreen);
    }
}

void HighlightSelectedWindow(u8 windowType)
{
    switch (windowType)
    {
    case MUS_WINDOW:
    default:
        REG_WIN1H = WIN_RANGE(17, 223);
        REG_WIN1V = WIN_RANGE(41, 87);
        break;
    case SE_WINDOW:
        REG_WIN1H = WIN_RANGE(17, 223);
        REG_WIN1V = WIN_RANGE(97, 143);
        break;
    }
}

void PrintSoundNumber(u16 soundIndex, u16 x, u16 y)
{
    u8 i;
    u8 str[5];
    bool8 hasHundredsDigit;
    u8 divisorValue;

    for (i = 0; i < 3; i++)
        str[i] = 0; // initialize array

    str[3] = CHAR_ELLIPSIS;
    str[4] = EOS;
    hasHundredsDigit = FALSE;

    divisorValue = soundIndex / 100;
    if (divisorValue)
    {
        str[0] = divisorValue + CHAR_0;
        hasHundredsDigit = TRUE;
    }

    divisorValue = (soundIndex % 100) / 10;
    if (divisorValue || hasHundredsDigit)
        str[1] = divisorValue + CHAR_0;

    str[2] = ((soundIndex % 100) % 10) + CHAR_0;
    Menu_PrintText(str, x, y);
}

void PrintPaddedString(const u8 *const string, u16 x, u16 y)
{
    u8 i;
    u8 str[11];

    for (i = 0; i <= 10; i++)
        str[i] = 0;

    str[10] = EOS;

    for (i = 0; string[i] != EOS && i < 10; i++)
        str[i] = string[i];

    Menu_PrintText(str, x, y);
}

void Task_DrawDriverTestMenu(u8 taskId) // Task_DrawDriverTestMenu
{
    u8 bbackStr[] = DTR("Bぼたんで　もどる", "B BUTTON: BACK");
    u8 aplayStr[] = DTR("Aぼたんで　さいせい", "A BUTTON: PLAY");
    u8 voiceStr[] = _("VOICE‥‥‥‥");
    u8 volumeStr[] = _("VOLUME‥‥‥");
    u8 panpotStr[] = _("PANPOT‥‥‥");
    u8 pitchStr[] = _("PITCH‥‥‥‥");
    u8 lengthStr[] = _("LENGTH‥‥‥");
    u8 releaseStr[] = _("RELEASE‥‥");
    u8 progressStr[] = _("PROGRESS‥");
    u8 chorusStr[] = _("CHORUS‥‥‥");
    u8 priorityStr[] = _("PRIORITY‥");
    u8 playingStr[] = DTR("さいせいちゆう‥", "PLAYING"); // 再生中 (playing)
    u8 reverseStr[] = DTR("はんてん‥‥‥‥", "REVERSE"); // 反転 (reverse)
    u8 stereoStr[] = DTR("すてれお‥‥‥‥", "STEREO"); // stereo

    REG_DISPCNT = 0x3140;
    Menu_DrawStdWindowFrame(0, 0, 29, 19);
    Menu_PrintText(bbackStr, 19, 4);
    Menu_PrintText(aplayStr, 19, 2);
    Menu_PrintText(voiceStr, 2, 1);
    Menu_PrintText(volumeStr, 2, 3);
    Menu_PrintText(panpotStr, 2, 5);
    Menu_PrintText(pitchStr, 2, 7);
    Menu_PrintText(lengthStr, 2, 9);
    Menu_PrintText(releaseStr, 2, 11);
    Menu_PrintText(progressStr, 2, 13);
    Menu_PrintText(chorusStr, 2, 15);
    Menu_PrintText(priorityStr, 2, 17);
    Menu_PrintText(playingStr, 19, 16);
    Menu_PrintText(reverseStr, 19, 14);
    Menu_PrintText(stereoStr, 19, 12);
    REG_WIN0H = WIN_RANGE(0, DISPLAY_WIDTH);
    REG_WIN0V = WIN_RANGE(0, DISPLAY_HEIGHT);
    sDriverTestSelection = 0;
    sDriverTest_IsCryPlaying = 0;
    sDriverTest_IsCryPlayingOld = 0;
    sMPlayInfo_Cry = NULL;
    sDriverTest_Reverse = 0;
    sDriverTest_Stereo = 1;
    sSoundTestParams[CRY_TEST_VOICE] = 0;
    sSoundTestParams[CRY_TEST_VOLUME] = 120;
    sSoundTestParams[CRY_TEST_PANPOT] = 0;
    sSoundTestParams[CRY_TEST_PITCH] = 15360;
    sSoundTestParams[CRY_TEST_LENGTH] = 180;
    sSoundTestParams[CRY_TEST_PROGRESS] = 0;
    sSoundTestParams[CRY_TEST_RELEASE] = 0;
    sSoundTestParams[CRY_TEST_CHORUS] = 0;
    sSoundTestParams[CRY_TEST_PRIORITY] = 2;
    PrintDriverTestMenuText();
    UpdateDriverTestCursor(0, 0);
    gTasks[taskId].func = Task_ProcessDriverTestInput;
}

void Task_ProcessDriverTestInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        REG_DISPCNT = 0x7140;
        REG_WIN0H = WIN_RANGE(17, 223);
        REG_WIN0V = WIN_RANGE(1, 31);
        Menu_EraseWindowRect(0, 0, 29, 19);
        gTasks[taskId].func = Task_InitSoundCheckMenu;
        return;
    }
    if (JOY_REPT(DPAD_UP))
    {
        u8 oldSelection = sDriverTestSelection;

        if(--sDriverTestSelection < 0)
            sDriverTestSelection = 8;
        UpdateDriverTestCursor(oldSelection, sDriverTestSelection);
        return;
    }
    if (JOY_REPT(DPAD_DOWN))
    {
        u8 oldSelection = sDriverTestSelection;

        if(++sDriverTestSelection > 8)
            sDriverTestSelection = 0;
        UpdateDriverTestCursor(oldSelection, sDriverTestSelection);
        return;
    }
    if (JOY_NEW(START_BUTTON))
    {
        sDriverTest_Reverse ^= 1;
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_NEW(SELECT_BUTTON))
    {
        sDriverTest_Stereo ^= 1;
        PrintDriverTestMenuText();
        SetPokemonCryStereo(sDriverTest_Stereo);
        return;
    }
    if (JOY_REPT(R_BUTTON))
    {
        AdjustSelectedDriverParam(10);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_REPT(L_BUTTON))
    {
        AdjustSelectedDriverParam(-10);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_REPT(DPAD_LEFT))
    {
        AdjustSelectedDriverParam(-1);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_REPT(DPAD_RIGHT))
    {
        AdjustSelectedDriverParam(1);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        u8 toneTableIndex, toneIndex;

        SetPokemonCryVolume(sSoundTestParams[CRY_TEST_VOLUME]);
        SetPokemonCryPanpot(sSoundTestParams[CRY_TEST_PANPOT]);
        SetPokemonCryPitch(sSoundTestParams[CRY_TEST_PITCH]);
        SetPokemonCryLength(sSoundTestParams[CRY_TEST_LENGTH]);
        SetPokemonCryProgress(sSoundTestParams[CRY_TEST_PROGRESS]);
        SetPokemonCryRelease(sSoundTestParams[CRY_TEST_RELEASE]);
        SetPokemonCryChorus(sSoundTestParams[CRY_TEST_CHORUS]);
        SetPokemonCryPriority(sSoundTestParams[CRY_TEST_PRIORITY]);

        toneIndex = sSoundTestParams[CRY_TEST_VOICE] % 128;
        toneTableIndex = sSoundTestParams[CRY_TEST_VOICE] / 128;

        switch (toneTableIndex)
        {
        case 0:
            if (sDriverTest_Reverse)
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 0) + toneIndex]);
            else
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 0) + toneIndex]);
            break;
        case 1:
            if (sDriverTest_Reverse)
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 1) + toneIndex]);
            else
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 1) + toneIndex]);
            break;
        case 2:
            if (sDriverTest_Reverse)
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 2) + toneIndex]);
            else
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 2) + toneIndex]);
            break;
        case 3:
            if (sDriverTest_Reverse)
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 3) + toneIndex]);
            else
                sMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 3) + toneIndex]);
            break;
        }
    }

    if (sMPlayInfo_Cry != NULL)
    {
        sDriverTest_IsCryPlaying = IsPokemonCryPlaying(sMPlayInfo_Cry);

        if (sDriverTest_IsCryPlaying != sDriverTest_IsCryPlayingOld)
            PrintDriverTestMenuText();

        sDriverTest_IsCryPlayingOld = sDriverTest_IsCryPlaying;
    }
}

void AdjustSelectedDriverParam(s8 delta)
{
    // also ideally should be a MinMax struct, but any attempt to make this into a struct causes it to not match due to the weird multi dim access.
    int paramRanges[] =
    {
        0, 387,         // Voice
        0, 127,         // Volume
        -127, 127,      // Panpot
        -128, 32639,    // Pitch
        0, 65535,       // Length
        0, 255,         // Release
        0, 65535,       // Progress
        -64, 63         // Chorus
                        // Priority has no range entry in the original table.
    };

    sSoundTestParams[sDriverTestSelection] += delta;

    if (sSoundTestParams[sDriverTestSelection] > paramRanges[MULTI_DIM_ARR(sDriverTestSelection, B_16, MAX)])
        sSoundTestParams[sDriverTestSelection] = paramRanges[MULTI_DIM_ARR(sDriverTestSelection, B_16, MIN)];

    if (sSoundTestParams[sDriverTestSelection] < paramRanges[MULTI_DIM_ARR(sDriverTestSelection, B_16, MIN)])
        sSoundTestParams[sDriverTestSelection] = paramRanges[MULTI_DIM_ARR(sDriverTestSelection, B_16, MAX)];
}

void PrintDriverTestMenuText(void)
{
    PrintSignedNumber(sSoundTestParams[CRY_TEST_VOICE] + 1, 11, 1, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_VOLUME], 11, 3, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PANPOT], 11, 5, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PITCH], 11, 7, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_LENGTH], 11, 9, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_RELEASE], 11, 11, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PROGRESS], 11, 13, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_CHORUS], 11, 15, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PRIORITY], 11, 17, 5);
    PrintSignedNumber(sDriverTest_IsCryPlaying, 27, 16, 1);
    PrintSignedNumber(sDriverTest_Reverse, 27, 14, 1);
    PrintSignedNumber(sDriverTest_Stereo, 27, 12, 1);
}

void UpdateDriverTestCursor(u8 oldSelection, u8 newSelection)
{
    u8 cursorStr[] = _("▶");
    u8 blankStr[] = _(" ");

    Menu_PrintText(blankStr, sDriverTestCursorCoords[MULTI_DIM_ARR(oldSelection, B_16, 0)], sDriverTestCursorCoords[MULTI_DIM_ARR(oldSelection, B_16, 1)]);
    Menu_PrintText(cursorStr, sDriverTestCursorCoords[MULTI_DIM_ARR(newSelection, B_16, 0)], sDriverTestCursorCoords[MULTI_DIM_ARR(newSelection, B_16, 1)]);
}

void PrintSignedNumber(int number, u16 x, u16 y, u8 digits)
{
    int powersOfTen[6] =
    {
              1,
             10,
            100,
           1000,
          10000,
         100000
    };
    u8 str[8];
    s8 i;
    s8 negative;
    s8 hasPrintedDigit;

    for (i = 0; i <= digits; i++)
        str[i] = CHAR_SPACE;
    str[digits + 1] = EOS;

    negative = FALSE;
    if (number < 0)
    {
        number = -number;
        negative = TRUE;
    }

    if (digits == 1)
        hasPrintedDigit = TRUE;
    else
        hasPrintedDigit = FALSE;

    for (i = digits - 1; i >= 0; i--)
    {
        s8 digit = number / powersOfTen[i];

        if (digit != 0 || hasPrintedDigit || i == 0)
        {
            if (negative && !hasPrintedDigit)
                str[digits - i - 1] = CHAR_HYPHEN;
            str[digits - i] = CHAR_0 + digit;
            hasPrintedDigit = TRUE;
        }
        number %= powersOfTen[i];
    }

    Menu_PrintText(str, x, y);
}

static const s8 sPanTestPanpots[5] = { 0x3F, 0x00, 0xC0, 0x7F, 0x80 };

void Task_DrawPanTestMenu(u8 taskId)
{
    u8 seStr[] = _("SE");
    u8 panStr[] = _("PAN");
    u8 playingStr[] = DTR("さいせいちゆう‥", "PLAYING");

    REG_DISPCNT = 0x3140;
    Menu_DrawStdWindowFrame(0, 0, 29, 19);
    Menu_PrintText(seStr, 3, 2);
    Menu_PrintText(panStr, 3, 4);
    Menu_PrintText(playingStr, 3, 8);
    REG_WIN0H = WIN_RANGE(0, DISPLAY_WIDTH);
    REG_WIN0V = WIN_RANGE(0, DISPLAY_HEIGHT);
    sSoundTestParams[CRY_TEST_VOICE] = 1;
    sSoundTestParams[CRY_TEST_PANPOT] = 0;
    sSoundTestParams[CRY_TEST_CHORUS] = 0;
    sSoundTestParams[CRY_TEST_PROGRESS] = 0;
    sSoundTestParams[CRY_TEST_RELEASE] = 0;
    PrintPanTestMenuText();
    gTasks[taskId].func = Task_ProcessPanTestInput;
}

void Task_ProcessPanTestInput(u8 taskId)
{
    PrintPanTestMenuText();
    if (sSoundTestParams[CRY_TEST_PROGRESS])
    {
        if (sSoundTestParams[CRY_TEST_RELEASE])
        {
            sSoundTestParams[CRY_TEST_RELEASE]--;
        }
        else
        {
            s8 panpot = sPanTestPanpots[sSoundTestParams[CRY_TEST_PANPOT]];
            if (panpot != -128)
            {
                if (panpot == 127)
                {
                    sSoundTestParams[CRY_TEST_CHORUS] += 2;
                    if (sSoundTestParams[CRY_TEST_CHORUS] < 63)
                        SE12PanpotControl(sSoundTestParams[CRY_TEST_CHORUS]);
                }
            }
            else
            {
                sSoundTestParams[CRY_TEST_CHORUS] -= 2;
                if (sSoundTestParams[CRY_TEST_CHORUS] > -64)
                    SE12PanpotControl(sSoundTestParams[CRY_TEST_CHORUS]);
            }
        }
    }
    if (JOY_NEW(B_BUTTON))
    {
        REG_DISPCNT = 0x7140;
        REG_WIN0H = WIN_RANGE(17, 223);
        REG_WIN0V = WIN_RANGE(1, 31);
        Menu_EraseWindowRect(0, 0, 29, 19);
        gTasks[taskId].func = Task_InitSoundCheckMenu;
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        s8 panpot = sPanTestPanpots[sSoundTestParams[CRY_TEST_PANPOT]];
        if (panpot != -128)
        {
            if (panpot == 127)
            {
                PlaySE12WithPanning(sSoundTestParams[CRY_TEST_VOICE], -64);
                sSoundTestParams[CRY_TEST_CHORUS] = -64;
                sSoundTestParams[CRY_TEST_PROGRESS] = 1;
                sSoundTestParams[CRY_TEST_RELEASE] = 30;
                return;
            }
        }
        else
        {
            PlaySE12WithPanning(sSoundTestParams[CRY_TEST_VOICE], 63);
            sSoundTestParams[CRY_TEST_CHORUS] = 63;
            sSoundTestParams[CRY_TEST_PROGRESS] = 1;
            sSoundTestParams[CRY_TEST_RELEASE] = 30;
            return;
        }
        PlaySE12WithPanning(sSoundTestParams[CRY_TEST_VOICE], panpot);
        sSoundTestParams[CRY_TEST_PROGRESS] = 0;
        return;
    }
    if (JOY_NEW(L_BUTTON))
    {
        sSoundTestParams[CRY_TEST_PANPOT]++;
        if (sSoundTestParams[CRY_TEST_PANPOT] > 4)
            sSoundTestParams[CRY_TEST_PANPOT] = 0;
    }
    if (JOY_NEW(R_BUTTON))
    {
        sSoundTestParams[CRY_TEST_PANPOT]--;
        if (sSoundTestParams[CRY_TEST_PANPOT] < 0)
            sSoundTestParams[CRY_TEST_PANPOT] = 4;
    }
    if (JOY_REPT(DPAD_RIGHT))
    {
        sSoundTestParams[CRY_TEST_VOICE]++;
        if (sSoundTestParams[CRY_TEST_VOICE] > NUM_SOUND_CHECK_SE - 1)
            sSoundTestParams[CRY_TEST_VOICE] = 0;
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        sSoundTestParams[CRY_TEST_VOICE]--;
        if (sSoundTestParams[CRY_TEST_VOICE] < 0)
            sSoundTestParams[CRY_TEST_VOICE] = NUM_SOUND_CHECK_SE - 1;
    }
}

void PrintPanTestMenuText(void)
{
    u8 lrStr[] = _("  LR");
    u8 rlStr[] = _("  RL");

    PrintSignedNumber(sSoundTestParams[CRY_TEST_VOICE], 7, 2, 3);

    switch (sPanTestPanpots[sSoundTestParams[CRY_TEST_PANPOT]])
    {
    case 127:
        Menu_PrintText(lrStr, 7, 4);
        break;
    case -128:
        Menu_PrintText(rlStr, 7, 4);
        break;
    default:
        PrintSignedNumber(sPanTestPanpots[sSoundTestParams[CRY_TEST_PANPOT]], 7, 4, 3);
        break;
    }
    PrintSignedNumber(IsSEPlaying(), 12, 8, 1);
}

#define SOUND_LIST_BGM \
	X(MUS_STOP, "STOP") \
	X(MUS_LITTLEROOT_TEST, "TETSUJI") \
	X(MUS_GSC_ROUTE38, "FIELD13") \
	X(MUS_CAUGHT, "KACHI22") \
	X(MUS_VICTORY_WILD, "KACHI2") \
	X(MUS_VICTORY_GYM_LEADER, "KACHI3") \
	X(MUS_VICTORY_LEAGUE, "KACHI5") \
	X(MUS_C_COMM_CENTER, "PCC") \
	X(MUS_GSC_PEWTER, "NIBI") \
	X(MUS_C_VS_LEGEND_BEAST, "SUIKUN") \
	X(MUS_ROUTE101, "DOORO1") \
	X(MUS_ROUTE110, "DOORO-X1") \
	X(MUS_ROUTE120, "DOORO-X3") \
	X(MUS_PETALBURG, "MACHI-S2") \
	X(MUS_OLDALE, "MACHI-S4") \
	X(MUS_GYM, "GIM") \
	X(MUS_SURF, "NAMINORI") \
	X(MUS_PETALBURG_WOODS, "DAN01") \
	X(MUS_LEVEL_UP, "FANFA1") \
	X(MUS_HEAL, "ME-ASA") \
	X(MUS_OBTAIN_BADGE, "ME-BACHI") \
	X(MUS_OBTAIN_ITEM, "FANFA4") \
	X(MUS_EVOLVED, "FANFA5") \
	X(MUS_OBTAIN_TMHM, "ME-WAZA") \
	X(MUS_LILYCOVE_MUSEUM, "BIJYUTU") \
	X(MUS_ROUTE122, "DOORO-X4") \
	X(MUS_OCEANIC_MUSEUM, "FUNE-KAN") \
	X(MUS_EVOLUTION_INTRO, "ME-SHINKA") \
	X(MUS_EVOLUTION, "SHINKA") \
	X(MUS_MOVE_DELETED, "ME-WASURE") \
	X(MUS_ENCOUNTER_GIRL, "SYOUJOEYE") \
	X(MUS_ENCOUNTER_MALE, "BOYEYE") \
	X(MUS_ABANDONED_SHIP, "DAN02") \
	X(MUS_FORTREE, "MACHI-S3") \
	X(MUS_BIRCH_LAB, "ODAMAKI") \
	X(MUS_B_TOWER_RS, "B-TOWER") \
	X(MUS_ENCOUNTER_SWIMMER, "SWIMEYE") \
	X(MUS_CAVE_OF_ORIGIN, "DAN03") \
	X(MUS_OBTAIN_BERRY, "ME-KINOMI") \
	X(MUS_AWAKEN_LEGEND, "ME-TAMA") \
	X(MUS_SLOTS_JACKPOT, "ME-B-BIG") \
	X(MUS_SLOTS_WIN, "ME-B-SMALL") \
	X(MUS_TOO_BAD, "ME-ZANNEN") \
	X(MUS_ROULETTE, "BD-TIME") \
	X(MUS_LINK_CONTEST_P1, "TEST1") \
	X(MUS_LINK_CONTEST_P2, "TEST2") \
	X(MUS_LINK_CONTEST_P3, "TEST3") \
	X(MUS_LINK_CONTEST_P4, "TEST4") \
	X(MUS_ENCOUNTER_RICH, "TEST") \
	X(MUS_VERDANTURF, "GOMACHI0") \
	X(MUS_RUSTBORO, "GOTOWN") \
	X(MUS_POKE_CENTER, "POKECEN") \
	X(MUS_ROUTE104, "NEXTROAD") \
	X(MUS_ROUTE119, "GRANROAD") \
	X(MUS_CYCLING, "CYCLING") \
	X(MUS_POKE_MART, "FRIENDLY") \
	X(MUS_LITTLEROOT, "MISHIRO") \
	X(MUS_MT_CHIMNEY, "TOZAN") \
	X(MUS_ENCOUNTER_FEMALE, "GIRLEYE") \
	X(MUS_LILYCOVE, "MINAMO") \
	X(MUS_DESERT, "ASHROAD") \
	X(MUS_HELP, "EVENT0") \
	X(MUS_UNDERWATER, "DEEPDEEP") \
	X(MUS_VICTORY_TRAINER, "KACHI1") \
	X(MUS_TITLE, "TITLE3") \
	X(MUS_INTRO, "DEMO1") \
	X(MUS_ENCOUNTER_MAY, "GIRL-SUP") \
	X(MUS_ENCOUNTER_INTENSE, "HAGESHII") \
	X(MUS_ENCOUNTER_COOL, "KAKKOII") \
	X(MUS_ROUTE113, "KAZANBAI") \
	X(MUS_ENCOUNTER_AQUA, "AQA-0") \
	X(MUS_FOLLOW_ME, "TSURETEK") \
	X(MUS_ENCOUNTER_BRENDAN, "BOY-SUP") \
	X(MUS_EVER_GRANDE, "RAINBOW") \
	X(MUS_ENCOUNTER_SUSPICIOUS, "AYASII") \
	X(MUS_VICTORY_AQUA_MAGMA, "KACHI4") \
	X(MUS_CABLE_CAR, "ROPEWAY") \
	X(MUS_GAME_CORNER, "CASINO") \
	X(MUS_DEWFORD, "HIGHTOWN") \
	X(MUS_SAFARI_ZONE, "SAFARI") \
	X(MUS_VICTORY_ROAD, "C-ROAD") \
	X(MUS_AQUA_MAGMA_HIDEOUT, "AJITO") \
	X(MUS_SAILING, "M-BOAT") \
	X(MUS_MT_PYRE, "M-DUNGON") \
	X(MUS_SLATEPORT, "FINECITY") \
	X(MUS_MT_PYRE_EXTERIOR, "MACHUPI") \
	X(MUS_SCHOOL, "P-SCHOOL") \
	X(MUS_HALL_OF_FAME, "DENDOU") \
	X(MUS_FALLARBOR, "TONEKUSA") \
	X(MUS_SEALED_CHAMBER, "MABOROSI") \
	X(MUS_CONTEST_WINNER, "CON-FAN") \
	X(MUS_CONTEST, "CONTEST0") \
	X(MUS_ENCOUNTER_MAGMA, "MGM0") \
	X(MUS_INTRO_BATTLE, "T-BATTLE") \
	X(MUS_WEATHER_KYOGRE, "OOAME") \
	X(MUS_WEATHER_GROUDON, "HIDERI") \
	X(MUS_SOOTOPOLIS, "RUNECITY") \
	X(MUS_CONTEST_RESULTS, "CON-K") \
	X(MUS_HALL_OF_FAME_ROOM, "EIKOU-R") \
	X(MUS_TRICK_HOUSE, "KARAKURI") \
	X(MUS_ENCOUNTER_TWINS, "HUTAGO") \
	X(MUS_ENCOUNTER_ELITE_FOUR, "SITENNOU") \
	X(MUS_ENCOUNTER_HIKER, "YAMA-EYE") \
	X(MUS_CONTEST_LOBBY, "CONLOBBY") \
	X(MUS_ENCOUNTER_INTERVIEWER, "INTER-V") \
	X(MUS_ENCOUNTER_CHAMPION, "DAIGO") \
	X(MUS_CREDITS, "THANKFOR") \
	X(MUS_END, "END") \
	X(MUS_VS_WILD, "BATTLE27") \
	X(MUS_VS_AQUA_MAGMA, "BATTLE31") \
	X(MUS_VS_TRAINER, "BATTLE20") \
	X(MUS_VS_GYM_LEADER, "BATTLE32") \
	X(MUS_VS_CHAMPION, "BATTLE33") \
	X(MUS_VS_REGI, "BATTLE36") \
	X(MUS_VS_KYOGRE_GROUDON, "BATTLE34") \
	X(MUS_VS_RIVAL, "BATTLE35") \
	X(MUS_VS_ELITE_FOUR, "BATTLE38") \
	X(MUS_VS_AQUA_MAGMA_LEADER, "BATTLE30")

#define SOUND_LIST_SE \
	X(MUS_DUMMY, "STOP") \
	X(SE_USE_ITEM, "KAIFUKU") \
	X(SE_PC_LOGIN, "PC-LOGON") \
	X(SE_PC_OFF, "PC-OFF") \
	X(SE_PC_ON, "PC-ON") \
	X(SE_SELECT, "SELECT") \
	X(SE_WIN_OPEN, "WIN-OPEN") \
	X(SE_WALL_HIT, "WALL-HIT") \
	X(SE_DOOR, "DOOR") \
	X(SE_EXIT, "KAIDAN") \
	X(SE_LEDGE, "DANSA") \
	X(SE_BIKE_BELL, "JITENSYA") \
	X(SE_NOT_EFFECTIVE, "KOUKA-L") \
	X(SE_EFFECTIVE, "KOUKA-M") \
	X(SE_SUPER_EFFECTIVE, "KOUKA-H") \
	X(SE_BALL_OPEN, "BOWA2") \
	X(SE_FAINT, "POKE-DEAD") \
	X(SE_FLEE, "NIGERU") \
	X(SE_SLIDING_DOOR, "JIDO-DOA") \
	X(SE_SHIP, "NAMINORI") \
	X(SE_BANG, "BAN") \
	X(SE_PIN, "PIN") \
	X(SE_BOO, "BOO") \
	X(SE_BALL, "BOWA") \
	X(SE_CONTEST_PLACE, "JYUNI") \
	X(SE_A, "A") \
	X(SE_I, "I") \
	X(SE_U, "U") \
	X(SE_E, "E") \
	X(SE_O, "O") \
	X(SE_N, "N") \
	X(SE_SUCCESS, "SEIKAI") \
	X(SE_FAILURE, "HAZURE") \
	X(SE_EXP, "EXP") \
	X(SE_BIKE_HOP, "JITE-PYOKO") \
	X(SE_SWITCH, "MU-PACHI") \
	X(SE_CLICK, "TK-KASYA") \
	X(SE_FU_ZAKU, "FU-ZAKU") \
	X(SE_CONTEST_CONDITION_LOSE, "FU-ZAKU2") \
	X(SE_LAVARIDGE_FALL_WARP, "FU-ZUZUZU") \
	X(SE_ICE_STAIRS, "RU-GASHIN") \
	X(SE_ICE_BREAK, "RU-GASYAN") \
	X(SE_ICE_CRACK, "RU-BARI") \
	X(SE_FALL, "RU-HYUU") \
	X(SE_UNLOCK, "KI-GASYAN") \
	X(SE_WARP_IN, "TK-WARPIN") \
	X(SE_WARP_OUT, "TK-WARPOUT") \
	X(SE_REPEL, "TU-SAA") \
	X(SE_ROTATING_GATE, "HI-TURUN") \
	X(SE_TRUCK_MOVE, "TRACK-MOVE") \
	X(SE_TRUCK_STOP, "TRACK-STOP") \
	X(SE_TRUCK_UNLOAD, "TRACK-HAIK") \
	X(SE_TRUCK_DOOR, "TRACK-DOOR") \
	X(SE_BERRY_BLENDER, "MOTER") \
	X(SE_CARD, "CARD") \
	X(SE_SAVE, "SAVE") \
	X(SE_BALL_BOUNCE_1, "KON") \
	X(SE_BALL_BOUNCE_2, "KON2") \
	X(SE_BALL_BOUNCE_3, "KON3") \
	X(SE_BALL_BOUNCE_4, "KON4") \
	X(SE_BALL_TRADE, "SUIKOMU") \
	X(SE_BALL_THROW, "NAGERU") \
	X(SE_NOTE_C, "TOY-C") \
	X(SE_NOTE_D, "TOY-D") \
	X(SE_NOTE_E, "TOY-E") \
	X(SE_NOTE_F, "TOY-F") \
	X(SE_NOTE_G, "TOY-G") \
	X(SE_NOTE_A, "TOY-A") \
	X(SE_NOTE_B, "TOY-B") \
	X(SE_NOTE_C_HIGH, "TOY-C1") \
	X(SE_PUDDLE, "MIZU") \
	X(SE_BRIDGE_WALK, "HASHI") \
	X(SE_ITEMFINDER, "DAUGI") \
	X(SE_DING_DONG, "PINPON") \
	X(SE_BALLOON_RED, "FUUSEN1") \
	X(SE_BALLOON_BLUE, "FUUSEN2") \
	X(SE_BALLOON_YELLOW, "FUUSEN3") \
	X(SE_BREAKABLE_DOOR, "TOY-KABE") \
	X(SE_MUD_BALL, "TOY-DANGO") \
	X(SE_FIELD_POISON, "DOKU") \
	X(SE_ESCALATOR, "ESUKA") \
	X(SE_THUNDERSTORM, "T-AME") \
	X(SE_THUNDERSTORM_STOP, "T-AME-E") \
	X(SE_DOWNPOUR, "T-OOAME") \
	X(SE_DOWNPOUR_STOP, "T-OOAME-E") \
	X(SE_RAIN, "T-KOAME") \
	X(SE_RAIN_STOP, "T-KOAME-E") \
	X(SE_THUNDER, "T-KAMI") \
	X(SE_THUNDER2, "T-KAMI2") \
	X(SE_ELEVATOR, "ELEBETA") \
	X(SE_LOW_HEALTH, "HINSI") \
	X(SE_EXP_MAX, "EXPMAX") \
	X(SE_ROULETTE_BALL, "TAMAKORO") \
	X(SE_ROULETTE_BALL2, "TAMAKORO-E") \
	X(SE_TAILLOW_WING_FLAP, "BASABASA") \
	X(SE_SHOP, "REGI") \
	X(SE_CONTEST_HEART, "C-GAJI") \
	X(SE_CONTEST_CURTAIN_RISE, "C-MAKU-U") \
	X(SE_CONTEST_CURTAIN_FALL, "C-MAKU-D") \
	X(SE_CONTEST_ICON_CHANGE, "C-PASI") \
	X(SE_CONTEST_ICON_CLEAR, "C-SYU") \
	X(SE_CONTEST_MONS_TURN, "C-PIKON") \
	X(SE_SHINY, "REAPOKE") \
	X(SE_INTRO_BLAST, "OP-BASYU") \
	X(SE_MUGSHOT, "BT-START") \
	X(SE_APPLAUSE, "DENDOU") \
	X(SE_VEND, "JIHANKI") \
	X(SE_ORB, "TAMA") \
	X(SE_DEX_SCROLL, "Z-SCROLL") \
	X(SE_DEX_PAGE, "Z-PAGE") \
	X(SE_POKENAV_ON, "PN-ON") \
	X(SE_POKENAV_OFF, "PN-OFF") \
	X(SE_DEX_SEARCH, "Z-SEARCH") \
	X(SE_EGG_HATCH, "TAMAGO") \
	X(SE_BALL_TRAY_ENTER, "TB-START") \
	X(SE_BALL_TRAY_BALL, "TB-KON") \
	X(SE_BALL_TRAY_EXIT, "TB-KARA") \
	X(SE_GLASS_FLUTE, "BIDORO") \
	X(SE_M_THUNDERBOLT, "W085") \
	X(SE_M_THUNDERBOLT2, "W085B") \
	X(SE_M_HARDEN, "W231") \
	X(SE_M_NIGHTMARE, "W171") \
	X(SE_M_VITAL_THROW, "W233") \
	X(SE_M_VITAL_THROW2, "W233B") \
	X(SE_M_BUBBLE, "W145") \
	X(SE_M_BUBBLE2, "W145B") \
	X(SE_M_BUBBLE3, "W145C") \
	X(SE_M_RAIN_DANCE, "W240") \
	X(SE_M_CUT, "W015") \
	X(SE_M_STRING_SHOT, "W081") \
	X(SE_M_STRING_SHOT2, "W081B") \
	X(SE_M_ROCK_THROW, "W088") \
	X(SE_M_GUST, "W016") \
	X(SE_M_GUST2, "W016B") \
	X(SE_M_DOUBLE_SLAP, "W003") \
	X(SE_M_DOUBLE_TEAM, "W104") \
	X(SE_M_RAZOR_WIND, "W013") \
	X(SE_M_ICY_WIND, "W196") \
	X(SE_M_THUNDER_WAVE, "W086") \
	X(SE_M_COMET_PUNCH, "W004") \
	X(SE_M_MEGA_KICK, "W025") \
	X(SE_M_MEGA_KICK2, "W025B") \
	X(SE_M_CRABHAMMER, "W152") \
	X(SE_M_JUMP_KICK, "W026") \
	X(SE_M_FLAME_WHEEL, "W172") \
	X(SE_M_FLAME_WHEEL2, "W172B") \
	X(SE_M_FLAMETHROWER, "W053") \
	X(SE_M_FIRE_PUNCH, "W007") \
	X(SE_M_TOXIC, "W092") \
	X(SE_M_SACRED_FIRE, "W221") \
	X(SE_M_SACRED_FIRE2, "W221B") \
	X(SE_M_EMBER, "W052") \
	X(SE_M_TAKE_DOWN, "W036") \
	X(SE_M_BLIZZARD, "W059") \
	X(SE_M_BLIZZARD2, "W059B") \
	X(SE_M_SCRATCH, "W010") \
	X(SE_M_VICEGRIP, "W011") \
	X(SE_M_WING_ATTACK, "W017") \
	X(SE_M_FLY, "W019") \
	X(SE_M_SAND_ATTACK, "W028") \
	X(SE_M_RAZOR_WIND2, "W013B") \
	X(SE_M_BITE, "W044") \
	X(SE_M_HEADBUTT, "W029") \
	X(SE_M_SURF, "W057") \
	X(SE_M_HYDRO_PUMP, "W056") \
	X(SE_M_WHIRLPOOL, "W250") \
	X(SE_M_HORN_ATTACK, "W030") \
	X(SE_M_TAIL_WHIP, "W039") \
	X(SE_M_MIST, "W054") \
	X(SE_M_POISON_POWDER, "W077") \
	X(SE_M_BIND, "W020") \
	X(SE_M_DRAGON_RAGE, "W082") \
	X(SE_M_SING, "W047") \
	X(SE_M_PERISH_SONG, "W195") \
	X(SE_M_PAY_DAY, "W006") \
	X(SE_M_DIG, "W091") \
	X(SE_M_DIZZY_PUNCH, "W146") \
	X(SE_M_SELF_DESTRUCT, "W120") \
	X(SE_M_EXPLOSION, "W153") \
	X(SE_M_ABSORB_2, "W071B") \
	X(SE_M_ABSORB, "W071") \
	X(SE_M_SCREECH, "W103") \
	X(SE_M_BUBBLE_BEAM, "W062") \
	X(SE_M_BUBBLE_BEAM2, "W062B") \
	X(SE_M_SUPERSONIC, "W048") \
	X(SE_M_BELLY_DRUM, "W187") \
	X(SE_M_METRONOME, "W118") \
	X(SE_M_BONEMERANG, "W155") \
	X(SE_M_LICK, "W122") \
	X(SE_M_PSYBEAM, "W060") \
	X(SE_M_FAINT_ATTACK, "W185") \
	X(SE_M_SWORDS_DANCE, "W014") \
	X(SE_M_LEER, "W043") \
	X(SE_M_SWAGGER, "W207") \
	X(SE_M_SWAGGER2, "W207B") \
	X(SE_M_HEAL_BELL, "W215") \
	X(SE_M_CONFUSE_RAY, "W109") \
	X(SE_M_SNORE, "W173") \
	X(SE_M_BRICK_BREAK, "W280") \
	X(SE_M_GIGA_DRAIN, "W202") \
	X(SE_M_PSYBEAM2, "W060B") \
	X(SE_M_SOLAR_BEAM, "W076") \
	X(SE_M_PETAL_DANCE, "W080") \
	X(SE_M_TELEPORT, "W100") \
	X(SE_M_MINIMIZE, "W107") \
	X(SE_M_SKETCH, "W166") \
	X(SE_M_SWIFT, "W129") \
	X(SE_M_REFLECT, "W115") \
	X(SE_M_BARRIER, "W112") \
	X(SE_M_DETECT, "W197") \
	X(SE_M_LOCK_ON, "W199") \
	X(SE_M_MOONLIGHT, "W236") \
	X(SE_M_CHARM, "W204") \
	X(SE_M_CHARGE, "W268") \
	X(SE_M_STRENGTH, "W070") \
	X(SE_M_HYPER_BEAM, "W063") \
	X(SE_M_WATERFALL, "W127") \
	X(SE_M_REVERSAL, "W179") \
	X(SE_M_ACID_ARMOR, "W151") \
	X(SE_M_SANDSTORM, "W201") \
	X(SE_M_TRI_ATTACK, "W161") \
	X(SE_M_TRI_ATTACK2, "W161B") \
	X(SE_M_ENCORE, "W227") \
	X(SE_M_ENCORE2, "W227B") \
	X(SE_M_BATON_PASS, "W226") \
	X(SE_M_MILK_DRINK, "W208") \
	X(SE_M_ATTRACT, "W213") \
	X(SE_M_ATTRACT2, "W213B") \
	X(SE_M_MORNING_SUN, "W234") \
	X(SE_M_FLATTER, "W260") \
	X(SE_M_SAND_TOMB, "W328") \
	X(SE_M_GRASSWHISTLE, "W320") \
	X(SE_M_SPIT_UP, "W255") \
	X(SE_M_DIVE, "W291") \
	X(SE_M_EARTHQUAKE, "W089") \
	X(SE_M_TWISTER, "W239") \
	X(SE_M_SWEET_SCENT, "W230") \
	X(SE_M_YAWN, "W281") \
	X(SE_M_SKY_UPPERCUT, "W327") \
	X(SE_M_STAT_INCREASE, "W287") \
	X(SE_M_HEAT_WAVE, "W257") \
	X(SE_M_UPROAR, "W253") \
	X(SE_M_HAIL, "W258") \
	X(SE_M_COSMIC_POWER, "W322") \
	X(SE_M_TEETER_DANCE, "W298") \
	X(SE_M_STAT_DECREASE, "W287B") \
	X(SE_M_HAZE, "W114") \
	X(SE_M_HYPER_BEAM2, "W063B")

// Create BGM list
#define X(songId, name) static const u8 sBGMName_##songId[] = _(name);
SOUND_LIST_BGM
#undef X

#define X(songId, name) sBGMName_##songId,
static const u8 *const gBGMNames[] =
{
SOUND_LIST_BGM
};
#undef X

// Create SE list
#define X(songId, name) static const u8 sSEName_##songId[] = _(name);
SOUND_LIST_SE
#undef X

#define X(songId, name) sSEName_##songId,
static const u8 *const gSENames[] =
{
SOUND_LIST_SE
};
#undef X

void Task_InitCryTest(u8 taskId)
{
    struct CryScreenWindow waveformWindow, cryScreenWindow;
    u8 unused;

    Text_LoadWindowTemplate(&gMenuTextWindowLoadTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    gSoundTestCryNum = 1;
    ResetSpriteData();
    FreeAllSpritePalettes();

    waveformWindow.charBase = 0x2000;
    waveformWindow.screenBase = 29;
    waveformWindow.paletteNo = 12;
    waveformWindow.yPos = 30;
    waveformWindow.xPos = 4;

    unused = 0;
    gDexCryScreenState = 0;

    while (LoadCryWaveformWindow(&waveformWindow, 3) == FALSE)
        ;

    cryScreenWindow.charBase = 0;
    cryScreenWindow.screenBase = 15;
    cryScreenWindow.paletteNo = 13;
    cryScreenWindow.xPos = 12;
    cryScreenWindow.yPos = 12;

    unused = 0;
    gDexCryScreenState = 0;

    while (LoadCryMeter(&cryScreenWindow, 2) == FALSE)
        ;

    Menu_DrawStdWindowFrame(0, 16, 5, 19);
    PrintCryNumber();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG2CNT = 0xF01;
    REG_BG3CNT = 0x1D03;
    REG_DISPCNT = 0x1d40;
    m4aMPlayFadeOutTemporarily(&gMPlayInfo_BGM, 2);
    gTasks[taskId].func = Task_ProcessCryTestInput;
}

void Task_ProcessCryTestInput(u8 taskId)
{
    UpdateCryWaveformWindow(3);

    if (JOY_NEW(A_BUTTON))
    {
        CryScreenPlayButton(gSoundTestCryNum);
    }
    if (JOY_NEW(R_BUTTON))
    {
        StopCryAndClearCrySongs();
    }
    if (JOY_REPT(DPAD_UP))
    {
        if(--gSoundTestCryNum == 0)
            gSoundTestCryNum = NUM_SOUND_CHECK_CRY_SPECIES;
        PrintCryNumber();
    }
    if (JOY_REPT(DPAD_DOWN))
    {
        if(++gSoundTestCryNum > NUM_SOUND_CHECK_CRY_SPECIES)
            gSoundTestCryNum = 1;
        PrintCryNumber();
    }
    if (JOY_NEW(B_BUTTON))
    {
        REG_DISPCNT = 0x7140;
        REG_WIN0H = WIN_RANGE(17, 223);
        REG_WIN0V = WIN_RANGE(1, 31);
        Menu_EraseWindowRect(0, 0, 29, 19);
        gTasks[taskId].func = Task_InitSoundCheckMenu;
        FreeCryScreen();
    }
}

void PrintCryNumber(void)
{
    PrintSignedNumber(gSoundTestCryNum, 1, 17, 3);
}
