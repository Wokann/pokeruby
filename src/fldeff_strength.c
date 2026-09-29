#include "global.h"
#include "braille_puzzles.h"
#include "event_data.h"
#include "field_effect.h"
#include "overworld.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_menu.h"
#include "fldeff.h"
#include "script.h"
#include "task.h"
#include "text.h"
#include "constants/field_effects.h"
#include "constants/event_objects.h"

static void FieldCallback_Strength(void);
static void FieldCallback_BrailleStrength(void);
static void StartStrengthFieldEffect(void);

extern const u8 EventScript_UseStrength[];

#if DEBUG
void Debug_UseStrength(void)
{
    if (ShouldDoBrailleStrengthEffect())
    {
        gLastFieldPokeMenuOpened = 0;
        gSpecialVar_Result = 0;
        FieldCallback_BrailleStrength();
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
        gPostMenuFieldCallback = FieldCallback_BrailleStrength;
    }
    else
    {
        if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_PUSHABLE_BOULDER) != TRUE)
            return FALSE;
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

static void FieldCallback_BrailleStrength(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    FieldEffectStart(FLDEFF_USE_STRENGTH);
}

bool8 FldEff_UseStrength(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (u32)StartStrengthFieldEffect >> 16;
    gTasks[taskId].data[9] = (u32)StartStrengthFieldEffect;
    GetMonNickname(&gPlayerParty[gFieldEffectArguments[0]], gStringVar1);
    return FALSE;
}

static void StartStrengthFieldEffect(void)
{
    if (ShouldDoBrailleStrengthEffect())
    {
        DoBrailleStrengthEffect();
    }
    else
    {
        FieldEffectActiveListRemove(FLDEFF_USE_STRENGTH);
        ScriptContext_Enable();
    }
}
