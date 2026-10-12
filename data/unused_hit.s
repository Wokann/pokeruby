	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
gAnim_FlickeringImpact_0:: @ 8393124
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
gAnim_FlickeringImpact_1:: @ 839312C
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
gAnim_FlickeringImpact_2:: @ 8393134
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
gAnims_FlickeringImpact:: @ 839313C
	.4byte gAnim_FlickeringImpact_0
	.4byte gAnim_FlickeringImpact_1
	.4byte gAnim_FlickeringImpact_2

	.align 2
gFlickeringImpactSpriteTemplate:: @ 8393148
	spr_template 10135, 10135, gOamData_AffineOff_ObjNormal_32x32, gAnims_FlickeringImpact, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker
