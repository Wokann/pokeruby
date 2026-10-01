#include "constants/battle_anim.h"
	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
sAnim_FlickeringOrb:: @ 8393054
	obj_image_anim_frame 0, 3
	obj_image_anim_frame 4, 3
	obj_image_anim_frame 8, 3
	obj_image_anim_frame 12, 3
	obj_image_anim_jump 0

	.align 2
sAnims_FlickeringOrb:: @ 8393068
	.4byte sAnim_FlickeringOrb

	.align 2
sFlickeringOrbSpriteTemplate:: @ 839306C
	spr_template ANIM_TAG_ORB, ANIM_TAG_ORB, gOamData_AffineOff_ObjNormal_16x16, sAnims_FlickeringOrb, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker

	.align 2
sFlickeringOrbFlippedSpriteTemplate:: @ 8393084
	spr_template ANIM_TAG_ORB, ANIM_TAG_ORB, gOamData_AffineOff_ObjNormal_16x16, sAnims_FlickeringOrb, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker_Flipped
