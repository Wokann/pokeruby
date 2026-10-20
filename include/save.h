#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

// Ruby/Sapphire's save data is Flash 128K, which is 32 save sectors.
#define SECTORS_COUNT 32 // defined in agb_flash but not in a header

#define SECTOR_SIGNATURE 0x08012025  // signature value to determine if a sector is in use
// Each 4 KiB flash sector contains 3968 bytes of actual data followed by a 128 byte footer
#define SECTOR_DATA_SIZE 3968
#define SECTOR_FOOTER_SIZE 128
#define SECTOR_ID_HOF_1 28
#define NUM_SECTORS_PER_SLOT 14  // Number of sectors occupied by a save slot
#define NUM_HOF_SECTORS 2

#define SAVE_STATUS_EMPTY 0
#define SAVE_STATUS_OK 1
#define SAVE_STATUS_CORRUPT  2
#define SAVE_STATUS_NO_FLASH 4
#define SAVE_STATUS_ERROR 0xFF

enum
{
    SAVE_NORMAL,
    SAVE_LINK,
    SAVE_EREADER,
    SAVE_HALL_OF_FAME,
    SAVE_OVERWRITE_DIFFERENT_FILE,
    SAVE_HALL_OF_FAME_ERASE_BEFORE, // unused
};

struct SaveSectorLocation
{
    u8 *data;
    u16 size;
};

struct SaveSector
{
    u8 data[0xFF4];
    u16 id;
    u16 checksum;
    u32 signature;
    u32 counter;
}; // size is 0x1000

#define gSaveDataBuffer                 (struct SaveSector *)(gSharedMem + 0x0)

extern u16 gLastWrittenSector;
extern u32 gLastSaveCounter;
extern u16 gLastKnownGoodSector;
extern u32 gDamagedSaveSectors;
extern u32 gSaveCounter;
extern u16 gIncrementalSectorId;
extern u16 gSaveFileStatus;

void ClearSaveData(void);
void Save_ResetSaveCounters(void);

u8 HandleSavingData(u8 saveType);
u8 TrySavingData(u8 saveType);

bool8 LinkFullSave_Init(void);
bool8 LinkFullSave_WriteSector(void);
bool8 LinkFullSave_ReplaceLastSector(void);
bool8 LinkFullSave_SetLastSectorSignature(void);
bool8 WriteSaveBlock2(void);
bool8 WriteSaveBlock1Sector(void);
u8 LoadGameSave(u8 saveType);
void sub_813B79C(void);

// save_failed_screen.c
void DoSaveFailedScreen(u8 saveType);

#endif // GUARD_SAVE_H
