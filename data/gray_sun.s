	.include "include/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

	.equ ANIM_TAG_WEATHER_BALL, 10283

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
