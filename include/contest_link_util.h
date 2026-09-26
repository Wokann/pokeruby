#ifndef GUARD_CONTEST_LINK_UTIL_H
#define GUARD_CONTEST_LINK_UTIL_H

void CB2_StartShowContestResults(void);
void BufferContestantTrainerName(void);
void BufferContestantMonNickname(void);
void StartContest(void);
void BufferContestantMonSpecies(void);
void ShowContestResults(void);
void ContestLinkTransfer(u8 category);
u8 LinkContest_GetLeaderIndex(u8 *a0);
void Contest_CopyAndConvertTrainerName_Intl(u8 *, const u8 *);
void Contest_CopyAndConvertNicknameI_Intl(u8 *, u8);

#endif // GUARD_CONTEST_LINK_UTIL_H
