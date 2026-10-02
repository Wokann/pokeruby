#include "constants/battle_anim.h"
	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
sAnim_FlickeringShrinkOrb:: @ 8393160
	obj_image_anim_frame 0, 15
	obj_image_anim_jump 0

	.align 2
sAnims_FlickeringShrinkOrb:: @ 8393168
	.4byte sAnim_FlickeringShrinkOrb

	.align 2
sAffineAnim_FlickeringShrinkOrb:: @ 839316C
	obj_rot_scal_anim_frame 0x60, 0x60, 0, 0
	obj_rot_scal_anim_frame 0x2, 0x2, 0, 1
	obj_rot_scal_anim_jump 1

	.align 2
sAffineAnims_FlickeringShrinkOrb:: @ 8393184
	.4byte sAffineAnim_FlickeringShrinkOrb

	.align 2
sFlickeringShrinkOrbSpriteTemplate:: @ 8393188
	spr_template ANIM_TAG_ORB, ANIM_TAG_ORB, gOamData_AffineDouble_ObjNormal_16x16, sAnims_FlickeringShrinkOrb, NULL, sAffineAnims_FlickeringShrinkOrb, AnimTranslateLinearAndFlicker_Flipped
