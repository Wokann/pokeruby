	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
sAnim_SpinningSparkle:: @ 83930D8
	obj_image_anim_frame 0, 3
	obj_image_anim_frame 16, 3
	obj_image_anim_frame 32, 3
	obj_image_anim_frame 48, 3
	obj_image_anim_frame 64, 3
	obj_image_anim_end

	.align 2
sAnims_SpinningSparkle:: @ 83930F0
	.4byte sAnim_SpinningSparkle

	.align 2
gSpinningSparkleSpriteTemplate:: @ 83930F4
	spr_template 10071, 10071, gOamData_AffineOff_ObjNormal_32x32, sAnims_SpinningSparkle, NULL, gDummySpriteAffineAnimTable, AnimSpinningSparkle
