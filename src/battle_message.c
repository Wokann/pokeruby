#include "global.h"
#include "battle.h"
#include "battle_message.h"
#include "battle_tower.h"
#include "item.h"
#include "event_data.h"
#include "constants/items.h"
#include "pokemon.h"
#include "data.h"
#include "battle_main.h"
#include "text.h"
#include "string_util.h"
#include "link.h"
#include "battle_setup.h"
#include "battle_tower.h"
#include "constants/flags.h"
#include "ewram.h"
#include "constants/battle_string_ids.h"

#define BATTLESTRINGS_NO    351
#define BATTLESTRINGS_MAX   BATTLESTRINGS_NO + BATTLESTRINGS_TABLE_START

#ifdef GERMAN
#include "data/battle_strings_de.h" // TODO: German
#else
#include "data/battle_strings_en.h"
#endif

// This is four lists of moves which use a different attack string in Japanese
// to the default. See the documentation for ChooseTypeOfMoveUsedString for more detail.
static const u16 sGrammarMoveUsedTable[] =
{
    MOVE_SWORDS_DANCE,
    MOVE_STRENGTH,
    MOVE_GROWTH,
    MOVE_HARDEN,
    MOVE_MINIMIZE,
    MOVE_SMOKESCREEN,
    MOVE_WITHDRAW,
    MOVE_DEFENSE_CURL,
    MOVE_EGG_BOMB,
    MOVE_SMOG,
    MOVE_BONE_CLUB,
    MOVE_FLASH,
    MOVE_SPLASH,
    MOVE_ACID_ARMOR,
    MOVE_BONEMERANG,
    MOVE_REST,
    MOVE_SHARPEN,
    MOVE_SUBSTITUTE,
    MOVE_MIND_READER,
    MOVE_SNORE,
    MOVE_PROTECT,
    MOVE_SPIKES,
    MOVE_ENDURE,
    MOVE_ROLLOUT,
    MOVE_SWAGGER,
    MOVE_SLEEP_TALK,
    MOVE_HIDDEN_POWER,
    MOVE_PSYCH_UP,
    MOVE_EXTREME_SPEED,
    MOVE_FOLLOW_ME,
    MOVE_TRICK,
    MOVE_ASSIST,
    MOVE_INGRAIN,
    MOVE_KNOCK_OFF,
    MOVE_CAMOUFLAGE,
    MOVE_ASTONISH,
    MOVE_ODOR_SLEUTH,
    MOVE_GRASS_WHISTLE,
    MOVE_SHEER_COLD,
    MOVE_MUDDY_WATER,
    MOVE_IRON_DEFENSE,
    MOVE_BOUNCE,
    0,
    MOVE_TELEPORT,
    MOVE_RECOVER,
    MOVE_BIDE,
    MOVE_AMNESIA,
    MOVE_FLAIL,
    MOVE_TAUNT,
    MOVE_BULK_UP,
    0,
    MOVE_MEDITATE,
    MOVE_AGILITY,
    MOVE_MIMIC,
    MOVE_DOUBLE_TEAM,
    MOVE_BARRAGE,
    MOVE_TRANSFORM,
    MOVE_STRUGGLE,
    MOVE_SCARY_FACE,
    MOVE_CHARGE,
    MOVE_WISH,
    MOVE_BRICK_BREAK,
    MOVE_YAWN,
    MOVE_FEATHER_DANCE,
    MOVE_TEETER_DANCE,
    MOVE_MUD_SPORT,
    MOVE_FAKE_TEARS,
    MOVE_WATER_SPORT,
    MOVE_CALM_MIND,
    0,
    MOVE_POUND,
    MOVE_SCRATCH,
    MOVE_VICE_GRIP,
    MOVE_WING_ATTACK,
    MOVE_FLY,
    MOVE_BIND,
    MOVE_SLAM,
    MOVE_HORN_ATTACK,
    MOVE_WRAP,
    MOVE_THRASH,
    MOVE_TAIL_WHIP,
    MOVE_LEER,
    MOVE_BITE,
    MOVE_GROWL,
    MOVE_ROAR,
    MOVE_SING,
    MOVE_PECK,
    MOVE_ABSORB,
    MOVE_STRING_SHOT,
    MOVE_EARTHQUAKE,
    MOVE_FISSURE,
    MOVE_DIG,
    MOVE_TOXIC,
    MOVE_SCREECH,
    MOVE_METRONOME,
    MOVE_LICK,
    MOVE_CLAMP,
    MOVE_CONSTRICT,
    MOVE_POISON_GAS,
    MOVE_BUBBLE,
    MOVE_SLASH,
    MOVE_SPIDER_WEB,
    MOVE_NIGHTMARE,
    MOVE_CURSE,
    MOVE_FORESIGHT,
    MOVE_CHARM,
    MOVE_ATTRACT,
    MOVE_ROCK_SMASH,
    MOVE_UPROAR,
    MOVE_SPIT_UP,
    MOVE_SWALLOW,
    MOVE_TORMENT,
    MOVE_FLATTER,
    MOVE_ROLE_PLAY,
    MOVE_ENDEAVOR,
    MOVE_TICKLE,
    MOVE_COVET,
    0,
};

static const u8 sText_EmptyStatus[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // empty flags

extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u8 gStringVar1[];
extern u8 gStringVar2[];
extern u8 gStringVar3[];
extern u16 gBattleTypeFlags;
extern u16 gTrainerBattleOpponent_A;
extern u8 gDisplayedStringBattle[];
extern u8 gStringVar1[];
extern u8 gStringVar2[];
extern u8 gStringVar3[];
extern u16 gBattlerPartyIndexes[4];

static EWRAM_DATA u8 sBattlerAbilities[4] = {0};

struct StatusFlagString
{
    u8* flag;
    u8* ptr;
};

extern const struct StatusFlagString gStatusConditionStringsTable[7]; // status flag/text

// Ruby reuses this EWRAM pointer outside battles; retain its linked identity.
extern struct BattleMsgData* gSelectedOrderFromParty;
#define gBattleMsgDataPtr gSelectedOrderFromParty

static void ChooseMoveUsedParticle(u8* textBuff);
static void ChooseTypeOfMoveUsedString(u8* dst);
static void ExpandBattleTextBuffPlaceholders(const u8* src, u8* dst);

u8 GetBattlerSide(u8 battler);
s32 GetBattlerMultiplayerId(u16);
void get_trainer_name(u8* dst);
u8 get_trainer_class_name_index(void);
u8 GetMultiplayerId(void);
u8 GetBattlerAtPosition(u8 position);
u8 GetBattlerSide(u8 battler);
u8 GetBattlerPosition(u8 battler);
#ifdef GERMAN
extern u8 *de_sub_804110C();
#endif

void BufferStringBattle(u16 stringID)
{
    s32 i;
    const u8* stringPtr = NULL;

    gBattleMsgDataPtr = (struct BattleMsgData*)(&gBattleBufferA[gActiveBattler][4]);
    gLastUsedItem = gBattleMsgDataPtr->lastItem;
    gLastUsedAbility = gBattleMsgDataPtr->lastAbility;
    gBattleStruct->battler = gBattleMsgDataPtr->scrActive;
    gBattleStruct->scriptPartyIdx = gBattleMsgDataPtr->bakScriptPartyIdx;
    gBattleStruct->hpScale = gBattleMsgDataPtr->hpScale;
    gPotentialItemEffectBattler = gBattleMsgDataPtr->itemEffectBattler;
    gBattleStruct->stringMoveType = gBattleMsgDataPtr->moveType;
    for (i = 0; i < 4; i++)
    {
        sBattlerAbilities[i] = gBattleMsgDataPtr->abilities[i];
    }
    for (i = 0; i < 0x10; i++)
    {
        gBattleTextBuff1[i] = gBattleMsgDataPtr->textBuffs[0][i];
        gBattleTextBuff2[i] = gBattleMsgDataPtr->textBuffs[1][i];
        gBattleTextBuff3[i] = gBattleMsgDataPtr->textBuffs[2][i];
    }
    switch (stringID)
    {
    case 0: // first battle msg
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_TwoLinkTrainersWantToBattle;
                else
                    stringPtr = sText_LinkTrainerWantsToBattle;
            }
            else
            {
                stringPtr = sText_Trainer1WantsToBattle;
#ifdef GERMAN
                stringPtr = de_sub_804110C(0xFFFF, stringPtr);
#endif
            }
        }
        else
        {
            if (gBattleTypeFlags & BATTLE_TYPE_LEGENDARY)
                stringPtr = sText_LegendaryPkmnAppeared;
            else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) // interesting, looks like they had something planned for wild double battles
                stringPtr = sText_TwoWildPkmnAppeared;
            else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL)
                stringPtr = sText_WildPkmnAppearedPause;
            else
                stringPtr = sText_WildPkmnAppeared;
        }
        break;
    case 1: // poke first send-out
        if (GetBattlerSide(gActiveBattler) == 0)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_LinkPartnerSentOutPkmnGoPkmn;
                else
                    stringPtr = sText_GoTwoPkmn;
            }
            else
                stringPtr = sText_GoPkmn;
        }
        else
        {
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_TwoLinkTrainersSentOutPkmn;
                else if (gBattleTypeFlags & BATTLE_TYPE_LINK)
                    stringPtr = sText_LinkTrainerSentOutTwoPkmn;
                else
                {
                    stringPtr = sText_Trainer1SentOutTwoPkmn;
#ifdef GERMAN
                    stringPtr = de_sub_804110C(0xFFFF, stringPtr);
#endif
                }
            }
            else if (gBattleTypeFlags & BATTLE_TYPE_LINK)
                stringPtr = sText_LinkTrainerSentOutPkmn;
            else
            {
                stringPtr = sText_Trainer1SentOutPkmn;
#ifdef GERMAN
                stringPtr = de_sub_804110C(0xFFFF, stringPtr);
#endif
            }
        }
        break;
    case 2: // sending poke to ball msg
        if (GetBattlerSide(gActiveBattler) == 0)
        {
            if (gBattleStruct->hpScale == 0)
                stringPtr = sText_PkmnThatsEnough;
            else if (gBattleStruct->hpScale == 1 || gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                stringPtr = sText_PkmnComeBack;
            else if (gBattleStruct->hpScale == 2)
                stringPtr = sText_PkmnOkComeBack;
            else
                stringPtr = sText_PkmnGoodComeBack;
        }
        else
        {
            if (gTrainerBattleOpponent_A == LINK_BATTLE_OPPONENT)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_LinkTrainer2WithdrewPkmn;
                else
                    stringPtr = sText_LinkTrainer1WithdrewPkmn;
            }
            else
            {
                stringPtr = sText_Trainer1WithdrewPkmn;
#ifdef GERMAN
                stringPtr = de_sub_804110C(0xFFFF, stringPtr);
#endif
            }
        }
        break;
    case 3: // switch-in msg
        if (GetBattlerSide(gBattleStruct->battler) == 0)
        {
            if (gBattleStruct->hpScale == 0 || gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                stringPtr = sText_GoPkmn2;
            else if (gBattleStruct->hpScale == 1)
                stringPtr = sText_DoItPkmn;
            else if (gBattleStruct->hpScale == 2)
                stringPtr = sText_GoForItPkmn;
            else
                stringPtr = sText_YourFoesWeakGetEmPkmn;
        }
        else
        {
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_LinkTrainerMultiSentOutPkmn;
                else
                    stringPtr = sText_LinkTrainerSentOutPkmn2;
            }
            else
            {
                stringPtr = sText_Trainer1SentOutPkmn2;
#ifdef GERMAN
                stringPtr = de_sub_804110C(0xFFFF, stringPtr);
#endif
            }
        }
        break;
    case 4: // pokemon used a move msg
        ChooseMoveUsedParticle(gBattleTextBuff1);
        if (gBattleMsgDataPtr->currentMove > 0x162)
            StringCopy(gBattleTextBuff2, sATypeMove_Table[gBattleStruct->stringMoveType]);
        else
            StringCopy(gBattleTextBuff2, gMoveNames[gBattleMsgDataPtr->currentMove]);
        ChooseTypeOfMoveUsedString(gBattleTextBuff2);
        stringPtr = sText_AttackerUsedX;
        break;
    case 5: // battle end
        if (gBattleTextBuff1[0] & 0x80)
        {
            gBattleTextBuff1[0] &= ~(0x80);
            if (GetBattlerSide(gActiveBattler) == 1 && gBattleTextBuff1[0] != 3)
                gBattleTextBuff1[0] ^= 3;
            if (gBattleTextBuff1[0] == B_OUTCOME_LOST || gBattleTextBuff1[0] == B_OUTCOME_DREW)
                stringPtr = sText_GotAwaySafely;
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_TwoWildFled;
                else
                    stringPtr = sText_WildFled;
            }
        }
        else
        {
            if (GetBattlerSide(gActiveBattler) == 1 && gBattleTextBuff1[0] != 3)
                gBattleTextBuff1[0] ^= 3;
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    stringPtr = sText_TwoLinkTrainersDefeated;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostToTwo;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawVsTwo;
                    break;
                }
            }
            else
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    stringPtr = sText_PlayerDefeatedLinkTrainer;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostAgainstLinkTrainer;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawLinkTrainer;
                    break;
                }
            }
        }
        break;
    default: // load a string from the table
        if (stringID >= BATTLESTRINGS_MAX)
        {
            gDisplayedStringBattle[0] = EOS;
            return;
        }
        else
        {
            stringPtr = gBattleStringsTable[stringID - BATTLESTRINGS_TABLE_START];
#ifdef GERMAN
            stringPtr = de_sub_804110C(stringID, stringPtr);
#endif
        }
        break;
    }
    BattleStringExpandPlaceholdersToDisplayedString(stringPtr);
}

u32 BattleStringExpandPlaceholdersToDisplayedString(const u8* src)
{
    BattleStringExpandPlaceholders(src, gDisplayedStringBattle);
}

static const u8* TryGetStatusString(u8* src)
{
    u32 i;
    u8 status[8];
    u32 chars1, chars2;
    u8* statusPtr;

    memcpy(status, sText_EmptyStatus, 8);

    statusPtr = status;
    for (i = 0; i < sizeof(struct StatusFlagString); i++)
    {
        if (*src == EOS) break; // oneline required to match with -g
        *statusPtr = *src;
        src++;
        statusPtr++;
    }
    chars1 = *(u32*)(&status[0]);
    chars2 = *(u32*)(&status[4]);
    for (i = 0; i < 7; i++)
    {
        if (chars1 == *(u32*)(&gStatusConditionStringsTable[i].flag[0]) && chars2 == *(u32*)(&gStatusConditionStringsTable[i].flag[4]))
            return gStatusConditionStringsTable[i].ptr;
    }
    return NULL;
}

#ifdef GERMAN
extern u8 *de_sub_8073174(u8 *, const u8 *);
extern const u8 *de_sub_8041024(s32, u32);
#endif

#ifdef ENGLISH
#define HANDLE_NICKNAME_STRING_CASE(battler, monIndex)                     \
    if (GetBattlerSide(battler) != 0)                                         \
    {                                                                   \
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)                     \
            toCpy = sText_FoePkmnPrefix;                                  \
        else                                                            \
            toCpy = sText_WildPkmnPrefix;                                  \
        while (*toCpy != EOS)                                           \
        {                                                               \
            dst[dstID] = *toCpy;                                        \
            dstID++;                                                    \
            toCpy++;                                                    \
        }                                                               \
        GetMonData(&gEnemyParty[monIndex], MON_DATA_NICKNAME, text);    \
    }                                                                   \
    else                                                                \
    {                                                                   \
        GetMonData(&gPlayerParty[monIndex], MON_DATA_NICKNAME, text);   \
    }                                                                   \
    StringGet_Nickname(text);                                           \
    toCpy = text;
#else
#define HANDLE_NICKNAME_STRING_CASE(battler, monIndex)                     \
    if (GetBattlerSide(battler) != 0)                                         \
    {                                                                   \
        GetMonData(&gEnemyParty[monIndex], MON_DATA_NICKNAME, text);    \
        StringGet_Nickname(text);                                       \
        toCpy = text;                                                   \
        while (*toCpy != EOS)                                           \
        {                                                               \
            dst[dstID] = *toCpy;                                        \
            dstID++;                                                    \
            toCpy++;                                                    \
        }                                                               \
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)                     \
            toCpy = sText_FoePkmnPrefix;                                  \
        else                                                            \
            toCpy = sText_WildPkmnPrefix;                                  \
    }                                                                   \
    else                                                                \
    {                                                                   \
        GetMonData(&gPlayerParty[monIndex], MON_DATA_NICKNAME, text);   \
        StringGet_Nickname(text);                                       \
        toCpy = text;                                                   \
    }
#endif

u32 BattleStringExpandPlaceholders(const u8* src, u8* dst)
{
    u32 dstID = 0; // if they used dstID, why not use srcID as well?
    const u8* toCpy = NULL;
    u8 text[12];
    u8 multiplayerId = GetMultiplayerId();

    while (*src != EOS)
    {
        if (*src == B_BUFF_PLACEHOLDER_BEGIN)
        {
            src++;
            switch (*src)
            {
            case B_TXT_BUFF1:
                if (gBattleTextBuff1[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff1, gStringVar1);
                    toCpy = gStringVar1;
                }
                else
                {
                    toCpy = TryGetStatusString(gBattleTextBuff1);
                    if (toCpy == NULL)
                        toCpy = gBattleTextBuff1;
                }
                break;
            case B_TXT_BUFF2:
                if (gBattleTextBuff2[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff2, gStringVar2);
                    toCpy = gStringVar2;
                }
                else
                    toCpy = gBattleTextBuff2;
                break;
            case B_TXT_BUFF3:
                if (gBattleTextBuff3[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff3, gStringVar3);
                    toCpy = gStringVar3;
                }
                else
                    toCpy = gBattleTextBuff3;
                break;
            case B_TXT_PLAYER_MON1_NAME: // first player poke name
                GetMonData(&gPlayerParty[gBattlerPartyIndexes[GetBattlerAtPosition(0)]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_OPPONENT_MON1_NAME: // first enemy poke name
                GetMonData(&gEnemyParty[gBattlerPartyIndexes[GetBattlerAtPosition(1)]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_PLAYER_MON2_NAME: // second player poke name
                GetMonData(&gPlayerParty[gBattlerPartyIndexes[GetBattlerAtPosition(2)]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_OPPONENT_MON2_NAME: // second enemy poke name
                GetMonData(&gEnemyParty[gBattlerPartyIndexes[GetBattlerAtPosition(3)]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_LINK_PLAYER_MON1_NAME: // link first player poke name
                GetMonData(&gPlayerParty[gBattlerPartyIndexes[gLinkPlayers[multiplayerId].id]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_LINK_OPPONENT_MON1_NAME: // link first opponent poke name
                GetMonData(&gEnemyParty[gBattlerPartyIndexes[gLinkPlayers[multiplayerId].id ^ 1]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_LINK_PLAYER_MON2_NAME: // link second player poke name
                GetMonData(&gPlayerParty[gBattlerPartyIndexes[gLinkPlayers[multiplayerId].id ^ 2]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_LINK_OPPONENT_MON2_NAME: // link second opponent poke name
                GetMonData(&gEnemyParty[gBattlerPartyIndexes[gLinkPlayers[multiplayerId].id ^ 3]], MON_DATA_NICKNAME, text);
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX_MON1: // attacker name with prefix, only bank 0/1
                HANDLE_NICKNAME_STRING_CASE(gBattlerAttacker, gBattlerPartyIndexes[GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) & 1)])
                break;
            case B_TXT_ATK_PARTNER_NAME: // attacker partner name, only bank 0/1
                if (GetBattlerSide(gBattlerAttacker) == 0)
                    GetMonData(&gPlayerParty[gBattlerPartyIndexes[GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) & 1) + 2]], MON_DATA_NICKNAME, text);
                else
                    GetMonData(&gEnemyParty[gBattlerPartyIndexes[GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) & 1) + 2]], MON_DATA_NICKNAME, text);

                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX: // attacker name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker])
                break;
            case B_TXT_DEF_NAME_WITH_PREFIX: // target name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattlerTarget, gBattlerPartyIndexes[gBattlerTarget])
                break;
            case B_TXT_EFF_NAME_WITH_PREFIX: // effect bank name with prefix
                HANDLE_NICKNAME_STRING_CASE(gEffectBattler, gBattlerPartyIndexes[gEffectBattler])
                break;
            case B_TXT_ACTIVE_NAME_WITH_PREFIX: // active bank name with prefix
                HANDLE_NICKNAME_STRING_CASE(gActiveBattler, gBattlerPartyIndexes[gActiveBattler])
                break;
            case B_TXT_SCR_ACTIVE_NAME_WITH_PREFIX: // scripting active bank name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattleStruct->battler, gBattlerPartyIndexes[gBattleStruct->battler])
                break;
            case B_TXT_CURRENT_MOVE: // current move name
                if (gBattleMsgDataPtr->currentMove >= MOVES_COUNT)
                    toCpy = (void*) &sATypeMove_Table[gBattleStruct->stringMoveType];
                else
                    toCpy = gMoveNames[gBattleMsgDataPtr->currentMove];
                break;
            case B_TXT_LAST_MOVE: // last used move name
                if (gBattleMsgDataPtr->originallyUsedMove >= MOVES_COUNT)
                    toCpy = (void*) &sATypeMove_Table[gBattleStruct->stringMoveType];
                else
                    toCpy = gMoveNames[gBattleMsgDataPtr->originallyUsedMove];
                break;
            case B_TXT_LAST_ITEM: // last used item
                if (gBattleTypeFlags & BATTLE_TYPE_LINK)
                {
                    if (gLastUsedItem == ITEM_ENIGMA_BERRY)
                    {
                        if (gLinkPlayers[gBattleStruct->multiplayerId].id == gPotentialItemEffectBattler)
                        {
                            StringCopy(text, gEnigmaBerries[gPotentialItemEffectBattler].name);
#ifdef ENGLISH
                            StringAppend(text, sText_BerrySuffix);
#else
                            de_sub_8073174(text, sText_BerrySuffix);
#endif
                            toCpy = text;
                        }
                        else
                            toCpy = sText_EnigmaBerry;
                    }
                    else
                    {
                        CopyItemName(gLastUsedItem, text);
                        toCpy = text;
                    }
                }
                else
                {
                    CopyItemName(gLastUsedItem, text);
                    toCpy = text;
                }
                break;
            case B_TXT_LAST_ABILITY: // last used ability
                toCpy = gAbilityNames[gLastUsedAbility];
                break;
            case B_TXT_ATK_ABILITY: // attacker ability
                toCpy = gAbilityNames[sBattlerAbilities[gBattlerAttacker]];
                break;
            case B_TXT_DEF_ABILITY: // target ability
                toCpy = gAbilityNames[sBattlerAbilities[gBattlerTarget]];
                break;
            case B_TXT_SCR_ACTIVE_ABILITY: // scripting active ability
                toCpy = gAbilityNames[sBattlerAbilities[gBattleStruct->battler]];
                break;
            case B_TXT_EFF_ABILITY: // effect bank ability
                toCpy = gAbilityNames[sBattlerAbilities[gEffectBattler]];
                break;
            case B_TXT_TRAINER1_CLASS: // trainer class name
#ifdef ENGLISH
                if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
                    toCpy = gTrainerClassNames[GetSecretBaseTrainerClass()];
                else if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
                    toCpy = gTrainerClassNames[get_trainer_class_name_index()];
                else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
                    toCpy = gTrainerClassNames[GetEreaderTrainerClassId()];
                else
                    toCpy = gTrainerClassNames[gTrainers[gTrainerBattleOpponent_A].trainerClass];
                break;
#else
                if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
                    toCpy = de_sub_8041024(gTrainerBattleOpponent_A, 0);
                else if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
                    toCpy = de_sub_8041024(BATTLE_TYPE_BATTLE_TOWER, 0);
                else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
                    toCpy = de_sub_8041024(BATTLE_TYPE_EREADER_TRAINER, 0);
                else
                    toCpy = de_sub_8041024(0, gTrainerBattleOpponent_A);
                break;
#endif
            case B_TXT_TRAINER1_NAME: // trainer name
                if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
                {
                    memset(text, 0xFF, 8);
                    memcpy(text, eSecretBaseRecord->trainerName, 7);
                    toCpy = text;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_BATTLE_TOWER)
                {
                    get_trainer_name(text);
                    toCpy = text;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
                {
                    GetEreaderTrainerName(text);
                    toCpy = text;
                }
                else
                    toCpy = gTrainers[gTrainerBattleOpponent_A].trainerName;
                break;
            case B_TXT_LINK_PLAYER_NAME: // link player name?
                toCpy = gLinkPlayers[multiplayerId].name;
                break;
            case B_TXT_LINK_PARTNER_NAME: // link partner name?
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(2 ^ gLinkPlayers[multiplayerId].id)].name;
                break;
            case B_TXT_LINK_OPPONENT1_NAME: // link opponent 1 name?
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(1 ^ gLinkPlayers[multiplayerId].id)].name;
                break;
            case B_TXT_LINK_OPPONENT2_NAME: // link opponent 2 name?
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(3 ^ gLinkPlayers[multiplayerId].id)].name;
                break;
            case B_TXT_LINK_SCR_TRAINER_NAME: // link scripting active name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(gBattleStruct->battler)].name;
                break;
            case B_TXT_PLAYER_NAME: // player name
                toCpy = gSaveBlock2.playerName;
                break;
            case B_TXT_TRAINER1_LOSE_TEXT: // ?
                toCpy = GetTrainerALoseText();
                break;
            case B_TXT_22: // ?
                HANDLE_NICKNAME_STRING_CASE(gBattleStruct->battler, gBattleStruct->scriptPartyIdx)
                break;
            case B_TXT_PC_CREATOR_NAME: // lanette pc
                if (FlagGet(FLAG_SYS_PC_LANETTE))
                    toCpy = sText_Lanettes;
                else
                    toCpy = sText_Someones;
                break;
            case B_TXT_ATK_PREFIX2:
                if (GetBattlerSide(gBattlerAttacker) == 0)
                    toCpy = sText_AllyPkmnPrefix2;
                else
                    toCpy = sText_FoePkmnPrefix3;
                break;
            case B_TXT_DEF_PREFIX2:
                if (GetBattlerSide(gBattlerTarget) == 0)
                    toCpy = sText_AllyPkmnPrefix2;
                else
                    toCpy = sText_FoePkmnPrefix3;
                break;
            case B_TXT_ATK_PREFIX1:
                if (GetBattlerSide(gBattlerAttacker) == 0)
                    toCpy = sText_AllyPkmnPrefix;
                else
                    toCpy = sText_FoePkmnPrefix2;
                break;
            case B_TXT_DEF_PREFIX1:
                if (GetBattlerSide(gBattlerTarget) == 0)
                    toCpy = sText_AllyPkmnPrefix;
                else
                    toCpy = sText_FoePkmnPrefix2;
                break;
            case B_TXT_ATK_PREFIX3:
                if (GetBattlerSide(gBattlerAttacker) == 0)
                    toCpy = sText_AllyPkmnPrefix3;
                else
                    toCpy = sText_FoePkmnPrefix4;
                break;
            case B_TXT_DEF_PREFIX3:
                if (GetBattlerSide(gBattlerTarget) == 0)
                    toCpy = sText_AllyPkmnPrefix3;
                else
                    toCpy = sText_FoePkmnPrefix4;
                break;
            }
            //if (toCpy != NULL) really GF, why did you forget about this?
            while (*toCpy != EOS)
            {
                dst[dstID] = *toCpy;
                dstID++;
                toCpy++;
            }
            if (*src == B_TXT_TRAINER1_LOSE_TEXT)
            {
                dst[dstID] = EXT_CTRL_CODE_BEGIN;
                dstID++;
                dst[dstID] = EXT_CTRL_CODE_PAUSE_UNTIL_PRESS;
                dstID++;
            }
        }
        else
        {
            dst[dstID] = *src;
            dstID++;
        }
        src++;
    }
    dst[dstID] = *src;
    dstID++;
    return dstID;
}

static void ExpandBattleTextBuffPlaceholders(const u8* src, u8* dst)
{
    u32 srcID = 1;
    u32 value = 0;
    u8 nickname[12];
    u16 hword;

    *dst = EOS;
    while (src[srcID] != EOS)
    {
        switch (src[srcID])
        {
        case B_BUFF_STRING: // battle string
            hword = T1_READ_16(&src[srcID + 1]);
#ifdef GERMAN
            if (hword == 209 || hword == 211)
                srcID += 3;
#endif
            StringAppend(dst, gBattleStringsTable[hword - BATTLESTRINGS_TABLE_START]);
            srcID += 3;
            break;
        case B_BUFF_NUMBER: // int to string
            switch (src[srcID + 1])
            {
            case 1:
                value = T1_READ_8(&src[srcID + 3]);
                break;
            case 2:
                value = T1_READ_16(&src[srcID + 3]);
                break;
            case 4:
                value = T1_READ_32(&src[srcID + 3]);
                break;
            }
            ConvertIntToDecimalStringN(dst, value, 0, src[srcID + 2]);
            srcID += src[srcID + 1] + 3;
            break;
        case B_BUFF_MOVE: // move name
            StringAppend(dst, gMoveNames[T1_READ_16(&src[srcID + 1])]);
            srcID += 3;
            break;
        case B_BUFF_TYPE: // type name
            StringAppend(dst, gTypeNames[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_MON_NICK_WITH_PREFIX: // poke nick with prefix
#ifdef ENGLISH
            if (GetBattlerSide(src[srcID + 1]) == 0)
            {
                GetMonData(&gPlayerParty[src[srcID + 2]], MON_DATA_NICKNAME, nickname);
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                    StringAppend(dst, sText_FoePkmnPrefix);
                else
                    StringAppend(dst, sText_WildPkmnPrefix);
                GetMonData(&gEnemyParty[src[srcID + 2]], MON_DATA_NICKNAME, nickname);
            }
            StringGet_Nickname(nickname);
            StringAppend(dst, nickname);
#else
            if (GetBattlerSide(src[srcID + 1]) == 0)
            {
                GetMonData(&gPlayerParty[src[srcID + 2]], MON_DATA_NICKNAME, nickname);
                StringGet_Nickname(nickname);
                StringAppend(dst, nickname);
            }
            else
            {
                GetMonData(&gEnemyParty[src[srcID + 2]], MON_DATA_NICKNAME, nickname);
                StringGet_Nickname(nickname);
                StringAppend(dst, nickname);
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                    StringAppend(dst, sText_FoePkmnPrefix);
                else
                    StringAppend(dst, sText_WildPkmnPrefix);
            }
#endif
            srcID += 3;
            break;
        case B_BUFF_STAT: // stats
            StringAppend(dst, gStatNamesTable[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_SPECIES: // species name
            GetSpeciesName(dst, T1_READ_16(&src[srcID + 1]));
            srcID += 3;
            break;
        case B_BUFF_MON_NICK: // poke nick without prefix
            if (GetBattlerSide(src[srcID + 1]) == 0)
                GetMonData(&gPlayerParty[src[srcID + 2]], MON_DATA_NICKNAME, dst);
            else
                GetMonData(&gEnemyParty[src[srcID + 2]], MON_DATA_NICKNAME, dst);
            StringGet_Nickname(dst);
            srcID += 3;
            break;
        case B_BUFF_NEGATIVE_FLAVOR: // flavour table
            StringAppend(dst, gPokeblockWasTooXStringTable[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_ABILITY: // ability names
            StringAppend(dst, gAbilityNames[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_ITEM: // item name
            {
                hword = T1_READ_16(&src[srcID + 1]);
                if (gBattleTypeFlags & BATTLE_TYPE_LINK)
                {
                    if (hword == ITEM_ENIGMA_BERRY)
                    {
                        if (gLinkPlayers[gBattleStruct->multiplayerId].id == gPotentialItemEffectBattler)
                        {
                            StringCopy(dst, gEnigmaBerries[gPotentialItemEffectBattler].name);
#ifdef ENGLISH
                            StringAppend(dst, sText_BerrySuffix);
#else
                            de_sub_8073174(dst, sText_BerrySuffix);
#endif
                        }
                        else
                            StringAppend(dst, sText_EnigmaBerry);
                    }
                    else
                        CopyItemName(hword, dst);
                }
                else
                    CopyItemName(hword, dst);
                srcID += 3;
            }
            break;
        }
    }
}

// Loads one of two text strings into the provided buffer. This is functionally
// unused, since the value loaded into the buffer is not read; it loaded one of
// two particles (either "は" or "の") which works in tandem with ChooseTypeOfMoveUsedString
// below to effect changes in the meaning of the line.
static void ChooseMoveUsedParticle(u8* textBuff)
{
    s32 counter = 0;
    u32 i = 0;

    while (counter != 4)
    {
        if (sGrammarMoveUsedTable[i] == 0)
            counter++;
        if (sGrammarMoveUsedTable[i++] == gBattleMsgDataPtr->currentMove)
            break;
    }

    if (counter >= 0)
    {
        if (counter <= 2)
            StringCopy(textBuff, sText_SpaceIs); // is
        else if (counter <= 4)
            StringCopy(textBuff, sText_ApostropheS); // 's
    }
}

// Appends "!" to the text buffer `dst`. In the original Japanese this looked
// into the table of moves at gUnknown_084016BC and varied the line accordingly.
// 
// sText_ExclamationMark was a plain "!", used for any attack not on the list.
// It resulted in the translation "<NAME>'s <ATTACK>!".
// 
// sText_ExclamationMark2 was "を つかった！". This resulted in the translation
// "<NAME> used <ATTACK>!", which was used for all attacks in English.
// 
// sText_ExclamationMark3 was "した！". This was used for those moves whose
// names were verbs, such as Recover, and resulted in translations like "<NAME>
// recovered itself!".
// 
// sText_ExclamationMark4 was "を した！" This resulted in a translation of
// "<NAME> did an <ATTACK>!".
// 
// sText_ExclamationMark5 was " こうげき！" This resulted in a translation of
// "<NAME>'s <ATTACK> attack!".
static void ChooseTypeOfMoveUsedString(u8* dst)
{
    s32 counter = 0;
    s32 i = 0;

    while (*dst != EOS)
        dst++;

    while (counter != 4)
    {
        if (sGrammarMoveUsedTable[i] == 0)
            counter++;
        if (sGrammarMoveUsedTable[i++] == gBattleMsgDataPtr->currentMove)
            break;
    }

    switch (counter)
    {
    case 0:
        StringCopy(dst, sText_ExclamationMark);
        break;
    case 1:
        StringCopy(dst, sText_ExclamationMark2);
        break;
    case 2:
        StringCopy(dst, sText_ExclamationMark3);
        break;
    case 3:
        StringCopy(dst, sText_ExclamationMark4);
        break;
    case 4:
        StringCopy(dst, sText_ExclamationMark5);
        break;
    }
}
