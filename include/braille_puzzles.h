#ifndef GUARD_BRAILLE_PUZZLES_H
#define GUARD_BRAILLE_PUZZLES_H

bool8 ShouldDoBrailleStrengthEffect(void);
bool8 ShouldDoBrailleFlyEffect(void);
void DoBrailleStrengthEffect(void);
void UseFlyAncientTomb_Callback(void);
void UseFlyAncientTomb_Finish(void);
void Task_BrailleWait(u8 taskId);
bool32 BrailleWait_CheckButtonPress(void);
bool8 ShouldDoBrailleDigEffect(void);
void DoBrailleDigEffect(void);
void DoBrailleFlyEffect(void);

#endif // GUARD_BRAILLE_PUZZLES_H
