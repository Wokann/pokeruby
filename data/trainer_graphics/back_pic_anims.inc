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
