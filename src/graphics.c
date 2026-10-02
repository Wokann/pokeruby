#include "global.h"

// Battle textbox.
const u16 gBattleTextboxTiles[] = INCBIN_U16("graphics/battle_interface/textbox.4bpp.lz");
const u16 gBattleTextboxPalette[] = INCBIN_U16("graphics/battle_interface/textbox.gbapal.lz");
const u16 gBattleTextboxTilemap[] = INCBIN_U16("graphics/battle_interface/textbox_map.bin");

const u8 gMonFrontPic_CircledQuestionMark[] = INCBIN_U8("graphics/pokemon/question_mark/circled/front.4bpp.lz");
const u8 gMonBackPic_CircledQuestionMark[] = INCBIN_U8("graphics/pokemon/question_mark/circled/back.4bpp.lz");
const u16 gMonPalette_CircledQuestionMark[] = INCBIN_U16("graphics/pokemon/question_mark/circled/normal.gbapal.lz");
const u16 gMonShinyPalette_CircledQuestionMark[] = INCBIN_U16("graphics/pokemon/question_mark/circled/shiny.gbapal.lz");

// The test menu debug battle tileset. This uses the old Crystal
// charmap and rendering method (it uses tiles instead of text).
// The non-English releases use translated graphics.
#if DEBUG_FIX || !ENGLISH
const u8 gDebugBattleCharmap_Gfx_lz[] = INCBIN_U8("graphics/debug/debug_battle_charmap_en.4bpp.lz");
const u8 gDebugBattleCharmap_Tilemap_lz[] = INCBIN_U8("graphics/debug/debug_battle_charmap_en.bin.lz");
const u8 gDebugBattleCharmap_Pal_lz[] = INCBIN_U8("graphics/debug/debug_battle_charmap_en.gbapal.lz");
#else
const u8 gDebugBattleCharmap_Gfx_lz[] = INCBIN_U8("graphics/debug/debug_battle_charmap_jp.4bpp.lz");
const u8 gDebugBattleCharmap_Tilemap_lz[] = INCBIN_U8("graphics/debug/debug_battle_charmap_jp.bin.lz");
const u8 gDebugBattleCharmap_Pal_lz[] = INCBIN_U8("graphics/debug/debug_battle_charmap_jp.gbapal.lz");
#endif

const u8 gSmokescreenImpactTiles[] = INCBIN_U8("graphics/battle_anims/sprites/smokescreen_impact.4bpp.lz");
const u16 gSmokescreenImpactPalette[] = INCBIN_U16("graphics/battle_anims/sprites/smokescreen_impact.gbapal.lz");

#include "data/graphics/pokeballs.h"
const u8 gBlankGfxCompressed[] = INCBIN_U8("graphics/interface/blank.4bpp.lz");

const u8 gBattleAnimSpriteGfx_Bubble[] = INCBIN_U8("graphics/battle_anims/sprites/bubble.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Bone[] = INCBIN_U8("graphics/battle_anims/sprites/bone.4bpp.lz");
const u8 gBattleAnimSpriteGfx_AirWave[] = INCBIN_U8("graphics/battle_anims/sprites/air_wave.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Orb[] = INCBIN_U8("graphics/battle_anims/sprites/orb.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Sword[] = INCBIN_U8("graphics/battle_anims/sprites/sword.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Seed[] = INCBIN_U8("graphics/battle_anims/sprites/seed.4bpp.lz");
const u8 gBattleAnimSpritePal_RainDrops[] = INCBIN_U8("graphics/battle_anims/sprites/rain_drops.gbapal.lz");
const u8 gBattleAnimSpritePal_Bone[] = INCBIN_U8("graphics/battle_anims/sprites/bone.gbapal.lz");
const u8 gBattleAnimSpritePal_AirWave[] = INCBIN_U8("graphics/battle_anims/sprites/air_wave.gbapal.lz");
const u8 gBattleAnimSpritePal_Orb[] = INCBIN_U8("graphics/battle_anims/sprites/orb.gbapal.lz");
const u8 gBattleAnimSpritePal_Sword[] = INCBIN_U8("graphics/battle_anims/sprites/sword.gbapal.lz");
const u8 gBattleAnimSpritePal_Seed[] = INCBIN_U8("graphics/battle_anims/sprites/seed.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Needle[] = INCBIN_U8("graphics/battle_anims/sprites/needle.4bpp.lz");
const u8 gBattleAnimSpritePal_Needle[] = INCBIN_U8("graphics/battle_anims/sprites/needle.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Explosion6[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_6.4bpp.lz");
const u8 gBattleAnimSpritePal_Explosion6[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_6.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PinkOrb[] = INCBIN_U8("graphics/battle_anims/sprites/pink_orb.4bpp.lz");
const u8 gBattleAnimSpritePal_PinkOrb[] = INCBIN_U8("graphics/battle_anims/sprites/pink_orb.gbapal.lz");
const u8 gBattleAnimSpritePal_IceCube[] = INCBIN_U8("graphics/battle_anims/sprites/ice_cube.gbapal.lz");
const u8 gBattleAnimSpriteGfx_IceCube[] = INCBIN_U8("graphics/battle_anims/sprites/ice_cube.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Gust[] = INCBIN_U8("graphics/battle_anims/sprites/gust.4bpp.lz");
const u8 gBattleAnimSpritePal_Gust[] = INCBIN_U8("graphics/battle_anims/sprites/gust.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Spark2[] = INCBIN_U8("graphics/battle_anims/sprites/spark_2.4bpp.lz");
const u8 gBattleAnimSpritePal_Spark2[] = INCBIN_U8("graphics/battle_anims/sprites/spark_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Orange[] = INCBIN_U8("graphics/battle_anims/sprites/orange.4bpp.lz");
const u8 gBattleAnimSpritePal_Orange[] = INCBIN_U8("graphics/battle_anims/sprites/orange.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Spikes[] = INCBIN_U8("graphics/battle_anims/sprites/spikes.4bpp.lz");
const u8 gBattleAnimSpritePal_Spikes[] = INCBIN_U8("graphics/battle_anims/sprites/spikes.gbapal.lz");
const u8 gBattleAnimSpriteGfx_YellowBall[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_ball.4bpp.lz");
const u8 gBattleAnimSpritePal_YellowBall[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_ball.gbapal.lz");
const u8 gBattleAnimSpriteGfx_TiedBag[] = INCBIN_U8("graphics/battle_anims/sprites/tied_bag.4bpp.lz");
const u8 gBattleAnimSpritePal_TiedBag[] = INCBIN_U8("graphics/battle_anims/sprites/tied_bag.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BlackSmoke[] = INCBIN_U8("graphics/battle_anims/sprites/black_smoke.4bpp.lz");
const u8 gBattleAnimSpritePal_BlackSmoke[] = INCBIN_U8("graphics/battle_anims/sprites/black_smoke.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BlackBall[] = INCBIN_U8("graphics/battle_anims/sprites/black_ball.4bpp.lz");

const u8 gBattleAnimSpritePal_BlackBall[] = INCBIN_U8("graphics/battle_anims/sprites/black_ball.gbapal.lz");

const u8 gBattleAnimSpritePal_Glass[] = INCBIN_U8("graphics/battle_anims/sprites/glass.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Glass[] = INCBIN_U8("graphics/battle_anims/sprites/glass.4bpp.lz");
const u8 gBattleAnimSpriteGfx_HornHit[] = INCBIN_U8("graphics/battle_anims/sprites/horn_hit.4bpp.lz");
const u8 gBattleAnimSpritePal_HornHit[] = INCBIN_U8("graphics/battle_anims/sprites/horn_hit.gbapal.lz");
const u8 gBattleAnimSpritePal_BlueShards[] = INCBIN_U8("graphics/battle_anims/sprites/blue_shards.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BlueShards[] = INCBIN_U8("graphics/battle_anims/sprites/blue_shards.4bpp.lz");

const u8 gBattleAnimUnusedPal_MusicNotes[] = INCBIN_U8("graphics/battle_anims/unused/music_notes.gbapal.lz");
const u8 gBattleAnimUnusedGfx_MusicNotes[] = INCBIN_U8("graphics/battle_anims/unused/music_notes.4bpp.lz");

const u8 gBattleAnimSpritePal_Hit[] = INCBIN_U8("graphics/battle_anims/sprites/hit.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Hit[] = INCBIN_U8("graphics/battle_anims/sprites/hit.4bpp.lz");
const u8 gBattleAnimSpritePal_Hit2[] = INCBIN_U8("graphics/battle_anims/sprites/hit_2.gbapal.lz");
const u8 gBattleAnimSpritePal_WavingHand[] = INCBIN_U8("graphics/battle_anims/sprites/waving_hand.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WavingHand[] = INCBIN_U8("graphics/battle_anims/sprites/waving_hand.4bpp.lz");
const u8 gBattleAnimSpriteGfx_ClosingEye[] = INCBIN_U8("graphics/battle_anims/sprites/closing_eye.4bpp.lz");
const u8 gBattleAnimSpritePal_ClosingEye[] = INCBIN_U8("graphics/battle_anims/sprites/closing_eye.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BlueStar[] = INCBIN_U8("graphics/battle_anims/sprites/blue_star.4bpp.lz");
const u8 gBattleAnimSpritePal_BlueStar[] = INCBIN_U8("graphics/battle_anims/sprites/blue_star.gbapal.lz");
const u8 gBattleAnimSpritePal_BubbleBurst[] = INCBIN_U8("graphics/battle_anims/sprites/bubble_burst.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BubbleBurst[] = INCBIN_U8("graphics/battle_anims/sprites/bubble_burst.4bpp.lz");
const u8 gBattleAnimSpriteGfx_HitDuplicate[] = INCBIN_U8("graphics/battle_anims/sprites/hit_duplicate.4bpp.lz");
const u8 gBattleAnimSpritePal_HitDuplicate[] = INCBIN_U8("graphics/battle_anims/sprites/hit_duplicate.gbapal.lz");
const u8 gBattleAnimSpritePal_Leer[] = INCBIN_U8("graphics/battle_anims/sprites/leer.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Leer[] = INCBIN_U8("graphics/battle_anims/sprites/leer.4bpp.lz");
const u8 gBattleAnimSpritePal_BlueBurst[] = INCBIN_U8("graphics/battle_anims/sprites/blue_burst.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BlueBurst[] = INCBIN_U8("graphics/battle_anims/sprites/blue_burst.4bpp.lz");
const u8 gBattleAnimSpriteGfx_SmallEmber[] = INCBIN_U8("graphics/battle_anims/sprites/small_ember.4bpp.lz");
const u8 gBattleAnimSpritePal_SmallEmber[] = INCBIN_U8("graphics/battle_anims/sprites/small_ember.gbapal.lz");
const u8 gBattleAnimSpriteGfx_GraySmoke[] = INCBIN_U8("graphics/battle_anims/sprites/gray_smoke.4bpp.lz");
const u8 gBattleAnimSpritePal_GraySmoke[] = INCBIN_U8("graphics/battle_anims/sprites/gray_smoke.gbapal.lz");
const u8 gBattleAnimSpritePal_Fire[] = INCBIN_U8("graphics/battle_anims/sprites/fire.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Fire[] = INCBIN_U8("graphics/battle_anims/sprites/fire.4bpp.lz");
const u8 gBattleAnimSpriteGfx_SpinningFire[] = INCBIN_U8("graphics/battle_anims/sprites/spinning_fire.4bpp.lz");
const u8 gBattleAnimSpriteGfx_FirePlume[] = INCBIN_U8("graphics/battle_anims/sprites/fire_plume.4bpp.lz");
const u8 gBattleAnimSpritePal_Lightning2[] = INCBIN_U8("graphics/battle_anims/sprites/lightning_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Lightning2[] = INCBIN_U8("graphics/battle_anims/sprites/lightning_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Lightning[] = INCBIN_U8("graphics/battle_anims/sprites/lightning.4bpp.lz");

const u8 gBattleAnimSpriteGfx_SpinningBall[] = INCBIN_U8("graphics/battle_anims/unused/spinning_ball.4bpp.lz");
const u8 gBattleAnimSpritePal_SpinningBall[] = INCBIN_U8("graphics/battle_anims/unused/spinning_ball.gbapal.lz");
const u8 gBattleAnimSpritePal_SpinningBall2[] = INCBIN_U8("graphics/battle_anims/unused/spinning_ball_2.gbapal.lz");

// old battle interface data, unused
const u8 gOldBattleInterfaceGfx[] = INCBIN_U8("graphics/unused/obi1.4bpp.lz");
const u8 gOldBattleInterfacePal_1_2_3[] = INCBIN_U8("graphics/unused/obi_palpak1.gbapal.lz"); // palettes 1-3
const u8 gOldBattleInterfacePal4[] = INCBIN_U8("graphics/unused/old_pal4.gbapal.lz"); // 4 is by itself
const u8 gOldBattleInterfacePal_5_6_7[] = INCBIN_U8("graphics/unused/obi_palpak3.gbapal.lz"); // palettes 5-7
const u8 gOldBattleInterfaceGfx2[] = INCBIN_U8("graphics/unused/obi2.4bpp.lz");
const u8 gOldBattleInterfaceTilemap[] = INCBIN_U8("graphics/unused/old_battle_interface_tilemap.bin.lz");

const u8 gBattleAnimSpritePal_ClawSlash2[] = INCBIN_U8("graphics/battle_anims/sprites/claw_slash_2.gbapal.lz");
const u8 gBattleAnimSpritePal_ClawSlash[] = INCBIN_U8("graphics/battle_anims/sprites/claw_slash.gbapal.lz");
const u8 gBattleAnimSpriteGfx_ClawSlash2[] = INCBIN_U8("graphics/battle_anims/sprites/claw_slash_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_ClawSlash[] = INCBIN_U8("graphics/battle_anims/sprites/claw_slash.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Scratch3[] = INCBIN_U8("graphics/battle_anims/sprites/scratch_3.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Scratch2[] = INCBIN_U8("graphics/battle_anims/sprites/scratch_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_BubbleBurst2[] = INCBIN_U8("graphics/battle_anims/sprites/bubble_burst_2.4bpp.lz");
const u8 gBattleAnimSpritePal_BubbleBurst2[] = INCBIN_U8("graphics/battle_anims/sprites/bubble_burst_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_IceChunk[] = INCBIN_U8("graphics/battle_anims/sprites/ice_chunk.4bpp.lz");
const u8 gBattleAnimSpritePal_IceChunk[] = INCBIN_U8("graphics/battle_anims/sprites/ice_chunk.gbapal.lz");
const u8 gBattleAnimSpritePal_Glass2[] = INCBIN_U8("graphics/battle_anims/sprites/glass_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Glass2[] = INCBIN_U8("graphics/battle_anims/sprites/glass_2.4bpp.lz");
const u8 gBattleAnimSpritePal_PinkHeart2[] = INCBIN_U8("graphics/battle_anims/sprites/pink_heart_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PinkHeart2[] = INCBIN_U8("graphics/battle_anims/sprites/pink_heart_2.4bpp.lz");

const u8 gBattleInterfaceGfx_BattleBar[] = INCBIN_U8("graphics/battle_interface/battle_bar.4bpp.lz");

const u8 gBattleAnimSpriteGfx_SapDrip[] = INCBIN_U8("graphics/battle_anims/sprites/sap_drip.4bpp.lz");
const u8 gBattleAnimSpritePal_SapDrip[] = INCBIN_U8("graphics/battle_anims/sprites/sap_drip.gbapal.lz");
const u8 gBattleAnimSpritePal_SapDrip2[] = INCBIN_U8("graphics/battle_anims/sprites/sap_drip_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Sparkle1[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_1.4bpp.lz");
const u8 gBattleAnimSpritePal_Sparkle1[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_1.gbapal.lz");
const u8 gBattleAnimSpritePal_Sparkle2[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_2.gbapal.lz");
const u8 gBattleAnimSpritePal_HumanoidFoot[] = INCBIN_U8("graphics/battle_anims/sprites/humanoid_foot.gbapal.lz");
const u8 gBattleAnimSpriteGfx_HumanoidFoot[] = INCBIN_U8("graphics/battle_anims/sprites/humanoid_foot.4bpp.lz");
const u8 gBattleAnimSpriteGfx_MonsterFoot[] = INCBIN_U8("graphics/battle_anims/sprites/monster_foot.4bpp.lz");
const u8 gBattleAnimSpriteGfx_HumanoidHand[] = INCBIN_U8("graphics/battle_anims/sprites/humanoid_hand.4bpp.lz");
const u8 gBattleAnimSpriteGfx_LineSketch[] = INCBIN_U8("graphics/battle_anims/unused/line_sketch.4bpp.lz"); // Unused

const u8 gBattleAnimSpritePal_LineSketch[] = INCBIN_U8("graphics/battle_anims/unused/line_sketch.gbapal.lz");
const u8 gBattleAnimSpriteGfx_YellowUnk[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_unk.4bpp.lz");
const u8 gBattleAnimSpritePal_YellowUnk[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_unk.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SlamHit[] = INCBIN_U8("graphics/battle_anims/sprites/slam_hit.4bpp.lz");
const u8 gBattleAnimSpritePal_SlamHit[] = INCBIN_U8("graphics/battle_anims/sprites/slam_hit.gbapal.lz");
const u8 gBattleAnimSpriteGfx_RedFist[] = INCBIN_U8("graphics/battle_anims/sprites/red_fist.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Ring[] = INCBIN_U8("graphics/battle_anims/sprites/ring.4bpp.lz");
const u8 gBattleAnimSpritePal_Ring[] = INCBIN_U8("graphics/battle_anims/sprites/ring.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Rocks[] = INCBIN_U8("graphics/battle_anims/sprites/rocks.4bpp.lz");
const u8 gBattleAnimSpritePal_Rocks[] = INCBIN_U8("graphics/battle_anims/sprites/rocks.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Z[] = INCBIN_U8("graphics/battle_anims/sprites/z.4bpp.lz");
const u8 gBattleAnimSpritePal_Z[] = INCBIN_U8("graphics/battle_anims/sprites/z.gbapal.lz");
const u8 gBattleAnimSpriteGfx_YellowUnk2[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_unk_2.4bpp.lz");
const u8 gBattleAnimSpritePal_YellowUnk2[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_unk_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_AirSlash[] = INCBIN_U8("graphics/battle_anims/sprites/air_slash.4bpp.lz");
const u8 gBattleAnimSpritePal_AirSlash[] = INCBIN_U8("graphics/battle_anims/sprites/air_slash.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SpinningGreenOrbs[] = INCBIN_U8("graphics/battle_anims/sprites/spinning_green_orbs.4bpp.lz");
const u8 gBattleAnimSpritePal_SpinningGreenOrbs[] = INCBIN_U8("graphics/battle_anims/sprites/spinning_green_orbs.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Leaf[] = INCBIN_U8("graphics/battle_anims/sprites/leaf.4bpp.lz");
const u8 gBattleAnimSpritePal_Leaf[] = INCBIN_U8("graphics/battle_anims/sprites/leaf.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MetronomeSmallHand[] = INCBIN_U8("graphics/battle_anims/unused/metronome_hand_small.4bpp.lz"); // Unused, was for Metronome at one point

const u8 gBattleAnimSpritePal_Clapping[] = INCBIN_U8("graphics/battle_anims/sprites/clapping.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PoisonPowder[] = INCBIN_U8("graphics/battle_anims/sprites/poison_powder.4bpp.lz");
const u8 gBattleAnimSpritePal_PoisonPowder[] = INCBIN_U8("graphics/battle_anims/sprites/poison_powder.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BrownTriangle[] = INCBIN_U8("graphics/battle_anims/sprites/brown_triangle.4bpp.lz");
const u8 gBattleAnimSpritePal_BrownTriangle[] = INCBIN_U8("graphics/battle_anims/sprites/brown_triangle.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Sparkle3[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_3.4bpp.lz");
const u8 gBattleAnimSpritePal_Sparkle3[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_3.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Sparkle4[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_4.4bpp.lz");
const u8 gBattleAnimSpriteGfx_MusicNotes[] = INCBIN_U8("graphics/battle_anims/sprites/music_notes.4bpp.lz");
const u8 gBattleAnimSpritePal_MusicNotes[] = INCBIN_U8("graphics/battle_anims/sprites/music_notes.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Duck[] = INCBIN_U8("graphics/battle_anims/sprites/duck.4bpp.lz");
const u8 gBattleAnimSpritePal_Duck[] = INCBIN_U8("graphics/battle_anims/sprites/duck.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Alert[] = INCBIN_U8("graphics/battle_anims/sprites/alert.4bpp.lz");
const u8 gBattleAnimSpritePal_Alert[] = INCBIN_U8("graphics/battle_anims/sprites/alert.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Shock4[] = INCBIN_U8("graphics/battle_anims/sprites/shock_4.4bpp.lz");
const u8 gBattleAnimSpritePal_Shock4[] = INCBIN_U8("graphics/battle_anims/sprites/shock_4.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Shock[] = INCBIN_U8("graphics/battle_anims/sprites/shock.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Bell2[] = INCBIN_U8("graphics/battle_anims/sprites/bell_2.4bpp.lz");
const u8 gBattleAnimSpritePal_Bell2[] = INCBIN_U8("graphics/battle_anims/sprites/bell_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PinkGlove[] = INCBIN_U8("graphics/battle_anims/sprites/pink_glove.4bpp.lz");
const u8 gBattleAnimSpritePal_PinkGlove[] = INCBIN_U8("graphics/battle_anims/sprites/pink_glove.gbapal.lz");

const u8 gBattleAnimUnused_Unknown1[] = INCBIN_U8("graphics/battle_anims/unused/unknown_1.bin");
const u8 gBattleAnimUnused_Unknown2[] = INCBIN_U8("graphics/battle_anims/unused/unknown_2.bin");
const u8 gBattleAnimUnused_Unknown3[] = INCBIN_U8("graphics/battle_anims/unused/unknown_3.bin");
const u8 gBattleAnimUnusedGfx_LineSketch2[] = INCBIN_U8("graphics/battle_anims/unused/line_sketch_2.8bpp.lz");
const u8 gBattleAnimUnusedPal_LineSketch2[] = INCBIN_U8("graphics/battle_anims/unused/line_sketch_2_palette.bin");
const u8 gBattleAnimUnusedTilemap_LineSketch2[] = INCBIN_U8("graphics/battle_anims/unused/line_sketch_2.bin.lz");
const u8 gBattleAnimSpriteGfx_BlueLines[] = INCBIN_U8("graphics/battle_anims/sprites/blue_lines.4bpp.lz");
const u8 gBattleAnimSpritePal_BlueLines[] = INCBIN_U8("graphics/battle_anims/sprites/blue_lines.gbapal.lz");
const u8 gBattleAnimSpritePal_Impact3[] = INCBIN_U8("graphics/battle_anims/sprites/impact_3.gbapal.lz");
const u8 gBattleAnimSpritePal_Impact2[] = INCBIN_U8("graphics/battle_anims/sprites/impact_2.gbapal.lz");
const u8 gBattleAnimSpritePal_Reticle[] = INCBIN_U8("graphics/battle_anims/sprites/reticle.gbapal.lz");
const u8 gBattleAnimSpritePal_Breath[] = INCBIN_U8("graphics/battle_anims/sprites/breath.gbapal.lz");
const u8 gBattleAnimSpritePal_Snowball[] = INCBIN_U8("graphics/battle_anims/sprites/snowball.gbapal.lz");
const u8 gBattleAnimSpritePal_Vine[] = INCBIN_U8("graphics/battle_anims/sprites/vine.gbapal.lz");
const u8 gBattleAnimSpritePal_Sword2[] = INCBIN_U8("graphics/battle_anims/sprites/sword_2.gbapal.lz");
const u8 gBattleAnimSpritePal_RedTube[] = INCBIN_U8("graphics/battle_anims/sprites/red_tube.gbapal.lz");
const u8 gBattleAnimSpritePal_Amnesia[] = INCBIN_U8("graphics/battle_anims/sprites/amnesia.gbapal.lz");
const u8 gBattleAnimSpritePal_String2[] = INCBIN_U8("graphics/battle_anims/sprites/string_2.gbapal.lz");
const u8 gBattleAnimUnused_Unknown4[] = INCBIN_U8("graphics/battle_anims/unused/unknown_4.bin.lz");
const u8 gBattleAnimSpritePal_Pencil2[] = INCBIN_U8("graphics/battle_anims/sprites/pencil_2.gbapal.lz");
const u8 gBattleAnimSpritePal_Petal[] = INCBIN_U8("graphics/battle_anims/sprites/petal.gbapal.lz");
const u8 gBattleAnimSpritePal_BentSpoon[] = INCBIN_U8("graphics/battle_anims/sprites/bent_spoon.gbapal.lz");
const u8 gBattleAnimSpritePal_Coin[] = INCBIN_U8("graphics/battle_anims/sprites/coin.gbapal.lz");
const u8 gBattleAnimSpritePal_CrackedEgg[] = INCBIN_U8("graphics/battle_anims/sprites/cracked_egg.gbapal.lz");
const u8 gBattleAnimSpritePal_FreshEgg[] = INCBIN_U8("graphics/battle_anims/sprites/fresh_egg.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Impact3[] = INCBIN_U8("graphics/battle_anims/sprites/impact_3.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Impact2[] = INCBIN_U8("graphics/battle_anims/sprites/impact_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Reticle[] = INCBIN_U8("graphics/battle_anims/sprites/reticle.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Breath[] = INCBIN_U8("graphics/battle_anims/sprites/breath.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Snowball[] = INCBIN_U8("graphics/battle_anims/sprites/snowball.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Vine[] = INCBIN_U8("graphics/battle_anims/sprites/vine.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Sword2[] = INCBIN_U8("graphics/battle_anims/sprites/sword_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Clapping[] = INCBIN_U8("graphics/battle_anims/sprites/clapping.4bpp.lz");
const u8 gBattleAnimSpriteGfx_RedTube[] = INCBIN_U8("graphics/battle_anims/sprites/red_tube.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Amnesia[] = INCBIN_U8("graphics/battle_anims/sprites/amnesia.4bpp.lz");
const u8 gBattleAnimSpriteGfx_String2[] = INCBIN_U8("graphics/battle_anims/sprites/string_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Pencil2[] = INCBIN_U8("graphics/battle_anims/sprites/pencil_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Petal[] = INCBIN_U8("graphics/battle_anims/sprites/petal.4bpp.lz");
const u8 gBattleAnimSpriteGfx_BentSpoon[] = INCBIN_U8("graphics/battle_anims/sprites/bent_spoon.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Web[] = INCBIN_U8("graphics/battle_anims/sprites/web.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Coin[] = INCBIN_U8("graphics/battle_anims/sprites/coin.4bpp.lz");
const u8 gBattleAnimSpriteGfx_CrackedEgg[] = INCBIN_U8("graphics/battle_anims/sprites/cracked_egg.4bpp.lz");
const u8 gBattleAnimSpriteGfx_HatchedEgg[] = INCBIN_U8("graphics/battle_anims/sprites/hatched_egg.4bpp.lz");
const u8 gBattleAnimSpriteGfx_FreshEgg[] = INCBIN_U8("graphics/battle_anims/sprites/fresh_egg.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Fangs[] = INCBIN_U8("graphics/battle_anims/sprites/fangs.4bpp.lz");
const u8 gBattleAnimSpritePal_Fangs[] = INCBIN_U8("graphics/battle_anims/sprites/fangs.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Explosion2[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_2.4bpp.lz");
const u8 gBattleAnimSpritePal_Explosion2[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Explosion3[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_3.4bpp.lz");
const u8 gBattleAnimSpriteGfx_WaterDroplet[] = INCBIN_U8("graphics/battle_anims/sprites/water_droplet.4bpp.lz");
const u8 gBattleAnimSpritePal_WaterDroplet[] = INCBIN_U8("graphics/battle_anims/sprites/water_droplet.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WaterDroplet2[] = INCBIN_U8("graphics/battle_anims/sprites/water_droplet_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Seed2[] = INCBIN_U8("graphics/battle_anims/sprites/seed_2.4bpp.lz");
const u8 gBattleAnimSpritePal_Seed2[] = INCBIN_U8("graphics/battle_anims/sprites/seed_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Sprout[] = INCBIN_U8("graphics/battle_anims/sprites/sprout.4bpp.lz");
const u8 gBattleAnimSpriteGfx_RedWand[] = INCBIN_U8("graphics/battle_anims/sprites/red_wand.4bpp.lz");
const u8 gBattleAnimSpritePal_RedWand[] = INCBIN_U8("graphics/battle_anims/sprites/red_wand.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PurpleGreenUnk[] = INCBIN_U8("graphics/battle_anims/sprites/purple_green_unk.4bpp.lz");
const u8 gBattleAnimSpritePal_PurpleGreenUnk[] = INCBIN_U8("graphics/battle_anims/sprites/purple_green_unk.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WaterColumn[] = INCBIN_U8("graphics/battle_anims/sprites/water_column.4bpp.lz");
const u8 gBattleAnimSpritePal_WaterColumn[] = INCBIN_U8("graphics/battle_anims/sprites/water_column.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MudUnk[] = INCBIN_U8("graphics/battle_anims/sprites/mud_unk.4bpp.lz");
const u8 gBattleAnimSpritePal_MudUnk[] = INCBIN_U8("graphics/battle_anims/sprites/mud_unk.gbapal.lz");

const u8 gUnusedTilemap_BlueFrame[] = INCBIN_U8("graphics/unused/blue_frame.bin.lz"); // P1, P2, P3 and P4 tilemaps?
const u8 gUnusedTilemap_RedYellowGreenFrame[] = INCBIN_U8("graphics/unused/redyellowgreen_frame.bin.lz");
const u8 gUnusedGfx_ColorFrames[] = INCBIN_U8("graphics/unused/color_frames.4bpp.lz");
const u8 gUnusedPal_ColorFrames[] = INCBIN_U8("graphics/unused/color_frames.gbapal.lz");

const u8 gBattleAnimSpriteGfx_RainDrops[] = INCBIN_U8("graphics/battle_anims/sprites/rain_drops.4bpp.lz");

const u8 gBattleAnimUnusedGfx_WaterSplash[] = INCBIN_U8("graphics/battle_anims/unused/water_splash.8bpp.lz");
const u8 gBattleAnimUnusedTilemap_WaterSplash[] = INCBIN_U8("graphics/battle_anims/unused/water_splash.bin.lz");
const u8 gBattleAnimUnusedPal_WaterSplash[] = INCBIN_U8("graphics/battle_anims/unused/water_splash.gbapal.lz");
const u8 gUnusedGfx_BasicFrame[] = INCBIN_U8("graphics/unused/basic_frame.4bpp.lz");
const u8 gUnusedPal_BasicFrame[] = INCBIN_U8("graphics/unused/basic_frame.gbapal.lz");
const u8 gUnusedTilemap_BasicFrame[] = INCBIN_U8("graphics/unused/basic_frame.bin.lz");

const u8 gBattleInterface_BallStatusBarPal[] = INCBIN_U8("graphics/battle_interface/healthbox_singles_player.gbapal");
const u8 gBattleInterface_BallDisplayPal[] = INCBIN_U8("graphics/battle_interface/hpbar.gbapal");

const u8 gHealthboxElementsGfxTable[][32] = INCBIN_U8("graphics/battle_interface/healthbox_elements.4bpp");

const u8 gBattleInterface_BallDisplayGfx[] = INCBIN_U8("graphics/battle_interface/ball_display.4bpp");

// extra stuff
const u8 gBattleInterfaceGfx_UnusedBallCaughtIndicator[] = INCBIN_U8("graphics/battle_interface/unused_ball_caught_indicator.4bpp");
const u8 gBattleInterfaceGfx_HealthboxStatus2[] = INCBIN_U8("graphics/battle_interface/status2.4bpp"); // Duplicate graphics for the opponent Pokemon in double battles.
const u8 gBattleInterfaceGfx_HealthboxStatus3[] = INCBIN_U8("graphics/battle_interface/status3.4bpp");
const u8 gBattleInterfaceGfx_HealthboxStatus4[] = INCBIN_U8("graphics/battle_interface/status4.4bpp");
const u8 gBattleInterfaceGfx_HealthboxDoublesFrameEnd[] = INCBIN_U8("graphics/battle_interface/healthbox_doubles_frameend.4bpp");
const u8 gBattleInterfaceGfx_HealthboxDoublesFrameEndBar[] = INCBIN_U8("graphics/battle_interface/healthbox_doubles_frameend_bar.4bpp");

const u8 gBattleAnimSpriteGfx_FurySwipes[] = INCBIN_U8("graphics/battle_anims/sprites/fury_swipes.4bpp.lz");
const u8 gBattleAnimSpritePal_FurySwipes[] = INCBIN_U8("graphics/battle_anims/sprites/fury_swipes.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Vine2[] = INCBIN_U8("graphics/battle_anims/sprites/vine_2.4bpp.lz");
const u8 gBattleAnimSpritePal_Vine2[] = INCBIN_U8("graphics/battle_anims/sprites/vine_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Teeth[] = INCBIN_U8("graphics/battle_anims/sprites/teeth.4bpp.lz");
const u8 gBattleAnimSpritePal_Teeth[] = INCBIN_U8("graphics/battle_anims/sprites/teeth.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Bone2[] = INCBIN_U8("graphics/battle_anims/sprites/bone_2.4bpp.lz");
const u8 gBattleAnimSpritePal_Bone2[] = INCBIN_U8("graphics/battle_anims/sprites/bone_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WhiteBag[] = INCBIN_U8("graphics/battle_anims/sprites/white_bag.4bpp.lz");
const u8 gBattleAnimSpritePal_WhiteBag[] = INCBIN_U8("graphics/battle_anims/sprites/white_bag.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Unknown[] = INCBIN_U8("graphics/battle_anims/sprites/unknown.4bpp.lz");
const u8 gBattleAnimSpritePal_Unknown[] = INCBIN_U8("graphics/battle_anims/sprites/unknown.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PurpleCoral[] = INCBIN_U8("graphics/battle_anims/sprites/purple_coral.4bpp.lz");
const u8 gBattleAnimSpritePal_PurpleCoral[] = INCBIN_U8("graphics/battle_anims/sprites/purple_coral.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PurpleDroplet[] = INCBIN_U8("graphics/battle_anims/sprites/purple_droplet.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Shock2[] = INCBIN_U8("graphics/battle_anims/sprites/shock_2.4bpp.lz");
const u8 gBattleAnimSpritePal_Shock2[] = INCBIN_U8("graphics/battle_anims/sprites/shock_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_ClosingEye2[] = INCBIN_U8("graphics/battle_anims/sprites/closing_eye_2.4bpp.lz");
const u8 gBattleAnimSpritePal_ClosingEye2[] = INCBIN_U8("graphics/battle_anims/sprites/closing_eye_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MetalBall[] = INCBIN_U8("graphics/battle_anims/sprites/metal_ball.4bpp.lz");
const u8 gBattleAnimSpritePal_MetalBall[] = INCBIN_U8("graphics/battle_anims/sprites/metal_ball.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MonsterDoll[] = INCBIN_U8("graphics/battle_anims/sprites/monster_doll.4bpp.lz");
const u8 gBattleAnimSpritePal_MonsterDoll[] = INCBIN_U8("graphics/battle_anims/sprites/monster_doll.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Whirlwind[] = INCBIN_U8("graphics/battle_anims/sprites/whirlwind.4bpp.lz");
const u8 gBattleAnimSpritePal_Whirlwind[] = INCBIN_U8("graphics/battle_anims/sprites/whirlwind.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Whirlwind2[] = INCBIN_U8("graphics/battle_anims/sprites/whirlwind_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Explosion4[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_4.4bpp.lz");
const u8 gBattleAnimSpritePal_Explosion4[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_4.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Explosion5[] = INCBIN_U8("graphics/battle_anims/sprites/explosion_5.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Tongue[] = INCBIN_U8("graphics/battle_anims/sprites/tongue.4bpp.lz");
const u8 gBattleAnimSpritePal_Tongue[] = INCBIN_U8("graphics/battle_anims/sprites/tongue.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Smoke[] = INCBIN_U8("graphics/battle_anims/sprites/smoke.4bpp.lz");
const u8 gBattleAnimSpritePal_Smoke[] = INCBIN_U8("graphics/battle_anims/sprites/smoke.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Smoke2[] = INCBIN_U8("graphics/battle_anims/sprites/smoke_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_BlueFlames[] = INCBIN_U8("graphics/battle_anims/sprites/blue_flames.4bpp.lz");
const u8 gBattleAnimSpritePal_BlueFlames[] = INCBIN_U8("graphics/battle_anims/sprites/blue_flames.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BlueFlames2[] = INCBIN_U8("graphics/battle_anims/sprites/blue_flames_2.4bpp.lz");
const u8 gUnusedGfx_OldContest[] = INCBIN_U8("graphics/unused/old_contest.4bpp.lz");
const u8 gUnusedPal_OldContest[] = INCBIN_U8("graphics/unused/old_contest.gbapal.lz");
const u8 gUnusedTilemap_OldContest[] = INCBIN_U8("graphics/unused/old_contest.bin.lz");
const u8 gJPContestWindowsTilemap[] = INCBIN_U8("graphics/contest/japanese/windows.bin.lz");

const u8 gContestNumbers_UnusedGfx[] = INCBIN_U8("graphics/unused/numbers.4bpp.lz");
const u8 gContestNumbers_UnusedPal[] = INCBIN_U8("graphics/unused/numbers.gbapal.lz");
const u8 gUnusedGfx_OldContest2[] = INCBIN_U8("graphics/unused/old_contest_2.4bpp.lz");
const u8 gContestInterfaceAudiencePalette[] = INCBIN_U8("graphics/contest/interface.gbapal.lz");

const u8 gContestAudienceTilemap[] = INCBIN_U8("graphics/contest/audience.bin.lz");

const u8 gContestInterfaceTilemap[] = INCBIN_U8("graphics/contest/interface.bin.lz");
const u8 gJPContestInterfaceTilemap[] = INCBIN_U8("graphics/contest/japanese/interface.bin.lz");
const u8 gJPContestAudienceTilemap[] = INCBIN_U8("graphics/contest/japanese/audience.bin.lz");

const u8 gContestCurtainTilemap[] = INCBIN_U8("graphics/contest/curtain.bin.lz");

const u8 gContestInterfaceGfx[] = INCBIN_U8("graphics/contest/interface.4bpp.lz");

const u8 gContestAudienceGfx[] = INCBIN_U8("graphics/contest/audience.4bpp.lz");
const u8 gContestFaces_Gfx[] = INCBIN_U8("graphics/contest/faces.4bpp.lz");

const u8 gContestJudgeSymbolsGfx[] = INCBIN_U8("graphics/contest/judge_symbols.4bpp.lz");

const u8 gContestJudgeSymbolsPal[] = INCBIN_U8("graphics/contest/judge_symbols.gbapal.lz");

const u8 gContestSliderHeart_Gfx[] = INCBIN_U8("graphics/contest/slider_heart.4bpp");

const u8 gContestResults_Gfx[] = INCBIN_U8("graphics/contest/results_screen/tiles.4bpp.lz");

const u8 gContestResults_WinnerBanner_Tilemap[] = INCBIN_U8("graphics/contest/results_screen/winner_banner.bin.lz");

const u8 gContestResults_Interface_Tilemap[] = INCBIN_U8("graphics/contest/results_screen/interface.bin.lz");

const u8 gContestResults_Bg_Tilemap[] = INCBIN_U8("graphics/contest/results_screen/bg.bin.lz");

const u8 gContestResults_Pal[] = INCBIN_U8("graphics/contest/results_screen/tiles.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Impact[] = INCBIN_U8("graphics/battle_anims/sprites/impact.4bpp.lz");
const u8 gBattleAnimSpritePal_Impact[] = INCBIN_U8("graphics/battle_anims/sprites/impact.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Particles[] = INCBIN_U8("graphics/battle_anims/sprites/particles.4bpp.lz");
const u8 gBattleAnimSpriteGfx_CircleImpact[] = INCBIN_U8("graphics/battle_anims/sprites/circle_impact.4bpp.lz");
const u8 gBattleAnimSpritePal_CircleImpact[] = INCBIN_U8("graphics/battle_anims/sprites/circle_impact.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Scratch[] = INCBIN_U8("graphics/battle_anims/sprites/scratch.4bpp.lz");
const u8 gBattleAnimSpriteGfx_SharpTeeth[] = INCBIN_U8("graphics/battle_anims/sprites/sharp_teeth.4bpp.lz");
const u8 gBattleAnimSpritePal_SharpTeeth[] = INCBIN_U8("graphics/battle_anims/sprites/sharp_teeth.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Clamp[] = INCBIN_U8("graphics/battle_anims/sprites/clamp.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Cut[] = INCBIN_U8("graphics/battle_anims/sprites/cut.4bpp.lz");
const u8 gBattleAnimSpriteGfx_RainbowRings[] = INCBIN_U8("graphics/battle_anims/sprites/rainbow_rings.4bpp.lz");
const u8 gBattleAnimSpritePal_RainbowRings[] = INCBIN_U8("graphics/battle_anims/sprites/rainbow_rings.gbapal.lz");
const u8 gBattleAnimSpriteGfx_IceCrystals[] = INCBIN_U8("graphics/battle_anims/sprites/ice_crystals.4bpp.lz");
const u8 gBattleAnimSpritePal_IceCrystals[] = INCBIN_U8("graphics/battle_anims/sprites/ice_crystals.gbapal.lz");
const u8 gBattleAnimSpriteGfx_IceSpikes[] = INCBIN_U8("graphics/battle_anims/sprites/ice_spikes.4bpp.lz");

const u8 gBattleAnimSpriteGfx_OldBeatUp[] = INCBIN_U8("graphics/battle_anims/unused/old_beatup.4bpp.lz");
const u8 gBattleAnimSpritePal_OldBeatUp[] = INCBIN_U8("graphics/battle_anims/unused/old_beatup.gbapal.lz");

const u8 gBattleAnimSpriteGfx_Orbs[] = INCBIN_U8("graphics/battle_anims/sprites/orbs.4bpp.lz");
const u8 gBattleAnimSpritePal_Orbs[] = INCBIN_U8("graphics/battle_anims/sprites/orbs.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WaterOrb[] = INCBIN_U8("graphics/battle_anims/sprites/water_orb.4bpp.lz");
const u8 gBattleAnimSpriteGfx_WaterImpact[] = INCBIN_U8("graphics/battle_anims/sprites/water_impact.4bpp.lz");
const u8 gBattleAnimSpritePal_WaterImpact[] = INCBIN_U8("graphics/battle_anims/sprites/water_impact.gbapal.lz");
const u8 gBattleAnimSpritePal_BrownOrb[] = INCBIN_U8("graphics/battle_anims/sprites/brown_orb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MudSand[] = INCBIN_U8("graphics/battle_anims/sprites/mud_sand.4bpp.lz");
const u8 gBattleAnimSpritePal_MudSand[] = INCBIN_U8("graphics/battle_anims/sprites/mud_sand.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PoisonBubble[] = INCBIN_U8("graphics/battle_anims/sprites/poison_bubble.4bpp.lz");
const u8 gBattleAnimSpritePal_PoisonBubble[] = INCBIN_U8("graphics/battle_anims/sprites/poison_bubble.gbapal.lz");
const u8 gBattleAnimSpriteGfx_ToxicBubble[] = INCBIN_U8("graphics/battle_anims/sprites/toxic_bubble.4bpp.lz");
const u8 gBattleAnimSpriteGfx_HornHit2[] = INCBIN_U8("graphics/battle_anims/sprites/horn_hit_2.4bpp.lz");
const u8 gBattleAnimSpritePal_HornHit2[] = INCBIN_U8("graphics/battle_anims/sprites/horn_hit_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_AirWave2[] = INCBIN_U8("graphics/battle_anims/sprites/air_wave_2.4bpp.lz");
const u8 gBattleAnimSpritePal_AirWave2[] = INCBIN_U8("graphics/battle_anims/sprites/air_wave_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SmallBubbles[] = INCBIN_U8("graphics/battle_anims/sprites/small_bubbles.4bpp.lz");
const u8 gBattleAnimSpritePal_SmallBubbles[] = INCBIN_U8("graphics/battle_anims/sprites/small_bubbles.gbapal.lz");
const u8 gBattleAnimSpriteGfx_RoundShadow[] = INCBIN_U8("graphics/battle_anims/sprites/round_shadow.4bpp.lz");
const u8 gBattleAnimSpritePal_RoundShadow[] = INCBIN_U8("graphics/battle_anims/sprites/round_shadow.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Sunlight[] = INCBIN_U8("graphics/battle_anims/sprites/sunlight.4bpp.lz");
const u8 gBattleAnimSpritePal_Sunlight[] = INCBIN_U8("graphics/battle_anims/sprites/sunlight.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Spore[] = INCBIN_U8("graphics/battle_anims/sprites/spore.4bpp.lz");
const u8 gBattleAnimSpritePal_Spore[] = INCBIN_U8("graphics/battle_anims/sprites/spore.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Flower[] = INCBIN_U8("graphics/battle_anims/sprites/flower.4bpp.lz");
const u8 gBattleAnimSpritePal_Flower[] = INCBIN_U8("graphics/battle_anims/sprites/flower.gbapal.lz");
const u8 gBattleAnimSpriteGfx_RazorLeaf[] = INCBIN_U8("graphics/battle_anims/sprites/razor_leaf.4bpp.lz");
const u8 gBattleAnimSpritePal_RazorLeaf[] = INCBIN_U8("graphics/battle_anims/sprites/razor_leaf.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MistCloud[] = INCBIN_U8("graphics/battle_anims/sprites/mist_cloud.4bpp.lz");
const u8 gBattleAnimSpritePal_MistCloud[] = INCBIN_U8("graphics/battle_anims/sprites/mist_cloud.gbapal.lz");

const u8 gBattleAnimUnusedGfx_Lights[] = INCBIN_U8("graphics/battle_anims/unused/lights.4bpp.lz");
const u8 gBattleAnimUnusedPal_Lights[] = INCBIN_U8("graphics/battle_anims/unused/lights.gbapal.lz");
const u8 gBattleAnimUnusedTilemap_Lights[] = INCBIN_U8("graphics/battle_anims/unused/lights.bin.lz");

const u8 gBattleAnimSpriteGfx_WhirlwindLines[] = INCBIN_U8("graphics/battle_anims/sprites/whirlwind_lines.4bpp.lz");
const u8 gBattleAnimSpritePal_WhirlwindLines[] = INCBIN_U8("graphics/battle_anims/sprites/whirlwind_lines.gbapal.lz");
const u8 gBattleAnimSpriteGfx_GoldRing[] = INCBIN_U8("graphics/battle_anims/sprites/gold_ring.4bpp.lz");
const u8 gBattleAnimSpritePal_GoldRing[] = INCBIN_U8("graphics/battle_anims/sprites/gold_ring.gbapal.lz");
const u8 gBattleAnimSpritePal_BlueRing2[] = INCBIN_U8("graphics/battle_anims/sprites/blue_ring_2.gbapal.lz");
const u8 gBattleAnimSpritePal_PurpleRing[] = INCBIN_U8("graphics/battle_anims/sprites/purple_ring.gbapal.lz");
const u8 gBattleAnimSpritePal_BlueRing[] = INCBIN_U8("graphics/battle_anims/sprites/blue_ring.gbapal.lz");
const u8 gBattleAnimSpriteGfx_GreenLightWall[] = INCBIN_U8("graphics/battle_anims/sprites/green_light_wall.4bpp.lz");
const u8 gBattleAnimSpritePal_GreenLightWall[] = INCBIN_U8("graphics/battle_anims/sprites/green_light_wall.gbapal.lz");
const u8 gBattleAnimSpritePal_BlueLightWall[] = INCBIN_U8("graphics/battle_anims/sprites/blue_light_wall.gbapal.lz");
const u8 gBattleAnimSpritePal_RedLightWall[] = INCBIN_U8("graphics/battle_anims/sprites/red_light_wall.gbapal.lz");
const u8 gBattleAnimSpritePal_GrayLightWall[] = INCBIN_U8("graphics/battle_anims/sprites/gray_light_wall.gbapal.lz");
const u8 gBattleAnimSpritePal_OrangeLightWall[] = INCBIN_U8("graphics/battle_anims/sprites/orange_light_wall.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BlackBall2[] = INCBIN_U8("graphics/battle_anims/sprites/black_ball_2.4bpp.lz");
const u8 gBattleAnimSpritePal_BlackBall2[] = INCBIN_U8("graphics/battle_anims/sprites/black_ball_2.gbapal.lz");
const u8 gBattleAnimSpritePal_PurpleGasCloud[] = INCBIN_U8("graphics/battle_anims/sprites/purple_gas_cloud.gbapal.lz");

const u8 gContestJudgeGfx[] = INCBIN_U8("graphics/contest/judge.4bpp.lz");
const u8 gContest2Pal[] = INCBIN_U8("graphics/contest/judge.gbapal.lz");

const u8 gBattleAnimSpriteGfx_Spark[] = INCBIN_U8("graphics/battle_anims/sprites/spark.4bpp.lz");
const u8 gBattleAnimSpritePal_Spark[] = INCBIN_U8("graphics/battle_anims/sprites/spark.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SparkH[] = INCBIN_U8("graphics/battle_anims/sprites/spark_h.4bpp.lz");

const u8 gBattleAnimBgImage_Dark[] = INCBIN_U8("graphics/battle_anims/backgrounds/dark.4bpp.lz");
const u8 gBattleAnimBgPalette_Dark[] = INCBIN_U8("graphics/battle_anims/backgrounds/dark.gbapal.lz");
const u8 gBattleAnimBgTilemap_Dark[] = INCBIN_U8("graphics/battle_anims/backgrounds/dark.bin.lz");

const u8 gMetalShineGfx[] = INCBIN_U8("graphics/battle_anims/masks/metal_shine.4bpp.lz");
const u8 gMetalShinePalette[] = INCBIN_U8("graphics/battle_anims/masks/metal_shine.gbapal.lz");
const u8 gMetalShineTilemap[] = INCBIN_U8("graphics/battle_anims/masks/metal_shine.bin.lz");

const u8 gUnusedGfx_Goosuto[] = INCBIN_U8("graphics/battle_anims/unused/goosuto.4bpp.lz"); // ghost
const u8 gUnusedPal_Goosuto[] = INCBIN_U8("graphics/battle_anims/unused/goosuto.gbapal.lz");
const u8 gUnusedTilemap_Goosuto[] = INCBIN_U8("graphics/battle_anims/unused/goosuto.bin.lz");

const u8 gBattleAnimSpriteGfx_YellowStar[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_star.4bpp.lz");
const u8 gBattleAnimSpritePal_YellowStar[] = INCBIN_U8("graphics/battle_anims/sprites/yellow_star.gbapal.lz");
const u8 gBattleAnimSpriteGfx_LargeFreshEgg[] = INCBIN_U8("graphics/battle_anims/sprites/large_fresh_egg.4bpp.lz");
const u8 gBattleAnimSpritePal_LargeFreshEgg[] = INCBIN_U8("graphics/battle_anims/sprites/large_fresh_egg.gbapal.lz");
const u8 gBattleAnimSpriteGfx_ShadowBall[] = INCBIN_U8("graphics/battle_anims/sprites/shadow_ball.4bpp.lz");
const u8 gBattleAnimSpritePal_ShadowBall[] = INCBIN_U8("graphics/battle_anims/sprites/shadow_ball.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Lick[] = INCBIN_U8("graphics/battle_anims/sprites/lick.4bpp.lz");
const u8 gBattleAnimSpritePal_Lick[] = INCBIN_U8("graphics/battle_anims/sprites/lick.gbapal.lz");
const u8 gBattleAnimSpriteGfx_VoidLines[] = INCBIN_U8("graphics/battle_anims/sprites/void_lines.4bpp.lz");
const u8 gBattleAnimSpritePal_VoidLines[] = INCBIN_U8("graphics/battle_anims/sprites/void_lines.gbapal.lz");
const u8 gBattleAnimSpritePal_String[] = INCBIN_U8("graphics/battle_anims/sprites/string.gbapal.lz");
const u8 gBattleAnimSpriteGfx_String[] = INCBIN_U8("graphics/battle_anims/sprites/string.4bpp.lz");
const u8 gBattleAnimSpriteGfx_WebThread[] = INCBIN_U8("graphics/battle_anims/sprites/web_thread.4bpp.lz");
const u8 gBattleAnimSpriteGfx_SpiderWeb[] = INCBIN_U8("graphics/battle_anims/sprites/spider_web.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Lightbulb[] = INCBIN_U8("graphics/battle_anims/sprites/lightbulb.4bpp.lz");
const u8 gBattleAnimSpritePal_Lightbulb[] = INCBIN_U8("graphics/battle_anims/sprites/lightbulb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Slash[] = INCBIN_U8("graphics/battle_anims/sprites/slash.4bpp.lz");
const u8 gBattleAnimSpritePal_Slash[] = INCBIN_U8("graphics/battle_anims/sprites/slash.gbapal.lz");
const u8 gBattleAnimSpriteGfx_FocusEnergy[] = INCBIN_U8("graphics/battle_anims/sprites/focus_energy.4bpp.lz");
const u8 gBattleAnimSpritePal_FocusEnergy[] = INCBIN_U8("graphics/battle_anims/sprites/focus_energy.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SphereToCube[] = INCBIN_U8("graphics/battle_anims/sprites/sphere_to_cube.4bpp.lz");
const u8 gBattleAnimSpritePal_SphereToCube[] = INCBIN_U8("graphics/battle_anims/sprites/sphere_to_cube.gbapal.lz");

const u8 gBattleAnimBgImage_Psychic[] = INCBIN_U8("graphics/battle_anims/backgrounds/psychic.4bpp.lz");
const u8 gBattleAnimBgPalette_Psychic[] = INCBIN_U8("graphics/battle_anims/backgrounds/psychic.gbapal.lz");
const u8 gBattleAnimBgTilemap_Psychic[] = INCBIN_U8("graphics/battle_anims/backgrounds/psychic.bin.lz");
const u8 gBattleAnimSpriteGfx_Eye[] = INCBIN_U8("graphics/battle_anims/sprites/eye.4bpp.lz");
const u8 gBattleAnimSpritePal_Eye[] = INCBIN_U8("graphics/battle_anims/sprites/eye.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Tendrils[] = INCBIN_U8("graphics/battle_anims/sprites/tendrils.4bpp.lz");
const u8 gBattleAnimSpritePal_Tendrils[] = INCBIN_U8("graphics/battle_anims/sprites/tendrils.gbapal.lz");

const u8 gHealthboxSinglesPlayerGfx[] = INCBIN_U8("graphics/battle_interface/healthbox_singles_player.4bpp.lz");
const u8 gHealthboxSinglesOpponentGfx[] = INCBIN_U8("graphics/battle_interface/healthbox_singles_opponent.4bpp.lz");
const u8 gHealthboxDoublesPlayerGfx[] = INCBIN_U8("graphics/battle_interface/healthbox_doubles_player.4bpp.lz");
const u8 gHealthboxDoublesOpponentGfx[] = INCBIN_U8("graphics/battle_interface/healthbox_doubles_opponent.4bpp.lz");
const u8 gHealthboxSafariGfx[] = INCBIN_U8("graphics/battle_interface/healthbox_safari.4bpp.lz");

const u8 gUnusedGfx_Shadow[] = INCBIN_U8("graphics/unused/shadow.4bpp.lz");
const u8 gUnusedPal_Shadow[] = INCBIN_U8("graphics/unused/shadow.gbapal.lz");

const u8 gBattleAnimSpriteGfx_LockOn[] = INCBIN_U8("graphics/battle_anims/sprites/lock_on.4bpp.lz");
const u8 gBattleAnimSpritePal_LockOn[] = INCBIN_U8("graphics/battle_anims/sprites/lock_on.gbapal.lz");
const u8 gBattleAnimSpriteGfx_OpeningEye[] = INCBIN_U8("graphics/battle_anims/sprites/opening_eye.4bpp.lz");
const u8 gBattleAnimSpritePal_OpeningEye[] = INCBIN_U8("graphics/battle_anims/sprites/opening_eye.gbapal.lz");
const u8 gBattleAnimSpriteGfx_RoundWhiteHalo[] = INCBIN_U8("graphics/battle_anims/sprites/round_white_halo.4bpp.lz");
const u8 gBattleAnimSpritePal_RoundWhiteHalo[] = INCBIN_U8("graphics/battle_anims/sprites/round_white_halo.gbapal.lz");
const u8 gBattleAnimSpriteGfx_TealAlert[] = INCBIN_U8("graphics/battle_anims/sprites/teal_alert.4bpp.lz");
const u8 gBattleAnimSpritePal_TealAlert[] = INCBIN_U8("graphics/battle_anims/sprites/teal_alert.gbapal.lz");
const u8 gBattleAnimSpriteGfx_FangAttack[] = INCBIN_U8("graphics/battle_anims/sprites/fang_attack.4bpp.lz");
const u8 gBattleAnimSpritePal_FangAttack[] = INCBIN_U8("graphics/battle_anims/sprites/fang_attack.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PurpleHandOutline[] = INCBIN_U8("graphics/battle_anims/sprites/purple_hand_outline.4bpp.lz");
const u8 gBattleAnimSpritePal_PurpleHandOutline[] = INCBIN_U8("graphics/battle_anims/sprites/purple_hand_outline.gbapal.lz");

const u8 gBattleAnimMaskImage_Curse[] = INCBIN_U8("graphics/battle_anims/masks/curse.4bpp.lz");
const u8 gBattleAnimMaskTilemap_Curse[] = INCBIN_U8("graphics/battle_anims/masks/curse.bin.lz");

const u8 gBattleAnimSpriteGfx_Pencil[] = INCBIN_U8("graphics/battle_anims/sprites/pencil.4bpp.lz");
const u8 gBattleAnimSpritePal_Pencil[] = INCBIN_U8("graphics/battle_anims/sprites/pencil.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Spiral[] = INCBIN_U8("graphics/battle_anims/sprites/spiral.4bpp.lz");
const u8 gBattleAnimSpritePal_Spiral[] = INCBIN_U8("graphics/battle_anims/sprites/spiral.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Moon[] = INCBIN_U8("graphics/battle_anims/sprites/moon.4bpp.lz");
const u8 gBattleAnimSpritePal_Moon[] = INCBIN_U8("graphics/battle_anims/sprites/moon.gbapal.lz");
const u8 gBattleAnimSpriteGfx_GreenSparkle[] = INCBIN_U8("graphics/battle_anims/sprites/green_sparkle.4bpp.lz");
const u8 gBattleAnimSpritePal_GreenSparkle[] = INCBIN_U8("graphics/battle_anims/sprites/green_sparkle.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SnoreZ[] = INCBIN_U8("graphics/battle_anims/sprites/snore_z.4bpp.lz");
const u8 gBattleAnimSpritePal_SnoreZ[] = INCBIN_U8("graphics/battle_anims/sprites/snore_z.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Explosion[] = INCBIN_U8("graphics/battle_anims/sprites/explosion.4bpp.lz");
const u8 gBattleAnimSpritePal_Explosion[] = INCBIN_U8("graphics/battle_anims/sprites/explosion.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Nail[] = INCBIN_U8("graphics/battle_anims/sprites/nail.4bpp.lz");
const u8 gBattleAnimSpritePal_Nail[] = INCBIN_U8("graphics/battle_anims/sprites/nail.gbapal.lz");
const u8 gBattleAnimSpriteGfx_GhostlySpirit[] = INCBIN_U8("graphics/battle_anims/sprites/ghostly_spirit.4bpp.lz");
const u8 gBattleAnimSpritePal_GhostlySpirit[] = INCBIN_U8("graphics/battle_anims/sprites/ghostly_spirit.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WarmRock[] = INCBIN_U8("graphics/battle_anims/sprites/warm_rock.4bpp.lz");
const u8 gBattleAnimSpritePal_WarmRock[] = INCBIN_U8("graphics/battle_anims/sprites/warm_rock.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PunchImpact[] = INCBIN_U8("graphics/battle_anims/sprites/punch_impact.4bpp.lz");
const u8 gBattleAnimSpritePal_PunchImpact[] = INCBIN_U8("graphics/battle_anims/sprites/punch_impact.gbapal.lz");
const u8 gBattleAnimSpriteGfx_BreakingEgg[] = INCBIN_U8("graphics/battle_anims/sprites/breaking_egg.4bpp.lz");
const u8 gBattleAnimSpritePal_BreakingEgg[] = INCBIN_U8("graphics/battle_anims/sprites/breaking_egg.gbapal.lz");
const u8 gBattleAnimSpriteGfx_ThinRing[] = INCBIN_U8("graphics/battle_anims/sprites/thin_ring.4bpp.lz");
const u8 gBattleAnimSpritePal_ThinRing[] = INCBIN_U8("graphics/battle_anims/sprites/thin_ring.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MusicNotes2[] = INCBIN_U8("graphics/battle_anims/sprites/music_notes_2.4bpp.lz");
const u8 gBattleAnimSpritePal_MusicNotes2[] = INCBIN_U8("graphics/battle_anims/sprites/music_notes_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Bell[] = INCBIN_U8("graphics/battle_anims/sprites/bell.4bpp.lz");
const u8 gBattleAnimSpritePal_Bell[] = INCBIN_U8("graphics/battle_anims/sprites/bell.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SpeedDust[] = INCBIN_U8("graphics/battle_anims/sprites/speed_dust.4bpp.lz");
const u8 gBattleAnimSpritePal_SpeedDust[] = INCBIN_U8("graphics/battle_anims/sprites/speed_dust.gbapal.lz");
const u8 gBattleAnimSpriteGfx_TornMetal[] = INCBIN_U8("graphics/battle_anims/sprites/torn_metal.4bpp.lz");
const u8 gBattleAnimSpriteGfx_ThoughtBubble[] = INCBIN_U8("graphics/battle_anims/sprites/thought_bubble.4bpp.lz");
const u8 gBattleAnimSpritePal_ThoughtBubble[] = INCBIN_U8("graphics/battle_anims/sprites/thought_bubble.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Finger[] = INCBIN_U8("graphics/battle_anims/sprites/finger.4bpp.lz");
const u8 gBattleAnimSpritePal_Finger[] = INCBIN_U8("graphics/battle_anims/sprites/finger.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MagentaHeart[] = INCBIN_U8("graphics/battle_anims/sprites/magenta_heart.4bpp.lz");
const u8 gBattleAnimSpritePal_PinkHeart[] = INCBIN_U8("graphics/battle_anims/sprites/pink_heart.gbapal.lz");
const u8 gBattleAnimSpritePal_MagentaHeart[] = INCBIN_U8("graphics/battle_anims/sprites/magenta_heart.gbapal.lz");
const u8 gBattleAnimSpritePal_RedHeart[] = INCBIN_U8("graphics/battle_anims/sprites/red_heart.gbapal.lz");

const u8 gBattleAnimBgImage_Attract[] = INCBIN_U8("graphics/battle_anims/backgrounds/attract.4bpp.lz");
const u8 gBattleAnimBgPalette_Attract[] = INCBIN_U8("graphics/battle_anims/backgrounds/attract.gbapal.lz");
const u8 gBattleAnimBgTilemap_Attract[] = INCBIN_U8("graphics/battle_anims/backgrounds/attract.bin.lz");

const u8 gBattleAnimSpriteGfx_RedOrb[] = INCBIN_U8("graphics/battle_anims/sprites/red_orb.4bpp.lz");
const u8 gBattleAnimSpritePal_RedOrb[] = INCBIN_U8("graphics/battle_anims/sprites/red_orb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_CircleOfLight[] = INCBIN_U8("graphics/battle_anims/sprites/circle_of_light.4bpp.lz");
const u8 gBattleAnimSpriteGfx_ElectricOrbs[] = INCBIN_U8("graphics/battle_anims/sprites/electric_orbs.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Electricity[] = INCBIN_U8("graphics/battle_anims/sprites/electricity.4bpp.lz");
const u8 gBattleAnimSpritePal_ElectricOrbs[] = INCBIN_U8("graphics/battle_anims/sprites/electric_orbs.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Finger2[] = INCBIN_U8("graphics/battle_anims/sprites/finger_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_MovementWaves[] = INCBIN_U8("graphics/battle_anims/sprites/movement_waves.4bpp.lz");
const u8 gBattleAnimSpritePal_MovementWaves[] = INCBIN_U8("graphics/battle_anims/sprites/movement_waves.gbapal.lz");

const u8 gBattleAnimBgPalette_ScaryFace[] = INCBIN_U8("graphics/battle_anims/backgrounds/scary_face.gbapal.lz");
const u8 gBattleAnimBgImage_ScaryFace[] = INCBIN_U8("graphics/battle_anims/backgrounds/scary_face.4bpp.lz");

const u8 gBattleAnimSpritePal_EyeSparkle[] = INCBIN_U8("graphics/battle_anims/sprites/eye_sparkle.gbapal.lz");
const u8 gBattleAnimSpriteGfx_EyeSparkle[] = INCBIN_U8("graphics/battle_anims/sprites/eye_sparkle.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Anger[] = INCBIN_U8("graphics/battle_anims/sprites/anger.4bpp.lz");
const u8 gBattleAnimSpritePal_Anger[] = INCBIN_U8("graphics/battle_anims/sprites/anger.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Conversion[] = INCBIN_U8("graphics/battle_anims/sprites/conversion.4bpp.lz");
const u8 gBattleAnimSpritePal_Conversion[] = INCBIN_U8("graphics/battle_anims/sprites/conversion.gbapal.lz");
const u8 gBattleAnimSpritePal_Angel[] = INCBIN_U8("graphics/battle_anims/sprites/angel.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Angel[] = INCBIN_U8("graphics/battle_anims/sprites/angel.4bpp.lz");
const u8 gBattleAnimSpritePal_Devil[] = INCBIN_U8("graphics/battle_anims/sprites/devil.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Devil[] = INCBIN_U8("graphics/battle_anims/sprites/devil.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Swipe[] = INCBIN_U8("graphics/battle_anims/sprites/swipe.4bpp.lz");
const u8 gBattleAnimSpritePal_Swipe[] = INCBIN_U8("graphics/battle_anims/sprites/swipe.gbapal.lz");
const u8 gBattleAnimSpritePal_Roots[] = INCBIN_U8("graphics/battle_anims/sprites/roots.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Roots[] = INCBIN_U8("graphics/battle_anims/sprites/roots.4bpp.lz");
const u8 gBattleAnimSpritePal_ItemBag[] = INCBIN_U8("graphics/battle_anims/sprites/item_bag.gbapal.lz");
const u8 gBattleAnimSpriteGfx_ItemBag[] = INCBIN_U8("graphics/battle_anims/sprites/item_bag.4bpp.lz");
const u8 gBattleAnimSpritePal_TriAttackTriangle[] = INCBIN_U8("graphics/battle_anims/sprites/tri_attack_triangle.gbapal.lz");
const u8 gBattleAnimSpriteGfx_TriAttackTriangle[] = INCBIN_U8("graphics/battle_anims/sprites/tri_attack_triangle.4bpp.lz");
const u8 gBattleAnimSpritePal_LetterZ[] = INCBIN_U8("graphics/battle_anims/sprites/letter_z.gbapal.lz");
const u8 gBattleAnimSpriteGfx_LetterZ[] = INCBIN_U8("graphics/battle_anims/sprites/letter_z.4bpp.lz");

const u8 gBattleAnimBgPalette_Impact[] = INCBIN_U8("graphics/battle_anims/backgrounds/impact.gbapal.lz");
const u8 gBattleAnimBgImage_Impact[] = INCBIN_U8("graphics/battle_anims/backgrounds/impact.4bpp.lz");
const u8 gBattleAnimBgTilemap_ImpactOpponent[] = INCBIN_U8("graphics/battle_anims/backgrounds/impact_opponent.bin.lz");
const u8 gBattleAnimBgTilemap_ImpactPlayer[] = INCBIN_U8("graphics/battle_anims/backgrounds/impact_player.bin.lz");
const u8 gBattleAnimBgTilemap_ImpactContests[] = INCBIN_U8("graphics/battle_anims/backgrounds/impact_contests.bin.lz");
const u8 gBattleAnimSpriteGfx_JaggedMusicNote[] = INCBIN_U8("graphics/battle_anims/sprites/jagged_music_note.4bpp.lz");
const u8 gBattleAnimSpritePal_JaggedMusicNote[] = INCBIN_U8("graphics/battle_anims/sprites/jagged_music_note.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Spotlight[] = INCBIN_U8("graphics/battle_anims/sprites/spotlight.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Pokeball[] = INCBIN_U8("graphics/battle_anims/sprites/pokeball.4bpp.lz");
const u8 gBattleAnimSpritePal_Pokeball[] = INCBIN_U8("graphics/battle_anims/sprites/pokeball.gbapal.lz");
const u8 gBattleAnimSpriteGfx_RapidSpin[] = INCBIN_U8("graphics/battle_anims/sprites/rapid_spin.4bpp.lz");
const u8 gBattleAnimSpritePal_RapidSpin[] = INCBIN_U8("graphics/battle_anims/sprites/rapid_spin.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MilkBottle[] = INCBIN_U8("graphics/battle_anims/sprites/milk_bottle.4bpp.lz");
const u8 gBattleAnimSpritePal_MilkBottle[] = INCBIN_U8("graphics/battle_anims/sprites/milk_bottle.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WispFire[] = INCBIN_U8("graphics/battle_anims/sprites/wisp_fire.4bpp.lz");
const u8 gBattleAnimSpritePal_WispOrb[] = INCBIN_U8("graphics/battle_anims/sprites/wisp_orb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WispOrb[] = INCBIN_U8("graphics/battle_anims/sprites/wisp_orb.4bpp.lz");
const u8 gBattleAnimSpriteGfx_GoldStars[] = INCBIN_U8("graphics/battle_anims/sprites/gold_stars.4bpp.lz");
const u8 gBattleAnimSpritePal_GoldStars[] = INCBIN_U8("graphics/battle_anims/sprites/gold_stars.gbapal.lz");
const u8 gBattleAnimSpriteGfx_EclipsingOrb[] = INCBIN_U8("graphics/battle_anims/sprites/eclipsing_orb.4bpp.lz");
const u8 gBattleAnimSpritePal_EclipsingOrb[] = INCBIN_U8("graphics/battle_anims/sprites/eclipsing_orb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PinkPetal[] = INCBIN_U8("graphics/battle_anims/sprites/pink_petal.4bpp.lz");
const u8 gBattleAnimSpritePal_PinkPetal[] = INCBIN_U8("graphics/battle_anims/sprites/pink_petal.gbapal.lz");
const u8 gBattleAnimSpriteGfx_GrayOrb[] = INCBIN_U8("graphics/battle_anims/sprites/gray_orb.4bpp.lz");
const u8 gBattleAnimSpritePal_GrayOrb[] = INCBIN_U8("graphics/battle_anims/sprites/gray_orb.gbapal.lz");
const u8 gBattleAnimSpritePal_BlueOrb[] = INCBIN_U8("graphics/battle_anims/sprites/blue_orb.gbapal.lz");
const u8 gBattleAnimSpritePal_RedOrb2[] = INCBIN_U8("graphics/battle_anims/sprites/red_orb_2.gbapal.lz");
const u8 gBattleAnimBgImage_Drill[] = INCBIN_U8("graphics/battle_anims/backgrounds/drill.4bpp.lz");
const u8 gBattleAnimBgPalette_Drill[] = INCBIN_U8("graphics/battle_anims/backgrounds/drill.gbapal.lz");
const u8 gBattleAnimBgPalette_Sky[] = INCBIN_U8("graphics/battle_anims/backgrounds/sky.gbapal.lz");
const u8 gBattleAnimBgTilemap_Drill[] = INCBIN_U8("graphics/battle_anims/backgrounds/drill.bin.lz");
const u8 gBattleAnimBgTilemap_DrillContests[] = INCBIN_U8("graphics/battle_anims/backgrounds/drill_contests.bin.lz");
const u8 gBattleAnimBgImage_Aurora[] = INCBIN_U8("graphics/battle_anims/backgrounds/aurora.4bpp.lz");
const u8 gBattleAnimBgPalette_Aurora[] = INCBIN_U8("graphics/battle_anims/backgrounds/aurora.gbapal.lz");
const u8 gBattleAnimBgTilemap_Aurora[] = INCBIN_U8("graphics/battle_anims/backgrounds/aurora.bin.lz");
const u8 gBattleAnimBgTilemap_HighspeedOpponent[] = INCBIN_U8("graphics/battle_anims/backgrounds/highspeed_opponent.bin.lz");
const u8 gBattleAnimBgPalette_Highspeed[] = INCBIN_U8("graphics/battle_anims/backgrounds/highspeed.gbapal.lz");
const u8 gBattleAnimBgPalette_Bug[] = INCBIN_U8("graphics/battle_anims/backgrounds/bug.gbapal.lz");
const u8 gBattleAnimBgImage_Highspeed[] = INCBIN_U8("graphics/battle_anims/backgrounds/highspeed.4bpp.lz");
const u8 gBattleAnimBgTilemap_HighspeedPlayer[] = INCBIN_U8("graphics/battle_anims/backgrounds/highspeed_player.bin.lz");
const u8 gBattleAnimMaskImage_LightBeam[] = INCBIN_U8("graphics/battle_anims/masks/light_beam.4bpp.lz");
const u8 gBattleAnimMaskPalette_LightBeam[] = INCBIN_U8("graphics/battle_anims/masks/light_beam.gbapal.lz");
const u8 gBattleAnimMaskTilemap_LightBeam[] = INCBIN_U8("graphics/battle_anims/masks/light_beam.bin.lz");
const u8 gBattleAnimBgTilemap_GuillotineOpponent[] = INCBIN_U8("graphics/battle_anims/backgrounds/guillotine_opponent.bin.lz");
const u8 gBattleAnimBgTilemap_GuillotinePlayer[] = INCBIN_U8("graphics/battle_anims/backgrounds/guillotine_player.bin.lz");
const u8 gBattleAnimBgTilemap_GuillotineContests[] = INCBIN_U8("graphics/battle_anims/backgrounds/guillotine_contests.bin.lz");
const u8 gBattleAnimBgImage_Guillotine[] = INCBIN_U8("graphics/battle_anims/backgrounds/guillotine.4bpp.lz");
const u8 gBattleAnimBgPalette_Guillotine[] = INCBIN_U8("graphics/battle_anims/backgrounds/guillotine.gbapal.lz");
const u8 gBattleAnimBgImage_Thunder[] = INCBIN_U8("graphics/battle_anims/backgrounds/thunder.4bpp.lz");
const u8 gBattleAnimBgPalette_Thunder[] = INCBIN_U8("graphics/battle_anims/backgrounds/thunder.gbapal.lz");
const u8 gBattleAnimBgTilemap_Thunder[] = INCBIN_U8("graphics/battle_anims/backgrounds/thunder.bin.lz");
const u8 gBattleAnimSpriteGfx_PainSplit[] = INCBIN_U8("graphics/battle_anims/sprites/pain_split.4bpp.lz");
const u8 gBattleAnimSpritePal_PainSplit[] = INCBIN_U8("graphics/battle_anims/sprites/pain_split.gbapal.lz");
const u8 gBattleAnimSpriteGfx_HandsAndFeet[] = INCBIN_U8("graphics/battle_anims/sprites/hands_and_feet.4bpp.lz");
const u8 gBattleAnimSpritePal_HandsAndFeet[] = INCBIN_U8("graphics/battle_anims/sprites/hands_and_feet.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Confetti[] = INCBIN_U8("graphics/battle_anims/sprites/confetti.4bpp.lz");
const u8 gBattleAnimSpritePal_Confetti[] = INCBIN_U8("graphics/battle_anims/sprites/confetti.gbapal.lz");
const u8 gSubstituteDollPal[] = INCBIN_U8("graphics/battle_anims/sprites/substitute.gbapal.lz");
const u8 gSubstituteDollGfx[] = INCBIN_U8("graphics/battle_anims/sprites/substitute.4bpp.lz");
const u8 gSubstituteDollTilemap[] = INCBIN_U8("graphics/battle_anims/sprites/substitute.bin.lz");
const u8 gBattleAnimSpriteGfx_GreenStar[] = INCBIN_U8("graphics/battle_anims/sprites/green_star.4bpp.lz");
const u8 gBattleAnimSpritePal_GreenStar[] = INCBIN_U8("graphics/battle_anims/sprites/green_star.gbapal.lz");
const u8 gConfetti_Gfx[] = INCBIN_U8("graphics/misc/confetti.4bpp.lz");
const u8 gConfetti_Pal[] = INCBIN_U8("graphics/misc/confetti.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PinkCloud[] = INCBIN_U8("graphics/battle_anims/sprites/pink_cloud.4bpp.lz");
const u8 gBattleAnimSpritePal_PinkCloud[] = INCBIN_U8("graphics/battle_anims/sprites/pink_cloud.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SweatDrop[] = INCBIN_U8("graphics/battle_anims/sprites/sweat_drop.4bpp.lz");
const u8 gBattleAnimSpritePal_SweatDrop[] = INCBIN_U8("graphics/battle_anims/sprites/sweat_drop.gbapal.lz");
const u8 gStatAnim_Gfx[] = INCBIN_U8("graphics/battle_anims/stat_change/tiles.4bpp.lz");
const u8 gStatAnim_Increase_Tilemap[] = INCBIN_U8("graphics/battle_anims/stat_change/increase.bin.lz");
const u8 gStatAnim_Decrease_Tilemap[] = INCBIN_U8("graphics/battle_anims/stat_change/decrease.bin.lz");
const u8 gStatAnim_Defense_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/defense.gbapal.lz");
const u8 gStatAnim_Attack_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/attack.gbapal.lz");
const u8 gStatAnim_Accuracy_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/accuracy.gbapal.lz");
const u8 gStatAnim_Speed_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/speed.gbapal.lz");
const u8 gStatAnim_Multiple_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/multiple.gbapal.lz");
const u8 gStatAnim_Evasion_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/evasion.gbapal.lz");
const u8 gStatAnim_SpAttack_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/sp_attack.gbapal.lz");
const u8 gStatAnim_SpDefense_Pal[] = INCBIN_U8("graphics/battle_anims/stat_change/sp_defense.gbapal.lz");
const u8 gCureBubblesGfx[] = INCBIN_U8("graphics/battle_anims/masks/cure_bubbles.4bpp.lz");
const u8 gCureBubblesPal[] = INCBIN_U8("graphics/battle_anims/masks/cure_bubbles.gbapal.lz");
const u8 gCureBubblesTilemap[] = INCBIN_U8("graphics/battle_anims/masks/cure_bubbles.bin.lz");
const u8 gBattleAnimSpritePal_PurpleScratch[] = INCBIN_U8("graphics/battle_anims/sprites/purple_scratch.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PurpleScratch[] = INCBIN_U8("graphics/battle_anims/sprites/purple_scratch.4bpp.lz");
const u8 gBattleAnimSpriteGfx_PurpleSwipe[] = INCBIN_U8("graphics/battle_anims/sprites/purple_swipe.4bpp.lz");
const u8 gBattleAnimSpriteGfx_GuardRing[] = INCBIN_U8("graphics/battle_anims/sprites/guard_ring.4bpp.lz");
const u8 gBattleAnimSpritePal_GuardRing[] = INCBIN_U8("graphics/battle_anims/sprites/guard_ring.gbapal.lz");
const u8 gBattleAnimSpriteGfx_TagHand[] = INCBIN_U8("graphics/battle_anims/sprites/tag_hand.4bpp.lz");
const u8 gBattleAnimSpriteGfx_NoiseLine[] = INCBIN_U8("graphics/battle_anims/sprites/noise_line.4bpp.lz");
const u8 UnusedLevelupAnimationGfx[] = INCBIN_U8("graphics/battle_anims/masks/unused_level_up.4bpp.lz");
const u8 UnusedLevelupAnimationTilemap[] = INCBIN_U8("graphics/battle_anims/masks/unused_level_up.bin.lz");
const u8 gBattleAnimSpriteGfx_SmallRedEye[] = INCBIN_U8("graphics/battle_anims/sprites/small_red_eye.4bpp.lz");
const u8 gBattleAnimSpritePal_SmallRedEye[] = INCBIN_U8("graphics/battle_anims/sprites/small_red_eye.gbapal.lz");
const u8 gBattleAnimSpriteGfx_HollowOrb[] = INCBIN_U8("graphics/battle_anims/sprites/hollow_orb.4bpp.lz");
const u8 gBattleAnimSpritePal_HollowOrb[] = INCBIN_U8("graphics/battle_anims/sprites/hollow_orb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_XSign[] = INCBIN_U8("graphics/battle_anims/sprites/x_sign.4bpp.lz");
const u8 gBattleAnimSpriteGfx_BluegreenOrb[] = INCBIN_U8("graphics/battle_anims/sprites/bluegreen_orb.4bpp.lz");
const u8 gBattleAnimSpritePal_BluegreenOrb[] = INCBIN_U8("graphics/battle_anims/sprites/bluegreen_orb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PawPrint[] = INCBIN_U8("graphics/battle_anims/sprites/paw_print.4bpp.lz");
const u8 gBattleAnimSpritePal_PawPrint[] = INCBIN_U8("graphics/battle_anims/sprites/paw_print.gbapal.lz");
const u8 gBattleAnimSpriteGfx_PurpleFlame[] = INCBIN_U8("graphics/battle_anims/sprites/purple_flame.4bpp.lz");
const u8 gBattleAnimSpritePal_PurpleFlame[] = INCBIN_U8("graphics/battle_anims/sprites/purple_flame.gbapal.lz");
const u8 gBattleAnimSpriteGfx_RedBall[] = INCBIN_U8("graphics/battle_anims/sprites/red_ball.4bpp.lz");
const u8 gBattleAnimSpritePal_RedBall[] = INCBIN_U8("graphics/battle_anims/sprites/red_ball.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SmellingsaltEffect[] = INCBIN_U8("graphics/battle_anims/sprites/smellingsalt_effect.4bpp.lz");
const u8 gBattleAnimSpritePal_SmellingsaltEffect[] = INCBIN_U8("graphics/battle_anims/sprites/smellingsalt_effect.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MagnifyingGlass[] = INCBIN_U8("graphics/battle_anims/sprites/magnifying_glass.4bpp.lz");
const u8 gBattleAnimSpritePal_MagnifyingGlass[] = INCBIN_U8("graphics/battle_anims/sprites/magnifying_glass.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Meteor[] = INCBIN_U8("graphics/battle_anims/sprites/meteor.4bpp.lz");
const u8 gBattleAnimSpritePal_Meteor[] = INCBIN_U8("graphics/battle_anims/sprites/meteor.gbapal.lz");
const u8 gBattleAnimSpriteGfx_FlatRock[] = INCBIN_U8("graphics/battle_anims/sprites/flat_rock.4bpp.lz");
const u8 gBattleAnimSpritePal_FlatRock[] = INCBIN_U8("graphics/battle_anims/sprites/flat_rock.gbapal.lz");

#include "data/graphics/pokemon.h"
#include "data/graphics/trainers.h"

const u8 gMonIcon_QuestionMark[] = INCBIN_U8("graphics/pokemon/question_mark/icon.4bpp");
const u8 gMonFootprint_QuestionMark[] = INCBIN_U8("graphics/pokemon/question_mark/footprint.1bpp");
const u8 gVersusFrameGfx[] = INCBIN_U8("graphics/battle_transitions/vs_frame.4bpp.lz");
const u8 gVersusFrameTilemap[] = INCBIN_U8("graphics/battle_transitions/vs_frame.bin.lz");
const u8 gVersusFramePal[] = INCBIN_U8("graphics/battle_transitions/vs_frame.gbapal.lz");
const u8 gVsLettersGfx[] = INCBIN_U8("graphics/battle_transitions/vs.4bpp.lz");
const u8 gBattleEnvironmentTiles_TallGrass[] = INCBIN_U8("graphics/battle_environment/tall_grass/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_TallGrass[] = INCBIN_U8("graphics/battle_environment/tall_grass/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_TallGrass[] = INCBIN_U8("graphics/battle_environment/tall_grass/map.bin.lz");
const u8 gBattleEnvironmentTiles_LongGrass[] = INCBIN_U8("graphics/battle_environment/long_grass/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_LongGrass[] = INCBIN_U8("graphics/battle_environment/long_grass/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_LongGrass[] = INCBIN_U8("graphics/battle_environment/long_grass/map.bin.lz");
const u8 gBattleEnvironmentTiles_Sand[] = INCBIN_U8("graphics/battle_environment/sand/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_Sand[] = INCBIN_U8("graphics/battle_environment/sand/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_Sand[] = INCBIN_U8("graphics/battle_environment/sand/map.bin.lz");
const u8 gBattleEnvironmentTiles_Underwater[] = INCBIN_U8("graphics/battle_environment/underwater/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_Underwater[] = INCBIN_U8("graphics/battle_environment/underwater/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_Underwater[] = INCBIN_U8("graphics/battle_environment/underwater/map.bin.lz");
const u8 gBattleEnvironmentTiles_Water[] = INCBIN_U8("graphics/battle_environment/water/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_Water[] = INCBIN_U8("graphics/battle_environment/water/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_Water[] = INCBIN_U8("graphics/battle_environment/water/map.bin.lz");
const u8 gBattleEnvironmentTiles_PondWater[] = INCBIN_U8("graphics/battle_environment/pond_water/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_PondWater[] = INCBIN_U8("graphics/battle_environment/pond_water/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_PondWater[] = INCBIN_U8("graphics/battle_environment/pond_water/map.bin.lz");
const u8 gBattleEnvironmentTiles_Rock[] = INCBIN_U8("graphics/battle_environment/rock/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_Rock[] = INCBIN_U8("graphics/battle_environment/rock/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_Rock[] = INCBIN_U8("graphics/battle_environment/rock/map.bin.lz");
const u8 gBattleEnvironmentTiles_Cave[] = INCBIN_U8("graphics/battle_environment/cave/tiles.4bpp.lz");
const u8 gBattleEnvironmentPalette_Cave[] = INCBIN_U8("graphics/battle_environment/cave/palette.gbapal.lz");
const u8 gBattleEnvironmentTilemap_Cave[] = INCBIN_U8("graphics/battle_environment/cave/map.bin.lz");
const u8 gBattleEnvironmentPalette_Plain[] = INCBIN_U8("graphics/battle_environment/plain/palette.gbapal.lz");
const u8 gBattleEnvironmentTiles_Building[] = INCBIN_U8("graphics/battle_environment/building/tiles.4bpp.lz");

// this is also used for link battles
const u8 gBattleEnvironmentPalette_BattleTower[] = INCBIN_U8("graphics/battle_environment/stadium/battle_tower.gbapal.lz");
const u8 gBattleEnvironmentTilemap_Building[] = INCBIN_U8("graphics/battle_environment/building/map.bin.lz");
const u8 gBattleEnvironmentTiles_Stadium[] = INCBIN_U8("graphics/battle_environment/stadium/tiles.4bpp.lz");
const u8 gBattleEnvironmentTilemap_Stadium[] = INCBIN_U8("graphics/battle_environment/stadium/map.bin.lz");
const u8 gBattleEnvironmentPalette_Building[] = INCBIN_U8("graphics/battle_environment/building/palette.gbapal.lz");
const u8 gBattleEnvironmentPalette_Kyogre[] = INCBIN_U8("graphics/battle_environment/water/kyogre.gbapal.lz");
const u8 gBattleEnvironmentPalette_Groudon[] = INCBIN_U8("graphics/battle_environment/cave/groudon.gbapal.lz");
const u8 gBattleEnvironmentPalette_BuildingGym[] = INCBIN_U8("graphics/battle_environment/building/gym.gbapal.lz");
const u8 gBattleEnvironmentPalette_BuildingLeader[] = INCBIN_U8("graphics/battle_environment/building/leader.gbapal.lz");
const u8 gBattleEnvironmentPalette_StadiumAqua[] = INCBIN_U8("graphics/battle_environment/stadium/aqua.gbapal.lz");
const u8 gBattleEnvironmentPalette_StadiumMagma[] = INCBIN_U8("graphics/battle_environment/stadium/magma.gbapal.lz");
const u8 gBattleEnvironmentPalette_StadiumSidney[] = INCBIN_U8("graphics/battle_environment/stadium/sidney.gbapal.lz");
const u8 gBattleEnvironmentPalette_StadiumPhoebe[] = INCBIN_U8("graphics/battle_environment/stadium/phoebe.gbapal.lz");
const u8 gBattleEnvironmentPalette_StadiumGlacia[] = INCBIN_U8("graphics/battle_environment/stadium/glacia.gbapal.lz");
const u8 gBattleEnvironmentPalette_StadiumDrake[] = INCBIN_U8("graphics/battle_environment/stadium/drake.gbapal.lz");
const u8 gBattleEnvironmentPalette_StadiumSteven[] = INCBIN_U8("graphics/battle_environment/stadium/palette7.gbapal.lz");
const u8 gBattleEnvironmentAnimTiles_TallGrass[] = INCBIN_U8("graphics/battle_environment/tall_grass/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_TallGrass[] = INCBIN_U8("graphics/battle_environment/tall_grass/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_LongGrass[] = INCBIN_U8("graphics/battle_environment/long_grass/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_LongGrass[] = INCBIN_U8("graphics/battle_environment/long_grass/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_Sand[] = INCBIN_U8("graphics/battle_environment/sand/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_Sand[] = INCBIN_U8("graphics/battle_environment/sand/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_Underwater[] = INCBIN_U8("graphics/battle_environment/underwater/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_Underwater[] = INCBIN_U8("graphics/battle_environment/underwater/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_Water[] = INCBIN_U8("graphics/battle_environment/water/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_Water[] = INCBIN_U8("graphics/battle_environment/water/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_PondWater[] = INCBIN_U8("graphics/battle_environment/pond_water/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_PondWater[] = INCBIN_U8("graphics/battle_environment/pond_water/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_Rock[] = INCBIN_U8("graphics/battle_environment/rock/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_Rock[] = INCBIN_U8("graphics/battle_environment/rock/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_Cave[] = INCBIN_U8("graphics/battle_environment/cave/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_Cave[] = INCBIN_U8("graphics/battle_environment/cave/anim_map.bin.lz");
const u8 gBattleEnvironmentAnimTiles_Building[] = INCBIN_U8("graphics/battle_environment/building/anim_tiles.4bpp.lz");
const u8 gBattleEnvironmentAnimTilemap_Building[] = INCBIN_U8("graphics/battle_environment/building/anim_map.bin.lz");
const u8 gBattleAnimSpriteGfx_FlyingDirt[] = INCBIN_U8("graphics/battle_anims/sprites/flying_dirt.4bpp.lz");
const u8 gBattleAnimBgTilemap_Sandstorm[] = INCBIN_U8("graphics/battle_anims/backgrounds/sandstorm_brew.bin.lz");
const u8 gBattleAnimBgImage_Sandstorm[] = INCBIN_U8("graphics/battle_anims/backgrounds/sandstorm_brew.4bpp.lz");
const u8 gBattleAnimSpritePal_FlyingDirt[] = INCBIN_U8("graphics/battle_anims/sprites/flying_dirt.gbapal.lz");
const u8 gBattleAnimSpriteGfx_MetalSoundWaves[] = INCBIN_U8("graphics/battle_anims/sprites/metal_sound_waves.4bpp.lz");
const u8 gBattleAnimSpritePal_MetalSoundWaves[] = INCBIN_U8("graphics/battle_anims/sprites/metal_sound_waves.gbapal.lz");
const u8 gBattleAnimBgImage_Ice[] = INCBIN_U8("graphics/battle_anims/backgrounds/ice.4bpp.lz");
const u8 gBattleAnimBgPalette_Ice[] = INCBIN_U8("graphics/battle_anims/backgrounds/ice.gbapal.lz");
const u8 gBattleAnimBgTilemap_Ice[] = INCBIN_U8("graphics/battle_anims/backgrounds/ice.bin.lz");
const u8 gBattleAnimSpriteGfx_IcicleSpear[] = INCBIN_U8("graphics/battle_anims/sprites/icicle_spear.4bpp.lz");
const u8 gBattleAnimSpritePal_IcicleSpear[] = INCBIN_U8("graphics/battle_anims/sprites/icicle_spear.gbapal.lz");
const u8 gContestNextTurnGfx[] = INCBIN_U8("graphics/contest/nextturn.4bpp.lz");
const u8 gContestNextTurnNumbersGfx[] = INCBIN_U8("graphics/contest/nextturn_numbers.4bpp");
const u8 gContestNextTurnRandomGfx[] = INCBIN_U8("graphics/contest/nextturn_random.4bpp");
const u8 gBattleAnimSpriteGfx_GlowyRedOrb[] = INCBIN_U8("graphics/battle_anims/sprites/glowy_red_orb.4bpp.lz");
const u8 gBattleAnimSpritePal_GlowyRedOrb[] = INCBIN_U8("graphics/battle_anims/sprites/glowy_red_orb.gbapal.lz");
const u8 gBattleAnimSpritePal_GlowyGreenOrb[] = INCBIN_U8("graphics/battle_anims/sprites/glowy_green_orb.gbapal.lz");
const u8 gBattleAnimSpritePal_SleepPowder[] = INCBIN_U8("graphics/battle_anims/sprites/sleep_powder.gbapal.lz");
const u8 gBattleAnimSpritePal_StunSpore[] = INCBIN_U8("graphics/battle_anims/sprites/stun_spore.gbapal.lz");
const u8 gContestApplauseGfx[] = INCBIN_U8("graphics/contest/applause.4bpp.lz");
const u8 gContestApplauseMeterGfx[] = INCBIN_U8("graphics/contest/applause_meter.4bpp");
const u8 gContestPal[] = INCBIN_U8("graphics/contest/nextturn.gbapal");
const u8 gBattleAnimSpriteGfx_Splash[] = INCBIN_U8("graphics/battle_anims/sprites/splash.4bpp.lz");
const u8 gBattleAnimSpritePal_Splash[] = INCBIN_U8("graphics/battle_anims/sprites/splash.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SweatBead[] = INCBIN_U8("graphics/battle_anims/sprites/sweat_bead.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Pokeblock[] = INCBIN_U8("graphics/battle_anims/sprites/pokeblock.4bpp.lz");
const u8 gBattleAnimSpritePal_Pokeblock[] = INCBIN_U8("graphics/battle_anims/sprites/pokeblock.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Gem1[] = INCBIN_U8("graphics/battle_anims/sprites/gem_1.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Gem2[] = INCBIN_U8("graphics/battle_anims/sprites/gem_2.4bpp.lz");
const u8 gBattleAnimSpriteGfx_Gem3[] = INCBIN_U8("graphics/battle_anims/sprites/gem_3.4bpp.lz");
const u8 gBattleAnimSpritePal_Gem1[] = INCBIN_U8("graphics/battle_anims/sprites/gem_1.gbapal.lz");
const u8 gBattleAnimBgImage_InAir[] = INCBIN_U8("graphics/battle_anims/backgrounds/in_air.4bpp.lz");
const u8 gBattleAnimBgPalette_InAir[] = INCBIN_U8("graphics/battle_anims/backgrounds/in_air.gbapal.lz");
const u8 gBattleAnimBgTilemap_InAir[] = INCBIN_U8("graphics/battle_anims/backgrounds/in_air.bin.lz");
const u8 gBattleAnimSpriteGfx_Protect[] = INCBIN_U8("graphics/battle_anims/sprites/protect.4bpp.lz");
const u8 gBattleAnimSpritePal_Protect[] = INCBIN_U8("graphics/battle_anims/sprites/protect.gbapal.lz");
const u8 gBattleAnimBackgroundImageMuddyWater_Pal[] = INCBIN_U8("graphics/battle_anims/backgrounds/water_muddy.gbapal.lz");
const u8 gEnemyMonShadow_Gfx[] = INCBIN_U8("graphics/battle_interface/enemy_mon_shadow.4bpp.lz");
const u8 gBattleInterface_BallStatusBarGfx[] = INCBIN_U8("graphics/battle_interface/ball_status_bar.4bpp.lz");
const u8 gMonIcon_Egg[] = INCBIN_U8("graphics/pokemon/egg/icon.4bpp");
const u8 gBattleAnimBgImage_Ghost[] = INCBIN_U8("graphics/battle_anims/backgrounds/ghost.4bpp.lz");
const u8 gBattleAnimBgPalette_Ghost[] = INCBIN_U8("graphics/battle_anims/backgrounds/ghost.gbapal.lz");
const u8 gBattleAnimBgTilemap_Ghost[] = INCBIN_U8("graphics/battle_anims/backgrounds/ghost.bin.lz");
const u8 gBattleAnimSpritePal_WhipHit[] = INCBIN_U8("graphics/battle_anims/sprites/whip_hit.gbapal.lz");
const u8 gBattleAnimBgPalette_SolarBeam[] = INCBIN_U8("graphics/battle_anims/backgrounds/solarbeam.gbapal.lz");
const u8 gBattleAnimBgTilemap_SolarBeam[] = INCBIN_U8("graphics/battle_anims/backgrounds/solarbeam.bin.lz");
const u8 gBerryBlenderCenter_Gfx[] = INCBIN_U8("graphics/berry_blender/center.8bpp.lz");
const u8 gBerryBlenderOuter_Gfx[] = INCBIN_U8("graphics/berry_blender/outer.4bpp.lz");
const u8 gBerryBlenderOuter_Tilemap[] = INCBIN_U8("graphics/berry_blender/outer_map.bin.lz");
const u8 gBattleAnimBgPalette_Cosmic[] = INCBIN_U8("graphics/battle_anims/backgrounds/cosmic.gbapal.lz");
const u8 gBattleAnimBgImage_Cosmic[] = INCBIN_U8("graphics/battle_anims/backgrounds/cosmic.4bpp.lz");
const u8 gBattleAnimBgTilemap_Cosmic[] = INCBIN_U8("graphics/battle_anims/backgrounds/cosmic.bin.lz");
const u8 gBattleAnimSpritePal_SlamHit2[] = INCBIN_U8("graphics/battle_anims/sprites/slam_hit_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_SlamHit2[] = INCBIN_U8("graphics/battle_anims/sprites/slam_hit_2.4bpp.lz");
const u8 gBattleAnimFogTilemap[] = INCBIN_U8("graphics/battle_anims/backgrounds/fog.bin.lz");
const u8 gBattleAnimSpritePal_WeatherBall[] = INCBIN_U8("graphics/battle_anims/sprites/weather_ball.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WeatherBall[] = INCBIN_U8("graphics/battle_anims/sprites/weather_ball.4bpp.lz");
const u8 gBattleAnimBgTilemap_ScaryFacePlayer[] = INCBIN_U8("graphics/battle_anims/backgrounds/scary_face_player.bin.lz");
const u8 gBattleAnimBgTilemap_ScaryFaceOpponent[] = INCBIN_U8("graphics/battle_anims/backgrounds/scary_face_opponent.bin.lz");
const u8 gBattleAnimBgTilemap_ScaryFaceContest[] = INCBIN_U8("graphics/battle_anims/backgrounds/scary_face_contest.bin.lz");
const u8 gBattleAnimSpriteGfx_Hail[] = INCBIN_U8("graphics/battle_anims/sprites/hail.4bpp.lz");
const u8 gBattleAnimSpritePal_Hail[] = INCBIN_U8("graphics/battle_anims/sprites/hail.gbapal.lz");
const u8 gBattleAnimSpriteGfx_GreenSpike[] = INCBIN_U8("graphics/battle_anims/sprites/green_spike.4bpp.lz");
const u8 gBattleAnimSpritePal_GreenSpike[] = INCBIN_U8("graphics/battle_anims/sprites/green_spike.gbapal.lz");
const u8 gBattleAnimSpritePal_WhiteCircleOfLight[] = INCBIN_U8("graphics/battle_anims/sprites/white_circle_of_light.gbapal.lz");
const u8 gBattleAnimSpritePal_GlowyBlueOrb[] = INCBIN_U8("graphics/battle_anims/sprites/glowy_blue_orb.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Recycle[] = INCBIN_U8("graphics/battle_anims/sprites/recycle.4bpp.lz");
const u8 gBattleAnimSpritePal_Recycle[] = INCBIN_U8("graphics/battle_anims/sprites/recycle.gbapal.lz");
const u8 gBattleAnimSpriteGfx_RedParticles[] = INCBIN_U8("graphics/battle_anims/sprites/red_particles.4bpp.lz");
const u8 gBattleAnimSpritePal_RedParticles[] = INCBIN_U8("graphics/battle_anims/sprites/red_particles.gbapal.lz");
const u8 gBattleAnimSpriteGfx_DirtMound[] = INCBIN_U8("graphics/battle_anims/sprites/dirt_mound.4bpp.lz");
const u8 gBattleAnimSpritePal_DirtMound[] = INCBIN_U8("graphics/battle_anims/sprites/dirt_mound.gbapal.lz");
const u8 gBattleAnimBgImage_Fissure[] = INCBIN_U8("graphics/battle_anims/backgrounds/fissure.4bpp.lz");
const u8 gBattleAnimBgPalette_Fissure[] = INCBIN_U8("graphics/battle_anims/backgrounds/fissure.gbapal.lz");
const u8 gBattleAnimBgTilemap_Fissure[] = INCBIN_U8("graphics/battle_anims/backgrounds/fissure.bin.lz");
const u8 gBattleAnimSpriteGfx_Bird[] = INCBIN_U8("graphics/battle_anims/sprites/bird.4bpp.lz");
const u8 gBattleAnimSpritePal_Bird[] = INCBIN_U8("graphics/battle_anims/sprites/bird.gbapal.lz");
const u8 gBattleAnimSpriteGfx_CrossImpact[] = INCBIN_U8("graphics/battle_anims/sprites/cross_impact.4bpp.lz");
const u8 gBattleAnimSpritePal_CrossImpact[] = INCBIN_U8("graphics/battle_anims/sprites/cross_impact.gbapal.lz");
const u8 gBattleAnimBgImage_Surf[] = INCBIN_U8("graphics/battle_anims/backgrounds/water.4bpp.lz");
const u8 gBattleAnimBgPalette_Surf[] = INCBIN_U8("graphics/battle_anims/backgrounds/water.gbapal.lz");
const u8 gBattleAnimBgTilemap_SurfOpponent[] = INCBIN_U8("graphics/battle_anims/backgrounds/water_opponent.bin.lz");
const u8 gBattleAnimBgTilemap_SurfPlayer[] = INCBIN_U8("graphics/battle_anims/backgrounds/water_player.bin.lz");
const u8 gBattleAnimBgTilemap_SurfContest[] = INCBIN_U8("graphics/battle_anims/backgrounds/water_contest.bin.lz");
const u8 gBattleAnimSpritePal_Slash2[] = INCBIN_U8("graphics/battle_anims/sprites/slash_2.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WhiteShadow[] = INCBIN_U8("graphics/battle_anims/sprites/white_shadow.4bpp.lz");
const u8 gBattleAnimSpritePal_WhiteShadow[] = INCBIN_U8("graphics/battle_anims/sprites/white_shadow.gbapal.lz");
const u8 gPartyMenuMisc_Gfx[] = INCBIN_U8("graphics/party_menu/misc.4bpp.lz");
const u8 gPartyMenuMisc_Pal[] = INCBIN_U8("graphics/party_menu/misc.gbapal.lz");
const u8 gPartyMenuMisc_Tilemap[] = INCBIN_U8("graphics/party_menu/misc.bin.lz");
const u8 gPartyMenuHpBar_Gfx[] = INCBIN_U8("graphics/party_menu/hpbar.4bpp.lz");
const u8 gPartyMenuOrderText_Gfx[] = INCBIN_U8("graphics/party_menu/order_text.4bpp.lz");
const u8 gMoveTypes_Gfx[] = INCBIN_U8("graphics/types/move_types.4bpp.lz");
const u8 gMoveTypes_Pal[] = INCBIN_U8("graphics/types/move_types.gbapal.lz");
const u8 gSummaryMoveSelect_Gfx[] = INCBIN_U8("graphics/summary_screen/move_select.4bpp.lz");// the arrows appear to be unused.

const u8 gSummaryMoveSelect_Pal[] = INCBIN_U8("graphics/summary_screen/move_select.gbapal.lz");
const u8 gStatusGfx_Icons[] = INCBIN_U8("graphics/interface/status_icons.4bpp.lz");
const u8 gStatusPal_Icons[] = INCBIN_U8("graphics/interface/status_icons.gbapal.lz");
const u8 gSummaryScreen_Gfx[] = INCBIN_U8("graphics/summary_screen/tiles.4bpp.lz");
const u8 gSummaryScreen_Pal[] = INCBIN_U8("graphics/summary_screen/tiles.gbapal.lz");
const u8 gSummaryScreen_StatusTilemap[] = INCBIN_U8("graphics/summary_screen/status_tilemap.bin");
const u8 gSummaryPage_Skills_Tilemap[] = INCBIN_U8("graphics/summary_screen/page_skills.bin.lz");
const u8 gSummaryPage_BattleMoves_Tilemap[] = INCBIN_U8("graphics/summary_screen/page_battle_moves.bin");
const u8 gSummaryPage_ContestMoves_Tilemap[] = INCBIN_U8("graphics/summary_screen/page_contest_moves.bin");
const u8 gSummaryPage_Info_Tilemap[] = INCBIN_U8("graphics/summary_screen/page_info.bin.lz");
const u8 gBagMaleTiles[] = INCBIN_U8("graphics/bag/bag_male.4bpp.lz");
const u8 gBagFemaleTiles[] = INCBIN_U8("graphics/bag/bag_female.4bpp.lz");
const u8 gBagPalette[] = INCBIN_U8("graphics/bag/bag.gbapal.lz");
const u8 gBagScreen_Gfx[] = INCBIN_U8("graphics/bag/menu.4bpp.lz");
const u8 gBagScreenMale_Pal[] = INCBIN_U8("graphics/bag/menu_male.gbapal.lz");
const u8 gBagScreenFemale_Pal[] = INCBIN_U8("graphics/bag/menu_female.gbapal.lz");
const u8 gBagScreen_GfxTileMap[] = INCBIN_U8("graphics/bag/menu.bin");
const u8 gShopMenu_Gfx[] = INCBIN_U8("graphics/shop/menu.4bpp.lz");
const u8 gShopMenuMoney_Pal[] = INCBIN_U8("graphics/shop/money.gbapal.lz");
const u8 gShopMenu_Tilemap[] = INCBIN_U8("graphics/shop/menu.bin.lz");
const u8 gShopMenuMoney_Gfx[] = INCBIN_U8("graphics/shop/money.4bpp.lz");
const u8 gMenuPokeblock_Gfx[] = INCBIN_U8("graphics/pokeblock/menu.4bpp.lz");
const u8 gMenuPokeblock_Pal[] = INCBIN_U8("graphics/pokeblock/menu.gbapal.lz");
const u8 gMenuPokeblockDevice_Gfx[] = INCBIN_U8("graphics/pokeblock/device.4bpp.lz");
const u8 gMenuPokeblockDevice_Pal[] = INCBIN_U8("graphics/pokeblock/device.gbapal.lz");
const u8 gMenuPokeblock_Tilemap[] = INCBIN_U8("graphics/pokeblock/menu.bin.lz");
const u8 gPokeblock_Gfx[] = INCBIN_U8("graphics/pokeblock/pokeblock.4bpp.lz");
const u8 gPokeblockRed_Pal[] = INCBIN_U8("graphics/pokeblock/red.gbapal.lz");
const u8 gPokeblockBlue_Pal[] = INCBIN_U8("graphics/pokeblock/blue.gbapal.lz");
const u8 gPokeblockPink_Pal[] = INCBIN_U8("graphics/pokeblock/pink.gbapal.lz");
const u8 gPokeblockGreen_Pal[] = INCBIN_U8("graphics/pokeblock/green.gbapal.lz");
const u8 gPokeblockYellow_Pal[] = INCBIN_U8("graphics/pokeblock/yellow.gbapal.lz");
const u8 gPokeblockPurple_Pal[] = INCBIN_U8("graphics/pokeblock/purple.gbapal.lz");
const u8 gPokeblockIndigo_Pal[] = INCBIN_U8("graphics/pokeblock/indigo.gbapal.lz");
const u8 gPokeblockBrown_Pal[] = INCBIN_U8("graphics/pokeblock/brown.gbapal.lz");
const u8 gPokeblockLiteBlue_Pal[] = INCBIN_U8("graphics/pokeblock/liteblue.gbapal.lz");
const u8 gPokeblockOlive_Pal[] = INCBIN_U8("graphics/pokeblock/olive.gbapal.lz");
const u8 gPokeblockGray_Pal[] = INCBIN_U8("graphics/pokeblock/gray.gbapal.lz");
const u8 gPokeblockBlack_Pal[] = INCBIN_U8("graphics/pokeblock/black.gbapal.lz");
const u8 gPokeblockWhite_Pal[] = INCBIN_U8("graphics/pokeblock/white.gbapal.lz");
const u8 gPokeblockGold_Pal[] = INCBIN_U8("graphics/pokeblock/gold.gbapal.lz");
const u8 gPokeblockFeedBg_Tilemap[] = INCBIN_U8("graphics/pokeblock/feeding_bg.bin.lz");
const u8 gBerryCheck_Gfx[] = INCBIN_U8("graphics/bag/check_berry.4bpp.lz");
const u8 gBerryCheck_Pal[] = INCBIN_U8("graphics/bag/check_berry.gbapal.lz");
const u8 gBerryTag_Gfx[] = INCBIN_U8("graphics/bag/berry_tag.bin.lz");
const u8 gBerryTag_Tilemap[] = INCBIN_U8("graphics/bag/berry_tag_title.bin.lz");
const u8 gBerryCheckCircle_Gfx[] = INCBIN_U8("graphics/bag/check_berry_circle.4bpp.lz");

#include "data/graphics/berries.h"

const u8 gBattleAnimSpritePal_Shock3[] = INCBIN_U8("graphics/battle_anims/sprites/shock_3.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Shock3[] = INCBIN_U8("graphics/battle_anims/sprites/shock_3.4bpp.lz");
const u8 gBattleAnimSpritePal_WhiteFeather[] = INCBIN_U8("graphics/battle_anims/sprites/white_feather.gbapal.lz");
const u8 gBattleAnimSpriteGfx_WhiteFeather[] = INCBIN_U8("graphics/battle_anims/sprites/white_feather.4bpp.lz");
const u8 gBattleAnimSpritePal_Sparkle6[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_6.gbapal.lz");
const u8 gBattleAnimSpriteGfx_Sparkle6[] = INCBIN_U8("graphics/battle_anims/sprites/sparkle_6.4bpp.lz");

const u16 gCableCarBg_Pal[] = INCBIN_U16("graphics/cable_car/bg.gbapal");
const u16 gCableCar_Pal[] = INCBIN_U16("graphics/cable_car/cable_car.gbapal");
const u8 gCableCarBg_Gfx[] = INCBIN_U8("graphics/cable_car/bg.4bpp.lz");
const u8 gCableCar_Gfx[] = INCBIN_U8("graphics/cable_car/cable_car.4bpp.lz");
const u8 gCableCarDoor_Gfx[] = INCBIN_U8("graphics/cable_car/door.4bpp.lz");
const u8 gCableCarCable_Gfx[] = INCBIN_U8("graphics/cable_car/cable.4bpp.lz");

const u8 gRouletteMenu_Gfx[] = INCBIN_U8("graphics/roulette/window.4bpp.lz");

const u8 gRouletteWheel_Gfx[] = INCBIN_U8("graphics/roulette/wheel.8bpp.lz");
const u8 gRouletteCenter_Gfx[] = INCBIN_U8("graphics/roulette/center.4bpp.lz");
const u8 gRouletteHeaders_Gfx[] = INCBIN_U8("graphics/roulette/headers.4bpp.lz");
const u8 gRouletteCredit_Gfx[] = INCBIN_U8("graphics/roulette/credit.4bpp.lz");
const u8 gRouletteNumbers_Gfx[] = INCBIN_U8("graphics/roulette/numbers.4bpp.lz");
const u8 gRouletteMultiplier_Gfx[] = INCBIN_U8("graphics/roulette/multiplier.4bpp.lz");

const u16 gMailPalette_Orange[] = INCBIN_U16("graphics/mail/orange/palette.gbapal");
const u16 gMailPalette_Harbor[] = INCBIN_U16("graphics/mail/harbor/palette.gbapal");
const u16 gMailPalette_Glitter[] = INCBIN_U16("graphics/mail/glitter/palette.gbapal");
const u16 gMailPalette_Mech[] = INCBIN_U16("graphics/mail/mech/palette.gbapal");
const u16 gMailPalette_Wood[] = INCBIN_U16("graphics/mail/wood/palette.gbapal");
const u16 gMailPalette_Wave[] = INCBIN_U16("graphics/mail/wave/palette.gbapal");
const u16 gMailPalette_Bead[] = INCBIN_U16("graphics/mail/bead/palette.gbapal");
const u16 gMailPalette_Shadow[] = INCBIN_U16("graphics/mail/shadow/palette.gbapal");
const u16 gMailPalette_Tropic[] = INCBIN_U16("graphics/mail/tropic/palette.gbapal");
const u16 gMailPalette_Dream[] = INCBIN_U16("graphics/mail/dream/palette.gbapal");
const u16 gMailPalette_Fab[] = INCBIN_U16("graphics/mail/fab/palette.gbapal");
const u16 gMailPalette_Retro[] = INCBIN_U16("graphics/mail/retro/palette.gbapal");

const u8 gMailTiles_Orange[] = INCBIN_U8("graphics/mail/orange/tiles.4bpp.lz");
const u8 gMailTiles_Harbor[] = INCBIN_U8("graphics/mail/harbor/tiles.4bpp.lz");
const u8 gMailTiles_Glitter[] = INCBIN_U8("graphics/mail/glitter/tiles.4bpp.lz");
const u8 gMailTiles_Mech[] = INCBIN_U8("graphics/mail/mech/tiles.4bpp.lz");
const u8 gMailTiles_Wood[] = INCBIN_U8("graphics/mail/wood/tiles.4bpp.lz");
const u8 gMailTiles_Wave[] = INCBIN_U8("graphics/mail/wave/tiles.4bpp.lz");
const u8 gMailTiles_Bead[] = INCBIN_U8("graphics/mail/bead/tiles.4bpp.lz");
const u8 gMailTiles_Shadow[] = INCBIN_U8("graphics/mail/shadow/tiles.4bpp.lz");
const u8 gMailTiles_Tropic[] = INCBIN_U8("graphics/mail/tropic/tiles.4bpp.lz");
const u8 gMailTiles_Dream[] = INCBIN_U8("graphics/mail/dream/tiles.4bpp.lz");
const u8 gMailTiles_Fab[] = INCBIN_U8("graphics/mail/fab/tiles.4bpp.lz");
const u8 gMailTiles_Retro[] = INCBIN_U8("graphics/mail/retro/tiles.4bpp.lz");

const u8 gMailTilemap_Orange[] = INCBIN_U8("graphics/mail/orange/map.bin.lz");
const u8 gMailTilemap_Harbor[] = INCBIN_U8("graphics/mail/harbor/map.bin.lz");
const u8 gMailTilemap_Glitter[] = INCBIN_U8("graphics/mail/glitter/map.bin.lz");
const u8 gMailTilemap_Mech[] = INCBIN_U8("graphics/mail/mech/map.bin.lz");
const u8 gMailTilemap_Wood[] = INCBIN_U8("graphics/mail/wood/map.bin.lz");
const u8 gMailTilemap_Wave[] = INCBIN_U8("graphics/mail/wave/map.bin.lz");
const u8 gMailTilemap_Bead[] = INCBIN_U8("graphics/mail/bead/map.bin.lz");
const u8 gMailTilemap_Shadow[] = INCBIN_U8("graphics/mail/shadow/map.bin.lz");
const u8 gMailTilemap_Tropic[] = INCBIN_U8("graphics/mail/tropic/map.bin.lz");
const u8 gMailTilemap_Dream[] = INCBIN_U8("graphics/mail/dream/map.bin.lz");
const u8 gMailTilemap_Fab[] = INCBIN_U8("graphics/mail/fab/map.bin.lz");
const u8 gMailTilemap_Retro[] = INCBIN_U8("graphics/mail/retro/map.bin.lz");

const u8 gNamingScreenBackButton_Gfx[] = INCBIN_U8("graphics/naming_screen/back_button.4bpp");
const u8 gNamingScreenInputArrow_Gfx[] = INCBIN_U8("graphics/naming_screen/input_arrow.4bpp");
const u8 gNamingScreenOKButton_Gfx[] = INCBIN_U8("graphics/naming_screen/ok_button.4bpp");
const u8 gNamingScreenUnderscore_Gfx[] = INCBIN_U8("graphics/naming_screen/underscore.4bpp");
const u8 gNamingScreenPageSwapFrame_Gfx[] = INCBIN_U8("graphics/naming_screen/page_swap_frame.4bpp");
const u8 gNamingScreenPageSwapButton_Gfx[] = INCBIN_U8("graphics/naming_screen/page_swap_button.4bpp");

asm(".space 128");

const u8 gNamingScreenPageSwapOthers_Gfx[] = INCBIN_U8("graphics/naming_screen/page_swap_others.4bpp");
const u8 gNamingScreenCursor_Gfx[] = INCBIN_U8("graphics/naming_screen/cursor.4bpp");
const u8 gNamingScreenCursorSquished_Gfx[] = INCBIN_U8("graphics/naming_screen/cursor_squished.4bpp");
const u8 gNamingScreenCursorFilled_Gfx[] = INCBIN_U8("graphics/naming_screen/cursor_filled.4bpp");
const u8 gNamingScreenPageSwapLower_Gfx[] = INCBIN_U8("graphics/naming_screen/page_swap_lower.4bpp");
const u8 gNamingScreenPageSwapUpper_Gfx[] = INCBIN_U8("graphics/naming_screen/page_swap_upper.4bpp");

asm(".space 352");

const u8 gNamingScreenMenu_Gfx[] = INCBIN_U8("graphics/naming_screen/menu.4bpp");

const u16 gNamingScreenMenu_Pal[][16] =
{
    INCBIN_U16("graphics/naming_screen/menu.gbapal"),
    INCBIN_U16("graphics/naming_screen/page_swap_upper.gbapal"),
    INCBIN_U16("graphics/naming_screen/page_swap_lower.gbapal"),
    INCBIN_U16("graphics/naming_screen/page_swap_others.gbapal"),
    INCBIN_U16("graphics/naming_screen/buttons.gbapal"),
    INCBIN_U16("graphics/naming_screen/cursor.gbapal"),
};

const u8 gNamingScreenBackground_Tilemap[] = INCBIN_U8("graphics/naming_screen/background.bin");
const u8 gPokedexMenu_Gfx[] = INCBIN_U8("graphics/pokedex/menu.4bpp.lz");
const u8 gPokedexInterface_Gfx[] = INCBIN_U8("graphics/pokedex/interface.4bpp.lz");
const u16 gPokedexBgHoenn_Pal[] = INCBIN_U16("graphics/pokedex/bg_hoenn.gbapal");
const u16 gPokedexCaughtScreen_Pal[] = INCBIN_U16("graphics/pokedex/caught_screen.gbapal");

const u8 gPokedexAreaScreenAreaUnknown_Gfx[] = INCBIN_U8("graphics/pokedex/area_unknown.4bpp.lz");
const u16 gPokedexAreaScreenAreaUnknown_Pal[] = INCBIN_U16("graphics/pokedex/area_unknown.gbapal");

const u8 gPokedexSearchMenu_Gfx[] = INCBIN_U8("graphics/pokedex/search_menu.4bpp.lz");
const u16 gPokedexSearchMenu_Pal[] = INCBIN_U16("graphics/pokedex/search_menu.gbapal");

const u8 gPokenavMainMenuHeader_Gfx[] = INCBIN_U8("graphics/pokenav/left_headers/main_menu.4bpp.lz");
const u8 gPokenavMenuOptions_Gfx[] = INCBIN_U8("graphics/pokenav/options/main_menu_labels.4bpp.lz");
const u16 gPokenavMenuOptions1_Pal[] = INCBIN_U16("graphics/pokenav/pokenav1.gbapal");
const u16 gPokenavMenuOptions2_Pal[] = INCBIN_U16("graphics/pokenav/pokenav2.gbapal");
const u16 gPokenavMenuOptions3_Pal[] = INCBIN_U16("graphics/pokenav/pokenav3.gbapal");
const u8 gPokenavHoennMapHeader_Gfx[] = INCBIN_U8("graphics/pokenav/left_headers/hoenn_map.4bpp.lz");
const u8 gPokenavHoennMapMisc_Gfx[] = INCBIN_U8("graphics/pokenav/map_misc2.4bpp.lz");
const u8 gPokenavRegionMapZoomTiles_Gfx[] = INCBIN_U8("graphics/pokenav/region_map/zoom_tiles.4bpp.lz");
const u16 gPokenavHoennMap1_Pal[] = INCBIN_U16("graphics/pokenav/map_menu.gbapal");
const u8 gPokenavRegionMapZoomTiles_Pal[] = INCBIN_U8("graphics/pokenav/region_map/zoom_tiles.gbapal");
const u8 gPokenavConditionMenu_Gfx[] = INCBIN_U8("graphics/pokenav/options/condition_menu.4bpp.lz");
const u16 gPokenavConditionMenuOptions_Pal[] = INCBIN_U16("graphics/pokenav/options/condition_menu.gbapal");
const u8 gPokenavConditionMenuHeader_Gfx[] = INCBIN_U8("graphics/pokenav/left_headers/condition.4bpp.lz");
const u8 gPokenavConditionView_Gfx[] = INCBIN_U8("graphics/pokenav/condition/view.4bpp.lz");
const u8 gPokenavConditionView_Pal[] = INCBIN_U8("graphics/pokenav/condition/view.gbapal");
const u8 gPokenavConditionHeaderLabels_Gfx[] = INCBIN_U8("graphics/pokenav/left_headers/condition_labels_1.4bpp.lz");
const u8 gPokenavConditionCancel_Gfx[] = INCBIN_U8("graphics/pokenav/condition/cancel.4bpp");
const u8 gPokenavConditionPokeball_Pal[] = INCBIN_U8("graphics/pokenav/condition/pokeball.gbapal");//
const u16 gPokenavConditionCancel_Pal[] = INCBIN_U16("graphics/pokenav/condition/cancel.gbapal");
const u8 gPokenavConditionHeaderLabels2_Gfx[] = INCBIN_U8("graphics/pokenav/left_headers/condition_labels_2.4bpp.lz");
const u8 gMonMarkingsMenu_Gfx[] = INCBIN_U8("graphics/interface/mon_markings_menu.4bpp");
const u16 gPokenavConditionHeader_Pal[] = INCBIN_U16("graphics/pokenav/left_headers/condition.gbapal");
const u8 gPokenavConditionSearch_Gfx[] = INCBIN_U8("graphics/pokenav/options/condition_search_labels.4bpp.lz");
const u16 gPokenavConditionSearchPrimary_Pal[] = INCBIN_U16("graphics/pokenav/options/condition_search.gbapal");
// another palette thats impossible to leave as native pal file due to 0xFFFF instead of 0x7FFF.
const u16 gPokenavConditionSearchSecondary_Pal[] = INCBIN_U16("graphics/pokenav/options/condition_search_secondary.pal.bin");
const u8 gPokenavTrainersEyesHeader_Gfx[] = INCBIN_U8("graphics/pokenav/left_headers/trainers_eyes.4bpp.lz");
const u8 gPokenavRibbonsHeader_Gfx[] = INCBIN_U8("graphics/pokenav/left_headers/ribbons.4bpp.lz");

const u8 gHoennTrainerCard_Gfx[] = INCBIN_U8("graphics/trainer_card/tiles.4bpp");
const u16 gHoennTrainerCardGreen_Pal[] = INCBIN_U16("graphics/trainer_card/green.gbapal");
const u16 gHoennTrainerCardBronze_Pal[] = INCBIN_U16("graphics/trainer_card/bronze.gbapal");
const u16 gHoennTrainerCardCopper_Pal[] = INCBIN_U16("graphics/trainer_card/copper.gbapal");
const u16 gHoennTrainerCardSilver_Pal[] = INCBIN_U16("graphics/trainer_card/silver.gbapal");
const u16 gHoennTrainerCardGold_Pal[] = INCBIN_U16("graphics/trainer_card/gold.gbapal");

const u8 gHoennTrainerCardFront_Tilemap[] = INCBIN_U8("graphics/trainer_card/front.bin");
const u8 gHoennTrainerCardBack_Tilemap[] = INCBIN_U8("graphics/trainer_card/back.bin");
const u8 gHoennTrainerCardFrontLink_Tilemap[] = INCBIN_U8("graphics/trainer_card/front_link.bin");
const u8 gHoennTrainerCardBg_Tilemap[] = INCBIN_U8("graphics/trainer_card/bg.bin");

const u8 gPSSMenuHeader_Gfx[] = INCBIN_U8("graphics/pokemon_storage/header.4bpp.lz");
const u16 gPSSMenu1_Pal[] = INCBIN_U16("graphics/pokemon_storage/menu1.gbapal");
const u16 gPSSMenu2_Pal[] = INCBIN_U16("graphics/pokemon_storage/menu2.bin");
const u8 gPSSMenuHeader_Tilemap[] = INCBIN_U8("graphics/pokemon_storage/header.bin.lz");
const u8 gPSSMenuMisc_Gfx[] = INCBIN_U8("graphics/pokemon_storage/misc1.4bpp.lz");
const u16 gPSSMenu3_Pal[] = INCBIN_U16("graphics/pokemon_storage/menu3.gbapal");
const u16 gPSSMenu4_Pal[] = INCBIN_U16("graphics/pokemon_storage/menu4.gbapal");
const u8 gPSSMenuMisc_Tilemap[] = INCBIN_U8("graphics/pokemon_storage/misc1.bin.lz");

const u8 gHallOfFameGfx[] = INCBIN_U8("graphics/misc/hall_of_fame.4bpp.lz");
const u16 gHallOfFamePal[] = INCBIN_U16("graphics/misc/hall_of_fame.gbapal");

const u8 gMenuWordGroupIndicator_Gfx[] = INCBIN_U8("graphics/interface/wordgroup_indicator.4bpp.lz");
const u8 gMenuWordGroupFrame_Gfx[] = INCBIN_U8("graphics/interface/wordgroup_frame.4bpp.lz");

const u16 gMenuWordGroupIndicator_Pal[] = INCBIN_U16("graphics/interface/wordgroup_indicator.gbapal");
const u16 gMenuWordGroupFrame1_Pal[] = INCBIN_U16("graphics/interface/wordgroup_pink.gbapal");
const u16 gMenuWordGroupFrame2_Pal[] = INCBIN_U16("graphics/interface/wordgroup_blue.gbapal");

const u8 gWallClock_Gfx[] = INCBIN_U8("graphics/wallclock/clock.4bpp.lz");
const u16 gWallClockMale_Pal[] = INCBIN_U16("graphics/wallclock/male.gbapal"); // boy
const u16 gWallClockFemale_Pal[] = INCBIN_U16("graphics/wallclock/female.gbapal"); // girl

const u8 gSlotMachineMenu_Gfx[] = INCBIN_U8("graphics/slot_machine/menu.4bpp.lz");
const u8 gBattleInterfaceStatusIcons_DynPal[] = INCBIN_U8("graphics/battle_interface/status.gbapal");
const u8 gUnusedArrowGfx[] = INCBIN_U8("graphics/unused/arrow.4bpp"); // unused
const u8 gBerryBlenderScoreSymbols_Gfx[] = INCBIN_U8("graphics/berry_blender/score_symbols.4bpp");
const u8 gBerryBlenderParticles_Gfx[] = INCBIN_U8("graphics/berry_blender/particles.4bpp");

asm(".space 0x120");

const u8 gBerryBlenderCountdownNumbers_Gfx[] = INCBIN_U8("graphics/berry_blender/countdown_numbers.4bpp");
const u8 gBerryBlenderStart_Gfx[] = INCBIN_U8("graphics/berry_blender/start.4bpp");
const u8 gUnusedBerryBlenderGfx[] = INCBIN_U8("graphics/berry_blender/unused.bin");

asm(".space 0x1E0");

const u8 gBerryBlenderPlayerArrow_Gfx[] = INCBIN_U8("graphics/berry_blender/arrow.4bpp");
asm(".space 0x2C0");

const u8 gBerryBlenderMiscPalette[] = INCBIN_U8("graphics/berry_blender/misc.gbapal");
const u8 gBerryBlenderArrowPalette[] = INCBIN_U8("graphics/berry_blender/arrow.gbapal");

const u16 gTilesetTiles_General[] = INCBIN_U16("data/tilesets/primary/general/tiles.4bpp.lz");
const u16 gTilesetPalettes_General[][16] = 
{
    INCBIN_U16("data/tilesets/primary/general/palettes/00.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/01.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/02.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/03.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/04.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/05.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/06.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/07.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/08.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/09.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/10.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/11.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/12.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/13.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/14.gbapal"),
    INCBIN_U16("data/tilesets/primary/general/palettes/15.gbapal"),
};

const u8 gSummaryScreen_MoveEffect_Cancel_TilemapTop[] = INCBIN_U8("graphics/summary_screen/effect_cancel_top.bin");
const u8 gSummaryScreen_MoveEffect_Cancel_TilemapMiddle[] = INCBIN_U8("graphics/summary_screen/effect_cancel_middle.bin");
const u8 gSummaryScreen_MoveEffect_Cancel_TilemapBottom[] = INCBIN_U8("graphics/summary_screen/effect_cancel_bottom.bin");
const u8 gEasyChatWordGroup_Tilemap[] = INCBIN_U8("graphics/interface/wordgroup_layout.bin");
const u8 gEasyChatFrame_Tilemap[] = INCBIN_U8("graphics/easy_chat/frame.bin");
const u8 gEasyChatInputSelect_Tilemap[] = INCBIN_U8("graphics/easy_chat/input_select.bin.lz");
const u8 gWallClockStart_Tilemap[] = INCBIN_U8("graphics/wallclock/clock_start.bin.lz");
const u8 gWallClockView_Tilemap[] = INCBIN_U8("graphics/wallclock/clock_view.bin.lz");

#ifdef SAPPHIRE
const u8 gSlotMachineMenu_Pal[] = INCBIN_U8("graphics/slot_machine/menu_sapphire.gbapal");
#else
const u8 gSlotMachineMenu_Pal[] = INCBIN_U8("graphics/slot_machine/menu_ruby.gbapal");
#endif // SAPPHIRE

const u8 gSlotMachineMenu_Tilemap[] = INCBIN_U8("graphics/slot_machine/menu.bin");
const u8 gSlotMachineInfoBox_Tilemap[] = INCBIN_U8("graphics/slot_machine/info_box.bin");
const u8 gContestResultsTitleWords_Tilemap[] = INCBIN_U8("graphics/contest/results_screen/title_words.bin");
const u16 gMonMarkingsMenu_Pal[] = INCBIN_U16("graphics/interface/mon_markings_menu.gbapal");

const u16 gMonIconPalettes[][16] = 
{
    INCBIN_U16("graphics/pokemon/icon_palettes/icon_palette_0.gbapal"),
    INCBIN_U16("graphics/pokemon/icon_palettes/icon_palette_1.gbapal"),
    INCBIN_U16("graphics/pokemon/icon_palettes/icon_palette_2.gbapal"),
};

const u8 gPokedexList_Tilemap[] = INCBIN_U8("graphics/pokedex/list.bin.lz");
const u8 gPokedexStartMenuMain_Tilemap[] = INCBIN_U8("graphics/pokedex/start_menu_main.bin.lz");
const u8 gPokedexStartMenuSearchResults_Tilemap[] = INCBIN_U8("graphics/pokedex/start_menu_search_results.bin.lz");
const u8 gPokedexScreenSelectBarMain_Tilemap[] = INCBIN_U8("graphics/pokedex/screen_select_bar_main.bin.lz");
const u8 gPokedexScreenSelectBarSubmenu_Tilemap[] = INCBIN_U8("graphics/pokedex/screen_select_bar_submenu.bin.lz");
const u8 gPokedexInfoScreen_Tilemap[] = INCBIN_U8("graphics/pokedex/info_screen.bin.lz");
const u8 gPokedexSearchMenuNational_Tilemap[] = INCBIN_U8("graphics/pokedex/search_menu_national.bin.lz");

const u8 gBagScreenLabels_Tilemap[] = INCBIN_U8("graphics/bag/menu_labels.bin");

const u16 gSlotMachineReelSymbols_Pal[] = INCBIN_U16("graphics/slot_machine/reel_symbols.gbapal");
const u16 gSlotMachineReelTimePikachu_Pal[] = INCBIN_U16("graphics/slot_machine/reel_time_pikachu.gbapal");

#ifdef SAPPHIRE
const u16 gSlotMachineReelTimeMisc_Pal[] = INCBIN_U16("graphics/slot_machine/reel_time_misc_sapphire.gbapal");
#else
const u16 gSlotMachineReelTimeMisc_Pal[] = INCBIN_U16("graphics/slot_machine/reel_time_misc_ruby.gbapal");
#endif // SAPPHIRE

#ifdef SAPPHIRE
const u16 gSlotMachineReelTimeMachine_Pal[] = INCBIN_U16("graphics/slot_machine/reel_time_machine_sapphire.gbapal");
#else
const u16 gSlotMachineReelTimeMachine_Pal[] = INCBIN_U16("graphics/slot_machine/reel_time_machine_ruby.gbapal");
#endif // SAPPHIRE

const u16 gSlotMachineMisc_Pal[] = INCBIN_U16("graphics/slot_machine/misc.gbapal");
const u16 gSlotMachineReelTimeExplosion_Pal[] = INCBIN_U16("graphics/slot_machine/reel_time_explosion.gbapal");
const u16 gSlotMachineDigitalDisplay_Pal[] = INCBIN_U16("graphics/slot_machine/digital_display.gbapal");

const u8 gSlotMachineReelSymbol1Tiles[] = INCBIN_U8("graphics/slot_machine/reel_symbols/1.4bpp");
const u8 gSlotMachineReelSymbol2Tiles[] = INCBIN_U8("graphics/slot_machine/reel_symbols/2.4bpp");
const u8 gSlotMachineReelSymbol3Tiles[] = INCBIN_U8("graphics/slot_machine/reel_symbols/3.4bpp");
const u8 gSlotMachineReelSymbol4Tiles[] = INCBIN_U8("graphics/slot_machine/reel_symbols/4.4bpp");
const u8 gSlotMachineReelSymbol5Tiles[] = INCBIN_U8("graphics/slot_machine/reel_symbols/5.4bpp");
const u8 gSlotMachineReelSymbol6Tiles[] = INCBIN_U8("graphics/slot_machine/reel_symbols/6.4bpp");
const u8 gSlotMachineReelSymbol7Tiles[] = INCBIN_U8("graphics/slot_machine/reel_symbols/7.4bpp");

const u8 gSlotMachineNumber0Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/0.4bpp");
const u8 gSlotMachineNumber1Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/1.4bpp");
const u8 gSlotMachineNumber2Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/2.4bpp");
const u8 gSlotMachineNumber3Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/3.4bpp");
const u8 gSlotMachineNumber4Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/4.4bpp");
const u8 gSlotMachineNumber5Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/5.4bpp");
const u8 gSlotMachineNumber6Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/6.4bpp");
const u8 gSlotMachineNumber7Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/7.4bpp");
const u8 gSlotMachineNumber8Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/8.4bpp");
const u8 gSlotMachineNumber9Tiles[] = INCBIN_U8("graphics/slot_machine/numbers/9.4bpp");

const u8 gSlotMachinePikaPowerBolt[] = INCBIN_U8("graphics/slot_machine/bolt.4bpp");
const u8 gSlotMachineReelTimeDuck[] = INCBIN_U8("graphics/slot_machine/duck.4bpp");
const u8 gSlotMachineReelTimeSmoke[] = INCBIN_U8("graphics/slot_machine/smoke.4bpp");
const u8 gSlotMachineReelTimeNumber0[] = INCBIN_U8("graphics/slot_machine/reel_time_numbers/0.4bpp");
const u8 gSlotMachineReelTimeNumber1[] = INCBIN_U8("graphics/slot_machine/reel_time_numbers/1.4bpp");
const u8 gSlotMachineReelTimeNumber2[] = INCBIN_U8("graphics/slot_machine/reel_time_numbers/2.4bpp");
const u8 gSlotMachineReelTimeNumber3[] = INCBIN_U8("graphics/slot_machine/reel_time_numbers/3.4bpp");
const u8 gSlotMachineReelTimeNumber4[] = INCBIN_U8("graphics/slot_machine/reel_time_numbers/4.4bpp");
const u8 gSlotMachineReelTimeNumber5[] = INCBIN_U8("graphics/slot_machine/reel_time_numbers/5.4bpp");
const u8 gSlotMachineReelTimeBolt0[] = INCBIN_U8("graphics/slot_machine/large_bolt/0.4bpp");
const u8 gSlotMachineReelTimeBolt1[] = INCBIN_U8("graphics/slot_machine/large_bolt/1.4bpp");
const u8 gSlotMachineReelTimeExplosion0[] = INCBIN_U8("graphics/slot_machine/reel_time_explosion/0.4bpp");
const u8 gSlotMachineReelTimeExplosion1[] = INCBIN_U8("graphics/slot_machine/reel_time_explosion/1.4bpp");
const u8 gSlotMachineReelTimeShadow[] = INCBIN_U8("graphics/slot_machine/shadow.4bpp");
const u8 gSlotMachineReelTimePikaAura[] = INCBIN_U8("graphics/slot_machine/pika_aura.4bpp");

const u8 gSlotMachineReelBackground_Tilemap[] = INCBIN_U8("graphics/slot_machine/reel_background.gbapal");
const u8 gSlotMachineReelTimeNumberGap_Gfx[] = INCBIN_U8("graphics/slot_machine/reel_time_number_gap.4bpp");
const u8 gSlotMachineDigitalDisplay_Gfx[] = INCBIN_U8("graphics/slot_machine/digital_display.4bpp.lz");

const u8 gPokenavHoennMap_Tilemap[] = INCBIN_U8("graphics/pokenav/map_menu.bin.lz");
const u8 gPokenavBottomToolbarTilemap[] = INCBIN_U8("graphics/pokenav/bottom_toolbar.bin");
const u8 gPartyMenuDescriptorStatusTilemap[] = INCBIN_U8("graphics/party_menu/descriptor_status_tilemap.bin");
const u8 gEasyChatBg2_Gfx[] = INCBIN_U8("graphics/easy_chat/bg2_gfx.bin");
const u8 gEasyChatBg2_Pal[] = INCBIN_U8("graphics/easy_chat/bg2.gbapal");
const u8 gEasyChatBg2_Tilemap[] = INCBIN_U8("graphics/easy_chat/bg2.bin.lz");
const u8 gPokenavRegionMapCityBannerFrames[] = INCBIN_U8("graphics/pokenav/region_map/city_banner_frames.bin");
const u8 gPokenavRegionMapCityBannerBlank[] = INCBIN_U8("graphics/pokenav/region_map/city_banner_blank.bin");
const u8 gPokenavCondition_Tilemap[] = INCBIN_U8("graphics/pokenav/condition/graph.bin.lz");

// text window frames
const u8 gTextWindowFrame1_Gfx[] = INCBIN_U8("graphics/text_window/1.4bpp");
const u16 gTextWindowFrame1_Pal[] = INCBIN_U16("graphics/text_window/1.gbapal");
const u8 gTextWindowFrame2_Gfx[] = INCBIN_U8("graphics/text_window/2.4bpp");
const u16 gTextWindowFrame2_Pal[] = INCBIN_U16("graphics/text_window/2.gbapal");
const u8 gTextWindowFrame3_Gfx[] = INCBIN_U8("graphics/text_window/3.4bpp");
const u16 gTextWindowFrame3_Pal[] = INCBIN_U16("graphics/text_window/3.gbapal");
const u8 gTextWindowFrame4_Gfx[] = INCBIN_U8("graphics/text_window/4.4bpp");
const u16 gTextWindowFrame4_Pal[] = INCBIN_U16("graphics/text_window/4.gbapal");
const u8 gTextWindowFrame5_Gfx[] = INCBIN_U8("graphics/text_window/5.4bpp");
const u16 gTextWindowFrame5_Pal[] = INCBIN_U16("graphics/text_window/5.gbapal");
const u8 gTextWindowFrame6_Gfx[] = INCBIN_U8("graphics/text_window/6.4bpp");
const u16 gTextWindowFrame6_Pal[] = INCBIN_U16("graphics/text_window/6.gbapal");
const u8 gTextWindowFrame7_Gfx[] = INCBIN_U8("graphics/text_window/7.4bpp");
const u16 gTextWindowFrame7_Pal[] = INCBIN_U16("graphics/text_window/7.gbapal");
const u8 gTextWindowFrame8_Gfx[] = INCBIN_U8("graphics/text_window/8.4bpp");
const u16 gTextWindowFrame8_Pal[] = INCBIN_U16("graphics/text_window/8.gbapal");
const u8 gTextWindowFrame9_Gfx[] = INCBIN_U8("graphics/text_window/9.4bpp");
const u16 gTextWindowFrame9_Pal[] = INCBIN_U16("graphics/text_window/9.gbapal");
const u8 gTextWindowFrame10_Gfx[] = INCBIN_U8("graphics/text_window/10.4bpp");
const u16 gTextWindowFrame10_Pal[] = INCBIN_U16("graphics/text_window/10.gbapal");
const u8 gTextWindowFrame11_Gfx[] = INCBIN_U8("graphics/text_window/11.4bpp");
const u16 gTextWindowFrame11_Pal[] = INCBIN_U16("graphics/text_window/11.gbapal");
const u8 gTextWindowFrame12_Gfx[] = INCBIN_U8("graphics/text_window/12.4bpp");
const u16 gTextWindowFrame12_Pal[] = INCBIN_U16("graphics/text_window/12.gbapal");
const u8 gTextWindowFrame13_Gfx[] = INCBIN_U8("graphics/text_window/13.4bpp");
const u16 gTextWindowFrame13_Pal[] = INCBIN_U16("graphics/text_window/13.gbapal");
const u8 gTextWindowFrame14_Gfx[] = INCBIN_U8("graphics/text_window/14.4bpp");
const u16 gTextWindowFrame14_Pal[] = INCBIN_U16("graphics/text_window/14.gbapal");
const u8 gTextWindowFrame15_Gfx[] = INCBIN_U8("graphics/text_window/15.4bpp");
const u16 gTextWindowFrame15_Pal[] = INCBIN_U16("graphics/text_window/15.gbapal");
const u8 gTextWindowFrame16_Gfx[] = INCBIN_U8("graphics/text_window/16.4bpp");
const u16 gTextWindowFrame16_Pal[] = INCBIN_U16("graphics/text_window/16.gbapal");
const u8 gTextWindowFrame17_Gfx[] = INCBIN_U8("graphics/text_window/17.4bpp");
const u16 gTextWindowFrame17_Pal[] = INCBIN_U16("graphics/text_window/17.gbapal");
const u8 gTextWindowFrame18_Gfx[] = INCBIN_U8("graphics/text_window/18.4bpp");
const u16 gTextWindowFrame18_Pal[] = INCBIN_U16("graphics/text_window/18.gbapal");
const u8 gTextWindowFrame19_Gfx[] = INCBIN_U8("graphics/text_window/19.4bpp");
const u16 gTextWindowFrame19_Pal[] = INCBIN_U16("graphics/text_window/19.gbapal");
const u8 gTextWindowFrame20_Gfx[] = INCBIN_U8("graphics/text_window/20.4bpp");
const u16 gTextWindowFrame20_Pal[] = INCBIN_U16("graphics/text_window/20.gbapal");

const u8 gPokedexListUnderlay_Tilemap[] = INCBIN_U8("graphics/pokedex/list_underlay.bin.lz");

const u8 gIntroCopyright_Gfx[] = INCBIN_U8("graphics/intro/copyright.4bpp.lz");
const u16 gIntroCopyright_Pal[] = INCBIN_U16("graphics/intro/copyright.gbapal");
const u8 gIntroCopyright_Tilemap[] = INCBIN_U8("graphics/intro/copyright.bin");

const u8 gEasyChatRsOutlineCursor_Gfx[] = INCBIN_U8("graphics/easy_chat/rs_outline_cursor.4bpp");
const u8 gTitleScreenPressStartGfx[] = INCBIN_U8("graphics/title_screen/press_start.4bpp.lz");
const u8 gTitleScreenPressStartPal[] = INCBIN_U8("graphics/title_screen/logo_shine.gbapal");

#ifdef SAPPHIRE
const u8 gTitleScreenPokemonLogoGfx[] = INCBIN_U8("graphics/title_screen/pokemon_logo_sapphire.8bpp.lz");
const u8 gTitleScreenRubySapphireVersionGfx[] = INCBIN_U8("graphics/title_screen/sapphireversion.8bpp.lz");
const u16 gTitleScreenBgPalettes[] = INCBIN_U16("graphics/title_screen/logo_sapphire.gbapal");
// for some reason, the sapphire palette is different even though the visible colors for the logo and version are not
#else
const u8 gTitleScreenPokemonLogoGfx[] = INCBIN_U8("graphics/title_screen/pokemon_logo_ruby.8bpp.lz");
const u8 gTitleScreenRubySapphireVersionGfx[] = INCBIN_U8("graphics/title_screen/rubyversion.8bpp.lz");
const u16 gTitleScreenBgPalettes[] = INCBIN_U16("graphics/title_screen/logo_ruby.gbapal");
#endif // SAPPHIRE

const u8 gTitleScreenPokemonLogoTilemap[] = INCBIN_U8("graphics/title_screen/pokemon_logo.bin.lz");
const u16 gPokenavListArrowPalette[] = INCBIN_U16("graphics/pokenav/list_arrows_up_down.gbapal");
const u16 gPokenavListArrowAltPalette[] = INCBIN_U16("graphics/pokenav/list_arrows_alt.gbapal");
const u8 gPokenavListRightArrowGfx[] = INCBIN_U8("graphics/pokenav/list_arrows_right.4bpp");
const u16 gPokenavTrainerEyesListBg_Pal[] = INCBIN_U16("graphics/pokenav/trainers_eyes/list_bg.gbapal");
const u8 gPokenavConditionSearch2_Gfx[] = INCBIN_U8("graphics/pokenav/condition_search2.4bpp.lz");
const u8 gPokenavRibbonView_Pal[] = INCBIN_U8("graphics/pokenav/ribbons/view.pal.bin");
const u8 gPokenavRibbonView_Gfx[] = INCBIN_U8("graphics/pokenav/ribbons/view.4bpp.lz");
const u8 gPokenavRibbonsSummaryTilemap[] = INCBIN_U8("graphics/pokenav/ribbons/summary_bg.bin.lz");
const u8 gPokenavList_Tilemap[] = INCBIN_U8("graphics/pokenav/list.bin.lz");
const u8 gPokenavListInfo_Tilemap[] = INCBIN_U8("graphics/pokenav/list_info.bin");
const u8 gPokenavTrainerEyesListBottom_Tilemap[] = INCBIN_U8("graphics/pokenav/trainers_eyes/list_bottom.bin");
const u8 gPokenavTrainerEyesListTop_Tilemap[] = INCBIN_U8("graphics/pokenav/trainers_eyes/list_top.bin");
const u8 gConditionGraphData_Tilemap[] = INCBIN_U8("graphics/pokenav/condition/graph_data.bin.lz");
const u8 gPokenavMonInfoHeaderTilemap[] = INCBIN_U8("graphics/pokenav/mon_info_header.bin.lz");
const u8 gPokenavMonInfoHeader_Gfx[] = INCBIN_U8("graphics/pokenav/mon_info_header.4bpp");
const u8 gDialogueFrame_Gfx[] = INCBIN_U8("graphics/text_window/message_box.4bpp");

const u16 gTradeMenu_Pal[] = INCBIN_U16("graphics/trade/menu.gbapal", "graphics/trade/stripes.gbapal", "graphics/trade/stripes.gbapal");

const u16 gTradeCursor_Pal[] = INCBIN_U16("graphics/trade/cursor.gbapal");
const u8 gTradeMenu_Gfx[] = INCBIN_U8("graphics/trade/menu.4bpp");
const u8 gTradeMenu_Tilemap[] = INCBIN_U8("graphics/trade/menu.bin");
const u8 gTradeMonBoxTilemap[] = INCBIN_U8("graphics/trade/menu_mon_box.bin");
const u8 gTradeCursor_Gfx[] = INCBIN_U8("graphics/trade/cursor.4bpp");

const u8 gTradeUnused_Tilemap[] = INCBIN_U8("graphics/trade/unused.bin");
const u8 gCreditsCopyrightEnd_Gfx[] = INCBIN_U8("graphics/credits/the_end_copyright.4bpp.lz");
const u8 gDecoration10000_Gfx[] = INCBIN_U8("graphics/misc/decoration_10000.4bpp");

#ifdef ENGLISH
    #if REVISION >= 1
        const u8 gFont3LatinGlyphs[] = INCBIN_U8("graphics/fonts/font3_lat_rev1.4bpp");
    #else
        const u8 gFont3LatinGlyphs[] = INCBIN_U8("graphics/fonts/font3_lat.4bpp");
    #endif
#else
    const u8 gFont3LatinGlyphs[] = INCBIN_U8("graphics/fonts/font3_lat.4bpp");
#endif

#ifdef ENGLISH
    #if REVISION >= 1
    const u8 gFont4LatinGlyphs[] = INCBIN_U8("graphics/fonts/font4_lat_rev1.4bpp");
    #else
    const u8 gFont4LatinGlyphs[] = INCBIN_U8("graphics/fonts/font4_lat.4bpp");
    #endif
#else
    const u8 gFont4LatinGlyphs[] = INCBIN_U8("graphics/fonts/font4_lat.4bpp.bin");
#endif // ENGLISH

const u8 gFont3JapaneseGlyphs[] = INCBIN_U8("graphics/fonts/font3_jpn.4bpp");
const u8 gFont4JapaneseGlyphs[] = INCBIN_U8("graphics/fonts/font4_jpn.4bpp");

#ifdef GERMAN
const u8 gSummaryScreenTextTiles[] = INCBIN_U8("graphics/summary_screen/text.4bpp");
const u8 gPokedexNoBall_Gfx[] = INCBIN_U8("graphics/pokedex/noball.4bpp.lz");
const u8 sIntroDropsLogo_Gfx[] = INCBIN_U8("graphics/intro/scene_1/drops_logo.4bpp.lz");

const u8 gUsePokeblockUpDown_Gfx[] = INCBIN_U8("graphics/pokeblock/use_screen/updown.4bpp");
const u16 gUsePokeblockUpDown_Pal[] = INCBIN_U16("graphics/pokeblock/use_screen/updown.gbapal");

const u16 gRouletteShadow_Pal[] = INCBIN_U16("graphics/roulette/shadow.gbapal");
const u16 gRouletteBall_Pal[] = INCBIN_U16("graphics/roulette/ball.gbapal");
const u16 gRouletteBallCounter_Pal[] = INCBIN_U16("graphics/roulette/ball_counter.gbapal");
const u16 gRouletteCursor_Pal[] = INCBIN_U16("graphics/roulette/cursor.gbapal");
const u16 gRouletteCredit_Pal[] = INCBIN_U16("graphics/roulette/credit.gbapal");
const u16 gRouletteShroomish_Pal[] = INCBIN_U16("graphics/roulette/shroomish.gbapal");
const u16 gRouletteTaillow_Pal[] = INCBIN_U16("graphics/roulette/tailow.gbapal");
const u16 gRouletteGridIcons_Pal[] = INCBIN_U16("graphics/roulette/grid_icons.gbapal");
const u16 gRouletteWynaut_Pal[] = INCBIN_U16("graphics/roulette/wynaut.gbapal");
const u16 gRouletteAzurill_Pal[] = INCBIN_U16("graphics/roulette/azurill.gbapal");
const u16 gRouletteSkitty_Pal[] = INCBIN_U16("graphics/roulette/skitty.gbapal");
const u16 gRouletteMakuhita_Pal[] = INCBIN_U16("graphics/roulette/makuhita.gbapal");
const u16 gRouletteUnused1_Pal[] = INCBIN_U16("graphics/roulette/unused_1.gbapal");
const u16 gRouletteUnused2_Pal[] = INCBIN_U16("graphics/roulette/unused_2.gbapal");
const u16 gRouletteUnused3_Pal[] = INCBIN_U16("graphics/roulette/unused_3.gbapal");
const u16 gRouletteUnused4_Pal[] = INCBIN_U16("graphics/roulette/unused_4.gbapal");

const u16 gTilesetPalettes_Shop[][16] = 
{
    INCBIN_U16("data/tilesets/secondary/shop/palettes/00.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/01.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/02.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/03.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/04.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/05.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/06.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/07.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/08.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/09.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/10.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/11.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/12.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/13.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/14.gbapal"),
    INCBIN_U16("data/tilesets/secondary/shop/palettes/15.gbapal"),
};

const u8 gTilesetTiles_Shop[] = INCBIN_U8("data/tilesets/secondary/shop/tiles.4bpp.lz");
const u16 gCryMeter_Pal[] = INCBIN_U16("graphics/pokedex/cry_meter.gbapal");
const u8 gCryMeter_Gfx[] = INCBIN_U8("graphics/pokedex/cry_meter.4bpp.lz");
const u8 gCryMeter_Tilemap[] = INCBIN_U8("graphics/pokedex/cry_meter_map.bin");
#endif // GERMAN
