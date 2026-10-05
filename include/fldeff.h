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
void StartEscalator(u8 goingUp);
void StopEscalator(void);
bool8 IsEscalatorMoving(void);

// rock smash
bool8 CheckObjectGraphicsInFrontOfPlayer(u8 graphicsId);
u8 CreateFieldMoveTask(void);

// Ruby debug field moves
void Debug_SetUpFieldMove_Cut(void);
void debug_sub_8120968(void);

#endif // GUARD_FLDEFF_H
