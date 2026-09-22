#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern const union AffineAnimCmd gFallingBagAffineAnimCmds1[];
extern const union AffineAnimCmd gFallingBagAffineAnimCmds2[];
extern const union AnimCmd *const gFallingBagAnimTable[];

static void AnimTrickBag(struct Sprite *sprite);
static void AnimTrickBag_Step1(struct Sprite *sprite);
static void AnimTrickBag_Step2(struct Sprite *sprite);
static void AnimTrickBag_Step3(struct Sprite *sprite);

// switch (makes an item and circles it from side to side on the field.)
// Used in Trick.


const union AffineAnimCmd gTrickBagAffineAnimCmds1[] = {
    AFFINEANIMCMD_FRAME(0, 0, 0, 3),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gTrickBagAffineAnimCmds2[] = {
    AFFINEANIMCMD_FRAME(0, -10, 0, 3),
    AFFINEANIMCMD_FRAME(0, -6, 0, 3),
    AFFINEANIMCMD_FRAME(0, -2, 0, 3),
    AFFINEANIMCMD_FRAME(0, 0, 0, 3),
    AFFINEANIMCMD_FRAME(0, 2, 0, 3),
    AFFINEANIMCMD_FRAME(0, 6, 0, 3),
    AFFINEANIMCMD_FRAME(0, 10, 0, 3),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gTrickBagAffineAnimTable[] = {
    gTrickBagAffineAnimCmds1,
    gTrickBagAffineAnimCmds2,
    gFallingBagAffineAnimCmds1,
    gFallingBagAffineAnimCmds2,
};

const struct SpriteTemplate gTrickBagSpriteTemplate =
{
    .tileTag = ANIM_TAG_ITEM_BAG,
    .paletteTag = ANIM_TAG_ITEM_BAG,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gFallingBagAnimTable,
    .images = NULL,
    .affineAnims = gTrickBagAffineAnimTable,
    .callback = AnimTrickBag,
};

const s8 gTrickBagCoordinates[][3] =
{
    {5, 24,   1},
    {0,  4,   0},
    {8, 16,  -1},
    {0,  2,   0},
    {8, 16,   1},
    {0,  2,   0},
    {8, 16,   1},
    {0,  2,   0},
    {8, 16,   1},
    {0, 16,   0},
    {0,  0, 127},
};

static void AnimTrickBag(struct Sprite *sprite)
{
    int a;
    int b;

    if (sprite->data[0] == 0)
    {
        if (!IsContest())
        {
            sprite->data[1] = gBattleAnimArgs[1];
            sprite->x = 120;
        }
        else
        {
            a = gBattleAnimArgs[1] - 32;
            if (a < 0)
                b = gBattleAnimArgs[1] + 0xDF;
            else
                b = a;

            sprite->data[1] = a - ((b >> 8) << 8);
            sprite->x = 70;
        }

        sprite->y = gBattleAnimArgs[0];
        sprite->data[2] = gBattleAnimArgs[0];
        sprite->data[4] = 20;
        sprite->x2 = Cos(sprite->data[1], 60);
        sprite->y2 = Sin(sprite->data[1], 20);
        sprite->callback = AnimTrickBag_Step1;
        if (sprite->data[1] > 0 && sprite->data[1] < 0xC0)
            sprite->subpriority = 31;
        else
            sprite->subpriority = 29;
    }
}

static void AnimTrickBag_Step1(struct Sprite *sprite)
{
    switch (sprite->data[3])
    {
    case 0:
        if (sprite->data[2] > 0x4E)
        {
            sprite->data[3] = 1;
            StartSpriteAffineAnim(sprite, 1);
            break;
        }
        else
        {
            sprite->data[2] += sprite->data[4] / 10;
            sprite->data[4] += 3;
            sprite->y = sprite->data[2];
            break;
        }
        break;
    case 1:
        if (sprite->data[3] && sprite->affineAnimEnded)
        {
            sprite->data[0] = 0;
            sprite->data[2] = 0;
            sprite->callback = AnimTrickBag_Step2;
        }
        break;
    }
}

static void AnimTrickBag_Step2(struct Sprite *sprite)
{
    if (sprite->data[2] == gTrickBagCoordinates[sprite->data[0]][1])
	{
	    if (gTrickBagCoordinates[sprite->data[0]][2] == 0x7F)
		{
		    sprite->data[0] = 0;
			sprite->callback = AnimTrickBag_Step3;
		}

		sprite->data[2] = 0;
		sprite->data[0]++;
	}
	else
	{
	    sprite->data[2]++;
		sprite->data[1] = (gTrickBagCoordinates[sprite->data[0]][0] * gTrickBagCoordinates[sprite->data[0]][2] + sprite->data[1]) & 0xFF;
		if (!IsContest())
		{
		    if ((u16)(sprite->data[1] - 1) <= 0xBE)
            {
			    sprite->subpriority = 31;
			}
			else
			{
			    sprite->subpriority = 29;
		    }
		}

		sprite->x2 = Cos(sprite->data[1], 60);
		sprite->y2 = Sin(sprite->data[1], 20);
	}
}

static void AnimTrickBag_Step3(struct Sprite *sprite)
{
    if (sprite->data[0] > 20)
        DestroyAnimSprite(sprite);

	sprite->invisible = sprite->data[0] % 2;
	sprite->data[0]++;
}
