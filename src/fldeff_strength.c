#include "global.h"
#include "braille_puzzles.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_menu.h"
#include "fldeff.h"
#include "script.h"
#include "task.h"
#include "string_util.h"
#include "text.h"
#include "constants/field_effects.h"
#include "constants/event_objects.h"

static void FieldCallback_Strength(void);
static void sub_811AA38(void);
static void sub_811AA9C(void);

extern u8 gLastFieldPokeMenuOpened;
extern u16 gSpecialVar_Result;
extern void (*gFieldCallback)(void);

#if DEBUG
void debug_sub_8130318(void)
{
    if (ShouldDoBrailleStrengthEffect())
    {
        gLastFieldPokeMenuOpened = 0;
        gSpecialVar_Result = 0;
        sub_811AA38();
    }
    else if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_PUSHABLE_BOULDER) == TRUE)
    {
        gLastFieldPokeMenuOpened = 0;
        gSpecialVar_Result = 0;
        FieldCallback_Strength();
    }
    else
    {
        UnlockPlayerFieldControls();
    }
}
#endif

bool8 SetUpFieldMove_Strength(void)
{
    if (ShouldDoBrailleStrengthEffect())
    {
        gSpecialVar_Result = gLastFieldPokeMenuOpened;
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = sub_811AA38;
    }
    else
    {
        if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_PUSHABLE_BOULDER) != TRUE)
            return 0;
        gSpecialVar_Result = gLastFieldPokeMenuOpened;
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_Strength;
    }

    return TRUE;
}

static void FieldCallback_Strength(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    ScriptContext_SetupScript(EventScript_UseStrength);
}

static void sub_811AA38(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    FieldEffectStart(FLDEFF_USE_STRENGTH);
}

bool8 FldEff_UseStrength(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (u32)sub_811AA9C >> 16;
    gTasks[taskId].data[9] = (u32)sub_811AA9C;
    GetMonNickname(&gPlayerParty[gFieldEffectArguments[0]], gStringVar1);
    return FALSE;
}

static void sub_811AA9C(void)
{
    if (ShouldDoBrailleStrengthEffect())
    {
        DoBrailleStrengthEffect();
    }
    else
    {
        FieldEffectActiveListRemove(40);
        ScriptContext_Enable();
    }
}
