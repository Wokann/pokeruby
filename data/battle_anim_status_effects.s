#include "constants/battle_anim.h"
	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
sAnim_FlickeringOrb:: @ 8393054
	obj_image_anim_frame 0, 3
	obj_image_anim_frame 4, 3
	obj_image_anim_frame 8, 3
	obj_image_anim_frame 12, 3
	obj_image_anim_jump 0

	.align 2
sAnims_FlickeringOrb:: @ 8393068
	.4byte sAnim_FlickeringOrb

	.align 2
sFlickeringOrbSpriteTemplate:: @ 839306C
	spr_template ANIM_TAG_ORB, ANIM_TAG_ORB, gOamData_AffineOff_ObjNormal_16x16, sAnims_FlickeringOrb, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker

	.align 2
sFlickeringOrbFlippedSpriteTemplate:: @ 8393084
	spr_template ANIM_TAG_ORB, ANIM_TAG_ORB, gOamData_AffineOff_ObjNormal_16x16, sAnims_FlickeringOrb, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker_Flipped

	.align 2
sAnim_WeatherBallNormal:: @ 839309C
	obj_image_anim_frame 0, 3
	obj_image_anim_jump 0

	.align 2
sAnims_WeatherBallNormal:: @ 83930A4
	.4byte sAnim_WeatherBallNormal

	.align 2
gWeatherBallUpSpriteTemplate:: @ 83930A8
	spr_template ANIM_TAG_WEATHER_BALL, ANIM_TAG_WEATHER_BALL, gOamData_AffineOff_ObjNormal_32x32, sAnims_WeatherBallNormal, NULL, gDummySpriteAffineAnimTable, AnimWeatherBallUp

	.align 2
gWeatherBallNormalDownSpriteTemplate:: @ 83930C0
	spr_template ANIM_TAG_WEATHER_BALL, ANIM_TAG_WEATHER_BALL, gOamData_AffineOff_ObjNormal_32x32, sAnims_WeatherBallNormal, NULL, gDummySpriteAffineAnimTable, AnimWeatherBallDown

	.align 2
sAnim_SpinningSparkle:: @ 83930D8
	obj_image_anim_frame 0, 3
	obj_image_anim_frame 16, 3
	obj_image_anim_frame 32, 3
	obj_image_anim_frame 48, 3
	obj_image_anim_frame 64, 3
	obj_image_anim_end

	.align 2
sAnims_SpinningSparkle:: @ 83930F0
	.4byte sAnim_SpinningSparkle

	.align 2
gSpinningSparkleSpriteTemplate:: @ 83930F4
	spr_template ANIM_TAG_SPARKLE_4, ANIM_TAG_SPARKLE_4, gOamData_AffineOff_ObjNormal_32x32, sAnims_SpinningSparkle, NULL, gDummySpriteAffineAnimTable, AnimSpinningSparkle

	.align 2
sFlickeringFootSpriteTemplate:: @ 839310C
	spr_template ANIM_TAG_MONSTER_FOOT, ANIM_TAG_MONSTER_FOOT, gOamData_AffineOff_ObjNormal_32x32, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker

	.align 2
sAnim_FlickeringImpact_0:: @ 8393124
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
sAnim_FlickeringImpact_1:: @ 839312C
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
sAnim_FlickeringImpact_2:: @ 8393134
	obj_image_anim_frame 0, 5
	obj_image_anim_jump 0

	.align 2
sAnims_FlickeringImpact:: @ 839313C
	.4byte sAnim_FlickeringImpact_0
	.4byte sAnim_FlickeringImpact_1
	.4byte sAnim_FlickeringImpact_2

	.align 2
sFlickeringImpactSpriteTemplate:: @ 8393148
	spr_template ANIM_TAG_IMPACT, ANIM_TAG_IMPACT, gOamData_AffineOff_ObjNormal_32x32, sAnims_FlickeringImpact, NULL, gDummySpriteAffineAnimTable, AnimTranslateLinearAndFlicker

	.align 2
sAnim_FlickeringShrinkOrb:: @ 8393160
	obj_image_anim_frame 0, 15
	obj_image_anim_jump 0

	.align 2
sAnims_FlickeringShrinkOrb:: @ 8393168
	.4byte sAnim_FlickeringShrinkOrb

	.align 2
sAffineAnim_FlickeringShrinkOrb:: @ 839316C
	obj_rot_scal_anim_frame 0x60, 0x60, 0, 0
	obj_rot_scal_anim_frame 0x2, 0x2, 0, 1
	obj_rot_scal_anim_jump 1

	.align 2
sAffineAnims_FlickeringShrinkOrb:: @ 8393184
	.4byte sAffineAnim_FlickeringShrinkOrb

	.align 2
sFlickeringShrinkOrbSpriteTemplate:: @ 8393188
	spr_template ANIM_TAG_ORB, ANIM_TAG_ORB, gOamData_AffineDouble_ObjNormal_16x16, sAnims_FlickeringShrinkOrb, NULL, sAffineAnims_FlickeringShrinkOrb, AnimTranslateLinearAndFlicker_Flipped
