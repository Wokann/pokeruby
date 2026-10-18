#ifndef GUARD_PARTY_MENU_H
#define GUARD_PARTY_MENU_H

#include "main.h"
#include "menu.h"
#include "constants/party_menu.h"
#include "task.h"

struct MultiPartnerMenuPokemon;

struct PartyPopupMenu
{
    /*0x0*/u8 numChoices; // number of menu choices
    /*0x1*/u8 width; // menu width (number of characters it can fit horizontally)
    /*0x4*/const u8 *items;  // menu item ids (ids for a MenuAction2 array)
};

#define pmStatGrowths      data
#define pmSetupState       data[0]
#define pmMonIndex         data[1]
#define pmUnk268           data[2]
#define pmUnk272           data[7]
#define pmUnk27E           data[13]
#define pmUnk280           data[14]
#define pmUnk282           data[15]

struct Unk201B000
{
    struct Pokemon unk0[6];
    u8 menuType;
    u8 promptTextId;
    TaskFunc menuHandler;
    u8 menuHandlerTaskId;
    u8 unk261;
    u8 unk262; // Never read
    u8 unk263;
    s16 data[16];
};

struct Unk2001000
{
    u8 unk0;
    u8 slotId;
    u8 slotId2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    s16 unk8;
    s16 unkA;
    TaskFunc unkC;
    u16 array[53561];
};

struct PartyMenu
{
    /*0x00*/ struct Pokemon *pokemon;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 primarySelectedMonIndex;
    /*0x06*/ u16 secondarySelectedIndex;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 pad_0A[2];
    /*0x0C*/ s32 unkC;
    /*0x10*/ TaskFunc unk10;
    /*0x14*/ TaskFunc unk14;
};

void CB2_PartyMenuFromStartMenu(void);
void CB2_ReturnToPartyMenuFromFlyMap(void);
void CB2_ShowPartyMenuForItemUse(void);
void FieldCallback_PrepareFadeInFromMenu(void);
void CB2_PartyMenuMain(void);
void ChangeBattleTowerPartyMenuSelection(u8 taskId, s8 directionPressed);
void SetPartyMenuSettings(u8 menuType, u8 battleTypeFlags, TaskFunc menuHandlerFunc, u8 textId);
void OpenPartyMenu(u8 menuType, u8 battleFlags);
bool8 InitPartyMenu(void);
bool8 IsLinkDoubleBattle(void);
u8 DrawPartyMonBackground(u8);
void sub_806B908(void);
void DrawMonDescriptorStatus(u8, u8);
void sub_806BCE8(void);
u8 sub_806BD58(u8, u8);
u8 sub_806BD58(u8, u8);
u16 HandleDefaultPartyMenuInput(u8 taskId);
u16 HandleBattleTowerPartyMenuInput(u8 taskId);
void task_pc_turn_off(const u8 *a, u8 b);
void ChangePartyMenuSelection(u8 taskId, s8 directionPressed);
void SelectBattleTowerOKButton(u8 taskId);
void sub_806C994(u8 taskId, u8 b);
u8 GetCursorSelectionMonId(u8 taskID);
void HandlePartyMenuSwitchPokemonInput(u8 taskId);
void sub_806CCE4(void);
void sub_806CD44(u8 taskId);
void sub_806D3B4(u8 taskId, u16 species1, u16 species2);
void sub_806D4AC(u8 taskId, u16 species, u8 c);
void sub_806D50C(u8 taskId, u8 monIndex);
void PrintPartyMenuPromptText(u8 textId, u8 b);
void PartyMenuEraseMsgBoxAndFrame(void);
void CreatePartyMenuMonIcon(u8 taskId, u8 monIndex, u8 c, struct Pokemon *pokemon);
void TryCreatePartyMenuMonIcon(u8 a, u8 monIndex, struct Pokemon *pokemon);
void LoadHeldItemIcons(void);
void PartyMenuTryGiveMonHeldItem(u8 taskId, u16 newItem, TaskFunc c);
void SetMonIconSpriteId(u8 taskId, u8 monIndex, u8 spriteId);
void CreateHeldItemIcon_806DCD4(u8 taskId, u8 monIndex, u16 item);
void CreateHeldItemIcons_806DC34(u8 taskId);
void DrawHeldItemIconsForTrade(u8 *partyCounts, u8 *partySpriteIds, u8 whichParty);
void SetHeldItemIconVisibility(u8 a, u8 monIndex);
void PartyMenuDoPrintMonNickname(u8 monIndex, int b, const u8 *nameBuffer);
void PrintPartyMenuMonNickname(u8 monIndex, u8 b, struct Pokemon *pokemon);
void PrintPartyMenuMonNicknames(void);
void CreateMonIcon_LinkMultiBattle(u8 taskId, u8 monIndex, u8 menuType, struct MultiPartnerMenuPokemon *pokemon);
u8 *GetMonNickname(struct Pokemon *mon, u8 *dest);
u8 GetAilmentFromStatus(u32 status);
u8 GetMonAilment(struct Pokemon *mon);
void PartyMenuPutStatusTilemap(u8 monIndex, u8 b, u8 status);
void PartyMenuDoPrintLevel(u8 monIndex, u8 b, u8 level);
void PartyMenuPrintLevel(u8 monIndex, u8 b, struct Pokemon *pokemon);
void PartyMenuPrintMonLevelOrStatus(u8 monIndex, struct Pokemon *pokemon);
void PartyMenuPrintMonsLevelOrStatus(void);
void PartyMenuDoPrintGenderIcon(u16 species, u8 gender, u8 c, u8 monIndex, u8 *nickname);
void PartyMenuPrintGenderIcon(u8 monIndex, u8 b, struct Pokemon *pokemon);
void PartyMenuDoPrintHP(u8 monIndex, u8 b, u16 currentHP, u16 maxHP);
void PartyMenuTryPrintMonsHP(void);
void nullsub_13(void);
void PartyMenuDoDrawHPBar(u8 monIndex, u8 b, u16 currentHP, u16 maxHP);
void PartyMenuDrawHPBar(u8 monIndex, u8 b, struct Pokemon *pokemon);
void PartyMenuTryDrawHPBar(u8 monIndex, struct Pokemon *pokemon);
void PartyMenuDrawHPBars(void);
void SwapPokemon(struct Pokemon *a, struct Pokemon *b);
void SetPartyPopupMenuOffsets(u8 menuIndex, u8 *left, u8 *top, const struct PartyPopupMenu *menu);
void ShowPartyPopupMenu(u8 menuIndex, const struct PartyPopupMenu *menu, const struct MenuAction2 *menuActions, u8 cursorPos);
void ClosePartyPopupMenu(u8 index, const struct PartyPopupMenu *menu);
TaskFunc PartyMenuGetPopupMenuFunc(u8 menuIndex, const struct PartyPopupMenu *menus, const struct MenuAction2 *menuActions, u8 itemIndex);
u8 DisplayPartyMenuMessage(const u8 *message, bool8 keepOpen);
void sub_806E8D0(u8 taskId, u16 b, TaskFunc c);
void party_menu_link_mon_held_item_object(u8);
void Task_ConfirmGiveHeldItem(u8);
void DisplayGiveHeldItemMessage(u8, u16, bool8);
void DisplayTakeHeldItemMessage(u8, u16, bool8);
void Task_ConfirmTakeHeldMail(u8);
u16 ItemIdToBattleMoveId(u16);
bool8 MonKnowsMove(struct Pokemon *mon, u16 move);
void TeachMonTMMove(u8, u16, TaskFunc);
void Task_TeamMonTMMove(u8);
void Task_TeamMonTMMove2(u8);
void Task_TeamMonTMMove3(u8);
void Task_TeamMonTMMove4(u8);
void sub_806F358(u8);
void sub_806F390(u8);
void sub_806F44C(u8);
void TMMoveUpdateMoveSlot(u8);
void StopTryingToTeachMove_806F614(u8);
void StopTryingToTeachMove_806F67C(u8);
void StopTryingToTeachMove_806F6B4(u8);
void sub_806F8AC(u8 taskId);
void sub_806FA18(u8 taskId);
void sub_806FB0C(u8 taskId);
void sub_806FB44(u8);
void PartyMenuUpdateLevelOrStatus(struct Pokemon *, u8);
bool8 ExecuteTableBasedItemEffect_(u8 partyMonIndex, u16 item, u8 monMoveIndex);
void UseMedicine(u8, u16, TaskFunc);
bool8 IsBlueYellowRedFlute(u16);
void DoSacredAshItemEffect(u8, u16, TaskFunc);
void sub_8070088(u8);
void sub_80701DC(u8 taskId);
void DoPPRecoveryItemEffect(u8, u16, TaskFunc);
void DoRecoverPP(u8);
void DoPPUpItemEffect(u8, u16, TaskFunc);
void DoRareCandyItemEffect(u8, u16, TaskFunc);
void Task_RareCandy1(u8);
void Task_RareCandy2(u8);
void PrintStatGrowthsInLevelUpWindow(u8 taskId);
void PrintNewStatsInLevelUpWindow(u8 taskId);
void RedrawPokemonInfoInMenu(u8, struct Pokemon *);
void Task_RareCandy3(u8);
void TeachMonMoveInPartyMenu(u8);
void DoEvolutionStoneItemEffect(u8 taskId, u16 evolutionStoneItem, TaskFunc c);
u8 GetItemEffectType(u16 item);
void SetupDefaultPartyMenuSwitchPokemon(u8 taskId);
void sub_806CD5C(u8 taskId);
void DoTakeMail(u8 taskId, TaskFunc func);
void PartyMenuTryGiveMonHeldItem_806ECE8(u8 taskId, TaskFunc func);
void PartyMenuTryGiveMonMail(u8 taskId, TaskFunc func);
void sub_806D668(u8 monIndex);
void TaughtMove(u8 taskId);
void StopTryingToTeachMove_806F588(u8 taskId);
bool8 IsMoveHm(u16 move);

extern MainCallback gPostMenuFieldCallback;
extern u8 gSelectedMonPartyId;
extern u8 gSelectedOrderFromParty[3];
extern u8 gBattlePartyCurrentOrder[3];

void HandleBattlePartyMenu(u8 taskId);
bool8 SetUpBattlePartyMenu(void);
u8 GetPartyIdFromBattleSlot(u8 slot);
void OpenPartyMenuInBattle(u8 partyAction);
void ChooseMonForInBattleItem(void);
void BufferBattlePartyCurrentOrder(void);
void BufferBattlePartyCurrentOrderBySide(u8 battler, u8 flankId);
void SwitchPartyOrderLinkMulti(u8 battler, u8 slot, u8 slot2);
void SwitchPartyMonSlots(u8 slot, u8 slot2);
u8 GetPartyIdFromBattlePartyId(u8 battlePartyId);

void ChooseContestMon(void);
void ChoosePartyMon(void);
void ChooseMonForMoveRelearner(void);
void IsSelectedMonEgg(void);
void MoveDeleterChooseMoveToForget(void);
void GetNumMovesSelectedMonHas(void);
void BufferMoveDeleterNicknameAndMove(void);
void MoveDeleterForgetMove(void);

#endif // GUARD_PARTY_MENU_H
