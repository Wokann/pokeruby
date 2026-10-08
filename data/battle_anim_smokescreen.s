	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
sSmokescreenImpactSpriteSheet:: @ 81FAEA4
	obj_tiles gSmokescreenImpactTiles, 0x180, 55019

	.align 2
sSmokescreenImpactSpritePalette:: @ 81FAEAC
	obj_pal gSmokescreenImpactPalette, 55019

	.align 2
sOamData_SmokescreenImpact:: @ 81FAEB4
	.2byte 0x0000
	.2byte 0x4000
	.2byte 0x0400

	.align 2
sAnim_SmokescreenImpact_0:: @ 81FAEBC
	obj_image_anim_frame 0, 4
	obj_image_anim_frame 4, 4
	obj_image_anim_frame 8, 4
	obj_image_anim_end

	.align 2
sAnim_SmokescreenImpact_1:: @ 81FAECC
	obj_image_anim_frame 0, 4, OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 4, 4, OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 8, 4, OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_end

	.align 2
sAnim_SmokescreenImpact_2:: @ 81FAEDC
	obj_image_anim_frame 0, 4, OBJ_IMAGE_ANIM_V_FLIP
	obj_image_anim_frame 4, 4, OBJ_IMAGE_ANIM_V_FLIP
	obj_image_anim_frame 8, 4, OBJ_IMAGE_ANIM_V_FLIP
	obj_image_anim_end

	.align 2
sAnim_SmokescreenImpact_3:: @ 81FAEEC
	obj_image_anim_frame 0, 4, OBJ_IMAGE_ANIM_V_FLIP | OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 4, 4, OBJ_IMAGE_ANIM_V_FLIP | OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 8, 4, OBJ_IMAGE_ANIM_V_FLIP | OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_end

	.align 2
sAnims_SmokescreenImpact:: @ 81FAEFC
	.4byte sAnim_SmokescreenImpact_0
	.4byte sAnim_SmokescreenImpact_1
	.4byte sAnim_SmokescreenImpact_2
	.4byte sAnim_SmokescreenImpact_3

	.align 2
sSmokescreenImpactSpriteTemplate:: @ 81FAF0C
	spr_template 55019, 55019, sOamData_SmokescreenImpact, sAnims_SmokescreenImpact, NULL, gDummySpriteAffineAnimTable, SpriteCB_SmokescreenImpact
