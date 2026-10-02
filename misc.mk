#### Pokemon Storage System ####

PSSGFXDIR := graphics/pokemon_storage

$(PSSGFXDIR)/header.4bpp: GFX_OPTS := -num_tiles 47

$(PSSGFXDIR)/misc1.4bpp: GFX_OPTS := -num_tiles 91

$(PSSGFXDIR)/forest_frame.4bpp: GFX_OPTS := -num_tiles 49
$(PSSGFXDIR)/forest.4bpp: $(PSSGFXDIR)/forest_frame.4bpp $(PSSGFXDIR)/forest_bg.4bpp
	@cat $(PSSGFXDIR)/forest_frame.4bpp $(PSSGFXDIR)/forest_bg.4bpp >$@

$(PSSGFXDIR)/city_frame.4bpp: GFX_OPTS := -num_tiles 37
$(PSSGFXDIR)/city.4bpp: $(PSSGFXDIR)/city_frame.4bpp $(PSSGFXDIR)/city_bg.4bpp
	@cat $(PSSGFXDIR)/city_frame.4bpp $(PSSGFXDIR)/city_bg.4bpp >$@

$(PSSGFXDIR)/desert_frame.4bpp: GFX_OPTS := -num_tiles 48
$(PSSGFXDIR)/desert.4bpp: $(PSSGFXDIR)/desert_frame.4bpp $(PSSGFXDIR)/desert_bg.4bpp
	@cat $(PSSGFXDIR)/desert_frame.4bpp $(PSSGFXDIR)/desert_bg.4bpp >$@

$(PSSGFXDIR)/savanna_frame.4bpp: GFX_OPTS := -num_tiles 40
$(PSSGFXDIR)/savanna.4bpp: $(PSSGFXDIR)/savanna_frame.4bpp $(PSSGFXDIR)/savanna_bg.4bpp
	@cat $(PSSGFXDIR)/savanna_frame.4bpp $(PSSGFXDIR)/savanna_bg.4bpp >$@

$(PSSGFXDIR)/crag_frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSGFXDIR)/crag.4bpp: $(PSSGFXDIR)/crag_frame.4bpp $(PSSGFXDIR)/crag_bg.4bpp
	@cat $(PSSGFXDIR)/crag_frame.4bpp $(PSSGFXDIR)/crag_bg.4bpp >$@

$(PSSGFXDIR)/volcano_frame.4bpp: GFX_OPTS := -num_tiles 49
$(PSSGFXDIR)/volcano.4bpp: $(PSSGFXDIR)/volcano_frame.4bpp $(PSSGFXDIR)/volcano_bg.4bpp
	@cat $(PSSGFXDIR)/volcano_frame.4bpp $(PSSGFXDIR)/volcano_bg.4bpp >$@

$(PSSGFXDIR)/snow_frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSGFXDIR)/snow.4bpp: $(PSSGFXDIR)/snow_frame.4bpp $(PSSGFXDIR)/snow_bg.4bpp
	@cat $(PSSGFXDIR)/snow_frame.4bpp $(PSSGFXDIR)/snow_bg.4bpp >$@

$(PSSGFXDIR)/cave_frame.4bpp: GFX_OPTS := -num_tiles 57
$(PSSGFXDIR)/cave.4bpp: $(PSSGFXDIR)/cave_frame.4bpp $(PSSGFXDIR)/cave_bg.4bpp
	@cat $(PSSGFXDIR)/cave_frame.4bpp $(PSSGFXDIR)/cave_bg.4bpp >$@

$(PSSGFXDIR)/beach_frame.4bpp: GFX_OPTS := -num_tiles 48
$(PSSGFXDIR)/beach.4bpp: $(PSSGFXDIR)/beach_frame.4bpp $(PSSGFXDIR)/beach_bg.4bpp
	@cat $(PSSGFXDIR)/beach_frame.4bpp $(PSSGFXDIR)/beach_bg.4bpp >$@

$(PSSGFXDIR)/seafloor_frame.4bpp: GFX_OPTS := -num_tiles 45
$(PSSGFXDIR)/seafloor.4bpp: $(PSSGFXDIR)/seafloor_frame.4bpp $(PSSGFXDIR)/seafloor_bg.4bpp
	@cat $(PSSGFXDIR)/seafloor_frame.4bpp $(PSSGFXDIR)/seafloor_bg.4bpp >$@

$(PSSGFXDIR)/river_frame.4bpp: GFX_OPTS := -num_tiles 57
$(PSSGFXDIR)/river.4bpp: $(PSSGFXDIR)/river_frame.4bpp $(PSSGFXDIR)/river_bg.4bpp
	@cat $(PSSGFXDIR)/river_frame.4bpp $(PSSGFXDIR)/river_bg.4bpp >$@

$(PSSGFXDIR)/sky_frame.4bpp: GFX_OPTS := -num_tiles 45
$(PSSGFXDIR)/sky.4bpp: $(PSSGFXDIR)/sky_frame.4bpp $(PSSGFXDIR)/sky_bg.4bpp
	@cat $(PSSGFXDIR)/sky_frame.4bpp $(PSSGFXDIR)/sky_bg.4bpp >$@

$(PSSGFXDIR)/polkadot_frame.4bpp: GFX_OPTS := -num_tiles 34
$(PSSGFXDIR)/polkadot.4bpp: $(PSSGFXDIR)/polkadot_frame.4bpp $(PSSGFXDIR)/polkadot_bg.4bpp
	@cat $(PSSGFXDIR)/polkadot_frame.4bpp $(PSSGFXDIR)/polkadot_bg.4bpp >$@

$(PSSGFXDIR)/pokecenter_bg.4bpp: GFX_OPTS := -num_tiles 38
$(PSSGFXDIR)/pokecenter.4bpp: $(PSSGFXDIR)/pokecenter_frame.4bpp $(PSSGFXDIR)/pokecenter_bg.4bpp
	@cat $(PSSGFXDIR)/pokecenter_frame.4bpp $(PSSGFXDIR)/pokecenter_bg.4bpp >$@

$(PSSGFXDIR)/machine_frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSGFXDIR)/machine.4bpp: $(PSSGFXDIR)/machine_frame.4bpp $(PSSGFXDIR)/machine_bg.4bpp
	@cat $(PSSGFXDIR)/machine_frame.4bpp $(PSSGFXDIR)/machine_bg.4bpp >$@

$(PSSGFXDIR)/plain_frame.4bpp: GFX_OPTS := -num_tiles 18
$(PSSGFXDIR)/plain.4bpp: $(PSSGFXDIR)/plain_frame.4bpp $(PSSGFXDIR)/plain_bg.4bpp
	@cat $(PSSGFXDIR)/plain_frame.4bpp $(PSSGFXDIR)/plain_bg.4bpp >$@


#### Intro ####

INTROGFXDIR := graphics/intro

$(INTROGFXDIR)/scene_1/drops_logo.4bpp: $(INTROGFXDIR)/scene_1/drops.4bpp $(INTROGFXDIR)/scene_1/game_freak.4bpp
	@cat $(INTROGFXDIR)/scene_1/drops.4bpp $(INTROGFXDIR)/scene_1/game_freak.4bpp >$@

$(INTROGFXDIR)/scene_1/bg_leaves.4bpp: $(INTROGFXDIR)/scene_1/bg.4bpp $(INTROGFXDIR)/scene_1/leaves.4bpp
	@cat $(INTROGFXDIR)/scene_1/bg.4bpp $(INTROGFXDIR)/scene_1/leaves.4bpp >$@

$(INTROGFXDIR)/scene_3/battle_effects.4bpp: $(INTROGFXDIR)/scene_3/battle_star.4bpp $(INTROGFXDIR)/scene_3/battle_attack.4bpp $(INTROGFXDIR)/scene_3/battle_explosion.4bpp
	@cat $(INTROGFXDIR)/scene_3/battle_star.4bpp $(INTROGFXDIR)/scene_3/battle_attack.4bpp $(INTROGFXDIR)/scene_3/battle_explosion.4bpp >$@

$(INTROGFXDIR)/copyright.4bpp: GFX_OPTS := -num_tiles 43


#### Title Screen ####

TITLEGFXDIR := graphics/title_screen

$(TITLEGFXDIR)/press_start.4bpp: GFX_OPTS := -num_tiles 41


#### Interface ####

MENUGFXDIR := graphics/interface
BATINTGFXDIR := graphics/battle_interface

$(BATINTGFXDIR)/textbox.gbapal: $(BATINTGFXDIR)/textbox_0.gbapal $(BATINTGFXDIR)/textbox_1.gbapal
	@cat $(BATINTGFXDIR)/textbox_0.gbapal $(BATINTGFXDIR)/textbox_1.gbapal >$@

$(MENUGFXDIR)/wordgroup_frame.4bpp: GFX_OPTS := -num_tiles 65

$(MENUGFXDIR)/party_menu_misc.4bpp: GFX_OPTS := -num_tiles 116

graphics/summary_screen/tiles.4bpp: GFX_OPTS := -num_tiles 217

$(MENUGFXDIR)/mon_markings_menu.4bpp: GFX_OPTS := -num_tiles 41

$(MENUGFXDIR)/hp_numbers.4bpp: $(MENUGFXDIR)/hpbar_anim.4bpp $(MENUGFXDIR)/numbers1.4bpp $(MENUGFXDIR)/numbers2.4bpp
	@cat $(MENUGFXDIR)/hpbar_anim.4bpp $(MENUGFXDIR)/numbers1.4bpp $(MENUGFXDIR)/numbers2.4bpp >$@


#### Battle Interface ####

BATTLEINTERFACEDIR := graphics/battle_interface

$(BATTLEINTERFACEDIR)/healthbox_elements.4bpp: $(BATTLEINTERFACEDIR)/hpbar.4bpp $(BATTLEINTERFACEDIR)/expbar.4bpp $(BATTLEINTERFACEDIR)/status_psn.4bpp $(BATTLEINTERFACEDIR)/status_par.4bpp $(BATTLEINTERFACEDIR)/status_slp.4bpp $(BATTLEINTERFACEDIR)/status_frz.4bpp $(BATTLEINTERFACEDIR)/status_brn.4bpp $(BATTLEINTERFACEDIR)/misc.4bpp $(BATTLEINTERFACEDIR)/hpbar_anim.4bpp $(BATTLEINTERFACEDIR)/misc_frameend.4bpp
	cat $^ > $@

#### PokeNav ####

PKNAVGFXDIR := graphics/pokenav

$(PKNAVGFXDIR)/region_map/map.8bpp: GFX_OPTS := -num_tiles 233

$(PKNAVGFXDIR)/outline.4bpp: GFX_OPTS := -num_tiles 43

$(PKNAVGFXDIR)/options/main_menu_labels.4bpp: $(PKNAVGFXDIR)/options/hoenn_map.4bpp $(PKNAVGFXDIR)/options/condition.4bpp $(PKNAVGFXDIR)/options/trainers_eyes.4bpp $(PKNAVGFXDIR)/options/ribbons.4bpp $(PKNAVGFXDIR)/options/switch_off.4bpp
	@cat $(PKNAVGFXDIR)/options/hoenn_map.4bpp $(PKNAVGFXDIR)/options/condition.4bpp $(PKNAVGFXDIR)/options/trainers_eyes.4bpp $(PKNAVGFXDIR)/options/ribbons.4bpp $(PKNAVGFXDIR)/options/switch_off.4bpp >$@

$(PKNAVGFXDIR)/options/condition_search_labels.4bpp: $(PKNAVGFXDIR)/options/cool.4bpp $(PKNAVGFXDIR)/options/beauty.4bpp $(PKNAVGFXDIR)/options/cute.4bpp $(PKNAVGFXDIR)/options/smart.4bpp $(PKNAVGFXDIR)/options/tough.4bpp $(PKNAVGFXDIR)/options/cancel.4bpp
	@cat $(PKNAVGFXDIR)/options/cool.4bpp $(PKNAVGFXDIR)/options/beauty.4bpp $(PKNAVGFXDIR)/options/cute.4bpp $(PKNAVGFXDIR)/options/smart.4bpp $(PKNAVGFXDIR)/options/tough.4bpp $(PKNAVGFXDIR)/options/cancel.4bpp >$@


#### Trade ####

TRADEGFXDIR := graphics/trade

$(TRADEGFXDIR)/gba_affine.8bpp: GFX_OPTS := -num_tiles 129

$(TRADEGFXDIR)/menu.4bpp: GFX_OPTS := -num_tiles 148


#### Casino ####

REELGFXDIR := graphics/slot_machine
ROULGFXDIR := graphics/roulette

$(REELGFXDIR)/reel_time_gfx.4bpp: $(REELGFXDIR)/reel_time_pikachu.4bpp $(REELGFXDIR)/reel_time_machine.4bpp
	@cat $(REELGFXDIR)/reel_time_pikachu.4bpp $(REELGFXDIR)/reel_time_machine.4bpp >$@

$(ROULGFXDIR)/roulette_tilt.4bpp: $(ROULGFXDIR)/shroomish.4bpp $(ROULGFXDIR)/tailow.4bpp
	@cat $(ROULGFXDIR)/shroomish.4bpp $(ROULGFXDIR)/tailow.4bpp >$@

$(ROULGFXDIR)/wheel_icons.4bpp: $(ROULGFXDIR)/wynaut.4bpp $(ROULGFXDIR)/azurill.4bpp $(ROULGFXDIR)/skitty.4bpp $(ROULGFXDIR)/makuhita.4bpp
	@cat $(ROULGFXDIR)/wynaut.4bpp $(ROULGFXDIR)/azurill.4bpp $(ROULGFXDIR)/skitty.4bpp $(ROULGFXDIR)/makuhita.4bpp >$@


#### Battle Animations ####

BTLANMSPRGFXDIR := graphics/battle_anims/sprites
MASKSGFXDIR := graphics/battle_anims/masks

$(BTLANMSPRGFXDIR)/spark.4bpp: $(BTLANMSPRGFXDIR)/spark_0.4bpp $(BTLANMSPRGFXDIR)/spark_1.4bpp
	@cat $(BTLANMSPRGFXDIR)/spark_0.4bpp $(BTLANMSPRGFXDIR)/spark_1.4bpp >$@

$(BTLANMSPRGFXDIR)/ice_cube.4bpp: $(BTLANMSPRGFXDIR)/ice_cube_0.4bpp $(BTLANMSPRGFXDIR)/ice_cube_1.4bpp $(BTLANMSPRGFXDIR)/ice_cube_2.4bpp $(BTLANMSPRGFXDIR)/ice_cube_3.4bpp
	@cat $(BTLANMSPRGFXDIR)/ice_cube_0.4bpp $(BTLANMSPRGFXDIR)/ice_cube_1.4bpp $(BTLANMSPRGFXDIR)/ice_cube_2.4bpp $(BTLANMSPRGFXDIR)/ice_cube_3.4bpp >$@

$(BTLANMSPRGFXDIR)/mud_sand.4bpp: $(BTLANMSPRGFXDIR)/mud_sand_0.4bpp $(BTLANMSPRGFXDIR)/mud_sand_1.4bpp
	@cat $(BTLANMSPRGFXDIR)/mud_sand_0.4bpp $(BTLANMSPRGFXDIR)/mud_sand_1.4bpp >$@

$(BTLANMSPRGFXDIR)/ice_crystals.4bpp: $(BTLANMSPRGFXDIR)/ice_crystals_0.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_1.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_2.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_3.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_4.4bpp
	@cat $(BTLANMSPRGFXDIR)/ice_crystals_0.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_1.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_2.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_3.4bpp $(BTLANMSPRGFXDIR)/ice_crystals_4.4bpp >$@

$(BTLANMSPRGFXDIR)/flower.4bpp: $(BTLANMSPRGFXDIR)/flower_0.4bpp $(BTLANMSPRGFXDIR)/flower_1.4bpp
	@cat $(BTLANMSPRGFXDIR)/flower_0.4bpp $(BTLANMSPRGFXDIR)/flower_1.4bpp >$@

$(MASKSGFXDIR)/unused_level_up.4bpp: GFX_OPTS := -num_tiles 14


#### Battle Transition

BATTRANSGFXDIR := graphics/battle_transitions

$(BATTRANSGFXDIR)/vs_frame.4bpp: GFX_OPTS := -num_tiles 16


#### Contest ####

CONTESTGFXDIR := graphics/contest

$(CONTESTGFXDIR)/nextturn.4bpp: GFX_OPTS := -num_tiles 8


#### Picture Frame ####

PICFRAMEGFXDIR := graphics/picture_frame

$(PICFRAMEGFXDIR)/lobby.4bpp: GFX_OPTS := -num_tiles 86


#### Move Types ####

TYPESGFXDIR := graphics/types

types := normal fight flying poison ground rock bug ghost steel mystery fire water grass electric psychic ice dragon dark
contest_types := cool beauty cute smart tough

$(TYPESGFXDIR)/move_types.4bpp: $(types:%=$(TYPESGFXDIR)/%.4bpp) $(contest_types:%=$(TYPESGFXDIR)/contest_%.4bpp)
	@cat $^ >$@

$(TYPESGFXDIR)/move_types.gbapal: $(TYPESGFXDIR)/move_types_1.gbapal $(TYPESGFXDIR)/move_types_2.gbapal $(TYPESGFXDIR)/move_types_3.gbapal
	@cat $(TYPESGFXDIR)/move_types_1.gbapal $(TYPESGFXDIR)/move_types_2.gbapal $(TYPESGFXDIR)/move_types_3.gbapal >$@


#### Miscellaneous ####

MISCGFXDIR := graphics/misc

$(MISCGFXDIR)/hall_of_fame.4bpp: GFX_OPTS := -num_tiles 37

graphics/credits/the_end_copyright.4bpp: GFX_OPTS := -num_tiles 90

STARTERCHOOSEGFXDIR := graphics/starter_choose

$(STARTERCHOOSEGFXDIR)/tiles.4bpp: $(STARTERCHOOSEGFXDIR)/birch_bag.4bpp $(STARTERCHOOSEGFXDIR)/birch_grass.4bpp
	@cat $(STARTERCHOOSEGFXDIR)/birch_bag.4bpp $(STARTERCHOOSEGFXDIR)/birch_grass.4bpp >$@

WALLCLOCKGFXDIR := graphics/wallclock

$(WALLCLOCKGFXDIR)/hand.4bpp: $(WALLCLOCKGFXDIR)/hands.4bpp $(WALLCLOCKGFXDIR)/am_pm.4bpp
	@cat $(WALLCLOCKGFXDIR)/hands.4bpp $(WALLCLOCKGFXDIR)/am_pm.4bpp >$@


#### Unused ####

UNUSEDGFXDIR := graphics/unused

$(UNUSEDGFXDIR)/redyellowgreen_frame.bin: $(UNUSEDGFXDIR)/red_frame.bin $(UNUSEDGFXDIR)/yellow_frame.bin $(UNUSEDGFXDIR)/green_frame.bin $(UNUSEDGFXDIR)/blank_frame.bin
	@cat $(UNUSEDGFXDIR)/red_frame.bin $(UNUSEDGFXDIR)/yellow_frame.bin $(UNUSEDGFXDIR)/green_frame.bin $(UNUSEDGFXDIR)/blank_frame.bin >$@

$(UNUSEDGFXDIR)/color_frames.4bpp: GFX_OPTS := -num_tiles 353

$(UNUSEDGFXDIR)/old_contest.4bpp: $(UNUSEDGFXDIR)/old_contest_frame_1.4bpp $(UNUSEDGFXDIR)/old_contest_floor.4bpp $(UNUSEDGFXDIR)/old_contest_frame_2.4bpp $(UNUSEDGFXDIR)/old_contest_symbols.4bpp $(UNUSEDGFXDIR)/old_contest_meter.4bpp $(UNUSEDGFXDIR)/old_contest_classes.4bpp $(UNUSEDGFXDIR)/old_contest_numbers.4bpp
	@cat $(UNUSEDGFXDIR)/old_contest_frame_1.4bpp $(UNUSEDGFXDIR)/old_contest_floor.4bpp $(UNUSEDGFXDIR)/old_contest_frame_2.4bpp $(UNUSEDGFXDIR)/old_contest_symbols.4bpp $(UNUSEDGFXDIR)/old_contest_meter.4bpp $(UNUSEDGFXDIR)/old_contest_classes.4bpp $(UNUSEDGFXDIR)/old_contest_numbers.4bpp >$@

$(UNUSEDGFXDIR)/old_contest_2.4bpp: $(UNUSEDGFXDIR)/old_contest_2_1.4bpp $(UNUSEDGFXDIR)/old_contest_2_2.4bpp
	@cat $(UNUSEDGFXDIR)/old_contest_2_1.4bpp $(UNUSEDGFXDIR)/old_contest_2_2.4bpp >$@

$(UNUSEDGFXDIR)/obi_palpak1.gbapal: $(UNUSEDGFXDIR)/old_pal1.gbapal $(UNUSEDGFXDIR)/old_pal2.gbapal $(UNUSEDGFXDIR)/old_pal3.gbapal
	@cat $(UNUSEDGFXDIR)/old_pal1.gbapal $(UNUSEDGFXDIR)/old_pal2.gbapal $(UNUSEDGFXDIR)/old_pal3.gbapal >$@

$(UNUSEDGFXDIR)/obi_palpak3.gbapal: $(UNUSEDGFXDIR)/old_pal5.gbapal $(UNUSEDGFXDIR)/old_pal6.gbapal $(UNUSEDGFXDIR)/old_pal7.gbapal
	@cat $(UNUSEDGFXDIR)/old_pal5.gbapal $(UNUSEDGFXDIR)/old_pal6.gbapal $(UNUSEDGFXDIR)/old_pal7.gbapal >$@

$(UNUSEDGFXDIR)/obi1.4bpp: $(UNUSEDGFXDIR)/old_bulbasaur.4bpp $(UNUSEDGFXDIR)/old_charizard.4bpp
	@cat $(UNUSEDGFXDIR)/old_bulbasaur.4bpp $(UNUSEDGFXDIR)/old_charizard.4bpp >$@

$(UNUSEDGFXDIR)/obi2.4bpp: $(UNUSEDGFXDIR)/old_bulbasaur2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_1.4bpp $(UNUSEDGFXDIR)/old_battle_interface_2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_3.4bpp
	@cat $(UNUSEDGFXDIR)/old_bulbasaur2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_1.4bpp $(UNUSEDGFXDIR)/old_battle_interface_2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_3.4bpp >$@
