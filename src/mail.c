#include "global.h"
#include "mail.h"
#include "data.h"
#include "easy_chat.h"
#include "constants/items.h"
#include "constants/species.h"
#include "field_screen_effect.h"
#include "graphics.h"
#include "menu.h"
#include "menu_helpers.h"
#include "name_string_util.h"
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
    u8 unk_0_0:2;
    u8 numEasyChatWords:2;
    u8 xOffset:4;
};

struct MailLayout
{
    u8 numLines;
    u8 signatureYPos;
    u8 var2;
    u8 wordsYPos:4;
    u8 wordsXPos:4;
    const struct MailLineLayout *lines;
};

struct MailRead
{
    /* 0x00*/ u8 message[8][27];
    /* 0xD8*/ u8 playerName[20];
    /* 0xEC*/ MainCallback exitCallback;
    /* 0xF0*/ MainCallback callback;
    /* 0xF4*/ struct Mail *mail;
    /* 0xF8*/ u8 hasText;
    /* 0xF9*/ u8 varF9;
    /* 0xFA*/ u8 mailType;
    /* 0xFB*/ u8 iconType;
    /* 0xFC*/ u8 monIconSpriteId;
    u8 padFD[1];
    /* 0xFE*/ u8 varFE;
    /* 0xFF*/ u8 language;
    /*0x100*/ u8 international;
    u8 pad101[3];
    /*0x104*/ u8 * (*unusedParserSingle)(u8 *dst, u16 word);
    /*0x108*/ u8 * (*unusedParserMultiple)(u8 *dst, u16* words, u16 arg2, u16 arg3);
    /*0x10C*/ const struct MailLayout *layout;
};

struct MailGraphics
{
    const u16 *palette;
    const u8 *tiles;
    const u8 *tileMap;
    u16 var0C;
    u16 var0E;
    u16 textColor;
    u16 textShadow;
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
        .var0C = 0x2C0,
        .var0E = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Harbor,
        .tiles = gMailTiles_Harbor,
        .tileMap = gMailTilemap_Harbor,
        .var0C = 0x2E0,
        .var0E = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Glitter,
        .tiles = gMailTiles_Glitter,
        .tileMap = gMailTilemap_Glitter,
        .var0C = 0x400,
        .var0E = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Mech,
        .tiles = gMailTiles_Mech,
        .tileMap = gMailTilemap_Mech,
        .var0C = 0x1E0,
        .var0E = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Wood,
        .tiles = gMailTiles_Wood,
        .tileMap = gMailTilemap_Wood,
        .var0C = 0x2E0,
        .var0E = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Wave,
        .tiles = gMailTiles_Wave,
        .tileMap = gMailTilemap_Wave,
        .var0C = 0x300,
        .var0E = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Bead,
        .tiles = gMailTiles_Bead,
        .tileMap = gMailTilemap_Bead,
        .var0C = 0x140,
        .var0E = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Shadow,
        .tiles = gMailTiles_Shadow,
        .tileMap = gMailTilemap_Shadow,
        .var0C = 0x300,
        .var0E = 0,
        .textColor = 0x7FFF,
        .textShadow = 0x4631,
    },
    {
        .palette = gMailPalette_Tropic,
        .tiles = gMailTiles_Tropic,
        .tileMap = gMailTilemap_Tropic,
        .var0C = 0x220,
        .var0E = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Dream,
        .tiles = gMailTiles_Dream,
        .tileMap = gMailTilemap_Dream,
        .var0C = 0x340,
        .var0E = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Fab,
        .tiles = gMailTiles_Fab,
        .tileMap = gMailTilemap_Fab,
        .var0C = 0x2A0,
        .var0E = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
    {
        .palette = gMailPalette_Retro,
        .tiles = gMailTiles_Retro,
        .tileMap = gMailTilemap_Retro,
        .var0C = 0x520,
        .var0E = 0,
        .textColor = 0x294A,
        .textShadow = 0x6739,
    },
};

static const struct MailLineLayout sLineLayouts_Wide[] =
{
    {0, 3, 0},
    {0, 3, 0},
    {0, 3, 0},
};

static const struct MailLayout sMailLayouts_Wide[] =
{
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
    {3, 16, 27, 4, 3, sLineLayouts_Wide},
    {3, 15, 27, 4, 3, sLineLayouts_Wide},
};

static const struct MailLineLayout sLineLayouts_Tall[] =
{
    {0, 2, 0},
    {0, 2, 0},
    {0, 2, 0},
    {0, 2, 0},
    {0, 1, 0},
};

static const struct MailLayout sMailLayouts_Tall[] =
{
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
    {5, 15, 16, 4, 3, sLineLayouts_Tall},
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
    {5, 15, 16, 4, 3, sLineLayouts_Tall},
    {5, 15, 16, 4, 3, sLineLayouts_Tall},
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
    {5, 16, 15, 4, 3, sLineLayouts_Tall},
    {5, 15, 15, 4, 3, sLineLayouts_Tall},
};

#if DEBUG

const u8 Str_8411540[] = DTR("レイアウトを　にほんごで　ひょうじ？", "Display using JAPANESE layout?");
const u8 Str_8411553[] = DTR("にほんご　", "JAPANESE");
const u8 Str_8411559[] = DTR("かいがい　", "OVERSEAS");

const u8 *const _8411560[] =
{
    Str_8411553,
    Str_8411559,
};

const u8 Str_8411568[] = DTR("イメージ　タイプを　えらんでね", "Choose an image type");
const u8 Str_8411578[] = _("{STR_VAR_1}");
const u8 Str_841157B[] = DTR("オレンジ　", "ORANGE ");
const u8 Str_8411581[] = DTR("ハーバー　", "HARBOR ");
const u8 Str_8411587[] = DTR("キラキラ　", "GLITTER");
const u8 Str_841158D[] = DTR("メカニカル", "MECH   ");
const u8 Str_8411593[] = DTR("ウッディー", "WOOD   ");
const u8 Str_8411599[] = DTR("クロス　　", "WAVE   ");
const u8 Str_841159F[] = DTR("トレジャー", "BEAD   ");
const u8 Str_84115A5[] = DTR("シャドウ　", "SHADOW ");
const u8 Str_84115AB[] = DTR("トロピカル", "TROPIC ");
const u8 Str_84115B1[] = DTR("ドリーム　", "DREAM  ");
const u8 Str_84115B7[] = DTR("ミラクル　", "FAB    ");
const u8 Str_84115BD[] = DTR("レトロ　　", "RETRO  ");

const u8 *const _84115C4[] =
{
    Str_841157B,
    Str_8411581,
    Str_8411587,
    Str_841158D,
    Str_8411593,
    Str_8411599,
    Str_841159F,
    Str_84115A5,
    Str_84115AB,
    Str_84115B1,
    Str_84115B7,
    Str_84115BD,
};

const u8 Str_84115F4[] = DTR("メールをみる", "View MAIL");
const u8 Str_84115FB[] = DTR("メールとうろく", "Register MAIL");
const u8 Str_8411603[] = DTR("もたせる", "Give MAIL");
const u8 Str_8411608[] = DTR("メールをけす", "Delete MAIL");

#endif

// XXX: what is this?
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
static u8 gUnknown_Debug_0300079C;
#endif

void ReadMail(struct Mail *mail, MainCallback exitCallback, bool8 hasText)
{
    u16 buffer[2];
    u16 species;

#if DEBUG
    if (gUnknown_Debug_0300079C != 0)
    {
        sMailRead->language = GAME_LANGUAGE;
        sMailRead->international = gSpecialVar_0x8004;
        sMailRead->unusedParserSingle = CopyEasyChatWord;
        sMailRead->unusedParserMultiple = ConvertEasyChatWordsToString;
        sMailRead->mailType = gSpecialVar_0x8006;
    }
    else
#endif
    {
        sMailRead->language = GAME_LANGUAGE;
        sMailRead->international = 1;
        sMailRead->unusedParserSingle = CopyEasyChatWord;
        sMailRead->unusedParserMultiple = ConvertEasyChatWordsToString;
        if (IS_ITEM_MAIL(mail->itemId))
        {
            sMailRead->mailType = mail->itemId - 0x79;
        }
        else
        {
            sMailRead->mailType = 0;
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

    species = MailSpeciesToSpecies(mail->species, buffer);
    if (species >= 1 && species <= 411)
    {
        switch (sMailRead->mailType)
        {
        case 6:
            sMailRead->iconType = 1;
            break;
        case 9:
            sMailRead->iconType = 2;
            break;
        default:
            sMailRead->iconType = 0;
            break;
        }
    }
    else
    {
        sMailRead->iconType = 0;
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
        Text_LoadWindowTemplate(&gWindowTemplate_81E6DFC);
        RETURN_UP_STATE;

    case 7:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E6DFC);
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
        if (sMailRead->hasText != 0)
        {
            BufferMailText();
        }
        RETURN_UP_STATE;

    case 15:
        if (sMailRead->hasText != 0)
        {
            PrintMailText();
        }

        SetVBlankCallback(VBlankCB_MailRead);
        gPaletteFade.bufferTransferDisabled = 1;
        RETURN_UP_STATE;

    case 16:
    {
        u16 icon;

        icon = GetIconSpeciesNoPersonality(sMailRead->mail->species);

        switch (sMailRead->iconType)
        {
        case 1:
            LoadMonIconPalette(icon);
            sMailRead->monIconSpriteId = CreateMonIconNoPersonality(icon, SpriteCallbackDummy, 96, 128, 0);
            break;

        case 2:
            LoadMonIconPalette(icon);
            sMailRead->monIconSpriteId = CreateMonIconNoPersonality(icon, SpriteCallbackDummy, 40, 128, 0);
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
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gPaletteFade.bufferTransferDisabled = 0;
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

static u8 *sub_80F8D7C(u8 *dest, u8 *src)
{
    u16 length;

    StringCopy(dest, src);
    SanitizeNameString(dest);

    length = StringLength(dest);

    return dest + length;
}

static void BufferMailText(void)
{
    u16 i;
    u8 numWords;
    u8 *ptr;

    numWords = 0;
    for (i = 0; i < sMailRead->layout->numLines; i++)
    {
        ConvertEasyChatWordsToString(sMailRead->message[i], &sMailRead->mail->words[numWords], sMailRead->layout->lines[i].numEasyChatWords, 1);
        numWords += sMailRead->layout->lines[i].numEasyChatWords;
    }
    ptr = sMailRead->playerName;
    if (sMailRead->international == 0)
    {
        ptr = sub_80F8D7C(ptr, sMailRead->mail->playerName);
        StringCopy(ptr, gOtherText_From);
        sMailRead->varF9 = sMailRead->layout->var2 - StringLength(sMailRead->playerName);

    }
    else
    {
        ptr = StringCopy(ptr, gOtherText_From);
        sub_80F8D7C(ptr, sMailRead->mail->playerName);
        sMailRead->varF9 = sMailRead->layout->var2;
    }
}

static void PrintMailText(void)
{
    u16 i;
    u8 x;
    u8 y = 0;

    for (i = 0; i < sMailRead->layout->numLines; i++)
    {
        if (sMailRead->message[i][0] == 0xFF)
        {
            continue;
        }

        if (sMailRead->message[i][0] == 0x00)
        {
            continue;
        }

        x = sMailRead->layout->lines[i].xOffset;
        y += sMailRead->layout->lines[i].unk_0_0;
        Menu_PrintText(sMailRead->message[i], sMailRead->layout->wordsXPos + x, sMailRead->layout->wordsYPos + y);
        y += 2;
    }

    Menu_PrintText(sMailRead->playerName, sMailRead->varF9, sMailRead->layout->signatureYPos);
}

static void VBlankCB_MailRead(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_MailRead(void)
{
    if (sMailRead->iconType != 0)
    {
        AnimateSprites();
        BuildOamBuffer();
    }

    sMailRead->callback();
}

static void CB2_WaitForPaletteExitOnKeyPress(void)
{
    u8 local0;

    local0 = UpdatePaletteFade();
    if (local0 == 0)
    {
        sMailRead->callback = CB2_ExitOnKeyPress;
    }
}

static void CB2_ExitOnKeyPress(void)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
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
        case 2:
        case 1:
            FreeMonIconPalette(GetIconSpeciesNoPersonality(sMailRead->mail->species));
            FreeAndDestroyMonIconSprite(&gSprites[sMailRead->monIconSpriteId]);
            break;
        }
#if !DEBUG
        memset(sMailRead, 0, sizeof(struct MailRead));
#endif
        ResetPaletteFade();
    }
}

#if DEBUG

void debug_sub_810CA7C(u8);
void debug_sub_810CE1C(u8);
void debug_sub_810D388(void);
void debug_sub_810D340(void);

void debug_sub_810C910(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gUnknown_Debug_0300079C = 1;
        Menu_EraseScreen();
        DestroyTask(taskId);
        if (gSaveBlock1.mail[gSpecialVar_0x8005].itemId == 0)
            ReadMail(&gSaveBlock1.mail[gSpecialVar_0x8005], debug_sub_810D388, 0);
        else
            ReadMail(&gSaveBlock1.mail[gSpecialVar_0x8005], debug_sub_810D388, 1);
    }
}

void debug_sub_810C990(u8 taskId)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_DOWN))
    {
        gSpecialVar_0x8006 = (gSpecialVar_0x8006 + 11) % 12;
        Menu_DrawStdWindowFrame(1, 1, 10, 4);
        Menu_PrintText(_84115C4[gSpecialVar_0x8006], 2, 2);
    }
    else if (JOY_NEW(DPAD_UP | DPAD_RIGHT))
    {
        gSpecialVar_0x8006 = (gSpecialVar_0x8006 + 1) % 12;
        Menu_DrawStdWindowFrame(1, 1, 10, 4);
        Menu_PrintText(_84115C4[gSpecialVar_0x8006], 2, 2);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = debug_sub_810C910;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        Menu_DisplayDialogueFrame();
        Menu_PrintText(Str_8411540, 4, 15);
        Menu_PrintText(_8411560[gSpecialVar_0x8004], 2, 2);
        gTasks[taskId].func = debug_sub_810CA7C;
    }
}

void debug_sub_810CA7C(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        gSpecialVar_0x8004 ^= 1;
        Menu_PrintText(_8411560[gSpecialVar_0x8004], 2, 2);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gMain.watchedKeysPressed)
        {
            gMain.watchedKeysPressed = FALSE;
            Menu_DisplayDialogueFrame();
            Menu_PrintText(Str_8411540, 4, 15);
            Menu_DrawStdWindowFrame(1, 1, 10, 4);
            Menu_PrintText(_8411560[gSpecialVar_0x8004], 2, 2);
        }
        else
        {
            Menu_DisplayDialogueFrame();
            Menu_PrintText(Str_8411568, 4, 15);
            Menu_DrawStdWindowFrame(1, 1, 10, 4);
            Menu_PrintText(_84115C4[gSpecialVar_0x8006], 2, 2);
            gTasks[taskId].func = debug_sub_810C990;
        }
    }
}

void debug_sub_810CB50(u8 taskId)
{
    u8 sp0[] = _("{STR_VAR_1}　{STR_VAR_2}");
    u8 sp8[] = DTR("メール{STR_VAR_1}　{STR_VAR_2}", "MAIL{STR_VAR_1} {STR_VAR_2}");
    bool8 r2 = FALSE;

    if (JOY_NEW(A_BUTTON))
    {
        ConvertIntToDecimalStringN(gStringVar1, gSpecialVar_0x8005, 2, 1);
        StringCopy(gStringVar2, gSpeciesNames[gTasks[taskId].data[0]]);
        gSaveBlock1.mail[gSpecialVar_0x8005].species = gTasks[taskId].data[0];
        StringExpandPlaceholders(gStringVar4, sp8);
        Menu_DisplayDialogueFrame();
        Menu_PrintText(gStringVar4, 4, 15);
        DestroyTask(taskId);
        CreateTask(debug_sub_810CE1C, 0);
        debug_sub_810D340();
        return;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        DestroyTask(taskId);
        Menu_EraseScreen();
        CreateTask(debug_sub_810CE1C, 0);
        debug_sub_810D340();
        return;
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_DOWN))
    {
        gTasks[taskId].data[0] = (gTasks[taskId].data[0] + 0x19B) % 0x19C;
        r2 = TRUE;
    }
    else if (JOY_NEW(DPAD_UP | DPAD_RIGHT))
    {
        gTasks[taskId].data[0] = (gTasks[taskId].data[0] + 1) % 0x19C;
        r2 = TRUE;
    }

    if (r2)
    {
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[0], 2, 3);
        StringCopy(gStringVar2, gSpeciesNames[gTasks[taskId].data[0]]);
        StringExpandPlaceholders(gStringVar4, sp0);
        Menu_DrawStdWindowFrame(0, 0, 12, 3);
        Menu_PrintText(gStringVar4, 1, 1);
    }
}

void debug_sub_810CCEC(u8 taskId)
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

    gTasks[taskId].func = debug_sub_810CB50;
}

u8 debug_sub_810CD9C(void)
{
    CloseMenu();
    ScriptContext_Stop();
    LockPlayerFieldControls();
    gUnknown_Debug_0300079C = 0;
    gSpecialVar_0x8004 = gSpecialVar_0x8005 = gSpecialVar_0x8006 = 0;
    SetMainCallback2(debug_sub_810D388);
    return 1;
}

void debug_sub_810CDE0(void)
{
    UpdatePaletteFade();
    RunTasks();
}

void debug_sub_810CDF0(void)
{
    gMain.watchedKeysPressed = 0;
    gMain.watchedKeysMask = 0;
    gFieldCallback = FieldCB_DefaultWarpExit;
    SetMainCallback2(CB2_ReturnToField);
}

void debug_sub_810CE1C(u8 taskId)
{
    if (gMenuCallback() == TRUE)
    {
        CloseMenu();
        DestroyTask(taskId);
    }
}

u8 debug_sub_810CE48(void)
{
    s8 input = Menu_ProcessInput();

    switch (input)
    {
    case -1:
        Menu_EraseScreen();
        debug_sub_810D340();
        return 0;
    case -2:
        return 0;
    default:
        gSpecialVar_0x8005 = input;
        gSpecialVar_0x8006 %= 12;
        CreateTask(debug_sub_810CA7C, 0);
        gMain.watchedKeysPressed = TRUE;  // huh?
        gMain.watchedKeysMask = 0;
        return 1;
    }
}

u8 debug_sub_810CEA4(void)
{
    if (!gPaletteFade.active)
    {
        gSpecialVar_0x8004 = 4;
        ShowEasyChatScreen();
        return 1;
    }
    return 0;
}

u8 debug_sub_810CED0(void)
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
            gMenuCallback = debug_sub_810CEA4;
            return 0;
        }
        break;
    }
    debug_sub_810D340();
    return 0;
}

u8 debug_sub_810CFA4(void)
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
        debug_sub_810D340();
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
            debug_sub_810D340();
            return 0;
        }
        else
        {
            CreateTask(debug_sub_810CCEC, 0);
            return 1;
        }
    }
}

u8 debug_sub_810D030(void)
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
        debug_sub_810D340();
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
            debug_sub_810D340();
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
            debug_sub_810D340();
            return 0;
        }
    }
}

void debug_sub_810D174(u8 a)
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
        gMenuCallback = debug_sub_810CE48;
        break;
    case 1:
        gMenuCallback = debug_sub_810CED0;
        break;
    case 2:
        gMenuCallback = debug_sub_810CFA4;
        break;
    case 3:
        gMenuCallback = debug_sub_810D030;
        break;
    }
}

u8 debug_sub_810D2F4(void)
{
    s8 input = Menu_ProcessInput();

    switch (input)
    {
    case -1:
        SetMainCallback2(debug_sub_810CDF0);
        return 1;
    case -2:
        return 0;
    }

    switch (input)
    {
    case 0:
        debug_sub_810D174(input);
        return 0;
    case 1:
        debug_sub_810D174(input);
        return 0;
    case 2:
        debug_sub_810D174(input);
        return 0;
    case 3:
        debug_sub_810D174(input);
        return 0;
    }

    return 1;
}

const struct MenuAction _84116BC[] =
{
    {Str_84115F4, NULL},
    {Str_84115FB, NULL},
    {Str_8411603, NULL},
    {Str_8411608, NULL},
};

void debug_sub_810D340(void)
{
    Menu_DrawStdWindowFrame(0, 0, 9, 9);
    Menu_PrintItems(2, 1, 4, _84116BC);
    InitMenu(0, 1, 1, 4, 0, 8);
    gMenuCallback = debug_sub_810D2F4;
}

void debug_sub_810D388(void)
{
    switch (gMain.state)
    {
    case 0:
        if (gUnknown_Debug_0300079C != 0)
        {
            gMain.state = 2;
            gUnknown_Debug_0300079C = 0;
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
        debug_sub_810D340();
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_MailRead);
        break;
    case 7:
        if (!UpdatePaletteFade())
            return;
        break;
    case 8:
        CreateTask(debug_sub_810CE1C, 0);
        SetMainCallback2(debug_sub_810CDE0);
        break;
    default:
        return;
    }
    gMain.state++;
}

#endif
