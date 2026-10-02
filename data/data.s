@ the second big chunk of data

#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"
	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata


@ 81E7C74
	.include "data/pokemon_graphics/front_pic_coordinates.inc"

@ 81E8354
	.include "data/pokemon_graphics/front_pic_table.inc"

@ 81E9114
	.include "data/pokemon_graphics/back_pic_coordinates.inc"

@ 81E97F4
	.include "data/pokemon_graphics/back_pic_table.inc"

@ 81EA5B4
	.include "data/pokemon_graphics/palette_table.inc"

@ 81EB374
	.include "data/pokemon_graphics/shiny_palette_table.inc"

@ 81EC134
	.include "data/trainer_graphics/front_pic_anims.inc"

@ 81EC3F0
	.include "data/trainer_graphics/front_pic_coordinates.inc"

@ 81EC53C
	.include "data/trainer_graphics/front_pic_table.inc"

@ 81EC7D4
	.include "data/trainer_graphics/front_pic_palette_table.inc"

@ 81ECA6C
	.include "data/trainer_graphics/back_pic_anims.inc"
