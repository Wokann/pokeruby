LilycoveCity_DepartmentStore_5F_MapScripts:: @ 815A060
	.byte 0

LilycoveCity_DepartmentStore_5F_EventScript_ClerkFarLeft:: @ 815A061
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration2 LilycoveCity_DepartmentStore_5F_Pokemart_Dolls
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Pokemart_Dolls:: @ 815A078
	.2byte DECOR_PICHU_DOLL
	.2byte DECOR_PIKACHU_DOLL
	.2byte DECOR_MARILL_DOLL
	.2byte DECOR_JIGGLYPUFF_DOLL
	.2byte 91
	.2byte 92
	.2byte DECOR_BALTOY_DOLL
	.2byte DECOR_KECLEON_DOLL
	.2byte DECOR_AZURILL_DOLL
	.2byte DECOR_SKITTY_DOLL
	.2byte DECOR_SWABLU_DOLL
	.2byte DECOR_GULPIN_DOLL
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_ClerkMidLeft:: @ 815A094
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration2 LilycoveCity_DepartmentStore_5F_Pokemart_Cushions
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Pokemart_Cushions:: @ 815A0AC
	.2byte 101
	.2byte 102
	.2byte DECOR_ZIGZAG_CUSHION
	.2byte 105
	.2byte DECOR_DIAMOND_CUSHION
	.2byte DECOR_BALL_CUSHION
	.2byte DECOR_GRASS_CUSHION
	.2byte DECOR_FIRE_CUSHION
	.2byte DECOR_WATER_CUSHION
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_ClerkMidRight:: @ 815A0C2
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration2 LilycoveCity_DepartmentStore_5F_Pokemart_Posters
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Pokemart_Posters:: @ 815A0DC
	.2byte DECOR_BALL_POSTER
	.2byte DECOR_GREEN_POSTER
	.2byte DECOR_RED_POSTER
	.2byte DECOR_BLUE_POSTER
	.2byte DECOR_CUTE_POSTER
	.2byte DECOR_PIKA_POSTER
	.2byte 72
	.2byte DECOR_SEA_POSTER
	.2byte DECOR_SKY_POSTER
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_ClerkFarRight:: @ 815A0F2
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration2 LilycoveCity_DepartmentStore_5F_Pokemart_Mats
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Pokemart_Mats:: @ 815A10C
	.2byte 59
	.2byte 60
	.2byte 61
	.2byte 62
	.2byte DECOR_ATTRACT_MAT
	.2byte DECOR_FISSURE_MAT
	.2byte DECOR_SPIKES_MAT
	.2byte DECOR_GLITTER_MAT
	.2byte DECOR_JUMP_MAT
	.2byte DECOR_SPIN_MAT
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_PokefanF:: @ 815A124
	msgbox LilycoveCity_DepartmentStore_5F_Text_PlaceFullOfCuteDolls, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_5F_EventScript_WomanNormal:: @ 815A12D
	msgbox LilycoveCity_DepartmentStore_5F_Text_SellManyCuteMatsHere, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_5F_EventScript_LittleGirl:: @ 815A136
	msgbox LilycoveCity_DepartmentStore_5F_Text_GettingDollInsteadOfPokemon, MSGBOX_NPC
	end
