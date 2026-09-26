#ifndef GUARD_FIELD_SPECIAL_SCENE_H
#define GUARD_FIELD_SPECIAL_SCENE_H

s16 GetTruckCameraBobbingY(int time);
s16 GetTruckBoxYMovement(int time);
void Task_Truck1(u8 taskId);
void Task_Truck2(u8 taskId);
void Task_Truck3(u8 taskId);
void Task_HandleTruckSequence(u8 taskId);
void ExecuteTruckSequence(void);
void EndTruckSequence(u8);
void FieldCB_ShowPortholeView(void);

#endif // GUARD_FIELD_SPECIAL_SCENE_H
