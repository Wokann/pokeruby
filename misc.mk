#### Pokemon Storage System ####

PSSGFXDIR := graphics/pokemon_storage

$(PSSGFXDIR)/header.4bpp: GFX_OPTS := -num_tiles 47

$(PSSGFXDIR)/misc1.4bpp: GFX_OPTS := -num_tiles 91

$(PSSGFXDIR)/wallpapers/forest/frame.4bpp: GFX_OPTS := -num_tiles 49
$(PSSGFXDIR)/wallpapers/forest/tiles.4bpp: $(PSSGFXDIR)/wallpapers/forest/frame.4bpp $(PSSGFXDIR)/wallpapers/forest/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/forest/frame.4bpp $(PSSGFXDIR)/wallpapers/forest/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/city/frame.4bpp: GFX_OPTS := -num_tiles 37
$(PSSGFXDIR)/wallpapers/city/tiles.4bpp: $(PSSGFXDIR)/wallpapers/city/frame.4bpp $(PSSGFXDIR)/wallpapers/city/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/city/frame.4bpp $(PSSGFXDIR)/wallpapers/city/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/desert/frame.4bpp: GFX_OPTS := -num_tiles 48
$(PSSGFXDIR)/wallpapers/desert/tiles.4bpp: $(PSSGFXDIR)/wallpapers/desert/frame.4bpp $(PSSGFXDIR)/wallpapers/desert/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/desert/frame.4bpp $(PSSGFXDIR)/wallpapers/desert/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/savanna/frame.4bpp: GFX_OPTS := -num_tiles 40
$(PSSGFXDIR)/wallpapers/savanna/tiles.4bpp: $(PSSGFXDIR)/wallpapers/savanna/frame.4bpp $(PSSGFXDIR)/wallpapers/savanna/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/savanna/frame.4bpp $(PSSGFXDIR)/wallpapers/savanna/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/crag/frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSGFXDIR)/wallpapers/crag/tiles.4bpp: $(PSSGFXDIR)/wallpapers/crag/frame.4bpp $(PSSGFXDIR)/wallpapers/crag/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/crag/frame.4bpp $(PSSGFXDIR)/wallpapers/crag/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/volcano/frame.4bpp: GFX_OPTS := -num_tiles 49
$(PSSGFXDIR)/wallpapers/volcano/tiles.4bpp: $(PSSGFXDIR)/wallpapers/volcano/frame.4bpp $(PSSGFXDIR)/wallpapers/volcano/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/volcano/frame.4bpp $(PSSGFXDIR)/wallpapers/volcano/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/snow/frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSGFXDIR)/wallpapers/snow/tiles.4bpp: $(PSSGFXDIR)/wallpapers/snow/frame.4bpp $(PSSGFXDIR)/wallpapers/snow/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/snow/frame.4bpp $(PSSGFXDIR)/wallpapers/snow/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/cave/frame.4bpp: GFX_OPTS := -num_tiles 57
$(PSSGFXDIR)/wallpapers/cave/tiles.4bpp: $(PSSGFXDIR)/wallpapers/cave/frame.4bpp $(PSSGFXDIR)/wallpapers/cave/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/cave/frame.4bpp $(PSSGFXDIR)/wallpapers/cave/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/beach/frame.4bpp: GFX_OPTS := -num_tiles 48
$(PSSGFXDIR)/wallpapers/beach/tiles.4bpp: $(PSSGFXDIR)/wallpapers/beach/frame.4bpp $(PSSGFXDIR)/wallpapers/beach/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/beach/frame.4bpp $(PSSGFXDIR)/wallpapers/beach/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/seafloor/frame.4bpp: GFX_OPTS := -num_tiles 45
$(PSSGFXDIR)/wallpapers/seafloor/tiles.4bpp: $(PSSGFXDIR)/wallpapers/seafloor/frame.4bpp $(PSSGFXDIR)/wallpapers/seafloor/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/seafloor/frame.4bpp $(PSSGFXDIR)/wallpapers/seafloor/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/river/frame.4bpp: GFX_OPTS := -num_tiles 57
$(PSSGFXDIR)/wallpapers/river/tiles.4bpp: $(PSSGFXDIR)/wallpapers/river/frame.4bpp $(PSSGFXDIR)/wallpapers/river/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/river/frame.4bpp $(PSSGFXDIR)/wallpapers/river/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/sky/frame.4bpp: GFX_OPTS := -num_tiles 45
$(PSSGFXDIR)/wallpapers/sky/tiles.4bpp: $(PSSGFXDIR)/wallpapers/sky/frame.4bpp $(PSSGFXDIR)/wallpapers/sky/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/sky/frame.4bpp $(PSSGFXDIR)/wallpapers/sky/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/polkadot/frame.4bpp: GFX_OPTS := -num_tiles 34
$(PSSGFXDIR)/wallpapers/polkadot/tiles.4bpp: $(PSSGFXDIR)/wallpapers/polkadot/frame.4bpp $(PSSGFXDIR)/wallpapers/polkadot/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/polkadot/frame.4bpp $(PSSGFXDIR)/wallpapers/polkadot/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/pokecenter/bg.4bpp: GFX_OPTS := -num_tiles 38
$(PSSGFXDIR)/wallpapers/pokecenter/tiles.4bpp: $(PSSGFXDIR)/wallpapers/pokecenter/frame.4bpp $(PSSGFXDIR)/wallpapers/pokecenter/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/pokecenter/frame.4bpp $(PSSGFXDIR)/wallpapers/pokecenter/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/machine/frame.4bpp: GFX_OPTS := -num_tiles 46
$(PSSGFXDIR)/wallpapers/machine/tiles.4bpp: $(PSSGFXDIR)/wallpapers/machine/frame.4bpp $(PSSGFXDIR)/wallpapers/machine/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/machine/frame.4bpp $(PSSGFXDIR)/wallpapers/machine/bg.4bpp >$@

$(PSSGFXDIR)/wallpapers/plain/frame.4bpp: GFX_OPTS := -num_tiles 18
$(PSSGFXDIR)/wallpapers/plain/tiles.4bpp: $(PSSGFXDIR)/wallpapers/plain/frame.4bpp $(PSSGFXDIR)/wallpapers/plain/bg.4bpp
	@cat $(PSSGFXDIR)/wallpapers/plain/frame.4bpp $(PSSGFXDIR)/wallpapers/plain/bg.4bpp >$@


#### Intro ####

INTROGFXDIR := graphics/intro

$(INTROGFXDIR)/intro.4bpp: $(INTROGFXDIR)/intro1_waterdrops.4bpp $(INTROGFXDIR)/intro1_gamefreak.4bpp
	@cat $(INTROGFXDIR)/intro1_waterdrops.4bpp $(INTROGFXDIR)/intro1_gamefreak.4bpp >$@

$(INTROGFXDIR)/introgfx.4bpp: $(INTROGFXDIR)/intro1_bg.4bpp $(INTROGFXDIR)/intro1_leaves.4bpp
	@cat $(INTROGFXDIR)/intro1_bg.4bpp $(INTROGFXDIR)/intro1_leaves.4bpp >$@

$(INTROGFXDIR)/intro3_misc.4bpp: $(INTROGFXDIR)/intro3_star.4bpp $(INTROGFXDIR)/intro3_attackgfx.4bpp $(INTROGFXDIR)/intro3_explosion.4bpp
	@cat $(INTROGFXDIR)/intro3_star.4bpp $(INTROGFXDIR)/intro3_attackgfx.4bpp $(INTROGFXDIR)/intro3_explosion.4bpp >$@

$(INTROGFXDIR)/copyright.4bpp: GFX_OPTS := -num_tiles 43


#### Title Screen ####

TITLEGFXDIR := graphics/title_screen

$(TITLEGFXDIR)/press_start.4bpp: GFX_OPTS := -num_tiles 41


#### Interface ####

MENUGFXDIR := graphics/interface

$(MENUGFXDIR)/wordgroup_frame.4bpp: GFX_OPTS := -num_tiles 65

$(MENUGFXDIR)/party_menu_misc.4bpp: GFX_OPTS := -num_tiles 116

$(MENUGFXDIR)/mon_markings_menu.4bpp: GFX_OPTS := -num_tiles 41

graphics/summary_screen/tiles.4bpp: GFX_OPTS := -num_tiles 217



#### Battle Interface ####

BATTLEINTERFACEDIR := graphics/battle_interface

$(BATTLEINTERFACEDIR)/battle_bar.4bpp: $(BATTLEINTERFACEDIR)/hpbar_anim_unused.4bpp $(BATTLEINTERFACEDIR)/numbers1.4bpp $(BATTLEINTERFACEDIR)/numbers2.4bpp
	@cat $(BATTLEINTERFACEDIR)/hpbar_anim_unused.4bpp $(BATTLEINTERFACEDIR)/numbers1.4bpp $(BATTLEINTERFACEDIR)/numbers2.4bpp >$@

$(BATTLEINTERFACEDIR)/healthbox_elements.4bpp: $(BATTLEINTERFACEDIR)/hpbar.4bpp $(BATTLEINTERFACEDIR)/expbar.4bpp $(BATTLEINTERFACEDIR)/status_psn.4bpp $(BATTLEINTERFACEDIR)/status_par.4bpp $(BATTLEINTERFACEDIR)/status_slp.4bpp $(BATTLEINTERFACEDIR)/status_frz.4bpp $(BATTLEINTERFACEDIR)/status_brn.4bpp $(BATTLEINTERFACEDIR)/misc.4bpp $(BATTLEINTERFACEDIR)/hpbar_anim.4bpp $(BATTLEINTERFACEDIR)/misc_frameend.4bpp
	cat $^ > $@

#### PokeNav ####

PKNAVGFXDIR := graphics/pokenav

$(PKNAVGFXDIR)/region_map/map.8bpp: GFX_OPTS := -num_tiles 233

$(PKNAVGFXDIR)/device_outline.4bpp: GFX_OPTS := -num_tiles 43

$(PKNAVGFXDIR)/menu_options.4bpp: $(PKNAVGFXDIR)/options/hoenn_map.4bpp $(PKNAVGFXDIR)/options/condition.4bpp $(PKNAVGFXDIR)/menu_eyes.4bpp $(PKNAVGFXDIR)/options/ribbons.4bpp $(PKNAVGFXDIR)/options/switch_off.4bpp
	@cat $(PKNAVGFXDIR)/options/hoenn_map.4bpp $(PKNAVGFXDIR)/options/condition.4bpp $(PKNAVGFXDIR)/menu_eyes.4bpp $(PKNAVGFXDIR)/options/ribbons.4bpp $(PKNAVGFXDIR)/options/switch_off.4bpp >$@

$(PKNAVGFXDIR)/condition_search.4bpp: $(PKNAVGFXDIR)/options/cool.4bpp $(PKNAVGFXDIR)/options/beauty.4bpp $(PKNAVGFXDIR)/options/cute.4bpp $(PKNAVGFXDIR)/options/smart.4bpp $(PKNAVGFXDIR)/options/tough.4bpp $(PKNAVGFXDIR)/options/cancel.4bpp
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

$(MISCGFXDIR)/hof.4bpp: GFX_OPTS := -num_tiles 37

graphics/credits/the_end_copyright.4bpp: GFX_OPTS := -num_tiles 90

STARTERCHOOSEGFXDIR := graphics/starter_choose

$(STARTERCHOOSEGFXDIR)/tiles.4bpp: $(STARTERCHOOSEGFXDIR)/birch_bag.4bpp $(STARTERCHOOSEGFXDIR)/birch_grass.4bpp
	@cat $(STARTERCHOOSEGFXDIR)/birch_bag.4bpp $(STARTERCHOOSEGFXDIR)/birch_grass.4bpp >$@

WALLCLOCKGFXDIR := graphics/wallclock

$(WALLCLOCKGFXDIR)/hand.4bpp: $(WALLCLOCKGFXDIR)/hands.4bpp $(WALLCLOCKGFXDIR)/am_pm.4bpp
	@cat $(WALLCLOCKGFXDIR)/hands.4bpp $(WALLCLOCKGFXDIR)/am_pm.4bpp >$@


#### Unused ####

UNUSEDGFXDIR := graphics/unused
JPCONTESTGFXDIR := graphics/contest/japanese

$(UNUSEDGFXDIR)/redyellowgreen_frame.bin: $(UNUSEDGFXDIR)/red_frame.bin $(UNUSEDGFXDIR)/yellow_frame.bin $(UNUSEDGFXDIR)/green_frame.bin $(UNUSEDGFXDIR)/blank_frame.bin
	@cat $(UNUSEDGFXDIR)/red_frame.bin $(UNUSEDGFXDIR)/yellow_frame.bin $(UNUSEDGFXDIR)/green_frame.bin $(UNUSEDGFXDIR)/blank_frame.bin >$@

$(UNUSEDGFXDIR)/color_frames.4bpp: GFX_OPTS := -num_tiles 353

$(JPCONTESTGFXDIR)/composite_1.4bpp: $(JPCONTESTGFXDIR)/frame_1.4bpp $(JPCONTESTGFXDIR)/floor.4bpp $(JPCONTESTGFXDIR)/frame_2.4bpp $(JPCONTESTGFXDIR)/symbols.4bpp $(JPCONTESTGFXDIR)/meter.4bpp $(JPCONTESTGFXDIR)/letters.4bpp $(JPCONTESTGFXDIR)/numbers.4bpp
	@cat $(JPCONTESTGFXDIR)/frame_1.4bpp $(JPCONTESTGFXDIR)/floor.4bpp $(JPCONTESTGFXDIR)/frame_2.4bpp $(JPCONTESTGFXDIR)/symbols.4bpp $(JPCONTESTGFXDIR)/meter.4bpp $(JPCONTESTGFXDIR)/letters.4bpp $(JPCONTESTGFXDIR)/numbers.4bpp >$@

$(JPCONTESTGFXDIR)/composite_2.4bpp: $(JPCONTESTGFXDIR)/interface.4bpp $(JPCONTESTGFXDIR)/audience.4bpp
	@cat $(JPCONTESTGFXDIR)/interface.4bpp $(JPCONTESTGFXDIR)/audience.4bpp >$@

$(UNUSEDGFXDIR)/obi_palpak1.gbapal: $(UNUSEDGFXDIR)/old_pal1.gbapal $(UNUSEDGFXDIR)/old_pal2.gbapal $(UNUSEDGFXDIR)/old_pal3.gbapal
	@cat $(UNUSEDGFXDIR)/old_pal1.gbapal $(UNUSEDGFXDIR)/old_pal2.gbapal $(UNUSEDGFXDIR)/old_pal3.gbapal >$@

$(UNUSEDGFXDIR)/obi_palpak3.gbapal: $(UNUSEDGFXDIR)/old_pal5.gbapal $(UNUSEDGFXDIR)/old_pal6.gbapal $(UNUSEDGFXDIR)/old_pal7.gbapal
	@cat $(UNUSEDGFXDIR)/old_pal5.gbapal $(UNUSEDGFXDIR)/old_pal6.gbapal $(UNUSEDGFXDIR)/old_pal7.gbapal >$@

$(UNUSEDGFXDIR)/obi1.4bpp: $(UNUSEDGFXDIR)/old_bulbasaur.4bpp $(UNUSEDGFXDIR)/old_charizard.4bpp
	@cat $(UNUSEDGFXDIR)/old_bulbasaur.4bpp $(UNUSEDGFXDIR)/old_charizard.4bpp >$@

$(UNUSEDGFXDIR)/obi2.4bpp: $(UNUSEDGFXDIR)/old_bulbasaur2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_1.4bpp $(UNUSEDGFXDIR)/old_battle_interface_2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_3.4bpp
	@cat $(UNUSEDGFXDIR)/old_bulbasaur2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_1.4bpp $(UNUSEDGFXDIR)/old_battle_interface_2.4bpp $(UNUSEDGFXDIR)/old_battle_interface_3.4bpp >$@
