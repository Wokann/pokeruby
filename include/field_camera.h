#ifndef GUARD_FIELD_CAMERA_H
#define GUARD_FIELD_CAMERA_H

struct CameraObject
{
    void (*callback)(struct CameraObject *);
    u32 spriteId;
    s32 movementSpeedX;
    s32 movementSpeedY;
    s32 curMovementOffsetX;
    s32 curMovementOffsetY;
};

extern struct CameraObject gFieldCamera;
extern u16 gTotalCameraPixelOffsetX;
extern u16 gTotalCameraPixelOffsetY;

void move_tilemap_camera_to_upper_left_corner(void);
void sub_8057A58(void);
void GetCameraOffsetWithPan(u16 *x, u16 *y);
void DrawWholeMapView(void);
void CurrentMapDrawMetatileAt(int x, int y);
void DrawDoorMetatileAt(int x, int y, u16 *tiles);
void ResetCameraUpdateInfo(void);
u32 InitCameraUpdateCallback(u8 trackedSpriteId);
void CameraUpdate(void);
void SetCameraPanningCallback(void (*callback)(void));
void SetCameraPanning(s16 horizontal, s16 vertical);
void InstallCameraPanAheadCallback(void);
void UpdateCameraPanning(void);

#endif // GUARD_FIELD_CAMERA_H
