#ifndef GUARD_CONTEST_LINK_H
#define GUARD_CONTEST_LINK_H

void Task_LinkContest_CommunicateMonsRS(u8 taskId);
void Task_LinkContest_CommunicateRngRS(u8 taskId);
void Task_LinkContest_CommunicateMonIdxs(u8 taskId);
void Task_LinkContest_CommunicateLeaderIdsRS(u8 taskId);
void Task_LinkContest_CommunicateRound1Points(u8 taskId);
void Task_LinkContest_CommunicateTurnOrder(u8 taskId);
void Task_LinkContest_CommunicateMoveSelections(u8 taskId);
void Task_LinkContest_CommunicateFinalStandings(u8 taskId);
void Task_LinkContest_CommunicateAppealsState(u8 taskId);
u8 GetStringLanguage(const u8 *string);
void Task_LinkContest_Init(u8 taskId);
void Task_LinkContest_CommunicateCategoryRS(u8 taskId);

#endif // GUARD_CONTEST_LINK_H
