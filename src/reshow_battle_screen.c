#include "global.h"
#include "reshow_battle_screen.h"
#include "battle.h"
#include "battle_controllers.h"
#include "palette.h"
#include "main.h"
#include "scanline_effect.h"
#include "text.h"
#include "battle_anim.h"
#include "battle_interface.h"
#include "sprite.h"
#include "data2.h"
#include "ewram.h"
#include "constants/trainers.h"

extern struct SpriteTemplate gCreatingSpriteTemplate;
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern u8 gActionSelectionCursor[4];
extern u8 gBattlerInMenuId;
extern u16 gBattlerPartyIndexes[4];
extern u8 gBattlersCount;
extern u16 gBattleTypeFlags;
extern u8 gBattlerSpriteIds[4];
extern u8 gBattleMonForms[4];
extern u8 gHealthboxSpriteIds[4];

bool8 LoadChosenBattleElement(u8 a0);
bool8 BattleLoadAllHealthBoxesGfx(u8 state);
void ClearSpritesHealthboxAnimData(void);
void LoadAndCreateEnemyShadowSprites(void);
void HBlankCB_Battle(void);
void BattleLoadOpponentMonSpriteGfx(struct Pokemon *, u8 battler);
void BattleLoadPlayerMonSpriteGfx(struct Pokemon *, u8 battler);
void BattleLoadSubstituteOrMonSpriteGfx(u8 bank, u8 loadMonSprite);
void DecompressTrainerBackPic(u16 a0, u8 bank);

// this file's functions
static void CB2_ReshowBattleScreenAfterMenu(void);
static bool8 LoadBattlerSpriteGfx(u8 battler);
static void CreateBattlerSprite(u8 battler);
static void CreateHealthboxSprite(u8 battler);
static void ClearBattleBgCntBaseBlocks(void);

void ReshowBattleScreenDummy(void)
{

}

void ReshowBattleScreenAfterMenu(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    SetHBlankCallback(0);
    SetVBlankCallback(0);
    REG_MOSAIC = 0;
    gReshowBattleScreenMainState = 0;
    gReshowBattleScreenHelperState = 0;
    SetMainCallback2(CB2_ReshowBattleScreenAfterMenu);
}

static void CB2_ReshowBattleScreenAfterMenu(void)
{
    switch (gReshowBattleScreenMainState)
    {
    case 0:
        ScanlineEffect_Clear();
        Text_LoadWindowTemplate(&gStandardBattleWindowTemplate);
        ResetPaletteFade();
        Text_InitWindowWithTemplate(&gWindowTemplate_Contest_MoveDescription, &gStandardBattleWindowTemplate);
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 0;
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 0;
        gBattle_BG2_X = 0;
        gBattle_BG2_Y = 0;
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;
        break;
    case 1:
        {
            const u32 zero = 0;
            CpuFastSet(&zero, (void*) VRAM, 0x1006000);
        }
        break;
    case 2:
        if (!LoadChosenBattleElement(gReshowBattleScreenHelperState))
        {
            gReshowBattleScreenHelperState++;
            gReshowBattleScreenMainState--;
        }
        else
            gReshowBattleScreenHelperState = 0;
        break;
    case 3:
        ResetSpriteData();
        break;
    case 4:
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 4;
        break;
    case 5:
        ClearSpritesHealthboxAnimData();
        break;
    case 6:
        if (BattleLoadAllHealthBoxesGfx(gReshowBattleScreenHelperState))
            gReshowBattleScreenHelperState = 0;
        else
        {
            gReshowBattleScreenHelperState++;
            gReshowBattleScreenMainState--;
        }
        break;
    case 7:
        if (!LoadBattlerSpriteGfx(0))
            gReshowBattleScreenMainState--;
        break;
    case 8:
        if (!LoadBattlerSpriteGfx(1))
            gReshowBattleScreenMainState--;
        break;
    case 9:
        if (!LoadBattlerSpriteGfx(2))
            gReshowBattleScreenMainState--;
        break;
    case 10:
        if (!LoadBattlerSpriteGfx(3))
            gReshowBattleScreenMainState--;
        break;
    case 11:
        CreateBattlerSprite(0);
        break;
    case 12:
        CreateBattlerSprite(1);
        break;
    case 13:
        CreateBattlerSprite(2);
        break;
    case 14:
        CreateBattlerSprite(3);
        break;
    case 15:
        CreateHealthboxSprite(0);
        break;
    case 16:
        CreateHealthboxSprite(1);
        break;
    case 17:
        CreateHealthboxSprite(2);
        break;
    case 18:
        CreateHealthboxSprite(3);
        break;
    case 19:
        {
            u8 opponentBattler;
            u16 species;

            LoadAndCreateEnemyShadowSprites();

            opponentBattler = GetBattlerAtPosition(1);
            species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[opponentBattler]], MON_DATA_SPECIES);
            SetBattlerShadowSpriteCallback(opponentBattler, species);

            if (IsDoubleBattle())
            {
                opponentBattler = GetBattlerAtPosition(3);
                species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[opponentBattler]], MON_DATA_SPECIES);
                SetBattlerShadowSpriteCallback(opponentBattler, species);
            }
            ActionSelectionCreateCursorAt(gActionSelectionCursor[gBattlerInMenuId], 0);
        }
        break;
    default:
        SetHBlankCallback(HBlankCB_Battle);
        SetVBlankCallback(VBlankCB_Battle);
        ClearBattleBgCntBaseBlocks();
        BeginHardwarePaletteFade(0xFF, 0, 0x10, 0, 1);
        gPaletteFade.bufferTransferDisabled = 0;
        SetMainCallback2(BattleMainCB2);
        break;
    }
    gReshowBattleScreenMainState++;
}

static void ClearBattleBgCntBaseBlocks(void)
{
    InitBattleBgsVideo();
    ((vBgCnt *)&REG_BG1CNT)->charBaseBlock = 0;
    ((vBgCnt *)&REG_BG2CNT)->charBaseBlock = 0;
}

static bool8 LoadBattlerSpriteGfx(u8 battler)
{
    if (battler < gBattlersCount)
    {
        if (GetBattlerSide(battler))
        {
            if (!gBattleSpriteInfo[battler].behindSubstitute)
                BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[battler]], battler);
            else
                BattleLoadSubstituteOrMonSpriteGfx(battler, FALSE);
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI && battler == 0)
            DecompressTrainerBackPic(gSaveBlock2.playerGender, 0);
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL && battler == 0)
            DecompressTrainerBackPic(TRAINER_BACK_PIC_WALLY, 0);
        else if (!gBattleSpriteInfo[battler].behindSubstitute)
            BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[battler]], battler);
        else
            BattleLoadSubstituteOrMonSpriteGfx(battler, FALSE);

        gReshowBattleScreenHelperState = 0;
    }
    return 1;
}

static void CreateBattlerSprite(u8 battler)
{
    if (battler < gBattlersCount)
    {
        u8 posY;

        if (gBattleSpriteInfo[battler].behindSubstitute)
            posY = GetSubstituteSpriteDefault_Y(battler);
        else
            posY = GetBattlerSpriteDefault_Y(battler);
        if (GetBattlerSide(battler))
        {
            if (GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_HP) == 0)
                return;
            SetMultiuseSpriteTemplateToPokemon(GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES), GetBattlerPosition(battler));
            gBattlerSpriteIds[battler] = CreateSprite(&gCreatingSpriteTemplate, GetBattlerSpriteCoord(battler, 2), posY, GetBattlerSpriteSubpriority(battler));
            gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
            gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
            gSprites[gBattlerSpriteIds[battler]].data[2] = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
            StartSpriteAnim(&gSprites[gBattlerSpriteIds[battler]], gBattleMonForms[battler]);
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI && battler == 0)
        {
            SetMultiuseSpriteTemplateToTrainerBack(gSaveBlock2.playerGender, GetBattlerPosition(0));
            gBattlerSpriteIds[battler] = CreateSprite(&gCreatingSpriteTemplate, 0x50,
                                                (8 - gTrainerBackPicCoords[gSaveBlock2.playerGender].coords) * 4 + 80,
                                                 GetBattlerSpriteSubpriority(0));
            gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
            gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL && battler == 0)
        {
            SetMultiuseSpriteTemplateToTrainerBack(TRAINER_BACK_PIC_WALLY, GetBattlerPosition(0));
            gBattlerSpriteIds[battler] = CreateSprite(&gCreatingSpriteTemplate, 0x50,
                                                (8 - gTrainerBackPicCoords[2].coords) * 4 + 80,
                                                 GetBattlerSpriteSubpriority(0));
            gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
            gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
        }
        else
        {
            if (GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_HP) == 0)
                return;
            SetMultiuseSpriteTemplateToPokemon(GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES), GetBattlerPosition(battler));
            gBattlerSpriteIds[battler] = CreateSprite(&gCreatingSpriteTemplate, GetBattlerSpriteCoord(battler, 2), posY, GetBattlerSpriteSubpriority(battler));
            gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
            gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
            gSprites[gBattlerSpriteIds[battler]].data[2] = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
            StartSpriteAnim(&gSprites[gBattlerSpriteIds[battler]], gBattleMonForms[battler]);
        }
        gSprites[gBattlerSpriteIds[battler]].invisible = gBattleSpriteInfo[battler].invisible;
    }
}

static void CreateHealthboxSprite(u8 battler)
{
    if (battler < gBattlersCount)
    {
        u8 healthboxSpriteId;
        if (gBattleTypeFlags & BATTLE_TYPE_SAFARI && battler == 0)
            healthboxSpriteId = CreateSafariPlayerHealthboxSprites();
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL && battler == 0)
            return;
        else
            healthboxSpriteId = CreateBattlerHealthboxSprites(battler);
        gHealthboxSpriteIds[battler] = healthboxSpriteId;
        InitBattlerHealthboxCoords(battler);
        SetHealthboxSpriteVisible(healthboxSpriteId);
        if (GetBattlerSide(battler))
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gEnemyParty[gBattlerPartyIndexes[battler]], 0);
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gPlayerParty[gBattlerPartyIndexes[battler]], 10);
        else
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gPlayerParty[gBattlerPartyIndexes[battler]], 0);
        if (GetBattlerPosition(battler) == 3 || GetBattlerPosition(battler) == 2)
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[battler], TRUE);
        else
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[battler], FALSE);
        if (GetBattlerSide(battler))
        {
            if (GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_HP) == 0)
                SetHealthboxSpriteInvisible(healthboxSpriteId);
        }
        else if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
        {
            if (GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_HP) == 0)
                SetHealthboxSpriteInvisible(healthboxSpriteId);
        }
    }
}
