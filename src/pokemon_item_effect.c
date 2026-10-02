#include "global.h"
#include "constants/battle.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/species.h"
#include "battle.h"
#include "evolution_scene.h"
#include "ewram.h"
#include "item.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_item_effect.h"
#include "battle_anim.h"
#include "party_menu.h"
#include "util.h"
#include "constants/item_effects.h"

extern s32 gBattleMoveDamage;
extern u8 gAbsentBattlerFlags;
extern u8 gBattlerInMenuId;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern u8 gActiveBattler;
extern u8 gPotentialItemEffectBattler;
extern struct BattlePokemon gBattleMons[];
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

bool8 PokemonUseItemEffects(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex, bool8 usedByAI);

bool8 ExecuteTableBasedItemEffect(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex)
{
    return PokemonUseItemEffects(mon, item, partyIndex, moveIndex, FALSE);
}

bool8 PokemonUseItemEffects(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex, bool8 usedByAI)
{
    u32 dataUnsigned;
    s32 friendship;
    s32 i;
    bool8 retVal = TRUE;
    const u8 *itemEffect;
    u8 itemEffectParam = ITEM_EFFECT_ARG_START;
    u32 temp;
    s8 friendshipChange = 0;
    u8 holdEffect;
    u8 battler = MAX_BATTLERS_COUNT;
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
        i = (GetBattlerSide(gActiveBattler) != B_SIDE_PLAYER);
        while (i < gBattlersCount)
        {
            if (gBattlerPartyIndexes[i] == partyIndex)
            {
                battler = i;
                break;
            }
            i += 2;
        }
    }
    else
    {
        gActiveBattler = 0;
        battler = MAX_BATTLERS_COUNT;
    }

    if (!ITEM_HAS_EFFECT(item))
        return TRUE;
    if (gItemEffectTable[item - ITEM_POTION] == NULL && item != ITEM_ENIGMA_BERRY)
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
        itemEffect = gItemEffectTable[item - ITEM_POTION];
    }

    for (i = 0; i < ITEM_EFFECT_ARG_START; i++)
    {
        switch (i)
        {
        // status healing effects
        case 0:
            if ((itemEffect[i] & ITEM0_INFATUATION)
             && gMain.inBattle && battler != MAX_BATTLERS_COUNT && (gBattleMons[battler].status2 & STATUS2_INFATUATION))
            {
                gBattleMons[battler].status2 &= ~STATUS2_INFATUATION;
                retVal = FALSE;
            }
            if ((itemEffect[i] & ITEM0_DIRE_HIT)
             && !(gBattleMons[gActiveBattler].status2 & STATUS2_FOCUS_ENERGY))
            {
                gBattleMons[gActiveBattler].status2 |= STATUS2_FOCUS_ENERGY;
                retVal = FALSE;
            }
            if ((itemEffect[i] & ITEM0_X_ATTACK)
             && gBattleMons[gActiveBattler].statStages[STAT_STAGE_ATK] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_STAGE_ATK] += itemEffect[i] & ITEM0_X_ATTACK;
                if (gBattleMons[gActiveBattler].statStages[STAT_STAGE_ATK] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_STAGE_ATK] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            break;
        // in-battle stat boosting effects?
        case 1:
            if ((itemEffect[i] & ITEM1_X_DEFEND)
             && gBattleMons[gActiveBattler].statStages[STAT_STAGE_DEF] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_STAGE_DEF] += (itemEffect[i] & ITEM1_X_DEFEND) >> 4;
                if (gBattleMons[gActiveBattler].statStages[STAT_STAGE_DEF] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_STAGE_DEF] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            if ((itemEffect[i] & ITEM1_X_SPEED)
             && gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPEED] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPEED] += itemEffect[i] & ITEM1_X_SPEED;
                if (gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPEED] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPEED] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            break;
        // more stat boosting effects?
        case 2:
            if ((itemEffect[i] & ITEM2_X_ACCURACY)
             && gBattleMons[gActiveBattler].statStages[STAT_STAGE_ACC] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_STAGE_ACC] += (itemEffect[i] & ITEM2_X_ACCURACY) >> 4;
                if (gBattleMons[gActiveBattler].statStages[STAT_STAGE_ACC] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_STAGE_ACC] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            if ((itemEffect[i] & ITEM2_X_SPATK)
             && gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPATK] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPATK] += itemEffect[i] & ITEM2_X_SPATK;
                if (gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPATK] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_STAGE_SPATK] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            break;
        case 3:
            if ((itemEffect[i] & ITEM3_GUARD_SPEC)
             && gSideTimers[GetBattlerSide(gActiveBattler)].mistTimer == 0)
            {
                gSideTimers[GetBattlerSide(gActiveBattler)].mistTimer = 5;
                retVal = FALSE;
            }
            if ((itemEffect[i] & ITEM3_LEVEL_UP)  // raise level
             && GetMonData(mon, MON_DATA_LEVEL, NULL) != MAX_LEVEL)
            {
                dataUnsigned = gExperienceTables[gSpeciesInfo[GetMonData(mon, MON_DATA_SPECIES, NULL)].growthRate][GetMonData(mon, MON_DATA_LEVEL, NULL) + 1];
                SetMonData(mon, MON_DATA_EXP, &dataUnsigned);
                CalculateMonStats(mon);
                retVal = FALSE;
            }
            if ((itemEffect[i] & ITEM3_SLEEP)
             && HealStatusConditions(mon, partyIndex, STATUS1_SLEEP, battler) == 0)
            {
                if (battler != MAX_BATTLERS_COUNT)
                    gBattleMons[battler].status2 &= ~STATUS2_NIGHTMARE;
                retVal = FALSE;
            }
            if ((itemEffect[i] & ITEM3_POISON) && HealStatusConditions(mon, partyIndex, (STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER), battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_BURN) && HealStatusConditions(mon, partyIndex, STATUS1_BURN, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_FREEZE) && HealStatusConditions(mon, partyIndex, STATUS1_FREEZE, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_PARALYSIS) && HealStatusConditions(mon, partyIndex, STATUS1_PARALYSIS, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_CONFUSION)  // heal confusion
             && gMain.inBattle && battler != MAX_BATTLERS_COUNT && (gBattleMons[battler].status2 & STATUS2_CONFUSION))
            {
                gBattleMons[battler].status2 &= ~STATUS2_CONFUSION;
                retVal = FALSE;
            }
            break;
        // EV, HP, and PP raising effects
        case 4:
            effectFlags = itemEffect[i];
            if (effectFlags & ITEM4_PP_UP)
            {
                effectFlags &= ~ITEM4_PP_UP;
                dataUnsigned = (GetMonData(mon, MON_DATA_PP_BONUSES, NULL) & gPPUpGetMask[moveIndex]) >> (moveIndex * 2);
                temp = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex);
                if (dataUnsigned < 3 && temp > 4)
                {
                    dataUnsigned = GetMonData(mon, MON_DATA_PP_BONUSES, NULL) + gPPUpAddValues[moveIndex];
                    SetMonData(mon, MON_DATA_PP_BONUSES, &dataUnsigned);

                    dataUnsigned = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), dataUnsigned, moveIndex) - temp;
                    dataUnsigned = GetMonData(mon, MON_DATA_PP1 + moveIndex, NULL) + dataUnsigned;
                    SetMonData(mon, MON_DATA_PP1 + moveIndex, &dataUnsigned);
                    retVal = FALSE;
                }
            }
            temp = 0;
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    u16 evCount;
                    s32 temp2;
     
                    switch (temp)
                    {
                    case 0: // EV_HP
                    case 1: // EV_ATK
                        evCount = GetMonEVCount(mon);
                        if (evCount >= MAX_TOTAL_EVS)
                            return TRUE;
                        dataUnsigned = GetMonData(mon, sGetMonDataEVConstants[temp], NULL);
                        if (dataUnsigned < EV_ITEM_RAISE_LIMIT)
                        {
                            if (dataUnsigned + itemEffect[itemEffectParam] > EV_ITEM_RAISE_LIMIT)
                                evChange = EV_ITEM_RAISE_LIMIT - (dataUnsigned + itemEffect[itemEffectParam]) + itemEffect[itemEffectParam];
                            else
                                evChange = itemEffect[itemEffectParam];
                            if (evCount + evChange > MAX_TOTAL_EVS)
                                evChange += MAX_TOTAL_EVS - (evCount + evChange);
                            dataUnsigned += evChange;
                            SetMonData(mon, sGetMonDataEVConstants[temp], &dataUnsigned);
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
                                if (battler != MAX_BATTLERS_COUNT)
                                {
                                    gAbsentBattlerFlags &= ~gBitTable[battler];
                                    CopyPlayerPartyMonToBattleData(battler, GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battler]));
                                    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER && gBattleResults.numRevivesUsed < 255)
                                        gBattleResults.numRevivesUsed++;
                                }
                                else
                                {
                                    gAbsentBattlerFlags &= ~gBitTable[gActiveBattler ^ 2];
                                    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER && gBattleResults.numRevivesUsed < 255)
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
                            if (usedByAI == FALSE)
                            {
                                dataUnsigned = GetMonData(mon, MON_DATA_HP, NULL) + dataUnsigned;
                                if (dataUnsigned > GetMonData(mon, MON_DATA_MAX_HP, NULL))
                                    dataUnsigned = GetMonData(mon, MON_DATA_MAX_HP, NULL);
                                SetMonData(mon, MON_DATA_HP, &dataUnsigned);
                                if (gMain.inBattle && battler != MAX_BATTLERS_COUNT)
                                {
                                    gBattleMons[battler].hp = dataUnsigned;
                                    if (!(effectFlags & (ITEM4_REVIVE >> 2)) && GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
                                    {
                                        if (gBattleResults.numHealingItemsUsed < 255)
                                            gBattleResults.numHealingItemsUsed++;
                                        // Keep the existing temporary reuse for matching.
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
                            for (temp2 = 0; temp2 < MAX_MON_MOVES; temp2++)
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
                                     && battler != MAX_BATTLERS_COUNT && !(gBattleMons[battler].status2 & STATUS2_TRANSFORMED)
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
                                 && battler != MAX_BATTLERS_COUNT && !(gBattleMons[battler].status2 & STATUS2_TRANSFORMED)
                                 && !(gDisableStructs[battler].mimickedMoves & gBitTable[moveIndex]))
                                    gBattleMons[battler].pp[moveIndex] = dataUnsigned;
                                retVal = FALSE;
                            }
                        }
                        break;
                    case 7: // EVO_STONE
                        {
                            u16 targetSpecies = GetEvolutionTargetSpecies(mon, EVO_MODE_ITEM_USE, item);

                            if (targetSpecies != SPECIES_NONE)
                            {
                                BeginEvolutionScene(mon, targetSpecies, FALSE, partyIndex);
                                return FALSE;
                            }
                        }
                        break;
                    }
                }
                temp++;
                effectFlags >>= 1;
            }
            break;
        case 5:
            effectFlags = itemEffect[i];
            temp = 0;
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    u16 evCount;

                    switch (temp)
                    {
                    case 0: // EV_DEF
                    case 1: // EV_SPEED
                    case 2: // EV_SPDEF
                    case 3: // EV_SPATK
                        evCount = GetMonEVCount(mon);
                        if (evCount >= MAX_TOTAL_EVS)
                            return TRUE;
                        dataUnsigned = GetMonData(mon, sGetMonDataEVConstants[temp + 2], NULL);
                        if (dataUnsigned < EV_ITEM_RAISE_LIMIT)
                        {
                            if (dataUnsigned + itemEffect[itemEffectParam] > EV_ITEM_RAISE_LIMIT)
                                evChange = EV_ITEM_RAISE_LIMIT - (dataUnsigned + itemEffect[itemEffectParam]) + itemEffect[itemEffectParam];
                            else
                                evChange = itemEffect[itemEffectParam];
                            if (evCount + evChange > MAX_TOTAL_EVS)
                                evChange += MAX_TOTAL_EVS - (evCount + evChange);
                            dataUnsigned += evChange;
                            SetMonData(mon, sGetMonDataEVConstants[temp + 2], &dataUnsigned);
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
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) < 100 && retVal == FALSE &&
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
                                if (GetMonData(mon, MON_DATA_POKEBALL, NULL) == ITEM_LUXURY_BALL)
                                    friendship++;
                                if (GetMonData(mon, MON_DATA_MET_LOCATION, NULL) == GetCurrentRegionMapSectionId())
                                    friendship++;
                            }
                            if (friendship < 0)
                                friendship = 0;
                            if (friendship > MAX_FRIENDSHIP)
                                friendship = MAX_FRIENDSHIP;
                            SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
                        }
                        itemEffectParam++;
                        break;
                    case 6: // FRIENDSHIP_MID
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) >= 100 && GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) < 200
                         && retVal == FALSE &&
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
                                if (GetMonData(mon, MON_DATA_POKEBALL, NULL) == ITEM_LUXURY_BALL)
                                    friendship++;
                                if (GetMonData(mon, MON_DATA_MET_LOCATION, NULL) == GetCurrentRegionMapSectionId())
                                    friendship++;
                            }
                            if (friendship < 0)
                                friendship = 0;
                            if (friendship > MAX_FRIENDSHIP)
                                friendship = MAX_FRIENDSHIP;
                            SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
                        }
                        itemEffectParam++;
                        break;
                    case 7: // FRIENDSHIP_HIGH
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) >= 200 && retVal == FALSE &&
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
                                if (GetMonData(mon, MON_DATA_POKEBALL, NULL) == ITEM_LUXURY_BALL)
                                    friendship++;
                                if (GetMonData(mon, MON_DATA_MET_LOCATION, NULL) == GetCurrentRegionMapSectionId())
                                    friendship++;
                            }
                            if (friendship < 0)
                                friendship = 0;
                            if (friendship > MAX_FRIENDSHIP)
                                friendship = MAX_FRIENDSHIP;
                            SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
                        }
                        itemEffectParam++;
                        break;
                    }
                }
                temp++;
                effectFlags >>= 1;
            }
            break;
        }
    }
    return retVal;
}
