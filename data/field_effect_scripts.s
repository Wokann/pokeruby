	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section script_data, "aw", %progbits

	.align 2
gFieldEffectScriptPointers:: @ 81D9B34
	.4byte gFieldEffectScript_ExclamationMarkIcon1      @ FLDEFF_EXCLAMATION_MARK_ICON
	.4byte gFieldEffectScript_UseCutOnTallGrass         @ FLDEFF_USE_CUT_ON_GRASS
	.4byte gFieldEffectScript_UseCutOnTree              @ FLDEFF_USE_CUT_ON_TREE
	.4byte gFieldEffectScript_Shadow                    @ FLDEFF_SHADOW
	.4byte gFieldEffectScript_TallGrass                 @ FLDEFF_TALL_GRASS
	.4byte gFieldEffectScript_Ripple                    @ FLDEFF_RIPPLE
	.4byte gFieldEffectScript_FieldMoveShowMon          @ FLDEFF_FIELD_MOVE_SHOW_MON
	.4byte gFieldEffectScript_Ash                       @ FLDEFF_ASH
	.4byte gFieldEffectScript_SurfBlob                  @ FLDEFF_SURF_BLOB
	.4byte gFieldEffectScript_UseSurf                   @ FLDEFF_USE_SURF
	.4byte gFieldEffectScript_GroundImpactDust          @ FLDEFF_DUST
	.4byte gFieldEffectScript_UseSecretPowerCave        @ FLDEFF_USE_SECRET_POWER_CAVE
	.4byte gFieldEffectScript_JumpTallGrass             @ FLDEFF_JUMP_TALL_GRASS
	.4byte gFieldEffectScript_SandFootprints            @ FLDEFF_SAND_FOOTPRINTS
	.4byte gFieldEffectScript_JumpBigSplash             @ FLDEFF_JUMP_BIG_SPLASH
	.4byte gFieldEffectScript_Splash                    @ FLDEFF_SPLASH
	.4byte gFieldEffectScript_JumpSmallSplash           @ FLDEFF_JUMP_SMALL_SPLASH
	.4byte gFieldEffectScript_LongGrass                 @ FLDEFF_LONG_GRASS
	.4byte gFieldEffectScript_JumpLongGrass             @ FLDEFF_JUMP_LONG_GRASS
	.4byte gFieldEffectScript_UnusedGrass               @ FLDEFF_UNUSED_GRASS
	.4byte gFieldEffectScript_UnusedGrass2              @ FLDEFF_UNUSED_GRASS_2
	.4byte gFieldEffectScript_UnusedSand                @ FLDEFF_UNUSED_SAND
	.4byte gFieldEffectScript_WaterSurfacing            @ FLDEFF_WATER_SURFACING
	.4byte gFieldEffectScript_BerryTreeGrowthSparkle    @ FLDEFF_BERRY_TREE_GROWTH_SPARKLE
	.4byte gFieldEffectScript_DeepSandFootprints        @ FLDEFF_DEEP_SAND_FOOTPRINTS
	.4byte gFieldEffectScript_PokeCenterHeal            @ FLDEFF_POKECENTER_HEAL
	.4byte gFieldEffectScript_UseSecretPowerTree        @ FLDEFF_USE_SECRET_POWER_TREE
	.4byte gFieldEffectScript_UseSecretPowerShrub       @ FLDEFF_USE_SECRET_POWER_SHRUB
	.4byte gFieldEffectScript_TreeDisguise              @ FLDEFF_TREE_DISGUISE
	.4byte gFieldEffectScript_MountainDisguise          @ FLDEFF_MOUNTAIN_DISGUISE
	.4byte gFieldEffectScript_NPCUseFly                 @ FLDEFF_NPCFLY_OUT
	.4byte gFieldEffectScript_UseFly                    @ FLDEFF_USE_FLY
	.4byte gFieldEffectScript_FlyIn                     @ FLDEFF_FLY_IN
	.4byte gFieldEffectScript_QuestionMarkIcon          @ FLDEFF_QUESTION_MARK_ICON
	.4byte gFieldEffectScript_FeetInFlowingWater        @ FLDEFF_FEET_IN_FLOWING_WATER
	.4byte gFieldEffectScript_BikeTireTracks            @ FLDEFF_BIKE_TIRE_TRACKS
	.4byte gFieldEffectScript_SandDisguisePlaceholder   @ FLDEFF_SAND_DISGUISE
	.4byte gFieldEffectScript_UseRockSmash              @ FLDEFF_USE_ROCK_SMASH
	.4byte gFieldEffectScript_UseDig                    @ FLDEFF_USE_DIG
	.4byte gFieldEffectScript_SandPile                  @ FLDEFF_SAND_PILE
	.4byte gFieldEffectScript_UseStrength               @ FLDEFF_USE_STRENGTH
	.4byte gFieldEffectScript_ShortGrass                @ FLDEFF_SHORT_GRASS
	.4byte gFieldEffectScript_HotSpringsWater           @ FLDEFF_HOT_SPRINGS_WATER
	.4byte gFieldEffectScript_UseWaterfall              @ FLDEFF_USE_WATERFALL
	.4byte gFieldEffectScript_UseDive                   @ FLDEFF_USE_DIVE
	.4byte gFieldEffectScript_PokeballTrail             @ FLDEFF_POKEBALL_TRAIL
	.4byte gFieldEffectScript_HeartIcon                 @ FLDEFF_HEART_ICON
	.4byte gFieldEffectScript_Nop47                     @ FLDEFF_NOP_47
	.4byte gFieldEffectScript_Nop48                     @ FLDEFF_NOP_48
	.4byte gFieldEffectScript_AshPuff                   @ FLDEFF_ASH_PUFF
	.4byte gFieldEffectScript_AshLaunch                 @ FLDEFF_ASH_LAUNCH
	.4byte gFieldEffectScript_SweetScent                @ FLDEFF_SWEET_SCENT
	.4byte gFieldEffectScript_SandPillar                @ FLDEFF_SAND_PILLAR
	.4byte gFieldEffectScript_Bubbles                   @ FLDEFF_BUBBLES
	.4byte gFieldEffectScript_Sparkle                   @ FLDEFF_SPARKLE
	.4byte gFieldEffectScript_ShowSecretPowerCave       @ FLDEFF_SECRET_POWER_CAVE
	.4byte gFieldEffectScript_ShowSecretPowerTree       @ FLDEFF_SECRET_POWER_TREE
	.4byte gFieldEffectScript_ShowSecretPowerShrub      @ FLDEFF_SECRET_POWER_SHRUB
	.4byte gFieldEffectScript_ShowCutGrass              @ FLDEFF_CUT_GRASS
	.4byte gFieldEffectScript_FieldMoveShowMonInit      @ FLDEFF_FIELD_MOVE_SHOW_MON_INIT
	.4byte gFieldEffectScript_UseFlyAncientTomb         @ FLDEFF_USE_FLY_ANCIENT_TOMB
	.4byte gFieldEffectScript_SecretBaseBootPC          @ FLDEFF_PCTURN_ON
	.4byte gFieldEffectScript_HallOfFameRecord          @ FLDEFF_HALL_OF_FAME_RECORD
	.4byte gFieldEffectScript_UseTeleport               @ FLDEFF_USE_TELEPORT

gFieldEffectScript_ExclamationMarkIcon1:: @ 81D9C34
	field_eff_callnative FldEff_ExclamationMarkIcon
	field_eff_end

gFieldEffectScript_UseCutOnTallGrass:: @ 81D9C3A
	field_eff_callnative FldEff_UseCutOnGrass
	field_eff_end

gFieldEffectScript_UseCutOnTree:: @ 81D9C40
	field_eff_callnative FldEff_UseCutOnTree
	field_eff_end

gFieldEffectScript_Shadow:: @ 81D9C46
	field_eff_callnative FldEff_Shadow
	field_eff_end

gFieldEffectScript_TallGrass:: @ 81D9C4C
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_TallGrass
	field_eff_end

gFieldEffectScript_Ripple:: @ 81D9C56
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_Ripple
	field_eff_end

gFieldEffectScript_FieldMoveShowMon:: @ 81D9C60
	field_eff_callnative FldEff_FieldMoveShowMon
	field_eff_end

gFieldEffectScript_Ash:: @ 81D9C66
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_Ash
	field_eff_end

gFieldEffectScript_SurfBlob:: @ 81D9C70
	field_eff_callnative FldEff_SurfBlob
	field_eff_end

gFieldEffectScript_UseSurf:: @ 81D9C76
	field_eff_callnative FldEff_UseSurf
	field_eff_end

gFieldEffectScript_GroundImpactDust:: @ 81D9C7C
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_Dust
	field_eff_end

gFieldEffectScript_UseSecretPowerCave:: @ 81D9C86
	field_eff_callnative FldEff_UseSecretPowerCave
	field_eff_end

gFieldEffectScript_JumpTallGrass:: @ 81D9C8C
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_JumpTallGrass
	field_eff_end

gFieldEffectScript_SandFootprints:: @ 81D9C96
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_SandFootprints
	field_eff_end

gFieldEffectScript_JumpBigSplash:: @ 81D9CA0
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_JumpBigSplash
	field_eff_end

gFieldEffectScript_Splash:: @ 81D9CAA
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_Splash
	field_eff_end

gFieldEffectScript_JumpSmallSplash:: @ 81D9CB4
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_JumpSmallSplash
	field_eff_end

gFieldEffectScript_LongGrass:: @ 81D9CBE
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_LongGrass
	field_eff_end

gFieldEffectScript_JumpLongGrass:: @ 81D9CC8
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_JumpLongGrass
	field_eff_end

gFieldEffectScript_UnusedGrass:: @ 81D9CD2
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_UnusedGrass
	field_eff_end

gFieldEffectScript_UnusedGrass2:: @ 81D9CDC
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_UnusedGrass2
	field_eff_end

gFieldEffectScript_UnusedSand:: @ 81D9CE6
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_UnusedSand
	field_eff_end

gFieldEffectScript_WaterSurfacing:: @ 81D9CF0
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_WaterSurfacing
	field_eff_end

gFieldEffectScript_BerryTreeGrowthSparkle:: @ 81D9CFA
	field_eff_callnative FldEff_BerryTreeGrowthSparkle
	field_eff_end

gFieldEffectScript_DeepSandFootprints:: @ 81D9D00
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_DeepSandFootprints
	field_eff_end

gFieldEffectScript_PokeCenterHeal:: @ 81D9D0A
	field_eff_loadfadedpal gSpritePalette_PokeballGlow
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_PokecenterHeal
	field_eff_end

gFieldEffectScript_UseSecretPowerTree:: @ 81D9D19
	field_eff_callnative FldEff_UseSecretPowerTree
	field_eff_end

gFieldEffectScript_UseSecretPowerShrub:: @ 81D9D1F
	field_eff_callnative FldEff_UseSecretPowerShrub
	field_eff_end

gFieldEffectScript_TreeDisguise:: @ 81D9D25
	field_eff_callnative ShowTreeDisguiseFieldEffect
	field_eff_end

gFieldEffectScript_MountainDisguise:: @ 81D9D2B
	field_eff_callnative ShowMountainDisguiseFieldEffect
	field_eff_end

gFieldEffectScript_NPCUseFly:: @ 81D9D31
	field_eff_callnative FldEff_NPCFlyOut
	field_eff_end

gFieldEffectScript_UseFly:: @ 81D9D37
	field_eff_callnative FldEff_UseFly
	field_eff_end

gFieldEffectScript_FlyIn:: @ 81D9D3D
	field_eff_callnative FldEff_FlyIn
	field_eff_end

gFieldEffectScript_QuestionMarkIcon:: @ 81D9D43
	field_eff_callnative FldEff_QuestionMarkIcon
	field_eff_end

gFieldEffectScript_FeetInFlowingWater:: @ 81D9D49
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_FeetInFlowingWater
	field_eff_end

gFieldEffectScript_BikeTireTracks:: @ 81D9D53
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_BikeTireTracks
	field_eff_end

gFieldEffectScript_SandDisguisePlaceholder:: @ 81D9D5D
	field_eff_callnative ShowSandDisguiseFieldEffect
	field_eff_end

gFieldEffectScript_UseRockSmash:: @ 81D9D63
	field_eff_callnative FldEff_UseRockSmash
	field_eff_end

gFieldEffectScript_UseStrength:: @ 81D9D69
	field_eff_callnative FldEff_UseStrength
	field_eff_end

gFieldEffectScript_UseDig:: @ 81D9D6F
	field_eff_callnative FldEff_UseDig
	field_eff_end

gFieldEffectScript_SandPile:: @ 81D9D75
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_SandPile
	field_eff_end

gFieldEffectScript_ShortGrass:: @ 81D9D7F
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_ShortGrass
	field_eff_end

gFieldEffectScript_HotSpringsWater:: @ 81D9D89
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect1, FldEff_HotSpringsWater
	field_eff_end

gFieldEffectScript_UseWaterfall:: @ 81D9D93
	field_eff_callnative FldEff_UseWaterfall
	field_eff_end

gFieldEffectScript_UseDive:: @ 81D9D99
	field_eff_callnative FldEff_UseDive
	field_eff_end

gFieldEffectScript_PokeballTrail:: @ 81D9D9F
	field_eff_loadpal gSpritePalette_Pokeball
	field_eff_callnative FldEff_PokeballTrail
	field_eff_end

gFieldEffectScript_HeartIcon:: @ 81D9DAA
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_HeartIcon
	field_eff_end

gFieldEffectScript_Nop47:: @ 81D9DB4
	field_eff_callnative FldEff_Nop47
	field_eff_end

gFieldEffectScript_Nop48:: @ 81D9DBA
	field_eff_callnative FldEff_Nop48
	field_eff_end

gFieldEffectScript_AshPuff:: @ 81D9DC0
	field_eff_loadfadedpal_callnative gSpritePalette_Ash, FldEff_AshPuff
	field_eff_end

gFieldEffectScript_AshLaunch:: @ 81D9DCA
	field_eff_loadfadedpal_callnative gSpritePalette_Ash, FldEff_AshLaunch
	field_eff_end

gFieldEffectScript_SweetScent:: @ 81D9DD4
	field_eff_callnative FldEff_SweetScent
	field_eff_end

gFieldEffectScript_SandPillar:: @ 81D9DDA
	field_eff_loadfadedpal_callnative gSpritePalette_SandPillar, FldEff_SandPillar
	field_eff_end

gFieldEffectScript_Bubbles:: @ 81D9DE4
	field_eff_loadfadedpal_callnative gSpritePalette_GeneralFieldEffect0, FldEff_Bubbles
	field_eff_end

gFieldEffectScript_Sparkle:: @ 81D9DEE
	field_eff_loadfadedpal_callnative gSpritePalette_SmallSparkle, FldEff_Sparkle
	field_eff_end

gFieldEffectScript_ShowSecretPowerCave:: @ 81D9DF8
	field_eff_loadfadedpal_callnative gSpritePalette_SecretPower_Cave, FldEff_SecretPowerCave
	field_eff_end

gFieldEffectScript_ShowSecretPowerTree:: @ 81D9E02
	field_eff_loadfadedpal_callnative gSpritePalette_SecretPower_Plant, FldEff_SecretPowerTree
	field_eff_end

gFieldEffectScript_ShowSecretPowerShrub:: @ 81D9E0C
	field_eff_loadfadedpal_callnative gSpritePalette_SecretPower_Plant, FldEff_SecretPowerShrub
	field_eff_end

gFieldEffectScript_ShowCutGrass:: @ 81D9E16
	field_eff_loadfadedpal_callnative gSpritePalette_CutGrass, FldEff_CutGrass
	field_eff_end

gFieldEffectScript_FieldMoveShowMonInit:: @ 81D9E20
	field_eff_callnative FldEff_FieldMoveShowMonInit
	field_eff_end

gFieldEffectScript_UseFlyAncientTomb:: @ 81D9E26
	field_eff_callnative FldEff_UseFlyAncientTomb
	field_eff_end

gFieldEffectScript_SecretBaseBootPC:: @ 81D9E2C
	field_eff_callnative FldEff_SecretBasePCTurnOn
	field_eff_end

gFieldEffectScript_HallOfFameRecord:: @ 81D9E32
	field_eff_loadfadedpal gSpritePalette_PokeballGlow
	field_eff_loadfadedpal_callnative gSpritePalette_HofMonitor, FldEff_HallOfFameRecord
	field_eff_end

gFieldEffectScript_UseTeleport:: @ 81D9E41
	field_eff_callnative FldEff_UseTeleport
	field_eff_end
