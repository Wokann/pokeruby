	.include "include/macros.inc"
	.include "constants/constants.inc"

	.equ TAG_SMOKESCREEN, 55019

	.section .rodata

	.align 2
gSmokescreenImpactSpriteSheet:: @ 81FAEA4
	obj_tiles gSmokescreenImpactTiles, 0x180, TAG_SMOKESCREEN

	.align 2
gSmokescreenImpactSpritePalette:: @ 81FAEAC
	obj_pal gSmokescreenImpactPalette, TAG_SMOKESCREEN

	.align 2
gOamData_SmokescreenImpact:: @ 81FAEB4
	.2byte 0x0000
	.2byte 0x4000
	.2byte 0x0400

	.align 2
gAnim_SmokescreenImpact_0:: @ 81FAEBC
	obj_image_anim_frame 0, 4
	obj_image_anim_frame 4, 4
	obj_image_anim_frame 8, 4
	obj_image_anim_end

	.align 2
gAnim_SmokescreenImpact_1:: @ 81FAECC
	obj_image_anim_frame 0, 4, OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 4, 4, OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 8, 4, OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_end

	.align 2
gAnim_SmokescreenImpact_2:: @ 81FAEDC
	obj_image_anim_frame 0, 4, OBJ_IMAGE_ANIM_V_FLIP
	obj_image_anim_frame 4, 4, OBJ_IMAGE_ANIM_V_FLIP
	obj_image_anim_frame 8, 4, OBJ_IMAGE_ANIM_V_FLIP
	obj_image_anim_end

	.align 2
gAnim_SmokescreenImpact_3:: @ 81FAEEC
	obj_image_anim_frame 0, 4, OBJ_IMAGE_ANIM_V_FLIP | OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 4, 4, OBJ_IMAGE_ANIM_V_FLIP | OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_frame 8, 4, OBJ_IMAGE_ANIM_V_FLIP | OBJ_IMAGE_ANIM_H_FLIP
	obj_image_anim_end

	.align 2
gAnims_SmokescreenImpact:: @ 81FAEFC
	.4byte gAnim_SmokescreenImpact_0
	.4byte gAnim_SmokescreenImpact_1
	.4byte gAnim_SmokescreenImpact_2
	.4byte gAnim_SmokescreenImpact_3

	.align 2
gSmokescreenImpactSpriteTemplate:: @ 81FAF0C
	spr_template TAG_SMOKESCREEN, TAG_SMOKESCREEN, gOamData_SmokescreenImpact, gAnims_SmokescreenImpact, NULL, gDummySpriteAffineAnimTable, SpriteCB_SmokescreenImpact
