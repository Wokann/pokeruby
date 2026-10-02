#include "constants/global.h"
#include "constants/layouts.h"
#include "constants/map_types.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"
#include "constants/weather.h"
// These are defined as assembler symbols in constants/misc_constants.inc.
#undef POKEMON_NAME_LENGTH
#undef MOVE_NAME_LENGTH
	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.include "data/layouts/layouts.inc"
	.include "data/layouts/layouts_table.inc"
	.include "data/maps/headers.inc"
	.include "data/maps/groups.inc"
	.include "data/maps/connections.inc"
