	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
gMonSpriteGfxBufferPtrs:: @ 81FAF4C
	.4byte gSharedMem + 0x8000
	.4byte gSharedMem + 0xA000
	.4byte gSharedMem + 0xC000
	.4byte gSharedMem + 0xE000
