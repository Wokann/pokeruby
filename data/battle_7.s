	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
gSpriteSheet_EnemyShadow:: @ 81FAF24
	obj_tiles gEnemyMonShadow_Gfx, 128, 0xD759

	.align 2
sOamData_EnemyShadow:: @ 81FAF2C
	oam_data 0, OAM_AFFINE_NONE, OAM_OBJ_NORMAL, OAM_MOSAIC_OFF, OAM_4BPP, OAM_SIZE_32x8, 0, 0, 0, 3, 0

	.align 2
gSpriteTemplate_EnemyShadow:: @ 81FAF34
	spr_template 55129, 55039, sOamData_EnemyShadow, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_SetInvisible

	.align 2
gMonSpriteGfxBufferPtrs:: @ 81FAF4C
	.4byte gSharedMem + 0x8000
	.4byte gSharedMem + 0xA000
	.4byte gSharedMem + 0xC000
	.4byte gSharedMem + 0xE000
