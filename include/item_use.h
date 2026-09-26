#ifndef GUARD_ITEM_USE_H
#define GUARD_ITEM_USE_H

extern u16 gSpecialVar_ItemId;

void ItemUseOutOfBattle_Mail(u8);
void ItemUseOutOfBattle_Bike(u8);
void ItemUseOnFieldCB_Bike(u8);
void ItemUseOutOfBattle_Rod(u8);
void ItemUseOnFieldCB_Rod(u8);
void ItemUseOutOfBattle_Itemfinder(u8);
void ItemUseOnFieldCB_Itemfinder(u8);
void ItemUseOutOfBattle_Berry(u8);
void Task_UseItemfinder(u8);
void Task_CloseItemfinderMessage(u8);
bool8 ItemfinderCheckForHiddenItems(const struct MapEvents *, u8);
void CheckForHiddenItemsInMapConnection(u8);
void SetDistanceOfClosestHiddenItem(u8, s16, s16);
u8 GetDirectionToHiddenItem(s16, s16);
void PlayerFaceHiddenItem(u8);
void Task_HiddenItemNearby(u8);
void Task_StandingOnHiddenItem(u8);
void ItemUseOutOfBattle_PokeblockCase(u8);
void ItemUseOutOfBattle_CoinCase(u8);
void ItemUseOutOfBattle_SSTicket(u8);
void ItemUseOutOfBattle_WailmerPail(u8);
void ItemUseOutOfBattle_Medicine(u8);
void ItemUseOutOfBattle_SacredAsh(u8);
void ItemUseOutOfBattle_PPRecovery(u8);
void ItemUseOutOfBattle_PPUp(u8);
void ItemUseOutOfBattle_RareCandy(u8);
void ItemUseOutOfBattle_TMHM(u8);
void ItemUseOutOfBattle_Repel(u8);
void ItemUseOutOfBattle_BlackWhiteFlute(u8);
void Task_UseDigEscapeRopeOnField(u8);
u8 CanUseDigOrEscapeRopeOnCurMap(void);
void ItemUseOutOfBattle_EscapeRope(u8);
void ItemUseOutOfBattle_EvolutionStone(u8);
void ItemUseInBattle_PokeBall(u8);
void ItemUseInBattle_StatIncrease(u8);
void ItemUseInBattle_Medicine(u8);
void ItemUseInBattle_PPRecovery(u8);
void ItemUseInBattle_Escape(u8);
void ItemUseOutOfBattle_EnigmaBerry(u8);
void ItemUseInBattle_EnigmaBerry(u8);
void ItemUseOutOfBattle_CannotUse(u8);
u8 CheckIfItemIsTMHMOrEvolutionStone(u16 itemId);

enum ItemTMHMOrEvolutionStone
{
    ITEM_IS_OTHER,
    ITEM_IS_TM_HM,
    ITEM_IS_EVOLUTION_STONE,
};

#endif // GUARD_ITEM_USE_H
