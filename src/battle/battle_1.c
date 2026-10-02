#include "global.h"
#include "data2.h"
#include "battle_setup.h"
#include "battle.h"
#include "graphics.h"
#include "gba/types.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/opponents.h"
#include "constants/trainers.h"

#include "../data/trainer_graphics/back_pic_tables.h"
#include "../data/pokemon_graphics/enemy_mon_elevation.h"

#include "../data/trainer_parties.h"

#if ENGLISH
#include "../data/text/trainer_class_names_en.h"
#include "../data/trainers_en.h"
#include "../data/text/species_names_en.h"
#include "../data/text/move_names_en.h"
#elif GERMAN
#include "../data/text/trainer_class_names_de.h"
#include "../data/trainers_de.h"
#include "../data/text/species_names_de.h"
#include "../data/text/move_names_de.h"
#endif

const u32 gUnusedBattleData = 0x101;
