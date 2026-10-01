#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

// Ruby/Sapphire's save data is Flash 128K, which is 32 save sectors.
#define SECTORS_COUNT 32 // defined in agb_flash but not in a header

// If the sector's signature field is not this value, it is invalid or empty.
#define SECTOR_SIGNATURE 0x08012025

// Special sector id used when an operation addresses a complete save slot.
#define FULL_SAVE_SLOT 0xFFFF

#define SAVE_STATUS_EMPTY 0
#define SAVE_STATUS_OK 1
#define SAVE_STATUS_CORRUPT 2
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
u8 LoadGameSave(u8 saveType);
void DebugFixSaveChecksums(void);

#endif // GUARD_SAVE_H
