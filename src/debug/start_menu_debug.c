#if DEBUG

#include "global.h"
#include "constants/songs.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "battle.h"
#include "debug.h"
#include "link.h"
#include "main.h"
#include "menu.h"
#include "mystery_event_menu.h"
#include "overworld.h"
#include "reset_rtc_screen.h"
#include "rtc.h"
#include "save.h"
#include "sound.h"
#include "task.h"
#include "text.h"
#include "trade.h"
#include "start_menu.h"
#include "string_util.h"
#include "new_game.h"
#include "script.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "credits.h"
#include "event_data.h"
#include "berry.h"
#include "pokedex.h"
#include "mori_debug_menu.h"
#include "cable_club.h"
#include "field_fadetransition.h"
#include "wild_encounter.h"
#include "battle_setup.h"
#include "safari_zone.h"
#include "fldeff_flash.h"
#include "fldeff.h"
#include "fldeff_strength.h"
#include "pokemon_menu.h"
#include "data2.h"
#include "item.h"
#include "reset_rtc_screen.h"
#include "pokeblock.h"
#include "ewram.h"
#include "gba/flash_internal.h"
#include "gba/m4a_internal.h"
#include "berry_blender.h"

extern void Debug_SetUpFieldMove_Cut(void);
extern void Debug_SetUpFieldMove_SecretPower(void);

#if (ENGLISH && REVISION == 0)
u8 gDebugTextModeDisplayEnabled;
u8 gDebugTextModeDisplaySuspended;
#else
u32 gDebugForceFlashError;
#endif

void Task_DebugMenuTestMenuInput(u8);
void DebugMenu_DisplayBuildDate(u8 x, u8 y);
void DebugMenu_DisplayContinuousDate(u8 x, u8 y);
u8 DebugMenu_InitMainMenu(void);
u8 DebugMenu_HandleMainMenuInput(void);
void DebugMenu_DrawMainMenuPage(void);
void DebugMenu_DrawMapId(void);
void DebugMenu_DrawPlayerCoords(void);
u8 DebugMenu_ProcessActionMenuInput(const struct MenuAction *menuActions);
void DebugMenu_DrawActionMenu(const struct MenuAction *menuAction, u8 width, u8 itemCount);
u8 DebugMenu_OpenFieldMoveMenu(void);
bool8 DebugMenu_WaitForAButton(void);
void DebugMenu_OpenItemGrantMenu(void);
void Task_DebugMenuInitItemGrant(u8 taskId);
void Task_DebugMenuSelectItem(u8 taskId);
void Task_DebugMenuSelectItemQuantity(u8 taskId);
void Task_DebugMenuCloseItemGrant(u8 taskId);
void Task_DebugMenuSelectBattleTowerLevel(u8 taskId);
void Task_DebugMenuEditBattleTowerStage(u8 taskId);
void Task_DebugMenuCloseBattleTowerStageEditor(u8 taskId);
bool8 DebugMenu_PromptFlashWrite(void);
bool8 DebugMenu_HandleFlashWritePrompt(void);
bool8 DebugMenu_WriteSharedMemToFlash(void);
bool8 DebugMenu_ReadFlashToSharedMem(void);
bool8 DebugMenu_ShowFlashOperationResult(void);
bool8 DebugMenu_CloseFlashOperationResult(void);
bool8 DebugMenu_HandleFlashTestMenuInput(void);
bool32 DebugMenu_ProgramFlashSectorsWithVBlankOff(u8 sector, u8 *data, u32 size);
void DebugMenu_ReadFlashSectors(u8 sector, u8 *data, u32 size);
void Task_DebugMenuEditPlayTime(u8 taskId);
void Task_DebugMenuClosePlayTimeEditor(u8 taskId);

u8 DebugMenu_Exit(void);
u8 DebugMenu_OpenWatanabe(void);
u8 DebugMenu_OpenSogabe(void);
u8 DebugMenu_OpenTamada(void);
u8 DebugMenu_OpenKagaya(void);
u8 DebugMenu_OpenMatsuda(void);
u8 DebugMenu_OpenNohara(void);
u8 DebugMenu_OpenNakamura(void);
u8 DebugMenu_Teleport(void);
u8 DebugMenu_EditPKMN(void);
u8 DebugMenu_SwitchBG(void);
u8 DebugMenu_OpenMori(void);
u8 DebugMenu_OpenTomomichi(void);
u8 DebugMenu_OpenAoki(void);
u8 DebugMenu_OpenTaya(void);
u8 DebugMenu_ToggleClearFlag(void);
u8 DebugMenu_ControlEncounter(void);
u8 DebugMenu_PTime(void);
u8 DebugMenu_MakeItems(void);
u8 TayaDebugMenu_OpenRegionMap(void);
u8 DebugMenu_ViewPortraits(void);
u8 DebugMenu_TimeRecords(void);
u8 DebugMenu_SetTime(void);
u8 DebugMenu_NationalDex(void);
u8 DebugMenu_CreatePKMN(void);
u8 DebugMenu_ViewPokemonGraphics(void);
u8 DebugMenu_BattleForDebug(void);
u8 DebugMenu_AllBadges(void);
u8 DebugMenu_HoennNationalDex(void);
u8 DebugMenu_SetRamBerry(void);
u8 DebugMenu_UseHM(void);
u8 DebugMenu_OpenIwasawa(void);
u8 DebugMenu_ToggleBGM(void);
u8 DebugMenu_OpenSizeComparison(void);
u8 DebugMenu_Safari(void);
u8 DebugMenu_RematchTrainers(void);
u8 DebugMenu_MiragaIslandRND(void);
u8 DebugMenu_HallOfFame(void);
u8 DebugMenu_OpenMysteryEvent(void);
u8 DebugMenu_OpenLegendsRecord(void);
u8 DebugMenu_OpenWeatherEvents(void);
u8 DebugMenu_CellInfo(void);
u8 DebugMenu_CheckPKBLCK(void);
u8 DebugMenu_EffortValues(void);
u8 DebugMenu_HoennDex(void);
u8 DebugMenu_OpenSeeTrainers(void);
u8 DebugMenu_OpenBerryInfo(void);
u8 DebugMenu_BattleTowerStages(void);
u8 DebugMenu_EndSequenceDemo(void);
u8 DebugMenu_RandomNumberTest(void);
u8 DebugMenu_MeTooBackupMan(void);
u8 DebugMenu_OpenMurakawa(void);
#if !(ENGLISH && REVISION == 0)
u8 DebugMenu_OpenKiwa(void);
#endif
u8 DebugMenu_TestMenuNoOp(void);
u8 DebugMenu_OpenDisabledTest2(void);
u8 DebugMenu_BgTestNoOp(void);
u8 DebugMenu_OpenBattlePartyEditor(void);
u8 DebugMenu_OpenBerryBlender(void);
u8 DebugMenu_OpenLinkTest(void);
u8 DebugMenu_OpenSoundTest(void);
u8 DebugMenu_OpenFieldStartNameMenu(void);
u8 DebugMenu_ContinueSavedGame(void);
u8 DebugMenu_ClearSaveData(void);
u8 DebugMenu_OpenMysteryEventMenu(void);
u8 DebugMenu_OpenTradeDemo(void);
u8 DebugMenu_OpenMysteryEventDebugMenu(void);
u8 DebugMenu_OpenResetRtcScreen(void);
u8 DebugMenu_OpenRtcReset(void);
u8 DebugMenu_OpenSaveConverter(void);
void DebugMenu_ShowFieldStartNameMenu(void);

const u8 gDebugTextWatanabe[] = _("·WATANABE");
const u8 gDebugTextSogabe[] = _("SOGABE");
const u8 gDebugTextTamada[] = _("·TAMADA");
const u8 gDebugTextKagaya[] = _("KAGAYA");
const u8 gDebugTextMatsuda[] = _("MATUDA");
const u8 gDebugTextNohara[] = _("NOHARA");
const u8 gDebugTextNakamura[] = _("NAKAMURA");
const u8 gDebugTextExit[] = _("EXIT");
const u8 gDebugTextTeleport[] = _("Teleport");
const u8 gDebugTextSwitchBg[] = _("Switch BG");
const u8 gDebugTextEditPokemon[] = _("Edit your {PKMN}");
const u8 gDebugTextMori[] = _("MORI");
const u8 gDebugTextTomomichi[] = _("TOMOMITI");
const u8 gDebugTextAoki[] = _("·AOKI");
const u8 gDebugTextTaya[] = _("TAYA");
const u8 gDebugTextControlEncounter[] = _("Control Encounter");
const u8 gDebugTextPlayTime[] = _("PTIME");
const u8 gDebugTextMakeItems[] = _("Make items");
const u8 gDebugTextOpenRegionMap[] = _("Transport");
const u8 gDebugTextViewPortraits[] = _("See portraits");
const u8 gDebugTextTimeRecords[] = _("Time records");
const u8 gDebugTextSetTime[] = _("Set time");
const u8 gDebugTextNationalDex[] = _("National {POKE}Dex");
const u8 gDebugTextHoennDex[] = _("Hoenn {POKE}Dex");
const u8 gDebugTextCreatePokemon[] = _("Create {PKMN}");
const u8 gDebugTextViewPokemonGraphics[] = _("See {PKMN} graphics");
const u8 gDebugTextSeeTrainers[] = _("See trainers");
const u8 gDebugTextBattleForDebug[] = _("Battle for debug");
const u8 gDebugTextAllBadges[] = _("Full set of badges");
const u8 gDebugTextHoennNationalDex[] = _("Hoenn National Dex");
const u8 gDebugTextSetRamBerry[] = _("Set Ram berry");
const u8 gDebugTextUseHm[] = _("Use HM");
const u8 gDebugTextIwasawa[] = _("IWASAWA");
const u8 gDebugTextToggleBgm[] = _("BGM ON/OFF");
const u8 gDebugTextSizeComparison[] = _("Size comparison");
const u8 gDebugTextToggleClearFlag[] = _("Clear flag ON/OFF");
const u8 gDebugTextSafari[] = _("Safari");
const u8 gDebugTextTrainerRematches[] = _("Rematch trainers");
const u8 gDebugTextMirageIslandRandom[] = _("Mirage island RND");
const u8 gDebugTextHallOfFame[] = _("Hall of fame");
const u8 gDebugTextMysteryEvent[] = _("Mystery event");
const u8 gDebugTextGameStats[] = _("Legends records");
const u8 gDebugTextWeatherEvents[] = _("Weather events");
const u8 gDebugTextOpenCellInfo[] = _("Cell info.");
const u8 gDebugTextCheckPokeblock[] = _("Check {POKEBLOCK}");
const u8 gDebugTextEffortValues[] = _("Effort values");
const u8 gDebugTextBerryInfo[] = _("Berrie Info");
const u8 gDebugTextBattleTowerStages[] = _("Battle Tower stages");
const u8 gDebugTextEndSequenceDemo[] = _("End sequence demo");
const u8 gDebugTextRandomNumberTest[] = _("Random number test");
const u8 gDebugTextMeTooBackupMan[] = _("Me-too BackupMan");
const u8 gDebugTextMurakawa[] = _("MURAKAWA");
#if !(ENGLISH && REVISION == 0)
const u8 gDebugTextKiwaFont[] = _("KINA(FONT)");
#endif

const struct MenuAction gDebugMenuActions[] =
{
    { gDebugTextExit, DebugMenu_Exit },
    { gDebugTextWatanabe, DebugMenu_OpenWatanabe },
    { gDebugTextSogabe, DebugMenu_OpenSogabe },
    { gDebugTextTamada, DebugMenu_OpenTamada },
    { gDebugTextKagaya, DebugMenu_OpenKagaya },
    { gDebugTextMatsuda, DebugMenu_OpenMatsuda },
    { gDebugTextNohara, DebugMenu_OpenNohara },
    { gDebugTextNakamura, DebugMenu_OpenNakamura },
    { gDebugTextTeleport, DebugMenu_Teleport },
    { gDebugTextEditPokemon, DebugMenu_EditPKMN },
    { gDebugTextSwitchBg, DebugMenu_SwitchBG },
    { gDebugTextMori, DebugMenu_OpenMori },
    { gDebugTextTomomichi, DebugMenu_OpenTomomichi },
    { gDebugTextAoki, DebugMenu_OpenAoki },
    { gDebugTextTaya, DebugMenu_OpenTaya },
    { gDebugTextToggleClearFlag, DebugMenu_ToggleClearFlag },
    { gDebugTextControlEncounter, DebugMenu_ControlEncounter },
    { gDebugTextPlayTime, DebugMenu_PTime },
    { gDebugTextMakeItems, DebugMenu_MakeItems },
    { gDebugTextOpenRegionMap, TayaDebugMenu_OpenRegionMap },
    { gDebugTextViewPortraits, DebugMenu_ViewPortraits },
    { gDebugTextTimeRecords, DebugMenu_TimeRecords },
    { gDebugTextSetTime, DebugMenu_SetTime },
    { gDebugTextNationalDex, DebugMenu_NationalDex },
    { gDebugTextCreatePokemon, DebugMenu_CreatePKMN },
    { gDebugTextViewPokemonGraphics, DebugMenu_ViewPokemonGraphics },
    { gDebugTextBattleForDebug, DebugMenu_BattleForDebug },
    { gDebugTextAllBadges, DebugMenu_AllBadges },
    { gDebugTextHoennNationalDex, DebugMenu_HoennNationalDex },
    { gDebugTextSetRamBerry, DebugMenu_SetRamBerry },
    { gDebugTextUseHm, DebugMenu_UseHM },
    { gDebugTextIwasawa, DebugMenu_OpenIwasawa },
    { gDebugTextToggleBgm, DebugMenu_ToggleBGM },
    { gDebugTextSizeComparison, DebugMenu_OpenSizeComparison },
    { gDebugTextSafari, DebugMenu_Safari },
    { gDebugTextTrainerRematches, DebugMenu_RematchTrainers },
    { gDebugTextMirageIslandRandom, DebugMenu_MiragaIslandRND },
    { gDebugTextHallOfFame, DebugMenu_HallOfFame },
    { gDebugTextMysteryEvent, DebugMenu_OpenMysteryEvent },
    { gDebugTextGameStats, DebugMenu_OpenLegendsRecord },
    { gDebugTextWeatherEvents, DebugMenu_OpenWeatherEvents },
    { gDebugTextOpenCellInfo, DebugMenu_CellInfo },
    { gDebugTextCheckPokeblock, DebugMenu_CheckPKBLCK },
    { gDebugTextEffortValues, DebugMenu_EffortValues },
    { gDebugTextHoennDex, DebugMenu_HoennDex },
    { gDebugTextSeeTrainers, DebugMenu_OpenSeeTrainers },
    { gDebugTextBerryInfo, DebugMenu_OpenBerryInfo },
    { gDebugTextBattleTowerStages, DebugMenu_BattleTowerStages },
    { gDebugTextEndSequenceDemo, DebugMenu_EndSequenceDemo },
    { gDebugTextRandomNumberTest, DebugMenu_RandomNumberTest },
    { gDebugTextMeTooBackupMan, DebugMenu_MeTooBackupMan },
    { gDebugTextMurakawa, DebugMenu_OpenMurakawa },
#if !(ENGLISH && REVISION == 0)
    { gDebugTextKiwaFont, DebugMenu_OpenKiwa },
#endif
};

#if (ENGLISH && REVISION == 0)
const u8 gDebugMenuPageActionOrders[] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x1f, 0x12, 0x13, 0x00, 0x08, 0x09, 0x0a, 0x1e, 0x10, 0x24, 0x0f, 0x00, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00,
    0x1c, 0x1d, 0x14, 0x20, 0x21, 0x22, 0x23, 0x00, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x00, 0x11, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#else
const u8 gDebugMenuPageActionOrders[] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x1f, 0x12, 0x13, 0x00, 0x08, 0x09, 0x0a, 0x1e, 0x10, 0x24, 0x0f, 0x00, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00,
    0x1c, 0x1d, 0x14, 0x20, 0x21, 0x22, 0x23, 0x00, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x00, 0x11, 0x33, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#endif

#if ENGLISH
const u8 gDebugCreatedString[] = _("Created");
const u8 gDebugContinuousString[] = _("Continuous play\n"
                                      "available from");
#else
const u8 gDebugRomString[] = _("Debugging Version");
#endif
const u8 gDebugVersionString[] = _("{VERSION} Version");
const u8 gDebugRTCString[] = _("Normal RTC compatible");

const u8 gTestMenu_TestMenuHeaderText[] = DTR("か　の　じっけん", "TEST MENU"); // 科の実験 (Test section)
const u8 gTestMenu_Test2Text[] = DTR("じっけん2", "Test2");
const u8 gTestMenu_BGTestText[] = DTR("BGじっけん", "BG Test");
const u8 gTestMenu_BattleText[] = _("Battle");
const u8 gTestMenu_UnusedLinkTestText[] = DTR("つうしん　じっけん", "LINK Test"); // unused
const u8 gTestMenu_LinkTest2Text[] = _("LINK Test2");
const u8 gTestMenu_FieldStartText[] = DTR("フィールド　はじめる", "New game"); // "Field start"
const u8 gTestMenu_FieldContinueText[] = DTR("フィールド　つづき", "Continue game"); // "Field continue"
const u8 gTestMenu_SoundTestText[] = _("Sound test");
const u8 gTestMenu_PokeblockTestText[] = _("{POKEBLOCK} test");
const u8 gTestMenu_CrashBackupDataText[] = _("Crash backup data");
const u8 gTestMenu_eCardTestText[] = _("e-Card test");
const u8 gTestMenu_TradeDemoText[] = DTR("こうかんデモ　　　", "Trade demo");
const u8 gTestMenu_TimeInGameText[] = _("Time in game");
const u8 gTestMenu_UnusedFestaModeText[] = DTR("フェスタ　モード", "Festa mode"); // unused
const u8 gTestMenu_MimicECardText[] = _("Mimic e-Card");
const u8 gTestMenu_RTCResetText[] = _("RTC reset");
const u8 gTestMenu_ConverterText[] = _("Converter");

const struct MenuAction gDebugTestMenuActions[] =
{
    { gTestMenu_TestMenuHeaderText,  DebugMenu_TestMenuNoOp }, // Disabled - nop header
    { gTestMenu_Test2Text,           DebugMenu_OpenDisabledTest2 }, // Disabled - freezes game (likely went to tomohara menu
    { gTestMenu_BGTestText,          DebugMenu_BgTestNoOp }, // Disabled - nop
    { gTestMenu_BattleText,          DebugMenu_OpenBattlePartyEditor },
    { gTestMenu_PokeblockTestText,   DebugMenu_OpenBerryBlender },
    { gTestMenu_LinkTest2Text,       DebugMenu_OpenLinkTest },
    { gTestMenu_SoundTestText,       DebugMenu_OpenSoundTest },
    { gTestMenu_FieldStartText,      DebugMenu_OpenFieldStartNameMenu }, // Disabled - starts new game w/o intro with preset name
    { gTestMenu_FieldContinueText,   DebugMenu_ContinueSavedGame }, // Disabled - continues game
    { gTestMenu_CrashBackupDataText, DebugMenu_ClearSaveData },
    { gTestMenu_eCardTestText,       DebugMenu_OpenMysteryEventMenu },
    { gTestMenu_TradeDemoText,       DebugMenu_OpenTradeDemo }, // Disabled - starts a trade then crashes
    { gTestMenu_MimicECardText,      DebugMenu_OpenMysteryEventDebugMenu },
    { gTestMenu_TimeInGameText,      DebugMenu_OpenResetRtcScreen },
    { gTestMenu_RTCResetText,        DebugMenu_OpenRtcReset },
    { gTestMenu_ConverterText,       DebugMenu_OpenSaveConverter },
};

// NOTE: When decompiling this file, I found out that having an extraneous extern
// in a C file can affect the generated asm. If this extern is commented out,
// Task_DebugMenuTestMenuInput will access the array differently and no longer match.
//extern const struct MenuAction gDebug_TestMenuItems[];

const u8 gDebugTestMenuPageActionOrders[][10] =
{
    { 3, 6, 4, 0xFF },
    { 5, 9, 10, 12, 13, 14, 15, 0xFF },
};

const u8 gDebugTestMenuUnusedBytes[] = {2, 0, 0, 0};  // doesn't appear to be referenced

const u8 gDebugTextRtcResetPrompt[] = DTR("RTCを　リセット　します\n"         // Reset RTC
                                          "Aで　じっこう　　　Bでキャンセル",
                                          "Reset RTC?\n"
                                          "A: START    B: CANCEL");

const u8 gDebugTextRtcResetSuccess[] = DTR("RTCを　リセット　した！", "RTC reset successful!");

const u8 gDebugTextSaveConversionPrompt[] = DTR("セーブデータを　コンバートします\n"
                                                  "Aで　けってい　　Bで　キャンセル",
                                                  "Convert save data?\n"
                                                  "A: CONFIRM   B: CANCEL");

const u8 gDebugTextSaveConversionSuccess[] = DTR("へんかんが　しゅうりょう　しました！",
                                                  "Conversion finished!");

extern const u8 gDebugTextRtcResetPrompt[];
extern const u8 gDebugTextRtcResetSuccess[];

static u8 sDebugBgLayerCursorPos;

#ifndef ENGLISH
struct DebugBuildDateInfo {
    s32 max;
    s32 digitMultiplier;
    s32 numDigits;
    s32 offset;
};

static const struct DebugBuildDateInfo *gDebugBuildDate;
#endif
static u8 sDebugMenuCursorPos;
static u8 sDebugMenuPageIndex;
static const u8 *sDebugMenuActionOrder;
static u8 sDebugBgLayerEnableMask;

void DebugMenu_DrawTestMenuPage(u8 page)
{
    s32 r4;

    sDebugMenuActionOrder = gDebugTestMenuPageActionOrders[page];
    for (r4 = 0; sDebugMenuActionOrder[r4] != 0xFF; r4++)
        ;
    Menu_EraseWindowRect(16, 0, 29, 19);
    Menu_DrawStdWindowFrame(16, 0, 29, r4 * 2 + 1);
    Menu_PrintItemsReordered(17, 1, r4, (struct MenuAction2 *)gDebugTestMenuActions, sDebugMenuActionOrder);
    InitMenu(0, 17, 1, r4, 0, 12);
}

void DebugMenu_DestroyTestMenuTask(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DebugMenuTestMenuInput);

    if (taskId != 0xFF)
        DestroyTask(taskId);
}

// Initializes test menu
void DebugMenu_InitTestMenu(void)
{
    Menu_PrintText(gDebugVersionString, 1, 1);
    Menu_PrintText(gDebugRTCString, 1, 3);
#if ENGLISH
    Menu_PrintText(gDebugCreatedString, 1, 7);
    DebugMenu_DisplayBuildDate(3, 9);
    Menu_PrintText(gDebugContinuousString, 1, 12);
    DebugMenu_DisplayContinuousDate(3, 16);
#else
    Menu_PrintText(gDebugRomString, 1, 9);
    DebugMenu_DisplayBuildDate(4, 11);
#endif
    DebugMenu_DrawTestMenuPage(0);
    CreateTask(Task_DebugMenuTestMenuInput, 1);
}

// Handles input for the test menu
void Task_DebugMenuTestMenuInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s8 input = Menu_ProcessInput();
    s8 cursorPos = Menu_GetCursorPos();

    switch (input)
    {
    default:
            gDebugTestMenuActions[sDebugMenuActionOrder[cursorPos]].func();
        break;
    case -2:
        if (JOY_NEW(DPAD_LEFT))
        {
            if (data[0] == 0)
                data[0] = 1;
            else
                data[0]--;
            DebugMenu_DrawTestMenuPage(data[0]);
            PlaySE(SE_SELECT);
        }
        else if (JOY_NEW(DPAD_RIGHT))
        {
            if (data[0] == 1)
                data[0] = 0;
            else
                data[0]++;
            DebugMenu_DrawTestMenuPage(data[0]);
            PlaySE(SE_SELECT);
        }
        break;
    case -1:
        DoSoftReset();
        break;
    }
}

u8 DebugMenu_OpenBattlePartyEditor(void)
{
    SetMainCallback2(InitDebugBattlePartyEditor);
    return 0;
}

u8 DebugMenu_OpenFieldStartNameMenu(void)
{
    DebugMenu_DestroyTestMenuTask();
    DebugMenu_ShowFieldStartNameMenu();
    return 0;
}

u8 DebugMenu_ContinueSavedGame(void)
{
    if (gSaveFileStatus == SAVE_STATUS_ERROR)
    {
        PlaySE(0x16);
    }
    else
    {
        DebugMenu_DestroyTestMenuTask();
        SetMainCallback2(CB2_ContinueSavedGame);
    }
    return 0;
}

u8 DebugMenu_TestMenuNoOp(void)
{
    return 0;
}

u8 DebugMenu_OpenDisabledTest2(void)
{
    SetMainCallback2(CB2_DebugMenuDisabledTest);
    return 0;
}

u8 DebugMenu_BgTestNoOp(void)
{
    return 0;
}

u8 DebugMenu_OpenBerryBlender(void)
{
    SetMainCallback2(CB2_InitBerryBlenderDebugMenu);
    return 0;
}

u8 DebugMenu_OpenMysteryEventMenu(void)
{
    SetMainCallback2(CB2_InitMysteryEventMenu);
    return 0;
}

u8 DebugMenu_OpenMysteryEventDebugMenu(void)
{
    SetMainCallback2(CB2_InitMysteryEventDebugMenu);
    return 0;
}

u8 DebugMenu_OpenTradeDemo(void)
{
    SetMainCallback2(CB2_InitInGameTrade);
    return 0;
}

u8 DebugMenu_OpenLinkTest(void)
{
    SetMainCallback2(LinkTestScreen);
    return 0;
}

u8 DebugMenu_OpenSoundTest(void)
{
    SetMainCallback2(CB2_StartSoundCheckMenu);
    return 0;
}

u8 DebugMenu_ClearSaveData(void)
{
    ClearSaveData();
    return 0;
}

u8 DebugMenu_OpenResetRtcScreen(void)
{
    DebugMenu_DestroyTestMenuTask();
    Debug_StartResetRtcScreen();
    return 0;
}

void Task_DebugMenuConfirmRtcReset(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        Menu_DisplayDialogueFrame();
        Menu_PrintText(gDebugTextRtcResetPrompt, 2, 15);
        data[0]++;
        break;
    case 1:
        if (JOY_NEW(A_BUTTON))
        {
            Menu_DisplayDialogueFrame();
            Menu_PrintText(gDebugTextRtcResetSuccess, 2, 15);
            PlaySE(0x49);
            RtcReset();
            data[0]++;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            DestroyTask(taskId);
            DoSoftReset();
        }
        break;
    case 2:
        if (JOY_NEW(A_BUTTON))
        {
            DestroyTask(taskId);
            DoSoftReset();
        }
        break;
    }
}

u8 DebugMenu_OpenRtcReset(void)
{
    DebugMenu_DestroyTestMenuTask();
    CreateTask(Task_DebugMenuConfirmRtcReset, 10);
    return 0;
}

void Task_DebugMenuConfirmSaveConversion(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
        case 0:
            Menu_DisplayDialogueFrame();
            Menu_PrintText(gDebugTextSaveConversionPrompt, 2, 15);
            data[0]++;
            break;
        case 1:
            if (JOY_NEW(A_BUTTON))
            {
                Menu_DisplayDialogueFrame();
                DebugFixSaveChecksums();
                Menu_PrintText(gDebugTextSaveConversionSuccess, 2, 15);
                PlaySE(SE_DING_DONG);
                data[0]++;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                DestroyTask(taskId);
                DoSoftReset(); // noreturn
            }
            break;
        case 2:
            if (JOY_NEW(A_BUTTON))
            {
                DestroyTask(taskId);
                DoSoftReset(); // noreturn
            }
            break;
    }
}

u8 DebugMenu_OpenSaveConverter(void)
{
    DebugMenu_DestroyTestMenuTask();
    CreateTask(Task_DebugMenuConfirmSaveConversion, 10);
    return 0;
}

// A list of preset player names
const u8 gTestMenuStr_Junichi[] = DTR("ジュンイチ", "JUNICHI");
const u8 gTestMenuStr_Daigoro[] = DTR("ダイゴロウ", "DAIGORO");
const u8 gTestMenuStr_Tetsuji[] = DTR("テツジ",     "TETSUJI");
const u8 gTestMenuStr_Valencia[] = DTR("バレンシア", "VALENCY"); // Valencia is too long
const u8 gTestMenuStr_Harukomachi[] = DTR("ハルコマチ", "MAY");     // May town?
const u8 gTestMenuStr_Raymond[] = _("RAYMOND");
const u8 gTestMenuStr_Tiffany[] = _("TIFFANY");
const u8 gTestMenuStr_KumaXChie[] = DTR("くまxちえ",  "KUMA"); // Chie Kuma?

const struct {
    const u8 * text;
    u32 flags;
} gTestMenu_FieldStartNames[] = {
    {gTestMenuStr_Junichi, 0x0},
    {gTestMenuStr_Daigoro, 0x0},
    {gTestMenuStr_Valencia, 0x1},
    {gTestMenuStr_Harukomachi, 0x1},
    {gTestMenuStr_Raymond, 0x80},
    {gTestMenuStr_Tiffany, 0x81},
    {gTestMenuStr_Tetsuji, 0x0},
    {gTestMenuStr_KumaXChie, 0x1}
};

void DebugMenu_DrawFieldStartNameMenu(void)
{
    u8 i;

    Menu_DrawStdWindowFrame(0, 0, 11, 17);
    Menu_PrintItems(2, 1, ARRAY_COUNT(gTestMenu_FieldStartNames), gTestMenu_FieldStartNames);
    for (i = 0; i < ARRAY_COUNT(gTestMenu_FieldStartNames); i++)
    {
        Menu_PrintChar((gTestMenu_FieldStartNames[i].flags & 1) == MALE ? CHAR_MALE : CHAR_FEMALE, 10, 2 * i + 1);
    }
    InitMenu(0, 1, 1, ARRAY_COUNT(gTestMenu_FieldStartNames), 0, 9);
}

void Task_DebugMenuHandleFieldStartNameMenu(u8 taskId)
{
    s8 input = Menu_ProcessInput();
    s8 cursorPos = Menu_GetCursorPos();

    switch (input)
    {
        case -2:
            break;
        case -1:
            Menu_EraseScreen();
            DebugMenu_InitTestMenu();
            DestroyTask(taskId);
            break;
        default:
        {
            u8 flags = gTestMenu_FieldStartNames[cursorPos].flags;
            gSaveBlock2.playerGender = flags & 1;
            StringCopy(gSaveBlock2.playerName, gTestMenu_FieldStartNames[cursorPos].text);
            if ((flags & 0x80) == 0)
                Debug_NewGameInit(FALSE);
            else
                Debug_NewGameInit(TRUE);
            DestroyTask(taskId);
            SetMainCallback2(CB2_DebugFieldStart);
            break;
        }
    }
}

void DebugMenu_ShowFieldStartNameMenu(void)
{
    DebugMenu_DrawFieldStartNameMenu();
    CreateTask(Task_DebugMenuHandleFieldStartNameMenu, 10);
}

void DebugMenu_SetMainMenuPageActionOrder(void)
{
    sDebugMenuActionOrder = gDebugMenuPageActionOrders + sDebugMenuPageIndex * 8;
}

void Task_DebugMenuWaitForClose(u8 taskId)
{
    if (gMenuCallback() == TRUE)
        DestroyTask(taskId);
}

void DebugMenu_OpenMainMenu(void)
{
    gMenuCallback = DebugMenu_InitMainMenu;
    LockPlayerFieldControls();
    CreateTask(Task_DebugMenuWaitForClose, 80);
}

u8 DebugMenu_InitMainMenu(void)
{
#if (ENGLISH && REVISION == 0)
    gDebugTextModeDisplaySuspended = TRUE;
#endif
    InitMenuWindow(&gMenuTextWindowTemplate);
    DebugMenu_SetMainMenuPageActionOrder();
    DebugMenu_DrawMainMenuPage();
    DebugMenu_DrawMapId();
    DebugMenu_DrawPlayerCoords();
    gMenuCallback = DebugMenu_HandleMainMenuInput;
    return FALSE;
}

u8 DebugMenu_HandleMainMenuInput(void)
{
    u8 actionRet;

    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sDebugMenuCursorPos = Menu_MoveCursor(-1);
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sDebugMenuCursorPos = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        if (sDebugMenuPageIndex == 0)
            sDebugMenuPageIndex = 7;
        else
            sDebugMenuPageIndex--;
        DebugMenu_SetMainMenuPageActionOrder();
        DebugMenu_DrawMainMenuPage();
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        if (sDebugMenuPageIndex == 7)
            sDebugMenuPageIndex = 0;
        else
            sDebugMenuPageIndex++;
        DebugMenu_SetMainMenuPageActionOrder();
        DebugMenu_DrawMainMenuPage();
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        actionRet = gDebugMenuActions[sDebugMenuActionOrder[sDebugMenuCursorPos]].func();
// TODO: see if this is in rev1+
#if (ENGLISH && REVISION == 0)
        if (actionRet == 1)
            gDebugTextModeDisplaySuspended = FALSE;
#endif
        return actionRet;
    }
    if (JOY_NEW(B_BUTTON | START_BUTTON))
    {
        CloseMenu();
// TODO: see if this is in rev1+
#if (ENGLISH && REVISION == 0)
        gDebugTextModeDisplaySuspended = FALSE;
#endif
        return TRUE;
    }
    return FALSE;
}

void DebugMenu_DrawMainMenuPage(void)
{
    Menu_DrawStdWindowFrame(0, 0, 14, 17);
    Menu_PrintItemsReordered(1, 1, 8, gDebugMenuActions, sDebugMenuActionOrder);
    sDebugMenuCursorPos = InitMenu(0, 1, 1, 8, sDebugMenuCursorPos, 13);
}

const u8 gDebugTextMapId[] = _("ID");

void DebugMenu_DrawMapId(void)
{
    u8 * buf = gStringVar4;
    buf = StringCopy(buf, gDebugTextMapId);
    buf = ConvertIntToDecimalStringN(buf, gSaveBlock1.location.mapGroup, STR_CONV_MODE_RIGHT_ALIGN, 3);
    buf = ConvertIntToDecimalStringN(buf, gSaveBlock1.location.mapNum, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_DrawStdWindowFrame(19, 12, 29, 15);
    Menu_PrintText(gStringVar4, 20, 13);
}

void DebugMenu_DrawPlayerCoords(void)
{
    u8 sp00[] = _("X");
    u8 sp04[] = _(" Y");
    u8 sp08[] = _(" H");
    u8 sp0c[4];

    s16 x;
    s16 y;
    u8 z;
    u16 sp14 = 0;

    PlayerGetDestCoords(&x, &y);
    z = MapGridGetElevationAt(x, y);
    x -= 7;
    y -= 7;

    StringCopy(gStringVar4, sp00);
    ConvertIntToDecimalStringN(sp0c, x, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringAppend(gStringVar4, sp0c);
    StringAppend(gStringVar4, sp04);
    ConvertIntToDecimalStringN(sp0c, y, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringAppend(gStringVar4, sp0c);
    StringAppend(gStringVar4, sp08);
    ConvertIntToDecimalStringN(sp0c, z, STR_CONV_MODE_LEFT_ALIGN, 2);
    StringAppend(gStringVar4, sp0c);

    for (sp14 = 0; gStringVar4[sp14] != EOS; sp14++)
        ;

    Menu_DrawStdWindowFrame(28 - sp14, 16, 29, 19);
    Menu_PrintText(gStringVar4, 29 - sp14, 17);
}

u8 DebugMenu_Exit(void)
{
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_OpenSogabe(void)
{
    return InitSogabeDebugMenu();
}

u8 DebugMenu_OpenTamada(void)
{
    return DebugMenu_StartDmaTimingTest();
}

u8 DebugMenu_OpenKagaya(void)
{
    return InitKagayaDebugMenu();
}

u8 DebugMenu_OpenMatsuda(void)
{
    return InitMatsudaDebugMenu();
}

u8 DebugMenu_OpenNohara(void)
{
    return InitNoharaDebugMenu();
}

u8 DebugMenu_OpenWatanabe(void)
{
    CloseMenu();
    SetMainCallback2(InitWatanabeDebugMenu);
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_EndSequenceDemo(void)
{
    CloseMenu();
    SetMainCallback2(CB2_StartCreditsSequence);
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_HallOfFame(void)
{
    CloseMenu();
    GameClear();
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_OpenSizeComparison(void)
{
    CloseMenu();
    InitSizeComparison();
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_HoennNationalDex(void)
{
    if (IsNationalPokedexEnabled())
        DisableNationalPokedex();
    else
        EnableNationalPokedex();
    CloseMenu();
    return TRUE;
}

const u8 gDebugTextEnigmaBerryHp[] = _("HP");
const u8 gDebugTextEnigmaBerryParalysis[] = _("PAR");
const u8 gDebugTextEnigmaBerrySleep[] = _("SLP");
const u8 gDebugTextEnigmaBerryPoison[] = _("PSN");
const u8 gDebugTextEnigmaBerryUmai[] = KANA("ウマイ");

u8 DebugMenu_SetEnigmaBerryPreset(void)
{
    switch (Menu_GetCursorPos())
    {
    case 0:
        DebugCreateEnigmaBerry(gDebugTextEnigmaBerryHp, 1, 4);
            DebugSetEnigmaBerryFlavors(0xff, 0xff, 0xff, 0, 0xff, 0xff);
            break;
        case 1:
        DebugCreateEnigmaBerry(gDebugTextEnigmaBerryParalysis, 2, 0);
            DebugSetEnigmaBerryFlavors(0xff, 0xff, 0, 0xff, 0xff, 0xff);
            break;
        case 2:
        DebugCreateEnigmaBerry(gDebugTextEnigmaBerrySleep, 3, 0);
            DebugSetEnigmaBerryFlavors(0xff, 0, 0xff, 0xff, 0xff, 0xff);
            break;
        case 3:
        DebugCreateEnigmaBerry(gDebugTextEnigmaBerryPoison, 4, 0);
            DebugSetEnigmaBerryFlavors(0, 0xff, 0xff, 0xff, 0xff, 0xff);
            break;
        case 4:
        DebugCreateEnigmaBerry(gDebugTextEnigmaBerryUmai, 4, 0);
            DebugSetEnigmaBerryFlavors(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
            break;
    }
    CloseMenu();
    return TRUE;
}

const struct MenuAction gDebugMenuEnigmaBerryPresetActions[] = {
    {gDebugTextEnigmaBerryHp, DebugMenu_SetEnigmaBerryPreset},
    {gDebugTextEnigmaBerryParalysis, DebugMenu_SetEnigmaBerryPreset},
    {gDebugTextEnigmaBerrySleep, DebugMenu_SetEnigmaBerryPreset},
    {gDebugTextEnigmaBerryPoison, DebugMenu_SetEnigmaBerryPreset},
    {gDebugTextEnigmaBerryUmai, DebugMenu_SetEnigmaBerryPreset}
};

u8 DebugMenu_HandleEnigmaBerryPresetInput(void)
{
    return DebugMenu_ProcessActionMenuInput(gDebugMenuEnigmaBerryPresetActions);
}

u8 DebugMenu_SetRamBerry(void)
{
    DebugMenu_DrawActionMenu(gDebugMenuEnigmaBerryPresetActions, 12, ARRAY_COUNT(gDebugMenuEnigmaBerryPresetActions));
    gMenuCallback = DebugMenu_HandleEnigmaBerryPresetInput;
    return FALSE;
}

u8 DebugMenu_ToggleBGM(void)
{
    gDisableMusic = gDisableMusic ? FALSE : TRUE;
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_BattleForDebug(void)
{
    CloseMenu();
    InitBattleForDebug();
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_NationalDex(void)
{
    u16 i;

    for (i = 0; i < 386; i++)
    {
        GetSetPokedexFlag(i + 1, 2);
        GetSetPokedexFlag(i + 1, 3);
    }
    gUnusedPokedexU8 = TRUE;
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_HoennDex(void)
{
    u16 i;

    for (i = 0; i < 202; i++)
    {
        u16 nati = HoennToNationalOrder(i + 1);
        GetSetPokedexFlag(nati, 2);
        GetSetPokedexFlag(nati, 3);
    }
    gUnusedPokedexU8 = TRUE;
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_CreatePKMN(void)
{
    CloseMenu();
    InitCreatePokemon();
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_ViewPokemonGraphics(void)
{
    CloseMenu();
    InitSeePokemonGraphics();
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_OpenSeeTrainers(void)
{
    CloseMenu();
    InitSeeTrainers();
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_OpenMori(void)
{
    return InitMoriDebugMenu();
}

u8 DebugMenu_OpenTomomichi(void)
{
    return InitTomomichiDebugWindow();
}

u8 DebugMenu_OpenAoki(void)
{
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_OpenTaya(void)
{
    return InitTayaDebugWindow();
}

u8 DebugMenu_OpenNakamura(void)
{
    return InitNakamuraDebugMenu();
}

u8 DebugMenu_OpenIwasawa(void)
{
    return DebugOpenMiscMenu();
}

u8 DebugMenu_Teleport(void)
{
    Overworld_SetWarpDestToLastHealLoc();
    DoWarp();
    LockPlayerFieldControls();
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_EditPKMN(void)
{
    Menu_EraseScreen();
    NakaGenderTest();
    return FALSE;
}

void DebugMenu_DrawBgLayerToggle(u8 bgNum)
{
    u8 sp00[] = __("OFF$"
                   "ON$");
    Menu_BlankWindowRect(25, bgNum * 2, 29, bgNum * 2 + 1);
    Menu_PrintText(sp00 + 4 * ((sDebugBgLayerEnableMask >> bgNum) & 1), 25, bgNum * 2);
}

void DebugMenu_DrawBgLayerToggleMenu(void)
{
    u8 sp00[] = _("BG0\n"
                  "BG1\n"
                  "BG2\n"
                  "BG3\n");
    Menu_BlankWindowRect(19, 0, 29, 8);
    Menu_PrintText(sp00, 21, 0);
    sDebugBgLayerEnableMask = (REG_DISPCNT >> 8) & 0x0F;
    DebugMenu_DrawBgLayerToggle(0);
    DebugMenu_DrawBgLayerToggle(1);
    DebugMenu_DrawBgLayerToggle(2);
    DebugMenu_DrawBgLayerToggle(3);
}

u8 DebugMenu_HandleBgLayerToggleInput(void)
{
    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
        if (sDebugBgLayerCursorPos != 0)
            sDebugBgLayerCursorPos--;
        else
            sDebugBgLayerCursorPos = 3;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(+1);
        if (sDebugBgLayerCursorPos != 3)
            sDebugBgLayerCursorPos++;
        else
            sDebugBgLayerCursorPos = 0;
    }
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        sDebugBgLayerEnableMask ^= (1 << Menu_GetCursorPos());
        DebugMenu_DrawBgLayerToggle(Menu_GetCursorPos());
    }
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        REG_DISPCNT = (REG_DISPCNT & 0xF0FF) + (sDebugBgLayerEnableMask << 8);
        CloseMenu();
        return TRUE;
    }
    return FALSE;
}

u8 DebugMenu_SwitchBG(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawBgLayerToggleMenu();
    InitMenu(0, 20, 0, 4, 0, 6);
    gMenuCallback = DebugMenu_HandleBgLayerToggleInput;
    return FALSE;
}

void DebugMenu_DrawOnOffToggle(u8 state)
{
    u8 sp00[] = __("ON$"
                   "OFF$");

    Menu_DrawStdWindowFrame(25, 0, 29, 3);
    Menu_PrintText(sp00 + 3 * state, 26, 1);
}

u8 DebugMenu_HandleWildEncounterToggle(void)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        gWildEncountersDisabled ^= 1;
        DebugMenu_DrawOnOffToggle(gWildEncountersDisabled);
    }
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        CloseMenu();
        return TRUE;
    }
    return FALSE;
}

u8 DebugMenu_ControlEncounter(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawOnOffToggle(gWildEncountersDisabled);
    gMenuCallback = DebugMenu_HandleWildEncounterToggle;
    return FALSE;
}

u8 DebugMenu_UseHM()
{
    gMenuCallback = DebugMenu_OpenFieldMoveMenu;
    return FALSE;
}

u8 DebugMenu_SetAllTrainerRematchFlags(void)
{
    int i;

    for (i = 0; i < ARRAY_COUNT(gSaveBlock1.trainerRematches); i++)
        gSaveBlock1.trainerRematches[i] = 1;
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_ClearAllTrainerRematchFlags(void)
{
    int i;

    for (i = 0; i < ARRAY_COUNT(gSaveBlock1.trainerRematches); i++)
        gSaveBlock1.trainerRematches[i] = 0;
    CloseMenu();
    return TRUE;
}

const u8 gDebugTextTrainerRematchStatus[] = DTR("さいせんかのうトレーナー:{STR_VAR_1}\n"
                                                  "さいせんじょうたいトレーナー:{STR_VAR_2}",
                                                  "Rematchable TRAINERS: {STR_VAR_1}\n"
                                                  "TRAINERS in rematch state: {STR_VAR_2}");
const u8 gDebugTextOn[] = DTR("いる", "ON");
const u8 gDebugTextOff[] = DTR("いない", "OFF");

bool8 DebugMenu_ShowTrainerRematchStatus(void)
{
    Menu_EraseScreen();
    if (IsRematchTrainerIn(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum))
        StringCopy(gStringVar1, gDebugTextOn);
    else
        StringCopy(gStringVar1, gDebugTextOff);
    if (DoesSomeoneWantRematchIn(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum))
        StringCopy(gStringVar2, gDebugTextOn);
    else
        StringCopy(gStringVar2, gDebugTextOff);
    Menu_PrintTextWithStdFrame(gDebugTextTrainerRematchStatus);
    gMenuCallback = DebugMenu_WaitForAButton;
    return FALSE;
}

bool8 DebugMenu_SetTrainerRematchCounterMax(void)
{
    gSaveBlock1.trainerRematchStepCounter = 255;
    CloseMenu();
    return TRUE;
}

bool8 DebugMenu_ClearTrainerRematchCounter(void)
{
    gSaveBlock1.trainerRematchStepCounter = 0;
    CloseMenu();
    return TRUE;
}

const u8 gDebugTextSetRematchCounterMax[] = _("Max num of steps");
const u8 gDebugTextClearRematchCounter[] = _("Clear num of steps");
const u8 gDebugTextShowRematchTrainers[] = _("See trainers");
const u8 gDebugTextClearAllRematchFlags[] = _("Flag all clear");
const u8 gDebugTextSetAllRematchFlags[] = _("Flag all set");

const struct MenuAction gDebugMenuTrainerRematchActions[] = {
    {gDebugTextShowRematchTrainers, DebugMenu_ShowTrainerRematchStatus},
    {gDebugTextSetRematchCounterMax, DebugMenu_SetTrainerRematchCounterMax},
    {gDebugTextClearRematchCounter, DebugMenu_ClearTrainerRematchCounter},
    {gDebugTextSetAllRematchFlags, DebugMenu_SetAllTrainerRematchFlags},
    {gDebugTextClearAllRematchFlags, DebugMenu_ClearAllTrainerRematchFlags}
};

u8 DebugMenu_HandleTrainerRematchMenuInput(void)
{
    return DebugMenu_ProcessActionMenuInput(gDebugMenuTrainerRematchActions);
}

u8 DebugMenu_RematchTrainers(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawActionMenu(gDebugMenuTrainerRematchActions, 13, ARRAY_COUNT(gDebugMenuTrainerRematchActions));
    gMenuCallback = DebugMenu_HandleTrainerRematchMenuInput;
    return FALSE;
}

bool8 DebugMenu_SetSafariBalls30(void)
{
    gNumSafariBalls = 30;
    CloseMenu();
    return TRUE;
}

bool8 DebugMenu_SetSafariBalls1(void)
{
    gNumSafariBalls = 1;
    CloseMenu();
    return TRUE;
}

bool8 DebugMenu_SetSafariSteps500(void)
{
    gSafariZoneStepCounter = 500;
    CloseMenu();
    return TRUE;
}

bool8 DebugMenu_SetSafariSteps1(void)
{
    gSafariZoneStepCounter = 1;
    CloseMenu();
    return TRUE;
}

const u8 gDebugTextSafariRemainingSteps[] = DTR("のこり　{STR_VAR_1}ほ", "Remaining steps: {STR_VAR_1}");
const u8 gDebugTextSafariFeederStatus[] = DTR("{STR_VAR_1}\n"
                                                "のこり　{STR_VAR_2}ほ",
                                                "{STR_VAR_1}\n"
                                                "Remaining steps: {STR_VAR_2}");
const u8 gDebugTextNoPokeblockAtFeeder[] = DTR("めのまえには\n"
                                                "キューブが　ありません！",
                                                "There is no {POKEBLOCK}\n"
                                                "in front of you!");

bool8 DebugMenu_ShowSafariFeederStatus(void)
{
    Menu_EraseScreen();
    if (GetInFrontFeederPokeblockAndSteps())
        Menu_PrintTextWithStdFrame(gDebugTextSafariFeederStatus);
    else
        Menu_PrintTextWithStdFrame(gDebugTextNoPokeblockAtFeeder);
    gMenuCallback = DebugMenu_WaitForAButton;
    return FALSE;
}

const u8 gDebugTextSafariOneStepRemaining[] = DTR("ほすうをのこり1にする", "One step remaining");
const u8 gDebugTextSafariOneBallRemaining[] = DTR("ボールをのこり1にする", "One BALL remaining");
const u8 gDebugTextSafariPokeblockFeeder[] = DTR("キューブおきば", "{POKEBLOCK} FEEDER");
const u8 gDebugTextSafariThirtyBallsRemaining[] = DTR("ボールをのこり30に", "30 BALLS remaining");
const u8 gDebugTextSafariFiveHundredStepsRemaining[] = DTR("ほすうをのこり500に", "500 steps remaining");

const struct MenuAction gDebugMenuSafariActions[] = {
    {gDebugTextSafariPokeblockFeeder, DebugMenu_ShowSafariFeederStatus},
    {gDebugTextSafariFiveHundredStepsRemaining, DebugMenu_SetSafariSteps500},
    {gDebugTextSafariOneStepRemaining, DebugMenu_SetSafariSteps1},
    {gDebugTextSafariThirtyBallsRemaining, DebugMenu_SetSafariBalls30},
    {gDebugTextSafariOneBallRemaining, DebugMenu_SetSafariBalls1}
};

bool8 DebugMenu_HandleSafariMenuInput(void)
{
    return DebugMenu_ProcessActionMenuInput(gDebugMenuSafariActions);
}

u8 DebugMenu_Safari(void)
{
    if (!GetSafariZoneFlag())
    {
        CloseMenu();
        PlaySE(SE_BOO);
        return TRUE;
    }
    Menu_EraseScreen();
    DebugMenu_DrawActionMenu(gDebugMenuSafariActions, 14, ARRAY_COUNT(gDebugMenuSafariActions));
    ConvertIntToDecimalStringN(gStringVar1, gSafariZoneStepCounter, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintTextWithStdFrame(gDebugTextSafariRemainingSteps);
    gMenuCallback = DebugMenu_HandleSafariMenuInput;
    LockPlayerFieldControls();
    return FALSE;
}

bool8 DebugMenu_UseCut(void)
{
    Debug_SetUpFieldMove_Cut();
    return TRUE;
}

bool8 DebugMenu_UseFlash(void)
{
    Debug_UseFlashInCave();
    return TRUE;
}

bool8 DebugMenu_UseRockSmash(void)
{
    Debug_UseRockSmashInFrontOfPlayer();
    return TRUE;
}

bool8 DebugMenu_UseStrength(void)
{
    Debug_UseStrength();
    return TRUE;
}

bool8 DebugMenu_UseWaterfall(void)
{
    Debug_TryUseWaterfall();
    CloseMenu();
    return TRUE;
}

bool8 DebugMenu_UseSecretPower(void)
{
    Debug_SetUpFieldMove_SecretPower();
    return TRUE;
}

const struct MenuAction gDebugMenuFieldMoveActions[] = {
    {gMoveNames[MOVE_CUT], DebugMenu_UseCut},
    {gMoveNames[MOVE_FLASH], DebugMenu_UseFlash},
    {gMoveNames[MOVE_ROCK_SMASH], DebugMenu_UseRockSmash},
    {gMoveNames[MOVE_STRENGTH], DebugMenu_UseStrength},
    {gMoveNames[MOVE_SURF], Debug_TryUseSurf},
    {gMoveNames[MOVE_FLY], Debug_OpenFlyMap},
    {gMoveNames[MOVE_WATERFALL], DebugMenu_UseWaterfall},
    {gMoveNames[MOVE_DIVE], Debug_TryUseDive},
    {gMoveNames[MOVE_SECRET_POWER], DebugMenu_UseSecretPower}
};

bool8 DebugMenu_HandleFieldMoveMenuInput(void)
{
    return DebugMenu_ProcessActionMenuInput(gDebugMenuFieldMoveActions);
}

u8 DebugMenu_OpenFieldMoveMenu(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawActionMenu(gDebugMenuFieldMoveActions, 10, ARRAY_COUNT(gDebugMenuFieldMoveActions));
    gMenuCallback = DebugMenu_HandleFieldMoveMenuInput;
    return FALSE;
}


const u8 gDebugTextPlayTimeHeader[] = DTR("　じかん　ふん　びょう", "  HOURS MINUTES SECONDS");

#if ENGLISH
const u8 gDebugContinuousDateTime[] = "2002 08 01 20:25";
#else
static const struct DebugBuildDateInfo gDebugBuildDateInfo[] = {
    { 99, 10, 2, 23 },
    { 99, 10, 2, 26 }
};
#endif

// Parses the version code in a highly inefficient and unsafe way.
void DebugMenu_ConvertBuildDate(const u8 *buildDateStr, u8 *out)
{
    s32 i;
#if ENGLISH
    u16 year;
    u16 month;
    u16 day;
    u16 hour;
    u16 minutes;

    for (year = 0, i = 0; i < 2; i++)
    {
        year *= 10;
        year += (buildDateStr[i+2] - '0');
    }

    month = (buildDateStr[5] - '0') * 10 + (buildDateStr[6] - '0');
    day = (buildDateStr[8] - '0') * 10 + (buildDateStr[9] - '0');
    hour = (buildDateStr[11] - '0') * 10 + (buildDateStr[12] - '0');
    minutes = (buildDateStr[14] - '0') * 10 + (buildDateStr[15] - '0');

    out = ConvertIntToDecimalStringN(out, year, STR_CONV_MODE_LEADING_ZEROS, 2);
    *out++ = CHAR_SLASH;
    out = ConvertIntToDecimalStringN(out, month, STR_CONV_MODE_LEADING_ZEROS, 2);
    *out++ = CHAR_SLASH;
    out = ConvertIntToDecimalStringN(out, day, STR_CONV_MODE_LEADING_ZEROS, 2);
    *out++ = CHAR_SPACE;
    out = ConvertIntToDecimalStringN(out, hour, STR_CONV_MODE_LEADING_ZEROS, 2);
    *out++ = CHAR_COLON;
    out = ConvertIntToDecimalStringN(out, minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
#else
    // Prevents register allocation swap
    // Intended: r7 = out, r6 = gDebugBuildDate->numDigits
    // Observed: r6 = out, r7 = gDebugBuildDate->numDigits
    ++out; --out;

    gDebugBuildDate = gDebugBuildDateInfo;
    *out++ = CHAR_SPACE;
    *out++ = CHAR_SPACE;
    for (i = 0; i < 2; i++, gDebugBuildDate++)
    {
        s32 j;
        s32 mul;
        u32 date = 0;
        for (mul = gDebugBuildDate->digitMultiplier, j = 0; j < gDebugBuildDate->numDigits; j++, mul /= 10)
            date += (buildDateStr[gDebugBuildDate->offset + j] - '0') * mul;
        if (gDebugBuildDate->max < date)
            date = gDebugBuildDate->max;
        out = ConvertIntToDecimalStringN(out, date, STR_CONV_MODE_LEADING_ZEROS, gDebugBuildDate->numDigits);
    }
    out[0] = buildDateStr[29] - 'A' + CHAR_A;
    if (out[0] > 0xf6) // prevent special characters
        out[0] = 0xf6;
    out[1] = EOS;
#endif
}

void DebugMenu_DisplayBuildDate(u8 x, u8 y)
{
    DebugMenu_ConvertBuildDate(BuildDateTime, gStringVar4);
    Menu_PrintText(gStringVar4, x, y);
}

#if ENGLISH
void DebugMenu_DisplayContinuousDate(u8 x, u8 y)
{
    DebugMenu_ConvertBuildDate(gDebugContinuousDateTime, gStringVar4);
    Menu_PrintText(gStringVar4, x, y);
}
#endif

void DebugMenu_DrawActionMenu(const struct MenuAction *menuAction, u8 width, u8 itemCount)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, width, 2 * itemCount + 1);
    Menu_PrintItems(1, 1, itemCount, menuAction);
    InitMenu(0, 1, 1, itemCount, 0, width - 1);
}

u8 DebugMenu_ProcessActionMenuInput(const struct MenuAction *menuActions)
{
    s8 input = Menu_ProcessInput();
    switch (input)
    {
        default:
            Menu_EraseScreen();
            gMenuCallback = menuActions[input].func;
            return FALSE;
        case -2:
            return FALSE;
        case -1:
            CloseMenu();
            return TRUE;
    }
}

bool8 DebugMenu_WaitForAButton(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        CloseMenu();
        return TRUE;
    }
    return FALSE;
}

bool32 DebugMenu_AdjustValue(s16 *value, s32 min, s32 max, u16 keys)
{
    if (keys & DPAD_DOWN)
    {
        *value -= 1;
        if (*value < min)
            *value = max;
    }
    else if (keys & DPAD_UP)
    {
        *value += 1;
        if (*value > max)
            *value = min;
    }
    else if (keys & DPAD_LEFT)
    {
        *value -= 10;
        if (*value < min)
            *value = max;
    }
    else if (keys & DPAD_RIGHT)
    {
        *value += 10;
        if (*value > max)
            *value = min;
    }
    else
        return FALSE;
    return TRUE;
}

int DebugMenu_HandleOnOffToggle(int flag)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        CloseMenu();
        return -1;
    }
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        if (flag)
        {
            DebugMenu_DrawOnOffToggle(1);
            return 0;
        }
        else
        {
            DebugMenu_DrawOnOffToggle(0);
            return 1;
        }
    }
    return flag;
}

u8 DebugMenu_MakeItems()
{
    CloseMenu();
    DebugMenu_OpenItemGrantMenu();
    LockPlayerFieldControls();
    return TRUE;
}

void DebugMenu_OpenItemGrantMenu(void)
{
    u8 taskId = CreateTask(Task_DebugMenuInitItemGrant, 80);
    Menu_EraseScreen();
    gTasks[taskId].data[1] = 1;
    gTasks[taskId].data[2] = 1;
}

void DebugMenu_PrintItemGrantSelection(u16 itemId, u16 quantity)
{
    Menu_BlankWindowRect(4, 17, 22, 18);
    ConvertIntToDecimalStringN(gStringVar1, itemId, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintText(gStringVar1, 4, 17);
    Menu_PrintText(GetItemName(itemId), 8, 17);
    ConvertIntToDecimalStringN(gStringVar1, quantity, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintText(gStringVar1, 18, 17);
}

const u8 gDebugTextWhichItem[] = _("Which item?");

void Task_DebugMenuInitItemGrant(u8 taskId)
{
    struct Task *task = gTasks + taskId;
    Menu_DisplayDialogueFrame();
    Menu_PrintText(gDebugTextWhichItem, 2, 15);
    DebugMenu_PrintItemGrantSelection(task->data[1], task->data[2]);
    task->func = Task_DebugMenuSelectItem;
}

const u8 gDebugTextItemQuantity[] = _("How many？");

void Task_DebugMenuSelectItem(u8 taskId)
{
    struct Task *task = gTasks + taskId;
    if (JOY_NEW(A_BUTTON))
    {
        Menu_DisplayDialogueFrame();
        Menu_PrintText(gDebugTextItemQuantity, 2, 15);
        DebugMenu_PrintItemGrantSelection(task->data[1], task->data[2]);
        task->func = Task_DebugMenuSelectItemQuantity;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        task->func = Task_DebugMenuCloseItemGrant;
    }
    else if (DebugMenu_AdjustValue(task->data + 1, 1, ITEM_15C, gMain.newAndRepeatedKeys) == TRUE)
    {
        DebugMenu_PrintItemGrantSelection(task->data[1], task->data[2]);
    }
}

void Task_DebugMenuSelectItemQuantity(u8 taskId)
{
    struct Task *task = gTasks + taskId;
    if (JOY_NEW(A_BUTTON))
    {
        if (AddBagItem(task->data[1], task->data[2]) == TRUE)
            PlaySE(SE_SELECT);
        task->func = Task_DebugMenuInitItemGrant;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        task->func = Task_DebugMenuInitItemGrant;
    }
    else if (DebugMenu_AdjustValue(task->data + 2, 1, 99, gMain.newAndRepeatedKeys) == TRUE)
    {
        DebugMenu_PrintItemGrantSelection(task->data[1], task->data[2]);
    }
}

void Task_DebugMenuCloseItemGrant(u8 taskId)
{
    Menu_EraseScreen();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
    DebugMenu_OpenMainMenu();
}

const u8 gDebugTextContestWinnerInfo[] = DTR("にがおえ　ばんごう\n"
                             "ポケモンナンバー\n"
                             "ポケモンめい\n"
                             "ブリーダーめい\n"
                             "しゅるい\n"
                             "こせいらんすう",
                             "PORTRAIT No.\n"
                             "POKéMON No.\n"
                             "POKéMON Name\n"
                             "BREEDER Name\n" // TRN ?
                             "TYPE\n"
                             "Personality");
const u8 gDebugTextNoContestWinnerData[] = DTR("データなし", "No data");

void DebugMenu_DrawContestWinnerInfo(u32 winnerId)
{
    u8 strbuf[11];
    struct ContestWinner *winner;

    if (winnerId == 0 || winnerId > 13)
        winnerId = 1;
    winner = gSaveBlock1.contestWinners + winnerId - 1;

    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 19, 17);
    Menu_PrintText(gDebugTextContestWinnerInfo, 2, 2);

    ConvertIntToDecimalStringN(strbuf, winnerId, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintText(strbuf, 11, 2);

    ConvertIntToDecimalStringN(strbuf, winner->species, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintText(strbuf, 11, 4);

    if (StringLength(winner->nickname) <= 11)
        Menu_PrintText(winner->nickname, 11, 6);
    else
        Menu_PrintText(gDebugTextNoContestWinnerData, 11, 6);

    if (StringLength(winner->trainerName) <= 11)
        Menu_PrintText(winner->trainerName, 11, 8);
    else
        Menu_PrintText(gDebugTextNoContestWinnerData, 11, 8);

    ConvertIntToDecimalStringN(strbuf, winner->contestCategory, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintText(strbuf, 11, 10);

    ConvertIntToHexStringN(strbuf, winner->personality, STR_CONV_MODE_LEADING_ZEROS, 8);
    Menu_PrintText(strbuf, 11, 12);
}

void Task_DebugMenuCloseContestWinnerInfo(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        Menu_EraseScreen();
        DestroyTask(taskId);
        UnlockPlayerFieldControls();
    }
}

void Task_DebugMenuSelectContestWinner(u8 taskId)
{
    struct Task *task = gTasks + taskId;

    if (task->data[0] == 0)
        task->data[0]++;
    else if (JOY_NEW(A_BUTTON))
    {
        DebugMenu_DrawContestWinnerInfo(task->data[0]);
        task->func = Task_DebugMenuCloseContestWinnerInfo;
        return;
    }
    else if (!DebugMenu_AdjustValue(task->data + 0, 1, 13, JOY_REPT(DPAD_UP | DPAD_DOWN)))
        return;
    Menu_BlankWindowRect(4, 17, 22, 18);
    ConvertIntToDecimalStringN(gStringVar1, task->data[0], STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintText(gStringVar1, 4, 17);
}

u8 DebugMenu_ViewPortraits(void)
{
    CloseMenu();
    CreateTask(Task_DebugMenuSelectContestWinner, 80);
    LockPlayerFieldControls();
    return TRUE;
}

void DebugMenu_ShowContestWinnerInfo(u32 winnerId)
{
    DebugMenu_DrawContestWinnerInfo(winnerId);
}

u8 DebugMenu_AllBadges(void)
{
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    FlagSet(FLAG_BADGE03_GET);
    FlagSet(FLAG_BADGE04_GET);
    FlagSet(FLAG_BADGE05_GET);
    FlagSet(FLAG_BADGE06_GET);
    FlagSet(FLAG_BADGE07_GET);
    FlagSet(FLAG_BADGE08_GET);
    CloseMenu();
    return TRUE;
}

u8 DebugMenu_TimeRecords(void)
{
    CloseMenu();
    CreateTask(Task_DebugDisplayRtcInfo, 80);
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_SetTime(void)
{
    CloseMenu();
    Debug_StartSetRtcTime();
    LockPlayerFieldControls();
    return TRUE;
}

const u8 gDebugTextMirageIslandRandomNumber[] = _("Miracle Island emerged\n"
                                                    "Random numbers:{STR_VAR_1}");

void DebugMenu_ShowMirageIslandRandomNumber(u16 randomNumber)
{
    ConvertIntToHexStringN(gStringVar1, randomNumber, STR_CONV_MODE_LEFT_ALIGN, 4);
    Menu_PrintTextWithStdFrame(gDebugTextMirageIslandRandomNumber);
}

void Task_DebugMenuEditMirageIslandRandomNumber(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
        case 0:
            data[1] = VarGet(VAR_MIRAGE_RND_H);
            DebugMenu_ShowMirageIslandRandomNumber(data[1]);
            data[0]++;
            break;
        case 1:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_DING_DONG);
                *GetVarPointer(VAR_MIRAGE_RND_H) = data[1];
                Menu_EraseScreen();
                DestroyTask(taskId);
                UnlockPlayerFieldControls();
            }
            else if (JOY_NEW(B_BUTTON))
            {
                Menu_EraseScreen();
                DestroyTask(taskId);
                UnlockPlayerFieldControls();
            }
            else if (DebugMenu_AdjustValue(data + 1, 0, 0xFFFF, gMain.newAndRepeatedKeys) == TRUE)
                DebugMenu_ShowMirageIslandRandomNumber(data[1]);
            break;
    }
}

u8 DebugMenu_MiragaIslandRND(void)
{
    CloseMenu();
    CreateTask(Task_DebugMenuEditMirageIslandRandomNumber, 80);
    LockPlayerFieldControls();
    return TRUE;
}

bool8 DebugMenu_HandleGameClearFlagToggle(void)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        if (!FlagGet(FLAG_SYS_GAME_CLEAR))
        {
            FlagSet(FLAG_SYS_GAME_CLEAR);
            DebugMenu_DrawOnOffToggle(0);
        }
        else
        {
            FlagClear(FLAG_SYS_GAME_CLEAR);
            DebugMenu_DrawOnOffToggle(1);
        }
    }
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        CloseMenu();
        return TRUE;
    }
    return FALSE;
}

u8 DebugMenu_ToggleClearFlag(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawOnOffToggle(FlagGet(FLAG_SYS_GAME_CLEAR) ^ 1);
    gMenuCallback = DebugMenu_HandleGameClearFlagToggle;
    return FALSE;
}

bool8 DebugMenu_HandleWeatherEventsToggle(void)
{
    switch (DebugMenu_HandleOnOffToggle(FlagGet(FLAG_SYS_WEATHER_CTRL)))
    {
        case 0:
            FlagClear(FLAG_SYS_WEATHER_CTRL);
            return FALSE;
        case 1:
            FlagSet(FLAG_SYS_WEATHER_CTRL);
            return FALSE;
        case -1:
            return TRUE;
    }
    return FALSE;
}

u8 DebugMenu_OpenWeatherEvents(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawOnOffToggle(FlagGet(FLAG_SYS_WEATHER_CTRL) ? FALSE : TRUE);
    gMenuCallback = DebugMenu_HandleWeatherEventsToggle;
    return FALSE;
}

bool8 DebugMenu_HandleMysteryEventToggle(void)
{
    switch (DebugMenu_HandleOnOffToggle(IsMysteryEventEnabled()))
    {
        case 0:
            DisableMysteryEvent();
            return FALSE;
        case 1:
            EnableMysteryEvent();
            return FALSE;
        case -1:
            return TRUE;
    }
    return FALSE;
}

u8 DebugMenu_OpenMysteryEvent(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawOnOffToggle(IsMysteryEventEnabled() ? FALSE : TRUE);
    gMenuCallback = DebugMenu_HandleMysteryEventToggle;
    return FALSE;
}

const u8 gDebugTextGameStatValue[] = _("{STR_VAR_1}:{STR_VAR_2}\n"
                                        "{STR_VAR_3}");
const u8 gDebugTextUnusedGameStat[] = DTR("みしよう　データ", "Unused data");
const u8 gDebugTextGameStatView[] = _("Confirmation");
const u8 gDebugTextGameStatSetMax[] = _("Max-1 set");
const u8 gDebugTextGameStatClear[] = _("Zero clear");
const u8 gDebugTextGameStatSavedGames[] = DTR("レポート", "SAVES");
const u8 gDebugTextGameStatFirstHallOfFamePlayTime[] = DTR("クリア　じかん", "Clear time");
const u8 gDebugTextGameStatStartedTrends[] = DTR("りゅうこう　さくせい", "TRENDS created");
const u8 gDebugTextGameStatPlantedBerries[] = DTR("きのみを　うえたかいすう", "BERRIES planted");
const u8 gDebugTextGameStatTradedBikes[] = DTR("じてんしゃこうかん", "BIKE swaps");
const u8 gDebugTextGameStatSteps[] = DTR("ほすう", "Steps");
const u8 gDebugTextGameStatInterviews[] = DTR("インタビュー", "INTERVIEWS");
const u8 gDebugTextGameStatTotalBattles[] = DTR("バトル", "BATTLES");
const u8 gDebugTextGameStatWildBattles[] = DTR("やせいバトル", "Wild BATTLES");
const u8 gDebugTextGameStatTrainerBattles[] = DTR("トレーナーバトル", "TRAINER BATTLES");
const u8 gDebugTextGameStatEnteredHallOfFame[] = DTR("でんどういり", "HALL of FAME times");
const u8 gDebugTextGameStatPokemonCaptures[] = DTR("ポケモンほかく", "POKéMON caught");
const u8 gDebugTextGameStatFishingCaptures[] = DTR("つった　かいすう", "Times fished");
const u8 gDebugTextGameStatHatchedEggs[] = DTR("タマゴかえった", "EGGS hatched");
const u8 gDebugTextGameStatEvolvedPokemon[] = DTR("しんかした", "Evolutions");
const u8 gDebugTextGameStatUsedPokecenter[] = DTR("ポケセン　かいふく", "{PKMN} Center heals");
const u8 gDebugTextGameStatRestedAtHome[] = DTR("じたく　かいふく", "Home heals");
const u8 gDebugTextGameStatEnteredSafariZone[] = DTR("サファリ　りよう", "SAFARI times");
const u8 gDebugTextGameStatUsedCut[] = DTR("ひでん:いあいぎり", "HM: CUT");
const u8 gDebugTextGameStatUsedRockSmash[] = DTR("ひでん:いわくだき", "HM: ROCK SMASH");
const u8 gDebugTextGameStatMovedSecretBase[] = DTR("きち　ひっこし", "BASE relocations");
const u8 gDebugTextGameStatPokemonTrades[] = DTR("つうしんこうかん", "LINK TRADES");
const u8 gDebugTextGameStatUnknown22[] = DTR("つうしんたいせん", "LINK BATTLES");
const u8 gDebugTextGameStatLinkBattleWins[] = DTR("つうしん　かち", "LINK WINS");
const u8 gDebugTextGameStatLinkBattleLosses[] = DTR("つうしん　まけ", "LINK LOSSES");
const u8 gDebugTextGameStatLinkBattleDraws[] = DTR("つうしん　ひきわけ", "LINK TIES");
// Idiot counter?
const u8 gDebugTextGameStatUsedSplash[] = DTR("わざ:はねる", "Move: SPLASH");
const u8 gDebugTextGameStatUsedStruggle[] = DTR("わざ:わるあがき", "Move: STRUGGLE");
const u8 gDebugTextGameStatSlotJackpots[] = DTR("スロットおおあたりかいすう", "SLOTS jackpots");
const u8 gDebugTextGameStatConsecutiveRouletteWins[] = DTR("ルーレット　れんしょう", "ROULETTE streak");
const u8 gDebugTextGameStatEnteredBattleTower[] = DTR("バトルタワー", "BATTLE TOWER");
const u8 gDebugTextGameStatUnknown31[] = DTR("バトルタワーかち", "BATTLE TOWER wins");
const u8 gDebugTextGameStatBattleTowerBestStreak[] = DTR("バトルタワーれんしょう", "BATTLE TOWER streak");
const u8 gDebugTextGameStatPokeblocks[] = DTR("ポロックさくせい", "{POKEBLOCK}s created");
const u8 gDebugTextGameStatPokeblocksWithFriends[] = DTR("つうしんでポロックつくった", "LINK {POKEBLOCK}s made");
const u8 gDebugTextGameStatWonLinkContest[] = DTR("つうしんコンテストかち", "LINK CONTEST wins");
const u8 gDebugTextGameStatEnteredContest[] = DTR("CPUコンテストさんか", "CPU CONTEST entries");
const u8 gDebugTextGameStatWonContest[] = DTR("CPUコンテストかち", "CPU CONTEST wins");
const u8 gDebugTextGameStatShopped[] = DTR("かいもの", "Shopping");
const u8 gDebugTextGameStatUsedItemfinder[] = DTR("ダウジングマシン", "ITEMFINDER");
const u8 gDebugTextGameStatGotRainedOn[] = DTR("あめふり", "Rained");
const u8 gDebugTextGameStatCheckedPokedex[] = DTR("ずかんをみた", "POKéDEX reads");
const u8 gDebugTextGameStatReceivedRibbons[] = DTR("リボン　もらった", "RIBBONS awarded");
const u8 gDebugTextGameStatJumpedDownLedges[] = DTR("だんさ　とびおり", "Ledge hops");
const u8 gDebugTextGameStatWatchedTv[] = DTR("TVを　みた", "Watched TV");
const u8 gDebugTextGameStatCheckedClock[] = DTR("とけいを　みた", "Checked CLOCK");
const u8 gDebugTextGameStatWonPokemonLottery[] = DTR("くじが　あたった", "LOTTERY wins");
const u8 gDebugTextGameStatUsedDaycare[] = DTR("ポケモンあずけた", "POKéMON DAYCARE uses");
const u8 gDebugTextGameStatRodeCableCar[] = DTR("ロープウェイりよう", "CABLE CAR uses");
const u8 gDebugTextGameStatEnteredHotSprings[] = DTR("おんせん　はいった", "HOT SPRING uses");

struct DebugGameStatInfo {
    u32 maxValue;
    const u8 * name;
};

const struct DebugGameStatInfo gDebugGameStatInfo[] = {
    {0xFFFFFF, gDebugTextGameStatSavedGames},
    {65485627, gDebugTextGameStatFirstHallOfFamePlayTime},
    {0xFFFFFF, gDebugTextGameStatStartedTrends},
    {0xFFFFFF, gDebugTextGameStatPlantedBerries},
    {0xFFFFFF, gDebugTextGameStatTradedBikes},
    {0xFFFFFF, gDebugTextGameStatSteps},
    {0xFFFFFF, gDebugTextGameStatInterviews},
    {0xFFFFFF, gDebugTextGameStatTotalBattles},
    {0xFFFFFF, gDebugTextGameStatWildBattles},
    {0xFFFFFF, gDebugTextGameStatTrainerBattles},
    {999,      gDebugTextGameStatEnteredHallOfFame},
    {0xFFFFFF, gDebugTextGameStatPokemonCaptures},
    {0xFFFFFF, gDebugTextGameStatFishingCaptures},
    {0xFFFFFF, gDebugTextGameStatHatchedEggs},
    {0xFFFFFF, gDebugTextGameStatEvolvedPokemon},
    {0xFFFFFF, gDebugTextGameStatUsedPokecenter},
    {0xFFFFFF, gDebugTextGameStatRestedAtHome},
    {0xFFFFFF, gDebugTextGameStatEnteredSafariZone},
    {0xFFFFFF, gDebugTextGameStatUsedCut},
    {0xFFFFFF, gDebugTextGameStatUsedRockSmash},
    {0xFFFFFF, gDebugTextGameStatMovedSecretBase},
    {0xFFFFFF, gDebugTextGameStatPokemonTrades},
    {0xFFFFFF, gDebugTextGameStatUnknown22},
    {9999,     gDebugTextGameStatLinkBattleWins},
    {9999,     gDebugTextGameStatLinkBattleLosses},
    {9999,     gDebugTextGameStatLinkBattleDraws},
    {0xFFFFFF, gDebugTextGameStatUsedSplash},
    {0xFFFFFF, gDebugTextGameStatUsedStruggle},
    {0xFFFFFF, gDebugTextGameStatSlotJackpots},
    {0xFFFFFF, gDebugTextGameStatConsecutiveRouletteWins},
    {0xFFFFFF, gDebugTextGameStatEnteredBattleTower},
    {0x0,      gDebugTextGameStatUnknown31},
    {0x0,      gDebugTextGameStatBattleTowerBestStreak},
    {0xFFFFFF, gDebugTextGameStatPokeblocks},
    {0xFFFFFF, gDebugTextGameStatPokeblocksWithFriends},
    {0xFFFFFF, gDebugTextGameStatWonLinkContest},
    {0xFFFFFF, gDebugTextGameStatEnteredContest},
    {0xFFFFFF, gDebugTextGameStatWonContest},
    {0xFFFFFF, gDebugTextGameStatShopped},
    {0xFFFFFF, gDebugTextGameStatUsedItemfinder},
    {0xFFFFFF, gDebugTextGameStatGotRainedOn},
    {0xFFFFFF, gDebugTextGameStatCheckedPokedex},
    {0xFFFFFF, gDebugTextGameStatReceivedRibbons},
    {0xFFFFFF, gDebugTextGameStatJumpedDownLedges},
    {0xFFFFFF, gDebugTextGameStatWatchedTv},
    {0xFFFFFF, gDebugTextGameStatCheckedClock},
    {0xFFFFFF, gDebugTextGameStatWonPokemonLottery},
    {0xFFFFFF, gDebugTextGameStatUsedDaycare},
    {0xFFFFFF, gDebugTextGameStatRodeCableCar},
    {0xFFFFFF, gDebugTextGameStatEnteredHotSprings}
};

void DebugMenu_PrintGameStat(u16 gameStat)
{
    u32 value = GetGameStat(gameStat);

    ConvertIntToDecimalStringN(gStringVar1, gameStat, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringCopy(gStringVar2, gDebugGameStatInfo[gameStat].name);
    if (gDebugGameStatInfo[gameStat].maxValue == 0)
        StringCopy(gStringVar3, gDebugTextUnusedGameStat);
    else if (gameStat != GAME_STAT_FIRST_HOF_PLAY_TIME)
        ConvertIntToDecimalStringN(gStringVar3, value, STR_CONV_MODE_RIGHT_ALIGN, 10);
    else
    {
        ConvertIntToDecimalStringN(gStringVar3, value >> 16, STR_CONV_MODE_RIGHT_ALIGN, 3);
        gStringVar3[3] = CHAR_COLON;
        ConvertIntToDecimalStringN(gStringVar3 + 4, (value >> 8) & 0xff, STR_CONV_MODE_LEADING_ZEROS, 2);
        gStringVar3[6] = CHAR_COLON;
        ConvertIntToDecimalStringN(gStringVar3 + 7, value & 0xff, STR_CONV_MODE_LEADING_ZEROS, 2);
    }
    Menu_DisplayDialogueFrame();
    Menu_PrintText(gDebugTextGameStatValue, 2, 15);
}

bool32 DebugMenu_HandleGameStatInput(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
        case 0:
            DebugMenu_PrintGameStat(data[1]);
            data[0]++;
            return FALSE;
        case 1:
            if (JOY_NEW(A_BUTTON))
            {
                data[0]--;
                return TRUE;
            }
            if (JOY_NEW(B_BUTTON))
            {
                data[0]++;
                return FALSE;
            }
            if (JOY_REPT(DPAD_UP))
            {
                data[1]++;
                if (data[1] == NUM_GAME_STATS)
                    data[1] = 0;
            }
            else if (JOY_REPT(DPAD_DOWN))
            {
                data[1]--;
                if (data[1] < 0)
                    data[1] = NUM_GAME_STATS - 1;
            }
            else
                return FALSE;
            DebugMenu_PrintGameStat(data[1]);
            return FALSE;
        case 2:
            Menu_EraseScreen();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
            return FALSE;
    }
    return FALSE;
}

void Task_DebugMenuViewGameStats(u8 taskId)
{
    DebugMenu_HandleGameStatInput(taskId);
}

void Task_DebugMenuSetGameStatMax(u8 taskId)
{
    if (DebugMenu_HandleGameStatInput(taskId))
    {
        u8 gameStat = gTasks[taskId].data[1];
        if (gDebugGameStatInfo[gameStat].maxValue != 0)
        {
            SetGameStat(gameStat, gDebugGameStatInfo[gameStat].maxValue - 1);
            PlaySE(SE_PIN);
        }
    }
}

void Task_DebugMenuClearGameStat(u8 taskId)
{
    if (DebugMenu_HandleGameStatInput(taskId))
    {
        u8 gameStat = gTasks[taskId].data[1];
        if (gDebugGameStatInfo[gameStat].maxValue != 0)
        {
            SetGameStat(gameStat, 0);
            PlaySE(SE_PIN);
        }
    }
}

bool8 DebugMenu_OpenGameStatTask(TaskFunc taskFunc)
{
    CloseMenu();
    LockPlayerFieldControls();
    CreateTask(taskFunc, 80);
    return TRUE;
}

bool8 DebugMenu_OpenGameStatViewer(void)
{
    return DebugMenu_OpenGameStatTask(Task_DebugMenuViewGameStats);
}

bool8 DebugMenu_OpenGameStatMaxEditor(void)
{
    return DebugMenu_OpenGameStatTask(Task_DebugMenuSetGameStatMax);
}

bool8 DebugMenu_OpenGameStatClearEditor()
{
    return DebugMenu_OpenGameStatTask(Task_DebugMenuClearGameStat);
}

const struct MenuAction gDebugMenuGameStatActions[] = {
    {gDebugTextGameStatView, DebugMenu_OpenGameStatViewer},
    {gDebugTextGameStatSetMax, DebugMenu_OpenGameStatMaxEditor},
    {gDebugTextGameStatClear, DebugMenu_OpenGameStatClearEditor}
};

bool8 DebugMenu_HandleGameStatMenuInput()
{
    return DebugMenu_ProcessActionMenuInput(gDebugMenuGameStatActions);
}

u8 DebugMenu_OpenLegendsRecord()
{
    Menu_EraseScreen();
    DebugMenu_DrawActionMenu(gDebugMenuGameStatActions, 12, ARRAY_COUNT(gDebugMenuGameStatActions));
    gMenuCallback = DebugMenu_HandleGameStatMenuInput;
    return FALSE;
}

const u8 gDebugTextCellInfo[] = _("ATTR:{STR_VAR_1}　HEIGHT:{STR_VAR_3}\n"
                                  "CODE:{STR_VAR_2}");

void DebugMenu_BuildCellInfoText(void)
{
    s16 x;
    s16 y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    gSpecialVar_Result = MapGridGetMetatileBehaviorAt(x, y);
    ConvertIntToHexStringN(gStringVar1, gSpecialVar_Result, STR_CONV_MODE_LEADING_ZEROS, 4);
    gSpecialVar_Result = MapGridGetMetatileIdAt(x, y);
    ConvertIntToHexStringN(gStringVar2, gSpecialVar_Result, STR_CONV_MODE_LEADING_ZEROS, 4);
    gSpecialVar_Result = MapGridGetElevationAt(x, y);
    ConvertIntToHexStringN(gStringVar3, gSpecialVar_Result, STR_CONV_MODE_LEADING_ZEROS, 4);
    StringExpandPlaceholders(gStringVar4, gDebugTextCellInfo);
}

void Task_DebugMenuShowCellInfo(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        case 0:
            Menu_DisplayDialogueFrame();
            gTasks[taskId].data[0]++;
            break;
        case 1:
            Menu_PrintText(gStringVar4, 2, 15);
            gTasks[taskId].data[0]++;
            break;
        case 2:
            if (JOY_NEW(A_BUTTON))
            {
                Menu_EraseScreen();
                UnlockPlayerFieldControls();
                DestroyTask(taskId);
            }
            break;
    }
}

u8 DebugMenu_CellInfo(void)
{
    CloseMenu();
    CreateTask(Task_DebugMenuShowCellInfo, 80);
    DebugMenu_BuildCellInfoText();
    LockPlayerFieldControls();
    return TRUE;
}

u8 DebugMenu_OpenBerryInfo(void)
{
    const u8 * berryInfo = DebugOpenBerryInfo();
    if (berryInfo != NULL)
    {
        Menu_DrawStdWindowFrame(0, 0, 19, 19);
        Menu_PrintText(berryInfo, 1, 1);
        gMenuCallback = DebugMenu_WaitForAButton;
        return FALSE;
    }
    CloseMenu();
    return TRUE;
}

const u8 gDebugTextBattleTowerLevelAndStage[] = DTR("レベル　　ステージ", "TOWER level stage");

void DebugMenu_PrintBattleTowerStagePrompt(void)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(gDebugTextBattleTowerLevelAndStage, 4, 15);
}

void DebugMenu_PrintBattleTowerLevel(u16 level)
{
    if (level == 0)
        ConvertIntToDecimalStringN(gStringVar1,  50, STR_CONV_MODE_LEADING_ZEROS, 3);
    else
        ConvertIntToDecimalStringN(gStringVar1, 100, STR_CONV_MODE_LEADING_ZEROS, 3);
    Menu_PrintText(gStringVar1, 4, 17);
}

void DebugMenu_PrintBattleTowerStage(u16 stage)
{
    ConvertIntToDecimalStringN(gStringVar1, stage, STR_CONV_MODE_LEADING_ZEROS, 4);
    Menu_PrintText(gStringVar1, 9, 17);
}

void Task_DebugMenuInitBattleTowerStageEditor(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    data[0] = 0;
    data[1] = gSaveBlock2.battleTower.curStreakChallengesNum[0];
    DebugMenu_PrintBattleTowerStagePrompt();
    DebugMenu_PrintBattleTowerLevel(data[0]);
    gTasks[taskId].func = Task_DebugMenuSelectBattleTowerLevel;
}

void Task_DebugMenuSelectBattleTowerLevel(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    if (JOY_NEW(DPAD_UP | DPAD_DOWN))
    {
        data[0] = data[0] == 0 ? 1 : 0;
        DebugMenu_PrintBattleTowerLevel(data[0]);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        data[1] = gSaveBlock2.battleTower.curStreakChallengesNum[data[0]];
        DebugMenu_PrintBattleTowerStage(data[1]);
        gTasks[taskId].func = Task_DebugMenuEditBattleTowerStage;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_DebugMenuCloseBattleTowerStageEditor;
    }
}

void Task_DebugMenuEditBattleTowerStage(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    if (DebugMenu_AdjustValue(data + 1, 0, 2000, gMain.newAndRepeatedKeys) == TRUE)
    {
        DebugMenu_PrintBattleTowerStage(data[1]);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        gSaveBlock2.battleTower.curStreakChallengesNum[data[0]] = data[1];
        PlaySE(SE_DING_DONG);
        gTasks[taskId].func = Task_DebugMenuCloseBattleTowerStageEditor;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        DebugMenu_PrintBattleTowerStagePrompt();
        DebugMenu_PrintBattleTowerLevel(data[0]);
        gTasks[taskId].func = Task_DebugMenuSelectBattleTowerLevel;
    }
}

void Task_DebugMenuCloseBattleTowerStageEditor(u8 taskId)
{
    Menu_EraseScreen();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

u8 DebugMenu_BattleTowerStages(void)
{
    CloseMenu();
    LockPlayerFieldControls();
    CreateTask(Task_DebugMenuInitBattleTowerStageEditor, 80);
    return TRUE;
}

const u8 gDebugTextPokeblockGroup[] = DTR("ポロック　グループ{STR_VAR_1}", "{POKEBLOCK} GROUP{STR_VAR_1}");

void DebugMenu_ShowPokeblockGroup(u16 group)
{
    ConvertIntToHexStringN(gStringVar1, group, STR_CONV_MODE_LEFT_ALIGN, 1);
    Menu_PrintTextWithStdFrame(gDebugTextPokeblockGroup);
}

const struct Pokeblock gDebugPokeblockFillTemplate[] = {
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {PBLOCK_CLR_BLACK, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x00},
    {}
};

void DebugMenu_FillPokeblockCase(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(gSaveBlock1.pokeblocks); i++)
    {
        if (gDebugPokeblockFillTemplate[i].color == 0) break;
        gSaveBlock1.pokeblocks[i] = gDebugPokeblockFillTemplate[i];
    }
}

void Task_DebugMenuSelectPokeblockGroup(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    switch (data[0])
    {
        case 0:
            DebugMenu_ShowPokeblockGroup(data[1]);
            data[0]++;
            break;
        case 1:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_DING_DONG);
                Menu_EraseScreen();
                DestroyTask(taskId);
                UnlockPlayerFieldControls();
                DebugMenu_FillPokeblockCase();
            }
            else if (JOY_NEW(B_BUTTON))
            {
                Menu_EraseScreen();
                DestroyTask(taskId);
                UnlockPlayerFieldControls();
            }
            else if (DebugMenu_AdjustValue(data + 1, 0, 4, gMain.newAndRepeatedKeys) == TRUE)
            {
                DebugMenu_ShowPokeblockGroup(data[1]);
            }
            break;
    }
}

u8 DebugMenu_CheckPKBLCK()
{
    CloseMenu();
    CreateTask(Task_DebugMenuSelectPokeblockGroup, 80);
    LockPlayerFieldControls();
    return TRUE;
}

const u8 gDebugTextWriteToFlash[] = _("write to flash");
const u8 gDebugTextReadFromFlash[] = _("read from flash");

const struct MenuAction gDebugMenuFlashTestActions[] = {
    {gDebugTextWriteToFlash, DebugMenu_PromptFlashWrite},
    {gDebugTextReadFromFlash, DebugMenu_ReadFlashToSharedMem}
};

const u8 gDebugTextFlashWritePrompt[] = _("Press A, begin to write\n"
                                          "Press B, cancel");

bool8 DebugMenu_PromptFlashWrite(void)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(gDebugTextFlashWritePrompt, 2, 15);
    gMenuCallback = DebugMenu_HandleFlashWritePrompt;
    return FALSE;
}

bool8 DebugMenu_HandleFlashWritePrompt(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        gMenuCallback = DebugMenu_WriteSharedMemToFlash;
        return FALSE;
    }
    if (JOY_NEW(B_BUTTON))
    {
        CloseMenu();
        return TRUE;
    }
    return FALSE;
}

const u8 gDebugTextFlashWriteSuccess[] = DTR("かきこみ　せいこう", "Write successful");
const u8 gDebugTextFlashReadSuccess[] = DTR("よみこみせいこう", "Read successful");
const u8 gDebugTextFlashWriteFailure[] = DTR("かきこみ　しっぱい", "Writing failed");
const u8 gDebugTextFlashReadFailure[] = DTR("よみこみ　しっぱい", "Reading failed");

bool8 DebugMenu_WriteSharedMemToFlash(void)
{
    if (DebugMenu_ProgramFlashSectorsWithVBlankOff(0, gSharedMem, 0x20000) == TRUE)
        StringCopy(gStringVar4, gDebugTextFlashWriteSuccess);
    else
        StringCopy(gStringVar4, gDebugTextFlashWriteFailure);
    gMenuCallback = DebugMenu_ShowFlashOperationResult;
    return FALSE;
}

bool8 DebugMenu_ReadFlashToSharedMem(void)
{
    u8 * data = gSharedMem;
    u8 i;

    for (i = 0; i < 32; i++)
    {
        DebugMenu_ReadFlashSectors(i, data, 0x1000);
        data += 0x1000;
    }
    StringCopy(gStringVar4, gDebugTextFlashReadSuccess);
    gMenuCallback = DebugMenu_ShowFlashOperationResult;
    return FALSE;
}

bool8 DebugMenu_ShowFlashOperationResult(void)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(gStringVar4, 2, 15);
    gMenuCallback = DebugMenu_CloseFlashOperationResult;
    return FALSE;
}

bool8 DebugMenu_CloseFlashOperationResult(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        CloseMenu();
        return TRUE;
    }
    return FALSE;
}

u8 DebugMenu_MeTooBackupMan(void)
{
    Menu_EraseScreen();
    DebugMenu_DrawActionMenu(gDebugMenuFlashTestActions, 12, ARRAY_COUNT(gDebugMenuFlashTestActions));
    gMenuCallback = DebugMenu_HandleFlashTestMenuInput;
    return FALSE;
}

bool8 DebugMenu_HandleFlashTestMenuInput()
{
    return DebugMenu_ProcessActionMenuInput(gDebugMenuFlashTestActions);
}

bool32 DebugMenu_ProgramFlashSectors(u16 sector, u8 *data, u32 size)
{
    while (1)
    {
        if (ProgramFlashSectorAndVerify(sector, data))
            return FALSE;
        if (size <= 0x1000)
            break;
        size -= 0x1000;
        data += 0x1000;
        sector++;
    }
    return TRUE;
}

bool32 DebugMenu_ProgramFlashSectorsWithVBlankOff(u8 sector, u8 *data, u32 size)
{
    bool32 result;
    m4aSoundVSyncOff();
    result = DebugMenu_ProgramFlashSectors(sector, data, size);
    m4aSoundVSyncOn();
    return result;
}

void DebugMenu_ReadFlashSectors(u8 sector, u8 *data, u32 size)
{
    ReadFlash(sector, 0, data, size);
}

struct DebugPlayTimeFieldInfo {
    u16 minValue;
    u16 maxValue;
    u8 cursorX;
    u8 taskDataIndex;
};

const struct DebugPlayTimeFieldInfo gDebugPlayTimeFieldInfo[] = {
    {0, 999, 2, 1},
    {0,  59, 6, 2},
    {0,  59, 9, 3}
};

void DebugMenu_DrawPlayTimeCursor(s16 *data)
{
    Menu_BlankWindowRect(2, 15, 22, 16);
    Menu_PrintChar(0xd0, gDebugPlayTimeFieldInfo[*data].cursorX, 15);
}

void DebugMenu_DrawPlayTime(s16 *data)
{
    ConvertIntToDecimalStringN(gStringVar1, data[1], STR_CONV_MODE_RIGHT_ALIGN, 3);
    Menu_PrintText(gStringVar1, 2, 17);
    Menu_PrintChar(0xf0, 5, 17);
    ConvertIntToDecimalStringN(gStringVar1, data[2], STR_CONV_MODE_LEADING_ZEROS, 2);
    Menu_PrintText(gStringVar1, 6, 17);
    Menu_PrintChar(0xf0, 8, 17);
    ConvertIntToDecimalStringN(gStringVar1, data[3], STR_CONV_MODE_LEADING_ZEROS, 2);
    Menu_PrintText(gStringVar1, 9, 17);
}

void Task_DebugMenuInitPlayTimeEditor(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    data[0] = 0;
    data[1] = gSaveBlock2.playTimeHours;
    data[2] = gSaveBlock2.playTimeMinutes;
    data[3] = gSaveBlock2.playTimeSeconds;

    Menu_DisplayDialogueFrame();
    DebugMenu_DrawPlayTime(data);
    DebugMenu_DrawPlayTimeCursor(data);
    gTasks[taskId].func = Task_DebugMenuEditPlayTime;
}

void Task_DebugMenuEditPlayTime(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    if (JOY_NEW(A_BUTTON))
    {
        gSaveBlock2.playTimeHours = data[1];
        gSaveBlock2.playTimeMinutes = data[2];
        gSaveBlock2.playTimeSeconds = data[3];
        PlaySE(SE_DING_DONG);
        gTasks[taskId].func = Task_DebugMenuClosePlayTimeEditor;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_DebugMenuClosePlayTimeEditor;
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        if ((u16)data[0] > 0)
        {
            data[0]--;
            DebugMenu_DrawPlayTimeCursor(data);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        if ((u16)data[0] < 2)
        {
            data[0]++;
            DebugMenu_DrawPlayTimeCursor(data);
        }
    }
    else
    {
        const struct DebugPlayTimeFieldInfo *field = gDebugPlayTimeFieldInfo + data[0];
        if (DebugMenu_AdjustValue(data + field->taskDataIndex, field->minValue, field->maxValue, gMain.newAndRepeatedKeys) == TRUE)
            DebugMenu_DrawPlayTime(data);
    }
}

void Task_DebugMenuClosePlayTimeEditor(u8 taskId)
{
    Menu_EraseScreen();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

u8 DebugMenu_PTime(void)
{
    CloseMenu();
    CreateTask(Task_DebugMenuInitPlayTimeEditor, 80);
    LockPlayerFieldControls();
    return TRUE;
}

#if (ENGLISH && REVISION == 0)
const u8 gDebugTextDisplayModeOrFlashError[] = _("switch DISP mode");
#else
const u8 gDebugTextDisplayModeOrFlashError[] = _("Set FLASH ERR");
#endif

void Task_DebugMenuConfigureDisplayModeOrFlashError(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        case 0:
#if (ENGLISH && REVISION == 0)
            gDebugTextModeDisplaySuspended = TRUE;
#else
            gDebugForceFlashError = FALSE;
#endif
            Menu_DisplayDialogueFrame();
            gTasks[taskId].data[0]++;
            break;
        case 1:
            Menu_PrintText(gDebugTextDisplayModeOrFlashError, 4, 15);
            DisplayYesNoMenu(20, 8, 1);
            gTasks[taskId].data[0]++;
            break;
        case 2:
            switch (Menu_ProcessInputNoWrap_())
            {
                case -2:
                    return;
                case 0:
#if (ENGLISH && REVISION == 0)
                    gDebugTextModeDisplayEnabled = TRUE;
#else
                    gDebugForceFlashError = TRUE;
#endif
                    break;
                default:
#if (ENGLISH && REVISION == 0)
                    gDebugTextModeDisplayEnabled = FALSE;
#else
                    gDebugForceFlashError = FALSE;
#endif
                    break;
            }
#if (ENGLISH && REVISION == 0)
            gDebugTextModeDisplaySuspended = FALSE;
#endif
            Menu_EraseScreen();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
            break;
    }
}

u8 DebugMenu_OpenMurakawa(void)
{
    CloseMenu();
    CreateTask(Task_DebugMenuConfigureDisplayModeOrFlashError, 80);
    LockPlayerFieldControls();
    return TRUE;
}

#if !(ENGLISH && REVISION == 0)
const u8 gDebugTextKiwaFontTest[] = _(
#if GERMAN
    "abcde;　abcde:　ABCDE;　ABCDE:\p"
#endif
    "Tableaux　des　verbes　du　2{SUPER_E}　groupe.\p"
    "La1{SUPER_RE}　chose　à apprendre　c'est　de　lire.\p"
    "Tableaux　des　verbes　du　1{SUPER_ER}　groupe.\p"
    "“あいうえおかきくけコさしすせそたちつてとな”\n"
    "<にぬネのはひふへほマみむめもやゆよらりるれろわャッ>\p"
    "をんゃゅょアイウエオカキクケサシスルレロワ,");

void Task_DebugMenuShowKiwaFontTest(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        case 0:
            Menu_DisplayDialogueFrame();
            gTasks[taskId].data[0]++;
            break;
        case 1:
            MenuPrintMessage(gDebugTextKiwaFontTest, 2, 15);
            gTasks[taskId].data[0]++;
            break;
        case 2:
            if (Menu_UpdateWindowText() && JOY_NEW(A_BUTTON))
            {
                Menu_EraseScreen();
                UnlockPlayerFieldControls();
                DestroyTask(taskId);
            }
            break;
    }
}

u8 DebugMenu_OpenKiwa(void)
{
    CloseMenu();
    CreateTask(Task_DebugMenuShowKiwaFontTest, 80);
    LockPlayerFieldControls();
    // return TRUE;
}
#endif

#endif
