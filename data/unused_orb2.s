	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
gAnim_FlickeringShrinkOrb:: @ 8393160
	obj_image_anim_frame 0, 15
	obj_image_anim_jump 0

	.align 2
gAnims_FlickeringShrinkOrb:: @ 8393168
	.4byte gAnim_FlickeringShrinkOrb

	.align 2
gAffineAnim_FlickeringShrinkOrb:: @ 839316C
	obj_rot_scal_anim_frame 0x60, 0x60, 0, 0
	obj_rot_scal_anim_frame 0x2, 0x2, 0, 1
	obj_rot_scal_anim_jump 1

	.align 2
gAffineAnims_FlickeringShrinkOrb:: @ 8393184
	.4byte gAffineAnim_FlickeringShrinkOrb

	.align 2
gFlickeringShrinkOrbSpriteTemplate:: @ 8393188
	spr_template 10004, 10004, gOamData_AffineDouble_ObjNormal_16x16, gAnims_FlickeringShrinkOrb, NULL, gAffineAnims_FlickeringShrinkOrb, AnimTranslateLinearAndFlicker_Flipped
