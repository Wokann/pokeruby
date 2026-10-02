#ifndef GUARD_EWRAM_H
#define GUARD_EWRAM_H

extern u8 gSharedMem[];

// Note: we have duplicate ewram labels because its clear some memory
// regions overlap others but have different definitions. Until some
// dupes can be determined to be the same, they will be deliberate
// seperated.
#define gDecompressionBuffer            (gSharedMem + 0x1E000)

// battle_interface.c
#define sBattleInterfaceGfxBuffer       ((u8 *)EWRAM)

// region_map.c, field_region_map.c
#define sRegionMapState                 (*(struct RegionMapState *)gSharedMem)

// field_weather.c
#define sDroughtPaletteData             (*(struct WeatherPaletteData *)gSharedMem)

// battle etc.
// This overlaps with so many things it's not even funny
// The actual data starts at 0x02014000
#define sBattleMultiBuffer              (*(union MultiBuffers *)gSharedMem)
#define gBattleStruct                   ((struct BattleStruct *)     (gSharedMem + 0x0))
// This macro ensures that any changes to the battle struct
// definition will not horribly break things in battle.
#define BSTRUCT_OFF(member)             (offsetof(struct BattleStruct, member) + ((u8 *)gBattleStruct - gSharedMem))
#define BCOMM(buff, member, blockOff, i)  (gSharedMem[BSTRUCT_OFF(buff) + offsetof(struct LinkBattleCommunicationHeader, member) + blockOff + i])

// battle_transition.c
#define sMugshotDecompBuffer            (&gSharedMem[0xC03C])

#define sCastformPalette                (gSharedMem + 0x16400)

// Fixed Ruby equivalents of gBattleResources fields.
#define AI_THINKING_STRUCT                ((struct AI_ThinkingStruct *)(gSharedMem + 0x16800))
#define BATTLE_HISTORY                    ((struct BattleHistory *) (gSharedMem + 0x16A00))
#define sAIScriptsStack                   ((struct BattleScriptsStack *)(gSharedMem + 0x16C00))
#define sSecretBase                       ((struct SecretBaseRecord *)(gSharedMem + 0x17000))
#define sBattleResourceFlags              (*(struct ResourceFlags *)(gSharedMem + 0x17100))
#define sBattleScriptsStack               ((struct BattleScriptsStack *)(gSharedMem + 0x17110))
#define sBattleCallbackStack              ((struct BattleCallbacksStack *)(gSharedMem + 0x17140))
#define sStatsBeforeLvlUp                 ((struct StatsArray *)(gSharedMem + 0x17180))
#define gBattleSpriteInfo                 ((struct BattleSpriteInfo *)    (gSharedMem + 0x17800))
#define gBattleHealthBoxInfo              ((struct BattleHealthboxInfo *)    (gSharedMem + 0x17810))
#define gBattleAnimationInfo               (*(struct BattleAnimationInfo *)(gSharedMem + 0x17840))
#define sBattleBars                       ((struct BattleBar *)(gSharedMem + 0x17850))

#define eBattleAnimPalBackup_Battle     ((u16 *)(gSharedMem + 0x18000)) // also: shop.c, battle_anim.c

// Contest
#define eBattleAnimPalBackup_Contest    ((u16 *)(gSharedMem + 0x14800))
#define eUnzippedContestAudience_Gfx    (&gSharedMem[0x15800]) // overlaps other contest vars
#define gCurContestWinnerSaveIdx         (gSharedMem[0x15DDE])
#define gCurContestWinnerIsForArtist     (gSharedMem[0x15DDF])
#define gCurContestWinner               (*(struct ContestWinner *)(gSharedMem + 0x15DE0))
#define eContestPaintingBuffers         ((*(struct ContestPaintingBuffers*)(gSharedMem + 0x15E00)))
#define gEnableContestDebugging         (gSharedMem[0x18000])
#define eContestTempSave                (*(struct ContestTempSave *)(gSharedMem + 0x18004))
#define eContest                        (*(struct Contest *)(gSharedMem + 0x19204))
#define eContestantStatus               ((struct ContestantStatus *)(gSharedMem + 0x19260))
#define eContestAppealResults           (*(struct ContestAppealMoveResults *)(gSharedMem + 0x192D0))
#define eContestAI                      ((struct ContestAIInfo *)(gSharedMem + 0x192E4))
#define eContestExcitement              (*(struct ContestExcitement *)(gSharedMem + 0x19328))
#define eContestGfxState                ((struct ContestGraphicsState *)(gSharedMem + 0x19338))
#define gContestResources__moveAnim     (*(struct ContestMoveAnim *)(gSharedMem + 0x19348))

// Non-battle allocations
#define namingScreenData                (*(struct NamingScreenData *)(gSharedMem + 0x0))
#define gSaveDataBuffer                 (struct SaveSector *)(gSharedMem + 0x0)
#define eSlotMachine                    ((struct SlotMachine *)(gSharedMem + 0x0))
#define sPokemonStorageSystem           (*(struct PokemonStorageSystemData *)(gSharedMem + 0))

#define sTempWallyBagItems              ((struct ItemSlot *)(gSharedMem + 0x1E000))
#define sTempWallyBagPokeBalls          ((struct ItemSlot *)(gSharedMem + 0x1F000))

// party_menu.c
#define sPartyMenuTwoMonAction        (*(struct PartyMenuTwoMonActionState *)(gSharedMem + 0x1000))
#define sMoveRelearnerMenuSharedMem     (struct MoveRelearnerMenu *)(gSharedMem + 0x17000)
#define gPartyMenu                      (*(struct PartyMenu *)(gSharedMem + 0x1B000))
#define sPartyMenuAction                (*(struct PartyMenuActionState *)(gSharedMem + 0x1C000))

// player_pc.c, party_menu.c
// Ruby's legacy menu stores this four-byte mailbox paging subset in fixed EWRAM.
#define gPlayerPCItemPageInfo           (*(struct PlayerPCItemPageStruct *)(gSharedMem + 0x1FE00))

// item_menu.c
#define sBerryPicGfxBuffer              (gSharedMem + 0x0)
#define sBerryPicGfxDecompBuffer        (gSharedMem + 0x1000)

// record_mixing.c
#define sRecordMixTvShows               ((union TVShow (*)[TV_SHOWS_COUNT])(gSharedMem + 0x7000))
#define sRecordMixPokeNews              ((struct PokeNews (*)[POKE_NEWS_COUNT])(gSharedMem + 0x7000)) // same addr as above
#define sLinkedDewfordTrendsBuffer      ((struct DewfordTrend *)(gSharedMem + 0x7800))
#define sSavedDewfordTrendsBuffer       ((struct DewfordTrend *)(gSharedMem + 0x7900))
#define sReceivedRecords                (*(struct PlayerRecords *)(gSharedMem + 0x08000))
#define sSentRecord                     (*(struct PlayerRecords *)(gSharedMem + 0x18000))

// pokedex.c
#define ePokedexGraphicsBuffer          ((void *)&gSharedMem[0x8000])
#define ePokedexView1                   (struct PokedexView *)(gSharedMem + 0x18000)
#define ePokedexView2                   (struct PokedexView *)(gSharedMem + 0x18800)
#define ePokedexView3                   (struct PokedexView *)(gSharedMem + 0x19000)
#define ePokedexView4                   (struct PokedexView *)(gSharedMem + 0x19800)

// pokedex_cry_screen.c
#define sCryWaveformWindowTiledata      (&gSharedMem[0])
#define ePokedexCryScreen               (*(struct PokedexCryScreen *)(gSharedMem + 0x1C000))
#define ePokedexCryMeterNeedle          (*(struct PokedexCryMeterNeedle *)(gSharedMem + 0x1C800))

#define sTradeSharedMem                  (*(struct TradeSharedMem *)(gSharedMem + 0x10000))
#define sBerryBlenderGfxBuffer          (&gSharedMem[0x10000])
#define sSlotMachineGfxBuffer           (void *)(gSharedMem + 0x10000) // slot machine
#define sSlotMachineReelTimeGfxBuffer   (&gSharedMem[0x13200])
#define sEvoInfo                        ((*(struct EvoInfo*)(gSharedMem + 0x14800)))

// roulette_gfx.c
#define sRouletteShroomishTaillowGfxBuffer (gSharedMem + 0x17000)
#define sRouletteBallGfxBuffer          (gSharedMem + 0x17E00)
#define ewram18000                      ((u16 *)(gSharedMem + 0x18000)) // also: shop.c, battle_anim.c

// cable_car.c
#define sCableCarSharedMem               ((struct CableCar *)(gSharedMem + 0x17000))
#define sCableCarTilemapData            ((struct CableCarTilemapData *)(gSharedMem + 0x19000))

#define sBerryBlenderSharedMem          ((struct BerryBlender *)(&gSharedMem[0x18000]))
#define sEggHatchSharedMem              ((struct EggHatchData *)(&gSharedMem[0x18000]))

// pokemon_summary_screen.c
#define sMonSummaryScreen               ((struct PokemonSummaryScreenData *)(gSharedMem + 0x18000))

// roulette.c
#define sRouletteGridTilemap            ((u16 *)(gSharedMem + 0x18800))
#define sRoulette                       ((struct Roulette *)(gSharedMem + 0x19000))

#define sCreditsData                    (*(struct CreditsData *)(gSharedMem + 0x1C000))

// hall_of_fame.c
#define sHofGfxPtr                      (gSharedMem + 0x8000)
#define sHofMonPtr                      ((struct HallofFameTeam *)(&gSharedMem[0x1C000]))
#define sHofPCScreenEffect              (*(struct PCScreenEffectStruct *)(gSharedMem + 0x1c000))

#define sMultiPartnerPartyBuffer \
    ((struct Pokemon *)(gSharedMem + 0x1D000))

// pokeblock_feed.c
#define sPokeblockFeedMonAnimX          ((s16 *)(gSharedMem + 0x1D000))
#define sPokeblockFeedMonAnimY          ((s16 *)(gSharedMem + 0x1D400))

// decoration.c
#define sDecorationContext              (*(struct DecorationPCContext *)(gSharedMem + 0x1f000))

#define sCutGrassSpriteArrayPtr        (gSharedMem + 0x1FFF0) // 8 sprite IDs

// reshow_battle_screen.c
#define gReshowBattleScreenHelperState  (gSharedMem[0x1FFFE])
#define gReshowBattleScreenMainState    (gSharedMem[0x1FFFF])

// item_menu.c
#define sBagMenuGraphicsLoadState       (gSharedMem[0x1FFFF])

// pokeblock_feed.c
#define sPokeblockFeedPokeblockSpriteId (gSharedMem[0x1FFFC])
#define sPokeblockFeedCaseSpriteId      (gSharedMem[0x1FFFD])
#define sPokeblockFeedMonSpriteId       (gSharedMem[0x1FFFE])

// pokeblock.c, pokeblock_feed.c
#define sPokeblockMenuCaseSpriteId      (gSharedMem[0x1FFFE])
#define sPokeblockLoadGfxState          (gSharedMem[0x1FFFF])

#endif
