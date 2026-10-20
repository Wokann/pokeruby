#include "global.h"
#include "constants/rgb.h"
#include "constants/battle.h"
#include "constants/region_map_sections.h"
#include "battle_anim.h"
#include "international_string_util.h"
#include "string_util.h"
#include "text.h"
#include "main.h"
#include "constants/species.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/easy_chat.h"
#include "link.h"
#include "strings2.h"
#include "graphics.h"
#include "palette.h"
#include "task.h"
#include "menu.h"
#include "text_window.h"
#include "pokemon_icon.h"
#include "cable_club.h"
#include "party_menu.h"
#include "constants/songs.h"
#include "sound.h"
#include "data.h"
#include "pokemon_summary_screen.h"
#include "overworld.h"
#include "daycare.h"
#include "event_data.h"
#include "strings.h"
#include "load_save.h"
#include "save.h"
#include "script.h"
#include "field_screen_effect.h"
#include "decompress.h"
#include "mail.h"
#include "evolution_scene.h"
#include "pokeball.h"
#include "pokedex.h"
#include "field_effect.h"
#include "util.h"
#include "battle_interface.h"
#include "trade.h"
#include "ewram.h"
#ifndef NONMATCHING
asm(".include \"constants/gba_constants.inc\"");
#endif

#ifdef ENGLISH
#define sub_804A96C_alt sub_804A96C
asm(".set sub_804A96C_alt, sub_804A96C");
#endif

// IDs for RunTradeMenuCallback
enum {
    CB_MAIN_MENU,
    CB_SELECTED_MON,
    CB_SHOW_MON_SUMMARY,
    CB_CONFIRM_TRADE_PROMPT,
    CB_CANCEL_TRADE_PROMPT,
    CB_READY_WAIT,
    CB_SET_SELECTED_MONS,
    CB_PRINT_IS_THIS_OKAY,
    CB_HANDLE_TRADE_CANCELED,
    CB_FADE_TO_START_TRADE,
    CB_WAIT_TO_START_TRADE,
    CB_INIT_EXIT_CANCELED_TRADE,
    CB_EXIT_CANCELED_TRADE,
    CB_START_LINK_TRADE,
    CB_INIT_CONFIRM_TRADE_PROMPT,
    CB_UNUSED_CLOSE_MSG,
    CB_IDLE = 100,
};

// Indexes for sActionTexts
enum {
    TEXT_CANCEL,
    TEXT_CHOOSE_MON,
    TEXT_SUMMARY,
    TEXT_TRADE,
    TEXT_CANCEL_TRADE,
};

// Indexes for sMessages; native message 3 is the partner-cannot-trade text.
enum {
    MSG_STANDBY = 0,
    MSG_CANCELED = 1,
    MSG_ONLY_MON1 = 2,
    MSG_WAITING_FOR_FRIEND = 4,
    MSG_FRIEND_WANTS_TO_TRADE = 5,
};

// IDs for QueueAction
enum {
    QUEUE_SEND_DATA,
    QUEUE_STANDBY,
    QUEUE_ONLY_MON1,
};

#define QUEUE_DELAY_MSG   3
#define QUEUE_DELAY_DATA  5

// Offsets from GFXTAG_MENU_TEXT and indexes into sMenuTextTileBuffers.
// Ruby has five Choose-a-Pokemon sheets, not Emerald's six.
enum {
    GFXTAG_PLAYER_NAME_L,
    GFXTAG_PLAYER_NAME_M,
    GFXTAG_PLAYER_NAME_R,
    GFXTAG_PARTNER_NAME_L,
    GFXTAG_PARTNER_NAME_M,
    GFXTAG_PARTNER_NAME_R,
    GFXTAG_CANCEL_L,
    GFXTAG_CANCEL_R,
    GFXTAG_CHOOSE_PKMN_L,
    GFXTAG_CHOOSE_PKMN_M,
    GFXTAG_CHOOSE_PKMN_R,
    GFXTAG_CHOOSE_PKMN_EMPTY_1,
    GFXTAG_CHOOSE_PKMN_EMPTY_2,
    NUM_MENU_TEXT_SPRITES
};
#define NUM_PLAYER_NAME_SPRITES (1 + GFXTAG_PLAYER_NAME_R - GFXTAG_PLAYER_NAME_L)
#define NUM_PARTNER_NAME_SPRITES (1 + GFXTAG_PARTNER_NAME_R - GFXTAG_PARTNER_NAME_L)
#define NUM_CHOOSE_PKMN_SPRITES (1 + GFXTAG_CHOOSE_PKMN_EMPTY_2 - GFXTAG_CHOOSE_PKMN_L)

// Values for signaling to/from the link partner
enum {
    STATUS_NONE,
    STATUS_READY,
    STATUS_CANCEL,
};

#define DRAW_SELECTED_FINISH 5

enum {
    STATE_START,
    STATE_MON_SLIDE_IN,
    // 2-9 unused
    STATE_SEND_MSG = 10,
    STATE_BYE_BYE,
    STATE_POKEBALL_DEPART,
    STATE_POKEBALL_DEPART_WAIT,
    STATE_FADE_OUT_TO_GBA_SEND,
    // 15-19 unused
    STATE_WAIT_FADE_OUT_TO_GBA_SEND = 20,
    STATE_FADE_IN_TO_GBA_SEND,
    STATE_WAIT_FADE_IN_TO_GBA_SEND,
    STATE_GBA_ZOOM_OUT,
    STATE_GBA_FLASH_SEND,
    STATE_GBA_STOP_FLASH_SEND,
    STATE_PAN_AWAY_GBA,
    STATE_CREATE_LINK_MON_LEAVING,
    STATE_LINK_MON_TRAVEL_OUT,
    STATE_FADE_OUT_TO_CROSSING,
    STATE_WAIT_FADE_OUT_TO_CROSSING,
    STATE_FADE_IN_TO_CROSSING,
    STATE_WAIT_FADE_IN_TO_CROSSING,
    STATE_CROSSING_LINK_MONS_ENTER,
    STATE_CROSSING_BLEND_WHITE_1,
    STATE_CROSSING_BLEND_WHITE_2,
    STATE_CROSSING_BLEND_WHITE_3,
    STATE_CROSSING_CREATE_MON_PICS,
    STATE_CROSSING_MON_PICS_MOVE,
    STATE_CROSSING_LINK_MONS_EXIT,
    STATE_CREATE_LINK_MON_ARRIVING,
    STATE_FADE_OUT_TO_GBA_RECV,
    STATE_WAIT_FADE_OUT_TO_GBA_RECV,
    STATE_LINK_MON_TRAVEL_IN,
    STATE_PAN_TO_GBA,
    STATE_DESTROY_LINK_MON,
    STATE_LINK_MON_ARRIVED_DELAY,
    STATE_MOVE_GBA_TO_CENTER,
    STATE_GBA_FLASH_RECV,
    STATE_UNUSED,
    STATE_GBA_STOP_FLASH_RECV,
    STATE_GBA_ZOOM_IN,
    STATE_FADE_OUT_TO_NEW_MON,
    // 53-59 unused
    STATE_WAIT_FADE_OUT_TO_NEW_MON = 60,
    STATE_FADE_IN_TO_NEW_MON,
    STATE_WAIT_FADE_IN_TO_NEW_MON,
    STATE_POKEBALL_ARRIVE,
    STATE_FADE_POKEBALL_TO_NORMAL,
    STATE_POKEBALL_ARRIVE_WAIT,
    STATE_SHOW_NEW_MON,
    STATE_NEW_MON_MSG,
    STATE_TAKE_CARE_OF_MON,
    STATE_AFTER_NEW_MON_DELAY,
    STATE_CHECK_RIBBONS,
    STATE_END_LINK_TRADE,
    STATE_TRY_EVOLUTION,
    STATE_FADE_OUT_END,
    STATE_WAIT_FADE_OUT_END,
    STATE_LINK_MON_TRAVEL_OFFSCREEN = 200,
};

#define Trade_SendData(ptr) (SendBlock(BitmaskAllOtherLinkPlayers(), ptr->linkData, 20))

struct InGameTrade {
    /*0x00*/ u8 nickname[POKEMON_NAME_LENGTH + 1];
    /*0x0C*/ u16 species;
    /*0x0E*/ u8 ivs[NUM_STATS];
    /*0x14*/ bool8 abilityNum;
    /*0x18*/ u32 otId;
    /*0x1C*/ u8 conditions[CONTEST_CATEGORIES_COUNT];
    /*0x24*/ u32 personality;
    /*0x28*/ u16 heldItem;
    /*0x2A*/ u8 mailNum;
    /*0x2B*/ u8 otName[TRAINER_NAME_LENGTH + 1];
    /*0x36*/ u8 otGender;
    /*0x37*/ u8 sheen;
    /*0x38*/ u16 requestedSpecies;
};

struct UnkStructC {
    /*0x00*/ u16 words[9];
    /*0x10*/ u8 string[8];
    /*0x1A*/ u8 otId[4];
    /*0x1E*/ u16 species;
    /*0x20*/ u16 heldItem;
};

struct UnkStructD {
    /*0x00*/ u8 filler_00[8];
    /*0x08*/ void *vramAddr;
    /*0x0c*/ u8 filler_0c[4];
    /*0x10*/ u8 unk_10;
    /*0x12*/ u16 tilemapBuffer[BG_SCREEN_SIZE / 2];
};

struct UnkStructE {
    /*0x00*/ bool8 active;
    /*0x02*/ u16 delay;
    /*0x04*/ u8 actionId;
};

struct TradeEwramSubstruct {
    /*0x0000*/ u8 bg2hofs;
    /*0x0001*/ u8 bg3hofs;
    /*0x0004*/ struct Window window;
    /*0x0034*/ u8 partySpriteIds[2][PARTY_SIZE];
    /*0x0040*/ u8 cursorSpriteId;
    /*0x0041*/ u8 cursorPosition;
    /*0x0042*/ u8 partyCounts[2];
    /*0x0044*/ bool8 optionsActive[PARTY_SIZE * 2 + 1];
    /*0x0051*/ bool8 isLiveMon[2][PARTY_SIZE];
    /*0x005d*/ bool8 isEgg[2][PARTY_SIZE];
    /*0x0069*/ u8 hpBarLevels[2][PARTY_SIZE];
    /*0x0075*/ u8 bufferPartyState;
    /*0x0076*/ u8 filler_0076[4];
    /*0x007a*/ u8 unk_007a;
    /*0x007b*/ u8 callbackId;
    /*0x007c*/ u8 unk_007c;
    /*0x007d*/ u8 unk_007d;
    /*0x007e*/ u16 bottomTextTileStart;
    /*0x0080*/ u8 drawSelectedMonState[2];
    /*0x0082*/ u8 selectedMonIdx[2];
    /*0x0084*/ u8 playerSelectStatus;
    /*0x0085*/ u8 partnerSelectStatus;
    /*0x0086*/ u8 playerConfirmStatus;
    /*0x0087*/ u8 partnerConfirmStatus;
    /*0x0088*/ u8 filler_0088[2];
    /*0x008a*/ u8 partnerCursorPosition;
    /*0x008c*/ u16 linkData[20];
    /*0x00b4*/ u8 timer;
    /*0x00b5*/ u8 giftRibbons[GIFT_RIBBONS_COUNT];
    /*0x00c0*/ u8 filler_00c0[8];
    /*0x00c8*/ struct UnkStructD unk_00c8;
    /*0x08dc*/ struct UnkStructE queuedActions[4];
    /*0x08fc*/ u8 filler_08fc[0x704];
};

struct TradeEwramSubstruct2 *gTradeAnim;

static u8 gUnknown_03000508[8];

struct UnkStructF {
    u8 filler_0000[9];
    u8 curMonIndex;
    u8 filler_000a[0xff6];
};

struct TradeEwramStruct {
    /*0x00000*/ u8 filler_00000[0x7000];
    /*0x07000*/ struct TradeEwramSubstruct unk_07000;
    /*0x08000*/ struct UnkStructF unk_08000;
    /*0x09000*/ u8 filler_09000[0x4000];
    /*0x0d000*/ u8 tileBuffers[NUM_MENU_TEXT_SPRITES][256];
    /*0x0dd00*/ u8 filler_0dd00[0x1300];
    /*0x0f000*/ struct TradeEwramSubstruct2 unk_0f000;
};

static void CB2_CreateTradeMenu(void);
static void sub_804AFB8(const struct WindowTemplate *, u8 *, const u8 *, u8);
static void DrawBottomRowText(const u8 *, u8 *, u8);
static void nullsub_5(u8, u8);
static void sub_804AA88(void);
static void sub_804A964(struct UnkStructD *, void *);
static void VBlankCB_TradeMenu(void);
static void PrintTradeMessage(u8);
static bool8 BufferTradeParties(void);
static void SaveTradeGiftRibbons(void);
static bool8 LoadUISpriteGfx(void);
static void ComputePartyTradeableFlags(u8);
static void PrintPartyNicknames(u8);
static void SetActiveMenuOptions(void);
static void LoadTradeBgGfx(u8);
static void ComputePartyHPBarLevels(u8);
static void SetTradePartyHPBarSprites(void);
static void CB1_UpdateLink(void);
static void CB2_TradeMenu(void);
static void sub_804A940(struct UnkStructD *);
static void RunTradeMenuCallback(void);
static void DoQueuedActions(void);
static void DrawSelectedMonScreen(u8);
static void PrintPartyLevelsAndGenders(u8);
static void sub_804A938(struct UnkStructD *);
static void Task_DrawSelectionSummary(u8);
static void Task_DrawSelectionTrade(u8);
static void SetSelectedMon(u8);
static void QueueAction(u16, u8);
static void PrintTradePartnerPartyNicknames(void);
static u8 sub_80499F0(const u8 *, u8, u8);
static void RedrawPartyWindow(u8);
u8 GetMonNicknameWidth(u8 *, u8, u8);
static void sub_804A96C_alt(struct UnkStructD *, u8, u8, const u16 *, u8, u8, u16);
static void sub_804A96C(struct UnkStructD *, u8, u8, const u16 *, u8, u8, u16);
static void UNUSED BufferMovesString(u8 *, u8, u8);
void PrintLevelAndGender(u8, u8, u8, u8, u8, u8);
static void SpriteCB_BouncingPokeballDepartEnd(struct Sprite *);
static bool8 DoTradeAnim(void);
static void CB2_WaitTradeComplete(void);
static u8 TradeGetMultiplayerId(void);
static void sub_804E144(void);
static void Task_InGameTrade(u8);
static void GetInGameTradeMail(struct Mail *, const struct InGameTrade *);
static void CB2_SaveAndEndTrade(void);
static void SpriteCB_BouncingPokeball(struct Sprite *);
static void SpriteCB_BouncingPokeballDepart(struct Sprite *);
static void SetTradeSequenceBgGpuRegs(u8);
static void SpriteCB_LinkMonGlow(struct Sprite *);
static void SpriteCB_LinkMonShadow(struct Sprite *);
static void SpriteCB_CableEndSending(struct Sprite *);
static void SpriteCB_GbaScreen(struct Sprite *);
static void SpriteCB_BouncingPokeballArrive(struct Sprite *);
static void CheckPartnersMonForRibbons(void);
static void CB2_InGameTrade(void);
static void BufferInGameTradeMonName(void);
static void LoadTradeSequenceSpriteSheetsAndPalettes(void);
static void BufferTradeSceneStrings(void);
void CB2_UpdateLinkTrade(void);

static EWRAM_DATA u8 *sMenuTextTileBuffers[NUM_MENU_TEXT_SPRITES] = {0};
EWRAM_DATA struct Mail gTradeMail[PARTY_SIZE] = {0};
EWRAM_DATA u8 gSelectedTradeMonPositions[2] = {0};

struct TradeEwramSubstruct *gTradeMenu;

#include "data/trade.h"

// .text

void CB2_StartCreateTradeMenu(void)
{
    SetMainCallback2(CB2_CreateTradeMenu);
}

static void sub_8047CE8(void)
{
    u8 mpId;
    sub_804AFB8(&gWindowTemplate_81E725C, sMenuTextTileBuffers[GFXTAG_PLAYER_NAME_L], gSaveBlock2.playerName, 0xC);
    mpId = GetMultiplayerId();
    sub_804AFB8(&gWindowTemplate_81E725C, sMenuTextTileBuffers[GFXTAG_PARTNER_NAME_L], gLinkPlayers[mpId ^ 1].name, 0xC);
    sub_804AFB8(&gWindowTemplate_81E725C, sMenuTextTileBuffers[GFXTAG_CANCEL_L], sActionTexts[TEXT_CANCEL], 0x8);
    DrawBottomRowText(sActionTexts[TEXT_CHOOSE_MON], sMenuTextTileBuffers[GFXTAG_CHOOSE_PKMN_L], 0x14);
    nullsub_5(3, 0);
}

static void sub_8047D58(void)
{
    struct SpriteTemplate spriteTemplate;
    int i;
    u8 mpId;
    u16 slen;
    int language;

    slen = StringLength(gSaveBlock2.playerName);
    language = slen <= 5 ? 0 : 1;
    for (i = 0; i < NUM_PLAYER_NAME_SPRITES; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + GFXTAG_PLAYER_NAME_L;
        CreateSprite(&spriteTemplate, gTradeUnknownSpriteCoords[language][0][0] + 32 * i, gTradeUnknownSpriteCoords[language][0][1], 1);
    }

    mpId = GetMultiplayerId();
    slen = StringLength(gLinkPlayers[mpId ^ 1].name);
    language = slen <= 5 ? 0 : 1;
    for (i = 0; i < NUM_PARTNER_NAME_SPRITES; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + GFXTAG_PARTNER_NAME_L;
        CreateSprite(&spriteTemplate, gTradeUnknownSpriteCoords[language][1][0] + 32 * i, gTradeUnknownSpriteCoords[language][1][1], 1);
    }
    nullsub_5(5, 0);
}

static void sub_8047E44(void)
{
    struct SpriteTemplate spriteTemplate;
    int i;

    for (i = 0; i < 2; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + GFXTAG_CANCEL_L;
        CreateSprite(&spriteTemplate, 0xd6 + 32 * i, 0x98, 1);
    }

    for (i = 0; i < NUM_CHOOSE_PKMN_SPRITES; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + GFXTAG_CHOOSE_PKMN_L;
        CreateSprite(&spriteTemplate, 0x18 + 32 * i, 0x96, 1);
    }
}

static void CB2_CreateTradeMenu(void)
{
    int i;

    switch (gMain.state)
    {
        case  0:
            gTradeMenu = &ewram_2010000.unk_07000;
            sub_804AA88();
            ResetSpriteData();
            FreeAllSpritePalettes();
            ResetTasks();
            sub_804A964(&gTradeMenu->unk_00c8, BG_SCREEN_ADDR(5));
            SetVBlankCallback(VBlankCB_TradeMenu);
            InitMenuWindow(&gMenuTextWindowTemplate);
            Text_LoadWindowTemplate(&gWindowTemplate_81E6F84);
            Text_InitWindowWithTemplate(&gTradeMenu->window, &gWindowTemplate_81E6F84);
            gTradeMenu->unk_007a = TextWindow_SetBaseTileNum(20);
            TextWindow_LoadStdFrameGraphics(&gTradeMenu->window);
            Menu_EraseScreen();
            LoadMonIconPalettes();
            gTradeMenu->bufferPartyState = 0;
            gTradeMenu->callbackId = CB_MAIN_MENU;
            gTradeMenu->unk_007c = 0;
            gTradeMenu->drawSelectedMonState[TRADE_PLAYER] = 0;
            gTradeMenu->drawSelectedMonState[TRADE_PARTNER] = 0;
            gTradeMenu->playerConfirmStatus = STATUS_NONE;
            gTradeMenu->partnerConfirmStatus = STATUS_NONE;
            gTradeMenu->timer = 0;
            gUnknown_03000508[0] = 0;
            gMain.state ++;
            PrintTradeMessage(MSG_STANDBY);
            CpuFill16(0, ewram_2010000.tileBuffers, sizeof(ewram_2010000.tileBuffers));
            for (i = 0; i < NUM_MENU_TEXT_SPRITES; i ++)
                sMenuTextTileBuffers[i] = ewram_2010000.tileBuffers[i];
            break;
        case  1:
            gLinkType = LINKTYPE_TRADE_CONNECTING;
            OpenLink();
            for (i = 0; i < PARTY_SIZE; i ++)
                CreateMon(&gEnemyParty[i], SPECIES_NONE, 0, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
            gMain.state ++;
            gTradeMenu->timer = 0;
            CreateTask(Task_WaitForLinkPlayerConnection, 1);
            break;
        case  2:
            gTradeMenu->timer ++;
            if (gTradeMenu->timer > 11)
            {
                gTradeMenu->timer = 0;
                gMain.state ++;
            }
            break;
        case  3:
            if (GetLinkPlayerCount_2() >= GetSavedPlayerCount())
            {
                if (IsLinkMaster())
                {
                    if (++gTradeMenu->timer > 30)
                    {
                        CheckShouldAdvanceLinkState();
                        gMain.state ++;
                    }
                }
                else
                    gMain.state ++;
            }
            break;
        case  4:
            if (gReceivedRemoteLinkPlayers == TRUE && IsLinkPlayerDataExchangeComplete() == TRUE)
            {
                CalculatePlayerPartyCount();
                gMain.state ++;
            }
            break;
        case  5:
            if (BufferTradeParties())
            {
                SaveTradeGiftRibbons();
                gMain.state ++;
            }
            break;
        case  6:
            CalculateEnemyPartyCount();
            Text_FillWindowRectDefPalette(&gTradeMenu->window, 0, 0, 0, 29, 19);
            REG_DISPCNT = 0;
            gTradeMenu->partyCounts[TRADE_PLAYER] = gPlayerPartyCount;
            gTradeMenu->partyCounts[TRADE_PARTNER] = gEnemyPartyCount;
            for (i = 0; i < gTradeMenu->partyCounts[TRADE_PLAYER]; i ++)
                gTradeMenu->partySpriteIds[TRADE_PLAYER][i] = CreateMonIcon(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG), SpriteCB_MonIcon, sTradeMonSpriteCoords[i][0] * 8 + 14, sTradeMonSpriteCoords[i][1] * 8 - 12, TRUE, GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
            for (i = 0; i < gTradeMenu->partyCounts[TRADE_PARTNER]; i ++)
                gTradeMenu->partySpriteIds[TRADE_PARTNER][i] = CreateMonIcon(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG, NULL), SpriteCB_MonIcon, sTradeMonSpriteCoords[PARTY_SIZE + i][0] * 8 + 14, sTradeMonSpriteCoords[PARTY_SIZE + i][1] * 8 - 12, TRUE, GetMonData(&gEnemyParty[i], MON_DATA_PERSONALITY));
            nullsub_5(2, 0);
            gMain.state ++;
            break;
        case  7:
            LoadHeldItemIcons();
            DrawHeldItemIconsForTrade(&gTradeMenu->partyCounts[TRADE_PLAYER], gTradeMenu->partySpriteIds[TRADE_PLAYER], 0);
            gMain.state ++;
            break;
        case  8:
            DrawHeldItemIconsForTrade(&gTradeMenu->partyCounts[TRADE_PLAYER], gTradeMenu->partySpriteIds[TRADE_PLAYER], 1);
            gMain.state ++;
            break;
        case  9:
            sub_8047CE8();
            gMain.state ++;
            gTradeMenu->timer = 0;
            break;
        case 10:
            nullsub_5(4, 0);
            if (LoadUISpriteGfx())
                gMain.state ++;
            break;
        case 11:
            sub_8047D58();
            gMain.state ++;
            break;
        case 12:
            sub_8047E44();
            gTradeMenu->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, sTradeMonSpriteCoords[0][0] * 8 + 32, sTradeMonSpriteCoords[0][1] * 8, 2);
            gTradeMenu->cursorPosition = 0;
            gMain.state ++;
            nullsub_5(6, 0);
            break;
        case 13:
            ComputePartyTradeableFlags(TRADE_PLAYER);
            PrintPartyNicknames(TRADE_PLAYER);
            gTradeMenu->bg2hofs = 0;
            gTradeMenu->bg3hofs = 0;
            SetActiveMenuOptions();
            gMain.state ++;
            nullsub_5(7, 0);
            PlayBGM(MUS_SCHOOL);
            break;
        case 14:
            ComputePartyTradeableFlags(TRADE_PARTNER);
            PrintPartyNicknames(TRADE_PARTNER);
            gMain.state ++;
            // fallthrough
        case 15:
            LoadTradeBgGfx(0);
            gMain.state ++;
            break;
        case 16:
            LoadTradeBgGfx(1);
            gMain.state ++;
            break;
        case 17:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            gMain.state ++;
            break;
        case 18:
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
            gMain.state ++;
            break;
        case 19:
            ComputePartyHPBarLevels(TRADE_PLAYER);
            gMain.state ++;
            break;
        case 20:
            ComputePartyHPBarLevels(TRADE_PARTNER);
            SetTradePartyHPBarSprites();
            gMain.state ++;
            break;
        case 21:
            if (!gPaletteFade.active)
            {
                gMain.callback1 = CB1_UpdateLink;
                SetMainCallback2(CB2_TradeMenu);
                gUnknown_03000508[0] = 0;
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_ReturnToTradeMenu(void)
{
    int i;
    struct UnkStructF *unkStructF;

    switch (gMain.state)
    {
        case  0:
            gTradeMenu = &ewram_2010000.unk_07000;
            ResetSpriteData();
            FreeAllSpritePalettes();
            ResetTasks();
            sub_804A964(&gTradeMenu->unk_00c8, BG_SCREEN_ADDR(5));
            SetVBlankCallback(VBlankCB_TradeMenu);
            InitMenuWindow(&gMenuTextWindowTemplate);
            Text_LoadWindowTemplate(&gWindowTemplate_81E6F84);
            Text_InitWindowWithTemplate(&gTradeMenu->window, &gWindowTemplate_81E6F84);
            gTradeMenu->unk_007a = TextWindow_SetBaseTileNum(20);
            TextWindow_LoadStdFrameGraphics(&gTradeMenu->window);
            Menu_EraseScreen();
            LoadMonIconPalettes();
            gTradeMenu->bufferPartyState = 0;
            gTradeMenu->callbackId = CB_MAIN_MENU;
            gTradeMenu->unk_007c = 0;
            gTradeMenu->drawSelectedMonState[TRADE_PLAYER] = 0;
            gTradeMenu->drawSelectedMonState[TRADE_PARTNER] = 0;
            gTradeMenu->playerConfirmStatus = STATUS_NONE;
            gTradeMenu->partnerConfirmStatus = STATUS_NONE;
            gTradeMenu->timer = 0;
            gUnknown_03000508[0] = 0;
            gMain.state ++;
            for (i = 0; i < NUM_MENU_TEXT_SPRITES; i ++)
                sMenuTextTileBuffers[i] = ewram_2010000.tileBuffers[i];
            break;
        case  1:
            gMain.state ++;
            gTradeMenu->timer = 0;
            break;
        case  2:
            gMain.state ++;
            break;
        case  3:
            gMain.state ++;
            break;
        case  4:
            CalculatePlayerPartyCount();
            gMain.state ++;
            break;
        case  5:
            gMain.state ++;
            break;
        case  6:
            CalculateEnemyPartyCount();
            REG_DISPCNT = 0;
            gTradeMenu->partyCounts[TRADE_PLAYER] = gPlayerPartyCount;
            gTradeMenu->partyCounts[TRADE_PARTNER] = gEnemyPartyCount;
            PrintPartyNicknames(TRADE_PLAYER);
            PrintPartyNicknames(TRADE_PARTNER);
            for (i = 0; i < gTradeMenu->partyCounts[TRADE_PLAYER]; i ++)
                gTradeMenu->partySpriteIds[TRADE_PLAYER][i] = CreateMonIcon(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL), SpriteCB_MonIcon, sTradeMonSpriteCoords[i][0] * 8 + 14, sTradeMonSpriteCoords[i][1] * 8 - 12, TRUE, GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
            for (i = 0; i < gTradeMenu->partyCounts[TRADE_PARTNER]; i ++)
                gTradeMenu->partySpriteIds[TRADE_PARTNER][i] = CreateMonIcon(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG, NULL), SpriteCB_MonIcon, sTradeMonSpriteCoords[PARTY_SIZE + i][0] * 8 + 14, sTradeMonSpriteCoords[PARTY_SIZE + i][1] * 8 - 12, TRUE, GetMonData(&gEnemyParty[i], MON_DATA_PERSONALITY));
            nullsub_5(2, 0);
            gMain.state ++;
            break;
        case  7:
            LoadHeldItemIcons();
            DrawHeldItemIconsForTrade(&gTradeMenu->partyCounts[TRADE_PLAYER], gTradeMenu->partySpriteIds[TRADE_PLAYER], 0);
            gMain.state ++;
            break;
        case  8:
            DrawHeldItemIconsForTrade(&gTradeMenu->partyCounts[TRADE_PLAYER], gTradeMenu->partySpriteIds[TRADE_PLAYER], 1);
            gMain.state ++;
            break;
        case  9:
            sub_8047CE8();
            gMain.state ++;
            gTradeMenu->timer = 0;
            break;
        case 10:
            nullsub_5(4, 0);
            if (LoadUISpriteGfx())
            {
                gMain.state ++;
            }
            break;
        case 11:
            sub_8047D58();
            gMain.state ++;
            break;
        case 12:
            sub_8047E44();
            unkStructF = &ewram_2010000.unk_08000;
            if (gTradeMenu->cursorPosition < 6)
                gTradeMenu->cursorPosition = unkStructF->curMonIndex;
            else
                gTradeMenu->cursorPosition = unkStructF->curMonIndex + PARTY_SIZE;
            gTradeMenu->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, sTradeMonSpriteCoords[gTradeMenu->cursorPosition][0] * 8 + 32, sTradeMonSpriteCoords[gTradeMenu->cursorPosition][1] * 8, 2);
            gMain.state = 15;
            nullsub_5(6, 0);
            break;
        case 15:
            LoadTradeBgGfx(0);
            gMain.state ++;
            break;
        case 16:
            LoadTradeBgGfx(1);
            gTradeMenu->bg2hofs = 0;
            gTradeMenu->bg3hofs = 0;
            SetActiveMenuOptions();
            nullsub_5(7, 0);
            gMain.state ++;
            break;
        case 17:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            gMain.state ++;
            break;
        case 18:
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
            gMain.state ++;
            break;
        case 19:
            gMain.state ++;
            break;
        case 20:
            SetTradePartyHPBarSprites();
            gMain.state ++;
            break;
        case 21:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(CB2_TradeMenu);
                gUnknown_03000508[0] = 0;
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_TradeMenu(void)
{
    sub_804A940(&gTradeMenu->unk_00c8);
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB_FadeToStartTrade(void)
{
    if (++gTradeMenu->timer >= 16)
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTradeMenu->callbackId = CB_WAIT_TO_START_TRADE;
    }
}

/*static*/ void CB_WaitToStartTrade(void)
{
    if (!gPaletteFade.active)
    {
        gSelectedTradeMonPositions[TRADE_PLAYER] = gTradeMenu->cursorPosition;
        gSelectedTradeMonPositions[TRADE_PARTNER] = gTradeMenu->partnerCursorPosition;
        SetCloseLinkCallback();
        gTradeMenu->callbackId = CB_START_LINK_TRADE;
    }
}

/*static*/ void CB_StartLinkTrade(void)
{
    if (!gReceivedRemoteLinkPlayers)
    {
        gMain.callback1 = NULL;
        SetMainCallback2(CB2_LinkTrade);
    }
}

static void CB2_TradeMenu(void)
{
    RunTradeMenuCallback();
    DoQueuedActions();
    DrawSelectedMonScreen(TRADE_PLAYER);
    DrawSelectedMonScreen(TRADE_PARTNER);
    REG_BG2HOFS = gTradeMenu->bg2hofs++;
    REG_BG3HOFS = gTradeMenu->bg3hofs--;
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void LoadTradeBgGfx(u8 state)
{
    int i;

    switch (state)
    {
        case 0:
            for (i = 0; i < (int)(3 * PLTT_SIZE_4BPP / sizeof(u16)); i ++)
            {
                gPlttBufferUnfaded[i] = *(gTradeMenu_Pal + i);
                gPlttBufferFaded[i] = *(gTradeMenu_Pal + i);
            }
            DmaCopyLarge16(3, gTradeMenu_Gfx, (void *)BG_VRAM, 0x1280, 0x1000);
            for (i = 0; i < (int)ARRAY_COUNT(gTradeMenu->unk_00c8.tilemapBuffer); i ++)
                gTradeMenu->unk_00c8.tilemapBuffer[i] = gTradeMenu_Tilemap[i];
            DmaCopy16Defvars(3, sTradeStripesBG2Tilemap, BG_SCREEN_ADDR(6), BG_SCREEN_SIZE);
            break;
        case 1:
            DmaCopy16Defvars(3, sTradeStripesBG3Tilemap, BG_SCREEN_ADDR(7), BG_SCREEN_SIZE);
            PrintPartyLevelsAndGenders(TRADE_PLAYER);
            PrintPartyLevelsAndGenders(TRADE_PARTNER);
            sub_804A938(&gTradeMenu->unk_00c8);
            REG_BG0CNT &= ~0x03; // BGCNT_PRIORITY(0)
            REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(5);
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(6);
            REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_SCREENBASE(7);
            REG_BG0HOFS = 0;
            REG_BG1HOFS = 0;
            REG_BG2HOFS = 0;
            REG_BG3HOFS = 0;
            REG_BG0VOFS = 0;
            REG_BG1VOFS = 0;
            REG_BG2VOFS = 0;
            REG_BG3VOFS = 0;
            break;
    }
}

void SetActiveMenuOptions(void)
{
    int i;
    for (i = 0; i < PARTY_SIZE; i ++)
    {
        if (i < gTradeMenu->partyCounts[TRADE_PLAYER])
        {
            gSprites[gTradeMenu->partySpriteIds[TRADE_PLAYER][i]].invisible = FALSE;
            gTradeMenu->optionsActive[i] = TRUE;
        }
        else
        {
            gTradeMenu->optionsActive[i] = FALSE;
        }
        if (i < gTradeMenu->partyCounts[TRADE_PARTNER])
        {
            gSprites[gTradeMenu->partySpriteIds[TRADE_PARTNER][i]].invisible = FALSE;
            gTradeMenu->optionsActive[i + PARTY_SIZE] = TRUE;
        }
        else
        {
            gTradeMenu->optionsActive[i + PARTY_SIZE] = FALSE;
        }
    }
    gTradeMenu->optionsActive[PARTY_SIZE * 2] = TRUE;
}

static void nullsub_5(u8 a0, u8 a1) {}

// why not just use memcpy?
static void Trade_Memcpy(void *dest, void *src, u32 size)
{
    u8 *_dest = dest;
    u8 *_src = src;
    int i;
    for (i = 0; i < size; i ++) _dest[i] = _src[i];
}

static bool8 BufferTradeParties(void)
{
    u8 id = GetMultiplayerId();
    int i;
    u16 species;
    u8 name[POKEMON_NAME_LENGTH + 1];
    struct Pokemon *mon;

    SetLinkDebugValues(gTradeMenu->bufferPartyState / 100, gTradeMenu->bufferPartyState % 100);
    switch (gTradeMenu->bufferPartyState)
    {
        case  0:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[0], 2 * sizeof(struct Pokemon));
            gTradeMenu->bufferPartyState ++;
            break;
        case  1:
            if (IsLinkTaskFinished())
            {
                if (GetBlockReceivedStatus() == 0)
                {
                    gTradeMenu->bufferPartyState ++;
                }
                else
                {
                    ResetBlockReceivedFlags();
                    gTradeMenu->bufferPartyState ++;
                }
            }
            break;
        case  2:
            if (id == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_200);
            }
            gTradeMenu->bufferPartyState ++;
            break;
        case  3:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[0], gBlockRecvBuffer[id ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                gTradeMenu->bufferPartyState ++;
            }
            break;
        case  4:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[2], 2 * sizeof(struct Pokemon));
            gTradeMenu->bufferPartyState ++;
            break;
        case  5:
            if (id == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_200);
            }
            gTradeMenu->bufferPartyState ++;
            break;
        case  6:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[2], gBlockRecvBuffer[id ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                gTradeMenu->bufferPartyState ++;
            }
            break;
        case  7:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[4], 2 * sizeof(struct Pokemon));
            gTradeMenu->bufferPartyState ++;
            break;
        case  8:
            if (id == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_200);
            }
            gTradeMenu->bufferPartyState ++;
            break;
        case  9:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[4], gBlockRecvBuffer[id ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                gTradeMenu->bufferPartyState ++;
            }
            break;
        case 10:
            Trade_Memcpy(gBlockSendBuffer, &gSaveBlock1.mail[0], PARTY_SIZE * sizeof(struct Mail) + 4);
            gTradeMenu->bufferPartyState ++;
            break;
        case 11:
            if (id == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_220);
            }
            gTradeMenu->bufferPartyState ++;
            break;
        case 12:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gTradeMail[0], gBlockRecvBuffer[id ^ 1], PARTY_SIZE * sizeof(struct Mail));
                ResetBlockReceivedFlags();
                gTradeMenu->bufferPartyState ++;
            }
            break;
        case 13:
            Trade_Memcpy(gBlockSendBuffer, gSaveBlock1.giftRibbons, sizeof(gTradeMenu->giftRibbons));
            gTradeMenu->bufferPartyState ++;
            break;
        case 14:
            if (id == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_40);
            }
            gTradeMenu->bufferPartyState ++;
            break;
        case 15:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(gTradeMenu->giftRibbons, gBlockRecvBuffer[id ^ 1], sizeof(gTradeMenu->giftRibbons));
                ResetBlockReceivedFlags();
                gTradeMenu->bufferPartyState ++;
            }
            break;
        case 16:
            mon = gEnemyParty;
            for (i = 0; i < PARTY_SIZE; i ++)
            {
                if ((species = GetMonData(mon, MON_DATA_SPECIES)) != SPECIES_NONE && species == SPECIES_SHEDINJA && GetMonData(mon, MON_DATA_LANGUAGE) != LANGUAGE_JAPANESE)
                {
                    GetMonData(mon, MON_DATA_NICKNAME, name);
                    if (!StringCompareWithoutExtCtrlCodes(name, sText_ShedinjaJP))
                        SetMonData(mon, MON_DATA_NICKNAME, gSpeciesNames[SPECIES_SHEDINJA]);
                }
                mon ++;
            }
            return TRUE;
    }
    return FALSE;
}

static void PrintIsThisTradeOkay(void)
{
    u8 string[28];
    StringCopy(string, sText_IsThisTradeOkay);
    DrawBottomRowText(string, BG_CHAR_ADDR(4) + gTradeMenu->bottomTextTileStart * 32, 20);
}

static void Leader_ReadLinkBuffer(u8 mpId, u8 status)
{
    if (status & 1)
    {
        switch (gBlockRecvBuffer[mpId][0])
        {
            case LINKCMD_REQUEST_CANCEL:
                gTradeMenu->playerSelectStatus = STATUS_CANCEL;
                break;
            case LINKCMD_READY_TO_TRADE:
                gTradeMenu->playerSelectStatus = STATUS_READY;
                break;
            case LINKCMD_INIT_BLOCK:
                gTradeMenu->playerConfirmStatus = STATUS_READY;
                break;
            case LINKCMD_READY_CANCEL_TRADE:
                gTradeMenu->playerConfirmStatus = STATUS_CANCEL;
                break;
        }
        ResetBlockReceivedFlag(0);
    }
    if (status & 2)
    {
        switch (gBlockRecvBuffer[1][0])
        {
            case LINKCMD_REQUEST_CANCEL:
                gTradeMenu->partnerSelectStatus = STATUS_CANCEL;
                break;
            case LINKCMD_READY_TO_TRADE:
                gTradeMenu->partnerCursorPosition = gBlockRecvBuffer[1][1] + PARTY_SIZE;
                gTradeMenu->partnerSelectStatus = STATUS_READY;
                break;
            case LINKCMD_INIT_BLOCK:
                gTradeMenu->partnerConfirmStatus = STATUS_READY;
                break;
            case LINKCMD_READY_CANCEL_TRADE:
                gTradeMenu->partnerConfirmStatus = STATUS_CANCEL;
                break;
        }
        ResetBlockReceivedFlag(1);
    }
}

static void Follower_ReadLinkBuffer(u8 mpId, u8 status)
{
    if (status & 1)
    {
        switch (gBlockRecvBuffer[0][0])
        {
            case LINKCMD_BOTH_CANCEL_TRADE:
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                PrintTradeMessage(MSG_WAITING_FOR_FRIEND);
                gTradeMenu->callbackId = CB_INIT_EXIT_CANCELED_TRADE;
                break;
            case LINKCMD_PARTNER_CANCEL_TRADE:
                PrintTradeMessage(MSG_FRIEND_WANTS_TO_TRADE);
                gTradeMenu->callbackId = CB_HANDLE_TRADE_CANCELED;
                break;
            case LINKCMD_SET_MONS_TO_TRADE:
                gTradeMenu->partnerCursorPosition = gBlockRecvBuffer[0][1] + PARTY_SIZE;
                SetSelectedMon(gTradeMenu->cursorPosition);
                SetSelectedMon(gTradeMenu->partnerCursorPosition);
                gTradeMenu->callbackId = CB_PRINT_IS_THIS_OKAY;
                break;
            case LINKCMD_START_TRADE:
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gTradeMenu->callbackId = CB_WAIT_TO_START_TRADE;
                break;
            case LINKCMD_PLAYER_CANCEL_TRADE:
                PrintTradeMessage(MSG_CANCELED);
                gTradeMenu->callbackId = CB_HANDLE_TRADE_CANCELED;
                break;
        }
        ResetBlockReceivedFlag(0);
    }
    if (status & 2)
    {
        ResetBlockReceivedFlag(1);
    }
}

static void Leader_HandleCommunication(void)
{
    if (gTradeMenu->playerSelectStatus && gTradeMenu->partnerSelectStatus)
    {
        if (gTradeMenu->playerSelectStatus == STATUS_READY && gTradeMenu->partnerSelectStatus == STATUS_READY)
        {
            gTradeMenu->callbackId = CB_SET_SELECTED_MONS;
            gTradeMenu->linkData[0] = LINKCMD_SET_MONS_TO_TRADE;
            gTradeMenu->linkData[1] = gTradeMenu->cursorPosition;
            QueueAction(QUEUE_DELAY_DATA, QUEUE_SEND_DATA);
            gTradeMenu->playerSelectStatus = gTradeMenu->partnerSelectStatus = STATUS_NONE;
        }
        else if (gTradeMenu->playerSelectStatus == STATUS_READY && gTradeMenu->partnerSelectStatus == STATUS_CANCEL)
        {
            PrintTradeMessage(MSG_CANCELED);
            gTradeMenu->linkData[0] = LINKCMD_PARTNER_CANCEL_TRADE;
            gTradeMenu->linkData[1] = 0;
            QueueAction(QUEUE_DELAY_DATA, QUEUE_SEND_DATA);
            gTradeMenu->playerConfirmStatus = gTradeMenu->partnerConfirmStatus = STATUS_NONE;
            gTradeMenu->playerSelectStatus = gTradeMenu->partnerSelectStatus = STATUS_NONE;
            gTradeMenu->callbackId = CB_HANDLE_TRADE_CANCELED;
        }
        else if (gTradeMenu->playerSelectStatus == STATUS_CANCEL && gTradeMenu->partnerSelectStatus == STATUS_READY)
        {
            PrintTradeMessage(MSG_FRIEND_WANTS_TO_TRADE);
            gTradeMenu->linkData[0] = LINKCMD_PLAYER_CANCEL_TRADE;
            gTradeMenu->linkData[1] = 0;
            QueueAction(QUEUE_DELAY_DATA, QUEUE_SEND_DATA);
            gTradeMenu->playerConfirmStatus = gTradeMenu->partnerConfirmStatus = STATUS_NONE;
            gTradeMenu->playerSelectStatus = gTradeMenu->partnerSelectStatus = STATUS_NONE;
            gTradeMenu->callbackId = CB_HANDLE_TRADE_CANCELED;
        }
        else if (gTradeMenu->playerSelectStatus == STATUS_CANCEL && gTradeMenu->partnerSelectStatus == STATUS_CANCEL)
        {
            gTradeMenu->linkData[0] = LINKCMD_BOTH_CANCEL_TRADE;
            gTradeMenu->linkData[1] = 0;
            QueueAction(QUEUE_DELAY_DATA, QUEUE_SEND_DATA);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTradeMenu->playerSelectStatus = gTradeMenu->partnerSelectStatus = STATUS_NONE;
            gTradeMenu->callbackId = CB_INIT_EXIT_CANCELED_TRADE;
        }
    }
    if (gTradeMenu->playerConfirmStatus && gTradeMenu->partnerConfirmStatus)
    {
        if (gTradeMenu->playerConfirmStatus == STATUS_READY && gTradeMenu->partnerConfirmStatus == STATUS_READY)
        {
            gTradeMenu->linkData[0] = LINKCMD_START_TRADE;
            gTradeMenu->linkData[1] = 0;
            QueueAction(QUEUE_DELAY_DATA, QUEUE_SEND_DATA);
            gTradeMenu->playerConfirmStatus = STATUS_NONE;
            gTradeMenu->partnerConfirmStatus = STATUS_NONE;
            gTradeMenu->callbackId = CB_FADE_TO_START_TRADE;
        }
        if (gTradeMenu->playerConfirmStatus == STATUS_CANCEL || gTradeMenu->partnerConfirmStatus == STATUS_CANCEL)
        {
            PrintTradeMessage(MSG_CANCELED);
            gTradeMenu->linkData[0] = LINKCMD_PLAYER_CANCEL_TRADE;
            gTradeMenu->linkData[1] = 0;
            QueueAction(QUEUE_DELAY_DATA, QUEUE_SEND_DATA);
            gTradeMenu->playerConfirmStatus = STATUS_NONE;
            gTradeMenu->partnerConfirmStatus = STATUS_NONE;
            gTradeMenu->callbackId = CB_HANDLE_TRADE_CANCELED;
        }
    }
}

static void CB1_UpdateLink(void)
{
    u8 mpId = GetMultiplayerId();
    u8 status;
    if ((status = GetBlockReceivedStatus()))
    {
        if (mpId == 0)
            Leader_ReadLinkBuffer(mpId, status);
        else
            Follower_ReadLinkBuffer(mpId, status);
        ResetBlockReceivedFlags();
    }
    if (mpId == 0)
        Leader_HandleCommunication();
}

static u8 GetNewCursorPosition(u8 oldPosition, u8 direction)
{
    int i;
    u8 newPosition = 0;
    for (i = 0; i < PARTY_SIZE; i ++)
    {
        if (gTradeMenu->optionsActive[sCursorMoveDestinations[oldPosition][direction][i]] == TRUE)
        {
            newPosition = sCursorMoveDestinations[oldPosition][direction][i];
            break;
        }
    }
    return newPosition;
}

static void TradeMenuMoveCursor(u8 *cursorPosition, u8 direction)
{
    u8 newPosition = GetNewCursorPosition(*cursorPosition, direction);
    if (newPosition == PARTY_SIZE * 2) // CANCEL
    {
        StartSpriteAnim(&gSprites[gTradeMenu->cursorSpriteId], CURSOR_ANIM_ON_CANCEL);
        gSprites[gTradeMenu->cursorSpriteId].x = DISPLAY_WIDTH - 16;
        gSprites[gTradeMenu->cursorSpriteId].y = DISPLAY_HEIGHT;
    }
    else
    {
        StartSpriteAnim(&gSprites[gTradeMenu->cursorSpriteId], CURSOR_ANIM_NORMAL);
        gSprites[gTradeMenu->cursorSpriteId].x = sTradeMonSpriteCoords[newPosition][0] * 8 + 32;
        gSprites[gTradeMenu->cursorSpriteId].y = sTradeMonSpriteCoords[newPosition][1] * 8;
    }
    if (*cursorPosition != newPosition)
    {
        PlaySE(SE_SELECT);
    }
    *cursorPosition = newPosition;
}

static void SetReadyToTrade(void)
{
    PrintTradeMessage(MSG_STANDBY);
    gTradeMenu->callbackId = CB_READY_WAIT;
    if (GetMultiplayerId() == 1)
    {
        gTradeMenu->linkData[0] = LINKCMD_READY_TO_TRADE;
        gTradeMenu->linkData[1] = gTradeMenu->cursorPosition;
        Trade_SendData(gTradeMenu);
    }
    else
    {
        gTradeMenu->playerSelectStatus = STATUS_READY;
    }
}

static void CB_ProcessMenuInput(void)
{
    int i;
    if (JOY_REPEAT(DPAD_UP))
    {
        TradeMenuMoveCursor(&gTradeMenu->cursorPosition, 0);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        TradeMenuMoveCursor(&gTradeMenu->cursorPosition, 1);
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        TradeMenuMoveCursor(&gTradeMenu->cursorPosition, 2);
    }
    else if (JOY_REPEAT(DPAD_RIGHT))
    {
        TradeMenuMoveCursor(&gTradeMenu->cursorPosition, 3);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gTradeMenu->cursorPosition < PARTY_SIZE)
        {
            TextWindow_DrawStdFrame(&gTradeMenu->window, 18, 14, 28, 19);
            Menu_PrintItems(19, 15, ARRAY_COUNT(sSelectTradeMonActions), (const struct MenuAction *)sSelectTradeMonActions);
            InitMenu(0, 19, 15, ARRAY_COUNT(sSelectTradeMonActions), 0, 9);
            gTradeMenu->callbackId = CB_SELECTED_MON;
        }
        else if (gTradeMenu->cursorPosition < 2 * PARTY_SIZE)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTradeMenu->callbackId = CB_SHOW_MON_SUMMARY;
        }
        else if (gTradeMenu->cursorPosition == 2 * PARTY_SIZE)
        {
            TextWindow_DrawStdFrame(&gTradeMenu->window, 24, 14, 29, 19);
            InitYesNoMenu(24, 14, 4);
            gTradeMenu->callbackId = CB_CANCEL_TRADE_PROMPT;
            DrawBottomRowText(sActionTexts[TEXT_CANCEL_TRADE], OBJ_VRAM0 + TILE_SIZE_4BPP * gTradeMenu->bottomTextTileStart, 20);
        }
    }
    if (JOY_NEW(R_BUTTON))
    {
        for (i = 0; i < 10; i ++)
        {
            gTradeMenu->linkData[i] = i;
        }
        Trade_SendData(gTradeMenu);
    }
}

static void RedrawChooseAPokemonWindow(void)
{
    Menu_DestroyCursor();
    PrintTradePartnerPartyNicknames();
    gTradeMenu->callbackId = CB_MAIN_MENU;
    gSprites[gTradeMenu->cursorSpriteId].invisible = FALSE;
    DrawBottomRowText(sActionTexts[TEXT_CHOOSE_MON], OBJ_VRAM0 + TILE_SIZE_4BPP * gTradeMenu->bottomTextTileStart, 20);
}

static void CB_ProcessSelectedMonInput(void)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gTradeMenu->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gTradeMenu->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTradeMenu->callbackId = CB_SHOW_MON_SUMMARY;
        }
        else if (sub_80499F0(gTradeMenu->isLiveMon[TRADE_PLAYER], gTradeMenu->partyCounts[TRADE_PLAYER], gTradeMenu->cursorPosition) == 0)
        {
            QueueAction(QUEUE_DELAY_MSG, QUEUE_ONLY_MON1);
            gTradeMenu->callbackId = CB_HANDLE_TRADE_CANCELED;
        }
        else
        {
            SetReadyToTrade();
            gSprites[gTradeMenu->cursorSpriteId].invisible = TRUE;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawChooseAPokemonWindow();
    }
}

static void CB_ChooseMonAfterButtonPress(void)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawChooseAPokemonWindow();
    }
}

static void CB_ShowTradeMonSummaryScreen(void)
{
    if (!gPaletteFade.active)
    {
        if (gTradeMenu->cursorPosition < PARTY_SIZE)
        {
            ShowPokemonSummaryScreen(gPlayerParty, gTradeMenu->cursorPosition, gTradeMenu->partyCounts[TRADE_PLAYER] - 1, CB2_ReturnToTradeMenu, SUMMARY_MODE_LOCK_MOVES);
        }
        else
        {
            ShowPokemonSummaryScreen(gEnemyParty, gTradeMenu->cursorPosition - PARTY_SIZE, gTradeMenu->partyCounts[TRADE_PARTNER] - 1, CB2_ReturnToTradeMenu, SUMMARY_MODE_LOCK_MOVES);
        }
    }
}

static u8 sub_80499F0(const u8 *aliveMons, u8 playerPartyCount, u8 playerMonIdx)
{
    u8 retval = 0;
    int i;
    for (i = 0; i < playerPartyCount; i ++)
    {
        if (playerMonIdx != i)
        {
            retval += aliveMons[i];
        }
    }
    return retval;
}

static void CheckMonsBeforeTrade(void)
{
    u8 aliveMons[PARTY_SIZE * 2];
    int i;
    for (i = 0; i < gTradeMenu->partyCounts[TRADE_PLAYER]; i ++)
    {
        aliveMons[i] = gTradeMenu->isLiveMon[TRADE_PLAYER][i];
    }
    if (sub_80499F0(aliveMons, gTradeMenu->partyCounts[TRADE_PLAYER], gTradeMenu->cursorPosition) == 0)
    {
        QueueAction(QUEUE_DELAY_MSG, QUEUE_ONLY_MON1);
        gTradeMenu->linkData[0] = LINKCMD_READY_CANCEL_TRADE;
        QueueAction(0xb4, QUEUE_SEND_DATA);
    }
    else
    {
        QueueAction(QUEUE_DELAY_MSG, QUEUE_STANDBY);
        gTradeMenu->linkData[0] = LINKCMD_INIT_BLOCK;
        if (IsLinkTaskFinished())
        {
            Trade_SendData(gTradeMenu);
        }
    }
}

static void CB_ProcessConfirmTradeInput(void)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gTradeMenu->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gTradeMenu->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            CheckMonsBeforeTrade();
        }
        else
        {
            QueueAction(QUEUE_DELAY_MSG, QUEUE_STANDBY);
            if (IsLinkTaskFinished())
            {
                gTradeMenu->linkData[0] = LINKCMD_READY_CANCEL_TRADE;
                Trade_SendData(gTradeMenu);
            }
        }
        gTradeMenu->callbackId = CB_IDLE;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        QueueAction(QUEUE_DELAY_MSG, QUEUE_STANDBY);
        if (Menu_GetCursorPos() == 0)
        {
            gTradeMenu->unk_007c = Menu_MoveCursor(+1);
        }
        gTradeMenu->linkData[0] = LINKCMD_READY_CANCEL_TRADE;
        Trade_SendData(gTradeMenu);
        gTradeMenu->callbackId = CB_IDLE;
    }
}

static void CB_ProcessCancelTradeInput(void)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gTradeMenu->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gTradeMenu->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            PrintTradeMessage(MSG_WAITING_FOR_FRIEND);
            gTradeMenu->linkData[0] = LINKCMD_REQUEST_CANCEL;
            gTradeMenu->linkData[1] = 0;
            QueueAction(QUEUE_DELAY_DATA, QUEUE_SEND_DATA);
            gSprites[gTradeMenu->cursorSpriteId].invisible = TRUE;
            gTradeMenu->callbackId = CB_IDLE;
        }
        else
        {
            RedrawChooseAPokemonWindow();
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawChooseAPokemonWindow();
    }
}

static void CB_SetSelectedMons(void)
{
    if (GetMultiplayerId() == 0)
    {
        SetSelectedMon(gTradeMenu->cursorPosition);
        SetSelectedMon(gTradeMenu->partnerCursorPosition);
    }
    gTradeMenu->callbackId = CB_PRINT_IS_THIS_OKAY;
}

static void CB_PrintIsThisTradeOkay(void)
{
    if (gTradeMenu->drawSelectedMonState[TRADE_PLAYER] == DRAW_SELECTED_FINISH && gTradeMenu->drawSelectedMonState[TRADE_PARTNER] == DRAW_SELECTED_FINISH)
    {
        PrintIsThisTradeOkay();
        gTradeMenu->callbackId = CB_INIT_CONFIRM_TRADE_PROMPT;
    }
}

static void CB_InitConfirmTradePrompt(void)
{
    gTradeMenu->timer++;
    if (gTradeMenu->timer > 120)
    {
        TextWindow_DrawStdFrame(&gTradeMenu->window, 24, 14, 29, 19);
        InitYesNoMenu(24, 14, 4);
        gTradeMenu->timer = 0;
        gTradeMenu->callbackId = CB_CONFIRM_TRADE_PROMPT;
    }
}

static void CB_HandleTradeCanceled(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawPartyWindow(TRADE_PLAYER);
        RedrawPartyWindow(TRADE_PARTNER);
        gTradeMenu->callbackId = CB_MAIN_MENU;
        gSprites[gTradeMenu->cursorSpriteId].invisible = FALSE;
    }
}

static void CB_InitExitCanceledTrade(void)
{
    if (!gPaletteFade.active)
    {
        SetCloseLinkCallback();
        gTradeMenu->callbackId = CB_EXIT_CANCELED_TRADE;
    }
}

static void CB_ExitCanceledTrade(void)
{
    if (gReceivedRemoteLinkPlayers == 0)
    {
        SetMainCallback2(CB2_ReturnToFieldFromMultiplayer);
    }
}

static void RunTradeMenuCallback(void)
{
    switch (gTradeMenu->callbackId)
    {
        case CB_MAIN_MENU:
            CB_ProcessMenuInput();
            break;
        case CB_SELECTED_MON:
            CB_ProcessSelectedMonInput();
            break;
        case CB_SHOW_MON_SUMMARY:
            CB_ShowTradeMonSummaryScreen();
            break;
        case CB_CONFIRM_TRADE_PROMPT:
            CB_ProcessConfirmTradeInput();
            break;
        case CB_CANCEL_TRADE_PROMPT:
            CB_ProcessCancelTradeInput();
            break;
        case CB_SET_SELECTED_MONS:
            CB_SetSelectedMons();
            break;
        case CB_PRINT_IS_THIS_OKAY:
            CB_PrintIsThisTradeOkay();
            break;
        case CB_HANDLE_TRADE_CANCELED:
            CB_HandleTradeCanceled();
            break;
        case CB_FADE_TO_START_TRADE:
            CB_FadeToStartTrade();
            break;
        case CB_WAIT_TO_START_TRADE:
            CB_WaitToStartTrade();
            break;
        case CB_INIT_EXIT_CANCELED_TRADE:
            CB_InitExitCanceledTrade();
            break;
        case CB_EXIT_CANCELED_TRADE:
            CB_ExitCanceledTrade();
            break;
        case CB_START_LINK_TRADE:
            CB_StartLinkTrade();
            break;
        case CB_INIT_CONFIRM_TRADE_PROMPT:
            CB_InitConfirmTradePrompt();
            break;
        case CB_UNUSED_CLOSE_MSG:
            CB_ChooseMonAfterButtonPress();
            break;
    }
}

static void SetSelectedMon(u8 cursorPosition)
{
    u8 whichParty = cursorPosition / PARTY_SIZE;
    if (gTradeMenu->drawSelectedMonState[whichParty] == 0)
    {
        gTradeMenu->drawSelectedMonState[whichParty] = 1;
        gTradeMenu->selectedMonIdx[whichParty] = cursorPosition;
    }
}

// TODO: Figure out what the f**k is going on here
#ifdef NONMATCHING
static void DrawSelectedMonScreen(u8 whichParty)
{
    u8 i;
    s8 nameStringWidth;
    u8 selectedMonParty = TRADE_PARTNER;
    u8 partyIdx;
    u8 nickname[40];
    u8 movesString[56];
    u8 selectedMonIdx = gTradeMenu->selectedMonIdx[whichParty];
    if (selectedMonIdx < PARTY_SIZE)
        selectedMonParty = TRADE_PLAYER;
    partyIdx = selectedMonIdx % PARTY_SIZE;

    switch (gTradeMenu->drawSelectedMonState[whichParty])
    {
        case 1:
            for (i = 0; i < gTradeMenu->partyCounts[whichParty]; i ++)
            {
                gSprites[gTradeMenu->partySpriteIds[selectedMonParty][i]].invisible = TRUE;
            }
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].invisible = FALSE;
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].data[0] = 20;
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].data[2] = (sTradeMonSpriteCoords[PARTY_SIZE * selectedMonParty + partyIdx][0] + sTradeMonSpriteCoords[PARTY_SIZE * selectedMonParty + partyIdx + 1][0]) / 2 * 8 + 14;
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].data[4] = sTradeMonSpriteCoords[PARTY_SIZE * selectedMonParty + partyIdx][1] * 8 - 12;
            StoreSpriteCallbackInData6(&gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]], SpriteCB_MonIcon);
            gTradeMenu->drawSelectedMonState[whichParty] ++;
            Trade_MoveSelectedMonToTarget(&gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]]);
            Menu_DestroyCursor();
            Text_FillWindowRectDefPalette(&gTradeMenu->window, 0, gUnknown_0820C330[selectedMonParty][0], 0, gUnknown_0820C330[selectedMonParty][1], 19);
            sub_804A96C_alt(&gTradeMenu->unk_00c8, 15 * whichParty, 0, sTradePartyBoxTilemap, 15, 17, 0);
            if (selectedMonParty == TRADE_PLAYER)
            {
                PrintTradePartnerPartyNicknames();
            }
            break;
        case 2:
            if (gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].callback == SpriteCB_MonIcon)
            {
                gTradeMenu->drawSelectedMonState[whichParty] = 3;
            }
            break;
        case 3:
            sub_804A96C_alt(&gTradeMenu->unk_00c8, 15 * selectedMonParty, 0, sTradePartyBoxTilemap, 15, 17, 0);
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].x = (sTradeMonSpriteCoords[PARTY_SIZE * selectedMonParty + partyIdx ][0] + sTradeMonSpriteCoords[PARTY_SIZE * selectedMonParty + partyIdx + 1][0]) / 2 * 8 + 14;
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].y = sTradeMonSpriteCoords[PARTY_SIZE * selectedMonParty + partyIdx ][1] * 8 - 12;
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].x2 = 0;
            gSprites[gTradeMenu->partySpriteIds[selectedMonParty][partyIdx]].y2 = 0;
            nameStringWidth = GetMonNicknameWidth(nickname + 6, selectedMonParty, partyIdx);
            nickname[0] = EXT_CTRL_CODE_BEGIN;
            nickname[1] = EXT_CTRL_CODE_FONT;
            nickname[2] = 0x04;
            nickname[3] = EXT_CTRL_CODE_BEGIN;
            nickname[4] = EXT_CTRL_CODE_CLEAR;
            nickname[5] = (64 - nameStringWidth) / 2;
            Text_InitWindowAndPrintText(&gTradeMenu->window, nickname, gTradeMenu->unk_007a + selectedMonParty * PARTY_SIZE * 32, gUnknown_0820C334[selectedMonParty][0], gUnknown_0820C334[selectedMonParty][1]);
            BufferMovesString(movesString, selectedMonParty, partyIdx);
            Text_InitWindowAndPrintText(&gTradeMenu->window, gOtherText_Terminator2, gTradeMenu->unk_007a + selectedMonParty * PARTY_SIZE * 32 + 32, gUnknown_0820C334[selectedMonParty + 1][0], gUnknown_0820C334[selectedMonParty + 1][1] + 1);
            Text_InitWindowAndPrintText(&gTradeMenu->window, movesString, gTradeMenu->unk_007a + selectedMonParty * PARTY_SIZE * 32 + 38, gUnknown_0820C334[selectedMonParty + 1][0], gUnknown_0820C334[selectedMonParty + 1][1] + 1);
            gTradeMenu->drawSelectedMonState[whichParty] ++;
            break;
        case 4:
            DrawBottomRowText(sActionTexts[5], OBJ_VRAM0 + TILE_SIZE_4BPP * gTradeMenu->bottomTextTileStart, 20);
            PrintLevelAndGender(whichParty, partyIdx, sSelectedMonLevelGenderCoords[whichParty][0] + 4, sSelectedMonLevelGenderCoords[whichParty][1] + 1, sSelectedMonLevelGenderCoords[whichParty][0], sSelectedMonLevelGenderCoords[whichParty][1]);
            gTradeMenu->drawSelectedMonState[whichParty] ++;
            break;
    }
}
#else
NAKED
static void DrawSelectedMonScreen(u8 whichParty)
{
    asm(".syntax unified\n"
"\tpush {r4-r7,lr}\n"
                    "\tmov r7, r10\n"
                    "\tmov r6, r9\n"
                    "\tmov r5, r8\n"
                    "\tpush {r5-r7}\n"
                    "\tsub sp, 0x74\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tmov r10, r0\n"
                    "\tldr r0, _08049F24 @ =gTradeMenu\n"
                    "\tldr r5, [r0]\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0x82\n"
                    "\tadd r0, r10\n"
                    "\tldrb r0, [r0]\n"
                    "\tmovs r1, 0x1\n"
                    "\tstr r1, [sp, 0x6C]\n"
                    "\tcmp r0, %0 @ Last party index\n"
                    "\tbhi _08049EFE\n"
                    "\tmovs r2, 0\n"
                    "\tstr r2, [sp, 0x6C]\n"
                    "_08049EFE:\n"
                    "\tmovs r1, %1 @ PARTY_SIZE\n"
                    "\tbl __umodsi3\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tmov r8, r0\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0x80\n"
                    "\tmov r1, r10\n"
                    "\tadds r3, r0, r1\n"
                    "\tldrb r0, [r3]\n"
                    "\tcmp r0, 0x2\n"
                    "\tbne _08049F1A\n"
                    "\tb _0804A0AC\n"
                    "_08049F1A:\n"
                    "\tcmp r0, 0x2\n"
                    "\tbgt _08049F28\n"
                    "\tcmp r0, 0x1\n"
                    "\tbeq _08049F36\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_08049F24: .4byte gTradeMenu\n"
                    "_08049F28:\n"
                    "\tcmp r0, 0x3\n"
                    "\tbne _08049F2E\n"
                    "\tb _0804A0E4\n"
                    "_08049F2E:\n"
                    "\tcmp r0, 0x4\n"
                    "\tbne _08049F34\n"
                    "\tb _0804A244\n"
                    "_08049F34:\n"
                    "\tb _0804A294\n"
                    "_08049F36:\n"
                    "\tmovs r4, 0\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0x42\n"
                    "\tadd r0, r10\n"
                    "\tldr r7, _0804A094 @ =gSprites\n"
                    "\tldr r2, [sp, 0x6C]\n"
                    "\tlsls r2, 1\n"
                    "\tmov r9, r2\n"
                    "\tldr r3, _0804A098 @ =sTradeMonSpriteCoords\n"
                    "\tmov r12, r3\n"
                    "\tmov r5, r10\n"
                    "\tlsls r5, 4\n"
                    "\tstr r5, [sp, 0x70]\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r4, r0\n"
                    "\tbcs _08049F8E\n"
                    "\tadds r6, r7, 0\n"
                    "\tldr r2, _0804A09C @ =gTradeMenu\n"
                    "\tldr r0, [sp, 0x6C]\n"
                    "\tadd r0, r9\n"
                    "\tlsls r3, r0, 1\n"
                    "\tmovs r5, 0x4\n"
                    "_08049F62:\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r1, r4, r3\n"
                    "\tadds r0, 0x34\n"
                    "\tadds r0, r1\n"
                    "\tldrb r1, [r0]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r6\n"
                    "\tadds r0, 0x3E\n"
                    "\tldrb r1, [r0]\n"
                    "\torrs r1, r5\n"
                    "\tstrb r1, [r0]\n"
                    "\tadds r0, r4, 0x1\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r4, r0, 24\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x42\n"
                    "\tadd r0, r10\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r4, r0\n"
                    "\tbcc _08049F62\n"
                    "_08049F8E:\n"
                    "\tldr r1, _0804A09C @ =gTradeMenu\n"
                    "\tldr r0, [r1]\n"
                    "\tldr r5, [sp, 0x6C]\n"
                    "\tadd r5, r9\n"
                    "\tlsls r3, r5, 1\n"
                    "\tmov r2, r8\n"
                    "\tadds r6, r2, r3\n"
                    "\tadds r0, 0x34\n"
                    "\tadds r0, r6\n"
                    "\tldrb r1, [r0]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tadds r0, 0x3E\n"
                    "\tldrb r2, [r0]\n"
                    "\tmovs r1, 0x5\n"
                    "\tnegs r1, r1\n"
                    "\tands r1, r2\n"
                    "\tstrb r1, [r0]\n"
                    "\tldr r0, _0804A09C @ =gTradeMenu\n"
                    "\tldr r4, [r0]\n"
                    "\tadds r4, 0x34\n"
                    "\tadds r4, r6\n"
                    "\tldrb r1, [r4]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tmovs r1, 0x14\n"
                    "\tstrh r1, [r0, 0x2E]\n"
                    "\tldrb r0, [r4]\n"
                    "\tlsls r2, r0, 4\n"
                    "\tadds r2, r0\n"
                    "\tlsls r2, 2\n"
                    "\tadds r2, r7\n"
                    "\tlsls r5, 2\n"
                    "\tmov r1, r12\n"
                    "\tadds r0, r5, r1\n"
                    "\tldrb r0, [r0]\n"
                    "\tadds r3, 0x1\n"
                    "\tlsls r3, 1\n"
                    "\tadd r3, r12\n"
                    "\tldrb r1, [r3]\n"
                    "\tadds r0, r1\n"
                    "\tasrs r0, 1\n"
                    "\tlsls r0, 3\n"
                    "\tadds r0, 0xE\n"
                    "\tstrh r0, [r2, 0x32]\n"
                    "\tldrb r0, [r4]\n"
                    "\tlsls r1, r0, 4\n"
                    "\tadds r1, r0\n"
                    "\tlsls r1, 2\n"
                    "\tadds r1, r7\n"
                    "\tmov r0, r12\n"
                    "\tadds r0, 0x1\n"
                    "\tadds r5, r0\n"
                    "\tldrb r0, [r5]\n"
                    "\tlsls r0, 3\n"
                    "\tsubs r0, 0xC\n"
                    "\tstrh r0, [r1, 0x36]\n"
                    "\tldrb r1, [r4]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tldr r1, _0804A0A0 @ =SpriteCB_MonIcon\n"
                    "\tbl StoreSpriteCallbackInData6\n"
                    "\tldr r2, _0804A09C @ =gTradeMenu\n"
                    "\tldr r1, [r2]\n"
                    "\tadds r1, 0x80\n"
                    "\tadd r1, r10\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x34\n"
                    "\tadds r0, r6\n"
                    "\tldrb r1, [r0]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tbl Trade_MoveSelectedMonToTarget\n"
                    "\tbl Menu_DestroyCursor\n"
                    "\tldr r3, _0804A09C @ =gTradeMenu\n"
                    "\tldr r0, [r3]\n"
                    "\tadds r0, 0x4\n"
                    "\tldr r1, _0804A0A4 @ =gUnknown_0820C330\n"
                    "\tmov r5, r9\n"
                    "\tadds r2, r5, r1\n"
                    "\tldrb r2, [r2]\n"
                    "\tadds r1, 0x1\n"
                    "\tadd r1, r9\n"
                    "\tldrb r1, [r1]\n"
                    "\tstr r1, [sp]\n"
                    "\tmovs r1, 0x13\n"
                    "\tstr r1, [sp, 0x4]\n"
                    "\tmovs r1, 0\n"
                    "\tmovs r3, 0\n"
                    "\tbl Text_FillWindowRectDefPalette\n"
                    "\tldr r1, _0804A09C @ =gTradeMenu\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0xC8\n"
                    "\tldr r2, [sp, 0x70]\n"
                    "\tmov r3, r10\n"
                    "\tsubs r1, r2, r3\n"
                    "\tlsls r1, 24\n"
                    "\tlsrs r1, 24\n"
                    "\tldr r3, _0804A0A8 @ =sTradePartyBoxTilemap\n"
                    "\tmovs r2, 0xF\n"
                    "\tstr r2, [sp]\n"
                    "\tmovs r2, 0x11\n"
                    "\tstr r2, [sp, 0x4]\n"
                    "\tmovs r5, 0\n"
                    "\tstr r5, [sp, 0x8]\n"
                    "\tmovs r2, 0\n"
                    "\tbl sub_804A96C_alt\n"
                    "\tldr r0, [sp, 0x6C]\n"
                    "\tcmp r0, 0\n"
                    "\tbeq _0804A08C\n"
                    "\tb _0804A294\n"
                    "_0804A08C:\n"
                    "\tbl PrintTradePartnerPartyNicknames\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_0804A094: .4byte gSprites\n"
                    "_0804A098: .4byte sTradeMonSpriteCoords\n"
                    "_0804A09C: .4byte gTradeMenu\n"
                    "_0804A0A0: .4byte SpriteCB_MonIcon\n"
                    "_0804A0A4: .4byte gUnknown_0820C330\n"
                    "_0804A0A8: .4byte sTradePartyBoxTilemap\n"
                    "_0804A0AC:\n"
                    "\tldr r2, _0804A0DC @ =gSprites\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tlsls r0, r1, 1\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 1\n"
                    "\tadd r0, r8\n"
                    "\tadds r1, r5, 0\n"
                    "\tadds r1, 0x34\n"
                    "\tadds r1, r0\n"
                    "\tldrb r1, [r1]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r2, 0x1C\n"
                    "\tadds r0, r2\n"
                    "\tldr r1, [r0]\n"
                    "\tldr r0, _0804A0E0 @ =SpriteCB_MonIcon\n"
                    "\tcmp r1, r0\n"
                    "\tbeq _0804A0D4\n"
                    "\tb _0804A294\n"
                    "_0804A0D4:\n"
                    "\tmovs r0, 0x3\n"
                    "\tstrb r0, [r3]\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_0804A0DC: .4byte gSprites\n"
                    "_0804A0E0: .4byte SpriteCB_MonIcon\n"
                    "_0804A0E4:\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0xC8\n"
                    "\tldr r2, [sp, 0x6C]\n"
                    "\tlsls r1, r2, 4\n"
                    "\tsubs r1, r2\n"
                    "\tlsls r1, 24\n"
                    "\tlsrs r1, 24\n"
                    "\tldr r3, _0804A228 @ =sTradeMovesBoxTilemap\n"
                    "\tmovs r2, 0xF\n"
                    "\tstr r2, [sp]\n"
                    "\tmovs r7, 0x11\n"
                    "\tstr r7, [sp, 0x4]\n"
                    "\tmovs r5, 0\n"
                    "\tstr r5, [sp, 0x8]\n"
                    "\tmovs r2, 0\n"
                    "\tbl sub_804A96C_alt\n"
                    "\tldr r0, _0804A22C @ =gTradeMenu\n"
                    "\tldr r3, [r0]\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tlsls r6, r1, 1\n"
                    "\tadds r1, r6\n"
                    "\tmov r9, r1\n"
                    "\tlsls r1, 1\n"
                    "\tmov r2, r8\n"
                    "\tadds r0, r2, r1\n"
                    "\tadds r3, 0x34\n"
                    "\tadds r3, r0\n"
                    "\tldrb r0, [r3]\n"
                    "\tlsls r2, r0, 4\n"
                    "\tadds r2, r0\n"
                    "\tlsls r2, 2\n"
                    "\tldr r5, _0804A230 @ =gSprites\n"
                    "\tadds r2, r5\n"
                    "\tldr r4, _0804A234 @ =sTradeMonSpriteCoords\n"
                    "\tmov r0, r9\n"
                    "\tlsls r5, r0, 2\n"
                    "\tadds r0, r5, r4\n"
                    "\tldrb r0, [r0]\n"
                    "\tadds r1, 0x1\n"
                    "\tlsls r1, 1\n"
                    "\tadds r1, r4\n"
                    "\tldrb r1, [r1]\n"
                    "\tadds r0, r1\n"
                    "\tasrs r0, 1\n"
                    "\tlsls r0, 3\n"
                    "\tadds r0, 0xE\n"
                    "\tstrh r0, [r2, 0x20]\n"
                    "\tldrb r0, [r3]\n"
                    "\tlsls r1, r0, 4\n"
                    "\tadds r1, r0\n"
                    "\tlsls r1, 2\n"
                    "\tldr r2, _0804A230 @ =gSprites\n"
                    "\tadds r1, r2\n"
                    "\tadds r4, 0x1\n"
                    "\tadds r5, r4\n"
                    "\tldrb r0, [r5]\n"
                    "\tlsls r0, 3\n"
                    "\tsubs r0, 0xC\n"
                    "\tstrh r0, [r1, 0x22]\n"
                    "\tldrb r1, [r3]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r2\n"
                    "\tmovs r5, 0\n"
                    "\tstrh r5, [r0, 0x24]\n"
                    "\tldrb r1, [r3]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r2\n"
                    "\tstrh r5, [r0, 0x26]\n"
                    "\tmov r0, sp\n"
                    "\tadds r0, 0x12\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tmov r2, r8\n"
                    "\tbl GetMonNicknameWidth\n"
                    "\tadd r1, sp, 0xC\n"
                    "\tmovs r3, %2 @ EXT_CTRL_CODE_BEGIN\n"
                    "\tstrb r3, [r1]\n"
                    "\tadds r2, r1, 0\n"
                    "\tmovs r1, %3 @ EXT_CTRL_CODE_FONT\n"
                    "\tstrb r1, [r2, 0x1]\n"
                    "\tmovs r1, 0x4\n"
                    "\tstrb r1, [r2, 0x2]\n"
                    "\tadds r1, r2, 0\n"
                    "\tstrb r3, [r1, 0x3]\n"
                    "\tstrb r7, [r1, 0x4]\n"
                    "\tlsls r0, 24\n"
                    "\tasrs r0, 24\n"
                    "\tmovs r1, 0x40\n"
                    "\tsubs r1, r0\n"
                    "\tlsrs r0, r1, 31\n"
                    "\tadds r1, r0\n"
                    "\tasrs r1, 1\n"
                    "\tstrb r1, [r2, 0x5]\n"
                    "\tldr r0, _0804A22C @ =gTradeMenu\n"
                    "\tldr r1, [r0]\n"
                    "\tadds r0, r1, 0x4\n"
                    "\tadds r1, 0x7A\n"
                    "\tldrb r2, [r1]\n"
                    "\tmov r1, r9\n"
                    "\tlsls r1, 6\n"
                    "\tmov r9, r1\n"
                    "\tadd r2, r9\n"
                    "\tldr r4, _0804A238 @ =gUnknown_0820C334\n"
                    "\tldr r3, [sp, 0x6C]\n"
                    "\tlsls r1, r3, 2\n"
                    "\tadds r3, r1, r4\n"
                    "\tldrb r3, [r3]\n"
                    "\tldr r5, _0804A23C @ =gUnknown_0820C334 + 0x1\n"
                    "\tadds r1, r5\n"
                    "\tldrb r1, [r1]\n"
                    "\tstr r1, [sp]\n"
                    "\tadd r1, sp, 0xC\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tadd r7, sp, 0x34\n"
                    "\tadds r0, r7, 0\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tmov r2, r8\n"
                    "\tbl BufferMovesString\n"
                    "\tldr r0, _0804A22C @ =gTradeMenu\n"
                    "\tldr r2, [r0]\n"
                    "\tadds r0, r2, 0x4\n"
                    "\tldr r1, _0804A240 @ =gOtherText_Terminator2\n"
                    "\tadds r2, 0x7A\n"
                    "\tldrb r2, [r2]\n"
                    "\tadd r2, r9\n"
                    "\tadds r2, 0x20\n"
                    "\tadds r6, 0x1\n"
                    "\tlsls r6, 1\n"
                    "\tadds r4, r6, r4\n"
                    "\tldrb r5, [r4]\n"
                    "\tldr r3, _0804A23C @ =gUnknown_0820C334 + 0x1\n"
                    "\tadds r6, r3\n"
                    "\tldrb r4, [r6]\n"
                    "\tadds r4, 0x1\n"
                    "\tlsls r4, 24\n"
                    "\tlsrs r4, 24\n"
                    "\tstr r4, [sp]\n"
                    "\tadds r3, r5, 0\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tldr r0, _0804A22C @ =gTradeMenu\n"
                    "\tldr r1, [r0]\n"
                    "\tadds r0, r1, 0x4\n"
                    "\tadds r1, 0x7A\n"
                    "\tldrb r2, [r1]\n"
                    "\tadd r2, r9\n"
                    "\tadds r2, 0x26\n"
                    "\tstr r4, [sp]\n"
                    "\tadds r1, r7, 0\n"
                    "\tadds r3, r5, 0\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tldr r2, _0804A22C @ =gTradeMenu\n"
                    "\tldr r1, [r2]\n"
                    "\tb _0804A28A\n"
                    "\t.align 2, 0\n"
                    "_0804A228: .4byte sTradeMovesBoxTilemap\n"
                    "_0804A22C: .4byte gTradeMenu\n"
                    "_0804A230: .4byte gSprites\n"
                    "_0804A234: .4byte sTradeMonSpriteCoords\n"
                    "_0804A238: .4byte gUnknown_0820C334\n"
                    "_0804A23C: .4byte gUnknown_0820C334 + 0x1\n"
                    "_0804A240: .4byte gOtherText_Terminator2\n"
                    "_0804A244:\n"
                    "\tldr r0, _0804A2A4 @ =sActionTexts\n"
                    "\tldr r0, [r0, 0x14]\n"
                    "\tadds r1, r5, 0\n"
                    "\tadds r1, 0x7E\n"
                    "\tldrh r1, [r1]\n"
                    "\tlsls r1, 5\n"
                    "\tldr r3, _0804A2A8 @ =0x06010000\n"
                    "\tadds r1, r3\n"
                    "\tmovs r2, 0x14\n"
                    "\tbl DrawBottomRowText\n"
                    "\tldr r0, _0804A2AC @ =sSelectedMonLevelGenderCoords\n"
                    "\tmov r5, r10\n"
                    "\tlsls r1, r5, 1\n"
                    "\tadds r4, r1, r0\n"
                    "\tldrb r2, [r4]\n"
                    "\tadds r2, 0x4\n"
                    "\tlsls r2, 24\n"
                    "\tlsrs r2, 24\n"
                    "\tadds r0, 0x1\n"
                    "\tadds r1, r0\n"
                    "\tldrb r3, [r1]\n"
                    "\tadds r3, 0x1\n"
                    "\tlsls r3, 24\n"
                    "\tlsrs r3, 24\n"
                    "\tldrb r0, [r4]\n"
                    "\tstr r0, [sp]\n"
                    "\tldrb r0, [r1]\n"
                    "\tstr r0, [sp, 0x4]\n"
                    "\tmov r0, r10\n"
                    "\tmov r1, r8\n"
                    "\tbl PrintLevelAndGender\n"
                    "\tldr r0, _0804A2B0 @ =gTradeMenu\n"
                    "\tldr r1, [r0]\n"
                    "_0804A28A:\n"
                    "\tadds r1, 0x80\n"
                    "\tadd r1, r10\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "_0804A294:\n"
                    "\tadd sp, 0x74\n"
                    "\tpop {r3-r5}\n"
                    "\tmov r8, r3\n"
                    "\tmov r9, r4\n"
                    "\tmov r10, r5\n"
                    "\tpop {r4-r7}\n"
                    "\tpop {r0}\n"
                    "\tbx r0\n"
                    "\t.align 2, 0\n"
                    "_0804A2A4: .4byte sActionTexts\n"
                    "_0804A2A8: .4byte %a4 @ OBJ_VRAM0\n"
                    "_0804A2AC: .4byte sSelectedMonLevelGenderCoords\n"
                    "_0804A2B0: .4byte gTradeMenu"
                    "\n.syntax divided\n"
                    :
                    : "i"(PARTY_SIZE - 1),
                      "i"(PARTY_SIZE),
                      "i"(EXT_CTRL_CODE_BEGIN),
                      "i"(EXT_CTRL_CODE_FONT),
                      "i"(OBJ_VRAM0));
}
#endif

u8 GetMonNicknameWidth(u8 *str, u8 whichParty, u8 partyIdx)
{
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    if (whichParty == TRADE_PLAYER)
    {
        GetMonData(&gPlayerParty[partyIdx], MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(str, nickname);
        GetMonGender(&gPlayerParty[partyIdx]);
        GetMonData(&gPlayerParty[partyIdx], MON_DATA_LEVEL);
    }
    else
    {
        GetMonData(&gEnemyParty[partyIdx], MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(str, nickname);
        GetMonGender(&gEnemyParty[partyIdx]);
        GetMonData(&gEnemyParty[partyIdx], MON_DATA_LEVEL);
    }
    return Text_GetStringWidthFromWindowTemplate(&gWindowTemplate_81E7294, str);
}

static void UNUSED BufferMovesString(u8 *str, u8 whichParty, u8 partyIdx)
{
    u16 i;
    u16 moves[MAX_MON_MOVES];
    if (gTradeMenu->isEgg[whichParty][partyIdx] == FALSE)
    {
        for (i = 0; i < 4; i ++)
        {
            if (whichParty == TRADE_PLAYER)
                moves[i] = GetMonData(&gPlayerParty[partyIdx], MON_DATA_MOVE1 + i, NULL);
            else
                moves[i] = GetMonData(&gEnemyParty[partyIdx], MON_DATA_MOVE1 + i, NULL);
        }
        StringCopy(str, sText_EmptyString);
        for (i = 0; i < 4; i ++)
        {
            if (moves[i] != 0)
            {
                StringAppend(str, gMoveNames[moves[i]]);
            }
            StringAppend(str, sText_NewLine);
        }
    }
    else
    {
        StringCopy(str, sText_EmptyString);
        StringAppend(str, sText_FourQuestionMarks);
    }
}

#ifdef NONMATCHING
static void PrintPartyNicknames(u8 whichParty)
{
    u8 i;
    u8 nickname[22];
    u8 str[40];
    struct Pokemon *pokemon;

    str[0] = EXT_CTRL_CODE_BEGIN;
    str[1] = EXT_CTRL_CODE_FONT;
    str[2] = 0x04;
    str[3] = EXT_CTRL_CODE_BEGIN;
    str[4] = EXT_CTRL_CODE_CLEAR;
    str[5] = 0x00;

    for (i = 0; i < gTradeMenu->partyCounts[whichParty]; i ++)
    {
        pokemon = whichParty == TRADE_PLAYER ? &gPlayerParty[i] : &gEnemyParty[i];
        GetMonData(pokemon, MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(str + 6, nickname);
        GetMonGender(pokemon);
        str[5] = (50 - Text_GetStringWidthFromWindowTemplate(&gWindowTemplate_81E7294, str + 6)) / 2;
        Text_InitWindowAndPrintText(&gTradeMenu->window, str, gTradeMenu->unk_007a + 22 * PARTY_SIZE * whichParty + 22 * i, sTradeMonSpriteCoords[i + PARTY_SIZE * whichParty][0], sTradeMonSpriteCoords[i + PARTY_SIZE * whichParty][1]);
    }
}
#else
NAKED
static void PrintPartyNicknames(u8 whichParty)
{
    asm(".syntax unified\n"
"\tpush {r4-r7,lr}\n"
                    "\tmov r7, r10\n"
                    "\tmov r6, r9\n"
                    "\tmov r5, r8\n"
                    "\tpush {r5-r7}\n"
                    "\tsub sp, 0x44\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r5, r0, 24\n"
                    "\tadd r3, sp, 0x1C\n"
                    "\tmovs r2, 0\n"
                    "\tmovs r1, %0 @ EXT_CTRL_CODE_BEGIN\n"
                    "\tstrb r1, [r3]\n"
                    "\tmovs r0, %1 @ EXT_CTRL_CODE_FONT\n"
                    "\tstrb r0, [r3, 0x1]\n"
                    "\tmovs r0, 0x4\n"
                    "\tstrb r0, [r3, 0x2]\n"
                    "\tstrb r1, [r3, 0x3]\n"
                    "\tmovs r0, %2 @ EXT_CTRL_CODE_CLEAR\n"
                    "\tstrb r0, [r3, 0x4]\n"
                    "\tstrb r2, [r3, 0x5]\n"
                    "\tmovs r6, 0\n"
                    "\tldr r1, _0804A470 @ =gTradeMenu\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0x42\n"
                    "\tadds r0, r5\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r6, r0\n"
                    "\tbcs _0804A504\n"
                    "\tmov r8, r3\n"
                    "\tadds r7, r1, 0\n"
                    "\tldr r0, _0804A474 @ =sTradeMonSpriteCoords\n"
                    "\tmov r9, r0\n"
                    "\tmovs r4, 0x1\n"
                    "\tadd r4, r9\n"
                    "\tmov r10, r4\n"
                    "_0804A462:\n"
                    "\tcmp r5, 0\n"
                    "\tbne _0804A47C\n"
                    "\tmovs r0, %3 @ sizeof(struct Pokemon)\n"
                    "\tadds r4, r6, 0\n"
                    "\tmuls r4, r0\n"
                    "\tldr r0, _0804A478 @ =gPlayerParty\n"
                    "\tb _0804A484\n"
                    "\t.align 2, 0\n"
                    "_0804A470: .4byte gTradeMenu\n"
                    "_0804A474: .4byte sTradeMonSpriteCoords\n"
                    "_0804A478: .4byte gPlayerParty\n"
                    "_0804A47C:\n"
                    "\tmovs r0, %3 @ sizeof(struct Pokemon)\n"
                    "\tadds r4, r6, 0\n"
                    "\tmuls r4, r0\n"
                    "\tldr r0, _0804A514 @ =gEnemyParty\n"
                    "_0804A484:\n"
                    "\tadds r4, r0\n"
                    "\tadds r0, r4, 0\n"
                    "\tmovs r1, %4 @ MON_DATA_NICKNAME\n"
                    "\tadd r2, sp, 0x4\n"
                    "\tbl GetMonData\n"
                    "\tmov r0, sp\n"
                    "\tadds r0, 0x22\n"
                    "\tadd r1, sp, 0x4\n"
                    "\tbl StringCopy_Nickname\n"
                    "\tadds r0, r4, 0\n"
                    "\tbl GetMonGender\n"
                    "\tmov r1, sp\n"
                    "\tadds r1, 0x22\n"
                    "\tldr r0, _0804A518 @ =gWindowTemplate_81E7294\n"
                    "\tbl Text_GetStringWidthFromWindowTemplate\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tmovs r1, 0x32\n"
                    "\tsubs r1, r0\n"
                    "\tlsrs r0, r1, 31\n"
                    "\tadds r1, r0\n"
                    "\tasrs r1, 1\n"
                    "\tmov r0, r8\n"
                    "\tstrb r1, [r0, 0x5]\n"
                    "\tldr r1, [r7]\n"
                    "\tadds r0, r1, 0x4\n"
                    "\tadds r1, 0x7A\n"
                    "\tlsls r2, r5, 5\n"
                    "\tadds r2, r5\n"
                    "\tlsls r2, 2\n"
                    "\tldrb r1, [r1]\n"
                    "\tadds r2, r1\n"
                    "\tmovs r1, 0x16\n"
                    "\tmuls r1, r6\n"
                    "\tadds r2, r1\n"
                    "\tlsls r2, 16\n"
                    "\tlsrs r2, 16\n"
                    "\tlsls r1, r5, 1\n"
                    "\tadds r1, r5\n"
                    "\tlsls r1, 1\n"
                    "\tadds r1, r6, r1\n"
                    "\tlsls r1, 1\n"
                    "\tmov r4, r9\n"
                    "\tadds r3, r1, r4\n"
                    "\tldrb r3, [r3]\n"
                    "\tadd r1, r10\n"
                    "\tldrb r1, [r1]\n"
                    "\tstr r1, [sp]\n"
                    "\tmov r1, r8\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tadds r0, r6, 0x1\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r6, r0, 24\n"
                    "\tldr r0, [r7]\n"
                    "\tadds r0, 0x42\n"
                    "\tadds r0, r5\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r6, r0\n"
                    "\tbcc _0804A462\n"
                    "_0804A504:\n"
                    "\tadd sp, 0x44\n"
                    "\tpop {r3-r5}\n"
                    "\tmov r8, r3\n"
                    "\tmov r9, r4\n"
                    "\tmov r10, r5\n"
                    "\tpop {r4-r7}\n"
                    "\tpop {r0}\n"
                    "\tbx r0\n"
                    "\t.align 2, 0\n"
                    "_0804A514: .4byte gEnemyParty\n"
                    "_0804A518: .4byte gWindowTemplate_81E7294"
                    "\n.syntax divided\n"
                    :
                    : "i"(EXT_CTRL_CODE_BEGIN),
                      "i"(EXT_CTRL_CODE_FONT),
                      "i"(EXT_CTRL_CODE_CLEAR),
                      "i"(sizeof(struct Pokemon)),
                      "i"(MON_DATA_NICKNAME));
}
#endif

void PrintLevelAndGender(u8 whichParty, u8 monIdx, u8 x, u8 y, u8 width, u8 height)
{
    u8 nickname[24];
    u8 level;
    u8 gender;
    u8 lv_div_10;
    sub_804A96C(&gTradeMenu->unk_00c8, width, height, gTradeMenuMonBox_Tilemap, 6, 3, 0);
    if (whichParty == TRADE_PLAYER)
    {
        level = GetMonData(&gPlayerParty[monIdx], MON_DATA_LEVEL, NULL);
        gender = GetMonGender(&gPlayerParty[monIdx]);
        GetMonData(&gPlayerParty[monIdx], MON_DATA_NICKNAME, nickname);
    }
    else
    {
        level = GetMonData(&gEnemyParty[monIdx], MON_DATA_LEVEL, NULL);
        gender = GetMonGender(&gEnemyParty[monIdx]);
        GetMonData(&gEnemyParty[monIdx], MON_DATA_NICKNAME, nickname);
    }
    if (gTradeMenu->isEgg[whichParty][monIdx] == FALSE)
    {
        lv_div_10 = level / 10;
        if (lv_div_10 != 0)
        {
            gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y] = lv_div_10 + 0x60;
        }
        gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y + 1] = (u8)(level % 10) + 0x70;
        if (gender == MON_MALE)
        {
            if (!NameHasGenderSymbol(nickname, MON_MALE))
            {
                gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y - 31] += 1;
            }
        }
        else if (gender == MON_FEMALE)
        {
            if (!NameHasGenderSymbol(nickname, MON_FEMALE))
            {
                gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y - 31] += 2;
            }
        }
    }
    else
    {
        gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y - 32] = gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y - 33];
        gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y - 31] = gTradeMenu->unk_00c8.tilemapBuffer[x + 32 * y - 36] | 0x400;
    }
#ifdef GERMAN
    gTradeMenu->unk_00c8.unk_10 = TRUE;
#endif
}

static void PrintPartyLevelsAndGenders(u8 whichParty)
{
    s32 i;
    for (i = 0; i < gTradeMenu->partyCounts[whichParty]; i++)
    {
        s32 j = i + whichParty * PARTY_SIZE;
        PrintLevelAndGender(whichParty, i, sTradeMonLevelCoords[j][0], sTradeMonLevelCoords[j][1], sTradeMonBoxCoords[j][0], sTradeMonBoxCoords[j][1]);
    }
}

static void ShowTradePartyMonIcons(u8 whichParty)
{
    int i;
    for (i = 0; i < gTradeMenu->partyCounts[whichParty]; i ++)
    {
        gSprites[gTradeMenu->partySpriteIds[whichParty][i]].invisible = FALSE;
        gSprites[gTradeMenu->partySpriteIds[whichParty][i]].x = sTradeMonSpriteCoords[PARTY_SIZE * whichParty + i][0] * 8 + 14;
        gSprites[gTradeMenu->partySpriteIds[whichParty][i]].y = sTradeMonSpriteCoords[PARTY_SIZE * whichParty + i][1] * 8 - 12;
        gSprites[gTradeMenu->partySpriteIds[whichParty][i]].x2 = 0;
        gSprites[gTradeMenu->partySpriteIds[whichParty][i]].y2 = 0;
    }
}

static void PrintTradePartnerPartyNicknames(void)
{
    Text_FillWindowRectDefPalette(&gTradeMenu->window, 0, gUnknown_0820C330[1][0], 0, gUnknown_0820C330[1][1], 19);
    PrintPartyNicknames(TRADE_PARTNER);
}

static void RedrawPartyWindow(u8 whichParty)
{
    if (whichParty == TRADE_PLAYER)
    {
        Text_FillWindowRectDefPalette(&gTradeMenu->window, 0, gUnknown_0820C330[0][0], 0, gUnknown_0820C330[0][1], 19);
        sub_804A96C_alt(&gTradeMenu->unk_00c8, 0, 0, sTradePartyBoxTilemap, 15, 17, 0);
        PrintPartyLevelsAndGenders(TRADE_PLAYER);
        PrintPartyNicknames(TRADE_PLAYER);
        ShowTradePartyMonIcons(TRADE_PLAYER);
        sub_804A938(&gTradeMenu->unk_00c8);
    }
    else
    {
        Menu_DestroyCursor();
        Text_FillWindowRectDefPalette(&gTradeMenu->window, 0, gUnknown_0820C330[1][0], 0, gUnknown_0820C330[1][1], 19);
        sub_804A96C_alt(&gTradeMenu->unk_00c8, 15, 0, sTradePartyBoxTilemap, 15, 17, 0);
        PrintPartyLevelsAndGenders(TRADE_PARTNER);
        PrintPartyNicknames(TRADE_PARTNER);
        ShowTradePartyMonIcons(TRADE_PARTNER);
        sub_804A938(&gTradeMenu->unk_00c8);
    }
    DrawBottomRowText(sActionTexts[TEXT_CHOOSE_MON], OBJ_VRAM0 + TILE_SIZE_4BPP * gTradeMenu->bottomTextTileStart, 20);
    gTradeMenu->drawSelectedMonState[whichParty] = 0;
}

static void sub_804A938(struct UnkStructD *unkStructD)
{
    unkStructD->unk_10 = TRUE;
}

static void sub_804A940(struct UnkStructD *unkStructD)
{
    if (unkStructD->unk_10)
    {
        CpuCopy16(unkStructD->tilemapBuffer, unkStructD->vramAddr, sizeof(unkStructD->tilemapBuffer));
        unkStructD->unk_10 = FALSE;
    }
}

static void sub_804A964(struct UnkStructD *unkStructD, void *dest)
{
    unkStructD->unk_10 = FALSE;
    unkStructD->vramAddr = dest;
}

static void sub_804A96C(struct UnkStructD *arg0, u8 destX, u8 destY, const u16 *tilemap, u8 rectWidth, u8 rectHeight, u16 sp8) {
    int y, x;

    for (y = 0; y < rectHeight; y++)
    {

        for (x = 0; x < rectWidth; x++)
        {
            arg0->tilemapBuffer[(destY * 32 + destX) + y * 32 + x] = tilemap[rectWidth * y + x] | sp8;
        }
    }

#if ENGLISH
    arg0->unk_10 = TRUE;
#endif
}

#if GERMAN
static void sub_804A96C_alt(struct UnkStructD *arg0, u8 destX, u8 destY, const u16 *tilemap, u8 rectWidth, u8 rectHeight, u16 sp8) {
    sub_804A96C(arg0, destX, destY, tilemap, rectWidth, rectHeight, sp8);

    arg0->unk_10 = TRUE;
}
#endif

static void Task_DrawSelectionSummary(u8 unused)
{
    Menu_EraseScreen();
}

static void Task_DrawSelectionTrade(u8 unused)
{
    Menu_EraseScreen();
}

static void PrintTradeMessage(u8 messageId)
{
    TextWindow_DrawStdFrame(&gTradeMenu->window, gTradeMessageWindowRects[messageId][0], gTradeMessageWindowRects[messageId][1], gTradeMessageWindowRects[messageId][2], gTradeMessageWindowRects[messageId][3]);
    Text_InitWindowAndPrintText(&gTradeMenu->window, sMessages[messageId], 0x180 + gTradeMenu->unk_007a, gTradeMessageWindowRects[messageId][0] + 1, gTradeMessageWindowRects[messageId][1] + 1);
}

static void sub_804AA88(void)
{
    int i;
    for (i = 0; i < (int)ARRAY_COUNT(gTradeMenu->queuedActions); i ++)
    {
        gTradeMenu->queuedActions[i].active = FALSE;
        gTradeMenu->queuedActions[i].delay = 0;
        gTradeMenu->queuedActions[i].actionId |= 0xff;
    }
}

static void QueueAction(u16 delay, u8 actionId)
{
    int i;
    for (i = 0; i < (int)ARRAY_COUNT(gTradeMenu->queuedActions); i ++)
    {
        if (gTradeMenu->queuedActions[i].active == FALSE)
        {
            gTradeMenu->queuedActions[i].delay = delay;
            gTradeMenu->queuedActions[i].actionId = actionId;
            gTradeMenu->queuedActions[i].active = TRUE;
            break;
        }
    }
}

static void DoQueuedActions(void)
{
    int i;
    for (i = 0; i < (int)ARRAY_COUNT(gTradeMenu->queuedActions); i ++)
    {
        if (gTradeMenu->queuedActions[i].active)
        {
            if (gTradeMenu->queuedActions[i].delay != 0)
            {
                gTradeMenu->queuedActions[i].delay --;
                continue;
            }
            switch (gTradeMenu->queuedActions[i].actionId)
            {
                case QUEUE_SEND_DATA:
                    Trade_SendData(gTradeMenu);
                    break;
                case QUEUE_STANDBY:
                    PrintTradeMessage(MSG_STANDBY);
                    break;
                case QUEUE_ONLY_MON1:
                    PrintTradeMessage(MSG_ONLY_MON1);
                    break;
                case 3:
                    PrintTradeMessage(3);
                    break;
                case 4:
                    PrintTradeMessage(3);
                    break;
                case 5:
                    PrintTradeMessage(3);
                    break;
            }
            gTradeMenu->queuedActions[i].active = FALSE;
        }
    }
}

static bool8 LoadUISpriteGfx(void)
{
    switch (gTradeMenu->timer)
    {
        case GFXTAG_CHOOSE_PKMN_L:
            gTradeMenu->bottomTextTileStart = LoadSpriteSheet(&gUnknown_0820C07C[gTradeMenu->timer]);
            gTradeMenu->timer ++;
            return FALSE;
        case NUM_MENU_TEXT_SPRITES:
            LoadSpritePalette(&sSpritePalette_MenuText);
            gTradeMenu->timer ++;
            return FALSE;
        case NUM_MENU_TEXT_SPRITES + 1:
            LoadSpritePalette(&sCursor_SpritePalette);
            gTradeMenu->timer ++;
            return FALSE;
        case NUM_MENU_TEXT_SPRITES + 2:
            LoadSpriteSheet(&sCursor_SpriteSheet);
            gTradeMenu->timer ++;
            // fallthrough
        case NUM_MENU_TEXT_SPRITES + 3:
            gTradeMenu->timer = 0;
            return TRUE;
        default:
            LoadSpriteSheet(&gUnknown_0820C07C[gTradeMenu->timer]);
            gTradeMenu->timer ++;
            return FALSE;
    }
}

static void DrawBottomRowText(const u8 *str, u8 *dest, u8 a2)
{
    sub_804AFB8(&gWindowTemplate_81E725C, dest, str, a2);
}

// replacing [who] with [0]/[1] depending on case compiles better code
static void ComputePartyTradeableFlags(u8 whichParty)
{
    struct Pokemon *pokemon;
    int i;
    
    switch (whichParty)
    {
        case TRADE_PLAYER:
            for (i = 0; i < gTradeMenu->partyCounts[whichParty]; i++)
            {
                pokemon = &gPlayerParty[i];
                if (GetMonData(pokemon, MON_DATA_IS_EGG) == TRUE)
                {
                    gTradeMenu->isLiveMon[whichParty][i] = FALSE;
                    gTradeMenu->isEgg[whichParty][i] = TRUE;
                }
                else if (GetMonData(pokemon, MON_DATA_HP) == 0)
                {
                    gTradeMenu->isLiveMon[whichParty][i] = FALSE;
                    gTradeMenu->isEgg[whichParty][i] = FALSE;
                }
                else
                {
                    gTradeMenu->isLiveMon[whichParty][i] = TRUE;
                    gTradeMenu->isEgg[whichParty][i] = FALSE;
                }
            }
            break;
        case TRADE_PARTNER:
            for (i = 0; i < gTradeMenu->partyCounts[whichParty]; i++)
            {
                pokemon = &gEnemyParty[i];
                if (GetMonData(pokemon, MON_DATA_IS_EGG) == TRUE)
                {
                    gTradeMenu->isLiveMon[whichParty][i] = FALSE;
                    gTradeMenu->isEgg[whichParty][i] = TRUE;
                }
                else if (GetMonData(pokemon, MON_DATA_HP) == 0)
                {
                    gTradeMenu->isLiveMon[whichParty][i] = FALSE;
                    gTradeMenu->isEgg[whichParty][i] = FALSE;
                }
                else
                {
                    gTradeMenu->isLiveMon[whichParty][i] = TRUE;
                    gTradeMenu->isEgg[whichParty][i] = FALSE;
                }
            }
            break;
    }
}

static void ComputePartyHPBarLevels(u8 whichParty)
{
    u16 i;
    u16 curHp;
    u16 maxHp;
    switch (whichParty)
    {
        case TRADE_PLAYER:
            for (i = 0; i < gTradeMenu->partyCounts[TRADE_PLAYER]; i ++)
            {
                curHp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
                maxHp = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
                gTradeMenu->hpBarLevels[TRADE_PLAYER][i] = GetHPBarLevel(curHp, maxHp);
            }
            break;
        case TRADE_PARTNER:
            for (i = 0; i < gTradeMenu->partyCounts[TRADE_PARTNER]; i ++)
            {
                curHp = GetMonData(&gEnemyParty[i], MON_DATA_HP);
                maxHp = GetMonData(&gEnemyParty[i], MON_DATA_MAX_HP);
                gTradeMenu->hpBarLevels[TRADE_PARTNER][i] = GetHPBarLevel(curHp, maxHp);
            }
            break;
    }
}

static void SetTradePartyHPBarSprites(void)
{
    int i, j;
    for (i = 0; i < 2; i ++)
    {
        for (j = 0; j < gTradeMenu->partyCounts[i]; j ++)
        {
            SetPartyHPBarSprite(&gSprites[gTradeMenu->partySpriteIds[i][j]], HP_BAR_FULL - gTradeMenu->hpBarLevels[i][j]);
        }
    }
}

static void SaveTradeGiftRibbons(void)
{
    int i;
    for (i = 0; i < (int)ARRAY_COUNT(gTradeMenu->giftRibbons); i ++)
        if (gSaveBlock1.giftRibbons[i] == 0 && gTradeMenu->giftRibbons[i] != 0)
            gSaveBlock1.giftRibbons[i] = gTradeMenu->giftRibbons[i];
}

static void sub_804AFB8(const struct WindowTemplate *windowConfig, u8 *dest, const u8 *src, u8 size)
{
    u8 i;
    u8 *tileBuffer;
    size = (size + 3) / 4;
    tileBuffer = gTileBuffer;
    CpuFill16(0, tileBuffer, size * 4 * TILE_SIZE_4BPP);
    CpuFill16(0, tileBuffer + windowConfig->width * TILE_SIZE_4BPP, size * 4 * TILE_SIZE_4BPP);
    Text_InitWindow8004E3C(windowConfig, tileBuffer, src);
    for (i = 0; i < size; i ++)
    {
        CpuCopy16(&tileBuffer[TILE_SIZE_4BPP * (i * 4)], &dest[TILE_SIZE_4BPP * (i * 8)], 4 * TILE_SIZE_4BPP);
        CpuCopy16(&tileBuffer[TILE_SIZE_4BPP * (i * 4 + windowConfig->width)], &dest[TILE_SIZE_4BPP * (i * 8 + 4)], 4 * TILE_SIZE_4BPP);
    }
}

static void SpriteCB_LinkMonGlow(struct Sprite *sprite)
{
    if (++ sprite->data[0] == 10)
    {
        PlaySE(SE_BALL);
        sprite->data[0] = 0;
    }
}

static void SpriteCB_LinkMonShadow(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        if (++ sprite->data[0] == 12)
            sprite->data[0] = 0;
        LoadPalette(&sLinkMonShadow_Pal[sprite->data[0]], OBJ_PLTT_ID2(sprite->oam.paletteNum) + 4, PLTT_SIZEOF(1));
    }
}

static void SpriteCB_CableEndSending(struct Sprite *sprite)
{
    sprite->data[0] ++;
    sprite->y2 ++;
    if (sprite->data[0] == 10)
        DestroySprite(sprite);
}

static void SpriteCB_CableEndReceiving(struct Sprite *sprite)
{
    sprite->data[0] ++;
    sprite->y2 --;
    if (sprite->data[0] == 10)
        DestroySprite(sprite);
}

static void SpriteCB_GbaScreen(struct Sprite *sprite)
{
    if (++ sprite->data[0] == 15)
    {
        PlaySE(SE_M_MINIMIZE);
        sprite->data[0] = 0;
    }
}

static void SetTradeBGAffine(void)
{
    struct BgAffineDstData affine;
    DoBgAffineSet(&affine, gTradeAnim->texX << 8, gTradeAnim->texY << 8, gTradeAnim->scrX, gTradeAnim->scrY, gTradeAnim->sXY, gTradeAnim->sXY, gTradeAnim->alpha);
    REG_BG2PA = affine.pa;
    REG_BG2PB = affine.pb;
    REG_BG2PC = affine.pc;
    REG_BG2PD = affine.pd;
    REG_BG2X = affine.dx;
    REG_BG2Y = affine.dy;
}

static void SetTradeGpuRegs(void)
{
    u16 dispcnt;

    REG_BG1VOFS = gTradeAnim->bg1vofs;
    REG_BG1HOFS = gTradeAnim->bg1hofs;

    /*
        A u16 cast allows for REG_DISPCNT storage to be swapped.
        This is required for the function to match.

        You can see this less obfuscated in FireRed and Emerald,
        since they use gflib's GPU manager for this instead.
    */
    dispcnt = (*(u16 *)REG_ADDR_DISPCNT);
    if ((dispcnt & 7) == DISPCNT_MODE_0)
    {
        REG_BG2VOFS = gTradeAnim->bg2vofs;
        REG_BG2HOFS = gTradeAnim->bg2hofs;
    }
    else
    {
        SetTradeBGAffine();
    }
}

static void VBlankCB_TradeAnim(void)
{
    SetTradeGpuRegs();
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void ClearLinkTimeoutTimer(void)
{
    gTradeAnim->linkTimeoutTimer = 0;
    gTradeAnim->linkTimeoutZero1 = 0;
    gTradeAnim->linkTimeoutZero2 = 0;
}

void CheckForLinkTimeout(void)
{
    if (gTradeAnim->linkTimeoutZero1 == gTradeAnim->linkTimeoutZero2)
    {
        gTradeAnim->linkTimeoutTimer ++;
    }
    else
    {
        gTradeAnim->linkTimeoutTimer = 0;
    }
    if (gTradeAnim->linkTimeoutTimer > 0xb4)
    {
        gTradeAnim->linkTimeoutTimer = 0;
        gTradeAnim->linkTimeoutZero2 = 0;
        gTradeAnim->linkTimeoutZero1 = 0;
    }
    gTradeAnim->linkTimeoutZero2 = gTradeAnim->linkTimeoutZero1;
}

static u8 TradeGetMultiplayerId(void)
{
    if (gReceivedRemoteLinkPlayers)
        return GetMultiplayerId();
    return 0;
}

static void LoadTradeMonPic(u8 whichParty, u8 state)
{
    u8 pos;
    struct Pokemon *mon;
    u16 species;
    u32 personality;

    pos = 0;
    mon = NULL;
    if (whichParty == TRADE_PLAYER)
    {
        mon = &gPlayerParty[gSelectedTradeMonPositions[TRADE_PLAYER]];
        pos = B_POSITION_OPPONENT_LEFT;
    }
    if (whichParty == TRADE_PARTNER)
    {
        mon = &gEnemyParty[gSelectedTradeMonPositions[TRADE_PARTNER] % PARTY_SIZE];
        pos = B_POSITION_OPPONENT_RIGHT;
    }
    switch (state)
    {
        case 0:
            species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
            personality = GetMonData(mon, MON_DATA_PERSONALITY);
            HandleLoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, gSharedMem, gMonSpriteGfx_Sprite_ptr[whichParty * 2 + B_POSITION_OPPONENT_LEFT], species, personality);
            LoadCompressedSpritePalette(GetMonSpritePalStruct(mon));
            gTradeAnim->monSpecies[whichParty] = species;
            break;
        case 1:
            SetMultiuseSpriteTemplateToPokemon(GetMonSpritePalStruct(mon)->tag, pos);
            gTradeAnim->monSpriteIds[whichParty] = CreateSprite(&gMultiuseSpriteTemplate, 0x78, 0x3c, 0x6);
            gSprites[gTradeAnim->monSpriteIds[whichParty]].invisible = TRUE;
            gSprites[gTradeAnim->monSpriteIds[whichParty]].callback = SpriteCallbackDummy;
            break;
    }
}

// non-shifting nonmatch
// out of order statements + register swaps?
// r4 is loaded with 0 way before it's used (and it should be r2)
#ifdef NONMATCHING
void CB2_LinkTrade(void)
// Link trade init
{
    switch (gMain.state)
    {
        case 0:
            REG_DISPCNT = 0;
            ResetTasks();
            CloseLink();
            gTradeAnim = &ewram_2010000.unk_0f000;
            ResetSpriteData();
            FreeAllSpritePalettes();
            SetVBlankCallback(VBlankCB_TradeAnim);
            ClearLinkTimeoutTimer();
            Text_LoadWindowTemplate(&gWindowTemplate_81E6F84);
            Text_InitWindowWithTemplate(&gTradeAnim->window, &gWindowTemplate_81E6F84);
            gTradeAnim->textWindowBaseTileNum = TextWindow_SetBaseTileNum(2);
            //
            // start of nonmatching here?
            TextWindow_LoadStdFrameGraphics(&gTradeAnim->window);
            Menu_EraseScreen();
            gLinkType = LINKTYPE_TRADE_DISCONNECTED;
            gMain.state++;
            LZDecompressVram(gBattleTextboxTiles, (void *)VRAM);
            CpuCopy16(gBattleTextboxTilemap, gSharedMem, 0x1000);
            DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
            LoadCompressedPalette(gBattleTextboxPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
            gTradeAnim->unk_00b6 = 0; // nonmatching code writes to this address with r4, as opposed to r2 in matching code. r4 is also loaded with zero way before
            gTradeAnim->state = STATE_START;
            gTradeAnim->isLinkTrade = TRUE;
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x40;
            gTradeAnim->unk_0108 = 0;
            gTradeAnim->unk_010a = 0;
            gTradeAnim->scrX = DISPLAY_WIDTH / 2;
            gTradeAnim->scrY = DISPLAY_HEIGHT / 2;
            gTradeAnim->sXY = Q_8_8(1);
            gTradeAnim->alpha = 0;
            break;
        case 1:
            OpenLink();
            gMain.state ++;
            gTradeAnim->timer = 0;
            break;
        case 2:
            if (++ gTradeAnim->timer > 60)
            {
                gTradeAnim->timer = 0;
                gMain.state ++;
            }
            break;
        case 3:
            if (IsLinkMaster())
            {
                if (GetLinkPlayerCount_2() >= GetSavedPlayerCount() && ++ gTradeAnim->timer > 30)
                {
                    CheckShouldAdvanceLinkState();
                    gMain.state ++;
                }
            }
            else
            {
                gMain.state ++;
            }
            break;
        case 4:
            CheckForLinkTimeout();
            if (gReceivedRemoteLinkPlayers == TRUE && IsLinkPlayerDataExchangeComplete() == TRUE) gMain.state ++;
            break;
        case 5:
            gTradeAnim->playerFinishStatus = STATUS_NONE;
            gTradeAnim->partnerFinishStatus = STATUS_NONE;
            gTradeAnim->scheduleLinkTransfer = 0;
            LoadTradeMonPic(TRADE_PLAYER, 0);
            gMain.state ++;
            break;
        case 6:
            LoadTradeMonPic(TRADE_PLAYER, 1);
            gMain.state ++;
            break;
        case 7:
            LoadTradeMonPic(TRADE_PARTNER, 0);
            gMain.state ++;
            break;
        case 8:
            LoadTradeMonPic(TRADE_PARTNER, 1);
            gMain.state ++;
            break;
        case 9:
            LoadTradeSequenceSpriteSheetsAndPalettes();
            LoadSpriteSheet(&sPokeBallSpriteSheet);
            LoadSpritePalette(&sPokeBallSpritePalette);
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5);
            gMain.state ++;
            break;
        case 10:
            gMain.state ++;
            // fallthrough
        case 11:
            SetTradeSequenceBgGpuRegs(5);
            SetTradeSequenceBgGpuRegs(0);
            BufferTradeSceneStrings();
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            gMain.state ++;
            break;
        case 12:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(CB2_UpdateLinkTrade);
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}
#else
NAKED void CB2_LinkTrade(void)
{
    asm(".syntax unified\n"
"\tpush {r4-r6,lr}\n"
                    "\tsub sp, 0x4\n"
                    "\tldr r1, _0804B43C @ =gMain\n"
                    "\tldr r2, _0804B440 @ =0x0000043c\n"
                    "\tadds r0, r1, r2\n"
                    "\tldrb r0, [r0]\n"
                    "\tadds r2, r1, 0\n"
                    "\tcmp r0, 0xC\n"
                    "\tbls _0804B430\n"
                    "\tb _0804B76E_break\n"
                    "_0804B430:\n"
                    "\tlsls r0, 2\n"
                    "\tldr r1, _0804B444 @ =_0804B448\n"
                    "\tadds r0, r1\n"
                    "\tldr r0, [r0]\n"
                    "\tmov pc, r0\n"
                    "\t.align 2, 0\n"
                    "_0804B43C: .4byte gMain\n"
                    "_0804B440: .4byte 0x0000043c\n"
                    "_0804B444: .4byte _0804B448\n"
                    "\t.align 2, 0\n"
                    "_0804B448:\n"
                    "\t.4byte _0804B47C_case00\n"
                    "\t.4byte _0804B5AC_case01\n"
                    "\t.4byte _0804B5D4_case02\n"
                    "\t.4byte _0804B5FC_case03\n"
                    "\t.4byte _0804B648_case04\n"
                    "\t.4byte _0804B678_case05\n"
                    "\t.4byte _0804B6A8_case06\n"
                    "\t.4byte _0804B6B2_case07\n"
                    "\t.4byte _0804B6CC_case08\n"
                    "\t.4byte _0804B6E4_case09\n"
                    "\t.4byte _0804B71C_case10\n"
                    "\t.4byte _0804B726_case11\n"
                    "\t.4byte _0804B75C_case12\n"
                    "_0804B47C_case00:\n"
                    "\tmovs r1, 0x80\n"
                    "\tlsls r1, 19\n"
                    "\tmovs r0, 0\n"
                    "\tstrh r0, [r1]\n"
                    "\tbl ResetTasks\n"
                    "\tbl CloseLink\n"
                    "\tldr r6, _0804B570 @ =gTradeAnim\n"
                    "\tldr r5, _0804B574 @ =gSharedMem + 0x1F000\n"
                    "\tstr r5, [r6]\n"
                    "\tbl ResetSpriteData\n"
                    "\tbl FreeAllSpritePalettes\n"
                    "\tldr r0, _0804B578 @ =VBlankCB_TradeAnim\n"
                    "\tbl SetVBlankCallback\n"
                    "\tbl ClearLinkTimeoutTimer\n"
                    "\tldr r4, _0804B57C @ =gWindowTemplate_81E6F84\n"
                    "\tadds r0, r4, 0\n"
                    "\tbl Text_LoadWindowTemplate\n"
                    "\tldr r0, [r6]\n"
                    "\tadds r0, 0x4\n"
                    "\tadds r1, r4, 0\n"
                    "\tbl Text_InitWindowWithTemplate\n"
                    "\tmovs r0, 0x2\n"
                    "\tbl TextWindow_SetBaseTileNum\n"
                    "\tldr r1, [r6]\n"
                    "\tadds r1, 0x34\n"
                    "\tstrb r0, [r1]\n" // start of nonmatching
                    "\tldr r0, [r6]\n"
                    "\tadds r0, 0x4\n"
                    "\tbl TextWindow_LoadStdFrameGraphics\n"
                    "\tbl Menu_EraseScreen\n"
                    "\tldr r1, _0804B580 @ =gLinkType\n"
                    "\tldr r4, _0804B584 @ =0x00001144\n"
                    "\tadds r0, r4, 0\n"
                    "\tstrh r0, [r1]\n"
                    "\tldr r1, _0804B588 @ =gMain\n"
                    "\tldr r0, _0804B58C @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r0, _0804B590 @ =gBattleTextboxTiles\n"
                    "\tmovs r1, 0xC0\n"
                    "\tlsls r1, 19\n"
                    "\tbl LZDecompressVram\n"
                    "\tldr r0, _0804B594 @ =gBattleTextboxTilemap\n"
                    "\tldr r1, _0804B598 @ =0xfffe1000\n"
                    "\tadds r5, r1\n"
                    "\tmovs r2, 0x80\n"
                    "\tlsls r2, 4\n"
                    "\tadds r1, r5, 0\n"
                    "\tbl CpuSet\n"
                    "\tldr r1, _0804B59C @ =0x06002800\n"
                    "\tldr r0, _0804B5A0 @ =0x040000d4\n"
                    "\tstr r5, [r0]\n"
                    "\tstr r1, [r0, 0x4]\n"
                    "\tldr r1, _0804B5A4 @ =0x80000280\n"
                    "\tstr r1, [r0, 0x8]\n"
                    "\tldr r0, [r0, 0x8]\n"
                    "\tldr r0, _0804B5A8 @ =gBattleTextboxPalette\n"
                    "\tmovs r1, 0\n"
                    "\tmovs r2, 0x20\n"
                    "\tbl LoadCompressedPalette\n"
                    "\tldr r1, [r6]\n"
                    "\tadds r0, r1, 0\n"
                    "\tadds r0, 0xB6\n"
                    "\tmovs r2, 0\n"
                    "\tstrh r2, [r0]\n"
                    "\tadds r0, 0xE\n"
                    "\tstrh r2, [r0]\n"
                    "\tmovs r4, 0x8F\n"
                    "\tlsls r4, 1\n"
                    "\tadds r1, r4\n"
                    "\tmovs r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r3, [r6]\n"
                    "\tmovs r1, 0x82\n"
                    "\tlsls r1, 1\n"
                    "\tadds r0, r3, r1\n"
                    "\tmovs r1, 0x40\n"
                    "\tstrh r1, [r0]\n"
                    "\tsubs r4, 0x18\n"
                    "\tadds r0, r3, r4\n"
                    "\tstrh r1, [r0]\n"
                    "\tadds r1, 0xC8\n"
                    "\tadds r0, r3, r1\n"
                    "\tstrh r2, [r0]\n"
                    "\tadds r4, 0x4\n"
                    "\tadds r0, r3, r4\n"
                    "\tstrh r2, [r0]\n"
                    "\tmovs r0, 0x86\n"
                    "\tlsls r0, 1\n"
                    "\tadds r1, r3, r0\n"
                    "\tmovs r0, 0x78\n"
                    "\tstrh r0, [r1]\n"
                    "\tadds r4, 0x4\n"
                    "\tadds r1, r3, r4\n"
                    "\tmovs r0, 0x50\n"
                    "\tstrh r0, [r1]\n"
                    "\tadds r0, 0xC8\n"
                    "\tadds r1, r3, r0\n"
                    "\tsubs r0, 0x18\n"
                    "\tstrh r0, [r1]\n"
                    "\tmovs r1, 0x8E\n"
                    "\tlsls r1, 1\n"
                    "\tadds r0, r3, r1\n"
                    "\tstrh r2, [r0]\n"
                    "\tb _0804B76E_break\n"
                    "\t.align 2, 0\n"
                    "_0804B570: .4byte gTradeAnim\n"
                    "_0804B574: .4byte gSharedMem + 0x1F000\n"
                    "_0804B578: .4byte VBlankCB_TradeAnim\n"
                    "_0804B57C: .4byte gWindowTemplate_81E6F84\n"
                    "_0804B580: .4byte gLinkType\n"
                    "_0804B584: .4byte %a0 @ LINKTYPE_TRADE_DISCONNECTED\n"
                    "_0804B588: .4byte gMain\n"
                    "_0804B58C: .4byte 0x0000043c\n"
                    "_0804B590: .4byte gBattleTextboxTiles\n"
                    "_0804B594: .4byte gBattleTextboxTilemap\n"
                    "_0804B598: .4byte 0xfffe1000\n"
                    "_0804B59C: .4byte 0x06002800\n"
                    "_0804B5A0: .4byte 0x040000d4\n"
                    "_0804B5A4: .4byte 0x80000280\n"
                    "_0804B5A8: .4byte gBattleTextboxPalette\n"
                    "_0804B5AC_case01:\n"
                    "\tbl OpenLink\n"
                    "\tldr r1, _0804B5C8 @ =gMain\n"
                    "\tldr r2, _0804B5CC @ =0x0000043c\n"
                    "\tadds r1, r2\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tmovs r2, 0\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r0, _0804B5D0 @ =gTradeAnim\n"
                    "\tldr r0, [r0]\n"
                    "\tadds r0, 0xC0\n"
                    "\tstr r2, [r0]\n"
                    "\tb _0804B76E_break\n"
                    "\t.align 2, 0\n"
                    "_0804B5C8: .4byte gMain\n"
                    "_0804B5CC: .4byte 0x0000043c\n"
                    "_0804B5D0: .4byte gTradeAnim\n"
                    "_0804B5D4_case02:\n"
                    "\tldr r0, _0804B5F4 @ =gTradeAnim\n"
                    "\tldr r0, [r0]\n"
                    "\tadds r1, r0, 0\n"
                    "\tadds r1, 0xC0\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstr r0, [r1]\n"
                    "\tcmp r0, 0x3C\n"
                    "\tbhi _0804B5E8\n"
                    "\tb _0804B76E_break\n"
                    "_0804B5E8:\n"
                    "\tmovs r0, 0\n"
                    "\tstr r0, [r1]\n"
                    "\tldr r4, _0804B5F8 @ =0x0000043c\n"
                    "\tadds r1, r2, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B5F4: .4byte gTradeAnim\n"
                    "_0804B5F8: .4byte 0x0000043c\n"
                    "_0804B5FC_case03:\n"
                    "\tbl IsLinkMaster\n"
                    "\tlsls r0, 24\n"
                    "\tcmp r0, 0\n"
                    "\tbne _0804B608\n"
                    "\tb _0804B746\n"
                    "_0804B608:\n"
                    "\tbl GetLinkPlayerCount_2\n"
                    "\tadds r4, r0, 0\n"
                    "\tbl GetSavedPlayerCount\n"
                    "\tlsls r4, 24\n"
                    "\tlsls r0, 24\n"
                    "\tcmp r4, r0\n"
                    "\tbcs _0804B61C\n"
                    "\tb _0804B76E_break\n"
                    "_0804B61C:\n"
                    "\tldr r0, _0804B63C @ =gTradeAnim\n"
                    "\tldr r1, [r0]\n"
                    "\tadds r1, 0xC0\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstr r0, [r1]\n"
                    "\tcmp r0, 0x1E\n"
                    "\tbhi _0804B62E\n"
                    "\tb _0804B76E_break\n"
                    "_0804B62E:\n"
                    "\tbl CheckShouldAdvanceLinkState\n"
                    "\tldr r1, _0804B640 @ =gMain\n"
                    "\tldr r0, _0804B644 @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B63C: .4byte gTradeAnim\n"
                    "_0804B640: .4byte gMain\n"
                    "_0804B644: .4byte 0x0000043c\n"
                    "_0804B648_case04:\n"
                    "\tbl CheckForLinkTimeout\n"
                    "\tldr r0, _0804B66C @ =gReceivedRemoteLinkPlayers\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r0, 0x1\n"
                    "\tbeq _0804B656\n"
                    "\tb _0804B76E_break\n"
                    "_0804B656:\n"
                    "\tbl IsLinkPlayerDataExchangeComplete\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tcmp r0, 0x1\n"
                    "\tbeq _0804B664\n"
                    "\tb _0804B76E_break\n"
                    "_0804B664:\n"
                    "\tldr r1, _0804B670 @ =gMain\n"
                    "\tldr r4, _0804B674 @ =0x0000043c\n"
                    "\tadds r1, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B66C: .4byte gReceivedRemoteLinkPlayers\n"
                    "_0804B670: .4byte gMain\n"
                    "_0804B674: .4byte 0x0000043c\n"
                    "_0804B678_case05:\n"
                    "\tldr r2, _0804B69C @ =gTradeAnim\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x9C\n"
                    "\tmovs r1, 0\n"
                    "\tstrb r1, [r0]\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x9D\n"
                    "\tstrb r1, [r0]\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0xBD\n"
                    "\tstrb r1, [r0]\n"
                    "\tmovs r0, 0\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tldr r1, _0804B6A0 @ =gMain\n"
                    "\tldr r0, _0804B6A4 @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B69C: .4byte gTradeAnim\n"
                    "_0804B6A0: .4byte gMain\n"
                    "_0804B6A4: .4byte 0x0000043c\n"
                    "_0804B6A8_case06:\n"
                    "\tmovs r0, 0\n"
                    "\tmovs r1, 0x1\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tb _0804B746\n"
                    "_0804B6B2_case07:\n"
                    "\tmovs r0, 0x1\n"
                    "\tmovs r1, 0\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tldr r1, _0804B6C4 @ =gMain\n"
                    "\tldr r4, _0804B6C8 @ =0x0000043c\n"
                    "\tadds r1, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B6C4: .4byte gMain\n"
                    "_0804B6C8: .4byte 0x0000043c\n"
                    "_0804B6CC_case08:\n"
                    "\tmovs r0, 0x1\n"
                    "\tmovs r1, 0x1\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tldr r1, _0804B6DC @ =gMain\n"
                    "\tldr r0, _0804B6E0 @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B6DC: .4byte gMain\n"
                    "_0804B6E0: .4byte 0x0000043c\n"
                    "_0804B6E4_case09:\n"
                    "\tbl LoadTradeSequenceSpriteSheetsAndPalettes\n"
                    "\tldr r0, _0804B704 @ =sPokeBallSpriteSheet\n"
                    "\tbl LoadSpriteSheet\n"
                    "\tldr r0, _0804B708 @ =sPokeBallSpritePalette\n"
                    "\tbl LoadSpritePalette\n"
                    "\tldr r1, _0804B70C @ =REG_BG1CNT\n"
                    "\tldr r2, _0804B710 @ =0x00000502\n"
                    "\tadds r0, r2, 0\n"
                    "\tstrh r0, [r1]\n"
                    "\tldr r1, _0804B714 @ =gMain\n"
                    "\tldr r4, _0804B718 @ =0x0000043c\n"
                    "\tadds r1, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B704: .4byte sPokeBallSpriteSheet\n"
                    "_0804B708: .4byte sPokeBallSpritePalette\n"
                    "_0804B70C: .4byte REG_BG1CNT\n"
                    "_0804B710: .4byte %a1 @ native BG1 priority/screen base\n"
                    "_0804B714: .4byte gMain\n"
                    "_0804B718: .4byte 0x0000043c\n"
                    "_0804B71C_case10:\n"
                    "\tldr r0, _0804B754 @ =0x0000043c\n"
                    "\tadds r1, r2, r0\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "_0804B726_case11:\n"
                    "\tmovs r0, 0x5\n"
                    "\tbl SetTradeSequenceBgGpuRegs\n"
                    "\tmovs r0, 0\n"
                    "\tbl SetTradeSequenceBgGpuRegs\n"
                    "\tbl BufferTradeSceneStrings\n"
                    "\tmovs r0, 0x1\n"
                    "\tnegs r0, r0\n"
                    "\tmovs r1, 0\n"
                    "\tstr r1, [sp]\n"
                    "\tmovs r2, 0x10\n"
                    "\tmovs r3, 0\n"
                    "\tbl BeginNormalPaletteFade\n"
                    "_0804B746:\n"
                    "\tldr r1, _0804B758 @ =gMain\n"
                    "\tldr r2, _0804B754 @ =0x0000043c\n"
                    "\tadds r1, r2\n"
                    "_0804B74C:\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tb _0804B76E_break\n"
                    "\t.align 2, 0\n"
                    "_0804B754: .4byte 0x0000043c\n"
                    "_0804B758: .4byte gMain\n"
                    "_0804B75C_case12:\n"
                    "\tldr r0, _0804B788 @ =gPaletteFade\n"
                    "\tldrb r1, [r0, 0x7]\n"
                    "\tmovs r0, 0x80\n"
                    "\tands r0, r1\n"
                    "\tcmp r0, 0\n"
                    "\tbne _0804B76E_break\n"
                    "\tldr r0, _0804B78C @ =CB2_UpdateLinkTrade\n"
                    "\tbl SetMainCallback2\n"
                    "_0804B76E_break:\n"
                    "\tbl RunTasks\n"
                    "\tbl AnimateSprites\n"
                    "\tbl BuildOamBuffer\n"
                    "\tbl UpdatePaletteFade\n"
                    "\tadd sp, 0x4\n"
                    "\tpop {r4-r6}\n"
                    "\tpop {r0}\n"
                    "\tbx r0\n"
                    "\t.align 2, 0\n"
                    "_0804B788: .4byte gPaletteFade\n"
                    "_0804B78C: .4byte CB2_UpdateLinkTrade"
                    "\n.syntax divided\n"
                    :
                    : "i"(LINKTYPE_TRADE_DISCONNECTED),
                      "i"(BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5)));
}
#endif

// In-game trade init
void CB2_InitInGameTrade(void)
{
    u8 otName[TRAINER_NAME_LENGTH + 1];
    switch (gMain.state)
    {
        case 0:
            gSelectedTradeMonPositions[TRADE_PLAYER] = gSpecialVar_0x8005;
            gSelectedTradeMonPositions[TRADE_PARTNER] = PARTY_SIZE;
            StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
            GetMonData(&gEnemyParty[0], MON_DATA_OT_NAME, otName);
            StringCopy(gLinkPlayers[1].name, otName);
            REG_DISPCNT = 0;
            ResetTasks();
            gTradeAnim = &ewram_2010000.unk_0f000;
            ResetSpriteData();
            FreeAllSpritePalettes();
            SetVBlankCallback(VBlankCB_TradeAnim);
            ClearLinkTimeoutTimer();
            Text_LoadWindowTemplate(&gWindowTemplate_81E717C);
            Text_InitWindowWithTemplate(&gTradeAnim->window, &gWindowTemplate_81E717C);
            gTradeAnim->textWindowBaseTileNum = TextWindow_SetBaseTileNum(2);
            TextWindow_LoadStdFrameGraphics(&gTradeAnim->window);
            Menu_EraseScreen();
            gLinkType = LINKTYPE_TRADE_DISCONNECTED;
            gTradeAnim->isLinkTrade = FALSE;
            gTradeAnim->unk_00b6 = 0;
            gTradeAnim->state = STATE_START;
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x40;
            gTradeAnim->unk_0108 = 0;
            gTradeAnim->unk_010a = 0;
            gTradeAnim->scrX = DISPLAY_WIDTH / 2;
            gTradeAnim->scrY = DISPLAY_HEIGHT / 2;
            gTradeAnim->sXY = Q_8_8(1);
            gTradeAnim->alpha = 0;
            gTradeAnim->timer = 0;
            gMain.state = 5;
            break;
        case 5:
            LoadTradeMonPic(TRADE_PLAYER, 0);
            gMain.state ++;
            break;
        case 6:
            LoadTradeMonPic(TRADE_PLAYER, 1);
            gMain.state ++;
            break;
        case 7:
            LoadTradeMonPic(TRADE_PARTNER, 0);
            gMain.state ++;
            break;
        case 8:
            LoadTradeMonPic(TRADE_PARTNER, 1);
            gMain.state ++;
            break;
        case 9:
            LoadTradeSequenceSpriteSheetsAndPalettes();
            LoadSpriteSheet(&sPokeBallSpriteSheet);
            LoadSpritePalette(&sPokeBallSpritePalette);
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5);
            gMain.state ++;
            break;
        case 10:
            gMain.state ++;
            // fallthrough
        case 11:
            SetTradeSequenceBgGpuRegs(5);
            SetTradeSequenceBgGpuRegs(0);
            BufferTradeSceneStrings();
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            gMain.state ++;
            break;
        case 12:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(CB2_InGameTrade);
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void UpdatePokedexForReceivedMon(u8 partyIdx)
{
    struct Pokemon *mon = &gPlayerParty[partyIdx];
    if (!GetMonData(mon, MON_DATA_IS_EGG))
    {
        u16 species = SpeciesToNationalPokedexNum(GetMonData(mon, MON_DATA_SPECIES, NULL));
        GetSetPokedexFlag(species, FLAG_SET_SEEN);
        GetSetPokedexFlag(species, FLAG_SET_CAUGHT);
    }
}

static void TryEnableNationalDexFromLinkPartner(void)
{
    u8 mpId = GetMultiplayerId();
    if (gLinkPlayers[mpId ^ 1].lp_field_2 == 0x8000)
        EnableNationalPokedex();
}

static void TradeMons(u8 playerPartyIdx, u8 partnerPartyIdx)
{
    u8 friendship;
    struct Pokemon *playerMon = &gPlayerParty[playerPartyIdx];
    u16 playerMail = GetMonData(playerMon, MON_DATA_MAIL);

    struct Pokemon *partnerMon = &gEnemyParty[partnerPartyIdx];
    u16 partnerMail = GetMonData(partnerMon, MON_DATA_MAIL);

    if (playerMail != MAIL_NONE)
        ClearMail(&gSaveBlock1.mail[playerMail]);

    // This is where the actual trade happens!!
    gTradeAnim->tempMon = *playerMon;
    *playerMon = *partnerMon;
    *partnerMon = gTradeAnim->tempMon;

    friendship = STANDARD_FRIENDSHIP;
    if (!GetMonData(playerMon, MON_DATA_IS_EGG))
        SetMonData(playerMon, MON_DATA_FRIENDSHIP, &friendship);

    if (partnerMail != MAIL_NONE)
        GiveMailToMon(playerMon, &gTradeMail[partnerMail]);

    UpdatePokedexForReceivedMon(playerPartyIdx);
    if (gReceivedRemoteLinkPlayers)
        TryEnableNationalDexFromLinkPartner();
}

static void HandleLinkDataSend(void)
{
    switch (gTradeAnim->scheduleLinkTransfer)
    {
        case 1:
            if (IsLinkTaskFinished())
            {
                Trade_SendData(gTradeAnim);
                gTradeAnim->scheduleLinkTransfer ++;
            }
            // fallthrough
        case 2:
            gTradeAnim->scheduleLinkTransfer = 0;
            break;
    }
}

static void CB2_InGameTrade(void)
{
    DoTradeAnim();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void SetTradeSequenceBgGpuRegs(u8 state)
{
    int i;
    u16 *buffer;
    switch (state)
    {
        case 0:
            LoadPalette(gTradeGba2_Pal, BG_PLTT_ID(1), sizeof(gTradeGba2_Pal));
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(1), 0x1300, 0x1000);
            DmaCopy16Defvars(3, gTradePlatform_Tilemap, BG_SCREEN_ADDR(18), 0x1000);
            gTradeAnim->bg2vofs = 0;
            gTradeAnim->bg2hofs = 0xb4;
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(18) | BGCNT_TXT512x256;
            break;
        case 1:
            gTradeAnim->bg1hofs = 0;
            gTradeAnim->bg1vofs = 0x15c;
            REG_BG1VOFS = 0x15c;
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(5) | BGCNT_TXT256x512;

            DmaCopy16Defvars(3, sGbaMapCable, BG_SCREEN_ADDR(5), sizeof(sGbaMapCable));
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(0), 0x1300, 0x1000);
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            break;
        case 2:
            gTradeAnim->bg1vofs = 0;
            gTradeAnim->bg1hofs = 0;
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            DmaCopy16Defvars(3, sCableCloseup_Map, BG_SCREEN_ADDR(5), sizeof(sCableCloseup_Map));
            break;
        case 3:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x40;
            gTradeAnim->scrX = DISPLAY_WIDTH / 2;
            gTradeAnim->scrY = -0x46;
            gTradeAnim->alpha = 0;
            DmaCopyLarge16(3, sPokeballSymbol_Gfx, BG_CHAR_ADDR(1), sizeof(sPokeballSymbol_Gfx), 0x1000);
            DmaCopy16Defvars(3, sPokeballSymbol_Map, BG_SCREEN_ADDR(18), sizeof(sPokeballSymbol_Map));
            break;
        case 4:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(18);
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x5c;
            gTradeAnim->sXY = Q_8_8(0.125);
            gTradeAnim->gbaScale = Q_8_8(4);
            gTradeAnim->alpha = 0;
            DmaCopyLarge16(3, sGbaAffine_Gfx, BG_CHAR_ADDR(1), sizeof(sGbaAffine_Gfx), 0x1000);
            DmaCopy16Defvars(3, sGbaAffineMapCable, BG_SCREEN_ADDR(18), sizeof(sGbaAffineMapCable));
            break;
        case 5:
            gTradeAnim->bg1vofs = 0;
            gTradeAnim->bg1hofs = 0;
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(5);
            LZDecompressVram(gBattleTextboxTiles, BG_CHAR_ADDR(0));
            CpuCopy16(gBattleTextboxTilemap, buffer = (u16 *)gSharedMem, 0x1000);
            LoadCompressedPalette(gBattleTextboxPalette, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
            FillPalette(0, 0, 2);
            for (i = 0; i < 0x280; i ++)
                buffer[i] |= 0x7000;
            DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
            Menu_EraseWindowRect(2, 15, 27, 18);
            break;
        case 6:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(18);
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x5c;
            gTradeAnim->sXY = Q_8_8(1);
            gTradeAnim->gbaScale = Q_8_8(0.5);
            gTradeAnim->scrX = DISPLAY_WIDTH / 2;
            gTradeAnim->scrY = DISPLAY_HEIGHT / 2;
            gTradeAnim->alpha = 0;
            DmaCopyLarge16(3, sGbaAffine_Gfx, BG_CHAR_ADDR(1), sizeof(sGbaAffine_Gfx), 0x1000);
            DmaCopy16Defvars(3, sGbaAffineMapCable, BG_SCREEN_ADDR(18), sizeof(sGbaAffineMapCable));
            break;
        case 7:
            gTradeAnim->bg2vofs = 0;
            gTradeAnim->bg2hofs = 0;
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(18) | BGCNT_TXT512x256;
            LoadPalette(gTradeGba2_Pal, BG_PLTT_ID(1), sizeof(gTradeGba2_Pal));
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(1), 0x1300, 0x1000);
            DmaCopy16Defvars(3, gTradePlatform_Tilemap, BG_SCREEN_ADDR(18), 0x1000);
            break;
    }
}

static void sub_804C0F8(u8 a0)
{
    if (a0 == 0)
    {
        if (gTradeAnim->bg1vofs < 0x10a)
        {
            gTradeAnim->scrY ++;
            gTradeAnim->alpha += 64;
        }
    }
    else
    {
        if (gTradeAnim->scrY > -0x40)
        {
            gTradeAnim->scrY --;
            gTradeAnim->alpha += 64;
        }
    }
}

static void LoadTradeSequenceSpriteSheetsAndPalettes(void)
{
    LoadSpriteSheet(&sSpriteSheet_LinkMonGlow);
    LoadSpriteSheet(&sSpriteSheet_LinkMonShadow);
    LoadSpriteSheet(&sSpriteSheet_CableEnd);
    LoadSpriteSheet(&sSpriteSheet_GbaScreen);
    LoadSpritePalette(&sSpritePalette_LinkMon);
    LoadSpritePalette(&sSpritePalette_Gba);
}

static void BufferTradeSceneStrings(void)
{
    /*Sets the variable strings printed on the
     *actual trading screen. For use in strings
     *like "[Pokemon] will be sent to [Trainer]."
     */
    u8 mpId;
    u8 name[POKEMON_NAME_BUFFER_SIZE];
    const struct InGameTrade *ingameTrade;
    if (gTradeAnim->isLinkTrade)
    {
        mpId = GetMultiplayerId();
        StringCopy(gStringVar1, gLinkPlayers[mpId ^ 1].name);
        GetMonData(&gEnemyParty[gSelectedTradeMonPositions[TRADE_PARTNER] % PARTY_SIZE], MON_DATA_NICKNAME, name);
        StringCopy_Nickname(gStringVar3, name);
        GetMonData(&gPlayerParty[gSelectedTradeMonPositions[TRADE_PLAYER]], MON_DATA_NICKNAME, name);
        StringCopy_Nickname(gStringVar2, name);
    }
    else
    {
        ingameTrade = &sIngameTrades[gSpecialVar_0x8004];
        StringCopy(gStringVar1, ingameTrade->otName);
        StringCopy_Nickname(gStringVar3, ingameTrade->nickname);
        GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_NICKNAME, name);
        StringCopy_Nickname(gStringVar2, name);
    }
}

static bool8 DoTradeAnim(void)
{
    u16 evoTarget;

    switch (gTradeAnim->state)
    {
        case STATE_START:
            gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].invisible = FALSE;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].x2 = -0xb4;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].y2 = gMonFrontPicCoords[gTradeAnim->monSpecies[TRADE_PLAYER]].y_offset;
            gTradeAnim->state ++;
            gTradeAnim->cachedMapMusic = GetCurrentMapMusic();
            PlayBGM(MUS_EVOLUTION);
            break;
        case STATE_MON_SLIDE_IN:
            if (gTradeAnim->bg2hofs > 0)
            {
                gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].x2 += 3;
                gTradeAnim->bg2hofs -= 3;
            }
            else
            {
                gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].x2 = 0;
                gTradeAnim->bg2hofs = 0;
                gTradeAnim->state = STATE_SEND_MSG;
            }
            break;

        case STATE_SEND_MSG:
            StringExpandPlaceholders(gStringVar4, gText_XWillBeSentToY);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            gTradeAnim->state = STATE_BYE_BYE;
            gTradeAnim->timer = 0;
            break;
        case STATE_BYE_BYE:
            if (++gTradeAnim->timer == 80)
            {
                gTradeAnim->releasePokeballSpriteId = CreateTradePokeballSprite(gTradeAnim->monSpriteIds[TRADE_PLAYER], gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].oam.paletteNum, 0x78, 0x20, 0x2, 0x1, 0x14, 0xfffff);
                gTradeAnim->state ++;
                Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
                StringExpandPlaceholders(gStringVar4, gText_ByeByeVar1);
                Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            }
            break;
        case STATE_POKEBALL_DEPART:
            if (gSprites[gTradeAnim->releasePokeballSpriteId].callback == SpriteCallbackDummy && Text_UpdateWindow(&gTradeAnim->window) == TRUE)
            {
                gTradeAnim->bouncingPokeballSpriteId = CreateSprite(&sSpriteTemplate_Pokeball, 0x78, 0x20, 0);
                gSprites[gTradeAnim->bouncingPokeballSpriteId].callback = SpriteCB_BouncingPokeballDepart;
                DestroySprite(&gSprites[gTradeAnim->releasePokeballSpriteId]);
                gTradeAnim->state ++;
            }
            break;
        case STATE_POKEBALL_DEPART_WAIT:
            // The game waits here for the sprite to finish its animation sequence.
            break;
        case STATE_FADE_OUT_TO_GBA_SEND:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTradeAnim->state = STATE_WAIT_FADE_OUT_TO_GBA_SEND;
            break;

        case STATE_WAIT_FADE_OUT_TO_GBA_SEND:
            if (!gPaletteFade.active)
            {
                SetTradeSequenceBgGpuRegs(4);
                gTradeAnim->state ++;
            }
            break;
        case STATE_FADE_IN_TO_GBA_SEND:
            BeginNormalPaletteFade(PALETTES_ALL, -1, 16, 0, RGB_BLACK);
            gTradeAnim->state ++;
            break;
        case STATE_WAIT_FADE_IN_TO_GBA_SEND:
            if (!gPaletteFade.active)
            {
                gTradeAnim->state = STATE_GBA_ZOOM_OUT;
            }
            break;
        case STATE_GBA_ZOOM_OUT:
            if (gTradeAnim->gbaScale > Q_8_8(1))
            {
                gTradeAnim->gbaScale -= 0x34;
            }
            else
            {
                SetTradeSequenceBgGpuRegs(1);
                gTradeAnim->gbaScale = Q_8_8(0.5);
                gTradeAnim->state ++;
                gTradeAnim->timer = 0;
            }
            gTradeAnim->sXY = 0x8000 / gTradeAnim->gbaScale;
            break;
        case STATE_GBA_FLASH_SEND:
            if (++ gTradeAnim->timer > 20)
            {
                SetTradeSequenceBgGpuRegs(3);
                SetTradeBGAffine();
                gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_GbaScreenFlash_Long, 0x78, 0x50, 0);
                gTradeAnim->state ++;
            }
            break;
        case STATE_GBA_STOP_FLASH_SEND:
            if (gSprites[gTradeAnim->connectionSpriteId2].animEnded)
            {
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
                REG_BLDCNT = BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2;
                REG_BLDALPHA = BLDALPHA_BLEND(12, 4);
                gTradeAnim->state ++;
            }
            break;
        case STATE_PAN_AWAY_GBA:
            if (-- gTradeAnim->bg1vofs == 0x13C)
            {
                gTradeAnim->state ++;
            }
            if (gTradeAnim->bg1vofs == 0x148)
            {
                gTradeAnim->cableEndSpriteId = CreateSprite(&sSpriteTemplate_CableEnd, 0x80, 0x41, 0);
            }
            break;
        case STATE_CREATE_LINK_MON_LEAVING:
            gTradeAnim->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonGlow, 0x80, 0x50, 3);
            gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x80, 0x50, 0);
            StartSpriteAnim(&gSprites[gTradeAnim->connectionSpriteId2], ANIM_LINKMON_SMALL);
            gTradeAnim->state ++;
            break;
        case STATE_LINK_MON_TRAVEL_OUT:
            if ((gTradeAnim->bg1vofs -= 2) == 0xA6)
            {
                gTradeAnim->state = STATE_LINK_MON_TRAVEL_OFFSCREEN;
            }
            sub_804C0F8(0);
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            break;
        case STATE_LINK_MON_TRAVEL_OFFSCREEN:
            gSprites[gTradeAnim->connectionSpriteId1].y -= 2;
            gSprites[gTradeAnim->connectionSpriteId2].y -= 2;
            sub_804C0F8(0);
            if (gSprites[gTradeAnim->connectionSpriteId1].y < -8)
            {
                gTradeAnim->state = STATE_FADE_OUT_TO_CROSSING;
            }
            break;
        case STATE_FADE_OUT_TO_CROSSING:
            BeginNormalPaletteFade(PALETTES_ALL, -1, 0, 16, RGB_BLACK);
            gTradeAnim->state = STATE_WAIT_FADE_OUT_TO_CROSSING;
            break;
        case STATE_WAIT_FADE_OUT_TO_CROSSING:
            if (!gPaletteFade.active)
            {
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId1]);
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
                SetTradeSequenceBgGpuRegs(2);
                gTradeAnim->state ++;
            }
            break;
        case STATE_FADE_IN_TO_CROSSING:
            BeginNormalPaletteFade(PALETTES_ALL, -1, 16, 0, RGB_BLACK);
            gTradeAnim->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x6f, 0xaa, 0);
            gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x81, -0xa, 0);
            gTradeAnim->state ++;
            break;
        case STATE_WAIT_FADE_IN_TO_CROSSING:
            if (!gPaletteFade.active)
            {
                PlaySE(SE_WARP_OUT);
                gTradeAnim->state ++;
            }
            gSprites[gTradeAnim->connectionSpriteId1].y2 -= 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            break;
        case STATE_CROSSING_LINK_MONS_ENTER:
            gSprites[gTradeAnim->connectionSpriteId1].y2 -= 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            if (gSprites[gTradeAnim->connectionSpriteId1].y2 <= -0x5a)
            {
                gSprites[gTradeAnim->connectionSpriteId1].data[1] = 1;
                gSprites[gTradeAnim->connectionSpriteId2].data[1] = 1;
                gTradeAnim->state ++;
            }
            break;
        case STATE_CROSSING_BLEND_WHITE_1:
            BlendPalettes(0x1, 16, RGB_WHITEALPHA);
            gTradeAnim->state ++;
            break;
        case STATE_CROSSING_BLEND_WHITE_2:
            BlendPalettes(0x1,  0, RGB_WHITEALPHA);
            gTradeAnim->state ++;
            break;
        case STATE_CROSSING_BLEND_WHITE_3:
            BlendPalettes(0x1, 16, RGB_WHITEALPHA);
            gTradeAnim->state ++;
            break;
        case STATE_CROSSING_CREATE_MON_PICS:
            if (!IsMonSpriteNotFlipped(gTradeAnim->monSpecies[TRADE_PLAYER]))
            {
                gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].affineAnims = sAffineAnims_CrossingMonPics;
                gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].oam.affineMode = ST_OAM_AFFINE_DOUBLE;
                CalcCenterToCornerVec(&gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]], SPRITE_SHAPE(64x64), SPRITE_SIZE(64x64), ST_OAM_AFFINE_DOUBLE);
                StartSpriteAffineAnim(&gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]], 0);
            }
            else
            {
                StartSpriteAffineAnim(&gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]], 0);
            }
            StartSpriteAffineAnim(&gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]], 0);
            gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].x = 0x3c;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].x = 0xb4;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].y = 0xc0;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].y = -0x20;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].invisible = FALSE;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].invisible = FALSE;
            gTradeAnim->state ++;
            break;
        case STATE_CROSSING_MON_PICS_MOVE:
            gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].y2 -= 3;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].y2 += 3;
            if (-DISPLAY_HEIGHT > gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].y2 && gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].y2 >= -DISPLAY_HEIGHT - 3)
            {
                PlaySE(SE_WARP_IN);
            }
            if (gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].y2 < -0xde)
            {
                gSprites[gTradeAnim->connectionSpriteId1].data[1] = 0;
                gSprites[gTradeAnim->connectionSpriteId2].data[1] = 0;
                gTradeAnim->state ++;
                gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]].invisible = TRUE;
                gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].invisible = TRUE;
                BlendPalettes(0x1, 0, RGB_WHITEALPHA);
            }
            break;
        case STATE_CROSSING_LINK_MONS_EXIT:
            gSprites[gTradeAnim->connectionSpriteId1].y2 -= 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            if (gSprites[gTradeAnim->connectionSpriteId1].y2 <= -0xde)
            {
                BeginNormalPaletteFade(PALETTES_ALL, -1, 0, 16, RGB_BLACK);
                gTradeAnim->state ++;
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId1]);
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
            }
            break;
        case STATE_CREATE_LINK_MON_ARRIVING:
            if (!gPaletteFade.active)
            {
                gTradeAnim->state ++;
                SetTradeSequenceBgGpuRegs(1);
                gTradeAnim->bg1vofs = 0xa6;
                gTradeAnim->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonGlow, 0x80, -0x14, 3);
                gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x80, -0x14, 0);
                StartSpriteAnim(&gSprites[gTradeAnim->connectionSpriteId2], ANIM_LINKMON_SMALL);
            }
            break;
        case STATE_FADE_OUT_TO_GBA_RECV:
            BeginNormalPaletteFade(PALETTES_ALL, -1, 16, 0, RGB_BLACK);
            gTradeAnim->state ++;
            break;
        case STATE_WAIT_FADE_OUT_TO_GBA_RECV:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            sub_804C0F8(1);
            if (!gPaletteFade.active)
            {
                gTradeAnim->state ++;
            }
            break;
        case STATE_LINK_MON_TRAVEL_IN:
            sub_804C0F8(1);
            gSprites[gTradeAnim->connectionSpriteId1].y2 += 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            if (gSprites[gTradeAnim->connectionSpriteId1].y2 + gSprites[gTradeAnim->connectionSpriteId1].y == 64)
            {
                gTradeAnim->state ++;
            }
            break;
        case STATE_PAN_TO_GBA:
            sub_804C0F8(1);
            if ((gTradeAnim->bg1vofs += 2) > 0x13c)
            {
                gTradeAnim->bg1vofs = 0x13c;
                gTradeAnim->state ++;
            }
            break;
        case STATE_DESTROY_LINK_MON:
            DestroySprite(&gSprites[gTradeAnim->connectionSpriteId1]);
            DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
            gTradeAnim->state ++;
            gTradeAnim->timer = 0;
            break;
        case STATE_LINK_MON_ARRIVED_DELAY:
            if (++ gTradeAnim->timer == 10)
            {
                gTradeAnim->state ++;
            }
            break;
        case STATE_MOVE_GBA_TO_CENTER:
            if (++ gTradeAnim->bg1vofs > 0x15c)
            {
                gTradeAnim->bg1vofs = 0x15c;
                gTradeAnim->state ++;
            }
            if (gTradeAnim->bg1vofs == 0x148)
                gTradeAnim->cableEndSpriteId = CreateSprite(&sSpriteTemplate_CableEnd, 0x80, 0x41, 0);
            gSprites[gTradeAnim->cableEndSpriteId].callback = SpriteCB_CableEndReceiving;
            break;
        case STATE_GBA_FLASH_RECV:
            gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_GbaScreenFlash_Long, 0x78, 0x50, 0);
            gTradeAnim->state = STATE_GBA_STOP_FLASH_RECV;
            break;

        case STATE_GBA_STOP_FLASH_RECV:
            if (gSprites[gTradeAnim->connectionSpriteId2].animEnded)
            {
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
                SetTradeSequenceBgGpuRegs(6);
                gTradeAnim->state ++;
                PlaySE(SE_M_SAND_ATTACK);
            }
            break;
        case STATE_GBA_ZOOM_IN:
            if (gTradeAnim->gbaScale < Q_8_8(4))
            {
                gTradeAnim->gbaScale += 0x34;
            }
            else
            {
                gTradeAnim->gbaScale = Q_8_8(4);
                gTradeAnim->state ++;
            }
            gTradeAnim->sXY = 0x8000 / gTradeAnim->gbaScale;
            break;
        case STATE_FADE_OUT_TO_NEW_MON:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTradeAnim->state = STATE_WAIT_FADE_OUT_TO_NEW_MON;
            break;

        case STATE_WAIT_FADE_OUT_TO_NEW_MON:
            if (!gPaletteFade.active)
            {
                SetTradeSequenceBgGpuRegs(5);
                SetTradeSequenceBgGpuRegs(7);
                gTradeAnim->state ++;
            }
            break;
        case STATE_FADE_IN_TO_NEW_MON:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            gTradeAnim->state ++;
            break;
        case STATE_WAIT_FADE_IN_TO_NEW_MON:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            if (!gPaletteFade.active)
            {
                gTradeAnim->state ++;
            }
            break;
        case STATE_POKEBALL_ARRIVE:
            gTradeAnim->bouncingPokeballSpriteId = CreateSprite(&sSpriteTemplate_Pokeball, 0x78, -0x8, 0);
            gSprites[gTradeAnim->bouncingPokeballSpriteId].data[3] = 0x4a;
            gSprites[gTradeAnim->bouncingPokeballSpriteId].callback = SpriteCB_BouncingPokeballArrive;
            StartSpriteAnim(&gSprites[gTradeAnim->bouncingPokeballSpriteId], 1);
            StartSpriteAffineAnim(&gSprites[gTradeAnim->bouncingPokeballSpriteId], 2);
            BlendPalettes(1 << (16 + gSprites[gTradeAnim->bouncingPokeballSpriteId].oam.paletteNum), 16, RGB_WHITEALPHA);
            gTradeAnim->state ++;
            gTradeAnim->timer = 0;
            break;
        case STATE_FADE_POKEBALL_TO_NORMAL:
            BeginNormalPaletteFade(1 << (16 + gSprites[gTradeAnim->bouncingPokeballSpriteId].oam.paletteNum), 1, 16, 0, RGB_WHITEALPHA);
            gTradeAnim->state ++;
            break;
        case STATE_POKEBALL_ARRIVE_WAIT:
            if (gSprites[gTradeAnim->bouncingPokeballSpriteId].callback == SpriteCallbackDummy)
            {
                gTradeAnim->state ++;
            }
            break;
        case STATE_SHOW_NEW_MON:
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].x = 0x78;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].y = gMonFrontPicCoords[gTradeAnim->monSpecies[TRADE_PARTNER]].y_offset + 60;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].x2 = 0;
            gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].y2 = 0;
            CreatePokeballSpriteToReleaseMon(gTradeAnim->monSpriteIds[TRADE_PARTNER], gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]].oam.paletteNum, 0x78, 0x54, 2, 1, 0x14, 0xfffff);
            FreeSpriteOamMatrix(&gSprites[gTradeAnim->bouncingPokeballSpriteId]);
            DestroySprite(&gSprites[gTradeAnim->bouncingPokeballSpriteId]);
            gTradeAnim->state ++;
            break;
        case STATE_NEW_MON_MSG:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
            StringExpandPlaceholders(gStringVar4, gText_XSentOverY);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            gTradeAnim->state ++;
            gTradeAnim->timer = 0;
            break;
        case STATE_TAKE_CARE_OF_MON:
            if (++ gTradeAnim->timer == 4)
            {
                PlayFanfare(MUS_EVOLVED);
            }
            if (gTradeAnim->timer == 0xf0)
            {
                gTradeAnim->state ++;
                Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
                StringExpandPlaceholders(gStringVar4, gText_TakeGoodCareOfX);
                Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
                gTradeAnim->timer = 0;
            }
            break;
        case STATE_AFTER_NEW_MON_DELAY: // OneHand
            if (++ gTradeAnim->timer == 60)
            {
                gTradeAnim->state ++;
            }
            break;
        case STATE_CHECK_RIBBONS:
            CheckPartnersMonForRibbons();
            gTradeAnim->state ++;
            break;
        case STATE_END_LINK_TRADE:
            if (gTradeAnim->isLinkTrade)
            {
                return TRUE;
            }
            else if (JOY_NEW(A_BUTTON))
            {
                gTradeAnim->state ++;
            }
            break;
        case STATE_TRY_EVOLUTION: // Only if in-game trade
            TradeMons(gSpecialVar_0x8005, 0);
            gCB2_AfterEvolution = CB2_InGameTrade;
            evoTarget = GetEvolutionTargetSpecies(&gPlayerParty[gSelectedTradeMonPositions[TRADE_PLAYER]], EVO_MODE_TRADE, ITEM_NONE);
            if (evoTarget != SPECIES_NONE)
                TradeEvolutionScene(&gPlayerParty[gSelectedTradeMonPositions[TRADE_PLAYER]], evoTarget, gTradeAnim->monSpriteIds[TRADE_PARTNER], gSelectedTradeMonPositions[TRADE_PLAYER]);
            gTradeAnim->state ++;
            break;
        case STATE_FADE_OUT_END:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTradeAnim->state ++;
            break;
        case STATE_WAIT_FADE_OUT_END:
            if (!gPaletteFade.active)
            {
                PlayBGM(gTradeAnim->cachedMapMusic);
                SetMainCallback2(CB2_ReturnToField);
                BufferInGameTradeMonName();
            }
            break;
    }
    return FALSE;
}

static void CB2_TryLinkTradeEvolution(void)
{
    u16 evoTarget;
    switch (gMain.state)
    {
        case 0:
            gMain.state = 4;
            gSoftResetDisabled = TRUE;
            break;
        case 4:
            gCB2_AfterEvolution = CB2_SaveAndEndTrade;
            evoTarget = GetEvolutionTargetSpecies(&gPlayerParty[gSelectedTradeMonPositions[TRADE_PLAYER]], EVO_MODE_TRADE, ITEM_NONE);
            if (evoTarget != SPECIES_NONE)
                TradeEvolutionScene(&gPlayerParty[gSelectedTradeMonPositions[TRADE_PLAYER]], evoTarget, gTradeAnim->monSpriteIds[TRADE_PARTNER], gSelectedTradeMonPositions[TRADE_PLAYER]);
            else
                SetMainCallback2(CB2_SaveAndEndTrade);
            gSelectedTradeMonPositions[TRADE_PLAYER] = 255;
            break;
    }
    if (!HasLinkErrorOccurred())
        RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void HandleLinkDataReceive(void)
{
    u8 recvStatus;
    TradeGetMultiplayerId();
    recvStatus = GetBlockReceivedStatus();
    if (recvStatus & 0x01)
    {
        if (gBlockRecvBuffer[0][0] == LINKCMD_CONFIRM_FINISH_TRADE)
        {
            SetMainCallback2(CB2_TryLinkTradeEvolution);
        }
        if (gBlockRecvBuffer[0][0] == LINKCMD_READY_FINISH_TRADE)
        {
            gTradeAnim->playerFinishStatus = STATUS_READY;
        }
        ResetBlockReceivedFlag(0);
    }
    if (recvStatus & 0x02)
    {
        if (gBlockRecvBuffer[1][0] == LINKCMD_READY_FINISH_TRADE)
        {
            gTradeAnim->partnerFinishStatus = STATUS_READY;
        }
        ResetBlockReceivedFlag(1);
    }
}

static void SpriteCB_BouncingPokeball(struct Sprite *sprite)
{
    sprite->y += sprite->data[0] / 10;
    sprite->data[5] += sprite->data[1];
    sprite->x = sprite->data[5] / 10;
    if (sprite->y > 0x4c)
    {
        sprite->y = 0x4c;
        sprite->data[0] = -(sprite->data[0] * sprite->data[2]) / 100;
        sprite->data[3] ++;
    }
    if (sprite->x == 0x78)
        sprite->data[1] = 0;
    sprite->data[0] += sprite->data[4];
    if (sprite->data[3] == 4)
    {
        sprite->data[7] = 1;
        sprite->callback = SpriteCallbackDummy;
    }
}

static void SpriteCB_BouncingPokeballDepart(struct Sprite *sprite)
{
    sprite->y2 += sTradeBallVerticalVelocityTable[sprite->data[0]];
    if (sprite->data[0] == 22)
        PlaySE(SE_BALL_BOUNCE_1);
    if (++ sprite->data[0] == 44)
    {
        PlaySE(SE_M_MEGA_KICK);
        sprite->callback = SpriteCB_BouncingPokeballDepartEnd;
        sprite->data[0] = 0;
        BeginNormalPaletteFade(1 << (16 + sprite->oam.paletteNum), -1, 0, 16, RGB_WHITEALPHA);
    }
}

static void SpriteCB_BouncingPokeballDepartEnd(struct Sprite *sprite)
{
    if (sprite->data[1] == 20)
        StartSpriteAffineAnim(sprite, 1);
    if (++ sprite->data[1] > 20)
    {
        sprite->y2 -= sTradeBallVerticalVelocityTable[sprite->data[0]];
        if (++ sprite->data[0] == 23)
        {
            DestroySprite(sprite);
            gTradeAnim->state = STATE_FADE_OUT_TO_GBA_SEND; // Resume the master trade animation
        }
    }
}

static void SpriteCB_BouncingPokeballArrive(struct Sprite *sprite)
{
    if (sprite->data[2] == 0)
    {
        if ((sprite->y += 4) > sprite->data[3])
        {
            sprite->data[2] ++;
            sprite->data[0] = 0x16;
            PlaySE(SE_BALL_BOUNCE_1);
        }
    }
    else
    {
        if (sprite->data[0] == 0x42)
            PlaySE(SE_BALL_BOUNCE_2);
        if (sprite->data[0] == 0x5c)
            PlaySE(SE_BALL_BOUNCE_3);
        if (sprite->data[0] == 0x6b)
            PlaySE(SE_BALL_BOUNCE_4);
        sprite->y2 += sTradeBallVerticalVelocityTable[sprite->data[0]];
        if (++sprite->data[0] == 0x6c)
            sprite->callback = SpriteCallbackDummy;
    }
}

u16 GetInGameTradeSpeciesInfo(void)
{
    const struct InGameTrade *inGameTrade = &sIngameTrades[gSpecialVar_0x8004];
    StringCopy(gStringVar1, gSpeciesNames[inGameTrade->requestedSpecies]);
    StringCopy(gStringVar2, gSpeciesNames[inGameTrade->species]);
    return inGameTrade->requestedSpecies;
}

static void BufferInGameTradeMonName(void)
{
    u8 nickname[max(32, POKEMON_NAME_BUFFER_SIZE)];
    const struct InGameTrade *inGameTrade = &sIngameTrades[gSpecialVar_0x8004];
    GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    StringCopy(gStringVar2, gSpeciesNames[inGameTrade->species]);
}

static void CreateInGameTradePokemonInternal(u8 whichPlayerMon, u8 whichInGameTrade)
{
    const struct InGameTrade *inGameTrade = &sIngameTrades[whichInGameTrade];
    u8 level = GetMonData(&gPlayerParty[whichPlayerMon], MON_DATA_LEVEL);

    struct Mail mail;
    u8 metLocation = METLOC_IN_GAME_TRADE;
    u8 mailNum;
    struct Pokemon *pokemon = &gEnemyParty[0];

    CreateMon(pokemon, inGameTrade->species, level, USE_RANDOM_IVS, TRUE, inGameTrade->personality, OT_ID_PRESET, inGameTrade->otId);

    SetMonData(pokemon, MON_DATA_HP_IV, &inGameTrade->ivs[STAT_HP]);
    SetMonData(pokemon, MON_DATA_ATK_IV, &inGameTrade->ivs[STAT_ATK]);
    SetMonData(pokemon, MON_DATA_DEF_IV, &inGameTrade->ivs[STAT_DEF]);
    SetMonData(pokemon, MON_DATA_SPEED_IV, &inGameTrade->ivs[STAT_SPEED]);
    SetMonData(pokemon, MON_DATA_SPATK_IV, &inGameTrade->ivs[STAT_SPATK]);
    SetMonData(pokemon, MON_DATA_SPDEF_IV, &inGameTrade->ivs[STAT_SPDEF]);
    SetMonData(pokemon, MON_DATA_NICKNAME, inGameTrade->nickname);
    SetMonData(pokemon, MON_DATA_OT_NAME, inGameTrade->otName);
    SetMonData(pokemon, MON_DATA_OT_GENDER, &inGameTrade->otGender);
    SetMonData(pokemon, MON_DATA_ABILITY_NUM, &inGameTrade->abilityNum);
    SetMonData(pokemon, MON_DATA_BEAUTY, &inGameTrade->conditions[CONTEST_CATEGORY_BEAUTY]);
    SetMonData(pokemon, MON_DATA_CUTE, &inGameTrade->conditions[CONTEST_CATEGORY_CUTE]);
    SetMonData(pokemon, MON_DATA_COOL, &inGameTrade->conditions[CONTEST_CATEGORY_COOL]);
    SetMonData(pokemon, MON_DATA_SMART, &inGameTrade->conditions[CONTEST_CATEGORY_SMART]);
    SetMonData(pokemon, MON_DATA_TOUGH, &inGameTrade->conditions[CONTEST_CATEGORY_TOUGH]);
    SetMonData(pokemon, MON_DATA_SHEEN, &inGameTrade->sheen);
    SetMonData(pokemon, MON_DATA_MET_LOCATION, &metLocation);

    mailNum = FALSE;
    if (inGameTrade->heldItem != ITEM_NONE)
    {
        if (ItemIsMail(inGameTrade->heldItem))
        {
            GetInGameTradeMail(&mail, inGameTrade);
            gTradeMail[0] = mail;
            SetMonData(pokemon, MON_DATA_MAIL, &mailNum);
            SetMonData(pokemon, MON_DATA_HELD_ITEM, &inGameTrade->heldItem);
        }
        else
        {
            SetMonData(pokemon, MON_DATA_HELD_ITEM, &inGameTrade->heldItem);
        }
    }
    CalculateMonStats(&gEnemyParty[0]);
}

static void GetInGameTradeMail(struct Mail *mail, const struct InGameTrade *trade) {
    s32 i;

    for (i = 0; i < MAIL_WORDS_COUNT; i++)
    {
        mail->words[i] = sIngameTradeMail[trade->mailNum][i];
    }

    StringCopy(mail->playerName, trade->otName);

#if GERMAN
    PadNameString(mail->playerName, CHAR_SPACE);
#endif

    mail->trainerId[0] = trade->otId >> 24;
    mail->trainerId[1] = trade->otId >> 16;
    mail->trainerId[2] = trade->otId >> 8;
    mail->trainerId[3] = trade->otId;
    mail->species = trade->species;
    mail->itemId = trade->heldItem;
}

u16 GetTradeSpecies(void)
{
    if (GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_IS_EGG))
        return SPECIES_NONE;
    return GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_SPECIES);
}

void CreateInGameTradePokemon(void)
{
    CreateInGameTradePokemonInternal(gSpecialVar_0x8005, gSpecialVar_0x8004);
}

void CB2_UpdateLinkTrade(void)
{
    if (DoTradeAnim() == TRUE)
    {
        DestroySprite(&gSprites[gTradeAnim->monSpriteIds[TRADE_PLAYER]]);
        FreeSpriteOamMatrix(&gSprites[gTradeAnim->monSpriteIds[TRADE_PARTNER]]);
        TradeMons(gSelectedTradeMonPositions[TRADE_PLAYER], gSelectedTradeMonPositions[TRADE_PARTNER] % PARTY_SIZE);
        gTradeAnim->linkData[0] = LINKCMD_READY_FINISH_TRADE;
        gTradeAnim->scheduleLinkTransfer = 1;
        SetMainCallback2(CB2_WaitTradeComplete);
    }
    HandleLinkDataSend();
    HandleLinkDataReceive();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_WaitTradeComplete(void)
{
    u8 mpId = TradeGetMultiplayerId();
    HandleLinkDataReceive();
    if (mpId == 0 && gTradeAnim->playerFinishStatus == STATUS_READY && gTradeAnim->partnerFinishStatus == STATUS_READY)
    {
        gTradeAnim->linkData[0] = LINKCMD_CONFIRM_FINISH_TRADE;
        Trade_SendData(gTradeAnim);
        gTradeAnim->playerFinishStatus = 2;
        gTradeAnim->partnerFinishStatus = 2;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_SaveAndEndTrade(void)
{
    switch (gMain.state)
    {
        case 0:
            gTradeAnim = &ewram_2010000.unk_0f000;
            gMain.state ++;
            Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
            StringExpandPlaceholders(gStringVar4, gText_CommunicationStandby5);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            break;
        case 1:
            SetLinkStandbyCallback();
            gMain.state = 100;
            gTradeAnim->timer = 0;
            break;
        case 100:
            if (++ gTradeAnim->timer > 180)
            {
                gMain.state = 101;
                gTradeAnim->timer = 0;
            }
            if (IsLinkTaskFinished())
            {
                gMain.state = 2;
            }
            break;
        case 101:
            if (IsLinkTaskFinished())
            {
                gMain.state = 2;
            }
            break;
        case 2:
            gMain.state = 50;
            Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gText_SavingDontTurnOffPower, gTradeAnim->textWindowBaseTileNum, 2, 15);
            break;
        case 50:
            SetContinueGameWarpStatusToDynamicWarp();
            IncrementGameStat(GAME_STAT_POKEMON_TRADES);
            LinkFullSave_Init();
            gMain.state ++;
            gTradeAnim->timer = 0;
            break;
        case 51:
            if (++ gTradeAnim->timer == 5)
            {
                gMain.state ++;
            }
            break;
        case 52:
            if (LinkFullSave_WriteSector())
            {
                ClearContinueGameWarpStatus2();
                gMain.state = 4;
            }
            else
            {
                gTradeAnim->timer = 0;
                gMain.state = 51;
            }
            break;
        case 4:
            LinkFullSave_ReplaceLastSector();
            gMain.state = 40;
            gTradeAnim->timer = 0;
            break;
        case 40:
            if (++ gTradeAnim->timer > 50)
            {
                gTradeAnim->timer = 0;
                gMain.state = 41;
            }
            break;
        case 41:
            SetLinkStandbyCallback();
            gMain.state = 42;
            break;
        case 42:
            if (IsLinkTaskFinished())
            {
                LinkFullSave_SetLastSectorSignature();
                gSoftResetDisabled = FALSE;
                gMain.state = 5;
            }
            break;
        case 5:
            if (++ gTradeAnim->timer > 60)
            {
                gMain.state ++;
                SetLinkStandbyCallback();
            }
            break;
        case 6:
            if (IsLinkTaskFinished())
            {
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gMain.state ++;
            }
            break;
        case 7:
            if (!gPaletteFade.active)
            {
                FadeOutBGM(3);
                gMain.state ++;
            }
            break;
        case 8:
            if (IsBGMStopped() == TRUE)
            {
                SetCloseLinkCallback();
                gMain.state ++;
            }
            break;
        case 9:
            if (!gReceivedRemoteLinkPlayers)
            {
                SetMainCallback2(sub_804E144);
            }
            break;
    }
    if (!HasLinkErrorOccurred())
    {
        RunTasks();
    }
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_804E144(void)
{
    if (!gPaletteFade.active)
        SetMainCallback2((CB2_StartCreateTradeMenu));
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void DoInGameTradeScene(void)
{
    LockPlayerFieldControls();
    CreateTask(Task_InGameTrade, 10);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}

static void Task_InGameTrade(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_InitInGameTrade);
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        DestroyTask(taskId);
    }
}

static void CheckPartnersMonForRibbons(void)
{
    u8 i;
    u8 numRibbons = 0;
    for (i = 0; i < (MON_DATA_WORLD_RIBBON - MON_DATA_CHAMPION_RIBBON + 1); i ++)
    {
        numRibbons += GetMonData(&gEnemyParty[gSelectedTradeMonPositions[TRADE_PARTNER] % PARTY_SIZE], MON_DATA_CHAMPION_RIBBON + i);
    }
    if (numRibbons != 0)
        FlagSet(FLAG_SYS_RIBBON_GET);
}

void LoadTradeAnimGfx(void)
{
    LZDecompressVram(gBattleTextboxTiles, (void *)VRAM);
    CpuCopy16(gBattleTextboxTilemap, gSharedMem, 0x1000);
    DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
    LoadCompressedPalette(gBattleTextboxPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
    REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5);
}
