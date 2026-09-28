#ifndef GUARD_MATSUDA_DEBUG_MENU_H
#define GUARD_MATSUDA_DEBUG_MENU_H

#define BIT(n) (1 << (n))

void PrintMatsudaDebugContestantNames(u8 contestantId);
void PrintMatsudaDebugContestRank(u8 contestRank);
void PrintMatsudaDebugContestantData(u8 contestantId);
u8 MatsudaDebugMenu_Contest(void);
u8 MatsudaDebugMenu_ContestResults(void);
u8 MatsudaDebugMenu_ContestComm(void);
u8 MatsudaDebugMenu_CommTest(void);
u8 MatsudaDebugMenu_SetHighScore(void);
u8 MatsudaDebugMenu_ResetHighScore(void);
u8 MatsudaDebugMenu_SetArtMuseumItems(void);
void SpriteCB_MatsudaDebugCursor(struct Sprite *sprite);
void SelectMatsudaDebugContestant0(struct Sprite *sprite, s8 delta);
void SelectMatsudaDebugContestant1(struct Sprite *sprite, s8 delta);
void SelectMatsudaDebugContestant2(struct Sprite *sprite, s8 delta);
void SelectMatsudaDebugContestant3(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestCategory(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestRank(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantCool(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantCute(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantBeauty(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantSmart(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantTough(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantSheen(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantMove1(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantMove2(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantMove3(struct Sprite *sprite, s8 delta);
void AdjustMatsudaDebugContestantMove4(struct Sprite *sprite, s8 delta);
void ReturnToFieldWithMatsudaDebugContestScores(struct Sprite *sprite, s8 delta);

#endif
