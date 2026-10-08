	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2

gInputFuncs:: @ 83BBBD4
	.4byte InBoxInput_Normal, 0
	.4byte HandleInput_InParty, 1
	.4byte HandleInput_OnBox, 2
	.4byte HandleInput_OnButtons, 3
	.4byte NULL, 0

	.align 2
gHandCursorSpriteSheets:: @ 83BBBFC
	obj_tiles gHandCursor_Gfx, 0x800, 0
	obj_tiles gHandCursorShadow_Gfx, 0x80, 1
	null_obj_tiles

	.align 2
gHandCursorSpritePalettes:: @ 83BBC14
	obj_pal HandCursorPalette, 56006
	obj_pal HandCursorAltPalette, 56017
	null_obj_pal

	.align 2
sOamData_Cursor: @ 83BBC2C
	.2byte 0x0000
	.2byte 0x8000
	.2byte 0x0400

	.align 2
sOamData_CursorShadow: @ 83BBC34
	.2byte 0x0000
	.2byte 0x4000
	.2byte 0x0800

	.align 2
sAnim_Cursor_Bouncing: @ 83BBC3C
	obj_image_anim_frame 0, 30
	obj_image_anim_frame 16, 30
	obj_image_anim_jump 0

	.align 2
sAnim_Cursor_Still: @ 83BBC48
	obj_image_anim_frame 0, 5
	obj_image_anim_end

	.align 2
sAnim_Cursor_Open: @ 83BBC50
	obj_image_anim_frame 32, 5
	obj_image_anim_end

	.align 2
sAnim_Cursor_Fist: @ 83BBC58
	obj_image_anim_frame 48, 5
	obj_image_anim_end

	.align 2
sAnims_Cursor: @ 83BBC60
	.4byte sAnim_Cursor_Bouncing
	.4byte sAnim_Cursor_Still
	.4byte sAnim_Cursor_Open
	.4byte sAnim_Cursor_Fist

	.align 2
gSpriteTemplate_Cursor:: @ 83BBC70
	spr_template 0, 56006, sOamData_Cursor, sAnims_Cursor, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy

	.align 2
gSpriteTemplate_CursorShadow:: @ 83BBC88
	spr_template 1, 56006, sOamData_CursorShadow, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_CursorShadow

.if DEBUG
.if DEBUG_FIX
gUnknown_Debug_0x83E6268:: @ 0x83E6268
@ TODO: translations
	.string "SET$"
.else @ !DEBUG_FIX
gUnknown_Debug_0x83E6268:: @ 0x83E6268
	.string "セットする$"
.endif @ !DEBUG_FIX
.endif

	.align 2
gMenuTexts:: @ 83BBCA0
	.4byte PCText_Cancel2
	.4byte gPCText_Store
	.4byte gPCText_Withdraw
	.4byte gPCText_Move
	.4byte gPCText_Shift
	.4byte gPCText_Place
	.4byte gPCText_Summary
	.4byte gPCText_Release
	.4byte gPCText_Mark
	.4byte gPCText_Jump
	.4byte gPCText_Wallpaper
	.4byte gPCText_Name
	.4byte gPCText_Scenery1
	.4byte gPCText_Scenery2
	.4byte gPCText_Scenery3
	.4byte gPCText_Etcetera
	.4byte gPCText_Forest
	.4byte gPCText_City
	.4byte gPCText_Desert
	.4byte gPCText_Savanna
	.4byte gPCText_Crag
	.4byte gPCText_Volcano
	.4byte gPCText_Snow
	.4byte gPCText_Cave
	.4byte gPCText_Beach
	.4byte gPCText_Seafloor
	.4byte gPCText_River
	.4byte gPCText_Sky
	.4byte gPCText_PolkaDot
	.4byte gPCText_Pokecenter
	.4byte gPCText_Machine
	.4byte gPCText_Simple
.if DEBUG
	.4byte gUnknown_Debug_0x83E6268
.endif
