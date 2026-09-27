#ifndef GUARD_FIELD_CAMERA_H
#define GUARD_FIELD_CAMERA_H

struct FieldCamera
{
    void (*callback)(struct FieldCamera *);
    u32 trackedSpriteId;
    s32 movementSpeedX;
    s32 movementSpeedY;
    s32 curMovementOffsetX;
    s32 curMovementOffsetY;
};

extern struct Camera gCamera;

void ResetFieldCamera(void);
void FieldUpdateBgTilemapScroll(void);
void GetCameraOffsetWithPan(u16 *a, u16 *b);
void DrawWholeMapView(void);
void CurrentMapDrawMetatileAt(int a, int b);
void DrawDoorMetatileAt(int x, int y, u16 *arr);
void ResetCameraUpdateInfo(void);
u32 InitCameraUpdateCallback(u8 a);
void CameraUpdate(void);
void SetCameraPanningCallback(void (*a)(void));
void SetCameraPanning(s16 horizontal, s16 vertical);
void InstallCameraPanAheadCallback(void);
void UpdateCameraPanning(void);

#endif // GUARD_FIELD_CAMERA_H
