	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2

@ Assembly mirror of PSS_CURSOR_AREA_* in include/pokemon_storage_system.h.
.equiv PSS_CURSOR_AREA_IN_BOX,    0
.equiv PSS_CURSOR_AREA_IN_PARTY,  1
.equiv PSS_CURSOR_AREA_BOX_TITLE, 2
.equiv PSS_CURSOR_AREA_BUTTONS,   3

@ Tags used by the storage cursor sheets, palettes, and templates.
.equiv PSS_GFXTAG_CURSOR,        0
.equiv PSS_GFXTAG_CURSOR_SHADOW, 1
.equiv PSS_PALTAG_CURSOR,        0xDAC6
.equiv PSS_PALTAG_CURSOR_ALT,    0xDAD1

gPokeStorageInputHandlers:: @ 83BBBD4
	poke_storage_input_handler InBoxInput_Normal, PSS_CURSOR_AREA_IN_BOX
	poke_storage_input_handler HandleInput_InParty, PSS_CURSOR_AREA_IN_PARTY
	poke_storage_input_handler HandleInput_OnBox, PSS_CURSOR_AREA_BOX_TITLE
	poke_storage_input_handler HandleInput_OnButtons, PSS_CURSOR_AREA_BUTTONS
	poke_storage_input_handler_end

	.align 2
gPokeStorageCursorSpriteSheets:: @ 83BBBFC
	obj_tiles gHandCursorTiles, 0x800, PSS_GFXTAG_CURSOR
	obj_tiles gHandCursorShadowTiles, 0x80, PSS_GFXTAG_CURSOR_SHADOW
	null_obj_tiles

	.align 2
gPokeStorageCursorSpritePalettes:: @ 83BBC14
	obj_pal gHandCursorPalette, PSS_PALTAG_CURSOR
	obj_pal gHandCursorAltPalette, PSS_PALTAG_CURSOR_ALT
	null_obj_pal

	.align 2
gOamData_PokeStorageCursor:: @ 83BBC2C
	oam_data 0, OAM_AFFINE_NONE, OAM_OBJ_NORMAL, OAM_MOSAIC_OFF, OAM_4BPP, OAM_SIZE_32x32, 0, 0, 0, 1, 0

	.align 2
gOamData_PokeStorageCursorShadow:: @ 83BBC34
	oam_data 0, OAM_AFFINE_NONE, OAM_OBJ_NORMAL, OAM_MOSAIC_OFF, OAM_4BPP, OAM_SIZE_16x16, 0, 0, 0, 2, 0

	.align 2
gAnim_PokeStorageCursorBouncing:: @ 83BBC3C
	obj_image_anim_frame 0, 30
	obj_image_anim_frame 16, 30
	obj_image_anim_jump 0

	.align 2
gAnim_PokeStorageCursorStill:: @ 83BBC48
	obj_image_anim_frame 0, 5
	obj_image_anim_end

	.align 2
gAnim_PokeStorageCursorOpen:: @ 83BBC50
	obj_image_anim_frame 32, 5
	obj_image_anim_end

	.align 2
gAnim_PokeStorageCursorFist:: @ 83BBC58
	obj_image_anim_frame 48, 5
	obj_image_anim_end

	.align 2
gAnims_PokeStorageCursor:: @ 83BBC60
	.4byte gAnim_PokeStorageCursorBouncing
	.4byte gAnim_PokeStorageCursorStill
	.4byte gAnim_PokeStorageCursorOpen
	.4byte gAnim_PokeStorageCursorFist

	.align 2
gSpriteTemplate_PokeStorageCursor:: @ 83BBC70
	spr_template PSS_GFXTAG_CURSOR, PSS_PALTAG_CURSOR, gOamData_PokeStorageCursor, gAnims_PokeStorageCursor, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy

	.align 2
gSpriteTemplate_PokeStorageCursorShadow:: @ 83BBC88
	spr_template PSS_GFXTAG_CURSOR_SHADOW, PSS_PALTAG_CURSOR, gOamData_PokeStorageCursorShadow, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_CursorShadow

.if DEBUG
.if DEBUG_FIX
gText_DebugPokeStorageSet:: @ 0x83E6268
@ TODO: translations
	.string "SET$"
.else @ !DEBUG_FIX
gText_DebugPokeStorageSet:: @ 0x83E6268
	.string "セットする$"
.endif @ !DEBUG_FIX
.endif

	.align 2
gPokeStorageMenuTexts:: @ 83BBCA0
	.4byte PCText_Cancel2
	.4byte PCText_Deposit
	.4byte PCText_Withdraw
	.4byte PCText_Move
	.4byte PCText_Switch
	.4byte PCText_Place
	.4byte PCText_Summary
	.4byte PCText_Release
	.4byte PCText_Mark
	.4byte PCText_Jump
	.4byte PCText_Wallpaper
	.4byte PCText_Name
	.4byte PCText_Scenery1
	.4byte PCText_Scenery2
	.4byte PCText_Scenery3
	.4byte PCText_Etc
	.4byte PCText_Forest
	.4byte PCText_City
	.4byte PCText_Desert
	.4byte PCText_Savanna
	.4byte PCText_Crag
	.4byte PCText_Volcano
	.4byte PCText_Snow
	.4byte PCText_Cave
	.4byte PCText_Beach
	.4byte PCText_Seafloor
	.4byte PCText_River
	.4byte PCText_Sky
	.4byte PCText_Polka
	.4byte PCText_PokeCenter
	.4byte PCText_Machine
	.4byte PCText_Plain
.if DEBUG
	.4byte gText_DebugPokeStorageSet
.endif
