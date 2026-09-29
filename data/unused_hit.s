#include "constants/battle_anim.h"
	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
sAnim_FlickeringImpact_0:: @ 8393124
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
sAnim_FlickeringImpact_1:: @ 839312C
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
sAnim_FlickeringImpact_2:: @ 8393134
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
sAnims_FlickeringImpact:: @ 839313C
	.4byte sAnim_FlickeringImpact_0
	.4byte sAnim_FlickeringImpact_1
	.4byte sAnim_FlickeringImpact_2

	.align 2
sFlickeringImpactSpriteTemplate:: @ 8393148
	spr_template ANIM_TAG_IMPACT, ANIM_TAG_IMPACT, gOamData_AffineOff_ObjNormal_32x32, sAnims_FlickeringImpact, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker
