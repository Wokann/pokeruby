#include "global.h"
#include "gba/m4a_internal.h"
#include "battle.h"
#include "battle_anim_special.h"
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

#define GFX_TAG_POKE_BALL    55000
#define GFX_TAG_GREAT_BALL   55001
#define GFX_TAG_SAFARI_BALL  55002
#define GFX_TAG_ULTRA_BALL   55003
#define GFX_TAG_MASTER_BALL  55004
#define GFX_TAG_NET_BALL     55005
#define GFX_TAG_DIVE_BALL    55006
#define GFX_TAG_NEST_BALL    55007
#define GFX_TAG_REPEAT_BALL  55008
#define GFX_TAG_TIMER_BALL   55009
#define GFX_TAG_LUXURY_BALL  55010
#define GFX_TAG_PREMIER_BALL 55011

const struct CompressedSpriteSheet gBallSpriteSheets[] =
{
    {gBallGfx_Poke,    384, GFX_TAG_POKE_BALL},
    {gBallGfx_Great,   384, GFX_TAG_GREAT_BALL},
    {gBallGfx_Safari,  384, GFX_TAG_SAFARI_BALL},
    {gBallGfx_Ultra,   384, GFX_TAG_ULTRA_BALL},
    {gBallGfx_Master,  384, GFX_TAG_MASTER_BALL},
    {gBallGfx_Net,     384, GFX_TAG_NET_BALL},
    {gBallGfx_Dive,    384, GFX_TAG_DIVE_BALL},
    {gBallGfx_Nest,    384, GFX_TAG_NEST_BALL},
    {gBallGfx_Repeat,  384, GFX_TAG_REPEAT_BALL},
    {gBallGfx_Timer,   384, GFX_TAG_TIMER_BALL},
    {gBallGfx_Luxury,  384, GFX_TAG_LUXURY_BALL},
    {gBallGfx_Premier, 384, GFX_TAG_PREMIER_BALL},
};

const struct CompressedSpritePalette gBallSpritePalettes[] =
{
    {gBallPal_Poke,    GFX_TAG_POKE_BALL},
    {gBallPal_Great,   GFX_TAG_GREAT_BALL},
    {gBallPal_Safari,  GFX_TAG_SAFARI_BALL},
    {gBallPal_Ultra,   GFX_TAG_ULTRA_BALL},
    {gBallPal_Master,  GFX_TAG_MASTER_BALL},
    {gBallPal_Net,     GFX_TAG_NET_BALL},
    {gBallPal_Dive,    GFX_TAG_DIVE_BALL},
    {gBallPal_Nest,    GFX_TAG_NEST_BALL},
    {gBallPal_Repeat,  GFX_TAG_REPEAT_BALL},
    {gBallPal_Timer,   GFX_TAG_TIMER_BALL},
    {gBallPal_Luxury,  GFX_TAG_LUXURY_BALL},
    {gBallPal_Premier, GFX_TAG_PREMIER_BALL},
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

static const union AffineAnimCmd sAffineAnim_BallRotate_0[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 0, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sAffineAnim_BallRotate_Right[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -3, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sAffineAnim_BallRotate_Left[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 3, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sAffineAnim_BallRotate_3[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_BallRotate_4[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 25, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd *const sAffineAnim_BallRotate[] =
{
    sAffineAnim_BallRotate_0,
    sAffineAnim_BallRotate_Right,
    sAffineAnim_BallRotate_Left,
    sAffineAnim_BallRotate_3,
    sAffineAnim_BallRotate_4,
};

static void SpriteCB_BallThrow(struct Sprite *sprite);
const struct SpriteTemplate gBallSpriteTemplates[] =
{
    {
        .tileTag = GFX_TAG_POKE_BALL,
        .paletteTag = GFX_TAG_POKE_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_GREAT_BALL,
        .paletteTag = GFX_TAG_GREAT_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_SAFARI_BALL,
        .paletteTag = GFX_TAG_SAFARI_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_ULTRA_BALL,
        .paletteTag = GFX_TAG_ULTRA_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_MASTER_BALL,
        .paletteTag = GFX_TAG_MASTER_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_NET_BALL,
        .paletteTag = GFX_TAG_NET_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_DIVE_BALL,
        .paletteTag = GFX_TAG_DIVE_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_NEST_BALL,
        .paletteTag = GFX_TAG_NEST_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_REPEAT_BALL,
        .paletteTag = GFX_TAG_REPEAT_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_TIMER_BALL,
        .paletteTag = GFX_TAG_TIMER_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_LUXURY_BALL,
        .paletteTag = GFX_TAG_LUXURY_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
        .callback = SpriteCB_BallThrow,
    },
    {
        .tileTag = GFX_TAG_PREMIER_BALL,
        .paletteTag = GFX_TAG_PREMIER_BALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sAffineAnim_BallRotate,
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
static void sub_8046C78(struct Sprite *);
static void SpriteCB_BallThrow_StartCaptureMon(struct Sprite *);
static void HandleBallAnimEnd(struct Sprite *);
static void SpriteCB_BallThrow_CaptureMon(struct Sprite *);
static void SpriteCB_PlayerMonSendOut_1(struct Sprite *);
static void SpriteCB_PlayerMonSendOut_2(struct Sprite *);
static void SpriteCB_ReleaseMon2FromBall(struct Sprite *);
static void SpriteCB_OpponentMonSendOut(struct Sprite *);
static void SpriteCB_PokeballReleaseMon(struct Sprite *);
static void sub_804748C(struct Sprite *);
static void SpriteCB_TradePokeball(struct Sprite *);
static void SpriteCB_TradePokeballSendOff(struct Sprite *);
static void SpriteCB_TradePokeballEnd(struct Sprite *);
static void SpriteCB_HealthboxSlideInDelayed(struct Sprite *);
static void SpriteCB_HealthboxSlideIn(struct Sprite *);
static void SpriteCB_HitAnimHealthoxEffect(struct Sprite *);
static u16 GetBattlerPokeballItemId(u8);

u8 DoPokeballSendOutAnimation(u16 pan, u8 kindOfThrow)
{
    u8 taskId;

    gDoingBattleAnim = TRUE;
    gBattleHealthBoxInfo[gActiveBattler].ballAnimActive = 1;
    taskId = CreateTask(Task_DoPokeballSendOutAnim, 5);
    gTasks[taskId].data[1] = pan;
    gTasks[taskId].data[2] = kindOfThrow;
    gTasks[taskId].data[3] = gActiveBattler;
    return 0;
}

static void Task_DoPokeballSendOutAnim(u8 taskId)
{
    bool8 notSendOut = FALSE;
    u16 throwCaseId;
    u8 battler;
    u16 itemId;
    u8 ballId;
    u8 ballSpriteId;

    if (gTasks[taskId].data[0] == 0)
    {
        gTasks[taskId].data[0]++;
        return;
    }

    throwCaseId = gTasks[taskId].data[2];
    battler = gTasks[taskId].data[3];
    if (GetBattlerSide(battler) != B_SIDE_PLAYER)
        itemId = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);
    else
        itemId = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);

    ballId = ball_number_to_ball_processing_index(itemId);
    LoadBallGfx(ballId);
    ballSpriteId = CreateSprite(&gBallSpriteTemplates[ballId], 32, 80, 29);
    gSprites[ballSpriteId].data[0] = 0x80;
    gSprites[ballSpriteId].data[1] = 0;
    gSprites[ballSpriteId].data[7] = throwCaseId;

    switch (throwCaseId)
    {
    case 0xFF: // Player's side
        gBattlerTarget = battler;
        gSprites[ballSpriteId].x = 24;
        gSprites[ballSpriteId].y = 68;
        gSprites[ballSpriteId].callback = SpriteCB_PlayerMonSendOut_1;
        break;
    case 0xFE: // Opponent's side
        gSprites[ballSpriteId].x = GetBattlerSpriteCoord(battler, 0);
        gSprites[ballSpriteId].y = GetBattlerSpriteCoord(battler, 1) + 24;
        gBattlerTarget = battler;
        gSprites[ballSpriteId].data[0] = 0;
        gSprites[ballSpriteId].callback = SpriteCB_OpponentMonSendOut;
        break;
    default:
        gBattlerTarget = GetBattlerAtPosition(1);
        notSendOut = TRUE;
        break;
    }

    gSprites[ballSpriteId].data[6] = gBattlerTarget;
    if (!notSendOut)
    {
        DestroyTask(taskId);
        return;
    }

    gSprites[ballSpriteId].data[0] = 0x22;
    gSprites[ballSpriteId].data[2] = GetBattlerSpriteCoord(gBattlerTarget, 0);
    gSprites[ballSpriteId].data[4] = GetBattlerSpriteCoord(gBattlerTarget, 1) - 16;
    gSprites[ballSpriteId].data[5] = -40;
    InitAnimArcTranslation(&gSprites[ballSpriteId]);
    gSprites[ballSpriteId].oam.affineParam = taskId;
    gTasks[taskId].data[4] = gBattlerTarget;
    gTasks[taskId].func = TaskDummy;
    PlaySE(SE_BALL_THROW);
}

static void SpriteCB_BallThrow(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
    {
        u8 taskId = sprite->oam.affineParam;
        u8 opponentBattler = gTasks[taskId].data[4];
        u8 noOfShakes = gTasks[taskId].data[2];
        u32 ballId;  // not sure of this type

        StartSpriteAnim(sprite, 1);
        sprite->affineAnimPaused = TRUE;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[5] = 0;
        ballId = ball_number_to_ball_processing_index(GetBattlerPokeballItemId(opponentBattler));
        AnimateBallOpenParticles(sprite->x, sprite->y - 5, 1, 0x1C, ballId);
        sprite->data[0] = sub_8141314(0, opponentBattler, 14, ballId);
        sprite->data[6] = opponentBattler;
        sprite->data[7] = noOfShakes;
        DestroyTask(taskId);
        sprite->callback = SpriteCB_BallThrow_ReachMon;
    }
}

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
        StartSpriteAffineAnim(&gSprites[gBattlerSpriteIds[sprite->data[6]]], 2);
        AnimateSprite(&gSprites[gBattlerSpriteIds[sprite->data[6]]]);
        gSprites[gBattlerSpriteIds[sprite->data[6]]].data[1] = 0;
    }
}

static void SpriteCB_BallThrow_ShrinkMon(struct Sprite *sprite)
{
    sprite->data[5]++;
    if (sprite->data[5] == 11)
        PlaySE(SE_BALL_TRADE);
    if (gSprites[gBattlerSpriteIds[sprite->data[6]]].affineAnimEnded)
    {
        StartSpriteAnim(sprite, 2);
        gSprites[gBattlerSpriteIds[sprite->data[6]]].invisible = TRUE;
        sprite->data[5] = 0;
        sprite->callback = SpriteCB_BallThrow_Close;
    }
    else
    {
        gSprites[gBattlerSpriteIds[sprite->data[6]]].data[1] += 0x60;
        gSprites[gBattlerSpriteIds[sprite->data[6]]].y2 = -gSprites[gBattlerSpriteIds[sprite->data[6]]].data[1] >> 8;
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
    bool8 r5 = FALSE;

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
                r5 = TRUE;
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
    if (r5)
    {
        sprite->data[3] = 0;
        sprite->y += Cos(64, 32);
        sprite->y2 = 0;
        if (sprite->data[7] == 0)
        {
            sprite->callback = sub_8046C78;
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
            sprite->callback = sub_8046C78;
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

static void sub_8046AD0(u8 taskId)
{
    u8 wantedCry = gTasks[taskId].data[2];
    u8 pan = gTasks[taskId].data[1];
    u16 species = gTasks[taskId].data[0];

    switch (gTasks[taskId].data[15])
    {
    case 0:
    default:
        if (gTasks[taskId].data[8] < 3)
            gTasks[taskId].data[8]++;
        else
            gTasks[taskId].data[15] = wantedCry + 1;
        break;
    case 1:
        PlayCry_Normal(species, pan);
        DestroyTask(taskId);
        break;
    case 2:
        StopCryAndClearCrySongs();
        gTasks[taskId].data[10] = 3;
        gTasks[taskId].data[15] = 20;
        break;
    case 20:
        if (gTasks[taskId].data[10] != 0)
        {
            gTasks[taskId].data[10]--;
            break;
        }
        PlayCry_ReleaseDouble(species, pan, 1);
        DestroyTask(taskId);
        break;
    case 3:
        gTasks[taskId].data[10] = 6;
        gTasks[taskId].data[15] = 30;
        break;
    case 30:
        if (gTasks[taskId].data[10] != 0)
        {
            gTasks[taskId].data[10]--;
            break;
        }
        gTasks[taskId].data[15]++;
        // fall through
    case 31:
        if (!IsCryPlayingOrClearCrySongs())
        {
            StopCryAndClearCrySongs();
            gTasks[taskId].data[10] = 3;
            gTasks[taskId].data[15]++;
        }
        break;
    case 32:
        if (gTasks[taskId].data[10] != 0)
        {
            gTasks[taskId].data[10]--;
            break;
        }
        PlayCry_ReleaseDouble(species, pan, 0);
        DestroyTask(taskId);
        break;
    }
}

static void sub_8046C78(struct Sprite *sprite)
{
    int ballId;
    u8 battler = sprite->data[6];

    StartSpriteAnim(sprite, 1);
    ballId = ball_number_to_ball_processing_index(GetBattlerPokeballItemId(battler));
    AnimateBallOpenParticles(sprite->x, sprite->y - 5, 1, 28, ballId);
    sprite->data[0] = sub_8141314(1, sprite->data[6], 14, ballId);
    sprite->callback = HandleBallAnimEnd;
    if (gMain.inBattle)
    {
        struct Pokemon *mon;
        u16 species;
        s8 cryPanning;
        u16 wantedCryCase;
        u8 taskId;

        if (GetBattlerSide(battler) != 0)
        {
            mon = &gEnemyParty[gBattlerPartyIndexes[battler]];
            cryPanning = 25;
        }
        else
        {
            mon = &gPlayerParty[gBattlerPartyIndexes[battler]];
            cryPanning = -25;
        }

        species = GetMonData(mon, MON_DATA_SPECIES);
        if ((battler == GetBattlerAtPosition(0) || battler == GetBattlerAtPosition(1))
         && IsDoubleBattle() && gBattleAnimationInfo->introAnimActive)
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

        if (!IsDoubleBattle() || !gBattleAnimationInfo->introAnimActive)
            wantedCryCase = 0;
        else if (battler == GetBattlerAtPosition(0) || battler == GetBattlerAtPosition(1))
            wantedCryCase = 1;
        else
            wantedCryCase = 2;

        taskId = CreateTask(sub_8046AD0, 3);
        gTasks[taskId].data[0] = species;
        gTasks[taskId].data[1] = cryPanning;
        gTasks[taskId].data[2] = wantedCryCase;
        gTasks[taskId].data[15] = 0;
    }

    StartSpriteAffineAnim(&gSprites[gBattlerSpriteIds[sprite->data[6]]], 1);
    AnimateSprite(&gSprites[gBattlerSpriteIds[sprite->data[6]]]);
    gSprites[gBattlerSpriteIds[sprite->data[6]]].data[1] = 0x1000;
}

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
    u8 battler = sprite->data[6];

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
        gBattleHealthBoxInfo[battler].ballAnimActive = 0;
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
        for (doneBattlers = 0, i = 0; i < 4; i++)
        {
            if (gBattleHealthBoxInfo[i].ballAnimActive == 0)
                doneBattlers++;
        }
        if (doneBattlers == 4)
        {
            for (i = 0; i < 12; i++)
                FreeBallGfx(i);
        }
    }
}

static void SpriteCB_BallThrow_CaptureMon(struct Sprite *sprite)
{
    u8 battler = sprite->data[6];

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
        FreeOamMatrix(gSprites[gBattlerSpriteIds[sprite->data[6]]].oam.matrixNum);
        DestroySprite(&gSprites[gBattlerSpriteIds[sprite->data[6]]]);
        DestroySpriteAndFreeResources(sprite);
        if (gMain.inBattle)
            gBattleHealthBoxInfo[battler].ballAnimActive = 0;
    }
}

static void SpriteCB_PlayerMonSendOut_1(struct Sprite *sprite)
{
    sprite->data[0] = 25;
    sprite->data[2] = GetBattlerSpriteCoord(sprite->data[6], 2);
    sprite->data[4] = GetBattlerSpriteCoord(sprite->data[6], 3) + 24;
    sprite->data[5] = -30;
    sprite->oam.affineParam = sprite->data[6];
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
        sprite->data[7] += sprite->data[6] / 3;
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
            sprite->data[6] = sprite->oam.affineParam & 0xFF;
            sprite->data[0] = 0;
            if (IsDoubleBattle() && gBattleAnimationInfo->introAnimActive && sprite->data[6] == GetBattlerAtPosition(2))
                sprite->callback = SpriteCB_ReleaseMon2FromBall;
            else
                sprite->callback = sub_8046C78;

            StartSpriteAffineAnim(sprite, 0);
        }
    }
}

static void SpriteCB_ReleaseMon2FromBall(struct Sprite *sprite)
{
    if (sprite->data[0]++ > 24)
    {
        sprite->data[0] = 0;
        sprite->callback = sub_8046C78;
    }
}

static void SpriteCB_OpponentMonSendOut(struct Sprite *sprite)
{
    sprite->data[0]++;
    if (sprite->data[0] > 15)
    {
        sprite->data[0] = 0;
        if (IsDoubleBattle() && gBattleAnimationInfo->introAnimActive && sprite->data[6] == GetBattlerAtPosition(3))
            sprite->callback = SpriteCB_ReleaseMon2FromBall;
        else
            sprite->callback = sub_8046C78;
    }
}

static u8 AnimateBallOpenParticlesForPokeball(u8 x, u8 y, u8 priority, u8 subpriority)
{
    return AnimateBallOpenParticles(x, y, priority, subpriority, 0);
}

static u8 LaunchBallFadeMonTaskForPokeball(u8 unFadeLater, u8 spritePalNum, u32 selectedPalettes)
{
    return sub_8141314(unFadeLater, spritePalNum, selectedPalettes, 0);
}

void CreatePokeballSprite(u8 monSpriteId, u8 monPalNum, u8 x, u8 y, u8 oamPriority, u8 subpriority, u8 delay, u32 fadePalettes)
{
    u8 spriteId;

    LoadCompressedObjectPic(&gBallSpriteSheets[0]);
    LoadCompressedObjectPalette(&gBallSpritePalettes[0]);
    spriteId = CreateSprite(&gBallSpriteTemplates[0], x, y, subpriority);
    gSprites[spriteId].data[0] = monSpriteId;
    gSprites[spriteId].data[5] = gSprites[monSpriteId].x;
    gSprites[spriteId].data[6] = gSprites[monSpriteId].y;
    gSprites[monSpriteId].x = x;
    gSprites[monSpriteId].y = y;
    gSprites[spriteId].data[1] = delay;
    gSprites[spriteId].data[2] = monPalNum;
    gSprites[spriteId].data[3] = fadePalettes;
    gSprites[spriteId].data[4] = fadePalettes >> 16;
    gSprites[spriteId].oam.priority = oamPriority;
    gSprites[spriteId].callback = SpriteCB_PokeballReleaseMon;
    gSprites[monSpriteId].invisible = TRUE;
}

static void SpriteCB_PokeballReleaseMon(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        u8 subpriority;
        u8 spriteId = sprite->data[0];
        u8 monPalNum = sprite->data[2];
        u32 selectedPalettes = (u16)sprite->data[3] | ((u16)sprite->data[4] << 16);

        if (sprite->subpriority != 0)
            subpriority = sprite->subpriority - 1;
        else
            subpriority = 0;
        StartSpriteAnim(sprite, 1);
        AnimateBallOpenParticlesForPokeball(sprite->x, sprite->y - 5, sprite->oam.priority, subpriority);
        sprite->data[1] = LaunchBallFadeMonTaskForPokeball(1, monPalNum, selectedPalettes);
        sprite->callback = sub_804748C;
        gSprites[spriteId].invisible = FALSE;
        StartSpriteAffineAnim(&gSprites[spriteId], 1);
        AnimateSprite(&gSprites[spriteId]);
        gSprites[spriteId].data[1] = 0x1000;
        sprite->data[7] = 0;
    }
    else
    {
        sprite->data[1]--;
    }
}

static void sub_804748C(struct Sprite *sprite)
{
    bool8 emergeAnimFinished = FALSE;
    bool8 atFinalPosition = FALSE;
    u8 monSpriteId = sprite->data[0];
    u16 x;
    u16 y;

    if (sprite->animEnded)
        sprite->invisible = TRUE;
    if (gSprites[monSpriteId].affineAnimEnded)
    {
        StartSpriteAffineAnim(&gSprites[monSpriteId], 0);
        emergeAnimFinished = TRUE;
    }
    x = (sprite->data[5] - sprite->x) * sprite->data[7] / 128 + sprite->x;
    y = (sprite->data[6] - sprite->y) * sprite->data[7] / 128 + sprite->y;
    gSprites[monSpriteId].x = x;
    gSprites[monSpriteId].y = y;
    if (sprite->data[7] < 128)
    {
        s16 sine = -(gSineTable[(u8)sprite->data[7]] / 8);

        sprite->data[7] += 4;
        gSprites[monSpriteId].x2 = sine;
        gSprites[monSpriteId].y2 = sine;
    }
    else
    {
        gSprites[monSpriteId].x = sprite->data[5];
        gSprites[monSpriteId].y = sprite->data[6];
        gSprites[monSpriteId].x2 = 0;
        gSprites[monSpriteId].y2 = 0;
        atFinalPosition = TRUE;
    }
    if (sprite->animEnded && emergeAnimFinished && atFinalPosition)
        DestroySpriteAndFreeResources(sprite);
}

u8 CreateTradePokeballSprite(u8 monSpriteId, u8 monPalNum, u8 x, u8 y, u8 oamPriority, u8 subPriority, u8 delay, u32 fadePalettes)
{
    u8 spriteId;

    LoadCompressedObjectPic(&gBallSpriteSheets[0]);
    LoadCompressedObjectPalette(&gBallSpritePalettes[0]);
    spriteId = CreateSprite(&gBallSpriteTemplates[0], x, y, subPriority);
    gSprites[spriteId].data[0] = monSpriteId;
    gSprites[spriteId].data[1] = delay;
    gSprites[spriteId].data[2] = monPalNum;
    gSprites[spriteId].data[3] = fadePalettes;
    gSprites[spriteId].data[4] = fadePalettes >> 16;
    gSprites[spriteId].oam.priority = oamPriority;
    gSprites[spriteId].callback = SpriteCB_TradePokeball;
    return spriteId;
}

static void SpriteCB_TradePokeball(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        u8 subpriority;
        u8 monSpriteId = sprite->data[0];
        u8 monPalNum = sprite->data[2];
        u32 selectedPalettes = (u16)sprite->data[3] | ((u16)sprite->data[4] << 16);

        if (sprite->subpriority != 0)
            subpriority = sprite->subpriority - 1;
        else
            subpriority = 0;
        StartSpriteAnim(sprite, 1);
        AnimateBallOpenParticlesForPokeball(sprite->x, sprite->y - 5, sprite->oam.priority, subpriority);
        sprite->data[1] = LaunchBallFadeMonTaskForPokeball(1, monPalNum, selectedPalettes);
        sprite->callback = SpriteCB_TradePokeballSendOff;
        StartSpriteAffineAnim(&gSprites[monSpriteId], 2);
        AnimateSprite(&gSprites[monSpriteId]);
        gSprites[monSpriteId].data[1] = 0;
    }
    else
    {
        sprite->data[1]--;
    }
}

static void SpriteCB_TradePokeballSendOff(struct Sprite *sprite)
{
    u8 monSpriteId;

    sprite->data[5]++;
    if (sprite->data[5] == 11)
        PlaySE(SE_BALL_TRADE);
    monSpriteId = sprite->data[0];
    if (gSprites[monSpriteId].affineAnimEnded)
    {
        StartSpriteAnim(sprite, 2);
        gSprites[monSpriteId].invisible = TRUE;
        sprite->data[5] = 0;
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

static void UNUSED DestroySpriteAndFreeResources_Ball(struct Sprite *sprite)
{
    DestroySpriteAndFreeResources(sprite);
}

void StartHealthboxSlideIn(u8 battler)
{
    struct Sprite *healthboxSprite = &gSprites[gHealthboxSpriteIds[battler]];

    healthboxSprite->data[0] = 5;
    healthboxSprite->data[1] = 0;
    healthboxSprite->x2 = 0x73;
    healthboxSprite->y2 = 0;
    healthboxSprite->callback = SpriteCB_HealthboxSlideIn;
    if (GetBattlerSide(battler) != 0)
    {
        healthboxSprite->data[0] = -healthboxSprite->data[0];
        healthboxSprite->data[1] = -healthboxSprite->data[1];
        healthboxSprite->x2 = -healthboxSprite->x2;
        healthboxSprite->y2 = -healthboxSprite->y2;
    }
    gSprites[healthboxSprite->data[5]].callback(&gSprites[healthboxSprite->data[5]]);
    if (GetBattlerPosition(battler) == 2)
        healthboxSprite->callback = SpriteCB_HealthboxSlideInDelayed;
}

static void SpriteCB_HealthboxSlideInDelayed(struct Sprite *sprite)
{
    sprite->data[1]++;
    if (sprite->data[1] == 20)
    {
        sprite->data[1] = 0;
        sprite->callback = SpriteCB_HealthboxSlideIn;
    }
}

static void SpriteCB_HealthboxSlideIn(struct Sprite *sprite)
{
    sprite->x2 -= sprite->data[0];
    sprite->y2 -= sprite->data[1];
    if (sprite->x2 == 0 && sprite->y2 == 0)
        sprite->callback = SpriteCallbackDummy;
}

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
    u8 r1 = sprite->data[1];

    gSprites[r1].y2 = sprite->data[0];
    sprite->data[0] = -sprite->data[0];
    sprite->data[2]++;
    if (sprite->data[2] == 21)
    {
        gSprites[r1].x2 = 0;
        gSprites[r1].y2 = 0;
        DestroySprite(sprite);
    }
}

void LoadBallGfx(u8 ballId)
{
    u16 tileStart;

    if (GetSpriteTileStartByTag(gBallSpriteSheets[ballId].tag) == 0xFFFF)
    {
        LoadCompressedObjectPic(&gBallSpriteSheets[ballId]);
        LoadCompressedObjectPalette(&gBallSpritePalettes[ballId]);
    }

    switch (ballId)
    {
    case 6:
    case 10:
    case 11:
        break;
    default:
        tileStart = GetSpriteTileStartByTag(gBallSpriteSheets[ballId].tag);
        LZDecompressVram(gOpenPokeballGfx, (void *)(VRAM + 0x10100 + tileStart * 32));
        break;
    }
}

void FreeBallGfx(u8 ballId)
{
    FreeSpriteTilesByTag(gBallSpriteSheets[ballId].tag);
    FreeSpritePaletteByTag(gBallSpritePalettes[ballId].tag);
}

static u16 GetBattlerPokeballItemId(u8 battler)
{
    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
        return GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);
    else
        return GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_POKEBALL);
}
