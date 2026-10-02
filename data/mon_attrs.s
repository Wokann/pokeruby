@ the second big chunk of data

#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
gBattlerPicTable_PlayerLeft:: @ 81E7A10
	obj_frame_tiles gSharedMem + 0x8000, 0x800
	obj_frame_tiles gSharedMem + 0x8800, 0x800
	obj_frame_tiles gSharedMem + 0x9000, 0x800
	obj_frame_tiles gSharedMem + 0x9800, 0x800

	.align 2
gBattlerPicTable_OpponentLeft:: @ 81E7A30
	obj_frame_tiles gSharedMem + 0xA000, 0x800
	obj_frame_tiles gSharedMem + 0xA800, 0x800
	obj_frame_tiles gSharedMem + 0xB000, 0x800
	obj_frame_tiles gSharedMem + 0xB800, 0x800

	.align 2
gBattlerPicTable_PlayerRight:: @ 81E7A50
	obj_frame_tiles gSharedMem + 0xC000, 0x800
	obj_frame_tiles gSharedMem + 0xC800, 0x800
	obj_frame_tiles gSharedMem + 0xD000, 0x800
	obj_frame_tiles gSharedMem + 0xD800, 0x800

	.align 2
gBattlerPicTable_OpponentRight:: @ 81E7A70
	obj_frame_tiles gSharedMem + 0xE000, 0x800
	obj_frame_tiles gSharedMem + 0xE800, 0x800
	obj_frame_tiles gSharedMem + 0xF000, 0x800
	obj_frame_tiles gSharedMem + 0xF800, 0x800

	.align 2
sAnim_GeneralFrame0:: @ 81E7A90
	obj_image_anim_frame 0, 0
	obj_image_anim_end

	.align 2
sAnim_GeneralFrame3:: @ 81E7A98
	obj_image_anim_frame 3, 0
	obj_image_anim_end

	.align 2
sAffineAnim_Battler_Normal:: @ 81E7AA0
	obj_rot_scal_anim_frame 0x100, 0x100, 0, 0
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_Flipped:: @ 81E7AB0
	obj_rot_scal_anim_frame 0xFF00, 0x100, 0, 0
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_Emerge:: @ 81E7AC0
	obj_rot_scal_anim_frame 0x28, 0x28, 0, 0
	obj_rot_scal_anim_frame 0x12, 0x12, 0, 12
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_Return:: @ 81E7AD8
	obj_rot_scal_anim_frame 0xFFFE, 0xFFFE, 0, 18
	obj_rot_scal_anim_frame 0xFFF0, 0xFFF0, 0, 15
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_HorizontalSquishLoop:: @ 81E7AF0
	obj_rot_scal_anim_frame 0xA0, 0x100, 0, 0
	obj_rot_scal_anim_frame 0x4, 0x0, 0, 8
	obj_rot_scal_anim_frame 0xFFFC, 0x0, 0, 8
	obj_rot_scal_anim_jump 1

	.align 2
sAffineAnim_Battler_Grow:: @ 81E7B10
	obj_rot_scal_anim_frame 0x2, 0x2, 0, 20
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_Shrink:: @ 81E7B20
	obj_rot_scal_anim_frame 0xFFFE, 0xFFFE, 0, 20
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_BigToSmall:: @ 81E7B30
	obj_rot_scal_anim_frame 0x100, 0x100, 0, 0
	obj_rot_scal_anim_frame 0xFFF0, 0xFFF0, 0, 9
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_GrowLarge:: @ 81E7B48
	obj_rot_scal_anim_frame 0x4, 0x4, 0, 63
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_TipRight:: @ 81E7B58
	obj_rot_scal_anim_frame 0x0, 0x0, -3, 5
	obj_rot_scal_anim_frame 0x0, 0x0, 3, 5
	obj_rot_scal_anim_end

	.align 2
gAffineAnims_BattleSpritePlayerSide:: @ 81E7B70
	.4byte sAffineAnim_Battler_Normal
	.4byte sAffineAnim_Battler_Emerge
	.4byte sAffineAnim_Battler_Return
	.4byte sAffineAnim_Battler_HorizontalSquishLoop
	.4byte sAffineAnim_Battler_Grow
	.4byte sAffineAnim_Battler_Shrink
	.4byte sAffineAnim_Battler_GrowLarge
	.4byte sAffineAnim_Battler_TipRight
	.4byte sAffineAnim_Battler_BigToSmall

	.align 2
sAffineAnim_Battler_SpinShrink:: @ 81E7B94
	obj_rot_scal_anim_frame 0xFFFC, 0xFFFC, 4, 63
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_TipLeft:: @ 81E7BA4
	obj_rot_scal_anim_frame 0x0, 0x0, 3, 5
	obj_rot_scal_anim_frame 0x0, 0x0, -3, 5
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_RotateUpAndBack:: @ 81E7BBC
	obj_rot_scal_anim_frame 0x0, 0x0, -5, 20
	obj_rot_scal_anim_frame 0x0, 0x0, 0, 20
	obj_rot_scal_anim_frame 0x0, 0x0, 5, 20
	obj_rot_scal_anim_end

	.align 2
sAffineAnim_Battler_Spin:: @ 81E7BDC
	obj_rot_scal_anim_frame 0x0, 0x0, 9, 110
	obj_rot_scal_anim_end

	.align 2
gAffineAnims_BattleSpriteOpponentSide:: @ 81E7BEC
	.4byte sAffineAnim_Battler_Normal
	.4byte sAffineAnim_Battler_Emerge
	.4byte sAffineAnim_Battler_Return
	.4byte sAffineAnim_Battler_HorizontalSquishLoop
	.4byte sAffineAnim_Battler_Grow
	.4byte sAffineAnim_Battler_Shrink
	.4byte sAffineAnim_Battler_SpinShrink
	.4byte sAffineAnim_Battler_TipLeft
	.4byte sAffineAnim_Battler_RotateUpAndBack
	.4byte sAffineAnim_Battler_BigToSmall
	.4byte sAffineAnim_Battler_Spin

	.align 2
gAffineAnims_BattleSpriteContest:: @ 81E7C18
	.4byte sAffineAnim_Battler_Flipped
	.4byte sAffineAnim_Battler_Emerge
	.4byte sAffineAnim_Battler_Return
	.4byte sAffineAnim_Battler_HorizontalSquishLoop
	.4byte sAffineAnim_Battler_Grow
	.4byte sAffineAnim_Battler_Shrink
	.4byte sAffineAnim_Battler_SpinShrink
	.4byte sAffineAnim_Battler_TipLeft
	.4byte sAffineAnim_Battler_RotateUpAndBack
	.4byte sAffineAnim_Battler_BigToSmall
	.4byte sAffineAnim_Battler_Spin

	.align 2
sAnim_MonPic_0:: @ 81E7C44
	obj_image_anim_frame 0, 0
	obj_image_anim_end

	.align 2
sAnim_MonPic_1:: @ 81E7C4C
	obj_image_anim_frame 1, 0
	obj_image_anim_end

	.align 2
sAnim_MonPic_2:: @ 81E7C54
	obj_image_anim_frame 2, 0
	obj_image_anim_end

	.align 2
sAnim_MonPic_3:: @ 81E7C5C
	obj_image_anim_frame 3, 0
	obj_image_anim_end

	.align 2
gAnims_MonPic:: @ 81E7C64
	.4byte sAnim_MonPic_0
	.4byte sAnim_MonPic_1
	.4byte sAnim_MonPic_2
	.4byte sAnim_MonPic_3

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

	.align 2
sAnimCmd_Lass_1:: @ 81EC134
	obj_image_anim_frame 0, 7
	obj_image_anim_end

	.align 2
sAnimCmd_Lady_1:: @ 81EC13C
	obj_image_anim_frame 0, 2
	obj_image_anim_end

	.align 2
sAnimCmd_CyclingTriathleteM_1:: @ 81EC144
	obj_image_anim_frame 0, 2
	obj_image_anim_end

	.align 2
sAnims_Brendan:: @ 81EC14C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_May:: @ 81EC150
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Lass:: @ 81EC154
	.4byte sAnim_GeneralFrame0
	.4byte sAnimCmd_Lass_1

	.align 2
sAnims_Youngster:: @ 81EC15C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Hiker:: @ 81EC160
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Beauty:: @ 81EC164
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Fisherman:: @ 81EC168
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Lady:: @ 81EC16C
	.4byte sAnim_GeneralFrame0
	.4byte sAnimCmd_Lady_1

	.align 2
sAnims_CyclingTriathleteM:: @ 81EC174
	.4byte sAnim_GeneralFrame0
	.4byte sAnimCmd_CyclingTriathleteM_1

	.align 2
sAnims_AquaGruntM:: @ 81EC17C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Twins:: @ 81EC180
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SwimmerF:: @ 81EC184
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_BugCatcher:: @ 81EC188
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SchoolKidM:: @ 81EC18C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_RichBoy:: @ 81EC190
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SrAndJr:: @ 81EC194
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_BlackBelt:: @ 81EC198
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Wally:: @ 81EC19C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_AquaGruntF:: @ 81EC1A0
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_TuberF:: @ 81EC1A4
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_HexManiac:: @ 81EC1A8
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PokemonBreederF:: @ 81EC1AC
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_MagmaGruntM:: @ 81EC1B0
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_MagmaGruntF:: @ 81EC1B4
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Interviewer:: @ 81EC1B8
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_TuberM:: @ 81EC1BC
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_YoungCouple:: @ 81EC1C0
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Guitarist:: @ 81EC1C4
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Gentleman:: @ 81EC1C8
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Steven:: @ 81EC1CC
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Maxie:: @ 81EC1D0
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_RunningTriathleteF:: @ 81EC1D4
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_RunningTriathleteM:: @ 81EC1D8
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_BattleGirl:: @ 81EC1DC
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_CyclingTriathleteF:: @ 81EC1E0
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SwimmerM:: @ 81EC1E4
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PokefanF:: @ 81EC1E8
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_ExpertM:: @ 81EC1EC
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_ExpertF:: @ 81EC1F0
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SwimmingTriathleteM:: @ 81EC1F4
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SwimmingTriathleteF:: @ 81EC1F8
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_DragonTamer:: @ 81EC1FC
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_BirdKeeper:: @ 81EC200
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_NinjaBoy:: @ 81EC204
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_ParasolLady:: @ 81EC208
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_BugManiac:: @ 81EC20C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Sailor:: @ 81EC210
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Collector:: @ 81EC214
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PokemonBreederM:: @ 81EC218
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PokemonRangerM:: @ 81EC21C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PokemonRangerF:: @ 81EC220
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_MagmaAdminM:: @ 81EC224
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_MagmaAdminF:: @ 81EC228
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_AromaLady:: @ 81EC22C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_RuinManiac:: @ 81EC230
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_CoolTrainerM:: @ 81EC234
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_CoolTrainerF:: @ 81EC238
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Pokemaniac:: @ 81EC23C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Kindler:: @ 81EC240
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Camper:: @ 81EC244
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Picnicker:: @ 81EC248
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PsychicM:: @ 81EC24C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PsychicF:: @ 81EC250
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SchoolKidF:: @ 81EC254
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_PokefanM:: @ 81EC258
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_SisAndBro:: @ 81EC25C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_OldCouple:: @ 81EC260
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_AquaAdminM:: @ 81EC264
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_AquaAdminF:: @ 81EC268
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_UnusedAqua:: @ 81EC26C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Archie:: @ 81EC270
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Roxanne:: @ 81EC274
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Brawly:: @ 81EC278
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Wattson:: @ 81EC27C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Flannery:: @ 81EC280
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Norman:: @ 81EC284
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Winona:: @ 81EC288
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_TateAndLiza:: @ 81EC28C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Wallace:: @ 81EC290
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Sidney:: @ 81EC294
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Phoebe:: @ 81EC298
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Glacia:: @ 81EC29C
	.4byte sAnim_GeneralFrame0

	.align 2
sAnims_Drake:: @ 81EC2A0
	.4byte sAnim_GeneralFrame0

	.align 2
gTrainerFrontAnimsPtrTable:: @ 81EC2A4
	.4byte sAnims_Brendan @ TRAINER_PIC_BRENDAN
	.4byte sAnims_May @ TRAINER_PIC_MAY
	.4byte sAnims_Lass @ TRAINER_PIC_LASS
	.4byte sAnims_Youngster @ TRAINER_PIC_YOUNGSTER
	.4byte sAnims_Hiker @ TRAINER_PIC_HIKER
	.4byte sAnims_Beauty @ TRAINER_PIC_BEAUTY
	.4byte sAnims_Fisherman @ TRAINER_PIC_FISHERMAN
	.4byte sAnims_Lady @ TRAINER_PIC_LADY
	.4byte sAnims_CyclingTriathleteM @ TRAINER_PIC_CYCLING_TRIATHLETE_M
	.4byte sAnims_AquaGruntM @ TRAINER_PIC_AQUA_GRUNT_M
	.4byte sAnims_Twins @ TRAINER_PIC_TWINS
	.4byte sAnims_SwimmerF @ TRAINER_PIC_SWIMMER_F
	.4byte sAnims_BugCatcher @ TRAINER_PIC_BUG_CATCHER
	.4byte sAnims_SchoolKidM @ TRAINER_PIC_SCHOOL_KID_M
	.4byte sAnims_RichBoy @ TRAINER_PIC_RICH_BOY
	.4byte sAnims_SrAndJr @ TRAINER_PIC_SR_AND_JR
	.4byte sAnims_BlackBelt @ TRAINER_PIC_BLACK_BELT
	.4byte sAnims_Wally @ TRAINER_PIC_WALLY
	.4byte sAnims_AquaGruntF @ TRAINER_PIC_AQUA_GRUNT_F
	.4byte sAnims_TuberF @ TRAINER_PIC_TUBER_F
	.4byte sAnims_HexManiac @ TRAINER_PIC_HEX_MANIAC
	.4byte sAnims_PokemonBreederF @ TRAINER_PIC_POKEMON_BREEDER_F
	.4byte sAnims_MagmaGruntM @ TRAINER_PIC_MAGMA_GRUNT_M
	.4byte sAnims_MagmaGruntF @ TRAINER_PIC_MAGMA_GRUNT_F
	.4byte sAnims_Interviewer @ TRAINER_PIC_INTERVIEWER
	.4byte sAnims_TuberM @ TRAINER_PIC_TUBER_M
	.4byte sAnims_YoungCouple @ TRAINER_PIC_YOUNG_COUPLE
	.4byte sAnims_Guitarist @ TRAINER_PIC_GUITARIST
	.4byte sAnims_Gentleman @ TRAINER_PIC_GENTLEMAN
	.4byte sAnims_Steven @ TRAINER_PIC_STEVEN
	.4byte sAnims_Maxie @ TRAINER_PIC_MAXIE
	.4byte sAnims_RunningTriathleteF @ TRAINER_PIC_RUNNING_TRIATHLETE_F
	.4byte sAnims_RunningTriathleteM @ TRAINER_PIC_RUNNING_TRIATHLETE_M
	.4byte sAnims_BattleGirl @ TRAINER_PIC_BATTLE_GIRL
	.4byte sAnims_CyclingTriathleteF @ TRAINER_PIC_CYCLING_TRIATHLETE_F
	.4byte sAnims_SwimmerM @ TRAINER_PIC_SWIMMER_M
	.4byte sAnims_PokefanF @ TRAINER_PIC_POKEFAN_F
	.4byte sAnims_ExpertM @ TRAINER_PIC_EXPERT_M
	.4byte sAnims_ExpertF @ TRAINER_PIC_EXPERT_F
	.4byte sAnims_SwimmingTriathleteM @ TRAINER_PIC_SWIMMING_TRIATHLETE_M
	.4byte sAnims_SwimmingTriathleteF @ TRAINER_PIC_SWIMMING_TRIATHLETE_F
	.4byte sAnims_DragonTamer @ TRAINER_PIC_DRAGON_TAMER
	.4byte sAnims_BirdKeeper @ TRAINER_PIC_BIRD_KEEPER
	.4byte sAnims_NinjaBoy @ TRAINER_PIC_NINJA_BOY
	.4byte sAnims_ParasolLady @ TRAINER_PIC_PARASOL_LADY
	.4byte sAnims_BugManiac @ TRAINER_PIC_BUG_MANIAC
	.4byte sAnims_Sailor @ TRAINER_PIC_SAILOR
	.4byte sAnims_Collector @ TRAINER_PIC_COLLECTOR
	.4byte sAnims_PokemonBreederM @ TRAINER_PIC_POKEMON_BREEDER_M
	.4byte sAnims_PokemonRangerM @ TRAINER_PIC_POKEMON_RANGER_M
	.4byte sAnims_PokemonRangerF @ TRAINER_PIC_POKEMON_RANGER_F
	.4byte sAnims_MagmaAdminM @ TRAINER_PIC_MAGMA_ADMIN_M
	.4byte sAnims_MagmaAdminF @ TRAINER_PIC_MAGMA_ADMIN_F
	.4byte sAnims_AromaLady @ TRAINER_PIC_AROMA_LADY
	.4byte sAnims_RuinManiac @ TRAINER_PIC_RUIN_MANIAC
	.4byte sAnims_CoolTrainerM @ TRAINER_PIC_COOL_TRAINER_M
	.4byte sAnims_CoolTrainerF @ TRAINER_PIC_COOL_TRAINER_F
	.4byte sAnims_Pokemaniac @ TRAINER_PIC_POKEMANIAC
	.4byte sAnims_Kindler @ TRAINER_PIC_KINDLER
	.4byte sAnims_Camper @ TRAINER_PIC_CAMPER
	.4byte sAnims_Picnicker @ TRAINER_PIC_PICNICKER
	.4byte sAnims_PsychicM @ TRAINER_PIC_PSYCHIC_M
	.4byte sAnims_PsychicF @ TRAINER_PIC_PSYCHIC_F
	.4byte sAnims_SchoolKidF @ TRAINER_PIC_SCHOOL_KID_F
	.4byte sAnims_PokefanM @ TRAINER_PIC_POKEFAN_M
	.4byte sAnims_SisAndBro @ TRAINER_PIC_SIS_AND_BRO
	.4byte sAnims_OldCouple @ TRAINER_PIC_OLD_COUPLE
	.4byte sAnims_AquaAdminM @ TRAINER_PIC_AQUA_ADMIN_M
	.4byte sAnims_AquaAdminF @ TRAINER_PIC_AQUA_ADMIN_F
	.4byte sAnims_UnusedAqua @ TRAINER_PIC_UNUSED_AQUA
	.4byte sAnims_Archie @ TRAINER_PIC_ARCHIE
	.4byte sAnims_Roxanne @ TRAINER_PIC_ROXANNE
	.4byte sAnims_Brawly @ TRAINER_PIC_BRAWLY
	.4byte sAnims_Wattson @ TRAINER_PIC_WATTSON
	.4byte sAnims_Flannery @ TRAINER_PIC_FLANNERY
	.4byte sAnims_Norman @ TRAINER_PIC_NORMAN
	.4byte sAnims_Winona @ TRAINER_PIC_WINONA
	.4byte sAnims_TateAndLiza @ TRAINER_PIC_TATE_AND_LIZA
	.4byte sAnims_Wallace @ TRAINER_PIC_WALLACE
	.4byte sAnims_Sidney @ TRAINER_PIC_SIDNEY
	.4byte sAnims_Phoebe @ TRAINER_PIC_PHOEBE
	.4byte sAnims_Glacia @ TRAINER_PIC_GLACIA
	.4byte sAnims_Drake @ TRAINER_PIC_DRAKE

@ 81EC3F0
	.include "data/trainer_graphics/front_pic_coordinates.inc"

@ 81EC53C
	.include "data/trainer_graphics/front_pic_table.inc"

@ 81EC7D4
	.include "data/trainer_graphics/front_pic_palette_table.inc"

	.align 2
sAnimCmd_Brendan_1:: @ 81ECA6C
	obj_image_anim_frame 0, 24
	obj_image_anim_frame 1, 9
	obj_image_anim_frame 2, 24
	obj_image_anim_frame 0, 9
	obj_image_anim_frame 3, 50
	obj_image_anim_end

	.align 2
sAnimCmd_May_1:: @ 81ECA84
	obj_image_anim_frame 0, 24
	obj_image_anim_frame 1, 9
	obj_image_anim_frame 2, 24
	obj_image_anim_frame 0, 9
	obj_image_anim_frame 3, 50
	obj_image_anim_end

	.align 2
sAnimCmd_Wally_1:: @ 81ECA9C
	obj_image_anim_frame 0, 24
	obj_image_anim_frame 1, 9
	obj_image_anim_frame 2, 24
	obj_image_anim_frame 0, 9
	obj_image_anim_frame 3, 50
	obj_image_anim_end

	.align 2
sBackAnims_Brendan:: @ 81ECAB4
	.4byte sAnim_GeneralFrame3
	.4byte sAnimCmd_Brendan_1

	.align 2
sBackAnims_May:: @ 81ECABC
	.4byte sAnim_GeneralFrame3
	.4byte sAnimCmd_May_1

	.align 2
sBackAnims_Wally:: @ 81ECAC4
	.4byte sAnim_GeneralFrame3
	.4byte sAnimCmd_Wally_1

	.align 2
gTrainerBackAnimsPtrTable:: @ 81ECACC
	.4byte sBackAnims_Brendan @ TRAINER_BACK_PIC_BRENDAN
	.4byte sBackAnims_May @ TRAINER_BACK_PIC_MAY
	.4byte sBackAnims_Wally @ TRAINER_BACK_PIC_WALLY
