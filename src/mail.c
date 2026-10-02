#include "global.h"
#include "mail.h"
#include "data.h"
#include "easy_chat.h"
#include "constants/items.h"
#include "constants/species.h"
#include "field_fadetransition.h"
#include "graphics.h"
#include "menu.h"
#include "menu_helpers.h"
#include "international_string_util.h"
#include "palette.h"
#include "pokemon_icon.h"
#include "overworld.h"
#include "script.h"
#include "sprite.h"
#include "string_util.h"
#include "strings2.h"
#include "task.h"
#include "text.h"
#include "scanline_effect.h"
#include "ewram.h"

extern u8 (*gMenuCallback)(void);

struct MailLineLayout
{
    u8 yOffset:2;
    u8 numEasyChatWords:2;
    u8 xOffset:4;
};

struct MailLayout
{
    u8 numLines;
    u8 signatureYPos;
    u8 signatureWidth;
    u8 wordsYPos:4;
    u8 wordsXPos:4;
    const struct MailLineLayout *lines;
};

struct MailRead
{
    /* 0x000 */ u8 message[8][27];
    /* 0x0D8 */ u8 playerName[20];
    /* 0x0EC */ MainCallback exitCallback;
    /* 0x0F0 */ MainCallback callback;
    /* 0x0F4 */ struct Mail *mail;
    /* 0x0F8 */ bool8 hasText;
    /* 0x0F9 */ u8 signatureWidth;
    /* 0x0FA */ u8 mailType;
    /* 0x0FB */ u8 iconType;
    /* 0x0FC */ u8 monIconSpriteId;
    u8 unusedFD;
    /* 0x0FE */ u8 unusedFE;
    /* 0x0FF */ u8 language;
    /* 0x100 */ bool8 international;
    u8 pad101[3];
    /* 0x104 */ u8 *(*parserSingle)(u8 *dest, u16 word);
    /* 0x108 */ u8 *(*parserMultiple)(u8 *dest, u16 *words, u16, u16);
    /* 0x10C */ const struct MailLayout *layout;
};

struct MailGraphics
{
    const u16 *palette;
    const u8 *tiles;
    const u8 *tileMap;
    u16 unused1;
    u16 unused2;
    u16 textColor;
    u16 textShadow;
};

enum
{
    ICON_TYPE_NONE,
    ICON_TYPE_BEAD,
    ICON_TYPE_DREAM,
};

static const u16 sBgColors[][2] =
{
    {0x6ACD, 0x51A5},
    {0x45FC, 0x38D4},
};

static const struct MailGraphics sMailGraphics[] =
{
    {
        .palette = gMailPalette_Orange,
        .tiles = gMailTiles_Orange,
        .tileMap = gMailTilemap_Orange,
        .unused1 = 0x2C0,
        .unused2 = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Harbor,
        .tiles = gMailTiles_Harbor,
        .tileMap = gMailTilemap_Harbor,
        .unused1 = 0x2E0,
        .unused2 = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Glitter,
        .tiles = gMailTiles_Glitter,
        .tileMap = gMailTilemap_Glitter,
        .unused1 = 0x400,
        .unused2 = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Mech,
        .tiles = gMailTiles_Mech,
        .tileMap = gMailTilemap_Mech,
        .unused1 = 0x1E0,
        .unused2 = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Wood,
        .tiles = gMailTiles_Wood,
        .tileMap = gMailTilemap_Wood,
        .unused1 = 0x2E0,
        .unused2 = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Wave,
        .tiles = gMailTiles_Wave,
        .tileMap = gMailTilemap_Wave,
        .unused1 = 0x300,
        .unused2 = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Bead,
        .tiles = gMailTiles_Bead,
        .tileMap = gMailTilemap_Bead,
        .unused1 = 0x140,
        .unused2 = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Shadow,
        .tiles = gMailTiles_Shadow,
        .tileMap = gMailTilemap_Shadow,
        .unused1 = 0x300,
        .unused2 = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Tropic,
        .tiles = gMailTiles_Tropic,
        .tileMap = gMailTilemap_Tropic,
        .unused1 = 0x220,
        .unused2 = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Dream,
        .tiles = gMailTiles_Dream,
        .tileMap = gMailTilemap_Dream,
        .unused1 = 0x340,
        .unused2 = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Fab,
        .tiles = gMailTiles_Fab,
        .tileMap = gMailTilemap_Fab,
        .unused1 = 0x2A0,
        .unused2 = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Retro,
        .tiles = gMailTiles_Retro,
        .tileMap = gMailTilemap_Retro,
        .unused1 = 0x520,
        .unused2 = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
};

static const struct MailLineLayout sMailLineLayouts_Wide[] =
{
    {0, 3, 0},
    {0, 3, 0},
    {0, 3, 0},
};

static const struct MailLayout sMailLayouts_Wide[] =
{
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 16, 27, 4, 3, sMailLineLayouts_Wide},
    {3, 15, 27, 4, 3, sMailLineLayouts_Wide},
};

static const struct MailLineLayout sMailLineLayouts_Tall[] =
{
    {0, 2, 0},
    {0, 2, 0},
    {0, 2, 0},
    {0, 2, 0},
    {0, 1, 0},
};

static const struct MailLayout sMailLayouts_Tall[] =
{
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 16, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 16, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 16, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 16, 15, 4, 3, sMailLineLayouts_Tall},
    {5, 15, 15, 4, 3, sMailLineLayouts_Tall},
};

#if DEBUG

static const u8 sText_DebugMailLayoutPrompt[] = DTR("レイアウトを　にほんごで　ひょうじ？", "Display using JAPANESE layout?");
static const u8 sText_DebugMailLayoutJapanese[] = DTR("にほんご　", "JAPANESE");
static const u8 sText_DebugMailLayoutOverseas[] = DTR("かいがい　", "OVERSEAS");

static const u8 *const sDebugMailLayouts[] =
{
    sText_DebugMailLayoutJapanese,
    sText_DebugMailLayoutOverseas,
};

static const u8 sText_DebugMailImageTypePrompt[] = DTR("イメージ　タイプを　えらんでね", "Choose an image type");
static const u8 sText_DebugMailImageTypeLabel[] = _("{STR_VAR_1}");
static const u8 sText_DebugMailImageTypeOrange[] = DTR("オレンジ　", "ORANGE ");
static const u8 sText_DebugMailImageTypeHarbor[] = DTR("ハーバー　", "HARBOR ");
static const u8 sText_DebugMailImageTypeGlitter[] = DTR("キラキラ　", "GLITTER");
static const u8 sText_DebugMailImageTypeMech[] = DTR("メカニカル", "MECH   ");
static const u8 sText_DebugMailImageTypeWood[] = DTR("ウッディー", "WOOD   ");
static const u8 sText_DebugMailImageTypeWave[] = DTR("クロス　　", "WAVE   ");
static const u8 sText_DebugMailImageTypeBead[] = DTR("トレジャー", "BEAD   ");
static const u8 sText_DebugMailImageTypeShadow[] = DTR("シャドウ　", "SHADOW ");
static const u8 sText_DebugMailImageTypeTropic[] = DTR("トロピカル", "TROPIC ");
static const u8 sText_DebugMailImageTypeDream[] = DTR("ドリーム　", "DREAM  ");
static const u8 sText_DebugMailImageTypeFab[] = DTR("ミラクル　", "FAB    ");
static const u8 sText_DebugMailImageTypeRetro[] = DTR("レトロ　　", "RETRO  ");

static const u8 *const sDebugMailImageTypes[] =
{
    sText_DebugMailImageTypeOrange,
    sText_DebugMailImageTypeHarbor,
    sText_DebugMailImageTypeGlitter,
    sText_DebugMailImageTypeMech,
    sText_DebugMailImageTypeWood,
    sText_DebugMailImageTypeWave,
    sText_DebugMailImageTypeBead,
    sText_DebugMailImageTypeShadow,
    sText_DebugMailImageTypeTropic,
    sText_DebugMailImageTypeDream,
    sText_DebugMailImageTypeFab,
    sText_DebugMailImageTypeRetro,
};

static const u8 sText_DebugMailActionView[] = DTR("メールをみる", "View MAIL");
static const u8 sText_DebugMailActionRegister[] = DTR("メールとうろく", "Register MAIL");
static const u8 sText_DebugMailActionGive[] = DTR("もたせる", "Give MAIL");
static const u8 sText_DebugMailActionDelete[] = DTR("メールをけす", "Delete MAIL");

#endif

// Ruby uses shared EWRAM for the mail reader instead of Emerald's allocation.
static struct MailRead *const sMailRead = (struct MailRead *)gSharedMem;

static u8 MailReadBuildGraphics(void);
static void CB2_InitMailRead(void);
static void BufferMailText(void);
static void PrintMailText(void);
static void VBlankCB_MailRead(void);
static void CB2_MailRead(void);
static void CB2_WaitForPaletteExitOnKeyPress(void);
static void CB2_ExitOnKeyPress(void);
static void CB2_ExitMailReadFreeVars(void);

extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u16 gSpecialVar_0x8006;

#if DEBUG
static u8 sDebugMailReadActive;
#endif

void ReadMail(struct Mail *mail, MainCallback exitCallback, bool8 hasText)
{
    u16 speciesBuffer[2];
    u16 species;

#if DEBUG
    if (sDebugMailReadActive != 0)
    {
        sMailRead->language = GAME_LANGUAGE;
        sMailRead->international = gSpecialVar_0x8004;
        sMailRead->parserSingle = CopyEasyChatWord;
        sMailRead->parserMultiple = ConvertEasyChatWordsToString;
        sMailRead->mailType = gSpecialVar_0x8006;
    }
    else
#endif
    {
        sMailRead->language = GAME_LANGUAGE;
        sMailRead->international = TRUE;
        sMailRead->parserSingle = CopyEasyChatWord;
        sMailRead->parserMultiple = ConvertEasyChatWordsToString;
        if (IS_ITEM_MAIL(mail->itemId))
        {
            sMailRead->mailType = ITEM_TO_MAIL(mail->itemId);
        }
        else
        {
            sMailRead->mailType = ITEM_TO_MAIL(FIRST_MAIL_INDEX);
            hasText = FALSE;
        }
    }

    switch (sMailRead->international)
    {
    case 0:
    default:
        sMailRead->layout = &sMailLayouts_Wide[sMailRead->mailType];
        break;
    case 1:
        sMailRead->layout = &sMailLayouts_Tall[sMailRead->mailType];
        break;
    }

    species = MailSpeciesToSpecies(mail->species, speciesBuffer);
    if (species > SPECIES_NONE && species < NUM_SPECIES)
    {
        switch (sMailRead->mailType)
        {
        case ITEM_TO_MAIL(ITEM_BEAD_MAIL):
            sMailRead->iconType = ICON_TYPE_BEAD;
            break;
        case ITEM_TO_MAIL(ITEM_DREAM_MAIL):
            sMailRead->iconType = ICON_TYPE_DREAM;
            break;
        default:
            sMailRead->iconType = ICON_TYPE_NONE;
            break;
        }
    }
    else
    {
        sMailRead->iconType = ICON_TYPE_NONE;
    }

    sMailRead->mail = mail;
    sMailRead->exitCallback = exitCallback;
    sMailRead->hasText = hasText;

    SetMainCallback2(CB2_InitMailRead);
}

#define RETURN_UP_STATE break
#define RETURN_SKIP_STATE return FALSE

static u8 MailReadBuildGraphics(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        ScanlineEffect_Stop();
        REG_DISPCNT = 0;
        RETURN_UP_STATE;

    case 1: CpuFill16(0, (void *)OAM, OAM_SIZE);
        RETURN_UP_STATE;

    case 2:
        ResetPaletteFade();
        RETURN_UP_STATE;

    case 3:
        ResetTasks();
        RETURN_UP_STATE;

    case 4:
        ResetSpriteData();
        RETURN_UP_STATE;

    case 5:
        FreeAllSpritePalettes();
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG2HOFS = 0;
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 0;
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        RETURN_UP_STATE;

    case 6:
        Text_LoadWindowTemplate(&gMenuWindowTemplate);
        RETURN_UP_STATE;

    case 7:
        MultistepInitMenuWindowBegin(&gMenuWindowTemplate);
        RETURN_UP_STATE;

    case 8:
        if (MultistepInitMenuWindowContinue() == 0)
        {
            return FALSE;
        }
        RETURN_UP_STATE;

    case 9:
        Menu_EraseScreen();
        RETURN_UP_STATE;

    case 10: CpuFill16(1, (void *)(VRAM + 0x4800), 0x800);
        RETURN_UP_STATE;

    case 11:
        LoadPalette(sMailGraphics[sMailRead->mailType].palette, 0, 16 * 2);
        RETURN_UP_STATE;

    case 12:
        LZ77UnCompVram(sMailGraphics[sMailRead->mailType].tileMap, (void *)(VRAM + 0x4000));
        RETURN_UP_STATE;

    case 13:
        LZ77UnCompVram(sMailGraphics[sMailRead->mailType].tiles, (void *)(VRAM));

        gPlttBufferUnfaded[241] = sMailGraphics[sMailRead->mailType].textColor;
        gPlttBufferUnfaded[248] = sMailGraphics[sMailRead->mailType].textShadow;
        gPlttBufferUnfaded[10] = sBgColors[gSaveBlock2.playerGender][0];
        gPlttBufferUnfaded[11] = sBgColors[gSaveBlock2.playerGender][1];
        RETURN_UP_STATE;

    case 14:
        if (sMailRead->hasText)
        {
            BufferMailText();
        }
        RETURN_UP_STATE;

    case 15:
        if (sMailRead->hasText)
        {
            PrintMailText();
        }

        SetVBlankCallback(VBlankCB_MailRead);
        gPaletteFade.bufferTransferDisabled = 1;
        RETURN_UP_STATE;

    case 16:
    {
        u16 iconSpecies;

        iconSpecies = GetIconSpeciesNoPersonality(sMailRead->mail->species);

        switch (sMailRead->iconType)
        {
        case ICON_TYPE_BEAD:
            LoadMonIconPalette(iconSpecies);
            sMailRead->monIconSpriteId = CreateMonIconNoPersonality(iconSpecies, SpriteCallbackDummy, 96, 128, 0);
            break;

        case ICON_TYPE_DREAM:
            LoadMonIconPalette(iconSpecies);
            sMailRead->monIconSpriteId = CreateMonIconNoPersonality(iconSpecies, SpriteCallbackDummy, 40, 128, 0);
            break;
        }
        RETURN_UP_STATE;
    }

    case 17:
        if (Overworld_IsRecvQueueAtMax() != TRUE)
        {
            RETURN_UP_STATE;
        }
        RETURN_SKIP_STATE;

    case 18:
        REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_TXT256x512;
        REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(8)  | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(9)  | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BLDCNT = 0;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB(0, 0, 0));
        gPaletteFade.bufferTransferDisabled = FALSE;
        sMailRead->callback = CB2_WaitForPaletteExitOnKeyPress;
        return TRUE;

    default:
        return FALSE;
    }

    gMain.state += 1;
    return FALSE;
}

static void CB2_InitMailRead(void)
{
    do
    {
        if (MailReadBuildGraphics() == 1)
        {
            SetMainCallback2(CB2_MailRead);
            return;
        }
    } while (MenuHelpers_IsLinkActive() != 1);
}

static u8 *CopyAndSanitizeMailPlayerName(u8 *dest, u8 *src)
{
    u16 length;

    StringCopy(dest, src);
    ConvertInternationalPlayerName(dest);

    length = StringLength(dest);

    return dest + length;
}

static void BufferMailText(void)
{
    u16 i;
    u8 numWords;
    u8 *playerName;

    numWords = 0;
    for (i = 0; i < sMailRead->layout->numLines; i++)
    {
        ConvertEasyChatWordsToString(sMailRead->message[i], &sMailRead->mail->words[numWords], sMailRead->layout->lines[i].numEasyChatWords, 1);
        numWords += sMailRead->layout->lines[i].numEasyChatWords;
    }
    playerName = sMailRead->playerName;
    if (!sMailRead->international)
    {
        playerName = CopyAndSanitizeMailPlayerName(playerName, sMailRead->mail->playerName);
        StringCopy(playerName, gOtherText_From);
        sMailRead->signatureWidth = sMailRead->layout->signatureWidth - StringLength(sMailRead->playerName);

    }
    else
    {
        playerName = StringCopy(playerName, gOtherText_From);
        CopyAndSanitizeMailPlayerName(playerName, sMailRead->mail->playerName);
        sMailRead->signatureWidth = sMailRead->layout->signatureWidth;
    }
}

static void PrintMailText(void)
{
    u16 line;
    u8 xOffset;
    u8 y = 0;

    for (line = 0; line < sMailRead->layout->numLines; line++)
    {
        if (sMailRead->message[line][0] == EOS)
        {
            continue;
        }

        if (sMailRead->message[line][0] == CHAR_SPACE)
        {
            continue;
        }

        xOffset = sMailRead->layout->lines[line].xOffset;
        y += sMailRead->layout->lines[line].yOffset;
        Menu_PrintText(sMailRead->message[line], sMailRead->layout->wordsXPos + xOffset, sMailRead->layout->wordsYPos + y);
        y += 2;
    }

    Menu_PrintText(sMailRead->playerName, sMailRead->signatureWidth, sMailRead->layout->signatureYPos);
}

static void VBlankCB_MailRead(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_MailRead(void)
{
    if (sMailRead->iconType != ICON_TYPE_NONE)
    {
        AnimateSprites();
        BuildOamBuffer();
    }

    sMailRead->callback();
}

static void CB2_WaitForPaletteExitOnKeyPress(void)
{
    u8 isFading;

    isFading = UpdatePaletteFade();
    if (isFading == 0)
    {
        sMailRead->callback = CB2_ExitOnKeyPress;
    }
}

static void CB2_ExitOnKeyPress(void)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB(0, 0, 0));
        sMailRead->callback = CB2_ExitMailReadFreeVars;
    }
}

static void CB2_ExitMailReadFreeVars(void)
{
    if (!UpdatePaletteFade())
    {
        SetMainCallback2(sMailRead->exitCallback);
        switch (sMailRead->iconType)
        {
        case ICON_TYPE_DREAM:
        case ICON_TYPE_BEAD:
            FreeMonIconPalette(GetIconSpeciesNoPersonality(sMailRead->mail->species));
            FreeAndDestroyMonIconSprite(&gSprites[sMailRead->monIconSpriteId]);
            break;
        }
#if !DEBUG
        memset(sMailRead, 0, sizeof(*sMailRead));
#endif
        ResetPaletteFade();
    }
}

#if DEBUG

void Task_DebugMailChooseLayout(u8);
void Task_DebugMailMenuCallback(u8);
void CB2_InitDebugMailViewer(void);
void ShowDebugMailMainMenu(void);

void Task_DebugMailOpenSelected(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        sDebugMailReadActive = 1;
        Menu_EraseScreen();
        DestroyTask(taskId);
        if (gSaveBlock1.mail[gSpecialVar_0x8005].itemId == 0)
            ReadMail(&gSaveBlock1.mail[gSpecialVar_0x8005], CB2_InitDebugMailViewer, 0);
        else
            ReadMail(&gSaveBlock1.mail[gSpecialVar_0x8005], CB2_InitDebugMailViewer, 1);
    }
}

void Task_DebugMailChooseImage(u8 taskId)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_DOWN))
    {
        gSpecialVar_0x8006 = (gSpecialVar_0x8006 + 11) % 12;
        Menu_DrawStdWindowFrame(1, 1, 10, 4);
        Menu_PrintText(sDebugMailImageTypes[gSpecialVar_0x8006], 2, 2);
    }
    else if (JOY_NEW(DPAD_UP | DPAD_RIGHT))
    {
        gSpecialVar_0x8006 = (gSpecialVar_0x8006 + 1) % 12;
        Menu_DrawStdWindowFrame(1, 1, 10, 4);
        Menu_PrintText(sDebugMailImageTypes[gSpecialVar_0x8006], 2, 2);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_DebugMailOpenSelected;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        Menu_DisplayDialogueFrame();
        Menu_PrintText(sText_DebugMailLayoutPrompt, 4, 15);
        Menu_PrintText(sDebugMailLayouts[gSpecialVar_0x8004], 2, 2);
        gTasks[taskId].func = Task_DebugMailChooseLayout;
    }
}

void Task_DebugMailChooseLayout(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        gSpecialVar_0x8004 ^= 1;
        Menu_PrintText(sDebugMailLayouts[gSpecialVar_0x8004], 2, 2);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gMain.watchedKeysPressed)
        {
            gMain.watchedKeysPressed = FALSE;
            Menu_DisplayDialogueFrame();
            Menu_PrintText(sText_DebugMailLayoutPrompt, 4, 15);
            Menu_DrawStdWindowFrame(1, 1, 10, 4);
            Menu_PrintText(sDebugMailLayouts[gSpecialVar_0x8004], 2, 2);
        }
        else
        {
            Menu_DisplayDialogueFrame();
            Menu_PrintText(sText_DebugMailImageTypePrompt, 4, 15);
            Menu_DrawStdWindowFrame(1, 1, 10, 4);
            Menu_PrintText(sDebugMailImageTypes[gSpecialVar_0x8006], 2, 2);
            gTasks[taskId].func = Task_DebugMailChooseImage;
        }
    }
}

void Task_DebugMailChooseSpecies(u8 taskId)
{
    u8 speciesText[] = _("{STR_VAR_1}　{STR_VAR_2}");
    u8 mailText[] = DTR("メール{STR_VAR_1}　{STR_VAR_2}", "MAIL{STR_VAR_1} {STR_VAR_2}");
    bool8 selectionChanged = FALSE;

    if (JOY_NEW(A_BUTTON))
    {
        ConvertIntToDecimalStringN(gStringVar1, gSpecialVar_0x8005, 2, 1);
        StringCopy(gStringVar2, gSpeciesNames[gTasks[taskId].data[0]]);
        gSaveBlock1.mail[gSpecialVar_0x8005].species = gTasks[taskId].data[0];
        StringExpandPlaceholders(gStringVar4, mailText);
        Menu_DisplayDialogueFrame();
        Menu_PrintText(gStringVar4, 4, 15);
        DestroyTask(taskId);
        CreateTask(Task_DebugMailMenuCallback, 0);
        ShowDebugMailMainMenu();
        return;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        DestroyTask(taskId);
        Menu_EraseScreen();
        CreateTask(Task_DebugMailMenuCallback, 0);
        ShowDebugMailMainMenu();
        return;
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_DOWN))
    {
        gTasks[taskId].data[0] = (gTasks[taskId].data[0] + 0x19B) % 0x19C;
        selectionChanged = TRUE;
    }
    else if (JOY_NEW(DPAD_UP | DPAD_RIGHT))
    {
        gTasks[taskId].data[0] = (gTasks[taskId].data[0] + 1) % 0x19C;
        selectionChanged = TRUE;
    }

    if (selectionChanged)
    {
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[0], 2, 3);
        StringCopy(gStringVar2, gSpeciesNames[gTasks[taskId].data[0]]);
        StringExpandPlaceholders(gStringVar4, speciesText);
        Menu_DrawStdWindowFrame(0, 0, 12, 3);
        Menu_PrintText(gStringVar4, 1, 1);
    }
}

void Task_DebugMailPromptSpecies(u8 taskId)
{
    u8 string1[] = DTR("メール{STR_VAR_1}を　だれに　もたせる？", "Give MAIL {STR_VAR_1} to who?");
    u8 string2[] = _("000　{STR_VAR_1}");

    ConvertIntToDecimalStringN(gStringVar1, gSpecialVar_0x8005, 1, 1);
    StringExpandPlaceholders(gStringVar4, string1);
    Menu_DisplayDialogueFrame();
    Menu_PrintText(gStringVar4, 4, 15);

    StringCopy(gStringVar1, gSpeciesNames[0]);
    StringExpandPlaceholders(gStringVar4, string2);
    Menu_DrawStdWindowFrame(0, 0, 12, 3);
    Menu_PrintText(string2, 1, 1);  // This probably should have been gStringVar4

    gTasks[taskId].func = Task_DebugMailChooseSpecies;
}

u8 StartDebugMailViewer(void)
{
    CloseMenu();
    ScriptContext_Stop();
    LockPlayerFieldControls();
    sDebugMailReadActive = 0;
    gSpecialVar_0x8004 = gSpecialVar_0x8005 = gSpecialVar_0x8006 = 0;
    SetMainCallback2(CB2_InitDebugMailViewer);
    return 1;
}

void CB2_DebugMailViewer(void)
{
    UpdatePaletteFade();
    RunTasks();
}

void CB2_ExitDebugMailViewer(void)
{
    gMain.watchedKeysPressed = 0;
    gMain.watchedKeysMask = 0;
    gFieldCallback = FieldCB_DefaultWarpExit;
    SetMainCallback2(CB2_ReturnToField);
}

void Task_DebugMailMenuCallback(u8 taskId)
{
    if (gMenuCallback() == TRUE)
    {
        CloseMenu();
        DestroyTask(taskId);
    }
}

u8 DebugMailChooseSlotToView(void)
{
    s8 input = Menu_ProcessInput();

    switch (input)
    {
    case -1:
        Menu_EraseScreen();
        ShowDebugMailMainMenu();
        return 0;
    case -2:
        return 0;
    default:
        gSpecialVar_0x8005 = input;
        gSpecialVar_0x8006 %= 12;
        CreateTask(Task_DebugMailChooseLayout, 0);
        gMain.watchedKeysPressed = TRUE;  // huh?
        gMain.watchedKeysMask = 0;
        return 1;
    }
}

u8 DebugMailOpenEasyChat(void)
{
    if (!gPaletteFade.active)
    {
        gSpecialVar_0x8004 = 4;
        ShowEasyChatScreen();
        return 1;
    }
    return 0;
}

u8 DebugMailChooseSlotToRegister(void)
{
    u8 text[] = DTR("せいきの　データが　とうろくずみ　です\n"
                    "しんき　とうろく　できません",
                    "Normal data is registered.\n"
                    "Cannot register new data.");
    s8 input = Menu_ProcessInput();

    switch (input)
    {
    case -1:
        Menu_EraseScreen();
        break;
    case -2:
        return 0;
    default:
        gSpecialVar_0x8005 = input;
        if (IS_ITEM_MAIL(gSaveBlock1.mail[input].itemId))
        {
            Menu_EraseScreen();
            Menu_DisplayDialogueFrame();
            Menu_PrintText(text, 4, 15);
        }
        else
        {
            gSaveBlock1.mail[gSpecialVar_0x8005].itemId = 0xFFFF;
            StringCopy(gSaveBlock1.mail[gSpecialVar_0x8005].playerName, gSaveBlock2.playerName);
            *(u32 *)gSaveBlock1.mail[gSpecialVar_0x8005].trainerId = *(u32 *)gSaveBlock2.playerTrainerId;
            gMenuCallback = DebugMailOpenEasyChat;
            return 0;
        }
        break;
    }
    ShowDebugMailMainMenu();
    return 0;
}

u8 DebugMailChooseSlotToSetSpecies(void)
{
    u8 text[] = DTR("せいきに　とうろくされたメールデータは\n"
                    "へんこう　できません",
                    "Regularly, registered MAIL data\n"
                    "cannot be changed.");
    s8 input = Menu_ProcessInput();

    switch (input)
    {
    case -1:
        Menu_EraseScreen();
        ShowDebugMailMainMenu();
        return 0;
    case -2:
        return 0;
    default:
        gSpecialVar_0x8005 = input;
        if (IS_ITEM_MAIL(gSaveBlock1.mail[input].itemId))
        {
            Menu_EraseScreen();
            Menu_DisplayDialogueFrame();
            Menu_PrintText(text, 4, 15);
            ShowDebugMailMainMenu();
            return 0;
        }
        else
        {
            CreateTask(Task_DebugMailPromptSpecies, 0);
            return 1;
        }
    }
}

u8 DebugMailChooseSlotToUnregister(void)
{
    const u8 text1[] = DTR("メール{STR_VAR_1}　とうろく　かいじょ", "UNREGISTER MAIL{STR_VAR_1}");
    const u8 text2[] = DTR("デバッグメニューから　とうろくした\n"
                           "メール　いがいは　けせません",
                           "You cannot delete REGISTERED MAIL\n"
                           "from the DEBUG MENU");
    s8 input = Menu_ProcessInput();

    switch (input)
    {
    case -1:
        Menu_EraseScreen();
        ShowDebugMailMainMenu();
        return 0;
    case -2:
        return 0;
    default:
        gSpecialVar_0x8005 = input;
        if (gSaveBlock1.mail[gSpecialVar_0x8005].itemId != 0xFFFF
         && gSaveBlock1.mail[gSpecialVar_0x8005].itemId != 0)
        {
            Menu_EraseScreen();
            Menu_DisplayDialogueFrame();
            Menu_PrintText(text2, 4, 15);
            ShowDebugMailMainMenu();
            return 0;
        }
        else
        {
            s8 i;

            gSaveBlock1.mail[gSpecialVar_0x8005].itemId = 0;
            gSaveBlock1.mail[gSpecialVar_0x8005].species = 0;
            gSaveBlock1.mail[gSpecialVar_0x8005].playerName[0] = EOS;
            *(u32 *)gSaveBlock1.mail[gSpecialVar_0x8005].trainerId = 0;
            for (i = 0; i < 9; i++)
                gSaveBlock1.mail[gSpecialVar_0x8005].words[i] = 0xFFFF;
            ConvertIntToDecimalStringN(gStringVar1, gSpecialVar_0x8005, 1, 1);
            StringExpandPlaceholders(gStringVar4, text1);
            Menu_EraseScreen();
            Menu_DisplayDialogueFrame();
            Menu_PrintText(gStringVar4, 4, 15);
            ShowDebugMailMainMenu();
            return 0;
        }
    }
}

void ShowDebugMailSlotList(u8 a)
{
    u8 string[] = DTR("メール{STR_VAR_1}　{STR_VAR_2}　{STR_VAR_3}", "MAIL{STR_VAR_1} {STR_VAR_2} {STR_VAR_3}");
    u8 buffer[9][20];
    struct MenuAction menuActions[9];
    u8 i;

    for (i = 0; i < 9; i++)
    {
        ConvertIntToDecimalStringN(gStringVar1, i, 1, 1);
        if (gSaveBlock1.mail[i].itemId == 0xFFFF)
        {
            ConvertIntToDecimalStringN(gStringVar2, 1, 1, 1);
        }
        else if (IS_ITEM_MAIL(gSaveBlock1.mail[i].itemId))
        {
            ConvertIntToDecimalStringN(gStringVar2, 2, 1, 1);
        }
        else
        {
            ConvertIntToDecimalStringN(gStringVar2, 0, 1, 1);
            gSaveBlock1.mail[i].itemId = 0;
        }
        StringCopy(gStringVar3, gSpeciesNames[gSaveBlock1.mail[i].species]);
        StringExpandPlaceholders(buffer[i], string);
        menuActions[i].text = buffer[i];
        menuActions[i].func = NULL;
    }
    Menu_DrawStdWindowFrame(0, 0, 16, 19);
    Menu_PrintItems(2, 1, 9, menuActions);
    InitMenu(0, 1, 1, 9, 0, 15);
    switch (a)
    {
    case 0:
        gMenuCallback = DebugMailChooseSlotToView;
        break;
    case 1:
        gMenuCallback = DebugMailChooseSlotToRegister;
        break;
    case 2:
        gMenuCallback = DebugMailChooseSlotToSetSpecies;
        break;
    case 3:
        gMenuCallback = DebugMailChooseSlotToUnregister;
        break;
    }
}

u8 DebugMailHandleMainMenuInput(void)
{
    s8 input = Menu_ProcessInput();

    switch (input)
    {
    case -1:
        SetMainCallback2(CB2_ExitDebugMailViewer);
        return 1;
    case -2:
        return 0;
    }

    switch (input)
    {
    case 0:
        ShowDebugMailSlotList(input);
        return 0;
    case 1:
        ShowDebugMailSlotList(input);
        return 0;
    case 2:
        ShowDebugMailSlotList(input);
        return 0;
    case 3:
        ShowDebugMailSlotList(input);
        return 0;
    }

    return 1;
}

const struct MenuAction sDebugMailMenuActions[] =
{
    {sText_DebugMailActionView, NULL},
    {sText_DebugMailActionRegister, NULL},
    {sText_DebugMailActionGive, NULL},
    {sText_DebugMailActionDelete, NULL},
};

void ShowDebugMailMainMenu(void)
{
    Menu_DrawStdWindowFrame(0, 0, 9, 9);
    Menu_PrintItems(2, 1, 4, sDebugMailMenuActions);
    InitMenu(0, 1, 1, 4, 0, 8);
    gMenuCallback = DebugMailHandleMainMenuInput;
}

void CB2_InitDebugMailViewer(void)
{
    switch (gMain.state)
    {
    case 0:
        if (sDebugMailReadActive != 0)
        {
            gMain.state = 2;
            sDebugMailReadActive = 0;
            return;
        }
        else
        {
            ScanlineEffect_Stop();
            ResetPaletteFade();
            SetVBlankCallback(VBlankCB_MailRead);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
        break;
    case 1:
        if (UpdatePaletteFade())
            return;
        break;
    case 2:
        SetVBlankCallback(NULL);
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG2HOFS = 0;
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 0;
        break;
    case 3:
        ResetSpriteData();
        ResetTasks();
        FreeAllSpritePalettes();
        break;
    case 4:
        Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
        InitMenuWindow(&gMenuTextWindowTemplate);
        Menu_EraseScreen();
        break;
    case 5:
        LoadPalette(sMailGraphics[0].palette, 0, 32);
        LZ77UnCompVram(sMailGraphics[0].tiles, (void *)VRAM);
        DmaFill16(3, 1, (void *)(VRAM + 0x4000), 0x500);
        break;
    case 6:
        REG_BG0CNT = 0x9F08;
        REG_BG1CNT = 0x0801;
        REG_BLDCNT = 0;
        REG_DISPCNT = 0x0340;
        ShowDebugMailMainMenu();
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_MailRead);
        break;
    case 7:
        if (!UpdatePaletteFade())
            return;
        break;
    case 8:
        CreateTask(Task_DebugMailMenuCallback, 0);
        SetMainCallback2(CB2_DebugMailViewer);
        break;
    default:
        return;
    }
    gMain.state++;
}

#endif
