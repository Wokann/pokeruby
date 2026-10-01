#ifndef GUARD_PARTY_MENU_H
#define GUARD_PARTY_MENU_H

#include "constants/party_menu.h"
#include "menu.h"
#include "task.h"


enum
{
    PARTY_MENU_TYPE_STANDARD,
    PARTY_MENU_TYPE_BATTLE,
    PARTY_MENU_TYPE_CONTEST,
    PARTY_MENU_TYPE_IN_GAME_TRADE,
    PARTY_MENU_TYPE_BATTLE_TOWER,
    PARTY_MENU_TYPE_LINK_MULTI_BATTLE,
    PARTY_MENU_TYPE_DAYCARE,
    PARTY_MENU_TYPE_MOVE_RELEARNER,
};

// The party menu screen is presented differently depending on which menu layout is set.
enum
{
    PARTY_MENU_LAYOUT_STANDARD,
    PARTY_MENU_LAYOUT_DOUBLE_BATTLE,
    PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE,
    PARTY_MENU_LAYOUT_MULTI_BATTLE,
};

struct PartyPopupMenu
{
    /*0x0*/u8 numChoices; // number of menu choices
    /*0x1*/u8 width; // menu width (number of characters it can fit horizontally)
    /*0x4*/const u8 *items;  // menu item ids (ids for a MenuAction2 array)
};

#define pmStatGrowths      data
#define pmSetupState       data[0]
#define pmMonIndex         data[1]
#define pmUnusedData2                      data[2]
#define pmTwoMonActionPromptTextId         data[7]
#define pmSacredAshRestoreInProgress       data[13]
#define pmBattleTowerHeldItem               data[14]
#define pmSacredAshHadEffect                data[14]
#define pmBattleTowerSpecies                data[15]
#define pmMoveLearningFromLevelUp           data[15]
#define pmPartyMenuHpBarValue               data[15]

// Shared state for all of Ruby's old-style party menu layouts.
struct PartyMenu
{
    struct Pokemon partyBuffer[PARTY_SIZE];
    u8 menuType;
    u8 promptTextId;
    TaskFunc menuHandler;
    u8 menuHandlerTaskId;
    u8 backgroundRedrawState;
    u8 unusedResumeAction; // Never read
    u8 battleTowerSelectionMode;
    s16 data[16];
};

// State used while selecting two party mons, such as switching or Soft-Boiled.
struct PartyMenuTwoMonActionState
{
    u8 partyMenuTaskId;
    u8 firstSelectedMonSpriteId;
    u8 currentSelectionSpriteId;
    u8 lowerMonIconSpriteId;
    u8 higherMonIconSpriteId;
    u8 lowerMonIndex;
    u8 higherMonIndex;
    u8 unused;
    s16 lowerMonBackgroundX;
    s16 higherMonBackgroundX;
    TaskFunc twoMonActionCallback;
};

// Context shared by party-menu item effects and their follow-up tasks.
struct PartyMenuActionState
{
    /*0x00*/ struct Pokemon *selectedMon;
    /*0x04*/ u8 menuTaskId;
    /*0x05*/ u8 selectedMonIndex;
    /*0x06*/ u16 itemOrMoveId;
    /*0x08*/ u16 moveToLearn;
    /*0x0A*/ u8 padding[2];
    /*0x0C*/ s32 hpBarAnimationState;
    /*0x10*/ TaskFunc followupCallback;
    /*0x14*/ TaskFunc afterHpAnimationCallback;
};

extern u8 gSelectedMonPartyId;

void CB2_PartyMenuMain(void);
void ChangeBattleTowerPartyMenuSelection(u8 taskId, s8 directionPressed);
void SetPartyMenuSettings(u8 menuType, u8 battleTypeFlags, TaskFunc menuHandlerFunc, u8 textId);
void OpenPartyMenu(u8 menuType, u8 battleFlags);
bool8 InitPartyMenu(void);
bool8 IsLinkDoubleBattle(void);
u8 DrawPartyMonBackground(u8);
void DrawLinkMultiBattlePartyMonBackgrounds(void);
void DrawMonDescriptorStatus(u8, u8);
void DrawPartyMonGenderDescriptorStatuses(void);
u8 CreatePartyMenuCursor(u8, u8);
u16 HandleDefaultPartyMenuInput(u8 taskId);
u16 HandleBattleTowerPartyMenuInput(u8 taskId);
void DrawPartyMenuMonBackgroundAt(const u8 *coords, u8 paletteNum);
void ChangePartyMenuSelection(u8 taskId, s8 directionPressed);
void SelectBattleTowerOKButton(u8 taskId);
void SetPartyMenuCursorSelection(u8 taskId, u8 selection);
u8 GetCursorSelectionMonId(u8 taskId);
void HandlePartyMenuSwitchPokemonInput(u8 taskId);
void ResetTwoMonActionSelection(void);
void CancelTwoMonAction(u8 taskId);
void UpdateLinkMultiBattlePartnerPartyBackgrounds(u8 taskId, u16 species1, u16 species2);
void StartLinkMultiBattlePartnerMonIconSlide(u8 taskId, u16 species, u8 partnerMonIndex);
void OffsetLinkMultiBattlePartnerMonIcon(u8 taskId, u8 monIndex);
void PrintPartyMenuPromptText(u8 textId, u8 windowType);
void PartyMenuEraseMsgBoxAndFrame(void);
void UpdatePartyMonHPBar(u8 spriteId, struct Pokemon *pokemon);
void CreatePartyMenuMonIcon(u8 taskId, u8 monIndex, u8 menuType, struct Pokemon *pokemon);
void TryCreatePartyMenuMonIcon(u8 taskId, u8 monIndex, struct Pokemon *pokemon);
void LoadHeldItemIconGraphics(void);
void PartyMenuTryGiveMonHeldItem(u8 taskId, u16 newItem, TaskFunc followupCallback);
void SetMonIconSpriteId(u8 taskId, u8 monIndex, u8 spriteId);
void CreatePartyMenuHeldItemIcon(u8 taskId, u8 monIndex, u16 item);
void CreatePartyMenuHeldItemIcons(u8 taskId);
void DrawHeldItemIconsForTrade(u8 *partyCounts, u8 *partySpriteIds, u8 whichParty);
void SetHeldItemIconVisibility(u8 taskId, u8 monIndex);
void PartyMenuDoPrintMonNickname(u8 monIndex, int menuLayout, const u8 *nameBuffer);
void PrintPartyMenuMonNickname(u8 monIndex, u8 menuLayout, struct Pokemon *pokemon);
void PrintPartyMenuMonNicknames(void);
void CreateMonIcon_LinkMultiBattle(u8 taskId, u8 monIndex, u8 menuType, struct MultiBattlePokemonTx *pokemon);
u8 *GetMonNickname(struct Pokemon *pokemon, u8 *stringBuffer);
void PartyMenuPutStatusTilemap(u8 monIndex, u8 menuLayout, u8 status);
void PartyMenuDoPrintLevel(u8 monIndex, u8 menuLayout, u8 level);
void PartyMenuPrintLevel(u8 monIndex, u8 menuLayout, struct Pokemon *pokemon);
void PartyMenuPrintMonLevelOrStatus(u8 monIndex, struct Pokemon *pokemon);
void PartyMenuPrintMonsLevelOrStatus(void);
void PartyMenuDoPrintGenderIcon(u16 species, u8 gender, u8 menuLayout, u8 monIndex, u8 *nickname);
void PartyMenuPrintGenderIcon(u8 monIndex, u8 menuLayout, struct Pokemon *pokemon);
void PartyMenuDoPrintHP(u8 monIndex, u8 menuLayout, u16 currentHP, u16 maxHP);
void PartyMenuTryPrintMonsHP(void);
void PartyMenuNoOpSetupStep(void);
void PartyMenuDoDrawHPBar(u8 monIndex, u8 menuLayout, u16 currentHP, u16 maxHP);
void PartyMenuDrawHPBar(u8 monIndex, u8 menuLayout, struct Pokemon *pokemon);
void PartyMenuTryDrawHPBar(u8 monIndex, struct Pokemon *pokemon);
void PartyMenuDrawHPBars(void);
void SwapPokemon(struct Pokemon *first, struct Pokemon *second);
void SetPartyPopupMenuOffsets(u8 menuIndex, u8 *left, u8 *top, const struct PartyPopupMenu *menu);
void ShowPartyPopupMenu(u8 menuIndex, const struct PartyPopupMenu *menu, const struct MenuAction2 *menuActions, u8 cursorPos);
void ClosePartyPopupMenu(u8 index, const struct PartyPopupMenu *menu);
TaskFunc PartyMenuGetPopupMenuFunc(u8 menuIndex, const struct PartyPopupMenu *menus, const struct MenuAction2 *menuActions, u8 itemIndex);
u8 DisplayPartyMenuMessage(const u8 *message, u8 noClearAfter);
void SetPartyMenuItemUseContext(u8 taskId, u16 itemOrMoveId, TaskFunc followupCallback);
void Task_UpdatePartyMenuHeldItemIconAndFollowup(u8 taskId);
void Task_ConfirmGiveHeldItem(u8);
void DisplayGiveHeldItemMessage(u8 monIndex, u16 item, u8 noClearAfter);
void DisplayTakeHeldItemMessage(u8 monIndex, u16 item, u8 noClearAfter);
void Task_ConfirmTakeHeldMail(u8);
u16 ItemIdToBattleMoveId(u16);
bool8 MonKnowsMove(struct Pokemon *, u16);
void TeachMonTMMove(u8 taskId, u16 move, TaskFunc followupCallback);
void Task_TeamMonTMMove(u8);
void Task_TeamMonTMMove2(u8);
void Task_TeamMonTMMove3(u8);
void Task_TeamMonTMMove4(u8);
void Task_ShowForgetMovePrompt(u8 taskId);
void Task_HandleForgetMovePrompt(u8 taskId);
void Task_FadeToMoveSelectionSummary(u8 taskId);
void TMMoveUpdateMoveSlot(u8);
void StopLearningMovePrompt(u8 taskId);
void Task_ShowStopTryingMovePrompt(u8 taskId);
void Task_HandleStopTryingMovePrompt(u8 taskId);
void Task_AnimatePartyMenuHpRestore(u8 taskId);
void Task_SoftboiledReduceUserHp(u8 taskId);
void Task_WaitForPartyMenuMessage(u8 taskId);
void Task_WaitForPartyMenuMessageThenFollowup(u8 taskId);
void PartyMenuUpdateLevelOrStatus(struct Pokemon *, u8);
u8 GetAilmentFromStatus(u32 status);
u8 GetMonAilment(struct Pokemon *mon);
void GetMedicineItemEffectMessage(u16);
bool8 ExecuteTableBasedItemEffectForPartyMon(u8 monIndex, u16 item, u8 moveIndex);
void UseMedicine(u8 taskId, u16 item, TaskFunc followupCallback);
bool8 IsItemFlute(u16);
void DoSacredAshItemEffect(u8 taskId, u16 item, TaskFunc followupCallback);
void TryUseSacredAshOnNextPartyMon(u8 taskId);
void FinishSacredAshPartyMonRestore(u8 taskId);
void DoPPRecoveryItemEffect(u8 taskId, u16 item, TaskFunc followupCallback);
void DoRecoverPP(u8);
void DoPPUpItemEffect(u8 taskId, u16 item, TaskFunc followupCallback);
void DoRareCandyItemEffect(u8 taskId, u16 item, TaskFunc followupCallback);
void Task_RareCandy1(u8);
void Task_RareCandy2(u8);
void PrintStatGrowthsInLevelUpWindow(u8 taskId);
void PrintNewStatsInLevelUpWindow(u8 taskId);
void RedrawPokemonInfoInMenu(u8, struct Pokemon *);
void Task_RareCandy3(u8);
void TeachMonMoveInPartyMenu(u8);
void DoEvolutionStoneItemEffect(u8 taskId, u16 evolutionStoneItem, TaskFunc followupCallback);
u8 GetItemEffectType(u16 item);
void SetupDefaultPartyMenuSwitchPokemon(u8 taskId);
void Task_SwitchSelectedPartyMons(u8 taskId);
void DoTakeMail(u8 taskId, TaskFunc followupCallback);
void TryTakePartyMonHeldItem(u8 taskId, TaskFunc followupCallback);
void PartyMenuTryGiveMonMail(u8 taskId, TaskFunc followupCallback);
void ClearPartyMonDescriptorStatus(u8 monIndex);
void TaughtMove(u8 taskId);
void Task_WaitToShowStopTryingMoveMessage(u8 taskId);
bool8 IsMoveHm(u16 move);
void BufferBattlePartyCurrentOrderBySide(u8 battler, u8 flankId);
u8 GetPartyIdFromBattleSlot(u8 slot);
void SwitchPartyMonSlots(u8 slot, u8 slot2);
u8 GetPartyIdFromBattlePartyId(u8 battlePartyId);
void OpenBattlePartyMenuForItemUse(void);
void OpenBattlePartyMenuWithAction(u8 action);

#endif // GUARD_PARTY_MENU_H
