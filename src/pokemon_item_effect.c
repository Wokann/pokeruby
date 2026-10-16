#include "global.h"
#include "constants/battle.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/species.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_util.h"
#include "evolution_scene.h"
#include "ewram.h"
#include "item.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"
#include "battle_anim.h"
#include "party_menu.h"
#include "util.h"
#include "constants/item_effects.h"

extern s32 gBattleMoveDamage;
extern u8 gBattlerInMenuId;
extern u8 gBattlersCount;
extern u8 gPotentialItemEffectBattler;
extern struct BattleEnigmaBerry gEnigmaBerries[];

static const u8 sGetMonDataEVConstants[] =
{
    MON_DATA_HP_EV,
    MON_DATA_ATK_EV,
    MON_DATA_DEF_EV,
    MON_DATA_SPEED_EV,
    MON_DATA_SPDEF_EV,
    MON_DATA_SPATK_EV
};


bool8 ExecuteTableBasedItemEffect(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex)
{
    return PokemonUseItemEffects(mon, item, partyIndex, moveIndex, 0);
}

bool8 PokemonUseItemEffects(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex, bool8 usedByAI)
{
    u32 dataUnsigned;
    s32 friendship;
    s32 cmdIndex;
    bool8 retVal = TRUE;
    const u8 *itemEffect;
    u8 itemEffectParam = ITEM_EFFECT_ARG_START;
    u32 temp1;
    s8 friendshipChange = 0;
    u8 holdEffect;
    u8 battler = 4; // Not active
    u16 heldItem;
    u8 effectFlags;
    u32 evChange;

    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);
    if (heldItem == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
            holdEffect = gEnigmaBerries[gBattlerInMenuId].holdEffect;
        else
            holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
    }
    else
    {
        holdEffect = GetItemHoldEffect(heldItem);
    }

    gPotentialItemEffectBattler = gBattlerInMenuId;
    if (gMain.inBattle)
    {
        gActiveBattler = gBattlerInMenuId;
        cmdIndex = (GetBattlerSide(gActiveBattler) != 0);
        while (cmdIndex < gBattlersCount)
        {
            if (gBattlerPartyIndexes[cmdIndex] == partyIndex)
            {
                battler = cmdIndex;
                break;
            }
            cmdIndex += 2;
        }
    }
    else
    {
        gActiveBattler = 0;
        battler = 4;
    }

    if (!ITEM_HAS_EFFECT(item))
        return TRUE;
    if (gItemEffectTable[item - 13] == NULL && item != ITEM_ENIGMA_BERRY)
        return TRUE;

    if (item == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
            itemEffect = gEnigmaBerries[gActiveBattler].itemEffect;
        else
            itemEffect = gSaveBlock1.enigmaBerry.itemEffect;
    }
    else
    {
        itemEffect = gItemEffectTable[item - 13];
    }

    for (cmdIndex = 0; cmdIndex < 6; cmdIndex++)
    {
        switch (cmdIndex)
        {
        // status healing effects
        case 0:
            if ((itemEffect[cmdIndex] & ITEM0_INFATUATION)
             && gMain.inBattle && battler != 4 && (gBattleMons[battler].status2 & STATUS2_INFATUATION))
            {
                gBattleMons[battler].status2 &= ~STATUS2_INFATUATION;
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM0_DIRE_HIT)
             && !(gBattleMons[gActiveBattler].status2 & STATUS2_FOCUS_ENERGY))
            {
                gBattleMons[gActiveBattler].status2 |= STATUS2_FOCUS_ENERGY;
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM0_X_ATTACK)
             && gBattleMons[gActiveBattler].statStages[STAT_ATK] < 12)
            {
                gBattleMons[gActiveBattler].statStages[STAT_ATK] += itemEffect[cmdIndex] & ITEM0_X_ATTACK;
                if (gBattleMons[gActiveBattler].statStages[STAT_ATK] > 12)
                    gBattleMons[gActiveBattler].statStages[STAT_ATK] = 12;
                retVal = FALSE;
            }
            break;
        // in-battle stat boosting effects?
        case 1:
            if ((itemEffect[cmdIndex] & ITEM1_X_DEFEND)
             && gBattleMons[gActiveBattler].statStages[STAT_DEF] < 12)
            {
                gBattleMons[gActiveBattler].statStages[STAT_DEF] += (itemEffect[cmdIndex] & ITEM1_X_DEFEND) >> 4;
                if (gBattleMons[gActiveBattler].statStages[STAT_DEF] > 12)
                    gBattleMons[gActiveBattler].statStages[STAT_DEF] = 12;
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM1_X_SPEED)
             && gBattleMons[gActiveBattler].statStages[STAT_SPEED] < 12)
            {
                gBattleMons[gActiveBattler].statStages[STAT_SPEED] += itemEffect[cmdIndex] & ITEM1_X_SPEED;
                if (gBattleMons[gActiveBattler].statStages[STAT_SPEED] > 12)
                    gBattleMons[gActiveBattler].statStages[STAT_SPEED] = 12;
                retVal = FALSE;
            }
            break;
        // more stat boosting effects?
        case 2:
            if ((itemEffect[cmdIndex] & ITEM2_X_ACCURACY)
             && gBattleMons[gActiveBattler].statStages[STAT_ACC] < 12)
            {
                gBattleMons[gActiveBattler].statStages[STAT_ACC] += (itemEffect[cmdIndex] & ITEM2_X_ACCURACY) >> 4;
                if (gBattleMons[gActiveBattler].statStages[STAT_ACC] > 12)
                    gBattleMons[gActiveBattler].statStages[STAT_ACC] = 12;
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM2_X_SPATK)
             && gBattleMons[gActiveBattler].statStages[STAT_SPATK] < 12)
            {
                gBattleMons[gActiveBattler].statStages[STAT_SPATK] += itemEffect[cmdIndex] & ITEM2_X_SPATK;
                if (gBattleMons[gActiveBattler].statStages[STAT_SPATK] > 12)
                    gBattleMons[gActiveBattler].statStages[STAT_SPATK] = 12;
                retVal = FALSE;
            }
            break;
        case 3:
            if ((itemEffect[cmdIndex] & ITEM3_GUARD_SPEC)
             && gSideTimers[GetBattlerSide(gActiveBattler)].mistTimer == 0)
            {
                gSideTimers[GetBattlerSide(gActiveBattler)].mistTimer = 5;
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM3_LEVEL_UP)  // raise level
             && GetMonData(mon, MON_DATA_LEVEL, NULL) != 100)
            {
                dataUnsigned = gExperienceTables[gSpeciesInfo[GetMonData(mon, MON_DATA_SPECIES, NULL)].growthRate][GetMonData(mon, MON_DATA_LEVEL, NULL) + 1];
                SetMonData(mon, MON_DATA_EXP, &dataUnsigned);
                CalculateMonStats(mon);
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM3_SLEEP)
             && HealStatusConditions(mon, partyIndex, 7, battler) == 0)
            {
                if (battler != 4)
                    gBattleMons[battler].status2 &= ~STATUS2_NIGHTMARE;
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM3_POISON) && HealStatusConditions(mon, partyIndex, (STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER), battler) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_BURN) && HealStatusConditions(mon, partyIndex, STATUS1_BURN, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_FREEZE) && HealStatusConditions(mon, partyIndex, STATUS1_FREEZE, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_PARALYSIS) && HealStatusConditions(mon, partyIndex, STATUS1_PARALYSIS, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_CONFUSION)  // heal confusion
             && gMain.inBattle && battler != 4 && (gBattleMons[battler].status2 & STATUS2_CONFUSION))
            {
                gBattleMons[battler].status2 &= ~STATUS2_CONFUSION;
                retVal = FALSE;
            }
            break;
        // EV, HP, and PP raising effects
        case 4:
            effectFlags = itemEffect[cmdIndex];
            if (effectFlags & ITEM4_PP_UP)
            {
                effectFlags &= ~ITEM4_PP_UP;
                dataUnsigned = (GetMonData(mon, MON_DATA_PP_BONUSES, NULL) & gPPUpGetMask[moveIndex]) >> (moveIndex * 2);
                temp1 = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex);
                if (dataUnsigned < 3 && temp1 > 4)
                {
                    dataUnsigned = GetMonData(mon, MON_DATA_PP_BONUSES, NULL) + gPPUpAddValues[moveIndex];
                    SetMonData(mon, MON_DATA_PP_BONUSES, &dataUnsigned);

                    dataUnsigned = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), dataUnsigned, moveIndex) - temp1;
                    dataUnsigned = GetMonData(mon, MON_DATA_PP1 + moveIndex, NULL) + dataUnsigned;
                    SetMonData(mon, MON_DATA_PP1 + moveIndex, &dataUnsigned);
                    retVal = FALSE;
                }
            }
            temp1 = 0;
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    u16 evCount;
                    s32 temp2;
     
                    switch (temp1)
                    {
                    case 0: // EV_HP
                    case 1: // EV_ATK
                        evCount = GetMonEVCount(mon);
                        if (evCount >= 510)
                            return TRUE;
                        dataUnsigned = GetMonData(mon, sGetMonDataEVConstants[temp1], NULL);
                        if (dataUnsigned < 100)
                        {
                            if (dataUnsigned + itemEffect[itemEffectParam] > 100)
                                evChange = 100 - (dataUnsigned + itemEffect[itemEffectParam]) + itemEffect[itemEffectParam];
                            else
                                evChange = itemEffect[itemEffectParam];
                            if (evCount + evChange > 510)
                                evChange += 510 - (evCount + evChange);
                            dataUnsigned += evChange;
                            SetMonData(mon, sGetMonDataEVConstants[temp1], &dataUnsigned);
                            CalculateMonStats(mon);
                            itemEffectParam++;
                            retVal = FALSE;
                        }
                        break;
                    case 2: // HEAL_HP
                        // revive?
                        if (effectFlags & (ITEM4_REVIVE >> 2))
                        {
                            if (GetMonData(mon, MON_DATA_HP, NULL) != 0)
                            {
                                itemEffectParam++;
                                break;
                            }
                            if (gMain.inBattle)
                            {
                                if (battler != 4)
                                {
                                    gAbsentBattlerFlags &= ~gBitTable[battler];
                                    CopyPlayerPartyMonToBattleData(battler, GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battler]));
                                    if (GetBattlerSide(gActiveBattler) == 0 && gBattleResults.numRevivesUsed < 255)
                                        gBattleResults.numRevivesUsed++;
                                }
                                else
                                {
                                    gAbsentBattlerFlags &= ~gBitTable[gActiveBattler ^ 2];
                                    if (GetBattlerSide(gActiveBattler) == 0 && gBattleResults.numRevivesUsed < 255)
                                        gBattleResults.numRevivesUsed++;
                                }
                            }
                        }
                        else
                        {
                            if (GetMonData(mon, MON_DATA_HP, NULL) == 0)
                            {
                                itemEffectParam++;
                                break;
                            }
                        }
                        dataUnsigned = itemEffect[itemEffectParam++];
                        switch (dataUnsigned)
                        {
                        case ITEM6_HEAL_HP_FULL:
                            dataUnsigned = GetMonData(mon, MON_DATA_MAX_HP, NULL) - GetMonData(mon, MON_DATA_HP, NULL);
                            break;
                        case ITEM6_HEAL_HP_HALF:
                            dataUnsigned = GetMonData(mon, MON_DATA_MAX_HP, NULL) / 2;
                            if (dataUnsigned == 0)
                                dataUnsigned = 1;
                            break;
                        case ITEM6_HEAL_HP_LVL_UP:
                            dataUnsigned = gBattleStruct->levelUpHP;
                            break;
                        }
                        if (GetMonData(mon, MON_DATA_MAX_HP, NULL) != GetMonData(mon, MON_DATA_HP, NULL))
                        {
                            if (usedByAI == 0)
                            {
                                dataUnsigned = GetMonData(mon, MON_DATA_HP, NULL) + dataUnsigned;
                                if (dataUnsigned > GetMonData(mon, MON_DATA_MAX_HP, NULL))
                                    dataUnsigned = GetMonData(mon, MON_DATA_MAX_HP, NULL);
                                SetMonData(mon, MON_DATA_HP, &dataUnsigned);
                                if (gMain.inBattle && battler != 4)
                                {
                                    gBattleMons[battler].hp = dataUnsigned;
                                    if (!(effectFlags & (ITEM4_REVIVE >> 2)) && GetBattlerSide(gActiveBattler) == 0)
                                    {
                                        if (gBattleResults.numHealingItemsUsed < 255)
                                            gBattleResults.numHealingItemsUsed++;
                                        // I have to re-use this variable to match.
                                        temp2 = gActiveBattler;
                                        gActiveBattler = battler;
                                        BtlController_EmitGetMonData(0, 0, 0);
                                        MarkBattlerForControllerExec(gActiveBattler);
                                        gActiveBattler = temp2;
                                    }
                                }
                            }
                            else
                            {
                                gBattleMoveDamage = -dataUnsigned;
                            }
                            retVal = FALSE;
                        }
                        effectFlags &= ~(ITEM4_REVIVE >> 2);
                        break;
                    case 3: // HEAL_PP
                        if (!(effectFlags & (ITEM4_HEAL_PP_ONE >> 3)))
                        {
                            for (temp2 = 0; temp2 < 4; temp2++)
                            {
                                u16 move;

                                dataUnsigned = GetMonData(mon, MON_DATA_PP1 + temp2, NULL);
                                move = GetMonData(mon, MON_DATA_MOVE1 + temp2, NULL);
                                if (dataUnsigned != CalculatePPWithBonus(move, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), temp2))
                                {
                                    dataUnsigned += itemEffect[itemEffectParam];
                                    move = GetMonData(mon, MON_DATA_MOVE1 + temp2, NULL);
                                    if (dataUnsigned > CalculatePPWithBonus(move, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), temp2))
                                    {
                                        move = GetMonData(mon, MON_DATA_MOVE1 + temp2, NULL);
                                        dataUnsigned = CalculatePPWithBonus(move, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), temp2);
                                    }
                                    SetMonData(mon, MON_DATA_PP1 + temp2, &dataUnsigned);
                                    if (gMain.inBattle
                                     && battler != 4 && !(gBattleMons[battler].status2 & STATUS2_TRANSFORMED)
                                     && !(gDisableStructs[battler].mimickedMoves & gBitTable[temp2]))
                                        gBattleMons[battler].pp[temp2] = dataUnsigned;
                                    retVal = FALSE;
                                }
                            }
                            itemEffectParam++;
                        }
                        else
                        {
                            u16 move;

                            dataUnsigned = GetMonData(mon, MON_DATA_PP1 + moveIndex, NULL);
                            move = GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL);
                            if (dataUnsigned != CalculatePPWithBonus(move, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex))
                            {
                                dataUnsigned += itemEffect[itemEffectParam++];
                                move = GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL);
                                if (dataUnsigned > CalculatePPWithBonus(move, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex))
                                {
                                    move = GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL);
                                    dataUnsigned = CalculatePPWithBonus(move, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex);
                                }
                                SetMonData(mon, MON_DATA_PP1 + moveIndex, &dataUnsigned);
                                if (gMain.inBattle
                                 && battler != 4 && !(gBattleMons[battler].status2 & STATUS2_TRANSFORMED)
                                 && !(gDisableStructs[battler].mimickedMoves & gBitTable[moveIndex]))
                                    gBattleMons[battler].pp[moveIndex] = dataUnsigned;
                                retVal = FALSE;
                            }
                        }
                        break;
                    case 7: // EVO_STONE
                        {
                            u16 targetSpecies = GetEvolutionTargetSpecies(mon, 2, item);

                            if (targetSpecies != SPECIES_NONE)
                            {
                                BeginEvolutionScene(mon, targetSpecies, 0, partyIndex);
                                return FALSE;
                            }
                        }
                        break;
                    }
                }
                temp1++;
                effectFlags >>= 1;
            }
            break;
        case 5:
            effectFlags = itemEffect[cmdIndex];
            temp1 = 0;
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    u16 evCount;

                    switch (temp1)
                    {
                    case 0: // EV_DEF
                    case 1: // EV_SPEED
                    case 2: // EV_SPDEF
                    case 3: // EV_SPATK
                        evCount = GetMonEVCount(mon);
                        if (evCount >= 510)
                            return TRUE;
                        dataUnsigned = GetMonData(mon, sGetMonDataEVConstants[temp1 + 2], NULL);
                        if (dataUnsigned < 100)
                        {
                            if (dataUnsigned + itemEffect[itemEffectParam] > 100)
                                evChange = 100 - (dataUnsigned + itemEffect[itemEffectParam]) + itemEffect[itemEffectParam];
                            else
                                evChange = itemEffect[itemEffectParam];
                            if (evCount + evChange > 510)
                                evChange += 510 - (evCount + evChange);
                            dataUnsigned += evChange;
                            SetMonData(mon, sGetMonDataEVConstants[temp1 + 2], &dataUnsigned);
                            CalculateMonStats(mon);
                            retVal = FALSE;
                            itemEffectParam++;
                        }
                        break;
                    case 4: // PP_MAX
                        dataUnsigned = (GetMonData(mon, MON_DATA_PP_BONUSES, NULL) & gPPUpGetMask[moveIndex]) >> (moveIndex * 2);
                        if (dataUnsigned < 3)
                        {
                            evChange = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex);
                            dataUnsigned = GetMonData(mon, MON_DATA_PP_BONUSES, NULL);
                            dataUnsigned &= gPPUpClearMask[moveIndex];
                            dataUnsigned += gPPUpAddValues[moveIndex] * 3;

                            SetMonData(mon, MON_DATA_PP_BONUSES, &dataUnsigned);
                            dataUnsigned = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), dataUnsigned, moveIndex) -
                                   evChange;
                            dataUnsigned = GetMonData(mon, MON_DATA_PP1 + moveIndex, NULL) + dataUnsigned;
                            SetMonData(mon, MON_DATA_PP1 + moveIndex, &dataUnsigned);
                            retVal = FALSE;
                        }
                        break;
                    case 5: // FRIENDSHIP_LOW
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) < 100 && retVal == 0 &&
                            friendshipChange == 0)
                        {
                            friendshipChange = itemEffect[itemEffectParam];
                            friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, NULL);
                            if (friendshipChange > 0 && holdEffect == HOLD_EFFECT_FRIENDSHIP_UP)
                                friendship += 150 * friendshipChange / 100;
                            else
                                friendship += friendshipChange;
                            if (friendshipChange > 0)
                            {
                                if (GetMonData(mon, MON_DATA_POKEBALL, NULL) == 11)
                                    friendship++;
                                if (GetMonData(mon, MON_DATA_MET_LOCATION, NULL) == GetCurrentRegionMapSectionId())
                                    friendship++;
                            }
                            if (friendship < 0)
                                friendship = 0;
                            if (friendship > 255)
                                friendship = 255;
                            SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
                        }
                        itemEffectParam++;
                        break;
                    case 6: // FRIENDSHIP_MID
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) >= 100 && GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) < 200
                         && retVal == 0 &&
                            friendshipChange == 0)
                        {
                            friendshipChange = itemEffect[itemEffectParam];
                            friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, NULL);
                            if (friendshipChange > 0 && holdEffect == HOLD_EFFECT_FRIENDSHIP_UP)
                                friendship += 150 * friendshipChange / 100;
                            else
                                friendship += friendshipChange;
                            if (friendshipChange > 0)
                            {
                                if (GetMonData(mon, MON_DATA_POKEBALL, NULL) == 11)
                                    friendship++;
                                if (GetMonData(mon, MON_DATA_MET_LOCATION, NULL) == GetCurrentRegionMapSectionId())
                                    friendship++;
                            }
                            if (friendship < 0)
                                friendship = 0;
                            if (friendship > 255)
                                friendship = 255;
                            SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
                        }
                        itemEffectParam++;
                        break;
                    case 7: // FRIENDSHIP_HIGH
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) >= 200 && retVal == 0 &&
                            friendshipChange == 0)
                        {
                            friendshipChange = itemEffect[itemEffectParam];
                            friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, NULL);
                            if (friendshipChange > 0 && holdEffect == HOLD_EFFECT_FRIENDSHIP_UP)
                                friendship += 150 * friendshipChange / 100;
                            else
                                friendship += friendshipChange;
                            if (friendshipChange > 0)
                            {
                                if (GetMonData(mon, MON_DATA_POKEBALL, NULL) == 11)
                                    friendship++;
                                if (GetMonData(mon, MON_DATA_MET_LOCATION, NULL) == GetCurrentRegionMapSectionId())
                                    friendship++;
                            }
                            if (friendship < 0)
                                friendship = 0;
                            if (friendship > 255)
                                friendship = 255;
                            SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
                        }
                        itemEffectParam++;
                        break;
                    }
                }
                temp1++;
                effectFlags >>= 1;
            }
            break;
        }
    }
    return retVal;
}
