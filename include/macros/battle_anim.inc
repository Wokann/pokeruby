	.macro loadspritegfx id
	.byte 0x00
	.2byte \id
	.endm

	.macro unloadspritegfx id
	.byte 0x01
	.2byte \id
	.endm

	.macro createsprite template, anim_battler, subpriority_offset, argv:vararg
    .byte 0x02
    .4byte \template
    .if \anim_battler == ANIM_BATTLER_TARGET
    .byte 0x80 | (\subpriority_offset & 0x7F)
    .else
    .byte (\subpriority_offset & 0x7F)
    .endif
    .byte (.Lsprite_\@_2 - .Lsprite_\@_1) / 2
.Lsprite_\@_1:
    .2byte \argv
.Lsprite_\@_2:
	.endm

	.macro createvisualtask addr, priority, argv:vararg
	.byte 0x03
	.4byte \addr
	.byte \priority
	.byte (.Lcreatetask_\@_2 - .Lcreatetask_\@_1) / 2
.Lcreatetask_\@_1:
	.2byte \argv
.Lcreatetask_\@_2:
	.endm

	.macro delay amount
	.byte 0x04
	.byte \amount
	.endm

	.macro waitforvisualfinish
	.byte 0x05
	.endm

	.macro hang1
	.byte 0x06
	.endm

	.macro hang2
	.byte 0x07
	.endm

	.macro end
	.byte 0x08
	.endm

	.macro playse id
	.byte 0x09
	.2byte \id
	.endm

	.macro monbg which
	.byte 0x0A
	.byte \which
	.endm

	.macro clearmonbg which
	.byte 0x0B
	.byte \which
	.endm

	.macro setalpha eva, evb
	.byte 0x0C
	.2byte ((\evb) << 8) | (\eva)
	.endm

	.macro blendoff
	.byte 0x0D
	.endm

	.macro call addr
	.byte 0x0E
	.4byte \addr
	.endm

	.macro return
	.byte 0x0F
	.endm

	.macro setarg arg_num, value
	.byte 0x10
	.byte \arg_num
	.2byte \value
	.endm

	.macro choosetwoturnanim addr1, addr2
	.byte 0x11
	.4byte \addr1
	.4byte \addr2
	.endm

	.macro jumpifmoveturn cond, addr
	.byte 0x12
	.byte \cond
	.4byte \addr
	.endm

	.macro goto addr
	.byte 0x13
	.4byte \addr
	.endm

	.macro fadetobg id
	.byte 0x14
	.byte \id
	.endm

	.macro restorebg
	.byte 0x15
	.endm

	.macro waitbgfadeout
	.byte 0x16
	.endm

	.macro waitbgfadein
	.byte 0x17
	.endm

	.macro changebg id
	.byte 0x18
	.byte \id
	.endm

	.macro playsewithpan id, pan
	.byte 0x19
	.2byte \id
	.byte \pan
	.endm

	.macro setpan pan
	.byte 0x1A
	.byte \pan
	.endm

	.macro panse_1B id, pan_start, pan_end, step, delay
	.byte 0x1B
	.2byte \id
	.byte \pan_start
	.byte \pan_end
	.byte \step
	.byte \delay
	.endm

	.macro panse se:req, currentPan:req, targetPan:req, incrementPan:req, delay:req
	.byte 0x1B
	.2byte \se
	.byte \currentPan
	.byte \targetPan
	.byte \incrementPan
	.byte \delay
	.endm

	.macro loopsewithpan id, pan, delay, count
	.byte 0x1C
	.2byte \id
	.byte \pan
	.byte \delay
	.byte \count
	.endm

	.macro waitplaysewithpan id, pan, count
	.byte 0x1D
	.2byte \id
	.byte \pan
	.byte \count
	.endm

	.macro setbldcnt bldcnt
	.byte 0x1E
	.2byte \bldcnt
	.endm

	.macro createsoundtask addr, argv:vararg
	.byte 0x1F
	.4byte \addr
	.byte (.Lcreatetask_1F_\@_2 - .Lcreatetask_1F_\@_1) / 2
.Lcreatetask_1F_\@_1:
	.2byte \argv
.Lcreatetask_1F_\@_2:
	.endm

	.macro waitsound
	.byte 0x20
	.endm

	.macro jumpargeq arg_num, value, addr
	.byte 0x21
	.byte \arg_num
	.2byte \value
	.4byte \addr
	.endm

@ useful macros
	.macro jumpreteq value, addr
	jumpargeq ARG_RET_ID, \value, \addr
	.endm

	.macro jumprettrue addr
	jumpreteq TRUE, \addr
	.endm

	.macro jumpretfalse addr
	jumpreteq FALSE, \addr
	.endm

	.macro monbg_22 bank
	.byte 0x22
	.byte \bank
	.endm

	.macro clearmonbg_23 bank
	.byte 0x23
	.byte \bank
	.endm

	.macro monbg_static bank
	.byte 0x22
	.byte \bank
	.endm

	.macro clearmonbg_static bank
	.byte 0x23
	.byte \bank
	.endm

	.macro jumpifcontest addr
	.byte 0x24
	.4byte \addr
	.endm

	.macro fadetobgfromset a, b, c
	.byte 0x25
	.byte \a
	.byte \b
	.byte \c
	.endm

	.macro panse_adjustnone se:req, currentPan:req, targetPan:req, incrementPan:req, delay:req
	.byte 0x26
	.2byte \se
	.byte \currentPan
	.byte \targetPan
	.byte \incrementPan
	.byte \delay
	.endm

	.macro panse_27 id, pan_start, pan_end, step, delay
	.byte 0x27
	.2byte \id
	.byte \pan_start
	.byte \pan_end
	.byte \step
	.byte \delay
	.endm

	.macro splitbgprio battler:req
	.byte 0x28
	.byte \battler
	.endm

	.macro splitbgprio_all
	.byte 0x29
	.endm

	.macro splitbgprio_foes battler:req
	.byte 0x2A
	.byte \battler
	.endm

	.macro invisible bank
	.byte 0x2B
	.byte \bank
	.endm

	.macro visible bank
	.byte 0x2C
	.byte \bank
	.endm

	.macro doublebattle_2D bank
	.byte 0x2D
	.byte \bank
	.endm

	.macro doublebattle_2E bank
	.byte 0x2E
	.byte \bank
	.endm

	.macro stopsound
	.byte 0x2F
	.endm

@ createsprite wrappers
	.macro simple_palette_blend unused_anim_battler=ANIM_BATTLER_ATTACKER, unused_subpriority_offset=2, selector:req, delay:req, initial_blend_y:req, target_blend_y:req, color:req
	createsprite gSimplePaletteBlendSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \selector, \delay, \initial_blend_y, \target_blend_y, \color
	.endm

	.macro complex_palette_blend unused_anim_battler=ANIM_BATTLER_ATTACKER, unused_subpriority_offset=2, selector:req, delay:req, num_blends:req, color1:req, blend_y1:req, color2:req, blend_y2:req
	createsprite gComplexPaletteBlendSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \selector, \delay, \num_blends, \color1, \blend_y1, \color2, \blend_y2
	.endm

	.macro create_surf_wave priority=2, palette:req
	createvisualtask AnimTask_CreateSurfWave, \priority, \palette
	.endm

	.macro shake_mon_or_platform unused_anim_battler=ANIM_BATTLER_ATTACKER, unused_subpriority_offset=2, velocity:req, shake_timer:req, shake_duration:req, type:req, battler_selector
	.if \type == SHAKE_MON_X || \type == SHAKE_MON_Y
	.ifb \battler_selector
	.error "battler_selector required for SHAKE_MON_X or SHAKE_MON_Y"
	.endif
	.else
	.ifnb \battler_selector
	.warning "unused battler_selector in shake_mon_or_platform"
	.endif
	.endif
	.ifb \battler_selector
	createsprite gShakeMonOrPlatformSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \velocity, \shake_timer, \shake_duration, \type
	.else
	createsprite gShakeMonOrPlatformSpriteTemplate, \unused_anim_battler, \unused_subpriority_offset, \velocity, \shake_timer, \shake_duration, \type, \battler_selector
	.endif
	.endm

	.macro create_basic_hitsplat_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, relative_to:req, animation:req
	createsprite gBasicHitSplatSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \relative_to, \animation
	.endm

	.macro create_claw_slash_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, animation:req
	createsprite gClawSlashSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \animation
	.endm

	.macro create_cross_impact_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, relative_to:req, duration:req
	createsprite gCrossImpactSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \relative_to, \duration
	.endm

	.macro create_flashing_hitsplat_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, relative_to:req, animation:req
	createsprite gFlashingHitSplatSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \relative_to, \animation
	.endm

	.macro create_megahorn_horn_sprite anim_battler:req, subpriority_offset:req, x1:req, y1:req, x2:req, y2:req, duration:req
	createsprite gMegahornHornSpriteTemplate, \anim_battler, \subpriority_offset, \x1, \y1, \x2, \y2, \duration
	.endm

	.macro create_absorption_orb_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, wave_amplitude:req, wave_period:req
	createsprite gAbsorptionOrbSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \wave_amplitude, \wave_period
	.endm

	.macro create_leech_life_needle_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, duration:req
	createsprite gLeechLifeNeedleSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \duration
	.endm

	.macro create_web_thread_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, unk2:req, amplitude:req, targets_both:req
	createsprite gWebThreadSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \unk2, \amplitude, \targets_both
	.endm

	.macro create_string_wrap_sprite anim_battler:req, subpriority_offset:req, x:req, y:req
	createsprite gStringWrapSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y
	.endm

	.macro create_spore_particle_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, wave_offset:req, duration:req, blend:req
	createsprite gSporeParticleSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \wave_offset, \duration, \blend
	.endm

	.macro create_petal_dance_big_flower_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, target_y:req, duration:req
	createsprite gPetalDanceBigFlowerSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \target_y, \duration
	.endm

	.macro create_petal_dance_small_flower_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, target_y:req, duration:req
	createsprite gPetalDanceSmallFlowerSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \target_y, \duration
	.endm

	.macro create_razor_leaf_particle_sprite anim_battler:req, subpriority_offset:req, upward_delta_x:req, upward_delta_y:req, upward_duration:req
	createsprite gRazorLeafParticleSpriteTemplate, \anim_battler, \subpriority_offset, \upward_delta_x, \upward_delta_y, \upward_duration
	.endm

	.macro create_razor_leaf_cutter_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, target_x:req, target_y:req, duration:req, wave_amplitude:req, target_both:req
	createsprite gRazorLeafCutterSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \target_x, \target_y, \duration, \wave_amplitude, \target_both
	.endm

	.macro create_twister_leaf_sprite anim_battler:req, subpriority_offset:req, duration:req, distance_y:req, wave_period:req, wave_amplitude:req, speed_up_on_frame:req
	createsprite gTwisterLeafSpriteTemplate, \anim_battler, \subpriority_offset, \duration, \distance_y, \wave_period, \wave_amplitude, \speed_up_on_frame
	.endm

	.macro create_sharp_teeth_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, animation:req, x_velocity:req, y_velocity:req, half_duration:req
	createsprite gSharpTeethSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \animation, 256 * \x_velocity, 256 * \y_velocity, \half_duration
	.endm

	.macro create_clamp_jaw_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, animation:req, x_velocity:req, y_velocity:req, half_duration:req
	createsprite gClampJawSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \animation, 256 * \x_velocity, 256 * \y_velocity, \half_duration
	.endm

	.macro create_fist_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, duration:req, initPosition=ANIM_BATTLER_TARGET
	createsprite gFistFootSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \duration, \initPosition, 0
	.endm

	.macro create_wide_foot_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, duration:req, initPosition=ANIM_BATTLER_TARGET
	createsprite gFistFootSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \duration, \initPosition, 1
	.endm

@ createvisualtask wrappers
	.macro metallic_shine priority=5, permanent:req, color
	.ifb \color
	createvisualtask AnimTask_MetallicShine, \priority, \permanent, FALSE, rgb(0, 0, 0)
	.else
	createvisualtask AnimTask_MetallicShine, \priority, \permanent, TRUE, \color
	.endif
	.endm

	.macro set_original_pal priority=5, battler:req
	createvisualtask AnimTask_SetGrayscaleOrOriginalPal, \priority, \battler, TRUE
	.endm

	.macro attacker_fade_to_invisible priority=2, step_delay:req
	createvisualtask AnimTask_AttackerFadeToInvisible, \priority, \step_delay
	.endm

	.macro attacker_fade_from_invisible priority=2, step_delay:req
	createvisualtask AnimTask_AttackerFadeFromInvisible, \priority, \step_delay
	.endm

	.macro shrink_target_copy priority=5, unk0:req, unk1:req
	createvisualtask AnimTask_ShrinkTargetCopy, \priority, \unk0, \unk1
	.endm

	.macro blend_color_cycle priority=2, selector:req, delay:req, num_blends:req, initial_blend_y:req, target_blend_y:req, color:req
	createvisualtask AnimTask_BlendColorCycle, \priority, \selector, \delay, \num_blends, \initial_blend_y, \target_blend_y, \color
	.endm

	.macro invert_screen_color priority=2, scenery:req, attacker:req, target:req
	@ NOTE: These generate 0x000 or 0x101 to match, but the code checks for '& 0x100'.
	createvisualtask AnimTask_InvertScreenColor, \priority, (\scenery << 8) | \scenery, (\attacker << 8) | \attacker, (\target << 8) | \target
	.endm

	.macro create_outrage_flame_sprite anim_battler, subpriority_offset, x, y, duration, x_velocity, y_velocity, flicker_duration
	createsprite gOutrageFlameSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \duration, 256 * \x_velocity, 256 * \y_velocity, \flicker_duration
	.endm

	.macro create_dragon_breath_fire_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, target_x:req, target_y:req, duration:req
	createsprite gDragonBreathFireSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \target_x, \target_y, \duration
	.endm

	.macro create_dragon_rage_fire_spit_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, target_x:req, target_y:req, duration:req
	createsprite gDragonRageFireSpitSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \target_x, \target_y, \duration
	.endm

	.macro create_dragon_rage_fire_plume_sprite anim_battler:req, subpriority_offset:req, relative_to:req, x:req, y:req
	createsprite gDragonRageFirePlumeSpriteTemplate, \anim_battler, \subpriority_offset, \relative_to, \x, \y
	.endm

	.macro create_power_absorption_orb_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, duration:req
	createsprite gPowerAbsorptionOrbSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \duration
	.endm

	.macro create_mimic_orb_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req
	createsprite gMimicOrbSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y
	.endm

	.macro create_constrict_binding_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, affine_animation:req, squeezes:req
	createsprite gConstrictBindingSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \affine_animation, \squeezes
	.endm

	.macro create_solar_beam_big_orb_sprite anim_battler:req, subpriority_offset:req, x:req, y:req, duration:req, animation:req
	createsprite gSolarBeamBigOrbSpriteTemplate, \anim_battler, \subpriority_offset, \x, \y, \duration, \animation
	.endm

	.macro create_present_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, unk2:req, unk3:req, unk4:req
	createsprite gPresentSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \unk2, \unk3, \unk4
	.endm

	.macro create_present_heal_particle_sprite anim_battler:req, subpriority_offset:req, initial_x:req, initial_y:req, velocity_y:req, unused3=1
	createsprite gPresentHealParticleSpriteTemplate, \anim_battler, \subpriority_offset, \initial_x, \initial_y, \velocity_y, \unused3
	.endm
