//

//

#ifndef POKERUBY_FIELD_EFFECT_OBJECTS_H
#define POKERUBY_FIELD_EFFECT_OBJECTS_H

const struct SpritePalette gSpritePalette_GeneralFieldEffect0 = {gFieldEffectObjectPalette0, FLDEFF_PAL_TAG_GENERAL_0};

const struct SpritePalette gFieldEffectObjectPaletteInfo1 = {gFieldEffectObjectPalette1, 0x1005};

const union AnimCmd gFieldEffectAnim_8374534[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_Shadow[] = {
    gFieldEffectAnim_8374534
};

const struct SpriteFrameImage gFieldEffectPicTable_ShadowSmall[] = {
    overworld_frame(gFieldEffectPic_ShadowSmall, 1, 1, 0)
};

const struct SpriteFrameImage gFieldEffectPicTable_ShadowMedium[] = {
    overworld_frame(gFieldEffectPic_ShadowMedium, 2, 1, 0)
};

const struct SpriteFrameImage gFieldEffectPicTable_ShadowLarge[] = {
    overworld_frame(gFieldEffectPic_ShadowLarge, 4, 1, 0)
};

const struct SpriteFrameImage gFieldEffectPicTable_ShadowExtraLarge[] = {
    overworld_frame(gFieldEffectPic_ShadowExtraLarge, 8, 4, 0)
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_ShadowSmall = {0xFFFF, 0xFFFF, &gFieldOamData_8x8, gFieldEffectAnimTable_Shadow, gFieldEffectPicTable_ShadowSmall, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

const struct SpriteTemplate gFieldEffectSpriteTemplate_ShadowMedium = {0xFFFF, 0xFFFF, &gFieldOamData_16x8, gFieldEffectAnimTable_Shadow, gFieldEffectPicTable_ShadowMedium, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

const struct SpriteTemplate gFieldEffectSpriteTemplate_ShadowLarge = {0xFFFF, 0xFFFF, &gFieldOamData_32x8, gFieldEffectAnimTable_Shadow, gFieldEffectPicTable_ShadowLarge, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

const struct SpriteTemplate gFieldEffectSpriteTemplate_ShadowExtraLarge = {0xFFFF, 0xFFFF, &gFieldOamData_64x32, gFieldEffectAnimTable_Shadow, gFieldEffectPicTable_ShadowExtraLarge, gDummySpriteAffineAnimTable, UpdateShadowFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_TallGrass[] = {
    overworld_frame(gFieldEffectPic_TallGrass, 2, 2, 0),
    overworld_frame(gFieldEffectPic_TallGrass, 2, 2, 1),
    overworld_frame(gFieldEffectPic_TallGrass, 2, 2, 2),
    overworld_frame(gFieldEffectPic_TallGrass, 2, 2, 3),
    overworld_frame(gFieldEffectPic_TallGrass, 2, 2, 4)
};

const union AnimCmd gFieldEffectAnim_83745E8[] = {
    ANIMCMD_FRAME(1, 10),
    ANIMCMD_FRAME(2, 10),
    ANIMCMD_FRAME(3, 10),
    ANIMCMD_FRAME(4, 10),
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_TallGrass[] = {
    gFieldEffectAnim_83745E8
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_TallGrass = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_TallGrass, gFieldEffectPicTable_TallGrass, gDummySpriteAffineAnimTable, UpdateTallGrassFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_Ripple[] = {
    overworld_frame(gFieldEffectPic_Ripple, 2, 2, 0),
    overworld_frame(gFieldEffectPic_Ripple, 2, 2, 1),
    overworld_frame(gFieldEffectPic_Ripple, 2, 2, 2),
    overworld_frame(gFieldEffectPic_Ripple, 2, 2, 3),
    overworld_frame(gFieldEffectPic_Ripple, 2, 2, 4)
};

const union AnimCmd gFieldEffectAnim_8374644[] = {
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

const union AnimCmd *const gFieldEffectAnimTable_Ripple[] = {
    gFieldEffectAnim_8374644
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Ripple = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_Ripple, gFieldEffectPicTable_Ripple, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

const struct SpriteFrameImage gFieldEffectPicTable_Ash[] = {
    overworld_frame(gFieldEffectPic_Ash, 2, 2, 0),
    overworld_frame(gFieldEffectPic_Ash, 2, 2, 1),
    overworld_frame(gFieldEffectPic_Ash, 2, 2, 2),
    overworld_frame(gFieldEffectPic_Ash, 2, 2, 3),
    overworld_frame(gFieldEffectPic_Ash, 2, 2, 4)
};

const union AnimCmd gFieldEffectAnim_83746AC[] = {
    ANIMCMD_FRAME(0, 12),
    ANIMCMD_FRAME(1, 12),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 12),
    ANIMCMD_FRAME(4, 12),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_Ash[] = {
    gFieldEffectAnim_83746AC
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Ash = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_Ash, gFieldEffectPicTable_Ash, gDummySpriteAffineAnimTable, UpdateAshFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_SurfBlob[] = {
    overworld_frame(gFieldEffectPic_SurfBlob, 4, 4, 0),
    overworld_frame(gFieldEffectPic_SurfBlob, 4, 4, 1),
    overworld_frame(gFieldEffectPic_SurfBlob, 4, 4, 2)
};

const union AnimCmd gFieldEffectAnim_83746F8[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gFieldEffectAnim_8374700[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gFieldEffectAnim_8374708[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gFieldEffectAnim_8374710[] = {
    ANIMCMD_FRAME(2, 1, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gFieldEffectAnimTable_SurfBlob[] = {
    gFieldEffectAnim_83746F8,
    gFieldEffectAnim_8374700,
    gFieldEffectAnim_8374708,
    gFieldEffectAnim_8374710
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_SurfBlob = {0xFFFF, 0xFFFF, &gFieldOamData_32x32, gFieldEffectAnimTable_SurfBlob, gFieldEffectPicTable_SurfBlob, gDummySpriteAffineAnimTable, UpdateSurfBlobFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_Arrow[] = {
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 0),
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 1),
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 2),
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 3),
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 4),
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 5),
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 6),
    overworld_frame(gFieldEffectPic_Arrow, 2, 2, 7)
};

const union AnimCmd gFieldEffectAnim_8374780[] = {
    ANIMCMD_FRAME(3, 32),
    ANIMCMD_FRAME(7, 32),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gFieldEffectAnim_837478C[] = {
    ANIMCMD_FRAME(0, 32),
    ANIMCMD_FRAME(4, 32),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gFieldEffectAnim_8374798[] = {
    ANIMCMD_FRAME(1, 32),
    ANIMCMD_FRAME(5, 32),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gFieldEffectAnim_83747A4[] = {
    ANIMCMD_FRAME(2, 32),
    ANIMCMD_FRAME(6, 32),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gFieldEffectAnimTable_Arrow[] = {
    gFieldEffectAnim_8374780,
    gFieldEffectAnim_837478C,
    gFieldEffectAnim_8374798,
    gFieldEffectAnim_83747A4
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Arrow = {0xFFFF, 0xFFFF, &gFieldOamData_16x16, gFieldEffectAnimTable_Arrow, gFieldEffectPicTable_Arrow, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct SpriteFrameImage gFieldEffectPicTable_GroundImpactDust[] = {
    overworld_frame(gFieldEffectPic_GroundImpactDust, 2, 1, 0),
    overworld_frame(gFieldEffectPic_GroundImpactDust, 2, 1, 1),
    overworld_frame(gFieldEffectPic_GroundImpactDust, 2, 1, 2)
};

const union AnimCmd gFieldEffectAnim_GroundImpactDust[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_GroundImpactDust[] = {
    gFieldEffectAnim_GroundImpactDust
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_GroundImpactDust = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x8, gFieldEffectAnimTable_GroundImpactDust, gFieldEffectPicTable_GroundImpactDust, gDummySpriteAffineAnimTable, UpdateJumpImpactEffect};

const struct SpriteFrameImage gFieldEffectPicTable_JumpTallGrass[] = {
    overworld_frame(gFieldEffectPic_JumpTallGrass, 2, 1, 0),
    overworld_frame(gFieldEffectPic_JumpTallGrass, 2, 1, 1),
    overworld_frame(gFieldEffectPic_JumpTallGrass, 2, 1, 2),
    overworld_frame(gFieldEffectPic_JumpTallGrass, 2, 1, 3)
};

const union AnimCmd gFieldEffectAnim_837483C[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_JumpTallGrass[] = {
    gFieldEffectAnim_837483C
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_JumpTallGrass = {0xFFFF, 0x1005, &gFieldOamData_16x8, gFieldEffectAnimTable_JumpTallGrass, gFieldEffectPicTable_JumpTallGrass, gDummySpriteAffineAnimTable, UpdateJumpImpactEffect};

const struct SpriteFrameImage gFieldEffectPicTable_SandFootprints[] = {
    overworld_frame(gFieldEffectPic_SandFootprints, 2, 2, 0),
    overworld_frame(gFieldEffectPic_SandFootprints, 2, 2, 1)
};

const union AnimCmd gFieldEffectAnim_837487C[] = {
    ANIMCMD_FRAME(0, 1, .vFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374884[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_837488C[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374894[] = {
    ANIMCMD_FRAME(1, 1, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_SandFootprints[] = {
    gFieldEffectAnim_837487C,
    gFieldEffectAnim_837487C,
    gFieldEffectAnim_8374884,
    gFieldEffectAnim_837488C,
    gFieldEffectAnim_8374894
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_SandFootprints = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x16, gFieldEffectAnimTable_SandFootprints, gFieldEffectPicTable_SandFootprints, gDummySpriteAffineAnimTable, UpdateFootprintsTireTracksFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_DeepSandFootprints[] = {
    overworld_frame(gFieldEffectPic_DeepSandFootprints, 2, 2, 0),
    overworld_frame(gFieldEffectPic_DeepSandFootprints, 2, 2, 1)
};

const union AnimCmd gFieldEffectAnim_83748D8[] = {
    ANIMCMD_FRAME(0, 1, .vFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_83748E0[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_83748E8[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_83748F0[] = {
    ANIMCMD_FRAME(1, 1, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_DeepSandFootprints[] = {
    gFieldEffectAnim_83748D8,
    gFieldEffectAnim_83748D8,
    gFieldEffectAnim_83748E0,
    gFieldEffectAnim_83748E8,
    gFieldEffectAnim_83748F0
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_DeepSandFootprints = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x16, gFieldEffectAnimTable_DeepSandFootprints, gFieldEffectPicTable_DeepSandFootprints, gDummySpriteAffineAnimTable, UpdateFootprintsTireTracksFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_BikeTireTracks[] = {
    overworld_frame(gFieldEffectPic_BikeTireTracks, 2, 2, 0),
    overworld_frame(gFieldEffectPic_BikeTireTracks, 2, 2, 1),
    overworld_frame(gFieldEffectPic_BikeTireTracks, 2, 2, 2),
    overworld_frame(gFieldEffectPic_BikeTireTracks, 2, 2, 3)
};

const union AnimCmd gFieldEffectAnim_8374944[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_837494C[] = {
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374954[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_837495C[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374964[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_837496C[] = {
    ANIMCMD_FRAME(0, 1, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374974[] = {
    ANIMCMD_FRAME(3, 1, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_837497C[] = {
    ANIMCMD_FRAME(3, 1),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_BikeTireTracks[] = {
    gFieldEffectAnim_8374944,
    gFieldEffectAnim_8374944,
    gFieldEffectAnim_837494C,
    gFieldEffectAnim_8374954,
    gFieldEffectAnim_837495C,
    gFieldEffectAnim_8374964,
    gFieldEffectAnim_837496C,
    gFieldEffectAnim_8374974,
    gFieldEffectAnim_837497C
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_BikeTireTracks = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x16, gFieldEffectAnimTable_BikeTireTracks, gFieldEffectPicTable_BikeTireTracks, gDummySpriteAffineAnimTable, UpdateFootprintsTireTracksFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_JumpBigSplash[] = {
    overworld_frame(gFieldEffectPic_JumpBigSplash, 2, 2, 0),
    overworld_frame(gFieldEffectPic_JumpBigSplash, 2, 2, 1),
    overworld_frame(gFieldEffectPic_JumpBigSplash, 2, 2, 2),
    overworld_frame(gFieldEffectPic_JumpBigSplash, 2, 2, 3)
};

const union AnimCmd gFieldEffectAnim_83749E0[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_JumpBigSplash[] = {
    gFieldEffectAnim_83749E0
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_JumpBigSplash = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x16, gFieldEffectAnimTable_JumpBigSplash, gFieldEffectPicTable_JumpBigSplash, gDummySpriteAffineAnimTable, UpdateJumpImpactEffect};

const struct SpriteFrameImage gFieldEffectPicTable_Splash[] = {
    overworld_frame(gFieldEffectPic_Splash, 2, 1, 0),
    overworld_frame(gFieldEffectPic_Splash, 2, 1, 1)
};

const union AnimCmd gFieldEffectAnim_8374A20[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374A2C[] = {
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

const union AnimCmd *const gFieldEffectAnimTable_Splash[] = {
    gFieldEffectAnim_8374A20,
    gFieldEffectAnim_8374A2C
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Splash = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x8, gFieldEffectAnimTable_Splash, gFieldEffectPicTable_Splash, gDummySpriteAffineAnimTable, UpdateSplashFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_JumpSmallSplash[] = {
    overworld_frame(gFieldEffectPic_JumpSmallSplash, 2, 1, 0),
    overworld_frame(gFieldEffectPic_JumpSmallSplash, 2, 1, 1),
    overworld_frame(gFieldEffectPic_JumpSmallSplash, 2, 1, 2)
};

const union AnimCmd gFieldEffectAnim_8374A88[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_JumpSmallSplash[] = {
    gFieldEffectAnim_8374A88
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_JumpSmallSplash = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x8, gFieldEffectAnimTable_JumpSmallSplash, gFieldEffectPicTable_JumpSmallSplash, gDummySpriteAffineAnimTable, UpdateJumpImpactEffect};

const struct SpriteFrameImage gFieldEffectPicTable_LongGrass[] = {
    overworld_frame(gFieldEffectPic_LongGrass, 2, 2, 0),
    overworld_frame(gFieldEffectPic_LongGrass, 2, 2, 1),
    overworld_frame(gFieldEffectPic_LongGrass, 2, 2, 2),
    overworld_frame(gFieldEffectPic_LongGrass, 2, 2, 3)
};

const union AnimCmd gFieldEffectAnim_8374AD4[] = {
    ANIMCMD_FRAME(1, 3),
    ANIMCMD_FRAME(2, 3),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_LongGrass[] = {
    gFieldEffectAnim_8374AD4
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_LongGrass = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_LongGrass, gFieldEffectPicTable_LongGrass, gDummySpriteAffineAnimTable, UpdateLongGrassFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_JumpLongGrass[] = {
    overworld_frame(gFieldEffectPic_JumpLongGrass, 2, 2, 0),
    overworld_frame(gFieldEffectPic_JumpLongGrass, 2, 2, 1),
    overworld_frame(gFieldEffectPic_JumpLongGrass, 2, 2, 2),
    overworld_frame(gFieldEffectPic_JumpLongGrass, 2, 2, 3),
    overworld_frame(gFieldEffectPic_JumpLongGrass, 2, 2, 4),
    overworld_frame(gFieldEffectPic_JumpLongGrass, 2, 2, 6)
};

const union AnimCmd gFieldEffectAnim_8374B40[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_JumpLongGrass[] = {
    gFieldEffectAnim_8374B40
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_JumpLongGrass = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_JumpLongGrass, gFieldEffectPicTable_JumpLongGrass, gDummySpriteAffineAnimTable, UpdateJumpImpactEffect};

const struct SpriteFrameImage gFieldEffectPicTable_UnusedGrass[] = {
    overworld_frame(gFieldEffectPic_JumpLongGrass, 2, 2, 6),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 0),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 1),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 2),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 3),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 4),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 5),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 6),
    overworld_frame(gFieldEffectPic_Unknown17, 2, 2, 7)
};

const union AnimCmd gFieldEffectAnim_UnusedGrass[] = {
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

const union AnimCmd *const gFieldEffectAnimTable_UnusedGrass[] = {
    gFieldEffectAnim_UnusedGrass
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_UnusedGrass = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_UnusedGrass, gFieldEffectPicTable_UnusedGrass, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

const struct SpriteFrameImage gFieldEffectPicTable_UnusedGrass2[] = {
    overworld_frame(gFieldEffectPic_UnusedGrass2, 2, 2, 0),
    overworld_frame(gFieldEffectPic_UnusedGrass2, 2, 2, 1),
    overworld_frame(gFieldEffectPic_UnusedGrass2, 2, 2, 2),
    overworld_frame(gFieldEffectPic_UnusedGrass2, 2, 2, 3)
};

const union AnimCmd gFieldEffectAnim_UnusedGrass2[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gFieldEffectAnimTable_UnusedGrass2[] = {
    gFieldEffectAnim_UnusedGrass2
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_UnusedGrass2 = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_UnusedGrass2, gFieldEffectPicTable_UnusedGrass2, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

const struct SpriteFrameImage gFieldEffectPicTable_UnusedSand[] = {
    overworld_frame(gFieldEffectPic_UnusedSand, 2, 2, 0),
    overworld_frame(gFieldEffectPic_UnusedSand, 2, 2, 1),
    overworld_frame(gFieldEffectPic_UnusedSand, 2, 2, 2),
    overworld_frame(gFieldEffectPic_UnusedSand, 2, 2, 3)
};

const union AnimCmd gFieldEffectAnim_UnusedSand[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gFieldEffectAnimTable_UnusedSand[] = {
    gFieldEffectAnim_UnusedSand
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_UnusedSand = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x16, gFieldEffectAnimTable_UnusedSand, gFieldEffectPicTable_UnusedSand, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

const struct SpriteFrameImage gFieldEffectPicTable_SandPile[] = {
    overworld_frame(gFieldEffectPic_SandPile, 2, 1, 0),
    overworld_frame(gFieldEffectPic_SandPile, 2, 1, 1),
    overworld_frame(gFieldEffectPic_SandPile, 2, 1, 2)
};

const union AnimCmd gFieldEffectAnim_8374CC4[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_SandPile[] = {
    gFieldEffectAnim_8374CC4
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_SandPile = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x8, gFieldEffectAnimTable_SandPile, gFieldEffectPicTable_SandPile, gDummySpriteAffineAnimTable, UpdateSandPileFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_WaterSurfacing[] = {
    overworld_frame(gFieldEffectPic_WaterSurfacing, 2, 2, 0),
    overworld_frame(gFieldEffectPic_WaterSurfacing, 2, 2, 1),
    overworld_frame(gFieldEffectPic_WaterSurfacing, 2, 2, 2),
    overworld_frame(gFieldEffectPic_WaterSurfacing, 2, 2, 3)
};

const union AnimCmd gFieldEffectAnim_WaterSurfacing[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gFieldEffectAnimTable_WaterSurfacing[] = {
    gFieldEffectAnim_WaterSurfacing
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_WaterSurfacing = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x16, gFieldEffectAnimTable_WaterSurfacing, gFieldEffectPicTable_WaterSurfacing, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

const union AffineAnimCmd gFieldEffectAffineAnim_WavyReflection[] = {
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

const union AffineAnimCmd gFieldEffectAffineAnim_WavyReflectionFlipped[] = {
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

const union AffineAnimCmd *const gFieldEffectAffineAnimTable_Reflection[] = {
    gFieldEffectAffineAnim_WavyReflection,
    gFieldEffectAffineAnim_WavyReflectionFlipped
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Reflection = {0x0, 0xFFFF, &gDummyOamData, gDummySpriteAnimTable, NULL, gFieldEffectAffineAnimTable_Reflection, SpriteCallbackDummy};

const struct SpriteFrameImage gFieldEffectPicTable_BerryTreeGrowthSparkle[] = {
    overworld_frame(gFieldEffectPic_BerryTreeGrowthSparkle, 2, 2, 0),
    overworld_frame(gFieldEffectPic_BerryTreeGrowthSparkle, 2, 2, 1),
    overworld_frame(gFieldEffectPic_BerryTreeGrowthSparkle, 2, 2, 2),
    overworld_frame(gFieldEffectPic_BerryTreeGrowthSparkle, 2, 2, 3),
    overworld_frame(gFieldEffectPic_BerryTreeGrowthSparkle, 2, 2, 4),
    overworld_frame(gFieldEffectPic_BerryTreeGrowthSparkle, 2, 2, 5)
};

const union AnimCmd gFieldEffectAnim_8374E38[] = {
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

const union AnimCmd *const gFieldEffectAnimTable_BerryTreeGrowthSparkle[] = {
    gFieldEffectAnim_8374E38
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_BerryTreeGrowthSparkle = {0xFFFF, 0xFFFF, &gFieldOamData_16x16, gFieldEffectAnimTable_BerryTreeGrowthSparkle, gFieldEffectPicTable_BerryTreeGrowthSparkle, gDummySpriteAffineAnimTable, WaitFieldEffectSpriteAnim};

const struct SpriteFrameImage gFieldEffectPicTable_TreeDisguise[] = {
    overworld_frame(gFieldEffectPic_TreeDisguise, 2, 4, 0),
    overworld_frame(gFieldEffectPic_TreeDisguise, 2, 4, 1),
    overworld_frame(gFieldEffectPic_TreeDisguise, 2, 4, 2),
    overworld_frame(gFieldEffectPic_TreeDisguise, 2, 4, 3),
    overworld_frame(gFieldEffectPic_TreeDisguise, 2, 4, 4),
    overworld_frame(gFieldEffectPic_TreeDisguise, 2, 4, 5),
    overworld_frame(gFieldEffectPic_TreeDisguise, 2, 4, 6)
};

const union AnimCmd gFieldEffectAnim_8374EE0[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374EE8[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(5, 4),
    ANIMCMD_FRAME(6, 4),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_TreeDisguise[] = {
    gFieldEffectAnim_8374EE0,
    gFieldEffectAnim_8374EE8
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_TreeDisguise = {0xFFFF, 0xFFFF, &gFieldOamData_16x32, gFieldEffectAnimTable_TreeDisguise, gFieldEffectPicTable_TreeDisguise, gDummySpriteAffineAnimTable, UpdateDisguiseFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_MountainDisguise[] = {
    overworld_frame(gFieldEffectPic_MountainDisguise, 2, 4, 0),
    overworld_frame(gFieldEffectPic_MountainDisguise, 2, 4, 1),
    overworld_frame(gFieldEffectPic_MountainDisguise, 2, 4, 2),
    overworld_frame(gFieldEffectPic_MountainDisguise, 2, 4, 3),
    overworld_frame(gFieldEffectPic_MountainDisguise, 2, 4, 4),
    overworld_frame(gFieldEffectPic_MountainDisguise, 2, 4, 5),
    overworld_frame(gFieldEffectPic_MountainDisguise, 2, 4, 6)
};

const union AnimCmd gFieldEffectAnim_8374F60[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_END
};

const union AnimCmd gFieldEffectAnim_8374F68[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(5, 4),
    ANIMCMD_FRAME(6, 4),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_MountainDisguise[] = {
    gFieldEffectAnim_8374F60,
    gFieldEffectAnim_8374F68
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_MountainDisguise = {0xFFFF, 0xFFFF, &gFieldOamData_16x32, gFieldEffectAnimTable_MountainDisguise, gFieldEffectPicTable_MountainDisguise, gDummySpriteAffineAnimTable, UpdateDisguiseFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_SandDisguisePlaceholder[] = {
    overworld_frame(gFieldEffectPic_SandDisguisePlaceholder, 2, 4, 0),
    overworld_frame(gFieldEffectPic_SandDisguisePlaceholder, 2, 4, 1),
    overworld_frame(gFieldEffectPic_SandDisguisePlaceholder, 2, 4, 2),
    overworld_frame(gFieldEffectPic_SandDisguisePlaceholder, 2, 4, 3),
    overworld_frame(gFieldEffectPic_SandDisguisePlaceholder, 2, 4, 4),
    overworld_frame(gFieldEffectPic_SandDisguisePlaceholder, 2, 4, 5),
    overworld_frame(gFieldEffectPic_SandDisguisePlaceholder, 2, 4, 6)
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_SandDisguisePlaceholder = {0xFFFF, 0xFFFF, &gFieldOamData_16x32, gFieldEffectAnimTable_TreeDisguise, gFieldEffectPicTable_SandDisguisePlaceholder, gDummySpriteAffineAnimTable, UpdateDisguiseFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_Bird[] = {
    overworld_frame(gFieldEffectPic_Bird, 4, 4, 0)
};

const union AnimCmd gFieldEffectAnim_8375000[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_Bird[] = {
    gFieldEffectAnim_8375000
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Bird = {0xFFFF, 0xFFFF, &gFieldOamData_32x32, gFieldEffectAnimTable_Bird, gFieldEffectPicTable_Bird, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct SpriteFrameImage gFieldEffectPicTable_ShortGrass[] = {
    overworld_frame(gFieldEffectPic_ShortGrass, 2, 2, 0),
    overworld_frame(gFieldEffectPic_ShortGrass, 2, 2, 1)
};

const union AnimCmd gFieldEffectAnim_8375034[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_ShortGrass[] = {
    gFieldEffectAnim_8375034
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_ShortGrass = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_ShortGrass, gFieldEffectPicTable_ShortGrass, gDummySpriteAffineAnimTable, UpdateShortGrassFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_HotSpringsWater[] = {
    overworld_frame(gFieldEffectPic_HotSpringsWater, 2, 2, 0)
};

const union AnimCmd gFieldEffectAnim_8375064[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_HotSpringsWater[] = {
    gFieldEffectAnim_8375064
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_HotSpringsWater = {0xFFFF, 0x1005, &gFieldOamData_16x16, gFieldEffectAnimTable_HotSpringsWater, gFieldEffectPicTable_HotSpringsWater, gDummySpriteAffineAnimTable, UpdateHotSpringsWaterFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_AshPuff[] = {
    overworld_frame(gFieldEffectPic_AshPuff, 2, 2, 0),
    overworld_frame(gFieldEffectPic_AshPuff, 2, 2, 1),
    overworld_frame(gFieldEffectPic_AshPuff, 2, 2, 2),
    overworld_frame(gFieldEffectPic_AshPuff, 2, 2, 3),
    overworld_frame(gFieldEffectPic_AshPuff, 2, 2, 4)
};

const union AnimCmd gFieldEffectAnim_AshPuff[] = {
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_AshPuff[] = {
    gFieldEffectAnim_AshPuff
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_AshPuff = {0xFFFF, FLDEFF_PAL_TAG_ASH, &gFieldOamData_16x16, gFieldEffectAnimTable_AshPuff, gFieldEffectPicTable_AshPuff, gDummySpriteAffineAnimTable, SpriteCB_AshPuff};

const struct SpritePalette gSpritePalette_Ash = {gFieldEffectPal_Ash, FLDEFF_PAL_TAG_ASH};

const struct SpriteFrameImage gFieldEffectPicTable_AshLaunch[] = {
    overworld_frame(gFieldEffectPic_AshLaunch, 2, 2, 0),
    overworld_frame(gFieldEffectPic_AshLaunch, 2, 2, 1),
    overworld_frame(gFieldEffectPic_AshLaunch, 2, 2, 2),
    overworld_frame(gFieldEffectPic_AshLaunch, 2, 2, 3),
    overworld_frame(gFieldEffectPic_AshLaunch, 2, 2, 4)
};

const union AnimCmd gFieldEffectAnim_AshLaunch[] = {
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_AshLaunch[] = {
    gFieldEffectAnim_AshLaunch
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_AshLaunch = {0xFFFF, FLDEFF_PAL_TAG_ASH, &gFieldOamData_16x16, gFieldEffectAnimTable_AshLaunch, gFieldEffectPicTable_AshLaunch, gDummySpriteAffineAnimTable, SpriteCB_AshLaunch};

const struct SpriteFrameImage gFieldEffectPicTable_Bubbles[] = {
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 0),
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 1),
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 2),
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 3),
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 4),
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 5),
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 6),
    overworld_frame(gFieldEffectPic_Bubbles, 2, 4, 7)
};

const union AnimCmd gFieldEffectAnim_Bubbles[] = {
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

const union AnimCmd *const gFieldEffectAnimTable_Bubbles[] = {
    gFieldEffectAnim_Bubbles
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Bubbles = {0xFFFF, FLDEFF_PAL_TAG_GENERAL_0, &gFieldOamData_16x32, gFieldEffectAnimTable_Bubbles, gFieldEffectPicTable_Bubbles, gDummySpriteAffineAnimTable, UpdateBubblesFieldEffect};

const struct SpriteFrameImage gFieldEffectPicTable_Sparkle[] = {
    overworld_frame(gFieldEffectPic_Sparkle, 2, 2, 0),
    overworld_frame(gFieldEffectPic_Sparkle, 2, 2, 1)
};

const union AnimCmd gFieldEffectAnim_83751D8[] = {
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd *const gFieldEffectAnimTable_Sparkle[] = {
    gFieldEffectAnim_83751D8
};

const struct SpriteTemplate gFieldEffectSpriteTemplate_Sparkle = {0xFFFF, 0x100F, &gFieldOamData_16x16, gFieldEffectAnimTable_Sparkle, gFieldEffectPicTable_Sparkle, gDummySpriteAffineAnimTable, UpdateSparkleFieldEffect};

const struct SpritePalette gFieldEffectObjectPaletteInfo3 = {gFieldEffectObjectPalette3, 0x100F};

#endif //POKERUBY_FIELD_EFFECT_OBJECTS_H
