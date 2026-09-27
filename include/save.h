#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

// Ruby/Sapphire's save data is Flash 128K, which is 32 save sectors.
#define NUM_SECTORS 32 // defined in agb_flash but not in a header

#define SAVE_STATUS_EMPTY 0
#define SAVE_STATUS_OK 1
#define SAVE_STATUS_NO_FLASH 4
#define SAVE_STATUS_ERROR 0xFF

extern u16 gSaveFileStatus;

void ClearSaveData(void);
void Save_ResetSaveCounters(void);

enum
{
    SAVE_NORMAL,
    SAVE_LINK,
    SAVE_EREADER,
    SAVE_HALL_OF_FAME,
    SAVE_OVERWRITE_DIFFERENT_FILE,
    SAVE_HALL_OF_FAME_ERASE_BEFORE, // unused
};

u8 HandleSavingData(u8 saveType);
u8 TrySavingData(u8 saveType);

u8 LinkFullSave_Init(void);
bool8 LinkFullSave_WriteSector(void);
u8 LinkFullSave_ReplaceLastSector(void);
u8 LinkFullSave_SetLastSectorSignature(void);
u8 WriteSaveBlock2(void);
bool8 WriteSaveBlock1Sector(void);
u8 LoadGameSave(u8 a1);
void sub_813B79C(void);

#endif // GUARD_SAVE_H
