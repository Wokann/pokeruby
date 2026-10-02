#### Pokemon Storage System ####

PSSGFXDIR := graphics/pokemon_storage
PSSWALLPAPERGFXDIR := $(PSSGFXDIR)/wallpapers

$(PSSGFXDIR)/header.4bpp: GFX_OPTS := -num_tiles 47

$(PSSGFXDIR)/misc1.4bpp: GFX_OPTS := -num_tiles 91

$(PSSWALLPAPERGFXDIR)/forest/frame.4bpp: GFX_OPTS := -num_tiles 49
$(PSSWALLPAPERGFXDIR)/forest/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/forest/frame.4bpp $(PSSWALLPAPERGFXDIR)/forest/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/forest/frame.4bpp $(PSSWALLPAPERGFXDIR)/forest/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/city/frame.4bpp: GFX_OPTS := -num_tiles 37
$(PSSWALLPAPERGFXDIR)/city/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/city/frame.4bpp $(PSSWALLPAPERGFXDIR)/city/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/city/frame.4bpp $(PSSWALLPAPERGFXDIR)/city/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/desert/frame.4bpp: GFX_OPTS := -num_tiles 48
$(PSSWALLPAPERGFXDIR)/desert/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/desert/frame.4bpp $(PSSWALLPAPERGFXDIR)/desert/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/desert/frame.4bpp $(PSSWALLPAPERGFXDIR)/desert/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/savanna/frame.4bpp: GFX_OPTS := -num_tiles 40
$(PSSWALLPAPERGFXDIR)/savanna/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/savanna/frame.4bpp $(PSSWALLPAPERGFXDIR)/savanna/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/savanna/frame.4bpp $(PSSWALLPAPERGFXDIR)/savanna/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/crag/frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSWALLPAPERGFXDIR)/crag/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/crag/frame.4bpp $(PSSWALLPAPERGFXDIR)/crag/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/crag/frame.4bpp $(PSSWALLPAPERGFXDIR)/crag/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/volcano/frame.4bpp: GFX_OPTS := -num_tiles 49
$(PSSWALLPAPERGFXDIR)/volcano/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/volcano/frame.4bpp $(PSSWALLPAPERGFXDIR)/volcano/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/volcano/frame.4bpp $(PSSWALLPAPERGFXDIR)/volcano/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/snow/frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSWALLPAPERGFXDIR)/snow/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/snow/frame.4bpp $(PSSWALLPAPERGFXDIR)/snow/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/snow/frame.4bpp $(PSSWALLPAPERGFXDIR)/snow/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/cave/frame.4bpp: GFX_OPTS := -num_tiles 57
$(PSSWALLPAPERGFXDIR)/cave/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/cave/frame.4bpp $(PSSWALLPAPERGFXDIR)/cave/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/cave/frame.4bpp $(PSSWALLPAPERGFXDIR)/cave/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/beach/frame.4bpp: GFX_OPTS := -num_tiles 48
$(PSSWALLPAPERGFXDIR)/beach/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/beach/frame.4bpp $(PSSWALLPAPERGFXDIR)/beach/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/beach/frame.4bpp $(PSSWALLPAPERGFXDIR)/beach/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/seafloor/frame.4bpp: GFX_OPTS := -num_tiles 45
$(PSSWALLPAPERGFXDIR)/seafloor/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/seafloor/frame.4bpp $(PSSWALLPAPERGFXDIR)/seafloor/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/seafloor/frame.4bpp $(PSSWALLPAPERGFXDIR)/seafloor/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/river/frame.4bpp: GFX_OPTS := -num_tiles 57
$(PSSWALLPAPERGFXDIR)/river/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/river/frame.4bpp $(PSSWALLPAPERGFXDIR)/river/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/river/frame.4bpp $(PSSWALLPAPERGFXDIR)/river/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/sky/frame.4bpp: GFX_OPTS := -num_tiles 45
$(PSSWALLPAPERGFXDIR)/sky/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/sky/frame.4bpp $(PSSWALLPAPERGFXDIR)/sky/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/sky/frame.4bpp $(PSSWALLPAPERGFXDIR)/sky/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/polkadot/frame.4bpp: GFX_OPTS := -num_tiles 34
$(PSSWALLPAPERGFXDIR)/polkadot/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/polkadot/frame.4bpp $(PSSWALLPAPERGFXDIR)/polkadot/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/polkadot/frame.4bpp $(PSSWALLPAPERGFXDIR)/polkadot/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/pokecenter/bg.4bpp: GFX_OPTS := -num_tiles 38
$(PSSWALLPAPERGFXDIR)/pokecenter/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/pokecenter/frame.4bpp $(PSSWALLPAPERGFXDIR)/pokecenter/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/pokecenter/frame.4bpp $(PSSWALLPAPERGFXDIR)/pokecenter/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/machine/frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSWALLPAPERGFXDIR)/machine/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/machine/frame.4bpp $(PSSWALLPAPERGFXDIR)/machine/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/machine/frame.4bpp $(PSSWALLPAPERGFXDIR)/machine/bg.4bpp >$@

$(PSSWALLPAPERGFXDIR)/plain/frame.4bpp: GFX_OPTS := -num_tiles 18
$(PSSWALLPAPERGFXDIR)/plain/tiles.4bpp: $(PSSWALLPAPERGFXDIR)/plain/frame.4bpp $(PSSWALLPAPERGFXDIR)/plain/bg.4bpp
	@cat $(PSSWALLPAPERGFXDIR)/plain/frame.4bpp $(PSSWALLPAPERGFXDIR)/plain/bg.4bpp >$@


#### Spinda ####

SPINDAGFXDIR := graphics/pokemon/spinda

$(SPINDAGFXDIR)/spots/spot_%.1bpp: GFX_OPTS := -plain -data_width 2


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
PARTYMENUGFXDIR := graphics/party_menu
BATINTGFXDIR := graphics/battle_interface

$(BATINTGFXDIR)/textbox.gbapal: $(BATINTGFXDIR)/textbox_0.gbapal $(BATINTGFXDIR)/textbox_1.gbapal
	@cat $(BATINTGFXDIR)/textbox_0.gbapal $(BATINTGFXDIR)/textbox_1.gbapal >$@

$(MENUGFXDIR)/wordgroup_frame.4bpp: GFX_OPTS := -num_tiles 65

$(PARTYMENUGFXDIR)/misc.4bpp: GFX_OPTS := -num_tiles 116

graphics/summary_screen/tiles.4bpp: GFX_OPTS := -num_tiles 217

$(MENUGFXDIR)/mon_markings_menu.4bpp: GFX_OPTS := -num_tiles 41

$(BATINTGFXDIR)/battle_bar.4bpp: $(BATINTGFXDIR)/hpbar_anim_unused.4bpp \
                                  $(BATINTGFXDIR)/numbers1.4bpp \
                                  $(BATINTGFXDIR)/numbers2.4bpp
	@cat $^ >$@


#### Battle Interface ####

$(BATINTGFXDIR)/healthbox_elements.4bpp: $(BATINTGFXDIR)/hpbar.4bpp $(BATINTGFXDIR)/expbar.4bpp $(BATINTGFXDIR)/status_psn.4bpp $(BATINTGFXDIR)/status_par.4bpp $(BATINTGFXDIR)/status_slp.4bpp $(BATINTGFXDIR)/status_frz.4bpp $(BATINTGFXDIR)/status_brn.4bpp $(BATINTGFXDIR)/misc.4bpp $(BATINTGFXDIR)/hpbar_anim.4bpp $(BATINTGFXDIR)/misc_frameend.4bpp
	cat $^ > $@

#### PokeNav ####

PKNAVGFXDIR := graphics/pokenav

$(PKNAVGFXDIR)/region_map/map.8bpp: GFX_OPTS := -num_tiles 233

$(PKNAVGFXDIR)/device_outline.4bpp: GFX_OPTS := -num_tiles 43

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
JPCONTESTGFXDIR := graphics/contest/japanese

$(CONTESTGFXDIR)/nextturn.4bpp: GFX_OPTS := -num_tiles 8

$(JPCONTESTGFXDIR)/composite_1.4bpp: $(JPCONTESTGFXDIR)/frame_1.4bpp $(JPCONTESTGFXDIR)/floor.4bpp $(JPCONTESTGFXDIR)/frame_2.4bpp $(JPCONTESTGFXDIR)/symbols.4bpp $(JPCONTESTGFXDIR)/meter.4bpp $(JPCONTESTGFXDIR)/letters.4bpp $(JPCONTESTGFXDIR)/numbers.4bpp
	@cat $(JPCONTESTGFXDIR)/frame_1.4bpp $(JPCONTESTGFXDIR)/floor.4bpp $(JPCONTESTGFXDIR)/frame_2.4bpp $(JPCONTESTGFXDIR)/symbols.4bpp $(JPCONTESTGFXDIR)/meter.4bpp $(JPCONTESTGFXDIR)/letters.4bpp $(JPCONTESTGFXDIR)/numbers.4bpp >$@

$(JPCONTESTGFXDIR)/composite_2.4bpp: $(JPCONTESTGFXDIR)/interface.4bpp $(JPCONTESTGFXDIR)/audience.4bpp
	@cat $(JPCONTESTGFXDIR)/interface.4bpp $(JPCONTESTGFXDIR)/audience.4bpp >$@


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

$(UNUSEDGFXDIR)/obi_palpak1.gbapal: $(UNUSEDGFXDIR)/old_pal1.gbapal $(UNUSEDGFXDIR)/old_pal2.gbapal $(UNUSEDGFXDIR)/old_pal3.gbapal
	@cat $(UNUSEDGFXDIR)/old_pal1.gbapal $(UNUSEDGFXDIR)/old_pal2.gbapal $(UNUSEDGFXDIR)/old_pal3.gbapal >$@

$(UNUSEDGFXDIR)/obi_palpak3.gbapal: $(UNUSEDGFXDIR)/old_pal5.gbapal $(UNUSEDGFXDIR)/old_pal6.gbapal $(UNUSEDGFXDIR)/old_pal7.gbapal
	@cat $(UNUSEDGFXDIR)/old_pal5.gbapal $(UNUSEDGFXDIR)/old_pal6.gbapal $(UNUSEDGFXDIR)/old_pal7.gbapal >$@

$(UNUSEDGFXDIR)/obi1.4bpp: $(UNUSEDGFXDIR)/old_bulbasaur.4bpp $(UNUSEDGFXDIR)/old_charizard.4bpp
	@cat $(UNUSEDGFXDIR)/old_bulbasaur.4bpp $(UNUSEDGFXDIR)/old_charizard.4bpp >$@

$(UNUSEDGFXDIR)/obi2.4bpp: $(UNUSEDGFXDIR)/old_bulbasaur2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_1.4bpp $(UNUSEDGFXDIR)/old_battle_interface_2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_3.4bpp
	@cat $(UNUSEDGFXDIR)/old_bulbasaur2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_1.4bpp $(UNUSEDGFXDIR)/old_battle_interface_2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_3.4bpp >$@
