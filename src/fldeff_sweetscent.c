#include "global.h"
#include "constants/rgb.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon_menu.h"
#include "fldeff.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "wild_encounter.h"
#include "constants/field_effects.h"
#include "constants/songs.h"
static void FieldCallback_SweetScent(void);
static void StartSweetScentFieldEffect(void);
static void TrySweetScentEncounter(u8 taskId);
static void FailSweetScentEncounter(u8 taskId);

extern u8 gLastFieldPokeMenuOpened;
extern void (*gFieldCallback)(void);

bool8 SetUpFieldMove_SweetScent(void)
{
    gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
    gPostMenuFieldCallback = FieldCallback_SweetScent;
    return TRUE;
}

static void FieldCallback_SweetScent(void)
{
    FieldEffectStart(FLDEFF_SWEET_SCENT);
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
}

bool8 FldEff_SweetScent(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (u32)StartSweetScentFieldEffect >> 16;
    gTasks[taskId].data[9] = (u32)StartSweetScentFieldEffect;
    return FALSE;
}

static void StartSweetScentFieldEffect(void)
{
    u8 taskId;

    PlaySE(SE_M_SWEET_SCENT);
    BeginNormalPaletteFade(~(1 << (gSprites[GetPlayerAvatarSpriteId()].oam.paletteNum + 16)), 4, 0, 8, RGB(31, 0, 0));
    taskId = CreateTask(TrySweetScentEncounter, 0);
    gTasks[taskId].data[0] = 0;
    FieldEffectActiveListRemove(FLDEFF_SWEET_SCENT);
}

static void TrySweetScentEncounter(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (gTasks[taskId].data[0] == 64)
        {
            gTasks[taskId].data[0] = 0;
            if (SweetScentWildEncounter() == TRUE)
            {
                DestroyTask(taskId);
            }
            else
            {
                gTasks[taskId].func = FailSweetScentEncounter;
                BeginNormalPaletteFade(~(1 << (gSprites[GetPlayerAvatarSpriteId()].oam.paletteNum + 16)), 4, 8, 0, RGB(31, 0, 0));
            }
        }
        else
        {
            gTasks[taskId].data[0]++;
        }
    }
}

static void FailSweetScentEncounter(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        ScriptContext_SetupScript(EventScript_FailSweetScent);
        DestroyTask(taskId);
    }
}
