#include "constants/battle_anim.h"
	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
sFlickeringFootSpriteTemplate:: @ 839310C
	spr_template ANIM_TAG_MONSTER_FOOT, ANIM_TAG_MONSTER_FOOT, gOamData_AffineOff_ObjNormal_32x32, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker
