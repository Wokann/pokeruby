#include "global.h"
#include "battle_anim.h"
#include "name_string_util.h"
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
#include "data2.h"
#include "pokemon_summary_screen.h"
#include "overworld.h"
#include "rom_8077ABC.h"
#include "daycare.h"
#include "event_data.h"
#include "strings.h"
#include "load_save.h"
#include "save.h"
#include "script.h"
#include "field_fadetransition.h"
#include "decompress.h"
#include "mail_data.h"
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

#define Trade_SendData(ptr) (SendBlock(BitmaskAllOtherLinkPlayers(), ptr->linkData, 20))

struct InGameTrade {
    /*0x00*/ u8 nickname[11];
    /*0x0C*/ u16 species;
    /*0x0E*/ u8 ivs[6];
    /*0x14*/ bool8 abilityNum;
    /*0x18*/ u32 otId;
    /*0x1C*/ u8 conditions[5];
    /*0x24*/ u32 personality;
    /*0x28*/ u16 heldItem;
    /*0x2A*/ u8 mailNum;
    /*0x2B*/ u8 otName[11];
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
    /*0x12*/ u16 tilemapBuffer[0x400];
};

struct UnkStructE {
    /*0x00*/ u8 active;
    /*0x02*/ u16 delay;
    /*0x04*/ u8 actionId;
};

struct TradeEwramSubstruct {
    /*0x0000*/ u8 bg2hofs;
    /*0x0001*/ u8 bg3hofs;
    /*0x0004*/ struct Window window;
    /*0x0034*/ u8 partySpriteIds[2][6];
    /*0x0040*/ u8 cursorSpriteId;
    /*0x0041*/ u8 cursorPosition;
    /*0x0042*/ u8 partyCounts[2];
    /*0x0044*/ u8 optionsActive[13];
    /*0x0051*/ u8 isLiveMon[2][6];
    /*0x005d*/ u8 isEgg[2][6];
    /*0x0069*/ u8 hpBarLevels[2][6];
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
    /*0x00b5*/ u8 giftRibbons[11];
    /*0x00c0*/ u8 filler_00c0[8];
    /*0x00c8*/ struct UnkStructD unk_00c8;
    /*0x08dc*/ struct UnkStructE queuedActions[4];
    /*0x08fc*/ u8 filler_08fc[0x704];
};

struct TradeEwramSubstruct2 *gUnknown_03004828;

static u8 gUnknown_03000508[8];

struct UnkStructF {
    u8 filler_0000[9];
    u8 unk_0009;
    u8 filler_000a[0xff6];
};

struct TradeEwramStruct {
    /*0x00000*/ u8 filler_00000[0x7000];
    /*0x07000*/ struct TradeEwramSubstruct unk_07000;
    /*0x08000*/ struct UnkStructF unk_08000;
    /*0x09000*/ u8 filler_09000[0x4000];
    /*0x0d000*/ u8 tileBuffers[13][256];
    /*0x0dd00*/ u8 filler_0dd00[0x1300];
    /*0x0f000*/ struct TradeEwramSubstruct2 unk_0f000;
};

static void CB2_CreateTradeMenu(void);
static void sub_804AFB8(const struct WindowTemplate *, u8 *, const u8 *, u8);
static void sub_804ACD8(const u8 *, u8 *, u8);
static void nullsub_5(u8, u8);
static void sub_804AA88(void);
static void sub_804A964(struct UnkStructD *, void *);
static void sub_80489F4(void);
static void sub_804AA0C(u8);
static bool8 sub_8048D44(void);
static void sub_804AF84(void);
static bool8 sub_804ABF8(void);
static void ComputePartyTradeableFlags(u8);
static void sub_804A41C(u8);
static void SetActiveMenuOptions(void);
static void sub_8048B0C(u8);
static void ComputePartyHPBarLevels(u8);
static void SetTradePartyHPBarSprites(void);
static void sub_80494D8(void);
static void sub_8048AB4(void);
static void sub_804A940(struct UnkStructD *);
void sub_804B41C(void);
static void sub_8049DE0(void);
static void sub_804AB30(void);
static void sub_8049ED4(u8);
static void PrintPartyLevelsAndGenders(u8);
static void sub_804A938(struct UnkStructD *);
static void sub_804A9F4(u8);
static void sub_804AA00(u8);
static void SetSelectedMon(u8);
static void QueueAction(u16, u8);
static void sub_804A80C(void);
static u8 sub_80499F0(const u8 *, u8, u8);
static void sub_804A840(u8);
u8 sub_804A2B4(u8 *, u8, u8);
static void sub_804A96C_alt(struct UnkStructD *, u8, u8, const u16 *, u8, u8, u16);
static void sub_804A96C(struct UnkStructD *, u8, u8, const u16 *, u8, u8, u16);
static void UNUSED BufferMovesString(u8 *, u8, u8);
void sub_804A51C(u8, u8, u8, u8, u8, u8);
static void SpriteCB_BouncingPokeballDepartEnd(struct Sprite *);
static bool8 sub_804C29C(void);
static void sub_804DC18(void);
static u8 TradeGetMultiplayerId(void);
static void sub_804E144(void);
static void Task_InGameTrade(u8);
static void GetInGameTradeMail(struct MailStruct *, const struct InGameTrade *);
static void sub_804DC88(void);
static void SpriteCB_BouncingPokeball(struct Sprite *);
static void SpriteCB_BouncingPokeballDepart(struct Sprite *);
static void sub_804BBE8(u8);
static void SpriteCB_LinkMonGlow(struct Sprite *);
static void SpriteCB_LinkMonShadow(struct Sprite *);
static void SpriteCB_CableEndSending(struct Sprite *);
static void SpriteCB_GbaScreen(struct Sprite *);
static void SpriteCB_BouncingPokeballArrive(struct Sprite *);
static void CheckPartnersMonForRibbons(void);
static void sub_804BBCC(void);
static void BufferInGameTradeMonName(void);
static void LoadTradeSequenceSpriteSheetsAndPalettes(void);
static void BufferTradeSceneStrings(void);
void sub_804DB84(void);

static EWRAM_DATA u8 *sMenuTextTileBuffers[13] = {0};
EWRAM_DATA struct MailStruct gTradeMail[6] = {0};
EWRAM_DATA u8 gSelectedTradeMonPositions[2] = {0};

struct TradeEwramSubstruct *gUnknown_03004824;

#include "data/trade.h"

// .text

void CB2_StartCreateTradeMenu(void)
{
    SetMainCallback2(CB2_CreateTradeMenu);
}

static void sub_8047CE8(void)
{
    u8 mpId;
    sub_804AFB8(&gWindowTemplate_81E725C, sMenuTextTileBuffers[0], gSaveBlock2.playerName, 0xC);
    mpId = GetMultiplayerId();
    sub_804AFB8(&gWindowTemplate_81E725C, sMenuTextTileBuffers[3], gLinkPlayers[mpId ^ 1].name, 0xC);
    sub_804AFB8(&gWindowTemplate_81E725C, sMenuTextTileBuffers[6], sActionTexts[0], 0x8);
    sub_804ACD8(sActionTexts[1], sMenuTextTileBuffers[8], 0x14);
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
    for (i = 0; i < 3; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i;
        CreateSprite(&spriteTemplate, gTradeUnknownSpriteCoords[language][0][0] + 32 * i, gTradeUnknownSpriteCoords[language][0][1], 1);
    }

    mpId = GetMultiplayerId();
    slen = StringLength(gLinkPlayers[mpId ^ 1].name);
    language = slen <= 5 ? 0 : 1;
    for (i = 0; i < 3; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + 3;
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
        spriteTemplate.tileTag += i + 6;
        CreateSprite(&spriteTemplate, 0xd6 + 32 * i, 0x98, 1);
    }

    for (i = 0; i < 5; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + 8;
        CreateSprite(&spriteTemplate, 0x18 + 32 * i, 0x96, 1);
    }
}

static void CB2_CreateTradeMenu(void)
{
    int i;

    switch (gMain.state)
    {
        case  0:
            gUnknown_03004824 = &ewram_2010000.unk_07000;
            sub_804AA88();
            ResetSpriteData();
            FreeAllSpritePalettes();
            ResetTasks();
            sub_804A964(&gUnknown_03004824->unk_00c8, BG_SCREEN_ADDR(5));
            SetVBlankCallback(sub_80489F4);
            InitMenuWindow(&gMenuTextWindowTemplate);
            Text_LoadWindowTemplate(&gWindowTemplate_81E6F84);
            Text_InitWindowWithTemplate(&gUnknown_03004824->window, &gWindowTemplate_81E6F84);
            gUnknown_03004824->unk_007a = TextWindow_SetBaseTileNum(20);
            TextWindow_LoadStdFrameGraphics(&gUnknown_03004824->window);
            Menu_EraseScreen();
            sub_809D51C();
            gUnknown_03004824->bufferPartyState = 0;
            gUnknown_03004824->callbackId = 0;
            gUnknown_03004824->unk_007c = 0;
            gUnknown_03004824->drawSelectedMonState[0] = 0;
            gUnknown_03004824->drawSelectedMonState[1] = 0;
            gUnknown_03004824->playerConfirmStatus = 0;
            gUnknown_03004824->partnerConfirmStatus = 0;
            gUnknown_03004824->timer = 0;
            gUnknown_03000508[0] = 0;
            gMain.state ++;
            sub_804AA0C(0);
            CpuFill16(0, ewram_2010000.tileBuffers, sizeof(ewram_2010000.tileBuffers));
            for (i = 0; i < 13; i ++)
                sMenuTextTileBuffers[i] = ewram_2010000.tileBuffers[i];
            break;
        case  1:
            gLinkType = 0x1122;
            OpenLink();
            for (i = 0; i < PARTY_SIZE; i ++)
                CreateMon(&gEnemyParty[i], 0, 0, 0x20, FALSE, 0, FALSE, 0);
            gMain.state ++;
            gUnknown_03004824->timer = 0;
            CreateTask(sub_8083C50, 1);
            break;
        case  2:
            gUnknown_03004824->timer ++;
            if (gUnknown_03004824->timer > 11)
            {
                gUnknown_03004824->timer = 0;
                gMain.state ++;
            }
            break;
        case  3:
            if (GetLinkPlayerCount_2() >= GetSavedPlayerCount())
            {
                if (IsLinkMaster())
                {
                    if (++gUnknown_03004824->timer > 30)
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
            if (gReceivedRemoteLinkPlayers == 1 && IsLinkPlayerDataExchangeComplete() == TRUE)
            {
                CalculatePlayerPartyCount();
                gMain.state ++;
            }
            break;
        case  5:
            if (sub_8048D44())
            {
                sub_804AF84();
                gMain.state ++;
            }
            break;
        case  6:
            CalculateEnemyPartyCount();
            Text_FillWindowRectDefPalette(&gUnknown_03004824->window, 0, 0, 0, 29, 19);
            REG_DISPCNT = 0;
            gUnknown_03004824->partyCounts[0] = gPlayerPartyCount;
            gUnknown_03004824->partyCounts[1] = gEnemyPartyCount;
            for (i = 0; i < gUnknown_03004824->partyCounts[0]; i ++)
                gUnknown_03004824->partySpriteIds[0][i] = CreateMonIcon(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG), SpriteCB_PokemonIcon, sTradeMonSpriteCoords[i][0] * 8 + 14, sTradeMonSpriteCoords[i][1] * 8 - 12, TRUE, GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
            for (i = 0; i < gUnknown_03004824->partyCounts[1]; i ++)
                gUnknown_03004824->partySpriteIds[1][i] = CreateMonIcon(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG, NULL), SpriteCB_PokemonIcon, sTradeMonSpriteCoords[6 + i][0] * 8 + 14, sTradeMonSpriteCoords[6 + i][1] * 8 - 12, TRUE, GetMonData(&gEnemyParty[i], MON_DATA_PERSONALITY));
            nullsub_5(2, 0);
            gMain.state ++;
            break;
        case  7:
            LoadHeldItemIconGraphics();
            CreateHeldItemIcons(&gUnknown_03004824->partyCounts[0], gUnknown_03004824->partySpriteIds[0], 0);
            gMain.state ++;
            break;
        case  8:
            CreateHeldItemIcons(&gUnknown_03004824->partyCounts[0], gUnknown_03004824->partySpriteIds[0], 1);
            gMain.state ++;
            break;
        case  9:
            sub_8047CE8();
            gMain.state ++;
            gUnknown_03004824->timer = 0;
            break;
        case 10:
            nullsub_5(4, 0);
            if (sub_804ABF8())
                gMain.state ++;
            break;
        case 11:
            sub_8047D58();
            gMain.state ++;
            break;
        case 12:
            sub_8047E44();
            gUnknown_03004824->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, sTradeMonSpriteCoords[0][0] * 8 + 32, sTradeMonSpriteCoords[0][1] * 8, 2);
            gUnknown_03004824->cursorPosition = 0;
            gMain.state ++;
            nullsub_5(6, 0);
            break;
        case 13:
            ComputePartyTradeableFlags(0);
            sub_804A41C(0);
            gUnknown_03004824->bg2hofs = 0;
            gUnknown_03004824->bg3hofs = 0;
            SetActiveMenuOptions();
            gMain.state ++;
            nullsub_5(7, 0);
            PlayBGM(MUS_SCHOOL);
            break;
        case 14:
            ComputePartyTradeableFlags(1);
            sub_804A41C(1);
            gMain.state ++;
            // fallthrough
        case 15:
            sub_8048B0C(0);
            gMain.state ++;
            break;
        case 16:
            sub_8048B0C(1);
            gMain.state ++;
            break;
        case 17:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gMain.state ++;
            break;
        case 18:
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
            gMain.state ++;
            break;
        case 19:
            ComputePartyHPBarLevels(0);
            gMain.state ++;
            break;
        case 20:
            ComputePartyHPBarLevels(1);
            SetTradePartyHPBarSprites();
            gMain.state ++;
            break;
        case 21:
            if (!gPaletteFade.active)
            {
                gMain.callback1 = sub_80494D8;
                SetMainCallback2(sub_8048AB4);
                gUnknown_03000508[0] = 0;
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_80484F4(void)
{
    int i;
    struct UnkStructF *unkStructF;

    switch (gMain.state)
    {
        case  0:
            gUnknown_03004824 = &ewram_2010000.unk_07000;
            ResetSpriteData();
            FreeAllSpritePalettes();
            ResetTasks();
            sub_804A964(&gUnknown_03004824->unk_00c8, BG_SCREEN_ADDR(5));
            SetVBlankCallback(sub_80489F4);
            InitMenuWindow(&gMenuTextWindowTemplate);
            Text_LoadWindowTemplate(&gWindowTemplate_81E6F84);
            Text_InitWindowWithTemplate(&gUnknown_03004824->window, &gWindowTemplate_81E6F84);
            gUnknown_03004824->unk_007a = TextWindow_SetBaseTileNum(20);
            TextWindow_LoadStdFrameGraphics(&gUnknown_03004824->window);
            Menu_EraseScreen();
            sub_809D51C();
            gUnknown_03004824->bufferPartyState = 0;
            gUnknown_03004824->callbackId = 0;
            gUnknown_03004824->unk_007c = 0;
            gUnknown_03004824->drawSelectedMonState[0] = 0;
            gUnknown_03004824->drawSelectedMonState[1] = 0;
            gUnknown_03004824->playerConfirmStatus = 0;
            gUnknown_03004824->partnerConfirmStatus = 0;
            gUnknown_03004824->timer = 0;
            gUnknown_03000508[0] = 0;
            gMain.state ++;
            for (i = 0; i < 13; i ++)
                sMenuTextTileBuffers[i] = ewram_2010000.tileBuffers[i];
            break;
        case  1:
            gMain.state ++;
            gUnknown_03004824->timer = 0;
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
            gUnknown_03004824->partyCounts[0] = gPlayerPartyCount;
            gUnknown_03004824->partyCounts[1] = gEnemyPartyCount;
            sub_804A41C(0);
            sub_804A41C(1);
            for (i = 0; i < gUnknown_03004824->partyCounts[0]; i ++)
                gUnknown_03004824->partySpriteIds[0][i] = CreateMonIcon(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL), SpriteCB_PokemonIcon, sTradeMonSpriteCoords[i][0] * 8 + 14, sTradeMonSpriteCoords[i][1] * 8 - 12, TRUE, GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
            for (i = 0; i < gUnknown_03004824->partyCounts[1]; i ++)
                gUnknown_03004824->partySpriteIds[1][i] = CreateMonIcon(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES_OR_EGG, NULL), SpriteCB_PokemonIcon, sTradeMonSpriteCoords[6 + i][0] * 8 + 14, sTradeMonSpriteCoords[6 + i][1] * 8 - 12, TRUE, GetMonData(&gEnemyParty[i], MON_DATA_PERSONALITY));
            nullsub_5(2, 0);
            gMain.state ++;
            break;
        case  7:
            LoadHeldItemIconGraphics();
            CreateHeldItemIcons(&gUnknown_03004824->partyCounts[0], gUnknown_03004824->partySpriteIds[0], 0);
            gMain.state ++;
            break;
        case  8:
            CreateHeldItemIcons(&gUnknown_03004824->partyCounts[0], gUnknown_03004824->partySpriteIds[0], 1);
            gMain.state ++;
            break;
        case  9:
            sub_8047CE8();
            gMain.state ++;
            gUnknown_03004824->timer = 0;
            break;
        case 10:
            nullsub_5(4, 0);
            if (sub_804ABF8())
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
            if (gUnknown_03004824->cursorPosition < 6)
                gUnknown_03004824->cursorPosition = unkStructF->unk_0009;
            else
                gUnknown_03004824->cursorPosition = unkStructF->unk_0009 + 6;
            gUnknown_03004824->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, sTradeMonSpriteCoords[gUnknown_03004824->cursorPosition][0] * 8 + 32, sTradeMonSpriteCoords[gUnknown_03004824->cursorPosition][1] * 8, 2);
            gMain.state = 15;
            nullsub_5(6, 0);
            break;
        case 15:
            sub_8048B0C(0);
            gMain.state ++;
            break;
        case 16:
            sub_8048B0C(1);
            gUnknown_03004824->bg2hofs = 0;
            gUnknown_03004824->bg3hofs = 0;
            SetActiveMenuOptions();
            nullsub_5(7, 0);
            gMain.state ++;
            break;
        case 17:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
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
                SetMainCallback2(sub_8048AB4);
                gUnknown_03000508[0] = 0;
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_80489F4(void)
{
    sub_804A940(&gUnknown_03004824->unk_00c8);
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB_FadeToStartTrade(void)
{
    if (++gUnknown_03004824->timer >= 16)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gUnknown_03004824->callbackId = 10;
    }
}

/*static*/ void sub_8048A50(void)
{
    if (!gPaletteFade.active)
    {
        gSelectedTradeMonPositions[0] = gUnknown_03004824->cursorPosition;
        gSelectedTradeMonPositions[1] = gUnknown_03004824->partnerCursorPosition;
        SetCloseLinkCallback();
        gUnknown_03004824->callbackId = 13;
    }
}

/*static*/ void sub_8048A90(void)
{
    if (!gReceivedRemoteLinkPlayers)
    {
        gMain.callback1 = NULL;
        SetMainCallback2(sub_804B41C);
    }
}

static void sub_8048AB4(void)
{
    sub_8049DE0();
    sub_804AB30();
    sub_8049ED4(0);
    sub_8049ED4(1);
    REG_BG2HOFS = gUnknown_03004824->bg2hofs++;
    REG_BG3HOFS = gUnknown_03004824->bg3hofs--;
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void sub_8048B0C(u8 state)
{
    int i;

    switch (state)
    {
        case 0:
            for (i = 0; i < 48; i ++)
            {
                gPlttBufferUnfaded[i] = *(gTradeMenu_Pal + i);
                gPlttBufferFaded[i] = *(gTradeMenu_Pal + i);
            }
            DmaCopyLarge16(3, gTradeMenu_Gfx, (void *)BG_VRAM, 0x1280, 0x1000);
            for (i = 0; i < 0x400; i ++)
                gUnknown_03004824->unk_00c8.tilemapBuffer[i] = gTradeMenu_Tilemap[i];
            DmaCopy16Defvars(3, sTradeStripesBG2Tilemap, BG_SCREEN_ADDR(6), 0x800);
            break;
        case 1:
            DmaCopy16Defvars(3, sTradeStripesBG3Tilemap, BG_SCREEN_ADDR(7), 0x800);
            PrintPartyLevelsAndGenders(0);
            PrintPartyLevelsAndGenders(1);
            sub_804A938(&gUnknown_03004824->unk_00c8);
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
        if (i < gUnknown_03004824->partyCounts[0])
        {
            gSprites[gUnknown_03004824->partySpriteIds[0][i]].invisible = FALSE;
            gUnknown_03004824->optionsActive[i] = TRUE;
        }
        else
        {
            gUnknown_03004824->optionsActive[i] = FALSE;
        }
        if (i < gUnknown_03004824->partyCounts[1])
        {
            gSprites[gUnknown_03004824->partySpriteIds[1][i]].invisible = FALSE;
            gUnknown_03004824->optionsActive[i + 6] = TRUE;
        }
        else
        {
            gUnknown_03004824->optionsActive[i + 6] = FALSE;
        }
    }
    gUnknown_03004824->optionsActive[12] = TRUE;
}

static void nullsub_5(u8 a0, u8 a1) {}

// why not just use memcpy?
static void Trade_Memcpy(void *dataDest, void *dataSrc, u32 size)
{
    u8 *dest = dataDest;
    u8 *src = dataSrc;
    int i;
    for (i = 0; i < size; i ++) dest[i] = src[i];
}

static bool8 sub_8048D44(void)
{
    u8 mpId = GetMultiplayerId();
    int i;
    u16 species;
    u8 nickname[11];
    struct Pokemon *pokemon;

    SetLinkDebugValues(gUnknown_03004824->bufferPartyState / 100, gUnknown_03004824->bufferPartyState % 100);
    switch (gUnknown_03004824->bufferPartyState)
    {
        case  0:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[0], 2 * sizeof(struct Pokemon));
            gUnknown_03004824->bufferPartyState ++;
            break;
        case  1:
            if (IsLinkTaskFinished())
            {
                if (GetBlockReceivedStatus() == 0)
                {
                    gUnknown_03004824->bufferPartyState ++;
                }
                else
                {
                    ResetBlockReceivedFlags();
                    gUnknown_03004824->bufferPartyState ++;
                }
            }
            break;
        case  2:
            if (mpId == 0)
            {
                SendBlockRequest(1);
            }
            gUnknown_03004824->bufferPartyState ++;
            break;
        case  3:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[0], gBlockRecvBuffer[mpId ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                gUnknown_03004824->bufferPartyState ++;
            }
            break;
        case  4:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[2], 2 * sizeof(struct Pokemon));
            gUnknown_03004824->bufferPartyState ++;
            break;
        case  5:
            if (mpId == 0)
            {
                SendBlockRequest(1);
            }
            gUnknown_03004824->bufferPartyState ++;
            break;
        case  6:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[2], gBlockRecvBuffer[mpId ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                gUnknown_03004824->bufferPartyState ++;
            }
            break;
        case  7:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[4], 2 * sizeof(struct Pokemon));
            gUnknown_03004824->bufferPartyState ++;
            break;
        case  8:
            if (mpId == 0)
            {
                SendBlockRequest(1);
            }
            gUnknown_03004824->bufferPartyState ++;
            break;
        case  9:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[4], gBlockRecvBuffer[mpId ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                gUnknown_03004824->bufferPartyState ++;
            }
            break;
        case 10:
            Trade_Memcpy(gBlockSendBuffer, &gSaveBlock1.mail[0], 6 * sizeof(struct MailStruct) + 4);
            gUnknown_03004824->bufferPartyState ++;
            break;
        case 11:
            if (mpId == 0)
            {
                SendBlockRequest(3);
            }
            gUnknown_03004824->bufferPartyState ++;
            break;
        case 12:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gTradeMail[0], gBlockRecvBuffer[mpId ^ 1], 6 * sizeof(struct MailStruct));
                ResetBlockReceivedFlags();
                gUnknown_03004824->bufferPartyState ++;
            }
            break;
        case 13:
            Trade_Memcpy(gBlockSendBuffer, gSaveBlock1.giftRibbons, 11);
            gUnknown_03004824->bufferPartyState ++;
            break;
        case 14:
            if (mpId == 0)
            {
                SendBlockRequest(4);
            }
            gUnknown_03004824->bufferPartyState ++;
            break;
        case 15:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(gUnknown_03004824->giftRibbons, gBlockRecvBuffer[mpId ^ 1], 11);
                ResetBlockReceivedFlags();
                gUnknown_03004824->bufferPartyState ++;
            }
            break;
        case 16:
            pokemon = gEnemyParty;
            for (i = 0; i < PARTY_SIZE; i ++)
            {
                if ((species = GetMonData(pokemon, MON_DATA_SPECIES)) != SPECIES_NONE && species == SPECIES_SHEDINJA && GetMonData(pokemon, MON_DATA_LANGUAGE) != LANGUAGE_JAPANESE)
                {
                    GetMonData(pokemon, MON_DATA_NICKNAME, nickname);
                    if (!StringCompareWithoutExtCtrlCodes(nickname, sText_ShedinjaJP))
                        SetMonData(pokemon, MON_DATA_NICKNAME, gSpeciesNames[SPECIES_SHEDINJA]);
                }
                pokemon ++;
            }
            return TRUE;
    }
    return FALSE;
}

static void sub_8049088(void)
{
    u8 string[28];
    StringCopy(string, sText_IsThisTradeOkay);
    sub_804ACD8(string, BG_CHAR_ADDR(4) + gUnknown_03004824->bottomTextTileStart * 32, 20);
}

static void sub_80490BC(u8 mpId, u8 status)
{
    if (status & 1)
    {
        switch (gBlockRecvBuffer[mpId][0])
        {
            case 0xeeaa:
                gUnknown_03004824->playerSelectStatus = 2;
                break;
            case 0xaabb:
                gUnknown_03004824->playerSelectStatus = 1;
                break;
            case 0xbbbb:
                gUnknown_03004824->playerConfirmStatus = 1;
                break;
            case 0xbbcc:
                gUnknown_03004824->playerConfirmStatus = 2;
                break;
        }
        ResetBlockReceivedFlag(0);
    }
    if (status & 2)
    {
        switch (gBlockRecvBuffer[1][0])
        {
            case 0xeeaa:
                gUnknown_03004824->partnerSelectStatus = 2;
                break;
            case 0xaabb:
                gUnknown_03004824->partnerCursorPosition = gBlockRecvBuffer[1][1] + 6;
                gUnknown_03004824->partnerSelectStatus = 1;
                break;
            case 0xbbbb:
                gUnknown_03004824->partnerConfirmStatus = 1;
                break;
            case 0xbbcc:
                gUnknown_03004824->partnerConfirmStatus = 2;
                break;
        }
        ResetBlockReceivedFlag(1);
    }
}

static void sub_80491E4(u8 mpId, u8 status)
{
    if (status & 1)
    {
        switch (gBlockRecvBuffer[0][0])
        {
            case 0xeebb:
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                sub_804AA0C(4);
                gUnknown_03004824->callbackId = 11;
                break;
            case 0xeecc:
                sub_804AA0C(5);
                gUnknown_03004824->callbackId = 8;
                break;
            case 0xdddd:
                gUnknown_03004824->partnerCursorPosition = gBlockRecvBuffer[0][1] + 6;
                SetSelectedMon(gUnknown_03004824->cursorPosition);
                SetSelectedMon(gUnknown_03004824->partnerCursorPosition);
                gUnknown_03004824->callbackId = 7;
                break;
            case 0xccdd:
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                gUnknown_03004824->callbackId = 10;
                break;
            case 0xddee:
                sub_804AA0C(1);
                gUnknown_03004824->callbackId = 8;
                break;
        }
        ResetBlockReceivedFlag(0);
    }
    if (status & 2)
    {
        ResetBlockReceivedFlag(1);
    }
}

static void sub_80492D8(void)
{
    if (gUnknown_03004824->playerSelectStatus && gUnknown_03004824->partnerSelectStatus)
    {
        if (gUnknown_03004824->playerSelectStatus == 1 && gUnknown_03004824->partnerSelectStatus == 1)
        {
            gUnknown_03004824->callbackId = 6;
            gUnknown_03004824->linkData[0] = 0xdddd;
            gUnknown_03004824->linkData[1] = gUnknown_03004824->cursorPosition;
            QueueAction(5, 0);
            gUnknown_03004824->playerSelectStatus = gUnknown_03004824->partnerSelectStatus = 0;
        }
        else if (gUnknown_03004824->playerSelectStatus == 1 && gUnknown_03004824->partnerSelectStatus == 2)
        {
            sub_804AA0C(1);
            gUnknown_03004824->linkData[0] = 0xeecc;
            gUnknown_03004824->linkData[1] = 0;
            QueueAction(5, 0);
            gUnknown_03004824->playerConfirmStatus = gUnknown_03004824->partnerConfirmStatus = 0;
            gUnknown_03004824->playerSelectStatus = gUnknown_03004824->partnerSelectStatus = 0;
            gUnknown_03004824->callbackId = 8;
        }
        else if (gUnknown_03004824->playerSelectStatus == 2 && gUnknown_03004824->partnerSelectStatus == 1)
        {
            sub_804AA0C(5);
            gUnknown_03004824->linkData[0] = 0xddee;
            gUnknown_03004824->linkData[1] = 0;
            QueueAction(5, 0);
            gUnknown_03004824->playerConfirmStatus = gUnknown_03004824->partnerConfirmStatus = 0;
            gUnknown_03004824->playerSelectStatus = gUnknown_03004824->partnerSelectStatus = 0;
            gUnknown_03004824->callbackId = 8;
        }
        else if (gUnknown_03004824->playerSelectStatus == 2 && gUnknown_03004824->partnerSelectStatus == 2)
        {
            gUnknown_03004824->linkData[0] = 0xeebb;
            gUnknown_03004824->linkData[1] = 0;
            QueueAction(5, 0);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_03004824->playerSelectStatus = gUnknown_03004824->partnerSelectStatus = 0;
            gUnknown_03004824->callbackId = 11;
        }
    }
    if (gUnknown_03004824->playerConfirmStatus && gUnknown_03004824->partnerConfirmStatus)
    {
        if (gUnknown_03004824->playerConfirmStatus == 1 && gUnknown_03004824->partnerConfirmStatus == 1)
        {
            gUnknown_03004824->linkData[0] = 0xccdd;
            gUnknown_03004824->linkData[1] = 0;
            QueueAction(5, 0);
            gUnknown_03004824->playerConfirmStatus = 0;
            gUnknown_03004824->partnerConfirmStatus = 0;
            gUnknown_03004824->callbackId = 9;
        }
        if (gUnknown_03004824->playerConfirmStatus == 2 || gUnknown_03004824->partnerConfirmStatus == 2)
        {
            sub_804AA0C(1);
            gUnknown_03004824->linkData[0] = 0xddee;
            gUnknown_03004824->linkData[1] = 0;
            QueueAction(5, 0);
            gUnknown_03004824->playerConfirmStatus = 0;
            gUnknown_03004824->partnerConfirmStatus = 0;
            gUnknown_03004824->callbackId = 8;
        }
    }
}

static void sub_80494D8(void)
{
    u8 mpId = GetMultiplayerId();
    u8 status;
    if ((status = GetBlockReceivedStatus()))
    {
        if (mpId == 0)
            sub_80490BC(mpId, status);
        else
            sub_80491E4(mpId, status);
        ResetBlockReceivedFlags();
    }
    if (mpId == 0)
        sub_80492D8();
}

static u8 GetNewCursorPosition(u8 oldPosition, u8 direction)
{
    int i;
    u8 newPosition = 0;
    for (i = 0; i < PARTY_SIZE; i ++)
    {
        if (gUnknown_03004824->optionsActive[sCursorMoveDestinations[oldPosition][direction][i]] == TRUE)
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
    if (newPosition == 12) // CANCEL
    {
        StartSpriteAnim(&gSprites[gUnknown_03004824->cursorSpriteId], 1);
        gSprites[gUnknown_03004824->cursorSpriteId].x = 0xe0;
        gSprites[gUnknown_03004824->cursorSpriteId].y = 0xa0;
    }
    else
    {
        StartSpriteAnim(&gSprites[gUnknown_03004824->cursorSpriteId], 0);
        gSprites[gUnknown_03004824->cursorSpriteId].x = sTradeMonSpriteCoords[newPosition][0] * 8 + 32;
        gSprites[gUnknown_03004824->cursorSpriteId].y = sTradeMonSpriteCoords[newPosition][1] * 8;
    }
    if (*cursorPosition != newPosition)
    {
        PlaySE(SE_SELECT);
    }
    *cursorPosition = newPosition;
}

static void sub_8049620(void)
{
    sub_804AA0C(0);
    gUnknown_03004824->callbackId = 5;
    if (GetMultiplayerId() == 1)
    {
        gUnknown_03004824->linkData[0] = 0xaabb;
        gUnknown_03004824->linkData[1] = gUnknown_03004824->cursorPosition;
        Trade_SendData(gUnknown_03004824);
    }
    else
    {
        gUnknown_03004824->playerSelectStatus = 1;
    }
}

static void sub_8049680(void)
{
    int i;
    if (JOY_REPT(DPAD_UP))
    {
        TradeMenuMoveCursor(&gUnknown_03004824->cursorPosition, 0);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        TradeMenuMoveCursor(&gUnknown_03004824->cursorPosition, 1);
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        TradeMenuMoveCursor(&gUnknown_03004824->cursorPosition, 2);
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        TradeMenuMoveCursor(&gUnknown_03004824->cursorPosition, 3);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gUnknown_03004824->cursorPosition < PARTY_SIZE)
        {
            TextWindow_DrawStdFrame(&gUnknown_03004824->window, 18, 14, 28, 19);
            Menu_PrintItems(19, 15, 2, (const struct MenuAction *)sSelectTradeMonActions);
            InitMenu(0, 19, 15, 2, 0, 9);
            gUnknown_03004824->callbackId = 1;
        }
        else if (gUnknown_03004824->cursorPosition < 2 * PARTY_SIZE)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_03004824->callbackId = 2;
        }
        else if (gUnknown_03004824->cursorPosition == 2 * PARTY_SIZE)
        {
            TextWindow_DrawStdFrame(&gUnknown_03004824->window, 24, 14, 29, 19);
            InitYesNoMenu(24, 14, 4);
            gUnknown_03004824->callbackId = 4;
            sub_804ACD8(sActionTexts[4], BG_CHAR_ADDR(4) + 32 * gUnknown_03004824->bottomTextTileStart, 20);
        }
    }
    if (JOY_NEW(R_BUTTON))
    {
        for (i = 0; i < 10; i ++)
        {
            gUnknown_03004824->linkData[i] = i;
        }
        Trade_SendData(gUnknown_03004824);
    }
}

static void sub_8049804(void)
{
    Menu_DestroyCursor();
    sub_804A80C();
    gUnknown_03004824->callbackId = 0;
    gSprites[gUnknown_03004824->cursorSpriteId].invisible = FALSE;
    sub_804ACD8(sActionTexts[1], BG_CHAR_ADDR(4) + 32 * gUnknown_03004824->bottomTextTileStart, 20);
}

static void sub_8049860(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gUnknown_03004824->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gUnknown_03004824->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_03004824->callbackId = 2;
        }
        else if (sub_80499F0(gUnknown_03004824->isLiveMon[0], gUnknown_03004824->partyCounts[0], gUnknown_03004824->cursorPosition) == 0)
        {
            QueueAction(3, 2);
            gUnknown_03004824->callbackId = 8;
        }
        else
        {
            sub_8049620();
            gSprites[gUnknown_03004824->cursorSpriteId].invisible = TRUE;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sub_8049804();
    }
}

static void CB_ChooseMonAfterButtonPress(void)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sub_8049804();
    }
}

static void sub_804997C(void)
{
    if (!gPaletteFade.active)
    {
        if (gUnknown_03004824->cursorPosition < PARTY_SIZE)
        {
            ShowPokemonSummaryScreen(gPlayerParty, gUnknown_03004824->cursorPosition, gUnknown_03004824->partyCounts[0] - 1, sub_80484F4, PSS_MODE_NO_MOVE_ORDER_EDIT);
        }
        else
        {
            ShowPokemonSummaryScreen(gEnemyParty, gUnknown_03004824->cursorPosition - 6, gUnknown_03004824->partyCounts[1] - 1, sub_80484F4, PSS_MODE_NO_MOVE_ORDER_EDIT);
        }
    }
}

static u8 sub_80499F0(const u8 *src, u8 partyCount, u8 tradeMenuCursorPosition)
{
    u8 retval = 0;
    int i;
    for (i = 0; i < partyCount; i ++)
    {
        if (tradeMenuCursorPosition != i)
        {
            retval += src[i];
        }
    }
    return retval;
}

static void sub_8049A20(void)
{
    u8 unk_0051[12];
    int i;
    for (i = 0; i < gUnknown_03004824->partyCounts[0]; i ++)
    {
        unk_0051[i] = gUnknown_03004824->isLiveMon[0][i];
    }
    if (sub_80499F0(unk_0051, gUnknown_03004824->partyCounts[0], gUnknown_03004824->cursorPosition) == 0)
    {
        QueueAction(3, 2);
        gUnknown_03004824->linkData[0] = 0xbbcc;
        QueueAction(0xb4, 0);
    }
    else
    {
        QueueAction(3, 1);
        gUnknown_03004824->linkData[0] = 0xbbbb;
        if (IsLinkTaskFinished())
        {
            Trade_SendData(gUnknown_03004824);
        }
    }
}

static void sub_8049AC0(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gUnknown_03004824->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gUnknown_03004824->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            sub_8049A20();
        }
        else
        {
            QueueAction(3, 1);
            if (IsLinkTaskFinished())
            {
                gUnknown_03004824->linkData[0] = 0xbbcc;
                Trade_SendData(gUnknown_03004824);
            }
        }
        gUnknown_03004824->callbackId = 100;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        QueueAction(3, 1);
        if (Menu_GetCursorPos() == 0)
        {
            gUnknown_03004824->unk_007c = Menu_MoveCursor(+1);
        }
        gUnknown_03004824->linkData[0] = 0xbbcc;
        Trade_SendData(gUnknown_03004824);
        gUnknown_03004824->callbackId = 100;
    }
}

static void sub_8049BC0(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gUnknown_03004824->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gUnknown_03004824->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            sub_804AA0C(4);
            gUnknown_03004824->linkData[0] = 0xeeaa;
            gUnknown_03004824->linkData[1] = 0;
            QueueAction(5, 0);
            gSprites[gUnknown_03004824->cursorSpriteId].invisible = TRUE;
            gUnknown_03004824->callbackId = 100;
        }
        else
        {
            sub_8049804();
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sub_8049804();
    }
}

static void sub_8049C8C(void)
{
    if (GetMultiplayerId() == 0)
    {
        SetSelectedMon(gUnknown_03004824->cursorPosition);
        SetSelectedMon(gUnknown_03004824->partnerCursorPosition);
    }
    gUnknown_03004824->callbackId = 7;
}

static void CB_PrintIsThisTradeOkay(void)
{
    if (gUnknown_03004824->drawSelectedMonState[0] == 5 && gUnknown_03004824->drawSelectedMonState[1] == 5)
    {
        sub_8049088();
        gUnknown_03004824->callbackId = 14;
    }
}

static void DisplayMessageAndContinueTask(void)
{
    gUnknown_03004824->timer++;
    if (gUnknown_03004824->timer > 120)
    {
        TextWindow_DrawStdFrame(&gUnknown_03004824->window, 24, 14, 29, 19);
        InitYesNoMenu(24, 14, 4);
        gUnknown_03004824->timer = 0;
        gUnknown_03004824->callbackId = 3;
    }
}

static void sub_8049D44(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sub_804A840(0);
        sub_804A840(1);
        gUnknown_03004824->callbackId = 0;
        gSprites[gUnknown_03004824->cursorSpriteId].invisible = FALSE;
    }
}

static void sub_8049D9C(void)
{
    if (!gPaletteFade.active)
    {
        SetCloseLinkCallback();
        gUnknown_03004824->callbackId = 12;
    }
}

static void sub_8049DC4(void)
{
    if (gReceivedRemoteLinkPlayers == 0)
    {
        SetMainCallback2(sub_805465C);
    }
}

static void sub_8049DE0(void)
{
    switch (gUnknown_03004824->callbackId)
    {
        case  0:
            sub_8049680();
            break;
        case  1:
            sub_8049860();
            break;
        case  2:
            sub_804997C();
            break;
        case  3:
            sub_8049AC0();
            break;
        case  4:
            sub_8049BC0();
            break;
        case  6:
            sub_8049C8C();
            break;
        case  7:
            CB_PrintIsThisTradeOkay();
            break;
        case  8:
            sub_8049D44();
            break;
        case  9:
            CB_FadeToStartTrade();
            break;
        case 10:
            sub_8048A50();
            break;
        case 11:
            sub_8049D9C();
            break;
        case 12:
            sub_8049DC4();
            break;
        case 13:
            sub_8048A90();
            break;
        case 14:
            DisplayMessageAndContinueTask();
            break;
        case 15:
            CB_ChooseMonAfterButtonPress();
            break;
    }
}

static void SetSelectedMon(u8 cursorPosition)
{
    u8 whichParty = cursorPosition / 6;
    if (gUnknown_03004824->drawSelectedMonState[whichParty] == 0)
    {
        gUnknown_03004824->drawSelectedMonState[whichParty] = 1;
        gUnknown_03004824->selectedMonIdx[whichParty] = cursorPosition;
    }
}

// TODO: Figure out what the f**k is going on here
#ifdef NONMATCHING
static void sub_8049ED4(u8 a0)
{
    u8 i;
    s8 stringLength;
    u8 whichParty = 1;
    u8 whichPokemon;
    u8 string1[40];
    u8 string2[56];
    u8 temp0 = gUnknown_03004824->selectedMonIdx[a0];
    if (temp0 < PARTY_SIZE)
        whichParty = 0;
    whichPokemon = temp0 % PARTY_SIZE;

    switch (gUnknown_03004824->drawSelectedMonState[a0])
    {
        case 1:
            for (i = 0; i < gUnknown_03004824->partyCounts[a0]; i ++)
            {
                gSprites[gUnknown_03004824->partySpriteIds[whichParty][i]].invisible = TRUE;
            }
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].invisible = FALSE;
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].data[0] = 20;
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].data[2] = (sTradeMonSpriteCoords[6 * whichParty + whichPokemon][0] + sTradeMonSpriteCoords[6 * whichParty + whichPokemon + 1][0]) / 2 * 8 + 14;
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].data[4] = sTradeMonSpriteCoords[6 * whichParty + whichPokemon][1] * 8 - 12;
            StoreSpriteCallbackInData6(&gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]], SpriteCB_PokemonIcon);
            gUnknown_03004824->drawSelectedMonState[a0] ++;
            sub_8078A34(&gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]]);
            Menu_DestroyCursor();
            Text_FillWindowRectDefPalette(&gUnknown_03004824->window, 0, gUnknown_0820C330[whichParty][0], 0, gUnknown_0820C330[whichParty][1], 19);
            sub_804A96C_alt(&gUnknown_03004824->unk_00c8, 15 * a0, 0, sTradePartyBoxTilemap, 15, 17, 0);
            if (whichParty == 0)
            {
                sub_804A80C();
            }
            break;
        case 2:
            if (gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].callback == SpriteCB_PokemonIcon)
            {
                gUnknown_03004824->drawSelectedMonState[a0] = 3;
            }
            break;
        case 3:
            sub_804A96C_alt(&gUnknown_03004824->unk_00c8, 15 * whichParty, 0, sTradePartyBoxTilemap, 15, 17, 0);
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].x = (sTradeMonSpriteCoords[6 * whichParty + whichPokemon ][0] + sTradeMonSpriteCoords[6 * whichParty + whichPokemon + 1][0]) / 2 * 8 + 14;
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].y = sTradeMonSpriteCoords[6 * whichParty + whichPokemon ][1] * 8 - 12;
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].x2 = 0;
            gSprites[gUnknown_03004824->partySpriteIds[whichParty][whichPokemon]].y2 = 0;
            stringLength = sub_804A2B4(string1 + 6, whichParty, whichPokemon);
            string1[0] = 0xFC;
            string1[1] = 0x06;
            string1[2] = 0x04;
            string1[3] = 0xFC;
            string1[4] = 0x11;
            string1[5] = (64 - stringLength) / 2;
            Text_InitWindowAndPrintText(&gUnknown_03004824->window, string1, gUnknown_03004824->unk_007a + whichParty * 6 * 32, gUnknown_0820C334[whichParty][0], gUnknown_0820C334[whichParty][1]);
            BufferMovesString(string2, whichParty, whichPokemon);
            Text_InitWindowAndPrintText(&gUnknown_03004824->window, gOtherText_Terminator2, gUnknown_03004824->unk_007a + whichParty * 6 * 32 + 32, gUnknown_0820C334[whichParty + 1][0], gUnknown_0820C334[whichParty + 1][1] + 1);
            Text_InitWindowAndPrintText(&gUnknown_03004824->window, string2, gUnknown_03004824->unk_007a + whichParty * 6 * 32 + 38, gUnknown_0820C334[whichParty + 1][0], gUnknown_0820C334[whichParty + 1][1] + 1);
            gUnknown_03004824->drawSelectedMonState[a0] ++;
            break;
        case 4:
            sub_804ACD8(sActionTexts[5], BG_CHAR_ADDR(4) + 32 * gUnknown_03004824->bottomTextTileStart, 20);
            sub_804A51C(a0, whichPokemon, sSelectedMonLevelGenderCoords[a0][0] + 4, sSelectedMonLevelGenderCoords[a0][1] + 1, sSelectedMonLevelGenderCoords[a0][0], sSelectedMonLevelGenderCoords[a0][1]);
            gUnknown_03004824->drawSelectedMonState[a0] ++;
            break;
    }
}
#else
NAKED
static void sub_8049ED4(u8 a0)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                    "\tmov r7, r10\n"
                    "\tmov r6, r9\n"
                    "\tmov r5, r8\n"
                    "\tpush {r5-r7}\n"
                    "\tsub sp, 0x74\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tmov r10, r0\n"
                    "\tldr r0, _08049F24 @ =gUnknown_03004824\n"
                    "\tldr r5, [r0]\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0x82\n"
                    "\tadd r0, r10\n"
                    "\tldrb r0, [r0]\n"
                    "\tmovs r1, 0x1\n"
                    "\tstr r1, [sp, 0x6C]\n"
                    "\tcmp r0, 0x5\n"
                    "\tbhi _08049EFE\n"
                    "\tmovs r2, 0\n"
                    "\tstr r2, [sp, 0x6C]\n"
                    "_08049EFE:\n"
                    "\tmovs r1, 0x6\n"
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
                    "_08049F24: .4byte gUnknown_03004824\n"
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
                    "\tldr r2, _0804A09C @ =gUnknown_03004824\n"
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
                    "\tldr r1, _0804A09C @ =gUnknown_03004824\n"
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
                    "\tldr r0, _0804A09C @ =gUnknown_03004824\n"
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
                    "\tldr r1, _0804A0A0 @ =SpriteCB_PokemonIcon\n"
                    "\tbl StoreSpriteCallbackInData6\n"
                    "\tldr r2, _0804A09C @ =gUnknown_03004824\n"
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
                    "\tbl sub_8078A34\n"
                    "\tbl Menu_DestroyCursor\n"
                    "\tldr r3, _0804A09C @ =gUnknown_03004824\n"
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
                    "\tldr r1, _0804A09C @ =gUnknown_03004824\n"
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
                    "\tbl sub_804A80C\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_0804A094: .4byte gSprites\n"
                    "_0804A098: .4byte sTradeMonSpriteCoords\n"
                    "_0804A09C: .4byte gUnknown_03004824\n"
                    "_0804A0A0: .4byte SpriteCB_PokemonIcon\n"
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
                    "\tldr r0, _0804A0E0 @ =SpriteCB_PokemonIcon\n"
                    "\tcmp r1, r0\n"
                    "\tbeq _0804A0D4\n"
                    "\tb _0804A294\n"
                    "_0804A0D4:\n"
                    "\tmovs r0, 0x3\n"
                    "\tstrb r0, [r3]\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_0804A0DC: .4byte gSprites\n"
                    "_0804A0E0: .4byte SpriteCB_PokemonIcon\n"
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
                    "\tldr r0, _0804A22C @ =gUnknown_03004824\n"
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
                    "\tbl sub_804A2B4\n"
                    "\tadd r1, sp, 0xC\n"
                    "\tmovs r3, 0xFC\n"
                    "\tstrb r3, [r1]\n"
                    "\tadds r2, r1, 0\n"
                    "\tmovs r1, 0x6\n"
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
                    "\tldr r0, _0804A22C @ =gUnknown_03004824\n"
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
                    "\tldr r0, _0804A22C @ =gUnknown_03004824\n"
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
                    "\tldr r0, _0804A22C @ =gUnknown_03004824\n"
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
                    "\tldr r2, _0804A22C @ =gUnknown_03004824\n"
                    "\tldr r1, [r2]\n"
                    "\tb _0804A28A\n"
                    "\t.align 2, 0\n"
                    "_0804A228: .4byte sTradeMovesBoxTilemap\n"
                    "_0804A22C: .4byte gUnknown_03004824\n"
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
                    "\tbl sub_804ACD8\n"
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
                    "\tbl sub_804A51C\n"
                    "\tldr r0, _0804A2B0 @ =gUnknown_03004824\n"
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
                    "_0804A2A8: .4byte 0x06010000\n"
                    "_0804A2AC: .4byte sSelectedMonLevelGenderCoords\n"
                    "_0804A2B0: .4byte gUnknown_03004824");
}
#endif

u8 sub_804A2B4(u8 *str, u8 whichParty, u8 partyIdx)
{
    u8 nickname[11];
    if (whichParty == 0)
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
    u16 moves[4];
    if (gUnknown_03004824->isEgg[whichParty][partyIdx] == 0)
    {
        for (i = 0; i < 4; i ++)
        {
            if (whichParty == 0)
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
            StringAppend(str, gOtherText_ControlAndMiscText);
        }
    }
    else
    {
        StringCopy(str, sText_EmptyString);
        StringAppend(str, gOtherText_FourQuestions);
    }
}

#ifdef NONMATCHING
static void sub_804A41C(u8 whichParty)
{
    u8 i;
    u8 nickname[22];
    u8 string[40];
    struct Pokemon *pokemon;

    string[0] = 0xFC;
    string[1] = 0x06;
    string[2] = 0x04;
    string[3] = 0xFC;
    string[4] = 0x11;
    string[5] = 0x00;

    for (i = 0; i < gUnknown_03004824->partyCounts[whichParty]; i ++)
    {
        pokemon = whichParty == 0 ? &gPlayerParty[i] : &gEnemyParty[i];
        GetMonData(pokemon, MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(string + 6, nickname);
        GetMonGender(pokemon);
        string[5] = (50 - Text_GetStringWidthFromWindowTemplate(&gWindowTemplate_81E7294, string + 6)) / 2;
        Text_InitWindowAndPrintText(&gUnknown_03004824->window, string, gUnknown_03004824->unk_007a + 22 * 6 * whichParty + 22 * i, sTradeMonSpriteCoords[i + 6 * whichParty][0], sTradeMonSpriteCoords[i + 6 * whichParty][1]);
    }
}
#else
NAKED
static void sub_804A41C(u8 whichParty)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                    "\tmov r7, r10\n"
                    "\tmov r6, r9\n"
                    "\tmov r5, r8\n"
                    "\tpush {r5-r7}\n"
                    "\tsub sp, 0x44\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r5, r0, 24\n"
                    "\tadd r3, sp, 0x1C\n"
                    "\tmovs r2, 0\n"
                    "\tmovs r1, 0xFC\n"
                    "\tstrb r1, [r3]\n"
                    "\tmovs r0, 0x6\n"
                    "\tstrb r0, [r3, 0x1]\n"
                    "\tmovs r0, 0x4\n"
                    "\tstrb r0, [r3, 0x2]\n"
                    "\tstrb r1, [r3, 0x3]\n"
                    "\tmovs r0, 0x11\n"
                    "\tstrb r0, [r3, 0x4]\n"
                    "\tstrb r2, [r3, 0x5]\n"
                    "\tmovs r6, 0\n"
                    "\tldr r1, _0804A470 @ =gUnknown_03004824\n"
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
                    "\tmovs r0, 0x64\n"
                    "\tadds r4, r6, 0\n"
                    "\tmuls r4, r0\n"
                    "\tldr r0, _0804A478 @ =gPlayerParty\n"
                    "\tb _0804A484\n"
                    "\t.align 2, 0\n"
                    "_0804A470: .4byte gUnknown_03004824\n"
                    "_0804A474: .4byte sTradeMonSpriteCoords\n"
                    "_0804A478: .4byte gPlayerParty\n"
                    "_0804A47C:\n"
                    "\tmovs r0, 0x64\n"
                    "\tadds r4, r6, 0\n"
                    "\tmuls r4, r0\n"
                    "\tldr r0, _0804A514 @ =gEnemyParty\n"
                    "_0804A484:\n"
                    "\tadds r4, r0\n"
                    "\tadds r0, r4, 0\n"
                    "\tmovs r1, 0x2\n"
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
                    "_0804A518: .4byte gWindowTemplate_81E7294");
}
#endif

void sub_804A51C(u8 whichParty, u8 monIdx, u8 x, u8 y, u8 width, u8 height)
{
    u8 nickname[24];
    u8 level;
    u8 gender;
    u8 lv_div_10;
    sub_804A96C(&gUnknown_03004824->unk_00c8, width, height, gTradeMenuMonBox_Tilemap, 6, 3, 0);
    if (whichParty == 0)
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
    if (gUnknown_03004824->isEgg[whichParty][monIdx] == 0)
    {
        lv_div_10 = level / 10;
        if (lv_div_10 != 0)
        {
            gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y] = lv_div_10 + 0x60;
        }
        gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y + 1] = (u8)(level % 10) + 0x70;
        if (gender == MON_MALE)
        {
            if (!NameHasGenderSymbol(nickname, MON_MALE))
            {
                gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y - 31] += 1;
            }
        }
        else if (gender == MON_FEMALE)
        {
            if (!NameHasGenderSymbol(nickname, MON_FEMALE))
            {
                gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y - 31] += 2;
            }
        }
    }
    else
    {
        gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y - 32] = gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y - 33];
        gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y - 31] = gUnknown_03004824->unk_00c8.tilemapBuffer[x + 32 * y - 36] | 0x400;
    }
#ifdef GERMAN
    gUnknown_03004824->unk_00c8.unk_10 = 1;
#endif
}

static void PrintPartyLevelsAndGenders(u8 whichParty)
{
    int i;
    for (i = 0; i < gUnknown_03004824->partyCounts[whichParty]; i++)
    {
        int loc = i + whichParty * 6;
        sub_804A51C(whichParty, i, sTradeMonLevelCoords[loc][0], sTradeMonLevelCoords[loc][1], sTradeMonBoxCoords[loc][0], sTradeMonBoxCoords[loc][1]);
    }
}

static void ShowTradePartyMonIcons(u8 whichParty)
{
    int i;
    for (i = 0; i < gUnknown_03004824->partyCounts[whichParty]; i ++)
    {
        gSprites[gUnknown_03004824->partySpriteIds[whichParty][i]].invisible = FALSE;
        gSprites[gUnknown_03004824->partySpriteIds[whichParty][i]].x = sTradeMonSpriteCoords[6 * whichParty + i][0] * 8 + 14;
        gSprites[gUnknown_03004824->partySpriteIds[whichParty][i]].y = sTradeMonSpriteCoords[6 * whichParty + i][1] * 8 - 12;
        gSprites[gUnknown_03004824->partySpriteIds[whichParty][i]].x2 = 0;
        gSprites[gUnknown_03004824->partySpriteIds[whichParty][i]].y2 = 0;
    }
}

static void sub_804A80C(void)
{
    Text_FillWindowRectDefPalette(&gUnknown_03004824->window, 0, gUnknown_0820C330[1][0], 0, gUnknown_0820C330[1][1], 19);
    sub_804A41C(1);
}

static void sub_804A840(u8 whichParty)
{
    if (whichParty == 0)
    {
        Text_FillWindowRectDefPalette(&gUnknown_03004824->window, 0, gUnknown_0820C330[0][0], 0, gUnknown_0820C330[0][1], 19);
        sub_804A96C_alt(&gUnknown_03004824->unk_00c8, 0, 0, sTradePartyBoxTilemap, 15, 17, 0);
        PrintPartyLevelsAndGenders(0);
        sub_804A41C(0);
        ShowTradePartyMonIcons(0);
        sub_804A938(&gUnknown_03004824->unk_00c8);
    }
    else
    {
        Menu_DestroyCursor();
        Text_FillWindowRectDefPalette(&gUnknown_03004824->window, 0, gUnknown_0820C330[1][0], 0, gUnknown_0820C330[1][1], 19);
        sub_804A96C_alt(&gUnknown_03004824->unk_00c8, 15, 0, sTradePartyBoxTilemap, 15, 17, 0);
        PrintPartyLevelsAndGenders(1);
        sub_804A41C(1);
        ShowTradePartyMonIcons(1);
        sub_804A938(&gUnknown_03004824->unk_00c8);
    }
    sub_804ACD8(sActionTexts[1], BG_CHAR_ADDR(4) + 32 * gUnknown_03004824->bottomTextTileStart, 20);
    gUnknown_03004824->drawSelectedMonState[whichParty] = 0;
}

static void sub_804A938(struct UnkStructD *unkStructD)
{
    unkStructD->unk_10 = 1;
}

static void sub_804A940(struct UnkStructD *unkStructD)
{
    if (unkStructD->unk_10)
    {
        CpuCopy16(unkStructD->tilemapBuffer, unkStructD->vramAddr, sizeof(unkStructD->tilemapBuffer));
        unkStructD->unk_10 = 0;
    }
}

static void sub_804A964(struct UnkStructD *unkStructD, void *dest)
{
    unkStructD->unk_10 = 0;
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
    arg0->unk_10 = 1;
#endif
}

#if GERMAN
static void sub_804A96C_alt(struct UnkStructD *arg0, u8 destX, u8 destY, const u16 *tilemap, u8 rectWidth, u8 rectHeight, u16 sp8) {
    sub_804A96C(arg0, destX, destY, tilemap, rectWidth, rectHeight, sp8);

    arg0->unk_10 = 1;
}
#endif

static void sub_804A9F4(u8 unused)
{
    Menu_EraseScreen();
}

static void sub_804AA00(u8 unused)
{
    Menu_EraseScreen();
}

static void sub_804AA0C(u8 messageId)
{
    TextWindow_DrawStdFrame(&gUnknown_03004824->window, gTradeMessageWindowRects[messageId][0], gTradeMessageWindowRects[messageId][1], gTradeMessageWindowRects[messageId][2], gTradeMessageWindowRects[messageId][3]);
    Text_InitWindowAndPrintText(&gUnknown_03004824->window, sMessages[messageId], 0x180 + gUnknown_03004824->unk_007a, gTradeMessageWindowRects[messageId][0] + 1, gTradeMessageWindowRects[messageId][1] + 1);
}

static void sub_804AA88(void)
{
    int i;
    for (i = 0; i < 4; i ++)
    {
        gUnknown_03004824->queuedActions[i].active = 0;
        gUnknown_03004824->queuedActions[i].delay = 0;
        gUnknown_03004824->queuedActions[i].actionId |= 0xff;
    }
}

static void QueueAction(u16 delay, u8 actionId)
{
    int i;
    for (i = 0; i < 4; i ++)
    {
        if (gUnknown_03004824->queuedActions[i].active == 0)
        {
            gUnknown_03004824->queuedActions[i].delay = delay;
            gUnknown_03004824->queuedActions[i].actionId = actionId;
            gUnknown_03004824->queuedActions[i].active = 1;
            break;
        }
    }
}

static void sub_804AB30(void)
{
    int i;
    for (i = 0; i < 4; i ++)
    {
        if (gUnknown_03004824->queuedActions[i].active)
        {
            if (gUnknown_03004824->queuedActions[i].delay != 0)
            {
                gUnknown_03004824->queuedActions[i].delay --;
                continue;
            }
            switch (gUnknown_03004824->queuedActions[i].actionId)
            {
                case 0:
                    Trade_SendData(gUnknown_03004824);
                    break;
                case 1:
                    sub_804AA0C(0);
                    break;
                case 2:
                    sub_804AA0C(2);
                    break;
                case 3:
                    sub_804AA0C(3);
                    break;
                case 4:
                    sub_804AA0C(3);
                    break;
                case 5:
                    sub_804AA0C(3);
                    break;
            }
            gUnknown_03004824->queuedActions[i].active = 0;
        }
    }
}

static bool8 sub_804ABF8(void)
{
    switch (gUnknown_03004824->timer)
    {
        case  8:
            gUnknown_03004824->bottomTextTileStart = LoadSpriteSheet(&gUnknown_0820C07C[gUnknown_03004824->timer]);
            gUnknown_03004824->timer ++;
            return FALSE;
        case 13:
            LoadSpritePalette(&sSpritePalette_MenuText);
            gUnknown_03004824->timer ++;
            return FALSE;
        case 14:
            LoadSpritePalette(&sCursor_SpritePalette);
            gUnknown_03004824->timer ++;
            return FALSE;
        case 15:
            LoadSpriteSheet(&sCursor_SpriteSheet);
            gUnknown_03004824->timer ++;
            // fallthrough
        case 16:
            gUnknown_03004824->timer = 0;
            return TRUE;
        default:
            LoadSpriteSheet(&gUnknown_0820C07C[gUnknown_03004824->timer]);
            gUnknown_03004824->timer ++;
            return FALSE;
    }
}

static void sub_804ACD8(const u8 *src, u8 *dest, u8 a2)
{
    sub_804AFB8(&gWindowTemplate_81E725C, dest, src, a2);
}

// replacing [who] with [0]/[1] depending on case compiles better code
static void ComputePartyTradeableFlags(u8 whichParty)
{
    struct Pokemon *pokemon;
    int i;
    
    switch (whichParty)
    {
        case 0:
            for (i = 0; i < gUnknown_03004824->partyCounts[whichParty]; i++)
            {
                pokemon = &gPlayerParty[i];
                if (GetMonData(pokemon, MON_DATA_IS_EGG) == TRUE)
                {
                    gUnknown_03004824->isLiveMon[whichParty][i] = 0;
                    gUnknown_03004824->isEgg[whichParty][i] = 1;
                }
                else if (GetMonData(pokemon, MON_DATA_HP) == 0)
                {
                    gUnknown_03004824->isLiveMon[whichParty][i] = 0;
                    gUnknown_03004824->isEgg[whichParty][i] = 0;
                }
                else
                {
                    gUnknown_03004824->isLiveMon[whichParty][i] = 1;
                    gUnknown_03004824->isEgg[whichParty][i] = 0;
                }
            }
            break;
        case 1:
            for (i = 0; i < gUnknown_03004824->partyCounts[whichParty]; i++)
            {
                pokemon = &gEnemyParty[i];
                if (GetMonData(pokemon, MON_DATA_IS_EGG) == TRUE)
                {
                    gUnknown_03004824->isLiveMon[whichParty][i] = 0;
                    gUnknown_03004824->isEgg[whichParty][i] = 1;
                }
                else if (GetMonData(pokemon, MON_DATA_HP) == 0)
                {
                    gUnknown_03004824->isLiveMon[whichParty][i] = 0;
                    gUnknown_03004824->isEgg[whichParty][i] = 0;
                }
                else
                {
                    gUnknown_03004824->isLiveMon[whichParty][i] = 1;
                    gUnknown_03004824->isEgg[whichParty][i] = 0;
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
        case 0:
            for (i = 0; i < gUnknown_03004824->partyCounts[0]; i ++)
            {
                curHp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
                maxHp = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
                gUnknown_03004824->hpBarLevels[0][i] = GetHPBarLevel(curHp, maxHp);
            }
            break;
        case 1:
            for (i = 0; i < gUnknown_03004824->partyCounts[1]; i ++)
            {
                curHp = GetMonData(&gEnemyParty[i], MON_DATA_HP);
                maxHp = GetMonData(&gEnemyParty[i], MON_DATA_MAX_HP);
                gUnknown_03004824->hpBarLevels[1][i] = GetHPBarLevel(curHp, maxHp);
            }
            break;
    }
}

static void SetTradePartyHPBarSprites(void)
{
    int i, j;
    for (i = 0; i < 2; i ++)
    {
        for (j = 0; j < gUnknown_03004824->partyCounts[i]; j ++)
        {
            SetPartyHPBarSprite(&gSprites[gUnknown_03004824->partySpriteIds[i][j]], 4 - gUnknown_03004824->hpBarLevels[i][j]);
        }
    }
}

static void sub_804AF84(void)
{
    int i;
    for (i = 0; i < 11; i ++)
        if (gSaveBlock1.giftRibbons[i] == 0 && gUnknown_03004824->giftRibbons[i] != 0)
            gSaveBlock1.giftRibbons[i] = gUnknown_03004824->giftRibbons[i];
}

static void sub_804AFB8(const struct WindowTemplate *windowConfig, u8 *dest, const u8 *src, u8 size)
{
    u8 i;
    u8 *tileBuffer;
    size = (size + 3) / 4;
    tileBuffer = gTileBuffer;
    CpuFill16(0, tileBuffer, size * 0x80);
    CpuFill16(0, tileBuffer + windowConfig->width * 0x20, size * 0x80);
    Text_InitWindow8004E3C(windowConfig, tileBuffer, src);
    for (i = 0; i < size; i ++)
    {
        CpuCopy16(&tileBuffer[32 * (i * 4)], &dest[32 * (i * 8)], 0x80);
        CpuCopy16(&tileBuffer[32 * (i * 4 + windowConfig->width)], &dest[32 * (i * 8 + 4)], 0x80);
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
        LoadPalette(&sLinkMonShadow_Pal[sprite->data[0]], 16 * (sprite->oam.paletteNum + 16) + 4, 2);
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

static void sub_804B128(void)
{
    struct BgAffineDstData affine;
    DoBgAffineSet(&affine, gUnknown_03004828->texX << 8, gUnknown_03004828->texY << 8, gUnknown_03004828->scrX, gUnknown_03004828->scrY, gUnknown_03004828->sXY, gUnknown_03004828->sXY, gUnknown_03004828->alpha);
    REG_BG2PA = affine.pa;
    REG_BG2PB = affine.pb;
    REG_BG2PC = affine.pc;
    REG_BG2PD = affine.pd;
    REG_BG2X = affine.dx;
    REG_BG2Y = affine.dy;
}

static void sub_804B1BC(void)
{
    u16 dispcnt;

    REG_BG1VOFS = gUnknown_03004828->bg1vofs;
    REG_BG1HOFS = gUnknown_03004828->bg1hofs;

    /*
        A u16 cast allows for REG_DISPCNT storage to be swapped.
        This is required for the function to match.

        You can see this less obfuscated in FireRed and Emerald,
        since they use gflib's GPU manager for this instead.
    */
    dispcnt = (*(u16 *)REG_ADDR_DISPCNT);
    if ((dispcnt & 7) == DISPCNT_MODE_0)
    {
        REG_BG2VOFS = gUnknown_03004828->bg2vofs;
        REG_BG2HOFS = gUnknown_03004828->bg2hofs;
    }
    else
    {
        sub_804B128();
    }
}

static void VBlankCB_TradeAnim(void)
{
    sub_804B1BC();
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void ClearLinkTimeoutTimer(void)
{
    gUnknown_03004828->linkTimeoutTimer = 0;
    gUnknown_03004828->linkTimeoutZero1 = 0;
    gUnknown_03004828->linkTimeoutZero2 = 0;
}

void sub_804B24C(void)
{
    if (gUnknown_03004828->linkTimeoutZero1 == gUnknown_03004828->linkTimeoutZero2)
    {
        gUnknown_03004828->linkTimeoutTimer ++;
    }
    else
    {
        gUnknown_03004828->linkTimeoutTimer = 0;
    }
    if (gUnknown_03004828->linkTimeoutTimer > 0xb4)
    {
        gUnknown_03004828->linkTimeoutTimer = 0;
        gUnknown_03004828->linkTimeoutZero2 = 0;
        gUnknown_03004828->linkTimeoutZero1 = 0;
    }
    gUnknown_03004828->linkTimeoutZero2 = gUnknown_03004828->linkTimeoutZero1;
}

static u8 TradeGetMultiplayerId(void)
{
    if (gReceivedRemoteLinkPlayers)
        return GetMultiplayerId();
    return 0;
}

static void sub_804B2D0(u8 whichParty, u8 state)
{
    u8 pos;
    struct Pokemon *pokemon;
    u16 species;
    u32 personality;

    pos = 0;
    pokemon = NULL;
    if (whichParty == 0)
    {
        pokemon = &gPlayerParty[gSelectedTradeMonPositions[0]];
        pos = 1;
    }
    if (whichParty == 1)
    {
        pokemon = &gEnemyParty[gSelectedTradeMonPositions[1] % 6];
        pos = 3;
    }
    switch (state)
    {
        case 0:
            species = GetMonData(pokemon, MON_DATA_SPECIES_OR_EGG);
            personality = GetMonData(pokemon, MON_DATA_PERSONALITY);
            HandleLoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].coords, gMonFrontPicCoords[species].y_offset, gSharedMem, gMonSpriteGfx_Sprite_ptr[whichParty * 2 + 1], species, personality);
            LoadCompressedObjectPalette(GetMonSpritePalStruct(pokemon));
            gUnknown_03004828->monSpecies[whichParty] = species;
            break;
        case 1:
            SetMultiuseSpriteTemplateToPokemon(GetMonSpritePalStruct(pokemon)->tag, pos);
            gUnknown_03004828->monSpriteIds[whichParty] = CreateSprite(&gMultiuseSpriteTemplate, 0x78, 0x3c, 0x6);
            gSprites[gUnknown_03004828->monSpriteIds[whichParty]].invisible = TRUE;
            gSprites[gUnknown_03004828->monSpriteIds[whichParty]].callback = SpriteCallbackDummy;
            break;
    }
}

// non-shifting nonmatch
// out of order statements + register swaps?
// r4 is loaded with 0 way before it's used (and it should be r2)
#ifdef NONMATCHING
void sub_804B41C(void)
// Link trade init
{
    switch (gMain.state)
    {
        case 0:
            REG_DISPCNT = 0;
            ResetTasks();
            CloseLink();
            gUnknown_03004828 = &ewram_2010000.unk_0f000;
            ResetSpriteData();
            FreeAllSpritePalettes();
            SetVBlankCallback(VBlankCB_TradeAnim);
            ClearLinkTimeoutTimer();
            Text_LoadWindowTemplate(&gWindowTemplate_81E6F84);
            Text_InitWindowWithTemplate(&gUnknown_03004828->window, &gWindowTemplate_81E6F84);
            gUnknown_03004828->textWindowBaseTileNum = TextWindow_SetBaseTileNum(2);
            //
            // start of nonmatching here?
            TextWindow_LoadStdFrameGraphics(&gUnknown_03004828->window);
            Menu_EraseScreen();
            gLinkType = 0x1144;
            gMain.state++;
            LZDecompressVram(gBattleTextboxTiles, (void *)VRAM);
            CpuCopy16(gBattleTextboxTilemap, gSharedMem, 0x1000);
            DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
            LoadCompressedPalette(gBattleTextboxPalette, 0, 32);
            gUnknown_03004828->unk_00b6 = 0; // nonmatching code writes to this address with r4, as opposed to r2 in matching code. r4 is also loaded with zero way before
            gUnknown_03004828->state = 0;
            gUnknown_03004828->isLinkTrade = TRUE;
            gUnknown_03004828->texX = 0x40;
            gUnknown_03004828->texY = 0x40;
            gUnknown_03004828->unk_0108 = 0;
            gUnknown_03004828->unk_010a = 0;
            gUnknown_03004828->scrX = 0x78;
            gUnknown_03004828->scrY = 0x50;
            gUnknown_03004828->sXY = 0x100;
            gUnknown_03004828->alpha = 0;
            break;
        case 1:
            OpenLink();
            gMain.state ++;
            gUnknown_03004828->timer = 0;
            break;
        case 2:
            if (++ gUnknown_03004828->timer > 60)
            {
                gUnknown_03004828->timer = 0;
                gMain.state ++;
            }
            break;
        case 3:
            if (IsLinkMaster())
            {
                if (GetLinkPlayerCount_2() >= GetSavedPlayerCount() && ++ gUnknown_03004828->timer > 30)
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
            sub_804B24C();
            if (gReceivedRemoteLinkPlayers == TRUE && IsLinkPlayerDataExchangeComplete() == TRUE) gMain.state ++;
            break;
        case 5:
            gUnknown_03004828->playerFinishStatus = 0;
            gUnknown_03004828->partnerFinishStatus = 0;
            gUnknown_03004828->scheduleLinkTransfer = 0;
            sub_804B2D0(0, 0);
            gMain.state ++;
            break;
        case 6:
            sub_804B2D0(0, 1);
            gMain.state ++;
            break;
        case 7:
            sub_804B2D0(1, 0);
            gMain.state ++;
            break;
        case 8:
            sub_804B2D0(1, 1);
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
            sub_804BBE8(5);
            sub_804BBE8(0);
            BufferTradeSceneStrings();
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gMain.state ++;
            break;
        case 12:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(sub_804DB84);
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}
#else
NAKED void sub_804B41C(void)
{
    asm_unified("\tpush {r4-r6,lr}\n"
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
                    "\tldr r6, _0804B570 @ =gUnknown_03004828\n"
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
                    "_0804B570: .4byte gUnknown_03004828\n"
                    "_0804B574: .4byte gSharedMem + 0x1F000\n"
                    "_0804B578: .4byte VBlankCB_TradeAnim\n"
                    "_0804B57C: .4byte gWindowTemplate_81E6F84\n"
                    "_0804B580: .4byte gLinkType\n"
                    "_0804B584: .4byte 0x00001144\n"
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
                    "\tldr r0, _0804B5D0 @ =gUnknown_03004828\n"
                    "\tldr r0, [r0]\n"
                    "\tadds r0, 0xC0\n"
                    "\tstr r2, [r0]\n"
                    "\tb _0804B76E_break\n"
                    "\t.align 2, 0\n"
                    "_0804B5C8: .4byte gMain\n"
                    "_0804B5CC: .4byte 0x0000043c\n"
                    "_0804B5D0: .4byte gUnknown_03004828\n"
                    "_0804B5D4_case02:\n"
                    "\tldr r0, _0804B5F4 @ =gUnknown_03004828\n"
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
                    "_0804B5F4: .4byte gUnknown_03004828\n"
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
                    "\tldr r0, _0804B63C @ =gUnknown_03004828\n"
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
                    "_0804B63C: .4byte gUnknown_03004828\n"
                    "_0804B640: .4byte gMain\n"
                    "_0804B644: .4byte 0x0000043c\n"
                    "_0804B648_case04:\n"
                    "\tbl sub_804B24C\n"
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
                    "\tldr r2, _0804B69C @ =gUnknown_03004828\n"
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
                    "\tbl sub_804B2D0\n"
                    "\tldr r1, _0804B6A0 @ =gMain\n"
                    "\tldr r0, _0804B6A4 @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B69C: .4byte gUnknown_03004828\n"
                    "_0804B6A0: .4byte gMain\n"
                    "_0804B6A4: .4byte 0x0000043c\n"
                    "_0804B6A8_case06:\n"
                    "\tmovs r0, 0\n"
                    "\tmovs r1, 0x1\n"
                    "\tbl sub_804B2D0\n"
                    "\tb _0804B746\n"
                    "_0804B6B2_case07:\n"
                    "\tmovs r0, 0x1\n"
                    "\tmovs r1, 0\n"
                    "\tbl sub_804B2D0\n"
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
                    "\tbl sub_804B2D0\n"
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
                    "_0804B710: .4byte 0x00000502\n"
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
                    "\tbl sub_804BBE8\n"
                    "\tmovs r0, 0\n"
                    "\tbl sub_804BBE8\n"
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
                    "\tldr r0, _0804B78C @ =sub_804DB84\n"
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
                    "_0804B78C: .4byte sub_804DB84");
}
#endif

// In-game trade init
void CB2_InitInGameTrade(void)
{
    u8 otName[11];
    switch (gMain.state)
    {
        case 0:
            gSelectedTradeMonPositions[0] = gSpecialVar_0x8005;
            gSelectedTradeMonPositions[1] = 6;
            StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
            GetMonData(&gEnemyParty[0], MON_DATA_OT_NAME, otName);
            StringCopy(gLinkPlayers[1].name, otName);
            REG_DISPCNT = 0;
            ResetTasks();
            gUnknown_03004828 = &ewram_2010000.unk_0f000;
            ResetSpriteData();
            FreeAllSpritePalettes();
            SetVBlankCallback(VBlankCB_TradeAnim);
            ClearLinkTimeoutTimer();
            Text_LoadWindowTemplate(&gWindowTemplate_81E717C);
            Text_InitWindowWithTemplate(&gUnknown_03004828->window, &gWindowTemplate_81E717C);
            gUnknown_03004828->textWindowBaseTileNum = TextWindow_SetBaseTileNum(2);
            TextWindow_LoadStdFrameGraphics(&gUnknown_03004828->window);
            Menu_EraseScreen();
            gLinkType = 0x1144;
            gUnknown_03004828->isLinkTrade = FALSE;
            gUnknown_03004828->unk_00b6 = 0;
            gUnknown_03004828->state = 0;
            gUnknown_03004828->texX = 0x40;
            gUnknown_03004828->texY = 0x40;
            gUnknown_03004828->unk_0108 = 0;
            gUnknown_03004828->unk_010a = 0;
            gUnknown_03004828->scrX = 0x78;
            gUnknown_03004828->scrY = 0x50;
            gUnknown_03004828->sXY = 0x100;
            gUnknown_03004828->alpha = 0;
            gUnknown_03004828->timer = 0;
            gMain.state = 5;
            break;
        case 5:
            sub_804B2D0(0, 0);
            gMain.state ++;
            break;
        case 6:
            sub_804B2D0(0, 1);
            gMain.state ++;
            break;
        case 7:
            sub_804B2D0(1, 0);
            gMain.state ++;
            break;
        case 8:
            sub_804B2D0(1, 1);
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
            sub_804BBE8(5);
            sub_804BBE8(0);
            BufferTradeSceneStrings();
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gMain.state ++;
            break;
        case 12:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(sub_804BBCC);
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_804BA18(u8 partyIdx)
{
    struct Pokemon *pokemon = &gPlayerParty[partyIdx];
    if (!GetMonData(pokemon, MON_DATA_IS_EGG))
    {
        u16 species = SpeciesToNationalPokedexNum(GetMonData(pokemon, MON_DATA_SPECIES, NULL));
        GetSetPokedexFlag(species, 2);
        GetSetPokedexFlag(species, 3);
    }
}

static void sub_804BA64(void)
{
    u8 mpId = GetMultiplayerId();
    if (gLinkPlayers[mpId ^ 1].lp_field_2 == 0x8000)
        EnableNationalPokedex();
}

static void sub_804BA94(u8 playerPartyIdx, u8 partnerPartyIdx)
{
    u8 friendship;
    struct Pokemon *playerMon = &gPlayerParty[playerPartyIdx];
    u16 playerMail = GetMonData(playerMon, MON_DATA_MAIL);

    struct Pokemon *partnerMon = &gEnemyParty[partnerPartyIdx];
    u16 partnerMail = GetMonData(partnerMon, MON_DATA_MAIL);

    if (playerMail != 0xff)
        ClearMailStruct(&gSaveBlock1.mail[playerMail]);

    // This is where the actual trade happens!!
    gUnknown_03004828->tempMon = *playerMon;
    *playerMon = *partnerMon;
    *partnerMon = gUnknown_03004828->tempMon;

    friendship = 70;
    if (!GetMonData(playerMon, MON_DATA_IS_EGG))
        SetMonData(playerMon, MON_DATA_FRIENDSHIP, &friendship);

    if (partnerMail != 0xff)
        GiveMailToMon2(playerMon, &gTradeMail[partnerMail]);

    sub_804BA18(playerPartyIdx);
    if (gReceivedRemoteLinkPlayers)
        sub_804BA64();
}

static void sub_804BB78(void)
{
    switch (gUnknown_03004828->scheduleLinkTransfer)
    {
        case 1:
            if (IsLinkTaskFinished())
            {
                Trade_SendData(gUnknown_03004828);
                gUnknown_03004828->scheduleLinkTransfer ++;
            }
            // fallthrough
        case 2:
            gUnknown_03004828->scheduleLinkTransfer = 0;
            break;
    }
}

static void sub_804BBCC(void)
{
    sub_804C29C();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_804BBE8(u8 state)
{
    int i;
    u16 *buffer;
    switch (state)
    {
        case 0:
            LoadPalette(gTradeGba2_Pal, 0x10, 0xa0);
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(1), 0x1300, 0x1000);
            DmaCopy16Defvars(3, gTradePlatform_Tilemap, BG_SCREEN_ADDR(18), 0x1000);
            gUnknown_03004828->bg2vofs = 0;
            gUnknown_03004828->bg2hofs = 0xb4;
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(18) | BGCNT_TXT512x256;
            break;
        case 1:
            gUnknown_03004828->bg1hofs = 0;
            gUnknown_03004828->bg1vofs = 0x15c;
            REG_BG1VOFS = 0x15c;
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(5) | BGCNT_TXT256x512;

            DmaCopy16Defvars(3, sGbaMapCable, BG_SCREEN_ADDR(5), 0x1000);
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(0), 0x1300, 0x1000);
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            break;
        case 2:
            gUnknown_03004828->bg1vofs = 0;
            gUnknown_03004828->bg1hofs = 0;
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            DmaCopy16Defvars(3, sCableCloseup_Map, BG_SCREEN_ADDR(5), 0x800);
            break;
        case 3:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            gUnknown_03004828->texX = 0x40;
            gUnknown_03004828->texY = 0x40;
            gUnknown_03004828->scrX = 0x78;
            gUnknown_03004828->scrY = -0x46;
            gUnknown_03004828->alpha = 0;
            DmaCopyLarge16(3, sPokeballSymbol_Gfx, BG_CHAR_ADDR(1), 0x1a00, 0x1000);
            DmaCopy16Defvars(3, sPokeballSymbol_Map, BG_SCREEN_ADDR(18), 0x100);
            break;
        case 4:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(18);
            gUnknown_03004828->texX = 0x40;
            gUnknown_03004828->texY = 0x5c;
            gUnknown_03004828->sXY = 0x20;
            gUnknown_03004828->gbaScale = 0x400;
            gUnknown_03004828->alpha = 0;
            DmaCopyLarge16(3, sGbaAffine_Gfx, BG_CHAR_ADDR(1), 0x2040, 0x1000);
            DmaCopy16Defvars(3, sGbaAffineMapCable, BG_SCREEN_ADDR(18), 0x100);
            break;
        case 5:
            gUnknown_03004828->bg1vofs = 0;
            gUnknown_03004828->bg1hofs = 0;
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(5);
            LZDecompressVram(gBattleTextboxTiles, BG_CHAR_ADDR(0));
            CpuCopy16(gBattleTextboxTilemap, buffer = (u16 *)gSharedMem, 0x1000);
            LoadCompressedPalette(gBattleTextboxPalette, 0x70, 0x20);
            FillPalette(0, 0, 2);
            for (i = 0; i < 0x280; i ++)
                buffer[i] |= 0x7000;
            DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
            Menu_EraseWindowRect(2, 15, 27, 18);
            break;
        case 6:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(18);
            gUnknown_03004828->texX = 0x40;
            gUnknown_03004828->texY = 0x5c;
            gUnknown_03004828->sXY = 0x100;
            gUnknown_03004828->gbaScale = 0x80;
            gUnknown_03004828->scrX = 0x78;
            gUnknown_03004828->scrY = 0x50;
            gUnknown_03004828->alpha = 0;
            DmaCopyLarge16(3, sGbaAffine_Gfx, BG_CHAR_ADDR(1), 0x2040, 0x1000);
            DmaCopy16Defvars(3, sGbaAffineMapCable, BG_SCREEN_ADDR(18), 0x100);
            break;
        case 7:
            gUnknown_03004828->bg2vofs = 0;
            gUnknown_03004828->bg2hofs = 0;
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(18) | BGCNT_TXT512x256;
            LoadPalette(gTradeGba2_Pal, 0x10, 0xa0);
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(1), 0x1300, 0x1000);
            DmaCopy16Defvars(3, gTradePlatform_Tilemap, BG_SCREEN_ADDR(18), 0x1000);
            break;
    }
}

static void sub_804C0F8(u8 a0)
{
    if (a0 == 0)
    {
        if (gUnknown_03004828->bg1vofs < 0x10a)
        {
            gUnknown_03004828->scrY ++;
            gUnknown_03004828->alpha += 64;
        }
    }
    else
    {
        if (gUnknown_03004828->scrY > -0x40)
        {
            gUnknown_03004828->scrY --;
            gUnknown_03004828->alpha += 64;
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
    u8 name[20];
    const struct InGameTrade *ingameTrade;
    if (gUnknown_03004828->isLinkTrade)
    {
        mpId = GetMultiplayerId();
        StringCopy(gStringVar1, gLinkPlayers[mpId ^ 1].name);
        GetMonData(&gEnemyParty[gSelectedTradeMonPositions[1] % 6], MON_DATA_NICKNAME, name);
        StringCopy_Nickname(gStringVar3, name);
        GetMonData(&gPlayerParty[gSelectedTradeMonPositions[0]], MON_DATA_NICKNAME, name);
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

static bool8 sub_804C29C(void)
{
    u16 evoTarget;

    switch (gUnknown_03004828->state)
    {
        case 0:
            gSprites[gUnknown_03004828->monSpriteIds[0]].invisible = FALSE;
            gSprites[gUnknown_03004828->monSpriteIds[0]].x2 = -0xb4;
            gSprites[gUnknown_03004828->monSpriteIds[0]].y2 = gMonFrontPicCoords[gUnknown_03004828->monSpecies[0]].y_offset;
            gUnknown_03004828->state ++;
            gUnknown_03004828->cachedMapMusic = GetCurrentMapMusic();
            PlayBGM(MUS_EVOLUTION);
            break;
        case 1:
            if (gUnknown_03004828->bg2hofs > 0)
            {
                gSprites[gUnknown_03004828->monSpriteIds[0]].x2 += 3;
                gUnknown_03004828->bg2hofs -= 3;
            }
            else
            {
                gSprites[gUnknown_03004828->monSpriteIds[0]].x2 = 0;
                gUnknown_03004828->bg2hofs = 0;
                gUnknown_03004828->state = 10;
            }
            break;

        case 10:
            StringExpandPlaceholders(gStringVar4, gText_XWillBeSentToY);
            Text_InitWindowAndPrintText(&gUnknown_03004828->window, gStringVar4, gUnknown_03004828->textWindowBaseTileNum, 2, 15);
            gUnknown_03004828->state = 11;
            gUnknown_03004828->timer = 0;
            break;
        case 11:
            if (++gUnknown_03004828->timer == 80)
            {
                gUnknown_03004828->releasePokeballSpriteId = CreateTradePokeballSprite(gUnknown_03004828->monSpriteIds[0], gSprites[gUnknown_03004828->monSpriteIds[0]].oam.paletteNum, 0x78, 0x20, 0x2, 0x1, 0x14, 0xfffff);
                gUnknown_03004828->state ++;
                Text_EraseWindowRect(&gUnknown_03004828->window, 0, 0, 29, 19);
                StringExpandPlaceholders(gStringVar4, gText_ByeByeVar1);
                Text_InitWindowAndPrintText(&gUnknown_03004828->window, gStringVar4, gUnknown_03004828->textWindowBaseTileNum, 2, 15);
            }
            break;
        case 12:
            if (gSprites[gUnknown_03004828->releasePokeballSpriteId].callback == SpriteCallbackDummy && Text_UpdateWindow(&gUnknown_03004828->window) == TRUE)
            {
                gUnknown_03004828->bouncingPokeballSpriteId = CreateSprite(&sSpriteTemplate_Pokeball, 0x78, 0x20, 0);
                gSprites[gUnknown_03004828->bouncingPokeballSpriteId].callback = SpriteCB_BouncingPokeballDepart;
                DestroySprite(&gSprites[gUnknown_03004828->releasePokeballSpriteId]);
                gUnknown_03004828->state ++;
            }
            break;
        case 13:
            // The game waits here for the sprite to finish its animation sequence.
            break;
        case 14:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_03004828->state = 20;
            break;

        case 20:
            if (!gPaletteFade.active)
            {
                sub_804BBE8(4);
                gUnknown_03004828->state ++;
            }
            break;
        case 21:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 16, 0, RGB(0, 0, 0));
            gUnknown_03004828->state ++;
            break;
        case 22:
            if (!gPaletteFade.active)
            {
                gUnknown_03004828->state = 23;
            }
            break;
        case 23:
            if (gUnknown_03004828->gbaScale > 0x100)
            {
                gUnknown_03004828->gbaScale -= 0x34;
            }
            else
            {
                sub_804BBE8(1);
                gUnknown_03004828->gbaScale = 0x80;
                gUnknown_03004828->state ++;
                gUnknown_03004828->timer = 0;
            }
            gUnknown_03004828->sXY = 0x8000 / gUnknown_03004828->gbaScale;
            break;
        case 24:
            if (++ gUnknown_03004828->timer > 20)
            {
                sub_804BBE8(3);
                sub_804B128();
                gUnknown_03004828->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_GbaScreenFlash_Long, 0x78, 0x50, 0);
                gUnknown_03004828->state ++;
            }
            break;
        case 25:
            if (gSprites[gUnknown_03004828->connectionSpriteId2].animEnded)
            {
                DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId2]);
                REG_BLDCNT = 0x640;
                REG_BLDALPHA = 0x40C;
                gUnknown_03004828->state ++;
            }
            break;
        case 26:
            if (-- gUnknown_03004828->bg1vofs == 0x13C)
            {
                gUnknown_03004828->state ++;
            }
            if (gUnknown_03004828->bg1vofs == 0x148)
            {
                gUnknown_03004828->cableEndSpriteId = CreateSprite(&sSpriteTemplate_CableEnd, 0x80, 0x41, 0);
            }
            break;
        case 27:
            gUnknown_03004828->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonGlow, 0x80, 0x50, 3);
            gUnknown_03004828->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x80, 0x50, 0);
            StartSpriteAnim(&gSprites[gUnknown_03004828->connectionSpriteId2], 1);
            gUnknown_03004828->state ++;
            break;
        case 28:
            if ((gUnknown_03004828->bg1vofs -= 2) == 0xA6)
            {
                gUnknown_03004828->state = 200;
            }
            sub_804C0F8(0);
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            break;
        case 200:
            gSprites[gUnknown_03004828->connectionSpriteId1].y -= 2;
            gSprites[gUnknown_03004828->connectionSpriteId2].y -= 2;
            sub_804C0F8(0);
            if (gSprites[gUnknown_03004828->connectionSpriteId1].y < -8)
            {
                gUnknown_03004828->state = 29;
            }
            break;
        case 29:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 0, 16, RGB(0, 0, 0));
            gUnknown_03004828->state = 30;
            break;
        case 30:
            if (!gPaletteFade.active)
            {
                DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId1]);
                DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId2]);
                sub_804BBE8(2);
                gUnknown_03004828->state ++;
            }
            break;
        case 31:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 16, 0, RGB(0, 0, 0));
            gUnknown_03004828->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x6f, 0xaa, 0);
            gUnknown_03004828->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x81, -0xa, 0);
            gUnknown_03004828->state ++;
            break;
        case 32:
            if (!gPaletteFade.active)
            {
                PlaySE(SE_WARP_OUT);
                gUnknown_03004828->state ++;
            }
            gSprites[gUnknown_03004828->connectionSpriteId1].y2 -= 3;
            gSprites[gUnknown_03004828->connectionSpriteId2].y2 += 3;
            break;
        case 33:
            gSprites[gUnknown_03004828->connectionSpriteId1].y2 -= 3;
            gSprites[gUnknown_03004828->connectionSpriteId2].y2 += 3;
            if (gSprites[gUnknown_03004828->connectionSpriteId1].y2 <= -0x5a)
            {
                gSprites[gUnknown_03004828->connectionSpriteId1].data[1] = 1;
                gSprites[gUnknown_03004828->connectionSpriteId2].data[1] = 1;
                gUnknown_03004828->state ++;
            }
            break;
        case 34:
            BlendPalettes(0x1, 16, FADE_COLOR_WHITE);
            gUnknown_03004828->state ++;
            break;
        case 35:
            BlendPalettes(0x1,  0, FADE_COLOR_WHITE);
            gUnknown_03004828->state ++;
            break;
        case 36:
            BlendPalettes(0x1, 16, FADE_COLOR_WHITE);
            gUnknown_03004828->state ++;
            break;
        case 37:
            if (!IsMonSpriteNotFlipped(gUnknown_03004828->monSpecies[0]))
            {
                gSprites[gUnknown_03004828->monSpriteIds[0]].affineAnims = sAffineAnims_CrossingMonPics;
                gSprites[gUnknown_03004828->monSpriteIds[0]].oam.affineMode = 3;
                CalcCenterToCornerVec(&gSprites[gUnknown_03004828->monSpriteIds[0]], 0, 3, 3);
                StartSpriteAffineAnim(&gSprites[gUnknown_03004828->monSpriteIds[0]], 0);
            }
            else
            {
                StartSpriteAffineAnim(&gSprites[gUnknown_03004828->monSpriteIds[0]], 0);
            }
            StartSpriteAffineAnim(&gSprites[gUnknown_03004828->monSpriteIds[1]], 0);
            gSprites[gUnknown_03004828->monSpriteIds[0]].x = 0x3c;
            gSprites[gUnknown_03004828->monSpriteIds[1]].x = 0xb4;
            gSprites[gUnknown_03004828->monSpriteIds[0]].y = 0xc0;
            gSprites[gUnknown_03004828->monSpriteIds[1]].y = -0x20;
            gSprites[gUnknown_03004828->monSpriteIds[0]].invisible = FALSE;
            gSprites[gUnknown_03004828->monSpriteIds[1]].invisible = FALSE;
            gUnknown_03004828->state ++;
            break;
        case 38:
            gSprites[gUnknown_03004828->monSpriteIds[0]].y2 -= 3;
            gSprites[gUnknown_03004828->monSpriteIds[1]].y2 += 3;
            if (-0xa0 > gSprites[gUnknown_03004828->monSpriteIds[0]].y2 && gSprites[gUnknown_03004828->monSpriteIds[0]].y2 >= -0xa3)
            {
                PlaySE(SE_WARP_IN);
            }
            if (gSprites[gUnknown_03004828->monSpriteIds[0]].y2 < -0xde)
            {
                gSprites[gUnknown_03004828->connectionSpriteId1].data[1] = 0;
                gSprites[gUnknown_03004828->connectionSpriteId2].data[1] = 0;
                gUnknown_03004828->state ++;
                gSprites[gUnknown_03004828->monSpriteIds[0]].invisible = TRUE;
                gSprites[gUnknown_03004828->monSpriteIds[1]].invisible = TRUE;
                BlendPalettes(0x1, 0, FADE_COLOR_WHITE);
            }
            break;
        case 39:
            gSprites[gUnknown_03004828->connectionSpriteId1].y2 -= 3;
            gSprites[gUnknown_03004828->connectionSpriteId2].y2 += 3;
            if (gSprites[gUnknown_03004828->connectionSpriteId1].y2 <= -0xde)
            {
                BeginNormalPaletteFade(0xFFFFFFFF, -1, 0, 16, RGB(0, 0, 0));
                gUnknown_03004828->state ++;
                DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId1]);
                DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId2]);
            }
            break;
        case 40:
            if (!gPaletteFade.active)
            {
                gUnknown_03004828->state ++;
                sub_804BBE8(1);
                gUnknown_03004828->bg1vofs = 0xa6;
                gUnknown_03004828->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonGlow, 0x80, -0x14, 3);
                gUnknown_03004828->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x80, -0x14, 0);
                StartSpriteAnim(&gSprites[gUnknown_03004828->connectionSpriteId2], 1);
            }
            break;
        case 41:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 16, 0, RGB(0, 0, 0));
            gUnknown_03004828->state ++;
            break;
        case 42:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            sub_804C0F8(1);
            if (!gPaletteFade.active)
            {
                gUnknown_03004828->state ++;
            }
            break;
        case 43:
            sub_804C0F8(1);
            gSprites[gUnknown_03004828->connectionSpriteId1].y2 += 3;
            gSprites[gUnknown_03004828->connectionSpriteId2].y2 += 3;
            if (gSprites[gUnknown_03004828->connectionSpriteId1].y2 + gSprites[gUnknown_03004828->connectionSpriteId1].y == 64)
            {
                gUnknown_03004828->state ++;
            }
            break;
        case 44:
            sub_804C0F8(1);
            if ((gUnknown_03004828->bg1vofs += 2) > 0x13c)
            {
                gUnknown_03004828->bg1vofs = 0x13c;
                gUnknown_03004828->state ++;
            }
            break;
        case 45:
            DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId1]);
            DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId2]);
            gUnknown_03004828->state ++;
            gUnknown_03004828->timer = 0;
            break;
        case 46:
            if (++ gUnknown_03004828->timer == 10)
            {
                gUnknown_03004828->state ++;
            }
            break;
        case 47:
            if (++ gUnknown_03004828->bg1vofs > 0x15c)
            {
                gUnknown_03004828->bg1vofs = 0x15c;
                gUnknown_03004828->state ++;
            }
            if (gUnknown_03004828->bg1vofs == 0x148)
                gUnknown_03004828->cableEndSpriteId = CreateSprite(&sSpriteTemplate_CableEnd, 0x80, 0x41, 0);
            gSprites[gUnknown_03004828->cableEndSpriteId].callback = SpriteCB_CableEndReceiving;
            break;
        case 48:
            gUnknown_03004828->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_GbaScreenFlash_Long, 0x78, 0x50, 0);
            gUnknown_03004828->state = 50;
            break;

        case 50:
            if (gSprites[gUnknown_03004828->connectionSpriteId2].animEnded)
            {
                DestroySprite(&gSprites[gUnknown_03004828->connectionSpriteId2]);
                sub_804BBE8(6);
                gUnknown_03004828->state ++;
                PlaySE(SE_M_SAND_ATTACK);
            }
            break;
        case 51:
            if (gUnknown_03004828->gbaScale < 0x400)
            {
                gUnknown_03004828->gbaScale += 0x34;
            }
            else
            {
                gUnknown_03004828->gbaScale = 0x400;
                gUnknown_03004828->state ++;
            }
            gUnknown_03004828->sXY = 0x8000 / gUnknown_03004828->gbaScale;
            break;
        case 52:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_03004828->state = 60;
            break;

        case 60:
            if (!gPaletteFade.active)
            {
                sub_804BBE8(5);
                sub_804BBE8(7);
                gUnknown_03004828->state ++;
            }
            break;
        case 61:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gUnknown_03004828->state ++;
            break;
        case 62:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            if (!gPaletteFade.active)
            {
                gUnknown_03004828->state ++;
            }
            break;
        case 63:
            gUnknown_03004828->bouncingPokeballSpriteId = CreateSprite(&sSpriteTemplate_Pokeball, 0x78, -0x8, 0);
            gSprites[gUnknown_03004828->bouncingPokeballSpriteId].data[3] = 0x4a;
            gSprites[gUnknown_03004828->bouncingPokeballSpriteId].callback = SpriteCB_BouncingPokeballArrive;
            StartSpriteAnim(&gSprites[gUnknown_03004828->bouncingPokeballSpriteId], 1);
            StartSpriteAffineAnim(&gSprites[gUnknown_03004828->bouncingPokeballSpriteId], 2);
            BlendPalettes(1 << (16 + gSprites[gUnknown_03004828->bouncingPokeballSpriteId].oam.paletteNum), 16, FADE_COLOR_WHITE);
            gUnknown_03004828->state ++;
            gUnknown_03004828->timer = 0;
            break;
        case 64:
            BeginNormalPaletteFade(1 << (16 + gSprites[gUnknown_03004828->bouncingPokeballSpriteId].oam.paletteNum), 1, 16, 0, FADE_COLOR_WHITE);
            gUnknown_03004828->state ++;
            break;
        case 65:
            if (gSprites[gUnknown_03004828->bouncingPokeballSpriteId].callback == SpriteCallbackDummy)
            {
                gUnknown_03004828->state ++;
            }
            break;
        case 66:
            gSprites[gUnknown_03004828->monSpriteIds[1]].x = 0x78;
            gSprites[gUnknown_03004828->monSpriteIds[1]].y = gMonFrontPicCoords[gUnknown_03004828->monSpecies[1]].y_offset + 60;
            gSprites[gUnknown_03004828->monSpriteIds[1]].x2 = 0;
            gSprites[gUnknown_03004828->monSpriteIds[1]].y2 = 0;
            CreatePokeballSprite(gUnknown_03004828->monSpriteIds[1], gSprites[gUnknown_03004828->monSpriteIds[1]].oam.paletteNum, 0x78, 0x54, 2, 1, 0x14, 0xfffff);
            FreeSpriteOamMatrix(&gSprites[gUnknown_03004828->bouncingPokeballSpriteId]);
            DestroySprite(&gSprites[gUnknown_03004828->bouncingPokeballSpriteId]);
            gUnknown_03004828->state ++;
            break;
        case 67:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            Text_EraseWindowRect(&gUnknown_03004828->window, 0, 0, 29, 19);
            StringExpandPlaceholders(gStringVar4, gText_XSentOverY);
            Text_InitWindowAndPrintText(&gUnknown_03004828->window, gStringVar4, gUnknown_03004828->textWindowBaseTileNum, 2, 15);
            gUnknown_03004828->state ++;
            gUnknown_03004828->timer = 0;
            break;
        case 68:
            if (++ gUnknown_03004828->timer == 4)
            {
                PlayFanfare(MUS_EVOLVED);
            }
            if (gUnknown_03004828->timer == 0xf0)
            {
                gUnknown_03004828->state ++;
                Text_EraseWindowRect(&gUnknown_03004828->window, 0, 0, 29, 19);
                StringExpandPlaceholders(gStringVar4, gText_TakeGoodCareOfX);
                Text_InitWindowAndPrintText(&gUnknown_03004828->window, gStringVar4, gUnknown_03004828->textWindowBaseTileNum, 2, 15);
                gUnknown_03004828->timer = 0;
            }
            break;
        case 69: // OneHand
            if (++ gUnknown_03004828->timer == 60)
            {
                gUnknown_03004828->state ++;
            }
            break;
        case 70:
            CheckPartnersMonForRibbons();
            gUnknown_03004828->state ++;
            break;
        case 71:
            if (gUnknown_03004828->isLinkTrade)
            {
                return TRUE;
            }
            else if (JOY_NEW(A_BUTTON))
            {
                gUnknown_03004828->state ++;
            }
            break;
        case 72: // Only if in-game trade
            sub_804BA94(gSpecialVar_0x8005, 0);
            gCB2_AfterEvolution = sub_804BBCC;
            evoTarget = GetEvolutionTargetSpecies(&gPlayerParty[gSelectedTradeMonPositions[0]], TRUE, ITEM_NONE);
            if (evoTarget != SPECIES_NONE)
                TradeEvolutionScene(&gPlayerParty[gSelectedTradeMonPositions[0]], evoTarget, gUnknown_03004828->monSpriteIds[1], gSelectedTradeMonPositions[0]);
            gUnknown_03004828->state ++;
            break;
        case 73:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gUnknown_03004828->state ++;
            break;
        case 74:
            if (!gPaletteFade.active)
            {
                PlayBGM(gUnknown_03004828->cachedMapMusic);
                SetMainCallback2(CB2_ReturnToField);
                BufferInGameTradeMonName();
            }
            break;
    }
    return FALSE;
}

static void sub_804D588(void)
{
    u16 evoTarget;
    switch (gMain.state)
    {
        case 0:
            gMain.state = 4;
            gSoftResetDisabled = TRUE;
            break;
        case 4:
            gCB2_AfterEvolution = sub_804DC88;
            evoTarget = GetEvolutionTargetSpecies(&gPlayerParty[gSelectedTradeMonPositions[0]], TRUE, ITEM_NONE);
            if (evoTarget != SPECIES_NONE)
                TradeEvolutionScene(&gPlayerParty[gSelectedTradeMonPositions[0]], evoTarget, gUnknown_03004828->monSpriteIds[1], gSelectedTradeMonPositions[0]);
            else
                SetMainCallback2(sub_804DC88);
            gSelectedTradeMonPositions[0] = 255;
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
    u8 blockReceivedStatus;
    TradeGetMultiplayerId();
    blockReceivedStatus = GetBlockReceivedStatus();
    if (blockReceivedStatus & 0x01)
    {
        if (gBlockRecvBuffer[0][0] == 0xdcba)
        {
            SetMainCallback2(sub_804D588);
        }
        if (gBlockRecvBuffer[0][0] == 0xabcd)
        {
            gUnknown_03004828->playerFinishStatus = 1;
        }
        ResetBlockReceivedFlag(0);
    }
    if (blockReceivedStatus & 0x02)
    {
        if (gBlockRecvBuffer[1][0] == 0xabcd)
        {
            gUnknown_03004828->partnerFinishStatus = 1;
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
        BeginNormalPaletteFade(1 << (16 + sprite->oam.paletteNum), -1, 0, 16, FADE_COLOR_WHITE);
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
            gUnknown_03004828->state = 14; // Resume the master trade animation
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
    u8 nickname[32];
    const struct InGameTrade *inGameTrade = &sIngameTrades[gSpecialVar_0x8004];
    GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    StringCopy(gStringVar2, gSpeciesNames[inGameTrade->species]);
}

static void CreateInGameTradePokemonInternal(u8 whichPlayerMon, u8 whichInGameTrade)
{
    const struct InGameTrade *inGameTrade = &sIngameTrades[whichInGameTrade];
    u8 level = GetMonData(&gPlayerParty[whichPlayerMon], MON_DATA_LEVEL);

    struct MailStruct mail;
    u8 metLocation = 0xFE;
    u8 mailNum;
    struct Pokemon *pokemon = &gEnemyParty[0];

    CreateMon(pokemon, inGameTrade->species, level, 32, TRUE, inGameTrade->personality, TRUE, inGameTrade->otId);

    SetMonData(pokemon, MON_DATA_HP_IV, &inGameTrade->ivs[0]);
    SetMonData(pokemon, MON_DATA_ATK_IV, &inGameTrade->ivs[1]);
    SetMonData(pokemon, MON_DATA_DEF_IV, &inGameTrade->ivs[2]);
    SetMonData(pokemon, MON_DATA_SPEED_IV, &inGameTrade->ivs[3]);
    SetMonData(pokemon, MON_DATA_SPATK_IV, &inGameTrade->ivs[4]);
    SetMonData(pokemon, MON_DATA_SPDEF_IV, &inGameTrade->ivs[5]);
    SetMonData(pokemon, MON_DATA_NICKNAME, inGameTrade->nickname);
    SetMonData(pokemon, MON_DATA_OT_NAME, inGameTrade->otName);
    SetMonData(pokemon, MON_DATA_OT_GENDER, &inGameTrade->otGender);
    SetMonData(pokemon, MON_DATA_ABILITY_NUM, &inGameTrade->abilityNum);
    SetMonData(pokemon, MON_DATA_BEAUTY, &inGameTrade->conditions[1]);
    SetMonData(pokemon, MON_DATA_CUTE, &inGameTrade->conditions[2]);
    SetMonData(pokemon, MON_DATA_COOL, &inGameTrade->conditions[0]);
    SetMonData(pokemon, MON_DATA_SMART, &inGameTrade->conditions[3]);
    SetMonData(pokemon, MON_DATA_TOUGH, &inGameTrade->conditions[4]);
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

static void GetInGameTradeMail(struct MailStruct *mail, const struct InGameTrade *trade) {
    s32 i;

    for (i = 0; i < 9; i++)
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

void sub_804DB84(void)
{
    if (sub_804C29C() == TRUE)
    {
        DestroySprite(&gSprites[gUnknown_03004828->monSpriteIds[0]]);
        FreeSpriteOamMatrix(&gSprites[gUnknown_03004828->monSpriteIds[1]]);
        sub_804BA94(gSelectedTradeMonPositions[0], gSelectedTradeMonPositions[1] % 6);
        gUnknown_03004828->linkData[0] = 0xabcd;
        gUnknown_03004828->scheduleLinkTransfer = 1;
        SetMainCallback2(sub_804DC18);
    }
    sub_804BB78();
    HandleLinkDataReceive();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_804DC18(void)
{
    u8 mpId = TradeGetMultiplayerId();
    HandleLinkDataReceive();
    if (mpId == 0 && gUnknown_03004828->playerFinishStatus == 1 && gUnknown_03004828->partnerFinishStatus == 1)
    {
        gUnknown_03004828->linkData[0] = 0xdcba;
        Trade_SendData(gUnknown_03004828);
        gUnknown_03004828->playerFinishStatus = 2;
        gUnknown_03004828->partnerFinishStatus = 2;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void sub_804DC88(void)
{
    switch (gMain.state)
    {
        case 0:
            gUnknown_03004828 = &ewram_2010000.unk_0f000;
            gMain.state ++;
            Text_EraseWindowRect(&gUnknown_03004828->window, 0, 0, 29, 19);
            StringExpandPlaceholders(gStringVar4, gOtherText_LinkStandby2);
            Text_InitWindowAndPrintText(&gUnknown_03004828->window, gStringVar4, gUnknown_03004828->textWindowBaseTileNum, 2, 15);
            break;
        case 1:
            SetLinkStandbyCallback();
            gMain.state = 100;
            gUnknown_03004828->timer = 0;
            break;
        case 100:
            if (++ gUnknown_03004828->timer > 180)
            {
                gMain.state = 101;
                gUnknown_03004828->timer = 0;
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
            Text_EraseWindowRect(&gUnknown_03004828->window, 0, 0, 29, 19);
            Text_InitWindowAndPrintText(&gUnknown_03004828->window, gSystemText_Saving, gUnknown_03004828->textWindowBaseTileNum, 2, 15);
            break;
        case 50:
            SetSecretBase2Field_9_AndHideBG();
            IncrementGameStat(GAME_STAT_POKEMON_TRADES);
            sub_8125D80();
            gMain.state ++;
            gUnknown_03004828->timer = 0;
            break;
        case 51:
            if (++ gUnknown_03004828->timer == 5)
            {
                gMain.state ++;
            }
            break;
        case 52:
            if (sub_8125DA8())
            {
                ClearSecretBase2Field_9_2();
                gMain.state = 4;
            }
            else
            {
                gUnknown_03004828->timer = 0;
                gMain.state = 51;
            }
            break;
        case 4:
            sub_8125DDC();
            gMain.state = 40;
            gUnknown_03004828->timer = 0;
            break;
        case 40:
            if (++ gUnknown_03004828->timer > 50)
            {
                gUnknown_03004828->timer = 0;
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
                sub_8125E04();
                gSoftResetDisabled = FALSE;
                gMain.state = 5;
            }
            break;
        case 5:
            if (++ gUnknown_03004828->timer > 60)
            {
                gMain.state ++;
                SetLinkStandbyCallback();
            }
            break;
        case 6:
            if (IsLinkTaskFinished())
            {
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
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
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void Task_InGameTrade(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_InitInGameTrade);
        gFieldCallback = sub_8080990;
        DestroyTask(taskId);
    }
}

static void CheckPartnersMonForRibbons(void)
{
    u8 i;
    u8 numRibbons = 0;
    for (i = 0; i < 12; i ++)
    {
        numRibbons += GetMonData(&gEnemyParty[gSelectedTradeMonPositions[1] % 6], MON_DATA_CHAMPION_RIBBON + i);
    }
    if (numRibbons != 0)
        FlagSet(FLAG_SYS_RIBBON_GET);
}

void sub_804E22C(void)
{
    LZDecompressVram(gBattleTextboxTiles, (void *)VRAM);
    CpuCopy16(gBattleTextboxTilemap, gSharedMem, 0x1000);
    DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
    LoadCompressedPalette(gBattleTextboxPalette, 0, 32);
    REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5);
}
