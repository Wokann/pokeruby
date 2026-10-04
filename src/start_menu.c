#include "global.h"
#include "battle_tower.h"
#include "start_menu.h"
#include "event_data.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "item_menu.h"
#include "load_save.h"
#include "m4a.h"
#include "main.h"
#include "event_object_lock.h"
#include "menu.h"
#include "new_game.h"
#include "option_menu.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon_menu.h"
#include "pokenav.h"
#include "overworld.h"
#include "safari_zone.h"
#include "save.h"
#include "save_menu_util.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "trainer_card.h"
#include "scanline_effect.h"

//Menu actions
enum {
    MENU_ACTION_POKEDEX,
    MENU_ACTION_POKEMON,
    MENU_ACTION_BAG,
    MENU_ACTION_POKENAV,
    MENU_ACTION_PLAYER,
    MENU_ACTION_SAVE,
    MENU_ACTION_OPTION,
    MENU_ACTION_EXIT,
    MENU_ACTION_RETIRE_SAFARI,
    MENU_ACTION_PLAYER_LINK
};

#if DEBUG
static u32 _debugStartMenu_0  __attribute__((unused));
static u32 _debugStartMenu_1 __attribute__((unused));
#endif

static u8 (*sSaveDialogCallback)(void);
static u8 sSaveDialogTimer;    //Number of frames to keep the window on screen after save was completed
static bool8 sSavingComplete;

extern u16 gSaveFileStatus;
extern u16 gSpecialVar_Result;


u8 (*gMenuCallback)(void);

EWRAM_DATA static u8 sStartMenuCursorPos = 0;
EWRAM_DATA static u8 sNumStartMenuActions = 0;
EWRAM_DATA static u8 sCurrentStartMenuActions[10] = {0};

static bool8 StartMenuPokedexCallback(void);
static bool8 StartMenuPokemonCallback(void);
static bool8 StartMenuBagCallback(void);
static bool8 StartMenuPokeNavCallback(void);
static bool8 StartMenuPlayerNameCallback(void);
static bool8 StartMenuSaveCallback(void);
static bool8 StartMenuOptionCallback(void);
static bool8 StartMenuExitCallback(void);
static bool8 StartMenuSafariZoneRetireCallback(void);
static bool8 StartMenuLinkModePlayerNameCallback(void);

static const struct MenuAction sStartMenuItems[] =
{
    { gText_MenuPokedex, StartMenuPokedexCallback },
    { gText_MenuPokemon, StartMenuPokemonCallback },
    { gText_MenuBag, StartMenuBagCallback },
    { gText_MenuPokenav, StartMenuPokeNavCallback },
    { gText_MenuPlayer, StartMenuPlayerNameCallback },
    { gText_MenuSave, StartMenuSaveCallback },
    { gText_MenuOption, StartMenuOptionCallback },
    { gText_MenuExit, StartMenuExitCallback },
    { gText_MenuRetire, StartMenuSafariZoneRetireCallback },
    { gText_MenuPlayer, StartMenuLinkModePlayerNameCallback },
};

//Private functions
static void BuildStartMenuActions(void);
static void AddStartMenuAction(u8 action);
static void BuildNormalStartMenu(void);
static void BuildSafariZoneStartMenu(void);
static void BuildLinkModeStartMenu(void);
static void ShowSafariBallsWindow(void);
static bool32 PrintStartMenuActions(s16 *pIndex, u32 count);
static bool32 InitStartMenuStep(s16 *step, s16 *index);
static void StartMenuTask(u8 taskId);
static bool8 HandleStartMenuInput(void);
static bool8 SaveStartCallback(void);
static bool8 SaveCallback(void);
static void InitSave(void);
static u8 RunSaveCallback(void);
static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void));
static void SaveGameTask(u8 taskId);
static void HideSaveInfoWindow(void);
static void HideSaveMessageWindow(void);
static void SaveStartTimer(void);
static u8 SaveDialogCheckForTimeoutOrKeypress(void);
static u8 SaveDialogCheckForTimeoutAndKeypress(void);
static u8 SaveDialogCheckForTimeoutAndKeypress(void);
static u8 SaveConfirmSaveCallback(void);
static u8 SaveYesNoCallback(void);
static u8 SaveConfirmInputCallback(void);
static u8 SaveFileExistsCallback(void);
static u8 SaveConfirmOverwriteCallback(void);
static u8 SaveOverwriteInputCallback(void);
static u8 SaveSavingMessageCallback(void);
static u8 SaveDoSaveCallback(void);
static u8 SaveSuccessCallback(void);
static u8 SaveReturnSuccessCallback(void);
static u8 SaveErrorCallback(void);
static u8 SaveReturnErrorCallback(void);
static void VBlankCB_LinkBattleSave(void);
static bool32 InitSaveWindowAfterLinkBattle(u8 *state);
static void CB2_SaveAfterLinkBattle(void);
static void Task_SaveAfterLinkBattle(u8 taskId);

#if DEBUG

void debug_sub_8075D9C(void);

u8 debug_sub_8075C30(void)
{
    CloseMenu();
    debug_sub_8075D9C();
    return 1;
}

extern const u8 gUnknown_Debug_839B6D8[];

void debug_sub_8075C40(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 savedIme;
    s32 i;

    switch (data[0])
    {
    case 0:
        m4aSoundVSyncOff();
        data[0]++;
        break;
    case 1:
        savedIme = REG_IME;
        REG_IME = 0;
        REG_TM2CNT_L = 0;
        REG_TM2CNT = 0x830000;
        for (i = 0; i < 0xC350; i++)
        {
            DmaSet(
                0,
                gScanlineEffectRegBuffers,
                &REG_WIN0H,
                ((DMA_ENABLE | DMA_START_HBLANK | DMA_16BIT | DMA_REPEAT | DMA_SRC_INC | DMA_DEST_RELOAD) << 16) | 1);
            DmaStop(0);
        }
        REG_TM2CNT_H = 0;
        _debugStartMenu_0 = REG_TM2CNT_L;
        REG_TM2CNT_L = 0;
        REG_IME = savedIme;
        _debugStartMenu_1 = i;
        m4aSoundVSyncOn();
        data[0]++;
        break;
    case 2:
        PlaySE(0x15);
        ConvertIntToDecimalStringN(gStringVar1, _debugStartMenu_1, 1, 8);
        ConvertIntToDecimalStringN(gStringVar2, _debugStartMenu_0, 1, 8);
        Menu_DisplayDialogueFrame();
        Menu_PrintText(gUnknown_Debug_839B6D8, 2, 15);
        data[0]++;
        break;
    case 3:
        if (JOY_NEW(A_BUTTON))
        {
            Menu_EraseScreen();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
        }
        break;
    }
}

void debug_sub_8075D9C(void)
{
    CreateTask(debug_sub_8075C40, 10);
    LockPlayerFieldControls();
}

void debug_sub_8075DB4(struct BattleTowerEReaderTrainer *ereaderTrainer, const u8 *b, u32 trainerId)
{
    s32 i;
    s32 r3;

    ereaderTrainer->trainerClass = trainerId % 77;
    SetTrainerId(trainerId, ereaderTrainer->trainerId);
    StringCopy8(ereaderTrainer->name, b);
    r3 = 7;
    for (i = 0; i < 6; i++)
    {
        ereaderTrainer->greeting[i] = gSaveBlock1.easyChats.unk2B28[i];
        ereaderTrainer->farewellPlayerLost[i] = r3;
        ereaderTrainer->farewellPlayerWon[i] = r3 + 6;
        r3++;
    }
    for (i = 0; i < 3; i++)
        ConvertPokemonToBattleTowerPokemon(&gPlayerParty[i], &ereaderTrainer->party[i]);
    SetEReaderTrainerChecksum(ereaderTrainer);
}

void SetDexPokemonPokenavFlags(void)
{
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKENAV_GET);
}

#endif

static void BuildStartMenuActions(void)
{
    sNumStartMenuActions = 0;
    if (IsOverworldLinkActive() == TRUE)
        BuildLinkModeStartMenu();
    else
    {
        if (GetSafariZoneFlag() == TRUE)
            BuildSafariZoneStartMenu();
        else
            BuildNormalStartMenu();
    }
}

static void AddStartMenuAction(u8 action)
{
    AppendToList(sCurrentStartMenuActions, &sNumStartMenuActions, action);
}

static void BuildNormalStartMenu(void)
{
    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKEDEX);
    if (FlagGet(FLAG_SYS_POKEMON_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);
    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKENAV);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_SAVE);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildSafariZoneStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_RETIRE_SAFARI);
    AddStartMenuAction(MENU_ACTION_POKEDEX);
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildLinkModeStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);
    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
        AddStartMenuAction(MENU_ACTION_POKENAV);
    AddStartMenuAction(MENU_ACTION_PLAYER_LINK);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

//Show number of safari balls left
static void ShowSafariBallsWindow(void)
{
    AlignInt2InMenuWindow(gStringVar1, gNumSafariBalls, 12, 1);
    Menu_DrawStdWindowFrame(0, 0, 10, 5);
    Menu_PrintText(gText_SafariBallStock, 1, 1);
}

//Prints count menu items starting at *pIndex
static bool32 PrintStartMenuActions(s16 *pIndex, u32 count)
{
    int index = *pIndex;

    do
    {
        Menu_PrintText(sStartMenuItems[sCurrentStartMenuActions[index]].text, 23, 2 + index * 2);
        index++;
        if (index >= sNumStartMenuActions)
        {
            *pIndex = index;
            return TRUE;
        }
    }
    while (--count != 0);
    *pIndex = index;
    return FALSE;
}

static bool32 InitStartMenuStep(s16 *step, s16 *index)
{
    switch (*step)
    {
    case 1:
        BuildStartMenuActions();
        (*step)++;
        break;
    case 2:
        Menu_DrawStdWindowFrame(22, 0, 29, sNumStartMenuActions * 2 + 3);
        *index = 0;
        (*step)++;
        break;
    case 3:
        if (GetSafariZoneFlag())
            ShowSafariBallsWindow();
        (*step)++;
        break;
    case 4:
        if (PrintStartMenuActions(index, 2))
            (*step)++;
        break;
    case 0:
        (*step)++;
        break;
    case 5:
        sStartMenuCursorPos = InitMenu(0, 0x17, 2, sNumStartMenuActions, sStartMenuCursorPos, 6);
        return TRUE;
    }
    return FALSE;
}

static void InitStartMenu(void)
{
    s16 step = 0;
    s16 index = 0;

    while (InitStartMenuStep(&step, &index) == FALSE)
        ;
}

static void StartMenuTask(u8 taskId)
{
    if (InitStartMenuStep(gTasks[taskId].data, gTasks[taskId].data + 1) == TRUE)
    {
        *gTasks[taskId].data = 0;
        SwitchTaskToFollowupFunc(taskId);
    }
}

void CreateStartMenuTask(TaskFunc followupFunc)
{
    u8 taskId;

    InitMenuWindow(&gMenuTextWindowTemplate);
    taskId = CreateTask(StartMenuTask, 0x50);
    SetTaskFuncWithFollowupFunc(taskId, StartMenuTask, followupFunc);
}

void Task_ShowStartMenu(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        gMenuCallback = HandleStartMenuInput;
        task->data[0]++;
        break;
    case 1:
        if (gMenuCallback() == 1)
            DestroyTask(taskId);
        break;
    }
}

void ShowStartMenu(void)
{
    if (!IsOverworldLinkActive())
    {
        FreezeObjectEvents();
        PlayerFreeze();
        StopPlayerAvatar();
    }
    CreateStartMenuTask(Task_ShowStartMenu);
    LockPlayerFieldControls();
}

static bool8 HandleStartMenuInput(void)
{
    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(-1);
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func == StartMenuPokedexCallback)
        {
            if (GetNationalPokedexCount(0) == 0)
                return 0;
        }
        gMenuCallback = sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func;
        if (gMenuCallback != StartMenuSaveCallback &&
           gMenuCallback != StartMenuExitCallback &&
           gMenuCallback != StartMenuSafariZoneRetireCallback)
            FadeScreen(1, 0);
        return 0;
    }
    if (JOY_NEW(START_BUTTON | B_BUTTON))
    {
        CloseMenu();
        return 1;
    }
    return 0;
}

//When player selects POKEDEX
static bool8 StartMenuPokedexCallback(void)
{
    if (!gPaletteFade.active)
    {
        IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
        PlayRainStoppingSoundEffect();
        SetMainCallback2(CB2_OpenPokedex);
        return 1;
    }
    return 0;
}

//When player selects POKEMON
static bool8 StartMenuPokemonCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        SetMainCallback2(CB2_PartyMenuFromStartMenu);
        return 1;
    }
    return 0;
}

//When player selects BAG
static bool8 StartMenuBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        SetMainCallback2(sub_80A53F8);
        return 1;
    }
    return 0;
}

//When player selects POKENAV
static bool8 StartMenuPokeNavCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        SetMainCallback2(sub_80EBA5C);
        return 1;
    }
    return 0;
}

//When player selects his/her name
static bool8 StartMenuPlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu);
        return 1;
    }
    return 0;
}

//When player selects SAVE
static bool8 StartMenuSaveCallback(void)
{
    Menu_DestroyCursor();
    gMenuCallback = SaveStartCallback;
    return 0;
}

//When player selects OPTION
static bool8 StartMenuOptionCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        SetMainCallback2(CB2_InitOptionMenu);
        gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;
        return 1;
    }
    return 0;
}

//When player selects EXIT
static bool8 StartMenuExitCallback(void)
{
    CloseMenu();
    return 1;
}

//When player selects RETIRE
static bool8 StartMenuSafariZoneRetireCallback(void)
{
    CloseMenu();
    SafariZoneRetirePrompt();
    return 1;
}

//When player selects their name in multiplayer mode
static bool8 StartMenuLinkModePlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        ShowTrainerCardInLink(gLocalLinkPlayerId, CB2_ReturnToFieldWithOpenMenu);
        return 1;
    }
    return 0;
}

//Save dialog status
enum
{
    SAVE_IN_PROGRESS,
    SAVE_SUCCESS,
    SAVE_CANCELED,
    SAVE_ERROR,
};

static bool8 SaveStartCallback(void)
{
    InitSave();
    gMenuCallback = SaveCallback;
    return FALSE;
}

static bool8 SaveCallback(void)
{
    switch (RunSaveCallback())
    {
    case SAVE_IN_PROGRESS:
        return FALSE;
    case SAVE_CANCELED:
        //Go back to start menu
        Menu_EraseScreen();
        InitStartMenu();
        gMenuCallback = HandleStartMenuInput;
        return FALSE;
    case SAVE_SUCCESS:
    case SAVE_ERROR:
        Menu_EraseScreen();
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        return TRUE;
    }
    return FALSE;
}

static void InitSave(void)
{
    SaveMapView();
    sSaveDialogCallback = SaveConfirmSaveCallback;
    sSavingComplete = FALSE;
}

static u8 RunSaveCallback(void)
{
    if (sSavingComplete)
    {
        if (!Menu_UpdateWindowText())
            return 0;
    }
    sSavingComplete = FALSE;
    return sSaveDialogCallback();
}

void SaveGame(void)
{
    InitSave();
    CreateTask(SaveGameTask, 0x50);
}

static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void))
{
    StringExpandPlaceholders(gStringVar4, message);
    Menu_DisplayDialogueFrame();
    MenuPrintMessageDefaultCoords(gStringVar4);
    sSavingComplete = TRUE;
    sSaveDialogCallback = saveCallback;
}

static void SaveGameTask(u8 taskId)
{
    u8 status = RunSaveCallback();

    switch (status)
    {
    case SAVE_CANCELED:
    case SAVE_ERROR:
        gSpecialVar_Result = 0;
        break;
    case SAVE_SUCCESS:
        gSpecialVar_Result = status;
        break;
    case SAVE_IN_PROGRESS:
        return;
    }
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void HideSaveInfoWindow(void)
{
    RemoveSaveInfoWindow(0, 0);
}

static void HideSaveMessageWindow(void)
{
    Menu_EraseWindowRect(20, 8, 26, 13);
}

static void SaveStartTimer(void)
{
    sSaveDialogTimer = 60;
}

static bool8 SaveDialogCheckForTimeoutOrKeypress(void)
{
    sSaveDialogTimer--;
    if (JOY_HELD(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        return TRUE;
    }
    else if (sSaveDialogTimer == 0)
        return TRUE;
    return FALSE;
}

static bool8 SaveDialogCheckForTimeoutAndKeypress(void)
{
    if (sSaveDialogTimer != 0)
        sSaveDialogTimer--;
    else if (JOY_HELD(A_BUTTON))
        return TRUE;
    return FALSE;
}

static u8 SaveConfirmSaveCallback(void)
{
    Menu_EraseScreen();
    ShowSaveInfoWindow(0, 0);
    ShowSaveMessage(gText_ConfirmSave, SaveYesNoCallback);
    return SAVE_IN_PROGRESS;
}

static u8 SaveYesNoCallback(void)
{
    DisplayYesNoMenu(20, 8, 1);
    sSaveDialogCallback = SaveConfirmInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmInputCallback(void)
{
    switch (Menu_ProcessInputNoWrap_())
    {
    case 0:     //YES
        HideSaveMessageWindow();
        switch (gSaveFileStatus)
        {
        case 0:
        case 2:
            if (gDifferentSaveFile == FALSE)
            {
                sSaveDialogCallback = SaveFileExistsCallback;
                return SAVE_IN_PROGRESS;
            }
            sSaveDialogCallback = SaveSavingMessageCallback;
            return SAVE_IN_PROGRESS;
        default:
            sSaveDialogCallback = SaveFileExistsCallback;
            return SAVE_IN_PROGRESS;
        }
        break;
    case -1:    //B button
    case 1:     //NO
        HideSaveMessageWindow();
        HideSaveInfoWindow();
        return SAVE_CANCELED;
    }
    return SAVE_IN_PROGRESS;
}

static u8 SaveFileExistsCallback(void)
{
    ShowSaveMessage(
      gDifferentSaveFile == TRUE ? gText_DifferentSaveFile : gText_AlreadySavedFile,
      SaveConfirmOverwriteCallback);
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmOverwriteCallback(void)
{
    DisplayYesNoMenu(20, 8, 1);
    sSaveDialogCallback = SaveOverwriteInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveOverwriteInputCallback(void)
{
    switch (Menu_ProcessInputNoWrap_())
    {
    case 0:     //YES
        HideSaveMessageWindow();
        sSaveDialogCallback = SaveSavingMessageCallback;
        break;
    case -1:    //B button
    case 1:     //NO
        HideSaveMessageWindow();
        HideSaveInfoWindow();
        return SAVE_CANCELED;
    }
    return SAVE_IN_PROGRESS;
}

static u8 SaveSavingMessageCallback(void)
{
    //"SAVING... DON'T TURN OFF THE POWER."
    ShowSaveMessage(gText_SavingDontTurnOff, SaveDoSaveCallback);
    return SAVE_IN_PROGRESS;
}

static u8 SaveDoSaveCallback(void)
{
    u8 saveStatus;

    IncrementGameStat(GAME_STAT_SAVED_GAME);
    if (gDifferentSaveFile == TRUE)
    {
        saveStatus = Save_WriteData(SAVE_OVERWRITE_DIFFERENT_FILE);
        gDifferentSaveFile = FALSE;
    }
    else
    {
        saveStatus = Save_WriteData(SAVE_NORMAL);
    }

    if (saveStatus == SAVE_STATUS_OK)
    {
        //"(Player) saved the game."
        ShowSaveMessage(gText_PlayerSavedGame, SaveSuccessCallback);
    }
    else
    {
        //"Save error. Please exchange the backup memory."
        ShowSaveMessage(gText_SaveError, SaveErrorCallback);
    }

    SaveStartTimer();
    return SAVE_IN_PROGRESS;
}

static u8 SaveSuccessCallback(void)
{
    if (Menu_UpdateWindowText())
    {
        PlaySE(SE_SAVE);
        sSaveDialogCallback = SaveReturnSuccessCallback;
    }
    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnSuccessCallback(void)
{
    if (!IsSEPlaying() && SaveDialogCheckForTimeoutOrKeypress())
    {
        HideSaveInfoWindow();
        return SAVE_SUCCESS;
    }
    else
        return SAVE_IN_PROGRESS;
}

static u8 SaveErrorCallback(void)
{
    if (Menu_UpdateWindowText())
    {
        PlaySE(SE_BOO);
        sSaveDialogCallback = SaveReturnErrorCallback;
    }
    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnErrorCallback(void)
{
    if (!SaveDialogCheckForTimeoutAndKeypress())
        return SAVE_IN_PROGRESS;
    else
    {
        HideSaveInfoWindow();
        return SAVE_ERROR;
    }
}

static void VBlankCB_LinkBattleSave(void)
{
    TransferPlttBuffer();
}

static bool32 InitSaveWindowAfterLinkBattle(u8 *state)
{
    switch (*state)
    {
    case 0:
        REG_DISPCNT = 0;
        SetVBlankCallback(NULL);
        ScanlineEffect_Stop();
        DmaClear16(3, PLTT, PLTT_SIZE);
        DmaFill16Large(3, 0, (void *)(VRAM + 0x0), 0x18000, 0x1000);
        break;
    case 1:
        ResetSpriteData();
        ResetTasks();
        ResetPaletteFade();
        ScanlineEffect_Clear();
        break;
    case 2:
        Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
        InitMenuWindow(&gMenuTextWindowTemplate);
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_BG0_ON;
        break;
    case 3:
    {
        u32 savedIme;

        BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_LinkBattleSave);
        savedIme = REG_IME;
        REG_IME = 0;
        REG_IE |= 1;
        REG_IME = savedIme;
        break;
    }
    case 4:
        return TRUE;
    }
    (*state)++;
    return FALSE;
}

void CB2_SetUpSaveAfterLinkBattle(void)
{
    if (InitSaveWindowAfterLinkBattle(&gMain.state))
    {
        CreateTask(Task_SaveAfterLinkBattle, 0x50);
        SetMainCallback2(CB2_SaveAfterLinkBattle);
    }
}

static void CB2_SaveAfterLinkBattle(void)
{
    RunTasks();
    UpdatePaletteFade();
}

static void Task_SaveAfterLinkBattle(u8 taskId)
{
    s16 *state = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        switch (*state)
        {
        case 0:
            Menu_DisplayDialogueFrame();
            Menu_PrintText(gText_SavingDontTurnOffPower, 2, 15);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            (*state)++;
            break;
        case 1:
            SetSecretBase2Field_9_AndHideBG();
            sub_8125E2C();
            (*state)++;
            break;
        case 2:
            if (!sub_8125E6C())
                break;
            ClearSecretBase2Field_9_2();
            (*state)++;
            break;
        case 3:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            (*state)++;
            break;
        case 4:
            SetMainCallback2(gMain.savedCallback);
            DestroyTask(taskId);
            break;
        }
    }
}
