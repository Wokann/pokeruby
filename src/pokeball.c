#include "global.h"
#include "gba/m4a_internal.h"
#include "battle.h"
#include "battle_anim_throw.h"
#include "decompress.h"
#include "graphics.h"
#include "m4a.h"
#include "main.h"
#include "pokeball.h"
#include "pokemon.h"
#include "battle_anim.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "util.h"
#include "ewram.h"

extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u16 gBattleTypeFlags;
extern u8 gBattlerTarget;
extern u8 gActiveBattler;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerSpriteIds[];
extern u8 gDoingBattleAnim;
extern u8 gHealthboxSpriteIds[];

#define GFX_TAG_POKEBALL    55000
#define GFX_TAG_GREATBALL   55001
#define GFX_TAG_SAFARIBALL  55002
#define GFX_TAG_ULTRABALL   55003
#define GFX_TAG_MASTERBALL  55004
#define GFX_TAG_NETBALL     55005
#define GFX_TAG_DIVEBALL    55006
#define GFX_TAG_NESTBALL    55007
#define GFX_TAG_REPEATBALL  55008
#define GFX_TAG_TIMERBALL   55009
#define GFX_TAG_LUXURYBALL  55010
#define GFX_TAG_PREMIERBALL 55011

static const struct CompressedSpriteSheet sBallSpriteSheets[] =
{
    {gInterfaceGfx_PokeBall,    384, GFX_TAG_POKEBALL},
    {gInterfaceGfx_GreatBall,   384, GFX_TAG_GREATBALL},
    {gInterfaceGfx_SafariBall,  384, GFX_TAG_SAFARIBALL},
    {gInterfaceGfx_UltraBall,   384, GFX_TAG_ULTRABALL},
    {gInterfaceGfx_MasterBall,  384, GFX_TAG_MASTERBALL},
    {gInterfaceGfx_NetBall,     384, GFX_TAG_NETBALL},
    {gInterfaceGfx_DiveBall,    384, GFX_TAG_DIVEBALL},
    {gInterfaceGfx_NestBall,    384, GFX_TAG_NESTBALL},
    {gInterfaceGfx_RepeatBall,  384, GFX_TAG_REPEATBALL},
    {gInterfaceGfx_TimerBall,   384, GFX_TAG_TIMERBALL},
    {gInterfaceGfx_LuxuryBall,  384, GFX_TAG_LUXURYBALL},
    {gInterfaceGfx_PremierBall, 384, GFX_TAG_PREMIERBALL},
};

static const struct CompressedSpritePalette sBallSpritePalettes[] =
{
    {gInterfacePal_PokeBall,    GFX_TAG_POKEBALL},
    {gInterfacePal_GreatBall,   GFX_TAG_GREATBALL},
    {gInterfacePal_SafariBall,  GFX_TAG_SAFARIBALL},
    {gInterfacePal_UltraBall,   GFX_TAG_ULTRABALL},
    {gInterfacePal_MasterBall,  GFX_TAG_MASTERBALL},
    {gInterfacePal_NetBall,     GFX_TAG_NETBALL},
    {gInterfacePal_DiveBall,    GFX_TAG_DIVEBALL},
    {gInterfacePal_NestBall,    GFX_TAG_NESTBALL},
    {gInterfacePal_RepeatBall,  GFX_TAG_REPEATBALL},
    {gInterfacePal_TimerBall,   GFX_TAG_TIMERBALL},
    {gInterfacePal_LuxuryBall,  GFX_TAG_LUXURYBALL},
    {gInterfacePal_PremierBall, GFX_TAG_PREMIERBALL},
};

static const struct OamData sBallOamData =
{
    .y = 0,
    .affineMode = 3,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sBallAnimSeq3[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq5[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq4[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq6[] =
{
    ANIMCMD_FRAME(12, 1),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq0[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sBallAnimSeq1[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sBallAnimSeq2[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sBallAnimSequences[] =
{
    sBallAnimSeq0,
    sBallAnimSeq1,
    sBallAnimSeq2,

    // unused?
    sBallAnimSeq3,
    sBallAnimSeq4,
    sBallAnimSeq5,
    sBallAnimSeq6,
};

static const union AffineAnimCmd sBallAffineAnimSeq0[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 0, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sBallAffineAnimSeq1[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -3, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sBallAffineAnimSeq2[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 3, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sBallAffineAnimSeq3[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sBallAffineAnimSeq4[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 25, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd *const sBallAffineAnimSequences[] =
{
    sBallAffineAnimSeq0,
    sBallAffineAnimSeq1,
    sBallAffineAnimSeq2,
    sBallAffineAnimSeq3,
    sBallAffineAnimSeq4,
};

static void SpriteCB_BallThrow(struct Sprite *sprite);
const struct SpriteTemplate gBallSpriteTemplates[] =
{
    {
        .tileTag = GFX_TAG_POKEBALL,
        .paletteTag = GFX_TAG_POKEBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_GREATBALL,
        .paletteTag = GFX_TAG_GREATBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_SAFARIBALL,
        .paletteTag = GFX_TAG_SAFARIBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_ULTRABALL,
        .paletteTag = GFX_TAG_ULTRABALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_MASTERBALL,
        .paletteTag = GFX_TAG_MASTERBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_NETBALL,
        .paletteTag = GFX_TAG_NETBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_DIVEBALL,
        .paletteTag = GFX_TAG_DIVEBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_NESTBALL,
        .paletteTag = GFX_TAG_NESTBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_REPEATBALL,
        .paletteTag = GFX_TAG_REPEATBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_TIMERBALL,
        .paletteTag = GFX_TAG_TIMERBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_LUXURYBALL,
        .paletteTag = GFX_TAG_LUXURYBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_PREMIERBALL,
        .paletteTag = GFX_TAG_PREMIERBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_BallThrow,
    },
};

extern void InitAnimArcTranslation();
extern bool8 TranslateAnimHorizontalArc(struct Sprite *);

static void Task_DoPokeballSendOutAnim(u8);
static void SpriteCB_BallThrow_ReachMon(struct Sprite *);
static void SpriteCB_BallThrow_StartShrinkMon(struct Sprite *);
static void SpriteCB_BallThrow_ShrinkMon(struct Sprite *);
static void SpriteCB_BallThrow_Close(struct Sprite *);
static void SpriteCB_BallThrow_FallToGround(struct Sprite *);
static void SpriteCB_BallThrow_StartShakes(struct Sprite *);
static void SpriteCB_BallThrow_Shake(struct Sprite *);
static void SpriteCB_ReleaseMonFromBall(struct Sprite *);
static void SpriteCB_BallThrow_StartCaptureMon(struct Sprite *);
static void HandleBallAnimEnd(struct Sprite *);
static void SpriteCB_BallThrow_CaptureMon(struct Sprite *);
static void SpriteCB_PlayerMonSendOut_1(struct Sprite *);
static void SpriteCB_PlayerMonSendOut_2(struct Sprite *);
static void SpriteCB_ReleaseMon2FromBall(struct Sprite *);
static void SpriteCB_OpponentMonSendOut(struct Sprite *);
static void SpriteCB_PokeballReleaseMon(struct Sprite *);
static void SpriteCB_ReleasedMonFlyOut(struct Sprite *);
static void SpriteCB_TradePokeball(struct Sprite *);
static void SpriteCB_TradePokeballSendOff(struct Sprite *);
static void SpriteCB_TradePokeballEnd(struct Sprite *);
static void SpriteCB_HealthboxSlideInDelayed(struct Sprite *);
static void SpriteCB_HealthboxSlideIn(struct Sprite *);
static void SpriteCB_HitAnimHealthoxEffect(struct Sprite *);
static u16 GetBattlerPokeballItemId(u8);

#define tFrames          data[0]
#define tPan             data[1]
#define tThrowId         data[2]
#define tBattler         data[3]
#define tOpponentBattler data[4]

u8 DoPokeballSendOutAnimation(u16 pan, u8 kindOfThrow)
{
    u8 taskId;

    gDoingBattleAnim = TRUE;
    gBattleHealthBoxInfo[gActiveBattler].ballAnimActive = TRUE;
    taskId = CreateTask(Task_DoPokeballSendOutAnim, 5);
    gTasks[taskId].tPan = pan;
    gTasks[taskId].tThrowId = kindOfThrow;
    gTasks[taskId].tBattler = gActiveBattler;
    return 0;
}

#define sBattler data[6]

static void Task_DoPokeballSendOutAnim(u8 taskId)
{
    bool8 notSendOut = FALSE;
    u16 throwCaseId;
    u8 battler;
    u16 itemId;
    u8 ballId;
    u8 ballSpriteId;

    if (gTasks[taskId].tFrames == 0)
    {
        gTasks[taskId].tFrames++;
        return;
    }

    throwCaseId = gTasks[taskId].tThrowId;
    battler = gTasks[taskId].tBattler;
    if (GetBattlerSide(battler) != B_SIDE_PLAYER)
        itemId = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);
    else
        itemId = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);

    ballId = ItemIdToBallId(itemId);
    LoadBallGfx(ballId);
    ballSpriteId = CreateSprite(&gBallSpriteTemplates[ballId], 32, 80, 29);
    gSprites[ballSpriteId].data[0] = 0x80;
    gSprites[ballSpriteId].data[1] = 0;
    gSprites[ballSpriteId].data[7] = throwCaseId;

    switch (throwCaseId)
    {
    case POKEBALL_PLAYER_SENDOUT:
        gBattlerTarget = battler;
        gSprites[ballSpriteId].x = 24;
        gSprites[ballSpriteId].y = 68;
        gSprites[ballSpriteId].callback = SpriteCB_PlayerMonSendOut_1;
        break;
    case POKEBALL_OPPONENT_SENDOUT:
        gSprites[ballSpriteId].x = GetBattlerSpriteCoord(battler, BATTLER_COORD_X);
        gSprites[ballSpriteId].y = GetBattlerSpriteCoord(battler, BATTLER_COORD_Y) + 24;
        gBattlerTarget = battler;
        gSprites[ballSpriteId].data[0] = 0;
        gSprites[ballSpriteId].callback = SpriteCB_OpponentMonSendOut;
        break;
    default:
        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        notSendOut = TRUE;
        break;
    }

    gSprites[ballSpriteId].sBattler = gBattlerTarget;
    if (!notSendOut)
    {
        DestroyTask(taskId);
        return;
    }

    gSprites[ballSpriteId].data[0] = 0x22;
    gSprites[ballSpriteId].data[2] = GetBattlerSpriteCoord(gBattlerTarget, BATTLER_COORD_X);
    gSprites[ballSpriteId].data[4] = GetBattlerSpriteCoord(gBattlerTarget, BATTLER_COORD_Y) - 16;
    gSprites[ballSpriteId].data[5] = -40;
    InitAnimArcTranslation(&gSprites[ballSpriteId]);
    gSprites[ballSpriteId].oam.affineParam = taskId;
    gTasks[taskId].tOpponentBattler = gBattlerTarget;
    gTasks[taskId].func = TaskDummy;
    PlaySE(SE_BALL_THROW);
}

static void SpriteCB_BallThrow(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
    {
        u8 taskId = sprite->oam.affineParam;
        u8 opponentBattler = gTasks[taskId].tOpponentBattler;
        u8 noOfShakes = gTasks[taskId].tThrowId;
        u32 ballId;

        StartSpriteAnim(sprite, 1);
        sprite->affineAnimPaused = TRUE;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[5] = 0;
        ballId = ItemIdToBallId(GetBattlerPokeballItemId(opponentBattler));
        AnimateBallOpenParticles(sprite->x, sprite->y - 5, 1, 0x1C, ballId);
        sprite->data[0] = LaunchBallFadeMonTask(FALSE, opponentBattler, 14, ballId);
        sprite->sBattler = opponentBattler;
        sprite->data[7] = noOfShakes;
        DestroyTask(taskId);
        sprite->callback = SpriteCB_BallThrow_ReachMon;
    }
}

#undef tFrames
#undef tPan
#undef tThrowId
#undef tBattler
#undef tOpponentBattler

static void SpriteCB_BallThrow_ReachMon(struct Sprite *sprite)
{
    sprite->callback = SpriteCB_BallThrow_StartShrinkMon;
}

static void SpriteCB_BallThrow_StartShrinkMon(struct Sprite *sprite)
{
    sprite->data[5]++;
    if (sprite->data[5] == 10)
    {
        sprite->data[5] = 0;
        sprite->callback = SpriteCB_BallThrow_ShrinkMon;
        StartSpriteAffineAnim(&gSprites[gBattlerSpriteIds[sprite->sBattler]], 2);
        AnimateSprite(&gSprites[gBattlerSpriteIds[sprite->sBattler]]);
        gSprites[gBattlerSpriteIds[sprite->sBattler]].data[1] = 0;
    }
}

static void SpriteCB_BallThrow_ShrinkMon(struct Sprite *sprite)
{
    sprite->data[5]++;
    if (sprite->data[5] == 11)
        PlaySE(SE_BALL_TRADE);
    if (gSprites[gBattlerSpriteIds[sprite->sBattler]].affineAnimEnded)
    {
        StartSpriteAnim(sprite, 2);
        gSprites[gBattlerSpriteIds[sprite->sBattler]].invisible = TRUE;
        sprite->data[5] = 0;
        sprite->callback = SpriteCB_BallThrow_Close;
    }
    else
    {
        gSprites[gBattlerSpriteIds[sprite->sBattler]].data[1] += 0x60;
        gSprites[gBattlerSpriteIds[sprite->sBattler]].y2 = -gSprites[gBattlerSpriteIds[sprite->sBattler]].data[1] >> 8;
    }
}

static void SpriteCB_BallThrow_Close(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        sprite->data[5]++;
        if (sprite->data[5] == 1)
        {
            sprite->data[3] = 0;
            sprite->data[4] = 32;
            sprite->data[5] = 0;
            sprite->y += Cos(0, 32);
            sprite->y2 = -Cos(0, sprite->data[4]);
            sprite->callback = SpriteCB_BallThrow_FallToGround;
        }
    }
}

static void SpriteCB_BallThrow_FallToGround(struct Sprite *sprite)
{
    bool8 finishedBouncing = FALSE;

    switch (sprite->data[3] & 0xFF)
    {
    case 0:
        sprite->y2 = -Cos(sprite->data[5], sprite->data[4]);
        sprite->data[5] += 4 + (sprite->data[3] >> 8);
        if (sprite->data[5] >= 64)
        {
            sprite->data[4] -= 10;
            sprite->data[3] += 0x101;
            if (sprite->data[3] >> 8 == 4)
                finishedBouncing = TRUE;
            switch (sprite->data[3] >> 8)
            {
            case 1:
                PlaySE(SE_BALL_BOUNCE_1);
                break;
            case 2:
                PlaySE(SE_BALL_BOUNCE_2);
                break;
            case 3:
                PlaySE(SE_BALL_BOUNCE_3);
                break;
            default:
                PlaySE(SE_BALL_BOUNCE_4);
                break;
            }
        }
        break;
    case 1:
        sprite->y2 = -Cos(sprite->data[5], sprite->data[4]);
        sprite->data[5] -= 4 + (sprite->data[3] >> 8);
        if (sprite->data[5] <= 0)
        {
            sprite->data[5] = 0;
            sprite->data[3] &= 0xFF00;
        }
        break;
    }
    if (finishedBouncing)
    {
        sprite->data[3] = 0;
        sprite->y += Cos(64, 32);
        sprite->y2 = 0;
        if (sprite->data[7] == 0)
        {
            sprite->callback = SpriteCB_ReleaseMonFromBall;
        }
        else
        {
            sprite->callback = SpriteCB_BallThrow_StartShakes;
            sprite->data[4] = 1;
            sprite->data[5] = 0;
        }
    }
}

static void SpriteCB_BallThrow_StartShakes(struct Sprite *sprite)
{
    sprite->data[3]++;
    if (sprite->data[3] == 31)
    {
        sprite->data[3] = 0;
        sprite->affineAnimPaused = TRUE;
        StartSpriteAffineAnim(sprite, 1);
        sprite->callback = SpriteCB_BallThrow_Shake;
        PlaySE(SE_BALL);
    }
}

static void SpriteCB_BallThrow_Shake(struct Sprite *sprite)
{
    switch (sprite->data[3] & 0xFF)
    {
    case 0:
    case 2:
        sprite->x2 += sprite->data[4];
        sprite->data[5] += sprite->data[4];
        sprite->affineAnimPaused = FALSE;
        if (sprite->data[5] > 3 || sprite->data[5] < -3)
        {
            sprite->data[3]++;
            sprite->data[5] = 0;
        }
        break;
    case 1:
        sprite->data[5]++;
        if (sprite->data[5] == 1)
        {
            sprite->data[5] = 0;
            sprite->data[4] = -sprite->data[4];
            sprite->data[3]++;
            sprite->affineAnimPaused = FALSE;
            if (sprite->data[4] < 0)
                ChangeSpriteAffineAnim(sprite, 2);
            else
                ChangeSpriteAffineAnim(sprite, 1);
        }
        else
        {
            sprite->affineAnimPaused = TRUE;
        }
        break;
    case 3:
        sprite->data[3] += 0x100;
        if (sprite->data[3] >> 8 == sprite->data[7])
        {
            sprite->callback = SpriteCB_ReleaseMonFromBall;
        }
        else
        {
            if (sprite->data[7] == 4 && sprite->data[3] >> 8 == 3)
            {
                sprite->callback = SpriteCB_BallThrow_StartCaptureMon;
                sprite->affineAnimPaused = TRUE;
            }
            else
            {
                sprite->data[3]++;
                sprite->affineAnimPaused = TRUE;
            }
        }
        break;
    case 4:
    default:
        sprite->data[5]++;
        if (sprite->data[5] == 31)
        {
            sprite->data[5] = 0;
            sprite->data[3] &= 0xFF00;
            StartSpriteAffineAnim(sprite, 3);
            if (sprite->data[4] < 0)
                StartSpriteAffineAnim(sprite, 2);
            else
                StartSpriteAffineAnim(sprite, 1);
            PlaySE(SE_BALL);
        }
        break;
    }
}

#define tCryTaskSpecies      data[0]
#define tCryTaskPan          data[1]
#define tCryTaskWantedCry    data[2]
#define tCryTaskStartDelay   data[8]
#define tCryTaskFrames       data[10]
#define tCryTaskState        data[15]

static void Task_PlayCryWhenReleasedFromBall(u8 taskId)
{
    u8 wantedCry = gTasks[taskId].tCryTaskWantedCry;
    u8 pan = gTasks[taskId].tCryTaskPan;
    u16 species = gTasks[taskId].tCryTaskSpecies;

    switch (gTasks[taskId].tCryTaskState)
    {
    case 0:
    default:
        if (gTasks[taskId].tCryTaskStartDelay < 3)
            gTasks[taskId].tCryTaskStartDelay++;
        else
            gTasks[taskId].tCryTaskState = wantedCry + 1;
        break;
    case 1:
        PlayCry_Normal(species, pan);
        DestroyTask(taskId);
        break;
    case 2:
        StopCryAndClearCrySongs();
        gTasks[taskId].tCryTaskFrames = 3;
        gTasks[taskId].tCryTaskState = 20;
        break;
    case 20:
        if (gTasks[taskId].tCryTaskFrames != 0)
        {
            gTasks[taskId].tCryTaskFrames--;
            break;
        }
        PlayCry_ReleaseDouble(species, pan, 1);
        DestroyTask(taskId);
        break;
    case 3:
        gTasks[taskId].tCryTaskFrames = 6;
        gTasks[taskId].tCryTaskState = 30;
        break;
    case 30:
        if (gTasks[taskId].tCryTaskFrames != 0)
        {
            gTasks[taskId].tCryTaskFrames--;
            break;
        }
        gTasks[taskId].tCryTaskState++;
        // fall through
    case 31:
        if (!IsCryPlayingOrClearCrySongs())
        {
            StopCryAndClearCrySongs();
            gTasks[taskId].tCryTaskFrames = 3;
            gTasks[taskId].tCryTaskState++;
        }
        break;
    case 32:
        if (gTasks[taskId].tCryTaskFrames != 0)
        {
            gTasks[taskId].tCryTaskFrames--;
            break;
        }
        PlayCry_ReleaseDouble(species, pan, 0);
        DestroyTask(taskId);
        break;
    }
}

static void SpriteCB_ReleaseMonFromBall(struct Sprite *sprite)
{
    int ballId;
    u8 battler = sprite->sBattler;

    StartSpriteAnim(sprite, 1);
    ballId = ItemIdToBallId(GetBattlerPokeballItemId(battler));
    AnimateBallOpenParticles(sprite->x, sprite->y - 5, 1, 28, ballId);
    sprite->data[0] = LaunchBallFadeMonTask(TRUE, sprite->sBattler, 14, ballId);
    sprite->callback = HandleBallAnimEnd;
    if (gMain.inBattle)
    {
        struct Pokemon *mon;
        u16 species;
        s8 pan;
        u16 wantedCry;
        u8 taskId;

        if (GetBattlerSide(battler) != B_SIDE_PLAYER)
        {
            mon = &gEnemyParty[gBattlerPartyIndexes[battler]];
            pan = 25;
        }
        else
        {
            mon = &gPlayerParty[gBattlerPartyIndexes[battler]];
            pan = -25;
        }

        species = GetMonData(mon, MON_DATA_SPECIES);
        if ((battler == GetBattlerAtPosition(B_POSITION_PLAYER_LEFT) || battler == GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT))
         && IsDoubleBattle() && gBattleAnimationInfo.introAnimActive)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                if (IsBGMPlaying())
                    m4aMPlayStop(&gMPlayInfo_BGM);
            }
            else
            {
                m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 128);
            }
        }

        if (!IsDoubleBattle() || !gBattleAnimationInfo.introAnimActive)
            wantedCry = 0;
        else if (battler == GetBattlerAtPosition(B_POSITION_PLAYER_LEFT) || battler == GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT))
            wantedCry = 1;
        else
            wantedCry = 2;

        taskId = CreateTask(Task_PlayCryWhenReleasedFromBall, 3);
        gTasks[taskId].tCryTaskSpecies = species;
        gTasks[taskId].tCryTaskPan = pan;
        gTasks[taskId].tCryTaskWantedCry = wantedCry;
        gTasks[taskId].tCryTaskState = 0;
    }

    StartSpriteAffineAnim(&gSprites[gBattlerSpriteIds[sprite->sBattler]], 1);
    AnimateSprite(&gSprites[gBattlerSpriteIds[sprite->sBattler]]);
    gSprites[gBattlerSpriteIds[sprite->sBattler]].data[1] = 0x1000;
}

#undef tCryTaskSpecies
#undef tCryTaskPan
#undef tCryTaskWantedCry
#undef tCryTaskStartDelay
#undef tCryTaskFrames
#undef tCryTaskState

static void SpriteCB_BallThrow_StartCaptureMon(struct Sprite *sprite)
{
    sprite->animPaused = TRUE;
    sprite->callback = SpriteCB_BallThrow_CaptureMon;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    sprite->data[5] = 0;
}

static void HandleBallAnimEnd(struct Sprite *sprite)
{
    bool8 affineAnimEnded = FALSE;
    u8 battler = sprite->sBattler;

    gSprites[gBattlerSpriteIds[battler]].invisible = FALSE;
    if (sprite->animEnded)
        sprite->invisible = TRUE;
    if (gSprites[gBattlerSpriteIds[battler]].affineAnimEnded)
    {
        StartSpriteAffineAnim(&gSprites[gBattlerSpriteIds[battler]], 0);
        affineAnimEnded = TRUE;
    }
    else
    {
        gSprites[gBattlerSpriteIds[battler]].data[1] -= 288;
        gSprites[gBattlerSpriteIds[battler]].y2 = gSprites[gBattlerSpriteIds[battler]].data[1] >> 8;
    }
    if (sprite->animEnded && affineAnimEnded)
    {
        s32 i;
        u32 doneBattlers;

        gSprites[gBattlerSpriteIds[battler]].y2 = 0;
        gDoingBattleAnim = FALSE;
        gBattleHealthBoxInfo[battler].ballAnimActive = FALSE;
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
        for (doneBattlers = 0, i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            if (gBattleHealthBoxInfo[i].ballAnimActive == FALSE)
                doneBattlers++;
        }
        if (doneBattlers == MAX_BATTLERS_COUNT)
        {
            for (i = 0; i < POKEBALL_COUNT; i++)
                FreeBallGfx(i);
        }
    }
}

static void SpriteCB_BallThrow_CaptureMon(struct Sprite *sprite)
{
    u8 battler = sprite->sBattler;

    sprite->data[4]++;
    if (sprite->data[4] == 40)
    {
        return;
    }
    else if (sprite->data[4] == 95)
    {
        gDoingBattleAnim = FALSE;
        m4aMPlayAllStop();
        PlaySE(MUS_EVOLVED);
    }
    else if (sprite->data[4] == 315)
    {
        FreeOamMatrix(gSprites[gBattlerSpriteIds[sprite->sBattler]].oam.matrixNum);
        DestroySprite(&gSprites[gBattlerSpriteIds[sprite->sBattler]]);
        DestroySpriteAndFreeResources(sprite);
        if (gMain.inBattle)
            gBattleHealthBoxInfo[battler].ballAnimActive = FALSE;
    }
}

static void SpriteCB_PlayerMonSendOut_1(struct Sprite *sprite)
{
    sprite->data[0] = 25;
    sprite->data[2] = GetBattlerSpriteCoord(sprite->sBattler, BATTLER_COORD_X_2);
    sprite->data[4] = GetBattlerSpriteCoord(sprite->sBattler, BATTLER_COORD_Y_PIC_OFFSET) + 24;
    sprite->data[5] = -30;
    sprite->oam.affineParam = sprite->sBattler;
    InitAnimArcTranslation(sprite);
    sprite->callback = SpriteCB_PlayerMonSendOut_2;
}

#define HIBYTE(x) (((x) >> 8) & 0xFF)

static void SpriteCB_PlayerMonSendOut_2(struct Sprite *sprite)
{
    u32 r6;
    u32 r7;

    if (HIBYTE(sprite->data[7]) >= 35 && HIBYTE(sprite->data[7]) < 80)
    {
        s16 r4;

        if ((sprite->oam.affineParam & 0xFF00) == 0)
        {
            r6 = sprite->data[1] & 1;
            r7 = sprite->data[2] & 1;
            sprite->data[1] = ((sprite->data[1] / 3) & ~1) | r6;
            sprite->data[2] = ((sprite->data[2] / 3) & ~1) | r7;
            StartSpriteAffineAnim(sprite, 4);
        }

        r4 = sprite->data[0];
        AnimTranslateLinear(sprite);
        sprite->data[7] += sprite->sBattler / 3;
        sprite->y2 += Sin(HIBYTE(sprite->data[7]), sprite->data[5]);
        sprite->oam.affineParam += 0x100;
        if ((sprite->oam.affineParam >> 8) % 3 != 0)
            sprite->data[0] = r4;
        else
            sprite->data[0] = r4 - 1;

        if (HIBYTE(sprite->data[7]) >= 80)
        {
            r6 = sprite->data[1] & 1;
            r7 = sprite->data[2] & 1;
            sprite->data[1] = ((sprite->data[1] * 3) & ~1) | r6;
            sprite->data[2] = ((sprite->data[2] * 3) & ~1) | r7;
        }
    }
    else
    {
        if (TranslateAnimHorizontalArc(sprite))
        {
            sprite->x += sprite->x2;
            sprite->y += sprite->y2;
            sprite->y2 = 0;
            sprite->x2 = 0;
            sprite->sBattler = sprite->oam.affineParam & 0xFF;
            sprite->data[0] = 0;
            if (IsDoubleBattle() && gBattleAnimationInfo.introAnimActive && sprite->sBattler == GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT))
                sprite->callback = SpriteCB_ReleaseMon2FromBall;
            else
                sprite->callback = SpriteCB_ReleaseMonFromBall;

            StartSpriteAffineAnim(sprite, 0);
        }
    }
}

static void SpriteCB_ReleaseMon2FromBall(struct Sprite *sprite)
{
    if (sprite->data[0]++ > 24)
    {
        sprite->data[0] = 0;
        sprite->callback = SpriteCB_ReleaseMonFromBall;
    }
}

static void SpriteCB_OpponentMonSendOut(struct Sprite *sprite)
{
    sprite->data[0]++;
    if (sprite->data[0] > 15)
    {
        sprite->data[0] = 0;
        if (IsDoubleBattle() && gBattleAnimationInfo.introAnimActive && sprite->sBattler == GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT))
            sprite->callback = SpriteCB_ReleaseMon2FromBall;
        else
            sprite->callback = SpriteCB_ReleaseMonFromBall;
    }
}

static u8 AnimateBallOpenParticlesForPokeball(u8 x, u8 y, u8 kindOfStars, u8 subpriority)
{
    return AnimateBallOpenParticles(x, y, kindOfStars, subpriority, BALL_POKE);
}

static u8 LaunchBallFadeMonTaskForPokeball(u8 unFadeLater, u8 spritePalNum, u32 selectedPalettes)
{
    return LaunchBallFadeMonTask(unFadeLater, spritePalNum, selectedPalettes, BALL_POKE);
}

#undef sBattler

#define sMonSpriteId data[0]
#define sDelay       data[1]
#define sMonPalNum   data[2]
#define sFadePalsLo  data[3]
#define sFadePalsHi  data[4]
#define sFinalMonX   data[5]
#define sFinalMonY   data[6]
#define sTrigIdx     data[7]

void CreatePokeballSpriteToReleaseMon(u8 monSpriteId, u8 monPalNum, u8 x, u8 y, u8 oamPriority, u8 subpriority, u8 delay, u32 fadePalettes)
{
    u8 spriteId;

    LoadCompressedSpriteSheet(&sBallSpriteSheets[BALL_POKE]);
    LoadCompressedSpritePalette(&sBallSpritePalettes[BALL_POKE]);
    spriteId = CreateSprite(&gBallSpriteTemplates[BALL_POKE], x, y, subpriority);
    gSprites[spriteId].sMonSpriteId = monSpriteId;
    gSprites[spriteId].sFinalMonX = gSprites[monSpriteId].x;
    gSprites[spriteId].sFinalMonY = gSprites[monSpriteId].y;
    gSprites[monSpriteId].x = x;
    gSprites[monSpriteId].y = y;
    gSprites[spriteId].sDelay = delay;
    gSprites[spriteId].sMonPalNum = monPalNum;
    gSprites[spriteId].sFadePalsLo = fadePalettes;
    gSprites[spriteId].sFadePalsHi = fadePalettes >> 16;
    gSprites[spriteId].oam.priority = oamPriority;
    gSprites[spriteId].callback = SpriteCB_PokeballReleaseMon;
    gSprites[monSpriteId].invisible = TRUE;
}

static void SpriteCB_PokeballReleaseMon(struct Sprite *sprite)
{
    if (sprite->sDelay == 0)
    {
        u8 subpriority;
        u8 monSpriteId = sprite->sMonSpriteId;
        u8 monPalNum = sprite->sMonPalNum;
        u32 selectedPalettes = (u16)sprite->sFadePalsLo | ((u16)sprite->sFadePalsHi << 16);

        if (sprite->subpriority != 0)
            subpriority = sprite->subpriority - 1;
        else
            subpriority = 0;
        StartSpriteAnim(sprite, 1);
        AnimateBallOpenParticlesForPokeball(sprite->x, sprite->y - 5, sprite->oam.priority, subpriority);
        sprite->sDelay = LaunchBallFadeMonTaskForPokeball(TRUE, monPalNum, selectedPalettes);
        sprite->callback = SpriteCB_ReleasedMonFlyOut;
        gSprites[monSpriteId].invisible = FALSE;
        StartSpriteAffineAnim(&gSprites[monSpriteId], 1);
        AnimateSprite(&gSprites[monSpriteId]);
        gSprites[monSpriteId].data[1] = 0x1000;
        sprite->sTrigIdx = 0;
    }
    else
    {
        sprite->sDelay--;
    }
}

static void SpriteCB_ReleasedMonFlyOut(struct Sprite *sprite)
{
    bool8 emergeAnimFinished = FALSE;
    bool8 atFinalPosition = FALSE;
    u8 monSpriteId = sprite->sMonSpriteId;
    u16 x;
    u16 y;

    if (sprite->animEnded)
        sprite->invisible = TRUE;
    if (gSprites[monSpriteId].affineAnimEnded)
    {
        StartSpriteAffineAnim(&gSprites[monSpriteId], 0);
        emergeAnimFinished = TRUE;
    }
    x = (sprite->sFinalMonX - sprite->x) * sprite->sTrigIdx / 128 + sprite->x;
    y = (sprite->sFinalMonY - sprite->y) * sprite->sTrigIdx / 128 + sprite->y;
    gSprites[monSpriteId].x = x;
    gSprites[monSpriteId].y = y;
    if (sprite->sTrigIdx < 128)
    {
        s16 sine = -(gSineTable[(u8)sprite->sTrigIdx] / 8);

        sprite->sTrigIdx += 4;
        gSprites[monSpriteId].x2 = sine;
        gSprites[monSpriteId].y2 = sine;
    }
    else
    {
        gSprites[monSpriteId].x = sprite->sFinalMonX;
        gSprites[monSpriteId].y = sprite->sFinalMonY;
        gSprites[monSpriteId].x2 = 0;
        gSprites[monSpriteId].y2 = 0;
        atFinalPosition = TRUE;
    }
    if (sprite->animEnded && emergeAnimFinished && atFinalPosition)
        DestroySpriteAndFreeResources(sprite);
}

#undef sFinalMonX
#undef sFinalMonY
#undef sTrigIdx

#define sTimer data[5]

u8 CreateTradePokeballSprite(u8 monSpriteId, u8 monPalNum, u8 x, u8 y, u8 oamPriority, u8 subPriority, u8 delay, u32 fadePalettes)
{
    u8 spriteId;

    LoadCompressedSpriteSheet(&sBallSpriteSheets[BALL_POKE]);
    LoadCompressedSpritePalette(&sBallSpritePalettes[BALL_POKE]);
    spriteId = CreateSprite(&gBallSpriteTemplates[BALL_POKE], x, y, subPriority);
    gSprites[spriteId].sMonSpriteId = monSpriteId;
    gSprites[spriteId].sDelay = delay;
    gSprites[spriteId].sMonPalNum = monPalNum;
    gSprites[spriteId].sFadePalsLo = fadePalettes;
    gSprites[spriteId].sFadePalsHi = fadePalettes >> 16;
    gSprites[spriteId].oam.priority = oamPriority;
    gSprites[spriteId].callback = SpriteCB_TradePokeball;
    return spriteId;
}

static void SpriteCB_TradePokeball(struct Sprite *sprite)
{
    if (sprite->sDelay == 0)
    {
        u8 subpriority;
        u8 monSpriteId = sprite->sMonSpriteId;
        u8 monPalNum = sprite->sMonPalNum;
        u32 selectedPalettes = (u16)sprite->sFadePalsLo | ((u16)sprite->sFadePalsHi << 16);

        if (sprite->subpriority != 0)
            subpriority = sprite->subpriority - 1;
        else
            subpriority = 0;
        StartSpriteAnim(sprite, 1);
        AnimateBallOpenParticlesForPokeball(sprite->x, sprite->y - 5, sprite->oam.priority, subpriority);
        sprite->sDelay = LaunchBallFadeMonTaskForPokeball(TRUE, monPalNum, selectedPalettes);
        sprite->callback = SpriteCB_TradePokeballSendOff;
        StartSpriteAffineAnim(&gSprites[monSpriteId], 2);
        AnimateSprite(&gSprites[monSpriteId]);
        gSprites[monSpriteId].data[1] = 0;
    }
    else
    {
        sprite->sDelay--;
    }
}

static void SpriteCB_TradePokeballSendOff(struct Sprite *sprite)
{
    u8 monSpriteId;

    sprite->sTimer++;
    if (sprite->sTimer == 11)
        PlaySE(SE_BALL_TRADE);
    monSpriteId = sprite->sMonSpriteId;
    if (gSprites[monSpriteId].affineAnimEnded)
    {
        StartSpriteAnim(sprite, 2);
        gSprites[monSpriteId].invisible = TRUE;
        sprite->sTimer = 0;
        sprite->callback = SpriteCB_TradePokeballEnd;
    }
    else
    {
        gSprites[monSpriteId].data[1] += 96;
        gSprites[monSpriteId].y2 = -gSprites[monSpriteId].data[1] >> 8;
    }
}

static void SpriteCB_TradePokeballEnd(struct Sprite *sprite)
{
    if (sprite->animEnded)
        sprite->callback = SpriteCallbackDummy;
}

#undef sMonSpriteId
#undef sDelay
#undef sMonPalNum
#undef sFadePalsLo
#undef sFadePalsHi
#undef sTimer

void DestroySpriteAndFreeResources_Ball(struct Sprite *sprite)
{
    DestroySpriteAndFreeResources(sprite);
}

#define sSpeedX     data[0]
#define sSpeedY     data[1]
#define sDelayTimer data[1]

void StartHealthboxSlideIn(u8 battler)
{
    struct Sprite *healthboxSprite = &gSprites[gHealthboxSpriteIds[battler]];

    healthboxSprite->sSpeedX = 5;
    healthboxSprite->sSpeedY = 0;
    healthboxSprite->x2 = 0x73;
    healthboxSprite->y2 = 0;
    healthboxSprite->callback = SpriteCB_HealthboxSlideIn;
    if (GetBattlerSide(battler) != B_SIDE_PLAYER)
    {
        healthboxSprite->sSpeedX = -healthboxSprite->sSpeedX;
        healthboxSprite->sSpeedY = -healthboxSprite->sSpeedY;
        healthboxSprite->x2 = -healthboxSprite->x2;
        healthboxSprite->y2 = -healthboxSprite->y2;
    }
    gSprites[healthboxSprite->data[5]].callback(&gSprites[healthboxSprite->data[5]]);
    if (GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT)
        healthboxSprite->callback = SpriteCB_HealthboxSlideInDelayed;
}

static void SpriteCB_HealthboxSlideInDelayed(struct Sprite *sprite)
{
    sprite->sDelayTimer++;
    if (sprite->sDelayTimer == 20)
    {
        sprite->sDelayTimer = 0;
        sprite->callback = SpriteCB_HealthboxSlideIn;
    }
}

static void SpriteCB_HealthboxSlideIn(struct Sprite *sprite)
{
    sprite->x2 -= sprite->sSpeedX;
    sprite->y2 -= sprite->sSpeedY;
    if (sprite->x2 == 0 && sprite->y2 == 0)
        sprite->callback = SpriteCallbackDummy;
}

#undef sSpeedX
#undef sSpeedY
#undef sDelayTimer

void DoHitAnimHealthboxEffect(u8 battler)
{
    u8 spriteId;

    spriteId = CreateInvisibleSpriteWithCallback(SpriteCB_HitAnimHealthoxEffect);
    gSprites[spriteId].data[0] = 1;
    gSprites[spriteId].data[1] = gHealthboxSpriteIds[battler];
    gSprites[spriteId].callback = SpriteCB_HitAnimHealthoxEffect;
}

static void SpriteCB_HitAnimHealthoxEffect(struct Sprite *sprite)
{
    u8 healthboxSpriteId = sprite->data[1];

    gSprites[healthboxSpriteId].y2 = sprite->data[0];
    sprite->data[0] = -sprite->data[0];
    sprite->data[2]++;
    if (sprite->data[2] == 21)
    {
        gSprites[healthboxSpriteId].x2 = 0;
        gSprites[healthboxSpriteId].y2 = 0;
        DestroySprite(sprite);
    }
}

void LoadBallGfx(u8 ballId)
{
    u16 tileStart;

    if (GetSpriteTileStartByTag(sBallSpriteSheets[ballId].tag) == 0xFFFF)
    {
        LoadCompressedSpriteSheet(&sBallSpriteSheets[ballId]);
        LoadCompressedSpritePalette(&sBallSpritePalettes[ballId]);
    }

    switch (ballId)
    {
    case BALL_DIVE:
    case BALL_LUXURY:
    case BALL_PREMIER:
        break;
    default:
        tileStart = GetSpriteTileStartByTag(sBallSpriteSheets[ballId].tag);
        LZDecompressVram(gOpenPokeballGfx, (void *)(VRAM + 0x10100 + tileStart * 32));
        break;
    }
}

void FreeBallGfx(u8 ballId)
{
    FreeSpriteTilesByTag(sBallSpriteSheets[ballId].tag);
    FreeSpritePaletteByTag(sBallSpritePalettes[ballId].tag);
}

static u16 GetBattlerPokeballItemId(u8 battler)
{
    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
        return GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);
    else
        return GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);
}
