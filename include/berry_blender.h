#ifndef GUARD_BERRY_BLENDER_H
#define GUARD_BERRY_BLENDER_H

// Command-word indices; Ruby stores receive words before player IDs.
#define BLENDER_COMM_INPUT_STATE  0
#define BLENDER_COMM_RESP         1
#define BLENDER_COMM_SCORE        2
#define BLENDER_COMM_STOP_TYPE    2
#define BLENDER_COMM_PLAYER_ID    3

void debug_sub_80524BC(void);
void StartBlender(void);

extern u8 gInGameOpponentsNo;
void DoBerryBlending(void);
void ShowBerryBlenderRecordWindow(void);
void SetPlayerBerryData(u8 playerId, u16 itemId);

#endif //GUARD_BERRY_BLENDER_H
