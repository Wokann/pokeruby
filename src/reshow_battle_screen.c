#include "global.h"
#include "sprite.h"
#include "reshow_battle_screen.h"
#include "battle_interface.h"
#include "battle_controllers.h"
#include "battle_main.h"
#include "battle.h"
#include "battle_gfx_sfx_util.h"
#include "battle_anim.h"
#include "palette.h"
#include "main.h"
#include "scanline_effect.h"
#include "text.h"
#include "data.h"
#include "ewram.h"
#include "constants/trainers.h"

extern struct Window gWindowTemplate_Contest_MoveDescription;

bool8 LoadChosenBattleElement(u8 caseId);

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
    gPaletteFade.bufferTransferDisabled = TRUE;
    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);
    REG_MOSAIC = 0;
    gReshowMainState = 0;
    gReshowHelperState = 0;
    SetMainCallback2(CB2_ReshowBattleScreenAfterMenu);
}

static void CB2_ReshowBattleScreenAfterMenu(void)
{
    switch (gReshowMainState)
    {
    case 0:
        ScanlineEffect_Clear();
        Text_LoadWindowTemplate(&gWindowTemplate_81E6C58);
        ResetPaletteFade();
        Text_InitWindowWithTemplate(&gWindowTemplate_Contest_MoveDescription, &gWindowTemplate_81E6C58);
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
            CpuFastSet(&zero, (void*) VRAM, CPU_FAST_SET_SRC_FIXED | (VRAM_SIZE / sizeof(u32)));
        }
        break;
    case 2:
        if (!LoadChosenBattleElement(gReshowHelperState))
        {
            gReshowHelperState++;
            gReshowMainState--;
        }
        else
            gReshowHelperState = 0;
        break;
    case 3:
        ResetSpriteData();
        break;
    case 4:
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = MAX_BATTLERS_COUNT;
        break;
    case 5:
        ClearSpritesHealthboxAnimData();
        break;
    case 6:
        if (BattleLoadAllHealthBoxesGfx(gReshowHelperState))
            gReshowHelperState = 0;
        else
        {
            gReshowHelperState++;
            gReshowMainState--;
        }
        break;
    case 7:
        if (!LoadBattlerSpriteGfx(0))
            gReshowMainState--;
        break;
    case 8:
        if (!LoadBattlerSpriteGfx(1))
            gReshowMainState--;
        break;
    case 9:
        if (!LoadBattlerSpriteGfx(2))
            gReshowMainState--;
        break;
    case 10:
        if (!LoadBattlerSpriteGfx(3))
            gReshowMainState--;
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

            opponentBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[opponentBattler]], MON_DATA_SPECIES);
            SetBattlerShadowSpriteCallback(opponentBattler, species);

            if (IsDoubleBattle())
            {
                opponentBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
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
        BeginHardwarePaletteFade(BLDCNT_TGT1_ALL | BLDCNT_EFFECT_DARKEN, 0, 0x10, 0, TRUE);
        gPaletteFade.bufferTransferDisabled = FALSE;
        SetMainCallback2(BattleMainCB2);
        break;
    }
    gReshowMainState++;
}

static void ClearBattleBgCntBaseBlocks(void)
{
    sub_800D6D4();
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

        gReshowHelperState = 0;
    }
    return TRUE;
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
            gBattlerSpriteIds[battler] = CreateSprite(&gMultiuseSpriteTemplate, GetBattlerSpriteCoord(battler, BATTLER_COORD_X_2), posY, GetBattlerSpriteSubpriority(battler));
            gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
            gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
            gSprites[gBattlerSpriteIds[battler]].data[2] = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
            StartSpriteAnim(&gSprites[gBattlerSpriteIds[battler]], gBattleMonForms[battler]);
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI && battler == 0)
        {
            SetMultiuseSpriteTemplateToTrainerBack(gSaveBlock2.playerGender, GetBattlerPosition(0));
            gBattlerSpriteIds[battler] = CreateSprite(&gMultiuseSpriteTemplate, 0x50,
                                                (8 - gTrainerBackPicCoords[gSaveBlock2.playerGender].size) * 4 + 80,
                                                 GetBattlerSpriteSubpriority(0));
            gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
            gSprites[gBattlerSpriteIds[battler]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[battler]].data[0] = battler;
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL && battler == 0)
        {
            SetMultiuseSpriteTemplateToTrainerBack(TRAINER_BACK_PIC_WALLY, GetBattlerPosition(0));
            gBattlerSpriteIds[battler] = CreateSprite(&gMultiuseSpriteTemplate, 0x50,
                                                (8 - gTrainerBackPicCoords[TRAINER_BACK_PIC_WALLY].size) * 4 + 80,
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
            gBattlerSpriteIds[battler] = CreateSprite(&gMultiuseSpriteTemplate, GetBattlerSpriteCoord(battler, BATTLER_COORD_X_2), posY, GetBattlerSpriteSubpriority(battler));
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
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gEnemyParty[gBattlerPartyIndexes[battler]], HEALTHBOX_ALL);
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gPlayerParty[gBattlerPartyIndexes[battler]], HEALTHBOX_SAFARI_ALL_TEXT);
        else
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gPlayerParty[gBattlerPartyIndexes[battler]], HEALTHBOX_ALL);
        if (GetBattlerPosition(battler) == B_POSITION_OPPONENT_RIGHT || GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT)
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
