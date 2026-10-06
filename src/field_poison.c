#include "global.h"
#include "field_poison.h"
#include "event_data.h"
#include "field_message_box.h"
#include "fldeff_misc.h"
#include "pokemon.h"
#include "party_menu.h"
#include "script.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "constants/species.h"

static bool32 IsMonValidSpecies(struct Pokemon *pokemon)
{
    // UB: Too few arguments for function 'GetMonData'
    u16 species = GetMonData(pokemon, MON_DATA_SPECIES_OR_EGG);
    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;
    else
        return TRUE;
}

static bool32 AllMonsFainted(void)
{
    int i;
    struct Pokemon *pokemon = gPlayerParty;

    for (i = 0; i < PARTY_SIZE; i++, pokemon++)
    {
        // UB: Too few arguments for function 'GetMonData'
        if (IsMonValidSpecies(pokemon) && GetMonData(pokemon, MON_DATA_HP) != 0)
            return FALSE;
    }

    return TRUE;
}

static void FaintFromFieldPoison(u8 partyIdx)
{
    struct Pokemon *pokemon = &gPlayerParty[partyIdx];
    u32 status = 0;

    AdjustFriendship(pokemon, FRIENDSHIP_EVENT_FAINT_FIELD_PSN);
    SetMonData(pokemon, MON_DATA_STATUS, &status);
    GetMonData(pokemon, MON_DATA_NICKNAME, gStringVar1);
    StringGet_Nickname(gStringVar1);
}

static bool32 MonFaintedFromPoison(u8 partyIdx)
{
    struct Pokemon *pokemon = &gPlayerParty[partyIdx];

    // UB: Too few arguments for function 'GetMonData'
    if (IsMonValidSpecies(pokemon) && GetMonData(pokemon, MON_DATA_HP) == 0
     && GetAilmentFromStatus(GetMonData(pokemon, MON_DATA_STATUS)) == AILMENT_PSN)
        return TRUE;
    else
        return FALSE;
}

#define tState    data[0]
#define tPartyIdx data[1]

static void Task_TryFieldPoisonWhiteOut(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        // Check if any Pokemon have fainted due to poison
        while (tPartyIdx < PARTY_SIZE)
        {
            if (MonFaintedFromPoison(tPartyIdx))
            {
                // Show message about fainted mon
                FaintFromFieldPoison(tPartyIdx);
                ShowFieldMessage(gText_PkmnFainted_FldPsn);
                tState++;
                return;
            }
            tPartyIdx++;
        }
        tState = 2;
        break;
    case 1:  // Wait for message box to disappear
        if (IsFieldMessageBoxHidden())
            tState--;  // Go to previous step and check next party member
        break;
    case 2:  // done checking all mons
        if (AllMonsFainted())
            gSpecialVar_Result = 1;
        else
            gSpecialVar_Result = 0;
        ScriptContext_Enable();
        DestroyTask(taskId);
        break;
    }
}

#undef tState
#undef tPartyIdx

void TryFieldPoisonWhiteOut(void)
{
    CreateTask(Task_TryFieldPoisonWhiteOut, 0x50);
    ScriptContext_Stop();
}

s32 DoPoisonFieldEffect(void)
{
    struct Pokemon *pokemon = &gPlayerParty[0];
    u32 numPoisoned = 0;
    u32 numFainted = 0;
    int i;

    // count the number of mons that are poisoned and fainting from poison,
    // and decrement HP of all poisoned mons
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u32 hp;

        if (GetMonData(pokemon, MON_DATA_SANITY_HAS_SPECIES) != 0
         && GetAilmentFromStatus(GetMonData(pokemon, MON_DATA_STATUS)) == AILMENT_PSN)
        {
            // decrement HP of poisoned mon
            hp = GetMonData(pokemon, MON_DATA_HP);
            if (hp != 0)
                hp--;
            if (hp == 0)
                numFainted++;
            SetMonData(pokemon, MON_DATA_HP, &hp);
            numPoisoned++;
        }
        pokemon++;
    }
    if (numFainted != 0 || numPoisoned != 0)
    {
        FldEffPoison_Start();
    }
    if (numFainted != 0)
    {
        return 2;
    }
    if (numPoisoned != 0)
    {
        return 1;
    }
    return 0;
}
