#include "global.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "gba/flash_internal.h"
#include "battle.h"
#include "battle_gfx_sfx_util.h"
#include "battle_main.h"
#include "battle_ai_script_commands.h"
#include "battle_ai_switch_items.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_message.h"
#include "battle_setup.h"
#include "battle_util.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "evolution_scene.h"
#include "item.h"
#include "item_menu.h"
#include "link.h"
#include "main.h"
#include "m4a.h"
#include "international_string_util.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokeball.h"
#include "pokeblock.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "roamer.h"
#include "battle_anim.h"
#include "safari_zone.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "trig.h"
#include "tv.h"
#include "scanline_effect.h"
#include "util.h"
#include "ewram.h"
#include "constants/battle_string_ids.h"
#include "constants/trainers.h"

void SpriteCB_UnusedBattleInit_Main(struct Sprite *);
void SpriteCB_Flicker(struct Sprite *);
void SpriteCB_MoveWildMonToRight(struct Sprite *sprite);
void SpriteCB_WildMonShowHealthbox(struct Sprite *sprite);
void SpriteCB_AnimFaintOpponent(struct Sprite *);
void SpriteCB_BounceEffect(struct Sprite *);
void BattleIntroDrawPartySummaryScreens(void);
void BattleIntroPrintTrainerWantsToBattle(void);
void BattleIntroPrintWildMonAttacked(void);

static void CB2_InitBattleInternal(void);
static void CB2_HandleStartBattle(void);
static void CB2_HandleStartMultiBattle(void);
static u8 CreateNPCTrainerParty(struct Pokemon *, u16);
static void BattleStartClearSetData(void);
static void BattleIntroGetMonsData(void);
static void BattleIntroPrintPlayerSendsOut(void);

struct UnknownStruct7
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
};

struct UnknownStruct8
{
    u8 unk0[7];
    u8 unk7;
    u8 unk8[18];
    u8 unk1A;
};

extern void sub_802BBD4(u8 r0, u8 r1, u8 r2, u8 r3, u8 sp0);
#if DEBUG
extern void HandleInputChooseAction(void);
#endif

extern const u8 Str_821F7B8[];
extern u8 gUnknown_02023A14_50;
extern const u16 gBattleTextboxPalette[];
extern const struct MonCoords gCastformFrontSpriteCoords[];
extern const u8 Str_821F7EA[];
extern const u8 gUnknown_Debug_821F7F3[];
extern const u8 gText_BattleYesNoChoice[];
extern u8 gActionsByTurnOrder[4];
extern struct MultiBattlePokemonTx gMultiPartnerParty[];
extern u8 gBattleBufferB[][0x200];
extern u8 gBattlersCount;
extern u8 gCurrentActionFuncId;
extern u8 gBattlerByTurnOrder[];
extern u16 gCurrentMove;  // This is mis-named. It is a species, not a move ID.
extern u8 gLastUsedAbility;
extern u8 gPotentialItemEffectBattler;
extern u8 gMultiHitCounter;
extern u16 gLastPrintedMoves[];
extern u16 gLastMoves[];
extern u16 gLastLandedMoves[];
extern u16 gLastHitByType[];
extern u16 gLastResultingMoves[];
extern u16 gLockedMoves[];
extern u32 gHitMarker;
extern u8 sUnusedBattlersArray[];
extern u16 gSideStatuses[];
extern u32 gStatuses3[];
//extern u8 gDisableStructs[][0x1C];
extern u16 gPauseCounterBattle;
extern u16 gPaydayMoney;
extern u16 gRandomTurnNumber;
extern u8 gBattleCommunication[];
extern u8 gUnknown_02024D1F[];  // I don't actually know what type this is.
extern u8 gBattleOutcome;
MainCallback gPreBattleCallback1;
u8 gMultiUsePlayerCursor;
u8 gNumberOfMovesToChoose;
extern void (*gBattleMainFunc)(void);
u8 gLeveledUpInBattle;
void (*gBattlerControllerFuncs[MAX_BATTLERS_COUNT])(void);
u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
u8 gBattleControllerData[MAX_BATTLERS_COUNT];
extern u16 gBattleTypeFlags;
extern u8 gReservedSpritePaletteCount;
extern u16 gTrainerBattleOpponent_A;
extern struct BattleEnigmaBerry gEnigmaBerries[];
extern u16 gBlockRecvBuffer[MAX_LINK_PLAYERS][BLOCK_BUFFER_SIZE / 2];
extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u16 gBattleWeather;
extern s32 gBattleMoveDamage;
extern u8 gMoveResultFlags;
extern u8 BattleScript_FocusPunchSetUp[];
extern u16 gDynamicBasePower;
extern u8 gCurrentTurnActionNumber;
extern void (* const sTurnActionsFuncsTable[])(void);
extern void (* const sEndTurnFuncsTable[])(void);
extern u8* gBattlescriptCurrInstr;
extern u8 BattleScript_LinkBattleWonOrLost[];
extern u8 BattleScript_PayDayMoneyAndPickUpItems[];
extern u8 gUnknown_081D8E0D[];
extern u8 BattleScript_LocalTrainerBattleWon[];
extern u8 BattleScript_LocalBattleLost[];
extern u8 BattleScript_GotAwaySafely[];
extern u8 BattleScript_SmokeBallEscape[];
extern u8 BattleScript_RanAwayUsingMonAbility[];
extern u8 BattleScript_WildMonFled[];
extern u8 BattleScript_ActionSwitch[];
extern u8 BattleScript_PrintFailedToRunString[];
extern const BattleCmdFunc gBattleScriptingCommandsTable[];
extern u8 gCritMultiplier;
extern u8 gCurrMovePos;
extern u16 gChosenMove;
extern u8* gBattleScriptsForMoveEffects[];
extern u16 gLastUsedItem;
extern u8 * const gBattlescriptsForBallThrow[];
extern u8 * const gBattlescriptsForRunningByItem[];
extern u8 * const gBattlescriptsForUsingItem[];
extern u8 * const gBattlescriptsForSafariActions[];
extern u8 gBattleTextBuff2[];
extern u8 gNumSafariBalls;
extern u8 sPkblToEscapeFactor[][3];
extern u8 sGoNearCounterToCatchFactor[];
extern u8 sGoNearCounterToEscapeFactor[];
struct BattleResults gBattleResults;

static void BattleMainCB1(void);
static void SpriteCB_BlinkVisible(struct Sprite *sprite);
static void SpriteCB_BattleSpriteSlideLeft(struct Sprite *sprite);
static void SpriteCB_Idle(struct Sprite *sprite);
static void BattleIntroPrepareBackgroundSlide(void);
static void CheckFocusPunch_ClearVarsBeforeTurnStarts(void);
void SetActionsAndBanksTurnOrder(void);
static void TurnValuesCleanUp(u8);
static void RunTurnActionsFunctions(void);
void HandleEndTurn_FinishBattle();
static void FreeResetData_ReturnToOvOrDoEvolutions(void);
static void TryEvolvePokemon(void);
static void ReturnFromBattleToOverworld(void);
static void WaitForEvoSceneToFinish(void);


u16 gBattle_BG0_X;
u16 gBattle_BG0_Y;
u16 gBattle_BG1_X;
u16 gBattle_BG1_Y;
u16 gBattle_BG2_X;
u16 gBattle_BG2_Y;
u16 gBattle_BG3_X;
u16 gBattle_BG3_Y;
u16 gBattle_WIN0H;
u16 gBattle_WIN0V;
u16 gBattle_WIN1H;
u16 gBattle_WIN1V;

u8 gBattleTextBuff1[TEXT_BUFF_ARRAY_COUNT];
u8 gBattleTextBuff2[TEXT_BUFF_ARRAY_COUNT];
u8 gBattleTextBuff3[TEXT_BUFF_ARRAY_COUNT];

s8 gBattleEnvironment;

void (*gBattleMainFunc)(void);

u32 gUnknown_03004284;
struct Window gWindowTemplate_Contest_MoveDescription;
struct Window gUnknown_030041D0;
struct Window gUnknown_03004250;

void CB2_InitBattle(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        HandleLinkBattleSetup();
        SetMainCallback2(sub_800F104);
        gBattleCommunication[0] = 0;
    }
    else
    {
        CB2_InitBattleInternal();
    }
}

static void CB2_InitBattleInternal(void)
{
    s32 i;

    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);

    CpuFill32(0, (void *)VRAM, VRAM_SIZE);

    REG_MOSAIC = 0;
    REG_WIN0H = 0xF0;
    REG_WIN0V = 0x5051;
    REG_WININ = 0;
    REG_WINOUT = 0;
    gBattle_WIN0H = 0xF0;
    gBattle_WIN0V = 0x5051;
    ScanlineEffect_Clear();

    for (i = 0; i < 80; i++)
    {
        gScanlineEffectRegBuffers[0][i] = 0xF0;
        gScanlineEffectRegBuffers[1][i] = 0xF0;
    }
    for (; i < 160; i++)
    {
        gScanlineEffectRegBuffers[0][i] = 0xFF10;
        gScanlineEffectRegBuffers[1][i] = 0xFF10;
    }
    ScanlineEffect_SetParams(sIntroScanlineParams16Bit);
    Text_LoadWindowTemplate(&gWindowTemplate_81E6C58);
    ResetPaletteFade();
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;

#if DEBUG
    if (!(gUnknown_02023A14_50 & 8))
        gBattleEnvironment = BattleSetup_GetEnvironmentId();
#else
    gBattleEnvironment = BattleSetup_GetEnvironmentId();
#endif

    Text_InitWindowWithTemplate(&gWindowTemplate_Contest_MoveDescription, &gWindowTemplate_81E6C58);
    Text_InitWindowWithTemplate(&gUnknown_030041D0, &gWindowTemplate_81E71D0);
    Text_InitWindowWithTemplate(&gUnknown_03004250, &gWindowTemplate_81E71EC);
    sub_800D6D4();
    LoadBattleTextboxAndBackground();
    ResetSpriteData();
    ResetTasks();
    DrawBattleEntryBackground();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 4;
    SetVBlankCallback(VBlankCB_Battle);
    SetUpBattleVarsAndBirchPoochyena();
    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        SetMainCallback2(CB2_HandleStartMultiBattle);
    else
        SetMainCallback2(CB2_HandleStartBattle);
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK)
#if DEBUG
     && !(gUnknown_02023A14_50 & 8)
#endif
       )
    {
        CreateNPCTrainerParty(gEnemyParty, gTrainerBattleOpponent_A);
        SetWildMonHeldItem();
    }
    gMain.inBattle = TRUE;
    for (i = 0; i < PARTY_SIZE; i++)
        AdjustFriendship(&gPlayerParty[i], FRIENDSHIP_EVENT_LEAGUE_BATTLE);
    gBattleCommunication[0] = 0;
}

static void BufferPartyVsScreenHealth_AtStart(void)
{
    u16 flags = 0;
    u16 species;
    u16 hp;
    u32 status;
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
        hp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
        status = GetMonData(&gPlayerParty[i], MON_DATA_STATUS);

        if (species == SPECIES_NONE)
            continue;
        if (species != SPECIES_EGG && hp != 0 && status == 0)
            flags |= 1 << i * 2;

        if (species == SPECIES_NONE)
            continue;
        if (hp != 0 && (species == SPECIES_EGG || status != 0))
            flags |= 2 << i * 2;

        if (species == SPECIES_NONE)
            continue;
        if (species != SPECIES_EGG && hp == 0)
            flags |= 3 << i * 2;
    }
    eMultiTxBuffer.linkBattlerHeader.vsScreenHealthFlagsLo = flags;
    eMultiTxBuffer.linkBattlerHeader.vsScreenHealthFlagsHi = flags >> 8;
}

static void SetPlayerBerryDataInBattleStruct(void)
{
    s32 i;
    struct BattleEnigmaBerry * battleBerry = &eMultiTxBuffer.linkBattlerHeader.battleEnigmaBerry;

    for (i = 0; i < 7; i++)
        battleBerry->name[i] = gSaveBlock1.enigmaBerry.berry.name[i];
    for (i = 0; i < 18; i++)
        battleBerry->itemEffect[i] = gSaveBlock1.enigmaBerry.itemEffect[i];
    battleBerry->holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
    battleBerry->holdEffectParam = gSaveBlock1.enigmaBerry.holdEffectParam;
}

static void SetAllPlayersBerryData(void)
{
    s32 i;
    s32 j;

    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        for (i = 0; i < 7; i++)
        {
            gEnigmaBerries[0].name[i] = gSaveBlock1.enigmaBerry.berry.name[i];
            gEnigmaBerries[2].name[i] = gSaveBlock1.enigmaBerry.berry.name[i];
        }
        for (i = 0; i < 18; i++)
        {
            gEnigmaBerries[0].itemEffect[i] = gSaveBlock1.enigmaBerry.itemEffect[i];
            gEnigmaBerries[2].itemEffect[i] = gSaveBlock1.enigmaBerry.itemEffect[i];
        }
        gEnigmaBerries[0].holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
        gEnigmaBerries[2].holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
        gEnigmaBerries[0].holdEffectParam = gSaveBlock1.enigmaBerry.holdEffectParam;
        gEnigmaBerries[2].holdEffectParam = gSaveBlock1.enigmaBerry.holdEffectParam;
    }
    else
    {
        s32 numPlayers;
        struct BattleEnigmaBerry *src;
        u8 battler;

        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            numPlayers = 4;
        else
            numPlayers = 2;
        for (i = 0; i < numPlayers; i++)
        {
            src = (struct BattleEnigmaBerry *)(gBlockRecvBuffer[i] + 2);
            battler = gLinkPlayers[i].id;

            for (j = 0; j < 7; j++)
                gEnigmaBerries[battler].name[j] = src->name[j];
            for (j = 0; j < 18; j++)
                gEnigmaBerries[battler].itemEffect[j] = src->itemEffect[j];
            gEnigmaBerries[battler].holdEffect = src->holdEffect;
            gEnigmaBerries[battler].holdEffectParam = src->holdEffectParam;
        }
    }
}

static void TryCorrectShedinjaLanguage(struct Pokemon *mon)
{
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u8 language = 1;

    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_SHEDINJA
     && GetMonData(mon, MON_DATA_LANGUAGE) != language)
    {
        GetMonData(mon, MON_DATA_NICKNAME, nickname);
        if (StringCompareWithoutExtCtrlCodes(nickname, sText_ShedinjaJpnName) == 0)
            SetMonData(mon, MON_DATA_LANGUAGE, &language);
    }
}

static void CB2_HandleStartBattle(void)
{
    u8 playerMultiplayerId;
    u8 enemyMultiplayerId;
    s32 id;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    playerMultiplayerId = GetMultiplayerId();
    gBattleStruct->multiplayerId = playerMultiplayerId;
    enemyMultiplayerId = playerMultiplayerId ^ 1;

    switch (gBattleCommunication[0])
    {
    case 0:
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            if (gReceivedRemoteLinkPlayers != 0 && IsLinkTaskFinished())
            {
                eMultiTxBuffer.linkBattlerHeader.versionSignatureLo = 1;
                eMultiTxBuffer.linkBattlerHeader.versionSignatureHi = 1;
                BufferPartyVsScreenHealth_AtStart();
                SetPlayerBerryDataInBattleStruct();
#if DEBUG
                if (gUnknown_02023A14_50 & 8)
                {
                    for (id = 0; id < 2; id++)  // Why < 2 here?
                    {
                        gLinkPlayers[id].id = id;
                        gLinkPlayers[id].linkType = 0x2211;
                    }
                }
#endif
                SendBlock(BitmaskAllOtherLinkPlayers(), &eMultiTxBuffer.linkBattlerHeader, sizeof(eMultiTxBuffer.linkBattlerHeader));
                gBattleCommunication[0] = 1;
            }
        }
        else
        {
            gBattleTypeFlags |= BATTLE_TYPE_IS_MASTER;
            gBattleCommunication[0] = 8;
            SetAllPlayersBerryData();
        }
        break;
    case 1:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            u8 taskId;

            ResetBlockReceivedFlags();
            id = 0;
            if (gBlockRecvBuffer[0][0] == 0x100)
            {
                if (playerMultiplayerId == 0)
                    gBattleTypeFlags |= 12;
                else
                    gBattleTypeFlags |= 8;
                id++;
            }
            if (id == 0)
            {
                if (gBlockRecvBuffer[0][0] == gBlockRecvBuffer[1][0])
                {
                    if (playerMultiplayerId == 0)
                        gBattleTypeFlags |= 12;
                    else
                        gBattleTypeFlags |= 8;
                    id++;
                }
                if (id == 0)
                {
                    while (id < 2)
                    {
                        if (gBlockRecvBuffer[id][0] > 0x0101 && id != playerMultiplayerId)
                            break;
                        id++;
                    }
                    if (id == 2)
                        gBattleTypeFlags |= 12;
                    else
                        gBattleTypeFlags |= 8;
                }
            }
            SetAllPlayersBerryData();
            taskId = CreateTask(InitLinkBattleVsScreen, 0);
            gTasks[taskId].data[1] = 0x10E;
            gTasks[taskId].data[2] = 0x5A;
            gTasks[taskId].data[5] = 0;
            gTasks[taskId].data[3] = eMultiTxBuffer.linkBattlerHeader.vsScreenHealthFlagsLo | (eMultiTxBuffer.linkBattlerHeader.vsScreenHealthFlagsHi << 8);
            gTasks[taskId].data[4] = gBlockRecvBuffer[enemyMultiplayerId][1];
            gBattleCommunication[0]++;
        }
        break;
    case 2:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gPlayerParty, sizeof(*gPlayerParty) * 2);
            gBattleCommunication[0]++;
        }
        break;
    case 3:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty, gBlockRecvBuffer[enemyMultiplayerId], sizeof(*gEnemyParty) * 2);
            gBattleCommunication[0]++;
        }
        break;
    case 4:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gPlayerParty + 2, sizeof(*gPlayerParty) * 2);
            gBattleCommunication[0]++;
        }
        break;
    case 5:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty + 2, gBlockRecvBuffer[enemyMultiplayerId], sizeof(*gEnemyParty) * 2);
            gBattleCommunication[0]++;
        }
        break;
    case 6:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), gPlayerParty + 4, sizeof(*gPlayerParty) * 2);
            gBattleCommunication[0]++;
        }
        break;
    case 7:
        if ((GetBlockReceivedStatus() & 3) == 3)
        {
            ResetBlockReceivedFlags();
            memcpy(gEnemyParty + 4, gBlockRecvBuffer[enemyMultiplayerId], sizeof(*gEnemyParty) * 2);
            TryCorrectShedinjaLanguage(&gEnemyParty[0]);
            TryCorrectShedinjaLanguage(&gEnemyParty[1]);
            TryCorrectShedinjaLanguage(&gEnemyParty[2]);
            TryCorrectShedinjaLanguage(&gEnemyParty[3]);
            TryCorrectShedinjaLanguage(&gEnemyParty[4]);
            TryCorrectShedinjaLanguage(&gEnemyParty[5]);
            gBattleCommunication[0]++;
        }
        break;
    case 8:
        sub_800B950();
        gBattleCommunication[0]++;
        gBattleCommunication[1] = 0;
        gBattleCommunication[2] = 0;
        break;
    case 9:
        if (BattleInitAllSprites(gUnknown_02024D1F, gUnknown_02024D1F + 1) != 0)
        {
            gPreBattleCallback1 = gMain.callback1;
            gMain.callback1 = BattleMainCB1;
            SetMainCallback2(BattleMainCB2);
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                gTrainerBattleOpponent_A = LINK_BATTLE_OPPONENT;
                gBattleTypeFlags |= BATTLE_TYPE_20;
            }
        }
        break;
    }
}

void PrepareOwnMultiPartnerBuffer(void)
{
    s32 i;

    for (i = 0; i < 3; i++)
    {
        u8 *nickname = gMultiPartnerParty[i].nickname;

        gMultiPartnerParty[i].species     = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        gMultiPartnerParty[i].heldItem    = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
        GetMonData(&gPlayerParty[i], MON_DATA_NICKNAME, nickname);
        gMultiPartnerParty[i].level       = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
        gMultiPartnerParty[i].hp          = GetMonData(&gPlayerParty[i], MON_DATA_HP);
        gMultiPartnerParty[i].maxhp       = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
        gMultiPartnerParty[i].status      = GetMonData(&gPlayerParty[i], MON_DATA_STATUS);
        gMultiPartnerParty[i].personality = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY);
        gMultiPartnerParty[i].gender      = GetMonGender(&gPlayerParty[i]);
        StripExtCtrlCodes(nickname);
        gMultiPartnerParty[i].language    = GetMonData(&gPlayerParty[i], MON_DATA_LANGUAGE);
        if (gMultiPartnerParty[i].language != 1)
            PadNameString(nickname, 0);
    }
    memcpy(eMultiTxBuffer.multiBattleMons, gMultiPartnerParty, 3 * sizeof(struct MultiBattlePokemonTx));
}

void sub_800F104(void)
{
    u8 playerId;
    MainCallback *pSavedCallback;
    u16 *pSavedBattleTypeFlags;
    s32 i;

    playerId = GetMultiplayerId();
    gBattleStruct->multiplayerId = playerId;
    // Seriously, Game Freak?
    pSavedCallback = &gBattleStruct->savedCallback;
    pSavedBattleTypeFlags = &gBattleStruct->savedBattleTypeFlags;
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    switch (gBattleCommunication[0])
    {
    case 0:
        if (gReceivedRemoteLinkPlayers != 0)
        {
#if DEBUG
            if (gUnknown_02023A14_50 & 8)
            {
                for (i = 0; i < 4; i++)
                {
                    gLinkPlayers[i].id = i;
                    gLinkPlayers[i].linkType = 0x2211;
                }
            }
#endif
            if (IsLinkTaskFinished())
            {
                PrepareOwnMultiPartnerBuffer();
                SendBlock(BitmaskAllOtherLinkPlayers(), eMultiTxBuffer.multiBattleMons, 3 * sizeof(struct MultiBattlePokemonTx));
                gBattleCommunication[0]++;
            }
        }
        break;
    case 1:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            //s32 i;
            // Get your multi battle partner's party
            ResetBlockReceivedFlags();
            for (i = 0; i < 4; i++)
            {
                if (i != playerId)
                {
                    if ((!(gLinkPlayers[i].id & 1) && !(gLinkPlayers[playerId].id & 1))
                     || ((gLinkPlayers[i].id & 1) && (gLinkPlayers[playerId].id & 1)))
                        memcpy(gMultiPartnerParty, gBlockRecvBuffer[i], 3 * sizeof(struct MultiBattlePokemonTx));
                }
            }
            gBattleCommunication[0]++;
            *pSavedCallback = gMain.savedCallback;
            *pSavedBattleTypeFlags = gBattleTypeFlags;
            gMain.savedCallback = sub_800F104;
            OpenPartyMenu(PARTY_MENU_TYPE_LINK_MULTI_BATTLE, 0);
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            gBattleCommunication[0] = 3;
            SetCloseLinkCallback();
        }
        break;
    case 3:
        if (gReceivedRemoteLinkPlayers == 0)
        {
            gBattleTypeFlags = *pSavedBattleTypeFlags;
            gMain.savedCallback = *pSavedCallback;
            SetMainCallback2(CB2_InitBattleInternal);
        }
        break;
    }
}

static void CB2_HandleStartMultiBattle(void)
{
    u8 playerMultiplayerId;
    s32 id;

    playerMultiplayerId = GetMultiplayerId();
    gBattleStruct->multiplayerId = playerMultiplayerId;
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    switch (gBattleCommunication[0])
    {
    case 0:
        if (gReceivedRemoteLinkPlayers != 0)
        {
#if DEBUG
            if (gUnknown_02023A14_50 & 8)
            {
                for (id = 0; id < 4; id++)
                {
                    gLinkPlayers[id].id = id;
                    gLinkPlayers[id].linkType = 0x2211;
                }
            }
#endif
            if (IsLinkTaskFinished())
            {
                *(&eMultiTxBuffer.linkBattlerHeader.versionSignatureLo) = 1;
                *(&eMultiTxBuffer.linkBattlerHeader.versionSignatureHi) = 1;
                BufferPartyVsScreenHealth_AtStart();
                SetPlayerBerryDataInBattleStruct();
                SendBlock(BitmaskAllOtherLinkPlayers(), &eMultiTxBuffer.linkBattlerHeader, sizeof(eMultiTxBuffer.linkBattlerHeader));
                gBattleCommunication[0]++;
            }
        }
        break;
    case 1:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            u8 taskId;

            ResetBlockReceivedFlags();
            // LinkBattleComputeBattleTypeFlags
            id = 0;
            if (gBlockRecvBuffer[0][0] == 0x100)
            {
                if (playerMultiplayerId == 0)
                    gBattleTypeFlags |= 12;
                else
                    gBattleTypeFlags |= 8;
                id++;
            }
            if (id == 0)
            {
                s32 i;

                for (i = 0; i < MAX_LINK_PLAYERS; i++)
                {
                    if (gBlockRecvBuffer[0][0] != gBlockRecvBuffer[i][0])
                        break;
                }
                if (i == MAX_LINK_PLAYERS)
                {
                    if (playerMultiplayerId == 0)
                        gBattleTypeFlags |= 12;
                    else
                        gBattleTypeFlags |= 8;
                    id++;
                }
                if (id == 0)
                {
                    while (id < MAX_LINK_PLAYERS)
                    {
                        if (gBlockRecvBuffer[id][0] == 0x0101 && id != playerMultiplayerId)
                            if (id < playerMultiplayerId)
                                break;
                        if (gBlockRecvBuffer[id][0] > 0x0101 && id != playerMultiplayerId)
                            break;
                        id++;
                    }
                    if (id == MAX_LINK_PLAYERS)
                        gBattleTypeFlags |= 12;
                    else
                        gBattleTypeFlags |= 8;
                }
            }
            SetAllPlayersBerryData();
            memcpy(eMultiBattleSetupPartySendBuffer, gPlayerParty, sizeof(struct Pokemon) * 3);
            taskId = CreateTask(InitLinkBattleVsScreen, 0);
            gTasks[taskId].data[1] = 270;
            gTasks[taskId].data[2] = 90;
            gTasks[taskId].data[5] = 0;
            gTasks[taskId].data[3] = 0;
            gTasks[taskId].data[4] = 0;
            for (id = 0; id < MAX_LINK_PLAYERS; id++)
            {
                switch (gLinkPlayers[id].id)
                {
                case 0:
                    gTasks[taskId].data[3] |= gBlockRecvBuffer[id][1] & 0x3F;
                    break;
                case 1:
                    gTasks[taskId].data[4] |= gBlockRecvBuffer[id][1] & 0x3F;
                    break;
                case 2:
                    gTasks[taskId].data[3] |= (gBlockRecvBuffer[id][1] & 0x3F) << 6;
                    break;
                case 3:
                    gTasks[taskId].data[4] |= (gBlockRecvBuffer[id][1] & 0x3F) << 6;
                    break;
                }
            }
            ZeroPlayerPartyMons();
            ZeroEnemyPartyMons();
            gBattleCommunication[0]++;
            // fallthrough
    case 2:
            if (IsLinkTaskFinished())
            {
                SendBlock(BitmaskAllOtherLinkPlayers(), eMultiBattleSetupPartySendBuffer, sizeof(struct Pokemon) * 2);
                gBattleCommunication[0]++;
            }
	}
        break;
    case 3:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            for (id = 0; id < MAX_LINK_PLAYERS; id++)
            {
                if (id == playerMultiplayerId)
                {
                    switch (gLinkPlayers[id].id)
                    {
                    case 0:
                    case 3:
                        memcpy(gPlayerParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                        break;
                    case 1:
                    case 2:
                        memcpy(gPlayerParty + 3, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                        break;
                    }
                }
                else
                {
                    if ((!(gLinkPlayers[id].id & 1) && !(gLinkPlayers[playerMultiplayerId].id & 1))
                     || ((gLinkPlayers[id].id & 1) && (gLinkPlayers[playerMultiplayerId].id & 1)))
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gPlayerParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        case 1:
                        case 2:
                            memcpy(gPlayerParty + 3, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        }
                    }
                    else
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gEnemyParty, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        case 1:
                        case 2:
                            memcpy(gEnemyParty + 3, gBlockRecvBuffer[id], sizeof(struct Pokemon) * 2);
                            break;
                        }
                    }
                }
            }
            gBattleCommunication[0]++;
        }
        break;
    case 4:
        if (IsLinkTaskFinished())
        {
            SendBlock(BitmaskAllOtherLinkPlayers(), eMultiBattleSetupPartySendBuffer + 2, sizeof(struct Pokemon));
            gBattleCommunication[0]++;
        }
        break;
    case 5:
        if ((GetBlockReceivedStatus() & 0xF) == 0xF)
        {
            ResetBlockReceivedFlags();
            for (id = 0; id < MAX_LINK_PLAYERS; id++)
            {
                if (id == playerMultiplayerId)
                {
                    switch (gLinkPlayers[id].id)
                    {
                    case 0:
                    case 3:
                        memcpy(gPlayerParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                        break;
                    case 1:
                    case 2:
                        memcpy(gPlayerParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                        break;
                    }
                }
                else
                {
                    if ((!(gLinkPlayers[id].id & 1) && !(gLinkPlayers[playerMultiplayerId].id & 1))
                     || ((gLinkPlayers[id].id & 1) && (gLinkPlayers[playerMultiplayerId].id & 1)))
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gPlayerParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        case 1:
                        case 2:
                            memcpy(gPlayerParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        }
                    }
                    else
                    {
                        switch (gLinkPlayers[id].id)
                        {
                        case 0:
                        case 3:
                            memcpy(gEnemyParty + 2, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        case 1:
                        case 2:
                            memcpy(gEnemyParty + 5, gBlockRecvBuffer[id], sizeof(struct Pokemon));
                            break;
                        }
                    }
                }
            }

            TryCorrectShedinjaLanguage(&gPlayerParty[0]);
            TryCorrectShedinjaLanguage(&gPlayerParty[1]);
            TryCorrectShedinjaLanguage(&gPlayerParty[2]);
            TryCorrectShedinjaLanguage(&gPlayerParty[3]);
            TryCorrectShedinjaLanguage(&gPlayerParty[4]);
            TryCorrectShedinjaLanguage(&gPlayerParty[5]);

            TryCorrectShedinjaLanguage(&gEnemyParty[0]);
            TryCorrectShedinjaLanguage(&gEnemyParty[1]);
            TryCorrectShedinjaLanguage(&gEnemyParty[2]);
            TryCorrectShedinjaLanguage(&gEnemyParty[3]);
            TryCorrectShedinjaLanguage(&gEnemyParty[4]);
            TryCorrectShedinjaLanguage(&gEnemyParty[5]);

            gBattleCommunication[0]++;
        }
        break;
    case 6:
        sub_800B950();
        gBattleCommunication[0]++;
        gBattleCommunication[1] = 0;
        gBattleCommunication[2] = 0;
        break;
    case 7:
        if (BattleInitAllSprites(gUnknown_02024D1F, gUnknown_02024D1F + 1) != 0)
        {
            gPreBattleCallback1 = gMain.callback1;
            gMain.callback1 = BattleMainCB1;
            SetMainCallback2(BattleMainCB2);
            if (gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                gTrainerBattleOpponent_A = 0x800;
                gBattleTypeFlags |= BATTLE_TYPE_20;
            }
        }
        break;
    }
}

void BattleMainCB2(void)
{
    AnimateSprites();
    BuildOamBuffer();

#if DEBUG
    if (JOY_HELD(R_BUTTON | SELECT_BUTTON) == ((R_BUTTON | SELECT_BUTTON)))
    {
        gSpecialVar_Result = gBattleOutcome = 1;
        gMain.inBattle = FALSE;
        gScanlineEffect.state = 3;
        gMain.callback1 = gPreBattleCallback1;
        ZeroEnemyPartyMons();
        m4aSongNumStop(0x5A);
        if (gBattleTypeFlags & 2)
            SetMainCallback2(CB2_ReturnToFieldFromMultiplayer);
        else
            SetMainCallback2(gMain.savedCallback);
    }
    if (gBattleTypeFlags & 2)
    {
        debug_sub_8008264((gBattleTypeFlags >> 2) % 2, 1, 1, 1, 1);
        debug_sub_8008264((gBattleTypeFlags >> 2) % 2, 1, 21, 1, 1);
        debug_sub_8008264((gBattleTypeFlags >> 2) % 2, 1, 41, 1, 1);
    }
#endif

    Text_UpdateWindowInBattle(&gWindowTemplate_Contest_MoveDescription);
    UpdatePaletteFade();
    RunTasks();
}

void SpriteCB_UnusedBattleInit(struct Sprite *sprite)
{
    sprite->data[0] = 0;
    sprite->callback = SpriteCB_UnusedBattleInit_Main;
}

void SpriteCB_UnusedBattleInit_Main(struct Sprite *sprite)
{
    u16 *arr = (u16 *)gSharedMem;

    switch (sprite->data[0])
    {
    case 0:
        sprite->data[0]++;
        sprite->data[1] = 0;
        sprite->data[2] = 0x281;
        sprite->data[3] = 0;
        sprite->data[4] = 1;
        // fall through
    case 1:
        sprite->data[4]--;
        if (sprite->data[4] == 0)
        {
            s32 i;
            s32 r2;
            s32 r0;

            sprite->data[4] = 2;
            r2 = sprite->data[1] + sprite->data[3] * 32;
            r0 = sprite->data[2] - sprite->data[3] * 32;
            for (i = 0; i < 29; i += 2)
            {
                arr[r2 + i] = 0x3D;
                arr[r0 + i] = 0x3D;
            }
            sprite->data[3]++;
            if (sprite->data[3] == 21)
            {
                sprite->data[0]++;
                sprite->data[1] = 32;
            }
        }
        break;
    case 2:
        sprite->data[1]--;
        if (sprite->data[1] == 20)
            SetMainCallback2(CB2_InitBattle);
        break;
    }
}

static u8 CreateNPCTrainerParty(struct Pokemon *party, u16 trainerNum)
{
    u32 nameHash = 0;
    s32 i;

    if (trainerNum == 0x400)
        return 0;

    if ((gBattleTypeFlags & 0x908) == 8)
    {
        ZeroEnemyPartyMons();
        for (i = 0; i < gTrainers[trainerNum].partySize; i++)
        {
            u32 personalityValue;
            s32 j;
            u8 fixedIV;

            if (gTrainers[trainerNum].doubleBattle == TRUE)
                personalityValue = 0x80;
            else if (gTrainers[trainerNum].encounterMusic_gender & 0x80)
                personalityValue = 0x78;
            else
                personalityValue = 0x88;

            for (j = 0; gTrainers[trainerNum].trainerName[j] != 0xFF; j++)
                nameHash += gTrainers[trainerNum].trainerName[j];

            switch (gTrainers[trainerNum].partyFlags)
            {
            case 0:
            {
                const struct TrainerMonNoItemDefaultMoves *partyData = gTrainers[trainerNum].party.NoItemDefaultMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != 0xFF; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];
                personalityValue += nameHash << 8;
                fixedIV = partyData[i].iv * 31 / 255;
                CreateMon(&party[i], partyData[i].species, partyData[i].level, fixedIV, TRUE, personalityValue, 2, 0);
                break;
            }
            case F_TRAINER_PARTY_CUSTOM_MOVESET:
            {
                const struct TrainerMonNoItemCustomMoves *partyData = gTrainers[trainerNum].party.NoItemCustomMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != 0xFF; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];
                personalityValue += nameHash << 8;
                fixedIV = partyData[i].iv * 31 / 255;
                CreateMon(&party[i], partyData[i].species, partyData[i].level, fixedIV, TRUE, personalityValue, 2, 0);

                for (j = 0; j < 4; j++)
                {
                    SetMonData(&party[i], MON_DATA_MOVE1 + j, &partyData[i].moves[j]);
                    SetMonData(&party[i], MON_DATA_PP1 + j, &gBattleMoves[partyData[i].moves[j]].pp);
                }
                break;
            }
            case F_TRAINER_PARTY_HELD_ITEM:
            {
                const struct TrainerMonItemDefaultMoves *partyData = gTrainers[trainerNum].party.ItemDefaultMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != 0xFF; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];
                personalityValue += nameHash << 8;
                fixedIV = partyData[i].iv * 31 / 255;
                CreateMon(&party[i], partyData[i].species, partyData[i].level, fixedIV, TRUE, personalityValue, 2, 0);

                SetMonData(&party[i], MON_DATA_HELD_ITEM, &partyData[i].heldItem);
                break;
            }
            case F_TRAINER_PARTY_CUSTOM_MOVESET | F_TRAINER_PARTY_HELD_ITEM:
            {
                const struct TrainerMonItemCustomMoves *partyData = gTrainers[trainerNum].party.ItemCustomMoves;

                for (j = 0; gSpeciesNames[partyData[i].species][j] != 0xFF; j++)
                    nameHash += gSpeciesNames[partyData[i].species][j];
                personalityValue += nameHash << 8;
                fixedIV = partyData[i].iv * 31 / 255;
                CreateMon(&party[i], partyData[i].species, partyData[i].level, fixedIV, TRUE, personalityValue, 2, 0);

                SetMonData(&party[i], MON_DATA_HELD_ITEM, &partyData[i].heldItem);
                for (j = 0; j < 4; j++)
                {
                    SetMonData(&party[i], MON_DATA_MOVE1 + j, &partyData[i].moves[j]);
                    SetMonData(&party[i], MON_DATA_PP1 + j, &gBattleMoves[partyData[i].moves[j]].pp);
                }
                break;
            }
            }
        }
        gBattleTypeFlags |= gTrainers[trainerNum].doubleBattle;
    }
    return gTrainers[trainerNum].partySize;
}

void HBlankCB_Battle(void)
{
    if (REG_VCOUNT < 0xA0 && REG_VCOUNT >= 0x6F )
        REG_BG0CNT = 0x9800;
}

void VBlankCB_Battle(void)
{
    Random();  // unused return value
    REG_BG0HOFS = gBattle_BG0_X;
    REG_BG0VOFS = gBattle_BG0_Y;
    REG_BG1HOFS = gBattle_BG1_X;
    REG_BG1VOFS = gBattle_BG1_Y;
    REG_BG2HOFS = gBattle_BG2_X;
    REG_BG2VOFS = gBattle_BG2_Y;
    REG_BG3HOFS = gBattle_BG3_X;
    REG_BG3VOFS = gBattle_BG3_Y;
    REG_WIN0H = gBattle_WIN0H;
    REG_WIN0V = gBattle_WIN0V;
    REG_WIN1H = gBattle_WIN1H;
    REG_WIN1V = gBattle_WIN1V;
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void SpriteCB_VsLetterDummy(struct Sprite *sprite)
{
}

static void SpriteCB_VsLetter(struct Sprite *sprite)
{
    if (sprite->data[0] != 0)
        sprite->x = sprite->data[1] + ((sprite->data[2] & 0xFF00) >> 8);
    else
        sprite->x = sprite->data[1] - ((sprite->data[2] & 0xFF00) >> 8);
    sprite->data[2] += 0x180;
    if (sprite->affineAnimEnded)
    {
        FreeSpriteTilesByTag(0x2710);
        FreeSpritePaletteByTag(0x2710);
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
    }
}

void SpriteCB_VsLetterInit(struct Sprite *sprite)
{
    StartSpriteAffineAnim(sprite, 1);
    sprite->callback = SpriteCB_VsLetter;
    PlaySE(SE_MUGSHOT);
}

void BufferPartyVsScreenHealth_AtEnd(u8 taskId)
{
    struct Pokemon *party1 = NULL;
    struct Pokemon *party2 = NULL;
    u8 multiplayerId = gBattleStruct->multiplayerId;
    u32 flags;
    s32 i;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        switch (gLinkPlayers[multiplayerId].id)
        {
        case 0:
        case 2:
            party1 = gPlayerParty;
            party2 = gEnemyParty;
            break;
        case 1:
        case 3:
            party1 = gEnemyParty;
            party2 = gPlayerParty;
            break;
        }
    }
    else
    {
        party1 = gPlayerParty;
        party2 = gEnemyParty;
    }

    flags = 0;
    for (i = 0; i < 6; i++)
    {
        u16 species = GetMonData(&party1[i], MON_DATA_SPECIES_OR_EGG);
        u16 hp = GetMonData(&party1[i], MON_DATA_HP);
        u32 status = GetMonData(&party1[i], MON_DATA_STATUS);

        if (species == 0)
            continue;
        if (species != SPECIES_EGG && hp != 0 && status == 0)
            flags |= 1 << i * 2;

        if (species == 0)
            continue;
        if (hp != 0 && (species == SPECIES_EGG || status != 0))
            flags |= 2 << i * 2;

        if (species == 0)
            continue;
        if (species != SPECIES_EGG && hp == 0)
            flags |= 3 << i * 2;
    }
    gTasks[taskId].data[3] = flags;

    flags = 0;
    for (i = 0; i < 6; i++)
    {
        u16 species = GetMonData(&party2[i], MON_DATA_SPECIES_OR_EGG);
        u16 hp = GetMonData(&party2[i], MON_DATA_HP);
        u32 status = GetMonData(&party2[i], MON_DATA_STATUS);

        if (species == 0)
            continue;
        if (species != SPECIES_EGG && hp != 0 && status == 0)
            flags |= 1 << i * 2;

        if (species == 0)
            continue;
        if (hp != 0 && (species == SPECIES_EGG || status != 0))
            flags |= 2 << i * 2;

        if (species == 0)
            continue;
        if (species != SPECIES_EGG && hp == 0)
            flags |= 3 << i * 2;
    }
    gTasks[taskId].data[4] = flags;
}

void CB2_InitEndLinkBattle(void)
{
    s32 i;
    u8 taskId;

    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);
    CpuFill32(0, (void *)VRAM, VRAM_SIZE);
    REG_MOSAIC = 0;
    REG_WIN0H = 0xF0;
    REG_WIN0V = 0x5051;
    REG_WININ = 0;
    REG_WINOUT = 0;
    gBattle_WIN0H = 0xF0;
    gBattle_WIN0V = 0x5051;
    ScanlineEffect_Clear();

    for (i = 0; i < 80; i++)
    {
        gScanlineEffectRegBuffers[0][i] = 0xF0;
        gScanlineEffectRegBuffers[1][i] = 0xF0;
    }
    for (; i < 160; i++)
    {
        gScanlineEffectRegBuffers[0][i] = 0xFF10;
        gScanlineEffectRegBuffers[1][i] = 0xFF10;
    }
    Text_LoadWindowTemplate(&gWindowTemplate_81E6C58);
    ResetPaletteFade();
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;

    Text_InitWindowWithTemplate(&gWindowTemplate_Contest_MoveDescription, &gWindowTemplate_81E6C58);
    Text_InitWindowWithTemplate(&gUnknown_030041D0, &gWindowTemplate_81E71D0);
    Text_InitWindowWithTemplate(&gUnknown_03004250, &gWindowTemplate_81E71EC);
    sub_800D6D4();
    LoadCompressedPalette(gBattleTextboxPalette, 0, 64);
    ApplyPlayerChosenFrameToBattleMenu();
    ResetSpriteData();
    ResetTasks();
    DrawBattleEntryBackground();
    REG_WINOUT = 0x37;
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 4;
    SetVBlankCallback(VBlankCB_Battle);
    taskId = CreateTask(InitLinkBattleVsScreen, 0);
    gTasks[taskId].data[1] = 0x10E;
    gTasks[taskId].data[2] = 0x5A;
    gTasks[taskId].data[5] = 1;
    BufferPartyVsScreenHealth_AtEnd(taskId);
    SetMainCallback2(sub_80101B8);
    gBattleCommunication[0] = 0;
}

void sub_80101B8(void)
{
    c2_081284E0();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    RunTasks();
}

void c2_081284E0(void)
{
    switch (gBattleCommunication[0])
    {
    case 0:
        gBattleCommunication[1] = 0xFF;
        gBattleCommunication[0]++;
        break;
    case 1:
        gBattleCommunication[1]--;
        if (gBattleCommunication[1] == 0)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gBattleCommunication[0]++;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
            SetMainCallback2(gMain.savedCallback);
        break;
    }
}

#if DEBUG

extern u8 gUnknown_Debug_2023B62[];
extern const u8 Str_821F7BD[];
extern const u8 Str_821F7DA[];

void debug_sub_8010818(void);
void debug_sub_80108B8(void);
void debug_sub_8010CAC(void);
void debug_sub_8011498(void);
void debug_sub_801174C(void);
void debug_sub_8011D40(void);
void debug_sub_8011E5C(void);
void debug_sub_8011E74(void);
void debug_sub_8011EA0(u8);
void debug_sub_8012294(void);
void debug_sub_80123D8(u8);
void debug_sub_8012540(void);
void debug_ShowCurrentAnimAudioOptions(void);
void debug_sub_80125A0(void);
void debug_sub_80125E4(void);
void debug_sub_8012628(void);
void debug_sub_8012658(void);
void debug_sub_8012688(void);
void debug_sub_8012878(void);
void debug_sub_8012D10(u8);
u32 debug_sub_8013294(u8, void *, u32);
void debug_sub_80132C8(u8, void *, u32);

extern s16 gUnknown_Debug_2023A76[][35];
extern s16 gUnknown_Debug_2023B02[][6][4];
u8 gUnknown_Debug_03004360;
struct Window gUnknown_Debug_03004370;
u8 gUnknown_Debug_030043A0;
u8 gUnknown_Debug_030043A4;
u8 gUnknown_Debug_030043A8;
extern u8 gBattleBuffersTransferData[];

extern const u16 gUnknown_Debug_821F424[][5];
extern const u16 gUnknown_Debug_821F56C[][5];
extern const u32 gUnknown_Debug_821F798[][4];

extern const u8 gDebugBattleCharmap_Gfx_lz[];
extern const u8 gDebugBattleCharmap_Tilemap_lz[];
extern const u8 gDebugBattleCharmap_Pal_lz[];

void debug_sub_8010800(void)
{
    debug_sub_8010818();
    debug_sub_80108B8();
    *(u32 *)(gBattleBuffersTransferData + 0x100) = 0;
}

void debug_sub_8010818(void)
{
	s32 i;

	gUnknown_Debug_2023A76[0][0] = 0x115;
	gUnknown_Debug_2023A76[1][0] = 0x115;
	for (i = 1; i < 31; i++)
	{
		gUnknown_Debug_2023A76[0][i] = gUnknown_Debug_821F424[i][4];
		gUnknown_Debug_2023A76[1][i] = gUnknown_Debug_821F424[i][4];
	}

	for (i = 0; i < 6; i++)
	{
		for (gUnknown_Debug_030043A8 = 0; gUnknown_Debug_030043A8 < 4; gUnknown_Debug_030043A8++)
		{
			gUnknown_Debug_2023B02[0][i][gUnknown_Debug_030043A8] = gUnknown_Debug_821F56C[gUnknown_Debug_030043A8][0];
			gUnknown_Debug_2023B02[1][i][gUnknown_Debug_030043A8] = gUnknown_Debug_821F56C[gUnknown_Debug_030043A8][0];
		}
	}
}

void debug_sub_80108B8(void)
{
	s32 i;

	m4aSoundVSyncOff();
	SetHBlankCallback(NULL);
	SetVBlankCallback(NULL);
	DmaFill32(3, 0, (void *)VRAM, VRAM_SIZE);
	REG_IE = 1;
	REG_DISPCNT = 0x1340;
	gBattle_BG0_X = 0;
	gBattle_BG0_Y = 0;
	gBattle_BG1_X = 0;
	gBattle_BG1_Y = 0;
	gBattle_BG2_X = 0;
	gBattle_BG2_Y = 0;
	gBattle_BG3_X = 0;
	gBattle_BG3_Y = 0;
	REG_BG0CNT = 0x1F09;
	REG_BG1CNT = 0x4801;
	REG_BLDCNT = 0;
	REG_BLDY = 0;
	LZDecompressVram(gDebugBattleCharmap_Gfx_lz, (void *)VRAM);
	LZDecompressWram(gDebugBattleCharmap_Tilemap_lz, gSharedMem);
	LZDecompressVram(gDebugBattleCharmap_Pal_lz, (void *)PLTT);
	LZDecompressVram(gDebugBattleCharmap_Pal_lz, (void *)(PLTT + 0x1E0));
	m4aSoundVSyncOn();
	SetVBlankCallback(debug_sub_8011D40);
	SetMainCallback2(debug_sub_8010CAC);
	ResetTasks();
	ResetSpriteData();
	ScanlineEffect_Stop();
	Text_LoadWindowTemplate(&gWindowTemplate_81E6C3C);
	Text_InitWindowWithTemplate(&gUnknown_Debug_03004370, &gWindowTemplate_81E6C3C);
	gUnknown_Debug_03004360 = 0;
	gUnknown_Debug_030043A0 = 0;
	gUnknown_Debug_030043A4 = 0;
	for (i = 0; i < 31; i++)
		debug_sub_8011EA0(i);
	for (gUnknown_Debug_030043A8 = 0; gUnknown_Debug_030043A8 < 4; gUnknown_Debug_030043A8++)
		debug_sub_8012294();
	debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
	debug_sub_8012540();
	debug_ShowCurrentAnimAudioOptions();
	gUnknown_Debug_030043A8 = 0;
	debug_sub_80125A0();
	if (gUnknown_Debug_2023A76[0][0x22] == 8)
	{
		debug_sub_801174C();
	}
	else
	{
		for (i = 0; i < 8; i++)
			gSharedMem[0x160B4 + i] = 0;
	}
}

void debug_sub_8010A7C(u8 a, u8 b)
{
    s32 i;

    for (i = 0; i < b; i++)
        gBattleTextBuff1[i] = a;
    gBattleTextBuff1[i] = B_BUFF_EOS;
}

void debug_sub_8010AAC(u8 a)
{
	switch (gSpeciesInfo[gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5]].genderRatio)
	{
	case 0:
		gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + 4] = 2;
		break;
	case 0xFE:
		gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + 4] = 3;
		break;
	case 0xFF:
		gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + 4] = 4;
		break;
	default:
		gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + 4] &= 1;
		if (a != 0)
			gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + 4] ^= 1;
		else
			gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + 4] = 0;
		break;
	}
}

// gUnknown_Debug_2023A76 2D array
void debug_sub_8010B80(u8 a)
{
	s8 r12 = 0;
	s8 r7 = gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5];

	while (r7 >= 10)
	{
		r7 -= 10;
		r12++;
	}

	if (a & 2)
	{
		if (a & 1)
			r12++;
		else
			r12--;
		if (r12 < 0)
			r12 = 9;
		if (r12 > 9)
			r12 = 0;
	}
	else
	{
		if (a & 1)
			r7++;
		else
			r7--;
		if (r7 < 1)
			r7 = 9;
		if (r7 > 9)
			r7 = 1;
	}
	gUnknown_Debug_2023A76[gUnknown_Debug_03004360 ^ 1][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5]
	= gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5]
	= r12 * 10 + r7;
}

void debug_sub_8010CAC(void)
{
    s32 r5;

    if (gMain.heldKeysRaw == (L_BUTTON | SELECT_BUTTON))
        DoSoftReset();
    if (gMain.newKeysRaw == SELECT_BUTTON)
    {
        if (gUnknown_Debug_030043A4 < 6)
        {
            gUnknown_Debug_030043A8 = 0;
            debug_sub_8012628();
            SetMainCallback2(debug_sub_8011498);
        }
        if (gUnknown_Debug_030043A0 == 0 && gUnknown_Debug_030043A4 == 6)
        {
            gMain.savedCallback = debug_sub_80108B8;
            CreateMon(
              &gPlayerParty[0],
              gUnknown_Debug_2023A76[0][0 * 5 + 0],
              gUnknown_Debug_2023A76[0][0 * 5 + 1],
              32,
              0, 0, 0, 0);
            for (r5 = 0; r5 < 4; r5++)
            {
                SetMonData(&gPlayerParty[0], MON_DATA_MOVE1 + r5, &gUnknown_Debug_2023B02[0][0][r5]);
                SetMonData(&gPlayerParty[0], MON_DATA_PP1 + r5, &gBattleMoves[gUnknown_Debug_2023B02[0][0][r5]].pp);
            }
            switch (gUnknown_Debug_2023A76[0][6 * 5 + 0])
            {
            case 1:
                gCB2_AfterEvolution = debug_sub_80108B8;
                EvolutionScene(&gPlayerParty[0], gUnknown_Debug_2023A76[0][1 * 5 + 0], 1, 0);
                break;
            case 2:
                debug_sub_8012688();
                break;
            }
        }
        if (gUnknown_Debug_030043A0 == 1 && gUnknown_Debug_030043A4 == 6)
        {
            // This is really weird
            r5 = (gSaveBlock2.optionsBattleSceneOff | (gSaveBlock2.optionsSound << 1));
            r5++;
            if (r5 == 4)
                r5 = 0;
            gSaveBlock2.optionsBattleSceneOff = (r5 & 1);
            gSaveBlock2.optionsSound = (r5 & 2) >> 1;
            SetPokemonCryStereo(gSaveBlock2.optionsSound);
            debug_ShowCurrentAnimAudioOptions();
        }
    }
    if (gMain.newKeysRaw == START_BUTTON)
        debug_sub_801174C();
    if (gMain.newKeysRaw == DPAD_UP)
    {
        debug_sub_80125E4();
        if (gUnknown_Debug_030043A4 != 0)
            gUnknown_Debug_030043A4--;
        else
            gUnknown_Debug_030043A4 = 6;
        debug_sub_8011E74();
        debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
        debug_sub_80125A0();
    }
    if (gMain.newKeysRaw == DPAD_DOWN)
    {
        debug_sub_80125E4();
        if (gUnknown_Debug_030043A4 == 6)
            gUnknown_Debug_030043A4 = 0;
        else
            gUnknown_Debug_030043A4++;
        debug_sub_8011E74();
        debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
        debug_sub_80125A0();
    }
    if (gMain.newKeysRaw == DPAD_LEFT)
    {
        debug_sub_80125E4();
        if (gUnknown_Debug_030043A0 != 0)
        {
            gUnknown_Debug_030043A0--;
        }
        else
        {
            if (gUnknown_Debug_03004360 != 0)
            {
                gUnknown_Debug_03004360 = 0;
                gUnknown_Debug_030043A0 = 4;
                gBattle_BG1_X = 0;
                debug_sub_8011E5C();
                debug_sub_8011E74();
                debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            }
        }
        debug_sub_80125A0();
    }
    if (gMain.newKeysRaw == DPAD_RIGHT)
    {
        debug_sub_80125E4();
        if (gUnknown_Debug_030043A0 != 4)
        {
            gUnknown_Debug_030043A0++;
        }
        else
        {
            if (gUnknown_Debug_03004360 == 0)
            {
                gUnknown_Debug_03004360 = 1;
                gUnknown_Debug_030043A0 = 0;
                gBattle_BG1_X = 0x100;
                debug_sub_8011E5C();
                debug_sub_8011E74();
                debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            }
        }
        debug_sub_80125A0();
    }
    if (JOY_REPEAT(B_BUTTON))
    {
        switch (gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5)
        {
        case 31:
            debug_sub_8010818();
            debug_sub_8011E5C();
            debug_sub_8011E74();
            debug_sub_8012540();
            debug_ShowCurrentAnimAudioOptions();
            debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            break;
        case 32:
            debug_sub_80132C8(31, gUnknown_Debug_2023A76, 0xEC);
            debug_sub_8011E5C();
            debug_sub_8011E74();
            debug_sub_8012540();
            debug_ShowCurrentAnimAudioOptions();
            debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            break;
        case 33:
            debug_sub_8013294(31, gUnknown_Debug_2023A76, 0xEC);
            break;
        case 34:
            if (gUnknown_Debug_2023A76[0][6 * 5 + 4] != 0)
            {
                gUnknown_Debug_2023A76[0][6 * 5 + 4]--;
                gUnknown_Debug_2023A76[1][6 * 5 + 4]--;
            }
            else
            {
                gUnknown_Debug_2023A76[0][6 * 5 + 4] = 8;
                gUnknown_Debug_2023A76[1][6 * 5 + 4] = 8;
            }
            debug_sub_8012540();
            break;
        case 30:
            debug_sub_8010B80(0);
            debug_sub_8011EA0(gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5);
            break;
        default:
            if (gUnknown_Debug_030043A0 == 4 && gUnknown_Debug_030043A4 < 6)
            {
                debug_sub_8010AAC(1);
            }
            else
            {
                gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5]--;
                if (gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5] < gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][4])
                    gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5] = gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][3];
            }
            if (gUnknown_Debug_030043A0 == 0)
            {
                debug_sub_8010AAC(0);
                debug_sub_8011EA0(gUnknown_Debug_030043A4 * 5 + 4);
            }
            debug_sub_8011EA0(gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0);
            debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            break;
        }
    }
    if (JOY_REPEAT(A_BUTTON))
    {
        switch (gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5)
        {
        case 31:
            debug_sub_8010818();
            debug_sub_8011E5C();
            debug_sub_8011E74();
            debug_sub_8012540();
            debug_ShowCurrentAnimAudioOptions();
            debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            break;
        case 32:
            debug_sub_80132C8(31, gUnknown_Debug_2023A76, 0xEC);
            debug_sub_8011E5C();
            debug_sub_8011E74();
            debug_sub_8012540();
            debug_ShowCurrentAnimAudioOptions();
            debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            break;
        case 33:
            debug_sub_8013294(31, gUnknown_Debug_2023A76, 0xEC);
            break;
        case 34:
            if (gUnknown_Debug_2023A76[0][6 * 5 + 4] < 8)
            {
                gUnknown_Debug_2023A76[0][6 * 5 + 4]++;
                gUnknown_Debug_2023A76[1][6 * 5 + 4]++;
            }
            else
            {
                gUnknown_Debug_2023A76[0][6 * 5 + 4] = 0;
                gUnknown_Debug_2023A76[1][6 * 5 + 4] = 0;
            }
            debug_sub_8012540();
            break;
        case 30:
            debug_sub_8010B80(1);
            debug_sub_8011EA0(gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5);
            break;
        default:
            if (gUnknown_Debug_030043A0 == 4 && gUnknown_Debug_030043A4 < 6)
            {
                debug_sub_8010AAC(1);
            }
            else
            {
                gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5]++;
                if (gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] > gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][3])
                    gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] = gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][4];
            }
            if (gUnknown_Debug_030043A0 == 0)
            {
                debug_sub_8010AAC(0);
                debug_sub_8011EA0(gUnknown_Debug_030043A4 * 5 + 4);
            }
            debug_sub_8011EA0(gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5);
            debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
            break;
        }
    }
    if (JOY_REPEAT(L_BUTTON))
    {
        if (gUnknown_Debug_030043A0 == 4 && gUnknown_Debug_030043A4 < 6)
        {
            debug_sub_8010AAC(1);
        }
        else
        {
            if (gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0 == 30)
            {
                debug_sub_8010B80(2);
            }
            else
            {
                gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] -= 10;
                while (gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] < gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][4])
                    gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] += gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][3];
            }
        }
        if (gUnknown_Debug_030043A0 == 0)
        {
            debug_sub_8010AAC(0);
            debug_sub_8011EA0(gUnknown_Debug_030043A4 * 5 + 4);
        }
        debug_sub_8011EA0(gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0);
        debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
    }
    if (JOY_REPEAT(R_BUTTON))
    {
        if (gUnknown_Debug_030043A0 == 4 && gUnknown_Debug_030043A4 < 6)
        {
            debug_sub_8010AAC(1);
        }
        else
        {
            if (gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0 == 30)
            {
                debug_sub_8010B80(3);
            }
            else
            {
                gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] += 10;
                while (gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] > gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][3])
                    gUnknown_Debug_2023A76[gUnknown_Debug_03004360][gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0] -= gUnknown_Debug_821F424[gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0][3];
            }
        }
        if (gUnknown_Debug_030043A0 == 0)
        {
            debug_sub_8010AAC(0);
            debug_sub_8011EA0(gUnknown_Debug_030043A4 * 5 + 4);
        }
        debug_sub_8011EA0(gUnknown_Debug_030043A4 * 5 + gUnknown_Debug_030043A0);
        debug_sub_80123D8(gUnknown_Debug_030043A4 * 5);
    }
    AnimateSprites();
    BuildOamBuffer();
}

extern u16 gUnknown_Debug_821F564[][5];

void debug_sub_8011498(void)
{
	u8 r9 = gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5;

	if (gMain.heldKeysRaw == (L_BUTTON | SELECT_BUTTON))
		DoSoftReset();
	if (gMain.newKeysRaw == SELECT_BUTTON)
	{
		debug_sub_8012658();
		SetMainCallback2(debug_sub_8010CAC);
	}
	if (gMain.newKeysRaw == START_BUTTON)
		debug_sub_801174C();
	if (gMain.newKeysRaw == DPAD_UP || gMain.newKeysRaw == DPAD_DOWN)
	{
		debug_sub_8012658();
		gUnknown_Debug_030043A8 ^= 2;
		debug_sub_8012628();
	}
	if (gMain.newKeysRaw == DPAD_LEFT || gMain.newKeysRaw == DPAD_RIGHT)
	{
		debug_sub_8012658();
		gUnknown_Debug_030043A8 ^= 1;
		debug_sub_8012628();
	}
	if (JOY_REPEAT(B_BUTTON))
	{
		gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8]--;
		if (gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] < gUnknown_Debug_821F564[gUnknown_Debug_030043A8][4])
			gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] = gUnknown_Debug_821F564[gUnknown_Debug_030043A8][3];
		debug_sub_8012294();
	}
	if (JOY_REPEAT(A_BUTTON))
	{
		gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8]++;
		if (gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] > gUnknown_Debug_821F564[gUnknown_Debug_030043A8][3])
			gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] = gUnknown_Debug_821F564[gUnknown_Debug_030043A8][4];
		debug_sub_8012294();
	}
	if (JOY_REPEAT(L_BUTTON))
	{
		gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] -= 10;
		while (gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] < gUnknown_Debug_821F564[gUnknown_Debug_030043A8][4])
			gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] += gUnknown_Debug_821F564[gUnknown_Debug_030043A8][3];
		debug_sub_8012294();
	}
	if (JOY_REPEAT(R_BUTTON))
	{
		gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] += 10;
		while (gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] > gUnknown_Debug_821F564[gUnknown_Debug_030043A8][3])
			gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r9 / 5][gUnknown_Debug_030043A8] -= gUnknown_Debug_821F564[gUnknown_Debug_030043A8][3];
		debug_sub_8012294();
	}
	AnimateSprites();
	BuildOamBuffer();
}

extern const u16 gUnknown_Debug_821F598[];
extern const u8 str_821F631[][6];
extern const u8 Str_821F649[];
extern const struct Pokeblock gUnknown_Debug_821F5AC[];

extern u8 gUnknown_020297ED;

extern void UnusedBattleInit(void);

void debug_sub_801174C(void)
{
	u8 r9 = 0;
	u8 r6;
	s32 i;
	s32 spC;
	u16 sp10;

	gUnknown_020297ED = 1;
	r6 = Random() % 4;
	StringCopy(gSaveBlock2.playerName, str_821F631[r6]);
	gSaveBlock2.playerGender = r6 >> 1;
	ZeroPlayerPartyMons();
	ZeroEnemyPartyMons();
	i = gUnknown_Debug_2023A76[0][30];
	spC = 0;
	if (i >= 10)
	{
		spC = 0;
		while (i >= 10)
		{
			i -= 10;
			spC++;
		}
	}
	gBattleTypeFlags = gUnknown_Debug_821F598[i - 1];
	gUnknown_02023A14_50 = 8;
	gBattleEnvironment = spC;
	if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
		EnterSafariMode();
	if (gUnknown_Debug_2023A76[0][30] >= 2 && gUnknown_Debug_2023A76[0][30] <= 4)
		gTrainerBattleOpponent_A = (Random() & 7) + 1;

	gPlayerPartyCount = 0;
	for (i = 0; i < 30; i += 5)
	{
		if (gUnknown_Debug_2023A76[0][i] != 0)
		{
			switch (gUnknown_Debug_2023A76[0][i + 4])
			{
			case 0:
			case 2:
				r6 = 0;
				break;
			case 1:
            case 3:
				r6 = 0xFE;
				break;
			default:
				r6 = 0xFF;
				break;
			}
			if (gUnknown_Debug_2023A76[0][i] == 0xC9 && i + 5 < 30)
				r9 = gUnknown_Debug_2023A76[0][i + 7];
			else
				r9 = 0;
			CreateMonWithGenderNatureLetter(
				&gEnemyParty[i / 5],
				gUnknown_Debug_2023A76[0][i],
				gUnknown_Debug_2023A76[0][i + 1],
				0,
				r6,
				0,
				r9);
		}
		SetMonData(&gEnemyParty[i / 5], MON_DATA_HELD_ITEM, &gUnknown_Debug_2023A76[0][i + 2]);
		sp10 = gUnknown_Debug_2023A76[0][i + 2] - 1;
		if (sp10 <= 11)
			SetMonData(&gEnemyParty[i / 5], MON_DATA_POKEBALL, &gUnknown_Debug_2023A76[0][i + 2]);
		if (gUnknown_Debug_2023A76[0][i + 3] != 0 && gUnknown_Debug_2023A76[0][i + 3] != 3)
		{
			if (gUnknown_Debug_2023A76[0][i + 3] <= 2)
				spC = gUnknown_Debug_2023A76[0][i + 3] - 1;
			else
				spC = gUnknown_Debug_2023A76[0][i + 3] - 4;
			SetMonData(&gEnemyParty[i / 5], MON_DATA_ABILITY_NUM, &spC);
		}

		if (gUnknown_Debug_2023A76[1][i] != 0)
		{
			switch (gUnknown_Debug_2023A76[1][i + 4])
			{
			case 0:
			case 2:
				r6 = 0;
				break;
			case 1:
            case 3:
				r6 = 0xFE;
				break;
			default:
				r6 = 0xFF;
				break;
			}
			if (gUnknown_Debug_2023A76[1][i] == 0xC9 && i + 5 < 30)
				r9 = gUnknown_Debug_2023A76[1][i + 7];
			else
				r9 = 0;
			CreateMonWithGenderNatureLetter(
				&gPlayerParty[i / 5],
				gUnknown_Debug_2023A76[1][i],
				gUnknown_Debug_2023A76[1][i + 1],
				0,
				r6,
				0,
				r9);
			gPlayerPartyCount++;
		}
		SetMonData(&gPlayerParty[i / 5], MON_DATA_HELD_ITEM, &gUnknown_Debug_2023A76[1][i + 2]);
		sp10 = gUnknown_Debug_2023A76[0][i + 2] - 1;
		if (sp10 <= 11)
			SetMonData(&gPlayerParty[i / 5], MON_DATA_POKEBALL, &gUnknown_Debug_2023A76[1][i + 2]);
		if (gUnknown_Debug_2023A76[1][i + 3] != 0 && gUnknown_Debug_2023A76[1][i + 3] != 3)
		{
			if (gUnknown_Debug_2023A76[1][i + 3] <= 2)
				spC = gUnknown_Debug_2023A76[1][i + 3] - 1;
			else
				spC = gUnknown_Debug_2023A76[1][i + 3] - 4;
			SetMonData(&gPlayerParty[i / 5], MON_DATA_ABILITY_NUM, &spC);
		}
		if (gUnknown_Debug_2023A76[1][i + 3] > 2)
		{
			SetMonData(&gPlayerParty[i / 5], MON_DATA_OT_NAME, Str_821F649);
			gUnknown_02023A14_50 |= 0x40;
		}
	}

    for (spC = 0; spC < 6; spC++)
    {
        for (i = 0; i < 4; i++)
        {
            SetMonData(&gEnemyParty[spC], MON_DATA_MOVE1 + i, &gUnknown_Debug_2023B02[0][spC][i]);
            SetMonData(&gEnemyParty[spC], MON_DATA_PP1 + i, &gBattleMoves[gUnknown_Debug_2023B02[0][spC][i]].pp);
            SetMonData(&gPlayerParty[spC], MON_DATA_MOVE1 + i, &gUnknown_Debug_2023B02[1][spC][i]);
            SetMonData(&gPlayerParty[spC], MON_DATA_PP1 + i, &gBattleMoves[gUnknown_Debug_2023B02[1][spC][i]].pp);
        }
    }

    if (gUnknown_Debug_2023A76[0][0x22] == 8)
    {
        gUnknown_02023A14_50 |= 0x80;
        PlayBattleBGM();
    }
    else if (gUnknown_Debug_2023A76[0][0x22] == 7)
    {
        gUnknown_02023A14_50 |= 0x20;
        PlayBattleBGM();
    }
    else if (gUnknown_Debug_2023A76[0][0x22] == 6)
    {
        gUnknown_02023A14_50 |= 0x10;
        if (gUnknown_Debug_2023A76[0][2] > 5)
            gBattleStruct->unk160A3 = gUnknown_Debug_2023A76[0][2] - 2;
        else
            gBattleStruct->unk160A3 = gUnknown_Debug_2023A76[0][2];
        PlayBattleBGM();
    }
    else if (gUnknown_Debug_2023A76[0][0x22] == 5)
    {
        gUnknown_02023A14_50 |= 0x21;
        PlayBattleBGM();
    }
    else
    {
        if (!(gUnknown_Debug_2023A76[0][0x22] & 1))
            PlayBattleBGM();
        if (gUnknown_Debug_2023A76[0][0x22] & 2)
            gUnknown_02023A14_50 |= 4;
        if (gUnknown_Debug_2023A76[0][0x22] & 4)
            gUnknown_02023A14_50 |= 6;
    }

    gMain.savedCallback = debug_sub_80108B8;
    SetMainCallback2(UnusedBattleInit);

    ClearBag();

    AddBagItem(ITEM_MASTER_BALL, 10);
    AddBagItem(ITEM_ULTRA_BALL, 10);
    AddBagItem(ITEM_GREAT_BALL, 10);
    AddBagItem(ITEM_POKE_BALL, 10);
    AddBagItem(ITEM_SAFARI_BALL, 10);
    AddBagItem(ITEM_NET_BALL, 10);
    AddBagItem(ITEM_DIVE_BALL, 10);
    AddBagItem(ITEM_NEST_BALL, 10);
    AddBagItem(ITEM_REPEAT_BALL, 10);
    AddBagItem(ITEM_TIMER_BALL, 10);
    AddBagItem(ITEM_LUXURY_BALL, 10);
    AddBagItem(ITEM_PREMIER_BALL, 10);

    AddBagItem(ITEM_FULL_RESTORE, 99);
    AddBagItem(ITEM_MAX_POTION, 99);
    AddBagItem(ITEM_MAX_REVIVE, 99);
    AddBagItem(ITEM_ETHER, 99);
    AddBagItem(ITEM_MAX_ETHER, 99);
    AddBagItem(ITEM_MAX_ELIXIR, 99);

    AddBagItem(ITEM_GUARD_SPEC, 99);
    AddBagItem(ITEM_DIRE_HIT, 99);
    AddBagItem(ITEM_X_ATTACK, 99);
    AddBagItem(ITEM_X_DEFEND, 99);
    AddBagItem(ITEM_X_SPEED, 99);
    AddBagItem(ITEM_X_ACCURACY, 99);
    // hmm... no X Special? Why do we need Poke Doll?
    AddBagItem(ITEM_POKE_DOLL, 99);

    for (i = 0; i < 15; i++)
        GivePokeblock(&gUnknown_Debug_821F5AC[i]);
}

void debug_sub_8011D40(void)
{
    DmaCopy16(3, gSharedMem, (void *)(VRAM + 0x4000), 0x1000);
    REG_BG0HOFS = gBattle_BG0_X;
    REG_BG0VOFS = gBattle_BG0_Y;
    REG_BG1HOFS = gBattle_BG1_X;
    REG_BG1VOFS = gBattle_BG1_Y;
    REG_BG2HOFS = gBattle_BG2_X;
    REG_BG2VOFS = gBattle_BG2_Y;
    REG_BG3HOFS = gBattle_BG3_X;
    REG_BG3VOFS = gBattle_BG3_Y;
    LoadOam();
    ProcessSpriteCopyRequests();
}

void debug_nullsub_45()
{
}

void debug_sub_8011DD4(void)
{
    REG_BG0CNT = 0x9803;

    REG_BG0HOFS = gBattle_BG0_X;
    REG_BG0VOFS = gBattle_BG0_Y;

    REG_BG1HOFS = gBattle_BG1_X;
    REG_BG1VOFS = gBattle_BG1_Y;

    REG_BG2HOFS = gBattle_BG2_X;
    REG_BG2VOFS = gBattle_BG2_Y;

    REG_BG3HOFS = gBattle_BG3_X;
    REG_BG3VOFS = gBattle_BG3_Y;

    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void debug_sub_8011E5C(void)
{
    s32 i;

    for (i = 0; i < 31; i++)
        debug_sub_8011EA0(i);
}

extern u8 gUnknown_Debug_030043A8;

void debug_sub_8011E74(void)
{
    u8 r5 = gUnknown_Debug_030043A8;

    for (gUnknown_Debug_030043A8 = 0; gUnknown_Debug_030043A8 < 4; gUnknown_Debug_030043A8++)
        debug_sub_8012294();

    gUnknown_Debug_030043A8 = r5;
}

extern const u8 Str_821F624[];

void debug_sub_8011EA0(u8 a)
{
    u32 length;

    switch (a)
    {
    case 0:
    case 5:
    case 10:
    case 15:
    case 20:
    case 25:
// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
        debug_sub_8010A7C(0, 8);
#else
        debug_sub_8010A7C(0, 20);
#endif
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        ConvertIntToDecimalStringN(gBattleTextBuff1, gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a], 2, 3);
// TODO: check other revisions
#if !(ENGLISH && REVISION == 0)
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            422,
            gUnknown_Debug_03004360 * 32 + 25,
            0);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        gBattleTextBuff1[0] = B_BUFF_EOS;
#endif
        StringAppend(gBattleTextBuff1, gSpeciesNames[gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a]]);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        break;
    case 1:
    case 6:
    case 11:
    case 16:
    case 21:
    case 26:
    case 30:
        ConvertIntToDecimalStringN(gBattleTextBuff1, gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a], 2, 3);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        break;
    case 2:
    case 7:
    case 12:
    case 17:
    case 22:
    case 27:
// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
        debug_sub_8010A7C(0, 11);
#else
        debug_sub_8010A7C(0, 24);
#endif
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        ConvertIntToDecimalStringN(gBattleTextBuff1, gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a], 2, 3);
// TODO: check other revisions
#if !(ENGLISH && REVISION == 0)
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            422,
            gUnknown_Debug_03004360 * 32 + 25,
            0);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        gBattleTextBuff1[0] = B_BUFF_EOS;
#endif
        if (gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a] != 0)
            StringAppend(gBattleTextBuff1, GetItemName(gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a]));
        else
            StringAppend(gBattleTextBuff1, Str_821F624);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        break;
    case 4:
    case 9:
    case 14:
    case 19:
    case 24:
    case 29:
// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
        debug_sub_8010A7C(0, 2);
#else
        debug_sub_8010A7C(0, 4);
#endif
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        length = 0;
        switch (gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a])
        {
        case 0:
            gBattleTextBuff1[0] = CHAR_MALE;
            length = 1;
            break;
        case 1:
            gBattleTextBuff1[0] = CHAR_FEMALE;
            length = 1;
            break;
        case 2:
            gBattleTextBuff1[0] = CHAR_MALE;
            gBattleTextBuff1[1] = CHAR_MALE;
            length = 2;
            break;
        case 3:
            gBattleTextBuff1[0] = CHAR_FEMALE;
            gBattleTextBuff1[1] = CHAR_FEMALE;
            length = 2;
            break;
        default:
            gBattleTextBuff1[length] = CHAR_QUESTION_MARK;
            length++;
            break;
        }
        gBattleTextBuff1[length] = B_BUFF_EOS;
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        break;
    case 3:
    case 8:
    case 13:
    case 18:
    case 23:
    case 28:
    default:
        ConvertIntToDecimalStringN(gBattleTextBuff1, gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a], 2, 1);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F424[a][0],
            gUnknown_Debug_821F424[a][1],
            gUnknown_Debug_821F424[a][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        break;
    case 31:
    case 32:
    case 33:
    case 34:
        break;
    }
}

void debug_sub_8012294(void)
{
    u8 r5 = gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5;
    
    if (r5 < 30)
    {
// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
        debug_sub_8010A7C(0, 10);
#else
        debug_sub_8010A7C(0, 24);
#endif
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F564[gUnknown_Debug_030043A8][0],
            gUnknown_Debug_821F564[gUnknown_Debug_030043A8][1],
            gUnknown_Debug_821F564[gUnknown_Debug_030043A8][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        ConvertIntToDecimalStringN(gBattleTextBuff1, gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r5 / 5][gUnknown_Debug_030043A8], 2, 3);
// TODO: check other revisions
#if !(ENGLISH && REVISION == 0)
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            422,
            gUnknown_Debug_03004360 * 32 + 25,
            0);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        gBattleTextBuff1[0] = B_BUFF_EOS;
#endif
        StringAppend(gBattleTextBuff1, gMoveNames[gUnknown_Debug_2023B02[gUnknown_Debug_03004360][r5 / 5][gUnknown_Debug_030043A8]]);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F564[gUnknown_Debug_030043A8][0],
            gUnknown_Debug_821F564[gUnknown_Debug_030043A8][1],
            gUnknown_Debug_821F564[gUnknown_Debug_030043A8][2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
    }
}

extern const u16 gUnknown_Debug_821F58C[];

void debug_sub_80123D8(u8 a)
{
    if (a < 30)
    {
// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
        debug_sub_8010A7C(0, 7);
#else
        debug_sub_8010A7C(0, 18);
#endif
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F58C[0],
            gUnknown_Debug_821F58C[1],
            gUnknown_Debug_821F58C[2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        StringCopy(gBattleTextBuff1, gAbilityNames[gSpeciesInfo[gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a]].ability1]);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F58C[0],
            gUnknown_Debug_821F58C[1],
            gUnknown_Debug_821F58C[2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
        debug_sub_8010A7C(0, 7);
#else
        debug_sub_8010A7C(0, 18);
#endif
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F58C[3],
            gUnknown_Debug_821F58C[4],
            gUnknown_Debug_821F58C[5]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        StringCopy(gBattleTextBuff1, gAbilityNames[gSpeciesInfo[gUnknown_Debug_2023A76[gUnknown_Debug_03004360][a]].ability2]);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F58C[3],
            gUnknown_Debug_821F58C[4],
            gUnknown_Debug_821F58C[5]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
    }
    else
    {
        StringCopy(gBattleTextBuff1, gAbilityNames[0]);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F58C[0],
            gUnknown_Debug_821F58C[1],
            gUnknown_Debug_821F58C[2]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            gBattleTextBuff1,
            gUnknown_Debug_821F58C[3],
            gUnknown_Debug_821F58C[4],
            gUnknown_Debug_821F58C[5]);
        Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
    }
}

void debug_sub_8012540(void)
{
    ConvertIntToDecimalStringN(gBattleTextBuff1, gUnknown_Debug_2023A76[0][0x22], 0, 1);
    Text_InitWindow(
        &gUnknown_Debug_03004370,
        gBattleTextBuff1,
        gUnknown_Debug_821F424[31][0],
        gUnknown_Debug_821F424[31][1],
        gUnknown_Debug_821F424[31][2]);
    Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
}

// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
extern const u8 Str_821F64D[];
extern const u8 Str_821F652[];
extern const u8 Str_821F655[];
extern const u8 Str_821F65A[];
#endif

void debug_ShowCurrentAnimAudioOptions(void)
{
// TODO: check other revisions
#if (ENGLISH && REVISION == 0)
    if (gSaveBlock2.optionsBattleSceneOff)
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            Str_821F652,
            418,
            gUnknown_Debug_03004360 * 32 + 21,
            0);
    else
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            Str_821F64D,
            418,
            gUnknown_Debug_03004360 * 32 + 21,
            0);
    Text_PrintWindow8002F44(&gUnknown_Debug_03004370);

    if (gSaveBlock2.optionsSound)
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            Str_821F65A,
            422,
            gUnknown_Debug_03004360 * 32 + 24,
            0);
    else
        Text_InitWindow(
            &gUnknown_Debug_03004370,
            Str_821F655,
            422,
            gUnknown_Debug_03004360 * 32 + 24,
            0);
    Text_PrintWindow8002F44(&gUnknown_Debug_03004370);
#endif
}

extern const u32 gUnknown_Debug_821F680[][0x23];

// Shows a cursor
void debug_sub_80125A0(void)
{
    gSharedMem[gUnknown_Debug_821F680[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5]] = 0x6D;
}

// Hides a cursor
void debug_sub_80125E4(void)
{
    gSharedMem[gUnknown_Debug_821F680[gUnknown_Debug_03004360][gUnknown_Debug_030043A0 + gUnknown_Debug_030043A4 * 5]] = 0x81;
}

void debug_sub_8012628(void)
{
	gSharedMem[gUnknown_Debug_821F798[gUnknown_Debug_03004360][gUnknown_Debug_030043A8]] = 0x6D;
}

void debug_sub_8012658(void)
{
	gSharedMem[gUnknown_Debug_821F798[gUnknown_Debug_03004360][gUnknown_Debug_030043A8]] = 0x81;
}

void debug_sub_8012688(void)
{
	s32 i;
	u8 spriteId;
	u8 taskId;

	for (i = 0; i < 411; i++)
		gUnknown_Debug_2023B62[i] = 0;
	SetHBlankCallback(NULL);
	SetVBlankCallback(NULL);
	DmaFill32(3, 0, (void *)VRAM, VRAM_SIZE);
	REG_MOSAIC = 0;
	REG_WIN0H = 0;
	REG_WIN0V = 0;
	REG_WIN1H = 0;
	REG_WIN1V = 0;
	REG_WININ = 0;
	REG_WINOUT = 0;
	Text_LoadWindowTemplate(&gWindowTemplate_81E6C58);
	ResetPaletteFade();
	gBattle_BG0_X = 0;
	gBattle_BG0_Y = DISPLAY_HEIGHT;
	gBattle_BG1_X = 0;
	gBattle_BG1_Y = 0;
	gBattle_BG2_X = 0;
	gBattle_BG2_Y = 0;
	gBattle_BG3_X = 0;
	gBattle_BG3_Y = 0;
	gBattleEnvironment = 9;
	sub_800D6D4();
	LoadBattleTextboxAndBackground();
	ResetSpriteData();
	ResetTasks();
	FreeAllSpritePalettes();
	gReservedSpritePaletteCount = 4;
	gCurrentMove = 1;
	Text_InitWindowWithTemplate(&gWindowTemplate_Contest_MoveDescription, &gWindowTemplate_81E6C58);
	DecompressPicFromTable_2(
	  &gMonFrontPicTable[gCurrentMove],
	  gMonFrontPicCoords[gCurrentMove].size,
	  gMonFrontPicCoords[gCurrentMove].y_offset,
	  (void *)EWRAM,
	  gMonSpriteGfx_Sprite_ptr[1],
	  gCurrentMove);
	LoadCompressedPalette(gMonPaletteTable[gCurrentMove].data, 272, 32);
	SetMultiuseSpriteTemplateToPokemon(gCurrentMove, 1);
	spriteId = CreateSprite(&gMultiuseSpriteTemplate, 176, 40 + gMonFrontPicCoords[gCurrentMove].y_offset, 40);
	gSprites[spriteId].callback = SpriteCallbackDummy_2;
	gSprites[spriteId].oam.paletteNum = 1;
	REG_DISPCNT = 0x1F40;
	SetHBlankCallback(debug_nullsub_45);
	SetVBlankCallback(debug_sub_8011DD4);
	m4aMPlayAllStop();
	taskId = CreateTask(debug_sub_8012D10, 0);
	gTasks[taskId].data[0] = 0;
	gTasks[taskId].data[1] = spriteId;
	SetMainCallback2(debug_sub_8012878);
}

void debug_sub_8012878(void)
{
	AnimateSprites();
	BuildOamBuffer();
	Text_UpdateWindowInBattle(&gWindowTemplate_Contest_MoveDescription);
	UpdatePaletteFade();
	RunTasks();
	if (gMain.heldKeys == (SELECT_BUTTON | R_BUTTON))
		SetMainCallback2(debug_sub_80108B8);
}

void debug_sub_80128B4(void)
{
    debug_sub_8010A7C(0, 9);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gBattleTextBuff1, 144, 2, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    ConvertIntToDecimalStringN(gBattleTextBuff1, gCurrentMove, 2, 3);
    gBattleTextBuff1[3] = CHAR_SPACE;
    gBattleTextBuff1[4] = B_BUFF_EOS;
    StringAppend(gBattleTextBuff1, gSpeciesNames[gCurrentMove]);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gBattleTextBuff1, 144, 2, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
}

void debug_sub_8012938(u8 taskId)
{
    debug_sub_8010A7C(0, 7);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gBattleTextBuff1, 162, 2, 37);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    StringCopy(gBattleTextBuff1, Str_821F7B8);
    ConvertIntToDecimalStringN(gBattleTextBuff1 + 4, gUnknown_Debug_2023B62[gCurrentMove - 1], 2, 3);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gBattleTextBuff1, 162, 2, 37);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    gSprites[gTasks[taskId].data[1]].y2 = -gUnknown_Debug_2023B62[gCurrentMove - 1];
}

void debug_sub_80129F8(u8 taskId)
{
    DecompressPicFromTable_2(
      &gMonFrontPicTable[gCurrentMove],
      gMonFrontPicCoords[gCurrentMove].size,
      gMonFrontPicCoords[gCurrentMove].y_offset,
      (void *)EWRAM,
      gMonSpriteGfx_Sprite_ptr[1],
      gCurrentMove);
    LoadCompressedPalette(gMonPaletteTable[gCurrentMove].data, 272, 32);
    gSprites[gTasks[taskId].data[1]].y = gMonFrontPicCoords[gCurrentMove].y_offset + 40;
    gSprites[gTasks[taskId].data[1]].y2 = -gUnknown_Debug_2023B62[gCurrentMove - 1];
    StartSpriteAnim(&gSprites[gTasks[taskId].data[1]], 0);
}

void debug_sub_8012AC0(s8 a, u8 taskId)
{
    do
    {
        gCurrentMove += a;
        if (gCurrentMove == 0)
            gCurrentMove = 411;
        if (gCurrentMove == 411)
            gCurrentMove = 1;
    } while (gSpeciesInfo[gCurrentMove].type1 != 2 && gSpeciesInfo[gCurrentMove].type2 != 2);
    debug_sub_80128B4();
    debug_sub_8012938(taskId);
    debug_sub_80129F8(taskId);
}

void debug_sub_8012B2C(u8 a)
{
    *(u16 *)(VRAM + 0xC000 + 0x772 + (a * 4 + 0) * 0x20) = 1;
    *(u16 *)(VRAM + 0xC000 + 0x772 + (a * 4 + 2) * 0x20) = 2;
}

void debug_sub_8012B4C(u8 a)
{
    *(u16 *)(VRAM + 0xC000 + 0x772 + (a * 4 + 0) * 0x20) = 0x1016;
    *(u16 *)(VRAM + 0xC000 + 0x772 + (a * 4 + 2) * 0x20) = 0x1016;
}

void debug_sub_8012B70(u8 taskId, u8 b)
{
    if (b != 0)
    {
        sub_802BBD4(24, 28, 29, 33, 1);
        debug_sub_80128B4();
        debug_sub_8012938(taskId);
        debug_sub_80129F8(taskId);
        gTasks[taskId].data[0] = 1;
    }
    else
    {
        sub_802BBD4(24, 28, 29, 33, 0);
        gTasks[taskId].data[0] = 2;
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, Str_821F7DA, 656, 26, 29);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
        gTasks[taskId].data[3] = 0;
        debug_sub_8012B2C(0);
    }
}

void debug_sub_8012C08(u8 taskId, u8 b)
{
    debug_sub_8010A7C(0, 9);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gBattleTextBuff1, 144, 2, 35);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    debug_sub_8010A7C(0, 7);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gBattleTextBuff1, 162, 2, 37);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    sub_802BBD4(24, 28, 29, 33, 0);
    if (b != 0)
    {
        gTasks[taskId].data[0] = 4;
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gUnknown_Debug_821F7F3, 144, 2, 35);
    }
    else
    {
        gTasks[taskId].data[0] = 3;
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, Str_821F7EA, 144, 2, 35);
    }
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gText_BattleYesNoChoice, 656, 26, 29);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    gTasks[taskId].data[3] = 1;
    debug_sub_8012B2C(1);
}

void debug_sub_8012D10(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        debug_sub_80128B4();
        debug_sub_8012938(taskId);
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, Str_821F7BD, 400, 19, 35);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
        gTasks[taskId].data[0]++;
        ActionSelectionCreateCursorAt(gTasks[taskId].data[2], 0);
        break;
    case 1:
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gTasks[taskId].data[2]);
            gTasks[taskId].data[2] &= ~2;
            ActionSelectionCreateCursorAt(gTasks[taskId].data[2], 0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gTasks[taskId].data[2]);
            gTasks[taskId].data[2] |= 2;
            ActionSelectionCreateCursorAt(gTasks[taskId].data[2], 0);
        }
        else if (JOY_NEW(DPAD_LEFT))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gTasks[taskId].data[2]);
            gTasks[taskId].data[2] &= ~1;
            ActionSelectionCreateCursorAt(gTasks[taskId].data[2], 0);
        }
        else if (JOY_NEW(DPAD_RIGHT))
        {
            PlaySE(SE_SELECT);
            nullsub_8(gTasks[taskId].data[2]);
            gTasks[taskId].data[2] |= 1;
            ActionSelectionCreateCursorAt(gTasks[taskId].data[2], 0);
        }
        else if (JOY_REPEAT(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            switch (gTasks[taskId].data[2])
            {
            case 0:
                if (gUnknown_Debug_2023B62[gCurrentMove - 1] < 64)
                {
                    gUnknown_Debug_2023B62[gCurrentMove - 1] += 1;
                    debug_sub_8012938(taskId);
                }
                break;
            case 1:
                debug_sub_8012AC0(1, taskId);
                break;
            case 2:
                if (gCurrentMove < 411)
                    gCurrentMove++;
                else
                    gCurrentMove = 1;
                debug_sub_80128B4();
                debug_sub_8012938(taskId);
                debug_sub_80129F8(taskId);
                break;
            case 3:
                debug_sub_8012B70(taskId, 0);
                break;
            }
        }
        else if (JOY_REPEAT(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            switch (gTasks[taskId].data[2])
            {
            case 0:
                if (gUnknown_Debug_2023B62[gCurrentMove - 1] > 0)
                {
                    gUnknown_Debug_2023B62[gCurrentMove - 1] -= 1;
                    debug_sub_8012938(taskId);
                }
                break;
            case 1:
                debug_sub_8012AC0(-1, taskId);
                break;
            case 2:
                if (gCurrentMove > 1)
                    gCurrentMove--;
                else
                    gCurrentMove = 411;
                debug_sub_80128B4();
                debug_sub_8012938(taskId);
                debug_sub_80129F8(taskId);
                break;
            case 3:
                debug_sub_8012B70(taskId, 0);
                break;
            }
        }
        else if (JOY_REPEAT(R_BUTTON))
        {
            PlaySE(SE_SELECT);
            switch (gTasks[taskId].data[2])
            {
            case 0:
                if (gUnknown_Debug_2023B62[gCurrentMove - 1] < 64)
                {
                    gUnknown_Debug_2023B62[gCurrentMove - 1] += 8;
                    if (gUnknown_Debug_2023B62[gCurrentMove - 1] > 64)
                        gUnknown_Debug_2023B62[gCurrentMove - 1] = 64;
                    debug_sub_8012938(taskId);
                }
                break;
            case 1:
                debug_sub_8012AC0(1, taskId);
                break;
            case 2:
                if (gCurrentMove + 10 < 412)
                    gCurrentMove += 10;
                else
                    gCurrentMove -= 400;
                debug_sub_80128B4();
                debug_sub_8012938(taskId);
                debug_sub_80129F8(taskId);
                break;
            case 3:
                debug_sub_8012B70(taskId, 0);
                break;
            }
        }
        else if (JOY_REPEAT(L_BUTTON))
        {
            PlaySE(SE_SELECT);
            switch (gTasks[taskId].data[2])
            {
            case 0:
                if (gUnknown_Debug_2023B62[gCurrentMove - 1] > 0)
                {
                    if (gUnknown_Debug_2023B62[gCurrentMove - 1] > 8)
                        gUnknown_Debug_2023B62[gCurrentMove - 1] -= 8;
                    else
                        gUnknown_Debug_2023B62[gCurrentMove - 1] = 0;
                    debug_sub_8012938(taskId);
                }
                break;
            case 1:
                debug_sub_8012AC0(-1, taskId);
                break;
            case 2:
                if (gCurrentMove - 10 > 1)
                    gCurrentMove -= 10;
                else
                    gCurrentMove += 400;
                debug_sub_80128B4();
                debug_sub_8012938(taskId);
                debug_sub_80129F8(taskId);
                break;
            case 3:
                debug_sub_8012B70(taskId, 0);
                break;
            }
        }
        break;
    case 2:
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B4C(gTasks[taskId].data[3]);
            gTasks[taskId].data[3] = 0;
            debug_sub_8012B2C(0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B4C(gTasks[taskId].data[3]);
            gTasks[taskId].data[3] = 1;
            debug_sub_8012B2C(1);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012C08(taskId, gTasks[taskId].data[3]);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            asm("");
            debug_sub_8012B70(taskId, 1);
        }
        return;
    case 3:
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B4C(gTasks[taskId].data[3]);
            gTasks[taskId].data[3] = 0;
            debug_sub_8012B2C(0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B4C(gTasks[taskId].data[3]);
            gTasks[taskId].data[3] = 1;
            debug_sub_8012B2C(1);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (gTasks[taskId].data[3] == 0)
                debug_sub_80132C8(31, gUnknown_Debug_2023B62, 411);
            debug_sub_8012B70(taskId, 1);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B70(taskId, 1);
        }
        break;
    case 4:
        if (JOY_NEW(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B4C(gTasks[taskId].data[3]);
            gTasks[taskId].data[3] = 0;
            debug_sub_8012B2C(0);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B4C(gTasks[taskId].data[3]);
            gTasks[taskId].data[3] = 1;
            debug_sub_8012B2C(1);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (gTasks[taskId].data[3] == 0)
                debug_sub_8013294(31, gUnknown_Debug_2023B62, 411);
            debug_sub_8012B70(taskId, 1);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            debug_sub_8012B70(taskId, 1);
        }
        break;
    }
}

u8 debug_sub_8013240(void)
{
    if (IdentifyFlash() == 0)
        return 0;
    else
        return 1;
}

u32 debug_sub_8013258(u16 sectorNum, u8 *data, u32 size)
{
    while (1)
    {
        if (ProgramFlashSectorAndVerify(sectorNum, data) != 0)
            return 0;
        if (size <= 0x1000)
            break;
        size -= 0x1000;
        data += 0x1000;
        sectorNum++;
    }
    return 1;
}

u32 debug_sub_8013294(u8 sectorNum, void *data, u32 size)
{
    u32 result;

    if (debug_sub_8013240() != 0)
        return 0;
    m4aSoundVSyncOff();
    result = debug_sub_8013258(sectorNum, data, size);
    m4aSoundVSyncOn();
    return result;
}

void debug_sub_80132C8(u8 a, void *b, u32 c)
{
    if (debug_sub_8013240() == 0)
        ReadFlash(a, 0, b, c);
}
#endif

void SpriteCB_WildMon(struct Sprite *sprite)
{
    sprite->callback = SpriteCB_MoveWildMonToRight;
    StartSpriteAnimIfDifferent(sprite, 0);
    BeginNormalPaletteFade(0x00020000, 0, 10, 10, RGB(15, 15, 15));
}

void SpriteCB_MoveWildMonToRight(struct Sprite *sprite)
{
    if ((gIntroSlideFlags & 1) == 0)
    {
        sprite->x2 += 2;
        if (sprite->x2 == 0)
        {
            sprite->callback = SpriteCB_WildMonShowHealthbox;
            PlayCry_Normal(sprite->data[2], 25);
        }
    }
}

void SpriteCB_WildMonShowHealthbox(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        StartHealthboxSlideIn(sprite->data[0]);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[sprite->data[0]]);
        sprite->callback = SpriteCallbackDummy_2;
        StartSpriteAnimIfDifferent(sprite, 0);
        BeginNormalPaletteFade(0x00020000, 0, 10, 0, RGB(15, 15, 15));
    }
}

void SpriteCallbackDummy_2(struct Sprite *sprite)
{
}

void SpriteCB_InitFlicker(struct Sprite *sprite)
{
    sprite->data[3] = 6;
    sprite->data[4] = 1;
    sprite->callback = SpriteCB_Flicker;
}

void SpriteCB_Flicker(struct Sprite *sprite)
{
    sprite->data[4]--;
    if (sprite->data[4] == 0)
    {
        sprite->data[4] = 8;
        sprite->invisible ^= 1;
        sprite->data[3]--;
        if (sprite->data[3] == 0)
        {
            sprite->invisible = FALSE;
            sprite->callback = SpriteCallbackDummy_2;
            gUnknown_03004284 = 0;
        }
    }
}

void SpriteCB_FaintOpponentMon(struct Sprite *sprite)
{
    u8 battler = sprite->data[0];
    u16 species;
    u8 yOffset;

    if (gBattleSpriteInfo[battler].transformSpecies != 0)
        species = gBattleSpriteInfo[battler].transformSpecies;
    else
        species = sprite->data[2];

    GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);  // Unused return value

    if (species == SPECIES_UNOWN)
    {
        u32 personalityValue = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY);
        u16 unownForm = ((((personalityValue & 0x3000000) >> 18) | ((personalityValue & 0x30000) >> 12) | ((personalityValue & 0x300) >> 6) | (personalityValue & 3)) % 0x1C);
        u16 unownSpecies;

        if (unownForm == 0)
            unownSpecies = SPECIES_UNOWN;  // Use the A Unown form
        else
            unownSpecies = NUM_SPECIES + unownForm;  // Use one of the other Unown letters

        yOffset = gMonFrontPicCoords[unownSpecies].y_offset;
    }
    else if (species == SPECIES_CASTFORM)
    {
        yOffset = gCastformFrontSpriteCoords[gBattleMonForms[battler]].y_offset;
    }
    else if (species > NUM_SPECIES)
    {
        yOffset = gMonFrontPicCoords[SPECIES_NONE].y_offset;
    }
    else
    {
        yOffset = gMonFrontPicCoords[species].y_offset;
    }

    sprite->data[3] = 8 - yOffset / 8;
    sprite->data[4] = 1;
    sprite->callback = SpriteCB_AnimFaintOpponent;
}

void SpriteCB_AnimFaintOpponent(struct Sprite *sprite)
{
    s32 i;
    u8 *dst;

    sprite->data[4]--;
    if (sprite->data[4] == 0)
    {
        sprite->data[4] = 2;
        sprite->y2 += 8;
        sprite->data[3]--;
        if (sprite->data[3] < 0)
        {
            FreeSpriteOamMatrix(sprite);
            DestroySprite(sprite);
        }
        else
        {
            // this should use a MEMSET_ALT, but *(dst++) wont match with it.
            dst = (u8 *)gMonSpriteGfx_Sprite_ptr[GetBattlerPosition(sprite->data[0])] + (gBattleMonForms[sprite->data[0]] << 11) + (sprite->data[3] << 8);
            for (i = 0; i < 0x100; i++)
                *(dst++) = 0;
            StartSpriteAnim(sprite, gBattleMonForms[sprite->data[0]]);
        }
    }
}

void SpriteCB_ShowAsMoveTarget(struct Sprite *sprite)
{
    sprite->data[3] = 8;
    sprite->data[4] = sprite->invisible;
    sprite->callback = SpriteCB_BlinkVisible;
}

static void SpriteCB_BlinkVisible(struct Sprite *sprite)
{
    sprite->data[3]--;
    if (sprite->data[3] == 0)
    {
        sprite->invisible ^= 1;
        sprite->data[3] = 8;
    }
}

void SpriteCB_HideAsMoveTarget(struct Sprite *sprite)
{
    sprite->invisible = sprite->data[4];
    sprite->data[4] = FALSE;
    sprite->callback = SpriteCallbackDummy_2;
}

void SpriteCB_BattleSpriteStartSlideLeft(struct Sprite *sprite)
{
    sprite->callback = SpriteCB_BattleSpriteSlideLeft;
}

static void SpriteCB_BattleSpriteSlideLeft(struct Sprite *sprite)
{
    if ((gIntroSlideFlags & 1) == 0)
    {
        sprite->x2 -= 2;
        if (sprite->x2 == 0)
        {
            sprite->callback = SpriteCB_Idle;
            sprite->data[1] = 0;
        }
    }
}

void SetIdleSpriteCallback(struct Sprite *sprite)
{
    sprite->callback = SpriteCB_Idle;
}

static void SpriteCB_Idle(struct Sprite *sprite)
{
}

void SpriteCB_FaintSlideAnim(struct Sprite *sprite)
{
    if ((gIntroSlideFlags & 1) == 0)
    {
        sprite->x2 += sprite->data[1];
        sprite->y2 += sprite->data[2];
    }
}

void DoBounceEffect(u8 battler, u8 which, s8 delta, s8 amplitude)
{
    u8 invisibleSpriteId;
    u8 bouncerSpriteId;

    if (which)
    {
        if (gBattleHealthBoxInfo[battler].healthboxIsBouncing)
            return;
    }
    else
    {
        if (gBattleHealthBoxInfo[battler].battlerIsBouncing)
            return;
    }

    invisibleSpriteId = CreateInvisibleSpriteWithCallback(SpriteCB_BounceEffect);
    if (which == TRUE)
    {
        bouncerSpriteId = gHealthboxSpriteIds[battler];
        gBattleHealthBoxInfo[battler].healthboxBounceSpriteId = invisibleSpriteId;
        gBattleHealthBoxInfo[battler].healthboxIsBouncing = 1;
        gSprites[invisibleSpriteId].data[0] = 0x80;
    }
    else
    {
        bouncerSpriteId = gBattlerSpriteIds[battler];
        gBattleHealthBoxInfo[battler].battlerBounceSpriteId = invisibleSpriteId;
        gBattleHealthBoxInfo[battler].battlerIsBouncing = 1;
        gSprites[invisibleSpriteId].data[0] = 0xC0;
    }
    gSprites[invisibleSpriteId].data[1] = delta;
    gSprites[invisibleSpriteId].data[2] = amplitude;
    gSprites[invisibleSpriteId].data[3] = bouncerSpriteId;
    gSprites[invisibleSpriteId].data[4] = which;
    gSprites[bouncerSpriteId].x2 = 0;
    gSprites[bouncerSpriteId].y2 = 0;
}

void EndBounceEffect(u8 battler, u8 which)
{
    u8 bouncerSpriteId;

    if (which == TRUE)
    {
        if (!gBattleHealthBoxInfo[battler].healthboxIsBouncing)
            return;
        bouncerSpriteId = gSprites[gBattleHealthBoxInfo[battler].healthboxBounceSpriteId].data[3];
        DestroySprite(&gSprites[gBattleHealthBoxInfo[battler].healthboxBounceSpriteId]);
        gBattleHealthBoxInfo[battler].healthboxIsBouncing = 0;
    }
    else
    {
        if (!gBattleHealthBoxInfo[battler].battlerIsBouncing)
            return;
        bouncerSpriteId = gSprites[gBattleHealthBoxInfo[battler].battlerBounceSpriteId].data[3];
        DestroySprite(&gSprites[gBattleHealthBoxInfo[battler].battlerBounceSpriteId]);
        gBattleHealthBoxInfo[battler].battlerIsBouncing = 0;
    }
    gSprites[bouncerSpriteId].x2 = 0;
    gSprites[bouncerSpriteId].y2 = 0;
}

void SpriteCB_BounceEffect(struct Sprite *sprite)
{
    u8 bouncerSpriteId = sprite->data[3];
    s32 index;

    if (sprite->data[4] == 1)
        index = sprite->data[0];
    else
        index = sprite->data[0];

    gSprites[bouncerSpriteId].y2 = Sin(index, sprite->data[2]) + sprite->data[2];
    sprite->data[0] = (sprite->data[0] + sprite->data[1]) & 0xFF;
}

void BeginBattleIntroDummy(void)
{
}

void BeginBattleIntro(void)
{
    BattleStartClearSetData();
    gBattleCommunication[1] = 0;
    gBattleMainFunc = BattleIntroGetMonsData;
}

#if DEBUG
void debug_sub_80138CC(void)
{
    if (GetBattlerSide(gActiveBattler) == 0)
    {
        switch (gBattleStruct->unk160FD)
        {
        case 0:
            if (gBattlerControllerFuncs[gActiveBattler] == HandleInputChooseAction)
                gBattleStruct->unk160FD++;
            break;
        case 1:
            gMain.heldKeys = A_BUTTON;
            gMain.newKeys = A_BUTTON;
            gBattleStruct->unk160FD++;
            gBattleStruct->unk160FE = 0x80;
            break;
        case 2:
            gBattleStruct->unk160FE--;
            if (gBattleStruct->unk160FE == 0)
            {
                gMain.heldKeys = A_BUTTON;
                gMain.newKeys = A_BUTTON;
                gBattleStruct->unk160FD++;
                gBattleStruct->unk160FE = 0x80;
            }
            break;
        case 3:
            gBattleStruct->unk160FE--;
            if (gBattleStruct->unk160FE == 0)
            {
                gMain.heldKeys = A_BUTTON;
                gMain.newKeys = A_BUTTON;
                gBattleStruct->unk160FD++;
            }
            break;
        case 4:
            gBattleStruct->unk160FD = 0;
            break;
        }
    }
}
#endif

static void BattleMainCB1(void)
{
#if DEBUG
    if (gUnknown_02023A14_50 & 0x80)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            debug_sub_80138CC();
        gBattleMainFunc();
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            gBattlerControllerFuncs[gActiveBattler]();
    }
    else
#endif
    {
        gBattleMainFunc();
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            gBattlerControllerFuncs[gActiveBattler]();
    }
}

static void BattleStartClearSetData(void)
{
    s32 i;
    u32 j;
    u8 *dataPtr;

    TurnValuesCleanUp(0);
    SpecialStatusesClear();

    for (i = 0; i < 4; i++)
    {
        gStatuses3[i] = 0;

        MEMSET_ALT(&gDisableStructs[i], 0, 0x1C, j, dataPtr);
        gDisableStructs[i].isFirstTurn = 2;
        sUnusedBattlersArray[i] = 0;
        gLastMoves[i] = 0;
        gLastLandedMoves[i] = 0;
        gLastHitByType[i] = 0;
        gLastResultingMoves[i] = 0;
        gLastHitBy[i] = 0xFF;
        gLockedMoves[i] = 0;
        gLastPrintedMoves[i] = 0;
        eBattleFlagsArr.flags[i] = 0;
    }

    for (i = 0; i < 2; i++)
    {
        gSideStatuses[i] = 0;
        MEMSET_ALT(&gSideTimers[i], 0, sizeof(struct SideTimer), j, dataPtr);
    }

    gBattlerAttacker = 0;
    gBattlerTarget = 0;
    gBattleWeather = 0;

    MEMSET_ALT(&gWishFutureKnock, 0, sizeof(struct WishFutureKnock), i, dataPtr);

    gHitMarker = 0;
    if ((gBattleTypeFlags & BATTLE_TYPE_LINK) == 0 && gSaveBlock2.optionsBattleSceneOff == TRUE)
        gHitMarker = HITMARKER_NO_ANIMATIONS;

    gBattleStruct->battleStyle = gSaveBlock2.optionsBattleStyle;

    gMultiHitCounter = 0;
    gBattleOutcome = 0;
    gBattleControllerExecFlags = 0;
    gPaydayMoney = 0;
    (*(u8 *)&B_BATTLESCRIPTS_STACK->size) = 0;
    (*(u8 *)&B_FUNCTION_STACK->size) = 0;

    for (i = 0; i < 8; i++)
        gBattleCommunication[i] = 0;

    gPauseCounterBattle = 0;
    gBattleMoveDamage = 0;
    gIntroSlideFlags = 0;
    gBattleStruct->animTurn = 0;
    gBattleStruct->animTargetsHit = 0;
    gLeveledUpInBattle = 0;
    gAbsentBattlerFlags = 0;
    gBattleStruct->runTries = 0;
    gBattleStruct->safariGoNearCounter = 0;
    gBattleStruct->safariPkblThrowCounter = 0;
    gBattleStruct->safariCatchFactor = gSpeciesInfo[GetMonData(&gEnemyParty[0], MON_DATA_SPECIES)].catchRate * 100 / 1275;
    gBattleStruct->safariEscapeFactor = 3;
    gBattleStruct->wildVictorySong = 0;
    gBattleStruct->moneyMultiplier = 1;

    for (i = 0; i < 8; i++)
    {
        gSharedMem[BSTRUCT_OFF(lastTakenMove)             + i] = 0;
        gSharedMem[BSTRUCT_OFF(usedHeldItems)             + i] = 0;
        gSharedMem[BSTRUCT_OFF(choicedMove)               + i] = 0;
        gSharedMem[BSTRUCT_OFF(changedItems)              + i] = 0;
        gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 0 * 8 + i] = 0;
        gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 1 * 8 + i] = 0;
    }

    gBattleStruct->AI_monToSwitchIntoId[0] = PARTY_SIZE;
    gBattleStruct->AI_monToSwitchIntoId[1] = PARTY_SIZE;

    gBattleStruct->givenExpMons = 0;

    // Reset gBattleResults
    for (i = 0; i < 11; i++)
        gBattleResults.catchAttempts[i] = 0;
    gBattleResults.battleTurnCounter = 0;
    gBattleResults.playerFaintCounter = 0;
    gBattleResults.opponentFaintCounter = 0;
    gBattleResults.totalMonSwitchCounter = 0;
    gBattleResults.numHealingItemsUsed = 0;
    gBattleResults.numRevivesUsed = 0;
    gBattleResults.playerMonWasDamaged = 0;
    gBattleResults.usedMasterBall = 0;
    gBattleResults.lastOpponentSpecies = 0;
    gBattleResults.lastUsedMovePlayer = 0;
    gBattleResults.lastUsedMoveOpponent = 0;
    gBattleResults.playerMon1Species = 0;
    gBattleResults.playerMon2Species = 0;
    gBattleResults.caughtMonSpecies = 0;
    for (i = 0; i < 10; i++)
    {
        gBattleResults.playerMon1Name[i] = 0;
        gBattleResults.playerMon2Name[i] = 0;
        gBattleResults.caughtMonNick[i] = 0;
    }
#if DEBUG
    gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) + 0] = 0;
    gSharedMem[BSTRUCT_OFF(dbgAICycleMoveTracker) + 1] = 0;
#endif
}

void SwitchInClearSetData(void)
{
    struct DisableStruct disableStructCopy = gDisableStructs[gActiveBattler];
    s32 i;
    u8 *ptr;

    if (gBattleMoves[gCurrentMove].effect != EFFECT_BATON_PASS)
    {
        for (i = 0; i < 8; i++)
            gBattleMons[gActiveBattler].statStages[i] = 6;
        for (i = 0; i < gBattlersCount; i++)
        {
            if ((gBattleMons[i].status2 & STATUS2_ESCAPE_PREVENTION) && gDisableStructs[i].battlerPreventingEscape == gActiveBattler)
                gBattleMons[i].status2 &= ~STATUS2_ESCAPE_PREVENTION;
            if ((gStatuses3[i] & STATUS3_ALWAYS_HITS) && gDisableStructs[i].battlerWithSureHit == gActiveBattler)
            {
                gStatuses3[i] &= ~STATUS3_ALWAYS_HITS;
                gDisableStructs[i].battlerWithSureHit = 0;
            }
        }
    }
    if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        gBattleMons[gActiveBattler].status2 &= (STATUS2_CONFUSION | STATUS2_FOCUS_ENERGY | STATUS2_SUBSTITUTE | STATUS2_ESCAPE_PREVENTION | STATUS2_CURSED);
        gStatuses3[gActiveBattler] &= (STATUS3_LEECHSEED_BATTLER | STATUS3_LEECHSEED | STATUS3_ALWAYS_HITS | STATUS3_PERISH_SONG | STATUS3_ROOTED | STATUS3_MUDSPORT | STATUS3_WATERSPORT);

        for (i = 0; i < gBattlersCount; i++)
        {
            if (GetBattlerSide(gActiveBattler) != GetBattlerSide(i)
             && (gStatuses3[i] & STATUS3_ALWAYS_HITS) != 0
             && (gDisableStructs[i].battlerWithSureHit == gActiveBattler))
            {
                gStatuses3[i] &= ~STATUS3_ALWAYS_HITS;
                gStatuses3[i] |= STATUS3_ALWAYS_HITS_TURN(2);
            }
        }
    }
    else
    {
        gBattleMons[gActiveBattler].status2 = 0;
        gStatuses3[gActiveBattler] = 0;
    }

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].status2 & STATUS2_INFATUATED_WITH(gActiveBattler))
            gBattleMons[i].status2 &= ~STATUS2_INFATUATED_WITH(gActiveBattler);
        if ((gBattleMons[i].status2 & STATUS2_WRAPPED) && gSharedMem[BSTRUCT_OFF(wrappedBy) + i] == gActiveBattler)
            gBattleMons[i].status2 &= ~STATUS2_WRAPPED;
    }

    gActionSelectionCursor[gActiveBattler] = 0;
    gMoveSelectionCursor[gActiveBattler] = 0;

    MEMSET_ALT(&gDisableStructs[gActiveBattler], 0, 0x1C, i, ptr);

    if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        gDisableStructs[gActiveBattler].substituteHP = disableStructCopy.substituteHP;
        gDisableStructs[gActiveBattler].battlerWithSureHit = disableStructCopy.battlerWithSureHit;
        gDisableStructs[gActiveBattler].perishSongTimer = disableStructCopy.perishSongTimer;
        gDisableStructs[gActiveBattler].perishSongTimerStartValue = disableStructCopy.perishSongTimerStartValue;
    }

    gDisableStructs[gActiveBattler].isFirstTurn= 2;
    gLastMoves[gActiveBattler] = 0;
    gLastLandedMoves[gActiveBattler] = 0;
    gLastHitByType[gActiveBattler] = 0;
    gLastResultingMoves[gActiveBattler] = 0;
    gLastPrintedMoves[gActiveBattler] = 0;
    gLastHitBy[gActiveBattler] = 0xFF;

    gSharedMem[BSTRUCT_OFF(lastTakenMove)     + 2 * gActiveBattler + 0] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMove)     + 2 * gActiveBattler + 1] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 0] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 1] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 2] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 3] = 0;
    gSharedMem[BSTRUCT_OFF(choicedMove)       + 2 * gActiveBattler + 0] = 0;
    gSharedMem[BSTRUCT_OFF(choicedMove)       + 2 * gActiveBattler + 1] = 0;

    eBattleFlagsArr.flags[gActiveBattler] = 0;

    gCurrentMove = 0;
}

void UndoEffectsAfterFainting(void)
{
    s32 i;
    u8 *ptr;

    for (i = 0; i < 8; i++)
        gBattleMons[gActiveBattler].statStages[i] = 6;
    gBattleMons[gActiveBattler].status2 = 0;
    gStatuses3[gActiveBattler] = 0;
    for (i = 0; i < gBattlersCount; i++)
    {
        if ((gBattleMons[i].status2 & STATUS2_ESCAPE_PREVENTION) && gDisableStructs[i].battlerPreventingEscape == gActiveBattler)
            gBattleMons[i].status2 &= ~STATUS2_ESCAPE_PREVENTION;
        if (gBattleMons[i].status2 & STATUS2_INFATUATED_WITH(gActiveBattler))
            gBattleMons[i].status2 &= ~STATUS2_INFATUATED_WITH(gActiveBattler);
        if ((gBattleMons[i].status2 & STATUS2_WRAPPED) && gSharedMem[BSTRUCT_OFF(wrappedBy) + i] == gActiveBattler)
            gBattleMons[i].status2 &= ~STATUS2_WRAPPED;
    }
    gActionSelectionCursor[gActiveBattler] = 0;
    gMoveSelectionCursor[gActiveBattler] = 0;

    MEMSET_ALT(&gDisableStructs[gActiveBattler], 0, 0x1C, i, ptr);
    gProtectStructs[gActiveBattler].protected = 0;
    gProtectStructs[gActiveBattler].endured = 0;
    gProtectStructs[gActiveBattler].noValidMoves = 0;
    gProtectStructs[gActiveBattler].helpingHand = 0;
    gProtectStructs[gActiveBattler].bounceMove = 0;
    gProtectStructs[gActiveBattler].stealMove = 0;
    gProtectStructs[gActiveBattler].flag0Unknown = 0;
    gProtectStructs[gActiveBattler].prlzImmobility = 0;
    gProtectStructs[gActiveBattler].confusionSelfDmg = 0;
    gProtectStructs[gActiveBattler].targetNotAffected = 0;
    gProtectStructs[gActiveBattler].chargingTurn = 0;
    gProtectStructs[gActiveBattler].fleeType = 0;
    gProtectStructs[gActiveBattler].usedImprisonedMove = 0;
    gProtectStructs[gActiveBattler].loveImmobility = 0;
    gProtectStructs[gActiveBattler].usedDisabledMove = 0;
    gProtectStructs[gActiveBattler].usedTauntedMove = 0;
    gProtectStructs[gActiveBattler].flag2Unknown = 0;
    gProtectStructs[gActiveBattler].flinchImmobility = 0;
    gProtectStructs[gActiveBattler].notFirstStrike = 0;

    gDisableStructs[gActiveBattler].isFirstTurn= 2;
    gLastMoves[gActiveBattler] = 0;
    gLastLandedMoves[gActiveBattler] = 0;
    gLastHitByType[gActiveBattler] = 0;
    gLastResultingMoves[gActiveBattler] = 0;
    gLastPrintedMoves[gActiveBattler] = 0;
    gLastHitBy[gActiveBattler] = 0xFF;

    gSharedMem[BSTRUCT_OFF(choicedMove) + 2 * gActiveBattler + 0] = 0;
    gSharedMem[BSTRUCT_OFF(choicedMove) + 2 * gActiveBattler + 1] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMove) + 2 * gActiveBattler + 0] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMove) + 2 * gActiveBattler + 1] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 0] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 1] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 2] = 0;
    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gActiveBattler + 3] = 0;

    eBattleFlagsArr.flags[gActiveBattler] = 0;

    gBattleMons[gActiveBattler].type1 = gSpeciesInfo[gBattleMons[gActiveBattler].species].type1;
    gBattleMons[gActiveBattler].type2 = gSpeciesInfo[gBattleMons[gActiveBattler].species].type2;
}

static void BattleIntroGetMonsData(void)
{
    switch (gBattleCommunication[0])
    {
    case 0:
        gActiveBattler = gBattleCommunication[1];
        BtlController_EmitGetMonData(0, 0, 0);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattleCommunication[0]++;
        break;
    case 1:
        if (gBattleControllerExecFlags == 0)
        {
            gBattleCommunication[1]++;
            if (gBattleCommunication[1] == gBattlersCount)
                gBattleMainFunc = BattleIntroPrepareBackgroundSlide;
            else
                gBattleCommunication[0] = 0;
        }
        break;
    }
}

static void BattleIntroPrepareBackgroundSlide(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        gActiveBattler = GetBattlerAtPosition(0);
        BtlController_EmitIntroSlide(0, gBattleEnvironment);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattleMainFunc = sub_8011384;
        gBattleCommunication[0] = 0;
        gBattleCommunication[1] = 0;
    }
}

void sub_8011384(void)
{
    u8 *ptr;
    s32 i;

    if (gBattleControllerExecFlags == 0)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if ((gBattleTypeFlags & BATTLE_TYPE_SAFARI)
             && GetBattlerSide(gActiveBattler) == 0)
            {
                MEMSET_ALT(&gBattleMons[gActiveBattler], 0, 0x58, i, ptr);
            }
            else
            {
                u16 *hpOnSwitchout;

                MEMSET_ALT(&gBattleMons[gActiveBattler], gBattleBufferB[gActiveBattler][4 + i], 0x58, i, ptr);
                gBattleMons[gActiveBattler].type1 = gSpeciesInfo[gBattleMons[gActiveBattler].species].type1;
                gBattleMons[gActiveBattler].type2 = gSpeciesInfo[gBattleMons[gActiveBattler].species].type2;
                gBattleMons[gActiveBattler].ability = GetAbilityBySpecies(gBattleMons[gActiveBattler].species, gBattleMons[gActiveBattler].abilityNum);

                hpOnSwitchout = &gBattleStruct->hpOnSwitchout[GetBattlerSide(gActiveBattler)];
                *hpOnSwitchout = gBattleMons[gActiveBattler].hp;
                for (i = 0; i < 8; i++)
                    gBattleMons[gActiveBattler].statStages[i] = 6;
                gBattleMons[gActiveBattler].status2 = 0;
            }

            if (GetBattlerPosition(gActiveBattler) == 0)
            {
                BtlController_EmitDrawTrainerPic(0);
                MarkBattlerForControllerExec(gActiveBattler);
            }

            if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
            {
                if (GetBattlerPosition(gActiveBattler) == 1)
                {
                    BtlController_EmitDrawTrainerPic(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
                if (GetBattlerSide(gActiveBattler) == 1
                 && !(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK)))
                    GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gActiveBattler].species), 2);
            }
            else
            {
                if (GetBattlerSide(gActiveBattler) == 1
                 && !(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK)))
                {
                    GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gActiveBattler].species), 2);
                    BtlController_EmitLoadMonSprite(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
            }

            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                if (GetBattlerPosition(gActiveBattler) == 2
                 || GetBattlerPosition(gActiveBattler) == 3)
                {
                    BtlController_EmitDrawTrainerPic(0);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
            }
        }
        gBattleMainFunc = BattleIntroDrawPartySummaryScreens;
    }
}

void BattleIntroDrawPartySummaryScreens(void)
{
    s32 i;

    if (gBattleControllerExecFlags == 0)
    {
		struct HpAndStatus hpStatus[6];

        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            for (i = 0; i < 6; i++)
            {
                if (GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG) == 0
                 || GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
                {
                    hpStatus[i].hp = 0xFFFF;
                    hpStatus[i].status = 0;
                }
                else
                {
                    hpStatus[i].hp = GetMonData(&gEnemyParty[i], MON_DATA_HP);
                    hpStatus[i].status = GetMonData(&gEnemyParty[i], MON_DATA_STATUS);
                }
            }
            gActiveBattler = GetBattlerAtPosition(1);
            BtlController_EmitDrawPartyStatusSummary(0, hpStatus, 0x80);
            MarkBattlerForControllerExec(gActiveBattler);

            for (i = 0; i < 6; i++)
            {
                if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG) == 0
                 || GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
                {
                    hpStatus[i].hp = 0xFFFF;
                    hpStatus[i].status = 0;
                }
                else
                {
                    hpStatus[i].hp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
                    hpStatus[i].status = GetMonData(&gPlayerParty[i], MON_DATA_STATUS);
                }
            }
            gActiveBattler = GetBattlerAtPosition(0);
            BtlController_EmitDrawPartyStatusSummary(0, hpStatus, 0x80);
            MarkBattlerForControllerExec(gActiveBattler);

            gBattleMainFunc = BattleIntroPrintTrainerWantsToBattle;
        }
        else
        {
            // The array gets set here, but nothing is ever done with it.
            // Likely unfinished code.

            for (i = 0; i < 6; i++)
            {
                if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG) == 0
                 || GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
                {
                    hpStatus[i].hp = 0xFFFF;
                    hpStatus[i].status = 0;
                }
                else
                {
                    hpStatus[i].hp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
                    hpStatus[i].status = GetMonData(&gPlayerParty[i], MON_DATA_STATUS);
                }
            }

            gBattleMainFunc = BattleIntroPrintWildMonAttacked;
        }
    }
}

void BattleIntroPrintTrainerWantsToBattle(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        gActiveBattler = GetBattlerAtPosition(1);
        PrepareStringBattle(0, gActiveBattler);
        gBattleMainFunc = sub_8011800;
    }
}

void BattleIntroPrintWildMonAttacked(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        gBattleMainFunc = BattleIntroPrintPlayerSendsOut;
        PrepareStringBattle(0, 0);
    }
}

void sub_8011800(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        PrepareStringBattle(1, GetBattlerAtPosition(1));
        gBattleMainFunc = sub_8011834;
    }
}

void sub_8011834(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if (GetBattlerPosition(gActiveBattler) == 1)
            {
                BtlController_EmitIntroTrainerBallThrow(0);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            if ((gBattleTypeFlags & BATTLE_TYPE_MULTI)
             && GetBattlerPosition(gActiveBattler) == 3)
            {
                BtlController_EmitIntroTrainerBallThrow(0);
                MarkBattlerForControllerExec(gActiveBattler);
            }
        }
        gBattleMainFunc = bc_801362C;
    }
}

void bc_801362C(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if (GetBattlerSide(gActiveBattler) == 1
             && !(gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK)))
                GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gActiveBattler].species), 2);
        }
        gBattleMainFunc = BattleIntroPrintPlayerSendsOut;
    }
}

static void UNUSED BattleIntroSkipRecordMonsToDex(void)
{
    if (gBattleControllerExecFlags == 0)
        gBattleMainFunc = BattleIntroPrintPlayerSendsOut;
}

static void BattleIntroPrintPlayerSendsOut(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
            PrepareStringBattle(1, GetBattlerAtPosition(0));
        gBattleMainFunc = sub_80119B4;
    }
}

void sub_80119B4(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if (GetBattlerPosition(gActiveBattler) == 0)
            {
                BtlController_EmitIntroTrainerBallThrow(0);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            if ((gBattleTypeFlags & BATTLE_TYPE_MULTI)
             && GetBattlerPosition(gActiveBattler) == 2)
            {
                BtlController_EmitIntroTrainerBallThrow(0);
                MarkBattlerForControllerExec(gActiveBattler);
            }
        }
        gBattleStruct->switchInAbilitiesCounter = 0;
        gBattleStruct->switchInItemsCounter = 0;
        gBattleStruct->overworldWeatherDone = 0;
        gBattleMainFunc = BattleBeginFirstTurn;
    }
}

void BattleIntroSwitchInPlayerMons(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if (GetBattlerSide(gActiveBattler) == 0)
            {
                BtlController_EmitSwitchInAnim(0, gBattlerPartyIndexes[gActiveBattler], 0);
                MarkBattlerForControllerExec(gActiveBattler);
            }
        }
        gBattleStruct->switchInAbilitiesCounter = 0;
        gBattleStruct->switchInItemsCounter = 0;
        gBattleStruct->overworldWeatherDone = 0;
        gBattleMainFunc = BattleBeginFirstTurn;
    }
}

void BattleBeginFirstTurn(void)
{
    s32 i;
    s32 j;
    u8 effect = 0;

    if (gBattleControllerExecFlags == 0)
    {
        if (gBattleStruct->switchInAbilitiesCounter == 0)
        {
            for (i = 0; i < gBattlersCount; i++)
                gBattlerByTurnOrder[i] = i;
            for (i = 0; i < gBattlersCount - 1; i++)
            {
                for (j = i + 1; j < gBattlersCount; j++)
                {
                    if (GetWhoStrikesFirst(gBattlerByTurnOrder[i], gBattlerByTurnOrder[j], 1) != 0)
                        SwapTurnOrder(i, j);
                }
            }
        }
        if (gBattleStruct->overworldWeatherDone == 0 && AbilityBattleEffects(0, 0, 0, 0xFF, 0) != 0)
        {
            gBattleStruct->overworldWeatherDone = 1;
            return;
        }
        while (gBattleStruct->switchInAbilitiesCounter < gBattlersCount)
        {
            if (AbilityBattleEffects(0, gBattlerByTurnOrder[gBattleStruct->switchInAbilitiesCounter], 0, 0, 0) != 0)
                effect++;
            gBattleStruct->switchInAbilitiesCounter++;
            if (effect != 0)
                return;
        }
        if (AbilityBattleEffects(9, 0, 0, 0, 0) != 0)
            return;
        if (AbilityBattleEffects(11, 0, 0, 0, 0) != 0)
            return;
        while (gBattleStruct->switchInItemsCounter < gBattlersCount)
        {
            if (ItemBattleEffects(0, gBattlerByTurnOrder[gBattleStruct->switchInItemsCounter], 0) != 0)
                effect++;
            gBattleStruct->switchInItemsCounter++;
            if (effect != 0)
                return;
        }
        // Absolutely pointless for-loop that somehow doesn't get optimized out
        for (i = 0; i < gBattlersCount; i++)
            ;
        for (i = 0; i < 4; i++)
        {
            gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + i] = PARTY_SIZE;
            gChosenActionByBattler[i] = 0xFF;
            gChosenMoveByBattler[i] = 0;
        }
        TurnValuesCleanUp(0);
        SpecialStatusesClear();
        gBattleStruct->absentBattlerFlags = gAbsentBattlerFlags;
        gBattleMainFunc = sub_8012324;
        ResetSentPokesToOpponentValue();
        for (i = 0; i < 8; i++)
            gBattleCommunication[i] = 0;
        for (i = 0; i < gBattlersCount; i++)
            gBattleMons[i].status2 &= ~STATUS2_FLINCHED;
        gBattleStruct->turnEffectsTracker = 0;
        gBattleStruct->turnEffectsBattlerId = 0;
        gBattleStruct->wishPerishSongState = 0;
        gBattleStruct->wishPerishSongBattlerId = 0;
        gBattleStruct->moveendState = 0;
        gBattleStruct->faintedActionsState = 0;
        gBattleStruct->turnCountersTracker = 0;
        gMoveResultFlags = 0;
        gRandomTurnNumber = Random();
    }
}

void HandleEndTurn_ContinueBattle(void)
{
    s32 i;

    if (gBattleControllerExecFlags == 0)
    {
        gBattleMainFunc = BattleTurnPassed;
        for (i = 0; i < 8; i++)
            gBattleCommunication[i] = 0;
        for (i = 0; i < gBattlersCount; i++)
        {
            gBattleMons[i].status2 &= ~STATUS2_FLINCHED;
            if ((gBattleMons[i].status1 & STATUS1_SLEEP) && (gBattleMons[i].status2 & STATUS2_MULTIPLETURNS))
                CancelMultiTurnMoves(i);
        }
        gBattleStruct->turnEffectsTracker = 0;
        gBattleStruct->turnEffectsBattlerId = 0;
        gBattleStruct->wishPerishSongState = 0;
        gBattleStruct->wishPerishSongBattlerId = 0;
        gBattleStruct->turnCountersTracker = 0;
        gMoveResultFlags = 0;
    }
}

void BattleTurnPassed(void)
{
    s32 i;

    TurnValuesCleanUp(1);
    if (gBattleOutcome == 0)
    {
        if (DoFieldEndTurnEffects() != 0)
            return;
        if (DoBattlerEndTurnEffects() != 0)
            return;
    }
    if (HandleFaintedMonActions() != 0)
        return;
    gBattleStruct->faintedActionsState = 0;
    if (HandleWishPerishSongOnTurnEnd() != 0)
        return;
    TurnValuesCleanUp(0);
    gHitMarker &= ~HITMARKER_NO_ATTACKSTRING;
    gHitMarker &= ~0x80000;
    gHitMarker &= ~0x400000;
    gHitMarker &= ~0x100000;
    gBattleStruct->animTurn = 0;
    gBattleStruct->animTargetsHit = 0;
    gBattleStruct->moveendState = 0;
    gBattleMoveDamage = 0;
    gMoveResultFlags = 0;
    for (i = 0; i < 5; i++)
        gBattleCommunication[i] = 0;
    if (gBattleOutcome != 0)
    {
        gCurrentActionFuncId = 12;
        gBattleMainFunc = RunTurnActionsFunctions;
        return;
    }
    if (gBattleResults.battleTurnCounter < 0xFF)
        gBattleResults.battleTurnCounter++;
    for (i = 0; i < gBattlersCount; i++)
    {
        gChosenActionByBattler[i] = 0xFF;
        gChosenMoveByBattler[i] = 0;
    }
    for (i = 0; i < 4; i++)
        gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + i] = 6;
    gBattleStruct->absentBattlerFlags = gAbsentBattlerFlags;
    gBattleMainFunc = sub_8012324;
    gRandomTurnNumber = Random();
}

u8 IsRunningFromBattleImpossible(void)
{
    u8 holdEffect;
    u8 side;
    s32 i;

    if (gBattleMons[gActiveBattler].item == ITEM_ENIGMA_BERRY)
        holdEffect = gEnigmaBerries[gActiveBattler].holdEffect;
    else
        holdEffect = GetItemHoldEffect(gBattleMons[gActiveBattler].item);
    gPotentialItemEffectBattler = gActiveBattler;
    if (holdEffect == HOLD_EFFECT_CAN_ALWAYS_RUN)
        return 0;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        return 0;
    if (gBattleMons[gActiveBattler].ability == ABILITY_RUN_AWAY)
        return 0;
    side = GetBattlerSide(gActiveBattler);
    for (i = 0; i < gBattlersCount; i++)
    {
        if (side != GetBattlerSide(i)
         && gBattleMons[i].ability == ABILITY_SHADOW_TAG)
        {
            gBattleStruct->battler = i;
            gLastUsedAbility = gBattleMons[i].ability;
            gBattleCommunication[5] = 2;
            return 2;
        }
        if (side != GetBattlerSide(i)
         && gBattleMons[gActiveBattler].ability != ABILITY_LEVITATE
         && gBattleMons[gActiveBattler].type1 != 2
         && gBattleMons[gActiveBattler].type2 != 2
         && gBattleMons[i].ability == ABILITY_ARENA_TRAP)
        {
            gBattleStruct->battler = i;
            gLastUsedAbility = gBattleMons[i].ability;
            gBattleCommunication[5] = 2;
            return 2;
        }
    }
    i = AbilityBattleEffects(15, gActiveBattler, ABILITY_MAGNET_PULL, 0, 0);
    if (i != 0 && (gBattleMons[gActiveBattler].type1 == 8 || gBattleMons[gActiveBattler].type2 == 8))
    {
        gBattleStruct->battler = i - 1;
        gLastUsedAbility = gBattleMons[i - 1].ability;
        gBattleCommunication[5] = 2;
        return 2;
    }
    if ((gBattleMons[gActiveBattler].status2 & (STATUS2_ESCAPE_PREVENTION | STATUS2_WRAPPED)) || (gStatuses3[gActiveBattler] & STATUS3_ROOTED))
    {
        gBattleCommunication[5] = 0;
        return 1;
    }
    if (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE)
    {
        gBattleCommunication[5] = 1;
        return 1;
    }
    return 0;
}

void SwitchPartyOrder(u8 battler)
{
    s32 i;
    u8 partyId1;
    u8 partyId2;

    for (i = 0; i < 3; i++)
        gBattlePartyCurrentOrder[i] = gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + i + battler * 3];
    partyId1 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battler]);
    partyId2 = GetPartyIdFromBattlePartyId(gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + battler]);
    SwitchPartyMonSlots(partyId1, partyId2);
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        for (i = 0; i < 3; i++)
        {
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + i + battler * 3] = gBattlePartyCurrentOrder[i];
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + i + (battler ^ BIT_FLANK) * 3] =
                gBattlePartyCurrentOrder[i];
        }
    }
    else
    {
        for (i = 0; i < 3; i++)
        {
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + i + battler * 3] = gBattlePartyCurrentOrder[i];
        }
    }
}

enum
{
    STATE_BEFORE_ACTION_CHOSEN,
    STATE_WAIT_ACTION_CHOSEN,
    STATE_WAIT_ACTION_CASE_CHOSEN,
    STATE_WAIT_ACTION_CONFIRMED_STANDBY,
    STATE_WAIT_ACTION_CONFIRMED,
    STATE_SELECTION_SCRIPT,
    STATE_WAIT_SET_BEFORE_ACTION,
    STATE_SELECTION_SCRIPT_MAY_RUN
};

extern u8 * gSelectionBattleScripts[];
extern u8 BattleScript_ActionSelectionItemsCantBeUsed[];
extern u8 BattleScript_PrintFullBox[];
extern u8 BattleScript_PrintCantRunFromTrainer[];
extern u8 BattleScript_PrintCantEscapeFromBattle[];

void sub_8012324(void)
{
    u8 position;
    s32 i;

    gBattleCommunication[4] = 0;
    // inverted loop
    //_0801234C
    for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
    {
        position = GetBattlerPosition(gActiveBattler);
        switch (gBattleCommunication[gActiveBattler])
        {
            case STATE_BEFORE_ACTION_CHOSEN:
                gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + gActiveBattler] = 6;
                if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    && (position & BIT_FLANK) != B_FLANK_LEFT
                    && !(gBattleStruct->absentBattlerFlags & gBitTable[GetBattlerAtPosition(BATTLE_PARTNER(position))])
                    && gBattleCommunication[GetBattlerAtPosition(BATTLE_PARTNER(position))] != STATE_WAIT_ACTION_CONFIRMED)
                    break;
                //_080123F8
                if (gBattleStruct->absentBattlerFlags & gBitTable[gActiveBattler])
                {
                    gChosenActionByBattler[gActiveBattler] = 13;
                    if (!(gBattleTypeFlags & 0x40))
                        gBattleCommunication[gActiveBattler] = 4;
                        //_08012454
                    else
                        gBattleCommunication[gActiveBattler] = 3;
                    break;
                }
                //_08012468
                if ((gBattleMons[gActiveBattler].status2 & STATUS2_MULTIPLETURNS)
                    || (gBattleMons[gActiveBattler].status2 & STATUS2_RECHARGE))
                {
                    gChosenActionByBattler[gActiveBattler] = 0;
                    gBattleCommunication[gActiveBattler] = 3;
                }
                else
                {
                    BtlController_EmitChooseAction(0, gChosenActionByBattler[0], gBattleBufferB[0][1] | (gBattleBufferB[0][2] << 8));
                    MarkBattlerForControllerExec(gActiveBattler);
                    gBattleCommunication[gActiveBattler]++;
                }
                break;
            case STATE_WAIT_ACTION_CHOSEN:
                if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF0000000) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 0xC))))
                {
                    gChosenActionByBattler[gActiveBattler] = gBattleBufferB[gActiveBattler][1];
                    switch (gBattleBufferB[gActiveBattler][1])
                    {
                        case B_ACTION_USE_MOVE:
                            if (AreAllMovesUnusable())
                            {
                                gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                                gSharedMem[BSTRUCT_OFF(selectionScriptFinished) + gActiveBattler] = FALSE;
                                gSharedMem[BSTRUCT_OFF(stateIdAfterSelScript) + gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                                gSharedMem[BSTRUCT_OFF(moveTarget) + gActiveBattler] = gBattleBufferB[gActiveBattler][3];
                                return;
                            }
                            else if (gDisableStructs[gActiveBattler].encoredMove != 0)
                            {
                                gChosenMoveByBattler[gActiveBattler] = gDisableStructs[gActiveBattler].encoredMove;
                                gBattleCommunication[gActiveBattler] = STATE_WAIT_ACTION_CONFIRMED_STANDBY;
                                return;
                            }
                            else
                            {
                                struct ChooseMoveStruct moveInfo;

                                moveInfo.species = gBattleMons[gActiveBattler].species;
                                moveInfo.monType1 = gBattleMons[gActiveBattler].type1;
                                moveInfo.monType2 = gBattleMons[gActiveBattler].type2;

                                for (i = 0; i < 4; i++)
                                {
                                    moveInfo.moves[i] = gBattleMons[gActiveBattler].moves[i];
                                    moveInfo.currentPP[i] = gBattleMons[gActiveBattler].pp[i];
                                    moveInfo.maxPP[i] = CalculatePPWithBonus(
                                        gBattleMons[gActiveBattler].moves[i],
                                        gBattleMons[gActiveBattler].ppBonuses,
                                        i);
                                }

                                BtlController_EmitChooseMove(0, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) != 0, FALSE, (u8 *)&moveInfo);
                                MarkBattlerForControllerExec(gActiveBattler);
                            }
                            break;
                        case B_ACTION_USE_ITEM:
                            if (gBattleTypeFlags & (BATTLE_TYPE_LINK
                                                    | BATTLE_TYPE_BATTLE_TOWER
                                                    | BATTLE_TYPE_EREADER_TRAINER))
                            {
                                gSelectionBattleScripts[gActiveBattler] = BattleScript_ActionSelectionItemsCantBeUsed;
                                gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                                gSharedMem[BSTRUCT_OFF(selectionScriptFinished) + gActiveBattler] = FALSE;
                                gSharedMem[BSTRUCT_OFF(stateIdAfterSelScript) + gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                                return;
                            }
                            else
                            {
                                BtlController_EmitChooseItem(0, &gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0]);
                                MarkBattlerForControllerExec(gActiveBattler);
                            }
                            break;
                        case B_ACTION_SWITCH:
                            gSharedMem[BSTRUCT_OFF(battlerPartyIndexes) + gActiveBattler] = gBattlerPartyIndexes[gActiveBattler];
                            if (gBattleMons[gActiveBattler].status2 & (STATUS2_WRAPPED | STATUS2_ESCAPE_PREVENTION)
                                || gStatuses3[gActiveBattler] & STATUS3_ROOTED)
                            {
                                BtlController_EmitChoosePokemon(0, 2, 6, ABILITY_NONE, &gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0]);
                            }
                            else if ((i = ABILITY_ON_OPPOSING_FIELD(gActiveBattler, ABILITY_SHADOW_TAG))
                                     || ((i = ABILITY_ON_OPPOSING_FIELD(gActiveBattler, ABILITY_ARENA_TRAP))
                                         && !IS_BATTLER_OF_TYPE(gActiveBattler, TYPE_FLYING)
                                         && gBattleMons[gActiveBattler].ability != ABILITY_LEVITATE)
                                     || ((i = AbilityBattleEffects(ABILITYEFFECT_CHECK_FIELD_EXCEPT_BATTLER, gActiveBattler, ABILITY_MAGNET_PULL, 0, 0))
                                         && IS_BATTLER_OF_TYPE(gActiveBattler, TYPE_STEEL)))
                            {
                                BtlController_EmitChoosePokemon(0, ((i - 1) << 4) | PARTY_ABILITY_PREVENTS, 6, gLastUsedAbility, &gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0]);
                            }
                            else
                            {
                                if (gActiveBattler == 2 && gChosenActionByBattler[0] == B_ACTION_SWITCH)
                                    BtlController_EmitChoosePokemon(0, PARTY_CHOOSE_MON, gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + 0], ABILITY_NONE, &gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0]);
                                else if (gActiveBattler == 3 && gChosenActionByBattler[1] == B_ACTION_SWITCH)
                                    BtlController_EmitChoosePokemon(0, PARTY_CHOOSE_MON, gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + 1], ABILITY_NONE, &gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0]);
                                else
                                    BtlController_EmitChoosePokemon(0, PARTY_CHOOSE_MON, 6, ABILITY_NONE, &gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0]);
                            }
                            MarkBattlerForControllerExec(gActiveBattler);
                            break;
                        case B_ACTION_SAFARI_BALL:
                            if (IsPlayerPartyAndPokemonStorageFull())
                            {
                                gSelectionBattleScripts[gActiveBattler] = BattleScript_PrintFullBox;
                                gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                                gSharedMem[BSTRUCT_OFF(selectionScriptFinished) + gActiveBattler] = FALSE;
                                gSharedMem[BSTRUCT_OFF(stateIdAfterSelScript) + gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                                return;
                            }
                            break;
                        case B_ACTION_SAFARI_POKEBLOCK:
                            BtlController_EmitChooseItem(0, &gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0]);
                            MarkBattlerForControllerExec(gActiveBattler);
                            break;
                        case B_ACTION_CANCEL_PARTNER:
                            gBattleCommunication[gActiveBattler] = STATE_WAIT_SET_BEFORE_ACTION;
                            gBattleCommunication[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))] = STATE_BEFORE_ACTION_CHOSEN;
                            BtlController_EmitEndBounceEffect(0);
                            MarkBattlerForControllerExec(gActiveBattler);
                            return;
                    }

                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER
                             && !(gBattleTypeFlags & (BATTLE_TYPE_LINK))
                             && gBattleBufferB[gActiveBattler][1] == B_ACTION_RUN)
                    {
                        BattleScriptExecute(BattleScript_PrintCantRunFromTrainer);
                        gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                    }
                    else if (IsRunningFromBattleImpossible()
                             && gBattleBufferB[gActiveBattler][1] == B_ACTION_RUN)
                    {
                        gSelectionBattleScripts[gActiveBattler] = BattleScript_PrintCantEscapeFromBattle;
                        gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                        gSharedMem[BSTRUCT_OFF(selectionScriptFinished) + gActiveBattler] = FALSE;
                        gSharedMem[BSTRUCT_OFF(stateIdAfterSelScript) + gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                        return;
                    }
                    else
                    {
                        gBattleCommunication[gActiveBattler]++;
                    }
                }
                break;
            case STATE_WAIT_ACTION_CASE_CHOSEN:
                if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF0000000) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 0xC))))
                {
                    switch (gChosenActionByBattler[gActiveBattler])
                    {
                        case B_ACTION_USE_MOVE:
                            switch (gBattleBufferB[gActiveBattler][1])
                            {
                                case 3:
                                case 4:
                                case 5:
                                case 6:
                                case 7:
                                case 8:
                                case 9:
                                    gChosenActionByBattler[gActiveBattler] = gBattleBufferB[gActiveBattler][1];
                                    return;
                                default:
                                    if ((gBattleBufferB[gActiveBattler][2] | (gBattleBufferB[gActiveBattler][3] << 8)) == 0xFFFF)
                                    {
                                        gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                                    }
                                    else if (TrySetCantSelectMoveBattleScript())
                                    {
                                        gBattleCommunication[gActiveBattler] = STATE_SELECTION_SCRIPT;
                                        gSharedMem[BSTRUCT_OFF(selectionScriptFinished) + gActiveBattler] = FALSE;
                                        gBattleBufferB[gActiveBattler][1] = 0;
                                        gSharedMem[BSTRUCT_OFF(stateIdAfterSelScript) + gActiveBattler] = STATE_WAIT_ACTION_CHOSEN;
                                        return;
                                    }
                                    else
                                    {
                                        gSharedMem[BSTRUCT_OFF(chosenMovePositions) + gActiveBattler] = gBattleBufferB[gActiveBattler][2];
                                        gChosenMoveByBattler[gActiveBattler] = gBattleMons[gActiveBattler].moves[gSharedMem[BSTRUCT_OFF(chosenMovePositions) + gActiveBattler]];
                                        gSharedMem[BSTRUCT_OFF(moveTarget) + gActiveBattler] = gBattleBufferB[gActiveBattler][3];
                                        gBattleCommunication[gActiveBattler]++;
                                    }
                                    break;
                            }
                            break;
                        case B_ACTION_USE_ITEM:
                            if ((gBattleBufferB[gActiveBattler][1] | (gBattleBufferB[gActiveBattler][2] << 8)) == 0)
                            {
                                gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                            }
                            else
                            {
                                gLastUsedItem = (gBattleBufferB[gActiveBattler][1] | (gBattleBufferB[gActiveBattler][2] << 8));
                                gBattleCommunication[gActiveBattler]++;
                            }
                            break;
                        case B_ACTION_SWITCH:
                            if (gBattleBufferB[gActiveBattler][1] == PARTY_SIZE)
                            {
                                gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                            }
                            else
                            {
                                gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + gActiveBattler] = gBattleBufferB[gActiveBattler][1];

                                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                                {
                                    gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0] &= 0xF;
                                    gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0] |= (gBattleBufferB[gActiveBattler][2] & 0xF0);
                                    gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 1] = gBattleBufferB[gActiveBattler][3];

                                    gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * (gActiveBattler ^ BIT_FLANK) + 0] &= (0xF0);
                                    gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * (gActiveBattler ^ BIT_FLANK) + 0] |= (gBattleBufferB[gActiveBattler][2] & 0xF0) >> 4;
                                    gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * (gActiveBattler ^ BIT_FLANK) + 2] = gBattleBufferB[gActiveBattler][3];
                                }
                                gBattleCommunication[gActiveBattler]++;
                            }
                            break;
                        case B_ACTION_RUN:
                            gHitMarker |= HITMARKER_RUN;
                            gBattleCommunication[gActiveBattler]++;
                            break;
                        case B_ACTION_SAFARI_WATCH_CAREFULLY:
                            gBattleCommunication[gActiveBattler]++;
                            break;
                        case B_ACTION_SAFARI_BALL:
                            gBattleCommunication[gActiveBattler]++;
                            break;
                        case B_ACTION_SAFARI_POKEBLOCK:
                            if ((gBattleBufferB[gActiveBattler][1] | (gBattleBufferB[gActiveBattler][2] << 8)) != 0)
                            {
                                gBattleCommunication[gActiveBattler]++;
                            }
                            else
                            {
                                gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                            }
                            break;
                        case B_ACTION_SAFARI_GO_NEAR:
                            gBattleCommunication[gActiveBattler]++;
                            break;
                        case B_ACTION_SAFARI_RUN:
                            gHitMarker |= HITMARKER_RUN;
                            gBattleCommunication[gActiveBattler]++;
                            break;
                        case B_ACTION_WALLY_THROW:
                            gBattleCommunication[gActiveBattler]++;
                            break;
                    }
                }
                break;
            case STATE_WAIT_ACTION_CONFIRMED_STANDBY:
                if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF0000000) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 0xC))))
                {
                    if (((gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE)) != BATTLE_TYPE_DOUBLE)
                        || (position & BIT_FLANK) != B_FLANK_LEFT
                        || (gBattleStruct->absentBattlerFlags & gBitTable[GetBattlerAtPosition(position ^ BIT_FLANK)]))
                    {
                        BtlController_EmitLinkStandbyMsg(0, 0);
                    }
                    else
                    {
                        BtlController_EmitLinkStandbyMsg(0, 1);
                    }
                    MarkBattlerForControllerExec(gActiveBattler);
                    gBattleCommunication[gActiveBattler]++;
                }
                break;
            case STATE_WAIT_ACTION_CONFIRMED:
                if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF0000000) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 0xC))))
                {
                    gBattleCommunication[ACTIONS_CONFIRMED_COUNT]++;
                }
                break;
            case STATE_SELECTION_SCRIPT:
                if (gSharedMem[BSTRUCT_OFF(selectionScriptFinished) + gActiveBattler])
                {
                    gBattleCommunication[gActiveBattler] = gSharedMem[BSTRUCT_OFF(stateIdAfterSelScript) + gActiveBattler];
                }
                else
                {
                    gBattlerAttacker = gActiveBattler;
                    gBattlescriptCurrInstr = gSelectionBattleScripts[gActiveBattler];
                    if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF0000000) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 0xC))))
                    {
                        gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
                    }
                    gSelectionBattleScripts[gActiveBattler] = gBattlescriptCurrInstr;
                }
                break;
            case STATE_WAIT_SET_BEFORE_ACTION:
                if (!(gBattleControllerExecFlags & ((gBitTable[gActiveBattler]) | (0xF0000000) | (gBitTable[gActiveBattler] << 4) | (gBitTable[gActiveBattler] << 8) | (gBitTable[gActiveBattler] << 0xC))))
                {
                    gBattleCommunication[gActiveBattler] = STATE_BEFORE_ACTION_CHOSEN;
                }
                break;
        }
    }

    // Check if everyone chose actions.
    if (gBattleCommunication[ACTIONS_CONFIRMED_COUNT] == gBattlersCount)
    {
        gBattleMainFunc = SetActionsAndBanksTurnOrder;
    }
}

void SwapTurnOrder(u8 id1, u8 id2)
{
    int temp;

    temp = gActionsByTurnOrder[id1];
    gActionsByTurnOrder[id1] = gActionsByTurnOrder[id2];
    gActionsByTurnOrder[id2] = temp;

    temp = gBattlerByTurnOrder[id1];
    gBattlerByTurnOrder[id1] = gBattlerByTurnOrder[id2];
    gBattlerByTurnOrder[id2] = temp;
}

// Determines which of the two given mons will strike first in a battle.
// Returns:
// 0 = first mon moves first
// 1 = second mon moves first
// 2 = second mon moves first because it won a 50/50 roll
u8 GetWhoStrikesFirst(u8 battler1, u8 battler2, bool8 ignoreMovePriorities)
{
    int speedMultiplierBattler1, speedMultiplierBattler2;
    u32 bank1AdjustedSpeed, bank2AdjustedSpeed;
    u8 heldItemEffect;
    u8 heldItemEffectParam;
    u16 moveBattler1;
    u16 moveBattler2;
    u8 strikesFirst = 0;

    // Check for abilities that boost speed in weather.
    if (WEATHER_HAS_EFFECT)
    {
        if ((gBattleMons[battler1].ability == ABILITY_SWIFT_SWIM && (gBattleWeather & B_WEATHER_RAIN))
            || (gBattleMons[battler1].ability == ABILITY_CHLOROPHYLL && (gBattleWeather & B_WEATHER_SUN)))
            speedMultiplierBattler1 = 2;
        else
            speedMultiplierBattler1 = 1;

        if ((gBattleMons[battler2].ability == ABILITY_SWIFT_SWIM && (gBattleWeather & B_WEATHER_RAIN))
            || (gBattleMons[battler2].ability == ABILITY_CHLOROPHYLL && (gBattleWeather & B_WEATHER_SUN)))
            speedMultiplierBattler2 = 2;
        else
            speedMultiplierBattler2 = 1;
    }
    else
    {
        speedMultiplierBattler1 = 1;
        speedMultiplierBattler2 = 1;
    }

    // Calculate adjusted speed for first mon.
    bank1AdjustedSpeed = (gBattleMons[battler1].speed * speedMultiplierBattler1)
        * gStatStageRatios[gBattleMons[battler1].statStages[STAT_SPEED]][0] / gStatStageRatios[gBattleMons[battler1].statStages[STAT_SPEED]][1];

    if (gBattleMons[battler1].item == ITEM_ENIGMA_BERRY)
    {
        heldItemEffect = gEnigmaBerries[battler1].holdEffect;
        heldItemEffectParam = gEnigmaBerries[battler1].holdEffectParam;
    }
    else
    {
        heldItemEffect = GetItemHoldEffect(gBattleMons[battler1].item);
        heldItemEffectParam = GetItemHoldEffectParam(gBattleMons[battler1].item);
    }

    // Only give badge speed boost to the player's mon.
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK) && FlagGet(FLAG_BADGE03_GET) && GetBattlerSide(battler1) == 0)
        bank1AdjustedSpeed = (bank1AdjustedSpeed * 110) / 100;

    if (heldItemEffect == HOLD_EFFECT_MACHO_BRACE)
        bank1AdjustedSpeed /= 2;

    if (gBattleMons[battler1].status1 & STATUS1_PARALYSIS)
        bank1AdjustedSpeed /= 4;

    if (heldItemEffect == HOLD_EFFECT_QUICK_CLAW && gRandomTurnNumber < (heldItemEffectParam * 0xFFFF) / 100)
        bank1AdjustedSpeed = UINT_MAX;

    // Calculate adjusted speed for second mon.
    bank2AdjustedSpeed = gBattleMons[battler2].speed * speedMultiplierBattler2
        * gStatStageRatios[gBattleMons[battler2].statStages[STAT_SPEED]][0] / gStatStageRatios[gBattleMons[battler2].statStages[STAT_SPEED]][1];

    if (gBattleMons[battler2].item == ITEM_ENIGMA_BERRY)
    {
        heldItemEffect = gEnigmaBerries[battler2].holdEffect;
        heldItemEffectParam = gEnigmaBerries[battler2].holdEffectParam;
    }
    else
    {
        heldItemEffect = GetItemHoldEffect(gBattleMons[battler2].item);
        heldItemEffectParam = GetItemHoldEffectParam(gBattleMons[battler2].item);
    }

    // Only give badge speed boost to the player's mon.
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK) && FlagGet(FLAG_BADGE03_GET) && GetBattlerSide(battler2) == 0)
    {
        bank2AdjustedSpeed = (bank2AdjustedSpeed * 110) / 100;
    }

    if (heldItemEffect == HOLD_EFFECT_MACHO_BRACE)
        bank2AdjustedSpeed /= 2;

    if (gBattleMons[battler2].status1 & STATUS1_PARALYSIS)
        bank2AdjustedSpeed /= 4;

    if (heldItemEffect == HOLD_EFFECT_QUICK_CLAW && gRandomTurnNumber < (heldItemEffectParam * 0xFFFF) / 100)
        bank2AdjustedSpeed = UINT_MAX;

    if (ignoreMovePriorities)
    {
        moveBattler1 = MOVE_NONE;
        moveBattler2 = MOVE_NONE;
    }
    else
    {
        if (gChosenActionByBattler[battler1] == 0)
        {
            if (gProtectStructs[battler1].noValidMoves)
                moveBattler1 = MOVE_STRUGGLE;
            else
                moveBattler1 = gBattleMons[battler1].moves[gSharedMem[BSTRUCT_OFF(chosenMovePositions) + battler1]];
        }
        else
            moveBattler1 = MOVE_NONE;

        if (gChosenActionByBattler[battler2] == 0)
        {
            if (gProtectStructs[battler2].noValidMoves)
                moveBattler2 = MOVE_STRUGGLE;
            else
                moveBattler2 = gBattleMons[battler2].moves[gSharedMem[BSTRUCT_OFF(chosenMovePositions) + battler2]];
        }
        else
            moveBattler2 = MOVE_NONE;
    }

    if (gBattleMoves[moveBattler1].priority != 0 || gBattleMoves[moveBattler2].priority != 0)
    {
        if (gBattleMoves[moveBattler1].priority == gBattleMoves[moveBattler2].priority)
        {
            if (bank1AdjustedSpeed == bank2AdjustedSpeed && (Random() & 1))
                strikesFirst = 2;
            else if (bank1AdjustedSpeed < bank2AdjustedSpeed)
                strikesFirst = 1;
        }
        else if (gBattleMoves[moveBattler1].priority < gBattleMoves[moveBattler2].priority)
            strikesFirst = 1;
    }
    else
    {
        if (bank1AdjustedSpeed == bank2AdjustedSpeed && (Random() & 1))
            strikesFirst = 2;
        else if (bank1AdjustedSpeed < bank2AdjustedSpeed)
            strikesFirst = 1;
    }

    return strikesFirst;
}

void SetActionsAndBanksTurnOrder(void)
{
    s32 turnOrderId = 0;
    s32 i, j;

    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
    {
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[gActiveBattler];
            gBattlerByTurnOrder[turnOrderId] = gActiveBattler;
            turnOrderId++;
        }
    }
    else
    {
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_RUN)
                {
                    turnOrderId = 5;
                    break;
                }
            }
        }
        else
        {
            if (gChosenActionByBattler[0] == B_ACTION_RUN)
            {
                gActiveBattler = 0;
                turnOrderId = 5;
            }
        }

        if (turnOrderId == 5)
        {
            gActionsByTurnOrder[0] = gChosenActionByBattler[gActiveBattler];
            gBattlerByTurnOrder[0] = gActiveBattler;
            turnOrderId = 1;
            for (i = 0; i < gBattlersCount; i++)
            {
                if (i != gActiveBattler)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[i];
                    gBattlerByTurnOrder[turnOrderId] = i;
                    turnOrderId++;
                }
            }
            gBattleMainFunc = CheckFocusPunch_ClearVarsBeforeTurnStarts;
            gBattleStruct->focusPunchBattlerId = 0;
            return;
        }
        else
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_USE_ITEM || gChosenActionByBattler[gActiveBattler] == B_ACTION_SWITCH)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[gActiveBattler];
                    gBattlerByTurnOrder[turnOrderId] = gActiveBattler;
                    turnOrderId++;
                }
            }
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (gChosenActionByBattler[gActiveBattler] != B_ACTION_USE_ITEM && gChosenActionByBattler[gActiveBattler] != B_ACTION_SWITCH)
                {
                    gActionsByTurnOrder[turnOrderId] = gChosenActionByBattler[gActiveBattler];
                    gBattlerByTurnOrder[turnOrderId] = gActiveBattler;
                    turnOrderId++;
                }
            }
            for (i = 0; i < gBattlersCount - 1; i++)
            {
                for (j = i + 1; j < gBattlersCount; j++)
                {
                    u8 battler1 = gBattlerByTurnOrder[i];
                    u8 battler2 = gBattlerByTurnOrder[j];
                    if (gActionsByTurnOrder[i] != B_ACTION_USE_ITEM
                        && gActionsByTurnOrder[j] != B_ACTION_USE_ITEM
                        && gActionsByTurnOrder[i] != B_ACTION_SWITCH
                        && gActionsByTurnOrder[j] != B_ACTION_SWITCH)
                    {
                        if (GetWhoStrikesFirst(battler1, battler2, FALSE))
                            SwapTurnOrder(i, j);
                    }
                }
            }
        }
    }

    gBattleMainFunc = CheckFocusPunch_ClearVarsBeforeTurnStarts;
    gBattleStruct->focusPunchBattlerId = 0;
}

static void TurnValuesCleanUp(bool8 var0)
{
    s32 i;
    u8 *dataPtr;

    for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
    {
        if (var0)
        {
            gProtectStructs[gActiveBattler].protected = 0;
            gProtectStructs[gActiveBattler].endured = 0;
        }
        else
        {
            dataPtr = (u8*)(&gProtectStructs[gActiveBattler]);
            for (i = 0; i < sizeof(struct ProtectStruct); i++)
                dataPtr[i] = 0;

            if (gDisableStructs[gActiveBattler].isFirstTurn)
                gDisableStructs[gActiveBattler].isFirstTurn--;

            if (gDisableStructs[gActiveBattler].rechargeTimer)
            {
                gDisableStructs[gActiveBattler].rechargeTimer--;
                if (gDisableStructs[gActiveBattler].rechargeTimer == 0)
                    gBattleMons[gActiveBattler].status2 &= ~(STATUS2_RECHARGE);
            }
        }

        if (gDisableStructs[gActiveBattler].substituteHP == 0)
                gBattleMons[gActiveBattler].status2 &= ~(STATUS2_SUBSTITUTE);
    }

    gSideTimers[0].followmeTimer = 0;
    gSideTimers[1].followmeTimer = 0;
}

void SpecialStatusesClear(void)
{
    for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
    {
        s32 i;
        u8 *dataPtr = (u8*)(&gSpecialStatuses[gActiveBattler]);

        for (i = 0; i < sizeof(struct SpecialStatus); i++)
            dataPtr[i] = 0;
    }
}

static void CheckFocusPunch_ClearVarsBeforeTurnStarts(void)
{
    if (!(gHitMarker & HITMARKER_RUN))
    {
        while (gBattleStruct->focusPunchBattlerId < gBattlersCount)
        {
            gActiveBattler = gBattlerAttacker = gBattleStruct->focusPunchBattlerId;
            gBattleStruct->focusPunchBattlerId++;
            if (gChosenMoveByBattler[gActiveBattler] == MOVE_FOCUS_PUNCH
                && !(gBattleMons[gActiveBattler].status1 & STATUS1_SLEEP)
                && !(gDisableStructs[gBattlerAttacker].truantCounter)
                && !(gProtectStructs[gActiveBattler].noValidMoves))
            {
                BattleScriptExecute(BattleScript_FocusPunchSetUp);
                return;
            }
        }
    }

    TryClearRageStatuses();
    gCurrentTurnActionNumber = 0;
    {
        // something stupid needed to match
        u8 zero;
        gCurrentActionFuncId = gActionsByTurnOrder[(zero = 0)];
    }

    gDynamicBasePower = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleMainFunc = RunTurnActionsFunctions;
    gBattleCommunication[3] = 0;
    gBattleCommunication[4] = 0;
    gBattleStruct->multihitMoveEffect = 0;
    B_BATTLESCRIPTS_STACK->size = 0;
}

static void RunTurnActionsFunctions(void)
{
    if (gBattleOutcome != 0)
        gCurrentActionFuncId = 12;

    gBattleStruct->savedTurnActionNumber = gCurrentTurnActionNumber;
    sTurnActionsFuncsTable[gCurrentActionFuncId]();

    if (gCurrentTurnActionNumber >= gBattlersCount) // everyone did their actions, turn finished
    {
        gHitMarker &= ~(HITMARKER_PASSIVE_HP_UPDATE);
        gBattleMainFunc = sEndTurnFuncsTable[gBattleOutcome & 0x7F];
    }
    else
    {
        if (gBattleStruct->savedTurnActionNumber != gCurrentTurnActionNumber) // action turn has been done, clear hitmarker bits for another bank
        {
            gHitMarker &= ~(HITMARKER_NO_ATTACKSTRING);
            gHitMarker &= ~(HITMARKER_UNABLE_TO_USE_MOVE);
        }
    }
}

void HandleEndTurn_BattleWon(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gBattleTextBuff1[0] = gBattleOutcome;
        gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
        gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
    }
    else if (gBattleTypeFlags & (BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_EREADER_TRAINER))
    {
        gBattlescriptCurrInstr = gUnknown_081D8E0D;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        BattleStopLowHpSound();
        gBattlescriptCurrInstr = BattleScript_LocalTrainerBattleWon;

        switch (gTrainers[gTrainerBattleOpponent_A].trainerClass)
        {
        case TRAINER_CLASS_ELITE_FOUR:
        case TRAINER_CLASS_CHAMPION:
            PlayBGM(MUS_VICTORY_LEAGUE);
            break;
        case TRAINER_CLASS_TEAM_AQUA:
        case TRAINER_CLASS_TEAM_MAGMA:
        case TRAINER_CLASS_AQUA_ADMIN:
        case TRAINER_CLASS_AQUA_LEADER:
        case TRAINER_CLASS_MAGMA_ADMIN:
        case TRAINER_CLASS_MAGMA_LEADER:
            PlayBGM(MUS_VICTORY_AQUA_MAGMA);
            break;
        case TRAINER_CLASS_LEADER:
            PlayBGM(MUS_VICTORY_GYM_LEADER);
            break;
        default:
            PlayBGM(MUS_VICTORY_TRAINER);
            break;
        }
    }
    else
    {
        gBattlescriptCurrInstr = BattleScript_PayDayMoneyAndPickUpItems;
    }

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

void HandleEndTurn_BattleLost(void)
{
    gCurrentActionFuncId = 0;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gBattleTextBuff1[0] = gBattleOutcome;
        gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        gBattlescriptCurrInstr = BattleScript_LinkBattleWonOrLost;
        gBattleOutcome &= ~(B_OUTCOME_LINK_BATTLE_RAN);
    }
    else
    {
        gBattlescriptCurrInstr = BattleScript_LocalBattleLost;
    }

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

void HandleEndTurn_RanFromBattle(void)
{
    gCurrentActionFuncId = 0;

    switch (gProtectStructs[gBattlerAttacker].fleeType)
    {
    default:
        gBattlescriptCurrInstr = BattleScript_GotAwaySafely;
        break;
    case 1:
        gBattlescriptCurrInstr = BattleScript_SmokeBallEscape;
        break;
    case 2:
        gBattlescriptCurrInstr = BattleScript_RanAwayUsingMonAbility;
        break;
    }

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

void HandleEndTurn_MonFled(void)
{
    gCurrentActionFuncId = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker]);
    gBattlescriptCurrInstr = BattleScript_WildMonFled;

    gBattleMainFunc = HandleEndTurn_FinishBattle;
}

void HandleEndTurn_FinishBattle(void)
{
    if (gCurrentActionFuncId == 0xB || gCurrentActionFuncId == 0xC)
    {
        if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK
                                  | BATTLE_TYPE_FIRST_BATTLE
                                  | BATTLE_TYPE_SAFARI
                                  | BATTLE_TYPE_EREADER_TRAINER
                                  | BATTLE_TYPE_WALLY_TUTORIAL
                                  | BATTLE_TYPE_BATTLE_TOWER)))
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
                {
                    if (gBattleResults.playerMon1Species == SPECIES_NONE)
                    {
                        gBattleResults.playerMon1Species = gBattleMons[gActiveBattler].species;
                        StringCopy(gBattleResults.playerMon1Name, gBattleMons[gActiveBattler].nickname);
                    }
                    else
                    {
                        gBattleResults.playerMon2Species = gBattleMons[gActiveBattler].species;
                        StringCopy(gBattleResults.playerMon2Name, gBattleMons[gActiveBattler].nickname);
                    }
                }
            }
            TryPutPokemonTodayOnAir();
        }

        BeginFastPaletteFade(3);
        FadeOutMapMusic(5);
        gBattleMainFunc = FreeResetData_ReturnToOvOrDoEvolutions;
        gCB2_AfterEvolution = BattleMainCB2;
    }
    else
    {
        if (gBattleControllerExecFlags == 0)
            gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
    }
}

static void FreeResetData_ReturnToOvOrDoEvolutions(void)
{
    if (!gPaletteFade.active)
    {
        ResetSpriteData();
        if (gLeveledUpInBattle == 0 || gBattleOutcome != B_OUTCOME_WON)
        {
            gBattleMainFunc = ReturnFromBattleToOverworld;
            return;
        }
        else
        {
            gBattleMainFunc = TryEvolvePokemon;
        }
    }
}

static void TryEvolvePokemon(void)
{
    s32 i;

    while (gLeveledUpInBattle != 0)
    {
        for (i = 0; i < 6; i++)
        {
            if (gLeveledUpInBattle & gBitTable[i])
            {
                u16 species;
                u8 levelUpBits = gLeveledUpInBattle;

                levelUpBits &= ~(gBitTable[i]);
                gLeveledUpInBattle = levelUpBits;

                species = GetEvolutionTargetSpecies(&gPlayerParty[i], 0, levelUpBits);
                if (species != SPECIES_NONE)
                {
                    gBattleMainFunc = WaitForEvoSceneToFinish;
                    EvolutionScene(&gPlayerParty[i], species, 0x81, i);
                    return;
                }
            }
        }
    }

    gBattleMainFunc = ReturnFromBattleToOverworld;
}

static void WaitForEvoSceneToFinish(void)
{
    if (gMain.callback2 == BattleMainCB2)
        gBattleMainFunc = TryEvolvePokemon;
}

static void ReturnFromBattleToOverworld(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK))
    {
        RandomlyGivePartyPokerus(gPlayerParty);
        PartySpreadPokerus(gPlayerParty);
    }

    if (gBattleTypeFlags & BATTLE_TYPE_LINK && gReceivedRemoteLinkPlayers != 0)
        return;

    gSpecialVar_Result = gBattleOutcome;
    gMain.inBattle = 0;
    gMain.callback1 = gPreBattleCallback1;

    if (gBattleTypeFlags & BATTLE_TYPE_ROAMER)
    {
        UpdateRoamerHPStatus(&gEnemyParty[0]);
        if (gBattleOutcome == B_OUTCOME_WON || gBattleOutcome == B_OUTCOME_CAUGHT)
            SetRoamerInactive();
    }

    m4aSongNumStop(0x5A);
    SetMainCallback2(gMain.savedCallback);
}

void RunBattleScriptCommands_PopCallbacksStack(void)
{
    if (gCurrentActionFuncId == 0xB || gCurrentActionFuncId == 0xC)
    {
        if (B_FUNCTION_STACK->size != 0)
            B_FUNCTION_STACK->size--;
        gBattleMainFunc = B_FUNCTION_STACK->function[B_FUNCTION_STACK->size];
    }
    else
    {
        if (gBattleControllerExecFlags == 0)
            gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
    }
}

void RunBattleScriptCommands(void)
{
    if (gBattleControllerExecFlags == 0)
        gBattleScriptingCommandsTable[gBattlescriptCurrInstr[0]]();
}

void HandleAction_UseMove(void)
{
    u8 side;
    u8 var = 4;

    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];

    if (gBattleStruct->absentBattlerFlags & gBitTable[gBattlerAttacker])
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }

    gCritMultiplier = 1;
    gBattleStruct->dmgMultiplier = 1;
    gBattleStruct->atkCancelerTracker = 0;
    gMoveResultFlags = 0;
    gMultiHitCounter = 0;
    gBattleCommunication[6] = 0;
    gCurrMovePos = gChosenMovePos = gSharedMem[BSTRUCT_OFF(chosenMovePositions) + gBattlerAttacker];

    // choose move
    if (gProtectStructs[gBattlerAttacker].noValidMoves)
    {
        gProtectStructs[gBattlerAttacker].noValidMoves = 0;
        gCurrentMove = gChosenMove = MOVE_STRUGGLE;
        gHitMarker |= HITMARKER_NO_PPDEDUCT;
        gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker] = GetMoveTarget(MOVE_STRUGGLE, 0);
    }
    else if (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS || gBattleMons[gBattlerAttacker].status2 & STATUS2_RECHARGE)
    {
        gCurrentMove = gChosenMove = gLockedMoves[gBattlerAttacker];
    }
    // encore forces you to use the same move
    else if (gDisableStructs[gBattlerAttacker].encoredMove != MOVE_NONE
             && gDisableStructs[gBattlerAttacker].encoredMove == gBattleMons[gBattlerAttacker].moves[gDisableStructs[gBattlerAttacker].encoredMovePos])
    {
        gCurrentMove = gChosenMove = gDisableStructs[gBattlerAttacker].encoredMove;
        gCurrMovePos = gChosenMovePos = gDisableStructs[gBattlerAttacker].encoredMovePos;
        gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker] = GetMoveTarget(gCurrentMove, 0);
    }
    // check if the encored move wasn't overwritten
    else if (gDisableStructs[gBattlerAttacker].encoredMove != MOVE_NONE
             && gDisableStructs[gBattlerAttacker].encoredMove != gBattleMons[gBattlerAttacker].moves[gDisableStructs[gBattlerAttacker].encoredMovePos])
    {
        gCurrMovePos = gChosenMovePos = gDisableStructs[gBattlerAttacker].encoredMovePos;
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        gDisableStructs[gBattlerAttacker].encoredMove = MOVE_NONE;
        gDisableStructs[gBattlerAttacker].encoredMovePos = 0;
        gDisableStructs[gBattlerAttacker].encoreTimer = 0;
        gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker] = GetMoveTarget(gCurrentMove, 0);
    }
    else if (gBattleMons[gBattlerAttacker].moves[gCurrMovePos] != gChosenMoveByBattler[gBattlerAttacker])
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker] = GetMoveTarget(gCurrentMove, 0);
    }
    else
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
    }

    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        gBattleResults.lastUsedMovePlayer = gCurrentMove;
    else
        gBattleResults.lastUsedMoveOpponent = gCurrentMove;

    // choose target
    side = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
    if (gSideTimers[side].followmeTimer != 0
        && gBattleMoves[gCurrentMove].target == MOVE_TARGET_SELECTED
        && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gSideTimers[side].followmeTarget)
        && gBattleMons[gSideTimers[side].followmeTarget].hp != 0)
    {
        gBattlerTarget = gSideTimers[side].followmeTarget;
    }
    else if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
             && gSideTimers[side].followmeTimer == 0
             && (gBattleMoves[gCurrentMove].power != 0
                 || gBattleMoves[gCurrentMove].target != MOVE_TARGET_USER)
             && gBattleMons[gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker]].ability != ABILITY_LIGHTNING_ROD
             && gBattleMoves[gCurrentMove].type == TYPE_ELECTRIC)
    {
        side = GetBattlerSide(gBattlerAttacker);
        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if (side != GetBattlerSide(gActiveBattler)
                && gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker] != gActiveBattler
                && gBattleMons[gActiveBattler].ability == ABILITY_LIGHTNING_ROD
                && GetBattlerTurnOrderNum(gActiveBattler) < var)
            {
                var = GetBattlerTurnOrderNum(gActiveBattler);
            }
        }
        if (var == 4)
        {
            if (gBattleMoves[gChosenMove].target & MOVE_TARGET_RANDOM)
            {
                if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                {
                    if (Random() & 1)
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
                    else
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
                }
                else
                {
                    if (Random() & 1)
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
                    else
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
                }
            }
            else
            {
                gBattlerTarget = gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker];
            }

            if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
            {
                if (GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
                {
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
                }
                else
                {
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_SIDE);
                    if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
                        gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
                }
            }
        }
        else
        {
            gActiveBattler = gBattlerByTurnOrder[var];
            RecordAbilityBattle(gActiveBattler, gBattleMons[gActiveBattler].ability);
            gSpecialStatuses[gActiveBattler].lightningRodRedirected = 1;
            gBattlerTarget = gActiveBattler;
        }
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE
             && gBattleMoves[gChosenMove].target & MOVE_TARGET_RANDOM)
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        {
            if (Random() & 1)
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            else
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        }
        else
        {
            if (Random() & 1)
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            else
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        }

        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget]
            && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
        {
            gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
        }
    }
    else
    {
        gBattlerTarget = gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerAttacker];
        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
        {
            if (GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
            {
                gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
            }
            else
            {
                gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_SIDE);
                if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
            }
        }
    }

    gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_Switch(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gActionSelectionCursor[gBattlerAttacker] = 0;
    gMoveSelectionCursor[gBattlerAttacker] = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gSharedMem[BSTRUCT_OFF(battlerPartyIndexes) + gBattlerAttacker])

    gBattleStruct->battler = gBattlerAttacker;
    gBattlescriptCurrInstr = BattleScript_ActionSwitch;
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;

    if (gBattleResults.totalMonSwitchCounter < 255)
        gBattleResults.totalMonSwitchCounter++;
}

void HandleAction_UseItem(void)
{
    gBattlerAttacker = gBattlerTarget = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gDisableStructs[gBattlerAttacker].furyCutterCounter = 0;
    gLastUsedItem = gBattleBufferB[gBattlerAttacker][1] | (gBattleBufferB[gBattlerAttacker][2] << 8);

    if (gLastUsedItem <= ITEM_PREMIER_BALL) // is ball
    {
        gBattlescriptCurrInstr = gBattlescriptsForBallThrow[gLastUsedItem];
    }
    else if (gLastUsedItem == ITEM_POKE_DOLL || gLastUsedItem == ITEM_FLUFFY_TAIL)
    {
        gBattlescriptCurrInstr = gBattlescriptsForRunningByItem[0];
    }
    else if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
    {
        gBattlescriptCurrInstr = gBattlescriptsForUsingItem[0];
    }
    else
    {

        switch (gSharedMem[BSTRUCT_OFF(AI_itemType) + ((gBattleStruct->battler = gBattlerAttacker) / 2)])
        {
        case AI_ITEM_FULL_RESTORE:
        case AI_ITEM_HEAL_HP:
            break;
        case AI_ITEM_CURE_CONDITION:
            gBattleCommunication[MULTISTRING_CHOOSER] = 0;
            if (gSharedMem[BSTRUCT_OFF(AI_itemFlags) + (gBattlerAttacker >> 1)] & 1)
            {
                if (gSharedMem[BSTRUCT_OFF(AI_itemFlags) + (gBattlerAttacker >> 1)] & 0x3E)
                    gBattleCommunication[MULTISTRING_CHOOSER] = 5;
            }
            else
            {
                while (!(gSharedMem[BSTRUCT_OFF(AI_itemFlags) + (gBattlerAttacker >> 1)] & 1))
                {
                    gSharedMem[BSTRUCT_OFF(AI_itemFlags) + (gBattlerAttacker >> 1)] >>= 1;
                    gBattleCommunication[MULTISTRING_CHOOSER]++;
                }
            }
            break;
        case AI_ITEM_X_STAT:
            gBattleCommunication[MULTISTRING_CHOOSER] = 4;
            if (gSharedMem[BSTRUCT_OFF(AI_itemFlags) + (gBattlerAttacker >> 1)] & 0x80)
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = 5;
            }
            else
            {
                PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_ATK)
                PREPARE_STRING_BUFFER(gBattleTextBuff2, STRINGID_STATROSE)

                while (!(gSharedMem[BSTRUCT_OFF(AI_itemFlags) + (gBattlerAttacker >> 1)] & 1))
                {
                    gSharedMem[BSTRUCT_OFF(AI_itemFlags) + (gBattlerAttacker >> 1)] >>= 1;
                    gBattleTextBuff1[2]++;
                }

                gBattleStruct->animArg1 = gBattleTextBuff1[2] + 14;
                gBattleStruct->animArg2 = 0;
            }
            break;
        case AI_ITEM_GUARD_SPEC:
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                gBattleCommunication[MULTISTRING_CHOOSER] = 2;
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = 0;
            break;
        }

        gBattlescriptCurrInstr = gBattlescriptsForUsingItem[gSharedMem[BSTRUCT_OFF(AI_itemType) + (gBattlerAttacker / 2)]];
    }
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

bool8 TryRunFromBattle(u8 battler)
{
    bool8 effect = FALSE;
    u8 holdEffect;
    u8 speedVar;

    if (gBattleMons[battler].item == ITEM_ENIGMA_BERRY)
        holdEffect = gEnigmaBerries[battler].holdEffect;
    else
        holdEffect = GetItemHoldEffect(gBattleMons[battler].item);

    gPotentialItemEffectBattler = battler;

    if (holdEffect == HOLD_EFFECT_CAN_ALWAYS_RUN)
    {
        gLastUsedItem = gBattleMons[battler].item ;
        gProtectStructs[battler].fleeType = 1;
        effect++;
    }
    else if (gBattleMons[battler].ability == ABILITY_RUN_AWAY)
    {
        gLastUsedAbility = ABILITY_RUN_AWAY;
        gProtectStructs[battler].fleeType = 2;
        effect++;
    }
    else
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        {
            if (gBattleMons[battler].speed < gBattleMons[battler ^ BIT_SIDE].speed)
            {
                speedVar = (gBattleMons[battler].speed * 128) / (gBattleMons[battler ^ BIT_SIDE].speed) + (gBattleStruct->runTries * 30);
                if (speedVar > (Random() & 0xFF))
                    effect++;
            }
            else // same speed or faster
            {
                effect++;
            }
        }

        gBattleStruct->runTries++;
    }

    if (effect)
    {
        gCurrentTurnActionNumber = gBattlersCount;
        gBattleOutcome = B_OUTCOME_RAN;
    }

    return effect;
}

void HandleAction_Run(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gCurrentTurnActionNumber = gBattlersCount;

        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_LOST;
            }
            else
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_WON;
            }
        }

        gBattleOutcome |= B_OUTCOME_LINK_BATTLE_RAN;
    }
    else
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        {
            if (!TryRunFromBattle(gBattlerAttacker)) // failed to run away
            {
                gBattleMons[gBattlerAttacker].status2 &= ~STATUS2_DESTINY_BOND;
                gBattleCommunication[MULTISTRING_CHOOSER] = 3;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
        }
        else
        {
            if (gBattleMons[gBattlerAttacker].status2 & (STATUS2_WRAPPED | STATUS2_ESCAPE_PREVENTION))
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = 4;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
            else
            {
                gCurrentTurnActionNumber = gBattlersCount;
                gBattleOutcome = B_OUTCOME_MON_FLED;
            }
        }
    }
}

void HandleAction_WatchesCarefully(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[0];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_SafariZoneBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gNumSafariBalls--;
    gLastUsedItem = ITEM_SAFARI_BALL;
    gBattlescriptCurrInstr = gBattlescriptsForBallThrow[ITEM_SAFARI_BALL];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_ThrowPokeblock(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattleCommunication[MULTISTRING_CHOOSER] = gBattleBufferB[gBattlerAttacker][1] - 1;
    gLastUsedItem = gBattleBufferB[gBattlerAttacker][2];

    if (gBattleStruct->safariPkblThrowCounter < 3)
        gBattleStruct->safariPkblThrowCounter++;
    if (gBattleStruct->safariEscapeFactor > 1)
    {
        if (gBattleStruct->safariEscapeFactor < sPkblToEscapeFactor[gBattleStruct->safariPkblThrowCounter][gBattleCommunication[MULTISTRING_CHOOSER]])
            gBattleStruct->safariEscapeFactor = 1;
        else
            gBattleStruct->safariEscapeFactor -= sPkblToEscapeFactor[gBattleStruct->safariPkblThrowCounter][gBattleCommunication[MULTISTRING_CHOOSER]];
    }

    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[2];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_GoNear(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    gBattleStruct->safariCatchFactor += sGoNearCounterToCatchFactor[gBattleStruct->safariGoNearCounter];
    if (gBattleStruct->safariCatchFactor > 20)
        gBattleStruct->safariCatchFactor = 20;

    gBattleStruct->safariEscapeFactor += sGoNearCounterToEscapeFactor[gBattleStruct->safariGoNearCounter];
    if (gBattleStruct->safariEscapeFactor > 20)
        gBattleStruct->safariEscapeFactor = 20;

    if (gBattleStruct->safariGoNearCounter < 3)
    {
        gBattleStruct->safariGoNearCounter++;
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
    }
    else
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
    }
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[1];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_SafariZoneRun(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    PlaySE(SE_FLEE);
    gCurrentTurnActionNumber = gBattlersCount;
    gBattleOutcome = B_OUTCOME_RAN;
}

void HandleAction_WallyBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker])

    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[3];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
    gActionsByTurnOrder[1] = B_ACTION_FINISHED;
}

void HandleAction_TryFinish(void)
{
    if (!HandleFaintedMonActions())
    {
        gBattleStruct->faintedActionsState = 0;
        gCurrentActionFuncId = B_ACTION_FINISHED;
    }
}

void HandleAction_NothingIsFainted(void)
{
    gCurrentTurnActionNumber++;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
    gHitMarker &= ~(HITMARKER_DESTINYBOND | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_ATTACKSTRING_PRINTED
                    | HITMARKER_NO_PPDEDUCT | HITMARKER_STATUS_ABILITY_EFFECT | HITMARKER_IGNORE_ON_AIR
                    | HITMARKER_IGNORE_UNDERGROUND | HITMARKER_IGNORE_UNDERWATER | HITMARKER_PASSIVE_HP_UPDATE
                    | HITMARKER_OBEYS | HITMARKER_WAKE_UP_CLEAR | HITMARKER_SYNCHRONIZE_EFFECT
                    | HITMARKER_CHARGING | HITMARKER_NEVER_SET);
}

void HandleAction_ActionFinished(void)
{
    gCurrentTurnActionNumber++;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
    SpecialStatusesClear();
    gHitMarker &= ~(HITMARKER_DESTINYBOND | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_ATTACKSTRING_PRINTED
                    | HITMARKER_NO_PPDEDUCT | HITMARKER_STATUS_ABILITY_EFFECT | HITMARKER_IGNORE_ON_AIR
                    | HITMARKER_IGNORE_UNDERGROUND | HITMARKER_IGNORE_UNDERWATER | HITMARKER_PASSIVE_HP_UPDATE
                    | HITMARKER_OBEYS | HITMARKER_WAKE_UP_CLEAR | HITMARKER_SYNCHRONIZE_EFFECT
                    | HITMARKER_CHARGING | HITMARKER_NEVER_SET);

    gBattleMoveDamage = 0;
    gBattleStruct->animTurn = 0;
    gBattleStruct->animTargetsHit = 0;
    gLastLandedMoves[gBattlerAttacker] = 0;
    gLastHitByType[gBattlerAttacker] = 0;
    gBattleStruct->dynamicMoveType = 0;
    gDynamicBasePower = 0;
    gBattleStruct->moveendState = 0;
    gBattleCommunication[3] = 0;
    gBattleCommunication[4] = 0;
    gBattleStruct->multihitMoveEffect = 0;
    B_BATTLESCRIPTS_STACK->size = 0;
}
