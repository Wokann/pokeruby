#ifndef GUARD_FLDEFF_H
#define GUARD_FLDEFF_H

// cut
bool8 SetUpFieldMove_Cut(void);
bool8 FldEff_UseCutOnGrass(void);
bool8 FldEff_UseCutOnTree(void);
bool8 FldEff_CutGrass(void);

extern const struct SpritePalette gSpritePalette_CutGrass;
extern struct MapPosition gPlayerFacingPosition;

// escalator
u8 CreateEscalatorTask(bool16 goingUp);
void StartEscalator(bool8 goingUp);
void StopEscalator(void);
bool8 IsEscalatorMoving(void);

// flash
bool8 SetUpFieldMove_Flash(void);
bool8 GetMapPairFadeToType(u8 _fromType, u8 _toType);
bool8 GetMapPairFadeFromType(u8 _fromType, u8 _toType);

// sweet scent
bool8 SetUpFieldMove_SweetScent(void);
bool8 FldEff_SweetScent(void);

// rock smash
bool8 CheckObjectGraphicsInFrontOfPlayer(u8 graphicsId);
u8 CreateFieldMoveTask(void);

// Ruby debug field moves
void Debug_SetUpFieldMove_Cut(void);
void debug_sub_8120968(void);

#endif // GUARD_FLDEFF_H
