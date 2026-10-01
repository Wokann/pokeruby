#include "global.h"
#include "fldeff_softboiled.h"
#include "menu.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_menu.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "strings.h"
#include "task.h"
#include "ewram.h"

#if ENGLISH
#define WINDOW_LEFT 3
#define WINDOW_RIGHT 26
#elif GERMAN
#define WINDOW_LEFT 0
#define WINDOW_RIGHT 29
#endif

// extern
extern u8 gPartyMenuMessage_IsPrinting;
extern u8 gLastFieldPokeMenuOpened;
extern u8 gPartyMenuActionResult;

// Static
static void Task_TryUseSoftboiledOnPartyMon(u8 taskId);
static void Task_ChooseNewMonForSoftboiled(u8 taskId);
static void CantUseSoftboiledOnMon(u8 taskId);
static void Task_FinishSoftboiled(u8 taskId);

bool8 SetUpFieldMove_SoftBoiled(void) {
    u16 maxHp;
    u16 hp;
    u16 minHp;

    maxHp = GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_MAX_HP);
    hp = GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_HP);

    minHp = (maxHp / 5);
    if (hp >= minHp)
    {
        return TRUE;
    }

    return FALSE;
}

void ChooseMonForSoftboiled(u8 taskid) {
    sPartyMenuTwoMonAction.twoMonActionCallback = Task_TryUseSoftboiledOnPartyMon;
    gPartyMenu.pmTwoMonActionPromptTextId = 3;
    DoPokemonMenu_Switch(taskid);
}

static void Task_TryUseSoftboiledOnPartyMon(u8 taskId) {
    u8 userPartyId, recipientPartyId;
    u16 hp;
    struct Pokemon *pokemon;
    //struct Task *task;

    struct Sprite *sprites = gSprites;

    userPartyId = sprites[sPartyMenuTwoMonAction.firstSelectedMonSpriteId].data[0];
    recipientPartyId = sprites[sPartyMenuTwoMonAction.currentSelectionSpriteId].data[0];

    if (userPartyId > 5 || recipientPartyId > 5)
    {
        CancelTwoMonAction(taskId);
        return;
    }

    sPartyMenuAction.selectedMon = &gPlayerParty[sprites[sPartyMenuTwoMonAction.currentSelectionSpriteId].data[0]];
    hp = GetMonData(sPartyMenuAction.selectedMon, MON_DATA_HP);

    if (hp == 0 || userPartyId == recipientPartyId || GetMonData(sPartyMenuAction.selectedMon, MON_DATA_MAX_HP) == hp)
    {
        CantUseSoftboiledOnMon(taskId);
        return;
    }

    PlaySE(SE_USE_ITEM);

    sPartyMenuAction.selectedMonIndex = gSprites[sPartyMenuTwoMonAction.firstSelectedMonSpriteId].data[0];

    pokemon = &gPlayerParty[sPartyMenuAction.selectedMonIndex];
    sPartyMenuAction.selectedMon = pokemon;
    sPartyMenuAction.itemOrMoveId = 0;
    sPartyMenuAction.hpBarAnimationState = -0x8000;
    sPartyMenuAction.followupCallback = Task_FinishSoftboiled;


    gTasks[taskId].data[10] = GetMonData(sPartyMenuAction.selectedMon, MON_DATA_MAX_HP);
    gTasks[taskId].data[11] = GetMonData(sPartyMenuAction.selectedMon, MON_DATA_HP);
    gTasks[taskId].data[12] = gTasks[taskId].data[10] / 5;

    PartyMenuEraseMsgBoxAndFrame();
    gTasks[taskId].func = Task_SoftboiledReduceUserHp;
    gPartyMenu.pmPartyMenuHpBarValue = gTasks[taskId].data[11];
}

static void Task_ChooseNewMonForSoftboiled(u8 taskId) {
    if (gPartyMenuMessage_IsPrinting)
    {
        return;
    }

    Menu_EraseWindowRect(WINDOW_LEFT, 14, WINDOW_RIGHT, 19);
    PrintPartyMenuPromptText(3, 0);
    gTasks[taskId].func = HandlePartyMenuSwitchPokemonInput;
}

static void CantUseSoftboiledOnMon(u8 taskId) {
    gPartyMenuActionResult = 0;
    PartyMenuEraseMsgBoxAndFrame();
    DisplayPartyMenuMessage(gOtherText_CantUseOnPoke, 1);
    gTasks[taskId].func = Task_ChooseNewMonForSoftboiled;
}

static void Task_FinishSoftboiled(u8 taskId)
{
    ResetTwoMonActionSelection();
    gPartyMenu.backgroundRedrawState = 2;
    DestroySprite(&gSprites[sPartyMenuTwoMonAction.firstSelectedMonSpriteId]);
    Menu_EraseWindowRect(WINDOW_LEFT, 14, WINDOW_RIGHT, 19);
    PrintPartyMenuPromptText(0, 0);
    SwitchTaskToFollowupFunc(sPartyMenuTwoMonAction.partyMenuTaskId);
}
