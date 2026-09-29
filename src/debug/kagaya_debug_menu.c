#if DEBUG

#include "global.h"
#include "field_effect.h"
#include "field_control_avatar.h"
#include "field_player_avatar.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "random.h"
#include "region_map.h"
#include "script.h"
#include "slot_machine.h"
#include "trainer_card.h"
#include "constants/field_effects.h"
#include "constants/rgb.h"

extern u8 (*gMenuCallback)(void);

static void InitKagayaDebugMenuDisplay(void);
static bool8 KagayaDebugMenu_ProcessInput(void);
static u8 KagayaDebugMenu_ShowTrainerCard(void);
static u8 KagayaDebugMenu_ShowTrainerCardAfterFade(void);
static u8 KagayaDebugMenu_ShowLinkTrainerCard(void);
static u8 KagayaDebugMenu_ShowLinkTrainerCardAfterFade(void);
static u8 KagayaDebugMenu_PlaySlotMachine(void);
static u8 KagayaDebugMenu_PlaySlotMachineAfterFade(void);
static u8 Debug_OpenFlyMapAfterFade(void);

#define DEBUG_FIELD_MOVE_PARTY_MON 0
#define NUM_DEBUG_SLOT_MACHINE_IDS 6

static const u8 sText_KagayaDebug_TrainerCard[] = _("Trainer's card");
static const u8 sText_KagayaDebug_LinkTrainerCard[] = _("Card to exchange");
static const u8 sText_KagayaDebug_SlotMachine[] = _("Slot machine");

static const struct MenuAction sMenuActions_KagayaDebug[] =
{
    {sText_KagayaDebug_TrainerCard, KagayaDebugMenu_ShowTrainerCard},
    {sText_KagayaDebug_LinkTrainerCard, KagayaDebugMenu_ShowLinkTrainerCard},
    {sText_KagayaDebug_SlotMachine, KagayaDebugMenu_PlaySlotMachine},
};

bool8 InitKagayaDebugMenu(void)
{
    InitKagayaDebugMenuDisplay();
    return FALSE;
}

static void InitKagayaDebugMenuDisplay(void)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 14, 7);
    Menu_PrintItems(2, 1, ARRAY_COUNT(sMenuActions_KagayaDebug), sMenuActions_KagayaDebug);
    InitMenu(0, 1, 1, ARRAY_COUNT(sMenuActions_KagayaDebug), 0, 13);
    gMenuCallback = KagayaDebugMenu_ProcessInput;
}

static bool8 KagayaDebugMenu_ProcessInput(void)
{
    s8 input = Menu_ProcessInput();

    if (input == MENU_NOTHING_CHOSEN)
    {
        return FALSE;
    }
    else if (input == MENU_B_PRESSED)
    {
        CloseMenu();
        return TRUE;
    }
    else
    {
        gMenuCallback = sMenuActions_KagayaDebug[input].func;
        return FALSE;
    }
}

static u8 KagayaDebugMenu_ShowTrainerCard(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gMenuCallback = KagayaDebugMenu_ShowTrainerCardAfterFade;
    return FALSE;
}

static u8 KagayaDebugMenu_ShowTrainerCardAfterFade(void)
{
    if (!UpdatePaletteFade())
    {
        Debug_ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu);
        CloseMenu();
        LockPlayerFieldControls();
        return TRUE;
    }
    return FALSE;
}

static u8 KagayaDebugMenu_ShowLinkTrainerCard(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gMenuCallback = KagayaDebugMenu_ShowLinkTrainerCardAfterFade;
    return FALSE;
}

static u8 KagayaDebugMenu_ShowLinkTrainerCardAfterFade(void)
{
    if (!UpdatePaletteFade())
    {
        Debug_ShowLinkTrainerCard(CB2_ReturnToFieldWithOpenMenu);
        CloseMenu();
        LockPlayerFieldControls();
        return TRUE;
    }
    return FALSE;
}

static u8 KagayaDebugMenu_PlaySlotMachine(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gMenuCallback = KagayaDebugMenu_PlaySlotMachineAfterFade;
    return FALSE;
}

static u8 KagayaDebugMenu_PlaySlotMachineAfterFade(void)
{
    if (!UpdatePaletteFade())
    {
        Debug_PlaySlotMachine(Random() % NUM_DEBUG_SLOT_MACHINE_IDS, CB2_ReturnToFieldWithOpenMenu);
        CloseMenu();
        LockPlayerFieldControls();
        return TRUE;
    }
    return FALSE;
}

bool8 Debug_TryUseSurf(void)
{
    if (!(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_SURFING))
    {
        CloseMenu();
        if (IsPlayerFacingSurfableFishableWater())
        {
            gFieldEffectArguments[0] = DEBUG_FIELD_MOVE_PARTY_MON;
            FieldEffectStart(FLDEFF_USE_SURF);
        }
    }
    else
    {
        CloseMenu();
    }
    return TRUE;
}

bool8 Debug_OpenFlyMap(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gMenuCallback = Debug_OpenFlyMapAfterFade;
    return FALSE;
}

static u8 Debug_OpenFlyMapAfterFade(void)
{
    if (!UpdatePaletteFade())
    {
        CloseMenu();
        SetMainCallback2(CB2_OpenFlyMap);
        return TRUE;
    }
    return FALSE;
}

bool8 Debug_TryUseDive(void)
{
    u8 diveWarpResult;

    CloseMenu();
    diveWarpResult = TrySetDiveWarp();
    if (diveWarpResult != 0)
    {
        gFieldEffectArguments[0] = DEBUG_FIELD_MOVE_PARTY_MON;
        gFieldEffectArguments[1] = diveWarpResult - 1;
        FieldEffectStart(FLDEFF_USE_DIVE);
    }
    return TRUE;
}

#endif // DEBUG
