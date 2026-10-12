	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
gAnim_FlickeringOrb:: @ 8393054
	obj_image_anim_frame 0, 3
	obj_image_anim_frame 4, 3
	obj_image_anim_frame 8, 3
	obj_image_anim_frame 12, 3
	obj_image_anim_jump 0

	.align 2
gAnims_FlickeringOrb:: @ 8393068
	.4byte gAnim_FlickeringOrb

	.align 2
gFlickeringOrbSpriteTemplate:: @ 839306C
	spr_template 10004, 10004, gOamData_AffineOff_ObjNormal_16x16, gAnims_FlickeringOrb, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker

	.align 2
gFlickeringOrbFlippedSpriteTemplate:: @ 8393084
	spr_template 10004, 10004, gOamData_AffineOff_ObjNormal_16x16, gAnims_FlickeringOrb, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker_Flipped
