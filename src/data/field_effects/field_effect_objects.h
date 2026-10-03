//

//

#ifndef POKERUBY_FIELD_EFFECT_OBJECTS_H
#define POKERUBY_FIELD_EFFECT_OBJECTS_H

const struct SpritePalette gSpritePalette_GeneralFieldEffect0 = {gFieldEffectObjectPalette0, 0x1004};

const struct SpritePalette gSpritePalette_GeneralFieldEffect1 = {gFieldEffectObjectPalette1, 0x1005};

static const union AnimCmd sAnim_Shadow[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Shadow[] = {
    sAnim_Shadow
};

static const struct SpriteFrameImage sPicTable_ShadowSmall[] = {
    overworld_frame(gFieldEffectObjectPic_ShadowSmall, 1, 1, 0)
};

static const struct SpriteFrameImage sPicTable_ShadowMedium[] = {
    overworld_frame(gFieldEffectObjectPic_ShadowMedium, 2, 1, 0)
};

static const struct SpriteFrameImage sPicTable_ShadowLarge[] = {
    overworld_frame(gFieldEffectObjectPic_ShadowLarge, 4, 1, 0)
};

static const struct SpriteFrameImage sPicTable_ShadowExtraLarge[] = {
    overworld_frame(gFieldEffectObjectPic_ShadowExtraLarge, 8, 4, 0)
};

const struct SpriteTemplate gFieldEffectObjectTemplate_ShadowSmall = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_8x8, sAnimTable_Shadow, sPicTable_ShadowSmall, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

const struct SpriteTemplate gFieldEffectObjectTemplate_ShadowMedium = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_16x8, sAnimTable_Shadow, sPicTable_ShadowMedium, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

const struct SpriteTemplate gFieldEffectObjectTemplate_ShadowLarge = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_32x8, sAnimTable_Shadow, sPicTable_ShadowLarge, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

const struct SpriteTemplate gFieldEffectObjectTemplate_ShadowExtraLarge = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_64x32, sAnimTable_Shadow, sPicTable_ShadowExtraLarge, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

static const struct SpriteFrameImage sPicTable_TallGrass[] = {
    overworld_frame(gFieldEffectObjectPic_TallGrass, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_TallGrass, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_TallGrass, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_TallGrass, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_TallGrass, 2, 2, 4)
};

static const union AnimCmd sAnim_TallGrass[] = {
    ANIMCMD_FRAME(1, 10),
    ANIMCMD_FRAME(2, 10),
    ANIMCMD_FRAME(3, 10),
    ANIMCMD_FRAME(4, 10),
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_TallGrass[] = {
    sAnim_TallGrass
};

const struct SpriteTemplate gFieldEffectObjectTemplate_TallGrass = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_TallGrass, sPicTable_TallGrass, gDummySpriteAffineAnimTable, UpdateTallGrassFieldEffect};

static const struct SpriteFrameImage sPicTable_Ripple[] = {
    overworld_frame(gFieldEffectObjectPic_Ripple, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_Ripple, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_Ripple, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_Ripple, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_Ripple, 2, 2, 4)
};

static const union AnimCmd sAnim_Ripple[] = {
    ANIMCMD_FRAME(0, 12),
    ANIMCMD_FRAME(1, 9),
    ANIMCMD_FRAME(2, 9),
    ANIMCMD_FRAME(3, 9),
    ANIMCMD_FRAME(0, 9),
    ANIMCMD_FRAME(1, 9),
    ANIMCMD_FRAME(2, 11),
    ANIMCMD_FRAME(4, 11),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Ripple[] = {
    sAnim_Ripple
};

const struct SpriteTemplate gFieldEffectObjectTemplate_Ripple = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_Ripple, sPicTable_Ripple, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

static const struct SpriteFrameImage sPicTable_Ash[] = {
    overworld_frame(gFieldEffectObjectPic_Ash, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_Ash, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_Ash, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_Ash, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_Ash, 2, 2, 4)
};

static const union AnimCmd sAnim_Ash[] = {
    ANIMCMD_FRAME(0, 12),
    ANIMCMD_FRAME(1, 12),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 12),
    ANIMCMD_FRAME(4, 12),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Ash[] = {
    sAnim_Ash
};

const struct SpriteTemplate gFieldEffectObjectTemplate_Ash = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_Ash, sPicTable_Ash, gDummySpriteAffineAnimTable, UpdateAshFieldEffect};

static const struct SpriteFrameImage sPicTable_SurfBlob[] = {
    overworld_frame(gFieldEffectObjectPic_SurfBlob, 4, 4, 0),
    overworld_frame(gFieldEffectObjectPic_SurfBlob, 4, 4, 1),
    overworld_frame(gFieldEffectObjectPic_SurfBlob, 4, 4, 2)
};

static const union AnimCmd sSurfBlobAnim_FaceSouth[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sSurfBlobAnim_FaceNorth[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sSurfBlobAnim_FaceWest[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sSurfBlobAnim_FaceEast[] = {
    ANIMCMD_FRAME(2, 1, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_SurfBlob[] = {
    sSurfBlobAnim_FaceSouth,
    sSurfBlobAnim_FaceNorth,
    sSurfBlobAnim_FaceWest,
    sSurfBlobAnim_FaceEast
};

const struct SpriteTemplate gFieldEffectObjectTemplate_SurfBlob = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_32x32, sAnimTable_SurfBlob, sPicTable_SurfBlob, gDummySpriteAffineAnimTable, UpdateSurfBlobFieldEffect};

static const struct SpriteFrameImage sPicTable_Arrow[] = {
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 4),
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 5),
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 6),
    overworld_frame(gFieldEffectObjectPic_Arrow, 2, 2, 7)
};

static const union AnimCmd sArrowAnim_South[] = {
    ANIMCMD_FRAME(3, 32),
    ANIMCMD_FRAME(7, 32),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sArrowAnim_North[] = {
    ANIMCMD_FRAME(0, 32),
    ANIMCMD_FRAME(4, 32),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sArrowAnim_West[] = {
    ANIMCMD_FRAME(1, 32),
    ANIMCMD_FRAME(5, 32),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sArrowAnim_East[] = {
    ANIMCMD_FRAME(2, 32),
    ANIMCMD_FRAME(6, 32),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_Arrow[] = {
    sArrowAnim_South,
    sArrowAnim_North,
    sArrowAnim_West,
    sArrowAnim_East
};

const struct SpriteTemplate gFieldEffectObjectTemplate_Arrow = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_16x16, sAnimTable_Arrow, sPicTable_Arrow, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteFrameImage sPicTable_GroundImpactDust[] = {
    overworld_frame(gFieldEffectObjectPic_GroundImpactDust, 2, 1, 0),
    overworld_frame(gFieldEffectObjectPic_GroundImpactDust, 2, 1, 1),
    overworld_frame(gFieldEffectObjectPic_GroundImpactDust, 2, 1, 2)
};

static const union AnimCmd sAnim_GroundImpactDust[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_GroundImpactDust[] = {
    sAnim_GroundImpactDust
};

const struct SpriteTemplate gFieldEffectObjectTemplate_GroundImpactDust = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x8, sAnimTable_GroundImpactDust, sPicTable_GroundImpactDust, gDummySpriteAffineAnimTable, UpdateJumpLandingFieldEffect};

static const struct SpriteFrameImage sPicTable_JumpTallGrass[] = {
    overworld_frame(gFieldEffectObjectPic_JumpTallGrass, 2, 1, 0),
    overworld_frame(gFieldEffectObjectPic_JumpTallGrass, 2, 1, 1),
    overworld_frame(gFieldEffectObjectPic_JumpTallGrass, 2, 1, 2),
    overworld_frame(gFieldEffectObjectPic_JumpTallGrass, 2, 1, 3)
};

static const union AnimCmd sAnim_JumpTallGrass[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_JumpTallGrass[] = {
    sAnim_JumpTallGrass
};

const struct SpriteTemplate gFieldEffectObjectTemplate_JumpTallGrass = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x8, sAnimTable_JumpTallGrass, sPicTable_JumpTallGrass, gDummySpriteAffineAnimTable, UpdateJumpLandingFieldEffect};

static const struct SpriteFrameImage sPicTable_SandFootprints[] = {
    overworld_frame(gFieldEffectObjectPic_SandFootprints, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_SandFootprints, 2, 2, 1)
};

static const union AnimCmd sSandFootprintsAnim_South[] = {
    ANIMCMD_FRAME(0, 1, .vFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sSandFootprintsAnim_North[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sSandFootprintsAnim_West[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd sSandFootprintsAnim_East[] = {
    ANIMCMD_FRAME(1, 1, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_SandFootprints[] = {
    sSandFootprintsAnim_South,
    sSandFootprintsAnim_South,
    sSandFootprintsAnim_North,
    sSandFootprintsAnim_West,
    sSandFootprintsAnim_East
};

const struct SpriteTemplate gFieldEffectObjectTemplate_SandFootprints = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x16, sAnimTable_SandFootprints, sPicTable_SandFootprints, gDummySpriteAffineAnimTable, UpdateFootprintsTireTracksFieldEffect};

static const struct SpriteFrameImage sPicTable_DeepSandFootprints[] = {
    overworld_frame(gFieldEffectObjectPic_DeepSandFootprints, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_DeepSandFootprints, 2, 2, 1)
};

static const union AnimCmd sDeepSandFootprintsAnim_South[] = {
    ANIMCMD_FRAME(0, 1, .vFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sDeepSandFootprintsAnim_North[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sDeepSandFootprintsAnim_West[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd sDeepSandFootprintsAnim_East[] = {
    ANIMCMD_FRAME(1, 1, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_DeepSandFootprints[] = {
    sDeepSandFootprintsAnim_South,
    sDeepSandFootprintsAnim_South,
    sDeepSandFootprintsAnim_North,
    sDeepSandFootprintsAnim_West,
    sDeepSandFootprintsAnim_East
};

const struct SpriteTemplate gFieldEffectObjectTemplate_DeepSandFootprints = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x16, sAnimTable_DeepSandFootprints, sPicTable_DeepSandFootprints, gDummySpriteAffineAnimTable, UpdateFootprintsTireTracksFieldEffect};

static const struct SpriteFrameImage sPicTable_BikeTireTracks[] = {
    overworld_frame(gFieldEffectObjectPic_BikeTireTracks, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_BikeTireTracks, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_BikeTireTracks, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_BikeTireTracks, 2, 2, 3)
};

static const union AnimCmd sBikeTireTracksAnim_South[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

static const union AnimCmd sBikeTireTracksAnim_North[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

static const union AnimCmd sBikeTireTracksAnim_West[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd sBikeTireTracksAnim_East[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd sBikeTireTracksAnim_SECornerTurn[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sBikeTireTracksAnim_SWCornerTurn[] = {
    ANIMCMD_FRAME(0, 1, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sBikeTireTracksAnim_NWCornerTurn[] = {
    ANIMCMD_FRAME(3, 1, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sBikeTireTracksAnim_NECornerTurn[] = {
    ANIMCMD_FRAME(3, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_BikeTireTracks[] = {
    sBikeTireTracksAnim_South,
    sBikeTireTracksAnim_South,
    sBikeTireTracksAnim_North,
    sBikeTireTracksAnim_West,
    sBikeTireTracksAnim_East,
    sBikeTireTracksAnim_SECornerTurn,
    sBikeTireTracksAnim_SWCornerTurn,
    sBikeTireTracksAnim_NWCornerTurn,
    sBikeTireTracksAnim_NECornerTurn
};

const struct SpriteTemplate gFieldEffectObjectTemplate_BikeTireTracks = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x16, sAnimTable_BikeTireTracks, sPicTable_BikeTireTracks, gDummySpriteAffineAnimTable, UpdateFootprintsTireTracksFieldEffect};

static const struct SpriteFrameImage sPicTable_JumpBigSplash[] = {
    overworld_frame(gFieldEffectObjectPic_JumpBigSplash, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_JumpBigSplash, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_JumpBigSplash, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_JumpBigSplash, 2, 2, 3)
};

static const union AnimCmd sAnim_JumpBigSplash[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_JumpBigSplash[] = {
    sAnim_JumpBigSplash
};

const struct SpriteTemplate gFieldEffectObjectTemplate_JumpBigSplash = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x16, sAnimTable_JumpBigSplash, sPicTable_JumpBigSplash, gDummySpriteAffineAnimTable, UpdateJumpLandingFieldEffect};

static const struct SpriteFrameImage sPicTable_Splash[] = {
    overworld_frame(gFieldEffectObjectPic_Splash, 2, 1, 0),
    overworld_frame(gFieldEffectObjectPic_Splash, 2, 1, 1)
};

static const union AnimCmd sAnim_Splash_0[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Splash_1[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_Splash[] = {
    sAnim_Splash_0,
    sAnim_Splash_1
};

const struct SpriteTemplate gFieldEffectObjectTemplate_Splash = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x8, sAnimTable_Splash, sPicTable_Splash, gDummySpriteAffineAnimTable, UpdateSplashFieldEffect};

static const struct SpriteFrameImage sPicTable_JumpSmallSplash[] = {
    overworld_frame(gFieldEffectObjectPic_JumpSmallSplash, 2, 1, 0),
    overworld_frame(gFieldEffectObjectPic_JumpSmallSplash, 2, 1, 1),
    overworld_frame(gFieldEffectObjectPic_JumpSmallSplash, 2, 1, 2)
};

static const union AnimCmd sAnim_JumpSmallSplash[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_JumpSmallSplash[] = {
    sAnim_JumpSmallSplash
};

const struct SpriteTemplate gFieldEffectObjectTemplate_JumpSmallSplash = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x8, sAnimTable_JumpSmallSplash, sPicTable_JumpSmallSplash, gDummySpriteAffineAnimTable, UpdateJumpLandingFieldEffect};

static const struct SpriteFrameImage sPicTable_LongGrass[] = {
    overworld_frame(gFieldEffectObjectPic_LongGrass, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_LongGrass, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_LongGrass, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_LongGrass, 2, 2, 3)
};

static const union AnimCmd sAnim_LongGrass[] = {
    ANIMCMD_FRAME(1, 3),
    ANIMCMD_FRAME(2, 3),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_LongGrass[] = {
    sAnim_LongGrass
};

const struct SpriteTemplate gFieldEffectObjectTemplate_LongGrass = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_LongGrass, sPicTable_LongGrass, gDummySpriteAffineAnimTable, UpdateLongGrassFieldEffect};

static const struct SpriteFrameImage sPicTable_JumpLongGrass[] = {
    overworld_frame(gFieldEffectObjectPic_JumpLongGrass, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_JumpLongGrass, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_JumpLongGrass, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_JumpLongGrass, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_JumpLongGrass, 2, 2, 4),
    overworld_frame(gFieldEffectObjectPic_JumpLongGrass, 2, 2, 6)
};

static const union AnimCmd sAnim_JumpLongGrass[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_JumpLongGrass[] = {
    sAnim_JumpLongGrass
};

const struct SpriteTemplate gFieldEffectObjectTemplate_JumpLongGrass = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_JumpLongGrass, sPicTable_JumpLongGrass, gDummySpriteAffineAnimTable, UpdateJumpLandingFieldEffect};

static const struct SpriteFrameImage sPicTable_UnusedGrass[] = {
    overworld_frame(gFieldEffectObjectPic_JumpLongGrass, 2, 2, 6),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 4),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 5),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 6),
    overworld_frame(gFieldEffectObjectPic_Unknown17, 2, 2, 7)
};

static const union AnimCmd sAnim_UnusedGrass[] = {
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(5, 4),
    ANIMCMD_FRAME(6, 4),
    ANIMCMD_FRAME(7, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_JUMP(7)
};

static const union AnimCmd *const sAnimTable_UnusedGrass[] = {
    sAnim_UnusedGrass
};

const struct SpriteTemplate gFieldEffectObjectTemplate_UnusedGrass = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_UnusedGrass, sPicTable_UnusedGrass, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

static const struct SpriteFrameImage sPicTable_UnusedGrass2[] = {
    overworld_frame(gFieldEffectObjectPic_UnusedGrass2, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_UnusedGrass2, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_UnusedGrass2, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_UnusedGrass2, 2, 2, 3)
};

static const union AnimCmd sAnim_UnusedGrass2[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_UnusedGrass2[] = {
    sAnim_UnusedGrass2
};

const struct SpriteTemplate gFieldEffectObjectTemplate_UnusedGrass2 = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_UnusedGrass2, sPicTable_UnusedGrass2, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

static const struct SpriteFrameImage sPicTable_UnusedSand[] = {
    overworld_frame(gFieldEffectObjectPic_UnusedSand, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_UnusedSand, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_UnusedSand, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_UnusedSand, 2, 2, 3)
};

static const union AnimCmd sAnim_UnusedSand[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_UnusedSand[] = {
    sAnim_UnusedSand
};

const struct SpriteTemplate gFieldEffectObjectTemplate_UnusedSand = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x16, sAnimTable_UnusedSand, sPicTable_UnusedSand, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

static const struct SpriteFrameImage sPicTable_SandPile[] = {
    overworld_frame(gFieldEffectObjectPic_SandPile, 2, 1, 0),
    overworld_frame(gFieldEffectObjectPic_SandPile, 2, 1, 1),
    overworld_frame(gFieldEffectObjectPic_SandPile, 2, 1, 2)
};

static const union AnimCmd sAnim_SandPile[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_SandPile[] = {
    sAnim_SandPile
};

const struct SpriteTemplate gFieldEffectObjectTemplate_SandPile = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x8, sAnimTable_SandPile, sPicTable_SandPile, gDummySpriteAffineAnimTable, UpdateSandPileFieldEffect};

static const struct SpriteFrameImage sPicTable_WaterSurfacing[] = {
    overworld_frame(gFieldEffectObjectPic_WaterSurfacing, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_WaterSurfacing, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_WaterSurfacing, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_WaterSurfacing, 2, 2, 3)
};

static const union AnimCmd sAnim_WaterSurfacing[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_WaterSurfacing[] = {
    sAnim_WaterSurfacing
};

const struct SpriteTemplate gFieldEffectObjectTemplate_WaterSurfacing = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x16, sAnimTable_WaterSurfacing, sPicTable_WaterSurfacing, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

static const union AffineAnimCmd sAffineAnim_ReflectionDistortion_0[] = {
    AFFINEANIMCMD_FRAME(0xFF00, 0x100, -128, 0),
    AFFINEANIMCMD_FRAME( 1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_FRAME(-1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_FRAME(-1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_FRAME( 1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_JUMP(1)
};

static const union AffineAnimCmd sAffineAnim_ReflectionDistortion_1[] = {
    AFFINEANIMCMD_FRAME(0x100, 0x100, -128, 0),
    AFFINEANIMCMD_FRAME(-1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_FRAME( 1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_FRAME( 1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_FRAME(-1, 0, 0, 4),
    AFFINEANIMCMD_FRAME( 0, 0, 0, 8),
    AFFINEANIMCMD_JUMP(1)
};

static const union AffineAnimCmd *const sAffineAnims_ReflectionDistortion[] = {
    sAffineAnim_ReflectionDistortion_0,
    sAffineAnim_ReflectionDistortion_1
};

const struct SpriteTemplate gFieldEffectObjectTemplate_ReflectionDistortion = {0x0, 0xFFFF, &gDummyOamData, gDummySpriteAnimTable, NULL, sAffineAnims_ReflectionDistortion, SpriteCallbackDummy};

static const struct SpriteFrameImage sPicTable_Sparkle[] = {
    overworld_frame(gFieldEffectObjectPic_Sparkle, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_Sparkle, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_Sparkle, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_Sparkle, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_Sparkle, 2, 2, 4),
    overworld_frame(gFieldEffectObjectPic_Sparkle, 2, 2, 5)
};

static const union AnimCmd sAnim_Sparkle[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_LOOP(0),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(5, 4),
    ANIMCMD_LOOP(3),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Sparkle[] = {
    sAnim_Sparkle
};

const struct SpriteTemplate gFieldEffectObjectTemplate_Sparkle = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_16x16, sAnimTable_Sparkle, sPicTable_Sparkle, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

static const struct SpriteFrameImage sPicTable_TreeDisguise[] = {
    overworld_frame(gFieldEffectObjectPic_TreeDisguise, 2, 4, 0),
    overworld_frame(gFieldEffectObjectPic_TreeDisguise, 2, 4, 1),
    overworld_frame(gFieldEffectObjectPic_TreeDisguise, 2, 4, 2),
    overworld_frame(gFieldEffectObjectPic_TreeDisguise, 2, 4, 3),
    overworld_frame(gFieldEffectObjectPic_TreeDisguise, 2, 4, 4),
    overworld_frame(gFieldEffectObjectPic_TreeDisguise, 2, 4, 5),
    overworld_frame(gFieldEffectObjectPic_TreeDisguise, 2, 4, 6)
};

static const union AnimCmd sAnim_TreeDisguise[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_END
};

static const union AnimCmd sAnim_TreeDisguiseReveal[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(5, 4),
    ANIMCMD_FRAME(6, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_TreeDisguise[] = {
    sAnim_TreeDisguise,
    sAnim_TreeDisguiseReveal
};

const struct SpriteTemplate gFieldEffectObjectTemplate_TreeDisguise = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_16x32, sAnimTable_TreeDisguise, sPicTable_TreeDisguise, gDummySpriteAffineAnimTable, UpdateDisguiseFieldEffect};

static const struct SpriteFrameImage sPicTable_MountainDisguise[] = {
    overworld_frame(gFieldEffectObjectPic_MountainDisguise, 2, 4, 0),
    overworld_frame(gFieldEffectObjectPic_MountainDisguise, 2, 4, 1),
    overworld_frame(gFieldEffectObjectPic_MountainDisguise, 2, 4, 2),
    overworld_frame(gFieldEffectObjectPic_MountainDisguise, 2, 4, 3),
    overworld_frame(gFieldEffectObjectPic_MountainDisguise, 2, 4, 4),
    overworld_frame(gFieldEffectObjectPic_MountainDisguise, 2, 4, 5),
    overworld_frame(gFieldEffectObjectPic_MountainDisguise, 2, 4, 6)
};

static const union AnimCmd sAnim_MountainDisguise[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MountainDisguiseReveal[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(5, 4),
    ANIMCMD_FRAME(6, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_MountainDisguise[] = {
    sAnim_MountainDisguise,
    sAnim_MountainDisguiseReveal
};

const struct SpriteTemplate gFieldEffectObjectTemplate_MountainDisguise = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_16x32, sAnimTable_MountainDisguise, sPicTable_MountainDisguise, gDummySpriteAffineAnimTable, UpdateDisguiseFieldEffect};

static const struct SpriteFrameImage sPicTable_SandDisguisePlaceholder[] = {
    overworld_frame(gFieldEffectObjectPic_SandDisguisePlaceholder, 2, 4, 0),
    overworld_frame(gFieldEffectObjectPic_SandDisguisePlaceholder, 2, 4, 1),
    overworld_frame(gFieldEffectObjectPic_SandDisguisePlaceholder, 2, 4, 2),
    overworld_frame(gFieldEffectObjectPic_SandDisguisePlaceholder, 2, 4, 3),
    overworld_frame(gFieldEffectObjectPic_SandDisguisePlaceholder, 2, 4, 4),
    overworld_frame(gFieldEffectObjectPic_SandDisguisePlaceholder, 2, 4, 5),
    overworld_frame(gFieldEffectObjectPic_SandDisguisePlaceholder, 2, 4, 6)
};

const struct SpriteTemplate gFieldEffectObjectTemplate_SandDisguisePlaceholder = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_16x32, sAnimTable_TreeDisguise, sPicTable_SandDisguisePlaceholder, gDummySpriteAffineAnimTable, UpdateDisguiseFieldEffect};

static const struct SpriteFrameImage sPicTable_Bird[] = {
    overworld_frame(gFieldEffectObjectPic_Bird, 4, 4, 0)
};

static const union AnimCmd sAnim_Bird[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Bird[] = {
    sAnim_Bird
};

const struct SpriteTemplate gFieldEffectObjectTemplate_Bird = {0xFFFF, 0xFFFF, &gObjectEventBaseOam_32x32, sAnimTable_Bird, sPicTable_Bird, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteFrameImage sPicTable_ShortGrass[] = {
    overworld_frame(gFieldEffectObjectPic_ShortGrass, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_ShortGrass, 2, 2, 1)
};

static const union AnimCmd sAnim_ShortGrass[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_ShortGrass[] = {
    sAnim_ShortGrass
};

const struct SpriteTemplate gFieldEffectObjectTemplate_ShortGrass = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_ShortGrass, sPicTable_ShortGrass, gDummySpriteAffineAnimTable, UpdateShortGrassFieldEffect};

static const struct SpriteFrameImage sPicTable_HotSpringsWater[] = {
    overworld_frame(gFieldEffectObjectPic_HotSpringsWater, 2, 2, 0)
};

static const union AnimCmd sAnim_HotSpringsWater[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_HotSpringsWater[] = {
    sAnim_HotSpringsWater
};

const struct SpriteTemplate gFieldEffectObjectTemplate_HotSpringsWater = {0xFFFF, 0x1005, &gObjectEventBaseOam_16x16, sAnimTable_HotSpringsWater, sPicTable_HotSpringsWater, gDummySpriteAffineAnimTable, UpdateHotSpringsWaterFieldEffect};

static const struct SpriteFrameImage sPicTable_AshPuff[] = {
    overworld_frame(gFieldEffectObjectPic_AshPuff, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_AshPuff, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_AshPuff, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_AshPuff, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_AshPuff, 2, 2, 4)
};

static const union AnimCmd sAnim_AshPuff[] = {
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_AshPuff[] = {
    sAnim_AshPuff
};

const struct SpriteTemplate gFieldEffectObjectTemplate_AshPuff = {0xFFFF, 0x100D, &gObjectEventBaseOam_16x16, sAnimTable_AshPuff, sPicTable_AshPuff, gDummySpriteAffineAnimTable, sub_808788C};

const struct SpritePalette gSpritePalette_Ash = {gFieldEffectPal_Ash, 0x100D};

static const struct SpriteFrameImage sPicTable_AshLaunch[] = {
    overworld_frame(gFieldEffectObjectPic_AshLaunch, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_AshLaunch, 2, 2, 1),
    overworld_frame(gFieldEffectObjectPic_AshLaunch, 2, 2, 2),
    overworld_frame(gFieldEffectObjectPic_AshLaunch, 2, 2, 3),
    overworld_frame(gFieldEffectObjectPic_AshLaunch, 2, 2, 4)
};

static const union AnimCmd sAnim_AshLaunch[] = {
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_AshLaunch[] = {
    sAnim_AshLaunch
};

const struct SpriteTemplate gFieldEffectObjectTemplate_AshLaunch = {0xFFFF, 0x100D, &gObjectEventBaseOam_16x16, sAnimTable_AshLaunch, sPicTable_AshLaunch, gDummySpriteAffineAnimTable, sub_8087638};

static const struct SpriteFrameImage sPicTable_Bubbles[] = {
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 0),
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 1),
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 2),
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 3),
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 4),
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 5),
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 6),
    overworld_frame(gFieldEffectObjectPic_Bubbles, 2, 4, 7)
};

static const union AnimCmd sAnim_Bubbles[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_FRAME(5, 4),
    ANIMCMD_FRAME(6, 4),
    ANIMCMD_FRAME(7, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Bubbles[] = {
    sAnim_Bubbles
};

const struct SpriteTemplate gFieldEffectObjectTemplate_Bubbles = {0xFFFF, 0x1004, &gObjectEventBaseOam_16x32, sAnimTable_Bubbles, sPicTable_Bubbles, gDummySpriteAffineAnimTable, UpdateBubblesFieldEffect};

static const struct SpriteFrameImage sPicTable_SmallSparkle[] = {
    overworld_frame(gFieldEffectObjectPic_SmallSparkle, 2, 2, 0),
    overworld_frame(gFieldEffectObjectPic_SmallSparkle, 2, 2, 1)
};

static const union AnimCmd sAnim_SmallSparkle[] = {
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_SmallSparkle[] = {
    sAnim_SmallSparkle
};

const struct SpriteTemplate gFieldEffectObjectTemplate_SmallSparkle = {0xFFFF, 0x100F, &gObjectEventBaseOam_16x16, sAnimTable_SmallSparkle, sPicTable_SmallSparkle, gDummySpriteAffineAnimTable, UpdateSparkleFieldEffect};

const struct SpritePalette gSpritePalette_SmallSparkle = {gFieldEffectPal_SmallSparkle, 0x100F};

#endif //POKERUBY_FIELD_EFFECT_OBJECTS_H
