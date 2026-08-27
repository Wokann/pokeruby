#include "global.h"
#include "field_poison.h"
#include "field_message_box.h"
#include "fldeff_poison.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "constants/species.h"

extern u16 gSpecialVar_Result;
extern const u8 gText_PkmnFainted_FldPsn[];

static bool32 IsMonValidSpecies(struct Pokemon *pokemon)
{
    // UB: Too few arguments for function 'GetMonData'
    u16 species = GetMonData(pokemon, MON_DATA_SPECIES2);
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

    AdjustFriendship(pokemon, FRIENDSHIP_EVENT_FAINT_OUTSIDE_BATTLE);
    SetMonData(pokemon, MON_DATA_STATUS, &status);
    GetMonData(pokemon, MON_DATA_NICKNAME, gStringVar1);
    StringGet_Nickname(gStringVar1);
}

static bool32 MonFaintedFromPoison(u8 partyIdx)
{
    struct Pokemon *pokemon = &gPlayerParty[partyIdx];

    // UB: Too few arguments for function 'GetMonData'
    if (IsMonValidSpecies(pokemon) && GetMonData(pokemon, MON_DATA_HP) == 0
     && GetPrimaryStatus(GetMonData(pokemon, MON_DATA_STATUS)) == STATUS_PRIMARY_POISON)
        return TRUE;
    else
        return FALSE;
}

#define tState       data[0]
#define tPartyIdx    data[1]

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

        if (GetMonData(pokemon, MON_DATA_SANITY_BIT2) != 0
         && GetPrimaryStatus(GetMonData(pokemon, MON_DATA_STATUS)) == STATUS_PRIMARY_POISON)
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
        FldeffPoison_Start();
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
