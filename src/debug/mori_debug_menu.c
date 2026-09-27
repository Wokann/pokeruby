#include "global.h"
#include "mori_debug_menu.h"
#include "data2.h"
#include "daycare.h"
#include "move_relearner.h"
#include "link.h"
#include "main.h"
#include "menu.h"
#include "pokeblock.h"
#include "start_menu.h"
#include "string_util.h"

#define SIO_MULTI_CNT ((struct SioMultiCnt *)REG_ADDR_SIOCNT)

extern u8 (*gMenuCallback)(void);

u8 gMoriDebugLinkKeyFlagsText[0x20];

const u8 sLinkKeyNames[][3] =
{
	_(" A"),
	_(" B"),
	_("SL"),
	_("ST"),
	_("RK"),
	_("LK"),
	_("UK"),
	_("DK"),
	_("RT"),
	_("LT"),
};

const u8 sText_LinkKeyFlagsPrefix[] = _("ND");
const u8 sText_ChildIs[] = DTR("の　こどもは\n", "'s child is\n");
const u8 sText_TrailingSpace[] = _(" ");
const u8 sText_LongPokemonName[] = DTR("ながいなまえぽけもん", "LongName{PKMN}"); // "long name pokemon" (used as test name)
const u8 sText_SearchChild[] = _("Search a child");
const u8 sText_Egg[] = _("Egg");
const u8 sText_MaleEgg[] = _("Egg (male)");
const u8 sText_Add1000Steps[] = _("1000 steps");
const u8 sText_Add10000Steps[] = _("10000 steps");
const u8 sText_MoveTutor[] = _("MOVE TUTOR");
const u8 sText_BreedEgg[] = _("Breed an egg");
const u8 sText_LongName[] = _("Long name");
#if (ENGLISH && !DEBUG_FIX)
const u8 sText_DeletePokeblocks[] = _("ポロックけす");
#else
const u8 sText_DeletePokeblocks[] = _("Delete {POKEBLOCK}");
#endif

const struct MenuAction sMoriDebugMenuActions[] =
{
    {sText_SearchChild, (u8 (*) (void))MoriDebugMenu_SearchChild}, // ugly cast needed to stop complaints of u8 (*func)() not being compatible with this declaration (TODO: Make MenuAction a Callback union to allow a new definition.)
    {sText_Egg, MoriDebugMenu_Egg},
    {sText_MaleEgg, MoriDebugMenu_MaleEgg},
    {sText_Add1000Steps, MoriDebugMenu_1000Steps},
    {sText_Add10000Steps, MoriDebugMenu_10000Steps},
    {sText_MoveTutor, MoriDebugMenu_MoveRelearner},
    {sText_BreedEgg, MoriDebugMenu_BreedEgg},
    {sText_LongName, MoriDebugMenu_LongName},
    {sText_DeletePokeblocks, MoriDebugMenu_PokeblockCase},
};

void MoriDebugMenu_PrintLinkKeyFlags(void)
{
    int i;
    int id = SIO_MULTI_CNT->id;
    gMoriDebugLinkKeyFlagsText[0] = EOS;
    StringAppend(gMoriDebugLinkKeyFlagsText, sText_LinkKeyFlagsPrefix);
    for (i = 0; i < 10; i++)
        if ((word_3002910[id ^ 1] >> i) & 1)
            StringAppend(gMoriDebugLinkKeyFlagsText, sLinkKeyNames[i]);
}

bool8 MoriDebugMenu_WaitForAButton(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        CloseMenu();
        return TRUE;
    }
    else
        return FALSE;
}

u8 MoriDebugMenu_SearchChild(u8 a1, u8 a2, u8 *ptr)
{
    u8 localPtr[52];
    u16 monData;
    u16 eggSpecies;

    monData = GetMonData(gPlayerParty, MON_DATA_SPECIES, ptr);
    eggSpecies = GetEggSpecies(monData);
    StringCopy(localPtr, gSpeciesNames[monData]);
    StringAppend(localPtr, sText_ChildIs);
    StringAppend(localPtr, gSpeciesNames[eggSpecies]);
    StringAppend(localPtr, sText_TrailingSpace);
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 14, 30, 19);
    Menu_PrintText(localPtr, 1, 15);
    gMenuCallback = MoriDebugMenu_WaitForAButton;
    return 0;
}

u8 MoriDebugMenu_Egg(void)
{
    if (CountPokemonInDaycare(&gSaveBlock1.daycare) == 2 && GetDaycareCompatibilityScoreFromSave() )
        TriggerPendingDaycareEgg();
    CloseMenu();

    return 1;
}

u8 MoriDebugMenu_MaleEgg(void)
{
    if (CountPokemonInDaycare(&gSaveBlock1.daycare) == 2 && GetDaycareCompatibilityScoreFromSave() )
        TriggerPendingDaycareMaleEgg();
    CloseMenu();

    return 1;
}

u8 MoriDebugMenu_1000Steps(void)
{
    Debug_AddDaycareSteps(1000);
    CloseMenu();
    return 1;
}

u8 MoriDebugMenu_10000Steps(void)
{
    Debug_AddDaycareSteps(10000);
    CloseMenu();
    return 1;
}

u8 MoriDebugMenu_MoveRelearner(void)
{
    TeachMoveRelearnerMove();
    CloseMenu();
    return 1;
}

u8 MoriDebugMenu_BreedEgg(void)
{
    u8 loopCounter;

    for (loopCounter = 0; loopCounter <= 5; loopCounter++)
    {
        //UB: Too few arguments for function 'GetMonData'
        if ( GetMonData(&gPlayerParty[loopCounter], MON_DATA_IS_EGG) )
        {
            u8 friendship = 0;
            SetMonData(&gPlayerParty[loopCounter], MON_DATA_FRIENDSHIP, &friendship);
        }
    }
    gSaveBlock1.daycare.misc.countersEtc.eggCycleStepsRemaining = -3;
    CloseMenu();
    return 1;
}

u8 MoriDebugMenu_LongName(void)
{
    SetMonData(gPlayerParty, MON_DATA_NICKNAME, sText_LongPokemonName);
    CloseMenu();
    return 1;
}

u8 MoriDebugMenu_PokeblockCase(void)
{
    s32 loopCounter;

    for (loopCounter = 0; loopCounter <= 39; loopCounter++)
        PokeblockClearIfExists(loopCounter);

    CloseMenu();
    return 1;
}

bool8 MoriDebugMenu_ProcessInput(void)
{
    s8 choice = Menu_ProcessInput();

    switch (choice)
    {
    default:
        gMenuCallback = sMoriDebugMenuActions[choice].func;
        return FALSE;
    case -2:
        return FALSE;
    case -1:
        CloseMenu();
        return TRUE;
    }
}

s8 InitMoriDebugMenu(void)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 10, 19);
    Menu_PrintItems(1, 1, 9, sMoriDebugMenuActions);
    InitMenu(0, 1, 1, 9, 0, 9);
    gMenuCallback = MoriDebugMenu_ProcessInput;
    return 0;
}
